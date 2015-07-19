/*
 * GUI.h
 *
 *  Created on: 27 de ene. de 2016
 *      Author: jamof
 */
#include <vector>
#include <string>
#include "RobotController.h"

#ifndef GUI_H_
#define GUI_H_

class RobotController;
class GUI {
public:
	GUI(RobotController* rc);
	GUI();
	virtual ~GUI();
	virtual bool appLogLine(std::string line);


protected:
	std::vector<std::string> log;
	RobotController* rc;
};
#endif /* GUI_H_ */
