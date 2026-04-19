# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is the "Super Fantastic Motor Project" — an H-Bridge motor control circuit project from Idaho State University. It spans hardware design, embedded software, mechanical parts, and an academic research paper. The project controls a DC motor using an H-Bridge, with a Raspberry Pi Pico reading a quadrature encoder for speed/position feedback.

## Repository Structure

- **Software/** — Embedded code for the Raspberry Pi Pico
  - `quadrature_encoder/` — C code + PIO program using the Pico C/C++ SDK. Built with CMake. Reads encoder via PIO state machine and outputs PWM signals for speed/position feedback plus direction LEDs.
  - `EncoderCircuitPy.py` — Early CircuitPython prototype (abandoned due to performance limitations)
- **PCB/** — KiCad PCB designs
  - `Anderson Projects/Pico_H-bridge_complete/` — Main board: Pico + H-Bridge, versioned Gerber outputs in `production/`
  - `Anderson Projects/DB37 passthrough/` — DB37 connector interface box PCB
  - Legacy boards: H_Bridge, MotorInterfaceDiscrete, MotorInterfacePiPico
- **Documentation/** — LaTeX research paper (`main.tex` with `sections/` subfiles) and reference documents
- **Mechanical/** — 3D print files and mechanical drawings (laser table, motor coupler, stoplight project)

## Build Commands

### Pico Firmware (C/C++ SDK)
The quadrature encoder firmware uses CMake with the Pico C/C++ SDK. Typical build on a Raspberry Pi:
```bash
cd Software/quadrature_encoder
mkdir build && cd build
cmake ..
make
```

### LaTeX Paper
```bash
cd Documentation
pdflatex main.tex
bibtex main
pdflatex main.tex
pdflatex main.tex
```

## Key Technical Details

- **Encoder pins**: Phase A on GP2, Phase B on GP3 (adjacent pin required by PIO)
- **Output pins**: FWD LED (GP16), REV LED (GP17), Position PWM (GP18), Speed PWM (GP19), PositionX10 PWM (GP20)
- **Calibration constants**: `speed_max = 4700` (encoder changes/sec), `position_max = 980` (counts per revolution, 240 for gearmotor variant)
- **Sample rate**: 100ms loop (`sleep_ms(100)`)
- PCB designs use **KiCad** — `.kicad_sch`, `.kicad_pcb`, `.kicad_pro` files

## Git Conventions

- The `.gitignore` is tuned for KiCad temporary files and LaTeX build artifacts
- Contains a git submodule at `Documentation/EncoderInterfaceDiscrete/.template` (lesson plan template)
- Contribution workflow: fork, branch, PR (see `Contribute.md`)
