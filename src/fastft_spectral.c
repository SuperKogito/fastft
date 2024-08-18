#include "fastft_spectral.h"


void fastft_spectral_calculate_magnitude(fftwf_complex *complex_signal, float *magnitude, int length) {
    for (int i = 0; i < length; ++i) {
        magnitude[i] = hypotf(complex_signal[i][0], complex_signal[i][1]);
    }
}
