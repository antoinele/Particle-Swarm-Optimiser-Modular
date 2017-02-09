#pragma once

#include <Eigen/Dense>
#include <random>

namespace pso {
	typedef Eigen::VectorXd coordinate;
	typedef std::mt19937_64 pso_rng;
}