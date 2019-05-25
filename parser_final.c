#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
	char* value;
} tok_t;

int numOftok = 0;

//void readFile(char* filename, char** doc, int* size);
tok_t* parseJSON(char* ref, int size, tok_t* origin, int offset);
void printTokInfo(tok_t* tok);
//void freeResource(void* addr);
void setValue(char* ref, tok_t* tok);

int main(int argc, char **argv)
{
	FILE* fp = fopen("example.json", "r");

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

	/*
	for (int k = 0; k < size; k++)
	{
		printf("%d %c\n", k, doc[k]);
	}
	*/

	fclose(fp);



	tok = parseJSON(doc, size, tok, 0);

	if (tok != NULL)
	{
		setValue(doc, tok);
		printTokInfo(tok);
	}
	for (int i = 0; i < numOftok; i++)
	{
		free(tok[i].value);
	}
	free(tok);
	free(doc);

	system("pause");
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
tok_t* parseJSON(char* ref, int size, tok_t* origin, int offset)
{
	int start = 0;
	if (numOftok == 0)
		start = 0;
	else
		start = numOftok - 1;

	tok_t* tok = origin;
	for (int i = 0 + offset; i < size; i++)
	{
		int cur = numOftok;

		switch (ref[i])
		{
		case '"':
		{
			tok = realloc(tok, sizeof(tok_t)*(numOftok + 1));
			tok[cur].type = STRING;
			tok[cur].start = i + 1;
			tok[cur].size = 0;


			if (strchr(ref + tok[cur].start, '"') == NULL)

			{
				free(&tok[cur]);
				printf("Syntax error!");
				return NULL;
			}
			if (strchr(ref + tok[cur].start, '\\') == NULL || strchr(ref + tok[cur].start, '\\') > strchr(ref + tok[cur].start, '"'))
			{
				tok[cur].end = strchr(ref + tok[cur].start, '"') - ref - 1;
			}
			else
			{
				int n = 0;
				while (strchr(ref + tok[cur].start + n, '\\') != NULL || (strchr(ref + tok[cur].start + n, '\\') > strchr(ref + tok[cur].start + n, '"') && strchr(ref + tok[cur].start + n, '\\') != NULL))
				{
					n++;
				}
				tok[cur].end = strchr(ref + tok[cur].start + n + 1, '"') - ref - 1;
			}
			numOftok++;
			i = tok[cur].end + 1;

		}
		break;
		case '[':
		{
			tok = realloc(tok, sizeof(tok_t)*(numOftok + 1));
			tok[cur].start = i;
			tok[cur].type = ARRAY;
			tok[cur].size = 0;


			if (strchr(ref + i, ']') == NULL)
			{
				free(tok);
				printf("Syntax error!");
				return NULL;
			}
			numOftok++;

			tok = parseJSON(ref, size, tok, tok[cur].start + 1);
			i = tok[cur].end;
			tok[cur].size = numOftok - cur - 1;
			if (cur == 0)
				return tok;
		}
		break;
		case ']':
		{
			for (int k = start + 1; k < numOftok; k++)
			{
				tok[start].size += tok[k].size;
			}
			tok[start].end = i;
			return tok;
		}
		case '{':
		{
			tok = realloc(tok, sizeof(tok_t)*(numOftok + 1));
			tok[cur].start = i;
			tok[cur].type = OBJECT;
			tok[cur].size = 0;


			if (strchr(ref + i, '}') == NULL)
			{
				free(tok);
				printf("Syntax error!");
				return NULL;
			}
			numOftok++;

			tok = parseJSON(ref, size, tok, tok[cur].start + 1);
			i = tok[cur].end;
			if (cur == 0)
				return tok;
			break;
		}
		break;
		case '}':
		{
			for (int k = start + 1; k < numOftok; k++)
			{
				tok[start].size += tok[k].size;
			}
			tok[start].end = i;
			return tok;
		}
		case '0': case '1': case '2': case '3': case '4': case '5':
		case '6': case '7': case '8': case '9': case '-':
		{
			tok = realloc(tok, sizeof(tok_t)*(numOftok + 1));
			tok[cur].type = PRIMITIVE;
			tok[cur].start = i;
			tok[cur].size = 0;

			if (strchr(ref + tok[numOftok].start, ',') == NULL || strchr(ref + tok[numOftok].start, '}') == NULL)
			{
				free(tok);
				printf("Syntax error!");
				return NULL;
			}
			else
			{
				tok[cur].end = strchr(ref + tok[cur].start, ',') - ref - 1;
				numOftok++;
			}
			i = tok[cur].end + 1;

		}
		break;
		case ':':
		{
			if (tok[numOftok - 1].type == STRING || tok[numOftok - 1].type == PRIMITIVE)
				tok[numOftok - 1].size += 1;
		}
		break;
		}
	}
}
void setValue(char* ref, tok_t* tok)
{
	for (int i = 0; i < numOftok; i++)
	{
		tok[i].value = malloc(sizeof(char)*(tok[i].end - tok[i].start + 2));
		strncpy(tok[i].value, ref + tok[i].start, tok[i].end - tok[i].start + 1);
		tok[i].value[tok[i].end - tok[i].start + 1] = '\0';
	}
}
void printTokInfo(tok_t* tok)
{
	for (int i = 0; i < numOftok; i++)
	{
		printf("tok[%d].start: %d, tok[%d].end: %d tok[%d].size: %d\n", i, tok[i].start, i, tok[i].end, i, tok[i].size);
		printf("tok[%d].value: %s\n", i, tok[i].value);
	}
}
