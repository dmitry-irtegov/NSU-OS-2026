#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct node
{
    char* str;
    struct node* next;
} node;

void add_node(char* arr, node** start, node** end)
{
    node* element = malloc(sizeof(node));
    if (element == NULL)
    {
        perror("malloc");
        exit(1);
    }

    element->str = malloc(strlen(arr) + 1);
    if (element->str == NULL)
    {
        perror("malloc");
        free(element);
        exit(1);
    }

    strcpy(element->str, arr);
    element->next = NULL;

    if (*start == NULL)
    {
        *start = element;
        *end = element;
    }
    else
    {
        (*end)->next = element;
        *end = element;
    }
}

void print_free(node* start)
{
    while (start != NULL)
    {
        node* temp = start;
        printf("%s", start->str);
        start = start->next;
        free(temp->str);
        free(temp);
    }
}

int main()
{
    char arr[4096];
    node* start = NULL;
    node* end = NULL;

    while (1)
    {
        if (fgets(arr, 4096, stdin) == NULL) {
            break;
        }

        if (arr[0] == '.')
        {
            break;
        }

        add_node(arr, &start, &end);
    }

    print_free(start);

    return 0;
}
