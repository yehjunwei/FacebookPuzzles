#include <stdio.h>

int main(int argc, char *argv[])
{
	FILE *fp = fopen(argv[1], "r");
	if(fp == 0)
	{
		printf("input file not found!");
		return 0;
	}
	else
	{
		int number = 0;
		fscanf(fp, "%d", &number);
		fclose(fp);
		for(int i = 1 ; i <= number ; ++i)
		{
			if(i % 15 == 0)
				printf("Hop\n");
			else if(i % 3 == 0)
				printf("Hoppity\n");
			else if(i % 5 == 0)
				printf("Hophop\n");
			else
				continue;
		}
	}

	return 0;
}