/*
 * OkcDataModifierClusterHighlight.h
 *
 *  Created on: Jul 14, 2009
 *      Author: Zaixian Xie
 */

/*
 * Class OkcDataModifierClusterHighlight represents
 * which tuple to highlight in the result
 * of SBBOperator (structure-based brush)
 */
#ifndef OKCDATAMODIFIERCLUSTERHIGHLIGHT_H_
#define OKCDATAMODIFIERCLUSTERHIGHLIGHT_H_

#include <vector>
#include "data/multidim/OkcDataModifier.h"

class OkcDataModifierClusterHighlight : public OkcDataModifier {
public:
	OkcDataModifierClusterHighlight();
	OkcDataModifierClusterHighlight(OkcData* okcdata);
	virtual ~OkcDataModifierClusterHighlight();

private:
	std::vector<bool> m_highlighted;

public:
	// return the modifier type
	XmdvTool::MODIFIERTYPE getModifierType();

	// Initialize the highlighted array,
	// its length is data_size of the OkcData
	void initHighlightArr();
	void setHighlight(int i, bool isHighlighted);
	bool getHighlight(int i);

	// Return a reference to the highlight array
	std::vector<bool> &getHighlightArr();

	// Add a color value to the end of array
	void insertTail(bool isHighlighted);

	void copyFrom(const OkcDataModifierClusterHighlight* copy);
	// overloading the operator "="
	void operator=(const OkcDataModifierClusterHighlight &copy);



};

#endif /* OKCDATAMODIFIERCLUSTERHIGHLIGHT_H_ */
