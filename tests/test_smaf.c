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

static void test_create_named_invalid(void)
{
	int ret;
	int fd;

	ret = smaf_create_buffer(LENGTH, O_CLOEXEC | O_RDWR, "deadbeef", &fd);

	if (!ret) {
		printf("%s smaf_create_buffer() failed %d\n", __func__, ret);
		return;
	}

	printf("%s successed\n", __func__);
}

static void test_create_named(void)
{
	int ret;
	int fd;

	ret = smaf_create_buffer(LENGTH, O_CLOEXEC | O_RDWR, "smaf-cma", &fd);

	if (ret || (fd == -1)) {
		printf("%s smaf_create_buffer() failed %d\n", __func__, ret);
		return;
	}

	printf("%s successed\n", __func__);

	close(fd);
}

static void test_secure(void)
{
	int ret;
	int fd;

	ret = smaf_create_buffer(LENGTH, O_CLOEXEC | O_RDWR, NULL, &fd);

	if (ret || (fd == -1)) {
		printf("%s smaf_create_buffer() failed %d\n", __func__, ret);
		return;
	}

	ret = smaf_set_secure(fd, 1);
	if (ret) {
		printf("%s smaf_set_secure() failed %d\n", __func__, ret);
		goto end;
	}

	ret = smaf_get_secure(fd);
	if (!ret) {
		printf("%s smaf_get_secure() failed %d\n", __func__, ret);
		goto end;
	}

	printf("%s successed\n", __func__);
end:
	close(fd);
}

static void test_create_unnamed(void)
{
	int ret;
	int fd;

	ret = smaf_create_buffer(LENGTH, O_CLOEXEC | O_RDWR, NULL, &fd);

	if (ret || (fd == -1)) {
		printf("%s smaf_create_buffer() failed %d\n", __func__, ret);
		return;
	}

	close(fd);
	printf("%s successed\n", __func__);
}

void main (void)
{
	if (smaf_open()) {
		printf("Can't open /dev/smaf\n");
		return;
	}

	test_create_unnamed();
	test_create_named();
	test_create_named_invalid();
	test_secure();

	smaf_close();
}
