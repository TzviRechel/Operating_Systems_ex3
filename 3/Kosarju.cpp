#include <iostream>
#include <list>
#include <vector>
#include <sstream>
using namespace std;

#define WHITE 0
#define GRAY 1
#define BLACK 2

bool Tflag = false;
list<int> finish; // Store the vertices by DFS's finish time 


void printGraph(vector<list<int>>& listGraph){
     for (int i = 0; i < listGraph.size(); i++) {
        cout << i+1 << "--> ";
        for (auto it = listGraph[i].begin(); it != listGraph[i].end(); ++it){
            cout << *it + 1 << "  ";
        }
        cout << "\n";
    }
}


void newEdge(vector<list<int>>& listGraph, int v, int u){
    
    listGraph[v-1].push_back(u-1);
}

void removeEdge(vector<list<int>>& listGraph, int v, int u){

    for (auto it = listGraph[v].begin(); it != listGraph[v].end(); ++it)
         {
            if(*it == u){
                listGraph[v].erase(it);
            }
         }
}

vector<list<int>> newGraph(int n, int m){
    vector<list<int>> listGraph(n);
    int v, u;
    for (size_t i = 0; i < m; i++) {
        //cout << "Enter the vertices for edge " << i + 1 << " (forlistGraph: v u): " << endl;
        cin >> v >> u;
        newEdge(listGraph, v, u);
    }

    return listGraph;
}


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
    Tflag = false;
}


int main() {

    vector<list<int>> listGraph;
    string command;
    while(true){
        getline(cin, command);
        istringstream iss(command);
        iss >> command;
        if(command == "Newgraph"){
             int n, m;
             char comma;
             iss >> n >> comma >> m;
             if (n <= 0 || m < 0) {
            cerr << "Please provide a positive number of vertices and a non-negative number of edges." << endl;
            return 1;
            }      
            listGraph = newGraph(n, m);
            printGraph(listGraph);
        }

        else if(command == "Newedge"){
             int v, u;
             char comma;
             iss >> v >> comma >> u;
             newEdge(listGraph, v, u);
             printGraph(listGraph);
        }

        else if(command == "Removeedge"){
            int v, u;
             char comma;
             iss >> v >> comma >> u;
             removeEdge(listGraph, v, u);
             printGraph(listGraph);
        }

        else if(command == "Kosaraju"){
            kosarju(listGraph);
        }


    }

    return 0;
}