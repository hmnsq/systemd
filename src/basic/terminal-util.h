/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#അന്തര്ഭാവ "forward.h"

/* Erase characters until the end of the line */
#define ANSI_ERASE_TO_END_OF_വരി "\x1B[K"

/* Erase characters until end of screen */
#define ANSI_ERASE_TO_END_OF_SCREEN "\x1B[J"

/* Move cursor up one line */
#define ANSI_REVERSE_LINEFEED "\x1BM"

/* Set cursor to top left corner and clear screen */
#define ANSI_HOME_CLEAR "\x1B[H\x1B[2J"

/* Push/pop a window title off the stack of window titles */
#define ANSI_WINDOW_TITLE_തള്ളുക "\x1b[22;2t"
#define ANSI_WINDOW_TITLE_എടുക്കുക "\x1b[23;2t"

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

bool isatty_safe(പൂർണ്ണസംഖ്യ fd);

പൂർണ്ണസംഖ്യ terminal_reset_ansi_seq(പൂർണ്ണസംഖ്യ fd);

തരനിർവചനം എണ്ണൽ TerminalResetFlags {
        TERMINAL_RESET_SWITCH_TO_TEXT = 1 << 0,
        TERMINAL_RESET_AVOID_ANSI_SEQ = 1 << 1,
        TERMINAL_RESET_FORCE_ANSI_SEQ = 1 << 2,
} TerminalResetFlags;

പൂർണ്ണസംഖ്യ terminal_reset_defensive(പൂർണ്ണസംഖ്യ fd, TerminalResetFlags flags);
പൂർണ്ണസംഖ്യ terminal_reset_defensive_locked(പൂർണ്ണസംഖ്യ fd, TerminalResetFlags flags);

പൂർണ്ണസംഖ്യ terminal_set_cursor_position(പൂർണ്ണസംഖ്യ fd, unsigned row, unsigned column);
പൂർണ്ണസംഖ്യ terminal_get_cursor_position(പൂർണ്ണസംഖ്യ നിവേശം_fd, പൂർണ്ണസംഖ്യ output_fd, unsigned *ret_rows, unsigned *ret_column);

പൂർണ്ണസംഖ്യ open_terminal(സ്ഥിരം അക്ഷരം *name, പൂർണ്ണസംഖ്യ mode);

/* Flags for tweaking the way we become the controlling process of a terminal. */
തരനിർവചനം എണ്ണൽ AcquireTerminalFlags {
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

പൂർണ്ണസംഖ്യ acquire_terminal(സ്ഥിരം അക്ഷരം *name, AcquireTerminalFlags flags, usec_t timeout);
പൂർണ്ണസംഖ്യ release_terminal(ശൂന്യം);

പൂർണ്ണസംഖ്യ terminal_new_session(ശൂന്യം);
ശൂന്യം terminal_detach_session(ശൂന്യം);

പൂർണ്ണസംഖ്യ terminal_vhangup_fd(പൂർണ്ണസംഖ്യ fd);
പൂർണ്ണസംഖ്യ terminal_vhangup(സ്ഥിരം അക്ഷരം *tty);

പൂർണ്ണസംഖ്യ terminal_set_size_fd(പൂർണ്ണസംഖ്യ fd, സ്ഥിരം അക്ഷരം *ident, unsigned rows, unsigned cols);
പൂർണ്ണസംഖ്യ proc_cmdline_tty_size(സ്ഥിരം അക്ഷരം *tty, unsigned *ret_rows, unsigned *ret_cols);

പൂർണ്ണസംഖ്യ chvt(പൂർണ്ണസംഖ്യ vt);

പൂർണ്ണസംഖ്യ read_one_char(FILE *f, അക്ഷരം *ret, usec_t timeout, bool echo, bool *need_nl);
പൂർണ്ണസംഖ്യ ask_char(അക്ഷരം *ret, സ്ഥിരം അക്ഷരം *replies, സ്ഥിരം അക്ഷരം *fmt, ...) _printf_(3, 4);

തരനിർവചനം എണ്ണൽ GetCompletionsFlags {
        /* Only return the items subject to preselection: typically you want to suppress meta entries such as
         * "list" or alias entries if this flag is set. */
        GET_COMPLETIONS_PRESELECT = 1 << 0,
} GetCompletionsFlags;

തരനിർവചനം പൂർണ്ണസംഖ്യ (*GetCompletionsCallback)(സ്ഥിരം അക്ഷരം *താക്കോൽ, GetCompletionsFlags flags, അക്ഷരം ***ret_list, ശൂന്യം *userdata);
പൂർണ്ണസംഖ്യ ask_string_full(അക്ഷരം **ret, സ്ഥിരം അക്ഷരം *prefill, GetCompletionsCallback get_completions, ശൂന്യം *userdata, സ്ഥിരം അക്ഷരം *text, ...) _printf_(5, 6);
#define ask_string(ret, text, ...) ask_string_full(ret, ശൂന്യ, ശൂന്യ, ശൂന്യ, text, ##__VA_ARGS__)

bool any_താക്കോൽ_to_proceed(ശൂന്യം);
പൂർണ്ണസംഖ്യ show_menu(അക്ഷരം **x, size_t n_columns, size_t column_width, unsigned ellipsize_percentage, സ്ഥിരം അക്ഷരം *grey_prefix, bool with_numbers);

പൂർണ്ണസംഖ്യ vt_disallocate(സ്ഥിരം അക്ഷരം *tty_path);

പൂർണ്ണസംഖ്യ resolve_dev_console(അക്ഷരം **ret);
പൂർണ്ണസംഖ്യ get_kernel_consoles(അക്ഷരം ***ret);
bool tty_is_vc(സ്ഥിരം അക്ഷരം *tty);
bool tty_is_vc_resolve(സ്ഥിരം അക്ഷരം *tty);
bool tty_is_console(സ്ഥിരം അക്ഷരം *tty) _pure_;
പൂർണ്ണസംഖ്യ vtnr_from_tty(സ്ഥിരം അക്ഷരം *tty);

ശൂന്യം reset_dev_console_fd(പൂർണ്ണസംഖ്യ fd, bool switch_to_text);
പൂർണ്ണസംഖ്യ lock_dev_console(ശൂന്യം);
പൂർണ്ണസംഖ്യ make_console_stdio(bool switch_to_text);

പൂർണ്ണസംഖ്യ getenv_columns(ശൂന്യം);
പൂർണ്ണസംഖ്യ fd_columns(പൂർണ്ണസംഖ്യ fd);
unsigned columns(ശൂന്യം);
പൂർണ്ണസംഖ്യ fd_വരികൾ(പൂർണ്ണസംഖ്യ fd);
unsigned വരികൾ(ശൂന്യം);

ശൂന്യം columns_വരികൾ_cache_reset(പൂർണ്ണസംഖ്യ _unused_ signum);
ശൂന്യം reset_terminal_feature_caches(ശൂന്യം);

bool on_tty(ശൂന്യം);
bool term_env_valid(സ്ഥിരം അക്ഷരം *term);
bool getenv_terminal_is_dumb(ശൂന്യം);
bool terminal_is_dumb(ശൂന്യം);

bool dev_console_colors_enabled(ശൂന്യം);

പൂർണ്ണസംഖ്യ get_ctty_devnr(pid_t pid, dev_t *ret);
പൂർണ്ണസംഖ്യ get_ctty(pid_t pid, dev_t *ret_devnr, അക്ഷരം **ret);

പൂർണ്ണസംഖ്യ getttyname_malloc(പൂർണ്ണസംഖ്യ fd, അക്ഷരം **ret);
പൂർണ്ണസംഖ്യ getttyname_harder(പൂർണ്ണസംഖ്യ fd, അക്ഷരം **ret);

പൂർണ്ണസംഖ്യ ptsname_malloc(പൂർണ്ണസംഖ്യ fd, അക്ഷരം **ret);

പൂർണ്ണസംഖ്യ openpt_allocate(പൂർണ്ണസംഖ്യ flags, അക്ഷരം **ret_peer_path);
പൂർണ്ണസംഖ്യ openpt_allocate_in_namespace(സ്ഥിരം PidRef *pidref, പൂർണ്ണസംഖ്യ flags, അക്ഷരം **ret_peer_path);

പൂർണ്ണസംഖ്യ vt_restore(പൂർണ്ണസംഖ്യ fd);
പൂർണ്ണസംഖ്യ vt_release(പൂർണ്ണസംഖ്യ fd, bool restore);

ശൂന്യം get_log_colors(പൂർണ്ണസംഖ്യ priority, സ്ഥിരം അക്ഷരം **on, സ്ഥിരം അക്ഷരം **off, സ്ഥിരം അക്ഷരം **highlight);

/* Assume TTY_MODE is defined in config.h. Also, this assumes there is a 'tty' group. */
assert_cc((TTY_MODE & ~0666) == 0);
assert_cc((TTY_MODE & 0711) == 0600);

ശൂന്യം termios_disable_echo(ഘടന termios *termios);

/* A termios sentinel with all flag fields set to all-ones-bits. No real tcgetattr() result will ever
 * match this because no real terminal configuration uses all-ones in every flag field simultaneously. */
#define TERMIOS_NULL (ഘടന termios) {         \
        .c_iflag = UINT_MAX,                    \
        .c_oflag = UINT_MAX,                    \
        .c_cflag = UINT_MAX,                    \
        .c_lflag = UINT_MAX,                    \
}

തരനിർവചനം ഘടന TermiosResetContext {
        പൂർണ്ണസംഖ്യ *fd;
        ഘടന termios *termios;
} TermiosResetContext;

ശൂന്യം termios_reset(സ്ഥിരം TermiosResetContext *c);

#define CLEANUP_TERMIOS_RESET(_fd, _termios)                                   \
        _cleanup_(termios_reset) _unused_ സ്ഥിരം TermiosResetContext            \
                CONCATENATE(_cleanup_termios_, UNIQ) = {                       \
                        .fd = &(_fd),                                          \
                        .termios = &(_termios),                                \
                }

/* The $TERM value we use for terminals other than the Linux console */
#define FALLBACK_TERM "vt220"

പൂർണ്ണസംഖ്യ get_default_background_color(ഇരട്ട *ret_red, ഇരട്ട *ret_green, ഇരട്ട *ret_blue);
പൂർണ്ണസംഖ്യ terminal_get_size(പൂർണ്ണസംഖ്യ നിവേശം_fd, പൂർണ്ണസംഖ്യ output_fd, unsigned *ret_rows, unsigned *ret_columns, bool try_dsr, bool try_csi18);
പൂർണ്ണസംഖ്യ terminal_fix_size(പൂർണ്ണസംഖ്യ നിവേശം_fd, പൂർണ്ണസംഖ്യ output_fd);

പൂർണ്ണസംഖ്യ terminal_get_terminfo_by_dcs(പൂർണ്ണസംഖ്യ fd, അക്ഷരം **ret_name);
പൂർണ്ണസംഖ്യ have_terminfo_file(സ്ഥിരം അക്ഷരം *name);
പൂർണ്ണസംഖ്യ query_term_for_tty(സ്ഥിരം അക്ഷരം *tty, അക്ഷരം **ret_term);

പൂർണ്ണസംഖ്യ terminal_is_pty_fd(പൂർണ്ണസംഖ്യ fd);

പൂർണ്ണസംഖ്യ pty_open_peer(പൂർണ്ണസംഖ്യ fd, പൂർണ്ണസംഖ്യ mode);

സ്ഥിതം വരിയിൽ bool osc_char_is_valid(അക്ഷരം c) {
        /* Checks whether the specified character is safe to be included inside an ANSI OSC sequence, as per
         * ECMA-48 5th edition, section 8.3.89 */
        തിരികെ (unsigned അക്ഷരം) c >= 32 && (unsigned അക്ഷരം) c < 127;
}

#define VTNR_MAX 63

സ്ഥിതം വരിയിൽ bool vtnr_is_valid(unsigned n) {
        തിരികെ n >= 1 && n <= VTNR_MAX;
}
