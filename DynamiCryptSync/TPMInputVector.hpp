/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   TPMInputVector.hpp
 * Author: mrarchinton
 *
 * Created on 11 March 2019, 14:27
 */

#ifndef TPMINPUTVECTOR_HPP
#define TPMINPUTVECTOR_HPP

#include "DynamicArray.hpp"
#include "TreeParityMachine.hpp"

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

	

#endif /* TPMINPUTVECTOR_HPP */

