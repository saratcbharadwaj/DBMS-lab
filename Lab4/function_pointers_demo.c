#include<stdio.h>

void somefun1(char *s){printf ("somefun1 called with %s\n", s);}

void somefun2(char *s){printf ("somefun2 called with %s\n", s);}

void fpdemo(char *normalparam, void (*fpointer)(char *));

int main()
{
    char *s = "Hello";

    printf("Calling fpdemo with somefun1 in main\n");
    fpdemo(s, somefun1);

    printf("Calling fpdemo with somefun2 in main\n");
    fpdemo(s, somefun2);

    return 0;
}

void fpdemo(char *normalparam, void (*fpointer)(char *))
{
    printf("Calling function pointer with param %s in fpdemo\n", normalparam);
    fpointer(normalparam);
}

