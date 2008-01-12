@q 1996 by Stefan Thienel @>

@q m i EDGE @>

@ All member functions are defined in the file {\tt edge.cc}.

@(edge.cc@>=
#include "edge.h"
#include "abacus/master.h"

@ The constructor.
    \BeginArg
    \docarg{|master|}{A pointer to the corresponding master of the optimization.}
    \docarg{|tail|}{The |tail| of the edge associated with the variable.}
    \docarg{|head|}{The |head| of the edge associated with the variable.}
    \docarg{|obj|}{The objective function coefficient of the variable.}
    \EndArg
  In the call of the base class constructor we make sure that 
  a variable associated with the edge of a graph is not dynamic, but
  globally valid. It is a binary variable, which has hence lower bound
  $0.0$ and upper bound $1.0$.
  
@(edge.cc@>=
  EDGE::EDGE(ABA_MASTER *master, int tail, int head, double obj) : @/
    ABA_VARIABLE(master, 0, false, false, obj, 0.0, 1.0, ABA_VARTYPE::Binary), @/
    tail_(tail), @/
    head_(head)
  { }

@ The destructor.

@(edge.cc@>=
  EDGE::~EDGE()
  { }

@ The function |tail()|.
  \BeginReturn
    The tail node of the edge.
  \EndReturn

@(edge.cc@>=
  int EDGE::tail () const
  {
    return tail_;
  }

@ The function |head()|.
  \BeginReturn
    The head node of the edge.
  \EndReturn

@(edge.cc@>=
  int EDGE::head () const
  {
    return head_;
  }

