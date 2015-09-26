/*
 * InterRingCluster.h
 *
 *  Created on: Sep 10, 2009
 *      Ported by Zaixian Xie
 */

/*
 * Class InterRingCluster represents a node in the dimension cluster tree.
 * It can be an original dimension, or a derived dimension containing
 * multiple dimensions.
 */

#ifndef INTERRINGCLUSTER_H_
#define INTERRINGCLUSTER_H_

#include "datatype/RGBt.h"

class LinkList;

class InterRingCluster {
public:
	InterRingCluster();
	virtual ~InterRingCluster();

private:
	// This is used to calculate the color for each cluster
	const double COLORCOEF;// 0.70833333

public:
    //tree structure related fields;
    InterRingCluster *parent;  //pointer to its parent;
    InterRingCluster *children;  //pointer to its first children;
    InterRingCluster *siblings;  //pointer to its next sibling;

    //display related fields;
    int entries;  //number of original dimensions contained in this node. leaf node, entries = 1;
    int level;  //root node, level = 1; else level = parent->level+1;
    double start_angle;  //start angle in interRing;
    double sweep_angle;  //sweep angle in interRing;
    bool selected;
    bool pinned; //if pinned in circular distortion with pin mode
    bool show_children;
	bool current;  //current = true means this node need to be shown on screen;
    RGBt color;  //colormap[color_idx] is the color for this node in interRing;
    char *name; //name of the cluster
    double reorder_para; //parameter that decides the order of siblings
	float clustqual, dataqual;

    double radius;  //cluster radius;

    bool distorted; //for circular distortion feedback

public:

	//reset level of this node and its children and next silbings, find the maximum level among them;
	//current_level: level to be signed to this node;
	//max_level: will be the maximum level of itself, its children and next silbings
	void resetLevels(int current_level, int &max_level);
    void rotate(double degree);

    void resetEntries();
	//reset the start_angle of this node;
	//start_angle: the new start angle to be asigned to this node;
	//unit_angle: angle responds to one dimension;
	void resetAngles(double start_angle, double unit_angle);

	//reset color of this node; color responds to its mid angle.
	void resetColors();
    //reorder siblings according to their reorder_para
    bool reorder();

	//void resetCurrentList(LinkList* current_list);

	//add a child to this node; the new child will be at the beginning of the child link list.
	//child: the new child to be added;
	void addChild(InterRingCluster *child);

	//add a silbing to this node; the new sibling will be its next sibling;
	//sibling: the new silbing to be added;
	void addSibling(InterRingCluster *sibling);

	//remove a child of this node;
	//rm_child: the child to be removed;
	void removeChild(InterRingCluster *rm_child);

	//get the end angle of the node in interRing;
	double getEndAngle();

	//get the mid angle of the node in interRing;
	double getMidAngle();

    int getEntries();

	double getRadius();

	void changeSweepAngle(bool toward_left, double new_angle, double minimum_angle);

	//   |<-------------|
	//    delta_angle   start_angle
	void changeSweepAngleTowardLeft(double delta_angle, double minimum_angle);

	//   |------------->|
	//end_angle  delta_angle
	void changeSweepAngleTowardRight(double delta_angle, double minimum_angle);

	InterRingCluster* findNodeAccordingLevelAndAngle(int level, double angle);

	InterRingCluster* findNodeAccordingLevelAndAngle(int level, double angle, double eps, bool &start, bool &end, bool &in);

    void getCenterPositon();

    void resetSelected(bool mark);

    void sizeStructureSelect(int threshold, bool mark );

    void radiusStructureSelect(double threshold, bool mark );

    void filter(double similarity_threshold, double importance_threshold);

    virtual void changeSelectedStatus(bool mark);

    virtual bool setReorderPara();

protected:

	//return the distance from start_angle to end_angle
	//  start_angle            end_angle
	//       | --  distance   ------|
	double getAnglesDistance(double start_angle, double end_engle);

	void changeSweepAngle();

	//Increase sweep_angle by alpha, alpha > 0
	void increaseSweepAngle();

	//decrease sweep_angle by alpha, alpha < 0, sweep_angle should > entries*min_angle
	void decreaseSweepAngle();

	//change start angle of this and all its children.
	void changeStartAngle(double new_start_angle);

	bool isAngleIn(double angle);

	bool isAngleIn(double angle, double eps, bool &start, bool &end, bool &in);

	bool isAngleNear(double angle1, double angle, double eps);

	double standarizeAngle(double angle);

	InterRingCluster* findLastSibling();

	InterRingCluster* findBeforeSibling();

};

#endif /* INTERRINGCLUSTER_H_ */
