#######################################################################
#
# Makefile for ABACUS 3.0
#
# Please adapt the following settings, if necessary.
#
#######################################################################

# Path to coin-osi header files 
	OSI_INCLUDE=/usr/include/coin
# Path to solver include directories - leave empty if the solver 
# is not installed
     CLP_PATH=$(OSI_INCLUDE)
     CBC_PATH= 
   CPLEX_PATH= 
    DYLP_PATH=
  FORTMP_PATH=
    GLPK_PATH=
   MOSEK_PATH=
     OSL_PATH=
  SOPLEX_PATH=
SYMPHONY_PATH=
     VOL_PATH=
  XPRESS_PATH=

# Path that 'make install' will copy the ABACUS library to
ABACUS_INSTALL_LIBDIR =

# Path that 'make install' will copy the ABACUS header files to
ABACUS_INSTALL_HEADERDIR =


# No changes should be necessary below this line
# _____________________________________________________________________

#######################################################################
#
# Default settings 
#
#######################################################################

# gcc 
CCC = g++-4.4

# add files to an archive
ARCHIVE	    = ar rc

# PRE_ARCHIVE is used to instantiate required templates (SGI)
PRE_ARCHIVE = true

STRIP       = strip -x

AWK	    = awk
PERL	    = /usr/bin/perl

# Gnu cp
GCP	    = /bin/cp

CPLEXFLAGS      = -DCPX_PROTOTYPE_ANSI -DPROTOTYPE_MAX
CCFLAG_SYS      = -DABACUS_SYS_LINUX -DABACUS_THROW_WITH_INFO
CCFLAG_COMPILER = -DABACUS_COMPILER_GCC43 -Wall
CCFLAG_DEBOPT   = -O2
CCFLAG_PARALLEL =

# default parallel flags (to be defined in machine specific makefile).
CCFLAG_PARALLEL	=
ABACUS_PARALLEL_C =
ABACUS_PARALLEL_H =
ABACUS_PARALLEL_INC =

# $(ARCHTECT) identifies the architecture and the operating system of 
#  the target (i.e. solaris, linux)
ARCHTECT = linux20-gcc44

#
# Where the output goes
#
OUTPUTDIR = .

# Additional targets (to be defined in machine specific makefile)
# ADDITIONAL_LIB_ALL =
# ADDITIONAL_CLEAN =


#######################################################################
#
# Let us change the default variable settings, if desired.
#
#######################################################################

ifdef ABACUS_MAKE_SETTINGS
include Make-settings/$(ABACUS_MAKE_SETTINGS)
endif


#######################################################################
#
# What can be done?
#
#######################################################################

default:
	@echo "usage: make abacus         : compile abacus at default path "
	@echo "       make install        : compile abacus and install it to a specific location"
	@echo "                             Set the variables ABACUS_INSTALL_LIBDIR"
	@echo "                             and ABACUS_INSTALL_HEADERDIR in the Makefile first."
	@echo "       make doxygendoc     : produce the reference manual in html"
	@echo "       make manual         : produce the complete user's guide and reference manual in pdf"
	@echo "       make manualhtml     : produce the complete user's guide and reference manual in html"
	@echo "       make clean          : remove the object files"
	@echo "       make clean-all      : remove all temporary files"
	@echo "       make tar            : create a tar file containing the ABACUS source code"


#######################################################################
#
# General stuff
#
# Directories and variables
#
# include files (*.h, *.inc)
#######################################################################

HEADERDIR	= $(OUTPUTDIR)/Include/abacus
ABACUS_INCLUDE	= -I$(OUTPUTDIR)/Include -I$(OSI_INCLUDE) 

# library
LIBDIR		= $(OUTPUTDIR)/lib/$(ARCHTECT)

# for c++ source files (*.cc)
SRCDIR		= $(OUTPUTDIR)/sources

# for object files (*.o) and Makefile.dep
OBJDIR		= $(OUTPUTDIR)/tmp/$(ARCHTECT)

# for the tex classes
TEXDIR		= $(OUTPUTDIR)/tmp
TEXCLASSDIR	= $(TEXDIR)/classes
TEXSRCDIR	= Tex

DEPEND = $(OBJDIR)/Makefile.dep

CCFLAGS = $(ABACUS_INCLUDE) $(CCFLAG_SYS) $(CCFLAG_COMPILER) \
	$(CCFLAG_TEMPLATE) $(CCFLAG_PARALLEL) $(CCFLAG_DEBOPT) 

#######################################################################
#
# Build directories if needed
#
#######################################################################

subdirs: $(HEADERDIR) $(SRCDIR) $(OBJDIR) $(LIBDIR) $(LIBDIR)/stuff \
	 $(TEXCLASSDIR)

$(HEADERDIR):
	-mkdir -p $(HEADERDIR)
$(SRCDIR):
	-mkdir -p $(SRCDIR)
$(OBJDIR):
	-mkdir -p $(OBJDIR)
$(LIBDIR):
	-mkdir -p $(LIBDIR)
$(LIBDIR)/stuff:
	-mkdir -p $(LIBDIR)/stuff
$(TEXCLASSDIR):
	-mkdir -p $(TEXCLASSDIR)


#######################################################################
#
# General rules
#
#######################################################################

# the object files
$(OBJDIR)/%.o: $(SRCDIR)/%.cc
	$(CCC)  $(CCFLAGS) $(ADDFLAGS) -c $< -o $@
	$(STRIP) $@


#######################################################################
#
# Cleanup
#
#######################################################################

clean: $(ADDITIONAL_CLEAN)
	rm -rf $(OBJDIR)/*.o $(LIBDIR)

clean-all:
	rm -rf $(OBJDIR) $(LIBDIR) $(TEXDIR)/ latex html manualhtml lib abacus-doxygen.tag manual.pdf abacus.tar.gz

test:
	ls $(OUTPUTDIR)
	echo $(OUTPUTDIR)


#######################################################################
#
# The files of the library
#
#######################################################################

ABACUS_C =    $(ABACUS_PARALLEL_C) \
              lp.cc global.cc abacusroot.cc \
              master.cc \
              vartype.cc sparvec.cc \
              optsense.cc \
              history.cc fixcand.cc opensub.cc \
              fsvarstat.cc lpvarstat.cc slackstat.cc \
              timer.cc cputimer.cc cowtimer.cc \
              branchrule.cc sub.cc active.cc \
              tailoff.cc  poolslot.cc \
              poolslotref.cc standardpool.cc nonduplpool.cc \
              lpsub.cc infeascon.cc \
              pool.cc  \
	      setbranchrule.cc \
	      valbranchrule.cc \
              row.cc column.cc \
              convar.cc constraint.cc conclass.cc csense.cc \
              rowcon.cc srowcon.cc \
              variable.cc numvar.cc \
              cutbuffer.cc boundbranchrule.cc conbranchrule.cc set.cc fastset.cc \
              array.cc buffer.cc dictionary.cc hash.cc \
              dlist.cc dlistitem.cc ring.cc list.cc listitem.cc \
              bprioqueue.cc bheap.cc bstack.cc \
	      sorter.cc colvar.cc numcon.cc string.cc \
	      lpsolution.cc separator.cc ostream.cc

ABACUS_H =  $(ABACUS_PARALLEL_H) \
             abacusroot.h \
             active.h \
             array.h \
             master.h \
             bheap.h \
             bprioqueue.h \
             branchrule.h \
             boundbranchrule.h \
             conbranchrule.h \
             setbranchrule.h \
             valbranchrule.h \
             buffer.h \
             column.h \
	     colvar.h \
	     conclass.h \
             constraint.h \
             convar.h \
	     cputimer.h \
             csense.h \
             cutbuffer.h \
             dictionary.h \
	     cowtimer.h \
             fastset.h \
             fixcand.h \
             fsvarstat.h \
             global.h \
	     hash.h \
             history.h \
             infeascon.h \
             listitem.h \
             list.h \
	     lp.h \
             lpvarstat.h \
	     lpsub.h \
             lpmaster.h \
             numvar.h \
	     numcon.h \
	     opensub.h \
             optsense.h \
             ostream.h \
	     pool.h \
	     poolslot.h \
             poolslotref.h \
	     row.h \
             ring.h \
             rowcon.h \
	     set.h \
             slackstat.h \
	     sorter.h \
	     srowcon.h \
             standardpool.h \
	     nonduplpool.h \
             sparvec.h \
             string.h \
	     sub.h \
             tailoff.h \
             timer.h \
             variable.h \
             vartype.h \
             bstack.h \
	     dlist.h \
	     dlistitem.h \
             ring.h \
	     lpsolution.h \
	     separator.h \
	     exceptions.h

BASIC_INC= array.inc \
      bheap.inc \
      bprioqueue.inc \
      bstack.inc \
      buffer.inc \
      dlist.inc \
      dlistitem.inc \
      dictionary.inc \
      hash.inc \
      listitem.inc \
      list.inc \
      ring.inc \
      sorter.inc

SPECIAL_INC= \
      active.inc \
      cutbuffer.inc \
      pool.inc \
      poolslot.inc \
      poolslotref.inc \
      standardpool.inc  \
      nonduplpool.inc \
      lpsolution.inc separator.inc 

ABACUS_INC= $(ABACUS_PARALLEL_INC) $(BASIC_INC) $(SPECIAL_INC)


#######################################################################
#
# The files of the LP-interface
#
#######################################################################

ABACUS_OSI_C  = osiif.cc  lpsubosi.cc lpmasterosi.cc
ABACUS_OSI_H = osiif.h lpsubosi.h lpmasterosi.h

ABACUS_ALL_C   = $(ABACUS_C) $(ABACUS_OSI_C) lpif.cc

ABACUS_ALL_H  = $(ABACUS_H) $(ABACUS_OSI_H)

ABACUS_HEADERS = $(ABACUS_ALL_H:%.h=$(HEADERDIR)/%.h) \
                 $(ABACUS_INC:%.inc=$(HEADERDIR)/%.inc)


#######################################################################
#
# The files of the documentation
#
#######################################################################

PUBLICDOC = \
            abacusroot.ccpub \
            master.ccpub \
            csense.ccpub \
	    fsvarstat.ccpub \
	    infeascon.ccpub \
            lp.ccpub \
	    lpvarstat.ccpub \
	    optsense.ccpub \
	    slackstat.ccpub \
	    sub.ccpub \
            vartype.ccpub 

PROTECTEDDOC = \
               convar.ccpro \
	       constraint.ccpro \
               lp.ccpro \
	       numvar.ccpro \
	       rowcon.ccpro \
               row.ccpro \
	       set.ccpro \
               sparvec.ccpro \
	       timer.ccpro \
               variable.ccpro

PRIVATEDOC = \
             active.ccpri \
             array.ccpri \
	     bheap.ccpri \
	     bprioqueue.ccpri \
	     bstack.ccpri \
	     buffer.ccpri \
	     column.ccpri \
             conclass.ccpri \
	     cplexif.ccpri \
	     csense.ccpri \
	     cutbuffer.ccpri \
	     dlist.ccpri \
	     dictionary.ccpri \
	     cowtimer.ccpri \
	     fixcand.ccpri \
	     fsvarstat.ccpri \
	     global.ccpri \
	     hash.ccpri \
	     history.ccpri \
	     infeascon.ccpri \
	     list.ccpri \
             lpsub.ccpri \
	     lpvarstat.ccpri \
	     opensub.ccpri \
	     optsense.ccpri \
	     ostream.ccpri \
	     poolslot.ccpri \
	     poolslotref.ccpri \
	     ring.ccpri \
	     slackstat.ccpri \
	     standardpool.ccpri \
             sorter.ccpri \
	     string.ccpri \
	     tailoff.ccpri \
	     timer.ccpri \
	     vartype.ccpri


#######################################################################
#
# the |LP|-solver interface
#
# the osi interface
#
#######################################################################

OSI_PATH=
OSI_FLAGS=-DABACUS_LP_OSI 

ifneq ($(strip $(CLP_PATH)),)
          OSI_PATH+= -I$(CLP_PATH)
	  OSI_FLAGS+= -DOSI_CLP
          endif
ifneq ($(strip $(CBC_PATH)),)
          OSI_PATH+= -I$(CBC_PATH)
	  OSI_FLAGS+= -DOSI_CBC
          endif
ifneq ($(strip $(CPLEX_PATH)),)
          OSI_PATH+= -I$(CPLEX_PATH)
	  OSI_FLAGS+= -DOSI_CPLEX 
          endif
ifneq ($(strip $(DYLP_PATH)),)
          OSI_PATH+= -I$(DYLP_PATH)
	  OSI_FLAGS+= -DOSI_DYLP 
          endif
ifneq ($(strip $(FORTMP_PATH)),)
          OSI_PATH+= -I$(FORTMP_PATH)
	  OSI_FLAGS+= -DOSI_FORTMP
          endif
ifneq ($(strip $(GLPK_PATH)),)
          OSI_PATH+= -I$(GLPK_PATH)
	  OSI_FLAGS+= -DOSI_GLPK 
          endif
ifneq ($(strip $(MOSEK_PATH)),)
          OSI_PATH+= -I$(MOSEK_PATH)
	  OSI_FLAGS+= -DOSI_MOSEK 
          endif
ifneq ($(strip $(OSL_PATH)),)
          OSI_PATH+= -I$(OSL_PATH)
	  OSI_FLAGS+= -DOSI_OSL 
          endif
ifneq ($(strip $(SOPLEX_PATH)),)
          OSI_PATH+= -I$(SOPLEX_PATH)
	  OSI_FLAGS+= -DOSI_SOPLEX 
          endif
ifneq ($(strip $(SYMPHONY_PATH)),)
          OSI_PATH+= -I$(SYMPHONY_PATH)
	  OSI_FLAGS+= -DOSI_SYMPHONY 
          endif
ifneq ($(strip $(VOL_PATH)),)
          OSI_PATH+= -I$(VOL_PATH)
	  OSI_FLAGS+= -DOSI_VOL 
          endif
ifneq ($(strip $(XPRESS_PATH)),)
          OSI_PATH+= -I$(XPRESS_PATH)
	  OSI_FLAGS+= -DOSI_XPRESS
          endif

OSIFLAGS = $(OSI_PATH) $(OSI_FLAGS) 
OSI_O = $(ABACUS_OSI_C:%.cc=$(OBJDIR)/%.o)

# interface objects with special CCFLAGS
$(OBJDIR)/osiif.o: $(SRCDIR)/osiif.cc
	$(CCC) $(CCFLAGS) $(OSIFLAGS) -c $(SRCDIR)/osiif.cc -o $@
	$(STRIP) $@

$(OBJDIR)/lpsubosi.o: $(SRCDIR)/lpsubosi.cc
	$(CCC) $(CCFLAGS) $(OSIFLAGS) -c $(SRCDIR)/lpsubosi.cc -o $@
	$(STRIP) $@

$(OBJDIR)/lpmasterosi.o: $(SRCDIR)/lpmasterosi.cc
	$(CCC) $(CCFLAGS) $(OSIFLAGS) -c $(SRCDIR)/lpmasterosi.cc -o $@
	$(STRIP) $@


#######################################################################
#
# build the basic ABACUS objects
#
#######################################################################

ABACUS_O  = $(ABACUS_C:%.cc=$(OBJDIR)/%.o)
objects: subdirs $(ABACUS_HEADERS) $(ABACUS_O)

# build all ABACUS *.cc source files
ABACUS_ALL_CC = $(ABACUS_ALL_C:%.cc=$(SRCDIR)/%.cc)

# create the interface objects lpifosi-osi.o 
#
# $@ The name of the current target
LPIF = osi 

lpif: subdirs $(LPIF:%=$(LIBDIR)/stuff/lpif-%.o)


$(LIBDIR)/stuff/lpif-osi.o: $(SRCDIR)/lpif.cc $(ABACUS_HEADERS)
	$(CCC) $(CCFLAGS) $(OSIFLAGS) -c $(SRCDIR)/lpif.cc -o $@
	$(STRIP) $@

# create the LP solver specific interface library
#
# $@ The name of the current target
INTERFACES =  osi 

interface: subdirs $(INTERFACES:%=$(LIBDIR)/stuff/interface-%.a)

$(LIBDIR)/stuff/interface-osi.a: $(OSI_O)
	rm -f $@
	@$(PRE_ARCHIVE) $(OSI_O)
	$(ARCHIVE) $@ $(OSI_O)

# The basic ABACUS library
abacus-base:	$(LIBDIR)/stuff/abacus-base.a

$(LIBDIR)/stuff/abacus-base.a: $(ABACUS_O)
	rm -f $@
	@$(PRE_ARCHIVE) $(ABACUS_O)
	$(ARCHIVE) $@ $(ABACUS_O)

# Build the library distribution directory (lib and lib/stuff)
lib-all: subdirs abacus-base lpif interface $(ADDITIONAL_LIB_ALL)

# Create a tar file containing all necessary stuff to compile ABACUS on
# a remote system.
#
# This file contains all *.cc *.h *.inc files at the correct locations.
#
# The following steps have to be done at the remote system:
#
# - unpack aba.tar.gz in the working directory
# - gmake abacus ABACUS_MAKE_SETTINGS=<system>
#
# In Make-settings/<system> the variable OUTPUTDIR should be set to the current
# directory (OUTPUTDIR=.)
tar:	subdirs $(ABACUS_HEADERS) $(ABACUS_ALL_CC)
	tar --exclude-vcs -czf abacus.tar.gz \
		.abacus \
		abacus-doxygen \
		Make* \
		ETI \
		Include \
		INSTALL \
		README \
		AUTHORS \
		COPYRIGHT \
		CHANGES \
		ReleaseNotes.3.2beta \
		sources \
		example \
		Tex

#######################################################################
#
# make abacus and install
#
#######################################################################
have_lib = $(wildcard $(LIBDIR)/libabacus-osi.a)

abacus: lib-all
	cp $(LIBDIR)/stuff/abacus-base.a $(LIBDIR)/libabacus-osi.a
	mkdir $(LIBDIR)/temp
	cd $(LIBDIR)/temp; ar x ../stuff/interface-osi.a 
	find $(LIBDIR)/temp -name \*.o | xargs -n1 ar r $(LIBDIR)/libabacus-osi.a
	find $(LIBDIR)/stuff -name \*.o | xargs -n1 ar r $(LIBDIR)/libabacus-osi.a
	rm -r $(LIBDIR)/temp
	ranlib  $(LIBDIR)/libabacus-osi.a


install: abacus 
ifneq ($(strip $(ABACUS_INSTALL_LIBDIR)),)
	mkdir -p $(ABACUS_INSTALL_LIBDIR)
else
	$(error Set ABACUS_INSTALL_HEADERDIR and ABACUS_INSTALL_LIBDIR in the Makefile)
endif
ifneq ($(strip $(ABACUS_INSTALL_HEADERDIR)),)
	mkdir -p $(ABACUS_INSTALL_HEADERDIR)
	find Include/abacus/ -name \*.h | xargs -n1 cp -t $(ABACUS_INSTALL_HEADERDIR)
	find Include/abacus/ -name \*.inc | xargs -n1 cp -t $(ABACUS_INSTALL_HEADERDIR)
endif
ifneq ($(strip $(have_lib)),)
	mv $(LIBDIR)/libabacus-osi.a $(ABACUS_INSTALL_LIBDIR)
	@echo "ABACUS-LIBRARY installed to " $(ABACUS_INSTALL_LIBDIR)
else
	$(error no library found. Please run: make abacus)
endif 



#######################################################################
#
# Documentation
#
#######################################################################

# for the tex classes
TEXDIR          = $(OUTPUTDIR)/tmp
TEXCLASSDIR     = $(TEXDIR)/classes
TEXSRCDIR       = Tex

MANUAL_WEBS     = $(ABACUS_ALL_C) $(ABACUS_ALL_H) $(ABACUS_INC) \
                  $(PUBLICDOC) $(PRIVATEDOC) $(PROTECTEDDOC)

TEX_CLASSES  = $(MANUAL_WEBS:%=$(TEXCLASSDIR)/%.tex)

manual: subdirs doxygendoc
	$(GCP) -a $(TEXSRCDIR)/* $(TEXDIR)      
	$(GCP) -a $(TEXSRCDIR)/../latex/* $(TEXDIR)      
	-cd $(TEXDIR);latex manual
	-cd $(TEXDIR);bibtex manual
	-cd $(TEXDIR);makeindex -s index.sty manual
	-cd $(TEXDIR);latex manual
	-cd $(TEXDIR);latex manual
	-cd $(TEXDIR);dvipdf manual.dvi ../manual.pdf;

PAR_MANUAL_WEBS = $(ABACUS_PARALLEL_W) $(ABACUS_PARALLEL_WH) \
                  $(ABACUS_PARALLEL_WINC)

manual-par: subdirs $(PAR_MANUAL_WEBS:%=$(TEXCLASSDIR)/%.tex)
	$(GCP) -a $(TEXSRCDIR)/* $(TEXDIR)      
	cd $(TEXDIR);latex manual-par

manualhtml: subdirs doxygendoc
	$(GCP) -a $(TEXSRCDIR)/* $(TEXDIR)      
	$(GCP) -a $(TEXSRCDIR)/../latex/* $(TEXDIR)      
	-cd $(TEXDIR);latex manual
	-cd $(TEXDIR);bibtex manual
	-cd $(TEXDIR);makeindex -s index.sty manual
	-cd $(TEXDIR);latex manual
	-cd $(TEXDIR);mkdir ../manualhtml
	-cd $(TEXDIR);mk4ht htlatex manual.tex "html,3" "" "-d../manualhtml/"


.PHONY: doxygendoc
doxygendoc:
	doxygen abacus-doxygen


#######################################################################
#
# DEPENDENCY section
#
# works with gcc only
#
#######################################################################

# rebuild the dependencies
depend: subdirs $(ABACUS_HEADERS) $(ABACUS_ALL_CC)
	$(CCC) -MM  $(CCFLAGS) \
		$(ABACUS_C:%.cc=$(SRCDIR)/%.cc) \
		| sed "s~^\(.*\)\.o:~$(OBJDIR)/\1.o:~" >$(DEPEND)
-include $(DEPEND)


# DO NOT DELETE
