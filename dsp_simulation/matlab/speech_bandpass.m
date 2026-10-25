%% speech_bandpass.m
% DSP Script: IIR Biquad Bandpass Filter & Automatic Gain Control (AGC)
% Reverse Path: Speech to Bone-Conduction Transducer (300 Hz - 3400 Hz)

clear; clc; close all;

fs = 16000;          % Audio sample rate (16 kHz)
f_low = 300;         % High-pass cutoff (Hz)
f_high = 3400;       % Low-pass cutoff (Hz)

% Normalised frequencies
W_low = f_low / (fs / 2);
W_high = f_high / (fs / 2);

% Design 2nd-order Butterworth bandpass filter
[b, a] = butter(2, [W_low, W_high], 'bandpass');

% Frequency Response
[H, f] = freqz(b, a, 2048, fs);
mag_db = 20 * log10(abs(H));

figure('Name', 'Speech Bandpass Response', 'Color', 'w');
plot(f, mag_db, 'LineWidth', 1.8); grid on; hold on;
plot([f_low, f_low], [-60, 5], 'r--');
plot([f_high, f_high], [-60, 5], 'r--');
xlabel('Frequency (Hz)');
ylabel('Magnitude (dB)');
title('Speech Bandpass Filter for Bone-Conduction Audio (300 Hz - 3400 Hz)');
xlim([0, 8000]); ylim([-60, 5]);
legend('IIR Bandpass', 'Cutoff Band Limits');

% Convert to Second-Order Sections (SOS) for embedded biquads
[sos, g] = tf2sos(b, a);
disp('Second-Order Sections (SOS Matrix):');
disp(sos);
disp('Overall Scale Gain:');
disp(g);
