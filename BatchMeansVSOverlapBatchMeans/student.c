/* Simulação de Eventos Discretos - Lab 5
 * Participantes:
 * Ayan Perez de Abreu
 * Luan de Freitas Uchôa
 *
 * Código fornecido pelo professor e transformado em header por conveniência.
 *
 * Instruções de compilação em lab5.c
 * */

#include <math.h>

/* Exemplo de código que gera valores da tabela da distribuição de Student.
 * Esse cálculo também utiliza valores da tabela da distribuição Normal
 *
 * Na chamada seguinte:
 * 		df é uma variável inteira representando o número de graus de liberdade (procure na literatura o que significa isso),
 * 		alpha é o nível de significância;
 */

//Valor_t = StudentDist(df, 1 - (alpha / 2));

/*
 * Retorna um ponto crítico da distribuiçao normal
 */
double normal_dist(double p) {
  double q, z1, n, d;
  q = (p > 0.5) ? (1 - p) : p;
  double logq = log(q);
  z1 = sqrt(-2.0 * logq);
  n = (0.010328 * z1 + 0.802853) * z1 + 2.515517;
  d = ((0.001308 * z1 + 0.189269) * z1 + 1.43278) * z1 + 1;
  z1 -= n / d;
  if (p > 0.5)
    z1 =- z1;
     return z1;
}

/*
 * Retorna um ponto crítico da distribuiçao de student
 */
double student_dist(int ndf, double p) {
  int i;
  double z1, z2, h[4], x=0.0;

  z1 = fabs(normal_dist(p));
  z2 = z1 * z1;
  h[0] = 0.25 * z1 * (z2 + 1.0);
  h[1] = 0.010416667 * z1 * ((5.0 * z2 + 16.0) * z2 + 3.0);
  h[2] = 0.002604167 * z1 * (((3.0 * z2 + 19.0) * z2 + 17.0) * z2 - 15.0);
  h[3] = z1*((((79.0*z2+776.0)*z2+1482.0)*z2-1920.0)*z2-945.0);
  h[3] *= 0.000010851;
  for (i = 3; i >= 0; i--)
    x = (x + h[i]) / ((double) ndf);
  z1 += x;
  //if (p > 0.5)
  // abs() não funciona para doubles
  if (z1 < 0.0)
    z1 = -z1;
  return z1;
}
