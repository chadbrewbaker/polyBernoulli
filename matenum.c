#include <stdio.h>

int M[15][15];

/*#ruby
vals = [0,1]
vals = vals.repeated_permutation(2).to_a
mats = vals.repeated_permutation(2).to_a.inspect.to_s
puts mats.gsub(/]/ , '}').gsub(/\[/ , '{')
*/
int PATTERN[16][2][2] = {{{0, 0}, {0, 0}},
                         {{0, 0}, {0, 1}},
                         {{0, 0}, {1, 0}},
                         {{0, 0}, {1, 1}},
                         {{0, 1}, {0, 0}},
                         {{0, 1}, {0, 1}},
                         {{0, 1}, {1, 0}},
                         {{0, 1}, {1, 1}},
                         {{1, 0}, {0, 0}},
                         {{1, 0}, {0, 1}},
                         {{1, 0}, {1, 0}},
                         {{1, 0}, {1, 1}},
                         {{1, 1}, {0, 0}},
                         {{1, 1}, {0, 1}},
                         {{1, 1}, {1, 0}},
                         {{1, 1}, {1, 1}}};
#define PRINT_MASKS_TOGGLE 0

int PATTERN_MASK;
#define CHECK_BIT(var,pos) ((var) & ( 1 << (pos)))

unsigned long long int COUNT;
int MAX = 5;

void printPatternMask() {
  int i;
  for (i = 0; i < 16; i++) {
    printf("%d", !!CHECK_BIT(PATTERN_MASK,i) ); 
  }
  printf("\n");
}

int nextSubset() {
  PATTERN_MASK++;
  /*Quit when we overflow*/
  if ( PATTERN_MASK == 65536) 
    return 0;
  else
    return 1;
}

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

int avoidsPartialPatterns(int n, int k, int n2, int k2) {
  int i, j;
  int hit;
  int mask;
  if ((k2 == 0) || (n2 == 0))
    return 1;
  for (i = 0; i < n2; i++) {

    for (j = 0; j < k2; j++) {

      mask = 0;
      for (mask = 0; mask < 16; mask++) {
        if (CHECK_BIT(PATTERN_MASK,mask)) {
          hit = 0;
          hit += (M[i][j] == PATTERN[mask][0][0]);
          hit += (M[i][k2] == PATTERN[mask][0][1]);
          hit += (M[n2][j] == PATTERN[mask][1][0]);
          hit += (M[n2][k2] == PATTERN[mask][1][1]);
          if (hit == 4)
            return 0;
        }
      }
    }
  }
  return 1;
}

void dfs_sub(int n, int k, int n2, int k2) {
  if (!avoidsPartialPatterns(n, k, n2, k2))
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
  printf("%llu %d %d ", COUNT, n, k);
}

int main() {
  int i, j;
  PATTERN_MASK=0; 
 while (nextSubset()) {
    for (i = 1; i < MAX; i++) {
      for (j = 1; j < MAX; j++) {
        dfs(i, j);
      }
    }
     if(PRINT_MASKS_TOGGLE)
     	printPatternMask();
     else
        printf("\n");
  }
  return 0;
}
