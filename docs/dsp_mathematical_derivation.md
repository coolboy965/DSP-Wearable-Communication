# Mathematical Formulations and DSP Signal Pipeline

## 1. Quantization Noise and ADC Model
The 12-bit successive-approximation ADC on the STM32F4 produces a quantization step size:

$$\Delta = \frac{V_{\text{ref}}}{2^N} = \frac{3.3\,\text{V}}{4096} \approx 0.8057\,\text{mV}$$

Under the uniform distribution model, the theoretical quantization noise power is:

$$P_q = \frac{\Delta^2}{12} \approx \frac{(0.8057 \times 10^{-3})^2}{12} \approx 5.41 \times 10^{-8}\,\text{V}^2$$

---

## 2. Linear-Phase FIR Filter Formulation
For sensor noise suppression (muscle tremors, electrical drift, and ADC jitter), a 40th-order (41-tap) Type-I symmetric FIR low-pass filter is implemented:

$$y[n] = \sum_{k=0}^{M} h[k] \cdot x[n-k], \quad M = 40$$

Symmetry condition:

$$h[k] = h[M-k]$$

This guarantees a constant group delay across all frequencies in the passband:

$$\tau_g = \frac{M}{2} = 20\,\text{samples} = \frac{20}{200\,\text{Hz}} = 100\,\text{ms}$$

### Windowed-Sinc Synthesis (Hamming Window):
The ideal impulse response is:

$$h_d[k] = \frac{\sin(2\pi f_c (k - M/2))}{\pi (k - M/2)}$$

Weighted by the Hamming window:

$$w[k] = 0.54 - 0.46 \cos\left(\frac{2\pi k}{M}\right), \quad 0 \le k \le M$$

Final coefficients:

$$h[k] = h_d[k] \cdot w[k]$$

---

## 3. Statistical Feature Vector
Each 1.0 s window (200 samples) across the 11 channels ($i = 1, \dots, 11$) is mapped to:

1. **Root Mean Square (RMS):**
   $$x_{\text{rms},i} = \sqrt{\frac{1}{N}\sum_{n=0}^{N-1} x_i^2[n]}$$

2. **Mean:**
   $$\bar{x}_i = \frac{1}{N}\sum_{n=0}^{N-1} x_i[n]$$

3. **Sample Variance:**
   $$\sigma_i^2 = \frac{1}{N}\sum_{n=0}^{N-1} (x_i[n] - \bar{x}_i)^2$$

Resulting in a 33-dimensional feature vector:

$$\mathbf{f} = \begin{bmatrix} x_{\text{rms},1} & \bar{x}_1 & \sigma^2_1 & \cdots & x_{\text{rms},11} & \bar{x}_{11} & \sigma^2_{11} \end{bmatrix}^T \in \mathbb{R}^{33}$$

---

## 4. Nearest-Neighbor Metric and Classification
The classified gesture label $\hat{g}$ is determined using the minimum squared Euclidean distance against calibrated prototype vectors $\mathbf{p}_g$:

$$\hat{g} = \arg\min_{g \in \{1,\dots,10\}} \|\mathbf{f} - \mathbf{p}_g\|_2^2 = \arg\min_{g} \sum_{j=1}^{33} (f_j - p_{g,j})^2$$

A rejection gate is enforced if the nearest distance exceeds threshold $\tau$:

$$\text{Decision} = \begin{cases} \hat{g}, & \text{if } \|\mathbf{f} - \mathbf{p}_{\hat{g}}\|_2 \le \tau \\ \text{REJECTED}, & \text{if } \|\mathbf{f} - \mathbf{p}_{\hat{g}}\|_2 > \tau \end{cases}$$
