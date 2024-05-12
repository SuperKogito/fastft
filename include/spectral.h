#ifndef SPECTRAL_H
#define SPECTRAL_H


#include "pad.h"
#include "window.h"
#include "trafo_stft.h"
#include "trafo_istft.h"


/**
 * @brief Calculate the magnitude of complex signals.
 *
 * @param complex_signal Pointer to the complex signal.
 * @param magnitude Pointer to store the magnitude values.
 * @param length Length of the signal.
 */
void calculate_magnitude(fftwf_complex *complex_signal, float *magnitude, int length);

#endif
