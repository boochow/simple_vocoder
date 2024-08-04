# Simple vocoder
This is a basic implementation of a vocoder for NTS-1 mk II and [NTS-3 kaoss pad](https://github.com/boochow/simple_vocoder/blob/nts-3/README.md). Connect your voice source to the audio-in to play some robot voice.

## Oscillator parameters

### SHAPE (knob A)
The decay curve parameter of the envelope extracted from the modulator signal.

### ALT (knob B)
The gain of the audio input signal.

## Files

### vocoder.h
An 8-band vocoder implementation. 

### osc.h
A sawtooth oscillator with a vocoder.

### test/exp_sweep.py
This script generates a signal that sweeps exponentially from 50 Hz to 10 kHz and back to 50 Hz, then saves it as a WAV file named exponential_sweep.wav.

### test/plot_filter.py
This code will plot the frequency responses of the individual bandpass filters in this vocoder and the combined response. 
