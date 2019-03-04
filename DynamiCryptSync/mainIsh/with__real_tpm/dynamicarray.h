/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   dynamicarray.h
 * Author: mrarchinton
 *
 * Created on 29 November 2018, 12:30
 */

#ifndef DYNAMICARRAY_H
#define DYNAMICARRAY_H
#include "definitions.h"

template <class T>
	class DynamicArray {
		public:
		T* Z;
		~DynamicArray (void){ delete [] Z; Z = NULL;}
		void length (T n){ Z = new T [n]; }
	};



#endif /* DYNAMICARRAY_H */

