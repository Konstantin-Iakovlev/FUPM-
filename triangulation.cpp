#include <math.h>
#include <stdio.h>
#include <iostream>

using namespace std;

struct Point {
  int x, y;
};

double min(double x, double y) { return (x <= y) ? x : y; }

double dist(Point p1, Point p2) {
  return sqrt((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y));
}

double cost(Point points[], int i, int j, int k) {
  Point p1 = points[i], p2 = points[j], p3 = points[k];
  return dist(p1, p2) + dist(p2, p3) + dist(p3, p1);
}



double GetMinCost(Point points[], int i, int j, int n, double **table) {
    //cout << i << " " << j << " " << endl;
  if (table[i][j] != -1)
    return table[i][j];

    if ( (i + 1) % n == j ) { //двуугольник (если такой в природе существует)
        table[i][j] = 0;
        return table[i][j]; //меморизация
    }

  double ans = 100000.0;
  int k = (i + 1) % n;
  while (k <= (j - 1 + n) % n) { //i-j -- сторона некоторого треугольника. Ищем оптимальный
    ans = min(cost(points, i, j, k) + GetMinCost(points, i, k, n, table) +
                  GetMinCost(points, k, j, n, table),
              ans);
    k++;
  }
  table[i][j] = ans;
  return ans;
}

int main() {
  Point points[] = {{0, 0}, {1, 0}, {2, 1}, {1, 2}, {0, 2}};
  int n = sizeof(points) / sizeof(points[0]);
  double **table = (double**)malloc(sizeof(double) * n); //храним ответы на подзадачи
  for (int i = 0; i < n; i++) {
    table[i] = (double*)malloc(sizeof(double) * n);
    for (int j = 0; j < n; j++) {
      table[i][j] = -1;
    }
  }

  cout << GetMinCost(points, 0, n - 1, n, table) << endl;
  return 0;
}
