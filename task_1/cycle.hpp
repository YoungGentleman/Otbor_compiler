#pragma once
#include <string>
#include <vector>
#include <map>
#include <iostream>

#define MAX 1000

struct Node {
    std::string id;
    std::string type = "nonheader";
    int DFS_number = -1;
    int rank = 0;
    int header = 1;
    
    Node* parent = nullptr;
    std::vector<Node*> childs;
    
    Node(std::string id_) : id(std::move(id_)) {}
    Node(const Node&) = delete;
    Node& operator=(const Node&) = delete;
};

class GraphCreator {
private:
    GraphCreator() = default;
    Node* root = nullptr;
    
public:
    std::map<std::string, Node*> node_register;  
    
    GraphCreator& operator=(const GraphCreator&) = delete;
    GraphCreator(const GraphCreator&) = delete;
    
    static GraphCreator& getInstance();
    
    Node* buildGraphFromEdges(std::vector<std::pair<std::string, std::string>>& edges);
    Node* readEdgesAndBuildGraph(const std::string& input);
    Node* getRoot();
    Node* findDFS(int DFS_to_find);
    void reset();
};

class Graph {
private:
    GraphCreator* gc = nullptr;
    
    void DFS(Node* node, int& counter, std::map<int, int>& last_descendant);
    
public:
    Graph();
    void setChilds(GraphCreator* gc);

    Node* FIND(Node* v);
    void UNION(Node* x, Node* y);
    bool isAncestor(Node* w, Node* v, const std::map<int, int>& last_descendant);
    
    void analize_loops();
};