#include <core/psotypes.h>
#include <core/optimiser.h>
#include <core/utilities.h>

#include <core/init.h>
#include <core/problem.h>
#include <core/neighbourhood.h>

#include <iostream>
#include <fstream>
#include <csignal>
#include <map>

/**
 * Modules markers
 */
#ifdef __GNUC__
extern const initcall_t __start_initcalls[], __stop_initcalls[];
#elif _MSC_VER
__declspec(allocate(".initmod$a")) initcall_t __start_initcalls_seg;
__declspec(allocate(".initmod$z")) initcall_t __stop_initcalls_seg;

#define __start_initcalls &__start_initcalls_seg
#define __stop_initcalls &__stop_initcalls_seg
#endif

using namespace std;
using namespace pso;

void print_help(string programname) {
	cerr
		<< "Usage: " << programname << " [-options] -N neighbourhood [-options] -P problem [-options]" << endl
		<< endl
		<< "Options:" << endl
		<< "  -h                 Prints this help message" << endl
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
#if _OPENMP
		<< "  -threads <n>       The number of threads to run in parallel. Default: 1." << endl
#endif
		;
}

// global optimiser pointer for signal handler
shared_ptr<pso::optimiser> opt;
bool sigintraised = false;

void signalhandler(int sig)
{
	if (sig == SIGINT) {
		sigintraised = true;
		cerr << "SIGINT Caught. " << endl;
		opt->stop_simulation();
	}
}

void do_initcalls() {
	vector<initcall_t> initcalls(__start_initcalls, __stop_initcalls);

	for(initcall_t fn : initcalls)
	{
		/**
		 * Windows executable sections are 256 byte aligned and zero padded
		 * meaning most of the pointers are 0, so check if the pointer
		 * isn't 0 before calling.
		 */
#ifdef _MSC_VER
		if(fn != (initcall_t)0)
#endif
			fn();
	}
}

int main(int argc, char const *argv[])
{
	// Defaults
	double target_fitness = numeric_limits<double>::max();
	int num_solutions = 10;
	int max_cycles = -1;
	int max_runtime = -1;
#if _OPENMP
	int n_threads = 1;
#endif
	unsigned int seed = 0;
	bool wait_after_end = false;
	string logfile;

	string neighbourhood_name;
	string problem_name;

	vector<string> neighbourhood_args;
	vector<string> problem_args;

	if (argc == 1) {
		print_help(argv[0]);
		exit(0);
	}
	
	{
		enum argparsestate {
			NORMAL, NEIGHBOURHOOD, PROBLEM
		} argparsestate = NORMAL;
		int i = 0;
	// Parse normal arguments
		for (i = 1; i < argc; i++)
		{
			if (argparsestate == NORMAL) {
				if (strcmp(argv[i], "-h") == 0)
				{
					print_help(argv[0]);
					exit(0);
				}
				else if (strcmp(argv[i], "-solutions") == 0)
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
#if _OPENMP
				else if (strcmp(argv[i], "-threads") == 0)
				{
					n_threads = atoi(argv[++i]);
				}
#endif
				else if (strcmp(argv[i], "-N") == 0)
				{
					argparsestate = NEIGHBOURHOOD;
					i++;
					if(i < argc)
						neighbourhood_name = argv[i];
				}
				else if (strcmp(argv[i], "-P") == 0)
				{
					argparsestate = PROBLEM;
					i++;
					if(i < argc)
						problem_name = argv[i];
				}
				else {
					cerr << "Unknown argument: " << argv[i] << endl;
					print_help(argv[0]);
					exit(1);
				}
			}
			else if (argparsestate == NEIGHBOURHOOD) {
				if (strcmp(argv[i], "-P") == 0)
				{
					argparsestate = PROBLEM;
					i++;
					if(i < argc)
						problem_name = argv[i];
				}
				else
				{
					neighbourhood_args.push_back(argv[i]);
				}
			}
			else if (argparsestate == PROBLEM) {
				if (strcmp(argv[i], "-N") == 0)
				{
					argparsestate = NEIGHBOURHOOD;
					i++;
					if(i < argc)
						neighbourhood_name = argv[i];
				}
				else
				{
					problem_args.push_back(argv[i]);
				}
			}
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

	do_initcalls();


	shared_ptr<problem_base> problem;
	{	// Set up problem
		if (problem_base::registered_problems.count(problem_name) == 0)
		{
			cerr << "Problem not found: " << problem_name << endl;

			cerr << "Available problems:" << endl;

			for (auto const& p : problem_base::registered_problems)
			{
				cerr << "  " << p.first << endl;
			}

			return 1;
		}

		problemfactory pf = problem_base::registered_problems[problem_name];
		problem = pf(problem_args);
	}

	shared_ptr<neighbourhood_base> neighbourhood;
	{
		if (neighbourhood_base::registered_neighbourhoods.count(neighbourhood_name) == 0)
		{
			cerr << "Neighbourhood not found: " << neighbourhood_name << endl;

			cerr << "Available neighbourhoods:" << endl;

			for (auto const& p : neighbourhood_base::registered_neighbourhoods)
			{
				cerr << "  " << p.first << endl;
			}
			return 1;
		}

		neighbourhoodfactory nf = neighbourhood_base::registered_neighbourhoods[neighbourhood_name];
		neighbourhood = nf(neighbourhood_args);
	}

	// Configure opt
	optimiser::init_optimiser(problem);
    opt = optimiser::get_optimiser();
	opt->set_neighbourhood(neighbourhood);
	optimiserlogging logger(opt);
	if(seed) opt->set_seed(seed);
#if _OPENMP
	opt->enable_parallel(n_threads);
#endif

	if (!logfile.empty()) {
		opt->set_logger(&logger);
	}

	{	// Create initial solutions
		vector<coordinate> solutions(generate_solutions(problem.get(), num_solutions, seed));

		//print_solutions(problem.get(), &solutions);

		for (auto s : solutions)
		{
			assert(problem->is_valid(s));
			opt->add_solution(s);
		}
	}

	// Set up simulation
	opt->set_max_cycles(max_cycles);
	opt->set_max_runtime(max_runtime);
	opt->set_target_fitness(target_fitness);

	opt->init_simulation();

	// Loop to optionally continue after completion
	while (1) {
		// Enable signal handler to pause optimiser
		signal(SIGINT, &signalhandler);

		opt->run_simulation();

		// Get best solution
		auto c = opt->best_solution();

		// Print best solution
		cerr << "Best solution: " << coordinateToString(&c.first) << endl;

		cerr << "Fitness: " << c.second << endl;

#ifdef DEBUG
		if (!opt->problem()->is_valid(c.first))
		{
			cerr << "WARNING! Solution is invalid!" << endl;
		}
#endif

		// if -wait isn't specified, exit
		if (!sigintraised && !wait_after_end) {
			goto exit;
		}

		// Disable signal handler until the optimiser is to continue
		signal(SIGINT, SIG_DFL);

		while (1) {
			cerr << "Continue? Y/n" << endl;
			char in = cin.get();
			//cin.read(&in, 1);
			
			if (in == '\r' || in == '\n') { // default
				goto continuedone;
			}
			else if(in == 'n' || in == 'N') {
				goto exit;
			}
			else if (in == 'y' || in == 'Y') {
				goto continuedone;
			}
		}
	continuedone: ;
	}
exit:

	if (!logfile.empty()) {
		if (logfile.compare("-") == 0) {
			logger.writeout(&cout);
		}
		else {
			ofstream ofs(logfile.c_str(), ofstream::trunc);
			if (ofs.is_open()) {
				logger.writeout(&ofs);
			}
			ofs.close();
		}
	}
	
    return 0;
}
