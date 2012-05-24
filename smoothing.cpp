#include "stdafx.h"
#include "smoothing.h"
#include <map>
#include <list>

#define SMOOTHSIZE 8
#define GRABTHRES 0.6
int grabList[SMOOTHSIZE];
int index =0;
int sum;

static std::list<int> smoothlist;

/********************************************************
					Hand smoothing
********************************************************/
void smoothHand(int i){

	smoothlist.push_front(i);
	// Keep size of history buffer
	if (smoothlist.size() > SMOOTHSIZE)
		smoothlist.pop_back();
}

bool isGrabsmooth(){
	//find average 

	int sum =0, count = 0;

	std::list<int>::const_iterator grabIterator;
	for (grabIterator = smoothlist.begin();
		grabIterator != smoothlist.end();
		++grabIterator){

			sum += *grabIterator;
			count++;
	}

	if(sum/count > GRABTHRES) return true;
	else return false;
}

void deleteSmHand(int id){

}