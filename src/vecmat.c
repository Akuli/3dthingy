#include "vecmat.h"
#include <math.h>

struct Vec3 vec3_add(struct Vec3 a, struct Vec3 b)
{
	return (struct Vec3){ a.x + b.x, a.y + b.y, a.z + b.z };
}

struct Vec3 vec3_add3(struct Vec3 a, struct Vec3 b, struct Vec3 c)
{
	return vec3_add(a, vec3_add(b, c));
}

struct Vec3 vec3_neg(struct Vec3 a)
{
	return (struct Vec3){ -a.x, -a.y, -a.z };
}

struct Vec3 vec3_sub(struct Vec3 a, struct Vec3 b)
{
	return vec3_add(a, vec3_neg(b));
}

double vec3_dot(struct Vec3 a, struct Vec3 b)
{
	return a.x*b.x + a.y*b.y + a.z*b.z;
}

double vec3_len(struct Vec3 v)
{
	return sqrt(vec3_dot(v, v));
}

struct Vec3 vec3_mul_scalar(struct Vec3 v, double c)
{
	return (struct Vec3){v.x*c, v.y*c, v.z*c};
}

struct Vec3 vec3_projection(struct Vec3 v, struct Vec3 onto)
{
	return vec3_mul_scalar(onto, vec3_dot(v, onto) / vec3_dot(onto, onto));
}

struct Vec3 vec3_unit(struct Vec3 v)
{
	return vec3_mul_scalar(v, 1.0/vec3_len(v));
}

struct Vec3 vec3_cross(struct Vec3 v, struct Vec3 w)
{
	return (struct Vec3){
		v.y*w.z - v.z*w.y,
		v.z*w.x - v.x*w.z,
		v.x*w.y - v.y*w.x
	};
}


struct Mat3 mat3_rotation_xz(double w)
{
	// exercise for you: derive this formula with intuition of sin,cos of small angles
	return (struct Mat3){
		cos(w),  0, sin(w),
		0,       1, 0,
		-sin(w), 0, cos(w),
	};
}

struct Mat3 mat3_mul_scalar(struct Mat3 m, double c)
{
	return (struct Mat3){
		m.a*c, m.b*c, m.c*c,
		m.d*c, m.e*c, m.f*c,
		m.g*c, m.h*c, m.i*c,
	};
}

struct Vec3 mat3_mul_vec3(struct Mat3 m, struct Vec3 v)
{
	struct Vec3 xcol = { m.a, m.d, m.g };
	struct Vec3 ycol = { m.b, m.e, m.h };
	struct Vec3 zcol = { m.c, m.f, m.i };
	return vec3_add3(
		vec3_mul_scalar(xcol, v.x),
		vec3_mul_scalar(ycol, v.y),
		vec3_mul_scalar(zcol, v.z));
}

double mat3_det(struct Mat3 m)
{
	// https://ardoris.wordpress.com/2008/07/18/general-formula-for-the-inverse-of-a-3x3-matrix/
	double
		a=m.a, b=m.b, c=m.c,
		d=m.d, e=m.e, f=m.f,
		g=m.g, h=m.h, i=m.i;
	return a*(e*i - f*h) - b*(d*i - f*g) + c*(d*h - e*g);
}

struct Mat3 mat3_inverse(struct Mat3 m)
{
	// https://ardoris.wordpress.com/2008/07/18/general-formula-for-the-inverse-of-a-3x3-matrix/
	double
		a=m.a, b=m.b, c=m.c,
		d=m.d, e=m.e, f=m.f,
		g=m.g, h=m.h, i=m.i;

	return mat3_mul_scalar(
		(struct Mat3){
			e*i-f*h, c*h-b*i, f*b-c*e,
			f*g-d*i, a*i-c*g, c*d-a*f,
			d*h-e*g, b*g-a*h, a*e-b*d,
		},
		1.0/mat3_det(m)
	);
}
