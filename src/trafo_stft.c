#include "trafo_stft.h"


void stft_init(StftStruct *stft_struct, int nfft, int win_length, int hop_length, Padding* padding) {
    stft_struct->nfft = nfft;
    stft_struct->win  = win_length;
    stft_struct->hop  = hop_length;
    printf("STFT init: nfft=%d, win=%d, hop=%d\n", stft_struct->nfft, stft_struct->win, stft_struct->hop);


    stft_struct->stft_in  = (float*) calloc(nfft, sizeof(float));
    stft_struct->stft_out = (fftwf_complex*) fftwf_malloc(sizeof(fftwf_complex) * (nfft/2 + 1));

    stft_struct->wnd = (float*)calloc(nfft, sizeof(float));
    compute_hanning_window(nfft, stft_struct->wnd);

    stft_struct->plan = fftwf_plan_dft_r2c_1d(nfft, (float*)stft_struct->stft_in, stft_struct->stft_out, FFTW_ESTIMATE);
    stft_struct->padding = padding;
}

fftwf_complex* stft_compute(StftStruct* stft_struct, Signal *channel_singal, int num_bins, int do_pad) {
    Padding* padding = stft_struct->padding;

    float* signal  = NULL;
    int num_frames = 0;

    if (do_pad) {
        apply_padding(channel_singal->data, channel_singal->num_samples, padding);
        signal     = padding->padded_signal;
        num_frames = padding->padded_num_frames;
    } else {
        signal     = channel_singal->data;
        num_frames = padding->unpadded_num_frames;
    }
    fftwf_complex* stft_result = (fftwf_complex*) fftwf_malloc(sizeof(fftwf_complex) * num_frames * num_bins);
    stft_inner(stft_struct, signal, &stft_result, num_frames, num_bins);
    return stft_result; 
}

void stft_inner(StftStruct* stft_struct, float* signal, fftwf_complex** stft_result, int num_frames, int num_bins) {
    // process frames
    for (int m = 0; m < num_frames; ++m) {
        // apply Hanning window
        for (int n = 0; n < stft_struct->nfft; ++n) {
            stft_struct->stft_in[n] = signal[n + m * stft_struct->hop] * stft_struct->wnd[n];
        }

        // compute fft using fftw
        fftwf_execute(stft_struct->plan);

        // normalize and handle negative frequencies
        for (int n = 0; n < num_bins; ++n) {
            int idx = m * num_bins + n;
            
            (*stft_result)[idx][0] = stft_struct->stft_out[n][0];
            (*stft_result)[idx][1] = stft_struct->stft_out[n][1];
        }
        
    }
}

void stft_clean(StftStruct *stft_struct) {
    fftwf_destroy_plan(stft_struct->plan);
    fftwf_free(stft_struct->stft_out);
    free(stft_struct->stft_in);
    free(stft_struct->wnd);
    free(stft_struct);
}
