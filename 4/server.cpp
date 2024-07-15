#include <iostream>
#include <vector>
#include <stack>
#include <algorithm>
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

#define PORT "9036"
#define BACKLOG 10

using namespace std;

class Graph {
    int V;
    vector<vector<int>> graph;
    vector<vector<int>> transpose;

public:
    Graph(int vertices) : V(vertices), graph(vertices + 1), transpose(vertices + 1) {}

    void addEdge(int u, int v) {
        graph[u].push_back(v);
        transpose[v].push_back(u);
    }

    void removeEdge(int u, int v) {
        graph[u].erase(remove(graph[u].begin(), graph[u].end(), v), graph[u].end());
        transpose[v].erase(remove(transpose[v].begin(), transpose[v].end(), u), transpose[v].end());
    }

    void dfs(int v, vector<bool>& visited, vector<int>& stack, const vector<vector<int>>& g) {
        visited[v] = true;
        for (int i : g[v]) {
            if (!visited[i]) {
                dfs(i, visited, stack, g);
            }
        }
        stack.push_back(v);
    }

    vector<vector<int>> kosaraju() {
        vector<int> stack;
        vector<bool> visited(V + 1, false);

        for (int i = 1; i <= V; i++) {
            if (!visited[i]) {
                dfs(i, visited, stack, graph);
            }
        }

        fill(visited.begin(), visited.end(), false);

        vector<vector<int>> scc;
        while (!stack.empty()) {
            int i = stack.back();
            stack.pop_back();
            if (!visited[i]) {
                vector<int> component;
                dfs(i, visited, component, transpose);
                scc.push_back(component);
            }
        }

        return scc;
    }
};

Graph* g = nullptr;

void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

string handle_command(const string& command) {
    istringstream iss(command);
    string cmd;
    iss >> cmd;

    if (cmd == "Newgraph") {
        int n, m;
        iss >> n >> m;
        delete g;
        g = new Graph(n);
        for (int i = 0; i < m; i++) {
            int u, v;
            iss >> u >> v;
            g->addEdge(u, v);
        }
        return "New graph created\n";
    } else if (cmd == "Kosaraju") {
        if (g) {
            vector<vector<int>> scc = g->kosaraju();
            string result;
            for (const auto& component : scc) {
                for (int v : component) {
                    result += to_string(v) + " ";
                }
                result += "\n";
            }
            return result;
        }
        return "No graph exists\n";
    } else if (cmd == "Newedge") {
        int i, j;
        char comma;
        iss >> i >> comma >> j;
        if (g) {
            g->addEdge(i, j);
            return "Edge added\n";
        }
        return "No graph exists\n";
    } else if (cmd == "Removeedge") {
        int i, j;
        char comma;
        iss >> i >> comma >> j;
        if (g) {
            g->removeEdge(i, j);
            return "Edge removed\n";
        }
        return "No graph exists\n";
    }
    return "Unknown command\n";
}

int main(void) {
    int sockfd, new_fd;
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr;
    socklen_t sin_size;
    int yes=1;
    char s[INET6_ADDRSTRLEN];
    int rv;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                sizeof(int)) == -1) {
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

    if (p == NULL)  {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }

    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    printf("server: waiting for connections...\n");

    vector<pollfd> pfds;
    pfds.push_back({sockfd, POLLIN, 0});

    while(1) {
        int poll_count = poll(pfds.data(), pfds.size(), -1);

        if (poll_count == -1) {
            perror("poll");
            exit(1);
        }

        for (int i = 0; i < pfds.size(); i++) {
            if (pfds[i].revents & POLLIN) {
                if (pfds[i].fd == sockfd) {
                    sin_size = sizeof their_addr;
                    new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
                    if (new_fd == -1) {
                        perror("accept");
                    } else {
                        pfds.push_back({new_fd, POLLIN, 0});
                        printf("server: new connection\n");
                    }
                } else {
                    char buf[256];
                    int nbytes = recv(pfds[i].fd, buf, sizeof buf, 0);

                    if (nbytes <= 0) {
                        if (nbytes == 0) {
                            printf("pollserver: socket %d hung up\n", pfds[i].fd);
                        } else {
                            perror("recv");
                        }
                        close(pfds[i].fd);
                        pfds.erase(pfds.begin() + i);
                        i--;
                    } else {
                        buf[nbytes] = '\0';
                        string response = handle_command(buf);
                        send(pfds[i].fd, response.c_str(), response.length(), 0);
                    }
                }
            }
        }
    }

    return 0;
}