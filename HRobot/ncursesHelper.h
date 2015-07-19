/*
 * ncursesHelper.h
 *
 *  Created on: 21 de nov. de 2015
 *      Author: Javier
 */

#ifndef NCURSESHELPER_H_
#define NCURSESHELPER_H_

#include "ncurses.h"
#include <unistd.h>
#include <cstring>
#include <regex>

/* ANSI escaped colors */
#ifndef ANSI_COLORS
	#define ANSI_COLORS
	#define NRM  "\x1B[0m"
	#define RED  "\x1B[31m"
	#define GRN  "\x1B[32m"
	#define YEL  "\x1B[33m"
	#define BLU  "\x1B[34m"
	#define MAG  "\x1B[35m"
	#define CYN  "\x1B[36m"
	#define WHT  "\x1B[37m"
	#define BOL  "\x1b[1m"
#endif
/*
const int BG_COLOR_DEF = COLOR_BLACK;
const int CH_COLOR_DEF = COLOR_WHITE;

void init_ansi_colors();
int get_ansi_pair(int ch_color, int bg_color, bool bold);
void ansi_mvwprintw(WINDOW* w, int y, int x, const char* mesg,...);
*/

void remove_ansi_colors(std::string &s);
void remove_ansi_colors(char* c);

#endif /* NCURSESHELPER_H_ */
