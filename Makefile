### Directory and Executable Names ###########
EXADIR     = RhoExamples
OBJDIR     = tmp
BINDIR     = bin

################################################################################################
		
all: SUBS

exam:
	cd RhoExamples
	make

######  Targets ###############################

SUBS:
	cd ./RhoNNO; make
	cd ./RhoGA; make
	cd ./RhoMath; make
	cd ./RhoBase; make
	cd ./RhoConditions; make
	cd ./RhoTools; make
	cd ./RhoHistogram; make
	cd ./PAFSchema; make
	cd ./KangaSchema; make
	cd ./RhoManager; make
	cd ./RhoSelector; make
	cd ./RhoExamples; make
	cd ./PAFDisplay; make
	  

cleanall:
	cd ./lib; rm *.a *.so
	cd ./RhoNNO; make clean
	cd ./RhoGA; make clean
	cd ./RhoMath; make clean
	cd ./RhoBase; make clean
	cd ./RhoConditions; make clean
	cd ./RhoTools; make clean
	cd ./RhoHistogram; make clean
	cd ./PAFSchema; make clean
	cd ./KangaSchema; make clean
	cd ./RhoManager; make clean
	cd ./RhoSelector; make clean
	cd ./RhoExamples; make clean
	cd ./PAFDisplay; make clean

documentation:
#	mkdir tmp
	cd RhoNNO; cp *.cxx ../tmp; cp *.h ../tmp
	cd RhoMath; cp *.cxx ../tmp; cp *.h ../tmp
	cd RhoBase; cp *.cxx tmp; cp *.h ../tmp
	cd RhoConditions; cp *.cxx ../tmp; cp *.h ../tmp
	cd RhoTools; cp *.cxx ../tmp; cp *.h ../tmp
	cd RhoHistogram; cp *.cxx ../tmp; cp *.h ../tmp
	cd RhoManager; cp *.cxx ../tmp; cp *.h ../tmp
	cd RhoSelector; cp *.cxx ../tmp; cp *.h ../tmp
	cd PAFSchema; cp *.cxx ../tmp; cp *.h ../tmp
	cd KangaSchema; cp *.cxx ../tmp; cp *.h ../tmp

	root -b RhoMacros\documentation.C

#######################################
###     Complete Cleanup            ###
#######################################
		
clean :
	rm $(BINDIR)/*
	rm $(EXADIR)/*.o

