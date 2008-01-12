# 
# Makefile for TSP 1.2 and ABACUS 3.0
#

#######################################################################
#
# Please adapt the following settings, if necessary.
#
#######################################################################

# Define the C++ compiler you are using:
#
                  CCC = g++-4.1

# Define an ABACUS flag identifying the C++ compiler:
#
       COMPILER_FLAGS = -g -DABACUS_COMPILER_GCC41    # GNU C++ 4.1

# Set ABACUS_INCLUDE_PATH to the ABACUS include directory
# and ABACUS_LIBRARY_PATH to the ABACUS lib directory.
# Its pathname has the form  .../lib/<arch> where <arch> selects the 
# architecture of your system:
#
  ABACUS_INCLUDE_PATH = ../Include
  ABACUS_LIBRARY_PATH = ../lib/linux20-gcc41

# Set OSI_INCLUDE_PATH to the directory containing the coin-osi header files 
# and OSI_LIB_PATH to the directory containing the coin-osi libraries:
# 
     OSI_INCLUDE_PATH = ../../coin-Osi/include
     OSI_LIBRARY_PATH = ../../coin-Osi/lib

# Set the path to your solver include directories and solver libs,
# leave empty if the solver is not installed:
#
     CLP_INCLUDE_PATH = $(OSI_INCLUDE_PATH)
         CLP_LIB_PATH = $(OSI_LIBRARY_PATH)
     CBC_INCLUDE_PATH = 
         CBC_LIB_PATH = 
   CPLEX_INCLUDE_PATH = 
       CPLEX_LIB_PATH = 
    DYLP_INCLUDE_PATH = $(OSI_INCLUDE_PATH)
        DYLP_LIB_PATH = $(OSI_LIBRARY_PATH)
  FORTMP_INCLUDE_PATH =
      FORTMP_LIB_PATH =
    GLPK_INCLUDE_PATH =
        GLPK_LIB_PATH =
   MOSEK_INCLUDE_PATH =
       MOSEK_LIB_PATH =
     OSL_INCLUDE_PATH =
         OSL_LIB_PATH =
  SOPLEX_INCLUDE_PATH =
      SOPLEX_LIB_PATH =
SYMPHONY_INCLUDE_PATH =
    SYMPHONY_LIB_PATH =
     VOL_INCLUDE_PATH = $(OSI_INCLUDE_PATH)
         VOL_LIB_PATH = $(OSI_LIBRARY_PATH)
  XPRESS_INCLUDE_PATH =
      XPRESS_LIB_PATH =



######### NO CHANGES SHOULD BE NECESSARY BELOW THIS LINE #####################

           ABACUS_LIB = abacus-osi

             OSI_LIBS = -lOsi -lCoinUtils 
              LP_LIBS = $(OSI_LIBS) 
         OSI_LIB_PATH =

ifneq ($(strip $(CLP_INCLUDE_PATH)),)
         OSI_INC_PATH+= -I $(CLP_INCLUDE_PATH)
         OSI_LIB_PATH+= -L $(CLP_LIB_PATH)
              LP_LIBS+= -lOsiClp -lClp
          endif
ifneq ($(strip $(CBC_INCLUDE_PATH)),)
         OSI_INC_PATH+= -I $(CBC_INCLUDE_PATH)
         OSI_LIB_PATH+= -L $(CBC_LIB_PATH)
              LP_LIBS+= -lOsiCbc -lCbc
          endif
ifneq ($(strip $(CPLEX_INCLUDE_PATH)),)
         OSI_INC_PATH+= -I $(CPLEX_INCLUDE_PATH) -DABACUS_CPLEX 
         OSI_LIB_PATH+= -L $(CPLEX_LIB_PATH)
              LP_LIBS+= -lOsiCpx -lcplex -lpthread
          endif
ifneq ($(strip $(DYLP_INCLUDE_PATH)),)
         OSI_INC_PATH+= -I $(DYLP_INCLUDE_PATH)
         OSI_LIB_PATH+= -L $(DYLP_LIB_PATH)
              LP_LIBS+= -lOsiDylp -lDylp
          endif
ifneq ($(strip $(FORTMP_INCLUDE_PATH)),)
         OSI_INC_PATH+= -I $(FORTMP_INCLUDE_PATH)
         OSI_LIB_PATH+= -L $(FORTMP_LIB_PATH)
	      LP_LIBS+= -lOsiFmp -lfmp32st
          endif
ifneq ($(strip $(GLPK_INCLUDE_PATH)),)
         OSI_INC_PATH+= -I $(GLPK_INCLUDE_PATH)
         OSI_LIB_PATH+= -L $(GLPK_LIB_PATH)
              LP_LIBS+= -lOsiGlpk -lglpk
          endif
ifneq ($(strip $(MOSEK_INCLUDE_PATH)),)
         OSI_INC_PATH+= -I $(MOSEK_INCLUDE_PATH)
         OSI_LIB_PATH+= -L $(MOSEK_LIB_PATH)
	      LP_LIBS+= -lOsiMsk -lmosekdl
          endif
ifneq ($(strip $(OSL_INCLUDE_PATH)),)
         OSI_INC_PATH+= -I $(OSL_INCLUDE_PATH)
         OSI_LIB_PATH+= -L $(OSL_LIB_PATH)
	      LP_LIBS+= -lOsiOsl -losl
          endif
ifneq ($(strip $(SOPLEX_INCLUDE_PATH)),)
         OSI_INC_PATH+= -I $(SOPLEX_INCLUDE_PATH)
         OSI_LIB_PATH+= -L $(SOPLEX_LIB_PATH)
	      LP_LIBS+= -lOsiSpx -lsoplex
          endif
ifneq ($(strip $(SYMPHONY_INCLUDE_PATH)),)
         OSI_INC_PATH+= -I $(SYMPHONY_INCLUDE_PATH)
         OSI_LIB_PATH+= -L $(SYMPHONY_LIB_PATH)
              LP_LIBS+= -lOsiSym -lSym -lCgl -lOsiClp -lClp
          endif
ifneq ($(strip $(VOL_INCLUDE_PATH)),)
         OSI_INC_PATH+= -I $(VOL_INCLUDE_PATH)
         OSI_LIB_PATH+= -L $(VOL_LIB_PATH)
	      LP_LIBS+= -lOsiVol -lVol
          endif
ifneq ($(strip $(XPRESS_INCLUDE_PATH)),)
         OSI_INC_PATH+= -I $(XPRESS_INCLUDE_PATH)
         OSI_LIB_PATH+= -L $(XPRESS_LIB_PATH)
              LP_LIBS+= -lOsiXpr -lxosl -lmp-opt
          endif


CCFLAGS = -O $(COMPILER_FLAGS) -I $(ABACUS_INCLUDE_PATH) -Imincut \
	  $(OSI_INC_PATH) 

LD_FLAGS = -L $(ABACUS_LIBRARY_PATH) -l$(ABACUS_LIB) \
	   -L $(OSI_LIBRARY_PATH) $(OSI_LIB_PATH) $(LP_LIBS)  -lm

SOURCES = tspmain.cc tspmaster.cc tspsub.cc degree.cc subtour.cc edge.cc

#OBJECTS = $(SOURCES:.cc=.o)
OBJECTS = tspmain.o tspmaster.o tspsub.o degree.o subtour.o edge.o


tsp: mincut/mincut.a $(OBJECTS) 
	$(CCC) -o tsp $(OBJECTS) $(LD_FLAGS) mincut/mincut.a

mincut/mincut.a:
	cd mincut; \
	$(MAKE)

tangle:
	ctangle tsp.w
	/bin/rm -f tsp.c

#
# produce the .o files from the .cc and .c files
#

%.o: 
	make tangle 
	$(CCC) $(CCFLAGS) -c $(SOURCES)


#
# targets for TeX, dvi, and Postscript version
#

tsp.tex: tangle $(SOURCES) $(HEADERS) tsp.w
	cweave tsp.w

tsp.dvi: tsp.tex classes.ps
	tex tsp.tex

tsp.ps: tsp.dvi
	dvips -o tsp.ps tsp.dvi

.PHONY: clean

clean:
	-rm -f *.o *.cc *.h tsp
	-rm -f tsp.tex tsp.idx tsp.toc tsp.dvi tsp.ps tsp.scn
	cd mincut; \
	$(MAKE) clean

clean-all: clean
	-rm -f tsp.dvi tsp.log tsp.ps tsp.idx tsp.tex tsp.toc tsp.scn
