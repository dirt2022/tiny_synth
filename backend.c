#include <pulse/sample.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <pulse/simple.h>
#include <sys/types.h>
#include "include/player_backend.h"
#include "include/player_limit.h"

#define SWAP32BIT(x) \
	x=( ((unsigned int)x)<<16)|( ((unsigned int)x)>>16)

// Note: All backends uses signed float32 (little endian). which is not limit.
static union {
	unsigned int a;
	unsigned char b;
} be_check;

static int is_be(void){
	be_check.a=1;
	if (be_check.b == 0){
		return 1;
	}
	return 0;
}

static int is_be_cpu;

static void buffer_be2le(float * buffer,int size){ // used only on be platform
	for(unsigned int i=0;i<size/sizeof(float);i++){
		SWAP32BIT(buffer[i]);
	}
}

static struct pa_simple * PulseAudio_Init(){ // the only backend.
	pa_sample_spec spec;
	spec.format = PA_SAMPLE_FLOAT32LE;
	spec.channels = 1;
	spec.rate = BACKEND_RATE;
	return pa_simple_new(NULL,"MusicPlayer",PA_STREAM_PLAYBACK,
		NULL,"Player",&spec,NULL,NULL,NULL);
}

int PulseAudio_Write(backend_stream_t s,void * buffer,size_t size){
	int ret=0;
	if (is_be_cpu==1){
		buffer_be2le(buffer,size);
	}
	ret=pa_simple_write((struct pa_simple *)s,buffer,size,NULL);
	if (ret == 0){
		return size;
	}
	return -1;
}

int File_Write(backend_stream_t s,void * buffer,size_t size){
	if (is_be_cpu==1){
		buffer_be2le(buffer,size);
	}
	int res=0;
	res=fwrite(buffer,sizeof(float),size/sizeof(float),(FILE *)s);
	return res*sizeof(float);
}


int(* BackendWrite)(backend_stream_t s,void * buffer,size_t size)=NULL;

backend_stream_t backend_init(enum backend_type type){ //thread unsafe
	static int inited=0;
	backend_stream_t stream=NULL;
	is_be_cpu=is_be();
	if (inited == 1){
		printf("Fatal: Backend should not be inited twice.\n");
		abort();
	}

	switch (type){
		case PulseAudio:{
			stream = (void *)PulseAudio_Init();
			if (stream == NULL){
				printf("Fatal: Could not init the stream\n");
				abort();
			}
			BackendWrite=PulseAudio_Write;
			inited=1;
			return stream;
		}
		case File:{
			if (getenv("PLAYER_OUTPUT") == NULL){
				stream = (backend_stream_t)fopen("./output.pcm","w+");
				if (stream == NULL){
					printf("Fatal: Could not init the stream\n");
					abort();
				}
				BackendWrite=File_Write;
				return stream;
			} else {
				stream = fopen(getenv("PLAYER_OUTPUT"),"w+");
				if (stream == NULL){
					printf("Fatal: Could not init the stream\n");
					abort();
				}
				BackendWrite=File_Write;
				return stream;
			}
		}
	}
}

void backend_deinit(enum backend_type type,backend_stream_t stream){
	BackendWrite=NULL;
	switch (type){
		case PulseAudio:{
			pa_simple_drain(stream,NULL);
			pa_simple_free(stream);
			return;
		}
		case File:{
			fclose(stream);
		}
	}
}
