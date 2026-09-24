#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STR_LEN (32 * 1024)

#define TEST_MOD 1

typedef struct node {
    char* str;
    struct node *next;
} node_t;

typedef struct list {
    node_t *head;
    node_t *tail;
} list_t;

void init_list(list_t *list) {
    list->head = NULL;
    list->tail = NULL;
}

void add_list(list_t *list, const char *str) {
    node_t *new_node = (node_t *)malloc(sizeof(node_t));
    if (new_node == NULL) {
        printf("malloc error\n");
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }

    size_t len = strlen(str);

    new_node->str = malloc(len + 1);
    if (new_node->str == NULL) {
        printf("malloc error\n");
        perror("malloc failed");
        free(new_node);
        exit(EXIT_FAILURE);
    }

    strcpy(new_node->str, str);
    new_node->next = NULL;

    if (list->head == NULL) {
        list->head = new_node;
        list->tail = new_node;
    } else {
        list->tail->next = new_node;
        list->tail = new_node;
    }
}

int main() {    

    char buffer[MAX_STR_LEN];
    list_t list;
    init_list(&list);

    for (int i = 0; i < MAX_STR_LEN - 2; i++) {
    buffer[i] = 'a';
    }

    buffer[MAX_STR_LEN - 2] = '\n';
    buffer[MAX_STR_LEN - 1] = '\0';

    while (1) {
        if (!TEST_MOD) {
            if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
                break;
            }

            size_t len = strlen(buffer);

            if (buffer[0] == '.' &&
                (len == 1 || (len == 2 && buffer[1] == '\n'))) {
                break;
            }

            if (len > 0 && buffer[len - 1] == '\n') {
                buffer[len - 1] = '\0';
            } else {
                int c;

                while ((c = getchar()) != '\n' && c != EOF);

                fprintf(stderr, "Line is too long\n");
                continue;
            }
        }

        add_list(&list, buffer);
    }
    
    node_t *current = list.head;
    node_t *temp = NULL;

    while (current != NULL) {
        printf("%s\n", current->str);
        free(current->str);

        temp = current;
        current = current->next;
        free(temp);
    }
    return 0;
}