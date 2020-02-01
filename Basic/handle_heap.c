// Contains functions to handle heap
int _max_heapify_decrease_at(int ** heap, int ** value, int ** position, int index, int size_of_heap);
int _min_heapify_increase_at(int ** heap, int ** value, int ** position, int index, int size_of_heap);
int _min_heapify_decrease_at(int ** heap, int ** value, int ** position, int index, int size_of_heap);
bool is_max_heap(int * n, int * heap_array, int * value_array);
bool is_min_heap(int * n, int * heap_array, int * value_array);
 
int _max_heapify_decrease_at(int ** heap, int ** value, int ** position, int index, int size_of_heap){
  // max_heapify given heap accordinng to value array when value at index is decreased
  // Used in lossy reduction rule
  // Heapify_at takes input as :
  // heap (Array), value (array), position (array), index at heap (int), size_of_heap (int)
  // heapify given heap at given index accourding to value array. It chances position in accordingly
  int temp_val;
  int swap_index;
  int swap_val;
  
  while (index <= size_of_heap){
    // initialization of swap_val and swap_index
    swap_val = (*value)[(*heap)[index]];
    swap_index = index;
    
    if ( (2 * index <= size_of_heap) && (swap_val < (*value)[(*heap)[2 * index]]) ){
      swap_val = (*value)[(*heap)[2 * index]];
      swap_index = 2 * index;
    }

    if ( (2 * index + 1 <= size_of_heap) && (swap_val < (*value)[(*heap)[2 * index + 1]]) ){
      swap_val = (*value)[(*heap)[2 * index + 1]];
      swap_index = 2 * index + 1;
    }

    if (swap_index == index){
      break;
    }
    
    temp_val = (*heap)[swap_index];
    (*heap)[swap_index] = (*heap)[index];
    (*heap)[index] = temp_val; 

    (*position)[(*heap)[index]] = index; 
    (*position)[(*heap)[swap_index]] = swap_index;

    index = swap_index;
  }
  return(true);
}

int _min_heapify_increase_at(int ** heap, int ** value, int ** position, int index, int size_of_heap){
  // min_heapify given heap accordinng to value array when value at index is increased
  // Heapify_at takes input as :
  // heap (Array), value (array), index at heap (int), size_of_heap (int)
  // min_heapify given heap at given index accourding to value array. 
  int temp_val;
  int swap_index;
  int swap_val;
  
  while (index <= size_of_heap){
    // initialization of swap_val and swap_index
    swap_val = (*value)[(*heap)[index]];
    swap_index = index;
    
    if ((2 * index <= size_of_heap) && (swap_val > (*value)[(*heap)[2 * index]])){
      swap_val = (*value)[(*heap)[2 * index]];
      swap_index = 2 * index;
    }

    if ((2 * index + 1 <= size_of_heap) && (swap_val > (*value)[(*heap)[2 * index + 1]]) ){
      swap_val = (*value)[(*heap)[2 * index + 1]];
      swap_index = 2 * index + 1;
    }

    if (swap_index == index){
      break;
    }
    
    temp_val = (*heap)[swap_index];
    (*heap)[swap_index] = (*heap)[index];
    (*heap)[index] = temp_val;

    (*position)[(*heap)[index]] = index; 
    (*position)[(*heap)[swap_index]] = swap_index;

    index = swap_index;
  }
  return(true);
}


int _min_heapify_decrease_at(int ** heap, int ** value, int ** position, int index, int size_of_heap){
  // min_heapify given heap accordinng to value array when value at index is decreased
  // Heapify_at takes input as :
  // heap (Array), value (array), index at heap (int), size_of_heap (int)
  // min_heapify given heap at given index accourding to value array. 

  int temp_val;
  int swap_index;
  int swap_val;
  int parent_index;
  int parent_value;
  
  while (index >= 1){
    // initialization of swap_val and swap_index
    //swap_val = (*value)[(*heap)[index]];
    //swap_index = index;

    parent_index = floor(index/2);
    //printf("parent index %d \n", parent_index);
    if ((parent_index >= 1) && ((*value)[(*heap)[index]] < (*value)[(*heap)[parent_index]])){

      temp_val = (*heap)[parent_index];
      (*heap)[parent_index] = (*heap)[index];
      (*heap)[index] = temp_val;

      (*position)[(*heap)[index]] = index; 
      (*position)[(*heap)[parent_index]] = parent_index;
    }

    index = parent_index;
  }  
}


bool is_max_heap(int * n, int * heap_array, int * value_array){
  // * Starts function to check constructed heap is max heap
  for (int i=1; i <= *n; i++){
    if((2 * i <= *n) && (value_array[heap_array[i]] < value_array[heap_array[2 *i]])){
      printf("Something wrong at %d  %d  %d  %d %d \n", i, heap_array[i], value_array[heap_array[i]], heap_array[2 * i], value_array[heap_array[2 *i]]);
      return(false);
    }

    if((2 * i + 1 <= *n) && (value_array[heap_array[i]] < value_array[heap_array[2 *i + 1]])){
      printf("Something wrong at %d %d %d %d %d\n", i, heap_array[i], value_array[heap_array[i]], heap_array[2 *i + 1], value_array[heap_array[2 *i + 1]]);
      return (false);
    }
  }
  // * end
  return(true);
}

bool is_min_heap(int * n, int * heap_array, int * value_array){
  // * Starts function to check constructed heap is max heap
  for (int i=1; i <= *n; i++){
    if((2 * i <= *n) && (value_array[heap_array[i]] > value_array[heap_array[2 *i]])){
      printf("Something wrong at index %d, element here is  %d whose value is %d while its child %d have value %d \n", i, heap_array[i], value_array[heap_array[i]], heap_array[2 * i], value_array[heap_array[2 *i]]);
      return(false);
    }

    if((2 * i + 1 <= *n) && (value_array[heap_array[i]] > value_array[heap_array[2 *i + 1]])){
      printf("Something wrong at index %d, element here is  %d whose value is %d while its child %d have value %d \n", i, heap_array[i], value_array[heap_array[i]], heap_array[2 *i + 1], value_array[heap_array[2 *i + 1]]);
      return (false);
    }
  }
  // * end
  return(true);
}

