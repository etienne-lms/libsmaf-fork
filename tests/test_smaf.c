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

static unsigned int test_create_named(char *name)
{
	int ret;
	int fd;

	ret = smaf_create_buffer(LENGTH, SMAF_CLOEXEC | SMAF_RDWR, name, &fd);
	if (ret || fd < 0) {
		printf("%s: smaf_create_buffer(\"%s\") failed %d %d\n",
			__func__, name ? name : "null", ret, fd);
		return 1;
	}
	printf("%s: \"%s\" succeeded\n", __func__, name ? name : "null", ret);
	close(fd);
	return 0;
}

static unsigned int test_create_named_invalid(void)
{
	int ret;
	int fd;

	ret = smaf_create_buffer(LENGTH, SMAF_CLOEXEC | SMAF_RDWR, "deadbeef", &fd);
	if (!ret && fd >= 0) {
		close(fd);
		printf("%s: invalid smaf_create_buffer() did not fail: %d/%d\n",
			__func__, ret, fd);
		return 1;
	}
	printf("%s: succeeded\n", __func__);
	return 0;
}

static unsigned int test_iter_over_allocators(void)
{
	int i;
	int count = smaf_allocator_count();
	char *name;
	int err = 0;

	if (count <= 0) {
		printf("%s: no allocator %d\n", __func__, count);
		return 1;
	}

	for (i = 0; i < count; i++) {
		name = smaf_get_allocator_name(i);
		if (!name) {
			printf("%s: smaf_get_allocator_name() failed %d\n", __func__, i);
			err++;
			continue;
		}
		err += test_create_named(name);
		free(name);
	}
	return err;
}

static unsigned int test_invalid_allocator_index(void)
{
	int count = smaf_allocator_count();
	char *name;

	if (count <= 0) {
		printf("%s: smaf_allocator_count() failed %d\n", __func__, count);
		return 1;
	}

	name = smaf_get_allocator_name(count);
	if (name) {
		printf("%s: smaf_get_allocator_name(%s) did not fail\n", __func__, name);
		free(name);
		return 1;
	}

	printf("%s: succeeded\n", __func__);
	return 0;
}


static unsigned int test_secure(void)
{
	int ret;
	int fd;

	ret = smaf_create_buffer(LENGTH, SMAF_CLOEXEC | SMAF_RDWR, NULL, &fd);
	if (ret || (fd == -1)) {
		printf("%s: smaf_create_buffer() failed %d/%d\n", __func__, ret, fd);
		return 1;
	}

	ret = smaf_set_secure(fd, 1);
	if (ret) {
		printf("%s: smaf_set_secure() failed %d\n", __func__, ret);
		ret = 1;
		goto end;
	}

	ret = smaf_get_secure(fd);
	if (!ret) {  // TODO: check this
		printf("%s: smaf_get_secure() failed %d\n", __func__, ret);
		ret = 1;
		goto end;
	}

	printf("%s: succeeded\n", __func__);
	ret = 0;
end:
	close(fd);
	return ret;
}

static unsigned int test_create_non_page_aligned_mmap(void)
{
	int ret;
	int fd;
	void *data;

	ret = smaf_create_buffer(LENGTH+1, SMAF_CLOEXEC | SMAF_RDWR, NULL, &fd);
	if (ret || fd < 0) {
		printf("%s: smaf_create_buffer() failed %d/%d\n", __func__, ret, fd);
		return 1;
	}

	data = mmap(0, LENGTH+1, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	munmap(data, LENGTH+1);
	close(fd);

	if (data == MAP_FAILED) {
		printf("%s failed\n", __func__);
		return 1;
	}
	printf("%s succeeded\n", __func__);
	return 0;
}

static unsigned int test_create_non_page_aligned(void)
{
	int ret;
	int fd;

	ret = smaf_create_buffer(LENGTH+1, SMAF_CLOEXEC | SMAF_RDWR, NULL, &fd);
	if (ret || fd < 0) {
		printf("%s: smaf_create_buffer() failed %d/%d\n", __func__, ret, fd);
		return 1;
	}

	close(fd);
	printf("%s succeeded\n", __func__);
	return 0;
}

static unsigned int test_create_unnamed(void)
{
	return test_create_named(NULL);
}

static unsigned int test_mmap(void)
{
	int ret;
	int fd;
	char *data;

	ret = smaf_create_buffer(LENGTH, SMAF_CLOEXEC | SMAF_RDWR, NULL, &fd);
	if (ret || fd < 0) {
		printf("%s: smaf_create_buffer() failed %d/%d\n", __func__, ret, fd);
		return 1;
	}

	data = mmap(NULL, LENGTH, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	munmap(data, LENGTH);
	close(fd);

	if (data == MAP_FAILED) {
		printf("%s: mmap failed\n", __func__);
		return 1;
	}
	printf("%s: succeeded\n", __func__);
	return 0;
}

static unsigned int test_mmap_secure(void)
{
	int ret;
	int fd;
	char *data;

	ret = smaf_create_buffer(LENGTH, SMAF_CLOEXEC | SMAF_RDWR, NULL, &fd);
	if (ret || fd < 0) {
		printf("%s: smaf_create_buffer() failed %d/%d\n", __func__, ret, fd);
		return 1;
	}

	ret = smaf_set_secure(fd, 1);
	if (ret) {
		printf("%s: smaf_set_secure() failed %d\n", __func__, ret);
		return 1;
	}

	data = mmap(NULL, LENGTH, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	munmap(data, LENGTH);
	close(fd);
	if (data != MAP_FAILED) {
		printf("%s: mmap on secure buffer did not fail\n", __func__);
		return 1;
	}
	printf("%s: succeeded\n", __func__);
	return 0;
}

int main (void)
{
	if (smaf_open()) {
		printf("Can't open /dev/smaf\n");
		return -1;
	}

	err += test_create_unnamed();

	err += test_iter_over_allocators();
	err += test_invalid_allocator_index();
	err += test_create_named_invalid();
	err += test_create_non_page_aligned();
	err += test_create_non_page_aligned_mmap();
	err += test_secure();

	err += test_mmap();
	err += test_mmap_secure();

	smaf_close();
	printf("smaf test status: nb errors = %d\n", err);
	return !err ? 0 : 1;
}
