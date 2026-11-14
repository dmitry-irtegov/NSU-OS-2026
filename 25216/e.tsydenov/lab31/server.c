#include <ctype.h>
#include <poll.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCKET_PATH "/tmp/lab31.sock"
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 10

int main(void) {
  int server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
  if (server_fd == -1) {
    perror("socket");
    return 1;
  }

  printf("socket opened\n");

  struct sockaddr_un addr = {0};
  addr.sun_family = AF_UNIX;
  snprintf(addr.sun_path, sizeof(addr.sun_path), "%s", SOCKET_PATH);

  unlink(SOCKET_PATH);

  if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
    perror("bind");
    close(server_fd);
    return 1;
  }

  printf("socket bound\n");

  if (listen(server_fd, MAX_CLIENTS) == -1) {
    perror("listen");
    close(server_fd);
    unlink(SOCKET_PATH);
    return 1;
  }

  struct pollfd fds[MAX_CLIENTS + 1];

  fds[0].fd = server_fd;
  fds[0].events = POLLIN;

  for (int i = 1; i <= MAX_CLIENTS; i++) {
    fds[i].fd = -1;
    fds[i].events = POLLIN;
  }

  printf("waiting for clients...\n");

  while (1) {
    int ready = poll(fds, MAX_CLIENTS + 1, -1);

    if (ready == -1) {
      perror("poll");
      break;
    }

    if (fds[0].revents & POLLIN) {
      int client_fd = accept(server_fd, NULL, NULL);

      if (client_fd == -1) {
        perror("accept");
      } else {
        int added = 0;

        for (int i = 1; i <= MAX_CLIENTS; i++) {
          if (fds[i].fd == -1) {
            fds[i].fd = client_fd;
            printf("client connected: fd=%d\n", client_fd);
            added = 1;
            break;
          }
        }

        if (!added) {
          printf("too many clients\n");
          close(client_fd);
        }
      }
    }

    for (int i = 1; i <= MAX_CLIENTS; i++) {
      if (fds[i].fd == -1) {
        continue;
      }

      if (fds[i].revents & (POLLIN | POLLHUP | POLLERR)) {
        char buffer[BUFFER_SIZE];

        ssize_t n = read(fds[i].fd, buffer, sizeof(buffer));

        if (n > 0) {
          for (ssize_t j = 0; j < n; j++) {
            putchar(toupper((unsigned char)buffer[j]));
          }
          fflush(stdout);
        } else {
          if (n == -1) {
            perror("read");
          }

          printf("client disconnected: fd=%d\n", fds[i].fd);

          close(fds[i].fd);
          fds[i].fd = -1;
        }
      }
    }
  }

  for (int i = 1; i <= MAX_CLIENTS; i++) {
    if (fds[i].fd != -1) {
      close(fds[i].fd);
    }
  }

  close(server_fd);
  unlink(SOCKET_PATH);

  return 0;
}
