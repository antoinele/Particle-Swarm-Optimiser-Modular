#pragma once

#include "psotypes.h"
#include "problem.h"
#include <vector>
#include <random>
#include <Eigen/Dense>

using namespace std;
using namespace Eigen;

namespace pso {
	pso::coordinate generate_coordinate(const vector<vector<double>> *bounds, pso_rng *gen);

	VectorXd coordinateToVectorXd(const coordinate* c);
	coordinate vectorXdToCoordinate(const Eigen::VectorXd* v);

	VectorXd randomWeightVector(pso_rng* gen, const int n_dimensions);

	string coordinateToString(const coordinate* c);

	vector<coordinate> generate_solutions(problem* prob, int num_solutions, int seed = 0);

	void print_solutions(problem* aar, vector<coordinate>* solutions);
	void print_solutions(vector<coordinate>* solutions);

	template <typename T> void print_vecvec(vector<vector<T>>* vv)
	{
		for (vector<coordinate>::iterator i = vv->begin(); i != vv->end(); ++i)
		{
			cout << coordinateToString(&(*i)) << endl;
		}
	}
}