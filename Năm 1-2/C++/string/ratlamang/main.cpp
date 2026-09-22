#include <iostream>
#include <string.h>
#include <cstring>

using namespace std;

int main()
{
    //COPY
    char a[1000], b[1000];
    gets(a);

    //Copy block of memory//

    //memcpy(void *  destination, const void * source,size_t num)
    memcpy(b, a, strlen(a) + 1);

    //Move block of memory//

    //memmove( void * destination, const void * sourcce, size_t num)
    memmove(b + 20, b + 15, 11);

    /*---------------*/
    char s1[1000] = "This is an example", s2[100], s3[100];
    //Copy string//

    //strcpy(char * destination, const char * source)
    strcpy(s2, s1);


    //Copy characters from strings//
    //strncpy ( char * destination, const char * sources, size_t num)
    strncpy(s2, s1, sizeof(s2)); // Copy to sized buffer (overflow safe)
    strncpy(s3, s2, 5);
    s3[5] = '\0'; // NULL charater manually added // de bao hieu ket thuc chuoi

    /*-----------------------------------------------------------------------------------------------*/

    //CONCATENATION : LIEN HE, KET NOI

    char str[100];
    //Concatenate//

    //strcat (char * desination, const char * source)
    strcpy(str, "My ");
    strcat(str, "name ");
    strcat(str, "is ");
    strcat(str, "Hoa! ");
    //Output : My name is Hoa!

    //Append character from string//

    //strncat(char * destination, const char * source, size_t nu)
    char str1[100], str2[100];
    strcpy(str1, "To be ");
    strcpy(str2, "or not to be");
    strncat(str1, str2, 6); // Lay 6 ki tu dau tien de conatenate
    //Output: To be or not

    /*------------------------------------------------------------------------------------------------*/

    // COMPARISION

    //Compare 2 blocks of memmory//

    // int memcmp (const void * ptr1, const void * ptr2, size_t num)

    // < 0
    // The first byte that does not match in both memory blocks has a lower value in ptr1 than in ptr2
    //(if evaluated as unsigned char values)

    // == 0
    // the contents of both memory blocks are equal

    // > 0
    // The first byte that does not match in both memory blocks has a greater value in ptr1 than in ptr2

    // (if evaluated as unsigned char values)
    char buffer1[] = "DWgaOtP12df0";
    char buffer2[] = "DWGAOTP12DF0";

    int n;
    n = memcmp ( buffer1, buffer2, sizeof(buffer1) );
    // Luc nay n = 1

    //Compare 2 strings//

    // int strcmp (const char * str1, const * str2)
    // < 0
    // == 0
    // > 0
    strcmp(buffer1, buffer2);

    //Compare character of 2 strings//

    //strncmp (const char * str1, const char * str2,size_t num)
    char st[][5] = { "R2D2" , "C3PO" , "R2A6" };
    for (int i = 0 ; i < 3; i++)
    if (strncmp(st[i], "R2xx", 2) == 0); //cout << str[n] << "is correct"
    //Output:
    //R2D2 is correct
    //R2A6 is correct

    /*-------------------------------------------------------------------------------------------------------*/

    //SEATCHING

    //Locate character in block of memory

    //void * memchr (const void * ptr, int value, size_t num)
    char ex[] = "This is an example!";
    //cout << ex << endl;
    //cout << "We find char p at position: ";
    char *pch;
    pch =  (char*) memchr(ex, 'e', strlen(ex));
    //cout << pch << endl << pch - ex + 1;
    // Output: 16

    //Locate first occurrence of character in string

    //const char * strchr (const char  str, int character);
    char *p;
    p = strchr(ex, 'm');
    // p = "mple!"

    //Get span until character in string

    //size_t strspn (const char * str1, const char * str2);
    char s[] = "fcba73";
    char keys[] = "1234567890";
    int i;
    i = strcspn (s,keys);
    //cout << "The first char of keys appearing in str is at position" << i + 1;
    //Output : 5

    //Locate characters in string

    //const char * strpbrk(const char * str1, const char *str2)
    char str[] = "This is a sample string";
    char key[] = "aeiou";
    char * pch;
    cout << "Vowels in " << str << ": " << endl;
    pch = strpbrk (str, key);
    while (pch != NULL)
    {
        cout << pch << endl;
        pch = strpbrk (pch+1,key);
    }
}
