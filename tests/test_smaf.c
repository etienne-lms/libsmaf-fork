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
#include <sys/mman.h>

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
		return;
	}

	data = mmap(NULL, LENGTH, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (data == MAP_FAILED) {
		printf("%s: mmap failed\n", __func__);
		goto end;
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
		return;
	}

	ret = smaf_set_secure(fd, 1);
	if (ret) {
		printf("%s: smaf_set_secure() failed %d\n", __func__, ret);
		goto end;
	}

	data = mmap(NULL, LENGTH, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (data == MAP_FAILED) {
		printf("%s: mmap failed\n", __func__);
		goto end;
	}

	munmap(data, LENGTH);
	printf("%s: successed\n", __func__);
end:
	close(fd);
}

int main (void)
{
	if (smaf_open()) {
		printf("Can't open /dev/smaf\n");
		return 0;
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

	smaf_close();
	return 0;
}
