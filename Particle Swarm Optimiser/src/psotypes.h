#pragma once

#include <vector>
#include <functional>
#include <random>

namespace pso {
	using namespace std;

	typedef vector<double> coordinate;
	typedef function<double(coordinate)> evaluator_fn;
	// typedef double (*evaluator_fn)();
	typedef function<double()> rand_fn;
	typedef mt19937_64 pso_rng;

}