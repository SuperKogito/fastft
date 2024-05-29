/*
 * Description: Implementation of STFT using fastft, the result is saved to CSV file and compared via benchmark.py to Python.
 * Copyright (c) 2024 Ayoub Malek.
 * This source code is licensed under the terms of the BSD 3-Clause License.
 * For a copy, see <https://github.com/SuperKogito/fastft/blob/master/LICENSE>.
 */
#include <chrono>
#include <iostream>
#include <vector>
#include <complex>
#include <cmath>
#include <cstddef>
#include <ctime>
#include <valarray>
#include <algorithm>
#include <fftw3.h>  
#include <sndfile.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <iterator>
#include <algorithm>

extern "C" {
    #include <spectral.h>
}


void write_to_csv(const char *filename, float *sig_data, int num_frames, int num_bins) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Failed to open file");
        return;
    }

    int ncols = num_bins; // Assuming ncols is equivalent to num_bins
    int nrows = num_frames; // Assuming nrows is equivalent to 240

    for (int col = 0; col < ncols; col++) {
        for (int row = 0; row < nrows; row++) {
            double magnitude = sig_data[row * ncols + col];
            fprintf(file, "%lf%s", magnitude, (row < nrows - 1 ? "," : ""));
        }
        fprintf(file, "\n");
    }

    fclose(file);
}

void write_to_csv(const char *filename, fftwf_complex *stft_data, int num_frames, int num_bins) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Failed to open file");
        return;
    }

    int ncols = num_bins; // Assuming ncols is equivalent to num_bins
    int nrows = num_frames; // Assuming nrows is equivalent to 240

    for (int col = 0; col < ncols; col++) {
        for (int row = 0; row < nrows; row++) {
            double magnitude = sqrt(pow(stft_data[row * ncols + col][0], 2) + pow(stft_data[row * ncols + col][1], 2));
            fprintf(file, "%lf%s", magnitude, (row < nrows - 1 ? "," : ""));
        }
        fprintf(file, "\n");
    }

    fclose(file);
}

// Function to load a 3D vector from a CSV file
void load_from_csv(const std::string& filename, std::vector<std::vector<std::vector<float>>>& matrix, size_t num_frames, size_t num_bins) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }

    // Assuming the matrix is already initialized
    size_t t = 0;
    std::string line;
    while (std::getline(file, line) && t < num_frames) {
        std::istringstream iss(line);
        std::string value;
        size_t i = 0;
        while (std::getline(iss, value, ',') && i < num_bins) {
            matrix[0][t][i] = std::stof(value);
            ++i;
        }
        ++t;
    }

    file.close();
}

// Reads a WAV file and returns the audio data as a float array. Also sets the sampling rate.
float* read_wave_file(const char* filename, int* fs, int* num_samples) {
    SF_INFO sfinfo;
    sfinfo.format = 0;

    // Open the WAV file
    SNDFILE* file = sf_open(filename, SFM_READ, &sfinfo);
    if (!file) {
        fprintf(stderr, "Could not open WAV file: %s\n", sf_strerror(NULL));
        return NULL;
    }

    // Get the sampling rate and number of samples
    *fs = sfinfo.samplerate;
    *num_samples = sfinfo.frames * sfinfo.channels;

    // Allocate memory for the audio data
    float* float_data = (float*)malloc(*num_samples * sizeof(float));
    if (!float_data) {
        fprintf(stderr, "Memory allocation failed\n");
        sf_close(file);
        return NULL;
    }

    // Read the audio data
    sf_count_t num_read = sf_readf_float(file, float_data, sfinfo.frames);
    if (num_read < sfinfo.frames) {
        fprintf(stderr, "Only read %lld items out of %lld\n", (long long)num_read, (long long)sfinfo.frames);
        sf_close(file);
        free(float_data);
        return NULL;
    }

    sf_close(file);
    return float_data;
}

// Computes the STFT magnitude of a mono signal.
float* compute_mono_stft_mag(float* sig4mosnet, int fs, int num_samples, int fft_size, int hop_size, int win_size, int* num_frames, int num_bins) {
    Signal input_signal;
    input_signal.num_channels = 1;
    input_signal.sample_rate = fs;
    input_signal.num_samples = num_samples;
    input_signal.data = sig4mosnet;

    int do_pad = 1;
    Padding padding;
    padding.mode = REFLECT;
    padding.width = fft_size / 2;
    padding.padded_signal = NULL;
    padding.unpadded_signal_length = input_signal.num_samples;
    padding.padded_signal_length = input_signal.num_samples + 2 * padding.width;
    padding.unpadded_num_frames = (padding.unpadded_signal_length - fft_size) / hop_size + 1;
    padding.padded_num_frames = (padding.padded_signal_length - fft_size) / hop_size + 1;
    padding.padded_signal = (float *)calloc(padding.padded_signal_length, sizeof(float));

    *num_frames = do_pad ? padding.padded_num_frames : padding.unpadded_num_frames;

    StftStruct *stft_struct = (StftStruct *)calloc(1, sizeof(StftStruct));
    stft_init(stft_struct, fft_size, win_size, hop_size, &padding);
    fftwf_complex *mono_stft = stft_compute(stft_struct, &input_signal, num_bins, do_pad);
    stft_clean(stft_struct);

    size_t input_tensor_size = (*num_frames) * num_bins;
    float* mono_stft_mag = (float*)malloc(input_tensor_size * sizeof(float));
    calculate_magnitude(mono_stft,  mono_stft_mag,  input_tensor_size);    
    
    // log STFT to csv
    write_to_csv("fastft_stft.csv", mono_stft, *num_frames, num_bins);

    fftwf_free(mono_stft);
    return mono_stft_mag;
}

int main(int argc, char* argv[]) {
    if (argc < 2 || (argc == 2 && strcmp(argv[1], "--help") == 0)) {
        printf("Usage: %s <wav_fpath> <model_path> [verbose]\n", argv[0]);
        printf("  <wav_fpath>  : Path to the WAV file\n");
        return 0;
    }

    char* wav_fpath = argv[1];
    int fs;
    int num_samples;
    float* sig4mosnet = read_wave_file(wav_fpath, &fs, &num_samples);
    if (!sig4mosnet) {
        fprintf(stderr, "Failed to read audio file.\n");
        return 1;
    }

    // log STFT to csv
    write_to_csv("sndfile_signal.csv", sig4mosnet, 1, num_samples);

    int fft_size = 512;
    int hop_size = 256;
    int win_size = 512;
    int num_bins = fft_size / 2 + 1;
    int num_frames = 0;

    float* stft_mag = compute_mono_stft_mag(sig4mosnet, fs, num_samples, fft_size, hop_size, win_size, &num_frames, num_bins);
    printf("Spectrogram shape : %dx%d\n", num_frames, num_bins);
    return 0;
}
