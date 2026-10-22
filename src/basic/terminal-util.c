/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ <fcntl.h>
#అన్తర్భావ <linux/kd.h>
#అన్తర్భావ <linux/magic.h>
#అన్తర్భావ <linux/tiocl.h>
#అన్తర్భావ <linux/vt.h>
#అన్తర్భావ <poll.h>
#అన్తర్భావ <signal.h>
#అన్తర్భావ <stdlib.h>
#అన్తర్భావ <sys/inotify.h>
#అన్తర్భావ <sys/ioctl.h>
#అన్తర్భావ <sys/sysmacros.h>
#అన్తర్భావ <termios.h>
#అన్తర్భావ <time.h>
#అన్తర్భావ <unistd.h>

#అన్తర్భావ "alloc-util.h"
#అన్తర్భావ "ansi-color.h"
#అన్తర్భావ "chase.h"
#అన్తర్భావ "devnum-util.h"
#అన్తర్భావ "errno-util.h"
#అన్తర్భావ "extract-word.h"
#అన్తర్భావ "fd-util.h"
#అన్తర్భావ "fileio.h"
#అన్తర్భావ "fs-util.h"
#అన్తర్భావ "hexdecoct.h"
#అన్తర్భావ "inotify-util.h"
#అన్తర్భావ "io-util.h"
#అన్తర్భావ "log.h"
#అన్తర్భావ "namespace-util.h"
#అన్తర్భావ "parse-util.h"
#అన్తర్భావ "path-util.h"
#అన్తర్భావ "pidref.h"
#అన్తర్భావ "proc-cmdline.h"
#అన్తర్భావ "process-util.h"
#అన్తర్భావ "signal-util.h"
#అన్తర్భావ "socket-util.h"
#అన్తర్భావ "stat-util.h"
#అన్తర్భావ "stdio-util.h"
#అన్తర్భావ "string-util.h"
#అన్తర్భావ "strv.h"
#అన్తర్భావ "terminal-util.h"
#అన్తర్భావ "time-util.h"
#అన్తర్భావ "utf8.h"

/* How much to wait when reading/writing ANSI sequences from/to the console */
#నిర్వచించు CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC (333 * USEC_చొప్పున_MSEC)

స్థిర అస్థిర గుర్తులేని cached_columns = 0;
స్థిర అస్థిర గుర్తులేని cached_పంక్తులు = 0;

స్థిర అస్థిర పూర్ణాంకం cached_on_tty = -1;
స్థిర అస్థిర పూర్ణాంకం cached_on_dev_null = -1;

bool isatty_సురక్షిత(పూర్ణాంకం fd) {
        ప్రతిజ్ఞా(fd >= 0);

        ఒకవేళ (isatty(fd))
                తిరిగి true;

        /* Linux/glibc returns EIO for hung up TTY on isatty(). Which is wrong, the thing doesn't stop being
         * a TTY after all, just because it is temporarily hung up. Let's work around this here, until this
         * is fixed in glibc. See: https://sourceware.org/bugzilla/show_bug.cgi?id=32103 */
        ఒకవేళ (errno == EIO)
                తిరిగి true;

        /* Be resilient if we're working on stdio, since they're set up by parent process. */
        ప్రతిజ్ఞా(errno != EBADF || లోపల_సమితి(fd, STDIN_FILENO, STDOUT_FILENO, STDERR_FILENO));

        తిరిగి false;
}

పూర్ణాంకం chvt(పూర్ణాంకం vt) {
        _cleanup_close_ పూర్ణాంకం fd = -EBADF;

        /* Switch to the specified vt number. If the VT is specified <= 0 switch to the VT the kernel log messages go,
         * if that's configured. */

        fd = open_terminal("/dev/tty0", O_RDWR|O_NOCTTY|O_CLOEXEC|O_NONBLOCK);
        ఒకవేళ (fd < 0)
                తిరిగి fd;

        ఒకవేళ (vt <= 0) {
                పూర్ణాంకం tiocl[2] = {
                        TIOCL_GETKMSGREDIRECT,
                        0
                };

                ఒకవేళ (ioctl(fd, TIOCLINUX, tiocl) < 0)
                        తిరిగి -errno;

                vt = tiocl[0] <= 0 ? 1 : tiocl[0];
        }

        తిరిగి RET_NERRNO(ioctl(fd, VT_ACTIVATE, vt));
}

పూర్ణాంకం read_one_char(FILE *f, అక్షరం *ret, usec_t t, bool echo, bool *need_nl) {
        _cleanup_free_ అక్షరం *పంక్తి = ఖాళీ;
        నిర్మాణం termios old_termios;
        పూర్ణాంకం r, fd;

        ప్రతిజ్ఞా(ret);

        ఒకవేళ (!f)
                f = stdin;

        /* If this is a terminal, then switch canonical mode off, so that we can read a single
         * character. (Note that fmemopen() streams do not have an fd associated with them, let's handle that
         * nicely.) If 'echo' is false we'll also disable ECHO mode so that the pressed key is not made
         * visible to the user. */
        fd = fileno(f);
        ఒకవేళ (fd >= 0 && tcgetattr(fd, &old_termios) >= 0) {
                నిర్మాణం termios new_termios = old_termios;

                new_termios.c_lflag &= ~(ICANON|(echo ? 0 : ECHO));
                new_termios.c_cc[VMIN] = 1;
                new_termios.c_cc[VTIME] = 0;

                ఒకవేళ (tcsetattr(fd, TCSANOW, &new_termios) >= 0) {
                        అక్షరం c;

                        ఒకవేళ (t != USEC_INFINITY) {
                                ఒకవేళ (fd_wait_for_event(fd, POLLIN, t) <= 0) {
                                        (శూన్య) tcsetattr(fd, TCSANOW, &old_termios);
                                        తిరిగి -ETIMEDOUT;
                                }
                        }

                        r = సురక్షిత_fgetc(f, &c);
                        (శూన్య) tcsetattr(fd, TCSANOW, &old_termios);
                        ఒకవేళ (r < 0)
                                తిరిగి r;
                        ఒకవేళ (r == 0)
                                తిరిగి -EIO;

                        ఒకవేళ (need_nl)
                                *need_nl = c != '\n';

                        *ret = c;
                        తిరిగి 0;
                }
        }

        ఒకవేళ (t != USEC_INFINITY && fd >= 0) {
                /* Let's wait the specified amount of time for input. When we have no fd we skip this, under
                 * the assumption that this is an fmemopen() stream or so where waiting doesn't make sense
                 * anyway, as the data is either already in the stream or cannot possible be placed there
                 * while we access the stream */

                ఒకవేళ (fd_wait_for_event(fd, POLLIN, t) <= 0)
                        తిరిగి -ETIMEDOUT;
        }

        /* If this is not a terminal, then read a full line instead */

        r = read_పంక్తి(f, 16, &పంక్తి); /* longer than necessary, to eat up UTF-8 chars/vt100 key sequences */
        ఒకవేళ (r < 0)
                తిరిగి r;
        ఒకవేళ (r == 0)
                తిరిగి -EIO;

        ఒకవేళ (అక్షరశ్రేణి_పొడవు(పంక్తి) != 1)
                తిరిగి -EBADMSG;

        ఒకవేళ (need_nl)
                *need_nl = false;

        *ret = పంక్తి[0];
        తిరిగి 0;
}

#నిర్వచించు DEFAULT_ASK_REFRESH_USEC (2*USEC_చొప్పున_SEC)

పూర్ణాంకం ask_char(అక్షరం *ret, స్థిరరాశి అక్షరం *replies, స్థిరరాశి అక్షరం *fmt, ...) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(ret);
        ప్రతిజ్ఞా(replies);
        ప్రతిజ్ఞా(fmt);

        కొరకు (;;) {
                va_list ap;
                అక్షరం c;
                bool need_nl = true;

                fputs(ansi_highlight(), stdout);

                అక్షరం_రాయి('\r');

                va_start(ap, fmt);
                vprintf(fmt, ap);
                va_end(ap);

                fputs(ansi_సాధారణ(), stdout);

                fflush(stdout);

                r = read_one_char(stdin, &c, DEFAULT_ASK_REFRESH_USEC, /* echo= */ true, &need_nl);
                ఒకవేళ (r < 0) {

                        ఒకవేళ (r == -ETIMEDOUT)
                                కొనసాగు;

                        ఒకవేళ (r == -EBADMSG) {
                                puts("Bad input, please try again.");
                                కొనసాగు;
                        }

                        అక్షరం_రాయి('\n');
                        తిరిగి r;
                }

                ఒకవేళ (need_nl)
                        అక్షరం_రాయి('\n');

                ఒకవేళ (strchr(replies, c)) {
                        *ret = c;
                        తిరిగి 0;
                }

                puts("Read unexpected character, please try again.");
        }
}

రకనిర్వచనం గణన Completion_ఫలితం{
        COMPLETION_ALREADY,       /* the input string is already complete */
        COMPLETION_నిండిన,          /* completed the input string to be complete now */
        COMPLETION_PARTIAL,       /* completed the input string so that is still incomplete */
        COMPLETION_NONE,          /* found no matching completion */
        COMPLETION_ఫలితం_MAX,
        COMPLETION_ఫలితం_INVALID = -EINVAL,
        COMPLETION_ఫలితం_ERRNO_MAX = -ERRNO_MAX,
} Completion_ఫలితం;

స్థిర Completion_ఫలితం pick_completion(స్థిరరాశి అక్షరం *string, అక్షరం *స్థిరరాశి*completions, అక్షరం **ret) {
        _cleanup_free_ అక్షరం *దొరికింది = ఖాళీ;
        bool partial = false;

        ప్రతిజ్ఞా(ret);

        string = strempty(string);

        STRV_FOREACH(c, completions) {

                /* Ignore entries that are not actually completions */
                ఒకవేళ (!startswith(*c, string))
                        కొనసాగు;

                /* Store first completion that matches */
                ఒకవేళ (!దొరికింది) {
                        దొరికింది = strdup(*c);
                        ఒకవేళ (!దొరికింది)
                                తిరిగి -ENOMEM;

                        కొనసాగు;
                }

                /* If there's another completion that works truncate the one we already found by common
                 * prefix */
                size_t n = str_common_ఉపసర్గ(దొరికింది, *c);
                ఒకవేళ (n == SIZE_MAX)
                        కొనసాగు;

                దొరికింది[n] = 0;
                partial = true;
        }

        *ret = TAKE_PTR(దొరికింది);

        ఒకవేళ (!*ret)
                తిరిగి COMPLETION_NONE;
        ఒకవేళ (partial)
                తిరిగి COMPLETION_PARTIAL;

        తిరిగి streq(string, *ret) ? COMPLETION_ALREADY : COMPLETION_నిండిన;
}

స్థిర శూన్య clear_by_backspace(size_t n) {
        /* Erase the specified number of character cells backwards on the terminal */
        కొరకు (size_t i = 0; i < n; i++)
                fputs("\b \b", stdout);
}

పూర్ణాంకం ask_string_నిండిన(
                అక్షరం **ret,
                స్థిరరాశి అక్షరం *prefill,
                GetCompletionsCallback get_completions,
                శూన్య *userdata,
                స్థిరరాశి అక్షరం *పాఠ్యం, ...) {

        va_list ap;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(ret);
        ప్రతిజ్ఞా(పాఠ్యం);

        _cleanup_free_ అక్షరం *string = ఖాళీ;
        size_t n = 0;

        ఒకవేళ (prefill) {
                /* Prefill query with explicit data if specified */

                string = strdup(prefill);
                ఒకవేళ (!string)
                        తిరిగి -ENOMEM;

                n = అక్షరశ్రేణి_పొడవు(string);

        } లేకపోతే ఒకవేళ (get_completions) {
                /* Otherwise, figure out what string to preselect the query with */
                _cleanup_strv_free_ అక్షరం **completions = ఖాళీ;
                r = get_completions("", GET_COMPLETIONS_PRESELECT, &completions, userdata);
                ఒకవేళ (r < 0)
                        తిరిగి r;

                Completion_ఫలితం cr = pick_completion(string, completions, &string);
                ఒకవేళ (cr < 0)
                        తిరిగి cr;

                n = strlen_ptr(string);
        }

        /* Output the prompt */
        fputs(ansi_highlight(), stdout);
        va_start(ap, పాఠ్యం);
        vprintf(పాఠ్యం, ap);
        va_end(ap);
        fputs(ansi_సాధారణ(), stdout);
        ఒకవేళ (string)
                fputs(string, stdout);
        fflush(stdout);

        /* Do interactive logic only if stdin + stdout are connected to the same place. And yes, we could use
         * STDIN_FILENO and STDOUT_FILENO here, but let's be overly correct for once, after all libc allows
         * swapping out stdin/stdout. */
        పూర్ణాంకం fd_ఆదానం = fileno(stdin);
        పూర్ణాంకం fd_output = fileno(stdout);
        నిర్మాణం termios old_termios = TERMIOS_NULL;
        CLEANUP_TERMIOS_RESET(fd_ఆదానం, old_termios);

        ఒకవేళ (fd_ఆదానం < 0 || fd_output < 0 || same_fd(fd_ఆదానం, fd_output) <= 0)
                వెళ్ళు fallback;

        /* Try to disable echo, which also tells us if this even is a terminal */
        ఒకవేళ (tcgetattr(fd_ఆదానం, &old_termios) < 0) {
                old_termios = TERMIOS_NULL;
                వెళ్ళు fallback;
        }

        నిర్మాణం termios new_termios = old_termios;
        termios_disable_echo(&new_termios);
        ఒకవేళ (tcsetattr(fd_ఆదానం, TCSANOW, &new_termios) < 0)
                తిరిగి -errno;

        కొరకు (;;) {
                పూర్ణాంకం c = fgetc(stdin);

                /* On EOF or NUL, end the request, don't output anything anymore */
                ఒకవేళ (లోపల_సమితి(c, EOF, 0))
                        ఆపు;

                /* On Return also end the request, but make this visible */
                ఒకవేళ (లోపల_సమితి(c, '\n', '\r')) {
                        fputc('\n', stdout);
                        ఆపు;
                }

                ఒకవేళ (c == '\t') {
                        /* Tab */

                        _cleanup_strv_free_ అక్షరం **completions = ఖాళీ;
                        ఒకవేళ (get_completions) {
                                r = get_completions(string, /* flags= */ 0, &completions, userdata);
                                ఒకవేళ (r < 0)
                                        తిరిగి r;
                        }

                        _cleanup_free_ అక్షరం *new_string = ఖాళీ;
                        Completion_ఫలితం cr = pick_completion(string, completions, &new_string);
                        ఒకవేళ (cr < 0)
                                తిరిగి cr;
                        ఒకవేళ (లోపల_సమితి(cr, COMPLETION_PARTIAL, COMPLETION_నిండిన)) {
                                /* Output the new suffix we learned */
                                fputs(ASSERT_PTR(startswith(new_string, strempty(string))), stdout);

                                /* And update the whole string */
                                free_and_replace(string, new_string);
                                n = అక్షరశ్రేణి_పొడవు(string);
                        }
                        ఒకవేళ (cr == COMPLETION_NONE)
                                fputc('\a', stdout); /* BEL */

                        ఒకవేళ (లోపల_సమితి(cr, COMPLETION_PARTIAL, COMPLETION_ALREADY)) {
                                /* If this worked only partially, or if the user hit TAB even though we were
                                 * complete already, then show the remaining options (in the latter case just
                                 * the one). */
                                fputc('\n', stdout);

                                _cleanup_strv_free_ అక్షరం **filtered = strv_filter_ఉపసర్గ(completions, string);
                                ఒకవేళ (!filtered)
                                        తిరిగి -ENOMEM;

                                r = show_menu(filtered,
                                              /* n_columns= */ SIZE_MAX,
                                              /* column_width= */ SIZE_MAX,
                                              /* ellipsize_percentage= */ 0,
                                              /* grey_prefix= */ string,
                                              /* with_numbers= */ false);
                                ఒకవేళ (r < 0)
                                        తిరిగి r;

                                /* Show the prompt again */
                                fputs(ansi_highlight(), stdout);
                                va_start(ap, పాఠ్యం);
                                vprintf(పాఠ్యం, ap);
                                va_end(ap);
                                fputs(ansi_సాధారణ(), stdout);
                                fputs(string, stdout);
                        }

                } లేకపోతే ఒకవేళ (లోపల_సమితి(c, '\b', 127)) {
                        /* Backspace */

                        ఒకవేళ (n == 0)
                                fputc('\a', stdout); /* BEL */
                        లేకపోతే {
                                size_t m = utf8_చివరి_నిడివి(string, n);

                                అక్షరం *e = string + n - m;
                                clear_by_backspace(utf8_console_వెడల్పు(e));

                                *e = 0;
                                n -= m;
                        }

                } లేకపోతే ఒకవేళ (c == 21) {
                        /* Ctrl-u → erase all input */

                        clear_by_backspace(utf8_console_వెడల్పు(string));
                        ఒకవేళ (string)
                                string[n = 0] = 0;
                        లేకపోతే
                                ప్రతిజ్ఞా(n == 0);

                } లేకపోతే ఒకవేళ (c == 4) {
                        /* Ctrl-d → cancel this field input */

                        తిరిగి -ECANCELED;

                } లేకపోతే ఒకవేళ (char_is_cc(c) || n >= LINE_MAX)
                        /* refuse control characters and too long strings */
                        fputc('\a', stdout); /* BEL */
                లేకపోతే {
                        /* Regular char */

                        ఒకవేళ (!GREEDY_REALLOC(string, n+2))
                                తిరిగి -ENOMEM;

                        string[n++] = (అక్షరం) c;
                        string[n] = 0;

                        fputc(c, stdout);
                }

                fflush(stdout);
        }

        ఒకవేళ (!string) {
                string = strdup("");
                ఒకవేళ (!string)
                        తిరిగి -ENOMEM;
        }

        *ret = TAKE_PTR(string);
        తిరిగి 0;

fallback:
        /* A simple fallback without TTY magic */
        string = mfree(string);
        r = read_పంక్తి(stdin, LONG_పంక్తి_MAX, &string);
        ఒకవేళ (r < 0)
                తిరిగి r;
        ఒకవేళ (r == 0)
                తిరిగి -EIO;

        *ret = TAKE_PTR(string);
        తిరిగి 0;
}

bool any_కీలకం_గమ్యం_proceed(శూన్య) {

        /* Insert a new line here as well as to when the user inputs, as this is also used during the boot up
         * sequence when status messages may be interleaved with the current program output. This ensures
         * that the status messages aren't appended on the same line as this message. */

        fputc('\n', stdout);
        fputs(ansi_highlight_magenta(), stdout);
        fputs("-- Press any key to proceed --", stdout);
        fputs(ansi_సాధారణ(), stdout);
        fputc('\n', stdout);
        fflush(stdout);

        అక్షరం కీలకం = 0;
        (శూన్య) read_one_char(stdin, &కీలకం, USEC_INFINITY, /* echo= */ false, /* need_nl= */ ఖాళీ);

        fputc('\n', stdout);
        fflush(stdout);

        తిరిగి కీలకం != 'q';
}

స్థిర size_t widest_list_element(అక్షరం *స్థిరరాశి*l) {
        size_t w = 0;

        /* Returns the largest console width of all elements in 'l' */

        STRV_FOREACH(i, l)
                w = MAX(w, utf8_console_వెడల్పు(*i));

        తిరిగి w;
}

పూర్ణాంకం show_menu(అక్షరం **x,
              size_t n_columns,
              size_t స్తంభం_వెడల్పు,
              గుర్తులేని ellipsize_percentage,
              స్థిరరాశి అక్షరం *grey_ఉపసర్గ,
              bool with_numbers) {

        ప్రతిజ్ఞా(n_columns > 0);
        ప్రతిజ్ఞా(స్తంభం_వెడల్పు > 0);

        ఒకవేళ (n_columns == SIZE_MAX)
                n_columns = 3;

        ఒకవేళ (స్తంభం_వెడల్పు == SIZE_MAX) {
                size_t widest = widest_list_element(x);

                /* If not specified, derive column width from screen width */
                size_t స్తంభం_max = (columns()-1) / n_columns;

                /* Subtract room for numbers */
                ఒకవేళ (with_numbers)
                        స్తంభం_max = LESS_BY(స్తంభం_max, 6U);

                /* If columns would get too tight let's make this a linear list instead. */
                ఒకవేళ (స్తంభం_max < 10 && widest > 10) {
                        n_columns = 1;
                        స్తంభం_max = columns()-1;

                        ఒకవేళ (with_numbers)
                                స్తంభం_max = LESS_BY(స్తంభం_max, 6U);
                }

                /* Never make this narrower than 10 characters */
                స్తంభం_max = MAX(స్తంభం_max, 10U);
                స్తంభం_వెడల్పు = CLAMP(widest+1, 10U, స్తంభం_max);
        }

        size_t n = strv_నిడివి(x);
        size_t చొప్పున_స్తంభం = DIV_విడత_పైకి(n, n_columns);

        size_t break_పంక్తులు = పంక్తులు();
        ఒకవేళ (break_పంక్తులు > 2)
                break_పంక్తులు--;

        /* The first page gets two extra lines, since we want to show a title */
        size_t break_modulo = break_పంక్తులు;
        ఒకవేళ (break_modulo > 3)
                break_modulo -= 3;

        కొరకు (size_t i = 0; i < చొప్పున_స్తంభం; i++) {

                కొరకు (size_t j = 0; j < n_columns; j++) {
                        _cleanup_free_ అక్షరం *e = ఖాళీ;

                        size_t p = j * చొప్పున_స్తంభం + i;
                        ఒకవేళ (p >= n)
                                ఆపు;

                        e = ellipsize(x[p], స్తంభం_వెడల్పు, ellipsize_percentage);
                        ఒకవేళ (!e)
                                తిరిగి -ENOMEM;

                        ఒకవేళ (with_numbers)
                                రాయి("%s%4zu)%s ",
                                       ansi_grey(),
                                       p + 1,
                                       ansi_సాధారణ());

                        ఒకవేళ (grey_ఉపసర్గ && startswith(e, grey_ఉపసర్గ)) {
                                size_t k = MIN(అక్షరశ్రేణి_పొడవు(grey_ఉపసర్గ), స్తంభం_వెడల్పు);
                                రాయి("%s%.*s%s",
                                       ansi_grey(),
                                       (పూర్ణాంకం) k, e,
                                       ansi_సాధారణ());
                                రాయి("%-*s",
                                       (పూర్ణాంకం) (స్తంభం_వెడల్పు - k), e+k);
                        } లేకపోతే
                                రాయి("%-*s", (పూర్ణాంకం) స్తంభం_వెడల్పు, e);
                }

                అక్షరం_రాయి('\n');

                /* on the first screen we reserve 2 extra lines for the title */
                ఒకవేళ (i % break_పంక్తులు == break_modulo)
                        ఒకవేళ (!any_కీలకం_గమ్యం_proceed())
                                తిరిగి 0;
        }

        తిరిగి 0;
}

పూర్ణాంకం open_terminal(స్థిరరాశి అక్షరం *name, పూర్ణాంకం బహుళకం) {
        _cleanup_close_ పూర్ణాంకం fd = -EBADF;

        /*
         * If a TTY is in the process of being closed opening it might cause EIO. This is horribly awful, but
         * unlikely to be changed in the kernel. Hence we work around this problem by retrying a couple of
         * times.
         *
         * https://bugs.launchpad.net/ubuntu/+source/linux/+bug/554172/comments/245
         */

        ప్రతిజ్ఞా((బహుళకం & (O_CREAT|O_మార్గం|O_DIRECTORY|O_TMPFILE)) == 0);

        కొరకు (గుర్తులేని c = 0;; c++) {
                fd = open(name, బహుళకం, 0);
                ఒకవేళ (fd >= 0)
                        ఆపు;

                ఒకవేళ (errno != EIO)
                        తిరిగి -errno;

                /* Max 1s in total */
                ఒకవేళ (c >= 20)
                        తిరిగి -EIO;

                (శూన్య) usleep_సురక్షిత(50 * USEC_చొప్పున_MSEC);
        }

        ఒకవేళ (!isatty_సురక్షిత(fd))
                తిరిగి -ENOTTY;

        తిరిగి TAKE_FD(fd);
}

పూర్ణాంకం acquire_terminal(
                స్థిరరాశి అక్షరం *name,
                AcquireTerminalFlags flags,
                usec_t timeout) {

        _cleanup_close_ పూర్ణాంకం notify = -EBADF, fd = -EBADF;
        usec_t ts = USEC_INFINITY;
        పూర్ణాంకం r, wd = -1;

        ప్రతిజ్ఞా(name);

        AcquireTerminalFlags బహుళకం = flags & ACQUIRE_TERMINAL_బహుళకం_ముసుగు;
        ప్రతిజ్ఞా(లోపల_సమితి(బహుళకం, ACQUIRE_TERMINAL_TRY, ACQUIRE_TERMINAL_FORCE, ACQUIRE_TERMINAL_WAIT));
        ప్రతిజ్ఞా(బహుళకం == ACQUIRE_TERMINAL_WAIT || !FLAGS_సమితి(flags, ACQUIRE_TERMINAL_WATCH_SIGTERM));

        /* We use inotify to be notified when the tty is closed. We create the watch before checking if we can actually
         * acquire it, so that we don't lose any event.
         *
         * Note: strictly speaking this actually watches for the device being closed, it does *not* really watch
         * whether a tty loses its controlling process. However, unless some rogue process uses TIOCNOTTY on /dev/tty
         * *after* closing its tty otherwise this will not become a problem. As long as the administrator makes sure to
         * not configure any service on the same tty as an untrusted user this should not be a problem. (Which they
         * probably should not do anyway.) */

        ఒకవేళ (బహుళకం == ACQUIRE_TERMINAL_WAIT) {
                notify = inotify_init1(IN_CLOEXEC | IN_NONBLOCK);
                ఒకవేళ (notify < 0)
                        తిరిగి -errno;

                wd = inotify_add_watch(notify, name, IN_CLOSE);
                ఒకవేళ (wd < 0)
                        తిరిగి -errno;

                ఒకవేళ (timeout != USEC_INFINITY)
                        ts = now(CLOCK_MONOTONIC);
        }

        /* If we are called with ACQUIRE_TERMINAL_WATCH_SIGTERM we'll unblock SIGTERM during ppoll() temporarily */
        sigset_t poll_ss;
        assert_se(sigprocmask(SIG_SETMASK, /* newset= */ ఖాళీ, &poll_ss) >= 0);
        ఒకవేళ (flags & ACQUIRE_TERMINAL_WATCH_SIGTERM) {
                assert_se(sigismember(&poll_ss, SIGTERM) > 0);
                assert_se(sigdelset(&poll_ss, SIGTERM) >= 0);
        }

        కొరకు (;;) {
                ఒకవేళ (notify >= 0) {
                        r = flush_fd(notify);
                        ఒకవేళ (r < 0)
                                తిరిగి r;
                }

                /* We pass here O_NOCTTY only so that we can check the return value TIOCSCTTY and have a reliable way
                 * to figure out if we successfully became the controlling process of the tty */
                fd = open_terminal(name, O_RDWR|O_NOCTTY|O_CLOEXEC);
                ఒకవేళ (fd < 0)
                        తిరిగి fd;

                /* Temporarily ignore SIGHUP, so that we don't get SIGHUP'ed if we already own the tty. */
                నిర్మాణం sigaction sa_old;
                assert_se(sigaction(SIGHUP, &sigaction_ignore, &sa_old) >= 0);

                /* First, try to get the tty */
                r = RET_NERRNO(ioctl(fd, TIOCSCTTY, బహుళకం == ACQUIRE_TERMINAL_FORCE));

                /* Reset signal handler to old value */
                assert_se(sigaction(SIGHUP, &sa_old, ఖాళీ) >= 0);

                /* Success? Exit the loop now! */
                ఒకవేళ (r >= 0)
                        ఆపు;

                /* Any failure besides -EPERM? Fail, regardless of the mode. */
                ఒకవేళ (r != -EPERM)
                        తిరిగి r;

                ఒకవేళ (flags & ACQUIRE_TERMINAL_PERMISSIVE) /* If we are in permissive mode, then EPERM is fine, turn this
                                                          * into a success. Note that EPERM is also returned if we
                                                          * already are the owner of the TTY. */
                        ఆపు;

                ఒకవేళ (బహుళకం != ACQUIRE_TERMINAL_WAIT) /* If we are in TRY or FORCE mode, then propagate EPERM as EPERM */
                        తిరిగి r;

                ప్రతిజ్ఞా(notify >= 0);
                ప్రతిజ్ఞా(wd >= 0);

                కొరకు (;;) {
                        usec_t ఎడమ;
                        ఒకవేళ (timeout == USEC_INFINITY)
                                ఎడమ = USEC_INFINITY;
                        లేకపోతే {
                                ప్రతిజ్ఞా(ts != USEC_INFINITY);

                                usec_t n = usec_sub_unsigned(now(CLOCK_MONOTONIC), ts);
                                ఒకవేళ (n >= timeout)
                                        తిరిగి -ETIMEDOUT;

                                ఎడమ = timeout - n;
                        }

                        r = ppoll_usec_నిండిన(
                                        &(నిర్మాణం pollfd) {
                                                .fd = notify,
                                                .events = POLLIN,
                                        },
                                        /* n_fds= */ 1,
                                        ఎడమ,
                                        &poll_ss);
                        ఒకవేళ (r < 0)
                                తిరిగి r;
                        ఒకవేళ (r == 0)
                                తిరిగి -ETIMEDOUT;

                        సంఘం inotify_event_బఫరు బఫరు;
                        ssize_t l;
                        l = పఠ(notify, &బఫరు, పరిమాణం(బఫరు));
                        ఒకవేళ (l < 0) {
                                ఒకవేళ (ERRNO_IS_TRANSIENT(errno))
                                        కొనసాగు;

                                తిరిగి -errno;
                        }

                        FOREACH_INOTIFY_EVENT(e, బఫరు, l) {
                                ఒకవేళ (e->ముసుగు_3 & IN_Q_OVERFLOW) /* If we hit an inotify queue overflow, simply check if the terminal is up for grabs now. */
                                        ఆపు;

                                ఒకవేళ (e->wd != wd || !(e->ముసుగు_3 & IN_CLOSE)) /* Safety checks */
                                        తిరిగి -EIO;
                        }

                        ఆపు;
                }

                /* We close the tty fd here since if the old session ended our handle will be dead. It's important that
                 * we do this after sleeping, so that we don't enter an endless loop. */
                fd = సురక్షిత_close(fd);
        }

        తిరిగి TAKE_FD(fd);
}

పూర్ణాంకం release_terminal(శూన్య) {
        _cleanup_close_ పూర్ణాంకం fd = -EBADF;
        పూర్ణాంకం r;

        fd = open("/dev/tty", O_RDWR|O_NOCTTY|O_CLOEXEC|O_NONBLOCK);
        ఒకవేళ (fd < 0)
                తిరిగి -errno;

        /* Temporarily ignore SIGHUP, so that we don't get SIGHUP'ed
         * by our own TIOCNOTTY */
        నిర్మాణం sigaction sa_old;
        assert_se(sigaction(SIGHUP, &sigaction_ignore, &sa_old) >= 0);

        r = RET_NERRNO(ioctl(fd, TIOCNOTTY));

        assert_se(sigaction(SIGHUP, &sa_old, ఖాళీ) >= 0);

        తిరిగి r;
}

పూర్ణాంకం terminal_new_session(శూన్య) {

        /* Make us the new session leader, and set stdin tty to be our controlling terminal.
         *
         * Why stdin? Well, the ctty logic is relevant for signal delivery mostly, i.e. if people hit C-c
         * or the line is hung up. Such events are basically just a form of input, via a side channel
         * (that side channel being signal delivery, i.e. SIGINT, SIGHUP et al). Hence we focus on input,
         * not output here. */

        ఒకవేళ (!isatty_సురక్షిత(STDIN_FILENO))
                తిరిగి -ENXIO;

        (శూన్య) setsid();
        తిరిగి RET_NERRNO(ioctl(STDIN_FILENO, TIOCSCTTY, 0));
}

శూన్య terminal_detach_session(శూన్య) {
        (శూన్య) setsid();
        (శూన్య) release_terminal();
}

పూర్ణాంకం terminal_vhangup_fd(పూర్ణాంకం fd) {
        ప్రతిజ్ఞా(fd >= 0);
        తిరిగి RET_NERRNO(ioctl(fd, TIOCVHANGUP));
}

పూర్ణాంకం terminal_vhangup(స్థిరరాశి అక్షరం *tty) {
        _cleanup_close_ పూర్ణాంకం fd = -EBADF;

        ప్రతిజ్ఞా(tty);

        fd = open_terminal(tty, O_RDWR|O_NOCTTY|O_CLOEXEC);
        ఒకవేళ (fd < 0)
                తిరిగి fd;

        తిరిగి terminal_vhangup_fd(fd);
}

పూర్ణాంకం vt_disallocate(స్థిరరాశి అక్షరం *tty_మార్గం) {
        ప్రతిజ్ఞా(tty_మార్గం);

        /* Deallocate the VT if possible. If not possible (i.e. because it is the active one), at least clear
         * it entirely (including the scrollback buffer). */

        పూర్ణాంకం ttynr = vtnr_from_tty(tty_మార్గం);
        ఒకవేళ (ttynr > 0) {
                _cleanup_close_ పూర్ణాంకం fd = open_terminal("/dev/tty0", O_RDWR|O_NOCTTY|O_CLOEXEC|O_NONBLOCK);
                ఒకవేళ (fd < 0)
                        తిరిగి fd;

                /* Try to deallocate */
                ఒకవేళ (ioctl(fd, VT_DISALLOCATE, ttynr) >= 0)
                        తిరిగి 0;
                ఒకవేళ (errno != EBUSY)
                        తిరిగి -errno;
        }

        /* So this is not a VT (in which case we cannot deallocate it), or we failed to deallocate. Let's at
         * least clear the screen. */

        _cleanup_close_ పూర్ణాంకం fd2 = open_terminal(tty_మార్గం, O_WRONLY|O_NOCTTY|O_CLOEXEC|O_NONBLOCK);
        ఒకవేళ (fd2 < 0)
                తిరిగి fd2;

        తిరిగి loop_write_నిండిన(fd2,
                               "\033[r"   /* clear scrolling region */
                               "\033[H"   /* move home */
                               "\033[3J"  /* clear screen including scrollback, requires Linux 2.6.40 */
                               "\033c",   /* reset to initial state */
                               SIZE_MAX,
                               CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
}

స్థిర పూర్ణాంకం vt_default_utf8(శూన్య) {
        /* Read the default VT UTF8 setting from the kernel */
        తిరిగి read_boolean_file("/sys/module/vt/parameters/default_utf8");
}

స్థిర పూర్ణాంకం vt_reset_keyboard(పూర్ణాంకం fd) {
        పూర్ణాంకం r, kb;

        ప్రతిజ్ఞా(fd >= 0);

        /* If we can't read the default, then default to Unicode. It's 2024 after all. */
        r = vt_default_utf8();
        ఒకవేళ (r < 0)
                log_debug_errno(r, "Failed to determine kernel VT UTF-8 mode, assuming enabled: %m");

        kb = vt_default_utf8() != 0 ? K_UNICODE : K_XLATE;
        తిరిగి RET_NERRNO(ioctl(fd, KDSKBMODE, kb));
}

స్థిర పూర్ణాంకం terminal_reset_ioctl(పూర్ణాంకం fd, bool switch_గమ్యం_పాఠ్యం) {
        నిర్మాణం termios termios;
        పూర్ణాంకం r;

        /* Set terminal to some sane defaults */

        ప్రతిజ్ఞా(fd >= 0);

        /* We leave locked terminal attributes untouched, so that Plymouth may set whatever it wants to set,
         * and we don't interfere with that. */

        /* Disable exclusive mode, just in case */
        ఒకవేళ (ioctl(fd, TIOCNXCL) < 0)
                log_debug_errno(errno, "TIOCNXCL ioctl failed on TTY, ignoring: %m");

        /* Switch to text mode */
        ఒకవేళ (switch_గమ్యం_పాఠ్యం)
                ఒకవేళ (ioctl(fd, KDSETMODE, KD_TEXT) < 0)
                        log_debug_errno(errno, "KDSETMODE ioctl for switching to text mode failed on TTY, ignoring: %m");

        /* Set default keyboard mode */
        r = vt_reset_keyboard(fd);
        ఒకవేళ (r < 0)
                log_debug_errno(r, "Failed to reset VT keyboard, ignoring: %m");

        ఒకవేళ (tcgetattr(fd, &termios) < 0) {
                r = log_debug_errno(errno, "Failed to get terminal parameters: %m");
                వెళ్ళు ముగింపు;
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
        ఒకవేళ (r < 0)
                log_debug_errno(r, "Failed to set terminal parameters: %m");

ముగింపు:
        /* Just in case, flush all crap out */
        (శూన్య) tcflush(fd, TCIOFLUSH);

        తిరిగి r;
}

పూర్ణాంకం terminal_reset_ansi_seq(పూర్ణాంకం fd) {
        _cleanup_(nonblock_resetp) పూర్ణాంకం nonblock_reset = -EBADF;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(fd >= 0);

        ఒకవేళ (getenv_terminal_is_dumb())
                తిరిగి 0;

        r = fd_nonblock(fd, true);
        ఒకవేళ (r < 0)
                తిరిగి log_debug_errno(r, "Failed to set terminal to non-blocking mode: %m");
        ఒకవేళ (r > 0)
                nonblock_reset = fd;

        r = loop_write_నిండిన(fd,
                            "\033[!p"              /* soft terminal reset */
                            ANSI_OSC "104" ANSI_ST /* reset color palette via OSC 104 */
                            ANSI_సాధారణ            /* reset colors */
                            "\033[?7h"             /* enable line-wrapping */
                            "\033[1G"              /* place cursor at beginning of current line */
                            "\033[0J",             /* erase till end of screen */
                            SIZE_MAX,
                            CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        ఒకవేళ (r < 0)
                log_debug_errno(r, "Failed to reset terminal through ANSI sequences: %m");

        తిరిగి r;
}

శూన్య reset_dev_console_fd(పూర్ణాంకం fd, bool switch_గమ్యం_పాఠ్యం) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(fd >= 0);

        _cleanup_close_ పూర్ణాంకం తాళం_fd = తాళం_dev_console();
        ఒకవేళ (తాళం_fd < 0)
                log_debug_errno(తాళం_fd, "Failed to lock /dev/console, ignoring: %m");

        r = terminal_reset_ioctl(fd, switch_గమ్యం_పాఠ్యం);
        ఒకవేళ (r < 0)
                log_warning_errno(r, "Failed to reset /dev/console, ignoring: %m");

        గుర్తులేని అడ్డువరుసలు, నిలువుల_సంఖ్య;
        r = proc_cmdline_tty_కొలత("/dev/console", &అడ్డువరుసలు, &నిలువుల_సంఖ్య);
        ఒకవేళ (r < 0)
                log_warning_errno(r, "Failed to get /dev/console size, ignoring: %m");
        లేకపోతే ఒకవేళ (r > 0) {
                r = terminal_సమితి_కొలత_fd(fd, ఖాళీ, అడ్డువరుసలు, నిలువుల_సంఖ్య);
                ఒకవేళ (r < 0)
                        log_warning_errno(r, "Failed to set configured terminal size on /dev/console, ignoring: %m");
        } లేకపోతే
                (శూన్య) terminal_fix_కొలత(fd, fd);

        r = terminal_reset_ansi_seq(fd);
        ఒకవేళ (r < 0)
                log_warning_errno(r, "Failed to reset /dev/console using ANSI sequences, ignoring: %m");
}

పూర్ణాంకం తాళం_dev_console(శూన్య) {
        _cleanup_close_ పూర్ణాంకం fd = -EBADF;
        పూర్ణాంకం r;

        /* NB: We do not use O_NOFOLLOW here, because some container managers might place a symlink to some
         * pty in /dev/console, in which case it should be fine to lock the target TTY. */
        fd = open_terminal("/dev/console", O_RDONLY|O_CLOEXEC|O_NOCTTY);
        ఒకవేళ (fd < 0)
                తిరిగి fd;

        r = తాళం_generic(fd, తాళం_BSD, LOCK_EX);
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి TAKE_FD(fd);
}

పూర్ణాంకం make_console_stdio(bool switch_గమ్యం_పాఠ్యం) {
        పూర్ణాంకం fd, r;

        /* Make /dev/console the controlling terminal and stdin/stdout/stderr, if we can. If we can't use
         * /dev/null instead. This is particularly useful if /dev/console is turned off, e.g. if console=null
         * is specified on the kernel command line. */

        fd = acquire_terminal("/dev/console", ACQUIRE_TERMINAL_FORCE|ACQUIRE_TERMINAL_PERMISSIVE, USEC_INFINITY);
        ఒకవేళ (fd < 0) {
                log_warning_errno(fd, "Failed to acquire terminal, using /dev/null stdin/stdout/stderr instead: %m");

                r = make_null_stdio();
                ఒకవేళ (r < 0)
                        తిరిగి log_error_errno(r, "Failed to make /dev/null stdin/stdout/stderr: %m");

        } లేకపోతే {
                reset_dev_console_fd(fd, switch_గమ్యం_పాఠ్యం);

                r = rearrange_stdio(fd, fd, fd); /* This invalidates 'fd' both on success and on failure. */
                ఒకవేళ (r < 0)
                        తిరిగి log_error_errno(r, "Failed to make terminal stdin/stdout/stderr: %m");
        }

        reset_terminal_feature_caches();
        తిరిగి 0;
}

స్థిర పూర్ణాంకం vtnr_from_tty_raw(స్థిరరాశి అక్షరం *tty, గుర్తులేని *ret) {
        ప్రతిజ్ఞా(tty);

        tty = skip_dev_ఉపసర్గ(tty);

        స్థిరరాశి అక్షరం *e = startswith(tty, "tty");
        ఒకవేళ (!e)
                తిరిగి -EINVAL;

        తిరిగి సురక్షిత_atou(e, ret);
}

పూర్ణాంకం vtnr_from_tty(స్థిరరాశి అక్షరం *tty) {
        గుర్తులేని u;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(tty);

        r = vtnr_from_tty_raw(tty, &u);
        ఒకవేళ (r < 0)
                తిరిగి r;
        ఒకవేళ (!vtnr_is_valid(u))
                తిరిగి -ERANGE;

        తిరిగి (పూర్ణాంకం) u;
}

bool tty_is_vc(స్థిరరాశి అక్షరం *tty) {
        ప్రతిజ్ఞా(tty);

        /* NB: for >= 0 values no range check is conducted here, on the assumption that the caller will
         * either extract vtnr through vtnr_from_tty() later where ERANGE would be reported, or doesn't care
         * about whether it's strictly valid, but only asking "does this fall into the vt category?", for which
         * "yes" seems to be a better answer. */

        తిరిగి vtnr_from_tty_raw(tty, /* ret= */ ఖాళీ) >= 0;
}

bool tty_is_console(స్థిరరాశి అక్షరం *tty) {
        ప్రతిజ్ఞా(tty);

        తిరిగి streq(skip_dev_ఉపసర్గ(tty), "console");
}

పూర్ణాంకం resolve_dev_console(అక్షరం **ret) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(ret);

        /* Resolve where /dev/console is pointing to. If /dev/console is a symlink (like in container
         * managers), we'll just resolve the symlink. If it's a real device node, we'll use if
         * /sys/class/tty/tty0/active, but only if /sys/ is actually ours (i.e. not read-only-mounted which
         * is a sign for container setups). */

        _cleanup_free_ అక్షరం *chased = ఖాళీ;
        r = chase("/dev/console", /* root= */ ఖాళీ, /* flags= */ 0, &chased, /* ret_fd= */ ఖాళీ);
        ఒకవేళ (r < 0)
                తిరిగి r;
        ఒకవేళ (!మార్గం_equal(chased, "/dev/console")) {
                *ret = TAKE_PTR(chased);
                తిరిగి 0;
        }

        r = మార్గం_is_read_only_fs("/sys");
        ఒకవేళ (r < 0)
                తిరిగి r;
        ఒకవేళ (r > 0)
                తిరిగి -ENOMEDIUM;

        _cleanup_free_ అక్షరం *చురుకైన = ఖాళీ;
        r = read_one_పంక్తి_file("/sys/class/tty/console/active", &చురుకైన);
        ఒకవేళ (r < 0)
                తిరిగి r;
        ఒకవేళ (r == 0)
                తిరిగి -ENXIO;

        /* If multiple log outputs are configured the last one is what /dev/console points to */
        స్థిరరాశి అక్షరం *tty = strrchr(చురుకైన, ' ');
        ఒకవేళ (tty)
                tty++;
        లేకపోతే
                tty = చురుకైన;

        ఒకవేళ (streq(tty, "tty0")) {
                చురుకైన = mfree(చురుకైన);

                /* Get the active VC (e.g. tty1) */
                r = read_one_పంక్తి_file("/sys/class/tty/tty0/active", &చురుకైన);
                ఒకవేళ (r < 0)
                        తిరిగి r;
                ఒకవేళ (r == 0)
                        తిరిగి -ENXIO;

                tty = చురుకైన;
        }

        _cleanup_free_ అక్షరం *మార్గం = ఖాళీ;
        మార్గం = మార్గం_join("/dev", tty);
        ఒకవేళ (!మార్గం)
                తిరిగి -ENOMEM;

        *ret = TAKE_PTR(మార్గం);
        తిరిగి 0;
}

పూర్ణాంకం get_kernel_consoles(అక్షరం ***ret) {
        _cleanup_strv_free_ అక్షరం **l = ఖాళీ;
        _cleanup_free_ అక్షరం *పంక్తి = ఖాళీ;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(ret);

        /* If /sys/ is mounted read-only this means we are running in some kind of container environment.
         * In that case /sys/ would reflect the host system, not us, hence ignore the data we can read from it. */
        ఒకవేళ (మార్గం_is_read_only_fs("/sys") > 0)
                వెళ్ళు fallback;

        r = read_one_పంక్తి_file("/sys/class/tty/console/active", &పంక్తి);
        ఒకవేళ (r < 0)
                తిరిగి r;

        కొరకు (స్థిరరాశి అక్షరం *p = పంక్తి;;) {
                _cleanup_free_ అక్షరం *tty = ఖాళీ, *మార్గం = ఖాళీ;

                r = వెలికితీయి_మొదటి_మాట(&p, &tty, ఖాళీ, 0);
                ఒకవేళ (r < 0)
                        తిరిగి r;
                ఒకవేళ (r == 0)
                        ఆపు;

                ఒకవేళ (streq(tty, "tty0")) {
                        tty = mfree(tty);
                        r = read_one_పంక్తి_file("/sys/class/tty/tty0/active", &tty);
                        ఒకవేళ (r < 0)
                                తిరిగి r;
                        ఒకవేళ (r == 0) {
                                log_debug("No VT active, skipping /dev/tty0.");
                                కొనసాగు;
                        }
                }

                మార్గం = మార్గం_join("/dev", tty);
                ఒకవేళ (!మార్గం)
                        తిరిగి -ENOMEM;

                ఒకవేళ (access(మార్గం, F_OK) < 0) {
                        log_debug_errno(errno, "Console device %s is not accessible, skipping: %m", మార్గం);
                        కొనసాగు;
                }

                r = strv_consume(&l, TAKE_PTR(మార్గం));
                ఒకవేళ (r < 0)
                        తిరిగి r;
        }

        ఒకవేళ (strv_isempty(l)) {
                log_debug("No devices found for system console");
                వెళ్ళు fallback;
        }

        *ret = TAKE_PTR(l);
        తిరిగి strv_నిడివి(*ret);

fallback:
        r = strv_extend(&l, "/dev/console");
        ఒకవేళ (r < 0)
                తిరిగి r;

        *ret = TAKE_PTR(l);
        తిరిగి 0;
}

bool tty_is_vc_resolve(స్థిరరాశి అక్షరం *tty) {
        _cleanup_free_ అక్షరం *resolved = ఖాళీ;

        ప్రతిజ్ఞా(tty);

        ఒకవేళ (streq(skip_dev_ఉపసర్గ(tty), "console")) {
                ఒకవేళ (resolve_dev_console(&resolved) < 0)
                        తిరిగి false;

                tty = resolved;
        }

        తిరిగి tty_is_vc(tty);
}

పూర్ణాంకం fd_columns(పూర్ణాంకం fd) {
        నిర్మాణం winsize ws = {};

        ఒకవేళ (fd < 0)
                తిరిగి -EBADF;

        ఒకవేళ (ioctl(fd, TIOCGWINSZ, &ws) < 0)
                తిరిగి -errno;

        ఒకవేళ (ws.ws_col <= 0)
                తిరిగి -ENODATA; /* some tty types come up with invalid row/column initially, return a recognizable error for that */

        తిరిగి ws.ws_col;
}

పూర్ణాంకం getenv_columns(శూన్య) {
        పూర్ణాంకం r;

        స్థిరరాశి అక్షరం *e = getenv("COLUMNS");
        ఒకవేళ (!e)
                తిరిగి -ENXIO;

        గుర్తులేని c;
        r = సురక్షిత_atou_bounded(e, 1, USHRT_MAX, &c);
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి (పూర్ణాంకం) c;
}

గుర్తులేని columns(శూన్య) {

        ఒకవేళ (cached_columns > 0)
                తిరిగి cached_columns;

        పూర్ణాంకం c = getenv_columns();
        ఒకవేళ (c < 0) {
                c = fd_columns(STDOUT_FILENO);
                ఒకవేళ (c < 0)
                        c = 80;
        }

        ప్రతిజ్ఞా(c > 0);

        cached_columns = c;
        తిరిగి cached_columns;
}

పూర్ణాంకం fd_పంక్తులు(పూర్ణాంకం fd) {
        నిర్మాణం winsize ws = {};

        ఒకవేళ (fd < 0)
                తిరిగి -EBADF;

        ఒకవేళ (ioctl(fd, TIOCGWINSZ, &ws) < 0)
                తిరిగి -errno;

        ఒకవేళ (ws.ws_row <= 0)
                తిరిగి -ENODATA; /* some tty types come up with invalid row/column initially, return a recognizable error for that */

        తిరిగి ws.ws_row;
}

గుర్తులేని పంక్తులు(శూన్య) {
        స్థిరరాశి అక్షరం *e;
        పూర్ణాంకం l;

        ఒకవేళ (cached_పంక్తులు > 0)
                తిరిగి cached_పంక్తులు;

        l = 0;
        e = getenv("LINES");
        ఒకవేళ (e)
                (శూన్య) సురక్షిత_atoi(e, &l);

        ఒకవేళ (l <= 0 || l > USHRT_MAX) {
                l = fd_పంక్తులు(STDOUT_FILENO);
                ఒకవేళ (l <= 0)
                        l = 24;
        }

        cached_పంక్తులు = l;
        తిరిగి cached_పంక్తులు;
}

పూర్ణాంకం terminal_సమితి_కొలత_fd(పూర్ణాంకం fd, స్థిరరాశి అక్షరం *ident, గుర్తులేని అడ్డువరుసలు, గుర్తులేని నిలువుల_సంఖ్య) {
        నిర్మాణం winsize ws;

        ప్రతిజ్ఞా(fd >= 0);

        ఒకవేళ (!ident)
                ident = "TTY";

        ఒకవేళ (అడ్డువరుసలు == UINT_MAX && నిలువుల_సంఖ్య == UINT_MAX)
                తిరిగి 0;

        ఒకవేళ (ioctl(fd, TIOCGWINSZ, &ws) < 0)
                తిరిగి log_debug_errno(errno,
                                       "TIOCGWINSZ ioctl for getting %s size failed, not setting terminal size: %m",
                                       ident);

        ఒకవేళ (అడ్డువరుసలు == UINT_MAX)
                అడ్డువరుసలు = ws.ws_row;
        లేకపోతే ఒకవేళ (అడ్డువరుసలు > USHRT_MAX)
                అడ్డువరుసలు = USHRT_MAX;

        ఒకవేళ (నిలువుల_సంఖ్య == UINT_MAX)
                నిలువుల_సంఖ్య = ws.ws_col;
        లేకపోతే ఒకవేళ (నిలువుల_సంఖ్య > USHRT_MAX)
                నిలువుల_సంఖ్య = USHRT_MAX;

        ఒకవేళ (అడ్డువరుసలు == ws.ws_row && నిలువుల_సంఖ్య == ws.ws_col)
                తిరిగి 0;

        ws.ws_row = అడ్డువరుసలు;
        ws.ws_col = నిలువుల_సంఖ్య;

        ఒకవేళ (ioctl(fd, TIOCSWINSZ, &ws) < 0)
                తిరిగి log_debug_errno(errno, "TIOCSWINSZ ioctl for setting %s size failed: %m", ident);

        తిరిగి 0;
}

పూర్ణాంకం proc_cmdline_tty_కొలత(స్థిరరాశి అక్షరం *tty, గుర్తులేని *ret_అడ్డువరుసలు, గుర్తులేని *ret_నిలువుల_సంఖ్య) {
        _cleanup_free_ అక్షరం *rowskey = ఖాళీ, *rowsvalue = ఖాళీ, *colskey = ఖాళీ, *colsvalue = ఖాళీ;
        గుర్తులేని అడ్డువరుసలు = UINT_MAX, నిలువుల_సంఖ్య = UINT_MAX;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(tty);

        ఒకవేళ (!ret_అడ్డువరుసలు && !ret_నిలువుల_సంఖ్య)
                తిరిగి 0;

        tty = skip_dev_ఉపసర్గ(tty);
        ఒకవేళ (మార్గం_startswith(tty, "pts/"))
                తిరిగి -EMEDIUMTYPE;
        ఒకవేళ (!లోపల_charset(tty, ALPHANUMERICAL))
                తిరిగి log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "TTY name '%s' contains non-alphanumeric characters, not searching kernel cmdline for size.", tty);

        rowskey = strjoin("systemd.tty.rows.", tty);
        ఒకవేళ (!rowskey)
                తిరిగి -ENOMEM;

        colskey = strjoin("systemd.tty.columns.", tty);
        ఒకవేళ (!colskey)
                తిరిగి -ENOMEM;

        r = proc_cmdline_get_కీలకం_many(/* flags= */ 0,
                                      rowskey, &rowsvalue,
                                      colskey, &colsvalue);
        ఒకవేళ (r < 0)
                తిరిగి log_debug_errno(r, "Failed to read TTY size of %s from kernel cmdline: %m", tty);

        ఒకవేళ (rowsvalue) {
                r = సురక్షిత_atou(rowsvalue, &అడ్డువరుసలు);
                ఒకవేళ (r < 0)
                        తిరిగి log_debug_errno(r, "Failed to parse %s=%s: %m", rowskey, rowsvalue);
        }

        ఒకవేళ (colsvalue) {
                r = సురక్షిత_atou(colsvalue, &నిలువుల_సంఖ్య);
                ఒకవేళ (r < 0)
                        తిరిగి log_debug_errno(r, "Failed to parse %s=%s: %m", colskey, colsvalue);
        }

        ఒకవేళ (ret_అడ్డువరుసలు)
                *ret_అడ్డువరుసలు = అడ్డువరుసలు;
        ఒకవేళ (ret_నిలువుల_సంఖ్య)
                *ret_నిలువుల_సంఖ్య = నిలువుల_సంఖ్య;

        తిరిగి అడ్డువరుసలు != UINT_MAX || నిలువుల_సంఖ్య != UINT_MAX;
}

/* intended to be used as a SIGWINCH sighandler */
శూన్య columns_పంక్తులు_cache_reset(పూర్ణాంకం signum) {
        cached_columns = 0;
        cached_పంక్తులు = 0;
}

శూన్య reset_terminal_feature_caches(శూన్య) {
        cached_columns = 0;
        cached_పంక్తులు = 0;

        cached_on_tty = -1;
        cached_on_dev_null = -1;

        reset_ansi_feature_caches();
}

bool on_tty(శూన్య) {

        /* We check both stdout and stderr, so that situations where pipes on the shell are used are reliably
         * recognized, regardless if only the output or the errors are piped to some place. Since on_tty() is generally
         * used to default to a safer, non-interactive, non-color mode of operation it's probably good to be defensive
         * here, and check for both. Note that we don't check for STDIN_FILENO, because it should fine to use fancy
         * terminal functionality when outputting stuff, even if the input is piped to us. */

        ఒకవేళ (cached_on_tty < 0)
                cached_on_tty =
                        isatty_సురక్షిత(STDOUT_FILENO) &&
                        isatty_సురక్షిత(STDERR_FILENO);

        తిరిగి cached_on_tty;
}

పూర్ణాంకం getttyname_malloc(పూర్ణాంకం fd, అక్షరం **ret) {
        అక్షరం మార్గం[PATH_MAX]; /* PATH_MAX is counted *with* the trailing NUL byte */
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(fd >= 0);
        ప్రతిజ్ఞా(ret);

        r = ttyname_r(fd, మార్గం, పరిమాణం మార్గం); /* positive error */
        ప్రతిజ్ఞా(r >= 0);
        ఒకవేళ (r == ERANGE)
                తిరిగి -ENAMETOOLONG;
        ఒకవేళ (r > 0)
                తిరిగి -r;

        తిరిగి strdup_గమ్యం(ret, skip_dev_ఉపసర్గ(మార్గం));
}

పూర్ణాంకం getttyname_harder(పూర్ణాంకం fd, అక్షరం **ret) {
        _cleanup_free_ అక్షరం *s = ఖాళీ;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(ret);

        r = getttyname_malloc(fd, &s);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (streq(s, "tty"))
                తిరిగి get_ctty(0, ఖాళీ, ret);

        *ret = TAKE_PTR(s);
        తిరిగి 0;
}

పూర్ణాంకం get_ctty_devnr(pid_t pid, dev_t *ret) {
        _cleanup_free_ అక్షరం *పంక్తి = ఖాళీ;
        గుర్తులేని దీర్ఘ ttynr;
        స్థిరరాశి అక్షరం *p;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(pid >= 0);

        p = procfs_file_alloca(pid, "stat");
        r = read_one_పంక్తి_file(p, &పంక్తి);
        ఒకవేళ (r < 0)
                తిరిగి r;

        p = strrchr(పంక్తి, ')');
        ఒకవేళ (!p)
                తిరిగి -EIO;

        p++;

        ఒకవేళ (అక్షరశ్రేణి_చదువు(p, " "
                   "%*c "  /* state */
                   "%*d "  /* ppid */
                   "%*d "  /* pgrp */
                   "%*d "  /* session */
                   "%lu ", /* ttynr */
                   &ttynr) != 1)
                తిరిగి -EIO;

        ఒకవేళ (devnum_is_zero(ttynr))
                తిరిగి -ENXIO;

        ఒకవేళ (ret)
                *ret = (dev_t) ttynr;

        తిరిగి 0;
}

పూర్ణాంకం get_ctty(pid_t pid, dev_t *ret_devnr, అక్షరం **ret) {
        అక్షరం pty[STRLEN("/dev/pts/") + DECIMAL_STR_MAX(dev_t) + 1];
        _cleanup_free_ అక్షరం *buf = ఖాళీ;
        స్థిరరాశి అక్షరం *fn = ఖాళీ, *w;
        dev_t devnr;
        పూర్ణాంకం r;

        r = get_ctty_devnr(pid, &devnr);
        ఒకవేళ (r < 0)
                తిరిగి r;

        r = device_మార్గం_make_canonical(S_IFCHR, devnr, &buf);
        ఒకవేళ (r < 0) {
                నిర్మాణం stat st;

                ఒకవేళ (r != -ENOENT) /* No symlink for this in /dev/char/? */
                        తిరిగి r;

                /* Maybe this is PTY? PTY devices are not listed in /dev/char/, as they don't follow the
                 * Linux device model and hence device_path_make_canonical() doesn't work for them. Let's
                 * assume this is a PTY for a moment, and check if the device node this would then map to in
                 * /dev/pts/ matches the one we are looking for. This way we don't have to hardcode the major
                 * number (which is 136 btw), but we still rely on the fact that PTY numbers map directly to
                 * the minor number of the pty. */
                xsprintf(pty, "/dev/pts/%u", minor(devnr));

                ఒకవేళ (stat(pty, &st) < 0) {
                        ఒకవేళ (errno != ENOENT)
                                తిరిగి -errno;

                } లేకపోతే ఒకవేళ (S_ISCHR(st.st_mode) && devnr == st.st_rdev) /* Bingo! */
                        fn = pty;

                ఒకవేళ (!fn) {
                        /* Doesn't exist, or not a PTY? Probably something similar to the PTYs which have no
                         * symlink in /dev/char/. Let's return something vaguely useful. */
                        r = device_మార్గం_make_major_minor(S_IFCHR, devnr, &buf);
                        ఒకవేళ (r < 0)
                                తిరిగి r;

                        fn = buf;
                }
        } లేకపోతే
                fn = buf;

        w = మార్గం_startswith(fn, "/dev/");
        ఒకవేళ (!w)
                తిరిగి -EINVAL;

        ఒకవేళ (ret) {
                r = strdup_గమ్యం(ret, w);
                ఒకవేళ (r < 0)
                        తిరిగి r;
        }

        ఒకవేళ (ret_devnr)
                *ret_devnr = devnr;

        తిరిగి 0;
}

పూర్ణాంకం ptsname_malloc(పూర్ణాంకం fd, అక్షరం **ret) {
        ప్రతిజ్ఞా(fd >= 0);
        ప్రతిజ్ఞా(ret);

        కొరకు (size_t l = 50;;) {
                _cleanup_free_ అక్షరం *c = ఖాళీ;

                c = new(అక్షరం, l);
                ఒకవేళ (!c)
                        తిరిగి -ENOMEM;

                ఒకవేళ (ptsname_r(fd, c, l) >= 0) {
                        *ret = TAKE_PTR(c);
                        తిరిగి 0;
                }
                ఒకవేళ (errno != ERANGE)
                        తిరిగి -errno;

                ఒకవేళ (!MUL_కేటాయించు_సురక్షిత(&l, 2))
                        తిరిగి -ENOMEM;
        }
}

పూర్ణాంకం openpt_allocate(పూర్ణాంకం flags, అక్షరం **ret_peer_మార్గం) {
        _cleanup_close_ పూర్ణాంకం fd = -EBADF;
        పూర్ణాంకం r;

        fd = posix_openpt(flags|O_NOCTTY|O_CLOEXEC);
        ఒకవేళ (fd < 0)
                తిరిగి -errno;

        _cleanup_free_ అక్షరం *p = ఖాళీ;
        ఒకవేళ (ret_peer_మార్గం) {
                r = ptsname_malloc(fd, &p);
                ఒకవేళ (r < 0)
                        తిరిగి r;

                ఒకవేళ (!మార్గం_startswith(p, "/dev/pts/"))
                        తిరిగి -EINVAL;
        }

        ఒకవేళ (unlockpt(fd) < 0)
                తిరిగి -errno;

        ఒకవేళ (ret_peer_మార్గం)
                *ret_peer_మార్గం = TAKE_PTR(p);

        తిరిగి TAKE_FD(fd);
}

స్థిర పూర్ణాంకం ptsname_namespace(పూర్ణాంకం pty, అక్షరం **ret) {
        పూర్ణాంకం no = -1;

        ప్రతిజ్ఞా(pty >= 0);
        ప్రతిజ్ఞా(ret);

        /* Like ptsname(), but doesn't assume that the path is
         * accessible in the local namespace. */

        ఒకవేళ (ioctl(pty, TIOCGPTN, &no) < 0)
                తిరిగి -errno;

        ఒకవేళ (no < 0)
                తిరిగి -EIO;

        ఒకవేళ (asprintf(ret, "/dev/pts/%i", no) < 0)
                తిరిగి -ENOMEM;

        తిరిగి 0;
}

పూర్ణాంకం openpt_allocate_లోపల_namespace(
                స్థిరరాశి PidRef *pidref,
                పూర్ణాంకం flags,
                అక్షరం **ret_peer_మార్గం) {

        _cleanup_close_ పూర్ణాంకం pidnsfd = -EBADF, mntnsfd = -EBADF, usernsfd = -EBADF, rootfd = -EBADF, fd = -EBADF;
        _cleanup_close_pair_ పూర్ణాంకం pair[2] = EBADF_PAIR;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(pidref);

        r = pidref_namespace_open(pidref, &pidnsfd, &mntnsfd, /* ret_netns_fd= */ ఖాళీ, &usernsfd, &rootfd);
        ఒకవేళ (r < 0)
                తిరిగి log_debug_errno(r, "Failed to open namespaces of PID "PID_FMT": %m", pidref->pid);

        ఒకవేళ (socketpair(AF_UNIX, SOCK_DGRAM|SOCK_CLOEXEC, 0, pair) < 0)
                తిరిగి -errno;

        r = namespace_fork(
                        "(sd-openptns)",
                        "(sd-openpt)",
                        FORK_RESET_సంకేతాలు|FORK_DEATHSIG_SIGKILL|FORK_WAIT,
                        pidnsfd,
                        mntnsfd,
                        /* netns_fd= */ -EBADF,
                        usernsfd,
                        rootfd,
                        /* ret= */ ఖాళీ);
        ఒకవేళ (r < 0)
                తిరిగి r;
        ఒకవేళ (r == 0) {
                pair[0] = సురక్షిత_close(pair[0]);

                fd = openpt_allocate(flags, /* ret_peer_path= */ ఖాళీ);
                ఒకవేళ (fd < 0)
                        _exit(EXIT_FAILURE);

                ఒకవేళ (send_one_fd(pair[1], fd, 0) < 0)
                        _exit(EXIT_FAILURE);

                _exit(EXIT_SUCCESS);
        }

        pair[1] = సురక్షిత_close(pair[1]);

        fd = receive_one_fd(pair[0], 0);
        ఒకవేళ (fd < 0)
                తిరిగి fd;

        ఒకవేళ (ret_peer_మార్గం) {
                r = ptsname_namespace(fd, ret_peer_మార్గం);
                ఒకవేళ (r < 0)
                        తిరిగి r;
        }

        తిరిగి TAKE_FD(fd);
}

స్థిర bool on_dev_null(శూన్య) {
        నిర్మాణం stat dst, ost, est;

        ఒకవేళ (cached_on_dev_null >= 0)
                తిరిగి cached_on_dev_null;

        ఒకవేళ (stat("/dev/null", &dst) < 0 || fstat(STDOUT_FILENO, &ost) < 0 || fstat(STDERR_FILENO, &est) < 0)
                cached_on_dev_null = false;
        లేకపోతే
                cached_on_dev_null = stat_inode_same(&dst, &ost) && stat_inode_same(&dst, &est);

        తిరిగి cached_on_dev_null;
}

bool పదం_env_valid(స్థిరరాశి అక్షరం *పదం) {
        /* Checks if the specified $TERM value is suitable for propagation, i.e. is not empty, not set to
         * "unknown" (as is common in CI), and only contains characters valid in terminal type names.
         * Valid $TERM values are things like "xterm-256color", "linux", "screen.xterm-256color", i.e.
         * alphanumeric characters, hyphens, underscores, dots, and plus signs. */
        తిరిగి !isempty(పదం) &&
                !streq(పదం, "unknown") &&
                లోపల_charset(పదం, ALPHANUMERICAL "-_+.");
}

bool getenv_terminal_is_dumb(శూన్య) {
        స్థిరరాశి అక్షరం *e;

        e = getenv("TERM");
        ఒకవేళ (!e)
                తిరిగి true;

        తిరిగి streq(e, "dumb");
}

bool terminal_is_dumb(శూన్య) {
        ఒకవేళ (!on_tty() && !on_dev_null())
                తిరిగి true;

        తిరిగి getenv_terminal_is_dumb();
}

bool dev_console_colors_enabled(శూన్య) {
        _cleanup_free_ అక్షరం *s = ఖాళీ;
        Color_బహుళకం m;

        /* Returns true if we assume that color is supported on /dev/console.
         *
         * For that we first check if we explicitly got told to use colors or not, by checking $SYSTEMD_COLORS. If that
         * isn't set we check whether PID 1 has $TERM set, and if not, whether TERM is set on the kernel command
         * line. If we find $TERM set we assume color if it's not set to "dumb", similarly to how regular
         * colors_enabled() operates. */

        m = parse_systemd_colors();
        ఒకవేళ (m >= 0)
                తిరిగి m;

        ఒకవేళ (getenv("NO_COLOR"))
                తిరిగి false;

        ఒకవేళ (getenv_for_pid(1, "TERM", &s) <= 0)
                (శూన్య) proc_cmdline_get_కీలకం("TERM", 0, &s);

        తిరిగి !streq_ptr(s, "dumb");
}

పూర్ణాంకం vt_restore(పూర్ణాంకం fd) {

        స్థిర స్థిరరాశి నిర్మాణం vt_mode బహుళకం = {
                .బహుళకం = VT_AUTO,
        };

        పూర్ణాంకం r, ret = 0;

        ప్రతిజ్ఞా(fd >= 0);

        ఒకవేళ (!isatty_సురక్షిత(fd))
                తిరిగి log_debug_errno(SYNTHETIC_ERRNO(ENOTTY), "Asked to restore the VT for an fd that does not refer to a terminal.");

        ఒకవేళ (ioctl(fd, KDSETMODE, KD_TEXT) < 0)
                RET_GATHER(ret, log_debug_errno(errno, "Failed to set VT to text mode, ignoring: %m"));

        r = vt_reset_keyboard(fd);
        ఒకవేళ (r < 0)
                RET_GATHER(ret, log_debug_errno(r, "Failed to reset keyboard mode, ignoring: %m"));

        ఒకవేళ (ioctl(fd, VT_SETMODE, &బహుళకం) < 0)
                RET_GATHER(ret, log_debug_errno(errno, "Failed to set VT_AUTO mode, ignoring: %m"));

        r = fchmod_and_chown(fd, TTY_బహుళకం, 0, GID_INVALID);
        ఒకవేళ (r < 0)
                RET_GATHER(ret, log_debug_errno(r, "Failed to chmod()/chown() VT, ignoring: %m"));

        తిరిగి ret;
}

పూర్ణాంకం vt_release(పూర్ణాంకం fd, bool restore) {
        ప్రతిజ్ఞా(fd >= 0);

        /* This function releases the VT by acknowledging the VT-switch signal
         * sent by the kernel and optionally reset the VT in text and auto
         * VT-switching modes. */

        ఒకవేళ (!isatty_సురక్షిత(fd))
                తిరిగి log_debug_errno(SYNTHETIC_ERRNO(ENOTTY), "Asked to release the VT for an fd that does not refer to a terminal.");

        ఒకవేళ (ioctl(fd, VT_RELDISP, 1) < 0)
                తిరిగి -errno;

        ఒకవేళ (restore)
                తిరిగి vt_restore(fd);

        తిరిగి 0;
}

శూన్య get_log_colors(పూర్ణాంకం priority, స్థిరరాశి అక్షరం **on, స్థిరరాశి అక్షరం **off, స్థిరరాశి అక్షరం **highlight) {
        /* Note that this will initialize output variables only when there's something to output.
         * The caller must pre-initialize to "" or NULL as appropriate. */

        ఒకవేళ (priority <= LOG_ERR) {
                ఒకవేళ (on)
                        *on = ansi_highlight_red();
                ఒకవేళ (off)
                        *off = ansi_సాధారణ();
                ఒకవేళ (highlight)
                        *highlight = ansi_highlight();

        } లేకపోతే ఒకవేళ (priority <= LOG_WARNING) {
                ఒకవేళ (on)
                        *on = ansi_highlight_yellow();
                ఒకవేళ (off)
                        *off = ansi_సాధారణ();
                ఒకవేళ (highlight)
                        *highlight = ansi_highlight();

        } లేకపోతే ఒకవేళ (priority <= LOG_NOTICE) {
                ఒకవేళ (on)
                        *on = ansi_highlight();
                ఒకవేళ (off)
                        *off = ansi_సాధారణ();
                ఒకవేళ (highlight)
                        *highlight = ansi_highlight_red();

        } లేకపోతే ఒకవేళ (priority >= LOG_DEBUG) {
                ఒకవేళ (on)
                        *on = ansi_grey();
                ఒకవేళ (off)
                        *off = ansi_సాధారణ();
                ఒకవేళ (highlight)
                        *highlight = ansi_highlight_red();
        }
}

పూర్ణాంకం terminal_సమితి_cursor_position(పూర్ణాంకం fd, గుర్తులేని అడ్డువరుస, గుర్తులేని స్తంభం) {
        ప్రతిజ్ఞా(fd >= 0);

        అక్షరం cursor_position[STRLEN("\x1B[" ";" "H") + DECIMAL_STR_MAX(గుర్తులేని) * 2 + 1];
        xsprintf(cursor_position, "\x1B[%u;%uH", అడ్డువరుస, స్తంభం);

        తిరిగి loop_write(fd, cursor_position, SIZE_MAX);
}

స్థిర పూర్ణాంకం terminal_verify_same(పూర్ణాంకం ఆదానం_fd, పూర్ణాంకం output_fd) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(ఆదానం_fd >= 0);
        ప్రతిజ్ఞా(output_fd >= 0);

        /* Validates that the specified fds reference the same TTY */

        ఒకవేళ (ఆదానం_fd != output_fd) {
                నిర్మాణం stat sti;
                ఒకవేళ (fstat(ఆదానం_fd, &sti) < 0)
                        తిరిగి -errno;

                r = stat_verify_char(&sti); /* TTYs are character devices */
                ఒకవేళ (r < 0)
                        తిరిగి r;

                నిర్మాణం stat sto;
                ఒకవేళ (fstat(output_fd, &sto) < 0)
                        తిరిగి -errno;

                r = stat_verify_char(&sto);
                ఒకవేళ (r < 0)
                        తిరిగి r;

                ఒకవేళ (sti.st_rdev != sto.st_rdev)
                        తిరిగి -ENOLINK;
        }

        ఒకవేళ (!isatty_సురక్షిత(ఆదానం_fd)) /* The check above was just for char device, but now let's ensure it's actually a tty */
                తిరిగి -ENOTTY;

        తిరిగి 0;
}

రకనిర్వచనం గణన CursorPositionState {
        CURSOR_పాఠ్యం,
        CURSOR_ESCAPE,
        CURSOR_అడ్డువరుస,
        CURSOR_స్తంభం,
} CursorPositionState;

రకనిర్వచనం నిర్మాణం CursorPositionContext {
        CursorPositionState state;
        గుర్తులేని అడ్డువరుస, స్తంభం;
} CursorPositionContext;

స్థిర పూర్ణాంకం పరిశీలన_cursor_position_response(
                CursorPositionContext *context,
                స్థిరరాశి అక్షరం *buf,
                size_t కొలత,
                size_t *ret_processed) {

        ప్రతిజ్ఞా(context);
        ప్రతిజ్ఞా(buf);
        ప్రతిజ్ఞా(ret_processed);

        కొరకు (size_t i = 0; i < కొలత; i++) {
                అక్షరం c = buf[i];

                ఎంపిక (context->state) {

                స్థితి CURSOR_పాఠ్యం:
                        context->state = c == '\x1B' ? CURSOR_ESCAPE : CURSOR_పాఠ్యం;
                        ఆపు;

                స్థితి CURSOR_ESCAPE:
                        context->state = c == '[' ? CURSOR_అడ్డువరుస : CURSOR_పాఠ్యం;
                        ఆపు;

                స్థితి CURSOR_అడ్డువరుస:
                        ఒకవేళ (c == ';')
                                context->state = context->అడ్డువరుస > 0 ? CURSOR_స్తంభం : CURSOR_పాఠ్యం;
                        లేకపోతే {
                                పూర్ణాంకం d = undecchar(c);

                                /* We read a decimal character, let's suffix it to the number we so far read,
                                 * but let's do an overflow check first. */
                                ఒకవేళ (d < 0 || context->అడ్డువరుస > (UINT_MAX-d)/10)
                                        context->state = CURSOR_పాఠ్యం;
                                లేకపోతే
                                        context->అడ్డువరుస = context->అడ్డువరుస * 10 + d;
                        }
                        ఆపు;

                స్థితి CURSOR_స్తంభం:
                        ఒకవేళ (c == 'R') {
                                ఒకవేళ (context->స్తంభం > 0) {
                                        *ret_processed = i + 1;
                                        తిరిగి 1; /* success! */
                                }

                                context->state = CURSOR_పాఠ్యం;
                        } లేకపోతే {
                                పూర్ణాంకం d = undecchar(c);

                                /* As above, add the decimal character to our column number */
                                ఒకవేళ (d < 0 || context->స్తంభం > (UINT_MAX-d)/10)
                                        context->state = CURSOR_పాఠ్యం;
                                లేకపోతే
                                        context->స్తంభం = context->స్తంభం * 10 + d;
                        }

                        ఆపు;
                }

                /* Reset any positions we might have picked up */
                ఒకవేళ (లోపల_సమితి(context->state, CURSOR_పాఠ్యం, CURSOR_ESCAPE))
                        context->అడ్డువరుస = context->స్తంభం = 0;
        }

        *ret_processed = కొలత;
        తిరిగి 0; /* all good, but not enough data yet */
}

పూర్ణాంకం terminal_get_cursor_position(
                పూర్ణాంకం ఆదానం_fd,
                పూర్ణాంకం output_fd,
                గుర్తులేని *ret_అడ్డువరుస,
                గుర్తులేని *ret_స్తంభం) {

        _cleanup_close_ పూర్ణాంకం nonblock_ఆదానం_fd = -EBADF;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(ఆదానం_fd >= 0);
        ప్రతిజ్ఞా(output_fd >= 0);

        ఒకవేళ (getenv_terminal_is_dumb())
                తిరిగి -EOPNOTSUPP;

        r = terminal_verify_same(ఆదానం_fd, output_fd);
        ఒకవేళ (r < 0)
                తిరిగి log_debug_errno(r, "Called with distinct input/output fds: %m");

        /* Failure to reset the terminal is ignored here and in similar cases below.
         * We already have our result; if cleanup fails it doesn't change the validity of the result. */
        నిర్మాణం termios old_termios = TERMIOS_NULL;
        CLEANUP_TERMIOS_RESET(ఆదానం_fd, old_termios);

        ఒకవేళ (tcgetattr(ఆదానం_fd, &old_termios) < 0)
                తిరిగి log_debug_errno(errno, "Failed to get terminal settings: %m");

        నిర్మాణం termios new_termios = old_termios;
        termios_disable_echo(&new_termios);

        ఒకవేళ (tcsetattr(ఆదానం_fd, TCSANOW, &new_termios) < 0)
                తిరిగి log_debug_errno(errno, "Failed to set new terminal settings: %m");

        /* Request cursor position (DSR/CPR) */
        r = loop_write(output_fd, "\x1B[6n", SIZE_MAX);
        ఒకవేళ (r < 0)
                తిరిగి r;

        /* Open a 2nd input fd, in non-blocking mode, so that we won't ever hang in read() should someone
         * else process the POLLIN. */

        nonblock_ఆదానం_fd = r = fd_reopen(ఆదానం_fd, O_RDONLY|O_CLOEXEC|O_NONBLOCK|O_NOCTTY);
        ఒకవేళ (r < 0)
                తిరిగి r;

        usec_t చివర = usec_కూడు(now(CLOCK_MONOTONIC), CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        అక్షరం buf[STRLEN("\x1B[1;1R")]; /* The shortest valid reply possible */
        size_t buf_నిండిన = 0;
        CursorPositionContext context = {};

        కొరకు (bool మొదటి = true;; మొదటి = false) {
                ఒకవేళ (buf_నిండిన == 0) {
                        usec_t n = now(CLOCK_MONOTONIC);
                        ఒకవేళ (n >= చివర)
                                తిరిగి -EOPNOTSUPP;

                        r = fd_wait_for_event(nonblock_ఆదానం_fd, POLLIN, usec_sub_unsigned(చివర, n));
                        ఒకవేళ (r < 0)
                                తిరిగి r;
                        ఒకవేళ (r == 0)
                                తిరిగి -EOPNOTSUPP;

                        /* On the first try, read multiple characters, i.e. the shortest valid
                         * reply. Afterwards read byte-wise, since we don't want to read too much, and
                         * unnecessarily drop too many characters from the input queue. */
                        ssize_t l = పఠ(nonblock_ఆదానం_fd, buf, మొదటి ? పరిమాణం(buf) : 1);
                        ఒకవేళ (l < 0) {
                                ఒకవేళ (errno == EAGAIN)
                                        కొనసాగు;

                                తిరిగి -errno;
                        }

                        ప్రతిజ్ఞా((size_t) l <= పరిమాణం(buf));
                        buf_నిండిన = l;
                }

                size_t processed;
                r = పరిశీలన_cursor_position_response(&context, buf, buf_నిండిన, &processed);
                ఒకవేళ (r < 0)
                        తిరిగి r;

                ప్రతిజ్ఞా(processed <= buf_నిండిన);
                buf_నిండిన -= processed;
                memmove(buf, buf + processed, buf_నిండిన);

                ఒకవేళ (r > 0) {
                        /* Superficial validity check */
                        ఒకవేళ (context.అడ్డువరుస >= 32766 || context.స్తంభం >= 32766)
                                తిరిగి -ENODATA;

                        ఒకవేళ (ret_అడ్డువరుస)
                                *ret_అడ్డువరుస = context.అడ్డువరుస;
                        ఒకవేళ (ret_స్తంభం)
                                *ret_స్తంభం = context.స్తంభం;

                        తిరిగి 0;
                }
        }
}

పూర్ణాంకం terminal_reset_defensive(పూర్ణాంకం fd, TerminalResetFlags flags) {
        పూర్ణాంకం r = 0;

        ప్రతిజ్ఞా(fd >= 0);
        ప్రతిజ్ఞా(!FLAGS_సమితి(flags, TERMINAL_RESET_AVOID_ANSI_SEQ|TERMINAL_RESET_FORCE_ANSI_SEQ));

        /* Resets the terminal comprehensively, i.e. via both ioctl()s and via ANSI sequences, but do so only
         * if $TERM is unset or set to "dumb" */

        ఒకవేళ (!isatty_సురక్షిత(fd))
                తిరిగి -ENOTTY;

        RET_GATHER(r, terminal_reset_ioctl(fd, FLAGS_సమితి(flags, TERMINAL_RESET_SWITCH_గమ్యం_పాఠ్యం)));

        ఒకవేళ (!FLAGS_సమితి(flags, TERMINAL_RESET_AVOID_ANSI_SEQ) &&
            (FLAGS_సమితి(flags, TERMINAL_RESET_FORCE_ANSI_SEQ) || !getenv_terminal_is_dumb()))
                RET_GATHER(r, terminal_reset_ansi_seq(fd));

        తిరిగి r;
}

పూర్ణాంకం terminal_reset_defensive_locked(పూర్ణాంకం fd, TerminalResetFlags flags) {
        ప్రతిజ్ఞా(fd >= 0);

        _cleanup_close_ పూర్ణాంకం తాళం_fd = తాళం_dev_console();
        ఒకవేళ (తాళం_fd < 0)
                log_debug_errno(తాళం_fd, "Failed to acquire lock for /dev/console, ignoring: %m");

        తిరిగి terminal_reset_defensive(fd, flags);
}

శూన్య termios_disable_echo(నిర్మాణం termios *termios) {
        ప్రతిజ్ఞా(termios);

        termios->c_lflag &= ~(ICANON|ECHO);
        termios->c_cc[VMIN] = 1;
        termios->c_cc[VTIME] = 0;
}

స్థిర bool termios_is_null(స్థిరరాశి నిర్మాణం termios *t) {
        ఒకవేళ (!t)
                తిరిగి true;

        తిరిగి t->c_iflag == UINT_MAX &&
               t->c_oflag == UINT_MAX &&
               t->c_cflag == UINT_MAX &&
               t->c_lflag == UINT_MAX;
}

శూన్య termios_reset(స్థిరరాశి TermiosResetContext *c) {
        ప్రతిజ్ఞా(c);

        PROTECT_ERRNO;

        ఒకవేళ (c->fd && *c->fd >= 0 && !termios_is_null(c->termios))
                (శూన్య) tcsetattr(*c->fd, TCSANOW, c->termios);
}

రకనిర్వచనం గణన BackgroundColorState {
        BACKGROUND_పాఠ్యం,
        BACKGROUND_ESCAPE,
        BACKGROUND_BRACKET,
        BACKGROUND_మొదటి_ONE,
        BACKGROUND_రెండవ_ONE,
        BACKGROUND_SEMICOLON,
        BACKGROUND_R,
        BACKGROUND_G,
        BACKGROUND_B,
        BACKGROUND_RED,
        BACKGROUND_GREEN,
        BACKGROUND_BLUE,
        BACKGROUND_STRING_TERMINATOR,
} BackgroundColorState;

రకనిర్వచనం నిర్మాణం BackgroundColorContext {
        BackgroundColorState state;
        uint32_t red, green, blue;
        గుర్తులేని red_బిట్లు, green_బిట్లు, blue_బిట్లు;
} BackgroundColorContext;

స్థిర పూర్ణాంకం పరిశీలన_background_color_response(
                BackgroundColorContext *context,
                స్థిరరాశి అక్షరం *buf,
                size_t కొలత,
                size_t *ret_processed) {

        ప్రతిజ్ఞా(context);
        ప్రతిజ్ఞా(buf);
        ప్రతిజ్ఞా(ret_processed);

        కొరకు (size_t i = 0; i < కొలత; i++) {
                అక్షరం c = buf[i];

                ఎంపిక (context->state) {

                స్థితి BACKGROUND_పాఠ్యం:
                        context->state = c == '\x1B' ? BACKGROUND_ESCAPE : BACKGROUND_పాఠ్యం;
                        ఆపు;

                స్థితి BACKGROUND_ESCAPE:
                        context->state = c == ']' ? BACKGROUND_BRACKET : BACKGROUND_పాఠ్యం;
                        ఆపు;

                స్థితి BACKGROUND_BRACKET:
                        context->state = c == '1' ? BACKGROUND_మొదటి_ONE : BACKGROUND_పాఠ్యం;
                        ఆపు;

                స్థితి BACKGROUND_మొదటి_ONE:
                        context->state = c == '1' ? BACKGROUND_రెండవ_ONE : BACKGROUND_పాఠ్యం;
                        ఆపు;

                స్థితి BACKGROUND_రెండవ_ONE:
                        context->state = c == ';' ? BACKGROUND_SEMICOLON : BACKGROUND_పాఠ్యం;
                        ఆపు;

                స్థితి BACKGROUND_SEMICOLON:
                        context->state = c == 'r' ? BACKGROUND_R : BACKGROUND_పాఠ్యం;
                        ఆపు;

                స్థితి BACKGROUND_R:
                        context->state = c == 'g' ? BACKGROUND_G : BACKGROUND_పాఠ్యం;
                        ఆపు;

                స్థితి BACKGROUND_G:
                        context->state = c == 'b' ? BACKGROUND_B : BACKGROUND_పాఠ్యం;
                        ఆపు;

                స్థితి BACKGROUND_B:
                        context->state = c == ':' ? BACKGROUND_RED : BACKGROUND_పాఠ్యం;
                        ఆపు;

                స్థితి BACKGROUND_RED:
                        ఒకవేళ (c == '/')
                                context->state = context->red_బిట్లు > 0 ? BACKGROUND_GREEN : BACKGROUND_పాఠ్యం;
                        లేకపోతే {
                                పూర్ణాంకం d = unhexchar(c);
                                ఒకవేళ (d < 0 || context->red_బిట్లు >= పరిమాణం(context->red)*8)
                                        context->state = BACKGROUND_పాఠ్యం;
                                లేకపోతే {
                                        context->red = (context->red << 4) | d;
                                        context->red_బిట్లు += 4;
                                }
                        }
                        ఆపు;

                స్థితి BACKGROUND_GREEN:
                        ఒకవేళ (c == '/')
                                context->state = context->green_బిట్లు > 0 ? BACKGROUND_BLUE : BACKGROUND_పాఠ్యం;
                        లేకపోతే {
                                పూర్ణాంకం d = unhexchar(c);
                                ఒకవేళ (d < 0 || context->green_బిట్లు >= పరిమాణం(context->green)*8)
                                        context->state = BACKGROUND_పాఠ్యం;
                                లేకపోతే {
                                        context->green = (context->green << 4) | d;
                                        context->green_బిట్లు += 4;
                                }
                        }
                        ఆపు;

                స్థితి BACKGROUND_BLUE:
                        ఒకవేళ (c == '\x07') {
                                ఒకవేళ (context->blue_బిట్లు > 0) {
                                        *ret_processed = i + 1;
                                        తిరిగి 1; /* success! */
                                }

                                context->state = BACKGROUND_పాఠ్యం;
                        } లేకపోతే ఒకవేళ (c == '\x1b')
                                context->state = context->blue_బిట్లు > 0 ? BACKGROUND_STRING_TERMINATOR : BACKGROUND_పాఠ్యం;
                        లేకపోతే {
                                పూర్ణాంకం d = unhexchar(c);
                                ఒకవేళ (d < 0 || context->blue_బిట్లు >= పరిమాణం(context->blue)*8)
                                        context->state = BACKGROUND_పాఠ్యం;
                                లేకపోతే {
                                        context->blue = (context->blue << 4) | d;
                                        context->blue_బిట్లు += 4;
                                }
                        }
                        ఆపు;

                స్థితి BACKGROUND_STRING_TERMINATOR:
                        ఒకవేళ (c == '\\') {
                                *ret_processed = i + 1;
                                తిరిగి 1; /* success! */
                        }

                        context->state = c == ']' ? BACKGROUND_ESCAPE : BACKGROUND_పాఠ్యం;
                        ఆపు;

                }

                /* Reset any colors we might have picked up */
                ఒకవేళ (లోపల_సమితి(context->state, BACKGROUND_పాఠ్యం, BACKGROUND_ESCAPE)) {
                        /* reset color */
                        context->red = context->green = context->blue = 0;
                        context->red_బిట్లు = context->green_బిట్లు = context->blue_బిట్లు = 0;
                }
        }

        *ret_processed = కొలత;
        తిరిగి 0; /* all good, but not enough data yet */
}

పూర్ణాంకం get_default_background_color(ద్విగుణ *ret_red, ద్విగుణ *ret_green, ద్విగుణ *ret_blue) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(ret_red);
        ప్రతిజ్ఞా(ret_green);
        ప్రతిజ్ఞా(ret_blue);

        ఒకవేళ (!colors_enabled())
                తిరిగి -EOPNOTSUPP;

        r = terminal_verify_same(STDIN_FILENO, STDOUT_FILENO);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (streq_ptr(getenv("TERM"), "linux")) {
                /* Linux console is black */
                *ret_red = *ret_green = *ret_blue = 0.0;
                తిరిగి 0;
        }

        /* Open a 2nd input fd, in non-blocking mode, so that we won't ever hang in read()
         * should someone else process the POLLIN. Do all subsequent operations on the new fd. */
        _cleanup_close_ పూర్ణాంకం nonblock_ఆదానం_fd = r = fd_reopen(STDIN_FILENO, O_RDONLY|O_CLOEXEC|O_NONBLOCK|O_NOCTTY);
        ఒకవేళ (r < 0)
                తిరిగి r;

        నిర్మాణం termios old_termios = TERMIOS_NULL;
        CLEANUP_TERMIOS_RESET(nonblock_ఆదానం_fd, old_termios);

        ఒకవేళ (tcgetattr(nonblock_ఆదానం_fd, &old_termios) < 0)
                తిరిగి -errno;

        నిర్మాణం termios new_termios = old_termios;
        termios_disable_echo(&new_termios);

        ఒకవేళ (tcsetattr(nonblock_ఆదానం_fd, TCSANOW, &new_termios) < 0)
                తిరిగి -errno;

        r = loop_write(STDOUT_FILENO, ANSI_OSC "11;?" ANSI_ST, SIZE_MAX);
        ఒకవేళ (r < 0)
                తిరిగి r;

        usec_t చివర = usec_కూడు(now(CLOCK_MONOTONIC), CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        అక్షరం buf[STRLEN(ANSI_OSC "11;rgb:0/0/0" ANSI_ST)]; /* shortest possible reply */
        size_t buf_నిండిన = 0;
        BackgroundColorContext context = {};

        కొరకు (bool మొదటి = true;; మొదటి = false) {
                ఒకవేళ (buf_నిండిన == 0) {
                        usec_t n = now(CLOCK_MONOTONIC);
                        ఒకవేళ (n >= చివర)
                                తిరిగి -EOPNOTSUPP;

                        r = fd_wait_for_event(nonblock_ఆదానం_fd, POLLIN, usec_sub_unsigned(చివర, n));
                        ఒకవేళ (r < 0)
                                తిరిగి r;
                        ఒకవేళ (r == 0)
                                తిరిగి -EOPNOTSUPP;

                        /* On the first try, read multiple characters, i.e. the shortest valid
                         * reply. Afterwards read byte-wise, since we don't want to read too much, and
                         * unnecessarily drop too many characters from the input queue. */
                        ssize_t l = పఠ(nonblock_ఆదానం_fd, buf, మొదటి ? పరిమాణం(buf) : 1);
                        ఒకవేళ (l < 0) {
                                ఒకవేళ (errno == EAGAIN)
                                        కొనసాగు;
                                తిరిగి -errno;
                        }

                        ప్రతిజ్ఞా((size_t) l <= పరిమాణం(buf));
                        buf_నిండిన = l;
                }

                size_t processed;
                r = పరిశీలన_background_color_response(&context, buf, buf_నిండిన, &processed);
                ఒకవేళ (r < 0)
                        తిరిగి r;

                ప్రతిజ్ఞా(processed <= buf_నిండిన);
                buf_నిండిన -= processed;
                memmove(buf, buf + processed, buf_నిండిన);

                ఒకవేళ (r > 0) {
                        ప్రతిజ్ఞా(context.red_బిట్లు > 0);
                        *ret_red = (ద్విగుణ) context.red / ((UINT64_C(1) << context.red_బిట్లు) - 1);
                        ప్రతిజ్ఞా(context.green_బిట్లు > 0);
                        *ret_green = (ద్విగుణ) context.green / ((UINT64_C(1) << context.green_బిట్లు) - 1);
                        ప్రతిజ్ఞా(context.blue_బిట్లు > 0);
                        *ret_blue = (ద్విగుణ) context.blue / ((UINT64_C(1) << context.blue_బిట్లు) - 1);
                        తిరిగి 0;
                }
        }
}

/* Determine terminal dimensions by means of ANSI sequences: save the cursor via DECSC, position it far
 * to the bottom right (clamped to actual terminal dimensions), read back via DSR where we ended up, and
 * restore cursor via DECRC. Only needs a single DSR round-trip, and always restores the cursor regardless
 * of whether the response is received.
 *
 * Caller must have already opened a non-blocking input fd and configured termios (echo/icanon off). */
స్థిర పూర్ణాంకం terminal_query_కొలత_by_dsr(
                పూర్ణాంకం nonblock_ఆదానం_fd,
                పూర్ణాంకం output_fd,
                గుర్తులేని *ret_అడ్డువరుసలు,
                గుర్తులేని *ret_columns) {

        పూర్ణాంకం r;

        ప్రతిజ్ఞా(nonblock_ఆదానం_fd >= 0);
        ప్రతిజ్ఞా(output_fd >= 0);

        /* Use DECSC/DECRC to save/restore cursor instead of querying position via DSR. This way the cursor
         * is always restored — even on timeout — and we only need one DSR response instead of two. */
        r = loop_write_నిండిన(output_fd,
                            "\x1B" "7"              /* DECSC: save cursor position */
                            "\x1B[32766;32766H"     /* CUP: position cursor far to the right and to the bottom, staying within 16bit signed range */
                            "\x1B[6n"               /* DSR: request cursor position (CPR) */
                            "\x1B" "8",             /* DECRC: restore cursor position */
                            SIZE_MAX,
                            CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        ఒకవేళ (r < 0)
                తిరిగి r;

        usec_t చివర = usec_కూడు(now(CLOCK_MONOTONIC), CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        అక్షరం buf[STRLEN("\x1B[1;1R")]; /* The shortest valid reply possible */
        size_t buf_నిండిన = 0;
        CursorPositionContext context = {};

        కొరకు (bool మొదటి = true;; మొదటి = false) {
                ఒకవేళ (buf_నిండిన == 0) {
                        usec_t n = now(CLOCK_MONOTONIC);
                        ఒకవేళ (n >= చివర)
                                తిరిగి -EOPNOTSUPP;

                        r = fd_wait_for_event(nonblock_ఆదానం_fd, POLLIN, usec_sub_unsigned(చివర, n));
                        ఒకవేళ (r < 0)
                                తిరిగి r;
                        ఒకవేళ (r == 0)
                                తిరిగి -EOPNOTSUPP;

                        /* On the first try, read multiple characters, i.e. the shortest valid
                         * reply. Afterwards read byte-wise, since we don't want to read too much, and
                         * unnecessarily drop too many characters from the input queue. */
                        ssize_t l = పఠ(nonblock_ఆదానం_fd, buf, మొదటి ? పరిమాణం(buf) : 1);
                        ఒకవేళ (l < 0) {
                                ఒకవేళ (errno == EAGAIN)
                                        కొనసాగు;

                                తిరిగి -errno;
                        }

                        ప్రతిజ్ఞా((size_t) l <= పరిమాణం(buf));
                        buf_నిండిన = l;
                }

                size_t processed;
                r = పరిశీలన_cursor_position_response(&context, buf, buf_నిండిన, &processed);
                ఒకవేళ (r < 0)
                        తిరిగి r;

                ప్రతిజ్ఞా(processed <= buf_నిండిన);
                buf_నిండిన -= processed;
                memmove(buf, buf + processed, buf_నిండిన);

                ఒకవేళ (r > 0) {
                        /* Superficial validity checks (no particular reason to check for < 4, it's
                         * just a way to look for unreasonably small values) */
                        ఒకవేళ (context.అడ్డువరుస < 4 || context.స్తంభం < 4 || context.అడ్డువరుస >= 32766 || context.స్తంభం >= 32766)
                                తిరిగి -ENODATA;

                        ఒకవేళ (ret_అడ్డువరుసలు)
                                *ret_అడ్డువరుసలు = context.అడ్డువరుస;
                        ఒకవేళ (ret_columns)
                                *ret_columns = context.స్తంభం;

                        తిరిగి 0;
                }
        }
}

/* Common setup for ANSI terminal queries: validate the fds, open a non-blocking input fd, and configure
 * termios with echo and canonical mode disabled. Caller must restore termios and close the fd when done. */
స్థిర పూర్ణాంకం terminal_prepare_query(
                పూర్ణాంకం ఆదానం_fd,
                పూర్ణాంకం output_fd,
                పూర్ణాంకం *ret_nonblock_fd,
                నిర్మాణం termios *ret_saved_termios) {

        పూర్ణాంకం r;

        ప్రతిజ్ఞా(ఆదానం_fd >= 0);
        ప్రతిజ్ఞా(output_fd >= 0);
        ప్రతిజ్ఞా(ret_nonblock_fd);
        ప్రతిజ్ఞా(ret_saved_termios);

        /* Use getenv_terminal_is_dumb() instead of terminal_is_dumb() here since we operate on an
         * explicitly passed fd, not on stdio. terminal_is_dumb() additionally checks on_tty() which
         * tests whether *stderr* is a tty — that's irrelevant when we're querying a directly opened
         * terminal such as /dev/console. */
        ఒకవేళ (getenv_terminal_is_dumb())
                తిరిగి -EOPNOTSUPP;

        r = terminal_verify_same(ఆదానం_fd, output_fd);
        ఒకవేళ (r < 0)
                తిరిగి log_debug_errno(r, "Called with distinct input/output fds: %m");

        /* Open a 2nd input fd, in non-blocking mode, so that we won't ever hang in read()
         * should someone else process the POLLIN. Do all subsequent operations on the new fd. */
        _cleanup_close_ పూర్ణాంకం nonblock_ఆదానం_fd = r = fd_reopen(ఆదానం_fd, O_RDONLY|O_CLOEXEC|O_NONBLOCK|O_NOCTTY);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (tcgetattr(nonblock_ఆదానం_fd, ret_saved_termios) < 0)
                తిరిగి log_debug_errno(errno, "Failed to get terminal settings: %m");

        నిర్మాణం termios new_termios = *ret_saved_termios;
        termios_disable_echo(&new_termios);

        ఒకవేళ (tcsetattr(nonblock_ఆదానం_fd, TCSANOW, &new_termios) < 0)
                తిరిగి log_debug_errno(errno, "Failed to set new terminal settings: %m");

        *ret_nonblock_fd = TAKE_FD(nonblock_ఆదానం_fd);
        తిరిగి 0;
}

/*
 * See https://terminalguide.namepad.de/seq/csi_st-18/,
 * https://invisible-island.net/xterm/ctlseqs/ctlseqs.html#h3-Functions-using-CSI-_-ordered-by-the-final-character_s_.
 */
#నిర్వచించు CSI18_Q  "\x1B[18t"               /* Report the size of the text area in characters */
#నిర్వచించు CSI18_Rp "\x1B[8;"                /* Reply prefix */
#నిర్వచించు CSI18_R0 CSI18_Rp "1;1t"          /* Shortest reply */
#నిర్వచించు CSI18_R1 CSI18_Rp "32766;32766t"  /* Longest reply */

స్థిర పూర్ణాంకం పరిశీలన_పాఠ్యం_area_కొలత_response(
                స్థిరరాశి అక్షరం *buf,
                size_t కొలత,
                గుర్తులేని *ret_అడ్డువరుసలు,
                గుర్తులేని *ret_columns) {

        ప్రతిజ్ఞా(buf);
        ప్రతిజ్ఞా(ret_అడ్డువరుసలు);
        ప్రతిజ్ఞా(ret_columns);

        /* Check if we have enough space for the shortest possible answer. */
        ఒకవేళ (కొలత < STRLEN(CSI18_R0))
                తిరిగి -EAGAIN;

        /* Check if the terminating sequence is present */
        ఒకవేళ (buf[కొలత - 1] != 't')
                తిరిగి -EAGAIN;

        గుర్తులేని పొట్టి అడ్డువరుసలు, columns;
        ఒకవేళ (అక్షరశ్రేణి_చదువు(buf, CSI18_Rp "%hu;%hut", &అడ్డువరుసలు, &columns) != 2)
                తిరిగి -EINVAL;

        *ret_అడ్డువరుసలు = అడ్డువరుసలు;
        *ret_columns = columns;
        తిరిగి 0;
}

/* Determine terminal dimensions by means of an ANSI CSI 18 sequence.
 *
 * Caller must have already opened a non-blocking input fd and configured termios (echo/icanon off). */
స్థిర పూర్ణాంకం terminal_query_కొలత_by_csi18(
                పూర్ణాంకం nonblock_ఆదానం_fd,
                పూర్ణాంకం output_fd,
                గుర్తులేని *ret_అడ్డువరుసలు,
                గుర్తులేని *ret_columns) {

        పూర్ణాంకం r;

        ప్రతిజ్ఞా(nonblock_ఆదానం_fd >= 0);
        ప్రతిజ్ఞా(output_fd >= 0);

        r = loop_write_నిండిన(output_fd, CSI18_Q, SIZE_MAX, CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        ఒకవేళ (r < 0)
                తిరిగి r;

        usec_t చివర = usec_కూడు(now(CLOCK_MONOTONIC), CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        అక్షరం buf[STRLEN(CSI18_R1)];
        size_t బైట్లు = 0;

        కొరకు (;;) {
                usec_t n = now(CLOCK_MONOTONIC);
                ఒకవేళ (n >= చివర)
                        తిరిగి -EOPNOTSUPP;

                r = fd_wait_for_event(nonblock_ఆదానం_fd, POLLIN, usec_sub_unsigned(చివర, n));
                ఒకవేళ (r < 0)
                        తిరిగి r;
                ఒకవేళ (r == 0)
                        తిరిగి -EOPNOTSUPP;

                /* On the first read, read multiple characters, i.e. the shortest valid reply. Afterwards
                 * read byte by byte, since we don't want to read too much and drop characters from the input
                 * queue. */
                ssize_t l = పఠ(nonblock_ఆదానం_fd, buf + బైట్లు, బైట్లు == 0 ? STRLEN(CSI18_R0) : 1);
                ఒకవేళ (l < 0) {
                        ఒకవేళ (errno == EAGAIN)
                                కొనసాగు;
                        తిరిగి -errno;
                }

                ప్రతిజ్ఞా((size_t) l <= పరిమాణం(buf) - బైట్లు);
                బైట్లు += l;

                r = పరిశీలన_పాఠ్యం_area_కొలత_response(buf, బైట్లు, ret_అడ్డువరుసలు, ret_columns);
                ఒకవేళ (r != -EAGAIN)
                        తిరిగి r;

                ఒకవేళ (బైట్లు == పరిమాణం(buf))
                        తిరిగి -EOPNOTSUPP; /* The response has the right prefix, but we didn't find a valid
                                             * answer with a terminator in the allotted space. Something is
                                             * wrong, possibly some unrelated bytes got injected into the
                                             * answer. */
        }
}

పూర్ణాంకం terminal_get_కొలత(
                పూర్ణాంకం ఆదానం_fd,
                పూర్ణాంకం output_fd,
                గుర్తులేని *ret_అడ్డువరుసలు,
                గుర్తులేని *ret_columns,
                bool try_dsr,
                bool try_csi18) {

        _cleanup_close_ పూర్ణాంకం nonblock_ఆదానం_fd = -EBADF;
        నిర్మాణం termios old_termios = TERMIOS_NULL;
        CLEANUP_TERMIOS_RESET(nonblock_ఆదానం_fd, old_termios);
        _cleanup_(nonblock_resetp) పూర్ణాంకం nonblock_reset = -EBADF;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(try_dsr || try_csi18);

        r = terminal_prepare_query(ఆదానం_fd, output_fd, &nonblock_ఆదానం_fd, &old_termios);
        ఒకవేళ (r < 0)
                తిరిగి r;

        /* Put the output fd in non-blocking mode with a write timeout, to avoid blocking indefinitely on
         * write if the terminal is not consuming data (e.g. serial console with flow control). */
        r = fd_nonblock(output_fd, true);
        ఒకవేళ (r < 0)
                తిరిగి log_debug_errno(r, "Failed to set terminal to non-blocking mode: %m");
        ఒకవేళ (r > 0)
                nonblock_reset = output_fd;

        /* Flush any stale input that might confuse the response parsers. */
        (శూన్య) tcflush(nonblock_ఆదానం_fd, TCIFLUSH);

        ఒకవేళ (try_csi18) {
                r = terminal_query_కొలత_by_csi18(nonblock_ఆదానం_fd, output_fd, ret_అడ్డువరుసలు, ret_columns);
                ఒకవేళ (r >= 0)
                        తిరిగి r;

                /* Query failed. Flush any outstanding input. */
                (శూన్య) tcflush(nonblock_ఆదానం_fd, TCIFLUSH);

                ఒకవేళ (!లోపల_సమితి(r, -EOPNOTSUPP, -EINVAL))
                        తిరిగి r;
        }

        ఒకవేళ (try_dsr) {
                r = terminal_query_కొలత_by_dsr(nonblock_ఆదానం_fd, output_fd, ret_అడ్డువరుసలు, ret_columns);
                ఒకవేళ (r >= 0)
                        తిరిగి r;

                /* Query failed. Flush any outstanding input. */
                (శూన్య) tcflush(nonblock_ఆదానం_fd, TCIFLUSH);
        }

        తిరిగి r;
}

స్థిర పూర్ణాంకం fd_is_vc(పూర్ణాంకం fd) {
        _cleanup_free_ అక్షరం *name = ఖాళీ;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(fd >= 0);

        r = getttyname_harder(fd, &name);
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి tty_is_vc_resolve(name);
}

పూర్ణాంకం terminal_fix_కొలత(పూర్ణాంకం ఆదానం_fd, పూర్ణాంకం output_fd) {
        గుర్తులేని అడ్డువరుసలు, columns;
        పూర్ణాంకం r;

        /* Don't ask a virtual console at all. There is no intermediary here that could be wrong about the
         * dimensions: the kernel renders the screen itself, so its winsize is authoritative. */
        r = fd_is_vc(ఆదానం_fd);
        ఒకవేళ (r < 0)
                log_debug_errno(r, "Failed to determine whether terminal is a virtual console, assuming it is not: %m");
        లేకపోతే ఒకవేళ (r > 0) {
                log_debug("Not querying dimensions of a virtual console, kernel dimensions are authoritative.");
                తిరిగి 0;
        }

        నిర్మాణం winsize ws = {};
        ఒకవేళ (ioctl(output_fd, TIOCGWINSZ, &ws) < 0)
                తిరిగి log_debug_errno(errno, "Failed to query terminal dimensions, ignoring: %m");

        r = terminal_get_కొలత(ఆదానం_fd, output_fd, &అడ్డువరుసలు, &columns, /* try_dsr= */ true, /* try_csi18= */ true);
        ఒకవేళ (r < 0)
                తిరిగి log_debug_errno(r, "Failed to acquire terminal dimensions via ANSI sequences, not adjusting terminal dimensions: %m");

        ఒకవేళ (ws.ws_row == అడ్డువరుసలు && ws.ws_col == columns) {
                log_debug("Terminal dimensions reported via ANSI sequences match currently set terminal dimensions, not changing.");
                తిరిగి 0;
        }

        ws.ws_col = columns;
        ws.ws_row = అడ్డువరుసలు;

        ఒకవేళ (ioctl(output_fd, TIOCSWINSZ, &ws) < 0)
                తిరిగి log_debug_errno(errno, "Failed to update terminal dimensions, ignoring: %m");

        log_debug("Fixed terminal dimensions to %ux%u based on ANSI sequence information.", columns, అడ్డువరుసలు);
        తిరిగి 1;
}

#నిర్వచించు MAX_TERMINFO_నిడివి 64
/* python -c 'print("".join(hex(ord(i))[2:] for i in "name").upper())' */
#నిర్వచించు DCS_TERMINFO_Q ANSI_DCS "+q" "6E616D65" ANSI_ST
/* The answer is either 0+r… (invalid) or 1+r… (OK). */
#నిర్వచించు DCS_TERMINFO_R0 ANSI_DCS "0+r" ANSI_ST
#నిర్వచించు DCS_TERMINFO_R1 ANSI_DCS "1+r" "6E616D65" "=" /* This is followed by Pt ST. */
assert_cc(STRLEN(DCS_TERMINFO_R0) <= STRLEN(DCS_TERMINFO_R1 ANSI_ST));

స్థిర పూర్ణాంకం పరిశీలన_terminfo_response(
                స్థిరరాశి అక్షరం *buf,
                size_t కొలత,
                అక్షరం **ret_name) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(buf);
        ప్రతిజ్ఞా(ret_name);

        /* Check if we have enough space for the shortest possible answer. */
        ఒకవేళ (కొలత < STRLEN(DCS_TERMINFO_R0))
                తిరిగి -EAGAIN;

        /* Check if the terminating sequence is present */
        ఒకవేళ (memcmp(buf + కొలత - STRLEN(ANSI_ST), ANSI_ST, STRLEN(ANSI_ST)) != 0)
                తిరిగి -EAGAIN;

        ఒకవేళ (కొలత <= STRLEN(DCS_TERMINFO_R1 ANSI_ST))
                తిరిగి -EINVAL;  /* The answer is invalid or empty */

        ఒకవేళ (memcmp(buf, DCS_TERMINFO_R1, STRLEN(DCS_TERMINFO_R1)) != 0)
                తిరిగి -EINVAL;  /* The answer is not valid */

        _cleanup_free_ శూన్య *dec = ఖాళీ;
        size_t dec_కొలత;
        r = unhexmem_నిండిన(buf + STRLEN(DCS_TERMINFO_R1), కొలత - STRLEN(DCS_TERMINFO_R1 ANSI_ST),
                          /* secure= */ false,
                          &dec, &dec_కొలత);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ప్రతిజ్ఞా(((స్థిరరాశి అక్షరం *) dec)[dec_కొలత] == '\0'); /* unhexmem appends NUL for our convenience */
        ఒకవేళ (memchr(dec, '\0', dec_కొలత) || string_has_cc(dec, ఖాళీ) || !filename_is_valid(dec))
                తిరిగి -EUCLEAN;

        *ret_name = TAKE_PTR(dec);
        తిరిగి 0;
}

పూర్ణాంకం terminal_get_terminfo_by_dcs(పూర్ణాంకం fd, అక్షరం **ret_name) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(fd >= 0);
        ప్రతిజ్ఞా(ret_name);

        /* Note: fd must be in non-blocking read-write mode! */

        నిర్మాణం termios old_termios = TERMIOS_NULL;
        CLEANUP_TERMIOS_RESET(fd, old_termios);

        ఒకవేళ (tcgetattr(fd, &old_termios) < 0)
                తిరిగి -errno;

        నిర్మాణం termios new_termios = old_termios;
        termios_disable_echo(&new_termios);

        ఒకవేళ (tcsetattr(fd, TCSANOW, &new_termios) < 0)
                తిరిగి -errno;

        r = loop_write(fd, DCS_TERMINFO_Q, SIZE_MAX);
        ఒకవేళ (r < 0)
                తిరిగి r;

        usec_t చివర = usec_కూడు(now(CLOCK_MONOTONIC), CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        అక్షరం buf[STRLEN(DCS_TERMINFO_R1) + MAX_TERMINFO_నిడివి + STRLEN(ANSI_ST)];
        size_t బైట్లు = 0;

        కొరకు (;;) {
                usec_t n = now(CLOCK_MONOTONIC);
                ఒకవేళ (n >= చివర)
                        తిరిగి -EOPNOTSUPP;

                r = fd_wait_for_event(fd, POLLIN, usec_sub_unsigned(చివర, n));
                ఒకవేళ (r < 0)
                        తిరిగి r;
                ఒకవేళ (r == 0)
                        తిరిగి -EOPNOTSUPP;

                /* On the first read, read multiple characters, i.e. the shortest valid reply. Afterwards
                 * read byte by byte, since we don't want to read too much and drop characters from the input
                 * queue. */
                ssize_t l = పఠ(fd, buf + బైట్లు, బైట్లు == 0 ? STRLEN(DCS_TERMINFO_R0) : 1);
                ఒకవేళ (l < 0) {
                        ఒకవేళ (errno == EAGAIN)
                                కొనసాగు;
                        తిరిగి -errno;
                }

                ప్రతిజ్ఞా((size_t) l <= పరిమాణం(buf) - బైట్లు);
                బైట్లు += l;

                r = పరిశీలన_terminfo_response(buf, బైట్లు, ret_name);
                ఒకవేళ (r != -EAGAIN)
                        తిరిగి r;

                ఒకవేళ (బైట్లు == పరిమాణం(buf))
                        తిరిగి -EOPNOTSUPP; /* The response has the right prefix, but we didn't find a valid
                                             * answer with a terminator in the allotted space. Something is
                                             * wrong, possibly some unrelated bytes got injected into the
                                             * answer. */
        }
}

పూర్ణాంకం have_terminfo_file(స్థిరరాశి అక్షరం *name) {
        /* This is a heuristic check if we have the file, using the directory layout used on
         * current Linux systems. Checks for other layouts can be added later if appropriate. */
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(filename_is_valid(name));

        _cleanup_free_ అక్షరం *p = మార్గం_join("/usr/share/terminfo", CHAR_గమ్యం_STR(name[0]), name);
        ఒకవేళ (!p)
                తిరిగి log_oom_debug();

        r = RET_NERRNO(access(p, F_OK));
        ఒకవేళ (r == -ENOENT)
                తిరిగి false;
        ఒకవేళ (r < 0)
                తిరిగి r;
        తిరిగి true;
}

పూర్ణాంకం query_పదం_for_tty(స్థిరరాశి అక్షరం *tty, అక్షరం **ret_పదం) {
        _cleanup_free_ అక్షరం *dcs_పదం = ఖాళీ;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(tty);
        ప్రతిజ్ఞా(ret_పదం);

        ఒకవేళ (tty_is_vc_resolve(tty))
                తిరిగి strdup_గమ్యం(ret_పదం, "linux");

        /* Try to query the terminal implementation that we're on. This will not work in all
         * cases, which is fine, since this is intended to be used as a fallback. */

        _cleanup_close_ పూర్ణాంకం tty_fd = open_terminal(tty, O_RDWR|O_NOCTTY|O_CLOEXEC|O_NONBLOCK);
        ఒకవేళ (tty_fd < 0)
                తిరిగి log_debug_errno(tty_fd, "Failed to open %s to query terminfo: %m", tty);

        r = terminal_get_terminfo_by_dcs(tty_fd, &dcs_పదం);
        ఒకవేళ (r < 0)
                తిరిగి log_debug_errno(r, "Failed to query %s for terminfo: %m", tty);

        r = have_terminfo_file(dcs_పదం);
        ఒకవేళ (r < 0)
                తిరిగి log_debug_errno(r, "Failed to look for terminfo %s: %m", dcs_పదం);
        ఒకవేళ (r == 0)
                తిరిగి log_info_errno(SYNTHETIC_ERRNO(ENODATA),
                                      "Terminfo %s not found for %s.", dcs_పదం, tty);

        *ret_పదం = TAKE_PTR(dcs_పదం);
        తిరిగి 0;
}

పూర్ణాంకం terminal_is_pty_fd(పూర్ణాంకం fd) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(fd >= 0);

        /* Returns true if we are looking at a pty, i.e. if it's backed by the /dev/pts/ file system */

        ఒకవేళ (!isatty_సురక్షిత(fd))
                తిరిగి false;

        r = is_fs_type_at(fd, ఖాళీ, DEVPTS_SUPER_MAGIC);
        ఒకవేళ (r != 0)
                తిరిగి r;

        /* The ptmx device is weird, it exists twice, once inside and once outside devpts. To detect the
         * latter case, let's fire off an ioctl() that only works on ptmx devices. */

        పూర్ణాంకం v;
        ఒకవేళ (ioctl(fd, TIOCGPKT, &v) < 0) {
                ఒకవేళ (ERRNO_IS_NOT_SUPPORTED(errno))
                        తిరిగి false;

                తిరిగి -errno;
        }

        తిరిగి true;
}

పూర్ణాంకం pty_open_peer(పూర్ణాంకం fd, పూర్ణాంకం బహుళకం) {
        ప్రతిజ్ఞా(fd >= 0);

        /* Opens the peer PTY using the new race-free TIOCGPTPEER ioctl() (kernel 4.13).
         *
         * This is safe to be called on TTYs from other namespaces. */

        ప్రతిజ్ఞా((బహుళకం & (O_CREAT|O_మార్గం|O_DIRECTORY|O_TMPFILE)) == 0);

        /* This replicates the EIO retry logic of open_terminal() in a modified way. */
        కొరకు (గుర్తులేని c = 0;; c++) {
                పూర్ణాంకం peer_fd = ioctl(fd, TIOCGPTPEER, బహుళకం);
                ఒకవేళ (peer_fd >= 0)
                        తిరిగి peer_fd;

                ఒకవేళ (errno != EIO)
                        తిరిగి -errno;

                /* Max 1s in total */
                ఒకవేళ (c >= 20)
                        తిరిగి -EIO;

                (శూన్య) usleep_సురక్షిత(50 * USEC_చొప్పున_MSEC);
        }
}
