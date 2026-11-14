#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCKET_PATH "/tmp/lab31.sock"
#define BUFFER_SIZE 1024

int main(void) {
  int sock_fd = socket(AF_UNIX, SOCK_STREAM, 0);
  if (sock_fd == -1) {
    perror("socket");
    return 1;
  }

  printf("socket opened\n");

  struct sockaddr_un addr = {0};
  addr.sun_family = AF_UNIX;
  snprintf(addr.sun_path, sizeof(addr.sun_path), "%s", SOCKET_PATH);

  printf("connecting to server...\n");

  if (connect(sock_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
    perror("connect");
    close(sock_fd);
    return 1;
  }

  printf("connected to server\n");

  char buffer[BUFFER_SIZE];

  while (fgets(buffer, sizeof(buffer), stdin) != NULL) {
    size_t len = 0;
    while (buffer[len] != '\0') {
      len++;
    }

    size_t sent = 0;
    while (sent < len) {
      ssize_t n = write(sock_fd, buffer + sent, len - sent);
      if (n == -1) {
        perror("write");
        close(sock_fd);
        return 1;
      }
      if (n == 0) {
        fprintf(stderr, "connection closed\n");
        close(sock_fd);
        return 1;
      }

      sent += (size_t)n;
    }

    printf("message sent: %zu bytes\n", sent);
  }

  close(sock_fd);
  return 0;
}
