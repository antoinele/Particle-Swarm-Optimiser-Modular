#include "SphereProblem.h"
#include <cmath>
#include <cassert>

SphereProblem::SphereProblem(int n_dimensions) : n_dimensions(n_dimensions)
{
	for (size_t i = 0; i < n_dimensions; i++)
	{
		vector<double> bounds = { 1.0, 1.0 };
		_bounds.push_back(bounds);
	}
}

vector<vector<double>> SphereProblem::bounds()
{
	return _bounds;
}

bool SphereProblem::is_valid(coordinate c)
{
	assert(c.size() == n_dimensions);

	for (size_t i = 0; i < n_dimensions; i++)
	{
		if (c[i] < _bounds[i][0] || c[i] > _bounds[i][1])
			return false;
	}

	return true;
}

double SphereProblem::evaluate(coordinate c)
{
	assert(c.size() == n_dimensions);

	double d = 0.0;

	for (size_t i = 0; i < n_dimensions; i++)
	{
		d += c[i] * c[i];
	}

	return sqrt(d);
}

inline bool SphereProblem::comparator(double a, double b)
{
	return a < b;
}
