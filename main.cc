#include <vector>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include "graph.h"
#include "worker.h"

#define NUM_CORES 4

using std::cout;
using std::endl;

void* GraphWorker(void* data) {
    Graph* g = static_cast<Graph*>(data);
    g->CalculaMenoresCaminhosDe(0);
    return NULL;
}

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
    cout << "Grafo: " << endl << g << endl;

    g.ClearPaths();
    g.InitializeSearch(0);
    {
        /* Procura N menores caminhos aqui. */
        std::vector<Worker*> w;
        w.push_back(new Worker(GraphWorker, &g));
        w.push_back(new Worker(GraphWorker, &g));
            
        for(int i = 0; i < 2; i++) {
            printf("Inicializando thread %d\n", i);
            w[i]->Run();
        }

        for(int i = 0; i < 2; i++)
            w[i]->Join();

        for(std::vector<Worker*>::iterator it = w.begin(); it != w.end(); ++it)
            delete (*it);
    }

    /* Imprime a sa�da. */
    for(int j = 1; j < g.size(); ++j) {
        cout << "Caminhos para o vertice " << j << endl;
        const std::list<Path>& caminhos = g.menores_caminhos(j);
        for(std::list<Path>::const_iterator it = caminhos.begin(); it != caminhos.end(); ++it) {
            cout << '\t';
            Path::const_iterator v = it->begin();
            cout << *v;
            for(++v; v != it->end(); ++v)
                cout << " -> " << *v;
            cout << endl;
        }
    }
    return 0;
}
