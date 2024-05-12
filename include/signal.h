#ifndef SIGNAL_H
#define SIGNAL_H

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
} Signal;

/**
 * @brief Structure representing a stereo audio signal.
 */
typedef struct {
    int num_samples_per_channel; /**< Number of samples per channel. */
    Signal *left;                /**< Pointer to the left channel Signal. */
    Signal *right;               /**< Pointer to the right channel Signal. */
} StereoSignal;


int compute_num_frames(int num_samples, int win, int hop);
float* read_wav_file(const char *filename, unsigned int *channels, int *total_samples_count);
void write_wav_file(const char* filename, const float *audio_buffer, size_t total_samples_count, unsigned int sample_rate, unsigned int channels);
void write_to_csv(const char *filename, fftwf_complex *stft_data, int num_frames, int num_bins);

#endif
