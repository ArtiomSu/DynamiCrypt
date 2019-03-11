/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   DynamicArray.hpp
 * Author: mrarchinton
 *
 * Created on 11 March 2019, 14:29
 */

#ifndef DYNAMICARRAY_HPP
#define DYNAMICARRAY_HPP

template <class T>
	class DynamicArray {
		public:
		T* Z;
		~DynamicArray (void){ delete [] Z; Z = 0;}
		void length (T n){ Z = new T [n]; }
	};


#endif /* DYNAMICARRAY_HPP */

