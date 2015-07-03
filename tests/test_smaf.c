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
#include <stdio.h>

#define LENGTH 1024*16

static void test_select_invalid(void)
{
	int ret;
	int fd;
	ret = smaf_open();

	if (ret) {
		printf("test_select_invalid smaf_open() failed %d\n", ret);
		return;
	}

	ret = smaf_create_buffer(LENGTH, O_CLOEXEC | O_RDWR, &fd);

	if (ret || (fd == -1)) {
		printf("test_select_invalid smaf_create_buffer() failed %d\n", ret);
		smaf_close();
		return;
	}

	ret = smaf_select_allocator(fd, "deadbeef");
	if (!ret) {
		printf("test_select_invalid smaf_select_allocator failed %d\n", ret);
		goto end;
	}

	printf("test_select_invalid successed\n");
end:
	close(fd);
	smaf_close();
}

static void test_select(void)
{
	int ret;
	int fd;
	ret = smaf_open();

	if (ret) {
		printf("test_select smaf_open() failed %d\n", ret);
		return;
	}

	ret = smaf_create_buffer(LENGTH, O_CLOEXEC | O_RDWR, &fd);

	if (ret || (fd == -1)) {
		printf("test_select smaf_create_buffer() failed %d\n", ret);
		smaf_close();
		return;
	}

	ret = smaf_select_allocator(fd, "smaf-cma");
	if (ret) {
		printf("test_select smaf_select_allocator failed %d\n", ret);
		goto end;
	}

	printf("test_select successed\n");
end:
	close(fd);
	smaf_close();
}

static void test_secure(void)
{
	int ret;
	int fd;
	ret = smaf_open();

	if (ret) {
		printf("test_secure smaf_open() failed %d\n", ret);
		return;
	}

	ret = smaf_create_buffer(LENGTH, O_CLOEXEC | O_RDWR, &fd);

	if (ret || (fd == -1)) {
		printf("test_secure smaf_create_buffer() failed %d\n", ret);
		smaf_close();
		return;
	}

	ret = smaf_set_secure(fd, 1);
	if (ret) {
		printf("test_secure smaf_set_secure() failed %d\n", ret);
		goto end;
	}

	ret = smaf_get_secure(fd);
	if (!ret) {
		printf("test_secure smaf_get_secure() failed %d\n", ret);
		goto end;
	}

	printf("test_secure successed\n");
end:
	close(fd);
	smaf_close();
}

static void test_create(void)
{
	int ret;
	int fd;
	ret = smaf_open();

	if (ret) {
		printf("test_create smaf_open() failed %d\n", ret);
		return;
	}

	ret = smaf_create_buffer(LENGTH, O_CLOEXEC | O_RDWR, &fd);

	if (ret || (fd == -1)) {
		printf("test_create smaf_create_buffer() failed %d\n", ret);
		smaf_close();
		return;
	}

	close(fd);
	smaf_close();
	printf("test_create successed\n");
}

void main (void)
{
	test_create();
	test_secure();
	test_select();
	test_select_invalid();
}
