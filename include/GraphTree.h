//
// Created by mandriy on 5/25/15.
//

#ifndef TRANSLATORDESIGN_GRAPHTREE_H
#define TRANSLATORDESIGN_GRAPHTREE_H

#include <boost/graph/graphviz.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <string>

#include "grammar.h"
#include "Lexer.h"

using namespace std;

typedef pair<int, int> Edge;

class Graph {
private:
    friend class GraphVizTreeBuilder;
    typedef boost::adjacency_list <boost::vecS, boost::vecS, boost::directedS,
            boost::property<boost::vertex_color_t, boost::default_color_type>,
            boost::property<boost::edge_weight_t, int>> LibGraph;


    Graph(vector<string> vertexNames, vector<Edge> edges) : vertexNames(vertexNames) {
        int weights[edges.size()];
        std::fill(weights, weights + edges.size(), 1);
        g = LibGraph(begin(edges), end(edges), weights, 0);
    }

    LibGraph g;
    vector<string> vertexNames;

    class VertexLabelWriter {
    public:
        VertexLabelWriter(const Graph *g) : g(g) { }
        void operator()(ostream &os, unsigned const int vertex) {
            os << "[label=\"" << g->vertexNames.at(vertex) << "\"]";
        }
    private:
        const Graph *g;
    };

public:
    friend ostream &operator<<(ostream &os, const Graph &graph) {
        boost::write_graphviz(os, graph.g, VertexLabelWriter(&graph));
        return os;
    }
};

class GraphVizTreeBuilder {
public:

    GraphVizTreeBuilder() : nodeCounter(0) { }

    virtual ~GraphVizTreeBuilder() { }

    class InteriorNodeLayer {
    public:
        InteriorNodeLayer(GraphVizTreeBuilder *builder, Sort interiorNodeLabel) : builder(builder) {
            builder->pushLayer(interiorNodeLabel);
        }

        ~InteriorNodeLayer() {
            builder->popLayer();
        }

    private:
        GraphVizTreeBuilder *builder;
    };

    Graph getTree();
    void buildLeafNode(shared_ptr<Token> token);
    void buildEmptyNode();

private:
    void popLayer();
    void pushLayer(Sort layer);

    int nodeCounter;
    stack<int> layersStack;
    vector<Edge> edges;
    vector<string> vertexLabels;
};

#endif //TRANSLATORDESIGN_GRAPHTREE_H
