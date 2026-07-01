# 🚀 Portable Intelligent Circuit Tester

[![Platform](https://img.shields.io/badge/Platform-STM32F407-blue.svg)](https://www.st.com/en/microcontrollers-microprocessors/stm32f407-147.html)
[![Language](https://img.shields.io/badge/Language-C/C++-orange.svg)](https://en.cppreference.com/)
[![Toolchain](https://img.shields.io/badge/Toolchain-Keil%20MDK--ARM-green.svg)](https://www.keil.com/)
[![License](https://img.shields.io/badge/License-MIT-lightgrey.svg)](LICENSE)

[English](README.md) | [简体中文](README_CN.md)

An open-source **Portable Intelligent Circuit Tester** based on the **STM32F407VET6** microcontroller. This instrument is designed to fully automate the identification of two-terminal circuit "black box" topologies, calculation of internal parameters, and the measurement of independent two-terminal components with a single-key operation.

This project provides a complete solution including hardware schematics, PCB layouts, mechanical panels, and modular firmware code. It is suitable for electronic engineering hobbyists and university course design references.

---

## 📸 Gallery

### Overall Appearance
![Overall Appearance](Photos/整体外观.png)

### Internal Structure & PCB Workmanship
| Internal View 1 | Internal View 2 |
| :---: | :---: |
| ![Internal View 1](Photos/内部1.jpg) | ![Internal View 2](Photos/内部2.jpg) |

---

## 📂 Directory Structure

```text
Portable-Circuit-Tester/
├── Docs/               # Project Documents & Design Drawings
│   ├── 电路原理图.pdf                   # Core schematic diagram
│   └── 面板.pdf                         # Front panel silk screen design (PDF)
├── Hardware/           # Hardware Design Files (EasyEDA Pro)
│   ├── 黑箱检测仪.epro                  # Tester schematics & PCB layout project
│   ├── 面板设计文件.epanm               # Front panel screen print source file
│   └── PCB制板文件.zip                  # Gerber files for PCB manufacturing
├── Simulation/         # Circuit Simulation Files (Multisim 14)
│   ├── RC RL串联.ms14                   # RC/RL voltage divider simulation
│   ├── 低功耗&开关(1).ms14              # Low-power soft latch switch circuit simulation
│   ├── 提高 电阻接地 C .ms14            # Grounded resistor & capacitor frequency response simulation
│   ├── 提高 电阻接地 L.ms14            # Grounded resistor & inductor frequency response simulation
│   ├── 提高 黑箱接地 C .ms14            # Grounded black box & capacitor frequency response simulation
│   ├── 提高 黑箱接地 L.ms14            # Grounded black box & inductor frequency response simulation
│   └── 电路仿真（频域）.ms14            # System-wide frequency domain response analysis
├── Data/               # Sweep Data Records & Analysis
│   ├── 数据记录 测量仪.xlsx             # Measured gain-frequency data sheets
│   └── 过渡带仿真记录.xlsx              # Math simulations and calculations for transition-band algorithms
├── Firmware/           # Embedded Firmware Code (Keil MDK-ARM)
│   └── Black Box/                      
│       ├── User/                       # Application logic (Highly structured & modular)
│       │   ├── Src/                    # Source files (black_box.c, extend.c, adc_fft_rms.c)
│       │   └── Inc/                    # Header files (black_box.h, extend.h, adc_fft_rms.h)
│       ├── Hardware/                   # Hardware Abstraction Drivers
│       │   ├── Src/                    # Source files (ad9833.c, oled.c, gui.c, lcd_driver.c, font96.c)
│       │   └── Inc/                    # Header files and font maps
│       └── MDK-ARM/                    # Keil Project directory (TFTLCD.uvprojx)
└── README.md           # Default English documentation
```

---

## 🌟 Core Features

*   **One-Key Automated Measurement**: Complete testing and analysis processes within 5 seconds without manual intervention.
*   **Circuit Black Box Mode**:
    *   **Topology Recognition**: Automatically identifies the internal structure as "Pure Resistor", "RC Series", "RC Parallel", "RL Series", "RL Parallel", or "No Black Box (NULL)".
    *   **Parameter Derivation**: Calculates the value of the internal fixed resistor $R$ ($10\Omega \sim 10\text{k}\Omega$) and the unknown component $X$ ($Rx$, $Cx$, or $Lx$).
*   **Component Measurement Mode**:
    *   **Auto Component Classification**: Identifies if the component is a Resistor, Capacitor, Inductor, Diode, or Open-Circuit (UNKNOWN).
    *   **Auto-Ranging Resistor Measurement**: Switches between 5 reference resistor ranges ($30.3\Omega \sim 475\text{k}\Omega$) via relays, allowing high-accuracy measurement from $10\Omega \sim 1\text{M}\Omega$.
    *   **ESR Measurement**: Measures capacitance ($1\text{nF} \sim 100\mu\text{F}$) and inductance ($100\mu\text{H} \sim 10\text{mH}$), and extracts their Equivalent Series Resistance (ESR, $0 \sim 30\Omega$) using a dual-frequency sweep.
    *   **Diode Pin Polarities & Forward Voltage**: Automatically determines pin polarities and measures forward voltage drop $V_f$ under safe current limits ($<10\text{mA}$).
*   **Ultra-Low Power Management**:
    *   Powered by Lithium battery or Micro-USB 5V.
    *   Supports single-key power-on, auto-shutdown after 30s of inactivity, and manual long-press shutdown (3s).
    *   Standby current is below **$0.1\text{mA}$**.

---

## 🛠️ Operating Principles & Algorithms

### 1. Large-Class Classification via Multi-Frequency Voltage Divider
By coupling the **AD9833 DDS signal generator**, the system measures the initial gain ratio $Au = V_{out}/V_{in}$ at $10\text{Hz}$ (quasi-DC) and $1.2\text{MHz}$ (high-frequency limit).
*   **Increasing Gain Response** ── Classified as **RC Network**. The system switches the reference ground resistor to **$1020\Omega$** and initiates the log frequency sweep.
*   **Decreasing Gain Response** ── Classified as **RL Network**. The system switches the reference ground resistor to **$30.5\Omega$** and initiates the sweep.
*   **Gain Fluctuation < 0.05** ── Classified as **Pure Resistor**.

### 2. Transition-Band Midpoint Analytical Algorithm
After broad classification, the system generates 18 logarithmically spaced sweep points and plots the amplitude-frequency response curve on the TFT screen.
Instead of the traditional $-3\text{dB}$ cutoff search (which is highly sensitive to discrete sampling resolution), the algorithm finds the frequency point $f_x$ closest to the midpoint gain $Au_{mid} = (Au_{min} + Au_{max})/2$ in the transition band. Inserting $f_x$ and $Au$ into the analytic impedance divider equation solves for $C$ or $L$ directly. This method compresses measurement errors to **$\le 5\%$**.

### 3. Dual-Frequency Method for ESR Extraction
For larger capacitors and lossy inductors where high-frequency flat bands are unreachable, the system extracts two points in the transition band ($f_1, f_2$) and their respective gains ($n_1, n_2$), and solves the quadratic impedance equations simultaneously:
$$\text{ESR} = \sqrt{\frac{R_1^2}{n_1^2} - X_{C1}^2} - R_1$$
This decouples the ESR ($R$) from the reactive components ($C$ or $L$), removing the measurement errors introduced by the loss tangent.

---

## 🚀 Technical Highlights & Debugging Challenges

*   **DMA Buffer Corruption Resolution (HW-SW Co-design)**:
    Since ADC sampling is triggered by hardware timers and streamed via DMA circular buffer, computing RMS values right after changing the DDS output frequency causes calculation errors due to the mixed frequency components in the buffer. The team solved this by calculating the buffer update period $T_{min} = \text{BufferSize} / F_{sample} \approx 160\text{ms}$ and introducing a **$165\text{ms}$ settling delay** after each frequency hop, shrinking sweep error from $>20\%$ to **$\le 5\%$**.
*   **Transient Settling Optimization for High-Resistance Measurements**:
    During high-value resistor measurements, the voltage reading took up to 5 seconds to stabilize, leading to inaccurate results. This was caused by the large time constant $\tau = R_x \cdot C$ of the coupling capacitor ($C$) in the bias circuit. By **replacing the coupling capacitor with a 100nF (104) capacitor**, $\tau$ dropped to the **$10\text{ms}$ range**, reducing the settling time to under $50\text{ms}$. This maintains DC blocking while achieving fast and accurate DC voltage readings.
*   **DDS High-Frequency Amplitude Compensation**:
    DDS generators experience natural amplitude decay at high frequencies. The firmware implements a segmented amplitude correction table (`AD9833_AmpSet`), dynamically boosting the output voltage at higher frequency bands to maintain stable stimulus signals.
*   **Logic Level Mismatch Bypass for Low-Power Control**:
    The shutdown chip's shutdown trigger pin (OPT) requires a $0.7V_{cc} = 3.5\text{V}$ threshold, which cannot be triggered by the MCU's 3.3V GPIO. The team bypassed this limitation by **software-pulling the PA0 pin (wired to the physical key) low for 3 seconds**, simulating a physical button press to trigger shutdown, achieving a standby current **$\le 0.1\text{mA}$**.

---

## 💻 Firmware Setup & Compile Guide

### 1. Software Environment
*   **Keil uVision 5** (MDK-ARM) IDE.
*   **Keil.STM32F4xx_DFP** device family pack.

### 2. Compilation & Flashing Steps
1.  Double-click `Firmware/Black Box/MDK-ARM/TFTLCD.uvprojx` to open the Keil project.
2.  Click **Rebuild** in Keil (the directory is cleaned and preconfigured with the new relative paths, compiling out-of-the-box).
3.  Connect your ST-Link or DAP-Link to the onboard SWD debug port, and click **Download** to flash the code into the STM32F407VET6.
4.  Power on the board. The OLED screen will play the boot animation and enter the measurement mode.

---

## 🤝 Contributors & Credits
*   **Hardware Design & Casing Layout**: Student B (YYYYYYYY@example_univ.edu.cn)
*   **Firmware Algorithms & Logic Coding**: Student A (XXXXXXXX@example_univ.edu.cn)
*   **System Testing & Data Log Sheets**: Student C (ZZZZZZZZ@example_univ.edu.cn)
*   **Project Advisor**: Advisor A

Contributions, forks, and suggestions are welcome! Please open an issue to discuss any modifications.