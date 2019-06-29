#include <iostream>
#include <vector>
#include <math.h>

using namespace std;

int main() {
    int m, n, k;
    cin >> m >> n >> k;
    vector<vector<int> > maxim(m); //Самый тяжелый путь в [i][j]
    vector<vector< vector<int> > > d(m); //кол-во путей, отстающих от
    //максимального в [i][j] на t-> d[i][j][t]
    vector<vector<int> > matrix(m); //наше поле с весами
    for(int i = 0; i < m; i++) {
        maxim[i].resize(n);
        matrix[i].resize(n);
        d[i].resize(n);
        for (int j = 0; j < n; j++) {
            cin >> matrix[i][j];
            d[i][j].resize(402); //храним не только остающие на k, но и на 2k
            //есть простой контрпример, если рассмотреть очередную клетку с весом > k
            maxim[i][j] = 0;
            for (int l = 0; l < 402; l++) {
                d[i][j][l] = 0;
            }
        }
    }
    maxim[0][0] = matrix[0][0];
    d[0][0][0] = 1;
    for (int i = 1; i < m; i++) {
        maxim[i][0] = matrix[i][0] + maxim[i - 1][0];
        d[i][0][0] = 1;
    }

    for (int i = 1; i < n; i++) {
        maxim[0][i] = matrix[0][i] + maxim[0][i - 1];
        d[0][i][0] = 1;
    }


    for (int i = 1; i < m; i++) {
        for (int j = 1; j < n; j++) {
            maxim[i][j] = max(maxim[i - 1][j], maxim[i][j - 1]) + matrix[i][j]; //всего 2 способа зайти в клетку
            for (int l = 0; l <= 2 * k + 1; l++) {
                //данный переход лучше осознать, записав непосредственно длину пути
                //из начала в [i - 1][j]. 
                //maxim[i][j] - l = maxim[i - 1][j] - x1 + matrix[i][j]
                //из этого уравнения достаем х1
                int x1 = maxim[i - 1][j] + matrix[i][j] - maxim[i][j] + l;
                if (x1 >= 0) {
                    d[i][j][l] += d[i - 1][j][x1];
                }
                int x2 = maxim[i][j - 1] + matrix[i][j] - maxim[i][j] + l;
                if (x2 >= 0) {
                    d[i][j][l] += d[i][j - 1][x2];
                }
            }
        }
    }
    int ans = 0;
    for (int i = 0; i <= k; i++) {
        ans += d[m - 1][n - 1][i];
    }
    cout << "maxim is "<< maxim[m - 1][n - 1] << endl;
    cout<<"ans is " << ans << endl;
    return 0;
}
