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
	int size;
	char* value;
} tok_t;

int numOftok = 0;

tok_t* parseJSON(char* ref, int size, tok_t* origin, int offset);
void printTokInfo(tok_t* tok);
void setValue(char* ref, tok_t* tok);

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


	tok = parseJSON(doc, size, tok, 0);

	if (tok != NULL)
	{
		setValue(doc, tok);
		//setSize(doc, tok);
		printTokInfo(tok);
	}
	for (int i = 0; i < numOftok; i++)
	{
		free(tok[i].value);
	}
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
				int n = tok[cur].start;
				while (strchr(ref + n, '\\') + 1 == strchr(ref + n + 1, '\"'))
				{
					n++;
				}
				tok[cur].end = strchr(ref + n + 2, '"') - ref - 1;
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
			if (tok == NULL)
			{
				free(tok);
				printf("Syntax error!");
				return NULL;
			}
			i = tok[cur].end;
			if (cur == 0)
				return tok;
		}
		break;
		case ']':
		{
			for(int k = start + 1; k < numOftok; k++)
			{
                                if(tok[k].type == OBJECT || tok[k].type == ARRAY)
				{
					int bound = tok[k].end;
					if(tok[k - 1].type != STRING && tok[k - 1].size == 0)
                                        	tok[start].size += 1;
					while(bound >= tok[k].start)
					{
						k++;
					}
					k -= 1;
				}
				else
				{
                                        tok[start].size += tok[k].size;
				}

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
			if (tok == NULL)
			{
				free(tok);
				printf("Syntax error!");
				return NULL;
			}
			i = tok[cur].end;
			if (cur == 0)
				return tok;
			break;
		}
		break;
		case '}':
		{
	
			for(int k = start + 1; k < numOftok; k++)
                        {
                                if(tok[k].type == OBJECT || tok[k].type == ARRAY)
                                {
                                        int bound = tok[k].end;
					if(tok[k - 1].type != STRING && tok[k - 1].size == 0)
                                        	tok[start].size += 1;
                                        while(bound >= tok[k].start)
                                        {
                                                k++;
                                        }
					k -= 1;
                                }
                                else
                                {
                                        tok[start].size += tok[k].size;
                                }
                        }

			tok[start].end = i;
			return tok;
		}
		case '0': case '1': case '2': case '3': case '4': case '5':
		case '6': case '7': case '8': case '9': case '-': case 't':
		case 'T': case 'f': case 'F':
		{
			tok = realloc(tok, sizeof(tok_t)*(numOftok + 1));
			tok[cur].type = PRIMITIVE;
			tok[cur].start = i;
			tok[cur].end = 0;
			tok[cur].size = 0;

			if (strchr(ref + tok[cur].start, ',') == NULL && strchr(ref + tok[cur].start, ']') == NULL && strchr(ref + tok[cur].start, '}') == NULL)
			{
				free(tok);
				printf("Syntax error!");
				return NULL;
			}
			if (strchr(ref + tok[cur].start, ',') != NULL && strchr(ref + tok[cur].start, ',') < strchr(ref + tok[cur].start, ']') && strchr(ref + tok[cur].start, ',') < strchr(ref + tok[cur].start, '}'))
			{
				tok[cur].end = strchr(ref + tok[cur].start, ',') - ref - 1;
				i = tok[cur].end;
			}
			if(strchr(ref + tok[cur].start, ',') == NULL || (strchr(ref + tok[cur].start, ',') != NULL && strchr(ref + tok[cur].start, ',') > strchr(ref + tok[cur].start, ']')) || (strchr(ref + tok[cur].start, ',') != NULL && strchr(ref + tok[cur].start, ',') > strchr(ref + tok[cur].start, '}')))
			{
				while (1)
				{
					switch (ref[++i])
					{
					case '0': case '1': case '2': case '3': case '4': case '5':
					case '6': case '7': case '8': case '9': case '-': case '.':
					case 'e': case 'E': case '+': case 'r': case 'R': case 'u':
					case 'U': case 'a': case 'A': case 'l': case 'L': case 's':
					case 'S':
					break;
					default:
					{
						tok[cur].end = i - 1;
					}
					}
					if (tok[cur].end == i - 1)
					{
						break;
					}
				}
				i -= 1;
			}
			numOftok++;
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
		tok[i].value = malloc(sizeof(char)*(tok[i].end - tok[i].start + 1));
		strncpy(tok[i].value, ref + tok[i].start, tok[i].end - tok[i].start + 1);
		tok[i].value[tok[i].end - tok[i].start + 1] = '\0';
		tok[i].end += 1;
	}
}
void printTokInfo(tok_t* tok)
{

	for (int i = 0; i < numOftok; i++)
	{
		switch(tok[i].type)
		{
		case 0:
		{
			printf("[%d] %s (size=%d, %d~%d, UNDEFINED)\n", i, tok[i].value, tok[i].size, tok[i].start, tok[i].end);
		}
		break;
		case 1:
		{
			printf("[%d] %s (size=%d, %d~%d, OBJECT)\n", i, tok[i].value, tok[i].size, tok[i].start, tok[i].end);
		}
		break;
		case 2:
		{
			printf("[%d] %s (size=%d, %d~%d, ARRAY)\n", i, tok[i].value, tok[i].size, tok[i].start, tok[i].end);
		}
		break;
		case 3:
		{
			printf("[%d] %s (size=%d, %d~%d, STRING)\n", i, tok[i].value, tok[i].size, tok[i].start, tok[i].end);
		}
		break;
		case 4:
		{
			printf("[%d] %s (size=%d, %d~%d, PRIMITIVE)\n", i, tok[i].value, tok[i].size, tok[i].start, tok[i].end);
		}
		break;
		}

	}
}
