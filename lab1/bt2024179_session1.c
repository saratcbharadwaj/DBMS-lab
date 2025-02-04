# include <stdio.h> 

struct student {
  int rollnum;
  char name[30];
  int age;
};

int save_num_text( char *filename ) {
//write your code
FILE *fptr = fopen(filename, "w");

int a[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20};

for (int i = 0; i < 20; i++) {

fprintf(fptr, "%d ", a[i]);

}

fclose(fptr);
return 0;

}


int read_num_text( char *filename ) {
  //write your code
FILE *fptr1= fopen(filename, "r");

if ( fptr1 == NULL ) {

    printf("Error opening file\n");
    return 2;
    }

int a1[20];
int i = 0;

while (i < 20 && fscanf(fptr1, "%d", &a1[i]) == 1) {

i++;

}

for (int j = 0; j < 20; j++) {

printf("%d\n", a1[j]);

}

fclose(fptr1);
return 0;

}


int save_struct_text( char *filename ) {
  //write your code
  FILE *fptr2 = fopen(filename, "w");

  struct student studentsinfo[5] = {

{1, "aman", 18},
{2, "bman", 18},
{3, "cman", 18},
{4, "dman", 18},
{5, "eman", 18} };

for (int i = 0; i < 5; i++) {

fprintf(fptr2, "%d %s %d\n", studentsinfo[i].rollnum, studentsinfo[i].name, studentsinfo[i].age);

}
 
 fclose(fptr2);
 return 0;

}


int read_struct_text( char *filename ) {
//write your code
FILE *fptr3 = fopen(filename, "r");

if (fptr3 == NULL) {

    printf("Error opening file\n");
    return 4;
    
    }

struct student studentsinfo2[5];
int i = 0;

while ( i < 5 && fscanf(fptr3, "%d %s %d", &studentsinfo2[i].rollnum, studentsinfo2[i].name, &studentsinfo2[i].age) == 3) {
i++;

}
for (int j = 0;j < i; j++) {

printf("%d %s %d\n", studentsinfo2[j].rollnum, studentsinfo2[j].name, studentsinfo2[j].age);

}

fclose(fptr3);
return 0;

}

int save_num_binary( char *filename ) {
//write your code
FILE *fptr4 = fopen("filename", "w");

int a[20] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20};

for (int i = 0; i <= 19; i++) {

int count = sizeof(a) / sizeof(a[0]);
for (int j = 0; j < 20; j ++) {
size_t numbers = fwrite(&a[i], sizeof(int), count, fptr4);
}
}

fclose(fptr4);
return 0;
}
int read_num_binary( char *filename ) {
 //write your code
FILE *fptr5 = fopen("filename", "r");
if ( fptr5 == NULL) {

    printf("Error opening file\n");
    return 6;
    }

int nums[20];

size_t elements_read = fread(nums, sizeof(int), 20, fptr5);

for (int i = 0; i < elements_read; i++) {

printf("%d\n", nums[i]);

}

fclose(fptr5);
return 0;

}

int save_struct_binary( char *filename ) {
  //write your code
  FILE *fptr6 = fopen(filename, "w");

  struct student studentsinfo[5] = {
  
  {1, "aman", 18},
  {2, "bman", 18},
  {3, "cman", 18},
  {4, "dman", 18},
  {5, "eman", 18} 
                   };
  int i = 0;       
  
  while (i < 5) {
      
      if (i == 5) {
          break;
      }
      
      fwrite(&studentsinfo[i], sizeof(struct student), 3, fptr6);
      i++;
}

  fclose(fptr6);
  return 0;
  
  }

int read_struct_binary( char *filename ) {
 //write your code
 FILE *fptr7 = fopen(filename, "r");
 if (fptr7 == NULL) {

    printf("Error opening file");
    return 8;
    }
 struct student studentsinfo2[5];
 
 for (int i = 0; i < 5; i++) {
     
     fread(&studentsinfo2[i], sizeof(struct student), 3, fptr7);
     printf("%d %s %d\n", studentsinfo2[i].rollnum, studentsinfo2[i].name, studentsinfo2[i].age);

}

fclose(fptr7);

return 0;

}

