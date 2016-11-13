#pragma once

#include <problem.h>
#include <vector>

using namespace pso;

class SphereProblem : public problem {
private:
	const int n_dimensions;
	vector<vector<double>> _bounds;
public:
	SphereProblem(int n_dimensions);
	vector<vector<double>> bounds();
	bool is_valid(coordinate c);
	double evaluate(coordinate c);

	bool comparator(double a, double b);
};