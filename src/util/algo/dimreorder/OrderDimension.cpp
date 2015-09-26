/*
 * OrderDimension.cpp
 *
 *  Created on: Nov 14, 2009
 *      Ported by Zaixian Xie
 */

#include "util/algo/dimreorder/OrderDimension.h"

#include <cmath>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <list>
#include <time.h>
#include <vector>

#include <QMessageBox>
using namespace std;

#include "main/XmdvTool.h"
#include <vector>
#include "datatype/Vec2.h"
#include "data/multidim/OkcData.h"
#include "data/multidim/DimOOOMap.h"

OrderDimension::OrderDimension() {
}

OrderDimension::~OrderDimension() {
}

void OrderDimension::DimensionReorder(OkcData* inputOkcData, DimOOOMap* map, Reorder_Parameters* reorderPara,
		OrderDimension::ORDERMETHOD method, XmdvTool::VISTYPE visType)
{
	// This function considers only the visible dimensions using the array on in the DimOOOMap
	int i,j;
	double **datamatrix = 0;
	double **norm_matrix = 0;

	// data_size is the number of tuples
	int data_size = inputOkcData->getDataSize();
	if( data_size <= 1)
		return;
	// all_dims is the number of dimensions before turning off some of them
	int all_dims = inputOkcData->getDimSize();
	// dims is the number of dimension after turning off some of them
	int dims = map->getViewDimSize();

	// all_data is a buffer to represent a tuple before dimension on/off/reordering
	// all_dim_max and all_dim_min are maximum and minimum values for each dimension
	// before dimension on/off/reordering
	std::vector<double> all_data(all_dims), all_dim_max(all_dims), all_dim_min(all_dims);
	std::vector<bool> on(all_dims);
	std::vector<int> map_in_to_out(all_dims);
	map->getOn(on);
	map->getMapInToOut(map_in_to_out);
	// Get the maximum and minimum values for each dimension before applying
	// dimension on/off/reordering
	inputOkcData->getDimMaxArr(all_dim_max);
	inputOkcData->getDimMinArr(all_dim_min);

	if ( (method == OrderDimension::OM_CLUTTER)
			&& ( ( visType == XmdvTool::VISTYPE_PA ) || ( visType == XmdvTool::VISTYPE_GL ) ) ) {
		// If under "clutter-based" mode and the visualization is parallel coordinates or glyphs,
		// we use normalized values

		// Initialize the normalized data matrix
		norm_matrix = new double *[data_size];
		for(i = 0; i < data_size; i++)
			norm_matrix[i] = new double [dims];

		// ratio[i] = 1.0/(all_dim_max[i]-all_dim_min[i])
		std::vector<double> ratio(all_dims);
		for(j = 0; j< all_dims; j++) {
			ratio [j] = 1.0/(all_dim_max[j]-all_dim_min[j]);
		}
		for(i = 0; i < data_size; i++)
		{
			// Get the tuple including all lines in the line i
			inputOkcData->getData(all_data,i);
			int idx = 0;
			// put the visible dimensions into the datamatrix
			for(j = 0; j< all_dims; j++) {
				if (on[j]) {
					norm_matrix[i][idx] = (all_data[j]-all_dim_min[j])*ratio[j];
					idx++;
				}
			}
		}
	} else {
		// For other situation, we use the original data

		// // Initialize the non-normalized data matrix
		datamatrix = new double *[data_size];
		for(i = 0; i < data_size; i++)
			datamatrix[i] = new double [dims];
		for(i = 0; i < data_size; i++)
		{
			// Get the tuple including all lines in the line i
			inputOkcData->getData(all_data,i);
			int idx = 0;
			// put the visible dimensions into the datamatrix
			for(j = 0; j< all_dims; j++) {
				if (on[j]) {
					datamatrix[i][idx] = all_data[j];
					idx++;
				}
			}
		}
	}

	// parameters is defined in OrderDimension.h

	// Get the data and put it into the 2d array datamatrix
	//set_size = data_size;

	// initialize the order array with the order in map_in_to_out
	// orderarray is the order of those visible dimensions
	// origpos is the position in the original dataset of those visible dimensions
	// Example (dimensions 0, 1, 3, 5 are visible dimensions):
	//      on       order              origpos
	//   0   1     order[0] = 0      origpos[0] = 0
	//   1   1     order[1] = 1      origpos[1] = 1
	//   2   0     -                 -
	//   3   1     order[2] = 2      origpos[2] = 3
	//   4   0     -                 -
	//   5   1     order[3] = 3      origpos[3] = 5
	std::vector<int> orderarray(dims), origpos(dims);
	int posidx=0;
	for(i = 0; i < dims; i++) {
		orderarray[i] = i;
		// search the next visible dimension
		while ( !on[posidx] ) {
			posidx++;
		}
		origpos[i] = posidx;
		posidx++;
	}

	// Call the functions to do dimension reordering
	switch (method) {
	case OrderDimension::OM_SIMILARITY :
		similarity_reorder(data_size, dims, datamatrix, orderarray, *reorderPara);
		break;
	case OrderDimension::OM_CLUTTER :
		switch ( visType ) {
		case XmdvTool::VISTYPE_PA :
			clutterbased_reorder_parcord(data_size, dims, norm_matrix, orderarray, *reorderPara);
			break;
		case XmdvTool::VISTYPE_SC :
			clutterbased_reorder_scatterplot(data_size, dims, datamatrix, orderarray, *reorderPara);
			break;
		case XmdvTool::VISTYPE_GL :
			clutterbased_reorder_glyph(data_size, dims, norm_matrix, orderarray, *reorderPara);
			break;
		case XmdvTool::VISTYPE_DI :
			clutterbased_reorder_dimstck(data_size, dims, datamatrix, orderarray, *reorderPara);
			break;
		default :
			break;
		}
		break;
	}

	// Release the memory occupied by datamatrix
	if ( datamatrix!=0) {
		for(i = 0; i < data_size; i++) {
			delete datamatrix[i];
		}
		delete datamatrix;
	}

	// Release the memory occupied by norm_matrix
	if ( norm_matrix!=0) {
		for(i = 0; i < data_size; i++) {
			delete norm_matrix[i];
		}
		delete norm_matrix;
	}

	// Set the array map_in_to_out in terms of the orderarray
	// only for visible dimensions
	for(i = 0; i < dims; i++) {
		map_in_to_out[ origpos [ orderarray[i] ] ] = origpos[i];
	}
	// Reset the array map_in_to_out for those invisible dimensions
	for(i = 0; i < all_dims; i++) {
		if ( !on[i] ) map_in_to_out[i] = i;
	}
	map->setMapInToOut(map_in_to_out);
}

///////////////////////////////////////helper function , swap two elements in an array//////////////////////////////////////////
void OrderDimension::swap_element(std::vector<int> &array, int index_1, int index_2)
{
	int temp;
	temp = array[index_1];
	array[index_1] = array[index_2];
	array[index_2] = temp;
	return;
}


//////////////////////////////////////recursive function for the optimal ordering///////////////////////////////////////////////
void OrderDimension::recur_order(double ** matrix, int dim_num, std::vector<int> &temp_array, std::vector<int> &order_array, int start, int type, Reorder_Parameters param, int data_num)
{
	int count;

	double orig, temp;
	std::vector<Vec2> ordered_positions, temp_positions;	// 2-dimensional vectors representing bin positions
	double ordered_group_ratio, temp_group_ratio;

	switch (type)
	{
		case 0:		orig = sim_total(matrix, dim_num, order_array);
					temp = sim_total(matrix, dim_num, temp_array);
					break;
		case 1:		orig = Total_Outlier_Number(matrix, dim_num, order_array);
					temp = Total_Outlier_Number(matrix, dim_num, temp_array);
					break;
		case 2:		orig = Total_Separation(matrix, dim_num, order_array, param.threshold_1);
					temp = Total_Separation(matrix, dim_num, temp_array, param.threshold_1);
					break;
		case 3:		orig = clutter_measure_symmetry(matrix, data_num, dim_num, order_array, param.threshold_1, param.threshold_2);
					temp = clutter_measure_symmetry(matrix, data_num, dim_num, temp_array, param.threshold_1, param.threshold_2);
					break;
		case 4:		ordered_positions = Calc_Dimstck_Pos(param.dimstack_size, matrix, data_num, dim_num, order_array, param);
					temp_positions = Calc_Dimstck_Pos(param.dimstack_size, matrix, data_num, dim_num, temp_array, param);

					//orig and temp: number of isolated bins in the two occasions
					// ordered_group and temp_group: number of groups in the two occasions
					orig = isolated_ratio(ordered_positions, data_num, ordered_group_ratio, param.threshold_1, param.eight_connected, param, dim_num); // get the necessary counts
					temp = isolated_ratio(temp_positions, data_num, temp_group_ratio, param.threshold_1, param.eight_connected, param, dim_num);          // get the necessary counts
					break;
		default:    return;

	}

	if(start == dim_num)  //the exit condition for the recurrence
	{
		if(param.min_max == 0)
		{
			if ( orig > temp )
				for(count = 0; count < dim_num; count++ )
					order_array[count] = temp_array[count];

			else if ( orig == temp )  // two orders have the same number of isolated points ratio
			{
				if (type == 4)	// if it's dimensional stacking
					if ( ordered_group_ratio > temp_group_ratio)              // the original one has more groups
						for(count = 0; count < dim_num; count++ )  // then use the new(temp) one
							order_array[count] = temp_array[count];

			}
		}
		else    // maximum clutter
		{
			if ( orig < temp )
				for(count = 0; count < dim_num; count++ )
					order_array[count] = temp_array[count];
			else if ( orig == temp )  // two orders have the same number of isolated points
			{
				if (type == 4)	// if it's dimensional stacking
					if ( ordered_group_ratio < temp_group_ratio)              // the temp one has more groups
						for(count = 0; count < dim_num; count++ )  // then use the new(temp) one
							order_array[count] = temp_array[count];

			}
		}

	}

	else
	{
		for( count = start; count < dim_num; count++ )
		{
			swap_element(temp_array, count, start);
			recur_order(matrix, dim_num, temp_array, order_array, start+1, type, param, data_num);
			swap_element(temp_array, count, start);
		}

	}

	return;
}

///////////////////////////////////////////////function for optimal ordering///////////////////////////////////////
void OrderDimension::optimal_reorder(double ** matrix, int dim_num, std::vector<int> &order_array, int type, Reorder_Parameters param, int data_num)
{
	std::vector<int> temp_array(dim_num);

	for(int count = 0; count < dim_num; count++ )  // initialize the  order array
	{
		temp_array[count] = order_array[count];
	}

	recur_order(matrix, dim_num, temp_array, order_array, 0, type, param, data_num);
}


/////////////////////////////////////////////function for random ordering/////////////////////////////////////////
void OrderDimension::random_reorder(double ** matrix, int dim_num, std::vector<int> &order_array, int type, Reorder_Parameters param, int data_num)
{
	std::vector<int> temp_array(dim_num);
	double orig, temp;
	std::vector<Vec2> ordered_positions, temp_positions;	// 2-dimensional vectors representing bin positions
	double ordered_group_ratio, temp_group_ratio;
	int count;

	for(count = 0; count < dim_num; count++ )  // initialize the order array
	{
		temp_array[count] = count;
	}

	int dim_1, dim_2;

	srand( (unsigned)time( NULL ));

	for(int times = 0; times < dim_num*dim_num; times++)
	{
		dim_1 = rand()%dim_num;
		dim_2 = rand()%dim_num;
		swap_element(temp_array, dim_1, dim_2);

		switch (type)
		{
			case 0:		orig = sim_total(matrix, dim_num, order_array);
						temp = sim_total(matrix, dim_num, temp_array);
						break;
			case 1:		orig = Total_Outlier_Number(matrix, dim_num, order_array);
						temp = Total_Outlier_Number(matrix, dim_num, temp_array);
						break;
			case 2:		orig = Total_Separation(matrix, dim_num, order_array, param.threshold_1);
						temp = Total_Separation(matrix, dim_num, temp_array, param.threshold_1);
						break;
			case 3:		orig = clutter_measure_symmetry(matrix, data_num, dim_num, order_array, param.threshold_1, param.threshold_2);
						temp = clutter_measure_symmetry(matrix, data_num, dim_num, temp_array, param.threshold_1, param.threshold_2);
						break;
			case 4:		ordered_positions = Calc_Dimstck_Pos(param.dimstack_size, matrix, data_num, dim_num, order_array, param);
						temp_positions = Calc_Dimstck_Pos(param.dimstack_size, matrix, data_num, dim_num, temp_array, param);

						//orig and temp: number of isolated bins in the two occasions
						// ordered_group and temp_group: number of groups in the two occasions
						orig = isolated_ratio(ordered_positions, data_num, ordered_group_ratio, param.threshold_1, param.eight_connected, param, dim_num); // get the necessary counts
						temp = isolated_ratio(temp_positions, data_num, temp_group_ratio, param.threshold_1, param.eight_connected, param, dim_num);          // get the necessary counts
						break;
			default:	return;
		}

		if(param.min_max == 0)    // minimize relationship
		{

			if ( orig > temp )
				for(count = 0; count < dim_num; count++ )
					order_array[count] = temp_array[count];

			else if ( orig == temp )  // two orders have the same number of isolated points
			{
				if (type == 4)	// if it's dimensional stacking
					if ( ordered_group_ratio > temp_group_ratio)              // the original one has more groups
						for(count = 0; count < dim_num; count++ )  // then use the new(temp) one
							order_array[count] = temp_array[count];

			}
		}
		else				// maximize relationship
		{

			if ( orig < temp )
				for(count = 0; count < dim_num; count++ )
					order_array[count] = temp_array[count];
			else if ( orig == temp )  // two orders have the same number of isolated points
			{
				if (type == 4)	// if it's dimensional stacking
					if ( ordered_group_ratio < temp_group_ratio)              // the temp one has more groups
						for(count = 0; count < dim_num; count++ )  // then use the new(temp) one
							order_array[count] = temp_array[count];

			}
		}
	}
//	delete temp_array;
}

// helper function . find the dimension pair with the largest number of relationship between them
//for example: correlation, outlier number, etc
double OrderDimension::find_max_distance(double **matrix, int dim_num)
{
	int first, next;
	double max = -1;

	for(first = 0; first < dim_num; first++)
		for(next = first + 1; next < dim_num; next++)
		{
			if(matrix[first][next] > max)
				max = matrix[first][next];
		}

	return max;
}

void OrderDimension::nearest_neighbor(double ** matrix, int dim_num, std::vector<int> &order_array, int type, Reorder_Parameters param)
{
	std::vector<bool> included_dimension(dim_num);   // keep record of the dimensions already in the tour
	int count, current, interested;
	int min_dim = 0, max_dim = 0;
	double min = 0.0, max = 0.0;

	for(count = 0; count < dim_num; count++)  // initially no dimension is in the tour
		included_dimension[count] = false;

	order_array[0] = param.nn_start_dim;      // the first dimension (start point) in the tour
	included_dimension[param.nn_start_dim] = true;

	switch (type)
	{
	case 0:		max = find_max_distance(matrix,dim_num)+1;   //similarity (in all of the 3 methods, min can't be less than -1)
				min = -1;
				break;
	case 1:		max = find_max_distance(matrix,dim_num)+1;   //outlier, +1 is to  make sure min_dim will be assigned value
				min = -1;
				break;
	case 2:		max = 1;								  //pearson's correlation
				min = -1;

				break;
	default:	break;
	}

	if(param.min_max == 0)		// find minimum
	{
		for( current = 0; current < dim_num - 1; current++ ) // assign correct dimension for the current postion
		{
			min = max;

			for( interested = 0; interested < dim_num; interested++ )
			{
				if ( included_dimension[interested] == true || interested == order_array[current]) // if the dimension is already in the tour
					continue;

				if( matrix[order_array[current]][interested] < min )
				{
					min = matrix[order_array[current]][interested];
					min_dim = interested;
				}
				else if(current == dim_num - 2)   // The last two dimensions happen to be the max distance pair
				{
					min = matrix[order_array[current]][interested];
					min_dim = interested;
				}
			}

			order_array[current+1] = min_dim;
			included_dimension[min_dim] = true;

		}


	}
	else		// find maximum
	{
		for( current = 0; current < dim_num - 1; current++ ) // assign correct dimension for the current postion
		{
			max = min;

			for( interested = 0; interested < dim_num; interested++ )
			{
				if ( included_dimension[interested] == true || interested == order_array[current]) // if the dimension is already in the tour
					continue;

				if( matrix[order_array[current]][interested] > max )
				{
					max = matrix[order_array[current]][interested];
					max_dim = interested;
				}
				else if(current == dim_num - 2)   // The last two dimensions happen to be the max distance pair
				{
					max = matrix[order_array[current]][interested];
					max_dim = interested;
				}
			}

			order_array[current+1] = max_dim;
			included_dimension[max_dim] = true;
		}
	}
}


// helper function for greedy algorithm. find the dimension pair with the smallest
// similarity parameter and not yet in the tour
double OrderDimension::find_smallest_pair(double ** matrix, int dim_num, double max, int &min_first_dim, int &min_next_dim, bool **flag)
{
	//Modified by Zaixian: add codes about found to deal with the situation that
	//no values less than max can be found.  If there is not such values, a real
	//number greater than max is returned
	int first, next;
	double min = max;
	bool found = false;

	//Added by Zaixian: the followling two lines
	//min_first_dim = 0;
	//min_next_dim = 1;

	for( first = 0; first < dim_num; first++ )
		for(next = first + 1; next < dim_num; next++)
		{
			if(matrix[first][next] < min)
				if(!flag[first][next])
				{
					min = matrix[first][next];
					min_first_dim = first;
					min_next_dim = next;
					found = true;
				}

		}

	if (found) {
		flag[min_first_dim][min_next_dim] = flag[min_next_dim][min_first_dim] = true;
		return min;
	} else {
		if (fabs(min)>1e-3)
			return min+fabs(min);
		else
			return min+1.0;
	}
}

// helper function for greedy algorithm. find the dimension pair with the largest
// relationship and not yet in the tour
double OrderDimension::find_largest_pair(double ** matrix, int dim_num, double min, int &max_first_dim, int &max_next_dim, bool **flag)
{
	//Modified by Zaixian: add codes about found to deal with the situation that
	//no values greater than min can be found.  If there is not such values, a real
	//number less than max is returned
	int first, next;
	double max = min;
	bool found=false;

	for( first = 0; first < dim_num; first++ )
		for(next = first + 1; next < dim_num; next++)
		{
			if(matrix[first][next] > max)
				if(!flag[first][next])
				{
					max = matrix[first][next];
					max_first_dim = first;
					max_next_dim = next;
					found = true;
				}

		}

	if (found) {
		flag[max_first_dim][max_next_dim] = flag[max_next_dim][max_first_dim] = true;
		return max;
	} else {
		//return a real number less than max
		if (fabs(max)>1e-3)
			return max-fabs(max);
		else
			return max-1.0;
	}
}

bool OrderDimension::in_same_group(int **neighbor_dim, int dim_num, int dim_1, int dim_2)
{
	int first = dim_1, next, count, direction;

	for(direction = 0; direction < 2; direction++)
	{
		for(count = 0,next = neighbor_dim[dim_1][direction]; count < dim_num; count++)
		{
			if(next == dim_2)
				return true;
			if(next == dim_num)
				break;
			if(first == neighbor_dim[next][1])
			{
				first = next;
				next = neighbor_dim[next][0];
			}
			else
			{
				first = next;
				next = neighbor_dim[next][1];
			}
		}
	}
	return false;
}

void OrderDimension::greedy_algorithm(double ** matrix, int dim_num, std::vector<int> &order_array, int type, Reorder_Parameters param)
{
	int groupnum = dim_num;
	int start = dim_num, end = dim_num;                // The starting and ending dimension
	int count,row,column;
	double max_distance, min_distance;
	int min_first,min_next, max_first, max_next;		// The two dimensions with minimum or maximum distance
	int first, next;

	int **neighbor_dim = new int *[dim_num];  // The two neighbors of a dimension
	for(count = 0; count < dim_num; count++)
		neighbor_dim[count] = new int[2];

	bool **already_visited = new bool *[dim_num];
	for(count = 0; count < dim_num; count++)
		already_visited[count] = new bool[dim_num];

	for(count = 0; count < dim_num; count++)
	{
		neighbor_dim[count][0] = neighbor_dim[count][1] = dim_num; //initialize the neighbor list
	}

	for(row = 0; row < dim_num; row++)
		for(column = 0; column < dim_num; column++)
			already_visited[row][column] = false;

	max_distance = find_max_distance(matrix,dim_num);
	min_distance = -1;

	if(param.min_max == 0)	// find minimum distance order
	{

		for(;;)
		{
			switch (type)
			{
			case 0:
			case 1:
			case 2:		min_distance = find_smallest_pair(matrix,dim_num,max_distance,min_first,min_next,already_visited);
						//added by Zaixian
						if ( min_distance>max_distance ) {
							QMessageBox::warning(0,
									"Inappropriate Parameters",
									"reedy algorithm can not be executed successfully because of inappropriate threshold!");

							return;
						}
						break;
			default:	break;
			}

			if(neighbor_dim[min_first][0] != dim_num && neighbor_dim[min_first][1] != dim_num)
				continue; //first dimension is already connected to two other dimensions, reject
			if(neighbor_dim[min_next][0] != dim_num && neighbor_dim[min_next][1] != dim_num)
				continue; //second dimension is alredy connected to two other dimensions, reject
			if(neighbor_dim[min_first][0] != dim_num)	// first dimension is a starting dimension of a group
			{
				if(neighbor_dim[min_next][0] != dim_num)  // next dimension is a starting dimension of a group
				{
					if(in_same_group(neighbor_dim,dim_num,min_first,min_next))
						continue;
					else
					{
						neighbor_dim[min_first][1] = min_next;
						neighbor_dim[min_next][1] = min_first;
						groupnum--;
					}
				}
				else	// next dimension is a group with a single dimension
				{
					neighbor_dim[min_first][1] = min_next;
					neighbor_dim[min_next][0] = min_first;
					groupnum--;
				}
			}
			else	// first dimension is a group with a single dimension
			{
				if(neighbor_dim[min_next][0] != dim_num)	// next dimension is a starting dimension of a group
				{
					neighbor_dim[min_first][0] = min_next;
					neighbor_dim[min_next][1] = min_first;
					groupnum--;
				}
				else	// next dimension is a group with a single dimension
				{
					neighbor_dim[min_first][0] = min_next;
					neighbor_dim[min_next][0] = min_first;
					groupnum--;
				}
			}
			if (groupnum == 1)
				break;
		}
	}
	else	// find maximum distance order
	{
		for(;;)
		{
			switch (type)
			{
			case 0:
			case 1:
			case 2:		max_distance = find_largest_pair(matrix,dim_num,min_distance,max_first,max_next,already_visited);
						//added by Zaixian
						if ( max_distance<min_distance ) {
							QMessageBox::warning(0,
									"Inappropriate Parameters",
									"reedy algorithm can not be executed successfully because of inappropriate threshold!");
							return;
						}
						break;
			default:	break;
			}

			if(neighbor_dim[max_first][0] != dim_num && neighbor_dim[max_first][1] != dim_num)
				continue; //first dimension is already connected to two other dimensions, reject
			if(neighbor_dim[max_next][0] != dim_num && neighbor_dim[max_next][1] != dim_num)
				continue; //second dimension is alredy connected to two other dimensions, reject
			if(neighbor_dim[max_first][0] != dim_num)	// first dimension is a starting dimension of a group
			{
				if(neighbor_dim[max_next][0] != dim_num)  // next dimension is a starting dimension of a group
				{
					if(in_same_group(neighbor_dim,dim_num,max_first,max_next))
						continue;
					else
					{
						neighbor_dim[max_first][1] = max_next;
						neighbor_dim[max_next][1] = max_first;
						groupnum--;
					}
				}
				else	// next dimension is a group with a single dimension
				{
					neighbor_dim[max_first][1] = max_next;
					neighbor_dim[max_next][0] = max_first;
					groupnum--;
				}
			}
			else	// first dimension is a group with a single dimension
			{
				if(neighbor_dim[max_next][0] != dim_num)	// next dimension is a starting dimension of a group
				{
					neighbor_dim[max_first][0] = max_next;
					neighbor_dim[max_next][1] = max_first;
					groupnum--;
				}
				else	// next dimension is a group with a single dimension
				{
					neighbor_dim[max_first][0] = max_next;
					neighbor_dim[max_next][0] = max_first;
					groupnum--;
				}
			}
			if (groupnum == 1)
				break;
		}
	}

	for(count = 0; count < dim_num; count++)
	{
		if(neighbor_dim[count][1] == dim_num) {	// Only one dimension is connected to this dimension
			if(start == dim_num) {	// start has not been assigned a dimension
				start = count;
			} else {					// start has been assigned a dimension
				end = count;
			}
		}
	}

	first = next = start;	// in order to put the dimensions at right place in the order_array
	count = 0;

	if(neighbor_dim[first][1] == dim_num)	// currently marking the start or end dimension
	{
		next = neighbor_dim[first][0];		// next we should mark the only one connected to current dimension
		order_array[count++] = first;
	}

	do
	{
			if(neighbor_dim[next][0] == first)
			{
				first = next;		// move forward
				next = neighbor_dim[next][1];
			}
			else
			{
				first = next;
				next = neighbor_dim[next][0];
			}
			order_array[count++] = first;

	}while ( (next<dim_num) && (neighbor_dim[next][1] != dim_num) );
	if (count<dim_num) order_array[count] = next;

	for( count = 0; count < dim_num; count++ )
		delete already_visited[count];
	delete already_visited;

	for( count = 0; count < dim_num; count++ )
		delete neighbor_dim[count];
	delete neighbor_dim;

}

//this is a recursive function called by isolated_ratio, neighbor_def: 0: 4-connected, 1: 8_connected
int OrderDimension::assign_group(int i, int j, int X_bin, int Y_bin, bool **filled_bins, bool **flag, int neighbor_def)
{
	//used to count number of group members
	int count = 1;

	flag[i][j] = true;

	if (j != 0) //west
	{
		//from the west
		if (filled_bins[i][j-1] && !flag[i][j-1])
		{
			count += assign_group(i,j-1, X_bin, Y_bin, filled_bins, flag, neighbor_def);
		}

		//from north west
		if (i != 0)
		{
			if (filled_bins[i-1][j-1] && !flag[i-1][j-1] && neighbor_def)
				count += assign_group(i-1,j-1, X_bin, Y_bin, filled_bins, flag, neighbor_def);
		}

		//from the south west
		if (i != Y_bin-1)
		{
			if (filled_bins[i+1][j-1] && !flag[i+1][j-1] && neighbor_def)
				count += assign_group(i+1,j-1, X_bin, Y_bin, filled_bins, flag, neighbor_def);
		}
	}

	if (j != X_bin-1) //east
	{
		//from the east
		if (filled_bins[i][j+1] && !flag[i][j+1])
			count += assign_group(i,j+1, X_bin, Y_bin, filled_bins, flag, neighbor_def);

		//from north east
		if (i != 0)
		{
			if (filled_bins[i-1][j+1] && !flag[i-1][j+1] && neighbor_def)
				count += assign_group(i-1,j+1, X_bin, Y_bin, filled_bins, flag, neighbor_def);
		}

		//from south east
		if (i != Y_bin-1)
		{
			if (filled_bins[i+1][j+1] && !flag[i+1][j+1] && neighbor_def)
				count += assign_group(i+1,j+1, X_bin, Y_bin, filled_bins, flag, neighbor_def);
		}
	}

	//from the north
	if (i != 0)
	{
		if (filled_bins[i-1][j] && !flag[i-1][j])
			count += assign_group(i-1,j, X_bin, Y_bin, filled_bins, flag, neighbor_def);
	}

	//from the south
	if (i != Y_bin-1)
	{
		if (filled_bins[i+1][j] && !flag[i+1][j])
			count += assign_group(i+1,j, X_bin, Y_bin, filled_bins, flag, neighbor_def);
	}

	return count;
}

double OrderDimension::isolated_ratio(std::vector<Vec2> DimStckData, int set_size, double &group_ratio, double threshold, int neighbor_def, Reorder_Parameters param, int dim_num)
{
	int i, j;

	int isolated = 0;
	int aggregated = 0;
	int isolated_group = 0;
	int aggregated_group = 0;

	double ratio = 0;

	double A_X, A_Y;
/*	std::vector<bool> flag(set_size);
	for ( i = 0; i < set_size; i++ )
		flag[i] = false;
	// get the data points' positions
	for( i = 0; i < set_size - 1 ; i++ )
	{
		for ( j = i + 1; j < set_size; j++ )
		{
			A_X = DimStckData[i].X;
			A_Y = DimStckData[i].Y;
			B_X = DimStckData[j].X;
			B_Y = DimStckData[j].Y;
			temp1 = fabs(A_X - B_X);
			temp2 = fabs(A_Y - B_Y);

			// temp1 and temp2 are the x and y difference of the two bin, they should not be both 0
			if (temp1 <= 1.1*g_globals.screen_distortion->dimstack_X_blocksize )
				if ( temp2 <= 1.1*g_globals.screen_distortion->dimstack_Y_blocksize )
					  if ((temp1 + temp2) > 0) // meaning they are not mapped to the same bin

			{
				if (flag[i] == false && flag[j] == false)
					group ++;
				flag[i] = true;
				flag[j] = true;
			}
		}
	}
	for ( i = 0 ; i < set_size; i++ )
	{
		if (flag[i] == false)
			isolated ++;
	}
*/
	// number of bins on X and Y direction
	int X_bin, Y_bin;
	X_bin = Y_bin = 1;

	for (i = 0; i < dim_num; i+=2)
		X_bin *= param.cardinality[i];
	for (j = 1; j < dim_num; j+=2)
		Y_bin *= param.cardinality[j];

	bool **filled_bins = new bool *[X_bin];
		for(i = 0; i < X_bin; i++)
			filled_bins[i] = new bool [Y_bin];

	for ( i = 0; i < X_bin; i++ )
		for ( j = 0; j < Y_bin; j++ )
			filled_bins[i][j] = false;

	int direction_X_Bins, direction_Y_Bins;
	for( i = 0; i < set_size ; i++ )
	{
		A_X = DimStckData[i].X;
		A_Y = DimStckData[i].Y;

		// position of the filled bin, add 0.5 to make sure the correct typecasting
		direction_X_Bins = (int) (A_X/param.dimstack_X_blocksize + 0.5);
		direction_Y_Bins = (int) (A_Y/param.dimstack_Y_blocksize + 0.5);

		filled_bins[direction_X_Bins][direction_Y_Bins] = true;
	}

	bool **flag = new bool *[X_bin];
		for(i = 0; i < X_bin; i++)
			flag[i] = new bool [Y_bin];

	for ( i = 0; i < X_bin; i++ )
		for ( j = 0; j < Y_bin; j++ )
			flag[i][j] = false;

	//list used to keep track of number of members in each group
	list<int> group_members;

	for ( i = 0; i < X_bin; i ++ )
		for ( j = 0; j < Y_bin; j++ )
		{
			if (filled_bins[i][j] && !flag[i][j])
			{
				flag[i][j] = true;

				//recursion
				int members = assign_group(i, j, X_bin, Y_bin, filled_bins, flag, neighbor_def);

				//add the number of group members
				group_members.push_back(members);
			}
		}


	//create constant iterator for list.
	list<int>::const_iterator iter;

	//compare number of group members against used defined threshold
	for (iter=group_members.begin(); iter != group_members.end(); iter++)
	{

		if (*iter >= threshold/*user defined threshold*/)
		{
			aggregated += *iter;
			aggregated_group ++ ;
		}
		else
		{
			isolated += *iter;
			isolated_group ++ ;
		}
	}

	ratio = (double)isolated / (isolated + aggregated);
	group_ratio = (double)isolated_group/(isolated_group + aggregated_group);

	return ratio;
}



int OrderDimension::get_contribution(int ind, double data, Reorder_Parameters param)
{
    int res = int( ((data - param.dim_min[ind]) / (param.dim_max[ind] - param.dim_min[ind])) *
              (double)(param.cardinality[ind]));

    return res;
}

std::vector<Vec2> OrderDimension::Calc_Dimstck_Pos(const std::vector<double> &size, double ** data_matrix, int set_size, int dim_num, std::vector<int> order_array, Reorder_Parameters param)
{
	std::vector<Vec2> dimstck_point(set_size);
	for ( int count = 0; count < set_size; count++ )
		dimstck_point[count].X = dimstck_point[count].Y = 0;

	std::vector<int> contrib(dim_num);
    int i,j;
	for ( i = 0; i < set_size; i++ )
	{
		for (j = 0; j < dim_num ; j++)
			contrib[j] = get_contribution(order_array[j], data_matrix[i][order_array[j]], param);

		for (j=0; j<dim_num; j++)
		{
			dimstck_point[i].X += contrib[j] * size[j];
			j++;
			if (j < dim_num)
				dimstck_point[i].Y += contrib[j] * size[j];
		}

	}
	return dimstck_point;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////helper function for calculating the distances between dimensions//////////////////////////////////

double OrderDimension::calc_scale( double num, double **data_matrix, int data_num, int dimension)
{
	double scale;
	double max = num, min = num;

	////////////////////////////////////////get the max and min value of one dimension///////////////////////////////////////////
	for(int count = 0; count < data_num; count++ )
	{
		if(data_matrix[count][dimension] > max)
			max = data_matrix[count][dimension];
		if(data_matrix[count][dimension] < min)
			min = data_matrix[count][dimension];
	}

	if(max == min)
		scale = 1;
	else
		scale = (num - min)/(max - min);

	return scale;

}


////////////////////////////////////////////get the similarity between two dimensions////////////////////////////////////////////
double OrderDimension::simi_func(double **data_matrix, int data_num, int dim_1, int dim_2, int method)
{
	double scale_1, scale_2; // for euclidean distance
	double ave_1, ave_2, cov, std_1, std_2, total_1, total_2; // for pearson's correlation
	double cross, square_1, square_2;				// for cosine angle correlation
	double total=0;
	int count = 0;

	//////////////////////calculate distance between dimension 1 and dimension 2//////////////////////////////////////
	switch (method)
	{
	case 1:			// Euclidean Distance
		for ( count = 0; count < data_num; count++ )
		{
			scale_1 = calc_scale(data_matrix[count][dim_1],data_matrix,data_num,dim_1);
			scale_2 = calc_scale(data_matrix[count][dim_2],data_matrix,data_num,dim_2);

			total += pow(scale_1-scale_2,2);
		}

		total = sqrt(total);
		break;
	case 2:			// Pearson's Correlation
		// First calculate the average of the two dimensions
		for ( total_1 = 0, total_2 = 0, count = 0; count < data_num; count++ )
		{
			total_1 += calc_scale(data_matrix[count][dim_1],data_matrix,data_num,dim_1);
			total_2 += calc_scale(data_matrix[count][dim_2],data_matrix,data_num,dim_2);
		}
		ave_1 = total_1/data_num;
		ave_2 = total_2/data_num;
		// calculate pearson's correlation
		for ( cov = std_1 = std_2 = 0, count = 0; count < data_num; count++ )
		{
			cov += ( calc_scale(data_matrix[count][dim_1],data_matrix,data_num,dim_1) - ave_1 ) * ( calc_scale(data_matrix[count][dim_2],data_matrix,data_num,dim_2) - ave_2);
			std_1 += pow(calc_scale(data_matrix[count][dim_1],data_matrix,data_num,dim_1) - ave_1,2);
			std_2 += pow(calc_scale(data_matrix[count][dim_2],data_matrix,data_num,dim_2) - ave_2,2);
		}
		std_1 = sqrt(std_1);
		std_2 = sqrt(std_2);
		total = cov/(std_1*std_2);
		break;
	case 3:			// cosine angle correlation
		for( cross = square_1 = square_2 = 0, count = 0; count < data_num; count++ )
		{
			cross += calc_scale(data_matrix[count][dim_1],data_matrix,data_num,dim_1)*calc_scale(data_matrix[count][dim_2],data_matrix,data_num,dim_2);
			square_1 += pow(calc_scale(data_matrix[count][dim_1],data_matrix,data_num,dim_1),2);
			square_2 += pow(calc_scale(data_matrix[count][dim_2],data_matrix,data_num,dim_2),2);
		}
		total = cross/sqrt(square_1*square_2);
		break;
	}
	return total;
}

///////////////////////////////////////calculate the sum of distances between neighboring dimensions//////////////////////////////
/////////array gives the sequence of dimensions

double OrderDimension::sim_total(double **similarity_matrix, int dim_num, std::vector<int> array)
{
	double total = 0;

	for(int count = 0; count < dim_num-1; count++ )
	{
		total += similarity_matrix[array[count]][array[count+1]];
	}

	return total;
}



////////////////////////////////////////////////reorder using similarity method////////////////////////////////////

bool OrderDimension::similarity_reorder(int n, int m, double **data_matrix, std::vector<int> &order_array, Reorder_Parameters param)
//n: data points, m: dimension
{
	int row, column;
	int count;
//	double sum_of_similarity;    // for spacing purpose

	double **simi_matrix = new double *[m]; // Similarity matrix of the n data points
	for(count = 0; count < m; count++ )
		simi_matrix[count] = new double[m];


	/////////////////get the similarity matrix of the data set///////////////////////////
	for(row = 0 ; row < m; row++)
		for(column = 0; column <= row; column++)
		{
			simi_matrix[row][column] = simi_matrix[column][row]
									 = simi_func(data_matrix, n, row, column, param.correlation);
			// "correlation" here determines the dimension correlation measure used.
			// correlation = 1,  euclidean distance
			// for other kind of correlation measures, larger value represents greater similarity
		}

	//////////////////////reorder the dimensions//////////////////////////////////////////

	switch (param.algorithm)
	{
		case 0:	if(param.correlation == 1)		// euclidean distance, the smaller the distance, the greater the similarity
					optimal_reorder(simi_matrix, m, order_array, 0, param);
				else						// other correlation mesures, the greater the value, the greater the similarity
				{	param.min_max = !(param.min_max);
					optimal_reorder(simi_matrix, m, order_array, 0, param);
				}
						// dissimilar dimensions stand toghther
				break;
		case 1: if(param.correlation == 1)		// euclidean distance
					random_reorder(simi_matrix, m, order_array, 0, param);
				else
				{
					param.min_max = !(param.min_max);
					random_reorder(simi_matrix, m, order_array, 0, param);
				}
				break;
		// do the nearest neighbor algorithm using chosen_dim as the first dimension in the arrangement
		case 2: if(param.correlation == 1)		// euclidean distance
					nearest_neighbor(simi_matrix, m, order_array, 0, param);
				else
				{
					param.min_max = !(param.min_max);
					nearest_neighbor(simi_matrix, m, order_array, 0, param);
				}
				break;
		case 3: if(param.correlation == 1)		// euclidean distance
					greedy_algorithm(simi_matrix, m, order_array, 0, param);
				else
				{
					param.min_max = !(param.min_max);
					greedy_algorithm(simi_matrix, m, order_array, 0, param);
				}
				break;
		return false;
	}

	for( count = 0; count < m; count++ )
		delete simi_matrix[count];
	delete simi_matrix;
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////// Functions for the clutter-based reordering //////////////////////

double OrderDimension::clutter_measure_symmetry(double **norm_matrix, int data_num, int dim_num, std::vector<int> array, double mono_threshold, double sym_threshold)
{

	int i,j;
	double clutter = 0;
	double mono_dif, sym_dif;

	for(i = 0; i < data_num; i++)
	{
		for(j = 1; j < dim_num; j++)
		{
			if ( j <= dim_num/2 )
			{
				if(norm_matrix[i][array[j]] >= norm_matrix[i][array[j-1]])
					continue;
				else
				{
					mono_dif = norm_matrix[i][array[j-1]] - norm_matrix[i][array[j]];
					if( mono_dif > mono_threshold )
						clutter ++;
				}
			}
			else
			{
				if(norm_matrix[i][array[j-1]] >= norm_matrix[i][array[j]])
					continue;
				else
				{
					mono_dif = norm_matrix[i][array[j]] - norm_matrix[i][array[j-1]];
					if ( mono_dif > mono_threshold)
						clutter ++;
				}
			}
		}
		// above calculates the non-monotonicity ( one sort of clutter )

		for( j = 1; j <= dim_num/2; j++ )
		{
			sym_dif = fabs(norm_matrix[i][array[j]] - norm_matrix[i][array[dim_num-j]]);
			if ( sym_dif > sym_threshold )
				clutter++;
		}
		// above calculates the dissymmetry


	}

	return clutter;
}




void OrderDimension::clutterbased_reorder_glyph(int data_num, int dim_num, double **norm_matrix, std::vector<int> &order_array, Reorder_Parameters clutter_param)
//n: data points, m: dimension
{
	switch (clutter_param.algorithm)
	{
		case 0:		optimal_reorder(norm_matrix, dim_num, order_array, 3, clutter_param, data_num);
					break;
		case 1:		random_reorder(norm_matrix, dim_num, order_array, 3, clutter_param, data_num);
					break;
		default:	break;
	}
}

// Added by debbie 12/2003
// Used for dimensional stacking purpose


void OrderDimension::clutterbased_reorder_dimstck(int set_size, int dim_num, double ** data_matrix, std::vector<int> &order_array, Reorder_Parameters clutter_param)
{
	switch (clutter_param.algorithm)
	{
		case 0:		optimal_reorder(data_matrix, dim_num, order_array, 4/*type*/, clutter_param, set_size);
					break;
		case 1:		random_reorder(data_matrix, dim_num, order_array, 4, clutter_param, set_size);
					break;
		default:	break;
	}

}



// Added by debbie 3/2004
// Used for parallel coordinate clutter reduction



int OrderDimension::Outlier_Between(double **norm_matrix, int data_num, int dim_1, int dim_2, double threshold)
{

	double distance;
	int i, j, count;   // used for counting data points
	int outlier = 0;
	std::vector<bool> flag(data_num);   // to indicate if a point is an outlier or not
	for ( count = 0; count < data_num; count++ )
		flag[count] = false;

	double maxdistance = 0;
	double mindistance = 1;

	for ( i = 0; i < data_num; i ++ )
	{
		for ( j = 0; j < data_num ; j ++ )
		{
			if (j == i)
				continue;

			distance = sqrt(pow(norm_matrix[i][dim_1] - norm_matrix[j][dim_1], 2) +
				pow(norm_matrix[i][dim_2] - norm_matrix[j][dim_2], 2) );

			if ( distance > maxdistance )
				maxdistance = distance;
			if (distance < mindistance )
				mindistance = distance;

			if ( distance < threshold )
			{
				flag[i] = flag[j] = true;
				break;
			}
		}
	}

	for ( count = 0; count < data_num; count ++ )
		if (flag[count] == false)
		{
			// PAUL: comment out these (?) unused arrays
			//g_globals.dataset->outlier_flag[count] = false;
			//g_globals.dataset->global_flag[count][dim_1][dim_2] = g_globals.dataset->global_flag[count][dim_2][dim_1]= false;
			outlier ++;
		}
	return outlier;

}

double OrderDimension::Total_Outlier_Number(double **outlier_matrix, int dim_num, std::vector<int> array)
{
	double total = 0;

	for ( int count = 0; count < dim_num - 1; count ++ )
	{

		total += outlier_matrix[array[count]][array[count+1]];
	}
	return total;


}


void OrderDimension::clutterbased_reorder_parcord(int set_size, int dim_num, double **norm_matrix, std::vector<int> &order_array, Reorder_Parameters clutter_param)
{
	// PAUL: hog_leak_unused
//	g_globals.dataset->draw_outlier = true;

	int count, row, column;
	double **outlier_matrix = new double *[dim_num]; // Similarity matrix of the n data points
	for(count = 0; count < dim_num; count++ )
		outlier_matrix[count] = new double[dim_num];


	for(row = 1 ; row < dim_num; row++)
		for(column = 0; column < row; column++)
		{
			outlier_matrix[row][column] = outlier_matrix[column][row]
									 = Outlier_Between(norm_matrix, set_size, row, column, clutter_param.threshold_1);

		}

	for (row = 0; row < dim_num; row++)
		outlier_matrix[row][row] = 0;

	switch (clutter_param.algorithm)
	{
		case 1:
			random_reorder(outlier_matrix, dim_num, order_array, 1, clutter_param);
			break;
		case 2:
			nearest_neighbor(outlier_matrix, dim_num, order_array, 1, clutter_param);
			break;
		case 3:
			greedy_algorithm(outlier_matrix, dim_num, order_array, 1, clutter_param);
			break;
		default:
			optimal_reorder(outlier_matrix, dim_num, order_array, 1, clutter_param);
	}
}

double OrderDimension::Total_Separation(double **sim_matrix, int used_dim, std::vector<int> order, double threshold)
{
	double total = 0;
	int row, column, i, j;
	double sim_1, sim_2;

	for ( row = 0; row < used_dim; row ++ )
	{
		for ( column =0; column < row; column ++ )
		{
			sim_1 = sim_matrix[order[row]][order[column]];
			for ( i = row; i < used_dim; i++ )
			{
				for ( j = 0; j < i; j ++ )
				{
					if ( j <= column && i <= row )
						continue;
					sim_2 = sim_matrix[order[i]][order[j]];
					if ( fabs(sim_1 - sim_2) < threshold )
						total += sqrt( (double)(row-i)*(row-i) + (double)(column-j)*(column-j));
				}
			}
		}
	}

	return total;

}


void OrderDimension::clutter_based_reorder_scatter_high_card(double **data_matrix, int set_size, int dim_num, int used_dim, std::vector<int> &order_array, std::vector<bool> flag, Reorder_Parameters clutter_param)
{
	int row, column, count;
	double pearson;

	double **sim_matrix = new double *[dim_num]; // Similarity matrix of the n data points
	for(count = 0; count < dim_num; count++ )
		sim_matrix[count] = new double[dim_num];


	/////////////////get the similarity matrix of the data set///////////////////////////
	for(row = 0 ; row < dim_num; row++)
		for(column = 0; column <= row; column++)
		{
			pearson = simi_func(data_matrix, set_size, row, column, 2);   // pearson correlation
			sim_matrix[row][column] = sim_matrix[column][row] = pearson;

		}


	int temp;
	for(temp = 0,count = 0; count < dim_num; count++ )  // initialize the  order array
	{
		if( flag[count] )
		{
			order_array[temp] = count;
			temp++;
		}

	}

	switch (clutter_param.algorithm)
	{

		case 1:		random_reorder(sim_matrix, used_dim, order_array, 2, clutter_param);
					break;
        case 2:		clutter_param.min_max = !(clutter_param.min_max);
					nearest_neighbor(sim_matrix, used_dim, order_array, 2, clutter_param);
					break;
		case 3:		clutter_param.min_max = !(clutter_param.min_max);
					greedy_algorithm(sim_matrix, used_dim, order_array, 2, clutter_param);
					break;
		default:	optimal_reorder(sim_matrix, used_dim, order_array, 2, clutter_param);
					break;

	}

	for( count = 0; count < dim_num; count++ )
		delete sim_matrix[count];
	delete sim_matrix;


}
void OrderDimension::clutter_based_reorder_scatter_low_card(double **data_matrix, int set_size, int dim_num, int used_dim, std::vector<int> &order_array, std::vector<bool> flag)
{
	int dim, data, count;
	int max, max_dim=0;
	std::vector<double> array(set_size);
	std::vector<int> cardinality(dim_num);
	std::vector<bool> included(dim_num);

	for ( count = 0; count < dim_num; count ++ )
		included[count] = false;

	for ( dim = 0; dim < dim_num; dim ++ )
	{
		if ( flag[dim] == true )									// high cardinality dimension, ignore
			continue;

		for ( data = 0; data < set_size; data ++ )
			array[data] = data_matrix[data][dim];			// get all the data on one dimension
		//std::sort(array.addr(), array.addr() + set_size);
		std::sort(array.begin(),array.end());

		for ( data = 0, count = 1; data < set_size - 1; data ++ )
		{
			if (array[data] == array[data + 1])
				continue;
			count ++;
		}

		cardinality[dim] = count;
	}

	for ( int i = 0; i < used_dim; i ++ )
	{
		max = 0;

		for ( dim = 0; dim < dim_num; dim ++ )
		{
			if ( flag[dim] == true || included[dim] == true)
				continue;

			if ( cardinality[dim] > max )
			{
				max = cardinality[dim];
				max_dim = dim;
			}
		}

		order_array[i] = max_dim;
		included[max_dim] = true;

	}

}


void OrderDimension::separate_dimensions(double **data_matrix, int set_size, int dim_num, std::vector<bool> &flag, Reorder_Parameters clutter_param)
{
	int dim, data;
	int count;

	int plot_width, plot_height;
	int width, height;
	//(clutter_param.canvas)->GetSize(&width, &height);
	width = clutter_param.canvasWidth;
	height = clutter_param.canvasHeight;
	plot_width = width/dim_num;
	plot_height = height/dim_num;

	double sparse_level = clutter_param.threshold_2;
	std::vector<double> array(set_size);			// for deciding if a dimension is high cardinality or low cardinality
	double temp;

	for ( dim = 0; dim < dim_num; dim ++ )
	{
		for ( data = 0; data < set_size; data ++ )
			temp = array[data] = data_matrix[data][dim];			// get all the data on one dimension
		std::sort(array.begin(), array.end() );

		for ( data = 0, count = 1; data < set_size - 1; data ++ )
		{
			if (array[data] == array[data + 1])
				continue;
			count ++;
		}

		if ( count < sparse_level*plot_width && count < sparse_level*plot_height )		// sparse on both x and y direction
			flag[dim] = false;
		else
			flag[dim] = true;

	}

}


void OrderDimension::clutter_based_reorder_scatter_distinct(double **data_matrix, int set_size, int dim_num, std::vector<int> &order_array, Reorder_Parameters clutter_param)
{

	std::vector<bool> flag(dim_num);					// for deciding which dimensions are high cardinality and which are low

	int count = 0;
	int i;
	bool temp;
	separate_dimensions(data_matrix, set_size, dim_num, flag, clutter_param);

	for ( i = 0; i < dim_num; i ++ )
	{
		temp = flag[i];
		if ( flag[i] == true )
			count ++ ;
	}

	int high_card_num = count;
	int low_card_num = dim_num - count;

	std::vector<int> high_card_order(count);
	std::vector<int> low_card_order(dim_num - count);

    clutter_based_reorder_scatter_high_card(data_matrix, set_size, dim_num, high_card_num, high_card_order, flag, clutter_param);
	clutter_based_reorder_scatter_low_card(data_matrix, set_size, dim_num, low_card_num, low_card_order, flag);

	for ( count = 0; count < high_card_num; count ++ )
		order_array[count] = high_card_order[count];
	for ( i = 0; i < low_card_num; i++, count++ )
		order_array[count] = low_card_order[i];


}

void OrderDimension::clutterbased_reorder_scatterplot(int set_size, int dim_num, double **data_matrix, std::vector<int> &order_array, Reorder_Parameters clutter_param)
{

	clutter_based_reorder_scatter_distinct(data_matrix, set_size, dim_num, order_array, clutter_param);


}



///////////////////////////////////For InterRing Use Only////////////////////////////////////////////////////////////////
bool OrderDimension::similarity_reorder_interring(int n, int m, double **data_matrix, std::vector<int> &order_array, int choice, int correlation, char * sim_or_dis, int)
//n: data points, m: dimension
{
	int row, column;
	int count;
//	double sum_of_similarity;    // for spacing purpose

	double **simi_matrix = new double *[m]; // Similarity matrix of the n data points
	for(count = 0; count < m; count++ )
		simi_matrix[count] = new double[m];


	/////////////////get the similarity matrix of the data set///////////////////////////
	for(row = 0 ; row < m; row++)
		for(column = 0; column <= row; column++)
		{
			simi_matrix[row][column] = simi_matrix[column][row]
									 = simi_func(data_matrix, n, row, column, correlation);
			// "correlation" here determines the dimension correlation measure used.
			// correlation = 1,  euclidean distance
			// for other kind of correlation measures, larger value represents greater similarity
		}

	Reorder_Parameters temp_param;

	//////////////////////reorder the dimensions//////////////////////////////////////////

	switch (choice)
	{
		case 0:	if(correlation == 1)		// euclidean distance, the smaller the distance, the greater the similarity
				{
					if(!strcmp(sim_or_dis,"sim"))
						temp_param.min_max = 0;
					else
						temp_param.min_max = 1;
				}
				else
				{
					if(!strcmp(sim_or_dis,"sim"))
						temp_param.min_max = 1;
					else
						temp_param.min_max = 0;
				}
				optimal_reorder(simi_matrix, m, order_array, 0, temp_param);
				break;
		case 1: if(correlation == 1)		// euclidean distance, the smaller the distance, the greater the similarity
				{
					if(!strcmp(sim_or_dis,"sim"))
						temp_param.min_max = 0;
					else
						temp_param.min_max = 1;
				}
				else
				{
					if(!strcmp(sim_or_dis,"sim"))
						temp_param.min_max = 1;
					else
						temp_param.min_max = 0;
				}
				random_reorder(simi_matrix, m, order_array, 0, temp_param);
				break;
		// do the nearest neighbor algorithm using chosen_dim as the first dimension in the arrangement
		case 2: if(correlation == 1)		// euclidean distance, the smaller the distance, the greater the similarity
				{
					if(!strcmp(sim_or_dis,"sim"))
						temp_param.min_max = 0;
					else
						temp_param.min_max = 1;
				}
				else
				{
					if(!strcmp(sim_or_dis,"sim"))
						temp_param.min_max = 1;
					else
						temp_param.min_max = 0;
				}
				nearest_neighbor(simi_matrix, m, order_array, 0, temp_param);
				break;
		case 3: if(correlation == 1)		// euclidean distance, the smaller the distance, the greater the similarity
				{
					if(!strcmp(sim_or_dis,"sim"))
						temp_param.min_max = 0;
					else
						temp_param.min_max = 1;
				}
				else
				{
					if(!strcmp(sim_or_dis,"sim"))
						temp_param.min_max = 1;
					else
						temp_param.min_max = 0;
				}
				greedy_algorithm(simi_matrix, m, order_array, 0, temp_param);
				break;
		return false;
	}

	for( count = 0; count < m; count++ )
		delete simi_matrix[count];
	delete simi_matrix;
	return true;
}
