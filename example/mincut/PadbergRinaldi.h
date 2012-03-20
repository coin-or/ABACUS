#ifndef MINCUT_H
#define MINCUT

typedef double  Weight;

void 
PadbergRinaldi(
	       int nnod,
	       int nedg,
	       int ncno,
	       int one_to_n,
	       int *node,
	       int *tail,
	       int *head,
	       Weight * ecap,
	       int *ncutarray,
	       int *cutarray,
	       Weight * mincutvalue
);

#endif
