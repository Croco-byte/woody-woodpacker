#include <stdio.h>

int global = 0x4444;

int	main(void)
{
	size_t a = 0x1111111111111111;
	char str[] = "Bonjour bonjour\n";
	printf("%s", "Hello World\n");
	printf("%s", str);
	printf("%x\n", global);
}
