#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXLINE 4096

struct node {
    char        *str;
    struct node *next;
};

static struct node *new_node(const char *s, size_t len)
{
    struct node *n;

    n = (struct node *) malloc(sizeof(struct node));
    if (n == NULL)
        return NULL;

    n->str = (char *) malloc(len + 1);
    if (n->str == NULL) {
        free(n);
        return NULL;
    }
    memcpy(n->str, s, len);
    n->str[len] = '\0';
    n->next = NULL;
    return n;
}

static void free_list(struct node *head)
{
    struct node *tmp;

    while (head != NULL) {
        tmp = head->next;
        free(head->str);
        free(head);
        head = tmp;
    }
}

int main(void)
{
    char         buf[MAXLINE];
    struct node *head = NULL;
    struct node *tail = NULL;
    struct node *n;
    size_t       len;

    while (fgets(buf, sizeof(buf), stdin) != NULL) {
        if (buf[0] == '.')
            break;

        len = strlen(buf);

        n = new_node(buf, len);
        if (n == NULL) {
            perror("malloc");
            free_list(head);
            return 1;
        }

        if (head == NULL) 
            head = n;
        else
            tail->next = n;
        tail = n;
    }

    if (ferror(stdin)) {
        perror("fgets");
        free_list(head);
        return 1;
    }

    for (n = head; n != NULL; n = n->next)
        fputs(n->str, stdout);

    free_list(head);
    return 0;
}