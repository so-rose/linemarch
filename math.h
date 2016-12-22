#ifndef MATH
#define MATH

#define PI 3.14159265

#include <ostream>
class Vec4;
class Vec3;
class Mat4;

class Vec3 {
	public:
		Vec3();
		Vec3(float);
		Vec3(float, float, float);
		Vec3(Vec4, int);
//		Vec3(Vec3); //Deep copy with constructors.
		~Vec3();
		
		
		float x,y,z;
		
		float length();
		Vec3 normalized();
		float dot(Vec3);
		float maxDim();
		Vec3 cross(Vec3);
		
		Vec3 colMult(Vec3);
		
		Vec3 operator+(const Vec3);
//		friend Vec3 operator*(const float in, const Vec3 a);
		Vec3 operator*(float);
		friend std::ostream& operator<<(std::ostream& s, const Vec3& a);
};

class Vec4 {
	public:
		Vec4();
		Vec4(float, float, float, float);
		Vec4(Vec3, float);
		~Vec4();
		
		float x,y,z,w;
		
		float length();
		Vec4 normalized();
		Vec4 cross(Vec4);
		float dot(Vec4);
		float maxDim();
		
		Vec4 operator+(Vec4);
		Vec4 operator-(Vec4);
		Vec4 operator*(float);
		Vec4 operator/(float);
		friend std::ostream& operator<<(std::ostream& s, const Vec4& a);
};

class Mat4 {
	public:
		Mat4();
		Mat4(Vec4, Vec4, Vec4, Vec4);
		Mat4(float);
//		Mat4(Vec4*4)
		~Mat4();
		
		Vec4 r1, r2, r3, r4;
		
		//Identity Matrix: Multiply by this to get the same thing you put in.
		Mat4 transpose();
		
		Mat4 operator+(Mat4);
		Vec4 operator*(Vec4);
		//Mat4 operator*(Mat4);
		
};

#endif
