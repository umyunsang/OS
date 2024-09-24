#include <stdio.h>

main()
{
  FILE *fp, *fpOut;
  int i, j, k, min, max, n, sum=0;

  fp = fopen("sum.inp", "r");
  fpOut = fopen("sum.out", "w");

  fscanf(fp, "%d", &n);

  for(i=0; i<n; i++) {
    fscanf(fp, "%d %d", &j, &k);

    // 이 부분에 여러분 각자가 문제에서 요구하는 sum 을 구하는 프로그램을 짜야 함

    fprintf(fpOut, "%d\n", sum);
  }

  fclose(fp);
  fclose(fpOut);
  return 0;
}
