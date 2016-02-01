#include <stdio.h>
#include <stdlib.h>
#define MAX 7
// 	3m33.418s for MAX 10
// 	16m40.840s for MAX 11
// real	242m22.820s for MAX 12
int M[MAX][MAX];

/*#ruby
vals = [0,1]
vals = vals.repeated_permutation(2).to_a
mats = vals.repeated_permutation(2).to_a.inspect.to_s
puts mats.gsub(/]/ , '}').gsub(/\[/ , '{')
*/
int PATTERN[16][2][2] = {
    {{0, 0}, {0, 0}}, {{0, 0}, {0, 1}}, {{0, 0}, {1, 0}}, {{0, 0}, {1, 1}},
    {{0, 1}, {0, 0}}, {{0, 1}, {0, 1}}, {{0, 1}, {1, 0}}, {{0, 1}, {1, 1}},
    {{1, 0}, {0, 0}}, {{1, 0}, {0, 1}}, {{1, 0}, {1, 0}}, {{1, 0}, {1, 1}},
    {{1, 1}, {0, 0}}, {{1, 1}, {0, 1}}, {{1, 1}, {1, 0}}, {{1, 1}, {1, 1}}};
#define PRINT_MASKS_TOGGLE 0
#define PRINT_COUNTS_TOGGLE 0
#define LATEX_TOGGLE 1
#define EQUIVALENCE_CLASSES_TOGGLE 0

#define CHECK_BIT(var, pos) ((var) & (1 << (pos)))
#define SET_BIT(var, pos) ((var) | (1 << (pos)))

struct Pattern {
  int aa;
  int ab;
  int ba;
  int bb;
  int index;
};

struct Pattern PATTERNS[16];
int ROT_PERM[16];
int COMP_PERM[16];
int VERT_FLIP_PERM[16];
int HORIZ_FLIP_PERM[16];

typedef enum { VERT, HORIZ, COMP, ROT } operation;

void printOpLatex(FILE *f, operation op) {
  if (op == VERT)
    fprintf(f, "\\uparrow ");
  if (op == HORIZ)
    fprintf(f, "\\leftarrow ");
  if (op == COMP)
    fprintf(f, "\\not ");
  if (op == ROT)
    fprintf(f, "\\searrow ");
}

struct MatrixSums {
  int pattern_mask;
  unsigned long long counts[MAX][MAX];
  int isTranspose;
};

operation OPS_FROM_CANNON[65536][16]; // Making assuption that 16 is enough
int OPS_LENGTH[65536];

struct MatrixSums ALL_RESULTS[65536];

int CANNONICAL_INDEX[65536];

int matchPatterns(struct Pattern p1, struct Pattern p2) {
  if (p1.aa != p2.aa) {
    return 0;
  }
  if (p1.ab != p2.ab) {
    return 0;
  }
  if (p1.ba != p2.ba) {
    return 0;
  }
  if (p1.ab != p2.ab) {
    return 0;
  }
  return 1;
}

void fillPatterns() {
  int i;
  for (i = 0; i < 16; i++) {
    PATTERNS[i].aa = PATTERN[i][0][0];
    PATTERNS[i].ab = PATTERN[i][0][1];
    PATTERNS[i].ba = PATTERN[i][1][0];
    PATTERNS[i].bb = PATTERN[i][1][1];
    PATTERNS[i].index = i;
  }
}

struct SmallList {
  int vals[16];
  int length;
};

struct CannonList {
  int vals[1024];
  int length;
};

// Assumes lists have been sorted
int eqSmallLists(struct SmallList l1, struct SmallList l2) {
  int i;
  if (l1.length < l2.length)
    return -1;
  if (l1.length > l2.length)
    return 1;
  for (i = 0; i < l1.length; i++) {
    if (l1.vals[i] < l2.vals[i])
      return -1;
    if (l1.vals[i] > l2.vals[i])
      return 1;
  }
  return 0;
}

// Assumes lists have been sorted
int eqCannonLists(struct CannonList l1, struct CannonList l2) {
  int i;
  if (l1.length < l2.length)
    return -1;
  if (l1.length > l2.length)
    return 1;
  for (i = 0; i < l1.length; i++) {
    if (l1.vals[i] < l2.vals[i])
      return -1;
    if (l1.vals[i] > l2.vals[i])
      return 1;
  }
  return 0;
}

// bubblesort for non-negative ints
void sortSmallList(struct SmallList *l) {
  int i, j, swap;
  for (i = 0; i < l->length; i++) {
    swap = -1;
    for (j = i; j < l->length - 1; j++) {
      if (l->vals[j] > l->vals[j + 1]) {
        swap = l->vals[j + 1];
        l->vals[j + 1] = l->vals[j];
        l->vals[j] = swap;
      }
    }
    if (swap == -1)
      return;
  }
}

// bubblesort for non-negative ints
void sortCannonList(struct CannonList *l) {
  int i, j, swap;
  for (i = 0; i < l->length; i++) {
    swap = -1;
    for (j = i; j < l->length - 1; j++) {
      if (l->vals[j] > l->vals[j + 1]) {
        swap = l->vals[j + 1];
        l->vals[j + 1] = l->vals[j];
        l->vals[j] = swap;
      }
    }
    if (swap == -1)
      return;
  }
}

int insert(struct CannonList *l, int newVal) {
  int i;
  for (i = 0; i < l->length; i++) {
    if (l->vals[i] == newVal)
      return 0;
  }
  l->vals[l->length] = newVal;
  l->length++;
  return 1;
}

void mergeLists(struct SmallList *l1, struct SmallList *l2,
                struct SmallList *l3) {
  int i, j, k;
  i = j = k = 0;
  struct SmallList lMerged;
  lMerged.length = 0;
  sortSmallList(l1);
  sortSmallList(l2);

  while (1) {
    if (i >= l1->length && j >= l2->length)
      break;

    if (k > 0) { // Remove dupes
      if (i < l1->length) {
        if (l1->vals[i] == lMerged.vals[k]) {
          i++;
          continue;
        }
      }
      if (j < l2->length) {
        if (l2->vals[j] == lMerged.vals[k]) {
          j++;
          continue;
        }
      }
    }

    if (i >= l1->length || j >= l2->length) {
      if (i >= l1->length) {
        lMerged.vals[k] = l2->vals[j];
        j++;
      } else {
        lMerged.vals[k] = l1->vals[i];
        i++;
      }

    } else {
      if (l1->vals[i] < l2->vals[j]) {
        lMerged.vals[k] = l1->vals[i];
        i++;
      } else {
        lMerged.vals[k] = l2->vals[j];
        j++;
      }
    }

    k++;
  }

  lMerged.length = k;
  l3->length = lMerged.length;
  for (i = 0; i < lMerged.length; i++) {
    l3->vals[i] = lMerged.vals[i];
  }
}

void mergeCannonLists(struct CannonList *l1, struct CannonList *l2,
                struct CannonList *l3) {
  int i, j, k;
  i = j = k = 0;
  struct CannonList lMerged;
  lMerged.length = 0;
  sortCannonList(l1);
  sortCannonList(l2);

  while (1) {
    if (i >= l1->length && j >= l2->length)
      break;

    if (k > 0) { // Remove dupes
      if (i < l1->length) {
        if (l1->vals[i] == lMerged.vals[k]) {
          i++;
          continue;
        }
      }
      if (j < l2->length) {
        if (l2->vals[j] == lMerged.vals[k]) {
          j++;
          continue;
        }
      }
    }

    if (i >= l1->length || j >= l2->length) {
      if (i >= l1->length) {
        lMerged.vals[k] = l2->vals[j];
        j++;
      } else {
        lMerged.vals[k] = l1->vals[i];
        i++;
      }

    } else {
      if (l1->vals[i] < l2->vals[j]) {
        lMerged.vals[k] = l1->vals[i];
        i++;
      } else {
        lMerged.vals[k] = l2->vals[j];
        j++;
      }
    }

    k++;
  }

  lMerged.length = k;
  l3->length = lMerged.length;
  for (i = 0; i < lMerged.length; i++) {
    l3->vals[i] = lMerged.vals[i];
  }
}

void indexToBinVector(int index, int vector[16]) {}

int binVectorToIndex(int vector[16]) {
  int index = 0;
  return index;
}

int patternIndex(struct Pattern p) {
  int i;
  for (i = 0; i < 16; i++) {
    if (matchPatterns(p, PATTERNS[i]))
      return i;
  }
  fprintf(stderr, "ERROR Should never get here");
  return -1;
}

void complementPattern(struct Pattern p, struct Pattern *complement) {
  complement->aa = !(p.aa);
  complement->ab = !(p.ab);
  complement->ba = !(p.ba);
  complement->bb = !(p.bb);
  complement->index = patternIndex(*complement);
}

void vertFlipPattern(struct Pattern p, struct Pattern *vertFlip) {
  vertFlip->aa = p.ba;
  vertFlip->ab = p.bb;
  vertFlip->ba = p.aa;
  vertFlip->bb = p.ab;
  vertFlip->index = patternIndex(*vertFlip);
}

void horizFlipPattern(struct Pattern p, struct Pattern *horizFlip) {
  horizFlip->aa = p.ab;
  horizFlip->ab = p.aa;
  horizFlip->ba = p.bb;
  horizFlip->bb = p.ba;
  horizFlip->index = patternIndex(*horizFlip);
}

void rotatePattern(struct Pattern p, struct Pattern *rotated) {
  rotated->aa = p.ab;
  rotated->ab = p.bb;
  rotated->ba = p.aa;
  rotated->bb = p.ba;
  rotated->index = patternIndex(*rotated);
}

void fillPerms(){
    int i;
    struct Pattern out;
    
    for(i=0;i<16;i++){
        ROT_PERM[i] = -1;
        COMP_PERM[i] = -1;
        VERT_FLIP_PERM[i] = -1;
        HORIZ_FLIP_PERM[i] = -1;
    }
    

    for(i=0;i< 16;i++){
        rotatePattern(PATTERNS[i],&out);
        ROT_PERM[i]= out.index;
    }
    
    for(i=0;i< 16;i++){
        complementPattern(PATTERNS[i],&out);
        COMP_PERM[i]= out.index;
    }
    
    for(i=0;i< 16;i++){
        horizFlipPattern(PATTERNS[i],&out);
        HORIZ_FLIP_PERM[i]= out.index;
    }
    
    for(i=0;i< 16;i++){
        vertFlipPattern(PATTERNS[i],&out);
        VERT_FLIP_PERM[i]= out.index;
    }
    
    fprintf(stderr, "\nROT_PERM");
    for(i=0;i<16;i++)
        fprintf(stderr, "%d ", ROT_PERM[i]);
     fprintf(stderr, "\nCOMP_PERM");
    for(i=0;i<16;i++)
        fprintf(stderr, "%d ", COMP_PERM[i]);
     fprintf(stderr, "\nVERT_FLIP_PERM");
    for(i=0;i<16;i++)
        fprintf(stderr, "%d ", VERT_FLIP_PERM[i]);
     fprintf(stderr, "\nHORIZ_FLI_PERM");
    for(i=0;i<16;i++)
        fprintf(stderr, "%d ", HORIZ_FLIP_PERM[i]);
    
}
/*
 void rotatePattern2(struct Pattern p, struct Pattern *rotated2) {
 rotatePattern(p, rotated2);
 rotatePattern(p, rotated2);
 }

 void rotatePattern3(struct Pattern p, struct Pattern *rotated3) {
 rotatePattern(p, rotated3);
 rotatePattern(p, rotated3);
 rotatePattern(p, rotated3);
 }*/

/*

int rotateForbs(int index) {
    int i;
    int result = 0;
    for (i = 0; i < 16; i++) {
        if (CHECK_BIT(index, i)) {
            SET_BIT(result, ROT_PERM[i]  );
        }
    }
    return result;
}



int horizFlipForbs(int index) {
    int i;
    int result = 0;
    for (i = 0; i < 16; i++) {
        if (CHECK_BIT(index, i)) {
            SET_BIT(result, HORIZ_FLIP_PERM[i]);
        }
    }
    return result;
}

int vertFlipForbs(int index) {
    int i;
    int result = 0;
    for (i = 0; i < 16; i++) {
        if (CHECK_BIT(index, i)) {
            SET_BIT(result, VERT_FLIP_PERM[i]);
        }
    }
    return result;
}

int complementForbs(int index) {
  int i;
  int result = 0;
  for (i = 0; i < 16; i++) {
    if (CHECK_BIT(index, i)) {
      SET_BIT(result, COMP_PERM[i]);
    }
  }
    return result;
}

*/

int applyPerm(int index, int *perm) {
  int i;
  int result = 0;
  for (i = 0; i < 16; i++) {
    if (CHECK_BIT(index, i)) {
      SET_BIT(result, perm[i]);
    }
  }
  return result;
}

void calculateCannonicalIndex() {
  int i, j;
  int r, v, h, c;
  fillPatterns();
  fillPerms();
  struct CannonList list;
  for (i = 0; i < 65536; i++) {
    CANNONICAL_INDEX[i] = i;
    OPS_LENGTH[i] = 0;
  }
  for (i = 0; i < 65536; i++) {
    if (CANNONICAL_INDEX[i] == i) {

      list.length = 1;
      list.vals[0] = i;

      for (j = 0; j < list.length; j++) {
        r = applyPerm(list.vals[j], ROT_PERM);
        h = applyPerm(list.vals[j], HORIZ_FLIP_PERM);
        v = applyPerm(list.vals[j], VERT_FLIP_PERM);
        c = applyPerm(list.vals[j], COMP_PERM);

        insert(&list, r);
        insert(&list, h);
        insert(&list, v);
        insert(&list, c);
      }

      for (j = 0; j < list.length; j++)
        CANNONICAL_INDEX[list.vals[j]] = i;
    }
 }
}

int PATTERN_MASK;

unsigned long long int COUNT;
int MAX_EXPONENT;

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
  for (i = 1; i < MAX - 1; i++)
    printf("c");
  printf("}\n");
  for (i = 1; i < MAX - 1; i++) {
    for (j = 1; j < MAX - 1; j++) {
      if (i * j <= MAX_EXPONENT) {

        printf("%llu ", ms.counts[i][j]);
      } else {
        printf("\? ");
      }
      if ((j + 1) < MAX - 1)
        printf(" & ");
    }
    if ((i + 1) < MAX - 1)
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

void toMinTranspose() {
  int i, j, k;
  unsigned long long transCounts[MAX][MAX];
  for (i = 0; i < 65536; i++) {
    for (j = 0; j < MAX; j++) {
      for (k = 0; k < MAX; k++) {
        transCounts[k][j] = ALL_RESULTS[i].counts[j][k];
      }
    }
    if (cmpMats(transCounts, ALL_RESULTS[i].counts) < 0) {
      for (j = 0; j < MAX; j++) {
        for (k = 0; k < MAX; k++) {
          ALL_RESULTS[i].counts[j][k] = transCounts[j][k];
        }
      }
      ALL_RESULTS[i].isTranspose = 1;
    } else {
      ALL_RESULTS[i].isTranspose = 0;
    }
  }
}

int cmpfunc(const void *a, const void *b) {

  return (cmpMats(((struct MatrixSums *)a)->counts,
                  ((struct MatrixSums *)b)->counts));
}

void printPatternMask(FILE *f) {
  int i;
  for (i = 0; i < 16; i++) {
    fprintf(f, "%d", !!CHECK_BIT(PATTERN_MASK, i));
  }
  fprintf(f, "\n");
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
  if (EQUIVALENCE_CLASSES_TOGGLE)
    calculateCannonicalIndex();

  if (LATEX_TOGGLE) {
    system("cat prelude.tex");
  }
  MAX_EXPONENT = 0;
  for (i = 1; i < MAX; i++) {
    for (j = 1; j < MAX; j++) {
      if ((i + j < MAX) || j == 2 || i == 2) {
        if (MAX_EXPONENT < i * j)
          MAX_EXPONENT = i * j;
      }
    }
  }

  PATTERN_MASK = 0;
  do {
    for (i = 1; i < MAX; i++) {
      for (j = 1; j < MAX; j++) {
        if (i * j <= MAX_EXPONENT) {
          dfs(i, j);
        } else {
          if (i < MAX && j < MAX)
            ALL_RESULTS[PATTERN_MASK].counts[i][j] = 0;
        }
      }
    }
    if (PRINT_MASKS_TOGGLE)
      printPatternMask(stderr);
    else if (PRINT_COUNTS_TOGGLE)
      printf("\n");
  } while (nextSubset());
  /*for(i=0;i<16;i++)
    print2x2MatrixInLatex(i);*/

  if (LATEX_TOGGLE) {
    toMinTranspose();

    int wilfClass = 0;
    qsort(ALL_RESULTS, 65536, sizeof(struct MatrixSums), cmpfunc);

    printMatrixSumsInLatex(ALL_RESULTS[0]);
    printf("\\\\\n");
    printf("Wilf Class %d\\\\\n", wilfClass++);
    if (ALL_RESULTS[0].isTranspose)
      printf("Transpose ");
    PATTERN_MASK = ALL_RESULTS[0].pattern_mask;
    printPatternMask(stdout);
    // printPatternMaskInLatex(ALL_RESULTS[0].pattern_mask);

    for (i = 1; i < 65536; i++) {
      if (cmpMats(ALL_RESULTS[i].counts, ALL_RESULTS[i - 1].counts) != 0) {
        printf("\\\\\n");
        printf("\\\\\n");
        printMatrixSumsInLatex(ALL_RESULTS[i]);
        printf("Wilf Class %d\\\\\n", wilfClass++);
      }
      printf("\\\\\n");
      PATTERN_MASK = ALL_RESULTS[i].pattern_mask;
      if (ALL_RESULTS[i].isTranspose)
        printf("Transpose ");
      printPatternMask(stdout);
      // printPatternMaskInLatex(ALL_RESULTS[i].pattern_mask);
    }
    printf("\\end{document}");
  }
  return 0;
}
