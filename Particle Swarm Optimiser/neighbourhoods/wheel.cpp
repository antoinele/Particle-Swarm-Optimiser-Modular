#include <core/neighbourhood.h>
#include <core/init.h>

#include <core/optimiser.h>
#include <core/particle.h>

class wheel : public pso::neighbourhood_base
{
public:
	void init_neighbourhood();
	VectorXd find_lbest(particle* p);
};

void wheel::init_neighbourhood()
{
	auto particles = *optimiser::get_optimiser()->get_particles();

	// assume first particle is the hub and attach all particles to it
	for (auto p : particles) {
		particles[0]->neighbours.push_back(p.get());
		
		p->neighbours.push_back(particles[0].get());
	}
}

VectorXd wheel::find_lbest(particle * p)
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

std::shared_ptr<pso::neighbourhood_base> wheel_factory(vector<string> args)
{
	return make_shared<wheel>();
}

void wheel_init() {
	neighbourhood_base::register_neighbourhood("wheel", wheel_factory);
}

module_init(wheel_init);