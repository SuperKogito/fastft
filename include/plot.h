#define MAX_DB_SCALE 80  

#include <stdio.h>
#include <math.h>

#ifdef WIN32
#define GNUPLOT_NAME "gnuplot.exe -persistent"
#else
#define GNUPLOT_NAME "gnuplot -persistent"
#endif

void plot_magnitude_spectrogram_in_db(float *magnitude_spectrogram, int nfft, int num_frames, int analysis_bin_limit, const char *title, int index, int use_db);
void plot_xy(float *data, int data_count, const char *x_label, const char *y_label, const char *title);
