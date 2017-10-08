/********************************************************************
*
*
* File: rcg3.cpp
* Description: Create a method that reads the most recently generated random number and the current time and writes them both to disk on one line.
*
*
* Version History:
*	08 Oct 2017		1.00		JH		First Revision
*   08 Oct 2017		1.10		JH		Change to method
*   08 Oct 2017		1.20		JH		Add writer function
*
********************************************************************/

#include <iostream>
#include <stdio.h>

/*
**	class RNG
**		summary:
**			simple RNG class that will generate weighted 
**          value between 1-5 
**			
**		
**
*/
class RNG {
private:
    int count[5] = {0, 0, 0, 0, 0}; 
    int random[100];
    int first = -1;
    int last = -1;

    void enq(int r);

public:
    RNG(int seed);
    ~RNG();
    int generate();
    double getFrequency(int r);
    void printRandomNumbers();
    void writeLatestToDisk(const char * file);
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
        enq(1);
        return 1;
    }
    if (r < 75) {
        enq(2);
        return 2;
    }
    if (r < 90) {
        enq(3);
        return 3;
    }
    if (r < 95) {
        enq(4);
        return 4;
    }
    enq(5);
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
        total = total + count[i];
    }
    return count[r-1] * 100 / total;
}

/*
**	enq()
**		summary:
**			Enqueues last generated value with max size 100
**			
**		parameters:
**			int value between 1-5
**		returns:
**          none		
**
*/
void RNG::enq(int r) {
    count[r-1]++;
    if (first == -1) {
        first = 0;
        last = 0;
        random[last] = r;
        return;
    }
    last = (last + 1) % 100;
    random[last] = r;
    if (last == first) {
        first = (first + 1) % 100;
    }
}

/*
**	printRandomNumbers()
**		summary:
**			Prints stream of generated values
**			
**		parameters:
**			none
**		returns:
**          none		
**
*/
void RNG::printRandomNumbers() {
    if (first == -1) {
        return;
    }
    int i = first;
    while (i != last) {
        printf("%d %d\n", i, random[i]);
        i++;
        if (i >= 100) {
            i = 0;
        }
    }
    printf("%d %d\n", i, random[i]);
}

/*
**	writeLatesttoDisk()
**		summary:
**			Writes latest value to file on disk
**			
**		parameters:
**			input file to write data 
**		returns:
**          none		
**
*/
void RNG::writeLatestToDisk(const char * file) {
    FILE *f = fopen(file, "w");
    
    //time_t now = time(0);   // get time now
    //char* dt = ctime(&now);
    //fprintf(f, "%d, %s\n", random[last], dt);
    //printf("%d, %d, %s\n", last, random[last], dt);
    time_t rawtime;
    struct tm * timeinfo;
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );
    fprintf(f, "%d, %s\n", random[last], asctime(timeinfo));
    printf("%d, %d, %s\n", last, random[last], asctime(timeinfo));
    fclose(f);
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
    }

   for (int j = 1; j <= 5; j++) {
       printf("Percent of %d: %.2f%%\n", j, rng->getFrequency(j));
   }
   
   rng->printRandomNumbers();
   rng->writeLatestToDisk("afile.txt");
   
   delete rng;
}
