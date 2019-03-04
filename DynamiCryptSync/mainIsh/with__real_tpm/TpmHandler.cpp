/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   TpmHandler.cpp
 * Author: mrarchinton
 * 
 * Created on 29 November 2018, 12:43
 */

#include "tpmhandler.h"
#include "tpminputvector.h"
#include "dynamicarray.h"
#include "treeparitymachine.h"
#include "definitions.h"
#include "tpminputvector.h"
#include <iostream>
#include <stdlib.h>

TpmHandler::TpmHandler () {

}

int TpmHandler::IsEqual (int A, int B) {
	int result;
	if (A == B) 
		result = 1;
	else
		result = 0;
	return result;
}

int TpmHandler::RandomBit () {
	int result, A;
	A = rand() % 2;
	if (A == 0) 
		result = - 1;
	else
		result = 1;
	return result;
}

int TpmHandler::Signum (double r) {
	int result;
	if ( r > 0 ) 
		result = 1;
	else
		result = - 1;
	return result;
}

