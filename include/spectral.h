#define BIN_LIMIT    2049
#define FFT_SIZE     4096
#define HOP_SIZE     1024
#define WINDOW_SIZE  4096
#define LIBROSA_PI   3.14159265358979323846


#include <fftw3.h>
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <libgen.h>
#include <stdint.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdint.h>

#include "pad.h"


typedef struct {
    int sample_rate;          // Sample rate of the audio signal
    unsigned int num_channels; // Number of channels in the audio signal
    int num_samples;  // Number of audio samples per channel
    float *data;              // Array of audio samples for each channel
} Signal;

typedef struct {
    int num_samples_per_channel;
    Signal *left;  // Pointer to the left channel Signal
    Signal *right; // Pointer to the right channel Signal
} StereoSignal;

// Structure for STFT parameters and plans
typedef struct {
    int win;
    int hop;
    int nfft;
    fftwf_plan plan;
    float* stft_in;
    fftwf_complex* stft_out;
    float *wnd;
    Padding* padding;
} StftStruct;

// Structure for ISTFT parameters and plans
typedef struct {
    int win;
    int hop;
    int nfft;
    fftwf_plan plan;
    fftwf_complex *istft_in;
    float *istft_out;
    float *wnd;
    float *window_sum;
} IstftStruct;



void compute_hanning_window(int n_fft, float *window);

void stft_init(StftStruct *stft_struct, int n_fft, int win_length, int hop_length, Padding* padding);
fftwf_complex* stft_compute(StftStruct* stft_struct, Signal *channel_singal, int num_bins, int do_pad);
void stft_inner(StftStruct* stft_struct, float* signal, fftwf_complex** stft_result, int num_frames, int num_bins);
void stft_clean(StftStruct *stft_struct);


void istft_init(IstftStruct *istft_struct, int n_fft, int reconstructed_signal_length, int win_length, int hop_length);
void istft_compute(IstftStruct* istft_struct, fftwf_complex *stft_values, float *reconstructed_signal, int reconstructed_signal_length, int num_frames, int num_bins);
void istft_clean(IstftStruct *istft_struct);


void calculate_magnitude(fftwf_complex *complex_signal, float *magnitude, int length);