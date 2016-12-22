#include "reader.h"

#include <iostream>
#include <string>
#include <fstream>
#include <stdlib.h> 

using namespace std;

Reader::Reader(March* m) {
	marcher = m;
}

Reader::~Reader() {}

int Reader::read(string config) {
	string line;
	ifstream myfile;
	
	myfile.open(config);
		
	if (myfile.is_open()) {
		while (getline(myfile,line)) {
			if(line.substr(0, 1) == "#") continue;
			line = line.substr(0, line.find("#")-1);
			string lineID = line.substr(0, line.find(" "));
			
			if(lineID == "STEP") {
				marcher->stepSize = atof( line.substr(5,string::npos).c_str() );
				//c_str makes it into a C style character array.
				
			} if(lineID == "VOXD") {
				marcher->voxDetail = atof( line.substr(5,string::npos).c_str() );
				//c_str makes it into a C style character array.
				
			} else if(lineID == "STAR") {
				marcher->startFrame = atoi( line.substr(5,string::npos).c_str() );
			} else if(lineID == "STOP") {
				string stuff (" \t\f\v\n\r");
				marcher->stopFrame = atoi( line.substr(5,line.find_last_not_of(stuff)).c_str() );
			} else if(lineID == "KATT") {
				marcher->k = atof( line.substr(5,string::npos).c_str() );
			} else if(lineID == "DENS") {
				marcher->denDiv = atof( line.substr(5,string::npos).c_str() );
			} else if(lineID == "PREF") {
				string stuff (" \t\f\v\n\r");
				marcher->pref = line.substr(5,line.find_last_not_of(stuff));
			} else if(lineID == "LOFR") {
				marcher->loadFrom = atoi( line.substr(5,string::npos).c_str() );
			} else if(lineID == "ALPH") {
				string stuff (" \t\f\v\n\r");
				string val = line.substr(5,line.find_last_not_of(stuff));
				
				if (val == "true") marcher->useAlpha = true;
				else marcher->useAlpha = false;
			} else if(lineID == "DIMS") {
				string seq = line.substr(5,std::string::npos);
				int ind = 0;
				for(int i=0; i < 3; i++) {
					if(!(seq.find(" ", ind+1) == string::npos)) {
						marcher->dimensions[i] = atof( seq.substr(ind, seq.find(" ", ind+1)).c_str() );
						ind = seq.find(" ", ind+1);
					} else {
						marcher->dimensions[i] = atof( seq.substr(ind, string::npos).c_str() );
					}
				}
					
			} else if(lineID == "BRGB") { 
				string seq = line.substr(5,std::string::npos);
				int ind = 0;
				marcher->backColor.x = atof( seq.substr(ind, seq.find(" ", ind)).c_str() );
				ind = seq.find(" ", ind+1);
				marcher->backColor.y = atof( seq.substr(ind, seq.find(" ", ind+1)).c_str() );
				ind = seq.find(" ", ind+1);
				marcher->backColor.z = atof( seq.substr(ind, seq.find(" ", ind+1)).c_str() );
				
			} else if(lineID == "PNOI") { 
				string seq = line.substr(5,std::string::npos);
				int ind = 0;
				marcher->noiseAttr[0] = atof( seq.substr(ind, seq.find(" ", ind)).c_str() );
				ind = seq.find(" ", ind+1);
				marcher->noiseAttr[1] = atof( seq.substr(ind, seq.find(" ", ind+1)).c_str() );
				ind = seq.find(" ", ind+1);
				marcher->noiseAttr[2] = atof( seq.substr(ind, seq.find(" ", ind+1)).c_str() );
				ind = seq.find(" ", ind+1);
				marcher->noiseAttr[3] = atof( seq.substr(ind, seq.find(" ", ind+1)).c_str() );
				
			} else if(lineID == "MRGB") {
				string seq = line.substr(5,std::string::npos);
				int ind = 0;
				marcher->volColor.x = atof( seq.substr(ind, seq.find(" ", ind)).c_str() );
				ind = seq.find(" ", ind+1);
				marcher->volColor.y = atof( seq.substr(ind, seq.find(" ", ind+1)).c_str() );
				ind = seq.find(" ", ind+1);
				marcher->volColor.z = atof( seq.substr(ind, seq.find(" ", ind+1)).c_str() );
				
			} else if(lineID == "FILE") {
				string stuff (" \t\f\v\n\r");
				marcher->outFile = line.substr(5,line.find_last_not_of(stuff));
				
			} else if(lineID == "VOXN") {
				string stuff (" \t\f\v\n\r");
				marcher->voxName = line.substr(5,line.find_last_not_of(stuff));
				
			} else if(lineID == "RESO") {
				string seq = line.substr(5,std::string::npos);
				int ind = 0;
				for(int i=0; i < 2; i++) {
					marcher->resolution[i] = atoi( seq.substr(ind, seq.find(" ", ind+1)).c_str() );
					ind = seq.find(" ", ind);
				}
				
			} else if(lineID == "CENT") { //Middle Broken
				string seq = line.substr(5,std::string::npos);
				int ind = 0;
				
				marcher->vCent.x = atof( seq.substr(ind, seq.find(" ", ind)).c_str() );
				ind = seq.find(" ", ind+1);
				
				marcher->vCent.y = atof( seq.substr(ind, seq.find(" ", ind+1)).c_str() );
				ind = seq.find(" ", ind+1);
				
				marcher->vCent.z = atof( seq.substr(ind, seq.find(" ", ind+1)).c_str() );

				marcher->vCent.w = 1;
				
			} else if(lineID == "EYEP") { //Last Broken
				string seq = line.substr(5,std::string::npos);
				int ind = 0;
				
				marcher->eyePos.x = atof( seq.substr(ind, seq.find(" ", ind)).c_str() );
				ind = seq.find(" ", ind+1);
				
				marcher->eyePos.y = atof( seq.substr(ind, seq.find(" ", ind+1)).c_str() );
				ind = seq.find(" ", ind+1);
				
				marcher->eyePos.z = atof( seq.substr(ind, seq.find(" ", ind+1)).c_str() );
				ind = seq.find(" ", ind+1);
				
				marcher->eyePos.w = 1;
				
			} else if(lineID == "VDIR") { //-1 is nowhere...
				string seq = line.substr(5,std::string::npos);
				int ind = 0;
				
				marcher->camDir.x = atof( seq.substr(ind, seq.find(" ", ind)).c_str() );
				ind = seq.find(" ", ind+1);
				
				marcher->camDir.y = atof( seq.substr(ind, seq.find(" ", ind+1)).c_str() );
				ind = seq.find(" ", ind+1);
				
				marcher->camDir.z = atof( seq.substr(ind, std::string::npos).c_str() );
				
				marcher->camDir.w = 1;
				
			} else if(lineID == "UVEC") { //Middle Broken
				string seq = line.substr(5,std::string::npos);
				int ind = 0;
				
				marcher->camUp.x = atof( seq.substr(ind, seq.find(" ", ind)).c_str() );
				ind = seq.find(" ", ind+1);
				
				marcher->camUp.y = atof( seq.substr(ind, seq.find(" ", ind+1)).c_str() );
				ind = seq.find(" ", ind+1);
				
				marcher->camUp.z = atof( seq.substr(ind, seq.find(" ", ind+1)).c_str() );

				marcher->camUp.w = 1;
				
			} else if(lineID == "FOVY") { //All good.
				marcher->fov = atof( line.substr(5,std::string::npos).c_str() );
				
			} else if(lineID == "LPOS") { //All good :)
				Vec4 lPos = Vec4();
				
				string seq = line.substr(5,std::string::npos);
				int ind = 0;
				lPos.x = atof( seq.substr(ind, seq.find(" ", ind)).c_str() );
				ind = seq.find(" ", ind+1);
				lPos.y = atof( seq.substr(ind, seq.find(" ", ind+1)).c_str() );
				ind = seq.find(" ", ind+1);
				lPos.z = atof( seq.substr(ind, seq.find(" ", ind+1)).c_str() );
				ind = seq.find(" ", ind+1);
				lPos.w = 1;
				
				marcher->lightPos.push_back(lPos);
				
			} else if(lineID == "LCOL") { //All good.
				Vec3 lCol = Vec3();
				
				string seq = line.substr(5,std::string::npos);
				int ind = 0;
				lCol.x = atof( seq.substr(ind, seq.find(" ", ind)).c_str() );
				ind = seq.find(" ", ind+1);
				lCol.y = atof( seq.substr(ind, seq.find(" ", ind+1)).c_str() );
				ind = seq.find(" ", ind+1);
				lCol.z = atof( seq.substr(ind, seq.find(" ", ind+1)).c_str() );
				
				marcher->lightCol.push_back(lCol);
			}
		};
		
		myfile.close();
	} else {
		cout << "Unable to open file";
	};
		
	return 0;
}
