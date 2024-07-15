#include "Reactor.hpp"
#include <algorithm>
#include <iostream>
#include <unistd.h>
#include <errno.h>

Reactor::Reactor() : max_fd(-1), running(false) {
    FD_ZERO(&read_fds);
    functions.resize(FD_SETSIZE, nullptr);
}


int Reactor::addFd(int fd, std::function<void(int)> func) {
    if (fd < 0 || fd >= FD_SETSIZE) {
        return -1;
    }
    FD_SET(fd, &read_fds);
    functions[fd] = func;
    max_fd = std::max(max_fd, fd);
    return 0;
}

int Reactor::removeFd(int fd) {
    if (fd < 0 || fd >= FD_SETSIZE) {
        return -1;
    }
    FD_CLR(fd, &read_fds);
    functions[fd] = nullptr;

    // If the fd being removed is the current max_fd, we need to find the new max_fd
    if (fd == max_fd) {
        max_fd = -1; // Reset max_fd
        for (int i = 20; i >= 0; --i) {
            if (functions[i] != nullptr) {
                max_fd = i; // Update max_fd to the highest active fd
                break;
            }
        }
    }

    //std::cout << "Removed fd: " << fd << ", New max_fd: " << max_fd << std::endl; // Debugging

    return 0;
}

void Reactor::start() {
    running = true;
    while (running) {
        fd_set temp_fds = read_fds;
        int ready = select(max_fd + 1, &temp_fds, nullptr, nullptr, nullptr);
        if (ready == -1) {
            if (errno == EINTR) {
                continue;
            }
            std::cerr << "select error: " << std::endl;
            break;
        }
        for (int fd = 0; fd <= max_fd; ++fd) {
            if (FD_ISSET(fd, &temp_fds)) {
                if (functions[fd]) {
                    functions[fd](fd);
                }
            }
        }
    }
}

void Reactor::stop() {
    running = false;
}