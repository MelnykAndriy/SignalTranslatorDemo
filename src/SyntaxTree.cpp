//
// Created by mandriy on 5/25/15.
//

#include "../include/SyntaxTree.h"

bool InteriorNode::isIterable() {
    return true;
}

vector<shared_ptr<Node>> &InteriorNode::getChildren() {
    return children;
}

shared_ptr<Node> InteriorNode::getChildAt(unsigned int index) {
    return children[index];
}

Position LeafNode::getNodePosition() {
    return token->pos;
}

Position InteriorNode::getNodePosition() {
    return getChildAt(0)->getNodePosition();
}


string getIdentifieString(shared_ptr<Node> idNode) {
    SyntaxTree identifierSubTree(idNode);
    string id;
    identifierSubTree.preTraverse(Matcher(Sort::Identifier, [&](shared_ptr<Node> node) -> void {
        id = node->getChildAt(0)->getLabel();
        return;
    }));
    if ( id != "") {
        return id;
    } else {
        throw logic_error("Identifier is not found");
    }
}

int evalUnsignedInteger(shared_ptr<Node> intNode) {
    SyntaxTree intSubTree(intNode);
    string unsignedInt;

    intSubTree.preTraverse(Matcher(Sort::UnsignedInteger, [&](shared_ptr<Node> unsignedIntNode) -> void {
        unsignedInt = unsignedIntNode->getChildAt(0)->getLabel();
    }));
    if ( unsignedInt != "") {
        return stoi(unsignedInt);
    } else {
        throw logic_error("Unsigned integer is not found.");
    }

}

bool isArrayCall(std::shared_ptr<Node> variable) {
    return ((InteriorNode*) variable.get())->getSort() == Sort::Variable && !isEmptyDimension(variable->getChildAt(1));
}

bool isEmptyDimension(shared_ptr<Node> dimension) {
    return ((InteriorNode*) dimension.get())->getSort() == Sort::Dimension && dimension->getChildren().size() == 1;
}

shared_ptr<Node> getVarDimensionExpr(shared_ptr<Node> variable) {
    return variable->getChildAt(1)->getChildAt(1);
}
