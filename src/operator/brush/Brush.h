/*
 * brush.h
 *
 *  Created on: Sep 22, 2008
 *      Author: Zhenyu Guo
 */

/*
 * Class Brush defines all parameters
 * to denote a flat brush for an OkcData.
 * An instance of class Brush can be converted into
 * an OkcData storage and put to the transformation pipleine.
 * The functions toOkcDataStorage() and copyFromOkcDataStorage()
 * do these conversions.
 */

#ifndef BRUSH_H_
#define BRUSH_H_

class OkcData;

class Brush
{
public:
	Brush();
	~Brush();

public:
	enum BoundaryType {STEP, RAMP};

	BoundaryType bound_type; // brush boundary type
	int m_dimSize;

	std::vector<double> pos;
	std::vector<double> size;
	std::vector<double> min;
	std::vector<double> max;
	std::vector<double> bound_offset; // boundary offset from min/max

	std::vector<double> ramp_min;
	std::vector<double> ramp_max;

public:
	void setDimSize(int dimSize);
	int getDimSize();

	double Min(int d);
	double Max(int d);

	inline void resize(int dims) {
		pos.resize(dims);
		size.resize(dims);
		bound_offset.resize(dims);
		min.resize(dims);
		max.resize(dims);
		ramp_min.resize(dims);
		ramp_max.resize(dims);
	}

	inline const std::vector<double> & GetPos() const {
		return pos;
	}
	inline const std::vector<double> & GetSize() const {
		return size;
	}

	inline double Pos(int d) const {
		return pos[d];
	}
	inline double Size(int d) const {
		return size[d];
	}
	inline double BoundOffset(int d) const {
		return bound_offset[d];
	}

	inline double Min(int d) const {
		return min[d];
	}
	inline double Max(int d) const {
		return max[d];
	}
	inline const std::vector<double> & GetBoundOffset() const {
		return bound_offset;
	}
	void SetMinMax(int d,double lo,double hi);

	void SetPos(int d, double pos);
	void SetSize(int d, double size);
	void SetPosSize(int d, double pos, double size);
	void SetBoundOffset(int d, double offset);

	void Init(OkcData* dataset);
	void SetTriple(int d, double pos, double size, double offset);
	// PAUL: lang_opt brush_org
	// Nice inline functions
	inline double RampMin(int d) const {
		return ramp_min[d];
	}
	inline double RampMax(int d) const {
		return ramp_max[d];
	}

	double RampMin(int d);
	double RampMax(int d);

	// convert the current brush to an OkcData storage
	OkcData* toOkcDataStorage();
	// recreate the brush from an OkcData storage
	void copyFromOkcDataStorage(OkcData* storage);
};



#endif /* BRUSH_H_ */


