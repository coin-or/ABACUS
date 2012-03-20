


#include "mincut.h"

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
)
#else

int 
initdistlabels(
	       source,
	       sink,
	       n,
	       tip,
	       first,
	       next,
	       cap,
	       dist,
	       ndist
)
	int             source;
	int             sink;
	int             n;
	int            *tip;
	int            *first;
	int            *next;
	Weight         *cap;
	int            *dist;
	int            *ndist;

#endif

{


	int            *queue;
	int             tail_queue;
	int             head_queue;





	Ialloc(queue, n);





	{
		int             v;

		for (v = 1; v <= n; v++)
			dist[v] = n;
	}





	{


		{
			queue[1] = sink;
			tail_queue = head_queue = 1;
			dist[sink] = 0;
			ndist[0] = 1;
			ndist[n] = n - 1;
		}



		while (

		       tail_queue <= head_queue


			) {
			int             v;


			v = queue[tail_queue++];





			{
				int             w, a;
				int             newdis;

				newdis = dist[v] + 1;
				for_neighbors(v, w, a)
					if ((dist[w] == n) && (cap[a] > epsilon)) {


					{
						dist[w] = newdis;
						ndist[newdis]++;
						if (w != source)
							queue[++head_queue] = w;
						if (--ndist[n] == 0)
							break;
					}



				}
			}



		}
	}





	Free(queue);





	return dist[source] == n ? 1 : 0;



}



#ifdef ANSI

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
)
#else

void 
initdistlabels_phase2(
		      source,
		      sink,
		      n,
		      tip,
		      first,
		      next,
		      cap,
		      dist
)
	int             source;
	int             sink;
	int             n;
	int            *tip;
	int            *first;
	int            *next;
	Weight         *cap;
	int            *dist;

#endif

{


	int            *queue;
	int             tail_queue;
	int             head_queue;





	Ialloc(queue, n);





	{
		int             v;

		for (v = 1; v <= n; v++)
			dist[v] = 2 * n;
	}





	{
		int             v;



		{
			queue[1] = source;
			tail_queue = head_queue = 1;
			dist[source] = n;
		}



		while (

		       tail_queue <= head_queue


			) {


			v = queue[tail_queue++];





			{
				int             w, a;
				int             newdis;

				newdis = dist[v] + 1;
				for (a = first[v]; a; a = next[a])
					if ((dist[tip[a]] == 2 * n) && (cap[mate_of(a)] > epsilon)) {
						w = tip[a];


						{
							dist[w] = newdis;
							if (w != sink)
								queue[++head_queue] = w;
						}


					}
			}



		}
	}





	Free(queue);



}
