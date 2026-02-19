#include <pulse/sample.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <pulse/simple.h>
#include <sys/types.h>
#include "include/player_backend.h"
#include "include/player_limit.h"

// Note: All backends uses signed float32 (little endian). which is not limit.

static struct pa_simple * PulseAudio_Init(){ // the only backend.
	pa_sample_spec spec;
	spec.format = PA_SAMPLE_FLOAT32LE;
	spec.channels = 1;
	spec.rate = BACKEND_RATE;
	return pa_simple_new(NULL,"MusicPlayer",PA_STREAM_PLAYBACK,
		NULL,"Player",&spec,NULL,NULL,NULL);
}

int PulseAudio_Write(backend_stream_t s,void * buffer,size_t size){
	return pa_simple_write((struct pa_simple *)s,buffer,size,NULL);
}

int File_Write(backend_stream_t s,void * buffer,size_t size){
	int res=0;
	for(size_t i=0;i < size;i++){
		res=fputc(((char *)buffer)[i],s);
		if (res == EOF){
			return -1;
		}
	}
	return size;
}


int(* BackendWrite)(backend_stream_t s,void * buffer,size_t size)=NULL;

backend_stream_t backend_init(enum backend_type type){ //thread unsafe
	static int inited=0;
	backend_stream_t stream=NULL;
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