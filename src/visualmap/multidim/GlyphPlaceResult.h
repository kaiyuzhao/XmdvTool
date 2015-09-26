/*
 * GlyphPlaceResult.h
 *
 *  Created on: Apr 22, 2009
 *      Author: Zaixian Xie
 */

/*
 * Class GlyphPlaceResult stores the position and size for glyphs.
 * An instance of class GlyphPlace will compute these values.
 * The OkcVisualMapResult has a data member which is an instance
 * of this class.
 */

#ifndef GLYPHPLACERESULT_H_
#define GLYPHPLACERESULT_H_

#include <vector>

class GlyphPlaceResult {

	friend class GlyphPlace;
	friend class VisGlyph;
public:
	GlyphPlaceResult();
	virtual ~GlyphPlaceResult();

private:
	/////////////////////////////////////////////////////////////////////
	//used for glyph placement
	//
	//size of each of these two arrays is int data_size
	//basex[k] is the x coordinate of the kth data point in the canvas
	//basey[k] is the y coordinate of the kth data point in the canvas
	std::vector<double>		basex, basey;
	// Computed data
	//
	//blocksize is the maximum size of a glyph
	//halfblocksize is half of the maximum size of a glyph
	double			blocksize, halfblocksize;
	//number of lines of glyphs in the canvas when it is placed one by one
	//it is also the number of glyphs in each line when it is placed one by one
	int				blockcnt;

};

#endif /* GLYPHPLACERESULT_H_ */
