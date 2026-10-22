/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ "forward.h"

/* Erase characters until the end of the line */
#ವ್ಯಾಖ್ಯಾನಿಸು ANSI_ERASE_ಗಮ್ಯ_ಕೊನೆ_ದ_ಸಾಲು "\x1B[K"

/* Erase characters until end of screen */
#ವ್ಯಾಖ್ಯಾನಿಸು ANSI_ERASE_ಗಮ್ಯ_ಕೊನೆ_ದ_SCREEN "\x1B[J"

/* Move cursor up one line */
#ವ್ಯಾಖ್ಯಾನಿಸು ANSI_ಹಿಮ್ಮುಖ_LINEFEED "\x1BM"

/* Set cursor to top left corner and clear screen */
#ವ್ಯಾಖ್ಯಾನಿಸು ANSI_HOME_CLEAR "\x1B[H\x1B[2J"

/* Push/pop a window title off the stack of window titles */
#ವ್ಯಾಖ್ಯಾನಿಸು ANSI_ಕಿಟಕಿ_TITLE_ತಳ್ಳು "\x1b[22;2t"
#ವ್ಯಾಖ್ಯಾನಿಸು ANSI_ಕಿಟಕಿ_TITLE_ತೆಗೆ "\x1b[23;2t"

/* The "device control string" ("DCS") start sequence */
#ವ್ಯಾಖ್ಯಾನಿಸು ANSI_DCS "\eP"

/* The "operating system command" ("OSC") start sequence */
#ವ್ಯಾಖ್ಯಾನಿಸು ANSI_OSC "\e]"

/* ANSI "string terminator" character ("ST"). Terminal emulators typically allow three different ones: 0x07,
 * 0x9c, and 0x1B 0x5C. We'll avoid 0x07 (BEL, aka ^G) since it might trigger unexpected TTY signal handling.
 * And we'll avoid 0x9c since that's also valid regular codepoint in UTF-8 and elsewhere, and creates
 * ambiguities. Because of that some terminal emulators explicitly choose not to support it. Hence we use
 * 0x1B 0x5c. */
#ವ್ಯಾಖ್ಯಾನಿಸು ANSI_ST "\e\\"

bool isatty_ಸುರಕ್ಷಿತ(ಪೂರ್ಣಾಂಕ fd);

ಪೂರ್ಣಾಂಕ terminal_reset_ansi_seq(ಪೂರ್ಣಾಂಕ fd);

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ TerminalResetFlags {
        TERMINAL_RESET_SWITCH_ಗಮ್ಯ_ಪಠ್ಯ = 1 << 0,
        TERMINAL_RESET_AVOID_ANSI_SEQ = 1 << 1,
        TERMINAL_RESET_FORCE_ANSI_SEQ = 1 << 2,
} TerminalResetFlags;

ಪೂರ್ಣಾಂಕ terminal_reset_defensive(ಪೂರ್ಣಾಂಕ fd, TerminalResetFlags flags);
ಪೂರ್ಣಾಂಕ terminal_reset_defensive_locked(ಪೂರ್ಣಾಂಕ fd, TerminalResetFlags flags);

ಪೂರ್ಣಾಂಕ terminal_ಗಣ_cursor_position(ಪೂರ್ಣಾಂಕ fd, ಚಿಹ್ನೆರಹಿತ ಅಡ್ಡಸಾಲು, ಚಿಹ್ನೆರಹಿತ ಸ್ತಂಭ);
ಪೂರ್ಣಾಂಕ terminal_get_cursor_position(ಪೂರ್ಣಾಂಕ ಒಳಹರಿವು_fd, ಪೂರ್ಣಾಂಕ output_fd, ಚಿಹ್ನೆರಹಿತ *ret_ಅಡ್ಡಸಾಲುಗಳು, ಚಿಹ್ನೆರಹಿತ *ret_ಸ್ತಂಭ);

ಪೂರ್ಣಾಂಕ open_terminal(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ಪೂರ್ಣಾಂಕ ಬಹುಲಕ);

/* Flags for tweaking the way we become the controlling process of a terminal. */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ AcquireTerminalFlags {
        /* Try to become the controlling process of the TTY. If we can't return -EPERM. */
        ACQUIRE_TERMINAL_TRY        = 0,

        /* Tell the kernel to forcibly make us the controlling process of the TTY. Returns -EPERM if the kernel doesn't allow that. */
        ACQUIRE_TERMINAL_FORCE      = 1,

        /* If we can't become the controlling process of the TTY right-away, then wait until we can. */
        ACQUIRE_TERMINAL_WAIT       = 2,

        /* The combined mask of the above */
        ACQUIRE_TERMINAL_ಬಹುಲಕ_ಮುಖವಾಡ = ACQUIRE_TERMINAL_TRY | ACQUIRE_TERMINAL_FORCE | ACQUIRE_TERMINAL_WAIT,

        /* Pick one of the above, and then OR this flag in, in order to request permissive behaviour, if we can't become controlling process then don't mind */
        ACQUIRE_TERMINAL_PERMISSIVE = 1 << 2,

        /* Check for pending SIGTERM while waiting for inotify (SIGTERM must be blocked by caller) */
        ACQUIRE_TERMINAL_WATCH_SIGTERM = 1 << 3,
} AcquireTerminalFlags;

ಪೂರ್ಣಾಂಕ acquire_terminal(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, AcquireTerminalFlags flags, usec_t timeout);
ಪೂರ್ಣಾಂಕ release_terminal(ಶೂನ್ಯ);

ಪೂರ್ಣಾಂಕ terminal_new_session(ಶೂನ್ಯ);
ಶೂನ್ಯ terminal_detach_session(ಶೂನ್ಯ);

ಪೂರ್ಣಾಂಕ terminal_vhangup_fd(ಪೂರ್ಣಾಂಕ fd);
ಪೂರ್ಣಾಂಕ terminal_vhangup(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *tty);

ಪೂರ್ಣಾಂಕ terminal_ಗಣ_ಗಾತ್ರ_fd(ಪೂರ್ಣಾಂಕ fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ident, ಚಿಹ್ನೆರಹಿತ ಅಡ್ಡಸಾಲುಗಳು, ಚಿಹ್ನೆರಹಿತ ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ);
ಪೂರ್ಣಾಂಕ proc_cmdline_tty_ಗಾತ್ರ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *tty, ಚಿಹ್ನೆರಹಿತ *ret_ಅಡ್ಡಸಾಲುಗಳು, ಚಿಹ್ನೆರಹಿತ *ret_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ);

ಪೂರ್ಣಾಂಕ chvt(ಪೂರ್ಣಾಂಕ vt);

ಪೂರ್ಣಾಂಕ read_one_char(FILE *f, ಅಕ್ಷರ *ret, usec_t timeout, bool echo, bool *need_nl);
ಪೂರ್ಣಾಂಕ ask_char(ಅಕ್ಷರ *ret, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *replies, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, ...) _printf_(3, 4);

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ GetCompletionsFlags {
        /* Only return the items subject to preselection: typically you want to suppress meta entries such as
         * "list" or alias entries if this flag is set. */
        GET_COMPLETIONS_PRESELECT = 1 << 0,
} GetCompletionsFlags;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಪೂರ್ಣಾಂಕ (*GetCompletionsCallback)(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ, GetCompletionsFlags flags, ಅಕ್ಷರ ***ret_list, ಶೂನ್ಯ *userdata);
ಪೂರ್ಣಾಂಕ ask_string_ತುಂಬಿದ(ಅಕ್ಷರ **ret, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *prefill, GetCompletionsCallback get_completions, ಶೂನ್ಯ *userdata, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಪಠ್ಯ, ...) _printf_(5, 6);
#ವ್ಯಾಖ್ಯಾನಿಸು ask_string(ret, ಪಠ್ಯ, ...) ask_string_ತುಂಬಿದ(ret, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, ಪಠ್ಯ, ##__VA_ARGS__)

bool any_ಕೀಲಿ_ಗಮ್ಯ_proceed(ಶೂನ್ಯ);
ಪೂರ್ಣಾಂಕ show_menu(ಅಕ್ಷರ **x, size_t n_columns, size_t ಸ್ತಂಭ_ಅಗಲ, ಚಿಹ್ನೆರಹಿತ ellipsize_percentage, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *grey_ಪೂರ್ವಪ್ರತ್ಯಯ, bool with_numbers);

ಪೂರ್ಣಾಂಕ vt_disallocate(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *tty_ಮಾರ್ಗ);

ಪೂರ್ಣಾಂಕ resolve_dev_console(ಅಕ್ಷರ **ret);
ಪೂರ್ಣಾಂಕ get_kernel_consoles(ಅಕ್ಷರ ***ret);
bool tty_is_vc(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *tty);
bool tty_is_vc_resolve(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *tty);
bool tty_is_console(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *tty) ಶುದ್ಧ;
ಪೂರ್ಣಾಂಕ vtnr_from_tty(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *tty);

ಶೂನ್ಯ reset_dev_console_fd(ಪೂರ್ಣಾಂಕ fd, bool switch_ಗಮ್ಯ_ಪಠ್ಯ);
ಪೂರ್ಣಾಂಕ ಬೀಗ_dev_console(ಶೂನ್ಯ);
ಪೂರ್ಣಾಂಕ make_console_stdio(bool switch_ಗಮ್ಯ_ಪಠ್ಯ);

ಪೂರ್ಣಾಂಕ getenv_columns(ಶೂನ್ಯ);
ಪೂರ್ಣಾಂಕ fd_columns(ಪೂರ್ಣಾಂಕ fd);
ಚಿಹ್ನೆರಹಿತ columns(ಶೂನ್ಯ);
ಪೂರ್ಣಾಂಕ fd_ಸಾಲುಗಳು(ಪೂರ್ಣಾಂಕ fd);
ಚಿಹ್ನೆರಹಿತ ಸಾಲುಗಳು(ಶೂನ್ಯ);

ಶೂನ್ಯ columns_ಸಾಲುಗಳು_cache_reset(ಪೂರ್ಣಾಂಕ ಬಳಸದ signum);
ಶೂನ್ಯ reset_terminal_feature_caches(ಶೂನ್ಯ);

bool on_tty(ಶೂನ್ಯ);
bool ಪದ_env_valid(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಪದ);
bool getenv_terminal_is_dumb(ಶೂನ್ಯ);
bool terminal_is_dumb(ಶೂನ್ಯ);

bool dev_console_colors_enabled(ಶೂನ್ಯ);

ಪೂರ್ಣಾಂಕ get_ctty_devnr(pid_t pid, dev_t *ret);
ಪೂರ್ಣಾಂಕ get_ctty(pid_t pid, dev_t *ret_devnr, ಅಕ್ಷರ **ret);

ಪೂರ್ಣಾಂಕ getttyname_malloc(ಪೂರ್ಣಾಂಕ fd, ಅಕ್ಷರ **ret);
ಪೂರ್ಣಾಂಕ getttyname_harder(ಪೂರ್ಣಾಂಕ fd, ಅಕ್ಷರ **ret);

ಪೂರ್ಣಾಂಕ ptsname_malloc(ಪೂರ್ಣಾಂಕ fd, ಅಕ್ಷರ **ret);

ಪೂರ್ಣಾಂಕ openpt_allocate(ಪೂರ್ಣಾಂಕ flags, ಅಕ್ಷರ **ret_peer_ಮಾರ್ಗ);
ಪೂರ್ಣಾಂಕ openpt_allocate_ಒಳ_namespace(ಸ್ಥಿರಾಂಕ PidRef *pidref, ಪೂರ್ಣಾಂಕ flags, ಅಕ್ಷರ **ret_peer_ಮಾರ್ಗ);

ಪೂರ್ಣಾಂಕ vt_restore(ಪೂರ್ಣಾಂಕ fd);
ಪೂರ್ಣಾಂಕ vt_release(ಪೂರ್ಣಾಂಕ fd, bool restore);

ಶೂನ್ಯ get_log_colors(ಪೂರ್ಣಾಂಕ priority, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **on, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **off, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **highlight);

/* Assume TTY_MODE is defined in config.h. Also, this assumes there is a 'tty' group. */
assert_cc((TTY_ಬಹುಲಕ & ~0666) == 0);
assert_cc((TTY_ಬಹುಲಕ & 0711) == 0600);

ಶೂನ್ಯ termios_disable_echo(ರಚನೆ termios *termios);

/* A termios sentinel with all flag fields set to all-ones-bits. No real tcgetattr() result will ever
 * match this because no real terminal configuration uses all-ones in every flag field simultaneously. */
#ವ್ಯಾಖ್ಯಾನಿಸು TERMIOS_NULL (ರಚನೆ termios) {         \
        .c_iflag = UINT_MAX,                    \
        .c_oflag = UINT_MAX,                    \
        .c_cflag = UINT_MAX,                    \
        .c_lflag = UINT_MAX,                    \
}

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ TermiosResetContext {
        ಪೂರ್ಣಾಂಕ *fd;
        ರಚನೆ termios *termios;
} TermiosResetContext;

ಶೂನ್ಯ termios_reset(ಸ್ಥಿರಾಂಕ TermiosResetContext *c);

#ವ್ಯಾಖ್ಯಾನಿಸು CLEANUP_TERMIOS_RESET(_fd, _termios)                                   \
        _cleanup_(termios_reset) ಬಳಸದ ಸ್ಥಿರಾಂಕ TermiosResetContext            \
                CONCATENATE(_cleanup_termios_, UNIQ) = {                       \
                        .fd = &(_fd),                                          \
                        .termios = &(_termios),                                \
                }

/* The $TERM value we use for terminals other than the Linux console */
#ವ್ಯಾಖ್ಯಾನಿಸು FALLBACK_ಪದ "vt220"

ಪೂರ್ಣಾಂಕ get_default_background_color(ದ್ವಿಗುಣ *ret_red, ದ್ವಿಗುಣ *ret_green, ದ್ವಿಗುಣ *ret_blue);
ಪೂರ್ಣಾಂಕ terminal_get_ಗಾತ್ರ(ಪೂರ್ಣಾಂಕ ಒಳಹರಿವು_fd, ಪೂರ್ಣಾಂಕ output_fd, ಚಿಹ್ನೆರಹಿತ *ret_ಅಡ್ಡಸಾಲುಗಳು, ಚಿಹ್ನೆರಹಿತ *ret_columns, bool try_dsr, bool try_csi18);
ಪೂರ್ಣಾಂಕ terminal_fix_ಗಾತ್ರ(ಪೂರ್ಣಾಂಕ ಒಳಹರಿವು_fd, ಪೂರ್ಣಾಂಕ output_fd);

ಪೂರ್ಣಾಂಕ terminal_get_terminfo_by_dcs(ಪೂರ್ಣಾಂಕ fd, ಅಕ್ಷರ **ret_name);
ಪೂರ್ಣಾಂಕ have_terminfo_file(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name);
ಪೂರ್ಣಾಂಕ query_ಪದ_for_tty(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *tty, ಅಕ್ಷರ **ret_ಪದ);

ಪೂರ್ಣಾಂಕ terminal_is_pty_fd(ಪೂರ್ಣಾಂಕ fd);

ಪೂರ್ಣಾಂಕ pty_open_peer(ಪೂರ್ಣಾಂಕ fd, ಪೂರ್ಣಾಂಕ ಬಹುಲಕ);

ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool osc_char_is_valid(ಅಕ್ಷರ c) {
        /* Checks whether the specified character is safe to be included inside an ANSI OSC sequence, as per
         * ECMA-48 5th edition, section 8.3.89 */
        ಹಿಂತಿರುಗಿ (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ) c >= 32 && (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ) c < 127;
}

#ವ್ಯಾಖ್ಯಾನಿಸು VTNR_MAX 63

ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool vtnr_is_valid(ಚಿಹ್ನೆರಹಿತ n) {
        ಹಿಂತಿರುಗಿ n >= 1 && n <= VTNR_MAX;
}
