#ifndef FASTFT_ISTFT_H
#define FASTFT_ISTFT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <fftw3.h>
#include <math.h>
#include <stdlib.h>

#include "fastft_padding.h"
#include "fastft_window.h"
#include "fastft_signal.h"


/**
 * @brief Structure for ISTFT (Inverse Short-Time Fourier Transform) parameters and plans.
 */
typedef struct {
    int win;                        /**< Window size. */
    int hop;                        /**< Hop size. */
    int nfft;                       /**< Number of FFT points. */
    fftwf_plan plan;                /**< FFTW plan for ISTFT computation. */
    fftwf_complex *istft_in;        /**< Input array for ISTFT computation. */
    float *istft_out;               /**< Output array for ISTFT computation. */
    float *wnd;                     /**< Hanning window array. */
    float *window_sum;              /**< Array storing the sum of window values for normalization. */
} fastft_ISTFT_t;

/**
 * @brief Initialize the ISTFT structure.
 *
 * @param istft_struct Pointer to the ISTFT structure to initialize.
 * @param n_fft Length of the FFT.
 * @param reconstructed_signal_length Length of the reconstructed signal.
 * @param win_length Length of the window.
 * @param hop_length Length of the hop.
 */
void fastft_istft_init(fastft_ISTFT_t *istft_struct, int n_fft, int reconstructed_signal_length, int win_length, int hop_length);

/**
 * @brief Compute the ISTFT (Inverse Short-Time Fourier Transform) for given STFT values.
 *
 * @param istft_struct Pointer to the ISTFT structure.
 * @param stft_values Pointer to the input STFT values.
 * @param reconstructed_signal Pointer to store the reconstructed signal.
 * @param reconstructed_signal_length Length of the reconstructed signal.
 * @param num_frames Number of frames.
 * @param num_bins Number of bins.
 */
void fastft_istft_compute(fastft_ISTFT_t* istft_struct, fftwf_complex *stft_values, float *reconstructed_signal, int reconstructed_signal_length, int num_frames, int num_bins);

/**
 * @brief Clean up resources allocated for ISTFT.
 *
 * @param istft_struct Pointer to the ISTFT structure to clean up.
 */
void fastft_istft_clean(fastft_ISTFT_t *istft_struct);


// Function to normalize an array based on its maximum absolute value
void fastft_normalize_array(float *arr, int length, float max_value);

// Function to find the maximum absolute value in an array
float fastft_get_max(const float *arr, int length);

#ifdef __cplusplus
}
#endif

#endif
