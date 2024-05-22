/**
 * Copyright (c) 2023 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/timer.h"
#include "hardware/pwm.h"


#include "quadrature_encoder.pio.h"

//
// ---- quadrature encoder interface example
//
// the PIO program reads phase A/B of a quadrature encoder and increments or
// decrements an internal counter to keep the current absolute step count
// updated. At any point, the main code can query the current count by using
// the quadrature_encoder_*_count functions. The counter is kept in a full
// 32 bit register that just wraps around. Two's complement arithmetic means
// that it can be interpreted as a 32-bit signed or unsigned value, and it will
// work anyway.
//
// As an example, a two wheel robot being controlled at 100Hz, can use two
// state machines to read the two encoders and in the main control loop it can
// simply ask for the current encoder counts to get the absolute step count. It
// can also subtract the values from the last sample to check how many steps
// each wheel as done since the last sample period.
//
// One advantage of this approach is that it requires zero CPU time to keep the
// encoder count updated and because of that it supports very high step rates.
//

void pwm_pin_init(uint pin){
    uint slice_num = pwm_gpio_to_slice_num(pin);
    
    // Get some sensible defaults for the slice configuration. By default, the
    // counter is allowed to wrap over its maximum range (0 to 2**16-1)
    pwm_config config = pwm_get_default_config();
    // Set divider, reduces counter clock to sysclock/this value
    pwm_config_set_clkdiv(&config, 8.f);
    // Load the configuration into our PWM slice, and set it running.
    pwm_init(slice_num, &config, true);

}

int main() {
    int new_value, delta, old_value = 0;

    // Base pin to connect the A phase of the encoder.
    // The B phase must be connected to the next pin
    const uint PIN_AB = 2;
    const uint PIN_FWD = 16;
    const uint PIN_REV = 17;
    const uint PIN_PWM_POS = 18;
    const uint PIN_PWM_SPD = 19;
    const uint PIN_PWM_POSx10 =20;
    
    // max AB encoder changes per second
    const double speed_max = 4700;
    // max counts = 7 for 775 motor and 240 for gearmotor
    const double position_max = 980;
    
    double speed = 0;
    double position = 0;
    double positionX10 = 0;

    // Configure LED direction pins
    gpio_init(PIN_FWD);
    gpio_set_dir(PIN_FWD, GPIO_OUT);
    gpio_init(PIN_REV);
    gpio_set_dir(PIN_REV, GPIO_OUT);

    // Configure PWM outputs (Position and Velocity)
    gpio_set_function(PIN_PWM_POS, GPIO_FUNC_PWM);
    gpio_set_function(PIN_PWM_SPD, GPIO_FUNC_PWM);
    gpio_set_function(PIN_PWM_POSx10, GPIO_FUNC_PWM);

    // Initiallize the PWM slice with appropriate config
    pwm_pin_init(PIN_PWM_POS);
    // Initialize the PWM slice for the x10 Position
    pwm_pin_init(PIN_PWM_POSx10);

    // If you need serial
    stdio_init_all();

    // Configure the PIO program
    PIO pio = pio0;
    const uint sm = 0;

    // we don't really need to keep the offset, as this program must be loaded
    // at offset 0
    pio_add_program(pio, &quadrature_encoder_program);
    quadrature_encoder_program_init(pio, sm, PIN_AB, 0);

    while (1) {
        // note: thanks to two's complement arithmetic delta will always
        // be correct even when new_value wraps around MAXINT / MININT
        new_value = quadrature_encoder_get_count(pio, sm);
        delta = new_value - old_value;
        old_value = new_value;

	// speed in counts/second 
	speed = abs((double)delta)/0.1f;
	
	position += delta;
	if (position > position_max){
	    position -= position_max;
	}else if (position < 0){
	    position = position_max + position;
	}
	
	// positionX10 used to check calibration over 10 revs
	positionX10 += delta;
	if (positionX10 > position_max * 10){
	    positionX10 -= (position_max * 10);
	}else if (positionX10 < 0){
	    positionX10 = (position_max * 10) + positionX10;
	}
	
	pwm_set_gpio_level(PIN_PWM_SPD, speed *65535.f/speed_max);
	pwm_set_gpio_level(PIN_PWM_POS, position *65535.f/position_max);
	pwm_set_gpio_level(PIN_PWM_POSx10, positionX10 *65535.f/(position_max * 10));
	
	if(delta > 0){
	    gpio_put(PIN_FWD, 1);
	    gpio_put(PIN_REV, 0);
	}else if (delta < 0){
	    gpio_put(PIN_FWD, 0);
	    gpio_put(PIN_REV, 1);
	}else {
	    gpio_put(PIN_FWD, 0);
	    gpio_put(PIN_REV, 0);
	}

	// Serial is on GP0 and GP1 by default
        printf("position %8d, delta %6d\n", new_value, delta);
        sleep_ms(100);
    }
}

