@q 1996 by Stefan Thienel @>

@ All member functions are defined in the file {\tt subtour.cc}.

@(subtour.cc@>=

#include "tspmaster.h"
#include "subtour.h"
#include "edge.h"

@ The constructor.
  \BeginArg
  \docarg{|master|}{A pointer to the corresponding master of the optimization.}
  \docarg{|nNodes|}{The number of nodes defining the constraint.}
  \docarg{|nodes|}{An array with the nodes defining the constraints.}
  \EndArg
  When call the base class constructor we make sure that the constraint
  has no associated subproblem, is an $\le$-inequality, has right hand
  side |nNodes-1|, may be removed again from the LP-relaxation, is globally
  valid, and can be lifted.
@(subtour.cc@>=
 SUBTOUR::SUBTOUR(ABA_MASTER *master, int nNodes, int *nodes) : @/
    ABA_CONSTRAINT(master, 0, ABA_CSENSE::Less, nNodes-1, true, false, true), @/
    nodes_(master, nNodes), @/
    marked_(0)
  {
    for (int i = 0; i < nNodes; i++) nodes_[i] = nodes[i];
  }

@ The destructor.

@(subtour.cc@>=
  SUBTOUR::~SUBTOUR()
  {
    if (expanded_) delete [] marked_;
  }

@ The function |coeff()| computes the coefficient of the edge $(t,h)$.
  It redefines the pure virtual function of the base class |ABA_CONSTRAINT|.
  The coefficient of an edge |(t,h)| of a subtour elimination constraint
  is 1 if both nodes |t|
  and |h| belong to the node set |nodes|, otherwise its coefficient
  is 0.

  \BeginReturn
  The coefficient of variable |*v|.
  \EndReturn

  \BeginArg
  \docarg{|v|}{A pointer to a variable that must be of type |EDGE*|.}
  \EndArg
  Using RTTI the cast from |ABA_VARIABLE*| to |EDGE*| could be done more
  safely. However, this language feature is currently not supported by
  the GNU-compiler on all architectures.
  
@(subtour.cc@>=
  double SUBTOUR::coeff(ABA_VARIABLE *v)
  {
    if (expanded_) {
      @<compute coefficient for subtour in expanded format@>;
    }
    else {
      @<compute coefficient for subtour in compressed format@>;
    }
  }

@ The coefficient is 1 if both the tail node and the head node of the
  edge are marked in the expanded format.

@<compute coefficient for subtour in expanded format@>=
  if (marked_[((EDGE *) v)->tail()] && marked_[((EDGE *) v)->head()])
    return 1.0;
  else
    return 0.0;

@ In the compressed format we have to scan the set of nodes defining
   the constraint. We stop the scan as soon as both the tail node
   and the head node of the edge are found. In this case the coefficient
   is one, otherwise we return 0.

@<compute coefficient for subtour in compressed format@>=
  int t = ((EDGE *) v)->tail();
  int h = ((EDGE *) v)->head();

  bool tFound = false;
  bool hFound = false;

  for(int v = 0; v < nodes_.size(); v++) 
    if (nodes_[v] == t) {
      if (hFound) return 1.0;
      tFound = true;
    }
    else if (nodes_[v] == h) {
      if (tFound) return 1.0;
      hFound = true;
    }
  return 0.0;

@ The function |expand()| redefines a virtual function
  of the base class |ABA_CONVAR| in order to compute the expanded format.
  In the expanded format of a subtour elimination constraint
  we store in the array |marked_| of type |bool| for each node if it
  is contained in the set defining the constraint.

  \ABACUS\ makes sure that the function |expand()| is not called from any
  function of its kernel, if the constraint is already expanded. Since
  the function |expand()| is a private member of the class |SUBTOUR|
  and not called from any other function of this class we can be sure that
  repeated expansion cannot cause any memory leaks.

@(subtour.cc@>=
  void SUBTOUR::expand()
  {
    int n = ((TSPMASTER *) master_)->nNodes();

    marked_ = new bool[n];

    for (int v = 0; v < n; v++)
      marked_[v] = false;

    int nNodesSubTour = nodes_.size();
    
#ifdef ABACUS_NO_FOR_SCOPE
    for (v = 0; v < nNodesSubTour; v++)
#else
    for (int v = 0; v < nNodesSubTour; v++)
#endif
      marked_[nodes_[v]] = true;
      
  }

@ The function |compress()| deletes the array storing the expanded format.
  Like for the function |expand()| \ABACUS\ makes sure that the constraint
  is not compressed again if it is already in compressed format.

@(subtour.cc@>=
  void SUBTOUR::compress()
  {
    delete [] marked_;
  }
