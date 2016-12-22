#include <cmath>
#include <iostream>
#include <fstream>
#include "march.h"
#include "perlin.h"

using namespace std;

March::March() {
}
March::~March() {}

void March::testVals(March marcher) {
	cout << "stepSize " << marcher.stepSize << endl;
	cout << "dimensions " << marcher.dimensions[0] << endl;
	cout << "dimensions " << marcher.dimensions[1] << endl;
	cout << "dimensions " << marcher.dimensions[2] << endl;
	cout << "backColor " << marcher.backColor.x << endl;
	cout << "backColor " << marcher.backColor.y << endl;
	cout << "backColor " << marcher.backColor.z << endl;
	cout << "volColor " << marcher.volColor.x << endl;
	cout << "volColor " << marcher.volColor.y << endl;
	cout << "volColor " << marcher.volColor.z << endl;
	cout << "outFile " << marcher.outFile << endl;
	cout << "resolution " << marcher.resolution[0] << endl;
	cout << "resolution " << marcher.resolution[1] << endl;
	cout << "eyePos " << marcher.eyePos.x << endl;
	cout << "eyePos " << marcher.eyePos.y << endl;
	cout << "eyePos " << marcher.eyePos.z << endl;
	cout << "camUp " << marcher.camUp.x << endl;
	cout << "camUp " << marcher.camUp.y << endl;
	cout << "camUp " << marcher.camUp.z << endl;
	cout << "camDir " << marcher.camDir.x << endl;
	cout << "camDir " << marcher.camDir.y << endl;
	cout << "camDir " << marcher.camDir.z << endl;
	for (uint i=0; i < marcher.lightPos.size(); i++) {
		cout << "Light Pos " << i << ' ' << marcher.lightPos[i].x << endl;
		cout << "Light Pos " << i << ' ' << marcher.lightPos[i].y << endl;
		cout << "Light Pos " << i << ' ' << marcher.lightPos[i].z << endl;
		cout << "Light Pos " << i << ' ' << marcher.lightPos[i].w << endl;
	}
	cout << "fov " << marcher.fov << endl;
	for (uint i=0; i < marcher.lightCol.size(); i++) {
		cout << "lightCol " << i << ' ' << marcher.lightCol[i].x << endl;
		cout << "lightCol " << i << ' ' << marcher.lightCol[i].y << endl;
		cout << "lightCol " << i << ' ' << marcher.lightCol[i].z << endl;
	}
	cout << "useAlpha " << marcher.useAlpha << endl;
	cout << "voxName " << marcher.voxName << endl;
	cout << "loadFrom " << marcher.loadFrom << endl;
	cout << "noiseAttr " << marcher.noiseAttr[0] << endl;
	cout << "noiseAttr " << marcher.noiseAttr[1] << endl;
	cout << "noiseAttr " << marcher.noiseAttr[2] << endl;
	cout << "noiseAttr " << marcher.noiseAttr[3] << endl;
	cout << "denDiv " << marcher.denDiv << endl;
	cout << "k " << marcher.k << endl;
	cout << "Center " << marcher.vCent << endl;
	cout << "voxDetail " << marcher.voxDetail << endl;
}

void March::setupCamera() {
	//E = Eye world coordinates = eyePos
	//C = viewing direction vector (normalized) = camDir
	//U = up
	//fov = horizontal 1/2 angle
	//fovv = vertical 1/2 angle
	
	//W in Vec4: 1 means position, 0 means direction (essentially a toggle for whether translations apply).
	//^^^ Homogenous Coordinates: Using 4 value vector for 3D space.
	
	//~ eyePos = Vec4(-100, 0, 360, 1); //180 is location in relation to origin.
	//~ camDir = Vec4(0, 0, -1, 0); //Pointing along -z direction.
	//~ camUp = Vec4(0, 1, 0, 0); //Pointing along +y direction; up.
	//~ fov = 45.0;
	//~ lightPos = Vec4(-100, 0, 0, 1);
	//~ lightCol = Vec3(1, 0, 0);
	
	perlin = new Perlin((int)noiseAttr[0], noiseAttr[1], noiseAttr[2], (int)noiseAttr[3]); //Setup noise object.
	
	k = 1;
	
	A = camDir.normalized().cross(camUp); //Parallel to x axis, and also view plane.
	B = A.cross(camDir.normalized()); //In our case, up and b are the same.
	midpoint = eyePos + camDir.normalized(); //Midpoint of the screen, in WORLD SPACE.
	
	float aspectRatio = (float)resolution[1] / (float)resolution[0]; //Switch 1 and 0?
	float vAspectRatio = (float)resolution[0] / (float)resolution[1]; //Switch 1 and 0?
	
	halfHori = ( A * camDir.normalized().length() * tan((fov * aspectRatio) * PI/180.0) ) / A.length();
	halfVert = ( B * camDir.normalized().length() * tan((fov * aspectRatio / vAspectRatio) * PI/180.0) ) / B.length(); //Or just square the aspect ratio.
}

Vec4 March::rayCast(int x, int y) {	
	// x and y are non-normalized pixels on the output.
	//(x / xRes) - .5 = Sx in NDC. B/C origin is at midpoint, shift
	//    .5 to the left and down.
	
	float Sx = ((float)x / (float)(resolution[0]));
	float Sy = ((float)y / (float)(resolution[1])); //From 0 to 1, left edge to right edge.
		
	Vec4 P = midpoint + ( halfHori * (2 * Sx - 1) ) + ( halfVert * (2 * Sy - 1) );
	Vec4 ray = (P - eyePos);
	//~ Vec4 ray = eyePos + (P - eyePos).normalized();
	//~ cout << "Ray: " << ray << " Vec " << (float)resolution[1] / (float)resolution[0] << " NDC x: " << (2 * Sx - 1) << " NDC y: " << (2 * Sy - 1) << " x: " << x << " y: " << y << endl;
	
	return ray.normalized();
}

void March::setupVoxels() { //Expand later.
	maxDist = eyePos.length() + sqrt(pow(dimensions[0], 2) + pow(dimensions[1], 2) + pow(dimensions[2], 2));
	
	float ***tmp = new float**[dimensions[0]]; //density.txt: x, y, then z.
	for(int i=0; i<dimensions[0]; i++){
		tmp[i] = new float*[dimensions[1]];
		for(int j =0; j<dimensions[1]; j++){
			tmp[i][j] = new float[dimensions[2]];
			for(int k = 0; k<dimensions[2]; k++){
				tmp[i][j][k] = 0;
			}
		}
	}
	
	voxels = tmp;
	
	string line;
	ifstream myfile;
	
	myfile.open(voxName);
	if (myfile.is_open()) {
		int x = 0; int y = 0; int z = 0;
		while (getline(myfile,line)) {
			voxels[x][y][z] = atof(line.c_str());
			
			if(x == dimensions[0] - 1) {
				x = 0;
				if(y == dimensions[1] - 1) {
					y = 0;
					if(z == dimensions[2] - 1) {
						break;
					} else {
						z++;
					}
				} else {
					y++;
				}
			} else {
				x++;
			}
		}
	}
}

Image March::march(Vec4 ray) {
	struct Image out;
	out.rgb = Vec3(0, 0, 0);
	if(useAlpha) out.alpha = 0;
	else out.alpha = 1;
	out.depth = 0;
	
	//~ Vec3 color = Vec3(0, 0, 0); //Starting color is black.
	Vec4 position = eyePos;
	float transmit = 1; //This is the inverted alpha.
	float dTransmit = 0;
	float tTerm = 0; //For later optimization.
	
	bool firstDepth = true;
	bool firstSpec = true; //Only run specular and depth calculations on the first non-zero density point.
	
	while((position - eyePos).length() < maxDist && transmit > 0.0001) {
		position = position + (ray * stepSize);
		float density = sample(position);
		if(density == 0) continue; //Cool optimization - avoid all that lighting bullshit :).
		
		if(pass == SPEC && firstSpec) {
			float sum = 0;
			for (uint i=0; i < lightPos.size(); i++) { //For each light
				Vec4 lDir = (lightPos[i] - position).normalized();
				float d2 = sample(position + lDir);
				if(d2 == 0) {
					float f = lDir.dot(ray);
					sum += f;
				}
			}
			firstSpec = false;
			out.rgb = Vec3(sum);
		}
		
		if(firstDepth) {
			if(transmit < .3) {
				out.depth = (position-eyePos).length();
				firstDepth = false; //Only get the depth once.
			}
			
		}
		
		if(pass == RGBAZ) {
			dTransmit = exp(-k * stepSize * density);
			transmit *= dTransmit;
			tTerm = ((1 - dTransmit) / k) * transmit; //This calculation happens once, only.
						
			for (uint i=0; i < lightPos.size(); i++) {
				float lTrans = lightTransmit(position, lightPos[i]);
				out.rgb = out.rgb + volColor.colMult(lightCol[i]) * tTerm * lTrans;
				//Color contribution from the light at the current voxel. Do for each loop and accumulate for all lights.
			}
		}
	}
	if(transmit < 0.00015) transmit = 0; //Make sure it flattens out.
	if(useAlpha) out.alpha = 1 - transmit; //Set alpha channel.
	if(!useAlpha) out.rgb = out.rgb + (backColor * transmit);
	return out;
}

float March::getDensity(Vec4 pos) {
	//Voxel Density Lookup.
	Vec3 oldCent = Vec3((dimensions[0] / 2), (dimensions[1] / 2), (dimensions[2] / 2));
	if(
		(((int)(pos.x * voxDetail) < ((int)vCent.x + oldCent.x)) && ((int)(pos.x * voxDetail) >= ((int)vCent.x - oldCent.x))) 
		&& ((int)(pos.y * voxDetail) < ((int)vCent.y + oldCent.y) && (int)(pos.y * voxDetail) >= ((int)vCent.y - oldCent.y)) 
		&& ((int)(pos.z * voxDetail) < ((int)vCent.z + oldCent.z) && (int)(pos.z * voxDetail) >= ((int)vCent.z - oldCent.z)) // Centered at vCent.
	) {
		return voxels[(int)(pos.x * voxDetail) + ((int)oldCent.x - (int)vCent.x)][(int)(pos.y * voxDetail) + ((int)oldCent.y - (int)vCent.y)][(int)(pos.z * voxDetail) + ((int)oldCent.z - (int)vCent.z)] / denDiv;
	} else {
		return 0;
	}
}

float March::sample(Vec4 pos) {
	if(loadFrom == 1) {
		Vec4 center = vCent;
		float radius = 35;
		float dist = (1 - ((pos - center) * (1 / radius)).length() ) + perlin->Get(pos.x, pos.y, pos.z);
		
		if (dist > 0) return dist;
		return 0;
	} else if (loadFrom == 0) {
		return getDensity(pos);
	}
	return 0;
}

float March::lightTransmit(Vec4 pos, Vec4 lightP) {
	//Returns the transmittence between starting pos and light.
	Vec4 position = pos; //Starts at pos.
	float transmit = 1;
	Vec4 ray = (lightP - pos).normalized();
	float lightLength = (lightP - pos).length();
	float dTransmit = 0;
	
	while(((position - pos).length() < lightLength)) {
		position = position + (ray * stepSize);
		float density = sample(position);
		if(density == 0) continue; //Cool optimization, though only for faraway light - no use updating transmit if it's just gonna be transmit *= exp(0)...
		
		dTransmit = exp(-k * stepSize * density);
		transmit *= dTransmit;
	}
	return transmit;
}


//Good casting test: Feed in x and y, get generated ray R, use its x/y/z as r/g/b.
