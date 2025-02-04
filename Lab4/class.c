Void pointer

syntax : void *poitner name;
void *ptr;

int i = 9;
int *p;floap *fp;
void *ptr;
bool(p == fp) -> False
bool(fp = &i) -> False

ptr = p -> True;
ptr = fp; -> True
ptr = &i; -> True

// WHy use void pointers? >> This pointer can be assigned to any data type. 

// Function pointers give us the ability to pass functions themselves as parameters.
// THis is also known as polymorphism.

malloc() and calloc() return the void pointer, hence they are used to allocate memory for any data type.

void printfHello(void) 
void (*)(void)

data type specification is simple. You remove the name of the function with a pointer name.
int x; -> variable of type int.
void *func -> variable of type function pointer.


Syntax for function pointer: return type (*ptr_name)(type1, type2, ...);
Eg: int (*p)(int);

// Functional programming. TBdisCUSSed.

int strcmp(char *s1, char *s2);
int (*comparator)(char *s1, char *s);

float (*fp)(int, int);   // fp here means function pointer.
float func(int, int);
fp = func;

int get_rec_by_key() ; 

given a particular key, you search for the rec stored by it and then retrieve the record.

int (*comparator)(void *rec, char *search_value) -> What does this even signify? 

contact.h -> application specific theme.
YOu don't include contact.h in pds.c
in contact.c you write a name matcher and the prototype remains as it was earlier.
It was okay for contact.c to peek into that record.
in order to peek into record, it can't be a void pointer, hence you typecast it.
SO you typecast (struct COntact *)rec;
struct Contact *cplre = (struct COntact *)rec;

YOu can't use name_matcher in pds.c
Name matcher should be written in contact.c. 
int name_matcher(void *rec, char *search_rec) {

struct Contact *cplre - (struct COntact *)rec;
strcmp(rec->name, search_value);
}
float func(int, int);
result = func(a, b);

result = (*fp)(a, b);
result = fp(a, b);


// Same pds.c, you copy paste here, you have to call the matcher.

// Here, you need to copy paste the same linear search loop, you need to re-write the logic, you need to use the matcher function instead of finding it by a key.

int get_rec_by_field( void *search_value, void *rec, (*matcher) );

// contact.c is the new fiel you need to create.
// the important part is to write the contact_name_matcher.

search into key_search. in the .h file, you need to use the new pds.h file.

// Contact name search. Have a look at the macros. 
// New keyword KEY_SEARCH.
// Submit contact.c and pdsv3.c
// Prototype is present in pds.
// Play with function_pointer demo.
// YOu will notice that the prototypes should match and etc etc..
// New pds_tester is also available.

// SO, what should we do in the files right now? 
// name_matcher should match with contact name.

Differences between ( pds.h in lab3 ) and ( pds.h in lab4 ) :

MACRO REC_NOT_FOUND is 3 in lab3 while, it is -1 in lab4.

MACRO REPO_ALREADY_OPEN is 4 in lab3 while, it is 12 in lab4.
MACRO new defined: NDX_SAVE_FAILED.


// Primary focus is for all of us to complete it on our own.
// NO late submission penalties as of now.
// No shortcuts.
// Zero late penalties.
// Mid term -> Programming exam.
// THe full functionality of pds should be known.
// ONe search enhancement ot pds willl be given to us which we need ot solve in a matter of minutes.
// YOu will be given a working version of pds, and tou should be able ot modify the tester to know abou tthe new aenchance ment.
// THis means that we should be familiar with the tester.
// THat's how the mid term exam is conducted.
// 10 minutes quiz.
// PLan for the mid term.
// implementation of binary search tree.
// THere will not be any change to tester, testcase and you should be modifying only the bst.
