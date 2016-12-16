Particle Swarm Optimiser
========================

This project was made with Visual Studio 2015, however it can be built with the included Makefile.
All dependencies are also included. The preferred build environment is g++/Makefile as VS2015 doesn't
support OpenMP beyond 2.0.

This requires a compiler that supports C++14

When building using the makefile, an evaluator program is also produced which simply takes a
historical data CSV and a set of weights as its arguments then produces a prediction.

The makefile outputs to ./bin/Release or ./bin/Debug depending on whether `DEBUG=1` is specified.

The Linux binaries included are compiled for a 4th gen i7 and may need to be rebuilt with
`make clean && make`

It has not been tested on a 32 bit OS and there are some areas where 64 bit integer types are hard
coded.

The command I used when developing this to test was:
```
./bin/Release/pso -wait -seed 12341234 -solutions 100 -maxruntime 30 -threads 4 -neighbours 4 "cwk_train.csv"
```

Usage
=====

Optimiser usage
---------------
```
Usage: ./bin/Release/pso [-options] <testdata.csv>

Options:
  -solutions <n>     Number of solutions (particles) to use
  -seed <n>          Seed for random number generator
  -wait              Pause after simulation is finished
  -maxcycles <n>     If used with -targetfitness it will limit the numer of cycles the
                     simulation will run for. Otherwise it will be the absolute number
                     of cycles to run for.
  -targetfitness <d> The fitness the program should run until
  -maxruntime <n>    The time, in seconds, the program should run for until stopping.
                     When combined with -maxcycles or -targetfitness the first event
                     will stop the program. Set to 0 for infinite runtime. Default is
                     30s.
  -logfile <csv>         Write statistics to the specified file. Specify `-` for stdout.
  -threads <n>       The number of threads to run in parallel. Default: 1.
  -neighbours <n>    Set the average size of the neighbourhood. 0 means use g_best.
                     Default: 3.
```

Evaluator usage
---------------
```
./bin/Release/evaluator <testdata.csv> <d>...

where d are weights. The number of weights specified must be the same as the number of
columns in the csv -1.
```