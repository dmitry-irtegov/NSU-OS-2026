#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUF_SIZE 127

typedef struct Node {
    char* str;
    struct Node* next;
} Node;

void add_node(const char* str, size_t str_len, Node** tail) {
    Node* node = (Node*)malloc(sizeof(Node));
    if (node == NULL) {
        perror("malloc");
        exit(1);
    }

    node->str = (char*)malloc(str_len + 1);
    if (node->str == NULL) {
        perror("malloc");
        exit(1);
    }

    strcpy(node->str, str);
    node->next = NULL;

    (*tail)->next = node;
    *tail = node;
}

void print_list(Node* head) {
    Node* node = head->next;
    while (node != NULL) {
        printf("%s\n", node->str);
        node = node->next;
    }
}

int main(void) {
    Node head;
    head.next = NULL;

    Node* tail = &head;

    char buf[BUF_SIZE];

    int is_newline = 1;
    while (1) {
        if (fgets(buf, BUF_SIZE, stdin) == NULL) {
            if (ferror(stdin)) {
                perror("fgets");
                return 1;
            }
            break;
        }

        if (buf[0] == '.' && is_newline) {
            break;
        }

        char *newline_index = strchr(buf, '\n'); 
        if (newline_index != NULL) {
            is_newline = 1;
            *newline_index = '\0';
        } else {
            is_newline = 0;
        }

        size_t len = strlen(buf);
        if (len != 0) {
            add_node(buf, len, &tail);
        }
    }
    print_list(&head);

    return 0;
}
