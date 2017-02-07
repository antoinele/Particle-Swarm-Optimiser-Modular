#include <vector>
#include <iostream>
#include <cstdlib>
#include "shippingproblem.h"
#include "utilities.h"

using namespace std;

int main(int argc, char* argv[])
{
	string csvfile;
	vector<double> weights;

	if (argc < 2)
	{
		cerr << "not enough arguments" << endl;
		return 1;
	}

	csvfile = argv[1];

	for (int i = 2; i < argc; i++)
	{
		double d = atof(argv[i]);
		weights.push_back(d);
	}

	cerr << "num weights: " << weights.size() << endl;
	cerr << "weights: " << pso::coordinateToString(&weights) << endl;

	shippingproblem sp(csvfile);

	if (sp.bounds().size() != weights.size()) {
		cerr << "Incorrect number of weights given (" << weights.size() << ")" << endl;
		cerr << "Expected: " << sp.bounds().size() << endl;
		exit(1);
	}

	double fitness = sp.evaluate(weights);

	cout << "Fitness: " << fitness << endl;

	return 0;
}