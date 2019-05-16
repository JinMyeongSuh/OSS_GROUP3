#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*
typedef enum {
        UNDEFINED = 0,
        OBJECT = 1,
        ARRAY = 2,
        STRING =3,
        PRIMITIVE = 4;
} type_t;

typedef struct {
        enum type_t type;
        int start;
        int end;
        int size; // end - start + 1
} jsmntok_t;
*/
int main(int argc, char **argv)
{
        FILE* fp;

        if(fp == NULL)
        {
                printf("Fail to open!\n");
                return -1;
        }

        fp = fopen(argv[1], "a");
        fprintf(fp, "opensuccess");

        fclose(fp);
}

