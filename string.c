#include "string.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

StringPool stringP;

void init_string_pool() {
    if (stringP.startingPointer == 0) {
        char *temp = malloc(DEFAULT_SIZE);
        // Check to see if the malloc worked
        if (temp == NULL) {
            perror("Error with mallocing memory");
            exit(EXIT_FAILURE);
        }
        stringP.startingPointer = stringP.currentPointer = stringP.endPointer = temp;
        stringP.endPointer += DEFAULT_SIZE;
        stringP.totalSize = DEFAULT_SIZE;
        stringP.list = init_llist();
    }
}

bool cmp_label(void *i, void *target) {
    String *str1 = (String *) i;
    String *str2 = (String *) target;
    if (str1->pointer == str2->pointer && str1->size == str2->size) {
        return true;
    }
    return false;
}

// Takes a size and allocates a block of memory inside the pool
// Returns the pointer inside the pool
char *add_string(int size) {
    // Initilizes the string pool if haven't already
    init_string_pool();
    // Temporary node for iteration
    Node *temp = stringP.list.head;
    // Holds the return address
    char *returnAddress = NULL;
    // Check to see if there is a label with free memory first
    while (temp != NULL) {
        // Type casts the data to a string
        String *str = (String *) temp->data;
        // Removes the label from the list if the size is exactly equal to size needed
        if (str->size == size) {
            returnAddress = str->pointer;
            remove_node(&stringP.list, cmp_label, str);
        }
        // Else adjust the current label to the new size and pointer address;
        else if (str->size > size) {
            returnAddress = str->pointer;
            str->size -= size;
            str->pointer += size;
        }
        temp = temp->next;
    }
    // The return address is currently null, that means it couldn't find a label
    if (returnAddress == NULL) {
        if (stringP.currentPointer + size >= stringP.endPointer) {
            // Just in case the realloc moves the pointer address
            int index = stringP.currentPointer - stringP.startingPointer;
            char *temp = realloc(stringP.startingPointer, stringP.totalSize + size + 50);
            if (temp == NULL) {
                perror("Error with mallocing memory");
                exit(EXIT_FAILURE);
            }
            stringP.startingPointer = temp;
            stringP.endPointer += size + 50;
            stringP.totalSize += size + 50;
            stringP.currentPointer = stringP.startingPointer + index;
        }
        if (size == 0) {
            returnAddress = stringP.startingPointer;
        } else {
            returnAddress = stringP.currentPointer;
            if (size == 0) {
                stringP.currentPointer += 1;
            } else {
                stringP.currentPointer += size;
            }
        }
    }
    return returnAddress;
}

String create(char *str) {
    // Creates a string obj
    String str1;
    // The string is null terminating, the plus one include '\0'
    int size = strlen(str);
    if (size == 0) {
        str1.size = 0;
    } else {
        str1.size = strlen(str) + 1;
    }
    str1.is_null_term = true;
    str1.pointer = add_string(str1.size);
    str1.active = true;
    strcpy(str1.pointer, str);
    return str1;
}

String alloc_string(int size) {
    String str1;
    str1.size = size;
    str1.pointer = add_string(size);
    memset(str1.pointer, 0, str1.size);
    str1.is_null_term = false;
    str1.active = true;
    return str1;
}

void removes(String *str) {
    add(&stringP.list, str);
    str->active = 0;
}

void free_pool() {
    free_linkedlist(&stringP.list);
    free(stringP.startingPointer);
}

// String Methods

bool cmp(String str1, String str2) {
    // Checks to see if the strings are active
    if (str1.active && str2.active) {
        // Checks to see if the strings have a null term
        int size1 = str1.is_null_term ? str1.size - 1 : str1.size;
        int size2 = str2.is_null_term ? str2.size - 1 : str2.size;
        if (size1 != size2) {
            return false;
        }
        // If the strings are the same size, then iterate through the string to check for comparsion
        else {
            for (int i = 0; i < size1; i++) {
                if (str1.pointer[i] != str2.pointer[i]) {
                    return false;
                }
            }
            return true;
        }
    }
    // Exits with an error when trying to use an string that's inactive
    else {
        perror("Error with using an inactive strings\n");
        exit(EXIT_FAILURE);
    }
}

String removes_whitespace(String *str) {
    char previous = ' ';
    char buffer[str->size];
    int index = 0;
    for (int i = 0; i < str->size; i++) {
        if (previous == ' ' && str->pointer[i] == ' ') {
            continue;
        }
        previous = str->pointer[i];
        buffer[index++] = str->pointer[i];
    }
    buffer[index] = '\0';
    String newStr = create(buffer);
    return newStr;
}

StringArray init_string_array() {
    StringArray stringA;
    stringA.size = 0;
    stringA.totalSize = 5;
    String *temp = (String *) malloc(5 * sizeof(String));
    if (temp == NULL) {
        perror("Error with allocing memory");
        exit(EXIT_FAILURE);
    }
    stringA.startingPointer = temp;
    return stringA;
}

void add_stringArray(char *buffer, StringArray *stringA) {
    if (stringA->size >= stringA->totalSize) {
        String *temp = realloc(stringA->startingPointer, (stringA->totalSize + 2) * sizeof(String));
        if (temp == NULL) {
            perror("Error with allocing memory");
            exit(EXIT_FAILURE);
        }
        stringA->startingPointer = temp;
    }
    stringA->startingPointer[stringA->size++] = create(buffer);
}

StringArray split(String *str1, char delimiter) {
    StringArray stringA = init_string_array();
    char buffer[str1->size];
    int index = 0;
    for (int i = 0; i < str1->size; i++) {
        if (str1->pointer[i] != delimiter) {
            buffer[index++] = str1->pointer[i];
        } else {
            buffer[index] = '\0';
            add_stringArray(buffer, &stringA);
            memset(buffer, 0, str1->size);
            index = 0;
        }
    }
    if (stringA.size > 0) {
        buffer[index] = '\0';
        add_stringArray(buffer, &stringA);
    }
    else if (stringA.size == 0) {
        add_stringArray(str1->pointer, &stringA);
    }
    return stringA;
}

bool ends_with(String str1, String comp) {
    if (str1.size < comp.size) {
        return 0;
    } else {
        char *address = str1.pointer + str1.size - comp.size;
        for (int i = 0; i < comp.size; i++) {
            if (address[i] != comp.pointer[i]) {
                return 0;
            }
        }
        return 1;
    }
}

String replace(String *str1, char *newString) {
    removes(str1);
    String returningStr = create(newString);
    return returningStr;
}

String removes_char(String *str, char c) {
    char buffer[str->size];
    int index = 0;
    for (int i = 0; i < str->size; i++) {
        if (str->pointer[i] != c) {
            buffer[index++] = str->pointer[i];
        }
    }
    if (str->is_null_term) {
        removes(str);
        String newStr = create(buffer);
        return newStr;
    } else {
        String newStr = alloc_string(str->size);
        strcpy(newStr.pointer, buffer);
        return newStr;
    }
}

String removes_new_line(String *str) {
    char buffer[str->size];
    int index = 0;
    for (int i = 0; i < str->size; i++) {
        if (str->pointer[i] != '\n') {
            buffer[index++] = str->pointer[i];
        }
    }
    if (str->is_null_term) {
        removes(str);
        String newStr = create(buffer);
        return newStr;
    } else {
        String newStr = alloc_string(str->size);
        strcpy(newStr.pointer, buffer);
        return newStr;
    }
}


String removes_whitespace_newline(String *str) {
    char previous = ' ';
    char buffer[str->size];
    int index = 0;
    for (int i = 0; i < str->size; i++) {
        if (previous == ' ' && str->pointer[i] == ' ' || str->pointer[i] == '\n') {
            continue;
        }
        previous = str->pointer[i];
        buffer[index++] = str->pointer[i];
    }
    if (str->is_null_term) {
        removes(str);
        String newStr = create(buffer);
        return newStr;
    } else {
        String newStr = alloc_string(str->size);
        strcpy(newStr.pointer, buffer);
        return newStr;
    }
}

bool is_empty(String str) {
    for (int i = 0; i < str.size; i++) {
        if (str.pointer[i] != '\0') {
            return false;
        }
    }
    return true;
}


bool scan(String str, String cmp) {
    // Checks to see if the substring you are looking for is bigger than the actual string
    if (cmp.size > str.size) {
        return false;
    }
    int j = 0;
    for (int i = 0; i < str.size; i++) {
        if (str.pointer[i] == cmp.pointer[j]) {
            j++;
        } else {
            j = 0;
        }
        if (j == cmp.size - 2) {
            return true;
        }
    }
    return false;
}


String slice(String *str, char a) {
    int index = 0;
    char buffer[str->size];
    for (int i = 0; i < str->size; i++) {
        if (str->pointer[i] != a) {
           buffer[index++] = str->pointer[i];
        }
        else {
            buffer[index] = '\0';
            break;
        }
    }
     if (str->is_null_term) {
         return create(buffer);
     }
     else {
        memset(str->pointer + index, 0, str->size - index);

    }
}
