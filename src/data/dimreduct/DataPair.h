/*
 * DataPair.h
 *
 *  Created on: Sep 27, 2009
 *      Ported by Zaixian Xie
 */

/*
 * Class DataPair is a data structure used in online generating dimension cluster tree
 */
#ifndef DATAPAIR_H_
#define DATAPAIR_H_

class LinkListNode;

class DataPair {
public:
	DataPair(LinkListNode *pointer1, LinkListNode *pointer2);
	virtual ~DataPair();
public:
	LinkListNode *p1, *p2;
};

#endif /* DATAPAIR_H_ */
