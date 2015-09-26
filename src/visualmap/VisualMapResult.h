/*
 * VisualMapResult.h
 *
 *  Created on: Apr 1, 2008
 *      Author: Zaixian Xie
 */

/*
 * Class VisualMapResult stores the result of visual mapping
 * stage.  Since different types of data has different visual
 * attributes, it has a couple of derived class to correspond to
 * different types of Data.  The most important derived class is
 * OkcVisualMapResult.
 */

#ifndef VISUALMAPRESULT_H_
#define VISUALMAPRESULT_H_

class Data;

class VisualMapResult
{
public:
	VisualMapResult();
	VisualMapResult(Data* data);
	virtual ~VisualMapResult();

protected:
	// The data to be visualized
	Data* m_data;

public:
	//Set and get input
	void setData(Data* input);
	Data* getData();
};

#endif /*VISUALMAPRESULT_H_*/
