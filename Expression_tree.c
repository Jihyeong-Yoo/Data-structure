#include <stdlib.h> // malloc, atoi
#include <stdio.h>
#include <string.h>
#include <ctype.h> // isdigit
#include <assert.h> // assert

#define MAX_STACK_SIZE	50

////////////////////////////////////////////////////////////////////////////////
// LIST type definition
typedef struct node
{
	char		data;
	struct node	*left;
	struct node	*right;
} NODE;

typedef struct
{
	NODE	*root;
} TREE;

////////////////////////////////////////////////////////////////////////////////
// Prototype declarations

/* Allocates dynamic memory for a tree head node and returns its address to caller
   return	head node pointer
   NULL if overflow
   */
TREE *createTree( void);

/* Deletes all data in tree and recycles memory
*/
void destroyTree( TREE *pTree);

static void _destroy( NODE *root);

/*  Allocates dynamic memory for a node and returns its address to caller
    return	node pointer
    NULL if overflow
    */
static NODE *_makeNode( char ch);

/* converts postfix expression to binary tree
   return	1 success
   0 invalid postfix expression
   */
int postfix2tree( char *expr, TREE *pTree);

/* Print node in tree using inorder traversal
*/
void traverseTree( TREE *pTree);

/* internal traversal function
   an implementation of ALGORITHM 6-6
   */
static void _traverse( NODE *root);

/* Print tree using inorder right-to-left traversal
*/
void printTree( TREE *pTree);

/* internal traversal function
*/
static void _infix_print( NODE *root, int level);//level 수만큼 tab 출력

/* evaluate postfix expression
   return	value of expression
   */
float evalPostfix( char *expr);

////////////////////////////////////////////////////////////////////////////////
void destroyTree( TREE *pTree)
{
	if (pTree)
	{
		_destroy( pTree->root);
	}

	free( pTree);
}

////////////////////////////////////////////////////////////////////////////////
void printTree( TREE *pTree)
{
	_infix_print(pTree->root, 0);

	return;
}

////////////////////////////////////////////////////////////////////////////////
void traverseTree( TREE *pTree)
{
	_traverse(pTree->root);

	return;
}

////////////////////////////////////////////////////////////////////////////////
int main( int argc, char **argv)
{
	TREE *tree;
	char expr[1024];

	fprintf( stdout, "\nInput an expression (postfix): ");

	while (fscanf( stdin, "%s", expr) == 1)
	{
		// creates a null tree
		tree = createTree();

		if (!tree)
		{
			printf( "Cannot create tree\n");
			return 100;
		}

		//postfix expression -> expression tree
		int ret = postfix2tree( expr, tree);
		if (!ret)
		{
			fprintf( stdout, "invalid expression!\n");
			destroyTree( tree);
			continue;
		}

		// expression tree -> infix expression
		fprintf( stdout, "\nInfix expression : ");
		traverseTree( tree);

		// print tree with right-to-left infix traversal
		fprintf( stdout, "\n\nTree representation:\n");
		printTree(tree);

		// evaluate postfix expression
		float val = evalPostfix( expr);
		fprintf( stdout, "\nValue = %f\n", val);

		// destroy tree
		destroyTree( tree);

		fprintf( stdout, "\nInput an expression (postfix): ");
	}
	return 0;
}

int postfix2tree( char *expr, TREE *pTree){
	NODE* Stack[MAX_STACK_SIZE];
	int expLen = strlen(expr);
	NODE *pNode;

	int j=0;
	for(int i=0; i<expLen; i++)
	{

		if(isdigit(expr[i]))//피연산자
		{
			pNode = _makeNode(expr[i]);
		}
		else if(!isdigit(expr[i]) && j<2){
			for(int k=0;k<j;k++)
				free(Stack[k]);
			return 0;
		}
		else
		{
			pNode = _makeNode(expr[i]);
			pNode->right = Stack[j-1];
			pNode->left = Stack[j-2];
			j=j-2;
		}
		Stack[j] = pNode;
		j++;
	}
	if(j!=1){
		for(int k=0;k<j;k++)
			free(Stack[k]);
		return 0;
	}
	pTree->root=Stack[0];
	return 1;
}



TREE *createTree( void){
	TREE*pTree = (TREE*)malloc(sizeof(TREE));
	pTree->root=NULL;
	return pTree;
}

static NODE *_makeNode( char ch){
	NODE*pNode = (NODE*)malloc(sizeof(NODE));

	if(pNode == NULL){
		free(pNode);
		return NULL;
	}
	pNode->data = ch;
	pNode->left = NULL;
	pNode->right = NULL;
	return pNode;
}

static void _destroy( NODE *root){
	if(root == NULL)
		return;
	_destroy(root->left);
	_destroy(root->right);
	free(root);  // 노드 삭제
}

static void _traverse( NODE *root){//중위순회
	if(root == NULL)
		return;
	if(!isdigit(root->data)){
		printf("(");
		_traverse(root->left);
	}
	printf("%c",root->data);
	if(!isdigit(root->data)){
		_traverse(root->right);
		printf(")");
	}
}



static void _infix_print( NODE *root, int level){//level 수만큼 tab 출력	inorder right-to-left traversal
	if(root == NULL)
		return;
	_infix_print(root->right, level+1);
	for(int i=0;i<level;i++)
		printf("\t");
	printf("%c\n",root->data);
	_infix_print(root->left, level+1);
}


float evalPostfix( char *expr){
	float Stack[MAX_STACK_SIZE];
	int top=-1;

	float op1, op2;
	int value=0;
	int count = 0;
	int expLen = strlen(expr);
	char symbol;

	for(int i=0;i<expLen;i++)
	{
		symbol = expr[i];

		if(symbol >= '0' && symbol <= '9')
		{
			value = expr[i]-'0';
			Stack[++top] = value;
		}
		else
		{
			op2 = Stack[top--];
			op1 = Stack[top--];

			switch(symbol)
			{
				case'+': Stack[++top] = op1+op2;
					 break;
				case'-': Stack[++top] = op1-op2;
					 break;
				case'*': Stack[++top] = op1*op2;
					 break;
				case'/': Stack[++top] = op1/op2;
					 break;
			}
		}
	}
	return Stack[0];
}




















