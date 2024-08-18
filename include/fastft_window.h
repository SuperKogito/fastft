#ifndef FASTFT_WINDOW_H
#define FASTFT_WINDOW_H

#ifdef __cplusplus
extern "C" {
#endif

#include <math.h>

/**
 * @brief Value of pi used for audio processing calculations.
 */
#define LIBROSA_PI   3.14159265358979323846

/**
 * @brief Compute the Hanning window for STFT.
 *
 * @param n_fft Length of the FFT.
 * @param window Pointer to store the computed Hanning window.
 */
void fastft_window_compute_hanning(int n_fft, float *window);

#ifdef __cplusplus
}
#endif

#endif
