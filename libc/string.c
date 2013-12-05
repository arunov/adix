#include <stdio.h>
void *memcpy(void *dest, const void *src, unsigned int count)
{
    const char *sp = (const char *)src;
    char *dp = (char *)dest;

	/* NEVER EVER GIVE A PRINT STATMENT HERE-- THIS METHOD IS USED BY PRINT!!*/
    for(; count != 0; count--) *dp++ = *sp++;
    return dest;
}

void *memset(void *dest, char val, unsigned int count)
{
    char *temp = (char *)dest;
    for( ; count != 0; count--) *temp++ = val;
    return dest;
}

unsigned short *memsetw(unsigned short *dest, unsigned short val, unsigned int count)
{
    unsigned short *temp = (unsigned short *)dest;
    for( ; count != 0; count--) *temp++ = val;
    return dest;
}

unsigned int strlen(const char *str)
{
    unsigned int retval;
    for(retval = 0; *str != '\0'; str++) retval++;
    return retval;
}

char *
strtok(s, delim)
	register char *s;
	register const char *delim;
{
	register char *spanp;
	register int c, sc;
	char *tok;
	static char *last;

	if (s == NULL && (s = last) == NULL)
		return (NULL);

	/*
	 * Skip (span) leading delimiters (s += strspn(s, delim), sort of).
	 */
cont:
	c = *s++;
	for (spanp = (char *)delim; (sc = *spanp++) != 0;) {
		if (c == sc)
			goto cont;
	}

	if (c == 0) {		/* no non-delimiter characters */
		last = NULL;
		return (NULL);
	}
	tok = s - 1;

	/*
	 * Scan token (scan for delimiters: s += strcspn(s, delim), sort of).
	 * Note that delim must have one NUL; we stop if we see that, too.
	 */
	for (;;) {
		c = *s++;
		spanp = (char *)delim;
		do {
			if ((sc = *spanp++) == c) {
				if (c == 0)
					s = NULL;
				else
					s[-1] = 0;
				last = s;
				return (tok);
			}
		} while (sc != 0);
	}
	/* NOTREACHED */
}

int str_equal(char *src, char *dst){
	if(src == NULL || dst == NULL){
		return NULL;
	}
	if(strlen(src) != strlen(dst)){
		return 0;
	}
	while(*src != '\0' ){
		if(*src++ != *dst++){
			return 0;
		}
	}	
	return 1;
}

//TODO: http://www.geeksforgeeks.org/write-your-own-atoi/

// A utility function to check whether x is numeric
static int isNumericChar(char x)
{
    return (x >= '0' && x <= '9')? 1: 0;
}
 
// A simple atoi() function. If the given string contains
// any invalid character, then this function returns 0
int atoi(char *str)
{
    if (*str == NULL)
       return 0;
 
    int res = 0;  // Initialize result
    int sign = 1;  // Initialize sign as positive
    int i = 0;  // Initialize index of first digit
 
    // If number is negative, then update sign
    if (str[0] == '-')
    {
        sign = -1;
        i++;  // Also update index of first digit
    }
 
    // Iterate through all digits of input string and update result
    for (; str[i] != '\0'; ++i)
    {
        if (isNumericChar(str[i]) == 0)
            return 0; // You may add some lines to write error message
                      // to error stream
        res = res*10 + str[i] - '0';
    }
 
    // Return result with sign
    return sign*res;
}

int atohex( char *str )
{
	int value = 0;

	for(;; ++str ) switch( *str )
	{
	case '0': case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9':
	value = ((value << 4) | (*str & 0xf));
	break;
	case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
	case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
	value = ((value << 4) | ((9 + *str) & 0xf));
	break;
	default:
	return value;
	}
}

int strncmp(char *src, char *dst, uint64_t n){
		if(src == NULL || dst == NULL){
			return 0;
		}
		for(int i=0; i<n; i++){
			if(*src++ != *dst++){
				return 0;
			}
		}
		return 1;
}
