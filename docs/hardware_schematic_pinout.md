# Hardware Schematics & Pinout Specifications

## 1. System Overview
The DSP-Based Wearable Communication System is built around the **STMicroelectronics STM32F407VG** (ARM Cortex-M4, 168 MHz) microcontroller with hardware floating-point acceleration.

---

## 2. Pin Mapping Table

| Subsystem | Component | Pin / Channel | Interface | Description |
|---|---|---|---|---|
| **Gesture Input** | Flex Sensor 1 (Thumb) | `PA0` (ADC1_IN0) | Analog (12-bit) | Voltage divider with 47 kΩ fixed resistor |
| **Gesture Input** | Flex Sensor 2 (Index) | `PA1` (ADC1_IN1) | Analog (12-bit) | Voltage divider with 47 kΩ fixed resistor |
| **Gesture Input** | Flex Sensor 3 (Middle) | `PA2` (ADC1_IN2) | Analog (12-bit) | Voltage divider with 47 kΩ fixed resistor |
| **Gesture Input** | Flex Sensor 4 (Ring) | `PA3` (ADC1_IN3) | Analog (12-bit) | Voltage divider with 47 kΩ fixed resistor |
| **Gesture Input** | Flex Sensor 5 (Pinky) | `PA4` (ADC1_IN4) | Analog (12-bit) | Voltage divider with 47 kΩ fixed resistor |
| **Motion Tracking** | MPU-6050 IMU | `PB6` (SCL), `PB7` (SDA) | I2C (400 kHz) | 3-axis accelerometer + 3-axis gyroscope |
| **Motion Interrupt** | MPU-6050 INT | `PB5` (EXTI5) | Digital Input | Data-ready interrupt at 200 Hz |
| **Audio Input** | Knowles SPH0645LM4H | `PB12` (WS), `PB13` (CK), `PB15` (SD) | I2S Protocol | 24-bit audio stream at 16 kHz |
| **Speech Output** | PAM8403 Class-D Amp | `TIM1_CH1` (`PE9`) | 62.5 kHz PWM | Drives 0.5 W dynamic speaker |
| **Bone Conduction** | TDA2822M Amplifier | `PA4` (DAC_OUT1) | 12-bit DAC | Drives Dayton Audio TT25-8 transducer |
| **Power Supply** | AMS1117-3.3 | 3.3V Rail, GND | Power Bus | Regulated 3.3V rail from 3.7V 2000 mAh LiPo |

---

## 3. Voltage Divider Topology (Flex Sensors)

```
        +3.3V Rail
           |
           |
       [ 47 kΩ ]  Fixed Reference Resistor
           |
           +-----> ADC Channel (0.6V to 2.7V dynamic range)
           |
       [  R_flex ]  Spectra Symbol Flex Sensor (10 kΩ to 110 kΩ)
           |
          GND
```

---

## 4. Power Budget Breakdown

| Component | Operating Voltage | Current Consumption | Power Consumption |
|---|---|---|---|
| STM32F407VG (168 MHz active) | 3.3 V | 30.3 mA | 100 mW |
| MPU-6050 6-Axis IMU | 3.3 V | 1.5 mA | 5 mW |
| Knowles SPH0645 MEMS Mic | 3.3 V | 0.6 mA | 2 mW |
| PAM8403 Speaker Amp (active bursts) | 5.0 V (boosted) | 40.0 mA | 200 mW |
| Dayton TT25-8 Bone Transducer | 3.3 V | 22.7 mA | 75 mW |
| **Total Active Power Consumption** | — | — | **~382 mW** |

With a standard **3.7V 2000 mAh (7.4 Wh)** lithium-polymer battery, the continuous operational battery life is approximately **14 to 19 hours**.
