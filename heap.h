#include <stdio.h>
#include <stdlib.h>

typedef struct charNode {
    char symbol;
    unsigned long int freq;

    struct charNode *left, *right;

} charNode;

typedef struct Heap{
    //tail is the index of the last item, size is the number elements
    //in queue
    int tail, size;
    unsigned capacity;
    charNode** arrptr;

	//Pointer to comparator function
    int (*key)(const void* v1, const void* v2);
} Heap;

Heap* createHeap(unsigned capacity, int (*key)(const void* v1, const void* v2)){
    Heap* ref = (Heap*) malloc(sizeof(Heap));
    ref->size = 0;
    ref->tail = -1;
    ref->capacity = capacity;
    ref->arrptr = (charNode**) malloc(ref->capacity * sizeof(charNode*));
    ref->key = key;
    return ref;
}

void deleteHeap(Heap* h){
	free(h);
    h = NULL;
}

//Functions for accessing parent and child indices
unsigned parentIdx(unsigned childIdx){
    return (childIdx-1)/2;
}
unsigned leftIdx(unsigned nodeIdx){
    return (2*nodeIdx) + 1;
}
unsigned rightIdx(unsigned nodeIdx){
    return (2*nodeIdx) + 2;
}

void swap(charNode** n1ptr, charNode** n2ptr){
	if (*n1ptr == NULL || *n2ptr == NULL){
		printf("Error: Check node pointers");
	}
	else{
		charNode* tmp = *n1ptr;
		*n1ptr = *n2ptr;
		*n2ptr = tmp;
	}
}

void enqueue(Heap* h, charNode* node){
    if (h->size == h->capacity){
		printf("Error: Heap full!");
    }
	if (node == NULL){
		printf("Error: charNode ptr is NULL");
	}
    
	else{
		h->tail++;
		h->size++;
		unsigned i = h->tail;
		unsigned pidx = parentIdx(i);
		h->arrptr[i] = node;

		while (i > 0 && (*h->key)(h->arrptr[i], h->arrptr[pidx]) == -1){
			swap(&(h->arrptr[i]), &(h->arrptr[pidx]));
			i = pidx;
			pidx = parentIdx(i);
	}
    }
}

void dequeue(Heap* h, charNode** output){
    if (h->size == 0){
		printf("Error: MinHeap Empty!");
		//(*output)->symbol = '?';
		//(*output)->freq = -1;
		return;
    }
    else{
		*output = (h->arrptr[0]);
		
		h->arrptr[0] = h->arrptr[h->tail];
		h->tail--;
		h->size--;
		
		if (h->size > 0){
			int i = 0;
			while (1){
				int lidx = leftIdx(i);
				int ridx = rightIdx(i);
				int tmp = i;

				if (lidx < h->size && (*h->key)(h->arrptr[tmp], h->arrptr[lidx]) == 1){tmp = lidx;}
				if (ridx < h->size && (*h->key)(h->arrptr[tmp], h->arrptr[ridx]) == 1){tmp = ridx;}
				if (i != tmp){
					swap(&(h->arrptr[i]), &(h->arrptr[tmp]));
					i = tmp;
				}
				else{break;}
			}
			
		}
    }
}

void peek(Heap* h){
    if (h->size == 0){
		printf("Error: Heap is empty!");
    }
    else{
		printf("%c, %lu\n", h->arrptr[0]->symbol, h->arrptr[0]->freq);

    }
}

void peekArr(Heap* h){
    printf("[");
    for(int i = 0; i<h->size; i++){
		printf("[%c, %lu]", h->arrptr[i]->symbol, h->arrptr[i]->freq);

    }
    printf("]\n");
}




