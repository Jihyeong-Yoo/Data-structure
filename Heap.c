#include <stdio.h>
#include <stdlib.h> // malloc, rand
#include <time.h> // time

#define MAX_ELEM	20

typedef struct
{
	int *heapArr;
	int	last;
	int	capacity;
} HEAP;

/* Allocates memory for heap and returns address of heap head structure
if memory overflow, NULL returned
*/
HEAP *heapCreate( int capacity);

/* Free memory for heap
*/
void heapDestroy( HEAP *heap);

/* Inserts data into heap
return 1 if successful; 0 if heap full
*/
int heapInsert( HEAP *heap, int data);

/* Reestablishes heap by moving data in child up to correct location heap array
*/
static void _reheapUp( HEAP *heap, int index);

/* Deletes root of heap and passes data back to caller
return 1 if successful; 0 if heap empty
*/
int heapDelete( HEAP *heap, int* data);

/* Reestablishes heap by moving data in root down to its correct location in the heap
*/
static void _reheapDown( HEAP *heap, int index);

/* Print heap array */
void heapPrint( HEAP *heap);

int main(void)
{
	HEAP *heap;
	int data;
	int i;
	
	heap = heapCreate(MAX_ELEM);
	
	srand( time(NULL));
	
	for (i = 0; i < MAX_ELEM; i++)
	{
		data = rand() % MAX_ELEM * 3 + 1; // 1 ~ MAX_ELEM*3 random number
				
		fprintf( stdout, "Inserting %d: ", data);
		
		// insert function call
		heapInsert( heap, data);
		
		heapPrint( heap);
 	}

	while (heap->last >= 0)
	{
		// delete function call
		heapDelete( heap, &data);

		printf( "Deleting %d: ", data);

		heapPrint( heap);
 	}
	
	heapDestroy( heap);
	
	return 0;
}


HEAP *heapCreate( int capacity){
	HEAP *heap = (HEAP*)malloc(sizeof(HEAP));
	heap->heapArr = (int*)malloc(sizeof(int)*capacity);
	if (!heap){
		heapDestroy(heap);
		return 0;
	}
	heap->last = -1;
	heap->capacity = capacity;
	return heap;
}
	

int heapInsert( HEAP *heap, int data){
	if(!heap)
		return 0;
	
	if(heap->last == -1){
		(heap->last)++;
		heap->heapArr[heap->last] = data;
		return 1;
	}
	
	if(heap->last != heap->capacity){
		(heap->last)++;
		heap->heapArr[heap->last] = data;
		_reheapUp(heap, heap->last);	
		return 1;
	}
	return 0;
}

static void _reheapUp( HEAP *heap, int index){
	if(index== 0)
		return;
	
	if(heap->heapArr[index] > heap->heapArr[(index+1)/2-1]){//맨 밑의 데이터가 부모보다 크다면
		int parent = heap->heapArr[(index+1)/2-1];
		heap->heapArr[(index+1)/2-1] = heap->heapArr[index];
		heap->heapArr[index] = parent;
		_reheapUp(heap, (index+1)/2-1);	
	}
}




int heapDelete( HEAP *heap, int* data){
	if(!heap){
		heapDestroy(heap);
		return 0;
	}
	*data = heap->heapArr[0];	
	heap->heapArr[0] = heap->heapArr[heap->last-1];
	heap->last--;
	_reheapDown(heap, 0);
	return 1;	
}


static void _reheapDown( HEAP *heap, int index){
	int leftkey, leftkeyIndex = 0;
	int rightkey, rightkeyIndex = 0;
	int largeSubtree, largeSubtreeIndex = 0;
	if((index+1)*2 > heap->last)
		return;

	if(!(heap->heapArr[(index+1)*2-1])){//왼쪽 자식이 있다면
		leftkey = heap->heapArr[(index+1)*2-1];
		leftkeyIndex = (index+1)*2-1;
		
		if(!(heap->heapArr[(index+1)*2])){
			rightkey = heap->heapArr[(index+1)*2];
			rightkeyIndex = (index+1)*2;
		}
		else{
			rightkey = 0;
			rightkeyIndex = 0;
		}
		
		if(leftkey > rightkey){
			largeSubtree = leftkey;
			largeSubtreeIndex = leftkeyIndex;
		}
		else{
			largeSubtree = rightkey;
			largeSubtreeIndex = rightkeyIndex;
		}
	}
	
	if(heap->heapArr[index] < largeSubtree){
		int parent = heap->heapArr[index];
		heap->heapArr[index] = largeSubtree;
		heap->heapArr[largeSubtreeIndex] = parent;
		_reheapDown(heap, largeSubtreeIndex);
	}
	return;
}
		
		

void heapDestroy( HEAP *heap){ 
	free(heap->heapArr);
	free(heap);
	return;
}

void heapPrint( HEAP *heap){
	for(int i=0; i<=heap->last; i++){
		printf("%d  ", heap->heapArr[i]);
	} 
	printf("\n");
	return;
}










