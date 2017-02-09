#include "neighbourhood.h"

using namespace pso;

void pso::neighbourhood_base::register_neighbourhood(string name, neighbourhoodfactory factory)
{
	assert(registered_neighbourhoods.count(name) == 0);

	registered_neighbourhoods[name] = factory;
}

map<string, neighbourhoodfactory> neighbourhood_base::registered_neighbourhoods;