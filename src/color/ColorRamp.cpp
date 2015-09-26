#include <assert.h>
#include <math.h>
#include <string.h>

#include "color/ColorRamp.h"
#include "color/ColorManager.h"

using namespace std;

#define	SUP(a,b)	( ((a)>(b)) ? (a) : (b) )
#define	INF(a,b)	( ((a)<(b)) ? (a) : (b) )
#define	ABS(a)		( ((a)<0) ? -(a) : (a) )

#define tabCmpnt(c,n)	(m_tabcmpnt[((n)*4)+(c)])
#define tabCmpntBins(c,n)	(m_tabcmpntbins[((n)*4)+(c)])

//-----------------------------------------------------------------------------
ColorRamp::ColorRamp(unsigned int color)
{
	m_dimlut = 1; // no need to allocate more memory than necessary
	m_maxndx = m_dimlut - 1;
	m_lutratio = (float)m_maxndx/255.0f;
	m_tanlen = m_lutratio*25.0f;
	m_tabcmpnt = new float[4*m_dimlut];
	m_tabcmpntbins = new float[4*m_dimlut];

	m_origBins = 5;
	m_numBins = 5;
	m_model = MODEL_RGB;
	m_mode = CONTINUOUS;
	m_type = RAMP_SINGLE;
	m_id = ColorManager::TOTAL_RAMPS;
	m_linkType = LINK_NONE;

	m_tabcmpnt[0] = ((color>>16) & 0xFF)/255.;
	m_tabcmpnt[1] = ((color>>8) & 0xFF)/255.;
	m_tabcmpnt[2] = ((color) & 0xFF)/255.;
	m_tabcmpnt[3] = ((color>>24) & 0xFF)/255.;
	if(color <= 0xFFFFFF) m_tabcmpnt[3] = 1.0f;

	m_tabcmpntbins[0] = ((color>>16) & 0xFF)/255.;
	m_tabcmpntbins[1] = ((color>>8) & 0xFF)/255.;
	m_tabcmpntbins[2] = ((color) & 0xFF)/255.;
	m_tabcmpntbins[3] = ((color>>24) & 0xFF)/255.;
	if(color <= 0xFFFFFF) m_tabcmpntbins[3] = 1.0f;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
ColorRamp::ColorRamp(unsigned int N, float list[][5], int id, RampType type, ColorModel model, LinkType linktype)
{
	m_dimlut = 256;
	m_maxndx = m_dimlut - 1;
	m_lutratio = (float)m_maxndx/255.0f;
	m_tanlen = m_lutratio*25.0f;
	m_tabcmpnt = new float[4*m_dimlut];
	m_tabcmpntbins = new float[4*m_dimlut];
	m_id = id;
	m_linkType = linktype;

	if(N < 5 && type != RAMP_LINK)
		m_origBins = 5;
	else
		m_origBins = N;
	m_numBins = m_origBins;

	m_model = model;
	if(type == RAMP_QUAL || type == RAMP_LINK) m_mode = BINS;
	else m_mode = CONTINUOUS;
	m_type = type;

	initialize(N, list);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
ColorRamp::ColorRamp(const ColorRamp& ramp)
{
	m_dimlut = ramp.m_dimlut;
	m_maxndx = ramp.m_maxndx;
	m_lutratio = ramp.m_lutratio;
	m_tanlen = ramp.m_tanlen;
	m_tabcmpnt = new float[4*m_dimlut];
	memcpy(m_tabcmpnt, ramp.m_tabcmpnt, 4*m_dimlut*sizeof(float));
	m_tabcmpntbins = new float[4*m_dimlut];
	memcpy(m_tabcmpntbins, ramp.m_tabcmpntbins, 4*m_dimlut*sizeof(float));

	m_numBins = ramp.m_numBins;
	m_origBins = ramp.m_origBins;

	m_model = ramp.m_model;
	m_mode = ramp.m_mode;
	m_type = ramp.m_type;
	m_linkType = ramp.m_linkType;
	m_id = ramp.m_id;

	for (int i=0; i<4; ++i) {
		for (PointList::const_iterator j=ramp.m_userPoints[i].begin();
				j!=ramp.m_userPoints[i].end(); ++j)
		{
			RampPoint *pt = *j;
			RampPoint *newpt = new RampPoint;
			*newpt = *pt;
			m_userPoints[i].push_back(newpt);
		}
	}
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void ColorRamp::operator = (const ColorRamp& ramp)
{
	if (this == &ramp) return;
	m_dimlut = ramp.m_dimlut;
	m_maxndx = ramp.m_maxndx;
	m_lutratio = ramp.m_lutratio;
	m_tanlen = ramp.m_tanlen;

	delete [] m_tabcmpnt;
	m_tabcmpnt = new float[4*m_dimlut];
	memcpy(m_tabcmpnt, ramp.m_tabcmpnt, 4*m_dimlut*sizeof(float));

	delete [] m_tabcmpntbins;
	m_tabcmpntbins = new float[4*m_dimlut];
	memcpy(m_tabcmpntbins, ramp.m_tabcmpntbins, 4*m_dimlut*sizeof(float));

	m_numBins = ramp.m_numBins;
	m_origBins = ramp.m_origBins;

	m_model = ramp.m_model;
	m_mode = ramp.m_mode;
	m_type = ramp.m_type;
	m_linkType = ramp.m_linkType;
	m_id = ramp.m_id;

	int i;
	for (i=0; i<4; ++i) {
		for (PointListIter iter=m_userPoints[i].begin(); 
				iter != m_userPoints[i].end(); ++iter)
		{
			RampPoint *pt = *iter;
			delete pt;
			pt = NULL;
		}
		m_userPoints[i].clear();
	}
	for (i=0; i<4; ++i) {
		for (PointList::const_iterator j=ramp.m_userPoints[i].begin();
				j!=ramp.m_userPoints[i].end(); ++j)
		{
			RampPoint *pt = *j;
			RampPoint *newpt = new RampPoint;
			*newpt = *pt;
			m_userPoints[i].push_back(newpt);
		}
	}
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
ColorRamp::~ColorRamp(void)
{
	for (int i=0; i<4; ++i) {
		for (PointListIter iter=m_userPoints[i].begin(); 
				iter != m_userPoints[i].end(); ++iter)
		{
			RampPoint *pt = *iter;
			delete pt;
			pt = NULL;
		}
		m_userPoints[i].clear();
	}
	delete [] m_tabcmpnt;
	delete [] m_tabcmpntbins;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void ColorRamp::initialize(unsigned int N /*= 0*/, float list[][5] /*= NULL*/)
{
	if (N == 0) 
	{
		for (int i=0; i<4; i++)
		{
			RampPoint *userpoint = new RampPoint;
			userpoint->mode = ColorRamp::POLYNOMIAL;
			userpoint->point.x = 0.;
			userpoint->point.y = 0.;
			userpoint->tangent.x0 = 0.;
			userpoint->tangent.y0 = 0.;
			userpoint->tangent.x1 = m_tanlen;
			userpoint->tangent.y1 = 0.;
			m_userPoints[i].push_back(userpoint);

			userpoint = new RampPoint;
			userpoint->mode = ColorRamp::POLYNOMIAL;
			userpoint->point.x = (float)m_maxndx;
			userpoint->point.y = (float)m_maxndx;
			userpoint->tangent.x0 = (float)(m_maxndx-m_tanlen);
			userpoint->tangent.y0 = (float)m_maxndx;
			userpoint->tangent.x1 = (float)m_maxndx;
			userpoint->tangent.y1 = (float)m_maxndx;
			m_userPoints[i].push_back(userpoint);
		}
	} 
	else 
	{
		for (unsigned int i=0; i<N; i++)
		{
			for (int j=1; j<=4; j++)
			{
				PointListIter iter = m_userPoints[j-1].begin();

				RampPoint *userpoint = new RampPoint;
				userpoint->point.x = (float) list[i][0];
				userpoint->point.y = (float) list[i][j];
				userpoint->mode = ColorRamp::LINEAR;
				if (i==0)
				{
					userpoint->tangent.x0 = 0.;
					userpoint->tangent.y0 = 0.;
					userpoint->tangent.x1 = m_tanlen;
					userpoint->tangent.y1 = 0.;
					m_userPoints[j-1].push_back(userpoint);
				}
				else
				{
					RampPoint *prevpoint = *iter;
					float	slope = userpoint->point.y - prevpoint->point.y,
							dx = m_tanlen/sqrt(slope*slope + 1),
							dy = slope*dx;
					userpoint->tangent.x0 = userpoint->point.x - dx;
					userpoint->tangent.y0 = userpoint->point.y - dy;
					userpoint->tangent.x1 = userpoint->point.x + dx;
					userpoint->tangent.y1 = userpoint->point.y + dy;

					m_userPoints[j-1].push_back(userpoint);
					++iter;
				}
			}
		}
	}

	for (int cmpnt=0; cmpnt < 4; ++cmpnt) 
	{
		for (PointListIter iter=m_userPoints[cmpnt].begin()+1;
				iter != m_userPoints[cmpnt].end(); ++iter)
		{
			update(cmpnt, iter);
		}
	}

	updateBins();
}

void ColorRamp::updateBins() {
	if(m_numBins < 2) m_numBins = 2;
	if(m_type == RAMP_QUAL && m_numBins > m_origBins) m_numBins = m_origBins;
	int index = 0;
	float color[4];
	double distance;
	for(int i = 0; i < m_numBins; i++) {
		distance = (double)m_maxndx / (double)m_numBins;
		// For qualitative ramps, get the colors in the order they were originally listed
		if(m_type == RAMP_QUAL || m_type == RAMP_LINK) {
			color[0] = tabCmpnt(0, int(((double)m_maxndx/(double)(m_origBins - 1))*i));
			color[1] = tabCmpnt(1, int(((double)m_maxndx/(double)(m_origBins - 1))*i));
			color[2] = tabCmpnt(2, int(((double)m_maxndx/(double)(m_origBins - 1))*i));
			color[3] = tabCmpnt(3, int(((double)m_maxndx/(double)(m_origBins - 1))*i));
		// For all other ramps, calculate the color at intervals and use those values
		} else {
			color[0] = tabCmpnt(0, int(distance*i + distance/2));
			color[1] = tabCmpnt(1, int(distance*i + distance/2));
			color[2] = tabCmpnt(2, int(distance*i + distance/2));
			color[3] = tabCmpnt(3, int(distance*i + distance/2));
		}
		for(int j = 0; j < int(distance); j++) {
			for(int k = 0; k < 4; k++)
				m_tabcmpntbins[index*4+k] = color[k];
			index++;
		}
	}

	// Accommodate for any rounding errors
	for(; index <= m_maxndx; index++) {
		for(int j = 0; j < 4; j++)
			m_tabcmpntbins[index*4 + j] = color[j];
	}
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
float ColorRamp::evaluate(int index, int cmpnt)
{
	if(m_type == RAMP_SINGLE) return tabCmpnt(cmpnt, 0);
	if(m_mode == BINS) return tabCmpntBins(cmpnt, index);
	return tabCmpnt(cmpnt, index);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
ColorRamp::PointListIter ColorRamp::begin(int cmpnt)
{
	return m_userPoints[cmpnt].begin();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
ColorRamp::PointListIter ColorRamp::end(int cmpnt)
{
	return m_userPoints[cmpnt].end();
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
int ColorRamp::insertPoint(float, float, int ndx, int cmpnt,
		PointListIter &point)
{
	float	dx, dy, slope, coeff[4];
	int	i;
	RampPoint::Tangent *tg;
	RampPoint *upoint, *userpoint;

	PointListIter iter = begin(cmpnt);
	upoint = *iter;
	while ( (int) (*(iter+1))->point.x <= ndx ) {
		++iter;
		if ( iter == end(cmpnt) )
			return 0;
		upoint = *iter;
	}
	if ( (int) upoint->point.x == ndx ) {
		point = iter;
		return 1;
	}

	userpoint = new RampPoint;

	userpoint->point.x = (float) ndx;
	userpoint->point.y = solvePolynomial( (float) ndx, upoint->polynom );
	userpoint->point.y = SUP( INF(userpoint->point.y, (float)m_maxndx), 0.0f );
	userpoint->mode = upoint->mode;
	for ( i=0; i<4; i++ ) {
		userpoint->polynom[i] = upoint->polynom[i];
	}
	point = m_userPoints[cmpnt].insert(iter+1, userpoint);

	// calculation of slope = P' (index)
	if ( userpoint->point.y == (float)m_maxndx || userpoint->point.y == 0.0f ) {
		dx = 25.0f;
		dy = 0.0f;
	} else {
		coeff[0] = userpoint->polynom[0]*3.0f;
		coeff[1] = userpoint->polynom[1]*2.0f;
		coeff[2] = userpoint->polynom[2];
		slope = coeff[0];
		for (i=1; i<3; i++) {
			slope *= (float) ndx;	
			slope += coeff[i];
		}
		/*
		Recall :
		cos( arctg(x) ) = 1/sqr( 1 + x^2 );
		sin( arctg(x) ) = |x|/sqr( 1 + x^2 );
		 */
		dx = (float) (25. / sqrt( slope*slope + 1. ));
		dy = dx*slope;
	}
	dx *= m_lutratio;
	dy *= m_lutratio;

	tg = &userpoint->tangent;
	tg->x0 = userpoint->point.x - dx;
	tg->x1 = userpoint->point.x + dx;
	tg->y0 = userpoint->point.y - dy;
	tg->y1 = userpoint->point.y + dy;

	return 1;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void ColorRamp::removePoint(PointListIter iter, int cmpnt)
{
	if ( iter == begin(cmpnt) || iter == end(cmpnt)-1 )
		return;
	RampPoint *curPoint = *iter;

	PointListIter iterm1 = m_userPoints[cmpnt].erase(iter);
	update( cmpnt, iterm1 );
	delete curPoint;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
int ColorRamp::movePoint(PointListIter currIter, int curCmpnt,
		float x, float y)
{
	float	dx, dy, minx, maxx;
	RampPoint *curPoint = *currIter;

	y = SUP( INF(y,(float)m_maxndx), 0.0f );
	x = ((x-(int)x)<.5) ? (float)(int)x : (float)(int)(x+1);
	y = ((y-(int)y)<.5) ? (float)(int)y : (float)(int)(y+1);
	if ( currIter != (end(curCmpnt)-1) && currIter != begin(curCmpnt) )
	{
		minx = (*(currIter-1))->point.x + 1.0f;
		maxx = (*(currIter+1))->point.x - 1.0f;
		x = SUP( INF(x, maxx), minx );

		dx = (float)x - curPoint->point.x;
		curPoint->point.x = (float) x;
		curPoint->tangent.x0 += (float) dx;
		curPoint->tangent.x1 += (float) dx;
	}
	dy = (float)y - curPoint->point.y;
	curPoint->point.y = (float)y;
	curPoint->tangent.y0 += (float) dy;
	curPoint->tangent.y1 += (float) dy;

	if (currIter == begin(curCmpnt))
		update(curCmpnt, currIter+1);
	else
		update(curCmpnt, currIter);
	return 1;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
int ColorRamp::moveTangent(PointListIter currIter, int curCmpnt,
		int sens, float x, float y)
{
	float	dx, dy, slope;
	RampPoint::Tangent *tg;
	RampPoint *curPoint = *currIter;

	if ( sens == -1 )
	{
		x = INF(x, curPoint->point.x);
	}
	else
	{
		x = SUP(x, curPoint->point.x);
	}
	if ( x != curPoint->point.x )
	{
		slope =		(curPoint->point.y - y);
		slope /=	(curPoint->point.x - x);
		dx = (float) (25. / sqrt( slope*slope + 1.0f ));
		dy = dx*slope;
	}
	else
	{
		dx = 0.;
		dy = (curPoint->point.y < y) ? sens*25.0f : -sens*25.0f;
	}

	dx *= m_lutratio;
	dy *= m_lutratio;

	tg = &curPoint->tangent;
	tg->x0 = curPoint->point.x;
	tg->y0 = curPoint->point.y;
	if ( currIter != begin(curCmpnt) ) {
		tg->x0 -= dx;
		tg->y0 -= dy;
	}
	tg->x1 = curPoint->point.x;
	tg->y1 = curPoint->point.y;
	if ( currIter != (end(curCmpnt)-1) )
	{
		tg->x1 += dx;
		tg->y1 += dy;
	}

	if (currIter == begin(curCmpnt))
		update(curCmpnt, currIter+1);
	else
		update(curCmpnt, currIter);
	return 1;
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void ColorRamp::update(int cmpnt, PointListIter curr)
{
	if (curr == m_userPoints[cmpnt].begin())
		return;

	int	i;
	float	val, t0, t1;
	RampPoint::Tangent *tg;
	RampPoint *upoint = *curr;

	tg = &upoint->tangent;
	if ( tg->x1 != tg->x0 ) {
		t0 = (tg->y1-tg->y0) / (tg->x1-tg->x0);
	} else {
		t0 = (tg->y1-tg->y0)*10000.0f;
	}

	tabCmpnt(cmpnt,(int) upoint->point.x) = upoint->point.y/(float)m_maxndx;
	if ( curr != m_userPoints[cmpnt].end() ) {
		RampPoint *prevupoint = *(curr-1);

		tg = &prevupoint->tangent;
		if ( tg->x1 != tg->x0 ) {
			t1 = (tg->y1-tg->y0) / (tg->x1-tg->x0);
		} else {
			t1 = (tg->y1-tg->y0)*10000.0f;
		}

		getPolynomial(prevupoint->mode,
				prevupoint->point.x, prevupoint->point.y, t1,
				upoint->point.x, upoint->point.y, t0,
				prevupoint->polynom);

		for ( i=(int) prevupoint->point.x; i<(int) upoint->point.x; i++ ) {
			val = solvePolynomial((float) i,
					prevupoint->polynom)/(float)m_maxndx;
			tabCmpnt(cmpnt,i) = INF( SUP(val,0.0f), 1.0f );
		}
	}

	if ( curr+1 != m_userPoints[cmpnt].end() ) {
		RampPoint *nextupoint = *(curr+1);

		tg = &nextupoint->tangent;
		if ( tg->x1 != tg->x0 ) {
			t1 = (tg->y1-tg->y0) / (tg->x1-tg->x0);
		} else {
			t1 = (tg->y1-tg->y0)*10000.0f;
		}

		getPolynomial(upoint->mode,
				upoint->point.x, upoint->point.y, t0,
				nextupoint->point.x, nextupoint->point.y, t1,
				upoint->polynom);

		for ( i=1+(int) upoint->point.x; i<(int) nextupoint->point.x; i++ ) {
			val = solvePolynomial( (float) i, upoint->polynom )/(float)m_maxndx;
			tabCmpnt(cmpnt,i) = INF( SUP(val,0.0f), 1.0f );
		}
		tabCmpnt(cmpnt,(int) nextupoint->point.x) = nextupoint->point.y/(float)m_maxndx;
	}
}
//-----------------------------------------------------------------------------

//------------------------------------------------------------------------
void ColorRamp::getPolynomial(RampFunction mode,
		float a0, float b0, float t0,
		float a1, float b1, float t1,
		float coeff[4])
{
	typedef float Matrix[4][4];
	Matrix	mat, INV_mat;
	int	i;
	float	val;

	switch (mode)
	{
	case CONSTANT:
		/*			we want P(x) = b0 */
		coeff[0] = 0.0f;
		coeff[1] = 0.0f;
		coeff[2] = 0.0f;
		coeff[3] = b0;
		break;

	case LINEAR:
		/*
			we want P(x) = b0 + (x-a0)/(a1-a0)*(b1-b0)
			i.e. P(x) = x*[(b1-b0)/(a1-a0)] + b0 - a0*[(b1-b0)/(a1-a0)]
		 */
		val = (b1-b0)/(a1-a0);
		coeff[0] = 0.0f;
		coeff[1] = 0.0f;
		coeff[2] = val;
		coeff[3] = b0 - a0*val;
		break;

	case POLYNOMIAL:
		/*
			we want a polynom P(x) = Ax^3 + Bx^2 + Cx + D
			so that : P(a0) = b0, P'(a0) = t0, P(a1) = b1, P'(a1) = t1 
			i.e.

			| a0^3		a0^2	a0	1 | |A|	  |b0|
			| a1^3		a1^2	a1	1 | |B| = |b1|
			| 3a0^2		2a0		1	0 | |C|	  |t0|
			| 3a1^2		2a1		1	0 | |D|   |t1|
		 */
		val = 1.0f;
		for (i=3; i>=0; i--)
		{
			mat[0][i] = val;
			val *= a0;
		}

		val = 1.0f;
		for (i=3; i>=0; i--)
		{
			mat[1][i] = val;
			val *= a1;
		}

		mat[2][3] = 0.0f;
		mat[2][2] = 1.0f;
		mat[2][1] = 2.0f*a0;
		mat[2][0] = 3.0f*a0*a0;

		mat[3][3] = 0.0f;
		mat[3][2] = 1.0f;
		mat[3][1] = 2.0f*a1;
		mat[3][0] = 3.0f*a1*a1;

		invertmat( mat, INV_mat );

		for (i=0; i<4; i++)
			coeff[i] = INV_mat[i][0]*b0 +
			INV_mat[i][1]*b1 +
			INV_mat[i][2]*t0 +
			INV_mat[i][3]*t1;
		break;
	}
}
//------------------------------------------------------------------------

//------------------------------------------------------------------------
float ColorRamp::solvePolynomial(float x, float *coeff)
{
	float	val = coeff[0];

	for (int j=1; j<4; j++) {
		val *= x;
		val += coeff[j];
	}

	return	val;
}
//------------------------------------------------------------------------

//////////////////////////////////////////////////////////////////////////
// Matrix inversion helper function
void ColorRamp::invertmat(float from[4][4], float to[4][4])
{
	float wtemp[4][8];
	float m0,m1,m2,m3,s;
	float *r0,*r1,*r2,*r3, *rtemp;

	r0 = wtemp[0];
	r1 = wtemp[1];
	r2 = wtemp[2];
	r3 = wtemp[3];
	r0[0] = from[0][0];		/* build up [A][I]	*/
	r0[1] = from[0][1];
	r0[2] = from[0][2];
	r0[3] = from[0][3];
	r0[4] = 1.0;
	r0[5] = 0.0;
	r0[6] = 0.0;
	r0[7] = 0.0;
	r1[0] = from[1][0];
	r1[1] = from[1][1];
	r1[2] = from[1][2];
	r1[3] = from[1][3];
	r1[4] = 0.0;
	r1[5] = 1.0;
	r1[6] = 0.0;
	r1[7] = 0.0;
	r2[0] = from[2][0];
	r2[1] = from[2][1];
	r2[2] = from[2][2];
	r2[3] = from[2][3];
	r2[4] = 0.0;
	r2[5] = 0.0;
	r2[6] = 1.0;
	r2[7] = 0.0;
	r3[0] = from[3][0];
	r3[1] = from[3][1];
	r3[2] = from[3][2];
	r3[3] = from[3][3];
	r3[4] = 0.0;
	r3[5] = 0.0;
	r3[6] = 0.0;
	r3[7] = 1.0;

	if (r0[0] == 0.0) {		/* swap rows if needed		*/
		if (r1[0] == 0.0) {
			if (r2[0] == 0.0) {
				if (r3[0] == 0.0) goto singular;
				rtemp = r0; r0 = r3; r3 = rtemp;
			}
			else {rtemp = r0; r0 = r2; r2 = rtemp;}
		}
		else {rtemp = r0; r0 = r1; r1 = rtemp;}
	}
	m1 = r1[0]/r0[0];		/* eliminate first variable	*/
	m2 = r2[0]/r0[0];
	m3 = r3[0]/r0[0];
	s = r0[1];
	r1[1] = r1[1] - m1 * s;
	r2[1] = r2[1] - m2 * s;
	r3[1] = r3[1] - m3 * s;
	s = r0[2];
	r1[2] = r1[2] - m1 * s;
	r2[2] = r2[2] - m2 * s;
	r3[2] = r3[2] - m3 * s;
	s = r0[3];
	r1[3] = r1[3] - m1 * s;
	r2[3] = r2[3] - m2 * s;
	r3[3] = r3[3] - m3 * s;
	s = r0[4];
	if (s != 0.0) {
		r1[4] = r1[4] - m1 * s;
		r2[4] = r2[4] - m2 * s;
		r3[4] = r3[4] - m3 * s;
	}
	s = r0[5];
	if (s != 0.0) {
		r1[5] = r1[5] - m1 * s;
		r2[5] = r2[5] - m2 * s;
		r3[5] = r3[5] - m3 * s;
	}
	s = r0[6];
	if (s != 0.0) {
		r1[6] = r1[6] - m1 * s;
		r2[6] = r2[6] - m2 * s;
		r3[6] = r3[6] - m3 * s;
	}
	s = r0[7];
	if (s != 0.0) {
		r1[7] = r1[7] - m1 * s;
		r2[7] = r2[7] - m2 * s;
		r3[7] = r3[7] - m3 * s;
	}

	if (r1[1] == 0.0) {		/* swap rows if needed		*/
		if (r2[1] == 0.0) {
			if (r3[1] == 0.0) goto singular;
			rtemp = r1; r1 = r3; r3 = rtemp;
		}
		else {rtemp = r1; r1 = r2; r2 = rtemp;}
	}
	m2 = r2[1]/r1[1];		/* eliminate second variable	*/
	m3 = r3[1]/r1[1];
	r2[2] = r2[2] - m2 * r1[2];
	r3[2] = r3[2] - m3 * r1[2];
	r3[3] = r3[3] - m3 * r1[3];
	r2[3] = r2[3] - m2 * r1[3];
	s = r1[4];
	if (s != 0.0) {
		r2[4] = r2[4] - m2 * s;
		r3[4] = r3[4] - m3 * s;
	}
	s = r1[5];
	if (s != 0.0) {
		r2[5] = r2[5] - m2 * s;
		r3[5] = r3[5] - m3 * s;
	}
	s = r1[6];
	if (s != 0.0) {
		r2[6] = r2[6] - m2 * s;
		r3[6] = r3[6] - m3 * s;
	}
	s = r1[7];
	if (s != 0.0) {
		r2[7] = r2[7] - m2 * s;
		r3[7] = r3[7] - m3 * s;
	}

	if (r2[2] == 0.0) {		/* swap last 2 rows if needed	*/
		if (r3[2] == 0.0) goto singular;
		rtemp = r2; r2 = r3; r3 = rtemp;
	}
	m3 = r3[2]/r2[2];		/* eliminate third variable	*/
	r3[3] = r3[3] - m3 * r2[3];
	r3[4] = r3[4] - m3 * r2[4];
	r3[5] = r3[5] - m3 * r2[5];
	r3[6] = r3[6] - m3 * r2[6];
	r3[7] = r3[7] - m3 * r2[7];

	if (r3[3] == 0.0) goto singular;
	s = 1.0f/r3[3];		/* now back substitute row 3	*/
	r3[4] = r3[4] * s;
	r3[5] = r3[5] * s;
	r3[6] = r3[6] * s;
	r3[7] = r3[7] * s;

	m2 = r2[3];			/* now back substitute row 2	*/
	s = 1.0f/r2[2];
	r2[4] = s * (r2[4] - r3[4] * m2);
	r2[5] = s * (r2[5] - r3[5] * m2);
	r2[6] = s * (r2[6] - r3[6] * m2);
	r2[7] = s * (r2[7] - r3[7] * m2);
	m1 = r1[3];
	r1[4] = (r1[4] - r3[4] * m1);
	r1[5] = (r1[5] - r3[5] * m1);
	r1[6] = (r1[6] - r3[6] * m1);
	r1[7] = (r1[7] - r3[7] * m1);
	m0 = r0[3];
	r0[4] = (r0[4] - r3[4] * m0);
	r0[5] = (r0[5] - r3[5] * m0);
	r0[6] = (r0[6] - r3[6] * m0);
	r0[7] = (r0[7] - r3[7] * m0);

	m1 = r1[2];			/* now back substitute row 1	*/
	s = 1.0f/r1[1];
	r1[4] = s * (r1[4] - r2[4] * m1);
	r1[5] = s * (r1[5] - r2[5] * m1);
	r1[6] = s * (r1[6] - r2[6] * m1);
	r1[7] = s * (r1[7] - r2[7] * m1);
	m0 = r0[2];
	r0[4] = (r0[4] - r2[4] * m0);
	r0[5] = (r0[5] - r2[5] * m0);
	r0[6] = (r0[6] - r2[6] * m0);
	r0[7] = (r0[7] - r2[7] * m0);

	m0 = r0[1];			/* now back substitute row 0	*/
	s = 1.0f/r0[0];
	r0[4] = s * (r0[4] - r1[4] * m0);
	r0[5] = s * (r0[5] - r1[5] * m0);
	r0[6] = s * (r0[6] - r1[6] * m0);
	r0[7] = s * (r0[7] - r1[7] * m0);

	to[0][0] = r0[4];		/* copy results back		*/
	to[0][1] = r0[5];
	to[0][2] = r0[6];
	to[0][3] = r0[7];
	to[1][0] = r1[4];
	to[1][1] = r1[5];
	to[1][2] = r1[6];
	to[1][3] = r1[7];
	to[2][0] = r2[4];
	to[2][1] = r2[5];
	to[2][2] = r2[6];
	to[2][3] = r2[7];
	to[3][0] = r3[4];
	to[3][1] = r3[5];
	to[3][2] = r3[6];
	to[3][3] = r3[7];
	return;

	singular:
	std::cerr << "invertmat: singular matrix" << std::endl;
	return;
}
