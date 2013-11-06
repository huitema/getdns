/**
 *
 * /brief type declarations private to the getdns library
 *
 * These type declarations are not meant to be used by applications calling
 * the public library functions.
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

#ifndef TYPES_INTERNAL_H_
#define TYPES_INTERNAL_H_

#include "context.h"
#include <ldns/ldns.h>

/* declarations */
struct getdns_dns_req;
struct getdns_network_req;
struct ub_ctx;
struct event;
struct event_base;

typedef enum network_req_state_enum
{
	NET_REQ_NOT_SENT,
	NET_REQ_IN_FLIGHT,
	NET_REQ_FINISHED,
	NET_REQ_CANCELED
} network_req_state;

/**
 * structure used by validate_extensions() to check extension formats
 */
typedef struct getdns_extension_format
{
	char *extstring;
	getdns_data_type exttype;
} getdns_extension_format;

/**
 * Request data for unbound
 **/
typedef struct getdns_network_req
{
	/* the async_id from unbound */
	int unbound_id;
	/* state var */
	network_req_state state;
	/* owner request (contains name) */
	struct getdns_dns_req *owner;

	/* request type */
	uint16_t request_type;

	/* request class */
	uint16_t request_class;

	/* result */
	ldns_pkt *result;

	/* next request to issue after this one */
	struct getdns_network_req *next;
} getdns_network_req;

/**
 * dns request - manages a number of network requests and
 * the initial data passed to getdns_general
 */
typedef struct getdns_dns_req
{

	/* name */
	char *name;

	/* canceled flag */
	int canceled;

	/* current network request */
	struct getdns_network_req *current_req;

	/* first request in list */
	struct getdns_network_req *first_req;

	/* request timeout event */
	struct event *timeout;

	/* local callback timer */
	struct event *local_cb_timer;

	/* event base this req is scheduled on */
	struct event_base *ev_base;

	/* context that owns the request */
	getdns_context_t context;

	/* ub_ctx issuing the request */
	struct ub_ctx *unbound;

	/* request extensions */
	getdns_dict *extensions;

	/* callback data */
	getdns_callback_t user_callback;
	void *user_pointer;

	/* the transaction id */
	getdns_transaction_t trans_id;

} getdns_dns_req;

/* utility methods */

/* network request utilities */
void network_req_free(getdns_network_req * net_req);

getdns_network_req *network_req_new(getdns_dns_req * owner,
    uint16_t request_type,
    uint16_t request_class, struct getdns_dict *extensions);

/* dns request utils */
getdns_dns_req *dns_req_new(getdns_context_t context,
    struct ub_ctx *unbound,
    const char *name, uint16_t request_type, struct getdns_dict *extensions);

void dns_req_free(getdns_dns_req * req);

#endif

/* types-internal.h */