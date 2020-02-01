// Contains basic graph operations

void print_adj_list(int n, int *ver, int *edges){
  // For every vertex v, prints v and vertices adj to it.
  for(int i=1; i <= n; i++){
    printf("%d : ", i);
    for(int j= ver[i];j < ver[i+1]; j++) {
      printf("%d ", edges[j]);
    }
    printf("\n");
  }
}

void print_degree(int n, int *ver, int *edges){
  // For every vertex v, prints v and its degree
  printf("\n *** Print Degree Starts \n");
  int degree = 0;
  for(int i=1; i <= n; i++){
    printf("%d (%d) ", i, (ver[i+1] - ver[i]));
  }
  printf("\n *** Print Degree Ends **** \n");
}


bool is_dominating_set(int *n, int ** ver, int ** edges, int *color, int *solution){
  //Returns true if vertices in solution dominates all vertices which has black color. 
  int * is_dominated; //To check whether a vertex is dominated or not.
  is_dominated = (int *) malloc(sizeof(int) * (2 + *n));
  for(int i=1; i <= *n; i++){
    is_dominated[i] = color[i]; //Only check for vertices with black color
  }

  for(int i=1; i <= *n; i++){
    if (solution[i] == 1){
      is_dominated[i] = 0;
      for(int j=(*ver)[i]; j < (*ver)[i+1]; j++){
	is_dominated[(* edges)[j]] = 0;
      }
    }
  }

  for(int i=1; i <= *n; i++){
    if (is_dominated[i] == 1){
      //printf("%d ", i);
      free(is_dominated);
      return false;
    }
  }
  
  free(is_dominated);
  return true;
}

bool check_coloring(int *n, int *ver, int *edges, int *color, int *solution){
  // Used to check coloring with resp to present (partial) solution.
  // All vertices in closed neighborhood of solution vertices should have white color
  for(int i=1; i < *n; i++){
    if(solution[i] == 1){
      if (color[i] == 1) return false;
      for(int j=ver[i]; j < ver[i+1]; j++){
	if (color[edges[j]] == 1) return false;
      }
    }
  }
  return true;
}

int min_deg_vertex(int *n, int *ver, int *edges){
  // Return index of vertex of minimum degree.
  // If there are more than two vertices of minimum degree then it returns the one with smaller index.
  int index = 1;
  int min_deg = 0;
  for(int i=1; i <= *n; i++){
    if (min_deg > (ver[i+1] - ver[i])){
      min_deg = ver[i+1] - ver[i];
      index = i;
    }
  }
  return index;
}

int degeneracy_order(int *n, int ** ver, int ** edges, int * deg_order){
  //TODO: FIX THIS. Check if degeneracy is 1 then graph is acyclic
  //Returns degeneracy of graph
  //deg_order[i] = x implies that vertex x is at i^th place in degeneracy order

  int * degree; // Store the degree of vertices
  degree = (int *) malloc(sizeof(int) * (2 + *n));
  if (degree == NULL){
    printf("Unable to allocate space for degree array \n");
    return(false);
  }

  for(int i=1; i <= *n; i++){
    degree[i] = (*ver)[i+1] - (*ver)[i];
  }
  
  int degeneracy = 0;
  int min_val_index;
  int min_val_degree;
  //Fixing the order
  // 
  for(int i = 1; i <= *n; i++){
    min_val_index = 1;
    min_val_degree = 2 * (*n);
    for (int j = 1; j <= *n; j++){
      if (degree[j] < min_val_degree){
	min_val_index = j;
	min_val_degree = degree[j];
      }
    }
    
    deg_order[i] = min_val_index;
    //printf("| %d %d (%d) ", i,  min_val_index, degree[min_val_index]);

    if (degeneracy < degree[min_val_index]){
      degeneracy = degree[min_val_index];
    }

    degree[min_val_index] = 2 * (*n); //To ensure that this vertex is never considered. 
    //Reduce the degree of neighbors of min_val_index by 1 
    for(int k=(*ver)[min_val_index]; k < (*ver)[min_val_index+1]; k++){
      if (degree[(* edges)[k]] < (*n)) {
	degree[(* edges)[k]] -= 1;
      }
    }
    
  }
  free(degree);
  return degeneracy;
}


int greater1(const void * a, const void * b)
{
  //Used in qsort.
  //Return true if b is greater than a
  return (*(int*)b  - *(int*)a );
}

int lesser1(const void * a, const void * b)
{
  //Used in qsort.
  //Return true if b is lesser than a
  return (*(int*)a - *(int*)b);
}

int degeneracy_order_v1(int *n, int ** ver, int ** edges, int * deg_order){
  //Returns degeneracy of graph
  //deg_order[i] = x implies that vertex x is at i^th place in degeneracy order
  // v1 : runs in O(m log(n)) time

  int * degree; // Store the degree of vertices
  degree = (int *) malloc(sizeof(int) * (2 + *n));
  int * deg_min_heap; // To maintain vertices of minimum degree
  deg_min_heap = (int *) malloc(sizeof(int) * (2 + *n));
  int * pos_deg_min_heap; // Position in deg_min_heap
  pos_deg_min_heap = (int *) malloc(sizeof(int) * (2 + *n));
  
  if ((degree == NULL) || (deg_min_heap == NULL) || (pos_deg_min_heap == NULL)){
    printf("Unable to allocate space for degree/deg_min_heap/pos_deg_min_heap array \n");
    return(false);
  }

  for(int i=1; i <= *n; i++){
    degree[i] = (*ver)[i+1] - (*ver)[i];
    deg_min_heap[i] = i;
    pos_deg_min_heap[i] = i;
  }

  int index =  ceil(*n/2) + 1;
  while(index >= 1){
    _min_heapify_increase_at(&deg_min_heap, &degree, &pos_deg_min_heap, index, *n);
    index -= 1;
  }

  int degeneracy = 0;
  int xvertex, yvertex, zvertex, wvertex, tempvertex;

  for(int i = 1; i <= *n; i++){
    //printf("Iteration %d \n", i);
    //TEST
    //is_min_heap(n, deg_min_heap, degree);
    xvertex = deg_min_heap[1];
    deg_order[i] = xvertex;
    if (degeneracy < degree[xvertex]){
      degeneracy = degree[xvertex];
    }

    degree[xvertex] = 2 * (*n); //To ensure that this vertex is never considered. 
    _min_heapify_increase_at(&deg_min_heap, &degree, &pos_deg_min_heap, 1, *n); // Push the top element to the bottom

    
    int * modified_positions = (int *) malloc(sizeof(int) * (2 + *n));
    int size_modified_positions = 0;

    for(int j = (*ver)[xvertex]; j < (*ver)[xvertex + 1]; j++){     
      yvertex = (*edges)[j];
      if (degree[yvertex] > *n){
	//This vertex has been deleted.
	continue;
      }
      degree[yvertex] -= 1;
      //modify heap at the parent of vertex whose degree is affected
      modified_positions[size_modified_positions] =  pos_deg_min_heap[yvertex];
      size_modified_positions += 1;
    }

    qsort(modified_positions, size_modified_positions, sizeof(int), lesser1);
    // We sort with shortest element first as we are bubbling up the element in heap. For modification at i^th index we assume that there is a min heap from 1 to (i - 1).
    
    for(int j = 0; j < size_modified_positions; j++){
      //printf("%d ", modified_positions[j]);
      _min_heapify_decrease_at(&deg_min_heap, &degree, &pos_deg_min_heap, modified_positions[j], *n);
    }
    //printf("\n");

    free(modified_positions);
  }

  free(degree);
  free(deg_min_heap);
  free(pos_deg_min_heap);
  return degeneracy;
}


int degeneracy_order_black(int *n, int ** ver, int ** edges, int * color, int * deg_order){
  //Returns degeneracy of graph based on black neighbors
  //deg_order[i] = x implies that vertex x is at i^th place in degeneracy order
  // v1 : runs in O(m log(n)) time

  int * black_degree; // Store the black_degree of vertices
  black_degree = (int *) malloc(sizeof(int) * (2 + *n));
  int * deg_min_heap; // To maintain vertices of minimum black_degree
  deg_min_heap = (int *) malloc(sizeof(int) * (2 + *n));
  int * pos_deg_min_heap; // Position in deg_min_heap
  pos_deg_min_heap = (int *) malloc(sizeof(int) * (2 + *n));
  
  if ((black_degree == NULL) || (deg_min_heap == NULL) || (pos_deg_min_heap == NULL)){
    printf("Unable to allocate space for black_degree/deg_min_heap/pos_deg_min_heap array \n");
    return(false);
  }

  for(int i=1; i <= *n; i++){
    black_degree[i] = color[i];
    for(int j=(*ver)[i]; j < (*ver)[i + 1]; j++){
      black_degree[i] += color[(*edges)[j]];
    }
    deg_min_heap[i] = i;
    pos_deg_min_heap[i] = i;
  }

  int index =  ceil(*n/2) + 1;
  while(index >= 1){
    _min_heapify_increase_at(&deg_min_heap, &black_degree, &pos_deg_min_heap, index, *n);
    index -= 1;
  }

  int degeneracy = 0;
  int xvertex, yvertex, zvertex, wvertex, tempvertex;

  for(int i = 1; i <= *n; i++){
    //printf("Iteration %d \n", i);
    //TEST
    //is_min_heap(n, deg_min_heap, black_degree);
    xvertex = deg_min_heap[1];
    deg_order[i] = xvertex;
    if (degeneracy < black_degree[xvertex]){
      degeneracy = black_degree[xvertex];
    }

    black_degree[xvertex] = 2 * (*n); //To ensure that this vertex is never considered. 
    _min_heapify_increase_at(&deg_min_heap, &black_degree, &pos_deg_min_heap, 1, *n); // Push the top element to the bottom

    
    int * modified_positions = (int *) malloc(sizeof(int) * (2 + *n));
    int size_modified_positions = 0;

    for(int j = (*ver)[xvertex]; j < (*ver)[xvertex + 1]; j++){     
      yvertex = (*edges)[j];
      if (black_degree[yvertex] > *n){
	//This vertex has been deleted.
	continue;
      }
      black_degree[yvertex] -= 1;
      //modify heap at the parent of vertex whose black_degree is affected
      modified_positions[size_modified_positions] =  pos_deg_min_heap[yvertex];
      size_modified_positions += 1;
    }

    qsort(modified_positions, size_modified_positions, sizeof(int), lesser1);
    // We sort with shortest element first as we are bubbling up the element in heap. For modification at i^th index we assume that there is a min heap from 1 to (i - 1).
    
    for(int j = 0; j < size_modified_positions; j++){
      //printf("%d ", modified_positions[j]);
      _min_heapify_decrease_at(&deg_min_heap, &black_degree, &pos_deg_min_heap, modified_positions[j], *n);
    }
    //printf("\n");

    free(modified_positions);
  }

  free(black_degree);
  free(deg_min_heap);
  free(pos_deg_min_heap);
  return degeneracy;
}
