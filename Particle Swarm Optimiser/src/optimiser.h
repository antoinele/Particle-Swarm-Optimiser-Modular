#pragma once

#include <psotypes.h>
#include <problem.h>
#include <neighbourhood.h>
#include <optimiserlogging.h>

#include <vector>
#include <atomic>

namespace {
	using namespace std;
}

namespace pso {

	class particle;

    class optimiser : public enable_shared_from_this<optimiser>
    {
		friend class optimiserlogging;
		friend class particle;
    private:
		static shared_ptr<optimiser> instance;
		shared_ptr<problem_base> _problem;
		shared_ptr<neighbourhood_base> _neighbourhood;
		optimiser();

        vector<shared_ptr<particle>> particles;
        size_t n_dimensions;

        void do_cycle();
		void evaluate_cycle_mt();

		coordinate g_best;
		double g_best_fitness;

		uint64_t seed;
		atomic_uint seed_count;

		int n_threads = 1;

		optimiserlogging* logger = nullptr;

		uint32_t max_cycles = numeric_limits<uint32_t>::max();
		uint32_t max_runtime = numeric_limits<uint32_t>::max();
		double target_fitness;
		double worst_fitness;

		bool pause = false;

    public:
		inline double evaluator(coordinate position)
		{
			assert(position.size() == _problem->bounds().size());
			return _problem->evaluate(position);
		}
		// Return true if A is better than B
		inline bool comparator(const double a, const double b)
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

		void set_neighbourhood(shared_ptr<neighbourhood_base> neighbourhood);

		void run_simulation();
		void stop_simulation();

		void set_max_cycles(uint32_t max_cycles) { this->max_cycles = max_cycles; }
		void set_max_runtime(uint32_t max_runtime) { this->max_runtime = max_runtime; }
		void set_target_fitness(double target_fitness) { this->target_fitness = target_fitness; }

		void enable_parallel(int n_threads);

		void set_logger(optimiserlogging* logger);

		inline shared_ptr<problem_base> problem() {
			return _problem;
		}

		void set_problem(shared_ptr<problem_base> problem);

		static shared_ptr<optimiser> get_optimiser()
		{
			if(!instance)
				instance = shared_ptr<optimiser>(new optimiser());
			
			return instance;
		}

		virtual ~optimiser();
    };

}
