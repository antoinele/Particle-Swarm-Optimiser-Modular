#pragma once

#include "psotypes.h"
#include "problem.h"
#include "optimiserlogging.h"

#include <vector>
#include <functional>
#include <atomic>
#include <thread>
#include <mutex>
#include <condition_variable>

using namespace std;

namespace pso {

	//class optimiserlogging;

	class particle;

    class optimiser : public enable_shared_from_this<optimiser>
    {
		friend class optimiserlogging;
		friend class particle;
    private:
		shared_ptr<problem_base> _problem;
		optimiser(shared_ptr<problem_base> problem);

        vector<shared_ptr<particle>> particles;
        size_t n_dimensions;

        void connect_neighbourhood(size_t average_neighbours);

		void do_cycle();
		void evaluate_cycle_mt();

		coordinate g_best;
		double g_best_fitness = numeric_limits<double>::max();

		uint64_t seed;
		atomic_uint seed_count;

		int n_threads = 1;
		int neighbourhood_size = 3;

		optimiserlogging* logger = nullptr;

#pragma region run options
		uint32_t max_cycles = numeric_limits<uint32_t>::max();
		uint32_t max_runtime = numeric_limits<uint32_t>::max();
		double target_fitness;
#pragma endregion

    public:
		inline double evaluator(coordinate position)
		{
			return _problem->evaluate(position);
		}
		// Return true if A is better than B
		inline bool comparator(double a, double b)
		{
			return _problem->comparator(a, b);
		}

		shared_ptr<pso_rng> thread_rng();

        void add_solution(coordinate position);
        void add_solution(coordinate position, coordinate velocity);

		vector<coordinate> get_solutions();

		void set_seed(const uint64_t seed);

        pair<coordinate, double> best_solution();

		void init_simulation();

		void run_simulation();

		void set_max_cycles(uint32_t max_cycles) { this->max_cycles = max_cycles; }
		void set_max_runtime(uint32_t max_runtime) { this->max_runtime = max_runtime; }
		void set_target_fitness(double target_fitness) { this->target_fitness = target_fitness; }

		void enable_parallel(int n_threads);
		void set_neighbourhood_size(int neighbourhood_size);

		void set_logger(optimiserlogging* logger)
		{
			this->logger = logger;
		}

		shared_ptr<problem_base> problem() {
			return _problem;
		}

		static shared_ptr<optimiser> create_optimiser(shared_ptr<pso::problem_base> problem)
		{
			shared_ptr<optimiser> o(new optimiser(problem));

			return o;
		}

		virtual ~optimiser();
    };

}
