#include <iostream>
#include <list>
#include <vector>
using namespace std;

#define WHITE 0
#define GRAY 1
#define BLACK 2

bool Tflag = false;
list<int> finish; // Store the vertices by DFS's finish time 


void dfsVisit(vector<list<int>>& listGraph, vector<int>& prev, vector<int>& d, 
                vector<int>& f, vector<int>& color, int& time, int v){

        color[v] = GRAY;
         d[v] = ++time;
         for (auto it = listGraph[v].begin(); it != listGraph[v].end(); ++it)
         {
            if(color[*it] == WHITE){
                prev[*it] = v;
                dfsVisit(listGraph, prev, d, f, color, time, *it);
            }
         }
         color[v] = BLACK;
         f[v] = ++time;
         //only in the first DFS
         if(!Tflag){finish.push_front(v);}
         
    }

void kosarju(vector<list<int>>& listGraph){
    vector<int> prev(listGraph.size(), -1);
    vector<int> d(listGraph.size(), -1); // Discovery time
    vector<int> f(listGraph.size(), -1); // Finish time
    vector<int> color(listGraph.size(), WHITE);
    int time = 0;  

    // First DFS
    for (size_t i = 0; i < listGraph.size(); i++)
    {
        if(!color[i]){
            dfsVisit(listGraph, prev, d, f, color, time, i);
        }  
    }
    
    // Transpose the graph
    vector<list<int>> listGraphT(listGraph.size());
    for (size_t i = 0; i < listGraph.size(); i++)
    {
        for (auto it = listGraph[i].begin(); it != listGraph[i].end(); ++it)
         {
            listGraphT[*it].push_back(i);
        }
    }
    Tflag = true;

    // Reinitialize the vectors for the second DFS
    for (size_t i = 0; i < listGraph.size(); i++)
    {
        prev[i] = -1; d[i] = -1; f[i] = -1; color[i] = WHITE;
    }
    time = 0;

    // Second DFS for the transpose graph
    while(!finish.empty())
    {
        int v = finish.front();
         if(!color[v]){
            dfsVisit(listGraphT, prev, d, f, color, time, v);
        }
        finish.pop_front();  
    }

    // Find the SCCs
    vector<int> SCCs(listGraph.size(), 0);
    int number = 0;

    // Find the roots
    for (size_t i = 0; i < listGraph.size(); i++)
    {
        if(prev[i] == -1){
            SCCs[i] = ++number;
        }
    }

    // Find the vertices connected to each root
    for (size_t i = 0; i < listGraph.size(); i++)
    {
        int j = i;
        while(prev[j] != -1){
            j = prev[j];
        }
        SCCs[i] = SCCs[j];
    }

    // Print the SCCs
    for (size_t i = 1; i <= number; i++)
    {
        cout << "SCC " << i << ": ";
        for (size_t j = 0; j < listGraph.size(); j++)
        {
            if(SCCs[j] == i){
                cout << j+1 << " "; // Convert to 1-based index
            }
        }
        cout << endl;   
    }
}


int main() {
    int n, m;
    //cout << "Enter the number of vertices and edges (forlistGraph: n m): " << endl;
    cin >> n >> m;

    if (n <= 0 || m < 0) {
        cerr << "Please provide a positive number of vertices and a non-negative number of edges." << endl;
        return 1;
    }

    // Initialize an List
    vector<list<int>> listGraph(n);

    int v, u;
    for (size_t i = 0; i < m; i++) {
        //cout << "Enter the vertices for edge " << i + 1 << " (forlistGraph: v u): " << endl;
        cin >> v >> u;
        if (v < 1 || v > n || u < 1 || u > n) {
            cerr << "Vertices should be between 1 and " << n << "." << endl;
            return 1;
        }
        // // Adjust indices to be zero-based for the adjacency listGraphrix
        // if(adjlistGraphrix[v-1][u-1]){
        //     cout << "you chose this edge earlier, choose another edge" << endl;
        //     i--;
        //     continue; 
        // }
        listGraph[v-1].push_back(u-1);
    }

    // // Print the listGraph
    // for (int i = 0; i < n; i++) {
    //     cout << i+1 << "--> ";
    //     for (auto it = listGraph[i].begin(); it != listGraph[i].end(); ++it){
    //         cout << *it + 1 << "  ";
    //     }
    //     cout << "\n";
    // }

    kosarju(listGraph);

    return 0;
}