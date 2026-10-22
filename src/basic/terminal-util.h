/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"

/* Erase characters until the end of the line */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ANSI_ERASE_ਮੰਜ਼ਿਲ_ਸਿਰਾ_ਦਾ_ਸਤਰ "\x1B[K"

/* Erase characters until end of screen */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ANSI_ERASE_ਮੰਜ਼ਿਲ_ਸਿਰਾ_ਦਾ_SCREEN "\x1B[J"

/* Move cursor up one line */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ANSI_ਉਲਟ_LINEFEED "\x1BM"

/* Set cursor to top left corner and clear screen */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ANSI_HOME_CLEAR "\x1B[H\x1B[2J"

/* Push/pop a window title off the stack of window titles */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ANSI_ਖਿੜਕੀ_TITLE_ਧੱਕੋ "\x1b[22;2t"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ANSI_ਖਿੜਕੀ_TITLE_ਕੱਢੋ "\x1b[23;2t"

/* The "device control string" ("DCS") start sequence */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ANSI_DCS "\eP"

/* The "operating system command" ("OSC") start sequence */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ANSI_OSC "\e]"

/* ANSI "string terminator" character ("ST"). Terminal emulators typically allow three different ones: 0x07,
 * 0x9c, and 0x1B 0x5C. We'll avoid 0x07 (BEL, aka ^G) since it might trigger unexpected TTY signal handling.
 * And we'll avoid 0x9c since that's also valid regular codepoint in UTF-8 and elsewhere, and creates
 * ambiguities. Because of that some terminal emulators explicitly choose not to support it. Hence we use
 * 0x1B 0x5c. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ANSI_ST "\e\\"

bool isatty_ਸੁਰੱਖਿਅਤ(ਪੂਰਨ fd);

ਪੂਰਨ terminal_reset_ansi_seq(ਪੂਰਨ fd);

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ TerminalResetFlags {
        TERMINAL_RESET_SWITCH_ਮੰਜ਼ਿਲ_ਪਾਠ = 1 << 0,
        TERMINAL_RESET_AVOID_ANSI_SEQ = 1 << 1,
        TERMINAL_RESET_FORCE_ANSI_SEQ = 1 << 2,
} TerminalResetFlags;

ਪੂਰਨ terminal_reset_defensive(ਪੂਰਨ fd, TerminalResetFlags flags);
ਪੂਰਨ terminal_reset_defensive_locked(ਪੂਰਨ fd, TerminalResetFlags flags);

ਪੂਰਨ terminal_ਸੈੱਟ_cursor_position(ਪੂਰਨ fd, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੰਕਤੀ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਕਾਲਮ);
ਪੂਰਨ terminal_get_cursor_position(ਪੂਰਨ ਨਿਵੇਸ਼_fd, ਪੂਰਨ output_fd, ਬਿਨਾਨਿਸ਼ਾਨੀ *ret_ਪੰਕਤੀਆਂ, ਬਿਨਾਨਿਸ਼ਾਨੀ *ret_ਕਾਲਮ);

ਪੂਰਨ open_terminal(ਅਚਲ ਅੱਖਰ *name, ਪੂਰਨ ਬਹੁਲਕ);

/* Flags for tweaking the way we become the controlling process of a terminal. */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ AcquireTerminalFlags {
        /* Try to become the controlling process of the TTY. If we can't return -EPERM. */
        ACQUIRE_TERMINAL_TRY        = 0,

        /* Tell the kernel to forcibly make us the controlling process of the TTY. Returns -EPERM if the kernel doesn't allow that. */
        ACQUIRE_TERMINAL_FORCE      = 1,

        /* If we can't become the controlling process of the TTY right-away, then wait until we can. */
        ACQUIRE_TERMINAL_WAIT       = 2,

        /* The combined mask of the above */
        ACQUIRE_TERMINAL_ਬਹੁਲਕ_ਨਕਾਬ = ACQUIRE_TERMINAL_TRY | ACQUIRE_TERMINAL_FORCE | ACQUIRE_TERMINAL_WAIT,

        /* Pick one of the above, and then OR this flag in, in order to request permissive behaviour, if we can't become controlling process then don't mind */
        ACQUIRE_TERMINAL_PERMISSIVE = 1 << 2,

        /* Check for pending SIGTERM while waiting for inotify (SIGTERM must be blocked by caller) */
        ACQUIRE_TERMINAL_WATCH_SIGTERM = 1 << 3,
} AcquireTerminalFlags;

ਪੂਰਨ acquire_terminal(ਅਚਲ ਅੱਖਰ *name, AcquireTerminalFlags flags, usec_t timeout);
ਪੂਰਨ release_terminal(ਖਾਲੀ);

ਪੂਰਨ terminal_new_session(ਖਾਲੀ);
ਖਾਲੀ terminal_detach_session(ਖਾਲੀ);

ਪੂਰਨ terminal_vhangup_fd(ਪੂਰਨ fd);
ਪੂਰਨ terminal_vhangup(ਅਚਲ ਅੱਖਰ *tty);

ਪੂਰਨ terminal_ਸੈੱਟ_ਮਾਪ_fd(ਪੂਰਨ fd, ਅਚਲ ਅੱਖਰ *ident, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੰਕਤੀਆਂ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਖਾਨੇ_ਗਿਣਤੀ);
ਪੂਰਨ proc_cmdline_tty_ਮਾਪ(ਅਚਲ ਅੱਖਰ *tty, ਬਿਨਾਨਿਸ਼ਾਨੀ *ret_ਪੰਕਤੀਆਂ, ਬਿਨਾਨਿਸ਼ਾਨੀ *ret_ਖਾਨੇ_ਗਿਣਤੀ);

ਪੂਰਨ chvt(ਪੂਰਨ vt);

ਪੂਰਨ read_one_char(FILE *f, ਅੱਖਰ *ret, usec_t timeout, bool echo, bool *need_nl);
ਪੂਰਨ ask_char(ਅੱਖਰ *ret, ਅਚਲ ਅੱਖਰ *replies, ਅਚਲ ਅੱਖਰ *fmt, ...) _printf_(3, 4);

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ GetCompletionsFlags {
        /* Only return the items subject to preselection: typically you want to suppress meta entries such as
         * "list" or alias entries if this flag is set. */
        GET_COMPLETIONS_PRESELECT = 1 << 0,
} GetCompletionsFlags;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਪੂਰਨ (*GetCompletionsCallback)(ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ, GetCompletionsFlags flags, ਅੱਖਰ ***ret_list, ਖਾਲੀ *userdata);
ਪੂਰਨ ask_string_ਭਰਿਆ(ਅੱਖਰ **ret, ਅਚਲ ਅੱਖਰ *prefill, GetCompletionsCallback get_completions, ਖਾਲੀ *userdata, ਅਚਲ ਅੱਖਰ *ਪਾਠ, ...) _printf_(5, 6);
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ask_string(ret, ਪਾਠ, ...) ask_string_ਭਰਿਆ(ret, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, ਪਾਠ, ##__VA_ARGS__)

bool any_ਕੁੰਜੀ_ਮੰਜ਼ਿਲ_proceed(ਖਾਲੀ);
ਪੂਰਨ show_menu(ਅੱਖਰ **x, size_t n_columns, size_t ਕਾਲਮ_ਚੌੜਾਈ, ਬਿਨਾਨਿਸ਼ਾਨੀ ellipsize_percentage, ਅਚਲ ਅੱਖਰ *grey_ਅਗੇਤਰ, bool with_numbers);

ਪੂਰਨ vt_disallocate(ਅਚਲ ਅੱਖਰ *tty_ਰਸਤਾ);

ਪੂਰਨ resolve_dev_console(ਅੱਖਰ **ret);
ਪੂਰਨ get_kernel_consoles(ਅੱਖਰ ***ret);
bool tty_is_vc(ਅਚਲ ਅੱਖਰ *tty);
bool tty_is_vc_resolve(ਅਚਲ ਅੱਖਰ *tty);
bool tty_is_console(ਅਚਲ ਅੱਖਰ *tty) ਸ਼ੁੱਧ;
ਪੂਰਨ vtnr_from_tty(ਅਚਲ ਅੱਖਰ *tty);

ਖਾਲੀ reset_dev_console_fd(ਪੂਰਨ fd, bool switch_ਮੰਜ਼ਿਲ_ਪਾਠ);
ਪੂਰਨ ਤਾਲਾ_dev_console(ਖਾਲੀ);
ਪੂਰਨ make_console_stdio(bool switch_ਮੰਜ਼ਿਲ_ਪਾਠ);

ਪੂਰਨ getenv_columns(ਖਾਲੀ);
ਪੂਰਨ fd_columns(ਪੂਰਨ fd);
ਬਿਨਾਨਿਸ਼ਾਨੀ columns(ਖਾਲੀ);
ਪੂਰਨ fd_ਸਤਰਾਂ(ਪੂਰਨ fd);
ਬਿਨਾਨਿਸ਼ਾਨੀ ਸਤਰਾਂ(ਖਾਲੀ);

ਖਾਲੀ columns_ਸਤਰਾਂ_cache_reset(ਪੂਰਨ ਅਣਵਰਤਿਆ signum);
ਖਾਲੀ reset_terminal_feature_caches(ਖਾਲੀ);

bool on_tty(ਖਾਲੀ);
bool ਪਦ_env_valid(ਅਚਲ ਅੱਖਰ *ਪਦ);
bool getenv_terminal_is_dumb(ਖਾਲੀ);
bool terminal_is_dumb(ਖਾਲੀ);

bool dev_console_colors_enabled(ਖਾਲੀ);

ਪੂਰਨ get_ctty_devnr(pid_t pid, dev_t *ret);
ਪੂਰਨ get_ctty(pid_t pid, dev_t *ret_devnr, ਅੱਖਰ **ret);

ਪੂਰਨ getttyname_malloc(ਪੂਰਨ fd, ਅੱਖਰ **ret);
ਪੂਰਨ getttyname_harder(ਪੂਰਨ fd, ਅੱਖਰ **ret);

ਪੂਰਨ ptsname_malloc(ਪੂਰਨ fd, ਅੱਖਰ **ret);

ਪੂਰਨ openpt_allocate(ਪੂਰਨ flags, ਅੱਖਰ **ret_peer_ਰਸਤਾ);
ਪੂਰਨ openpt_allocate_ਵਿੱਚ_namespace(ਅਚਲ PidRef *pidref, ਪੂਰਨ flags, ਅੱਖਰ **ret_peer_ਰਸਤਾ);

ਪੂਰਨ vt_restore(ਪੂਰਨ fd);
ਪੂਰਨ vt_release(ਪੂਰਨ fd, bool restore);

ਖਾਲੀ get_log_colors(ਪੂਰਨ priority, ਅਚਲ ਅੱਖਰ **on, ਅਚਲ ਅੱਖਰ **off, ਅਚਲ ਅੱਖਰ **highlight);

/* Assume TTY_MODE is defined in config.h. Also, this assumes there is a 'tty' group. */
assert_cc((TTY_ਬਹੁਲਕ & ~0666) == 0);
assert_cc((TTY_ਬਹੁਲਕ & 0711) == 0600);

ਖਾਲੀ termios_disable_echo(ਬਣਤਰ termios *termios);

/* A termios sentinel with all flag fields set to all-ones-bits. No real tcgetattr() result will ever
 * match this because no real terminal configuration uses all-ones in every flag field simultaneously. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TERMIOS_NULL (ਬਣਤਰ termios) {         \
        .c_iflag = UINT_MAX,                    \
        .c_oflag = UINT_MAX,                    \
        .c_cflag = UINT_MAX,                    \
        .c_lflag = UINT_MAX,                    \
}

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ TermiosResetContext {
        ਪੂਰਨ *fd;
        ਬਣਤਰ termios *termios;
} TermiosResetContext;

ਖਾਲੀ termios_reset(ਅਚਲ TermiosResetContext *c);

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CLEANUP_TERMIOS_RESET(_fd, _termios)                                   \
        _cleanup_(termios_reset) ਅਣਵਰਤਿਆ ਅਚਲ TermiosResetContext            \
                CONCATENATE(_cleanup_termios_, UNIQ) = {                       \
                        .fd = &(_fd),                                          \
                        .termios = &(_termios),                                \
                }

/* The $TERM value we use for terminals other than the Linux console */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FALLBACK_ਪਦ "vt220"

ਪੂਰਨ get_default_background_color(ਦੁੱਗਣਾ *ret_red, ਦੁੱਗਣਾ *ret_green, ਦੁੱਗਣਾ *ret_blue);
ਪੂਰਨ terminal_get_ਮਾਪ(ਪੂਰਨ ਨਿਵੇਸ਼_fd, ਪੂਰਨ output_fd, ਬਿਨਾਨਿਸ਼ਾਨੀ *ret_ਪੰਕਤੀਆਂ, ਬਿਨਾਨਿਸ਼ਾਨੀ *ret_columns, bool try_dsr, bool try_csi18);
ਪੂਰਨ terminal_fix_ਮਾਪ(ਪੂਰਨ ਨਿਵੇਸ਼_fd, ਪੂਰਨ output_fd);

ਪੂਰਨ terminal_get_terminfo_by_dcs(ਪੂਰਨ fd, ਅੱਖਰ **ret_name);
ਪੂਰਨ have_terminfo_file(ਅਚਲ ਅੱਖਰ *name);
ਪੂਰਨ query_ਪਦ_for_tty(ਅਚਲ ਅੱਖਰ *tty, ਅੱਖਰ **ret_ਪਦ);

ਪੂਰਨ terminal_is_pty_fd(ਪੂਰਨ fd);

ਪੂਰਨ pty_open_peer(ਪੂਰਨ fd, ਪੂਰਨ ਬਹੁਲਕ);

ਸਥਿਰ ਸਤਰਅੰਦਰ bool osc_char_is_valid(ਅੱਖਰ c) {
        /* Checks whether the specified character is safe to be included inside an ANSI OSC sequence, as per
         * ECMA-48 5th edition, section 8.3.89 */
        ਵਾਪਸ (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ) c >= 32 && (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ) c < 127;
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VTNR_MAX 63

ਸਥਿਰ ਸਤਰਅੰਦਰ bool vtnr_is_valid(ਬਿਨਾਨਿਸ਼ਾਨੀ n) {
        ਵਾਪਸ n >= 1 && n <= VTNR_MAX;
}
