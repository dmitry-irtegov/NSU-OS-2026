#include <stdio.h>

int main(void)
{
    FILE *fp;
    const char *text = "Text from parent\n";

    fp = popen("tr a-z A-Z", "w");
    if (fp == NULL) {
        perror("popen");
        return 1;
    }

    if (fputs(text, fp) == EOF) {
        perror("fputs");
        pclose(fp);
        return 1;
    }

    if (pclose(fp) == -1) {
        perror("pclose");
        return 1;
    }

    return 0;
}
