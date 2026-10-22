/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#અન્તર્ભાવ "forward.h"

/* Erase characters until the end of the line */
#define ANSI_ERASE_TO_END_OF_લીટી "\x1B[K"

/* Erase characters until end of screen */
#define ANSI_ERASE_TO_END_OF_SCREEN "\x1B[J"

/* Move cursor up one line */
#define ANSI_REVERSE_LINEFEED "\x1BM"

/* Set cursor to top left corner and clear screen */
#define ANSI_HOME_CLEAR "\x1B[H\x1B[2J"

/* Push/pop a window title off the stack of window titles */
#define ANSI_WINDOW_TITLE_ધકેલો "\x1b[22;2t"
#define ANSI_WINDOW_TITLE_કાઢો "\x1b[23;2t"

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

bool isatty_safe(પૂર્ણાંક fd);

પૂર્ણાંક terminal_reset_ansi_seq(પૂર્ણાંક fd);

પ્રકારવ્યાખ્યા ગણના TerminalResetFlags {
        TERMINAL_RESET_SWITCH_TO_TEXT = 1 << 0,
        TERMINAL_RESET_AVOID_ANSI_SEQ = 1 << 1,
        TERMINAL_RESET_FORCE_ANSI_SEQ = 1 << 2,
} TerminalResetFlags;

પૂર્ણાંક terminal_reset_defensive(પૂર્ણાંક fd, TerminalResetFlags flags);
પૂર્ણાંક terminal_reset_defensive_locked(પૂર્ણાંક fd, TerminalResetFlags flags);

પૂર્ણાંક terminal_set_cursor_position(પૂર્ણાંક fd, unsigned row, unsigned column);
પૂર્ણાંક terminal_get_cursor_position(પૂર્ણાંક આગત_fd, પૂર્ણાંક output_fd, unsigned *ret_rows, unsigned *ret_column);

પૂર્ણાંક open_terminal(અચલ અક્ષર *name, પૂર્ણાંક mode);

/* Flags for tweaking the way we become the controlling process of a terminal. */
પ્રકારવ્યાખ્યા ગણના AcquireTerminalFlags {
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

પૂર્ણાંક acquire_terminal(અચલ અક્ષર *name, AcquireTerminalFlags flags, usec_t timeout);
પૂર્ણાંક release_terminal(શૂન્ય);

પૂર્ણાંક terminal_new_session(શૂન્ય);
શૂન્ય terminal_detach_session(શૂન્ય);

પૂર્ણાંક terminal_vhangup_fd(પૂર્ણાંક fd);
પૂર્ણાંક terminal_vhangup(અચલ અક્ષર *tty);

પૂર્ણાંક terminal_set_size_fd(પૂર્ણાંક fd, અચલ અક્ષર *ident, unsigned rows, unsigned cols);
પૂર્ણાંક proc_cmdline_tty_size(અચલ અક્ષર *tty, unsigned *ret_rows, unsigned *ret_cols);

પૂર્ણાંક chvt(પૂર્ણાંક vt);

પૂર્ણાંક read_one_char(FILE *f, અક્ષર *ret, usec_t timeout, bool echo, bool *need_nl);
પૂર્ણાંક ask_char(અક્ષર *ret, અચલ અક્ષર *replies, અચલ અક્ષર *fmt, ...) _printf_(3, 4);

પ્રકારવ્યાખ્યા ગણના GetCompletionsFlags {
        /* Only return the items subject to preselection: typically you want to suppress meta entries such as
         * "list" or alias entries if this flag is set. */
        GET_COMPLETIONS_PRESELECT = 1 << 0,
} GetCompletionsFlags;

પ્રકારવ્યાખ્યા પૂર્ણાંક (*GetCompletionsCallback)(અચલ અક્ષર *ચાવી, GetCompletionsFlags flags, અક્ષર ***ret_list, શૂન્ય *userdata);
પૂર્ણાંક ask_string_full(અક્ષર **ret, અચલ અક્ષર *prefill, GetCompletionsCallback get_completions, શૂન્ય *userdata, અચલ અક્ષર *text, ...) _printf_(5, 6);
#define ask_string(ret, text, ...) ask_string_full(ret, ખાલી, ખાલી, ખાલી, text, ##__VA_ARGS__)

bool any_ચાવી_to_proceed(શૂન્ય);
પૂર્ણાંક show_menu(અક્ષર **x, size_t n_columns, size_t column_width, unsigned ellipsize_percentage, અચલ અક્ષર *grey_prefix, bool with_numbers);

પૂર્ણાંક vt_disallocate(અચલ અક્ષર *tty_path);

પૂર્ણાંક resolve_dev_console(અક્ષર **ret);
પૂર્ણાંક get_kernel_consoles(અક્ષર ***ret);
bool tty_is_vc(અચલ અક્ષર *tty);
bool tty_is_vc_resolve(અચલ અક્ષર *tty);
bool tty_is_console(અચલ અક્ષર *tty) _pure_;
પૂર્ણાંક vtnr_from_tty(અચલ અક્ષર *tty);

શૂન્ય reset_dev_console_fd(પૂર્ણાંક fd, bool switch_to_text);
પૂર્ણાંક lock_dev_console(શૂન્ય);
પૂર્ણાંક make_console_stdio(bool switch_to_text);

પૂર્ણાંક getenv_columns(શૂન્ય);
પૂર્ણાંક fd_columns(પૂર્ણાંક fd);
unsigned columns(શૂન્ય);
પૂર્ણાંક fd_લીટીઓ(પૂર્ણાંક fd);
unsigned લીટીઓ(શૂન્ય);

શૂન્ય columns_લીટીઓ_cache_reset(પૂર્ણાંક _unused_ signum);
શૂન્ય reset_terminal_feature_caches(શૂન્ય);

bool on_tty(શૂન્ય);
bool term_env_valid(અચલ અક્ષર *term);
bool getenv_terminal_is_dumb(શૂન્ય);
bool terminal_is_dumb(શૂન્ય);

bool dev_console_colors_enabled(શૂન્ય);

પૂર્ણાંક get_ctty_devnr(pid_t pid, dev_t *ret);
પૂર્ણાંક get_ctty(pid_t pid, dev_t *ret_devnr, અક્ષર **ret);

પૂર્ણાંક getttyname_malloc(પૂર્ણાંક fd, અક્ષર **ret);
પૂર્ણાંક getttyname_harder(પૂર્ણાંક fd, અક્ષર **ret);

પૂર્ણાંક ptsname_malloc(પૂર્ણાંક fd, અક્ષર **ret);

પૂર્ણાંક openpt_allocate(પૂર્ણાંક flags, અક્ષર **ret_peer_path);
પૂર્ણાંક openpt_allocate_in_namespace(અચલ PidRef *pidref, પૂર્ણાંક flags, અક્ષર **ret_peer_path);

પૂર્ણાંક vt_restore(પૂર્ણાંક fd);
પૂર્ણાંક vt_release(પૂર્ણાંક fd, bool restore);

શૂન્ય get_log_colors(પૂર્ણાંક priority, અચલ અક્ષર **on, અચલ અક્ષર **off, અચલ અક્ષર **highlight);

/* Assume TTY_MODE is defined in config.h. Also, this assumes there is a 'tty' group. */
assert_cc((TTY_MODE & ~0666) == 0);
assert_cc((TTY_MODE & 0711) == 0600);

શૂન્ય termios_disable_echo(સંરચના termios *termios);

/* A termios sentinel with all flag fields set to all-ones-bits. No real tcgetattr() result will ever
 * match this because no real terminal configuration uses all-ones in every flag field simultaneously. */
#define TERMIOS_NULL (સંરચના termios) {         \
        .c_iflag = UINT_MAX,                    \
        .c_oflag = UINT_MAX,                    \
        .c_cflag = UINT_MAX,                    \
        .c_lflag = UINT_MAX,                    \
}

પ્રકારવ્યાખ્યા સંરચના TermiosResetContext {
        પૂર્ણાંક *fd;
        સંરચના termios *termios;
} TermiosResetContext;

શૂન્ય termios_reset(અચલ TermiosResetContext *c);

#define CLEANUP_TERMIOS_RESET(_fd, _termios)                                   \
        _cleanup_(termios_reset) _unused_ અચલ TermiosResetContext            \
                CONCATENATE(_cleanup_termios_, UNIQ) = {                       \
                        .fd = &(_fd),                                          \
                        .termios = &(_termios),                                \
                }

/* The $TERM value we use for terminals other than the Linux console */
#define FALLBACK_TERM "vt220"

પૂર્ણાંક get_default_background_color(દ્વિગુણ *ret_red, દ્વિગુણ *ret_green, દ્વિગુણ *ret_blue);
પૂર્ણાંક terminal_get_size(પૂર્ણાંક આગત_fd, પૂર્ણાંક output_fd, unsigned *ret_rows, unsigned *ret_columns, bool try_dsr, bool try_csi18);
પૂર્ણાંક terminal_fix_size(પૂર્ણાંક આગત_fd, પૂર્ણાંક output_fd);

પૂર્ણાંક terminal_get_terminfo_by_dcs(પૂર્ણાંક fd, અક્ષર **ret_name);
પૂર્ણાંક have_terminfo_file(અચલ અક્ષર *name);
પૂર્ણાંક query_term_for_tty(અચલ અક્ષર *tty, અક્ષર **ret_term);

પૂર્ણાંક terminal_is_pty_fd(પૂર્ણાંક fd);

પૂર્ણાંક pty_open_peer(પૂર્ણાંક fd, પૂર્ણાંક mode);

સ્થિર અંતઃરેખ bool osc_char_is_valid(અક્ષર c) {
        /* Checks whether the specified character is safe to be included inside an ANSI OSC sequence, as per
         * ECMA-48 5th edition, section 8.3.89 */
        પાછા (unsigned અક્ષર) c >= 32 && (unsigned અક્ષર) c < 127;
}

#define VTNR_MAX 63

સ્થિર અંતઃરેખ bool vtnr_is_valid(unsigned n) {
        પાછા n >= 1 && n <= VTNR_MAX;
}
