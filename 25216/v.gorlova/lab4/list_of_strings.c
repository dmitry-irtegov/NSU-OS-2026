#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LENGTH_STRING 1024

typedef struct node{
    char *string;
    struct node *next;
} node;

node* create_node(const char *str){
    node *new_node=(node*)malloc(sizeof(node));
    if (new_node==NULL){
        fprintf(stderr, "Memory allocation error\n");
        exit(1);
    }
    size_t length=strlen(str);
    new_node->string=(char*)malloc(length+1);
    if (new_node->string==NULL){
        fprintf(stderr, "Memory allocation error\n");
        free(new_node);
        exit(1);
    }
    strcpy(new_node->string, str);
    new_node->next=NULL;
    return new_node;
}

void append_node(node **head, const char *str){
    node *new_node=create_node(str);
    if (*head==NULL) *head=new_node;
    else{
        node *current=*head;
        while (current->next!=NULL){
            current=current->next;
        }
        current->next=new_node;
    }
}

void print_list(node *head){
    node *current=head;
    while (current!=NULL){
        printf("%s", current->string);
        current=current->next;
    }
}

void free_list(node *head){
    node *current=head;
    while (current!=NULL){
        node *next=current->next;
        free(current->string);
        free(current);
        current=next;
    }
}

int main(){
    node *head=NULL;
    int flag_long_string=0;
    char buffer[MAX_LENGTH_STRING];
    printf("Input:\n");
    while (1){
        if (strlen(buffer)==MAX_LENGTH_STRING-1 && buffer[MAX_LENGTH_STRING-2]!='\n') flag_long_string=1;
        else flag_long_string=0;
        if (fgets(buffer, sizeof(buffer), stdin)==NULL){
            fprintf(stderr, "EOF\n");
            free_list(head);
            exit(1);
        }
        if (buffer[0]=='.' && flag_long_string==0 && strlen(buffer)==2) break;
        append_node(&head, buffer);
    }
    printf("Output:\n");
    print_list(head);
    free_list(head);
    return 0;
}