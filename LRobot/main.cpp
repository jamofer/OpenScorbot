#include "mbed.h"
#include "RobotManager.h"
#include "RobotPacket.h"
#include "RobotParams.h"
#include "Logger.h"
//#include "./includes/I2CSlaveX.h"

I2CSlave hRobotSocket(HR_DATA,HR_CLK);

RobotManager* rm;

void init();
bool getRobotPacket(RobotPacket &rp);
bool sendRobotPacket(RobotPacket &rp);
int onHRobotActivity();
void reportStatistics();

RobotPacket rp_tx;
RobotPacket rp_rx;
int rx_count = 0;
int tx_count = 0;
Ticker statistisc_ticker;

int main() {
	init();

	//Timer delta;
	const int IDLE_DELAY = 20;
	const int MOVING_DELAY = 10;
	const int OTHER_DELAY = 30;
	const int NUM_PACKETS_PER_TICK = 10;
	int hrb_activity;




	while(rm->getRobotStatus() < Robot::STOP) {
		//delta.start();

		do {
			hrb_activity = onHRobotActivity();
			wait_us(10);
		}
		while(hrb_activity != I2CSlave::NoData);
		//wait_ms(1);

		rm->performRobotTick();
		wait_ms(1);


		//delta.stop();

		/*switch(rm.getRobotStatus()) {
			case Robot::MOVING:
			case Robot::MOVING_TO:
				wait_ms(MOVING_DELAY-delta.read_ms());
				break;
			case Robot::IDLE:
				wait_ms(IDLE_DELAY-delta.read_ms());
				break;
			default:
				wait_ms(OTHER_DELAY-delta.read_ms());
		}*/

	}

	mbed_interface_reset();

}


void init() {
	//hRobotSocket.frequency(100000);
	Logger::pc.baud(115200);
	Logger::logInit("LRobot starting...");
	Logger::logInit("LRobot waiting for HRobot in I2C pins (" STRINGIFY(HR_CLK) "/"
			STRINGIFY(HR_DATA) ") at %x address.",HR_ADDR);
	hRobotSocket.address(HR_ADDR);
	memset(&rp_rx,0,PACKET_SIZE);
	memset(&rp_tx,0,PACKET_SIZE);
	rm = new RobotManager();
	rm->performRobotTick();
	statistisc_ticker.attach(&reportStatistics,5.0);
	//hRobotSocket.attach(&onHRobotActivity);

}

bool getRobotPacket(RobotPacket &rp) {

	char buffer[PACKET_SIZE] = {0};
	int siz;
	siz = hRobotSocket.read(buffer,PACKET_SIZE);

	if(siz == 0) { //Bytes received must be PACKET_SIZE - 1
		memcpy(&rp,buffer,PACKET_SIZE);
		rx_count++;
		//Logger::logDebug("OKCommand received: %02x:%02x:%02x:%02x:%02x (rp:%02x)",buffer[0],buffer[1],buffer[2],buffer[3],buffer[4],rp.command);
		return true;
	}
	else {
		rm->hRobotSocketError();
		Logger::logDebug("FailCommand received: %02x:%02x:%02x:%02x:%02x (rp:%02x)",buffer[0],buffer[1],buffer[2],buffer[3],buffer[4],rp.command);
		return false;
	}
	//hRobotSocket.stop();
}

bool sendRobotPacket(RobotPacket &rp) {

	char buffer[PACKET_SIZE] = {0};

	memcpy(buffer,&rp,PACKET_SIZE);
	if( hRobotSocket.write(buffer,PACKET_SIZE) == 0 ) { //Bytes sent must be PACKET_SIZE
		tx_count++;
		//Logger::logDebug("OKCommand send: %02x:%02x:%02x:%02x:%02x (rp:%02x)",buffer[0],buffer[1],buffer[2],buffer[3],buffer[4],rp.command);
		return true;
	}
	else {
		rm->hRobotSocketError();
		Logger::logDebug("failCommand send: %02x:%02x:%02x:%02x:%02x (rp:%02x)",buffer[0],buffer[1],buffer[2],buffer[3],buffer[4],rp.command);
		return false;
	}
	//hRobotSocket.stop();

}

int onHRobotActivity() {
	//Logger::logDebug("onHRobotActivity");
	int a = hRobotSocket.receive();
	switch(a) {
		case I2CSlave::WriteAddressed:
			//Logger::logDebug("Is sending");
			getRobotPacket(rp_rx);
			//hRobotSocket.stop();
			rp_tx = rm->processPacket(rp_rx);
			memset(&rp_rx,0,PACKET_SIZE);
			break;
		case I2CSlave::ReadAddressed:
			//Logger::logDebug("Is receiving");
			sendRobotPacket(rp_tx);
			//hRobotSocket.stop();
			memset(&rp_tx,0,PACKET_SIZE);
			break;


	}
	return a;
	//Logger::logDebug("onHRobotActivity end");
}

void reportStatistics() {
	Logger::logDebug("I2CSlave TX:%d RX:%d",tx_count,rx_count);
	rm->reportStatus();
}

