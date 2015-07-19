#ifndef MotorControllerI2C_H
#define MotorControllerI2C_H

#include "mbed.h"

class MotorControllerI2C {


public:

    /**
     * Constructor.
     *
     * Reads the current values on channel A and channel B to determine the
     * initial state.
     *
     * Attaches the encode function to the rise/fall interrupt edges of
     * channels A and B to perform X4 encoding.
     *
     * Attaches the index function to the rise interrupt edge of channel index
     * (if it is used) to count revolutions.
     *
     * @param i2c	I2C object for communication.
     * @param addr 	I2C controller address.
     */
    MotorControllerI2C(I2C* i2c, int addr);

    ~MotorControllerI2C();

    /**
     * Sets a velocity from 0 to 243 (0x00 - 0xF3).
     *
     * @param vel	Velocity.
     * @return If TRUE the command have been received by the controller.
     */
    bool setVel(int vel);
    
    
	/**
     * Sets a acceleration from 0 to 255 (0x00 - 0xFF).
     *
     * @param accel	Acceleration.
     * @return If TRUE the command have been received by the controller.
     */
	bool setAccel(int accel);
	
	/**
     * Gets the actual command (01 forwards, 02 reverse, 00 stop).
     *
     * @return command.
     */
	int getCommand();

    /**
     * Gets the actual velocity from 0 to 243 (0x00 - 0xF3).
     *
     * @return vel
     */
    int getVel(void);
    
    
    /**
     * Gets the actual acceleration from 0 to 255 (0x00 - 0xFF).
     *
     * @return accel
     */
    int getAccel(void);
    
    
	/**
     * Gets a relative temperature value of the module (0x00 - 0xFF).
     * Greater value means less temperature.
     * Addr: 0x04
     * 
     * @return temp
     */
    int getTemp(void);
    
    
	/**
     * Gets the actual power consumption from 0 to 186 (0x00 - 0xBA) 0xBA -> 20A.
     * Addr: 0x05
     * 
     * @return current
     */
    int getCurrent(void);
    
    
	/**
     * Gets the actual controller status
     * Addr: 0x01
     *
     * @return int:
     * 		bit0: Acceleration in progress. (status & 0x01)
     * 		bit1: Current limitator. (status & 0x02)
     * 		bit2: Temperature limitator. (status & 0x04)
     * 		bit7: Busy (status & 0x80)
     */
    int getStatus(void);
    
    
	/**
     * Gets the actual firmware revision number.
     * Addr: 0x07
     * 
     * @return revision number
     */
    int getFirmV(void);
    
    
	/**
     * Sends a command to the controller.
     * Addr: 0x00
     * 
     * @param com Command: 0x00 STOP, 0x01 FORWARD, 0x02 BACKWARD
     * @return If TRUE the command have been received by the controller.
     */
    bool sendCommand(int com);
    
    
	/**
     * Sets a velocity from 0 to 243 (0x00 - 0xF3) with a direcction.
     * AddrVel: 0x02; AddrDir: (command) 0x00
     * 
     * @param vel	Velocity.
     * @param dir	Direction. 0x00 STOP, 0x01 FORWARD, 0x02 BACKWARD
     * @return If TRUE the operation have been succesfully.
     */
    bool setVel(int vel,int dir);

	
private:
	I2C* i2c_;
	int addr_;
};
#endif
