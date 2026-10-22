/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"

/* Erase characters until the end of the line */
#define ANSI_ERASE_TO_END_OF_पंक्ति "\x1B[K"

/* Erase characters until end of screen */
#define ANSI_ERASE_TO_END_OF_SCREEN "\x1B[J"

/* Move cursor up one line */
#define ANSI_REVERSE_LINEFEED "\x1BM"

/* Set cursor to top left corner and clear screen */
#define ANSI_HOME_CLEAR "\x1B[H\x1B[2J"

/* Push/pop a window title off the stack of window titles */
#define ANSI_WINDOW_TITLE_धकेलो "\x1b[22;2t"
#define ANSI_WINDOW_TITLE_निकालो "\x1b[23;2t"

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

bool isatty_safe(पूर्णाङ्क fd);

पूर्णाङ्क terminal_reset_ansi_seq(पूर्णाङ्क fd);

प्रकारनिर्देश गणना TerminalResetFlags {
        TERMINAL_RESET_SWITCH_TO_TEXT = 1 << 0,
        TERMINAL_RESET_AVOID_ANSI_SEQ = 1 << 1,
        TERMINAL_RESET_FORCE_ANSI_SEQ = 1 << 2,
} TerminalResetFlags;

पूर्णाङ्क terminal_reset_defensive(पूर्णाङ्क fd, TerminalResetFlags flags);
पूर्णाङ्क terminal_reset_defensive_locked(पूर्णाङ्क fd, TerminalResetFlags flags);

पूर्णाङ्क terminal_set_cursor_position(पूर्णाङ्क fd, अचिन्हित row, अचिन्हित column);
पूर्णाङ्क terminal_get_cursor_position(पूर्णाङ्क निवेश_fd, पूर्णाङ्क output_fd, अचिन्हित *ret_rows, अचिन्हित *ret_column);

पूर्णाङ्क open_terminal(ध्रुव वर्ण *name, पूर्णाङ्क mode);

/* Flags for tweaking the way we become the controlling process of a terminal. */
प्रकारनिर्देश गणना AcquireTerminalFlags {
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

पूर्णाङ्क acquire_terminal(ध्रुव वर्ण *name, AcquireTerminalFlags flags, usec_t timeout);
पूर्णाङ्क release_terminal(शून्य);

पूर्णाङ्क terminal_new_session(शून्य);
शून्य terminal_detach_session(शून्य);

पूर्णाङ्क terminal_vhangup_fd(पूर्णाङ्क fd);
पूर्णाङ्क terminal_vhangup(ध्रुव वर्ण *tty);

पूर्णाङ्क terminal_set_size_fd(पूर्णाङ्क fd, ध्रुव वर्ण *ident, अचिन्हित rows, अचिन्हित cols);
पूर्णाङ्क proc_cmdline_tty_size(ध्रुव वर्ण *tty, अचिन्हित *ret_rows, अचिन्हित *ret_cols);

पूर्णाङ्क chvt(पूर्णाङ्क vt);

पूर्णाङ्क read_one_char(खाता *f, वर्ण *ret, usec_t timeout, bool echo, bool *need_nl);
पूर्णाङ्क ask_char(वर्ण *ret, ध्रुव वर्ण *replies, ध्रुव वर्ण *fmt, ...) _printf_(3, 4);

प्रकारनिर्देश गणना GetCompletionsFlags {
        /* Only return the items subject to preselection: typically you want to suppress meta entries such as
         * "list" or alias entries if this flag is set. */
        GET_COMPLETIONS_PRESELECT = 1 << 0,
} GetCompletionsFlags;

प्रकारनिर्देश पूर्णाङ्क (*GetCompletionsCallback)(ध्रुव वर्ण *कुंजी, GetCompletionsFlags flags, वर्ण ***ret_list, शून्य *userdata);
पूर्णाङ्क ask_string_full(वर्ण **ret, ध्रुव वर्ण *prefill, GetCompletionsCallback get_completions, शून्य *userdata, ध्रुव वर्ण *text, ...) _printf_(5, 6);
#define ask_string(ret, text, ...) ask_string_full(ret, रिक्त, रिक्त, रिक्त, text, ##__VA_ARGS__)

bool any_कुंजी_to_proceed(शून्य);
पूर्णाङ्क show_menu(वर्ण **x, माप_प्रकार n_columns, माप_प्रकार column_width, अचिन्हित ellipsize_percentage, ध्रुव वर्ण *grey_prefix, bool with_numbers);

पूर्णाङ्क vt_disallocate(ध्रुव वर्ण *tty_path);

पूर्णाङ्क resolve_dev_console(वर्ण **ret);
पूर्णाङ्क get_kernel_consoles(वर्ण ***ret);
bool tty_is_vc(ध्रुव वर्ण *tty);
bool tty_is_vc_resolve(ध्रुव वर्ण *tty);
bool tty_is_console(ध्रुव वर्ण *tty) _pure_;
पूर्णाङ्क vtnr_from_tty(ध्रुव वर्ण *tty);

शून्य reset_dev_console_fd(पूर्णाङ्क fd, bool switch_to_text);
पूर्णाङ्क lock_dev_console(शून्य);
पूर्णाङ्क make_console_stdio(bool switch_to_text);

पूर्णाङ्क getenv_columns(शून्य);
पूर्णाङ्क fd_columns(पूर्णाङ्क fd);
अचिन्हित columns(शून्य);
पूर्णाङ्क fd_पंक्तियाँ(पूर्णाङ्क fd);
अचिन्हित पंक्तियाँ(शून्य);

शून्य columns_पंक्तियाँ_cache_reset(पूर्णाङ्क _unused_ signum);
शून्य reset_terminal_feature_caches(शून्य);

bool on_tty(शून्य);
bool term_env_valid(ध्रुव वर्ण *term);
bool getenv_terminal_is_dumb(शून्य);
bool terminal_is_dumb(शून्य);

bool dev_console_colors_enabled(शून्य);

पूर्णाङ्क get_ctty_devnr(pid_t pid, dev_t *ret);
पूर्णाङ्क get_ctty(pid_t pid, dev_t *ret_devnr, वर्ण **ret);

पूर्णाङ्क getttyname_malloc(पूर्णाङ्क fd, वर्ण **ret);
पूर्णाङ्क getttyname_harder(पूर्णाङ्क fd, वर्ण **ret);

पूर्णाङ्क ptsname_malloc(पूर्णाङ्क fd, वर्ण **ret);

पूर्णाङ्क openpt_allocate(पूर्णाङ्क flags, वर्ण **ret_peer_path);
पूर्णाङ्क openpt_allocate_in_namespace(ध्रुव PidRef *pidref, पूर्णाङ्क flags, वर्ण **ret_peer_path);

पूर्णाङ्क vt_restore(पूर्णाङ्क fd);
पूर्णाङ्क vt_release(पूर्णाङ्क fd, bool restore);

शून्य get_log_colors(पूर्णाङ्क priority, ध्रुव वर्ण **on, ध्रुव वर्ण **off, ध्रुव वर्ण **highlight);

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

प्रकारनिर्देश संरचना TermiosResetContext {
        पूर्णाङ्क *fd;
        संरचना termios *termios;
} TermiosResetContext;

शून्य termios_reset(ध्रुव TermiosResetContext *c);

#define CLEANUP_TERMIOS_RESET(_fd, _termios)                                   \
        _cleanup_(termios_reset) _unused_ ध्रुव TermiosResetContext            \
                CONCATENATE(_cleanup_termios_, UNIQ) = {                       \
                        .fd = &(_fd),                                          \
                        .termios = &(_termios),                                \
                }

/* The $TERM value we use for terminals other than the Linux console */
#define FALLBACK_TERM "vt220"

पूर्णाङ्क get_default_background_color(द्विगुण *ret_red, द्विगुण *ret_green, द्विगुण *ret_blue);
पूर्णाङ्क terminal_get_size(पूर्णाङ्क निवेश_fd, पूर्णाङ्क output_fd, अचिन्हित *ret_rows, अचिन्हित *ret_columns, bool try_dsr, bool try_csi18);
पूर्णाङ्क terminal_fix_size(पूर्णाङ्क निवेश_fd, पूर्णाङ्क output_fd);

पूर्णाङ्क terminal_get_terminfo_by_dcs(पूर्णाङ्क fd, वर्ण **ret_name);
पूर्णाङ्क have_terminfo_file(ध्रुव वर्ण *name);
पूर्णाङ्क query_term_for_tty(ध्रुव वर्ण *tty, वर्ण **ret_term);

पूर्णाङ्क terminal_is_pty_fd(पूर्णाङ्क fd);

पूर्णाङ्क pty_open_peer(पूर्णाङ्क fd, पूर्णाङ्क mode);

स्थिर अन्तःस्थ bool osc_char_is_valid(वर्ण c) {
        /* Checks whether the specified character is safe to be included inside an ANSI OSC sequence, as per
         * ECMA-48 5th edition, section 8.3.89 */
        प्रत्यागम (अचिन्हित वर्ण) c >= 32 && (अचिन्हित वर्ण) c < 127;
}

#define VTNR_MAX 63

स्थिर अन्तःस्थ bool vtnr_is_valid(अचिन्हित n) {
        प्रत्यागम n >= 1 && n <= VTNR_MAX;
}
