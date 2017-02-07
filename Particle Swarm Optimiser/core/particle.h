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

		optimiser* opt;

        VectorXd find_nbest_position();
		VectorXd find_gbest_position();

		inline VectorXd find_lbest()
		{
			return opt->_neighbourhood->find_lbest(this);
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
			return opt->evaluator(position());
		}

        particle(optimiser* optimiser);
		particle(optimiser* optimiser, coordinate position, coordinate velocity);
    };
}
