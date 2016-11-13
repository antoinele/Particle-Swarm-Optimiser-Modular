#ifndef PARTICLE_H
#define PARTICLE_H

#include <vector>
#include <random>
#include <functional>
#include <limits>
#include <memory>

#include "psotypes.h"
#include "optimiser.h"
#include "utilities.h"
#include <Eigen/Dense>

using namespace std;
using namespace pso;
using namespace Eigen;

namespace pso {

	class optimiser;

    class particle
    {
    private:
		int n_dimensions;
		vector<vector<double>> bounds;

		/**
		 * Return whether the left item is better than the right one
		 */
		bool comparator(double a, double b);

		weak_ptr<optimiser> _optimiser;

		shared_ptr<optimiser> getOptimiser() {
			return _optimiser.lock();
		}

        VectorXd find_nbest_position();

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

        double evaluate();

        particle(shared_ptr<optimiser> optimiser);
		particle(shared_ptr<optimiser> optimiser, coordinate position, coordinate velocity);

        ~particle() {
			//_optimiser.reset();
        };
    };
}

#endif