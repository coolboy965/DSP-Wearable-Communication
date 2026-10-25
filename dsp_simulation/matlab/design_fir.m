%% design_fir.m
% DSP Script: Design of 41-Tap Low-Pass FIR Filter for Glove Sensor Conditioning
% Sampling Frequency: fs = 200 Hz
% Cutoff Frequency:   fc = 50 Hz

clear; clc; close all;

fs = 200;            % Sampling rate (Hz)
fc = 50;             % Cut-off frequency (Hz)
order = 40;          % Filter order (41 taps)
num_taps = order + 1;

% Normalised cutoff frequency (0 to 1, where 1 is Nyquist fs/2)
Wn = fc / (fs / 2);  % Wn = 0.50

% Compute Hamming-windowed FIR filter coefficients using fir1
h = fir1(order, Wn, 'low', hamming(num_taps));

% Frequency Response Analysis
[H, f] = freqz(h, 1, 1024, fs);
mag_db = 20 * log10(abs(H));
phase_deg = unwrap(angle(H)) * (180 / pi);

figure('Name', 'FIR Filter Frequency Response', 'Color', 'w');
subplot(2,1,1);
plot(f, mag_db, 'b', 'LineWidth', 1.8); grid on; hold on;
plot([0, fs/2], [-3, -3], 'r--', 'LineWidth', 1.2);
plot([fc, fc], [-80, 5], 'k:', 'LineWidth', 1.2);
xlabel('Frequency (Hz)');
ylabel('Magnitude (dB)');
title('Magnitude Response (Order 40, fc = 50 Hz, fs = 200 Hz)');
ylim([-80, 5]); xlim([0, fs/2]);
legend('Magnitude', '-3 dB Cutoff', 'fc = 50 Hz', 'Location', 'SouthWest');

subplot(2,1,2);
plot(f, phase_deg, 'r', 'LineWidth', 1.8); grid on;
xlabel('Frequency (Hz)');
ylabel('Phase (degrees)');
title('Linear Phase Response (Constant Group Delay = 20 samples = 100 ms)');
xlim([0, fs/2]);

% Export to Q15 format for STM32 embedded implementation
h_q15 = round(h * 32768);
disp('First 5 Q15 Coefficients:');
disp(h_q15(1:5));
