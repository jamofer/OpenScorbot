/*
 * Axis.h
 *
 *  Created on: 8 de oct. de 2015
 *      Author: Javier
 */

#ifndef AXIS_H_
#define AXIS_H_
#include "RobotParams.h"

class Axis {

public:


    struct PID {
    	float p;
    	float i;
    	float d;
    	int e;
    };

    struct Params {
    		int count_limit;
    		int count_home;
    		int pos;
    		PID pid;
    };

/*												*/
/*  Scope: HRobot owner							*/
/* Values:										*/
/* 	*FREE. 		Velocity isn't regulated.		*/
/* 	*MOVE_TO.	Velocity PID based regulation.	*/
/* 												*/
    enum Status {
    	FREE 	= 0x00,
		MOVE_TO = 0x10
    };

//    enum CalibDir {
//    	DEFAULT = 0x00,
//    	CW = 0x01,
//		ACW = 0x02
//    };

	Axis();
	Axis(Params ap);
	Axis(Axis* ax);
	~Axis();

	void setVel(int vel);
	void setVel(int desired_pos, int past_pos);
	int getVel();

	void setPos(int pos);
	int getPos();

	void setAccel(int accel);
	int getAccel();

	void setPID(PID pid);
	PID getPID();

	void setHsw(bool hsw);
	bool getHsw();

	void setCalibDir(int i);
	int getCalibDir();

	bool isCalibrated();
	void setCalibrated(bool c);

	int getCountHome();
	int getCountLimit();

	int getStatus();
	void setStatus(int s);

	Params getParams();
	void setParams(Params p);

	void copyFromAxis(Axis* ax);


private:
	int status;
	PID pid;
	int count_limit;
	int count_home;
	int pos;
	int vel;
	int accel;
	bool hsw;
	bool calibrated;
	int calib_dir;
	//unsigned int tick_counter = 0;
};

#endif /* AXIS_H_ */
