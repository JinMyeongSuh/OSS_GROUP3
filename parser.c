#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NUM_OF_TOK 1024

typedef enum _type_t {
	UNDEFINED = 0,
	OBJECT = 1,
	ARRAY = 2,
	STRING = 3,
	PRIMITIVE = 4
} type_t;

typedef struct _tok_t {
	type_t type;
	int start;
	int end;
	int size; // end - start + 1
} tok_t;

int numOftok = 0;

//void readFile(char* filename, char** doc, int* size);
tok_t* parseJSON(char* ref, int size, tok_t* tok);
void printTokInfo(tok_t* tok);
//void freeResource(void* addr);

int main(int argc, char **argv)
{
	FILE* fp = fopen(argv[1], "r");

        if (fp == NULL)
        {
                printf("Fail to open!\n");
                return -1;
        }

        int size = 0;
        char* doc = NULL;
        tok_t* tok = NULL;

//	readFile(argv[1], &doc, &size);

        fseek(fp, 0, SEEK_END);

        size = ftell(fp);

        doc = malloc(sizeof(char)*(size + 1));

        fseek(fp, 0, SEEK_SET);

        fread(doc, size, sizeof(char), fp);

        doc[size] = '\0';

        printf("%s\n", doc);

        fclose(fp);

	tok = parseJSON(doc, size, tok);

	printf("%d\n", *tok);

	printTokInfo(tok);
	
	free(tok);
	free(doc);
}
/*
void readFile(char* filename, char** doc, int* size)
{
	FILE* fp = fopen(filename, "r");

	if (fp == NULL)
	{
		printf("Fail to open!\n");
		return;
	}

	fseek(fp, 0, SEEK_END);

	*size = ftell(fp);

	doc = malloc(sizeof(char)*(*size + 1));

	fseek(fp, 0, SEEK_SET);

	fread(*doc, *size, sizeof(char), fp);

	doc[*size] = '\0';

	printf("%s\n", doc);

	fclose(fp);
}
*/
tok_t* parseJSON(char* ref, int size, tok_t* origin)
{
	tok_t* tok = origin;
	for (int i = 0; i < size; i++)
	{
		if (ref[0] != '{')
		{
			printf("Syntax error!");
			return NULL;
		}
		

		switch (ref[i])
		{
		case '"':
		{
			tok = realloc(tok, sizeof(tok_t)*(numOftok+1));
			tok[numOftok].type = STRING;
			tok[numOftok].start = i + 1;
			tok[numOftok].size = 0;

			
			if (strchr(ref + tok[numOftok].start, '"') == NULL)
			{
				printf("Syntax error!");
				return NULL;
			}
			else
				tok[numOftok].end = strchr(ref + tok[numOftok].start, '"') - ref;

			i += tok[numOftok].end - tok[numOftok].start + 1;

			numOftok++;
		}
		break;
		case '[':
		{
			tok = realloc(tok, sizeof(tok_t)*(numOftok + 1));
			tok[numOftok].type = ARRAY;
			tok[numOftok].start = i;
			tok[numOftok].size = 0;

			if (strchr(ref + tok[numOftok].start, ']') == NULL)
			{
				printf("Syntax error!");
				return NULL;
			}
			else
				tok[numOftok].end = strchr(ref + tok[numOftok].start, ']') - ref;

			i += tok[numOftok].end - tok[numOftok].start + 1;

			numOftok++;
		}
		break;
		case '{':
		{	
			//if (i == 0)
			//	continue;

			int cur = numOftok;

			tok = realloc(tok, sizeof(tok_t)*(numOftok + 1));
			tok[cur].type = OBJECT;
			tok[cur].start = i;
			
			tok[cur].size = 0;

			//printf("%s", ref + tok[cur].start);
			if (strchr(ref + tok[cur].start, '}') == NULL)
			{
				printf("Syntax error!");
				return NULL;
			}
			else if(strchr(ref + tok[cur].start, '{') == NULL)
			{
				tok[cur].end = strchr(ref + tok[cur].start, '}') - ref + 1;
				numOftok++;
				tok = parseJSON(ref + tok[cur].start, tok[cur].end - tok[cur].start, tok);
			}
			

			for(int i = cur; i < numOftok; i++)
			{
				tok[cur].size = tok[cur].size + tok[i].size;

				if(tok[cur].start > tok[i].start)
				{
					tok[i].start += tok[cur].start;
					tok[i].end += tok[cur].start;
				}
			}

			i += tok[cur].end - tok[cur].start + 1;

		}
		break;
		case '0': case '1': case '2': case '3': case '4': case '5':
		case '6': case '7': case '8': case '9': case '-': case 'e':
		case 'E':
		{
			tok = realloc(tok, sizeof(tok_t)*(numOftok + 1));
			tok[numOftok].type = PRIMITIVE;
			tok[numOftok].start = i;
			tok[numOftok].size = 0;

			if (strchr(ref + tok[numOftok].start, ',') == NULL || strchr(ref + tok[numOftok].start, '}') == NULL)
			{
				printf("Syntax error!");
				return NULL;
			}
			else
				tok[numOftok].end = strchr(ref + tok[numOftok].start, ',') - ref;

			i += tok[numOftok].end - tok[numOftok].start + 1;

			numOftok++;
		}
		break;
		case ':':
		{
			if(tok[numOftok-1].type == STRING || tok[numOftok-1].type == PRIMITIVE)
				tok[numOftok-1].size += 1;
		}
		break;
		case '}':
		{
			return tok;
		}
		}
	}
}
void printTokInfo(tok_t* tok)
{
	for(int i = 0; i < numOftok; i++)
        {
                printf("tok[%d].start: %d, tok[%d].end: %d tok[%d].size: %d\n", i, tok[i].start, i, tok[i].end, i, tok[i].size);
        }
}
