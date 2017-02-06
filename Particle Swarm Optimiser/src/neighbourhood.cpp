#include "neighbourhood.h"

using namespace pso;

pso::neighbourhood_base::neighbourhood_base()
{
}


pso::neighbourhood_base::~neighbourhood_base()
{
}

void pso::neighbourhood_base::register_neighbourhood(string name, neighbourhoodfactory factory)
{
	assert(registered_problems.count(name) == 0);

	registered_neighbourhoods[name] = factory;
}
