/*!\file
 * \author Matthias Elf
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

#include "abacus/lpsub.h"
#include "abacus/infeascon.h"
#include "abacus/row.h"
#include "abacus/constraint.h"
#include "abacus/sub.h"
#include "abacus/master.h"
#include "abacus/lpvarstat.h"
#include "abacus/variable.h"
#include "abacus/slackstat.h"
#include "abacus/column.h"
#include "abacus/exceptions.h"

  ABA_LPSUB::ABA_LPSUB (ABA_MASTER *master, const ABA_SUB *sub) 
  :  
    ABA_LP(master),  
    sub_(sub),  
    orig2lp_(master, sub->maxVar()),  
    lp2orig_(master, sub->maxVar()), 
    infeasCons_(master, sub->maxCon())
  { }

  ABA_LPSUB::~ABA_LPSUB()
  {
    const int nInfeasCons = infeasCons_.number();

    for(int c = 0; c < nInfeasCons; c++) delete infeasCons_[c];
  }

  void ABA_LPSUB::initialize()
  {
    // ABA_LPSUB::initialize(): local variables
    ABA_ARRAY<double> obj(master_, sub_->nVar());
    ABA_ARRAY<double> lBound(master_, sub_->nVar());
    ABA_ARRAY<double> uBound(master_, sub_->nVar());
    ABA_ARRAY<ABA_ROW*>   rows(master_, sub_->nCon());
    
    ABA_ARRAY<ABA_LPVARSTAT::STATUS> lpVarStat(master_, sub_->nVar());
    ABA_ARRAY<ABA_SLACKSTAT::STATUS> slackStat(master_, sub_->nCon());

    ABA_ROW   row(master_, sub_->nVar());                //!< buffer to store generated row
    int         conNnz;                          //!< number of nonzeros of constraint \a c
    int         c;                               //!< loop index

    // generate the row format of the active constraints
    /* After the generation of the row format we allocate a row of
     *   the correct length and make a copy in order to safe memory.
     */  
    int nRow = 0;

    const int nCon = sub_->nCon();
  
    for (c = 0; c < nCon; c++) {
      conNnz  = sub_->constraint(c)->genRow(sub_->actVar(), row);
      rows[nRow] = new ABA_ROW(master_, conNnz);
      rows[nRow]->copy(row);
      slackStat[nRow] = sub_->slackStat(c)->status();
      ++nRow;
      row.clear();
    }

    // eliminate set and fixed variables and initialize the columns
    ABA_VARIABLE   *v;                           //!< pointer to variable of subproblem
    ABA_ARRAY<bool> marked(master_, sub_->nVar(), false);  //!< \a true if variable can be eliminated

    nOrigVar_ = sub_->nVar();
    valueAdd_ = 0.0;

    // ABA_LPSUB: mark variables to eliminate, build objective function and bounds
    /* We mark all variables which can be eliminated, add them to the
     *   ABA_BUFFER \a delVar, compute the mappings from the original variable
     *   set to the actual variable set in the \a LP, and vice versa, and
     *   determine the correction term for the LP-value.

     *   If all variables can be eliminated then we do not eliminate the last
     *   variable for simpification. Otherwise it would be necessary to load
     *   an problem with 0 variables to the LP-solver which is, e.g., for
     *   Cplex not possible. Although the emulation of the optimization would
     *   still be simple, but extra work would have to be performed if later
     *   constraints were added.
     */  
    const int nVar = sub_->nVar();
    int nCol = 0;
    for (int i = 0; i < nVar; i++) {
      v = sub_->variable(i);
      if(sub_->fsVarStat(i)->fixedOrSet()) {
        if (eliminable(i) && (nCol || (i != sub_->nVar() - 1))) {
          
	  //! eliminate variable \a i from the LP
	  marked[i]  = true;
	  valueAdd_  += v->obj() * elimVal(i);
	  orig2lp_[i] = -1;

        }
        else {

	  // fix variable \a i in the LP
	  /* As variable \a i could not be eliminated we set both its upper and lower
	   *   bound to the value it is fixed or set to.
	   */  
	  orig2lp_[i]     = nCol;
	  lp2orig_[nCol]  = i;
	  obj[nCol]       = v->obj();
	  lBound[nCol]    = elimVal(i);
	  uBound[nCol]    = elimVal(i);
	  lpVarStat[nCol] = sub_->lpVarStat(i)->status();
	  ++nCol;

        }
      }
      else {

	// add variable \a i to the LP
	orig2lp_[i]     = nCol;
	lp2orig_[nCol]  = i;
	obj[nCol]       = v->obj();
	lBound[nCol]    = sub_->lBound(i);
	uBound[nCol]    = sub_->uBound(i);
	lpVarStat[nCol] = sub_->lpVarStat(i)->status();
	++nCol;
        
      }
    }

    // ABA_LPSUB: update the constraints
    /* If all active variables of a constraint are eliminated then
     *   its left hand side is void (implicitly 0), but its right hand side
     *   can be nonzero. Depending on the sense of the constraint it can be
     *   infeasible.
     *   If the elimination of variables from constraints causes an infeasible
     *   \a LP, the constraint is memorized in \a infeasCons_.
     */  
    ABA_BUFFER<int> delVar(master_, sub_->nVar()); //!< buffer of deletable components of row format
    double            rhsDelta;  //!< correction of right hand side due to eliminations
    ABA_INFEASCON::INFEAS infeas;    //!< infeasibility mode (TooLarge, TooSmall)

    for (c = 0; c < nCon; c++) {
      
      // eliminate the variables from the constraint
      delVar.clear();
      rhsDelta = 0.0;
      const int rNnz = rows[c]->nnz();
      for(int i = 0; i < rNnz; i++)
	if(marked[rows[c]->support(i)]) {
	  delVar.push(i);
	  rhsDelta += rows[c]->coeff(i)*elimVal(rows[c]->support(i));
	}

      rows[c]->delInd(delVar, rhsDelta);

      // check if the constraint is now infeasible
      if (rows[c]->nnz() == 0) {
	infeas = sub_->constraint(c)->voidLhsViolated(rows[c]->rhs());
	if (infeas != ABA_INFEASCON::Feasible)
	  infeasCons_.push(new ABA_INFEASCON(master_, sub_->constraint(c), infeas));
      }
      rows[c]->rename(orig2lp_);
     
    }

    // initialize the LP-solver and clean up
    ABA_LP::initialize(*master_->optSense(), nRow, sub_->maxCon(), nCol,
                     sub_->maxVar(), obj, lBound, uBound, rows,
                     lpVarStat, slackStat);

    for (c = 0; c < nCon; c++)
      delete rows[c];

  }

  void ABA_LPSUB::constraint2row(ABA_BUFFER<ABA_CONSTRAINT*> &cons,
                                 ABA_BUFFER<ABA_ROW*> &rows)
  {                              
    int   conNnz;                      //!< number of nonzero elements in constraint
    ABA_ROW   rowBuf(master_, sub_->nVar());  //!< dummy to generate row
    ABA_ROW  *row;                         //!< pointer to the new row

    const int nCons = cons.number();
    
    for (int c = 0; c < nCons; c++) {
      conNnz = cons[c]->genRow(sub_->actVar(), rowBuf);
      row = new ABA_ROW(master_, conNnz);
      row->copy(rowBuf);
      rows.push(row);
      rowBuf.clear();
    }
  }

  bool ABA_LPSUB::eliminable(int i) const
  {
    if (master_->eliminateFixedSet()) {
      return !sub_->lpVarStat(i)->basic();
    }
    else
      return false;
  }

  double ABA_LPSUB::elimVal(int i) const
  {
    switch (sub_->fsVarStat(i)->status()) {
      case ABA_FSVARSTAT::SetToLowerBound:   return sub_->lBound(i);
      case ABA_FSVARSTAT::FixedToLowerBound: return sub_->variable(i)->lBound();
      case ABA_FSVARSTAT::SetToUpperBound:   return sub_->uBound(i);
      case ABA_FSVARSTAT::FixedToUpperBound: return sub_->variable(i)->uBound();
      case ABA_FSVARSTAT::Set:               return sub_->fsVarStat(i)->value();
      case ABA_FSVARSTAT::Fixed:             return sub_->variable(i)->fsVarStat()->value();
      
      default: const char *_error="ABA_LPSUB::elimVal():\nvariable neither fixed nor set";
               THROW_PARAM(AlgorithmFailureException,afcLpSub,_error);
               return 0.0;   //!< to suppress compiler warning
    }
  }

   double ABA_LPSUB::elimVal(ABA_FSVARSTAT *stat, double lb, double ub) const
   {
     switch (stat->status()) {
       case ABA_FSVARSTAT::SetToLowerBound:   return lb;
       case ABA_FSVARSTAT::FixedToLowerBound: return lb;
       case ABA_FSVARSTAT::SetToUpperBound:   return ub;
       case ABA_FSVARSTAT::FixedToUpperBound: return ub;
       case ABA_FSVARSTAT::Set:               return stat->value();
       case ABA_FSVARSTAT::Fixed:             return stat->value();
       
       default: const char *_error="ABA_LPSUB::elimVal(): variable neither fixed nor set";
		THROW_PARAM(AlgorithmFailureException,afcLpSub,_error);
               return 0.0;   //!< to suppress compiler warning
     }
   }

  ABA_LP::OPTSTAT ABA_LPSUB::optimize(METHOD method)
  {
    OPTSTAT status;
    
    if (infeasCons_.number()) {
	      const char *_error="ABA_LPSUB::optimize(): there are infeasible constraints";
	      THROW_PARAM(AlgorithmFailureException,afcLpSub,_error);
              return Error;  //!< to suppress compiler warnings
    }
    else {
      status = ABA_LP::optimize(method);
      if (status == Infeasible && method != Dual) return optimize(Dual);
      else                                        return status;
    }
  }

  void ABA_LPSUB::removeCons(ABA_BUFFER<int> &ind)
  {
    ABA_LP::remRows(ind);
  }

  void ABA_LPSUB::removeVars(ABA_BUFFER<int> &vars)
  {
    // ABA_LPSUB::removeVars(): local variables
    ABA_BUFFER<int>       lpVars(master_, vars.number());   //!< indices in \a LP of removed variables
    ABA_ARRAY<double>     rhsDelta(master_, sub_->nCon(), 0.0);  //!< changes of right hand side
    int               lpName;                        //!< name of variable in the \a LP
    double            coeff;
    ABA_VARIABLE         *v;
    bool              modifyRhs = false;
    double            eps = master_->eps();

    // ABA_LPSUB::removeVars(): update the number of original variables
    int oldNOrigVar = nOrigVar_;

    nOrigVar_ -= vars.number();

    // divide removed variables in eliminated and non-eliminated ones
    /* If a removed variable has earlier been eliminated from the LP, then
     *   we might have to adapt the right hand side again, if earlier
     *   the elimination changed the right and side. Otherwise,
     *   we add the variable to the buffer \a lpVars in order to remove
     *   it explicitly later.
     */  
    const int nVars = vars.number();

    for (int i = 0; i < nVars; i++) {
      lpName = orig2lp_[vars[i]];
      if (lpName == -1) {
	//! remove eliminated variable
	valueAdd_  += sub_->variable(i)->obj() * elimVal(i);

	const int nCon = sub_->nCon();

	v = sub_->variable(i);
	for (int c = 0; c < nCon; c++) {
	  coeff = sub_->constraint(c)->coeff(v);
	  if (fabs(coeff) > eps) {
	    rhsDelta[c] += coeff * elimVal(i);
	    modifyRhs    = true;
	  }
	}
      }
      else lpVars.push(lpName);
    }

    // adapt the right hand side if eliminated variables are removed
    if (modifyRhs) {
      ABA_ARRAY<double> newRhs(master_, sub_->nCon());
      const int nCon = sub_->nCon();

      for (int c = 0; c < nCon; c++)
	newRhs[c] = rhs(c) - rhsDelta[c];
      ABA_LP::changeRhs(newRhs);
    }

    // remove the non-eliminated variables
    /* Here, we also should check for constraints getting a void left hand side
     *   and becoming infeasible. However, on the one hand this is computationally
     *   expensive (using the member function \a row())
     *   as most LP-solvers (as, e.g., Cplex) work in a column oriented
     *   form, and second, if immediately afterwards variables are added then
     *   the linear program could become again feasible.
     *
     *   Moreover, if only inequalities with void left hand side become infeasible,
     *   then these infeasibilities are recognized by the LP-solver and resolved
     *   in \a makeFeas(). Only equations can cause some trouble as there is no
     *   slack variable.
     *
     *   Therefore,  unfortunately, taking care that no equation becomes infeasible has to be
     *   left to the user.
     */  
    ABA_LP::remCols(lpVars);

    // update mappings of original variables and LP variables
    // sort the variables being removed
    // check if sorting is required
    bool unordered = false;

      for (int i = 0; i < nVars - 1; i++)
	if (vars[i] > vars[i+1]) {
	  unordered = true;
	  break;
	}

    // if yes, sort the variables
    ABA_BUFFER<int> varsSorted(master_, oldNOrigVar);

    if (unordered) {
    ABA_ARRAY<bool> marked(master_, oldNOrigVar, false);

      for (int i = 0; i < nVars; i++)
	marked[vars[i]] = true;

      for (int i = 0; i < oldNOrigVar; i++)
	if (marked[i]) 
        varsSorted.push(i);
    }
    else 
      for (int i = 0; i < nVars; i++)
	varsSorted.push(vars[i]);
   
// update mapping of original variables to LP variables
/* In order to update the mapping of the original variables to the LP-variables 
 *  we have to eliminate the removed variables from the array \a orig2lp_ by a
 *   leftshift. Moreover, if the variable \a i is not removed then we have to
 *   reduce \a orig2lp_ by the number of variables that have been removed with a 
 *   index than \a i that have not been eliminated.
*/
  int current = varsSorted[0];
  int nNotEliminatedRemoved = 0;

  for (int i = 0; i < nVars - 1; i++) {
    if (orig2lp_[varsSorted[i]] != -1)
      nNotEliminatedRemoved++;

    const int last = varsSorted[i+1];
    for(int j = varsSorted[i]+1; j < last; j++)
      if (orig2lp_[j] == -1)
        orig2lp_[current++] = -1;
      else
        orig2lp_[current++] = orig2lp_[j] - nNotEliminatedRemoved;
  }

  if (orig2lp_[varsSorted[nVars-1]] != -1)
      nNotEliminatedRemoved++;

  for (int j = varsSorted[nVars - 1] + 1; j < oldNOrigVar; j++)
    if (orig2lp_[j] == -1)
      orig2lp_[current++] = -1;
    else
      orig2lp_[current++] = orig2lp_[j] - nNotEliminatedRemoved;
  
  
  // update mapping of LP variables to original variables
  /* Since \a orig2lp_ is updated already we can update the reverse
   *   mapping \a lp2orig_ in a straight forward way by scanning \a orig2lp_.
   */ 
  int nVarLp = 0;

    for (int i = 0; i < nOrigVar_; i++)
      if (orig2lp_[i] != -1) 
	lp2orig_[nVarLp++] = i;

  }

  void ABA_LPSUB::addCons(ABA_BUFFER<ABA_CONSTRAINT*> &newCons)
  {
    
    // ABA_LPSUB::addCons(): local variables
    ABA_BUFFER<ABA_ROW*> newRows(master_, newCons.number());  //!< the new constraints in row format
    ABA_BUFFER<int> delVar(master_, sub_->nVar()); //!< buffer of deletable components of row format
    double            rhsDelta;  //!< correction of right hand side
    ABA_INFEASCON::INFEAS infeas;    //!< infeasibility mode (TooLarge, TooSmall)

    ABA_ROW*         nr;

    constraint2row(newCons, newRows);

    // eliminate variables in added constraints
    /* Also the elimination of variables in an added constraint might
     *   cause a void left hand side (interpreted as 0) violated the right hand
     *   side of the constraint. These infeasible constraints are recognized,
     *   but the resolution is currently not implemented.
     */
    const int nNewRows = newRows.number();

    for (int c = 0; c < nNewRows; c++) {
      //! eliminate variables in constraint \a c
      delVar.clear();
      rhsDelta = 0.0;
      nr       = newRows[c];
      const int nrNnz = nr->nnz();
      for(int i = 0; i < nrNnz; i++)
	if(eliminated(nr->support(i))) {
	  delVar.push(i);
	  rhsDelta += nr->coeff(i)*elimVal(nr->support(i));
	}
      nr->delInd(delVar,rhsDelta);
      nr->rename(orig2lp_);

      // check if constraint \a c has become infeasible
      if(nr->nnz() == 0) {
	infeas = newCons[c]->voidLhsViolated(nr->rhs());
	if (infeas != ABA_INFEASCON::Feasible) {
	  infeasCons_.push(new ABA_INFEASCON(master_, newCons[c], infeas));
	  const char *_error="ABA_LPSUB::addCons(): infeasible constraint added.\nAll variables with nonzero coefficients are\neliminated and constraint is violated.Sorry, resolution not implemented yet.";
	  THROW_PARAM(AlgorithmFailureException,afcLpSub,_error);
	}
      }

    }

    ABA_LP::addRows(newRows);

    for (int i = 0; i < newRows.number(); i++)
      delete newRows[i];
  }

  void ABA_LPSUB::addVars(ABA_BUFFER<ABA_VARIABLE*> &vars, 
                          ABA_BUFFER<ABA_FSVARSTAT*> &fsVarStat,
                          ABA_BUFFER<double> &lb, 
                          ABA_BUFFER<double> &ub)
  {
    // ABA_LPSUB::addVars(): local variables
    ABA_BUFFER<int> delVar(master_, vars.number());  //!< the eliminated variables
    ABA_ARRAY<double> rhsDelta(master_, sub_->nCon(), 0.0);  //!< the correction of the rhs
    double vValue;
    double coeff;
    bool modifyRhs = false;  //!< if \a true the modification of rhs required
    int oldNCol = trueNCol();
    int n = trueNCol();
    ABA_VARIABLE *v;

    // divide the added variables in eliminable and non-eliminable ones
    int nVariables = vars.number();

    for (int i = 0; i < nVariables; i++) {
      v = vars[i];
      if(fsVarStat[i]->fixedOrSet()) {
	if (eliminable(i)) {
	  //! the new variable is eliminated
	  delVar.push(i);

	  vValue = elimVal(fsVarStat[i], lb[i], ub[i]);

	  valueAdd_ += v->obj() * vValue;
	  orig2lp_[nOrigVar_++] = -1;

	  const int nCon = sub_->nCon();

	  for (int c = 0; c < nCon; c++) {
	    coeff = sub_->constraint(c)->coeff(v);
	    if (fabs(coeff) > master_->eps()) {
	      rhsDelta[c] += vValue * coeff;
	      modifyRhs    = true;
	    }
	  }

	}
	else {
	  // the new variable is fixed in the LP
	  orig2lp_[nOrigVar_++] = n;
	  lp2orig_[n] = oldNCol + i;
	  ++n;
	  lb[i] = ub[i] = elimVal(fsVarStat[i], lb[i], ub[i]);

	}
      }
      else {
	// the new variable will be added to the LP explicitly
	orig2lp_[nOrigVar_++] = n;
	lp2orig_[n] = oldNCol + i;
	++n;

      }
    }

    // remove the fixed and set added variables
    if (delVar.number()) {
      vars.leftShift(delVar);
      fsVarStat.leftShift(delVar);
      lb.leftShift(delVar);
      ub.leftShift(delVar);
    }

    // generate the column of the added variable and add them to the LP
    ABA_BUFFER<ABA_COLUMN*> newCols(master_, vars.number());  
                                    //!< new columns added to the constraint matrix
    ABA_COLUMN colBuf(master_, nRow());  //!< buffer for generated columns
    ABA_COLUMN *col;

    nVariables = vars.number();
      for(int i = 0; i < nVariables; i++) {
	vars[i]->genColumn(sub_->actCon(), colBuf);
	col = new ABA_COLUMN(master_, colBuf.nnz());
	col->copy(colBuf);
	col->obj(colBuf.obj());
	col->uBound(colBuf.uBound());
	col->lBound(colBuf.lBound());
	newCols.push(col);
	colBuf.clear();
      }

      ABA_LP::addCols(newCols);

      // modify the right hand side if fixed or set variables are added
      if (modifyRhs) {
	const int nCon = sub_->nCon();

	ABA_ARRAY<double> newRhs(master_, nCon);
	for(int c = 0; c < nCon; c++)
	  newRhs[c] = rhs(c) - rhsDelta[c];

	changeRhs(newRhs);
      }

      // ABA_LPSUB::addVars(): clean up
  for(int i = 0; i < nVariables; i++) delete newCols[i];

    }

   void ABA_LPSUB::changeLBound(int i, double newLb)
   {
    int lpVar = orig2lp_[i];

    if (lpVar == -1) {
        char *_error=new char[255];
	sprintf(_error,"ABA_LPSUB::changeLBound( %i , %g )\nvariable %i is eliminated,\ncannot change bounds!",i,newLb,i);
        THROW_PARAM(AlgorithmFailureException,afcLpSub,_error);
     }
     else ABA_LP::changeLBound(lpVar, newLb);
   }

   void ABA_LPSUB::changeUBound(int i, double newUb)
   {
     int lpVar = orig2lp_[i];

     if (lpVar == -1) {
       char *_error=new char[255];
       sprintf(_error,"ABA_LPSUB::changeUBound(%i,%g):\nvariable %i is eliminated,\ncannot change bounds!",i,newUb,i);
       THROW_PARAM(AlgorithmFailureException,afcLpSub,_error);
     }
     else ABA_LP::changeUBound(lpVar, newUb);
   }

  double ABA_LPSUB::lBound(int i) const
  {
    int lpVar = orig2lp_[i];

    if (lpVar != -1) return ABA_LP::lBound(lpVar);
    else             return elimVal(i);
  }

  double ABA_LPSUB::uBound(int i) const
  {
    int lpVar = orig2lp_[i];

    if (lpVar != -1) return ABA_LP::uBound(lpVar);
    else             return elimVal(i);
  }

  double ABA_LPSUB::xVal(int i)
  {
    int lpVar = orig2lp_[i];

    if (lpVar != -1) return ABA_LP::xVal(lpVar);
    else             return elimVal(i);
  }

  double ABA_LPSUB::barXVal(int i)
  {
    int lpVar = orig2lp_[i];

    if (lpVar != -1) return ABA_LP::barXVal(lpVar);
    else             return elimVal(i);
  }

  double ABA_LPSUB::reco(int i)
  {
    int lpVar = orig2lp_[i];

    if (lpVar != -1) return ABA_LP::reco(lpVar);
    else             return 0.0;
  }

  ABA_LPVARSTAT::STATUS ABA_LPSUB::lpVarStat(int i)
  {
    int lpVar = orig2lp_[i];

    if (lpVar != -1) return ABA_LP::lpVarStat(lpVar);
    else             return ABA_LPVARSTAT::Eliminated;
  }

  int ABA_LPSUB::getInfeas(int &infeasCon, int &infeasVar, double *bInvRow)
  {
    int status = ABA_LP::getInfeas(infeasCon, infeasVar, bInvRow);

    if (status) {
      const char *_error="ABA_LPSUB::getInfeas(): ABA_LP::getInfeas() failed";
      THROW_PARAM(AlgorithmFailureException,afcLpSub,_error);
    }

    if (infeasVar >= 0) infeasVar = lp2orig_[infeasVar];

    return 0;
  }

  bool ABA_LPSUB::infeasible() const
  {
    if (ABA_LP::infeasible() || infeasCons_.number()) return true;
    else                                         return false;
  }

  void ABA_LPSUB::loadBasis(ABA_ARRAY<ABA_LPVARSTAT::STATUS> &lpVarStat,
                             ABA_ARRAY<ABA_SLACKSTAT::STATUS> &slackStat)
  {
    ABA_ARRAY<ABA_LPVARSTAT::STATUS> colStat(master_, trueNCol());
    int n = 0;
    
    const int nVar = sub_->nVar();
    
    for (int i = 0; i < nVar; i++) 
      if (!eliminated(i))
	colStat[n++] = lpVarStat[i];
    
    ABA_LP::loadBasis(colStat, slackStat);
  }

  void ABA_LPSUB::varRealloc(int newSize)
  {
    ABA_LP::colRealloc(newSize);

    orig2lp_.realloc(newSize);
    lp2orig_.realloc(newSize);
  }

  void ABA_LPSUB::conRealloc(int newSize)
  {
    ABA_LP::rowRealloc(newSize);
    infeasCons_.realloc(newSize);
  }

