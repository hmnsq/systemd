/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ଅନ୍ତର୍ଭାଵ "forward.h"

/* Erase characters until the end of the line */
#define ANSI_ERASE_TO_END_OF_ଧାଡ଼ି "\x1B[K"

/* Erase characters until end of screen */
#define ANSI_ERASE_TO_END_OF_SCREEN "\x1B[J"

/* Move cursor up one line */
#define ANSI_REVERSE_LINEFEED "\x1BM"

/* Set cursor to top left corner and clear screen */
#define ANSI_HOME_CLEAR "\x1B[H\x1B[2J"

/* Push/pop a window title off the stack of window titles */
#define ANSI_WINDOW_TITLE_ଠେଲ "\x1b[22;2t"
#define ANSI_WINDOW_TITLE_କାଢ଼ "\x1b[23;2t"

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

bool isatty_safe(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା terminal_reset_ansi_seq(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd);

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା TerminalResetFlags {
        TERMINAL_RESET_SWITCH_TO_TEXT = 1 << 0,
        TERMINAL_RESET_AVOID_ANSI_SEQ = 1 << 1,
        TERMINAL_RESET_FORCE_ANSI_SEQ = 1 << 2,
} TerminalResetFlags;

ପୂର୍ଣ୍ଣସଂଖ୍ୟା terminal_reset_defensive(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, TerminalResetFlags flags);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା terminal_reset_defensive_locked(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, TerminalResetFlags flags);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା terminal_set_cursor_position(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, unsigned row, unsigned column);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା terminal_get_cursor_position(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ନିବେଶ_fd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା output_fd, unsigned *ret_rows, unsigned *ret_column);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା open_terminal(ଅଚଳ ଅକ୍ଷର *name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା mode);

/* Flags for tweaking the way we become the controlling process of a terminal. */
ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା AcquireTerminalFlags {
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

ପୂର୍ଣ୍ଣସଂଖ୍ୟା acquire_terminal(ଅଚଳ ଅକ୍ଷର *name, AcquireTerminalFlags flags, usec_t timeout);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା release_terminal(ଶୂନ୍ୟ);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା terminal_new_session(ଶୂନ୍ୟ);
ଶୂନ୍ୟ terminal_detach_session(ଶୂନ୍ୟ);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା terminal_vhangup_fd(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା terminal_vhangup(ଅଚଳ ଅକ୍ଷର *tty);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା terminal_set_size_fd(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ଅଚଳ ଅକ୍ଷର *ident, unsigned rows, unsigned cols);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା proc_cmdline_tty_size(ଅଚଳ ଅକ୍ଷର *tty, unsigned *ret_rows, unsigned *ret_cols);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା chvt(ପୂର୍ଣ୍ଣସଂଖ୍ୟା vt);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା read_one_char(FILE *f, ଅକ୍ଷର *ret, usec_t timeout, bool echo, bool *need_nl);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା ask_char(ଅକ୍ଷର *ret, ଅଚଳ ଅକ୍ଷର *replies, ଅଚଳ ଅକ୍ଷର *fmt, ...) _printf_(3, 4);

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା GetCompletionsFlags {
        /* Only return the items subject to preselection: typically you want to suppress meta entries such as
         * "list" or alias entries if this flag is set. */
        GET_COMPLETIONS_PRESELECT = 1 << 0,
} GetCompletionsFlags;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*GetCompletionsCallback)(ଅଚଳ ଅକ୍ଷର *ଚାବି, GetCompletionsFlags flags, ଅକ୍ଷର ***ret_list, ଶୂନ୍ୟ *userdata);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା ask_string_full(ଅକ୍ଷର **ret, ଅଚଳ ଅକ୍ଷର *prefill, GetCompletionsCallback get_completions, ଶୂନ୍ୟ *userdata, ଅଚଳ ଅକ୍ଷର *text, ...) _printf_(5, 6);
#define ask_string(ret, text, ...) ask_string_full(ret, ଖାଲି, ଖାଲି, ଖାଲି, text, ##__VA_ARGS__)

bool any_ଚାବି_to_proceed(ଶୂନ୍ୟ);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା show_menu(ଅକ୍ଷର **x, size_t n_columns, size_t column_width, unsigned ellipsize_percentage, ଅଚଳ ଅକ୍ଷର *grey_prefix, bool with_numbers);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା vt_disallocate(ଅଚଳ ଅକ୍ଷର *tty_path);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା resolve_dev_console(ଅକ୍ଷର **ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_kernel_consoles(ଅକ୍ଷର ***ret);
bool tty_is_vc(ଅଚଳ ଅକ୍ଷର *tty);
bool tty_is_vc_resolve(ଅଚଳ ଅକ୍ଷର *tty);
bool tty_is_console(ଅଚଳ ଅକ୍ଷର *tty) _pure_;
ପୂର୍ଣ୍ଣସଂଖ୍ୟା vtnr_from_tty(ଅଚଳ ଅକ୍ଷର *tty);

ଶୂନ୍ୟ reset_dev_console_fd(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, bool switch_to_text);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା lock_dev_console(ଶୂନ୍ୟ);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା make_console_stdio(bool switch_to_text);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା getenv_columns(ଶୂନ୍ୟ);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd_columns(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd);
unsigned columns(ଶୂନ୍ୟ);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd_ଧାଡ଼ିଗୁଡ଼ିକ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd);
unsigned ଧାଡ଼ିଗୁଡ଼ିକ(ଶୂନ୍ୟ);

ଶୂନ୍ୟ columns_ଧାଡ଼ିଗୁଡ଼ିକ_cache_reset(ପୂର୍ଣ୍ଣସଂଖ୍ୟା _unused_ signum);
ଶୂନ୍ୟ reset_terminal_feature_caches(ଶୂନ୍ୟ);

bool on_tty(ଶୂନ୍ୟ);
bool term_env_valid(ଅଚଳ ଅକ୍ଷର *term);
bool getenv_terminal_is_dumb(ଶୂନ୍ୟ);
bool terminal_is_dumb(ଶୂନ୍ୟ);

bool dev_console_colors_enabled(ଶୂନ୍ୟ);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_ctty_devnr(pid_t pid, dev_t *ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_ctty(pid_t pid, dev_t *ret_devnr, ଅକ୍ଷର **ret);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା getttyname_malloc(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ଅକ୍ଷର **ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା getttyname_harder(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ଅକ୍ଷର **ret);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା ptsname_malloc(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ଅକ୍ଷର **ret);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା openpt_allocate(ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags, ଅକ୍ଷର **ret_peer_path);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା openpt_allocate_in_namespace(ଅଚଳ PidRef *pidref, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags, ଅକ୍ଷର **ret_peer_path);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା vt_restore(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା vt_release(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, bool restore);

ଶୂନ୍ୟ get_log_colors(ପୂର୍ଣ୍ଣସଂଖ୍ୟା priority, ଅଚଳ ଅକ୍ଷର **on, ଅଚଳ ଅକ୍ଷର **off, ଅଚଳ ଅକ୍ଷର **highlight);

/* Assume TTY_MODE is defined in config.h. Also, this assumes there is a 'tty' group. */
assert_cc((TTY_MODE & ~0666) == 0);
assert_cc((TTY_MODE & 0711) == 0600);

ଶୂନ୍ୟ termios_disable_echo(ସଂରଚନା termios *termios);

/* A termios sentinel with all flag fields set to all-ones-bits. No real tcgetattr() result will ever
 * match this because no real terminal configuration uses all-ones in every flag field simultaneously. */
#define TERMIOS_NULL (ସଂରଚନା termios) {         \
        .c_iflag = UINT_MAX,                    \
        .c_oflag = UINT_MAX,                    \
        .c_cflag = UINT_MAX,                    \
        .c_lflag = UINT_MAX,                    \
}

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା TermiosResetContext {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା *fd;
        ସଂରଚନା termios *termios;
} TermiosResetContext;

ଶୂନ୍ୟ termios_reset(ଅଚଳ TermiosResetContext *c);

#define CLEANUP_TERMIOS_RESET(_fd, _termios)                                   \
        _cleanup_(termios_reset) _unused_ ଅଚଳ TermiosResetContext            \
                CONCATENATE(_cleanup_termios_, UNIQ) = {                       \
                        .fd = &(_fd),                                          \
                        .termios = &(_termios),                                \
                }

/* The $TERM value we use for terminals other than the Linux console */
#define FALLBACK_TERM "vt220"

ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_default_background_color(ଦ୍ୱିଗୁଣ *ret_red, ଦ୍ୱିଗୁଣ *ret_green, ଦ୍ୱିଗୁଣ *ret_blue);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା terminal_get_size(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ନିବେଶ_fd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା output_fd, unsigned *ret_rows, unsigned *ret_columns, bool try_dsr, bool try_csi18);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା terminal_fix_size(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ନିବେଶ_fd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା output_fd);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା terminal_get_terminfo_by_dcs(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ଅକ୍ଷର **ret_name);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା have_terminfo_file(ଅଚଳ ଅକ୍ଷର *name);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା query_term_for_tty(ଅଚଳ ଅକ୍ଷର *tty, ଅକ୍ଷର **ret_term);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା terminal_is_pty_fd(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା pty_open_peer(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା mode);

ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool osc_char_is_valid(ଅକ୍ଷର c) {
        /* Checks whether the specified character is safe to be included inside an ANSI OSC sequence, as per
         * ECMA-48 5th edition, section 8.3.89 */
        ଫେରନ୍ତୁ (unsigned ଅକ୍ଷର) c >= 32 && (unsigned ଅକ୍ଷର) c < 127;
}

#define VTNR_MAX 63

ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool vtnr_is_valid(unsigned n) {
        ଫେରନ୍ତୁ n >= 1 && n <= VTNR_MAX;
}
