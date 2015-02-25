#include <stdio.h>

int M[15][15];

int PATTERN[2][2] = {{1, 0}, {0, 1}};
unsigned long long int COUNT;

int MAX = 5;

void printM(int n, int k) {
  int i, j;
  for (i = 0; i < n; i++) {
    for (j = 0; j < k; j++) {
      printf("%d ", M[i][j]);
    }
    printf("\n");
  }
  printf("\n");
}

int avoidsPartialPattern(int n, int k, int n2, int k2) {
  int i, j;
  int hit;
  if ((k2 == 0) || (n2 == 0))
    return 1;
  for (i = 0; i < n2; i++) {
    for (j = 0; j < k2; j++) {
      hit = 0;
      hit += (M[i][j] == PATTERN[0][0]);
      hit += (M[i][k2] == PATTERN[0][1]);
      hit += (M[n2][j] == PATTERN[1][0]);
      hit += (M[n2][k2] == PATTERN[1][1]);
      if (hit == 4)
        return 0;
      hit = 0;
      hit += (M[i][j] == !PATTERN[0][0]);
      hit += (M[i][k2] == !PATTERN[0][1]);
      hit += (M[n2][j] == !PATTERN[1][0]);
      hit += (M[n2][k2] == !PATTERN[1][1]);
      if (hit == 4)
        return 0;
    }
  }

  return 1;
}

void dfs_sub(int n, int k, int n2, int k2) {
  if (!avoidsPartialPattern(n, k, n2, k2))
    return;
  if (k2 < (k - 1)) {
    k2++;
    M[n2][k2] = 0;
    dfs_sub(n, k, n2, k2);
    M[n2][k2] = 1;
    dfs_sub(n, k, n2, k2);
  } else {
    n2++;
    k2 = 0;
    if (n2 == n) {
      COUNT++;
      return;
    }
    M[n2][k2] = 0;
    dfs_sub(n, k, n2, k2);
    M[n2][k2] = 1;
    dfs_sub(n, k, n2, k2);
    return;
  }
}

void dfs(int n, int k) {
  COUNT = 0;
  M[0][0] = 0;
  dfs_sub(n, k, 0, 0);
  M[0][0] = 1;
  dfs_sub(n, k, 0, 0);
  printf("%llu ZeroOne matrices of size %d %d \n ", COUNT, n, k);
}

int main() {
  int i, j;
  for (i = 1; i < MAX; i++) {
    for (j = 1; j < MAX; j++) {
      dfs(i, j);
    }
  }
  return 0;
}
