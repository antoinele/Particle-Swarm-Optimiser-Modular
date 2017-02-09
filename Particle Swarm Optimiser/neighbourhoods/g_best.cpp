#include <core/neighbourhood.h>
#include <core/init.h>

#include <core/optimiser.h>

#include <iostream>

using namespace pso;
using namespace std;

class gbest_neighbourhood : public neighbourhood_base {
public:
	gbest_neighbourhood() {};

	VectorXd find_lbest(particle* p);
};

VectorXd gbest_neighbourhood::find_lbest(particle* p)
{
	return optimiser::get_optimiser_raw()->best_solution().first;
}

shared_ptr<neighbourhood_base> gbest_factory(vector<string> args) {
	if (args.size() > 0) {
		cerr
			<< "gbest takes no arguments." << endl;

		exit(1);
	}

	return make_shared<gbest_neighbourhood>();
}

void gbest_init()
{
	neighbourhood_base::register_neighbourhood("gbest", gbest_factory);
}

module_init(gbest_init);