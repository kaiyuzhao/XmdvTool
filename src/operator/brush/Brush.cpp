/*
 * brush.cpp
 *
 *  Created on: Sep 22, 2008
 *      Author: Zhenyu Guo
 */

#include <main/XmdvTool.h>
#include <operator/brush/Brush.h>
#include <data/multidim/OkcData.h>

Brush::Brush() {
	bound_type = STEP;
}

Brush::~Brush() {
}

void Brush::setDimSize(int dimSize) {
	m_dimSize = dimSize;
}

int  Brush::getDimSize() {
	return m_dimSize;
}

void Brush::Init(OkcData* dataset)
{
	// Initialize brushes here instead than in InitBrushSet() above

	//this->m_inputDataset = m_inputDataset;
	//this->m_inputDataset = dataset;
	m_dimSize =dataset->getDimSize();
	std::vector<double> dim_min, dim_max;
	dataset->getDimMinArr(dim_min);
	dataset->getDimMaxArr(dim_max);

	pos.resize(m_dimSize);
	size.resize(m_dimSize);
	bound_offset.resize(m_dimSize);
	min.resize(m_dimSize);
	max.resize(m_dimSize);
	ramp_min.resize(m_dimSize);
	ramp_max.resize(m_dimSize);

	for (int dim=0; dim<m_dimSize; dim++)
    {
		SetTriple(dim,
					(dim_max[dim] + dim_min[dim]) * .5,
					(dim_max[dim] - dim_min[dim]) * .5,
					(dim_max[dim] - dim_min[dim]) * .1);
	}
}

/*void Brush::MapVisualBrush(){
	DisplayMax.resize(dataMapResult->getNumVisibleCols());
	DisplayMin.resize(dataMapResult->getNumVisibleCols());
	DisplayRampMax.resize(dataMapResult->getNumVisibleCols());
	DisplayRampMin.resize(dataMapResult->getNumVisibleCols());

	dataMapResult->MapData(max, DisplayMax);
	dataMapResult->MapData(min, DisplayMin);
	dataMapResult->MapData(ramp_max, DisplayRampMax);
	dataMapResult->MapData(ramp_min, DisplayRampMin);
}*/

void Brush::SetTriple(int d, double _pos, double _size, double _offset)
{
	pos[d] = _pos;
	size[d] = _size;
	_size *= .5;
	min[d] = _pos - _size;
	max[d] = _pos + _size;
	bound_offset[d] = _offset;
	ramp_min[d] = min[d] - _offset;
	ramp_max[d] = max[d] + _offset;
}

// PAUL: Remove comments to restore this code
double Brush::Min(int d)
{
    return pos[d] - size[d]*.5;
}

double Brush::Max(int d)
{
    return pos[d] + size[d]*.5;
}

//
// Automatic global brush resizing
//
/*

void Brush::Resize(double factor)
{
	std::vector<double> dim_min, dim_max;
	m_inputDataset->getDimMinArr(dim_min);
	m_inputDataset->getDimMaxArr(dim_max);
   if (factor == 0) {
        // Resize to max
        for (int d=0; d<m_inputDataset->getDimSize(); d++)
		{
			SetPosSize(d, (dim_max[d] + dim_min[d]) / 2.0,
					dim_max[d] - dim_min[d]);
        }
    }
	else
	{
        for (int d=0; d<m_inputDataset->getDimSize(); d++)
		{
            size[d] *= factor;
            SetSize(d, size[d] * factor);

			double lmin, lmax;
			lmin = Min(d);
			lmax = Max(d);
			if (lmin < dim_min[d] || lmax > dim_max[d]) {
				lmin = MAX(lmin, dim_min[d] );
				lmax = MIN(lmax, dim_max[d]);
				SetMinMax(d, lmin, lmax);
			}
        }
    }
}

*/
/*
 * //commented by Zaixian because now
 * we do not need order_from and order_to
	int columns, rows;
	columns = m_inputDataset->d2_width;
	rows = m_inputDataset->data_size/columns+1;
    if (factor == 0) {
        // Resize to max
		order_from = 0;
		order_to = m_inputDataset->data_size-1;
		x_from= 0;
		x_to= columns-1;
		y_from= 0;
		y_to= rows-1;
    } else {
		double lsize, lpos;
		int tmp;
		lsize = order_to - order_from;
		lpos = (order_to + order_from)/2.0;
		lsize *= factor;
		tmp = (int)(lpos - lsize/2.0 + 0.5);
		order_from = MAX(tmp, 0);
		tmp = (int)(lpos + lsize/2.0 + 0.5);
		order_to = MIN(tmp, m_inputDataset->data_size-1);

		lsize = x_to - x_from;
		lpos = (x_to + x_from)/2.0;
		lsize *= factor;
		tmp = (int)(lpos - lsize/2.0 + 0.5);
		x_from = MAX(tmp, 0);
		tmp = (int)(lpos + lsize/2.0 + 0.5);
		x_to = MIN2(tmp, columns-1);

		lsize = y_to - y_from;
		lpos = (y_to + y_from)/2.0;
		lsize *= factor;
		tmp = (int)(lpos - lsize/2.0 + 0.5);
		y_from = MAX(tmp, 0);
		tmp = (int)(lpos + lsize/2.0 + 0.5);
		y_to = MIN(tmp, rows-1);
    }
*/

// PAUL
// When you set Pos, Size, ...
// The dependent values are automatically updated.
void Brush::SetPos(int d, double _pos)
{
	pos[d] = _pos;
	min[d] = _pos - size[d] * .5;
	max[d] = _pos + size[d] * .5;
	ramp_min[d] = min[d] - bound_offset[d];
	ramp_max[d] = max[d] + bound_offset[d];
}

void Brush::SetSize(int d, double _size)
{
	size[d] = _size;
	min[d] = pos[d] - _size * .5;
	max[d] = pos[d] + _size * .5;
	ramp_min[d] = min[d] - bound_offset[d];
	ramp_max[d] = max[d] + bound_offset[d];
}

void Brush::SetPosSize(int d, double _pos, double _size)
{
	pos[d] = _pos;
	size[d] = _size;
	_size *= .5;
	min[d] = _pos - _size;
	max[d] = _pos + _size;
	ramp_min[d] = min[d] - bound_offset[d];
	ramp_max[d] = max[d] + bound_offset[d];
}

void Brush::SetMinMax(int d,double lo,double hi)
{
	SetPosSize(d, (hi + lo)*.5, (hi - lo));
}
/*
void Brush::SetFromTo(int d, int from, int to)
{
    order_from = from;
    order_to = to;
}

void Brush::SetFromToX(int d, int from, int to)
{
    x_from = from;
    x_to = to;
}
*/
double Brush::RampMin(int d)
{
    return Min(d) - bound_offset[d];
}

double Brush::RampMax(int d)
{
    return Max(d) + bound_offset[d];
}

OkcData* Brush::toOkcDataStorage() {
	int i;
	OkcData* storage = new OkcData();

	storage->setBaseFlag(true);
	int dimSize = getDimSize();
	//each datapoint corresponds to an ARR
	storage->data_size = 7;
	storage->dims = dimSize;
	storage->data_buf->resize(7*dimSize);
	// put all of brush attribute into the data_buf
	storage->setOrigData(this->pos, 0);
	storage->setOrigData(this->size, 1);
	storage->setOrigData(this->min, 2);
	storage->setOrigData(this->max, 3);
	storage->setOrigData(this->bound_offset, 4);
	storage->setOrigData(this->ramp_min, 5);
	storage->setOrigData(this->ramp_max, 6);

	// The names, cardinality, dim_min and dim_max
	// does not make sense in this storage
	storage->names.resize(dimSize);
	storage->cardinality.resize(dimSize);
	storage->dim_min.resize(dimSize);
	storage->dim_max.resize(dimSize);
	for (i=0; i<dimSize; i++) {
		storage->names[i] = 0;
		storage->cardinality[i] = 0;
		storage->dim_min[i] = 0.0;
		storage->dim_max[i] = 0.0;
	}

	return storage;
}

void Brush::copyFromOkcDataStorage(OkcData* storage) {
	int dimSize = storage->getDimSize();
	m_dimSize = dimSize;
	this->pos.resize(dimSize);
	this->size.resize(dimSize);
	this->min.resize(dimSize);
	this->max.resize(dimSize);
	this->bound_offset.resize(dimSize);
	this->ramp_min.resize(dimSize);
	this->ramp_max.resize(dimSize);

	storage->getData(this->pos, 0);
	storage->getData(this->size, 1);
	storage->getData(this->min, 2);
	storage->getData(this->max, 3);
	storage->getData(this->bound_offset, 4);
	storage->getData(this->ramp_min, 5);
	storage->getData(this->ramp_max, 6);
}
