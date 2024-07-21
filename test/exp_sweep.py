import numpy as np
import scipy.io.wavfile as wavfile

# Parameters
fs = 48000  # Sampling frequency
f0 = 50     # Start frequency
f1 = 10000  # End frequency
duration = 10  # Duration of the sweep in seconds

# Time array
t = np.linspace(0, duration, int(fs * duration))

# Generate exponential sweep from f0 to f1
k = np.log(f1 / f0) / duration
sweep_up = np.sin(2 * np.pi * f0 * (np.exp(k * t) - 1) / k)

# Generate exponential sweep from f1 to f0
sweep_down = np.sin(2 * np.pi * f1 * (np.exp(-k * t) - 1) / -k)

# Concatenate the sweeps
sweep_signal = np.concatenate((sweep_up, sweep_down))

# Normalize the signal
sweep_signal = sweep_signal / np.max(np.abs(sweep_signal))

# Convert to 16-bit PCM
sweep_signal_pcm = np.int16(sweep_signal * 32767)

# Save to WAV file
wavfile.write('exponential_sweep.wav', fs, sweep_signal_pcm)

'exponential_sweep.wav file has been created'
