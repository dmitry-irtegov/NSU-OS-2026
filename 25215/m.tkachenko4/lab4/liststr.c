#include <stdio.h>
#include <stdlib.h>
#include <string.h>
struct node {
    char *str;
    struct node *next;
};
int main() {
    struct node *head = NULL;
    struct node *tail = NULL;
    char buf[BUFSIZ];
    while (fgets(buf, sizeof(buf), stdin) != NULL) {
        if (buf[0] == '.')
            break;
        size_t len = strlen(buf);
        if (len > 0 && buf[len - 1] == '\n')
            buf[--len] = '\0';
        char *copy = malloc(len + 1);
        strcpy(copy, buf);
        struct node *n = malloc(sizeof(*n));
        n->str = copy;
        n->next = NULL;
        if (tail != NULL)
            tail->next = n;
        else
            head = n;
        tail = n;
    }
    for (struct node *p = head; p != NULL; p = p->next)
        printf("%s\n", p->str);
    while (head != NULL) {
        struct node *next = head->next;
        free(head->str);
        free(head);
        head = next;
    }
    return 0;
}