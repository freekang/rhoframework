#PAF standalone makefile
PACKAGE = PAFDisplay

LIBA	= lib$(PACKAGE).a
LIBSO	= lib$(PACKAGE).so

# Lists of objects to include in library:
NAMELIST = PAFDisplay \
           PAFDisplayTrack \
	   PAFDisplayMcTrack \
           PAFDisplayVertex \
           PAFDetector \
           BpqGeometry \
           SvtGeometry \
           DrcGeometry \
           IfrGeometry

NOCINT =   PAFDetector \
           BpqGeometry \
           SvtGeometry \
           DrcGeometry \
           IfrGeometry

HDRS    = $(addsuffix .h, $(filter-out $(NOCINT),$(NAMELIST)) )
OBJS    = $(addsuffix .o, $(NAMELIST) )
#ROOTCINTTARGETS := $(filter-out $(PACKAGE)_LinkDef.rdl, $(wildcard *.rdl)) $(wildcard $(PACKAGE)_LinkDef.rdl)
#ROOTCINTHDRS :=   $(addsuffix .hh, $(basename $(ROOTCINTTARGETS)) )
ROOTCINTHDRS := $(HDRS) $(PACKAGE)_LinkDef.h

# Default action 
all : $(ROOTCINTHDRS) $(LIBA) $(LIBSO)

hdr: $(ROOTCINTHDRS)

shlib: $(LIBSO)

# Static library:
$(LIBA) : $(OBJS)  $(PACKAGE)Cint.o
	rm -f $@
	$(AR) $(ARFLAGS)  $(LIBDIR)/$@ $^

# Shared library:
$(LIBSO) : $(OBJS)  $(PACKAGE)Cint.o
	$(LD) -L$(PAF)/lib $(SOFLAGS) $(GLIBS) -o $(LIBDIR)/$@ $^
	$(LD) -dynamiclib -single_module -undefined dynamic_lookup -install_name $(LIBDIR)/$@ -L$(RHO)/lib $(LDFLAGS) $(GLIBS) -o $(LIBDIR)/$@ $^

# Rules for Dictionary:
$(PACKAGE)Cint.o : $(PACKAGE)Cint.cxx
	$(CXX) $(CXXFLAGS) -c $<
	@echo "-----$(PACKAGE)Cint.o---------"

$(PACKAGE)Cint.cxx : $(ROOTCINTHDRS)
	rootcint -f $@ -c  -I.. $^
	@echo "-----$(PACKAGE)Cint.cxx---------"

# Static pattern rule for object file dependency on sources:
$(OBJS) : %.o : %.cxx
	$(CXX) $(CXXFLAGS) -c $< -o $@

#$(ROOTCINTHDRS) : %.h : %.rdl
#	cp $< $@

# Some extra dependecies from #includes (needs to be completed,
# if in doubt do a "gmake clean" first):

#
# Complete cleanup:
#
clean :
	rm  -f *.o 
	rm  -f $(PACKAGE)Cint.*
	rm  -f $(LIBA) $(LIBSO)

