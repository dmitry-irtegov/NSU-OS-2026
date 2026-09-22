#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE BUFSIZ

typedef struct Node {
    char *data;
    struct Node *next;
} node_t;

int main(void) {
    char buffer[BUFFER_SIZE];
    node_t *head = NULL;
    node_t *tail = NULL;
    int is_start_of_line = 1;
    int exit_code = EXIT_SUCCESS;

    printf("Enter strings (type '.' at the beginning of a line to finish):\n");

    while (fgets(buffer, sizeof(buffer), stdin) != NULL) {
        size_t len;
        node_t *new_node;

        if (is_start_of_line && buffer[0] == '.') {
            break;
        }

        len = strlen(buffer);
        is_start_of_line = (len > 0 && buffer[len - 1] == '\n');

        new_node = (node_t *)malloc(sizeof(node_t));
        if (new_node == NULL) {
            perror("Error: node malloc failed");
            exit_code = EXIT_FAILURE;
            goto cleanup; 
        }

        new_node->data = (char *)malloc(sizeof(char) * (len + 1));
        if (new_node->data == NULL) {
            perror("Error: string malloc failed");
            free(new_node);
            exit_code = EXIT_FAILURE;
            goto cleanup;
        }

        strcpy(new_node->data, buffer);
        new_node->next = NULL;

        if (head == NULL) {
            head = new_node;
        } else {
            tail->next = new_node;
        }
        tail = new_node;
    }

    if (ferror(stdin)) {
        perror("Error reading stdin");
        exit_code = EXIT_FAILURE;
    }

    printf("\n--- List contents ---\n");
    {
        node_t *current = head;
        while (current != NULL) {
            fputs(current->data, stdout);
            current = current->next;
        }
    }

cleanup:
    {
        node_t *current = head;
        node_t *next_node;
        while (current != NULL) {
            next_node = current->next;
            free(current->data);
            free(current);
            current = next_node;
        }
    }

    return exit_code;
}