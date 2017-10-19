#include "controlLoop.h"
#include "control/velPid.h"
#include "device/quadEncoder.h"
#include "chassis/chassisModel.h"
#include "writeUart.h"

extern Semaphore velSem;

void controlLoop() {
	using namespace okapi;

	QuadEncoder leftEnc(1, 2, true), rightEnc(3, 4);
	SkidSteerModel<3> model({2_m,3_m,4_m, 5_m,6_m,7_m}, leftEnc, rightEnc);
	VelPid leftVel(0.3, 0.1), rightVel(0.3, 0.1);

	while (1) {
		semaphoreTake(velSem, 15);
		leftVel.setTarget(0);
		rightVel.setTarget(0);
		semaphoreGive(velSem);

		leftVel.loop(leftEnc.get());
		rightVel.loop(rightEnc.get());

		model.tank(leftVel.getOutput(), rightVel.getOutput());

		writeUart(0xF1, leftEnc.get());
		writeUart(0xF2, rightEnc.get());

		taskDelay(15);
	}
}
