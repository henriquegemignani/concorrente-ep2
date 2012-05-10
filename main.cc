#include <vector>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include "graph.h"

#define NUM_CORES 4

using std::cout;
using std::endl;

int main(int argc, char **argv) {
    if(argc < 3) {
        cout << "Uso: " << argv[0] << " N <path para arquivo de topologia>" << endl;
        return 1;
    }
    int N = std::atoi(argv[1]);
    std::ifstream arquivo(argv[2]);
    if(!arquivo.is_open()) {
        cout << "Erro ao abrir '" << argv[2] << "'" << endl;
        return 2;
    }
    cout << "Procurando os " << N << " menores caminhos." << endl;
    Graph g(arquivo);
    cout << "Grafo: " << endl << g;

    for(size_t j = 1; j < g.size(); ++j) {
        /* Procura N menores caminhos aqui. */
        /* Imprime a saída. */
    }
    return 0;
}
