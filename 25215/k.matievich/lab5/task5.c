#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


typedef struct stroke {
    off_t position;
    size_t len;
} stroke_t;

typedef struct stroke_table {
     stroke_t* table;
     size_t cap;
     size_t size;
} str_tab;


int main(int argc, char** argv) {


    if (argc != 2) {
        write(2, "Unknown command. Correct use is: \"./task5 <file_name>\n",
            strlen("Unknown command. Correct use is: \"./task5 <file_name>\n"));
        return 1;
    }

    str_tab* table = (str_tab*)malloc(sizeof(str_tab));
    table->cap = 16;
    table->table = (stroke_t*)malloc(table->cap * sizeof(stroke_t));
    table->size = 0;

    char* fileName = argv[1];
    int fildes = open(fileName, O_RDONLY);

    if (fildes == -1) {
        write(2, "There's no such file in this directory\n", strlen("There's no such file in this directory\n"));
        free(table->table);
        free(table);
        return 2;
    }

    
    char buf[BUFSIZ];
    ssize_t bytesRead;
    off_t offset = 0;
    size_t curSize = 0;
    off_t curOfffset = 0;


    bytesRead = read(fildes, buf, BUFSIZ - 1);

    if (bytesRead == -1) {
        write(2, "Не удалость прочитать файл\n", strlen("Не удалость прочитать файл\n"));
        free(table->table);
        free(table);
        close(fildes);
        return 1;
    }


    while (bytesRead > 0) {
        buf[bytesRead] = '\0';
        for (int i = 0; i < bytesRead; i++) {
            curSize++;
            if (buf[i] == '\n') {

                if (table->size == table->cap-1) {
                    table->cap *= 2;
                    table->table = (stroke_t*)realloc(table->table, table->cap*sizeof(stroke_t));
                }

                table->table[table->size].len = curSize;
                curSize = 0;
                table->table[table->size].position = curOfffset;
                curOfffset = offset + 1;
                table->size++;
            }
            offset++;
        }

        bytesRead = read(fildes, buf, BUFSIZ - 1);
        if (bytesRead == -1) {
            write(2, "Не удалость прочитать файл\n", strlen("Не удалость прочитать файл\n"));
            free(table->table);
            free(table);
            close(fildes);
        return 1;
    }
    }

    if (curSize != 0) {
        if (table->size == table->cap-1) {
            table->cap *= 2;
            table->table = (stroke_t*)realloc(table->table, table->cap*sizeof(stroke_t));
        }

        table->table[table->size].len = curSize;
        table->table[table->size].position = curOfffset;
        table->size++;
    }


    write(1, "Enter the line number for which you want to get information or \"0\" to exit.\n",
         strlen("Enter the line number for which you want to get information or \"0\" to exit.\n"));

    char inputBuf[BUFSIZ];
    long long lineNum;
    
    while ((bytesRead = read(0, inputBuf ,BUFSIZ - 1)) > 0) {


        inputBuf[bytesRead] = '\0';

        char* endPtr;

        lineNum = strtoll(inputBuf, &endPtr, 10);

        if (endPtr == inputBuf || (*endPtr != '\n' && *endPtr != '\0')) {
            write(2, "Incorrect input. Try again\n", strlen("Incorrect input. Try again\n"));
            write(1, "Enter the line number for which you want to get information or \"0\" to exit.\n",
                 strlen("Enter the line number for which you want to get information or \"0\" to exit.\n"));
            continue;
        }


        if (lineNum == 0) {
            write(1, "Exiting the programm\n", strlen("Exiting the programm\n"));
            break;
        }

        if (lineNum < 0 || lineNum > (long long)table->size) {
            write(2, "There's no such line in this file. Try again\n", strlen("There's no such line in this file. Try again\n"));
            write(1, "Enter the line number for which you want to get information or \"0\" to exit.\n",
                 strlen("Enter the line number for which you want to get information or \"0\" to exit.\n"));
            continue;
        }


        off_t newOffset = table->table[lineNum-1].position;
        size_t lineSize = table->table[lineNum-1].len;
        lseek(fildes, newOffset, SEEK_SET);
        char* newBuf = (char*)malloc((lineSize + 3)*sizeof(char));

        ssize_t newBytesRead = read(fildes, newBuf, lineSize);

        if (newBytesRead < 0) {
            write(2, "Ошибка вывода строки\n", strlen("Ошибка вывода строки\n"));
            free(newBuf);
            break;
        }

        newBuf[newBytesRead] = '\0';
        printf("%s\n", newBuf);

        free(newBuf);

        write(1, "Enter the line number for which you want to get information or \"0\" to exit.\n",
          strlen("Enter the line number for which you want to get information or \"0\" to exit.\n"));
    }

    free(table->table); 
    free(table);
    close(fildes);
    return 0;
}