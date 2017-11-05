#include "controlLoop.h"
#include "control/pid.h"
#include "control/velMath.h"
#include "device/quadEncoder.h"
#include "device/button.h"
#include "chassis/chassisModel.h"
#include "util/mathUtil.h"
#include "util/timer.h"

#include "uart.h"

using namespace okapi::literals;
float kp = 0.025;
float ki = 0.0;
float kd = 0.0005;
void controlLoop() {
	okapi::IME leftIME(0), rightIME(1, true);
	okapi::SkidSteerModel<3> model({2_m, 3_m, 4_m, 7_m, 8_m, 9_m}, leftIME, rightIME);
	// okapi::Button switchMode(1, 8, JOY_DOWN), uartButton(1, 8, JOY_LEFT);
	okapi::Pid left(kp, ki, kd), right(kp, ki, kd);
	okapi::VelMath leftVel(okapi::imeTurboTPR), rightVel(okapi::imeTurboTPR);
	float leftPower = 0, rightPower = 0;

	writeUart(0xF5, 50505);
	float leftVal = 0, rightVal = 0;
	bool joystickMode = true, uartMode = false;
	leftVal = 75;
	while (1) {
		uint8_t packet_id = 0;
		int32_t value = 0;

		readUart(packet_id, value);
		switch (packet_id) {
			case 0x1: leftVal = value/360.0; break; //leftVel.setTarget(value/360.0); break;
			case 0x2: rightVal = value/360.0; break; //rightVel.setTarget(value/360.0); break;
			case 0x16: kp = value/65536.0; break;
			case 0x17: ki = value/65536.0; break;
			case 0x18: kd = value/65536.0; break;
		}
		left.setGains(kp,ki,kd);
		right.setGains(kp,ki,kd);
		printf("%1.4f %1.4f %1.4f \n",kp,ki,kd);


		if (packet_id) {
			printf("[0x%02x] %d\n", packet_id, value);
		}

		//Step IME and velocity
		// imeGet(leftIME, &leftIMEVal);
		// imeGet(rightIME, &rightIMEVal);
		// rightIMEVal *= -1;
		leftVel.step(leftIME.get());
		rightVel.step(rightIME.get());

		//Step PID
		left.setTarget(leftVal);
		right.setTarget(rightVal);
		leftPower += left.step(leftVel.getOutput());
		rightPower += right.step(rightVel.getOutput());

		//Bound integrators
		if (leftPower > 127)
			leftPower = 127;
		else if (leftPower < -127)
			leftPower = -127;
		if (rightPower > 127)
			rightPower = 127;
		else if (rightPower < -127)
			rightPower = -127;
		
		// printf("%1.2f,%1.2f\n", leftVel.getOutput(), rightVel.getOutput());

		// toggle joystick / jetson control
		if (joystickGetDigital(1, 8, JOY_DOWN)) {
			joystickMode = true;
		} else if (joystickGetDigital(1, 8, JOY_UP)) {
			joystickMode = false;
			// leftIMEVal = 0;
			model.resetSensors();
			// rightIMEVal = 0;
			left.reset();
			right.reset();
			leftPower = 0;
			rightPower = 0;
		}

		if (joystickMode) {
			if (joystickGetDigital(1, 7, JOY_DOWN)) {
				model.tank(-65, 65); // if 7D is pressed, spin at a constant speed
			} else {
				model.tank(joystickGetAnalog(1, 3), joystickGetAnalog(1, 2), 10);
			}
		} else {
			// model.tank(leftVel.getOutput(), rightVel.getOutput());
			// model.tank(leftPower, rightPower);
			model.leftTS(leftPower + 127.0 * leftVal/200.0);
			model.rightTS(rightPower + 127.0 * rightVal/200.0);
			fprintf(uart2, "E%1.2f,%1.2f\n",leftVel.getOutput(),leftVal-leftVel.getOutput());
		}

		if (joystickGetDigital(1, 8, JOY_LEFT))
			uartMode = true;

		// reset button for testing
		if (joystickGetDigital(1, 8, JOY_RIGHT)) {
			uartMode = false;
			// leftEnc.reset();
			// rightEnc.reset();
		}

		if (uartMode) {
			writeUart(0xF1, leftIME.get());//leftEnc.get());
			writeUart(0xF2, rightIME.get());//rightEnc.get());
		}

		taskDelay(15);
	}
}
