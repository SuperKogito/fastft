#include "fastft_window.h"


void fastft_window_compute_hanning(int n_fft, float *window) {
    for (int i = 0; i < n_fft; i++) {
        window[i]  = 0.5 * (1.0 - cos(2.0 * LIBROSA_PI * i / (float)(n_fft - 1)));
    }
}
