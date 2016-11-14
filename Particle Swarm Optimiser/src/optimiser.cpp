#include "optimiser.h"
#include "psotypes.h"
#include "problem.h"
#include <memory>
#include <iterator>
#include <limits>
#include <vector>
#include <ctime>
#include <iomanip>
#include <chrono>
#include <iostream>
#include <thread>

using namespace pso;
using namespace std;

#define CYCLE_SIZE 100

optimiser::optimiser(shared_ptr<pso::problem> problem) 
                     : _problem(problem)
{
	n_dimensions = _problem->bounds().size();
	
	logger = make_shared<optimiserlogging>();
	logger->setoptimiser(shared_from_this());

	if (comparator(numeric_limits<double>::min(), numeric_limits<double>::max()))
		target_fitness = numeric_limits<double>::min();
	else
		target_fitness = numeric_limits<double>::max();

	set_seed(time(NULL));
}

optimiser::~optimiser() {
	//double maxvel = 0, minvel = 0;

	//for (auto p : particles)
	//{
	//	auto pv(p->velocity());

	//	double v = 0;

	//	for (double vx : pv)
	//	{
	//		v += vx * vx;
	//	}

	//	v = sqrt(v);

	//	if (v > maxvel)
	//		maxvel = v;

	//	if (v < minvel)
	//		minvel = v;
	//}

	//cerr << "Max vel: " << maxvel << ", Min vel: " << minvel << endl;
}

double optimiser::evaluator(coordinate position)
{
	return _problem->evaluate(position);
}

inline bool optimiser::comparator(double a, double b)
{
	return _problem->comparator(a, b);
}

void optimiser::add_solution(coordinate position)
{
	auto bounds = _problem->bounds();
	shared_ptr<pso_rng> rng = thread_rng();

    coordinate velocity;
    for (int i = 0; i < n_dimensions; ++i)
    {
		uniform_real_distribution<double> dist(bounds[i][0], bounds[i][1]);
        velocity.push_back(dist(*rng));
    }

    add_solution(position, velocity);
}

void optimiser::add_solution(coordinate position, coordinate velocity)
{
    shared_ptr<particle> particle(new particle(shared_from_this(), position, velocity));

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

    for(auto particle : particles)
    {
		coordinate p = particle->best_position();

		double fitness = evaluator(p);

		//double fitness = particle->evaluate();
        if(comparator(fitness, best_fitness))
        {
            //best_found = particle->position();
			best_found = p;
			best_fitness = fitness;
        }
    }

	pair<coordinate, double> result(best_found, best_fitness);
    return result;
}

void optimiser::init_simulation() {
	static bool initialised = false;
	if (!initialised) {
		connect_neighbourhood(3);
		initialised = true;
	}
}

void pso::optimiser::run_simulation()
{
	auto starttime = chrono::steady_clock().now();
	uint32_t cyclecount = 0;

	while (1) {
		for (int i = 0; i < CYCLE_SIZE; i++)
		{
			do_cycle();
		}

		logger->dorecord(cyclecount);

		double curfitness = best_solution().second;
		auto curtime = chrono::steady_clock().now();
		cyclecount += CYCLE_SIZE;

		// stop if the target fitness has been reached
		if (target_fitness < numeric_limits<double>::max() && comparator(curfitness, target_fitness)) {
			//cerr << "Hit target fitness." << endl;
			break;
		}

		// stop if the max time has been reached
		chrono::duration<double> elapsed_time = chrono::duration_cast<chrono::duration<double>>(curtime - starttime);
		if (elapsed_time.count() >= max_runtime) {
			//cerr << "Hit max runtime." << endl;
			break;
		}

		// stop if the maximum number of cycles has been reached
		if (cyclecount >= max_cycles) {
			break;
		}
	}

	auto endtime = chrono::steady_clock().now();
	chrono::duration<double> runtime = chrono::duration_cast<chrono::duration<double>>(endtime - starttime);

	cerr << "Cycles taken: " << cyclecount << endl;
	cerr << "Total run time: " << runtime.count() << " seconds" << endl;
}

void optimiser::enable_parallel(int parallel_jobs)
{
	n_threads = parallel_jobs;
}

void optimiser::connect_neighbourhood(int average_neighbours)
{
	uniform_int_distribution<int> dist(0, (int)particles.size() - 1);

	auto rng = thread_rng();

	for (auto p : particles)
	{
		for (int i = 0; i < average_neighbours; i++)
		{
			int stuck = 0;

			while (true) {
				int newindex = dist(*rng);
				shared_ptr<particle> nextparticle = particles[newindex];
				auto* neighbours = &nextparticle->neighbours;

				if (find(neighbours->begin(), neighbours->end(), nextparticle.get()) != neighbours->end() ||
					nextparticle->neighbours.size() > average_neighbours)
				{
					if (stuck > particles.size() * 0.9f) {
						cerr << "Couldn't pick neighbours" << endl;
						break;
					}

					stuck++;
					continue;
				}
				
				p->neighbours.push_back(nextparticle.get());
				break;
			}
		}
	}
}

void calcRanges(int &min, int &max, int n_threads, int n_particles, int thread_n)
{
	int ppt = n_particles + 1 / n_threads;
	min = ppt * thread_n;
	max = min + ppt - 1;

	if (thread_n == n_threads - 1)
	{
		max = n_particles - 1;
	}
}

void optimiser::do_cycle()
{
	vector<unique_ptr<thread>> threads(n_threads);
	for (int n = 0; n < n_threads; n++)
	{
		unique_ptr<thread> t = make_unique<thread>([this](int thread_n) {
			int min, max;
			
			calcRanges(min, max, n_threads, (int)particles.size(), thread_n);

			for (int i = min; i <= max; ++i)
			{
				particles[i]->move_step();
			}
		}, n);

		threads.push_back(move(t));
	}

	for (int n = 0; n < n_threads; n++)
	{
		threads[n]->join();
	}

	threads.clear();

	for (int n = 0; n < n_threads; n++)
	{
		unique_ptr<thread> t = make_unique<thread>([this](int thread_n) {
			int min, max;

			calcRanges(min, max, n_threads, (int)particles.size(), thread_n);

			for (int i = min; i <= max; ++i)
			{
				particles[i]->end_step();
			}
		}, n);

		threads.push_back(move(t));
	}

	for (int n = 0; n < n_threads; n++)
	{
		threads[n]->join();
	}
	/*#pragma loop(hint_parallel(8))
	for (int i = 0; i < particles.size(); ++i)
	{
		particles[i]->move_step();
	}

	#pragma loop(hint_parallel(8))
	for (int i = 0; i < particles.size(); ++i)
	{
		particles[i]->end_step();
	}*/

	/*for (auto particle : particles)
	{
		particle->move_step();
	}

	for (auto particle : particles)
	{
		particle->end_step();
	}*/

	evaluate_cycle();
}

void optimiser::evaluate_cycle()
{
	auto bs = best_solution();

	double curfitness = bs.second;

	if (comparator(curfitness, g_best_fitness))
	{
		g_best = bs.first;
		g_best_fitness = bs.second;
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
