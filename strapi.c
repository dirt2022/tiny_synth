#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "include/player_memmacro.h"
// this module defines args parser functions.
static int halfcmp(const char *s1, const char *s2, char end) {
    // if end == '\0', please use strcmp for better performance.
    // s1 should be longer than s2.
    if (strlen(s1) < strlen(s2)) {
        const char *tmp;
        tmp = s1;
        s1 = s2;
        s2 = tmp;
    }
    for (;;) {
        if ((*s1 == end || *s1 == '\0') && *s2 == '\0') {
            return 1;
        }
        if (*s1 != *s2) {
            return 0;
        }
        s1++;
        s2++;
    }
}

unsigned int next_space_offset(const char * str){
	unsigned int offset=0;
	for(;str[offset]!=0 && str[offset] != ' ';){
		offset++;
	}
	return offset;
}

unsigned int next_letter_offset(const char * str){
	unsigned int offset=0;
	for(;str[offset]!=0 &&
		str[offset]!=' ' && ( (str[offset] < 'A' || str[offset] > 'Z') &&
		(str[offset] < 'a' || str[offset] > 'z') ) ;){
		offset++;
	}
	return offset;
}

float * str2farray(const char * str,size_t * wrote_array_len){
	unsigned int cnt=1; // 即使没有空格,也应该有1个数字,有n个空格就有n+1个数字
	float * res=NULL;
	for (unsigned int i=0;str[i]!=0;i++){
		if(str[i] == ' '){
			cnt++;
		}
	}
	SAFE_MALLOC_DEF(res,cnt*sizeof(float));
	for(unsigned int i=0;i<cnt;i++){
		res[i]=atof(str);
		str+=next_space_offset(str)+1; // We don't have to restore the pointer.
	}
	if(wrote_array_len != NULL){
		*wrote_array_len=cnt;
	}
	return res;
}

unsigned int strlookup(const char *str,const char **tab, size_t lines_of_tab) {
    for (unsigned int i = 0; i < lines_of_tab; i++) {
        if (halfcmp(str, tab[i], ' ')) {
            return i;
        }
    }
    return -1;
}

size_t numlen(char * str){
	size_t len=0;
	for(;;){
		if(str[len] != '.' && 
		(str[len] > '9' || str[len] < '0') ){
			break;	
		}
		len++;
	}
	return len;
}

int myfgets(char * buffer,int size,FILE *fp){
	int ch=0;
	for(unsigned int i=0;i < size;i++){
		ch=fgetc(fp);
		if(ch==EOF){
			buffer[i]=0;
			return i;
		}
		if (ch == '\n' || ch == '\r'){
			ch=0;
			buffer[i]=ch;
			return i;
		}
		buffer[i]=ch;
	}
	return 0;
}