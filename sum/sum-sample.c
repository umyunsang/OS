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

    // �� �κп� ������ ���ڰ� �������� �䱸�ϴ� sum �� ���ϴ� ���α׷��� ¥�� ��

    fprintf(fpOut, "%d\n", sum);
  }

  fclose(fp);
  fclose(fpOut);
  return 0;
}
