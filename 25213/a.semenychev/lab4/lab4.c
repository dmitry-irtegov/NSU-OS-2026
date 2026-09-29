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
    char buf[1024];

    while (1) {
        if (fgets(buf, sizeof(buf), stdin) == NULL || buf[0] == '.') {
            break;
        }

        size_t len = strlen(buf);

        char *s = malloc(len + 1);
        if (!s) {
            perror("malloc");
            return 1;
        }
        (void) memcpy(s, buf, len + 1);

        struct node *n = malloc(sizeof(*n));
        if (!n) {
            free(s);
            perror("malloc");
            return 1;
        }
        n->str  = s;
        n->next = NULL;

        if (!head) {
            head = n;
            tail = n;
        }
        else {
            tail->next = n;
            tail = n;
        }
    }

    for (struct node *p = head; p; p = p->next) {
        fputs(p->str, stdout);
    }

    while (head) {
        struct node *next = head->next;
        free(head->str);
        free(head);
        head = next;
    }
    return 0;
}
