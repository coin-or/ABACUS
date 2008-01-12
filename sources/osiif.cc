/*!\file
 * \author Frank Baumann
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

#include "abacus/master.h"
#include "abacus/lpmasterosi.h"
#include "abacus/lpsubosi.h"
#include "abacus/osiif.h"
#include "abacus/column.h"
#include "abacus/row.h"

#include <string.h>
#include <stdio.h>
#include <limits.h>

#include <math.h>

extern "C" {
#include <unistd.h>
}

void ABA_OSIIF::freeInt(int*& ptr)
{
   if( ptr != NULL )
   {
      delete [] ptr;
      ptr = NULL;
   }
}

void ABA_OSIIF::freeDouble(const double*& ptr)
{
   if( ptr != NULL )
   {
      delete [] ptr;
      ptr = NULL;
   }
}

void ABA_OSIIF::freeDouble(double*& ptr)
{
   if( ptr != NULL )
   {
      delete [] ptr;
      ptr = NULL;
   }
}

void ABA_OSIIF::freeChar(char*& ptr)
{
   if( ptr != NULL )
   {
      delete [] ptr;
      ptr = NULL;
   }
}

void ABA_OSIIF::freeChar(const char*& ptr)
{
   if( ptr != NULL )
   {
      delete [] ptr;
      ptr = NULL;
   }
}

ABA_OSIIF::ABA_OSIIF(ABA_MASTER *master) 
   : 
ABA_LP(master),
   osiLP_(NULL),
   value_(0.),
   xVal_(NULL),
   barXVal_(NULL),
   yVal_(NULL),
   cStat_(NULL),
   rStat_(NULL),
   rhs_(NULL),
   rowactivity_(NULL),
   rowsense_(NULL),
   colupper_(NULL),
   collower_(NULL),
   objcoeff_(NULL),
   ws_(NULL)
{
   lpMasterOsi_ = master->lpMasterOsi();
}

ABA_OSIIF::ABA_OSIIF(ABA_MASTER *master, 
      ABA_OPTSENSE sense, 
      int nRow, 
      int maxRow, 
      int nCol, 
      int maxCol,
      ABA_ARRAY<double> &obj, 
      ABA_ARRAY<double> &lb, 
      ABA_ARRAY<double> &ub,
      ABA_ARRAY<ABA_ROW*> &rows
      )
: 
ABA_LP(master),
   osiLP_(NULL),
   value_(0.),
   xVal_(NULL),
   barXVal_(NULL),
   yVal_(NULL),
   cStat_(NULL),
   rStat_(NULL),
   rhs_(NULL),
   rowactivity_(NULL),
   rowsense_(NULL),
   colupper_(NULL),
   collower_(NULL),
   objcoeff_(NULL),
   ws_(NULL)
{
   lpMasterOsi_ = master->lpMasterOsi();

   _initialize(sense, nRow, maxRow, nCol, maxCol, obj, lb, ub, rows);
}

ABA_OSIIF::~ABA_OSIIF()
{
   delete ws_;
   delete osiLP_;
}

void ABA_OSIIF::_initialize(ABA_OPTSENSE sense, int nRow, int maxRow,
      int nCol, int maxCol,
      ABA_ARRAY<double> &obj, ABA_ARRAY<double> &lBound,
      ABA_ARRAY<double> &uBound, ABA_ARRAY<ABA_ROW*> &rows)
{
   osiLP_ = getDefaultInterface();
   currentSolverType_ = Exact;


   // switch off output from the solver
   // can be reset in setSolverParameters
   osiLP_->messageHandler()->setLogLevel(0);
   master_->setSolverParameters(osiLP_, currentSolverType());

   
   numRows_ = nRow;
   numCols_ = nCol;
   double *lbounds = new double[numCols_]; 
   double *ubounds = new double[numCols_]; 
   double *objectives = new double[numCols_]; 

   CoinPackedVector *coinrow = new CoinPackedVector();
   CoinPackedMatrix *matrix =  new CoinPackedMatrix(false,0,0);
   matrix->setDimensions(0, numCols_);

   for (int i = 0; i < numCols_; i++){
      lbounds[i] = lBound[i];
      ubounds[i] = uBound[i];
      objectives[i] = obj[i];
   }

   if (currentSolverType() == Exact && numRows_ == 0 && master_->defaultLpSolver() == ABA_MASTER::CPLEX) {
      loadDummyRow(osiLP_, lbounds, ubounds, objectives);
   }
   else {
      char *senses = new char[numRows_]; 
      double *rhs = new double[numRows_]; 
      double *ranges = new double[numRows_]; 

      for (int i = 0; i < numRows_; i++){
	 coinrow->clear();
	 for (int j = 0; j < rows[i]->nnz(); j++){
	    coinrow->insert(rows[i]->support(j), rows[i]->coeff(j));
	 }
	 matrix->appendRow(*coinrow);
	 senses[i] = csense2osi(rows[i]->sense());
	 rhs[i] = rows[i]->rhs();
	 ranges[i] = 0.0;
      }
      lpSolverTime_.start();
      osiLP_->loadProblem(*matrix, lbounds, ubounds, objectives, senses, rhs, ranges);
      lpSolverTime_.stop();
      
      freeChar(senses);
      freeDouble(rhs);
      freeDouble(rhs);
   }

   // set the sense of the optimization
   _sense(sense);

   // get the pointers to the solution, reduced costs etc.
   lpSolverTime_.start();
   numRows_ = osiLP_->getNumRows();
   numCols_ = osiLP_->getNumCols();
   rhs_ = osiLP_->getRightHandSide();
   rowsense_ = osiLP_->getRowSense();
   colupper_ = osiLP_->getColUpper();
   collower_ = osiLP_->getColLower();
   objcoeff_ = osiLP_->getObjCoefficients();
   value_ = osiLP_->getObjValue();
   delete ws_;
   ws_ = (CoinWarmStartBasis*)osiLP_->getWarmStart();

   xValStatus_ = recoStatus_ = yValStatus_ = slackStatus_ = basisStatus_ = Missing;
   lpSolverTime_.stop();

   delete coinrow;
   delete matrix;
   freeDouble(lbounds);
   freeDouble(ubounds);
   freeDouble(objectives);

}


void ABA_OSIIF::_loadBasis(ABA_ARRAY<ABA_LPVARSTAT::STATUS> &lpVarStat,
      ABA_ARRAY<ABA_SLACKSTAT::STATUS> &slackStat)
{
   int lps = lpVarStat.size();
   int sls = slackStat.size();

   CoinWarmStartBasis *ws = NULL;
   ws = new CoinWarmStartBasis();
   ws->setSize(numCols_, numRows_);

   if (osiLP_->getNumCols() > lps) {
      master_->err() << "ABA_OSIIF::_loadBasis: mismatch in number of columns: OSI " << osiLP_->getNumCols() << "  Abacus " << lps << endl;
      exit(Fatal);
   }
   for (int i = 0; i < numCols_; i++) 
      ws->setStructStatus(i, lpVarStat2osi(lpVarStat[i]));

   if (osiLP_->getNumRows() > sls) {
      master_->err() << "ABA_OSIIF::_loadBasis: mismatch in number of rows: OSI  " << osiLP_->getNumRows() << "  Abacus " << sls << endl;
      exit(Fatal);
   }
   for (int i = 0; i < numRows_; i++) 
      ws->setArtifStatus(i, slackStat2osi(slackStat[i]));

   lpSolverTime_.start();
   slackStatus_ = basisStatus_ = Missing;
   int status = 0;
   // FIXME loadBasis
   // better test whether the number of basic structurals is correct?
   if (ws->numberBasicStructurals() > 0) {
      status = osiLP_->setWarmStart(dynamic_cast<CoinWarmStart *>(ws));
      delete ws_;
      ws_ = (CoinWarmStartBasis*)osiLP_->getWarmStart();
      cStat_ = ws_->getStructuralStatus();
      rStat_ = ws_->getArtificialStatus();
      basisStatus_ = Available;
   }
   else status = 2;
   lpSolverTime_.stop();

   delete ws;

   if (status == 0) {
      master_->err() << "ABA_OSIIF::_loadBasis(): loading the new basis has failed. Status " << status << endl;
      // FIXME loadBasis
      // exit(Fatal);
      return;
   }
   else 
      return;
}  

int ABA_OSIIF::_getInfeas(int &infeasRow, int &infeasCol, double *bInvRow)
{
   // This is only implemented in CPLEX and rarely used.
   master_->err() << "ABA_OSIIF::_getInfeas(): currently not available" << endl;
   exit(Fatal);
   return 1;
}

void ABA_OSIIF::_remRows(ABA_BUFFER<int> &ind)
{
   const int n = ind.number();
   int *indices = new int[n];

   for (int i = 0; i < n; i++) {
   indices[i] = ind[i];
   } 

   lpSolverTime_.start();
   osiLP_->deleteRows(n, indices);
   numRows_ = osiLP_->getNumRows();
   rowsense_ = osiLP_->getRowSense();
   rhs_ = osiLP_->getRightHandSide();
   lpSolverTime_.stop();

   freeInt(indices);
}

void ABA_OSIIF::_addRows(ABA_BUFFER<ABA_ROW*> &rows)
{
   CoinPackedVector *coinrow = new CoinPackedVector();

   for (int i = 0; i < rows.number(); i++) {
      coinrow->clear();
      for (int j = 0; j < rows[i]->nnz(); j++){
	 coinrow->insert(rows[i]->support(j), rows[i]->coeff(j));
      }
      lpSolverTime_.start();
      osiLP_->addRow(*coinrow, csense2osi(rows[i]->sense()), rows[i]->rhs(), 0.0);
      lpSolverTime_.stop();
   }

   delete coinrow;
   lpSolverTime_.start();
   numRows_ = osiLP_->getNumRows();
   rhs_ = osiLP_->getRightHandSide();
   numCols_ = osiLP_->getNumCols();
   collower_ = osiLP_->getColLower();
   colupper_ = osiLP_->getColUpper();
   lpSolverTime_.stop();

}

void ABA_OSIIF::_remCols(ABA_BUFFER<int> &vars)
{
   int num = vars.number();
   int *indices = new int[num];

   for (int i = 0; i < num; i++)
      indices[i] = vars[i];

   lpSolverTime_.start();
   osiLP_->deleteCols(num, indices);
   numCols_ = osiLP_->getNumCols();
   collower_ = osiLP_->getColLower();
   colupper_ = osiLP_->getColUpper();
   objcoeff_ = osiLP_->getObjCoefficients();
   lpSolverTime_.stop();

   freeInt(indices);
}

void ABA_OSIIF::_addCols(ABA_BUFFER<ABA_COLUMN*> &newCols)
{
   int num;
   double ub, lb, obj;
   int  *supports; //!< supports of added rows
   double  *coeffs; //!< coefficients of added rows
   CoinPackedVector *newcol = new CoinPackedVector;

   for (int i = 0; i < newCols.number(); i++) {
      num = newCols[i]->nnz();
      ub =  newCols[i]->uBound();
      lb =  newCols[i]->lBound();
      obj =  newCols[i]->obj();
      supports = new int[num];
      coeffs = new double[num];

      for (int j = 0; j < num; j++) {
	 supports[j] = newCols[i]->support(j);
	 coeffs[j] = newCols[i]->coeff(j);
      }

      newcol->setVector(num, supports, coeffs);
      lpSolverTime_.start();
      osiLP_->addCol(*newcol, lb, ub, obj);
      lpSolverTime_.stop();

      freeInt(supports);
      freeDouble(coeffs);
   }

   lpSolverTime_.start();
   numCols_ = osiLP_->getNumCols();
   collower_ = osiLP_->getColLower();
   colupper_ = osiLP_->getColUpper();
   objcoeff_ = osiLP_->getObjCoefficients();
   lpSolverTime_.stop();
   delete newcol;
}

void ABA_OSIIF::_changeRhs(ABA_ARRAY<double> &newRhs)
{
   lpSolverTime_.start();

   for (int i = 0; i < newRhs.size(); i++) 
      osiLP_->setRowType(i, rowsense_[i], newRhs[i], 0);

   rowsense_ = osiLP_->getRowSense();
   rhs_ = osiLP_->getRightHandSide();
   lpSolverTime_.stop();
}

void ABA_OSIIF::_changeLBound(int i, double newLb)
{
   lpSolverTime_.start();

   osiLP_->setColLower(i, newLb);
   collower_ = osiLP_->getColLower();

   lpSolverTime_.stop();
}

void ABA_OSIIF::_changeUBound(int i, double newUb)
{
   lpSolverTime_.start();

   osiLP_->setColUpper(i, newUb);
   colupper_ = osiLP_->getColUpper();

   lpSolverTime_.stop();
}

int ABA_OSIIF::_pivotSlackVariableIn(ABA_BUFFER<int> &rows)
{
   master_->err() << "ABA_OSIIF::_getInfeas(): currently not available" << endl;
   return 1;
}


ABA_LP::OPTSTAT ABA_OSIIF::_primalSimplex()
{
   lpSolverTime_.start();

   // switch the interface, if necessary
   if (currentSolverType_ != Exact){
      currentSolverType_ = Exact;
      osiLP_ = switchInterfaces(Exact);
   }
   osiLP_->initialSolve();

   lpSolverTime_.stop();

   // check for solver statuses 
   if (osiLP_->isAbandoned()){
      master_->err() << "ABA_OSIIF::_primalSimplex(): ";
      master_->err() << "Warning: solver Interface reports status isAbandoned" << endl;
      master_->err() << "There have been numerical difficulties, aborting...";
      exit(Fatal);
   }

   // get information about the solution
   getSol();

   // The order is important here
   if (osiLP_->isProvenOptimal()) return Optimal;
   if (osiLP_->isProvenPrimalInfeasible()) return Infeasible;
   if (osiLP_->isProvenDualInfeasible()) return Unbounded;
   else {
      master_->err() << "ABA_OSIIF::_primalSimplex(): ";
      master_->err() << "unable to determine status of LP, aborting...";
      exit(Fatal);
      return Infeasible; //!< to suppress compiler warning
   }
}

ABA_LP::OPTSTAT ABA_OSIIF::_dualSimplex()
{
   lpSolverTime_.start();

   // switch the interface, if necessary
   if (currentSolverType_ != Exact){
      currentSolverType_ = Exact;
      osiLP_ = switchInterfaces(Exact);
   }
   osiLP_->resolve();

   lpSolverTime_.stop();

   // check for solver statuses 
   if (osiLP_->isAbandoned()){
      master_->err() << "ABA_OSIIF::_dualSimplex(): ";
      master_->err() << "Warning: solver Interface reports staus isAbandoned" << endl;
      master_->err() << "There have been numerical difficulties, aborting...";
      exit(Fatal);
   }

   // get information about the solution
   getSol();

   // The order is important here
   if (osiLP_->isProvenOptimal()) return Optimal;
   if (osiLP_->isProvenPrimalInfeasible()) return Infeasible;
   if (osiLP_->isProvenDualInfeasible()) return Unbounded;
   else {
      master_->err() << "ABA_OSIIF::_dualSimplex(): ";
      master_->err() << "unable to determine status of LP, aborting...";
      exit(Fatal);
      return Infeasible; //!< to suppress compiler warning
   }
}

ABA_LP::OPTSTAT ABA_OSIIF::_barrier(bool doCrossover)
{
      // The barrier method is not implemented in Osi.
      // We use the primal simplex method instead
   master_->err() << "ABA_OSIIF::_barrier: Sorry, Osi implements no barrier method." << endl;
   master_->err() << "Using primal simplex method instead." << endl;
   return _primalSimplex();
}

ABA_LP::OPTSTAT ABA_OSIIF::_approx()
{
   lpSolverTime_.start();

   // switch the interface, if necessary
   if (currentSolverType() != Approx){
      currentSolverType_ = Approx;
      osiLP_ = switchInterfaces(Approx);
   }
   osiLP_->resolve();

   lpSolverTime_.stop();

   // check for solver statuses 
   if (osiLP_->isAbandoned()){
      master_->err() << "ABA_OSIIF::_approx(): ";
      master_->err() << "Warning: solver Interface reports staus isAbandoned" << endl;
      master_->err() << "There have been numerical difficulties, aborting...";
      exit(Fatal);
   }

   // get information about the solution
   getSol();

   // The order is important here
   if (osiLP_->isProvenOptimal()) return Optimal;
   if (osiLP_->isProvenPrimalInfeasible()) return Infeasible;
   if (osiLP_->isProvenDualInfeasible()) return Unbounded;
   else {
      master_->err() << "ABA_OSIIF::_approx(): ";
      master_->err() << "unable to determine status of LP, assume the solution is optimal..." << endl;
      return Optimal; 
   }
}


ABA_OPTSENSE ABA_OSIIF::_sense() const
{
   ABA_OPTSENSE s;

   double osiSense = osiLP_->getObjSense();

   if (osiSense == 1.)
      s.sense(ABA_OPTSENSE::Min);
   else
      s.sense(ABA_OPTSENSE::Max);

   return s;
}

void ABA_OSIIF::_sense(const ABA_OPTSENSE &newSense)
{
   if (newSense.unknown()) {
      master_->err() << "ABA_OSIIF::_sense: The objective sense can not be set to 'unknown' with OSI." << endl;
      exit(Fatal);
   }
   else if (newSense.max())
      osiLP_->setObjSense(-1.);
   else 
      osiLP_->setObjSense(1.);
}

int ABA_OSIIF::_nRow() const
{
   return numRows_;
}

int ABA_OSIIF::_maxRow() const
{
   // Size management is completely done by Osi!
   return numRows_;
}


int ABA_OSIIF::_nCol() const
{
   return numCols_;
}

int ABA_OSIIF::_maxCol() const
{
   // Size management is completely done by Osi!
   return numCols_;
}

double ABA_OSIIF::_obj(int i) const
{
   return objcoeff_[i];
}

double ABA_OSIIF::_lBound(int i) const
{
   return collower_[i];
}

double ABA_OSIIF::_uBound(int i) const
{
   return colupper_[i];
}

double ABA_OSIIF::_rhs(int i) const
{
   return rhs_[i];
}

void ABA_OSIIF::_row(int i, ABA_ROW &r) const
{
   const CoinPackedMatrix* coinMatrix;
   CoinPackedVector coinVector;
   int coinNumEl;
   const int* coinIndices;
   const double* coinElements;

   coinMatrix = osiLP_->getMatrixByRow();
   coinVector = coinMatrix->getVector(i);
   coinNumEl = coinVector.getNumElements();
   coinIndices = coinVector.getIndices();
   coinElements = coinVector.getElements();

   r.clear();

   for (int j = 0; j < coinNumEl; j++)
      r.insert(coinIndices[j], coinElements[j]);
   r.sense(osi2csense(rowsense_[i]));
   r.rhs(_rhs(i));
}

int ABA_OSIIF::_nnz() const
{
   return osiLP_->getNumElements();
}

double ABA_OSIIF::_value() const
{
   return value_;
}

double ABA_OSIIF::_barXVal(int i)
{
   // The barrier algorithm is only supported by cplex
   // support may be added later
   master_->err() << "ABA_OSIIF::_barXVal: The barrier algorithm is currently not supported" << endl;
   exit(Fatal);
   return barXValStatus_ ? 0. : barXVal_[i];
   return 1.;
}

double ABA_OSIIF::_reco(int i)
{
   //return reco_[i];
   return osiLP_->getReducedCost()[i];
}

double ABA_OSIIF::_slack(int i)
{
      double rhs = rhs_[i];
      double ract = rowactivity_[i];
      switch (rowsense_[i]){
	 case 'L':
	    return rhs - ract;
	 case 'G':
	    return ract - rhs;
	 case 'E':
	    return 0.0;
	 default:
	    master_->err() << "ABA_OSIIF::_slack : slack not defined for sense " << rowsense_[i] << " for row " << i << " of " << osiLP_->getNumRows() << endl;
	    exit(Fatal);
	    return 0.0; //!< to suppress compiler warning
      }
}

void ABA_OSIIF::getSol()
{
#ifdef OSIDEBUG
   master_->err() << "<> isProvenOptimal: " << osiLP_->isProvenOptimal() << endl;
   master_->err() << "<> isProvenDualInfeasible: " << osiLP_->isProvenDualInfeasible() << endl;
   master_->err() << "<> isProvenPrimalInfeasible: " << osiLP_->isProvenPrimalInfeasible() << endl;
   master_->err() << "<> isProvenDualInfeasible: " << osiLP_->isProvenDualInfeasible() << endl;
   master_->err() << "<> isAbandoned: " << osiLP_->isAbandoned() << endl;
   master_->err() << "<> isPrimalObjetiveLimitReached: " << osiLP_->isPrimalObjectiveLimitReached() << endl;
   master_->err() << "<> isDualObjetiveLimitReached: " << osiLP_->isDualObjectiveLimitReached() << endl;
   master_->err() << "<> isIterationLimitReached: " << osiLP_->isIterationLimitReached() << endl;
   double POL;
   osiLP_->getDblParam(OsiPrimalObjectiveLimit, POL);
   master_->err() << "<> PrimalObjectiveLimit: " << POL << endl;
   master_->err() << "<> ObjectiveSense: " << osiLP_->getObjSense() << endl;
   master_->err() << "<> ObjectiveValue: " << osiLP_->getObjValue() << endl;
#endif

   lpSolverTime_.start();
   // get the solution
   xValStatus_ = recoStatus_ = yValStatus_ = slackStatus_ = basisStatus_ = Missing;

   numCols_ = osiLP_->getNumCols();
   numRows_ = osiLP_->getNumRows();
   collower_ = osiLP_->getColLower();
   colupper_ = osiLP_->getColUpper();
   objcoeff_ = osiLP_->getObjCoefficients();
   rhs_ = osiLP_->getRightHandSide();
   rowsense_ = osiLP_->getRowSense();

   if( !osiLP_->isProvenPrimalInfeasible() && !osiLP_->isAbandoned() ) {
      xVal_ = osiLP_->getColSolution();
      xValStatus_ = Available;
      rowactivity_ = osiLP_->getRowActivity();
      slackStatus_ = Available;
      recoStatus_ = Available;
   }

   if( !osiLP_->isProvenDualInfeasible() && !osiLP_->isAbandoned() ) {
      yVal_ = osiLP_->getRowPrice();
      yValStatus_ = Available;
      recoStatus_ = Available;
   }

   if( osiLP_->isProvenOptimal() || osiLP_->isIterationLimitReached() ) {
      value_ = osiLP_->getObjValue();
      reco_ = osiLP_->getReducedCost();
      recoStatus_ = Available;
      // get information about the basis 
      if( currentSolverType() != Approx) {
	 delete ws_;
	 ws_ = (CoinWarmStartBasis*)osiLP_->getWarmStart();
	 cStat_ = ws_->getStructuralStatus();
	 rStat_ = ws_->getArtificialStatus();
	 basisStatus_ = Available;
      }
   }

   lpSolverTime_.stop();

}

void ABA_OSIIF::_rowRealloc(int newSize)
{
   // Memory management is completely handled by Osi
   //master_->err() << "ABA_OSIIF::_rowRealloc : This function does not do anything. Memory management is completely handled by Osi." << endl;
}

void ABA_OSIIF::_colRealloc(int newSize)
{
   // Memory management is completely handled by Osi
   //master_->err() << "ABA_OSIIF::_colRealloc : This function does not do anything. Memory management is completely handled by Osi." << endl;
}

int ABA_OSIIF::_setSimplexIterationLimit(int limit)
{
   return(!osiLP_->setIntParam(OsiMaxNumIteration, limit));
}

int ABA_OSIIF::_getSimplexIterationLimit(int &limit)
{
   return(!osiLP_->getIntParam(OsiMaxNumIteration, limit));
}

double ABA_OSIIF::_xVal(int i)
{
   return xVal_[i];
}

double ABA_OSIIF::_yVal(int i)
{
   return yVal_[i];
}

ABA_LPVARSTAT::STATUS ABA_OSIIF::osi2lpVarStat(CoinWarmStartBasis::Status stat) const
{
   switch (stat) {
      case CoinWarmStartBasis::isFree:
	 return ABA_LPVARSTAT::NonBasicFree;
      case CoinWarmStartBasis::basic:
	 return ABA_LPVARSTAT::Basic;
      case CoinWarmStartBasis::atUpperBound:
	 return ABA_LPVARSTAT::AtUpperBound;
      case CoinWarmStartBasis::atLowerBound:
	 return ABA_LPVARSTAT::AtLowerBound;
      default:
	 master_->err() << "ABA_OSIIF::osi2lpVarStat(" << stat << "): ";
	 master_->err() << "unknown status" << endl;
	 exit(Fatal);
	 return ABA_LPVARSTAT::AtLowerBound; //!< to suppress compiler warning
   }
}

CoinWarmStartBasis::Status ABA_OSIIF::slackStat2osi(ABA_SLACKSTAT::STATUS stat) const
{
   switch(stat) {
      case ABA_SLACKSTAT::NonBasicZero:
	 return CoinWarmStartBasis::atLowerBound;
      case ABA_SLACKSTAT::Basic:
	 return CoinWarmStartBasis::basic;
      case ABA_SLACKSTAT::NonBasicNonZero:
	 return CoinWarmStartBasis::atUpperBound;
      case ABA_SLACKSTAT::Unknown:
	 return CoinWarmStartBasis::atLowerBound;
      default: 
	 master_->err() << "ABA_OSIIF::slackStat2osi(" << (int) stat << "): ";
	 master_->err() << " corresponding OSI status unknown" << endl;
	 exit (Fatal);
	 return CoinWarmStartBasis::atLowerBound; //!< to suppress compiler warning
   }
}

ABA_SLACKSTAT::STATUS ABA_OSIIF::osi2slackStat(CoinWarmStartBasis::Status stat) const
{
   switch (stat) {
      case CoinWarmStartBasis::atLowerBound:
	 return ABA_SLACKSTAT::NonBasicZero;
      case CoinWarmStartBasis::atUpperBound:
	 return ABA_SLACKSTAT::NonBasicNonZero;
      case CoinWarmStartBasis::basic:
	 return ABA_SLACKSTAT::Basic;
      case CoinWarmStartBasis::isFree:
      default: master_->err() << "ABA_OSIIF::osi2slackStat(" << int(stat) << "): ";
	       master_->err() << "unknown status" << endl;
	       exit(Fatal);
	       return ABA_SLACKSTAT::NonBasicZero; //!< to suppress compiler warning
   }
}

ABA_LPVARSTAT::STATUS ABA_OSIIF::_lpVarStat(int i)
{
   return basisStatus_ ? ABA_LPVARSTAT::Unknown : osi2lpVarStat(getStatus(cStat_, i));
}

ABA_SLACKSTAT::STATUS ABA_OSIIF::_slackStat(int i)
{
   return slackStatus_ ? ABA_SLACKSTAT::Unknown : osi2slackStat(getStatus(rStat_, i));
}

char ABA_OSIIF::csense2osi(ABA_CSENSE *sense) const
{
   switch(sense->sense()) {
      case ABA_CSENSE::Less:  return 'L';
      case ABA_CSENSE::Equal:  return 'E';
      case ABA_CSENSE::Greater: return 'G';
      default: master_->err() << "ABA_OSIIF::csense2osi(" << *sense << "):";
	       master_->err() << "unknown sense" << endl;
	       exit(Fatal);
	       return 'G'; //!< to suppress compiler warning
   }
}

ABA_CSENSE::SENSE ABA_OSIIF::osi2csense(char sense) const
{
   switch(sense) {
      case 'L': return ABA_CSENSE::Less;
      case 'E': return ABA_CSENSE::Equal;
      case 'G': return ABA_CSENSE::Greater;
      default: master_->err() << "ABA_OSIIF::osi2csense(" << sense << ")" << endl;
	       master_->err() << "unknown sense" << endl;
	       exit(Fatal);
	       return ABA_CSENSE::Less; //!< to suppress compiler warning
   }
}

CoinWarmStartBasis::Status ABA_OSIIF::lpVarStat2osi(ABA_LPVARSTAT::STATUS stat) const
{
   // Status Unknown <--> atlowerBound
   switch (stat) {
      case ABA_LPVARSTAT::AtLowerBound: 
	 return CoinWarmStartBasis::atLowerBound;
      case ABA_LPVARSTAT::AtUpperBound: 
	 return CoinWarmStartBasis::atUpperBound;
      case ABA_LPVARSTAT::Basic: 
	 return CoinWarmStartBasis::basic;
      case ABA_LPVARSTAT::NonBasicFree:
	 return CoinWarmStartBasis::isFree;
      case ABA_LPVARSTAT::Unknown:
	 return CoinWarmStartBasis::atLowerBound;
      default: 
	 master_->err() << "ABA_OSIIF::lpVarStat2osi(" << (int) stat << "): ";
	 master_->err() << "unknown status" << endl;
	 exit(Fatal);
	 return CoinWarmStartBasis::isFree; //!< to suppress compiler warning
   }
}

OsiSolverInterface* ABA_OSIIF::getDefaultInterface()
{
   switch(master_->defaultLpSolver()) {
#ifdef OSI_CBC
      case ABA_MASTER::Cbc:
	 return new OsiCbcSolverInterface;
	 break;
#endif
#ifdef OSI_CLP
      case ABA_MASTER::Clp:
	 return new OsiClpSolverInterface;
	 break;
#endif
#ifdef OSI_CPLEX
      case ABA_MASTER::CPLEX:
	 return new OsiCpxSolverInterface;
	 break;
#endif
#ifdef OSI_DYLP
      case ABA_MASTER::DyLP:
	 return new OsiDylpSolverInterface;
	 break;
#endif
#ifdef OSI_FORTMP
      case ABA_MASTER::FortMP:
	 return new OsiFmpSolverInterface;
	 break;
#endif
#ifdef OSI_GLPK
      case ABA_MASTER::GLPK:
	 return new OsiGlpkSolverInterface;
	 break;
#endif
#ifdef OSI_MOSEK
      case ABA_MASTER::MOSEK:
	 return new OsiMskSolverInterface;
	 break;
#endif
#ifdef OSI_OSL
      case ABA_MASTER::OSL:
	 return new OsiOslSolverInterface;
	 break;
#endif
#ifdef OSI_SOPLEX
      case ABA_MASTER::SoPlex:
	 return new OsiSpxSolverInterface;
	 break;
#endif
#ifdef OSI_SYMPHONY
      case ABA_MASTER::SYMPHONY:
	 return new OsiSymSolverInterface;
	 break;
#endif
#ifdef OSI_VOL
      case ABA_MASTER::Vol:
	 return new OsiVolSolverInterface;
	 break;
#endif
#ifdef OSI_XPRESS
      case ABA_MASTER::XPRESS_MP:
	 return new OsiXprSolverInterface;
	 break;
#endif
      default:
	 master_->err() << "No support for solver " << master_->defaultLpSolver() << " in Coin-Osi." << endl;
	 exit(Fatal);
   }
   return NULL; // dummy return, never reached
}


OsiSolverInterface* ABA_OSIIF::switchInterfaces(SOLVERTYPE newMethod) 
{
   OsiSolverInterface *s2 = NULL;
   if( newMethod == Exact ) 
      s2 = getDefaultInterface();
   else
#ifdef OSI_VOL
      // TODO switchInterfaces
      // s2 = getApproxInterface()
      s2 = new OsiVolSolverInterface;
#else
      master_->err() << "ABACUS has not been compiled with support for the Volume Algorithm, cannot switch to approximate solver." << endl;
      exit(Fatal);
#endif
   
   s2->messageHandler()->setLogLevel(0);
   master_->setSolverParameters(s2, currentSolverType());

   if (currentSolverType() == Exact && numRows_ == 0 && master_->defaultLpSolver() == ABA_MASTER::CPLEX) {
      loadDummyRow(s2, osiLP_->getColLower(), osiLP_->getColUpper(), osiLP_->getObjCoefficients());
   }
   else {
      s2->loadProblem(*osiLP_->getMatrixByCol(), osiLP_->getColLower(),
	    osiLP_->getColUpper(), osiLP_->getObjCoefficients(),
	    osiLP_->getRowLower(), osiLP_->getRowUpper());
   }
  
   s2->setObjSense(osiLP_->getObjSense());
   
   delete osiLP_;

   // get the pointers to the solution, reduced costs etc.
   rhs_ = s2->getRightHandSide();
   rowsense_ = s2->getRowSense();
   colupper_ = s2->getColUpper();
   collower_ = s2->getColLower();
   objcoeff_ = s2->getObjCoefficients();
   delete ws_;
   ws_ = (CoinWarmStartBasis*)s2->getWarmStart();

   xValStatus_ = recoStatus_= yValStatus_ = slackStatus_ = basisStatus_ = Missing;
   return s2;
}

void ABA_OSIIF::loadDummyRow(OsiSolverInterface* s2, const double* lbounds, const double* ubounds, const double* objectives)
{
   CoinPackedVector *coinrow = new CoinPackedVector();
   CoinPackedMatrix *matrix =  new CoinPackedMatrix(false,0,0);
   matrix->setDimensions(0, numCols_);
   ABA_BUFFER<int> dummy(master_, 1);
   dummy.push(0);
   char *senses = new char[1]; 
   double *rhs = new double[1]; 
   double *ranges = new double[1]; 
   coinrow->insert(0, 1.);
   matrix->appendRow(*coinrow);
   senses[0] = 'E';
   rhs[0] = 1.;
   ranges[0] = 0.;

   lpSolverTime_.start();
   s2->loadProblem(*matrix, lbounds, ubounds, objectives, senses, rhs, ranges);
   lpSolverTime_.stop();
   _remRows(dummy);

   delete coinrow;
   delete matrix;
   freeChar(senses);
   freeDouble(rhs);
   freeDouble(ranges);

   return;
}

