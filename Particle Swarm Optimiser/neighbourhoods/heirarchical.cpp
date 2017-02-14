#include <core/neighbourhood.h>
#include <core/init.h>

#include <core/optimiser.h>
#include <core/particle.h>

#include <memory>
#include <vector>
#include <random>
#include <scoped_allocator>

using namespace std;
using namespace pso;

class heirarchical : public neighbourhood_base
{
public:
	struct node {
		node* parent;
		node* left = nullptr;
		node* right = nullptr;

		bool add_child(node* n)
		{
			if (left == nullptr)
				left = n;
			else if (right == nullptr)
				right = n;
			else 
				return false;

			n->parent = this;
			return true;
		}

		particle* value;

		~node() {
			if(left != nullptr)
				delete left;
			if(right != nullptr)
				delete right;
		}
	};

	node root;

	vector<vector<node*>> stages; // not sure if this is needed
	map<particle*, node*> particlemap;

	void init_neighbourhood();
	VectorXd find_lbest(particle* p);
	void end_cycle();
};

void heirarchical::init_neighbourhood()
{
	auto particles = optimiser::get_optimiser()->get_particles();

	size_t stage = 0;

	vector<node*> prev_stage;
	vector<node*> cur_stage;

	size_t max_size = 1; // pow(particles_per_node, stage) ?
	size_t particles_per_node = 2;

	uniform_int_distribution<size_t> dist(0, 1);;

	pso_rng rng;
	rng.seed(optimiser::get_optimiser()->get_seed());

	for (auto it : *particles)
	{
		// hit max size, go to next stage
		if (cur_stage.size() == max_size)
		{
			stages.push_back(cur_stage);
			prev_stage.swap(cur_stage);
			cur_stage.clear();
			stage++;

			max_size = floor(pow(particles_per_node, stage));

			dist = uniform_int_distribution<size_t>(0, prev_stage.size()-1);
		}

		node* n = new node();
		n->value = it.get();

		particlemap[n->value] = n;

		if (stage > 0) {
			node* pn;
			do {
				size_t idx = dist(rng);

				pn = prev_stage[idx];
			} while (!pn->add_child(n));
		}
		else {
			n->parent = nullptr;
		}

		cur_stage.push_back(n);
	}
}

VectorXd heirarchical::find_lbest(particle * p)
{
	auto opt = optimiser::get_optimiser_raw();
	node* n = particlemap[p];
	
	node* best_node = nullptr;
	double cur_best_fitness = opt->worst_fitness;

	node* neighbours[3] = {
		n->parent,
		n->left,
		n->right
	};
	
	for (int i = 0; i < 3; i++) {
		if (neighbours[i] == nullptr)
			continue;

		double fitness = opt->evaluator(neighbours[i]->value->best_position());

		if (opt->comparator(fitness, cur_best_fitness))
		{
			cur_best_fitness = fitness;
			best_node = neighbours[i];
		}
	}

	assert(best_node != nullptr);

	return best_node->value->best_position();
}

void heirarchical::end_cycle()
{
	auto opt = optimiser::get_optimiser_raw();
	// start at 1, skip the first stage as it has no parent
	for (size_t s = 1; s < stages.size(); s++)
	{
		for (auto n : stages[s])
		{
			assert(n->value != nullptr);
			assert(n->parent != nullptr);
			assert(n->parent->value != nullptr);

			double selffitness   = opt->evaluator(n->value->best_position());
			double parentfitness = opt->evaluator(n->parent->value->best_position());

			if (opt->comparator(selffitness, parentfitness))
			{
				// swap child and parent

				// only the values need to be swapped, children aren't swapped (?)
				swap(n->value, n->parent->value);
			}
		}
	}
}

shared_ptr<neighbourhood_base> heirarchical_factory(vector<string> args)
{
	return make_shared<heirarchical>();
}

void heirarchical_init()
{
	neighbourhood_base::register_neighbourhood("heirarchical", heirarchical_factory);
}

module_init(heirarchical_init);