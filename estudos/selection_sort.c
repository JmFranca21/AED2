#include <stdio.h>

int Selection(int n, int v[]) {
  int i, j, menor, aux;

  for(i = 0; i < n-1; i++) {
    menor = i;

    for(j = i+1; j<n; j++) {
      if (v[j] < v[menor]) {
        menor = j;
      }
    }
    aux = v[i];
    v[i] = v[menor];
    v[menor] = aux;
  }

  return 0;
}

int main() {
  int v[5] = {5, 3, 1, 4, 2};
  int i;
  int n = 5;

  Selection(n, v);
  
  for(i = 0; i < 5; i++) {
    printf(" %d |", v[i]);
  }

  return 0;
}