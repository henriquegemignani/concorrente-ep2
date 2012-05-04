#include <vector>
#include <iostream>
#include <fstream>
#include "graph.h"

#define NUM_CORES 4

using std::cout;
using std::endl;

int main(int argc, char **argv) {
    if(argc < 3) {
        cout << "Uso: " << argv[0] << " N <path para arquivo de topologia>" << endl;
        return 1;
    }
    int N = atoi(argv[1]);
    std::ifstream arquivo(argv[2]);
    if(!arquivo.is_open()) {
        cout << "Erro ao abrir '" << argv[2] << "'" << endl;
        return 2;
    }
    cout << N << endl;
    Graph g(arquivo);

    cout << g;
    return 0;
}