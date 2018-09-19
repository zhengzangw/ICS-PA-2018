#include "nemu.h"
#include <stdlib.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
  TK_NOTYPE = 256, TK_EQ, TK_NUM, TK_U

  /* TODO: Add more token types */

};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */
  {"\\)", ')'},			// right brace
  {"\\(", '('},			// left brace
  {"/", '/'},			// divide
  {"\\*", '*'},			// multiply
  {"[ \n]+", TK_NOTYPE},    // spaces
  {"\\+", '+'},         // plus
  {"\\-", '-'},			// minus
  {"==", TK_EQ},        // equal
  {"[0-9]+", TK_NUM},	// number
  {"u", TK_U}
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

        //Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
        //    i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;

		uint32_t val = 0;
        switch (rules[i].token_type) {
			case TK_NUM:
				for (int i=0;i<substr_len;++i)
					val = val*10+substr_start[i]-'0';
				sprintf(tokens[nr_token].str,"%u", val);
				tokens[nr_token++].type = rules[i].token_type;

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

  return true;
}

bool check_parenthesis(int s, int t, bool *success, char *e){
	int count = 0,flag = 1;
	for (int i=s;i<=t;++i){
		if (tokens[i].type == '(') count++;
		if (tokens[i].type == ')') count--;
		if (count<1 && i!=t) flag = 0;
		if (count<0) {
			printf("Barce no match!\n");
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

int prime_op(int s,int t){
	int count = 0, pos=s-1, curop=256;
	for (int i=s;i<=t;++i){
		switch (tokens[i].type){
			case '(': count++; break;
			case ')': count--; break;
			case '*':
			case '/': if (count==0 && curop!='+' && curop!='-'){
						curop = tokens[pos=i].type;
					  }
					  break;
			case '+':
			case '-':
					  if (count==0){
						curop = tokens[pos=i].type;
					  }
			default: break;
		}
	}
	return pos;
}

uint32_t eval(int s, int t, bool *success, char *e){
	if (!*success) return 0;
	if (s>t){
		printf("Empty brace!\n");
		*success = false;
	} else if (s==t) {
		if (tokens[s].type!=TK_NUM){
			*success = false;
		} else {
			uint32_t val = strtol(tokens[s].str,NULL,10);
			return val;
		}
	} else if (check_parenthesis(s,t,success,e)){
		return eval(s+1,t-1,success,e);
	} else if (*success){
		int op = prime_op(s,t);
		if (op<s) {
			printf("Lack of operands!\n");
			*success = false;
			return false;
		}
		uint32_t val2 = eval(op+1,t,success,e);
		while (op>s && tokens[op].type=='-'&&tokens[op-1].type!=')'&&tokens[op-1].type!=TK_NUM){
			val2 = -val2;
			op = prime_op(s,op-1);
		}
		//Log("From %d to %d, Prime op is %c at tokens[%d]", s, t, tokens[op].type, op);
		uint32_t val1 = op>s?eval(s,op-1,success,e):0;
		if (!*success) return 0;
		switch (tokens[op].type){
			case '+': return val1+val2;
			case '-': return val1-val2;
			case '*': return val1*val2;
			case '/': if (val2==0) {
						printf("Divided by zero at position %d\n%s\n%*.s^\n",op,e,op,"");
						*success = false;
						return 0;
					  }
					  return val1/val2;
			default: *success = false;
		}
	}
	return 0;
}

uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  } 
  
  return eval(0, nr_token-1, success, e);
}


