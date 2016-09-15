/*
 * Copyright (c) 2016, Linaro Limited
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <err.h>
#include <stdio.h>
#include <string.h>
#include <tee_client_api.h>

#include "optee_invoke_sdp_ta.h"

#define TA_SDP_TEST_UUID	{ 0x12345678, 0x5b69, 0x11e4, \
				{ 0x9d, 0xbb, 0x10, 0x1f, 0x74, 0xf0, 0x00, 0x01 } }

void finalize_tee_ctx(struct tee_ctx *ctx)
{
	TEEC_CloseSession(&ctx->sess);
	TEEC_FinalizeContext(&ctx->ctx);
}

int create_tee_ctx(struct tee_ctx *ctx)
{
	TEEC_Result teerc;
	TEEC_UUID uuid = TA_SDP_TEST_UUID;
	uint32_t err_origin;

	teerc = TEEC_InitializeContext(NULL, &ctx->ctx);
	if (teerc != TEEC_SUCCESS)
		return -1;

	/*
	 * Open a session to the "hello world" TA, the TA will print "hello
	 * world!" in the log when the session is created.
	 */
	teerc = TEEC_OpenSession(&ctx->ctx, &ctx->sess, &uuid,
			       TEEC_LOGIN_PUBLIC, NULL, NULL, &err_origin);
	if (teerc != TEEC_SUCCESS)
		printf("Error in %s: open session to sdp test TA failed %x %d\n",
			__func__, teerc, err_origin);

	return (teerc == TEEC_SUCCESS) ? 0 : -1;
}

int inject_sdp_data(struct tee_ctx *ctx, void *in, size_t sz_in, struct sec_buf *sbuf)
{
	TEEC_Result teerc;
	TEEC_Operation op;
	uint32_t err_origin;
	TEEC_SharedMemory shm;

	memset(&op, 0, sizeof(op));
	op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_SECURE,TEEC_MEMREF_TEMP_INPUT,
					 TEEC_NONE, TEEC_NONE);

	op.params[1].tmpref.buffer = in;
	op.params[1].tmpref.size = sz_in;

	memset(&shm, 0, sizeof(shm));
	shm.id = sbuf->ref;
	shm.flags = TEEC_MEM_SECURE;
	shm.size = sbuf->size;
	op.params[0].memref.parent = &shm;
	op.params[0].memref.size = sbuf->size;
	op.params[0].memref.offset = sbuf->offset;

	teerc = TEEC_InvokeCommand(&ctx->sess, 0, &op, &err_origin);
	if (teerc != TEEC_SUCCESS)
		printf("Error in %s: invoke sdp test TA failed %x %d\n",
			__func__, teerc, err_origin);

	return (teerc == TEEC_SUCCESS) ? 0 : -1;
}

int transform_sdp_data(struct tee_ctx *ctx, struct sec_buf *sbuf)
{
	TEEC_Result teerc;
	TEEC_Operation op;
	uint32_t err_origin;
	TEEC_SharedMemory shm;

	memset(&op, 0, sizeof(op));
	op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_SECURE,
					 TEEC_NONE, TEEC_NONE, TEEC_NONE);

	memset(&shm, 0, sizeof(shm));
	shm.id = sbuf->ref;
	shm.flags = TEEC_MEM_SECURE;
	shm.size = sbuf->size;
	op.params[0].memref.parent = &shm;
	op.params[0].memref.size = sbuf->size;
	op.params[0].memref.offset = sbuf->offset;

	teerc = TEEC_InvokeCommand(&ctx->sess, 0, &op, &err_origin);
	if (teerc != TEEC_SUCCESS)
		printf("Error in %s: invoke sdp test TA failed %x %d\n",
			__func__, teerc, err_origin);

	return (teerc == TEEC_SUCCESS) ? 0 : -1;
}

int dump_sdp_data(struct tee_ctx *ctx, void *out, size_t sz_out, struct sec_buf *sbuf)
{
	TEEC_Result teerc;
	TEEC_Operation op;
	uint32_t err_origin;
	TEEC_SharedMemory shm;

	memset(&op, 0, sizeof(op));
	op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_SECURE,
					 TEEC_MEMREF_TEMP_OUTPUT,
					 TEEC_NONE, TEEC_NONE);

	memset(&shm, 0, sizeof(shm));
	shm.id = sbuf->ref;
	shm.flags = TEEC_MEM_SECURE;
	shm.size = sbuf->size;
	op.params[0].memref.parent = &shm;
	op.params[0].memref.size = sbuf->size;
	op.params[0].memref.offset = sbuf->offset;

	op.params[1].tmpref.buffer = out;
	op.params[1].tmpref.size = sz_out;

	teerc = TEEC_InvokeCommand(&ctx->sess, 0, &op, &err_origin);
	if (teerc != TEEC_SUCCESS)
		printf("Error in %s: invoke sdp test TA failed %x %d\n",
			__func__, teerc, err_origin);

	return (teerc == TEEC_SUCCESS) ? 0 : -1;
}
