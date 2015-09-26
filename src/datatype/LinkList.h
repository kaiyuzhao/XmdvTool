/*
 * LinkList.h
 *
 *  Created on: Sep 10, 2009
 *      Ported by Zaixian Xie
 */

#ifndef LINKLIST_H_
#define LINKLIST_H_

class LinkListNode;

class LinkList {
protected:
    LinkListNode *head, *tail;
	int length;

public:
    LinkList();
	~LinkList();

    int GetLength();

    void AddNodeAtEnd(void* ele);
	void AddNodeAtStart(void* ele);
	void AddNodeAfter(void* new_ele, LinkListNode *node);

    bool DeleteElement(void* ele);
    bool Delete(int N);
	bool DeleteNode(LinkListNode* del_node);

    LinkListNode* GetFirst();
    LinkListNode* GetNext( LinkListNode* node);
    LinkListNode* Get( int N );
    void* GetElement(int N);

    void** TransToArray();

    bool RemoveNode(LinkListNode* del_node);
	void AddNodeAtStart(LinkListNode* new_node);

	// Clear all elements in this linked list.
	// Note that the destructor of each node will NOT be invoked.
	void clear();

	// Judge whether this list contains the pointer ele
	bool Contain(void* ele);
};

#endif /* LINKLIST_H_ */
