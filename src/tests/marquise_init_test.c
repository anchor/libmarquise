#include <glib.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>

#include "../marquise.h"

void test_init() {
	setenv("MARQUISE_SPOOL_DIR", "/tmp", 1);
	mkdir("/tmp/marquisetest", 0700);
	marquise_ctx *ctx = marquise_init("marquisetest");
	if (ctx == NULL) {
		printf("marquise_init failed: %s\n", strerror(errno));
		g_test_fail();
	}
}

void test_init_no_dir() {
	setenv("MARQUISE_SPOOL_DIR", "/tmp", 1);
	marquise_ctx *ctx = marquise_init("marquisetest2");
	if (ctx == NULL) {
		printf("marquise_init failed: %s\n", strerror(errno));
		g_test_fail();
	}
}

int main(int argc, char **argv) {
	g_test_init(&argc, &argv, NULL);
	g_test_add_func("/marquise_init/init", test_init);
	g_test_add_func("/marquise_init/init_no_dir", test_init_no_dir);
	return g_test_run();
}
