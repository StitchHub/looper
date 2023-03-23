/*
 * Copyright (C) 2013 Elboulangero <elboulangero@gmail.com>
 *
 * Arduino firmware for my guitar looper controller.
 * 
 * This file is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This file is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with This file. If not, see <http://www.gnu.org/licenses/>.
 */

#include <Firmata.h>



void sysex_callback(byte command, byte argc, byte*argv)
{
	Firmata.sendSysex(command, argc, argv);
}

void check_digital(void)
{
	/* Static variables, persistent across successive calls.
	 * If no initial value is given, static variables are initialized to zero.
	 */
	static byte prev_mask = B11000011;
	static byte prev_value[8];
	static byte inhibit[8];

	int i;
	byte value;
	byte mask = B11000011;

	/* Iterate over digital pins (only 4 pins are connected).
	 * Inhibiting is needed for some switches that flicker when pushed.
	 */
	for (i = 2; i < 6; i++) {
		if (inhibit[i]) {
			value = prev_value[i];
			--inhibit[i];
		} else {
			value = digitalRead(i);
			if (value != prev_value[i]) {
				prev_value[i] = value;
				inhibit[i] = 5;
			}
		}
		mask |= value << i;
	}

	/* Send data only if some values changed. */
	if (mask != prev_mask) {
		prev_mask = mask;
		Firmata.sendDigitalPort(0, mask);
	}
}

void check_analog(void)
{
	/* Static variables, persistent across successive calls.
	 * If no initial value is given, static variables are initialized to zero.
	 */
	static int prev_value[6];

	int i;
	int value;

	/* Iterate over analog pins (only 1 pin connected).
	 * Analog pin is always swinging between two values,
	 * we consider that value changed only if difference
	 * with previous value is above a threshold.
	 */
	for (i = 0; i < 1; i++) {
		value = analogRead(i);
		if (abs(value - prev_value[i]) > 5) {
			Firmata.sendAnalog(i, value);
			prev_value[i] = value;
		}
	}
}

void setup()
{
	Firmata.setFirmwareVersion(0, 1);
	Firmata.attach(START_SYSEX, sysex_callback);
	Firmata.begin(57600);
}

void loop()
{
	/* Check pins */
	check_digital();
	check_analog();

	/* Process input messages */
	while (Firmata.available()) {
		Firmata.processInput();
	}
}
