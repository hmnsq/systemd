/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

/* Erase characters until the end of the line */
#define ANSI_ERASE_TO_END_OF_LINE "\x1B[K"

/* Erase characters until end of screen */
#define ANSI_ERASE_TO_END_OF_SCREEN "\x1B[J"

/* Move cursor up one line */
#define ANSI_REVERSE_LINEFEED "\x1BM"

/* Set cursor to top left corner and clear screen */
#define ANSI_HOME_CLEAR "\x1B[H\x1B[2J"

/* Push/pop a window title off the stack of window titles */
#define ANSI_WINDOW_TITLE_PUSH "\x1b[22;2t"
#define ANSI_WINDOW_TITLE_POP "\x1b[23;2t"

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

bool isatty_safe(puur_naa_nka fd);

puur_naa_nka terminal_reset_ansi_seq(puur_naa_nka fd);

typedef ga_nanaa TerminalResetFlags {
        TERMINAL_RESET_SWITCH_TO_TEXT = 1 << 0,
        TERMINAL_RESET_AVOID_ANSI_SEQ = 1 << 1,
        TERMINAL_RESET_FORCE_ANSI_SEQ = 1 << 2,
} TerminalResetFlags;

puur_naa_nka terminal_reset_defensive(puur_naa_nka fd, TerminalResetFlags flags);
puur_naa_nka terminal_reset_defensive_locked(puur_naa_nka fd, TerminalResetFlags flags);

puur_naa_nka terminal_set_cursor_position(puur_naa_nka fd, unsigned row, unsigned column);
puur_naa_nka terminal_get_cursor_position(puur_naa_nka input_fd, puur_naa_nka output_fd, unsigned *ret_rows, unsigned *ret_column);

puur_naa_nka open_terminal(const var_na *name, puur_naa_nka mode);

/* Flags for tweaking the way we become the controlling process of a terminal. */
typedef ga_nanaa AcquireTerminalFlags {
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

puur_naa_nka acquire_terminal(const var_na *name, AcquireTerminalFlags flags, usec_t timeout);
puur_naa_nka release_terminal(void);

puur_naa_nka terminal_new_session(void);
void terminal_detach_session(void);

puur_naa_nka terminal_vhangup_fd(puur_naa_nka fd);
puur_naa_nka terminal_vhangup(const var_na *tty);

puur_naa_nka terminal_set_size_fd(puur_naa_nka fd, const var_na *ident, unsigned rows, unsigned cols);
puur_naa_nka proc_cmdline_tty_size(const var_na *tty, unsigned *ret_rows, unsigned *ret_cols);

puur_naa_nka chvt(puur_naa_nka vt);

puur_naa_nka read_one_char(FILE *f, var_na *ret, usec_t timeout, bool echo, bool *need_nl);
puur_naa_nka ask_char(var_na *ret, const var_na *replies, const var_na *fmt, ...) _printf_(3, 4);

typedef ga_nanaa GetCompletionsFlags {
        /* Only return the items subject to preselection: typically you want to suppress meta entries such as
         * "list" or alias entries if this flag is set. */
        GET_COMPLETIONS_PRESELECT = 1 << 0,
} GetCompletionsFlags;

typedef puur_naa_nka (*GetCompletionsCallback)(const var_na *key, GetCompletionsFlags flags, var_na ***ret_list, void *userdata);
puur_naa_nka ask_string_full(var_na **ret, const var_na *prefill, GetCompletionsCallback get_completions, void *userdata, const var_na *text, ...) _printf_(5, 6);
#define ask_string(ret, text, ...) ask_string_full(ret, NULL, NULL, NULL, text, ##__VA_ARGS__)

bool any_key_to_proceed(void);
puur_naa_nka show_menu(var_na **x, size_t n_columns, size_t column_width, unsigned ellipsize_percentage, const var_na *grey_prefix, bool with_numbers);

puur_naa_nka vt_disallocate(const var_na *tty_path);

puur_naa_nka resolve_dev_console(var_na **ret);
puur_naa_nka get_kernel_consoles(var_na ***ret);
bool tty_is_vc(const var_na *tty);
bool tty_is_vc_resolve(const var_na *tty);
bool tty_is_console(const var_na *tty) _pure_;
puur_naa_nka vtnr_from_tty(const var_na *tty);

void reset_dev_console_fd(puur_naa_nka fd, bool switch_to_text);
puur_naa_nka lock_dev_console(void);
puur_naa_nka make_console_stdio(bool switch_to_text);

puur_naa_nka getenv_columns(void);
puur_naa_nka fd_columns(puur_naa_nka fd);
unsigned columns(void);
puur_naa_nka fd_lines(puur_naa_nka fd);
unsigned lines(void);

void columns_lines_cache_reset(puur_naa_nka _unused_ signum);
void reset_terminal_feature_caches(void);

bool on_tty(void);
bool term_env_valid(const var_na *term);
bool getenv_terminal_is_dumb(void);
bool terminal_is_dumb(void);

bool dev_console_colors_enabled(void);

puur_naa_nka get_ctty_devnr(pid_t pid, dev_t *ret);
puur_naa_nka get_ctty(pid_t pid, dev_t *ret_devnr, var_na **ret);

puur_naa_nka getttyname_malloc(puur_naa_nka fd, var_na **ret);
puur_naa_nka getttyname_harder(puur_naa_nka fd, var_na **ret);

puur_naa_nka ptsname_malloc(puur_naa_nka fd, var_na **ret);

puur_naa_nka openpt_allocate(puur_naa_nka flags, var_na **ret_peer_path);
puur_naa_nka openpt_allocate_in_namespace(const PidRef *pidref, puur_naa_nka flags, var_na **ret_peer_path);

puur_naa_nka vt_restore(puur_naa_nka fd);
puur_naa_nka vt_release(puur_naa_nka fd, bool restore);

void get_log_colors(puur_naa_nka priority, const var_na **on, const var_na **off, const var_na **highlight);

/* Assume TTY_MODE is defined in config.h. Also, this assumes there is a 'tty' group. */
assert_cc((TTY_MODE & ~0666) == 0);
assert_cc((TTY_MODE & 0711) == 0600);

void termios_disable_echo(sa_nracanaa termios *termios);

/* A termios sentinel with all flag fields set to all-ones-bits. No real tcgetattr() result will ever
 * match this because no real terminal configuration uses all-ones in every flag field simultaneously. */
#define TERMIOS_NULL (sa_nracanaa termios) {         \
        .c_iflag = UINT_MAX,                    \
        .c_oflag = UINT_MAX,                    \
        .c_cflag = UINT_MAX,                    \
        .c_lflag = UINT_MAX,                    \
}

typedef sa_nracanaa TermiosResetContext {
        puur_naa_nka *fd;
        sa_nracanaa termios *termios;
} TermiosResetContext;

void termios_reset(const TermiosResetContext *c);

#define CLEANUP_TERMIOS_RESET(_fd, _termios)                                   \
        _cleanup_(termios_reset) _unused_ const TermiosResetContext            \
                CONCATENATE(_cleanup_termios_, UNIQ) = {                       \
                        .fd = &(_fd),                                          \
                        .termios = &(_termios),                                \
                }

/* The $TERM value we use for terminals other than the Linux console */
#define FALLBACK_TERM "vt220"

puur_naa_nka get_default_background_color(dviguu_na *ret_red, dviguu_na *ret_green, dviguu_na *ret_blue);
puur_naa_nka terminal_get_size(puur_naa_nka input_fd, puur_naa_nka output_fd, unsigned *ret_rows, unsigned *ret_columns, bool try_dsr, bool try_csi18);
puur_naa_nka terminal_fix_size(puur_naa_nka input_fd, puur_naa_nka output_fd);

puur_naa_nka terminal_get_terminfo_by_dcs(puur_naa_nka fd, var_na **ret_name);
puur_naa_nka have_terminfo_file(const var_na *name);
puur_naa_nka query_term_for_tty(const var_na *tty, var_na **ret_term);

puur_naa_nka terminal_is_pty_fd(puur_naa_nka fd);

puur_naa_nka pty_open_peer(puur_naa_nka fd, puur_naa_nka mode);

static anta_hstha bool osc_char_is_valid(var_na c) {
        /* Checks whether the specified character is safe to be included inside an ANSI OSC sequence, as per
         * ECMA-48 5th edition, section 8.3.89 */
        ᤀᤡᤴ (unsigned var_na) c >= 32 && (unsigned var_na) c < 127;
}

#define VTNR_MAX 63

static anta_hstha bool vtnr_is_valid(unsigned n) {
        ᤀᤡᤴ n >= 1 && n <= VTNR_MAX;
}
