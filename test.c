#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum _type_t {
        UNDEFINED = 0,
        OBJECT = 1,
        ARRAY = 2,
        STRING =3,
        PRIMITIVE = 4;
} type_t;

typedef struct _tok_t{
        enum type_t type;
        int start;
        int end;
        int size; // end - start + 1
	char* value;
} tok_t;

int main(int argc, char **argv)
{
        FILE* fp = fopen(argv[1], "r");

        if(fp == NULL)
        {
                printf("Fail to open!\n");
                return -1;
        }

	/*tok_t* tok;

	if(getc(fp) == '"')
	{
		{
			tok = malloc(sizeof(tok_t));
			tok[0].start = 0;
			tok[0].end = 0;
			tok[0].size = 0;
			tok[0].size++
		}
	}
	*/
        
	if(fgetc(fp)

        fclose(fp);
}
