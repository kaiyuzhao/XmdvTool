/*
 * DiagVisAttr.h
 *
 *  Created on: Aug 14, 2009
 *      Author: zyguo
 */

#ifndef DIAGVISATTR_H_
#define DIAGVISATTR_H_

#include <vector>
#include "visualmap/VisualAttribute.h"
#include "visualmap/multidim/OkcVisualMapResult.h"
#include "main/XmdvTool.h"


class OkcData;
class GlyphPlaceResult;
class OkcVisualMapResult;

class DiagVisAttr {
public:
	DiagVisAttr();
	virtual ~DiagVisAttr();

	std::vector< std::vector < VisualAttribute* > > multi_dim_data_VisAttr;

	OkcData* m_okcData;

	OkcVisualMapResult* okcVMR;

	void setOkcData(OkcData* okcdata);

	OkcData* getOkcData();

	void diagColorMapping();
	void initVisAttrArray();

	void setOkcVisualMapResult(OkcVisualMapResult* _okcVMR) ;

	OkcVisualMapResult* getOkcVisualMapResult();

};

#endif /* DIAGVISATTR_H_ */
