#include <iostream>

using namespace std;

void show_bytes(size_t const size, void const *const ptr) {
  unsigned char *b = (unsigned char *)ptr;
  unsigned char byte;
  int i, j;

  for (i = size - 1; i >= 0; i--) {
    for (j = 7; j >= 0; j--) {
      byte = (b[i] >> j) & 1;
      printf("%u", byte);
      if (j == 4 || j == 0) {
        cout << "'";
      }
    }
  }
  cout << endl;
}


int main() {
  double a = 84874.123;
  show_bytes(sizeof(double), &a);
}
