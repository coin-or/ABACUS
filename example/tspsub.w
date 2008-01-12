@q 1996 by Stefan Thienel @>

@ All member functions are defined in the file {\tt tspsub.w}.

@(tspsub.cc@>=
#include "tspsub.h"
#include "tspmaster.h"
#include "abacus/fastset.h"
#include "edge.h"
#include "subtour.h"
#include "abacus/lpsub.h"

extern "C" {
#include "PadbergRinaldi.h"
}

@ The constructor for the root node of the enumeration tree.
     \BeginArg
     \docarg{|master|}{A pointer to the corresponding master of the optimization.}
     \EndArg
                       
@(tspsub.cc@>=
  TSPSUB::TSPSUB(ABA_MASTER *master) : @/
    ABA_SUB(master, 0, 0, 0), @/
    tailOffForVol_(false) @/
  { }
  
@ The constructor for a son of an existing node.
  \BeginArg
  \docarg{|master|}{A pointer to the corresponding master of the
                          optimization.}
  \docarg{|father|}{A pointer to the father in the enumeration tree.}
  \docarg{|branchRule|}{The rule defining the subspace of the
                        solution space associated with this node.}
  \EndArg

@(tspsub.cc@>=
  TSPSUB::TSPSUB(ABA_MASTER *master, 
                 ABA_SUB *father, 
                 ABA_BRANCHRULE *branchRule) : @/
    ABA_SUB(master, father, branchRule), @/
    tailOffForVol_(false)@/
  { }

@ The destructor.

@(tspsub.cc@>=
  TSPSUB::~TSPSUB()
  { }

@ The LP-solution of a traveling salesman problem is feasible if
  all variables have values zero or one and no subtour elimination
  constraint is violated. Instead of solving the separation problem
  for the subtour elimination constraint, we check first if there
  is no fractional value. In this case the LP-solution is the incidence
  vector of a tour if the graph induced by the edges having value 
  one is connected. Remember, the degree constraints hold for each node.

  We check if the graph induced by the edges with value 1 is connected
  with the help of a disjoint set data structure (\ABACUS\ class
  |ABA\_FASTSET|). Initially each node is the representative of a set
  containing only this node. We scan all edges. If there is a edge
  with value one we check if the two end nodes are contained in
  disjoint sets. If this is the case unit the two sets and continue.
  Otherwise the LP-solution is not the incidence vector of a tour
  except if the edge between the two nodes is the last edge closing the
  tour.

  \BeginReturn
  \docarg{|true|}{If the LP-solution is the incidence vector of a tour,}
  \docarg{|false|}{otherwise.}
  \EndReturn

@(tspsub.cc@>=
  bool TSPSUB::feasible()
  {
    @<local variables (|TSPSUB::feasible()|)@>;
    @<initialize the connected components with the single nodes of the graph@>;
    @<check if LP-value is integer for each edge and no subtour is induced@>;
  }


@ @<local variables (|TSPSUB::feasible()|)@>=
  ABA_FASTSET conComp(master_, tspMaster()->nNodes());  
               // each set represents a connected component
  double x;    // the LP-value of a variable
  int    t;    // the tail node associated with this variable
  int    h;    // the head node associated with this variable
  double eps = master_->machineEps();
  double oneMinusEps = 1.0 - eps;


@ @<initialize the connected components with the single nodes of the graph@>=
  for (int i = 0; i < tspMaster()->nNodes(); i++)
    conComp.makeSet(i);

@ If the member function |unionSets()| of the class |ABA_FASTSET| returns |false|,
  then |t| and |h| are contained already in the same set. Only if
  there are as many edges with value $1.0$
  as nodes in the graph and there is only one
  connected component, then the solution is feasible as the degree constraints
  hold for each node.

  If there are no fractional edges, but there is a subtour, then
  we we find this subtour already before the last edge is added.
  
@<check if LP-value is integer for each edge and no subtour is induced@>=
  int nEdges = 0;  // the number of edges with value 1;

#ifdef ABACUS_NO_FOR_SCOPE
  for (i = 0; i < nVar(); i++) {
#else  
  for (int i = 0; i < nVar(); i++) {
#endif
    x = xVal_[i];
    if (x > oneMinusEps) {
      t = edge(i)->tail();
      h = edge(i)->head();

      if (++nEdges == tspMaster()->nNodes()) {
        @<LP-solution is incidence vector of a tour@>;
      }
      
      if (!conComp.unionSets(t,h))
        return false;
    }
    else if (x >= eps) return false;
  }
  return false;

@ If this tour is shorter than the best known one, then we update
  it in the associated object of the class |TSPMASTER|.
  It is not necessary, but although no error, to update the value primal
  bound. This task is performed by \ABACUS.


@<LP-solution is incidence vector of a tour@>=
  if (master_->betterPrimal(lp_->value()))
    tspMaster()->updateBestTour(xVal_);
  return true;

@ The function |generateSon()| redefines a pure virtual function of the
  base class |ABA_SUB|. While the function |TSPMASTER::firstSub()| initializes
  the root node of the branch and bound tree with a problem specific
  subproblem, this function generates a problem specific son of a subproblem.

  Usually in all applications this function is defined like this one line
  function.

  \BeginReturn
  A pointer to a son of this
  subproblem, which is generated according to the branching
  rule |rule|.
  \EndReturn

  \BeginArg
  \docarg{|rule|}{The branching rule for the generation of the son.}
  \EndArg

@(tspsub.cc@>=
  ABA_SUB *TSPSUB::generateSon(ABA_BRANCHRULE *rule)
  {
    return new TSPSUB(master_ , this, rule);
  }

@ The function |separate()| generates violated subtour elimination
  constraint. Although the number of subtour elimination constraints
  is exponential in the number of nodes of the graph, this separation 
  problem can be solved in polynomial time by determining the minimum
  cut in the support graph, i.e., the graph induced by the variables
  (edges) having non-zero value in the LP-solution. According to the
  definition of a subtour elimination constraint, a subtour elimination
  constraint is violated if and only if the value of the minimum cut in
  the support graph is less or equal than 2. The corresponding subtour
  elimination constraint is given by one of the node sets defining
  a shore of this minimum cut.

  \BeginReturn
  The number of generated inequalities. 
  \EndReturn

@(tspsub.cc@>=
  int TSPSUB::separate()
  {
    @<compute the minimum cut in the support graph@>;
    @<set up the induced subtour elimination constraint@>;
    @<clean up and return number of constraints@>;
  }


@ @<compute the minimum cut in the support graph@>=
  int nCutNodes;  // the number of nodes of one shore of the cut
  int *cutNodes = new int[tspMaster()->nNodes()];  // the nodes of a shore of the cut
  double cutValue; // the value of the minimum cut

  cutValue = minCut(nCutNodes, cutNodes);
  master_->out() << "mincut value = " << cutValue << " on " << nCutNodes << " nodes" << endl;

@ If the value of the minimum cut is less than 2, we generate the constraint.
  A better implementation would check if the cardinality of the complement
  of the nodes defining the minimum cut is smaller. In this case one would
  prefer this equivalent cut induced by the complement.

  The function |addCons()| adds the generated constraint to the default
  cutting plane pool and the buffer of new constraints.
  This constraint is added
  to the current relaxation at the beginning of the next iteration.

@<set up the induced subtour elimination constraint@>=
  int nGen;  // the number of generated constraints

  if (cutValue < 2.0 - master_->eps()) {
    nGen = 1;
    ABA_BUFFER<ABA_CONSTRAINT*> constraint(master_, 1);
    SUBTOUR *subTour = new  SUBTOUR(master_, nCutNodes, cutNodes);

    constraint.push(subTour);

    int nAdded = addCons(constraint);

    if (nAdded != 1) {
      master_->err() << "Addition of constraint failed." << endl;
      exit(Fatal);
    }
    ((TSPMASTER*) master_)->newSubTours(1);
  }
  else nGen = 0;
    
@ @<clean up and return number of constraints@>=
  delete [] cutNodes;
  return nGen;

@ The function |minCut()| computes the minimcum cut in the support graph.

  \BeginReturn
  The value of the minimum cut.
  \EndReturn

  \BeginArg
  \docarg{|nCutNodes|}{Holds the number of nodes stored in the array
                       |cutNodes| after the function call.}
  \docarg{|cutNodes|}{Stores one shore of the minimum cut. This array
                      must have the length at least the number of nodes
                      of the graph minus 1.}
  \EndArg
  
@(tspsub.cc@>=
  double TSPSUB::minCut(int &nCutNodes, int *cutNodes)
  {
    @<initialize the support graph@>;
    @<initialize the node induced subgraph@>;
    @<call the Padberg-Rinaldi algorithm@>;
    @<clean up and return (|TSPSUB::mincut()|)@>;
  }

@ The function we will use for solving the minimum cut problems requires
  that the nodes are numbered beginning with 1 and the first used
  component of an array has the number 1.

@<initialize the support graph@>=
  int *tail = new int[nVar() + 1];  // the tail of the edges
  int *head = new int[nVar() + 1];  // the head of the edges
  double *x = new double[nVar() + 1];  // the weight of the edges

  int nEdges = 0;  // the number of edges of the graph

  for (int i = 0; i < nVar(); i++)
    if (xVal_[i] > master_->machineEps()) {
      ++nEdges;
      tail[nEdges] = edge(i)->tail() + 1;
      head[nEdges] = edge(i)->head() + 1;
      x[nEdges]    = xVal_[i];
    }

@ The function |PadbergRinaldi()|
  can compute the minimum cut of a subgraph induced by a set of nodes.
  Therefore, we store all nodes of the graph in the array |node|. 
  However, a better implementation for the solution of the separation problem
  would first compute the connected components of the
  support graph. Each connected component induces already a subtour elimination
  constraint. Then in addition, one would solve the minimum cut problem
  in each subgraph induced by the nodes of a connected component.
  There are further techniques to accelerate the solution of
  this separation problem (see [PR90]).

@<initialize the node induced subgraph@>=
  int *node = new int[tspMaster()->nNodes() + 1];  // the nodes inducing the graph

#ifdef ABACUS_NO_FOR_SCOPE
  for (i = 1; i <= tspMaster()->nNodes(); i++)
#else
  for (int i = 1; i <= tspMaster()->nNodes(); i++)
#endif
    node[i] = i;


@ For the solution of the minimum cut problem, we use the Padberg-Rinaldi
  algorithm that is part of a package for the solution of minimum
  cut problems [JRT97b] and turned out to be very fast for solving the
  separation problem for the subtour elimination constraints [JRT97a].

  The function |PadbergRinaldi()| stores the nodes defining a shore of
  minimum the cut
  in the components \hbox{1,\dots, |nCutNodes|} of an array and numbers the nodes
  beginning at 1.  Therefore, we require an extra array for calling
  this function and have to transform the node set.
  
@<call the Padberg-Rinaldi algorithm@>=
  double cutValue;
  int *prCutNodes = new int[tspMaster()->nNodes()+1];
  
  PadbergRinaldi(tspMaster()->nNodes(), nEdges, tspMaster()->nNodes(), 1,
                 node, tail, head, x,
                 &nCutNodes, prCutNodes, &cutValue);

#ifdef ABACUS_NO_FOR_SCOPE
  for (i = 0; i < nCutNodes; i++)
#else
  for (int i = 0; i < nCutNodes; i++)
#endif
    cutNodes[i] = prCutNodes[i+1] - 1;

@ @<clean up and return (|TSPSUB::mincut()|)@>=
  delete [] prCutNodes;
  delete [] tail;
  delete [] head;
  delete [] node;
  delete [] x;
  
  return cutValue;
  
@ The function |tspMaster()|.
  \BeginReturn
  A pointer to the corresponding object of the class |TSPMASTER|.
  \EndReturn

@(tspsub.cc@>=
  TSPMASTER *TSPSUB::tspMaster()
  {
    return (TSPMASTER *) master_;
  }
  

@ The function |edge()|.
  \BeginReturn
  A pointer to an object of the class |EDGE| corresponding to the |i|-th
  variable.
  \EndReturn

  \BeginArg
  \docarg{|i|}{The number of a variable.}
  \EndArg

@(tspsub.cc@>=
  EDGE *TSPSUB::edge(int i)
  {
    return (EDGE *) variable(i);
  }
  
  @ The function |solveApproxNow()|.
  \BeginReturn
  True or false depending on whether or not the next LP should be solved
  with the approximate solver.
  \EndReturn
  @(tspsub.cc@>=
  bool TSPSUB::solveApproxNow()
  {
  @<Decide which solver type to use and set the correct tailoff values@>;
  }


  @ In this example the Volume Algorithm is only used in the root node.
    That means if |level()| is greater than one the exact solver is used.
    Even when in the root node usage of the exact solver might be forced by
    the cutting plane algorithm to ensure that branching is done correctly.
    In this case |forceExactSolver()| returns true. 
    When the solver type switches from approximate to exact or vice versa we
    have to reinitialize the tailoff settings with the correct values.
    
@ @<Decide which solver type to use and set the correct tailoff values@>=
  if (level() > 1 || forceExactSolver()) {
  if( tailOffForVol_ ) {
@<reinitialize the tailoff settings with the values for the exact solver@>;
  }
  return false;
  }
  else {
     if( !tailOffForVol_ ) {
@<reinitialize the tailoff settings with the values for the approximate solver@>;
     }
     return true;
  }

  @ If |tailOffForVol_| is true this means that the current tailoff settings
    are for the Volume Algorithm. As the next LP will be solved with the exact 
    solver we have to delete the tailoff object and allocate a new one with the
    correct settings.

@<reinitialize the tailoff settings with the values for the exact solver@>=
  master_->err() << "Setting tailoff for exact solver: ";
  master_->err() << tspMaster()->tailOffNLp() << " LPs , ";
  master_->err() << tspMaster()->tailOffPercentExact() << "%" << endl;
  delete tailOff_;
  tailOff_ = new ABA_TAILOFF(master_);
  master_->tailOffPercent(tspMaster()->tailOffPercentExact());
  tailOffForVol_ = false;

  @ If the next LP should be solved with the Volume Algorithm but the tailoff
    settings are for the exact solver we have to reinitialize them.
    The current tailoff percent value is stored in |tspMaster:\:tailOffPercentExact_|
    so that we can use it later when switching back to the exact solver.
    We then delete the tailoff object and allocate a new one with the right size
    and set the correct tailoff percent value.

@<reinitialize the tailoff settings with the values for the approximate solver@>=
        master_->err() << "Setting tailoff for Volume Algorithm: ";
	master_->err() << tspMaster()->tailOffNLpVol() << " LPs , ";
	master_->err() << tspMaster()->tailOffPercentVol() << "%" << endl;
	tspMaster()->tailOffPercentExact(master_->tailOffPercent());
	delete tailOff_;
	tailOff_ = new ABA_TAILOFF(master_, tspMaster()->tailOffNLpVol());
	master_->tailOffPercent(tspMaster()->tailOffPercentVol());
	tailOffForVol_ = true;
