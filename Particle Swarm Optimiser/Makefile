CXX				:= g++-6
INCLUDES		:= -Isrc -I../packages/Eigen.3.2.9/build/native/include
CFLAGS			:= -Wall -fopenmp
CXXFLAGS		:= -std=c++14 -Wall -fopenmp -march=native
LDFLAGS			:= -fopenmp -march=native
SHAREDSOURCES	:= src/shippingproblem.cpp src/optimiser.cpp src/particle.cpp src/optimiserlogging.cpp src/utilities.cpp
SOURCES			:= $(SHAREDSOURCES) src/main.cpp
EXECUTABLE		:= pso

EVALUATORSRC	:= $(SHAREDSOURCES) src/evaluatorcli.cpp
EVALUATORBIN	:= evaluator
#PREDICTORSRC	:= $(SHAREDSOURCES) src/predictorcli.cpp
#PREDICTORBIN	:= predictor

DEBUGDIR   := bin/Debug
RELEASEDIR := bin/Release

DEBUG ?= 0
ifeq ($(DEBUG),1)
	CXXFLAGS += -DDEBUG -g
	BUILDDIR := $(DEBUGDIR)
else
	CXXFLAGS += -O3 -g
	LDFLAGS += -flto
	BUILDDIR := $(RELEASEDIR)
endif

TARGET := $(BUILDDIR)/$(EXECUTABLE)
EVALUATORTGT := $(BUILDDIR)/$(EVALUATORBIN)

srcs2obj = $(addprefix $(BUILDDIR)/,$(subst src/,,$(patsubst %.cpp,%.o,$(1))))
srcs2d   = $(addprefix $(BUILDDIR)/,$(subst src/,,$(patsubst %.cpp,%.d,$(1))))

OBJECTS	= $(call srcs2obj, $(SOURCES))
DEPS	= $(call srcs2d, $(SOURCES))

EVALUATOROBJS = $(call srcs2obj, $(EVALUATORSRC))

#OBJECTS	= $(addprefix $(BUILDDIR)/,$(subst src/,,$(SOURCES:.cpp=.o)))
#DEPS	= $(addprefix $(BUILDDIR)/,$(subst src/,,$(SOURCES:.cpp=.d)))

.SECONDARY: $(DEPS)
.PHONY: all clean

all: $(TARGET) $(EVALUATORTGT)

$(TARGET): $(OBJECTS) | $(BUILDDIR)
	$(CXX) $(LDFLAGS) $^ -o $@

$(EVALUATORTGT): $(EVALUATOROBJS) | $(BUILDDIR)
	$(CXX) $(LDFLAGS) $^ -o $@

$(BUILDDIR)/%.o: src/%.cpp | $(BUILDDIR) $(DEPS)
	$(CXX) -c $(CXXFLAGS) $(INCLUDES) $< -o $@

$(BUILDDIR)/%.d: src/%.cpp
	$(CXX) -MM -MP $(CXXFLAGS) $(INCLUDES) $< -MF $@

$(BUILDDIR):
	mkdir -p $@

clean:
	-rm -f $(OBJECTS) $(DEPS) $(TARGET) $(EVALUATOROBJS) $(EVALUATORTGT)
	-rmdir -p $(BUILDDIR)

ifeq (0, $(words $(findstring $(MAKECMDGOALS), clean)))
	include $(DEPS)
endif