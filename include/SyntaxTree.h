// Created by mandriy on 5/25/15.
//

#ifndef TRANSLATORDESIGN_SYNTAXTREE_H
#define TRANSLATORDESIGN_SYNTAXTREE_H

#include "grammar.h"
#include "Lexer.h"

#include <stack>
#include <memory>

using namespace std;

class Node {
public:
    Node(string label) : label(label) { }

    virtual ~Node() { }

    virtual void addChild(shared_ptr<Node> child) {
        throw std::logic_error("This kind of node does not support child appending.");
    }

    virtual vector<shared_ptr<Node>> &getChildren() {
        throw std::logic_error("This kind of node does not have children.");
    }

    virtual void traverseChildren(function<void(shared_ptr<Node>)> traverseFunc) {
        throw std::logic_error("This kind of node does not support children traversing.");
    }

    virtual shared_ptr<Node> getChildAt(unsigned int index) {
        throw std::logic_error("This kind of node has no child.");
    }

    virtual Position getNodePosition(){
        throw std::logic_error("Node position can't be determined.");
    }

    virtual bool isIterable() {
        return false;
    }

    const string &getLabel() const {
        return label;
    }

private:

    string label;
};

class EmptyNode : public Node {
public:
    EmptyNode() : Node("") { }

};

class LeafNode : public Node {
public:

    LeafNode(shared_ptr<Token> &token) : Node(token->rep), token(token) { }

    shared_ptr<Token> const &getToken() const {
        return token;
    }

    virtual Position getNodePosition() override;

    virtual ~LeafNode() { }

private:

    shared_ptr<Token> token;

};

class InteriorNode : public Node {
public:
    InteriorNode(Sort sort) : Node(sortsLabels.at(sort)), sort(sort) {

    }

    virtual bool isIterable() override;

    virtual ~InteriorNode() { }

    virtual vector<shared_ptr<Node>> &getChildren() override;

    virtual void addChild(shared_ptr<Node> child) {
        children.push_back(child);
    }

    virtual void traverseChildren(function<void(shared_ptr<Node>)> traverseFunc) {
        for (auto child: children) {
            traverseFunc(child);
        }
    }

    virtual shared_ptr<Node> getChildAt(unsigned int index) override;

    Sort getSort() {
        return sort;
    }

    virtual Position getNodePosition() override;

private:
    Sort sort;
    vector<shared_ptr<Node>> children;
};

class SyntaxTree {
public:
    typedef function<void(shared_ptr<Node>)> TraverseFunction;

    SyntaxTree(shared_ptr<Node> root) : root(root) { }

    virtual ~SyntaxTree() { }

    void treeTraverse(vector<TraverseFunction> pre, vector<TraverseFunction> post) {
        traversePrepare(pre, post);
        treeTraverseInner(root);
    }

    void preTraverse(TraverseFunction func) {
        treeTraverse(vector<TraverseFunction>(1, func), vector<TraverseFunction>());
    }

    void postTraverse(TraverseFunction func) {
        vector<TraverseFunction> post(1, func);
        treeTraverse(vector<TraverseFunction>(), vector<TraverseFunction>(1, func));
    }

    shared_ptr<Node> const &getRoot() const {
        return root;
    }

private:

    void treeTraverseInner(shared_ptr<Node> node) {
        for (auto &preFunc: preTraverseFuncs) {
            preFunc(node);
        }
        if (node->isIterable()) {
            for (auto &child: node->getChildren()) {
                treeTraverseInner(child);
            }
        }
        for (auto &postFunc: postTraverseFuncs) {
            postFunc(node);
        }
    }

    void traversePrepare(vector<TraverseFunction> &pre, vector<TraverseFunction> &post) {
        preTraverseFuncs.clear();
        postTraverseFuncs.clear();
        for (auto &preFunc: pre) {
            preTraverseFuncs.push_back(preFunc);
        }
        for (auto &postFunc: post) {
            postTraverseFuncs.push_back(postFunc);
        }
    }

    vector<TraverseFunction> preTraverseFuncs;
    vector<TraverseFunction> postTraverseFuncs;

    shared_ptr<Node> root;
};


class Matcher {
public:
    Matcher(Sort sortToMatch, SyntaxTree::TraverseFunction func) : func(func), sort(sortToMatch) { }

    void operator()(shared_ptr<Node> node) {
        if ( node->isIterable() && ((InteriorNode*) node.get())->getSort() == sort) {
            func(node);
        }
    }

private:
    SyntaxTree::TraverseFunction func;
    Sort sort;
};


class SyntaxTreeBuilder {
public:

    SyntaxTreeBuilder() : root(nullptr) { }

    class InteriorNodeLayer {
    public:
        InteriorNodeLayer(SyntaxTreeBuilder *builder, Sort interiorNodeLabel) : builder(builder) {
            builder->pushLayer(shared_ptr<Node>(new InteriorNode(interiorNodeLabel)));
        }

        ~InteriorNodeLayer() {
            builder->popLayer();
        }

    private:
        SyntaxTreeBuilder *builder;

    };

    SyntaxTree getTree() {
        return SyntaxTree(root);
    }

    void buildLeafNode(shared_ptr<Token> token) {
        layersStack.top()->addChild(shared_ptr<Node>(new LeafNode(token)));
    }

    void buildEmptyNode() {
        layersStack.top()->addChild(shared_ptr<Node>(new EmptyNode()));
    }

private:
    void popLayer() {
        layersStack.pop();
    }

    void pushLayer(shared_ptr<Node> layer) {
        if (root == nullptr) {
            root = layer;
        } else {
            layersStack.top()->addChild(layer);
        }
        layersStack.push(layer);
    }

    stack<shared_ptr<Node>> layersStack;
    shared_ptr<Node> root;
};


std::string getIdentifieString(std::shared_ptr<Node> idNode);
int evalUnsignedInteger(shared_ptr<Node> intNode);
bool isArrayCall(std::shared_ptr<Node> variable);
bool isEmptyDimension(shared_ptr<Node> dimension);
shared_ptr<Node> getVarDimensionExpr(shared_ptr<Node> variable);

#endif //TRANSLATORDESIGN_SYNTAXTREE_H
