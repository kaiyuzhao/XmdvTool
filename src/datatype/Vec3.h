#ifndef VEC3_H_
#define VEC3_H_

#include <math.h>
#include <iostream>

using namespace std ;

class Vec3
{
    public:
        Vec3(double x=0, double y=0, double z=0) :
            X(x), Y(y), Z(z)
        {}

        friend Vec3 operator-(const Vec3 &v1, const Vec3 &v2);
        friend Vec3 operator+(const Vec3 &v1, const Vec3 &v2);
		//shiping
		friend Vec3 interpolate(const Vec3 &v1, const Vec3 &v2, double factor);
		//shiping
        friend double operator*( const Vec3 & v1, const Vec3& v2);
        friend Vec3 operator*( double x, const Vec3& v2);

        friend ostream &operator<<(ostream &out, const Vec3& v);

        double norm() const;
        void normalize();
        void set(double x, double y, double z) { X=x; Y=y; Z=z;}
		void set(const Vec3& vec) {X=vec.X; Y=vec.Y; Z=vec.Z;}
		Vec3 cross (const Vec3 &v2) const;
		double dot(const Vec3 &v) const;
		Vec3 operator = (const Vec3& v)
		{
			this->X = v.X;
			this->Y = v.Y;
			this->Z = v.Z;
			return *this;
		}

		double z_angle() const
		{
			Vec3 vec(X,Y,Z);
			vec.normalize();
			double angle = acos(vec.Z);

			double zero = 1.0e-6;
			if (fabs(vec.Z) < zero){
				angle = acos( vec.X );	// rotation angle in x-y plane
				if ( vec.Y <= 0.0 ) angle = -angle;
			}
			else angle =(vec.Z>0.0)?angle:-angle;

			return angle;
		}
		double y_angle() const
		{
			Vec3 vec(X,Y,Z);
			vec.normalize();
			double angle = acos(vec.Y/sqrt(vec.X*vec.X + vec.Y*vec.Y));
			return ((vec.Y>0.0)?angle:(-angle));
		}
		double x_angle() const
		{
			Vec3 vec(X,Y,Z);
			vec.normalize();
			double angle = acos(vec.X/sqrt(vec.X*vec.X + vec.Y*vec.Y));
			return ((vec.X>0.0)?angle:(-angle));
		}

    public:
        double X,Y,Z;
};


inline Vec3 Vec3::cross (const Vec3 &v2) const{
	Vec3 v = Vec3(
				Y * v2.Z - Z * v2.Y,
				Z * v2.X - X* v2.Z,
				X * v2.Y - Y * v2.X);
	return v;
}

inline double Vec3::dot(const Vec3& v) const {
	return (X*v.X + Y*v.Y +Z*v.Z);
}

inline Vec3 operator- (const Vec3 &v1, const Vec3 &v2 )
{
    return Vec3(v1.X - v2.X, v1.Y - v2.Y, v1.Z - v2.Z );
}

inline Vec3 operator+ (const Vec3 &v1, const Vec3 &v2 )
{
    return Vec3(v1.X + v2.X, v1.Y + v2.Y, v1.Z +v2.Z );
}

inline ostream &operator<<(ostream &out, const Vec3 &v)
{
    out << " [ " << v.X << " " << v.Y <<v.Z<< " ] ";
    return out;
}

inline double Vec3::norm() const
{
    return sqrt(X*X + Y*Y + Z*Z);
}

inline void Vec3::normalize()
{
    double mag = norm();
    X /= mag;
    Y /= mag;
	Z /= mag;
}

inline double operator*( const Vec3 &v1, const Vec3 &v2)
{
    return v1.X*v2.X + v1.Y*v2.Y + v1.Z*v2.Z;
}

inline Vec3 operator*( double x, const Vec3 &v2)
{
    return Vec3( x*v2.X, x*v2.Y, x*v2.Z );
}

//shiping
inline Vec3 interpolate(const Vec3 &v1, const Vec3 &v2, double factor)
{
	return Vec3(v1.X + factor*(v2.X - v1.X), v1.Y + factor*(v2.Y - v1.Y), v1.Z + factor*(v2.Z - v1.Z));
}
//shiping

#endif /*VEC3_H_*/
