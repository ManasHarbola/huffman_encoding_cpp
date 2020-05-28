#include <stdio.h>
#include <stdlib.h>
#include "heap.h"

#define NUM_CHARS 256
//Assume only ASCII chars are being used

int minKey(const void* node1, const void* node2){
    //-1 if node1.freq < node2.freq
    //0 if node1.freq == node2.freq
    //1 if node1.freq > node2.freq
    unsigned long int f1 = ((charNode*) node1)->freq;
    unsigned long int f2 = ((charNode*) node2)->freq;
    if (f1 == f2) {return 0;}
    else if (f1 < f2) {return -1;}
    else {return 1;}
}

int maxKey(const void* node1, const void* node2){
    //-1 if node1.freq > node2.freq
    //0 if node1.freq == node2.freq
    //1 if node1.freq < node2.freq
    unsigned long int f1 = ((charNode*) node1)->freq;
    unsigned long int f2 = ((charNode*) node2)->freq;
    if (f1 == f2){return 0;}
    else if (f1 > f2){return -1;}
    else {return 1;}
}

void countFreq(FILE* fp, unsigned long int* freq, unsigned* uniqueChars){
    if (fp == NULL){freq = NULL;}
    else{
		int c = 0;
		while ((c = fgetc(fp)) != EOF){
			if (freq[c] == 0){(*uniqueChars)++;}
        	freq[c]++;
    	}
	fclose(fp);
    }
}


void viewFreq(unsigned long int* freq){
    for(int i = 0; i < NUM_CHARS; i++){
	    printf("%lu ", freq[i]);
    }
}

void fillHeap(Heap* h, unsigned long int* freqArray, unsigned uniqueChars, charNode* nodeArr){
    if (h == NULL || freqArray == NULL){
       printf("Error: Invalid Heap and/or freqArray ptrs\n");	
    }
    else {
		for (int i = 0, j = 0; i < NUM_CHARS && j < uniqueChars; i++){
			if (freqArray[i] != 0){
				nodeArr[j].symbol = i;
				nodeArr[j].freq = freqArray[i];
				nodeArr[j].left = NULL;
				nodeArr[j].right = NULL;
				enqueue(h, nodeArr + j);
				j++;
			}
		}
    }
}


void makeHuffmanTree(Heap* h, charNode** head, charNode** deleteRef){
	if (h == NULL || h->size <= 0){
		printf("Error: check Heap ptr and size value\n");
		deleteRef = NULL;
	}
	else if (h->size == 1){
		charNode* sum = (charNode*) malloc(sizeof(charNode));
		*deleteRef = sum;

		dequeue(h, &(sum->left));
		sum->symbol = -1;
		sum->freq = sum->left->freq;
		enqueue(h, sum);
		dequeue(h, head);
	}

	else{
		charNode* sum = (charNode*) malloc((h->size-1) * sizeof(charNode));
		*deleteRef = sum;

		int i = 0;
		while (h->size > 1){
			//dequeue first node
			//peekArr(h);
			dequeue(h, &((sum + i)->left));
			
			//dequeue second node
			dequeue(h, &((sum + i)->right));
			
			//Indicate this a sum node
			(sum + i)->symbol = -1;
			//set value of sum node
			(sum + i)->freq = (sum + i)->left->freq + (sum + i)->right->freq;

			//enqueue sum node
			enqueue(h, sum + i);
			i++;
		}
		
		//dequeue final node
		dequeue(h, head);

		return;
	}
}



void padding ( char ch, int n ){
  int i;
  
  for (i = 0; i < n; i++)
    putchar ( ch );
}

void viewHuffman(charNode* root, int level){
	  if ( root == NULL ) {
		padding ( '\t', level );
		puts ( "~" );
	  } 
	  else {
		if (root->symbol == '\0')	  
			viewHuffman ( root->right, level + 1 );
		
		padding ( '\t', level );
		
		if (root->symbol != '\n')
			printf ( "(%lu, %c)\n", root->freq, root->symbol );
		else if (root-> symbol == '\n')
			printf ( "(%lu, \\n)\n", root->freq);

		
		if (root-> symbol == '\0')
			viewHuffman ( root->left, level + 1 );
	  }
}

int main(){
    FILE* fp;
    char* filename = "./test.txt";
    fp = fopen(filename, "r");
    charNode cNode;
    unsigned uniqueChars = 0;

    if (fp == NULL){
		printf("Failed to open %s", filename);
		return 0;
    }
    else {
    		//Create freqArray
		unsigned long int* freqArray = (unsigned long int*) calloc(NUM_CHARS, sizeof(unsigned long int));
		//unsigned long int* backup = freqArray;
		//Count char frequencies
		countFreq(fp, freqArray, &uniqueChars);

		//View char frequencies
		//viewFreq(freqArray);
        
		//Create array of charNodes
		charNode* nodeArr = (charNode*) calloc(uniqueChars, sizeof(charNode));
		
		//Create Heap
		Heap* charHeap = createHeap(uniqueChars, &minKey);
		
		//charNode* out = (charNode*) malloc(sizeof(charNode));
		charNode* out = NULL;
		charNode* deleteRef = NULL;

		//Fill heap
		fillHeap(charHeap, freqArray, NUM_CHARS, nodeArr);
		
		//Create Huffman Tree
		makeHuffmanTree(charHeap, &out, &deleteRef);
		//peekArr(charHeap);
		//viewHuffman(out, 0);
		
				tfree(freqArray);
		free(nodeArr);
		if (deleteRef != NULL)
			free(deleteRef);

		free(charHeap->arrptr);
		free(charHeap);
		//free(out);
    }

    return 1;
}
	
