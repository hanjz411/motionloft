/********************************************************************
*
*
* File: rcg1.cpp
* Description: Create a function that prints a random number between 1 and 5 to stdout (or console). The probability distribution of the numbers should be as follows:
*
* 1 - 50%
* 2 - 25%
* 3 - 15%
* 4 - 5%
* 5 - 5%
*
* Or roughly that (no need to worry about rounding errors, etc)
*
*
* Version History:
*	08 Oct 2017		1.00		JH		First Revision
*
********************************************************************/

#include <iostream>
#include <ctime>
#include <random>

#define MAX_NUM 100

/*
**	simpleRNG()
**		summary:
**			Simple weighted RNG function 
**			
**		parameters:
**			none
**		returns:
**          int value between 1-5			
**
*/
int simpleRNG() {
    int r = rand() % 100;
    if (r < 50) {
        return 1;
    }
    if (r < 75) {
        return 2;
    }
    if (r < 90) {
        return 3;
    }
    if (r < 95) {
        return 4;
    }
    return 5;
}

/*
**	discreteRNG()
**		summary:
**			More powerful RNG using discrete distribution
**			(Interesting thought experiement I would love to discuss)
**		parameters:
**			reference to generator, reference to discrete distribution
**		returns:
**          int value between 1-5			
**
*/
int discreteRNG(std::mt19937 &gen, std::discrete_distribution<> &d) {
    return d(gen);      
}

/* 
** main() 
**      Driver function to test 
**
*/ 
int main()
{   
    // Initialize count to zero 
    int count[5] = { 0, 0, 0, 0, 0 };
   
     
    // Seed the random number generator 
    srand (time(NULL));
    for (int i = 0; i < MAX_NUM; i++) {
        int r = simpleRNG();
        count[r-1]++;
        printf("Generated value: %d\n", r);
    }

    for (int j = 0; j < 5; j++) {
       printf("Count %d: %d\n", j+1, count[j]);
    } 
     
    /* Discrete random number generator begins here */
    /* 
    std::random_device rd;
    std::mt19937 gen(rd());
    double weights[] =
        {0, // do not generate the number 0
         0.5, // number 1 at 50% probability
         0.25, // number 2 at 25% probability
         0.15, // number 3 at 0.15% probability
         0.05, // number 4 at 0.05% probability
         0.05}; // number 5 at 0.05% probability
         
    std::discrete_distribution<> d(std::begin(weights), std::end(weights));
    for (int i = 0; i < MAX_NUM; ++i) {
        int r = discreteRNG(gen, d);
        count[r-1]++;
        printf("Generated value: %d\n", r);
    }
    
    for (int j = 0; j < 5; j++) {
       printf("Count of %d: %d\n", j+1, count[j]);
    }   
    */

}
