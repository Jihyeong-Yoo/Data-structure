#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stddef.h>

#define MAX_YEAR_DURATION	10	// 기간

// 구조체 선언
typedef struct {
	char	name[20];		// 이름
	char	sex;			// 성별 M or F
	int		freq[MAX_YEAR_DURATION]; // 연도별 빈도
} tName;

typedef struct {
	int		len;		// 배열에 저장된 이름의 수
	int		capacity;	// 배열의 용량 (배열에 저장 가능한 이름의 수)
	tName	*data;		// 이름 배열의 포인터
} tNames;

// 함수 원형 선언

// 연도별 입력 파일을 읽어 이름 정보(이름, 성별, 빈도)를 이름 구조체에 저장
// 이미 구조체에 존재하는(저장된) 이름은 해당 연도의 빈도만 저장
// 새로 등장한 이름은 구조체에 추가
// 주의사항: 동일 이름이 남/여 각각 사용될 수 있으므로, 이름과 성별을 구별해야 함
// 주의사항: 구조체 배열은 정렬 리스트(ordered list)이어야 함
// 이미 등장한 이름인지 검사하기 위해 bsearch 함수를 사용
// 새로운 이름을 저장할 메모리 공간을 확보하기 위해 memmove 함수를 이용하여 메모리에 저장된 내용을 복사
// names->capacity는 2배씩 증가
void load_names( FILE *fp, int year_index, tNames *names);

// 구조체 배열을 화면에 출력
void print_names( tNames *names, int num_year);

// bsearch를 위한 비교 함수
int compare( const void *n1, const void *n2);

// 이진탐색 함수
// return value: key가 발견되는 경우, 배열의 인덱스
//				key가 발견되지 않는 경우, key가 삽입되어야 할 배열의 인덱스
int binary_search( const void *key, const void *base, size_t nmemb, size_t size, int (*compare)(const void *, const void *));

// 함수 정의

// 이름 구조체 초기화
// len를 0으로, capacity를 1로 초기화
// return : 구조체 포인터
tNames *create_names(void)
{
	tNames *pnames = (tNames *)malloc( sizeof(tNames));
	
	pnames->len = 0;
	pnames->capacity = 1;
	pnames->data = (tName *)malloc(pnames->capacity * sizeof(tName));

	return pnames;
}

// 이름 구조체에 할당된 메모리를 해제
void destroy_names(tNames *pnames)
{
	free(pnames->data);
	pnames->len = 0;
	pnames->capacity = 0;

	free(pnames);
}
	
////////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
	tNames *names;
	
	int num = 0;
	FILE *fp;
	int num_year = 0;
	
	if (argc == 1) return 0;

	// 이름 구조체 초기화
	names = create_names();

	// 첫 연도 알아내기 "yob2009.txt" -> 2009
	int start_year = atoi( &argv[1][3]);
	
	for (int i = 1; i < argc; i++)
	{
		num_year++;
		fp = fopen( argv[i], "r");
		assert( fp != NULL);
		
		int year = atoi( &argv[i][3]); // ex) "yob2009.txt" -> 2009
		
		fprintf( stderr, "Processing [%s]..\n", argv[i]);
		
		// 연도별 입력 파일(이름 정보)을 구조체에 저장
		load_names( fp, year-start_year, names);
		
		fclose( fp);
	}
	
	// 이름 구조체를 화면에 출력
	print_names( names, num_year);

	// 이름 구조체 해제
	destroy_names( names);
	
	return 1;
}
void load_names(FILE* fp, int year_index, tNames* names) {
   char sentence[30];
   int i;
   int index = 0;
   
   while (!feof(fp))
   {
      tNames *tdata = (tNames*)malloc(sizeof(tNames));//텍스트줄 담아놓을 임시적인 메모리 할당
      tdata->len=0;
      tdata->capacity=1;
      tdata->data = (tName*)malloc(sizeof(tName));
      
      fgets(sentence, sizeof(sentence), fp);//파일 한줄씩 읽기
      for (i = 0; i < strlen(sentence); i++)//콤마 변환
      {
         if (sentence[i] == ',')
            sentence[i] = ' ';
      }      
      sscanf(sentence, "%s %c %d", tdata->data[0].name, &tdata->data[0].sex, &tdata->data[0].freq[0]);//각각에 저장
      
   
      
      if(names->data == NULL)//tName 구조체가 비어있으면
      {
         if (names->capacity == names->len)
         {
            names->capacity = names->capacity * 2;
            names->data = (tName*)realloc(names->data, names->capacity * sizeof(tName));
         }
         strcpy(names->data[index].name, tdata->data[0].name);
         names->data[index].sex = tdata->data[0].sex;
         memset(names->data[index].freq, 0, (MAX_YEAR_DURATION)*sizeof(int));//freq 0으로 초기화
         names->data[index].freq[year_index] = tdata->data[0].freq[0];
         
         free(tdata->data);
         free(tdata);
         
         
         names->len++;
          continue;
      }
      
      index = binary_search((tdata->data), (names->data), names->len, sizeof(tName), compare);
      if(names->len > index && strcmp(names->data[index].name, tdata->data[0].name)==0 && names->data[index].sex==tdata->data[0].sex) //이름 성별 같을때
      {
         memset(names->data[index].freq+year_index+1, 0, (9 - year_index)*sizeof(int));//freq 0으로
         names->data[index].freq[year_index] = tdata->data[0].freq[0];
         
      }
      else//이름 성별 다를때
      {
         
         if (names->capacity <= names->len+1)
         {
            names->capacity = names->capacity * 2+1;
            names->data = (tName*)realloc(names->data, names->capacity * sizeof(tName));
         }
         
         memmove((names->data+(index+1)), (names->data + index), (names->len-index)*sizeof(tName)); //memmove 함수로 인덱스 뒷부분 복사
         strcpy(names->data[index].name, tdata->data[0].name);
         names->data[index].sex = tdata->data[0].sex;
         memset(names->data[index].freq, 0, (MAX_YEAR_DURATION)*sizeof(int));//freq 0으로 초기화
         names->data[index].freq[year_index] = tdata->data[0].freq[0];
         
         free(tdata->data);
         free(tdata);
         names->len++;
      }

   }

}

int binary_search(const void* key, const void* base, size_t nmemb, size_t size, int (*compare)(const void*, const void*)) {
   int first = 0, last = nmemb;//nmemb=len-1
   int value=0;
   int m = (first + last) / 2;
   
   while (first < last) {
      
      int value = compare(key, ((char*)base + m * size));
      switch (value) {
      case 0:
         return m;
        case 1://key의 순서가 data[m]보다 느리다
         first = m + 1;
         
         break;
      case -1://key의 순서가 data[m]보다 빠르다
         last = m;
         
         break;

      }
      m = (first + last) / 2;
   }
   return first;
}

void print_names(tNames* names, int num_year)
{
   for (int i = 0; i < names->len; i++)
   {
      printf("%-5s\t%c\t", names->data[i].name, names->data[i].sex);
      for (int j = 0; j < num_year; j++)
      {
         printf("%d\t", names->data[i].freq[j]);
      }
      printf("\n");
   }
}
int compare(const void* n1, const void* n2)
{
   int cmp1;
   cmp1 = strcmp(((tName*)n1)->name, ((tName*)n2)->name);
   

   if (cmp1 > 0)//이름이 다르고 n1의 순서가 느리면
   {
      return 1;
   }
   else if (cmp1 < 0)//이름이 다르고 n1의 순서가 빠르면
   {
      return -1;
   }
   else//이름이 같다면
   {
      if (((tName*)n1)->sex > ((tName*)n2)->sex)//성별이 다르고 n1의 순서가 느리면
      {
         return 1;
      }
      else if (((tName*)n1)->sex < ((tName*)n2)->sex) //성별이 다르고 n1의 순서가 빠르면
      {
         return -1;
      }
      else //이름과 성별이 같으면
      {
         return 0;
      }
   }

}


