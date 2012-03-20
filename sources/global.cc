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

#include "abacus/global.h"

#include <math.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "abacus/exceptions.h"
  ABA_GLOBAL::ABA_GLOBAL(double eps, double machineEps, double infinity) 
    :  
    out_(cout),  
    err_(cerr),  
    eps_(eps),  
    machineEps_(machineEps),  
    infinity_(infinity),  
    paramTable_(this, 100)
  {
    int tabSize = 4;
    
    tab_ = new char[tabSize+1];
    for (int i = 0; i <= tabSize; i++) tab_[i] = ' ';
    tab_[tabSize] = '\0';
  }

  ABA_GLOBAL::~ABA_GLOBAL()
  {
    delete [] tab_;
  }

  ostream &operator<<(ostream &out, const ABA_GLOBAL &rhs)
  {
    out << "zero tolerance:         " << rhs.eps_ << endl;
    out << "machine zero tolerance: " << rhs.machineEps_ << endl;
    out << "infinity:               " << rhs.infinity_ << endl;
    return out;
  }

  ABA_OSTREAM& ABA_GLOBAL::out(int nTab) /*const*/
  {
    for (int i = 0; i < nTab; i++) out_ << tab_;
    return out_;
  }

  ABA_OSTREAM& ABA_GLOBAL::err(int nTab) /*const*/
  {
    for (int i = 0; i < nTab; i++) err_ << tab_;
    return err_;
  }

  bool ABA_GLOBAL::isInteger(double x, double eps) const
  {
     double frac = fracPart(x);

     if ((frac > eps) &&
         (frac < 1.0 - eps))
       return false;    
     else
       return true;
  }

  char ABA_GLOBAL::enter(istream &in) 
  {
    char c;
    char d;

    out() << endl << "ENTER>" << flush;
    
    c = (char) in.get();
    d = c;
    while (d != '\n') d = (char) in.get();

    return c;

  }

  void ABA_GLOBAL::insertParameter(const char *name, const char *value)
  {
    if(!name||!value) {
      const char *_error="ABA_GLOBAL:insertParameter(): both arguments must\nbe non-zero pointers";
      THROW_PARAM(AlgorithmFailureException,afcGlobal,_error);
    }
    ABA_STRING stName(this,name);
    ABA_STRING stValue(this,value);
    paramTable_.overWrite(stName, stValue);
  }

  void ABA_GLOBAL::readParameters(const char *fileName)
  {
    // ABA_GLOBAL::readParameters(): local variables
    /* Setting the maximal number of characters per line to 1024 is not very
     *   nice, but should usually not generate too much trouble. We might remove
     *   this restrictions in a future release. The buffers for the strings
     *   have space for one additional character, the terminating \a '\0'.
     */  
    const int maxCharPerLine = 1025;
    char      line[maxCharPerLine];
    char      nameBuf[maxCharPerLine];
    char      valueBuf[maxCharPerLine];
    ABA_STRING    name(this);
    ABA_STRING    value(this);
    int       status;

    // open the parameter file \a fileName
    /* CHANGED  ifstream paramFile(fileName, ios::in \a  ios::nocreate);*/
    ifstream paramFile(fileName, ios::in);

    if (!paramFile) {
      char *_error=new char [255];
      sprintf(_error,"ABA_GLOBAL::readParameters(): opening file %s failed",fileName);
      THROW_PARAM(AlgorithmFailureException,afcGlobal,_error);
    }
    // read the parameter file line by line
    /* Lines in a parameter file starting with \a '#' are comments and hence they
     *   are skipped. Every other non-void line must contain two strings. The
     *   first one is the name of the parameter, the second one its value.
     *
     *  \par
     *   The function \a sscanf() returns the number of read arguments. Empty lines
     *   for which \a sscanf() returns 0 are ignored.
     */  
    while (!paramFile.eof()) {
      paramFile.getline(line, maxCharPerLine);
      if (line[0] == '#') continue;
      status = sscanf(line, "%s%s", nameBuf, valueBuf);
      if (status == 1) {
	char *_error =new char[255];
	sprintf(_error,"ABA_GLOBAL::readParameters %s value missing for parameter %s",fileName,nameBuf);
	THROW_PARAM(AlgorithmFailureException,afcGlobal,_error);
      }
      else if (status == 2) {
	name = nameBuf;
	value = valueBuf;
	paramTable_.overWrite(name, value);
      }
    }
  }

  void ABA_GLOBAL::assignParameter(int &param, const char *name, 
                                   int minVal, int maxVal)
  {
    if(getParameter(name, param)){
      char *_error=new char[255];
      sprintf(_error,"ABA_GLOBAL::assignParameter(): parameter %s\nnot found in parameter table",name);
      THROW_PARAM(AlgorithmFailureException,afcGlobal,_error);
    }
    if(param<minVal||param>maxVal) {
      char *_error=new char[255];
	sprintf(_error,"ABA_GLOBAL::assignParameter(): parameter %s is out of range.\nvalue: %i\nfeasible range: %i ... %i",name,param,minVal,maxVal); 
      THROW_PARAM(AlgorithmFailureException,afcGlobal,_error);
    }
  }

   void ABA_GLOBAL::assignParameter(unsigned &param, const char *name, 
                                    unsigned minVal, unsigned maxVal)
  {
    if(getParameter(name, param)){
      char *_error=new char[255];
      sprintf(_error,"ABA_GLOBAL::assignParameter(): parameter %s not found in parameter table.",name);
      THROW_PARAM(AlgorithmFailureException,afcGlobal,_error);
    }
    if(param<minVal||param>maxVal) {
	  char *_error=new char[255];
	 sprintf(_error,"ABA_GLOBAL::assignParameter(): parameter %s is out of range.\nvalue: %i\nfeasible range: %i ... %i",
        name,param,minVal,maxVal);
	THROW_PARAM(AlgorithmFailureException,afcGlobal,_error);
    }
  }

  void ABA_GLOBAL::assignParameter(double &param, const char *name, 
                                   double minVal, double maxVal)
  {
    if(getParameter(name, param)){
      char *_error=new char[255];
      sprintf(_error,"ABA_GLOBAL::assignParameter(): parameter %s\nnot found in parameter table",name);
      THROW_PARAM(AlgorithmFailureException,afcGlobal,_error);
    }
    if(param<minVal||param>maxVal) {
	char *_error=new char[255];
	sprintf(_error,"ABA_GLOBAL::assignParameter(): parameter %s is out of range.\nvalue: %g\nfeasible range: %g ... %g",name,param,minVal,maxVal);
      THROW_PARAM(AlgorithmFailureException,afcGlobal,_error);

    }
  }

  void ABA_GLOBAL::assignParameter(bool &param, const char *name)
  {   
    if(getParameter(name, param)){
      char *_error=new char[255];
      sprintf(_error,"ABA_GLOBAL::assignParameter(): parameter %s\nnot found in parameter table",name);
      THROW_PARAM(AlgorithmFailureException,afcGlobal,_error);
    }
  }

  void ABA_GLOBAL::assignParameter(ABA_STRING &param, const char *name, 
                                   unsigned nFeasible, const char *feasible[])
  {
    if(getParameter(name, param)){
      char *_error=new char[255];
      sprintf(_error,"ABA_GLOBAL::assignParameter(): parameter %s\nnot found in parameter table",name);
      THROW_PARAM(AlgorithmFailureException,afcGlobal,_error);
    }

    if(nFeasible){
        unsigned i;
      	for(i=0;i<nFeasible;i++) {
        	ABA_STRING stFeas(this,feasible[i]);
	        if(param==stFeas)
          	break;
        	}
      
	if(i==nFeasible) {
        	char *_error=new char [255];
		char *_string=param.string();
		string feasibleValue;
		for(i=0;i<nFeasible;i++) {
		          if(i>0){
	        	     feasibleValue.append(feasible[i]);
			     feasibleValue.append(",");
			     }
		          }
		char *fValue= new char [feasibleValue.size()+1];
		strcpy (fValue, feasibleValue.c_str());
     	       sprintf(_error,"ABA_GLOBAL::assignParameter(): parameter %s is not feasible!\nvalue of parameter: %s\nfesible Values are: %s",name,_string,fValue);
	       THROW_PARAM(AlgorithmFailureException,afcGlobal,_error);
               }                 
      } 
    }  
  

  void ABA_GLOBAL::assignParameter(char &param, const char *name, 
                                   const char *feasible)
  {
    if(getParameter(name, param)){
      char *_error=new char[255];
      sprintf(_error,"ABA_GLOBAL::assignParameter(): parameter %s\nnot found in parameter table",name);
      THROW_PARAM(AlgorithmFailureException,afcGlobal,_error);
    }
    if(feasible){
      int i,len;
      ABA_STRING stFeasible(this,feasible);
      len=stFeasible.size();
      for(i=0;i<len;i++) 
        if(stFeasible[i]==param)
          break;
      if(feasible&&i==len) {
        char *_error =new char[255];
	sprintf(_error,"ABA_GLOBAL::assignParameter(): parameter %s is not feasible.\nvalue:%c\nfeasible settings:%s",name,param,feasible);
	THROW_PARAM(AlgorithmFailureException,afcGlobal,_error);
      }
    }
  }

  void ABA_GLOBAL::assignParameter(int &param, const char *name, 
                                   int minVal, int maxVal, 
                                   int defVal)
  {
    if(getParameter(name, param)){
      param=defVal;
    }
    if(param<minVal||param>maxVal) {
      char *_error=new char[255];
      sprintf(_error,"ABA_GLOBAL::assignParameter(): parameter %s is out of range\nvalue %i\nfeasible range: %i...%i",name,param,minVal,maxVal);
      THROW_PARAM(AlgorithmFailureException,afcGlobal,_error);
    }
  }

  void ABA_GLOBAL::assignParameter(unsigned &param, const char *name, 
                                   unsigned minVal, unsigned maxVal,
                                   unsigned defVal)
  {
    if(getParameter(name, param)){
      param=defVal;
    }
    if(param<minVal||param>maxVal) {
	char *_error=new char[255];
       sprintf(_error,"ABA_GLOBAL::assignParameter(): parameter %s is out of range.\nvalue: %i\nfeasible range: %i ... %i",
        name,param,minVal,maxVal);
      THROW_PARAM(AlgorithmFailureException,afcGlobal,_error);
    }
  }

  void ABA_GLOBAL::assignParameter(double &param, const char *name, 
                                   double minVal, double maxVal,
                                   double defVal)
  {
    if(getParameter(name, param)){
      param=defVal;
    }
    if(param<minVal||param>maxVal) {
	char *_error=new char [255];
        sprintf(_error,"ABA_GLOBAL::assignParameter(): parameter %s is out of range.\nvalue: %g\nfeasible range: %g...%g",name,param,minVal,maxVal);
      THROW_PARAM(AlgorithmFailureException,afcGlobal,_error);
    }
  }

  void ABA_GLOBAL::assignParameter(bool &param, const char *name, 
                                   bool defVal)
  {   
    if(getParameter(name, param)){
      param=defVal;
    }
  }

  void ABA_GLOBAL::assignParameter(ABA_STRING &param, const char *name, 
                                   unsigned nFeasible, const char *feasible[],
                                   const char *defVal)
  {
    if(getParameter(name, param)){
      param=defVal;
    }
    if(nFeasible){
      unsigned i;
      for(i=0;i<nFeasible;i++) {
        ABA_STRING stFeas(this,feasible[i]);
        if(param==stFeas)
          break;
      }
      if(i==nFeasible) {
	char *_error=new char[255];
	char *_string=param.string();
	string feasibleValue;
	 for(i=0;i<nFeasible;i++) {
                          if(i>0){
                             feasibleValue.append(feasible[i]);
                             feasibleValue.append(",");
                             }
                          }
        char *fValue= new char [feasibleValue.size()+1];
        strcpy (fValue, feasibleValue.c_str());
        sprintf(_error,"ABA_GLOBAL::assignParameter(): parameter %s is not feasible.\nvalue of parameter: %s\nfeasible settings: %s\n",name,_string,fValue); 
        THROW_PARAM(AlgorithmFailureException,afcGlobal,_error);
      }
    }
  }
 
  void ABA_GLOBAL::assignParameter(char &param, const char *name, 
                                   const char* feasible, char defVal)
  {
    if(getParameter(name, param)){
      param=defVal;
    }
    if(feasible){
      int i;
      ABA_STRING stFeasible(this,feasible);
      int len=stFeasible.size();
      for(i=0;i<len;i++) 
      if(stFeasible[i]==param)
        break;
      if(i==len) {
        char *_error=new char[255];
	sprintf(_error,"ABA_GLOBAL::assignParameter(): parameter %s is not feasible.\nvalue: %c\nfeasible settings: %s",name,param,feasible);
	THROW_PARAM(AlgorithmFailureException,afcGlobal,_error);
      }
    }
  }

  int ABA_GLOBAL::findParameter(const char *name,
                                unsigned nFeasible,
                                const int *feasible)
  {
    unsigned i;
    int param;
    assignParameter(param,name,INT_MIN,INT_MAX);
    for(i=0;i<nFeasible;i++) 
      if(feasible[i]==param)
        break;
    if(i==nFeasible) {
      char *_error=new char[255];
      string feasibleValue;
	 for(i=0;i<nFeasible;i++) {
			     char buffer[30];
			     sprintf(buffer,"%i",feasible[i]);
                             feasibleValue.append(buffer);
                             feasibleValue.append(",");
                          }
       char *fValue= new char [feasibleValue.size()+1];
       strcpy (fValue, feasibleValue.c_str());
       sprintf(_error,"ABA_GLOBAL::findParameter(): parameter %s is not feasible.\nvalue of parameter:%i\nfeasible settings :%s",name,param,fValue);
       THROW_PARAM(AlgorithmFailureException,afcGlobal,_error);
    }
    return i;
  }

  int ABA_GLOBAL::findParameter(const char *name,
                                unsigned nFeasible,
                                const char *feasible[])
  {
    unsigned i;
    ABA_STRING param(this);
    assignParameter(param,name);
    for(i=0;i<nFeasible;i++) {
      ABA_STRING stFeas(this,feasible[i]);      
      if(stFeas==param)
        break;
    }
    if(i==nFeasible) {
	char *_error=new char[255];
	 char *_string=param.string();
	string feasibleValue;
	for(i=0;i<nFeasible;i++) {
                          if(i>0){
                             feasibleValue.append(feasible[i]);
                             feasibleValue.append(",");
                             }
                          }
       char *fValue= new char [feasibleValue.size()+1];
       strcpy (fValue, feasibleValue.c_str());
       sprintf(_error,"ABA_GLOBAL::findParameter(): parameter %s is not feasible.\nvalue of parameter: %s\nfeasible settings :%s",name,_string,fValue);
       THROW_PARAM(AlgorithmFailureException,afcGlobal,_error);      
    }
    return i;
  }

  int ABA_GLOBAL::findParameter(const char *name,
                                const char *feasible)
  {

    if(!feasible){
      const char *_error="ABA_GLOBAL::findParameter(const char*, const char*): second argument must be non-zero.";
      THROW_PARAM(AlgorithmFailureException,afcGlobal,_error);
    }
    int i;
    char param;
    assignParameter(param,name);
    int len=strlen(feasible);
    for(i=0;i<len;i++) {
      if(feasible[i]==param)
        break;
    }
    if(i==len) {
      char *_error=new char[255];
      sprintf(_error,"ABA_GLOBAL::findParameter(): parameter %s is not feasible.\nvalue: %c\nfeasible settings: %s",name,param,feasible);
      THROW_PARAM(AlgorithmFailureException,afcGlobal,_error);
    }
    return i;
  }

  int ABA_GLOBAL::getParameter(const char *name, int &parameter)
  {
    ABA_STRING  nameString(this, name);
    
    ABA_STRING *s = paramTable_.find(nameString);
    if  (s == 0)
      return 1;
    else {
      parameter = s->ascii2int();
      return 0;
    }
  }


  int ABA_GLOBAL::getParameter(const char *name, unsigned int &parameter)
  {
    ABA_STRING  nameString(this, name);
    
    ABA_STRING *s = paramTable_.find(nameString);
    if  (s == 0)
      return 1;
    else {
      parameter = s->ascii2unsignedint();
      return 0;
    }
  }
  
  int ABA_GLOBAL::getParameter(const char *name, double &parameter)

  {
    ABA_STRING  nameString(this, name);

    ABA_STRING *s = paramTable_.find(nameString);
    if  (s == 0)
      return 1;
    else {
      parameter = s->ascii2double();
      return 0;
    }
  }
  

  int ABA_GLOBAL::getParameter(const char *name, ABA_STRING &parameter)
  {
    ABA_STRING  nameString(this, name);

    ABA_STRING *s = paramTable_.find(nameString);
    if  (s == 0)
      return 1;
    else {
      parameter = *s;
      return 0;
    }
  }
  

  int ABA_GLOBAL::getParameter(const char *name, bool &parameter)
  {
    ABA_STRING  nameString(this, name);

    ABA_STRING *s = paramTable_.find(nameString);
    if  (s == 0)
      return 1;
    else {
      parameter = s->ascii2bool();
      return 0;
    }
  }


  int ABA_GLOBAL::getParameter(const char *name, char &parameter)
  {
    ABA_STRING  nameString(this, name);

    ABA_STRING *s = paramTable_.find(nameString);
    if  (s == 0)
      return 1;
    else {
      parameter = (s->string())[0];
      return 0;
    }
  }
