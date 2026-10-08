#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 128

typedef struct Node {
    char *data;
    struct Node *next;
} Node;

int main(void) {
    char chunk[BUFFER_SIZE];
    Node *head = NULL;
    Node *tail = NULL;
    int flag = 0;

    while (!flag) {
        char *current_str = NULL;
        size_t current_len = 0;
        int line_complete = 0;
        
        while (!line_complete && fgets(chunk, sizeof(chunk), stdin) != NULL) {
            
            if (current_len == 0 && chunk[0] == '.') {
                flag = 1;
                break;
            }

            size_t chunk_len = strlen(chunk);
            
            if (chunk_len > 0 && chunk[chunk_len - 1] == '\n') {
                line_complete = 1;
            }
            
            char *new_ptr = (char *)realloc(current_str, current_len + chunk_len + 1);
            if (!new_ptr) {
                perror("realloc");
                free(current_str);
                
                while (head != NULL) {
                    Node *tmp = head;
                    head = head->next;
                    free(tmp->data);
                    free(tmp);
                }
                return EXIT_FAILURE;
            }
            current_str = new_ptr;

            strcpy(current_str + current_len, chunk);
            current_len += chunk_len;
        }
        
        if (flag || current_len == 0) {
            free(current_str);
        } else {
            Node *node = (Node *)malloc(sizeof(Node));
            if (!node) {
                perror("malloc");
                free(current_str);

                while (head != NULL) {
                    Node *tmp = head;
                    head = head->next;
                    free(tmp->data);
                    free(tmp);
                }
                return EXIT_FAILURE;
            }

            node->data = current_str;
            node->next = NULL;

            if (!head) {
                head = node;
                tail = node;
            } else {
                tail->next = node;
                tail = node;
            }
        }
    }
    
    for (Node *curr = head; curr != NULL; curr = curr->next) {
        printf("%s", curr->data);
    }
    
    while (head != NULL) {
        Node *tmp = head;
        head = head->next;
        free(tmp->data);
        free(tmp);
    }

    return EXIT_SUCCESS;
}