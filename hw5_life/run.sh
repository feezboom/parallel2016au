#!/bin/bash

g++ ./gen.cpp -o gen
./gen $1 $2 state.dat
g++ ./main.cpp -fopenmp -o main

#for i in {1..20}
#do	
	./main $1 $2 state.dat
	#echo "-----------------------------------------"
#done

#rm ./gen
#rm ./main
#rm ./input
