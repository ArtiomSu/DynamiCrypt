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
#include <fstream>
//#include <boost/random/mersenne_twister.hpp>
//#include <boost/random/uniform_int_distribution.hpp>

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
       // boost::random::mt19937 gen;
       // boost::random::uniform_int_distribution<> dist(1, 429496729);
       // int random_number = dist(gen);
    
        unsigned long long int random_value = 0; //Declare value to store data into
        size_t size = sizeof(random_value); //Declare size of data
        std::ifstream urandom("/dev/urandom", std::ios::in|std::ios::binary); //Open stream
        if(urandom) //Check if stream is open
        {
            urandom.read(reinterpret_cast<char*>(&random_value), size); //Read from urandom
            if(urandom) //Check if stream is ok, read succeeded
            {
                std::cout << "Read random value: " << random_value << std::endl;
            }
            else //Read failed
            {
                std::cerr << "Failed to read from /dev/urandom" << std::endl;
            }
            urandom.close(); //close stream
        }
        else //Open failed
            {
                std::cerr << "Failed to open /dev/urandom" << std::endl;
        }
        
        
        std::cout << "random number: " << random_value << "\n";
        srand( random_value );
	int i;
	for ( i = 0; i < K * N; i ++ ){
		W.Z[i]= L - (rand() % (2 * L + 1));
            
            //W.Z[i]= L - (random_number % (2 * L + 1));
            //std::cout << random_number;
            //random_number = dist(gen);
        }
        
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


