/**
 *
 * /brief getdns list management functions
 * 
 * This is the meat of the API
 * Originally taken from the getdns API description pseudo implementation.
 *
 */

/*
 * Copyright (c) 2013, Versign, Inc.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * * Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 * * Neither the name of the <organization> nor the
 *   names of its contributors may be used to endorse or promote products
 *   derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Verisign, Inc. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <string.h>
#include "list.h"

/*---------------------------------------- getdns_list_get_length */
getdns_return_t
getdns_list_get_length(struct getdns_list * list, size_t * answer)
{
	int retval = GETDNS_RETURN_NO_SUCH_LIST_ITEM;

	if (list != NULL && answer != NULL) {
		retval = GETDNS_RETURN_GOOD;
		*answer = list->numinuse;
	}

	return retval;
}				/* getdns_list_get_length */

/*---------------------------------------- getdns_list_get_data_type */
getdns_return_t
getdns_list_get_data_type(struct getdns_list * list, size_t index,
    getdns_data_type * answer)
{
	getdns_return_t retval = GETDNS_RETURN_NO_SUCH_LIST_ITEM;

	if (list != NULL && index < list->numinuse) {
		*answer = list->items[index].dtype;
		retval = GETDNS_RETURN_GOOD;
	}
	return retval;
}				/* getdns_list_get_data_type */

/*---------------------------------------- getdns_list_get_dict */
getdns_return_t
getdns_list_get_dict(struct getdns_list * list, size_t index,
    struct getdns_dict ** answer)
{
	getdns_return_t retval = GETDNS_RETURN_NO_SUCH_LIST_ITEM;

	if (list != NULL && index < list->numinuse) {
		if (list->items[index].dtype != t_dict)
			retval = GETDNS_RETURN_WRONG_TYPE_REQUESTED;
		else {
			*answer = list->items[index].data.dict;
			retval = GETDNS_RETURN_GOOD;
		}
	}

	return retval;
}				/* getdns_list_get_dict */

/*---------------------------------------- getdns_list_get_list */
getdns_return_t
getdns_list_get_list(struct getdns_list * list, size_t index,
    struct getdns_list ** answer)
{
	getdns_return_t retval = GETDNS_RETURN_NO_SUCH_LIST_ITEM;

	if (list != NULL && index < list->numinuse) {
		if (list->items[index].dtype != t_list)
			retval = GETDNS_RETURN_WRONG_TYPE_REQUESTED;
		else {
			*answer = list->items[index].data.list;
			retval = GETDNS_RETURN_GOOD;
		}
	}

	return retval;
}				/* getdns_list_get_list */

/*---------------------------------------- getdns_list_get_bindata */
getdns_return_t
getdns_list_get_bindata(struct getdns_list * list, size_t index,
    struct getdns_bindata ** answer)
{
	getdns_return_t retval = GETDNS_RETURN_NO_SUCH_LIST_ITEM;

	if (list != NULL && index < list->numinuse) {
		if (list->items[index].dtype != t_bindata)
			retval = GETDNS_RETURN_WRONG_TYPE_REQUESTED;
		else {
			*answer = list->items[index].data.bindata;
			retval = GETDNS_RETURN_GOOD;
		}
	}

	return retval;
}				/* getdns_list_get_bindata */

/*---------------------------------------- getdns_list_get_int */
getdns_return_t
getdns_list_get_int(struct getdns_list * list, size_t index, uint32_t * answer)
{
	getdns_return_t retval = GETDNS_RETURN_NO_SUCH_LIST_ITEM;

	if (list != NULL && index < list->numinuse) {
		if (list->items[index].dtype != t_int)
			retval = GETDNS_RETURN_WRONG_TYPE_REQUESTED;
		else {
			*answer = list->items[index].data.n;
			retval = GETDNS_RETURN_GOOD;
		}
	}

	return retval;
}				/* getdns_list_get_int */

/*---------------------------------------- getdns_list_realloc */
/**
  * private function (API users should not be calling this)
  * allocates a block of items, should be called when a list needs to grow
  * preserves the existing items
  * in case of an error the list should be considered unusable
  * @return GETDNS_RETURN_GOOD on success, GETDNS_RETURN_GENERIC_ERROR if out of memory
  */
getdns_return_t
getdns_list_realloc(struct getdns_list * list)
{
	getdns_return_t retval = GETDNS_RETURN_GENERIC_ERROR;
	int i;
	struct getdns_list_item *newlist;

	if (list != NULL) {
		newlist =
		    (struct getdns_list_item *) realloc(list->items,
		    (list->numalloc +
			GETDNS_LIST_BLOCKSZ) *
		    sizeof(struct getdns_list_item));
		if (newlist != NULL) {
			list->items = newlist;
			for (i = list->numalloc;
			    i < list->numalloc + GETDNS_LIST_BLOCKSZ; i++) {
				list->items[i].inuse = false;
				list->items[i].dtype = t_invalid;
			}
			list->numalloc += GETDNS_LIST_BLOCKSZ;
			retval = GETDNS_RETURN_GOOD;
		}
	}

	return retval;
}				/* getdns_list_realloc */

/*---------------------------------------- getdns_list_copy */
getdns_return_t
getdns_list_copy(struct getdns_list * srclist, struct getdns_list ** dstlist)
{
	int i;
	size_t index;
	getdns_return_t retval = GETDNS_RETURN_NO_SUCH_LIST_ITEM;

	if (srclist != NULL && dstlist != NULL) {
		*dstlist = getdns_list_create();
		if (*dstlist != NULL) {
			retval = GETDNS_RETURN_GOOD;
			for (i = 0; i < srclist->numinuse; i++) {
				if (getdns_list_add_item(*dstlist,
					&index) == GETDNS_RETURN_GOOD) {
					(*dstlist)->items[index].inuse = true;
					(*dstlist)->items[index].dtype =
					    srclist->items[i].dtype;

					if (srclist->items[i].dtype == t_int)
						(*dstlist)->items[index].data.
						    n =
						    srclist->items[i].data.n;
					else if (srclist->items[i].dtype ==
					    t_list)
						retval =
						    getdns_list_copy(srclist->
						    items[index].data.list,
						    &((*dstlist)->items[i].
							data.list));
					else if (srclist->items[i].dtype ==
					    t_bindata) {
						(*dstlist)->items[i].data.
						    bindata =
						    (struct getdns_bindata *)
						    malloc(sizeof
						    (getdns_bindata));
						(*dstlist)->items[i].data.
						    bindata->size =
						    srclist->items[i].data.
						    bindata->size;
						(*dstlist)->items[i].data.
						    bindata->data = (uint8_t *)
						    malloc(srclist->items[i].
						    data.bindata->size);
						if ((*dstlist)->items[i].data.
						    bindata->data != NULL)
							memcpy((*dstlist)->
							    items[i].data.
							    bindata->data,
							    srclist->items[i].
							    data.bindata->data,
							    srclist->items[i].
							    data.bindata->
							    size);
						else
							retval =
							    GETDNS_RETURN_GENERIC_ERROR;
					} else if (srclist->items[i].dtype ==
					    t_dict) {
						retval =
						    getdns_dict_copy(srclist->
						    items[index].data.dict,
						    &((*dstlist)->items[i].
							data.dict));
					}
				} else {
					retval = GETDNS_RETURN_GENERIC_ERROR;
					getdns_list_destroy(*dstlist);
					*dstlist = NULL;
				}

				if (retval != GETDNS_RETURN_GOOD)
					break;
			}
		} else
			retval = GETDNS_RETURN_GENERIC_ERROR;
	}

	return retval;
}				/* getdns_list_copy */

/*---------------------------------------- getdns_list_create */
struct getdns_list *
getdns_list_create()
{
	struct getdns_list *list = NULL;

	list = (struct getdns_list *) malloc(sizeof(struct getdns_list));
	if (list != NULL) {
		list->numalloc = 0;
		list->numinuse = 0;
		list->items = NULL;

		getdns_list_realloc(list);
	}

	return list;
}				/* getdns_list_create */

/*---------------------------------------- getdns_list_destroy */
void
getdns_list_destroy(struct getdns_list *list)
{
	int i;

	if (list != NULL) {
		if (list->items != NULL) {
			for (i = 0; i < list->numinuse; i++) {
				if (list->items[i].dtype == t_list) {
					if (list->items[i].dtype == t_list)
						getdns_list_destroy(list->
						    items[i].data.list);
				} else if (list->items[i].dtype == t_bindata) {
					if (list->items[i].data.bindata->size >
					    0)
						free(list->items[i].data.
						    bindata->data);
					free(list->items[i].data.bindata);
				} else if (list->items[i].dtype == t_dict) {
					getdns_dict_destroy(list->items[i].
					    data.dict);
				}
			}
			free(list->items);
		}
		free(list);
	}
}				/* getdns_list_destroy */

/*---------------------------------------- getdns_list_add_item */
getdns_return_t
getdns_list_add_item(struct getdns_list *list, size_t * index)
{
	getdns_return_t retval = GETDNS_RETURN_NO_SUCH_LIST_ITEM;
	if (list != NULL && index != NULL) {
		if (list->numalloc == list->numinuse)
			retval = getdns_list_realloc(list);
		else
			retval = GETDNS_RETURN_GOOD;

		if (retval == GETDNS_RETURN_GOOD) {
			*index = list->numinuse;
			list->items[*index].inuse = true;
			list->numinuse++;
		}
	}
	return retval;
}				/* getdns_list_add_item */

/*---------------------------------------- getdns_list_set_dict */
getdns_return_t
getdns_list_set_dict(struct getdns_list * list, size_t index,
    struct getdns_dict * child_dict)
{
	getdns_return_t retval = GETDNS_RETURN_NO_SUCH_LIST_ITEM;

	if (list != NULL && child_dict != NULL) {
		if (list->numinuse > index) {
			list->items[index].dtype = t_dict;
			retval =
			    getdns_dict_copy(child_dict,
			    &(list->items[index].data.dict));
		}
	}

	return retval;
}				/* getdns_list_set_dict */

/*---------------------------------------- getdns_set_list */
getdns_return_t
getdns_list_set_list(struct getdns_list * list, size_t index,
    struct getdns_list * child_list)
{
	getdns_return_t retval = GETDNS_RETURN_NO_SUCH_LIST_ITEM;

	if (list != NULL && child_list != NULL) {
		if (list->numinuse > index) {
			list->items[index].dtype = t_list;
			retval =
			    getdns_list_copy(child_list,
			    &(list->items[index].data.list));
		}
	}

	return retval;
}				/* getdns_set_list */

/*---------------------------------------- getdns_list_set_bindata */
getdns_return_t
getdns_list_set_bindata(struct getdns_list * list, size_t index,
    struct getdns_bindata * child_bindata)
{
	getdns_return_t retval = GETDNS_RETURN_NO_SUCH_LIST_ITEM;

	if (list != NULL && child_bindata != NULL) {
		if (list->numinuse > index) {
			list->items[index].dtype = t_bindata;
			list->items[index].data.bindata =
			    (struct getdns_bindata *)
			    malloc(sizeof(struct getdns_bindata));
			if (list->items[index].data.bindata != NULL) {
				list->items[index].data.bindata->size =
				    child_bindata->size;
				list->items[index].data.bindata->data =
				    (uint8_t *) malloc(child_bindata->size *
				    sizeof(uint8_t));
				memcpy(list->items[index].data.bindata->data,
				    child_bindata->data, child_bindata->size);
				retval = GETDNS_RETURN_GOOD;
			} else
				retval = GETDNS_RETURN_GENERIC_ERROR;
		}
	}

	return retval;
}				/* getdns_list_set_bindata */

/*---------------------------------------- getdns_list_set_int */
getdns_return_t
getdns_list_set_int(struct getdns_list * list, size_t index,
    uint32_t child_uint32)
{
	getdns_return_t retval = GETDNS_RETURN_NO_SUCH_LIST_ITEM;

	if (list != NULL) {
		if (list->numinuse > index) {
			list->items[index].dtype = t_int;
			list->items[index].data.n = child_uint32;
			retval = GETDNS_RETURN_GOOD;
		}
	}

	return retval;
}				/* getdns_list_set_int */

/* getdns_list.c */