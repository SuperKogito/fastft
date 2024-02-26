#include "spectral.h"
#include <stdlib.h>
#include <math.h>


void compute_hanning_window(int n_fft, float *window) {
    for (int i = 0; i < n_fft; i++) {
        window[i]  = 0.5 * (1.0 - cos(2.0 * LIBROSA_PI * i / (float)(n_fft - 1)));
    }
}

void stft_init(StftStruct *stft_struct, int nfft, int win_length, int hop_length, Padding* padding) {
    stft_struct->nfft = nfft;
    stft_struct->win  = win_length;
    stft_struct->hop  = hop_length;
    printf("%d, %d, %d\n", stft_struct->nfft, stft_struct->win, stft_struct->hop);


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
    int num_frames = NULL;

    if (do_pad) {
        pad(channel_singal->data, channel_singal->num_samples, padding);
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
            stft_struct->stft_in[n] = signal[n + m * HOP_SIZE] * stft_struct->wnd[n];
        }

        // compute fft using fftw
        fftwf_execute(stft_struct->plan);

        // normalize and handle negative frequencies
        for (int n = 0; n < num_bins; ++n) {
            int idx = m * (stft_struct->nfft/2+1) + n;
            
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


// Function to find the maximum absolute value in an array
float max(const float *arr, int length) {
    float max_val = 0.0;
    for (int i = 0; i < length; ++i) {
        float abs_val = fabs(arr[i]);
        if (abs_val > max_val) {
            max_val = abs_val;
        }
    }
    return max_val;
}

// Function to normalize an array based on its maximum absolute value
void normalize(float *arr, int length, float max_value) {
    for (int i = 0; i < length; ++i) {
        if (max_value != 0) {
            arr[i] /= max_value;
        }
    }
}


void istft_init(IstftStruct *istft_struct, int n_fft, int reconstructed_signal_length, int win_length, int hop_length) {
    istft_struct->nfft = n_fft;
    istft_struct->win = win_length;
    istft_struct->hop = hop_length;
    istft_struct->istft_in = (fftwf_complex*) fftwf_malloc(sizeof(fftwf_complex) * n_fft);
    istft_struct->istft_out = (float*) calloc(n_fft, sizeof(float));
    istft_struct->wnd = (float*)calloc(WINDOW_SIZE, sizeof(float));
    compute_hanning_window(WINDOW_SIZE, istft_struct->wnd);
    istft_struct->plan = fftwf_plan_dft_c2r_1d(n_fft, istft_struct->istft_in, istft_struct->istft_out, FFTW_ESTIMATE);
    istft_struct->window_sum = (float*)calloc(reconstructed_signal_length, sizeof(float));
    if (!istft_struct->window_sum) {
        // Handle allocation failure
        printf("Failed to allocate window sum\n");
        return;
    }
}

void istft_compute(IstftStruct* istft_struct, fftwf_complex *stft_values, float *reconstructed_signal, int reconstructed_signal_length, int num_frames, int num_bins) {
    int n_fft = istft_struct->nfft;
    for (int i = 0; i < num_frames; ++i) {
        // Retrieve the complex spectrum for the current frame
        for (int j = 0; j < n_fft/2+1; ++j) {
            istft_struct->istft_in[j][0] = stft_values[i * num_bins + j][0];
            istft_struct->istft_in[j][1] = stft_values[i * num_bins + j][1];
 
            // Handle negative frequencies (except DC and Nyquist)
            if (j > 0) {
                istft_struct->istft_in[n_fft - j][0] = istft_struct->istft_in[j][0];
                istft_struct->istft_in[n_fft - j][1] = -istft_struct->istft_in[j][1];
            }
        }

        fftwf_execute(istft_struct->plan);

        for (int j = 0; j < n_fft; ++j) {
            int idx = i * istft_struct->hop + j;
            float window_value = istft_struct->wnd[j]; // Hanning window value

            istft_struct->window_sum[idx] += window_value;
            reconstructed_signal[idx] += istft_struct->istft_out[j] * window_value; // Apply window and add to signal
        }
    }
    
    // // Normalize the signal by the window sum
    // for (int i = 0; i < reconstructed_signal_length; ++i) {
    //     if (istft_struct->window_sum[i] != 0) {
    //         reconstructed_signal[i] /= istft_struct->window_sum[i];
    //     }
    // }


    // Find the maximum absolute value in the reconstructed signal
    float max_abs = max(reconstructed_signal, reconstructed_signal_length);
    normalize(reconstructed_signal, reconstructed_signal_length, max_abs);


    const char titlez[] = "Reconstruction";
    // plot_xy(reconstructed_signal, reconstructed_signal_length, NULL, NULL, titlez);
}

// Clean up ISTFT memory
void istft_clean(IstftStruct *istft_struct) {
    fftwf_destroy_plan(istft_struct->plan);
    fftwf_free(istft_struct->istft_in);
    free(istft_struct->istft_out);
    free(istft_struct->window_sum);
    free(istft_struct->wnd);
    free(istft_struct);
}

void calculate_magnitude(fftwf_complex *complex_signal, float *magnitude, int length) {
    for (int i = 0; i < length; ++i) {
        magnitude[i] = hypotf(complex_signal[i][0], complex_signal[i][1]);
    }
}
