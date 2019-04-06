#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <math.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <sstream>

using namespace std;

ifstream fin("/ProgrammingC/input.fasm");

enum code {
  HALT = 0,    // done
  SYSCALL = 1, // done
  ADD = 2,     // done
  ADDI = 3,    // done
  SUB = 4,     // done
  SUBI = 5,    // done
  MUL = 6,     // done
  MULI = 7,    // done
  DIV = 8,     // done
  DIVI = 9,    // done
  LC = 12,     // done
  SHL = 13,    // done
  SHLI = 14,   // done
  SHR = 15,    // done
  SHRI = 16,   // done
  AND = 17,    // done, named and_
  ANDI = 18,   // done
  OR = 19,     // done named or_
  ORI = 20,    // done
  XOR = 21,    // done named xor_
  XORI = 22,   // done
  NOT = 23,    // done named not_
  MOV = 24,    // done
  ADDD = 32,   // done
  SUBD = 33,   // done
  MULD = 34,   // done
  DIVD = 35,   // done
  ITOD = 36,   // done
  DTOI = 37,   // done
  PUSH = 38,   // done
  POP = 39,    // done
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
    {HALT, "halt", RI},       {SYSCALL, "syscall", RI}, {ADD, "add", RR},
    {ADDI, "addi", RI},       {SUB, "sub", RR},         {SUBI, "subi", RI},
    {MUL, "mul", RR},         {MULI, "muli", RI},       {DIV, "div", RR},
    {DIVI, "divi", RI},       {LC, "lc", RI},           {SHL, "shl", RR},
    {SHLI, "shli", RI},       {SHR, "shr", RR},         {SHRI, "shri", RI},
    {AND, "and", RR},         {ANDI, "andi", RI},       {OR, "or", RR},
    {ORI, "ori", RI},         {XOR, "xor", RR},         {XORI, "xori", RI},
    {NOT, "not", RI},         {MOV, "mov", RR},         {ADDD, "addd", RR},
    {SUBD, "subd", RR},       {MULD, "muld", RR},       {DIVD, "divd", RR},
    {ITOD, "itod", RR},       {DTOI, "dtoi", RR},       {PUSH, "push", RI},
    {POP, "pop", RI},         {CALL, "call", RR},       {CALLI, "calli", RM},
    {RET, "ret", RI},         {CMP, "cmp", RR},         {CMPI, "cmpi", RI},
    {CMPD, "cmpd", RR},       {JMP, "jmp", RM},         {JNE, "jne", RM},
    {JEQ, "jeq", RM},         {JLE, "jle", RM},         {JL, "jl", RM},
    {JGE, "jge", RM},         {JG, "jg", RM},           {LOAD, "load", RM},
    {STORE, "store", RM},     {LOAD2, "load2", RM},     {STORE2, "store2", RM},
    {LOADR, "loadr", RR},     {STORER, "storer", RR},   {LOADR2, "loadr2", RM},
    {STORER2, "storer2", RR},
};

class Register {
public:
  int word; // value (32bit)
  Register() {
    word = 0;
    condition = "";
  }
  string condition;
};

vector<Register> r(17); // all registers without flags
Register flags;


map<string, int> label;

int load(int suffix, int adress) {
   r[suffix].word = Memory[adress];
   return 0;
 }

int store(int suffix, int adress) {
   Memory[adress] = r[suffix].word;
   return 0;
  }

int load2(int suffix, int adress) {
  r[suffix].word = Memory[adress];
  r[suffix + 1].word = Memory[adress + 1];
  return 0;
}

int store2(int suffix, int adress) {
  Memory[adress] = r[suffix].word;
  Memory[adress + 1] = r[suffix + 1].word;
  return 0;
}

int loadr(int suffix1, int suffix2, int delta) {
  r[suffix1].word = Memory[r[suffix2].word + delta];
  return 0;
}

int storer(int suffix1, int suffix2, int delta) {
  Memory[r[suffix2].word + delta] = r[suffix1].word;
  return 0;
}

int loadr2(int suffix1, int suffix2, int delta) {
  r[suffix1].word = Memory[r[suffix2].word + delta];
  r[suffix1 + 1].word = Memory[r[suffix2].word + delta + 1];
  return 0;
}

int storer2(int suffix1, int suffix2, int delta) {
  Memory[r[suffix2].word + delta] = r[suffix1].word;
  Memory[r[suffix2].word + delta + 1] = r[suffix1 + 1].word;
  return 0;
}

int add(int suffix1, int suffix2, int delta) {
  r[suffix1].word += r[suffix2].word + delta;
  return 0;
}

int addi(int suffix, int delta) {
   r[suffix].word += delta;
   return 0;
 }

int sub(int suffix1, int suffix2, int delta) {
  r[suffix1].word -= r[suffix2].word + delta;
  return 0;
}

int subi(int suffix, int delta) {
  r[suffix].word -= delta;
  return 0;
 }

int and_(int suffix1, int suffix2, int delta) {
  r[suffix1].word &= r[suffix2].word + delta;
  return 0;
}

int andi(int suffix, int imm) {
 r[suffix].word &= imm;
return 0;
}

int or_(int suffix1, int suffix2, int delta) {
  r[suffix1].word |= r[suffix2].word + delta;
  return 0;
}

int ori(int suffix, int imm) {
  r[suffix].word |= imm;
return 0;
}

int xor_(int suffix1, int suffix2, int delta) {
  r[suffix1].word ^= r[suffix2].word + delta;
  return 0;
}

int xori(int suffix, int imm) {
 r[suffix].word ^= imm;
 return 0;
}

int not_(int suffix, int imm) {
  r[suffix].word ^= 0xFFFFFFFF;
  imm = 0;
  return 0;
}

int mov(int suffix1, int suffix2, int delta) {
  r[suffix1].word = r[suffix2].word + delta;
  return 0;
}

int lc(int suffix, int imm) {
   r[suffix].word = imm;
return 0;
 }

int mul(int suffix1, int suffix2, int delta) {
  long long pro = r[suffix1].word * (r[suffix2].word + delta);
  r[suffix1].word = pro % (long long)pow(2, 32);
  r[suffix1 + 1].word = pro >> 32;
  return 0;
}

int muli(int suffix, int imm) {
  long long pro = r[suffix].word * imm;
  r[suffix].word = pro % (long long)pow(2, 32);
  r[suffix + 1].word = pro >> 32;
  return 0;
}

int div(int suffix1, int suffix2, int delta) {
  long long dividend =
      r[suffix1 + 1].word * (long long)pow(2, 32) + r[suffix1].word;
  r[suffix1].word = dividend / (r[suffix2].word + delta);
  r[suffix1 + 1].word = dividend % (r[suffix2].word + delta);
  return 0;
}

int divi(int suffix, int imm) {
  long long dividend =
      r[suffix + 1].word * (long long)pow(2, 32) + r[suffix].word;
  r[suffix].word = dividend / imm;
  r[suffix + 1].word = dividend % imm;
  return 0;
}

int shl(int suffix1, int suffix2, int delta) {
  r[suffix1].word <<= r[suffix2].word + delta;
  return 0;
}

int shli(int suffix, int imm) {
   r[suffix].word <<= imm;
return 0;
 }

int shr(int suffix1, int suffix2, int delta) {
  r[suffix1].word >>= r[suffix2].word + delta;
  return 0;
}

int shri(int suffix, int imm) {
  r[suffix].word >>= imm;
return 0;
}

int addd(int suffix1, int suffix2, int delta) {
  long long d1 = (long long)r[suffix1 + 1].word * (long long)pow(2, 32) +
                 (long long)r[suffix1].word;
  long long d2 = (long long)r[suffix2 + 1].word * (long long)pow(2, 32) +
                 (long long)r[suffix2].word;
  double sum = *(double *)&d1 + *(double *)&d2; // read as double
  long long ans = *(long long *)&sum;
  r[suffix1].word = ans % (long long)pow(2, 32);
  r[suffix1 + 1].word = ans >> 32;
  return 0;
}

int subd(int suffix1, int suffix2, int delta) {
  long long d1 = (long long)r[suffix1 + 1].word * (long long)pow(2, 32) +
                 (long long)r[suffix1].word;
  long long d2 = (long long)r[suffix2 + 1].word * (long long)pow(2, 32) +
                 (long long)r[suffix2].word;
  double sub = *(double *)&d1 - *(double *)&d2;
  long long ans = *(long long *)&sub;
  r[suffix1].word = ans % (long long)pow(2, 32);
  r[suffix1 + 1].word = ans >> 32;
  return 0;
}

int muld(int suffix1, int suffix2, int delta) {
  long long d1 = (long long)r[suffix1 + 1].word * (long long)pow(2, 32) +
                 (long long)r[suffix1].word;
  long long d2 = (long long)r[suffix2 + 1].word * (long long)pow(2, 32) +
                 (long long)r[suffix2].word;
  double pro = *(double *)&d1 * (*(double *)&d2);
  long long ans = *(long long *)&pro;
  r[suffix1].word = ans % (long long)pow(2, 32);
  r[suffix1 + 1].word = ans >> 32;
  return 0;
}

int divd(int suffix1, int suffix2, int delta) {
  long long d1 = (long long)r[suffix1 + 1].word * (long long)pow(2, 32) +
                 (long long)r[suffix1].word;
  long long d2 = (long long)r[suffix2 + 1].word * (long long)pow(2, 32) +
                 (long long)r[suffix2].word;
  double div = *(double *)&d1 / (*(double *)&d2);
  long long ans = *(long long *)&div;
  r[suffix1].word = ans % (long long)pow(2, 32);
  r[suffix1 + 1].word = ans >> 32;
  return 0;
}

int itod(int suffix1, int suffix2, int delta) {
  double d = *(double *)&r[suffix2].word + *(double *)&delta;
  long long ans = *(long long *)&d;
  r[suffix1].word = ans % (long long)pow(2, 32);
  r[suffix1 + 1].word = ans >> 32;
  return 0;
}

int dtoi(int suffix1, int suffix2, int delta) {
  long long ans = (long long)r[suffix2 + 1].word * (long long)pow(2, 32) +
                  (long long)r[suffix2].word + *(long long *)&delta;
  double d = *(double *)&ans;    // read as double
  r[suffix1].word = (long int)d; // ignore frac
  return 0;
}

int push (int suffix, int imm) {
  r[14].word--;
  Memory[r[14].word] = r[suffix].word + imm;
//  cout << "we pushed "<< Memory[r[14].word] << endl;
return 0;
}

int pop (int suffix, int imm) {
  r[suffix].word = Memory[r[14].word] + imm;
  r[14].word++;
  return 0;
}

int cmp(int suffix1, int suffix2, int delta) {
  if (r[suffix1].word > r[suffix2].word + delta) {
    flags.condition = ">";
  }
  if (r[suffix1].word < r[suffix2].word + delta) {
    flags.condition = "<";
  }
  if (r[suffix1].word == r[suffix2].word + delta) {
    flags.condition = "==";
  }
  return 0;
}

int cmpi(int suffix, int imm) {
  if (r[suffix].word > imm) {
    flags.condition = ">";
  }
  if (r[suffix].word < imm) {
    flags.condition = "<";
  }
  if (r[suffix].word == imm) {
    flags.condition = "==";
  }
  return 0;
}

int cmpd(int suffix1, int suffix2, int delta) {
  long long l1 = (long long)r[suffix1 + 1].word * (long long)pow(2, 32) +
                 (long long)r[suffix1].word;
  long long l2 = (long long)r[suffix2 + 1].word * (long long)pow(2, 32) +
                 (long long)r[suffix2].word;
  double d1 = *(double *)&l1;
  double d2 = *(double *)&l2;
  if (d1 > d2) {
    flags.condition = ">";
  }
  if (d1 < d2) {
    flags.condition = "<";
  }
  if (d1 == d2) {
    flags.condition = "==";
  }
  return 0;
}

int syscall (int suffix, int code) {
  double d;
  long long ans;
  char c;
  switch (code) {
  case 0:
    return r[suffix].word;
  case 100:
    fin >> r[suffix].word;
    return 0;
  case 101:
    fin >> d;
    ans = *(long long *)&d;
    r[suffix].word = ans % (long long)pow(2, 32);
    r[suffix + 1].word = ans >> 32;
    return 0;
  case 102:
    cout << r[suffix].word;
    return 0;
  case 103:
    ans = (long long)r[suffix + 1].word * (long long)pow(2, 32) +
          (long long)r[suffix].word;
    d = *(double *)&ans;
    printf("%lg", d);
    return 0;
  case 104:
    fin >> c;
    r[suffix].word = *(long int *)&c;
    return 0;
  case 105:
    c = *(char *)&r[suffix].word;
    cout << c;
    return 0;
  }
  return 0;
}

int halt(int suffix, int delta) {return 0;}

int calli (string l) {
  push (15, 1);
  //cout << "calli->index " << r[15].word << endl;
  r[15].word = label[l];
  return 0;
}

int ret (string s) {
  r[15].word = Memory[r[14].word];
//  cout << "r15 from ret is " << r[15].word << endl;
  int imm = stoi(s);
  r[14].word += imm + 1;
  return 0;
}

int call (int suffix1, int suffix2, int imm) {
  //push (15, 1);
  r[suffix1].word = r[15].word = r[suffix2].word;
  return 0;
}

int jmp (string l) {
  r[15].word = label[l];
  return 0;
}

int jne (string l) {
  if (flags.condition != "==") {
    r[15].word = label[l];
  }
  else {r[15].word++;}
  return 0;
}

int jeq (string l) {
  if (flags.condition == "==") {
    r[15].word = label[l];
  }
  else {r[15].word++;}
  return 0;
}

int jle (string l) {
  if (flags.condition == "==" || flags.condition == "<") {
    r[15].word = label[l];
  }
  else {r[15].word++;}
  return 0;
}

int jl (string l) {
  if (flags.condition == "<") {
    r[15].word = label[l];
  }
  else {r[15].word++;}
  return 0;
}

int jge (string l) {
  if (flags.condition == "==" || flags.condition == ">") {
    r[15].word = label[l];
  }
  else {r[15].word++;}
  return 0;
}

int jg (string l) {
  if (flags.condition == ">") {
    r[15].word = label[l];
  }
  else {r[15].word++;}
  return 0;
}

std::set <int> special;
std::map <int, string> execute;


int code_to_memory() {
  int index = 0; // index to Memory
  while (!fin.eof()) {
    string s;
    fin >> s;

    if (s[s.length() - 1] == ':') {
    label[s] = index;
    //  cout << "inserted " << s <<"->" << label[s] << endl;
      continue;
    }

    if (s == "end") {
      string start;
      fin >> start;
      r[15].word = label[start + ":"];
    //  cout << "we start from " << label[start + ":"] << endl;
      execute[index] = "end";
    //  cout << execute[index] << endl;
      return 0;
    }

    for (int i = 0; i < 52; i++) {

      if (cmds_info[i].n == s) {
        if (cmds_info[i].m == RM || cmds_info[i].m == RI) {


          if ( special.find(cmds_info[i].c) != special.end() ) {
            string l;
            fin >> l;
            int token_code = cmds_info[i].c;
            int token = (token_code << 24) + label[l];
            execute[index] = s + " " + l;
          //  cout << index << "-> " << execute[index] << endl;
            Memory[index++] = token;
            break;
          }

          string reg, delta;
          char r;
          fin >> r >> reg >> delta;

          int reg_int = stoi(reg);
          int delta_int = stoi(delta);
          int token_code = cmds_info[i].c;

          int token = (token_code << 24) + (reg_int << 20) + delta_int;
          execute[index] = s + " r" + reg + " " + delta;
        //  cout << index << "-> " << execute[index] << endl;

          Memory[index++] = token;
        }

        if (cmds_info[i].m == RR) {
          string reg1, reg2, delta;
          char r1, r2;
          fin >> r1 >> reg1 >> r2 >> reg2 >> delta;

          int reg1_int = stoi(reg1);
          int reg2_int = stoi(reg2);
          int delta_int = stoi(delta);
          int token_code = cmds_info[i].c;

          int token = (token_code << 24) + (reg1_int << 20) + (reg2_int << 16) +
                      delta_int;
          execute[index] = s + " r" + reg1 + " r" + reg2 + " " + delta;
        //  cout << index << "-> "<< execute[index] << endl;

          Memory[index++] = token;
        }
      }
    }
  }
  return 0;
}


int main() {


  vector<int (*)(int, int)> cmds_RM_RI(72); // pointers to functions
  cmds_RM_RI[HALT] = halt;
  cmds_RM_RI[SYSCALL] = syscall;
  cmds_RM_RI[ADDI] = addi;
  cmds_RM_RI[SUBI] = subi;
  cmds_RM_RI[MULI] = muli;
  cmds_RM_RI[DIVI] = divi;
  cmds_RM_RI[LC] = lc;
  cmds_RM_RI[SHLI] = shli;
  cmds_RM_RI[SHRI] = shri;
  cmds_RM_RI[ANDI] = andi;
  cmds_RM_RI[ORI] = ori;
  cmds_RM_RI[XORI] = xori;
  cmds_RM_RI[NOT] = not_;
  cmds_RM_RI[PUSH] = push;
  cmds_RM_RI[POP] = pop;
  cmds_RM_RI[CMPI] = cmpi;
  cmds_RM_RI[LOAD] = load;
  cmds_RM_RI[STORE] = store;
  cmds_RM_RI[LOAD2] = load2;
  cmds_RM_RI[STORE2] = store2;

  vector<int (*)(int, int, int)> cmds_RR(72); // pointers to functions
  cmds_RR[ADD] = add;
  cmds_RR[SUB] = sub;
  cmds_RR[MUL] = mul;
  cmds_RR[DIV] = div;
  cmds_RR[SHL] = shl;
  cmds_RR[SHR] = shr;
  cmds_RR[AND] = and_;
  cmds_RR[OR] = or_;
  cmds_RR[XOR] = xor_;
  cmds_RR[MOV] = mov;
  cmds_RR[ADDD] = addd;
  cmds_RR[SUBD] = subd;
  cmds_RR[MULD] = muld;
  cmds_RR[DIVD] = divd;
  cmds_RR[ITOD] = itod;
  cmds_RR[DTOI] = dtoi;
  cmds_RR[CMP] = cmp;
  cmds_RR[CMPD] = cmpd;
  cmds_RR[LOADR] = loadr;
  cmds_RR[STORER] = storer;
  cmds_RR[LOADR2] = loadr2;
  cmds_RR[STORER2] = storer2;
  cmds_RR[CALL] = call;


  special.insert(CALLI);
  special.insert(RET);
  special.insert(JMP);
  special.insert(JNE);
  special.insert(JEQ);
  special.insert(JLE);
  special.insert(JL);
  special.insert(JGE);
  special.insert(JG);





  vector<int (*)(string)> spec_func(72); // pointers to functions
  spec_func[CALLI] = calli;
  spec_func[RET] = ret;
  spec_func[JMP] = jmp;
  spec_func[JNE] = jne;
  spec_func[JEQ] = jeq;
  spec_func[JLE] = jle;
  spec_func[JL] = jl;
  spec_func[JGE] = jge;
  spec_func[JG] = jg;



  code_to_memory();

  r[14].word = 1048575;

  string s;

 while (s != "end") {
  stringstream x;
  x << execute[r[15].word];
//  cout << "current -> " << execute[r[15].word] << endl;
  x >> s;
    for (int i = 0; i < 52; i++) {
      if (cmds_info[i].n == s) {

        if (cmds_info[i].m == RM || cmds_info[i].m == RI) {

          if ( special.find(cmds_info[i].c) != special.end() ) {
            string l;
            x >> l;
            spec_func[cmds_info[i].c](l + ":");
            continue;
          }

          string reg, delta;
          char r_char;
          x >> r_char >> reg >> delta;

          int reg_int = stoi(reg);
          int delta_int = stoi(delta);

          cmds_RM_RI[cmds_info[i].c](reg_int, delta_int); //falls here
          r[15].word++;
        }

        if (cmds_info[i].m == RR) {
          string reg1, reg2, delta;
          char r1, r2;
          x >> r1 >> reg1 >> r2 >> reg2 >> delta;

          int reg1_int = stoi(reg1);
          int reg2_int = stoi(reg2);
          int delta_int = stoi(delta);

          cmds_RR[cmds_info[i].c](reg1_int, reg2_int, delta_int);
          r[15].word++;
        }
      }
    }
}

  fin.close();
  return 0;
}
