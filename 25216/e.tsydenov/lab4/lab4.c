#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL_CAPACITY 64

typedef struct Node {
    char *value;
    struct Node *next;
} Node;

typedef struct {
    Node *head;
    int size;
} List;

void list_init(List *list) {
    list->head = NULL;
    list->size = 0;
}

void list_push(List *list, const char *str) {
    Node *n = malloc(sizeof(Node));
    n->value = malloc(strlen(str) + 1);
    strcpy(n->value, str);
    n->next = NULL;

    if (list->head == NULL) {
        list->head = n;
    } else {
        Node *last = list->head;
        while (last->next)
            last = last->next;
        last->next = n;
    }
    list->size++;
}

void list_print(const List *list) {
    Node *n;
    for (n = list->head; n; n = n->next)
        puts(n->value);
}

void list_clear(List *list) {
    Node *n = list->head;
    while (n) {
        Node *next = n->next;
        free(n->value);
        free(n);
        n = next;
    }
    list->head = NULL;
    list->size = 0;
}

char *read_line(void) {
    size_t capacity = INITIAL_CAPACITY;
    size_t len = 0;
    char *buf = malloc(capacity);
    if (!buf) return NULL;

    while (fgets(buf + len, (int)(capacity - len), stdin) != NULL) {
        len += strlen(buf + len);

        if (len > 0 && buf[len - 1] == '\n') {
            buf[len - 1] = '\0';
            return buf;
        }

        capacity *= 2;
        char *new_buf = realloc(buf, capacity);
        if (!new_buf) {
            free(buf);
            return NULL;
        }
        buf = new_buf;
    }

    if (len == 0) {
        free(buf);
        return NULL;
    }
    return buf;
}

int main(void) {
    List list;
    list_init(&list);

    char *line;
    while ((line = read_line()) != NULL) {
        int len = strlen(line);
        if (line[0] == '.' && (len == 1 || (len == 2 && line[1] == '\n'))) {
            free(line);
            break;
        }

        list_push(&list, line);
        free(line);
    }

    list_print(&list);
    list_clear(&list);

    return 0;
}