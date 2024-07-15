#include <iostream>
#include <list>
#include <vector>
#include <sstream>
#include <string>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <poll.h>
#include "Reactor.hpp"

#define PORT "9036"
#define BACKLOG 10
#define MAXDATASIZE 256

using namespace std;

#define WHITE 0
#define GRAY 1
#define BLACK 2

bool Tflag = false;
list<int> finish; // Store the vertices by DFS's finish time 
vector<list<int>> listGraph;
Reactor reactor;


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
    cout << "Edge " << v << "," << u << " added" << endl;
}

void removeEdge(vector<list<int>>& listGraph, int v, int u){

    for (auto it = listGraph[v-1].begin(); it != listGraph[v-1].end(); ++it)
         {
            if(*it == u-1){
                listGraph[v-1].erase(it);
                cout << "Edge " << v << "," << u << " removed" << endl;
                return;
            }
         }     
}

vector<list<int>> newGraph(int n, int m, int fd){
    vector<list<int>> listGraph(n);
    int v, u;
    for (size_t i = 0; i < m; i++) {
       char buf[256];
        int nbytes = recv(fd, buf, sizeof(buf) - 1, 0); // Receive data from client

        if (nbytes <= 0) {
            if (nbytes == 0) {
                printf("Client disconnected.\n");
            } else {
                perror("recv");
            }
            break; // Exit loop or handle disconnect
        }

        buf[nbytes] = '\0'; // Ensure null-termination for string operations
        printf("Received: %s\n", buf); // Print received data for debugging

        // Parse received data (assuming format "v u")
        sscanf(buf, "%d %d", &v, &u);

        newEdge(listGraph, v, u); // Update graph with new edge
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

void handleCommand(const string& command, int fd){
    
    istringstream iss(command);
    string cmd;
    iss >> cmd;
    if(cmd == "Newgraph"){
            int n, m;
            char comma;
            iss >> n >> comma >> m;
            if (n <= 0 || m < 0) {
                cerr << "Please provide a positive number of vertices and a non-negative number of edges." << endl;
                return;
            }      
        listGraph = newGraph(n, m, fd);
        cout << "New graph created" << endl;
        printGraph(listGraph);
    }

    else if(cmd == "Newedge"){
            int v, u;
            char comma;
            iss >> v >> comma >> u;
            newEdge(listGraph, v, u);
            printGraph(listGraph);
    }

    else if(cmd == "Removeedge"){
        int v, u;
            char comma;
            iss >> v >> comma >> u;
            removeEdge(listGraph, v, u);
            printGraph(listGraph);
    }

    else if(cmd == "Kosaraju"){
        kosarju(listGraph);
    }
}


void handleClient(int fd) {
    char buf[MAXDATASIZE];
    int nbytes = recv(fd, buf, MAXDATASIZE-1, 0);
    if (nbytes <= 0) {
        if (nbytes == 0) {
            cout << "Client " << fd <<  " disconnected" << endl;
        } else {
            perror("recv");
        }
        reactor.removeFd(fd);
        close(fd);
    } else {
        buf[nbytes] = '\0';
        handleCommand(buf, fd);
    }
}

void acceptClient(int sockfd) {
    struct sockaddr_storage their_addr;
    socklen_t sin_size = sizeof their_addr;
    int new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
    if (new_fd == -1) {
        perror("accept");
        return;
    }
    cout << "server: new connection" << endl;
    reactor.addFd(new_fd, handleClient);
}

int main() {
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int yes = 1;
    int rv;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
        cerr << "getaddrinfo: " << gai_strerror(rv) << endl;
        return 1;
    }

    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }

    freeaddrinfo(servinfo);

    if (p == NULL) {
        cerr << "server: failed to bind" << endl;
        exit(1);
    }

    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    cout << "server: waiting for connections..." << endl;

    reactor.addFd(sockfd, acceptClient);

    reactor.start();
    reactor.stop();

    close(sockfd);

    return 0;
}