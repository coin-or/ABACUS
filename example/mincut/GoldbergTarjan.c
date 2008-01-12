


#include "mincut.h"

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
)
#else

void
GoldbergTarjan(
	       source,
	       sink,
	       curnnod,
	       edge_size,
	       tip,
	       first,
	       next,
	       cap,
	       mincutvalue,
	       ncutarray,
	       cutarray
)
	int             source;
	int             sink;
	int             curnnod;
	int             edge_size;
	int            *tip;
	int            *first;
	int            *next;
	Weight         *cap;
	Weight         *mincutvalue;
	int            *ncutarray;
	int            *cutarray;

#endif
{




	int            *firstact;
	int            *succact;
	int             hd;






	int             wfirst;
	int            *wsucc;
	int            *wpred;
	int            *awake;





	Weight         *rcap;
	Weight         *excess;
	int            *dist;
	int            *ndist;
	int            *currentarc;





	int             bound;
	int             nrelabel;
	int             minrelabel;
#ifdef TTT
	int             cpuphase1 = cputime();
	int             cpuphase2;
#endif









	{
		Ialloc(succact, curnnod);
		Ialloc_and_clear(firstact, 2 * curnnod);
	}



	{
		Ialloc(wsucc, curnnod);
		Ialloc(wpred, curnnod);
		Ialloc(awake, curnnod);
	}



	{
		Walloc(rcap, 2 * edge_size);
		Walloc(excess, curnnod);
		Ialloc(dist, 2 * curnnod);
		Ialloc_and_clear(ndist, 2 * curnnod);
		Ialloc(currentarc, curnnod);
	}





	{
		int             v;

		(void) memcpy(currentarc, first, (curnnod + 1) * sizeof(int));
		(void) memcpy(rcap, cap, (2 * edge_size + 1) * sizeof(Weight));
		hd = 0;
		bound = curnnod - 1;
		nrelabel = 0;
		minrelabel = curnnod;

		wfirst = 0;
		for_nodes(v) if (v != source) {
			wsucc[v] = wfirst;
			wpred[wfirst] = v;
			wfirst = v;
		}
		wpred[wfirst] = 0;
		for_nodes(v) excess[v] = zero;
	}





	{
		int             status;

		status = initdistlabels(source, sink, curnnod, tip, first,
					next, cap, dist, ndist);
		if (status) {


			{
				int             v;

				*mincutvalue = excess[sink];
				*ncutarray = 0;
				for_nodes(v)
					if (dist[v] >= curnnod)
					cutarray[++(*ncutarray)] = v;
			}





			Free(succact);
			Free(firstact);




			Free(wsucc);
			Free(wpred);
			Free(awake);



			Free(rcap);
			Free(excess);
			Free(dist);
			Free(ndist);
			Free(currentarc);



			return;
		}
	}





	{
		int             a, w;
		Weight          delta;

		for_neighbors(source, w, a) {
			if (dist[w] < curnnod) {
				delta = rcap[a];
				if (delta > epsilon) {
					rcap[a] = zero;
					rcap[mate_of(a)] += delta;
					if (excess[w] <= epsilon) {


						{
							int             dw;

							dw = dist[w];
							if ((w != sink) && (w != source)) {
								succact[w] = firstact[dw];
								firstact[dw] = w;
								if (dw > hd)
									hd = dw;
							}
						}



					}
					excess[w] += delta;
					excess[source] -= delta;
				}
			}
		}
	}





	{
		ndist[dist[source]]--;
		dist[source] = curnnod;
	}





	{
		int             v, dv, a;
		while (

		       hd > 0


			) {


			{
				v = firstact[hd];
				firstact[hd] = succact[v];
			}



			while (v) {


				{
					for (a = currentarc[v]; a; a = next[a])
						if ((dist[v] == dist[tip[a]] + 1) && (rcap[a] > epsilon))
							break;
					currentarc[v] = a;
				}



				if (a) {


					{
						int             w;
						Weight          delta;

						if (excess[v] <= rcap[a] + epsilon) {


							{
								delta = excess[v];
								rcap[a] -= delta;
								excess[v] = zero;
								v = 0;
							}



						} else {


							{
								delta = rcap[a];
								rcap[a] = zero;
								excess[v] -= delta;
							}



						}
						rcap[mate_of(a)] += delta;
						w = tip[a];
						if (excess[w] <= epsilon) {


							{
								int             dw;

								dw = dist[w];
								if ((w != sink) && (w != source)) {
									succact[w] = firstact[dw];
									firstact[dw] = w;
									if (dw > hd)
										hd = dw;
								}
							}



						}
						excess[w] += delta;
					}



				} else {


					{
						int             newdis, firsta;

						dv = dist[v];


						{
							int             w;

							newdis = bound;
							for_neighbors(v, w, a)
								if ((rcap[a] > epsilon) && (dist[w] < newdis)) {
								newdis = dist[w];
								firsta = a;
								break;
							}
							for (; a; a = next[a], w = tip[a])
								if ((rcap[a] > epsilon) && (dist[w] < newdis))
									newdis = dist[w];
						}



						ndist[dv]--;

						if (newdis == bound) {


							{
								if (v == wfirst) {
									wfirst = wsucc[v];
									wpred[wfirst] = 0;
								} else {
									wpred[wsucc[v]] = wpred[v];
									wsucc[wpred[v]] = wsucc[v];
								}
								dist[v] = curnnod;
								bound--;
								v = 0;
							}



						} else if (ndist[dv]) {


							{
								newdis++;
								dist[v] = newdis;
								ndist[newdis]++;
								currentarc[v] = firsta;
								hd = newdis;
							}



						}
						if (!ndist[dv]) {


							{
								int             g;

								firstact[dv] = 0;
								for (g = wfirst; g; g = wsucc[g])
									if ((dv <= dist[g]) && (dist[g] < curnnod)) {
										dist[g] = curnnod;


										{
											if (g == wfirst) {
												wfirst = wsucc[wfirst];
												wpred[wfirst] = 0;
											} else {
												wpred[wsucc[g]] = wpred[g];
												wsucc[wpred[g]] = wsucc[g];
											}
										}



										bound--;
									}
								v = 0;
							}



						}
						++nrelabel;
					}



				}
			}
			if (nrelabel >= minrelabel) {


				nrelabel = 0;
				for_nodes(v) awake[v] = (dist[v] < curnnod) ? true : false;

				reinitdistlabels(source, sink, curnnod, tip, first, next, rcap, currentarc,
					 awake, wfirst, wsucc, dist, ndist);


				memset(firstact, 0, curnnod * sizeof(int));
				hd = 0;
				for (v = wfirst; v; v = wsucc[v])
					if (excess[v] > epsilon) {
						dv = dist[v];
						succact[v] = firstact[dv];
						firstact[dv] = v;
						if (dv > hd)
							hd = dv;
					}
			} else {


				while (hd && (firstact[hd] == 0))
					--hd;



			}
		}
	}





	{
		int             v;

		*mincutvalue = excess[sink];
		*ncutarray = 0;
		for_nodes(v)
			if (dist[v] >= curnnod)
			cutarray[++(*ncutarray)] = v;
	}



#ifdef TTT


	Err0("time for phase 1:");
	printtime(stderr, cputime() - cpuphase1);
	Err0("\n");
	cpuphase2 = cputime();
	initdistlabels_phase2(source, sink, curnnod, tip, first, next, rcap, dist);


	for_nodes(v) {
		currentarc[v] = first[v];
		if (excess[v] > epsilon) {
			dv = dist[v];
			if ((v != source) && (v != sink)) {
				succact[v] = firstact[dv];
				firstact[dv] = v;
				if (dv > hd)
					hd = dv;
			}
		}
	}
	bound = 2 * curnnod;





	while (

	       hd > 0


		) {


		{
			v = firstact[hd];
			firstact[hd] = succact[v];
		}



		while (v) {


			{
				for (a = currentarc[v]; a; a = next[a])
					if ((dist[v] == dist[tip[a]] + 1) && (rcap[a] > epsilon))
						break;
				currentarc[v] = a;
			}



			if (a) {


				{
					int             w;
					Weight          delta;

					if (excess[v] <= rcap[a] + epsilon) {


						{
							delta = excess[v];
							rcap[a] -= delta;
							excess[v] = zero;
							v = 0;
						}



					} else {


						{
							delta = rcap[a];
							rcap[a] = zero;
							excess[v] -= delta;
						}



					}
					rcap[mate_of(a)] += delta;
					w = tip[a];
					if (excess[w] <= epsilon) {


						{
							int             dw;

							dw = dist[w];
							if ((w != sink) && (w != source)) {
								succact[w] = firstact[dw];
								firstact[dw] = w;
								if (dw > hd)
									hd = dw;
							}
						}



					}
					excess[w] += delta;
				}



			} else {



				dv = dist[v];


				{
					int             w;

					newdis = bound;
					for_neighbors(v, w, a)
						if ((rcap[a] > epsilon) && (dist[w] < newdis)) {
						newdis = dist[w];
						firsta = a;
						break;
					}
					for (; a; a = next[a], w = tip[a])
						if ((rcap[a] > epsilon) && (dist[w] < newdis))
							newdis = dist[w];
				}



				ndist[dv]--;

				if (newdis == bound) {
					Err0("GT phase 2: relabeling failed\n");
					exit(1);
				} else {


					newdis++;
					dist[v] = newdis;
					currentarc[v] = firsta;
					hd = newdis;



				}



			}
		}


		while (hd && (firstact[hd] == 0))
			--hd;



	}



	Err0("time for phase 2:");
	printtime(stderr, cputime() - cpuphase2);
	Err0("\n");




	if (fabs(excess[source] + excess[sink]) < epsilon)
		Err0("maxflow == mincut\n");
	else {
		Err1("excess[source] = %lf\n", excess[source]);
		Err1("excess[sink]   = %lf\n", excess[sink]);
	}





	for_nodes(v)
		if ((excess[v] > epsilon) && (v != source) && (v != sink))
		Err2("excess[%d] = %lf\n", v, excess[v]);
	!







#endif


		Free(succact);
	Free(firstact);




	Free(wsucc);
	Free(wpred);
	Free(awake);



	Free(rcap);
	Free(excess);
	Free(dist);
	Free(ndist);
	Free(currentarc);




}
