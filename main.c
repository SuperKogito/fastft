// #define num_bins    2049  //1 + n_fft/2

#include "include/spectral.h"
#include "include/plot.h"
#include "include/io.h"
#include <stdlib.h>


int main() {
    int num_bins = 1024;
    int num_frames = 0;

    // You need to provide your own stereo interleaved audio signal and its length
    char file_in[]  = "../resources/short.wav";
    char file_out[] = "../resources/reconstructed_signal.wav";

    // read wave data
    Signal input_signal;
    input_signal.data = read_wav_file(file_in, &input_signal.num_channels, &input_signal.num_samples);

    // Use logging instead of printf
    fprintf(stdout, "- Audio file              :%s\n", file_in);
    fprintf(stdout, "- Samples count           :%d\n", input_signal.num_samples);
    fprintf(stdout, "- Channels count          :%d\n", input_signal.num_channels);

    if (!input_signal.data) {
        fprintf(stderr, "No wave input read\n");
        return -1;
    }

    // Separate channels
    StereoSignal stereo_signal;
    stereo_signal.left  = malloc(sizeof(Signal));
    stereo_signal.right = malloc(sizeof(Signal));
    stereo_signal.num_samples_per_channel = input_signal.num_samples / 2;
    stereo_signal.left->num_samples  = stereo_signal.num_samples_per_channel;
    stereo_signal.right->num_samples = stereo_signal.num_samples_per_channel;
    stereo_signal.left->data  = calloc(stereo_signal.left->num_samples, sizeof(float));
    stereo_signal.right->data = calloc(stereo_signal.left->num_samples, sizeof(float));
    for (int i = 0; i < input_signal.num_samples; i += 2) {
        stereo_signal.left->data[i / 2]  = input_signal.data[i];
        stereo_signal.right->data[i / 2] = input_signal.data[i + 1];
    }

    // Initialize padding
    int do_pad = 1;
    Padding padding;
    padding.mode = REFLECT;
    padding.width = FFT_SIZE / 2;
    padding.padded_signal = NULL;
    padding.unpadded_signal_length = stereo_signal.num_samples_per_channel;
    padding.padded_signal_length = stereo_signal.num_samples_per_channel  + 2 * padding.width;
    padding.unpadded_num_frames = (padding.unpadded_signal_length - FFT_SIZE) / HOP_SIZE + 1;
    padding.padded_num_frames = (padding.padded_signal_length - FFT_SIZE) / HOP_SIZE + 1;
    padding.padded_signal = (float*)calloc(padding.padded_signal_length, sizeof(float));

    // Compute frames for stft
    fprintf(stdout, "- Left signal size        :%d\n", stereo_signal.num_samples_per_channel);
    fprintf(stdout, "- Unpadded frames count   :%d\n", padding.unpadded_num_frames);
    fprintf(stdout, "- Padded frames count     :%d\n", padding.padded_num_frames);

    // update number of frames variable
    num_frames = do_pad ? padding.padded_num_frames : padding.unpadded_num_frames;

    // Initialize STFT and ISTFT structures
    StftStruct* stft_struct = malloc(sizeof(StftStruct));
    IstftStruct* istft_struct = malloc(sizeof(IstftStruct));

   
    // Compute STFT for both channels
    stft_init(stft_struct, FFT_SIZE, WINDOW_SIZE, HOP_SIZE, &padding);
    fftwf_complex* left_stft  = stft_compute(stft_struct, stereo_signal.left,  num_bins, do_pad);    
    fftwf_complex* right_stft = stft_compute(stft_struct, stereo_signal.right, num_bins, do_pad);
    stft_clean(stft_struct);
    
    // Compute magnitude
    float *left_mag  = (float*)malloc(sizeof(float) * num_frames * num_bins);
    float *right_mag = (float*)malloc(sizeof(float) * num_frames * num_bins);

    printf("%dx%d\n", num_frames, num_bins);
    calculate_magnitude(left_stft,  left_mag,  num_frames * num_bins);    
    calculate_magnitude(right_stft, right_mag, num_frames * num_bins);

    char const magnitude_plot_title[] = "Left magnitude";
    plot_magnitude_spectrogram_in_db(left_mag, FFT_SIZE, num_frames, num_bins, magnitude_plot_title, 0);

    // After computing STFT:
    write_to_csv("../resources/spectrogram_left.csv",  left_stft,  num_frames, num_bins);
    write_to_csv("../resources/spectrogram_right.csv", right_stft, num_frames, num_bins);
    fprintf(stdout, "STFT saved\n");
    
    // ISTFT (Inverse STFT) and reconstruction can be done similarly
    int reconstructed_channel_signal_lenght = (num_frames - 1 ) * HOP_SIZE + FFT_SIZE;
    float *reconstructed_audio_signal  = malloc(sizeof(float) * input_signal.num_samples);
    float *reconstructed_left_channel  = malloc(sizeof(float) * reconstructed_channel_signal_lenght);
    float *reconstructed_right_channel = malloc(sizeof(float) * reconstructed_channel_signal_lenght);
    fprintf(stdout, "- reconstructed left size :%d\n", input_signal.num_samples/2);
    fprintf(stdout, "- Frames count            :%d\n", num_frames);

    istft_init(istft_struct, FFT_SIZE, stereo_signal.num_samples_per_channel, WINDOW_SIZE, HOP_SIZE);
    istft_compute(istft_struct, left_stft,  reconstructed_left_channel,  stereo_signal.num_samples_per_channel, padding.unpadded_num_frames, num_bins);
    istft_compute(istft_struct, right_stft, reconstructed_right_channel, stereo_signal.num_samples_per_channel, padding.unpadded_num_frames, num_bins);
    istft_clean(istft_struct);

    fprintf(stdout, "ISTFT computed\n");
    float *diff = calloc(input_signal.num_samples/2, sizeof(float));
    for (int i = 0; i < input_signal.num_samples/2; i += 1) {
        diff[i] = fabs(reconstructed_left_channel[i] - stereo_signal.left->data[i]);
    }

    // plot_xy(reconstructed_left_channel, input_signal.num_samples / 2, x, y, titlex);
    // plot_xy(diff, input_signal.num_samples / 2, x, y, titlex);

    for (int i = 0; i < input_signal.num_samples / 2; i += 1) {
        reconstructed_audio_signal[2*i]     = reconstructed_left_channel[i];
        reconstructed_audio_signal[2*i + 1] = reconstructed_right_channel[i];
    }

    write_wav_file(file_out, reconstructed_audio_signal, stereo_signal.num_samples_per_channel, 44100, 2);
    fprintf(stdout, "wav computed\n");

    // Free signal allocated memory
    free(input_signal.data);
    fprintf(stdout, "1\n");

    // Free stereo_signal allocated memory
    free(stereo_signal.left->data);
    free(stereo_signal.right->data);
    fprintf(stdout, "2\n");

    // Free stereo_signal structure
    free(stereo_signal.left);
    free(stereo_signal.right);
    fprintf(stdout, "3\n");

    // Free stft memory after processing
    fftwf_free(left_stft);
    fftwf_free(right_stft);
    fprintf(stdout, "4\n");

    free(left_mag);
    free(right_mag);
    fprintf(stdout, "5\n");


    fprintf(stdout, "6\n");

    // Free istft memory after processing
    free(reconstructed_audio_signal);
    free(reconstructed_left_channel);
    free(reconstructed_right_channel);
    fprintf(stdout, "7\n");

    // Free additional memory after processing
    free(diff);
    fprintf(stdout, "8\n");
    return 0;
}
