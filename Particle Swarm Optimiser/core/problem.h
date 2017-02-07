#pragma once

#include <core/psotypes.h>

#include <cassert>
#include <string>
#include <memory>
#include <map>

namespace {
	using namespace std;
}

namespace pso {

	class problem_base;

	typedef shared_ptr<pso::problem_base>(*problemfactory)(vector<string> args);

	class problem_base {
	protected:
		problem_base() {};
	public:
		virtual vector<vector<double>> bounds() = 0;
		virtual bool is_valid(coordinate c) {
			// default behaviour is to check each dimension is within bounds
			auto b = bounds();
			size_t n_dimensions(b.size());
			assert(c.size() == n_dimensions);

			for (size_t i = 0; i < n_dimensions; i++)
			{
				if (c[i] < b[i][0] || c[i] > b[i][1])
					return false;
			}

			return true;
		};

		virtual double evaluate(const coordinate&) = 0;

		/**
		 * Returning true means that a is better than b
		 * Returning false means that b is better than a
		 */
		virtual bool comparator(const double a, const double b) = 0;

		static std::map<string, problemfactory> registered_problems;
		static void register_problem(string name, problemfactory factory)
		{
			assert(registered_problems.count(name) == 0);

			registered_problems[name] = factory;
		}
	};
}