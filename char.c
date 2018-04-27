#include<string.h>//字符左右交换
#include<stdio.h>
int  main()
{
	int i,j,k;
	char str[5];
	for (i=0;i<5;i++)
	{
		str[i]=getchar();
	}

	for(i=0,j=4;i<j;i++,j--)
	{
		k=str[i];
		str[i]=str[j];
		str[j]=k;
	}
	for(i=0;i<5;i++)
	{
		printf("%c",str[i]);
	}
	return 0;
}
