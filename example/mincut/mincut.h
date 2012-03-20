/* 3: */


#ifndef MINCUT_H
#define MINCUT_H
/* 4: */

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#ifdef SYSV
#include <string.h>
#else
#include <strings.h>
#endif
#include <ctype.h>
#include <math.h>
#include <limits.h>

/* :4 */

/* 5: */

/* 6: */

#define Err0(a)       (void) fprintf (stderr, a)
#define Err1(a,b)     (void) fprintf (stderr, a, b)
#define Err2(a,b,c)   (void) fprintf (stderr, a, b, c)
#define Err3(a,b,c,d) (void) fprintf (stderr, a, b, c, d)
#define Out0(a)       (void) fprintf (outfile, a)
#define Out1(a,b)     (void) fprintf (outfile, a, b)
#define Out2(a,b,c)   (void) fprintf (outfile, a, b, c)
#define Out3(a,b,c,d) (void) fprintf (outfile, a, b, c, d)
#define chkmem(p) if (!(p)) { Err0("run out of memory\n"); \
                              exit (not_enough_memory); }
#define Ialloc(p,a)  \
chkmem( p =  (int *) malloc ((size_t) (((a) + 1) * sizeof (int))) )
#define PIalloc(p,a) \
chkmem( p =  (int **) malloc ((size_t) (((a) + 1) * sizeof (int *))) )
#define Ialloc_and_clear(p,a) \
chkmem( p =  (int *) calloc ((size_t) ((a) + 1), (size_t) sizeof (int)) )
#define Walloc(p,a)  \
chkmem( p =  (Weight *) malloc((size_t) (((a) + 1) * sizeof (Weight))) )
#define Free(p) free ((char *) p)

/* :6 */

/* 22: */

#define find_end_vertices(e,t,h,tarc,harc) \
 {                                       \
   harc =  e << 1;                          \
   tarc =  harc - 1;                        \
   h =  tip[harc];                          \
   t =  tip[tarc];                          \
}

/* :22 *//* 23: */

#define find_edge(t,h,e,tarc,harc,cape)                         \
 { int u_fe, v_fe;                                          \
   e =  0;                                                       \
   cape =  zero;                                                 \
   for (u_fe =  first[t]; ((u_fe) && (!e)); u_fe =  next[u_fe]) { \
     v_fe =  tip[u_fe];                                          \
     if (v_fe == h) {                                           \
       harc =  u_fe;                                             \
       cape =  weight[harc];                                     \
       if (u_fe % 2) {                                          \
         tarc =  harc + 1;                                       \
         e =  tarc / 2;                                          \
       }                                                        \
       else {                                                   \
         tarc =  harc - 1;                                       \
         e =  harc / 2;                                          \
       }                                                        \
     }                                                          \
   }                                                            \
}

/* :23 *//* 24: */

#define insert_arc(x,v)    \
 { int u_ia, a_ia =  x; \
   prev[a_ia] =  0;         \
   u_ia =  first[v];        \
   next[a_ia] =  u_ia;      \
   prev[u_ia] =  a_ia;      \
   first[v] =  a_ia;        \
}

/* :24 *//* 25: */

#define insert_edge(t,h,w)             \
 { int h_ie, t_ie;                 \
   ++curnedg;                          \
   h_ie =  curnedg << 1;                \
   t_ie =  h_ie - 1;                    \
   weight[h_ie] =  weight[t_ie] =  w;    \
   tip[h_ie] =  h;                      \
   insert_arc(h_ie,t);                 \
   tip[t_ie] =  t;                      \
   insert_arc(t_ie,h);                 \
}

/* :25 *//* 26: */

#define  remove_arc(x,v)         \
 { int u_ra, w_ra, a_ra =  x; \
   u_ra =  prev[a_ra];            \
   w_ra =  next[a_ra];            \
   if (u_ra)                     \
     next[u_ra] =  w_ra;          \
   else                          \
     first[v] =  w_ra;            \
   prev[w_ra] =  u_ra;            \
}

/* :26 *//* 27: */

#define remove_edge(arc1,arc2) \
 {                           \
   remove_arc(arc1,tip[arc2]); \
   remove_arc(arc2,tip[arc1]); \
   tip[arc1] =  -1;             \
   tip[arc2] =  -1;             \
   curnedg--;                  \
}

/* :27 *//* 28: */

#define change_tip(tarc,harc,s) \
 {                            \
   remove_arc(tarc,tip[harc]);  \
   tip[harc] =  s;               \
   insert_arc(tarc,s);          \
}

/* :28 *//* 29: */

#define for_neighbors(u,v,a)                                      \
 if ((a =  first[u]))                                            \
     for (v =  tip[a]; a; a =  next[a], v =  tip[a])

/* :29 *//* 30: */

#define for_neighbors_save_next(u,v,a,n)          \
 if ((a =  first[u]))                            \
     for (v =  tip[a], n =  next[a];                \
        a; a =  n, n =  next[a], v =  tip[a])

/* :30 *//* 31: */

#define for_star(u,a)                 \
 for (a =  first[u]; a; a =  next[a])

/* :31 *//* 32: */

#define for_nodes(u)              \
 for (u =  1; u <= curnnod; u++)

/* :32 *//* 33: */

#define for_edges(e)              \
 for (e =  1; e <= curnedg; e++)

/* :33 *//* 34: */

#define edge_of(a) (((a)+1) >> 1)

/* :34 *//* 35: */

#define arc_of(e) ((e) << 1)

/* :35 *//* 36: */

#define mate_of(a) ((a) % 2 ? (a) + 1 : (a) - 1)

/* :36 *//* 37: */

#define Graphalloc(nnod,nedg)                        \
 node_size =  nnod;                                 \
   edge_size =  nedg;                                 \
   Ialloc(first,node_size);                          \
   Ialloc(prev,2 * edge_size);                       \
   Ialloc(next,2 * edge_size);  next[0] =  0;       \
   Ialloc(tip,2 * edge_size);  tip[0] =  0;         \
   Ialloc(orig,node_size);                           \
   Walloc(weight,2 * edge_size);                     \
   Walloc(star_w,node_size);

/* :37 */

/* 16: */

#define mark(v)               \
{                           \
    markednode[++nmarks] =  v; \
    marked[v] =  true;         \
}

/* :16 *//* 17: */

#define remove_marks \
 for (; nmarks; nmarks--) marked[markednode[nmarks]]= false

/* :17 */


/* :5 */

/* 7: */


#define  OK                      0
#define  not_enough_memory       1
#define  cannot_open_input_file  2
#define  cannot_open_output_file 3
#define  error_in_input_file     4
#define  graph_is_empty          5
#define  results_inconsistency   6

/* :7 */

/* 8: */

#ifdef INT

typedef int     Weight;
#define infinity  INT_MAX
#define epsilon 0
#define zero 0
#define two 2
#define wformat "%d"

#elif FLOAT

typedef float   Weight;
#define infinity  1.0e30
#define epsilon 1.0e-6
#define zero 0.0
#define two 2.0
#define wformat "%f"

#elif DOUBLE

typedef double  Weight;
#define infinity  1.0e30
#define epsilon 1.0e-8
#define zero 0.0
#define two 2.0
#define wformat "%f"

#else

typedef double  Weight;
#define infinity  1.0e30
#define epsilon 1.0e-8
#define zero 0.0
#define two 2.0
#define wformat "%f"

#endif


/* :8 */

/* 9: */

#define false 0
#define true  1

/* :9 *//* 10: */

#ifndef PRIL
#define PRIL  0
#endif
#define MAX_FNAME_LEN  255
#ifndef DATA_PATH
#define DATA_PATH ""
#endif
#ifndef INPUT_EXT
#define INPUT_EXT ""
#endif
#ifndef OUTPUT_EXT
#define OUTPUT_EXT ".out"
#endif

/* :10 *//* 329: */

#define HASH_MULT  314159
#define HASH_PRIME 516595003

/* :329 */

/* 11: */


#ifdef ANSI

#define ARGUMENT_LIST     \
  int nnod,             \
  int nedg,               \
  int ncno,               \
  int one_to_n,           \
  int *node,              \
  int *tail,              \
  int *head,              \
  Weight *ecap,           \
  int *ncutarray,         \
  int *cutarray,          \
  Weight *mincutvalue

#define ARGUMENT_DECLARATIONS


typedef void    (Mincut_function) (int, int, int, int, int *, int *, int *,
			                  Weight *, int *, int *, Weight *);
#else

#define ARGUMENT_LIST                        \
 nnod, nedg, ncno, one_to_n, node, tail, head, ecap, \
   ncutarray, cutarray, mincutvalue

#define ARGUMENT_DECLARATIONS                 \
 int nnod, nedg, ncno, one_to_n, *node, *tail, *head; \
   Weight  *ecap;                             \
   int *ncutarray,*cutarray;                  \
   Weight *mincutvalue;

typedef void    (Mincut_function) ();

#endif
/* :11 *//* 216: */


#ifdef ANSI
#define MINCOBOUNDARY_ARGUMENTS \
  int     nnod,               \
  int     nedg,                 \
  int     ncno,                 \
  int     scard,                \
  int     tcard,                \
  int     maxnumcuts,           \
  int     gtonly,               \
  int    *node,                 \
  int    *snode,                \
  int    *tnode,                \
  int    *tail,                 \
  int    *head,                 \
  Weight *ecap,                 \
  Weight  target,               \
  int    *numcuts,              \
  Weight *wcap,                 \
  int    *wcard,                \
  int    *wstart,               \
  int    *wnode

#define MINCOBOUNDARY_DECLARATIONS

void 
MinCoboundary(int, int, int, int, int, int, int, int *, int *,
	      int *, int *, int *, Weight *, Weight, int *,
	      Weight *, int *, int *, int *);

#else

#define MINCOBOUNDARY_ARGUMENTS                                       \
 nnod, nedg, ncno, scard, tcard, maxnumcuts, gtonly, node, snode,  \
  tnode, tail, head, ecap, target, numcuts, wcap, wcard, wstart, wnode

#define MINCOBOUNDARY_DECLARATIONS \
  int     nnod, nedg, ncno, scard, tcard, maxnumcuts, gtonly;  \
  int    *node, *snode, *tnode, *tail, *head;                    \
  Weight *ecap, target;                                          \
  int    *numcuts;                                               \
  Weight *wcap;                                                  \
  int    *wcard, *wstart, *wnode;

void            MinCoboundary();

#endif

/* :216 *//* 247: */


#ifdef ANSI

void 
GoldbergTarjan(
	       int source,
	       int sink,
	       int curnnod,
	       int edge_size,
	       int *tip,
	       int *first,
	       int *next,
	       Weight * cap,
	       Weight * mincutvalue,
	       int *ncutarray,
	       int *cutarray
);

#else

void            GoldbergTarjan();

#endif

/* :247 *//* 295: */


#ifdef ANSI

int 
initdistlabels(
	       int source,
	       int sink,
	       int n,
	       int *tip,
	       int *first,
	       int *next,
	       Weight * cap,
	       int *dist,
	       int *ndist
);

void 
initdistlabels_phase2(
		      int source,
		      int sink,
		      int n,
		      int *tip,
		      int *first,
		      int *next,
		      Weight * cap,
		      int *dist
);

#else

int             initdistlabels();

void            initdistlabels_phase2();

#endif

/* :295 *//* 314: */


#ifdef ANSI

void 
reinitdistlabels(
		 int source,
		 int sink,
		 int n,
		 int *tip,
		 int *first,
		 int *next,
		 Weight * cap,
		 int *curarc,
		 int *awake,
		 int wfirst,
		 int *wsucc,
		 int *dist,
		 int *ndist
);
#else

void            reinitdistlabels();

#endif

/* :314 *//* 320: */


#ifdef ANSI
#define STMINCUT_ARGUMENTS \
  int     source,        \
  int     sink,            \
  int     nnod,            \
  int     nedg,            \
  int     ncno,            \
  int     one_to_n,        \
  int    *node,            \
  int    *tail,            \
  int    *head,            \
  Weight *ecap,            \
  int    *ncutarray,       \
  int    *cutarray,        \
  Weight *mincutval

#define STMINCUT_DECLARATIONS

void 
stMinCut(int, int, int, int, int, int, int *, int *, int *, Weight *,
	 int *, int *, Weight *);

#else

#define STMINCUT_ARGUMENTS                                       \
  source, sink, nnod, nedg, ncno, one_to_n, node, tail, head, ecap, \
    ncutarray, cutarray, mincutval

#define STMINCUT_DECLARATIONS                         \
  int     source, sink, nnod, nedg, ncno, one_to_n; \
  int    *node, *tail, *head;                         \
  Weight *ecap;                                       \
  int    *ncutarray, *cutarray;                       \
  Weight *mincutval;

void            stMinCut();

#endif

/* :320 *//* 326: */


#ifdef ANSI

int             cputime();
void            printtime(FILE * f, int t);

#else

int             cputime();
void            printtime();

#endif

/* :326 *//* 328: */


/* 331: */

typedef union {
	char           *C;
	int            *I;
	long           *L;
	float          *F;
	double         *D;
}
                Key;
typedef struct {
	char           *keys;
	long           *head;
	long           *link;
	Key             key;
	long            size;
	long            key_len;
	long            last_index;
	long            index;
}
                Hash;

/* :331 */


#define setup_hash    1
#define insert_key    2
#define lookup_key    3
#define free_hash     4
#ifdef ANSI

void            hash(Hash * H, int op);

#else

void            hash();

#endif

/* :328 *//* 338: */


#define allocate_heap 1
#define init          2
#define enq           3
#define req           4
#define del           5
#define free_heap     6

#ifdef ANSI

typedef int     (Heap_function) (int, Weight *, int);

#else

typedef int     (Heap_function) ();

#endif

/* :338 */

#endif

/* :3 */
