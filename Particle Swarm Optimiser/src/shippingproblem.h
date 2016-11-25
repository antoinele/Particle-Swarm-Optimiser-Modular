#pragma once

#include "problem.h"
#include <string>

using namespace pso;
using namespace std;

class shippingproblem : public problem_base
{
private:
	size_t dimensions;
	vector<vector<double>> testdata;

	//double predict_load(vector<double>::iterator hd_start, vector<double>::iterator hd_end, coordinate* weights);

public:
	shippingproblem(string csvfile);
	vector<vector<double>> bounds();
	//bool is_valid(coordinate c); // default is fine
	double evaluate(coordinate c);

	inline bool comparator(double a, double b)
	{
		return a < b;
	}
};