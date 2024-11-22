//! add-flags.py(RUN): --dir fs::/

/*
 * We modified musl-fts not to use fchdir() and we made FTS_NOCHDIR
 * the default behavior. This test is to make sure that the modified
 * musl-fts works as expected.
 */

#include <fts.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>

void __expect_next_ftsent(FTSENT *p, const char *path, short level, int info, const char *file, int line) {
    if (p == NULL) {
        printf("Error: fts_read() returned NULL at %s:%d\n", file, line);
        exit(1);
    }

    if (strcmp(p->fts_path, path) != 0) {
        printf("Error: expected path \"%s\", got \"%s\" at %s:%d\n", path, p->fts_path, file, line);
        exit(1);
    }

    if (p->fts_level != level) {
        printf("Error: expected level %d, got %d at %s:%d\n", level, p->fts_level, file, line);
        exit(1);
    }

    if (p->fts_info != info) {
        printf("Error: expected info %d, got %d at %s:%d\n", info, p->fts_info, file, line);
        exit(1);
    }
}

#define expect_next_ftsent(p, path, level, info) __expect_next_ftsent(p, path, level, info, __FILE__, __LINE__)

static int compare_ents (const FTSENT **a, const FTSENT **b) {
    return strcmp((*a)->fts_name, (*b)->fts_name);
}

void touch(const char *path) {
    FILE *f = fopen(path, "w");
    if (f == NULL) {
        printf("Error: fopen(%s) failed: %s\n", path, strerror(errno));
        exit(1);
    }
    fclose(f);
}

void test_fts_info(int base_options) {
    FTS *ftsp;

    mkdir("t1", 0755);
    touch("t1/file1");
    mkdir("t1/dir1", 0755);
    touch("t1/dir1/file2");
    mkdir("t1/dir1/dir2", 0755);
    touch("t1/dir1/file3");

    char *paths[] = {"./t1", NULL};
    ftsp = fts_open(paths, base_options, compare_ents);
    if (ftsp == NULL) {
        printf("Error: fts_open(%s, %d) failed: %s\n", paths[0], base_options, strerror(errno));
        exit(1);
    }

    expect_next_ftsent(fts_read(ftsp), "./t1", 0, FTS_D);
    expect_next_ftsent(fts_read(ftsp), "./t1/dir1", 1, FTS_D);
    expect_next_ftsent(fts_read(ftsp), "./t1/dir1/dir2", 2, FTS_D);
    expect_next_ftsent(fts_read(ftsp), "./t1/dir1/dir2", 2, FTS_DP);
    expect_next_ftsent(fts_read(ftsp), "./t1/dir1/file2", 2, FTS_F);
    expect_next_ftsent(fts_read(ftsp), "./t1/dir1/file3", 2, FTS_F);
    expect_next_ftsent(fts_read(ftsp), "./t1/dir1", 1, FTS_DP);
    expect_next_ftsent(fts_read(ftsp), "./t1/file1", 1, FTS_F);
    expect_next_ftsent(fts_read(ftsp), "./t1", 0, FTS_DP);

    fts_close(ftsp);
}

void test_symlink_fts_info(int base_options) {
    FTS *ftsp;

    mkdir("t2", 0755);
    touch("t2/file1");
    symlink("file1", "t2/symlink1");
    symlink("nonexistent", "t2/broken_symlink1");

    char *paths[] = {"./t2", NULL};
    ftsp = fts_open(paths, base_options, compare_ents);
    if (ftsp == NULL) {
        printf("Error: fts_open(%s, %d) failed: %s\n", paths[0], base_options, strerror(errno));
        exit(1);
    }

    expect_next_ftsent(fts_read(ftsp), "./t2", 0, FTS_D);
    expect_next_ftsent(fts_read(ftsp), "./t2/broken_symlink1", 1, FTS_SL);
    expect_next_ftsent(fts_read(ftsp), "./t2/file1", 1, FTS_F);
    expect_next_ftsent(fts_read(ftsp), "./t2/symlink1", 1, FTS_SL);
    expect_next_ftsent(fts_read(ftsp), "./t2", 0, FTS_DP);

    fts_close(ftsp);

    ftsp = fts_open(paths, base_options | FTS_LOGICAL, compare_ents);
    if (ftsp == NULL) {
        printf("Error: fts_open(%s, %d | FTS_LOGICAL) failed: %s\n", paths[0], base_options, strerror(errno));
        exit(1);
    }

    expect_next_ftsent(fts_read(ftsp), "./t2", 0, FTS_D);
    // FTS_SLNONE should be returned for broken symlinks in FTS_LOGICAL mode
    expect_next_ftsent(fts_read(ftsp), "./t2/broken_symlink1", 1, FTS_SLNONE);
    expect_next_ftsent(fts_read(ftsp), "./t2/file1", 1, FTS_F);
    expect_next_ftsent(fts_read(ftsp), "./t2/symlink1", 1, FTS_F);
    expect_next_ftsent(fts_read(ftsp), "./t2", 0, FTS_DP);
}

void __expect_child_ftsent(FTSENT *p, const char *name, short level, int info, const char *file, int line) {
    if (p == NULL) {
        printf("Error: fts_children() returned NULL at %s:%d\n", file, line);
        exit(1);
    }

    // Check fts_name instead of fts_path because fts_children() doesn't set fts_path
    if (strcmp(p->fts_name, name) != 0) {
        printf("Error: expected name \"%s\", got \"%s\" at %s:%d\n", name, p->fts_name, file, line);
        exit(1);
    }

    if (p->fts_level != level) {
        printf("Error: expected level %d, got %d at %s:%d\n", level, p->fts_level, file, line);
        exit(1);
    }

    if (p->fts_info != info) {
        printf("Error: expected info %d, got %d at %s:%d\n", info, p->fts_info, file, line);
        exit(1);
    }
}

#define expect_child_ftsent(p, name, level, info) __expect_child_ftsent(p, name, level, info, __FILE__, __LINE__)

void test_fts_children(int base_options) {
    FTS *ftsp;
    FTSENT *p;

    mkdir("t3", 0755);
    touch("t3/file1");
    mkdir("t3/dir1", 0755);
    touch("t3/dir1/file2");
    mkdir("t3/dir1/dir2", 0755);
    touch("t3/dir1/file3");

    char *paths[] = {"./t3", NULL};
    ftsp = fts_open(paths, base_options, compare_ents);
    if (ftsp == NULL) {
        printf("Error: fts_open(%s, %d) failed: %s\n", paths[0], base_options, strerror(errno));
        exit(1);
    }

    FTSENT *t3 = fts_read(ftsp);
    expect_next_ftsent(t3, "./t3", 0, FTS_D);

    FTSENT *ents = fts_children(ftsp, 0);
    expect_child_ftsent(ents, "dir1", 1, FTS_D);
    expect_child_ftsent(ents->fts_link, "file1", 1, FTS_F);

    fts_close(ftsp);
}

int main(void) {
    int base_options_set[] = {FTS_PHYSICAL, FTS_NOCHDIR};
    for (int i = 0; i < sizeof(base_options_set) / sizeof(base_options_set[0]); i++) {
        test_fts_info(base_options_set[i]);
        test_symlink_fts_info(base_options_set[i]);
        test_fts_children(base_options_set[i]);
    }
    return 0;
}
