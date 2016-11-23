#include <fstream>
#include <ctime> 
#include <cstdlib>
#include <cstdlib>

#define fail_if(condition, message) if(condition) {fprintf(stdout, message); fflush(stdout); exit(EXIT_FAILURE);}

using namespace std;

int main(int argc, char** argv) {

	int a = atoi(argv[1]), b = atoi(argv[2]);
	srand(time(NULL));
	fstream o("state.dat", ios::out);
	fail_if(!o, "no state.dat found\n");

	o << a << " " << b << endl;

	for (int i = 0; i < a; ++i)
		for (int j = 0; j < b; ++j)
			if (rand() % 2) 
				o << rand() % a << " " << rand() % b << endl;

	o << endl;
	o.close();
	
	return 0;
}
