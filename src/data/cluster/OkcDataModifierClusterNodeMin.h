/*
 * OkcDataModifierClusterNodeMin.h
 *
 *  Created on: Jul 16, 2009
 *      Author: Zaixian Xie
 */

#ifndef OKCDATAMODIFIERCLUSTERNODEMIN_H_
#define OKCDATAMODIFIERCLUSTERNODEMIN_H_

#include "data/multidim/OkcDataModifier.h"

#include <vector>

class OkcDataModifierClusterNodeMin : public OkcDataModifier {
public:
	OkcDataModifierClusterNodeMin();
	OkcDataModifierClusterNodeMin(OkcData* okcdata);
	virtual ~OkcDataModifierClusterNodeMin();

private:
	std::vector<double> m_clusterMin;

public:
	// return the modifier type
	XmdvTool::MODIFIERTYPE getModifierType();

	// Initialize the highlighted array,
	// its length is data_size of the OkcData
	void initClusterMinArr();
	// set the cluster max value for a specific row and col
	void setClusterMin(double max, int row, int col);
	// set the cluster max values for a specific row
	void setClusterMin(std::vector<double>& max, int row);
	// insert the cluster max values to the tail
	void insertTail(std::vector<double>& max);

	// get the cluster max value for a specific row and col
	double getClusterMin(int row, int col);
	// get the cluster max values for a specific row
	void getClusterMin(std::vector<double> &max, int row);

	void copyFrom(const OkcDataModifierClusterNodeMin* copy);
	// overloading the operator "="
	void operator=(const OkcDataModifierClusterNodeMin &copy);

};

#endif /* OKCDATAMODIFIERCLUSTERNODEMIN_H_ */
