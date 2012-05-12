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
        std::string first_line;
        std::getline(input, first_line);
        std::stringstream first_line_stream(first_line);
        int x;
        std::vector<bool> first_line_vect;
        while(first_line_stream >> x)
            first_line_vect.push_back(x != 0);

        size_t size = first_line_vect.size();
        size_ = size;
        matrix_.resize(size);
        paths_.resize(size);
        matrix_[0] = first_line_vect;
        for(size_t j = 1; j < size; ++j) {
            std::vector<bool>& row = matrix_[j];
            row.resize(size);
            for(size_t i = 0; i < size; ++i) {
                int x;
                input >> x;
                row[i] = (x != 0);
            }
        }

    }

    void BuscaEmLargura(Vertex v) {
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
    }

    void BuscaEmLarguraIterativa(std::queue<QueueItem> queue) {
        QueueItem item;
        while(!queue.empty()) {
            item = queue.front();
            queue.pop();
            for(int i = 0; i < size_; i++)
                if(matrix_[item.path.back()][i] && !item.parents[i]) {
                    printf("%d -> %d\n", item.path.back(), i);
                    QueueItem itemn = {item.path, item.parents};
                    itemn.path.push_back(i);
                    itemn.parents[i] = true;
                    queue.push(itemn);
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

  private:
    std::vector< std::vector<bool> > matrix_;
	std::vector< std::list<Path> > paths_;
    int size_;
};

#endif /* GRAPH_H_ */
