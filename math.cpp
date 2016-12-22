#include "math.h"

#include <cmath>
#include <algorithm>

using namespace std;

//Vec3 convention: Provide statistics, make new copy.

//Homogenous coordinates: 0 is directional, 1 is coordinate.

Vec3::Vec3() {
	
	x = 0;
	y = 0;
	z = 0;
}

Vec3::Vec3(float xIn, float yIn, float zIn) {
	
	x = xIn;
	y = yIn;
	z = zIn;
}

Vec3::Vec3(float in) {
	
	x = in;
	y = in;
	z = in;
}


Vec3::Vec3(Vec4 in, int drop) {
	switch (drop) {
		case 1 : 
			x = in.y;
			y = in.z;
			z = in.w;
		case 2 :
			x = in.x;
			y = in.z;
			z = in.w;
		case 3 :
			x = in.x;
			y = in.y;
			z = in.w;
		case 4 :
			x = in.x;
			y = in.y;
			z = in.z;
	}
}


/*Vec3::Vec3(Vec3 vecIn) {
	
	x = vecIn.x;
	y = vecIn.y;
	z = vecIn.z;
}*/

Vec3::~Vec3() {}

float Vec3::length() {
	return sqrt( x*x + y*y + z*z );
}

Vec3 Vec3::normalized() {
	
	float len = length();
	Vec3 tempVec;
	
	tempVec.x = x/len;
	tempVec.y = y/len;
	tempVec.z = z/len;
	
	return tempVec;
}

Vec3 Vec3::cross(Vec3 vecIn) {
	float xOut = (y*vecIn.z - z*vecIn.y);
	float yOut = (z*vecIn.x - x*vecIn.z);
	float zOut = (x*vecIn.y - y*vecIn.x);
	
	return Vec3(xOut, yOut, zOut);

	// Cross Product Identity: a x b = -b x a
}

float Vec3::dot(Vec3 vecIn) {
	return x * vecIn.x + y * vecIn.y + z * vecIn.z;
}

Vec3 Vec3::colMult(Vec3 colIn) {
	return Vec3(x*colIn.x, y*colIn.y, z*colIn.z);
}

float Vec3::maxDim() {
	return max(x, max(y,z));
}



Vec3 Vec3::operator+(const Vec3 vecIn) {
	float xOut = vecIn.x + x;
	float yOut = vecIn.y + y;
	float zOut = vecIn.z + z;
	
	return Vec3(xOut, yOut, zOut);
}

Vec3 Vec3::operator*(float in) {
	float xOut = in * x;
	float yOut = in * y;
	float zOut = in * z;
	
	return Vec3(xOut, yOut, zOut);
}

/* FIX LATER
Vec3 operator * (const float in, const Vec3 a)
{ 
	float xOut = in * a.x;
	float yOut = in * a.y;
	float zOut = in * a.z;
	
	return Vec3(xOut, yOut, zOut);
}
*/

ostream& operator<<(ostream& s, const Vec3& a) {
	s << a.x << ", " << a.y << ", " << a.z;
	return s;
}

Vec4::Vec4() {
	
	x = 0;
	y = 0;
	z = 0;
	w = 0;
}

Vec4::Vec4(float xIn, float yIn, float zIn, float wIn) {
	
	x = xIn;
	y = yIn;
	z = zIn;
	w = wIn;
}

Vec4::Vec4(Vec3 vecIn, float wIn) {
	
	x = vecIn.x;
	y = vecIn.y;
	z = vecIn.z;
	w = wIn;
}

Vec4::~Vec4() {}

float Vec4::length() {
	return sqrt( x*x + y*y + z*z );
}

Vec4 Vec4::normalized() {
	
	float len = length();
	Vec4 tempVec;
	
	tempVec.x = x/len;
	tempVec.y = y/len;
	tempVec.z = z/len;
	tempVec.w = w;
	
	return tempVec;
}

Vec4 Vec4::cross(Vec4 vecIn) {
	float xOut = (y*vecIn.z - z*vecIn.y);
	float yOut = (z*vecIn.x - x*vecIn.z);
	float zOut = (x*vecIn.y - y*vecIn.x);
	
	return Vec4(xOut, yOut, zOut, w);

	// Cross Product Identity: a x b = -b x a
}

float Vec4::dot(Vec4 vecIn) {
	return x * vecIn.x + y * vecIn.y + z * vecIn.z;
}

float Vec4::maxDim() {
	return max(x, max(y,z));
}

Vec4 Vec4::operator+(const Vec4 vecIn) {
	float xOut = vecIn.x + x;
	float yOut = vecIn.y + y;
	float zOut = vecIn.z + z;
	float wOut = vecIn.w + w;
	
	return Vec4(xOut, yOut, zOut, wOut);
}

Vec4 Vec4::operator-(const Vec4 vecIn) {
	float xOut = x - vecIn.x;
	float yOut = y - vecIn.y;
	float zOut = z - vecIn.z;
	float wOut = w - vecIn.w;
	
	return Vec4(xOut, yOut, zOut, wOut);
}

Vec4 Vec4::operator*(float in) {
	float xOut = in * x;
	float yOut = in * y;
	float zOut = in * z;
	float wOut = in * w;
	
	return Vec4(xOut, yOut, zOut, wOut);
}

Vec4 Vec4::operator/(float in) {
	float xOut = x / in;
	float yOut = y / in;
	float zOut = z / in;
	float wOut = w / in;
	
	return Vec4(xOut, yOut, zOut, wOut);
}

ostream& operator<<(ostream& s, const Vec4& a) {
	s << a.x << ", " << a.y << ", " << a.z << ": " << a.w;
	return s;
}



Mat4::Mat4() {
	r1 = Vec4(1, 0, 0 ,0);
	r2 = Vec4(0, 1, 0, 0);
	r3 = Vec4(0, 0, 1, 0);
	r4 = Vec4(0, 0, 0, 1);
}

Mat4::Mat4(Vec4 r1In, Vec4 r2In, Vec4 r3In, Vec4 r4In) {
	r1 = r1In;
	r2 = r2In;
	r3 = r3In;
	r4 = r4In;
}

Mat4::Mat4(float rIn) {
	r1 = Vec4(rIn, 0, 0 ,0);
	r2 = Vec4(0, rIn, 0, 0);
	r3 = Vec4(0, 0, rIn, 0);
	r4 = Vec4(0, 0, 0, 1);
}

Mat4::~Mat4() {}

Mat4 Mat4::transpose() {
	Vec4 nR1 = Vec4(r1.x, r2.x, r3.x, r4.x);
	Vec4 nR2 = Vec4(r1.y, r2.y, r3.y, r4.y);
	Vec4 nR3 = Vec4(r1.z, r2.z, r3.z, r4.z);
	Vec4 nR4 = Vec4(r1.w, r2.w, r3.w, r4.w);
	
	return Mat4(nR1, nR2, nR3, nR4);
}


Mat4 Mat4::operator+(Mat4 matIn) {
	Vec4 r1Out = matIn.r1 + r1;
	Vec4 r2Out = matIn.r2 + r2;
	Vec4 r3Out = matIn.r3 + r3;
	Vec4 r4Out = matIn.r4 + r4;
	
	return Mat4(r1Out, r2Out, r3Out, r4Out);
}

Vec4 Mat4::operator*(Vec4 in) {
	float xOut = in.dot(r1);
	float yOut = in.dot(r2);
	float zOut = in.dot(r3);
	float wOut = in.dot(r4);
	
	return Vec4(xOut, yOut, zOut, wOut);
}

/*Mat4 Mat4::operator*(Mat4 in) {	
	Vec4 xOut = this * in.r1;
	Vec4 yOut = this * in.r2;
	Vec4 zOut = this * in.r3;
	Vec4 wOut = this * in.r4;
	
	return Mat4(xOut, yOut, zOut, wOut);
}
* */
