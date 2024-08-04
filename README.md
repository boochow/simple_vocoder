# Simple vocoder
This is a basic implementation of a vocoder for NTS-3 kaoss pad. Connect your voice source to the audio-in to play some robot voice.

## Default parameter settings

### Pitch
The oscillator's pitch, which is defined as `note number * 8`.

### Depth
The mix ratio of the input signal and the vocoder signal.

### Decay
The decay curve parameter of the envelope extracted from the modulator signal.

### Gain
The gain of the audio input signal.

### Chroma
When this option is enabled, the pitch is snapped to the chromatic scale.

## Files

### vocoder.h
An 8-band vocoder implementation. 

### effects.h
A sawtooth oscillator with a vocoder.

### test/exp_sweep.py
This script generates a signal that sweeps exponentially from 50 Hz to 10 kHz and back to 50 Hz, then saves it as a WAV file named exponential_sweep.wav.

### test/plot_filter.py
This code will plot the frequency responses of the individual bandpass filters in this vocoder and the combined response. 
