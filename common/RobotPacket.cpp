#include "RobotPacket.h"



bool checkLRC(RobotPacket& rp) {
	return true;
	return (rp.lrc == genLRC(rp));
}

char genLRC(RobotPacket& rp) {
	char buff[PACKET_SIZE];

	memcpy(buff,&rp,PACKET_SIZE);
	char lrc = buff[HEAD_LENGTH];
	
	for(int i = 1; i < PAY_LENGTH; i++)
		lrc = lrc^buff[i+HEAD_LENGTH];
	return lrc;
}

bool checkRobotCMD(RobotPacket& rp,int cmd) {
	return true;
	return (rp.command & RP_CMD::ACK_MASK) == cmd;
}


bool checkRobotACK(RobotPacket& rp) {
	//return true;
	return (rp.command & RP_CMD::ACK_MASK) == RP_CMD::ACK;
}
