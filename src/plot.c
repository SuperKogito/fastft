#include "plot.h"

void plot_magnitude_spectrogram_in_db(float *magnitude_spectrogram, int nfft, int num_frames, int analysis_bin_limit, const char *title, int index, int use_db) {
    FILE *gnuplotPipe = popen("gnuplot -persistent", "w");
    if (gnuplotPipe == NULL) {
        fprintf(stderr, "Error opening pipe to gnuplot.\n");
        return;
    }

    int FS = 44100;
    float bin_width = FS / nfft;
    float min_db = -MAX_DB_SCALE * use_db + (use_db == 0) * 1;
    float max_xaxis = analysis_bin_limit;
    float max_yaxis = num_frames;
    float EPSILON = 1e-6;

    fprintf(gnuplotPipe, "set title '%s id=%d'\n", title, index);
    fprintf(gnuplotPipe, "set xlabel 'Frequency (Hz)'\n");
    fprintf(gnuplotPipe, "set ylabel 'Time Frame'\n");
    fprintf(gnuplotPipe, "set pm3d map\n");
    fprintf(gnuplotPipe, "set cbrange [%f:%f]\n", min_db, 0.0f);
    fprintf(gnuplotPipe, "set colorbox user origin 0.85, 0.15 size 0.03, 0.7\n");
    fprintf(gnuplotPipe, "set xrange [0:%f]\n", max_xaxis);
    fprintf(gnuplotPipe, "set yrange [0:%f]\n", max_yaxis);

    fprintf(gnuplotPipe, "set xtics rotate\n");
    fprintf(gnuplotPipe, "set xlabel font ',6'\n"); // Adjust the font size as needed
    fprintf(gnuplotPipe, "set ylabel font ',6'\n"); // Adjust the font size as needed
    fprintf(gnuplotPipe, "set cblabel font ',6'\n"); // Adjust the font size as needed

    int ytics_interval = max_yaxis/ 10; // Change this value as needed
    fprintf(gnuplotPipe, "set ytics %d\n", ytics_interval);

    // Define a custom palette that matches the Librosa colormap
//    fprintf(gnuplotPipe, "set palette defined (0 '#000000', 1 '#030764', 2 '#1e9c22', 3 '#fceed9', 4 '#fbb4ac', 5 '#7e1e9c', 6 '#000000')\n");
    fprintf(gnuplotPipe, "set palette rgb 7,5,15\n");
    fprintf(gnuplotPipe, "splot '-' with image\n");

    float max_magnitude = 0;
    for (int i = 0; i < (num_frames * analysis_bin_limit); ++i) {
        if (max_magnitude < magnitude_spectrogram[i]) {
            max_magnitude = magnitude_spectrogram[i];
        }
    }

    for (int i = 0; i < num_frames; ++i) {
        for (int j = 0; j < analysis_bin_limit; ++j) {
            float magnitude = magnitude_spectrogram[i * analysis_bin_limit + j];
            float db_value = magnitude;
            magnitude = magnitude + EPSILON;

            if (use_db) {
                db_value = 20 * log10(magnitude / max_magnitude);  // Clipping the dB value
                db_value = fmax(db_value, -MAX_DB_SCALE);
            }


            // Calculate the frequency corresponding to the bin
            float frequency = j * bin_width;

            fprintf(gnuplotPipe, "%f %d %f\n", frequency, i, db_value);
        }
        fprintf(gnuplotPipe, "\n");
    }

    fprintf(gnuplotPipe, "e\n"); // End of data marker
    fflush(gnuplotPipe);

    // Close the gnuplot pipe
    fprintf(gnuplotPipe, "quit\n");
    pclose(gnuplotPipe);
}


void plot_xy(float *data, int data_count, const char *x_label, const char *y_label, const char *title) {
    FILE *gnuplotPipe = popen("gnuplot -persistent", "w");
    if (gnuplotPipe == NULL) {
        fprintf(stderr, "Error opening pipe to gnuplot.\n");
        return;
    }

    fprintf(gnuplotPipe, "set title '%s'\n", title);
    fprintf(gnuplotPipe, "set xlabel '%s'\n", x_label);
    fprintf(gnuplotPipe, "set ylabel '%s'\n", y_label);
    fprintf(gnuplotPipe, "set yrange [-1:1]\n");

    fprintf(gnuplotPipe, "plot '-' with lines\n");

    for (int i = 0; i < data_count; ++i) {
        // float idx = i * (44100.f / 2.f) / analysisBinLimit;
        fprintf(gnuplotPipe, "%d %f\n", i, data[i]);
    }

    fprintf(gnuplotPipe, "e\n");
    fflush(gnuplotPipe);
    pclose(gnuplotPipe);
}

