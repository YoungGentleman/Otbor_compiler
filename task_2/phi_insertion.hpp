#pragma once
#include <vector>
#include <unordered_set>

struct DJEdge { 
    int to; 
    bool isJoin;                                // J-edge == true, D-edge == false 
};

struct Node {
    int level = 0;                  
    bool vis = false;
    bool inPhi = false;
    bool alpha = false;

    std::vector<int> succ;                      // childs in CFG
    std::vector<int> pred;

    std::vector<DJEdge> dj;                     // DJedges from this node
    std::unordered_set<int> dom;                // all the dominators of this node
};

struct PiggyBank {
    int curLvl = -1;    
    std::vector<std::vector<int>> data;         // data[lvl]
    void init(int maxLvl);
    void push(int u, int l);
    int pop();
};

class PhiNodePlacer {
    int n;
    int start;
    int maxLvl = 0;
    std::vector<Node> nodes;
    PiggyBank bank;
    std::unordered_set<int> idf;

    void computeDoms();                         
    void computeLevels();
    void buildDJ();
    void visit(int root);

public:
    PhiNodePlacer(int n, int start);
    void addEdge(int u, int v);
    void setInitialDefs(const std::unordered_set<int>& defs);
    std::unordered_set<int> compute();
};