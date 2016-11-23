#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <omp.h>

#define MAX_THREADS 20
#define PARTS_NUMBER 10000000
const long double step = 1./PARTS_NUMBER;
const long double eps = 0.0000000001;

const double desired_res = M_PI;

double calcInnacuracy(double res, double desire) {
	return fabs(res-desire)/desire;
};

double f(double x) {
	return (1./(1.+x*x));
}

double calculateReduction() {
	double result = 0;
	int i;
	#pragma omp parallel reduction(+:result)
	{
		#pragma omp for
		for (i = 0; i < PARTS_NUMBER; ++i) {
			double x1 = ((i + 0.0)/PARTS_NUMBER)*1;
			double x2 = ((i + 1.0)/PARTS_NUMBER)*1;
			result += (f(x1) + f(x2))* step / 2;
		}
	}
	return result;
}

double calculateCritical() {
	double res = 0;
	#pragma omp parallel shared(res)
	{
		double temp = 0;
		int threads = omp_get_num_threads();
		int num = omp_get_thread_num();
		
		double curX = ((num+0.0)/PARTS_NUMBER) * 1;
		double nextX = curX + step;
		
		while (fabs(curX - 1) > eps  && (nextX < 1. + eps)) {
			temp += step * (f(curX) + f(nextX)) / 2.0;
			curX += step * (threads);
			nextX = curX + step;
		}
		#pragma omp critical
		{
			res += temp;
		}
	}
	return res;
}

int main(int argc, char** argv) {
	double res;
	int i;
	
// REDUCTION
	for (i = 1; i <= 20; ++i) {
		omp_set_num_threads(i);
		double begin, end, total;
		begin = omp_get_wtime();
		res = calculateReduction();
		end = omp_get_wtime();
		total = end - begin;
		printf("%d\t%lf\t%lf\n", i, total, res*4);
		//assert(calcInnacuracy(res*4, M_PI) < 0.001);
	}
	
	printf("\n\n\n");
	
// CRITICAL SECTION
	for (i = 1; i <= 20; ++i) {
		omp_set_num_threads(i);
		double begin, end, total;
		begin = omp_get_wtime();
		res = calculateCritical();
		end = omp_get_wtime();
		total = end - begin;
		printf("%d\t%lf\t%lf\n", i, total, res*4);
		//assert(calcInnacuracy(res*4, M_PI) < 0.001);
	}
	
	

	return 0;
}
