#include "wframe2.h" // Include files and define variables
#include "mmio.c"  // Matrix market routines for 
#include <stdbool.h> // For using bool
#include <math.h>
#include <stdlib.h> // For using qsort

#include "handle_graph.c" // For input-output of graph
#include "handle_heap.c" // For handling min-heap, max-heap
#include "reduction.c"
#include "lossy.c"
#include "graph_operations.c"
#include "approx.c"
#include "experiments.c" // Experiments 

int main(int argc, char *argv[]) {
  // Primary graph data structure, using compressed neighbor lists.
  // ver stores pointers into the edges list. Note that numbering of both vertices and edges both start at 1

  int *ver;          // Pointers to edge lists
  int *edges;        // Edge lists
  
  // Raw edge lists in the order it was read from file
  edge *e;           // List of all edges, no weight, one struct for each edge
  int tlist[max_threads];  // List for storing data for each thread (in plocalmax)
    
  if (!get_input(argc, argv, &n_graphs, &n_runs, &n_conf, &conf, &name)){
    // Get input and check that it is in order
    return(false);
  }

  if (!allocate_memory(n_graphs, &n, &m)) {
    // Allocating memory
    return(false);
  }  

  if (!allocate_graph_memory(&e, &ver, &edges, max_n)){
    // Allocating memory specifically for this graph
    return(false);
  }

  for(int k=0; k < n_graphs; k++) {
    // Main loop for all graphs
    printf("\n");
    printf("************************************************\n");
    printf("*  Graph %2d: %20s    *\n", k, name[k]);
    
    // Reading the i'th graph
    if (!read_graph(&(n[k]), &(m[k]), name[k], &ver, &edges, &e)) {
      printf("Results \t -1 \t Problem reading graph %s \n", name[k]);
      continue;
      //return(false);
    }

    //printf("Results %s \t %d \t \%d \t", name[k], n[k], m[k]);
    //int sol1 = experiment_01(&(n[k]), &ver, &edges);
    //printf("%d \t", sol1);
    int sol2 = experiment_02(&(n[k]), &ver, &edges);
    //printf("%d \t", sol2);
    //int sol3 = experiment_03(&(n[k]), &ver, &edges);
    printf("\n");
  }
}
