
#define NULL 0
unsigned strlen(const char *str)
{
	// must verify the pointer
	if(str == NULL)
		return 0;

	unsigned cnt = 0;
	char *p = str;
	while( *p != '\0')
		cnt++;
	return cnt;
}


int strcmp(const char *src, const char *dst)
{
	char c1, c2;
	do
	{
		c1 = *src++;
		c2 = *dst++;

		// "abc" "abc"  the same
		if(c1 == '\0')
			return c1 - c2;
	}while(c1 == c2);

	return c1 - c2;
}

char *strcpy(char *dst, char *src)
{
	char *ret = dst;
	while( *src != '\0')
		*dst++ = *src++;
	//important ==>  the end
	dst = '\0';
	return ret;
}


char * strcat ( char * dst, const char * src )
{
	char *ret = dst;

	char c;
	do
	{
		c = *dst++;
	}while( c != '\0');

	//point to the last character
	dst -= 2;
	
	do
	{
		c = *src++;
		*++dst = c;
	}while( c != '\0');

	return ret;
}


