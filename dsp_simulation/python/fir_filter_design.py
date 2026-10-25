#!/usr/bin/env python3
"""
fir_filter_design.py
DSP Simulation Script: Designs a 41-tap Hamming-windowed FIR low-pass filter
for gesture sensor signal conditioning. Generates frequency response metrics
and exports C array headers.
"""

import math

def sinc(x):
    if abs(x) < 1e-12:
        return 1.0
    return math.sin(math.pi * x) / (math.pi * x)

def design_fir_lpf(order=40, fs=200.0, fc=50.0):
    num_taps = order + 1
    fc_norm = fc / fs  # 0.25
    half_order = order / 2.0
    h = []

    # Windowed-sinc method with Hamming window
    for k in range(num_taps):
        # Ideal sinc
        t = k - half_order
        hd = 2.0 * fc_norm * sinc(2.0 * fc_norm * t)

        # Hamming window: 0.54 - 0.46 * cos(2 * pi * k / order)
        w = 0.54 - 0.46 * math.cos(2.0 * math.pi * k / order)

        h.append(hd * w)

    # Normalize gain at DC (f = 0) to 1.0 (0 dB)
    dc_gain = sum(h)
    h_normalized = [coeff / dc_gain for coeff in h]

    return h_normalized

def compute_frequency_response(h, fs=200.0, n_points=512):
    freqs = []
    magnitudes_db = []
    
    for i in range(n_points):
        f = (i / float(n_points - 1)) * (fs / 2.0)
        freqs.append(f)
        
        # Discrete-time Fourier Transform sum
        omega = 2.0 * math.pi * f / fs
        re = sum(coeff * math.cos(k * omega) for k, coeff in enumerate(h))
        im = -sum(coeff * math.sin(k * omega) for k, coeff in enumerate(h))
        mag = math.sqrt(re * re + im * im)
        
        db = 20.0 * math.log10(max(mag, 1e-9))
        magnitudes_db.append(db)
        
    return freqs, magnitudes_db

def export_c_header(h, filename="fir_coeffs.h"):
    q15_scale = 32768
    with open(filename, "w") as f:
        f.write("/* Auto-generated FIR filter coefficients */\n")
        f.write(f"#define FIR_ORDER {len(h) - 1}\n")
        f.write(f"#define FIR_NUM_TAPS {len(h)}\n\n")
        f.write("static const float H_FLOAT[FIR_NUM_TAPS] = {\n")
        for i, val in enumerate(h):
            f.write(f"    {val: .8f}f{',' if i < len(h)-1 else ''}\n")
        f.write("};\n\n")
        f.write("static const int16_t H_Q15[FIR_NUM_TAPS] = {\n")
        for i, val in enumerate(h):
            q_val = int(round(val * q15_scale))
            if q_val > 32767: q_val = 32767
            if q_val < -32768: q_val = -32768
            f.write(f"    {q_val:6d}{',' if i < len(h)-1 else ''}\n")
        f.write("};\n")

if __name__ == "__main__":
    taps = design_fir_lpf(order=40, fs=200.0, fc=50.0)
    print(f"Generated {len(taps)} FIR filter coefficients.")
    freqs, mag_db = compute_frequency_response(taps, fs=200.0)
    
    # Verify cut-off and stopband
    idx_50 = min(range(len(freqs)), key=lambda i: abs(freqs[i] - 50.0))
    idx_70 = min(range(len(freqs)), key=lambda i: abs(freqs[i] - 70.0))
    print(f"Attenuation at 50 Hz (Cutoff): {mag_db[idx_50]:.2f} dB")
    print(f"Attenuation at 70 Hz (Stopband): {mag_db[idx_70]:.2f} dB")
