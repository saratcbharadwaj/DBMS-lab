#include <stdio.h>
#include <string.h>

#define LOG(x) printf("%s\n",x)

struct Record{
  char name[30];
  int age;
};

int main()
{
  FILE *fp;
  char *filename = "demo.dat";
  int key1, key2, filekey;
  struct Record rec1, rec2, filerec;
  
  LOG("Opening file for binary write");
  if((fp=fopen(filename, "wb")) == NULL){
    printf("Error opening file %s\n", filename);
    return 1;
  }
  
  key1=101;
  strcpy(rec1.name, "Hello1");
  rec1.age=51;
  
  key2=102;
  strcpy(rec2.name, "Hello2");
  rec2.age=52;

  LOG("Writing first <key,record> pair");
  fwrite(&key1, sizeof(int), 1, fp);
  fwrite(&rec1, sizeof(struct Record), 1, fp);

  LOG("Writing second <key,record> pair");
  fwrite(&key2, sizeof(int), 1, fp);
  fwrite(&rec2, sizeof(struct Record), 1, fp);
  
  fclose(fp);
  
  //==================================================================================
  LOG("Opening file for binary read in loop");
  if((fp=fopen(filename, "rb")) == NULL){
    printf("Error opening file %s\n", filename);
    return 1;
  }
  
  while( fread(&filekey, sizeof(int), 1, fp) > 0 ){
    fread(&filerec, sizeof(struct Record), 1, fp);
    printf("Key read: %d; Filerec.name=%s;  filerec.age=%d\n", filekey, filerec.name, filerec.age);
  }
  fclose(fp);
  
  return 0;
}

