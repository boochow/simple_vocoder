import numpy as np
import matplotlib.pyplot as plt
from scipy.signal import iirfilter, freqz

# Define the center frequencies
center_frequencies = [250, 375, 563, 844, 1266, 1898, 2848, 4271]
Q = 6
fs = 48000  # Sampling frequency

# Prepare the plot
plt.figure(figsize=(12, 6))

# Initialize the cumulative frequency response
cumulative_h = np.zeros(8000)

# Loop through the center frequencies and plot each filter's response
for f0 in center_frequencies:
    # Calculate the parameters for the biquad filter
    omega = 2 * np.pi * f0 / fs
    alpha = np.sin(omega) / (2 * Q)

    b0 = alpha
    b1 = 0
    b2 = -alpha
    a0 = 1 + alpha
    a1 = -2 * np.cos(omega)
    a2 = 1 - alpha

    b = [b0, b1, b2]
    a = [a0, a1, a2]

    # Cascade two identical biquad filters
    b_cascade = np.convolve(b, b)
    a_cascade = np.convolve(a, a)

    # Frequency response
    w, h = freqz(b_cascade, a_cascade, worN=8000, fs=fs)

    # Convert to dB
    h_dB = 20 * np.log10(np.maximum(np.abs(h), 1e-10))

    # Plot individual filter response
    plt.plot(w, h_dB, label=f'fc: {f0} Hz')

    # Add to the cumulative response
    cumulative_h += np.abs(h)

# Plot the cumulative frequency response in dB
cumulative_h_dB = 20 * np.log10(np.maximum(cumulative_h, 1e-10))
plt.plot(w, cumulative_h_dB, 'k--', label='Cumulative Response')

# Finalize the plot
plt.xscale('log')
plt.xlim([100, 24000])
plt.ylim([-60, 5])
plt.title('Frequency Response of Multiple Cascaded BiQuad Bandpass Filters')
plt.xlabel('Frequency [Hz]')
plt.ylabel('Gain [dB]')
plt.grid(which='both', linestyle='--', linewidth=0.5)
plt.legend(loc=1)
plt.show()
