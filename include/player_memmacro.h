#define SAFE_MALLOC_DEF(ptr,size) \
	ptr=malloc(size); \
	if (ptr == NULL){ \
		abort(); \
	}

#define SAFE_REALLOC_DEF(ptr,size,tmpptr) \
	tmpptr=realloc(ptr,size); \
	if (tmpptr == NULL){ \
		free(ptr); \
		abort(); \
	} else { \
		ptr=tmpptr; \
	}

#define SAFE_MALLOC_CATCH(ptr,size,func,arg) \
	ptr=malloc(size); \
	if (ptr == NULL){ \
		func(arg); \
	}

#define SAFE_REALLOC_CATCH(ptr,size,tmpptr,func,arg) \
	tmpptr=realloc(ptr,size); \
	if (tmpptr == NULL){ \
		free(ptr); \
		func(arg); \
	} else { \
		ptr=tmpptr; \
	}

#define NFREE_SAFE_REALLOC_CATCH(ptr,size,tmpptr,func,arg) \
	tmpptr=realloc(ptr,size); \
	if (tmpptr == NULL){ \
		func(arg); \
	} else { \
		ptr=tmpptr; \
	}

#define SAFE_FREE(ptr) \
	free(ptr); \
	ptr=NULL;