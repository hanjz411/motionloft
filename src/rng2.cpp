/********************************************************************
*
*
* File: rcg2.cpp
* Description: Change the function to a method, add a class member that stores a history of the last 100 numbers, and a function to return the frequency percentages of each number.
*
* Create a method that reads the most recently generated random number and the current time and writes them both to disk on one line.
*
*
*
*
* Version History:
*	08 Oct 2017		1.00		JH		First Revision
* 08 Oct 2017		1.10		JH		Change to method
*
********************************************************************/

#include <iostream>
#include <ctime>

/*
**	class RNG
**		summary:
**			simple RNG class that will generate weighted 
**          value between 1-5
**          	
**
*/
class RNG {
private:
    int count[5] = {0, 0, 0, 0, 0}; 

public:
    RNG(int seed);
    ~RNG();
    int generate();
    double getFrequency(int r);
    void printCounts();
};

RNG::RNG(int seed) {
    srand(seed);
}

RNG::~RNG() {
}

/*
**	generate()
**		summary:
**			Generates weighted random value between 1-5 
**			
**		parameters:
**			none
**		returns:
**          int value between 1-5			
**
*/
int RNG::generate() {
    int r = rand() % 100;
    if (r < 50) {
        count[0]++;
        return 1;
    }
    if (r < 75) {
        count[1]++;
        return 2;
    }
    if (r < 90) {
        count[2]++;
        return 3;
    }
    if (r < 95) {
        count[3]++;
        return 4;
    }
    count[4]++;
    return 5;
}

/*
**	getFrequency()
**		summary:
**			Generates weighted random value between 1-5 
**			
**		parameters:
**			int value between 1-5
**		returns:
**          int count of value 			
**
*/
double RNG::getFrequency(int r) {
    if (r < 1 || r > 5) {
        return 0;
    }
    int total = 0;
    for (int i = 0; i < 5; i++) {
        total += count[i];
    }
    return count[r-1] * 100 / total;
}

/*
**	printCounts()
**		summary:
**			Prints to console frequency count of value
**			
**		parameters:
**			none
**		returns:
**          none
**
*/
void RNG::printCounts() {
    for (int j = 0; j < 5; j++) {
       printf("count %d: %d\n", j+1, count[j]);
    }
}

/* 
** main() 
**      Driver function to test 
**
*/ 
int main()
{
    int seed = time(NULL);
    
    RNG *rng = new RNG(seed);
    
    for (int i = 0; i < 100; i++) {
        int r = rng->generate();
        printf("%d\n", r);
   }
   
   for (int j = 1; j <= 5; j++) {
       printf("Percent of %d: %.2f\n", j, rng->getFrequency(j));
   }
}
