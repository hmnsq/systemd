/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ಅನ್ತರ್ಭಾವ "forward.h"

/* Erase characters until the end of the line */
#define ANSI_ERASE_TO_END_OF_ಸಾಲು "\x1B[K"

/* Erase characters until end of screen */
#define ANSI_ERASE_TO_END_OF_SCREEN "\x1B[J"

/* Move cursor up one line */
#define ANSI_REVERSE_LINEFEED "\x1BM"

/* Set cursor to top left corner and clear screen */
#define ANSI_HOME_CLEAR "\x1B[H\x1B[2J"

/* Push/pop a window title off the stack of window titles */
#define ANSI_WINDOW_TITLE_ತಳ್ಳು "\x1b[22;2t"
#define ANSI_WINDOW_TITLE_ತೆಗೆ "\x1b[23;2t"

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

bool isatty_safe(ಪೂರ್ಣಾಙ್ಕ fd);

ಪೂರ್ಣಾಙ್ಕ terminal_reset_ansi_seq(ಪೂರ್ಣಾಙ್ಕ fd);

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ TerminalResetFlags {
        TERMINAL_RESET_SWITCH_TO_TEXT = 1 << 0,
        TERMINAL_RESET_AVOID_ANSI_SEQ = 1 << 1,
        TERMINAL_RESET_FORCE_ANSI_SEQ = 1 << 2,
} TerminalResetFlags;

ಪೂರ್ಣಾಙ್ಕ terminal_reset_defensive(ಪೂರ್ಣಾಙ್ಕ fd, TerminalResetFlags flags);
ಪೂರ್ಣಾಙ್ಕ terminal_reset_defensive_locked(ಪೂರ್ಣಾಙ್ಕ fd, TerminalResetFlags flags);

ಪೂರ್ಣಾಙ್ಕ terminal_set_cursor_position(ಪೂರ್ಣಾಙ್ಕ fd, unsigned row, unsigned column);
ಪೂರ್ಣಾಙ್ಕ terminal_get_cursor_position(ಪೂರ್ಣಾಙ್ಕ ಒಳಹರಿವು_fd, ಪೂರ್ಣಾಙ್ಕ output_fd, unsigned *ret_rows, unsigned *ret_column);

ಪೂರ್ಣಾಙ್ಕ open_terminal(ಧ್ರುವ ವರ್ಣ *name, ಪೂರ್ಣಾಙ್ಕ mode);

/* Flags for tweaking the way we become the controlling process of a terminal. */
ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ AcquireTerminalFlags {
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

ಪೂರ್ಣಾಙ್ಕ acquire_terminal(ಧ್ರುವ ವರ್ಣ *name, AcquireTerminalFlags flags, usec_t timeout);
ಪೂರ್ಣಾಙ್ಕ release_terminal(ಶೂನ್ಯ);

ಪೂರ್ಣಾಙ್ಕ terminal_new_session(ಶೂನ್ಯ);
ಶೂನ್ಯ terminal_detach_session(ಶೂನ್ಯ);

ಪೂರ್ಣಾಙ್ಕ terminal_vhangup_fd(ಪೂರ್ಣಾಙ್ಕ fd);
ಪೂರ್ಣಾಙ್ಕ terminal_vhangup(ಧ್ರುವ ವರ್ಣ *tty);

ಪೂರ್ಣಾಙ್ಕ terminal_set_size_fd(ಪೂರ್ಣಾಙ್ಕ fd, ಧ್ರುವ ವರ್ಣ *ident, unsigned rows, unsigned cols);
ಪೂರ್ಣಾಙ್ಕ proc_cmdline_tty_size(ಧ್ರುವ ವರ್ಣ *tty, unsigned *ret_rows, unsigned *ret_cols);

ಪೂರ್ಣಾಙ್ಕ chvt(ಪೂರ್ಣಾಙ್ಕ vt);

ಪೂರ್ಣಾಙ್ಕ read_one_char(FILE *f, ವರ್ಣ *ret, usec_t timeout, bool echo, bool *need_nl);
ಪೂರ್ಣಾಙ್ಕ ask_char(ವರ್ಣ *ret, ಧ್ರುವ ವರ್ಣ *replies, ಧ್ರುವ ವರ್ಣ *fmt, ...) _printf_(3, 4);

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ GetCompletionsFlags {
        /* Only return the items subject to preselection: typically you want to suppress meta entries such as
         * "list" or alias entries if this flag is set. */
        GET_COMPLETIONS_PRESELECT = 1 << 0,
} GetCompletionsFlags;

ಪ್ರಕಾರನಿರ್ದೇಶ ಪೂರ್ಣಾಙ್ಕ (*GetCompletionsCallback)(ಧ್ರುವ ವರ್ಣ *ಕೀಲಿ, GetCompletionsFlags flags, ವರ್ಣ ***ret_list, ಶೂನ್ಯ *userdata);
ಪೂರ್ಣಾಙ್ಕ ask_string_full(ವರ್ಣ **ret, ಧ್ರುವ ವರ್ಣ *prefill, GetCompletionsCallback get_completions, ಶೂನ್ಯ *userdata, ಧ್ರುವ ವರ್ಣ *text, ...) _printf_(5, 6);
#define ask_string(ret, text, ...) ask_string_full(ret, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, text, ##__VA_ARGS__)

bool any_ಕೀಲಿ_to_proceed(ಶೂನ್ಯ);
ಪೂರ್ಣಾಙ್ಕ show_menu(ವರ್ಣ **x, size_t n_columns, size_t column_width, unsigned ellipsize_percentage, ಧ್ರುವ ವರ್ಣ *grey_prefix, bool with_numbers);

ಪೂರ್ಣಾಙ್ಕ vt_disallocate(ಧ್ರುವ ವರ್ಣ *tty_path);

ಪೂರ್ಣಾಙ್ಕ resolve_dev_console(ವರ್ಣ **ret);
ಪೂರ್ಣಾಙ್ಕ get_kernel_consoles(ವರ್ಣ ***ret);
bool tty_is_vc(ಧ್ರುವ ವರ್ಣ *tty);
bool tty_is_vc_resolve(ಧ್ರುವ ವರ್ಣ *tty);
bool tty_is_console(ಧ್ರುವ ವರ್ಣ *tty) _pure_;
ಪೂರ್ಣಾಙ್ಕ vtnr_from_tty(ಧ್ರುವ ವರ್ಣ *tty);

ಶೂನ್ಯ reset_dev_console_fd(ಪೂರ್ಣಾಙ್ಕ fd, bool switch_to_text);
ಪೂರ್ಣಾಙ್ಕ lock_dev_console(ಶೂನ್ಯ);
ಪೂರ್ಣಾಙ್ಕ make_console_stdio(bool switch_to_text);

ಪೂರ್ಣಾಙ್ಕ getenv_columns(ಶೂನ್ಯ);
ಪೂರ್ಣಾಙ್ಕ fd_columns(ಪೂರ್ಣಾಙ್ಕ fd);
unsigned columns(ಶೂನ್ಯ);
ಪೂರ್ಣಾಙ್ಕ fd_ಸಾಲುಗಳು(ಪೂರ್ಣಾಙ್ಕ fd);
unsigned ಸಾಲುಗಳು(ಶೂನ್ಯ);

ಶೂನ್ಯ columns_ಸಾಲುಗಳು_cache_reset(ಪೂರ್ಣಾಙ್ಕ _unused_ signum);
ಶೂನ್ಯ reset_terminal_feature_caches(ಶೂನ್ಯ);

bool on_tty(ಶೂನ್ಯ);
bool term_env_valid(ಧ್ರುವ ವರ್ಣ *term);
bool getenv_terminal_is_dumb(ಶೂನ್ಯ);
bool terminal_is_dumb(ಶೂನ್ಯ);

bool dev_console_colors_enabled(ಶೂನ್ಯ);

ಪೂರ್ಣಾಙ್ಕ get_ctty_devnr(pid_t pid, dev_t *ret);
ಪೂರ್ಣಾಙ್ಕ get_ctty(pid_t pid, dev_t *ret_devnr, ವರ್ಣ **ret);

ಪೂರ್ಣಾಙ್ಕ getttyname_malloc(ಪೂರ್ಣಾಙ್ಕ fd, ವರ್ಣ **ret);
ಪೂರ್ಣಾಙ್ಕ getttyname_harder(ಪೂರ್ಣಾಙ್ಕ fd, ವರ್ಣ **ret);

ಪೂರ್ಣಾಙ್ಕ ptsname_malloc(ಪೂರ್ಣಾಙ್ಕ fd, ವರ್ಣ **ret);

ಪೂರ್ಣಾಙ್ಕ openpt_allocate(ಪೂರ್ಣಾಙ್ಕ flags, ವರ್ಣ **ret_peer_path);
ಪೂರ್ಣಾಙ್ಕ openpt_allocate_in_namespace(ಧ್ರುವ PidRef *pidref, ಪೂರ್ಣಾಙ್ಕ flags, ವರ್ಣ **ret_peer_path);

ಪೂರ್ಣಾಙ್ಕ vt_restore(ಪೂರ್ಣಾಙ್ಕ fd);
ಪೂರ್ಣಾಙ್ಕ vt_release(ಪೂರ್ಣಾಙ್ಕ fd, bool restore);

ಶೂನ್ಯ get_log_colors(ಪೂರ್ಣಾಙ್ಕ priority, ಧ್ರುವ ವರ್ಣ **on, ಧ್ರುವ ವರ್ಣ **off, ಧ್ರುವ ವರ್ಣ **highlight);

/* Assume TTY_MODE is defined in config.h. Also, this assumes there is a 'tty' group. */
assert_cc((TTY_MODE & ~0666) == 0);
assert_cc((TTY_MODE & 0711) == 0600);

ಶೂನ್ಯ termios_disable_echo(ಸಂರಚನಾ termios *termios);

/* A termios sentinel with all flag fields set to all-ones-bits. No real tcgetattr() result will ever
 * match this because no real terminal configuration uses all-ones in every flag field simultaneously. */
#define TERMIOS_NULL (ಸಂರಚನಾ termios) {         \
        .c_iflag = UINT_MAX,                    \
        .c_oflag = UINT_MAX,                    \
        .c_cflag = UINT_MAX,                    \
        .c_lflag = UINT_MAX,                    \
}

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ TermiosResetContext {
        ಪೂರ್ಣಾಙ್ಕ *fd;
        ಸಂರಚನಾ termios *termios;
} TermiosResetContext;

ಶೂನ್ಯ termios_reset(ಧ್ರುವ TermiosResetContext *c);

#define CLEANUP_TERMIOS_RESET(_fd, _termios)                                   \
        _cleanup_(termios_reset) _unused_ ಧ್ರುವ TermiosResetContext            \
                CONCATENATE(_cleanup_termios_, UNIQ) = {                       \
                        .fd = &(_fd),                                          \
                        .termios = &(_termios),                                \
                }

/* The $TERM value we use for terminals other than the Linux console */
#define FALLBACK_TERM "vt220"

ಪೂರ್ಣಾಙ್ಕ get_default_background_color(ದ್ವಿಗುಣ *ret_red, ದ್ವಿಗುಣ *ret_green, ದ್ವಿಗುಣ *ret_blue);
ಪೂರ್ಣಾಙ್ಕ terminal_get_size(ಪೂರ್ಣಾಙ್ಕ ಒಳಹರಿವು_fd, ಪೂರ್ಣಾಙ್ಕ output_fd, unsigned *ret_rows, unsigned *ret_columns, bool try_dsr, bool try_csi18);
ಪೂರ್ಣಾಙ್ಕ terminal_fix_size(ಪೂರ್ಣಾಙ್ಕ ಒಳಹರಿವು_fd, ಪೂರ್ಣಾಙ್ಕ output_fd);

ಪೂರ್ಣಾಙ್ಕ terminal_get_terminfo_by_dcs(ಪೂರ್ಣಾಙ್ಕ fd, ವರ್ಣ **ret_name);
ಪೂರ್ಣಾಙ್ಕ have_terminfo_file(ಧ್ರುವ ವರ್ಣ *name);
ಪೂರ್ಣಾಙ್ಕ query_term_for_tty(ಧ್ರುವ ವರ್ಣ *tty, ವರ್ಣ **ret_term);

ಪೂರ್ಣಾಙ್ಕ terminal_is_pty_fd(ಪೂರ್ಣಾಙ್ಕ fd);

ಪೂರ್ಣಾಙ್ಕ pty_open_peer(ಪೂರ್ಣಾಙ್ಕ fd, ಪೂರ್ಣಾಙ್ಕ mode);

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool osc_char_is_valid(ವರ್ಣ c) {
        /* Checks whether the specified character is safe to be included inside an ANSI OSC sequence, as per
         * ECMA-48 5th edition, section 8.3.89 */
        ಪ್ರತ್ಯಾಗಮ (unsigned ವರ್ಣ) c >= 32 && (unsigned ವರ್ಣ) c < 127;
}

#define VTNR_MAX 63

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool vtnr_is_valid(unsigned n) {
        ಪ್ರತ್ಯಾಗಮ n >= 1 && n <= VTNR_MAX;
}
