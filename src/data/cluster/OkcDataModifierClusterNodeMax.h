/*
 * OkcDataModifierClusterNodeMax.h
 *
 *  Created on: Jul 12, 2009
 *      Ported by Zaixian Xie
 */

/*
 * This modifier contains the maximum values for each node
 * in the cluster tree.  It and the OkcDataModifierClusterNodeMin
 * represent a n-dimensional space covered by the cluster.
 * When this node is added into the OkcData as a tuple,
 * this maximum value will be added as a modifier.
 * Note that each dimension value has a maximum value
 *
 */

#ifndef OKCDATAMODIFIERCLUSTERNODEMAX_H_
#define OKCDATAMODIFIERCLUSTERNODEMAX_H_

#include "data/multidim/OkcDataModifier.h"

#include <vector>

class OkcDataModifierClusterNodeMax : public OkcDataModifier {
public:
	OkcDataModifierClusterNodeMax();
	OkcDataModifierClusterNodeMax(OkcData* okcdata);
	virtual ~OkcDataModifierClusterNodeMax();

private:
	std::vector<double> m_clusterMax;

public:
	// return the modifier type
	XmdvTool::MODIFIERTYPE getModifierType();

	// Initialize the highlighted array,
	// its length is data_size of the OkcData
	void initClusterMaxArr();
	// set the cluster max value for a specific row and col
	void setClusterMax(double max, int row, int col);
	// set the cluster max values for a specific row
	void setClusterMax(std::vector<double>& max, int row);
	// insert the cluster max values to the tail
	void insertTail(std::vector<double>& max);

	// get the cluster max value for a specific row and col
	double getClusterMax(int row, int col);
	// get the cluster max values for a specific row
	void getClusterMax(std::vector<double> &max, int row);

	void copyFrom(const OkcDataModifierClusterNodeMax* copy);
	// overloading the operator "="
	void operator=(const OkcDataModifierClusterNodeMax &copy);


};

#endif /* OKCDATAMODIFIERCLUSTERNODEMAX_H_ */
