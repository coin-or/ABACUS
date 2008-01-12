


#include "mincut.h"

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
)
#else

void 
reinitdistlabels(
		 source,
		 sink,
		 n,
		 tip,
		 first,
		 next,
		 cap,
		 curarc,
		 awake,
		 wfirst,
		 wsucc,
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
	int            *curarc;
	int            *awake;
	int             wfirst;
	int            *wsucc;
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

		for (v = wfirst; v; v = wsucc[v]) {
			dist[v] = n;
			curarc[v] = first[v];
		}
		memset(ndist, 0, n * sizeof(int));
	}





	{
		int             tail_queue, head_queue;


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
					if ((dist[w] == n) && awake[w] && (cap[mate_of(a)] > epsilon)) {


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



}
