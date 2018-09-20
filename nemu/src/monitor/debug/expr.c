#include "nemu.h"
#include <stdlib.h>
#include <stdarg.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
  TK_NOTYPE = 256, TK_EQ, TK_DNUM, TK_U, TK_HNUM, TK_REG, TK_NEQ, TK_AND, TK_DEREF, TK_NEG
};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {
  {"\\)", ')'},			// right brace
  {"\\(", '('},			// left brace
  {"/", '/'},			// divide
  {"\\*", '*'},			// multiply
  {"[ \n]+", TK_NOTYPE},// spaces
  {"\\+", '+'},         // plus
  {"\\-", '-'},			// minus
  {"&&", TK_AND},		// and
  {"==", TK_EQ},        // equal
  {"!=", TK_NEQ},		// unequal
  {"\\$(e[a-d]x|e[sd]i|e[bs]p)", TK_REG}, // register
  {"0[xX][0-9a-fA-F]+", TK_HNUM},// hexical number
  {"[0-9]+", TK_DNUM},	// demical number
  {"u", TK_U}			// u sign
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

Token tokens[1000];
int nr_token;

static inline bool singlecheck(int pos){
	if (pos==0) return true;
	int type = tokens[pos-1].type;
	if (type==')') return false;
	if (type==TK_DNUM) return false;
	if (type==TK_HNUM) return false;
	if (type==TK_REG)  return false;
	return true;
}

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;
  nr_token = 0;

  while (e[position] != '\0') {
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;

		uint32_t val = 0,cnt=0;
        switch (rules[i].token_type) {
			case TK_DNUM:
				for (int i=0;i<substr_len;++i)
					val = val*10+substr_start[i]-'0';
				sprintf(tokens[nr_token].str,"%u", val);
				tokens[nr_token++].type = TK_DNUM;
				break;

			case TK_HNUM:
				for (int i=2;i<substr_len;++i){
					if ('0'<=substr_start[i]&&substr_start[i]<='9')
						cnt = substr_start[i]-'0';
					else if ('a'<=substr_start[i]
							&&substr_start[i]<='f')
						cnt = substr_start[i]-'a'+10;
					else cnt = substr_start[i]-'A'+10;
					val = val*16+cnt;
				}
				sprintf(tokens[nr_token].str, "%u", val);
				tokens[nr_token++].type = TK_DNUM;

			case TK_REG:
			case TK_U:
			case TK_NOTYPE:
				break;
          default:
				tokens[nr_token++].type = rules[i].token_type;
        }
		break;
      }
    }

    if (i == NR_REGEX) {
	  Log("%d",e[position]);
      printf("No match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

	for (int i=0;i<nr_token;i++){
		if (tokens[i].type=='*' && singlecheck(i)){
			tokens[i].type = TK_DEREF;
			Log("Change Tokens: At %d, to DEREF", i);
		}
		if (tokens[i].type=='-' && singlecheck(i)){
			tokens[i].type = TK_NEG;
			Log("Change Tokens: At %d, to NEG", i);
		}
	}
  
	return true;
}

bool check_parenthesis(int s, int t, bool *success){
	int count = 0,flag = 1;
	Log("s=%d,t=%d",s,t);
	for (int i=s;i<=t;++i){
		if (tokens[i].type == '(') count++;
		if (tokens[i].type == ')') count--;
		if (count<1 && i!=t) flag = 0;
		if (count<0) {
			printf("-Barce no match!\n");
			*success = false;
			return false;
		}
	}
	if (count>0) {
		printf("Brace no match!\n");
		*success = false;
		return false;
	}	
	if (flag && count==0) return true;
	return false; 
}

static int prime_op(int s,int t){
	int pos=s-1, curop=256;
	for (int i=t;i>=s;--i){
		switch (tokens[i].type){
			case '*':
			case '/': 
					  if (curop==256){
						curop = tokens[pos=i].type;
					  }
					  break;
			case '+':
			case '-':
					  if (curop!=TK_EQ&&curop!=TK_NEQ&&curop!='+'&&curop!='-'){
						curop = tokens[pos=i].type;
					  }
					  break;
			case TK_EQ:
			case TK_NEQ:
					  if (curop!=TK_EQ&&curop!=TK_NEQ){
					    pos = i; curop = tokens[pos=i].type;
					  }
					  break;
			case TK_AND:
					  return i;
			default: break;
		}
	}
	return pos;
}

static uint32_t eval(int s, int t, bool *success){
	if (!*success) return 0;
	if (s>t){ //Error
		Log("s=%d,t=%d",s,t);
		printf("Empty brace!\n");
		*success = false;
	} else //Number
		if (s==t) {
			if (tokens[s].type==TK_DNUM){
				return strtol(tokens[s].str,NULL,10);
			} else if (tokens[s].type==TK_REG) {
				return cpu.eax; 
			} else *success = false;
	} else //Brace
		if (check_parenthesis(s,t,success)){
			return eval(s+1,t-1,success);
	} else
		if (*success){
			int op = prime_op(s,t);
			if (op<s) {
				if (tokens[s].type==TK_NEG){//Neg
					return -eval(s+1,t,success);
				} else 
				if (tokens[s].type==TK_DEREF){//Deref
					uint32_t size = 4;
					uint32_t x_pos = eval(s+1,t,success);
					if (x_pos>=0x8000000){
						printf("Out of range 0x8000000, not available!\n");
						*success = false;
					} else {
						return vaddr_read(x_pos,size);
					}
				} else {
					printf("Lack of operands!\n");
					*success = false;
				}
			} else {	
				uint32_t val2 = eval(op+1,t,success);
				uint32_t val1 = eval(s,op-1,success);
				if (!*success) return false;
				switch (tokens[op].type){
					case '+': return val1+val2;
					case '-': return val1-val2;
					case '*': return val1*val2;
					case '/': if (val2==0) {
							printf("Divided by zero!\n");
							*success = false;
							return false;
						  }
						  return val1/val2;
					case TK_EQ:
						  return val1==val2;
					case TK_NEQ:
						  return val1!=val2;
					case TK_AND:
						  return val1&&val2;
					default: *success = false;
				}
			}
		}	
	return 0;
}

uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  } 
  return eval(0, nr_token-1, success);
}


