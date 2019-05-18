#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum _type_t {
        UNDEFINED = 0,
        OBJECT = 1,
        ARRAY = 2,
        STRING =3,
        PRIMITIVE = 4
} type_t;

typedef struct _tok_t{
        type_t type;
        int start;
        int end;
        int size; // end - start + 1
	//char* value;
} tok_t;

int main(int argc, char **argv)
{
        FILE* fp = fopen(argv[1], "r");

        if(fp == NULL)
        {
                printf("Fail to open!\n");
                return -1;
        }

	int size = 0;
	char* ref = NULL;
	tok_t* tok = NULL

	fseek(fp, 0, SEEK_END);
	size = ftell(fp);

	ref = malloc(sizeof(char)*(size + 1));
	
	fseek(fp, 0, SEEK_SET);

	fread(ref, size, sizeof(char), fp);

	printf("%s\n", ref);


	free(ref);

        fclose(fp);
}
void parseJSON(char* ref, int size, tok_t* tok)
{
	int v_size = 0;
	int numOftok = 0;

	for(int i = 0; i < size; i++)
	{
		if(ref[0] == '{')
		{
			printf("Syntax error!");
			return;
		}
	
		switch(ref[i])
		{
		case '"':
		{
			tok = realloc(sizeof(tok) + numOftok);
			tok[i].start = i + 1;
			
			if((strchr(tok[i].start, '"') - ref) > (strchr(tok[i].start, ':') - ref) || strchr(tok[i].start, '"') == NULL)
			{
				printf("Syntax error!");
				return;
			}
			else
				tok[i].end = strchr(tok[i].start, '"') - ref;

			v_size = tok[i].start - tok[i].end + 1;
			
			tok[i].value = malloc(sizeof(char)*v_size);


			for(int j = 0; j < v_size; j++)
			{
				tok[i].value + j = ref[tok[i].start + j];
			}

			i = i + v_size;
		}
		}
	}
}
