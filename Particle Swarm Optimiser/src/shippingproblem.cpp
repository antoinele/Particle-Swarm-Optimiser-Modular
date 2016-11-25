#include "shippingproblem.h"
#include <utilities.h>

#include <cassert>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdlib>

#include <omp.h>

shippingproblem::shippingproblem(string csvfile) : testdata()
{
	ifstream fh(csvfile);

	if (!fh.is_open())
	{
		cerr << "File not found: " << csvfile << endl;
		exit(1);
	}

	dimensions = 0;

	while (!fh.eof())
	{
		vector<double> linedata;
		string line;

		getline(fh, line);

		if (line.find_first_of('#') != string::npos)
		{
			continue;
		}
		
		stringstream lss(line);
		string cell;

		while (getline(lss, cell, ','))
		{
			double v = atof(cell.c_str());

			linedata.push_back(v);
		}

		if (dimensions == 0)
			dimensions = linedata.size() - 1;
		else if (linedata.size() - 1 != dimensions)
		{
			//invalid line
			continue;
		}

		testdata.push_back(linedata);
	}

	//pso::print_vecvec<double>(&testdata);
}

//double shippingproblem::predict_load(vector<double>::iterator hd_start, vector<double>::iterator hd_end, coordinate* weights)
//{
//	//assert(historical_data->size() == weights->size());
//
//	double load = 0;
//
//	//for (int i = 0; i < historical_data->size(); ++i)
//	for(auto i = hd_start, j = weights->begin(); i != hd_end; ++i, ++j)
//	{
//		load += (*i) * (*j);
//	}
//
//	//load /= historical_data->size();
//
//	return load;
//}

vector<vector<double>> shippingproblem::bounds()
{
	vector<vector<double>> b(dimensions);

	vector<double> ib = { -10.0, 10.0 };

	for (size_t i = 0; i < dimensions; ++i)
	{
		b[i] = ib;
	}

	return b;
}

double shippingproblem::evaluate(coordinate c)
{
	double score(0);

	size_t testdatasize = testdata.size();

	assert(testdatasize > 0);
	assert(testdata[0].size() - 1 == c.size());

	size_t n_samples = testdata[0].size() - 1;
	#pragma omp simd 
	for (size_t i = 0; i < testdatasize; ++i)
	{
		double pload(0);

		// sum of historical data * weights
		for (size_t j=1; j < n_samples; ++j)
		{
			pload += testdata[i][j] * c[j];
		}

		// subtract the actual value to work out wastage
		pload -= testdata[i][0];

		pload = abs(pload);

		score += pload;
	}

	// normalise score to the amount of available data
	score /= testdata.size();

	return score;
}

