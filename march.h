#ifndef MARCH
#define MARCH

#include <iostream>
#include <string>
#include <vector>
#include "math.h"
#include "perlin.h"

using namespace std;


enum Passes {RGBAZ, SPEC,};

struct Image {
	Vec3 rgb;
	float alpha; //0..1 range
	float depth; //Will go > 1.
	float spec; //Direct bw reflection.
};

class March {
	public:
		March();
		~March();
		
		static void testVals(March);
		
		Vec4 A;
		Vec4 B;
		Vec4 H;
		Vec4 V;
		
		Vec4 halfHori;
		Vec4 halfVert;
		Vec4 midpoint;
		
		float stepSize; //Step size
		int dimensions[3]; //Dimensions of voxelbuffer.
		float voxDetail;
		Vec3 backColor; //Background color.
		Vec3 volColor; //Color of Volume
		string outFile; //Destination filename.
		int resolution[2]; //Final resolution of image.
		Vec4 eyePos; //Position of the vcamera in space.
		Vec4 camDir; //Direction of camera in space.
		Vec4 camUp; //Up vector for camera.
		float fov; //Field of view
		vector<Vec4> lightPos; //Light positions.
		vector<Vec3> lightCol; //Light colors.
		//~ Vec4 lightPos; //Light position.
		//~ Vec3 lightCol; //Light color.
		float k; //Attenuation.
		
		void setupCamera();
		Vec4 rayCast(int, int);
		
		float maxDist;
		
		//Noise Stuff
		Perlin* perlin;
		
		float ***voxels;
		void setupVoxels();
		Image march(Vec4);
		float getDensity(Vec4);
		float sample(Vec4 pos); //0: File, 1: Perlin
		float lightTransmit(Vec4, Vec4);
		
		Passes pass;
		
		int startFrame; //First frame
		int stopFrame; //Last frame
		string pref; //Prefix for config.
		
		bool useAlpha; //Whether or not to use config.
		string voxName; //Name of voxel file to load.
		int loadFrom; //Int of what to load from. 0: File, 1: Perlin
		float noiseAttr[4]; //Array of noise attributes.
		float denDiv; //Density divisor. Weird that I need this...
		Vec4 vCent; //Center of whichever mechanism we're using.
};

#endif
