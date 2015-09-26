#ifndef MDS_TYPES_HH
#define MDS_TYPES_HH


#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>


#include "util/algo/mds/IP_Vector.h"
#include "util/algo/mds/IP_Matrix.h"


//
// tyepdefs
//
typedef	IP_Vector<double>		MDS_Vector;
typedef IP_Matrix<double>		MDS_Matrix;

#endif // MDS_TYPES_HH
