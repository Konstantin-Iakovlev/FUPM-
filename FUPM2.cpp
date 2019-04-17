#include <fstream>
#include <iostream>
#include <map>
#include <math.h>
#include <set>
#include <sstream>
#include <stdio.h>
#include <string>
#include <vector>

using namespace std;

ifstream fin("input.fasm");

enum code {
  HALT = 0,
  SYSCALL = 1,
  ADD = 2,
  ADDI = 3,
  SUB = 4,
  SUBI = 5,
  MUL = 6,
  MULI = 7,
  DIV = 8,
  DIVI = 9,
  LC = 12,
  SHL = 13,
  SHLI = 14,
  SHR = 15,
  SHRI = 16,
  AND = 17,    // named and_
  ANDI = 18,
  OR = 19,     // named or_
  ORI = 20,
  XOR = 21,    // named xor_
  XORI = 22,
  NOT = 23,    // done named not_
  MOV = 24,
  ADDD = 32,
  SUBD = 33,
  MULD = 34,
  DIVD = 35,
  ITOD = 36,
  DTOI = 37,
  PUSH = 38,
  POP = 39,
  CALL = 40,
  CALLI = 41,
  RET = 42,
  CMP = 43,
  CMPI = 44,
  CMPD = 45,
  JMP = 46,
  JNE = 47,
  JEQ = 48,
  JLE = 49,
  JL = 50,
  JGE = 51,
  JG = 52,
  LOAD = 64,
  STORE = 65,
  LOAD2 = 66,
  STORE2 = 67,
  LOADR = 68,
  LOADR2 = 69,
  STORER = 70,
  STORER2 = 71
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
    {LOADR, "loadr", RR},     {STORER, "storer", RR},   {LOADR2, "loadr2", RR},
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

int load (int suffix, int adress) {
  r[suffix].word = Memory[adress];
  return 0;
}

int store (int suffix, int adress) {
  Memory[adress] = r[suffix].word;
  return 0;
}

int load2 (int suffix, int adress) {
  r[suffix].word = Memory[adress];
  r[suffix + 1].word = Memory[adress + 1];
  return 0;
}

int store2 (int suffix, int adress) {
  Memory[adress] = r[suffix].word;
  Memory[adress + 1] = r[suffix + 1].word;
  return 0;
}

int loadr (int suffix1, int suffix2, int delta) {
  r[suffix1].word = Memory[r[suffix2].word + delta];
  return 0;
}

int storer (int suffix1, int suffix2, int delta) {
  Memory[r[suffix2].word + delta] = r[suffix1].word;
  return 0;
}

int loadr2 (int suffix1, int suffix2, int delta) {
  r[suffix1].word = Memory[r[suffix2].word + delta];
  r[suffix1 + 1].word = Memory[r[suffix2].word + delta + 1];
  return 0;
}

int storer2 (int suffix1, int suffix2, int delta) {
  Memory[r[suffix2].word + delta] = r[suffix1].word;
  Memory[r[suffix2].word + delta + 1] = r[suffix1 + 1].word;
  return 0;
}

int add (int suffix1, int suffix2, int delta) {
  r[suffix1].word += r[suffix2].word + delta;
  return 0;
}

int addi (int suffix, int delta) {
  r[suffix].word += delta;
  return 0;
}

int sub (int suffix1, int suffix2, int delta) {
  r[suffix1].word -= r[suffix2].word + delta;
  return 0;
}

int subi (int suffix, int delta) {
  r[suffix].word -= delta;
  return 0;
}

int and_ (int suffix1, int suffix2, int delta) {
  r[suffix1].word &= r[suffix2].word + delta;
  return 0;
}

int andi (int suffix, int imm) {
  r[suffix].word &= imm;
  return 0;
}

int or_ (int suffix1, int suffix2, int delta) {
  r[suffix1].word |= r[suffix2].word + delta;
  return 0;
}

int ori (int suffix, int imm) {
  r[suffix].word |= imm;
  return 0;
}

int xor_ (int suffix1, int suffix2, int delta) {
  r[suffix1].word ^= r[suffix2].word + delta;
  return 0;
}

int xori (int suffix, int imm) {
  r[suffix].word ^= imm;
  return 0;
}

int not_ (int suffix, int imm) {
  r[suffix].word ^= 0xFFFFFFFF;
  imm = 0;
  return 0;
}

int mov (int suffix1, int suffix2, int delta) {
  r[suffix1].word = r[suffix2].word + delta;
  return 0;
}

int lc (int suffix, int imm) {
  r[suffix].word = imm;
  return 0;
}

int mul (int suffix1, int suffix2, int delta) {
  long long pro =
      (long long)r[suffix1].word * (long long)(r[suffix2].word + delta);
  r[suffix1].word = pro % (long long)pow(2, 32);
  r[suffix1 + 1].word = pro >> 32;
  return 0;
}

int muli (int suffix, int imm) {
  long long pro = (long long)r[suffix].word * (long long)imm;
  r[suffix].word = pro % (long long)pow(2, 32);
  r[suffix + 1].word = pro >> 32;
  return 0;
}

int div (int suffix1, int suffix2, int delta) {
  long long dividend =
      (long long)r[suffix1 + 1].word * (long long)pow(2, 32) + (long long)r[suffix1].word;
  r[suffix1].word = dividend / (r[suffix2].word + (long long)delta);
  r[suffix1 + 1].word = dividend % (r[suffix2].word + delta);
  return 0;
}

int divi (int suffix, int imm) {
  long long dividend =
      (long long)r[suffix + 1].word * (long long)pow(2, 32) + (long long)r[suffix].word;
  r[suffix].word = dividend / imm;
  r[suffix + 1].word = dividend % imm;
  return 0;
}

int shl (int suffix1, int suffix2, int delta) {
  r[suffix1].word <<= r[suffix2].word + delta;
  return 0;
}

int shli (int suffix, int imm) {
  r[suffix].word <<= imm;
  return 0;
}

int shr (int suffix1, int suffix2, int delta) {
  r[suffix1].word >>= r[suffix2].word + delta;
  return 0;
}

int shri (int suffix, int imm) {
  r[suffix].word >>= imm;
  return 0;
}

int addd (int suffix1, int suffix2, int delta) {
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

int subd (int suffix1, int suffix2, int delta) {
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

int muld (int suffix1, int suffix2, int delta) {
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

int divd (int suffix1, int suffix2, int delta) {
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

int itod (int suffix1, int suffix2, int delta) {
  double d = r[suffix2].word + delta;
  long long mask = *(long long*)&d;
  r[suffix1 + 1].word = mask >> 32;
  r[suffix1].word = mask % (long long)pow(2, 32);
  return 0;
}

int dtoi(int suffix1, int suffix2, int delta) {
  long long ans = (long long)r[suffix2 + 1].word * (long long)pow(2, 32) +
                  (long long)r[suffix2].word + *(long long *)&delta;
  double d = *(double *)&ans;    // read as double
  r[suffix1].word = (long int)d; // ignore frac
  return 0;
}

int push(int suffix, int imm) {
  r[14].word--;
  Memory[r[14].word] = r[suffix].word + imm;
  return 0;
}

int pop(int suffix, int imm) {
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

int syscall(int suffix, int code) {
  double d;
  long long ans;
  char c;
  switch (code) {
  case 0:
    exit(r[suffix].word);
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

int halt(int suffix, int delta) { return 0; }

int calli(string l) {
  push(15, 1);
  r[15].word = label[l];
  return 0;
}

int ret(string s) {
  r[15].word = Memory[r[14].word];
  int imm = stoi(s);
  r[14].word += imm + 1;
  return 0;
}

int call(int suffix1, int suffix2, int imm) {
  push(15, 1);
  r[suffix1].word = r[suffix2].word;
  r[15].word = r[suffix2].word;
  return 0;
}

int jmp(string l) {
  r[15].word = label[l];
  return 0;
}

int jne(string l) {
  if (flags.condition != "==") {
    r[15].word = label[l];
  } else {
    r[15].word++;
  }
  return 0;
}

int jeq(string l) {
  if (flags.condition == "==") {
    r[15].word = label[l];
  } else {
    r[15].word++;
  }
  return 0;
}

int jle(string l) {
  if (flags.condition == "==" || flags.condition == "<") {
    r[15].word = label[l];
  } else {
    r[15].word++;
  }
  return 0;
}

int jl(string l) {
  if (flags.condition == "<") {
    r[15].word = label[l];
  } else {
    r[15].word++;
  }
  return 0;
}

int jge(string l) {
  if (flags.condition == "==" || flags.condition == ">") {
    r[15].word = label[l];
  } else {
    r[15].word++;
  }
  return 0;
}

int jg(string l) {
  if (flags.condition == ">") {
    r[15].word = label[l];
  } else {
    r[15].word++;
  }
  return 0;
}

std::set<int> special;
std::vector< pair<int, int> > execute(20000); // first--code, second--mode
std::map <string, pair<int, int> > string_cmd; //cdms_info)
std::map <int, vector<int> > args_RR; //arguments
std::map <int, vector<int> > args_RM_RI;
std::map <int, string> args_special;

int code_to_memory() {
  int index = 128; // index to Memory (code segment)
  while (!fin.eof()) {
    string s;
    fin >> s;

    if (s[0] == ';') { //read comments
      string l;
      getline (fin, l);
      continue;
    }

    if (s[s.length() - 1] == ':') {
      label[s] = index;
      continue;
    }

    if (s == "end") {
      string start;
      fin >> start;
      r[15].word = label[start + ":"];
      execute[index] = pair<int, int>(-1, -1); //end)
      return 0;
    }

        if ( s!= "word" && (string_cmd[s].second == RM || string_cmd[s].second == RI) ) {

          if (special.find(string_cmd[s].first) != special.end()) {
            string l;
            fin >> l;
            args_special[index] = l + ":";
            int token_code = string_cmd[s].first;
            int token = (token_code << 24) + label[l+":"];
            execute[index].first = string_cmd[s].first;
            execute[index].second = string_cmd[s].second;
            Memory[index++] = token;
            continue;
          }

          string reg, delta;
          char r;
          fin >> r >> reg >> delta;

            vector <int> a(2);
          int reg_int = stoi(reg);
          a[0] = reg_int;
          int delta_int;
          if ( label[delta + ":"] > 0 ) {
            delta_int = label[delta + ":"];
          } else {
            delta_int = stoi(delta);
          }
          a[1] = delta_int;
          args_RM_RI[index] = a;

          int token_code = string_cmd[s].first;
          int token = (token_code << 24) + (reg_int << 20) + delta_int;
          execute[index].first = string_cmd[s].first;
          execute[index].second = string_cmd[s].second;

          Memory[index++] = token;
        }

        if (string_cmd[s].second == RR && s!= "word") {
          string reg1, reg2, delta;
          char r1, r2;
          if (s != "call") {
            fin >> r1 >> reg1 >> r2 >> reg2 >> delta;
          } else {
            fin >> r1 >> reg1 >> r2 >> reg2;
            delta = "0";
          }

            vector <int> a(3);
          int reg1_int = stoi(reg1);
          a[0] = reg1_int;
          int reg2_int = stoi(reg2);
          a[1] = reg2_int;
          int delta_int = stoi(delta);
          a[2] = delta_int;
          args_RR[index] = a;
          int token_code = string_cmd[s].first;

          int token = (token_code << 24) + (reg1_int << 20) + (reg2_int << 16) +
                      delta_int;
          execute[index].first = string_cmd[s].first;
          execute[index].second = string_cmd[s].second;

          Memory[index++] = token;
        }
  }
  return 0;
}




int main() {

  string_cmd["halt"] = pair<int, int>(HALT, RI);
  string_cmd["syscall"] = pair<int, int>(SYSCALL, RI);
  string_cmd["add"] = pair<int, int>(ADD, RR);
  string_cmd["addi"] = pair<int, int>(ADDI, RI);
  string_cmd["sub"] = pair<int, int>(SUB, RR);
  string_cmd["subi"] = pair<int, int>(SUBI, RI);
  string_cmd["mul"] = pair<int, int>(MUL, RR);
  string_cmd["muli"] = pair<int, int>(MULI, RI);
  string_cmd["div"] = pair<int, int>(DIV, RR);
  string_cmd["divi"] = pair<int, int>(DIVI, RI);
  string_cmd["lc"] = pair<int, int>(LC, RI);
  string_cmd["shl"] = pair<int, int>(SHL, RR);
  string_cmd["shli"] = pair<int, int>(SHLI, RI);
  string_cmd["shr"] = pair<int, int>(SHR, RR);
  string_cmd["shri"] = pair<int, int>(SHRI, RI);
  string_cmd["and"] = pair<int, int>(AND, RR);
  string_cmd["andi"] = pair<int, int>(ANDI, RI);
  string_cmd["or"] = pair<int, int>(OR, RR);
  string_cmd["ori"] = pair<int, int>(ORI, RI);
  string_cmd["xor"] = pair<int, int>(XOR, RR);
  string_cmd["xori"] = pair<int, int>(XORI, RI);
  string_cmd["not"] = pair<int, int>(NOT, RI);
  string_cmd["mov"] = pair<int, int>(MOV, RR);
  string_cmd["addd"] = pair<int, int>(ADDD, RR);
  string_cmd["subd"] = pair<int, int>(SUBD, RR);
  string_cmd["muld"] = pair<int, int>(MULD, RR);
  string_cmd["divd"] = pair<int, int>(DIVD, RR);
  string_cmd["itod"] = pair<int, int>(ITOD, RR);
  string_cmd["dtoi"] = pair<int, int>(DTOI, RR);
  string_cmd["push"] = pair<int, int>(PUSH, RI);
  string_cmd["pop"] = pair<int, int>(POP, RI);
  string_cmd["call"] = pair<int, int>(CALL, RR);
  string_cmd["calli"] = pair<int, int>(CALLI, RM);
  string_cmd["ret"] = pair<int, int>(RET, RI);
  string_cmd["cmp"] = pair<int, int>(CMP, RR);
  string_cmd["cmpi"] = pair<int, int>(CMPI, RI);
  string_cmd["cmpd"] = pair<int, int>(CMPD, RR);
  string_cmd["jmp"] = pair<int, int>(JMP, RM);
  string_cmd["jne"] = pair<int, int>(JNE, RM);
  string_cmd["jeq"] = pair<int, int>(JEQ, RM);
  string_cmd["jle"] = pair<int, int>(JLE, RM);
  string_cmd["jl"] = pair<int, int>(JL, RM);
  string_cmd["jge"] = pair<int, int>(JGE, RM);
  string_cmd["jg"] = pair<int, int>(JG, RM);
  string_cmd["load"] = pair<int, int>(LOAD, RM);
  string_cmd["store"] = pair<int, int>(STORE, RM);
  string_cmd["load2"] = pair<int, int>(LOAD2, RM);
  string_cmd["store2"] = pair<int, int>(STORE2, RM);
  string_cmd["loadr"] = pair<int, int>(LOADR, RR);
  string_cmd["storer"] = pair<int, int>(STORER, RR);
  string_cmd["loadr2"] = pair<int, int>(LOADR2, RR);
  string_cmd["storer2"] = pair<int, int>(STORER2, RR);



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
  fin.clear();
  fin.seekg(0, fin.beg);
  code_to_memory(); //we have to do it twice

  r[14].word = 1048575;

  string s;

  while (execute[r[15].word].first != -1) {
      int current_mode = execute[r[15].word].second;
      int current_cmd = execute[r[15].word].first;
        if (current_mode == RM || current_mode == RI) {

          if (special.find(current_cmd) != special.end()) {
            spec_func[current_cmd](args_special[r[15].word]);
            continue;
          }

          cmds_RM_RI[current_cmd](args_RM_RI[r[15].word][0], args_RM_RI[r[15].word][1]);
          r[15].word++;
          continue;
        }


          cmds_RR[current_cmd](args_RR[r[15].word][0], args_RR[r[15].word][1],args_RR[r[15].word][2]);
          if(current_cmd != CALL) {
          r[15].word++;
        }

  }

  fin.close();
  return 0;
}
