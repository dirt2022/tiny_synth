#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "include/player_wavespec.h"
#include "include/player_memmacro.h"
#include "include/player_limit.h"
#include "include/player_dataapi.h"
#define PI 3.141592653589793

static float loudness_attn_factor(float samplepercent,struct AttnTab * Tab,size_t *offset){
	float Factor=0;
	// Tab: 0: Start Percent 1: end percent 2: grad 3: sum Before the range
	if (samplepercent > Tab ->Attntab[*offset+1] ){
		*offset+=4;
	}
	Factor=Tab->Attntab[*offset+3]+(samplepercent-Tab->Attntab[*offset])*Tab->Attntab[*offset+2];
	return Factor;
}

void CalcLoudnessAttnTab(struct AttnTab * Tab){
	void * tmpptr;
	float StartPercent=0;
	float currentsum=0; // factor is 0 initially.

	size_t NewAttnTabLen=Tab->Tab2TLen*2;
	if(Tab->Tab2TLen%2==1){
		printf("Illegal Tab Length\n");
		abort();
	}
	if (Tab->TabLen==0){
		SAFE_MALLOC_DEF(Tab->Attntab,NewAttnTabLen*sizeof(float));
		Tab->TabLen=NewAttnTabLen;
	}
	else {
		if ( Tab ->TabLen != NewAttnTabLen){
			SAFE_REALLOC_DEF(Tab->Attntab,NewAttnTabLen*sizeof(float),tmpptr);
			Tab->TabLen=NewAttnTabLen;
		}
	}
	memset(Tab->Attntab,0,NewAttnTabLen*sizeof(float));
	for (unsigned int i=0;i < NewAttnTabLen;i+=4){
		if (Tab->Tab2T[i/2+1] < 0){
			printf("Fatal Error! The percentage must not be minus.\n");
			abort();
		}
		Tab->Attntab[i]=StartPercent;
		Tab->Attntab[i+1]=StartPercent+Tab->Tab2T[i/2+1];
		Tab->Attntab[i+2]=Tab->Tab2T[i/2];
		Tab->Attntab[i+3]=currentsum;
		currentsum+=Tab->Tab2T[i/2]*Tab->Tab2T[i/2+1];
		StartPercent=Tab->Attntab[i+1];
	}
	if (StartPercent != 1.0){
		printf("Warning: The percentage sum (%f %%) is not 100%%\n",StartPercent*100);
		printf("This may be caused by the precision loss. But it hardly happens.\n");
		printf("You should check the input file to ensure that the percentage is arranged correctly.\n");
		printf("We assume that the given input is correct, Trying to fix it automatically.\n");
		Tab->Attntab[NewAttnTabLen-1-2]=1.0; // 减1进入下标范围,减2为末百分比
	}
}

void WriteWave(float * Buffer,struct WaveTab * wt,struct WaveArgs * arg,size_t len){ //len refers to array element numbers.
	size_t whole_process_len=arg->total_len; // 处理整个音符的采样长度
	if (len < arg->pending_len){
		printf("Buffer length is not enough.\n");
		return;
	}
	if (arg->Attn.Attntab == NULL || arg->Attn.TabLen%4 != 0){
		printf("Illegal Tab Length or Attntab == NULL\n");
		printf("Hint: Did you forget to use SNAT instruction in the input file?\n");
		return;
	}
	if(wt->Tab2T == NULL || wt->Attn==NULL){
		printf("Illegal WaveTab\n");
		printf("Hint: Did you forget to use SWAA instruction in the input file?\n");
		return;
	}
	if(wt->Tab2TLen/2 > MAX_WAVETAB_LEN){
		printf("Due to the need of calucating relative Loudness to keep envelop not affected by time-based frequency domain functon.\n");
		printf("using a track that contains more than %d wavetab elements is not allowed\n",MAX_WAVETAB_LEN);
		return;
	}
	// 用前需memset();
	float percent=(float)(arg->wrote_len)/whole_process_len;
	register float sample=0;

	size_t attn_fac_offset_tmpvar[MAX_WAVETAB_LEN+1]={0};
	float Loudness_Factor[MAX_WAVETAB_LEN+1]={0};
	float Loudness_Factor_sum;

	for(unsigned int i=0;i < wt->Tab2TLen;i+=2){
		attn_fac_offset_tmpvar[i/2]=RangeFArrayLookup( (wt->Attn+i/2)->Attntab,percent,(wt->Attn+i/2)->TabLen,4);
	}
	attn_fac_offset_tmpvar[MAX_WAVETAB_LEN]=RangeFArrayLookup(arg->Attn.Attntab,percent,arg->Attn.TabLen,4);

	for(unsigned int i=0;i<arg->pending_len;i++){
		Loudness_Factor_sum=0.0f;
		for(unsigned int j=0;j < wt->Tab2TLen;j+=2){
			percent=(float)(i+arg->wrote_len)/whole_process_len;
			Loudness_Factor[j/2]=loudness_attn_factor(percent,wt->Attn+j/2,attn_fac_offset_tmpvar+j/2);
			Loudness_Factor_sum+=Loudness_Factor[j/2];
		}
		for(unsigned int j=0;j < wt->Tab2TLen;j+=2){
			sample=sin((float)(i+arg->wrote_len)/BACKEND_RATE*2*PI*arg->freq*wt->Tab2T[j] + wt->Tab2T[j+1]);
			sample*=Loudness_Factor[j/2];
			sample*=(Loudness_Factor_sum == 0.0f) ? 0.0f : arg->Loudnessfac*arg->TrackGlobalLoudnessFac/Loudness_Factor_sum;
			sample*=loudness_attn_factor(percent,&arg->Attn,attn_fac_offset_tmpvar+MAX_WAVETAB_LEN);
			Buffer[i]+=sample;
		}
	}
	arg->wrote_len+=arg->pending_len;
}
