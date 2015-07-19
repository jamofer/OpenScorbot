/*
 * Logger.h
 *
 *  Created on: 23 de sept. de 2015
 *      Author: Javier
 */

#ifndef LOGGER_H_
#define LOGGER_H_

#include "mbed.h"
#include <stdarg.h>

#define LOGGER_SIZE 1024 * 10 //1KB

/* Linux terminal color templates */
#define NRM  "\x1B[0m"
#define RED  "\x1B[31m"
#define GRN  "\x1B[32m"
#define YEL  "\x1B[33m"
#define BLU  "\x1B[34m"
#define MAG  "\x1B[35m"
#define CYN  "\x1B[36m"
#define WHT  "\x1B[37m"
#define BOL  "\x1b[1m"

class Logger {

public:

	enum TAG {
		INFO = 0x00,
		DEBUG = 0x01,
		WARNING = 0x02,
		ERROR = 0x03,
		FATAL = 0x04,
		INIT = 0x05
	};



	static char* getLog();
	static void clearLog();
	static unsigned int pos;

	static bool logInit(const char * mesg, ...);
	static bool logInfo(const char * mesg, ...);
	static bool logDebug(const char * mesg, ...);
	static bool logWarning(const char * mesg, ...);
	static bool logError(const char * mesg, ...);
	static bool logFatal(const char * mesg, ...);
	static bool logLine(const char * mesg, ...);
	static Serial pc;

private:

	static char log[LOGGER_SIZE];
	static bool logMessage(const char * mesg,int msgtype, va_list args);

};

#endif /* LOGGER_H_ */
