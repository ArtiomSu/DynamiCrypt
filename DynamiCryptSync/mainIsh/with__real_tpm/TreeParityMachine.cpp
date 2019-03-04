/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   TreeParityMachine.cpp
 * Author: mrarchinton
 * 
 * Created on 29 November 2018, 12:39
 */

#include "treeparitymachine.h"
#include "tpminputvector.h"
#include "dynamicarray.h"
#include "treeparitymachine.h"
#include "definitions.h"
#include "tpmhandler.h"

#include <iostream>
#include <stdlib.h>


void TreeParityMachine::ComputeTPMResult (const DynamicArray <int> & X) {
	int i, j, sum;
	TPMOutput = 1;
	for (i = 0; i < K; i++) {
		sum = 0;
		for (j = 0; j < N; j++) {
			sum = sum + (W.Z[i * N + j] * X.Z[i * N + j]);
		}
		H.Z[i]= TpmHandler::Signum(sum);
		TPMOutput = TPMOutput * TpmHandler::Signum (sum);
	}
}

void TreeParityMachine::Initialize () {
	W.length(K * N);
	H.length(K);
}

void TreeParityMachine::RandomWeight () {
	int i;
	for ( i = 0; i < K * N; i ++ )
		W.Z[i]= L - (rand() % (2 * L + 1));
}


void TreeParityMachine::UpdateWeight (const DynamicArray <int> & X) {
	int i, j, newW;
	for (i = 0; i < K; i++) {
		for (j = 0; j < N; j++) {
			newW = W.Z[i * N + j];
			newW = newW + X.Z[i * N + j] * TPMOutput * TpmHandler::IsEqual (TPMOutput, H.Z[i]) * TpmHandler::IsEqual (TPMOutput, TPMOutput);
			if (newW > L) 
				newW = L;
			if (newW < - L) 
				newW = - L;
			W.Z[i * N + j] = newW;
		}
	}
}


