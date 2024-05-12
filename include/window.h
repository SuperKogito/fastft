#ifndef WINDOW_H
#define WINDOW_H

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
void compute_hanning_window(int n_fft, float *window);


#endif
