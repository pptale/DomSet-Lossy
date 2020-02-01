//Contains reduction rules (with resp to normal kernelization)
void rr_deg0(int *n, int ** ver, int ** edges, int *color, int *solution);
void rr_deg1(int *n, int ** ver, int ** edges, int *color, int *solution);
 
void rr_deg0(int *n, int ** ver, int ** edges, int *color, int *solution){
  //Reduction Rule
  //It modifies color and solution array.
  //If there is isolated vertex of black color than include that vertex in solution and color it white
  for(int i=1; i < *n; i++){
    if (((*ver)[i+1] == (*ver)[i]) && color[i] == 1){
      color[i] = 0;
      solution[i] = 1;
    }
  }
}

void rr_deg1(int *n, int ** ver, int ** edges, int *color, int *solution){
  //Reduction Rule
  //It modifies color and solution array.
  //If there is a pendent vertex of black color than reduction rule includes its unique neighbor, say y, in solution. Colors all vertices in N[y] to 0 (white).
  int y;
  for(int i=1; i <= *n; i++){
    if ((color[i] == 1) && ((*ver)[i+1] - (*ver)[i] == 1)){
      y = (*edges)[(*ver)[i]];
      solution[y] = 1;
      color[y] = 0;
      for(int j=(*ver)[y]; j < (*ver)[y + 1]; j++){
	color[(*edges)[j]] = 0;
      }
    }
  }
}
