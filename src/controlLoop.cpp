#include "controlLoop.h"
#include "control/velPid.h"
#include "device/quadEncoder.h"
#include "chassis/chassisModel.h"

#include "uart.h"

extern Semaphore velSem;
extern float leftVelTarget, rightVelTarget;

using namespace okapi;

void controlLoop() {
	okapi::QuadEncoder leftEnc(1, 2, true), rightEnc(3, 4);
	okapi::SkidSteerModel<3> model({1_m, 2_m, 3_m, 7_m, 8_m, 9_m}, leftEnc, rightEnc);
	okapi::VelPid leftVel(0.3, 0.1), rightVel(0.3, 0.1);

	while (1) {
		uint8_t packet_id = 0;
		int32_t value = 0;

		readUart(packet_id, value);
		switch (packet_id) {
			case 0x1: leftVel.setTarget(value); break;
			case 0x2: rightVel.setTarget(value); break;
		}

		leftVel.loop(leftEnc.get());
		rightVel.loop(rightEnc.get());

		model.tank(leftVel.getOutput(), rightVel.getOutput());

		writeUart(0xF1, leftEnc.get());
		writeUart(0xF2, rightEnc.get());

		taskDelay(20);
	}
}
