//Contains an approximation algorithm to compute dominating set in sparse graph.
int approx_dom_set(int *n, int ** ver, int ** edges, int * color, int * solution);
int approx_dom_set_v1(int *n, int ** ver, int ** edges, int * color, int * solution);

int approx_dom_set(int *n, int ** ver, int ** edges, int * color, int * solution){
  // Approximation algorithm for Dominating Set on d-degenerate graphs.
  // It modifies color and solution array.
  // Check Parameterized Complexity of Directed Steiner Tree on Sparse Graphs by Jones et al.
  // Definition of sets.
  // Y -- Vertices included in solution
  // B -- Already dominated vertices outside Y. Vertices which has color white and/or neighbors of Y.
  // W -- Yet to be dominated vertices.
  // Bh (Denoted by "A") -- Vertices in B which are adjacent with at least d + 1 neighbors in W (in Wh or Wl).
  // Bl (Denoted by "C")-- Vertices in B \setminus Bh
  // Wh (Denoted by "V")-- Vertices which are adjacent with some vertex in Bh \cup W
  // Wl (Denoted by "X")-- Vertices in W \setminus Wh
  
  int degeneracy = 0;
  int * deg_order;
  int temp_vertex;
  deg_order = (int *) malloc(sizeof(int) * (2 + *n));  
  degeneracy = degeneracy_order(n, ver, edges, deg_order);

  char * partition; //To store partition of vertices
  partition = (char *) malloc(sizeof(char) * (2 + *n));

  for(int i=1; i <= *n; i++){
    if (solution[i] == 1){
      partition[i] = 'Y';
      continue;
    }
    
    if (color[i] == 0){
      //If color is white then vertex is in set B
      partition[i] = 'B';
    }else{
      partition[i] = 'W';
    }
  }

  int * deg_W; // size of neighborhood of vertices in W
  // for vertices not in B, it is defined as 2 * (*n)
  deg_W = (int *) malloc(sizeof(int) * (2 + *n));

  // Partitioning B into Bh (A) or Bl (C) 
  for (int i=1; i <= *n; i++){
    if (partition[i] != 'B'){
      deg_W[i] = 2 * (*n);
      continue;
    }
    deg_W[i] = 0;
    for(int j = (*ver)[i]; j < (*ver)[i + 1]; j++){
      temp_vertex = (*edges)[j];
      if (partition[temp_vertex] == 'W'){
	deg_W[i] += 1;
      }
    }
    if (deg_W[i] >= degeneracy + 1){
      partition[i] = 'A';
    }else{
      partition[i] = 'C';
    }
  }

  //Partition W into Wh(V) or Wl(X)
  for(int i=1; i <= *n; i++){
    if (partition[i] != 'W'){
      continue;
    }

    for(int j=(*ver)[i]; j < (*ver)[i + 1]; j++){
      temp_vertex = (*edges)[j];
      if ((partition[temp_vertex] == 'A') || (partition[temp_vertex] == 'W')){
	partition[i] = 'V';
	break;
      }
    }
  }
  //Marking vertices in W which are not marked Wh(V)
  for(int i=1; i <= *n; i++){
    if (partition[i] == 'W'){
      partition[i] = 'X';
    }
  }
  
  
  int * deg_BhWh; // size of neighborhood of vertex in Bh+Wh (or A+V. For vertices which are not in Wh (labeled V), we assign it to be 2 * (*n)
  deg_BhWh = (int *) malloc(sizeof(int) * (2 + *n));

  for(int i=1; i <= *n; i++){
    if (partition[i] != 'V'){
      deg_BhWh[i] = 2 * (*n);
      continue;
    }
    deg_BhWh[i] = 0;
    for(int j = (*ver)[i]; j < (*ver)[i + 1]; j++){
      if((partition[(*edges)[j]] == 'A') || (partition[(*edges)[j]] == 'V')){
	deg_BhWh[i] += 1;
      }
    }
  }
  
  bool is_Wh_empty = true;
  for(int i=1; i <= *n; i++){
    if (partition[i] == 'V'){
      is_Wh_empty = false;
      break;
    }
  }

  int vvertex; // vertex with minimum degree in BhWh
  int min_deg_BhWh;
  while(is_Wh_empty != true){
    vvertex = 1;
    min_deg_BhWh = 2 * (*n);
    for(int i=1; i <= *n; i++){
      if ((partition[i] == 'V') && (deg_BhWh[i] < min_deg_BhWh)){
	vvertex = i;
	min_deg_BhWh = deg_BhWh[i];
      }
    }
    
    //printf("vvertex \t %d \n", vvertex);

    for(int i = (*ver)[vvertex]; i < (*ver)[vvertex + 1]; i++){
      temp_vertex = (*edges)[i];
      if ((partition[temp_vertex] == 'C') || (partition[temp_vertex] == 'X')){
	continue;
      }
      // only add vertices which are Wh(V) or Bh(A)
      partition[temp_vertex] = 'Y';
      color[temp_vertex] = 0;
      for(int j=(*ver)[temp_vertex]; j < (*ver)[temp_vertex + 1]; j++){
	color[(*edges)[j]] = 0;
      }
    }

    //Repartition all vertices which are not in Y
    for(int i=1; i <= *n; i++){
      if (partition[i] == 'Y'){
	continue;
      }
      
      if (color[i] == 0){
	//If color is white then vertex is in set B
	partition[i] = 'B';
      }else{
	partition[i] = 'W';
      }
    }

    //Recalculating deg_W and Wh and Wl
    for (int i=1; i <= *n; i++){
      if (partition[i] != 'B'){
	deg_W[i] = 2 * (*n);
	continue;
      }
      deg_W[i] = 0;
      for(int j = (*ver)[i]; j < (*ver)[i + 1]; j++){
	temp_vertex = (*edges)[j];
	if (partition[temp_vertex] == 'W'){
	  deg_W[i] += 1;
	}
      }
      if (deg_W[i] >= degeneracy + 1){
	partition[i] = 'A';
      }else{
	partition[i] = 'C';
      }
    }

    //Partition W into Wh(V) or Wl(X)
    for(int i=1; i <= *n; i++){
      if (partition[i] != 'W'){
	continue;
      }

      for(int j=(*ver)[i]; j < (*ver)[i + 1]; j++){
	temp_vertex = (*edges)[j];
	if ((partition[temp_vertex] == 'A') || (partition[temp_vertex] == 'W')){
	  partition[i] = 'V';
	  break;
	}
      }
    }
    //Marking vertices in W which are not marked Wh(V)
    for(int i=1; i <= *n; i++){
      if (partition[i] == 'W'){
	partition[i] = 'X';
      }
    }
    
    //recalculating deg_BhWh for next computation
    for(int i=1; i <= *n; i++){
      if (partition[i] != 'V'){
	deg_BhWh[i] = 2 * (*n);
	continue;
      }
      deg_BhWh[i] = 0;
      for(int j = 1; j <= *n; j++){
	if((partition[i] == 'A') || (partition[i] == 'V')){
	  deg_BhWh[i] += 1;
	}
      }
    }
     
    is_Wh_empty = true;
    for(int i=1; i <= *n; i++){
      if (partition[i] == 'V'){
	is_Wh_empty = false;
	break;
      }
    }
  }

  for(int i=1; i <= *n; i++){
    if ((partition[i] == 'Y') || (partition[i] == 'X')){
      solution[i] = 1;
    }
  }
  return degeneracy;
}


int approx_dom_set_v1(int *n, int ** ver, int ** edges, int * color, int * solution){
  // Approximation algorithm for Dominating Set on d-degenerate graphs.
  // It modifies color and solution array.
  // Check Parameterized Complexity of Dinrected Steiner Tree on Sparse Graphs by Jones et al.
  // Definition of sets.
  // v1 : runs in time O(m log n)
  // Definition of sets.
  // Y -- Vertices included in solution
  // B -- Already dominated vertices outside Y. Vertices which has color white and/or neighbors of Y.
  // W -- Yet to be dominated vertices.
  // Bh (Denoted by "A") -- Vertices in B which are adjacent with at least d + 1 neighbors in W (in Wh or Wl).
  // Bl (Denoted by "C")-- Vertices in B \setminus Bh
  // Wh (Denoted by "V")-- Vertices which are adjacent with some vertex in Bh \cup W
  // Wl (Denoted by "X")-- Vertices in W \setminus Wh
 
  int degeneracy = 0;
  int * deg_order;// To store deg_order. Not really used but it is required pareameter.
  deg_order = (int *) malloc(sizeof(int) * (2 + *n));  
  char * partition; //To store partition of vertices
  partition = (char *) malloc(sizeof(char) * (2 + *n));
  int * deg_W; // size of neighborhood of vertices in W
  // for vertices not in B, it is defined as 2 * (*n)
  deg_W = (int *) malloc(sizeof(int) * (2 + *n));
  int * deg_BhWh; // size of neighborhood of vertex in Bh+Wh (or A+V. For vertices which are not in Wh (labeled V), we assign it to be 2 * (*n)
  deg_BhWh = (int *) malloc(sizeof(int) * (2 + *n));
  int * deg_BhWh_heap_min; // To store vertices according to their deg_BhWh
  deg_BhWh_heap_min  = (int *) malloc(sizeof(int) * (2 + *n));
  int * pos_deg_BhWh_heap_min; // To store position of vertices in heap_min_deg_BhWh
  pos_deg_BhWh_heap_min = (int *) malloc(sizeof(int) * (2 + *n));
  
  int temp_vertex;

  if ((deg_order == NULL) || (partition == NULL) || (deg_W == NULL) || (deg_BhWh == NULL) || (pos_deg_BhWh_heap_min == NULL) || (deg_BhWh_heap_min == NULL)){
    printf("No space for deg_order/partition/deg_w/deg_BhWh \n");
  }

  //initializing all arrays
  for (int i=1; i <= *n; i++){
    deg_order[i] = i;
    deg_W[i] = 0;
    deg_BhWh[i] = 0;
    deg_BhWh_heap_min[i] = i;
    pos_deg_BhWh_heap_min[i] = i;
    partition[i] = 'W';
  }
  
  degeneracy = degeneracy_order_v1(n, ver, edges, deg_order);
  //degeneracy = degeneracy_order_black(n, ver, edges, color, deg_order);

  for(int i=1; i <= *n; i++){
    if (solution[i] == 1){
      partition[i] = 'Y';
      continue;
    }
    
    if (color[i] == 0){
      //If color is white then vertex is in set B
      partition[i] = 'B';
    }else{
      partition[i] = 'W';
    }
  }

  // Computing deg_W[u] for every vertex.
  // Used to partition B into Bh (A) or Bl (C)
  // Also important for vertices in Wh if they move to Bh
  for (int i=1; i <= *n; i++){
    deg_W[i] = 0;
    for(int j = (*ver)[i]; j < (*ver)[i + 1]; j++){
      temp_vertex = (*edges)[j];
      if (partition[temp_vertex] == 'W'){
	deg_W[i] += 1;
      }
    }

    if (partition[i] != 'B'){
      continue;
    }
    
    if (deg_W[i] >= degeneracy + 1){
      partition[i] = 'A';
    }else{
      partition[i] = 'C';
    }
  }

  bool is_colored_V;
  //Partition W into Wh(V) or Wl(X)
  for(int i=1; i <= *n; i++){
    if (partition[i] != 'W'){
      continue;
    }

    is_colored_V = false;
    for(int j=(*ver)[i]; j < (*ver)[i + 1]; j++){
      temp_vertex = (*edges)[j];
      if ((partition[temp_vertex] != 'C')){
	partition[i] = 'V';
	is_colored_V = true;
	break;
      }
    }

    //Marking vertices in W which are not marked Wh(V)
    if (is_colored_V == false){
      partition[i] = 'X';
    }
  }
  
  // Initializingt deg_BhWh
  // size of neighborhood of vertex in Bh+Wh (or A+V. For vertices which are not in Wh (labeled V), we assign it to be 2 * (*n)
  for(int i=1; i <= *n; i++){
    if (partition[i] != 'V'){
      deg_BhWh[i] = 2 * (*n);
      continue;
    }
    deg_BhWh[i] = 0;
    for(int j = (*ver)[i]; j < (*ver)[i + 1]; j++){
      temp_vertex = (*edges)[j];
      if((partition[temp_vertex] == 'A') || (partition[temp_vertex] == 'V')){
	deg_BhWh[i] += 1;
      }
    }
  }

  // heapify deg_BhWh_heap_min
  int index =  ceil(*n/2) + 1;
  while(index >= 1){
    _min_heapify_increase_at(&deg_BhWh_heap_min, &deg_BhWh, &pos_deg_BhWh_heap_min, index, *n);
    index -= 1;
  }
  
  int vvertex, uvertex, alpha_vertex, beta_vertex, gamma_vertex; 
  int num_vertices_Wh = 0; // Vertices in set Wh(V)
  for(int i=1; i <= *n; i++){
    if (partition[i] == 'V'){
      num_vertices_Wh += 1;
    }
  }


  while (num_vertices_Wh > 0){
    //printf("Num verteices in Wh %d \n", num_vertices_Wh);
    vvertex = deg_BhWh_heap_min[1];
    //printf("vvertex, partition  %d  %c\n", vvertex, partition[vvertex]);
    //printf("vvertex deg %d \n", deg_BhWh[vvertex]);
    if (deg_BhWh[vvertex] >= *n){
      // We have exhausted all vertices.
      break;
    }
    
    if (deg_BhWh[vvertex] <= 0){
      // In this case, uvertex is moved from Wh (V) to Wl (X)
      partition[vvertex] = 'X';
      num_vertices_Wh -= 1;
      deg_BhWh[vvertex] = 2 * (*n);
      _min_heapify_increase_at(&deg_BhWh_heap_min, &deg_BhWh, &pos_deg_BhWh_heap_min, pos_deg_BhWh_heap_min[vvertex], *n);
      continue;
    }
    
    // move all neighbors of vvertex which are in Bh(A) or Wh(V) to Y
    for(int i = (*ver)[vvertex]; i < (*ver)[vvertex + 1]; i++){
      uvertex = (*edges)[i];
      //printf("uvertex %d %c \n", uvertex, partition[uvertex]);
      
      if ((partition[uvertex] == 'C') || (partition[uvertex] == 'Y') || (partition[uvertex] == 'X')){
	// if uvertex is in Bl(C) or Y then we don't move it. It can not be in Wl (X) 
	continue;
      }      
      // We are in case when partition[uvertex] = Wh(V) or Bh(A)
      if (partition[uvertex] == 'V'){
	// In this case, uvertex is moved out of Wh(V)
	num_vertices_Wh -= 1;
      }
      partition[uvertex] = 'Y';
      deg_BhWh[uvertex] = 2 * (*n);
      _min_heapify_increase_at(&deg_BhWh_heap_min, &deg_BhWh, &pos_deg_BhWh_heap_min, pos_deg_BhWh_heap_min[uvertex], *n);
      
      for(int j=(*ver)[uvertex]; j < (*ver)[uvertex + 1]; j++){
	alpha_vertex = (*edges)[j];
	if ((partition[alpha_vertex] == 'Y') || (partition[alpha_vertex] == 'A') || (partition[alpha_vertex] == 'C') ){
	  continue;
	}
	// since partition of [alpha_vertex] can not be equal to X, we are in case when it is V.
	// Since uvertex (nbr of alpha_vertex) is moved to Y, alpha_vertex is also moved to B
	// Hence we don't need deg_BhWh[alpha_vertex] anymore.
	deg_BhWh[alpha_vertex] = 2 * (*n);
	_min_heapify_increase_at(&deg_BhWh_heap_min, &deg_BhWh, &pos_deg_BhWh_heap_min, pos_deg_BhWh_heap_min[alpha_vertex], *n);
	// depending on deg_W[alpha_vertex] it will either be moved to Bh (A) or Bl (C)
	num_vertices_Wh -= 1; // as alpha_vertex moved from Wh(V)  
	// Case when deg_W[alpha_vertex] >= degeneracy + 1 starts
	if (deg_W[alpha_vertex] >= degeneracy + 1){
	  // in this case, alpha_vertex is moved to Bh
	  partition[alpha_vertex] = 'A';
	  for (int k = (*ver)[alpha_vertex]; k < (*ver)[alpha_vertex + 1]; k++){
	    beta_vertex = (*edges)[k];
	    // We only need to consider the cases when bata_vertex is in Bh(A) or Wh(V).
	    if (partition[beta_vertex] == 'A'){
	      deg_W[beta_vertex] -= 1;
	      if (deg_W[beta_vertex] < degeneracy + 1){
		// Move beta_vertex from Wh(A) to Wl(C).
		partition[beta_vertex] = 'C';
		// This might affect neighbors of beta_vertex in Wh
		for(int l = (*ver)[beta_vertex]; l < (*ver)[beta_vertex + 1]; l++){
		  gamma_vertex = (*edges)[l];
		  if (partition[gamma_vertex] != 'V'){
		    continue;
		  }
		  // We are in case when partition[gamma_vertex] == 'V'
		  deg_BhWh[gamma_vertex] -= 1;
		  // Since the value is decrased, we min_heapify at this point.
		  _min_heapify_decrease_at(&deg_BhWh_heap_min, &deg_BhWh, &pos_deg_BhWh_heap_min, pos_deg_BhWh_heap_min[gamma_vertex], *n);		  
		}
	      }
	    }
	    if (partition[beta_vertex] == 'V'){
	      deg_W[beta_vertex] -= 1;
	    }  
	  }
	  continue; 
	}
	// Case when deg_W[alpha_vertex] >= degeneracy + 1 ends
	// Case when deg_W[alpha_vertex] < degeneracy + 1 starts
	partition[alpha_vertex] = 'C'; // alpha_vertex is moved from Wh to Bl
	for (int k = (*ver)[alpha_vertex]; k < (*ver)[alpha_vertex + 1]; k++){
	  beta_vertex = (*edges)[k];
	  // We only need to consider cases when beta_vertex is in Bh(A) or Wh(V)
	  if (partition[beta_vertex] == 'A'){
	    deg_W[beta_vertex] -= 1;
	    if (deg_W[beta_vertex] < degeneracy + 1){
	      // Move beta_vertex from Bh(A) to Bl(C).
	      partition[beta_vertex] = 'C';
	      // This might affect neighbors of beta_vertex in Wh
	      for(int l = (*ver)[beta_vertex]; l < (*ver)[beta_vertex + 1]; l++){
		gamma_vertex = (*edges)[l];
		if (partition[gamma_vertex] != 'V'){
		  continue;
		}
		// We are in case when partition[gamma_vertex] == 'V'
		deg_BhWh[gamma_vertex] -= 1;
		// Since the value is decrased, we min_heapify at this point.
		_min_heapify_decrease_at(&deg_BhWh_heap_min, &deg_BhWh, &pos_deg_BhWh_heap_min, pos_deg_BhWh_heap_min[gamma_vertex], *n);	  
	      }
	    }
	  }
	  if (partition[beta_vertex] == 'V'){
	    deg_W[beta_vertex] -= 1;
	    deg_BhWh[beta_vertex] -= 1;
	    // if deg_BhWh is only decreased by 1, we modify heap
	    _min_heapify_decrease_at(&deg_BhWh_heap_min, &deg_BhWh, &pos_deg_BhWh_heap_min, pos_deg_BhWh_heap_min[beta_vertex], *n);
	  }
	}
	// Case when deg_W[alpha_vertex] < degeneracy + 1 ends
      }
    }
  }
    
  for(int i=1; i <= *n; i++){
    // Include vertices in set Y (Y) or Wl(X) in solution
    if ((partition[i] == 'Y') || (partition[i] == 'X')){
      solution[i] = 1;
    }
  }

  int vertices_in_X = 0;
  int vertices_in_C = 0;
  for(int i=1; i<= *n; i++){
    if (partition[i] == 'X'){
      vertices_in_X += 1;
    }

    if (partition[i] == 'C'){
      vertices_in_C += 1;
    }
  }

  //printf("%d \t %d \t", vertices_in_X, vertices_in_C);
    
  free(deg_order);
  free(partition);
  free(deg_W);
  free(deg_BhWh);
  free(deg_BhWh_heap_min);
  free(pos_deg_BhWh_heap_min);
}
