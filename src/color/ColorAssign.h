#if !defined COLORASSIGN
#define COLORASSIGN

#include <math.h>
#include "datatype/RGBt.h"

#define PI2	(3.1415926535897931)

class colorassign
{
public:
	unsigned long	color;
	char			colorprefer[100];

	colorassign()
	{
		color = 0x000000; colorprefer[0] = '\0';
	}
};


///
/// Represents a color space.
///
/// \date 12/3/2003
/// \author Anilkumar Patro
///
class HSIColor
{
public:		// methods

	// LIFECYCLE

	///
	/// Constructor taking Hue, Sat and Intensity
	///
	HSIColor(double H, double S, double I, double A = 1.0)
	{
		mRed = 0.0;
		mGreen = 0.0;
		mBlue = 0.0;
		mAlpha = A;
		if (H < 6.0) {
			mHue = H;
		} else {
			mHue = H - 6.0;
		}
		mSaturation = S;
		mIntensity = I;
	}

	///
	/// Copy Constructor
	///
	/// \param color the value to assign to this object
	///
	HSIColor(const HSIColor &color)
	{
		mRed = 0.0;
		mGreen = 0.0;
		mBlue = 0.0;
		mAlpha = color.mAlpha;
		mHue = color.mHue;
		mSaturation = color.mSaturation;
		mIntensity = color.mIntensity;
	}


	///
	/// Constructor taking an rgb value as integer (0x00rrggbb)
	///
	/// \param rgb
	///
	HSIColor(int rgb)
	{
		mRed = rgb >> 16 & 0xff;
		mGreen = rgb >> 8 & 0xff;
		mBlue = rgb >> 0 & 0xff;
		mAlpha = rgb >> 24 & 0xff;
		double	Mid = (mRed + mGreen + mBlue) / 3.0;
		double	Mr = mRed - Mid;
		double	Mg = mGreen - Mid;
		double	Mb = mBlue - Mid;
		double	Mult = Mr *Mr + Mg *Mg + Mb *Mb;
		double	CosHue = (2.0 * Mr - Mg - Mb) / sqrt(Mult * 6.0);
		mIntensity = Mid + sqrt((2.0 * Mult) / 3.0);
		mSaturation = (2.0 * (mIntensity - Mid)) / mIntensity;
		mHue = (acos(CosHue) * 3.0) / PI2;
		if (mBlue > mGreen) {
			mHue = 6.0 - mHue;
		}
	}

	// OPERATIONS

	///
	/// Convert HSI to RGB
	///
	/// \return RGB converted value
	/// \pre
	/// \post
	///
	int getRGB()
	{
		int	RGB;
		if (mSaturation == 0.0) {
			RGB = HSIColor::toRGB(mIntensity*255.0, mIntensity*255.0,
								  mIntensity*255.0, mAlpha*255.0);
		} else {
			double	dRed = value(mHue + 0.0) * 255.0;
			double	dGreen = value(mHue + 4.0) * 255.0;
			double	dBlue = value(mHue + 2.0) * 255.0;
			double  dAlpha = mAlpha * 255.0;
			RGB = HSIColor::toRGB(dRed, dGreen, dBlue, dAlpha);
		}
		return RGB;
	}

protected:	// methods

	static int toRGB(double r, double g, double b, double a = 255.0)
	{
		return ((((int) r & 0xff) << 16) |
				(((int) g & 0xff) << 8) |
				(((int) b & 0xff) << 0) |
				(((int) a & 0xff) << 24));
	}

	double value(double dHuePhase)
	{
		double	dPure = 0.5 * ((double) 1 + cos((dHuePhase *PI2) / 3.0));
		return mIntensity * (1.0 - mSaturation * (1.0 - dPure));
	}

public:		// members
	double	mHue;
	double	mSaturation;
	double	mIntensity;

protected:	// members
	double	mRed;
	double	mGreen;
	double	mBlue;
	double	mAlpha;
};


class HSVcolor
{
public:
	double	h, s, v;
public:
	HSVcolor()
	{
		h = 0.0; s = 0.0; v = 0.0;
	}
	HSVcolor(const unsigned long &c)
	{
		h = (c & 0xFF) / 255.;
		s = ((c >> 8) & 0xFF) / 255.;
		v = ((c >> 16) & 0xFF) / 255.;
	}

	void	toRGB(RGBt &rgb);
	void	formalize();
};

inline void HSVcolor::formalize()
{
	h /= (1.0 / 0xFF) * 360.0 / 60.0;
	s /= 1.0 / 0xFF;
	v /= 1.0 / 0xFF;
}

inline void HSVcolor::toRGB(RGBt &rgb)
{
	formalize();

	double m, n, f;
	int i;

	if (h == -1) {
		rgb.R = rgb.G = rgb.B = v;
	}

	i = (int) (h + 0.5);
	f = h - i;
	if (!(i & 1)) {
		f = 1 - f;
	} // if i is even
	m = v * (1 - s);
	n = v * (1 - s * f);
	switch (i) {
	case 6:
		break;
	case 0:
		rgb.R = v;
		rgb.G = n;
		rgb.B = m;
		break;
	case 1:
		rgb.R = v;
		rgb.G = n;
		rgb.B = m;
		break;
	case 2:
		rgb.R = m;
		rgb.G = v;
		rgb.B = n;
		break;
	case 3:
		rgb.R = m;
		rgb.G = n;
		rgb.B = v;
		break;
	case 4:
		rgb.R = n;
		rgb.G = m;
		rgb.B = v;
		break;
	case 5:
		rgb.R = v;
		rgb.G = m;
		rgb.B = n;
		break;
	}

	rgb.R *= 0xFF;
	rgb.G *= 0xFF;
	rgb.B *= 0xFF;

	return;
}

/*
 * hue [0,360], sat [0,1], lum [0,1]
 * R, G, B [0,1]
 */
#define undefined -1

class HLScolor
{
public:
	double	hue, sat, lum;
	double	max_of(double, double, double);
	double	min_of(double, double, double);
	double	rgb_func(double, double, double);
public:
	HLScolor()
	{
		hue = 0.0; sat = 0.0; lum = 0.0;
	}
			HLScolor(double, double, double);
			HLScolor(unsigned long);
	void	toRGB(RGBt &rgb);
	void	toUnsignedLong(unsigned long &);
	void	fromRGB(const RGBt &rgb);
};



inline double HLScolor::max_of(double r, double g, double b)
{
	double maxvalue;
	if (r > g) {
		maxvalue = r;
	} else {
		maxvalue = g;
	}
	if (b > maxvalue) {
		maxvalue = b;
	}
	return maxvalue;
}

inline double HLScolor::min_of(double r, double g, double b)
{
	double minvalue;
	if (r < g) {
		minvalue = r;
	} else {
		minvalue = g;
	}
	if (b < minvalue) {
		minvalue = b;
	}
	return minvalue;
}


inline double HLScolor::rgb_func(double q1, double q2, double hue)
{
	if (hue > 360) {
		hue = hue - 360;
	}
	if (hue < 0) {
		hue = hue + 360;
	}
	if (hue < 60) {
		return (q1 + (q2 - q1) * hue / 60);
	} else if (hue < 180) {
		return q2;
	} else if (hue < 240) {
		return (q1 + (q2 - q1) * (240 - hue) / 60);
	} else {
		return q1;
	}
}


inline HLScolor::HLScolor(double h, double s, double l)
{
	hue = h;
	sat = s;
	lum = l;
}


inline HLScolor::HLScolor(unsigned long c)
{
	RGBt rgb_color(c);
	fromRGB(rgb_color);
}

inline void HLScolor::toRGB(RGBt &rgb)
{
	double p1, p2;
	if (lum <= 0.5) {
		p2 = lum * (1 + sat);
	} else {
		p2 = lum + sat - (lum * sat);
	}
	p1 = 2 * lum - p2;
	if (sat == 0.0) {
		rgb.R = lum;
		rgb.G = lum;
		rgb.B = lum;
	} else {
		rgb.R = rgb_func(p1, p2, hue + 120);
		rgb.G = rgb_func(p1, p2, hue);
		rgb.B = rgb_func(p1, p2, hue - 120);
	}

	return;
}


inline void HLScolor::toUnsignedLong(unsigned long &c)
{
	RGBt rgb_color;
	toRGB(rgb_color);
	unsigned R = (unsigned) (rgb_color.R * 0xff);
	unsigned G = (unsigned) (rgb_color.G * 0xff);
	unsigned B = (unsigned) (rgb_color.B * 0xff);
	c = ((R >> 4) & 0xf) * 0xfffff +
		(R & 0xf) * 0xffff +
		((G >> 4) & 0xf) * 0xfff +
		(G & 0xf) * 0xff +
		((B >> 4) & 0xf) * 0xf +
		(B & 0xf);
	return;
}

inline void HLScolor::fromRGB(const RGBt &rgb)
{
	double R = rgb.R ;
	double G = rgb.G ;
	double B = rgb.B ;
	double max_value, min_value, diff, r_dist, g_dist, b_dist;

	max_value = max_of(R, G, B);
	min_value = min_of(R, G, B);
	diff = max_value - min_value;

	lum = (max_value + min_value) / 2;

	double small_value = 0.0000001;
	if (fabs(diff) == small_value) {
		hue = undefined;
		sat = 0.0;
	} else {
		if (lum < 0.5)
			sat = diff / (max_value + min_value);
		else
			sat = diff / (2 - max_value - min_value);

		r_dist = (max_value - R) / diff;
		g_dist = (max_value - G) / diff;
		b_dist = (max_value - B) / diff;
		if (R == max_value)
			hue = b_dist - g_dist;
		else if (G == max_value)
			hue = 2 + r_dist - b_dist;
		else if (B == max_value)
			hue = 4 + g_dist - r_dist;
		hue = hue * 60;
		if (hue < 0)
			hue = hue + 360;
	}
	return;
}

#endif // COLORASSIGN
