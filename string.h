#ifndef MYSTRING_H
#define MYSTRING_H
#include <stdbool.h>
#include "../datastructs/llist/linkedlist.h"

#define DEFAULT_SIZE 150


typedef struct{
    char* pointer;
    int size;
    bool is_null_term;
    bool active;
}String;


/**
*
* StringPool is a global memory pool for the strings
*
**/
typedef struct {
    char* startingPointer;
    char* currentPointer;
    char* endPointer;
    int totalSize;
    LinkedList list;
}StringPool;

typedef struct{
    String* startingPointer;
    int totalSize;
    int size;
}StringArray;

/**
* Initilize the string pool, a large pool of dynamic memory that strings will use
*
*
**/
void init_string_pool();


/**
 * Creates a String using the stringPool for the source of memory
 **/
String create(char* str);

String alloc_string(int size);

void removes(String* str);

bool cmp(String str1, String str2);

String removes_whitespace(String* str);

StringArray split(String* str, char delimiter);

String replace(String* str1, char* newString);

bool ends_with(String str1, String comp);

void free_pool();


String removes_char(String* str, char c);

String removes_new_line(String* str);

String removes_whitespace_newline(String* str);

bool is_empty(String str);

bool scan(String str, String cmp);

String slice(String* str, char a);

#endif
