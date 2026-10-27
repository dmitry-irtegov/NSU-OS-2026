#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
    FILE *channel;

    channel = popen("./upper", "w");
    if (channel == NULL) {
        perror("popen");
        exit(1);
    }
    setbuf(channel, NULL);

    char buffer[BUFSIZ];
    ssize_t count;
    while ((count = read(STDIN_FILENO, buffer, sizeof(buffer))) != 0) {
        if (count == -1) {
            perror("read");
            exit(1);
        }

        if (fwrite(buffer, 1, (size_t)count, channel) != (size_t)count) {
            perror("fwrite");
            exit(1);
        }
    }

    pclose(channel);
    return 0;
}
