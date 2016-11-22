#include <vector>
#include <random>
#include <chrono>
#include <memory>
#include <cmath>
#include <cstdlib>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <fstream>

#include "shippingproblem.h"
#include "psotypes.h"
#include "optimiser.h"

using namespace std;

void print_help(string programname) {
	cerr
		<< "Usage: " << programname << " [-options] <testdata.csv>" << endl
		<< endl
		<< "Options:" << endl
		<< "  -solutions <n>     Number of solutions (particles) to use" << endl
		<< "  -seed <n>          Seed for random number generator" << endl
		<< "  -wait              Pause after simulation is finished" << endl
		<< "  -maxcycles <n>     If used with -targetfitness it will limit the numer of cycles the" << endl
		<< "                     simulation will run for. Otherwise it will be the absolute number" << endl
		<< "                     of cycles to run for." << endl
		<< "  -targetfitness <d> The fitness the program should run until" << endl
		<< "  -maxruntime <n>    The time, in seconds, the program should run for until stopping." << endl
		<< "                     When combined with -maxcycles or -targetfitness the first event" << endl
		<< "                     will stop the program. Set to 0 for infinite runtime. Default is" << endl
		<< "                     30s." << endl
		<< "  -logfile <csv>	 Write statistics to the specified file. Specify `-` for stdout." << endl
		<< "  -threads <n>       The number of threads to run in parallel. Default: 1." << endl
		<< "  -neighbourhood <n> Set the average size of the neighbourhood. 0 means use g_best." << endl
		<< "                     Default: 3." << endl;
		
}

int main(int argc, char const *argv[])
{
	// Defaults
	double target_fitness = numeric_limits<double>::max();
	int num_solutions = 10;
	int max_cycles = -1;
	int max_runtime = -1;
	int n_threads = 1;
	int neighbourhood_size = 1;
	unsigned int seed = static_cast<unsigned int>(chrono::system_clock::now().time_since_epoch().count());
	bool wait_after_end = false;
	string csvfile;
	string logfile;

	shared_ptr<problem_base> problem;

	// Parse arguments
	for (int i = 1; i < argc; i++)
	{
		if (argv[i][0] == '-')
		{
			if (strcmp(argv[i], "-solutions") == 0)
			{
				num_solutions = atoi(argv[++i]);
			}
			else if (strcmp(argv[i], "-seed") == 0)
			{
				seed = atoi(argv[++i]);
			}
			else if (strcmp(argv[i], "-wait") == 0)
			{
				wait_after_end = true;
			}
			else if (strcmp(argv[i], "-maxcycles") == 0)
			{
				max_cycles = atoi(argv[++i]);
			}
			else if (strcmp(argv[i], "-targetfitness") == 0)
			{
				target_fitness = atof(argv[++i]);
			}
			else if (strcmp(argv[i], "-maxruntime") == 0)
			{
				max_runtime = atoi(argv[++i]);
			}
			else if (strcmp(argv[i], "-logfile") == 0)
			{
				logfile = argv[++i];
			}
			else if (strcmp(argv[i], "-threads") == 0)
			{
				n_threads = atoi(argv[++i]);
			}
			else if (strcmp(argv[i], "-neighbourhood") == 0)
			{
				neighbourhood_size = atoi(argv[++i]);
			}
			else {
				cerr << "Unknown argument: " << argv[i] << endl;
				print_help(argv[0]);
				exit(1);
			}
		}
		else
		{
			csvfile = argv[i];
		}
	}

	{	// Set defaults
		if (max_cycles == -1 && max_runtime == -1) {
			max_runtime = 30; //default 30s runtime
		}

		if (max_cycles == -1) {
			max_cycles = numeric_limits<int>::max();
		}

		if (max_runtime <= 0) {
			max_runtime = numeric_limits<int>::max();
		}
	}

	{	// Set up problem
		problem = make_shared<shippingproblem>(csvfile);
	}

	// Configure optimiser
    shared_ptr<pso::optimiser> optimiser(pso::optimiser::create_optimiser(problem));
	optimiser->set_seed(seed);
	optimiser->enable_parallel(n_threads);
	optimiser->set_neighbourhood_size(neighbourhood_size);

	{	// Create initial solutions
		vector<coordinate> solutions(generate_solutions(problem.get(), num_solutions, seed));

		//print_solutions(problem.get(), &solutions);

		for (auto s : solutions)
		{
			assert(problem->is_valid(s));
			optimiser->add_solution(s);
		}
	}

	// Set up simulation
	optimiser->set_max_cycles(max_cycles);
	optimiser->set_max_runtime(max_runtime);
	optimiser->set_target_fitness(target_fitness);

	// Loop to optionally continue after completion
	while (1) {
		optimiser->run_simulation();

		// Get best solution
		auto c = optimiser->best_solution();

		// Print best solution
		cerr << "Best solution: " << coordinateToString(&c.first) << endl;

		cerr << "Fitness: " << c.second << endl;

		if (!optimiser->problem()->is_valid(c.first))
		{
			cerr << "WARNING! Solution is invalid!" << endl;
		}

		//print_solutions(pt, solutions);

		//optimiser.reset();

		if (!wait_after_end) {
			goto exit;
		}

		while (1) {
			cerr << "Continue? y/N" << endl;
			string input;
			getline(cin, input);
			
			if (input.size() == 0 || input[0] == 'n' || input[0] == 'N') {
				goto exit;
			}
			else if (input[0] == 'y' || input[0] == 'Y') {
				goto continuedone;
			}
		}
	continuedone: ;
	}
exit:

	if (!logfile.empty()) {
		if (logfile.compare("-") == 0) {
			optimiser->logger()->writeout(&cout);
		}
		else {
			ofstream logfile(csvfile.c_str(), ofstream::trunc);
			if (logfile.is_open()) {
				optimiser->logger()->writeout(&logfile);
			}
			logfile.close();
		}
	}
	
    return 0;
}