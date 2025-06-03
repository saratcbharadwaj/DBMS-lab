#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "contact.h"

/* Return 0 if phone of the contact matches with phone parameter */
/* Return 1 if phone of the contact does NOT match */
/* Return > 1 in case of any other error such as NULL pointer for the parameters*/
int contact_name_matcher(void *rec, void *search_name)
{
    if (rec == NULL || search_name == NULL) {
        return 99; // Error: NULL input
    }

    struct Contact *cpointer = (struct Contact *)rec;
    const char *char_pointer = (const char *)search_name;

    // Ensure strings are properly null-terminated
    if (strncmp(cpointer->contact_name, char_pointer, sizeof(cpointer->contact_name)) == 0) {
        return 0; // Match
    } else {
        return 1; // No match
    }
}

