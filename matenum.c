#include <stdio.h>
#include <stdlib.h>
#define MAX 5
int M[MAX][MAX];

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
#define PRINT_COUNTS_TOGGLE 0
#define LATEX_TOGGLE 1

int PATTERN_MASK;
#define CHECK_BIT(var, pos) ((var) & (1 << (pos)))

unsigned long long int COUNT;

struct MatrixSums {
  int pattern_mask;
  unsigned long long counts[MAX][MAX];
};

struct MatrixSums ALL_RESULTS[65536];

void printMatrixSums(struct MatrixSums ms) {
  int i, j;
  for (i = 1; i < MAX; i++) {
    for (j = 1; j < MAX; j++) {
      printf("%llu ", ms.counts[i][j]);
    }
    printf("\n");
  }
  printf("\n");
}

void printMatrixSumsInLatex(struct MatrixSums ms) {
  int i, j;
  printf("$$\\left[ \\begin{array}{");
  for (i = 1; i < MAX; i++)
    printf("c");
  printf("}\n");
  for (i = 1; i < MAX; i++) {
    for (j = 1; j < MAX; j++) {
      printf("%llu ", ms.counts[i][j]);
      if ((j + 1) < MAX)
        printf(" & ");
    }
    if ((i + 1) < MAX)
      printf(" \\\\ \n");
  }
  printf(" \\end{array} \\right] $$\n");
}

int cmpMats(unsigned long long a[MAX][MAX], unsigned long long b[MAX][MAX]) {
  int i, j;
  for (i = 0; i < MAX; i++) {
    for (j = 0; j < MAX; j++) {
      if (a[i][j] < b[i][j])
        return -1;
      if (a[i][j] > b[i][j])
        return 1;
    }
  }
  return 0;
}

int cmpfunc(const void *a, const void *b) {

  return (cmpMats(((struct MatrixSums *)a)->counts,
                  ((struct MatrixSums *)b)->counts));
}

void printPatternMask() {
  int i;
  for (i = 0; i < 16; i++) {
    printf("%d", !!CHECK_BIT(PATTERN_MASK, i));
  }
  printf("\n");
}

void print2x2MatrixInLatex(int i) {

  printf(" \\left( \\begin{array}{cc}\n");
  printf("%d & %d  \\\\ \n", PATTERN[i][0][0], PATTERN[i][0][1]);
  printf("%d & %d \\end{array} \\right) \n", PATTERN[i][1][0],
         PATTERN[i][1][1]);
}

void printPatternMaskInLatex(int mask) {
  int i, count;
  count = 0;
  printf("$$\\left\\{ ");
  for (i = 0; i < 16; i++) {
    if (!!CHECK_BIT(mask, i)) {
      print2x2MatrixInLatex(i);
      count++;
      if (count == 8)
        printf("\\\\\n");
    }
  }
  printf(" \\right\\} $$\n");
}

int nextSubset() {
  PATTERN_MASK++;
  /*Quit when we overflow*/
  if (PATTERN_MASK == 65536)
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
        if (CHECK_BIT(PATTERN_MASK, mask)) {
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
  if (PRINT_COUNTS_TOGGLE)
    printf("%llu ", COUNT);
  ALL_RESULTS[PATTERN_MASK].counts[n][k] = COUNT;
  ALL_RESULTS[PATTERN_MASK].pattern_mask = PATTERN_MASK;
}

int main() {
  int i, j;

  if (LATEX_TOGGLE) {
    system("cat prelude.tex");
  }

  PATTERN_MASK = 0;
  do {
    for (i = 1; i < MAX; i++) {
      for (j = 1; j < MAX; j++) {
        dfs(i, j);
      }
    }
    if (PRINT_MASKS_TOGGLE)
      printPatternMask();
    else if (PRINT_COUNTS_TOGGLE)
      printf("\n");
  } while (nextSubset());
  /*for(i=0;i<16;i++)
    print2x2MatrixInLatex(i);*/

  if (LATEX_TOGGLE) {
    qsort(ALL_RESULTS, 65536, sizeof(struct MatrixSums), cmpfunc);

    printMatrixSumsInLatex(ALL_RESULTS[0]);
    printf("\\\\\n");
      PATTERN_MASK = ALL_RESULTS[0].pattern_mask;
      printPatternMask();
    //printPatternMaskInLatex(ALL_RESULTS[0].pattern_mask);

    for (i = 1; i < 65536; i++) {
      if (cmpMats(ALL_RESULTS[i].counts, ALL_RESULTS[i - 1].counts) != 0) {
        printf("\\\\\n");
        printf("\\\\\n");
        printMatrixSumsInLatex(ALL_RESULTS[i]);
      }
      printf("\\\\\n");
        PATTERN_MASK =ALL_RESULTS[i].pattern_mask;
        printPatternMask();
      //printPatternMaskInLatex(ALL_RESULTS[i].pattern_mask);
    }
    printf("\\end{document}");
  }
  return 0;
}
