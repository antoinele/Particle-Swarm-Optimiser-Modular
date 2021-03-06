﻿#include "utilities.h"
#include "psotypes.h"

#include <cmath>
#include <iterator>
#include <sstream>
#include <string>

#include <iostream>

using namespace std;
using namespace pso;

coordinate pso::generate_coordinate(const vector<vector<double>> *bounds, pso_rng *gen)
{
	coordinate c(bounds->size());

	assert(bounds->size() > 0);

	for (size_t i = 0; i < bounds->size(); ++i)
	{
		uniform_real_distribution<double> dist((*bounds)[i][0], (*bounds)[i][1]);

		c(i) = dist(*gen);
	}

	return c;
}

VectorXd pso::coordinateToVectorXd(const coordinate* c)
{
	return VectorXd::Map(c->data(), c->size());
	//VectorXd v(c->size());

	//for (size_t i = 0; i < c->size(); i++)
	//{
	//	double d = (*c)[i];
	//	//assert(isfinite<double>(d));
	//	v(i) = d;
	//}

	//assert((size_t)v.size() == c->size());

	//return v;
}

coordinate pso::vectorXdToCoordinate(const Eigen::VectorXd* v)
{
	return *v;
	//return coordinate(v->data(), v->data() + v->size());

	//coordinate c(v->size());
	////c.resize(v->size());

	//for (int i = 0; i < v->size(); i++)
	//{
	//	double d = (*v)(i);
	//	//assert(isfinite<double>(d));
	//	c[i] = d;
	//}

	//assert(c.size() == (size_t)v->size());

	//return c;
}

VectorXd pso::randomWeightVector(pso_rng * gen, const int n_dimensions)
{
	VectorXd v(n_dimensions);

	uniform_real_distribution<double> dist(0.0, 1.0);

	for (int i = 0; i < n_dimensions; i++)
	{
		v(i) = dist(*gen);
	}

	return v;
}

string pso::coordinateToString(const coordinate * c)
{
	assert(c->size() > 0);
	ostringstream oss;
	
	oss << (*c)(0);

	for (auto i=0; i < c->size(); ++i)
	{
		oss << ", ";
		oss << (*c)(i);
	}

	return oss.str();
}

vector<coordinate> pso::generate_solutions(problem_base * prob, int num_solutions, int seed)
{
	assert(num_solutions > 0);
	vector<coordinate> solutions(num_solutions);
	//solutions.reserve(num_solutions);

	pso_rng gen(seed);

	vector<vector<double>> bounds = prob->bounds();

	for (int i = 0; i < num_solutions; i++)
	{
		coordinate c;
		int tries = 0;

		do {
			tries++;
			c = generate_coordinate(&bounds, &gen);
		} while (!prob->is_valid(c));

		assert(c.size() == bounds.size());
		assert(prob->is_valid(c));

		solutions[i] = c;
	}

	return solutions;
}

void pso::print_solutions(problem_base* aar, vector<coordinate>* solutions)
{
	for (auto i : *solutions)
	{
		cout << "Solution: ";
		cout << coordinateToString(&i) << endl;
		cout << "Fitness: ";
		if (aar->is_valid(i))
			cout << aar->evaluate(i) << endl;
		else
			cout << "Invalid!" << endl;
	}
}

void pso::print_solutions(vector<coordinate>* solutions)
{
	for (vector<coordinate>::iterator i = solutions->begin(); i != solutions->end(); ++i)
	{
		cout << "Solution: ";
		cout << coordinateToString(&(*i)) << endl;
	}
}
