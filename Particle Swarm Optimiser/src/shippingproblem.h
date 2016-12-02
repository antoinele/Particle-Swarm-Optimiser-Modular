#pragma once

#include "problem.h"
#include <string>
#include <vector>
#include <Eigen/Dense>

class shippingproblem : public pso::problem_base
{
private:
	size_t dimensions;
	std::vector<std::vector<double>> testdata;

	Eigen::MatrixXd simdtestdata;

	void generatesimdtestdata();

public:
	shippingproblem(std::string csvfile);

	std::vector<std::vector<double>> bounds();
	//bool is_valid(coordinate c); // default is fine
	double evaluate(pso::coordinate c);

	inline bool comparator(double a, double b)
	{
		return a < b;
	}
};