#include <stdlib.h> // malloc
#include <stdio.h>
#include <string.h> // strdup, strcmp
#include <ctype.h> // toupper

#define QUIT			1
#define FORWARD_PRINT	2
#define BACKWARD_PRINT	3
#define SEARCH			4
#define DELETE			5
#define COUNT			6

// User structure type definition
typedef struct 
{
	char	*token;
	int		freq;
} tTOKEN;

////////////////////////////////////////////////////////////////////////////////
// LIST type definition
typedef struct node
{
	tTOKEN		*dataPtr;
	struct node	*llink;
	struct node	*rlink;
} NODE;

typedef struct
{
	int		count;
	NODE	*pos; // unused
	NODE	*head;
	NODE	*rear;
} LIST;

////////////////////////////////////////////////////////////////////////////////
// Prototype declarations

/* Allocates dynamic memory for a list head node and returns its address to caller
	return	head node pointer
			NULL if overflow
*/
LIST *createList( void);

/* Deletes all data in list and recycles memory
*/
void destroyList( LIST *pList);

/* Inserts data into list
	return	0 if overflow
			1 if successful
			2 if duplicated key
*/
int addNode( LIST *pList, tTOKEN *dataInPtr);

/* Removes data from list
	return	0 not found
			1 deleted
*/
int removeNode( LIST *pList, char *keyPtr, tTOKEN **dataOut);

/* interface to search function
	Argu	key being sought
	pDataOut	contains found data
	return	1 successful
			0 not found
*/
int searchList( LIST *pList, char *pArgu, tTOKEN **pDataOut);

/* returns number of nodes in list
*/
int countList( LIST *pList);

/* returns	1 empty
			0 list has data
*/
int emptyList( LIST *pList);

int fullList( LIST *pList);

/* prints data from list (forward)
*/
void printList( LIST *pList);

/* prints data from list (backward)
*/
void printListR( LIST *pList);

/* internal insert function
	inserts data into a new node
	return	1 if successful
			0 if memory overflow
*/
static int _insert( LIST *pList, NODE *pPre, tTOKEN *dataInPtr);

/* internal delete function
	deletes data from a list and saves the (deleted) data to dataOut
*/
static void _delete( LIST *pList, NODE *pPre, NODE *pLoc, tTOKEN **dataOutPtr);

/* internal search function
	searches list and passes back address of node
	containing target and its logical predecessor
	return	1 found
			0 not found
*/
static int _search( LIST *pList, NODE **pPre, NODE **pLoc, char *pArgu);

/* Allocates dynamic memory for a token structure, initialize fields(token, freq) and returns its address to caller
	return	token structure pointer
			NULL if overflow
*/
tTOKEN *createToken( char *str);

/* Deletes all data in token structure and recycles memory
	return	NULL head pointer
*/
void destroyToken( tTOKEN *pToken);

////////////////////////////////////////////////////////////////////////////////
/* gets user's input
*/
int get_action()
{
	char ch;
	scanf( "%c", &ch);
	ch = toupper( ch);
	switch( ch)
	{
		case 'Q':
			return QUIT;
		case 'F':
			return FORWARD_PRINT;
		case 'B':
			return BACKWARD_PRINT;
		case 'S':
			return SEARCH;
		case 'D':
			return DELETE;
		case 'C':
			return COUNT;
	}
	return 0; // undefined action
}

////////////////////////////////////////////////////////////////////////////////
int main( int argc, char **argv)
{
	LIST *list;
	char str[1024];
	tTOKEN *pToken;
	int ret;
	FILE *fp;
	
	if (argc != 2){
		fprintf( stderr, "usage: %s FILE\n", argv[0]);
		return 1;
	}
	
	fp = fopen( argv[1], "rt");
	if (!fp)
	{
		fprintf( stderr, "Error: cannot open file [%s]\n", argv[1]);
		return 2;
	}
	
	// creates an empty list
	list = createList();
	if (!list)
	{
		printf( "Cannot create list\n");
		return 100;
	}
	
	while(fscanf( fp, "%s", str) == 1)
	{
		pToken = createToken( str);
		
		// insert function call
		ret = addNode( list, pToken); //strdup(str)
		
		if (ret == 2) // duplicated 
			destroyToken( pToken);
	}
	
	fclose( fp);
	
	fprintf( stderr, "Select Q)uit, F)orward print, B)ackward print, S)earch, D)elete, C)ount: ");
	
	while (1)
	{
		int action = get_action();
		
		switch( action)
		{
			case QUIT:
				destroyList( list);
				return 0;
			
			case FORWARD_PRINT:
				printList( list);
				break;
			
			case BACKWARD_PRINT:
				printListR( list);
				break;
			
			case SEARCH:
				fprintf( stderr, "Input a string to find: ");
				fscanf( stdin, "%s", str);
				
				if (searchList( list, str, &pToken))
				{
					fprintf( stdout, "(%s, %d)\n", pToken->token, pToken->freq);
				}
				else fprintf( stdout, "%s not found\n", str);
				break;
				
			case DELETE:
				fprintf( stderr, "Input a string to delete: ");
				fscanf( stdin, "%s", str);
				
				if (removeNode( list, str, &pToken))
				{
					fprintf( stdout, "%s deleted\n", pToken->token);
					destroyToken( pToken);
				}
				else fprintf( stdout, "%s not found\n", str);
				break;
			 
			case COUNT:
				fprintf( stdout, "%d\n", countList( list));
				break;
			}
		
		if (action) fprintf( stderr, "Select Q)uit, F)orward print, B)ackward print, S)earch, D)elete, C)ount: ");
	}
	return 0;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int addNode(LIST* pList, tTOKEN* dataInPtr) {//list, pToken
	NODE* pPre = NULL;
	NODE* pLoc = NULL;

	int found1 = _search(pList, &pPre, &pLoc, dataInPtr->token);
	if (found1 == 1)
	{
		(pLoc->dataPtr->freq)++;
		return 2;
	}
	int found2 = _insert(pList, pPre, dataInPtr);
	if (found2 == 1)
		return 1;
	return 0;
}
		
	

static int _search(LIST* pList, NODE** pPre, NODE** pLoc, char* pArgu) { //pList, &pPre, &pLoc, dataInPtr->token

	int val=1;
	NODE*ptr;
   for(ptr = pList->head->rlink;ptr->rlink!=NULL;ptr=ptr->rlink){
	   val = strcmp(pArgu, ptr->dataPtr->token);
	   if(val<=0)
		   break;
   }
	*pPre = ptr->llink;
    if(val == 0)
    {
         *pLoc = ptr;
         return 1;
    }
    if(val < 0)
         return 0;
	return 0;
}



static void _delete( LIST *pList, NODE *pPre, NODE *pLoc, tTOKEN **dataOutPtr){//pList, pPre, pLoc, &pToken
	
	*dataOutPtr = pLoc->dataPtr;
	pLoc->rlink->llink = pPre;
	pPre->rlink=pLoc->rlink;
	
	free(pLoc);
	(pList->count)--;
}	
 

static int _insert(LIST* pList, NODE* pPre, tTOKEN* dataInPtr) {//pList, pPre, dataInPtr
	NODE* newNODE = (NODE*)malloc(sizeof(NODE));

	if (newNODE == NULL)
		return 0;
	newNODE->dataPtr = dataInPtr;
	newNODE->llink = pPre;
	newNODE->rlink = pPre->rlink;
	pPre->rlink = newNODE;
	newNODE->rlink->llink = newNODE;
	(pList->count)++;
	return 1;
}
	
	
	

int removeNode( LIST *pList, char *keyPtr, tTOKEN **dataOut){ //list, str, &pToken
	NODE *pPre;
	NODE *pLoc;
	
	if(_search(pList, &pPre, &pLoc, keyPtr) == 1)
	{
		_delete(pList, pPre, pLoc, dataOut);//LIST *pList, NODE *pPre, NODE *pLoc, tTOKEN **dataOutPtr
		return 1;
	}
	else
		return 0;
}



int searchList( LIST *pList, char *pArgu, tTOKEN **pDataOut){//list, str, &pToken
	NODE *pPre;
	NODE *pLoc;
	
	if(_search(pList, &pPre, &pLoc, pArgu)==1)
	{
		*pDataOut=pLoc->dataPtr;
		return 1;
	}
	return 0;
}

tTOKEN* createToken(char* str) {
	tTOKEN* pToken = (tTOKEN*)malloc(sizeof(tTOKEN));

	pToken->token = strdup(str); //*token에 str의 주솟값 저장
	if (pToken->token == NULL){
		free(pToken);
		return NULL;
	}

	pToken->freq = 1;


	return pToken;
}

LIST *createList( void){
	LIST *pList = (LIST*)malloc(sizeof(LIST)); // LIST 구조체 메모리 할당
	pList->head = (NODE*)malloc(sizeof(NODE));
	pList->rear = (NODE*)malloc(sizeof(NODE));
	
	pList->head->llink = NULL;
	pList->head->rlink = pList->rear;
	pList->rear->llink = pList->head;
	pList->rear->rlink = NULL;
	
	pList->count=0;
	
	if(pList->head==NULL && pList->rear==NULL)
	{
		free(pList->head);
		free(pList->rear);
		free(pList);
		return NULL;
	}
	return pList;
}
	
			
void destroyToken( tTOKEN *pToken){
	if(pToken == NULL)
		return;
	free(pToken->token);
	free(pToken);
}
		
void destroyList( LIST *pList){
	if(!pList)
		return;
	NODE*pLoc = pList->rear->llink;
	free(pList->rear);
		
	while(pLoc->llink!=NULL)
	{
		free(pLoc->dataPtr->token);
		free(pLoc->dataPtr);
		pLoc = pLoc->llink;
		free(pLoc->rlink);
	}
	free(pList->head);
	free(pList);
}	

int countList( LIST *pList){
	if(!pList)
		return 0;

	return pList->count;
}

void printList( LIST *pList){
	
	if(pList->head->rlink->rlink == NULL)
		printf("Nothing\n");
	for(NODE*pLoc=pList->head->rlink;pLoc->rlink !=NULL;pLoc = pLoc->rlink)
		printf("%s %d\n", pLoc->dataPtr->token, pLoc->dataPtr->freq);
}

void printListR( LIST *pList){
	if(pList->rear->llink->llink == NULL)
		printf("Nothing\n");
	for(NODE*pLoc=pList->rear->llink;pLoc->llink !=NULL;pLoc = pLoc->llink)
		printf("%s %d\n", pLoc->dataPtr->token, pLoc->dataPtr->freq);
	
}

int emptyList( LIST *pList){
	if(pList == NULL||pList->count == 0)
		return 1;
	return 0;
}













