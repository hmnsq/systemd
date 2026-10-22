/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <fcntl.h>
#ಅನ್ತರ್ಭಾವ <linux/kd.h>
#ಅನ್ತರ್ಭಾವ <linux/magic.h>
#ಅನ್ತರ್ಭಾವ <linux/tiocl.h>
#ಅನ್ತರ್ಭಾವ <linux/vt.h>
#ಅನ್ತರ್ಭಾವ <poll.h>
#ಅನ್ತರ್ಭಾವ <signal.h>
#ಅನ್ತರ್ಭಾವ <stdlib.h>
#ಅನ್ತರ್ಭಾವ <sys/inotify.h>
#ಅನ್ತರ್ಭಾವ <sys/ioctl.h>
#ಅನ್ತರ್ಭಾವ <sys/sysmacros.h>
#ಅನ್ತರ್ಭಾವ <termios.h>
#ಅನ್ತರ್ಭಾವ <time.h>
#ಅನ್ತರ್ಭಾವ <unistd.h>

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "ansi-color.h"
#ಅನ್ತರ್ಭಾವ "chase.h"
#ಅನ್ತರ್ಭಾವ "devnum-util.h"
#ಅನ್ತರ್ಭಾವ "errno-util.h"
#ಅನ್ತರ್ಭಾವ "extract-word.h"
#ಅನ್ತರ್ಭಾವ "fd-util.h"
#ಅನ್ತರ್ಭಾವ "fileio.h"
#ಅನ್ತರ್ಭಾವ "fs-util.h"
#ಅನ್ತರ್ಭಾವ "hexdecoct.h"
#ಅನ್ತರ್ಭಾವ "inotify-util.h"
#ಅನ್ತರ್ಭಾವ "io-util.h"
#ಅನ್ತರ್ಭಾವ "log.h"
#ಅನ್ತರ್ಭಾವ "namespace-util.h"
#ಅನ್ತರ್ಭಾವ "parse-util.h"
#ಅನ್ತರ್ಭಾವ "path-util.h"
#ಅನ್ತರ್ಭಾವ "pidref.h"
#ಅನ್ತರ್ಭಾವ "proc-cmdline.h"
#ಅನ್ತರ್ಭಾವ "process-util.h"
#ಅನ್ತರ್ಭಾವ "signal-util.h"
#ಅನ್ತರ್ಭಾವ "socket-util.h"
#ಅನ್ತರ್ಭಾವ "stat-util.h"
#ಅನ್ತರ್ಭಾವ "stdio-util.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"
#ಅನ್ತರ್ಭಾವ "strv.h"
#ಅನ್ತರ್ಭಾವ "terminal-util.h"
#ಅನ್ತರ್ಭಾವ "time-util.h"
#ಅನ್ತರ್ಭಾವ "utf8.h"

/* How much to wait when reading/writing ANSI sequences from/to the console */
#ವ್ಯಾಖ್ಯಾನಿಸು CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC (333 * USEC_ಪ್ರತಿ_MSEC)

ಸ್ಥಿರ ಅಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ cached_columns = 0;
ಸ್ಥಿರ ಅಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ cached_ಸಾಲುಗಳು = 0;

ಸ್ಥಿರ ಅಸ್ಥಿರ ಪೂರ್ಣಾಂಕ cached_on_tty = -1;
ಸ್ಥಿರ ಅಸ್ಥಿರ ಪೂರ್ಣಾಂಕ cached_on_dev_null = -1;

bool isatty_ಸುರಕ್ಷಿತ(ಪೂರ್ಣಾಂಕ fd) {
        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        ಆದರೆ (isatty(fd))
                ಹಿಂತಿರುಗಿ true;

        /* Linux/glibc returns EIO for hung up TTY on isatty(). Which is wrong, the thing doesn't stop being
         * a TTY after all, just because it is temporarily hung up. Let's work around this here, until this
         * is fixed in glibc. See: https://sourceware.org/bugzilla/show_bug.cgi?id=32103 */
        ಆದರೆ (errno == EIO)
                ಹಿಂತಿರುಗಿ true;

        /* Be resilient if we're working on stdio, since they're set up by parent process. */
        ಪ್ರತಿಜ್ಞಾ(errno != EBADF || ಒಳ_ಗಣ(fd, STDIN_FILENO, STDOUT_FILENO, STDERR_FILENO));

        ಹಿಂತಿರುಗಿ false;
}

ಪೂರ್ಣಾಂಕ chvt(ಪೂರ್ಣಾಂಕ vt) {
        _cleanup_close_ ಪೂರ್ಣಾಂಕ fd = -EBADF;

        /* Switch to the specified vt number. If the VT is specified <= 0 switch to the VT the kernel log messages go,
         * if that's configured. */

        fd = open_terminal("/dev/tty0", O_RDWR|O_NOCTTY|O_CLOEXEC|O_NONBLOCK);
        ಆದರೆ (fd < 0)
                ಹಿಂತಿರುಗಿ fd;

        ಆದರೆ (vt <= 0) {
                ಪೂರ್ಣಾಂಕ tiocl[2] = {
                        TIOCL_GETKMSGREDIRECT,
                        0
                };

                ಆದರೆ (ioctl(fd, TIOCLINUX, tiocl) < 0)
                        ಹಿಂತಿರುಗಿ -errno;

                vt = tiocl[0] <= 0 ? 1 : tiocl[0];
        }

        ಹಿಂತಿರುಗಿ RET_NERRNO(ioctl(fd, VT_ACTIVATE, vt));
}

ಪೂರ್ಣಾಂಕ read_one_char(FILE *f, ಅಕ್ಷರ *ret, usec_t t, bool echo, bool *need_nl) {
        _cleanup_free_ ಅಕ್ಷರ *ಸಾಲು = ಖಾಲಿ;
        ರಚನೆ termios old_termios;
        ಪೂರ್ಣಾಂಕ r, fd;

        ಪ್ರತಿಜ್ಞಾ(ret);

        ಆದರೆ (!f)
                f = stdin;

        /* If this is a terminal, then switch canonical mode off, so that we can read a single
         * character. (Note that fmemopen() streams do not have an fd associated with them, let's handle that
         * nicely.) If 'echo' is false we'll also disable ECHO mode so that the pressed key is not made
         * visible to the user. */
        fd = fileno(f);
        ಆದರೆ (fd >= 0 && tcgetattr(fd, &old_termios) >= 0) {
                ರಚನೆ termios new_termios = old_termios;

                new_termios.c_lflag &= ~(ICANON|(echo ? 0 : ECHO));
                new_termios.c_cc[VMIN] = 1;
                new_termios.c_cc[VTIME] = 0;

                ಆದರೆ (tcsetattr(fd, TCSANOW, &new_termios) >= 0) {
                        ಅಕ್ಷರ c;

                        ಆದರೆ (t != USEC_INFINITY) {
                                ಆದರೆ (fd_wait_for_event(fd, POLLIN, t) <= 0) {
                                        (ಶೂನ್ಯ) tcsetattr(fd, TCSANOW, &old_termios);
                                        ಹಿಂತಿರುಗಿ -ETIMEDOUT;
                                }
                        }

                        r = ಸುರಕ್ಷಿತ_fgetc(f, &c);
                        (ಶೂನ್ಯ) tcsetattr(fd, TCSANOW, &old_termios);
                        ಆದರೆ (r < 0)
                                ಹಿಂತಿರುಗಿ r;
                        ಆದರೆ (r == 0)
                                ಹಿಂತಿರುಗಿ -EIO;

                        ಆದರೆ (need_nl)
                                *need_nl = c != '\n';

                        *ret = c;
                        ಹಿಂತಿರುಗಿ 0;
                }
        }

        ಆದರೆ (t != USEC_INFINITY && fd >= 0) {
                /* Let's wait the specified amount of time for input. When we have no fd we skip this, under
                 * the assumption that this is an fmemopen() stream or so where waiting doesn't make sense
                 * anyway, as the data is either already in the stream or cannot possible be placed there
                 * while we access the stream */

                ಆದರೆ (fd_wait_for_event(fd, POLLIN, t) <= 0)
                        ಹಿಂತಿರುಗಿ -ETIMEDOUT;
        }

        /* If this is not a terminal, then read a full line instead */

        r = read_ಸಾಲು(f, 16, &ಸಾಲು); /* longer than necessary, to eat up UTF-8 chars/vt100 key sequences */
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;
        ಆದರೆ (r == 0)
                ಹಿಂತಿರುಗಿ -EIO;

        ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ಸಾಲು) != 1)
                ಹಿಂತಿರುಗಿ -EBADMSG;

        ಆದರೆ (need_nl)
                *need_nl = false;

        *ret = ಸಾಲು[0];
        ಹಿಂತಿರುಗಿ 0;
}

#ವ್ಯಾಖ್ಯಾನಿಸು DEFAULT_ASK_REFRESH_USEC (2*USEC_ಪ್ರತಿ_SEC)

ಪೂರ್ಣಾಂಕ ask_char(ಅಕ್ಷರ *ret, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *replies, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, ...) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);
        ಪ್ರತಿಜ್ಞಾ(replies);
        ಪ್ರತಿಜ್ಞಾ(fmt);

        ಗಾಗಿ (;;) {
                va_list ap;
                ಅಕ್ಷರ c;
                bool need_nl = true;

                fputs(ansi_highlight(), stdout);

                ಅಕ್ಷರ_ಬರೆ('\r');

                va_start(ap, fmt);
                vprintf(fmt, ap);
                va_end(ap);

                fputs(ansi_ಸಾಮಾನ್ಯ(), stdout);

                fflush(stdout);

                r = read_one_char(stdin, &c, DEFAULT_ASK_REFRESH_USEC, /* echo= */ true, &need_nl);
                ಆದರೆ (r < 0) {

                        ಆದರೆ (r == -ETIMEDOUT)
                                ಮುಂದುವರಿ;

                        ಆದರೆ (r == -EBADMSG) {
                                puts("Bad input, please try again.");
                                ಮುಂದುವರಿ;
                        }

                        ಅಕ್ಷರ_ಬರೆ('\n');
                        ಹಿಂತಿರುಗಿ r;
                }

                ಆದರೆ (need_nl)
                        ಅಕ್ಷರ_ಬರೆ('\n');

                ಆದರೆ (strchr(replies, c)) {
                        *ret = c;
                        ಹಿಂತಿರುಗಿ 0;
                }

                puts("Read unexpected character, please try again.");
        }
}

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ Completion_ಫಲಿತಾಂಶ{
        COMPLETION_ALREADY,       /* the input string is already complete */
        COMPLETION_ತುಂಬಿದ,          /* completed the input string to be complete now */
        COMPLETION_PARTIAL,       /* completed the input string so that is still incomplete */
        COMPLETION_NONE,          /* found no matching completion */
        COMPLETION_ಫಲಿತಾಂಶ_MAX,
        COMPLETION_ಫಲಿತಾಂಶ_INVALID = -EINVAL,
        COMPLETION_ಫಲಿತಾಂಶ_ERRNO_MAX = -ERRNO_MAX,
} Completion_ಫಲಿತಾಂಶ;

ಸ್ಥಿರ Completion_ಫಲಿತಾಂಶ pick_completion(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *string, ಅಕ್ಷರ *ಸ್ಥಿರಾಂಕ*completions, ಅಕ್ಷರ **ret) {
        _cleanup_free_ ಅಕ್ಷರ *ಸಿಕ್ಕಿತು = ಖಾಲಿ;
        bool partial = false;

        ಪ್ರತಿಜ್ಞಾ(ret);

        string = strempty(string);

        STRV_FOREACH(c, completions) {

                /* Ignore entries that are not actually completions */
                ಆದರೆ (!startswith(*c, string))
                        ಮುಂದುವರಿ;

                /* Store first completion that matches */
                ಆದರೆ (!ಸಿಕ್ಕಿತು) {
                        ಸಿಕ್ಕಿತು = strdup(*c);
                        ಆದರೆ (!ಸಿಕ್ಕಿತು)
                                ಹಿಂತಿರುಗಿ -ENOMEM;

                        ಮುಂದುವರಿ;
                }

                /* If there's another completion that works truncate the one we already found by common
                 * prefix */
                size_t n = str_common_ಪೂರ್ವಪ್ರತ್ಯಯ(ಸಿಕ್ಕಿತು, *c);
                ಆದರೆ (n == SIZE_MAX)
                        ಮುಂದುವರಿ;

                ಸಿಕ್ಕಿತು[n] = 0;
                partial = true;
        }

        *ret = TAKE_PTR(ಸಿಕ್ಕಿತು);

        ಆದರೆ (!*ret)
                ಹಿಂತಿರುಗಿ COMPLETION_NONE;
        ಆದರೆ (partial)
                ಹಿಂತಿರುಗಿ COMPLETION_PARTIAL;

        ಹಿಂತಿರುಗಿ streq(string, *ret) ? COMPLETION_ALREADY : COMPLETION_ತುಂಬಿದ;
}

ಸ್ಥಿರ ಶೂನ್ಯ clear_by_backspace(size_t n) {
        /* Erase the specified number of character cells backwards on the terminal */
        ಗಾಗಿ (size_t i = 0; i < n; i++)
                fputs("\b \b", stdout);
}

ಪೂರ್ಣಾಂಕ ask_string_ತುಂಬಿದ(
                ಅಕ್ಷರ **ret,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *prefill,
                GetCompletionsCallback get_completions,
                ಶೂನ್ಯ *userdata,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಪಠ್ಯ, ...) {

        va_list ap;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);
        ಪ್ರತಿಜ್ಞಾ(ಪಠ್ಯ);

        _cleanup_free_ ಅಕ್ಷರ *string = ಖಾಲಿ;
        size_t n = 0;

        ಆದರೆ (prefill) {
                /* Prefill query with explicit data if specified */

                string = strdup(prefill);
                ಆದರೆ (!string)
                        ಹಿಂತಿರುಗಿ -ENOMEM;

                n = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(string);

        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (get_completions) {
                /* Otherwise, figure out what string to preselect the query with */
                _cleanup_strv_free_ ಅಕ್ಷರ **completions = ಖಾಲಿ;
                r = get_completions("", GET_COMPLETIONS_PRESELECT, &completions, userdata);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;

                Completion_ಫಲಿತಾಂಶ cr = pick_completion(string, completions, &string);
                ಆದರೆ (cr < 0)
                        ಹಿಂತಿರುಗಿ cr;

                n = strlen_ptr(string);
        }

        /* Output the prompt */
        fputs(ansi_highlight(), stdout);
        va_start(ap, ಪಠ್ಯ);
        vprintf(ಪಠ್ಯ, ap);
        va_end(ap);
        fputs(ansi_ಸಾಮಾನ್ಯ(), stdout);
        ಆದರೆ (string)
                fputs(string, stdout);
        fflush(stdout);

        /* Do interactive logic only if stdin + stdout are connected to the same place. And yes, we could use
         * STDIN_FILENO and STDOUT_FILENO here, but let's be overly correct for once, after all libc allows
         * swapping out stdin/stdout. */
        ಪೂರ್ಣಾಂಕ fd_ಒಳಹರಿವು = fileno(stdin);
        ಪೂರ್ಣಾಂಕ fd_output = fileno(stdout);
        ರಚನೆ termios old_termios = TERMIOS_NULL;
        CLEANUP_TERMIOS_RESET(fd_ಒಳಹರಿವು, old_termios);

        ಆದರೆ (fd_ಒಳಹರಿವು < 0 || fd_output < 0 || same_fd(fd_ಒಳಹರಿವು, fd_output) <= 0)
                ಹೋಗು fallback;

        /* Try to disable echo, which also tells us if this even is a terminal */
        ಆದರೆ (tcgetattr(fd_ಒಳಹರಿವು, &old_termios) < 0) {
                old_termios = TERMIOS_NULL;
                ಹೋಗು fallback;
        }

        ರಚನೆ termios new_termios = old_termios;
        termios_disable_echo(&new_termios);
        ಆದರೆ (tcsetattr(fd_ಒಳಹರಿವು, TCSANOW, &new_termios) < 0)
                ಹಿಂತಿರುಗಿ -errno;

        ಗಾಗಿ (;;) {
                ಪೂರ್ಣಾಂಕ c = fgetc(stdin);

                /* On EOF or NUL, end the request, don't output anything anymore */
                ಆದರೆ (ಒಳ_ಗಣ(c, EOF, 0))
                        ನಿಲ್ಲಿಸು;

                /* On Return also end the request, but make this visible */
                ಆದರೆ (ಒಳ_ಗಣ(c, '\n', '\r')) {
                        fputc('\n', stdout);
                        ನಿಲ್ಲಿಸು;
                }

                ಆದರೆ (c == '\t') {
                        /* Tab */

                        _cleanup_strv_free_ ಅಕ್ಷರ **completions = ಖಾಲಿ;
                        ಆದರೆ (get_completions) {
                                r = get_completions(string, /* flags= */ 0, &completions, userdata);
                                ಆದರೆ (r < 0)
                                        ಹಿಂತಿರುಗಿ r;
                        }

                        _cleanup_free_ ಅಕ್ಷರ *new_string = ಖಾಲಿ;
                        Completion_ಫಲಿತಾಂಶ cr = pick_completion(string, completions, &new_string);
                        ಆದರೆ (cr < 0)
                                ಹಿಂತಿರುಗಿ cr;
                        ಆದರೆ (ಒಳ_ಗಣ(cr, COMPLETION_PARTIAL, COMPLETION_ತುಂಬಿದ)) {
                                /* Output the new suffix we learned */
                                fputs(ASSERT_PTR(startswith(new_string, strempty(string))), stdout);

                                /* And update the whole string */
                                free_and_replace(string, new_string);
                                n = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(string);
                        }
                        ಆದರೆ (cr == COMPLETION_NONE)
                                fputc('\a', stdout); /* BEL */

                        ಆದರೆ (ಒಳ_ಗಣ(cr, COMPLETION_PARTIAL, COMPLETION_ALREADY)) {
                                /* If this worked only partially, or if the user hit TAB even though we were
                                 * complete already, then show the remaining options (in the latter case just
                                 * the one). */
                                fputc('\n', stdout);

                                _cleanup_strv_free_ ಅಕ್ಷರ **filtered = strv_filter_ಪೂರ್ವಪ್ರತ್ಯಯ(completions, string);
                                ಆದರೆ (!filtered)
                                        ಹಿಂತಿರುಗಿ -ENOMEM;

                                r = show_menu(filtered,
                                              /* n_columns= */ SIZE_MAX,
                                              /* column_width= */ SIZE_MAX,
                                              /* ellipsize_percentage= */ 0,
                                              /* grey_prefix= */ string,
                                              /* with_numbers= */ false);
                                ಆದರೆ (r < 0)
                                        ಹಿಂತಿರುಗಿ r;

                                /* Show the prompt again */
                                fputs(ansi_highlight(), stdout);
                                va_start(ap, ಪಠ್ಯ);
                                vprintf(ಪಠ್ಯ, ap);
                                va_end(ap);
                                fputs(ansi_ಸಾಮಾನ್ಯ(), stdout);
                                fputs(string, stdout);
                        }

                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಒಳ_ಗಣ(c, '\b', 127)) {
                        /* Backspace */

                        ಆದರೆ (n == 0)
                                fputc('\a', stdout); /* BEL */
                        ಇಲ್ಲದಿದ್ದರೆ {
                                size_t m = utf8_ಕೊನೆಯ_ಉದ್ದಳತೆ(string, n);

                                ಅಕ್ಷರ *e = string + n - m;
                                clear_by_backspace(utf8_console_ಅಗಲ(e));

                                *e = 0;
                                n -= m;
                        }

                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == 21) {
                        /* Ctrl-u → erase all input */

                        clear_by_backspace(utf8_console_ಅಗಲ(string));
                        ಆದರೆ (string)
                                string[n = 0] = 0;
                        ಇಲ್ಲದಿದ್ದರೆ
                                ಪ್ರತಿಜ್ಞಾ(n == 0);

                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == 4) {
                        /* Ctrl-d → cancel this field input */

                        ಹಿಂತಿರುಗಿ -ECANCELED;

                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (char_is_cc(c) || n >= LINE_MAX)
                        /* refuse control characters and too long strings */
                        fputc('\a', stdout); /* BEL */
                ಇಲ್ಲದಿದ್ದರೆ {
                        /* Regular char */

                        ಆದರೆ (!GREEDY_REALLOC(string, n+2))
                                ಹಿಂತಿರುಗಿ -ENOMEM;

                        string[n++] = (ಅಕ್ಷರ) c;
                        string[n] = 0;

                        fputc(c, stdout);
                }

                fflush(stdout);
        }

        ಆದರೆ (!string) {
                string = strdup("");
                ಆದರೆ (!string)
                        ಹಿಂತಿರುಗಿ -ENOMEM;
        }

        *ret = TAKE_PTR(string);
        ಹಿಂತಿರುಗಿ 0;

fallback:
        /* A simple fallback without TTY magic */
        string = mfree(string);
        r = read_ಸಾಲು(stdin, LONG_ಸಾಲು_MAX, &string);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;
        ಆದರೆ (r == 0)
                ಹಿಂತಿರುಗಿ -EIO;

        *ret = TAKE_PTR(string);
        ಹಿಂತಿರುಗಿ 0;
}

bool any_ಕೀಲಿ_ಗಮ್ಯ_proceed(ಶೂನ್ಯ) {

        /* Insert a new line here as well as to when the user inputs, as this is also used during the boot up
         * sequence when status messages may be interleaved with the current program output. This ensures
         * that the status messages aren't appended on the same line as this message. */

        fputc('\n', stdout);
        fputs(ansi_highlight_magenta(), stdout);
        fputs("-- Press any key to proceed --", stdout);
        fputs(ansi_ಸಾಮಾನ್ಯ(), stdout);
        fputc('\n', stdout);
        fflush(stdout);

        ಅಕ್ಷರ ಕೀಲಿ = 0;
        (ಶೂನ್ಯ) read_one_char(stdin, &ಕೀಲಿ, USEC_INFINITY, /* echo= */ false, /* need_nl= */ ಖಾಲಿ);

        fputc('\n', stdout);
        fflush(stdout);

        ಹಿಂತಿರುಗಿ ಕೀಲಿ != 'q';
}

ಸ್ಥಿರ size_t widest_list_element(ಅಕ್ಷರ *ಸ್ಥಿರಾಂಕ*l) {
        size_t w = 0;

        /* Returns the largest console width of all elements in 'l' */

        STRV_FOREACH(i, l)
                w = MAX(w, utf8_console_ಅಗಲ(*i));

        ಹಿಂತಿರುಗಿ w;
}

ಪೂರ್ಣಾಂಕ show_menu(ಅಕ್ಷರ **x,
              size_t n_columns,
              size_t ಸ್ತಂಭ_ಅಗಲ,
              ಚಿಹ್ನೆರಹಿತ ellipsize_percentage,
              ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *grey_ಪೂರ್ವಪ್ರತ್ಯಯ,
              bool with_numbers) {

        ಪ್ರತಿಜ್ಞಾ(n_columns > 0);
        ಪ್ರತಿಜ್ಞಾ(ಸ್ತಂಭ_ಅಗಲ > 0);

        ಆದರೆ (n_columns == SIZE_MAX)
                n_columns = 3;

        ಆದರೆ (ಸ್ತಂಭ_ಅಗಲ == SIZE_MAX) {
                size_t widest = widest_list_element(x);

                /* If not specified, derive column width from screen width */
                size_t ಸ್ತಂಭ_max = (columns()-1) / n_columns;

                /* Subtract room for numbers */
                ಆದರೆ (with_numbers)
                        ಸ್ತಂಭ_max = LESS_BY(ಸ್ತಂಭ_max, 6U);

                /* If columns would get too tight let's make this a linear list instead. */
                ಆದರೆ (ಸ್ತಂಭ_max < 10 && widest > 10) {
                        n_columns = 1;
                        ಸ್ತಂಭ_max = columns()-1;

                        ಆದರೆ (with_numbers)
                                ಸ್ತಂಭ_max = LESS_BY(ಸ್ತಂಭ_max, 6U);
                }

                /* Never make this narrower than 10 characters */
                ಸ್ತಂಭ_max = MAX(ಸ್ತಂಭ_max, 10U);
                ಸ್ತಂಭ_ಅಗಲ = CLAMP(widest+1, 10U, ಸ್ತಂಭ_max);
        }

        size_t n = strv_ಉದ್ದಳತೆ(x);
        size_t ಪ್ರತಿ_ಸ್ತಂಭ = DIV_ಸುತ್ತು_ಮೇಲೆ(n, n_columns);

        size_t break_ಸಾಲುಗಳು = ಸಾಲುಗಳು();
        ಆದರೆ (break_ಸಾಲುಗಳು > 2)
                break_ಸಾಲುಗಳು--;

        /* The first page gets two extra lines, since we want to show a title */
        size_t break_modulo = break_ಸಾಲುಗಳು;
        ಆದರೆ (break_modulo > 3)
                break_modulo -= 3;

        ಗಾಗಿ (size_t i = 0; i < ಪ್ರತಿ_ಸ್ತಂಭ; i++) {

                ಗಾಗಿ (size_t j = 0; j < n_columns; j++) {
                        _cleanup_free_ ಅಕ್ಷರ *e = ಖಾಲಿ;

                        size_t p = j * ಪ್ರತಿ_ಸ್ತಂಭ + i;
                        ಆದರೆ (p >= n)
                                ನಿಲ್ಲಿಸು;

                        e = ellipsize(x[p], ಸ್ತಂಭ_ಅಗಲ, ellipsize_percentage);
                        ಆದರೆ (!e)
                                ಹಿಂತಿರುಗಿ -ENOMEM;

                        ಆದರೆ (with_numbers)
                                ಬರೆ("%s%4zu)%s ",
                                       ansi_grey(),
                                       p + 1,
                                       ansi_ಸಾಮಾನ್ಯ());

                        ಆದರೆ (grey_ಪೂರ್ವಪ್ರತ್ಯಯ && startswith(e, grey_ಪೂರ್ವಪ್ರತ್ಯಯ)) {
                                size_t k = MIN(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(grey_ಪೂರ್ವಪ್ರತ್ಯಯ), ಸ್ತಂಭ_ಅಗಲ);
                                ಬರೆ("%s%.*s%s",
                                       ansi_grey(),
                                       (ಪೂರ್ಣಾಂಕ) k, e,
                                       ansi_ಸಾಮಾನ್ಯ());
                                ಬರೆ("%-*s",
                                       (ಪೂರ್ಣಾಂಕ) (ಸ್ತಂಭ_ಅಗಲ - k), e+k);
                        } ಇಲ್ಲದಿದ್ದರೆ
                                ಬರೆ("%-*s", (ಪೂರ್ಣಾಂಕ) ಸ್ತಂಭ_ಅಗಲ, e);
                }

                ಅಕ್ಷರ_ಬರೆ('\n');

                /* on the first screen we reserve 2 extra lines for the title */
                ಆದರೆ (i % break_ಸಾಲುಗಳು == break_modulo)
                        ಆದರೆ (!any_ಕೀಲಿ_ಗಮ್ಯ_proceed())
                                ಹಿಂತಿರುಗಿ 0;
        }

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ open_terminal(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ಪೂರ್ಣಾಂಕ ಬಹುಲಕ) {
        _cleanup_close_ ಪೂರ್ಣಾಂಕ fd = -EBADF;

        /*
         * If a TTY is in the process of being closed opening it might cause EIO. This is horribly awful, but
         * unlikely to be changed in the kernel. Hence we work around this problem by retrying a couple of
         * times.
         *
         * https://bugs.launchpad.net/ubuntu/+source/linux/+bug/554172/comments/245
         */

        ಪ್ರತಿಜ್ಞಾ((ಬಹುಲಕ & (O_CREAT|O_ಮಾರ್ಗ|O_DIRECTORY|O_TMPFILE)) == 0);

        ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ c = 0;; c++) {
                fd = open(name, ಬಹುಲಕ, 0);
                ಆದರೆ (fd >= 0)
                        ನಿಲ್ಲಿಸು;

                ಆದರೆ (errno != EIO)
                        ಹಿಂತಿರುಗಿ -errno;

                /* Max 1s in total */
                ಆದರೆ (c >= 20)
                        ಹಿಂತಿರುಗಿ -EIO;

                (ಶೂನ್ಯ) usleep_ಸುರಕ್ಷಿತ(50 * USEC_ಪ್ರತಿ_MSEC);
        }

        ಆದರೆ (!isatty_ಸುರಕ್ಷಿತ(fd))
                ಹಿಂತಿರುಗಿ -ENOTTY;

        ಹಿಂತಿರುಗಿ TAKE_FD(fd);
}

ಪೂರ್ಣಾಂಕ acquire_terminal(
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name,
                AcquireTerminalFlags flags,
                usec_t timeout) {

        _cleanup_close_ ಪೂರ್ಣಾಂಕ notify = -EBADF, fd = -EBADF;
        usec_t ts = USEC_INFINITY;
        ಪೂರ್ಣಾಂಕ r, wd = -1;

        ಪ್ರತಿಜ್ಞಾ(name);

        AcquireTerminalFlags ಬಹುಲಕ = flags & ACQUIRE_TERMINAL_ಬಹುಲಕ_ಮುಖವಾಡ;
        ಪ್ರತಿಜ್ಞಾ(ಒಳ_ಗಣ(ಬಹುಲಕ, ACQUIRE_TERMINAL_TRY, ACQUIRE_TERMINAL_FORCE, ACQUIRE_TERMINAL_WAIT));
        ಪ್ರತಿಜ್ಞಾ(ಬಹುಲಕ == ACQUIRE_TERMINAL_WAIT || !FLAGS_ಗಣ(flags, ACQUIRE_TERMINAL_WATCH_SIGTERM));

        /* We use inotify to be notified when the tty is closed. We create the watch before checking if we can actually
         * acquire it, so that we don't lose any event.
         *
         * Note: strictly speaking this actually watches for the device being closed, it does *not* really watch
         * whether a tty loses its controlling process. However, unless some rogue process uses TIOCNOTTY on /dev/tty
         * *after* closing its tty otherwise this will not become a problem. As long as the administrator makes sure to
         * not configure any service on the same tty as an untrusted user this should not be a problem. (Which they
         * probably should not do anyway.) */

        ಆದರೆ (ಬಹುಲಕ == ACQUIRE_TERMINAL_WAIT) {
                notify = inotify_init1(IN_CLOEXEC | IN_NONBLOCK);
                ಆದರೆ (notify < 0)
                        ಹಿಂತಿರುಗಿ -errno;

                wd = inotify_add_watch(notify, name, IN_CLOSE);
                ಆದರೆ (wd < 0)
                        ಹಿಂತಿರುಗಿ -errno;

                ಆದರೆ (timeout != USEC_INFINITY)
                        ts = now(CLOCK_MONOTONIC);
        }

        /* If we are called with ACQUIRE_TERMINAL_WATCH_SIGTERM we'll unblock SIGTERM during ppoll() temporarily */
        sigset_t poll_ss;
        assert_se(sigprocmask(SIG_SETMASK, /* newset= */ ಖಾಲಿ, &poll_ss) >= 0);
        ಆದರೆ (flags & ACQUIRE_TERMINAL_WATCH_SIGTERM) {
                assert_se(sigismember(&poll_ss, SIGTERM) > 0);
                assert_se(sigdelset(&poll_ss, SIGTERM) >= 0);
        }

        ಗಾಗಿ (;;) {
                ಆದರೆ (notify >= 0) {
                        r = flush_fd(notify);
                        ಆದರೆ (r < 0)
                                ಹಿಂತಿರುಗಿ r;
                }

                /* We pass here O_NOCTTY only so that we can check the return value TIOCSCTTY and have a reliable way
                 * to figure out if we successfully became the controlling process of the tty */
                fd = open_terminal(name, O_RDWR|O_NOCTTY|O_CLOEXEC);
                ಆದರೆ (fd < 0)
                        ಹಿಂತಿರುಗಿ fd;

                /* Temporarily ignore SIGHUP, so that we don't get SIGHUP'ed if we already own the tty. */
                ರಚನೆ sigaction sa_old;
                assert_se(sigaction(SIGHUP, &sigaction_ignore, &sa_old) >= 0);

                /* First, try to get the tty */
                r = RET_NERRNO(ioctl(fd, TIOCSCTTY, ಬಹುಲಕ == ACQUIRE_TERMINAL_FORCE));

                /* Reset signal handler to old value */
                assert_se(sigaction(SIGHUP, &sa_old, ಖಾಲಿ) >= 0);

                /* Success? Exit the loop now! */
                ಆದರೆ (r >= 0)
                        ನಿಲ್ಲಿಸು;

                /* Any failure besides -EPERM? Fail, regardless of the mode. */
                ಆದರೆ (r != -EPERM)
                        ಹಿಂತಿರುಗಿ r;

                ಆದರೆ (flags & ACQUIRE_TERMINAL_PERMISSIVE) /* If we are in permissive mode, then EPERM is fine, turn this
                                                          * into a success. Note that EPERM is also returned if we
                                                          * already are the owner of the TTY. */
                        ನಿಲ್ಲಿಸು;

                ಆದರೆ (ಬಹುಲಕ != ACQUIRE_TERMINAL_WAIT) /* If we are in TRY or FORCE mode, then propagate EPERM as EPERM */
                        ಹಿಂತಿರುಗಿ r;

                ಪ್ರತಿಜ್ಞಾ(notify >= 0);
                ಪ್ರತಿಜ್ಞಾ(wd >= 0);

                ಗಾಗಿ (;;) {
                        usec_t ಎಡ;
                        ಆದರೆ (timeout == USEC_INFINITY)
                                ಎಡ = USEC_INFINITY;
                        ಇಲ್ಲದಿದ್ದರೆ {
                                ಪ್ರತಿಜ್ಞಾ(ts != USEC_INFINITY);

                                usec_t n = usec_sub_unsigned(now(CLOCK_MONOTONIC), ts);
                                ಆದರೆ (n >= timeout)
                                        ಹಿಂತಿರುಗಿ -ETIMEDOUT;

                                ಎಡ = timeout - n;
                        }

                        r = ppoll_usec_ತುಂಬಿದ(
                                        &(ರಚನೆ pollfd) {
                                                .fd = notify,
                                                .events = POLLIN,
                                        },
                                        /* n_fds= */ 1,
                                        ಎಡ,
                                        &poll_ss);
                        ಆದರೆ (r < 0)
                                ಹಿಂತಿರುಗಿ r;
                        ಆದರೆ (r == 0)
                                ಹಿಂತಿರುಗಿ -ETIMEDOUT;

                        ಒಕ್ಕೂಟ inotify_event_ಬಫರ್ ಬಫರ್;
                        ssize_t l;
                        l = ಪಠ(notify, &ಬಫರ್, ಗಾತ್ರ(ಬಫರ್));
                        ಆದರೆ (l < 0) {
                                ಆದರೆ (ERRNO_IS_TRANSIENT(errno))
                                        ಮುಂದುವರಿ;

                                ಹಿಂತಿರುಗಿ -errno;
                        }

                        FOREACH_INOTIFY_EVENT(e, ಬಫರ್, l) {
                                ಆದರೆ (e->ಮುಖವಾಡ_3 & IN_Q_OVERFLOW) /* If we hit an inotify queue overflow, simply check if the terminal is up for grabs now. */
                                        ನಿಲ್ಲಿಸು;

                                ಆದರೆ (e->wd != wd || !(e->ಮುಖವಾಡ_3 & IN_CLOSE)) /* Safety checks */
                                        ಹಿಂತಿರುಗಿ -EIO;
                        }

                        ನಿಲ್ಲಿಸು;
                }

                /* We close the tty fd here since if the old session ended our handle will be dead. It's important that
                 * we do this after sleeping, so that we don't enter an endless loop. */
                fd = ಸುರಕ್ಷಿತ_close(fd);
        }

        ಹಿಂತಿರುಗಿ TAKE_FD(fd);
}

ಪೂರ್ಣಾಂಕ release_terminal(ಶೂನ್ಯ) {
        _cleanup_close_ ಪೂರ್ಣಾಂಕ fd = -EBADF;
        ಪೂರ್ಣಾಂಕ r;

        fd = open("/dev/tty", O_RDWR|O_NOCTTY|O_CLOEXEC|O_NONBLOCK);
        ಆದರೆ (fd < 0)
                ಹಿಂತಿರುಗಿ -errno;

        /* Temporarily ignore SIGHUP, so that we don't get SIGHUP'ed
         * by our own TIOCNOTTY */
        ರಚನೆ sigaction sa_old;
        assert_se(sigaction(SIGHUP, &sigaction_ignore, &sa_old) >= 0);

        r = RET_NERRNO(ioctl(fd, TIOCNOTTY));

        assert_se(sigaction(SIGHUP, &sa_old, ಖಾಲಿ) >= 0);

        ಹಿಂತಿರುಗಿ r;
}

ಪೂರ್ಣಾಂಕ terminal_new_session(ಶೂನ್ಯ) {

        /* Make us the new session leader, and set stdin tty to be our controlling terminal.
         *
         * Why stdin? Well, the ctty logic is relevant for signal delivery mostly, i.e. if people hit C-c
         * or the line is hung up. Such events are basically just a form of input, via a side channel
         * (that side channel being signal delivery, i.e. SIGINT, SIGHUP et al). Hence we focus on input,
         * not output here. */

        ಆದರೆ (!isatty_ಸುರಕ್ಷಿತ(STDIN_FILENO))
                ಹಿಂತಿರುಗಿ -ENXIO;

        (ಶೂನ್ಯ) setsid();
        ಹಿಂತಿರುಗಿ RET_NERRNO(ioctl(STDIN_FILENO, TIOCSCTTY, 0));
}

ಶೂನ್ಯ terminal_detach_session(ಶೂನ್ಯ) {
        (ಶೂನ್ಯ) setsid();
        (ಶೂನ್ಯ) release_terminal();
}

ಪೂರ್ಣಾಂಕ terminal_vhangup_fd(ಪೂರ್ಣಾಂಕ fd) {
        ಪ್ರತಿಜ್ಞಾ(fd >= 0);
        ಹಿಂತಿರುಗಿ RET_NERRNO(ioctl(fd, TIOCVHANGUP));
}

ಪೂರ್ಣಾಂಕ terminal_vhangup(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *tty) {
        _cleanup_close_ ಪೂರ್ಣಾಂಕ fd = -EBADF;

        ಪ್ರತಿಜ್ಞಾ(tty);

        fd = open_terminal(tty, O_RDWR|O_NOCTTY|O_CLOEXEC);
        ಆದರೆ (fd < 0)
                ಹಿಂತಿರುಗಿ fd;

        ಹಿಂತಿರುಗಿ terminal_vhangup_fd(fd);
}

ಪೂರ್ಣಾಂಕ vt_disallocate(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *tty_ಮಾರ್ಗ) {
        ಪ್ರತಿಜ್ಞಾ(tty_ಮಾರ್ಗ);

        /* Deallocate the VT if possible. If not possible (i.e. because it is the active one), at least clear
         * it entirely (including the scrollback buffer). */

        ಪೂರ್ಣಾಂಕ ttynr = vtnr_from_tty(tty_ಮಾರ್ಗ);
        ಆದರೆ (ttynr > 0) {
                _cleanup_close_ ಪೂರ್ಣಾಂಕ fd = open_terminal("/dev/tty0", O_RDWR|O_NOCTTY|O_CLOEXEC|O_NONBLOCK);
                ಆದರೆ (fd < 0)
                        ಹಿಂತಿರುಗಿ fd;

                /* Try to deallocate */
                ಆದರೆ (ioctl(fd, VT_DISALLOCATE, ttynr) >= 0)
                        ಹಿಂತಿರುಗಿ 0;
                ಆದರೆ (errno != EBUSY)
                        ಹಿಂತಿರುಗಿ -errno;
        }

        /* So this is not a VT (in which case we cannot deallocate it), or we failed to deallocate. Let's at
         * least clear the screen. */

        _cleanup_close_ ಪೂರ್ಣಾಂಕ fd2 = open_terminal(tty_ಮಾರ್ಗ, O_WRONLY|O_NOCTTY|O_CLOEXEC|O_NONBLOCK);
        ಆದರೆ (fd2 < 0)
                ಹಿಂತಿರುಗಿ fd2;

        ಹಿಂತಿರುಗಿ loop_write_ತುಂಬಿದ(fd2,
                               "\033[r"   /* clear scrolling region */
                               "\033[H"   /* move home */
                               "\033[3J"  /* clear screen including scrollback, requires Linux 2.6.40 */
                               "\033c",   /* reset to initial state */
                               SIZE_MAX,
                               CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ vt_default_utf8(ಶೂನ್ಯ) {
        /* Read the default VT UTF8 setting from the kernel */
        ಹಿಂತಿರುಗಿ read_boolean_file("/sys/module/vt/parameters/default_utf8");
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ vt_reset_keyboard(ಪೂರ್ಣಾಂಕ fd) {
        ಪೂರ್ಣಾಂಕ r, kb;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        /* If we can't read the default, then default to Unicode. It's 2024 after all. */
        r = vt_default_utf8();
        ಆದರೆ (r < 0)
                log_debug_errno(r, "Failed to determine kernel VT UTF-8 mode, assuming enabled: %m");

        kb = vt_default_utf8() != 0 ? K_UNICODE : K_XLATE;
        ಹಿಂತಿರುಗಿ RET_NERRNO(ioctl(fd, KDSKBMODE, kb));
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ terminal_reset_ioctl(ಪೂರ್ಣಾಂಕ fd, bool switch_ಗಮ್ಯ_ಪಠ್ಯ) {
        ರಚನೆ termios termios;
        ಪೂರ್ಣಾಂಕ r;

        /* Set terminal to some sane defaults */

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        /* We leave locked terminal attributes untouched, so that Plymouth may set whatever it wants to set,
         * and we don't interfere with that. */

        /* Disable exclusive mode, just in case */
        ಆದರೆ (ioctl(fd, TIOCNXCL) < 0)
                log_debug_errno(errno, "TIOCNXCL ioctl failed on TTY, ignoring: %m");

        /* Switch to text mode */
        ಆದರೆ (switch_ಗಮ್ಯ_ಪಠ್ಯ)
                ಆದರೆ (ioctl(fd, KDSETMODE, KD_TEXT) < 0)
                        log_debug_errno(errno, "KDSETMODE ioctl for switching to text mode failed on TTY, ignoring: %m");

        /* Set default keyboard mode */
        r = vt_reset_keyboard(fd);
        ಆದರೆ (r < 0)
                log_debug_errno(r, "Failed to reset VT keyboard, ignoring: %m");

        ಆದರೆ (tcgetattr(fd, &termios) < 0) {
                r = log_debug_errno(errno, "Failed to get terminal parameters: %m");
                ಹೋಗು ಮುಕ್ತಾಯ;
        }

        /* We only reset the stuff that matters to the software. How
         * hardware is set up we don't touch assuming that somebody
         * else will do that for us */

        termios.c_iflag &= ~(IGNBRK | BRKINT | ISTRIP | INLCR | IGNCR | IUCLC);
        termios.c_iflag |= ICRNL | IMAXBEL | IUTF8;
        termios.c_oflag |= ONLCR | OPOST;
        termios.c_cflag |= CREAD;
        termios.c_lflag = ISIG | ICANON | IEXTEN | ECHO | ECHOE | ECHOK | ECHOCTL | ECHOKE;

        termios.c_cc[VINTR]    =   03;  /* ^C */
        termios.c_cc[VQUIT]    =  034;  /* ^\ */
        termios.c_cc[VERASE]   = 0177;
        termios.c_cc[VKILL]    =  025;  /* ^X */
        termios.c_cc[VEOF]     =   04;  /* ^D */
        termios.c_cc[VSTART]   =  021;  /* ^Q */
        termios.c_cc[VSTOP]    =  023;  /* ^S */
        termios.c_cc[VSUSP]    =  032;  /* ^Z */
        termios.c_cc[VLNEXT]   =  026;  /* ^V */
        termios.c_cc[VWERASE]  =  027;  /* ^W */
        termios.c_cc[VREPRINT] =  022;  /* ^R */
        termios.c_cc[VEOL]     =    0;
        termios.c_cc[VEOL2]    =    0;

        termios.c_cc[VTIME]  = 0;
        termios.c_cc[VMIN]   = 1;

        r = RET_NERRNO(tcsetattr(fd, TCSANOW, &termios));
        ಆದರೆ (r < 0)
                log_debug_errno(r, "Failed to set terminal parameters: %m");

ಮುಕ್ತಾಯ:
        /* Just in case, flush all crap out */
        (ಶೂನ್ಯ) tcflush(fd, TCIOFLUSH);

        ಹಿಂತಿರುಗಿ r;
}

ಪೂರ್ಣಾಂಕ terminal_reset_ansi_seq(ಪೂರ್ಣಾಂಕ fd) {
        _cleanup_(nonblock_resetp) ಪೂರ್ಣಾಂಕ nonblock_reset = -EBADF;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        ಆದರೆ (getenv_terminal_is_dumb())
                ಹಿಂತಿರುಗಿ 0;

        r = fd_nonblock(fd, true);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ log_debug_errno(r, "Failed to set terminal to non-blocking mode: %m");
        ಆದರೆ (r > 0)
                nonblock_reset = fd;

        r = loop_write_ತುಂಬಿದ(fd,
                            "\033[!p"              /* soft terminal reset */
                            ANSI_OSC "104" ANSI_ST /* reset color palette via OSC 104 */
                            ANSI_ಸಾಮಾನ್ಯ            /* reset colors */
                            "\033[?7h"             /* enable line-wrapping */
                            "\033[1G"              /* place cursor at beginning of current line */
                            "\033[0J",             /* erase till end of screen */
                            SIZE_MAX,
                            CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        ಆದರೆ (r < 0)
                log_debug_errno(r, "Failed to reset terminal through ANSI sequences: %m");

        ಹಿಂತಿರುಗಿ r;
}

ಶೂನ್ಯ reset_dev_console_fd(ಪೂರ್ಣಾಂಕ fd, bool switch_ಗಮ್ಯ_ಪಠ್ಯ) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        _cleanup_close_ ಪೂರ್ಣಾಂಕ ಬೀಗ_fd = ಬೀಗ_dev_console();
        ಆದರೆ (ಬೀಗ_fd < 0)
                log_debug_errno(ಬೀಗ_fd, "Failed to lock /dev/console, ignoring: %m");

        r = terminal_reset_ioctl(fd, switch_ಗಮ್ಯ_ಪಠ್ಯ);
        ಆದರೆ (r < 0)
                log_warning_errno(r, "Failed to reset /dev/console, ignoring: %m");

        ಚಿಹ್ನೆರಹಿತ ಅಡ್ಡಸಾಲುಗಳು, ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;
        r = proc_cmdline_tty_ಗಾತ್ರ("/dev/console", &ಅಡ್ಡಸಾಲುಗಳು, &ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ);
        ಆದರೆ (r < 0)
                log_warning_errno(r, "Failed to get /dev/console size, ignoring: %m");
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (r > 0) {
                r = terminal_ಗಣ_ಗಾತ್ರ_fd(fd, ಖಾಲಿ, ಅಡ್ಡಸಾಲುಗಳು, ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ);
                ಆದರೆ (r < 0)
                        log_warning_errno(r, "Failed to set configured terminal size on /dev/console, ignoring: %m");
        } ಇಲ್ಲದಿದ್ದರೆ
                (ಶೂನ್ಯ) terminal_fix_ಗಾತ್ರ(fd, fd);

        r = terminal_reset_ansi_seq(fd);
        ಆದರೆ (r < 0)
                log_warning_errno(r, "Failed to reset /dev/console using ANSI sequences, ignoring: %m");
}

ಪೂರ್ಣಾಂಕ ಬೀಗ_dev_console(ಶೂನ್ಯ) {
        _cleanup_close_ ಪೂರ್ಣಾಂಕ fd = -EBADF;
        ಪೂರ್ಣಾಂಕ r;

        /* NB: We do not use O_NOFOLLOW here, because some container managers might place a symlink to some
         * pty in /dev/console, in which case it should be fine to lock the target TTY. */
        fd = open_terminal("/dev/console", O_RDONLY|O_CLOEXEC|O_NOCTTY);
        ಆದರೆ (fd < 0)
                ಹಿಂತಿರುಗಿ fd;

        r = ಬೀಗ_generic(fd, ಬೀಗ_BSD, LOCK_EX);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ TAKE_FD(fd);
}

ಪೂರ್ಣಾಂಕ make_console_stdio(bool switch_ಗಮ್ಯ_ಪಠ್ಯ) {
        ಪೂರ್ಣಾಂಕ fd, r;

        /* Make /dev/console the controlling terminal and stdin/stdout/stderr, if we can. If we can't use
         * /dev/null instead. This is particularly useful if /dev/console is turned off, e.g. if console=null
         * is specified on the kernel command line. */

        fd = acquire_terminal("/dev/console", ACQUIRE_TERMINAL_FORCE|ACQUIRE_TERMINAL_PERMISSIVE, USEC_INFINITY);
        ಆದರೆ (fd < 0) {
                log_warning_errno(fd, "Failed to acquire terminal, using /dev/null stdin/stdout/stderr instead: %m");

                r = make_null_stdio();
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ log_error_errno(r, "Failed to make /dev/null stdin/stdout/stderr: %m");

        } ಇಲ್ಲದಿದ್ದರೆ {
                reset_dev_console_fd(fd, switch_ಗಮ್ಯ_ಪಠ್ಯ);

                r = rearrange_stdio(fd, fd, fd); /* This invalidates 'fd' both on success and on failure. */
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ log_error_errno(r, "Failed to make terminal stdin/stdout/stderr: %m");
        }

        reset_terminal_feature_caches();
        ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ vtnr_from_tty_raw(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *tty, ಚಿಹ್ನೆರಹಿತ *ret) {
        ಪ್ರತಿಜ್ಞಾ(tty);

        tty = skip_dev_ಪೂರ್ವಪ್ರತ್ಯಯ(tty);

        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *e = startswith(tty, "tty");
        ಆದರೆ (!e)
                ಹಿಂತಿರುಗಿ -EINVAL;

        ಹಿಂತಿರುಗಿ ಸುರಕ್ಷಿತ_atou(e, ret);
}

ಪೂರ್ಣಾಂಕ vtnr_from_tty(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *tty) {
        ಚಿಹ್ನೆರಹಿತ u;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(tty);

        r = vtnr_from_tty_raw(tty, &u);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;
        ಆದರೆ (!vtnr_is_valid(u))
                ಹಿಂತಿರುಗಿ -ERANGE;

        ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ) u;
}

bool tty_is_vc(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *tty) {
        ಪ್ರತಿಜ್ಞಾ(tty);

        /* NB: for >= 0 values no range check is conducted here, on the assumption that the caller will
         * either extract vtnr through vtnr_from_tty() later where ERANGE would be reported, or doesn't care
         * about whether it's strictly valid, but only asking "does this fall into the vt category?", for which
         * "yes" seems to be a better answer. */

        ಹಿಂತಿರುಗಿ vtnr_from_tty_raw(tty, /* ret= */ ಖಾಲಿ) >= 0;
}

bool tty_is_console(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *tty) {
        ಪ್ರತಿಜ್ಞಾ(tty);

        ಹಿಂತಿರುಗಿ streq(skip_dev_ಪೂರ್ವಪ್ರತ್ಯಯ(tty), "console");
}

ಪೂರ್ಣಾಂಕ resolve_dev_console(ಅಕ್ಷರ **ret) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        /* Resolve where /dev/console is pointing to. If /dev/console is a symlink (like in container
         * managers), we'll just resolve the symlink. If it's a real device node, we'll use if
         * /sys/class/tty/tty0/active, but only if /sys/ is actually ours (i.e. not read-only-mounted which
         * is a sign for container setups). */

        _cleanup_free_ ಅಕ್ಷರ *chased = ಖಾಲಿ;
        r = chase("/dev/console", /* root= */ ಖಾಲಿ, /* flags= */ 0, &chased, /* ret_fd= */ ಖಾಲಿ);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;
        ಆದರೆ (!ಮಾರ್ಗ_equal(chased, "/dev/console")) {
                *ret = TAKE_PTR(chased);
                ಹಿಂತಿರುಗಿ 0;
        }

        r = ಮಾರ್ಗ_is_read_only_fs("/sys");
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;
        ಆದರೆ (r > 0)
                ಹಿಂತಿರುಗಿ -ENOMEDIUM;

        _cleanup_free_ ಅಕ್ಷರ *ಸಕ್ರಿಯ = ಖಾಲಿ;
        r = read_one_ಸಾಲು_file("/sys/class/tty/console/active", &ಸಕ್ರಿಯ);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;
        ಆದರೆ (r == 0)
                ಹಿಂತಿರುಗಿ -ENXIO;

        /* If multiple log outputs are configured the last one is what /dev/console points to */
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *tty = strrchr(ಸಕ್ರಿಯ, ' ');
        ಆದರೆ (tty)
                tty++;
        ಇಲ್ಲದಿದ್ದರೆ
                tty = ಸಕ್ರಿಯ;

        ಆದರೆ (streq(tty, "tty0")) {
                ಸಕ್ರಿಯ = mfree(ಸಕ್ರಿಯ);

                /* Get the active VC (e.g. tty1) */
                r = read_one_ಸಾಲು_file("/sys/class/tty/tty0/active", &ಸಕ್ರಿಯ);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
                ಆದರೆ (r == 0)
                        ಹಿಂತಿರುಗಿ -ENXIO;

                tty = ಸಕ್ರಿಯ;
        }

        _cleanup_free_ ಅಕ್ಷರ *ಮಾರ್ಗ = ಖಾಲಿ;
        ಮಾರ್ಗ = ಮಾರ್ಗ_join("/dev", tty);
        ಆದರೆ (!ಮಾರ್ಗ)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        *ret = TAKE_PTR(ಮಾರ್ಗ);
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ get_kernel_consoles(ಅಕ್ಷರ ***ret) {
        _cleanup_strv_free_ ಅಕ್ಷರ **l = ಖಾಲಿ;
        _cleanup_free_ ಅಕ್ಷರ *ಸಾಲು = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        /* If /sys/ is mounted read-only this means we are running in some kind of container environment.
         * In that case /sys/ would reflect the host system, not us, hence ignore the data we can read from it. */
        ಆದರೆ (ಮಾರ್ಗ_is_read_only_fs("/sys") > 0)
                ಹೋಗು fallback;

        r = read_one_ಸಾಲು_file("/sys/class/tty/console/active", &ಸಾಲು);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = ಸಾಲು;;) {
                _cleanup_free_ ಅಕ್ಷರ *tty = ಖಾಲಿ, *ಮಾರ್ಗ = ಖಾಲಿ;

                r = ಹೊರತೆಗೆ_ಮೊದಲ_ಶಬ್ದ(&p, &tty, ಖಾಲಿ, 0);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
                ಆದರೆ (r == 0)
                        ನಿಲ್ಲಿಸು;

                ಆದರೆ (streq(tty, "tty0")) {
                        tty = mfree(tty);
                        r = read_one_ಸಾಲು_file("/sys/class/tty/tty0/active", &tty);
                        ಆದರೆ (r < 0)
                                ಹಿಂತಿರುಗಿ r;
                        ಆದರೆ (r == 0) {
                                log_debug("No VT active, skipping /dev/tty0.");
                                ಮುಂದುವರಿ;
                        }
                }

                ಮಾರ್ಗ = ಮಾರ್ಗ_join("/dev", tty);
                ಆದರೆ (!ಮಾರ್ಗ)
                        ಹಿಂತಿರುಗಿ -ENOMEM;

                ಆದರೆ (access(ಮಾರ್ಗ, F_OK) < 0) {
                        log_debug_errno(errno, "Console device %s is not accessible, skipping: %m", ಮಾರ್ಗ);
                        ಮುಂದುವರಿ;
                }

                r = strv_consume(&l, TAKE_PTR(ಮಾರ್ಗ));
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
        }

        ಆದರೆ (strv_isempty(l)) {
                log_debug("No devices found for system console");
                ಹೋಗು fallback;
        }

        *ret = TAKE_PTR(l);
        ಹಿಂತಿರುಗಿ strv_ಉದ್ದಳತೆ(*ret);

fallback:
        r = strv_extend(&l, "/dev/console");
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        *ret = TAKE_PTR(l);
        ಹಿಂತಿರುಗಿ 0;
}

bool tty_is_vc_resolve(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *tty) {
        _cleanup_free_ ಅಕ್ಷರ *resolved = ಖಾಲಿ;

        ಪ್ರತಿಜ್ಞಾ(tty);

        ಆದರೆ (streq(skip_dev_ಪೂರ್ವಪ್ರತ್ಯಯ(tty), "console")) {
                ಆದರೆ (resolve_dev_console(&resolved) < 0)
                        ಹಿಂತಿರುಗಿ false;

                tty = resolved;
        }

        ಹಿಂತಿರುಗಿ tty_is_vc(tty);
}

ಪೂರ್ಣಾಂಕ fd_columns(ಪೂರ್ಣಾಂಕ fd) {
        ರಚನೆ winsize ws = {};

        ಆದರೆ (fd < 0)
                ಹಿಂತಿರುಗಿ -EBADF;

        ಆದರೆ (ioctl(fd, TIOCGWINSZ, &ws) < 0)
                ಹಿಂತಿರುಗಿ -errno;

        ಆದರೆ (ws.ws_col <= 0)
                ಹಿಂತಿರುಗಿ -ENODATA; /* some tty types come up with invalid row/column initially, return a recognizable error for that */

        ಹಿಂತಿರುಗಿ ws.ws_col;
}

ಪೂರ್ಣಾಂಕ getenv_columns(ಶೂನ್ಯ) {
        ಪೂರ್ಣಾಂಕ r;

        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *e = getenv("COLUMNS");
        ಆದರೆ (!e)
                ಹಿಂತಿರುಗಿ -ENXIO;

        ಚಿಹ್ನೆರಹಿತ c;
        r = ಸುರಕ್ಷಿತ_atou_bounded(e, 1, USHRT_MAX, &c);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ) c;
}

ಚಿಹ್ನೆರಹಿತ columns(ಶೂನ್ಯ) {

        ಆದರೆ (cached_columns > 0)
                ಹಿಂತಿರುಗಿ cached_columns;

        ಪೂರ್ಣಾಂಕ c = getenv_columns();
        ಆದರೆ (c < 0) {
                c = fd_columns(STDOUT_FILENO);
                ಆದರೆ (c < 0)
                        c = 80;
        }

        ಪ್ರತಿಜ್ಞಾ(c > 0);

        cached_columns = c;
        ಹಿಂತಿರುಗಿ cached_columns;
}

ಪೂರ್ಣಾಂಕ fd_ಸಾಲುಗಳು(ಪೂರ್ಣಾಂಕ fd) {
        ರಚನೆ winsize ws = {};

        ಆದರೆ (fd < 0)
                ಹಿಂತಿರುಗಿ -EBADF;

        ಆದರೆ (ioctl(fd, TIOCGWINSZ, &ws) < 0)
                ಹಿಂತಿರುಗಿ -errno;

        ಆದರೆ (ws.ws_row <= 0)
                ಹಿಂತಿರುಗಿ -ENODATA; /* some tty types come up with invalid row/column initially, return a recognizable error for that */

        ಹಿಂತಿರುಗಿ ws.ws_row;
}

ಚಿಹ್ನೆರಹಿತ ಸಾಲುಗಳು(ಶೂನ್ಯ) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *e;
        ಪೂರ್ಣಾಂಕ l;

        ಆದರೆ (cached_ಸಾಲುಗಳು > 0)
                ಹಿಂತಿರುಗಿ cached_ಸಾಲುಗಳು;

        l = 0;
        e = getenv("LINES");
        ಆದರೆ (e)
                (ಶೂನ್ಯ) ಸುರಕ್ಷಿತ_atoi(e, &l);

        ಆದರೆ (l <= 0 || l > USHRT_MAX) {
                l = fd_ಸಾಲುಗಳು(STDOUT_FILENO);
                ಆದರೆ (l <= 0)
                        l = 24;
        }

        cached_ಸಾಲುಗಳು = l;
        ಹಿಂತಿರುಗಿ cached_ಸಾಲುಗಳು;
}

ಪೂರ್ಣಾಂಕ terminal_ಗಣ_ಗಾತ್ರ_fd(ಪೂರ್ಣಾಂಕ fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ident, ಚಿಹ್ನೆರಹಿತ ಅಡ್ಡಸಾಲುಗಳು, ಚಿಹ್ನೆರಹಿತ ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ) {
        ರಚನೆ winsize ws;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        ಆದರೆ (!ident)
                ident = "TTY";

        ಆದರೆ (ಅಡ್ಡಸಾಲುಗಳು == UINT_MAX && ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ == UINT_MAX)
                ಹಿಂತಿರುಗಿ 0;

        ಆದರೆ (ioctl(fd, TIOCGWINSZ, &ws) < 0)
                ಹಿಂತಿರುಗಿ log_debug_errno(errno,
                                       "TIOCGWINSZ ioctl for getting %s size failed, not setting terminal size: %m",
                                       ident);

        ಆದರೆ (ಅಡ್ಡಸಾಲುಗಳು == UINT_MAX)
                ಅಡ್ಡಸಾಲುಗಳು = ws.ws_row;
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಅಡ್ಡಸಾಲುಗಳು > USHRT_MAX)
                ಅಡ್ಡಸಾಲುಗಳು = USHRT_MAX;

        ಆದರೆ (ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ == UINT_MAX)
                ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = ws.ws_col;
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ > USHRT_MAX)
                ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = USHRT_MAX;

        ಆದರೆ (ಅಡ್ಡಸಾಲುಗಳು == ws.ws_row && ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ == ws.ws_col)
                ಹಿಂತಿರುಗಿ 0;

        ws.ws_row = ಅಡ್ಡಸಾಲುಗಳು;
        ws.ws_col = ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;

        ಆದರೆ (ioctl(fd, TIOCSWINSZ, &ws) < 0)
                ಹಿಂತಿರುಗಿ log_debug_errno(errno, "TIOCSWINSZ ioctl for setting %s size failed: %m", ident);

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ proc_cmdline_tty_ಗಾತ್ರ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *tty, ಚಿಹ್ನೆರಹಿತ *ret_ಅಡ್ಡಸಾಲುಗಳು, ಚಿಹ್ನೆರಹಿತ *ret_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ) {
        _cleanup_free_ ಅಕ್ಷರ *rowskey = ಖಾಲಿ, *rowsvalue = ಖಾಲಿ, *colskey = ಖಾಲಿ, *colsvalue = ಖಾಲಿ;
        ಚಿಹ್ನೆರಹಿತ ಅಡ್ಡಸಾಲುಗಳು = UINT_MAX, ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = UINT_MAX;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(tty);

        ಆದರೆ (!ret_ಅಡ್ಡಸಾಲುಗಳು && !ret_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ)
                ಹಿಂತಿರುಗಿ 0;

        tty = skip_dev_ಪೂರ್ವಪ್ರತ್ಯಯ(tty);
        ಆದರೆ (ಮಾರ್ಗ_startswith(tty, "pts/"))
                ಹಿಂತಿರುಗಿ -EMEDIUMTYPE;
        ಆದರೆ (!ಒಳ_charset(tty, ALPHANUMERICAL))
                ಹಿಂತಿರುಗಿ log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "TTY name '%s' contains non-alphanumeric characters, not searching kernel cmdline for size.", tty);

        rowskey = strjoin("systemd.tty.rows.", tty);
        ಆದರೆ (!rowskey)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        colskey = strjoin("systemd.tty.columns.", tty);
        ಆದರೆ (!colskey)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        r = proc_cmdline_get_ಕೀಲಿ_many(/* flags= */ 0,
                                      rowskey, &rowsvalue,
                                      colskey, &colsvalue);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ log_debug_errno(r, "Failed to read TTY size of %s from kernel cmdline: %m", tty);

        ಆದರೆ (rowsvalue) {
                r = ಸುರಕ್ಷಿತ_atou(rowsvalue, &ಅಡ್ಡಸಾಲುಗಳು);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ log_debug_errno(r, "Failed to parse %s=%s: %m", rowskey, rowsvalue);
        }

        ಆದರೆ (colsvalue) {
                r = ಸುರಕ್ಷಿತ_atou(colsvalue, &ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ log_debug_errno(r, "Failed to parse %s=%s: %m", colskey, colsvalue);
        }

        ಆದರೆ (ret_ಅಡ್ಡಸಾಲುಗಳು)
                *ret_ಅಡ್ಡಸಾಲುಗಳು = ಅಡ್ಡಸಾಲುಗಳು;
        ಆದರೆ (ret_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ)
                *ret_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;

        ಹಿಂತಿರುಗಿ ಅಡ್ಡಸಾಲುಗಳು != UINT_MAX || ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ != UINT_MAX;
}

/* intended to be used as a SIGWINCH sighandler */
ಶೂನ್ಯ columns_ಸಾಲುಗಳು_cache_reset(ಪೂರ್ಣಾಂಕ signum) {
        cached_columns = 0;
        cached_ಸಾಲುಗಳು = 0;
}

ಶೂನ್ಯ reset_terminal_feature_caches(ಶೂನ್ಯ) {
        cached_columns = 0;
        cached_ಸಾಲುಗಳು = 0;

        cached_on_tty = -1;
        cached_on_dev_null = -1;

        reset_ansi_feature_caches();
}

bool on_tty(ಶೂನ್ಯ) {

        /* We check both stdout and stderr, so that situations where pipes on the shell are used are reliably
         * recognized, regardless if only the output or the errors are piped to some place. Since on_tty() is generally
         * used to default to a safer, non-interactive, non-color mode of operation it's probably good to be defensive
         * here, and check for both. Note that we don't check for STDIN_FILENO, because it should fine to use fancy
         * terminal functionality when outputting stuff, even if the input is piped to us. */

        ಆದರೆ (cached_on_tty < 0)
                cached_on_tty =
                        isatty_ಸುರಕ್ಷಿತ(STDOUT_FILENO) &&
                        isatty_ಸುರಕ್ಷಿತ(STDERR_FILENO);

        ಹಿಂತಿರುಗಿ cached_on_tty;
}

ಪೂರ್ಣಾಂಕ getttyname_malloc(ಪೂರ್ಣಾಂಕ fd, ಅಕ್ಷರ **ret) {
        ಅಕ್ಷರ ಮಾರ್ಗ[PATH_MAX]; /* PATH_MAX is counted *with* the trailing NUL byte */
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);
        ಪ್ರತಿಜ್ಞಾ(ret);

        r = ttyname_r(fd, ಮಾರ್ಗ, ಗಾತ್ರ ಮಾರ್ಗ); /* positive error */
        ಪ್ರತಿಜ್ಞಾ(r >= 0);
        ಆದರೆ (r == ERANGE)
                ಹಿಂತಿರುಗಿ -ENAMETOOLONG;
        ಆದರೆ (r > 0)
                ಹಿಂತಿರುಗಿ -r;

        ಹಿಂತಿರುಗಿ strdup_ಗಮ್ಯ(ret, skip_dev_ಪೂರ್ವಪ್ರತ್ಯಯ(ಮಾರ್ಗ));
}

ಪೂರ್ಣಾಂಕ getttyname_harder(ಪೂರ್ಣಾಂಕ fd, ಅಕ್ಷರ **ret) {
        _cleanup_free_ ಅಕ್ಷರ *s = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        r = getttyname_malloc(fd, &s);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (streq(s, "tty"))
                ಹಿಂತಿರುಗಿ get_ctty(0, ಖಾಲಿ, ret);

        *ret = TAKE_PTR(s);
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ get_ctty_devnr(pid_t pid, dev_t *ret) {
        _cleanup_free_ ಅಕ್ಷರ *ಸಾಲು = ಖಾಲಿ;
        ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ttynr;
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(pid >= 0);

        p = procfs_file_alloca(pid, "stat");
        r = read_one_ಸಾಲು_file(p, &ಸಾಲು);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        p = strrchr(ಸಾಲು, ')');
        ಆದರೆ (!p)
                ಹಿಂತಿರುಗಿ -EIO;

        p++;

        ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಓದು(p, " "
                   "%*c "  /* state */
                   "%*d "  /* ppid */
                   "%*d "  /* pgrp */
                   "%*d "  /* session */
                   "%lu ", /* ttynr */
                   &ttynr) != 1)
                ಹಿಂತಿರುಗಿ -EIO;

        ಆದರೆ (devnum_is_zero(ttynr))
                ಹಿಂತಿರುಗಿ -ENXIO;

        ಆದರೆ (ret)
                *ret = (dev_t) ttynr;

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ get_ctty(pid_t pid, dev_t *ret_devnr, ಅಕ್ಷರ **ret) {
        ಅಕ್ಷರ pty[STRLEN("/dev/pts/") + DECIMAL_STR_MAX(dev_t) + 1];
        _cleanup_free_ ಅಕ್ಷರ *buf = ಖಾಲಿ;
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fn = ಖಾಲಿ, *w;
        dev_t devnr;
        ಪೂರ್ಣಾಂಕ r;

        r = get_ctty_devnr(pid, &devnr);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        r = device_ಮಾರ್ಗ_make_canonical(S_IFCHR, devnr, &buf);
        ಆದರೆ (r < 0) {
                ರಚನೆ stat st;

                ಆದರೆ (r != -ENOENT) /* No symlink for this in /dev/char/? */
                        ಹಿಂತಿರುಗಿ r;

                /* Maybe this is PTY? PTY devices are not listed in /dev/char/, as they don't follow the
                 * Linux device model and hence device_path_make_canonical() doesn't work for them. Let's
                 * assume this is a PTY for a moment, and check if the device node this would then map to in
                 * /dev/pts/ matches the one we are looking for. This way we don't have to hardcode the major
                 * number (which is 136 btw), but we still rely on the fact that PTY numbers map directly to
                 * the minor number of the pty. */
                xsprintf(pty, "/dev/pts/%u", minor(devnr));

                ಆದರೆ (stat(pty, &st) < 0) {
                        ಆದರೆ (errno != ENOENT)
                                ಹಿಂತಿರುಗಿ -errno;

                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (S_ISCHR(st.st_mode) && devnr == st.st_rdev) /* Bingo! */
                        fn = pty;

                ಆದರೆ (!fn) {
                        /* Doesn't exist, or not a PTY? Probably something similar to the PTYs which have no
                         * symlink in /dev/char/. Let's return something vaguely useful. */
                        r = device_ಮಾರ್ಗ_make_major_minor(S_IFCHR, devnr, &buf);
                        ಆದರೆ (r < 0)
                                ಹಿಂತಿರುಗಿ r;

                        fn = buf;
                }
        } ಇಲ್ಲದಿದ್ದರೆ
                fn = buf;

        w = ಮಾರ್ಗ_startswith(fn, "/dev/");
        ಆದರೆ (!w)
                ಹಿಂತಿರುಗಿ -EINVAL;

        ಆದರೆ (ret) {
                r = strdup_ಗಮ್ಯ(ret, w);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
        }

        ಆದರೆ (ret_devnr)
                *ret_devnr = devnr;

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ ptsname_malloc(ಪೂರ್ಣಾಂಕ fd, ಅಕ್ಷರ **ret) {
        ಪ್ರತಿಜ್ಞಾ(fd >= 0);
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಗಾಗಿ (size_t l = 50;;) {
                _cleanup_free_ ಅಕ್ಷರ *c = ಖಾಲಿ;

                c = new(ಅಕ್ಷರ, l);
                ಆದರೆ (!c)
                        ಹಿಂತಿರುಗಿ -ENOMEM;

                ಆದರೆ (ptsname_r(fd, c, l) >= 0) {
                        *ret = TAKE_PTR(c);
                        ಹಿಂತಿರುಗಿ 0;
                }
                ಆದರೆ (errno != ERANGE)
                        ಹಿಂತಿರುಗಿ -errno;

                ಆದರೆ (!MUL_ನಿಯೋಜಿಸು_ಸುರಕ್ಷಿತ(&l, 2))
                        ಹಿಂತಿರುಗಿ -ENOMEM;
        }
}

ಪೂರ್ಣಾಂಕ openpt_allocate(ಪೂರ್ಣಾಂಕ flags, ಅಕ್ಷರ **ret_peer_ಮಾರ್ಗ) {
        _cleanup_close_ ಪೂರ್ಣಾಂಕ fd = -EBADF;
        ಪೂರ್ಣಾಂಕ r;

        fd = posix_openpt(flags|O_NOCTTY|O_CLOEXEC);
        ಆದರೆ (fd < 0)
                ಹಿಂತಿರುಗಿ -errno;

        _cleanup_free_ ಅಕ್ಷರ *p = ಖಾಲಿ;
        ಆದರೆ (ret_peer_ಮಾರ್ಗ) {
                r = ptsname_malloc(fd, &p);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;

                ಆದರೆ (!ಮಾರ್ಗ_startswith(p, "/dev/pts/"))
                        ಹಿಂತಿರುಗಿ -EINVAL;
        }

        ಆದರೆ (unlockpt(fd) < 0)
                ಹಿಂತಿರುಗಿ -errno;

        ಆದರೆ (ret_peer_ಮಾರ್ಗ)
                *ret_peer_ಮಾರ್ಗ = TAKE_PTR(p);

        ಹಿಂತಿರುಗಿ TAKE_FD(fd);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ptsname_namespace(ಪೂರ್ಣಾಂಕ pty, ಅಕ್ಷರ **ret) {
        ಪೂರ್ಣಾಂಕ no = -1;

        ಪ್ರತಿಜ್ಞಾ(pty >= 0);
        ಪ್ರತಿಜ್ಞಾ(ret);

        /* Like ptsname(), but doesn't assume that the path is
         * accessible in the local namespace. */

        ಆದರೆ (ioctl(pty, TIOCGPTN, &no) < 0)
                ಹಿಂತಿರುಗಿ -errno;

        ಆದರೆ (no < 0)
                ಹಿಂತಿರುಗಿ -EIO;

        ಆದರೆ (asprintf(ret, "/dev/pts/%i", no) < 0)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ openpt_allocate_ಒಳ_namespace(
                ಸ್ಥಿರಾಂಕ PidRef *pidref,
                ಪೂರ್ಣಾಂಕ flags,
                ಅಕ್ಷರ **ret_peer_ಮಾರ್ಗ) {

        _cleanup_close_ ಪೂರ್ಣಾಂಕ pidnsfd = -EBADF, mntnsfd = -EBADF, usernsfd = -EBADF, rootfd = -EBADF, fd = -EBADF;
        _cleanup_close_pair_ ಪೂರ್ಣಾಂಕ pair[2] = EBADF_PAIR;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(pidref);

        r = pidref_namespace_open(pidref, &pidnsfd, &mntnsfd, /* ret_netns_fd= */ ಖಾಲಿ, &usernsfd, &rootfd);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ log_debug_errno(r, "Failed to open namespaces of PID "PID_FMT": %m", pidref->pid);

        ಆದರೆ (socketpair(AF_UNIX, SOCK_DGRAM|SOCK_CLOEXEC, 0, pair) < 0)
                ಹಿಂತಿರುಗಿ -errno;

        r = namespace_fork(
                        "(sd-openptns)",
                        "(sd-openpt)",
                        FORK_RESET_ಸಂಕೇತಗಳು|FORK_DEATHSIG_SIGKILL|FORK_WAIT,
                        pidnsfd,
                        mntnsfd,
                        /* netns_fd= */ -EBADF,
                        usernsfd,
                        rootfd,
                        /* ret= */ ಖಾಲಿ);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;
        ಆದರೆ (r == 0) {
                pair[0] = ಸುರಕ್ಷಿತ_close(pair[0]);

                fd = openpt_allocate(flags, /* ret_peer_path= */ ಖಾಲಿ);
                ಆದರೆ (fd < 0)
                        _exit(EXIT_FAILURE);

                ಆದರೆ (send_one_fd(pair[1], fd, 0) < 0)
                        _exit(EXIT_FAILURE);

                _exit(EXIT_SUCCESS);
        }

        pair[1] = ಸುರಕ್ಷಿತ_close(pair[1]);

        fd = receive_one_fd(pair[0], 0);
        ಆದರೆ (fd < 0)
                ಹಿಂತಿರುಗಿ fd;

        ಆದರೆ (ret_peer_ಮಾರ್ಗ) {
                r = ptsname_namespace(fd, ret_peer_ಮಾರ್ಗ);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
        }

        ಹಿಂತಿರುಗಿ TAKE_FD(fd);
}

ಸ್ಥಿರ bool on_dev_null(ಶೂನ್ಯ) {
        ರಚನೆ stat dst, ost, est;

        ಆದರೆ (cached_on_dev_null >= 0)
                ಹಿಂತಿರುಗಿ cached_on_dev_null;

        ಆದರೆ (stat("/dev/null", &dst) < 0 || fstat(STDOUT_FILENO, &ost) < 0 || fstat(STDERR_FILENO, &est) < 0)
                cached_on_dev_null = false;
        ಇಲ್ಲದಿದ್ದರೆ
                cached_on_dev_null = stat_inode_same(&dst, &ost) && stat_inode_same(&dst, &est);

        ಹಿಂತಿರುಗಿ cached_on_dev_null;
}

bool ಪದ_env_valid(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಪದ) {
        /* Checks if the specified $TERM value is suitable for propagation, i.e. is not empty, not set to
         * "unknown" (as is common in CI), and only contains characters valid in terminal type names.
         * Valid $TERM values are things like "xterm-256color", "linux", "screen.xterm-256color", i.e.
         * alphanumeric characters, hyphens, underscores, dots, and plus signs. */
        ಹಿಂತಿರುಗಿ !isempty(ಪದ) &&
                !streq(ಪದ, "unknown") &&
                ಒಳ_charset(ಪದ, ALPHANUMERICAL "-_+.");
}

bool getenv_terminal_is_dumb(ಶೂನ್ಯ) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *e;

        e = getenv("TERM");
        ಆದರೆ (!e)
                ಹಿಂತಿರುಗಿ true;

        ಹಿಂತಿರುಗಿ streq(e, "dumb");
}

bool terminal_is_dumb(ಶೂನ್ಯ) {
        ಆದರೆ (!on_tty() && !on_dev_null())
                ಹಿಂತಿರುಗಿ true;

        ಹಿಂತಿರುಗಿ getenv_terminal_is_dumb();
}

bool dev_console_colors_enabled(ಶೂನ್ಯ) {
        _cleanup_free_ ಅಕ್ಷರ *s = ಖಾಲಿ;
        Color_ಬಹುಲಕ m;

        /* Returns true if we assume that color is supported on /dev/console.
         *
         * For that we first check if we explicitly got told to use colors or not, by checking $SYSTEMD_COLORS. If that
         * isn't set we check whether PID 1 has $TERM set, and if not, whether TERM is set on the kernel command
         * line. If we find $TERM set we assume color if it's not set to "dumb", similarly to how regular
         * colors_enabled() operates. */

        m = parse_systemd_colors();
        ಆದರೆ (m >= 0)
                ಹಿಂತಿರುಗಿ m;

        ಆದರೆ (getenv("NO_COLOR"))
                ಹಿಂತಿರುಗಿ false;

        ಆದರೆ (getenv_for_pid(1, "TERM", &s) <= 0)
                (ಶೂನ್ಯ) proc_cmdline_get_ಕೀಲಿ("TERM", 0, &s);

        ಹಿಂತಿರುಗಿ !streq_ptr(s, "dumb");
}

ಪೂರ್ಣಾಂಕ vt_restore(ಪೂರ್ಣಾಂಕ fd) {

        ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ vt_mode ಬಹುಲಕ = {
                .ಬಹುಲಕ = VT_AUTO,
        };

        ಪೂರ್ಣಾಂಕ r, ret = 0;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        ಆದರೆ (!isatty_ಸುರಕ್ಷಿತ(fd))
                ಹಿಂತಿರುಗಿ log_debug_errno(SYNTHETIC_ERRNO(ENOTTY), "Asked to restore the VT for an fd that does not refer to a terminal.");

        ಆದರೆ (ioctl(fd, KDSETMODE, KD_TEXT) < 0)
                RET_GATHER(ret, log_debug_errno(errno, "Failed to set VT to text mode, ignoring: %m"));

        r = vt_reset_keyboard(fd);
        ಆದರೆ (r < 0)
                RET_GATHER(ret, log_debug_errno(r, "Failed to reset keyboard mode, ignoring: %m"));

        ಆದರೆ (ioctl(fd, VT_SETMODE, &ಬಹುಲಕ) < 0)
                RET_GATHER(ret, log_debug_errno(errno, "Failed to set VT_AUTO mode, ignoring: %m"));

        r = fchmod_and_chown(fd, TTY_ಬಹುಲಕ, 0, GID_INVALID);
        ಆದರೆ (r < 0)
                RET_GATHER(ret, log_debug_errno(r, "Failed to chmod()/chown() VT, ignoring: %m"));

        ಹಿಂತಿರುಗಿ ret;
}

ಪೂರ್ಣಾಂಕ vt_release(ಪೂರ್ಣಾಂಕ fd, bool restore) {
        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        /* This function releases the VT by acknowledging the VT-switch signal
         * sent by the kernel and optionally reset the VT in text and auto
         * VT-switching modes. */

        ಆದರೆ (!isatty_ಸುರಕ್ಷಿತ(fd))
                ಹಿಂತಿರುಗಿ log_debug_errno(SYNTHETIC_ERRNO(ENOTTY), "Asked to release the VT for an fd that does not refer to a terminal.");

        ಆದರೆ (ioctl(fd, VT_RELDISP, 1) < 0)
                ಹಿಂತಿರುಗಿ -errno;

        ಆದರೆ (restore)
                ಹಿಂತಿರುಗಿ vt_restore(fd);

        ಹಿಂತಿರುಗಿ 0;
}

ಶೂನ್ಯ get_log_colors(ಪೂರ್ಣಾಂಕ priority, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **on, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **off, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **highlight) {
        /* Note that this will initialize output variables only when there's something to output.
         * The caller must pre-initialize to "" or NULL as appropriate. */

        ಆದರೆ (priority <= LOG_ERR) {
                ಆದರೆ (on)
                        *on = ansi_highlight_red();
                ಆದರೆ (off)
                        *off = ansi_ಸಾಮಾನ್ಯ();
                ಆದರೆ (highlight)
                        *highlight = ansi_highlight();

        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (priority <= LOG_WARNING) {
                ಆದರೆ (on)
                        *on = ansi_highlight_yellow();
                ಆದರೆ (off)
                        *off = ansi_ಸಾಮಾನ್ಯ();
                ಆದರೆ (highlight)
                        *highlight = ansi_highlight();

        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (priority <= LOG_NOTICE) {
                ಆದರೆ (on)
                        *on = ansi_highlight();
                ಆದರೆ (off)
                        *off = ansi_ಸಾಮಾನ್ಯ();
                ಆದರೆ (highlight)
                        *highlight = ansi_highlight_red();

        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (priority >= LOG_DEBUG) {
                ಆದರೆ (on)
                        *on = ansi_grey();
                ಆದರೆ (off)
                        *off = ansi_ಸಾಮಾನ್ಯ();
                ಆದರೆ (highlight)
                        *highlight = ansi_highlight_red();
        }
}

ಪೂರ್ಣಾಂಕ terminal_ಗಣ_cursor_position(ಪೂರ್ಣಾಂಕ fd, ಚಿಹ್ನೆರಹಿತ ಅಡ್ಡಸಾಲು, ಚಿಹ್ನೆರಹಿತ ಸ್ತಂಭ) {
        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        ಅಕ್ಷರ cursor_position[STRLEN("\x1B[" ";" "H") + DECIMAL_STR_MAX(ಚಿಹ್ನೆರಹಿತ) * 2 + 1];
        xsprintf(cursor_position, "\x1B[%u;%uH", ಅಡ್ಡಸಾಲು, ಸ್ತಂಭ);

        ಹಿಂತಿರುಗಿ loop_write(fd, cursor_position, SIZE_MAX);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ terminal_verify_same(ಪೂರ್ಣಾಂಕ ಒಳಹರಿವು_fd, ಪೂರ್ಣಾಂಕ output_fd) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ಒಳಹರಿವು_fd >= 0);
        ಪ್ರತಿಜ್ಞಾ(output_fd >= 0);

        /* Validates that the specified fds reference the same TTY */

        ಆದರೆ (ಒಳಹರಿವು_fd != output_fd) {
                ರಚನೆ stat sti;
                ಆದರೆ (fstat(ಒಳಹರಿವು_fd, &sti) < 0)
                        ಹಿಂತಿರುಗಿ -errno;

                r = stat_verify_char(&sti); /* TTYs are character devices */
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;

                ರಚನೆ stat sto;
                ಆದರೆ (fstat(output_fd, &sto) < 0)
                        ಹಿಂತಿರುಗಿ -errno;

                r = stat_verify_char(&sto);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;

                ಆದರೆ (sti.st_rdev != sto.st_rdev)
                        ಹಿಂತಿರುಗಿ -ENOLINK;
        }

        ಆದರೆ (!isatty_ಸುರಕ್ಷಿತ(ಒಳಹರಿವು_fd)) /* The check above was just for char device, but now let's ensure it's actually a tty */
                ಹಿಂತಿರುಗಿ -ENOTTY;

        ಹಿಂತಿರುಗಿ 0;
}

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ CursorPositionState {
        CURSOR_ಪಠ್ಯ,
        CURSOR_ESCAPE,
        CURSOR_ಅಡ್ಡಸಾಲು,
        CURSOR_ಸ್ತಂಭ,
} CursorPositionState;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ CursorPositionContext {
        CursorPositionState state;
        ಚಿಹ್ನೆರಹಿತ ಅಡ್ಡಸಾಲು, ಸ್ತಂಭ;
} CursorPositionContext;

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಪರಿಶೀಲನೆ_cursor_position_response(
                CursorPositionContext *context,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *buf,
                size_t ಗಾತ್ರ_2,
                size_t *ret_processed) {

        ಪ್ರತಿಜ್ಞಾ(context);
        ಪ್ರತಿಜ್ಞಾ(buf);
        ಪ್ರತಿಜ್ಞಾ(ret_processed);

        ಗಾಗಿ (size_t i = 0; i < ಗಾತ್ರ_2; i++) {
                ಅಕ್ಷರ c = buf[i];

                ಆಯ್ಕೆ (context->state) {

                ಸ್ಥಿತಿ CURSOR_ಪಠ್ಯ:
                        context->state = c == '\x1B' ? CURSOR_ESCAPE : CURSOR_ಪಠ್ಯ;
                        ನಿಲ್ಲಿಸು;

                ಸ್ಥಿತಿ CURSOR_ESCAPE:
                        context->state = c == '[' ? CURSOR_ಅಡ್ಡಸಾಲು : CURSOR_ಪಠ್ಯ;
                        ನಿಲ್ಲಿಸು;

                ಸ್ಥಿತಿ CURSOR_ಅಡ್ಡಸಾಲು:
                        ಆದರೆ (c == ';')
                                context->state = context->ಅಡ್ಡಸಾಲು > 0 ? CURSOR_ಸ್ತಂಭ : CURSOR_ಪಠ್ಯ;
                        ಇಲ್ಲದಿದ್ದರೆ {
                                ಪೂರ್ಣಾಂಕ d = undecchar(c);

                                /* We read a decimal character, let's suffix it to the number we so far read,
                                 * but let's do an overflow check first. */
                                ಆದರೆ (d < 0 || context->ಅಡ್ಡಸಾಲು > (UINT_MAX-d)/10)
                                        context->state = CURSOR_ಪಠ್ಯ;
                                ಇಲ್ಲದಿದ್ದರೆ
                                        context->ಅಡ್ಡಸಾಲು = context->ಅಡ್ಡಸಾಲು * 10 + d;
                        }
                        ನಿಲ್ಲಿಸು;

                ಸ್ಥಿತಿ CURSOR_ಸ್ತಂಭ:
                        ಆದರೆ (c == 'R') {
                                ಆದರೆ (context->ಸ್ತಂಭ > 0) {
                                        *ret_processed = i + 1;
                                        ಹಿಂತಿರುಗಿ 1; /* success! */
                                }

                                context->state = CURSOR_ಪಠ್ಯ;
                        } ಇಲ್ಲದಿದ್ದರೆ {
                                ಪೂರ್ಣಾಂಕ d = undecchar(c);

                                /* As above, add the decimal character to our column number */
                                ಆದರೆ (d < 0 || context->ಸ್ತಂಭ > (UINT_MAX-d)/10)
                                        context->state = CURSOR_ಪಠ್ಯ;
                                ಇಲ್ಲದಿದ್ದರೆ
                                        context->ಸ್ತಂಭ = context->ಸ್ತಂಭ * 10 + d;
                        }

                        ನಿಲ್ಲಿಸು;
                }

                /* Reset any positions we might have picked up */
                ಆದರೆ (ಒಳ_ಗಣ(context->state, CURSOR_ಪಠ್ಯ, CURSOR_ESCAPE))
                        context->ಅಡ್ಡಸಾಲು = context->ಸ್ತಂಭ = 0;
        }

        *ret_processed = ಗಾತ್ರ_2;
        ಹಿಂತಿರುಗಿ 0; /* all good, but not enough data yet */
}

ಪೂರ್ಣಾಂಕ terminal_get_cursor_position(
                ಪೂರ್ಣಾಂಕ ಒಳಹರಿವು_fd,
                ಪೂರ್ಣಾಂಕ output_fd,
                ಚಿಹ್ನೆರಹಿತ *ret_ಅಡ್ಡಸಾಲು,
                ಚಿಹ್ನೆರಹಿತ *ret_ಸ್ತಂಭ) {

        _cleanup_close_ ಪೂರ್ಣಾಂಕ nonblock_ಒಳಹರಿವು_fd = -EBADF;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ಒಳಹರಿವು_fd >= 0);
        ಪ್ರತಿಜ್ಞಾ(output_fd >= 0);

        ಆದರೆ (getenv_terminal_is_dumb())
                ಹಿಂತಿರುಗಿ -EOPNOTSUPP;

        r = terminal_verify_same(ಒಳಹರಿವು_fd, output_fd);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ log_debug_errno(r, "Called with distinct input/output fds: %m");

        /* Failure to reset the terminal is ignored here and in similar cases below.
         * We already have our result; if cleanup fails it doesn't change the validity of the result. */
        ರಚನೆ termios old_termios = TERMIOS_NULL;
        CLEANUP_TERMIOS_RESET(ಒಳಹರಿವು_fd, old_termios);

        ಆದರೆ (tcgetattr(ಒಳಹರಿವು_fd, &old_termios) < 0)
                ಹಿಂತಿರುಗಿ log_debug_errno(errno, "Failed to get terminal settings: %m");

        ರಚನೆ termios new_termios = old_termios;
        termios_disable_echo(&new_termios);

        ಆದರೆ (tcsetattr(ಒಳಹರಿವು_fd, TCSANOW, &new_termios) < 0)
                ಹಿಂತಿರುಗಿ log_debug_errno(errno, "Failed to set new terminal settings: %m");

        /* Request cursor position (DSR/CPR) */
        r = loop_write(output_fd, "\x1B[6n", SIZE_MAX);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        /* Open a 2nd input fd, in non-blocking mode, so that we won't ever hang in read() should someone
         * else process the POLLIN. */

        nonblock_ಒಳಹರಿವು_fd = r = fd_reopen(ಒಳಹರಿವು_fd, O_RDONLY|O_CLOEXEC|O_NONBLOCK|O_NOCTTY);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        usec_t ಕೊನೆ = usec_ಕೂಡಿಸು(now(CLOCK_MONOTONIC), CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        ಅಕ್ಷರ buf[STRLEN("\x1B[1;1R")]; /* The shortest valid reply possible */
        size_t buf_ತುಂಬಿದ = 0;
        CursorPositionContext context = {};

        ಗಾಗಿ (bool ಮೊದಲ = true;; ಮೊದಲ = false) {
                ಆದರೆ (buf_ತುಂಬಿದ == 0) {
                        usec_t n = now(CLOCK_MONOTONIC);
                        ಆದರೆ (n >= ಕೊನೆ)
                                ಹಿಂತಿರುಗಿ -EOPNOTSUPP;

                        r = fd_wait_for_event(nonblock_ಒಳಹರಿವು_fd, POLLIN, usec_sub_unsigned(ಕೊನೆ, n));
                        ಆದರೆ (r < 0)
                                ಹಿಂತಿರುಗಿ r;
                        ಆದರೆ (r == 0)
                                ಹಿಂತಿರುಗಿ -EOPNOTSUPP;

                        /* On the first try, read multiple characters, i.e. the shortest valid
                         * reply. Afterwards read byte-wise, since we don't want to read too much, and
                         * unnecessarily drop too many characters from the input queue. */
                        ssize_t l = ಪಠ(nonblock_ಒಳಹರಿವು_fd, buf, ಮೊದಲ ? ಗಾತ್ರ(buf) : 1);
                        ಆದರೆ (l < 0) {
                                ಆದರೆ (errno == EAGAIN)
                                        ಮುಂದುವರಿ;

                                ಹಿಂತಿರುಗಿ -errno;
                        }

                        ಪ್ರತಿಜ್ಞಾ((size_t) l <= ಗಾತ್ರ(buf));
                        buf_ತುಂಬಿದ = l;
                }

                size_t processed;
                r = ಪರಿಶೀಲನೆ_cursor_position_response(&context, buf, buf_ತುಂಬಿದ, &processed);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;

                ಪ್ರತಿಜ್ಞಾ(processed <= buf_ತುಂಬಿದ);
                buf_ತುಂಬಿದ -= processed;
                memmove(buf, buf + processed, buf_ತುಂಬಿದ);

                ಆದರೆ (r > 0) {
                        /* Superficial validity check */
                        ಆದರೆ (context.ಅಡ್ಡಸಾಲು >= 32766 || context.ಸ್ತಂಭ >= 32766)
                                ಹಿಂತಿರುಗಿ -ENODATA;

                        ಆದರೆ (ret_ಅಡ್ಡಸಾಲು)
                                *ret_ಅಡ್ಡಸಾಲು = context.ಅಡ್ಡಸಾಲು;
                        ಆದರೆ (ret_ಸ್ತಂಭ)
                                *ret_ಸ್ತಂಭ = context.ಸ್ತಂಭ;

                        ಹಿಂತಿರುಗಿ 0;
                }
        }
}

ಪೂರ್ಣಾಂಕ terminal_reset_defensive(ಪೂರ್ಣಾಂಕ fd, TerminalResetFlags flags) {
        ಪೂರ್ಣಾಂಕ r = 0;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);
        ಪ್ರತಿಜ್ಞಾ(!FLAGS_ಗಣ(flags, TERMINAL_RESET_AVOID_ANSI_SEQ|TERMINAL_RESET_FORCE_ANSI_SEQ));

        /* Resets the terminal comprehensively, i.e. via both ioctl()s and via ANSI sequences, but do so only
         * if $TERM is unset or set to "dumb" */

        ಆದರೆ (!isatty_ಸುರಕ್ಷಿತ(fd))
                ಹಿಂತಿರುಗಿ -ENOTTY;

        RET_GATHER(r, terminal_reset_ioctl(fd, FLAGS_ಗಣ(flags, TERMINAL_RESET_SWITCH_ಗಮ್ಯ_ಪಠ್ಯ)));

        ಆದರೆ (!FLAGS_ಗಣ(flags, TERMINAL_RESET_AVOID_ANSI_SEQ) &&
            (FLAGS_ಗಣ(flags, TERMINAL_RESET_FORCE_ANSI_SEQ) || !getenv_terminal_is_dumb()))
                RET_GATHER(r, terminal_reset_ansi_seq(fd));

        ಹಿಂತಿರುಗಿ r;
}

ಪೂರ್ಣಾಂಕ terminal_reset_defensive_locked(ಪೂರ್ಣಾಂಕ fd, TerminalResetFlags flags) {
        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        _cleanup_close_ ಪೂರ್ಣಾಂಕ ಬೀಗ_fd = ಬೀಗ_dev_console();
        ಆದರೆ (ಬೀಗ_fd < 0)
                log_debug_errno(ಬೀಗ_fd, "Failed to acquire lock for /dev/console, ignoring: %m");

        ಹಿಂತಿರುಗಿ terminal_reset_defensive(fd, flags);
}

ಶೂನ್ಯ termios_disable_echo(ರಚನೆ termios *termios) {
        ಪ್ರತಿಜ್ಞಾ(termios);

        termios->c_lflag &= ~(ICANON|ECHO);
        termios->c_cc[VMIN] = 1;
        termios->c_cc[VTIME] = 0;
}

ಸ್ಥಿರ bool termios_is_null(ಸ್ಥಿರಾಂಕ ರಚನೆ termios *t) {
        ಆದರೆ (!t)
                ಹಿಂತಿರುಗಿ true;

        ಹಿಂತಿರುಗಿ t->c_iflag == UINT_MAX &&
               t->c_oflag == UINT_MAX &&
               t->c_cflag == UINT_MAX &&
               t->c_lflag == UINT_MAX;
}

ಶೂನ್ಯ termios_reset(ಸ್ಥಿರಾಂಕ TermiosResetContext *c) {
        ಪ್ರತಿಜ್ಞಾ(c);

        PROTECT_ERRNO;

        ಆದರೆ (c->fd && *c->fd >= 0 && !termios_is_null(c->termios))
                (ಶೂನ್ಯ) tcsetattr(*c->fd, TCSANOW, c->termios);
}

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ BackgroundColorState {
        BACKGROUND_ಪಠ್ಯ,
        BACKGROUND_ESCAPE,
        BACKGROUND_BRACKET,
        BACKGROUND_ಮೊದಲ_ONE,
        BACKGROUND_ಎರಡನೇ_ONE,
        BACKGROUND_SEMICOLON,
        BACKGROUND_R,
        BACKGROUND_G,
        BACKGROUND_B,
        BACKGROUND_RED,
        BACKGROUND_GREEN,
        BACKGROUND_BLUE,
        BACKGROUND_STRING_TERMINATOR,
} BackgroundColorState;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ BackgroundColorContext {
        BackgroundColorState state;
        uint32_t red, green, blue;
        ಚಿಹ್ನೆರಹಿತ red_ಬಿಟ್ಟುಗಳು, green_ಬಿಟ್ಟುಗಳು, blue_ಬಿಟ್ಟುಗಳು;
} BackgroundColorContext;

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಪರಿಶೀಲನೆ_background_color_response(
                BackgroundColorContext *context,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *buf,
                size_t ಗಾತ್ರ_2,
                size_t *ret_processed) {

        ಪ್ರತಿಜ್ಞಾ(context);
        ಪ್ರತಿಜ್ಞಾ(buf);
        ಪ್ರತಿಜ್ಞಾ(ret_processed);

        ಗಾಗಿ (size_t i = 0; i < ಗಾತ್ರ_2; i++) {
                ಅಕ್ಷರ c = buf[i];

                ಆಯ್ಕೆ (context->state) {

                ಸ್ಥಿತಿ BACKGROUND_ಪಠ್ಯ:
                        context->state = c == '\x1B' ? BACKGROUND_ESCAPE : BACKGROUND_ಪಠ್ಯ;
                        ನಿಲ್ಲಿಸು;

                ಸ್ಥಿತಿ BACKGROUND_ESCAPE:
                        context->state = c == ']' ? BACKGROUND_BRACKET : BACKGROUND_ಪಠ್ಯ;
                        ನಿಲ್ಲಿಸು;

                ಸ್ಥಿತಿ BACKGROUND_BRACKET:
                        context->state = c == '1' ? BACKGROUND_ಮೊದಲ_ONE : BACKGROUND_ಪಠ್ಯ;
                        ನಿಲ್ಲಿಸು;

                ಸ್ಥಿತಿ BACKGROUND_ಮೊದಲ_ONE:
                        context->state = c == '1' ? BACKGROUND_ಎರಡನೇ_ONE : BACKGROUND_ಪಠ್ಯ;
                        ನಿಲ್ಲಿಸು;

                ಸ್ಥಿತಿ BACKGROUND_ಎರಡನೇ_ONE:
                        context->state = c == ';' ? BACKGROUND_SEMICOLON : BACKGROUND_ಪಠ್ಯ;
                        ನಿಲ್ಲಿಸು;

                ಸ್ಥಿತಿ BACKGROUND_SEMICOLON:
                        context->state = c == 'r' ? BACKGROUND_R : BACKGROUND_ಪಠ್ಯ;
                        ನಿಲ್ಲಿಸು;

                ಸ್ಥಿತಿ BACKGROUND_R:
                        context->state = c == 'g' ? BACKGROUND_G : BACKGROUND_ಪಠ್ಯ;
                        ನಿಲ್ಲಿಸು;

                ಸ್ಥಿತಿ BACKGROUND_G:
                        context->state = c == 'b' ? BACKGROUND_B : BACKGROUND_ಪಠ್ಯ;
                        ನಿಲ್ಲಿಸು;

                ಸ್ಥಿತಿ BACKGROUND_B:
                        context->state = c == ':' ? BACKGROUND_RED : BACKGROUND_ಪಠ್ಯ;
                        ನಿಲ್ಲಿಸು;

                ಸ್ಥಿತಿ BACKGROUND_RED:
                        ಆದರೆ (c == '/')
                                context->state = context->red_ಬಿಟ್ಟುಗಳು > 0 ? BACKGROUND_GREEN : BACKGROUND_ಪಠ್ಯ;
                        ಇಲ್ಲದಿದ್ದರೆ {
                                ಪೂರ್ಣಾಂಕ d = unhexchar(c);
                                ಆದರೆ (d < 0 || context->red_ಬಿಟ್ಟುಗಳು >= ಗಾತ್ರ(context->red)*8)
                                        context->state = BACKGROUND_ಪಠ್ಯ;
                                ಇಲ್ಲದಿದ್ದರೆ {
                                        context->red = (context->red << 4) | d;
                                        context->red_ಬಿಟ್ಟುಗಳು += 4;
                                }
                        }
                        ನಿಲ್ಲಿಸು;

                ಸ್ಥಿತಿ BACKGROUND_GREEN:
                        ಆದರೆ (c == '/')
                                context->state = context->green_ಬಿಟ್ಟುಗಳು > 0 ? BACKGROUND_BLUE : BACKGROUND_ಪಠ್ಯ;
                        ಇಲ್ಲದಿದ್ದರೆ {
                                ಪೂರ್ಣಾಂಕ d = unhexchar(c);
                                ಆದರೆ (d < 0 || context->green_ಬಿಟ್ಟುಗಳು >= ಗಾತ್ರ(context->green)*8)
                                        context->state = BACKGROUND_ಪಠ್ಯ;
                                ಇಲ್ಲದಿದ್ದರೆ {
                                        context->green = (context->green << 4) | d;
                                        context->green_ಬಿಟ್ಟುಗಳು += 4;
                                }
                        }
                        ನಿಲ್ಲಿಸು;

                ಸ್ಥಿತಿ BACKGROUND_BLUE:
                        ಆದರೆ (c == '\x07') {
                                ಆದರೆ (context->blue_ಬಿಟ್ಟುಗಳು > 0) {
                                        *ret_processed = i + 1;
                                        ಹಿಂತಿರುಗಿ 1; /* success! */
                                }

                                context->state = BACKGROUND_ಪಠ್ಯ;
                        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (c == '\x1b')
                                context->state = context->blue_ಬಿಟ್ಟುಗಳು > 0 ? BACKGROUND_STRING_TERMINATOR : BACKGROUND_ಪಠ್ಯ;
                        ಇಲ್ಲದಿದ್ದರೆ {
                                ಪೂರ್ಣಾಂಕ d = unhexchar(c);
                                ಆದರೆ (d < 0 || context->blue_ಬಿಟ್ಟುಗಳು >= ಗಾತ್ರ(context->blue)*8)
                                        context->state = BACKGROUND_ಪಠ್ಯ;
                                ಇಲ್ಲದಿದ್ದರೆ {
                                        context->blue = (context->blue << 4) | d;
                                        context->blue_ಬಿಟ್ಟುಗಳು += 4;
                                }
                        }
                        ನಿಲ್ಲಿಸು;

                ಸ್ಥಿತಿ BACKGROUND_STRING_TERMINATOR:
                        ಆದರೆ (c == '\\') {
                                *ret_processed = i + 1;
                                ಹಿಂತಿರುಗಿ 1; /* success! */
                        }

                        context->state = c == ']' ? BACKGROUND_ESCAPE : BACKGROUND_ಪಠ್ಯ;
                        ನಿಲ್ಲಿಸು;

                }

                /* Reset any colors we might have picked up */
                ಆದರೆ (ಒಳ_ಗಣ(context->state, BACKGROUND_ಪಠ್ಯ, BACKGROUND_ESCAPE)) {
                        /* reset color */
                        context->red = context->green = context->blue = 0;
                        context->red_ಬಿಟ್ಟುಗಳು = context->green_ಬಿಟ್ಟುಗಳು = context->blue_ಬಿಟ್ಟುಗಳು = 0;
                }
        }

        *ret_processed = ಗಾತ್ರ_2;
        ಹಿಂತಿರುಗಿ 0; /* all good, but not enough data yet */
}

ಪೂರ್ಣಾಂಕ get_default_background_color(ದ್ವಿಗುಣ *ret_red, ದ್ವಿಗುಣ *ret_green, ದ್ವಿಗುಣ *ret_blue) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret_red);
        ಪ್ರತಿಜ್ಞಾ(ret_green);
        ಪ್ರತಿಜ್ಞಾ(ret_blue);

        ಆದರೆ (!colors_enabled())
                ಹಿಂತಿರುಗಿ -EOPNOTSUPP;

        r = terminal_verify_same(STDIN_FILENO, STDOUT_FILENO);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (streq_ptr(getenv("TERM"), "linux")) {
                /* Linux console is black */
                *ret_red = *ret_green = *ret_blue = 0.0;
                ಹಿಂತಿರುಗಿ 0;
        }

        /* Open a 2nd input fd, in non-blocking mode, so that we won't ever hang in read()
         * should someone else process the POLLIN. Do all subsequent operations on the new fd. */
        _cleanup_close_ ಪೂರ್ಣಾಂಕ nonblock_ಒಳಹರಿವು_fd = r = fd_reopen(STDIN_FILENO, O_RDONLY|O_CLOEXEC|O_NONBLOCK|O_NOCTTY);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ರಚನೆ termios old_termios = TERMIOS_NULL;
        CLEANUP_TERMIOS_RESET(nonblock_ಒಳಹರಿವು_fd, old_termios);

        ಆದರೆ (tcgetattr(nonblock_ಒಳಹರಿವು_fd, &old_termios) < 0)
                ಹಿಂತಿರುಗಿ -errno;

        ರಚನೆ termios new_termios = old_termios;
        termios_disable_echo(&new_termios);

        ಆದರೆ (tcsetattr(nonblock_ಒಳಹರಿವು_fd, TCSANOW, &new_termios) < 0)
                ಹಿಂತಿರುಗಿ -errno;

        r = loop_write(STDOUT_FILENO, ANSI_OSC "11;?" ANSI_ST, SIZE_MAX);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        usec_t ಕೊನೆ = usec_ಕೂಡಿಸು(now(CLOCK_MONOTONIC), CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        ಅಕ್ಷರ buf[STRLEN(ANSI_OSC "11;rgb:0/0/0" ANSI_ST)]; /* shortest possible reply */
        size_t buf_ತುಂಬಿದ = 0;
        BackgroundColorContext context = {};

        ಗಾಗಿ (bool ಮೊದಲ = true;; ಮೊದಲ = false) {
                ಆದರೆ (buf_ತುಂಬಿದ == 0) {
                        usec_t n = now(CLOCK_MONOTONIC);
                        ಆದರೆ (n >= ಕೊನೆ)
                                ಹಿಂತಿರುಗಿ -EOPNOTSUPP;

                        r = fd_wait_for_event(nonblock_ಒಳಹರಿವು_fd, POLLIN, usec_sub_unsigned(ಕೊನೆ, n));
                        ಆದರೆ (r < 0)
                                ಹಿಂತಿರುಗಿ r;
                        ಆದರೆ (r == 0)
                                ಹಿಂತಿರುಗಿ -EOPNOTSUPP;

                        /* On the first try, read multiple characters, i.e. the shortest valid
                         * reply. Afterwards read byte-wise, since we don't want to read too much, and
                         * unnecessarily drop too many characters from the input queue. */
                        ssize_t l = ಪಠ(nonblock_ಒಳಹರಿವು_fd, buf, ಮೊದಲ ? ಗಾತ್ರ(buf) : 1);
                        ಆದರೆ (l < 0) {
                                ಆದರೆ (errno == EAGAIN)
                                        ಮುಂದುವರಿ;
                                ಹಿಂತಿರುಗಿ -errno;
                        }

                        ಪ್ರತಿಜ್ಞಾ((size_t) l <= ಗಾತ್ರ(buf));
                        buf_ತುಂಬಿದ = l;
                }

                size_t processed;
                r = ಪರಿಶೀಲನೆ_background_color_response(&context, buf, buf_ತುಂಬಿದ, &processed);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;

                ಪ್ರತಿಜ್ಞಾ(processed <= buf_ತುಂಬಿದ);
                buf_ತುಂಬಿದ -= processed;
                memmove(buf, buf + processed, buf_ತುಂಬಿದ);

                ಆದರೆ (r > 0) {
                        ಪ್ರತಿಜ್ಞಾ(context.red_ಬಿಟ್ಟುಗಳು > 0);
                        *ret_red = (ದ್ವಿಗುಣ) context.red / ((UINT64_C(1) << context.red_ಬಿಟ್ಟುಗಳು) - 1);
                        ಪ್ರತಿಜ್ಞಾ(context.green_ಬಿಟ್ಟುಗಳು > 0);
                        *ret_green = (ದ್ವಿಗುಣ) context.green / ((UINT64_C(1) << context.green_ಬಿಟ್ಟುಗಳು) - 1);
                        ಪ್ರತಿಜ್ಞಾ(context.blue_ಬಿಟ್ಟುಗಳು > 0);
                        *ret_blue = (ದ್ವಿಗುಣ) context.blue / ((UINT64_C(1) << context.blue_ಬಿಟ್ಟುಗಳು) - 1);
                        ಹಿಂತಿರುಗಿ 0;
                }
        }
}

/* Determine terminal dimensions by means of ANSI sequences: save the cursor via DECSC, position it far
 * to the bottom right (clamped to actual terminal dimensions), read back via DSR where we ended up, and
 * restore cursor via DECRC. Only needs a single DSR round-trip, and always restores the cursor regardless
 * of whether the response is received.
 *
 * Caller must have already opened a non-blocking input fd and configured termios (echo/icanon off). */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ terminal_query_ಗಾತ್ರ_by_dsr(
                ಪೂರ್ಣಾಂಕ nonblock_ಒಳಹರಿವು_fd,
                ಪೂರ್ಣಾಂಕ output_fd,
                ಚಿಹ್ನೆರಹಿತ *ret_ಅಡ್ಡಸಾಲುಗಳು,
                ಚಿಹ್ನೆರಹಿತ *ret_columns) {

        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(nonblock_ಒಳಹರಿವು_fd >= 0);
        ಪ್ರತಿಜ್ಞಾ(output_fd >= 0);

        /* Use DECSC/DECRC to save/restore cursor instead of querying position via DSR. This way the cursor
         * is always restored — even on timeout — and we only need one DSR response instead of two. */
        r = loop_write_ತುಂಬಿದ(output_fd,
                            "\x1B" "7"              /* DECSC: save cursor position */
                            "\x1B[32766;32766H"     /* CUP: position cursor far to the right and to the bottom, staying within 16bit signed range */
                            "\x1B[6n"               /* DSR: request cursor position (CPR) */
                            "\x1B" "8",             /* DECRC: restore cursor position */
                            SIZE_MAX,
                            CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        usec_t ಕೊನೆ = usec_ಕೂಡಿಸು(now(CLOCK_MONOTONIC), CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        ಅಕ್ಷರ buf[STRLEN("\x1B[1;1R")]; /* The shortest valid reply possible */
        size_t buf_ತುಂಬಿದ = 0;
        CursorPositionContext context = {};

        ಗಾಗಿ (bool ಮೊದಲ = true;; ಮೊದಲ = false) {
                ಆದರೆ (buf_ತುಂಬಿದ == 0) {
                        usec_t n = now(CLOCK_MONOTONIC);
                        ಆದರೆ (n >= ಕೊನೆ)
                                ಹಿಂತಿರುಗಿ -EOPNOTSUPP;

                        r = fd_wait_for_event(nonblock_ಒಳಹರಿವು_fd, POLLIN, usec_sub_unsigned(ಕೊನೆ, n));
                        ಆದರೆ (r < 0)
                                ಹಿಂತಿರುಗಿ r;
                        ಆದರೆ (r == 0)
                                ಹಿಂತಿರುಗಿ -EOPNOTSUPP;

                        /* On the first try, read multiple characters, i.e. the shortest valid
                         * reply. Afterwards read byte-wise, since we don't want to read too much, and
                         * unnecessarily drop too many characters from the input queue. */
                        ssize_t l = ಪಠ(nonblock_ಒಳಹರಿವು_fd, buf, ಮೊದಲ ? ಗಾತ್ರ(buf) : 1);
                        ಆದರೆ (l < 0) {
                                ಆದರೆ (errno == EAGAIN)
                                        ಮುಂದುವರಿ;

                                ಹಿಂತಿರುಗಿ -errno;
                        }

                        ಪ್ರತಿಜ್ಞಾ((size_t) l <= ಗಾತ್ರ(buf));
                        buf_ತುಂಬಿದ = l;
                }

                size_t processed;
                r = ಪರಿಶೀಲನೆ_cursor_position_response(&context, buf, buf_ತುಂಬಿದ, &processed);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;

                ಪ್ರತಿಜ್ಞಾ(processed <= buf_ತುಂಬಿದ);
                buf_ತುಂಬಿದ -= processed;
                memmove(buf, buf + processed, buf_ತುಂಬಿದ);

                ಆದರೆ (r > 0) {
                        /* Superficial validity checks (no particular reason to check for < 4, it's
                         * just a way to look for unreasonably small values) */
                        ಆದರೆ (context.ಅಡ್ಡಸಾಲು < 4 || context.ಸ್ತಂಭ < 4 || context.ಅಡ್ಡಸಾಲು >= 32766 || context.ಸ್ತಂಭ >= 32766)
                                ಹಿಂತಿರುಗಿ -ENODATA;

                        ಆದರೆ (ret_ಅಡ್ಡಸಾಲುಗಳು)
                                *ret_ಅಡ್ಡಸಾಲುಗಳು = context.ಅಡ್ಡಸಾಲು;
                        ಆದರೆ (ret_columns)
                                *ret_columns = context.ಸ್ತಂಭ;

                        ಹಿಂತಿರುಗಿ 0;
                }
        }
}

/* Common setup for ANSI terminal queries: validate the fds, open a non-blocking input fd, and configure
 * termios with echo and canonical mode disabled. Caller must restore termios and close the fd when done. */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ terminal_prepare_query(
                ಪೂರ್ಣಾಂಕ ಒಳಹರಿವು_fd,
                ಪೂರ್ಣಾಂಕ output_fd,
                ಪೂರ್ಣಾಂಕ *ret_nonblock_fd,
                ರಚನೆ termios *ret_saved_termios) {

        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ಒಳಹರಿವು_fd >= 0);
        ಪ್ರತಿಜ್ಞಾ(output_fd >= 0);
        ಪ್ರತಿಜ್ಞಾ(ret_nonblock_fd);
        ಪ್ರತಿಜ್ಞಾ(ret_saved_termios);

        /* Use getenv_terminal_is_dumb() instead of terminal_is_dumb() here since we operate on an
         * explicitly passed fd, not on stdio. terminal_is_dumb() additionally checks on_tty() which
         * tests whether *stderr* is a tty — that's irrelevant when we're querying a directly opened
         * terminal such as /dev/console. */
        ಆದರೆ (getenv_terminal_is_dumb())
                ಹಿಂತಿರುಗಿ -EOPNOTSUPP;

        r = terminal_verify_same(ಒಳಹರಿವು_fd, output_fd);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ log_debug_errno(r, "Called with distinct input/output fds: %m");

        /* Open a 2nd input fd, in non-blocking mode, so that we won't ever hang in read()
         * should someone else process the POLLIN. Do all subsequent operations on the new fd. */
        _cleanup_close_ ಪೂರ್ಣಾಂಕ nonblock_ಒಳಹರಿವು_fd = r = fd_reopen(ಒಳಹರಿವು_fd, O_RDONLY|O_CLOEXEC|O_NONBLOCK|O_NOCTTY);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (tcgetattr(nonblock_ಒಳಹರಿವು_fd, ret_saved_termios) < 0)
                ಹಿಂತಿರುಗಿ log_debug_errno(errno, "Failed to get terminal settings: %m");

        ರಚನೆ termios new_termios = *ret_saved_termios;
        termios_disable_echo(&new_termios);

        ಆದರೆ (tcsetattr(nonblock_ಒಳಹರಿವು_fd, TCSANOW, &new_termios) < 0)
                ಹಿಂತಿರುಗಿ log_debug_errno(errno, "Failed to set new terminal settings: %m");

        *ret_nonblock_fd = TAKE_FD(nonblock_ಒಳಹರಿವು_fd);
        ಹಿಂತಿರುಗಿ 0;
}

/*
 * See https://terminalguide.namepad.de/seq/csi_st-18/,
 * https://invisible-island.net/xterm/ctlseqs/ctlseqs.html#h3-Functions-using-CSI-_-ordered-by-the-final-character_s_.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು CSI18_Q  "\x1B[18t"               /* Report the size of the text area in characters */
#ವ್ಯಾಖ್ಯಾನಿಸು CSI18_Rp "\x1B[8;"                /* Reply prefix */
#ವ್ಯಾಖ್ಯಾನಿಸು CSI18_R0 CSI18_Rp "1;1t"          /* Shortest reply */
#ವ್ಯಾಖ್ಯಾನಿಸು CSI18_R1 CSI18_Rp "32766;32766t"  /* Longest reply */

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಪರಿಶೀಲನೆ_ಪಠ್ಯ_area_ಗಾತ್ರ_response(
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *buf,
                size_t ಗಾತ್ರ_2,
                ಚಿಹ್ನೆರಹಿತ *ret_ಅಡ್ಡಸಾಲುಗಳು,
                ಚಿಹ್ನೆರಹಿತ *ret_columns) {

        ಪ್ರತಿಜ್ಞಾ(buf);
        ಪ್ರತಿಜ್ಞಾ(ret_ಅಡ್ಡಸಾಲುಗಳು);
        ಪ್ರತಿಜ್ಞಾ(ret_columns);

        /* Check if we have enough space for the shortest possible answer. */
        ಆದರೆ (ಗಾತ್ರ_2 < STRLEN(CSI18_R0))
                ಹಿಂತಿರುಗಿ -EAGAIN;

        /* Check if the terminating sequence is present */
        ಆದರೆ (buf[ಗಾತ್ರ_2 - 1] != 't')
                ಹಿಂತಿರುಗಿ -EAGAIN;

        ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ ಅಡ್ಡಸಾಲುಗಳು, columns;
        ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಓದು(buf, CSI18_Rp "%hu;%hut", &ಅಡ್ಡಸಾಲುಗಳು, &columns) != 2)
                ಹಿಂತಿರುಗಿ -EINVAL;

        *ret_ಅಡ್ಡಸಾಲುಗಳು = ಅಡ್ಡಸಾಲುಗಳು;
        *ret_columns = columns;
        ಹಿಂತಿರುಗಿ 0;
}

/* Determine terminal dimensions by means of an ANSI CSI 18 sequence.
 *
 * Caller must have already opened a non-blocking input fd and configured termios (echo/icanon off). */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ terminal_query_ಗಾತ್ರ_by_csi18(
                ಪೂರ್ಣಾಂಕ nonblock_ಒಳಹರಿವು_fd,
                ಪೂರ್ಣಾಂಕ output_fd,
                ಚಿಹ್ನೆರಹಿತ *ret_ಅಡ್ಡಸಾಲುಗಳು,
                ಚಿಹ್ನೆರಹಿತ *ret_columns) {

        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(nonblock_ಒಳಹರಿವು_fd >= 0);
        ಪ್ರತಿಜ್ಞಾ(output_fd >= 0);

        r = loop_write_ತುಂಬಿದ(output_fd, CSI18_Q, SIZE_MAX, CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        usec_t ಕೊನೆ = usec_ಕೂಡಿಸು(now(CLOCK_MONOTONIC), CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        ಅಕ್ಷರ buf[STRLEN(CSI18_R1)];
        size_t ಬೈಟುಗಳು = 0;

        ಗಾಗಿ (;;) {
                usec_t n = now(CLOCK_MONOTONIC);
                ಆದರೆ (n >= ಕೊನೆ)
                        ಹಿಂತಿರುಗಿ -EOPNOTSUPP;

                r = fd_wait_for_event(nonblock_ಒಳಹರಿವು_fd, POLLIN, usec_sub_unsigned(ಕೊನೆ, n));
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
                ಆದರೆ (r == 0)
                        ಹಿಂತಿರುಗಿ -EOPNOTSUPP;

                /* On the first read, read multiple characters, i.e. the shortest valid reply. Afterwards
                 * read byte by byte, since we don't want to read too much and drop characters from the input
                 * queue. */
                ssize_t l = ಪಠ(nonblock_ಒಳಹರಿವು_fd, buf + ಬೈಟುಗಳು, ಬೈಟುಗಳು == 0 ? STRLEN(CSI18_R0) : 1);
                ಆದರೆ (l < 0) {
                        ಆದರೆ (errno == EAGAIN)
                                ಮುಂದುವರಿ;
                        ಹಿಂತಿರುಗಿ -errno;
                }

                ಪ್ರತಿಜ್ಞಾ((size_t) l <= ಗಾತ್ರ(buf) - ಬೈಟುಗಳು);
                ಬೈಟುಗಳು += l;

                r = ಪರಿಶೀಲನೆ_ಪಠ್ಯ_area_ಗಾತ್ರ_response(buf, ಬೈಟುಗಳು, ret_ಅಡ್ಡಸಾಲುಗಳು, ret_columns);
                ಆದರೆ (r != -EAGAIN)
                        ಹಿಂತಿರುಗಿ r;

                ಆದರೆ (ಬೈಟುಗಳು == ಗಾತ್ರ(buf))
                        ಹಿಂತಿರುಗಿ -EOPNOTSUPP; /* The response has the right prefix, but we didn't find a valid
                                             * answer with a terminator in the allotted space. Something is
                                             * wrong, possibly some unrelated bytes got injected into the
                                             * answer. */
        }
}

ಪೂರ್ಣಾಂಕ terminal_get_ಗಾತ್ರ(
                ಪೂರ್ಣಾಂಕ ಒಳಹರಿವು_fd,
                ಪೂರ್ಣಾಂಕ output_fd,
                ಚಿಹ್ನೆರಹಿತ *ret_ಅಡ್ಡಸಾಲುಗಳು,
                ಚಿಹ್ನೆರಹಿತ *ret_columns,
                bool try_dsr,
                bool try_csi18) {

        _cleanup_close_ ಪೂರ್ಣಾಂಕ nonblock_ಒಳಹರಿವು_fd = -EBADF;
        ರಚನೆ termios old_termios = TERMIOS_NULL;
        CLEANUP_TERMIOS_RESET(nonblock_ಒಳಹರಿವು_fd, old_termios);
        _cleanup_(nonblock_resetp) ಪೂರ್ಣಾಂಕ nonblock_reset = -EBADF;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(try_dsr || try_csi18);

        r = terminal_prepare_query(ಒಳಹರಿವು_fd, output_fd, &nonblock_ಒಳಹರಿವು_fd, &old_termios);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        /* Put the output fd in non-blocking mode with a write timeout, to avoid blocking indefinitely on
         * write if the terminal is not consuming data (e.g. serial console with flow control). */
        r = fd_nonblock(output_fd, true);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ log_debug_errno(r, "Failed to set terminal to non-blocking mode: %m");
        ಆದರೆ (r > 0)
                nonblock_reset = output_fd;

        /* Flush any stale input that might confuse the response parsers. */
        (ಶೂನ್ಯ) tcflush(nonblock_ಒಳಹರಿವು_fd, TCIFLUSH);

        ಆದರೆ (try_csi18) {
                r = terminal_query_ಗಾತ್ರ_by_csi18(nonblock_ಒಳಹರಿವು_fd, output_fd, ret_ಅಡ್ಡಸಾಲುಗಳು, ret_columns);
                ಆದರೆ (r >= 0)
                        ಹಿಂತಿರುಗಿ r;

                /* Query failed. Flush any outstanding input. */
                (ಶೂನ್ಯ) tcflush(nonblock_ಒಳಹರಿವು_fd, TCIFLUSH);

                ಆದರೆ (!ಒಳ_ಗಣ(r, -EOPNOTSUPP, -EINVAL))
                        ಹಿಂತಿರುಗಿ r;
        }

        ಆದರೆ (try_dsr) {
                r = terminal_query_ಗಾತ್ರ_by_dsr(nonblock_ಒಳಹರಿವು_fd, output_fd, ret_ಅಡ್ಡಸಾಲುಗಳು, ret_columns);
                ಆದರೆ (r >= 0)
                        ಹಿಂತಿರುಗಿ r;

                /* Query failed. Flush any outstanding input. */
                (ಶೂನ್ಯ) tcflush(nonblock_ಒಳಹರಿವು_fd, TCIFLUSH);
        }

        ಹಿಂತಿರುಗಿ r;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ fd_is_vc(ಪೂರ್ಣಾಂಕ fd) {
        _cleanup_free_ ಅಕ್ಷರ *name = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        r = getttyname_harder(fd, &name);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ tty_is_vc_resolve(name);
}

ಪೂರ್ಣಾಂಕ terminal_fix_ಗಾತ್ರ(ಪೂರ್ಣಾಂಕ ಒಳಹರಿವು_fd, ಪೂರ್ಣಾಂಕ output_fd) {
        ಚಿಹ್ನೆರಹಿತ ಅಡ್ಡಸಾಲುಗಳು, columns;
        ಪೂರ್ಣಾಂಕ r;

        /* Don't ask a virtual console at all. There is no intermediary here that could be wrong about the
         * dimensions: the kernel renders the screen itself, so its winsize is authoritative. */
        r = fd_is_vc(ಒಳಹರಿವು_fd);
        ಆದರೆ (r < 0)
                log_debug_errno(r, "Failed to determine whether terminal is a virtual console, assuming it is not: %m");
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (r > 0) {
                log_debug("Not querying dimensions of a virtual console, kernel dimensions are authoritative.");
                ಹಿಂತಿರುಗಿ 0;
        }

        ರಚನೆ winsize ws = {};
        ಆದರೆ (ioctl(output_fd, TIOCGWINSZ, &ws) < 0)
                ಹಿಂತಿರುಗಿ log_debug_errno(errno, "Failed to query terminal dimensions, ignoring: %m");

        r = terminal_get_ಗಾತ್ರ(ಒಳಹರಿವು_fd, output_fd, &ಅಡ್ಡಸಾಲುಗಳು, &columns, /* try_dsr= */ true, /* try_csi18= */ true);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ log_debug_errno(r, "Failed to acquire terminal dimensions via ANSI sequences, not adjusting terminal dimensions: %m");

        ಆದರೆ (ws.ws_row == ಅಡ್ಡಸಾಲುಗಳು && ws.ws_col == columns) {
                log_debug("Terminal dimensions reported via ANSI sequences match currently set terminal dimensions, not changing.");
                ಹಿಂತಿರುಗಿ 0;
        }

        ws.ws_col = columns;
        ws.ws_row = ಅಡ್ಡಸಾಲುಗಳು;

        ಆದರೆ (ioctl(output_fd, TIOCSWINSZ, &ws) < 0)
                ಹಿಂತಿರುಗಿ log_debug_errno(errno, "Failed to update terminal dimensions, ignoring: %m");

        log_debug("Fixed terminal dimensions to %ux%u based on ANSI sequence information.", columns, ಅಡ್ಡಸಾಲುಗಳು);
        ಹಿಂತಿರುಗಿ 1;
}

#ವ್ಯಾಖ್ಯಾನಿಸು MAX_TERMINFO_ಉದ್ದಳತೆ 64
/* python -c 'print("".join(hex(ord(i))[2:] for i in "name").upper())' */
#ವ್ಯಾಖ್ಯಾನಿಸು DCS_TERMINFO_Q ANSI_DCS "+q" "6E616D65" ANSI_ST
/* The answer is either 0+r… (invalid) or 1+r… (OK). */
#ವ್ಯಾಖ್ಯಾನಿಸು DCS_TERMINFO_R0 ANSI_DCS "0+r" ANSI_ST
#ವ್ಯಾಖ್ಯಾನಿಸು DCS_TERMINFO_R1 ANSI_DCS "1+r" "6E616D65" "=" /* This is followed by Pt ST. */
assert_cc(STRLEN(DCS_TERMINFO_R0) <= STRLEN(DCS_TERMINFO_R1 ANSI_ST));

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಪರಿಶೀಲನೆ_terminfo_response(
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *buf,
                size_t ಗಾತ್ರ_2,
                ಅಕ್ಷರ **ret_name) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(buf);
        ಪ್ರತಿಜ್ಞಾ(ret_name);

        /* Check if we have enough space for the shortest possible answer. */
        ಆದರೆ (ಗಾತ್ರ_2 < STRLEN(DCS_TERMINFO_R0))
                ಹಿಂತಿರುಗಿ -EAGAIN;

        /* Check if the terminating sequence is present */
        ಆದರೆ (memcmp(buf + ಗಾತ್ರ_2 - STRLEN(ANSI_ST), ANSI_ST, STRLEN(ANSI_ST)) != 0)
                ಹಿಂತಿರುಗಿ -EAGAIN;

        ಆದರೆ (ಗಾತ್ರ_2 <= STRLEN(DCS_TERMINFO_R1 ANSI_ST))
                ಹಿಂತಿರುಗಿ -EINVAL;  /* The answer is invalid or empty */

        ಆದರೆ (memcmp(buf, DCS_TERMINFO_R1, STRLEN(DCS_TERMINFO_R1)) != 0)
                ಹಿಂತಿರುಗಿ -EINVAL;  /* The answer is not valid */

        _cleanup_free_ ಶೂನ್ಯ *dec = ಖಾಲಿ;
        size_t dec_ಗಾತ್ರ;
        r = unhexmem_ತುಂಬಿದ(buf + STRLEN(DCS_TERMINFO_R1), ಗಾತ್ರ_2 - STRLEN(DCS_TERMINFO_R1 ANSI_ST),
                          /* secure= */ false,
                          &dec, &dec_ಗಾತ್ರ);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಪ್ರತಿಜ್ಞಾ(((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *) dec)[dec_ಗಾತ್ರ] == '\0'); /* unhexmem appends NUL for our convenience */
        ಆದರೆ (memchr(dec, '\0', dec_ಗಾತ್ರ) || string_has_cc(dec, ಖಾಲಿ) || !filename_is_valid(dec))
                ಹಿಂತಿರುಗಿ -EUCLEAN;

        *ret_name = TAKE_PTR(dec);
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ terminal_get_terminfo_by_dcs(ಪೂರ್ಣಾಂಕ fd, ಅಕ್ಷರ **ret_name) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);
        ಪ್ರತಿಜ್ಞಾ(ret_name);

        /* Note: fd must be in non-blocking read-write mode! */

        ರಚನೆ termios old_termios = TERMIOS_NULL;
        CLEANUP_TERMIOS_RESET(fd, old_termios);

        ಆದರೆ (tcgetattr(fd, &old_termios) < 0)
                ಹಿಂತಿರುಗಿ -errno;

        ರಚನೆ termios new_termios = old_termios;
        termios_disable_echo(&new_termios);

        ಆದರೆ (tcsetattr(fd, TCSANOW, &new_termios) < 0)
                ಹಿಂತಿರುಗಿ -errno;

        r = loop_write(fd, DCS_TERMINFO_Q, SIZE_MAX);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        usec_t ಕೊನೆ = usec_ಕೂಡಿಸು(now(CLOCK_MONOTONIC), CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        ಅಕ್ಷರ buf[STRLEN(DCS_TERMINFO_R1) + MAX_TERMINFO_ಉದ್ದಳತೆ + STRLEN(ANSI_ST)];
        size_t ಬೈಟುಗಳು = 0;

        ಗಾಗಿ (;;) {
                usec_t n = now(CLOCK_MONOTONIC);
                ಆದರೆ (n >= ಕೊನೆ)
                        ಹಿಂತಿರುಗಿ -EOPNOTSUPP;

                r = fd_wait_for_event(fd, POLLIN, usec_sub_unsigned(ಕೊನೆ, n));
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
                ಆದರೆ (r == 0)
                        ಹಿಂತಿರುಗಿ -EOPNOTSUPP;

                /* On the first read, read multiple characters, i.e. the shortest valid reply. Afterwards
                 * read byte by byte, since we don't want to read too much and drop characters from the input
                 * queue. */
                ssize_t l = ಪಠ(fd, buf + ಬೈಟುಗಳು, ಬೈಟುಗಳು == 0 ? STRLEN(DCS_TERMINFO_R0) : 1);
                ಆದರೆ (l < 0) {
                        ಆದರೆ (errno == EAGAIN)
                                ಮುಂದುವರಿ;
                        ಹಿಂತಿರುಗಿ -errno;
                }

                ಪ್ರತಿಜ್ಞಾ((size_t) l <= ಗಾತ್ರ(buf) - ಬೈಟುಗಳು);
                ಬೈಟುಗಳು += l;

                r = ಪರಿಶೀಲನೆ_terminfo_response(buf, ಬೈಟುಗಳು, ret_name);
                ಆದರೆ (r != -EAGAIN)
                        ಹಿಂತಿರುಗಿ r;

                ಆದರೆ (ಬೈಟುಗಳು == ಗಾತ್ರ(buf))
                        ಹಿಂತಿರುಗಿ -EOPNOTSUPP; /* The response has the right prefix, but we didn't find a valid
                                             * answer with a terminator in the allotted space. Something is
                                             * wrong, possibly some unrelated bytes got injected into the
                                             * answer. */
        }
}

ಪೂರ್ಣಾಂಕ have_terminfo_file(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name) {
        /* This is a heuristic check if we have the file, using the directory layout used on
         * current Linux systems. Checks for other layouts can be added later if appropriate. */
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(filename_is_valid(name));

        _cleanup_free_ ಅಕ್ಷರ *p = ಮಾರ್ಗ_join("/usr/share/terminfo", CHAR_ಗಮ್ಯ_STR(name[0]), name);
        ಆದರೆ (!p)
                ಹಿಂತಿರುಗಿ log_oom_debug();

        r = RET_NERRNO(access(p, F_OK));
        ಆದರೆ (r == -ENOENT)
                ಹಿಂತಿರುಗಿ false;
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;
        ಹಿಂತಿರುಗಿ true;
}

ಪೂರ್ಣಾಂಕ query_ಪದ_for_tty(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *tty, ಅಕ್ಷರ **ret_ಪದ) {
        _cleanup_free_ ಅಕ್ಷರ *dcs_ಪದ = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(tty);
        ಪ್ರತಿಜ್ಞಾ(ret_ಪದ);

        ಆದರೆ (tty_is_vc_resolve(tty))
                ಹಿಂತಿರುಗಿ strdup_ಗಮ್ಯ(ret_ಪದ, "linux");

        /* Try to query the terminal implementation that we're on. This will not work in all
         * cases, which is fine, since this is intended to be used as a fallback. */

        _cleanup_close_ ಪೂರ್ಣಾಂಕ tty_fd = open_terminal(tty, O_RDWR|O_NOCTTY|O_CLOEXEC|O_NONBLOCK);
        ಆದರೆ (tty_fd < 0)
                ಹಿಂತಿರುಗಿ log_debug_errno(tty_fd, "Failed to open %s to query terminfo: %m", tty);

        r = terminal_get_terminfo_by_dcs(tty_fd, &dcs_ಪದ);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ log_debug_errno(r, "Failed to query %s for terminfo: %m", tty);

        r = have_terminfo_file(dcs_ಪದ);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ log_debug_errno(r, "Failed to look for terminfo %s: %m", dcs_ಪದ);
        ಆದರೆ (r == 0)
                ಹಿಂತಿರುಗಿ log_info_errno(SYNTHETIC_ERRNO(ENODATA),
                                      "Terminfo %s not found for %s.", dcs_ಪದ, tty);

        *ret_ಪದ = TAKE_PTR(dcs_ಪದ);
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ terminal_is_pty_fd(ಪೂರ್ಣಾಂಕ fd) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        /* Returns true if we are looking at a pty, i.e. if it's backed by the /dev/pts/ file system */

        ಆದರೆ (!isatty_ಸುರಕ್ಷಿತ(fd))
                ಹಿಂತಿರುಗಿ false;

        r = is_fs_type_at(fd, ಖಾಲಿ, DEVPTS_SUPER_MAGIC);
        ಆದರೆ (r != 0)
                ಹಿಂತಿರುಗಿ r;

        /* The ptmx device is weird, it exists twice, once inside and once outside devpts. To detect the
         * latter case, let's fire off an ioctl() that only works on ptmx devices. */

        ಪೂರ್ಣಾಂಕ v;
        ಆದರೆ (ioctl(fd, TIOCGPKT, &v) < 0) {
                ಆದರೆ (ERRNO_IS_NOT_SUPPORTED(errno))
                        ಹಿಂತಿರುಗಿ false;

                ಹಿಂತಿರುಗಿ -errno;
        }

        ಹಿಂತಿರುಗಿ true;
}

ಪೂರ್ಣಾಂಕ pty_open_peer(ಪೂರ್ಣಾಂಕ fd, ಪೂರ್ಣಾಂಕ ಬಹುಲಕ) {
        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        /* Opens the peer PTY using the new race-free TIOCGPTPEER ioctl() (kernel 4.13).
         *
         * This is safe to be called on TTYs from other namespaces. */

        ಪ್ರತಿಜ್ಞಾ((ಬಹುಲಕ & (O_CREAT|O_ಮಾರ್ಗ|O_DIRECTORY|O_TMPFILE)) == 0);

        /* This replicates the EIO retry logic of open_terminal() in a modified way. */
        ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ c = 0;; c++) {
                ಪೂರ್ಣಾಂಕ peer_fd = ioctl(fd, TIOCGPTPEER, ಬಹುಲಕ);
                ಆದರೆ (peer_fd >= 0)
                        ಹಿಂತಿರುಗಿ peer_fd;

                ಆದರೆ (errno != EIO)
                        ಹಿಂತಿರುಗಿ -errno;

                /* Max 1s in total */
                ಆದರೆ (c >= 20)
                        ಹಿಂತಿರುಗಿ -EIO;

                (ಶೂನ್ಯ) usleep_ಸುರಕ್ಷಿತ(50 * USEC_ಪ್ರತಿ_MSEC);
        }
}
