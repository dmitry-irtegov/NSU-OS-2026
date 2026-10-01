#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Node {
    char *str;
    struct Node *next;
};

int main(void)
{
    struct Node *head = NULL;
    struct Node *tail = NULL;

    while (1) {
        int size = 16;
        int len = 0;

        char *buffer = malloc(size);

        if (buffer == NULL)
            return 1;

        while (1) {
            char *result = fgets(buffer + len, size - len, stdin);

            if (result == NULL) {
                free(buffer);
                return 1;
            }

            len = strlen(buffer);

            if (buffer[len - 1] == '\n')
                break;

            char *temp = realloc(buffer, size * 2);

            if (temp == NULL) {
                free(buffer);
                return 1;
            }

            buffer = temp;
            size *= 2;
        }

        if (buffer[0] == '.') {
            free(buffer);
            break;
        }

        struct Node *new_node = malloc(sizeof(struct Node));

        if (new_node == NULL) {
            free(buffer);
            return 1;
        }

        new_node->str = buffer;
        new_node->next = NULL;

        if (head == NULL) {
            head = new_node;
            tail = new_node;
        } else {
            tail->next = new_node;
            tail = new_node;
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