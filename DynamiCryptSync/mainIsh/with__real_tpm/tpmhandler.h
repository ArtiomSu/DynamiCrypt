/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   TpmHandler.h
 * Author: mrarchinton
 *
 * Created on 29 November 2018, 12:43
 */

#ifndef TPMHANDLER_H
#define TPMHANDLER_H
#include "definitions.h"
#include "dynamicarray.h"
#include "treeparitymachine.h"
#include "tpminputvector.h"

class TpmHandler {
		public :
		TpmHandler (int initK, int initN, int initL, TreeParityMachine A, TreeParityMachine B, TPMInputVector objInput);
		static int IsEqual (int A, int B);
		static int RandomBit ();
		static int Signum (double r);
	};


#endif /* TPMHANDLER_H */

