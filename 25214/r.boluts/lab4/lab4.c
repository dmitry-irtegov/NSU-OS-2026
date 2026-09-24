#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

typedef struct node {
    char* cur_str;
    struct node* nxt_node;
} node;

int main() {
    char buffer[LINE_MAX + 1] = {};
    node* head = NULL;
    node* tail = NULL;

    while (1) {
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            if (feof(stdin)) {
                break;
            }
            perror("fgets");
            break;
        }

        if (buffer[0] == '.') {
            break;
        }

        char* ptr_end_str = strchr(buffer, '\n');
        if (ptr_end_str != NULL) {
            *ptr_end_str = '\0';
        }

        size_t len_string = strlen(buffer);

        node* new_node = malloc(sizeof(node));
        if (new_node == NULL) {
            perror("malloc_node");
            break;
        }
        char* s = malloc(len_string + 1);
        if (s == NULL) {
            perror("malloc_string");
            free(new_node);
            break;
        }
        (void) strcpy(s, buffer);
        new_node->cur_str = s;
        new_node->nxt_node = NULL;
        if (head == NULL) {
            head = new_node;
            tail = new_node;
        } else {
            tail->nxt_node = new_node;
            tail = tail->nxt_node;
        }
    }
    while (head != NULL) {
        printf("%s\n", head->cur_str);
        free(head->cur_str);
        node* nxt_node = head->nxt_node;
        free(head);
        head = nxt_node;
    }
    return 0;
}