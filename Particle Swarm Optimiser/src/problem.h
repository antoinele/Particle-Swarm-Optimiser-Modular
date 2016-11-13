#pragma once

#include <psotypes.h>
#include <cassert>

namespace pso {

	class problem {
	public:
		virtual vector<vector<double>> bounds() = 0;
		bool is_valid(coordinate c) {
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

		virtual double evaluate(coordinate) = 0;

		/**
		 * Returning true means that a is better than b
		 * Returning false means that b is better than a
		 */
		virtual bool comparator(double a, double b) = 0;
	};

}