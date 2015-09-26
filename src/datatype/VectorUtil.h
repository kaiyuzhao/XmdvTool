/*
 * VectorUtil.h
 *
 *  Created on: Aug 23, 2010
 *      Author: Zaixian Xie
 */

#ifndef VECTORUTIL_H_
#define VECTORUTIL_H_

#include <vector>

class VectorUtil {
public:
	VectorUtil();
	virtual ~VectorUtil();

	template<class T>
	static void init2DVector( std::vector< std::vector<T> > &matrix, int row, int col) {
		matrix.resize(row);
		for (int i = 0; i < row; i++) {
			matrix[i].assign(col, 0);
		}
	}
};

#endif /* VECTORUTIL_H_ */
