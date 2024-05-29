import csv
import librosa
import numpy as np
import matplotlib.pyplot as plt


# Constants
FS = 16000
FFT_SIZE = 512
HOP_SIZE = 256
WINDOW_SIZE = FFT_SIZE
MAX_DB_SCALE = 80.0
BIN_LIMIT = 257
EPSILON = 1e-10

def read_csv_to_numpy(filename):
    data = np.genfromtxt(filename, delimiter=',', skip_header=0)
    time_column = data[:, 0]  # Extract the time column
    stft_matrix = data[:, :]  # Extract the rest of the columns as the STFT matrix
    return stft_matrix.T

def fastft_stft(csv_filename):
    stft_matrix = read_csv_to_numpy(csv_filename)
    magnitude_spectrogram = np.abs(stft_matrix)
    return magnitude_spectrogram


def librosa_stft(audio_signal):
    stft = librosa.stft(audio_signal, n_fft=FFT_SIZE, hop_length=HOP_SIZE, win_length=FFT_SIZE, window='hann', 
                        center=True, dtype=None, pad_mode='constant', out=None)
    magnitude_spectrogram = np.abs(stft.T)
    return magnitude_spectrogram


def plot_magnitude_spectrogram_in_db(ax, magnitude_spectrogram, num_frames, analysis_bin_limit, title, index, max_magnitude, eps=True, max_yaxis=FFT_SIZE/2+1):
    min_db = -MAX_DB_SCALE
    db_values = librosa.amplitude_to_db(magnitude_spectrogram, ref=np.max)
    
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


def plot_difference_spectrogram(ax, magnitude_spectrogram_1, magnitude_spectrogram_2, num_frames, analysis_bin_limit, title, index):
    # Determine the maximum magnitude for consistent scaling
    diff_spectrogram = magnitude_spectrogram_1 - magnitude_spectrogram_2

    # Plot the difference spectrogram
    im = ax.imshow(diff_spectrogram, cmap="viridis", aspect="auto", origin="lower", extent=[0, analysis_bin_limit, 0, num_frames])
    fig.colorbar(im, ax=ax, label="dB")
    ax.set_title(f"{title} Difference id={index}")
    ax.set_xlabel("Frequency Bin")
    ax.set_ylabel("Time Frame")


if __name__ == "__main__":

    # Read wave data
    file_in = '../resources/test.wav'
    test_signal, sr = librosa.load(file_in, mono=True, sr=16000)

    # Load C read audio
    with open("./build/sndfile_signal.csv") as f: 
        snd_sig = np.array([float(line) for line in f.readlines()])

    # Filter out lines that cannot be converted to floats
    print("- len(librosa-sig) ? len(snd-sig) : ", len(test_signal), len(snd_sig))
    print("- librosa-sig      ? sndfile-sig  : ", (np.round(test_signal) == np.round(snd_sig)).all())
    print("- librosa-sig      - sndfile-sig  : ", ((test_signal - snd_sig)**2).mean(axis=0)) 

    # Load fastft STFT and compute librosa's STFT
    fastft_stft_result = fastft_stft("./build/fastft_stft.csv")
    librosa_stft_result = librosa_stft(test_signal)

    # Plot all three spectrograms side by side
    fig, axs = plt.subplots(1, 2, figsize=(20, 5))
    print("- fastft STFT shape               : ", fastft_stft_result.shape)
    print("- librosa STFT shape              : ", librosa_stft_result.shape)

    # Plot on the provided axes
    plot_magnitude_spectrogram_in_db(axs[0], fastft_stft_result,  fastft_stft_result.shape[0],  BIN_LIMIT, "C",       1, np.max(fastft_stft_result))
    plot_magnitude_spectrogram_in_db(axs[1], librosa_stft_result, librosa_stft_result.shape[0], BIN_LIMIT, "Librosa", 2, np.max(librosa_stft_result))
    plt.show()

    plt.plot([i for i in range(len(fastft_stft_result))], np.sqrt(np.square(fastft_stft_result - librosa_stft_result)), "b.")
    plt.title("Mean Squared Error (Difference)")
    plt.show()

    # Calculate the Mean Squared Error
    MSE = lambda A, B: (np.abs(A - B)).mean(axis=None)
    mse_fastft_librosa = MSE(fastft_stft_result, librosa_stft_result)
    print('- MSE(fastft_stft, librosa_stft)  : ', mse_fastft_librosa)
