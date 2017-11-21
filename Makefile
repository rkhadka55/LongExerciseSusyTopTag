IDIR       = .
ODIR       = obj
SDIR       = .

XGBOOST_DIRECTORY = /cvmfs/sft.cern.ch/lcg/releases/xgboost/0.60-04a4f/x86_64-slc6-gcc62-opt
TTDIR  = $(CMSSW_BASE)/src/TopTagger/
TTTDIR     = $(TTDIR)/TopTagger/test

CXX        = g++

CXXFLAGS  += -I. -I$(CMSSW_BASE)/src -std=c++0x
## Optimization flag
CXXFLAGS += -g #-O3
## Enable the maximun warning
#CXXFLAGS += -Wall -Wextra -Weffc++ -g

## Include ROOT
CXXFLAGS  += $(shell root-config --cflags)

CXXDEPFLAGS = -MMD -MP

LD         = g++
LDFLAGS    =

LIBS       = $(shell root-config --glibs)
LIBSTOPTAGGER = -L$(TTTDIR) -lTopTagger

# xgboost includes and libraries
ifdef XGBOOST_DIRECTORY
	CXXFLAGS += -I$(XGBOOST_DIRECTORY) -I$(XGBOOST_DIRECTORY)/rabit/include

	CXXFLAGS += -DDOXGBOOST
	LIBS     += -L$(XGBOOST_DIRECTORY)/lib -lxgboost
endif


PROGRAMS = RunSimpleAnalyzer


all: mkobj $(PROGRAMS)

mkobj:
	@mkdir -p obj

$(ODIR)/%.o : $(SDIR)/%.C
	$(CXX) $(CXXFLAGS) $(CXXDEPFLAGS)  -fPIC -o $@ -c $<

$(ODIR)/%.o : $(SDIR)/%.c
	$(CXX) $(CXXFLAGS) $(CXXDEPFLAGS)  -fPIC -o $@ -c $<

$(ODIR)/%.o : $(SDIR)/%.cc
	$(CXX) $(CXXFLAGS) $(CXXDEPFLAGS)  -fPIC -o $@ -c $<

$(ODIR)/%.o : $(SDIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(CXXDEPFLAGS)  -fPIC -o $@ -c $<


RunSimpleAnalyzer: $(ODIR)/RunSimpleAnalyzer.o $(ODIR)/SimpleAnalyzer.o
	$(LD) $^ $(LIBS) $(LIBSTOPTAGGER) -fPIC -o $@

clean:
	rm -f $(ODIR)/*.o $(ODIR)/*.so $(ODIR)/*.d $(PROGRAMS) core 
