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
	wp_pool[i].valid  = true;
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
		tmp->val = val;
		printf("Watchpoint %d: %s = %u\n", tmp->NO, tmp->expression, val);
	} else {
		printf("Invalid expression. Create FAILDED!\n");
	}
}

void wp_info(){
	printf("%s  %20s  %30s\n","NO.","Value","Expression");
	for (WP *p=head;p!=NULL;p=p->next){
		printf("%d  ", p->NO);
		if (p->valid) {
			printf("%20u", p->val);
		} else {
			printf("%20s", "Error");
		}
		printf("  %30s\n", p->expression);
	}
}

bool wp_check()
{
	bool flag = false;
	for (WP *p=head;p!=NULL;p=p->next){
		bool suc = true,change = false;
		uint32_t val = expr(p->expression,&suc);
		if (suc) {
			if (!p->valid){
				printf("Hit watchpoint %d: %s\n", p->NO, p->expression);
				printf("Old value = ERROR\nNew value = %u\n",val);
				p->valid = true; p->val = val;
				change =true;
			}
			else if (val!=p->val){
				printf("Hit watchpoint %d: %s\n", p->NO, p->expression);
				printf("Old value = %u\nNew value = %u\n", p->val, val);
				p->val = val;
				change = true;
			}
		} else {	
			printf("Hit watchpoint %d: %s\n", p->NO, p->expression);
			printf("Old value = %u\nNew value = ERROR\n", p->val);
			p->valid = false;
			change = true;
		}
		if (change) {
			puts("\n");
			flag =true;
		}
	}			
	return flag;
}
