#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
	wp_pool[i].enable = false;
    wp_pool[i].next = &wp_pool[i + 1];
	wp_pool[i].pre  = i==0?NULL:&wp_pool[i - 1];
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

static WP* new_wp_() {
	if (free_ == NULL){
		printf("Watchpoint pool full!\n");
	} else {
		WP* ret = free_;
		free_ = free_->next;
		ret -> enable = true;
		ret -> next = NULL;
		if (head!=NULL){
			head -> next = ret;
		}
		head = ret;
		return ret;
	}
	assert(free_!=NULL);
	return NULL;	
}

static void free_wp_(WP *wp){
	assert(wp!=NULL);
	if (wp==head){
		head = wp->next;
		wp->next->pre = NULL;
	} else {
		wp->pre->next = wp->next;
		wp->next->pre = wp->pre;
	}
	wp->enable = false;
	free_->next = wp;
	free_ = wp;	
}

void free_wp(uint32_t num){
	if (wp_pool[num].enable){
		free_wp_(wp_pool+num);
	} else {
		printf("No such watchpoint!\n");
	}
}

void new_wp(char *arg){
	bool success = true;
	uint32_t val = expr(arg,&success);
	if (success) {
		WP* tmp = new_wp_();
		strcpy(tmp->expression,arg);
		printf("Create Watchpoint successfully! NO.%d: %s = %u\n", tmp->NO, tmp->expression, val);
	} else {
		printf("Invalid expression. Create failed for %s", arg);
	}
}

void wp_info(){
	if (head==NULL){
		printf("No watchpoint in the pool!\n");
	} else {
		for (WP *p=head;p!=NULL;p=p->next){
			bool flag = true;
			printf("Watchpoint No.%d: ", p->NO);
			uint32_t val = expr(p->expression,&flag);
			if (flag) {
				printf("%s = %u\n", p->expression, val);
			} else {
				printf("Error in calculating %s\n", p->expression);
			}
		}
	}
}

