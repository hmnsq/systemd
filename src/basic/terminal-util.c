/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <fcntl.h>
#شامل <linux/kd.h>
#شامل <linux/magic.h>
#شامل <linux/tiocl.h>
#شامل <linux/vt.h>
#شامل <poll.h>
#شامل <signal.h>
#شامل <stdlib.h>
#شامل <sys/inotify.h>
#شامل <sys/ioctl.h>
#شامل <sys/sysmacros.h>
#شامل <termios.h>
#شامل <time.h>
#شامل <unistd.h>

#شامل "alloc-util.h"
#شامل "ansi-color.h"
#شامل "chase.h"
#شامل "devnum-util.h"
#شامل "errno-util.h"
#شامل "extract-word.h"
#شامل "fd-util.h"
#شامل "fileio.h"
#شامل "fs-util.h"
#شامل "hexdecoct.h"
#شامل "inotify-util.h"
#شامل "io-util.h"
#شامل "log.h"
#شامل "namespace-util.h"
#شامل "parse-util.h"
#شامل "path-util.h"
#شامل "pidref.h"
#شامل "proc-cmdline.h"
#شامل "process-util.h"
#شامل "signal-util.h"
#شامل "socket-util.h"
#شامل "stat-util.h"
#شامل "stdio-util.h"
#شامل "string-util.h"
#شامل "strv.h"
#شامل "terminal-util.h"
#شامل "time-util.h"
#شامل "utf8.h"

/* How much to wait when reading/writing ANSI sequences from/to the console */
#define CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC (333 * USEC_PER_MSEC)

ایستا فرار unsigned cached_columns = 0;
ایستا فرار unsigned cached_سطرها = 0;

ایستا فرار صحیح cached_on_tty = -1;
ایستا فرار صحیح cached_on_dev_null = -1;

bool isatty_safe(صحیح fd) {
        أكد(fd >= 0);

        اگر (isatty(fd))
                بازگشت true;

        /* Linux/glibc returns EIO for hung up TTY on isatty(). Which is wrong, the thing doesn't stop being
         * a TTY after all, just because it is temporarily hung up. Let's work around this here, until this
         * is fixed in glibc. See: https://sourceware.org/bugzilla/show_bug.cgi?id=32103 */
        اگر (errno == EIO)
                بازگشت true;

        /* Be resilient if we're working on stdio, since they're set up by parent process. */
        أكد(errno != EBADF || IN_SET(fd, STDIN_FILENO, STDOUT_FILENO, STDERR_FILENO));

        بازگشت false;
}

صحیح chvt(صحیح vt) {
        _cleanup_close_ صحیح fd = -EBADF;

        /* Switch to the specified vt number. If the VT is specified <= 0 switch to the VT the kernel log messages go,
         * if that's configured. */

        fd = open_terminal("/dev/tty0", O_RDWR|O_NOCTTY|O_CLOEXEC|O_NONBLOCK);
        اگر (fd < 0)
                بازگشت fd;

        اگر (vt <= 0) {
                صحیح tiocl[2] = {
                        TIOCL_GETKMSGREDIRECT,
                        0
                };

                اگر (ioctl(fd, TIOCLINUX, tiocl) < 0)
                        بازگشت -errno;

                vt = tiocl[0] <= 0 ? 1 : tiocl[0];
        }

        بازگشت RET_NERRNO(ioctl(fd, VT_ACTIVATE, vt));
}

صحیح read_one_char(FILE *f, نویسه *ret, usec_t t, bool echo, bool *need_nl) {
        _cleanup_free_ نویسه *سطر = پوچ;
        ساختار termios old_termios;
        صحیح r, fd;

        أكد(ret);

        اگر (!f)
                f = stdin;

        /* If this is a terminal, then switch canonical mode off, so that we can read a single
         * character. (Note that fmemopen() streams do not have an fd associated with them, let's handle that
         * nicely.) If 'echo' is false we'll also disable ECHO mode so that the pressed key is not made
         * visible to the user. */
        fd = fileno(f);
        اگر (fd >= 0 && tcgetattr(fd, &old_termios) >= 0) {
                ساختار termios new_termios = old_termios;

                new_termios.c_lflag &= ~(ICANON|(echo ? 0 : ECHO));
                new_termios.c_cc[VMIN] = 1;
                new_termios.c_cc[VTIME] = 0;

                اگر (tcsetattr(fd, TCSANOW, &new_termios) >= 0) {
                        نویسه c;

                        اگر (t != USEC_INFINITY) {
                                اگر (fd_wait_for_event(fd, POLLIN, t) <= 0) {
                                        (تهی) tcsetattr(fd, TCSANOW, &old_termios);
                                        بازگشت -ETIMEDOUT;
                                }
                        }

                        r = safe_fgetc(f, &c);
                        (تهی) tcsetattr(fd, TCSANOW, &old_termios);
                        اگر (r < 0)
                                بازگشت r;
                        اگر (r == 0)
                                بازگشت -EIO;

                        اگر (need_nl)
                                *need_nl = c != '\n';

                        *ret = c;
                        بازگشت 0;
                }
        }

        اگر (t != USEC_INFINITY && fd >= 0) {
                /* Let's wait the specified amount of time for input. When we have no fd we skip this, under
                 * the assumption that this is an fmemopen() stream or so where waiting doesn't make sense
                 * anyway, as the data is either already in the stream or cannot possible be placed there
                 * while we access the stream */

                اگر (fd_wait_for_event(fd, POLLIN, t) <= 0)
                        بازگشت -ETIMEDOUT;
        }

        /* If this is not a terminal, then read a full line instead */

        r = read_سطر(f, 16, &سطر); /* longer than necessary, to eat up UTF-8 chars/vt100 key sequences */
        اگر (r < 0)
                بازگشت r;
        اگر (r == 0)
                بازگشت -EIO;

        اگر (strlen(سطر) != 1)
                بازگشت -EBADMSG;

        اگر (need_nl)
                *need_nl = false;

        *ret = سطر[0];
        بازگشت 0;
}

#define DEFAULT_ASK_REFRESH_USEC (2*USEC_PER_SEC)

صحیح ask_char(نویسه *ret, ثابت نویسه *replies, ثابت نویسه *fmt, ...) {
        صحیح r;

        أكد(ret);
        أكد(replies);
        أكد(fmt);

        برای (;;) {
                va_list ap;
                نویسه c;
                bool need_nl = true;

                fputs(ansi_highlight(), stdout);

                putchar('\r');

                va_start(ap, fmt);
                vprintf(fmt, ap);
                va_end(ap);

                fputs(ansi_normal(), stdout);

                fflush(stdout);

                r = read_one_char(stdin, &c, DEFAULT_ASK_REFRESH_USEC, /* echo= */ true, &need_nl);
                اگر (r < 0) {

                        اگر (r == -ETIMEDOUT)
                                ادامه;

                        اگر (r == -EBADMSG) {
                                puts("Bad input, please try again.");
                                ادامه;
                        }

                        putchar('\n');
                        بازگشت r;
                }

                اگر (need_nl)
                        putchar('\n');

                اگر (strchr(replies, c)) {
                        *ret = c;
                        بازگشت 0;
                }

                puts("Read unexpected character, please try again.");
        }
}

تعریفنوع شمارش CompletionResult{
        COMPLETION_ALREADY,       /* the input string is already complete */
        COMPLETION_FULL,          /* completed the input string to be complete now */
        COMPLETION_PARTIAL,       /* completed the input string so that is still incomplete */
        COMPLETION_NONE,          /* found no matching completion */
        _COMPLETION_RESULT_MAX,
        _COMPLETION_RESULT_INVALID = -EINVAL,
        _COMPLETION_RESULT_ERRNO_MAX = -ERRNO_MAX,
} CompletionResult;

ایستا CompletionResult pick_completion(ثابت نویسه *string, نویسه *ثابت*completions, نویسه **ret) {
        _cleanup_free_ نویسه *found = پوچ;
        bool partial = false;

        أكد(ret);

        string = strempty(string);

        STRV_FOREACH(c, completions) {

                /* Ignore entries that are not actually completions */
                اگر (!startswith(*c, string))
                        ادامه;

                /* Store first completion that matches */
                اگر (!found) {
                        found = strdup(*c);
                        اگر (!found)
                                بازگشت -ENOMEM;

                        ادامه;
                }

                /* If there's another completion that works truncate the one we already found by common
                 * prefix */
                size_t n = str_common_prefix(found, *c);
                اگر (n == SIZE_MAX)
                        ادامه;

                found[n] = 0;
                partial = true;
        }

        *ret = TAKE_PTR(found);

        اگر (!*ret)
                بازگشت COMPLETION_NONE;
        اگر (partial)
                بازگشت COMPLETION_PARTIAL;

        بازگشت streq(string, *ret) ? COMPLETION_ALREADY : COMPLETION_FULL;
}

ایستا تهی clear_by_backspace(size_t n) {
        /* Erase the specified number of character cells backwards on the terminal */
        برای (size_t i = 0; i < n; i++)
                fputs("\b \b", stdout);
}

صحیح ask_string_full(
                نویسه **ret,
                ثابت نویسه *prefill,
                GetCompletionsCallback get_completions,
                تهی *userdata,
                ثابت نویسه *text, ...) {

        va_list ap;
        صحیح r;

        أكد(ret);
        أكد(text);

        _cleanup_free_ نویسه *string = پوچ;
        size_t n = 0;

        اگر (prefill) {
                /* Prefill query with explicit data if specified */

                string = strdup(prefill);
                اگر (!string)
                        بازگشت -ENOMEM;

                n = strlen(string);

        } وگرنه اگر (get_completions) {
                /* Otherwise, figure out what string to preselect the query with */
                _cleanup_strv_free_ نویسه **completions = پوچ;
                r = get_completions("", GET_COMPLETIONS_PRESELECT, &completions, userdata);
                اگر (r < 0)
                        بازگشت r;

                CompletionResult cr = pick_completion(string, completions, &string);
                اگر (cr < 0)
                        بازگشت cr;

                n = strlen_ptr(string);
        }

        /* Output the prompt */
        fputs(ansi_highlight(), stdout);
        va_start(ap, text);
        vprintf(text, ap);
        va_end(ap);
        fputs(ansi_normal(), stdout);
        اگر (string)
                fputs(string, stdout);
        fflush(stdout);

        /* Do interactive logic only if stdin + stdout are connected to the same place. And yes, we could use
         * STDIN_FILENO and STDOUT_FILENO here, but let's be overly correct for once, after all libc allows
         * swapping out stdin/stdout. */
        صحیح fd_درون_داد = fileno(stdin);
        صحیح fd_output = fileno(stdout);
        ساختار termios old_termios = TERMIOS_NULL;
        CLEANUP_TERMIOS_RESET(fd_درون_داد, old_termios);

        اگر (fd_درون_داد < 0 || fd_output < 0 || same_fd(fd_درون_داد, fd_output) <= 0)
                برو fallback;

        /* Try to disable echo, which also tells us if this even is a terminal */
        اگر (tcgetattr(fd_درون_داد, &old_termios) < 0) {
                old_termios = TERMIOS_NULL;
                برو fallback;
        }

        ساختار termios new_termios = old_termios;
        termios_disable_echo(&new_termios);
        اگر (tcsetattr(fd_درون_داد, TCSANOW, &new_termios) < 0)
                بازگشت -errno;

        برای (;;) {
                صحیح c = fgetc(stdin);

                /* On EOF or NUL, end the request, don't output anything anymore */
                اگر (IN_SET(c, EOF, 0))
                        بشکن;

                /* On Return also end the request, but make this visible */
                اگر (IN_SET(c, '\n', '\r')) {
                        fputc('\n', stdout);
                        بشکن;
                }

                اگر (c == '\t') {
                        /* Tab */

                        _cleanup_strv_free_ نویسه **completions = پوچ;
                        اگر (get_completions) {
                                r = get_completions(string, /* flags= */ 0, &completions, userdata);
                                اگر (r < 0)
                                        بازگشت r;
                        }

                        _cleanup_free_ نویسه *new_string = پوچ;
                        CompletionResult cr = pick_completion(string, completions, &new_string);
                        اگر (cr < 0)
                                بازگشت cr;
                        اگر (IN_SET(cr, COMPLETION_PARTIAL, COMPLETION_FULL)) {
                                /* Output the new suffix we learned */
                                fputs(ASSERT_PTR(startswith(new_string, strempty(string))), stdout);

                                /* And update the whole string */
                                free_and_replace(string, new_string);
                                n = strlen(string);
                        }
                        اگر (cr == COMPLETION_NONE)
                                fputc('\a', stdout); /* BEL */

                        اگر (IN_SET(cr, COMPLETION_PARTIAL, COMPLETION_ALREADY)) {
                                /* If this worked only partially, or if the user hit TAB even though we were
                                 * complete already, then show the remaining options (in the latter case just
                                 * the one). */
                                fputc('\n', stdout);

                                _cleanup_strv_free_ نویسه **filtered = strv_filter_prefix(completions, string);
                                اگر (!filtered)
                                        بازگشت -ENOMEM;

                                r = show_menu(filtered,
                                              /* n_columns= */ SIZE_MAX,
                                              /* column_width= */ SIZE_MAX,
                                              /* ellipsize_percentage= */ 0,
                                              /* grey_prefix= */ string,
                                              /* with_numbers= */ false);
                                اگر (r < 0)
                                        بازگشت r;

                                /* Show the prompt again */
                                fputs(ansi_highlight(), stdout);
                                va_start(ap, text);
                                vprintf(text, ap);
                                va_end(ap);
                                fputs(ansi_normal(), stdout);
                                fputs(string, stdout);
                        }

                } وگرنه اگر (IN_SET(c, '\b', 127)) {
                        /* Backspace */

                        اگر (n == 0)
                                fputc('\a', stdout); /* BEL */
                        وگرنه {
                                size_t m = utf8_last_length(string, n);

                                نویسه *e = string + n - m;
                                clear_by_backspace(utf8_console_width(e));

                                *e = 0;
                                n -= m;
                        }

                } وگرنه اگر (c == 21) {
                        /* Ctrl-u → erase all input */

                        clear_by_backspace(utf8_console_width(string));
                        اگر (string)
                                string[n = 0] = 0;
                        وگرنه
                                أكد(n == 0);

                } وگرنه اگر (c == 4) {
                        /* Ctrl-d → cancel this field input */

                        بازگشت -ECANCELED;

                } وگرنه اگر (char_is_cc(c) || n >= LINE_MAX)
                        /* refuse control characters and too long strings */
                        fputc('\a', stdout); /* BEL */
                وگرنه {
                        /* Regular char */

                        اگر (!GREEDY_REALLOC(string, n+2))
                                بازگشت -ENOMEM;

                        string[n++] = (نویسه) c;
                        string[n] = 0;

                        fputc(c, stdout);
                }

                fflush(stdout);
        }

        اگر (!string) {
                string = strdup("");
                اگر (!string)
                        بازگشت -ENOMEM;
        }

        *ret = TAKE_PTR(string);
        بازگشت 0;

fallback:
        /* A simple fallback without TTY magic */
        string = mfree(string);
        r = read_سطر(stdin, LONG_سطر_MAX, &string);
        اگر (r < 0)
                بازگشت r;
        اگر (r == 0)
                بازگشت -EIO;

        *ret = TAKE_PTR(string);
        بازگشت 0;
}

bool any_کلید_to_proceed(تهی) {

        /* Insert a new line here as well as to when the user inputs, as this is also used during the boot up
         * sequence when status messages may be interleaved with the current program output. This ensures
         * that the status messages aren't appended on the same line as this message. */

        fputc('\n', stdout);
        fputs(ansi_highlight_magenta(), stdout);
        fputs("-- Press any key to proceed --", stdout);
        fputs(ansi_normal(), stdout);
        fputc('\n', stdout);
        fflush(stdout);

        نویسه کلید = 0;
        (تهی) read_one_char(stdin, &کلید, USEC_INFINITY, /* echo= */ false, /* need_nl= */ پوچ);

        fputc('\n', stdout);
        fflush(stdout);

        بازگشت کلید != 'q';
}

ایستا size_t widest_list_element(نویسه *ثابت*l) {
        size_t w = 0;

        /* Returns the largest console width of all elements in 'l' */

        STRV_FOREACH(i, l)
                w = MAX(w, utf8_console_width(*i));

        بازگشت w;
}

صحیح show_menu(نویسه **x,
              size_t n_columns,
              size_t column_width,
              unsigned ellipsize_percentage,
              ثابت نویسه *grey_prefix,
              bool with_numbers) {

        أكد(n_columns > 0);
        أكد(column_width > 0);

        اگر (n_columns == SIZE_MAX)
                n_columns = 3;

        اگر (column_width == SIZE_MAX) {
                size_t widest = widest_list_element(x);

                /* If not specified, derive column width from screen width */
                size_t column_max = (columns()-1) / n_columns;

                /* Subtract room for numbers */
                اگر (with_numbers)
                        column_max = LESS_BY(column_max, 6U);

                /* If columns would get too tight let's make this a linear list instead. */
                اگر (column_max < 10 && widest > 10) {
                        n_columns = 1;
                        column_max = columns()-1;

                        اگر (with_numbers)
                                column_max = LESS_BY(column_max, 6U);
                }

                /* Never make this narrower than 10 characters */
                column_max = MAX(column_max, 10U);
                column_width = CLAMP(widest+1, 10U, column_max);
        }

        size_t n = strv_length(x);
        size_t per_column = DIV_ROUND_UP(n, n_columns);

        size_t break_سطرها = سطرها();
        اگر (break_سطرها > 2)
                break_سطرها--;

        /* The first page gets two extra lines, since we want to show a title */
        size_t break_modulo = break_سطرها;
        اگر (break_modulo > 3)
                break_modulo -= 3;

        برای (size_t i = 0; i < per_column; i++) {

                برای (size_t j = 0; j < n_columns; j++) {
                        _cleanup_free_ نویسه *e = پوچ;

                        size_t p = j * per_column + i;
                        اگر (p >= n)
                                بشکن;

                        e = ellipsize(x[p], column_width, ellipsize_percentage);
                        اگر (!e)
                                بازگشت -ENOMEM;

                        اگر (with_numbers)
                                printf("%s%4zu)%s ",
                                       ansi_grey(),
                                       p + 1,
                                       ansi_normal());

                        اگر (grey_prefix && startswith(e, grey_prefix)) {
                                size_t k = MIN(strlen(grey_prefix), column_width);
                                printf("%s%.*s%s",
                                       ansi_grey(),
                                       (صحیح) k, e,
                                       ansi_normal());
                                printf("%-*s",
                                       (صحیح) (column_width - k), e+k);
                        } وگرنه
                                printf("%-*s", (صحیح) column_width, e);
                }

                putchar('\n');

                /* on the first screen we reserve 2 extra lines for the title */
                اگر (i % break_سطرها == break_modulo)
                        اگر (!any_کلید_to_proceed())
                                بازگشت 0;
        }

        بازگشت 0;
}

صحیح open_terminal(ثابت نویسه *name, صحیح mode) {
        _cleanup_close_ صحیح fd = -EBADF;

        /*
         * If a TTY is in the process of being closed opening it might cause EIO. This is horribly awful, but
         * unlikely to be changed in the kernel. Hence we work around this problem by retrying a couple of
         * times.
         *
         * https://bugs.launchpad.net/ubuntu/+source/linux/+bug/554172/comments/245
         */

        أكد((mode & (O_CREAT|O_PATH|O_DIRECTORY|O_TMPFILE)) == 0);

        برای (unsigned c = 0;; c++) {
                fd = open(name, mode, 0);
                اگر (fd >= 0)
                        بشکن;

                اگر (errno != EIO)
                        بازگشت -errno;

                /* Max 1s in total */
                اگر (c >= 20)
                        بازگشت -EIO;

                (تهی) usleep_safe(50 * USEC_PER_MSEC);
        }

        اگر (!isatty_safe(fd))
                بازگشت -ENOTTY;

        بازگشت TAKE_FD(fd);
}

صحیح acquire_terminal(
                ثابت نویسه *name,
                AcquireTerminalFlags flags,
                usec_t timeout) {

        _cleanup_close_ صحیح notify = -EBADF, fd = -EBADF;
        usec_t ts = USEC_INFINITY;
        صحیح r, wd = -1;

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

        اگر (mode == ACQUIRE_TERMINAL_WAIT) {
                notify = inotify_init1(IN_CLOEXEC | IN_NONBLOCK);
                اگر (notify < 0)
                        بازگشت -errno;

                wd = inotify_add_watch(notify, name, IN_CLOSE);
                اگر (wd < 0)
                        بازگشت -errno;

                اگر (timeout != USEC_INFINITY)
                        ts = now(CLOCK_MONOTONIC);
        }

        /* If we are called with ACQUIRE_TERMINAL_WATCH_SIGTERM we'll unblock SIGTERM during ppoll() temporarily */
        sigset_t poll_ss;
        assert_se(sigprocmask(SIG_SETMASK, /* newset= */ پوچ, &poll_ss) >= 0);
        اگر (flags & ACQUIRE_TERMINAL_WATCH_SIGTERM) {
                assert_se(sigismember(&poll_ss, SIGTERM) > 0);
                assert_se(sigdelset(&poll_ss, SIGTERM) >= 0);
        }

        برای (;;) {
                اگر (notify >= 0) {
                        r = flush_fd(notify);
                        اگر (r < 0)
                                بازگشت r;
                }

                /* We pass here O_NOCTTY only so that we can check the return value TIOCSCTTY and have a reliable way
                 * to figure out if we successfully became the controlling process of the tty */
                fd = open_terminal(name, O_RDWR|O_NOCTTY|O_CLOEXEC);
                اگر (fd < 0)
                        بازگشت fd;

                /* Temporarily ignore SIGHUP, so that we don't get SIGHUP'ed if we already own the tty. */
                ساختار sigaction sa_old;
                assert_se(sigaction(SIGHUP, &sigaction_ignore, &sa_old) >= 0);

                /* First, try to get the tty */
                r = RET_NERRNO(ioctl(fd, TIOCSCTTY, mode == ACQUIRE_TERMINAL_FORCE));

                /* Reset signal handler to old value */
                assert_se(sigaction(SIGHUP, &sa_old, پوچ) >= 0);

                /* Success? Exit the loop now! */
                اگر (r >= 0)
                        بشکن;

                /* Any failure besides -EPERM? Fail, regardless of the mode. */
                اگر (r != -EPERM)
                        بازگشت r;

                اگر (flags & ACQUIRE_TERMINAL_PERMISSIVE) /* If we are in permissive mode, then EPERM is fine, turn this
                                                          * into a success. Note that EPERM is also returned if we
                                                          * already are the owner of the TTY. */
                        بشکن;

                اگر (mode != ACQUIRE_TERMINAL_WAIT) /* If we are in TRY or FORCE mode, then propagate EPERM as EPERM */
                        بازگشت r;

                أكد(notify >= 0);
                أكد(wd >= 0);

                برای (;;) {
                        usec_t چپ;
                        اگر (timeout == USEC_INFINITY)
                                چپ = USEC_INFINITY;
                        وگرنه {
                                أكد(ts != USEC_INFINITY);

                                usec_t n = usec_sub_unsigned(now(CLOCK_MONOTONIC), ts);
                                اگر (n >= timeout)
                                        بازگشت -ETIMEDOUT;

                                چپ = timeout - n;
                        }

                        r = ppoll_usec_full(
                                        &(ساختار pollfd) {
                                                .fd = notify,
                                                .events = POLLIN,
                                        },
                                        /* n_fds= */ 1,
                                        چپ,
                                        &poll_ss);
                        اگر (r < 0)
                                بازگشت r;
                        اگر (r == 0)
                                بازگشت -ETIMEDOUT;

                        اجتماع inotify_event_buffer buffer;
                        ssize_t l;
                        l = اقرأ(notify, &buffer, اندازه(buffer));
                        اگر (l < 0) {
                                اگر (ERRNO_IS_TRANSIENT(errno))
                                        ادامه;

                                بازگشت -errno;
                        }

                        FOREACH_INOTIFY_EVENT(e, buffer, l) {
                                اگر (e->mask & IN_Q_OVERFLOW) /* If we hit an inotify queue overflow, simply check if the terminal is up for grabs now. */
                                        بشکن;

                                اگر (e->wd != wd || !(e->mask & IN_CLOSE)) /* Safety checks */
                                        بازگشت -EIO;
                        }

                        بشکن;
                }

                /* We close the tty fd here since if the old session ended our handle will be dead. It's important that
                 * we do this after sleeping, so that we don't enter an endless loop. */
                fd = safe_close(fd);
        }

        بازگشت TAKE_FD(fd);
}

صحیح release_terminal(تهی) {
        _cleanup_close_ صحیح fd = -EBADF;
        صحیح r;

        fd = open("/dev/tty", O_RDWR|O_NOCTTY|O_CLOEXEC|O_NONBLOCK);
        اگر (fd < 0)
                بازگشت -errno;

        /* Temporarily ignore SIGHUP, so that we don't get SIGHUP'ed
         * by our own TIOCNOTTY */
        ساختار sigaction sa_old;
        assert_se(sigaction(SIGHUP, &sigaction_ignore, &sa_old) >= 0);

        r = RET_NERRNO(ioctl(fd, TIOCNOTTY));

        assert_se(sigaction(SIGHUP, &sa_old, پوچ) >= 0);

        بازگشت r;
}

صحیح terminal_new_session(تهی) {

        /* Make us the new session leader, and set stdin tty to be our controlling terminal.
         *
         * Why stdin? Well, the ctty logic is relevant for signal delivery mostly, i.e. if people hit C-c
         * or the line is hung up. Such events are basically just a form of input, via a side channel
         * (that side channel being signal delivery, i.e. SIGINT, SIGHUP et al). Hence we focus on input,
         * not output here. */

        اگر (!isatty_safe(STDIN_FILENO))
                بازگشت -ENXIO;

        (تهی) setsid();
        بازگشت RET_NERRNO(ioctl(STDIN_FILENO, TIOCSCTTY, 0));
}

تهی terminal_detach_session(تهی) {
        (تهی) setsid();
        (تهی) release_terminal();
}

صحیح terminal_vhangup_fd(صحیح fd) {
        أكد(fd >= 0);
        بازگشت RET_NERRNO(ioctl(fd, TIOCVHANGUP));
}

صحیح terminal_vhangup(ثابت نویسه *tty) {
        _cleanup_close_ صحیح fd = -EBADF;

        أكد(tty);

        fd = open_terminal(tty, O_RDWR|O_NOCTTY|O_CLOEXEC);
        اگر (fd < 0)
                بازگشت fd;

        بازگشت terminal_vhangup_fd(fd);
}

صحیح vt_disallocate(ثابت نویسه *tty_path) {
        أكد(tty_path);

        /* Deallocate the VT if possible. If not possible (i.e. because it is the active one), at least clear
         * it entirely (including the scrollback buffer). */

        صحیح ttynr = vtnr_from_tty(tty_path);
        اگر (ttynr > 0) {
                _cleanup_close_ صحیح fd = open_terminal("/dev/tty0", O_RDWR|O_NOCTTY|O_CLOEXEC|O_NONBLOCK);
                اگر (fd < 0)
                        بازگشت fd;

                /* Try to deallocate */
                اگر (ioctl(fd, VT_DISALLOCATE, ttynr) >= 0)
                        بازگشت 0;
                اگر (errno != EBUSY)
                        بازگشت -errno;
        }

        /* So this is not a VT (in which case we cannot deallocate it), or we failed to deallocate. Let's at
         * least clear the screen. */

        _cleanup_close_ صحیح fd2 = open_terminal(tty_path, O_WRONLY|O_NOCTTY|O_CLOEXEC|O_NONBLOCK);
        اگر (fd2 < 0)
                بازگشت fd2;

        بازگشت loop_write_full(fd2,
                               "\033[r"   /* clear scrolling region */
                               "\033[H"   /* move home */
                               "\033[3J"  /* clear screen including scrollback, requires Linux 2.6.40 */
                               "\033c",   /* reset to initial state */
                               SIZE_MAX,
                               CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
}

ایستا صحیح vt_default_utf8(تهی) {
        /* Read the default VT UTF8 setting from the kernel */
        بازگشت read_boolean_file("/sys/module/vt/parameters/default_utf8");
}

ایستا صحیح vt_reset_keyboard(صحیح fd) {
        صحیح r, kb;

        أكد(fd >= 0);

        /* If we can't read the default, then default to Unicode. It's 2024 after all. */
        r = vt_default_utf8();
        اگر (r < 0)
                log_debug_errno(r, "Failed to determine kernel VT UTF-8 mode, assuming enabled: %m");

        kb = vt_default_utf8() != 0 ? K_UNICODE : K_XLATE;
        بازگشت RET_NERRNO(ioctl(fd, KDSKBMODE, kb));
}

ایستا صحیح terminal_reset_ioctl(صحیح fd, bool switch_to_text) {
        ساختار termios termios;
        صحیح r;

        /* Set terminal to some sane defaults */

        أكد(fd >= 0);

        /* We leave locked terminal attributes untouched, so that Plymouth may set whatever it wants to set,
         * and we don't interfere with that. */

        /* Disable exclusive mode, just in case */
        اگر (ioctl(fd, TIOCNXCL) < 0)
                log_debug_errno(errno, "TIOCNXCL ioctl failed on TTY, ignoring: %m");

        /* Switch to text mode */
        اگر (switch_to_text)
                اگر (ioctl(fd, KDSETMODE, KD_TEXT) < 0)
                        log_debug_errno(errno, "KDSETMODE ioctl for switching to text mode failed on TTY, ignoring: %m");

        /* Set default keyboard mode */
        r = vt_reset_keyboard(fd);
        اگر (r < 0)
                log_debug_errno(r, "Failed to reset VT keyboard, ignoring: %m");

        اگر (tcgetattr(fd, &termios) < 0) {
                r = log_debug_errno(errno, "Failed to get terminal parameters: %m");
                برو finish;
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
        اگر (r < 0)
                log_debug_errno(r, "Failed to set terminal parameters: %m");

finish:
        /* Just in case, flush all crap out */
        (تهی) tcflush(fd, TCIOFLUSH);

        بازگشت r;
}

صحیح terminal_reset_ansi_seq(صحیح fd) {
        _cleanup_(nonblock_resetp) صحیح nonblock_reset = -EBADF;
        صحیح r;

        أكد(fd >= 0);

        اگر (getenv_terminal_is_dumb())
                بازگشت 0;

        r = fd_nonblock(fd, true);
        اگر (r < 0)
                بازگشت log_debug_errno(r, "Failed to set terminal to non-blocking mode: %m");
        اگر (r > 0)
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
        اگر (r < 0)
                log_debug_errno(r, "Failed to reset terminal through ANSI sequences: %m");

        بازگشت r;
}

تهی reset_dev_console_fd(صحیح fd, bool switch_to_text) {
        صحیح r;

        أكد(fd >= 0);

        _cleanup_close_ صحیح lock_fd = lock_dev_console();
        اگر (lock_fd < 0)
                log_debug_errno(lock_fd, "Failed to lock /dev/console, ignoring: %m");

        r = terminal_reset_ioctl(fd, switch_to_text);
        اگر (r < 0)
                log_warning_errno(r, "Failed to reset /dev/console, ignoring: %m");

        unsigned rows, cols;
        r = proc_cmdline_tty_size("/dev/console", &rows, &cols);
        اگر (r < 0)
                log_warning_errno(r, "Failed to get /dev/console size, ignoring: %m");
        وگرنه اگر (r > 0) {
                r = terminal_set_size_fd(fd, پوچ, rows, cols);
                اگر (r < 0)
                        log_warning_errno(r, "Failed to set configured terminal size on /dev/console, ignoring: %m");
        } وگرنه
                (تهی) terminal_fix_size(fd, fd);

        r = terminal_reset_ansi_seq(fd);
        اگر (r < 0)
                log_warning_errno(r, "Failed to reset /dev/console using ANSI sequences, ignoring: %m");
}

صحیح lock_dev_console(تهی) {
        _cleanup_close_ صحیح fd = -EBADF;
        صحیح r;

        /* NB: We do not use O_NOFOLLOW here, because some container managers might place a symlink to some
         * pty in /dev/console, in which case it should be fine to lock the target TTY. */
        fd = open_terminal("/dev/console", O_RDONLY|O_CLOEXEC|O_NOCTTY);
        اگر (fd < 0)
                بازگشت fd;

        r = lock_generic(fd, LOCK_BSD, LOCK_EX);
        اگر (r < 0)
                بازگشت r;

        بازگشت TAKE_FD(fd);
}

صحیح make_console_stdio(bool switch_to_text) {
        صحیح fd, r;

        /* Make /dev/console the controlling terminal and stdin/stdout/stderr, if we can. If we can't use
         * /dev/null instead. This is particularly useful if /dev/console is turned off, e.g. if console=null
         * is specified on the kernel command line. */

        fd = acquire_terminal("/dev/console", ACQUIRE_TERMINAL_FORCE|ACQUIRE_TERMINAL_PERMISSIVE, USEC_INFINITY);
        اگر (fd < 0) {
                log_warning_errno(fd, "Failed to acquire terminal, using /dev/null stdin/stdout/stderr instead: %m");

                r = make_null_stdio();
                اگر (r < 0)
                        بازگشت log_error_errno(r, "Failed to make /dev/null stdin/stdout/stderr: %m");

        } وگرنه {
                reset_dev_console_fd(fd, switch_to_text);

                r = rearrange_stdio(fd, fd, fd); /* This invalidates 'fd' both on success and on failure. */
                اگر (r < 0)
                        بازگشت log_error_errno(r, "Failed to make terminal stdin/stdout/stderr: %m");
        }

        reset_terminal_feature_caches();
        بازگشت 0;
}

ایستا صحیح vtnr_from_tty_raw(ثابت نویسه *tty, unsigned *ret) {
        أكد(tty);

        tty = skip_dev_prefix(tty);

        ثابت نویسه *e = startswith(tty, "tty");
        اگر (!e)
                بازگشت -EINVAL;

        بازگشت safe_atou(e, ret);
}

صحیح vtnr_from_tty(ثابت نویسه *tty) {
        unsigned u;
        صحیح r;

        أكد(tty);

        r = vtnr_from_tty_raw(tty, &u);
        اگر (r < 0)
                بازگشت r;
        اگر (!vtnr_is_valid(u))
                بازگشت -ERANGE;

        بازگشت (صحیح) u;
}

bool tty_is_vc(ثابت نویسه *tty) {
        أكد(tty);

        /* NB: for >= 0 values no range check is conducted here, on the assumption that the caller will
         * either extract vtnr through vtnr_from_tty() later where ERANGE would be reported, or doesn't care
         * about whether it's strictly valid, but only asking "does this fall into the vt category?", for which
         * "yes" seems to be a better answer. */

        بازگشت vtnr_from_tty_raw(tty, /* ret= */ پوچ) >= 0;
}

bool tty_is_console(ثابت نویسه *tty) {
        أكد(tty);

        بازگشت streq(skip_dev_prefix(tty), "console");
}

صحیح resolve_dev_console(نویسه **ret) {
        صحیح r;

        أكد(ret);

        /* Resolve where /dev/console is pointing to. If /dev/console is a symlink (like in container
         * managers), we'll just resolve the symlink. If it's a real device node, we'll use if
         * /sys/class/tty/tty0/active, but only if /sys/ is actually ours (i.e. not read-only-mounted which
         * is a sign for container setups). */

        _cleanup_free_ نویسه *chased = پوچ;
        r = chase("/dev/console", /* root= */ پوچ, /* flags= */ 0, &chased, /* ret_fd= */ پوچ);
        اگر (r < 0)
                بازگشت r;
        اگر (!path_equal(chased, "/dev/console")) {
                *ret = TAKE_PTR(chased);
                بازگشت 0;
        }

        r = path_is_read_only_fs("/sys");
        اگر (r < 0)
                بازگشت r;
        اگر (r > 0)
                بازگشت -ENOMEDIUM;

        _cleanup_free_ نویسه *active = پوچ;
        r = read_one_سطر_file("/sys/class/tty/console/active", &active);
        اگر (r < 0)
                بازگشت r;
        اگر (r == 0)
                بازگشت -ENXIO;

        /* If multiple log outputs are configured the last one is what /dev/console points to */
        ثابت نویسه *tty = strrchr(active, ' ');
        اگر (tty)
                tty++;
        وگرنه
                tty = active;

        اگر (streq(tty, "tty0")) {
                active = mfree(active);

                /* Get the active VC (e.g. tty1) */
                r = read_one_سطر_file("/sys/class/tty/tty0/active", &active);
                اگر (r < 0)
                        بازگشت r;
                اگر (r == 0)
                        بازگشت -ENXIO;

                tty = active;
        }

        _cleanup_free_ نویسه *path = پوچ;
        path = path_join("/dev", tty);
        اگر (!path)
                بازگشت -ENOMEM;

        *ret = TAKE_PTR(path);
        بازگشت 0;
}

صحیح get_kernel_consoles(نویسه ***ret) {
        _cleanup_strv_free_ نویسه **l = پوچ;
        _cleanup_free_ نویسه *سطر = پوچ;
        صحیح r;

        أكد(ret);

        /* If /sys/ is mounted read-only this means we are running in some kind of container environment.
         * In that case /sys/ would reflect the host system, not us, hence ignore the data we can read from it. */
        اگر (path_is_read_only_fs("/sys") > 0)
                برو fallback;

        r = read_one_سطر_file("/sys/class/tty/console/active", &سطر);
        اگر (r < 0)
                بازگشت r;

        برای (ثابت نویسه *p = سطر;;) {
                _cleanup_free_ نویسه *tty = پوچ, *path = پوچ;

                r = extract_first_word(&p, &tty, پوچ, 0);
                اگر (r < 0)
                        بازگشت r;
                اگر (r == 0)
                        بشکن;

                اگر (streq(tty, "tty0")) {
                        tty = mfree(tty);
                        r = read_one_سطر_file("/sys/class/tty/tty0/active", &tty);
                        اگر (r < 0)
                                بازگشت r;
                        اگر (r == 0) {
                                log_debug("No VT active, skipping /dev/tty0.");
                                ادامه;
                        }
                }

                path = path_join("/dev", tty);
                اگر (!path)
                        بازگشت -ENOMEM;

                اگر (access(path, F_OK) < 0) {
                        log_debug_errno(errno, "Console device %s is not accessible, skipping: %m", path);
                        ادامه;
                }

                r = strv_consume(&l, TAKE_PTR(path));
                اگر (r < 0)
                        بازگشت r;
        }

        اگر (strv_isempty(l)) {
                log_debug("No devices found for system console");
                برو fallback;
        }

        *ret = TAKE_PTR(l);
        بازگشت strv_length(*ret);

fallback:
        r = strv_extend(&l, "/dev/console");
        اگر (r < 0)
                بازگشت r;

        *ret = TAKE_PTR(l);
        بازگشت 0;
}

bool tty_is_vc_resolve(ثابت نویسه *tty) {
        _cleanup_free_ نویسه *resolved = پوچ;

        أكد(tty);

        اگر (streq(skip_dev_prefix(tty), "console")) {
                اگر (resolve_dev_console(&resolved) < 0)
                        بازگشت false;

                tty = resolved;
        }

        بازگشت tty_is_vc(tty);
}

صحیح fd_columns(صحیح fd) {
        ساختار winsize ws = {};

        اگر (fd < 0)
                بازگشت -EBADF;

        اگر (ioctl(fd, TIOCGWINSZ, &ws) < 0)
                بازگشت -errno;

        اگر (ws.ws_col <= 0)
                بازگشت -ENODATA; /* some tty types come up with invalid row/column initially, return a recognizable error for that */

        بازگشت ws.ws_col;
}

صحیح getenv_columns(تهی) {
        صحیح r;

        ثابت نویسه *e = getenv("COLUMNS");
        اگر (!e)
                بازگشت -ENXIO;

        unsigned c;
        r = safe_atou_bounded(e, 1, USHRT_MAX, &c);
        اگر (r < 0)
                بازگشت r;

        بازگشت (صحیح) c;
}

unsigned columns(تهی) {

        اگر (cached_columns > 0)
                بازگشت cached_columns;

        صحیح c = getenv_columns();
        اگر (c < 0) {
                c = fd_columns(STDOUT_FILENO);
                اگر (c < 0)
                        c = 80;
        }

        أكد(c > 0);

        cached_columns = c;
        بازگشت cached_columns;
}

صحیح fd_سطرها(صحیح fd) {
        ساختار winsize ws = {};

        اگر (fd < 0)
                بازگشت -EBADF;

        اگر (ioctl(fd, TIOCGWINSZ, &ws) < 0)
                بازگشت -errno;

        اگر (ws.ws_row <= 0)
                بازگشت -ENODATA; /* some tty types come up with invalid row/column initially, return a recognizable error for that */

        بازگشت ws.ws_row;
}

unsigned سطرها(تهی) {
        ثابت نویسه *e;
        صحیح l;

        اگر (cached_سطرها > 0)
                بازگشت cached_سطرها;

        l = 0;
        e = getenv("LINES");
        اگر (e)
                (تهی) safe_atoi(e, &l);

        اگر (l <= 0 || l > USHRT_MAX) {
                l = fd_سطرها(STDOUT_FILENO);
                اگر (l <= 0)
                        l = 24;
        }

        cached_سطرها = l;
        بازگشت cached_سطرها;
}

صحیح terminal_set_size_fd(صحیح fd, ثابت نویسه *ident, unsigned rows, unsigned cols) {
        ساختار winsize ws;

        أكد(fd >= 0);

        اگر (!ident)
                ident = "TTY";

        اگر (rows == UINT_MAX && cols == UINT_MAX)
                بازگشت 0;

        اگر (ioctl(fd, TIOCGWINSZ, &ws) < 0)
                بازگشت log_debug_errno(errno,
                                       "TIOCGWINSZ ioctl for getting %s size failed, not setting terminal size: %m",
                                       ident);

        اگر (rows == UINT_MAX)
                rows = ws.ws_row;
        وگرنه اگر (rows > USHRT_MAX)
                rows = USHRT_MAX;

        اگر (cols == UINT_MAX)
                cols = ws.ws_col;
        وگرنه اگر (cols > USHRT_MAX)
                cols = USHRT_MAX;

        اگر (rows == ws.ws_row && cols == ws.ws_col)
                بازگشت 0;

        ws.ws_row = rows;
        ws.ws_col = cols;

        اگر (ioctl(fd, TIOCSWINSZ, &ws) < 0)
                بازگشت log_debug_errno(errno, "TIOCSWINSZ ioctl for setting %s size failed: %m", ident);

        بازگشت 0;
}

صحیح proc_cmdline_tty_size(ثابت نویسه *tty, unsigned *ret_rows, unsigned *ret_cols) {
        _cleanup_free_ نویسه *rowskey = پوچ, *rowsvalue = پوچ, *colskey = پوچ, *colsvalue = پوچ;
        unsigned rows = UINT_MAX, cols = UINT_MAX;
        صحیح r;

        أكد(tty);

        اگر (!ret_rows && !ret_cols)
                بازگشت 0;

        tty = skip_dev_prefix(tty);
        اگر (path_startswith(tty, "pts/"))
                بازگشت -EMEDIUMTYPE;
        اگر (!in_charset(tty, ALPHANUMERICAL))
                بازگشت log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "TTY name '%s' contains non-alphanumeric characters, not searching kernel cmdline for size.", tty);

        rowskey = strjoin("systemd.tty.rows.", tty);
        اگر (!rowskey)
                بازگشت -ENOMEM;

        colskey = strjoin("systemd.tty.columns.", tty);
        اگر (!colskey)
                بازگشت -ENOMEM;

        r = proc_cmdline_get_کلید_many(/* flags= */ 0,
                                      rowskey, &rowsvalue,
                                      colskey, &colsvalue);
        اگر (r < 0)
                بازگشت log_debug_errno(r, "Failed to read TTY size of %s from kernel cmdline: %m", tty);

        اگر (rowsvalue) {
                r = safe_atou(rowsvalue, &rows);
                اگر (r < 0)
                        بازگشت log_debug_errno(r, "Failed to parse %s=%s: %m", rowskey, rowsvalue);
        }

        اگر (colsvalue) {
                r = safe_atou(colsvalue, &cols);
                اگر (r < 0)
                        بازگشت log_debug_errno(r, "Failed to parse %s=%s: %m", colskey, colsvalue);
        }

        اگر (ret_rows)
                *ret_rows = rows;
        اگر (ret_cols)
                *ret_cols = cols;

        بازگشت rows != UINT_MAX || cols != UINT_MAX;
}

/* intended to be used as a SIGWINCH sighandler */
تهی columns_سطرها_cache_reset(صحیح signum) {
        cached_columns = 0;
        cached_سطرها = 0;
}

تهی reset_terminal_feature_caches(تهی) {
        cached_columns = 0;
        cached_سطرها = 0;

        cached_on_tty = -1;
        cached_on_dev_null = -1;

        reset_ansi_feature_caches();
}

bool on_tty(تهی) {

        /* We check both stdout and stderr, so that situations where pipes on the shell are used are reliably
         * recognized, regardless if only the output or the errors are piped to some place. Since on_tty() is generally
         * used to default to a safer, non-interactive, non-color mode of operation it's probably good to be defensive
         * here, and check for both. Note that we don't check for STDIN_FILENO, because it should fine to use fancy
         * terminal functionality when outputting stuff, even if the input is piped to us. */

        اگر (cached_on_tty < 0)
                cached_on_tty =
                        isatty_safe(STDOUT_FILENO) &&
                        isatty_safe(STDERR_FILENO);

        بازگشت cached_on_tty;
}

صحیح getttyname_malloc(صحیح fd, نویسه **ret) {
        نویسه path[PATH_MAX]; /* PATH_MAX is counted *with* the trailing NUL byte */
        صحیح r;

        أكد(fd >= 0);
        أكد(ret);

        r = ttyname_r(fd, path, اندازه path); /* positive error */
        أكد(r >= 0);
        اگر (r == ERANGE)
                بازگشت -ENAMETOOLONG;
        اگر (r > 0)
                بازگشت -r;

        بازگشت strdup_to(ret, skip_dev_prefix(path));
}

صحیح getttyname_harder(صحیح fd, نویسه **ret) {
        _cleanup_free_ نویسه *s = پوچ;
        صحیح r;

        أكد(ret);

        r = getttyname_malloc(fd, &s);
        اگر (r < 0)
                بازگشت r;

        اگر (streq(s, "tty"))
                بازگشت get_ctty(0, پوچ, ret);

        *ret = TAKE_PTR(s);
        بازگشت 0;
}

صحیح get_ctty_devnr(pid_t pid, dev_t *ret) {
        _cleanup_free_ نویسه *سطر = پوچ;
        unsigned long ttynr;
        ثابت نویسه *p;
        صحیح r;

        أكد(pid >= 0);

        p = procfs_file_alloca(pid, "stat");
        r = read_one_سطر_file(p, &سطر);
        اگر (r < 0)
                بازگشت r;

        p = strrchr(سطر, ')');
        اگر (!p)
                بازگشت -EIO;

        p++;

        اگر (sscanf(p, " "
                   "%*c "  /* state */
                   "%*d "  /* ppid */
                   "%*d "  /* pgrp */
                   "%*d "  /* session */
                   "%lu ", /* ttynr */
                   &ttynr) != 1)
                بازگشت -EIO;

        اگر (devnum_is_zero(ttynr))
                بازگشت -ENXIO;

        اگر (ret)
                *ret = (dev_t) ttynr;

        بازگشت 0;
}

صحیح get_ctty(pid_t pid, dev_t *ret_devnr, نویسه **ret) {
        نویسه pty[STRLEN("/dev/pts/") + DECIMAL_STR_MAX(dev_t) + 1];
        _cleanup_free_ نویسه *buf = پوچ;
        ثابت نویسه *fn = پوچ, *w;
        dev_t devnr;
        صحیح r;

        r = get_ctty_devnr(pid, &devnr);
        اگر (r < 0)
                بازگشت r;

        r = device_path_make_canonical(S_IFCHR, devnr, &buf);
        اگر (r < 0) {
                ساختار stat st;

                اگر (r != -ENOENT) /* No symlink for this in /dev/char/? */
                        بازگشت r;

                /* Maybe this is PTY? PTY devices are not listed in /dev/char/, as they don't follow the
                 * Linux device model and hence device_path_make_canonical() doesn't work for them. Let's
                 * assume this is a PTY for a moment, and check if the device node this would then map to in
                 * /dev/pts/ matches the one we are looking for. This way we don't have to hardcode the major
                 * number (which is 136 btw), but we still rely on the fact that PTY numbers map directly to
                 * the minor number of the pty. */
                xsprintf(pty, "/dev/pts/%u", minor(devnr));

                اگر (stat(pty, &st) < 0) {
                        اگر (errno != ENOENT)
                                بازگشت -errno;

                } وگرنه اگر (S_ISCHR(st.st_mode) && devnr == st.st_rdev) /* Bingo! */
                        fn = pty;

                اگر (!fn) {
                        /* Doesn't exist, or not a PTY? Probably something similar to the PTYs which have no
                         * symlink in /dev/char/. Let's return something vaguely useful. */
                        r = device_path_make_major_minor(S_IFCHR, devnr, &buf);
                        اگر (r < 0)
                                بازگشت r;

                        fn = buf;
                }
        } وگرنه
                fn = buf;

        w = path_startswith(fn, "/dev/");
        اگر (!w)
                بازگشت -EINVAL;

        اگر (ret) {
                r = strdup_to(ret, w);
                اگر (r < 0)
                        بازگشت r;
        }

        اگر (ret_devnr)
                *ret_devnr = devnr;

        بازگشت 0;
}

صحیح ptsname_malloc(صحیح fd, نویسه **ret) {
        أكد(fd >= 0);
        أكد(ret);

        برای (size_t l = 50;;) {
                _cleanup_free_ نویسه *c = پوچ;

                c = new(نویسه, l);
                اگر (!c)
                        بازگشت -ENOMEM;

                اگر (ptsname_r(fd, c, l) >= 0) {
                        *ret = TAKE_PTR(c);
                        بازگشت 0;
                }
                اگر (errno != ERANGE)
                        بازگشت -errno;

                اگر (!MUL_ASSIGN_SAFE(&l, 2))
                        بازگشت -ENOMEM;
        }
}

صحیح openpt_allocate(صحیح flags, نویسه **ret_peer_path) {
        _cleanup_close_ صحیح fd = -EBADF;
        صحیح r;

        fd = posix_openpt(flags|O_NOCTTY|O_CLOEXEC);
        اگر (fd < 0)
                بازگشت -errno;

        _cleanup_free_ نویسه *p = پوچ;
        اگر (ret_peer_path) {
                r = ptsname_malloc(fd, &p);
                اگر (r < 0)
                        بازگشت r;

                اگر (!path_startswith(p, "/dev/pts/"))
                        بازگشت -EINVAL;
        }

        اگر (unlockpt(fd) < 0)
                بازگشت -errno;

        اگر (ret_peer_path)
                *ret_peer_path = TAKE_PTR(p);

        بازگشت TAKE_FD(fd);
}

ایستا صحیح ptsname_namespace(صحیح pty, نویسه **ret) {
        صحیح no = -1;

        أكد(pty >= 0);
        أكد(ret);

        /* Like ptsname(), but doesn't assume that the path is
         * accessible in the local namespace. */

        اگر (ioctl(pty, TIOCGPTN, &no) < 0)
                بازگشت -errno;

        اگر (no < 0)
                بازگشت -EIO;

        اگر (asprintf(ret, "/dev/pts/%i", no) < 0)
                بازگشت -ENOMEM;

        بازگشت 0;
}

صحیح openpt_allocate_in_namespace(
                ثابت PidRef *pidref,
                صحیح flags,
                نویسه **ret_peer_path) {

        _cleanup_close_ صحیح pidnsfd = -EBADF, mntnsfd = -EBADF, usernsfd = -EBADF, rootfd = -EBADF, fd = -EBADF;
        _cleanup_close_pair_ صحیح pair[2] = EBADF_PAIR;
        صحیح r;

        أكد(pidref);

        r = pidref_namespace_open(pidref, &pidnsfd, &mntnsfd, /* ret_netns_fd= */ پوچ, &usernsfd, &rootfd);
        اگر (r < 0)
                بازگشت log_debug_errno(r, "Failed to open namespaces of PID "PID_FMT": %m", pidref->pid);

        اگر (socketpair(AF_UNIX, SOCK_DGRAM|SOCK_CLOEXEC, 0, pair) < 0)
                بازگشت -errno;

        r = namespace_fork(
                        "(sd-openptns)",
                        "(sd-openpt)",
                        FORK_RESET_SIGNALS|FORK_DEATHSIG_SIGKILL|FORK_WAIT,
                        pidnsfd,
                        mntnsfd,
                        /* netns_fd= */ -EBADF,
                        usernsfd,
                        rootfd,
                        /* ret= */ پوچ);
        اگر (r < 0)
                بازگشت r;
        اگر (r == 0) {
                pair[0] = safe_close(pair[0]);

                fd = openpt_allocate(flags, /* ret_peer_path= */ پوچ);
                اگر (fd < 0)
                        _exit(EXIT_FAILURE);

                اگر (send_one_fd(pair[1], fd, 0) < 0)
                        _exit(EXIT_FAILURE);

                _exit(EXIT_SUCCESS);
        }

        pair[1] = safe_close(pair[1]);

        fd = receive_one_fd(pair[0], 0);
        اگر (fd < 0)
                بازگشت fd;

        اگر (ret_peer_path) {
                r = ptsname_namespace(fd, ret_peer_path);
                اگر (r < 0)
                        بازگشت r;
        }

        بازگشت TAKE_FD(fd);
}

ایستا bool on_dev_null(تهی) {
        ساختار stat dst, ost, est;

        اگر (cached_on_dev_null >= 0)
                بازگشت cached_on_dev_null;

        اگر (stat("/dev/null", &dst) < 0 || fstat(STDOUT_FILENO, &ost) < 0 || fstat(STDERR_FILENO, &est) < 0)
                cached_on_dev_null = false;
        وگرنه
                cached_on_dev_null = stat_inode_same(&dst, &ost) && stat_inode_same(&dst, &est);

        بازگشت cached_on_dev_null;
}

bool term_env_valid(ثابت نویسه *term) {
        /* Checks if the specified $TERM value is suitable for propagation, i.e. is not empty, not set to
         * "unknown" (as is common in CI), and only contains characters valid in terminal type names.
         * Valid $TERM values are things like "xterm-256color", "linux", "screen.xterm-256color", i.e.
         * alphanumeric characters, hyphens, underscores, dots, and plus signs. */
        بازگشت !isempty(term) &&
                !streq(term, "unknown") &&
                in_charset(term, ALPHANUMERICAL "-_+.");
}

bool getenv_terminal_is_dumb(تهی) {
        ثابت نویسه *e;

        e = getenv("TERM");
        اگر (!e)
                بازگشت true;

        بازگشت streq(e, "dumb");
}

bool terminal_is_dumb(تهی) {
        اگر (!on_tty() && !on_dev_null())
                بازگشت true;

        بازگشت getenv_terminal_is_dumb();
}

bool dev_console_colors_enabled(تهی) {
        _cleanup_free_ نویسه *s = پوچ;
        ColorMode m;

        /* Returns true if we assume that color is supported on /dev/console.
         *
         * For that we first check if we explicitly got told to use colors or not, by checking $SYSTEMD_COLORS. If that
         * isn't set we check whether PID 1 has $TERM set, and if not, whether TERM is set on the kernel command
         * line. If we find $TERM set we assume color if it's not set to "dumb", similarly to how regular
         * colors_enabled() operates. */

        m = parse_systemd_colors();
        اگر (m >= 0)
                بازگشت m;

        اگر (getenv("NO_COLOR"))
                بازگشت false;

        اگر (getenv_for_pid(1, "TERM", &s) <= 0)
                (تهی) proc_cmdline_get_کلید("TERM", 0, &s);

        بازگشت !streq_ptr(s, "dumb");
}

صحیح vt_restore(صحیح fd) {

        ایستا ثابت ساختار vt_mode mode = {
                .mode = VT_AUTO,
        };

        صحیح r, ret = 0;

        أكد(fd >= 0);

        اگر (!isatty_safe(fd))
                بازگشت log_debug_errno(SYNTHETIC_ERRNO(ENOTTY), "Asked to restore the VT for an fd that does not refer to a terminal.");

        اگر (ioctl(fd, KDSETMODE, KD_TEXT) < 0)
                RET_GATHER(ret, log_debug_errno(errno, "Failed to set VT to text mode, ignoring: %m"));

        r = vt_reset_keyboard(fd);
        اگر (r < 0)
                RET_GATHER(ret, log_debug_errno(r, "Failed to reset keyboard mode, ignoring: %m"));

        اگر (ioctl(fd, VT_SETMODE, &mode) < 0)
                RET_GATHER(ret, log_debug_errno(errno, "Failed to set VT_AUTO mode, ignoring: %m"));

        r = fchmod_and_chown(fd, TTY_MODE, 0, GID_INVALID);
        اگر (r < 0)
                RET_GATHER(ret, log_debug_errno(r, "Failed to chmod()/chown() VT, ignoring: %m"));

        بازگشت ret;
}

صحیح vt_release(صحیح fd, bool restore) {
        أكد(fd >= 0);

        /* This function releases the VT by acknowledging the VT-switch signal
         * sent by the kernel and optionally reset the VT in text and auto
         * VT-switching modes. */

        اگر (!isatty_safe(fd))
                بازگشت log_debug_errno(SYNTHETIC_ERRNO(ENOTTY), "Asked to release the VT for an fd that does not refer to a terminal.");

        اگر (ioctl(fd, VT_RELDISP, 1) < 0)
                بازگشت -errno;

        اگر (restore)
                بازگشت vt_restore(fd);

        بازگشت 0;
}

تهی get_log_colors(صحیح priority, ثابت نویسه **on, ثابت نویسه **off, ثابت نویسه **highlight) {
        /* Note that this will initialize output variables only when there's something to output.
         * The caller must pre-initialize to "" or NULL as appropriate. */

        اگر (priority <= LOG_ERR) {
                اگر (on)
                        *on = ansi_highlight_red();
                اگر (off)
                        *off = ansi_normal();
                اگر (highlight)
                        *highlight = ansi_highlight();

        } وگرنه اگر (priority <= LOG_WARNING) {
                اگر (on)
                        *on = ansi_highlight_yellow();
                اگر (off)
                        *off = ansi_normal();
                اگر (highlight)
                        *highlight = ansi_highlight();

        } وگرنه اگر (priority <= LOG_NOTICE) {
                اگر (on)
                        *on = ansi_highlight();
                اگر (off)
                        *off = ansi_normal();
                اگر (highlight)
                        *highlight = ansi_highlight_red();

        } وگرنه اگر (priority >= LOG_DEBUG) {
                اگر (on)
                        *on = ansi_grey();
                اگر (off)
                        *off = ansi_normal();
                اگر (highlight)
                        *highlight = ansi_highlight_red();
        }
}

صحیح terminal_set_cursor_position(صحیح fd, unsigned row, unsigned column) {
        أكد(fd >= 0);

        نویسه cursor_position[STRLEN("\x1B[" ";" "H") + DECIMAL_STR_MAX(unsigned) * 2 + 1];
        xsprintf(cursor_position, "\x1B[%u;%uH", row, column);

        بازگشت loop_write(fd, cursor_position, SIZE_MAX);
}

ایستا صحیح terminal_verify_same(صحیح درون_داد_fd, صحیح output_fd) {
        صحیح r;

        أكد(درون_داد_fd >= 0);
        أكد(output_fd >= 0);

        /* Validates that the specified fds reference the same TTY */

        اگر (درون_داد_fd != output_fd) {
                ساختار stat sti;
                اگر (fstat(درون_داد_fd, &sti) < 0)
                        بازگشت -errno;

                r = stat_verify_char(&sti); /* TTYs are character devices */
                اگر (r < 0)
                        بازگشت r;

                ساختار stat sto;
                اگر (fstat(output_fd, &sto) < 0)
                        بازگشت -errno;

                r = stat_verify_char(&sto);
                اگر (r < 0)
                        بازگشت r;

                اگر (sti.st_rdev != sto.st_rdev)
                        بازگشت -ENOLINK;
        }

        اگر (!isatty_safe(درون_داد_fd)) /* The check above was just for char device, but now let's ensure it's actually a tty */
                بازگشت -ENOTTY;

        بازگشت 0;
}

تعریفنوع شمارش CursorPositionState {
        CURSOR_TEXT,
        CURSOR_ESCAPE,
        CURSOR_ROW,
        CURSOR_COLUMN,
} CursorPositionState;

تعریفنوع ساختار CursorPositionContext {
        CursorPositionState state;
        unsigned row, column;
} CursorPositionContext;

ایستا صحیح scan_cursor_position_response(
                CursorPositionContext *context,
                ثابت نویسه *buf,
                size_t size,
                size_t *ret_processed) {

        أكد(context);
        أكد(buf);
        أكد(ret_processed);

        برای (size_t i = 0; i < size; i++) {
                نویسه c = buf[i];

                انتخاب (context->state) {

                حالت CURSOR_TEXT:
                        context->state = c == '\x1B' ? CURSOR_ESCAPE : CURSOR_TEXT;
                        بشکن;

                حالت CURSOR_ESCAPE:
                        context->state = c == '[' ? CURSOR_ROW : CURSOR_TEXT;
                        بشکن;

                حالت CURSOR_ROW:
                        اگر (c == ';')
                                context->state = context->row > 0 ? CURSOR_COLUMN : CURSOR_TEXT;
                        وگرنه {
                                صحیح d = undecchar(c);

                                /* We read a decimal character, let's suffix it to the number we so far read,
                                 * but let's do an overflow check first. */
                                اگر (d < 0 || context->row > (UINT_MAX-d)/10)
                                        context->state = CURSOR_TEXT;
                                وگرنه
                                        context->row = context->row * 10 + d;
                        }
                        بشکن;

                حالت CURSOR_COLUMN:
                        اگر (c == 'R') {
                                اگر (context->column > 0) {
                                        *ret_processed = i + 1;
                                        بازگشت 1; /* success! */
                                }

                                context->state = CURSOR_TEXT;
                        } وگرنه {
                                صحیح d = undecchar(c);

                                /* As above, add the decimal character to our column number */
                                اگر (d < 0 || context->column > (UINT_MAX-d)/10)
                                        context->state = CURSOR_TEXT;
                                وگرنه
                                        context->column = context->column * 10 + d;
                        }

                        بشکن;
                }

                /* Reset any positions we might have picked up */
                اگر (IN_SET(context->state, CURSOR_TEXT, CURSOR_ESCAPE))
                        context->row = context->column = 0;
        }

        *ret_processed = size;
        بازگشت 0; /* all good, but not enough data yet */
}

صحیح terminal_get_cursor_position(
                صحیح درون_داد_fd,
                صحیح output_fd,
                unsigned *ret_row,
                unsigned *ret_column) {

        _cleanup_close_ صحیح nonblock_درون_داد_fd = -EBADF;
        صحیح r;

        أكد(درون_داد_fd >= 0);
        أكد(output_fd >= 0);

        اگر (getenv_terminal_is_dumb())
                بازگشت -EOPNOTSUPP;

        r = terminal_verify_same(درون_داد_fd, output_fd);
        اگر (r < 0)
                بازگشت log_debug_errno(r, "Called with distinct input/output fds: %m");

        /* Failure to reset the terminal is ignored here and in similar cases below.
         * We already have our result; if cleanup fails it doesn't change the validity of the result. */
        ساختار termios old_termios = TERMIOS_NULL;
        CLEANUP_TERMIOS_RESET(درون_داد_fd, old_termios);

        اگر (tcgetattr(درون_داد_fd, &old_termios) < 0)
                بازگشت log_debug_errno(errno, "Failed to get terminal settings: %m");

        ساختار termios new_termios = old_termios;
        termios_disable_echo(&new_termios);

        اگر (tcsetattr(درون_داد_fd, TCSANOW, &new_termios) < 0)
                بازگشت log_debug_errno(errno, "Failed to set new terminal settings: %m");

        /* Request cursor position (DSR/CPR) */
        r = loop_write(output_fd, "\x1B[6n", SIZE_MAX);
        اگر (r < 0)
                بازگشت r;

        /* Open a 2nd input fd, in non-blocking mode, so that we won't ever hang in read() should someone
         * else process the POLLIN. */

        nonblock_درون_داد_fd = r = fd_reopen(درون_داد_fd, O_RDONLY|O_CLOEXEC|O_NONBLOCK|O_NOCTTY);
        اگر (r < 0)
                بازگشت r;

        usec_t end = usec_add(now(CLOCK_MONOTONIC), CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        نویسه buf[STRLEN("\x1B[1;1R")]; /* The shortest valid reply possible */
        size_t buf_full = 0;
        CursorPositionContext context = {};

        برای (bool first = true;; first = false) {
                اگر (buf_full == 0) {
                        usec_t n = now(CLOCK_MONOTONIC);
                        اگر (n >= end)
                                بازگشت -EOPNOTSUPP;

                        r = fd_wait_for_event(nonblock_درون_داد_fd, POLLIN, usec_sub_unsigned(end, n));
                        اگر (r < 0)
                                بازگشت r;
                        اگر (r == 0)
                                بازگشت -EOPNOTSUPP;

                        /* On the first try, read multiple characters, i.e. the shortest valid
                         * reply. Afterwards read byte-wise, since we don't want to read too much, and
                         * unnecessarily drop too many characters from the input queue. */
                        ssize_t l = اقرأ(nonblock_درون_داد_fd, buf, first ? اندازه(buf) : 1);
                        اگر (l < 0) {
                                اگر (errno == EAGAIN)
                                        ادامه;

                                بازگشت -errno;
                        }

                        أكد((size_t) l <= اندازه(buf));
                        buf_full = l;
                }

                size_t processed;
                r = scan_cursor_position_response(&context, buf, buf_full, &processed);
                اگر (r < 0)
                        بازگشت r;

                أكد(processed <= buf_full);
                buf_full -= processed;
                memmove(buf, buf + processed, buf_full);

                اگر (r > 0) {
                        /* Superficial validity check */
                        اگر (context.row >= 32766 || context.column >= 32766)
                                بازگشت -ENODATA;

                        اگر (ret_row)
                                *ret_row = context.row;
                        اگر (ret_column)
                                *ret_column = context.column;

                        بازگشت 0;
                }
        }
}

صحیح terminal_reset_defensive(صحیح fd, TerminalResetFlags flags) {
        صحیح r = 0;

        أكد(fd >= 0);
        أكد(!FLAGS_SET(flags, TERMINAL_RESET_AVOID_ANSI_SEQ|TERMINAL_RESET_FORCE_ANSI_SEQ));

        /* Resets the terminal comprehensively, i.e. via both ioctl()s and via ANSI sequences, but do so only
         * if $TERM is unset or set to "dumb" */

        اگر (!isatty_safe(fd))
                بازگشت -ENOTTY;

        RET_GATHER(r, terminal_reset_ioctl(fd, FLAGS_SET(flags, TERMINAL_RESET_SWITCH_TO_TEXT)));

        اگر (!FLAGS_SET(flags, TERMINAL_RESET_AVOID_ANSI_SEQ) &&
            (FLAGS_SET(flags, TERMINAL_RESET_FORCE_ANSI_SEQ) || !getenv_terminal_is_dumb()))
                RET_GATHER(r, terminal_reset_ansi_seq(fd));

        بازگشت r;
}

صحیح terminal_reset_defensive_locked(صحیح fd, TerminalResetFlags flags) {
        أكد(fd >= 0);

        _cleanup_close_ صحیح lock_fd = lock_dev_console();
        اگر (lock_fd < 0)
                log_debug_errno(lock_fd, "Failed to acquire lock for /dev/console, ignoring: %m");

        بازگشت terminal_reset_defensive(fd, flags);
}

تهی termios_disable_echo(ساختار termios *termios) {
        أكد(termios);

        termios->c_lflag &= ~(ICANON|ECHO);
        termios->c_cc[VMIN] = 1;
        termios->c_cc[VTIME] = 0;
}

ایستا bool termios_is_null(ثابت ساختار termios *t) {
        اگر (!t)
                بازگشت true;

        بازگشت t->c_iflag == UINT_MAX &&
               t->c_oflag == UINT_MAX &&
               t->c_cflag == UINT_MAX &&
               t->c_lflag == UINT_MAX;
}

تهی termios_reset(ثابت TermiosResetContext *c) {
        أكد(c);

        PROTECT_ERRNO;

        اگر (c->fd && *c->fd >= 0 && !termios_is_null(c->termios))
                (تهی) tcsetattr(*c->fd, TCSANOW, c->termios);
}

تعریفنوع شمارش BackgroundColorState {
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

تعریفنوع ساختار BackgroundColorContext {
        BackgroundColorState state;
        uint32_t red, green, blue;
        unsigned red_bits, green_bits, blue_bits;
} BackgroundColorContext;

ایستا صحیح scan_background_color_response(
                BackgroundColorContext *context,
                ثابت نویسه *buf,
                size_t size,
                size_t *ret_processed) {

        أكد(context);
        أكد(buf);
        أكد(ret_processed);

        برای (size_t i = 0; i < size; i++) {
                نویسه c = buf[i];

                انتخاب (context->state) {

                حالت BACKGROUND_TEXT:
                        context->state = c == '\x1B' ? BACKGROUND_ESCAPE : BACKGROUND_TEXT;
                        بشکن;

                حالت BACKGROUND_ESCAPE:
                        context->state = c == ']' ? BACKGROUND_BRACKET : BACKGROUND_TEXT;
                        بشکن;

                حالت BACKGROUND_BRACKET:
                        context->state = c == '1' ? BACKGROUND_FIRST_ONE : BACKGROUND_TEXT;
                        بشکن;

                حالت BACKGROUND_FIRST_ONE:
                        context->state = c == '1' ? BACKGROUND_SECOND_ONE : BACKGROUND_TEXT;
                        بشکن;

                حالت BACKGROUND_SECOND_ONE:
                        context->state = c == ';' ? BACKGROUND_SEMICOLON : BACKGROUND_TEXT;
                        بشکن;

                حالت BACKGROUND_SEMICOLON:
                        context->state = c == 'r' ? BACKGROUND_R : BACKGROUND_TEXT;
                        بشکن;

                حالت BACKGROUND_R:
                        context->state = c == 'g' ? BACKGROUND_G : BACKGROUND_TEXT;
                        بشکن;

                حالت BACKGROUND_G:
                        context->state = c == 'b' ? BACKGROUND_B : BACKGROUND_TEXT;
                        بشکن;

                حالت BACKGROUND_B:
                        context->state = c == ':' ? BACKGROUND_RED : BACKGROUND_TEXT;
                        بشکن;

                حالت BACKGROUND_RED:
                        اگر (c == '/')
                                context->state = context->red_bits > 0 ? BACKGROUND_GREEN : BACKGROUND_TEXT;
                        وگرنه {
                                صحیح d = unhexchar(c);
                                اگر (d < 0 || context->red_bits >= اندازه(context->red)*8)
                                        context->state = BACKGROUND_TEXT;
                                وگرنه {
                                        context->red = (context->red << 4) | d;
                                        context->red_bits += 4;
                                }
                        }
                        بشکن;

                حالت BACKGROUND_GREEN:
                        اگر (c == '/')
                                context->state = context->green_bits > 0 ? BACKGROUND_BLUE : BACKGROUND_TEXT;
                        وگرنه {
                                صحیح d = unhexchar(c);
                                اگر (d < 0 || context->green_bits >= اندازه(context->green)*8)
                                        context->state = BACKGROUND_TEXT;
                                وگرنه {
                                        context->green = (context->green << 4) | d;
                                        context->green_bits += 4;
                                }
                        }
                        بشکن;

                حالت BACKGROUND_BLUE:
                        اگر (c == '\x07') {
                                اگر (context->blue_bits > 0) {
                                        *ret_processed = i + 1;
                                        بازگشت 1; /* success! */
                                }

                                context->state = BACKGROUND_TEXT;
                        } وگرنه اگر (c == '\x1b')
                                context->state = context->blue_bits > 0 ? BACKGROUND_STRING_TERMINATOR : BACKGROUND_TEXT;
                        وگرنه {
                                صحیح d = unhexchar(c);
                                اگر (d < 0 || context->blue_bits >= اندازه(context->blue)*8)
                                        context->state = BACKGROUND_TEXT;
                                وگرنه {
                                        context->blue = (context->blue << 4) | d;
                                        context->blue_bits += 4;
                                }
                        }
                        بشکن;

                حالت BACKGROUND_STRING_TERMINATOR:
                        اگر (c == '\\') {
                                *ret_processed = i + 1;
                                بازگشت 1; /* success! */
                        }

                        context->state = c == ']' ? BACKGROUND_ESCAPE : BACKGROUND_TEXT;
                        بشکن;

                }

                /* Reset any colors we might have picked up */
                اگر (IN_SET(context->state, BACKGROUND_TEXT, BACKGROUND_ESCAPE)) {
                        /* reset color */
                        context->red = context->green = context->blue = 0;
                        context->red_bits = context->green_bits = context->blue_bits = 0;
                }
        }

        *ret_processed = size;
        بازگشت 0; /* all good, but not enough data yet */
}

صحیح get_default_background_color(دوگانه *ret_red, دوگانه *ret_green, دوگانه *ret_blue) {
        صحیح r;

        أكد(ret_red);
        أكد(ret_green);
        أكد(ret_blue);

        اگر (!colors_enabled())
                بازگشت -EOPNOTSUPP;

        r = terminal_verify_same(STDIN_FILENO, STDOUT_FILENO);
        اگر (r < 0)
                بازگشت r;

        اگر (streq_ptr(getenv("TERM"), "linux")) {
                /* Linux console is black */
                *ret_red = *ret_green = *ret_blue = 0.0;
                بازگشت 0;
        }

        /* Open a 2nd input fd, in non-blocking mode, so that we won't ever hang in read()
         * should someone else process the POLLIN. Do all subsequent operations on the new fd. */
        _cleanup_close_ صحیح nonblock_درون_داد_fd = r = fd_reopen(STDIN_FILENO, O_RDONLY|O_CLOEXEC|O_NONBLOCK|O_NOCTTY);
        اگر (r < 0)
                بازگشت r;

        ساختار termios old_termios = TERMIOS_NULL;
        CLEANUP_TERMIOS_RESET(nonblock_درون_داد_fd, old_termios);

        اگر (tcgetattr(nonblock_درون_داد_fd, &old_termios) < 0)
                بازگشت -errno;

        ساختار termios new_termios = old_termios;
        termios_disable_echo(&new_termios);

        اگر (tcsetattr(nonblock_درون_داد_fd, TCSANOW, &new_termios) < 0)
                بازگشت -errno;

        r = loop_write(STDOUT_FILENO, ANSI_OSC "11;?" ANSI_ST, SIZE_MAX);
        اگر (r < 0)
                بازگشت r;

        usec_t end = usec_add(now(CLOCK_MONOTONIC), CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        نویسه buf[STRLEN(ANSI_OSC "11;rgb:0/0/0" ANSI_ST)]; /* shortest possible reply */
        size_t buf_full = 0;
        BackgroundColorContext context = {};

        برای (bool first = true;; first = false) {
                اگر (buf_full == 0) {
                        usec_t n = now(CLOCK_MONOTONIC);
                        اگر (n >= end)
                                بازگشت -EOPNOTSUPP;

                        r = fd_wait_for_event(nonblock_درون_داد_fd, POLLIN, usec_sub_unsigned(end, n));
                        اگر (r < 0)
                                بازگشت r;
                        اگر (r == 0)
                                بازگشت -EOPNOTSUPP;

                        /* On the first try, read multiple characters, i.e. the shortest valid
                         * reply. Afterwards read byte-wise, since we don't want to read too much, and
                         * unnecessarily drop too many characters from the input queue. */
                        ssize_t l = اقرأ(nonblock_درون_داد_fd, buf, first ? اندازه(buf) : 1);
                        اگر (l < 0) {
                                اگر (errno == EAGAIN)
                                        ادامه;
                                بازگشت -errno;
                        }

                        أكد((size_t) l <= اندازه(buf));
                        buf_full = l;
                }

                size_t processed;
                r = scan_background_color_response(&context, buf, buf_full, &processed);
                اگر (r < 0)
                        بازگشت r;

                أكد(processed <= buf_full);
                buf_full -= processed;
                memmove(buf, buf + processed, buf_full);

                اگر (r > 0) {
                        أكد(context.red_bits > 0);
                        *ret_red = (دوگانه) context.red / ((UINT64_C(1) << context.red_bits) - 1);
                        أكد(context.green_bits > 0);
                        *ret_green = (دوگانه) context.green / ((UINT64_C(1) << context.green_bits) - 1);
                        أكد(context.blue_bits > 0);
                        *ret_blue = (دوگانه) context.blue / ((UINT64_C(1) << context.blue_bits) - 1);
                        بازگشت 0;
                }
        }
}

/* Determine terminal dimensions by means of ANSI sequences: save the cursor via DECSC, position it far
 * to the bottom right (clamped to actual terminal dimensions), read back via DSR where we ended up, and
 * restore cursor via DECRC. Only needs a single DSR round-trip, and always restores the cursor regardless
 * of whether the response is received.
 *
 * Caller must have already opened a non-blocking input fd and configured termios (echo/icanon off). */
ایستا صحیح terminal_query_size_by_dsr(
                صحیح nonblock_درون_داد_fd,
                صحیح output_fd,
                unsigned *ret_rows,
                unsigned *ret_columns) {

        صحیح r;

        أكد(nonblock_درون_داد_fd >= 0);
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
        اگر (r < 0)
                بازگشت r;

        usec_t end = usec_add(now(CLOCK_MONOTONIC), CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        نویسه buf[STRLEN("\x1B[1;1R")]; /* The shortest valid reply possible */
        size_t buf_full = 0;
        CursorPositionContext context = {};

        برای (bool first = true;; first = false) {
                اگر (buf_full == 0) {
                        usec_t n = now(CLOCK_MONOTONIC);
                        اگر (n >= end)
                                بازگشت -EOPNOTSUPP;

                        r = fd_wait_for_event(nonblock_درون_داد_fd, POLLIN, usec_sub_unsigned(end, n));
                        اگر (r < 0)
                                بازگشت r;
                        اگر (r == 0)
                                بازگشت -EOPNOTSUPP;

                        /* On the first try, read multiple characters, i.e. the shortest valid
                         * reply. Afterwards read byte-wise, since we don't want to read too much, and
                         * unnecessarily drop too many characters from the input queue. */
                        ssize_t l = اقرأ(nonblock_درون_داد_fd, buf, first ? اندازه(buf) : 1);
                        اگر (l < 0) {
                                اگر (errno == EAGAIN)
                                        ادامه;

                                بازگشت -errno;
                        }

                        أكد((size_t) l <= اندازه(buf));
                        buf_full = l;
                }

                size_t processed;
                r = scan_cursor_position_response(&context, buf, buf_full, &processed);
                اگر (r < 0)
                        بازگشت r;

                أكد(processed <= buf_full);
                buf_full -= processed;
                memmove(buf, buf + processed, buf_full);

                اگر (r > 0) {
                        /* Superficial validity checks (no particular reason to check for < 4, it's
                         * just a way to look for unreasonably small values) */
                        اگر (context.row < 4 || context.column < 4 || context.row >= 32766 || context.column >= 32766)
                                بازگشت -ENODATA;

                        اگر (ret_rows)
                                *ret_rows = context.row;
                        اگر (ret_columns)
                                *ret_columns = context.column;

                        بازگشت 0;
                }
        }
}

/* Common setup for ANSI terminal queries: validate the fds, open a non-blocking input fd, and configure
 * termios with echo and canonical mode disabled. Caller must restore termios and close the fd when done. */
ایستا صحیح terminal_prepare_query(
                صحیح درون_داد_fd,
                صحیح output_fd,
                صحیح *ret_nonblock_fd,
                ساختار termios *ret_saved_termios) {

        صحیح r;

        أكد(درون_داد_fd >= 0);
        أكد(output_fd >= 0);
        أكد(ret_nonblock_fd);
        أكد(ret_saved_termios);

        /* Use getenv_terminal_is_dumb() instead of terminal_is_dumb() here since we operate on an
         * explicitly passed fd, not on stdio. terminal_is_dumb() additionally checks on_tty() which
         * tests whether *stderr* is a tty — that's irrelevant when we're querying a directly opened
         * terminal such as /dev/console. */
        اگر (getenv_terminal_is_dumb())
                بازگشت -EOPNOTSUPP;

        r = terminal_verify_same(درون_داد_fd, output_fd);
        اگر (r < 0)
                بازگشت log_debug_errno(r, "Called with distinct input/output fds: %m");

        /* Open a 2nd input fd, in non-blocking mode, so that we won't ever hang in read()
         * should someone else process the POLLIN. Do all subsequent operations on the new fd. */
        _cleanup_close_ صحیح nonblock_درون_داد_fd = r = fd_reopen(درون_داد_fd, O_RDONLY|O_CLOEXEC|O_NONBLOCK|O_NOCTTY);
        اگر (r < 0)
                بازگشت r;

        اگر (tcgetattr(nonblock_درون_داد_fd, ret_saved_termios) < 0)
                بازگشت log_debug_errno(errno, "Failed to get terminal settings: %m");

        ساختار termios new_termios = *ret_saved_termios;
        termios_disable_echo(&new_termios);

        اگر (tcsetattr(nonblock_درون_داد_fd, TCSANOW, &new_termios) < 0)
                بازگشت log_debug_errno(errno, "Failed to set new terminal settings: %m");

        *ret_nonblock_fd = TAKE_FD(nonblock_درون_داد_fd);
        بازگشت 0;
}

/*
 * See https://terminalguide.namepad.de/seq/csi_st-18/,
 * https://invisible-island.net/xterm/ctlseqs/ctlseqs.html#h3-Functions-using-CSI-_-ordered-by-the-final-character_s_.
 */
#define CSI18_Q  "\x1B[18t"               /* Report the size of the text area in characters */
#define CSI18_Rp "\x1B[8;"                /* Reply prefix */
#define CSI18_R0 CSI18_Rp "1;1t"          /* Shortest reply */
#define CSI18_R1 CSI18_Rp "32766;32766t"  /* Longest reply */

ایستا صحیح scan_text_area_size_response(
                ثابت نویسه *buf,
                size_t size,
                unsigned *ret_rows,
                unsigned *ret_columns) {

        أكد(buf);
        أكد(ret_rows);
        أكد(ret_columns);

        /* Check if we have enough space for the shortest possible answer. */
        اگر (size < STRLEN(CSI18_R0))
                بازگشت -EAGAIN;

        /* Check if the terminating sequence is present */
        اگر (buf[size - 1] != 't')
                بازگشت -EAGAIN;

        unsigned short rows, columns;
        اگر (sscanf(buf, CSI18_Rp "%hu;%hut", &rows, &columns) != 2)
                بازگشت -EINVAL;

        *ret_rows = rows;
        *ret_columns = columns;
        بازگشت 0;
}

/* Determine terminal dimensions by means of an ANSI CSI 18 sequence.
 *
 * Caller must have already opened a non-blocking input fd and configured termios (echo/icanon off). */
ایستا صحیح terminal_query_size_by_csi18(
                صحیح nonblock_درون_داد_fd,
                صحیح output_fd,
                unsigned *ret_rows,
                unsigned *ret_columns) {

        صحیح r;

        أكد(nonblock_درون_داد_fd >= 0);
        أكد(output_fd >= 0);

        r = loop_write_full(output_fd, CSI18_Q, SIZE_MAX, CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        اگر (r < 0)
                بازگشت r;

        usec_t end = usec_add(now(CLOCK_MONOTONIC), CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        نویسه buf[STRLEN(CSI18_R1)];
        size_t bytes = 0;

        برای (;;) {
                usec_t n = now(CLOCK_MONOTONIC);
                اگر (n >= end)
                        بازگشت -EOPNOTSUPP;

                r = fd_wait_for_event(nonblock_درون_داد_fd, POLLIN, usec_sub_unsigned(end, n));
                اگر (r < 0)
                        بازگشت r;
                اگر (r == 0)
                        بازگشت -EOPNOTSUPP;

                /* On the first read, read multiple characters, i.e. the shortest valid reply. Afterwards
                 * read byte by byte, since we don't want to read too much and drop characters from the input
                 * queue. */
                ssize_t l = اقرأ(nonblock_درون_داد_fd, buf + bytes, bytes == 0 ? STRLEN(CSI18_R0) : 1);
                اگر (l < 0) {
                        اگر (errno == EAGAIN)
                                ادامه;
                        بازگشت -errno;
                }

                أكد((size_t) l <= اندازه(buf) - bytes);
                bytes += l;

                r = scan_text_area_size_response(buf, bytes, ret_rows, ret_columns);
                اگر (r != -EAGAIN)
                        بازگشت r;

                اگر (bytes == اندازه(buf))
                        بازگشت -EOPNOTSUPP; /* The response has the right prefix, but we didn't find a valid
                                             * answer with a terminator in the allotted space. Something is
                                             * wrong, possibly some unrelated bytes got injected into the
                                             * answer. */
        }
}

صحیح terminal_get_size(
                صحیح درون_داد_fd,
                صحیح output_fd,
                unsigned *ret_rows,
                unsigned *ret_columns,
                bool try_dsr,
                bool try_csi18) {

        _cleanup_close_ صحیح nonblock_درون_داد_fd = -EBADF;
        ساختار termios old_termios = TERMIOS_NULL;
        CLEANUP_TERMIOS_RESET(nonblock_درون_داد_fd, old_termios);
        _cleanup_(nonblock_resetp) صحیح nonblock_reset = -EBADF;
        صحیح r;

        أكد(try_dsr || try_csi18);

        r = terminal_prepare_query(درون_داد_fd, output_fd, &nonblock_درون_داد_fd, &old_termios);
        اگر (r < 0)
                بازگشت r;

        /* Put the output fd in non-blocking mode with a write timeout, to avoid blocking indefinitely on
         * write if the terminal is not consuming data (e.g. serial console with flow control). */
        r = fd_nonblock(output_fd, true);
        اگر (r < 0)
                بازگشت log_debug_errno(r, "Failed to set terminal to non-blocking mode: %m");
        اگر (r > 0)
                nonblock_reset = output_fd;

        /* Flush any stale input that might confuse the response parsers. */
        (تهی) tcflush(nonblock_درون_داد_fd, TCIFLUSH);

        اگر (try_csi18) {
                r = terminal_query_size_by_csi18(nonblock_درون_داد_fd, output_fd, ret_rows, ret_columns);
                اگر (r >= 0)
                        بازگشت r;

                /* Query failed. Flush any outstanding input. */
                (تهی) tcflush(nonblock_درون_داد_fd, TCIFLUSH);

                اگر (!IN_SET(r, -EOPNOTSUPP, -EINVAL))
                        بازگشت r;
        }

        اگر (try_dsr) {
                r = terminal_query_size_by_dsr(nonblock_درون_داد_fd, output_fd, ret_rows, ret_columns);
                اگر (r >= 0)
                        بازگشت r;

                /* Query failed. Flush any outstanding input. */
                (تهی) tcflush(nonblock_درون_داد_fd, TCIFLUSH);
        }

        بازگشت r;
}

ایستا صحیح fd_is_vc(صحیح fd) {
        _cleanup_free_ نویسه *name = پوچ;
        صحیح r;

        أكد(fd >= 0);

        r = getttyname_harder(fd, &name);
        اگر (r < 0)
                بازگشت r;

        بازگشت tty_is_vc_resolve(name);
}

صحیح terminal_fix_size(صحیح درون_داد_fd, صحیح output_fd) {
        unsigned rows, columns;
        صحیح r;

        /* Don't ask a virtual console at all. There is no intermediary here that could be wrong about the
         * dimensions: the kernel renders the screen itself, so its winsize is authoritative. */
        r = fd_is_vc(درون_داد_fd);
        اگر (r < 0)
                log_debug_errno(r, "Failed to determine whether terminal is a virtual console, assuming it is not: %m");
        وگرنه اگر (r > 0) {
                log_debug("Not querying dimensions of a virtual console, kernel dimensions are authoritative.");
                بازگشت 0;
        }

        ساختار winsize ws = {};
        اگر (ioctl(output_fd, TIOCGWINSZ, &ws) < 0)
                بازگشت log_debug_errno(errno, "Failed to query terminal dimensions, ignoring: %m");

        r = terminal_get_size(درون_داد_fd, output_fd, &rows, &columns, /* try_dsr= */ true, /* try_csi18= */ true);
        اگر (r < 0)
                بازگشت log_debug_errno(r, "Failed to acquire terminal dimensions via ANSI sequences, not adjusting terminal dimensions: %m");

        اگر (ws.ws_row == rows && ws.ws_col == columns) {
                log_debug("Terminal dimensions reported via ANSI sequences match currently set terminal dimensions, not changing.");
                بازگشت 0;
        }

        ws.ws_col = columns;
        ws.ws_row = rows;

        اگر (ioctl(output_fd, TIOCSWINSZ, &ws) < 0)
                بازگشت log_debug_errno(errno, "Failed to update terminal dimensions, ignoring: %m");

        log_debug("Fixed terminal dimensions to %ux%u based on ANSI sequence information.", columns, rows);
        بازگشت 1;
}

#define MAX_TERMINFO_LENGTH 64
/* python -c 'print("".join(hex(ord(i))[2:] for i in "name").upper())' */
#define DCS_TERMINFO_Q ANSI_DCS "+q" "6E616D65" ANSI_ST
/* The answer is either 0+r… (invalid) or 1+r… (OK). */
#define DCS_TERMINFO_R0 ANSI_DCS "0+r" ANSI_ST
#define DCS_TERMINFO_R1 ANSI_DCS "1+r" "6E616D65" "=" /* This is followed by Pt ST. */
assert_cc(STRLEN(DCS_TERMINFO_R0) <= STRLEN(DCS_TERMINFO_R1 ANSI_ST));

ایستا صحیح scan_terminfo_response(
                ثابت نویسه *buf,
                size_t size,
                نویسه **ret_name) {
        صحیح r;

        أكد(buf);
        أكد(ret_name);

        /* Check if we have enough space for the shortest possible answer. */
        اگر (size < STRLEN(DCS_TERMINFO_R0))
                بازگشت -EAGAIN;

        /* Check if the terminating sequence is present */
        اگر (memcmp(buf + size - STRLEN(ANSI_ST), ANSI_ST, STRLEN(ANSI_ST)) != 0)
                بازگشت -EAGAIN;

        اگر (size <= STRLEN(DCS_TERMINFO_R1 ANSI_ST))
                بازگشت -EINVAL;  /* The answer is invalid or empty */

        اگر (memcmp(buf, DCS_TERMINFO_R1, STRLEN(DCS_TERMINFO_R1)) != 0)
                بازگشت -EINVAL;  /* The answer is not valid */

        _cleanup_free_ تهی *dec = پوچ;
        size_t dec_size;
        r = unhexmem_full(buf + STRLEN(DCS_TERMINFO_R1), size - STRLEN(DCS_TERMINFO_R1 ANSI_ST),
                          /* secure= */ false,
                          &dec, &dec_size);
        اگر (r < 0)
                بازگشت r;

        أكد(((ثابت نویسه *) dec)[dec_size] == '\0'); /* unhexmem appends NUL for our convenience */
        اگر (memchr(dec, '\0', dec_size) || string_has_cc(dec, پوچ) || !filename_is_valid(dec))
                بازگشت -EUCLEAN;

        *ret_name = TAKE_PTR(dec);
        بازگشت 0;
}

صحیح terminal_get_terminfo_by_dcs(صحیح fd, نویسه **ret_name) {
        صحیح r;

        أكد(fd >= 0);
        أكد(ret_name);

        /* Note: fd must be in non-blocking read-write mode! */

        ساختار termios old_termios = TERMIOS_NULL;
        CLEANUP_TERMIOS_RESET(fd, old_termios);

        اگر (tcgetattr(fd, &old_termios) < 0)
                بازگشت -errno;

        ساختار termios new_termios = old_termios;
        termios_disable_echo(&new_termios);

        اگر (tcsetattr(fd, TCSANOW, &new_termios) < 0)
                بازگشت -errno;

        r = loop_write(fd, DCS_TERMINFO_Q, SIZE_MAX);
        اگر (r < 0)
                بازگشت r;

        usec_t end = usec_add(now(CLOCK_MONOTONIC), CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        نویسه buf[STRLEN(DCS_TERMINFO_R1) + MAX_TERMINFO_LENGTH + STRLEN(ANSI_ST)];
        size_t bytes = 0;

        برای (;;) {
                usec_t n = now(CLOCK_MONOTONIC);
                اگر (n >= end)
                        بازگشت -EOPNOTSUPP;

                r = fd_wait_for_event(fd, POLLIN, usec_sub_unsigned(end, n));
                اگر (r < 0)
                        بازگشت r;
                اگر (r == 0)
                        بازگشت -EOPNOTSUPP;

                /* On the first read, read multiple characters, i.e. the shortest valid reply. Afterwards
                 * read byte by byte, since we don't want to read too much and drop characters from the input
                 * queue. */
                ssize_t l = اقرأ(fd, buf + bytes, bytes == 0 ? STRLEN(DCS_TERMINFO_R0) : 1);
                اگر (l < 0) {
                        اگر (errno == EAGAIN)
                                ادامه;
                        بازگشت -errno;
                }

                أكد((size_t) l <= اندازه(buf) - bytes);
                bytes += l;

                r = scan_terminfo_response(buf, bytes, ret_name);
                اگر (r != -EAGAIN)
                        بازگشت r;

                اگر (bytes == اندازه(buf))
                        بازگشت -EOPNOTSUPP; /* The response has the right prefix, but we didn't find a valid
                                             * answer with a terminator in the allotted space. Something is
                                             * wrong, possibly some unrelated bytes got injected into the
                                             * answer. */
        }
}

صحیح have_terminfo_file(ثابت نویسه *name) {
        /* This is a heuristic check if we have the file, using the directory layout used on
         * current Linux systems. Checks for other layouts can be added later if appropriate. */
        صحیح r;

        أكد(filename_is_valid(name));

        _cleanup_free_ نویسه *p = path_join("/usr/share/terminfo", CHAR_TO_STR(name[0]), name);
        اگر (!p)
                بازگشت log_oom_debug();

        r = RET_NERRNO(access(p, F_OK));
        اگر (r == -ENOENT)
                بازگشت false;
        اگر (r < 0)
                بازگشت r;
        بازگشت true;
}

صحیح query_term_for_tty(ثابت نویسه *tty, نویسه **ret_term) {
        _cleanup_free_ نویسه *dcs_term = پوچ;
        صحیح r;

        أكد(tty);
        أكد(ret_term);

        اگر (tty_is_vc_resolve(tty))
                بازگشت strdup_to(ret_term, "linux");

        /* Try to query the terminal implementation that we're on. This will not work in all
         * cases, which is fine, since this is intended to be used as a fallback. */

        _cleanup_close_ صحیح tty_fd = open_terminal(tty, O_RDWR|O_NOCTTY|O_CLOEXEC|O_NONBLOCK);
        اگر (tty_fd < 0)
                بازگشت log_debug_errno(tty_fd, "Failed to open %s to query terminfo: %m", tty);

        r = terminal_get_terminfo_by_dcs(tty_fd, &dcs_term);
        اگر (r < 0)
                بازگشت log_debug_errno(r, "Failed to query %s for terminfo: %m", tty);

        r = have_terminfo_file(dcs_term);
        اگر (r < 0)
                بازگشت log_debug_errno(r, "Failed to look for terminfo %s: %m", dcs_term);
        اگر (r == 0)
                بازگشت log_info_errno(SYNTHETIC_ERRNO(ENODATA),
                                      "Terminfo %s not found for %s.", dcs_term, tty);

        *ret_term = TAKE_PTR(dcs_term);
        بازگشت 0;
}

صحیح terminal_is_pty_fd(صحیح fd) {
        صحیح r;

        أكد(fd >= 0);

        /* Returns true if we are looking at a pty, i.e. if it's backed by the /dev/pts/ file system */

        اگر (!isatty_safe(fd))
                بازگشت false;

        r = is_fs_type_at(fd, پوچ, DEVPTS_SUPER_MAGIC);
        اگر (r != 0)
                بازگشت r;

        /* The ptmx device is weird, it exists twice, once inside and once outside devpts. To detect the
         * latter case, let's fire off an ioctl() that only works on ptmx devices. */

        صحیح v;
        اگر (ioctl(fd, TIOCGPKT, &v) < 0) {
                اگر (ERRNO_IS_NOT_SUPPORTED(errno))
                        بازگشت false;

                بازگشت -errno;
        }

        بازگشت true;
}

صحیح pty_open_peer(صحیح fd, صحیح mode) {
        أكد(fd >= 0);

        /* Opens the peer PTY using the new race-free TIOCGPTPEER ioctl() (kernel 4.13).
         *
         * This is safe to be called on TTYs from other namespaces. */

        أكد((mode & (O_CREAT|O_PATH|O_DIRECTORY|O_TMPFILE)) == 0);

        /* This replicates the EIO retry logic of open_terminal() in a modified way. */
        برای (unsigned c = 0;; c++) {
                صحیح peer_fd = ioctl(fd, TIOCGPTPEER, mode);
                اگر (peer_fd >= 0)
                        بازگشت peer_fd;

                اگر (errno != EIO)
                        بازگشت -errno;

                /* Max 1s in total */
                اگر (c >= 20)
                        بازگشت -EIO;

                (تهی) usleep_safe(50 * USEC_PER_MSEC);
        }
}
