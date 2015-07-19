#ifndef Axis_H
#define Axis_H

/**
 * Includes
 */
#include "mbed.h"
#include "QEI.h"
#include "MotorControllerI2C.h"
#include "RobotParams.h"




/**
 * LRobot Axis.
 */
class Axis {




public:
	
	enum Status {
		FREE = 0x00,
		MOVING_TO = 0x01
	};
    struct PID {
    	float p;
    	float i;
    	float d;
    	int e;
    };

    struct Params {
    		PinName enc_ch_a;
    		PinName enc_ch_b;
    		PinName sw;
    		int count_limit;
    		int count_home;
    		PID pid;
    };

    struct Data {
    	int vel;
    	int pos;
    	int accel;
    	int status;

    };

	Axis();

    /**
     * Constructor.
     *
     * Saves i2c instance and address for MotorControllerI2C communication.
     *
     * Setups PID and QEI parameters for the desired axis.
     *
     * @param a        Desired axis encoder channel A pin.
     * @param b	       Desired axis encoder channel B pin.
     * @param sw       Desired axis Home Switch pin.
     * @param i2c      I2C instance used by MotorControllerI2C.
     * @param addr     I2C address used by Axis MotorControllerI2C instance.
     */
	Axis(PinName a, PinName b, PinName sw, I2C* i2c, int addr);

	/**
     * Constructor.
     *
     * Saves i2c instance and address for MotorControllerI2C communication.
     *
     * Setups PID and QEI parameters for the desired axis.
     *
     * @param ap       Axis params, see Params struct.
     * @param i2c      I2C instance used by MotorControllerI2C.
     * @param addr     I2C address used by Axis MotorControllerI2C instance.
     */
	Axis(Params ap, I2C* i2c, int addr);
	~Axis();
	

	/**
     * Set current axis speed.
     *
     * @param vel       Desired axis speed. Value range: [-MAX_PWR,MAX_PWR]
     *
     * @return If success, return true.
     */
	bool setVel(int vel);

	/**
     * Set current axis speed using PID method.
     *
     * @param desired_pos Axis desired speed. Value range: [-MAX_PWR,MAX_PWR]
     * @param past_pos Axis desired speed. Value range: [-MAX_PWR,MAX_PWR]
     *
     * @return If success, return true.
     */
	bool setVel(int desired_pos, int past_pos);

	/**
     * Returns the current speed.
     *
     * By default it doesn't try to update the value. In a normal condition the
     * value shall be correct without need to re-check again. A special case
     * may be at Axis initialization.
     *
     * @param force_update By default is false. If true, it will update the current speed value.
     *
     * @return Returns the current speed.
     */
	int getVel(bool force_update=false);

	/**
     * Set current axis position.
     *
     * After this call, Axis instance is mark as calibrated.
     *
     * @param vel       Desired axis position.
     */
	void setPos(int pos);

	/**
     * Returns current axis position.
     *
     * @return Current position.
     */
	int getPos();


	bool setAccel(int accel);
	int getAccel(bool force_update=false);

	void setPID(PID pid);
	PID getPID();

	bool getHsw();

	bool isCalibrated();

	void setStatus(int status);
	int getStatus();
	
private:
	QEI* enc;
	DigitalIn hsw;
	MotorControllerI2C* mc;
	PID pid;

	int count_limit;
	int count_home;
	int vel;
	int accel;
	int status;
	bool calibrated;
};

#endif
