/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ "forward.h"

/* Erase characters until the end of the line */
#నిర్వచించు ANSI_ERASE_గమ్యం_చివర_యొక్క_పంక్తి "\x1B[K"

/* Erase characters until end of screen */
#నిర్వచించు ANSI_ERASE_గమ్యం_చివర_యొక్క_SCREEN "\x1B[J"

/* Move cursor up one line */
#నిర్వచించు ANSI_తిరగేయి_LINEFEED "\x1BM"

/* Set cursor to top left corner and clear screen */
#నిర్వచించు ANSI_HOME_CLEAR "\x1B[H\x1B[2J"

/* Push/pop a window title off the stack of window titles */
#నిర్వచించు ANSI_కిటికీ_TITLE_నెట్టు "\x1b[22;2t"
#నిర్వచించు ANSI_కిటికీ_TITLE_తీయి "\x1b[23;2t"

/* The "device control string" ("DCS") start sequence */
#నిర్వచించు ANSI_DCS "\eP"

/* The "operating system command" ("OSC") start sequence */
#నిర్వచించు ANSI_OSC "\e]"

/* ANSI "string terminator" character ("ST"). Terminal emulators typically allow three different ones: 0x07,
 * 0x9c, and 0x1B 0x5C. We'll avoid 0x07 (BEL, aka ^G) since it might trigger unexpected TTY signal handling.
 * And we'll avoid 0x9c since that's also valid regular codepoint in UTF-8 and elsewhere, and creates
 * ambiguities. Because of that some terminal emulators explicitly choose not to support it. Hence we use
 * 0x1B 0x5c. */
#నిర్వచించు ANSI_ST "\e\\"

bool isatty_సురక్షిత(పూర్ణాంకం fd);

పూర్ణాంకం terminal_reset_ansi_seq(పూర్ణాంకం fd);

రకనిర్వచనం గణన TerminalResetFlags {
        TERMINAL_RESET_SWITCH_గమ్యం_పాఠ్యం = 1 << 0,
        TERMINAL_RESET_AVOID_ANSI_SEQ = 1 << 1,
        TERMINAL_RESET_FORCE_ANSI_SEQ = 1 << 2,
} TerminalResetFlags;

పూర్ణాంకం terminal_reset_defensive(పూర్ణాంకం fd, TerminalResetFlags flags);
పూర్ణాంకం terminal_reset_defensive_locked(పూర్ణాంకం fd, TerminalResetFlags flags);

పూర్ణాంకం terminal_సమితి_cursor_position(పూర్ణాంకం fd, గుర్తులేని అడ్డువరుస, గుర్తులేని స్తంభం);
పూర్ణాంకం terminal_get_cursor_position(పూర్ణాంకం ఆదానం_fd, పూర్ణాంకం output_fd, గుర్తులేని *ret_అడ్డువరుసలు, గుర్తులేని *ret_స్తంభం);

పూర్ణాంకం open_terminal(స్థిరరాశి అక్షరం *name, పూర్ణాంకం బహుళకం);

/* Flags for tweaking the way we become the controlling process of a terminal. */
రకనిర్వచనం గణన AcquireTerminalFlags {
        /* Try to become the controlling process of the TTY. If we can't return -EPERM. */
        ACQUIRE_TERMINAL_TRY        = 0,

        /* Tell the kernel to forcibly make us the controlling process of the TTY. Returns -EPERM if the kernel doesn't allow that. */
        ACQUIRE_TERMINAL_FORCE      = 1,

        /* If we can't become the controlling process of the TTY right-away, then wait until we can. */
        ACQUIRE_TERMINAL_WAIT       = 2,

        /* The combined mask of the above */
        ACQUIRE_TERMINAL_బహుళకం_ముసుగు = ACQUIRE_TERMINAL_TRY | ACQUIRE_TERMINAL_FORCE | ACQUIRE_TERMINAL_WAIT,

        /* Pick one of the above, and then OR this flag in, in order to request permissive behaviour, if we can't become controlling process then don't mind */
        ACQUIRE_TERMINAL_PERMISSIVE = 1 << 2,

        /* Check for pending SIGTERM while waiting for inotify (SIGTERM must be blocked by caller) */
        ACQUIRE_TERMINAL_WATCH_SIGTERM = 1 << 3,
} AcquireTerminalFlags;

పూర్ణాంకం acquire_terminal(స్థిరరాశి అక్షరం *name, AcquireTerminalFlags flags, usec_t timeout);
పూర్ణాంకం release_terminal(శూన్య);

పూర్ణాంకం terminal_new_session(శూన్య);
శూన్య terminal_detach_session(శూన్య);

పూర్ణాంకం terminal_vhangup_fd(పూర్ణాంకం fd);
పూర్ణాంకం terminal_vhangup(స్థిరరాశి అక్షరం *tty);

పూర్ణాంకం terminal_సమితి_కొలత_fd(పూర్ణాంకం fd, స్థిరరాశి అక్షరం *ident, గుర్తులేని అడ్డువరుసలు, గుర్తులేని నిలువుల_సంఖ్య);
పూర్ణాంకం proc_cmdline_tty_కొలత(స్థిరరాశి అక్షరం *tty, గుర్తులేని *ret_అడ్డువరుసలు, గుర్తులేని *ret_నిలువుల_సంఖ్య);

పూర్ణాంకం chvt(పూర్ణాంకం vt);

పూర్ణాంకం read_one_char(FILE *f, అక్షరం *ret, usec_t timeout, bool echo, bool *need_nl);
పూర్ణాంకం ask_char(అక్షరం *ret, స్థిరరాశి అక్షరం *replies, స్థిరరాశి అక్షరం *fmt, ...) _printf_(3, 4);

రకనిర్వచనం గణన GetCompletionsFlags {
        /* Only return the items subject to preselection: typically you want to suppress meta entries such as
         * "list" or alias entries if this flag is set. */
        GET_COMPLETIONS_PRESELECT = 1 << 0,
} GetCompletionsFlags;

రకనిర్వచనం పూర్ణాంకం (*GetCompletionsCallback)(స్థిరరాశి అక్షరం *కీలకం, GetCompletionsFlags flags, అక్షరం ***ret_list, శూన్య *userdata);
పూర్ణాంకం ask_string_నిండిన(అక్షరం **ret, స్థిరరాశి అక్షరం *prefill, GetCompletionsCallback get_completions, శూన్య *userdata, స్థిరరాశి అక్షరం *పాఠ్యం, ...) _printf_(5, 6);
#నిర్వచించు ask_string(ret, పాఠ్యం, ...) ask_string_నిండిన(ret, ఖాళీ, ఖాళీ, ఖాళీ, పాఠ్యం, ##__VA_ARGS__)

bool any_కీలకం_గమ్యం_proceed(శూన్య);
పూర్ణాంకం show_menu(అక్షరం **x, size_t n_columns, size_t స్తంభం_వెడల్పు, గుర్తులేని ellipsize_percentage, స్థిరరాశి అక్షరం *grey_ఉపసర్గ, bool with_numbers);

పూర్ణాంకం vt_disallocate(స్థిరరాశి అక్షరం *tty_మార్గం);

పూర్ణాంకం resolve_dev_console(అక్షరం **ret);
పూర్ణాంకం get_kernel_consoles(అక్షరం ***ret);
bool tty_is_vc(స్థిరరాశి అక్షరం *tty);
bool tty_is_vc_resolve(స్థిరరాశి అక్షరం *tty);
bool tty_is_console(స్థిరరాశి అక్షరం *tty) శుద్ధ;
పూర్ణాంకం vtnr_from_tty(స్థిరరాశి అక్షరం *tty);

శూన్య reset_dev_console_fd(పూర్ణాంకం fd, bool switch_గమ్యం_పాఠ్యం);
పూర్ణాంకం తాళం_dev_console(శూన్య);
పూర్ణాంకం make_console_stdio(bool switch_గమ్యం_పాఠ్యం);

పూర్ణాంకం getenv_columns(శూన్య);
పూర్ణాంకం fd_columns(పూర్ణాంకం fd);
గుర్తులేని columns(శూన్య);
పూర్ణాంకం fd_పంక్తులు(పూర్ణాంకం fd);
గుర్తులేని పంక్తులు(శూన్య);

శూన్య columns_పంక్తులు_cache_reset(పూర్ణాంకం వాడని signum);
శూన్య reset_terminal_feature_caches(శూన్య);

bool on_tty(శూన్య);
bool పదం_env_valid(స్థిరరాశి అక్షరం *పదం);
bool getenv_terminal_is_dumb(శూన్య);
bool terminal_is_dumb(శూన్య);

bool dev_console_colors_enabled(శూన్య);

పూర్ణాంకం get_ctty_devnr(pid_t pid, dev_t *ret);
పూర్ణాంకం get_ctty(pid_t pid, dev_t *ret_devnr, అక్షరం **ret);

పూర్ణాంకం getttyname_malloc(పూర్ణాంకం fd, అక్షరం **ret);
పూర్ణాంకం getttyname_harder(పూర్ణాంకం fd, అక్షరం **ret);

పూర్ణాంకం ptsname_malloc(పూర్ణాంకం fd, అక్షరం **ret);

పూర్ణాంకం openpt_allocate(పూర్ణాంకం flags, అక్షరం **ret_peer_మార్గం);
పూర్ణాంకం openpt_allocate_లోపల_namespace(స్థిరరాశి PidRef *pidref, పూర్ణాంకం flags, అక్షరం **ret_peer_మార్గం);

పూర్ణాంకం vt_restore(పూర్ణాంకం fd);
పూర్ణాంకం vt_release(పూర్ణాంకం fd, bool restore);

శూన్య get_log_colors(పూర్ణాంకం priority, స్థిరరాశి అక్షరం **on, స్థిరరాశి అక్షరం **off, స్థిరరాశి అక్షరం **highlight);

/* Assume TTY_MODE is defined in config.h. Also, this assumes there is a 'tty' group. */
assert_cc((TTY_బహుళకం & ~0666) == 0);
assert_cc((TTY_బహుళకం & 0711) == 0600);

శూన్య termios_disable_echo(నిర్మాణం termios *termios);

/* A termios sentinel with all flag fields set to all-ones-bits. No real tcgetattr() result will ever
 * match this because no real terminal configuration uses all-ones in every flag field simultaneously. */
#నిర్వచించు TERMIOS_NULL (నిర్మాణం termios) {         \
        .c_iflag = UINT_MAX,                    \
        .c_oflag = UINT_MAX,                    \
        .c_cflag = UINT_MAX,                    \
        .c_lflag = UINT_MAX,                    \
}

రకనిర్వచనం నిర్మాణం TermiosResetContext {
        పూర్ణాంకం *fd;
        నిర్మాణం termios *termios;
} TermiosResetContext;

శూన్య termios_reset(స్థిరరాశి TermiosResetContext *c);

#నిర్వచించు CLEANUP_TERMIOS_RESET(_fd, _termios)                                   \
        _cleanup_(termios_reset) వాడని స్థిరరాశి TermiosResetContext            \
                CONCATENATE(_cleanup_termios_, UNIQ) = {                       \
                        .fd = &(_fd),                                          \
                        .termios = &(_termios),                                \
                }

/* The $TERM value we use for terminals other than the Linux console */
#నిర్వచించు FALLBACK_పదం "vt220"

పూర్ణాంకం get_default_background_color(ద్విగుణ *ret_red, ద్విగుణ *ret_green, ద్విగుణ *ret_blue);
పూర్ణాంకం terminal_get_కొలత(పూర్ణాంకం ఆదానం_fd, పూర్ణాంకం output_fd, గుర్తులేని *ret_అడ్డువరుసలు, గుర్తులేని *ret_columns, bool try_dsr, bool try_csi18);
పూర్ణాంకం terminal_fix_కొలత(పూర్ణాంకం ఆదానం_fd, పూర్ణాంకం output_fd);

పూర్ణాంకం terminal_get_terminfo_by_dcs(పూర్ణాంకం fd, అక్షరం **ret_name);
పూర్ణాంకం have_terminfo_file(స్థిరరాశి అక్షరం *name);
పూర్ణాంకం query_పదం_for_tty(స్థిరరాశి అక్షరం *tty, అక్షరం **ret_పదం);

పూర్ణాంకం terminal_is_pty_fd(పూర్ణాంకం fd);

పూర్ణాంకం pty_open_peer(పూర్ణాంకం fd, పూర్ణాంకం బహుళకం);

స్థిర అంతఃవరుస bool osc_char_is_valid(అక్షరం c) {
        /* Checks whether the specified character is safe to be included inside an ANSI OSC sequence, as per
         * ECMA-48 5th edition, section 8.3.89 */
        తిరిగి (గుర్తులేని అక్షరం) c >= 32 && (గుర్తులేని అక్షరం) c < 127;
}

#నిర్వచించు VTNR_MAX 63

స్థిర అంతఃవరుస bool vtnr_is_valid(గుర్తులేని n) {
        తిరిగి n >= 1 && n <= VTNR_MAX;
}
