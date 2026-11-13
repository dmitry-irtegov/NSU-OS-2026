#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 4096

struct Node {
    char *line;
    struct Node *next;
};

struct List {
    struct Node *head;
    struct Node *tail;
};

static void list_init(struct List *list) {
    list->head = NULL;
    list->tail = NULL;
}

static int push_in_back(struct List *list, const char *line) {
    int len = strlen(line) + 1;
    char *copy = malloc(len);
    if (copy == NULL) {
        perror("malloc");
        return -1;
    }
    memcpy(copy, line, len);

    struct Node *node = malloc(sizeof(*node));
    if (node == NULL) {
        perror("malloc");
        free(copy);
        return -1;
    }
    node->line = copy;
    node->next = NULL;

    if (list->tail == NULL) {
        list->head = node;
        list->tail = node;
    } else {
        list->tail->next = node;
        list->tail = node;
    }
    return 0;
}

static void free_list(struct List *list) {
    struct Node *list_head = list->head;
    while (list_head != NULL) {
        struct Node *next = list_head->next;
        free(list_head->line);
        free(list_head);
        list_head = next;
    }
    list->head = NULL;
    list->tail = NULL;
}

int main(void) {
    char buf[MAX_LINE];
    struct List list;
    list_init(&list);

    while (1) {
        if (fgets(buf, sizeof(buf), stdin) == NULL) {
            /*if (feof(stdin)) {
                break;
            }*/
            perror("fgets");
            free_list(&list);
            return 1;
        }

        if (buf[0] == '.') {
            break;
        }

        if (push_in_back(&list, buf) != 0) {
            perror("push_in_back");
            free_list(&list);
            return 1;
        }
    }
    
    struct Node *tmp;
    for (tmp = list.head; tmp != NULL; tmp = tmp->next) {
        if (fputs(tmp->line, stdout) == EOF) {
            perror("fputs");
            free_list(&list);
            return 1;
        }
    }

    if (fflush(stdout) == EOF) {
        perror("fflush");
        free_list(&list);
        return 1;
    }

    free_list(&list);
    return 0;
}
