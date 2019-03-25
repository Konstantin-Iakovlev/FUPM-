#include <iostream>
#include <map>
#include <math.h>
#include <string>
#include <vector>
#include <stdio.h>
#include "task.cpp"

using namespace std;

enum code {
  HALT = 0,
  SYSCALL = 1,
  ADD = 2,   // done
  ADDI = 3,  // done
  SUB = 4,   // done
  SUBI = 5,  // done
  MUL = 6,   // done
  MULI = 7,  // done
  DIV = 8,   // done
  DIVI = 9,  // done
  LC = 12,   // done
  SHL = 13,  // done
  SHLI = 14, // done
  SHR = 15,  // done
  SHRI = 16, // done
  AND = 17,  // done, named and_
  ANDI = 18, // done
  OR = 19,   // done named or_
  ORI = 20,  // done
  XOR = 21,  // done named xor_
  XORI = 22, // done
  NOT = 23,  // done named not_
  MOV = 24,  // done
  ADDD = 32, // done
  SUBD = 33, // done
  MULD = 34, // done
  DIVD = 35, // done
  ITOD = 36, // done
  DTOI = 37, // done
  PUSH = 38, // done
  POP = 39,  // done
  CALL = 40,
  CALLI = 41,
  RET = 42,
  CMP = 43,  // done
  CMPI = 44, // done
  CMPD = 45, // done
  JMP = 46,
  JNE = 47,
  JEQ = 48,
  JLE = 49,
  JL = 50,
  JGE = 51,
  JG = 52,
  LOAD = 64,   // done
  STORE = 65,  // done
  LOAD2 = 66,  // done
  STORE2 = 67, // done
  LOADR = 68,  // done
  LOADR2 = 69, // done
  STORER = 70, // done
  STORER2 = 71 // done
};

enum mode { RM, RR, RI };

vector<int> Memory(1048576, 0);

struct cmds_info {
  enum code c;
  string n;
  enum mode m;
} cmds_info[] = {
  {HALT, "halt", RI},
  {SYSCALL, "syscall", RI},
  {ADD, "add", RR},
  {ADDI, "addi", RI},
  {SUB, "sub", RR},
  {SUBI, "subi", RI},
  {MUL, "mul", RR},
  {MULI, "muli", RI},
  {DIV, "div", RR},
  {DIVI, "divi", RI},
  {LC, "lc", RI},
  {SHL, "shl", RR},
  {SHLI, "shli", RI},
  {SHR, "shr", RR},
  {SHRI, "shri", RI},
  {AND, "and", RR},
  {ANDI, "andi", RI},
  {OR, "or", RR},
  {ORI, "ori", RI},
  {XOR, "xor", RR},
  {XORI, "xori", RI},
  {NOT, "not", RI},
  {MOV, "mov", RR},
  {ADDD, "addd", RR},
  {SUBD, "subd", RR},
  {MULD, "muld", RR},
  {DIVD, "divd", RR},
  {ITOD, "itod", RR},
  {DTOI, "dtoi", RR},
  {PUSH, "push", RI},
  {POP, "pop", RI},
  {CALL, "call", RR},
  {CALLI, "calli", RM},
  {RET, "ret", RI},
  {CMP, "cmp", RR},
  {CMPI, "cmpi", RI},
  {CMPD, "cmpd", RR},
  {JMP, "jmp", RM},
  {JNE, "jne", RM},
  {JEQ, "jeq", RM},
  {JLE, "jle", RM},
  {JL, "jl", RM},
  {JGE, "jge", RM},
  {JG, "jg", RM},
  {LOAD, "load", RM},
  {STORE, "store", RM},
  {LOAD2, "load2", RM},
  {STORE2, "store2", RM},
  {LOADR, "loadr", RR},
  {STORER, "storer", RR},
  {LOADR2, "loadr2", RM},
  {STORER2, "storer2", RR},
    };

class Register {
public:
  long int word; // value (32bit)
  Register() {
    word = 0;
    condition = "";
  }
  string condition;
};

vector<Register> r(17); // all registers without flags
Register flags;

void load(int suffix, unsigned long long adress) {
  r[suffix].word = Memory[adress];
}

void store(int suffix, unsigned long long adress) {
  Memory[adress] = r[suffix].word;
}

void load2(int suffix, unsigned long long adress) {
  r[suffix].word = Memory[adress];
  r[suffix + 1].word = Memory[adress + 1];
}

void store2(int suffix, unsigned long long adress) {
  Memory[adress] = r[suffix].word;
  Memory[adress + 1] = r[suffix + 1].word;
}

void loadr(int suffix1, int suffix2, long int delta) {
  r[suffix1].word = Memory[r[suffix2].word + delta];
}

void storer(int suffix1, int suffix2, long int delta) {
  Memory[r[suffix2].word + delta] = r[suffix1].word;
}

void loadr2(int suffix1, int suffix2, long int delta) {
  r[suffix1].word = Memory[r[suffix2].word + delta];
  r[suffix1 + 1].word = Memory[r[suffix2].word + delta + 1];
}

void storer2(int suffix1, int suffix2, long int delta) {
  Memory[r[suffix2].word + delta] = r[suffix1].word;
  Memory[r[suffix2].word + delta + 1] = r[suffix1 + 1].word;
}

void add(int suffix1, int suffix2, long int delta) {
  r[suffix1].word += r[suffix2].word + delta;
}

void addi(int suffix, long int delta) { r[suffix].word += delta; }

void sub(int suffix1, int suffix2, long int delta) {
  r[suffix1].word -= r[suffix2].word + delta;
}

void subi(int suffix, long int delta) { r[suffix].word -= delta; }

void and_(int suffix1, int suffix2, long int delta) {
  r[suffix1].word &= r[suffix2].word + delta;
}

void andi(int suffix, long int imm) { r[suffix].word &= imm; }

void or_(int suffix1, int suffix2, long int delta) {
  r[suffix1].word |= r[suffix2].word + delta;
}

void ori(int suffix, long int imm) { r[suffix].word |= imm; }

void xor_(int suffix1, int suffix2, long int delta) {
  r[suffix1].word ^= r[suffix2].word + delta;
}

void xori(int suffix, long int imm) { r[suffix].word ^= imm; }

void not_(int suffix, long int imm) {
  r[suffix].word ^= 0xFFFFFFFF;
  imm = 0;
}

void mov(int suffix1, int suffix2, long int delta) {
  r[suffix1].word = r[suffix2].word + delta;
}

void lc(int suffix, long int imm) { r[suffix].word = imm; }

void mul(int suffix1, int suffix2, long int delta) {
  long long pro = r[suffix1].word * (r[suffix2].word + delta);
  r[suffix1].word = pro % (long long)pow(2, 32);
  r[suffix1 + 1].word = pro >> 32;
}

void muli(int suffix, long int imm) {
  long long pro = r[suffix].word * imm;
  r[suffix].word = pro % (long long)pow(2, 32);
  r[suffix + 1].word = pro >> 32;
}

void div(int suffix1, int suffix2, long int delta) {
  long long dividend =
      r[suffix1 + 1].word * (long long)pow(2, 32) + r[suffix1].word;
  r[suffix1].word = dividend / (r[suffix2].word + delta);
  r[suffix1 + 1].word = dividend % (r[suffix2].word + delta);
}

void divi(int suffix, long int imm) {
  long long dividend =
      r[suffix + 1].word * (long long)pow(2, 32) + r[suffix].word;
  r[suffix].word = dividend / imm;
  r[suffix + 1].word = dividend % imm;
}

void shl(int suffix1, int suffix2, long int delta) {
  r[suffix1].word <<= r[suffix2].word + delta;
}

void shli(int suffix, long int imm) { r[suffix].word <<= imm; }

void shr(int suffix1, int suffix2, long int delta) {
  r[suffix1].word >>= r[suffix2].word + delta;
}

void shri(int suffix, long int imm) { r[suffix].word >>= imm; }

void addd(int suffix1, int suffix2, double delta) {
  long long d1 = (long long)r[suffix1 + 1].word * (long long)pow(2, 32) +
                 (long long)r[suffix1].word;
  long long d2 = (long long)r[suffix2 + 1].word * (long long)pow(2, 32) +
                 (long long)r[suffix2].word;
  double sum = *(double *)&d1 + *(double *)&d2 + delta; // read as double
  long long ans = *(long long *)&sum;
  r[suffix1].word = ans % (long long)pow(2, 32);
  r[suffix1 + 1].word = ans >> 32;
}

void subd(int suffix1, int suffix2, double delta) {
  long long d1 = (long long)r[suffix1 + 1].word * (long long)pow(2, 32) +
                 (long long)r[suffix1].word;
  long long d2 = (long long)r[suffix2 + 1].word * (long long)pow(2, 32) +
                 (long long)r[suffix2].word;
  double sub = *(double *)&d1 - *(double *)&d2 - delta;
  long long ans = *(long long *)&sub;
  r[suffix1].word = ans % (long long)pow(2, 32);
  r[suffix1 + 1].word = ans >> 32;
}

void muld(int suffix1, int suffix2, double delta) {
  long long d1 = (long long)r[suffix1 + 1].word * (long long)pow(2, 32) +
                 (long long)r[suffix1].word;
  long long d2 = (long long)r[suffix2 + 1].word * (long long)pow(2, 32) +
                 (long long)r[suffix2].word;
  double pro = *(double *)&d1 * (*(double *)&d2 + delta);
  long long ans = *(long long *)&pro;
  r[suffix1].word = ans % (long long)pow(2, 32);
  r[suffix1 + 1].word = ans >> 32;
}

void divd(int suffix1, int suffix2, double delta) {
  long long d1 = (long long)r[suffix1 + 1].word * (long long)pow(2, 32) +
                 (long long)r[suffix1].word;
  long long d2 = (long long)r[suffix2 + 1].word * (long long)pow(2, 32) +
                 (long long)r[suffix2].word;
  double div = *(double *)&d1 / (*(double *)&d2 + delta);
  long long ans = *(long long *)&div;
  r[suffix1].word = ans % (long long)pow(2, 32);
  r[suffix1 + 1].word = ans >> 32;
}

void itod(int suffix1, int suffix2, long int delta) {
  double d = *(double *)&r[suffix2].word + *(double *)&delta;
  long long ans = *(long long *)&d;
  r[suffix1].word = ans % (long long)pow(2, 32);
  r[suffix1 + 1].word = ans >> 32;
}

void dtoi(int suffix1, int suffix2, double delta) {
  long long ans = (long long)r[suffix2 + 1].word * (long long)pow(2, 32) +
                  (long long)r[suffix2].word + *(long long *)&delta;
  double d = *(double *)&ans;    // read as double
  r[suffix1].word = (long int)d; // ignore frac
}

void push(int suffix, long int imm) {
  r[14].word--;
  Memory[r[14].word] = r[suffix].word + imm;
}

void pop(int suffix, long int imm) {
  r[suffix].word = Memory[r[14].word] + imm;
  r[14].word++;
}

void cmp(int suffix1, int suffix2, long int delta) {
  if (r[suffix1].word > r[suffix2].word + delta) {
    flags.condition = ">";
  }
  if (r[suffix1].word < r[suffix2].word + delta) {
    flags.condition = "<";
  }
  if (r[suffix1].word == r[suffix2].word + delta) {
    flags.condition = "==";
  }
}

void cmpi(int suffix, long int imm) {
  if (r[suffix].word > imm) {
    flags.condition = ">";
  }
  if (r[suffix].word < imm) {
    flags.condition = "<";
  }
  if (r[suffix].word == imm) {
    flags.condition = "==";
  }
}

void cmpd(int suffix1, int suffix2, double delta) {
  long long l1 = (long long)r[suffix1 + 1].word * (long long)pow(2, 32) +
                 (long long)r[suffix1].word;
  long long l2 = (long long)r[suffix2 + 1].word * (long long)pow(2, 32) +
                 (long long)r[suffix2].word;
  double d1 = *(double *)&l1;
  double d2 = *(double *)&l2 + delta;
  if (d1 > d2) {
    flags.condition = ">";
  }
  if (d1 < d2) {
    flags.condition = "<";
  }
  if (d1 == d2) {
    flags.condition = "==";
  }
}

void syscall(int suffix, int code) {
  double d;
  long long ans;
  char c;
  switch (code) {
  case 100:
    cin >> r[suffix].word;
    break;
  case 101:
    cin >> d;
    ans = *(long long *)&d;
    r[suffix].word = ans % (long long)pow(2, 32);
    r[suffix + 1].word = ans >> 32;
    break;
  case 102:
    cout << r[suffix].word << "\n";
    break;
  case 103:
    ans = (long long)r[suffix + 1].word * (long long)pow(2, 32) +
          (long long)r[suffix].word;
    d = *(double *)&ans;
    cout << d << "\n";
    break;
  case 104:
    cin >> c;
    r[suffix].word = *(long int *)&c;
    break;
  case 105:
    c = *(char *)&r[suffix].word;
    cout << c << "\n";
    break;
  }
}

int main() {
  unsigned long int index = 0; //index to Memory
  string s;
  cin >> s;
for(int i = 0; i < 52; i++) {
  if (cmds_info[i].n == s) {

    if (cmds_info[i].m == RM) {  //works well!!!
      string reg, delta;
      char r;
      cin >> r >> reg >> delta;
      cout << cmds_info[i].n << " " << reg << " " << delta << endl;

      int reg_int = stoi(reg);
      int delta_int = stoi(delta);
      int token_code = cmds_info[i].c;
      cout <<"token_code"<< token_code << endl;
      int token = (token_code << 24) + ((int)reg_int << 20) + (int)delta_int;
      Memory[index] = token;
      show_bytes(sizeof(int), &token);
    }

    if(cmds_info[i].m == RR) {
      string reg1, reg2, delta;
      char r1, r2;
      cin >> r1 >> reg1 >> r2 >> reg2 >> delta;
      unsigned long long delta_ll = stoll(delta);
      cout << reg1 << " " << reg2 << " " << delta_ll << endl;
    }

  }
}

  return 0;
}
