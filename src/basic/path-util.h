/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ "forward.h"

#నిర్వచించు మార్గం_విభజించు_BIN(x) x "sbin:" x "bin"
#నిర్వచించు మార్గం_విభజించు_BIN_NULSTR(x) x "sbin\0" x "bin\0"

#నిర్వచించు మార్గం_MERGED_BIN(x) x "bin"
#నిర్వచించు మార్గం_MERGED_BIN_NULSTR(x) x "bin\0"

#నిర్వచించు DEFAULT_మార్గం_WITH_నిండిన_SBIN మార్గం_విభజించు_BIN("/usr/local/") ":" మార్గం_విభజించు_BIN("/usr/") EXTRA_EXEC_శోధన_మార్గం_SUFFIX
#నిర్వచించు DEFAULT_మార్గం_WITH_LOCAL_SBIN మార్గం_విభజించు_BIN("/usr/local/") ":" మార్గం_MERGED_BIN("/usr/") EXTRA_EXEC_శోధన_మార్గం_SUFFIX
#నిర్వచించు DEFAULT_మార్గం_WITHOUT_SBIN మార్గం_MERGED_BIN("/usr/local/") ":" మార్గం_MERGED_BIN("/usr/") EXTRA_EXEC_శోధన_మార్గం_SUFFIX

#నిర్వచించు DEFAULT_మార్గం_COMPAT మార్గం_విభజించు_BIN("/usr/local/") ":" మార్గం_విభజించు_BIN("/usr/") ":" మార్గం_విభజించు_BIN("/") EXTRA_EXEC_శోధన_మార్గం_SUFFIX

స్థిరరాశి అక్షరం* default_మార్గం(శూన్య);

స్థిర అంతఃవరుస స్థిరరాశి అక్షరం* default_user_మార్గం(శూన్య) {
#నిర్వచితమైతే DEFAULT_USER_మార్గం
        తిరిగి DEFAULT_USER_మార్గం EXTRA_EXEC_శోధన_మార్గం_SUFFIX;
#లేకపోతే
        తిరిగి default_మార్గం();
#అయితే_ముగింపు
}

bool is_మార్గం(స్థిరరాశి అక్షరం *p) శుద్ధ;

స్థిర అంతఃవరుస bool మార్గం_is_absolute(స్థిరరాశి అక్షరం *p) {
        ఒకవేళ (!p) /* A NULL pointer is definitely not an absolute path */
                తిరిగి false;

        తిరిగి p[0] == '/';
}

పూర్ణాంకం మార్గం_విభజించు_and_make_absolute(స్థిరరాశి అక్షరం *p, అక్షరం ***ret);
అక్షరం* మార్గం_make_absolute(స్థిరరాశి అక్షరం *p, స్థిరరాశి అక్షరం *ఉపసర్గ);
పూర్ణాంకం సురక్షిత_getcwd(అక్షరం **ret);
పూర్ణాంకం మార్గం_make_absolute_cwd(స్థిరరాశి అక్షరం *p, అక్షరం **ret);
పూర్ణాంకం మార్గం_make_relative(స్థిరరాశి అక్షరం *from, స్థిరరాశి అక్షరం *to, అక్షరం **ret);
పూర్ణాంకం మార్గం_make_relative_మాతృ(స్థిరరాశి అక్షరం *from_బిడ్డ, స్థిరరాశి అక్షరం *to, అక్షరం **ret);

రకనిర్వచనం గణన మార్గం_ప్రారంభం_With_Flags {
        మార్గం_STARTSWITH_REFUSE_DOT_DOT       = 1U << 0,
        మార్గం_STARTSWITH_RETURN_LEADING_SLASH = 1U << 1,
} మార్గం_ప్రారంభం_With_Flags;

అక్షరం* మార్గం_startswith_నిండిన(స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి అక్షరం *ఉపసర్గ, మార్గం_ప్రారంభం_With_Flags flags) శుద్ధ;
స్థిర అంతఃవరుస అక్షరం* మార్గం_startswith(స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి అక్షరం *ఉపసర్గ) {
        తిరిగి మార్గం_startswith_నిండిన(మార్గం, ఉపసర్గ, 0);
}

పూర్ణాంకం మార్గం_compare(స్థిరరాశి అక్షరం *a, స్థిరరాశి అక్షరం *b) శుద్ధ;
స్థిర అంతఃవరుస bool మార్గం_equal(స్థిరరాశి అక్షరం *a, స్థిరరాశి అక్షరం *b) {
        తిరిగి మార్గం_compare(a, b) == 0;
}

పూర్ణాంకం మార్గం_compare_filename(స్థిరరాశి అక్షరం *a, స్థిరరాశి అక్షరం *b);
స్థిర అంతఃవరుస bool మార్గం_equal_filename(స్థిరరాశి అక్షరం *a, స్థిరరాశి అక్షరం *b) {
        తిరిగి మార్గం_compare_filename(a, b) == 0;
}

పూర్ణాంకం మార్గం_equal_or_inode_same_నిండిన(స్థిరరాశి అక్షరం *a, స్థిరరాశి అక్షరం *b, పూర్ణాంకం flags);
స్థిర అంతఃవరుస bool మార్గం_equal_or_inode_same(స్థిరరాశి అక్షరం *a, స్థిరరాశి అక్షరం *b, పూర్ణాంకం flags) {
        తిరిగి మార్గం_equal_or_inode_same_నిండిన(a, b, flags) > 0;
}

అక్షరం* మార్గం_extend_అంతర్గత(అక్షరం **x, ...);
#నిర్వచించు మార్గం_extend(x, ...) మార్గం_extend_అంతర్గత(x, __VA_ARGS__, సూచిక_MAX)
#నిర్వచించు మార్గం_join(...) మార్గం_extend_అంతర్గత(ఖాళీ, __VA_ARGS__, సూచిక_MAX)

అక్షరం* skip_leading_slash(స్థిరరాశి అక్షరం *p) శుద్ధ;

రకనిర్వచనం గణన మార్గం_Simplify_Flags {
        మార్గం_SIMPLIFY_KEEP_TRAILING_SLASH = 1 << 0,
} మార్గం_Simplify_Flags;

అక్షరం* మార్గం_simplify_నిండిన(అక్షరం *మార్గం, మార్గం_Simplify_Flags flags);
స్థిర అంతఃవరుస అక్షరం* మార్గం_simplify(అక్షరం *మార్గం) {
        తిరిగి మార్గం_simplify_నిండిన(మార్గం, 0);
}

పూర్ణాంకం మార్గం_simplify_alloc(స్థిరరాశి అక్షరం *మార్గం, అక్షరం **ret);

/* Note: the search terminates on the first NULL item. */
#నిర్వచించు మార్గం_లోపల_సమితి(p, ...) మార్గం_strv_కలిగియుంది(STRV_MAKE(__VA_ARGS__), p)

అక్షరం* మార్గం_startswith_strv(స్థిరరాశి అక్షరం *p, అక్షరం * స్థిరరాశి *strv);
#నిర్వచించు మార్గం_STARTSWITH_సమితి(p, ...) మార్గం_startswith_strv(p, STRV_MAKE(__VA_ARGS__))

పూర్ణాంకం మార్గం_strv_make_absolute_cwd(అక్షరం **l);
అక్షరం** మార్గం_strv_resolve(అక్షరం **l, స్థిరరాశి అక్షరం *మూలం);
అక్షరం** మార్గం_strv_resolve_uniq(అక్షరం **l, స్థిరరాశి అక్షరం *మూలం);

పూర్ణాంకం open_and_check_executable(స్థిరరాశి అక్షరం *name, స్థిరరాశి అక్షరం *మూలం, అక్షరం **ret_మార్గం, పూర్ణాంకం *ret_fd);
పూర్ణాంకం కనుగొను_executable_నిండిన(
                స్థిరరాశి అక్షరం *name,
                స్థిరరాశి అక్షరం *మూలం,
                అక్షరం * స్థిరరాశి *exec_శోధన_మార్గం,
                bool use_మార్గం_envvar,
                అక్షరం **ret_filename,
                పూర్ణాంకం *ret_fd);
స్థిర అంతఃవరుస పూర్ణాంకం కనుగొను_executable(స్థిరరాశి అక్షరం *name, అక్షరం **ret_filename) {
        తిరిగి కనుగొను_executable_నిండిన(name, /* root= */ ఖాళీ, ఖాళీ, true, ret_filename, ఖాళీ);
}

పూర్ణాంకం fsck_exists(శూన్య);
పూర్ణాంకం fsck_exists_for_fstype(స్థిరరాశి అక్షరం *fstype);

/* Iterates through the path prefixes of the specified path, going up
 * the tree, to root. Also returns "" (and not "/"!) for the root
 * directory. Excludes the specified directory itself */
#నిర్వచించు మార్గం_FOREACH_ఉపసర్గ(ఉపసర్గ, మార్గం)                               \
        కొరకు (అక్షరం *_slash = ({                                          \
                                మార్గం_simplify(strcpy(ఉపసర్గ, మార్గం));    \
                                streq(ఉపసర్గ, "/") ? ఖాళీ : strrchr(ఉపసర్గ, '/'); \
                        });                                             \
             _slash && ((*_slash = 0), true);                           \
             _slash = strrchr((ఉపసర్గ), '/'))

/* Same as PATH_FOREACH_PREFIX but also includes the specified path itself */
#నిర్వచించు మార్గం_FOREACH_ఉపసర్గ_మరిన్ని(ఉపసర్గ, మార్గం)                          \
        కొరకు (అక్షరం *_slash = ({                                          \
                                మార్గం_simplify(strcpy(ఉపసర్గ, మార్గం));    \
                                ఒకవేళ (streq(ఉపసర్గ, "/"))                 \
                                        ఉపసర్గ[0] = 0;                  \
                                strrchr(ఉపసర్గ, 0);                     \
                        });                                             \
             _slash && ((*_slash = 0), true);                           \
             _slash = strrchr((ఉపసర్గ), '/'))

పూర్ణాంకం మార్గం_కనుగొను_మొదటి_component(స్థిరరాశి అక్షరం **p, bool accept_dot_dot, స్థిరరాశి అక్షరం **ret);
పూర్ణాంకం మార్గం_కనుగొను_చివరి_component(స్థిరరాశి అక్షరం *మార్గం, bool accept_dot_dot, స్థిరరాశి అక్షరం **తదుపరి_2, స్థిరరాశి అక్షరం **ret);
స్థిరరాశి అక్షరం* చివరి_మార్గం_component(స్థిరరాశి అక్షరం *మార్గం);

పూర్ణాంకం మార్గం_విభజించు_ఉపసర్గ_filename(స్థిరరాశి అక్షరం *మార్గం, అక్షరం **ret_dir, అక్షరం **ret_filename);
స్థిర అంతఃవరుస పూర్ణాంకం మార్గం_వెలికితీయి_filename(స్థిరరాశి అక్షరం *మార్గం, అక్షరం **ret) {
        తిరిగి మార్గం_విభజించు_ఉపసర్గ_filename(మార్గం, ఖాళీ, ret);
}
స్థిర అంతఃవరుస పూర్ణాంకం మార్గం_వెలికితీయి_directory(స్థిరరాశి అక్షరం *మార్గం, అక్షరం **ret) {
        పూర్ణాంకం r = మార్గం_విభజించు_ఉపసర్గ_filename(మార్గం, ret, ఖాళీ);
        తిరిగి r < 0 ? r : 0; /* suppress O_DIRECTORY */
}

bool filename_part_is_valid(స్థిరరాశి అక్షరం *p) శుద్ధ;
bool filename_is_valid(స్థిరరాశి అక్షరం *p) శుద్ధ;
bool మార్గం_is_valid_నిండిన(స్థిరరాశి అక్షరం *p, bool accept_dot_dot) శుద్ధ;
స్థిర అంతఃవరుస bool మార్గం_is_valid(స్థిరరాశి అక్షరం *p) {
        తిరిగి మార్గం_is_valid_నిండిన(p, /* accept_dot_dot= */ true);
}
స్థిర అంతఃవరుస bool మార్గం_is_సురక్షిత(స్థిరరాశి అక్షరం *p) {
        తిరిగి మార్గం_is_valid_నిండిన(p, /* accept_dot_dot= */ false);
}
bool మార్గం_is_normalized(స్థిరరాశి అక్షరం *p) శుద్ధ;
స్థిర అంతఃవరుస bool filename_or_absolute_మార్గం_is_valid(స్థిరరాశి అక్షరం *p) {
        ఒకవేళ (మార్గం_is_absolute(p))
                తిరిగి మార్గం_is_valid(p);

        తిరిగి filename_is_valid(p);
}

పూర్ణాంకం file_లోపల_same_dir(స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి అక్షరం *filename, అక్షరం **ret);

bool hidden_or_backup_file(స్థిరరాశి అక్షరం *filename) శుద్ధ;

bool is_device_మార్గం(స్థిరరాశి అక్షరం *మార్గం) శుద్ధ;

bool valid_device_కణుపు_మార్గం(స్థిరరాశి అక్షరం *మార్గం) శుద్ధ;
bool valid_device_allow_నమూనా(స్థిరరాశి అక్షరం *మార్గం) శుద్ధ;

bool dot_or_dot_dot(స్థిరరాశి అక్షరం *మార్గం) శుద్ధ;

bool మార్గం_సూచిస్తుంది_directory(స్థిరరాశి అక్షరం *మార్గం) శుద్ధ;

స్థిర అంతఃవరుస స్థిరరాశి అక్షరం* skip_dev_ఉపసర్గ(స్థిరరాశి అక్షరం *p) {
        స్థిరరాశి అక్షరం *e;

        /* Drop any /dev prefix if there is any */

        e = మార్గం_startswith(p, "/dev/");

        తిరిగి e ?: p;
}

bool ఖాళీలు_or_మూలం(స్థిరరాశి అక్షరం *మార్గం) శుద్ధ;
స్థిరరాశి అక్షరం* ఖాళీలు_గమ్యం_మూలం(స్థిరరాశి అక్షరం *మార్గం) శుద్ధ;
పూర్ణాంకం ఖాళీలు_or_మూలం_harder_గమ్యం_null(స్థిరరాశి అక్షరం **మార్గం);

bool మార్గం_strv_కలిగియుంది(అక్షరం * స్థిరరాశి *l, స్థిరరాశి అక్షరం *మార్గం);
bool prefixed_మార్గం_strv_కలిగియుంది(అక్షరం * స్థిరరాశి *l, స్థిరరాశి అక్షరం *మార్గం);

పూర్ణాంకం మార్గం_glob_can_సరిపోలిక(స్థిరరాశి అక్షరం *నమూనా, స్థిరరాశి అక్షరం *ఉపసర్గ, అక్షరం **ret);
