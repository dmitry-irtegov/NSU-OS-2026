#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct node {
	char* str;
	struct node* next;
}node;

void free_node(node* head) {
	node* p = head;
	while (p != NULL) {
		node* next = p->next;
		free(p->str);
		free(p);
		p = next;
	}
}

int main() {
	char buffer[1024];

	node* head = NULL;
	node* tail = NULL;

	while (1) {
		if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
			break;
		}
		if (buffer[0] == '.') {
			break;
		}

		char* copy = malloc(strlen(buffer) + 1);
		if (copy == NULL) {
			perror("malloc");
			free_node(head);
			return 1;
		}

		strcpy(copy, buffer);

		node* n = malloc(sizeof(node));
		if (n == NULL) {
			perror("malloc");
			free(copy);
			free_node(head);
			return 1;
		}
		n->str = copy;
		n->next = NULL;

		if (head == NULL) {
			head = n;
			tail = n;
		}
		else {
			tail->next = n;
			tail = n;
		}

	}

	for (node* p = head; p != NULL; p = p->next) {
		printf("%s", p->str);
	}

	free_node(head);

	return 0;
}

