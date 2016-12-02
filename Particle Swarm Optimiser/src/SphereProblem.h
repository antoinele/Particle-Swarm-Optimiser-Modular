#pragma once

#include <problem.h>
#include <vector>

class SphereProblem : public pso::problem_base {
private:
	const int n_dimensions;
	std::vector<std::vector<double>> _bounds;
public:
	SphereProblem(int n_dimensions);
	std::vector<std::vector<double>> bounds();
	bool is_valid(pso::coordinate c);
	double evaluate(pso::coordinate c);

	inline bool comparator(double a, double b)
	{
		return a < b;
	}
};