#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define MAX_LINE 1024


struct node {
    char *data;
    struct node *next;
};


int main() {
    char buf[MAX_LINE];
    struct node *head = NULL;
    struct node *tail = NULL;

    while(fgets(buf, MAX_LINE, stdin) != NULL) {
        if(buf[0] == '.'){
            break;
        }

        char *newline = strchr(buf, '\n');
        if (newline != NULL) {
            *newline = '\0';
        }

        size_t len = strlen(buf);
        char *copy = malloc(len + 1);
        strcpy(copy, buf);

        struct node *n = malloc(sizeof(struct node));
        n -> data = copy;
        n -> next = NULL;

        if (head == NULL) {
            head = n;
            tail = n;
        } else {
            tail -> next = n;
            tail = n;
        }
    }


    struct node *p = head;
    while (p != NULL) {
        printf("%s\n", p -> data);
        p = p -> next;
    }

    p = head;
    while (p != NULL) {
        struct node *next = p -> next;
        free(p -> data);
        free(p);
        p = next;
    }

    return 0;
}