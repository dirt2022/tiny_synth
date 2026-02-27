#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/param.h>
#include <sys/types.h>
#include "include/player_memmacro.h"
#include "include/player_strapi.h"
#include "include/player_dataapi.h"
#include "include/player_limit.h"
#include "include/player_wavespec.h"
#include "include/player_backend.h"
#include "include/player_parser.h"
#include "include/player_note.h"

#define GETCHAR(fp,x,inttmpvar) \
	inttmpvar=fgetc(fp); \
	if (inttmpvar == EOF){ \
		printf("Unexcepted EOF"); \
		abort(); \
	} \
	x=inttmpvar;

#define GETCHAR_NINT(fp,x) \
	x=fgetc(fp); \
	if (x == EOF){ \
		printf("Unexcepted EOF"); \
		abort(); \
	}

struct GlobalStatus gs={0};
struct LoudnessLoopTab llpt={0};
struct WaveTab wt[MAX_TRACKNUM]={0};
struct WaveArgs args[MAX_TRACKNUM]={0};
struct Input ipt[MAX_TRACKNUM]={0};

float buffer[BUFFER_LEN]={0};
// 音频输出缓冲区

static const char * Keywords[]={
	"SBPM","STRN","SLLP","SWAA","DATA","STGL","SNAT"
// set bpm, set track num, set loudness loop
// set wave tab and attn factor ,"data section start","set track global loudness"
// set attn table for a single note (envelop)
// While '!' marks the end of a track.
};

// Usage: "SBPM 60.0" "SETR 1" "SLLP 0.0 2.0 0.5 2.0 3.0 0.6... start stop factor"
static int CosumeTrack(FILE * fp,backend_stream_t s){
	memset(buffer,0,sizeof(float)*BUFFER_LEN);
	char fbuffer[MAX_LINELEN]={0};

	int tmpvar=0;
	int writeflag=1;
	int skipcnt=0;

	float time_min=(float)BUFFER_LEN/(float)BACKEND_RATE;
	for(unsigned int i=0; i<gs.tracknum; i++){
		if( args->wrote_len != (unsigned int)(args[i].time*BACKEND_RATE) ){
			time_min=MIN(time_min,(args[i].time*BACKEND_RATE-args[i].wrote_len)/BACKEND_RATE);
		} else {
			if(gs.SectionInputEnd == 1){
				skipcnt++;
				continue;
			}
			fseek(fp,*(int *)(ipt[i].line->data),SEEK_SET);
			for(unsigned int j=0;j < MAX_LINELEN;j++){
				GETCHAR(fp,fbuffer[j],tmpvar);
				if(fbuffer[j] == '\n'){ // only support unix style file.
					fbuffer[j]=0;
					ipt[i].line=ChainTabHeadCosume(ipt[i].line);
					writeflag=0;
					break;
				}
				if(fbuffer[j] == ' '){
					// 移除这个行	来保证 对空格判断无误, 且parse_note已经能够处理空格
					break;
				}
			}

			if(writeflag == 1){
				*(int *)(ipt[i].line->data)+=parse_note(fbuffer,args+i,&gs,&llpt,gs.ticks,MAX_LINELEN);
			} else {
				parse_note(fbuffer,args+i,&gs,&llpt,gs.ticks,MAX_LINELEN);
			}

			if(time_min > args[i].time){
				time_min=args[i].time;
			}
		}
	}
	if(skipcnt == gs.tracknum){
		return 1;
	}
	gs.ticks+=time_min/60*gs.bpm;
	for(unsigned int i=0; i<gs.tracknum; i++){
		args[i].pending_len=time_min*BACKEND_RATE;
		WriteWave(buffer,wt+i,args+i,BUFFER_LEN);
	}
	BackendWrite(s,buffer,time_min*BACKEND_RATE*sizeof(float));
	return 0;
}

static void SeekTrack(FILE * fp,int tracknum){
	if(gs.SectionInputEnd == 0){
		fseek(fp,gs.search_offset,SEEK_SET);
	} else {
		return;
	}
	int ch=0;
	int found_tracknum=0;
	int * data_payload=NULL;
	char fbuffer[MAX_LINELEN]={0};
	int offset=0;

	for(unsigned int i=0; i< gs.tracknum; i++){
		if(ipt[i].line == NULL||ipt[i].line->data == NULL){ //音轨间必须紧密书写, @ 0 ... 下一行就得是 @ 1 .....
			GETCHAR_NINT(fp,ch);
			if(ch == '@'){
				offset=myfgets(fbuffer,MAX_LINELEN,fp);
				SAFE_MALLOC_DEF(data_payload,sizeof(int));
				*data_payload=ftell(fp)-offset+1+next_space_offset(fbuffer+1); // 空格存在偏移,且要跳过一个数字param
				found_tracknum=atoi(fbuffer+1);
				ChainTabAppend(&ipt[found_tracknum].line,data_payload);
				if (found_tracknum == tracknum){
					gs.search_offset=ftell(fp);
					break;
				}
				continue;
			}
			if (ch == '!') { // End entered.
				gs.SectionInputEnd=1;
				break;
			}	
		}
	}
}

static void InitPlayerWithCleanUp(void){
	gs.SectionInputEnd=0;
	for(unsigned int i=0;i < MAX_TRACKNUM; i++){
		if (ipt[i].line != NULL){
			ChainTabDestory(ipt[i].line);
		}
	}
	memset(ipt,0,sizeof(ipt));
	for(unsigned int i=0;i < MAX_TRACKNUM; i++){
		ipt[i].line=ChainTabCreate();
	}
}

void PlayerMain(FILE * fp,backend_stream_t s){ // 此处必在一行之首
	int res=0;
	InitPlayerWithCleanUp();
	gs.search_offset=ftell(fp);
	for(;;){
		for(unsigned int i=0;i<gs.tracknum;i++){
			SeekTrack(fp,i);
		}
		res=CosumeTrack(fp,s);
		if(res==1){
			return;
		}
	}
}
static void ParserDeinit(void){
	for(unsigned int i=0;i < MAX_TRACKNUM;i++){
		if (ipt[i].line != NULL){
			ChainTabDestory(ipt[i].line);
		}
		if (wt[i].Tab2T != NULL){
			SAFE_FREE(wt[i].Tab2T);
		}
		if (wt[i].Attn != NULL){
			for (unsigned int j=0;j<wt[i].Tab2TLen/2;j++){
				if(wt[i].Attn[j].Tab2T != NULL){
					SAFE_FREE(wt[i].Attn[j].Tab2T);
				}
				if(wt[i].Attn[j].Attntab != NULL){
					SAFE_FREE(wt[i].Attn[j].Attntab);
				}
			}
			SAFE_FREE(wt[i].Attn);
		}
		if (llpt.Tab != NULL){
			SAFE_FREE(llpt.Tab);
		}
	}
}
void FileParse(FILE * fp,backend_stream_t s){
	void * tmpptr=NULL;
	unsigned int index=0;
	char fbuffer[MAX_LINELEN]={0};
	int firstparam=0;

	while(!feof(fp)){
		myfgets(fbuffer,MAX_LINELEN,fp);
		printf("string %s\n",fbuffer);
		index=strlookup(fbuffer,Keywords,7);
		switch (index){
			case 0:{
				printf("Setting bpm...\n");
				gs.bpm=atof(fbuffer+5);
				break;
			}
			case 1:{
				printf("Setting tracknum...\n");
				gs.tracknum=atoi(fbuffer+5);
				break;
			}
			case 2:{
				printf("Setting loudness loop tab...\n");
				if (llpt.Tab != NULL){
					SAFE_FREE(llpt.Tab);
				}
				llpt.Tab=str2farray(fbuffer+5,&(llpt.Len));
				break;
			}
			case 3:{ // wt, (\n) factor table, (\n)
				printf("Setting loudness tab and wave tab...\n");
				for(unsigned int i=0;i<gs.tracknum;i++){
					myfgets(fbuffer,MAX_LINELEN,fp);
					wt[i].Tab2T=str2farray(fbuffer,&wt[i].Tab2TLen);
					if (wt[i].Attn != NULL){
						for(unsigned int j=0;j<wt[i].Tab2TLen/2;j++){
							SAFE_FREE(wt[i].Attn[j].Attntab);
							SAFE_FREE(wt[i].Attn[j].Tab2T);
						}
						SAFE_REALLOC_DEF(wt[i].Attn,sizeof(struct AttnTab)*wt[i].Tab2TLen/2,tmpptr);
					} else {
						SAFE_MALLOC_DEF(wt[i].Attn,sizeof(struct AttnTab)*wt[i].Tab2TLen/2);
					}
					for(unsigned int j=0;j<wt[i].Tab2TLen/2;j++){
						wt[i].Attn[j].TabLen=0;
					}
					for(unsigned int j=0;j<wt[i].Tab2TLen/2;j++){
						myfgets(fbuffer,MAX_LINELEN,fp);
						wt[i].Attn[j].Tab2T=str2farray(fbuffer,&wt[i].Attn[j].Tab2TLen);
						CalcLoudnessAttnTab(wt[i].Attn+j);
					}
				}
				break;
			}
			case 4:{
				PlayerMain(fp,s);
				break;
			}
			case 5:{
				printf("Setting TrackFactor\n");
				args[atoi(fbuffer+5)].TrackGlobalLoudnessFac=atof(fbuffer+5+next_letter_offset(fbuffer+5,NULL));
				printf("Set %f\n",args[atoi(fbuffer+5)].TrackGlobalLoudnessFac);
				break;
			}
			case 6:{
				firstparam=atoi(fbuffer+5);
				if (args[firstparam].Attn.Tab2T != NULL){
					SAFE_FREE(args[firstparam].Attn.Tab2T);
				}
				args[firstparam].Attn.Tab2T=str2farray(fbuffer+5+next_space_offset(fbuffer+5) // 格式是: 先写音轨号,在写包络点
				,&args[firstparam].Attn.Tab2TLen);
				CalcLoudnessAttnTab(&args[firstparam].Attn);
				break;
			}
			default:{
				ParserDeinit();
				return;
			}
		}
	}
}