build-dir = $(patsubst %/,%,$(dir $@))
target-dir = $(dir $@)

INCLUDES := -I../packages/Eigen.3.2.9/build/native/include

include scripts/Makefile.include

all: pso

core-y				:= core/
neighbourhoods-y	:= neighbourhoods/
problems-y			:= problems/

pso-dirs := $(patsubst %/,%,$(filter %/, $(core-y) $(neighbourhoods-y) $(problems-y)))

pso-alldirs := $(sort $(pso-dirs) $(patsubst %/,%,$(filter %/, $(core-) $(neighbourhoods-y) $(problems-y))))

core-y				:= $(patsubst %/, %/built-in.o, $(core-y))
neighbourhoods-y	:= $(patsubst %/, %/built-in.o, $(neighbourhoods-y))
problems-y			:= $(patsubst %/, %/built-in.o, $(problems-y))

export PSO_MAIN := $(core-y) $(neighbourhoods-y) $(problems-y)

export CXXFLAGS := -I. $(INCLUDES) -std=c++14 -O3 -Wall -fopenmp -march=native -mtune=generic
#export LDFLAGS  := -fopenmp -march=native -mtune=generic -flto -O3
export LDFLAGS := -flto

pso: $(PSO_MAIN)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^

PHONY += $(pso-dirs)
$(pso-dirs):
	$(MAKE) $(build)=$@

$(sort $(PSO_MAIN)): $(pso-dirs) ;

clean-dirs		:= $(addprefix _clean_, . $(pso-alldirs))

PHONY += $(clean-dirs) clean
$(clean-dirs):
	$(MAKE) $(clean)=$(patsubst _clean_%,%,$@)

clean: $(clean-dirs)

%.o: %.cpp
	$(MAKE) $(build)=$(build-dir) $(target-dir)$(notdir $@)
%.o: %.c
	$(MAKE) $(build)=$(build-dir) $(target-dir)$(notdir $@)

.PHONY: $(PHONY)