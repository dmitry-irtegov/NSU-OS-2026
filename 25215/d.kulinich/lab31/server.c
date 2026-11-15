#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>
#include <signal.h>
#include <poll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "/tmp/task31.sock"
#define BUFFER_SIZE 4096
#define INITIAL_CAPACITY 16

volatile sig_atomic_t quit_flag = 0;

void sigint_handler(int sig) {
    (void)sig;
    quit_flag = 1;
}

int write_all(int fd, const char *buf, size_t len) {
    size_t written = 0;
    while (written < len) {
        ssize_t res = write(fd, buf + written, len - written);
        if (res == -1) {
            return -1;
        }
        written += (size_t)res;
    }
    return 0;
}

struct sockaddr_un server_addr;
struct sigaction sa;
char buffer[BUFFER_SIZE];

int main(void) {
    struct pollfd *fds = NULL;
    nfds_t nfds = 0;
    nfds_t capacity = INITIAL_CAPACITY;

    sa.sa_handler = sigint_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGINT, &sa, NULL) == -1 || sigaction(SIGTERM, &sa, NULL) == -1) {
        perror("sigaction");
        return 1;
    }

    signal(SIGPIPE, SIG_IGN);

    fds = malloc(capacity * sizeof(*fds));
    if (fds == NULL) {
        perror("malloc");
        return 1;
    }

    int server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket");
        free(fds);
        return 1;
    }

    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);
    
    unlink(SOCKET_PATH);
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof server_addr) == -1) {
        perror("bind");
        close(server_fd);
        free(fds);
        return 1;
    }

    if (listen(server_fd, SOMAXCONN) == -1) {
        perror("listen");
        close(server_fd);
        unlink(SOCKET_PATH);
        free(fds);
        return 1;
    }

    fds[0].fd = server_fd;
    fds[0].events = POLLIN;
    fds[0].revents = 0;
    nfds = 1;


    while (!quit_flag) {
        int ready = poll(fds, nfds, -1);
        if (ready == -1) {
            if (errno == EINTR) {
                continue;
            }
            perror("poll");
            break;
        }

        for (nfds_t i = 1; i < nfds; i++) {
            if (fds[i].revents == 0) {
                continue;
            }

            if (fds[i].revents & (POLLIN | POLLHUP | POLLERR)) {
                ssize_t bytes_read = read(fds[i].fd, buffer, sizeof buffer);

                if (bytes_read > 0) {
                    for (ssize_t j = 0; j < bytes_read; j++) {
                        buffer[j] = (char)toupper((unsigned char)buffer[j]);
                    }
                    if (write_all(STDOUT_FILENO, buffer, (size_t)bytes_read) == -1) {
                        perror("write to stdout");
                        quit_flag = 1;
                        break;
                    }
                } else if (bytes_read == 0 || bytes_read == -1) {
                    if (bytes_read == -1) {
                        perror("read from client");
                    }
                    close(fds[i].fd);

                    fds[i] = fds[nfds - 1];
                    nfds--;
                    i--; 

                    fds[0].events = POLLIN;
                }
            }
        }

        if (fds[0].revents & POLLIN) {
            int client_fd = accept(server_fd, NULL, NULL);
            if (client_fd == -1) {
                if (errno == EMFILE || errno == ENFILE) {
                    fds[0].events = 0;
                } else {
                    perror("accept");
                }
            } else {
                if (nfds == capacity) {
                    nfds_t new_cap = capacity * 2;
                    struct pollfd *new_fds = realloc(fds, new_cap * sizeof(*new_fds));
                    if (new_fds == NULL) {
                        perror("realloc");
                        close(client_fd);
                    } else {
                        fds = new_fds;
                        capacity = new_cap;
                    }
                }

                if (nfds < capacity) {
                    fds[nfds].fd = client_fd;
                    fds[nfds].events = POLLIN;
                    fds[nfds].revents = 0;
                    nfds++;
                }
            }
        }
    }

    for (nfds_t i = 0; i < nfds; i++) {
        close(fds[i].fd);
    }
    free(fds);
    unlink(SOCKET_PATH);

    return 0;
}
