/*!\file
 * \author  Carsten Gutwenger, Markus Chimani, modified by Mark Sprenger
 *
 * This class adds the overall exception handling to ABACUS 
 *
 * \par License:
 * This file is part of ABACUS - A Branch And CUt System
 * Copyright (C) 1995 - 2003                                                  
 * University of Cologne, Germany                                             
 * 
 * \par                                                                           
 * This library is free software; you can redistribute it and/or              
 * modify it under the terms of the GNU Lesser General Public                 
 * License as published by the Free Software Foundation; either               
 * version 2.1 of the License, or (at your option) any later version.         
 *     
 * \par                                                                       
 * This library is distributed in the hope that it will be useful,            
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU          
 * Lesser General Public License for more details.                            
 *     
 * \par                                                                       
 * You should have received a copy of the GNU Lesser General Public           
 * License along with this library; if not, write to the Free Software        
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA  
 *
 * \see http://www.gnu.org/copyleft/gpl.html
 */

#ifndef ABA_EXCEPTIONS_H
#define ABA_EXCEPTIONS_H

#include <stdio.h>
#include <iostream>

#ifdef THROW
#undef THROW
#endif

#ifdef ABACUS_THROW_WITH_INFO
#define THROW_PARAM(CLASS, PARAM, ERROR_STRING) throw CLASS ( PARAM , __FILE__ , __LINE__ , ERROR_STRING )
#define THROW(CLASS, ERROR_STRING)              throw CLASS ( __FILE__ , __LINE__ , ERROR_STRING )
#else

#define THROW_PARAM(CLASS, PARAM, ERROR_STRING) throw CLASS ( PARAM, ERROR_STRING )
#define THROW(CLASS, ERROR_STRING)              throw CLASS ( ERROR_STRING )

#endif


	enum AlgorithmFailureCode {
		pvcIllegalParameter,
		afcPrimalBound, 
		afcDualBound,
		afcNotInteger,
		afcBuffer,
		afcAddVar,
		afcSorter,
		afcPhase,
		afcActive,
		afcNoSolution,
		afcMakeFeasible,
		afcGuarantee,
		afcBranchingVariable,
		afcStrategy,
		afcCloseHalf,
		afcStandardPool,
		afcVariable,
		afcLpIf,
		afcLp,
		afcBstack,
		afcLpStatus,
		afcBranchingRule,
		afcFixSet,
		afcLpSub,
		afcString,
		afcConstraint,
		afcPool,
		afcGlobal,
		afcFsVarStat,
		afcOsiIf,
		afcConBranchRule,
		afcTimer,
		afcArray,
		afcCsense,
		afcBPrioQueue,
		afcFixCand,
		afcBHeap,
		afcPoolslot,
		afcSparVec,
		afcConvar,
		afcOstream,
		afcHash,
		afcParamaster,
		afcInfeasCon,
		afcUnknown
	}; // enum AlgorithmFailureCode


	class Exception { 

	private:

		const char *m_file; //!< Source file where exception occurred.
		int         m_line; //!< Line number where exception occurred.
		const char *m_exception; //!< The reason for throwing the exception.

	public:
		Exception(const char *_file = NULL, int _line = -1, const char *_exception = NULL) : 
		  m_file(_file),
		  m_line(_line),
		  m_exception(_exception)
		{
		}
		const char *file() { return m_file; }
		const char *exception() { return m_exception; }
		int line() { return m_line; }
	};

	class AlgorithmFailureException : public Exception
	{
	public:

		//! Constructs an algorithm failure exception.
		AlgorithmFailureException(AlgorithmFailureCode code,
			const char *_file = NULL,
			int _line = -1,
			const char *_exception = NULL) :
		Exception(_file,_line,_exception),
		m_exceptionCode(code)
		{
                #ifdef ABACUS_THROW_WITH_INFO
                std::cerr<<"exception occurred at ";
                std::cerr<<"Line: "<<line()<<" File: "<<file()<<" Code:"<<exceptionCode()<<endl;
                std::cerr<<exception()<<endl;
                #endif
		}

		//! Constructs an algorithm failure exception.
		AlgorithmFailureException(
			const char *file = NULL,
			int line = -1) :
		Exception(file, line),
		m_exceptionCode(afcUnknown)
		{}

		//! Returns the error code of the exception.
		AlgorithmFailureCode exceptionCode() const { return m_exceptionCode; }

	private:
		AlgorithmFailureCode m_exceptionCode; //!< The error code specifying the exception.
	}; // class AlgorithmFailureException



#endif
