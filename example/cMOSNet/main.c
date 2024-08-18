/*
 * Description: Implementation of MOSNet: Deep Learning based Objective Assessment for Voice Conversion.
 * Copyright (c) 2024 Ayoub Malek.
 * This source code is licensed under the terms of the BSD 3-Clause License.
 * For a copy, see <https://github.com/SuperKogito/fastft/blob/master/LICENSE>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <sndfile.h>
#include <math.h>
#include <assert.h>

#include "fastft_spectral.h"
#include "onnxruntime_c_api.h"

#define DEBUG 1
const OrtApi* g_ort = NULL;

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
    fastft_signal_t input_signal;
    input_signal.num_channels = 1;
    input_signal.sample_rate = fs;
    input_signal.num_samples = num_samples;
    input_signal.data = sig4mosnet;

    int do_pad = 1;
    fastft_padding_t padding;
    padding.mode = REFLECT;
    padding.width = fft_size / 2;
    padding.padded_signal = NULL;
    padding.unpadded_signal_length = input_signal.num_samples;
    padding.padded_signal_length = input_signal.num_samples + 2 * padding.width;
    padding.unpadded_num_frames = (padding.unpadded_signal_length - fft_size) / hop_size + 1;
    padding.padded_num_frames = (padding.padded_signal_length - fft_size) / hop_size + 1;
    padding.padded_signal = (float *)calloc(padding.padded_signal_length, sizeof(float));

    *num_frames = do_pad ? padding.padded_num_frames : padding.unpadded_num_frames;

    fastft_STFT_t *stft_struct = (fastft_STFT_t *)calloc(1, sizeof(fastft_STFT_t));
    fastft_stft_init(stft_struct, fft_size, win_size, hop_size, &padding);
    fftwf_complex *mono_stft = fastft_stft_compute(stft_struct, &input_signal, num_bins, do_pad);
    fastft_stft_clean(stft_struct);

    size_t input_tensor_size = (*num_frames) * num_bins;
    float* mono_stft_mag = (float*)malloc(input_tensor_size * sizeof(float));
    fastft_spectral_calculate_magnitude(mono_stft,  mono_stft_mag,  input_tensor_size);    

    fftwf_free(mono_stft);
    return mono_stft_mag;
}

int main(int argc, char* argv[]) {
    if (argc < 2 || (argc == 2 && strcmp(argv[1], "--help") == 0)) {
        printf("Usage: %s <wav_fpath> <model_path> [verbose]\n", argv[0]);
        printf("  <wav_fpath>  : Path to the WAV file\n");
        printf("  <model_path> : Path to the ONNX model file\n");
        printf("  [verbose]    : Optional. Set to 1 for verbose output\n");
        return 0;
    }

    char* wav_fpath = argv[1];
    if (argc < 3) {
        fprintf(stderr, "Model path not provided.\n");
        return 1;
    }

    char* model_path = argv[2];
    int verbose = 0;
    if (argc > 3) {
        verbose = atoi(argv[3]);
    }

    int fs;
    int num_samples;
    float* sig4mosnet = read_wave_file(wav_fpath, &fs, &num_samples);
    if (!sig4mosnet) {
        fprintf(stderr, "Failed to read audio file.\n");
        return 1;
    }

    int fft_size = 512;
    int hop_size = 256;
    int win_size = 512;
    int num_bins = fft_size / 2 + 1;
    int num_frames = 0;

    float* input_tensor_values = compute_mono_stft_mag(sig4mosnet, fs, num_samples, fft_size, hop_size, win_size, &num_frames, num_bins);
    size_t input_tensor_size = num_frames * num_bins;

    #ifndef DEBUG
    printf("Spectrogram shape : %dx%d\n", num_frames, num_bins);
    #endif

    g_ort = OrtGetApiBase()->GetApi(ORT_API_VERSION);
    if (!g_ort) {
        fprintf(stderr, "Failed to init ONNX Runtime engine.\n");
        return -1;
    }

    OrtEnv* env;
    if (g_ort->CreateEnv(ORT_LOGGING_LEVEL_WARNING, "MOSNetModel", &env) != ORT_OK) {
        fprintf(stderr, "Failed to create environment.\n");
        return -1;
    }
    assert(env != NULL);

    int ret = 0;
    OrtSessionOptions* session_options;
    if (g_ort->CreateSessionOptions(&session_options) != ORT_OK) {
        fprintf(stderr, "Failed to create session options.\n");
        return -1;
    }

    OrtSession* session;
    if (g_ort->CreateSession(env, model_path, session_options, &session) != ORT_OK) {
        fprintf(stderr, "Failed to create session.\n");
        return -1;
    }

    size_t input_height = num_frames;
    size_t input_width = num_bins; 
    float* model_input = input_tensor_values;
    size_t model_input_ele_count = input_height * input_width;

    OrtMemoryInfo* memory_info;
    if (g_ort->CreateCpuMemoryInfo(OrtArenaAllocator, OrtMemTypeDefault, &memory_info) != ORT_OK) {
        fprintf(stderr, "Failed to create memory info.\n");
        return -1;
    }

    const int64_t input_shape[] = {1, num_frames, num_bins};
    const size_t input_shape_len = sizeof(input_shape) / sizeof(input_shape[0]);
    const size_t model_input_len = model_input_ele_count * sizeof(float);

    OrtValue* input_tensor = NULL;
    if (g_ort->CreateTensorWithDataAsOrtValue(memory_info, model_input, model_input_len, input_shape,
                                              input_shape_len, ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT,
                                              &input_tensor) != ORT_OK) {
        fprintf(stderr, "Failed to create tensor with data as OrtValue.\n");
        return -1;
    }
    assert(input_tensor != NULL);

    int is_tensor;
    if (g_ort->IsTensor(input_tensor, &is_tensor) != ORT_OK || !is_tensor) {
        fprintf(stderr, "Input tensor is not a valid tensor.\n");
        return -1;
    }
        
    // Release memory info
    g_ort->ReleaseMemoryInfo(memory_info);

    // Define input and output names
    const char* input_names[] = {"input_1"};
    const char* output_names[] = {"avg", "frame"}; 

    // Create an array of OrtValue* to hold the output tensors
    OrtValue* output_tensors[2] = {NULL, NULL};

    // Run the model
    if (g_ort->Run(session, NULL, input_names, (const OrtValue* const*)&input_tensor, 1, output_names, 2, output_tensors) != ORT_OK) {
        fprintf(stderr, "Failed to run the model.\n");
        return -1;
    }

    // Process the first output tensor (average value)
    assert(output_tensors[0] != NULL);
    if (g_ort->IsTensor(output_tensors[0], &is_tensor) != ORT_OK || !is_tensor) {
        fprintf(stderr, "Output tensor 'avg' is not a valid tensor.\n");
        return -1;
    }

    float* avg_output_data = NULL;
    if (g_ort->GetTensorMutableData(output_tensors[0], (void**)&avg_output_data) != ORT_OK) {
        fprintf(stderr, "Failed to get tensor mutable data for 'avg'.\n");
        return -1;
    }
    printf("MOS average: %.3f\n", *avg_output_data);

    // Process the second output tensor (array of frame values)
    assert(output_tensors[1] != NULL);
    if (g_ort->IsTensor(output_tensors[1], &is_tensor) != ORT_OK || !is_tensor) {
        fprintf(stderr, "Output tensor 'frame' is not a valid tensor.\n");
        return -1;
    }

    float* frame_output_data = NULL;
    if (g_ort->GetTensorMutableData(output_tensors[1], (void**)&frame_output_data) != ORT_OK) {
        fprintf(stderr, "Failed to get tensor mutable data for 'frame'.\n");
        return -1;
    }

    // Retrieve the shape of the second output tensor
    OrtTensorTypeAndShapeInfo* shape_info;
    if (g_ort->GetTensorTypeAndShape(output_tensors[1], &shape_info) != ORT_OK) {
        fprintf(stderr, "Failed to get tensor type and shape info.\n");
        return -1;
    }

    size_t frame_output_count;
    if (g_ort->GetDimensionsCount(shape_info, &frame_output_count) != ORT_OK) {
        fprintf(stderr, "Failed to get dimensions count.\n");
        return -1;
    }

    int64_t* dims = (int64_t*)malloc(frame_output_count * sizeof(int64_t));
    if (g_ort->GetDimensions(shape_info, dims, frame_output_count) != ORT_OK) {
        fprintf(stderr, "Failed to get dimensions.\n");
        return -1;
    }

    size_t num_elements = 1;
    for (size_t i = 0; i < frame_output_count; ++i) {
        num_elements *= dims[i];
    }

    if (verbose == 1) {
        for (size_t i = 0; i < num_elements; ++i) {
            printf("Frame %3zu: %.3f\n", i, frame_output_data[i]);
        }
    }

    // Free the shape information
    g_ort->ReleaseTensorTypeAndShapeInfo(shape_info);
    free(dims);

    // Release the output tensors
    for (size_t i = 0; i < 2; ++i) {
        g_ort->ReleaseValue(output_tensors[i]);
    }

    g_ort->ReleaseSessionOptions(session_options);
    g_ort->ReleaseSession(session);
    g_ort->ReleaseEnv(env);
    if (ret != 0) {
        fprintf(stderr, "fail\n");
    }
    return ret;
}
