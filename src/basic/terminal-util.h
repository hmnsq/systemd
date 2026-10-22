/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#அந்தர்பாவ "forward.h"

/* Erase characters until the end of the line */
#define ANSI_ERASE_TO_END_OF_வரி "\x1B[K"

/* Erase characters until end of screen */
#define ANSI_ERASE_TO_END_OF_SCREEN "\x1B[J"

/* Move cursor up one line */
#define ANSI_REVERSE_LINEFEED "\x1BM"

/* Set cursor to top left corner and clear screen */
#define ANSI_HOME_CLEAR "\x1B[H\x1B[2J"

/* Push/pop a window title off the stack of window titles */
#define ANSI_WINDOW_TITLE_தள்ளு "\x1b[22;2t"
#define ANSI_WINDOW_TITLE_எடு "\x1b[23;2t"

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

bool isatty_safe(முழு fd);

முழு terminal_reset_ansi_seq(முழு fd);

வகைவரை எண்ணிக்கை TerminalResetFlags {
        TERMINAL_RESET_SWITCH_TO_TEXT = 1 << 0,
        TERMINAL_RESET_AVOID_ANSI_SEQ = 1 << 1,
        TERMINAL_RESET_FORCE_ANSI_SEQ = 1 << 2,
} TerminalResetFlags;

முழு terminal_reset_defensive(முழு fd, TerminalResetFlags flags);
முழு terminal_reset_defensive_locked(முழு fd, TerminalResetFlags flags);

முழு terminal_set_cursor_position(முழு fd, unsigned row, unsigned column);
முழு terminal_get_cursor_position(முழு உள்ளீடு_fd, முழு output_fd, unsigned *ret_rows, unsigned *ret_column);

முழு open_terminal(மாறிலி எழுத்து *name, முழு mode);

/* Flags for tweaking the way we become the controlling process of a terminal. */
வகைவரை எண்ணிக்கை AcquireTerminalFlags {
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

முழு acquire_terminal(மாறிலி எழுத்து *name, AcquireTerminalFlags flags, usec_t timeout);
முழு release_terminal(வெறுமை);

முழு terminal_new_session(வெறுமை);
வெறுமை terminal_detach_session(வெறுமை);

முழு terminal_vhangup_fd(முழு fd);
முழு terminal_vhangup(மாறிலி எழுத்து *tty);

முழு terminal_set_size_fd(முழு fd, மாறிலி எழுத்து *ident, unsigned rows, unsigned cols);
முழு proc_cmdline_tty_size(மாறிலி எழுத்து *tty, unsigned *ret_rows, unsigned *ret_cols);

முழு chvt(முழு vt);

முழு read_one_char(FILE *f, எழுத்து *ret, usec_t timeout, bool echo, bool *need_nl);
முழு ask_char(எழுத்து *ret, மாறிலி எழுத்து *replies, மாறிலி எழுத்து *fmt, ...) _printf_(3, 4);

வகைவரை எண்ணிக்கை GetCompletionsFlags {
        /* Only return the items subject to preselection: typically you want to suppress meta entries such as
         * "list" or alias entries if this flag is set. */
        GET_COMPLETIONS_PRESELECT = 1 << 0,
} GetCompletionsFlags;

வகைவரை முழு (*GetCompletionsCallback)(மாறிலி எழுத்து *சாவி, GetCompletionsFlags flags, எழுத்து ***ret_list, வெறுமை *userdata);
முழு ask_string_full(எழுத்து **ret, மாறிலி எழுத்து *prefill, GetCompletionsCallback get_completions, வெறுமை *userdata, மாறிலி எழுத்து *text, ...) _printf_(5, 6);
#define ask_string(ret, text, ...) ask_string_full(ret, வெற்று, வெற்று, வெற்று, text, ##__VA_ARGS__)

bool any_சாவி_to_proceed(வெறுமை);
முழு show_menu(எழுத்து **x, size_t n_columns, size_t column_width, unsigned ellipsize_percentage, மாறிலி எழுத்து *grey_prefix, bool with_numbers);

முழு vt_disallocate(மாறிலி எழுத்து *tty_path);

முழு resolve_dev_console(எழுத்து **ret);
முழு get_kernel_consoles(எழுத்து ***ret);
bool tty_is_vc(மாறிலி எழுத்து *tty);
bool tty_is_vc_resolve(மாறிலி எழுத்து *tty);
bool tty_is_console(மாறிலி எழுத்து *tty) _pure_;
முழு vtnr_from_tty(மாறிலி எழுத்து *tty);

வெறுமை reset_dev_console_fd(முழு fd, bool switch_to_text);
முழு lock_dev_console(வெறுமை);
முழு make_console_stdio(bool switch_to_text);

முழு getenv_columns(வெறுமை);
முழு fd_columns(முழு fd);
unsigned columns(வெறுமை);
முழு fd_வரிகள்(முழு fd);
unsigned வரிகள்(வெறுமை);

வெறுமை columns_வரிகள்_cache_reset(முழு _unused_ signum);
வெறுமை reset_terminal_feature_caches(வெறுமை);

bool on_tty(வெறுமை);
bool term_env_valid(மாறிலி எழுத்து *term);
bool getenv_terminal_is_dumb(வெறுமை);
bool terminal_is_dumb(வெறுமை);

bool dev_console_colors_enabled(வெறுமை);

முழு get_ctty_devnr(pid_t pid, dev_t *ret);
முழு get_ctty(pid_t pid, dev_t *ret_devnr, எழுத்து **ret);

முழு getttyname_malloc(முழு fd, எழுத்து **ret);
முழு getttyname_harder(முழு fd, எழுத்து **ret);

முழு ptsname_malloc(முழு fd, எழுத்து **ret);

முழு openpt_allocate(முழு flags, எழுத்து **ret_peer_path);
முழு openpt_allocate_in_namespace(மாறிலி PidRef *pidref, முழு flags, எழுத்து **ret_peer_path);

முழு vt_restore(முழு fd);
முழு vt_release(முழு fd, bool restore);

வெறுமை get_log_colors(முழு priority, மாறிலி எழுத்து **on, மாறிலி எழுத்து **off, மாறிலி எழுத்து **highlight);

/* Assume TTY_MODE is defined in config.h. Also, this assumes there is a 'tty' group. */
assert_cc((TTY_MODE & ~0666) == 0);
assert_cc((TTY_MODE & 0711) == 0600);

வெறுமை termios_disable_echo(கட்டமைப்பு termios *termios);

/* A termios sentinel with all flag fields set to all-ones-bits. No real tcgetattr() result will ever
 * match this because no real terminal configuration uses all-ones in every flag field simultaneously. */
#define TERMIOS_NULL (கட்டமைப்பு termios) {         \
        .c_iflag = UINT_MAX,                    \
        .c_oflag = UINT_MAX,                    \
        .c_cflag = UINT_MAX,                    \
        .c_lflag = UINT_MAX,                    \
}

வகைவரை கட்டமைப்பு TermiosResetContext {
        முழு *fd;
        கட்டமைப்பு termios *termios;
} TermiosResetContext;

வெறுமை termios_reset(மாறிலி TermiosResetContext *c);

#define CLEANUP_TERMIOS_RESET(_fd, _termios)                                   \
        _cleanup_(termios_reset) _unused_ மாறிலி TermiosResetContext            \
                CONCATENATE(_cleanup_termios_, UNIQ) = {                       \
                        .fd = &(_fd),                                          \
                        .termios = &(_termios),                                \
                }

/* The $TERM value we use for terminals other than the Linux console */
#define FALLBACK_TERM "vt220"

முழு get_default_background_color(இரட்டை *ret_red, இரட்டை *ret_green, இரட்டை *ret_blue);
முழு terminal_get_size(முழு உள்ளீடு_fd, முழு output_fd, unsigned *ret_rows, unsigned *ret_columns, bool try_dsr, bool try_csi18);
முழு terminal_fix_size(முழு உள்ளீடு_fd, முழு output_fd);

முழு terminal_get_terminfo_by_dcs(முழு fd, எழுத்து **ret_name);
முழு have_terminfo_file(மாறிலி எழுத்து *name);
முழு query_term_for_tty(மாறிலி எழுத்து *tty, எழுத்து **ret_term);

முழு terminal_is_pty_fd(முழு fd);

முழு pty_open_peer(முழு fd, முழு mode);

நிலையான உள்வரி bool osc_char_is_valid(எழுத்து c) {
        /* Checks whether the specified character is safe to be included inside an ANSI OSC sequence, as per
         * ECMA-48 5th edition, section 8.3.89 */
        திரும்பு (unsigned எழுத்து) c >= 32 && (unsigned எழுத்து) c < 127;
}

#define VTNR_MAX 63

நிலையான உள்வரி bool vtnr_is_valid(unsigned n) {
        திரும்பு n >= 1 && n <= VTNR_MAX;
}
