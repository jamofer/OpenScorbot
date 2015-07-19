#ifndef RobotPacket_H
#define RobotPacket_H

#include <cstring>
#include <cstdint>

#define PACKET_SIZE 32
#define HEAD_LENGTH 2
#define PAY_LENGTH 30
#define CONTENT_MAX 6


enum RP_CMD {
    	EMPTY = 0x00,
    	ACK = 0x80,
		NACK = 0x40,


		RESET = 0x01, //Reset MBED program
		STOP = 0x02, //Stops robot
		MOVE = 0x05, //Moving counts
		SET_PID = 0x06, //Sets PID values
		GET_PWM = 0x07,   //Get RAW motors power
		SET_PWM = 0x08, //Set RAW motors power
		SET_ACCEL_RATES = 0x09, //Set acceleration rates
		GET_ACCEL_RATES = 0x0A, //Get acceleration rates
		SET_COUNTS = 0x0B, //Set axis encoder pulses
		GET_COUNTS = 0x0C, //Get axis encoder pulses
		STATUS = 0x0D, //RobotManager status
		GET_LROBOT_LOG = 0x0E, //Dumps the log buffer from LROBOT TO HROBOT
		FLASH_LROBOT = 0x1F, // Dumps a new software for LROBOT

		CMD_MASK = 0x1F, //ACK NACK AND CMD shares the same byte
		ACK_MASK = 0xC0
	};

	typedef struct RobotPacket {
		uint8_t lrc __attribute__ ((packed));
		uint8_t command __attribute__ ((packed));
		int contenti[CONTENT_MAX] __attribute__ ((packed));
		bool contentb[CONTENT_MAX] __attribute__ ((packed));
		
	};

	bool checkLRC(RobotPacket& rp);
	bool checkRobotCMD(RobotPacket& rp,int cmd);
	bool checkRobotACK(RobotPacket& rp);
	char genLRC(RobotPacket& rp);


#endif
