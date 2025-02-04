#include <stdio.h>

void sort_array(int a[]);
int main() {

  int a[20];
  int num, pos;
  
  for (int i = 0; i < 20; i++) {
  
      printf("Enter value of element: %d", a[i]);
      scanf("%d", &a[i]);
      
  }
  
  printf("Enter the value of num: ");
  scanf("%d", &num);
  
  sort_array(a);
  
  pos = give_pos(a, num);
  
  if (pos == -1) {

    printf("Element does not exist");
    
  }
  
  else {

    printf("Element exists!");
    
  }

  return 0;
  
}

void sort_array(int a[]) {
    
  int leng = sizeof(a)/sizeof(a[0]);
  for (int i = 0; i < leng- 1; i++) {
 
 
      for (int j = 0; j < leng - i - 1; j++) {
      
          if (a[i] > a[j + 1]) {

            int temp = ar[j];
            a[j] = a[j + 1];
            a[j + 1] = temp;
            
          }
        
      }
      
  }
  
}

int give_pos(a, num) {

  int lb, ub, mid;
  
  lb = 0;
  ub = len(a) - 1;
  
  while (lb <= ub) {
  
      mid = (lb + ub)/2;
      
      if ( num == a[mid]) {
      
          return mid;
      }    
      else if ( num > a[mid]) {

              lb = mid + 1;
          
      }

      else if ( num < a[mid] ) {

              ub = mid - 1;
              
      }
    
    return -1;
    
  }
  
}

    

  

          
  
