/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   treeparitymachine.h
 * Author: mrarchinton
 *
 * Created on 29 November 2018, 12:38
 */

#ifndef TREEPARITYMACHINE_H
#define TREEPARITYMACHINE_H
#include "definitions.h"
#include "dynamicarray.h"

class TreeParityMachine {
		public :
		DynamicArray <int> W, H;
		int K, N, L;
		int TPMOutput;
		void Initialize ();
		void ComputeTPMResult (const DynamicArray <int> & X);
		void UpdateWeight (const DynamicArray <int> & X);
		void RandomWeight ();
	};



#endif /* TREEPARITYMACHINE_H */

