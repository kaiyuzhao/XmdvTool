/*
 * MouseKeyboardStatus.cpp
 *
 *  Created on: Oct 21, 2008
 *      Author: Zhenyu Guo
 */

#include "MouseKeyboardStatus.h"

MouseKeyboardStatus::MouseKeyboardStatus() {
	this->altkey = false;
	this->controlkey = false;
	this->shiftkey = false;
	for (int i=0; i<3; i++){
		this->button[i] = false;
	}

}

MouseKeyboardStatus::~MouseKeyboardStatus() {
}
