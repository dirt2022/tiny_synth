#include <stdio.h>
#include <stdlib.h>
#include <pulse/simple.h>
#include "include/player_strapi.h"
#include "include/player_limit.h"
#include "include/player_backend.h"
#include "include/player_dataapi.h"
#include "include/player_parser.h"

static const char * backendlist[]={
	"pa","file"
};

int main(int argc,char * argv[]){
	// args: ./prog backend_type inputfile
	if (argc != 3){
		printf("Usage: %s <Backend type> <Input File>\n",argv[0]);
		return -1;
	}

	enum backend_type type=PulseAudio;
	type=strlookup(argv[1],(const char **)backendlist,2);
	backend_stream_t stream=backend_init(type);
	FILE * fp=fopen(argv[2],"rb");
	FileParse(fp,stream);
	backend_deinit(type,stream);
}
