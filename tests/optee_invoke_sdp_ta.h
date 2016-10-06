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

#ifndef INVOKE_SDP_TA_H
#define INVOKE_SDP_TA_H

#include <tee_client_api.h>

struct tee_ctx {
	TEEC_Context ctx;
	TEEC_Session sess;
};

struct sec_buf {
	int fd;
	void *teeref;
	size_t offset;
	size_t size;
};

void finalize_tee_ctx(struct tee_ctx *ctx);

int create_tee_ctx(struct tee_ctx *ctx);

int tee_deregister_buffer(struct tee_ctx *ctx, void *shm_ref);

int tee_deregister_buffer(struct tee_ctx *ctx, void *shm_ref);

int inject_sdp_data(struct tee_ctx *ctx, void *in, size_t offset, size_t sz_in, void *sbuf);

int transform_sdp_data(struct tee_ctx *ctx, size_t offset, size_t len, void *sbuf);

int dump_sdp_data(struct tee_ctx *ctx, void *out, size_t offset, size_t sz_out, void *sbuf);

#endif /* INVOKE_SDP_TA_H */
