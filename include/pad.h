typedef enum {
    REFLECT,
    SYMMETRIC,
    EDGE,
    CONSTANT
} PaddingMode;


typedef struct {
    int width;
    float* padded_signal;
    int padded_signal_length;
    int unpadded_signal_length;
    PaddingMode mode;
    int padded_num_frames;
    int unpadded_num_frames;
} Padding;


void pad(float* signal, int signal_length, Padding* padding);