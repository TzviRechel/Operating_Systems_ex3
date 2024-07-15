#include "Proactor.hpp"
#include <algorithm>
#include <unistd.h>


struct ThreadArgs {
    int sockfd;
    proactorFunc func;
};

void* threadWrapper(void* arg) {
    ThreadArgs* args = static_cast<ThreadArgs*>(arg);
    args->func(args->sockfd);
    delete args;
    return nullptr;
}

pthread_t startProactor(int sockfd, proactorFunc threadFunc) {
    pthread_t tid;
    ThreadArgs* args = new ThreadArgs{sockfd, threadFunc};
    if (pthread_create(&tid, nullptr, threadWrapper, args) != 0) {
        delete args;
        return 0;
    }   
    return tid;
}

int stopProactor(pthread_t tid) {
    pthread_join(tid, nullptr);
    return 0;
}
