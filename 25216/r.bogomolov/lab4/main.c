#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Node {
    char *str;
    struct Node *next;
};

int main() {
    char buffer[1024];

    struct Node *head = NULL;
    struct Node *tail = NULL;

    while (fgets(buffer, sizeof(buffer), stdin) != NULL) {
        if (buffer[0] == '.') {
            break;
        }

        size_t len = strlen(buffer);

        char *str = malloc(len + 1);
        if (str == NULL) {
            perror("malloc");
            return 1;
        }

        strcpy(str, buffer);

        while (len > 0 && str[len - 1] != '\n') {
            if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
                break;
            }

            size_t part_len = strlen(buffer);

            char *tmp = realloc(str, len + part_len + 1);
            if (tmp == NULL) {
                perror("realloc");
                free(str);
                return 1;
            }

            str = tmp;

            strcpy(str + len, buffer);
            len += part_len;
        }

        struct Node *node = malloc(sizeof(struct Node));
        if (node == NULL) {
            perror("malloc");
            free(str);
            return 1;
        }

        node->str = str;
        node->next = NULL;

        if (head == NULL) {
            head = node;
            tail = node;
        } else {
            tail->next = node;
            tail = node;
        }
    }

    struct Node *current = head;

    while (current != NULL) {
        printf("%s", current->str);
        current = current->next;
    }

    current = head;

    while (current != NULL) {
        struct Node *next = current->next;

        free(current->str);
        free(current);

        current = next;
    }

    return 0;
}