#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <sys/types.h>
#include "include/player_strapi.h"
#include "include/player_wavespec.h"
#include "include/player_backend.h"
#include "include/player_dataapi.h"
#include "include/player_parser.h"
// 十二平均律实现
static float note2freq(char * str,int * cosumed_len){
	int len=0;
	int offset=0;
	if (*str == '0'){
		len=1;
		return 0; //休止符只占一个字符, 即0,无前导字母,像其他音符一样,必须加入时值
	}
	if (*str == '#'){
		len=3;
		if (str[1] >= 'C' && str[1] <= 'E'){
			offset=2*(str[1]-'C')+(str[2]-'1')*12;
			goto next;
		}
		if (str[1] == 'F' || str [1] == 'G'){
			offset=2*(str[1]-'C')-1+(str[2]-'1')*12;
			goto next;
		}
		if (str[1] == 'A' || str[1] == 'B'){
			offset=2*(str[1]-'C')+1+(str[2]-'1')*12;
			goto next;
		}
	} else {
		len=2;
		if (str[0] >= 'C' && str[0] <= 'E'){
			offset=2*(str[0]-'C'+(str[1]-'1')*12);
			goto next;
		}
		if (str[0] == 'F' || str [0] == 'G'){
			offset=2*(str[0]-'C')-1+(str[1]-'1')*12;
			goto next;
		}
		if (str[0] == 'A' || str[0] == 'B'){
			offset=2*(str[0]-'C')+1+(str[1]-'1')*12;
			goto next;
		}	
	}
	next:
	if (cosumed_len != NULL){
		*cosumed_len = len;
	}
	return 440*pow(2,(float)offset/12);
}

int parse_note(char * buffer,struct WaveArgs * arg,const struct GlobalStatus * gs,const struct LoudnessLoopTab *llpt,float ticks,size_t len){
	int backup_len=len;
	int cosumed_len=0;
	int force_loudness_set=0;
	arg->freq=note2freq(buffer,&cosumed_len);

	buffer+=cosumed_len;
	len-=cosumed_len;
	while(len > 0 && *buffer != ' '){
		switch(*buffer){
			case 'T':{
				arg->time=atof(buffer+1)/(gs->bpm)*60;
				cosumed_len=next_letter_offset(buffer+1)+1;
				buffer+=cosumed_len;
				len-=cosumed_len;
				break;
			}
			case 'L':{
				arg->Loudnessfac=atof(buffer+1);
				cosumed_len=next_letter_offset(buffer+1)+1;
				buffer+=cosumed_len;
				len-=cosumed_len;
				force_loudness_set=1;
				break;
			}
			default:{
				goto exit;
			}
		}
	}

	exit:

	arg->wrote_len=0;
	arg->pending_len=0;
	if (force_loudness_set==0){
		arg->Loudnessfac=llpt->Tab[LoopFArrayLookup(llpt->Tab,ticks,llpt->Len,3)+2];
	}
	return backup_len-len;
}