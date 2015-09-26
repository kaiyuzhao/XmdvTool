/*
 * OkcDataModifierPCADerive.h
 *
 *  Created on: Sep 2, 2009
 *      Ported by Zaixian Xie
 */

/*
 * Class OkcDataModifierPCADerive contains principal components
 * derived from the PCA algorithm.
 */

#ifndef OKCDATAMODIFIERPCADERIVE_H_
#define OKCDATAMODIFIERPCADERIVE_H_

#include "data/multidim/OkcDataModifier.h"
#include <vector>

class OkcDataModifierPCADerive : public OkcDataModifier {
public:
	OkcDataModifierPCADerive();
	OkcDataModifierPCADerive(OkcData* okcdata);
	virtual ~OkcDataModifierPCADerive();

public:
	// return the modifier type
	XmdvTool::MODIFIERTYPE getModifierType();

	// prin_num is the number of principal components
	int prin_num;
	// the lenght of array, which is the data size of the input okcdata
	int arr_len;
	// principal is a two-dim array
	// princicpal[i] (0<=i<=prin_num-1) corresponds to a component whose order is i.
	// Its size is the original data size of the okcdata
	double **principal;
	// principal_max and principal_min are arrays.
	// They denotes the maximum and minimum values for each principal components.
	// For example, principal_max[i] is the maximum values for principal[i][0..n-1]
	// (n is the original data size of the okc data)
	// The size of these arrays is prin_num.
	double *principal_max;
	double *principal_min;

	// Set the arrays to contain the principal components to NULL
	void resetPrinArr();
	// Initialize the arrays to contain the principal components  in terms of the current okcdata
	void initPrinArr();

	void copyFrom(const OkcDataModifierPCADerive* copy);
	// overloading the operator "="
	void operator=(const OkcDataModifierPCADerive &copy);

	void getOneDimPrincipal(std::vector<double> &data, int dimIdx);
};

#endif /* OKCDATAMODIFIERPCADERIVE_H_ */
