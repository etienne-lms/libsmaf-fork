/*
 * test_smaf.c
 *
 * Copyright (C) Linaro SA 2015
 * Author: Benjamin Gaignard <benjamin.gaignard@linaro.org> for Linaro.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Suite 500, Boston, MA  02110-1335  USA
 */

#include <../lib/libsmaf.h>
#include <../lib/smaf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

#include "optee_invoke_sdp_ta.h"

#define LENGTH 1024*16

static void test_create_named(char *name)
{
	int ret;
	int fd;

	ret = smaf_create_buffer(LENGTH, SMAF_CLOEXEC | SMAF_RDWR, name, &fd);

	if (ret || (fd == -1)) {
		printf("%s: smaf_create_buffer() failed %d\n", __func__, ret);
		return;
	}

	printf("%s: smaf_create_buffer() for allocator %s successed\n", __func__, name);

	close(fd);
}

static void test_create_named_invalid(void)
{
	int ret;
	int fd;

	ret = smaf_create_buffer(LENGTH, SMAF_CLOEXEC | SMAF_RDWR, "deadbeef", &fd);

	if (!ret) {
		printf("%s: smaf_create_buffer() failed %d\n", __func__, ret);
		return;
	}

	printf("%s: successed\n", __func__);
}

static void test_iter_over_allocators(void)
{
	int i;
	int count = smaf_allocator_count();
	char *name;

	if (count <= 0) {
		printf("%s: smaf_allocator_count() failed %d\n", __func__, count);
		return;
	}

	printf("%s: smaf_allocator_count() found %d allocators\n", __func__, count);
	for (i = 0; i < count; i++) {
		name = smaf_get_allocator_name(i);
		if (!name) {
			printf("%s: smaf_get_allocator_name() failed %d\n", __func__, i);
			return;
		}

		test_create_named(name);
		free(name);
	}
}

static void test_invalid_allocator_index(void)
{
	int count = smaf_allocator_count();
	char *name;

	if (count <= 0) {
		printf("%s: smaf_allocator_count() failed %d\n", __func__, count);
		return;
	}

	printf("%s: smaf_allocator_count() found %d allocators\n", __func__, count);
	name = smaf_get_allocator_name(count);
	if (name) {
		printf("%s: smaf_get_allocator_name() failed %s\n", __func__, name);
		free(name);
		return;
	}

	printf("%s: successed\n", __func__);
}


static void test_secure(void)
{
	int ret;
	int fd;

	ret = smaf_create_buffer(LENGTH, SMAF_CLOEXEC | SMAF_RDWR, NULL, &fd);

	if (ret || (fd == -1)) {
		printf("%s: smaf_create_buffer() failed %d\n", __func__, ret);
		return;
	}

	ret = smaf_set_secure(fd, 1);
	if (ret) {
		printf("%s: smaf_set_secure() failed %d\n", __func__, ret);
		goto end;
	}

	ret = smaf_get_secure(fd);
	if (!ret) {
		printf("%s: smaf_get_secure() failed %d\n", __func__, ret);
		goto end;
	}

	printf("%s: successed\n", __func__);
end:
	close(fd);
}

static void test_create_non_page_aligned_mmap(void)
{
	int ret;
	int fd;
	void *data;

	ret = smaf_create_buffer(LENGTH+1, SMAF_CLOEXEC | SMAF_RDWR, NULL, &fd);

	if (ret || (fd == -1)) {
		printf("%s smaf_create_buffer() failed %d\n", __func__, ret);
		return;
	}

	data = mmap(0, LENGTH+1, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	munmap(data, LENGTH+1);
	close(fd);

	if (data == MAP_FAILED)
		printf("%s failed\n", __func__);
	else
		printf("%s successed\n", __func__);
}

static void test_create_non_page_aligned(void)
{
	int ret;
	int fd;

	ret = smaf_create_buffer(LENGTH+1, SMAF_CLOEXEC | SMAF_RDWR, NULL, &fd);

	if (ret || (fd == -1)) {
		printf("%s smaf_create_buffer() failed %d\n", __func__, ret);
		return;
	}

	close(fd);
	printf("%s successed\n", __func__);
}

static void test_create_unnamed(void)
{
	test_create_named(NULL);
}

static void test_mmap(void)
{
	int ret, fd;
	char *data;

	ret = smaf_create_buffer(LENGTH, SMAF_CLOEXEC | SMAF_RDWR, NULL, &fd);

	if (ret || (fd == -1)) {
		printf("%s: smaf_create_buffer() failed %d\n", __func__, ret);
		exit(1);
	}

	data = mmap(NULL, LENGTH, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (data == MAP_FAILED) {
		printf("%s: mmap failed\n", __func__);
		exit(1);
	}

	munmap(data, LENGTH);
	printf("%s: successed\n", __func__);
end:
	close(fd);
}

static void test_mmap_secure(void)
{
	int ret, fd;
	char *data;

	ret = smaf_create_buffer(LENGTH, SMAF_CLOEXEC | SMAF_RDWR, NULL, &fd);

	if (ret || (fd == -1)) {
		printf("%s: smaf_create_buffer() failed %d\n", __func__, ret);
		exit(1);
	}

	ret = smaf_set_secure(fd, 1);
	if (ret) {
		printf("%s: smaf_set_secure() failed %d\n", __func__, ret);
		exit(1);
	}

	data = mmap(NULL, LENGTH, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (data != MAP_FAILED) {
		printf("%s: mmap succeed but shall fail !!!\n", __func__);
		exit(1);
	}
	printf("%s: successed\n", __func__);
end:
	close(fd);
}

static int test_secure_data_path(void)
{
	struct tee_ctx ctx;
	unsigned char *test_buf;
	unsigned char *ref_buf;
	size_t len;
	int rc;
	int fd;
	struct sec_buf sbuf;

	len = 16 * 1024;
	test_buf = malloc(len);
	ref_buf = malloc(len);
	if (!test_buf || !ref_buf)
		return -1;

	fd = open("/dev/urandom", O_RDONLY);
	if (fd < 0)
		return -1;
	rc = read(fd, ref_buf, len);
	if (rc != len) {
		printf("failed to read %d bytes from /dev/urandom\n", len);
		return -1;
	}
	memcpy(test_buf, ref_buf, len);

	memset(&sbuf, 0, sizeof(sbuf));
	sbuf.size = len;

	rc = smaf_create_buffer(len, SMAF_CLOEXEC | SMAF_RDWR, "smaf-optee", &sbuf.ref);
	if (rc || sbuf.ref < 0) {
		printf("smaf_create_buffer() failed %d, d\n", __func__, rc, sbuf.ref);
		return -1;
	}
	rc = smaf_set_secure(sbuf.ref, 1);
	if (rc) {
		printf("%s: smaf_set_secure() failed %d\n", __func__, rc);
		return -1;
	}

	if (create_tee_ctx(&ctx))
		return -1;

	rc = smaf_set_secure(sbuf.ref, 1);
	if (rc) {
		printf("%s: smaf_set_secure() failed %d\n", __func__, rc);
		return -1;
	}

	if (inject_sdp_data(&ctx, test_buf, len, &sbuf))
		return -1;

	if (transform_sdp_data(&ctx, &sbuf))
		return -1;

	if (dump_sdp_data(&ctx, test_buf, len, &sbuf))
		return -1;

	while(len--) {
		if (test_buf[len] != (unsigned char)(~ref_buf[len] + 1)) {
			printf("Unexpected content found\n");
			return -1;
		}
	}

	close(sbuf.ref);
	finalize_tee_ctx(&ctx);
	printf("%s: successed\n", __func__);

	return 0;
}


int main (void)
{
	int err = 0;

	if (smaf_open()) {
		printf("Can't open /dev/smaf\n");
		return -1;
	}

	test_create_unnamed();

	test_iter_over_allocators();
	test_invalid_allocator_index();
	test_create_named_invalid();
	test_create_non_page_aligned();
	test_create_non_page_aligned_mmap();
	test_secure();

	test_mmap();
	test_mmap_secure();

	if (test_secure_data_path()) {
		printf("error found\n");
		err++;
	}

	smaf_close();

	return err ? -1 : 0;
}
