/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"

/* Erase characters until the end of the line */
#define ANSI_ERASE_TO_END_OF_ओळ "\x1B[K"

/* Erase characters until end of screen */
#define ANSI_ERASE_TO_END_OF_SCREEN "\x1B[J"

/* Move cursor up one line */
#define ANSI_REVERSE_LINEFEED "\x1BM"

/* Set cursor to top left corner and clear screen */
#define ANSI_HOME_CLEAR "\x1B[H\x1B[2J"

/* Push/pop a window title off the stack of window titles */
#define ANSI_WINDOW_TITLE_ढकला "\x1b[22;2t"
#define ANSI_WINDOW_TITLE_काढा "\x1b[23;2t"

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

bool isatty_safe(पूर्णांक fd);

पूर्णांक terminal_reset_ansi_seq(पूर्णांक fd);

प्रकारव्याख्या गणना TerminalResetFlags {
        TERMINAL_RESET_SWITCH_TO_TEXT = 1 << 0,
        TERMINAL_RESET_AVOID_ANSI_SEQ = 1 << 1,
        TERMINAL_RESET_FORCE_ANSI_SEQ = 1 << 2,
} TerminalResetFlags;

पूर्णांक terminal_reset_defensive(पूर्णांक fd, TerminalResetFlags flags);
पूर्णांक terminal_reset_defensive_locked(पूर्णांक fd, TerminalResetFlags flags);

पूर्णांक terminal_set_cursor_position(पूर्णांक fd, अचिन्हित row, अचिन्हित column);
पूर्णांक terminal_get_cursor_position(पूर्णांक आदान_fd, पूर्णांक output_fd, अचिन्हित *ret_rows, अचिन्हित *ret_column);

पूर्णांक open_terminal(स्थिरांक अक्षर *name, पूर्णांक mode);

/* Flags for tweaking the way we become the controlling process of a terminal. */
प्रकारव्याख्या गणना AcquireTerminalFlags {
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

पूर्णांक acquire_terminal(स्थिरांक अक्षर *name, AcquireTerminalFlags flags, usec_t timeout);
पूर्णांक release_terminal(शून्य);

पूर्णांक terminal_new_session(शून्य);
शून्य terminal_detach_session(शून्य);

पूर्णांक terminal_vhangup_fd(पूर्णांक fd);
पूर्णांक terminal_vhangup(स्थिरांक अक्षर *tty);

पूर्णांक terminal_set_size_fd(पूर्णांक fd, स्थिरांक अक्षर *ident, अचिन्हित rows, अचिन्हित cols);
पूर्णांक proc_cmdline_tty_size(स्थिरांक अक्षर *tty, अचिन्हित *ret_rows, अचिन्हित *ret_cols);

पूर्णांक chvt(पूर्णांक vt);

पूर्णांक read_one_char(खाता *f, अक्षर *ret, usec_t timeout, bool echo, bool *need_nl);
पूर्णांक ask_char(अक्षर *ret, स्थिरांक अक्षर *replies, स्थिरांक अक्षर *fmt, ...) _printf_(3, 4);

प्रकारव्याख्या गणना GetCompletionsFlags {
        /* Only return the items subject to preselection: typically you want to suppress meta entries such as
         * "list" or alias entries if this flag is set. */
        GET_COMPLETIONS_PRESELECT = 1 << 0,
} GetCompletionsFlags;

प्रकारव्याख्या पूर्णांक (*GetCompletionsCallback)(स्थिरांक अक्षर *किल्ली, GetCompletionsFlags flags, अक्षर ***ret_list, शून्य *userdata);
पूर्णांक ask_string_full(अक्षर **ret, स्थिरांक अक्षर *prefill, GetCompletionsCallback get_completions, शून्य *userdata, स्थिरांक अक्षर *text, ...) _printf_(5, 6);
#define ask_string(ret, text, ...) ask_string_full(ret, रिक्त, रिक्त, रिक्त, text, ##__VA_ARGS__)

bool any_किल्ली_to_proceed(शून्य);
पूर्णांक show_menu(अक्षर **x, माप_प्रकार n_columns, माप_प्रकार column_width, अचिन्हित ellipsize_percentage, स्थिरांक अक्षर *grey_prefix, bool with_numbers);

पूर्णांक vt_disallocate(स्थिरांक अक्षर *tty_path);

पूर्णांक resolve_dev_console(अक्षर **ret);
पूर्णांक get_kernel_consoles(अक्षर ***ret);
bool tty_is_vc(स्थिरांक अक्षर *tty);
bool tty_is_vc_resolve(स्थिरांक अक्षर *tty);
bool tty_is_console(स्थिरांक अक्षर *tty) _pure_;
पूर्णांक vtnr_from_tty(स्थिरांक अक्षर *tty);

शून्य reset_dev_console_fd(पूर्णांक fd, bool switch_to_text);
पूर्णांक lock_dev_console(शून्य);
पूर्णांक make_console_stdio(bool switch_to_text);

पूर्णांक getenv_columns(शून्य);
पूर्णांक fd_columns(पूर्णांक fd);
अचिन्हित columns(शून्य);
पूर्णांक fd_ओळी(पूर्णांक fd);
अचिन्हित ओळी(शून्य);

शून्य columns_ओळी_cache_reset(पूर्णांक _unused_ signum);
शून्य reset_terminal_feature_caches(शून्य);

bool on_tty(शून्य);
bool term_env_valid(स्थिरांक अक्षर *term);
bool getenv_terminal_is_dumb(शून्य);
bool terminal_is_dumb(शून्य);

bool dev_console_colors_enabled(शून्य);

पूर्णांक get_ctty_devnr(pid_t pid, dev_t *ret);
पूर्णांक get_ctty(pid_t pid, dev_t *ret_devnr, अक्षर **ret);

पूर्णांक getttyname_malloc(पूर्णांक fd, अक्षर **ret);
पूर्णांक getttyname_harder(पूर्णांक fd, अक्षर **ret);

पूर्णांक ptsname_malloc(पूर्णांक fd, अक्षर **ret);

पूर्णांक openpt_allocate(पूर्णांक flags, अक्षर **ret_peer_path);
पूर्णांक openpt_allocate_in_namespace(स्थिरांक PidRef *pidref, पूर्णांक flags, अक्षर **ret_peer_path);

पूर्णांक vt_restore(पूर्णांक fd);
पूर्णांक vt_release(पूर्णांक fd, bool restore);

शून्य get_log_colors(पूर्णांक priority, स्थिरांक अक्षर **on, स्थिरांक अक्षर **off, स्थिरांक अक्षर **highlight);

/* Assume TTY_MODE is defined in config.h. Also, this assumes there is a 'tty' group. */
assert_cc((TTY_MODE & ~0666) == 0);
assert_cc((TTY_MODE & 0711) == 0600);

शून्य termios_disable_echo(संरचना termios *termios);

/* A termios sentinel with all flag fields set to all-ones-bits. No real tcgetattr() result will ever
 * match this because no real terminal configuration uses all-ones in every flag field simultaneously. */
#define TERMIOS_NULL (संरचना termios) {         \
        .c_iflag = अच_पूर्णांक_उच्च,                    \
        .c_oflag = अच_पूर्णांक_उच्च,                    \
        .c_cflag = अच_पूर्णांक_उच्च,                    \
        .c_lflag = अच_पूर्णांक_उच्च,                    \
}

प्रकारव्याख्या संरचना TermiosResetContext {
        पूर्णांक *fd;
        संरचना termios *termios;
} TermiosResetContext;

शून्य termios_reset(स्थिरांक TermiosResetContext *c);

#define CLEANUP_TERMIOS_RESET(_fd, _termios)                                   \
        _cleanup_(termios_reset) _unused_ स्थिरांक TermiosResetContext            \
                CONCATENATE(_cleanup_termios_, UNIQ) = {                       \
                        .fd = &(_fd),                                          \
                        .termios = &(_termios),                                \
                }

/* The $TERM value we use for terminals other than the Linux console */
#define FALLBACK_TERM "vt220"

पूर्णांक get_default_background_color(द्विगुण *ret_red, द्विगुण *ret_green, द्विगुण *ret_blue);
पूर्णांक terminal_get_size(पूर्णांक आदान_fd, पूर्णांक output_fd, अचिन्हित *ret_rows, अचिन्हित *ret_columns, bool try_dsr, bool try_csi18);
पूर्णांक terminal_fix_size(पूर्णांक आदान_fd, पूर्णांक output_fd);

पूर्णांक terminal_get_terminfo_by_dcs(पूर्णांक fd, अक्षर **ret_name);
पूर्णांक have_terminfo_file(स्थिरांक अक्षर *name);
पूर्णांक query_term_for_tty(स्थिरांक अक्षर *tty, अक्षर **ret_term);

पूर्णांक terminal_is_pty_fd(पूर्णांक fd);

पूर्णांक pty_open_peer(पूर्णांक fd, पूर्णांक mode);

स्थिर ओळीत bool osc_char_is_valid(अक्षर c) {
        /* Checks whether the specified character is safe to be included inside an ANSI OSC sequence, as per
         * ECMA-48 5th edition, section 8.3.89 */
        परत (अचिन्हित अक्षर) c >= 32 && (अचिन्हित अक्षर) c < 127;
}

#define VTNR_MAX 63

स्थिर ओळीत bool vtnr_is_valid(अचिन्हित n) {
        परत n >= 1 && n <= VTNR_MAX;
}
