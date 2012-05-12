#ifndef GRAPH_H_
#define GRAPH_H_

#include <iostream>
#include <sstream>
#include <queue>
#include <vector>
#include <string>
#include <list>

typedef int Vertex;
typedef std::list<Vertex> Path;
typedef std::vector<bool> Parents;
struct item {
    Path path;
    Parents parents;
};
typedef item QueueItem;

class Graph {
  public:
    Graph(std::istream& input) {

        // Le a primeira linha da entrada
        std::string first_line;
        std::getline(input, first_line);
        std::stringstream first_line_stream(first_line);

        // Parseia a primeira linha e guarda num vector
        int x;
        std::vector<bool> first_line_vect;
        while(first_line_stream >> x)
            first_line_vect.push_back(x != 0);

        // A quantidade de elementos da primeira linha � o tamanho do grafo
        size_ = first_line_vect.size();
        matrix_.resize(size_);
        paths_.resize(size_);
        paths_per_vertex_.resize(size_);

        // Guarda a primeira linha
        matrix_[0] = first_line_vect;

        // Le as outras linhas.
        for(size_t j = 1; j < size_; ++j) {
            std::vector<bool>& row = matrix_[j];
            row.resize(size_);
            for(size_t i = 0; i < size_; ++i) {
                int x;
                input >> x;
                row[i] = (x != 0);
            }
        }

    }

    void PrintPaths() {
        for(int i = 0; i < size_; i++) {
            std::list<Path>::iterator it;
            for(it = paths_per_vertex_[i].begin(); it!=paths_per_vertex_[i].end(); it++) {
                std::list<Vertex>::iterator vertex = (*it).begin();
                printf("%d", (*vertex));
                for(vertex++ ; vertex != (*it).end(); vertex++)
                    printf(" -> %d", (*vertex));
                printf("\n");
            }
        }
    }

    const std::list<Path>& menores_caminhos(Vertex v) {
        std::queue<QueueItem> queue = std::queue<QueueItem>();
        std::list<Vertex> vertex_list;
        QueueItem item;

        item.parents = Parents(size_);
        item.path = Path();
        item.parents[v] = true;
        vertex_list.push_front(v);
        item.path.push_front(v);
        queue.push(item);
        BuscaEmLarguraIterativa(queue);
        return paths_;
    }

    void BuscaEmLarguraIterativa(std::queue<QueueItem> queue) {
        int n_paths = 0;
        QueueItem item;
        for(int i = 0; i < size_; i++)
            paths_per_vertex_[i].clear();
        while(!queue.empty()) {
            item = queue.front();
            queue.pop();
            int i;
            for(i = 0; i < size_; i++)
                if(matrix_[item.path.back()][i] && !item.parents[i]) {
                    n_paths++;
                    QueueItem itemn = {item.path, item.parents};
                    itemn.path.push_back(i);
                    itemn.parents[i] = true;
                    queue.push(itemn);
                    paths_per_vertex_[i].push_back(itemn.path);
                }
            if(n_paths == 0)
                paths_.push_back(item.path);
            else
                n_paths = 0;
        }
    }

    int size() { return size_; }

    friend std::ostream& operator<< (std::ostream& out, const Graph& g) {
        std::vector< std::vector<bool> >::const_iterator j;
        for(j = g.matrix_.begin(); j != g.matrix_.end(); ++j) {
            std::vector<bool>::const_iterator x;
            for(x = j->begin(); x != j->end(); ++x)
                out << ((*x) ? "1 " : "0 ");
            out << std::endl;
        }
        return out;
    }

    size_t size() const { return matrix_.size(); }

  private:
    std::vector< std::vector<bool> > matrix_;
    std::vector< std::list<Path> > paths_per_vertex_;
    std::list<Path> paths_;
    size_t size_;
};

#endif /* GRAPH_H_ */
