/*
 * MouseKeyboardStatus.h
 *
 *  Created on: Oct 21, 2008
 *      Author: Zhenyu Guo
 */

#ifndef MOUSEKEYBOARDSTATUS_H_
#define MOUSEKEYBOARDSTATUS_H_

#include "datatype/XmdvType.h"


class MouseKeyboardStatus {
public:
	MouseKeyboardStatus();
	virtual ~MouseKeyboardStatus();

	bool shiftkey, controlkey, altkey;
	bool button[3];

	Vec2 press_pos, curr_pos, prev_pos;
};

#endif /* MOUSEKEYBOARDSTATUS_H_ */
