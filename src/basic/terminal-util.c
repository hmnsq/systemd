/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ <fcntl.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <linux/kd.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <linux/magic.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <linux/tiocl.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <linux/vt.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <poll.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <signal.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdlib.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/inotify.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/ioctl.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/sysmacros.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <termios.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <time.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <unistd.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "alloc-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ansi-color.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "chase.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "devnum-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "errno-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "extract-word.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fd-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fileio.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fs-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hexdecoct.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "inotify-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "io-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "log.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "namespace-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "parse-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "path-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "pidref.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "proc-cmdline.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "process-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "signal-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "socket-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "stat-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "stdio-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "strv.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "terminal-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "time-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "utf8.h"

/* How much to wait when reading/writing ANSI sequences from/to the console */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC (333 * USEC_ਪ੍ਰਤੀ_MSEC)

ਸਥਿਰ ਅਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ cached_columns = 0;
ਸਥਿਰ ਅਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ cached_ਸਤਰਾਂ = 0;

ਸਥਿਰ ਅਸਥਿਰ ਪੂਰਨ cached_on_tty = -1;
ਸਥਿਰ ਅਸਥਿਰ ਪੂਰਨ cached_on_dev_null = -1;

bool isatty_ਸੁਰੱਖਿਅਤ(ਪੂਰਨ fd) {
        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);

        ਜੇ (isatty(fd))
                ਵਾਪਸ true;

        /* Linux/glibc returns EIO for hung up TTY on isatty(). Which is wrong, the thing doesn't stop being
         * a TTY after all, just because it is temporarily hung up. Let's work around this here, until this
         * is fixed in glibc. See: https://sourceware.org/bugzilla/show_bug.cgi?id=32103 */
        ਜੇ (errno == EIO)
                ਵਾਪਸ true;

        /* Be resilient if we're working on stdio, since they're set up by parent process. */
        ਪ੍ਰਤਿਜ੍ਞਾ(errno != EBADF || ਵਿੱਚ_ਸੈੱਟ(fd, STDIN_FILENO, STDOUT_FILENO, STDERR_FILENO));

        ਵਾਪਸ false;
}

ਪੂਰਨ chvt(ਪੂਰਨ vt) {
        _cleanup_close_ ਪੂਰਨ fd = -EBADF;

        /* Switch to the specified vt number. If the VT is specified <= 0 switch to the VT the kernel log messages go,
         * if that's configured. */

        fd = open_terminal("/dev/tty0", O_RDWR|O_NOCTTY|O_CLOEXEC|O_NONBLOCK);
        ਜੇ (fd < 0)
                ਵਾਪਸ fd;

        ਜੇ (vt <= 0) {
                ਪੂਰਨ tiocl[2] = {
                        TIOCL_GETKMSGREDIRECT,
                        0
                };

                ਜੇ (ioctl(fd, TIOCLINUX, tiocl) < 0)
                        ਵਾਪਸ -errno;

                vt = tiocl[0] <= 0 ? 1 : tiocl[0];
        }

        ਵਾਪਸ RET_NERRNO(ioctl(fd, VT_ACTIVATE, vt));
}

ਪੂਰਨ read_one_char(FILE *f, ਅੱਖਰ *ret, usec_t t, bool echo, bool *need_nl) {
        _cleanup_free_ ਅੱਖਰ *ਸਤਰ = ਨਿਰਾ;
        ਬਣਤਰ termios old_termios;
        ਪੂਰਨ r, fd;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        ਜੇ (!f)
                f = stdin;

        /* If this is a terminal, then switch canonical mode off, so that we can read a single
         * character. (Note that fmemopen() streams do not have an fd associated with them, let's handle that
         * nicely.) If 'echo' is false we'll also disable ECHO mode so that the pressed key is not made
         * visible to the user. */
        fd = fileno(f);
        ਜੇ (fd >= 0 && tcgetattr(fd, &old_termios) >= 0) {
                ਬਣਤਰ termios new_termios = old_termios;

                new_termios.c_lflag &= ~(ICANON|(echo ? 0 : ECHO));
                new_termios.c_cc[VMIN] = 1;
                new_termios.c_cc[VTIME] = 0;

                ਜੇ (tcsetattr(fd, TCSANOW, &new_termios) >= 0) {
                        ਅੱਖਰ c;

                        ਜੇ (t != USEC_INFINITY) {
                                ਜੇ (fd_wait_for_event(fd, POLLIN, t) <= 0) {
                                        (ਖਾਲੀ) tcsetattr(fd, TCSANOW, &old_termios);
                                        ਵਾਪਸ -ETIMEDOUT;
                                }
                        }

                        r = ਸੁਰੱਖਿਅਤ_fgetc(f, &c);
                        (ਖਾਲੀ) tcsetattr(fd, TCSANOW, &old_termios);
                        ਜੇ (r < 0)
                                ਵਾਪਸ r;
                        ਜੇ (r == 0)
                                ਵਾਪਸ -EIO;

                        ਜੇ (need_nl)
                                *need_nl = c != '\n';

                        *ret = c;
                        ਵਾਪਸ 0;
                }
        }

        ਜੇ (t != USEC_INFINITY && fd >= 0) {
                /* Let's wait the specified amount of time for input. When we have no fd we skip this, under
                 * the assumption that this is an fmemopen() stream or so where waiting doesn't make sense
                 * anyway, as the data is either already in the stream or cannot possible be placed there
                 * while we access the stream */

                ਜੇ (fd_wait_for_event(fd, POLLIN, t) <= 0)
                        ਵਾਪਸ -ETIMEDOUT;
        }

        /* If this is not a terminal, then read a full line instead */

        r = read_ਸਤਰ(f, 16, &ਸਤਰ); /* longer than necessary, to eat up UTF-8 chars/vt100 key sequences */
        ਜੇ (r < 0)
                ਵਾਪਸ r;
        ਜੇ (r == 0)
                ਵਾਪਸ -EIO;

        ਜੇ (ਲੜੀ_ਲੰਬਾਈ(ਸਤਰ) != 1)
                ਵਾਪਸ -EBADMSG;

        ਜੇ (need_nl)
                *need_nl = false;

        *ret = ਸਤਰ[0];
        ਵਾਪਸ 0;
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFAULT_ASK_REFRESH_USEC (2*USEC_ਪ੍ਰਤੀ_SEC)

ਪੂਰਨ ask_char(ਅੱਖਰ *ret, ਅਚਲ ਅੱਖਰ *replies, ਅਚਲ ਅੱਖਰ *fmt, ...) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret);
        ਪ੍ਰਤਿਜ੍ਞਾ(replies);
        ਪ੍ਰਤਿਜ੍ਞਾ(fmt);

        ਲਈ (;;) {
                va_list ap;
                ਅੱਖਰ c;
                bool need_nl = true;

                fputs(ansi_highlight(), stdout);

                ਅੱਖਰ_ਲਿਖੋ('\r');

                va_start(ap, fmt);
                vprintf(fmt, ap);
                va_end(ap);

                fputs(ansi_ਸਾਧਾਰਨ(), stdout);

                fflush(stdout);

                r = read_one_char(stdin, &c, DEFAULT_ASK_REFRESH_USEC, /* echo= */ true, &need_nl);
                ਜੇ (r < 0) {

                        ਜੇ (r == -ETIMEDOUT)
                                ਜਾਰੀ;

                        ਜੇ (r == -EBADMSG) {
                                puts("Bad input, please try again.");
                                ਜਾਰੀ;
                        }

                        ਅੱਖਰ_ਲਿਖੋ('\n');
                        ਵਾਪਸ r;
                }

                ਜੇ (need_nl)
                        ਅੱਖਰ_ਲਿਖੋ('\n');

                ਜੇ (strchr(replies, c)) {
                        *ret = c;
                        ਵਾਪਸ 0;
                }

                puts("Read unexpected character, please try again.");
        }
}

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ Completion_ਨਤੀਜਾ{
        COMPLETION_ALREADY,       /* the input string is already complete */
        COMPLETION_ਭਰਿਆ,          /* completed the input string to be complete now */
        COMPLETION_PARTIAL,       /* completed the input string so that is still incomplete */
        COMPLETION_NONE,          /* found no matching completion */
        COMPLETION_ਨਤੀਜਾ_MAX,
        COMPLETION_ਨਤੀਜਾ_INVALID = -EINVAL,
        COMPLETION_ਨਤੀਜਾ_ERRNO_MAX = -ERRNO_MAX,
} Completion_ਨਤੀਜਾ;

ਸਥਿਰ Completion_ਨਤੀਜਾ pick_completion(ਅਚਲ ਅੱਖਰ *string, ਅੱਖਰ *ਅਚਲ*completions, ਅੱਖਰ **ret) {
        _cleanup_free_ ਅੱਖਰ *ਮਿਲਿਆ = ਨਿਰਾ;
        bool partial = false;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        string = strempty(string);

        STRV_FOREACH(c, completions) {

                /* Ignore entries that are not actually completions */
                ਜੇ (!startswith(*c, string))
                        ਜਾਰੀ;

                /* Store first completion that matches */
                ਜੇ (!ਮਿਲਿਆ) {
                        ਮਿਲਿਆ = strdup(*c);
                        ਜੇ (!ਮਿਲਿਆ)
                                ਵਾਪਸ -ENOMEM;

                        ਜਾਰੀ;
                }

                /* If there's another completion that works truncate the one we already found by common
                 * prefix */
                size_t n = str_common_ਅਗੇਤਰ(ਮਿਲਿਆ, *c);
                ਜੇ (n == SIZE_MAX)
                        ਜਾਰੀ;

                ਮਿਲਿਆ[n] = 0;
                partial = true;
        }

        *ret = TAKE_PTR(ਮਿਲਿਆ);

        ਜੇ (!*ret)
                ਵਾਪਸ COMPLETION_NONE;
        ਜੇ (partial)
                ਵਾਪਸ COMPLETION_PARTIAL;

        ਵਾਪਸ streq(string, *ret) ? COMPLETION_ALREADY : COMPLETION_ਭਰਿਆ;
}

ਸਥਿਰ ਖਾਲੀ clear_by_backspace(size_t n) {
        /* Erase the specified number of character cells backwards on the terminal */
        ਲਈ (size_t i = 0; i < n; i++)
                fputs("\b \b", stdout);
}

ਪੂਰਨ ask_string_ਭਰਿਆ(
                ਅੱਖਰ **ret,
                ਅਚਲ ਅੱਖਰ *prefill,
                GetCompletionsCallback get_completions,
                ਖਾਲੀ *userdata,
                ਅਚਲ ਅੱਖਰ *ਪਾਠ, ...) {

        va_list ap;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਪਾਠ);

        _cleanup_free_ ਅੱਖਰ *string = ਨਿਰਾ;
        size_t n = 0;

        ਜੇ (prefill) {
                /* Prefill query with explicit data if specified */

                string = strdup(prefill);
                ਜੇ (!string)
                        ਵਾਪਸ -ENOMEM;

                n = ਲੜੀ_ਲੰਬਾਈ(string);

        } ਨਹੀਂਤਾਂ ਜੇ (get_completions) {
                /* Otherwise, figure out what string to preselect the query with */
                _cleanup_strv_free_ ਅੱਖਰ **completions = ਨਿਰਾ;
                r = get_completions("", GET_COMPLETIONS_PRESELECT, &completions, userdata);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;

                Completion_ਨਤੀਜਾ cr = pick_completion(string, completions, &string);
                ਜੇ (cr < 0)
                        ਵਾਪਸ cr;

                n = strlen_ptr(string);
        }

        /* Output the prompt */
        fputs(ansi_highlight(), stdout);
        va_start(ap, ਪਾਠ);
        vprintf(ਪਾਠ, ap);
        va_end(ap);
        fputs(ansi_ਸਾਧਾਰਨ(), stdout);
        ਜੇ (string)
                fputs(string, stdout);
        fflush(stdout);

        /* Do interactive logic only if stdin + stdout are connected to the same place. And yes, we could use
         * STDIN_FILENO and STDOUT_FILENO here, but let's be overly correct for once, after all libc allows
         * swapping out stdin/stdout. */
        ਪੂਰਨ fd_ਨਿਵੇਸ਼ = fileno(stdin);
        ਪੂਰਨ fd_output = fileno(stdout);
        ਬਣਤਰ termios old_termios = TERMIOS_NULL;
        CLEANUP_TERMIOS_RESET(fd_ਨਿਵੇਸ਼, old_termios);

        ਜੇ (fd_ਨਿਵੇਸ਼ < 0 || fd_output < 0 || same_fd(fd_ਨਿਵੇਸ਼, fd_output) <= 0)
                ਜਾਓ fallback;

        /* Try to disable echo, which also tells us if this even is a terminal */
        ਜੇ (tcgetattr(fd_ਨਿਵੇਸ਼, &old_termios) < 0) {
                old_termios = TERMIOS_NULL;
                ਜਾਓ fallback;
        }

        ਬਣਤਰ termios new_termios = old_termios;
        termios_disable_echo(&new_termios);
        ਜੇ (tcsetattr(fd_ਨਿਵੇਸ਼, TCSANOW, &new_termios) < 0)
                ਵਾਪਸ -errno;

        ਲਈ (;;) {
                ਪੂਰਨ c = fgetc(stdin);

                /* On EOF or NUL, end the request, don't output anything anymore */
                ਜੇ (ਵਿੱਚ_ਸੈੱਟ(c, EOF, 0))
                        ਰੋਕੋ;

                /* On Return also end the request, but make this visible */
                ਜੇ (ਵਿੱਚ_ਸੈੱਟ(c, '\n', '\r')) {
                        fputc('\n', stdout);
                        ਰੋਕੋ;
                }

                ਜੇ (c == '\t') {
                        /* Tab */

                        _cleanup_strv_free_ ਅੱਖਰ **completions = ਨਿਰਾ;
                        ਜੇ (get_completions) {
                                r = get_completions(string, /* flags= */ 0, &completions, userdata);
                                ਜੇ (r < 0)
                                        ਵਾਪਸ r;
                        }

                        _cleanup_free_ ਅੱਖਰ *new_string = ਨਿਰਾ;
                        Completion_ਨਤੀਜਾ cr = pick_completion(string, completions, &new_string);
                        ਜੇ (cr < 0)
                                ਵਾਪਸ cr;
                        ਜੇ (ਵਿੱਚ_ਸੈੱਟ(cr, COMPLETION_PARTIAL, COMPLETION_ਭਰਿਆ)) {
                                /* Output the new suffix we learned */
                                fputs(ASSERT_PTR(startswith(new_string, strempty(string))), stdout);

                                /* And update the whole string */
                                free_and_replace(string, new_string);
                                n = ਲੜੀ_ਲੰਬਾਈ(string);
                        }
                        ਜੇ (cr == COMPLETION_NONE)
                                fputc('\a', stdout); /* BEL */

                        ਜੇ (ਵਿੱਚ_ਸੈੱਟ(cr, COMPLETION_PARTIAL, COMPLETION_ALREADY)) {
                                /* If this worked only partially, or if the user hit TAB even though we were
                                 * complete already, then show the remaining options (in the latter case just
                                 * the one). */
                                fputc('\n', stdout);

                                _cleanup_strv_free_ ਅੱਖਰ **filtered = strv_filter_ਅਗੇਤਰ(completions, string);
                                ਜੇ (!filtered)
                                        ਵਾਪਸ -ENOMEM;

                                r = show_menu(filtered,
                                              /* n_columns= */ SIZE_MAX,
                                              /* column_width= */ SIZE_MAX,
                                              /* ellipsize_percentage= */ 0,
                                              /* grey_prefix= */ string,
                                              /* with_numbers= */ false);
                                ਜੇ (r < 0)
                                        ਵਾਪਸ r;

                                /* Show the prompt again */
                                fputs(ansi_highlight(), stdout);
                                va_start(ap, ਪਾਠ);
                                vprintf(ਪਾਠ, ap);
                                va_end(ap);
                                fputs(ansi_ਸਾਧਾਰਨ(), stdout);
                                fputs(string, stdout);
                        }

                } ਨਹੀਂਤਾਂ ਜੇ (ਵਿੱਚ_ਸੈੱਟ(c, '\b', 127)) {
                        /* Backspace */

                        ਜੇ (n == 0)
                                fputc('\a', stdout); /* BEL */
                        ਨਹੀਂਤਾਂ {
                                size_t m = utf8_ਆਖਰੀ_ਲੰਮਾਈ(string, n);

                                ਅੱਖਰ *e = string + n - m;
                                clear_by_backspace(utf8_console_ਚੌੜਾਈ(e));

                                *e = 0;
                                n -= m;
                        }

                } ਨਹੀਂਤਾਂ ਜੇ (c == 21) {
                        /* Ctrl-u → erase all input */

                        clear_by_backspace(utf8_console_ਚੌੜਾਈ(string));
                        ਜੇ (string)
                                string[n = 0] = 0;
                        ਨਹੀਂਤਾਂ
                                ਪ੍ਰਤਿਜ੍ਞਾ(n == 0);

                } ਨਹੀਂਤਾਂ ਜੇ (c == 4) {
                        /* Ctrl-d → cancel this field input */

                        ਵਾਪਸ -ECANCELED;

                } ਨਹੀਂਤਾਂ ਜੇ (char_is_cc(c) || n >= LINE_MAX)
                        /* refuse control characters and too long strings */
                        fputc('\a', stdout); /* BEL */
                ਨਹੀਂਤਾਂ {
                        /* Regular char */

                        ਜੇ (!GREEDY_REALLOC(string, n+2))
                                ਵਾਪਸ -ENOMEM;

                        string[n++] = (ਅੱਖਰ) c;
                        string[n] = 0;

                        fputc(c, stdout);
                }

                fflush(stdout);
        }

        ਜੇ (!string) {
                string = strdup("");
                ਜੇ (!string)
                        ਵਾਪਸ -ENOMEM;
        }

        *ret = TAKE_PTR(string);
        ਵਾਪਸ 0;

fallback:
        /* A simple fallback without TTY magic */
        string = mfree(string);
        r = read_ਸਤਰ(stdin, LONG_ਸਤਰ_MAX, &string);
        ਜੇ (r < 0)
                ਵਾਪਸ r;
        ਜੇ (r == 0)
                ਵਾਪਸ -EIO;

        *ret = TAKE_PTR(string);
        ਵਾਪਸ 0;
}

bool any_ਕੁੰਜੀ_ਮੰਜ਼ਿਲ_proceed(ਖਾਲੀ) {

        /* Insert a new line here as well as to when the user inputs, as this is also used during the boot up
         * sequence when status messages may be interleaved with the current program output. This ensures
         * that the status messages aren't appended on the same line as this message. */

        fputc('\n', stdout);
        fputs(ansi_highlight_magenta(), stdout);
        fputs("-- Press any key to proceed --", stdout);
        fputs(ansi_ਸਾਧਾਰਨ(), stdout);
        fputc('\n', stdout);
        fflush(stdout);

        ਅੱਖਰ ਕੁੰਜੀ = 0;
        (ਖਾਲੀ) read_one_char(stdin, &ਕੁੰਜੀ, USEC_INFINITY, /* echo= */ false, /* need_nl= */ ਨਿਰਾ);

        fputc('\n', stdout);
        fflush(stdout);

        ਵਾਪਸ ਕੁੰਜੀ != 'q';
}

ਸਥਿਰ size_t widest_list_element(ਅੱਖਰ *ਅਚਲ*l) {
        size_t w = 0;

        /* Returns the largest console width of all elements in 'l' */

        STRV_FOREACH(i, l)
                w = MAX(w, utf8_console_ਚੌੜਾਈ(*i));

        ਵਾਪਸ w;
}

ਪੂਰਨ show_menu(ਅੱਖਰ **x,
              size_t n_columns,
              size_t ਕਾਲਮ_ਚੌੜਾਈ,
              ਬਿਨਾਨਿਸ਼ਾਨੀ ellipsize_percentage,
              ਅਚਲ ਅੱਖਰ *grey_ਅਗੇਤਰ,
              bool with_numbers) {

        ਪ੍ਰਤਿਜ੍ਞਾ(n_columns > 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਕਾਲਮ_ਚੌੜਾਈ > 0);

        ਜੇ (n_columns == SIZE_MAX)
                n_columns = 3;

        ਜੇ (ਕਾਲਮ_ਚੌੜਾਈ == SIZE_MAX) {
                size_t widest = widest_list_element(x);

                /* If not specified, derive column width from screen width */
                size_t ਕਾਲਮ_max = (columns()-1) / n_columns;

                /* Subtract room for numbers */
                ਜੇ (with_numbers)
                        ਕਾਲਮ_max = LESS_BY(ਕਾਲਮ_max, 6U);

                /* If columns would get too tight let's make this a linear list instead. */
                ਜੇ (ਕਾਲਮ_max < 10 && widest > 10) {
                        n_columns = 1;
                        ਕਾਲਮ_max = columns()-1;

                        ਜੇ (with_numbers)
                                ਕਾਲਮ_max = LESS_BY(ਕਾਲਮ_max, 6U);
                }

                /* Never make this narrower than 10 characters */
                ਕਾਲਮ_max = MAX(ਕਾਲਮ_max, 10U);
                ਕਾਲਮ_ਚੌੜਾਈ = CLAMP(widest+1, 10U, ਕਾਲਮ_max);
        }

        size_t n = strv_ਲੰਮਾਈ(x);
        size_t ਪ੍ਰਤੀ_ਕਾਲਮ = DIV_ਗੇੜਾ_ਉੱਪਰ(n, n_columns);

        size_t break_ਸਤਰਾਂ = ਸਤਰਾਂ();
        ਜੇ (break_ਸਤਰਾਂ > 2)
                break_ਸਤਰਾਂ--;

        /* The first page gets two extra lines, since we want to show a title */
        size_t break_modulo = break_ਸਤਰਾਂ;
        ਜੇ (break_modulo > 3)
                break_modulo -= 3;

        ਲਈ (size_t i = 0; i < ਪ੍ਰਤੀ_ਕਾਲਮ; i++) {

                ਲਈ (size_t j = 0; j < n_columns; j++) {
                        _cleanup_free_ ਅੱਖਰ *e = ਨਿਰਾ;

                        size_t p = j * ਪ੍ਰਤੀ_ਕਾਲਮ + i;
                        ਜੇ (p >= n)
                                ਰੋਕੋ;

                        e = ellipsize(x[p], ਕਾਲਮ_ਚੌੜਾਈ, ellipsize_percentage);
                        ਜੇ (!e)
                                ਵਾਪਸ -ENOMEM;

                        ਜੇ (with_numbers)
                                ਲਿਖੋ("%s%4zu)%s ",
                                       ansi_grey(),
                                       p + 1,
                                       ansi_ਸਾਧਾਰਨ());

                        ਜੇ (grey_ਅਗੇਤਰ && startswith(e, grey_ਅਗੇਤਰ)) {
                                size_t k = MIN(ਲੜੀ_ਲੰਬਾਈ(grey_ਅਗੇਤਰ), ਕਾਲਮ_ਚੌੜਾਈ);
                                ਲਿਖੋ("%s%.*s%s",
                                       ansi_grey(),
                                       (ਪੂਰਨ) k, e,
                                       ansi_ਸਾਧਾਰਨ());
                                ਲਿਖੋ("%-*s",
                                       (ਪੂਰਨ) (ਕਾਲਮ_ਚੌੜਾਈ - k), e+k);
                        } ਨਹੀਂਤਾਂ
                                ਲਿਖੋ("%-*s", (ਪੂਰਨ) ਕਾਲਮ_ਚੌੜਾਈ, e);
                }

                ਅੱਖਰ_ਲਿਖੋ('\n');

                /* on the first screen we reserve 2 extra lines for the title */
                ਜੇ (i % break_ਸਤਰਾਂ == break_modulo)
                        ਜੇ (!any_ਕੁੰਜੀ_ਮੰਜ਼ਿਲ_proceed())
                                ਵਾਪਸ 0;
        }

        ਵਾਪਸ 0;
}

ਪੂਰਨ open_terminal(ਅਚਲ ਅੱਖਰ *name, ਪੂਰਨ ਬਹੁਲਕ) {
        _cleanup_close_ ਪੂਰਨ fd = -EBADF;

        /*
         * If a TTY is in the process of being closed opening it might cause EIO. This is horribly awful, but
         * unlikely to be changed in the kernel. Hence we work around this problem by retrying a couple of
         * times.
         *
         * https://bugs.launchpad.net/ubuntu/+source/linux/+bug/554172/comments/245
         */

        ਪ੍ਰਤਿਜ੍ਞਾ((ਬਹੁਲਕ & (O_CREAT|O_ਰਸਤਾ|O_DIRECTORY|O_TMPFILE)) == 0);

        ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ c = 0;; c++) {
                fd = open(name, ਬਹੁਲਕ, 0);
                ਜੇ (fd >= 0)
                        ਰੋਕੋ;

                ਜੇ (errno != EIO)
                        ਵਾਪਸ -errno;

                /* Max 1s in total */
                ਜੇ (c >= 20)
                        ਵਾਪਸ -EIO;

                (ਖਾਲੀ) usleep_ਸੁਰੱਖਿਅਤ(50 * USEC_ਪ੍ਰਤੀ_MSEC);
        }

        ਜੇ (!isatty_ਸੁਰੱਖਿਅਤ(fd))
                ਵਾਪਸ -ENOTTY;

        ਵਾਪਸ TAKE_FD(fd);
}

ਪੂਰਨ acquire_terminal(
                ਅਚਲ ਅੱਖਰ *name,
                AcquireTerminalFlags flags,
                usec_t timeout) {

        _cleanup_close_ ਪੂਰਨ notify = -EBADF, fd = -EBADF;
        usec_t ts = USEC_INFINITY;
        ਪੂਰਨ r, wd = -1;

        ਪ੍ਰਤਿਜ੍ਞਾ(name);

        AcquireTerminalFlags ਬਹੁਲਕ = flags & ACQUIRE_TERMINAL_ਬਹੁਲਕ_ਨਕਾਬ;
        ਪ੍ਰਤਿਜ੍ਞਾ(ਵਿੱਚ_ਸੈੱਟ(ਬਹੁਲਕ, ACQUIRE_TERMINAL_TRY, ACQUIRE_TERMINAL_FORCE, ACQUIRE_TERMINAL_WAIT));
        ਪ੍ਰਤਿਜ੍ਞਾ(ਬਹੁਲਕ == ACQUIRE_TERMINAL_WAIT || !FLAGS_ਸੈੱਟ(flags, ACQUIRE_TERMINAL_WATCH_SIGTERM));

        /* We use inotify to be notified when the tty is closed. We create the watch before checking if we can actually
         * acquire it, so that we don't lose any event.
         *
         * Note: strictly speaking this actually watches for the device being closed, it does *not* really watch
         * whether a tty loses its controlling process. However, unless some rogue process uses TIOCNOTTY on /dev/tty
         * *after* closing its tty otherwise this will not become a problem. As long as the administrator makes sure to
         * not configure any service on the same tty as an untrusted user this should not be a problem. (Which they
         * probably should not do anyway.) */

        ਜੇ (ਬਹੁਲਕ == ACQUIRE_TERMINAL_WAIT) {
                notify = inotify_init1(IN_CLOEXEC | IN_NONBLOCK);
                ਜੇ (notify < 0)
                        ਵਾਪਸ -errno;

                wd = inotify_add_watch(notify, name, IN_CLOSE);
                ਜੇ (wd < 0)
                        ਵਾਪਸ -errno;

                ਜੇ (timeout != USEC_INFINITY)
                        ts = now(CLOCK_MONOTONIC);
        }

        /* If we are called with ACQUIRE_TERMINAL_WATCH_SIGTERM we'll unblock SIGTERM during ppoll() temporarily */
        sigset_t poll_ss;
        assert_se(sigprocmask(SIG_SETMASK, /* newset= */ ਨਿਰਾ, &poll_ss) >= 0);
        ਜੇ (flags & ACQUIRE_TERMINAL_WATCH_SIGTERM) {
                assert_se(sigismember(&poll_ss, SIGTERM) > 0);
                assert_se(sigdelset(&poll_ss, SIGTERM) >= 0);
        }

        ਲਈ (;;) {
                ਜੇ (notify >= 0) {
                        r = flush_fd(notify);
                        ਜੇ (r < 0)
                                ਵਾਪਸ r;
                }

                /* We pass here O_NOCTTY only so that we can check the return value TIOCSCTTY and have a reliable way
                 * to figure out if we successfully became the controlling process of the tty */
                fd = open_terminal(name, O_RDWR|O_NOCTTY|O_CLOEXEC);
                ਜੇ (fd < 0)
                        ਵਾਪਸ fd;

                /* Temporarily ignore SIGHUP, so that we don't get SIGHUP'ed if we already own the tty. */
                ਬਣਤਰ sigaction sa_old;
                assert_se(sigaction(SIGHUP, &sigaction_ignore, &sa_old) >= 0);

                /* First, try to get the tty */
                r = RET_NERRNO(ioctl(fd, TIOCSCTTY, ਬਹੁਲਕ == ACQUIRE_TERMINAL_FORCE));

                /* Reset signal handler to old value */
                assert_se(sigaction(SIGHUP, &sa_old, ਨਿਰਾ) >= 0);

                /* Success? Exit the loop now! */
                ਜੇ (r >= 0)
                        ਰੋਕੋ;

                /* Any failure besides -EPERM? Fail, regardless of the mode. */
                ਜੇ (r != -EPERM)
                        ਵਾਪਸ r;

                ਜੇ (flags & ACQUIRE_TERMINAL_PERMISSIVE) /* If we are in permissive mode, then EPERM is fine, turn this
                                                          * into a success. Note that EPERM is also returned if we
                                                          * already are the owner of the TTY. */
                        ਰੋਕੋ;

                ਜੇ (ਬਹੁਲਕ != ACQUIRE_TERMINAL_WAIT) /* If we are in TRY or FORCE mode, then propagate EPERM as EPERM */
                        ਵਾਪਸ r;

                ਪ੍ਰਤਿਜ੍ਞਾ(notify >= 0);
                ਪ੍ਰਤਿਜ੍ਞਾ(wd >= 0);

                ਲਈ (;;) {
                        usec_t ਖੱਬਾ;
                        ਜੇ (timeout == USEC_INFINITY)
                                ਖੱਬਾ = USEC_INFINITY;
                        ਨਹੀਂਤਾਂ {
                                ਪ੍ਰਤਿਜ੍ਞਾ(ts != USEC_INFINITY);

                                usec_t n = usec_sub_unsigned(now(CLOCK_MONOTONIC), ts);
                                ਜੇ (n >= timeout)
                                        ਵਾਪਸ -ETIMEDOUT;

                                ਖੱਬਾ = timeout - n;
                        }

                        r = ppoll_usec_ਭਰਿਆ(
                                        &(ਬਣਤਰ pollfd) {
                                                .fd = notify,
                                                .events = POLLIN,
                                        },
                                        /* n_fds= */ 1,
                                        ਖੱਬਾ,
                                        &poll_ss);
                        ਜੇ (r < 0)
                                ਵਾਪਸ r;
                        ਜੇ (r == 0)
                                ਵਾਪਸ -ETIMEDOUT;

                        ਸੰਘ inotify_event_ਬਫਰ ਬਫਰ;
                        ssize_t l;
                        l = ਪਠ(notify, &ਬਫਰ, ਆਕਾਰ(ਬਫਰ));
                        ਜੇ (l < 0) {
                                ਜੇ (ERRNO_IS_TRANSIENT(errno))
                                        ਜਾਰੀ;

                                ਵਾਪਸ -errno;
                        }

                        FOREACH_INOTIFY_EVENT(e, ਬਫਰ, l) {
                                ਜੇ (e->ਨਕਾਬ_3 & IN_Q_OVERFLOW) /* If we hit an inotify queue overflow, simply check if the terminal is up for grabs now. */
                                        ਰੋਕੋ;

                                ਜੇ (e->wd != wd || !(e->ਨਕਾਬ_3 & IN_CLOSE)) /* Safety checks */
                                        ਵਾਪਸ -EIO;
                        }

                        ਰੋਕੋ;
                }

                /* We close the tty fd here since if the old session ended our handle will be dead. It's important that
                 * we do this after sleeping, so that we don't enter an endless loop. */
                fd = ਸੁਰੱਖਿਅਤ_close(fd);
        }

        ਵਾਪਸ TAKE_FD(fd);
}

ਪੂਰਨ release_terminal(ਖਾਲੀ) {
        _cleanup_close_ ਪੂਰਨ fd = -EBADF;
        ਪੂਰਨ r;

        fd = open("/dev/tty", O_RDWR|O_NOCTTY|O_CLOEXEC|O_NONBLOCK);
        ਜੇ (fd < 0)
                ਵਾਪਸ -errno;

        /* Temporarily ignore SIGHUP, so that we don't get SIGHUP'ed
         * by our own TIOCNOTTY */
        ਬਣਤਰ sigaction sa_old;
        assert_se(sigaction(SIGHUP, &sigaction_ignore, &sa_old) >= 0);

        r = RET_NERRNO(ioctl(fd, TIOCNOTTY));

        assert_se(sigaction(SIGHUP, &sa_old, ਨਿਰਾ) >= 0);

        ਵਾਪਸ r;
}

ਪੂਰਨ terminal_new_session(ਖਾਲੀ) {

        /* Make us the new session leader, and set stdin tty to be our controlling terminal.
         *
         * Why stdin? Well, the ctty logic is relevant for signal delivery mostly, i.e. if people hit C-c
         * or the line is hung up. Such events are basically just a form of input, via a side channel
         * (that side channel being signal delivery, i.e. SIGINT, SIGHUP et al). Hence we focus on input,
         * not output here. */

        ਜੇ (!isatty_ਸੁਰੱਖਿਅਤ(STDIN_FILENO))
                ਵਾਪਸ -ENXIO;

        (ਖਾਲੀ) setsid();
        ਵਾਪਸ RET_NERRNO(ioctl(STDIN_FILENO, TIOCSCTTY, 0));
}

ਖਾਲੀ terminal_detach_session(ਖਾਲੀ) {
        (ਖਾਲੀ) setsid();
        (ਖਾਲੀ) release_terminal();
}

ਪੂਰਨ terminal_vhangup_fd(ਪੂਰਨ fd) {
        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);
        ਵਾਪਸ RET_NERRNO(ioctl(fd, TIOCVHANGUP));
}

ਪੂਰਨ terminal_vhangup(ਅਚਲ ਅੱਖਰ *tty) {
        _cleanup_close_ ਪੂਰਨ fd = -EBADF;

        ਪ੍ਰਤਿਜ੍ਞਾ(tty);

        fd = open_terminal(tty, O_RDWR|O_NOCTTY|O_CLOEXEC);
        ਜੇ (fd < 0)
                ਵਾਪਸ fd;

        ਵਾਪਸ terminal_vhangup_fd(fd);
}

ਪੂਰਨ vt_disallocate(ਅਚਲ ਅੱਖਰ *tty_ਰਸਤਾ) {
        ਪ੍ਰਤਿਜ੍ਞਾ(tty_ਰਸਤਾ);

        /* Deallocate the VT if possible. If not possible (i.e. because it is the active one), at least clear
         * it entirely (including the scrollback buffer). */

        ਪੂਰਨ ttynr = vtnr_from_tty(tty_ਰਸਤਾ);
        ਜੇ (ttynr > 0) {
                _cleanup_close_ ਪੂਰਨ fd = open_terminal("/dev/tty0", O_RDWR|O_NOCTTY|O_CLOEXEC|O_NONBLOCK);
                ਜੇ (fd < 0)
                        ਵਾਪਸ fd;

                /* Try to deallocate */
                ਜੇ (ioctl(fd, VT_DISALLOCATE, ttynr) >= 0)
                        ਵਾਪਸ 0;
                ਜੇ (errno != EBUSY)
                        ਵਾਪਸ -errno;
        }

        /* So this is not a VT (in which case we cannot deallocate it), or we failed to deallocate. Let's at
         * least clear the screen. */

        _cleanup_close_ ਪੂਰਨ fd2 = open_terminal(tty_ਰਸਤਾ, O_WRONLY|O_NOCTTY|O_CLOEXEC|O_NONBLOCK);
        ਜੇ (fd2 < 0)
                ਵਾਪਸ fd2;

        ਵਾਪਸ loop_write_ਭਰਿਆ(fd2,
                               "\033[r"   /* clear scrolling region */
                               "\033[H"   /* move home */
                               "\033[3J"  /* clear screen including scrollback, requires Linux 2.6.40 */
                               "\033c",   /* reset to initial state */
                               SIZE_MAX,
                               CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
}

ਸਥਿਰ ਪੂਰਨ vt_default_utf8(ਖਾਲੀ) {
        /* Read the default VT UTF8 setting from the kernel */
        ਵਾਪਸ read_boolean_file("/sys/module/vt/parameters/default_utf8");
}

ਸਥਿਰ ਪੂਰਨ vt_reset_keyboard(ਪੂਰਨ fd) {
        ਪੂਰਨ r, kb;

        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);

        /* If we can't read the default, then default to Unicode. It's 2024 after all. */
        r = vt_default_utf8();
        ਜੇ (r < 0)
                log_debug_errno(r, "Failed to determine kernel VT UTF-8 mode, assuming enabled: %m");

        kb = vt_default_utf8() != 0 ? K_UNICODE : K_XLATE;
        ਵਾਪਸ RET_NERRNO(ioctl(fd, KDSKBMODE, kb));
}

ਸਥਿਰ ਪੂਰਨ terminal_reset_ioctl(ਪੂਰਨ fd, bool switch_ਮੰਜ਼ਿਲ_ਪਾਠ) {
        ਬਣਤਰ termios termios;
        ਪੂਰਨ r;

        /* Set terminal to some sane defaults */

        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);

        /* We leave locked terminal attributes untouched, so that Plymouth may set whatever it wants to set,
         * and we don't interfere with that. */

        /* Disable exclusive mode, just in case */
        ਜੇ (ioctl(fd, TIOCNXCL) < 0)
                log_debug_errno(errno, "TIOCNXCL ioctl failed on TTY, ignoring: %m");

        /* Switch to text mode */
        ਜੇ (switch_ਮੰਜ਼ਿਲ_ਪਾਠ)
                ਜੇ (ioctl(fd, KDSETMODE, KD_TEXT) < 0)
                        log_debug_errno(errno, "KDSETMODE ioctl for switching to text mode failed on TTY, ignoring: %m");

        /* Set default keyboard mode */
        r = vt_reset_keyboard(fd);
        ਜੇ (r < 0)
                log_debug_errno(r, "Failed to reset VT keyboard, ignoring: %m");

        ਜੇ (tcgetattr(fd, &termios) < 0) {
                r = log_debug_errno(errno, "Failed to get terminal parameters: %m");
                ਜਾਓ ਅੰਤ;
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
        ਜੇ (r < 0)
                log_debug_errno(r, "Failed to set terminal parameters: %m");

ਅੰਤ:
        /* Just in case, flush all crap out */
        (ਖਾਲੀ) tcflush(fd, TCIOFLUSH);

        ਵਾਪਸ r;
}

ਪੂਰਨ terminal_reset_ansi_seq(ਪੂਰਨ fd) {
        _cleanup_(nonblock_resetp) ਪੂਰਨ nonblock_reset = -EBADF;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);

        ਜੇ (getenv_terminal_is_dumb())
                ਵਾਪਸ 0;

        r = fd_nonblock(fd, true);
        ਜੇ (r < 0)
                ਵਾਪਸ log_debug_errno(r, "Failed to set terminal to non-blocking mode: %m");
        ਜੇ (r > 0)
                nonblock_reset = fd;

        r = loop_write_ਭਰਿਆ(fd,
                            "\033[!p"              /* soft terminal reset */
                            ANSI_OSC "104" ANSI_ST /* reset color palette via OSC 104 */
                            ANSI_ਸਾਧਾਰਨ            /* reset colors */
                            "\033[?7h"             /* enable line-wrapping */
                            "\033[1G"              /* place cursor at beginning of current line */
                            "\033[0J",             /* erase till end of screen */
                            SIZE_MAX,
                            CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        ਜੇ (r < 0)
                log_debug_errno(r, "Failed to reset terminal through ANSI sequences: %m");

        ਵਾਪਸ r;
}

ਖਾਲੀ reset_dev_console_fd(ਪੂਰਨ fd, bool switch_ਮੰਜ਼ਿਲ_ਪਾਠ) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);

        _cleanup_close_ ਪੂਰਨ ਤਾਲਾ_fd = ਤਾਲਾ_dev_console();
        ਜੇ (ਤਾਲਾ_fd < 0)
                log_debug_errno(ਤਾਲਾ_fd, "Failed to lock /dev/console, ignoring: %m");

        r = terminal_reset_ioctl(fd, switch_ਮੰਜ਼ਿਲ_ਪਾਠ);
        ਜੇ (r < 0)
                log_warning_errno(r, "Failed to reset /dev/console, ignoring: %m");

        ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੰਕਤੀਆਂ, ਖਾਨੇ_ਗਿਣਤੀ;
        r = proc_cmdline_tty_ਮਾਪ("/dev/console", &ਪੰਕਤੀਆਂ, &ਖਾਨੇ_ਗਿਣਤੀ);
        ਜੇ (r < 0)
                log_warning_errno(r, "Failed to get /dev/console size, ignoring: %m");
        ਨਹੀਂਤਾਂ ਜੇ (r > 0) {
                r = terminal_ਸੈੱਟ_ਮਾਪ_fd(fd, ਨਿਰਾ, ਪੰਕਤੀਆਂ, ਖਾਨੇ_ਗਿਣਤੀ);
                ਜੇ (r < 0)
                        log_warning_errno(r, "Failed to set configured terminal size on /dev/console, ignoring: %m");
        } ਨਹੀਂਤਾਂ
                (ਖਾਲੀ) terminal_fix_ਮਾਪ(fd, fd);

        r = terminal_reset_ansi_seq(fd);
        ਜੇ (r < 0)
                log_warning_errno(r, "Failed to reset /dev/console using ANSI sequences, ignoring: %m");
}

ਪੂਰਨ ਤਾਲਾ_dev_console(ਖਾਲੀ) {
        _cleanup_close_ ਪੂਰਨ fd = -EBADF;
        ਪੂਰਨ r;

        /* NB: We do not use O_NOFOLLOW here, because some container managers might place a symlink to some
         * pty in /dev/console, in which case it should be fine to lock the target TTY. */
        fd = open_terminal("/dev/console", O_RDONLY|O_CLOEXEC|O_NOCTTY);
        ਜੇ (fd < 0)
                ਵਾਪਸ fd;

        r = ਤਾਲਾ_generic(fd, ਤਾਲਾ_BSD, LOCK_EX);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ TAKE_FD(fd);
}

ਪੂਰਨ make_console_stdio(bool switch_ਮੰਜ਼ਿਲ_ਪਾਠ) {
        ਪੂਰਨ fd, r;

        /* Make /dev/console the controlling terminal and stdin/stdout/stderr, if we can. If we can't use
         * /dev/null instead. This is particularly useful if /dev/console is turned off, e.g. if console=null
         * is specified on the kernel command line. */

        fd = acquire_terminal("/dev/console", ACQUIRE_TERMINAL_FORCE|ACQUIRE_TERMINAL_PERMISSIVE, USEC_INFINITY);
        ਜੇ (fd < 0) {
                log_warning_errno(fd, "Failed to acquire terminal, using /dev/null stdin/stdout/stderr instead: %m");

                r = make_null_stdio();
                ਜੇ (r < 0)
                        ਵਾਪਸ log_error_errno(r, "Failed to make /dev/null stdin/stdout/stderr: %m");

        } ਨਹੀਂਤਾਂ {
                reset_dev_console_fd(fd, switch_ਮੰਜ਼ਿਲ_ਪਾਠ);

                r = rearrange_stdio(fd, fd, fd); /* This invalidates 'fd' both on success and on failure. */
                ਜੇ (r < 0)
                        ਵਾਪਸ log_error_errno(r, "Failed to make terminal stdin/stdout/stderr: %m");
        }

        reset_terminal_feature_caches();
        ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ vtnr_from_tty_raw(ਅਚਲ ਅੱਖਰ *tty, ਬਿਨਾਨਿਸ਼ਾਨੀ *ret) {
        ਪ੍ਰਤਿਜ੍ਞਾ(tty);

        tty = skip_dev_ਅਗੇਤਰ(tty);

        ਅਚਲ ਅੱਖਰ *e = startswith(tty, "tty");
        ਜੇ (!e)
                ਵਾਪਸ -EINVAL;

        ਵਾਪਸ ਸੁਰੱਖਿਅਤ_atou(e, ret);
}

ਪੂਰਨ vtnr_from_tty(ਅਚਲ ਅੱਖਰ *tty) {
        ਬਿਨਾਨਿਸ਼ਾਨੀ u;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(tty);

        r = vtnr_from_tty_raw(tty, &u);
        ਜੇ (r < 0)
                ਵਾਪਸ r;
        ਜੇ (!vtnr_is_valid(u))
                ਵਾਪਸ -ERANGE;

        ਵਾਪਸ (ਪੂਰਨ) u;
}

bool tty_is_vc(ਅਚਲ ਅੱਖਰ *tty) {
        ਪ੍ਰਤਿਜ੍ਞਾ(tty);

        /* NB: for >= 0 values no range check is conducted here, on the assumption that the caller will
         * either extract vtnr through vtnr_from_tty() later where ERANGE would be reported, or doesn't care
         * about whether it's strictly valid, but only asking "does this fall into the vt category?", for which
         * "yes" seems to be a better answer. */

        ਵਾਪਸ vtnr_from_tty_raw(tty, /* ret= */ ਨਿਰਾ) >= 0;
}

bool tty_is_console(ਅਚਲ ਅੱਖਰ *tty) {
        ਪ੍ਰਤਿਜ੍ਞਾ(tty);

        ਵਾਪਸ streq(skip_dev_ਅਗੇਤਰ(tty), "console");
}

ਪੂਰਨ resolve_dev_console(ਅੱਖਰ **ret) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        /* Resolve where /dev/console is pointing to. If /dev/console is a symlink (like in container
         * managers), we'll just resolve the symlink. If it's a real device node, we'll use if
         * /sys/class/tty/tty0/active, but only if /sys/ is actually ours (i.e. not read-only-mounted which
         * is a sign for container setups). */

        _cleanup_free_ ਅੱਖਰ *chased = ਨਿਰਾ;
        r = chase("/dev/console", /* root= */ ਨਿਰਾ, /* flags= */ 0, &chased, /* ret_fd= */ ਨਿਰਾ);
        ਜੇ (r < 0)
                ਵਾਪਸ r;
        ਜੇ (!ਰਸਤਾ_equal(chased, "/dev/console")) {
                *ret = TAKE_PTR(chased);
                ਵਾਪਸ 0;
        }

        r = ਰਸਤਾ_is_read_only_fs("/sys");
        ਜੇ (r < 0)
                ਵਾਪਸ r;
        ਜੇ (r > 0)
                ਵਾਪਸ -ENOMEDIUM;

        _cleanup_free_ ਅੱਖਰ *ਸਰਗਰਮ = ਨਿਰਾ;
        r = read_one_ਸਤਰ_file("/sys/class/tty/console/active", &ਸਰਗਰਮ);
        ਜੇ (r < 0)
                ਵਾਪਸ r;
        ਜੇ (r == 0)
                ਵਾਪਸ -ENXIO;

        /* If multiple log outputs are configured the last one is what /dev/console points to */
        ਅਚਲ ਅੱਖਰ *tty = strrchr(ਸਰਗਰਮ, ' ');
        ਜੇ (tty)
                tty++;
        ਨਹੀਂਤਾਂ
                tty = ਸਰਗਰਮ;

        ਜੇ (streq(tty, "tty0")) {
                ਸਰਗਰਮ = mfree(ਸਰਗਰਮ);

                /* Get the active VC (e.g. tty1) */
                r = read_one_ਸਤਰ_file("/sys/class/tty/tty0/active", &ਸਰਗਰਮ);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
                ਜੇ (r == 0)
                        ਵਾਪਸ -ENXIO;

                tty = ਸਰਗਰਮ;
        }

        _cleanup_free_ ਅੱਖਰ *ਰਸਤਾ = ਨਿਰਾ;
        ਰਸਤਾ = ਰਸਤਾ_join("/dev", tty);
        ਜੇ (!ਰਸਤਾ)
                ਵਾਪਸ -ENOMEM;

        *ret = TAKE_PTR(ਰਸਤਾ);
        ਵਾਪਸ 0;
}

ਪੂਰਨ get_kernel_consoles(ਅੱਖਰ ***ret) {
        _cleanup_strv_free_ ਅੱਖਰ **l = ਨਿਰਾ;
        _cleanup_free_ ਅੱਖਰ *ਸਤਰ = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        /* If /sys/ is mounted read-only this means we are running in some kind of container environment.
         * In that case /sys/ would reflect the host system, not us, hence ignore the data we can read from it. */
        ਜੇ (ਰਸਤਾ_is_read_only_fs("/sys") > 0)
                ਜਾਓ fallback;

        r = read_one_ਸਤਰ_file("/sys/class/tty/console/active", &ਸਤਰ);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਲਈ (ਅਚਲ ਅੱਖਰ *p = ਸਤਰ;;) {
                _cleanup_free_ ਅੱਖਰ *tty = ਨਿਰਾ, *ਰਸਤਾ = ਨਿਰਾ;

                r = ਕੱਢੋ_ਪਹਿਲਾ_ਸ਼ਬਦ(&p, &tty, ਨਿਰਾ, 0);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
                ਜੇ (r == 0)
                        ਰੋਕੋ;

                ਜੇ (streq(tty, "tty0")) {
                        tty = mfree(tty);
                        r = read_one_ਸਤਰ_file("/sys/class/tty/tty0/active", &tty);
                        ਜੇ (r < 0)
                                ਵਾਪਸ r;
                        ਜੇ (r == 0) {
                                log_debug("No VT active, skipping /dev/tty0.");
                                ਜਾਰੀ;
                        }
                }

                ਰਸਤਾ = ਰਸਤਾ_join("/dev", tty);
                ਜੇ (!ਰਸਤਾ)
                        ਵਾਪਸ -ENOMEM;

                ਜੇ (access(ਰਸਤਾ, F_OK) < 0) {
                        log_debug_errno(errno, "Console device %s is not accessible, skipping: %m", ਰਸਤਾ);
                        ਜਾਰੀ;
                }

                r = strv_consume(&l, TAKE_PTR(ਰਸਤਾ));
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
        }

        ਜੇ (strv_isempty(l)) {
                log_debug("No devices found for system console");
                ਜਾਓ fallback;
        }

        *ret = TAKE_PTR(l);
        ਵਾਪਸ strv_ਲੰਮਾਈ(*ret);

fallback:
        r = strv_extend(&l, "/dev/console");
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        *ret = TAKE_PTR(l);
        ਵਾਪਸ 0;
}

bool tty_is_vc_resolve(ਅਚਲ ਅੱਖਰ *tty) {
        _cleanup_free_ ਅੱਖਰ *resolved = ਨਿਰਾ;

        ਪ੍ਰਤਿਜ੍ਞਾ(tty);

        ਜੇ (streq(skip_dev_ਅਗੇਤਰ(tty), "console")) {
                ਜੇ (resolve_dev_console(&resolved) < 0)
                        ਵਾਪਸ false;

                tty = resolved;
        }

        ਵਾਪਸ tty_is_vc(tty);
}

ਪੂਰਨ fd_columns(ਪੂਰਨ fd) {
        ਬਣਤਰ winsize ws = {};

        ਜੇ (fd < 0)
                ਵਾਪਸ -EBADF;

        ਜੇ (ioctl(fd, TIOCGWINSZ, &ws) < 0)
                ਵਾਪਸ -errno;

        ਜੇ (ws.ws_col <= 0)
                ਵਾਪਸ -ENODATA; /* some tty types come up with invalid row/column initially, return a recognizable error for that */

        ਵਾਪਸ ws.ws_col;
}

ਪੂਰਨ getenv_columns(ਖਾਲੀ) {
        ਪੂਰਨ r;

        ਅਚਲ ਅੱਖਰ *e = getenv("COLUMNS");
        ਜੇ (!e)
                ਵਾਪਸ -ENXIO;

        ਬਿਨਾਨਿਸ਼ਾਨੀ c;
        r = ਸੁਰੱਖਿਅਤ_atou_bounded(e, 1, USHRT_MAX, &c);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ (ਪੂਰਨ) c;
}

ਬਿਨਾਨਿਸ਼ਾਨੀ columns(ਖਾਲੀ) {

        ਜੇ (cached_columns > 0)
                ਵਾਪਸ cached_columns;

        ਪੂਰਨ c = getenv_columns();
        ਜੇ (c < 0) {
                c = fd_columns(STDOUT_FILENO);
                ਜੇ (c < 0)
                        c = 80;
        }

        ਪ੍ਰਤਿਜ੍ਞਾ(c > 0);

        cached_columns = c;
        ਵਾਪਸ cached_columns;
}

ਪੂਰਨ fd_ਸਤਰਾਂ(ਪੂਰਨ fd) {
        ਬਣਤਰ winsize ws = {};

        ਜੇ (fd < 0)
                ਵਾਪਸ -EBADF;

        ਜੇ (ioctl(fd, TIOCGWINSZ, &ws) < 0)
                ਵਾਪਸ -errno;

        ਜੇ (ws.ws_row <= 0)
                ਵਾਪਸ -ENODATA; /* some tty types come up with invalid row/column initially, return a recognizable error for that */

        ਵਾਪਸ ws.ws_row;
}

ਬਿਨਾਨਿਸ਼ਾਨੀ ਸਤਰਾਂ(ਖਾਲੀ) {
        ਅਚਲ ਅੱਖਰ *e;
        ਪੂਰਨ l;

        ਜੇ (cached_ਸਤਰਾਂ > 0)
                ਵਾਪਸ cached_ਸਤਰਾਂ;

        l = 0;
        e = getenv("LINES");
        ਜੇ (e)
                (ਖਾਲੀ) ਸੁਰੱਖਿਅਤ_atoi(e, &l);

        ਜੇ (l <= 0 || l > USHRT_MAX) {
                l = fd_ਸਤਰਾਂ(STDOUT_FILENO);
                ਜੇ (l <= 0)
                        l = 24;
        }

        cached_ਸਤਰਾਂ = l;
        ਵਾਪਸ cached_ਸਤਰਾਂ;
}

ਪੂਰਨ terminal_ਸੈੱਟ_ਮਾਪ_fd(ਪੂਰਨ fd, ਅਚਲ ਅੱਖਰ *ident, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੰਕਤੀਆਂ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਖਾਨੇ_ਗਿਣਤੀ) {
        ਬਣਤਰ winsize ws;

        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);

        ਜੇ (!ident)
                ident = "TTY";

        ਜੇ (ਪੰਕਤੀਆਂ == UINT_MAX && ਖਾਨੇ_ਗਿਣਤੀ == UINT_MAX)
                ਵਾਪਸ 0;

        ਜੇ (ioctl(fd, TIOCGWINSZ, &ws) < 0)
                ਵਾਪਸ log_debug_errno(errno,
                                       "TIOCGWINSZ ioctl for getting %s size failed, not setting terminal size: %m",
                                       ident);

        ਜੇ (ਪੰਕਤੀਆਂ == UINT_MAX)
                ਪੰਕਤੀਆਂ = ws.ws_row;
        ਨਹੀਂਤਾਂ ਜੇ (ਪੰਕਤੀਆਂ > USHRT_MAX)
                ਪੰਕਤੀਆਂ = USHRT_MAX;

        ਜੇ (ਖਾਨੇ_ਗਿਣਤੀ == UINT_MAX)
                ਖਾਨੇ_ਗਿਣਤੀ = ws.ws_col;
        ਨਹੀਂਤਾਂ ਜੇ (ਖਾਨੇ_ਗਿਣਤੀ > USHRT_MAX)
                ਖਾਨੇ_ਗਿਣਤੀ = USHRT_MAX;

        ਜੇ (ਪੰਕਤੀਆਂ == ws.ws_row && ਖਾਨੇ_ਗਿਣਤੀ == ws.ws_col)
                ਵਾਪਸ 0;

        ws.ws_row = ਪੰਕਤੀਆਂ;
        ws.ws_col = ਖਾਨੇ_ਗਿਣਤੀ;

        ਜੇ (ioctl(fd, TIOCSWINSZ, &ws) < 0)
                ਵਾਪਸ log_debug_errno(errno, "TIOCSWINSZ ioctl for setting %s size failed: %m", ident);

        ਵਾਪਸ 0;
}

ਪੂਰਨ proc_cmdline_tty_ਮਾਪ(ਅਚਲ ਅੱਖਰ *tty, ਬਿਨਾਨਿਸ਼ਾਨੀ *ret_ਪੰਕਤੀਆਂ, ਬਿਨਾਨਿਸ਼ਾਨੀ *ret_ਖਾਨੇ_ਗਿਣਤੀ) {
        _cleanup_free_ ਅੱਖਰ *rowskey = ਨਿਰਾ, *rowsvalue = ਨਿਰਾ, *colskey = ਨਿਰਾ, *colsvalue = ਨਿਰਾ;
        ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੰਕਤੀਆਂ = UINT_MAX, ਖਾਨੇ_ਗਿਣਤੀ = UINT_MAX;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(tty);

        ਜੇ (!ret_ਪੰਕਤੀਆਂ && !ret_ਖਾਨੇ_ਗਿਣਤੀ)
                ਵਾਪਸ 0;

        tty = skip_dev_ਅਗੇਤਰ(tty);
        ਜੇ (ਰਸਤਾ_startswith(tty, "pts/"))
                ਵਾਪਸ -EMEDIUMTYPE;
        ਜੇ (!ਵਿੱਚ_charset(tty, ALPHANUMERICAL))
                ਵਾਪਸ log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "TTY name '%s' contains non-alphanumeric characters, not searching kernel cmdline for size.", tty);

        rowskey = strjoin("systemd.tty.rows.", tty);
        ਜੇ (!rowskey)
                ਵਾਪਸ -ENOMEM;

        colskey = strjoin("systemd.tty.columns.", tty);
        ਜੇ (!colskey)
                ਵਾਪਸ -ENOMEM;

        r = proc_cmdline_get_ਕੁੰਜੀ_many(/* flags= */ 0,
                                      rowskey, &rowsvalue,
                                      colskey, &colsvalue);
        ਜੇ (r < 0)
                ਵਾਪਸ log_debug_errno(r, "Failed to read TTY size of %s from kernel cmdline: %m", tty);

        ਜੇ (rowsvalue) {
                r = ਸੁਰੱਖਿਅਤ_atou(rowsvalue, &ਪੰਕਤੀਆਂ);
                ਜੇ (r < 0)
                        ਵਾਪਸ log_debug_errno(r, "Failed to parse %s=%s: %m", rowskey, rowsvalue);
        }

        ਜੇ (colsvalue) {
                r = ਸੁਰੱਖਿਅਤ_atou(colsvalue, &ਖਾਨੇ_ਗਿਣਤੀ);
                ਜੇ (r < 0)
                        ਵਾਪਸ log_debug_errno(r, "Failed to parse %s=%s: %m", colskey, colsvalue);
        }

        ਜੇ (ret_ਪੰਕਤੀਆਂ)
                *ret_ਪੰਕਤੀਆਂ = ਪੰਕਤੀਆਂ;
        ਜੇ (ret_ਖਾਨੇ_ਗਿਣਤੀ)
                *ret_ਖਾਨੇ_ਗਿਣਤੀ = ਖਾਨੇ_ਗਿਣਤੀ;

        ਵਾਪਸ ਪੰਕਤੀਆਂ != UINT_MAX || ਖਾਨੇ_ਗਿਣਤੀ != UINT_MAX;
}

/* intended to be used as a SIGWINCH sighandler */
ਖਾਲੀ columns_ਸਤਰਾਂ_cache_reset(ਪੂਰਨ signum) {
        cached_columns = 0;
        cached_ਸਤਰਾਂ = 0;
}

ਖਾਲੀ reset_terminal_feature_caches(ਖਾਲੀ) {
        cached_columns = 0;
        cached_ਸਤਰਾਂ = 0;

        cached_on_tty = -1;
        cached_on_dev_null = -1;

        reset_ansi_feature_caches();
}

bool on_tty(ਖਾਲੀ) {

        /* We check both stdout and stderr, so that situations where pipes on the shell are used are reliably
         * recognized, regardless if only the output or the errors are piped to some place. Since on_tty() is generally
         * used to default to a safer, non-interactive, non-color mode of operation it's probably good to be defensive
         * here, and check for both. Note that we don't check for STDIN_FILENO, because it should fine to use fancy
         * terminal functionality when outputting stuff, even if the input is piped to us. */

        ਜੇ (cached_on_tty < 0)
                cached_on_tty =
                        isatty_ਸੁਰੱਖਿਅਤ(STDOUT_FILENO) &&
                        isatty_ਸੁਰੱਖਿਅਤ(STDERR_FILENO);

        ਵਾਪਸ cached_on_tty;
}

ਪੂਰਨ getttyname_malloc(ਪੂਰਨ fd, ਅੱਖਰ **ret) {
        ਅੱਖਰ ਰਸਤਾ[PATH_MAX]; /* PATH_MAX is counted *with* the trailing NUL byte */
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        r = ttyname_r(fd, ਰਸਤਾ, ਆਕਾਰ ਰਸਤਾ); /* positive error */
        ਪ੍ਰਤਿਜ੍ਞਾ(r >= 0);
        ਜੇ (r == ERANGE)
                ਵਾਪਸ -ENAMETOOLONG;
        ਜੇ (r > 0)
                ਵਾਪਸ -r;

        ਵਾਪਸ strdup_ਮੰਜ਼ਿਲ(ret, skip_dev_ਅਗੇਤਰ(ਰਸਤਾ));
}

ਪੂਰਨ getttyname_harder(ਪੂਰਨ fd, ਅੱਖਰ **ret) {
        _cleanup_free_ ਅੱਖਰ *s = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        r = getttyname_malloc(fd, &s);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (streq(s, "tty"))
                ਵਾਪਸ get_ctty(0, ਨਿਰਾ, ret);

        *ret = TAKE_PTR(s);
        ਵਾਪਸ 0;
}

ਪੂਰਨ get_ctty_devnr(pid_t pid, dev_t *ret) {
        _cleanup_free_ ਅੱਖਰ *ਸਤਰ = ਨਿਰਾ;
        ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ttynr;
        ਅਚਲ ਅੱਖਰ *p;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(pid >= 0);

        p = procfs_file_alloca(pid, "stat");
        r = read_one_ਸਤਰ_file(p, &ਸਤਰ);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        p = strrchr(ਸਤਰ, ')');
        ਜੇ (!p)
                ਵਾਪਸ -EIO;

        p++;

        ਜੇ (ਲੜੀ_ਤੋਂ_ਪੜ੍ਹੋ(p, " "
                   "%*c "  /* state */
                   "%*d "  /* ppid */
                   "%*d "  /* pgrp */
                   "%*d "  /* session */
                   "%lu ", /* ttynr */
                   &ttynr) != 1)
                ਵਾਪਸ -EIO;

        ਜੇ (devnum_is_zero(ttynr))
                ਵਾਪਸ -ENXIO;

        ਜੇ (ret)
                *ret = (dev_t) ttynr;

        ਵਾਪਸ 0;
}

ਪੂਰਨ get_ctty(pid_t pid, dev_t *ret_devnr, ਅੱਖਰ **ret) {
        ਅੱਖਰ pty[STRLEN("/dev/pts/") + DECIMAL_STR_MAX(dev_t) + 1];
        _cleanup_free_ ਅੱਖਰ *buf = ਨਿਰਾ;
        ਅਚਲ ਅੱਖਰ *fn = ਨਿਰਾ, *w;
        dev_t devnr;
        ਪੂਰਨ r;

        r = get_ctty_devnr(pid, &devnr);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        r = device_ਰਸਤਾ_make_canonical(S_IFCHR, devnr, &buf);
        ਜੇ (r < 0) {
                ਬਣਤਰ stat st;

                ਜੇ (r != -ENOENT) /* No symlink for this in /dev/char/? */
                        ਵਾਪਸ r;

                /* Maybe this is PTY? PTY devices are not listed in /dev/char/, as they don't follow the
                 * Linux device model and hence device_path_make_canonical() doesn't work for them. Let's
                 * assume this is a PTY for a moment, and check if the device node this would then map to in
                 * /dev/pts/ matches the one we are looking for. This way we don't have to hardcode the major
                 * number (which is 136 btw), but we still rely on the fact that PTY numbers map directly to
                 * the minor number of the pty. */
                xsprintf(pty, "/dev/pts/%u", minor(devnr));

                ਜੇ (stat(pty, &st) < 0) {
                        ਜੇ (errno != ENOENT)
                                ਵਾਪਸ -errno;

                } ਨਹੀਂਤਾਂ ਜੇ (S_ISCHR(st.st_mode) && devnr == st.st_rdev) /* Bingo! */
                        fn = pty;

                ਜੇ (!fn) {
                        /* Doesn't exist, or not a PTY? Probably something similar to the PTYs which have no
                         * symlink in /dev/char/. Let's return something vaguely useful. */
                        r = device_ਰਸਤਾ_make_major_minor(S_IFCHR, devnr, &buf);
                        ਜੇ (r < 0)
                                ਵਾਪਸ r;

                        fn = buf;
                }
        } ਨਹੀਂਤਾਂ
                fn = buf;

        w = ਰਸਤਾ_startswith(fn, "/dev/");
        ਜੇ (!w)
                ਵਾਪਸ -EINVAL;

        ਜੇ (ret) {
                r = strdup_ਮੰਜ਼ਿਲ(ret, w);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
        }

        ਜੇ (ret_devnr)
                *ret_devnr = devnr;

        ਵਾਪਸ 0;
}

ਪੂਰਨ ptsname_malloc(ਪੂਰਨ fd, ਅੱਖਰ **ret) {
        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        ਲਈ (size_t l = 50;;) {
                _cleanup_free_ ਅੱਖਰ *c = ਨਿਰਾ;

                c = new(ਅੱਖਰ, l);
                ਜੇ (!c)
                        ਵਾਪਸ -ENOMEM;

                ਜੇ (ptsname_r(fd, c, l) >= 0) {
                        *ret = TAKE_PTR(c);
                        ਵਾਪਸ 0;
                }
                ਜੇ (errno != ERANGE)
                        ਵਾਪਸ -errno;

                ਜੇ (!MUL_ਸੌਂਪੋ_ਸੁਰੱਖਿਅਤ(&l, 2))
                        ਵਾਪਸ -ENOMEM;
        }
}

ਪੂਰਨ openpt_allocate(ਪੂਰਨ flags, ਅੱਖਰ **ret_peer_ਰਸਤਾ) {
        _cleanup_close_ ਪੂਰਨ fd = -EBADF;
        ਪੂਰਨ r;

        fd = posix_openpt(flags|O_NOCTTY|O_CLOEXEC);
        ਜੇ (fd < 0)
                ਵਾਪਸ -errno;

        _cleanup_free_ ਅੱਖਰ *p = ਨਿਰਾ;
        ਜੇ (ret_peer_ਰਸਤਾ) {
                r = ptsname_malloc(fd, &p);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;

                ਜੇ (!ਰਸਤਾ_startswith(p, "/dev/pts/"))
                        ਵਾਪਸ -EINVAL;
        }

        ਜੇ (unlockpt(fd) < 0)
                ਵਾਪਸ -errno;

        ਜੇ (ret_peer_ਰਸਤਾ)
                *ret_peer_ਰਸਤਾ = TAKE_PTR(p);

        ਵਾਪਸ TAKE_FD(fd);
}

ਸਥਿਰ ਪੂਰਨ ptsname_namespace(ਪੂਰਨ pty, ਅੱਖਰ **ret) {
        ਪੂਰਨ no = -1;

        ਪ੍ਰਤਿਜ੍ਞਾ(pty >= 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        /* Like ptsname(), but doesn't assume that the path is
         * accessible in the local namespace. */

        ਜੇ (ioctl(pty, TIOCGPTN, &no) < 0)
                ਵਾਪਸ -errno;

        ਜੇ (no < 0)
                ਵਾਪਸ -EIO;

        ਜੇ (asprintf(ret, "/dev/pts/%i", no) < 0)
                ਵਾਪਸ -ENOMEM;

        ਵਾਪਸ 0;
}

ਪੂਰਨ openpt_allocate_ਵਿੱਚ_namespace(
                ਅਚਲ PidRef *pidref,
                ਪੂਰਨ flags,
                ਅੱਖਰ **ret_peer_ਰਸਤਾ) {

        _cleanup_close_ ਪੂਰਨ pidnsfd = -EBADF, mntnsfd = -EBADF, usernsfd = -EBADF, rootfd = -EBADF, fd = -EBADF;
        _cleanup_close_pair_ ਪੂਰਨ pair[2] = EBADF_PAIR;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(pidref);

        r = pidref_namespace_open(pidref, &pidnsfd, &mntnsfd, /* ret_netns_fd= */ ਨਿਰਾ, &usernsfd, &rootfd);
        ਜੇ (r < 0)
                ਵਾਪਸ log_debug_errno(r, "Failed to open namespaces of PID "PID_FMT": %m", pidref->pid);

        ਜੇ (socketpair(AF_UNIX, SOCK_DGRAM|SOCK_CLOEXEC, 0, pair) < 0)
                ਵਾਪਸ -errno;

        r = namespace_fork(
                        "(sd-openptns)",
                        "(sd-openpt)",
                        FORK_RESET_ਸੰਕੇਤ|FORK_DEATHSIG_SIGKILL|FORK_WAIT,
                        pidnsfd,
                        mntnsfd,
                        /* netns_fd= */ -EBADF,
                        usernsfd,
                        rootfd,
                        /* ret= */ ਨਿਰਾ);
        ਜੇ (r < 0)
                ਵਾਪਸ r;
        ਜੇ (r == 0) {
                pair[0] = ਸੁਰੱਖਿਅਤ_close(pair[0]);

                fd = openpt_allocate(flags, /* ret_peer_path= */ ਨਿਰਾ);
                ਜੇ (fd < 0)
                        _exit(EXIT_FAILURE);

                ਜੇ (send_one_fd(pair[1], fd, 0) < 0)
                        _exit(EXIT_FAILURE);

                _exit(EXIT_SUCCESS);
        }

        pair[1] = ਸੁਰੱਖਿਅਤ_close(pair[1]);

        fd = receive_one_fd(pair[0], 0);
        ਜੇ (fd < 0)
                ਵਾਪਸ fd;

        ਜੇ (ret_peer_ਰਸਤਾ) {
                r = ptsname_namespace(fd, ret_peer_ਰਸਤਾ);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
        }

        ਵਾਪਸ TAKE_FD(fd);
}

ਸਥਿਰ bool on_dev_null(ਖਾਲੀ) {
        ਬਣਤਰ stat dst, ost, est;

        ਜੇ (cached_on_dev_null >= 0)
                ਵਾਪਸ cached_on_dev_null;

        ਜੇ (stat("/dev/null", &dst) < 0 || fstat(STDOUT_FILENO, &ost) < 0 || fstat(STDERR_FILENO, &est) < 0)
                cached_on_dev_null = false;
        ਨਹੀਂਤਾਂ
                cached_on_dev_null = stat_inode_same(&dst, &ost) && stat_inode_same(&dst, &est);

        ਵਾਪਸ cached_on_dev_null;
}

bool ਪਦ_env_valid(ਅਚਲ ਅੱਖਰ *ਪਦ) {
        /* Checks if the specified $TERM value is suitable for propagation, i.e. is not empty, not set to
         * "unknown" (as is common in CI), and only contains characters valid in terminal type names.
         * Valid $TERM values are things like "xterm-256color", "linux", "screen.xterm-256color", i.e.
         * alphanumeric characters, hyphens, underscores, dots, and plus signs. */
        ਵਾਪਸ !isempty(ਪਦ) &&
                !streq(ਪਦ, "unknown") &&
                ਵਿੱਚ_charset(ਪਦ, ALPHANUMERICAL "-_+.");
}

bool getenv_terminal_is_dumb(ਖਾਲੀ) {
        ਅਚਲ ਅੱਖਰ *e;

        e = getenv("TERM");
        ਜੇ (!e)
                ਵਾਪਸ true;

        ਵਾਪਸ streq(e, "dumb");
}

bool terminal_is_dumb(ਖਾਲੀ) {
        ਜੇ (!on_tty() && !on_dev_null())
                ਵਾਪਸ true;

        ਵਾਪਸ getenv_terminal_is_dumb();
}

bool dev_console_colors_enabled(ਖਾਲੀ) {
        _cleanup_free_ ਅੱਖਰ *s = ਨਿਰਾ;
        Color_ਬਹੁਲਕ m;

        /* Returns true if we assume that color is supported on /dev/console.
         *
         * For that we first check if we explicitly got told to use colors or not, by checking $SYSTEMD_COLORS. If that
         * isn't set we check whether PID 1 has $TERM set, and if not, whether TERM is set on the kernel command
         * line. If we find $TERM set we assume color if it's not set to "dumb", similarly to how regular
         * colors_enabled() operates. */

        m = parse_systemd_colors();
        ਜੇ (m >= 0)
                ਵਾਪਸ m;

        ਜੇ (getenv("NO_COLOR"))
                ਵਾਪਸ false;

        ਜੇ (getenv_for_pid(1, "TERM", &s) <= 0)
                (ਖਾਲੀ) proc_cmdline_get_ਕੁੰਜੀ("TERM", 0, &s);

        ਵਾਪਸ !streq_ptr(s, "dumb");
}

ਪੂਰਨ vt_restore(ਪੂਰਨ fd) {

        ਸਥਿਰ ਅਚਲ ਬਣਤਰ vt_mode ਬਹੁਲਕ = {
                .ਬਹੁਲਕ = VT_AUTO,
        };

        ਪੂਰਨ r, ret = 0;

        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);

        ਜੇ (!isatty_ਸੁਰੱਖਿਅਤ(fd))
                ਵਾਪਸ log_debug_errno(SYNTHETIC_ERRNO(ENOTTY), "Asked to restore the VT for an fd that does not refer to a terminal.");

        ਜੇ (ioctl(fd, KDSETMODE, KD_TEXT) < 0)
                RET_GATHER(ret, log_debug_errno(errno, "Failed to set VT to text mode, ignoring: %m"));

        r = vt_reset_keyboard(fd);
        ਜੇ (r < 0)
                RET_GATHER(ret, log_debug_errno(r, "Failed to reset keyboard mode, ignoring: %m"));

        ਜੇ (ioctl(fd, VT_SETMODE, &ਬਹੁਲਕ) < 0)
                RET_GATHER(ret, log_debug_errno(errno, "Failed to set VT_AUTO mode, ignoring: %m"));

        r = fchmod_and_chown(fd, TTY_ਬਹੁਲਕ, 0, GID_INVALID);
        ਜੇ (r < 0)
                RET_GATHER(ret, log_debug_errno(r, "Failed to chmod()/chown() VT, ignoring: %m"));

        ਵਾਪਸ ret;
}

ਪੂਰਨ vt_release(ਪੂਰਨ fd, bool restore) {
        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);

        /* This function releases the VT by acknowledging the VT-switch signal
         * sent by the kernel and optionally reset the VT in text and auto
         * VT-switching modes. */

        ਜੇ (!isatty_ਸੁਰੱਖਿਅਤ(fd))
                ਵਾਪਸ log_debug_errno(SYNTHETIC_ERRNO(ENOTTY), "Asked to release the VT for an fd that does not refer to a terminal.");

        ਜੇ (ioctl(fd, VT_RELDISP, 1) < 0)
                ਵਾਪਸ -errno;

        ਜੇ (restore)
                ਵਾਪਸ vt_restore(fd);

        ਵਾਪਸ 0;
}

ਖਾਲੀ get_log_colors(ਪੂਰਨ priority, ਅਚਲ ਅੱਖਰ **on, ਅਚਲ ਅੱਖਰ **off, ਅਚਲ ਅੱਖਰ **highlight) {
        /* Note that this will initialize output variables only when there's something to output.
         * The caller must pre-initialize to "" or NULL as appropriate. */

        ਜੇ (priority <= LOG_ERR) {
                ਜੇ (on)
                        *on = ansi_highlight_red();
                ਜੇ (off)
                        *off = ansi_ਸਾਧਾਰਨ();
                ਜੇ (highlight)
                        *highlight = ansi_highlight();

        } ਨਹੀਂਤਾਂ ਜੇ (priority <= LOG_WARNING) {
                ਜੇ (on)
                        *on = ansi_highlight_yellow();
                ਜੇ (off)
                        *off = ansi_ਸਾਧਾਰਨ();
                ਜੇ (highlight)
                        *highlight = ansi_highlight();

        } ਨਹੀਂਤਾਂ ਜੇ (priority <= LOG_NOTICE) {
                ਜੇ (on)
                        *on = ansi_highlight();
                ਜੇ (off)
                        *off = ansi_ਸਾਧਾਰਨ();
                ਜੇ (highlight)
                        *highlight = ansi_highlight_red();

        } ਨਹੀਂਤਾਂ ਜੇ (priority >= LOG_DEBUG) {
                ਜੇ (on)
                        *on = ansi_grey();
                ਜੇ (off)
                        *off = ansi_ਸਾਧਾਰਨ();
                ਜੇ (highlight)
                        *highlight = ansi_highlight_red();
        }
}

ਪੂਰਨ terminal_ਸੈੱਟ_cursor_position(ਪੂਰਨ fd, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੰਕਤੀ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਕਾਲਮ) {
        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);

        ਅੱਖਰ cursor_position[STRLEN("\x1B[" ";" "H") + DECIMAL_STR_MAX(ਬਿਨਾਨਿਸ਼ਾਨੀ) * 2 + 1];
        xsprintf(cursor_position, "\x1B[%u;%uH", ਪੰਕਤੀ, ਕਾਲਮ);

        ਵਾਪਸ loop_write(fd, cursor_position, SIZE_MAX);
}

ਸਥਿਰ ਪੂਰਨ terminal_verify_same(ਪੂਰਨ ਨਿਵੇਸ਼_fd, ਪੂਰਨ output_fd) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਨਿਵੇਸ਼_fd >= 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(output_fd >= 0);

        /* Validates that the specified fds reference the same TTY */

        ਜੇ (ਨਿਵੇਸ਼_fd != output_fd) {
                ਬਣਤਰ stat sti;
                ਜੇ (fstat(ਨਿਵੇਸ਼_fd, &sti) < 0)
                        ਵਾਪਸ -errno;

                r = stat_verify_char(&sti); /* TTYs are character devices */
                ਜੇ (r < 0)
                        ਵਾਪਸ r;

                ਬਣਤਰ stat sto;
                ਜੇ (fstat(output_fd, &sto) < 0)
                        ਵਾਪਸ -errno;

                r = stat_verify_char(&sto);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;

                ਜੇ (sti.st_rdev != sto.st_rdev)
                        ਵਾਪਸ -ENOLINK;
        }

        ਜੇ (!isatty_ਸੁਰੱਖਿਅਤ(ਨਿਵੇਸ਼_fd)) /* The check above was just for char device, but now let's ensure it's actually a tty */
                ਵਾਪਸ -ENOTTY;

        ਵਾਪਸ 0;
}

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ CursorPositionState {
        CURSOR_ਪਾਠ,
        CURSOR_ESCAPE,
        CURSOR_ਪੰਕਤੀ,
        CURSOR_ਕਾਲਮ,
} CursorPositionState;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ CursorPositionContext {
        CursorPositionState state;
        ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੰਕਤੀ, ਕਾਲਮ;
} CursorPositionContext;

ਸਥਿਰ ਪੂਰਨ ਜਾਂਚ_cursor_position_response(
                CursorPositionContext *context,
                ਅਚਲ ਅੱਖਰ *buf,
                size_t ਮਾਪ,
                size_t *ret_processed) {

        ਪ੍ਰਤਿਜ੍ਞਾ(context);
        ਪ੍ਰਤਿਜ੍ਞਾ(buf);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret_processed);

        ਲਈ (size_t i = 0; i < ਮਾਪ; i++) {
                ਅੱਖਰ c = buf[i];

                ਚੋਣ (context->state) {

                ਹਾਲਤ CURSOR_ਪਾਠ:
                        context->state = c == '\x1B' ? CURSOR_ESCAPE : CURSOR_ਪਾਠ;
                        ਰੋਕੋ;

                ਹਾਲਤ CURSOR_ESCAPE:
                        context->state = c == '[' ? CURSOR_ਪੰਕਤੀ : CURSOR_ਪਾਠ;
                        ਰੋਕੋ;

                ਹਾਲਤ CURSOR_ਪੰਕਤੀ:
                        ਜੇ (c == ';')
                                context->state = context->ਪੰਕਤੀ > 0 ? CURSOR_ਕਾਲਮ : CURSOR_ਪਾਠ;
                        ਨਹੀਂਤਾਂ {
                                ਪੂਰਨ d = undecchar(c);

                                /* We read a decimal character, let's suffix it to the number we so far read,
                                 * but let's do an overflow check first. */
                                ਜੇ (d < 0 || context->ਪੰਕਤੀ > (UINT_MAX-d)/10)
                                        context->state = CURSOR_ਪਾਠ;
                                ਨਹੀਂਤਾਂ
                                        context->ਪੰਕਤੀ = context->ਪੰਕਤੀ * 10 + d;
                        }
                        ਰੋਕੋ;

                ਹਾਲਤ CURSOR_ਕਾਲਮ:
                        ਜੇ (c == 'R') {
                                ਜੇ (context->ਕਾਲਮ > 0) {
                                        *ret_processed = i + 1;
                                        ਵਾਪਸ 1; /* success! */
                                }

                                context->state = CURSOR_ਪਾਠ;
                        } ਨਹੀਂਤਾਂ {
                                ਪੂਰਨ d = undecchar(c);

                                /* As above, add the decimal character to our column number */
                                ਜੇ (d < 0 || context->ਕਾਲਮ > (UINT_MAX-d)/10)
                                        context->state = CURSOR_ਪਾਠ;
                                ਨਹੀਂਤਾਂ
                                        context->ਕਾਲਮ = context->ਕਾਲਮ * 10 + d;
                        }

                        ਰੋਕੋ;
                }

                /* Reset any positions we might have picked up */
                ਜੇ (ਵਿੱਚ_ਸੈੱਟ(context->state, CURSOR_ਪਾਠ, CURSOR_ESCAPE))
                        context->ਪੰਕਤੀ = context->ਕਾਲਮ = 0;
        }

        *ret_processed = ਮਾਪ;
        ਵਾਪਸ 0; /* all good, but not enough data yet */
}

ਪੂਰਨ terminal_get_cursor_position(
                ਪੂਰਨ ਨਿਵੇਸ਼_fd,
                ਪੂਰਨ output_fd,
                ਬਿਨਾਨਿਸ਼ਾਨੀ *ret_ਪੰਕਤੀ,
                ਬਿਨਾਨਿਸ਼ਾਨੀ *ret_ਕਾਲਮ) {

        _cleanup_close_ ਪੂਰਨ nonblock_ਨਿਵੇਸ਼_fd = -EBADF;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਨਿਵੇਸ਼_fd >= 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(output_fd >= 0);

        ਜੇ (getenv_terminal_is_dumb())
                ਵਾਪਸ -EOPNOTSUPP;

        r = terminal_verify_same(ਨਿਵੇਸ਼_fd, output_fd);
        ਜੇ (r < 0)
                ਵਾਪਸ log_debug_errno(r, "Called with distinct input/output fds: %m");

        /* Failure to reset the terminal is ignored here and in similar cases below.
         * We already have our result; if cleanup fails it doesn't change the validity of the result. */
        ਬਣਤਰ termios old_termios = TERMIOS_NULL;
        CLEANUP_TERMIOS_RESET(ਨਿਵੇਸ਼_fd, old_termios);

        ਜੇ (tcgetattr(ਨਿਵੇਸ਼_fd, &old_termios) < 0)
                ਵਾਪਸ log_debug_errno(errno, "Failed to get terminal settings: %m");

        ਬਣਤਰ termios new_termios = old_termios;
        termios_disable_echo(&new_termios);

        ਜੇ (tcsetattr(ਨਿਵੇਸ਼_fd, TCSANOW, &new_termios) < 0)
                ਵਾਪਸ log_debug_errno(errno, "Failed to set new terminal settings: %m");

        /* Request cursor position (DSR/CPR) */
        r = loop_write(output_fd, "\x1B[6n", SIZE_MAX);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        /* Open a 2nd input fd, in non-blocking mode, so that we won't ever hang in read() should someone
         * else process the POLLIN. */

        nonblock_ਨਿਵੇਸ਼_fd = r = fd_reopen(ਨਿਵੇਸ਼_fd, O_RDONLY|O_CLOEXEC|O_NONBLOCK|O_NOCTTY);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        usec_t ਸਿਰਾ = usec_ਜੋੜੋ(now(CLOCK_MONOTONIC), CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        ਅੱਖਰ buf[STRLEN("\x1B[1;1R")]; /* The shortest valid reply possible */
        size_t buf_ਭਰਿਆ = 0;
        CursorPositionContext context = {};

        ਲਈ (bool ਪਹਿਲਾ = true;; ਪਹਿਲਾ = false) {
                ਜੇ (buf_ਭਰਿਆ == 0) {
                        usec_t n = now(CLOCK_MONOTONIC);
                        ਜੇ (n >= ਸਿਰਾ)
                                ਵਾਪਸ -EOPNOTSUPP;

                        r = fd_wait_for_event(nonblock_ਨਿਵੇਸ਼_fd, POLLIN, usec_sub_unsigned(ਸਿਰਾ, n));
                        ਜੇ (r < 0)
                                ਵਾਪਸ r;
                        ਜੇ (r == 0)
                                ਵਾਪਸ -EOPNOTSUPP;

                        /* On the first try, read multiple characters, i.e. the shortest valid
                         * reply. Afterwards read byte-wise, since we don't want to read too much, and
                         * unnecessarily drop too many characters from the input queue. */
                        ssize_t l = ਪਠ(nonblock_ਨਿਵੇਸ਼_fd, buf, ਪਹਿਲਾ ? ਆਕਾਰ(buf) : 1);
                        ਜੇ (l < 0) {
                                ਜੇ (errno == EAGAIN)
                                        ਜਾਰੀ;

                                ਵਾਪਸ -errno;
                        }

                        ਪ੍ਰਤਿਜ੍ਞਾ((size_t) l <= ਆਕਾਰ(buf));
                        buf_ਭਰਿਆ = l;
                }

                size_t processed;
                r = ਜਾਂਚ_cursor_position_response(&context, buf, buf_ਭਰਿਆ, &processed);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;

                ਪ੍ਰਤਿਜ੍ਞਾ(processed <= buf_ਭਰਿਆ);
                buf_ਭਰਿਆ -= processed;
                memmove(buf, buf + processed, buf_ਭਰਿਆ);

                ਜੇ (r > 0) {
                        /* Superficial validity check */
                        ਜੇ (context.ਪੰਕਤੀ >= 32766 || context.ਕਾਲਮ >= 32766)
                                ਵਾਪਸ -ENODATA;

                        ਜੇ (ret_ਪੰਕਤੀ)
                                *ret_ਪੰਕਤੀ = context.ਪੰਕਤੀ;
                        ਜੇ (ret_ਕਾਲਮ)
                                *ret_ਕਾਲਮ = context.ਕਾਲਮ;

                        ਵਾਪਸ 0;
                }
        }
}

ਪੂਰਨ terminal_reset_defensive(ਪੂਰਨ fd, TerminalResetFlags flags) {
        ਪੂਰਨ r = 0;

        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(!FLAGS_ਸੈੱਟ(flags, TERMINAL_RESET_AVOID_ANSI_SEQ|TERMINAL_RESET_FORCE_ANSI_SEQ));

        /* Resets the terminal comprehensively, i.e. via both ioctl()s and via ANSI sequences, but do so only
         * if $TERM is unset or set to "dumb" */

        ਜੇ (!isatty_ਸੁਰੱਖਿਅਤ(fd))
                ਵਾਪਸ -ENOTTY;

        RET_GATHER(r, terminal_reset_ioctl(fd, FLAGS_ਸੈੱਟ(flags, TERMINAL_RESET_SWITCH_ਮੰਜ਼ਿਲ_ਪਾਠ)));

        ਜੇ (!FLAGS_ਸੈੱਟ(flags, TERMINAL_RESET_AVOID_ANSI_SEQ) &&
            (FLAGS_ਸੈੱਟ(flags, TERMINAL_RESET_FORCE_ANSI_SEQ) || !getenv_terminal_is_dumb()))
                RET_GATHER(r, terminal_reset_ansi_seq(fd));

        ਵਾਪਸ r;
}

ਪੂਰਨ terminal_reset_defensive_locked(ਪੂਰਨ fd, TerminalResetFlags flags) {
        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);

        _cleanup_close_ ਪੂਰਨ ਤਾਲਾ_fd = ਤਾਲਾ_dev_console();
        ਜੇ (ਤਾਲਾ_fd < 0)
                log_debug_errno(ਤਾਲਾ_fd, "Failed to acquire lock for /dev/console, ignoring: %m");

        ਵਾਪਸ terminal_reset_defensive(fd, flags);
}

ਖਾਲੀ termios_disable_echo(ਬਣਤਰ termios *termios) {
        ਪ੍ਰਤਿਜ੍ਞਾ(termios);

        termios->c_lflag &= ~(ICANON|ECHO);
        termios->c_cc[VMIN] = 1;
        termios->c_cc[VTIME] = 0;
}

ਸਥਿਰ bool termios_is_null(ਅਚਲ ਬਣਤਰ termios *t) {
        ਜੇ (!t)
                ਵਾਪਸ true;

        ਵਾਪਸ t->c_iflag == UINT_MAX &&
               t->c_oflag == UINT_MAX &&
               t->c_cflag == UINT_MAX &&
               t->c_lflag == UINT_MAX;
}

ਖਾਲੀ termios_reset(ਅਚਲ TermiosResetContext *c) {
        ਪ੍ਰਤਿਜ੍ਞਾ(c);

        PROTECT_ERRNO;

        ਜੇ (c->fd && *c->fd >= 0 && !termios_is_null(c->termios))
                (ਖਾਲੀ) tcsetattr(*c->fd, TCSANOW, c->termios);
}

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ BackgroundColorState {
        BACKGROUND_ਪਾਠ,
        BACKGROUND_ESCAPE,
        BACKGROUND_BRACKET,
        BACKGROUND_ਪਹਿਲਾ_ONE,
        BACKGROUND_ਦੂਜਾ_ONE,
        BACKGROUND_SEMICOLON,
        BACKGROUND_R,
        BACKGROUND_G,
        BACKGROUND_B,
        BACKGROUND_RED,
        BACKGROUND_GREEN,
        BACKGROUND_BLUE,
        BACKGROUND_STRING_TERMINATOR,
} BackgroundColorState;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ BackgroundColorContext {
        BackgroundColorState state;
        uint32_t red, green, blue;
        ਬਿਨਾਨਿਸ਼ਾਨੀ red_ਬਿੱਟ, green_ਬਿੱਟ, blue_ਬਿੱਟ;
} BackgroundColorContext;

ਸਥਿਰ ਪੂਰਨ ਜਾਂਚ_background_color_response(
                BackgroundColorContext *context,
                ਅਚਲ ਅੱਖਰ *buf,
                size_t ਮਾਪ,
                size_t *ret_processed) {

        ਪ੍ਰਤਿਜ੍ਞਾ(context);
        ਪ੍ਰਤਿਜ੍ਞਾ(buf);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret_processed);

        ਲਈ (size_t i = 0; i < ਮਾਪ; i++) {
                ਅੱਖਰ c = buf[i];

                ਚੋਣ (context->state) {

                ਹਾਲਤ BACKGROUND_ਪਾਠ:
                        context->state = c == '\x1B' ? BACKGROUND_ESCAPE : BACKGROUND_ਪਾਠ;
                        ਰੋਕੋ;

                ਹਾਲਤ BACKGROUND_ESCAPE:
                        context->state = c == ']' ? BACKGROUND_BRACKET : BACKGROUND_ਪਾਠ;
                        ਰੋਕੋ;

                ਹਾਲਤ BACKGROUND_BRACKET:
                        context->state = c == '1' ? BACKGROUND_ਪਹਿਲਾ_ONE : BACKGROUND_ਪਾਠ;
                        ਰੋਕੋ;

                ਹਾਲਤ BACKGROUND_ਪਹਿਲਾ_ONE:
                        context->state = c == '1' ? BACKGROUND_ਦੂਜਾ_ONE : BACKGROUND_ਪਾਠ;
                        ਰੋਕੋ;

                ਹਾਲਤ BACKGROUND_ਦੂਜਾ_ONE:
                        context->state = c == ';' ? BACKGROUND_SEMICOLON : BACKGROUND_ਪਾਠ;
                        ਰੋਕੋ;

                ਹਾਲਤ BACKGROUND_SEMICOLON:
                        context->state = c == 'r' ? BACKGROUND_R : BACKGROUND_ਪਾਠ;
                        ਰੋਕੋ;

                ਹਾਲਤ BACKGROUND_R:
                        context->state = c == 'g' ? BACKGROUND_G : BACKGROUND_ਪਾਠ;
                        ਰੋਕੋ;

                ਹਾਲਤ BACKGROUND_G:
                        context->state = c == 'b' ? BACKGROUND_B : BACKGROUND_ਪਾਠ;
                        ਰੋਕੋ;

                ਹਾਲਤ BACKGROUND_B:
                        context->state = c == ':' ? BACKGROUND_RED : BACKGROUND_ਪਾਠ;
                        ਰੋਕੋ;

                ਹਾਲਤ BACKGROUND_RED:
                        ਜੇ (c == '/')
                                context->state = context->red_ਬਿੱਟ > 0 ? BACKGROUND_GREEN : BACKGROUND_ਪਾਠ;
                        ਨਹੀਂਤਾਂ {
                                ਪੂਰਨ d = unhexchar(c);
                                ਜੇ (d < 0 || context->red_ਬਿੱਟ >= ਆਕਾਰ(context->red)*8)
                                        context->state = BACKGROUND_ਪਾਠ;
                                ਨਹੀਂਤਾਂ {
                                        context->red = (context->red << 4) | d;
                                        context->red_ਬਿੱਟ += 4;
                                }
                        }
                        ਰੋਕੋ;

                ਹਾਲਤ BACKGROUND_GREEN:
                        ਜੇ (c == '/')
                                context->state = context->green_ਬਿੱਟ > 0 ? BACKGROUND_BLUE : BACKGROUND_ਪਾਠ;
                        ਨਹੀਂਤਾਂ {
                                ਪੂਰਨ d = unhexchar(c);
                                ਜੇ (d < 0 || context->green_ਬਿੱਟ >= ਆਕਾਰ(context->green)*8)
                                        context->state = BACKGROUND_ਪਾਠ;
                                ਨਹੀਂਤਾਂ {
                                        context->green = (context->green << 4) | d;
                                        context->green_ਬਿੱਟ += 4;
                                }
                        }
                        ਰੋਕੋ;

                ਹਾਲਤ BACKGROUND_BLUE:
                        ਜੇ (c == '\x07') {
                                ਜੇ (context->blue_ਬਿੱਟ > 0) {
                                        *ret_processed = i + 1;
                                        ਵਾਪਸ 1; /* success! */
                                }

                                context->state = BACKGROUND_ਪਾਠ;
                        } ਨਹੀਂਤਾਂ ਜੇ (c == '\x1b')
                                context->state = context->blue_ਬਿੱਟ > 0 ? BACKGROUND_STRING_TERMINATOR : BACKGROUND_ਪਾਠ;
                        ਨਹੀਂਤਾਂ {
                                ਪੂਰਨ d = unhexchar(c);
                                ਜੇ (d < 0 || context->blue_ਬਿੱਟ >= ਆਕਾਰ(context->blue)*8)
                                        context->state = BACKGROUND_ਪਾਠ;
                                ਨਹੀਂਤਾਂ {
                                        context->blue = (context->blue << 4) | d;
                                        context->blue_ਬਿੱਟ += 4;
                                }
                        }
                        ਰੋਕੋ;

                ਹਾਲਤ BACKGROUND_STRING_TERMINATOR:
                        ਜੇ (c == '\\') {
                                *ret_processed = i + 1;
                                ਵਾਪਸ 1; /* success! */
                        }

                        context->state = c == ']' ? BACKGROUND_ESCAPE : BACKGROUND_ਪਾਠ;
                        ਰੋਕੋ;

                }

                /* Reset any colors we might have picked up */
                ਜੇ (ਵਿੱਚ_ਸੈੱਟ(context->state, BACKGROUND_ਪਾਠ, BACKGROUND_ESCAPE)) {
                        /* reset color */
                        context->red = context->green = context->blue = 0;
                        context->red_ਬਿੱਟ = context->green_ਬਿੱਟ = context->blue_ਬਿੱਟ = 0;
                }
        }

        *ret_processed = ਮਾਪ;
        ਵਾਪਸ 0; /* all good, but not enough data yet */
}

ਪੂਰਨ get_default_background_color(ਦੁੱਗਣਾ *ret_red, ਦੁੱਗਣਾ *ret_green, ਦੁੱਗਣਾ *ret_blue) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret_red);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret_green);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret_blue);

        ਜੇ (!colors_enabled())
                ਵਾਪਸ -EOPNOTSUPP;

        r = terminal_verify_same(STDIN_FILENO, STDOUT_FILENO);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (streq_ptr(getenv("TERM"), "linux")) {
                /* Linux console is black */
                *ret_red = *ret_green = *ret_blue = 0.0;
                ਵਾਪਸ 0;
        }

        /* Open a 2nd input fd, in non-blocking mode, so that we won't ever hang in read()
         * should someone else process the POLLIN. Do all subsequent operations on the new fd. */
        _cleanup_close_ ਪੂਰਨ nonblock_ਨਿਵੇਸ਼_fd = r = fd_reopen(STDIN_FILENO, O_RDONLY|O_CLOEXEC|O_NONBLOCK|O_NOCTTY);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਬਣਤਰ termios old_termios = TERMIOS_NULL;
        CLEANUP_TERMIOS_RESET(nonblock_ਨਿਵੇਸ਼_fd, old_termios);

        ਜੇ (tcgetattr(nonblock_ਨਿਵੇਸ਼_fd, &old_termios) < 0)
                ਵਾਪਸ -errno;

        ਬਣਤਰ termios new_termios = old_termios;
        termios_disable_echo(&new_termios);

        ਜੇ (tcsetattr(nonblock_ਨਿਵੇਸ਼_fd, TCSANOW, &new_termios) < 0)
                ਵਾਪਸ -errno;

        r = loop_write(STDOUT_FILENO, ANSI_OSC "11;?" ANSI_ST, SIZE_MAX);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        usec_t ਸਿਰਾ = usec_ਜੋੜੋ(now(CLOCK_MONOTONIC), CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        ਅੱਖਰ buf[STRLEN(ANSI_OSC "11;rgb:0/0/0" ANSI_ST)]; /* shortest possible reply */
        size_t buf_ਭਰਿਆ = 0;
        BackgroundColorContext context = {};

        ਲਈ (bool ਪਹਿਲਾ = true;; ਪਹਿਲਾ = false) {
                ਜੇ (buf_ਭਰਿਆ == 0) {
                        usec_t n = now(CLOCK_MONOTONIC);
                        ਜੇ (n >= ਸਿਰਾ)
                                ਵਾਪਸ -EOPNOTSUPP;

                        r = fd_wait_for_event(nonblock_ਨਿਵੇਸ਼_fd, POLLIN, usec_sub_unsigned(ਸਿਰਾ, n));
                        ਜੇ (r < 0)
                                ਵਾਪਸ r;
                        ਜੇ (r == 0)
                                ਵਾਪਸ -EOPNOTSUPP;

                        /* On the first try, read multiple characters, i.e. the shortest valid
                         * reply. Afterwards read byte-wise, since we don't want to read too much, and
                         * unnecessarily drop too many characters from the input queue. */
                        ssize_t l = ਪਠ(nonblock_ਨਿਵੇਸ਼_fd, buf, ਪਹਿਲਾ ? ਆਕਾਰ(buf) : 1);
                        ਜੇ (l < 0) {
                                ਜੇ (errno == EAGAIN)
                                        ਜਾਰੀ;
                                ਵਾਪਸ -errno;
                        }

                        ਪ੍ਰਤਿਜ੍ਞਾ((size_t) l <= ਆਕਾਰ(buf));
                        buf_ਭਰਿਆ = l;
                }

                size_t processed;
                r = ਜਾਂਚ_background_color_response(&context, buf, buf_ਭਰਿਆ, &processed);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;

                ਪ੍ਰਤਿਜ੍ਞਾ(processed <= buf_ਭਰਿਆ);
                buf_ਭਰਿਆ -= processed;
                memmove(buf, buf + processed, buf_ਭਰਿਆ);

                ਜੇ (r > 0) {
                        ਪ੍ਰਤਿਜ੍ਞਾ(context.red_ਬਿੱਟ > 0);
                        *ret_red = (ਦੁੱਗਣਾ) context.red / ((UINT64_C(1) << context.red_ਬਿੱਟ) - 1);
                        ਪ੍ਰਤਿਜ੍ਞਾ(context.green_ਬਿੱਟ > 0);
                        *ret_green = (ਦੁੱਗਣਾ) context.green / ((UINT64_C(1) << context.green_ਬਿੱਟ) - 1);
                        ਪ੍ਰਤਿਜ੍ਞਾ(context.blue_ਬਿੱਟ > 0);
                        *ret_blue = (ਦੁੱਗਣਾ) context.blue / ((UINT64_C(1) << context.blue_ਬਿੱਟ) - 1);
                        ਵਾਪਸ 0;
                }
        }
}

/* Determine terminal dimensions by means of ANSI sequences: save the cursor via DECSC, position it far
 * to the bottom right (clamped to actual terminal dimensions), read back via DSR where we ended up, and
 * restore cursor via DECRC. Only needs a single DSR round-trip, and always restores the cursor regardless
 * of whether the response is received.
 *
 * Caller must have already opened a non-blocking input fd and configured termios (echo/icanon off). */
ਸਥਿਰ ਪੂਰਨ terminal_query_ਮਾਪ_by_dsr(
                ਪੂਰਨ nonblock_ਨਿਵੇਸ਼_fd,
                ਪੂਰਨ output_fd,
                ਬਿਨਾਨਿਸ਼ਾਨੀ *ret_ਪੰਕਤੀਆਂ,
                ਬਿਨਾਨਿਸ਼ਾਨੀ *ret_columns) {

        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(nonblock_ਨਿਵੇਸ਼_fd >= 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(output_fd >= 0);

        /* Use DECSC/DECRC to save/restore cursor instead of querying position via DSR. This way the cursor
         * is always restored — even on timeout — and we only need one DSR response instead of two. */
        r = loop_write_ਭਰਿਆ(output_fd,
                            "\x1B" "7"              /* DECSC: save cursor position */
                            "\x1B[32766;32766H"     /* CUP: position cursor far to the right and to the bottom, staying within 16bit signed range */
                            "\x1B[6n"               /* DSR: request cursor position (CPR) */
                            "\x1B" "8",             /* DECRC: restore cursor position */
                            SIZE_MAX,
                            CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        usec_t ਸਿਰਾ = usec_ਜੋੜੋ(now(CLOCK_MONOTONIC), CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        ਅੱਖਰ buf[STRLEN("\x1B[1;1R")]; /* The shortest valid reply possible */
        size_t buf_ਭਰਿਆ = 0;
        CursorPositionContext context = {};

        ਲਈ (bool ਪਹਿਲਾ = true;; ਪਹਿਲਾ = false) {
                ਜੇ (buf_ਭਰਿਆ == 0) {
                        usec_t n = now(CLOCK_MONOTONIC);
                        ਜੇ (n >= ਸਿਰਾ)
                                ਵਾਪਸ -EOPNOTSUPP;

                        r = fd_wait_for_event(nonblock_ਨਿਵੇਸ਼_fd, POLLIN, usec_sub_unsigned(ਸਿਰਾ, n));
                        ਜੇ (r < 0)
                                ਵਾਪਸ r;
                        ਜੇ (r == 0)
                                ਵਾਪਸ -EOPNOTSUPP;

                        /* On the first try, read multiple characters, i.e. the shortest valid
                         * reply. Afterwards read byte-wise, since we don't want to read too much, and
                         * unnecessarily drop too many characters from the input queue. */
                        ssize_t l = ਪਠ(nonblock_ਨਿਵੇਸ਼_fd, buf, ਪਹਿਲਾ ? ਆਕਾਰ(buf) : 1);
                        ਜੇ (l < 0) {
                                ਜੇ (errno == EAGAIN)
                                        ਜਾਰੀ;

                                ਵਾਪਸ -errno;
                        }

                        ਪ੍ਰਤਿਜ੍ਞਾ((size_t) l <= ਆਕਾਰ(buf));
                        buf_ਭਰਿਆ = l;
                }

                size_t processed;
                r = ਜਾਂਚ_cursor_position_response(&context, buf, buf_ਭਰਿਆ, &processed);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;

                ਪ੍ਰਤਿਜ੍ਞਾ(processed <= buf_ਭਰਿਆ);
                buf_ਭਰਿਆ -= processed;
                memmove(buf, buf + processed, buf_ਭਰਿਆ);

                ਜੇ (r > 0) {
                        /* Superficial validity checks (no particular reason to check for < 4, it's
                         * just a way to look for unreasonably small values) */
                        ਜੇ (context.ਪੰਕਤੀ < 4 || context.ਕਾਲਮ < 4 || context.ਪੰਕਤੀ >= 32766 || context.ਕਾਲਮ >= 32766)
                                ਵਾਪਸ -ENODATA;

                        ਜੇ (ret_ਪੰਕਤੀਆਂ)
                                *ret_ਪੰਕਤੀਆਂ = context.ਪੰਕਤੀ;
                        ਜੇ (ret_columns)
                                *ret_columns = context.ਕਾਲਮ;

                        ਵਾਪਸ 0;
                }
        }
}

/* Common setup for ANSI terminal queries: validate the fds, open a non-blocking input fd, and configure
 * termios with echo and canonical mode disabled. Caller must restore termios and close the fd when done. */
ਸਥਿਰ ਪੂਰਨ terminal_prepare_query(
                ਪੂਰਨ ਨਿਵੇਸ਼_fd,
                ਪੂਰਨ output_fd,
                ਪੂਰਨ *ret_nonblock_fd,
                ਬਣਤਰ termios *ret_saved_termios) {

        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਨਿਵੇਸ਼_fd >= 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(output_fd >= 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret_nonblock_fd);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret_saved_termios);

        /* Use getenv_terminal_is_dumb() instead of terminal_is_dumb() here since we operate on an
         * explicitly passed fd, not on stdio. terminal_is_dumb() additionally checks on_tty() which
         * tests whether *stderr* is a tty — that's irrelevant when we're querying a directly opened
         * terminal such as /dev/console. */
        ਜੇ (getenv_terminal_is_dumb())
                ਵਾਪਸ -EOPNOTSUPP;

        r = terminal_verify_same(ਨਿਵੇਸ਼_fd, output_fd);
        ਜੇ (r < 0)
                ਵਾਪਸ log_debug_errno(r, "Called with distinct input/output fds: %m");

        /* Open a 2nd input fd, in non-blocking mode, so that we won't ever hang in read()
         * should someone else process the POLLIN. Do all subsequent operations on the new fd. */
        _cleanup_close_ ਪੂਰਨ nonblock_ਨਿਵੇਸ਼_fd = r = fd_reopen(ਨਿਵੇਸ਼_fd, O_RDONLY|O_CLOEXEC|O_NONBLOCK|O_NOCTTY);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (tcgetattr(nonblock_ਨਿਵੇਸ਼_fd, ret_saved_termios) < 0)
                ਵਾਪਸ log_debug_errno(errno, "Failed to get terminal settings: %m");

        ਬਣਤਰ termios new_termios = *ret_saved_termios;
        termios_disable_echo(&new_termios);

        ਜੇ (tcsetattr(nonblock_ਨਿਵੇਸ਼_fd, TCSANOW, &new_termios) < 0)
                ਵਾਪਸ log_debug_errno(errno, "Failed to set new terminal settings: %m");

        *ret_nonblock_fd = TAKE_FD(nonblock_ਨਿਵੇਸ਼_fd);
        ਵਾਪਸ 0;
}

/*
 * See https://terminalguide.namepad.de/seq/csi_st-18/,
 * https://invisible-island.net/xterm/ctlseqs/ctlseqs.html#h3-Functions-using-CSI-_-ordered-by-the-final-character_s_.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CSI18_Q  "\x1B[18t"               /* Report the size of the text area in characters */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CSI18_Rp "\x1B[8;"                /* Reply prefix */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CSI18_R0 CSI18_Rp "1;1t"          /* Shortest reply */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CSI18_R1 CSI18_Rp "32766;32766t"  /* Longest reply */

ਸਥਿਰ ਪੂਰਨ ਜਾਂਚ_ਪਾਠ_area_ਮਾਪ_response(
                ਅਚਲ ਅੱਖਰ *buf,
                size_t ਮਾਪ,
                ਬਿਨਾਨਿਸ਼ਾਨੀ *ret_ਪੰਕਤੀਆਂ,
                ਬਿਨਾਨਿਸ਼ਾਨੀ *ret_columns) {

        ਪ੍ਰਤਿਜ੍ਞਾ(buf);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret_ਪੰਕਤੀਆਂ);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret_columns);

        /* Check if we have enough space for the shortest possible answer. */
        ਜੇ (ਮਾਪ < STRLEN(CSI18_R0))
                ਵਾਪਸ -EAGAIN;

        /* Check if the terminating sequence is present */
        ਜੇ (buf[ਮਾਪ - 1] != 't')
                ਵਾਪਸ -EAGAIN;

        ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ ਪੰਕਤੀਆਂ, columns;
        ਜੇ (ਲੜੀ_ਤੋਂ_ਪੜ੍ਹੋ(buf, CSI18_Rp "%hu;%hut", &ਪੰਕਤੀਆਂ, &columns) != 2)
                ਵਾਪਸ -EINVAL;

        *ret_ਪੰਕਤੀਆਂ = ਪੰਕਤੀਆਂ;
        *ret_columns = columns;
        ਵਾਪਸ 0;
}

/* Determine terminal dimensions by means of an ANSI CSI 18 sequence.
 *
 * Caller must have already opened a non-blocking input fd and configured termios (echo/icanon off). */
ਸਥਿਰ ਪੂਰਨ terminal_query_ਮਾਪ_by_csi18(
                ਪੂਰਨ nonblock_ਨਿਵੇਸ਼_fd,
                ਪੂਰਨ output_fd,
                ਬਿਨਾਨਿਸ਼ਾਨੀ *ret_ਪੰਕਤੀਆਂ,
                ਬਿਨਾਨਿਸ਼ਾਨੀ *ret_columns) {

        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(nonblock_ਨਿਵੇਸ਼_fd >= 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(output_fd >= 0);

        r = loop_write_ਭਰਿਆ(output_fd, CSI18_Q, SIZE_MAX, CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        usec_t ਸਿਰਾ = usec_ਜੋੜੋ(now(CLOCK_MONOTONIC), CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        ਅੱਖਰ buf[STRLEN(CSI18_R1)];
        size_t ਬਾਈਟਾਂ = 0;

        ਲਈ (;;) {
                usec_t n = now(CLOCK_MONOTONIC);
                ਜੇ (n >= ਸਿਰਾ)
                        ਵਾਪਸ -EOPNOTSUPP;

                r = fd_wait_for_event(nonblock_ਨਿਵੇਸ਼_fd, POLLIN, usec_sub_unsigned(ਸਿਰਾ, n));
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
                ਜੇ (r == 0)
                        ਵਾਪਸ -EOPNOTSUPP;

                /* On the first read, read multiple characters, i.e. the shortest valid reply. Afterwards
                 * read byte by byte, since we don't want to read too much and drop characters from the input
                 * queue. */
                ssize_t l = ਪਠ(nonblock_ਨਿਵੇਸ਼_fd, buf + ਬਾਈਟਾਂ, ਬਾਈਟਾਂ == 0 ? STRLEN(CSI18_R0) : 1);
                ਜੇ (l < 0) {
                        ਜੇ (errno == EAGAIN)
                                ਜਾਰੀ;
                        ਵਾਪਸ -errno;
                }

                ਪ੍ਰਤਿਜ੍ਞਾ((size_t) l <= ਆਕਾਰ(buf) - ਬਾਈਟਾਂ);
                ਬਾਈਟਾਂ += l;

                r = ਜਾਂਚ_ਪਾਠ_area_ਮਾਪ_response(buf, ਬਾਈਟਾਂ, ret_ਪੰਕਤੀਆਂ, ret_columns);
                ਜੇ (r != -EAGAIN)
                        ਵਾਪਸ r;

                ਜੇ (ਬਾਈਟਾਂ == ਆਕਾਰ(buf))
                        ਵਾਪਸ -EOPNOTSUPP; /* The response has the right prefix, but we didn't find a valid
                                             * answer with a terminator in the allotted space. Something is
                                             * wrong, possibly some unrelated bytes got injected into the
                                             * answer. */
        }
}

ਪੂਰਨ terminal_get_ਮਾਪ(
                ਪੂਰਨ ਨਿਵੇਸ਼_fd,
                ਪੂਰਨ output_fd,
                ਬਿਨਾਨਿਸ਼ਾਨੀ *ret_ਪੰਕਤੀਆਂ,
                ਬਿਨਾਨਿਸ਼ਾਨੀ *ret_columns,
                bool try_dsr,
                bool try_csi18) {

        _cleanup_close_ ਪੂਰਨ nonblock_ਨਿਵੇਸ਼_fd = -EBADF;
        ਬਣਤਰ termios old_termios = TERMIOS_NULL;
        CLEANUP_TERMIOS_RESET(nonblock_ਨਿਵੇਸ਼_fd, old_termios);
        _cleanup_(nonblock_resetp) ਪੂਰਨ nonblock_reset = -EBADF;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(try_dsr || try_csi18);

        r = terminal_prepare_query(ਨਿਵੇਸ਼_fd, output_fd, &nonblock_ਨਿਵੇਸ਼_fd, &old_termios);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        /* Put the output fd in non-blocking mode with a write timeout, to avoid blocking indefinitely on
         * write if the terminal is not consuming data (e.g. serial console with flow control). */
        r = fd_nonblock(output_fd, true);
        ਜੇ (r < 0)
                ਵਾਪਸ log_debug_errno(r, "Failed to set terminal to non-blocking mode: %m");
        ਜੇ (r > 0)
                nonblock_reset = output_fd;

        /* Flush any stale input that might confuse the response parsers. */
        (ਖਾਲੀ) tcflush(nonblock_ਨਿਵੇਸ਼_fd, TCIFLUSH);

        ਜੇ (try_csi18) {
                r = terminal_query_ਮਾਪ_by_csi18(nonblock_ਨਿਵੇਸ਼_fd, output_fd, ret_ਪੰਕਤੀਆਂ, ret_columns);
                ਜੇ (r >= 0)
                        ਵਾਪਸ r;

                /* Query failed. Flush any outstanding input. */
                (ਖਾਲੀ) tcflush(nonblock_ਨਿਵੇਸ਼_fd, TCIFLUSH);

                ਜੇ (!ਵਿੱਚ_ਸੈੱਟ(r, -EOPNOTSUPP, -EINVAL))
                        ਵਾਪਸ r;
        }

        ਜੇ (try_dsr) {
                r = terminal_query_ਮਾਪ_by_dsr(nonblock_ਨਿਵੇਸ਼_fd, output_fd, ret_ਪੰਕਤੀਆਂ, ret_columns);
                ਜੇ (r >= 0)
                        ਵਾਪਸ r;

                /* Query failed. Flush any outstanding input. */
                (ਖਾਲੀ) tcflush(nonblock_ਨਿਵੇਸ਼_fd, TCIFLUSH);
        }

        ਵਾਪਸ r;
}

ਸਥਿਰ ਪੂਰਨ fd_is_vc(ਪੂਰਨ fd) {
        _cleanup_free_ ਅੱਖਰ *name = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);

        r = getttyname_harder(fd, &name);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ tty_is_vc_resolve(name);
}

ਪੂਰਨ terminal_fix_ਮਾਪ(ਪੂਰਨ ਨਿਵੇਸ਼_fd, ਪੂਰਨ output_fd) {
        ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੰਕਤੀਆਂ, columns;
        ਪੂਰਨ r;

        /* Don't ask a virtual console at all. There is no intermediary here that could be wrong about the
         * dimensions: the kernel renders the screen itself, so its winsize is authoritative. */
        r = fd_is_vc(ਨਿਵੇਸ਼_fd);
        ਜੇ (r < 0)
                log_debug_errno(r, "Failed to determine whether terminal is a virtual console, assuming it is not: %m");
        ਨਹੀਂਤਾਂ ਜੇ (r > 0) {
                log_debug("Not querying dimensions of a virtual console, kernel dimensions are authoritative.");
                ਵਾਪਸ 0;
        }

        ਬਣਤਰ winsize ws = {};
        ਜੇ (ioctl(output_fd, TIOCGWINSZ, &ws) < 0)
                ਵਾਪਸ log_debug_errno(errno, "Failed to query terminal dimensions, ignoring: %m");

        r = terminal_get_ਮਾਪ(ਨਿਵੇਸ਼_fd, output_fd, &ਪੰਕਤੀਆਂ, &columns, /* try_dsr= */ true, /* try_csi18= */ true);
        ਜੇ (r < 0)
                ਵਾਪਸ log_debug_errno(r, "Failed to acquire terminal dimensions via ANSI sequences, not adjusting terminal dimensions: %m");

        ਜੇ (ws.ws_row == ਪੰਕਤੀਆਂ && ws.ws_col == columns) {
                log_debug("Terminal dimensions reported via ANSI sequences match currently set terminal dimensions, not changing.");
                ਵਾਪਸ 0;
        }

        ws.ws_col = columns;
        ws.ws_row = ਪੰਕਤੀਆਂ;

        ਜੇ (ioctl(output_fd, TIOCSWINSZ, &ws) < 0)
                ਵਾਪਸ log_debug_errno(errno, "Failed to update terminal dimensions, ignoring: %m");

        log_debug("Fixed terminal dimensions to %ux%u based on ANSI sequence information.", columns, ਪੰਕਤੀਆਂ);
        ਵਾਪਸ 1;
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_TERMINFO_ਲੰਮਾਈ 64
/* python -c 'print("".join(hex(ord(i))[2:] for i in "name").upper())' */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DCS_TERMINFO_Q ANSI_DCS "+q" "6E616D65" ANSI_ST
/* The answer is either 0+r… (invalid) or 1+r… (OK). */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DCS_TERMINFO_R0 ANSI_DCS "0+r" ANSI_ST
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DCS_TERMINFO_R1 ANSI_DCS "1+r" "6E616D65" "=" /* This is followed by Pt ST. */
assert_cc(STRLEN(DCS_TERMINFO_R0) <= STRLEN(DCS_TERMINFO_R1 ANSI_ST));

ਸਥਿਰ ਪੂਰਨ ਜਾਂਚ_terminfo_response(
                ਅਚਲ ਅੱਖਰ *buf,
                size_t ਮਾਪ,
                ਅੱਖਰ **ret_name) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(buf);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret_name);

        /* Check if we have enough space for the shortest possible answer. */
        ਜੇ (ਮਾਪ < STRLEN(DCS_TERMINFO_R0))
                ਵਾਪਸ -EAGAIN;

        /* Check if the terminating sequence is present */
        ਜੇ (memcmp(buf + ਮਾਪ - STRLEN(ANSI_ST), ANSI_ST, STRLEN(ANSI_ST)) != 0)
                ਵਾਪਸ -EAGAIN;

        ਜੇ (ਮਾਪ <= STRLEN(DCS_TERMINFO_R1 ANSI_ST))
                ਵਾਪਸ -EINVAL;  /* The answer is invalid or empty */

        ਜੇ (memcmp(buf, DCS_TERMINFO_R1, STRLEN(DCS_TERMINFO_R1)) != 0)
                ਵਾਪਸ -EINVAL;  /* The answer is not valid */

        _cleanup_free_ ਖਾਲੀ *dec = ਨਿਰਾ;
        size_t dec_ਮਾਪ;
        r = unhexmem_ਭਰਿਆ(buf + STRLEN(DCS_TERMINFO_R1), ਮਾਪ - STRLEN(DCS_TERMINFO_R1 ANSI_ST),
                          /* secure= */ false,
                          &dec, &dec_ਮਾਪ);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(((ਅਚਲ ਅੱਖਰ *) dec)[dec_ਮਾਪ] == '\0'); /* unhexmem appends NUL for our convenience */
        ਜੇ (memchr(dec, '\0', dec_ਮਾਪ) || string_has_cc(dec, ਨਿਰਾ) || !filename_is_valid(dec))
                ਵਾਪਸ -EUCLEAN;

        *ret_name = TAKE_PTR(dec);
        ਵਾਪਸ 0;
}

ਪੂਰਨ terminal_get_terminfo_by_dcs(ਪੂਰਨ fd, ਅੱਖਰ **ret_name) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret_name);

        /* Note: fd must be in non-blocking read-write mode! */

        ਬਣਤਰ termios old_termios = TERMIOS_NULL;
        CLEANUP_TERMIOS_RESET(fd, old_termios);

        ਜੇ (tcgetattr(fd, &old_termios) < 0)
                ਵਾਪਸ -errno;

        ਬਣਤਰ termios new_termios = old_termios;
        termios_disable_echo(&new_termios);

        ਜੇ (tcsetattr(fd, TCSANOW, &new_termios) < 0)
                ਵਾਪਸ -errno;

        r = loop_write(fd, DCS_TERMINFO_Q, SIZE_MAX);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        usec_t ਸਿਰਾ = usec_ਜੋੜੋ(now(CLOCK_MONOTONIC), CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        ਅੱਖਰ buf[STRLEN(DCS_TERMINFO_R1) + MAX_TERMINFO_ਲੰਮਾਈ + STRLEN(ANSI_ST)];
        size_t ਬਾਈਟਾਂ = 0;

        ਲਈ (;;) {
                usec_t n = now(CLOCK_MONOTONIC);
                ਜੇ (n >= ਸਿਰਾ)
                        ਵਾਪਸ -EOPNOTSUPP;

                r = fd_wait_for_event(fd, POLLIN, usec_sub_unsigned(ਸਿਰਾ, n));
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
                ਜੇ (r == 0)
                        ਵਾਪਸ -EOPNOTSUPP;

                /* On the first read, read multiple characters, i.e. the shortest valid reply. Afterwards
                 * read byte by byte, since we don't want to read too much and drop characters from the input
                 * queue. */
                ssize_t l = ਪਠ(fd, buf + ਬਾਈਟਾਂ, ਬਾਈਟਾਂ == 0 ? STRLEN(DCS_TERMINFO_R0) : 1);
                ਜੇ (l < 0) {
                        ਜੇ (errno == EAGAIN)
                                ਜਾਰੀ;
                        ਵਾਪਸ -errno;
                }

                ਪ੍ਰਤਿਜ੍ਞਾ((size_t) l <= ਆਕਾਰ(buf) - ਬਾਈਟਾਂ);
                ਬਾਈਟਾਂ += l;

                r = ਜਾਂਚ_terminfo_response(buf, ਬਾਈਟਾਂ, ret_name);
                ਜੇ (r != -EAGAIN)
                        ਵਾਪਸ r;

                ਜੇ (ਬਾਈਟਾਂ == ਆਕਾਰ(buf))
                        ਵਾਪਸ -EOPNOTSUPP; /* The response has the right prefix, but we didn't find a valid
                                             * answer with a terminator in the allotted space. Something is
                                             * wrong, possibly some unrelated bytes got injected into the
                                             * answer. */
        }
}

ਪੂਰਨ have_terminfo_file(ਅਚਲ ਅੱਖਰ *name) {
        /* This is a heuristic check if we have the file, using the directory layout used on
         * current Linux systems. Checks for other layouts can be added later if appropriate. */
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(filename_is_valid(name));

        _cleanup_free_ ਅੱਖਰ *p = ਰਸਤਾ_join("/usr/share/terminfo", CHAR_ਮੰਜ਼ਿਲ_STR(name[0]), name);
        ਜੇ (!p)
                ਵਾਪਸ log_oom_debug();

        r = RET_NERRNO(access(p, F_OK));
        ਜੇ (r == -ENOENT)
                ਵਾਪਸ false;
        ਜੇ (r < 0)
                ਵਾਪਸ r;
        ਵਾਪਸ true;
}

ਪੂਰਨ query_ਪਦ_for_tty(ਅਚਲ ਅੱਖਰ *tty, ਅੱਖਰ **ret_ਪਦ) {
        _cleanup_free_ ਅੱਖਰ *dcs_ਪਦ = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(tty);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret_ਪਦ);

        ਜੇ (tty_is_vc_resolve(tty))
                ਵਾਪਸ strdup_ਮੰਜ਼ਿਲ(ret_ਪਦ, "linux");

        /* Try to query the terminal implementation that we're on. This will not work in all
         * cases, which is fine, since this is intended to be used as a fallback. */

        _cleanup_close_ ਪੂਰਨ tty_fd = open_terminal(tty, O_RDWR|O_NOCTTY|O_CLOEXEC|O_NONBLOCK);
        ਜੇ (tty_fd < 0)
                ਵਾਪਸ log_debug_errno(tty_fd, "Failed to open %s to query terminfo: %m", tty);

        r = terminal_get_terminfo_by_dcs(tty_fd, &dcs_ਪਦ);
        ਜੇ (r < 0)
                ਵਾਪਸ log_debug_errno(r, "Failed to query %s for terminfo: %m", tty);

        r = have_terminfo_file(dcs_ਪਦ);
        ਜੇ (r < 0)
                ਵਾਪਸ log_debug_errno(r, "Failed to look for terminfo %s: %m", dcs_ਪਦ);
        ਜੇ (r == 0)
                ਵਾਪਸ log_info_errno(SYNTHETIC_ERRNO(ENODATA),
                                      "Terminfo %s not found for %s.", dcs_ਪਦ, tty);

        *ret_ਪਦ = TAKE_PTR(dcs_ਪਦ);
        ਵਾਪਸ 0;
}

ਪੂਰਨ terminal_is_pty_fd(ਪੂਰਨ fd) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);

        /* Returns true if we are looking at a pty, i.e. if it's backed by the /dev/pts/ file system */

        ਜੇ (!isatty_ਸੁਰੱਖਿਅਤ(fd))
                ਵਾਪਸ false;

        r = is_fs_type_at(fd, ਨਿਰਾ, DEVPTS_SUPER_MAGIC);
        ਜੇ (r != 0)
                ਵਾਪਸ r;

        /* The ptmx device is weird, it exists twice, once inside and once outside devpts. To detect the
         * latter case, let's fire off an ioctl() that only works on ptmx devices. */

        ਪੂਰਨ v;
        ਜੇ (ioctl(fd, TIOCGPKT, &v) < 0) {
                ਜੇ (ERRNO_IS_NOT_SUPPORTED(errno))
                        ਵਾਪਸ false;

                ਵਾਪਸ -errno;
        }

        ਵਾਪਸ true;
}

ਪੂਰਨ pty_open_peer(ਪੂਰਨ fd, ਪੂਰਨ ਬਹੁਲਕ) {
        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);

        /* Opens the peer PTY using the new race-free TIOCGPTPEER ioctl() (kernel 4.13).
         *
         * This is safe to be called on TTYs from other namespaces. */

        ਪ੍ਰਤਿਜ੍ਞਾ((ਬਹੁਲਕ & (O_CREAT|O_ਰਸਤਾ|O_DIRECTORY|O_TMPFILE)) == 0);

        /* This replicates the EIO retry logic of open_terminal() in a modified way. */
        ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ c = 0;; c++) {
                ਪੂਰਨ peer_fd = ioctl(fd, TIOCGPTPEER, ਬਹੁਲਕ);
                ਜੇ (peer_fd >= 0)
                        ਵਾਪਸ peer_fd;

                ਜੇ (errno != EIO)
                        ਵਾਪਸ -errno;

                /* Max 1s in total */
                ਜੇ (c >= 20)
                        ਵਾਪਸ -EIO;

                (ਖਾਲੀ) usleep_ਸੁਰੱਖਿਅਤ(50 * USEC_ਪ੍ਰਤੀ_MSEC);
        }
}
