#ifndef TPMINPUTVECTOR_H
#define TPMINPUTVECTOR_H
#include "definitions.h"
#include "dynamicarray.h"
#include "treeparitymachine.h"

#include <string>
	//const char Dictionary[38] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ_0123456789";


	class TPMInputVector {
		public :   
		DynamicArray <int> X;
		void CreateRandomVector (int K, int N);
		void xLength (int K, int N);
		void set_from_string(std::string in_string, int K, int N);
		std::string to_string(int K, int N);
	};

	

#endif