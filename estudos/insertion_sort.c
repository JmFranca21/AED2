#include <stdio.h>
#include <stdlib.h>

int Insertion(int n, int v[]) {
  int i, j, aux;

  for(i = 1; i < n; i++) {
    aux = v[i];
    j = i - 1;

    while(j >= 0 && aux < v[j]) {
      v[j+1] = v[j];
      j--;
    }

    v[j+1] = aux;
  }

  return 0;
}

int main () {
  int v[5] = {5, 3, 1, 4, 2};
  int i;
  int n = 5;

  Insertion(n, v);

  for(i = 0; i < 5; i++) {
    printf(" %d |", v[i]);
  }

  return 0;
}