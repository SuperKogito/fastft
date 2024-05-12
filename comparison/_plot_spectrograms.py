import os
os.environ['TF_CPP_MIN_LOG_LEVEL'] = '3' 
import tensorflow as tf

import csv
import librosa
import numpy as np
import tensorflow as tf
import soundfile as sf
import matplotlib.pyplot as plt
from scipy.io.wavfile import write, read
from sklearn.metrics import mean_squared_error

# Constants
FS = 44100
FFT_SIZE = 4096
HOP_SIZE = 1024
WINDOW_SIZE = FFT_SIZE
MAX_DB_SCALE = 80.0
BIN_LIMIT = 2049
EPSILON = 1e-10

def read_csv_to_numpy(filename):
    data = np.genfromtxt(filename, delimiter=',', skip_header=0)
    time_column = data[:, 0]  # Extract the time column
    stft_matrix = data[:, 1:]  # Extract the rest of the columns as the STFT matrix
    return stft_matrix

def c_stft(csv_filename='resources/spectrogram_left.csv'):
    stft_matrix = read_csv_to_numpy(csv_filename)
    magnitude_spectrogram = np.abs(stft_matrix)
    return magnitude_spectrogram

def librosa_stft(audio_signal):
    stft = librosa.stft(audio_signal, n_fft=FFT_SIZE, hop_length=HOP_SIZE, win_length=FFT_SIZE, window='hann', 
                        center=True, dtype=None, pad_mode='constant', out=None)
    magnitude_spectrogram = np.abs(stft.T)
    return magnitude_spectrogram

def tensorflow_stft(audio_signal):
    stft = tf.signal.stft(audio_signal, frame_length=FFT_SIZE, frame_step=HOP_SIZE, fft_length=FFT_SIZE, window_fn=tf.signal.hann_window)
    magnitude_spectrogram = tf.abs(stft)
    return magnitude_spectrogram

def compute_mag_db(magnitude_spectrogram, max_magnitude, min_db = -MAX_DB_SCALE):
    magnitude_spectrogram = magnitude_spectrogram + EPSILON
    db_values = 20 * np.log10(magnitude_spectrogram / max_magnitude)
    db_values = np.maximum(db_values, min_db)
    return db_values

def plot_magnitude_spectrogram_in_db(ax, magnitude_spectrogram, num_frames, analysis_bin_limit, title, index, max_magnitude, eps=True, max_yaxis=FFT_SIZE/2+1):
    min_db = -MAX_DB_SCALE

    if eps:
        magnitude_spectrogram = magnitude_spectrogram + EPSILON
    
    db_values = compute_mag_db(magnitude_spectrogram, max_magnitude)
    
    # Calculate the bin width size
    bin_width = FS / FFT_SIZE
    
    # Specify the extent parameter with four values, factoring in the bin width on the x-axis
    extent = [0, analysis_bin_limit * bin_width, 0, num_frames]
    
    im = ax.imshow(db_values, cmap="viridis", aspect="auto", origin="lower", extent=extent)
    fig.colorbar(im, ax=ax, label="dB")
    ax.set_title(f"{title} id={index}")
    ax.set_xlabel("Frequency [Hz]")  # Update x-axis label to Frequency [Hz]
    ax.set_ylabel("Time Frame")
    ax.set_xlim(0, max_yaxis)


def plot_difference_spectrogram(ax, magnitude_spectrogram_1, magnitude_spectrogram_2, num_frames, analysis_bin_limit, title, index, max_magnitude, min_db=-MAX_DB_SCALE):
    # Calculate the difference between the two spectrograms
    diff_spectrogram = magnitude_spectrogram_1 - magnitude_spectrogram_2

    # Determine the maximum magnitude for consistent scaling
    common_max_magnitude = max(np.max(np.abs(magnitude_spectrogram_1)), np.max(np.abs(magnitude_spectrogram_2)))

    # Plot the difference spectrogram
    db_values = compute_mag_db(diff_spectrogram, min_db, common_max_magnitude)
    im = ax.imshow(db_values, cmap="viridis", aspect="auto", origin="lower", extent=[0, analysis_bin_limit, 0, num_frames])
    fig.colorbar(im, ax=ax, label="dB")
    ax.set_title(f"{title} Difference id={index}")
    ax.set_xlabel("Frequency Bin")
    ax.set_ylabel("Time Frame")



if __name__ == "__main__":
    # Read wave data
    file_in = 'resources/short.wav'
    audio_signal, sr = librosa.load(file_in, mono=False)
    sr, audio_signal = read(file_in)

    # Normalize the audio data to floats between -1 and 1
    left_channel = audio_signal[:, 0]  / 32767.0
    test_signal = left_channel
    print(test_signal.shape)

    plt.plot(left_channel)
    plt.show()

    # STFT
    c_stft_result = c_stft()
    tf_stft_result = tensorflow_stft(test_signal)
    librosa_stft_result = librosa_stft(test_signal)

    # Plot all three spectrograms side by side
    fig, axs = plt.subplots(1, 3, figsize=(20, 5))

    print("C:       ", c_stft_result.shape)
    print("TF:      ", tf_stft_result.shape)
    print("Librosa: ", librosa_stft_result.shape)

    y_axis_max = max(tf_stft_result.shape[0], librosa_stft_result.shape[0], c_stft_result.shape[0])

    # Plot on the provided axes
    plot_magnitude_spectrogram_in_db(axs[0], c_stft_result,       y_axis_max, BIN_LIMIT, "C",          1, np.max(c_stft_result))
    plot_magnitude_spectrogram_in_db(axs[1], tf_stft_result,      y_axis_max, BIN_LIMIT, "TensorFlow", 2, np.max(tf_stft_result))
    plot_magnitude_spectrogram_in_db(axs[2], librosa_stft_result, y_axis_max, BIN_LIMIT, "Librosa",    3, np.max(librosa_stft_result))

    plt.show()

    # check db values
    print('c?tf       : ', np.array_equal(c_stft_result, tf_stft_result))
    print('c?librosa  : ', np.array_equal(c_stft_result, librosa_stft_result))
    print('librosa?tf : ', np.array_equal(librosa_stft_result, tf_stft_result))

    # Find the common shape among the arrays
    common_shape = np.minimum.reduce([c_stft_result.shape, tf_stft_result.shape, librosa_stft_result.shape])

    # Slice the arrays to the common shape
    c_stft_result_common = c_stft_result[:common_shape[0], :common_shape[1]]
    tf_stft_result_common = tf_stft_result[:common_shape[0], :common_shape[1]]
    librosa_stft_result_common = librosa_stft_result[:common_shape[0], :common_shape[1]]

    # Calculate the Mean Squared Error
    mse_c_tf = mean_squared_error(c_stft_result_common, tf_stft_result_common)
    mse_c_librosa = mean_squared_error(c_stft_result_common, librosa_stft_result_common)
    mse_librosa_tf = mean_squared_error(librosa_stft_result_common, tf_stft_result_common)

    print('MSE(c, tf)       : ', mse_c_tf)
    print('MSE(c, librosa)  : ', mse_c_librosa)
    print('MSE(librosa, tf) : ', mse_librosa_tf)

    # Plot the differences
    fig, axs = plt.subplots(1, 3, figsize=(20, 5))

    # Determine the common size for all spectrograms
    a, b = librosa_stft_result.shape[0], c_stft_result.shape[1]

    # Determine the common maximum magnitude for scaling
    common_max = np.max([np.max(np.abs(c_stft_result[:a, :b])), np.max(np.abs(tf_stft_result[:a, :b])), np.max(np.abs(librosa_stft_result[:a, :b]))])

    # plot_difference_spectrogram(axs[0], c_stft_result[:a, :b], tf_stft_result[:a, :b], y_axis_max, BIN_LIMIT, "C vs TensorFlow", 4, common_max)
    plot_difference_spectrogram(axs[1], c_stft_result[:a, :b], librosa_stft_result[:a, :b], y_axis_max, BIN_LIMIT, "C vs Librosa", 5, common_max)
    # plot_difference_spectrogram(axs[2], tf_stft_result[:a, :b], librosa_stft_result[:a, :b], y_axis_max, BIN_LIMIT, "TensorFlow vs Librosa", 6, common_max)

    plt.show()