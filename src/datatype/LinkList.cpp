/*
 * LinkList.cpp
 *
 *  Created on: Sep 10, 2009
 *      Ported by Zaixian Xie
 */

#include "datatype/LinkList.h"
#include "datatype/LinkListNode.h"

LinkList::LinkList()
{
	head = new LinkListNode();
	tail = new LinkListNode();
	head->next = tail;
	tail->prev = head;

	length = 0;
}

LinkList::~LinkList()
{
	LinkListNode *node, *temp;
	node = head;

	while ( node != 0 )
	{
		temp = node->next;
		delete node;
		node = temp;
	}
}

void LinkList::AddNodeAtEnd(void* ele)
{
	LinkListNode *new_node, *node1;
	new_node = new LinkListNode(ele);
	node1 = tail->prev;

	node1->next = new_node;

	new_node->prev = tail->prev;
	new_node->next = tail;

	tail->prev = new_node;

	length += 1;
}


void LinkList::AddNodeAtStart(void* ele)
{
	LinkListNode *new_node, *node1;
	new_node = new LinkListNode(ele);
	node1 = head->next;

	node1->prev = new_node;

	new_node->prev = head;
	new_node->next = node1;

	head->next = new_node;

	length += 1;
}

void LinkList::AddNodeAfter(void* new_ele, LinkListNode *node)
{
	LinkListNode *new_node;
	new_node = new LinkListNode(new_ele);

	new_node->prev = node;
    new_node->next = node->next;

    node->next = new_node;

    new_node->next->prev = new_node;

	length += 1;
}

int LinkList::GetLength()
{
	return length;
}


LinkListNode* LinkList::GetFirst()
{
    if ( head->next != tail )
        return head->next;
    else
        return 0;
}


LinkListNode* LinkList::GetNext( LinkListNode* node)
{
    if ( node->next != tail )
        return node->next;
    else
        return 0;
}

void** LinkList::TransToArray()
{
    if ( length <= 0 )
        return 0;

    void** node_array = new void* [length];
    LinkListNode* node = GetFirst();
    for ( int i = 0; i < length; i++ )
    {
        node_array[i] = (void*) node->element;
        node = GetNext(node);
    }
    return node_array;
}

bool LinkList::RemoveNode(LinkListNode* del_node)
{
	LinkListNode *node1, *node2;
	node1 = del_node->prev;
    node2 = del_node->next;

	node1->next = node2;
	node2->prev = node1;

	length -= 1;
	return true;
}

bool LinkList::DeleteElement(void* ele)
{
	LinkListNode *node1, *node2, *del_node;
	node1 = head;
	node2 = node1->next;

	while ( node2 != tail && node2->element != ele )
	{
		node1 = node2;
		node2 = node2->next;
	}

	if ( node2 == tail )
		return false;

    del_node = node2;
	node2 = del_node->next;

	delete del_node;

	node1->next = node2;
	node2->prev = node1;

	length -= 1;

	return true;
}

bool LinkList::Delete(int N)
{
    if ( N >= length || N < 0 )
        return false;

    LinkListNode *node, *next_node;
    node = head;
    next_node = node->next;
    for ( int i = 1; i <= N; i++ )
    {
        node = next_node;
        next_node = node->next;
    }

    node->next = next_node->next;
    next_node->next->prev = node;
    delete next_node;
	length -= 1;

    return true;
}

bool LinkList::DeleteNode(LinkListNode* del_node)
{
	if ( del_node == head || del_node == tail )
		return false;

	LinkListNode *node1, *node2;
	node1 = head;
	node2 = node1->next;

	while ( node2 != tail && node2 != del_node )
	{
		node1 = node2;
		node2 = node2->next;
	}

	if ( node2 == tail )
		return false;

	node2 = del_node->next;

	delete del_node;

	node1->next = node2;
	node2->prev = node1;

	length -= 1;

	return true;
}

void LinkList::AddNodeAtStart(LinkListNode* new_node)
{
	LinkListNode *node1;
	node1 = head->next;

	node1->prev = new_node;

	new_node->prev = head;
	new_node->next = node1;

	head->next = new_node;

	length += 1;
}

void LinkList::clear()
{
    int i, n = this->GetLength();
    for ( i = n-1; i >= 0; i-- )
    {
        this->Delete(i);
    }
}

bool LinkList::Contain(void* ele) {
	// Get the first node
	LinkListNode* p = GetFirst();
	while (p) {
		// If this list contains this element, return true
		if ( (void*)(p->element) == ele ) {
			return true;
		}
		// Get the next node
		p = GetNext(p);
	}

	// Arriving here means that the element does not exist in the list
	return false;
}
