#include "hashset.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

const int SENTINEL = 4; 

void HashSetNew(hashset *h, int elemSize, int numBuckets,
		HashSetHashFunction hashfn, HashSetCompareFunction comparefn, HashSetFreeFunction freefn){
	assert(elemSize > 0);
	assert(numBuckets >= 0);
	assert(comparefn != NULL);
	assert(hashfn != NULL);
	
	h->elemSize = elemSize;
	h->numBuckets = numBuckets;
	h->count = 0;
	
	h->base = malloc(sizeof(vector) * numBuckets);
	for(int i = 0; i < numBuckets; i ++){
		VectorNew(&h->base[i], elemSize, freefn, SENTINEL);
	}
	h->hashFn = hashfn;
	h->compareFn = comparefn;
	h->freeFn = freefn;
}

void HashSetDispose(hashset *h){
	for(int i = 0; i < h->numBuckets; i ++){
		VectorDispose(&h->base[i]);
	}
	free(h->base);
}

int HashSetCount(const hashset *h){
	return h->count;
}

void HashSetMap(hashset *h, HashSetMapFunction mapfn, void *auxData){
	assert(mapfn != NULL);
	for(int i = 0; i < h->numBuckets; i ++){
		for(int j = 0; j < VectorLength(&h->base[i]); j ++){
			mapfn(VectorNth(&h->base[i], j), auxData);
		}
	}
}

void HashSetEnter(hashset *h, const void *elemAddr){
	int index = h->hashFn(elemAddr, h->numBuckets);
	assert(index >= 0 && index < h->numBuckets);
	int addInd = VectorSearch(&h->base[index], elemAddr, h->compareFn, 0, false);
	assert(elemAddr != NULL);
	assert(addInd >= -1 && addInd < VectorLength(&h->base[index]));
	
	if(addInd == -1){
		VectorAppend(&h->base[index], elemAddr);
	} else if(addInd < VectorLength(&h->base[index])) {
		VectorReplace(&h->base[index], elemAddr, addInd);
	}  
	
	h->count ++;
}

void *HashSetLookup(const hashset *h, const void *elemAddr){
	int index = h->hashFn(elemAddr, h->numBuckets);
	assert(elemAddr != NULL);
	assert(index >= 0 && index < h->numBuckets);
	
	int addInd = VectorSearch(&h->base[index], elemAddr, h->compareFn, 0, false);
	if(addInd != -1){
		return VectorNth(&h->base[index], addInd);
	}
	return NULL;
}
