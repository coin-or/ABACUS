


#include "mincut.h"



#define remove_candidate(e)          \
{ int p_rc, s_rc;                \
  if (candpred[e] != -1) {           \
    p_rc =  candpred[e];              \
    s_rc =  candsucc[e];              \
    if (p_rc) candsucc[p_rc] =  s_rc; \
    else candidate =  s_rc;           \
    if (s_rc) candpred[s_rc] =  p_rc; \
    candpred[e] =  -1;                \
    ncands--;                        \
  }                                  \
}



#define insert_candidate(e)                             \
{ int p_ic, s_ic;                                   \
  if (candpred[e] != -1) {   \
  p_ic =  candpred[e];                                 \
    s_ic =  candsucc[e];                                 \
    if (p_ic) candsucc[p_ic] =  s_ic;                    \
    else candidate =  s_ic;                              \
    if (s_ic) candpred[s_ic] =  p_ic;                    \
    ncands--;                                           \
  }                                                     \
  candpred[e] =  0;                                      \
  candsucc[e] =  candidate;                              \
  candpred[candidate] =  e;                              \
  candidate =  e;                                        \
  ncands++;                                             \
}



#define fatsize 10
#define add_to_fat_list(x)                              \
{ int i_afl, p_afl, r_afl, y_afl, z_afl, e_afl =  x; \
   if (curfat < fatsize)                                \
     fat[++curfat] =  e_afl;                             \
   else if (weight[arc_of(e_afl)] > minfat)             \
     fat[curfat] =  e_afl;                               \
   i_afl =  curfat;                                      \
   while (i_afl > 1) {                                  \
     r_afl =  fat[i_afl];                                \
     y_afl =  weight[arc_of(r_afl)];                     \
     p_afl =  fat[i_afl - 1];                            \
     z_afl =  weight[arc_of(p_afl)];                     \
     if (y_afl > z_afl) {                               \
       fat[i_afl - 1] =  r_afl;                          \
       fat[i_afl] =  p_afl;                              \
       i_afl--;                                         \
      }                                                 \
     else                                               \
       i_afl =  1;                                       \
   }                                                    \
   minfat =  weight[arc_of(fat[curfat])];                \
}




void 
PadbergRinaldi(ARGUMENT_LIST)
ARGUMENT_DECLARATIONS
{




	int             node_size;
	int             edge_size;
	int             curnnod;
	int             curnedg;
	int            *first;
	int            *prev;
	int            *next;
	int            *tip;
	int            *orig;
	Weight         *weight;
	Weight         *star_w;


	int             neighbor;
	int             neighborarc;
	int             nextarc;








	int             cutcar;
	Weight          flow;
	Weight          zerotolerance;
	int            *cutlist = 0;





	int             nmarks;
	int            *markednode;
	int            *marked;
	int            *linkarc;





	int            *last;
	int            *succ;
	int             supernode;





	int             candidate, ncands, curfat;
	Weight          minfat = zero;
	int            *candpred = 0;
	int            *candsucc = 0;
	int            *fat = 0;










	int            *out_degree;
	int            *index;
	int            *new_tail;
	int            *new_head;
	Weight         *new_ecap;
	int             t_index;
	int             e;
	int             v;
	int             t;


	;


	Ialloc_and_clear(out_degree, nnod);

	for (e = 1; e <= nedg; e++)
		out_degree[tail[e]]++;


	;


	Ialloc(index, nnod);

	index[1] = 1;
	for (v = 1; v < nnod; v++)
		index[v + 1] = index[v] + out_degree[v];


	;


	Ialloc(new_tail, nedg);
	Ialloc(new_head, nedg);
	Walloc(new_ecap, nedg);

	for (e = 1; e <= nedg; e++) {
		t = tail[e];
		t_index = index[t];
		new_tail[t_index] = tail[e];
		new_head[t_index] = head[e];
		new_ecap[t_index] = ecap[e];
		index[t]++;
	}


	;


	for (e = 1; e <= nedg; e++) {
		tail[e] = new_tail[e];
		head[e] = new_head[e];
		ecap[e] = new_ecap[e];
	}


	;


	Free(out_degree);



	Free(index);



	Free(new_tail);
	Free(new_head);
	Free(new_ecap);


	;


	;
	Graphalloc(nnod, nedg);


	{
		Hash            hash_area;
		Hash           *H = &hash_area;
		int             j, e, t, h;
		Weight          xe;

		if (one_to_n) {


			{
				curnnod = nnod;
				for_nodes(j) {
					orig[j] = j;
					first[j] = 0;
					star_w[j] = zero;
				}



			}





			{
				curnedg = 0;
				for (e = 1; e <= nedg; e++) {
					xe = ecap[e];
					t = tail[e];
					if ((t < 1) || (t > curnnod))
						t = 0;
					h = head[e];
					if ((h < 1) || (h > curnnod))
						h = 0;
					if (t && h && (xe > epsilon)) {
						insert_edge(t, h, xe);
						star_w[t] += xe;
						star_w[h] += xe;
					}
				}
			}



		} else {


			{
				curnnod = (ncno <= 0) ? nnod : ncno;
				H->size = curnnod;
				H->key_len = sizeof(int);


				{
					long            i;

					H->keys = (char *) malloc((size_t) (H->size + 1) * H->key_len);
					chkmem(H->keys);
					H->head = (long *) malloc((size_t) (H->size + 1) * sizeof(char *));
					chkmem(H->head);
					H->link = (long *) malloc((size_t) (H->size + 1) * sizeof(char *));
					chkmem(H->link);
					H->key.C = (char *) malloc((size_t) H->key_len * sizeof(char));
					chkmem(H->key.C);
					for (i = 0; i < (H->size + 1); i++)
						H->head[i] = 0;
					H->last_index = 0;
				}




				if (ncno > 0)
					for_nodes(j) {
					H->key.I[0] = node[j];


					{
						long            p;
						long            missing;


						{
							unsigned char  *t;
							long            i;

							for (i = 0, t = (unsigned char *) H->key.C, H->index = 0; i < H->key_len; i++, t++) {
								H->index += (H->index ^ (H->index >> 1)) + HASH_MULT * (*t);
								while (H->index >= HASH_PRIME)
									H->index -= HASH_PRIME;
							}
							H->index = H->index % H->size;
						}



						H->index++;
						p = H->head[H->index];
						if (!p) {


							if (H->last_index < H->size) {
								H->link[++H->last_index] = H->head[H->index];
								H->head[H->index] = H->last_index;
								(void) memcpy(H->keys + H->last_index * H->key_len, H->key.C,
									      (size_t) H->key_len);
								H->index = H->last_index;
							} else {
								H->index = -1;
							}



						} else {
							while ((missing = (memcmp(H->keys + p * H->key_len,
										  H->key.C, (size_t) H->key_len) != 0))
							  && (H->link[p])) {
								p = H->link[p];
							}
							if (missing) {


								if (H->last_index < H->size) {
									H->link[++H->last_index] = H->head[H->index];
									H->head[H->index] = H->last_index;
									(void) memcpy(H->keys + H->last_index * H->key_len, H->key.C,
										      (size_t) H->key_len);
									H->index = H->last_index;
								} else {
									H->index = -1;
								}



							} else {
								H->index = 0;
							}
						}
					}



					if (H->index) {
						orig[H->index] = H->key.I[0];
						first[H->index] = 0;
						star_w[H->index] = zero;
					}
					}
			}





			{
				curnedg = 0;
				for (e = 1; e <= nedg; e++) {
					xe = ecap[e];
					H->key.I[0] = tail[e];


					{
						long            p;



						{
							unsigned char  *t;
							long            i;

							for (i = 0, t = (unsigned char *) H->key.C, H->index = 0; i < H->key_len; i++, t++) {
								H->index += (H->index ^ (H->index >> 1)) + HASH_MULT * (*t);
								while (H->index >= HASH_PRIME)
									H->index -= HASH_PRIME;
							}
							H->index = H->index % H->size;
						}



						H->index++;
						p = H->head[H->index];
						while (p && (memcmp(H->keys + p * H->key_len,
								    H->key.C, (size_t) H->key_len) != 0))
							p = H->link[p];
						H->index = p;
					}


					if ((ncno <= 0) && (H->index == 0)) {


						{
							long            p;
							long            missing;


							{
								unsigned char  *t;
								long            i;

								for (i = 0, t = (unsigned char *) H->key.C, H->index = 0; i < H->key_len; i++, t++) {
									H->index += (H->index ^ (H->index >> 1)) + HASH_MULT * (*t);
									while (H->index >= HASH_PRIME)
										H->index -= HASH_PRIME;
								}
								H->index = H->index % H->size;
							}



							H->index++;
							p = H->head[H->index];
							if (!p) {


								if (H->last_index < H->size) {
									H->link[++H->last_index] = H->head[H->index];
									H->head[H->index] = H->last_index;
									(void) memcpy(H->keys + H->last_index * H->key_len, H->key.C,
										      (size_t) H->key_len);
									H->index = H->last_index;
								} else {
									H->index = -1;
								}



							} else {
								while ((missing = (memcmp(H->keys + p * H->key_len,
											  H->key.C, (size_t) H->key_len) != 0))
								       && (H->link[p])) {
									p = H->link[p];
								}
								if (missing) {


									if (H->last_index < H->size) {
										H->link[++H->last_index] = H->head[H->index];
										H->head[H->index] = H->last_index;
										(void) memcpy(H->keys + H->last_index * H->key_len, H->key.C,
											      (size_t) H->key_len);
										H->index = H->last_index;
									} else {
										H->index = -1;
									}



								} else {
									H->index = 0;
								}
							}
						}





						{
							orig[H->index] = H->key.I[0];
							first[H->index] = 0;
							star_w[H->index] = zero;
						}



					}
					t = H->index;
					if (t) {
						H->key.I[0] = head[e];


						{
							long            p;



							{
								unsigned char  *t;
								long            i;

								for (i = 0, t = (unsigned char *) H->key.C, H->index = 0; i < H->key_len; i++, t++) {
									H->index += (H->index ^ (H->index >> 1)) + HASH_MULT * (*t);
									while (H->index >= HASH_PRIME)
										H->index -= HASH_PRIME;
								}
								H->index = H->index % H->size;
							}



							H->index++;
							p = H->head[H->index];
							while (p && (memcmp(H->keys + p * H->key_len,
									    H->key.C, (size_t) H->key_len) != 0))
								p = H->link[p];
							H->index = p;
						}


						if ((ncno <= 0) && (H->index == 0)) {


							{
								long            p;
								long            missing;


								{
									unsigned char  *t;
									long            i;

									for (i = 0, t = (unsigned char *) H->key.C, H->index = 0; i < H->key_len; i++, t++) {
										H->index += (H->index ^ (H->index >> 1)) + HASH_MULT * (*t);
										while (H->index >= HASH_PRIME)
											H->index -= HASH_PRIME;
									}
									H->index = H->index % H->size;
								}



								H->index++;
								p = H->head[H->index];
								if (!p) {


									if (H->last_index < H->size) {
										H->link[++H->last_index] = H->head[H->index];
										H->head[H->index] = H->last_index;
										(void) memcpy(H->keys + H->last_index * H->key_len, H->key.C,
											      (size_t) H->key_len);
										H->index = H->last_index;
									} else {
										H->index = -1;
									}



								} else {
									while ((missing = (memcmp(H->keys + p * H->key_len,
												  H->key.C, (size_t) H->key_len) != 0))
									       && (H->link[p])) {
										p = H->link[p];
									}
									if (missing) {


										if (H->last_index < H->size) {
											H->link[++H->last_index] = H->head[H->index];
											H->head[H->index] = H->last_index;
											(void) memcpy(H->keys + H->last_index * H->key_len, H->key.C,
												      (size_t) H->key_len);
											H->index = H->last_index;
										} else {
											H->index = -1;
										}



									} else {
										H->index = 0;
									}
								}
							}





							{
								orig[H->index] = H->key.I[0];
								first[H->index] = 0;
								star_w[H->index] = zero;
							}



						}
						h = H->index;
					}
					if (t && h && (xe > epsilon)) {
						insert_edge(t, h, xe);
						star_w[t] += xe;
						star_w[h] += xe;
					}
				}
			}





			Free(H->keys);
			Free(H->head);
			Free(H->link);
			Free(H->key.C);



		}
	}







	Ialloc(cutlist, node_size);





	{
		Ialloc(markednode, node_size);
		Ialloc_and_clear(marked, node_size);
		Ialloc(linkarc, node_size);
	}





	{
		Ialloc(last, node_size);
		Ialloc(succ, node_size);
	}





	{
		Ialloc(candpred, edge_size);
		Ialloc(candsucc, edge_size);
		Ialloc(fat, fatsize);
	}








	zerotolerance = (Weight) epsilon / two;





	nmarks = 0;




	{
		int             v;

		for_nodes(v) {
			last[v] = v;
			succ[v] = 0;
		}
	}





	{
		int             v;
		Weight          w;

		*mincutvalue = infinity;
		for_nodes(v) {
			w = star_w[v];
			if (w < (*mincutvalue)) {
				*mincutvalue = w;
				*ncutarray = 1;
				cutarray[1] = orig[v];
				if (w <= epsilon)
					goto freeall;
			}
		}
	}






	{
		int             e;

		for_edges(e) candpred[e] = -1;
		curfat = 0;
		for_edges(e) add_to_fat_list(e);
		candidate = 0;
		ncands = 0;


		{
			int             i;
			int             e;

			for (i = curfat; i >= 1; i--) {
				e = fat[i];
				insert_candidate(e);
			}
			curfat = 0;
		}



		for_edges(e) {
			int             twoe;
			Weight          ttecse, scte, sche;

			twoe = e << 1;
			ttecse = two * weight[twoe];
			scte = star_w[tip[twoe]];
			sche = star_w[tip[twoe - 1]];
			if ((ttecse >= scte) || (ttecse >= sche))
				insert_candidate(e);
		}
	}





	{

		int             se;
		int             se_t;
		int             se_h;
		int             se_tarc;
		int             se_harc;
		Weight          se_w;




		while (curnedg > 1) {
			do {
				se = candidate;
				if (se) {
					remove_candidate(se);
					find_end_vertices(se, se_t, se_h, se_tarc, se_harc);
					se_w = weight[se_tarc];


					{
						int             ee, ff;
						Weight          ecff, ecee,
						                schse,
						                sctse,
						                mincap,
						                totcap,
						                scneighbor;

						totcap = se_w;
						if (se_w >= (*mincutvalue) - epsilon)
							goto shrink;
						sctse = star_w[se_t];
						if (sctse <= two * se_w + epsilon)
							goto shrink;
						schse = star_w[se_h];
						if (schse <= two * se_w + epsilon)
							goto shrink;


						{
							remove_marks;
							for_neighbors(se_t, neighbor, neighborarc) {
								mark(neighbor);
								linkarc[neighbor] = neighborarc;
							}
						}



						for_neighbors(se_h, neighbor, neighborarc)
							if (marked[neighbor]) {
							ff = edge_of(linkarc[neighbor]);
							ecff = weight[linkarc[neighbor]];
							ee = edge_of(neighborarc);
							ecee = weight[neighborarc];
							scneighbor = star_w[neighbor];
							mincap = (ecee < ecff) ? ecee : ecff;
							totcap += mincap;
							if (totcap >= (*mincutvalue) - epsilon)
								goto shrink;
							if ((sctse <= two * (ecff + se_w) + epsilon) &&
							    (schse <= two * (ecee + se_w) + epsilon))
								goto shrink;
							mincap = (se_w < ecff) ? se_w : ecff;
							if ((ecee + mincap >= (*mincutvalue) - epsilon) ||
							    ((schse <= two * (ecee + se_w) + epsilon) &&
							     (scneighbor <= two * (ecee + ecff) + epsilon))) {
								se = ee;
								find_end_vertices(se, se_t, se_h, se_tarc, se_harc);
								se_w = ecee;
								goto shrink;
							}
							mincap = (se_w < ecee) ? se_w : ecee;
							if ((ecff + mincap >= (*mincutvalue) - epsilon) ||
							    ((sctse <= two * (ecff + se_w) + epsilon) &&
							     (scneighbor <= two * (ecff + ecee) + epsilon))) {
								se = ff;
								find_end_vertices(se, se_t, se_h, se_tarc, se_harc);
								se_w = ecff;
								goto shrink;
							}
						}
						if (PRIL)
							Err0("-");
					}



				}
			} while (se);



			{
				int             v;
				int             s;
				Weight          bigcap, maxcap, w;

				remove_marks;
				maxcap = -infinity;
				for_nodes(v) {
					w = star_w[v];
					if (w > maxcap) {
						maxcap = w;
						s = v;
					}
				}
				mark(s);
				maxcap = -infinity;
				for_neighbors(s, neighbor, neighborarc) {
					mark(neighbor);
					bigcap = star_w[neighbor] - two * weight[neighborarc];
					if (bigcap > maxcap) {
						maxcap = bigcap;
						se = edge_of(neighborarc);
					}
				}
				find_end_vertices(se, se_t, se_h, se_tarc, se_harc);
				se_w = weight[se_tarc];
				for_nodes(v) if (!marked[v]) {
					bigcap = star_w[v];
					if (bigcap > maxcap) {
						maxcap = bigcap;
						se_t = s;
						se_h = v;
						se = 0;
						se_w = zero;
					}
				}
			}





			{
				int             ee, ff;
				Weight          ecff, ecee, schse, sctse,
				                mincap, totcap, scneighbor;

				totcap = se_w;
				if (se_w >= (*mincutvalue) - epsilon)
					goto shrink;
				sctse = star_w[se_t];
				if (sctse <= two * se_w + epsilon)
					goto shrink;
				schse = star_w[se_h];
				if (schse <= two * se_w + epsilon)
					goto shrink;


				{
					remove_marks;
					for_neighbors(se_t, neighbor, neighborarc) {
						mark(neighbor);
						linkarc[neighbor] = neighborarc;
					}
				}



				for_neighbors(se_h, neighbor, neighborarc)
					if (marked[neighbor]) {
					ff = edge_of(linkarc[neighbor]);
					ecff = weight[linkarc[neighbor]];
					ee = edge_of(neighborarc);
					ecee = weight[neighborarc];
					scneighbor = star_w[neighbor];
					mincap = (ecee < ecff) ? ecee : ecff;
					totcap += mincap;
					if (totcap >= (*mincutvalue) - epsilon)
						goto shrink;
					if ((sctse <= two * (ecff + se_w) + epsilon) &&
					    (schse <= two * (ecee + se_w) + epsilon))
						goto shrink;
					mincap = (se_w < ecff) ? se_w : ecff;
					if ((ecee + mincap >= (*mincutvalue) - epsilon) ||
					    ((schse <= two * (ecee + se_w) + epsilon) &&
					     (scneighbor <= two * (ecee + ecff) + epsilon))) {
						se = ee;
						find_end_vertices(se, se_t, se_h, se_tarc, se_harc);
						se_w = ecee;
						goto shrink;
					}
					mincap = (se_w < ecee) ? se_w : ecee;
					if ((ecff + mincap >= (*mincutvalue) - epsilon) ||
					    ((sctse <= two * (ecff + se_w) + epsilon) &&
					     (scneighbor <= two * (ecff + ecee) + epsilon))) {
						se = ff;
						find_end_vertices(se, se_t, se_h, se_tarc, se_harc);
						se_w = ecff;
						goto shrink;
					}
				}
				if (PRIL)
					Err0("-");
			}





			{
				if (PRIL)
					Err0("*");
				GoldbergTarjan(se_t, se_h, curnnod, edge_size, tip, first, next, weight,
					       &flow, &cutcar, cutlist);


				if (flow < (*mincutvalue)) {
					int             i, v;
					*mincutvalue = flow;
					*ncutarray = 0;
					for (i = 1; i <= cutcar; i++) {
						v = cutlist[i];
						do {
							cutarray[++(*ncutarray)] = orig[v];
							v = succ[v];
						} while (v);
					}
					if (flow <= epsilon)
						goto freeall;
				}
			}



	shrink:


			{
				if (PRIL)
					Err0("o");


				{
					remove_marks;
					for_neighbors(se_t, neighbor, neighborarc) {
						mark(neighbor);
						linkarc[neighbor] = neighborarc;
					}
				}





				{
					int             ff, ee, lan;
					int             otherarc;
					Weight          ecee;

					for_neighbors_save_next(se_h, neighbor, neighborarc, nextarc) {
						otherarc = mate_of(neighborarc);
						ee = edge_of(neighborarc);
						ecee = weight[neighborarc];
						if (marked[neighbor]) {
							lan = linkarc[neighbor];
							ff = edge_of(lan);
							weight[lan] += ecee;
							weight[mate_of(lan)] += ecee;
							add_to_fat_list(ff);
							remove_edge(neighborarc, otherarc);
							remove_candidate(ee);
						} else if (ee != se) {
							change_tip(neighborarc, otherarc, se_t);
						}
					}
					star_w[se_t] += (star_w[se_h] - two * se_w);
					if ((!curfat) && (ncands < fatsize)) {


						for_neighbors(se_t, neighbor, neighborarc)
							add_to_fat_list(edge_of(neighborarc));



					} {
						int             i;
						int             e;

						for (i = curfat; i >= 1; i--) {
							e = fat[i];
							insert_candidate(e);
						}
						curfat = 0;
					}



				}



				if (se) {
					remove_edge(se_tarc, se_harc);
					remove_candidate(se);
				} {
					int             v;

					if (se_h < curnnod) {
						v = orig[se_h];
						orig[se_h] = orig[curnnod];
						orig[curnnod] = v;

						v = succ[se_h];
						succ[se_h] = succ[curnnod];
						succ[curnnod] = v;

						v = last[se_h];
						if (last[se_h] == se_h)
							v = curnnod;
						last[se_h] = last[curnnod];
						if (last[curnnod] == curnnod)
							last[se_h] = se_h;
						last[curnnod] = v;

						star_w[se_h] = star_w[curnnod];
						first[se_h] = first[curnnod];
						for_star(se_h, neighborarc) tip[mate_of(neighborarc)] = se_h;
						if (se_t == curnnod)
							se_t = se_h;
						se_h = curnnod;
					}
					curnnod--;
				}





				{
					succ[last[se_t]] = se_h;
					last[se_t] = last[se_h];
				}



				supernode = se_t;


				{
					int             v;
					Weight          s;

					s = star_w[supernode];
					if (s < (*mincutvalue)) {
						*mincutvalue = s;
						*ncutarray = 0;
						v = supernode;
						do {
							cutarray[++(*ncutarray)] = orig[v];
							v = succ[v];
						} while (v);
						if (s <= epsilon)
							goto freeall;
					}
				}


			}



		}
	}





freeall:


	Free(star_w);
	Free(weight);
	Free(orig);
	Free(tip);
	Free(next);
	Free(prev);
	Free(first);





	Free(cutlist);





	Free(linkarc);
	Free(markednode);
	Free(marked);





	Free(succ);
	Free(last);





	Free(candpred);
	Free(candsucc);
	Free(fat);






}
