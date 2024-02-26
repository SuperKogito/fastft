# import numpy as np
# from scipy.signal import chirp
# import soundfile as sf

# fs = 44100
# T = 1
# t = np.arange(0, int(T * fs)) / fs
# w = chirp(t, f0=20, f1=20000, t1=T, method='logarithmic')

# # Create stereo signal by duplicating the mono signal
# stereo_signal = np.column_stack((w, w))

# # Save the stereo signal to a wave file
# sf.write('resources/chirp.wav', stereo_signal, fs, 'PCM_24')

# import numpy as np
# from scipy.io.wavfile import write

# def generate_stereo_test_signal(duration, sample_rate, frequencies):
#     t = np.linspace(0, duration, int(sample_rate * duration), endpoint=False)

#     # Generate left channel signal
#     left_signal = np.zeros_like(t)
#     for freq in frequencies:
#         left_signal += np.sin(2 * np.pi * freq * t)

#     # Generate right channel signal (you can modify this as needed)
#     right_signal = 0.5 * np.sin(2 * np.pi * 2 * frequencies[0] * t)

#     # Combine left and right signals
#     stereo_signal = np.vstack((left_signal, right_signal))

#     # Normalize the signals
#     stereo_signal /= np.max(np.abs(stereo_signal))

#     return stereo_signal

# def save_stereo_wav_file(filename, stereo_signal, sample_rate):
#     # Transpose stereo_signal to have time along the first axis
#     stereo_signal = stereo_signal.T

#     # Save the stereo signal as a WAV file
#     write(filename, sample_rate, stereo_signal)

# if __name__ == "__main__":
#     # Set parameters
#     duration = 3  # seconds
#     sample_rate = 44100  # Hz

#     # Define frequencies in Hz
#     frequencies = [64]

#     # Generate stereo test signal
#     stereo_test_signal = generate_stereo_test_signal(duration, sample_rate, frequencies)

#     # Save the stereo signal to a WAV file
#     save_stereo_wav_file("resources/test.wav", stereo_test_signal, sample_rate)




import os
os.environ['TF_CPP_MIN_LOG_LEVEL'] = '3' 
import tensorflow as tf

import librosa
import numpy as np
from scipy.fft import fft
from scipy.io import wavfile
import matplotlib.pyplot as plt

# Constants
FFT_SIZE = 1024
HOP_SIZE = 512
WINDOW_SIZE = 1024
MAX_DB_SCALE = 100
EPSILON = 1e-10  # Small value to avoid log(0)

def librosa_stft(audio_signal):
    stft = tf.signal.stft(audio_signal, frame_length=FFT_SIZE, frame_step=HOP_SIZE, fft_length=FFT_SIZE, window_fn=tf.signal.hann_window)
    magnitude_spectrogram = np.abs(stft)
    return magnitude_spectrogram

def tensorflow_stft(audio_signal):
    stft = tf.signal.stft(audio_signal, frame_length=FFT_SIZE, frame_step=HOP_SIZE, fft_length=FFT_SIZE, window_fn=tf.signal.hann_window)
    magnitude_spectrogram = tf.abs(stft)
    return magnitude_spectrogram

def compute_mag_db(magnitude_spectrogram, max_magnitude, min_db=-MAX_DB_SCALE):
    magnitude_spectrogram = magnitude_spectrogram + EPSILON
    db_values = 20 * np.log10(magnitude_spectrogram / max_magnitude)
    db_values = np.maximum(db_values, min_db)
    return db_values

def plot_magnitude_spectrogram_in_db(ax, magnitude_spectrogram, num_frames, analysis_bin_limit, title, index, max_magnitude, eps=True, fs=44100, fft_size=1024):
    min_db = -MAX_DB_SCALE

    if eps:
        magnitude_spectrogram = magnitude_spectrogram + EPSILON
    
    db_values = compute_mag_db(magnitude_spectrogram, max_magnitude)
    
    # Calculate the bin width size
    bin_width = fs / fft_size
    
    # Specify the extent parameter with four values, factoring in the bin width on the x-axis
    extent = [0, analysis_bin_limit * bin_width, 0, num_frames]
    
    im = ax.imshow(db_values, cmap="viridis", aspect="auto", origin="lower", extent=extent)
    fig.colorbar(im, ax=ax, label="dB")
    ax.set_title(f"{title} id={index}")
    ax.set_xlabel("Frequency [Hz]")  # Update x-axis label to Frequency [Hz]
    ax.set_ylabel("Time Frame")



def plot_difference_spectrogram(ax, magnitude_spectrogram_1, magnitude_spectrogram_2, analysis_bin_limit, title, index, max_magnitude, min_db=-MAX_DB_SCALE):
    # Calculate the difference between the two spectrograms
    diff_spectrogram = magnitude_spectrogram_1 - magnitude_spectrogram_2

    # Determine the maximum magnitude for consistent scaling
    common_max_magnitude = max(np.max(np.abs(magnitude_spectrogram_1)), np.max(np.abs(magnitude_spectrogram_2)))

    # Plot the difference spectrogram
    db_values = compute_mag_db(diff_spectrogram, common_max_magnitude)
    im = ax.imshow(db_values, cmap="viridis", aspect="auto", origin="lower", extent=[0, analysis_bin_limit])
    fig.colorbar(im, ax=ax, label="dB")
    ax.set_title(f"{title} Difference id={index}")
    ax.set_xlabel("Frequency Bin")
    ax.set_ylabel("Time Frame")




import numpy as np
from scipy.io import wavfile
import librosa

# Parameters
fs = 44100
N = int(fs * 0.25)
freq1 = 25
freq2 = 250

# Time vector
time = np.arange(N) / fs

# Generate a chirp signal
chirp_signal = np.sin(2 * np.pi * np.linspace(freq1, freq2, N) * time)
chirp_signal = chirp_signal.astype(np.float64)
signal = chirp_signal
# Create stereo chirp signal
stereo_signal = np.column_stack((chirp_signal, chirp_signal))

# Generate a signal with two frequency components
signal = np.sin(2 * np.pi * freq1 * time) #+ np.sin(2 * np.pi * freq2 * time)
signal = signal.astype(np.float64)

# Create stereo signal
stereo_signal = np.column_stack((signal, signal))

# Save to a stereo wave file with data type np.int16
wavfile.write("resources/test.wav", int(fs), stereo_signal.astype(np.float64))

# Load the audio with scipy.io.wavfile
samplerate, data_scipy = wavfile.read("resources/test.wav")

# Load the audio with librosa
audio_signal_librosa, sr_librosa = librosa.load("resources/test.wav", mono=False, dtype=np.float64)

# Print the differences
print(audio_signal_librosa.dtype, data_scipy.dtype, signal.dtype, stereo_signal.dtype)
print(np.array_equal(stereo_signal, data_scipy))  # Check right channel with scipy
print(np.array_equal(stereo_signal, audio_signal_librosa))  # Check left channel with librosa


# Compute FFT
fft_result = fft(signal)
max_bin = 100

# Compute and plot the STFT’s magnitude using TensorFlow
magnitude_spectrogram_tf = tensorflow_stft(signal)
magnitude_spectrogram_lb = librosa_stft(signal)

magnitude_spectrogram_tf = magnitude_spectrogram_tf[:, :max_bin]
magnitude_spectrogram_lb = magnitude_spectrogram_lb[:, :max_bin]
fig, ax = plt.subplots(figsize=(12, 6))  # Initialize fig here

plt.subplot(2, 2, 1)
plt.plot(time, signal)
plt.title('Time Domain')
plt.xlabel('Time [sec]')
plt.ylabel('Amplitude')

# Plot FFT
plt.subplot(2, 2, 2)
plt.plot(np.abs(fft_result[:max_bin]))
plt.title('FFT Magnitude')
plt.xlabel('Frequency Bin')
plt.ylabel('Amplitude')

plt.subplot(2, 2, 3)
plot_magnitude_spectrogram_in_db(plt.gca(), magnitude_spectrogram_tf, *magnitude_spectrogram_tf.shape, "TensorFlow STFT", 0, np.max(magnitude_spectrogram_tf))

plt.subplot(2, 2, 4)
plot_magnitude_spectrogram_in_db(plt.gca(), magnitude_spectrogram_lb, *magnitude_spectrogram_lb.shape, "Librosa STFT", 0, np.max(magnitude_spectrogram_lb))

plt.tight_layout()
plt.show()