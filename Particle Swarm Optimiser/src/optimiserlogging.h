#pragma once

#include <memory>
#include <list>

//using namespace pso;
using namespace std;

namespace pso {
	class optimiser;

	class optimiserlogging
	{
	public:
		struct record {
			uint32_t cycle;
			double best;
			double average;
			double worst;
		};

	private:
		weak_ptr<pso::optimiser> op;
		list<record> records;
		uint32_t prev_count = 0;
		uint32_t count_offset = 0;
	public:
		optimiserlogging(weak_ptr<optimiser> optimiser);
		~optimiserlogging();

		//void setoptimiser(weak_ptr<pso::optimiser> optimiser);

		void dorecord(uint32_t cycle);

		void writeout(string csvfile);
	};

}