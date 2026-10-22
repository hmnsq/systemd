/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব "forward.h"

/* Erase characters until the end of the line */
#define ANSI_ERASE_TO_END_OF_সারি "\x1B[K"

/* Erase characters until end of screen */
#define ANSI_ERASE_TO_END_OF_SCREEN "\x1B[J"

/* Move cursor up one line */
#define ANSI_REVERSE_LINEFEED "\x1BM"

/* Set cursor to top left corner and clear screen */
#define ANSI_HOME_CLEAR "\x1B[H\x1B[2J"

/* Push/pop a window title off the stack of window titles */
#define ANSI_WINDOW_TITLE_ঠেলো "\x1b[22;2t"
#define ANSI_WINDOW_TITLE_বের_করো "\x1b[23;2t"

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

bool isatty_safe(পূর্ণসংখ্যা fd);

পূর্ণসংখ্যা terminal_reset_ansi_seq(পূর্ণসংখ্যা fd);

প্রকারলেখ গণনা TerminalResetFlags {
        TERMINAL_RESET_SWITCH_TO_TEXT = 1 << 0,
        TERMINAL_RESET_AVOID_ANSI_SEQ = 1 << 1,
        TERMINAL_RESET_FORCE_ANSI_SEQ = 1 << 2,
} TerminalResetFlags;

পূর্ণসংখ্যা terminal_reset_defensive(পূর্ণসংখ্যা fd, TerminalResetFlags flags);
পূর্ণসংখ্যা terminal_reset_defensive_locked(পূর্ণসংখ্যা fd, TerminalResetFlags flags);

পূর্ণসংখ্যা terminal_set_cursor_position(পূর্ণসংখ্যা fd, unsigned row, unsigned column);
পূর্ণসংখ্যা terminal_get_cursor_position(পূর্ণসংখ্যা প্রবেশ_fd, পূর্ণসংখ্যা output_fd, unsigned *ret_rows, unsigned *ret_column);

পূর্ণসংখ্যা open_terminal(ধ্রুবক অক্ষর *name, পূর্ণসংখ্যা mode);

/* Flags for tweaking the way we become the controlling process of a terminal. */
প্রকারলেখ গণনা AcquireTerminalFlags {
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

পূর্ণসংখ্যা acquire_terminal(ধ্রুবক অক্ষর *name, AcquireTerminalFlags flags, usec_t timeout);
পূর্ণসংখ্যা release_terminal(শূন্য);

পূর্ণসংখ্যা terminal_new_session(শূন্য);
শূন্য terminal_detach_session(শূন্য);

পূর্ণসংখ্যা terminal_vhangup_fd(পূর্ণসংখ্যা fd);
পূর্ণসংখ্যা terminal_vhangup(ধ্রুবক অক্ষর *tty);

পূর্ণসংখ্যা terminal_set_আকার_fd(পূর্ণসংখ্যা fd, ধ্রুবক অক্ষর *ident, unsigned rows, unsigned cols);
পূর্ণসংখ্যা proc_cmdline_tty_আকার(ধ্রুবক অক্ষর *tty, unsigned *ret_rows, unsigned *ret_cols);

পূর্ণসংখ্যা chvt(পূর্ণসংখ্যা vt);

পূর্ণসংখ্যা read_one_char(FILE *f, অক্ষর *ret, usec_t timeout, bool echo, bool *need_nl);
পূর্ণসংখ্যা ask_char(অক্ষর *ret, ধ্রুবক অক্ষর *replies, ধ্রুবক অক্ষর *fmt, ...) _printf_(3, 4);

প্রকারলেখ গণনা GetCompletionsFlags {
        /* Only return the items subject to preselection: typically you want to suppress meta entries such as
         * "list" or alias entries if this flag is set. */
        GET_COMPLETIONS_PRESELECT = 1 << 0,
} GetCompletionsFlags;

প্রকারলেখ পূর্ণসংখ্যা (*GetCompletionsCallback)(ধ্রুবক অক্ষর *চাবি, GetCompletionsFlags flags, অক্ষর ***ret_list, শূন্য *userdata);
পূর্ণসংখ্যা ask_string_full(অক্ষর **ret, ধ্রুবক অক্ষর *prefill, GetCompletionsCallback get_completions, শূন্য *userdata, ধ্রুবক অক্ষর *text, ...) _printf_(5, 6);
#define ask_string(ret, text, ...) ask_string_full(ret, ফাঁকা, ফাঁকা, ফাঁকা, text, ##__VA_ARGS__)

bool any_চাবি_to_proceed(শূন্য);
পূর্ণসংখ্যা show_menu(অক্ষর **x, size_t n_columns, size_t column_width, unsigned ellipsize_percentage, ধ্রুবক অক্ষর *grey_prefix, bool with_numbers);

পূর্ণসংখ্যা vt_disallocate(ধ্রুবক অক্ষর *tty_path);

পূর্ণসংখ্যা resolve_dev_console(অক্ষর **ret);
পূর্ণসংখ্যা get_kernel_consoles(অক্ষর ***ret);
bool tty_is_vc(ধ্রুবক অক্ষর *tty);
bool tty_is_vc_resolve(ধ্রুবক অক্ষর *tty);
bool tty_is_console(ধ্রুবক অক্ষর *tty) _pure_;
পূর্ণসংখ্যা vtnr_from_tty(ধ্রুবক অক্ষর *tty);

শূন্য reset_dev_console_fd(পূর্ণসংখ্যা fd, bool switch_to_text);
পূর্ণসংখ্যা lock_dev_console(শূন্য);
পূর্ণসংখ্যা make_console_stdio(bool switch_to_text);

পূর্ণসংখ্যা getenv_columns(শূন্য);
পূর্ণসংখ্যা fd_columns(পূর্ণসংখ্যা fd);
unsigned columns(শূন্য);
পূর্ণসংখ্যা fd_সারিগুলি(পূর্ণসংখ্যা fd);
unsigned সারিগুলি(শূন্য);

শূন্য columns_সারিগুলি_cache_reset(পূর্ণসংখ্যা _unused_ signum);
শূন্য reset_terminal_feature_caches(শূন্য);

bool on_tty(শূন্য);
bool term_env_valid(ধ্রুবক অক্ষর *term);
bool getenv_terminal_is_dumb(শূন্য);
bool terminal_is_dumb(শূন্য);

bool dev_console_colors_enabled(শূন্য);

পূর্ণসংখ্যা get_ctty_devnr(pid_t pid, dev_t *ret);
পূর্ণসংখ্যা get_ctty(pid_t pid, dev_t *ret_devnr, অক্ষর **ret);

পূর্ণসংখ্যা getttyname_malloc(পূর্ণসংখ্যা fd, অক্ষর **ret);
পূর্ণসংখ্যা getttyname_harder(পূর্ণসংখ্যা fd, অক্ষর **ret);

পূর্ণসংখ্যা ptsname_malloc(পূর্ণসংখ্যা fd, অক্ষর **ret);

পূর্ণসংখ্যা openpt_allocate(পূর্ণসংখ্যা flags, অক্ষর **ret_peer_path);
পূর্ণসংখ্যা openpt_allocate_in_namespace(ধ্রুবক PidRef *pidref, পূর্ণসংখ্যা flags, অক্ষর **ret_peer_path);

পূর্ণসংখ্যা vt_restore(পূর্ণসংখ্যা fd);
পূর্ণসংখ্যা vt_release(পূর্ণসংখ্যা fd, bool restore);

শূন্য get_log_colors(পূর্ণসংখ্যা priority, ধ্রুবক অক্ষর **on, ধ্রুবক অক্ষর **off, ধ্রুবক অক্ষর **highlight);

/* Assume TTY_MODE is defined in config.h. Also, this assumes there is a 'tty' group. */
assert_cc((TTY_MODE & ~0666) == 0);
assert_cc((TTY_MODE & 0711) == 0600);

শূন্য termios_disable_echo(কাঠামো termios *termios);

/* A termios sentinel with all flag fields set to all-ones-bits. No real tcgetattr() result will ever
 * match this because no real terminal configuration uses all-ones in every flag field simultaneously. */
#define TERMIOS_NULL (কাঠামো termios) {         \
        .c_iflag = UINT_MAX,                    \
        .c_oflag = UINT_MAX,                    \
        .c_cflag = UINT_MAX,                    \
        .c_lflag = UINT_MAX,                    \
}

প্রকারলেখ কাঠামো TermiosResetContext {
        পূর্ণসংখ্যা *fd;
        কাঠামো termios *termios;
} TermiosResetContext;

শূন্য termios_reset(ধ্রুবক TermiosResetContext *c);

#define CLEANUP_TERMIOS_RESET(_fd, _termios)                                   \
        _cleanup_(termios_reset) _unused_ ধ্রুবক TermiosResetContext            \
                CONCATENATE(_cleanup_termios_, UNIQ) = {                       \
                        .fd = &(_fd),                                          \
                        .termios = &(_termios),                                \
                }

/* The $TERM value we use for terminals other than the Linux console */
#define FALLBACK_TERM "vt220"

পূর্ণসংখ্যা get_default_background_color(দ্বিগুণ *ret_red, দ্বিগুণ *ret_green, দ্বিগুণ *ret_blue);
পূর্ণসংখ্যা terminal_get_আকার(পূর্ণসংখ্যা প্রবেশ_fd, পূর্ণসংখ্যা output_fd, unsigned *ret_rows, unsigned *ret_columns, bool try_dsr, bool try_csi18);
পূর্ণসংখ্যা terminal_fix_আকার(পূর্ণসংখ্যা প্রবেশ_fd, পূর্ণসংখ্যা output_fd);

পূর্ণসংখ্যা terminal_get_terminfo_by_dcs(পূর্ণসংখ্যা fd, অক্ষর **ret_name);
পূর্ণসংখ্যা have_terminfo_file(ধ্রুবক অক্ষর *name);
পূর্ণসংখ্যা query_term_for_tty(ধ্রুবক অক্ষর *tty, অক্ষর **ret_term);

পূর্ণসংখ্যা terminal_is_pty_fd(পূর্ণসংখ্যা fd);

পূর্ণসংখ্যা pty_open_peer(পূর্ণসংখ্যা fd, পূর্ণসংখ্যা mode);

স্থির অন্তঃরেখ bool osc_char_is_valid(অক্ষর c) {
        /* Checks whether the specified character is safe to be included inside an ANSI OSC sequence, as per
         * ECMA-48 5th edition, section 8.3.89 */
        ফেরত (unsigned অক্ষর) c >= 32 && (unsigned অক্ষর) c < 127;
}

#define VTNR_MAX 63

স্থির অন্তঃরেখ bool vtnr_is_valid(unsigned n) {
        ফেরত n >= 1 && n <= VTNR_MAX;
}
