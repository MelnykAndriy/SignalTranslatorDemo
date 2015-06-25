//
// Created by mandriy on 5/25/15.
//

#include "../include/GraphTree.h"

Graph GraphVizTreeBuilder::getTree() {
    return Graph(vertexLabels, edges);
}

void GraphVizTreeBuilder::buildLeafNode(shared_ptr<Token> token) {
    edges.push_back(Edge(layersStack.top(), nodeCounter++));
    vertexLabels.push_back(token->rep);
}

void GraphVizTreeBuilder::buildEmptyNode() {
    edges.push_back(Edge(layersStack.top(), nodeCounter++));
    vertexLabels.push_back("");
}

void GraphVizTreeBuilder::pushLayer(Sort layer) {
    int vertexId = nodeCounter++;
    if (layersStack.size()) {
        edges.push_back(Edge(layersStack.top(), vertexId));
    }
    vertexLabels.push_back(sortsLabels.at(layer));
    layersStack.push(vertexId);
}

void GraphVizTreeBuilder::popLayer() {
    layersStack.pop();
}