#ifndef REACTOR_HPP
#define REACTOR_HPP

#include <functional>
#include <vector>
#include <sys/select.h>

class Reactor {
public:
    Reactor();

    int addFd(int fd, std::function<void(int)> func);
    int removeFd(int fd);
    void start();
    void stop();

private:
    fd_set read_fds;
    int max_fd;
    std::vector<std::function<void(int)>> functions;
    bool running;
};

#endif // REACTOR_HPP