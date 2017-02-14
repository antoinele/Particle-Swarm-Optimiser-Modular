#pragma once

#include <core/psotypes.h>
#include <core/problem.h>
#include <core/neighbourhood.h>
#include <core/optimiserlogging.h>

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
		static optimiser* instance_raw;
		shared_ptr<problem_base> _problem;
		shared_ptr<neighbourhood_base> _neighbourhood;
		optimiser(shared_ptr<problem_base> problem);

        vector<shared_ptr<particle>> particles;
        size_t _n_dimensions;

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
		
		bool pause = false;

    public:
		double target_fitness;
		double worst_fitness;

		inline vector<shared_ptr<particle>>* get_particles() {
			return &particles;
		}

		inline size_t n_dimensions() {
			return _n_dimensions;
		}

		inline double evaluator(coordinate position)
		{
			assert(position.size() == _problem->bounds().size());
			return _problem->evaluate(position);
		}
		// Return true if A is better than B
		static inline bool comparator(const double a, const double b)
		{
			return a < b;
		}

		shared_ptr<pso_rng> thread_rng();

        void add_solution(coordinate position);
        void add_solution(coordinate position, coordinate velocity);

		vector<coordinate> get_solutions();

		void set_seed(const uint64_t seed);
		uint64_t get_seed();

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

		static void init_optimiser(shared_ptr<problem_base> problem)
		{
			instance_raw = new optimiser(problem);
			instance = shared_ptr<optimiser>(instance_raw);
		}

		static inline shared_ptr<optimiser> get_optimiser()
		{
			assert(instance);

			return instance;
		}

		static inline optimiser* get_optimiser_raw()
		{
			// not sure if this will work as instance_raw isn't initialised
			assert(instance_raw != nullptr);

			return instance_raw;
		}

		virtual ~optimiser();
    };

}
