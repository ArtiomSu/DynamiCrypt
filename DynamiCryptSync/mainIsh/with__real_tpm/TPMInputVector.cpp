/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "tpminputvector.h"
#include "dynamicarray.h"
#include "treeparitymachine.h"
#include "definitions.h"
#include "tpmhandler.h"

#include <iostream>
#include <stdlib.h>

void TPMInputVector::CreateRandomVector (int K, int N) {
	int i;
	for (i = 0; i < K * N; i++)
		X.Z[i]= TpmHandler::RandomBit ();
}

void TPMInputVector::xLength (int K, int N) {
	X.length(K * N);
}

