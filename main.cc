#include <vector>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include "graph.h"
#include "worker.h"

/* Includes dependentes de ambiente. Achados usando Google. */
#ifdef _WIN32 /* _Win32 eh normalmente definido por compiladores compilando para sistemas Windows de 32 ou 64 bits */
# include <windows.h>
#elif __unix__ /* __unix__ eh normalmente definido por compiladores compilando para sistemas Unix. */
# include <unistd.h>
#endif

using std::cout;
using std::endl;

struct WorkerData {
    Graph& graph;
    int thread_number;

    WorkerData(Graph& g, int t_number)
        : graph(g),
          thread_number(t_number) {}
};

void* GraphWorker(Worker* w) {
    Graph* g = static_cast<Graph*>(w->data());
    g->BuscaEmLarguraIterativa(w->id());
    return NULL;
}

int main(int argc, char **argv) {
    size_t num_cores_;
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
    //cout << "Grafo: " << endl << g << endl;

    /* Standard number of threads */
    num_cores_ = 2;

    /* Number of cores detection */
	#ifdef WIN32
    SYSTEM_INFO sysinfo;
    GetSystemInfo( &sysinfo );
    num_cores_ = sysinfo.dwNumberOfProcessors;
    #else
    num_cores_ = sysconf( _SC_NPROCESSORS_ONLN );
    #endif

	cout << "Numero de cores: " << num_cores_ << endl;

    /* Inicializa numero maximo de caminhos por vertice */
    g.set_max_paths(N);

    /* Chamada de inicializacao do grafo. Recebe numero de threads a gerar e vertice da qual partirao as buscas. */
    g.Initialize(num_cores_, 0);

    /* Criacao e join de threads, delecao de workers. */

	/* Procura N menores caminhos aqui. */
	std::vector<Worker*> w;
	for(size_t i = 0; i < num_cores_; i++)
		w.push_back(new Worker(GraphWorker, &g));
		
	for(size_t i = 0; i < num_cores_; i++)
		w[i]->Run();

	for(size_t i = 0; i < num_cores_; i++)
		w[i]->Join();

	for(size_t i = 0; i < num_cores_; i++)
		delete w[i];

    printf("\n\nSaida:\n");
    /* Imprime a saida. */
    printf("Realizadas %d iteracoes.\n", g.iteration_number() - 1);
    for(int j = 1; j < g.size(); ++j) {
        cout << "Caminhos para o vertice " << j << endl;
        const std::list<Path>& caminhos = g.menores_caminhos(j);
        for(std::list<Path>::const_iterator it = caminhos.begin(); it != caminhos.end(); ++it) {
            cout << '\t';
            Path::const_iterator v = it->begin();
            cout << *v;
            for(++v; v != it->end(); ++v)
                cout << " - " << *v;
            cout << endl;
        }
    }
    return 0;
}
