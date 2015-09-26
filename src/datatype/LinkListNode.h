/*
 * LinkListNode.h
 *
 *  Created on: Sep 10, 2009
 *      Ported by Zaixian Xie
 */

/*
 * Class LinkListNode and class LinkList provide developers
 * a linked list.  This is directly ported from old XmdvTool.
 * Although C++ has STL container which can do the same thing,
 * we do not want to use it because it need extra time to do porting.
 */
#ifndef LINKLISTNODE_H_
#define LINKLISTNODE_H_

class LinkListNode {
public:
	void* element;
	LinkListNode* next;
	LinkListNode* prev;

public:
	LinkListNode();
	LinkListNode(void* ele);
	~LinkListNode();
};

#endif /* LINKLISTNODE_H_ */
