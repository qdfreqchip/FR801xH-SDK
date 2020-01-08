/*
 * Copyright (c) 2018, Sensirion AG
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * * Neither the name of Sensirion AG nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#include <stdbool.h>
#include "sht3x_arch_config.h"
#include "sht3x_sw_i2c_gpio.h"
//#include "user_sys.h"
#include "driver_gpio.h"
//#include "system.h"
#include "co_printf.h"
#include "sys_utils.h"
/*
 * INSTRUCTIONS
 * ============
 *
 * Implement all functions where they are marked as IMPLEMENT.
 * Follow the function specification in the comments.
 *
 * We use the following names for the two I2C signal lines:
 * SCL for the clock line
 * SDA for the data line
 *
 * Both lines must be equipped with pull-up resistors appropriate to the bus
 * frequency.
 */

/**
 * Initialize all hard- and software components that are needed to set the
 * SDA and SCL pins.
 */
void sensirion_init_pins(void) {
    // IMPLEMENT
     system_set_port_mux(GPIO_PORT_C, GPIO_BIT_6, PORTC6_FUNC_C6);
	 system_set_port_pull(GPIO_PC6, true);
	// system_set_port_pull(6 + (GPIO_PORT_D * 8), true);
	 gpio_set_dir(GPIO_PORT_C, GPIO_BIT_6, GPIO_DIR_OUT);


	 system_set_port_mux(GPIO_PORT_C, GPIO_BIT_7, PORTC7_FUNC_C7);
	 system_set_port_pull(GPIO_PC7, true);
	  //system_set_port_pull(7 + (GPIO_PORT_D * 8), true);
	 gpio_set_dir(GPIO_PORT_C, GPIO_BIT_7, GPIO_DIR_OUT);

}

/**
 * Release all resources initialized by sensirion_init_pins()
 */
void sensirion_release_pins(void) {
    // IMPLEMENT or leave empty if no resources need to be freed
}

/**
 * Configure the SDA pin as an input. With an external pull-up resistor the line
 * should be left floating, without external pull-up resistor, the input must be
 * configured to use the internal pull-up resistor.
 */
void sensirion_SDA_in(void) {
    // IMPLEMENT
	 gpio_set_dir(GPIO_PORT_C, GPIO_BIT_7, GPIO_DIR_IN);
}

/**
 * Configure the SDA pin as an output and drive it low or set to logical false.
 */
void sensirion_SDA_out(void) {
    // IMPLEMENT

	 gpio_set_dir(GPIO_PORT_C, GPIO_BIT_7, GPIO_DIR_OUT);
	gpio_portc_write(gpio_portc_read() & ~(BIT(GPIO_BIT_7)) );

}

/**
 * Read the value of the SDA pin.
 * @returns 0 if the pin is low and 1 otherwise.
 */
uint8_t sensirion_SDA_read(void) {
    // IMPLEMENT
    uint8_t SDARead = gpio_portc_read();
    return ((SDARead>>7)&0x01);
}

/**
 * Configure the SCL pin as an input. With an external pull-up resistor the line
 * should be left floating, without external pull-up resistor, the input must be
 * configured to use the internal pull-up resistor.
 */
void sensirion_SCL_in(void) {
    // IMPLEMENT
    gpio_set_dir(GPIO_PORT_C, GPIO_BIT_6, GPIO_DIR_IN);
}

/**
 * Configure the SCL pin as an output and drive it low or set to logical false.
 */
void sensirion_SCL_out(void) {
    // IMPLEMENT
    gpio_set_dir(GPIO_PORT_C, GPIO_BIT_6, GPIO_DIR_OUT);
	gpio_portc_write(gpio_portc_read() & ~(BIT(GPIO_BIT_6)) );
}

/**
 * Read the value of the SCL pin.
 * @returns 0 if the pin is low and 1 otherwise.
 */
uint8_t sensirion_SCL_read(void) {
    // IMPLEMENT
     uint8_t SCLRead = gpio_portc_read();
//	printf("SCLRead =0x%x\r\n",SCLRead);
    return ((SCLRead>>6)&0x01);

}

/**
 * Sleep for a given number of microseconds. The function should delay the
 * execution approximately, but no less than, the given time.
 *
 * The precision needed depends on the desired i2c frequency, i.e. should be
 * exact to about half a clock cycle (defined in
 * `SENSIRION_I2C_CLOCK_PERIOD_USEC` in `sensirion_arch_config.h`).
 *
 * Example with 400kHz requires a precision of 1 / (2 * 400kHz) == 1.25usec.
 *
 * @param useconds the sleep time in microseconds
 */
void sensirion_sleep_usec(uint32_t useconds) {
    // IMPLEMENT
    co_delay_100us(useconds/100);
}
