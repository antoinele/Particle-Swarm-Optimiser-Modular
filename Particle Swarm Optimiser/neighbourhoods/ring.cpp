#include <core/neighbourhood.h>
#include <core/init.h>

#include <core/optimiser.h>
#include <core/particle.h>

class ring : public neighbourhood_base
{
public:
	void init_neighbourhood();
	VectorXd find_lbest(particle* p);
};

void ring::init_neighbourhood()
{
	auto particles = optimiser::get_optimiser()->get_particles();

	size_t last = particles->size() - 1;

	// add neighbours for the first particle
	(*particles)[0]->neighbours.push_back((*particles)[last].get());
	(*particles)[0]->neighbours.push_back((*particles)[1].get());

	// add neighbours for the last particle
	(*particles)[last]->neighbours.push_back((*particles)[last - 1].get());
	(*particles)[last]->neighbours.push_back((*particles)[0].get());

	// add neighbours for the intermediate particles, skipping first and last
	for (int i = 1; i < particles->size() - 1; i++)
	{
		(*particles)[i]->neighbours.push_back((*particles)[i-1].get());
		(*particles)[i]->neighbours.push_back((*particles)[i+1].get());
	}
}

VectorXd ring::find_lbest(particle* p)
{
	auto opt = optimiser::get_optimiser_raw();

	particle* best_particle = nullptr;
	double cur_best_fitness = opt->worst_fitness;

	assert(p->neighbours.size() > 0);

	for (auto i : p->neighbours)
	{
		double fitness = opt->evaluator(i->best_position());

		if (opt->comparator(fitness, cur_best_fitness))
		{
			cur_best_fitness = fitness;
			best_particle = i;
		}
	}

	assert(best_particle != nullptr);

	return best_particle->best_position();
}

shared_ptr<neighbourhood_base> ring_factory(vector<string> args)
{
	return make_shared<ring>();
}

void ring_init()
{
	neighbourhood_base::register_neighbourhood("ring", ring_factory);
}

module_init(ring_init);