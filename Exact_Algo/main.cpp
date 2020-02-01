#include <ilcplex/ilocplex.h>
#include <string>
#include <iostream>


ILOSTLBEGIN


#include <stdbool.h> // For using bool

#include "wframe2.h"	// Include files and define variables
#include "mmio.c"       // Matrix market routines for reading files
//#include "lossy.c"

struct ilp_solution{
  int value;
  int status; 
};

ILOMIPINFOCALLBACK5(timeLimitCallback, IloCplex, cplex, IloBool,  aborted, IloNum, timeStart, IloNum, timeLimit, IloNum, acceptableGap)
{
  if (!aborted && hasIncumbent() ) {
    IloNum gap = 100.0 * getMIPRelativeGap();
    IloNum timeUsed = cplex.getCplexTime() - timeStart;
    //if (timeUsed > 1) getEnv().out() << timeUsed << endl;
    if (timeUsed > timeLimit && gap < acceptableGap ) {
      //getEnv().out() << endl << "Good enough solution at " << timeUsed << " sec., gap = " << gap << "%, quitting." << endl;
      aborted = IloTrue;
      abort();
    }
  }  
}


int main(int argc, char *argv[]) {
  // Primary graph data structure, using compressed neighbor lists.
  // ver stores pointers into the edges list. Note that numbering of both vertices and edges both start at 1

  int *ver;          // Pointers to edge lists
  int *edges;        // Edge lists
  double *weight;    // Corresponding edge weights

  // Edge lists where each neighbor list has been sorted by decreasing weight

  int *s_edges;      // Sorted edge lists
  double *s_weight;  // Sorted corresponding edge weights

  // Vertex lists used in various algorithms

  int *next;         
  int *used;        // Keeps track of which vertices has been used in the dynamic programming

  // Vertex lists used for holding the matching

  int *p;	     // If p(i) = j then i is matched with j, this is used in all the algorithms
  int *p1;           // Same as p, but used for first round of two round algorithms
  int *p2;           // Same as p, but used for second round of two round algorithms
  int *p3;           // Same as p, but used for second round of two round algorithms

  // Vertex lists for suitor algorithm

  double *ws1;       // If ws1(i) = w, then the weight of the best suitor of i is w
  double *ws2;       // Same as ws1, but used in two round algorithm

  // Raw edge lists in the order it was read from file

  edge *e;           // List of all edges, no weight, one struct for each edge
  wed *we;	     // Same as e, but now with weights
  neig *swe;	     // List of neighbor + weight 

  // omp_lock_t *nlocks;
  int tlist[max_threads];  // List for storing data for each thread (in plocalmax)

  //int read_graph(), get_input(), allocate_memory(), read_general_graph();
  int read_graph(int *n, int *m, char *f_name, int **ver, int **edges, edge **e, double **weight, wed **we, neig **swe, int *count, int *start, int **where);
  int get_input(int argc, char *argv[],int *n_graphs,int *n_runs,int *n_conf,int **conf,char ***name);
  int allocate_memory(int size,int **n,int **m,double ***timer,double ***cost,double ****p_timer,double ****p_cost,int n_conf);
  
  //int read_bin_graph();
  //int prepare_output();
  //int prepare_output(FILE **wf,int n_conf,int conf[]);
  //int allocate_graph_memory();
  int allocate_graph_memory(neig **swe,double **weight,edge **e,wed **we,int **ver,int **edges,int n,int **next,int **used,int **p,int **p1,int **p2,int **p3);
  int CPLEX_Solve(int *n, int *m, int **ver, int **edges, int **color, int timelimit1);
  //int rr_lossy_v2(int *n, int ** ver, int ** edges, int * color, int * solution, int * overload);

  
  if (!get_input(argc, argv, &n_graphs, &n_runs, &n_conf, &conf, &name))	// Get input and check that it is in order
    return(false);

  //if (!prepare_output(&wf, n_conf, conf)){				// Prepare output file
  //  return(false);
  //}

  if (!allocate_memory(n_graphs, &n, &m, &timer, &cost, &p_timer, &p_cost, n_conf)) {// Allocating memory
    return(false);
  }

  // Run the main loop, processing each graph, first sequentially and then in parallel

  if (!allocate_graph_memory(&swe, &weight, &e, &we, &ver, &edges, max_n, &next, &used, &p, &p1, &p2, &p3)){			// Allocating memory specifically for this graph
    return(false);
  }

  int *where;
  where = (int *) malloc(2*sizeof(int)*(max_m));
  if (where == NULL) {
    printf("Unable to allocate space for where array \n");
    return(false);
  }

  ws1 = (double *) malloc(sizeof(double)*(max_n+1));
  ws2 = (double *) malloc(sizeof(double)*(max_n+1));
  if ((ws1 == NULL) || (ws2 == NULL)) {
    printf("Unable to allocate space for ws-arrays \n");
    return(false);
  }

  for(int k=0; k < n_graphs; k++) {

    printf("\n");
    printf("************************************************\n");
    printf("*  Graph %2d: %20s    *\n",k,name[k]);

    
    // Reading the i'th graph

    if (!read_graph(&(n[k]), &(m[k]), name[k], &ver, &edges, &e, &weight, &we, &swe, p, p1, &where)) {
      printf("Problem reading graph %s \n",name[k]);
      return(false);
    }

    int *color; //To store color of vertices. color[v] = 1 (black); or 0 (white)
    color = (int *) malloc(sizeof(int)*(2 + *n));
    //int *solution; 
    //solution = (int *) malloc(sizeof(int)*(2 + *n));
    //int lossy_accuracy = 1; // Accuracy in lossy reduction rule.
    //int *overload; // Overload over every vertex.
    //overload = (int *) malloc(sizeof(int)*(2+ *n));

    if (color == NULL) {
      printf("Unable to allocate space for color array \n");
      return(false);
    }

    for(int i=1; i <= *n; i++){
      //overload[i] = lossy_accuracy;
      //solution[i] = 0;
      color[i] = 1;
    }

    //for(int i=1; i<= *n; i++){
    //  cout << "(" << solution[i] << "," << color[i] << "), ";
    //}

    int time_allowed_for_CPLEX = 30;
    int sol_val1 = CPLEX_Solve(&(n[k]), &(m[k]), &ver, &edges, &color, time_allowed_for_CPLEX);
    cout << "\n Results \t" << name[k] << "\t " << n[k] << "\t " << sol_val1 << endl;
    
    //rr_lossy_v2(&(n[k]), &ver, &edges, color, solution, overload);

    //cout << "After lossy \n";
    //int lossy_reduction = 0;
    //for(int i=1; i<= *n; i++){
    //  if (solution[i]) lossy_reduction += 1;
    //}

    // int sol_val2 = CPLEX_Solve(&(n[k]), &(m[k]), &ver, &edges, &color, 60);
    

    //cout << "\n Results \t" << name[k] << "\t " << sol_val1 << "\t " << sol_val2 + lossy_reduction << endl;
    

    //free(solution);
    free(color);
    //free(overload);
    
  }
  return 0;
}

int CPLEX_Solve(int *n, int *m, int **vertices, int **edges, int **color, int timelimit1){
  IloEnv env;
  IloModel model(env);
  IloNumVarArray var(env);
  IloRangeArray con(env);
  IloExpr sum_ver(env);
  
  for(int i=1; i <= *n; i++){
    var.add(IloNumVar(model.getEnv(), 0, 1, ILOINT));
  }
  
  for(int x=0; x < var.getSize(); x++){
    sum_ver += var[x];
  }

  IloObjective obj = IloMinimize(model.getEnv(), sum_ver);

  model.add(obj);

  IloExpr cls_nbr(env);//To express closed neighborhood of a vertex
  for(int i = 1; i <= *n; i++){
    if ((*color)[i] != 1) continue;
    
    //i goes from 1 to n but index in var goes from 0 to n-1
    cls_nbr = var[i-1]; //Add vertex itself
    for(int j=(*vertices)[i]; j < (*vertices)[i+1]; j++){
      cls_nbr += var[(*edges)[j] - 1];
    }
    con.add(cls_nbr >= 1);
  }

  model.add(con);

  IloCplex cplex(model);
  //cplex.exportModel("main.lp");

  cplex.setParam(IloCplex::Param::MIP::Display, 0);
  cplex.setParam(IloCplex::Param::TimeLimit, timelimit1);
  //cplex.use(timeLimitCallback(env, cplex, IloFalse, cplex.getCplexTime(), 1, 10.0));
  
  // Optimize the problem and obtain solution.
  if (!cplex.solve() ) {
    env.error() << "Results : Failed to optimize LP" << endl;
    throw(-1);
  }

  //env.out() << "Solution status = " << cplex.getStatus() << endl;
  //IloNum gap =  cplex.getMIPRelativeGap();
  //env.out() << "Gap " << gap << endl;
  //env.out() << "Solution value  = " << cplex.getObjValue() << endl;
  //env.out() << cplex.getObjValue() << "\t " << cplex.getStatus() << endl;  
  //IloNumArray vals(env);
  //cplex.getValues(vals, var);
  //env.out() << "Values        = " << vals << endl;
  //cplex.getSlacks(vals, con);
  //env.out() << "Slacks        = " << vals << endl;
  //cplex.getDuals(vals, con);
  //env.out() << "Duals         = " << vals << endl;
  //cplex.getReducedCosts(vals, var);
  //env.out() << "Reduced Costs = " << vals << endl;

  int solution =  cplex.getObjValue();

  //if (cplex.getStatus() == "feasible") solution.status = 1;

  //if (cplex.getStatus() == "optimal") solution.status = 2;
  
  //cout << solution.status << endl;
  cls_nbr.end();
  sum_ver.end();
  env.end();

  return solution;
}

int read_graph(int *n,int *m,char *f_name,int **ver,int **edges,edge **e,
               double **weight,wed **we,neig **swe,int *count, int *start,int **where) {

  // Note reuse of p as count and p1 as start

  FILE *fp;
  long int i;
  int x,y,z;
  double v,w;
  MM_typecode matcode;
  int cols;

  int mxdeg = 0;
  //int *count;
  //int *start;

  // printf("Opening file %s \n",f_name);

  // First make sure graph is of the right type

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


  // Start counting of degrees by setting counters to zero
  for(i=1;i<=*n;i++) {
    count[i] = 0;
  }

  int num_edges = 0;

  //  printf("Number of possible edges is %d \n",*m);
  
  // Read inn the edges
  if (mm_is_real(matcode)) {
    // printf("Real matrix, Starting to read %d edges \n",*m);
    srand48(time(NULL));
    for(i=0;i<*m;i++) {
      fscanf(fp,"%d %d %lf",&x,&y,&v); // Use this line if there is exactly one double weight 
      //  fscanf(fp,"%d %d %lf %lf",&x,&y,&v,&w); // Use this line for complex weights
      if (x != y) { // Avoid self-edges
	/*
	  (*e)[num_edges].x = x; // Store edges
	  (*e)[num_edges].y = y;
	*/

        (*we)[num_edges].x = x;
        (*we)[num_edges].y = y;
	//        int intv = (int) fabs(v);
	//        (*we)[num_edges].w = (double) intv; 
        (*we)[num_edges].w = fabs(v);     // Take absolute value of edge
    
        if (fabs(v) < 0.0)  {
          printf("error reading data,got %f \n",fabs(v));
          return(false);
        }
	//   (*we)[num_edges].w = 1.0;
	//  (*we)[num_edges].w = (double)rand();

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
    for(i=0;i<*m;i++) {
      fscanf(fp,"%d %d %lf",&x,&y,&v); // Use this line if there is exactly one double weight 
    
      if (x != y) { // Avoid self-edges
	/*
	  (*e)[num_edges].x = x; // Store edges
	  (*e)[num_edges].y = y;
	*/

        (*we)[num_edges].x = x;
        (*we)[num_edges].y = y;
        (*we)[num_edges].w = fabs(v) + drand48()/1000.0;     // Take absolute value of edge

        if (fabs(v) < 0.0)  {
          printf("error reading data,got %f \n",fabs(v));
          return(false);
        }

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
  else
    {          // Symbolic matrix
      srand48(time(NULL));
      printf("Symbolic matrix \n");
      for(i=0;i<*m;i++) {
	fscanf(fp,"%d %d",&x,&y);
	if (x != y) { // Avoid self-edges
	  /*
	    (*e)[num_edges].x = x; // Store edges
	    (*e)[num_edges].y = y;
	  */

	  (*we)[num_edges].x = x;
	  (*we)[num_edges].y = y;

	  (*we)[num_edges].w = drand48();  // Using random value

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
    start[i] = start[i-1]+count[i-1];
    (*ver)[i] = start[i];
  }

  // Place edges in edge lists, once for each endpoint

  for(i=0;i<*m;i++) {
    x = (*we)[i].x;
    y = (*we)[i].y;
    v = (*we)[i].w;
    if ((x == 0) || (y == 0)) {
      printf("edge %ld: %d and %d are neighbors, numbering starts from 1! \n",i,x,y);
      return(false);
    }

    (*where)[start[x]] = start[y];
    (*where)[start[y]] = start[x];

    (*edges)[start[x]] = y;
    (*weight)[start[x]] = v;
    start[x]++;

    (*edges)[start[y]] = x;
    (*weight)[start[y]] = v;
    start[y]++;

  }


  int maxnode;
  //  printf("ver1 = %d ver2=%d ver3=%d \n",(*ver)[1],(*ver)[2],(*ver)[3]);
  for(i=1;i<=*n;i++) {
    if (((*ver)[i+1] - (*ver)[i]) > mxdeg) {
      mxdeg = (*ver)[i+1] - (*ver)[i];
      maxnode = i;
    }
  }
  /*
    printf("Vertex %d has maximum degree %d \n",maxnode,mxdeg);
    printf("Average degree is %d \n",((*m) * 2)/(*n));
  */


  //  printf("Done read_graph, freeing up memory \n");
  fclose(fp);
  // free(count);
  // free(start);
  return(true);
}

// Get input. This is given as a file name to the executable. 
// This file should contain the following information (per line):
// Number of input graphs
// Number of times each configuration is run, the program reports the best running time out of these
// Number of configurations, followed by the number of threads in each configuration
// One line giving the complete name of each graph file that is to be run

int get_input(int argc, char *argv[],int *n_graphs,int *n_runs,int *n_conf,int **conf,char ***name) {

  FILE *rf;	// File pointer
  int i;

  if (argc != 2) {
    printf("Give data file name as first input parameter!\n");
    return(false);
  }

  printf("************************************************\n");
  printf("*       Reading setup from %10s          *\n",argv[1]);
  printf("************************************************\n");

  // Opening file containing file names for graphs

  rf = fopen(argv[1],"r");
  if (rf == NULL) {
    printf("Cannot open data file: %s \n",argv[1]);
    return(false);
  }


  // Get number of graphs to read
  fscanf(rf,"%d",n_graphs);

  *name = (char **) malloc(sizeof(char *)*(*n_graphs));  // Allocate one pointer for each graph 

  if (*name == NULL) {
    printf("Unable to allocate space for names of %d graphs\n",max_graphs);
    return(0);
  }

  // Get number of runs per configuration
  fscanf(rf,"%d",n_runs);

  // Get number of thread configurations
  fscanf(rf,"%d",n_conf);
  *conf = (int *) malloc(sizeof(int)*(*n_conf));  // Allocate space for configurations


  if (*conf == NULL) {
    printf("Unable to allocate memory for %ls different thread configurations \n",n_conf);
    return(0);
  }

  // Get the different configurations
  for(i=0;i< *n_conf;i++) {
    fscanf(rf,"%d",&(*conf)[i]);
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


int allocate_memory(int size,int **n,int **m,double ***timer,double ***cost,double ****p_timer,double ****p_cost,int n_conf) {

  *n = (int *) malloc(sizeof(int)*size);  // List holding the number of vertices in each graph

  if (*n == NULL) {
    printf("Unable to allocate memory for n[] in allocate_memory() \n");
    return(false);
  }

  *m = (int *) malloc(sizeof(int)*size);  // List holding the number of edges in each graph
  if (*m == NULL) {
    printf("Unable to allocate memory for m[] in allocate_memory() \n");
    return(false);
  }

  *timer = (double **) malloc(sizeof(double *)*(max_experiment));  // Allocate one pointer for each sequential experiment
  if (*timer == NULL) {
    printf("Unable to allocate memory for timer in allocate_memory() \n");
    return(false);
  }

  *cost = (double **) malloc(sizeof(double *)*(max_experiment));  // Allocate one pointer for each sequential experiment
  if (*timer == NULL) {
    printf("Unable to allocate memory for cost in allocate_memory() \n");
    return(false);
  }

  // For each experiment, allocate one number for each graph
  int i;
  for(i=0;i<max_experiment;i++) {
    (*cost)[i] = (double *) malloc(sizeof(double)*size);
    if ((*cost)[i] == NULL) {
      printf("Unable to allocate memory for cost %d in allocate_memory() \n",i);
      return(false);
    }
    (*timer)[i] = (double *) malloc(sizeof(double)*size);
    if ((*timer)[i] == NULL) {
      printf("Unable to allocate memory for timer %d in allocate_memory() \n",i);
      return(false);
    }
    int j;
    for(j=0;j<size;j++)          // Set each timer to -1
      (*timer)[i][j] = -1.0;
  }

  *p_timer = (double ***) malloc(sizeof(double **)*(max_experiment));  // Allocate one pointer for each parallel experiment
  *p_cost  = (double ***) malloc(sizeof(double **)*(max_experiment));  // Allocate one pointer for each parallel experiment

  if (*p_timer == NULL) {
    printf("Unable to allocate memory for p_timer in allocate_memory() \n");
    return(false);
  }
  if (*p_cost == NULL) {
    printf("Unable to allocate memory for p_cost in allocate_memory() \n");
    return(false);
  }
  // For each experiment, allocate "size" pointers for each graph
  for(i=0;i<max_experiment;i++) {
    (*p_timer)[i] = (double **) malloc(sizeof(double *)*size);
    (*p_cost)[i]  = (double **) malloc(sizeof(double *)*size);
    if ((*p_timer)[i] == NULL) {
      printf("Unable to allocate memory for p_timer %d in allocate_memory() \n",i);
      return(false);
    }
    if ((*p_cost)[i] == NULL) {
      printf("Unable to allocate memory for p_cost %d in allocate_memory() \n",i);
      return(false);
    }
    int j;
    for(j=0;j<size;j++) {
      (*p_timer)[i][j] = (double *)  malloc(sizeof(double)*n_conf);
      (*p_cost)[i][j] = (double *)  malloc(sizeof(double)*n_conf);
      if ((*p_timer)[i][j] == NULL) {
        printf("Unable to allocate memory for p_timer %d in allocate_memory(),%d \n",i,j);
        return(false);
      }
      if ((*p_cost)[i][j] == NULL) {
        printf("Unable to allocate memory for p_cost %d,%d in allocate memory \n",i,j);
        return(false);
      }
      int k;
      for(k=0;k<n_conf;k++) {
        (*p_timer)[i][j][k] = -1.0;
        (*p_cost)[i][j][k] = -1.0;
      }
    }
  }
  return(true);
}


int prepare_output(FILE **wf,int n_conf,int conf[]) {
  // Open file for writing of results
  *wf = fopen("results.m","w");
  if (*wf == NULL) {
    printf("Unable to open results.m for writing \n");
    return(false);
  }

  // print the thread configurations to file
  fprintf(*wf,"x = [");
  int i;
  for(i=0;i<n_conf;i++) 
    fprintf(*wf,"%d ",conf[i]);
  fprintf(*wf,"];\n");

  return(true);
}


// Allocating memory specifically for this graph
int allocate_graph_memory(neig **swe,double **weight,edge **e,wed **we,int **ver,int **edges,int n,int **next,int **used,int **p,int **p1,int **p2,int **p3) {		

  // The compressed edge list with the weight, needed for sorting using qsort()

  long temp_long = 2;
  //temp_long = temp_long * max_m * sizeof(neig);
  temp_long = max_m * sizeof(neig);
  //printf("Temp_long: %ld \n", temp_long);
  *swe = (neig *) malloc(temp_long);
  if (*swe == NULL) {
    printf("Unable to allocate space for swe-array in allocate_graph_memory() \n");
    return(false);
  }


  // The edge list with the weight
  //  printf("MAX_M = %d \n",max_m);

  *we = (wed *) malloc(max_m * sizeof(wed));
  if (*we == NULL) {
    printf("Unable to allocate space for we-array in allocate_graph_memory() \n");
    return(false);
  }


  // 'weight' contains the weight of each edge, stored in the same way as the edge-lists
  *weight= (double *) malloc(2*sizeof(double)*(max_m));
  if (*weight== NULL) {
    printf("Unable to allocate space for weight-array in allocate_graph_memory() \n");
    return(false);
  }
  // The raw edge list is stored in e, i.e. in the same order as it was read in

  /*  This part is removed to ensure space for parallel algorithms.
   *  Must be returned to run unweighted code
 
   *e = (edge *) malloc(sizeof(edge)*(max_m));
   if (*e == NULL) {
   printf("Unable to allocate space for e-array in allocate_graph_memory() \n");
   return(false);
   }
  */

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

  *p = (int *) malloc(sizeof(int)*(n+2));	// Storage for final matching
  if (*p == NULL) {
    printf("Unable to allocate space for p[] in allocate_graph_memory() \n");
    return(false);
  }

  *p1 = (int *) malloc(sizeof(int)*(n+2));	// Storage for first matching in two level algorithms
  if (*p1 == NULL) {
    printf("Unable to allocate space for p1[] in allocate_graph_memory() \n");
    return(false);
  }

  *p2 = (int *) malloc(sizeof(int)*(n+2));	// Storage for second matching in two level algorithms
  if (*p2 == NULL) {
    printf("Unable to allocate space for p2[] in allocate_graph_memory() \n");
    return(false);
  }

  *p3 = (int *) malloc(sizeof(int)*(n+2));	// Even more storage...
  if (*p3 == NULL) {
    printf("Unable to allocate space for p3[] in allocate_graph_memory() \n");
    return(false);
  }

  *next = (int *) malloc(sizeof(int)*n*2);	// Pointers used in suitor based algorithm with sorted edge lists
  if (*next == NULL) {
    printf("Unable to allocate space for next[] in allocate_graph_memory() \n");
    return(false);
  }

  *used = (int *) malloc(sizeof(int)*(n+2));
  if (*used == NULL) {
    printf("Unable to allocate space for used[] in allocate_graph_memory() \n");
    return(false);
  }
  return(true);
}
