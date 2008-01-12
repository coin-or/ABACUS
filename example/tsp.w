@q 1996 by Stefan Thienel @>

\hoffset=-2.6truemm

\input epsf

\font\abacusfont=cmssq8 scaled \magstep1
\font\seriesfont=cmr10 scaled \magstep2

\def\ABACUS{{\abacusfont ABACUS}}

\def\BeginArg{\smallskip\noindent Arguments:}
\def\EndArg{\smallskip\noindent}  

\def\BeginReturn{\smallskip\noindent Return Value:
  \par\hangindent\parindent
}
\def\EndReturn{
  \smallskip\noindent
}  

% macro to document the arguments of a function

\newdimen\oldparindent

\def\docarg#1#2{
  \par
  #1
  \oldparindent=\parindent
  \par\nobreak
  \advance\parindent by \oldparindent
  \hangindent\parindent
  #2
  \parindent=\oldparindent
}


\def\title{TSP 1.1}
\def\topofcontents{\null\vfill
  \centerline{\seriesfont ANGEWANDTE MATHEMATIK UND INFORMATIK}
  \vskip 8pt
  \centerline{\seriesfont UNIVERSIT\"AT ZU K\"OLN}
  \vskip 8pt
  \centerline{Report 96.245}
  \vskip 60pt
  \centerline{\titlefont A Simple TSP-Solver: An ABACUS Tutorial}
  \vskip 15pt
  \centerline{Version 1.2, July 2007}
  \vskip 15pt
  \centerline{Version 1.1, August 1997}
  \vskip 15pt
  \centerline{Stefan Thienel}
  \vfill
}

\def\botofcontents{\vfill
\noindent
Copyright \copyright\ Universit\"at zu K\"oln
\bigskip\noindent
Permission is granted to copy and modify this document and the
corresponding source files for the development of \ABACUS\
applications. However, we ask all users to keep the original files
of this example unmodified to keep it consistent everywhere in the
world. Therefore, modification is only allowed if the modified file
receives a new name.
\bigskip\noindent
This work has been partially supported by 
ESPRIT LTR Project no.\ 20244 (ALCOM-IT) and H.C.M.\ Institutional
Grant no.\ ERBCHBGCT940710 (DONET).
\bigskip\noindent
1991 Mathematics Subject Classification: 68-04, 90c11, 90c27

\noindent
Keywords: Traveling Salesman Problem, Mixed Integer Programming
}

@s bool int
@s ABA_MASTER int
@s ABA_SUB int
@s ABA_CONSTRAINT int
@s ABA_VARIABLE int
@s ABA_ARRAY int
@s ABA_BUFFER int
@s ABA_CONVAR int
@s ABA_FASTSET int
@s ABA_OPTSENSE int
@s ABA_CSENSE int 

@s EDGE int
@s DEGREE int
@s SUBTOUR int
@s TSPSUB int
@s TSPMASTER int
@s STATUS int

@** TSP.

This program implements a branch-and-cut algorithm for the
symmetric traveling salesman (TSP) problem based on the branch-and-cut
framework \ABACUS\ version~3.0. It does neither provide a practically efficient algorithm
for the traveling salesman problem nor it is a state-of-the-art implementation
of branch-and-cut algorithm for this optimization problem. 
This program has nothing in common with the one presented in [Thi95]
except that it is also based on \ABACUS.
The author of this program is
aware that many functions could be implemented better. However, the
ONLY purpose of this program is to show how a branch-and-cut algorithm
can be implemented with \ABACUS. Therefore we sacrifice efficiency for
didactic reasons. Descriptions of branch-and-cut algorithms for the traveling
salesman problem can be found in [PR91] and [JRT94].

On a first sight the reader of this example might get the feeling due to
the length of this document that an application based on \ABACUS\ requires
a significant amount of implementation. While this can be true for
sophisticated implementations of some
applications this example is mainly blown up by its detailed
documentation and the explanation and implementation of some additional
features of \ABACUS. A ``minimal'' branch-and-cut TSP-solver using
\ABACUS\ could be much shorter!

The problem instances that can be solved with this simple TSP-solver must
be in TSPLIB format [Rei91] and the distance between two nodes must be given
by the two dimensional Euclidean distance (edge weight type {\tt EUC\_2D}).

This program is written in the literate programming system CWEB [KL93].
But also readers not familiar with this system should 
be able to read this program. 

@ The difference between version~1.1 and version~1.2 of this program 
is its adaption to \ABACUS~3.0. All new features are explained in the 
user's guide and the reference manual. Important improvements like the 
possibility to switch between exact and approximate solvers arbitrarily
are documented in this program.

@ The difference between version~1.0 and version~1.1 of this program
is only its adaption to \ABACUS~2.0. All new features of \ABACUS\
are explained in the user's guide [Thi97]. The most important changes
in this example are the new names of the \ABACUS\ classes and the 
new way of including \ABACUS\ header files.

All \ABACUS\ classes have now a prefix |ABA_| in order to avoid
name conflicts. For making header files unique, we include them
from a subdirectory |"abacus"|, e.g.:

|#include "abacus/master.h"|

Moreover, we adapted this program to the Visual \CPLUSPLUS/ compiler.
These minor modifactions are included in the preprocessor conditions.
|#ifdef ABACUS_VISUAL_CPP| and |#ifdef ABACUS_NO_FOR_SCOPE|.


@ Given the complete graph $K_n = (V_n, E_n)$
with edge weights $c_e$ for every edge $e \in E_n$, the symmetric
traveling salesman problem is to find a tour with minimum length, 
i.e., with minimum sum of its edge weights. 

For a node set $W$ of a graph we denote by $\delta(W)$ the
set of edges with exactly one endnode in $W$ (if $W = \{v\}$ we 
write~$\delta(v)$). Edge sets induced by a node set $W$ in this way are
called cuts. 
For $W\subseteq V$ we denote by $E(W)$ the set of all edges in $E$
with both endnodes in $W$.
For an edge set $F$ we denote by $x(F)$ the sum of the
variables associated with the edges in~$F$.

By identifying with each edge $e \in E_n$ of the graph a 0-1 variable
$x_e \in \{0,1\}^{E_n}$ we obtain an integer programming formulation 
of the TSP:
$$ 
   \eqalign{ \min\quad     & c^T x \cr
             \hbox{s.t.}\quad &x(\delta (v)) = 2 
                             \hbox{\hskip 5.5pt \qquad for all } v \in V \cr
                             &x(\delta (W)) \geq 2 \hbox{\qquad for all }
                                    \emptyset \neq W \subset V \cr
                             & 0 \leq x \leq 1 \cr
                             & x \hbox{ integer.} \cr
   }
$$
The variables having value 1 in a feasible solution of this integer program
are in one-to-one correspondance with the edges of a tour.

The equations require that each node is incident to exactly two 
edges and are called degree constraints. The inequalities
forbid subtours, and are therefore called subtour elimination constraints.
By subtracting the degree constraint 
$x(\delta(v)) = 2$ for each node $v\in W$ of a subtour elimination constraint
and dividing the inequality by $-2$ we obtain the equivalent format
$x(E(W)) \le |W| - 1$. 

@ The basic idea of \ABACUS\ for the development of a new application
is the derivation of problem specific classes from a small amount of
base classes. For every application a problem specific master and
a problem specific subproblem have to be derived from the base
classes |ABA_MASTER| and |ABA_SUB|. Our master for the traveling-salesman problem
is implemented in the class |TSPMASTER| and the corresponding subproblem in
the class |TSPSUB|.

Problem specific structure of constraints and variables can be exploited
by deriving classes from the base classes |ABA_CONSTRAINT| and |ABA_VARIABLE|.
A variable in our implementation is in one-to-one correspondence with
the edge of a graph. Therefore, we represent this variable by the
class |EDGE|. The degree constraints and the subtour elimination constraints
are implemented in the classes |DEGREE| and |SUBTOUR|, respectively,
which are derived from the abstract base class |ABA_CONSTRAINT|.
The inheritance tree of these classes is presented in Figure~1. The
problem specific classes are surrouned by a bold frame.


\midinsert\vbox{\centerline{
  \epsfxsize=150truemm
  \epsfbox{classes.ps}}
  \bigskip
  \centerline{{\bf Figure 1.}\enspace The TSP inheritance tree.}}
\endinsert


Since the problem specific constraint and variable classes are used
in the class |TSPMASTER| and |TSPSUB|, we first describe the declaration
and definition of these classes. After the implementation of the
classes |TSPMASTER| and |TSPSUB| we present a short main program for 
starting the optimization.

@i edge.wh  
@i edge.w

@i degree.wh    
@i degree.w

@i subtour.wh  
@i subtour.w

@i tspmaster.wh
@i tspmaster.w

@i tspsub.wh
@i tspsub.w

@i tspmain.w

@* REFERENCES.

\def\reference#1{\par\hangindent46pt\noindent\ignorespaces
  \hbox to 46pt{[#1]\hfil}\ignorespaces}

\reference{JRT94} M.\ J\"unger, G.\ Reinelt, and S.\ Thienel (1994),
   Provably good solutions for the traveling salesman problem,
   {\sl Zeitschrift f\"ur Operations Research\/} {\bf 40}, 183--217.


\reference{JRT97a} M.\ J\"unger, G.\ Rinaldi, and S.\ Thienel (1996),
           Practical performance of minimum cut algorithms,
           technical report, Universit\"at zu K\"oln.

\reference{JRT97b} M.\ J\"unger, G.\ Rinaldi, and S.\ Thienel (1996),
                  MINCUT, software package, Universit\"at zu K\"oln, to appear.

\reference{KL93} D.E.\ Knuth and S.\ Levy (1993),
                 The CWEB system of structured documentation,
                 technical report and software package,
                 {\tt ftp://labrea.stanford.edu:/pub/cweb}.
 
\reference{PR90} M.W.\ Padberg and G.\ Rinaldi (1990),
          Facet identification for the symmetric traveling salesman polytope,
         {\sl Mathematical Programming\/}
         {\bf 47}, 219--257.

\reference{PR91} M.W.\ Padberg and G.\ Rinaldi (1991),
  {A branch-and-cut algorithm for the resolution of large-scale symmetric
   traveling salesman problems}, {\sl SIAM Review\/} {\bf 33}, 60--100.

\reference{Rei91} G.\ Reinelt (1991), TSPLIB---A traveling salesman
                  problem library, {\sl ORSA Journal on Computing\/} {\bf 3},
                   {376--384}, 
{\tt http://www.iwr.uni-heidelberg.de/iwr/comopt/soft/TSPLIB95/TSPLIB.html}.

\reference{Thi95} S.\ Thienel (1995), \ABACUS---A Branch-And-CUt System,
                  doctoral thesis, Universit\"at zu K\"oln, 1995.

\reference{Thi97} S.\ Thienel (1997), \ABACUS---A Branch-And-CUt System,
                  Version~2.0, Users's Guide and Reference Manual,
                  Universit\"at zu K\"oln, 1997.

@* INDEX AND SECTION NAMES.
