/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ضمن "forward.h"

/* Erase characters until the end of the line */
#define ANSI_ERASE_TO_END_OF_هێڵ "\x1B[K"

/* Erase characters until end of screen */
#define ANSI_ERASE_TO_END_OF_SCREEN "\x1B[J"

/* Move cursor up one line */
#define ANSI_REVERSE_LINEFEED "\x1BM"

/* Set cursor to top left corner and clear screen */
#define ANSI_HOME_CLEAR "\x1B[H\x1B[2J"

/* Push/pop a window title off the stack of window titles */
#define ANSI_WINDOW_TITLE_پاڵنان "\x1b[22;2t"
#define ANSI_WINDOW_TITLE_دەرهێنان "\x1b[23;2t"

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

bool isatty_safe(صحيح fd);

صحيح terminal_reset_ansi_seq(صحيح fd);

تعريفنوع تعداد TerminalResetFlags {
        TERMINAL_RESET_SWITCH_TO_TEXT = 1 << 0,
        TERMINAL_RESET_AVOID_ANSI_SEQ = 1 << 1,
        TERMINAL_RESET_FORCE_ANSI_SEQ = 1 << 2,
} TerminalResetFlags;

صحيح terminal_reset_defensive(صحيح fd, TerminalResetFlags flags);
صحيح terminal_reset_defensive_locked(صحيح fd, TerminalResetFlags flags);

صحيح terminal_set_cursor_position(صحيح fd, unsigned row, unsigned column);
صحيح terminal_get_cursor_position(صحيح تێخستن_fd, صحيح output_fd, unsigned *ret_rows, unsigned *ret_column);

صحيح open_terminal(ثابت حرف *name, صحيح mode);

/* Flags for tweaking the way we become the controlling process of a terminal. */
تعريفنوع تعداد AcquireTerminalFlags {
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

صحيح acquire_terminal(ثابت حرف *name, AcquireTerminalFlags flags, usec_t timeout);
صحيح release_terminal(فراغ);

صحيح terminal_new_session(فراغ);
فراغ terminal_detach_session(فراغ);

صحيح terminal_vhangup_fd(صحيح fd);
صحيح terminal_vhangup(ثابت حرف *tty);

صحيح terminal_set_size_fd(صحيح fd, ثابت حرف *ident, unsigned rows, unsigned cols);
صحيح proc_cmdline_tty_size(ثابت حرف *tty, unsigned *ret_rows, unsigned *ret_cols);

صحيح chvt(صحيح vt);

صحيح read_one_char(FILE *f, حرف *ret, usec_t timeout, bool echo, bool *need_nl);
صحيح ask_char(حرف *ret, ثابت حرف *replies, ثابت حرف *fmt, ...) _printf_(3, 4);

تعريفنوع تعداد GetCompletionsFlags {
        /* Only return the items subject to preselection: typically you want to suppress meta entries such as
         * "list" or alias entries if this flag is set. */
        GET_COMPLETIONS_PRESELECT = 1 << 0,
} GetCompletionsFlags;

تعريفنوع صحيح (*GetCompletionsCallback)(ثابت حرف *کلیل, GetCompletionsFlags flags, حرف ***ret_list, فراغ *userdata);
صحيح ask_string_full(حرف **ret, ثابت حرف *prefill, GetCompletionsCallback get_completions, فراغ *userdata, ثابت حرف *text, ...) _printf_(5, 6);
#define ask_string(ret, text, ...) ask_string_full(ret, عدم, عدم, عدم, text, ##__VA_ARGS__)

bool any_کلیل_to_proceed(فراغ);
صحيح show_menu(حرف **x, size_t n_columns, size_t column_width, unsigned ellipsize_percentage, ثابت حرف *grey_prefix, bool with_numbers);

صحيح vt_disallocate(ثابت حرف *tty_path);

صحيح resolve_dev_console(حرف **ret);
صحيح get_kernel_consoles(حرف ***ret);
bool tty_is_vc(ثابت حرف *tty);
bool tty_is_vc_resolve(ثابت حرف *tty);
bool tty_is_console(ثابت حرف *tty) _pure_;
صحيح vtnr_from_tty(ثابت حرف *tty);

فراغ reset_dev_console_fd(صحيح fd, bool switch_to_text);
صحيح lock_dev_console(فراغ);
صحيح make_console_stdio(bool switch_to_text);

صحيح getenv_columns(فراغ);
صحيح fd_columns(صحيح fd);
unsigned columns(فراغ);
صحيح fd_هێڵەکان(صحيح fd);
unsigned هێڵەکان(فراغ);

فراغ columns_هێڵەکان_cache_reset(صحيح _unused_ signum);
فراغ reset_terminal_feature_caches(فراغ);

bool on_tty(فراغ);
bool term_env_valid(ثابت حرف *term);
bool getenv_terminal_is_dumb(فراغ);
bool terminal_is_dumb(فراغ);

bool dev_console_colors_enabled(فراغ);

صحيح get_ctty_devnr(pid_t pid, dev_t *ret);
صحيح get_ctty(pid_t pid, dev_t *ret_devnr, حرف **ret);

صحيح getttyname_malloc(صحيح fd, حرف **ret);
صحيح getttyname_harder(صحيح fd, حرف **ret);

صحيح ptsname_malloc(صحيح fd, حرف **ret);

صحيح openpt_allocate(صحيح flags, حرف **ret_peer_path);
صحيح openpt_allocate_in_namespace(ثابت PidRef *pidref, صحيح flags, حرف **ret_peer_path);

صحيح vt_restore(صحيح fd);
صحيح vt_release(صحيح fd, bool restore);

فراغ get_log_colors(صحيح priority, ثابت حرف **on, ثابت حرف **off, ثابت حرف **highlight);

/* Assume TTY_MODE is defined in config.h. Also, this assumes there is a 'tty' group. */
assert_cc((TTY_MODE & ~0666) == 0);
assert_cc((TTY_MODE & 0711) == 0600);

فراغ termios_disable_echo(بنية termios *termios);

/* A termios sentinel with all flag fields set to all-ones-bits. No real tcgetattr() result will ever
 * match this because no real terminal configuration uses all-ones in every flag field simultaneously. */
#define TERMIOS_NULL (بنية termios) {         \
        .c_iflag = UINT_MAX,                    \
        .c_oflag = UINT_MAX,                    \
        .c_cflag = UINT_MAX,                    \
        .c_lflag = UINT_MAX,                    \
}

تعريفنوع بنية TermiosResetContext {
        صحيح *fd;
        بنية termios *termios;
} TermiosResetContext;

فراغ termios_reset(ثابت TermiosResetContext *c);

#define CLEANUP_TERMIOS_RESET(_fd, _termios)                                   \
        _cleanup_(termios_reset) _unused_ ثابت TermiosResetContext            \
                CONCATENATE(_cleanup_termios_, UNIQ) = {                       \
                        .fd = &(_fd),                                          \
                        .termios = &(_termios),                                \
                }

/* The $TERM value we use for terminals other than the Linux console */
#define FALLBACK_TERM "vt220"

صحيح get_default_background_color(مزدوج *ret_red, مزدوج *ret_green, مزدوج *ret_blue);
صحيح terminal_get_size(صحيح تێخستن_fd, صحيح output_fd, unsigned *ret_rows, unsigned *ret_columns, bool try_dsr, bool try_csi18);
صحيح terminal_fix_size(صحيح تێخستن_fd, صحيح output_fd);

صحيح terminal_get_terminfo_by_dcs(صحيح fd, حرف **ret_name);
صحيح have_terminfo_file(ثابت حرف *name);
صحيح query_term_for_tty(ثابت حرف *tty, حرف **ret_term);

صحيح terminal_is_pty_fd(صحيح fd);

صحيح pty_open_peer(صحيح fd, صحيح mode);

ساكن مضمن bool osc_char_is_valid(حرف c) {
        /* Checks whether the specified character is safe to be included inside an ANSI OSC sequence, as per
         * ECMA-48 5th edition, section 8.3.89 */
        أرجع (unsigned حرف) c >= 32 && (unsigned حرف) c < 127;
}

#define VTNR_MAX 63

ساكن مضمن bool vtnr_is_valid(unsigned n) {
        أرجع n >= 1 && n <= VTNR_MAX;
}
