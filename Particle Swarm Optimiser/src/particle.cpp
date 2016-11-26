#include "particle.h"

#include <memory>
#include <functional>
#include <cmath>
#include <iostream>

#include <Eigen/Dense>

#include "utilities.h"
#include "optimiser.h"

using namespace std;
using namespace pso;

particle::particle(optimiser* optimiser)
{
	bounds = optimiser->problem()->bounds();
	n_dimensions = (int)bounds.size();

	vector<vector<double>> vbounds;

	for (int i = 0; i < n_dimensions; ++i)
	{
		vector<double> b = { -1, 1 };
		vbounds.push_back(b);
	}

	shared_ptr<pso_rng> gen = optimiser->thread_rng();

	coordinate newposition;
	coordinate newvelocity;

	newposition = generate_coordinate(&bounds, gen.get());
	newvelocity = generate_coordinate(&vbounds, gen.get());

    particle(optimiser, newposition, newvelocity);
}

particle::particle(optimiser* optimiser, coordinate position, coordinate velocity)
	: opt(optimiser)
{
	bounds = optimiser->problem()->bounds();

	n_dimensions = (int)bounds.size();

	assert(position.size() == n_dimensions);
	assert(velocity.size() == n_dimensions);

	shared_ptr<pso_rng> gen = optimiser->thread_rng();
	
	_position = coordinateToVectorXd(&position);
	_velocity = coordinateToVectorXd(&velocity);

	_best_position = _position;
	best_position_fitness = evaluate();

	assert(_best_position.size() == n_dimensions);

	gen.reset();
}

VectorXd particle::find_nbest_position()
{
    VectorXd* cur_best_position = &_best_position;
	double cur_best_fitness = opt->evaluator(best_position());

    for (auto i : neighbours)
    {
        double fitness = opt->evaluator(i->best_position());

        if(comparator(fitness, cur_best_fitness))
        {
            cur_best_fitness = fitness;
            cur_best_position = &(i->_best_position);
        }
    }

    return *cur_best_position;
}

VectorXd pso::particle::find_gbest_position()
{
	return VectorXd::Map(opt->g_best.data(), opt->g_best.size());
}

void particle::move_step()
{
	double c1, c2, r1, r2;

	c1 = c2 = 2;

	VectorXd neighborhood_best;
	if (neighbours.size() > 0)
		neighborhood_best = find_nbest_position();
	else
		neighborhood_best = find_gbest_position();

	shared_ptr<pso_rng> rng(opt->thread_rng());

	uniform_real_distribution<double> dist(0.0, 1.0);

	r1 = dist(*rng.get());
	r2 = dist(*rng.get());

	VectorXd particleV = _best_position - _position;
	VectorXd nbestV = neighborhood_best - _position;

	particleV *= r1;
	nbestV    *= r2;

	//particleV = particleV.cwiseProduct(randomWeightVector(rng.get(), n_dimensions));
	//nbestV    = nbestV.cwiseProduct(randomWeightVector(rng.get(), n_dimensions));

	particleV *= c1;
	nbestV    *= c2;

	_velocity += particleV + nbestV;
}

void particle::end_step()
{
	_position += _velocity;

	// if the particle hits the edge, bound the values
	for (int i = 0; i < n_dimensions; i++)
	{
		if (_position[i] > bounds[i][1])
		{
			//_position[i] = bounds[i][1] - fabs(_position[i] - bounds[i][1]);
			_position[i] = bounds[i][1];
			_velocity[i] = 0; // stop moving further out of bounds
		}
		else if (_position[i] < bounds[i][0])
		{
			//_position[i] = bounds[i][0] + fabs(_position[i] - bounds[i][0]);
			_position[i] = bounds[i][0];
			_velocity[i] = 0; // stop moving further out of bounds
		}
	}

	double cur_fitness = evaluate();
	if (comparator(cur_fitness, best_position_fitness))
	{
		_best_position = _position;
		best_position_fitness = cur_fitness;
	}
}
