#pragma once

#include <problem.h>
#include <string>

using namespace pso;
using namespace std;

class shippingproblem : public problem
{
private:
	int dimensions;
	vector<vector<double>> testdata;

	double predict_load(vector<double>* historical_data, coordinate* weights);

public:
	shippingproblem(string csvfile);
	vector<vector<double>> bounds();
	//bool is_valid(coordinate c);
	double evaluate(coordinate c);

	bool comparator(double a, double b);
};