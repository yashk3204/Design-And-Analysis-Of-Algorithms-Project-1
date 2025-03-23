#include <iostream>
#include <vector>
#include <unordered_set>
#include <algorithm>
#include <fstream>
#include <chrono>

using namespace std;
using namespace std::chrono;
ofstream outfile("test-output.txt");

vector<unordered_set<int>> adj;
unordered_set<int> Q;
int maxCliqueSize = 0;
int cliqueCount = 0;
unordered_set<int> maxClique;
vector<int> sizes(500, 0);

void addEdge(int u, int v) {
    adj[u].insert(v);
    adj[v].insert(u);
}

void expand(unordered_set<int> SUBG, unordered_set<int> CAND) {

    if (SUBG.empty()) {
        if(Q.size() > maxCliqueSize) {
            maxCliqueSize = Q.size();
            maxClique.clear();
            for(int v : Q) maxClique.insert(v);
        }
        cliqueCount++;
        sizes[Q.size()]++;
        outfile << "Clique: { ";
        for (int v : Q) outfile << v << " ";
        outfile << "}" << endl;
        return;
    }
    
    int u = *max_element(SUBG.begin(), SUBG.end(), [&](int a, int b) {
        return (CAND.count(a) ? adj[a].size() : 0) < (CAND.count(b) ? adj[b].size() : 0);
    });
    
    unordered_set<int> EXTU;
    for (int v : CAND) {
        if (!adj[u].count(v)) {
            EXTU.insert(v);
        }
    }
    
    while (!EXTU.empty()) {

        int q = *EXTU.begin();
        EXTU.erase(q);
        Q.insert(q);
        
        unordered_set<int> SUBGq, CANDq;
        for (int v : SUBG) {
            if (adj[q].count(v)) {
                SUBGq.insert(v);
            }
        }
        for (int v : CAND) {
            if (adj[q].count(v)) {
                CANDq.insert(v);
            }
        }
        
        expand(SUBGq, CANDq);
        CAND.erase(q);
        Q.erase(q);
        
    }
}

void findCliques(int V) {
    unordered_set<int> vertices;
    for (int i = 0; i < V; i++) vertices.insert(i);
    expand(vertices, vertices);
}

int main() {

    ifstream infile("test-processed.txt");
    if (!infile) {
        cerr << "Error opening file!" << endl;
        return 1;
    }
    int nodes, edges;
    infile >> nodes >> edges;
    adj.resize(nodes);

    for (int i=0; i<edges; i++) {
        int from, to;
        infile >> from >> to;
        addEdge(from, to);
    }
    infile.close();
    
    auto start = high_resolution_clock::now();
    findCliques(nodes);
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end - start);

    outfile << "No. of Cliques = " << cliqueCount << endl;
    outfile << "Largest Clique: Size = " << maxCliqueSize << endl;
    for (int v : maxClique) outfile << v << " ";
    outfile << endl;
    for(int i=1; i<=20; i++) {
        outfile << "No. of cliques of size " << i << " = " << sizes[i] << endl;
    }
    outfile << "Execution Time: " << duration.count() << "ms" << endl;
    return 0;
}