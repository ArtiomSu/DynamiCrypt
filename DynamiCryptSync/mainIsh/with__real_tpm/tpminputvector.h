


#ifndef TPMINPUTVECTOR_H
#define TPMINPUTVECTOR_H
#include "definitions.h"
#include "dynamicarray.h"
#include "treeparitymachine.h"

	//const char Dictionary[38] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ_0123456789";


	class TPMInputVector {
		public :   
		DynamicArray <int> X;
		void CreateRandomVector (int K, int N);
		void xLength (int K, int N);
	};

	

#endif