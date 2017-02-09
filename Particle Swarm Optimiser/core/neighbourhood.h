#pragma once

#include <core/psotypes.h>

#include <Eigen/Dense>
#include <string>
#include <memory>
#include <map>

namespace {
	using namespace std;
	using namespace Eigen;
	using namespace pso;
}

namespace pso {

	class neighbourhood_base;
	class particle;

	typedef shared_ptr<neighbourhood_base>(*neighbourhoodfactory)(vector<string> args);

	class neighbourhood_base
	{
	protected:
		neighbourhood_base() {};
	public:
		virtual void init_neighbourhood() {};
		virtual VectorXd find_lbest(particle* p) = 0;
		virtual void end_cycle() {};

		static map<string, neighbourhoodfactory> registered_neighbourhoods;
		static void register_neighbourhood(string name, neighbourhoodfactory factory);
	};
}