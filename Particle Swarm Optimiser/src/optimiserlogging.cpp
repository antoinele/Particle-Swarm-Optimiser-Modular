#include "optimiserlogging.h"
#include "optimiser.h"

#include <iostream>
#include <fstream>

using namespace pso;
using namespace std;

optimiserlogging::optimiserlogging(weak_ptr<optimiser> optimiser) : op(optimiser)
{
	//op = optimiser;
}

optimiserlogging::~optimiserlogging()
{
}

//void optimiserlogging::setoptimiser(weak_ptr<optimiser> optimiser)
//{
//	op = move(optimiser);
//}

void optimiserlogging::dorecord(uint32_t cycle)
{
	uint32_t realcount = count_offset + cycle;
	if (realcount < prev_count)
	{
		count_offset = prev_count;
	}
	prev_count = realcount;

	bool set = false;
	double best, avg = 0, worst;
	
	auto op2 = op.lock();

	for (auto p : op2->particles)
	{
		double fitness = op2->problem()->evaluate(p->position());
		if (!set)
		{
			best = worst = fitness;
		}
		else
		{
			if (op2->comparator(fitness, best))
			{
				best = fitness;
			}

			if (op2->comparator(worst, fitness))
			{
				worst = fitness;
			}
		}

		avg += fitness;
	}

	avg /= op2->particles.size();

	record r;
	r.cycle = realcount;
	r.g_best = op2->g_best_fitness;
	r.best = best;
	r.average = avg;
	r.worst = worst;

	records.push_back(r);
}

void pso::optimiserlogging::writeout(ostream* logfile)
{
	*logfile << "cycle,g_best,best,average,worst" << endl;

	for (auto r : records)
	{
		*logfile
			<< r.cycle << ","
			<< r.g_best << ","
			<< r.best << ","
			<< r.average << ","
			<< r.worst << ","
			<< endl;
	}

	//*logfile.close();
}