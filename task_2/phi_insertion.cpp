#include "phi_insertion.hpp"
#include <queue>
#include <algorithm>

void PiggyBank::init(int maxLvl) {
    data.assign(maxLvl, {});
    curLvl = maxLvl - 1;
}

void PiggyBank::push(int u, int l) {
    if (l >= 0 && l < (int)data.size()) {
        data[l].push_back(u);
        if (l > curLvl) curLvl = l;
    }
}

int PiggyBank::pop() {
    while (curLvl >= 0 && data[curLvl].empty()) 
        --curLvl;
    if (curLvl < 0) 
        return -1;
    int u = data[curLvl].back();
    data[curLvl].pop_back();
    return u;
}

PhiNodePlacer::PhiNodePlacer(int n, int start) : n(n), start(start), nodes(n) {}

void PhiNodePlacer::addEdge(int u, int v) {
    if (u >= 0 && u < n && v >= 0 && v < n) {
        nodes[u].succ.push_back(v);
        nodes[v].pred.push_back(u);
    }
}

void PhiNodePlacer::setInitialDefs(const std::unordered_set<int>& defs) {
    for (int u : defs) 
        if (u >= 0 && u < n) 
            nodes[u].alpha = true;
}

void PhiNodePlacer::computeDoms() {
    std::unordered_set<int> all;
    for (int i = 0; i < n; ++i) 
        all.insert(i);

    nodes[start].dom = {start};
    for (int i = 0; i < n; ++i) 
        if (i != start) 
            nodes[i].dom = all;

    bool changed = true;
    while (changed) {
        changed = false;
        for (int node = 0; node < n; ++node) {
            if (node == start) 
                continue;
            std::unordered_set<int> newDom = all;
            for (int p : nodes[node].pred) {
                std::unordered_set<int> inter;
                for (int x : nodes[p].dom) 
                    if (newDom.count(x)) 
                        inter.insert(x);
                newDom = std::move(inter);
            }
            newDom.insert(node);
            if (newDom != nodes[node].dom) {
                nodes[node].dom = std::move(newDom);
                changed = true;
            }
        }
    }
}

void PhiNodePlacer::computeLevels() {
    std::vector<int> idom(n, -1);
    std::vector<std::vector<int>> children(n);

    for (int node = 0; node < n; ++node) {
        if (node == start) 
            continue;
        for (int d : nodes[node].dom) {
            if (d == node) 
                continue;
            if (nodes[d].dom.size() == nodes[node].dom.size() - 1) {
                idom[node] = d;
                children[d].push_back(node);
                break;
            }
        }
    }

    std::queue<int> q;
    q.push(start);
    nodes[start].level = 0;
    maxLvl = 1;

    while (!q.empty()) {
        int u = q.front(); q.pop();
        for (int v : children[u]) {
            nodes[v].level = nodes[u].level + 1;
            if (nodes[v].level >= maxLvl) 
                maxLvl = nodes[v].level + 1;
            q.push(v);
        }
    }
}

void PhiNodePlacer::buildDJ() {
    auto strictlyDoms = [&](int u, int v) { 
        return u != v && nodes[v].dom.count(u); 
    };
    for (int u = 0; u < n; ++u) {
        for (int v : nodes[u].succ) {
            nodes[u].dj.push_back({v, !strictlyDoms(u, v)});
        }
    }
}

std::unordered_set<int> PhiNodePlacer::compute() {
    computeDoms();
    computeLevels();
    buildDJ();

    bank.init(maxLvl);
    idf.clear();

    for (int i = 0; i < n; ++i) {
        nodes[i].vis = false;
        nodes[i].inPhi = false;
    }

    for (int i = 0; i < n; ++i) {
        if (nodes[i].alpha) 
            bank.push(i, nodes[i].level);
    }


    int root;
    while ((root = bank.pop()) != -1) {
        nodes[root].vis = true;
        visit(root);
    }

    return idf;
}

void PhiNodePlacer::visit(int root) {
    int rootLvl = nodes[root].level;
    for (const auto& edge : nodes[root].dj) {
        if (edge.isJoin) {
            if (nodes[edge.to].level <= rootLvl && !nodes[edge.to].inPhi) {
                nodes[edge.to].inPhi = true;
                idf.insert(edge.to);
                if (!nodes[edge.to].alpha) bank.push(edge.to, nodes[edge.to].level);
            }
        } else {
            if (!nodes[edge.to].vis) {
                nodes[edge.to].vis = true;
                visit(edge.to);
            }
        }
    }
}