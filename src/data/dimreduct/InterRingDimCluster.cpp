/*
 * InterRingDimCluster.cpp
 *
 *  Created on: Sep 10, 2009
 *      Ported by Zaixian Xie
 */

#include "data/dimreduct/InterRingDimCluster.h"

#include <math.h>
#include <string.h>

InterRingDimCluster::InterRingDimCluster() : InterRingCluster()
{
	name = NULL;
    weight = 1;
    PCAed = false;
    PCA1 = NULL;
}


InterRingDimCluster::~InterRingDimCluster()
{
}

void InterRingDimCluster::resetNames()
{
    if ( okc_id >= 0 )
        return;

    char temp_name[300];
    strcpy(temp_name, "");
    InterRingDimCluster *child = (InterRingDimCluster *)children;
    if ( child != NULL )
    {
        child->resetNames();
        strcat(temp_name, child->name);
        child = (InterRingDimCluster *)child->siblings;

        while ( child != NULL )
        {
            child->resetNames();
            strcat(temp_name, "_");
            strcat(temp_name, child->name);
            if ( strlen(temp_name) > 100 )
                break;
            child = (InterRingDimCluster *)child->siblings;
        }
    }
    name = new char[strlen(temp_name)+1];
    strcpy(name, temp_name);
    return;
}


double InterRingDimCluster::calculate(std::vector<double> &all_data, std::vector<double> &dim_min, std::vector<double> &dim_max)
{
    if ( okc_id >= 0 )
        return all_data[okc_id];
    else
    {
        double weight = 0;
        double sum = 0, temp;
        InterRingDimCluster *child = (InterRingDimCluster *)children;
        while ( child != NULL )
        {
            weight += child->weight;
            temp = child->calculate(all_data, dim_min, dim_max);
            if ( child->okc_id >= 0 )
            {
                 temp = (temp - dim_min[child->okc_id])/(dim_max[child->okc_id] - dim_min[child->okc_id]);
            }
            sum += temp;
            child = (InterRingDimCluster *)child->siblings;
        }
        if ( fabs(weight) < 1e-5 )
            return 0;
        else
            return sum/weight;
    }
}

int InterRingDimCluster::calculate(std::vector<int> &all_data)
{
    if ( okc_id >= 0 )
        return all_data[okc_id];
    else
    {
        double weight = 0;
        double sum = 0;
        InterRingDimCluster *child = (InterRingDimCluster *)children;
        while ( child != NULL )
        {
            weight += child->weight;
            sum += (child->calculate(all_data)) * (child->weight);
            child = (InterRingDimCluster *)child->siblings;
        }
        if ( fabs(weight) < 1e-5 )
            return 0;
        else
            return (int)(sum/weight);
    }
}
