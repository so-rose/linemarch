#include "OpenImageIO/imageio.h"
OIIO_NAMESPACE_USING

#include <omp.h>

#include "march.h"
#include "reader.h"

#include <iostream>
#include <string>
#include <chrono>
#include <unistd.h>

#include <sys/resource.h>
using namespace std;


int main(int argc, char* argv[]) {
	// TODO: Output All Passes simultaneously. HINT: enumeration is just a fancy list of int states.
	// TODO: Blender addon, using named empty properties.
	
	// Enable to debug segfaults.
	struct rlimit core_limits;
	core_limits.rlim_cur = core_limits.rlim_max = RLIM_INFINITY;
	setrlimit(RLIMIT_CORE, &core_limits);
	
	//Read constants file.
	string conf = "";
	if (argc > 1) conf = (string)argv[1];
	else conf = "constants.txt";
	
	// Setup basic variables.
	March marcher = March();
	Reader reader = Reader(&marcher);
	
	cout << "Reading Constants File: " + conf << endl;
	reader.read(conf);
	
	vector<Vec4> defLPos = marcher.lightPos;
	vector<Vec3> defLCol = marcher.lightCol;
	
	//~ if (marcher.pref.empty()) marcher.startFrame = 0; marcher.stopFrame = 0;
	if (marcher.voxName.empty()) marcher.voxName = "vox.txt";
	//~ cout << marcher.stopFrame << endl;
	
	for(int frame = marcher.startFrame; frame <= marcher.stopFrame; frame++) {
		marcher.lightPos = defLPos;
		marcher.lightCol = defLCol;
		if (!marcher.pref.empty()) reader.read(marcher.pref + to_string(frame) + ".txt"); //Read config sequence.
		//~ March::testVals(marcher);
		//~ return 0; //Test the reader.
		
		//Setup image specs.
		string filename;
		if (!marcher.pref.empty()) filename = marcher.outFile + to_string(frame) + ".exr";
		else filename = marcher.outFile + ".exr";
		
		const int xres = marcher.resolution[0];
		const int yres = marcher.resolution[1];
		const int channels = 5; // RGB for Rendering
		
		struct Pixel { //Struct where values will go.
			float r, g, b, a, z;
		};
		
		Pixel* pixels = new Pixel[xres*yres];
		
		//Render itself.
		marcher.pass = (Passes)0;//DEPTH;
		marcher.setupCamera();
		marcher.setupVoxels();
		unsigned long progress = 0;
		
		if (!marcher.pref.empty()) cout << "Rendering with config: " + marcher.pref + to_string(frame) + ".txt" + "..." << endl;
		else cout << "Rendering..." << endl;
		
		//Wee bit of benchmarking :)
		unsigned long start_ms = chrono::system_clock::now().time_since_epoch() / chrono::milliseconds(1);
		//The "magic".
		//~ #pragma omp parallel for //We're NOT THREAD SAFE!!! (But that's OK)
		for(int i=0; i < xres*yres; i++) {
			// Determine x and y for raycasting.
			int y = i / xres;
			int x = i % xres;
						
			// Update progress
			if (progress % 500 == 0 && progress != 0) {
				cout << "\r" << ((double)progress / (double)(xres*yres)) * 100 << "%  " << flush; //Percent Finished.
			}
					
			//"Magic" Calculations
			Vec4 ray = marcher.rayCast(x, y); //Returns a ray.
			Image vals = marcher.march(ray); //Marches along the ray, taking lighting into account. Uses alpha.
			
			pixels[i].r = vals.rgb.x;
			pixels[i].g = vals.rgb.y;
			pixels[i].b = vals.rgb.z;
			pixels[i].a = vals.alpha;
			pixels[i].z = vals.depth;
			
			progress++;
		}
		//Finishing the benchmark and printing it (with correct OOMs!)
		unsigned long end_ms = chrono::system_clock::now().time_since_epoch() / chrono::milliseconds(1);
		float time_sec = (float)(end_ms - start_ms) / 1000;
		cout << "Render completed in " << time_sec << " seconds." << endl;
		
		//Create output image.		
		ImageOutput *out = ImageOutput::create ("tmpOut.exr");
		
		if (! out) {
			cout << "Image Creation Failed." << endl;
			delete out;
			return 1;
		}
		
		if (! out->supports("channelformats")) {
			delete out;
			return 1;
		}
		
		ImageSpec spec (xres, yres, channels, TypeDesc::FLOAT);
		
		//Per Channel
		spec.channelformats.push_back (TypeDesc::FLOAT);
		spec.channelformats.push_back (TypeDesc::FLOAT);
		spec.channelformats.push_back (TypeDesc::FLOAT);
		spec.channelformats.push_back (TypeDesc::FLOAT);
		spec.channelformats.push_back (TypeDesc::FLOAT);
		
		spec.channelnames.clear ();
		
		spec.channelnames.push_back ("R");
		spec.channelnames.push_back ("G");
		spec.channelnames.push_back ("B");
		spec.channelnames.push_back ("A");
		spec.channelnames.push_back ("Z");
		
		//Write image.
		out->open ("tmpOut.exr", spec);
		out->write_image (TypeDesc::UNKNOWN, pixels, sizeof(Pixel));
		out->close();
		
		delete out;
		
		//Copy over file. Overwrite if exists.
		remove(filename.c_str());
		
		link("tmpOut.exr", filename.c_str());
		unlink("tmpOut.exr");
		
		cout << filename << " written!\n" << std::endl;
	}
	return 0;
}
