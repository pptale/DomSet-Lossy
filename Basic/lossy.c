//Contains lossy reduction rule for dominating set

int spot_holder_v2(int xvertex, int * n, int ** ver, int ** edges, int * color, int * overload, int * second_degree, int * deg_min_heap, int * pos_deg_min_heap, int * allowed_main_spotter){
  // Returns a spot holder z for given x
  // v2 : Uses min_heap to store minimum second_degree vertex. 
  // Def: z is spot holder for x if for all w in N[z] following
  // conditions are satisfied
  // (1) color[z] = 1
  // (2) overload[w] >= 1
  // (3) There is no edge between z and x
  // It returns a vertex z of minimum second_degree in graph which satisfy these three criteria

  int * no_more_main_spotter; // if xvertex is found to have a main spotter zvertex then any adj
  // vertices of xvertex which were examined before zvertex can no longer be main_spotter
  // this array stores that. If there is no main spotter found for xvertex then we don't
  // change deg_min_heap.
  no_more_main_spotter = (int *) malloc(sizeof(int) * (2 + *n));

  if (no_more_main_spotter == NULL){
    printf("No space for no_more_main_spotter \n");
  }
  
  int index_no_more_main_spotter = 1; // to control size of no_more_main_spotter.
  bool is_main_spotter;
  int temp_vertex;
  int zvertex = -1;
  
  for(int i=1; i <= *n; i++){
    temp_vertex = deg_min_heap[i];

    if ((color[temp_vertex] == 0) || (allowed_main_spotter[temp_vertex] == 0)){
      no_more_main_spotter[index_no_more_main_spotter] = temp_vertex;
      index_no_more_main_spotter += 1;
      continue;
    }

    if (second_degree[temp_vertex] == *n){
      break;
    }

    
    is_main_spotter = true;

    for(int j = (*ver)[temp_vertex]; j < (*ver)[temp_vertex + 1]; j++){
      if (((*edges)[j] == xvertex ) || (overload[(*edges)[j]] < 1)){
	is_main_spotter = false;
	no_more_main_spotter[index_no_more_main_spotter] = temp_vertex;
	index_no_more_main_spotter += 1;
      }
    }

    if (is_main_spotter == true){
      zvertex = temp_vertex;
      break;
    }
  }

  if (zvertex == -1){
    free(no_more_main_spotter);
    return zvertex;
  }

  for(int i = index_no_more_main_spotter - 1; i >= 1; i--){
    //It is important to min_heapify at largest index first
    second_degree[no_more_main_spotter[i]] = *n;
    _min_heapify_increase_at(&deg_min_heap, &second_degree, &pos_deg_min_heap, i, *n);
  }

  free(no_more_main_spotter);
  return zvertex;
}

int greater(const void * a, const void * b)
{
  //Used in qsort.
  //Return true if b is greater than a
  return (*(int*)b  - *(int*)a );
}

int rr_lossy_greedy(int *n, int ** ver, int ** edges, int * color, int * solution, int * overload){
  // Lossy Reduction Rule
  // It modifies color and solution array.
  // Includes some vertices in solution based on lossy_accuracy.
  // greedy : We construct set X which is to be included in solution in greedy faction. We pick vertex $x$ as first vertex with heightest black neighbors and $y$ as its neighbor of lowest degree. We implement this function to make sure it runs in O(n log n) time

  int * cls_nbr_black; // To store number vertices with black color in closed neighbhorhood of vertex
  cls_nbr_black = (int *) malloc(sizeof(int)*(2 + *n));  
  int * black_deg_max_heap; // To maintains black deg heap
  black_deg_max_heap = (int *) malloc(sizeof(int)*(2 + *n));
  int * deg_min_heap; // To maintain vertices of minimum degree
  deg_min_heap = (int *) malloc(sizeof(int) * (2 + *n));
  int * pos_deg_min_heap; // Position in deg_min_heap
  pos_deg_min_heap = (int *) malloc(sizeof(int) * (2 + *n));
  int * pos_black_deg_max_heap; // Position in black_deg_max_heap
  pos_black_deg_max_heap = (int *) malloc(sizeof(int) * (2 + *n));
  int * degree; // Store degree of vertices
  degree = (int *) malloc(sizeof(int) * (2 + *n));
  int * second_degree; // Store second_degree of vertices
  second_degree = (int *) malloc(sizeof(int) * (2 + *n));
  int * allowed_in_X; // To store vertices which are allowed in set X which is deleted as part of reduction rule. If z is main-spotter for some x then N[z] is not allowed in X.
  allowed_in_X = (int *) malloc(sizeof(int) * (2 + *n));
  int * allowed_main_spotter; //To store if a vertex can be main spotter or not. If x is inclued in X then no neighbor of x can be main spotter.
  allowed_main_spotter = (int *) malloc(sizeof(int) * (2 + *n));
    
  if ((cls_nbr_black == NULL) || (black_deg_max_heap == NULL) || (deg_min_heap == NULL) || (pos_deg_min_heap == NULL) || (pos_black_deg_max_heap == NULL) || (degree == NULL) || (second_degree == NULL) || (allowed_in_X == NULL) || (allowed_main_spotter == NULL)) {
    printf("Error Allocating memory for cls_nbr_black or deg_min_heap or black_deg_max_heap or pos_black_deg_max_heap or degree array  \n");
    return (false);
  }

  //Initialize all arrays
  for(int i = 1; i <= *n; i++){
    degree[i] = (*ver)[i+1] - (*ver)[i];
    second_degree[i] = (*ver)[i+1] - (*ver)[i];
    cls_nbr_black[i] = color[i]; //If vertex itself is black then it will be counted as one
    for(int j= (*ver)[i]; j < (*ver)[i+1]; j++){
      second_degree[i] += degree[(*edges)[j]];
      if (color[(*edges)[j]] == 1){
	cls_nbr_black[i] += 1;
      }
    }
    black_deg_max_heap[i] = i;
    pos_black_deg_max_heap[black_deg_max_heap[i]] = i;
    deg_min_heap[i] = i;
    pos_deg_min_heap[deg_min_heap[i]] = i;
    allowed_in_X[i] = 1;
    allowed_main_spotter[i] = 1;
  }
  
  int index =  ceil(*n/2);
  while(index >= 1){
    _max_heapify_decrease_at(&black_deg_max_heap, &cls_nbr_black, &pos_black_deg_max_heap, index, *n);
    //_min_heapify_increase_at(&deg_min_heap, &degree, &pos_deg_min_heap, index, *n);
    _min_heapify_increase_at(&deg_min_heap, &second_degree, &pos_deg_min_heap, index, *n);
    index -= 1;
  }

  //int x, y;
  //int xvertex;
  int xvertex, yvertex, zvertex, wvertex, tempvertex;

  for (int i=1; i <= *n; i++){
    //TEST
    //if (!is_max_heap(n, black_deg_max_heap, cls_nbr_black)){
    //  printf("Not max heap. %d\n", i);
    //}
    //TEST
    //if (!is_min_heap(n, deg_min_heap, degree)){
    //  printf("Not min heap. %d\n", i);
    //}
    xvertex = black_deg_max_heap[1]; // max deg at the top of heap.
    if (cls_nbr_black[xvertex] <= 0) break;
        
    cls_nbr_black[xvertex] = 0; // setting black_degree of this vertex to zero so that it will not be considered again
    
    if (allowed_in_X[i] != 1){
      // This vertex is not allowed in X
      _max_heapify_decrease_at(&black_deg_max_heap, &cls_nbr_black, &pos_black_deg_max_heap, 1, *n);// Modifing at pos_black_deg_max_heap[xvertex] which is 1
      continue;
    }
    
    //zvertex = spot_holder(xvertex, n, ver, edges, color, overload, degree);
    //zvertex = spot_holder(xvertex, n, ver, edges, color, overload, second_degree);
    //zvertex = spot_holder_v2(xvertex, n, ver, edges, color, overload, degree, deg_min_heap, pos_deg_min_heap, allowed_main_spotter);
    zvertex = spot_holder_v2(xvertex, n, ver, edges, color, overload, second_degree, deg_min_heap, pos_deg_min_heap, allowed_main_spotter);

    //printf("%d \t %d \n", xvertex, zvertex);
    if (zvertex == -1){
      // no main spotter for this xvertex
      _max_heapify_decrease_at(&black_deg_max_heap, &cls_nbr_black, &pos_black_deg_max_heap, 1, *n);// Modifing at pos_black_deg_max_heap[xvertex] which is 1
      continue; 
    }

    overload[zvertex] -= 1;
    allowed_in_X[zvertex] = 0; // zvertex and its nbrs are not allowed in X now
    allowed_main_spotter[zvertex] = 0; //zvertex and its nbrs can not be spotter of any other vertex
    for(int j = (*ver)[zvertex]; j < (*ver)[zvertex + 1]; j++){
      overload[(*edges)[j]] -= 1;
      allowed_in_X[(*edges)[j]] = 0;
      allowed_main_spotter[(*edges)[j]] = 0;
    }
    
    //printf("%d ", cls_nbr_black[xvertex]);
    
    int * modified_positions = (int *) malloc(sizeof(int) * (2 + *n));
    int size_modified_positions = 0; 

    for(int j= (*ver)[xvertex]; j < (*ver)[xvertex + 1]; j++){
      yvertex = (*edges)[j];
      cls_nbr_black[yvertex] -= color[xvertex];
      allowed_main_spotter[yvertex] = 0;
      
      if (color[yvertex] == 1){
	for(int k = (*ver)[yvertex]; k < (*ver)[yvertex]; k++){
	  wvertex = (*edges)[k];
	  cls_nbr_black[wvertex] -= 1; // as color[yvertex] will be turned to 0.
	  modified_positions[size_modified_positions] = pos_black_deg_max_heap[wvertex];
	  size_modified_positions += 1;
	}
	color[yvertex] = 0;
      }
      
      modified_positions[size_modified_positions] = pos_black_deg_max_heap[yvertex];
      size_modified_positions += 1;
    }

    solution[xvertex] = 1;
    color[xvertex] = 0;

    modified_positions[size_modified_positions] = 1; // last entry in position corresponds to xvertex in heap (which is one).
    size_modified_positions += 1;
    
    qsort(modified_positions, size_modified_positions, sizeof(int), greater);
    
    for(int j = 0; j < size_modified_positions; j++){
      //printf("%d ", positions[j]);
      _max_heapify_decrease_at(&black_deg_max_heap, &cls_nbr_black, &pos_black_deg_max_heap, modified_positions[j], *n);
    }
    free(modified_positions);
  }

  int not_allowed_main_spotter = 0;
  int not_allowed_in_X = 0;
  for(int i = 1; i <= *n; i++){
    if(allowed_main_spotter[i] == 0){
      not_allowed_main_spotter += 1;
    }
    if(allowed_in_X[i] == 0){
      not_allowed_in_X += 1;
    }
  }
  
  free(cls_nbr_black);
  free(black_deg_max_heap);
  free(deg_min_heap);
  free(pos_deg_min_heap);
  free(pos_black_deg_max_heap);
  free(degree);
  free(second_degree);
  free(allowed_in_X);
  free(allowed_main_spotter);
  return(true);
}
