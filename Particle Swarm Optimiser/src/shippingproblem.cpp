#include "shippingproblem.h"
#include <utilities.h>

#include <cassert>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdlib>

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
	vector<vector<double>> b;

	for (int i = 0; i < dimensions; ++i)
	{
		vector<double> ib = { 0.0, 1.0 };
		b.push_back(ib);
	}

	return b;
}

//bool shippingproblem::is_valid(coordinate c)
//{
//	return true;
//}

double shippingproblem::evaluate(coordinate c)
{
	double score(0);
	
	size_t testdatasize = testdata.size();

	assert(testdatasize > 0);
	assert(testdata[0].size() - 1 == c.size());

	for (int i = 0; i < testdatasize; ++i)
	{
		auto first = testdata[i].begin() + 1,
			 last = testdata[i].end();

		//vector<double> histdata(first, last);

		//double pload = predict_load(first, last, &c);
		//double pload = predict_load(&histdata, &c);

		double pload = 0;
		
		//for (int i = 0; i < historical_data->size(); ++i)
		for(auto i = first, j = c.begin(); i != last; ++i, ++j)
		{
			pload += (*i) * (*j);
		}
		
		//load /= historical_data->size();

		// subtract the actual value to work out wastage
		pload -= testdata[i][0];

		// penalise for under estimating
		if (pload < 0)
		{
			pload = abs(pload) * 100000;
		}

		score += pload;
	}

	// normalise score to the amount of available data
	score /= testdata.size();

	return score;
}

bool shippingproblem::comparator(double a, double b)
{
	return a < b;
}
