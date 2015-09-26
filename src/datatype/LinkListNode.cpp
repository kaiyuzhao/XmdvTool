/*
 * LinkListNode.cpp
 *
 *  Created on: Sep 10, 2009
 *      Ported by Zaixian Xie
 */

#include "datatype/LinkListNode.h"

LinkListNode::LinkListNode()
{
	next = 0;
	prev = 0;
}

LinkListNode::LinkListNode(void* ele)
{
	element = ele;
	next = 0;
	prev = 0;
}

LinkListNode::~LinkListNode()
{
}
