#ifndef RGBT_H_
#define RGBT_H_

#include <iostream>

#define RValue(rgb)      ((unsigned char)(rgb))
#define GValue(rgb)      ((unsigned char)((rgb) >> 8))
#define BValue(rgb)      ((unsigned char)((rgb)>>16))

#define RGB2int(r,g,b) \
((unsigned int)(((unsigned char)(r)      | \
((unsigned int)((unsigned char)(g))<<8)) | \
((unsigned int)((unsigned char)(b))<<16)))

struct RGBt {
    double R,G,B,A;

	static const RGBt WHITE;
	static const RGBt BLACK;


    RGBt(void) : R(1.), G(1.), B(1.), A(1.) {}

    RGBt(double r,double g,double b, double a = 1.)
		: R(r), G(g), B(b), A(a) {}

    RGBt(const unsigned long &c) {
        B = (c & 0xFF)/255.;
        G = ((c>>8) & 0xFF)/255.;
        R = ((c>>16) & 0xFF)/255.;
		A = ((c>>24) & 0xFF)/255.;
    }

	unsigned long getColor() const {
		return ( ((unsigned long)( 255. * B))       |
				 ((unsigned long)( 255. * G)) << 8  |
				 ((unsigned long)( 255. * R)) << 16 |
				 ((unsigned long)( 255. * A)) << 24 );
	}

	void blendTo(const RGBt& dest, double alpha)
	{
		R = R * alpha + ((1 - alpha)*dest.R),
		G = G * alpha + ((1 - alpha)*dest.G),
		B = B * alpha + ((1 - alpha)*dest.B);
	}

    RGBt interpolate(RGBt c1, RGBt c2, double alpha)
    {
		alpha *= 0.9;
		return RGBt(
			c1.R * (1.0-alpha) + c2.R * alpha,
			c1.G * (1.0-alpha) + c2.G * alpha,
			c1.B * (1.0-alpha) + c2.B * alpha);
	}



	friend RGBt blend(const RGBt& src, const RGBt& dest, double alpha);
    friend std::ostream &operator<<(std::ostream &out, const RGBt &v);
};

inline std::ostream &operator<<(std::ostream &out, const RGBt &c)
{
    out << " [ r=" << c.R << " g=" << c.G << " b=" << c.B << " ] ";
    return out;
}

//shiping
inline std::istream & operator >> (std::istream &in, RGBt &v)
{
	in >>v.R>>v.G>>v.B;
	return in;
}
//shiping

// Anil [7/16/2004]
inline RGBt blend(const RGBt& src, const RGBt& dest, double alpha)
{
	return RGBt(src.R * alpha + ((1 - alpha)*dest.R),
				src.G * alpha + ((1 - alpha)*dest.G),
				src.B * alpha + ((1 - alpha)*dest.B));
}
// Anil

#endif /*RGBT_H_*/
