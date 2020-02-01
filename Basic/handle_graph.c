// Contains function to read graphs and allocate memeory.

int get_input(int argc, char * argv[], int * n_graphs, int *n_runs, int *n_conf, int ** conf, char *** name);
int allocate_memory(int size, int ** n, int ** m);
int allocate_graph_memory(edge **e, int **ver, int **edges, int n);
int read_graph(int *n, int *m, char *f_name, int **ver, int **edges, edge **e);


int read_graph(int *n, int *m, char *f_name, int **ver, int **edges, edge **e){

  FILE *fp;
  long int i;
  int x,y,z;
  double v,w;
  MM_typecode matcode;
  int cols;

  int mxdeg = 0;
  int *count;
  int *start;

  // Make sure graph is of the right type

  fp = fopen(f_name,"r");
  if (fp == NULL) {
    printf("Could not open file %s \n",f_name);
    return(false);
  }

  if (mm_read_banner(fp, &matcode) != 0) {
    printf("Could not process Matrix Market banner.\n");
    return(false);
  }

  if ((mm_read_mtx_crd_size(fp, &cols, n, m)) !=0) {
    printf("Could not read size of graph.\n");
    return(false);
  }

  if (*m > max_m) {
    printf("Graph is too large. Asking for %d edges, max is %d \n",*m,max_m);
    return(false);
  }
  if (*n > max_n) {
    printf("Graph is too large. Asking for %d vertices, max is %d \n",*n,max_n);
    return(false);
  }

  if (!mm_is_matrix(matcode) || !mm_is_coordinate(matcode) || !mm_is_sparse(matcode) || !mm_is_symmetric(matcode)) {
    printf("The program can only read files that are sparse symmmetric matrices in coordinate format! \n");
    return(false);
  }

  count = (int *) malloc(sizeof(int)* (*n + 2));
  if (count == NULL){
    printf("Unable to allocate space for count in read_graph() \n");
    return(false);
  }

  start = (int *) malloc(sizeof(int)*(*n + 2));
  if (start == NULL){
    printf("Unable to allocate space for start in read_graph() \n");
    return(false);
  }
  
  // Start counting of degrees by setting counters to zero
  for(i=1;i<=*n;i++) {
    count[i] = 0;
  }

  int num_edges = 0;

  // printf("Number of possible edges is %d \n",*m);

  // Read inn the edges
  if (mm_is_real(matcode)) {
    printf("Real matrix, Starting to read %d edges \n", *m);
    srand48(time(NULL));
    for(i=0; i<*m; i++) {
      fscanf(fp,"%d %d %lf", &x, &y, &v); // Use this line if there is exactly one double weight 
      //  fscanf(fp,"%d %d %lf %lf",&x,&y,&v,&w); // Use this line for complex weights
      if (x != y) {
	// Avoid self-edges	
	(*e)[num_edges].x = x; // Store edges
	(*e)[num_edges].y = y;
	
        count[x]++; // Get vertex degrees
        count[y]++;
	
        num_edges++;

	if (num_edges > *m) {
          printf("Have set num_edges to %d while i=%ld \n",num_edges,i);
          return(false);
        }
      }
    }
  }
  else if (mm_is_integer(matcode)) {
    printf("Integer matrix, Starting to read %d edges \n",*m);
    srand48(time(NULL));
    for(i=0; i<*m; i++) {
      fscanf(fp,"%d %d %lf",&x,&y,&v); // Use this line if there is exactly one double weight 
    
      if (x != y) {
	// Avoid self-edges
	(*e)[num_edges].x = x; // Store edges
	(*e)[num_edges].y = y;
	
        count[x]++; // Get vertex degrees
        count[y]++;

        num_edges++;
        if (num_edges > *m) {
          printf("Have set num_edges to %d while i=%ld \n", num_edges, i);
          return(false);
        }
      }
    }
  }
  else
    { // Symbolic matrix
      srand48(time(NULL));
      printf("Symbolic matrix \n");
      for(i=0; i<*m; i++) {
	fscanf(fp,"%d %d",&x,&y);
	if (x != y) {
	  // Avoid self-edges
	  (*e)[num_edges].x = x; // Store edges
	  (*e)[num_edges].y = y;
	  
	  count[x]++; // Get vertex degrees
	  count[y]++;

	  num_edges++;
	}
      }
    }
  //   printf("original edges %d, used edges %d \n",*m,num_edges);
  printf("*  |V| = %d, |E| = %d \n",*n,num_edges);
  printf("************************************************\n");
  

  *m = num_edges;  // Make sure m is the correct number of edges

  // Find starting positions in edge list for each vertex
  start[1] = 0;
  (*ver)[1] = 0;
  for(i=2;i<=*n+1;i++) {
    start[i] = start[i-1] + count[i-1];
    (*ver)[i] = start[i];
  }

  // Place edges in edge lists, once for each endpoint

  for(i=0; i<*m; i++) {
    x = (*e)[i].x;
    y = (*e)[i].y;
    if ((x == 0) || (y == 0)) {
      printf("edge %ld: %d and %d are neighbors, numbering starts from 1! \n", i, x, y);
      return(false);
    }

    (*edges)[start[x]] = y;
    start[x]++;

    (*edges)[start[y]] = x;
    start[y]++;
  }

  // printf("Done read_graph, freeing up memory \n");
  fclose(fp);
  free(count);
  free(start);
  return(true);
}


// Allocating memory specifically for this graph
int allocate_graph_memory(edge **e, int **ver, int **edges, int n) {

  //int allocate_graph_memory(neig **swe,double **weight,edge **e,wed **we,int **ver,int **edges,int n,int **next,int **used,int **p,int **p1,int **p2,int **p3) 

  *e = (edge *) malloc(sizeof(edge)*(max_m));
   if (*e == NULL) {
   printf("Unable to allocate space for e-array in allocate_graph_memory() \n");
   return(false);
   }
  
  *ver = (int *) malloc(sizeof(int)*(2+n));
  if (*ver == NULL) {
    printf("Unable to allocate space for vertex-array in allocate_graph_memory() \n");
    return(false);
  }

  *edges = (int *) malloc(2*sizeof(int)*(max_m));
  if (*edges == NULL) {
    printf("Unable to allocate space for edges-array in allocate_graph_memory \n");
    return(false);
  }

  return(true);
}


int allocate_memory(int size, int **n, int **m){

  *n = (int *) malloc(sizeof(int) * size);  // List holding the number of vertices in each graph

  if (*n == NULL) {
    printf("Unable to allocate memory for n[] in allocate_memory() \n");
    return(false);
  }

  *m = (int *) malloc(sizeof(int)* size);  // List holding the number of edges in each graph
  
  if (*m == NULL) {
    printf("Unable to allocate memory for m[] in allocate_memory() \n");
    return(false);
  }

  return(true);
}


int get_input(int argc, char *argv[], int *n_graphs, int *n_runs, int *n_conf, int **conf, char ***name) {
  // Get input. This is given as a file name to the executable. 
  // This file should contain the following information (per line):
  // Number of input graphs
  // Number of times each configuration is run, the program reports the best running time out of these
  // Number of configurations, followed by the number of threads in each configuration
  // One line giving the complete name of each graph file that is to be run

  FILE *rf;	// File pointer
  int i;

  if (argc != 2) {
    printf("Give data file name as first input parameter!\n");
    return(false);
  }

  printf("************************************************\n");
  printf("*       Reading setup from %10s          *\n",argv[1]);
  printf("************************************************\n");

  //Opening file containing file names for graphs

  rf = fopen(argv[1],"r");
  if (rf == NULL) {
    printf("Cannot open data file: %s \n",argv[1]);
    return(false);
  }


  //Get number of graphs to read
  fscanf(rf,"%d", n_graphs);

  *name = (char **) malloc(sizeof(char *)*(*n_graphs));  // Allocate one pointer for each graph 

  if (*name == NULL) {
    printf("Unable to allocate space for names of %d graphs\n", max_graphs);
    return(0);
  }

  // Get number of runs per configuration
  fscanf(rf, "%d", n_runs);

  // Get number of thread configurations
  fscanf(rf, "%d", n_conf);
  *conf = (int *) malloc(sizeof(int)*(*n_conf));  // Allocate space for configurations

  if (*conf == NULL) {
    printf("Unable to allocate memory for %ls different thread configurations \n",n_conf);
    return(0);
  }

  // Get the different configurations
  for(i=0; i< *n_conf; i++) {
    fscanf(rf,"%d", &(*conf)[i]);
  }

  // Get the different file names
  for(i=0;i< *n_graphs;i++) {
    (*name)[i] = (char *) malloc(sizeof(char)*100);
    if ((*name)[i] == NULL) {
      printf("Unable to allocate memory for graph name %d \n",i);
      return(0);
    }
    // Read name of graph
    fscanf(rf,"%s",(*name)[i]);
  }

  fclose(rf);
  return(true);
}
