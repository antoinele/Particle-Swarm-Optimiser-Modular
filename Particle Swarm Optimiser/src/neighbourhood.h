#pragma once

#include <psotypes.h>
#include <string>
#include <memory>
#include <map>

#include <particle.h>

namespace pso {

	class neighbourhood_base;

	typedef shared_ptr<pso::neighbourhood_base>(*neighbourhoodfactory)(vector<string> args);

	class neighbourhood_base
	{
	public:
		neighbourhood_base();
		virtual ~neighbourhood_base();

		virtual void init_neighbourhood() = 0;
		virtual VectorXd find_lbest(particle* p) = 0;

		static std::map<string, neighbourhoodfactory> registered_neighbourhoods;
		static void register_neighbourhood(string name, neighbourhoodfactory factory);
	};
}