# GPIO Pin Usage Summary
- Digital Inputs
 - GPIO 2 & 3 (PIN_AB): Quadrature encoder A and B phase
 - GPIO 13 (PIN_MOTOR_SEL): Motor selection DIP switch to GND (internal Pull-up)
 - GPIO 14 (PIN_HOME_LIMIT): Limit switch 
 - GPIO 15 (PIN_HOME_CMD): Home command input

- Analog Inputs
 - GPIO 26 (PIN_SPD_CMD_ADC): Speed command input (0-10v from PLC)
 - GPIO 27 (PIN_SPD_CAL_ADC): Speed calibration input (0–3.3 V off Pot)

- Digital Outputs
 - GPIO 16 (PIN_FWD): Forward direction indicator
 - GPIO 17 (PIN_REV): Reverse direction indicator

- PWM Outputs
 - GPIO 18 (PIN_PWM_POS): Position feedback (0–100%)
 - GPIO 19 (PIN_PWM_SPD): Speed feedback (0–100%)
 - GPIO 20 (PIN_PWM_POSX10): Position ×10 feedback (0–100%)
 - GPIO 21 (PIN_PWM_SPD_CMD): Speed command output to H-bridge


