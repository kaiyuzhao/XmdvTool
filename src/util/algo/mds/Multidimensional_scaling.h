

////////////////////////////////////////////////////////////////////////////////////////
//  n: number of objects to be mapped. The objects are ob_0, ob_1, ...ob_n-1.
//  lower_dimensionality:  the dimensionality of the subspace the objects to be mapped to.
//                         Typically it is 2. But it might be 3.
//  distance_matrix: the nXn distance matrix of the n objects
//
//       | dis(ob_0,ob_0)    dis(ob_1,ob_0)  ...   dis(ob_0, ob_n-1)     |
//       | dis(ob_0,ob_1)    dis(ob_1,ob_1)  ...   dis(ob_1, ob_n-1)     |
//       |                     ...                                       |
//       | dis(ob_0,ob_n-1)  dis(ob_1,ob_n-1)...   dis(ob_n-1, ob_n-1)   |
//
//  positions: array to store the objects' position in the lower dimensional space.
//             For example, if lower_dimensionality = 2, n = 100, then after the calling
//             to the function positions will be double[2][100],
//             with positions[0][0] contains the postion of ob_0 in the first dimension,
//             and  positions[1][0] contains the postion of ob_1 in the second dimension,
//  return: return if the MDS algorithm stops normally. elsewise return false.
////////////////////////////////////////////////////////////////////////////////////////

extern bool multidimensional_scaling(int n,
									 int lower_dimensionality,
									 double **distance_matrix,
									 double** positions);
