#ifndef OSI_INCLUDE_H
#define OSI_INCLUDE_H
#ifdef OSI_CBC
#include <OsiCbcSolverInterface.hpp>
#endif
#ifdef OSI_CLP
#include <OsiClpSolverInterface.hpp>
#endif
#ifdef OSI_CPLEX
#include <OsiCpxSolverInterface.hpp>
#endif
#ifdef OSI_DYLP
#include <OsiDylpSolverInterface.hpp>
#endif
#ifdef OSI_FORTMP
#include <OsiFmpSolverInterface.hpp>
#endif
#ifdef OSI_GLPK
#include <OsiGlpkSolverInterface.hpp>
#endif
#ifdef OSI_MOSEK
#include <OsiMskSolverInterface.hpp>
#endif
#ifdef OSI_OSL
#include <OsiOslSolverInterface.hpp>
#endif
#ifdef OSI_SOPLEX
#include <OsiSpxSolverInterface.hpp>
#endif
#ifdef OSI_SYMPHONY
#include <OsiSymSolverInterface.hpp>
#endif
#ifdef OSI_VOL
#include <OsiVolSolverInterface.hpp>
#endif
#ifdef OSI_XPRESS
#include <OsiXprSolverInterface.hpp>
#endif
#ifdef OSI_CSDP
#include <OsiCsdpSolverInterface.hpp>
#endif

#include <OsiSolverInterface.hpp>
#endif // end ifndef
