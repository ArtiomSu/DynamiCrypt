/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   TpmHandler.hpp
 * Author: mrarchinton
 *
 * Created on 11 March 2019, 14:03
 */

#ifndef TPMHANDLER_HPP
#define TPMHANDLER_HPP

#include "DynamicArray.hpp"
#include "TreeParityMachine.hpp"
#include "TPMInputVector.hpp"

class TpmHandler {
		public :
		TpmHandler ();
		static int IsEqual (int A, int B);
		static int RandomBit ();
		static int Signum (double r);
	};


#endif /* TPMHANDLER_HPP */

