# platform independent part

BINDIR = $(RHO)/bin

PROGRAM = RhoToolsTest NetworkTrainer Analysis
OBJS    = $(addsuffix .o, $(PROGRAM) )

all : $(PROGRAM)

RhoToolsTest : RhoToolsTest.o
	$(CXX) -g $(CXXFLAGS) $(LDFLAGS) $< $(RHOLIBS) $(GLIBS) -o $(BINDIR)/$@

Kaniggels : Kaniggels.o
	$(CXX) -g $(CXXFLAGS) $(LDFLAGS) $< $(RHOLIBS) $(GLIBS) $(GALIB) -o $(BINDIR)/$@

NetworkTrainer : NetworkTrainer.o
	$(CXX) -g $(CXXFLAGS) $(LDFLAGS) $< $(RHOLIBS) $(GLIBS) -o $(BINDIR)/$@

Analysis : Analysis.o
	$(CXX) -g $(CXXFLAGS) $(LDFLAGS) $< $(RHOLIBS) $(GLIBS) -o $(BINDIR)/$@

# Static pattern rule for object file dependency on sources:
.SUFFIXES: .cxx

.cxx.o: %.o : %.cxx  %.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Some extra dependecies from #includes (needs to be completed,
# if in doubt do a "gmake clean" first):

#
# Complete cleanup:
#
clean :
	rm  -f *.o
	cd $(BINDIR); rm $(PROGRAM)

