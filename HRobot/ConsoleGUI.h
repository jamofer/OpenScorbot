/*
 * ConsoleGUI.h
 *
 *  Created on: 13 de oct. de 2015
 *      Author: Javier
 */

#ifndef CONSOLEGUI_H_
#define CONSOLEGUI_H_

#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
//#include "ncursesHelper.h"
#include <ncurses.h>
#include <unistd.h>
#include "Logger.h"
#include "RobotController.h"
#include "GUI.h"

#define STATUS_Y 	1
#define INFO_Y 		3
#define PROMPT_Y 	1

class RobotController;
class GUI;
class ConsoleGUI : public GUI {
public:

	static const char* MOVE;
	static const char* VEL;
	static const char* CALIB;
	static const char* HELP;
	static const char* STOP;
	static const char* OWNER;
	static const char* HOLD;

	ConsoleGUI(RobotController* rc);
	~ConsoleGUI();

	bool appLogLine(std::string line);

	void updateLogView();
	void updateStatusView();
	void updateInfoView();
	int checkPrompt(); // -1 invalid command, 0 no cmd, cmd_code otherwise

private:
	WINDOW* robotStatusView;
	WINDOW* robotInfoView;
	WINDOW* robotLogView;
	WINDOW* robotPrompt;

	int parent_x;
	int parent_y;
	int log_size_x;
	int log_size_y;
	int log_pos_x;
	int log_pos_y;

	bool logUpdated;
	bool statusUpdated;
	bool infoUpdated;

	std::string cmd;
	//std::vector<std::string> log;

	int processCmd(std::string cmd);
	void splitString(const std::string& s, char delim,std::vector<std::string>& v);

};

#endif /* CONSOLEGUI_H_ */
