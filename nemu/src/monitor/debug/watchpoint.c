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
		ret -> next = head;
		ret -> pre  = NULL;
		if (head!=NULL){
			head -> pre = ret;
		}
		head = ret;
		return ret;
	}
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
	wp->pre    = NULL;
	wp->next   = free_;
	if (free_!=NULL){
		free_->pre = wp;
	}
	free_ = wp;	
}

void free_wp(uint32_t num){
	if (num>=0&&num<NR_WP&&wp_pool[num].enable){
		free_wp_(wp_pool+num);
		printf("Delete Watchpoint NO.%d \n", wp_pool[num].NO);
	} else {
		printf("No such watchpoint!\n");
	}
}

void new_wp(char *arg){
	bool success = true;
	uint32_t val = expr(arg,&success);
	if (success) {
		WP* tmp = new_wp_();
		if (tmp==NULL) {
			printf("Create FAILED!\n");
			return;
		}
		strcpy(tmp->expression,arg);
		printf("Watchpoint %d: %s = %u\n", tmp->NO, tmp->expression, val);
	} else {
		printf("Invalid expression. Create FAILDED!\n");
	}
}

void wp_info(){
	printf("%20s%20s%20s\n","NO.","Value","Expression");
	for (WP *p=head;p!=NULL;p=p->next){
		bool flag = true;
		printf("%20d", p->NO);
		uint32_t val = expr(p->expression,&flag);
		if (flag) {
			printf("%20d", val);
		} else {
			printf("%20s", "Error");
		}
		printf("%20s\n", p->expression);
	}
}

