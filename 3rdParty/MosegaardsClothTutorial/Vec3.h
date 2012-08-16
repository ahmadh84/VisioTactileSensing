/**
 * @file Vec3.h
 * @author Can Erdogan
 * @date Aug 14, 2012
 * @brief
 */

#pragma once

#include <math.h>

class Vec3 // a minimal vector class of 3 floats and overloaded math operators
{
public:
	float f[3];

	Vec3(float x, float y, float z)
	{
		f[0] =x;
		f[1] =y;
		f[2] =z;
	}

	Vec3() {}

	float length()
	{
		return sqrt(f[0]*f[0]+f[1]*f[1]+f[2]*f[2]);
	}

	Vec3 normalized()
	{
		float l = length();
		return Vec3(f[0]/l,f[1]/l,f[2]/l);
	}

	void operator+= (const Vec3 &v)
	{
		f[0]+=v.f[0];
		f[1]+=v.f[1];
		f[2]+=v.f[2];
	}

	Vec3 operator/ (const float &a)
	{
		return Vec3(f[0]/a,f[1]/a,f[2]/a);
	}

	Vec3 operator- (const Vec3 &v)
	{
		return Vec3(f[0]-v.f[0],f[1]-v.f[1],f[2]-v.f[2]);
	}

	Vec3 operator+ (const Vec3 &v)
	{
		return Vec3(f[0]+v.f[0],f[1]+v.f[1],f[2]+v.f[2]);
	}

	Vec3 operator* (const float &a)
	{
		return Vec3(f[0]*a,f[1]*a,f[2]*a);
	}

	Vec3 operator-()
	{
		return Vec3(-f[0],-f[1],-f[2]);
	}

	Vec3 cross(const Vec3 &v)
	{
		return Vec3(f[1]*v.f[2] - f[2]*v.f[1], f[2]*v.f[0] - f[0]*v.f[2], f[0]*v.f[1] - f[1]*v.f[0]);
	}

	float dot(const Vec3 &v)
	{
		return f[0]*v.f[0] + f[1]*v.f[1] + f[2]*v.f[2];
	}
};


