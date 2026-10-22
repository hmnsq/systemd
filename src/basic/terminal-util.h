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

bool isatty_safe(পূর্ণাঙ্ক fd);

পূর্ণাঙ্ক terminal_reset_ansi_seq(পূর্ণাঙ্ক fd);

প্রকারনির্দেশ গণনা TerminalResetFlags {
        TERMINAL_RESET_SWITCH_TO_TEXT = 1 << 0,
        TERMINAL_RESET_AVOID_ANSI_SEQ = 1 << 1,
        TERMINAL_RESET_FORCE_ANSI_SEQ = 1 << 2,
} TerminalResetFlags;

পূর্ণাঙ্ক terminal_reset_defensive(পূর্ণাঙ্ক fd, TerminalResetFlags flags);
পূর্ণাঙ্ক terminal_reset_defensive_locked(পূর্ণাঙ্ক fd, TerminalResetFlags flags);

পূর্ণাঙ্ক terminal_set_cursor_position(পূর্ণাঙ্ক fd, unsigned row, unsigned column);
পূর্ণাঙ্ক terminal_get_cursor_position(পূর্ণাঙ্ক প্রবেশ_fd, পূর্ণাঙ্ক output_fd, unsigned *ret_rows, unsigned *ret_column);

পূর্ণাঙ্ক open_terminal(ধ্রুব বর্ণ *name, পূর্ণাঙ্ক mode);

/* Flags for tweaking the way we become the controlling process of a terminal. */
প্রকারনির্দেশ গণনা AcquireTerminalFlags {
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

পূর্ণাঙ্ক acquire_terminal(ধ্রুব বর্ণ *name, AcquireTerminalFlags flags, usec_t timeout);
পূর্ণাঙ্ক release_terminal(শূন্য);

পূর্ণাঙ্ক terminal_new_session(শূন্য);
শূন্য terminal_detach_session(শূন্য);

পূর্ণাঙ্ক terminal_vhangup_fd(পূর্ণাঙ্ক fd);
পূর্ণাঙ্ক terminal_vhangup(ধ্রুব বর্ণ *tty);

পূর্ণাঙ্ক terminal_set_size_fd(পূর্ণাঙ্ক fd, ধ্রুব বর্ণ *ident, unsigned rows, unsigned cols);
পূর্ণাঙ্ক proc_cmdline_tty_size(ধ্রুব বর্ণ *tty, unsigned *ret_rows, unsigned *ret_cols);

পূর্ণাঙ্ক chvt(পূর্ণাঙ্ক vt);

পূর্ণাঙ্ক read_one_char(FILE *f, বর্ণ *ret, usec_t timeout, bool echo, bool *need_nl);
পূর্ণাঙ্ক ask_char(বর্ণ *ret, ধ্রুব বর্ণ *replies, ধ্রুব বর্ণ *fmt, ...) _printf_(3, 4);

প্রকারনির্দেশ গণনা GetCompletionsFlags {
        /* Only return the items subject to preselection: typically you want to suppress meta entries such as
         * "list" or alias entries if this flag is set. */
        GET_COMPLETIONS_PRESELECT = 1 << 0,
} GetCompletionsFlags;

প্রকারনির্দেশ পূর্ণাঙ্ক (*GetCompletionsCallback)(ধ্রুব বর্ণ *চাবি, GetCompletionsFlags flags, বর্ণ ***ret_list, শূন্য *userdata);
পূর্ণাঙ্ক ask_string_full(বর্ণ **ret, ধ্রুব বর্ণ *prefill, GetCompletionsCallback get_completions, শূন্য *userdata, ধ্রুব বর্ণ *text, ...) _printf_(5, 6);
#define ask_string(ret, text, ...) ask_string_full(ret, রিক্ত, রিক্ত, রিক্ত, text, ##__VA_ARGS__)

bool any_চাবি_to_proceed(শূন্য);
পূর্ণাঙ্ক show_menu(বর্ণ **x, size_t n_columns, size_t column_width, unsigned ellipsize_percentage, ধ্রুব বর্ণ *grey_prefix, bool with_numbers);

পূর্ণাঙ্ক vt_disallocate(ধ্রুব বর্ণ *tty_path);

পূর্ণাঙ্ক resolve_dev_console(বর্ণ **ret);
পূর্ণাঙ্ক get_kernel_consoles(বর্ণ ***ret);
bool tty_is_vc(ধ্রুব বর্ণ *tty);
bool tty_is_vc_resolve(ধ্রুব বর্ণ *tty);
bool tty_is_console(ধ্রুব বর্ণ *tty) _pure_;
পূর্ণাঙ্ক vtnr_from_tty(ধ্রুব বর্ণ *tty);

শূন্য reset_dev_console_fd(পূর্ণাঙ্ক fd, bool switch_to_text);
পূর্ণাঙ্ক lock_dev_console(শূন্য);
পূর্ণাঙ্ক make_console_stdio(bool switch_to_text);

পূর্ণাঙ্ক getenv_columns(শূন্য);
পূর্ণাঙ্ক fd_columns(পূর্ণাঙ্ক fd);
unsigned columns(শূন্য);
পূর্ণাঙ্ক fd_সারিগুলি(পূর্ণাঙ্ক fd);
unsigned সারিগুলি(শূন্য);

শূন্য columns_সারিগুলি_cache_reset(পূর্ণাঙ্ক _unused_ signum);
শূন্য reset_terminal_feature_caches(শূন্য);

bool on_tty(শূন্য);
bool term_env_valid(ধ্রুব বর্ণ *term);
bool getenv_terminal_is_dumb(শূন্য);
bool terminal_is_dumb(শূন্য);

bool dev_console_colors_enabled(শূন্য);

পূর্ণাঙ্ক get_ctty_devnr(pid_t pid, dev_t *ret);
পূর্ণাঙ্ক get_ctty(pid_t pid, dev_t *ret_devnr, বর্ণ **ret);

পূর্ণাঙ্ক getttyname_malloc(পূর্ণাঙ্ক fd, বর্ণ **ret);
পূর্ণাঙ্ক getttyname_harder(পূর্ণাঙ্ক fd, বর্ণ **ret);

পূর্ণাঙ্ক ptsname_malloc(পূর্ণাঙ্ক fd, বর্ণ **ret);

পূর্ণাঙ্ক openpt_allocate(পূর্ণাঙ্ক flags, বর্ণ **ret_peer_path);
পূর্ণাঙ্ক openpt_allocate_in_namespace(ধ্রুব PidRef *pidref, পূর্ণাঙ্ক flags, বর্ণ **ret_peer_path);

পূর্ণাঙ্ক vt_restore(পূর্ণাঙ্ক fd);
পূর্ণাঙ্ক vt_release(পূর্ণাঙ্ক fd, bool restore);

শূন্য get_log_colors(পূর্ণাঙ্ক priority, ধ্রুব বর্ণ **on, ধ্রুব বর্ণ **off, ধ্রুব বর্ণ **highlight);

/* Assume TTY_MODE is defined in config.h. Also, this assumes there is a 'tty' group. */
assert_cc((TTY_MODE & ~0666) == 0);
assert_cc((TTY_MODE & 0711) == 0600);

শূন্য termios_disable_echo(সংরচনা termios *termios);

/* A termios sentinel with all flag fields set to all-ones-bits. No real tcgetattr() result will ever
 * match this because no real terminal configuration uses all-ones in every flag field simultaneously. */
#define TERMIOS_NULL (সংরচনা termios) {         \
        .c_iflag = UINT_MAX,                    \
        .c_oflag = UINT_MAX,                    \
        .c_cflag = UINT_MAX,                    \
        .c_lflag = UINT_MAX,                    \
}

প্রকারনির্দেশ সংরচনা TermiosResetContext {
        পূর্ণাঙ্ক *fd;
        সংরচনা termios *termios;
} TermiosResetContext;

শূন্য termios_reset(ধ্রুব TermiosResetContext *c);

#define CLEANUP_TERMIOS_RESET(_fd, _termios)                                   \
        _cleanup_(termios_reset) _unused_ ধ্রুব TermiosResetContext            \
                CONCATENATE(_cleanup_termios_, UNIQ) = {                       \
                        .fd = &(_fd),                                          \
                        .termios = &(_termios),                                \
                }

/* The $TERM value we use for terminals other than the Linux console */
#define FALLBACK_TERM "vt220"

পূর্ণাঙ্ক get_default_background_color(দ্বিগুণ *ret_red, দ্বিগুণ *ret_green, দ্বিগুণ *ret_blue);
পূর্ণাঙ্ক terminal_get_size(পূর্ণাঙ্ক প্রবেশ_fd, পূর্ণাঙ্ক output_fd, unsigned *ret_rows, unsigned *ret_columns, bool try_dsr, bool try_csi18);
পূর্ণাঙ্ক terminal_fix_size(পূর্ণাঙ্ক প্রবেশ_fd, পূর্ণাঙ্ক output_fd);

পূর্ণাঙ্ক terminal_get_terminfo_by_dcs(পূর্ণাঙ্ক fd, বর্ণ **ret_name);
পূর্ণাঙ্ক have_terminfo_file(ধ্রুব বর্ণ *name);
পূর্ণাঙ্ক query_term_for_tty(ধ্রুব বর্ণ *tty, বর্ণ **ret_term);

পূর্ণাঙ্ক terminal_is_pty_fd(পূর্ণাঙ্ক fd);

পূর্ণাঙ্ক pty_open_peer(পূর্ণাঙ্ক fd, পূর্ণাঙ্ক mode);

স্থির অন্তঃস্থ bool osc_char_is_valid(বর্ণ c) {
        /* Checks whether the specified character is safe to be included inside an ANSI OSC sequence, as per
         * ECMA-48 5th edition, section 8.3.89 */
        প্রত্যাগম (unsigned বর্ণ) c >= 32 && (unsigned বর্ণ) c < 127;
}

#define VTNR_MAX 63

স্থির অন্তঃস্থ bool vtnr_is_valid(unsigned n) {
        প্রত্যাগম n >= 1 && n <= VTNR_MAX;
}
