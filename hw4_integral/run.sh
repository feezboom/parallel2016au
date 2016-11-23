#!/bin/bash

for i in {1..20}
do
	gcc $1 -fopenmp -o out -lm
	./out
done
