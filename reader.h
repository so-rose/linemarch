#ifndef READER
#define READER

#include "march.h"
#include "math.h"

class Reader {
	public:
		Reader(March*);
		~Reader();
		int read(string);
		March* marcher;
};

#endif
