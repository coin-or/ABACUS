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

#include "abacus/master.h"
#include "abacus/lp.h"
#include "abacus/sparvec.h"
#include "abacus/row.h"
#include "abacus/column.h"
#include "abacus/lpvarstat.h"
#include "abacus/slackstat.h"

ABA_LP::ABA_LP(ABA_MASTER*master)

:
master_(master),
optStat_(Unoptimized),
xValStatus_(Missing),
barXValStatus_(Missing),
yValStatus_(Missing),
recoStatus_(Missing),
slackStatus_(Missing),
basisStatus_(Missing),
nOpt_(0),
lpSolverTime_(master)
{}

ABA_LP::~ABA_LP()

{}

void ABA_LP::initialize(ABA_OPTSENSE sense,
int nRow,
int maxRow,
int nCol,
int maxCol,
ABA_ARRAY<double> &obj,
ABA_ARRAY<double> &lBound,
ABA_ARRAY<double> &uBound,
ABA_ARRAY<ABA_ROW*> &rows)

{
_initialize(sense,nRow,maxRow,nCol,maxCol,obj,lBound,uBound,rows);
}

void ABA_LP::initialize(ABA_OPTSENSE sense,
int nRow,
int maxRow,
int nCol,
int maxCol,
ABA_ARRAY<double> &obj,
ABA_ARRAY<double> &lBound,
ABA_ARRAY<double> &uBound,
ABA_ARRAY<ABA_ROW*> &rows,
ABA_ARRAY<ABA_LPVARSTAT::STATUS> &lpVarStat,
ABA_ARRAY<ABA_SLACKSTAT::STATUS> &slackStat)

{
_initialize(sense,nRow,maxRow,nCol,maxCol,obj,lBound,uBound,rows);
ABA_LP::loadBasis(lpVarStat,slackStat);
}

void ABA_LP::loadBasis(ABA_ARRAY<ABA_LPVARSTAT::STATUS> &lpVarStat,
ABA_ARRAY<ABA_SLACKSTAT::STATUS> &slackStat)

{
_loadBasis(lpVarStat,slackStat);
}

ABA_LP::OPTSTAT ABA_LP::optimize(METHOD method)

{
if(nCol()==0){
master_->err()<<"ABA_LP::optimize(): cannot optimize ";
master_->err()<<"(number of columns is 0)"<<endl;
exit(Fatal);
}

++nOpt_;

switch(method){
case Primal:optStat_= _primalSimplex();
break;
case Dual:optStat_= _dualSimplex();
break;
case BarrierAndCrossover:optStat_= _barrier(true);
break;
case BarrierNoCrossover:optStat_= _barrier(false);
break;
case Approximate:optStat_= _approx();
break;
}

return optStat_;
}

void ABA_LP::remRows(ABA_BUFFER<int> &ind)

{

initPostOpt();

_remRows(ind);
}

void ABA_LP::initPostOpt()
{
optStat_= Unoptimized;
xValStatus_= barXValStatus_= recoStatus_= Missing;
slackStatus_= yValStatus_= basisStatus_= Missing;
}

void ABA_LP::addRows(ABA_BUFFER<ABA_ROW*> &newRows)

{

if(nRow()+newRows.number()> maxRow())
rowRealloc(nRow()+newRows.number());

initPostOpt();

_addRows(newRows);

}

void ABA_LP::rowRealloc(int newSize)

{
_rowRealloc(newSize);
}

void ABA_LP::remCols(ABA_BUFFER<int> &cols)

{
initPostOpt();
_remCols(cols);
}

void ABA_LP::addCols(ABA_BUFFER<ABA_COLUMN*> &newCols)

{

if(nCol()+newCols.number()> maxCol())
colRealloc(nCol()+newCols.number());

initPostOpt();

_addCols(newCols);
}

void ABA_LP::colRealloc(int newSize)

{
_colRealloc(newSize);
}

void ABA_LP::changeRhs(ABA_ARRAY<double> &newRhs)

{
initPostOpt();

_changeRhs(newRhs);
}

void ABA_LP::changeLBound(int i,double newLb)

{
#ifdef ABACUSSAFE
colRangeCheck(i);
#endif

initPostOpt();

_changeLBound(i,newLb);
}

void ABA_LP::changeUBound(int i,double newUb)

{
#ifdef ABACUSSAFE
colRangeCheck(i);
#endif

initPostOpt();

_changeUBound(i,newUb);
}

int ABA_LP::pivotSlackVariableIn(ABA_BUFFER<int> &rows)

{
initPostOpt();

return _pivotSlackVariableIn(rows);
}

int ABA_LP::getInfeas(int&infeasRow,int&infeasCol,double*bInvRow)

{
return _getInfeas(infeasRow,infeasCol,bInvRow);
}

void ABA_LP::colsNnz(int nRow,ABA_ARRAY<ABA_ROW*> &rows,ABA_ARRAY<int> &nnz)

{
ABA_ROW*row;
int i,r;
int rowNnz;

nnz.set(0);

for(r= 0;r<nRow;r++){
row= rows[r];
rowNnz= row->nnz();

for(i= 0;i<rowNnz;i++)
nnz[row->support(i)]++;
}
}

void ABA_LP::rows2cols(int nRow,
ABA_ARRAY<ABA_ROW*> &rows,
ABA_ARRAY<ABA_SPARVEC*> &cols)

{
ABA_ROW*row;
int i,r;

for(r= 0;r<nRow;r++){
row= rows[r];

const int rowNnz= row->nnz();

for(i= 0;i<rowNnz;i++)
cols[row->support(i)]->insert(r,row->coeff(i));
}
}

void ABA_LP::rowRangeCheck(int r)const

{
if(r<0||nRow()<=r){
master_->err()<<"ABA_LP::rowRangeCheck("<<r<<"): range of rows ";
master_->err()<<"0 ... "<<nRow()-1<<" violated."<<endl;
exit(Fatal);
}
}

void ABA_LP::colRangeCheck(int i)const

{
if(i<0||nCol()<=i){
master_->err()<<"ABA_LP::colRangeCheck("<<i<<"): range of columns ";
master_->err()<<"0 ... "<<nCol()-1<<" violated."<<endl;
exit(Fatal);
}
}

ostream&operator<<(ostream&out,const ABA_LP&rhs)

{
// ABA_LP: \a operator<<: local variables
char sign;
double c;
const double eps= rhs.master_->machineEps();

// output the objective function
    /* The objective function is written in the form
     *   {\tt min 3.1 x0 + 4 x2}. Variables with coefficient 0 are not output.
     *   We also suppress the output of a \f$+\f$ before the first coefficient
     *   and the output of coefficients with value \f$1.0\f$.
     */ 
out<<rhs.sense()<<' ';

for(int i= 0;i<rhs.nCol();i++){
c= rhs.obj(i);
if(c<-eps||c> eps){
if(c<0.0){
sign= '-';
c= -c;
}
else sign= '+';

if(i> 0||sign=='-')out<<sign<<' ';
if(c<1.0-eps||1.0+eps<c)
out<<c<<' ';
out<<'x'<<i<<' ';
}
if(i&&!(i%10))out<<endl;
}
out<<endl;

out<<"s.t."<<endl;

// output the constraints
/* The constraints of the ABA_LP are output row by row.
 */
ABA_ROW row(rhs.master_,rhs.nCol());

#ifdef ABACUS_NO_FOR_SCOPE
for(i= 0;i<rhs.nRow();i++){
#else
for(int i= 0;i<rhs.nRow();i++){
#endif
rhs.row(i,row);
out<<"("<<i<<"): "<<row<<endl;
}

// output the bounds
      /* The bounds are written in the form {\tt 0 <= x0 <= 2.3}. *//*:55*/

out<<"Bounds"<<endl;
#ifdef ABACUS_NO_FOR_SCOPE
for(i= 0;i<rhs.nCol();i++)
#else
for(int i= 0;i<rhs.nCol();i++)
#endif
out<<rhs.lBound(i)<<" <= x"<<i<<" <= "<<rhs.uBound(i)<<endl;


out<<"End"<<endl;
// output the solution of the linear program
    /* Finally the status of optimization of the ABA_LP is output, together
     *   with the value of the optimal solution if it is available.
     */
switch(rhs.optStat_){
case ABA_LP::Unoptimized:out<<"No solution available";
break;
case ABA_LP::Error:out<<"Optimization failed";
break;
case ABA_LP::Optimal:out<<"Optimum value: "<<rhs.value();
break;
case ABA_LP::Feasible:out<<"Primal feasible value: "<<rhs.value();
break;
case ABA_LP::Infeasible:out<<"Problem primal infeasible";
break;
case ABA_LP::Unbounded:out<<"Problem unbounded";
break;
default:rhs.master_->err()<<"operator<<(ABA_OSTREAM&, const ABA_LP&):";
rhs.master_->err()<<"Unknown ABA_LP::Status: "<<rhs.optStat_<<endl;
exit(ABA_ABACUSROOT::Fatal);
}
out<<endl;



return out;
}

int ABA_LP::writeBasisMatrix(const char*fileName)

{
if(optStat_!=Optimal||slackStatus_==Missing||
basisStatus_==Missing)return 1;

// open the file for writing the basis
ofstream file(fileName);
if(!file)return 0;

// mark the basic variables
  
    // mark the basic structural variables

ABA_ARRAY<bool> basicCol(master_,nCol());
ABA_ARRAY<int> basisIndexCol(master_,nCol());
int nBasic= 0;

for(int i= 0;i<nCol();i++)
if(lpVarStat(i)==ABA_LPVARSTAT::Basic){
basicCol[i]= true;
basisIndexCol[i]= nBasic;
nBasic++;
}
else
basicCol[i]= false;

//! mark the basic slack variables

ABA_ARRAY<int> basisIndexRow(master_,nRow());
#ifdef ABACUS_NO_FOR_SCOPE
for(i= 0;i<nRow();i++)
#else
for(int i= 0;i<nRow();i++)
#endif
if(slackStat(i)==ABA_SLACKSTAT::Basic){
basisIndexRow[i]= nBasic;
nBasic++;
}

// check the number of the basic variables

if(nBasic!=nRow()){
master_->err()<<"number of basic variables "<<nBasic<<" != ";
master_->err()<<"number of rows "<<nRow()<<endl;
exit(Fatal);
}

// write the basis row by row
file<<nRow()<<endl;

ABA_ROW sparseRow(master_,nCol());

#ifdef ABACUS_NO_FOR_SCOPE
for(i= 0;i<nRow();i++){
#else
for(int i= 0;i<nRow();i++){
#endif
row(i,sparseRow);
int nBasicInRow= 0;
const int sparseRowNnz= sparseRow.nnz();

for(int j= 0;j<sparseRowNnz;j++)
if(basicCol[sparseRow.support(j)])nBasicInRow++;
if(slackStat(i)==ABA_SLACKSTAT::Basic)nBasicInRow++;

file<<i<<' '<<nBasicInRow<<' ';
#ifdef ABACUS_NO_FOR_SCOPE
for(j= 0;j<sparseRowNnz;j++)
#else
for(int j= 0;j<sparseRowNnz;j++)
#endif
if(basicCol[sparseRow.support(j)]){
file<<basisIndexCol[sparseRow.support(j)]<<' ';
file<<sparseRow.coeff(j)<<' ';
}
if(slackStat(i)==ABA_SLACKSTAT::Basic)
file<<basisIndexRow[i]<<" 1";
file<<endl;
}


return 0;
}

int ABA_LP::setSimplexIterationLimit(int limit)

{
return _setSimplexIterationLimit(limit);
}

int ABA_LP::getSimplexIterationLimit(int&limit)

{
return _getSimplexIterationLimit(limit);
}
