//USAGE:
// cat avoidence.txt | grep 329462 | head -n1 | rev |cut -d' ' -f2- | rev | ./a.out > bork.tex; pdflatex bork.tex; open bork.pdf

#include <iostream>
#include <cstring>

const int MAX = 5;

void printHeader() {
  printf("\\documentclass[]{article}\n");
  printf("\\usepackage{longtable}\n");
  printf("\\usepackage{amsmath}\n");
  printf("\\begin{document}\n");
  printf("\\title{Poly-Bernoulli Numbers}\n");
  printf("\\maketitle\n");
}

int main() {
  char str[5000];
  unsigned long long int counts[MAX][MAX];
  int i, j;
  i = j = 0;
  printHeader();
  std::cin.getline(str, 5000, '\n');
  char *pch;
  pch = strtok(str, " \t\n");
  while (pch != NULL) {
    counts[i][j] = std::stoull(pch);
    j++;
    if (j == 5) {
      j = 0;
      i++;
    }
    pch = strtok(NULL, " \t\n");
  }
  printf("$$\\left[ \\begin{array}{");
  for (i = 0; i < 5; i++)
    printf("c");
  printf("}\n");
  for (i = 0; i < 5; i++) {
    for (j = 0; j < 5; j++) {
      printf("%llu ", counts[i][j]);
      if ((j + 1) < 5)
        printf(" & ");
    }
    if ((i + 1) < 5)
      printf(" \\\\ \n");
  }
  printf(" \\end{array} \\right] $$\n");
  printf("\\end{document}");
  return 0;
}
