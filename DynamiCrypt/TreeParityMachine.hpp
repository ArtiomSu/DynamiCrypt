/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   TreeParityMachine.hpp
 * Author: mrarchinton
 *
 * Created on 11 March 2019, 14:27
 */

#ifndef TREEPARITYMACHINE_HPP
#define TREEPARITYMACHINE_HPP

#include "DynamicArray.hpp"

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




#endif /* TREEPARITYMACHINE_HPP */

