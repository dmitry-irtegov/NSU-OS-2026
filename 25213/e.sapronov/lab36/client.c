#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <poll.h>

static struct termios saved_terminal;

static void restore_terminal(void)
{
    tcsetattr(STDIN_FILENO, TCSANOW, &saved_terminal);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s http://<url>/<page>\n", argv[0]);
        exit(1);
    }    

    // extract all we need
    char url[100] = {0};
    char port[8] = "80";
    char page[100] = "/";

    if (strncmp(argv[1], "http://", 7) != 0) {
        fprintf(stderr, "no http:// prefix\n");
        exit(1);
    }

    if (strchr(argv[1] + 7, ':') != NULL) {
        sscanf(argv[1], "http://%99[^:]:%7[^/]%99[^\n]", url, port, page);
    } else {
        sscanf(argv[1], "http://%99[^/]%99[^\n]", url, page);
    }

    if (url[0] == '\0') {
        fprintf(stderr, "failed to get url");
        exit(1);
    }

    // attempt connection
    int sockfd;
    struct addrinfo hint, *servinfo, *p;
    
    memset(&hint, 0, sizeof(hint));
    hint.ai_family = AF_UNSPEC;
    hint.ai_socktype = SOCK_STREAM;

    int rv = getaddrinfo(url, port, &hint, &servinfo);
    if (rv != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        exit(1);
    }

    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            perror("socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            perror("connect");
            close(sockfd);
            continue;
        }

        break;
    }
    if (p == NULL) {
        fprintf(stderr, "failed to connect\n");
        exit(1);
    }
    freeaddrinfo(servinfo);
    
    // send our request
    char httpreq[BUFSIZ];
    snprintf(httpreq, BUFSIZ, "GET %s HTTP/1.0\r\nHost: %s:%s\r\n\r\n", page, url, port);

    int tosend = strlen(httpreq);
    int cnt = 0;
    while (cnt < tosend) {
        ssize_t sent = send(sockfd, httpreq + cnt, tosend - cnt, 0);

        if (sent == -1) {
            perror("send");
            exit(1);
        }

        cnt += sent;
    }

    // qol
    int interactive = isatty(STDIN_FILENO) && isatty(STDOUT_FILENO);
    if (interactive) {
        if (tcgetattr(STDIN_FILENO, &saved_terminal) == -1) {
            perror("tcgetattr");
            exit(1);
        }

        if (atexit(restore_terminal) != 0) {
            fprintf(stderr, "failed to register terminal cleanup\n");
            exit(1);
        }

        struct termios mode = saved_terminal;
        mode.c_lflag &= ~(ICANON | ECHO);
        mode.c_cc[VMIN] = 1;
        mode.c_cc[VTIME] = 0;

        if (tcsetattr(STDIN_FILENO, TCSANOW, &mode) == -1) {
            perror("tcsetattr");
            exit(1);
        }
    }

    // poll setup
    struct pollfd fds[2];

    fds[0].fd = sockfd;
    fds[0].events = POLLIN;

    fds[1].fd = interactive ? STDIN_FILENO : -1;
    fds[1].events = POLLIN;

    // main loop
    char separator[] = "\r\n\r\n";
    int matched = 0;

    char recvbuf[BUFSIZ];
    size_t capacity = 512;
    char *pending = malloc(capacity);
    if (!pending) {
        perror("malloc");
        exit(1);
    }
    size_t to_output = 0;
    
    int headers_done = 0;
    int socket_open = 1;
    int keyboard_open = interactive;
    int lines = 0;
    int paused = 0;

    setbuf(stdout, NULL);
    while (socket_open || to_output > 0) {
        if (to_output > 0 && !paused) {
            size_t cnt = 0;
            while (cnt < to_output && (!keyboard_open || lines < 25)) {
                if (pending[cnt++] == '\n' && keyboard_open)
                    lines++;
            }

            if (cnt > 0) {
                if (fwrite(pending, 1, cnt, stdout) != cnt) {
                    perror("stdout");
                    exit(1);
                }

                to_output -= cnt;
                memmove(pending, pending + cnt, to_output);
            }

            if (keyboard_open && lines == 25 && to_output > 0) {
                fprintf(stderr, "Press space to scroll down\n");
                paused = 1;
            }
        }
        
        if (!socket_open && to_output == 0) {
            break;
        }

        int pollrv = poll(fds, 2, -1);
        if (pollrv == -1) {
            perror("poll");
            exit(1);
        } 

        if (fds[1].revents & POLLIN) {
            char key;
            ssize_t n = read(STDIN_FILENO, &key, 1);

            if (n == 0) {
                fds[1].fd = -1;
                keyboard_open = 0;
                paused = 0;
            } else if (n < 0) {
                perror("read");
                exit(1);
            } else if (n == 1 && key == ' ' && paused) {
                lines = 0;
                paused = 0;
            }
        }

        if (fds[0].revents & POLLIN) {
            ssize_t received = recv(sockfd, recvbuf, BUFSIZ, 0);
            if (received == -1) {
                perror("recv");
                exit(1);
            }
            if (received == 0) {
                socket_open = 0;
                fds[0].fd = -1;
                continue;
            }

            // finding \r\n\r\n
            size_t start = 0;
            while (!headers_done && start < received) {
                char byte = recvbuf[start++];
                if (byte == separator[matched]) {
                    matched++;
                } else {
                    matched = (byte == '\r');
                }

                headers_done = (matched == 4);
            }

            size_t cnt = received - start;
            if (cnt == 0) {
                continue;
            }

            size_t req = to_output + cnt;
            if (req > capacity) {
                while (capacity < req) {
                    capacity = capacity * 2;
                }

                char *grown = realloc(pending, capacity);
                if (!grown) {
                    perror("realloc");
                    exit(1);
                }
                pending = grown;
            }

            memcpy(pending + to_output, recvbuf + start, cnt);
            to_output += cnt;
        }
    }

    return 0;
}


