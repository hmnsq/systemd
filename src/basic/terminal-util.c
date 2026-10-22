/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <fcntl.h>
#ضمن <linux/kd.h>
#ضمن <linux/magic.h>
#ضمن <linux/tiocl.h>
#ضمن <linux/vt.h>
#ضمن <poll.h>
#ضمن <signal.h>
#ضمن <stdlib.h>
#ضمن <sys/inotify.h>
#ضمن <sys/ioctl.h>
#ضمن <sys/sysmacros.h>
#ضمن <termios.h>
#ضمن <time.h>
#ضمن <unistd.h>

#ضمن "alloc-util.h"
#ضمن "ansi-color.h"
#ضمن "chase.h"
#ضمن "devnum-util.h"
#ضمن "errno-util.h"
#ضمن "extract-word.h"
#ضمن "fd-util.h"
#ضمن "fileio.h"
#ضمن "fs-util.h"
#ضمن "hexdecoct.h"
#ضمن "inotify-util.h"
#ضمن "io-util.h"
#ضمن "log.h"
#ضمن "namespace-util.h"
#ضمن "parse-util.h"
#ضمن "path-util.h"
#ضمن "pidref.h"
#ضمن "proc-cmdline.h"
#ضمن "process-util.h"
#ضمن "signal-util.h"
#ضمن "socket-util.h"
#ضمن "stat-util.h"
#ضمن "stdio-util.h"
#ضمن "string-util.h"
#ضمن "strv.h"
#ضمن "terminal-util.h"
#ضمن "time-util.h"
#ضمن "utf8.h"

/* How much to wait when reading/writing ANSI sequences from/to the console */
#define CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC (333 * USEC_PER_MSEC)

ساكن متطاير unsigned cached_columns = 0;
ساكن متطاير unsigned cached_هێڵەکان = 0;

ساكن متطاير صحيح cached_on_tty = -1;
ساكن متطاير صحيح cached_on_dev_null = -1;

bool isatty_safe(صحيح fd) {
        أكد(fd >= 0);

        إذا (isatty(fd))
                أرجع true;

        /* Linux/glibc returns EIO for hung up TTY on isatty(). Which is wrong, the thing doesn't stop being
         * a TTY after all, just because it is temporarily hung up. Let's work around this here, until this
         * is fixed in glibc. See: https://sourceware.org/bugzilla/show_bug.cgi?id=32103 */
        إذا (errno == EIO)
                أرجع true;

        /* Be resilient if we're working on stdio, since they're set up by parent process. */
        أكد(errno != EBADF || IN_SET(fd, STDIN_FILENO, STDOUT_FILENO, STDERR_FILENO));

        أرجع false;
}

صحيح chvt(صحيح vt) {
        _cleanup_close_ صحيح fd = -EBADF;

        /* Switch to the specified vt number. If the VT is specified <= 0 switch to the VT the kernel log messages go,
         * if that's configured. */

        fd = open_terminal("/dev/tty0", O_RDWR|O_NOCTTY|O_CLOEXEC|O_NONBLOCK);
        إذا (fd < 0)
                أرجع fd;

        إذا (vt <= 0) {
                صحيح tiocl[2] = {
                        TIOCL_GETKMSGREDIRECT,
                        0
                };

                إذا (ioctl(fd, TIOCLINUX, tiocl) < 0)
                        أرجع -errno;

                vt = tiocl[0] <= 0 ? 1 : tiocl[0];
        }

        أرجع RET_NERRNO(ioctl(fd, VT_ACTIVATE, vt));
}

صحيح read_one_char(FILE *f, حرف *ret, usec_t t, bool echo, bool *need_nl) {
        _cleanup_free_ حرف *هێڵ = عدم;
        بنية termios old_termios;
        صحيح r, fd;

        أكد(ret);

        إذا (!f)
                f = stdin;

        /* If this is a terminal, then switch canonical mode off, so that we can read a single
         * character. (Note that fmemopen() streams do not have an fd associated with them, let's handle that
         * nicely.) If 'echo' is false we'll also disable ECHO mode so that the pressed key is not made
         * visible to the user. */
        fd = fileno(f);
        إذا (fd >= 0 && tcgetattr(fd, &old_termios) >= 0) {
                بنية termios new_termios = old_termios;

                new_termios.c_lflag &= ~(ICANON|(echo ? 0 : ECHO));
                new_termios.c_cc[VMIN] = 1;
                new_termios.c_cc[VTIME] = 0;

                إذا (tcsetattr(fd, TCSANOW, &new_termios) >= 0) {
                        حرف c;

                        إذا (t != USEC_INFINITY) {
                                إذا (fd_wait_for_event(fd, POLLIN, t) <= 0) {
                                        (فراغ) tcsetattr(fd, TCSANOW, &old_termios);
                                        أرجع -ETIMEDOUT;
                                }
                        }

                        r = safe_fgetc(f, &c);
                        (فراغ) tcsetattr(fd, TCSANOW, &old_termios);
                        إذا (r < 0)
                                أرجع r;
                        إذا (r == 0)
                                أرجع -EIO;

                        إذا (need_nl)
                                *need_nl = c != '\n';

                        *ret = c;
                        أرجع 0;
                }
        }

        إذا (t != USEC_INFINITY && fd >= 0) {
                /* Let's wait the specified amount of time for input. When we have no fd we skip this, under
                 * the assumption that this is an fmemopen() stream or so where waiting doesn't make sense
                 * anyway, as the data is either already in the stream or cannot possible be placed there
                 * while we access the stream */

                إذا (fd_wait_for_event(fd, POLLIN, t) <= 0)
                        أرجع -ETIMEDOUT;
        }

        /* If this is not a terminal, then read a full line instead */

        r = read_هێڵ(f, 16, &هێڵ); /* longer than necessary, to eat up UTF-8 chars/vt100 key sequences */
        إذا (r < 0)
                أرجع r;
        إذا (r == 0)
                أرجع -EIO;

        إذا (strlen(هێڵ) != 1)
                أرجع -EBADMSG;

        إذا (need_nl)
                *need_nl = false;

        *ret = هێڵ[0];
        أرجع 0;
}

#define DEFAULT_ASK_REFRESH_USEC (2*USEC_PER_SEC)

صحيح ask_char(حرف *ret, ثابت حرف *replies, ثابت حرف *fmt, ...) {
        صحيح r;

        أكد(ret);
        أكد(replies);
        أكد(fmt);

        لكل (;;) {
                va_list ap;
                حرف c;
                bool need_nl = true;

                fputs(ansi_highlight(), stdout);

                putchar('\r');

                va_start(ap, fmt);
                vprintf(fmt, ap);
                va_end(ap);

                fputs(ansi_normal(), stdout);

                fflush(stdout);

                r = read_one_char(stdin, &c, DEFAULT_ASK_REFRESH_USEC, /* echo= */ true, &need_nl);
                إذا (r < 0) {

                        إذا (r == -ETIMEDOUT)
                                استمر;

                        إذا (r == -EBADMSG) {
                                puts("Bad input, please try again.");
                                استمر;
                        }

                        putchar('\n');
                        أرجع r;
                }

                إذا (need_nl)
                        putchar('\n');

                إذا (strchr(replies, c)) {
                        *ret = c;
                        أرجع 0;
                }

                puts("Read unexpected character, please try again.");
        }
}

تعريفنوع تعداد CompletionResult{
        COMPLETION_ALREADY,       /* the input string is already complete */
        COMPLETION_FULL,          /* completed the input string to be complete now */
        COMPLETION_PARTIAL,       /* completed the input string so that is still incomplete */
        COMPLETION_NONE,          /* found no matching completion */
        _COMPLETION_RESULT_MAX,
        _COMPLETION_RESULT_INVALID = -EINVAL,
        _COMPLETION_RESULT_ERRNO_MAX = -ERRNO_MAX,
} CompletionResult;

ساكن CompletionResult pick_completion(ثابت حرف *string, حرف *ثابت*completions, حرف **ret) {
        _cleanup_free_ حرف *found = عدم;
        bool partial = false;

        أكد(ret);

        string = strempty(string);

        STRV_FOREACH(c, completions) {

                /* Ignore entries that are not actually completions */
                إذا (!startswith(*c, string))
                        استمر;

                /* Store first completion that matches */
                إذا (!found) {
                        found = strdup(*c);
                        إذا (!found)
                                أرجع -ENOMEM;

                        استمر;
                }

                /* If there's another completion that works truncate the one we already found by common
                 * prefix */
                size_t n = str_common_prefix(found, *c);
                إذا (n == SIZE_MAX)
                        استمر;

                found[n] = 0;
                partial = true;
        }

        *ret = TAKE_PTR(found);

        إذا (!*ret)
                أرجع COMPLETION_NONE;
        إذا (partial)
                أرجع COMPLETION_PARTIAL;

        أرجع streq(string, *ret) ? COMPLETION_ALREADY : COMPLETION_FULL;
}

ساكن فراغ clear_by_backspace(size_t n) {
        /* Erase the specified number of character cells backwards on the terminal */
        لكل (size_t i = 0; i < n; i++)
                fputs("\b \b", stdout);
}

صحيح ask_string_full(
                حرف **ret,
                ثابت حرف *prefill,
                GetCompletionsCallback get_completions,
                فراغ *userdata,
                ثابت حرف *text, ...) {

        va_list ap;
        صحيح r;

        أكد(ret);
        أكد(text);

        _cleanup_free_ حرف *string = عدم;
        size_t n = 0;

        إذا (prefill) {
                /* Prefill query with explicit data if specified */

                string = strdup(prefill);
                إذا (!string)
                        أرجع -ENOMEM;

                n = strlen(string);

        } وإلا إذا (get_completions) {
                /* Otherwise, figure out what string to preselect the query with */
                _cleanup_strv_free_ حرف **completions = عدم;
                r = get_completions("", GET_COMPLETIONS_PRESELECT, &completions, userdata);
                إذا (r < 0)
                        أرجع r;

                CompletionResult cr = pick_completion(string, completions, &string);
                إذا (cr < 0)
                        أرجع cr;

                n = strlen_ptr(string);
        }

        /* Output the prompt */
        fputs(ansi_highlight(), stdout);
        va_start(ap, text);
        vprintf(text, ap);
        va_end(ap);
        fputs(ansi_normal(), stdout);
        إذا (string)
                fputs(string, stdout);
        fflush(stdout);

        /* Do interactive logic only if stdin + stdout are connected to the same place. And yes, we could use
         * STDIN_FILENO and STDOUT_FILENO here, but let's be overly correct for once, after all libc allows
         * swapping out stdin/stdout. */
        صحيح fd_تێخستن = fileno(stdin);
        صحيح fd_output = fileno(stdout);
        بنية termios old_termios = TERMIOS_NULL;
        CLEANUP_TERMIOS_RESET(fd_تێخستن, old_termios);

        إذا (fd_تێخستن < 0 || fd_output < 0 || same_fd(fd_تێخستن, fd_output) <= 0)
                اذهب fallback;

        /* Try to disable echo, which also tells us if this even is a terminal */
        إذا (tcgetattr(fd_تێخستن, &old_termios) < 0) {
                old_termios = TERMIOS_NULL;
                اذهب fallback;
        }

        بنية termios new_termios = old_termios;
        termios_disable_echo(&new_termios);
        إذا (tcsetattr(fd_تێخستن, TCSANOW, &new_termios) < 0)
                أرجع -errno;

        لكل (;;) {
                صحيح c = fgetc(stdin);

                /* On EOF or NUL, end the request, don't output anything anymore */
                إذا (IN_SET(c, EOF, 0))
                        اكسر;

                /* On Return also end the request, but make this visible */
                إذا (IN_SET(c, '\n', '\r')) {
                        fputc('\n', stdout);
                        اكسر;
                }

                إذا (c == '\t') {
                        /* Tab */

                        _cleanup_strv_free_ حرف **completions = عدم;
                        إذا (get_completions) {
                                r = get_completions(string, /* flags= */ 0, &completions, userdata);
                                إذا (r < 0)
                                        أرجع r;
                        }

                        _cleanup_free_ حرف *new_string = عدم;
                        CompletionResult cr = pick_completion(string, completions, &new_string);
                        إذا (cr < 0)
                                أرجع cr;
                        إذا (IN_SET(cr, COMPLETION_PARTIAL, COMPLETION_FULL)) {
                                /* Output the new suffix we learned */
                                fputs(ASSERT_PTR(startswith(new_string, strempty(string))), stdout);

                                /* And update the whole string */
                                free_and_replace(string, new_string);
                                n = strlen(string);
                        }
                        إذا (cr == COMPLETION_NONE)
                                fputc('\a', stdout); /* BEL */

                        إذا (IN_SET(cr, COMPLETION_PARTIAL, COMPLETION_ALREADY)) {
                                /* If this worked only partially, or if the user hit TAB even though we were
                                 * complete already, then show the remaining options (in the latter case just
                                 * the one). */
                                fputc('\n', stdout);

                                _cleanup_strv_free_ حرف **filtered = strv_filter_prefix(completions, string);
                                إذا (!filtered)
                                        أرجع -ENOMEM;

                                r = show_menu(filtered,
                                              /* n_columns= */ SIZE_MAX,
                                              /* column_width= */ SIZE_MAX,
                                              /* ellipsize_percentage= */ 0,
                                              /* grey_prefix= */ string,
                                              /* with_numbers= */ false);
                                إذا (r < 0)
                                        أرجع r;

                                /* Show the prompt again */
                                fputs(ansi_highlight(), stdout);
                                va_start(ap, text);
                                vprintf(text, ap);
                                va_end(ap);
                                fputs(ansi_normal(), stdout);
                                fputs(string, stdout);
                        }

                } وإلا إذا (IN_SET(c, '\b', 127)) {
                        /* Backspace */

                        إذا (n == 0)
                                fputc('\a', stdout); /* BEL */
                        وإلا {
                                size_t m = utf8_last_length(string, n);

                                حرف *e = string + n - m;
                                clear_by_backspace(utf8_console_width(e));

                                *e = 0;
                                n -= m;
                        }

                } وإلا إذا (c == 21) {
                        /* Ctrl-u → erase all input */

                        clear_by_backspace(utf8_console_width(string));
                        إذا (string)
                                string[n = 0] = 0;
                        وإلا
                                أكد(n == 0);

                } وإلا إذا (c == 4) {
                        /* Ctrl-d → cancel this field input */

                        أرجع -ECANCELED;

                } وإلا إذا (char_is_cc(c) || n >= LINE_MAX)
                        /* refuse control characters and too long strings */
                        fputc('\a', stdout); /* BEL */
                وإلا {
                        /* Regular char */

                        إذا (!GREEDY_REALLOC(string, n+2))
                                أرجع -ENOMEM;

                        string[n++] = (حرف) c;
                        string[n] = 0;

                        fputc(c, stdout);
                }

                fflush(stdout);
        }

        إذا (!string) {
                string = strdup("");
                إذا (!string)
                        أرجع -ENOMEM;
        }

        *ret = TAKE_PTR(string);
        أرجع 0;

fallback:
        /* A simple fallback without TTY magic */
        string = mfree(string);
        r = read_هێڵ(stdin, LONG_هێڵ_MAX, &string);
        إذا (r < 0)
                أرجع r;
        إذا (r == 0)
                أرجع -EIO;

        *ret = TAKE_PTR(string);
        أرجع 0;
}

bool any_کلیل_to_proceed(فراغ) {

        /* Insert a new line here as well as to when the user inputs, as this is also used during the boot up
         * sequence when status messages may be interleaved with the current program output. This ensures
         * that the status messages aren't appended on the same line as this message. */

        fputc('\n', stdout);
        fputs(ansi_highlight_magenta(), stdout);
        fputs("-- Press any key to proceed --", stdout);
        fputs(ansi_normal(), stdout);
        fputc('\n', stdout);
        fflush(stdout);

        حرف کلیل = 0;
        (فراغ) read_one_char(stdin, &کلیل, USEC_INFINITY, /* echo= */ false, /* need_nl= */ عدم);

        fputc('\n', stdout);
        fflush(stdout);

        أرجع کلیل != 'q';
}

ساكن size_t widest_list_element(حرف *ثابت*l) {
        size_t w = 0;

        /* Returns the largest console width of all elements in 'l' */

        STRV_FOREACH(i, l)
                w = MAX(w, utf8_console_width(*i));

        أرجع w;
}

صحيح show_menu(حرف **x,
              size_t n_columns,
              size_t column_width,
              unsigned ellipsize_percentage,
              ثابت حرف *grey_prefix,
              bool with_numbers) {

        أكد(n_columns > 0);
        أكد(column_width > 0);

        إذا (n_columns == SIZE_MAX)
                n_columns = 3;

        إذا (column_width == SIZE_MAX) {
                size_t widest = widest_list_element(x);

                /* If not specified, derive column width from screen width */
                size_t column_max = (columns()-1) / n_columns;

                /* Subtract room for numbers */
                إذا (with_numbers)
                        column_max = LESS_BY(column_max, 6U);

                /* If columns would get too tight let's make this a linear list instead. */
                إذا (column_max < 10 && widest > 10) {
                        n_columns = 1;
                        column_max = columns()-1;

                        إذا (with_numbers)
                                column_max = LESS_BY(column_max, 6U);
                }

                /* Never make this narrower than 10 characters */
                column_max = MAX(column_max, 10U);
                column_width = CLAMP(widest+1, 10U, column_max);
        }

        size_t n = strv_length(x);
        size_t per_column = DIV_ROUND_UP(n, n_columns);

        size_t break_هێڵەکان = هێڵەکان();
        إذا (break_هێڵەکان > 2)
                break_هێڵەکان--;

        /* The first page gets two extra lines, since we want to show a title */
        size_t break_modulo = break_هێڵەکان;
        إذا (break_modulo > 3)
                break_modulo -= 3;

        لكل (size_t i = 0; i < per_column; i++) {

                لكل (size_t j = 0; j < n_columns; j++) {
                        _cleanup_free_ حرف *e = عدم;

                        size_t p = j * per_column + i;
                        إذا (p >= n)
                                اكسر;

                        e = ellipsize(x[p], column_width, ellipsize_percentage);
                        إذا (!e)
                                أرجع -ENOMEM;

                        إذا (with_numbers)
                                printf("%s%4zu)%s ",
                                       ansi_grey(),
                                       p + 1,
                                       ansi_normal());

                        إذا (grey_prefix && startswith(e, grey_prefix)) {
                                size_t k = MIN(strlen(grey_prefix), column_width);
                                printf("%s%.*s%s",
                                       ansi_grey(),
                                       (صحيح) k, e,
                                       ansi_normal());
                                printf("%-*s",
                                       (صحيح) (column_width - k), e+k);
                        } وإلا
                                printf("%-*s", (صحيح) column_width, e);
                }

                putchar('\n');

                /* on the first screen we reserve 2 extra lines for the title */
                إذا (i % break_هێڵەکان == break_modulo)
                        إذا (!any_کلیل_to_proceed())
                                أرجع 0;
        }

        أرجع 0;
}

صحيح open_terminal(ثابت حرف *name, صحيح mode) {
        _cleanup_close_ صحيح fd = -EBADF;

        /*
         * If a TTY is in the process of being closed opening it might cause EIO. This is horribly awful, but
         * unlikely to be changed in the kernel. Hence we work around this problem by retrying a couple of
         * times.
         *
         * https://bugs.launchpad.net/ubuntu/+source/linux/+bug/554172/comments/245
         */

        أكد((mode & (O_CREAT|O_PATH|O_DIRECTORY|O_TMPFILE)) == 0);

        لكل (unsigned c = 0;; c++) {
                fd = open(name, mode, 0);
                إذا (fd >= 0)
                        اكسر;

                إذا (errno != EIO)
                        أرجع -errno;

                /* Max 1s in total */
                إذا (c >= 20)
                        أرجع -EIO;

                (فراغ) usleep_safe(50 * USEC_PER_MSEC);
        }

        إذا (!isatty_safe(fd))
                أرجع -ENOTTY;

        أرجع TAKE_FD(fd);
}

صحيح acquire_terminal(
                ثابت حرف *name,
                AcquireTerminalFlags flags,
                usec_t timeout) {

        _cleanup_close_ صحيح notify = -EBADF, fd = -EBADF;
        usec_t ts = USEC_INFINITY;
        صحيح r, wd = -1;

        أكد(name);

        AcquireTerminalFlags mode = flags & _ACQUIRE_TERMINAL_MODE_MASK;
        أكد(IN_SET(mode, ACQUIRE_TERMINAL_TRY, ACQUIRE_TERMINAL_FORCE, ACQUIRE_TERMINAL_WAIT));
        أكد(mode == ACQUIRE_TERMINAL_WAIT || !FLAGS_SET(flags, ACQUIRE_TERMINAL_WATCH_SIGTERM));

        /* We use inotify to be notified when the tty is closed. We create the watch before checking if we can actually
         * acquire it, so that we don't lose any event.
         *
         * Note: strictly speaking this actually watches for the device being closed, it does *not* really watch
         * whether a tty loses its controlling process. However, unless some rogue process uses TIOCNOTTY on /dev/tty
         * *after* closing its tty otherwise this will not become a problem. As long as the administrator makes sure to
         * not configure any service on the same tty as an untrusted user this should not be a problem. (Which they
         * probably should not do anyway.) */

        إذا (mode == ACQUIRE_TERMINAL_WAIT) {
                notify = inotify_init1(IN_CLOEXEC | IN_NONBLOCK);
                إذا (notify < 0)
                        أرجع -errno;

                wd = inotify_add_watch(notify, name, IN_CLOSE);
                إذا (wd < 0)
                        أرجع -errno;

                إذا (timeout != USEC_INFINITY)
                        ts = now(CLOCK_MONOTONIC);
        }

        /* If we are called with ACQUIRE_TERMINAL_WATCH_SIGTERM we'll unblock SIGTERM during ppoll() temporarily */
        sigset_t poll_ss;
        assert_se(sigprocmask(SIG_SETMASK, /* newset= */ عدم, &poll_ss) >= 0);
        إذا (flags & ACQUIRE_TERMINAL_WATCH_SIGTERM) {
                assert_se(sigismember(&poll_ss, SIGTERM) > 0);
                assert_se(sigdelset(&poll_ss, SIGTERM) >= 0);
        }

        لكل (;;) {
                إذا (notify >= 0) {
                        r = flush_fd(notify);
                        إذا (r < 0)
                                أرجع r;
                }

                /* We pass here O_NOCTTY only so that we can check the return value TIOCSCTTY and have a reliable way
                 * to figure out if we successfully became the controlling process of the tty */
                fd = open_terminal(name, O_RDWR|O_NOCTTY|O_CLOEXEC);
                إذا (fd < 0)
                        أرجع fd;

                /* Temporarily ignore SIGHUP, so that we don't get SIGHUP'ed if we already own the tty. */
                بنية sigaction sa_old;
                assert_se(sigaction(SIGHUP, &sigaction_ignore, &sa_old) >= 0);

                /* First, try to get the tty */
                r = RET_NERRNO(ioctl(fd, TIOCSCTTY, mode == ACQUIRE_TERMINAL_FORCE));

                /* Reset signal handler to old value */
                assert_se(sigaction(SIGHUP, &sa_old, عدم) >= 0);

                /* Success? Exit the loop now! */
                إذا (r >= 0)
                        اكسر;

                /* Any failure besides -EPERM? Fail, regardless of the mode. */
                إذا (r != -EPERM)
                        أرجع r;

                إذا (flags & ACQUIRE_TERMINAL_PERMISSIVE) /* If we are in permissive mode, then EPERM is fine, turn this
                                                          * into a success. Note that EPERM is also returned if we
                                                          * already are the owner of the TTY. */
                        اكسر;

                إذا (mode != ACQUIRE_TERMINAL_WAIT) /* If we are in TRY or FORCE mode, then propagate EPERM as EPERM */
                        أرجع r;

                أكد(notify >= 0);
                أكد(wd >= 0);

                لكل (;;) {
                        usec_t چەپ;
                        إذا (timeout == USEC_INFINITY)
                                چەپ = USEC_INFINITY;
                        وإلا {
                                أكد(ts != USEC_INFINITY);

                                usec_t n = usec_sub_unsigned(now(CLOCK_MONOTONIC), ts);
                                إذا (n >= timeout)
                                        أرجع -ETIMEDOUT;

                                چەپ = timeout - n;
                        }

                        r = ppoll_usec_full(
                                        &(بنية pollfd) {
                                                .fd = notify,
                                                .events = POLLIN,
                                        },
                                        /* n_fds= */ 1,
                                        چەپ,
                                        &poll_ss);
                        إذا (r < 0)
                                أرجع r;
                        إذا (r == 0)
                                أرجع -ETIMEDOUT;

                        اتحاد inotify_event_buffer buffer;
                        ssize_t l;
                        l = اقرأ(notify, &buffer, حجم(buffer));
                        إذا (l < 0) {
                                إذا (ERRNO_IS_TRANSIENT(errno))
                                        استمر;

                                أرجع -errno;
                        }

                        FOREACH_INOTIFY_EVENT(e, buffer, l) {
                                إذا (e->mask & IN_Q_OVERFLOW) /* If we hit an inotify queue overflow, simply check if the terminal is up for grabs now. */
                                        اكسر;

                                إذا (e->wd != wd || !(e->mask & IN_CLOSE)) /* Safety checks */
                                        أرجع -EIO;
                        }

                        اكسر;
                }

                /* We close the tty fd here since if the old session ended our handle will be dead. It's important that
                 * we do this after sleeping, so that we don't enter an endless loop. */
                fd = safe_close(fd);
        }

        أرجع TAKE_FD(fd);
}

صحيح release_terminal(فراغ) {
        _cleanup_close_ صحيح fd = -EBADF;
        صحيح r;

        fd = open("/dev/tty", O_RDWR|O_NOCTTY|O_CLOEXEC|O_NONBLOCK);
        إذا (fd < 0)
                أرجع -errno;

        /* Temporarily ignore SIGHUP, so that we don't get SIGHUP'ed
         * by our own TIOCNOTTY */
        بنية sigaction sa_old;
        assert_se(sigaction(SIGHUP, &sigaction_ignore, &sa_old) >= 0);

        r = RET_NERRNO(ioctl(fd, TIOCNOTTY));

        assert_se(sigaction(SIGHUP, &sa_old, عدم) >= 0);

        أرجع r;
}

صحيح terminal_new_session(فراغ) {

        /* Make us the new session leader, and set stdin tty to be our controlling terminal.
         *
         * Why stdin? Well, the ctty logic is relevant for signal delivery mostly, i.e. if people hit C-c
         * or the line is hung up. Such events are basically just a form of input, via a side channel
         * (that side channel being signal delivery, i.e. SIGINT, SIGHUP et al). Hence we focus on input,
         * not output here. */

        إذا (!isatty_safe(STDIN_FILENO))
                أرجع -ENXIO;

        (فراغ) setsid();
        أرجع RET_NERRNO(ioctl(STDIN_FILENO, TIOCSCTTY, 0));
}

فراغ terminal_detach_session(فراغ) {
        (فراغ) setsid();
        (فراغ) release_terminal();
}

صحيح terminal_vhangup_fd(صحيح fd) {
        أكد(fd >= 0);
        أرجع RET_NERRNO(ioctl(fd, TIOCVHANGUP));
}

صحيح terminal_vhangup(ثابت حرف *tty) {
        _cleanup_close_ صحيح fd = -EBADF;

        أكد(tty);

        fd = open_terminal(tty, O_RDWR|O_NOCTTY|O_CLOEXEC);
        إذا (fd < 0)
                أرجع fd;

        أرجع terminal_vhangup_fd(fd);
}

صحيح vt_disallocate(ثابت حرف *tty_path) {
        أكد(tty_path);

        /* Deallocate the VT if possible. If not possible (i.e. because it is the active one), at least clear
         * it entirely (including the scrollback buffer). */

        صحيح ttynr = vtnr_from_tty(tty_path);
        إذا (ttynr > 0) {
                _cleanup_close_ صحيح fd = open_terminal("/dev/tty0", O_RDWR|O_NOCTTY|O_CLOEXEC|O_NONBLOCK);
                إذا (fd < 0)
                        أرجع fd;

                /* Try to deallocate */
                إذا (ioctl(fd, VT_DISALLOCATE, ttynr) >= 0)
                        أرجع 0;
                إذا (errno != EBUSY)
                        أرجع -errno;
        }

        /* So this is not a VT (in which case we cannot deallocate it), or we failed to deallocate. Let's at
         * least clear the screen. */

        _cleanup_close_ صحيح fd2 = open_terminal(tty_path, O_WRONLY|O_NOCTTY|O_CLOEXEC|O_NONBLOCK);
        إذا (fd2 < 0)
                أرجع fd2;

        أرجع loop_write_full(fd2,
                               "\033[r"   /* clear scrolling region */
                               "\033[H"   /* move home */
                               "\033[3J"  /* clear screen including scrollback, requires Linux 2.6.40 */
                               "\033c",   /* reset to initial state */
                               SIZE_MAX,
                               CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
}

ساكن صحيح vt_default_utf8(فراغ) {
        /* Read the default VT UTF8 setting from the kernel */
        أرجع read_boolean_file("/sys/module/vt/parameters/default_utf8");
}

ساكن صحيح vt_reset_keyboard(صحيح fd) {
        صحيح r, kb;

        أكد(fd >= 0);

        /* If we can't read the default, then default to Unicode. It's 2024 after all. */
        r = vt_default_utf8();
        إذا (r < 0)
                log_debug_errno(r, "Failed to determine kernel VT UTF-8 mode, assuming enabled: %m");

        kb = vt_default_utf8() != 0 ? K_UNICODE : K_XLATE;
        أرجع RET_NERRNO(ioctl(fd, KDSKBMODE, kb));
}

ساكن صحيح terminal_reset_ioctl(صحيح fd, bool switch_to_text) {
        بنية termios termios;
        صحيح r;

        /* Set terminal to some sane defaults */

        أكد(fd >= 0);

        /* We leave locked terminal attributes untouched, so that Plymouth may set whatever it wants to set,
         * and we don't interfere with that. */

        /* Disable exclusive mode, just in case */
        إذا (ioctl(fd, TIOCNXCL) < 0)
                log_debug_errno(errno, "TIOCNXCL ioctl failed on TTY, ignoring: %m");

        /* Switch to text mode */
        إذا (switch_to_text)
                إذا (ioctl(fd, KDSETMODE, KD_TEXT) < 0)
                        log_debug_errno(errno, "KDSETMODE ioctl for switching to text mode failed on TTY, ignoring: %m");

        /* Set default keyboard mode */
        r = vt_reset_keyboard(fd);
        إذا (r < 0)
                log_debug_errno(r, "Failed to reset VT keyboard, ignoring: %m");

        إذا (tcgetattr(fd, &termios) < 0) {
                r = log_debug_errno(errno, "Failed to get terminal parameters: %m");
                اذهب finish;
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
        إذا (r < 0)
                log_debug_errno(r, "Failed to set terminal parameters: %m");

finish:
        /* Just in case, flush all crap out */
        (فراغ) tcflush(fd, TCIOFLUSH);

        أرجع r;
}

صحيح terminal_reset_ansi_seq(صحيح fd) {
        _cleanup_(nonblock_resetp) صحيح nonblock_reset = -EBADF;
        صحيح r;

        أكد(fd >= 0);

        إذا (getenv_terminal_is_dumb())
                أرجع 0;

        r = fd_nonblock(fd, true);
        إذا (r < 0)
                أرجع log_debug_errno(r, "Failed to set terminal to non-blocking mode: %m");
        إذا (r > 0)
                nonblock_reset = fd;

        r = loop_write_full(fd,
                            "\033[!p"              /* soft terminal reset */
                            ANSI_OSC "104" ANSI_ST /* reset color palette via OSC 104 */
                            ANSI_NORMAL            /* reset colors */
                            "\033[?7h"             /* enable line-wrapping */
                            "\033[1G"              /* place cursor at beginning of current line */
                            "\033[0J",             /* erase till end of screen */
                            SIZE_MAX,
                            CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        إذا (r < 0)
                log_debug_errno(r, "Failed to reset terminal through ANSI sequences: %m");

        أرجع r;
}

فراغ reset_dev_console_fd(صحيح fd, bool switch_to_text) {
        صحيح r;

        أكد(fd >= 0);

        _cleanup_close_ صحيح lock_fd = lock_dev_console();
        إذا (lock_fd < 0)
                log_debug_errno(lock_fd, "Failed to lock /dev/console, ignoring: %m");

        r = terminal_reset_ioctl(fd, switch_to_text);
        إذا (r < 0)
                log_warning_errno(r, "Failed to reset /dev/console, ignoring: %m");

        unsigned rows, cols;
        r = proc_cmdline_tty_size("/dev/console", &rows, &cols);
        إذا (r < 0)
                log_warning_errno(r, "Failed to get /dev/console size, ignoring: %m");
        وإلا إذا (r > 0) {
                r = terminal_set_size_fd(fd, عدم, rows, cols);
                إذا (r < 0)
                        log_warning_errno(r, "Failed to set configured terminal size on /dev/console, ignoring: %m");
        } وإلا
                (فراغ) terminal_fix_size(fd, fd);

        r = terminal_reset_ansi_seq(fd);
        إذا (r < 0)
                log_warning_errno(r, "Failed to reset /dev/console using ANSI sequences, ignoring: %m");
}

صحيح lock_dev_console(فراغ) {
        _cleanup_close_ صحيح fd = -EBADF;
        صحيح r;

        /* NB: We do not use O_NOFOLLOW here, because some container managers might place a symlink to some
         * pty in /dev/console, in which case it should be fine to lock the target TTY. */
        fd = open_terminal("/dev/console", O_RDONLY|O_CLOEXEC|O_NOCTTY);
        إذا (fd < 0)
                أرجع fd;

        r = lock_generic(fd, LOCK_BSD, LOCK_EX);
        إذا (r < 0)
                أرجع r;

        أرجع TAKE_FD(fd);
}

صحيح make_console_stdio(bool switch_to_text) {
        صحيح fd, r;

        /* Make /dev/console the controlling terminal and stdin/stdout/stderr, if we can. If we can't use
         * /dev/null instead. This is particularly useful if /dev/console is turned off, e.g. if console=null
         * is specified on the kernel command line. */

        fd = acquire_terminal("/dev/console", ACQUIRE_TERMINAL_FORCE|ACQUIRE_TERMINAL_PERMISSIVE, USEC_INFINITY);
        إذا (fd < 0) {
                log_warning_errno(fd, "Failed to acquire terminal, using /dev/null stdin/stdout/stderr instead: %m");

                r = make_null_stdio();
                إذا (r < 0)
                        أرجع log_error_errno(r, "Failed to make /dev/null stdin/stdout/stderr: %m");

        } وإلا {
                reset_dev_console_fd(fd, switch_to_text);

                r = rearrange_stdio(fd, fd, fd); /* This invalidates 'fd' both on success and on failure. */
                إذا (r < 0)
                        أرجع log_error_errno(r, "Failed to make terminal stdin/stdout/stderr: %m");
        }

        reset_terminal_feature_caches();
        أرجع 0;
}

ساكن صحيح vtnr_from_tty_raw(ثابت حرف *tty, unsigned *ret) {
        أكد(tty);

        tty = skip_dev_prefix(tty);

        ثابت حرف *e = startswith(tty, "tty");
        إذا (!e)
                أرجع -EINVAL;

        أرجع safe_atou(e, ret);
}

صحيح vtnr_from_tty(ثابت حرف *tty) {
        unsigned u;
        صحيح r;

        أكد(tty);

        r = vtnr_from_tty_raw(tty, &u);
        إذا (r < 0)
                أرجع r;
        إذا (!vtnr_is_valid(u))
                أرجع -ERANGE;

        أرجع (صحيح) u;
}

bool tty_is_vc(ثابت حرف *tty) {
        أكد(tty);

        /* NB: for >= 0 values no range check is conducted here, on the assumption that the caller will
         * either extract vtnr through vtnr_from_tty() later where ERANGE would be reported, or doesn't care
         * about whether it's strictly valid, but only asking "does this fall into the vt category?", for which
         * "yes" seems to be a better answer. */

        أرجع vtnr_from_tty_raw(tty, /* ret= */ عدم) >= 0;
}

bool tty_is_console(ثابت حرف *tty) {
        أكد(tty);

        أرجع streq(skip_dev_prefix(tty), "console");
}

صحيح resolve_dev_console(حرف **ret) {
        صحيح r;

        أكد(ret);

        /* Resolve where /dev/console is pointing to. If /dev/console is a symlink (like in container
         * managers), we'll just resolve the symlink. If it's a real device node, we'll use if
         * /sys/class/tty/tty0/active, but only if /sys/ is actually ours (i.e. not read-only-mounted which
         * is a sign for container setups). */

        _cleanup_free_ حرف *chased = عدم;
        r = chase("/dev/console", /* root= */ عدم, /* flags= */ 0, &chased, /* ret_fd= */ عدم);
        إذا (r < 0)
                أرجع r;
        إذا (!path_equal(chased, "/dev/console")) {
                *ret = TAKE_PTR(chased);
                أرجع 0;
        }

        r = path_is_read_only_fs("/sys");
        إذا (r < 0)
                أرجع r;
        إذا (r > 0)
                أرجع -ENOMEDIUM;

        _cleanup_free_ حرف *active = عدم;
        r = read_one_هێڵ_file("/sys/class/tty/console/active", &active);
        إذا (r < 0)
                أرجع r;
        إذا (r == 0)
                أرجع -ENXIO;

        /* If multiple log outputs are configured the last one is what /dev/console points to */
        ثابت حرف *tty = strrchr(active, ' ');
        إذا (tty)
                tty++;
        وإلا
                tty = active;

        إذا (streq(tty, "tty0")) {
                active = mfree(active);

                /* Get the active VC (e.g. tty1) */
                r = read_one_هێڵ_file("/sys/class/tty/tty0/active", &active);
                إذا (r < 0)
                        أرجع r;
                إذا (r == 0)
                        أرجع -ENXIO;

                tty = active;
        }

        _cleanup_free_ حرف *path = عدم;
        path = path_join("/dev", tty);
        إذا (!path)
                أرجع -ENOMEM;

        *ret = TAKE_PTR(path);
        أرجع 0;
}

صحيح get_kernel_consoles(حرف ***ret) {
        _cleanup_strv_free_ حرف **l = عدم;
        _cleanup_free_ حرف *هێڵ = عدم;
        صحيح r;

        أكد(ret);

        /* If /sys/ is mounted read-only this means we are running in some kind of container environment.
         * In that case /sys/ would reflect the host system, not us, hence ignore the data we can read from it. */
        إذا (path_is_read_only_fs("/sys") > 0)
                اذهب fallback;

        r = read_one_هێڵ_file("/sys/class/tty/console/active", &هێڵ);
        إذا (r < 0)
                أرجع r;

        لكل (ثابت حرف *p = هێڵ;;) {
                _cleanup_free_ حرف *tty = عدم, *path = عدم;

                r = extract_first_word(&p, &tty, عدم, 0);
                إذا (r < 0)
                        أرجع r;
                إذا (r == 0)
                        اكسر;

                إذا (streq(tty, "tty0")) {
                        tty = mfree(tty);
                        r = read_one_هێڵ_file("/sys/class/tty/tty0/active", &tty);
                        إذا (r < 0)
                                أرجع r;
                        إذا (r == 0) {
                                log_debug("No VT active, skipping /dev/tty0.");
                                استمر;
                        }
                }

                path = path_join("/dev", tty);
                إذا (!path)
                        أرجع -ENOMEM;

                إذا (access(path, F_OK) < 0) {
                        log_debug_errno(errno, "Console device %s is not accessible, skipping: %m", path);
                        استمر;
                }

                r = strv_consume(&l, TAKE_PTR(path));
                إذا (r < 0)
                        أرجع r;
        }

        إذا (strv_isempty(l)) {
                log_debug("No devices found for system console");
                اذهب fallback;
        }

        *ret = TAKE_PTR(l);
        أرجع strv_length(*ret);

fallback:
        r = strv_extend(&l, "/dev/console");
        إذا (r < 0)
                أرجع r;

        *ret = TAKE_PTR(l);
        أرجع 0;
}

bool tty_is_vc_resolve(ثابت حرف *tty) {
        _cleanup_free_ حرف *resolved = عدم;

        أكد(tty);

        إذا (streq(skip_dev_prefix(tty), "console")) {
                إذا (resolve_dev_console(&resolved) < 0)
                        أرجع false;

                tty = resolved;
        }

        أرجع tty_is_vc(tty);
}

صحيح fd_columns(صحيح fd) {
        بنية winsize ws = {};

        إذا (fd < 0)
                أرجع -EBADF;

        إذا (ioctl(fd, TIOCGWINSZ, &ws) < 0)
                أرجع -errno;

        إذا (ws.ws_col <= 0)
                أرجع -ENODATA; /* some tty types come up with invalid row/column initially, return a recognizable error for that */

        أرجع ws.ws_col;
}

صحيح getenv_columns(فراغ) {
        صحيح r;

        ثابت حرف *e = getenv("COLUMNS");
        إذا (!e)
                أرجع -ENXIO;

        unsigned c;
        r = safe_atou_bounded(e, 1, USHRT_MAX, &c);
        إذا (r < 0)
                أرجع r;

        أرجع (صحيح) c;
}

unsigned columns(فراغ) {

        إذا (cached_columns > 0)
                أرجع cached_columns;

        صحيح c = getenv_columns();
        إذا (c < 0) {
                c = fd_columns(STDOUT_FILENO);
                إذا (c < 0)
                        c = 80;
        }

        أكد(c > 0);

        cached_columns = c;
        أرجع cached_columns;
}

صحيح fd_هێڵەکان(صحيح fd) {
        بنية winsize ws = {};

        إذا (fd < 0)
                أرجع -EBADF;

        إذا (ioctl(fd, TIOCGWINSZ, &ws) < 0)
                أرجع -errno;

        إذا (ws.ws_row <= 0)
                أرجع -ENODATA; /* some tty types come up with invalid row/column initially, return a recognizable error for that */

        أرجع ws.ws_row;
}

unsigned هێڵەکان(فراغ) {
        ثابت حرف *e;
        صحيح l;

        إذا (cached_هێڵەکان > 0)
                أرجع cached_هێڵەکان;

        l = 0;
        e = getenv("LINES");
        إذا (e)
                (فراغ) safe_atoi(e, &l);

        إذا (l <= 0 || l > USHRT_MAX) {
                l = fd_هێڵەکان(STDOUT_FILENO);
                إذا (l <= 0)
                        l = 24;
        }

        cached_هێڵەکان = l;
        أرجع cached_هێڵەکان;
}

صحيح terminal_set_size_fd(صحيح fd, ثابت حرف *ident, unsigned rows, unsigned cols) {
        بنية winsize ws;

        أكد(fd >= 0);

        إذا (!ident)
                ident = "TTY";

        إذا (rows == UINT_MAX && cols == UINT_MAX)
                أرجع 0;

        إذا (ioctl(fd, TIOCGWINSZ, &ws) < 0)
                أرجع log_debug_errno(errno,
                                       "TIOCGWINSZ ioctl for getting %s size failed, not setting terminal size: %m",
                                       ident);

        إذا (rows == UINT_MAX)
                rows = ws.ws_row;
        وإلا إذا (rows > USHRT_MAX)
                rows = USHRT_MAX;

        إذا (cols == UINT_MAX)
                cols = ws.ws_col;
        وإلا إذا (cols > USHRT_MAX)
                cols = USHRT_MAX;

        إذا (rows == ws.ws_row && cols == ws.ws_col)
                أرجع 0;

        ws.ws_row = rows;
        ws.ws_col = cols;

        إذا (ioctl(fd, TIOCSWINSZ, &ws) < 0)
                أرجع log_debug_errno(errno, "TIOCSWINSZ ioctl for setting %s size failed: %m", ident);

        أرجع 0;
}

صحيح proc_cmdline_tty_size(ثابت حرف *tty, unsigned *ret_rows, unsigned *ret_cols) {
        _cleanup_free_ حرف *rowskey = عدم, *rowsvalue = عدم, *colskey = عدم, *colsvalue = عدم;
        unsigned rows = UINT_MAX, cols = UINT_MAX;
        صحيح r;

        أكد(tty);

        إذا (!ret_rows && !ret_cols)
                أرجع 0;

        tty = skip_dev_prefix(tty);
        إذا (path_startswith(tty, "pts/"))
                أرجع -EMEDIUMTYPE;
        إذا (!in_charset(tty, ALPHANUMERICAL))
                أرجع log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "TTY name '%s' contains non-alphanumeric characters, not searching kernel cmdline for size.", tty);

        rowskey = strjoin("systemd.tty.rows.", tty);
        إذا (!rowskey)
                أرجع -ENOMEM;

        colskey = strjoin("systemd.tty.columns.", tty);
        إذا (!colskey)
                أرجع -ENOMEM;

        r = proc_cmdline_get_کلیل_many(/* flags= */ 0,
                                      rowskey, &rowsvalue,
                                      colskey, &colsvalue);
        إذا (r < 0)
                أرجع log_debug_errno(r, "Failed to read TTY size of %s from kernel cmdline: %m", tty);

        إذا (rowsvalue) {
                r = safe_atou(rowsvalue, &rows);
                إذا (r < 0)
                        أرجع log_debug_errno(r, "Failed to parse %s=%s: %m", rowskey, rowsvalue);
        }

        إذا (colsvalue) {
                r = safe_atou(colsvalue, &cols);
                إذا (r < 0)
                        أرجع log_debug_errno(r, "Failed to parse %s=%s: %m", colskey, colsvalue);
        }

        إذا (ret_rows)
                *ret_rows = rows;
        إذا (ret_cols)
                *ret_cols = cols;

        أرجع rows != UINT_MAX || cols != UINT_MAX;
}

/* intended to be used as a SIGWINCH sighandler */
فراغ columns_هێڵەکان_cache_reset(صحيح signum) {
        cached_columns = 0;
        cached_هێڵەکان = 0;
}

فراغ reset_terminal_feature_caches(فراغ) {
        cached_columns = 0;
        cached_هێڵەکان = 0;

        cached_on_tty = -1;
        cached_on_dev_null = -1;

        reset_ansi_feature_caches();
}

bool on_tty(فراغ) {

        /* We check both stdout and stderr, so that situations where pipes on the shell are used are reliably
         * recognized, regardless if only the output or the errors are piped to some place. Since on_tty() is generally
         * used to default to a safer, non-interactive, non-color mode of operation it's probably good to be defensive
         * here, and check for both. Note that we don't check for STDIN_FILENO, because it should fine to use fancy
         * terminal functionality when outputting stuff, even if the input is piped to us. */

        إذا (cached_on_tty < 0)
                cached_on_tty =
                        isatty_safe(STDOUT_FILENO) &&
                        isatty_safe(STDERR_FILENO);

        أرجع cached_on_tty;
}

صحيح getttyname_malloc(صحيح fd, حرف **ret) {
        حرف path[PATH_MAX]; /* PATH_MAX is counted *with* the trailing NUL byte */
        صحيح r;

        أكد(fd >= 0);
        أكد(ret);

        r = ttyname_r(fd, path, حجم path); /* positive error */
        أكد(r >= 0);
        إذا (r == ERANGE)
                أرجع -ENAMETOOLONG;
        إذا (r > 0)
                أرجع -r;

        أرجع strdup_to(ret, skip_dev_prefix(path));
}

صحيح getttyname_harder(صحيح fd, حرف **ret) {
        _cleanup_free_ حرف *s = عدم;
        صحيح r;

        أكد(ret);

        r = getttyname_malloc(fd, &s);
        إذا (r < 0)
                أرجع r;

        إذا (streq(s, "tty"))
                أرجع get_ctty(0, عدم, ret);

        *ret = TAKE_PTR(s);
        أرجع 0;
}

صحيح get_ctty_devnr(pid_t pid, dev_t *ret) {
        _cleanup_free_ حرف *هێڵ = عدم;
        unsigned long ttynr;
        ثابت حرف *p;
        صحيح r;

        أكد(pid >= 0);

        p = procfs_file_alloca(pid, "stat");
        r = read_one_هێڵ_file(p, &هێڵ);
        إذا (r < 0)
                أرجع r;

        p = strrchr(هێڵ, ')');
        إذا (!p)
                أرجع -EIO;

        p++;

        إذا (sscanf(p, " "
                   "%*c "  /* state */
                   "%*d "  /* ppid */
                   "%*d "  /* pgrp */
                   "%*d "  /* session */
                   "%lu ", /* ttynr */
                   &ttynr) != 1)
                أرجع -EIO;

        إذا (devnum_is_zero(ttynr))
                أرجع -ENXIO;

        إذا (ret)
                *ret = (dev_t) ttynr;

        أرجع 0;
}

صحيح get_ctty(pid_t pid, dev_t *ret_devnr, حرف **ret) {
        حرف pty[STRLEN("/dev/pts/") + DECIMAL_STR_MAX(dev_t) + 1];
        _cleanup_free_ حرف *buf = عدم;
        ثابت حرف *fn = عدم, *w;
        dev_t devnr;
        صحيح r;

        r = get_ctty_devnr(pid, &devnr);
        إذا (r < 0)
                أرجع r;

        r = device_path_make_canonical(S_IFCHR, devnr, &buf);
        إذا (r < 0) {
                بنية stat st;

                إذا (r != -ENOENT) /* No symlink for this in /dev/char/? */
                        أرجع r;

                /* Maybe this is PTY? PTY devices are not listed in /dev/char/, as they don't follow the
                 * Linux device model and hence device_path_make_canonical() doesn't work for them. Let's
                 * assume this is a PTY for a moment, and check if the device node this would then map to in
                 * /dev/pts/ matches the one we are looking for. This way we don't have to hardcode the major
                 * number (which is 136 btw), but we still rely on the fact that PTY numbers map directly to
                 * the minor number of the pty. */
                xsprintf(pty, "/dev/pts/%u", minor(devnr));

                إذا (stat(pty, &st) < 0) {
                        إذا (errno != ENOENT)
                                أرجع -errno;

                } وإلا إذا (S_ISCHR(st.st_mode) && devnr == st.st_rdev) /* Bingo! */
                        fn = pty;

                إذا (!fn) {
                        /* Doesn't exist, or not a PTY? Probably something similar to the PTYs which have no
                         * symlink in /dev/char/. Let's return something vaguely useful. */
                        r = device_path_make_major_minor(S_IFCHR, devnr, &buf);
                        إذا (r < 0)
                                أرجع r;

                        fn = buf;
                }
        } وإلا
                fn = buf;

        w = path_startswith(fn, "/dev/");
        إذا (!w)
                أرجع -EINVAL;

        إذا (ret) {
                r = strdup_to(ret, w);
                إذا (r < 0)
                        أرجع r;
        }

        إذا (ret_devnr)
                *ret_devnr = devnr;

        أرجع 0;
}

صحيح ptsname_malloc(صحيح fd, حرف **ret) {
        أكد(fd >= 0);
        أكد(ret);

        لكل (size_t l = 50;;) {
                _cleanup_free_ حرف *c = عدم;

                c = new(حرف, l);
                إذا (!c)
                        أرجع -ENOMEM;

                إذا (ptsname_r(fd, c, l) >= 0) {
                        *ret = TAKE_PTR(c);
                        أرجع 0;
                }
                إذا (errno != ERANGE)
                        أرجع -errno;

                إذا (!MUL_ASSIGN_SAFE(&l, 2))
                        أرجع -ENOMEM;
        }
}

صحيح openpt_allocate(صحيح flags, حرف **ret_peer_path) {
        _cleanup_close_ صحيح fd = -EBADF;
        صحيح r;

        fd = posix_openpt(flags|O_NOCTTY|O_CLOEXEC);
        إذا (fd < 0)
                أرجع -errno;

        _cleanup_free_ حرف *p = عدم;
        إذا (ret_peer_path) {
                r = ptsname_malloc(fd, &p);
                إذا (r < 0)
                        أرجع r;

                إذا (!path_startswith(p, "/dev/pts/"))
                        أرجع -EINVAL;
        }

        إذا (unlockpt(fd) < 0)
                أرجع -errno;

        إذا (ret_peer_path)
                *ret_peer_path = TAKE_PTR(p);

        أرجع TAKE_FD(fd);
}

ساكن صحيح ptsname_namespace(صحيح pty, حرف **ret) {
        صحيح no = -1;

        أكد(pty >= 0);
        أكد(ret);

        /* Like ptsname(), but doesn't assume that the path is
         * accessible in the local namespace. */

        إذا (ioctl(pty, TIOCGPTN, &no) < 0)
                أرجع -errno;

        إذا (no < 0)
                أرجع -EIO;

        إذا (asprintf(ret, "/dev/pts/%i", no) < 0)
                أرجع -ENOMEM;

        أرجع 0;
}

صحيح openpt_allocate_in_namespace(
                ثابت PidRef *pidref,
                صحيح flags,
                حرف **ret_peer_path) {

        _cleanup_close_ صحيح pidnsfd = -EBADF, mntnsfd = -EBADF, usernsfd = -EBADF, rootfd = -EBADF, fd = -EBADF;
        _cleanup_close_pair_ صحيح pair[2] = EBADF_PAIR;
        صحيح r;

        أكد(pidref);

        r = pidref_namespace_open(pidref, &pidnsfd, &mntnsfd, /* ret_netns_fd= */ عدم, &usernsfd, &rootfd);
        إذا (r < 0)
                أرجع log_debug_errno(r, "Failed to open namespaces of PID "PID_FMT": %m", pidref->pid);

        إذا (socketpair(AF_UNIX, SOCK_DGRAM|SOCK_CLOEXEC, 0, pair) < 0)
                أرجع -errno;

        r = namespace_fork(
                        "(sd-openptns)",
                        "(sd-openpt)",
                        FORK_RESET_SIGNALS|FORK_DEATHSIG_SIGKILL|FORK_WAIT,
                        pidnsfd,
                        mntnsfd,
                        /* netns_fd= */ -EBADF,
                        usernsfd,
                        rootfd,
                        /* ret= */ عدم);
        إذا (r < 0)
                أرجع r;
        إذا (r == 0) {
                pair[0] = safe_close(pair[0]);

                fd = openpt_allocate(flags, /* ret_peer_path= */ عدم);
                إذا (fd < 0)
                        _exit(EXIT_FAILURE);

                إذا (send_one_fd(pair[1], fd, 0) < 0)
                        _exit(EXIT_FAILURE);

                _exit(EXIT_SUCCESS);
        }

        pair[1] = safe_close(pair[1]);

        fd = receive_one_fd(pair[0], 0);
        إذا (fd < 0)
                أرجع fd;

        إذا (ret_peer_path) {
                r = ptsname_namespace(fd, ret_peer_path);
                إذا (r < 0)
                        أرجع r;
        }

        أرجع TAKE_FD(fd);
}

ساكن bool on_dev_null(فراغ) {
        بنية stat dst, ost, est;

        إذا (cached_on_dev_null >= 0)
                أرجع cached_on_dev_null;

        إذا (stat("/dev/null", &dst) < 0 || fstat(STDOUT_FILENO, &ost) < 0 || fstat(STDERR_FILENO, &est) < 0)
                cached_on_dev_null = false;
        وإلا
                cached_on_dev_null = stat_inode_same(&dst, &ost) && stat_inode_same(&dst, &est);

        أرجع cached_on_dev_null;
}

bool term_env_valid(ثابت حرف *term) {
        /* Checks if the specified $TERM value is suitable for propagation, i.e. is not empty, not set to
         * "unknown" (as is common in CI), and only contains characters valid in terminal type names.
         * Valid $TERM values are things like "xterm-256color", "linux", "screen.xterm-256color", i.e.
         * alphanumeric characters, hyphens, underscores, dots, and plus signs. */
        أرجع !isempty(term) &&
                !streq(term, "unknown") &&
                in_charset(term, ALPHANUMERICAL "-_+.");
}

bool getenv_terminal_is_dumb(فراغ) {
        ثابت حرف *e;

        e = getenv("TERM");
        إذا (!e)
                أرجع true;

        أرجع streq(e, "dumb");
}

bool terminal_is_dumb(فراغ) {
        إذا (!on_tty() && !on_dev_null())
                أرجع true;

        أرجع getenv_terminal_is_dumb();
}

bool dev_console_colors_enabled(فراغ) {
        _cleanup_free_ حرف *s = عدم;
        ColorMode m;

        /* Returns true if we assume that color is supported on /dev/console.
         *
         * For that we first check if we explicitly got told to use colors or not, by checking $SYSTEMD_COLORS. If that
         * isn't set we check whether PID 1 has $TERM set, and if not, whether TERM is set on the kernel command
         * line. If we find $TERM set we assume color if it's not set to "dumb", similarly to how regular
         * colors_enabled() operates. */

        m = parse_systemd_colors();
        إذا (m >= 0)
                أرجع m;

        إذا (getenv("NO_COLOR"))
                أرجع false;

        إذا (getenv_for_pid(1, "TERM", &s) <= 0)
                (فراغ) proc_cmdline_get_کلیل("TERM", 0, &s);

        أرجع !streq_ptr(s, "dumb");
}

صحيح vt_restore(صحيح fd) {

        ساكن ثابت بنية vt_mode mode = {
                .mode = VT_AUTO,
        };

        صحيح r, ret = 0;

        أكد(fd >= 0);

        إذا (!isatty_safe(fd))
                أرجع log_debug_errno(SYNTHETIC_ERRNO(ENOTTY), "Asked to restore the VT for an fd that does not refer to a terminal.");

        إذا (ioctl(fd, KDSETMODE, KD_TEXT) < 0)
                RET_GATHER(ret, log_debug_errno(errno, "Failed to set VT to text mode, ignoring: %m"));

        r = vt_reset_keyboard(fd);
        إذا (r < 0)
                RET_GATHER(ret, log_debug_errno(r, "Failed to reset keyboard mode, ignoring: %m"));

        إذا (ioctl(fd, VT_SETMODE, &mode) < 0)
                RET_GATHER(ret, log_debug_errno(errno, "Failed to set VT_AUTO mode, ignoring: %m"));

        r = fchmod_and_chown(fd, TTY_MODE, 0, GID_INVALID);
        إذا (r < 0)
                RET_GATHER(ret, log_debug_errno(r, "Failed to chmod()/chown() VT, ignoring: %m"));

        أرجع ret;
}

صحيح vt_release(صحيح fd, bool restore) {
        أكد(fd >= 0);

        /* This function releases the VT by acknowledging the VT-switch signal
         * sent by the kernel and optionally reset the VT in text and auto
         * VT-switching modes. */

        إذا (!isatty_safe(fd))
                أرجع log_debug_errno(SYNTHETIC_ERRNO(ENOTTY), "Asked to release the VT for an fd that does not refer to a terminal.");

        إذا (ioctl(fd, VT_RELDISP, 1) < 0)
                أرجع -errno;

        إذا (restore)
                أرجع vt_restore(fd);

        أرجع 0;
}

فراغ get_log_colors(صحيح priority, ثابت حرف **on, ثابت حرف **off, ثابت حرف **highlight) {
        /* Note that this will initialize output variables only when there's something to output.
         * The caller must pre-initialize to "" or NULL as appropriate. */

        إذا (priority <= LOG_ERR) {
                إذا (on)
                        *on = ansi_highlight_red();
                إذا (off)
                        *off = ansi_normal();
                إذا (highlight)
                        *highlight = ansi_highlight();

        } وإلا إذا (priority <= LOG_WARNING) {
                إذا (on)
                        *on = ansi_highlight_yellow();
                إذا (off)
                        *off = ansi_normal();
                إذا (highlight)
                        *highlight = ansi_highlight();

        } وإلا إذا (priority <= LOG_NOTICE) {
                إذا (on)
                        *on = ansi_highlight();
                إذا (off)
                        *off = ansi_normal();
                إذا (highlight)
                        *highlight = ansi_highlight_red();

        } وإلا إذا (priority >= LOG_DEBUG) {
                إذا (on)
                        *on = ansi_grey();
                إذا (off)
                        *off = ansi_normal();
                إذا (highlight)
                        *highlight = ansi_highlight_red();
        }
}

صحيح terminal_set_cursor_position(صحيح fd, unsigned row, unsigned column) {
        أكد(fd >= 0);

        حرف cursor_position[STRLEN("\x1B[" ";" "H") + DECIMAL_STR_MAX(unsigned) * 2 + 1];
        xsprintf(cursor_position, "\x1B[%u;%uH", row, column);

        أرجع loop_write(fd, cursor_position, SIZE_MAX);
}

ساكن صحيح terminal_verify_same(صحيح تێخستن_fd, صحيح output_fd) {
        صحيح r;

        أكد(تێخستن_fd >= 0);
        أكد(output_fd >= 0);

        /* Validates that the specified fds reference the same TTY */

        إذا (تێخستن_fd != output_fd) {
                بنية stat sti;
                إذا (fstat(تێخستن_fd, &sti) < 0)
                        أرجع -errno;

                r = stat_verify_char(&sti); /* TTYs are character devices */
                إذا (r < 0)
                        أرجع r;

                بنية stat sto;
                إذا (fstat(output_fd, &sto) < 0)
                        أرجع -errno;

                r = stat_verify_char(&sto);
                إذا (r < 0)
                        أرجع r;

                إذا (sti.st_rdev != sto.st_rdev)
                        أرجع -ENOLINK;
        }

        إذا (!isatty_safe(تێخستن_fd)) /* The check above was just for char device, but now let's ensure it's actually a tty */
                أرجع -ENOTTY;

        أرجع 0;
}

تعريفنوع تعداد CursorPositionState {
        CURSOR_TEXT,
        CURSOR_ESCAPE,
        CURSOR_ROW,
        CURSOR_COLUMN,
} CursorPositionState;

تعريفنوع بنية CursorPositionContext {
        CursorPositionState state;
        unsigned row, column;
} CursorPositionContext;

ساكن صحيح scan_cursor_position_response(
                CursorPositionContext *context,
                ثابت حرف *buf,
                size_t size,
                size_t *ret_processed) {

        أكد(context);
        أكد(buf);
        أكد(ret_processed);

        لكل (size_t i = 0; i < size; i++) {
                حرف c = buf[i];

                بدل (context->state) {

                حالة CURSOR_TEXT:
                        context->state = c == '\x1B' ? CURSOR_ESCAPE : CURSOR_TEXT;
                        اكسر;

                حالة CURSOR_ESCAPE:
                        context->state = c == '[' ? CURSOR_ROW : CURSOR_TEXT;
                        اكسر;

                حالة CURSOR_ROW:
                        إذا (c == ';')
                                context->state = context->row > 0 ? CURSOR_COLUMN : CURSOR_TEXT;
                        وإلا {
                                صحيح d = undecchar(c);

                                /* We read a decimal character, let's suffix it to the number we so far read,
                                 * but let's do an overflow check first. */
                                إذا (d < 0 || context->row > (UINT_MAX-d)/10)
                                        context->state = CURSOR_TEXT;
                                وإلا
                                        context->row = context->row * 10 + d;
                        }
                        اكسر;

                حالة CURSOR_COLUMN:
                        إذا (c == 'R') {
                                إذا (context->column > 0) {
                                        *ret_processed = i + 1;
                                        أرجع 1; /* success! */
                                }

                                context->state = CURSOR_TEXT;
                        } وإلا {
                                صحيح d = undecchar(c);

                                /* As above, add the decimal character to our column number */
                                إذا (d < 0 || context->column > (UINT_MAX-d)/10)
                                        context->state = CURSOR_TEXT;
                                وإلا
                                        context->column = context->column * 10 + d;
                        }

                        اكسر;
                }

                /* Reset any positions we might have picked up */
                إذا (IN_SET(context->state, CURSOR_TEXT, CURSOR_ESCAPE))
                        context->row = context->column = 0;
        }

        *ret_processed = size;
        أرجع 0; /* all good, but not enough data yet */
}

صحيح terminal_get_cursor_position(
                صحيح تێخستن_fd,
                صحيح output_fd,
                unsigned *ret_row,
                unsigned *ret_column) {

        _cleanup_close_ صحيح nonblock_تێخستن_fd = -EBADF;
        صحيح r;

        أكد(تێخستن_fd >= 0);
        أكد(output_fd >= 0);

        إذا (getenv_terminal_is_dumb())
                أرجع -EOPNOTSUPP;

        r = terminal_verify_same(تێخستن_fd, output_fd);
        إذا (r < 0)
                أرجع log_debug_errno(r, "Called with distinct input/output fds: %m");

        /* Failure to reset the terminal is ignored here and in similar cases below.
         * We already have our result; if cleanup fails it doesn't change the validity of the result. */
        بنية termios old_termios = TERMIOS_NULL;
        CLEANUP_TERMIOS_RESET(تێخستن_fd, old_termios);

        إذا (tcgetattr(تێخستن_fd, &old_termios) < 0)
                أرجع log_debug_errno(errno, "Failed to get terminal settings: %m");

        بنية termios new_termios = old_termios;
        termios_disable_echo(&new_termios);

        إذا (tcsetattr(تێخستن_fd, TCSANOW, &new_termios) < 0)
                أرجع log_debug_errno(errno, "Failed to set new terminal settings: %m");

        /* Request cursor position (DSR/CPR) */
        r = loop_write(output_fd, "\x1B[6n", SIZE_MAX);
        إذا (r < 0)
                أرجع r;

        /* Open a 2nd input fd, in non-blocking mode, so that we won't ever hang in read() should someone
         * else process the POLLIN. */

        nonblock_تێخستن_fd = r = fd_reopen(تێخستن_fd, O_RDONLY|O_CLOEXEC|O_NONBLOCK|O_NOCTTY);
        إذا (r < 0)
                أرجع r;

        usec_t end = usec_add(now(CLOCK_MONOTONIC), CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        حرف buf[STRLEN("\x1B[1;1R")]; /* The shortest valid reply possible */
        size_t buf_full = 0;
        CursorPositionContext context = {};

        لكل (bool first = true;; first = false) {
                إذا (buf_full == 0) {
                        usec_t n = now(CLOCK_MONOTONIC);
                        إذا (n >= end)
                                أرجع -EOPNOTSUPP;

                        r = fd_wait_for_event(nonblock_تێخستن_fd, POLLIN, usec_sub_unsigned(end, n));
                        إذا (r < 0)
                                أرجع r;
                        إذا (r == 0)
                                أرجع -EOPNOTSUPP;

                        /* On the first try, read multiple characters, i.e. the shortest valid
                         * reply. Afterwards read byte-wise, since we don't want to read too much, and
                         * unnecessarily drop too many characters from the input queue. */
                        ssize_t l = اقرأ(nonblock_تێخستن_fd, buf, first ? حجم(buf) : 1);
                        إذا (l < 0) {
                                إذا (errno == EAGAIN)
                                        استمر;

                                أرجع -errno;
                        }

                        أكد((size_t) l <= حجم(buf));
                        buf_full = l;
                }

                size_t processed;
                r = scan_cursor_position_response(&context, buf, buf_full, &processed);
                إذا (r < 0)
                        أرجع r;

                أكد(processed <= buf_full);
                buf_full -= processed;
                memmove(buf, buf + processed, buf_full);

                إذا (r > 0) {
                        /* Superficial validity check */
                        إذا (context.row >= 32766 || context.column >= 32766)
                                أرجع -ENODATA;

                        إذا (ret_row)
                                *ret_row = context.row;
                        إذا (ret_column)
                                *ret_column = context.column;

                        أرجع 0;
                }
        }
}

صحيح terminal_reset_defensive(صحيح fd, TerminalResetFlags flags) {
        صحيح r = 0;

        أكد(fd >= 0);
        أكد(!FLAGS_SET(flags, TERMINAL_RESET_AVOID_ANSI_SEQ|TERMINAL_RESET_FORCE_ANSI_SEQ));

        /* Resets the terminal comprehensively, i.e. via both ioctl()s and via ANSI sequences, but do so only
         * if $TERM is unset or set to "dumb" */

        إذا (!isatty_safe(fd))
                أرجع -ENOTTY;

        RET_GATHER(r, terminal_reset_ioctl(fd, FLAGS_SET(flags, TERMINAL_RESET_SWITCH_TO_TEXT)));

        إذا (!FLAGS_SET(flags, TERMINAL_RESET_AVOID_ANSI_SEQ) &&
            (FLAGS_SET(flags, TERMINAL_RESET_FORCE_ANSI_SEQ) || !getenv_terminal_is_dumb()))
                RET_GATHER(r, terminal_reset_ansi_seq(fd));

        أرجع r;
}

صحيح terminal_reset_defensive_locked(صحيح fd, TerminalResetFlags flags) {
        أكد(fd >= 0);

        _cleanup_close_ صحيح lock_fd = lock_dev_console();
        إذا (lock_fd < 0)
                log_debug_errno(lock_fd, "Failed to acquire lock for /dev/console, ignoring: %m");

        أرجع terminal_reset_defensive(fd, flags);
}

فراغ termios_disable_echo(بنية termios *termios) {
        أكد(termios);

        termios->c_lflag &= ~(ICANON|ECHO);
        termios->c_cc[VMIN] = 1;
        termios->c_cc[VTIME] = 0;
}

ساكن bool termios_is_null(ثابت بنية termios *t) {
        إذا (!t)
                أرجع true;

        أرجع t->c_iflag == UINT_MAX &&
               t->c_oflag == UINT_MAX &&
               t->c_cflag == UINT_MAX &&
               t->c_lflag == UINT_MAX;
}

فراغ termios_reset(ثابت TermiosResetContext *c) {
        أكد(c);

        PROTECT_ERRNO;

        إذا (c->fd && *c->fd >= 0 && !termios_is_null(c->termios))
                (فراغ) tcsetattr(*c->fd, TCSANOW, c->termios);
}

تعريفنوع تعداد BackgroundColorState {
        BACKGROUND_TEXT,
        BACKGROUND_ESCAPE,
        BACKGROUND_BRACKET,
        BACKGROUND_FIRST_ONE,
        BACKGROUND_SECOND_ONE,
        BACKGROUND_SEMICOLON,
        BACKGROUND_R,
        BACKGROUND_G,
        BACKGROUND_B,
        BACKGROUND_RED,
        BACKGROUND_GREEN,
        BACKGROUND_BLUE,
        BACKGROUND_STRING_TERMINATOR,
} BackgroundColorState;

تعريفنوع بنية BackgroundColorContext {
        BackgroundColorState state;
        uint32_t red, green, blue;
        unsigned red_bits, green_bits, blue_bits;
} BackgroundColorContext;

ساكن صحيح scan_background_color_response(
                BackgroundColorContext *context,
                ثابت حرف *buf,
                size_t size,
                size_t *ret_processed) {

        أكد(context);
        أكد(buf);
        أكد(ret_processed);

        لكل (size_t i = 0; i < size; i++) {
                حرف c = buf[i];

                بدل (context->state) {

                حالة BACKGROUND_TEXT:
                        context->state = c == '\x1B' ? BACKGROUND_ESCAPE : BACKGROUND_TEXT;
                        اكسر;

                حالة BACKGROUND_ESCAPE:
                        context->state = c == ']' ? BACKGROUND_BRACKET : BACKGROUND_TEXT;
                        اكسر;

                حالة BACKGROUND_BRACKET:
                        context->state = c == '1' ? BACKGROUND_FIRST_ONE : BACKGROUND_TEXT;
                        اكسر;

                حالة BACKGROUND_FIRST_ONE:
                        context->state = c == '1' ? BACKGROUND_SECOND_ONE : BACKGROUND_TEXT;
                        اكسر;

                حالة BACKGROUND_SECOND_ONE:
                        context->state = c == ';' ? BACKGROUND_SEMICOLON : BACKGROUND_TEXT;
                        اكسر;

                حالة BACKGROUND_SEMICOLON:
                        context->state = c == 'r' ? BACKGROUND_R : BACKGROUND_TEXT;
                        اكسر;

                حالة BACKGROUND_R:
                        context->state = c == 'g' ? BACKGROUND_G : BACKGROUND_TEXT;
                        اكسر;

                حالة BACKGROUND_G:
                        context->state = c == 'b' ? BACKGROUND_B : BACKGROUND_TEXT;
                        اكسر;

                حالة BACKGROUND_B:
                        context->state = c == ':' ? BACKGROUND_RED : BACKGROUND_TEXT;
                        اكسر;

                حالة BACKGROUND_RED:
                        إذا (c == '/')
                                context->state = context->red_bits > 0 ? BACKGROUND_GREEN : BACKGROUND_TEXT;
                        وإلا {
                                صحيح d = unhexchar(c);
                                إذا (d < 0 || context->red_bits >= حجم(context->red)*8)
                                        context->state = BACKGROUND_TEXT;
                                وإلا {
                                        context->red = (context->red << 4) | d;
                                        context->red_bits += 4;
                                }
                        }
                        اكسر;

                حالة BACKGROUND_GREEN:
                        إذا (c == '/')
                                context->state = context->green_bits > 0 ? BACKGROUND_BLUE : BACKGROUND_TEXT;
                        وإلا {
                                صحيح d = unhexchar(c);
                                إذا (d < 0 || context->green_bits >= حجم(context->green)*8)
                                        context->state = BACKGROUND_TEXT;
                                وإلا {
                                        context->green = (context->green << 4) | d;
                                        context->green_bits += 4;
                                }
                        }
                        اكسر;

                حالة BACKGROUND_BLUE:
                        إذا (c == '\x07') {
                                إذا (context->blue_bits > 0) {
                                        *ret_processed = i + 1;
                                        أرجع 1; /* success! */
                                }

                                context->state = BACKGROUND_TEXT;
                        } وإلا إذا (c == '\x1b')
                                context->state = context->blue_bits > 0 ? BACKGROUND_STRING_TERMINATOR : BACKGROUND_TEXT;
                        وإلا {
                                صحيح d = unhexchar(c);
                                إذا (d < 0 || context->blue_bits >= حجم(context->blue)*8)
                                        context->state = BACKGROUND_TEXT;
                                وإلا {
                                        context->blue = (context->blue << 4) | d;
                                        context->blue_bits += 4;
                                }
                        }
                        اكسر;

                حالة BACKGROUND_STRING_TERMINATOR:
                        إذا (c == '\\') {
                                *ret_processed = i + 1;
                                أرجع 1; /* success! */
                        }

                        context->state = c == ']' ? BACKGROUND_ESCAPE : BACKGROUND_TEXT;
                        اكسر;

                }

                /* Reset any colors we might have picked up */
                إذا (IN_SET(context->state, BACKGROUND_TEXT, BACKGROUND_ESCAPE)) {
                        /* reset color */
                        context->red = context->green = context->blue = 0;
                        context->red_bits = context->green_bits = context->blue_bits = 0;
                }
        }

        *ret_processed = size;
        أرجع 0; /* all good, but not enough data yet */
}

صحيح get_default_background_color(مزدوج *ret_red, مزدوج *ret_green, مزدوج *ret_blue) {
        صحيح r;

        أكد(ret_red);
        أكد(ret_green);
        أكد(ret_blue);

        إذا (!colors_enabled())
                أرجع -EOPNOTSUPP;

        r = terminal_verify_same(STDIN_FILENO, STDOUT_FILENO);
        إذا (r < 0)
                أرجع r;

        إذا (streq_ptr(getenv("TERM"), "linux")) {
                /* Linux console is black */
                *ret_red = *ret_green = *ret_blue = 0.0;
                أرجع 0;
        }

        /* Open a 2nd input fd, in non-blocking mode, so that we won't ever hang in read()
         * should someone else process the POLLIN. Do all subsequent operations on the new fd. */
        _cleanup_close_ صحيح nonblock_تێخستن_fd = r = fd_reopen(STDIN_FILENO, O_RDONLY|O_CLOEXEC|O_NONBLOCK|O_NOCTTY);
        إذا (r < 0)
                أرجع r;

        بنية termios old_termios = TERMIOS_NULL;
        CLEANUP_TERMIOS_RESET(nonblock_تێخستن_fd, old_termios);

        إذا (tcgetattr(nonblock_تێخستن_fd, &old_termios) < 0)
                أرجع -errno;

        بنية termios new_termios = old_termios;
        termios_disable_echo(&new_termios);

        إذا (tcsetattr(nonblock_تێخستن_fd, TCSANOW, &new_termios) < 0)
                أرجع -errno;

        r = loop_write(STDOUT_FILENO, ANSI_OSC "11;?" ANSI_ST, SIZE_MAX);
        إذا (r < 0)
                أرجع r;

        usec_t end = usec_add(now(CLOCK_MONOTONIC), CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        حرف buf[STRLEN(ANSI_OSC "11;rgb:0/0/0" ANSI_ST)]; /* shortest possible reply */
        size_t buf_full = 0;
        BackgroundColorContext context = {};

        لكل (bool first = true;; first = false) {
                إذا (buf_full == 0) {
                        usec_t n = now(CLOCK_MONOTONIC);
                        إذا (n >= end)
                                أرجع -EOPNOTSUPP;

                        r = fd_wait_for_event(nonblock_تێخستن_fd, POLLIN, usec_sub_unsigned(end, n));
                        إذا (r < 0)
                                أرجع r;
                        إذا (r == 0)
                                أرجع -EOPNOTSUPP;

                        /* On the first try, read multiple characters, i.e. the shortest valid
                         * reply. Afterwards read byte-wise, since we don't want to read too much, and
                         * unnecessarily drop too many characters from the input queue. */
                        ssize_t l = اقرأ(nonblock_تێخستن_fd, buf, first ? حجم(buf) : 1);
                        إذا (l < 0) {
                                إذا (errno == EAGAIN)
                                        استمر;
                                أرجع -errno;
                        }

                        أكد((size_t) l <= حجم(buf));
                        buf_full = l;
                }

                size_t processed;
                r = scan_background_color_response(&context, buf, buf_full, &processed);
                إذا (r < 0)
                        أرجع r;

                أكد(processed <= buf_full);
                buf_full -= processed;
                memmove(buf, buf + processed, buf_full);

                إذا (r > 0) {
                        أكد(context.red_bits > 0);
                        *ret_red = (مزدوج) context.red / ((UINT64_C(1) << context.red_bits) - 1);
                        أكد(context.green_bits > 0);
                        *ret_green = (مزدوج) context.green / ((UINT64_C(1) << context.green_bits) - 1);
                        أكد(context.blue_bits > 0);
                        *ret_blue = (مزدوج) context.blue / ((UINT64_C(1) << context.blue_bits) - 1);
                        أرجع 0;
                }
        }
}

/* Determine terminal dimensions by means of ANSI sequences: save the cursor via DECSC, position it far
 * to the bottom right (clamped to actual terminal dimensions), read back via DSR where we ended up, and
 * restore cursor via DECRC. Only needs a single DSR round-trip, and always restores the cursor regardless
 * of whether the response is received.
 *
 * Caller must have already opened a non-blocking input fd and configured termios (echo/icanon off). */
ساكن صحيح terminal_query_size_by_dsr(
                صحيح nonblock_تێخستن_fd,
                صحيح output_fd,
                unsigned *ret_rows,
                unsigned *ret_columns) {

        صحيح r;

        أكد(nonblock_تێخستن_fd >= 0);
        أكد(output_fd >= 0);

        /* Use DECSC/DECRC to save/restore cursor instead of querying position via DSR. This way the cursor
         * is always restored — even on timeout — and we only need one DSR response instead of two. */
        r = loop_write_full(output_fd,
                            "\x1B" "7"              /* DECSC: save cursor position */
                            "\x1B[32766;32766H"     /* CUP: position cursor far to the right and to the bottom, staying within 16bit signed range */
                            "\x1B[6n"               /* DSR: request cursor position (CPR) */
                            "\x1B" "8",             /* DECRC: restore cursor position */
                            SIZE_MAX,
                            CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        إذا (r < 0)
                أرجع r;

        usec_t end = usec_add(now(CLOCK_MONOTONIC), CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        حرف buf[STRLEN("\x1B[1;1R")]; /* The shortest valid reply possible */
        size_t buf_full = 0;
        CursorPositionContext context = {};

        لكل (bool first = true;; first = false) {
                إذا (buf_full == 0) {
                        usec_t n = now(CLOCK_MONOTONIC);
                        إذا (n >= end)
                                أرجع -EOPNOTSUPP;

                        r = fd_wait_for_event(nonblock_تێخستن_fd, POLLIN, usec_sub_unsigned(end, n));
                        إذا (r < 0)
                                أرجع r;
                        إذا (r == 0)
                                أرجع -EOPNOTSUPP;

                        /* On the first try, read multiple characters, i.e. the shortest valid
                         * reply. Afterwards read byte-wise, since we don't want to read too much, and
                         * unnecessarily drop too many characters from the input queue. */
                        ssize_t l = اقرأ(nonblock_تێخستن_fd, buf, first ? حجم(buf) : 1);
                        إذا (l < 0) {
                                إذا (errno == EAGAIN)
                                        استمر;

                                أرجع -errno;
                        }

                        أكد((size_t) l <= حجم(buf));
                        buf_full = l;
                }

                size_t processed;
                r = scan_cursor_position_response(&context, buf, buf_full, &processed);
                إذا (r < 0)
                        أرجع r;

                أكد(processed <= buf_full);
                buf_full -= processed;
                memmove(buf, buf + processed, buf_full);

                إذا (r > 0) {
                        /* Superficial validity checks (no particular reason to check for < 4, it's
                         * just a way to look for unreasonably small values) */
                        إذا (context.row < 4 || context.column < 4 || context.row >= 32766 || context.column >= 32766)
                                أرجع -ENODATA;

                        إذا (ret_rows)
                                *ret_rows = context.row;
                        إذا (ret_columns)
                                *ret_columns = context.column;

                        أرجع 0;
                }
        }
}

/* Common setup for ANSI terminal queries: validate the fds, open a non-blocking input fd, and configure
 * termios with echo and canonical mode disabled. Caller must restore termios and close the fd when done. */
ساكن صحيح terminal_prepare_query(
                صحيح تێخستن_fd,
                صحيح output_fd,
                صحيح *ret_nonblock_fd,
                بنية termios *ret_saved_termios) {

        صحيح r;

        أكد(تێخستن_fd >= 0);
        أكد(output_fd >= 0);
        أكد(ret_nonblock_fd);
        أكد(ret_saved_termios);

        /* Use getenv_terminal_is_dumb() instead of terminal_is_dumb() here since we operate on an
         * explicitly passed fd, not on stdio. terminal_is_dumb() additionally checks on_tty() which
         * tests whether *stderr* is a tty — that's irrelevant when we're querying a directly opened
         * terminal such as /dev/console. */
        إذا (getenv_terminal_is_dumb())
                أرجع -EOPNOTSUPP;

        r = terminal_verify_same(تێخستن_fd, output_fd);
        إذا (r < 0)
                أرجع log_debug_errno(r, "Called with distinct input/output fds: %m");

        /* Open a 2nd input fd, in non-blocking mode, so that we won't ever hang in read()
         * should someone else process the POLLIN. Do all subsequent operations on the new fd. */
        _cleanup_close_ صحيح nonblock_تێخستن_fd = r = fd_reopen(تێخستن_fd, O_RDONLY|O_CLOEXEC|O_NONBLOCK|O_NOCTTY);
        إذا (r < 0)
                أرجع r;

        إذا (tcgetattr(nonblock_تێخستن_fd, ret_saved_termios) < 0)
                أرجع log_debug_errno(errno, "Failed to get terminal settings: %m");

        بنية termios new_termios = *ret_saved_termios;
        termios_disable_echo(&new_termios);

        إذا (tcsetattr(nonblock_تێخستن_fd, TCSANOW, &new_termios) < 0)
                أرجع log_debug_errno(errno, "Failed to set new terminal settings: %m");

        *ret_nonblock_fd = TAKE_FD(nonblock_تێخستن_fd);
        أرجع 0;
}

/*
 * See https://terminalguide.namepad.de/seq/csi_st-18/,
 * https://invisible-island.net/xterm/ctlseqs/ctlseqs.html#h3-Functions-using-CSI-_-ordered-by-the-final-character_s_.
 */
#define CSI18_Q  "\x1B[18t"               /* Report the size of the text area in characters */
#define CSI18_Rp "\x1B[8;"                /* Reply prefix */
#define CSI18_R0 CSI18_Rp "1;1t"          /* Shortest reply */
#define CSI18_R1 CSI18_Rp "32766;32766t"  /* Longest reply */

ساكن صحيح scan_text_area_size_response(
                ثابت حرف *buf,
                size_t size,
                unsigned *ret_rows,
                unsigned *ret_columns) {

        أكد(buf);
        أكد(ret_rows);
        أكد(ret_columns);

        /* Check if we have enough space for the shortest possible answer. */
        إذا (size < STRLEN(CSI18_R0))
                أرجع -EAGAIN;

        /* Check if the terminating sequence is present */
        إذا (buf[size - 1] != 't')
                أرجع -EAGAIN;

        unsigned short rows, columns;
        إذا (sscanf(buf, CSI18_Rp "%hu;%hut", &rows, &columns) != 2)
                أرجع -EINVAL;

        *ret_rows = rows;
        *ret_columns = columns;
        أرجع 0;
}

/* Determine terminal dimensions by means of an ANSI CSI 18 sequence.
 *
 * Caller must have already opened a non-blocking input fd and configured termios (echo/icanon off). */
ساكن صحيح terminal_query_size_by_csi18(
                صحيح nonblock_تێخستن_fd,
                صحيح output_fd,
                unsigned *ret_rows,
                unsigned *ret_columns) {

        صحيح r;

        أكد(nonblock_تێخستن_fd >= 0);
        أكد(output_fd >= 0);

        r = loop_write_full(output_fd, CSI18_Q, SIZE_MAX, CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        إذا (r < 0)
                أرجع r;

        usec_t end = usec_add(now(CLOCK_MONOTONIC), CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        حرف buf[STRLEN(CSI18_R1)];
        size_t bytes = 0;

        لكل (;;) {
                usec_t n = now(CLOCK_MONOTONIC);
                إذا (n >= end)
                        أرجع -EOPNOTSUPP;

                r = fd_wait_for_event(nonblock_تێخستن_fd, POLLIN, usec_sub_unsigned(end, n));
                إذا (r < 0)
                        أرجع r;
                إذا (r == 0)
                        أرجع -EOPNOTSUPP;

                /* On the first read, read multiple characters, i.e. the shortest valid reply. Afterwards
                 * read byte by byte, since we don't want to read too much and drop characters from the input
                 * queue. */
                ssize_t l = اقرأ(nonblock_تێخستن_fd, buf + bytes, bytes == 0 ? STRLEN(CSI18_R0) : 1);
                إذا (l < 0) {
                        إذا (errno == EAGAIN)
                                استمر;
                        أرجع -errno;
                }

                أكد((size_t) l <= حجم(buf) - bytes);
                bytes += l;

                r = scan_text_area_size_response(buf, bytes, ret_rows, ret_columns);
                إذا (r != -EAGAIN)
                        أرجع r;

                إذا (bytes == حجم(buf))
                        أرجع -EOPNOTSUPP; /* The response has the right prefix, but we didn't find a valid
                                             * answer with a terminator in the allotted space. Something is
                                             * wrong, possibly some unrelated bytes got injected into the
                                             * answer. */
        }
}

صحيح terminal_get_size(
                صحيح تێخستن_fd,
                صحيح output_fd,
                unsigned *ret_rows,
                unsigned *ret_columns,
                bool try_dsr,
                bool try_csi18) {

        _cleanup_close_ صحيح nonblock_تێخستن_fd = -EBADF;
        بنية termios old_termios = TERMIOS_NULL;
        CLEANUP_TERMIOS_RESET(nonblock_تێخستن_fd, old_termios);
        _cleanup_(nonblock_resetp) صحيح nonblock_reset = -EBADF;
        صحيح r;

        أكد(try_dsr || try_csi18);

        r = terminal_prepare_query(تێخستن_fd, output_fd, &nonblock_تێخستن_fd, &old_termios);
        إذا (r < 0)
                أرجع r;

        /* Put the output fd in non-blocking mode with a write timeout, to avoid blocking indefinitely on
         * write if the terminal is not consuming data (e.g. serial console with flow control). */
        r = fd_nonblock(output_fd, true);
        إذا (r < 0)
                أرجع log_debug_errno(r, "Failed to set terminal to non-blocking mode: %m");
        إذا (r > 0)
                nonblock_reset = output_fd;

        /* Flush any stale input that might confuse the response parsers. */
        (فراغ) tcflush(nonblock_تێخستن_fd, TCIFLUSH);

        إذا (try_csi18) {
                r = terminal_query_size_by_csi18(nonblock_تێخستن_fd, output_fd, ret_rows, ret_columns);
                إذا (r >= 0)
                        أرجع r;

                /* Query failed. Flush any outstanding input. */
                (فراغ) tcflush(nonblock_تێخستن_fd, TCIFLUSH);

                إذا (!IN_SET(r, -EOPNOTSUPP, -EINVAL))
                        أرجع r;
        }

        إذا (try_dsr) {
                r = terminal_query_size_by_dsr(nonblock_تێخستن_fd, output_fd, ret_rows, ret_columns);
                إذا (r >= 0)
                        أرجع r;

                /* Query failed. Flush any outstanding input. */
                (فراغ) tcflush(nonblock_تێخستن_fd, TCIFLUSH);
        }

        أرجع r;
}

ساكن صحيح fd_is_vc(صحيح fd) {
        _cleanup_free_ حرف *name = عدم;
        صحيح r;

        أكد(fd >= 0);

        r = getttyname_harder(fd, &name);
        إذا (r < 0)
                أرجع r;

        أرجع tty_is_vc_resolve(name);
}

صحيح terminal_fix_size(صحيح تێخستن_fd, صحيح output_fd) {
        unsigned rows, columns;
        صحيح r;

        /* Don't ask a virtual console at all. There is no intermediary here that could be wrong about the
         * dimensions: the kernel renders the screen itself, so its winsize is authoritative. */
        r = fd_is_vc(تێخستن_fd);
        إذا (r < 0)
                log_debug_errno(r, "Failed to determine whether terminal is a virtual console, assuming it is not: %m");
        وإلا إذا (r > 0) {
                log_debug("Not querying dimensions of a virtual console, kernel dimensions are authoritative.");
                أرجع 0;
        }

        بنية winsize ws = {};
        إذا (ioctl(output_fd, TIOCGWINSZ, &ws) < 0)
                أرجع log_debug_errno(errno, "Failed to query terminal dimensions, ignoring: %m");

        r = terminal_get_size(تێخستن_fd, output_fd, &rows, &columns, /* try_dsr= */ true, /* try_csi18= */ true);
        إذا (r < 0)
                أرجع log_debug_errno(r, "Failed to acquire terminal dimensions via ANSI sequences, not adjusting terminal dimensions: %m");

        إذا (ws.ws_row == rows && ws.ws_col == columns) {
                log_debug("Terminal dimensions reported via ANSI sequences match currently set terminal dimensions, not changing.");
                أرجع 0;
        }

        ws.ws_col = columns;
        ws.ws_row = rows;

        إذا (ioctl(output_fd, TIOCSWINSZ, &ws) < 0)
                أرجع log_debug_errno(errno, "Failed to update terminal dimensions, ignoring: %m");

        log_debug("Fixed terminal dimensions to %ux%u based on ANSI sequence information.", columns, rows);
        أرجع 1;
}

#define MAX_TERMINFO_LENGTH 64
/* python -c 'print("".join(hex(ord(i))[2:] for i in "name").upper())' */
#define DCS_TERMINFO_Q ANSI_DCS "+q" "6E616D65" ANSI_ST
/* The answer is either 0+r… (invalid) or 1+r… (OK). */
#define DCS_TERMINFO_R0 ANSI_DCS "0+r" ANSI_ST
#define DCS_TERMINFO_R1 ANSI_DCS "1+r" "6E616D65" "=" /* This is followed by Pt ST. */
assert_cc(STRLEN(DCS_TERMINFO_R0) <= STRLEN(DCS_TERMINFO_R1 ANSI_ST));

ساكن صحيح scan_terminfo_response(
                ثابت حرف *buf,
                size_t size,
                حرف **ret_name) {
        صحيح r;

        أكد(buf);
        أكد(ret_name);

        /* Check if we have enough space for the shortest possible answer. */
        إذا (size < STRLEN(DCS_TERMINFO_R0))
                أرجع -EAGAIN;

        /* Check if the terminating sequence is present */
        إذا (memcmp(buf + size - STRLEN(ANSI_ST), ANSI_ST, STRLEN(ANSI_ST)) != 0)
                أرجع -EAGAIN;

        إذا (size <= STRLEN(DCS_TERMINFO_R1 ANSI_ST))
                أرجع -EINVAL;  /* The answer is invalid or empty */

        إذا (memcmp(buf, DCS_TERMINFO_R1, STRLEN(DCS_TERMINFO_R1)) != 0)
                أرجع -EINVAL;  /* The answer is not valid */

        _cleanup_free_ فراغ *dec = عدم;
        size_t dec_size;
        r = unhexmem_full(buf + STRLEN(DCS_TERMINFO_R1), size - STRLEN(DCS_TERMINFO_R1 ANSI_ST),
                          /* secure= */ false,
                          &dec, &dec_size);
        إذا (r < 0)
                أرجع r;

        أكد(((ثابت حرف *) dec)[dec_size] == '\0'); /* unhexmem appends NUL for our convenience */
        إذا (memchr(dec, '\0', dec_size) || string_has_cc(dec, عدم) || !filename_is_valid(dec))
                أرجع -EUCLEAN;

        *ret_name = TAKE_PTR(dec);
        أرجع 0;
}

صحيح terminal_get_terminfo_by_dcs(صحيح fd, حرف **ret_name) {
        صحيح r;

        أكد(fd >= 0);
        أكد(ret_name);

        /* Note: fd must be in non-blocking read-write mode! */

        بنية termios old_termios = TERMIOS_NULL;
        CLEANUP_TERMIOS_RESET(fd, old_termios);

        إذا (tcgetattr(fd, &old_termios) < 0)
                أرجع -errno;

        بنية termios new_termios = old_termios;
        termios_disable_echo(&new_termios);

        إذا (tcsetattr(fd, TCSANOW, &new_termios) < 0)
                أرجع -errno;

        r = loop_write(fd, DCS_TERMINFO_Q, SIZE_MAX);
        إذا (r < 0)
                أرجع r;

        usec_t end = usec_add(now(CLOCK_MONOTONIC), CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        حرف buf[STRLEN(DCS_TERMINFO_R1) + MAX_TERMINFO_LENGTH + STRLEN(ANSI_ST)];
        size_t bytes = 0;

        لكل (;;) {
                usec_t n = now(CLOCK_MONOTONIC);
                إذا (n >= end)
                        أرجع -EOPNOTSUPP;

                r = fd_wait_for_event(fd, POLLIN, usec_sub_unsigned(end, n));
                إذا (r < 0)
                        أرجع r;
                إذا (r == 0)
                        أرجع -EOPNOTSUPP;

                /* On the first read, read multiple characters, i.e. the shortest valid reply. Afterwards
                 * read byte by byte, since we don't want to read too much and drop characters from the input
                 * queue. */
                ssize_t l = اقرأ(fd, buf + bytes, bytes == 0 ? STRLEN(DCS_TERMINFO_R0) : 1);
                إذا (l < 0) {
                        إذا (errno == EAGAIN)
                                استمر;
                        أرجع -errno;
                }

                أكد((size_t) l <= حجم(buf) - bytes);
                bytes += l;

                r = scan_terminfo_response(buf, bytes, ret_name);
                إذا (r != -EAGAIN)
                        أرجع r;

                إذا (bytes == حجم(buf))
                        أرجع -EOPNOTSUPP; /* The response has the right prefix, but we didn't find a valid
                                             * answer with a terminator in the allotted space. Something is
                                             * wrong, possibly some unrelated bytes got injected into the
                                             * answer. */
        }
}

صحيح have_terminfo_file(ثابت حرف *name) {
        /* This is a heuristic check if we have the file, using the directory layout used on
         * current Linux systems. Checks for other layouts can be added later if appropriate. */
        صحيح r;

        أكد(filename_is_valid(name));

        _cleanup_free_ حرف *p = path_join("/usr/share/terminfo", CHAR_TO_STR(name[0]), name);
        إذا (!p)
                أرجع log_oom_debug();

        r = RET_NERRNO(access(p, F_OK));
        إذا (r == -ENOENT)
                أرجع false;
        إذا (r < 0)
                أرجع r;
        أرجع true;
}

صحيح query_term_for_tty(ثابت حرف *tty, حرف **ret_term) {
        _cleanup_free_ حرف *dcs_term = عدم;
        صحيح r;

        أكد(tty);
        أكد(ret_term);

        إذا (tty_is_vc_resolve(tty))
                أرجع strdup_to(ret_term, "linux");

        /* Try to query the terminal implementation that we're on. This will not work in all
         * cases, which is fine, since this is intended to be used as a fallback. */

        _cleanup_close_ صحيح tty_fd = open_terminal(tty, O_RDWR|O_NOCTTY|O_CLOEXEC|O_NONBLOCK);
        إذا (tty_fd < 0)
                أرجع log_debug_errno(tty_fd, "Failed to open %s to query terminfo: %m", tty);

        r = terminal_get_terminfo_by_dcs(tty_fd, &dcs_term);
        إذا (r < 0)
                أرجع log_debug_errno(r, "Failed to query %s for terminfo: %m", tty);

        r = have_terminfo_file(dcs_term);
        إذا (r < 0)
                أرجع log_debug_errno(r, "Failed to look for terminfo %s: %m", dcs_term);
        إذا (r == 0)
                أرجع log_info_errno(SYNTHETIC_ERRNO(ENODATA),
                                      "Terminfo %s not found for %s.", dcs_term, tty);

        *ret_term = TAKE_PTR(dcs_term);
        أرجع 0;
}

صحيح terminal_is_pty_fd(صحيح fd) {
        صحيح r;

        أكد(fd >= 0);

        /* Returns true if we are looking at a pty, i.e. if it's backed by the /dev/pts/ file system */

        إذا (!isatty_safe(fd))
                أرجع false;

        r = is_fs_type_at(fd, عدم, DEVPTS_SUPER_MAGIC);
        إذا (r != 0)
                أرجع r;

        /* The ptmx device is weird, it exists twice, once inside and once outside devpts. To detect the
         * latter case, let's fire off an ioctl() that only works on ptmx devices. */

        صحيح v;
        إذا (ioctl(fd, TIOCGPKT, &v) < 0) {
                إذا (ERRNO_IS_NOT_SUPPORTED(errno))
                        أرجع false;

                أرجع -errno;
        }

        أرجع true;
}

صحيح pty_open_peer(صحيح fd, صحيح mode) {
        أكد(fd >= 0);

        /* Opens the peer PTY using the new race-free TIOCGPTPEER ioctl() (kernel 4.13).
         *
         * This is safe to be called on TTYs from other namespaces. */

        أكد((mode & (O_CREAT|O_PATH|O_DIRECTORY|O_TMPFILE)) == 0);

        /* This replicates the EIO retry logic of open_terminal() in a modified way. */
        لكل (unsigned c = 0;; c++) {
                صحيح peer_fd = ioctl(fd, TIOCGPTPEER, mode);
                إذا (peer_fd >= 0)
                        أرجع peer_fd;

                إذا (errno != EIO)
                        أرجع -errno;

                /* Max 1s in total */
                إذا (c >= 20)
                        أرجع -EIO;

                (فراغ) usleep_safe(50 * USEC_PER_MSEC);
        }
}
