#include "Robot.h"


Robot::Robot(RobotManager* rm) : robotSwitch(NC) {
	this->rm = rm;
	owner = HROBOT;
	memset(robot_idle_counter,0,(size_t)sizeof(robot_idle_counter));

	if(start()) {
		setStatus(INIT);
	}
	else {
		setStatus(FATAL);
	}
}

Robot::Robot(PinName rsw,RobotManager* rm) :  robotSwitch(rsw){
	this->rm = rm;
	owner = HROBOT;
	memset(robot_idle_counter,0,(size_t)sizeof(robot_idle_counter));

	if(start())
		setStatus(INIT);
	else
		setStatus(FATAL);
}


Robot::~Robot() {
	for(int i = 0; i < NUM_AXES; i++) {
		if (axes[i]!=NULL)
			delete axes[i];
	}

}

bool Robot::start() {

	Axis::Params ap;
	I2C* i2c = new I2C(MC_DATA,MC_CLK);

	ap.enc_ch_a = ENC_BASE_CH_A;
	ap.enc_ch_b = ENC_BASE_CH_B;
	ap.sw = HSW_BASE;
	ap.count_home = BASE_COUNTS_HOME;
	ap.count_limit = BASE_COUNTS_MAX;
	ap.pid.p = BASE_PID_P;
	ap.pid.i = BASE_PID_I;
	ap.pid.d = BASE_PID_D;
	ap.pid.e = 0;
	axes[BASE] = new Axis(ap,i2c,MC_BASE_ADDR);

	ap.enc_ch_a = ENC_BODY_CH_A;
	ap.enc_ch_b = ENC_BODY_CH_B;
	ap.sw = HSW_BODY;
	ap.count_home = BODY_COUNTS_HOME;
	ap.count_limit = BODY_COUNTS_MAX;
	ap.pid.p = BODY_PID_P;
	ap.pid.i = BODY_PID_I;
	ap.pid.d = BODY_PID_D;
	axes[BODY] = new Axis(ap,i2c,MC_BODY_ADDR);

	ap.enc_ch_a = ENC_UPPER_ARM_CH_A;
	ap.enc_ch_b = ENC_UPPER_ARM_CH_B;
	ap.sw = HSW_UPPER_ARM;
	ap.count_home = UPPER_ARM_COUNTS_HOME;
	ap.count_limit = UPPER_ARM_COUNTS_MAX;
	ap.pid.p = UPPER_ARM_PID_P;
	ap.pid.i = UPPER_ARM_PID_I;
	ap.pid.d = UPPER_ARM_PID_D;
	axes[UPPER_ARM] = new Axis(ap,i2c,MC_UPPER_ARM_ADDR);
								
	ap.enc_ch_a = ENC_FOREARM_PITCH_CH_A;
	ap.enc_ch_b = ENC_FOREARM_PITCH_CH_B;
	ap.sw = HSW_FOREARM_PITCH;
	ap.count_home = FOREARM_PITCH_COUNTS_HOME;
	ap.count_limit = FOREARM_PITCH_COUNTS_MAX;
	ap.pid.p = FOREARM_PITCH_PID_P;
	ap.pid.i = FOREARM_PITCH_PID_I;
	ap.pid.d = FOREARM_PITCH_PID_D;
	axes[FOREARM_PITCH] = new Axis(ap,i2c,MC_FOREARM_PITCH_ADDR);
	
	ap.enc_ch_a = ENC_FOREARM_ROLL_CH_A;
	ap.enc_ch_b = ENC_FOREARM_ROLL_CH_B;
	ap.sw = HSW_FOREARM_ROLL;
	ap.count_home = FOREARM_ROLL_COUNTS_HOME;
	ap.count_limit = FOREARM_ROLL_COUNTS_MAX;
	ap.pid.p = FOREARM_ROLL_PID_P;
	ap.pid.i = FOREARM_ROLL_PID_I;
	ap.pid.d = FOREARM_ROLL_PID_D;
	axes[FOREARM_ROLL] = new Axis(ap,i2c,MC_FOREARM_ROLL_ADDR);

	ap.enc_ch_a = ENC_CLIP_CH_A;
	ap.enc_ch_b = ENC_CLIP_CH_B;
	ap.sw = HSW_CLIP;
	ap.count_home = CLIP_COUNTS_HOME;
	ap.count_limit = CLIP_COUNTS_MAX;
	ap.pid.p = CLIP_PID_P;
	ap.pid.i = CLIP_PID_I;
	ap.pid.d = CLIP_PID_D;
	axes[CLIP] = new Axis(ap,i2c,MC_CLIP_ADDR);

	unsigned int i=0;
	
	while (i<NUM_AXES && axes[i]!=NULL) {
		memset(&infoAxes_desired[i],0,(int)sizeof(Axis::Data));
		memset(&infoAxes_past[i],0,(int)sizeof(Axis::Data));
		i++;
	}

	if (i<NUM_AXES)	{
		Logger::logFatal("LRobot failed to initialize axis[%d]",i);
		stop();
		delete i2c;
		return false;
	}


	return true;
}


void Robot::setAxisVel(int axis,int vel) {
	//Logger::logDebug("Axis %d VEL: %d",axis,vel);
	infoAxes_desired[axis].vel = vel;
	axes[axis]->setStatus(Axis::FREE);
	
}

int Robot::getAxisVel(int axis) {

	return axes[axis]->getVel();
}

void Robot::setAxisPos(int axis,int pos) {
	axes[axis]->setPos(pos);
}

int Robot::getAxisPos(int axis) {
	return axes[axis]->getPos();
}

bool Robot::getAxisHsw(int axis) {
	return axes[axis]->getHsw();
}

void Robot::setAxisAccel(int axis, int accel) {
	infoAxes_desired[axis].accel = accel;
}

int Robot::getAxisAccel(int axis) {
	return axes[axis]->getAccel();
}

bool Robot::moveTo(int axis, int pos, bool force) {
	if(axes[axis]->isCalibrated() || force) {
		infoAxes_desired[axis].pos = pos;
		axes[axis]->setStatus(Axis::MOVING_TO);
		return true;
	}
	else
		return false;
}

void Robot::hold(int axis) {
	if (axis == -1) {
		for(int i = 0; i < NUM_AXES; i++) {
			moveTo(i,axes[i]->getPos(),true);
		}
	}
	else
		moveTo(axis,axes[axis]->getPos(),true);

}

bool Robot::stop() {
	//Logger::logMessage(cause);


	if(robotSwitch.is_connected() == 0) {
		bool v = true;
		for(int i = 0; i < NUM_AXES; i++) {
			v &= axes[i]->setVel(0);
		}
		if(v) {
			Logger::logInfo("LRobot stopped");
			return true;
		}
	}
	//else {
		//robotSwitch = true; NOT IMPLEMENTED
		//Logger::logInfo("LRobot powered off. Stopped"); NOT IMPLEMENTED
		//return true
	//}
	return false;

}

int Robot::getStatus() {
	return status;
}

void Robot::setStatus(int status) {
	 this->status = status;

	 switch(status) {

	 case INIT:
	 case IDLE:
		 hold(-1);
	 }
}

void Robot::setAxisPID(int axis, Axis::PID pid) {

	axes[axis]->setPID(pid);

}

Axis::PID Robot::getAxisPID(int axis) {

	return axes[axis]->getPID();
}

bool Robot::isCalibrated() {
	for (int i = 0; i < NUM_AXES; i++) {
		if(!axes[i]->isCalibrated())
			return false;
	}
	return true;
}

void Robot::performTick() {
	int st;
	int v;
	int a;
	int p;
	int dif;
	bool success = true;
/*
    if (status == IDLE) {

    	for (int i = 0; i < NUM_AXES; i++) {
    		p = axes[i]->getPos();

    		dif = infoAxes_past[i].pos - p;

    		if(abs(dif) > 10) {
    			robot_idle_counter = 0;
    			setStatus(MOVING);
    		}
    	}
    }
    else if (status == MOVING || status == MOVING_TO) {
    	for (int i = 0; i < NUM_AXES; i++) {
    		p = axes[i]->getPos();

    		dif = infoAxes_past[i].pos - p;

    		if(abs(dif) <= 10) {
    			robot_idle_counter++;
    			if(robot_idle_counter > 100) {
    				hold(-1);
    				setStatus(IDLE);
    			}
    		}
    		else {
    			robot_idle_counter = 0;
    		}
    	}
    }
*/
	//Logger::logDebug("Tick [%d]",LPC_TIM3->TC);
	if (status == MOVING_TO) {
		//Logger::logDebug("movingto");
		bool is_idle = true;
    	for (int i = 0; i < NUM_AXES; i++) {
    		p = axes[i]->getPos();

    		dif = infoAxes_desired[i].pos - p;

    		if(abs(dif) <= MAX_DIFF_POS) {
    			robot_idle_counter[i]++;
    			if(robot_idle_counter[i] > IDLE_MAX_COUNT) {
    				is_idle &= true;
    			}
    		}
    		else {
    			robot_idle_counter[i] = 0;
    			is_idle = false;
    		}
    	}
    	if(is_idle) {
    		setStatus(IDLE);
    	}
	}
	for (int i = 0; i < NUM_AXES; i++) {

		st = axes[i]->getStatus();
		v = axes[i]->getVel();
		//a = axes[i]->getAccel();
		p = axes[i]->getPos();
		int count;

		if(st == Axis::MOVING_TO) {
			count = 0;
			do {
			success &= axes[i]->setVel(infoAxes_desired[i].pos,infoAxes_past[i].pos);
			count++;
			}
			while (!success && count < MC_MAX_RETRY);
		}
		else {
			count = 0;
			do {
			success &= axes[i]->setVel(infoAxes_desired[i].vel);
			count++;
			}
			while (!success && count < MC_MAX_RETRY);
		}
		if(!success) {
			Logger::logWarning("Axis %d: failed to set vel.",i);
		}

		//Update past values for next PerformTick()
		//infoAxes_past[i].accel = a;
		infoAxes_past[i].pos = p;
		infoAxes_past[i].status = st;
		infoAxes_past[i].vel = v;
	}
	if(!success) {
		rm->mcSocketError();
	}


}
