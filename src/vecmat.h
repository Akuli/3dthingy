// vectors and matrices (currently 3d only)

#ifndef VECMAT_H
#define VECMAT_H

struct Vec3 {
	double x;   // more means right
	double y;   // more means up
	double z;   // more means back, less means forward
};

struct Vec3 vec3_add(struct Vec3 a, struct Vec3 b);
struct Vec3 vec3_add3(struct Vec3 a, struct Vec3 b, struct Vec3 c);
struct Vec3 vec3_neg(struct Vec3 a);
struct Vec3 vec3_sub(struct Vec3 a, struct Vec3 b);
struct Vec3 vec3_mul_scalar(struct Vec3 v, double c);
struct Vec3 vec3_unit(struct Vec3 v);
struct Vec3 vec3_cross(struct Vec3 v, struct Vec3 w);
struct Vec3 vec3_projection(struct Vec3 v, struct Vec3 onto);

double vec3_dot(struct Vec3 a, struct Vec3 b);
double vec3_len(struct Vec3 v);


struct Mat3 {
	double
		a, b, c,
		d, e, f,
		g, h, i;
};

// returned matrix rotates in xz plane clockwise by an angle w (viewed from above)
struct Mat3 mat3_rotation_xz(double w);

struct Mat3 mat3_mul_scalar(struct Mat3 m, double c);
struct Vec3 mat3_mul_vec3(struct Mat3 m, struct Vec3 v);
double mat3_det(struct Mat3 m);
struct Mat3 mat3_inverse(struct Mat3 m);

#endif    // VECMAT_H
