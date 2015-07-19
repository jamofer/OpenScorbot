#include "Axis.h"
#include "Logger.h"

Axis::Axis() : hsw(NC) {
	mc = NULL;
	enc = NULL;
	count_home = -1;
	count_limit = -1;
	vel = 0;
	accel = 0;
	status = Status::FREE;
	calibrated = false;
	memset(&pid,0,(int)sizeof(Axis::PID));

}

Axis::Axis(PinName a, PinName b, PinName sw, I2C* i2c, int addr) : hsw(sw) {

	enc = new QEI(a,b,NC);
	mc = new MotorControllerI2C(i2c,addr);
	count_home = -1;
	count_limit = -1;
	vel = 0;
	accel = 0;
	status = Status::FREE;
	calibrated = false;
	memset(&pid,0,(int)sizeof(Axis::PID));

}

Axis::Axis(Params ap, I2C* i2c, int addr) : hsw(ap.sw) {

	enc = new QEI(ap.enc_ch_a,ap.enc_ch_b,NC);
	count_home = ap.count_home;
	count_limit = ap.count_limit;
	pid = ap.pid;
	mc = new MotorControllerI2C(i2c,addr);
	vel = 0;
	accel = 0;
	status = Status::FREE;
	calibrated = false;


}

Axis::~Axis() {
	delete enc;
	delete mc;
	
}

bool Axis::setVel(int vel) {

	bool vel_changed = false;
	bool dir_changed = false;
	int last_dir;
	int v = abs(vel);
	int dir;
	bool success = true;

	//Limit vel at max value
	if(vel > MAX_PWR)
		vel = MAX_PWR;
	else if(vel < -MAX_PWR)
		vel = -MAX_PWR;

/*
	//Get last direction
	if(this->vel > 0)
		last_dir = 0x01; //Forward
	else if (this->vel < 0)
		last_dir = 0x02; //Backward
	else
		last_dir = 0x00; //Pause
*/
	//Get direction
	if(vel > 0)
		dir = 0x01; //Forward
	else if (vel < 0)
		dir = 0x02; //Backward
	else
		dir = 0x00; //Pause

	if(vel != this->vel) { //Commit new vel if changed
		success &= mc->setVel(v);
		success &= mc->sendCommand(dir);
	}

	if(success) { //If all went ok, update local value
		this->vel = vel;
	}
	
	return success;
	
	
}

bool Axis::setVel(int desired_pos, int past_pos) {
	int vel;
	int actual_pos = getPos();

	int dif = desired_pos - actual_pos; //counts remaining...
	int dif_p = actual_pos - past_pos;//counts[x-1]
	pid.e += dif; //Error

	vel = (int)(pid.p*dif + pid.i * pid.e + pid.d * dif_p + 0.5f); //PID algorithim


	return setVel(vel);

}

int Axis::getVel(bool force_update) {

	if (force_update) {
		int v = mc->getVel();

		int dir = mc->getCommand();

		switch(dir) {

			case 0x02:
				v = -v;
				break;
			case 0x00:
				v = 0;
				break;
			case 0x01:
				break;
			default:
				v = BAD_VALUE;
				break;
		}
		
		if(v <= MAX_PWR || v >= -MAX_PWR)
			vel = v;
	}

	return vel;

}

bool Axis::setAccel(int accel) {

	if (accel == this->accel)
		return true;
	else if(accel > 0xFF)
		accel = 0xFF;
	else if (accel < 0)
		accel = 0;


	if(mc->setAccel(accel)) {
		this->accel = accel;
		return true;
	}
	else {
		return false;
	}
}

int Axis::getAccel(bool force_update) {
	if(force_update) {
		accel = mc->getAccel();
	}
	return accel;
}

void Axis::setPos(int pos) {
	enc->setPulses(pos);
	calibrated = true;
}

int Axis::getPos() {
	return enc->getPulses();

}

bool Axis::getHsw() {
	return hsw;
	
}

void Axis::setPID(PID pid) {
	this->pid = pid;
}

Axis::PID Axis::getPID() {
	return pid;
}

void Axis::setStatus(int status) {
	this->status = status;
	pid.e = 0;
}

int Axis::getStatus() {
	return status;
}

bool Axis::isCalibrated() {
	return calibrated;
}
