#include "vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

const int SENT = 4;
void VectorNew(vector *v, int elemSize, VectorFreeFunction freeFn, int initialAllocation){
	assert(elemSize >= 0);
	assert(initialAllocation >= 0);
	
	if(initialAllocation == 0){
		initialAllocation = SENT;
	}
	
	v->elemSize = elemSize;
	v->allocLen = initialAllocation;
	v->logLen = 0;
	v->root = malloc(elemSize * initialAllocation);
	v->freeFn = freeFn;
}

void VectorDispose(vector *v){
	if(v->freeFn != NULL){
		for(int i = 0; i < v->logLen; i ++){
			void* elem = (char*)v->root + i * v->elemSize;
			v->freeFn(elem);
		}
	}	
	free(v->root);
}

int VectorLength(const vector *v){
	return v->logLen;
}

void *VectorNth(const vector *v, int position){
	assert(position >= 0 && position <= v->logLen);
	return (char*)v->root + v->elemSize * position;	
} 

void VectorReplace(vector *v, const void *elemAddr, int position){
	assert(position >= 0 && position < v->logLen);	

	void* dest = VectorNth(v, position);
	if(v->freeFn != NULL) v->freeFn(dest);
	memcpy(dest, elemAddr, v->elemSize);
}

void VectorInsert(vector *v, const void *elemAddr, int position){
	assert(position >= 0 && position <= v->logLen);	
	if(v->logLen == v->allocLen){
		v->allocLen *= 2;
		v->root = realloc(v->root, v->allocLen * (v->elemSize));
	}
	
	int diff = v->logLen - position;        
	void* curr = VectorNth(v, position);    //address to the element with the pos, position.
	void* next = (char*)curr + v->elemSize; //address, next to the curr

	memmove(next, curr, (diff)*v->elemSize);	
	memcpy(curr, elemAddr, v->elemSize);

	v->logLen++;
}

void VectorAppend(vector *v, const void *elemAddr){
	if(v->logLen == v->allocLen){
		v->allocLen *= 2;
		v->root = realloc(v->root, v->allocLen * (v->elemSize));
	}
	
	void* last = VectorNth(v, v->logLen);
	memcpy(last, elemAddr, v->elemSize);
	v->logLen ++;
}

void VectorDelete(vector *v, int position){
	assert(position >= 0 && position < v->logLen);	
	
	int diff = v->logLen - position;      
	void* curr = (char*)VectorNth(v, position); //address to the element with the pos, position.
	void* next = (char*)VectorNth(v, position) + v->elemSize;
	if(v->freeFn != NULL){
		v->freeFn(curr);
	}
	memmove(curr, next, (diff-1)*v->elemSize);
	v->logLen--;
}

void VectorSort(vector *v, VectorCompareFunction compare){
	assert(compare != NULL);
	qsort(v->root, v->logLen, v->elemSize, compare);
}

void VectorMap(vector *v, VectorMapFunction mapFn, void *auxData){
	assert(mapFn != NULL);
	
	for(int i = 0; i  < v->logLen; i ++){
		void* curr = VectorNth(v , i);
		
		mapFn(curr, auxData);
	}
}

static const int kNotFound = -1;

int VectorSearch(const vector *v, const void *key, VectorCompareFunction searchFn, int startIndex, bool isSorted){
	assert(key != NULL);
	assert(startIndex >= 0 && startIndex <= v->logLen);
	
	void* base = (char*)v->root + startIndex * v->elemSize; //address, where from we are going to search key
	if(isSorted){
	  	void* ans = bsearch(key, base, v->logLen- startIndex , v->elemSize, searchFn);
	 	if(ans != NULL){
			int i = (char*)ans - (char*)v->root;
			i = i/v->elemSize;
			return i;
		}
	} else {
		for(int i = 0; i < v->logLen - startIndex; i ++){
			if(searchFn(base, key) == 0){
				return startIndex + i;
			} 
			base = (char*)base +  v->elemSize;
		}
	}
 	return kNotFound;
 } 
