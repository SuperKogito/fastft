#include "trafo_istft.h"


void istft_init(IstftStruct *istft_struct, int n_fft, int reconstructed_signal_length, int win_length, int hop_length) {
    istft_struct->nfft = n_fft;
    istft_struct->win = win_length;
    istft_struct->hop = hop_length;
    istft_struct->istft_in = (fftwf_complex*) fftwf_malloc(sizeof(fftwf_complex) * n_fft);
    istft_struct->istft_out = (float*) calloc(n_fft, sizeof(float));
    istft_struct->wnd = (float*)calloc(n_fft, sizeof(float));
    compute_hanning_window(n_fft, istft_struct->wnd);
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
            if (j < num_bins) {
                istft_struct->istft_in[j][0] = stft_values[i * num_bins + j][0];
                istft_struct->istft_in[j][1] = stft_values[i * num_bins + j][1];
            } else {
                istft_struct->istft_in[j][0] = 0;
                istft_struct->istft_in[j][1] = 0;
            }
        }


        for (int j = 1; j < n_fft/2+1; ++j) {
            if (j < num_bins) {
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

    // Find the maximum absolute value in the reconstructed signal
    float max_abs = get_max(reconstructed_signal, reconstructed_signal_length);
    normalize_array(reconstructed_signal, reconstructed_signal_length, max_abs);

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

// Function to normalize an array based on its maximum absolute value
void normalize_array(float *arr, int length, float max_value) {
    for (int i = 0; i < length; ++i) {
        if (max_value != 0) {
            arr[i] /= max_value;
        }
    }
}

// Function to find the maximum absolute value in an array
float get_max(const float *arr, int length) {
    float max_val = 0.0;
    for (int i = 0; i < length; ++i) {
        float abs_val = fabs(arr[i]);
        if (abs_val > max_val) {
            max_val = abs_val;
        }
    }
    return max_val;
}