CXX		   := g++-6
INCLUDES   := -Isrc -I../packages/Eigen.3.2.9/build/native/include
CFLAGS	   := -Wall -fopenmp
CXXFLAGS   := -std=c++14 -Wall -fopenmp
LDFLAGS    := -fopenmp
SOURCES    := src/shippingproblem.cpp src/optimiser.cpp src/particle.cpp src/optimiserlogging.cpp src/utilities.cpp src/main.cpp 
EXECUTABLE := pso

DEBUGDIR   := bin/Debug
RELEASEDIR := bin/Release

DEBUG ?= 0
ifeq ($(DEBUG),1)
	CXXFLAGS += -DDEBUG -g
	BUILDDIR := $(DEBUGDIR)
	TARGET := $(BUILDDIR)/$(EXECUTABLE)
else
	CXXFLAGS += -O3
	LDFLAGS += -flto
	BUILDDIR := $(RELEASEDIR)
	TARGET := $(BUILDDIR)/$(EXECUTABLE)
endif

OBJECTS	= $(addprefix $(BUILDDIR)/,$(subst src/,,$(SOURCES:.cpp=.o)))
DEPS	= $(addprefix $(BUILDDIR)/,$(subst src/,,$(SOURCES:.cpp=.d)))

.SECONDARY: $(DEPS)
.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJECTS) | $(BUILDDIR)
	$(CXX) $(LDFLAGS) $^ -o $@

$(BUILDDIR)/%.o: src/%.cpp | $(BUILDDIR) $(DEPS)
	$(CXX) -c $(CXXFLAGS) $(INCLUDES) $< -o $@

$(BUILDDIR)/%.d: src/%.cpp
	$(CXX) -MM -MP $(CXXFLAGS) $(INCLUDES) $< -MF $@

$(BUILDDIR):
	mkdir -p $@

clean:
	-rm -f $(OBJECTS) $(DEPS) $(TARGET)
	-rmdir -p $(BUILDDIR)

ifeq (0, $(words $(findstring $(MAKECMDGOALS), clean)))
	include $(DEPS)
endif