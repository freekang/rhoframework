# Makefile for Linux
# Assumes the environment variable ROOTSYS to exist
#

# Define directory paths

OBJDIR        = $(RHO)/tmp
LIBDIR        = $(RHO)/lib
BINDIR        = $(RHO)/bin

ROOTCFLAGS    = $(shell root-config --cflags)
ROOTLIBS      = $(shell root-config --libs)
ROOTGLIBS     = $(shell root-config --glibs)
 
# Linux with egcs
CXX           = g++
CXXFLAGS      = $(RHOARCH) -DBABAR_COMP_INST -fno-exceptions -fPIC -I..
LD            = g++
LDFLAGS       = $(RHOARCH) -Wl,-rpath,/lib:$(LIBDIR):$(ROOTSYS)/lib
SOFLAGS       = -shared

CXXFLAGS     += $(ROOTCFLAGS)
LIBS          = $(ROOTLIBS) -lEG -lProof -lTreePlayer
GLIBS         = $(ROOTGLIBS) -lEG -lProof -lTreePlayer

# Get dependencies:
include GNUmake-depend.mk

