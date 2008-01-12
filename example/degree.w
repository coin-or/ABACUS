@q 1996 by Stefan Thienel @>

@ All member functions are defined in the file {\tt degree.cc}.

@(degree.cc@>=
#include "degree.h"
#include "edge.h"

@ The constructor.
    \BeginArg
    \docarg{|master|}{A pointer to the corresponding master of the optimization.}
    \docarg{|v|}{The number of the associated node.}
    \EndArg
  In the call of the base class constructor we make sure that the constraint
  is not assiociated with a specific subproblem, is an equation, has
  right hand side |2.0|, will not be removed from the LP-relaxation,
  is globally valid, and that it can be lifted.

@(degree.cc@>=
  DEGREE::DEGREE(ABA_MASTER *master,  int v) : @/
    ABA_CONSTRAINT(master, 0, ABA_CSENSE::Equal, 2.0, false, false, true), @/
    node_(v)
  { }

@ The destructor.

@(degree.cc@>=
  DEGREE::~DEGREE()
  { }
  
@ The function |coeff()|  defines the pure virtual function of the base 
  class |ABA_CONVAR| for the computation of a coefficient of a variable
  in a degree constraint.
  The coefficient of a variable associated with an edge |(t,h)| is 1 if 
  one of the two
  nodes |t| and |h| is the constraint defining node. Otherwise,
  the coefficient of the edge is 0.

  \BeginReturn
   The coefficient of edge $(t,h)$ associated with the variable |*v|.
  \EndReturn

  \BeginArg
  \docarg{|v|}{The pointer to the variable |v| must be of type
               |EDGE*|.}
@(degree.cc@>=
  double DEGREE::coeff(ABA_VARIABLE *v)
  {
    EDGE *edge = (EDGE *) v;

    if (edge->tail() == node_ || edge->head() == node_) return 1.0;
    else                                                return 0.0;
  }
  
