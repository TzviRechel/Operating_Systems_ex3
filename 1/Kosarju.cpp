#include <iostream>
#include <list>
#include <vector>
using namespace std;

#define WHITE 0
#define GRAY 1
#define BLACK 2
list<int> finish; // Store the vertices by DFS's finish time 


void dfsVisitT(vector<vector<int>>& mat, vector<int>& prev, vector<int>& d, 
                vector<int>& f, vector<int>& color, int& time, int v){

        color[v] = GRAY;
         d[v] = ++time;
         for (size_t i = 0; i < mat.size(); i++)
         {
            if( (mat[i][v]) && (color[i] == WHITE) ){
                prev[i] = v;
                dfsVisitT(mat, prev, d, f, color, time, i);
            }
         }
         color[v] = BLACK;
         f[v] = ++time;
    }


void dfsVisit(vector<vector<int>>& mat, vector<int>& prev, vector<int>& d, 
                vector<int>& f, vector<int>& color, int& time, int v){

        color[v] = GRAY;
         d[v] = ++time;
         for (size_t i = 0; i < mat.size(); i++)
         {
            if( (mat[v][i]) && (color[i] == WHITE) ){
                prev[i] = v;
                dfsVisit(mat, prev, d, f, color, time, i);
            }
         }
         color[v] = BLACK;
         f[v] = ++time;
         finish.push_front(v);
    }

void kosarju(vector<vector<int>>& mat){
    vector<int> prev(mat.size(), -1);
    vector<int> d(mat.size(), -1); // Discovery time
    vector<int> f(mat.size(), -1); // Finish time
    vector<int> color(mat.size(), WHITE);
    int time = 0;  

    // First DFS
    for (size_t i = 0; i < mat.size(); i++)
    {
        if(!color[i]){
            dfsVisit(mat, prev, d, f, color, time, i);
        }  
    }
    
    // Reinitialize the vectors for the second DFS
    for (size_t i = 0; i < mat.size(); i++)
    {
        prev[i] = -1; d[i] = -1; f[i] = -1; color[i] = WHITE;
    }
    time = 0;

    // Second DFS for the transpose graph
    while(!finish.empty())
    {
        int v = finish.front();
         if(!color[v]){
            dfsVisitT(mat, prev, d, f, color, time, v);
        }
        finish.pop_front();  
    }

    // Find the SCCs
    vector<int> SCCs(mat.size(), 0);
    int number = 0;

    // Find the roots
    for (size_t i = 0; i < mat.size(); i++)
    {
        if(prev[i] == -1){
            SCCs[i] = ++number;
        }
    }

    // Find the vertices connected to each root
    for (size_t i = 0; i < mat.size(); i++)
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
        for (size_t j = 0; j < mat.size(); j++)
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
    cout << "Enter the number of vertices and edges (format: n m): " << endl;
    cin >> n >> m;

    if (n <= 0 || m < 0) {
        cerr << "Please provide a positive number of vertices and a non-negative number of edges." << endl;
        return 1;
    }

    // Initialize an adjacency matrix
    vector<vector<int>> adjMatrix(n, vector<int>(n, 0));

    int v, u;
    for (size_t i = 0; i < m; i++) {
        cout << "Enter the vertices for edge " << i + 1 << " (format: v u): " << endl;
        cin >> v >> u;
        if (v < 1 || v > n || u < 1 || u > n) {
            cerr << "Vertices should be between 1 and " << n << "." << endl;
            return 1;
        }
        // Adjust indices to be zero-based for the adjacency matrix
        if(adjMatrix[v-1][u-1]){
            cout << "you chose this edge earlier, choose another edge" << endl;
            i--;
            continue; 
        }
        adjMatrix[v-1][u-1] = 1;
    }

    // // Print the adjacency matrix
    // cout << "Adjacency Matrix:" << endl;
    // for (int i = 0; i < n; i++) {
    //     for (int j = 0; j < n; j++) {
    //         cout << adjMatrix[i][j] << " ";
    //     }
    //     cout << endl;
    // }

    kosarju(adjMatrix);

    return 0;
}
