#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>


void try_open_file(void) {
	FILE* file = fopen("secret.txt", "r");

	if (file == NULL) {
		perror("fopen secret.txt");
		return;
	}

	printf("File opened successfully\n");

	if (fclose(file) == EOF) {
		perror("fclose");
	}
}


int main(void) {
	uid_t real_uid = getuid();
	uid_t effect_uid = geteuid();

	printf("Real UID: %lu; Effective UID: %lu\n", (unsigned long)real_uid, (unsigned long)effect_uid);

	try_open_file();

	if (setuid(getuid()) == -1) {
		perror("setuid");
		return 1;
	}

	real_uid = getuid();
        effect_uid = geteuid();

	printf("Real UID: %lu; Effective UID: %lu\n", (unsigned long)real_uid, (unsigned long)effect_uid);
	
	try_open_file();

	return 0;
}
