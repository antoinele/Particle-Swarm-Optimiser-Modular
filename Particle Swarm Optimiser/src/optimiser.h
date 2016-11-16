#pragma once

#include "psotypes.h"
#include "problem.h"
#include "particle.h"
#include "optimiserlogging.h"

#include <vector>
#include <functional>
#include <atomic>
#include <thread>
#include <mutex>
#include <condition_variable>

using namespace pso;
using namespace std;

namespace pso {

	//class optimiserlogging;

	class particle;

    class optimiser : public enable_shared_from_this<optimiser>
    {
		friend class optimiserlogging;
    private:
		shared_ptr<problem> _problem;
		optimiser(shared_ptr<problem> problem);

        vector<shared_ptr<particle>> particles;
        size_t n_dimensions;

        void connect_neighbourhood(int average_neighbours);

		void do_cycle();
		void do_cycle_mt();
		void evaluate_cycle();
		void evaluate_cycle_mt();

		coordinate g_best;
		double g_best_fitness = numeric_limits<double>::max();

		uint64_t seed;
		atomic_uint seed_count;

		int n_threads = 1;
		vector<thread> threads;
		atomic_int thread_counter;
#ifdef THREADING_USE_MUTEX
		mutex thread_counter_mutex;
		condition_variable thread_counter_cv;
		mutex thread_mutex;
		condition_variable thread_state_cv;
#endif
		enum class thread_state {
			idle = 0,
			move,
			end,
			exit
		}; // thread_state = thread_state::idle;
		atomic<thread_state> thread_state = thread_state::idle;

		void threads_reset();
		inline void threads_move();
		inline void threads_end();
		inline void threads_exit();
		inline void threads_wait();

		shared_ptr<optimiserlogging> _logger;

#pragma region run options
		uint32_t max_cycles = numeric_limits<uint32_t>::max();
		uint32_t max_runtime = numeric_limits<uint32_t>::max();
		double target_fitness;
#pragma endregion

		static void do_move_step(optimiser* op, int thread_n);
		static void do_end_step(optimiser* op, int thread_n);
		static void thread_handler(optimiser* op, int thread_n);

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

		void run_simulation();

		void set_max_cycles(uint32_t max_cycles) { this->max_cycles = max_cycles; }
		void set_max_runtime(uint32_t max_runtime) { this->max_runtime = max_runtime; }
		void set_target_fitness(double target_fitness) { this->target_fitness = target_fitness; }

		void enable_parallel(int n_threads);

		shared_ptr<problem> problem() {
			return _problem;
		}

		shared_ptr<optimiserlogging> logger() {
			return _logger;
		}

		static shared_ptr<optimiser> create_optimiser(shared_ptr<pso::problem> problem)
		{
			shared_ptr<optimiser> o(new optimiser(problem));

			o->_logger = make_shared<optimiserlogging>(o);
			
			return o;
		}

		virtual ~optimiser();
    };

}
