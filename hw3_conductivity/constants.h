#ifndef _CONSTANTS_H_
#define _CONSTANTS_H_

#include <assert.h>
#include <stdio.h>
#include <math.h>

typedef int bool;

// Plate sizes
long double Lx;
long double Ly;

// Space step
long double dh;

// Distribution
long int ROWS_NUMBER;
long int COLUMNS_NUMBER;

// Temperatures
long double Ul;
long double Ur;
long double Uu;
long double Ud;
long double U0;

bool dQtop; // bool
bool dQbottom; // bool
bool dQleft; // bool
bool dQright; // bool

// Physics constants
long double ro;
long double c;
long double lambda;
long double k;

// Time
long double dt;
long double UP_TIME;
 
// Threading
int PROCESSES_NUMBER;
int MAX_RANK;
 
// Update equation coefficient
long double coef;


// INIT
int initConstants() {
	// Plate sizes
	Lx = 0.5;
	Ly = 0.5;

	// Space step
	dh = 0.01;

	// Distribution
	ROWS_NUMBER = round(Ly/dh)-1;
	COLUMNS_NUMBER = round(Lx/dh)-1;

	// Temperatures
	Ul = 0;
	Ur = 0;
	Uu = 80;
	Ud = 30;
	U0 = 5;
	
	dQtop = 1; // true
	dQbottom = 1; // true
	dQleft = 0; // false
	dQright = 0;  // false

	// Physics constants
	ro = 8960;
	c = 380;
	lambda = 401;
	k = lambda/(ro*c);


	// Time
	UP_TIME = 60; // Time suprem
	// step : [ dt < (dh*dh)/k ]
	long double temp = ((dh*dh)/k)*0.05;
	dt = UP_TIME; int i = 0;
	while (dt >= temp) {
		if (i++ % 2 == 0) {
			dt /= 2;
		} else {
			dt /= 5;
		}
	}
	
	// Threading
	MAX_RANK = PROCESSES_NUMBER - 1;
	
	// Update equation coefficient
	coef = (k*dt)/(dh*dh);
	
	assert((k*dt)/(dh*dh) < 1);
	return 0;
}

int printConst() {
	printf("PROCESS NUMBER == %d\n", MAX_RANK + 1);
	printf("Chosen dt == %Lf\n", dt);
	printf("k = %Lf\tdt = %Lf\tcoef=%Lf\tdh ==%Lf\tLx == %Lf\t Ly == %Lf\n", k, dt, coef, dh, Lx, Ly);
	printf("Columns=%ld\tRows=%ld\t\n", COLUMNS_NUMBER, ROWS_NUMBER);
	return 0;
}

#endif
