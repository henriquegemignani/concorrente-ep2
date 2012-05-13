#ifndef GRAPH_H_
#define GRAPH_H_

#include <iostream>
#include <sstream>
#include <queue>
#include <vector>
#include <string>
#include <list>
#include "mutex.h"

#define NUM_CORES 4

typedef int Vertex;
typedef std::list<Vertex> Path;

struct QueueItem {
    std::list<Vertex> path;
    std::vector<bool> parents;

    QueueItem(size_t size)
        : parents(size) {}
};

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
        paths_per_vertex_.resize(size_);
        vertex_lock_.resize(size_);

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
    ~Graph() {}

    const std::list<Path>& menores_caminhos(Vertex v) {
        return paths_per_vertex_[v];
    }

    void CalculaMenoresCaminhosDe(Vertex v) {
        printf("Initializing:\n");
        BuscaEmLarguraIterativa();
    }

    void ClearPaths() {
        num_cores_finished_ = 0;
        for(size_t i = 0; i < size_; i++)
            paths_per_vertex_[i].clear();
    }

    void InitializeSearch(Vertex v) {
        queue_.push(createQueueItem(v));
    }

    void BuscaEmLarguraIterativa() {
        puts("Entrou na busca em largura.");
        counter_mutex_.Lock();
        num_cores_finished_++;
        counter_mutex_.Unlock();
        while(num_cores_finished_ != 2) {}
        puts("Saiu da barreira da entrada.");

        while(!queue_.empty()) {            
            queue_mutex_.Lock();
            if(!queue_.empty()) {
                QueueItem item = queue_.front();
                queue_.pop();
                queue_mutex_.Unlock();
                for(size_t i = 0; i < size_; i++)
                    if(matrix_[item.path.back()][i] && !item.parents[i]) {
                        QueueItem itemn = item;
                        itemn.path.push_back(i);
                        itemn.parents[i] = true;

                        queue_mutex_.Lock();
                        queue_.push(itemn);
                        queue_mutex_.Unlock();

                        vertex_lock_[i].Lock();
                        paths_per_vertex_[i].push_back(itemn.path);
                        vertex_lock_[i].Unlock();
                    }
            } else
                queue_mutex_.Unlock();
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
    QueueItem createQueueItem(Vertex v) {
        QueueItem item(size_);
        item.parents[v] = true;
        item.path.push_front(v);
        return item;
    }


    std::vector<Mutex> vertex_lock_;
    Mutex queue_mutex_, counter_mutex_;

    std::vector< std::vector<bool> > matrix_;
    std::vector< std::list<Path> > paths_per_vertex_;
    std::queue<QueueItem> queue_;
    size_t size_;

    size_t num_cores_finished_;
};

#endif /* GRAPH_H_ */
