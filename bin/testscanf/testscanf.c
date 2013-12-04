#include<stdio.h>
#include<string.h>

int main(int argc, char* argv[], char* envp[]){
	int counter;
	char string_check[100];
	int scanf_return;

	printf("please enter intcount\n");
	scanf_return =scanf("%d", &counter);
	printf("intcount: %d\n", counter);
	printf("scanf returned %d\n",scanf_return);

	printf("please enter string");
	scanf_return = scanf("%s", &string_check);
	printf("string_check: %s\n", string_check );
	printf("scanf returned %d\n",scanf_return);
	
	int hexcount;
	printf("please enter hexcount\n");
	scanf_return = scanf("%x", &hexcount);
	printf("inside checkscanf hexcount: %x\n", hexcount);
	printf("scanf returned %d\n",scanf_return);

	int error =100;
	printf("please enter hexcount\n");
	scanf_return = scanf("%c", &error);
	printf("inside checkscanf error: %x\n", error);
	printf("scanf returned %d\n",scanf_return);

	return 0;
}
