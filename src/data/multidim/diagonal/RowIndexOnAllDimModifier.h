/*
 * RowIndexOnAllDimModifier.h
 *
 *  Created on: Aug 21, 2009
 *      Author: zyguo
 */

#ifndef ROWINDEXONALLDIMMODIFIER_H_
#define ROWINDEXONALLDIMMODIFIER_H_

#include "data/multidim/OkcDataModifier.h"
#include "main/XmdvTool.h"
#include <vector>

class RowIndexOnAllDimModifier : public OkcDataModifier{
	friend class SortRowsOnAllDimOperator;
	friend class OkcDataModifierManager;
public:
	RowIndexOnAllDimModifier();
	RowIndexOnAllDimModifier(OkcData* okcdata);
	virtual ~RowIndexOnAllDimModifier();
public:
	std::vector< std::vector <int> >  sortOrderOnAllDim;
public:
	void initRequiredArr();
	void copyFrom(const RowIndexOnAllDimModifier* copy);
	void operator=(const RowIndexOnAllDimModifier &copy);
};

#endif /* ROWINDEXONALLDIMMODIFIER_H_ */
