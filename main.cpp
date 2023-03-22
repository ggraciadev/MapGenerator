#include "MapGenerator.h"

int main () {
    MapGenerator test;
    unsigned int seed;
    cin >> seed;
    //test.GenerateMap();
    test.Init(seed);
    bool finished = false;
    while(!finished) {
        cout << "ITERACION" << endl;
        finished = !test.GenerateMapStep();
        test.PrintMap();
        
    }
    return 0;
}