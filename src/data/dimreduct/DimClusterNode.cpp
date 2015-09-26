/*
 * DimClusterNode.cpp
 *
 *  Created on: Sep 27, 2009
 *      Ported by Zaixian Xie
 */

#include "data/dimreduct/DimClusterNode.h"

DimClusterNode::DimClusterNode() {
}

DimClusterNode::~DimClusterNode() {
	// release the memory used by data member data
    if ( data != 0 )  delete data;
}
