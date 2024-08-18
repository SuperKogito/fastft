#ifndef FASTFT_SIGNAL_H
#define FASTFT_SIGNAL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <fftw3.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sndfile.h>
#include <math.h>

/**
 * @brief Structure representing an audio signal.
 */
typedef struct {
    int sample_rate;          /**< Sample rate of the audio signal. */
    unsigned int num_channels; /**< Number of channels in the audio signal. */
    int num_samples;          /**< Number of audio samples per channel. */
    float *data;              /**< Array of audio samples for each channel. */
} fastft_signal_t;

/**
 * @brief Structure representing a stereo audio signal.
 */
typedef struct {
    int num_samples_per_channel; /**< Number of samples per channel. */
    fastft_signal_t *left;                /**< Pointer to the left channel fastft_signal_t. */
    fastft_signal_t *right;               /**< Pointer to the right channel fastft_signal_t. */
} fastft_stereo_signal_t;


int fastft_compute_num_frames(int num_samples, int win, int hop);
float* fastft_read_wav_file(const char *filename, unsigned int *channels, int *total_samples_count);
void fastft_write_wav_file(const char* filename, const float *audio_buffer, size_t total_samples_count, unsigned int sample_rate, unsigned int channels);
void fastft_write_to_csv(const char *filename, fftwf_complex *stft_data, int num_frames, int num_bins);

#ifdef __cplusplus
}
#endif

#endif
