// Contains various experiments

int experiment_00(int * n, int ** ver, int ** edges){
  // To test some function

  int * color; //To store color of vertices. color[v] = 1 (black); or 0 (white)
  color = (int *) malloc(sizeof(int)*(2 + (*n)));
  int * solution;// To store a solution. If solution[v] == 1 then it is included in the solution
  solution = (int *) malloc(sizeof(int)*(2 + (*n)));
  int * org_color;// Used to varify solution
  org_color = (int *) malloc(sizeof(int)*(2 + (*n)));
  int * deg_order; 
  deg_order = (int *) malloc(sizeof(int)*(2 + (*n)));
  int * degree; // Store degree of vertices
  degree = (int *) malloc(sizeof(int)*(2 + *n));
  int * cls_nbr_black; // To store number vertices with black color in closed neighbhorhood of vertex
  cls_nbr_black = (int *) malloc(sizeof(int)*(2+ *n));  
  
  
  if ((color == NULL) || (solution == NULL) || (org_color == NULL) || (deg_order == NULL)){
    printf("Unable to allocate space for color/solution/org_color/deg_order array \n");
    return(false);
  }

  //Initialize
  for (int i=1; i <= (*n); i++){
    color[i] = 1; // Initialzing color to black
    solution[i] = 0; // Initializing solution to an empty set
    org_color[i] = 1;
    cls_nbr_black[i] = 0;
  }

  rr_deg0(n, ver, edges, color, solution);
  rr_deg1(n, ver, edges, color, solution);

  for (int i=1; i <= (*n); i++){
    cls_nbr_black[i] = color[i]; //If vertex itself is black then it will be counted as one
    for(int j= (*ver)[i]; j < (*ver)[i+1]; j++){
      if (color[(*edges)[j]] == 1){
	cls_nbr_black[i] += 1;
      }
    }
  }
  
  //approx_dom_set(n, ver, edges, color, solution);
  //greedy_DS_v1(n, ver, edges, color, solution);
  //int vertices_in_solution = 0;

  //for(int i=1; i <= (*n); i++){
  //  vertices_in_solution += solution[i];
  //}
  
  //printf("Vertices in solution %d \n", vertices_in_solution);

  //printf("Is dominating set %d \n", is_dominating_set(n, ver, edges, org_color, solution));

  //Initialize
  for (int i=1; i <= (*n); i++){
    color[i] = 1; // Initialzing color to black
    solution[i] = 0; // Initializing solution to an empty set
    org_color[i] = 1;
  }

  rr_deg0(n, ver, edges, color, solution);
  rr_deg1(n, ver, edges, color, solution);

  for(int i = 1; i <= *n; i++){ 
    cls_nbr_black[i] = color[i]; //If vertex itself is black then it will be counted as one
    for(int j = (*ver)[i]; j < (*ver)[i+1]; j++){
      if (color[(*edges)[j]] == 1){
	cls_nbr_black[i] += 1;
      }
    }
  }   
  
  
  //for (int i=1; i <= (*n); i++){
  //  printf("%d \n", cls_nbr_black[i]);
  //}
  //approx_dom_set_v1(n, ver, edges, color, solution);    
  //vertices_in_solution = 0;
  //int degeneracy = degeneracy_order_v1(n, ver, edges, deg_order);
  
  
  //for(int i=1; i <= (*n); i++){
  //  vertices_in_solution += solution[i];
  //}

  //printf("Vertices in solution %d \n", vertices_in_solution);
  //printf("%d \t", degeneracy);
  // if (is_dominating_set(n, ver, edges, org_color, solution) == false){
  //  return -1;
  //}else{
  //  return vertices_in_solution;
  //}
  return 1;
}


int experiment_01(int * n, int ** ver, int ** edges){
  // Computes solution using approximation algorithm
  
  int * color; //To store color of vertices. color[v] = 1 (black); or 0 (white)
  color = (int *) malloc(sizeof(int)*(2 + (*n)));
  int * solution;// To store a solution. If solution[v] == 1 then it is included in the solution
  solution = (int *) malloc(sizeof(int)*(2 + (*n)));
  int * org_color;// Used to varify solution
  org_color = (int *) malloc(sizeof(int)*(2 + (*n)));
  
  if ((color == NULL) || (solution == NULL) || (org_color == NULL)){
    printf("Unable to allocate space for color/solution/org_color array \n");
    return(false);
  }
  
  //Initialize
  for (int i=1; i <= (*n); i++){
    color[i] = 1; // Initialzing color to black
    solution[i] = 0; // Initializing solution to an empty set
    org_color[i] = 1;
  }

  rr_deg0(n, ver, edges, color, solution);
  rr_deg1(n, ver, edges, color, solution);
  approx_dom_set_v1(n, ver, edges, color, solution);
    
  int vertices_in_solution = 0;

  for(int i=1; i <= (*n); i++){
    vertices_in_solution += solution[i];
  }
  
  //printf("Is dominating set %d \n", is_dominating_set(n, ver, edges, org_color, solution));

  free(solution);
  free(color);
  free(org_color);
  return vertices_in_solution;
}


int experiment_02(int * n, int ** ver, int ** edges){
  //Computes lossy kernel using rr_lossy_greedy and then uses approximation algorithm
 
  int * color; //To store color of vertices. color[v] = 1 (black); or 0 (white)
  color = (int *) malloc(sizeof(int)*(2 + (*n)));
  int * solution;// To store a solution. If solution[v] == 1 then it is included in the solution
  solution = (int *) malloc(sizeof(int)*(2 + (*n)));
  int * org_color;// Used to varify solution
  org_color = (int *) malloc(sizeof(int)*(2 + (*n)));
  int lossy_accuracy = 1;
  int * overload; // Overload over every vertex.
  overload = (int *) malloc(sizeof(int)*(2 + (*n)));
  
  if ((color == NULL) || (solution == NULL) || (org_color == NULL) || (overload == NULL)){
    printf("Unable to allocate space for color/solution/org_color array \n");
    return(false);
  }
  
  //Initialize
  for (int i=1; i <= (*n); i++){
    color[i] = 1; // Initializing color to black
    solution[i] = 0; // Initializing solution to an empty set
    org_color[i] = 1;
    overload[i] = lossy_accuracy;
  }

  rr_deg0(n, ver, edges, color, solution);
  rr_deg1(n, ver, edges, color, solution);
  rr_lossy_greedy(n, ver, edges, color, solution, overload);
  approx_dom_set_v1(n, ver, edges, color, solution);
    
  int vertices_in_solution = 0;

  for(int i=1; i <= (*n); i++){
    vertices_in_solution += solution[i];
  }
  
  //printf("Is dominating set %d \n", is_dominating_set(n, ver, edges, org_color, solution));
  free(solution);
  free(color);
  free(org_color);
  free(overload);
  return vertices_in_solution;  
}


int experiment_03(int * n, int ** ver, int ** edges){
  // Check the number of vertices which are turned from black (yet to dominate) to black (dominated) because of lossy reduction rule. 
  int * color; //To store color of vertices. color[v] = 1 (black); or 0 (white)
  color = (int *) malloc(sizeof(int)*(2 + (*n)));
  int * solution;// To store a solution. If solution[v] == 1 then it is included in the solution
  solution = (int *) malloc(sizeof(int)*(2 + (*n)));
  int * org_color;// Used to varify solution
  org_color = (int *) malloc(sizeof(int)*(2 + (*n)));
  int lossy_accuracy = 1;
  int * overload; // Overload over every vertex.
  overload = (int *) malloc(sizeof(int)*(2 + (*n)));
  
  if ((color == NULL) || (solution == NULL) || (org_color == NULL) || (overload == NULL)){
    printf("Unable to allocate space for color/solution/org_color array \n");
    return(false);
  }
  
  //Initialize
  for (int i=1; i <= (*n); i++){
    color[i] = 1; // Initializing color to black
    solution[i] = 0; // Initializing solution to an empty set
    org_color[i] = 1;
    overload[i] = lossy_accuracy;
  }

  rr_deg0(n, ver, edges, color, solution);
  rr_deg1(n, ver, edges, color, solution);
  int black_vertices = 0;
  int white_vertices = 0;

  for (int i=1; i <= (*n); i++){
    if (color[i] == 1){
      black_vertices += 1;
    }

    if (color[i] == 0){
      white_vertices += 1;
    }
  }

  printf("Black-White Vertices \t %d \t %d \t %d \t", (*n), black_vertices, white_vertices);
  
  rr_lossy_greedy(n, ver, edges, color, solution, overload);

  black_vertices = 0;
  white_vertices = 0;

  
  for (int i=1; i <= (*n); i++){
    if (color[i] == 1){
      black_vertices += 1;
    }

    if (color[i] == 0){
      white_vertices += 1;
    }
  }

  printf("%d \t %d \n", black_vertices, white_vertices);

  //approx_dom_set_v1(n, ver, edges, color, solution);
    
  //int vertices_in_solution = 0;

  //for(int i=1; i <= (*n); i++){
  //  vertices_in_solution += solution[i];
  //}
  
  //printf("Is dominating set %d \n", is_dominating_set(n, ver, edges, org_color, solution));
  free(solution);
  free(color);
  free(org_color);
  free(overload);
  return 1;
  //return vertices_in_solution;  
}
