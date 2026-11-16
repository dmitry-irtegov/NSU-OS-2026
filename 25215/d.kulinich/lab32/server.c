#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <aio.h>
#include <stropts.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/filio.h>
#include <sys/ioctl.h>

#define SOCKET_PATH "/tmp/task32.sock"
#define BUFFER_SIZE 4096

// linked list with ctx
struct client_ctx {
    int fd;
    struct aiocb cb;
    char buffer[BUFFER_SIZE];
    struct client_ctx *next;
};

volatile sig_atomic_t quit_flag = 0;
volatile sig_atomic_t got_sigio = 0;

void sigint_handler(int sig) {
    (void)sig;
    quit_flag = 1;
}

void sigio_handler(int sig) {
    (void)sig;
    got_sigio = 1;
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

// subscribe to AIO
int start_client_aio(struct client_ctx *client) {
    client->cb.aio_fildes = client->fd;
    client->cb.aio_buf = client->buffer;
    client->cb.aio_nbytes = sizeof client->buffer;
    client->cb.aio_offset = 0;

    client->cb.aio_sigevent.sigev_notify = SIGEV_SIGNAL;
    client->cb.aio_sigevent.sigev_signo = SIGIO;
    client->cb.aio_sigevent.sigev_value.sival_ptr = client;

    return aio_read(&client->cb);
}

struct sockaddr_un server_addr;
struct sigaction sa;

int main(void) {
    struct client_ctx *clients_head = NULL;
    int on = 1;

    memset(&sa, 0, sizeof sa);
    sa.sa_handler = sigint_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGINT, &sa, NULL) == -1 || sigaction(SIGTERM, &sa, NULL) == -1) {
        perror("sigaction SIGINT/SIGTERM");
        return 1;
    }

    memset(&sa, 0, sizeof sa);
    sa.sa_handler = sigio_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGIO, &sa, NULL) == -1) {
        perror("sigaction SIGIO");
        return 1;
    }

    signal(SIGPIPE, SIG_IGN);

    sigset_t block_mask, empty_mask;
    sigemptyset(&block_mask);
    sigaddset(&block_mask, SIGIO);
    sigaddset(&block_mask, SIGINT);
    sigaddset(&block_mask, SIGTERM);
    if (sigprocmask(SIG_BLOCK, &block_mask, NULL) == -1) {
        perror("sigprocmask");
        return 1;
    }
    sigemptyset(&empty_mask);

    int server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket");
        return 1;
    }

    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

    unlink(SOCKET_PATH);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof server_addr) == -1) {
        perror("bind");
        close(server_fd);
        return 1;
    }

    if (listen(server_fd, SOMAXCONN) == -1) {
        perror("listen");
        close(server_fd);
        unlink(SOCKET_PATH);
        return 1;
    }

    // O_NONBLOCK to socket
    int flags = fcntl(server_fd, F_GETFL, 0);
    if (flags == -1 || fcntl(server_fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("fcntl O_NONBLOCK");
        close(server_fd);
        unlink(SOCKET_PATH);
        return 1;
    }
    if (fcntl(server_fd, F_SETOWN, getpid()) == -1) {
        perror("fcntl F_SETOWN");
    }
    if (ioctl(server_fd, FIOASYNC, &on) == -1) {
        // for STREAMS
        (void)ioctl(server_fd, I_SETSIG, S_INPUT | S_RDNORM);
    }



    while (!quit_flag) {
        if (!got_sigio && !quit_flag) {
            sigsuspend(&empty_mask);
        }
        if (quit_flag) {
            break;
        }
        got_sigio = 0;

        for (;;) {
            int client_fd = accept(server_fd, NULL, NULL);
            if (client_fd == -1) {
                if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    break;
                }
                perror("accept");
                break;
            }

            // on Solaris socket inherits O_NONBLOCK
            // we should reset it manually
            int client_flags = fcntl(client_fd, F_GETFL, 0);
            if (client_flags == -1) {
                perror("fcntl F_GETFL client");
                close(client_fd);
                continue;
            }
            if (fcntl(client_fd, F_SETFL, client_flags & ~O_NONBLOCK) == -1) {
                perror("fcntl F_SETFL client");
                close(client_fd);
                continue;
            }

            struct client_ctx *new_client = calloc(1, sizeof(*new_client));
            if (new_client == NULL) {
                perror("calloc");
                close(client_fd);
                continue;
            }

            new_client->fd = client_fd;
            if (start_client_aio(new_client) == -1) {
                perror("aio_read");
                close(client_fd);
                free(new_client);
                continue;
            }

            new_client->next = clients_head;
            clients_head = new_client;
        }

        struct client_ctx **curr = &clients_head;
        while (*curr != NULL) {
            struct client_ctx *client = *curr;
            int err = aio_error(&client->cb);

            if (err == EINPROGRESS) {
                curr = &client->next;
                continue;
            }

            if (err == 0) {
                ssize_t bytes_read = aio_return(&client->cb);

                if (bytes_read > 0) {
                    for (ssize_t i = 0; i < bytes_read; i++) {
                        client->buffer[i] = (char)toupper((unsigned char)client->buffer[i]);
                    }
                    if (write_all(STDOUT_FILENO, client->buffer, (size_t)bytes_read) == -1) {
                        perror("write to stdout");
                        quit_flag = 1;
                        break;
                    }

                    if (start_client_aio(client) == 0) {
                        curr = &client->next;
                        continue;
                    }
                    perror("aio_read restart");
                }
            } else {
                errno = err;
                perror("aio_error");
            }

            *curr = client->next;
            close(client->fd);
            free(client);
        }
    }

    while (clients_head != NULL) {
        struct client_ctx *next = clients_head->next;
        aio_cancel(clients_head->fd, &clients_head->cb);
        close(clients_head->fd);
        free(clients_head);
        clients_head = next;
    }

    close(server_fd);
    unlink(SOCKET_PATH);

    return 0;
}
