/*
 * OrderDimension.h
 *
 *  Created on: Nov 14, 2009
 *      Ported by Zaixian Xie
 */

/*
 * Class OrderDimension contains data structure and functions used by
 * automated dimension reordering.
 */

#ifndef ORDERDIMENSION_H_
#define ORDERDIMENSION_H_

#include "main/XmdvTool.h"
#include <vector>
#include "datatype/Vec2.h"

typedef struct {
	int algorithm;
	int min_max;
	int brushed;
	int correlation;
	double threshold_1;
	double threshold_2;
	int nn_start_dim;
	int eight_connected;
	// dimstack_size is used in similarity-based reordering when the view is dimension-stacking
	std::vector<double> dimstack_size;
	// The number of bins for each dimension, including only visible dimensions
	std::vector<int> cardinality;
	// The minimum value for each dimension, including only visible dimensions
	std::vector<double> dim_min;
	// The maximum value for each dimension, including only visible dimensions
	std::vector<double> dim_max;
	// The width of each block in dimension stacking
	double dimstack_X_blocksize;
	// The height of each block in dimension stacking
	double dimstack_Y_blocksize;
	int canvasWidth, canvasHeight;
} Reorder_Parameters;

class OkcData;
class DimOOOMap;

class OrderDimension {
public:
	// The order method
	enum ORDERMETHOD {
		OM_SIMILARITY = 0,	// Similarity-Based
		OM_CLUTTER = 1,		// Clutter-based
	};
	// The ordering algorithm
	enum ORDERALGORITHM {
		OA_OPTIMAL = 0,		// Optimal ordering
		OA_RANDOM = 1,		// Random Swapping
		OA_NN = 2,			// Nearest Neighbor
		OA_GREEDY = 3,		// Greedy Algorithm
	};
public:
	OrderDimension();
	virtual ~OrderDimension();

public:
	// The main entrance for dimension reordering
	static void DimensionReorder(OkcData* inputOkcData, DimOOOMap* map, Reorder_Parameters* reorderPara,
			OrderDimension::ORDERMETHOD method, XmdvTool::VISTYPE visType);
	///////////////////////////////////////helper function , swap two elements in an array//////////////////////////////////////////
	static void swap_element(std::vector<int> &array, int index_1, int index_2);

	//////////////////////////////////////recursive function for the optimal ordering///////////////////////////////////////////////
	static void recur_order(double ** matrix, int dim_num, std::vector<int> &temp_array, std::vector<int> &order_array, int start, int type, Reorder_Parameters param, int data_num = 0);

	///////////////////////////////////////////////function for optimal ordering///////////////////////////////////////
	static void optimal_reorder(double ** matrix, int dim_num, std::vector<int> &order_array, int type, Reorder_Parameters param, int data_num = 0);

	/////////////////////////////////////////////function for random ordering/////////////////////////////////////////
	static void random_reorder(double ** matrix, int dim_num, std::vector<int> &order_array, int type, Reorder_Parameters param, int data_num = 0);

	// helper function . find the dimension pair with the largest number of relationship between them
	//for example: correlation, outlier number, etc
	static double find_max_distance(double **matrix, int dim_num);

	static void nearest_neighbor(double ** matrix, int dim_num, std::vector<int> &order_array, int type, Reorder_Parameters param);

	// helper function for greedy algorithm. find the dimension pair with the smallest
	// similarity parameter and not yet in the tour
	static double find_smallest_pair(double ** matrix, int dim_num, double max, int &min_first_dim, int &min_next_dim, bool **flag);

	// helper function for greedy algorithm. find the dimension pair with the largest
	// relationship and not yet in the tour
	static double find_largest_pair(double ** matrix, int dim_num, double min, int &max_first_dim, int &max_next_dim, bool **flag);

	static bool in_same_group(int **neighbor_dim, int dim_num, int dim_1, int dim_2);

	static void greedy_algorithm(double ** matrix, int dim_num, std::vector<int> &order_array, int type, Reorder_Parameters param);

	//this is a recursive function called by isolated_ratio, neighbor_def: 0: 4-connected, 1: 8_connected
	static int assign_group(int i, int j, int X_bin, int Y_bin, bool **filled_bins, bool **flag, int neighbor_def);

	static double isolated_ratio(std::vector<Vec2> DimStckData, int set_size, double &group_ratio, double threshold, int neighbor_def, Reorder_Parameters param, int dim_num);

	static int get_contribution(int ind, double data, Reorder_Parameters param);

	static std::vector<Vec2> Calc_Dimstck_Pos(const std::vector<double> &size, double ** data_matrix, int set_size, int dim_num, std::vector<int> order_array, Reorder_Parameters param);

	//helper function for calculating the distances between dimensions
	static double calc_scale( double num, double **data_matrix, int data_num, int dimension);

	//get the similarity between two dimensions////////////////////////////////////////////
	static double simi_func(double **data_matrix, int data_num, int dim_1, int dim_2, int method);

	//calculate the sum of distances between neighboring dimensions//////////////////////////////
	//array gives the sequence of dimensions
	static double sim_total(double **similarity_matrix, int dim_num, std::vector<int> array);

	//reorder using similarity method////////////////////////////////////
	static bool similarity_reorder(int n, int m, double **data_matrix, std::vector<int> &order_array, Reorder_Parameters param);

	// Functions for the clutter-based reordering
	static double clutter_measure_symmetry(double **norm_matrix, int data_num, int dim_num, std::vector<int> array, double mono_threshold, double sym_threshold);

	static void clutterbased_reorder_glyph(int data_num, int dim_num, double **norm_matrix, std::vector<int> &order_array, Reorder_Parameters clutter_param);

	// Added by debbie 12/2003
	// Used for dimensional stacking purpose
	static void clutterbased_reorder_dimstck(int set_size, int dim_num, double ** data_matrix, std::vector<int> &order_array, Reorder_Parameters clutter_param);

	// Added by debbie 3/2004
	// Used for parallel coordinate clutter reduction
	static int Outlier_Between(double **norm_matrix, int data_num, int dim_1, int dim_2, double threshold);

	static double Total_Outlier_Number(double **outlier_matrix, int dim_num, std::vector<int> array);

	static void clutterbased_reorder_parcord(int set_size, int dim_num, double **norm_matrix, std::vector<int> &order_array, Reorder_Parameters clutter_param);

	// Added by debbie 3/2004
	// Used for scatter plot matrices clutter reduction

	static double Total_Separation(double **sim_matrix, int used_dim, std::vector<int> order, double threshold);

	static void clutter_based_reorder_scatter_high_card(double **data_matrix, int set_size, int dim_num, int used_dim, std::vector<int> &order_array, std::vector<bool> flag, Reorder_Parameters clutter_param);

	static void clutter_based_reorder_scatter_low_card(double **data_matrix, int set_size, int dim_num, int used_dim, std::vector<int> &order_array, std::vector<bool> flag);

	static void separate_dimensions(double **data_matrix, int set_size, int dim_num, std::vector<bool> &flag, Reorder_Parameters clutter_param);

	static void clutter_based_reorder_scatter_distinct(double **data_matrix, int set_size, int dim_num, std::vector<int> &order_array, Reorder_Parameters clutter_param);

	static void clutterbased_reorder_scatterplot(int set_size, int dim_num, double **data_matrix, std::vector<int> &order_array, Reorder_Parameters clutter_param);

	//For InterRing Use Only
	static bool similarity_reorder_interring(int n, int m, double **data_matrix, std::vector<int> &order_array, int choice, int correlation, char * sim_or_dis, int nn_start_dim);
};

#endif /* ORDERDIMENSION_H_ */
