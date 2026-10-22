/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#شامل "forward.h"

/* Erase characters until the end of the line */
#define ANSI_ERASE_TO_END_OF_سطر "\x1B[K"

/* Erase characters until end of screen */
#define ANSI_ERASE_TO_END_OF_SCREEN "\x1B[J"

/* Move cursor up one line */
#define ANSI_REVERSE_LINEFEED "\x1BM"

/* Set cursor to top left corner and clear screen */
#define ANSI_HOME_CLEAR "\x1B[H\x1B[2J"

/* Push/pop a window title off the stack of window titles */
#define ANSI_WINDOW_TITLE_افزودن "\x1b[22;2t"
#define ANSI_WINDOW_TITLE_برداشت "\x1b[23;2t"

/* The "device control string" ("DCS") start sequence */
#define ANSI_DCS "\eP"

/* The "operating system command" ("OSC") start sequence */
#define ANSI_OSC "\e]"

/* ANSI "string terminator" character ("ST"). Terminal emulators typically allow three different ones: 0x07,
 * 0x9c, and 0x1B 0x5C. We'll avoid 0x07 (BEL, aka ^G) since it might trigger unexpected TTY signal handling.
 * And we'll avoid 0x9c since that's also valid regular codepoint in UTF-8 and elsewhere, and creates
 * ambiguities. Because of that some terminal emulators explicitly choose not to support it. Hence we use
 * 0x1B 0x5c. */
#define ANSI_ST "\e\\"

bool isatty_safe(صحیح fd);

صحیح terminal_reset_ansi_seq(صحیح fd);

تعریفنوع شمارش TerminalResetFlags {
        TERMINAL_RESET_SWITCH_TO_TEXT = 1 << 0,
        TERMINAL_RESET_AVOID_ANSI_SEQ = 1 << 1,
        TERMINAL_RESET_FORCE_ANSI_SEQ = 1 << 2,
} TerminalResetFlags;

صحیح terminal_reset_defensive(صحیح fd, TerminalResetFlags flags);
صحیح terminal_reset_defensive_locked(صحیح fd, TerminalResetFlags flags);

صحیح terminal_set_cursor_position(صحیح fd, unsigned row, unsigned column);
صحیح terminal_get_cursor_position(صحیح درون_داد_fd, صحیح output_fd, unsigned *ret_rows, unsigned *ret_column);

صحیح open_terminal(ثابت نویسه *name, صحیح mode);

/* Flags for tweaking the way we become the controlling process of a terminal. */
تعریفنوع شمارش AcquireTerminalFlags {
        /* Try to become the controlling process of the TTY. If we can't return -EPERM. */
        ACQUIRE_TERMINAL_TRY        = 0,

        /* Tell the kernel to forcibly make us the controlling process of the TTY. Returns -EPERM if the kernel doesn't allow that. */
        ACQUIRE_TERMINAL_FORCE      = 1,

        /* If we can't become the controlling process of the TTY right-away, then wait until we can. */
        ACQUIRE_TERMINAL_WAIT       = 2,

        /* The combined mask of the above */
        _ACQUIRE_TERMINAL_MODE_MASK = ACQUIRE_TERMINAL_TRY | ACQUIRE_TERMINAL_FORCE | ACQUIRE_TERMINAL_WAIT,

        /* Pick one of the above, and then OR this flag in, in order to request permissive behaviour, if we can't become controlling process then don't mind */
        ACQUIRE_TERMINAL_PERMISSIVE = 1 << 2,

        /* Check for pending SIGTERM while waiting for inotify (SIGTERM must be blocked by caller) */
        ACQUIRE_TERMINAL_WATCH_SIGTERM = 1 << 3,
} AcquireTerminalFlags;

صحیح acquire_terminal(ثابت نویسه *name, AcquireTerminalFlags flags, usec_t timeout);
صحیح release_terminal(تهی);

صحیح terminal_new_session(تهی);
تهی terminal_detach_session(تهی);

صحیح terminal_vhangup_fd(صحیح fd);
صحیح terminal_vhangup(ثابت نویسه *tty);

صحیح terminal_set_size_fd(صحیح fd, ثابت نویسه *ident, unsigned rows, unsigned cols);
صحیح proc_cmdline_tty_size(ثابت نویسه *tty, unsigned *ret_rows, unsigned *ret_cols);

صحیح chvt(صحیح vt);

صحیح read_one_char(FILE *f, نویسه *ret, usec_t timeout, bool echo, bool *need_nl);
صحیح ask_char(نویسه *ret, ثابت نویسه *replies, ثابت نویسه *fmt, ...) _printf_(3, 4);

تعریفنوع شمارش GetCompletionsFlags {
        /* Only return the items subject to preselection: typically you want to suppress meta entries such as
         * "list" or alias entries if this flag is set. */
        GET_COMPLETIONS_PRESELECT = 1 << 0,
} GetCompletionsFlags;

تعریفنوع صحیح (*GetCompletionsCallback)(ثابت نویسه *کلید, GetCompletionsFlags flags, نویسه ***ret_list, تهی *userdata);
صحیح ask_string_full(نویسه **ret, ثابت نویسه *prefill, GetCompletionsCallback get_completions, تهی *userdata, ثابت نویسه *text, ...) _printf_(5, 6);
#define ask_string(ret, text, ...) ask_string_full(ret, پوچ, پوچ, پوچ, text, ##__VA_ARGS__)

bool any_کلید_to_proceed(تهی);
صحیح show_menu(نویسه **x, size_t n_columns, size_t column_width, unsigned ellipsize_percentage, ثابت نویسه *grey_prefix, bool with_numbers);

صحیح vt_disallocate(ثابت نویسه *tty_path);

صحیح resolve_dev_console(نویسه **ret);
صحیح get_kernel_consoles(نویسه ***ret);
bool tty_is_vc(ثابت نویسه *tty);
bool tty_is_vc_resolve(ثابت نویسه *tty);
bool tty_is_console(ثابت نویسه *tty) _pure_;
صحیح vtnr_from_tty(ثابت نویسه *tty);

تهی reset_dev_console_fd(صحیح fd, bool switch_to_text);
صحیح lock_dev_console(تهی);
صحیح make_console_stdio(bool switch_to_text);

صحیح getenv_columns(تهی);
صحیح fd_columns(صحیح fd);
unsigned columns(تهی);
صحیح fd_سطرها(صحیح fd);
unsigned سطرها(تهی);

تهی columns_سطرها_cache_reset(صحیح _unused_ signum);
تهی reset_terminal_feature_caches(تهی);

bool on_tty(تهی);
bool term_env_valid(ثابت نویسه *term);
bool getenv_terminal_is_dumb(تهی);
bool terminal_is_dumb(تهی);

bool dev_console_colors_enabled(تهی);

صحیح get_ctty_devnr(pid_t pid, dev_t *ret);
صحیح get_ctty(pid_t pid, dev_t *ret_devnr, نویسه **ret);

صحیح getttyname_malloc(صحیح fd, نویسه **ret);
صحیح getttyname_harder(صحیح fd, نویسه **ret);

صحیح ptsname_malloc(صحیح fd, نویسه **ret);

صحیح openpt_allocate(صحیح flags, نویسه **ret_peer_path);
صحیح openpt_allocate_in_namespace(ثابت PidRef *pidref, صحیح flags, نویسه **ret_peer_path);

صحیح vt_restore(صحیح fd);
صحیح vt_release(صحیح fd, bool restore);

تهی get_log_colors(صحیح priority, ثابت نویسه **on, ثابت نویسه **off, ثابت نویسه **highlight);

/* Assume TTY_MODE is defined in config.h. Also, this assumes there is a 'tty' group. */
assert_cc((TTY_MODE & ~0666) == 0);
assert_cc((TTY_MODE & 0711) == 0600);

تهی termios_disable_echo(ساختار termios *termios);

/* A termios sentinel with all flag fields set to all-ones-bits. No real tcgetattr() result will ever
 * match this because no real terminal configuration uses all-ones in every flag field simultaneously. */
#define TERMIOS_NULL (ساختار termios) {         \
        .c_iflag = UINT_MAX,                    \
        .c_oflag = UINT_MAX,                    \
        .c_cflag = UINT_MAX,                    \
        .c_lflag = UINT_MAX,                    \
}

تعریفنوع ساختار TermiosResetContext {
        صحیح *fd;
        ساختار termios *termios;
} TermiosResetContext;

تهی termios_reset(ثابت TermiosResetContext *c);

#define CLEANUP_TERMIOS_RESET(_fd, _termios)                                   \
        _cleanup_(termios_reset) _unused_ ثابت TermiosResetContext            \
                CONCATENATE(_cleanup_termios_, UNIQ) = {                       \
                        .fd = &(_fd),                                          \
                        .termios = &(_termios),                                \
                }

/* The $TERM value we use for terminals other than the Linux console */
#define FALLBACK_TERM "vt220"

صحیح get_default_background_color(دوگانه *ret_red, دوگانه *ret_green, دوگانه *ret_blue);
صحیح terminal_get_size(صحیح درون_داد_fd, صحیح output_fd, unsigned *ret_rows, unsigned *ret_columns, bool try_dsr, bool try_csi18);
صحیح terminal_fix_size(صحیح درون_داد_fd, صحیح output_fd);

صحیح terminal_get_terminfo_by_dcs(صحیح fd, نویسه **ret_name);
صحیح have_terminfo_file(ثابت نویسه *name);
صحیح query_term_for_tty(ثابت نویسه *tty, نویسه **ret_term);

صحیح terminal_is_pty_fd(صحیح fd);

صحیح pty_open_peer(صحیح fd, صحیح mode);

ایستا درونخطی bool osc_char_is_valid(نویسه c) {
        /* Checks whether the specified character is safe to be included inside an ANSI OSC sequence, as per
         * ECMA-48 5th edition, section 8.3.89 */
        بازگشت (unsigned نویسه) c >= 32 && (unsigned نویسه) c < 127;
}

#define VTNR_MAX 63

ایستا درونخطی bool vtnr_is_valid(unsigned n) {
        بازگشت n >= 1 && n <= VTNR_MAX;
}
