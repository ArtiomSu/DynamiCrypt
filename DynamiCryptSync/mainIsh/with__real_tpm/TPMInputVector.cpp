#include "tpminputvector.h"
#include "dynamicarray.h"
#include "treeparitymachine.h"
#include "definitions.h"
#include "tpmhandler.h"

#include <iostream>
#include <stdlib.h>
#include <string>
#include <sstream>

void TPMInputVector::CreateRandomVector (int K, int N) {
	int i;
	for (i = 0; i < K * N; i++)
		X.Z[i]= TpmHandler::RandomBit ();
}

void TPMInputVector::xLength (int K, int N) {
	X.length(K * N);
}

void TPMInputVector::set_from_string(std::string in_string, int K, int N){
	int iterator = 0;

	for(int i=0;i<in_string.length();i++){
		//cout << input[i] - '0' << " " << input[i] << endl;
		if(in_string[i] == '-'){
			X.Z[iterator] = -1;
			i++;
			//std::cout << "" << -1;
		}
		else{
			X.Z[iterator] = 1;
			//std::cout << "" << 1;
		}

		iterator++;
    	//in_vec->X.Z[i] = input[i] - '0';
	}
}

std::string TPMInputVector::to_string(int K, int N){
	std::ostringstream oss;
  	for (int temp = 0; temp < K*N; temp++)
    	oss << X.Z[temp];
  	return oss.str();
}

