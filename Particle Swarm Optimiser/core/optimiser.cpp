#include "optimiser.h"
#include "psotypes.h"
#include "problem.h"
#include "particle.h"
#include <memory>
#include <iterator>
#include <limits>
#include <ctime>
#include <iomanip>
#include <chrono>
#include <iostream>

#if _OPENMP
#include <omp.h>
#endif

using namespace pso;
using namespace std;

#define CYCLE_SIZE 100

optimiser* optimiser::instance_raw = nullptr;

optimiser::optimiser(shared_ptr<problem_base> problem) : _problem(problem)
{
	_n_dimensions = _problem->bounds().size();

	if (comparator(numeric_limits<double>::min(), numeric_limits<double>::max())) {
		target_fitness = numeric_limits<double>::min();
		worst_fitness = numeric_limits<double>::max();
	}
	else {
		target_fitness = numeric_limits<double>::max();
		worst_fitness = numeric_limits<double>::min();
	}

	g_best_fitness = worst_fitness;

	set_seed(time(NULL));
}

optimiser::~optimiser() {
}

shared_ptr<optimiser> optimiser::instance;

void optimiser::add_solution(coordinate position)
{
	auto bounds = _problem->bounds();
	shared_ptr<pso_rng> rng = thread_rng();

    coordinate velocity(bounds.size());
	uniform_real_distribution<double> dist(-1.0, 1.0);
    for (size_t i = 0; i < _n_dimensions; ++i)
    {
		velocity(i) = dist(*rng);
    }

    add_solution(position, velocity);
}

void optimiser::add_solution(coordinate position, coordinate velocity)
{
	shared_ptr<particle> particle = make_shared<pso::particle>(position, velocity);

	if (!_problem->is_valid(position)) {
		cerr << "Invalid solution added" << endl;
		exit(1);
	}

	if (g_best_fitness == numeric_limits<double>::max())
	{
		g_best = particle->position();
		g_best_fitness = particle->evaluate();
	}

	particles.push_back(move(particle));
}

vector<coordinate> pso::optimiser::get_solutions()
{
	vector<coordinate> solutions;

	for (auto p : particles)
	{
		solutions.push_back(p->position());
	}

	return solutions;
}

void pso::optimiser::set_seed(const uint64_t seed)
{
	this->seed = seed;
	seed_count = 0;
}

pair<coordinate, double> optimiser::best_solution()
{
    double best_fitness(g_best_fitness);
    coordinate best_found(g_best);

#pragma omp parallel for
    for(int i=0; i<(int)particles.size(); i++)
    {
		coordinate p = particles[i]->best_position();

		double fitness = evaluator(p);

        if(comparator(fitness, best_fitness))
        {
			best_found = p;
			best_fitness = fitness;
        }
    }

	pair<coordinate, double> result(best_found, best_fitness);
    return result;
}

void optimiser::init_simulation() {
	_neighbourhood->init_neighbourhood();
}

void pso::optimiser::set_neighbourhood(shared_ptr<neighbourhood_base> neighbourhood)
{
	assert(!_neighbourhood);

	_neighbourhood = neighbourhood;
}

void pso::optimiser::run_simulation()
{
	pause = false;
	auto starttime = chrono::steady_clock().now();
	uint32_t cyclecount = 0;

	while (1) {
		for (int i = 0; i < CYCLE_SIZE; i++)
		{
			do_cycle();
		}

		if (logger != nullptr)
			logger->dorecord(cyclecount);

		auto curtime = chrono::steady_clock().now();
		cyclecount += CYCLE_SIZE;

		// stop if the target fitness has been reached
		if (target_fitness < numeric_limits<double>::max() && comparator(g_best_fitness, target_fitness)) {
			break;
		}

		// stop if the max time has been reached
		chrono::duration<double> elapsed_time = chrono::duration_cast<chrono::duration<double>>(curtime - starttime);
		if (elapsed_time.count() >= max_runtime) {
			break;
		}

		// stop if the maximum number of cycles has been reached
		if (cyclecount >= max_cycles) {
			break;
		}

		if (pause) {
			break;
		}
	}

	auto endtime = chrono::steady_clock().now();
	chrono::duration<double> runtime = chrono::duration_cast<chrono::duration<double>>(endtime - starttime);

	cerr << "Cycles taken: " << cyclecount << endl;
	cerr << "Total samples (cycles*particles): " << cyclecount * particles.size() << endl;
	cerr << "Total run time: " << runtime.count() << " seconds" << endl;
	cerr << "Cycles/s: " << cyclecount / runtime.count() << endl;
	cerr << "Samples/s: " << (cyclecount * particles.size()) / runtime.count() << endl;
}

void pso::optimiser::stop_simulation()
{
	pause = true;
}

void optimiser::enable_parallel(int parallel_jobs)
{
	n_threads = parallel_jobs;
/*
#if _OPENMP
	omp_set_dynamic(0);
	omp_set_num_threads(n_threads);

	omp_set_nested(1);
#endif
*/
}

void pso::optimiser::set_logger(optimiserlogging * logger)
{
	this->logger = logger;
}

void optimiser::do_cycle()
{
#pragma omp parallel for num_threads(n_threads)
	for (int i = 0; i < (int)particles.size(); ++i)
	{
		particles[i]->move_step();
	}

#pragma omp parallel for num_threads(n_threads)
	for (int i = 0; i < (int)particles.size(); ++i)
	{
		particles[i]->end_step();
	}

	_neighbourhood->end_cycle();

	evaluate_cycle_mt();
}

void pso::optimiser::evaluate_cycle_mt()
{
#pragma omp parallel for num_threads(n_threads)
	for (int i = 0; i<(int)particles.size(); i++)
	{ // i must be a signed integer for compatibility with VS2015/OMP2.0
		coordinate p = particles[i]->best_position();

		double fitness = evaluator(p);

#pragma omp critical 
		{
			if (comparator(fitness, g_best_fitness))
			{
				g_best = p;
				g_best_fitness = fitness;
			}
		}
	}
}

shared_ptr<pso_rng> pso::optimiser::thread_rng()
{
	static thread_local shared_ptr<pso_rng> rng;

	if (!rng) {
		rng = make_shared<pso_rng>(seed);

		int count = seed_count.fetch_add(1);

		rng->discard(count);

		uniform_int_distribution<uint32_t> dist;

		rng->seed(dist(*rng));
	}

	return rng;
}
