#include "stdlib.h"
#include "forge.h"
#include "imagine.h"

size_t i_shared_memory_allocate(size_t size)
{
	
}


void *i_shared_memory_get_pointer(size_t handle)
{
	
}

void i_shared_memory_free(size_t handle)
{
	
}


uint64 i_shared_memory_user_id(size_t handle)
{
	
}


void i_shared_memory_user_add(size_t handle, uint64 user_id)
{
	
}
/*
	
	// separate external and internal store

	login/out
	User create/delete.

	Create.
		non locking
			launcing checksuming, networking, signing, clean up and indexing threads
	Update
		Re sunc record.

	search
		Slow. wait for responce.	
	lookup
		client side.

	Alloc
		On request
		Predictive
	Cashe
		on request
		Predict from lookup/Search.
	UnCase
		FIFO





*/