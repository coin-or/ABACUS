@q 1996 by Stefan Thienel @>

@ First we specifiy the required include files.

@(tspmaster.cc@>=
#include "tspmaster.h"
#include "tspsub.h"
#include "edge.h"
#include "degree.h"
#include <VolVolume.hpp>
#include <OsiVolSolverInterface.hpp>
#ifdef ABACUS_CPLEX
#include <OsiCpxSolverInterface.hpp>
#endif
#include <OsiClpSolverInterface.hpp>
#include <ClpSimplex.hpp>
#include <ClpPrimalColumnSteepest.hpp>
#include <ClpPrimalColumnDantzig.hpp>
#include <ClpDualRowSteepest.hpp>
#include <ClpDualRowDantzig.hpp>


extern "C" {
#include <stdio.h>
#include <string.h>
}

#ifdef ABACUS_MATH_CPP
#include <math.h>
#else
extern "C" {
#include <math.h>
}
#endif

@ The Constructor.
    \BeginArg
    \docarg{|problemName|}{The name of the optimization problem instance.
                           If |problemName| starts with |"./"| then the
                           input file with the same name is searched in the
                           current working directory, if the problem name 
                           starts with |"/"| then the absolute path name 
                           of the problem instances is taken, otherwise the
                           input file is searched in the directory defined by
                           the environment variable |TSPLIB\_DIR|. The input
                           file must be in TSPLIB-format having 
                           edge weight type {\tt EUC\_2D} (two-dimensional
                           Euclidean distance).}                            
    \EndArg
  The constructor calls first the constructor of its base class
  |ABA_MASTER|. The second argument of the constructor of |ABA_MASTER| is |true|
  because we are using cutting plane generation for the solution of the
  subproblems. Since no variables are generated dynamically the third
  argument of the base class constructor is |false|. To indicate that
  the traveling salesman problem
  is a minimization problem we set the sense of the optimization
  to |ABA_OPTSENSE::Min|. If in another application the sense of the  
  optimization is still unknown (e.g., if it is later read from a file), 
  the fourth argument of |ABA_MASTER()| can be omitted. However, it must be
  later initialized by calling |ABA_MASTER::initializeOptSense(s)| where |s|
  is |ABA_OPTSENSE::Min| or |ABA_OPTSENSE::Max|.

  In the body of the constructor the problem data is read and memory
  is allocated.

@(tspmaster.cc@>=
  TSPMASTER::TSPMASTER(const char *problemName) : @/
    ABA_MASTER(problemName, true, false, ABA_OPTSENSE::Min), @/
    nNodes_(0), @/
    xCoor_(0), @/
    yCoor_(0), @/
    nSubTours_(0), @/
    bestSucc_(0), @/
    showBestTour_(false), @/
    clpPrimalColumnPivotAlgorithm_(this, ""), @/
    clpDualRowPivotAlgorithm_(this, ""), @/
    solverOutputLevel_(0), @/
    cplexPrimalPricing_(this, "CPX_PPRIIND_AUTO"), @/
    cplexDualPricing_(this, "CPX_DPRIIND_STEEP"), @/
    vol_ascentFirstCheck_(1000000), @/
    vol_ascentCheckInterval_(100), @/
    vol_printFlag_(0), @/
    vol_printInterval_(5000), @/
    vol_greenTestInterval_(1), @/
    vol_yellowTestInterval_(2), @/
    vol_redTestInterval_(10), @/
    vol_alphaInt_(200), @/
    vol_maxSubGradientIterations_(2000), @/
    vol_lambdaInit_(.1), @/
    vol_alphaInit_(.01), @/
    vol_alphaFactor_(.5), @/
    vol_alphaMin_(.000005), @/
    vol_primalAbsPrecision_(.02), @/
    vol_gapAbsPrecision_(1.), @/
    vol_gapRelPrecision_(.01), @/
    vol_granularity_(.0), @/
    vol_minimumRelAscent_(.0001), @/
    tailOffPercentExact_(.0001), @/
    tailOffPercentVol_(.0001), @/
    tailOffNLpsVol_(-1) @/
  {
    @<read the input data@>;
    @<allocate further memory for class |TSPMASTER|@>;
    @<clean up |TSPMASTER::TSPMASTER()|@>;
  }

@ @<read the input data@>=
  @<check if |problemName| is not 0@>;
  @<determine the complete file name@>;
  readTsplibFile(fileName);

@ @<check if |problemName| is not 0@>=
  if (problemName == 0) {
    err() << "TSPMASTER::TSPMASTER(): problem name is missing." << endl;
    exit(Fatal);
  }

@ If |problemName| does not start with |"./"| or |"/"| we have to determine
  the location of the TSPLIB.

@<determine the complete file name@>=
  char *fileName;

  if (problemName[0] == '/' ||
      strlen(problemName) > 1 &&
      problemName[0] == '.' && problemName[1] == '/') {
    fileName = new char[strlen(problemName) + 1];
    sprintf(fileName, "%s", problemName);
  }
  else {
    @<find the location of the TSPLIB@>;
    fileName = new char[strlen(tsplib) + strlen(problemName) + 2];
    sprintf(fileName, "%s/%s", tsplib, problemName);
  }

@ The location of the TSPLIB is determined with the help of the
  environment variable |TSPLIB_DIR|.

@<find the location of the TSPLIB@>=
  const char *tsplib = getenv("TSPLIB_DIR");

  if (tsplib == 0) {
    err() << "TSPMASTER::TSPMASTER(): environment variable ";
    err() << "TSPLIB_DIR not found" << endl;
    exit (Fatal);
  }
  
@ @<allocate further memory for class |TSPMASTER|@>=
  bestSucc_ = new int[nNodes_];

@ @<clean up |TSPMASTER::TSPMASTER()|@>=
  delete [] fileName;
    
@ The destructor frees the memory allocated in the constructor.

@(tspmaster.cc@>=
  TSPMASTER::~TSPMASTER()
  {
    delete [] xCoor_;
    delete [] yCoor_;
    delete [] bestSucc_;
  }

@ The function |readTsplibFile()| reads a problem instance in TSPLIB-format
  if the edge weight type is {\tt EUC\_2D}. The arrays |xCoor_| and |yCoor_|
  storing the coordinates of the nodes are allocated in the function.
  Also the number of nodes |nNodes_| is initialized.

  \BeginArg
  \docarg{|fileName|}{The name of the input file.}
  \EndArg

@(tspmaster.cc@>=
  void TSPMASTER::readTsplibFile(const char *fileName)
  {
    @<open the input file@>;
    @<read the problem@>;
    @<close the TSPLIB file@>;
  }

@ @<open the input file@>=
  FILE *tspFile = fopen(fileName, "r");

  if (tspFile == NULL) {  // {\tt NULL} is written |NULL| in CWEB
    err() << "TSPMASTER::TSPMASTER(): ";
    err() << "TSPLIB file " << fileName << " could not be opened." << endl;
    exit(Fatal);
  }

@ @<read the problem@>=
  @<check the problem type and read the dimension@>;
  @<have all required keywords been found in the file?@>;
  @<read the coordinates of the nodes@>;

@ The TSPLIB provides several input formats. For simplification in this
  example we only can read problems having edge weight type |"EUC_2D"|.
  In order to determine the number of nodes of the problem we look for
  a line starting with the string |"DIMENSION :"|. The edge weight type
  of the problem instance is correct if we find a line of the
  form |"EDGE_WEIGHT_TYPE : EUC_2D"| or |"EDGE_WEIGHT_TYPE: EUC_2D"|.

  As soon as we reach a line starting with the string 
  |"NODE_COORD_SECTION"| we can continue with stop analyzing the
  specification part of the TSPLIB-file.

@<check the problem type and read the dimension@>=
  const int maxCharPerLine = 1024;
  char lineBuf[maxCharPerLine + 1];
  bool dimensionFound    = false;
  bool typeFound         = false;
  bool coordSectionFound = false;
  
  while (fgets(lineBuf, maxCharPerLine, tspFile)) {
    if (strncmp(lineBuf, "DIMENSION", strlen("DIMENSION")) == 0) {
      if (sscanf(lineBuf, "DIMENSION : %d", &nNodes_) != 1) {
        err() << "Error when reading dimension of problem." << endl;
        exit(Fatal);
      }
      dimensionFound = true;
    }
    else if (strncmp(lineBuf, "EDGE_WEIGHT_TYPE", strlen("EDGE_WEIGHT_TYPE")) 
             == 0) {
      if (strncmp(lineBuf, "EDGE_WEIGHT_TYPE : EUC_2D", 
                  strlen("EDGE_WEIGHT_TYPE : EUC_2D")) && @/
          strncmp(lineBuf, "EDGE_WEIGHT_TYPE: EUC_2D", 
                  strlen("EDGE_WEIGHT_TYPE: EUC_2D"))) {
        err() << "Invalid EDGE_WEIGHT_TYPE, must be EUC_2D." << endl;
        exit(Fatal);
      }
      typeFound = true;
    }
    else if (strncmp(lineBuf, "NODE_COORD_SECTION", strlen("NODE_COORD_SECTION"))
             == 0) {
      coordSectionFound = true;
      break;
    }
  }

@ Before reading the coordinates of the nodes we check if all required
  keywords in the file have been found.
  
@<have all required keywords been found in the file?@>=
  if (!typeFound) {
    err() << "Keyword EDGE_WEIGHT_TYPE not found in file " << fileName << ".";
    err() << endl;
    exit(Fatal);
  }

  if (!dimensionFound) {
    err() << "Keyword DIMENSION not found in file " << fileName << ".";
    err() << endl;
    exit(Fatal);
  }

  if (!coordSectionFound) {
    err() << "Keyword  NODE_COORD_SECTION not found in file " << fileName;
    err() << "." << endl;
    exit(Fatal);
  }

@ A line of the |"NODE_COORD_SECTION"| consists of a number of the node
  and its |x|- and |y|-coordinate. We drop the node number given in the
  line and number all nodes consecutively from |0| to |nNodes_ - 1|.

@<read the coordinates of the nodes@>=
  xCoor_ = new double[nNodes_];
  yCoor_ = new double[nNodes_];
  
  int nodeNumber;

  for (int i = 0; i < nNodes_; i++)
    if (fscanf(tspFile, "%d %lf %lf", &nodeNumber, xCoor_ + i, yCoor_ + i)
        != 3) {
      err() << "Error while reading coordinates of node " << nodeNumber << "." << endl;
      exit(Fatal);
    }

@ @<close the TSPLIB file@>=
  if (fclose(tspFile)) {
    err() << "TSPMASTER::TSPMASTER(): error in closing file " << fileName << "." << endl;
    exit(Fatal);
  }


@ The function |firstSub()| redefines a pure virtual function of the
  base class |ABA_MASTER|.
  \BeginReturn
  A pointer to the root node of the enumeration tree.
  \EndReturn
  The root of the branch-and-bound tree is initialized with an
  object of the type
  |TSPSUB|. For any other application the function |firstSub()| has
  to be implemented in the same way replacing |TSPSUB| with the name of
  the problem specific subproblem class derived from the class |ABA_SUB|.

@(tspmaster.cc@>=
  ABA_SUB* TSPMASTER::firstSub()
  {
    return new TSPSUB(this);
  }

@ The function |initializeOptimization()| defines a virtual dummy function 
  of the class |ABA_MASTER|. Its main purpose is the initialization of the
  constraint and variable pools. The initialization of these pools must
  also be performed in any other application using \ABACUS.
  This initialization can also be done in the constructor of the
  problem specific class derived from |ABA_MASTER|, but we recommend to
  follow our strategy redefining the function |initializeOptimization()|
  in a similar way.

@(tspmaster.cc@>=
  void TSPMASTER::initializeOptimization()
  {
    @<output a banner@>;
    @<generate the variables@>;
    @<generate the degree constraints@>;
    @<initialize the pools@>;
    @<compute a nearest neighbor tour@>;
  }

@ @<output a banner@>=
  out() << "A Simple TSP-Solver." << endl;
  out() << "Copyright (c) Universitaet zu Koeln" << endl << endl;

  out() << "The intention of this program is to demonstrate various " << endl;
  out() << "features of ABACUS, but NOT the fast solution of" << endl;
  out() << "traveling salesman problems." << endl << endl;
  

@ Each variable in this traveling salesman problem solver is associated 
  with an edge of the
  undirected graph. We create these variables using the class |EDGE|
  that is derived from the class |ABA_VARIABLE|. The objective function
  coefficient of each variable is computed by the function |dist(t,h)|
  giving the distance of node |t| and |h|.

@<generate the variables@>=
  int nEdges = (nNodes_*(nNodes_-1))/2;

  ABA_BUFFER<ABA_VARIABLE *> variables(this, nEdges);
  
  for (int t = 0; t < nNodes_ - 1; t++) 
    for (int h = t + 1; h < nNodes_; h++) 
      variables.push(new EDGE(this, t, h, dist(t,h)));


@ In any solution for the traveling salesman problem
   each node must have exactly two incident 
  edges. Therefore we generate for each node a degree constraint
  having the form $x(\delta(v)) = 2$ with an object of the class |DEGREE|.

@<generate the degree constraints@>=
  ABA_BUFFER<ABA_CONSTRAINT*> degreeConstraints(this, nNodes_);

  for (int i = 0; i < nNodes(); i++)
    degreeConstraints.push(new @[@, DEGREE(this,  i) @]);

@ Any constraint and variable used in the optimization has to be
  stored in a pool. \ABACUS\ distinguishes three different pools:
  the constraint pool, the cut pool, and the variable pool.

  The constraint pool stores the constraints that should be included
  in any LP-relaxation. Therefore, we add the degree constraints
  to this pool. The size of the constraint pool is adapted to the number
  of constraints contained in the buffer |degreeConstraints|.

  We initialize the variable pool with the edges of the graph stored
  in the buffer |variables|. Since no variables are generated
  dynamically we set its size to |nEdges|. If in another application
  variables are generated dynamically then the size of the variable
  pool needs not be initialized with the maximal possible number of
  variables because the variable pool is reallocated automatically if
  necessary. Therefore, only an initial guess of its size should
  be used in this initialization.

  Finally, we specify the size of the cut pool, storing all
  cutting planes generated during the optimization, to 
  |5*nNodes_|. This value is only an estimation according to our
  experience with the traveling salesman problem. 
  Suitably estimations have to be determined
  for every particular optimization problem. Here, we omit
  the last argument of the function |initializePools()|. Therefore
  its default value is taken, i.e., the constraint pool is not increased
  if it is full, but non-active constraints are removed instead.

@<initialize the pools@>=
  initializePools(degreeConstraints, variables, nEdges, 5*nNodes_);


@ In order to show how the primal bound can be initialized we 
  compute a nearest-neighbor tour. The primal bound is 
  set with the function |primalBound()|. The computation of an initial
  primal bound is not required for the correctness of the optimization.

  After setting the primal bound, we initialize the best tour storing
  in the array |bestSucc_|.

@<compute a nearest neighbor tour@>=
  ABA_ARRAY<int> succ(this, nNodes_);

  int length = nearestNeighbor(succ);

  primalBound(length);

#ifdef ABACUS_NO_FOR_SCOPE
  for (i = 0; i < nNodes_; i++)
#else
  for (int i = 0; i < nNodes_; i++)
#endif
    bestSucc_[i] = succ[i];

@ The function |nearestNeighbor()| is a rather simple implementation
  for the determination of a nearest neighbor tour. We start at
  node 0 and insert the nearest neighbor of the previously
  inserted nodes until all nodes are included in the tour. Finally
  we have to close the tour from the last inserted node to node 0.
  \BeginReturn
  The length of the tour.
  \EndReturn

  \BeginArg
  \docarg{|succ|}{Stores the successor of each node in the tour after
                  the execution of the function. The size of this
                  array must be at least the number of nodes of the
                    traveling salesman problem.}
  \EndArg

@(tspmaster.cc@>=
  int TSPMASTER::nearestNeighbor(ABA_ARRAY<int> &succ) 
  {
    
    @<local variables (|TSPMASTER::nearestNeighbor()|)@>;
    @<initialize the partial tour with node 0@>;
    @<collect the other nodes@>;
    @<close the tour and return its length@>;
  }
  
@ @<local variables (|TSPMASTER::nearestNeighbor()|)@>=
  ABA_ARRAY<bool> marked(this, nNodes_, false);  // collected nodes become marked
  int length;   // the length of the tour
  int front;    // the last collected node
  int next;     // the next collected node
  int minDist;  // the distance between |front| and |next|
  
@ @<initialize the partial tour with node 0@>=
  marked[0] = true;
  front = 0;
  length = 0;

@ @<collect the other nodes@>=
  for (int i = 0; i < nNodes_ - 1; i++) {
    @<find the node |next| having minimal distance to |front|@>;
    @<add |next| to the partial tour@>;
  }

@ @<find the node |next| having minimal distance to |front|@>=
  minDist = INT_MAX;
  for (int j = 0; j < nNodes_; j++) 
    if (!marked[j] && (dist(front, j) < minDist)) {
      next = j;
      minDist = dist(front, j);
    }

@ @<add |next| to the partial tour@>=
  marked[next] = true;
  length += minDist;
  succ[front] = next;
  front = next;

@ @<close the tour and return its length@>=
  succ[front] = 0;
  length += dist(front, 0);
  return length;

@ The function |dist()|.
  \BeginReturn
  The two-dimensional Euclidean distance between node |t| and node |h|
  rounded to the nearest integer (TSPLIB specifies integer distances for
  all problem).
  \EndReturn

  \BeginArg
  \docarg{|t|}{The first end node of an edge.}
  \docarg{|h|}{The second end node of an edge.}
  \EndArg

@(tspmaster.cc@>=
  int TSPMASTER::dist(int t, int h)
  {
    double xd = xCoor_[t] - xCoor_[h];
    double yd = yCoor_[t] - yCoor_[h];

    return (int) floor(sqrt(xd*xd + yd*yd) + 0.5);
  }

@ The function |output()| redefines a virtual dummy function of the
  base class master to output statistics of the run and the best tour. 
  This function is called at the end of the optimization
  after the output of the \ABACUS\ statistics.

@(tspmaster.cc@>=
  void TSPMASTER::output()
  {
    @<output statistics on constraint generation@>;
    @<output best tour@>;
  }

@ @<output statistics on constraint generation@>=
  out() << endl;
  out() << "Statistics on TSP-constraints" << endl << endl;
  out() << "  Subtour Elimination Constraints: " << nSubTours_ << endl;

  out() << endl;

@ The best tour is only output if |showBestTour_| is |true|.
  This flag can be controlled by the parameter |ShowBestTour|
  in the configuration file {\tt .tsp}.

@<output best tour@>=
  if (showBestTour_) {
    out() << "Best tour: 0";
    
    int v = bestSucc_[0];
    while (v != 0) {
      out() << ' ' << v;
      v = bestSucc_[v];
    }
    out() << endl;
  }

@ The function |newSubTours| increments the counter for the generated
  subtour elimination constraints.
  \BeginArg
  \docarg{|n|}{The number of new generated subtour elimination constraints.}
  \EndArg

@(tspmaster.cc@>=
  void TSPMASTER::newSubTours(int n)
  {
    nSubTours_ += n;
  }


@ The function |updateBestTour()| replaces the tour stored in |bestSucc_|
  by extracting it from an incidence vector.
  \BeginArg
  \docarg{|xVal|}{An array storing an incidence vector of a tour.
                  The length of this array must be the number of edges
                  of the complete graph.}
  \EndArg

@(tspmaster.cc@>=
  void TSPMASTER::updateBestTour(double *xVal)
  {
    @<local variables (|TSPMASTER::updateBestTour()|)@>;
    @<find the two neighbors of each node@>;
    @<assign the successor of each node@>;
  }
      
@ The arrays |neigh1| and |neigh2| store for each node the first and the
  second neighbor, respectively. A node does not have a first or second
  neighbor so far, if the corresponding component of these arrays is
  |-1|.

@ @<local variables (|TSPMASTER::updateBestTour()|)@>=
  ABA_ARRAY<int> neigh1(this, nNodes_, -1);
  ABA_ARRAY<int> neigh2(this, nNodes_, -1);

  double oneMinusEps = 1.0 - machineEps();
  int edge = 0;

@ @<find the two neighbors of each node@>=
  for (int t = 0; t < nNodes_ - 1; t++)
    for (int h = t+1; h < nNodes_; h++) {
      if (xVal[edge] > oneMinusEps) {
        if (neigh1[t]  != -1) neigh2[t] = h;
        else                  neigh1[t] = h;
        if (neigh1[h] != -1)  neigh2[h] = t;
        else                  neigh1[h] = t;
      }
      else if (xVal[edge] > machineEps()) {
        err() << "TSPMASTER::update(): x is not incidence vector of a tour" << endl;
        exit (Fatal);
      }
      edge++;
    }

@ @<assign the successor of each node@>=
  int v, w;
  
  bestSucc_[0] = neigh1[0];
  v = 0;
  
  for (int i = 0; i < nNodes_ - 1; i++) {
    w            = bestSucc_[v];
    if (neigh1[w] == v) bestSucc_[w] = neigh2[w];
    else                bestSucc_[w] = neigh1[w];
    v = w;
  }

@ The function |setSolverParameters()|.
  \BeginReturn
  status
  \EndReturn

@(tspmaster.cc@>=
   bool TSPMASTER::setSolverParameters(OsiSolverInterface* interface, bool solverIsApprox)
   {
   if (!solverIsApprox) {
   switch(defaultLpSolver()) {
   case Clp:
   {
@<set parameters for Clp and return@>;
   }
   case CPLEX:
   {
@<set parameters for CPLEX and return@>;
   }
   default:
   return 0;
   }
   }
   else {
@<set parameters for Vol and return@>;
   }
}
    
@ @<set parameters for Clp and return@>=
   @<get a pointer to the Clp model@>;
   @<set the solver output level in the Clp model@>;
   @<set the primal column pivot algorithm@>;
   @<set the dual row pivot algorithm@>;
   return 0;

@ @<get a pointer to the Clp model@>=
   OsiClpSolverInterface* clpIf = dynamic_cast<OsiClpSolverInterface*>(interface);
   ClpSimplex* clp_simplex = clpIf->getModelPtr();

@ @<set the solver output level in the Clp model@>=
   clpIf->messageHandler()->setLogLevel(solverOutputLevel_);

@ @<set the primal column pivot algorithm@>=
   if (clpPrimalColumnPivotAlgorithm_ == "columnsteepest") {
   ClpPrimalColumnSteepest steepestP;
   clp_simplex->setPrimalColumnPivotAlgorithm(steepestP);
   }
   else if (clpPrimalColumnPivotAlgorithm_ == "columndantzig") {
   ClpPrimalColumnDantzig dantzigP;
   clp_simplex->setPrimalColumnPivotAlgorithm(dantzigP);
   }
   else {
      err() << "Unknown primal column pivot algotithm for Clp: ";
      err() << clpPrimalColumnPivotAlgorithm_ << endl;
      exit(Fatal);
   }

@ @<set the dual row pivot algorithm@>=
   if (clpDualRowPivotAlgorithm_ == "rowdantzig") {
      ClpDualRowDantzig dantzigD;
      clp_simplex->setDualRowPivotAlgorithm(dantzigD);
   }
   else if (clpDualRowPivotAlgorithm_ == "rowsteepest") {
      ClpDualRowSteepest steepestD;
      clp_simplex->setDualRowPivotAlgorithm(steepestD);
   }
   else {
      err() << "Unknown dual row pivot algotithm for Clp: ";
      err() << clpDualRowPivotAlgorithm_ << endl;
      exit(Fatal);
      }

@ @<set parameters for CPLEX and return@>=
#ifdef ABACUS_CPLEX
   @<get a pointer to the CPLEX environmnent@>;
   @<set the solver output level in the CPLEX environment@>;
   @<set the primal pricing strategy@>;
   @<set the dual pricing strategy@>;
#endif
   return 0;

@ @<get a pointer to the CPLEX environmnent@>=
      OsiCpxSolverInterface* cplexIf = dynamic_cast<OsiCpxSolverInterface*>(interface);
      CPXENVptr cpxenv = cplexIf->getEnvironmentPtr();

@ @<set the solver output level in the CPLEX environment@>=
      cplexIf->messageHandler()->setLogLevel(solverOutputLevel_);

@ @<set the primal pricing strategy@>=
      if (cplexPrimalPricing_ == "CPX_PPRIIND_AUTO")
	 ::CPXsetintparam(cpxenv, CPX_PARAM_PPRIIND, CPX_PPRIIND_AUTO);
      else if (cplexPrimalPricing_ == "CPX_PPRIIND_PARTIAL")
	 ::CPXsetintparam(cpxenv, CPX_PARAM_PPRIIND, CPX_PPRIIND_PARTIAL);
      else if (cplexPrimalPricing_ == "CPX_PPRIIND_DEVEX")
	 ::CPXsetintparam(cpxenv, CPX_PARAM_PPRIIND, CPX_PPRIIND_DEVEX);
      else if (cplexPrimalPricing_ == "CPX_PPRIIND_STEEP")
	 ::CPXsetintparam(cpxenv, CPX_PARAM_PPRIIND, CPX_PPRIIND_STEEP);
      else if (cplexPrimalPricing_ == "CPX_PPRIIND_STEEPQSTART")
	 ::CPXsetintparam(cpxenv, CPX_PARAM_PPRIIND, CPX_PPRIIND_STEEPQSTART);
      else if (cplexPrimalPricing_ == "CPX_PPRIIND_FULL")
	 ::CPXsetintparam(cpxenv, CPX_PARAM_PPRIIND, CPX_PPRIIND_FULL);
      else {
	 err() << "Unknown primal pricing strategy for Cplex: ";
	 err() << cplexPrimalPricing_ << "." << endl;
	 exit(Fatal);
      }

@ @<set the dual pricing strategy@>=
      if (cplexDualPricing_ == "CPX_DPRIIND_AUTO")
	 ::CPXsetintparam(cpxenv, CPX_PARAM_DPRIIND, CPX_DPRIIND_AUTO);
      else if (cplexDualPricing_ == "CPX_DPRIIND_STEEP")
	 ::CPXsetintparam(cpxenv, CPX_PARAM_DPRIIND, CPX_DPRIIND_STEEP);
      else if (cplexDualPricing_ == "CPX_DPRIIND_FULLSTEEP")
	 ::CPXsetintparam(cpxenv, CPX_PARAM_DPRIIND, CPX_DPRIIND_FULLSTEEP);
      else if (cplexDualPricing_ == "CPX_DPRIIND_STEEPQSTART")
	 ::CPXsetintparam(cpxenv, CPX_PARAM_DPRIIND, CPX_DPRIIND_STEEPQSTART);
      else if (cplexDualPricing_ == "CPX_DPRIIND_FULL")
	 ::CPXsetintparam(cpxenv, CPX_PARAM_DPRIIND, CPX_DPRIIND_FULL);
      else {
	 err() << "Unknown dual pricing strategy for Cplex: ";
	 err() << cplexDualPricing_ << "." << endl;
	 exit(Fatal);
      }

@ @<set parameters for Vol and return@>=
   @<get a pointer to the Volume model@>;
   @<set the parameters@>;
   return 0;

@ @<get a pointer to the Volume model@>=
      OsiVolSolverInterface* volIf = dynamic_cast<OsiVolSolverInterface*>(interface);
      VOL_parms& vpar = volIf->volprob()->parm;

@ @<set the parameters@>=
      vpar.ascent_first_check = vol_ascentFirstCheck_;
      vpar.printflag = vol_printFlag_;
      vpar.printinvl = vol_printInterval_;
      vpar.greentestinvl = vol_greenTestInterval_;
      vpar.yellowtestinvl = vol_yellowTestInterval_;
      vpar.redtestinvl = vol_redTestInterval_;
      vpar.alphaint = vol_alphaInt_;
      vpar.maxsgriters = vol_maxSubGradientIterations_;
      vpar.lambdainit = vol_lambdaInit_;
      vpar.alphainit = vol_alphaInit_;
      vpar.alphafactor = vol_alphaFactor_;
      vpar.alphamin = vol_alphaMin_;
      vpar.primal_abs_precision = vol_primalAbsPrecision_;
      vpar.gap_abs_precision = vol_gapAbsPrecision_;
      vpar.gap_rel_precision = vol_gapRelPrecision_;
      vpar.granularity = vol_granularity_;
      vpar.minimum_rel_ascent = vol_minimumRelAscent_;

@ The function |nNodes()|.
  \BeginReturn
  The number of nodes of the problem instance.
  \EndReturn

@(tspmaster.cc@>=
  int TSPMASTER::nNodes() const
  {
    return nNodes_;
  }

@ The function |tailOffNLpVol()|.
  \BeginReturn
  The number of LPs considered when checking for tailoff,
  when the Volume Algorithm is used.
  \EndReturn

@(tspmaster.cc@>=
   int TSPMASTER::tailOffNLpVol() const
   {
   return tailOffNLpsVol_;
   }

@ The function |tailOffPercentVol()|.
  \BeginReturn
  The tailoff value when solving with the Volume Algorithm.
  \EndReturn

@(tspmaster.cc@>=
   double TSPMASTER::tailOffPercentVol() const
   {
   return tailOffPercentVol_;
   }

@ The function |tailOffPercentExact()|.
  \BeginReturn
  The tailoff value when solving with the exact solver.
  \EndReturn

@(tspmaster.cc@>=
   double TSPMASTER::tailOffPercentExact() const
   {
   return tailOffPercentExact_;
   }

@ The function |tailOffPercentExact(percent)|.
  \BeginArg
  The new tailoff value for the exact solver.
  \EndArg

@(tspmaster.cc@>=
   void TSPMASTER::tailOffPercentExact(double percent)
   {
   tailOffPercentExact_ = percent;
   }



@ The function |initializeParameters()| redefines a virtual dummy function
  of the class |ABA_MASTER|. This function is called at the beginning
  of the optimization. We show how a parameter for the traveling salesman
  problem can read using the \ABACUS\ parameter reading facilities.

  The function |readParamters()| reads the file {\tt .tsp} that
  must be in the \ABACUS\ parameter file format (see the user's guide).
  All parameters together with their values are added to an internal
  parameter table. From this table parameters can be read with the
  function |getParameter()| that is overloaded for most basic data types.

@(tspmaster.cc@>=    
  void TSPMASTER::initializeParameters()
  {
    readParameters(".tsp");

    int status = getParameter("ShowBestTour", showBestTour_);

    if (status) {
      err() << "Parameter ShowBestTour not in configuration file .tsp." << endl;
      exit(Fatal);
    }

    getParameter("TailOffNLpsVol", tailOffNLpsVol_);
    getParameter("TailOffPercentVol", tailOffPercentVol_);

    getParameter("SolverOutputLevel", solverOutputLevel_);

    getParameter("CplexPrimalPricing", cplexPrimalPricing_);
    getParameter("CplexDualPricing", cplexDualPricing_);

    getParameter("ClpPrimalColumnPivotAlgorithm", clpPrimalColumnPivotAlgorithm_);
    getParameter("ClpDualRowPivotAlgorithm", clpDualRowPivotAlgorithm_);

    getParameter("Vol_ascentFirstCheck", vol_ascentFirstCheck_);
    getParameter("Vol_ascentCheckInterval", vol_ascentCheckInterval_);
    getParameter("Vol_printFlag", vol_printFlag_);
    getParameter("Vol_printInterval", vol_printInterval_);
    getParameter("Vol_greenTestInterval", vol_greenTestInterval_);
    getParameter("Vol_yellowTestInterval", vol_yellowTestInterval_);
    getParameter("Vol_redTestInterval", vol_redTestInterval_);
    getParameter("Vol_alphaInt", vol_alphaInt_);
    getParameter("Vol_maxSubGradientIterations", vol_maxSubGradientIterations_);
    getParameter("Vol_lambdaInit", vol_lambdaInit_);
    getParameter("Vol_alphaInit", vol_alphaInit_);
    getParameter("Vol_alphaFactor", vol_alphaFactor_);
    getParameter("Vol_alphaMin", vol_alphaMin_);
    getParameter("Vol_primalAbsPrecision", vol_primalAbsPrecision_);
    getParameter("Vol_gapAbsPrecision", vol_gapAbsPrecision_);
    getParameter("Vol_gapRelPrecision", vol_gapRelPrecision_);
    getParameter("Vol_granularity", vol_granularity_);
    getParameter("Vol_minimumRelAscent", vol_minimumRelAscent_);
  }
