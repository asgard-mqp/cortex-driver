#include "controlLoop.h"
#include "control/velPid.h"
#include "device/quadEncoder.h"
#include "device/button.h"
#include "chassis/chassisModel.h"

#include "uart.h"

using namespace okapi::literals;

void controlLoop() {
	okapi::QuadEncoder leftEnc(1, 2, true), rightEnc(3, 4);
	okapi::SkidSteerModel<3> model({2_m, 3_m, 4_m, 7_m, 8_m, 9_m}, leftEnc, rightEnc);
	okapi::VelPid leftVel(0.3, 0.1), rightVel(0.3, 0.1);
	// okapi::Button switchMode(1, 8, JOY_DOWN), uartButton(1, 8, JOY_LEFT);

	writeUart(0xF5, 50505);

	float leftVal = 0, rightVal = 0;

	bool joystickMode = true, uartMode = false;

	while (1) {
		uint8_t packet_id = 0;
		int32_t value = 0;

		readUart(packet_id, value);
		switch (packet_id) {
			case 0x1: leftVal = value/360.0; break; //leftVel.setTarget(value/360.0); break;
			case 0x2: rightVal = value/360.0; break; //rightVel.setTarget(value/360.0); break;
		}

		if (packet_id)
			printf("[0x%02x] %d\n", packet_id, value);

		leftVel.loop(leftEnc.get());
		rightVel.loop(rightEnc.get());

		// toggle joystick / jetson control
		if (joystickGetDigital(1, 8, JOY_DOWN)) {
			joystickMode = true;
		} else if (joystickGetDigital(1, 8, JOY_UP)) {
			joystickMode = false;
		}

		if (joystickMode) {
			if (joystickGetDigital(1, 7, JOY_DOWN)) {
				model.tank(-65, 65); // if 7D is pressed, spin at a constant speed
			} else {
				model.tank(joystickGetAnalog(1, 3), joystickGetAnalog(1, 2), 10);
			}
		} else {
			// model.tank(leftVel.getOutput(), rightVel.getOutput());
			model.tank((leftVal/200.0)*127, (rightVal/200.0)*127);
		}

		if (joystickGetDigital(1, 8, JOY_LEFT))
			uartMode = true;

		// reset button for testing
		if (joystickGetDigital(1, 8, JOY_RIGHT)) {
			uartMode = false;
			leftEnc.reset();
			rightEnc.reset();
		}

		if (uartMode) {
			writeUart(0xF1, leftEnc.get());
			writeUart(0xF2, rightEnc.get());
		}

		taskDelay(15);
	}
}
