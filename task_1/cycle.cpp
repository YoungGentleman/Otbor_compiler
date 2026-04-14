#include "cycle.hpp"
#include <iostream>
#include <algorithm>
#include <vector>
#include <map>

GraphCreator& GraphCreator::getInstance() {
    static GraphCreator instance;
    return instance;
}

void GraphCreator::reset() {
    for (auto& [k, v] : node_register) delete v;
    node_register.clear();
    root = nullptr;
}

Node* GraphCreator::readEdgesAndBuildGraph(const std::string& input) {
    std::vector<std::pair<std::string, std::string>> edges;
    std::vector<std::string> parsed_str;
    std::string unit;
    for (size_t i = 0; i < input.size(); i++) {
        switch (input[i]) {
            case '{': continue;
            case '}': continue;
            case ' ': continue;
            case ',': 
                parsed_str.push_back(unit); 
                unit = ""; 
                continue;
            default: 
                unit += input[i];
        }
    }
    if (!unit.empty()) parsed_str.push_back(unit);

    for (size_t i = 1; i < parsed_str.size(); i += 2) {
        edges.emplace_back(parsed_str[i-1], parsed_str[i]);
    }
    return buildGraphFromEdges(edges);
}

Node* GraphCreator::getRoot() { return root; }

Node* GraphCreator::buildGraphFromEdges(std::vector<std::pair<std::string, std::string>>& edges) {
    for (auto& [from, to] : edges) {
        if (node_register.count(from) == 0) {
            Node* n1 = new Node(from);
            n1->parent = n1; 
            node_register[from] = n1;
        }
        if (node_register.count(to) == 0) {
            Node* n2 = new Node(to);
            n2->parent = n2; 
            node_register[to] = n2;
        }
        node_register[from]->childs.emplace_back(node_register[to]);
    }
    if (node_register.count("start")) root = node_register["start"];
    return root;
}

Node* GraphCreator::findDFS(int DFS_to_find) {
    for (auto& [key, value] : node_register) {
        if (value->DFS_number == DFS_to_find) return value;
    }
    return nullptr;
}

// --- Graph Implementation ---
Graph::Graph() { this->gc = nullptr; }
void Graph::setChilds(GraphCreator* gc_ptr) { this->gc = gc_ptr; }

void Graph::DFS(Node* node, int& counter, std::map<int, int>& last_descendant) {
    if (!node || node->DFS_number != -1) return;
    int my_num = counter++;
    node->DFS_number = my_num;
    for (auto& child : node->childs) {
        DFS(child, counter, last_descendant);
    }
    last_descendant[my_num] = counter - 1;
}

bool Graph::isAncestor(Node* w, Node* v, const std::map<int, int>& last_descendant) {
    if (!w || !v) return false;
    int w_num = w->DFS_number;
    int v_num = v->DFS_number;
    if (w_num == -1 || v_num == -1) return false;
    auto it = last_descendant.find(w_num);
    if (it == last_descendant.end()) return false;
    return (w_num <= v_num) && (v_num <= it->second);
}


Node* Graph::FIND(Node* v) {
    if (!v) return nullptr;
    if(v != v->parent)
        v->parent = FIND(v->parent);
    return v->parent;
}

void Graph::UNION(Node* x, Node* y) {
    if (!x || !y) return;

    if(x->rank > y->rank)
        std::swap(x, y);

    if(x->rank == y->rank)
        y->rank = y->rank + 1;
        
    x->parent = y;
}

void Graph::analize_loops() {
    if (!gc || !gc->getRoot()) return;

    int counter = 1;
    std::map<int, int> last_descendant;
    DFS(gc->getRoot(), counter, last_descendant);
    int N = counter - 1;
    if (N <= 0) return;

    for (auto& [id, node] : gc->node_register) {
        node->type = "nonheader";
        node->header = 1;
        node->parent = node; 
    }
    if (gc->getRoot()) gc->getRoot()->header = 0;

    std::vector<std::vector<Node*>> backPreds(N + 1);
    std::vector<std::vector<Node*>> nonBackPreds(N + 1);

    for (auto& [id, u] : gc->node_register) {
        if (u->DFS_number < 1 || u->DFS_number > N) continue;
        for (Node* v : u->childs) {
            if (!v || v->DFS_number < 1 || v->DFS_number > N) continue;
            if (isAncestor(v, u, last_descendant)) {
                backPreds[v->DFS_number].push_back(u);
            } else {
                nonBackPreds[v->DFS_number].push_back(u);
            }
        }
    }

    for (int w = N; w >= 1; --w) {
        Node* W = gc->findDFS(w);
        if (!W) continue;

        std::vector<Node*> P;
        std::vector<Node*> worklist;

        
        for (Node* V : backPreds[w]) {
            if (!V) continue;
            Node* rep = FIND(V);
            if (rep && rep != W) {
                bool found = false;
                for (Node* p : P)   
                    if (p == rep) {
                        found = true; 
                        break; 
                    }
                if (!found) 
                    P.push_back(rep);
            }
        }

        if (!P.empty()) {
            W->type = "reducible";
            worklist = P;
        }

        
        while (!worklist.empty()) {
            Node* x = worklist.back();
            worklist.pop_back();
            if (!x) continue;

            int x_dfs = x->DFS_number;
            if (x_dfs < 1 || x_dfs > N) continue;

            for (Node* y : nonBackPreds[x_dfs]) {
                if (!y) continue;
                Node* y_prime = FIND(y);
                if (!y_prime) continue;

                if (!isAncestor(W, y_prime, last_descendant)) {
                    W->type = "irreducible";
                } else {
                    if (y_prime != W) {
                        bool inP = false;
                        for (Node* p : P) 
                            if (p == y_prime) { 
                                inP = true; break;
                            }
                        if (!inP) {
                            P.push_back(y_prime);
                            worklist.push_back(y_prime);
                        }
                    }
                }
            }
        }

        for (Node* x : P) {
            if (x) {
                x->header = w; 
                UNION(x, W);
            }
        }
    }
}