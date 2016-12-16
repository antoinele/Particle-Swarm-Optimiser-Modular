#include "shippingproblem.h"
#include <utilities.h>

#include <cassert>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdlib>

#include <omp.h>

#define SHIPPINGPROB_USEEIGEN 1

using namespace std;
using namespace pso;
using namespace Eigen;

// converts the raw CSV data into an Eigen matrix
void shippingproblem::generatesimdtestdata()
{
	size_t n_rows, row_size;

	n_rows = testdata.size();
	row_size = testdata[0].size();
	
	simdtestdata = MatrixXd(n_rows, row_size);

	// could be improved with Eigen mapping
	for (size_t i = 0; i < n_rows; i++)
	{
		for (size_t j = 0; j < row_size; j++)
		{
			simdtestdata(i, j) = testdata[i][j];
		}
	}
}

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
			//invalid line, ignore
			continue;
		}

		testdata.push_back(linedata);
	}

	generatesimdtestdata();
}

vector<vector<double>> shippingproblem::bounds()
{
	vector<vector<double>> b(dimensions);

	// for the assignment, bounds of +/- 10 seem to give the best results
	// however increasing the bounds beyond this doesn't hurt, it doesn't
	// improve the quality of the solution either.
	vector<double> ib = { -10.0, 10.0 };

	for (size_t i = 0; i < dimensions; ++i)
	{
		b[i] = ib;
	}

	return b;
}

#ifndef SHIPPINGPROB_USEEIGEN
double shippingproblem::evaluate(coordinate c)
{
	double score(0);

	size_t testdatasize = testdata.size();

	assert(testdatasize > 0);
	assert(testdata[0].size() - 1 == c.size());

	for (size_t i = 0; i < testdatasize; ++i)
	{
		auto first = testdata[i].begin() + 1,
			last = testdata[i].end();

		double pload(0);

		// sum of historical data * weights
		for (auto i = first, j = c.begin(); i != last; ++i, ++j)
		{
			pload += (*i) * (*j);
		}

		// subtract the actual value to work out wastage
		pload -= testdata[i][0];

		//// normalise pload to % over estimate
		//pload /= testdata[i][0];
		//pload -= 1;

		//// penalise for under estimating
		//if (pload < 0)
		//{
		//	pload = 100000;
		//	//pload = abs(pload) * 100000;
		//}

		pload = abs(pload);

		score += pload;
	}

	// normalise score to the amount of available data
	score /= testdata.size();

	return score;
}
#else
double shippingproblem::evaluate(const coordinate &c)
{
	// create an Eigen vector of the same length as a row from the
	// historical data.
	VectorXd vc(c.size() + 1);

	// set the weight of the first value to -1 as the 1st column in
	// the historical data is the actual value so this will subtract
	// it.
	vc(0) = -1;

	// copy the weights in, offset to skip the actual end of day
	// value.
	for (size_t i = 0; i < c.size(); i++)
	{
		vc(i + 1) = c[i];
	}

	// multiply the historical data by the weights, get the absolute
	// values, then return the mean of those values.
	return (simdtestdata * vc).cwiseAbs().mean();
}
#endif // !SHIPPINGPROB_USEEIGEN

