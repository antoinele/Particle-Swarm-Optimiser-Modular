#ifndef OPTIMISER_H
#define OPTIMISER_H

#include "psotypes.h"
#include "problem.h"
#include "particle.h"

#include <vector>
#include <functional>
#include <atomic>

using namespace std;

namespace pso {

	class particle;

    class optimiser : public enable_shared_from_this<optimiser>
    {
    private:
		shared_ptr<problem> _problem;
		optimiser(shared_ptr<problem> problem);

        vector<shared_ptr<particle>> particles;
        size_t n_dimensions;

        void connect_neighbourhood(int average_neighbours);

		void do_cycle();
		void evaluate_cycle();

		coordinate g_best;
		double g_best_fitness = numeric_limits<double>::max();

		uint64_t seed;
		atomic_uint seed_count;

    public:
        double evaluator(coordinate position);

		
		// Return true if A is better than B
		bool comparator(double a, double b);

		shared_ptr<pso_rng> thread_rng();

        void add_solution(coordinate position);
        void add_solution(coordinate position, coordinate velocity);

		vector<coordinate> get_solutions();

		void set_seed(const uint64_t seed);

        pair<coordinate, double> best_solution();

		void init_simulation();

		double run_simulation(int n_steps);
		double run_until(double targetfitness, int n_steps);

		void enable_parallel(int n_threads);

		shared_ptr<problem> problem() {
			return _problem;
		}

		static shared_ptr<optimiser> create_optimiser(shared_ptr<pso::problem> problem)
		{
			shared_ptr<optimiser> o(new optimiser(problem));

			return move(o);
		}

		virtual ~optimiser();
    };

}

#endif