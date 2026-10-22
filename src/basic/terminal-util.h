/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#شامل "forward.h"

/* Erase characters until the end of the line */
#تعریف_کرو ANSI_ERASE_مقصد_انتہا_کا_سطر "\x1B[K"

/* Erase characters until end of screen */
#تعریف_کرو ANSI_ERASE_مقصد_انتہا_کا_SCREEN "\x1B[J"

/* Move cursor up one line */
#تعریف_کرو ANSI_الٹ_LINEFEED "\x1BM"

/* Set cursor to top left corner and clear screen */
#تعریف_کرو ANSI_HOME_CLEAR "\x1B[H\x1B[2J"

/* Push/pop a window title off the stack of window titles */
#تعریف_کرو ANSI_کھڑکی_TITLE_ڈالو "\x1b[22;2t"
#تعریف_کرو ANSI_کھڑکی_TITLE_نکالو "\x1b[23;2t"

/* The "device control string" ("DCS") start sequence */
#تعریف_کرو ANSI_DCS "\eP"

/* The "operating system command" ("OSC") start sequence */
#تعریف_کرو ANSI_OSC "\e]"

/* ANSI "string terminator" character ("ST"). Terminal emulators typically allow three different ones: 0x07,
 * 0x9c, and 0x1B 0x5C. We'll avoid 0x07 (BEL, aka ^G) since it might trigger unexpected TTY signal handling.
 * And we'll avoid 0x9c since that's also valid regular codepoint in UTF-8 and elsewhere, and creates
 * ambiguities. Because of that some terminal emulators explicitly choose not to support it. Hence we use
 * 0x1B 0x5c. */
#تعریف_کرو ANSI_ST "\e\\"

bool isatty_محفوظ(صحیح fd);

صحیح terminal_reset_ansi_seq(صحیح fd);

قسمتعریف شمار TerminalResetFlags {
        TERMINAL_RESET_SWITCH_مقصد_متن = 1 << 0,
        TERMINAL_RESET_AVOID_ANSI_SEQ = 1 << 1,
        TERMINAL_RESET_FORCE_ANSI_SEQ = 1 << 2,
} TerminalResetFlags;

صحیح terminal_reset_defensive(صحیح fd, TerminalResetFlags flags);
صحیح terminal_reset_defensive_locked(صحیح fd, TerminalResetFlags flags);

صحیح terminal_سیٹ_cursor_position(صحیح fd, غیرعلامتی صف, غیرعلامتی عمود);
صحیح terminal_get_cursor_position(صحیح داخلہ_fd, صحیح output_fd, غیرعلامتی *ret_صفیں, غیرعلامتی *ret_عمود);

صحیح open_terminal(ثابت حرف *name, صحیح منوال);

/* Flags for tweaking the way we become the controlling process of a terminal. */
قسمتعریف شمار AcquireTerminalFlags {
        /* Try to become the controlling process of the TTY. If we can't return -EPERM. */
        ACQUIRE_TERMINAL_TRY        = 0,

        /* Tell the kernel to forcibly make us the controlling process of the TTY. Returns -EPERM if the kernel doesn't allow that. */
        ACQUIRE_TERMINAL_FORCE      = 1,

        /* If we can't become the controlling process of the TTY right-away, then wait until we can. */
        ACQUIRE_TERMINAL_WAIT       = 2,

        /* The combined mask of the above */
        ACQUIRE_TERMINAL_منوال_نقاب = ACQUIRE_TERMINAL_TRY | ACQUIRE_TERMINAL_FORCE | ACQUIRE_TERMINAL_WAIT,

        /* Pick one of the above, and then OR this flag in, in order to request permissive behaviour, if we can't become controlling process then don't mind */
        ACQUIRE_TERMINAL_PERMISSIVE = 1 << 2,

        /* Check for pending SIGTERM while waiting for inotify (SIGTERM must be blocked by caller) */
        ACQUIRE_TERMINAL_WATCH_SIGTERM = 1 << 3,
} AcquireTerminalFlags;

صحیح acquire_terminal(ثابت حرف *name, AcquireTerminalFlags flags, usec_t timeout);
صحیح release_terminal(خالی);

صحیح terminal_new_session(خالی);
خالی terminal_detach_session(خالی);

صحیح terminal_vhangup_fd(صحیح fd);
صحیح terminal_vhangup(ثابت حرف *tty);

صحیح terminal_سیٹ_سائز_fd(صحیح fd, ثابت حرف *ident, غیرعلامتی صفیں, غیرعلامتی ستون_تعداد);
صحیح proc_cmdline_tty_سائز(ثابت حرف *tty, غیرعلامتی *ret_صفیں, غیرعلامتی *ret_ستون_تعداد);

صحیح chvt(صحیح vt);

صحیح read_one_char(FILE *f, حرف *ret, usec_t timeout, bool echo, bool *need_nl);
صحیح ask_char(حرف *ret, ثابت حرف *replies, ثابت حرف *fmt, ...) _printf_(3, 4);

قسمتعریف شمار GetCompletionsFlags {
        /* Only return the items subject to preselection: typically you want to suppress meta entries such as
         * "list" or alias entries if this flag is set. */
        GET_COMPLETIONS_PRESELECT = 1 << 0,
} GetCompletionsFlags;

قسمتعریف صحیح (*GetCompletionsCallback)(ثابت حرف *کلید, GetCompletionsFlags flags, حرف ***ret_list, خالی *userdata);
صحیح ask_string_بھرا(حرف **ret, ثابت حرف *prefill, GetCompletionsCallback get_completions, خالی *userdata, ثابت حرف *متن, ...) _printf_(5, 6);
#تعریف_کرو ask_string(ret, متن, ...) ask_string_بھرا(ret, تہی, تہی, تہی, متن, ##__VA_ARGS__)

bool any_کلید_مقصد_proceed(خالی);
صحیح show_menu(حرف **x, size_t n_columns, size_t عمود_چوڑائی, غیرعلامتی ellipsize_percentage, ثابت حرف *grey_سابقہ, bool with_numbers);

صحیح vt_disallocate(ثابت حرف *tty_راستہ);

صحیح resolve_dev_console(حرف **ret);
صحیح get_kernel_consoles(حرف ***ret);
bool tty_is_vc(ثابت حرف *tty);
bool tty_is_vc_resolve(ثابت حرف *tty);
bool tty_is_console(ثابت حرف *tty) خالص;
صحیح vtnr_from_tty(ثابت حرف *tty);

خالی reset_dev_console_fd(صحیح fd, bool switch_مقصد_متن);
صحیح تالا_dev_console(خالی);
صحیح make_console_stdio(bool switch_مقصد_متن);

صحیح getenv_columns(خالی);
صحیح fd_columns(صحیح fd);
غیرعلامتی columns(خالی);
صحیح fd_سطریں(صحیح fd);
غیرعلامتی سطریں(خالی);

خالی columns_سطریں_cache_reset(صحیح غیر_مستعمل signum);
خالی reset_terminal_feature_caches(خالی);

bool on_tty(خالی);
bool رکن_env_valid(ثابت حرف *رکن);
bool getenv_terminal_is_dumb(خالی);
bool terminal_is_dumb(خالی);

bool dev_console_colors_enabled(خالی);

صحیح get_ctty_devnr(pid_t pid, dev_t *ret);
صحیح get_ctty(pid_t pid, dev_t *ret_devnr, حرف **ret);

صحیح getttyname_malloc(صحیح fd, حرف **ret);
صحیح getttyname_harder(صحیح fd, حرف **ret);

صحیح ptsname_malloc(صحیح fd, حرف **ret);

صحیح openpt_allocate(صحیح flags, حرف **ret_peer_راستہ);
صحیح openpt_allocate_اندر_namespace(ثابت PidRef *pidref, صحیح flags, حرف **ret_peer_راستہ);

صحیح vt_restore(صحیح fd);
صحیح vt_release(صحیح fd, bool restore);

خالی get_log_colors(صحیح priority, ثابت حرف **on, ثابت حرف **off, ثابت حرف **highlight);

/* Assume TTY_MODE is defined in config.h. Also, this assumes there is a 'tty' group. */
assert_cc((TTY_منوال & ~0666) == 0);
assert_cc((TTY_منوال & 0711) == 0600);

خالی termios_disable_echo(ساخت termios *termios);

/* A termios sentinel with all flag fields set to all-ones-bits. No real tcgetattr() result will ever
 * match this because no real terminal configuration uses all-ones in every flag field simultaneously. */
#تعریف_کرو TERMIOS_NULL (ساخت termios) {         \
        .c_iflag = UINT_MAX,                    \
        .c_oflag = UINT_MAX,                    \
        .c_cflag = UINT_MAX,                    \
        .c_lflag = UINT_MAX,                    \
}

قسمتعریف ساخت TermiosResetContext {
        صحیح *fd;
        ساخت termios *termios;
} TermiosResetContext;

خالی termios_reset(ثابت TermiosResetContext *c);

#تعریف_کرو CLEANUP_TERMIOS_RESET(_fd, _termios)                                   \
        _cleanup_(termios_reset) غیر_مستعمل ثابت TermiosResetContext            \
                CONCATENATE(_cleanup_termios_, UNIQ) = {                       \
                        .fd = &(_fd),                                          \
                        .termios = &(_termios),                                \
                }

/* The $TERM value we use for terminals other than the Linux console */
#تعریف_کرو FALLBACK_رکن "vt220"

صحیح get_default_background_color(دوہرا *ret_red, دوہرا *ret_green, دوہرا *ret_blue);
صحیح terminal_get_سائز(صحیح داخلہ_fd, صحیح output_fd, غیرعلامتی *ret_صفیں, غیرعلامتی *ret_columns, bool try_dsr, bool try_csi18);
صحیح terminal_fix_سائز(صحیح داخلہ_fd, صحیح output_fd);

صحیح terminal_get_terminfo_by_dcs(صحیح fd, حرف **ret_name);
صحیح have_terminfo_file(ثابت حرف *name);
صحیح query_رکن_for_tty(ثابت حرف *tty, حرف **ret_رکن);

صحیح terminal_is_pty_fd(صحیح fd);

صحیح pty_open_peer(صحیح fd, صحیح منوال);

ساکن اندرونی bool osc_char_is_valid(حرف c) {
        /* Checks whether the specified character is safe to be included inside an ANSI OSC sequence, as per
         * ECMA-48 5th edition, section 8.3.89 */
        واپس (غیرعلامتی حرف) c >= 32 && (غیرعلامتی حرف) c < 127;
}

#تعریف_کرو VTNR_MAX 63

ساکن اندرونی bool vtnr_is_valid(غیرعلامتی n) {
        واپس n >= 1 && n <= VTNR_MAX;
}
