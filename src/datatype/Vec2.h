#ifndef VEC2_H_
#define VEC2_H_

#include <math.h>
#include <iostream>
using namespace std;

class Vec2
{
    public:
        Vec2(double x=0, double y=0) :
            X(x), Y(y)
        {}

        double operator[](int i) const { return ((double*)&X)[i]; }
        double &operator[](int i) { return ((double*)&X)[i]; }
        friend Vec2 operator-(const Vec2 &v1, const Vec2 &v2);
        friend Vec2 operator+(const Vec2 &v1, const Vec2 &v2);
        friend double operator*( const Vec2 & v1, const Vec2& v2);
        friend Vec2 operator*( double x, const Vec2& v2);

        friend std::ostream &operator<<(std::ostream &out, const Vec2& v);

        double norm() const;
        void normalize();
        void set(double x,double y) { X=x; Y=y; }

		//Zaixian Data Quality Start
		friend Vec2 interpolate(const Vec2 &v1, const Vec2 &v2, double factor);
		bool isWithinBox(const Vec2 &v1, const Vec2 &v2);
		//Zaixian Data Quality End
    public:
        double X,Y;
};

inline Vec2 operator- (const Vec2 &v1, const Vec2 &v2 )
{
    return Vec2(v1.X - v2.X, v1.Y - v2.Y );
}

inline Vec2 operator+ (const Vec2 &v1, const Vec2 &v2 )
{
    return Vec2(v1.X + v2.X, v1.Y + v2.Y );
}

inline std::ostream &operator<<(std::ostream &out, const Vec2 &v)
{
    out << " [ " << v.X << " " << v.Y << " ] ";
    return out;
}

inline double Vec2::norm() const
{
    return sqrt(X*X + Y*Y);
}

inline void Vec2::normalize()
{
    double mag = norm();
    X /= mag;
    Y /= mag;
}

inline double operator*( const Vec2 &v1, const Vec2 &v2)
{
    return v1.X*v2.X + v1.Y*v2.Y;
}

inline Vec2 operator*( double x, const Vec2 &v2)
{
    return Vec2( x*v2.X, x*v2.Y );
}

//Zaixian Data Quality Start
inline Vec2 interpolate(const Vec2 &v1, const Vec2 &v2, double factor){
	return Vec2(v1.X + factor*(v2.X - v1.X), v1.Y + factor*(v2.Y - v1.Y));
}

inline bool Vec2::isWithinBox(const Vec2 &v1, const Vec2 &v2)
{
	return (X>=v1.X && X<=v2.X && Y>=v1.Y && Y<=v2.Y);
}

//Zaixian Data Quality End

#endif /*VEC2_H_*/
