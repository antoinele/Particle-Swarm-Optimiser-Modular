#pragma once

#include <problem.h>
#include <vector>

using namespace pso;

class SphereProblem : public problem_base {
private:
	const int n_dimensions;
	vector<vector<double>> _bounds;
public:
	SphereProblem(int n_dimensions);
	vector<vector<double>> bounds();
	bool is_valid(coordinate c);
	double evaluate(coordinate c);

	inline bool comparator(double a, double b)
	{
		return a < b;
	}
};