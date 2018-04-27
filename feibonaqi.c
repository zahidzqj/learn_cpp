#include<stdio.h>
int main()
{
	int i,f[20];
	f[0]=f[1]=1;
	for (i=2;i<20;i++)
	{
		f[i]=f[i-1]+f[i-2];
		
	}
	for (i=0;i<20;i++)
	{
		printf("%5d",f[i]);
	if((i+1)%5==0)
	{
		printf("\n");
	}
  }
}

