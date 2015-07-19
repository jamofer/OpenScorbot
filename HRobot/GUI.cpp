/*
 * GUI.cpp
 *
 *  Created on: 27 de ene. de 2016
 *      Author: jamof
 */

#include "GUI.h"

GUI::GUI(RobotController* rc) {
	// TODO Auto-generated constructor stub
	this->rc = rc;
	rc->setGUI(this);
}

GUI::~GUI() {
	// TODO Auto-generated destructor stub
}

bool GUI::appLogLine(std::string line) {
	return false;
}
