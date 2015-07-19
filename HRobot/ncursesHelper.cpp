/*
 * ncursesHelper.cpp
 *
 *  Created on: 21 de nov. de 2015
 *      Author: Javier
 */
#include "ncursesHelper.h"
/*
void init_ansi_colors() {
	int pair_count = 0;
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 16; j++) {
			init_pair(pair_count,j,i);
			pair_count++;
		}
	}
}

int get_ansi_pair(int ch_color, int bg_color, bool bold) {
	int c;

	if(ch_color > 7 || bg_color > 7)
		return 0;
	else {
		if(bold) {
			ch_color |= 8;
		}
		c = ch_color*
	}
}

void ansi_mvwprintw(WINDOW* w, int y, int x, const char* mesg,...) {
	const char COLOR_ESCAPE[] = "\x1B[";
	const int CS_LENGTH = strlen(COLOR_ESCAPE);
	char buff[256];
	int bg_color = BG_COLOR_DEF;
	int ch_color = CH_COLOR_DEF;
	va_list args;
	va_start(args,mesg);
	snprintf(buff,256,mesg,args);
	va_end(args);

	char* begin = buff;
	char* end;

	while(begin != NULL) {
		end = strstr(begin,COLOR_ESCAPE);
		size_t d = (size_t)end-begin;
		char slice[d+1] = {0};

		strncpy(slice,begin,d);
		mvwprintw(w,y,x,slice);
		int end_length = strlen(end);

		//Find ansi color escape
		if((end_length >= CS_LENGTH+2 && &end[CS_LENGTH+2] == 'm') ||
		   (end_length >= CS_LENGTH+3 && &end[CS_LENGTH+3] == 'm') ) {

			switch(buff[end+CS_LENGTH+1]) {
			case '0'://Restore color
				bg_color = BG_COLOR_DEF;
				ch_color = CH_COLOR_DEF;
				//begin = buff[end + CS_LENGTH+2];
				break;
			case '3'://char color
				if(end_length >= CS_LENGTH+2) {
					ch_color = &end[CS_LENGTH+2] - '0'; //atoi for single 0-9 char range
					//begin = buff[end + CS_LENGTH+3] - '0';
				}
				break;
			case '4'://Background color
				if(end_length >= CS_LENGTH+2) {
					bg_color = &end[CS_LENGTH+2] - '0';
					//begin = buff[end + CS_LENGTH+3] - '0';
				}
				break;
			default://Unrecognised
				//begin = buff[end+CS_LENGTH+1];
			}

			init_pair(1,ch_color,bg_color);
			wattron(w,1);

			if(end_length > CS_LENGTH+2 && &end[CS_LENGTH+2] == 'm')
				begin = &end[CS_LENGTH+3];
			else if(end_length > CS_LENGTH+3 && &end[CS_LENGTH+3] == 'm')
				begin = &end[CS_LENGTH+4];
			else
				begin = NULL;


		}
		//wprintw(w,mesg);
	}
	//mvwprintw(w,y,x,mesg);

}
*/

void remove_ansi_colors(std::string &s) {
	//std::regex ANSI_REGEX("\x1B\\[\\d{1,2}m");
	//s = std::regex_replace(s,ANSI_REGEX,"");
}

void remove_ansi_colors(char* c) {
	std::string s(c);
	remove_ansi_colors(s);
	strcpy(c,s.c_str());
}
