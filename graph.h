#ifndef GRAPH_H_
#define GRAPH_H_

#include <iostream>
#include <sstream>
#include <vector>
#include <string>

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
        matrix_.resize(size);
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
};

#endif /* GRAPH_H_ */
