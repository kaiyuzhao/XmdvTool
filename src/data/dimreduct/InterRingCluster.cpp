/*
 * InterRingCluster.cpp
 *
 *  Created on: Sep 10, 2009
 *      Ported by Zaixian Xie
 */

#include "data/dimreduct/InterRingCluster.h"

#include <math.h>
#include <string.h>
#include "main/XmdvTool.h"
#include "datatype/LinkList.h"
#include "color/ColorManager.h"

// What is this?  We shold avoid static variables outside classes
static double alpha, min_angle;

InterRingCluster::InterRingCluster()
	// Initializing the constant variables
	: COLORCOEF(0.70833333)
{
    parent = NULL;
    children = NULL;
	siblings = NULL;
    selected = false;
    show_children = true;
    pinned = false;
    current = false;
	radius = 0;
    name = NULL;
    reorder_para = 1;

    distorted = false;
}

InterRingCluster::~InterRingCluster()
{
    if ( children != NULL )
        delete children;
    if ( siblings != NULL )
        delete siblings;
    if ( name!= NULL )
        delete [] name;
}


//reset level of this node and its children and next silbings, find the maximum level among them;
//current_level: level to be signed to this node; max_level: will be the maximum level of itself, its children and next silbings
void InterRingCluster::resetLevels(int current_level, int &max_level)
{
	level = current_level;

	if ( level > max_level )
		max_level = level;

	if ( children != NULL && show_children == true)
		children->resetLevels(current_level+1, max_level);
	if ( siblings != NULL )
		siblings->resetLevels(current_level, max_level);
}


void InterRingCluster::resetEntries()
{
	if ( children == NULL )
    {
        entries = 1;
        return;
    }

    int count = 0;
    InterRingCluster *child = children;
    while ( child != NULL )
    {
        child->resetEntries();
        count += child->entries;
        child = child->siblings;
    }
    entries = count;

    if ( siblings != NULL )
		siblings->resetEntries();
}

/*
void InterRingCluster::resetCurrentList(LinkList* current_list)
{
	if ( current )
		current_list->AddNodeAtEnd(this);

	if ( siblings != NULL )
		siblings->resetCurrentList(current_list);

	if ( children != NULL )
		children->resetCurrentList(current_list);
}
*/

//reset the start_angle of this node;
//start_angle: the new start angle to be assigned to this node; unit_angle: angle responds to one dimension;
void InterRingCluster::resetAngles(double start_angle, double unit_angle)
{
	this->start_angle = start_angle;
	this->sweep_angle = unit_angle * entries;

	if ( children != NULL )
		children->resetAngles(start_angle, unit_angle);
	if ( siblings != NULL )
		siblings->resetAngles(getEndAngle(), unit_angle);
}


double InterRingCluster::getEndAngle()
{
	double angle = start_angle + sweep_angle;
	while ( angle >= 360 )
	{
		angle = angle - 360;
	}
	return angle;
}


double InterRingCluster::getMidAngle()
{
	double angle = start_angle + sweep_angle/2.;
	if ( angle >= 360 )
		angle = angle - 360;
	return angle;
}

/*
//reset color of this node; color responds to its mid angle.
void InterRingCluster::ResetColors(colormap CMAP)
{
	double mid_angle = GetMidAngle();
	double color_idx_test_punit = mid_angle * COLORCOEF;
    int color_idx = (int) color_idx_test_punit;
	color = CMAP[color_idx];

	if ( children != NULL )
		children->ResetColors(CMAP);
	if ( siblings != NULL )
		siblings->ResetColors(CMAP);
}
*/


void InterRingCluster::resetColors()
{
	ColorManager *cm = g_globals.colorManager;
	if ( children == NULL )
	{
		double mid_angle = getMidAngle();
		double color_idx_test_punit = mid_angle * COLORCOEF;
//		int color_idx = (int)mid_angle * COLORCOEF;
        int color_idx = (int) color_idx_test_punit;
		color = cm->getColor(ColorManager::BRUSH_UNSELECTED, color_idx/256.0);
		return;
	}

	double r, g, b;
	r = 0;
	g = 0;
	b = 0;

	InterRingCluster* child = children;
	while ( child != NULL )
	{
		child->resetColors();
		r += child->color.R * child->entries;
		g += child->color.G * child->entries;
		b += child->color.B * child->entries;
		child = child->siblings;
	}

	r /= entries;
	g /= entries;
	b /= entries;

	color.R = r;
	color.G = g;
	color.B = b;

	return;
}

//add a child to this node; the new child will be at the beginning of the child link list.
//child: the new child to be added;
void InterRingCluster::addChild(InterRingCluster *child)
{
    child->siblings = children;
    child->parent = this;
    child->level = level+1;
    children = child;
}

//add a silbing to this node; the new sibling will be its next sibling;
//sibling: the new silbing to be added;
void InterRingCluster::addSibling(InterRingCluster *sibling)
{
    sibling->parent = parent;
    sibling->level = level;
    sibling->siblings = siblings;
    siblings = sibling;
}

//remove a child of this node;
//rm_child: the child to be removed;
void InterRingCluster::removeChild(InterRingCluster *rm_child)
{
    InterRingCluster* child = children;
	InterRingCluster* next_child;
    if ( child == rm_child )
    {
         children = child->siblings;
    }
    else
    {
        while ( child != NULL )
        {
            next_child = child->siblings;
            if ( next_child == rm_child )
            {
                 child->siblings = next_child->siblings;
                 rm_child->siblings = NULL;
                 rm_child->parent = NULL;
                 child = NULL;
            }
            else
            {
                child = next_child;
            }
        }
    }

    if ( children == NULL )
        parent->removeChild(this);
    return;
}


//distortion
//return the distance from start_angle to end_angle
//  start_angle            end_angle
//       | --  distance   ------|
double InterRingCluster::getAnglesDistance(double start_angle, double end_angle)
{
	if ( end_angle < start_angle )
		end_angle += 360;
	return end_angle - start_angle;
}

void InterRingCluster::changeSweepAngle()
{
	if ( alpha > 1. )
		increaseSweepAngle();
	else
		decreaseSweepAngle();
}


//Increase sweep_angle by alpha, alpha > 0
void InterRingCluster::increaseSweepAngle()
{
	sweep_angle *= alpha;

	InterRingCluster* child = children;
	while ( child != NULL )
	{
		child->increaseSweepAngle();
		child = child->siblings;
	}
	return;
}


//decrease sweep_angle by alpha, alpha < 0, sweep_angle should > entries*min_angle
void InterRingCluster::decreaseSweepAngle()
{
    if ( children == NULL )
	{
		sweep_angle *= alpha;
		if ( sweep_angle < entries*min_angle )
			sweep_angle = entries*min_angle;
		return;
	}

	InterRingCluster* child = children;
    double new_sweep_angle = 0;
	while ( child != NULL )
	{
		child->decreaseSweepAngle();
		new_sweep_angle += child->sweep_angle;
		child = child->siblings;
	}

	sweep_angle = new_sweep_angle;
	return;
}


//change start angle of this and all its children.
void InterRingCluster::changeStartAngle(double new_start_angle)
{
    start_angle = new_start_angle;

    InterRingCluster* child = children;
	double angle = start_angle;
    while ( child != NULL )
    {
        child->changeStartAngle( angle );
		angle = child->getEndAngle();
		child = child->siblings;
    }
}


//return double between 0 and 360;
double InterRingCluster::standarizeAngle(double angle)
{
	while ( angle < 0 )
	{
		angle += 360;
		return angle;
	}

	while ( angle > 360 )
	{
		angle -= 360;
		return angle;
	}

    return angle;
}

InterRingCluster* InterRingCluster::findLastSibling()
{
	InterRingCluster *sibling = this;
	while ( sibling->siblings != NULL )
	{
		sibling = sibling->siblings;
	}
	return sibling;
}


InterRingCluster* InterRingCluster::findBeforeSibling()
{
	InterRingCluster *sibling = this->parent->children;
	while ( siblings != NULL && sibling->siblings != this )
	{
		sibling = sibling->siblings;
	}
	return sibling;
}

void InterRingCluster::changeSweepAngle(bool toward_left, double new_angle, double minimum_angle)
{
	if ( toward_left )
	{
		double delta_angle = start_angle - new_angle;
		if ( delta_angle < -180 )
			delta_angle += 360;
		else if ( delta_angle > 180 )
			delta_angle -= 360;

		InterRingCluster* node = this;
        if ( node->level == 1 )
            return;
		//if a node is the first child of its parent, it can't move leftward;
		while ( node->parent->children == node )
		{
			if ( fabs(node->parent->sweep_angle - 360) < minimum_angle/100 )
			{
				if ( node->siblings == NULL ) //node is the only child;
				{
					node->parent->changeStartAngle(new_angle);
					return;
				}
				else
				{
					InterRingCluster* last_sibling = node->findLastSibling();
					last_sibling->siblings = node;
					node->parent->start_angle = node->siblings->start_angle;
					node->parent->children = node->siblings;
					node->siblings = NULL;
				}
			}
			else
			{
				node = node->parent;
			}
		}

		node->changeSweepAngleTowardLeft(delta_angle, minimum_angle);
        //try
        //node->ChangeSweepAngleTowardRight(delta_angle, minimum_angle);
	}
	else //toward right;
	{
		double delta_angle = new_angle - getEndAngle();
		if ( delta_angle < -180 )
			delta_angle += 360;
		else if ( delta_angle > 180 )
			delta_angle -= 360;

		InterRingCluster* node = this;
        if ( node->level == 1 )
            return;
		//if a node is the last child of its parent, it can't move rightward;
		while ( node->siblings == NULL )
		{
			if ( fabs(node->parent->sweep_angle - 360) < minimum_angle / 100 )
			{
				if ( node->parent->children == node )   //node is the only child
				{
					double new_start_angle = standarizeAngle(node->parent->start_angle + delta_angle);
					node->parent->changeStartAngle(new_start_angle);
					return;
				}
				else
				{
					InterRingCluster* before_sibling = node->findBeforeSibling();
					before_sibling->siblings = NULL;
					node->siblings = node->parent->children;
					node->parent->children = node;
					node->parent->start_angle = node->start_angle;
				}
			}
			else
			{
				node = node->parent;
			}
		}

		node->changeSweepAngleTowardRight(delta_angle, minimum_angle);
        //try
        //node->ChangeSweepAngleTowardLeft(delta_angle, minimum_angle);

	}
}

//   |<-------------|
//    delta_angle   start_angle
void InterRingCluster::changeSweepAngleTowardLeft(double delta_angle, double minimum_angle)
{
	InterRingCluster* sibling;

	//change the sweep angles.
    min_angle = minimum_angle;
	double remain_angle = getAnglesDistance(parent->start_angle, start_angle);

	if ( delta_angle > 0 )  //this sweep angle increases, left sibling's sweep angles decrease
	{
		//calculate alpha for silbings
		if ( delta_angle > remain_angle)
		    alpha = 0;
		else
			alpha = 1. - delta_angle / remain_angle;

		double total = 0.;

		sibling = parent->children;
		while ( sibling != this )
		{
			sibling->changeSweepAngle();
			total += sibling->sweep_angle;
			sibling = sibling->siblings;
		}

		//calculate alpha for this
		alpha = 1. + ( remain_angle - total ) / sweep_angle;

		this->changeSweepAngle();
	}
	else  //this sweep angle decreases, left siblings' sweep angles increase
	{
		double old_sweep_angle = sweep_angle;
		//calculate alpha for this
		if ( sweep_angle + delta_angle < 0 )
		{
			alpha = 0;
		}
		else
		{
			alpha = 1. + delta_angle / sweep_angle;
		}

		this->changeSweepAngle();

		//calculate alpha for siblings
		delta_angle = sweep_angle - old_sweep_angle;
		alpha = 1. - delta_angle / remain_angle;

		sibling = parent->children;
		while ( sibling != this )
		{
			sibling->changeSweepAngle();
			sibling = sibling->siblings;
		}
	}

	//change the start angles;
	sibling = parent->children;
	double new_start_angle = sibling->start_angle;
	while ( sibling != this)
	{
		sibling->changeStartAngle(new_start_angle);
		new_start_angle = sibling->getEndAngle();
		sibling = sibling->siblings;
	}
	this->changeStartAngle(new_start_angle);
}



//   |------------->|
//end_angle  delta_angle
void InterRingCluster::changeSweepAngleTowardRight(double delta_angle, double minimum_angle)
{
	InterRingCluster* sibling;
	//change the sweep angles.
    min_angle = minimum_angle;
    double remain_angle = getAnglesDistance(getEndAngle(), parent->getEndAngle());

	if ( delta_angle > 0 )  //this sweep angle increases, right sibling's sweep angles decrease
	{
		if ( delta_angle > remain_angle)
		    alpha = 0;
		else
			alpha = 1. - delta_angle / remain_angle;

		double total = 0.;

		sibling = this->siblings;
		while ( sibling != NULL )
		{
			sibling->changeSweepAngle();
			total += sibling->sweep_angle;
			sibling = sibling->siblings;
		}

		alpha = 1. + ( remain_angle - total ) / sweep_angle;

		this->changeSweepAngle();
	}
	else  //this sweep angle decreases, left siblings' sweep angles increase
	{
		double old_sweep_angle = sweep_angle;

		if ( sweep_angle + delta_angle < 0 )
		{
			alpha = 0;
		}
		else
		{
			alpha = 1. + delta_angle / sweep_angle;
		}

		this->changeSweepAngle();

		delta_angle = sweep_angle - old_sweep_angle;
		alpha = 1. - delta_angle / remain_angle;

		sibling = this->siblings;
		while ( sibling != NULL )
		{
			sibling->changeSweepAngle();
			sibling = sibling->siblings;
		}
	}

	//change the start angles;
	sibling = this;
	double new_start_angle = sibling->start_angle;
	while ( sibling != NULL)
	{
		sibling->changeStartAngle(new_start_angle);
		new_start_angle = sibling->getEndAngle();
		sibling = sibling->siblings;
	}
}


//judge if an angle is in the node
//angle: the angle to be judged
//return true is angle is in the node.
bool InterRingCluster::isAngleIn(double angle)
{
	if ( angle < start_angle )
		angle += 360;

	if ( angle <= ( start_angle + sweep_angle ) )
		return true;
	else
		return false;
}

//judge if an angle is near the start_angle or end_angle of or in the node.
//angle: the angle to be judged; eps: the error control parater;
//return true mean that angle is near the start_angle or end_angle of the node;
//if return true and start = true, angle is near the start_angle of the node;
//if return true and end = true, angle is near the end_angle of the node;
//if return true and in = true, angle is in the end_angle of the node;
bool InterRingCluster::isAngleIn(double angle, double eps, bool &start, bool &end, bool &in)
{
    eps = this->sweep_angle / 3.0;
	in = isAngleIn(angle);
	if ( ! in )
	{
		start = false;
		end = false;
		return false;
	}

	start = isAngleNear(start_angle, angle, eps);
	end = isAngleNear(getEndAngle(), angle, eps);

	if ( start || end || in )
		return true;
	else
		return false;
}


//judge if angle is near angle1
//eps: the error control parater;
//return true mean that angle is near angle1;
bool InterRingCluster::isAngleNear(double angle1, double angle, double eps)
{
	double start_angle1 = angle1 - eps;

	if ( start_angle1 < 0 )
		start_angle1 += 360;

	if ( angle < start_angle1 )
		angle += 360;

	if ( angle <= ( start_angle1 + 2*eps ) )
		return true;
	else
		return false;
}

//Given a level and angle, judge if this node is of this level and covers the angle
InterRingCluster* InterRingCluster::findNodeAccordingLevelAndAngle(int level, double angle)
{
    if ( isAngleIn(angle) )
    {
        if ( this->level == level )
            return this;
        else if ( this->level < level )
        {
            if ( children != NULL )
                return children->findNodeAccordingLevelAndAngle(level, angle);
        }
    }
    else if ( siblings != NULL )
    {
        return siblings->findNodeAccordingLevelAndAngle(level, angle);
    }

    return NULL;
}


//Given a level and angle, judge if this node is of this level, and
//the angle near the node's start angle
//or near end angle
//or in the angle
//eps: error control parameter;
//start = true means the angle is near the start_angle of the node
//end = true means the angle is near the end_angle of the node
InterRingCluster* InterRingCluster::findNodeAccordingLevelAndAngle(int level, double angle, double eps,
														 bool &start, bool &end, bool &in)
{
    if ( isAngleIn(angle, eps, start, end, in) )
    {
        if ( this->level == level )
            return this;
        else if ( this->level < level )
        {
			bool near_this_end = end;
			InterRingCluster* result = NULL;

            if ( children != NULL )
                result = children->findNodeAccordingLevelAndAngle(level, angle, eps, start, end, in);

			if ( result != NULL || !near_this_end )
				return result;
			else if ( siblings != NULL )
			{
				return siblings->findNodeAccordingLevelAndAngle(level, angle, eps, start, end, in);
			}
        }
    }
    else if ( siblings != NULL )
    {
        return siblings->findNodeAccordingLevelAndAngle(level, angle, eps, start, end, in);
    }

    return NULL;
}


int InterRingCluster::getEntries()
{
    return entries;
}

double InterRingCluster::getRadius()
{
    return radius;
}


void InterRingCluster::rotate(double rotate_angle)
{
    this->start_angle += rotate_angle;
    this->standarizeAngle(this->start_angle);

    if (this->children != NULL )
        this->children->rotate(rotate_angle);
    if ( this->siblings != NULL )
        this->siblings->rotate(rotate_angle);
    return;
}


void InterRingCluster::sizeStructureSelect(int threshold, bool mark )
{
	if ( entries <= threshold || entries == 1 )
    {
        changeSelectedStatus(mark);
    }
    else
    {
        InterRingCluster *child = children;

	    while ( child != NULL )
        {
		    child->sizeStructureSelect(threshold, mark );
		    child = child->siblings;
        }
    }
}


void InterRingCluster::radiusStructureSelect(double threshold, bool mark )
{
	if ( radius <= threshold || fabs(radius) < 1e-5 )
    {
        changeSelectedStatus(mark);
    }
    else
    {
        InterRingCluster *child = children;

	    while ( child != NULL )
        {
		    child->radiusStructureSelect(threshold, mark );
		    child = child->siblings;
        }
    }
}


void InterRingCluster::filter(double similarity_threshold, double importance_threshold)
{
    if ( reorder_para < importance_threshold )
        return;

    if ( radius < similarity_threshold )
    {
        if ( children == NULL )
            changeSelectedStatus(true);
        else
            children->filter(similarity_threshold, importance_threshold);
    }
    else
    {
        InterRingCluster *child = children;

	    while ( child != NULL )
        {
		    child->filter(similarity_threshold, importance_threshold);
		    child = child->siblings;
        }
    }
}



void InterRingCluster::changeSelectedStatus(bool)
{
}

//reset selected mark;
//mark: the new selected mark to be set;
void InterRingCluster::resetSelected(bool mark)
{
    changeSelectedStatus(mark);

    InterRingCluster *child = children;

	while ( child != NULL )
    {
		child->resetSelected(mark);
		child = (InterRingCluster *)(child->siblings);
    }
}

bool InterRingCluster::setReorderPara()
{
    return true;
}

//reorder siblings according to their reorder_para
bool InterRingCluster::reorder()
{
    if ( children == NULL || children->siblings == NULL )
        return true;

    InterRingCluster *child = children;
    while ( child != NULL )
    {
        child->reorder();
        child = child->siblings;
    }

    InterRingCluster *new_list_head, *new_list_tail, *bigest, *pre_bigest, *old_list_head, *current;

    new_list_tail = new_list_head = new InterRingCluster;

    old_list_head = new InterRingCluster;
    old_list_head->siblings = children;

    while ( old_list_head->siblings != NULL )
    {
        pre_bigest = old_list_head;
        current = bigest = old_list_head->siblings;
        while ( current->siblings != NULL )
        {
            if ( current->siblings->reorder_para > bigest->reorder_para )
            {
                pre_bigest = current;
                bigest = current->siblings;
            }

            current = current->siblings;
        }
        pre_bigest->siblings = bigest->siblings;
        bigest->siblings = NULL;
        new_list_tail->siblings = bigest;
        new_list_tail = bigest;
    }

    children = new_list_head->siblings;
    /*    InterRingCluster *next_child, *bigest, *pre_bigest, *head, *tail;

    tail = NULL;
    head = children;
    while ( head->siblings != NULL )
    {
        bigest = head;
        pre_bigest = head;
        next_child = head->siblings;
        while ( next_child != NULL )
        {
            if ( bigest->reorder_para < next_child->reorder_para )
                bigest = next_child;
            pre_bigest = next_child;
            next_child = next_child->siblings;
        }

        if ( bigest == head )
            head = bigest->siblings;
        else
            pre_bigest->siblings = bigest->siblings;
        bigest->siblings = NULL;

        if ( tail == NULL )
            tail = bigest;
        else
            tail->siblings = bigest;
        tail = tail->siblings;
    }

    children = tai
*/
    return true;
}


