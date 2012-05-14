#ifndef GRAPH_H_
#define GRAPH_H_

#include <iostream>
#include <sstream>
#include <queue>
#include <vector>
#include <string>
#include <list>
#include <set>
#include <cmath>
#include "mutex.h"

typedef int Vertex;
typedef std::list<Vertex> Path;

static int num_threads = 0;

static unsigned int number_log2 (unsigned int val) {
    /* Warning: This very obviously DOES NOT WORK FOR THE NUMBER 0 */
    unsigned int ret = 0, number_of_ones_ = 0;
    do {
      ret++;
      if(val & 1)
          number_of_ones_++;
    } while (val >>= 1);
    if(number_of_ones_ == 1)
        return ret-1;
    else
        return ret;
}

struct QueueItem {
    std::list<Vertex> path;
    std::vector<bool> parents;

    QueueItem(size_t size)
        : parents(size) {}
};

bool PathCompareFunc(const Path& l, const Path& r) {
	return l.size() < r.size();
}

typedef bool (*PathCompare)(const Path& l, const Path& r);

class Graph {
  public:
    Graph(std::istream& input) : debug_(false) {
        // Le a primeira linha da entrada
        std::string first_line;
        std::getline(input, first_line);
        std::stringstream first_line_stream(first_line);

        // Parseia a primeira linha e guarda num vector
        int x;
        std::vector<bool> first_line_vect;
        while(first_line_stream >> x)
            first_line_vect.push_back(x != 0);

        // A quantidade de elementos da primeira linha é o tamanho do grafo
        size_ = first_line_vect.size();
        matrix_.resize(size_);
        paths_per_vertex_.resize(size_, std::multiset<Path, PathCompare>(PathCompareFunc) );
        number_of_paths_per_vertex_.resize(size_);
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

    const std::multiset<Path, PathCompare>& menores_caminhos(Vertex v) {
        return paths_per_vertex_[v];
    }

    void set_max_paths(int N) {
        max_paths_ = N;
    }

    size_t iteration_number() { return iteration_number_; }

    void CalculaMenoresCaminhosDe(Vertex v) {

        BuscaEmLarguraIterativa(num_threads++);
    }

    void Initialize(size_t num_cores, Vertex v) {
        num_cores_ = num_cores;
        threads_finished_ = false;
        iteration_number_ = 1;
        
        if (num_cores_ < 2)
            num_cores_ = 2;

        number_of_stages_ = number_log2(num_cores_);

        for(size_t i = 0; i < size_; i++) {
            paths_per_vertex_[i].clear();
            number_of_paths_per_vertex_[i] = 0;
        }
        
        arrived.resize(num_cores_);
        for(size_t i = 0; i < num_cores; i++)
            arrived[i] = 0;
        InitializeSearch(v);
    }

    void InitializeSearch(Vertex v) {
        list_of_paths_.push_front(createQueueItem(v));
    }

	void Barreira(int thread_number) {
		size_t sum = 1;
		for(size_t i = 1; i <= number_of_stages_; i++) {
			arrived[thread_number]++;
			int next_arrive = (thread_number+sum) % num_cores_;
			sum <<= 1;
			while (arrived[next_arrive] < arrived[thread_number]) {};
		}
	}

    void BuscaEmLarguraIterativa(int thread_number) {
        while(!threads_finished_) {
            
			Barreira(thread_number);
            
			if(list_of_paths_.empty()) break;
            
			/* Este código de if vai claramente contra a restrição de simetria do EP,
               porem é usado pois é apenas codigo de debug */
            if(thread_number == 0)
                printf("\nIteracao %d:\n", iteration_number_++);
			
			Barreira(thread_number);

            queue_mutex_.Lock();
            if(list_of_paths_.empty()) {
                queue_mutex_.Unlock();
                continue;
            }
            QueueItem item = list_of_paths_.front();
            list_of_paths_.pop_front();
            queue_mutex_.Unlock();

            for(size_t i = 0; i < size_; i++) {
                vertex_lock_[i].Lock();
                bool b = (matrix_[item.path.back()][i] && number_of_paths_per_vertex_[i] < max_paths_ && !item.parents[i]);
                vertex_lock_[i].Unlock();
				if(!b) continue;

				number_of_paths_per_vertex_[i]++;

				QueueItem itemn = item;
				itemn.path.push_back(i);
				itemn.parents[i] = true;

				queue_mutex_.Lock();
				if(!list_of_paths_.empty())
					if(itemn.path.size() < list_of_paths_.back().path.size())
						list_of_paths_.push_front(itemn);
					else
						list_of_paths_.push_back(itemn);
				else
					list_of_paths_.push_front(itemn);
				queue_mutex_.Unlock();

				vertex_lock_[i].Lock();
				paths_per_vertex_[i].insert(itemn.path);
				vertex_lock_[i].Unlock();
            }
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
	bool debug() const { return debug_; }
	void set_debug(bool debug) { debug_ = debug; }

  private:
    QueueItem createQueueItem(Vertex v) {
        QueueItem item(size_);
        item.parents[v] = true;
        item.path.push_front(v);
        return item;
    }

    unsigned int num_cores_;
    bool threads_finished_;

    std::vector<Mutex> vertex_lock_;
    Mutex queue_mutex_, counter_mutex_;

    std::vector< std::vector<bool> > matrix_;
    std::vector< std::multiset<Path, PathCompare> > paths_per_vertex_;
    std::list<QueueItem> list_of_paths_;

    std::vector<size_t> arrived;
    std::vector<size_t> number_of_paths_per_vertex_;
    size_t size_;
    size_t max_paths_;
    size_t number_of_stages_;
    size_t iteration_number_;
	bool debug_;
};

#endif /* GRAPH_H_ */
