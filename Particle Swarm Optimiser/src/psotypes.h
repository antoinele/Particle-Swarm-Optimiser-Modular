#pragma once

#include <vector>
#include <functional>
#include <random>

using namespace std;

namespace pso {

	typedef vector<double> coordinate;
	typedef function<double(coordinate)> evaluator_fn;
	// typedef double (*evaluator_fn)();
	typedef function<double()> rand_fn;
	typedef mt19937_64 pso_rng;
}