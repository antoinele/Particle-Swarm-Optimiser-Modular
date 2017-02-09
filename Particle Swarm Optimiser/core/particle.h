#pragma once

#include <core/optimiser.h>
#include <core/psotypes.h>
#include <core/utilities.h>
#include <Eigen/Dense>

#include <vector>
#include <limits>

namespace {
	using namespace std;
	using namespace pso;
	using namespace Eigen;
}

namespace pso {

    class particle
    {
    private:
		size_t n_dimensions;
		vector<vector<double>> bounds;

		inline VectorXd find_lbest()
		{
			return optimiser::get_optimiser_raw()->_neighbourhood->find_lbest(this);
		}

        VectorXd _velocity;
		VectorXd _position;
		VectorXd _best_position;

		double best_position_fitness;

    public:
		coordinate position() {
			return vectorXdToCoordinate(&_position);
		}
		coordinate best_position() {
			return vectorXdToCoordinate(&_best_position);
		}
		coordinate velocity() {
			return vectorXdToCoordinate(&_velocity);
		}

        vector<particle*> neighbours;

        void move_step();
        void end_step();

		inline double evaluate()
		{
			return optimiser::get_optimiser_raw()->evaluator(position());
		}

        particle();
		particle(coordinate position, coordinate velocity);
    };
}
