#ifndef FASTFT_SPECTRAL_H
#define FASTFT_SPECTRAL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "fastft_istft.h"
#include "fastft_padding.h"
#include "fastft_stft.h"
#include "fastft_window.h"

/**
 * @brief Calculate the magnitude of complex signals.
 *
 * @param complex_signal Pointer to the complex signal.
 * @param magnitude Pointer to store the magnitude values.
 * @param length Length of the signal.
 */
void fastft_spectral_calculate_magnitude(fftwf_complex *complex_signal, float *magnitude, int length);

#ifdef __cplusplus
}
#endif

#endif
