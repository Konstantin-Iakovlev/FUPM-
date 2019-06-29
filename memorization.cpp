#include <iostream>
#include <math.h>
#include <vector>

using namespace std;

vector<vector<int> > maxim;     //Самый тяжелый путь в [i][j]
vector<vector<vector<int> > > d; //кол-во путей, отстающих от
//максимального в [i][j] на t-> d[i][j][t]
vector<vector<int> > matrix; //наше поле с весами



int MaxWay(int i, int j) { //самый тяжелый путь в [i][j]
  if (i == 0 && j == 0) {
    return matrix[0][0]; 
  }
  if (i == 0) {
      if (maxim[i][j] == 0) { // если не вычисляли
          maxim[i][j] = MaxWay(i, j - 1) + matrix[i][j];
      }
    return maxim[i][j];
  }
  if (j == 0) {
      if (maxim[i][j] == 0) { //если не вычисляли
          maxim[i][j] = MaxWay(i - 1, j) + matrix[i][j];
      }
    return maxim[i][j];
  }
  if (maxim[i][j] == 0) { //если не вычисляли, но клетка внутри(есть верхний и левый сосед)
      maxim[i][j] = max(MaxWay(i, j - 1), MaxWay(i - 1, j)) + matrix[i][j];
  }
  return maxim[i][j];
}



int NumberOfWays(int i, int j, int l) { //количество путей в [i][j] с отставанием l от maxim[i][j]
    if (i == 0 && j == 0 && l == 0) {
        d[0][0][0] = 1;
        return 1;
    }
    if (i == 0) { //если клетка на первой строке
        if (l == 0){return 1;} //об этом писал в доказательстве
        else{return 0;}
    }
        int aux_num = 0; //вспомогательный ответ. В конце функции его использование
        int x1 = MaxWay(i - 1, j) + matrix[i][j] - MaxWay(i, j) + l; //тот же самый x1, что в 1 решении
        if (x1 >= 0 && d[i][j][l] == -1) { //если не вычисляли
            aux_num += NumberOfWays(i - 1, j, x1);
        }
    
    if (j == 0) { //аналогично, как выше
        if (l == 0){return 1;}
        else{return 0;}
    }

        int x2 = MaxWay(i, j - 1) + matrix[i][j] - MaxWay(i, j) + l;
        if (x2 >= 0 && d[i][j][l] == -1) {
            aux_num += NumberOfWays(i, j - 1, x2);
        }
    
    
    d[i][j][l] = aux_num; //меморизация
    return aux_num;
}




int main() {
  int m, n, k;
  cin >> m >> n >> k;
  maxim.resize(m);
  d.resize(m);
  matrix.resize(m);
  for (int i = 0; i < m; i++) {
    maxim[i].resize(n);
    matrix[i].resize(n);
    d[i].resize(n);
    for (int j = 0; j < n; j++) {
      cin >> matrix[i][j];
      d[i][j].resize(k + 1); //храним кол-во путей остающих на k
      maxim[i][j] = 0;
      for (int l = 0; l < k + 1; l++) {
        d[i][j][l] = -1; //теперь -1, а не 0
      }
    }
  }

int ans = 0;
cout << "Max way ->"<< MaxWay(m - 1, n - 1) << endl;
for (int i = 0; i <= k; i++) {
    ans += NumberOfWays(m - 1, n - 1, i); //собираем ответ
}
cout << "ans ->" << ans << endl;
  return 0;
}
