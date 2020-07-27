#include <stdio.h>
#include <stdlib.h>	// malloc
#include <string.h>	// strdup
#include <ctype.h>	// isupper, tolower

#define MAX_DEGREE	27 // 'a' ~ 'z' and EOW
#define EOW			'$'	// end of word

// used in the following functions: trieInsert, trieSearch, triePrefixList
#define getIndex(x)		(((x) == EOW) ? MAX_DEGREE-1 : ((x) - 'a'))

// TRIE type definition
typedef struct trieNode {
	int 			index; // -1 (non-word), 0, 1, 2, ...
	struct trieNode	*subtrees[MAX_DEGREE];
} TRIE;

////////////////////////////////////////////////////////////////////////////////
// Prototype declarations

/* Allocates dynamic memory for a trie node and returns its address to caller
return	node pointer
NULL if overflow
*/
TRIE *trieCreateNode(void);

/* Deletes all data in trie and recycles memory
*/
void trieDestroy(TRIE *root);

/* Inserts new entry into the trie
return	1 success
0 failure
*/
// 주의! 엔트리를 중복 삽입하지 않도록 체크해야 함
// 대소문자를 소문자로 통일하여 삽입->대문자인지 확인 is
// 영문자와 EOW 외 문자를 포함하는 문자열은 삽입하지 않음
int trieInsert(TRIE *root, char *str, int dic_index);

/* Retrieve trie for the requested key
return	index in dictionary (trie) if key found
-1 key not found
*/
int trieSearch(TRIE *root, char *str);

/* prints all entries in trie using preorder traversal
*/
void trieList(TRIE *root, char *dic[]);

/* prints all entries starting with str (as prefix) in trie
ex) "abb" -> "abbess", "abbesses", "abbey", ...
using trieList function
*/
void triePrefixList(TRIE *root, char *str, char *dic[]);

// 아래는 구현하지 않아도 됨
/* makes permuterms for given str
ex) "abc" -> "abc$", "bc$a", "c$ab", "$abc"
return	number of permuterms
*/
int make_permuterms(char *str, char *permuterms[]);

/* recycles memory for permuterms
*/
void clear_permuterms(char *permuterms[], int size);

/* wildcard search
ex) "ab*", "*ab", "a*b", "*ab*"
using triePrefixList function
*/
void trieSearchWildcard(TRIE *root, char *str, char *dic[]);

////////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
	TRIE *trie;
	int ret;
	char str[100];
	FILE *fp;
	char *dic[100000];
	
	if (argc != 2)
	{
		fprintf( stderr, "Usage: %s FILE\n", argv[0]);
		return 1;
	}
	
	fp = fopen( argv[1], "rt");
	if (fp == NULL)
	{
		fprintf( stderr, "File open error: %s\n", argv[1]);
		return 1;
	}
	
	trie = trieCreateNode();
	
	int index = 0;
	while (fscanf( fp, "%s", str) != EOF)
	{
		ret = trieInsert( trie, str, index);

		if (ret) dic[index++] = strdup( str);
	}
	
	fclose( fp);
	
	fprintf( stdout, "\nQuery: ");
	while (fscanf( stdin, "%s", str) != EOF)
	{
		// wildcard search
		if (str[strlen(str)-1] == '*')
		{
			str[strlen(str)-1] = 0;
			triePrefixList( trie, str, dic);
		}
		// keyword search
		else
		{
			ret = trieSearch( trie, str);
			if (ret == -1) printf( "[%s] not found!\n", str);
			else printf( "[%s] found!\n", dic[ret]);
		}
		
		fprintf( stdout, "\nQuery: ");
	}
	
	trieList( trie, dic);
	
	for (int i = 0; i < index; i++)
		free( dic[i]);
	
	trieDestroy( trie);
	
	return 0;
}





/* Allocates dynamic memory for a trie node and returns its address to caller
return	node pointer
NULL if overflow
*/
TRIE *trieCreateNode(void){
	TRIE *pNode = (TRIE*)malloc(sizeof(TRIE));
	if (pNode == NULL)
		return NULL;
	pNode->index = -1;
	for(int i=0;i<MAX_DEGREE;i++){
			pNode->subtrees[i] = NULL;
	}
	return pNode;	
}
/* Deletes all data in trie and recycles memory
*/
void trieDestroy(TRIE *root){
	if (root == NULL)
		return;
	for(int i=0;i<MAX_DEGREE;i++){
		trieDestroy(root->subtrees[i]);
	}	
	free(root);
}

/* Inserts new entry into the trie
return	1 success
0 failure
*/
// 주의! 엔트리를 중복 삽입하지 않도록 체크해야 함-> 해당 인덱스가 -1이 아니면 이미 있는것임
// 대소문자를 소문자로 통일하여 삽입->대문자인지 확인 isupper, 대문자 소문자로 바꿈 tolower
// 영문자와 EOW 외 문자를 포함하는 문자열은 삽입하지 않음 ->인덱스가 MAX_DEGREE를 벗어나면 삽입하지 않음
int trieInsert(TRIE *root, char *str, int dic_index){
	int slen = strlen(str);
	
	for (int i = 0; i < slen; i++){//대문자 소문자 변환
        if (isupper(str[i]))
            str[i] = tolower(str[i]);
    }
	
	for(int j=0;j<slen;j++){
		int next = getIndex(str[j]);
		
		if(next>MAX_DEGREE)//만약 인덱스가 MAX_DEGREE를 벗어나면 삽입하지 않음
			return 0;
		if(root->subtrees[next] == NULL)
			root->subtrees[next] = trieCreateNode();
		root = root->subtrees[next];
	}
	if (root->index != -1)//있는 문자면 삽입하지 않음
		return 0;
	root->index = dic_index;
	return 1;
}



/* Retrieve trie for the requested key
return	index in dictionary (trie) if key found
-1 key not found
*/
int trieSearch(TRIE *root, char *str){
	int len = strlen(str);
	
	for (int i = 0; i < len; i++){//대문자 소문자 변환
        if (isupper(str[i]))
            str[i] = tolower(str[i]);
    }

	for(int j=0;j<=len;j++){
		if(str[j] == '\0')
			return root->index;

		int next = getIndex(str[j]);
		
		if(root->subtrees[next] == NULL)
			return -1;
		root = root->subtrees[next];
	}
}

	
/* prints all entries in trie using preorder traversal*/

void trieList(TRIE *root, char *dic[]){//trie, dic[root->index]
	if (root == NULL)
		return;
	if (root->index != -1)
		printf("%s\n", dic[root->index]);
	for(int i=0;i<MAX_DEGREE;i++){
		trieList(root->subtrees[i], dic);
	}
}

/* prints all entries starting with str (as prefix) in trie
ex) "abb" -> "abbess", "abbesses", "abbey", ...
using trieList function
*/
void triePrefixList(TRIE *root, char *str, char *dic[]){//trie, str, dic
	TRIE *temp;
	int slen = strlen(str);
	
	for(int i=0; i<slen; i++){
		int next = getIndex(str[i]);
		temp = root->subtrees[next];
		root = temp;
	}	
	trieList(temp, dic);
	
}
	
	
	
	
	
	
	
	
	
