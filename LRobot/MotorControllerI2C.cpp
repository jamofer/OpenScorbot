#include "MotorControllerI2C.h"

MotorControllerI2C::MotorControllerI2C(I2C* i2c, int addr) {
	char request[2];
	addr_ = addr;
	i2c_ = i2c;
	//Stopping motor
	request[0] = 0x00; //Command address
	request[1] = 0x00; //Stop command
	
	i2c_->write(addr_,request,2);
	
	//wait_ms(2);

}

MotorControllerI2C::~MotorControllerI2C() {
	delete i2c_;
}


bool MotorControllerI2C::setVel(int vel) {
	char request[2];
	bool success;
	
	if (vel > 0xF3) //Velocity can't be more than 0xF3
		vel=0xF3;
	else if (vel < 0) //Velocity can't take a negative value
		vel=0;
	
	request[0] = 0x02;//Velocity address
	request[1] = vel & 0xFF;//Velocity value
	
	success = (i2c_->write(addr_,request,2)==0); //Return 0 if success
	//wait_us(2000);
	//wait_ms(2);
	return success;
}



bool MotorControllerI2C::setAccel(int accel) {
	char request[2];
	bool success;
	
	request[0] = 0x03;//Acceleration address
	request[1] = accel;
	
	
	if (accel > 0xFF) //Acceleration can't be more than 0xFF
		accel=0xFF;
	else if (accel < 0) //Acceleration can't take a negative value
		accel=0;
		
	success = (i2c_->write(addr_,request,2) == 0);
	
	//wait_ms(2);
	return success;
	
}


int MotorControllerI2C::getVel(void) {
	char request[1];
	char response[1];
	request[0] = 0x02;//Velocity address
	bool success;
	
	
	success = (i2c_->write(addr_,request,1) == 0);
	
	//wait_ms(2);
	
	
	success = success && (i2c_->read(addr_,response,1) == 0);//Getting vel
	//wait_us(100);
	
	//wait_ms(2);
	
	if(success)//Return response or -1 on error.
		return response[0];
	else
		return 0x1FFF;
}



int MotorControllerI2C::getAccel(void) {
	char request[1];
	char response[1];
	request[0] = 0x03;//Acceleration address
	bool success;
	
	
	success = (i2c_->write(addr_,request,1) == 0);
	
	//wait_ms(2);
	
	success = success && (i2c_->read(addr_,response,1) == 0);//Getting accel
	
	//wait_ms(2);
	
	if (success)//Return response or -1 on error.
		return response[0];
	else
		return -1;
}

bool MotorControllerI2C::sendCommand(int com) {
	char request[2];
	bool success;
	
	request[0] = 0x00; //Command address
	request[1] = com;
	
	
	success =  (i2c_->write(addr_,request,2) == 0);
	//wait_us(2000);
	
	//wait_ms(2);
	
	return success;
}

int MotorControllerI2C::getCommand() {
	char request[1];
	char response[1];
	request[0] = 0x00; //Command address
	bool success;
	
	
	success = (i2c_->write(addr_,request,1) == 0);
	
	//wait_ms(2);
	
	success = success && (i2c_->read(addr_,response,1) == 0);
	//wait_us(100);
	
	//wait_ms(2);
	
	if (success)
		return response[0];
	else
		return -1;
	
}

bool MotorControllerI2C::setVel(int vel,int dir) {
	bool success;
	switch (dir) {
		case 1:
			success = sendCommand(0x01); //Clockwise
			break;
		case 2:
			success = sendCommand(0x02); //Counter clockwise
			break;
		default:
			success = sendCommand(0x00); //Stop
	}
	return (success && setVel(vel));
}


int MotorControllerI2C::getTemp(void) {
	char request[1];
	char response[1];
	request[0] = 0x04;//Temperature address
	bool success;
	
	success = (i2c_->write(addr_,request,1) == 0);
	//wait_ms(2);
	success = success && (i2c_->read(addr_,response,1) == 0);//Getting temp
	//wait_ms(2);
	
	if (success)//Return response or -1 on error.
		return response[0];
	else
		return -1;
}

int MotorControllerI2C::getCurrent(void) {
	char request[1];
	char response[1];
	request[0] = 0x05;//Current address
	bool success;
	
	success = (i2c_->write(addr_,request,1) == 0);
	//wait_ms(2);
	success = success && (i2c_->read(addr_,response,1) == 0);//Getting current
	//wait_ms(2);
	
	if (success)//Return response or -1 on error.
		return response[0];
	else
		return -1;
}

int MotorControllerI2C::getStatus(void) {
	char request[1];
	char response[1];
	request[0] = 0x01;//Status address
	bool success;
	
	success = (i2c_->write(addr_,request,1) == 0);
	//wait_ms(2);
	success = success && (i2c_->read(addr_,response,1) == 0);//Getting status
	//wait_ms(2);
	
	if (success)//Return response or -1 on error.
		return response[0];
	else
		return -1;
}

int MotorControllerI2C::getFirmV(void) {
	char request[1];
	char response[1];
	request[0] = 0x07;//Firm revision number address
	bool success;
	
	success = (i2c_->write(addr_,request,1) == 0);
	//wait_ms(2);
	success = success && (i2c_->read(addr_,response,1) == 0);//Getting status
	//wait_ms(2);
	
	if (success)//Return response or -1 on error.
		return response[0];
	else
		return -1;
}
	
