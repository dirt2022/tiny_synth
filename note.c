#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <sys/param.h>
#include <sys/types.h>
#include "include/player_strapi.h"
#include "include/player_wavespec.h"
#include "include/player_backend.h"
#include "include/player_limit.h"
#include "include/player_dataapi.h"
#include "include/player_parser.h"
// 十二平均律实现
static const char * KeyboardTab[]={
	"A0","#A0","B0","C1","#C1","D1","#D1","E1","F1",
	"#F1","G1","#G1","A1","#A1","B1","C2","#C2","D2",
	"#D2","E2","F2","#F2","G2","#G2","A2","#A2","B2",
	"C3","#C3","D3","#D3","E3","F3","#F3","G3","#G3",
	"A3","#A3","B3","C4","#C4","D4","#D4","E4","F4",
	"#F4","G4","#G4","A4","#A4","B4","C5","#C5","D5",
	"#D5","E5","F5","#F5","G5","#G5","A5","#A5","B5",
	"C6","#C6","D6","#D6","E6","F6","#F6","G6","#G6",
	"A6","#A6","B6","C7","#C7","D7","#D7","E7","F7",
	"#F7","G7","#G7","A7","#A7","B7","C8"
};
static float note2freq(char * str,int * cosumed_len){
	int len=0;
	int offset=0;
	char buff[4]; // 无需初始化

	if (*str == '0'){
		if (cosumed_len != NULL){
			*cosumed_len = 1;
		}
		return 0.0f; //休止符只占一个字符, 即0,无前导字母,像其他音符一样,必须加入时值
	}
	if (*str == '#'){
		len=3;
		for (unsigned int i=0;i<3;i++){
			buff[i]=str[i];
		}
		buff[3]='\0';
		offset=strlookup(buff,KeyboardTab,88)-48; // A4 偏移
	} else {
		len=2;
		for (unsigned int i=0;i<2;i++){
			buff[i]=str[i];
		}
		buff[2]='\0';
		offset=strlookup(buff,KeyboardTab,88)-48; // A4 偏移
	}
	if (cosumed_len != NULL){
		*cosumed_len = len;
	}
	return 440*pow(2,(float)offset/12);
}

int parse_note(char * buffer,struct WaveArgs * arg,const struct GlobalStatus * gs,const struct LoudnessLoopTab *llpt,float ticks,size_t len){
	int backup_len=len;
	int cosumed_len=0;
	int force_loudness_set=0;
	int space_flag=0;
	arg->freq=note2freq(buffer,&cosumed_len);

	buffer+=cosumed_len;
	len-=cosumed_len;
	while(len > 0 && *buffer != ' '){
		switch(*buffer){
			if(space_flag){ //此时已经归位到下一个param,开头不为空格的检查不会触发,故手动退出
				break;
			}
			case 'T':{
				arg->total_len=atof(buffer+1)/(gs->bpm)*60*BACKEND_RATE;
				cosumed_len=next_letter_offset(buffer+1,&space_flag)+1; //要跳过字母T本身
				buffer+=cosumed_len;
				len-=cosumed_len;
				break;
			}
			case 'L':{
				arg->Loudnessfac=atof(buffer+1);
				cosumed_len=next_letter_offset(buffer+1,&space_flag)+1; //同理
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
