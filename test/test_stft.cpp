#include "gtest/gtest.h"
#include "fastft_istft.h"
#include "fastft_stft.h"
#include "fastft_signal.h"
#include "fastft_padding.h"


// Define your individual unit tests
class STFT : public ::testing::Test {
protected:
    void SetUp() override {
        // Load audio file
        input_signal.data = fastft_read_wav_file(file_in, &input_signal.num_channels, &input_signal.num_samples);

        // Separate channels
        stereo_signal.left  = (fastft_signal_t*)calloc(1, sizeof(fastft_signal_t));
        stereo_signal.right = (fastft_signal_t *)calloc(1, sizeof(fastft_signal_t));
        stereo_signal.num_samples_per_channel = input_signal.num_samples / 2;
        stereo_signal.left->num_samples  = stereo_signal.num_samples_per_channel;
        stereo_signal.right->num_samples = stereo_signal.num_samples_per_channel;
        stereo_signal.left->data  = (float*)calloc(stereo_signal.left->num_samples, sizeof(float));
        stereo_signal.right->data = (float*)calloc(stereo_signal.left->num_samples, sizeof(float));
        for (int i = 0; i < input_signal.num_samples; i += 2) {
            stereo_signal.left->data[i / 2]  = input_signal.data[i];
            stereo_signal.right->data[i / 2] = input_signal.data[i + 1];
        }

        // free mono audio data 
        free(input_signal.data);

        // Initialize common variables here
        num_bins = 1024;
        num_frames = 0;
        time_step = 64;
        fft_size = 4096;
        win_size = 4096;
        hop_size = 1024;

        // Initialize padding
        do_pad = 1;    
        pad_width = fft_size / 2;
        fastft_padding_init(&padding, 
                            REFLECT, 
                            pad_width,
                            stereo_signal.num_samples_per_channel,
                            stereo_signal.num_samples_per_channel  + 2 * pad_width,
                            fastft_compute_num_frames(stereo_signal.num_samples_per_channel, fft_size, hop_size),
                            fastft_compute_num_frames(stereo_signal.num_samples_per_channel + 2 * pad_width, fft_size, hop_size));
        }

    void TearDown() override {
        // Free stereo_signal, its structure and the padding allocated memory
        free(stereo_signal.left->data);
        free(stereo_signal.right->data);
        free(stereo_signal.left);
        free(stereo_signal.right);
        free(padding.padded_signal);
    }

    // File paths
    const char file_in[26] = "../../resources/test.wav";
    const char file_out[38] = "../resources/reconstructed_signal.wav";

    // signal variables
    fastft_signal_t input_signal;
    fastft_stereo_signal_t stereo_signal;

    // Common variables used across tests
    int num_bins;
    int num_frames;
    int time_step;
    int fft_size;
    int win_size;
    int hop_size;

    // Padding variables
    int do_pad;
    int pad_width;
    fastft_padding_t padding;

    // Initialize STFT and ISTFT structures
    fastft_STFT_t stft_struct;
    fastft_ISTFT_t istft_struct;
};

TEST_F(STFT, test_padding_initialization) {
    ASSERT_EQ(padding.mode, REFLECT);
    ASSERT_EQ(padding.width, pad_width);
    ASSERT_EQ(padding.unpadded_signal_length, stereo_signal.num_samples_per_channel);
    ASSERT_EQ(padding.padded_signal_length, padding.unpadded_signal_length + 2 * pad_width);
    ASSERT_EQ(padding.unpadded_num_frames, (stereo_signal.num_samples_per_channel - fft_size) / hop_size + 1);
    ASSERT_EQ(padding.padded_num_frames, (padding.padded_signal_length - fft_size) / hop_size + 1);
}

TEST_F(STFT, test_stft_initialization) {
    // Compute STFT for both channels
    fastft_stft_init(&stft_struct, fft_size, win_size, hop_size, &padding);
    ASSERT_EQ(stft_struct.nfft, fft_size);    
    ASSERT_EQ(stft_struct.win, win_size);    
    ASSERT_EQ(stft_struct.hop, hop_size);    
}

TEST_F(STFT, test_stft_compute) {  
  fastft_stft_init(&stft_struct, fft_size, win_size, hop_size, &padding);
  ASSERT_EQ(stft_struct.nfft, fft_size);    
  ASSERT_EQ(stft_struct.win, win_size);    
  ASSERT_EQ(stft_struct.hop, hop_size);    
  
  // update number of frames variable
  num_frames = do_pad ? padding.padded_num_frames : padding.unpadded_num_frames;
  ASSERT_EQ(padding.mode, REFLECT);
  ASSERT_EQ(padding.width, pad_width);
  ASSERT_EQ(padding.unpadded_signal_length, stereo_signal.num_samples_per_channel);
  ASSERT_EQ(padding.padded_signal_length, padding.unpadded_signal_length + 2 * pad_width);
  ASSERT_EQ(padding.unpadded_num_frames, (stereo_signal.num_samples_per_channel - fft_size) / hop_size + 1);
  ASSERT_EQ(padding.padded_num_frames, (padding.padded_signal_length - fft_size) / hop_size + 1);
  fftwf_complex* left_stft  = fastft_stft_compute(&stft_struct, stereo_signal.left,  num_bins, do_pad);
  fftwf_complex* right_stft = fastft_stft_compute(&stft_struct, stereo_signal.right, num_bins, do_pad);
  // stft_clean(&stft_struct);
  printf("3.\n");

  // Free stft memory after processing
  fftwf_free(left_stft);
  fftwf_free(right_stft);
}


