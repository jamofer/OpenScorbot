/*
 * Logger.cpp
 *
 *  Created on: 23 de sept. de 2015
 *      Author: Javier
 */

#include "Logger.h"


unsigned int Logger::pos = 0;
char Logger::log[] = {0};
Serial Logger::pc(USBTX, NC); // tx, rx


bool Logger::logWarning(const char * mesg, ...) {
	bool status;
	va_list args;

	va_start(args,mesg);
	status = logMessage(mesg,TAG::WARNING, args);
	va_end(args);

	return status;
}

bool Logger::logError(const char * mesg, ...) {
	bool status;
	va_list args;

	va_start(args,mesg);
	status = logMessage(mesg,TAG::ERROR, args);
	va_end(args);

	return status;
}

bool Logger::logFatal(const char * mesg, ...) {
	bool status;
	va_list args;

	va_start(args,mesg);
	status = logMessage(mesg,TAG::FATAL, args);
	va_end(args);

	return status;
}

bool Logger::logDebug(const char * mesg, ...) {
	bool status;
	va_list args;

	va_start(args,mesg);
	status = logMessage(mesg,TAG::DEBUG, args);
	va_end(args);

	return status;
}

bool Logger::logInfo(const char * mesg, ...) {
	bool status;
	va_list args;

	va_start(args,mesg);
	status = logMessage(mesg,TAG::INFO, args);
	va_end(args);

	return status;
}

bool Logger::logInit(const char * mesg, ...) {
	bool status;
	va_list args;

	va_start(args,mesg);
	status = logMessage(mesg,TAG::INIT, args);
	va_end(args);

	return status;
}

bool Logger::logLine(const char * mesg, ...) {
	bool status;
	va_list args;

	va_start(args,mesg);
	status = logMessage(mesg,-1, args);
	va_end(args);

	return status;
}


bool Logger::logMessage(const char * mesg,int msgtype, va_list args) {


	bool looped = false;
	char buffer[128];
	char line[128];
	DigitalOut error_led(LED3);
	DigitalOut fatal_led(LED4);



	if(strlen(mesg) > 100)
		return false; //Large lines are forbidden

	vsnprintf(buffer, sizeof(buffer), mesg, args);

	switch(msgtype) {

	case TAG::INFO:
		sprintf(line,"%s[INFO]%s %s\n",BOL,NRM,buffer);
		break;
	case TAG::WARNING:
		sprintf(line,"%s[WARNING]%s %s\n",YEL,NRM,buffer);
		break;
	case TAG::DEBUG:
		sprintf(line,"%s[DEBUG]%s %s\n",GRN,NRM,buffer);
		break;
	case TAG::ERROR:
		sprintf(line,"%s%s[ERROR]%s %s\n",BOL,RED,NRM,buffer);
		error_led = 1;
		break;
	case TAG::FATAL:
		sprintf(line,"%s[FATAL]%s %s\n",RED,NRM,buffer);
		fatal_led = 1;
		break;
	case TAG::INIT:
		sprintf(line,"%s[INIT]%s %s\n",CYN,NRM,buffer);
		break;
	default:
		sprintf(line,"%s\n",buffer);
	}

    unsigned int s = strlen(line);

	#ifdef USB_LOG
		pc.printf("%s\r",line);
	#endif

    if(s + pos >= LOGGER_SIZE) {
    	Logger::clearLog();
    	looped = true;
    }


    memcpy(log+pos,line,s);
    pos += s; //null character will be always overwritten

    return looped;
}

char* Logger::getLog() {
	return log;
}

void Logger::clearLog() {
	memset(log,0,LOGGER_SIZE);
	pos = 0;
}
