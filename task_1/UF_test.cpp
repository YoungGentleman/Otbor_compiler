#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

// Структура для представления дерева
struct Tree {
    int n; // количество вершин
    int root;
    vector<vector<int>> adj; // список смежности (неориентированный)
    vector<vector<int>> children; // список детей (ориентированное дерево)
    vector<int> parent;

    Tree(int n, int root) : n(n), root(root), adj(n + 1), children(n + 1), parent(n + 1, -1) {}

    // Добавление неориентированного ребра
    void addEdge(int u, int v) {
        adj[u].push_back(v);
        adj[v].push_back(u);
    }

    // DFS для укоренения дерева
    void buildTree(int u, int p) {
        parent[u] = p;
        for (int v : adj[u]) {
            if (v != p) { // Если сосед не родитель, значит это ребенок
                children[u].push_back(v);
                buildTree(v, u);
            }
        }
    }
};

int main() {
    int n = 6; // Количество вершин (например, 1-6)
    int root = 1; // Корень
    Tree tree(n, root);

    // Пример координат ребер (например, из файла или ввода)
    vector<pair<int, int>> edges = {{1, 2}, {1, 3}, {2, 4}, {2, 5}, {3, 6}};

    for (auto& edge : edges) {
        tree.addEdge(edge.first, edge.second);
    }

    // Строим дерево, начиная с корня
    tree.buildTree(root, -1);

    // Вывод структуры
    cout << "Дети корня " << root << ": ";
    for (int child : tree.children[root]) {
        cout << child << " ";
    }
    cout << endl;

    return 0;
}
