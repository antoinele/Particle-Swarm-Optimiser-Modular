#include <core/problem.h>
#include <core/init.h>

#include <iostream>

#include <cmath>
#include <cassert>
#include <vector>

using namespace std;
using namespace pso;

class sphereproblem : public problem_base
{
private:
	const size_t n_dimensions;
	vector<vector<double>> _bounds;
public:
	sphereproblem(size_t n_dimensions);
	vector<vector<double>> bounds();
	bool is_valid(pso::coordinate c);
	double evaluate(const pso::coordinate& c);

	virtual bool comparator(const double a, const double b)
	{
		return a < b;
	}
};

sphereproblem::sphereproblem(size_t n_dimensions) : problem_base(), n_dimensions(n_dimensions)
{
	for (size_t i = 0; i < n_dimensions; i++)
	{
		vector<double> bounds = { -1.0, 1.0 };
		_bounds.push_back(bounds);
	}
}

vector<vector<double>> sphereproblem::bounds()
{
	return _bounds;
}

bool sphereproblem::is_valid(coordinate c)
{
	assert(c.size() == n_dimensions);

	for (size_t i = 0; i < n_dimensions; i++)
	{
		if (c[i] < _bounds[i][0] || c[i] > _bounds[i][1])
			return false;
	}

	return true;
}

double sphereproblem::evaluate(const coordinate& c)
{
	assert(c.size() == n_dimensions);

	double d = 0.0;

	for (size_t i = 0; i < n_dimensions; i++)
	{
		d += c[i] * c[i];
	}

	return sqrt(d);
}

void sphereproblem_help() {
	cerr
		<< "sphere problem arguments" << endl
		<< "  -h      Prints this help message" << endl
		<< "  -d <n>  Sets the number of dimensions. (default: 3)" << endl;
}

shared_ptr<pso::problem_base> sphereproblem_factory(vector<string> args)
{
	int n_dimensions = 3;
	for (auto it = args.begin(); it != args.end(); it++)
	{
		if (it->compare("-d") == 0)
		{
			it++;
			n_dimensions = atoi(it->c_str());
		}
		else if (it->compare("-h") == 0)
		{
			sphereproblem_help();
			exit(0);
		}
		else
		{
			cerr << "Unknown argument: " << *it << endl;
			sphereproblem_help();
			exit(1);
		}
	}

	return make_shared<sphereproblem>(n_dimensions);
}

void sphereproblem_init()
{
	problem_base::register_problem("sphereproblem", &sphereproblem_factory);
}

module_init(sphereproblem_init);