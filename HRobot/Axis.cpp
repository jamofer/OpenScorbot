/*
 * Axis.cpp
 *
 *  Created on: 8 de oct. de 2015
 *      Author: Javier
 */

#include "Axis.h"

Axis::Axis() {
	// TODO Auto-generated constructor stub
	calibrated = false;
	count_home = 0;
	count_limit = 0;
	pos = 0;
	vel = 0;
	accel = 0;
	pid = {0};
	hsw = false;
	status = FREE;
	calib_dir = 1;

}

Axis::Axis(Axis* ax) {
	copyFromAxis(ax);
}

Axis::~Axis() {
	// TODO Auto-generated destructor stub
}

Axis::Axis(Params ap) {
	calibrated = false;
	count_home = ap.count_home;
	count_limit = ap.count_limit;
	pos = ap.pos;
	vel = 0;
	accel = 0;
	calib_dir = 0;

	pid = ap.pid;
	hsw = false;
	status = FREE;

}

void Axis::setHsw(bool hsw) {
	this->hsw = hsw;
}

bool Axis::getHsw() {
	return hsw;
}

bool Axis::isCalibrated() {
	return calibrated;
}

void Axis::setVel(int vel) {

	if (vel > MAX_PWR)
		vel = MAX_PWR;
	else if (vel < -MAX_PWR) {
		vel = -MAX_PWR;
	}

	this->vel = vel;
}

void Axis::setVel(int desired_pos, int past_pos) {
	int vel;
	int actual_pos = getPos();

	int dif = desired_pos - actual_pos;
	int dif_p = actual_pos - past_pos; //pos'
	pid.e += dif;

	// Velocity calculation by PID
	vel = (int)(pid.p*dif + pid.i * pid.e + pid.d * dif_p + 0.5f);

	setVel(vel);
}


int Axis::getVel() {
	return vel;
}

void Axis::setPos(int pos) {
	this->pos = pos;
}

void Axis::setCalibrated(bool c) {
	calibrated = c;
}

int Axis::getPos() {
	return pos;
}

void Axis::setAccel(int accel) {
	this->accel = accel;
}

int Axis::getAccel() {
	return accel;
}

void Axis::setPID(PID pid) {
	this->pid = pid;
}

Axis::PID Axis::getPID() {
	return pid;
}

int Axis::getCountHome() {
	return count_home;
}

int Axis::getCountLimit() {
	return count_limit;
}

Axis::Params Axis::getParams() {
	Params p;
	p.count_home = count_home;
	p.count_limit = count_limit;
	p.pid = pid;
	p.pos = pos;

	return p;
}

void Axis::setParams(Axis::Params p) {
	count_home = p.count_home;
	count_limit = p.count_limit;
	pid = p.pid;
	pos = p.pos;
}

int Axis::getStatus() {
	return status;
}

void Axis::setStatus(int s) {
	status = s;
	pid.e = 0;
}

void Axis::setCalibDir(int i) {
	calib_dir = i;
}
int Axis::getCalibDir() {
	return calib_dir;
}


void Axis::copyFromAxis(Axis* ax) {
	this->accel = ax->getAccel();
	this->calibrated = ax->isCalibrated();
	this->pid = ax->getPID();
	this->pos = ax->getPos();
	this->status = ax->getStatus();
	this->vel = ax->getVel();
	this->count_home = ax->getCountHome();
	this->count_limit = ax->getCountLimit();
	this->hsw = ax->getHsw();
	this->calib_dir = ax->getCalibDir();
}
