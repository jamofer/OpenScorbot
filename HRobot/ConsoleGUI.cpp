/*
 * ConsoleGUI.cpp
 *
 *  Created on: 13 de oct. de 2015
 *      Author: Javier
 */

#include "ConsoleGUI.h"

const char* ConsoleGUI::MOVE = "move";
const char* ConsoleGUI::VEL = "vel";
const char* ConsoleGUI::CALIB = "calibrate";
const char* ConsoleGUI::HELP = "help";
const char* ConsoleGUI::STOP = "stop";
const char* ConsoleGUI::OWNER = "owner";
const char* ConsoleGUI::HOLD = "hold";

ConsoleGUI::ConsoleGUI(RobotController* rc) : GUI(rc) {
	// TODO Auto-generated constructor stub

	initscr();
	cbreak();
	noecho();
	curs_set(FALSE);
	getmaxyx(stdscr,parent_y,parent_x);

	log_size_y = parent_y - STATUS_Y - PROMPT_Y - INFO_Y;
	log_size_x = parent_x;
	log_pos_y = STATUS_Y + INFO_Y;
	log_pos_x = 0;

	statusUpdated = true;
	infoUpdated = true;
	logUpdated = true;

	robotStatusView = newwin(STATUS_Y,parent_x,0,0);
	robotInfoView = newwin(INFO_Y,parent_x,STATUS_Y,0);
	robotPrompt = newwin(PROMPT_Y,parent_x,parent_y-PROMPT_Y,0);
	robotLogView = newwin(log_size_y,log_size_x,log_pos_y,log_pos_x);

	nodelay(robotPrompt,true);

}

ConsoleGUI::~ConsoleGUI() {
	// TODO Auto-generated destructor stub
	endwin();
}

bool ConsoleGUI::appLogLine(std::string line) {

	bool s;
	log.insert(log.begin(),line);

	if(log.size() > 20) {
		log.pop_back();
		s = true;
	}
	else
		s = false;

	updateLogView();
	return s;
}

void ConsoleGUI::updateLogView() {
	int x;
	int y;

	curs_set(FALSE);
	getmaxyx(robotLogView,y,x);
	werase(robotLogView);


	for (unsigned int i = 0; i < log.size(); i++) {
		std::string line = log[i];
		//remove_ansi_colors(line);
		//const char* strline = line.c_str();
		mvwprintw(robotLogView, y-i-1,0,line.c_str());
	}
	//log.clear();
	wrefresh(robotLogView);
}

void ConsoleGUI::updateStatusView() {
	curs_set(FALSE);
	wclrtoeol(robotStatusView);
	mvwprintw(robotStatusView,robotStatusView->_begy,0,"HRobot Status: %x LRobot Status: %x",rc->getHRobotStatus(),rc->getLRobotStatus());
	wrefresh(robotStatusView);
}

void ConsoleGUI::updateInfoView() {
	curs_set(FALSE);
	RobotPacket rpp = rc->getRobotPos();
	RobotPacket rpv = rc->getRobotVel();
	RobotPacket rpa;
	int status = rc->getHRobotStatus();
	std::string action;

	if(status == Robot::MOVING) {
		rpa = rc->getRobotDesiredVel();
		action = "[VELOCITY TARGET]";
	}
	else if(status == Robot::MOVING_TO || status == Robot::IDLE) {
		rpa = rc->getRobotDesiredPos();
		action = "[POSITION TARGET]";
	}
	else if(status == Robot::CALIBRATING) {
		rpa = rc->getRobotDesiredCalibration();
		action = "[CALIBRATION]";
	}
	else {
		rpa = rc->getRobotDesiredVel();
	}

	wclrtoeol(robotInfoView);

	mvwprintw(robotInfoView, 0,0,"POS: %06d %06d %06d %06d %06d %06d",
			rpp.contenti[0],rpp.contenti[1],rpp.contenti[2],rpp.contenti[3],rpp.contenti[4],rpp.contenti[5]);
	mvwprintw(robotInfoView, 1,0,"VEL: %03d %03d %03d %03d %03d %03d",
			rpv.contenti[0],rpv.contenti[1],rpv.contenti[2],rpv.contenti[3],rpv.contenti[4],rpv.contenti[5]);
	mvwprintw(robotInfoView,2,0,"ACT: %06d %06d %06d %06d %06d %06d %s",
			rpa.contenti[0],rpa.contenti[1],rpa.contenti[2],rpa.contenti[3],rpa.contenti[4],rpa.contenti[5],action.c_str());
	wrefresh(robotInfoView);
}

int ConsoleGUI::checkPrompt() {
	wclrtoeol(robotPrompt);
	mvwprintw(robotPrompt,0,0,"> %s",cmd.c_str());
	curs_set(TRUE);
	//wmove(robotPrompt,0,2+(int)cmd.length());
	int ch = wgetch(robotPrompt);
	int status = 0;
	switch (ch) {
	case '\33':
		wgetch(robotPrompt);
		wgetch(robotPrompt);
		break;
	case ERR:
		break;
	case '\n':
		if(!cmd.empty()) {
			status = processCmd(cmd);
		}
		wclrtoeol(robotPrompt);
		cmd = "";
		break;
	case 127:
		if(cmd.length() > 0)
			cmd.pop_back();
		break;
	default:
		if(!iscntrl(ch)) //Is not a delete character
			cmd += ch;
	}

	return status;

}

int ConsoleGUI::processCmd(std::string cmd) {
	int result = -1;
	std::vector<std::string> args;
	std::string command;
	RobotPacket rp = {0};

	std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower); //lowercase

	splitString(cmd,' ',args); //Spliting command

	if(args.size() > 0) {
		command = args[0];
	}

	if(args.size() > 1 && (command == MOVE || command == VEL)) {
		if(args[1] == "all") {
			memset(rp.contentb,true,(int)sizeof(rp.contentb));
			if(args.size() == 3 && args[2] != "-") {
				std::string a = args[2];
				memset(rp.contenti,std::stoi(a.c_str()),(int)sizeof(rp.contenti));
			}
			result = 1;
		}
		else if (args.size() == (1+NUM_AXES)) {
			for(int i = 0; i < NUM_AXES; i++) {
				std::string a = args[1+i];
				if(a == "-") {
					continue;
				}
				else {
					try {
						rp.contenti[i] = std::stoi(a);
						rp.contentb[i] = true;
					}
					catch (int e) {
						rp.contentb[i] = false;
					}
				}
			}
			result = 1;
		}
		else if (args[1] == "home" && command == MOVE) {
			rp.contenti[BASE] = BASE_COUNTS_HOME;
			rp.contenti[BODY] = BODY_COUNTS_HOME;
			rp.contenti[UPPER_ARM] = UPPER_ARM_COUNTS_HOME;
			rp.contenti[FOREARM_PITCH] = FOREARM_PITCH_COUNTS_HOME;
			rp.contenti[FOREARM_ROLL] = FOREARM_ROLL_COUNTS_HOME;
			rp.contenti[CLIP] = CLIP_COUNTS_HOME;
			memset(rp.contentb,true,(int)sizeof(rp.contentb));
			result = 1;
		}
		else {
			Logger::logGUI("\"%s\" bad usage.",command.c_str());
			Logger::logGUI("Usage: (vel|move) ([BASE,BODY,UPPER_ARM,FOREARM_PITCH,FOREARM_ROLL,CLIP]|all [VALUE])");
			result = -1;
		}

		if(result == 1) {
			if(command == MOVE) {
				result = rc->moveRobot(rp) ? 1 : -1; //success = 1 Fail = -1
			}
			else if(command == VEL) {
				rc->setRobotVel(rp);
			}
			else if(command == CALIB) {
				rc->calibrateRobot(rp);
			}
		}


	}
	else if (args.size() > 1 && command == CALIB) {
		if(args[1] == "all") {
			memset(rp.contentb,true,(int)sizeof(rp.contentb));
			//memset(rp.contentb,0,(int)sizeof(rp.contenti));
			rp.contenti[FOREARM_ROLL] = 1;
			result = 1;
		}
		else if (args.size() == (1+NUM_AXES)) {
			for(int i = 0; i < NUM_AXES; i++) {
				std::string a = args[1+i];
				if(a == "-") {
					continue;
				}
				else {
					if(a == "auto" || (a == "default" && i != FOREARM_ROLL)) {
						rp.contenti[i] = 0;
						rp.contentb[i] = true;
					}
					else if(a == "manual" || (a == "default" && i == FOREARM_ROLL)) {
						rp.contenti[i] = 1;
						rp.contentb[i] = true;
					}
				}
			}
			result = 1;
		}
		else if (args[1] == "confirm" && args.size() == 3) {
			try {
				int axis;
				std::string a = args[2];
				axis = std::stoi(a);
				rp.contentb[axis] = true;
				rp.contenti[axis] = 2;
				result = 1;
			}
			catch (int e) {
				result = -1;
			}
		}
		else {
			Logger::logGUI("\"%s\" bad usage.",command.c_str());
			Logger::logGUI("Usage: (calibrate) ({[BASE,BODY,UPPER_ARM,FOREARM_PITCH,FOREARM_ROLL,CLIP] [AUTO|MANUAL|DEFAULT]}|{ALL})");
			Logger::logGUI("After manual calibration request: (calibrate) (confirm) (AXIS_NUMBER)");

		}
		if(result == 1) {
			rc->calibrateRobot(rp);
		}
	}
	else if(args.size() > 1 && command == OWNER) {
		int o;
		if(args[1] == "hrobot") {
			o = HROBOT;
			result = 1;
		}
		else if(args[1] == "lrobot" ) {
			o = LROBOT;
			result = 1;
		}

		if(result == 1) {
			rc->setOwnerMode(o);
		}
		else {
			Logger::logGUI("Command 'owner' failed. Usage: OWNER [HROBOT|LROBOT]");
			result = -1;
		}

	}
	else if (command == HOLD) {
		rc->setRobotVel(rp);
		result = 1;
	}
	else if(command == HELP) {

		if (args.size() > 1 && args[1] == VEL ) {
			Logger::logGUI("Usage: vel ([BASE,BODY,UPPER_ARM,FOREARM_PITCH,FOREARM_ROLL,CLIP]|all [VALUE])");
			Logger::logGUI("Example: vel 100 0 200 - 30 30 #('-' for hold the axis)");
		}
		else if (args.size() > 1 && args[1] == MOVE) {
			Logger::logGUI("Usage: move ([BASE,BODY,UPPER_ARM,FOREARM_PITCH,FOREARM_ROLL,CLIP]|all [VALUE])");
			Logger::logGUI("Example: move 5000 0 200 - -300 30 #('-' for hold the axis)");
		}
		else if (args.size() > 1 && args[1] == HOLD) {
			Logger::logGUI("Usage: hold");
			Logger::logGUI("Holds all robot axes in their actual position.");
		}
		else if(args.size() > 1 && args[1] == OWNER) {
			Logger::logGUI("Sets the Robot's owner: HROBOT or LROBOT");
		}
		else if (args.size() > 1 && args[1] == CALIB) {
			Logger::logGUI("Usage: (calibrate) ({[BASE,BODY,UPPER_ARM,FOREARM_PITCH,FOREARM_ROLL,CLIP] [AUTO|MANUAL|DEFAULT]}|{ALL})");
			Logger::logGUI("After manual calibration request: (calibrate) (confirm) (AXIS_NUMBER)");
			Logger::logGUI("Example: calibrate all");
			Logger::logGUI("calibate confirm %d",FOREARM_ROLL);
		}
		else {
			Logger::logGUI("Command list: calibrate,vel,move,hold,stop,owner.");
		}
		result = 1;
	}
	else if (command == STOP) {
		result = 10;
	}
	else {
		result = -2;
	}

	if(result == 1) {
		Logger::logGUI("'%s' [OK]",cmd.c_str());
	}
	else if (result == -1) {
		Logger::logGUI("'%s' failed.",cmd.c_str());
	}
	else if (result == -5) {
		Logger::logGUI("Command '%s' not found.",cmd.c_str());
	}

	//10 STOP, 1 CORRECT, -1 ERROR
	return result;
}

void ConsoleGUI::splitString(const std::string& s, char delim,std::vector<std::string>& v) {
    int i = 0;
    size_t pos = s.find(delim);
    do {

      if (pos == std::string::npos) {
         v.push_back(s.substr(i, s.length()));
         break;
      }
      else {
          v.push_back(s.substr(i, pos-i));
          i = ++pos;
          pos = s.find(delim, pos);
      }
    }
    while (true);
}
