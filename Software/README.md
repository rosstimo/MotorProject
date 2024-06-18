
Software developed for this project consists of code for a Raspberry Pi Pico used to read the motor encoder and provide feedback signals for Speed 
and Position of the motor.

CircuitPython was attempted first, but quickly was determined to be inadequate for the frequency of the encoder pulses. Raspberry Pi Foundation 
provides an SDK for using C or C++ on the Pico which is a compile code and therefore much faster. The Pico C/C++ SDK also allows programming the 
Pico PIO (Programmable I/O) which are dedicated state machines separate from the Pico processors. PIO can also be programmed in CircuitPython and 
MicroPython, but the Pico C/C++ SDK already included an example for reading a quadrature encoder.

The quadrature_encoder folder is a copy from ~/pico/pico_projects on the Pi400 used to program the Pico. The C/C++ SDK installs on the Pi400 with 
all examples in a pico folder. The quadrature_encoder folder contains the required CMake file as well as the C code and PIO program.
