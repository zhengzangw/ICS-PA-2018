#include "nemu.h"
#include <stdlib.h>

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
  TK_NOTYPE = 256, TK_EQ, TK_NUM

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
  {" +", TK_NOTYPE},    // spaces
  {"\\+", '+'},         // plus
  {"\\-", '-'},			// minus
  {"==", TK_EQ},        // equal
  {"[0-9]+", TK_NUM}	// number
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

Token tokens[32];
int nr_token;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

		
        switch (rules[i].token_type) {
			case TK_NUM:
				if (substr_len>32) {
					printf("Input a number has more than 32 digits!\n");
					return false;
				}	
				strncpy(tokens[nr_token].str,substr_start,substr_len);
				tokens[nr_token++].type = rules[i].token_type;
				break;
			case TK_NOTYPE:
				break;
          default:
				tokens[nr_token++].type = rules[i].token_type;
        }
		
		Log("tokens[%d].str=%s, tokens[%d].type=%d", nr_token-1,tokens[nr_token-1].str,
				nr_token-1,tokens[nr_token-1].type);
        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

bool check_parenthesis(int s, int t, bool *success){
	int count = 0;
	for (int i=s;i<t;++i){
		if (tokens[i].type == '(') count++;
		if (tokens[i].type == ')') count--;
		if (count<1) return false;
	}
	Log("Before ruturn, count=%d, tokens[t].type=%c\n",count,tokens[t].type);
	if (count==1 && tokens[t].type == ')') return true;
	return false;
}

int prime_op(int s,int t){
	int count = 0, pos=-1, curop=256;
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

uint32_t eval(int s, int t, bool *success){
	if (s>t){
		assert(0);	
	} else if (s==t) {
		char *tmp;
		long val = strtol(tokens[s].str,&tmp,10);
		Log("val = %ld", val);
		return val;
	} else if (check_parenthesis(s,t,success)){
		return eval(s+1,t-1,success);
	} else {
		int op = prime_op(s,t);
		Log("Prime op is %c at tokens[%d]", tokens[op].type, op);
		uint32_t val1 = eval(s,op-1,success);
		uint32_t val2 = eval(op+1,t,success);
		switch (tokens[op].type){
			case '+': return val1+val2;
			case '-': return val1-val2;
			case '*': return val1*val2;
			case '/': return val1/val2;
			default: assert(0);
		}
		return 0;
	}	
}

uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  } 

  /* TODO: Insert codes to evaluate the expression. */
  return eval(0, nr_token-1, success);
}


