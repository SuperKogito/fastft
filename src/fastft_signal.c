#include "fastft_signal.h"

int fastft_compute_num_frames(int num_samples, int win, int hop) {
    int num_frames = (num_samples - win) / hop + 1;
    return num_frames;
}

float* fastft_read_wav_file(const char *filename, unsigned int *channels, int *total_samples_count) {
    SF_INFO sfinfo;
    sfinfo.format = 0; // This tells libsndfile to figure out the format automatically

    SNDFILE* file = sf_open(filename, SFM_READ, &sfinfo);
    if (!file) {
        printf("Could not open WAV file: %s\n", sf_strerror(NULL));
        return NULL;
    }

    *channels = sfinfo.channels;
    *total_samples_count = (int)sfinfo.frames * (int)sfinfo.channels ;

    // Allocate memory for the PCM data
    float *float_data = malloc(sfinfo.frames * sfinfo.channels * sizeof(float));
    if (!float_data) {
        printf("Memory allocation failed\n");
        sf_close(file);
        return NULL;
    }

    // Read the PCM data into our allocated buffer
    sf_count_t num_items = sfinfo.frames * sfinfo.channels;
    sf_count_t num_read = sf_read_float(file, float_data, num_items);
    if (num_read < num_items) {
        printf("Only read %ld items out of %ld\n", num_read, num_items);
        free(float_data);
        sf_close(file);
        return NULL;
    }

    sf_close(file);

    return float_data;
}

void fastft_write_wav_file(const char* filename, const float *audio_buffer, size_t total_samples_count, unsigned int sample_rate, unsigned int channels) {
    SNDFILE *file;
    SF_INFO sfinfo;

    // Clear SF_INFO memory and set up the structure.
    memset(&sfinfo, 0, sizeof(SF_INFO));
    sfinfo.frames = total_samples_count / channels;
    sfinfo.samplerate = sample_rate;
    sfinfo.channels = channels;
    sfinfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16; // Set format to WAV and PCM 16 bit.

    file = sf_open(filename, SFM_WRITE, &sfinfo);

    if (file == NULL) {
        fprintf(stderr, "Error opening the file '%s': %s\n", filename, sf_strerror(NULL));
        exit(1);
    }

    // Write the audio data and check for errors.
    sf_count_t framesWritten = sf_writef_float(file, audio_buffer, total_samples_count);
    if (framesWritten != total_samples_count) {
        fprintf(stderr, "Only wrote %ld frames out of %zu to the file '%s'\n", framesWritten, total_samples_count, filename);
    }

    sf_close(file);
}


void fastft_write_to_csv(const char *filename, fftwf_complex *stft_data, int num_frames, int num_bins) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Failed to open file");
        return;
    }

    double sample_rate = 44100;
    double hop_size = 1024;
    double time_per_frame = hop_size / sample_rate;

    for (int i = 0; i < num_frames; ++i) {
        double time = i * time_per_frame; // Calculate the time for each frame
        fprintf(file, "%lf,", time); // Write time as the first column

        for (int j = 0; j < num_bins; ++j) {
            double magnitude = sqrt(pow(stft_data[i * num_bins + j][0], 2) + pow(stft_data[i * num_bins + j][1], 2));
            fprintf(file, "%lf", magnitude);

            // Add comma if it's not the last bin
            if (j < num_bins - 1) {
                fprintf(file, ",");
            }
        }

        fprintf(file, "\n"); // Move to the next line for the next frame
    }

    fclose(file);
}
