/*
 * DataPair.cpp
 *
 *  Created on: Sep 27, 2009
 *      Ported by Zaixian Xie
 */

#include "data/dimreduct/DataPair.h"

#include "datatype/LinkListNode.h"

DataPair::DataPair(LinkListNode *pointer1, LinkListNode *pointer2)
	: p1(pointer1), p2(pointer2) {
}

DataPair::~DataPair() {
}
