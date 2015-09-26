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

/*
void InterRingDimCluster::calculateWithDimClusterExtends(std::vector<double> all_data, double &mean, double &max, double &min)
{
    if ( okc_id >= 0 )
    {
        mean = max = min = all_data[okc_id];
        return;
    }
    else
    {
        double total_weight = 0;
        double sum = 0;
        double child_mean, child_max, child_min, total_max, total_min;
        bool first = true;
        InterRingDimCluster *child = (InterRingDimCluster *)children;
        while ( child != NULL )
        {
            total_weight += child->weight;
            child->calculateWithDimClusterExtends(all_data, child_mean, child_max, child_min);
            if ( child->okc_id >= 0 )
            {
                 child_mean = (child_mean - g_globals.dim_reducer->dim_min[child->okc_id])/(g_globals.dim_reducer->dim_max[child->okc_id] -
                         g_globals.dim_reducer->dim_min[child->okc_id]);
                 child_max = (child_max - g_globals.dim_reducer->dim_min[child->okc_id])/(g_globals.dim_reducer->dim_max[child->okc_id] -
                         g_globals.dim_reducer->dim_min[child->okc_id]);
                 child_min = (child_min - g_globals.dim_reducer->dim_min[child->okc_id])/(g_globals.dim_reducer->dim_max[child->okc_id] -
                         g_globals.dim_reducer->dim_min[child->okc_id]);
            }
            sum += child_mean * child->weight;
            if ( first )
            {
                total_max = child_max;
                total_min = child_min;
                first = false;
            }
            else
            {
                if ( total_max < child_max )
                    total_max = child_max;
                if ( total_min > child_min )
                    total_min = child_min;
            }
            child = (InterRingDimCluster *)child->siblings;
        }
        if ( fabs(total_weight) < 1e-5 )
            mean = 0;
        else
            mean = sum/total_weight;

        max = total_max;
        min = total_min;
}
*/

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

/*
void InterRingDimCluster::changeSelectedStatus(bool mark)
{
	if ( this->selected != mark )
    {
        if ( this->selected == true )
        {
            g_globals.dim_reducer->DeleteRepresentativeDim(this);
        }
        else
        {
            g_globals.dim_reducer->AddRepresentativeDim(this);
        }

        this->selected = mark;
    }
}
*/
//start_jing_reorder

/*
void InterRingDimCluster::getClusterData(int& n, int& m, double **&data)
{
    InterRingDimCluster *child = (InterRingDimCluster *)children;
    int children_number = 0;
    while ( child != NULL )
    {
        child->PCA();
        children_number ++;
        child = (InterRingDimCluster *)child->siblings;
    }

    n = g_globals.dataset->data_size;
	m = children_number;

	data = matrix(n, m);

    int i, j;
    child = (InterRingDimCluster *)children;
    j = 0;
    while ( child != NULL )
    {
        double *child_PCA1;
        child_PCA1 = child->GetPCA1();

        if ( child_PCA1 != NULL )
        {
            for ( i = 0; i < n; i++ )
                data[i][j] = child_PCA1[i];
        }
        else
        {
            int all_dims = g_globals.dataset->all_dims;
            int dim_id = child->okc_id;
            int base = dim_id;
            for ( i = 0; i < n; i++ )
            {
                data[i][j] = g_globals.dataset->data_buf[base];
                base += dim_id;
            }
        }

        child = (InterRingDimCluster *)child->siblings;
        j++;
    }
 }
*/

/*
//return NULL if this is a leaf dimension.
double* InterRingDimCluster::getPCA1()
{
    if ( !PCAed )
        this->PCA();

    return PCA1;

}
*/
/*
bool InterRingDimCluster::PCA()
{
    if ( PCAed == true )
        return true;

    //leaf dimension
    if ( okc_id >= 0 )
    {
        PCAed = true;
        return true;
    }

    int i, n, m;
    double **data;

    getClusterData(n, m, data);

    int prin_num = 1;
	double **principal;
	double *principal_max;
	double *principal_min;
	double *pca1_contributions;
    principal = new double *[prin_num];
	for ( i = 0; i < prin_num; i++ )
		principal[i] = new double[n];
	principal_max = new double[prin_num];
	principal_min = new double[prin_num];
    pca1_contributions = new double[m];

	if ( !principal_Components_Analysis_2(n, m, data, prin_num, principal, principal_max, principal_min, pca1_contributions) )
	{
        free_matrix(data, n, m);
		for ( i = 0; i < prin_num; i++ )
			delete principal[i];
		delete [] principal;
		delete [] principal_max;
		delete [] principal_min;
    	delete [] pca1_contributions;
		return false;
	}

    PCA1 = principal[0];
    PCA1_min = principal_min[0];
    PCA1_max = principal_max[0];
    int j = 0;
    InterRingDimCluster *child = (InterRingDimCluster *)children;
    while ( child != NULL )
    {
        child->PCA1_contribution = pca1_contributions[j];
        child = (InterRingDimCluster *)child->siblings;
        j++;
    }


//    free_matrix(data, n, m);
	delete [] principal;
	delete [] principal_max;
	delete [] principal_min;
    delete [] pca1_contributions;

    PCAed = true;

    return true;
}
*/

/*
bool InterRingDimCluster::setReorderParaAccordingToPCA()
{
    if ( !PCAed )
        return false;

    reorder_para = fabs(PCA1_contribution) ;

    InterRingDimCluster *child = (InterRingDimCluster *)children;
    while ( child != NULL )
    {
        child->SetReorderParaAccordingToPCA();
        child = (InterRingDimCluster *)child->siblings;
    }

    return true;
}
*/

/*
/////////choice: 1,optimal reorder; 2,random reorder; 3,artificial ants reorder////////////////////////////////////
bool InterRingDimCluster::setReorderParaAccordingToChoice(int choice)
{
    int n, m;
    double **data;
    GetClusterData(n, m, data);

  //  double *order_para = new double[m];
	std::vector<int> order_para(m);
    if ( !similarity_reorder_interring(n, m, data, order_para, choice, 1, "sim") )

		return false;

    int j = 0;
    InterRingDimCluster *child = (InterRingDimCluster *)children;
    while ( child != NULL )
    {
        child->reorder_para = order_para[j];
        child = (InterRingDimCluster *)child->siblings;
        j++;
    }
    return true;
}
*/

/*
bool InterRingDimCluster::setReorderPara(REORDER_METHOD choice)
{
    if ( choice == PCA_REORDER )
    {
        if ( PCA() )
        return SetReorderParaAccordingToPCA();
    }
    else if ( choice == OPTIMAL_REORDER )
        return SetReorderParaAccordingToChoice(1);
    else if ( choice == RANDOM_REORDER )
        return SetReorderParaAccordingToChoice(2);

    return false;
}
*/


/*
bool InterRingDimCluster::setReorderPara()
{
    return SetReorderPara(PCA_REORDER);
}
*/
