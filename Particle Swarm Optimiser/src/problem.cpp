#include "problem.h"

using namespace std;

void pso::problem_base::register_problem(string name, problemfactory factory)
{
	assert(registered_problems.count(name) == 0);
		
	registered_problems[name] = factory;
}