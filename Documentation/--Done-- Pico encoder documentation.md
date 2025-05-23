# Note to TimL #
The formatting is in "Markdown"
### : header level 3
#### : header level 4
**This is bold text**
$$These delineate LaTex formatting as discussed$$


### Introduction
The purpose of this project is to develop a basic motor control board specifically designed to drive small, low-cost DC brushed motors using industrial signals. This innovative board is capable of accepting 0-10V inputs from a Programmable Logic Controller (PLC), allowing precise control over the commanded motor power. Additionally, it supports 24V Pulse Width Modulation (PWM) control signals directly for both forward and reverse motor operations, enhancing its versatility and applicability for various learning applications.

The motor control board is equipped with feedback capabilities, providing 0-10V outputs to the PLC for motor speed and position and a digital signal for motor direction. This allows a diverse range of process control learning opportunities. Furthermore, the board includes a position zeroing input from the PLC, enabling the control board to reset the motor position feedback to zero when desired to allow a homing routine to be used to establish absolute position control.

Overall, this project will provide a versatile motor controller that will facilitate a diverse range of learning opportunities.
### Using a Quadrature Encoder for Motor Direction, Speed, and Position
A quadrature encoder is a type of incremental encoder used to measure the direction, speed, and position of a rotating shaft. It provides two output signals, typically labeled A and B, which are 90 degrees out of phase. By analyzing these signals, the system can determine the shaft's state changes, allowing for precise measurement of its movement. Here's a detailed explanation of how state changes are used to determine direction, speed, and position with a quadrature encoder:

**Direction**

The direction of rotation is determined by the phase relationship between the A and B signals. There are four possible state transitions that occur when the encoder rotates:

1. **A leads B**: If the signal on channel A transitions before the signal on channel B, the encoder is rotating in one direction (e.g., clockwise).
2. **B leads A**: If the signal on channel B transitions before the signal on channel A, the encoder is rotating in the opposite direction (e.g., counterclockwise).

By continuously monitoring which channel leads the other, the system can accurately determine the direction of rotation.

**Speed**

The speed of rotation is calculated by measuring the frequency of the state changes (transitions) in the A and B signals. Since each transition represents a specific amount of shaft movement, a higher frequency of transitions indicates a higher speed of rotation. There are two primary methods to measure speed:

1. **Counting Transitions**: By counting the number of transitions in a given time period, the system can determine the speed. More transitions in a shorter time indicate higher speed.
2. **Time Between Transitions**: By measuring the time interval between consecutive transitions, the system can calculate speed. Shorter intervals between transitions indicate higher speed.

**Position**

The position is determined by counting the number of state changes or pulses from the encoder. Each full cycle of the A and B signals represents a fixed increment of rotation (usually a fraction of a degree or a millimeter, depending on the encoder's resolution). The position can be tracked by incrementing or decrementing a counter based on the direction of rotation.

1. **Incremental Counting**: Every transition (rising or falling edge) of both A and B signals is counted. Since there are four transitions per cycle (A rising, A falling, B rising, B falling), the system can achieve high resolution by counting each transition.
2. **Quadrature Decoding**: This method considers all four possible states (00, 01, 11, 10) in each cycle. By tracking the sequence of these states, the system can detect and count each increment of rotation.

**Example of State Transitions and Counting:**

Assume the initial state is 00 (A low, B low):
1. **00 to 01** (A low, B rising): One transition.
2. **01 to 11** (A rising, B high): Another transition.
3. **11 to 10** (A high, B falling): Another transition.
4. **10 to 00** (A falling, B low): Another transition.

This sequence repeats, and by tracking these transitions, the system determines the position by incrementing or decrementing a counter based on the direction of rotation.

In summary, by monitoring the state changes between the A and B signals, a quadrature encoder can accurately determine the direction, speed, and position of a rotating shaft. This information is crucial for precise control in various applications such as robotics, industrial automation, and motor control.
### Encoder Implementation
A Raspberry Pi Pico microcontroller was used in this project to read and process the encoder signal. The Pico uses Raspberry Pi’s RP2040 chip which has some unique capabilities, specifically, programmable I/O (PIO). PIO is a new piece of hardware developed for RP2040. It allows you to create new types of (or additional) hardware interfaces on your RP2040-based device. The PIO subsystem on RP2040 allows you to write small, simple programs for what are called PIO state machines, of which RP2040 has eight split across two PIO instances. A state machine is responsible for setting and reading one or more GPIOs, buffering data to or from the processor, and notifying the processor, via IRQ or polling, when data or attention is needed. These programs operate with cycle accuracy at up to system clock speed.

The pico C/C++ SDK (Software Development Kit) includes a PIO program example for reading a quadrature encoder. The example PIO tracks the A and B phase inputs and increments, decrements, or holds a count value depending on the state changes. The count value is accessible from a 32 bit register by the user program. The code in the CPU works with this PIO program to calculate speed, direction, and position.

The C code calculates the required motor values by reading the count register on a fixed period and calculating the difference from the previous read. Direction is determined based on whether the difference is positive or negative. Speed is calculated by dividing the difference by the time period of the loop. Position is maintained by adding the difference to a variable which is limited between zero and a maximum number of counts. The maximum number of counts represents the number of counts in a full revolution of the motor output shaft. This number can be adjusted in code based on motor gearing or for a specific application.
### Encoder Feedback Configuration
The values from the encoder have to be converted into a form that can be easily transmitted to the PLC. Specifically for this project, the goal is to provide feedback using 0-10v signals. It was determined that the easiest way to accomplish that was to output PWM signals from the Pico microcontroller that could then be integrated into a voltage signal. 

The speed feedback was the most straight forward. The PWM duty cycle was set to match the speed as a percentage from 0% to 100%. The maximum expected speed has to be programmed into the controller

**Add Calibration pot for adjusting max speed setting??**

The position feedback system uses PWM duty cycle to represent the percentage of a revolution. For example, a 180° rotation corresponds to a 50% duty cycle. This method provides high-resolution position feedback within a single revolution for the gear motor. However, for the non-geared motor, the resolution within a single revolution is poor.

To enhance the position feedback capability, the position is scaled by a factor of ten, providing feedback across ten revolutions of the motor. This approach allows for a broader indication of motor position with lower resolution over a larger rotation range, while still maintaining fine resolution within a single rotation, based on the PLC's analog input resolution.

### Testing results
#### TS-25GA370H-20 (Small Gear Motor)
This small motor’s gearbox results in excellent position feedback making it ideal for any precision application. The motor vendor stated a pulse resolution of 240 pulses per revolution and the the initial motor testing demonstrated a similar value (values from section 2.1.1):
$$\frac{1.111\ Kcyles}{second}=\frac{4.608\ revolutions}{second}$$
Solving for cycles per second yields: 
$$\frac{1,111\ cycles}{4.608\ revolutions}=241.1\frac{cycles}{revolutions}$$

Our initial testing is mostly consistent with the vendor provided spec, but when utilizing the encoder as described above, a state change per revolution number is needed. The cycles per second recorded during initial testing are for a single encoder signal. When reading the encoder using state changes, a resolution of four times the signal frequency is achieved. The result is that the the motor should have $$(240\times4)=960$$ state changes per revolution. The motor position accuracy was tested over ten rotations and it was found that the zero position drifted by almost 90° indicating that our 960 state changes per revolution value was inaccurate. Through trial and error, a value of 980 state changes per revolution was determined to be accurate across 10 turns. This means there is likely 245 pulses per revolution for each encoder signal. This error could be attributed to the vendor reporting a round value for the 20:1 gear reduction which may be actually closer to 20.4:1.

A maximum speed of 4700 state changes per second was recorded for this motor. This number is an approximation and could vary between motors. For this reason, a speed adustment will be provided to allow maximizing the resolution of the speed feedback to the PLC.
#### RS775-125 (Bare Motor)
This larger motor has significantly less encoder resolution because it does not have a gear reduction and because it it has almost half the resolution with the encoder itself. The motor vendor stated a pulse resolution of 240 pulses per revolution and the the initial motor testing demonstrated a similar value (values from section 1.2.1):
$$\frac{641\ cyles}{second}=\frac{91.667\  revolutions}{second}$$
Solving for cycles per second yields: 
$$\frac{641\ cycles}{91.667\ revolutions}=6.99\frac{cycles}{revolutions}$$
This tested value is much more consistent with the vendor’s specification than the gear motor. As described with the gear motor, the Pico counts the state changes per revolution which is four times the signal frequency. The result is 28 state changes per revolution. The resolution per state change is calculated as follows:
$$\frac{360\degree}{7\ pulses}\times\frac{pulses}{4\ state\ changes}=\frac{12.9\degree}{state\ change}$$
With this low resolution, it was simple to verify at the bench by checking that the feedback PWM signal returns to 0% duty cycle at the same point on each revolution. Because the resolution is so poor for this motor it will be better used for speed control applications instead of position control.

A maximum speed of less than 2700 state changes per second was recorded for this motor. As described previously, an adjustment potentiometer on the controller board will allow the speed signal to be set appropriately for each individual motor.
