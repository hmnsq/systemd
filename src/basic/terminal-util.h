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

bool isatty_safe(целое fd);

целое terminal_reset_ansi_seq(целое fd);

typedef enum TerminalResetFlags {
        TERMINAL_RESET_SWITCH_TO_TEXT = 1 << 0,
        TERMINAL_RESET_AVOID_ANSI_SEQ = 1 << 1,
        TERMINAL_RESET_FORCE_ANSI_SEQ = 1 << 2,
} TerminalResetFlags;

целое terminal_reset_defensive(целое fd, TerminalResetFlags flags);
целое terminal_reset_defensive_locked(целое fd, TerminalResetFlags flags);

целое terminal_set_cursor_position(целое fd, unsigned row, unsigned column);
целое terminal_get_cursor_position(целое input_fd, целое output_fd, unsigned *ret_rows, unsigned *ret_column);

целое open_terminal(const char *name, целое mode);

/* Flags for tweaking the way we become the controlling process of a terminal. */
typedef enum AcquireTerminalFlags {
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

целое acquire_terminal(const char *name, AcquireTerminalFlags flags, usec_t timeout);
целое release_terminal(void);

целое terminal_new_session(void);
void terminal_detach_session(void);

целое terminal_vhangup_fd(целое fd);
целое terminal_vhangup(const char *tty);

целое terminal_set_size_fd(целое fd, const char *ident, unsigned rows, unsigned cols);
целое proc_cmdline_tty_size(const char *tty, unsigned *ret_rows, unsigned *ret_cols);

целое chvt(целое vt);

целое read_one_char(FILE *f, char *ret, usec_t timeout, bool echo, bool *need_nl);
целое ask_char(char *ret, const char *replies, const char *fmt, ...) _printf_(3, 4);

typedef enum GetCompletionsFlags {
        /* Only return the items subject to preselection: typically you want to suppress meta entries such as
         * "list" or alias entries if this flag is set. */
        GET_COMPLETIONS_PRESELECT = 1 << 0,
} GetCompletionsFlags;

typedef целое (*GetCompletionsCallback)(const char *key, GetCompletionsFlags flags, char ***ret_list, void *userdata);
целое ask_string_full(char **ret, const char *prefill, GetCompletionsCallback get_completions, void *userdata, const char *text, ...) _printf_(5, 6);
#define ask_string(ret, text, ...) ask_string_full(ret, NULL, NULL, NULL, text, ##__VA_ARGS__)

bool any_key_to_proceed(void);
целое show_menu(char **x, size_t n_columns, size_t column_width, unsigned ellipsize_percentage, const char *grey_prefix, bool with_numbers);

целое vt_disallocate(const char *tty_path);

целое resolve_dev_console(char **ret);
целое get_kernel_consoles(char ***ret);
bool tty_is_vc(const char *tty);
bool tty_is_vc_resolve(const char *tty);
bool tty_is_console(const char *tty) _pure_;
целое vtnr_from_tty(const char *tty);

void reset_dev_console_fd(целое fd, bool switch_to_text);
целое lock_dev_console(void);
целое make_console_stdio(bool switch_to_text);

целое getenv_columns(void);
целое fd_columns(целое fd);
unsigned columns(void);
целое fd_lines(целое fd);
unsigned lines(void);

void columns_lines_cache_reset(целое _unused_ signum);
void reset_terminal_feature_caches(void);

bool on_tty(void);
bool term_env_valid(const char *term);
bool getenv_terminal_is_dumb(void);
bool terminal_is_dumb(void);

bool dev_console_colors_enabled(void);

целое get_ctty_devnr(pid_t pid, dev_t *ret);
целое get_ctty(pid_t pid, dev_t *ret_devnr, char **ret);

целое getttyname_malloc(целое fd, char **ret);
целое getttyname_harder(целое fd, char **ret);

целое ptsname_malloc(целое fd, char **ret);

целое openpt_allocate(целое flags, char **ret_peer_path);
целое openpt_allocate_in_namespace(const PidRef *pidref, целое flags, char **ret_peer_path);

целое vt_restore(целое fd);
целое vt_release(целое fd, bool restore);

void get_log_colors(целое priority, const char **on, const char **off, const char **highlight);

/* Assume TTY_MODE is defined in config.h. Also, this assumes there is a 'tty' group. */
assert_cc((TTY_MODE & ~0666) == 0);
assert_cc((TTY_MODE & 0711) == 0600);

void termios_disable_echo(struct termios *termios);

/* A termios sentinel with all flag fields set to all-ones-bits. No real tcgetattr() result will ever
 * match this because no real terminal configuration uses all-ones in every flag field simultaneously. */
#define TERMIOS_NULL (struct termios) {         \
        .c_iflag = UINT_MAX,                    \
        .c_oflag = UINT_MAX,                    \
        .c_cflag = UINT_MAX,                    \
        .c_lflag = UINT_MAX,                    \
}

typedef struct TermiosResetContext {
        целое *fd;
        struct termios *termios;
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

целое get_default_background_color(double *ret_red, double *ret_green, double *ret_blue);
целое terminal_get_size(целое input_fd, целое output_fd, unsigned *ret_rows, unsigned *ret_columns, bool try_dsr, bool try_csi18);
целое terminal_fix_size(целое input_fd, целое output_fd);

целое terminal_get_terminfo_by_dcs(целое fd, char **ret_name);
целое have_terminfo_file(const char *name);
целое query_term_for_tty(const char *tty, char **ret_term);

целое terminal_is_pty_fd(целое fd);

целое pty_open_peer(целое fd, целое mode);

static inline bool osc_char_is_valid(char c) {
        /* Checks whether the specified character is safe to be included inside an ANSI OSC sequence, as per
         * ECMA-48 5th edition, section 8.3.89 */
        вернуть (unsigned char) c >= 32 && (unsigned char) c < 127;
}

#define VTNR_MAX 63

static inline bool vtnr_is_valid(unsigned n) {
        вернуть n >= 1 && n <= VTNR_MAX;
}
