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
#تعریف_کرو CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC (333 * USEC_فی_MSEC)

ساکن متلون غیرعلامتی cached_columns = 0;
ساکن متلون غیرعلامتی cached_سطریں = 0;

ساکن متلون صحیح cached_on_tty = -1;
ساکن متلون صحیح cached_on_dev_null = -1;

bool isatty_محفوظ(صحیح fd) {
        أكد(fd >= 0);

        اگر (isatty(fd))
                واپس true;

        /* Linux/glibc returns EIO for hung up TTY on isatty(). Which is wrong, the thing doesn't stop being
         * a TTY after all, just because it is temporarily hung up. Let's work around this here, until this
         * is fixed in glibc. See: https://sourceware.org/bugzilla/show_bug.cgi?id=32103 */
        اگر (errno == EIO)
                واپس true;

        /* Be resilient if we're working on stdio, since they're set up by parent process. */
        أكد(errno != EBADF || اندر_سیٹ(fd, STDIN_FILENO, STDOUT_FILENO, STDERR_FILENO));

        واپس false;
}

صحیح chvt(صحیح vt) {
        _cleanup_close_ صحیح fd = -EBADF;

        /* Switch to the specified vt number. If the VT is specified <= 0 switch to the VT the kernel log messages go,
         * if that's configured. */

        fd = open_terminal("/dev/tty0", O_RDWR|O_NOCTTY|O_CLOEXEC|O_NONBLOCK);
        اگر (fd < 0)
                واپس fd;

        اگر (vt <= 0) {
                صحیح tiocl[2] = {
                        TIOCL_GETKMSGREDIRECT,
                        0
                };

                اگر (ioctl(fd, TIOCLINUX, tiocl) < 0)
                        واپس -errno;

                vt = tiocl[0] <= 0 ? 1 : tiocl[0];
        }

        واپس RET_NERRNO(ioctl(fd, VT_ACTIVATE, vt));
}

صحیح read_one_char(FILE *f, حرف *ret, usec_t t, bool echo, bool *need_nl) {
        _cleanup_free_ حرف *سطر = تہی;
        ساخت termios old_termios;
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
                ساخت termios new_termios = old_termios;

                new_termios.c_lflag &= ~(ICANON|(echo ? 0 : ECHO));
                new_termios.c_cc[VMIN] = 1;
                new_termios.c_cc[VTIME] = 0;

                اگر (tcsetattr(fd, TCSANOW, &new_termios) >= 0) {
                        حرف c;

                        اگر (t != USEC_INFINITY) {
                                اگر (fd_wait_for_event(fd, POLLIN, t) <= 0) {
                                        (خالی) tcsetattr(fd, TCSANOW, &old_termios);
                                        واپس -ETIMEDOUT;
                                }
                        }

                        r = محفوظ_fgetc(f, &c);
                        (خالی) tcsetattr(fd, TCSANOW, &old_termios);
                        اگر (r < 0)
                                واپس r;
                        اگر (r == 0)
                                واپس -EIO;

                        اگر (need_nl)
                                *need_nl = c != '\n';

                        *ret = c;
                        واپس 0;
                }
        }

        اگر (t != USEC_INFINITY && fd >= 0) {
                /* Let's wait the specified amount of time for input. When we have no fd we skip this, under
                 * the assumption that this is an fmemopen() stream or so where waiting doesn't make sense
                 * anyway, as the data is either already in the stream or cannot possible be placed there
                 * while we access the stream */

                اگر (fd_wait_for_event(fd, POLLIN, t) <= 0)
                        واپس -ETIMEDOUT;
        }

        /* If this is not a terminal, then read a full line instead */

        r = read_سطر(f, 16, &سطر); /* longer than necessary, to eat up UTF-8 chars/vt100 key sequences */
        اگر (r < 0)
                واپس r;
        اگر (r == 0)
                واپس -EIO;

        اگر (لڑی_لمبائی(سطر) != 1)
                واپس -EBADMSG;

        اگر (need_nl)
                *need_nl = false;

        *ret = سطر[0];
        واپس 0;
}

#تعریف_کرو DEFAULT_ASK_REFRESH_USEC (2*USEC_فی_SEC)

صحیح ask_char(حرف *ret, ثابت حرف *replies, ثابت حرف *fmt, ...) {
        صحیح r;

        أكد(ret);
        أكد(replies);
        أكد(fmt);

        برائے (;;) {
                va_list ap;
                حرف c;
                bool need_nl = true;

                fputs(ansi_highlight(), stdout);

                حرف_لکھو('\r');

                va_start(ap, fmt);
                vprintf(fmt, ap);
                va_end(ap);

                fputs(ansi_معمول(), stdout);

                fflush(stdout);

                r = read_one_char(stdin, &c, DEFAULT_ASK_REFRESH_USEC, /* echo= */ true, &need_nl);
                اگر (r < 0) {

                        اگر (r == -ETIMEDOUT)
                                جاری;

                        اگر (r == -EBADMSG) {
                                puts("Bad input, please try again.");
                                جاری;
                        }

                        حرف_لکھو('\n');
                        واپس r;
                }

                اگر (need_nl)
                        حرف_لکھو('\n');

                اگر (strchr(replies, c)) {
                        *ret = c;
                        واپس 0;
                }

                puts("Read unexpected character, please try again.");
        }
}

قسمتعریف شمار Completion_نتیجہ{
        COMPLETION_ALREADY,       /* the input string is already complete */
        COMPLETION_بھرا,          /* completed the input string to be complete now */
        COMPLETION_PARTIAL,       /* completed the input string so that is still incomplete */
        COMPLETION_NONE,          /* found no matching completion */
        COMPLETION_نتیجہ_MAX,
        COMPLETION_نتیجہ_INVALID = -EINVAL,
        COMPLETION_نتیجہ_ERRNO_MAX = -ERRNO_MAX,
} Completion_نتیجہ;

ساکن Completion_نتیجہ pick_completion(ثابت حرف *string, حرف *ثابت*completions, حرف **ret) {
        _cleanup_free_ حرف *ملا = تہی;
        bool partial = false;

        أكد(ret);

        string = strempty(string);

        STRV_FOREACH(c, completions) {

                /* Ignore entries that are not actually completions */
                اگر (!startswith(*c, string))
                        جاری;

                /* Store first completion that matches */
                اگر (!ملا) {
                        ملا = strdup(*c);
                        اگر (!ملا)
                                واپس -ENOMEM;

                        جاری;
                }

                /* If there's another completion that works truncate the one we already found by common
                 * prefix */
                size_t n = str_common_سابقہ(ملا, *c);
                اگر (n == SIZE_MAX)
                        جاری;

                ملا[n] = 0;
                partial = true;
        }

        *ret = TAKE_PTR(ملا);

        اگر (!*ret)
                واپس COMPLETION_NONE;
        اگر (partial)
                واپس COMPLETION_PARTIAL;

        واپس streq(string, *ret) ? COMPLETION_ALREADY : COMPLETION_بھرا;
}

ساکن خالی clear_by_backspace(size_t n) {
        /* Erase the specified number of character cells backwards on the terminal */
        برائے (size_t i = 0; i < n; i++)
                fputs("\b \b", stdout);
}

صحیح ask_string_بھرا(
                حرف **ret,
                ثابت حرف *prefill,
                GetCompletionsCallback get_completions,
                خالی *userdata,
                ثابت حرف *متن, ...) {

        va_list ap;
        صحیح r;

        أكد(ret);
        أكد(متن);

        _cleanup_free_ حرف *string = تہی;
        size_t n = 0;

        اگر (prefill) {
                /* Prefill query with explicit data if specified */

                string = strdup(prefill);
                اگر (!string)
                        واپس -ENOMEM;

                n = لڑی_لمبائی(string);

        } ورنہ اگر (get_completions) {
                /* Otherwise, figure out what string to preselect the query with */
                _cleanup_strv_free_ حرف **completions = تہی;
                r = get_completions("", GET_COMPLETIONS_PRESELECT, &completions, userdata);
                اگر (r < 0)
                        واپس r;

                Completion_نتیجہ cr = pick_completion(string, completions, &string);
                اگر (cr < 0)
                        واپس cr;

                n = strlen_ptr(string);
        }

        /* Output the prompt */
        fputs(ansi_highlight(), stdout);
        va_start(ap, متن);
        vprintf(متن, ap);
        va_end(ap);
        fputs(ansi_معمول(), stdout);
        اگر (string)
                fputs(string, stdout);
        fflush(stdout);

        /* Do interactive logic only if stdin + stdout are connected to the same place. And yes, we could use
         * STDIN_FILENO and STDOUT_FILENO here, but let's be overly correct for once, after all libc allows
         * swapping out stdin/stdout. */
        صحیح fd_داخلہ = fileno(stdin);
        صحیح fd_output = fileno(stdout);
        ساخت termios old_termios = TERMIOS_NULL;
        CLEANUP_TERMIOS_RESET(fd_داخلہ, old_termios);

        اگر (fd_داخلہ < 0 || fd_output < 0 || same_fd(fd_داخلہ, fd_output) <= 0)
                جاؤ fallback;

        /* Try to disable echo, which also tells us if this even is a terminal */
        اگر (tcgetattr(fd_داخلہ, &old_termios) < 0) {
                old_termios = TERMIOS_NULL;
                جاؤ fallback;
        }

        ساخت termios new_termios = old_termios;
        termios_disable_echo(&new_termios);
        اگر (tcsetattr(fd_داخلہ, TCSANOW, &new_termios) < 0)
                واپس -errno;

        برائے (;;) {
                صحیح c = fgetc(stdin);

                /* On EOF or NUL, end the request, don't output anything anymore */
                اگر (اندر_سیٹ(c, EOF, 0))
                        توڑو;

                /* On Return also end the request, but make this visible */
                اگر (اندر_سیٹ(c, '\n', '\r')) {
                        fputc('\n', stdout);
                        توڑو;
                }

                اگر (c == '\t') {
                        /* Tab */

                        _cleanup_strv_free_ حرف **completions = تہی;
                        اگر (get_completions) {
                                r = get_completions(string, /* flags= */ 0, &completions, userdata);
                                اگر (r < 0)
                                        واپس r;
                        }

                        _cleanup_free_ حرف *new_string = تہی;
                        Completion_نتیجہ cr = pick_completion(string, completions, &new_string);
                        اگر (cr < 0)
                                واپس cr;
                        اگر (اندر_سیٹ(cr, COMPLETION_PARTIAL, COMPLETION_بھرا)) {
                                /* Output the new suffix we learned */
                                fputs(ASSERT_PTR(startswith(new_string, strempty(string))), stdout);

                                /* And update the whole string */
                                free_and_replace(string, new_string);
                                n = لڑی_لمبائی(string);
                        }
                        اگر (cr == COMPLETION_NONE)
                                fputc('\a', stdout); /* BEL */

                        اگر (اندر_سیٹ(cr, COMPLETION_PARTIAL, COMPLETION_ALREADY)) {
                                /* If this worked only partially, or if the user hit TAB even though we were
                                 * complete already, then show the remaining options (in the latter case just
                                 * the one). */
                                fputc('\n', stdout);

                                _cleanup_strv_free_ حرف **filtered = strv_filter_سابقہ(completions, string);
                                اگر (!filtered)
                                        واپس -ENOMEM;

                                r = show_menu(filtered,
                                              /* n_columns= */ SIZE_MAX,
                                              /* column_width= */ SIZE_MAX,
                                              /* ellipsize_percentage= */ 0,
                                              /* grey_prefix= */ string,
                                              /* with_numbers= */ false);
                                اگر (r < 0)
                                        واپس r;

                                /* Show the prompt again */
                                fputs(ansi_highlight(), stdout);
                                va_start(ap, متن);
                                vprintf(متن, ap);
                                va_end(ap);
                                fputs(ansi_معمول(), stdout);
                                fputs(string, stdout);
                        }

                } ورنہ اگر (اندر_سیٹ(c, '\b', 127)) {
                        /* Backspace */

                        اگر (n == 0)
                                fputc('\a', stdout); /* BEL */
                        ورنہ {
                                size_t m = utf8_آخری_لمبائی(string, n);

                                حرف *e = string + n - m;
                                clear_by_backspace(utf8_console_چوڑائی(e));

                                *e = 0;
                                n -= m;
                        }

                } ورنہ اگر (c == 21) {
                        /* Ctrl-u → erase all input */

                        clear_by_backspace(utf8_console_چوڑائی(string));
                        اگر (string)
                                string[n = 0] = 0;
                        ورنہ
                                أكد(n == 0);

                } ورنہ اگر (c == 4) {
                        /* Ctrl-d → cancel this field input */

                        واپس -ECANCELED;

                } ورنہ اگر (char_is_cc(c) || n >= LINE_MAX)
                        /* refuse control characters and too long strings */
                        fputc('\a', stdout); /* BEL */
                ورنہ {
                        /* Regular char */

                        اگر (!GREEDY_REALLOC(string, n+2))
                                واپس -ENOMEM;

                        string[n++] = (حرف) c;
                        string[n] = 0;

                        fputc(c, stdout);
                }

                fflush(stdout);
        }

        اگر (!string) {
                string = strdup("");
                اگر (!string)
                        واپس -ENOMEM;
        }

        *ret = TAKE_PTR(string);
        واپس 0;

fallback:
        /* A simple fallback without TTY magic */
        string = mfree(string);
        r = read_سطر(stdin, LONG_سطر_MAX, &string);
        اگر (r < 0)
                واپس r;
        اگر (r == 0)
                واپس -EIO;

        *ret = TAKE_PTR(string);
        واپس 0;
}

bool any_کلید_مقصد_proceed(خالی) {

        /* Insert a new line here as well as to when the user inputs, as this is also used during the boot up
         * sequence when status messages may be interleaved with the current program output. This ensures
         * that the status messages aren't appended on the same line as this message. */

        fputc('\n', stdout);
        fputs(ansi_highlight_magenta(), stdout);
        fputs("-- Press any key to proceed --", stdout);
        fputs(ansi_معمول(), stdout);
        fputc('\n', stdout);
        fflush(stdout);

        حرف کلید = 0;
        (خالی) read_one_char(stdin, &کلید, USEC_INFINITY, /* echo= */ false, /* need_nl= */ تہی);

        fputc('\n', stdout);
        fflush(stdout);

        واپس کلید != 'q';
}

ساکن size_t widest_list_element(حرف *ثابت*l) {
        size_t w = 0;

        /* Returns the largest console width of all elements in 'l' */

        STRV_FOREACH(i, l)
                w = MAX(w, utf8_console_چوڑائی(*i));

        واپس w;
}

صحیح show_menu(حرف **x,
              size_t n_columns,
              size_t عمود_چوڑائی,
              غیرعلامتی ellipsize_percentage,
              ثابت حرف *grey_سابقہ,
              bool with_numbers) {

        أكد(n_columns > 0);
        أكد(عمود_چوڑائی > 0);

        اگر (n_columns == SIZE_MAX)
                n_columns = 3;

        اگر (عمود_چوڑائی == SIZE_MAX) {
                size_t widest = widest_list_element(x);

                /* If not specified, derive column width from screen width */
                size_t عمود_max = (columns()-1) / n_columns;

                /* Subtract room for numbers */
                اگر (with_numbers)
                        عمود_max = LESS_BY(عمود_max, 6U);

                /* If columns would get too tight let's make this a linear list instead. */
                اگر (عمود_max < 10 && widest > 10) {
                        n_columns = 1;
                        عمود_max = columns()-1;

                        اگر (with_numbers)
                                عمود_max = LESS_BY(عمود_max, 6U);
                }

                /* Never make this narrower than 10 characters */
                عمود_max = MAX(عمود_max, 10U);
                عمود_چوڑائی = CLAMP(widest+1, 10U, عمود_max);
        }

        size_t n = strv_لمبائی(x);
        size_t فی_عمود = DIV_باری_اوپر(n, n_columns);

        size_t break_سطریں = سطریں();
        اگر (break_سطریں > 2)
                break_سطریں--;

        /* The first page gets two extra lines, since we want to show a title */
        size_t break_modulo = break_سطریں;
        اگر (break_modulo > 3)
                break_modulo -= 3;

        برائے (size_t i = 0; i < فی_عمود; i++) {

                برائے (size_t j = 0; j < n_columns; j++) {
                        _cleanup_free_ حرف *e = تہی;

                        size_t p = j * فی_عمود + i;
                        اگر (p >= n)
                                توڑو;

                        e = ellipsize(x[p], عمود_چوڑائی, ellipsize_percentage);
                        اگر (!e)
                                واپس -ENOMEM;

                        اگر (with_numbers)
                                لکھو("%s%4zu)%s ",
                                       ansi_grey(),
                                       p + 1,
                                       ansi_معمول());

                        اگر (grey_سابقہ && startswith(e, grey_سابقہ)) {
                                size_t k = MIN(لڑی_لمبائی(grey_سابقہ), عمود_چوڑائی);
                                لکھو("%s%.*s%s",
                                       ansi_grey(),
                                       (صحیح) k, e,
                                       ansi_معمول());
                                لکھو("%-*s",
                                       (صحیح) (عمود_چوڑائی - k), e+k);
                        } ورنہ
                                لکھو("%-*s", (صحیح) عمود_چوڑائی, e);
                }

                حرف_لکھو('\n');

                /* on the first screen we reserve 2 extra lines for the title */
                اگر (i % break_سطریں == break_modulo)
                        اگر (!any_کلید_مقصد_proceed())
                                واپس 0;
        }

        واپس 0;
}

صحیح open_terminal(ثابت حرف *name, صحیح منوال) {
        _cleanup_close_ صحیح fd = -EBADF;

        /*
         * If a TTY is in the process of being closed opening it might cause EIO. This is horribly awful, but
         * unlikely to be changed in the kernel. Hence we work around this problem by retrying a couple of
         * times.
         *
         * https://bugs.launchpad.net/ubuntu/+source/linux/+bug/554172/comments/245
         */

        أكد((منوال & (O_CREAT|O_راستہ|O_DIRECTORY|O_TMPFILE)) == 0);

        برائے (غیرعلامتی c = 0;; c++) {
                fd = open(name, منوال, 0);
                اگر (fd >= 0)
                        توڑو;

                اگر (errno != EIO)
                        واپس -errno;

                /* Max 1s in total */
                اگر (c >= 20)
                        واپس -EIO;

                (خالی) usleep_محفوظ(50 * USEC_فی_MSEC);
        }

        اگر (!isatty_محفوظ(fd))
                واپس -ENOTTY;

        واپس TAKE_FD(fd);
}

صحیح acquire_terminal(
                ثابت حرف *name,
                AcquireTerminalFlags flags,
                usec_t timeout) {

        _cleanup_close_ صحیح notify = -EBADF, fd = -EBADF;
        usec_t ts = USEC_INFINITY;
        صحیح r, wd = -1;

        أكد(name);

        AcquireTerminalFlags منوال = flags & ACQUIRE_TERMINAL_منوال_نقاب;
        أكد(اندر_سیٹ(منوال, ACQUIRE_TERMINAL_TRY, ACQUIRE_TERMINAL_FORCE, ACQUIRE_TERMINAL_WAIT));
        أكد(منوال == ACQUIRE_TERMINAL_WAIT || !FLAGS_سیٹ(flags, ACQUIRE_TERMINAL_WATCH_SIGTERM));

        /* We use inotify to be notified when the tty is closed. We create the watch before checking if we can actually
         * acquire it, so that we don't lose any event.
         *
         * Note: strictly speaking this actually watches for the device being closed, it does *not* really watch
         * whether a tty loses its controlling process. However, unless some rogue process uses TIOCNOTTY on /dev/tty
         * *after* closing its tty otherwise this will not become a problem. As long as the administrator makes sure to
         * not configure any service on the same tty as an untrusted user this should not be a problem. (Which they
         * probably should not do anyway.) */

        اگر (منوال == ACQUIRE_TERMINAL_WAIT) {
                notify = inotify_init1(IN_CLOEXEC | IN_NONBLOCK);
                اگر (notify < 0)
                        واپس -errno;

                wd = inotify_add_watch(notify, name, IN_CLOSE);
                اگر (wd < 0)
                        واپس -errno;

                اگر (timeout != USEC_INFINITY)
                        ts = now(CLOCK_MONOTONIC);
        }

        /* If we are called with ACQUIRE_TERMINAL_WATCH_SIGTERM we'll unblock SIGTERM during ppoll() temporarily */
        sigset_t poll_ss;
        assert_se(sigprocmask(SIG_SETMASK, /* newset= */ تہی, &poll_ss) >= 0);
        اگر (flags & ACQUIRE_TERMINAL_WATCH_SIGTERM) {
                assert_se(sigismember(&poll_ss, SIGTERM) > 0);
                assert_se(sigdelset(&poll_ss, SIGTERM) >= 0);
        }

        برائے (;;) {
                اگر (notify >= 0) {
                        r = flush_fd(notify);
                        اگر (r < 0)
                                واپس r;
                }

                /* We pass here O_NOCTTY only so that we can check the return value TIOCSCTTY and have a reliable way
                 * to figure out if we successfully became the controlling process of the tty */
                fd = open_terminal(name, O_RDWR|O_NOCTTY|O_CLOEXEC);
                اگر (fd < 0)
                        واپس fd;

                /* Temporarily ignore SIGHUP, so that we don't get SIGHUP'ed if we already own the tty. */
                ساخت sigaction sa_old;
                assert_se(sigaction(SIGHUP, &sigaction_ignore, &sa_old) >= 0);

                /* First, try to get the tty */
                r = RET_NERRNO(ioctl(fd, TIOCSCTTY, منوال == ACQUIRE_TERMINAL_FORCE));

                /* Reset signal handler to old value */
                assert_se(sigaction(SIGHUP, &sa_old, تہی) >= 0);

                /* Success? Exit the loop now! */
                اگر (r >= 0)
                        توڑو;

                /* Any failure besides -EPERM? Fail, regardless of the mode. */
                اگر (r != -EPERM)
                        واپس r;

                اگر (flags & ACQUIRE_TERMINAL_PERMISSIVE) /* If we are in permissive mode, then EPERM is fine, turn this
                                                          * into a success. Note that EPERM is also returned if we
                                                          * already are the owner of the TTY. */
                        توڑو;

                اگر (منوال != ACQUIRE_TERMINAL_WAIT) /* If we are in TRY or FORCE mode, then propagate EPERM as EPERM */
                        واپس r;

                أكد(notify >= 0);
                أكد(wd >= 0);

                برائے (;;) {
                        usec_t بایاں;
                        اگر (timeout == USEC_INFINITY)
                                بایاں = USEC_INFINITY;
                        ورنہ {
                                أكد(ts != USEC_INFINITY);

                                usec_t n = usec_sub_unsigned(now(CLOCK_MONOTONIC), ts);
                                اگر (n >= timeout)
                                        واپس -ETIMEDOUT;

                                بایاں = timeout - n;
                        }

                        r = ppoll_usec_بھرا(
                                        &(ساخت pollfd) {
                                                .fd = notify,
                                                .events = POLLIN,
                                        },
                                        /* n_fds= */ 1,
                                        بایاں,
                                        &poll_ss);
                        اگر (r < 0)
                                واپس r;
                        اگر (r == 0)
                                واپس -ETIMEDOUT;

                        اتحاد inotify_event_بفر بفر;
                        ssize_t l;
                        l = اقرأ(notify, &بفر, حجم(بفر));
                        اگر (l < 0) {
                                اگر (ERRNO_IS_TRANSIENT(errno))
                                        جاری;

                                واپس -errno;
                        }

                        FOREACH_INOTIFY_EVENT(e, بفر, l) {
                                اگر (e->نقاب_3 & IN_Q_OVERFLOW) /* If we hit an inotify queue overflow, simply check if the terminal is up for grabs now. */
                                        توڑو;

                                اگر (e->wd != wd || !(e->نقاب_3 & IN_CLOSE)) /* Safety checks */
                                        واپس -EIO;
                        }

                        توڑو;
                }

                /* We close the tty fd here since if the old session ended our handle will be dead. It's important that
                 * we do this after sleeping, so that we don't enter an endless loop. */
                fd = محفوظ_close(fd);
        }

        واپس TAKE_FD(fd);
}

صحیح release_terminal(خالی) {
        _cleanup_close_ صحیح fd = -EBADF;
        صحیح r;

        fd = open("/dev/tty", O_RDWR|O_NOCTTY|O_CLOEXEC|O_NONBLOCK);
        اگر (fd < 0)
                واپس -errno;

        /* Temporarily ignore SIGHUP, so that we don't get SIGHUP'ed
         * by our own TIOCNOTTY */
        ساخت sigaction sa_old;
        assert_se(sigaction(SIGHUP, &sigaction_ignore, &sa_old) >= 0);

        r = RET_NERRNO(ioctl(fd, TIOCNOTTY));

        assert_se(sigaction(SIGHUP, &sa_old, تہی) >= 0);

        واپس r;
}

صحیح terminal_new_session(خالی) {

        /* Make us the new session leader, and set stdin tty to be our controlling terminal.
         *
         * Why stdin? Well, the ctty logic is relevant for signal delivery mostly, i.e. if people hit C-c
         * or the line is hung up. Such events are basically just a form of input, via a side channel
         * (that side channel being signal delivery, i.e. SIGINT, SIGHUP et al). Hence we focus on input,
         * not output here. */

        اگر (!isatty_محفوظ(STDIN_FILENO))
                واپس -ENXIO;

        (خالی) setsid();
        واپس RET_NERRNO(ioctl(STDIN_FILENO, TIOCSCTTY, 0));
}

خالی terminal_detach_session(خالی) {
        (خالی) setsid();
        (خالی) release_terminal();
}

صحیح terminal_vhangup_fd(صحیح fd) {
        أكد(fd >= 0);
        واپس RET_NERRNO(ioctl(fd, TIOCVHANGUP));
}

صحیح terminal_vhangup(ثابت حرف *tty) {
        _cleanup_close_ صحیح fd = -EBADF;

        أكد(tty);

        fd = open_terminal(tty, O_RDWR|O_NOCTTY|O_CLOEXEC);
        اگر (fd < 0)
                واپس fd;

        واپس terminal_vhangup_fd(fd);
}

صحیح vt_disallocate(ثابت حرف *tty_راستہ) {
        أكد(tty_راستہ);

        /* Deallocate the VT if possible. If not possible (i.e. because it is the active one), at least clear
         * it entirely (including the scrollback buffer). */

        صحیح ttynr = vtnr_from_tty(tty_راستہ);
        اگر (ttynr > 0) {
                _cleanup_close_ صحیح fd = open_terminal("/dev/tty0", O_RDWR|O_NOCTTY|O_CLOEXEC|O_NONBLOCK);
                اگر (fd < 0)
                        واپس fd;

                /* Try to deallocate */
                اگر (ioctl(fd, VT_DISALLOCATE, ttynr) >= 0)
                        واپس 0;
                اگر (errno != EBUSY)
                        واپس -errno;
        }

        /* So this is not a VT (in which case we cannot deallocate it), or we failed to deallocate. Let's at
         * least clear the screen. */

        _cleanup_close_ صحیح fd2 = open_terminal(tty_راستہ, O_WRONLY|O_NOCTTY|O_CLOEXEC|O_NONBLOCK);
        اگر (fd2 < 0)
                واپس fd2;

        واپس loop_write_بھرا(fd2,
                               "\033[r"   /* clear scrolling region */
                               "\033[H"   /* move home */
                               "\033[3J"  /* clear screen including scrollback, requires Linux 2.6.40 */
                               "\033c",   /* reset to initial state */
                               SIZE_MAX,
                               CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
}

ساکن صحیح vt_default_utf8(خالی) {
        /* Read the default VT UTF8 setting from the kernel */
        واپس read_boolean_file("/sys/module/vt/parameters/default_utf8");
}

ساکن صحیح vt_reset_keyboard(صحیح fd) {
        صحیح r, kb;

        أكد(fd >= 0);

        /* If we can't read the default, then default to Unicode. It's 2024 after all. */
        r = vt_default_utf8();
        اگر (r < 0)
                log_debug_errno(r, "Failed to determine kernel VT UTF-8 mode, assuming enabled: %m");

        kb = vt_default_utf8() != 0 ? K_UNICODE : K_XLATE;
        واپس RET_NERRNO(ioctl(fd, KDSKBMODE, kb));
}

ساکن صحیح terminal_reset_ioctl(صحیح fd, bool switch_مقصد_متن) {
        ساخت termios termios;
        صحیح r;

        /* Set terminal to some sane defaults */

        أكد(fd >= 0);

        /* We leave locked terminal attributes untouched, so that Plymouth may set whatever it wants to set,
         * and we don't interfere with that. */

        /* Disable exclusive mode, just in case */
        اگر (ioctl(fd, TIOCNXCL) < 0)
                log_debug_errno(errno, "TIOCNXCL ioctl failed on TTY, ignoring: %m");

        /* Switch to text mode */
        اگر (switch_مقصد_متن)
                اگر (ioctl(fd, KDSETMODE, KD_TEXT) < 0)
                        log_debug_errno(errno, "KDSETMODE ioctl for switching to text mode failed on TTY, ignoring: %m");

        /* Set default keyboard mode */
        r = vt_reset_keyboard(fd);
        اگر (r < 0)
                log_debug_errno(r, "Failed to reset VT keyboard, ignoring: %m");

        اگر (tcgetattr(fd, &termios) < 0) {
                r = log_debug_errno(errno, "Failed to get terminal parameters: %m");
                جاؤ اختتام;
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

اختتام:
        /* Just in case, flush all crap out */
        (خالی) tcflush(fd, TCIOFLUSH);

        واپس r;
}

صحیح terminal_reset_ansi_seq(صحیح fd) {
        _cleanup_(nonblock_resetp) صحیح nonblock_reset = -EBADF;
        صحیح r;

        أكد(fd >= 0);

        اگر (getenv_terminal_is_dumb())
                واپس 0;

        r = fd_nonblock(fd, true);
        اگر (r < 0)
                واپس log_debug_errno(r, "Failed to set terminal to non-blocking mode: %m");
        اگر (r > 0)
                nonblock_reset = fd;

        r = loop_write_بھرا(fd,
                            "\033[!p"              /* soft terminal reset */
                            ANSI_OSC "104" ANSI_ST /* reset color palette via OSC 104 */
                            ANSI_معمول            /* reset colors */
                            "\033[?7h"             /* enable line-wrapping */
                            "\033[1G"              /* place cursor at beginning of current line */
                            "\033[0J",             /* erase till end of screen */
                            SIZE_MAX,
                            CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        اگر (r < 0)
                log_debug_errno(r, "Failed to reset terminal through ANSI sequences: %m");

        واپس r;
}

خالی reset_dev_console_fd(صحیح fd, bool switch_مقصد_متن) {
        صحیح r;

        أكد(fd >= 0);

        _cleanup_close_ صحیح تالا_fd = تالا_dev_console();
        اگر (تالا_fd < 0)
                log_debug_errno(تالا_fd, "Failed to lock /dev/console, ignoring: %m");

        r = terminal_reset_ioctl(fd, switch_مقصد_متن);
        اگر (r < 0)
                log_warning_errno(r, "Failed to reset /dev/console, ignoring: %m");

        غیرعلامتی صفیں, ستون_تعداد;
        r = proc_cmdline_tty_سائز("/dev/console", &صفیں, &ستون_تعداد);
        اگر (r < 0)
                log_warning_errno(r, "Failed to get /dev/console size, ignoring: %m");
        ورنہ اگر (r > 0) {
                r = terminal_سیٹ_سائز_fd(fd, تہی, صفیں, ستون_تعداد);
                اگر (r < 0)
                        log_warning_errno(r, "Failed to set configured terminal size on /dev/console, ignoring: %m");
        } ورنہ
                (خالی) terminal_fix_سائز(fd, fd);

        r = terminal_reset_ansi_seq(fd);
        اگر (r < 0)
                log_warning_errno(r, "Failed to reset /dev/console using ANSI sequences, ignoring: %m");
}

صحیح تالا_dev_console(خالی) {
        _cleanup_close_ صحیح fd = -EBADF;
        صحیح r;

        /* NB: We do not use O_NOFOLLOW here, because some container managers might place a symlink to some
         * pty in /dev/console, in which case it should be fine to lock the target TTY. */
        fd = open_terminal("/dev/console", O_RDONLY|O_CLOEXEC|O_NOCTTY);
        اگر (fd < 0)
                واپس fd;

        r = تالا_generic(fd, تالا_BSD, LOCK_EX);
        اگر (r < 0)
                واپس r;

        واپس TAKE_FD(fd);
}

صحیح make_console_stdio(bool switch_مقصد_متن) {
        صحیح fd, r;

        /* Make /dev/console the controlling terminal and stdin/stdout/stderr, if we can. If we can't use
         * /dev/null instead. This is particularly useful if /dev/console is turned off, e.g. if console=null
         * is specified on the kernel command line. */

        fd = acquire_terminal("/dev/console", ACQUIRE_TERMINAL_FORCE|ACQUIRE_TERMINAL_PERMISSIVE, USEC_INFINITY);
        اگر (fd < 0) {
                log_warning_errno(fd, "Failed to acquire terminal, using /dev/null stdin/stdout/stderr instead: %m");

                r = make_null_stdio();
                اگر (r < 0)
                        واپس log_error_errno(r, "Failed to make /dev/null stdin/stdout/stderr: %m");

        } ورنہ {
                reset_dev_console_fd(fd, switch_مقصد_متن);

                r = rearrange_stdio(fd, fd, fd); /* This invalidates 'fd' both on success and on failure. */
                اگر (r < 0)
                        واپس log_error_errno(r, "Failed to make terminal stdin/stdout/stderr: %m");
        }

        reset_terminal_feature_caches();
        واپس 0;
}

ساکن صحیح vtnr_from_tty_raw(ثابت حرف *tty, غیرعلامتی *ret) {
        أكد(tty);

        tty = skip_dev_سابقہ(tty);

        ثابت حرف *e = startswith(tty, "tty");
        اگر (!e)
                واپس -EINVAL;

        واپس محفوظ_atou(e, ret);
}

صحیح vtnr_from_tty(ثابت حرف *tty) {
        غیرعلامتی u;
        صحیح r;

        أكد(tty);

        r = vtnr_from_tty_raw(tty, &u);
        اگر (r < 0)
                واپس r;
        اگر (!vtnr_is_valid(u))
                واپس -ERANGE;

        واپس (صحیح) u;
}

bool tty_is_vc(ثابت حرف *tty) {
        أكد(tty);

        /* NB: for >= 0 values no range check is conducted here, on the assumption that the caller will
         * either extract vtnr through vtnr_from_tty() later where ERANGE would be reported, or doesn't care
         * about whether it's strictly valid, but only asking "does this fall into the vt category?", for which
         * "yes" seems to be a better answer. */

        واپس vtnr_from_tty_raw(tty, /* ret= */ تہی) >= 0;
}

bool tty_is_console(ثابت حرف *tty) {
        أكد(tty);

        واپس streq(skip_dev_سابقہ(tty), "console");
}

صحیح resolve_dev_console(حرف **ret) {
        صحیح r;

        أكد(ret);

        /* Resolve where /dev/console is pointing to. If /dev/console is a symlink (like in container
         * managers), we'll just resolve the symlink. If it's a real device node, we'll use if
         * /sys/class/tty/tty0/active, but only if /sys/ is actually ours (i.e. not read-only-mounted which
         * is a sign for container setups). */

        _cleanup_free_ حرف *chased = تہی;
        r = chase("/dev/console", /* root= */ تہی, /* flags= */ 0, &chased, /* ret_fd= */ تہی);
        اگر (r < 0)
                واپس r;
        اگر (!راستہ_equal(chased, "/dev/console")) {
                *ret = TAKE_PTR(chased);
                واپس 0;
        }

        r = راستہ_is_read_only_fs("/sys");
        اگر (r < 0)
                واپس r;
        اگر (r > 0)
                واپس -ENOMEDIUM;

        _cleanup_free_ حرف *فعال = تہی;
        r = read_one_سطر_file("/sys/class/tty/console/active", &فعال);
        اگر (r < 0)
                واپس r;
        اگر (r == 0)
                واپس -ENXIO;

        /* If multiple log outputs are configured the last one is what /dev/console points to */
        ثابت حرف *tty = strrchr(فعال, ' ');
        اگر (tty)
                tty++;
        ورنہ
                tty = فعال;

        اگر (streq(tty, "tty0")) {
                فعال = mfree(فعال);

                /* Get the active VC (e.g. tty1) */
                r = read_one_سطر_file("/sys/class/tty/tty0/active", &فعال);
                اگر (r < 0)
                        واپس r;
                اگر (r == 0)
                        واپس -ENXIO;

                tty = فعال;
        }

        _cleanup_free_ حرف *راستہ = تہی;
        راستہ = راستہ_join("/dev", tty);
        اگر (!راستہ)
                واپس -ENOMEM;

        *ret = TAKE_PTR(راستہ);
        واپس 0;
}

صحیح get_kernel_consoles(حرف ***ret) {
        _cleanup_strv_free_ حرف **l = تہی;
        _cleanup_free_ حرف *سطر = تہی;
        صحیح r;

        أكد(ret);

        /* If /sys/ is mounted read-only this means we are running in some kind of container environment.
         * In that case /sys/ would reflect the host system, not us, hence ignore the data we can read from it. */
        اگر (راستہ_is_read_only_fs("/sys") > 0)
                جاؤ fallback;

        r = read_one_سطر_file("/sys/class/tty/console/active", &سطر);
        اگر (r < 0)
                واپس r;

        برائے (ثابت حرف *p = سطر;;) {
                _cleanup_free_ حرف *tty = تہی, *راستہ = تہی;

                r = نکالو_پہلا_لفظ(&p, &tty, تہی, 0);
                اگر (r < 0)
                        واپس r;
                اگر (r == 0)
                        توڑو;

                اگر (streq(tty, "tty0")) {
                        tty = mfree(tty);
                        r = read_one_سطر_file("/sys/class/tty/tty0/active", &tty);
                        اگر (r < 0)
                                واپس r;
                        اگر (r == 0) {
                                log_debug("No VT active, skipping /dev/tty0.");
                                جاری;
                        }
                }

                راستہ = راستہ_join("/dev", tty);
                اگر (!راستہ)
                        واپس -ENOMEM;

                اگر (access(راستہ, F_OK) < 0) {
                        log_debug_errno(errno, "Console device %s is not accessible, skipping: %m", راستہ);
                        جاری;
                }

                r = strv_consume(&l, TAKE_PTR(راستہ));
                اگر (r < 0)
                        واپس r;
        }

        اگر (strv_isempty(l)) {
                log_debug("No devices found for system console");
                جاؤ fallback;
        }

        *ret = TAKE_PTR(l);
        واپس strv_لمبائی(*ret);

fallback:
        r = strv_extend(&l, "/dev/console");
        اگر (r < 0)
                واپس r;

        *ret = TAKE_PTR(l);
        واپس 0;
}

bool tty_is_vc_resolve(ثابت حرف *tty) {
        _cleanup_free_ حرف *resolved = تہی;

        أكد(tty);

        اگر (streq(skip_dev_سابقہ(tty), "console")) {
                اگر (resolve_dev_console(&resolved) < 0)
                        واپس false;

                tty = resolved;
        }

        واپس tty_is_vc(tty);
}

صحیح fd_columns(صحیح fd) {
        ساخت winsize ws = {};

        اگر (fd < 0)
                واپس -EBADF;

        اگر (ioctl(fd, TIOCGWINSZ, &ws) < 0)
                واپس -errno;

        اگر (ws.ws_col <= 0)
                واپس -ENODATA; /* some tty types come up with invalid row/column initially, return a recognizable error for that */

        واپس ws.ws_col;
}

صحیح getenv_columns(خالی) {
        صحیح r;

        ثابت حرف *e = getenv("COLUMNS");
        اگر (!e)
                واپس -ENXIO;

        غیرعلامتی c;
        r = محفوظ_atou_bounded(e, 1, USHRT_MAX, &c);
        اگر (r < 0)
                واپس r;

        واپس (صحیح) c;
}

غیرعلامتی columns(خالی) {

        اگر (cached_columns > 0)
                واپس cached_columns;

        صحیح c = getenv_columns();
        اگر (c < 0) {
                c = fd_columns(STDOUT_FILENO);
                اگر (c < 0)
                        c = 80;
        }

        أكد(c > 0);

        cached_columns = c;
        واپس cached_columns;
}

صحیح fd_سطریں(صحیح fd) {
        ساخت winsize ws = {};

        اگر (fd < 0)
                واپس -EBADF;

        اگر (ioctl(fd, TIOCGWINSZ, &ws) < 0)
                واپس -errno;

        اگر (ws.ws_row <= 0)
                واپس -ENODATA; /* some tty types come up with invalid row/column initially, return a recognizable error for that */

        واپس ws.ws_row;
}

غیرعلامتی سطریں(خالی) {
        ثابت حرف *e;
        صحیح l;

        اگر (cached_سطریں > 0)
                واپس cached_سطریں;

        l = 0;
        e = getenv("LINES");
        اگر (e)
                (خالی) محفوظ_atoi(e, &l);

        اگر (l <= 0 || l > USHRT_MAX) {
                l = fd_سطریں(STDOUT_FILENO);
                اگر (l <= 0)
                        l = 24;
        }

        cached_سطریں = l;
        واپس cached_سطریں;
}

صحیح terminal_سیٹ_سائز_fd(صحیح fd, ثابت حرف *ident, غیرعلامتی صفیں, غیرعلامتی ستون_تعداد) {
        ساخت winsize ws;

        أكد(fd >= 0);

        اگر (!ident)
                ident = "TTY";

        اگر (صفیں == UINT_MAX && ستون_تعداد == UINT_MAX)
                واپس 0;

        اگر (ioctl(fd, TIOCGWINSZ, &ws) < 0)
                واپس log_debug_errno(errno,
                                       "TIOCGWINSZ ioctl for getting %s size failed, not setting terminal size: %m",
                                       ident);

        اگر (صفیں == UINT_MAX)
                صفیں = ws.ws_row;
        ورنہ اگر (صفیں > USHRT_MAX)
                صفیں = USHRT_MAX;

        اگر (ستون_تعداد == UINT_MAX)
                ستون_تعداد = ws.ws_col;
        ورنہ اگر (ستون_تعداد > USHRT_MAX)
                ستون_تعداد = USHRT_MAX;

        اگر (صفیں == ws.ws_row && ستون_تعداد == ws.ws_col)
                واپس 0;

        ws.ws_row = صفیں;
        ws.ws_col = ستون_تعداد;

        اگر (ioctl(fd, TIOCSWINSZ, &ws) < 0)
                واپس log_debug_errno(errno, "TIOCSWINSZ ioctl for setting %s size failed: %m", ident);

        واپس 0;
}

صحیح proc_cmdline_tty_سائز(ثابت حرف *tty, غیرعلامتی *ret_صفیں, غیرعلامتی *ret_ستون_تعداد) {
        _cleanup_free_ حرف *rowskey = تہی, *rowsvalue = تہی, *colskey = تہی, *colsvalue = تہی;
        غیرعلامتی صفیں = UINT_MAX, ستون_تعداد = UINT_MAX;
        صحیح r;

        أكد(tty);

        اگر (!ret_صفیں && !ret_ستون_تعداد)
                واپس 0;

        tty = skip_dev_سابقہ(tty);
        اگر (راستہ_startswith(tty, "pts/"))
                واپس -EMEDIUMTYPE;
        اگر (!اندر_charset(tty, ALPHANUMERICAL))
                واپس log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "TTY name '%s' contains non-alphanumeric characters, not searching kernel cmdline for size.", tty);

        rowskey = strjoin("systemd.tty.rows.", tty);
        اگر (!rowskey)
                واپس -ENOMEM;

        colskey = strjoin("systemd.tty.columns.", tty);
        اگر (!colskey)
                واپس -ENOMEM;

        r = proc_cmdline_get_کلید_many(/* flags= */ 0,
                                      rowskey, &rowsvalue,
                                      colskey, &colsvalue);
        اگر (r < 0)
                واپس log_debug_errno(r, "Failed to read TTY size of %s from kernel cmdline: %m", tty);

        اگر (rowsvalue) {
                r = محفوظ_atou(rowsvalue, &صفیں);
                اگر (r < 0)
                        واپس log_debug_errno(r, "Failed to parse %s=%s: %m", rowskey, rowsvalue);
        }

        اگر (colsvalue) {
                r = محفوظ_atou(colsvalue, &ستون_تعداد);
                اگر (r < 0)
                        واپس log_debug_errno(r, "Failed to parse %s=%s: %m", colskey, colsvalue);
        }

        اگر (ret_صفیں)
                *ret_صفیں = صفیں;
        اگر (ret_ستون_تعداد)
                *ret_ستون_تعداد = ستون_تعداد;

        واپس صفیں != UINT_MAX || ستون_تعداد != UINT_MAX;
}

/* intended to be used as a SIGWINCH sighandler */
خالی columns_سطریں_cache_reset(صحیح signum) {
        cached_columns = 0;
        cached_سطریں = 0;
}

خالی reset_terminal_feature_caches(خالی) {
        cached_columns = 0;
        cached_سطریں = 0;

        cached_on_tty = -1;
        cached_on_dev_null = -1;

        reset_ansi_feature_caches();
}

bool on_tty(خالی) {

        /* We check both stdout and stderr, so that situations where pipes on the shell are used are reliably
         * recognized, regardless if only the output or the errors are piped to some place. Since on_tty() is generally
         * used to default to a safer, non-interactive, non-color mode of operation it's probably good to be defensive
         * here, and check for both. Note that we don't check for STDIN_FILENO, because it should fine to use fancy
         * terminal functionality when outputting stuff, even if the input is piped to us. */

        اگر (cached_on_tty < 0)
                cached_on_tty =
                        isatty_محفوظ(STDOUT_FILENO) &&
                        isatty_محفوظ(STDERR_FILENO);

        واپس cached_on_tty;
}

صحیح getttyname_malloc(صحیح fd, حرف **ret) {
        حرف راستہ[PATH_MAX]; /* PATH_MAX is counted *with* the trailing NUL byte */
        صحیح r;

        أكد(fd >= 0);
        أكد(ret);

        r = ttyname_r(fd, راستہ, حجم راستہ); /* positive error */
        أكد(r >= 0);
        اگر (r == ERANGE)
                واپس -ENAMETOOLONG;
        اگر (r > 0)
                واپس -r;

        واپس strdup_مقصد(ret, skip_dev_سابقہ(راستہ));
}

صحیح getttyname_harder(صحیح fd, حرف **ret) {
        _cleanup_free_ حرف *s = تہی;
        صحیح r;

        أكد(ret);

        r = getttyname_malloc(fd, &s);
        اگر (r < 0)
                واپس r;

        اگر (streq(s, "tty"))
                واپس get_ctty(0, تہی, ret);

        *ret = TAKE_PTR(s);
        واپس 0;
}

صحیح get_ctty_devnr(pid_t pid, dev_t *ret) {
        _cleanup_free_ حرف *سطر = تہی;
        غیرعلامتی طویل ttynr;
        ثابت حرف *p;
        صحیح r;

        أكد(pid >= 0);

        p = procfs_file_alloca(pid, "stat");
        r = read_one_سطر_file(p, &سطر);
        اگر (r < 0)
                واپس r;

        p = strrchr(سطر, ')');
        اگر (!p)
                واپس -EIO;

        p++;

        اگر (لڑی_سے_پڑھو(p, " "
                   "%*c "  /* state */
                   "%*d "  /* ppid */
                   "%*d "  /* pgrp */
                   "%*d "  /* session */
                   "%lu ", /* ttynr */
                   &ttynr) != 1)
                واپس -EIO;

        اگر (devnum_is_zero(ttynr))
                واپس -ENXIO;

        اگر (ret)
                *ret = (dev_t) ttynr;

        واپس 0;
}

صحیح get_ctty(pid_t pid, dev_t *ret_devnr, حرف **ret) {
        حرف pty[STRLEN("/dev/pts/") + DECIMAL_STR_MAX(dev_t) + 1];
        _cleanup_free_ حرف *buf = تہی;
        ثابت حرف *fn = تہی, *w;
        dev_t devnr;
        صحیح r;

        r = get_ctty_devnr(pid, &devnr);
        اگر (r < 0)
                واپس r;

        r = device_راستہ_make_canonical(S_IFCHR, devnr, &buf);
        اگر (r < 0) {
                ساخت stat st;

                اگر (r != -ENOENT) /* No symlink for this in /dev/char/? */
                        واپس r;

                /* Maybe this is PTY? PTY devices are not listed in /dev/char/, as they don't follow the
                 * Linux device model and hence device_path_make_canonical() doesn't work for them. Let's
                 * assume this is a PTY for a moment, and check if the device node this would then map to in
                 * /dev/pts/ matches the one we are looking for. This way we don't have to hardcode the major
                 * number (which is 136 btw), but we still rely on the fact that PTY numbers map directly to
                 * the minor number of the pty. */
                xsprintf(pty, "/dev/pts/%u", minor(devnr));

                اگر (stat(pty, &st) < 0) {
                        اگر (errno != ENOENT)
                                واپس -errno;

                } ورنہ اگر (S_ISCHR(st.st_mode) && devnr == st.st_rdev) /* Bingo! */
                        fn = pty;

                اگر (!fn) {
                        /* Doesn't exist, or not a PTY? Probably something similar to the PTYs which have no
                         * symlink in /dev/char/. Let's return something vaguely useful. */
                        r = device_راستہ_make_major_minor(S_IFCHR, devnr, &buf);
                        اگر (r < 0)
                                واپس r;

                        fn = buf;
                }
        } ورنہ
                fn = buf;

        w = راستہ_startswith(fn, "/dev/");
        اگر (!w)
                واپس -EINVAL;

        اگر (ret) {
                r = strdup_مقصد(ret, w);
                اگر (r < 0)
                        واپس r;
        }

        اگر (ret_devnr)
                *ret_devnr = devnr;

        واپس 0;
}

صحیح ptsname_malloc(صحیح fd, حرف **ret) {
        أكد(fd >= 0);
        أكد(ret);

        برائے (size_t l = 50;;) {
                _cleanup_free_ حرف *c = تہی;

                c = new(حرف, l);
                اگر (!c)
                        واپس -ENOMEM;

                اگر (ptsname_r(fd, c, l) >= 0) {
                        *ret = TAKE_PTR(c);
                        واپس 0;
                }
                اگر (errno != ERANGE)
                        واپس -errno;

                اگر (!MUL_تفویض_محفوظ(&l, 2))
                        واپس -ENOMEM;
        }
}

صحیح openpt_allocate(صحیح flags, حرف **ret_peer_راستہ) {
        _cleanup_close_ صحیح fd = -EBADF;
        صحیح r;

        fd = posix_openpt(flags|O_NOCTTY|O_CLOEXEC);
        اگر (fd < 0)
                واپس -errno;

        _cleanup_free_ حرف *p = تہی;
        اگر (ret_peer_راستہ) {
                r = ptsname_malloc(fd, &p);
                اگر (r < 0)
                        واپس r;

                اگر (!راستہ_startswith(p, "/dev/pts/"))
                        واپس -EINVAL;
        }

        اگر (unlockpt(fd) < 0)
                واپس -errno;

        اگر (ret_peer_راستہ)
                *ret_peer_راستہ = TAKE_PTR(p);

        واپس TAKE_FD(fd);
}

ساکن صحیح ptsname_namespace(صحیح pty, حرف **ret) {
        صحیح no = -1;

        أكد(pty >= 0);
        أكد(ret);

        /* Like ptsname(), but doesn't assume that the path is
         * accessible in the local namespace. */

        اگر (ioctl(pty, TIOCGPTN, &no) < 0)
                واپس -errno;

        اگر (no < 0)
                واپس -EIO;

        اگر (asprintf(ret, "/dev/pts/%i", no) < 0)
                واپس -ENOMEM;

        واپس 0;
}

صحیح openpt_allocate_اندر_namespace(
                ثابت PidRef *pidref,
                صحیح flags,
                حرف **ret_peer_راستہ) {

        _cleanup_close_ صحیح pidnsfd = -EBADF, mntnsfd = -EBADF, usernsfd = -EBADF, rootfd = -EBADF, fd = -EBADF;
        _cleanup_close_pair_ صحیح pair[2] = EBADF_PAIR;
        صحیح r;

        أكد(pidref);

        r = pidref_namespace_open(pidref, &pidnsfd, &mntnsfd, /* ret_netns_fd= */ تہی, &usernsfd, &rootfd);
        اگر (r < 0)
                واپس log_debug_errno(r, "Failed to open namespaces of PID "PID_FMT": %m", pidref->pid);

        اگر (socketpair(AF_UNIX, SOCK_DGRAM|SOCK_CLOEXEC, 0, pair) < 0)
                واپس -errno;

        r = namespace_fork(
                        "(sd-openptns)",
                        "(sd-openpt)",
                        FORK_RESET_اشارات|FORK_DEATHSIG_SIGKILL|FORK_WAIT,
                        pidnsfd,
                        mntnsfd,
                        /* netns_fd= */ -EBADF,
                        usernsfd,
                        rootfd,
                        /* ret= */ تہی);
        اگر (r < 0)
                واپس r;
        اگر (r == 0) {
                pair[0] = محفوظ_close(pair[0]);

                fd = openpt_allocate(flags, /* ret_peer_path= */ تہی);
                اگر (fd < 0)
                        _exit(EXIT_FAILURE);

                اگر (send_one_fd(pair[1], fd, 0) < 0)
                        _exit(EXIT_FAILURE);

                _exit(EXIT_SUCCESS);
        }

        pair[1] = محفوظ_close(pair[1]);

        fd = receive_one_fd(pair[0], 0);
        اگر (fd < 0)
                واپس fd;

        اگر (ret_peer_راستہ) {
                r = ptsname_namespace(fd, ret_peer_راستہ);
                اگر (r < 0)
                        واپس r;
        }

        واپس TAKE_FD(fd);
}

ساکن bool on_dev_null(خالی) {
        ساخت stat dst, ost, est;

        اگر (cached_on_dev_null >= 0)
                واپس cached_on_dev_null;

        اگر (stat("/dev/null", &dst) < 0 || fstat(STDOUT_FILENO, &ost) < 0 || fstat(STDERR_FILENO, &est) < 0)
                cached_on_dev_null = false;
        ورنہ
                cached_on_dev_null = stat_inode_same(&dst, &ost) && stat_inode_same(&dst, &est);

        واپس cached_on_dev_null;
}

bool رکن_env_valid(ثابت حرف *رکن) {
        /* Checks if the specified $TERM value is suitable for propagation, i.e. is not empty, not set to
         * "unknown" (as is common in CI), and only contains characters valid in terminal type names.
         * Valid $TERM values are things like "xterm-256color", "linux", "screen.xterm-256color", i.e.
         * alphanumeric characters, hyphens, underscores, dots, and plus signs. */
        واپس !isempty(رکن) &&
                !streq(رکن, "unknown") &&
                اندر_charset(رکن, ALPHANUMERICAL "-_+.");
}

bool getenv_terminal_is_dumb(خالی) {
        ثابت حرف *e;

        e = getenv("TERM");
        اگر (!e)
                واپس true;

        واپس streq(e, "dumb");
}

bool terminal_is_dumb(خالی) {
        اگر (!on_tty() && !on_dev_null())
                واپس true;

        واپس getenv_terminal_is_dumb();
}

bool dev_console_colors_enabled(خالی) {
        _cleanup_free_ حرف *s = تہی;
        Color_منوال m;

        /* Returns true if we assume that color is supported on /dev/console.
         *
         * For that we first check if we explicitly got told to use colors or not, by checking $SYSTEMD_COLORS. If that
         * isn't set we check whether PID 1 has $TERM set, and if not, whether TERM is set on the kernel command
         * line. If we find $TERM set we assume color if it's not set to "dumb", similarly to how regular
         * colors_enabled() operates. */

        m = parse_systemd_colors();
        اگر (m >= 0)
                واپس m;

        اگر (getenv("NO_COLOR"))
                واپس false;

        اگر (getenv_for_pid(1, "TERM", &s) <= 0)
                (خالی) proc_cmdline_get_کلید("TERM", 0, &s);

        واپس !streq_ptr(s, "dumb");
}

صحیح vt_restore(صحیح fd) {

        ساکن ثابت ساخت vt_mode منوال = {
                .منوال = VT_AUTO,
        };

        صحیح r, ret = 0;

        أكد(fd >= 0);

        اگر (!isatty_محفوظ(fd))
                واپس log_debug_errno(SYNTHETIC_ERRNO(ENOTTY), "Asked to restore the VT for an fd that does not refer to a terminal.");

        اگر (ioctl(fd, KDSETMODE, KD_TEXT) < 0)
                RET_GATHER(ret, log_debug_errno(errno, "Failed to set VT to text mode, ignoring: %m"));

        r = vt_reset_keyboard(fd);
        اگر (r < 0)
                RET_GATHER(ret, log_debug_errno(r, "Failed to reset keyboard mode, ignoring: %m"));

        اگر (ioctl(fd, VT_SETMODE, &منوال) < 0)
                RET_GATHER(ret, log_debug_errno(errno, "Failed to set VT_AUTO mode, ignoring: %m"));

        r = fchmod_and_chown(fd, TTY_منوال, 0, GID_INVALID);
        اگر (r < 0)
                RET_GATHER(ret, log_debug_errno(r, "Failed to chmod()/chown() VT, ignoring: %m"));

        واپس ret;
}

صحیح vt_release(صحیح fd, bool restore) {
        أكد(fd >= 0);

        /* This function releases the VT by acknowledging the VT-switch signal
         * sent by the kernel and optionally reset the VT in text and auto
         * VT-switching modes. */

        اگر (!isatty_محفوظ(fd))
                واپس log_debug_errno(SYNTHETIC_ERRNO(ENOTTY), "Asked to release the VT for an fd that does not refer to a terminal.");

        اگر (ioctl(fd, VT_RELDISP, 1) < 0)
                واپس -errno;

        اگر (restore)
                واپس vt_restore(fd);

        واپس 0;
}

خالی get_log_colors(صحیح priority, ثابت حرف **on, ثابت حرف **off, ثابت حرف **highlight) {
        /* Note that this will initialize output variables only when there's something to output.
         * The caller must pre-initialize to "" or NULL as appropriate. */

        اگر (priority <= LOG_ERR) {
                اگر (on)
                        *on = ansi_highlight_red();
                اگر (off)
                        *off = ansi_معمول();
                اگر (highlight)
                        *highlight = ansi_highlight();

        } ورنہ اگر (priority <= LOG_WARNING) {
                اگر (on)
                        *on = ansi_highlight_yellow();
                اگر (off)
                        *off = ansi_معمول();
                اگر (highlight)
                        *highlight = ansi_highlight();

        } ورنہ اگر (priority <= LOG_NOTICE) {
                اگر (on)
                        *on = ansi_highlight();
                اگر (off)
                        *off = ansi_معمول();
                اگر (highlight)
                        *highlight = ansi_highlight_red();

        } ورنہ اگر (priority >= LOG_DEBUG) {
                اگر (on)
                        *on = ansi_grey();
                اگر (off)
                        *off = ansi_معمول();
                اگر (highlight)
                        *highlight = ansi_highlight_red();
        }
}

صحیح terminal_سیٹ_cursor_position(صحیح fd, غیرعلامتی صف, غیرعلامتی عمود) {
        أكد(fd >= 0);

        حرف cursor_position[STRLEN("\x1B[" ";" "H") + DECIMAL_STR_MAX(غیرعلامتی) * 2 + 1];
        xsprintf(cursor_position, "\x1B[%u;%uH", صف, عمود);

        واپس loop_write(fd, cursor_position, SIZE_MAX);
}

ساکن صحیح terminal_verify_same(صحیح داخلہ_fd, صحیح output_fd) {
        صحیح r;

        أكد(داخلہ_fd >= 0);
        أكد(output_fd >= 0);

        /* Validates that the specified fds reference the same TTY */

        اگر (داخلہ_fd != output_fd) {
                ساخت stat sti;
                اگر (fstat(داخلہ_fd, &sti) < 0)
                        واپس -errno;

                r = stat_verify_char(&sti); /* TTYs are character devices */
                اگر (r < 0)
                        واپس r;

                ساخت stat sto;
                اگر (fstat(output_fd, &sto) < 0)
                        واپس -errno;

                r = stat_verify_char(&sto);
                اگر (r < 0)
                        واپس r;

                اگر (sti.st_rdev != sto.st_rdev)
                        واپس -ENOLINK;
        }

        اگر (!isatty_محفوظ(داخلہ_fd)) /* The check above was just for char device, but now let's ensure it's actually a tty */
                واپس -ENOTTY;

        واپس 0;
}

قسمتعریف شمار CursorPositionState {
        CURSOR_متن,
        CURSOR_ESCAPE,
        CURSOR_صف,
        CURSOR_عمود,
} CursorPositionState;

قسمتعریف ساخت CursorPositionContext {
        CursorPositionState state;
        غیرعلامتی صف, عمود;
} CursorPositionContext;

ساکن صحیح جائزہ_cursor_position_response(
                CursorPositionContext *context,
                ثابت حرف *buf,
                size_t سائز,
                size_t *ret_processed) {

        أكد(context);
        أكد(buf);
        أكد(ret_processed);

        برائے (size_t i = 0; i < سائز; i++) {
                حرف c = buf[i];

                انتخاب (context->state) {

                حالت CURSOR_متن:
                        context->state = c == '\x1B' ? CURSOR_ESCAPE : CURSOR_متن;
                        توڑو;

                حالت CURSOR_ESCAPE:
                        context->state = c == '[' ? CURSOR_صف : CURSOR_متن;
                        توڑو;

                حالت CURSOR_صف:
                        اگر (c == ';')
                                context->state = context->صف > 0 ? CURSOR_عمود : CURSOR_متن;
                        ورنہ {
                                صحیح d = undecchar(c);

                                /* We read a decimal character, let's suffix it to the number we so far read,
                                 * but let's do an overflow check first. */
                                اگر (d < 0 || context->صف > (UINT_MAX-d)/10)
                                        context->state = CURSOR_متن;
                                ورنہ
                                        context->صف = context->صف * 10 + d;
                        }
                        توڑو;

                حالت CURSOR_عمود:
                        اگر (c == 'R') {
                                اگر (context->عمود > 0) {
                                        *ret_processed = i + 1;
                                        واپس 1; /* success! */
                                }

                                context->state = CURSOR_متن;
                        } ورنہ {
                                صحیح d = undecchar(c);

                                /* As above, add the decimal character to our column number */
                                اگر (d < 0 || context->عمود > (UINT_MAX-d)/10)
                                        context->state = CURSOR_متن;
                                ورنہ
                                        context->عمود = context->عمود * 10 + d;
                        }

                        توڑو;
                }

                /* Reset any positions we might have picked up */
                اگر (اندر_سیٹ(context->state, CURSOR_متن, CURSOR_ESCAPE))
                        context->صف = context->عمود = 0;
        }

        *ret_processed = سائز;
        واپس 0; /* all good, but not enough data yet */
}

صحیح terminal_get_cursor_position(
                صحیح داخلہ_fd,
                صحیح output_fd,
                غیرعلامتی *ret_صف,
                غیرعلامتی *ret_عمود) {

        _cleanup_close_ صحیح nonblock_داخلہ_fd = -EBADF;
        صحیح r;

        أكد(داخلہ_fd >= 0);
        أكد(output_fd >= 0);

        اگر (getenv_terminal_is_dumb())
                واپس -EOPNOTSUPP;

        r = terminal_verify_same(داخلہ_fd, output_fd);
        اگر (r < 0)
                واپس log_debug_errno(r, "Called with distinct input/output fds: %m");

        /* Failure to reset the terminal is ignored here and in similar cases below.
         * We already have our result; if cleanup fails it doesn't change the validity of the result. */
        ساخت termios old_termios = TERMIOS_NULL;
        CLEANUP_TERMIOS_RESET(داخلہ_fd, old_termios);

        اگر (tcgetattr(داخلہ_fd, &old_termios) < 0)
                واپس log_debug_errno(errno, "Failed to get terminal settings: %m");

        ساخت termios new_termios = old_termios;
        termios_disable_echo(&new_termios);

        اگر (tcsetattr(داخلہ_fd, TCSANOW, &new_termios) < 0)
                واپس log_debug_errno(errno, "Failed to set new terminal settings: %m");

        /* Request cursor position (DSR/CPR) */
        r = loop_write(output_fd, "\x1B[6n", SIZE_MAX);
        اگر (r < 0)
                واپس r;

        /* Open a 2nd input fd, in non-blocking mode, so that we won't ever hang in read() should someone
         * else process the POLLIN. */

        nonblock_داخلہ_fd = r = fd_reopen(داخلہ_fd, O_RDONLY|O_CLOEXEC|O_NONBLOCK|O_NOCTTY);
        اگر (r < 0)
                واپس r;

        usec_t انتہا = usec_جوڑو(now(CLOCK_MONOTONIC), CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        حرف buf[STRLEN("\x1B[1;1R")]; /* The shortest valid reply possible */
        size_t buf_بھرا = 0;
        CursorPositionContext context = {};

        برائے (bool پہلا = true;; پہلا = false) {
                اگر (buf_بھرا == 0) {
                        usec_t n = now(CLOCK_MONOTONIC);
                        اگر (n >= انتہا)
                                واپس -EOPNOTSUPP;

                        r = fd_wait_for_event(nonblock_داخلہ_fd, POLLIN, usec_sub_unsigned(انتہا, n));
                        اگر (r < 0)
                                واپس r;
                        اگر (r == 0)
                                واپس -EOPNOTSUPP;

                        /* On the first try, read multiple characters, i.e. the shortest valid
                         * reply. Afterwards read byte-wise, since we don't want to read too much, and
                         * unnecessarily drop too many characters from the input queue. */
                        ssize_t l = اقرأ(nonblock_داخلہ_fd, buf, پہلا ? حجم(buf) : 1);
                        اگر (l < 0) {
                                اگر (errno == EAGAIN)
                                        جاری;

                                واپس -errno;
                        }

                        أكد((size_t) l <= حجم(buf));
                        buf_بھرا = l;
                }

                size_t processed;
                r = جائزہ_cursor_position_response(&context, buf, buf_بھرا, &processed);
                اگر (r < 0)
                        واپس r;

                أكد(processed <= buf_بھرا);
                buf_بھرا -= processed;
                memmove(buf, buf + processed, buf_بھرا);

                اگر (r > 0) {
                        /* Superficial validity check */
                        اگر (context.صف >= 32766 || context.عمود >= 32766)
                                واپس -ENODATA;

                        اگر (ret_صف)
                                *ret_صف = context.صف;
                        اگر (ret_عمود)
                                *ret_عمود = context.عمود;

                        واپس 0;
                }
        }
}

صحیح terminal_reset_defensive(صحیح fd, TerminalResetFlags flags) {
        صحیح r = 0;

        أكد(fd >= 0);
        أكد(!FLAGS_سیٹ(flags, TERMINAL_RESET_AVOID_ANSI_SEQ|TERMINAL_RESET_FORCE_ANSI_SEQ));

        /* Resets the terminal comprehensively, i.e. via both ioctl()s and via ANSI sequences, but do so only
         * if $TERM is unset or set to "dumb" */

        اگر (!isatty_محفوظ(fd))
                واپس -ENOTTY;

        RET_GATHER(r, terminal_reset_ioctl(fd, FLAGS_سیٹ(flags, TERMINAL_RESET_SWITCH_مقصد_متن)));

        اگر (!FLAGS_سیٹ(flags, TERMINAL_RESET_AVOID_ANSI_SEQ) &&
            (FLAGS_سیٹ(flags, TERMINAL_RESET_FORCE_ANSI_SEQ) || !getenv_terminal_is_dumb()))
                RET_GATHER(r, terminal_reset_ansi_seq(fd));

        واپس r;
}

صحیح terminal_reset_defensive_locked(صحیح fd, TerminalResetFlags flags) {
        أكد(fd >= 0);

        _cleanup_close_ صحیح تالا_fd = تالا_dev_console();
        اگر (تالا_fd < 0)
                log_debug_errno(تالا_fd, "Failed to acquire lock for /dev/console, ignoring: %m");

        واپس terminal_reset_defensive(fd, flags);
}

خالی termios_disable_echo(ساخت termios *termios) {
        أكد(termios);

        termios->c_lflag &= ~(ICANON|ECHO);
        termios->c_cc[VMIN] = 1;
        termios->c_cc[VTIME] = 0;
}

ساکن bool termios_is_null(ثابت ساخت termios *t) {
        اگر (!t)
                واپس true;

        واپس t->c_iflag == UINT_MAX &&
               t->c_oflag == UINT_MAX &&
               t->c_cflag == UINT_MAX &&
               t->c_lflag == UINT_MAX;
}

خالی termios_reset(ثابت TermiosResetContext *c) {
        أكد(c);

        PROTECT_ERRNO;

        اگر (c->fd && *c->fd >= 0 && !termios_is_null(c->termios))
                (خالی) tcsetattr(*c->fd, TCSANOW, c->termios);
}

قسمتعریف شمار BackgroundColorState {
        BACKGROUND_متن,
        BACKGROUND_ESCAPE,
        BACKGROUND_BRACKET,
        BACKGROUND_پہلا_ONE,
        BACKGROUND_دوسرا_ONE,
        BACKGROUND_SEMICOLON,
        BACKGROUND_R,
        BACKGROUND_G,
        BACKGROUND_B,
        BACKGROUND_RED,
        BACKGROUND_GREEN,
        BACKGROUND_BLUE,
        BACKGROUND_STRING_TERMINATOR,
} BackgroundColorState;

قسمتعریف ساخت BackgroundColorContext {
        BackgroundColorState state;
        uint32_t red, green, blue;
        غیرعلامتی red_بٹ, green_بٹ, blue_بٹ;
} BackgroundColorContext;

ساکن صحیح جائزہ_background_color_response(
                BackgroundColorContext *context,
                ثابت حرف *buf,
                size_t سائز,
                size_t *ret_processed) {

        أكد(context);
        أكد(buf);
        أكد(ret_processed);

        برائے (size_t i = 0; i < سائز; i++) {
                حرف c = buf[i];

                انتخاب (context->state) {

                حالت BACKGROUND_متن:
                        context->state = c == '\x1B' ? BACKGROUND_ESCAPE : BACKGROUND_متن;
                        توڑو;

                حالت BACKGROUND_ESCAPE:
                        context->state = c == ']' ? BACKGROUND_BRACKET : BACKGROUND_متن;
                        توڑو;

                حالت BACKGROUND_BRACKET:
                        context->state = c == '1' ? BACKGROUND_پہلا_ONE : BACKGROUND_متن;
                        توڑو;

                حالت BACKGROUND_پہلا_ONE:
                        context->state = c == '1' ? BACKGROUND_دوسرا_ONE : BACKGROUND_متن;
                        توڑو;

                حالت BACKGROUND_دوسرا_ONE:
                        context->state = c == ';' ? BACKGROUND_SEMICOLON : BACKGROUND_متن;
                        توڑو;

                حالت BACKGROUND_SEMICOLON:
                        context->state = c == 'r' ? BACKGROUND_R : BACKGROUND_متن;
                        توڑو;

                حالت BACKGROUND_R:
                        context->state = c == 'g' ? BACKGROUND_G : BACKGROUND_متن;
                        توڑو;

                حالت BACKGROUND_G:
                        context->state = c == 'b' ? BACKGROUND_B : BACKGROUND_متن;
                        توڑو;

                حالت BACKGROUND_B:
                        context->state = c == ':' ? BACKGROUND_RED : BACKGROUND_متن;
                        توڑو;

                حالت BACKGROUND_RED:
                        اگر (c == '/')
                                context->state = context->red_بٹ > 0 ? BACKGROUND_GREEN : BACKGROUND_متن;
                        ورنہ {
                                صحیح d = unhexchar(c);
                                اگر (d < 0 || context->red_بٹ >= حجم(context->red)*8)
                                        context->state = BACKGROUND_متن;
                                ورنہ {
                                        context->red = (context->red << 4) | d;
                                        context->red_بٹ += 4;
                                }
                        }
                        توڑو;

                حالت BACKGROUND_GREEN:
                        اگر (c == '/')
                                context->state = context->green_بٹ > 0 ? BACKGROUND_BLUE : BACKGROUND_متن;
                        ورنہ {
                                صحیح d = unhexchar(c);
                                اگر (d < 0 || context->green_بٹ >= حجم(context->green)*8)
                                        context->state = BACKGROUND_متن;
                                ورنہ {
                                        context->green = (context->green << 4) | d;
                                        context->green_بٹ += 4;
                                }
                        }
                        توڑو;

                حالت BACKGROUND_BLUE:
                        اگر (c == '\x07') {
                                اگر (context->blue_بٹ > 0) {
                                        *ret_processed = i + 1;
                                        واپس 1; /* success! */
                                }

                                context->state = BACKGROUND_متن;
                        } ورنہ اگر (c == '\x1b')
                                context->state = context->blue_بٹ > 0 ? BACKGROUND_STRING_TERMINATOR : BACKGROUND_متن;
                        ورنہ {
                                صحیح d = unhexchar(c);
                                اگر (d < 0 || context->blue_بٹ >= حجم(context->blue)*8)
                                        context->state = BACKGROUND_متن;
                                ورنہ {
                                        context->blue = (context->blue << 4) | d;
                                        context->blue_بٹ += 4;
                                }
                        }
                        توڑو;

                حالت BACKGROUND_STRING_TERMINATOR:
                        اگر (c == '\\') {
                                *ret_processed = i + 1;
                                واپس 1; /* success! */
                        }

                        context->state = c == ']' ? BACKGROUND_ESCAPE : BACKGROUND_متن;
                        توڑو;

                }

                /* Reset any colors we might have picked up */
                اگر (اندر_سیٹ(context->state, BACKGROUND_متن, BACKGROUND_ESCAPE)) {
                        /* reset color */
                        context->red = context->green = context->blue = 0;
                        context->red_بٹ = context->green_بٹ = context->blue_بٹ = 0;
                }
        }

        *ret_processed = سائز;
        واپس 0; /* all good, but not enough data yet */
}

صحیح get_default_background_color(دوہرا *ret_red, دوہرا *ret_green, دوہرا *ret_blue) {
        صحیح r;

        أكد(ret_red);
        أكد(ret_green);
        أكد(ret_blue);

        اگر (!colors_enabled())
                واپس -EOPNOTSUPP;

        r = terminal_verify_same(STDIN_FILENO, STDOUT_FILENO);
        اگر (r < 0)
                واپس r;

        اگر (streq_ptr(getenv("TERM"), "linux")) {
                /* Linux console is black */
                *ret_red = *ret_green = *ret_blue = 0.0;
                واپس 0;
        }

        /* Open a 2nd input fd, in non-blocking mode, so that we won't ever hang in read()
         * should someone else process the POLLIN. Do all subsequent operations on the new fd. */
        _cleanup_close_ صحیح nonblock_داخلہ_fd = r = fd_reopen(STDIN_FILENO, O_RDONLY|O_CLOEXEC|O_NONBLOCK|O_NOCTTY);
        اگر (r < 0)
                واپس r;

        ساخت termios old_termios = TERMIOS_NULL;
        CLEANUP_TERMIOS_RESET(nonblock_داخلہ_fd, old_termios);

        اگر (tcgetattr(nonblock_داخلہ_fd, &old_termios) < 0)
                واپس -errno;

        ساخت termios new_termios = old_termios;
        termios_disable_echo(&new_termios);

        اگر (tcsetattr(nonblock_داخلہ_fd, TCSANOW, &new_termios) < 0)
                واپس -errno;

        r = loop_write(STDOUT_FILENO, ANSI_OSC "11;?" ANSI_ST, SIZE_MAX);
        اگر (r < 0)
                واپس r;

        usec_t انتہا = usec_جوڑو(now(CLOCK_MONOTONIC), CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        حرف buf[STRLEN(ANSI_OSC "11;rgb:0/0/0" ANSI_ST)]; /* shortest possible reply */
        size_t buf_بھرا = 0;
        BackgroundColorContext context = {};

        برائے (bool پہلا = true;; پہلا = false) {
                اگر (buf_بھرا == 0) {
                        usec_t n = now(CLOCK_MONOTONIC);
                        اگر (n >= انتہا)
                                واپس -EOPNOTSUPP;

                        r = fd_wait_for_event(nonblock_داخلہ_fd, POLLIN, usec_sub_unsigned(انتہا, n));
                        اگر (r < 0)
                                واپس r;
                        اگر (r == 0)
                                واپس -EOPNOTSUPP;

                        /* On the first try, read multiple characters, i.e. the shortest valid
                         * reply. Afterwards read byte-wise, since we don't want to read too much, and
                         * unnecessarily drop too many characters from the input queue. */
                        ssize_t l = اقرأ(nonblock_داخلہ_fd, buf, پہلا ? حجم(buf) : 1);
                        اگر (l < 0) {
                                اگر (errno == EAGAIN)
                                        جاری;
                                واپس -errno;
                        }

                        أكد((size_t) l <= حجم(buf));
                        buf_بھرا = l;
                }

                size_t processed;
                r = جائزہ_background_color_response(&context, buf, buf_بھرا, &processed);
                اگر (r < 0)
                        واپس r;

                أكد(processed <= buf_بھرا);
                buf_بھرا -= processed;
                memmove(buf, buf + processed, buf_بھرا);

                اگر (r > 0) {
                        أكد(context.red_بٹ > 0);
                        *ret_red = (دوہرا) context.red / ((UINT64_C(1) << context.red_بٹ) - 1);
                        أكد(context.green_بٹ > 0);
                        *ret_green = (دوہرا) context.green / ((UINT64_C(1) << context.green_بٹ) - 1);
                        أكد(context.blue_بٹ > 0);
                        *ret_blue = (دوہرا) context.blue / ((UINT64_C(1) << context.blue_بٹ) - 1);
                        واپس 0;
                }
        }
}

/* Determine terminal dimensions by means of ANSI sequences: save the cursor via DECSC, position it far
 * to the bottom right (clamped to actual terminal dimensions), read back via DSR where we ended up, and
 * restore cursor via DECRC. Only needs a single DSR round-trip, and always restores the cursor regardless
 * of whether the response is received.
 *
 * Caller must have already opened a non-blocking input fd and configured termios (echo/icanon off). */
ساکن صحیح terminal_query_سائز_by_dsr(
                صحیح nonblock_داخلہ_fd,
                صحیح output_fd,
                غیرعلامتی *ret_صفیں,
                غیرعلامتی *ret_columns) {

        صحیح r;

        أكد(nonblock_داخلہ_fd >= 0);
        أكد(output_fd >= 0);

        /* Use DECSC/DECRC to save/restore cursor instead of querying position via DSR. This way the cursor
         * is always restored — even on timeout — and we only need one DSR response instead of two. */
        r = loop_write_بھرا(output_fd,
                            "\x1B" "7"              /* DECSC: save cursor position */
                            "\x1B[32766;32766H"     /* CUP: position cursor far to the right and to the bottom, staying within 16bit signed range */
                            "\x1B[6n"               /* DSR: request cursor position (CPR) */
                            "\x1B" "8",             /* DECRC: restore cursor position */
                            SIZE_MAX,
                            CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        اگر (r < 0)
                واپس r;

        usec_t انتہا = usec_جوڑو(now(CLOCK_MONOTONIC), CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        حرف buf[STRLEN("\x1B[1;1R")]; /* The shortest valid reply possible */
        size_t buf_بھرا = 0;
        CursorPositionContext context = {};

        برائے (bool پہلا = true;; پہلا = false) {
                اگر (buf_بھرا == 0) {
                        usec_t n = now(CLOCK_MONOTONIC);
                        اگر (n >= انتہا)
                                واپس -EOPNOTSUPP;

                        r = fd_wait_for_event(nonblock_داخلہ_fd, POLLIN, usec_sub_unsigned(انتہا, n));
                        اگر (r < 0)
                                واپس r;
                        اگر (r == 0)
                                واپس -EOPNOTSUPP;

                        /* On the first try, read multiple characters, i.e. the shortest valid
                         * reply. Afterwards read byte-wise, since we don't want to read too much, and
                         * unnecessarily drop too many characters from the input queue. */
                        ssize_t l = اقرأ(nonblock_داخلہ_fd, buf, پہلا ? حجم(buf) : 1);
                        اگر (l < 0) {
                                اگر (errno == EAGAIN)
                                        جاری;

                                واپس -errno;
                        }

                        أكد((size_t) l <= حجم(buf));
                        buf_بھرا = l;
                }

                size_t processed;
                r = جائزہ_cursor_position_response(&context, buf, buf_بھرا, &processed);
                اگر (r < 0)
                        واپس r;

                أكد(processed <= buf_بھرا);
                buf_بھرا -= processed;
                memmove(buf, buf + processed, buf_بھرا);

                اگر (r > 0) {
                        /* Superficial validity checks (no particular reason to check for < 4, it's
                         * just a way to look for unreasonably small values) */
                        اگر (context.صف < 4 || context.عمود < 4 || context.صف >= 32766 || context.عمود >= 32766)
                                واپس -ENODATA;

                        اگر (ret_صفیں)
                                *ret_صفیں = context.صف;
                        اگر (ret_columns)
                                *ret_columns = context.عمود;

                        واپس 0;
                }
        }
}

/* Common setup for ANSI terminal queries: validate the fds, open a non-blocking input fd, and configure
 * termios with echo and canonical mode disabled. Caller must restore termios and close the fd when done. */
ساکن صحیح terminal_prepare_query(
                صحیح داخلہ_fd,
                صحیح output_fd,
                صحیح *ret_nonblock_fd,
                ساخت termios *ret_saved_termios) {

        صحیح r;

        أكد(داخلہ_fd >= 0);
        أكد(output_fd >= 0);
        أكد(ret_nonblock_fd);
        أكد(ret_saved_termios);

        /* Use getenv_terminal_is_dumb() instead of terminal_is_dumb() here since we operate on an
         * explicitly passed fd, not on stdio. terminal_is_dumb() additionally checks on_tty() which
         * tests whether *stderr* is a tty — that's irrelevant when we're querying a directly opened
         * terminal such as /dev/console. */
        اگر (getenv_terminal_is_dumb())
                واپس -EOPNOTSUPP;

        r = terminal_verify_same(داخلہ_fd, output_fd);
        اگر (r < 0)
                واپس log_debug_errno(r, "Called with distinct input/output fds: %m");

        /* Open a 2nd input fd, in non-blocking mode, so that we won't ever hang in read()
         * should someone else process the POLLIN. Do all subsequent operations on the new fd. */
        _cleanup_close_ صحیح nonblock_داخلہ_fd = r = fd_reopen(داخلہ_fd, O_RDONLY|O_CLOEXEC|O_NONBLOCK|O_NOCTTY);
        اگر (r < 0)
                واپس r;

        اگر (tcgetattr(nonblock_داخلہ_fd, ret_saved_termios) < 0)
                واپس log_debug_errno(errno, "Failed to get terminal settings: %m");

        ساخت termios new_termios = *ret_saved_termios;
        termios_disable_echo(&new_termios);

        اگر (tcsetattr(nonblock_داخلہ_fd, TCSANOW, &new_termios) < 0)
                واپس log_debug_errno(errno, "Failed to set new terminal settings: %m");

        *ret_nonblock_fd = TAKE_FD(nonblock_داخلہ_fd);
        واپس 0;
}

/*
 * See https://terminalguide.namepad.de/seq/csi_st-18/,
 * https://invisible-island.net/xterm/ctlseqs/ctlseqs.html#h3-Functions-using-CSI-_-ordered-by-the-final-character_s_.
 */
#تعریف_کرو CSI18_Q  "\x1B[18t"               /* Report the size of the text area in characters */
#تعریف_کرو CSI18_Rp "\x1B[8;"                /* Reply prefix */
#تعریف_کرو CSI18_R0 CSI18_Rp "1;1t"          /* Shortest reply */
#تعریف_کرو CSI18_R1 CSI18_Rp "32766;32766t"  /* Longest reply */

ساکن صحیح جائزہ_متن_area_سائز_response(
                ثابت حرف *buf,
                size_t سائز,
                غیرعلامتی *ret_صفیں,
                غیرعلامتی *ret_columns) {

        أكد(buf);
        أكد(ret_صفیں);
        أكد(ret_columns);

        /* Check if we have enough space for the shortest possible answer. */
        اگر (سائز < STRLEN(CSI18_R0))
                واپس -EAGAIN;

        /* Check if the terminating sequence is present */
        اگر (buf[سائز - 1] != 't')
                واپس -EAGAIN;

        غیرعلامتی مختصر صفیں, columns;
        اگر (لڑی_سے_پڑھو(buf, CSI18_Rp "%hu;%hut", &صفیں, &columns) != 2)
                واپس -EINVAL;

        *ret_صفیں = صفیں;
        *ret_columns = columns;
        واپس 0;
}

/* Determine terminal dimensions by means of an ANSI CSI 18 sequence.
 *
 * Caller must have already opened a non-blocking input fd and configured termios (echo/icanon off). */
ساکن صحیح terminal_query_سائز_by_csi18(
                صحیح nonblock_داخلہ_fd,
                صحیح output_fd,
                غیرعلامتی *ret_صفیں,
                غیرعلامتی *ret_columns) {

        صحیح r;

        أكد(nonblock_داخلہ_fd >= 0);
        أكد(output_fd >= 0);

        r = loop_write_بھرا(output_fd, CSI18_Q, SIZE_MAX, CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        اگر (r < 0)
                واپس r;

        usec_t انتہا = usec_جوڑو(now(CLOCK_MONOTONIC), CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        حرف buf[STRLEN(CSI18_R1)];
        size_t بائٹیں = 0;

        برائے (;;) {
                usec_t n = now(CLOCK_MONOTONIC);
                اگر (n >= انتہا)
                        واپس -EOPNOTSUPP;

                r = fd_wait_for_event(nonblock_داخلہ_fd, POLLIN, usec_sub_unsigned(انتہا, n));
                اگر (r < 0)
                        واپس r;
                اگر (r == 0)
                        واپس -EOPNOTSUPP;

                /* On the first read, read multiple characters, i.e. the shortest valid reply. Afterwards
                 * read byte by byte, since we don't want to read too much and drop characters from the input
                 * queue. */
                ssize_t l = اقرأ(nonblock_داخلہ_fd, buf + بائٹیں, بائٹیں == 0 ? STRLEN(CSI18_R0) : 1);
                اگر (l < 0) {
                        اگر (errno == EAGAIN)
                                جاری;
                        واپس -errno;
                }

                أكد((size_t) l <= حجم(buf) - بائٹیں);
                بائٹیں += l;

                r = جائزہ_متن_area_سائز_response(buf, بائٹیں, ret_صفیں, ret_columns);
                اگر (r != -EAGAIN)
                        واپس r;

                اگر (بائٹیں == حجم(buf))
                        واپس -EOPNOTSUPP; /* The response has the right prefix, but we didn't find a valid
                                             * answer with a terminator in the allotted space. Something is
                                             * wrong, possibly some unrelated bytes got injected into the
                                             * answer. */
        }
}

صحیح terminal_get_سائز(
                صحیح داخلہ_fd,
                صحیح output_fd,
                غیرعلامتی *ret_صفیں,
                غیرعلامتی *ret_columns,
                bool try_dsr,
                bool try_csi18) {

        _cleanup_close_ صحیح nonblock_داخلہ_fd = -EBADF;
        ساخت termios old_termios = TERMIOS_NULL;
        CLEANUP_TERMIOS_RESET(nonblock_داخلہ_fd, old_termios);
        _cleanup_(nonblock_resetp) صحیح nonblock_reset = -EBADF;
        صحیح r;

        أكد(try_dsr || try_csi18);

        r = terminal_prepare_query(داخلہ_fd, output_fd, &nonblock_داخلہ_fd, &old_termios);
        اگر (r < 0)
                واپس r;

        /* Put the output fd in non-blocking mode with a write timeout, to avoid blocking indefinitely on
         * write if the terminal is not consuming data (e.g. serial console with flow control). */
        r = fd_nonblock(output_fd, true);
        اگر (r < 0)
                واپس log_debug_errno(r, "Failed to set terminal to non-blocking mode: %m");
        اگر (r > 0)
                nonblock_reset = output_fd;

        /* Flush any stale input that might confuse the response parsers. */
        (خالی) tcflush(nonblock_داخلہ_fd, TCIFLUSH);

        اگر (try_csi18) {
                r = terminal_query_سائز_by_csi18(nonblock_داخلہ_fd, output_fd, ret_صفیں, ret_columns);
                اگر (r >= 0)
                        واپس r;

                /* Query failed. Flush any outstanding input. */
                (خالی) tcflush(nonblock_داخلہ_fd, TCIFLUSH);

                اگر (!اندر_سیٹ(r, -EOPNOTSUPP, -EINVAL))
                        واپس r;
        }

        اگر (try_dsr) {
                r = terminal_query_سائز_by_dsr(nonblock_داخلہ_fd, output_fd, ret_صفیں, ret_columns);
                اگر (r >= 0)
                        واپس r;

                /* Query failed. Flush any outstanding input. */
                (خالی) tcflush(nonblock_داخلہ_fd, TCIFLUSH);
        }

        واپس r;
}

ساکن صحیح fd_is_vc(صحیح fd) {
        _cleanup_free_ حرف *name = تہی;
        صحیح r;

        أكد(fd >= 0);

        r = getttyname_harder(fd, &name);
        اگر (r < 0)
                واپس r;

        واپس tty_is_vc_resolve(name);
}

صحیح terminal_fix_سائز(صحیح داخلہ_fd, صحیح output_fd) {
        غیرعلامتی صفیں, columns;
        صحیح r;

        /* Don't ask a virtual console at all. There is no intermediary here that could be wrong about the
         * dimensions: the kernel renders the screen itself, so its winsize is authoritative. */
        r = fd_is_vc(داخلہ_fd);
        اگر (r < 0)
                log_debug_errno(r, "Failed to determine whether terminal is a virtual console, assuming it is not: %m");
        ورنہ اگر (r > 0) {
                log_debug("Not querying dimensions of a virtual console, kernel dimensions are authoritative.");
                واپس 0;
        }

        ساخت winsize ws = {};
        اگر (ioctl(output_fd, TIOCGWINSZ, &ws) < 0)
                واپس log_debug_errno(errno, "Failed to query terminal dimensions, ignoring: %m");

        r = terminal_get_سائز(داخلہ_fd, output_fd, &صفیں, &columns, /* try_dsr= */ true, /* try_csi18= */ true);
        اگر (r < 0)
                واپس log_debug_errno(r, "Failed to acquire terminal dimensions via ANSI sequences, not adjusting terminal dimensions: %m");

        اگر (ws.ws_row == صفیں && ws.ws_col == columns) {
                log_debug("Terminal dimensions reported via ANSI sequences match currently set terminal dimensions, not changing.");
                واپس 0;
        }

        ws.ws_col = columns;
        ws.ws_row = صفیں;

        اگر (ioctl(output_fd, TIOCSWINSZ, &ws) < 0)
                واپس log_debug_errno(errno, "Failed to update terminal dimensions, ignoring: %m");

        log_debug("Fixed terminal dimensions to %ux%u based on ANSI sequence information.", columns, صفیں);
        واپس 1;
}

#تعریف_کرو MAX_TERMINFO_لمبائی 64
/* python -c 'print("".join(hex(ord(i))[2:] for i in "name").upper())' */
#تعریف_کرو DCS_TERMINFO_Q ANSI_DCS "+q" "6E616D65" ANSI_ST
/* The answer is either 0+r… (invalid) or 1+r… (OK). */
#تعریف_کرو DCS_TERMINFO_R0 ANSI_DCS "0+r" ANSI_ST
#تعریف_کرو DCS_TERMINFO_R1 ANSI_DCS "1+r" "6E616D65" "=" /* This is followed by Pt ST. */
assert_cc(STRLEN(DCS_TERMINFO_R0) <= STRLEN(DCS_TERMINFO_R1 ANSI_ST));

ساکن صحیح جائزہ_terminfo_response(
                ثابت حرف *buf,
                size_t سائز,
                حرف **ret_name) {
        صحیح r;

        أكد(buf);
        أكد(ret_name);

        /* Check if we have enough space for the shortest possible answer. */
        اگر (سائز < STRLEN(DCS_TERMINFO_R0))
                واپس -EAGAIN;

        /* Check if the terminating sequence is present */
        اگر (memcmp(buf + سائز - STRLEN(ANSI_ST), ANSI_ST, STRLEN(ANSI_ST)) != 0)
                واپس -EAGAIN;

        اگر (سائز <= STRLEN(DCS_TERMINFO_R1 ANSI_ST))
                واپس -EINVAL;  /* The answer is invalid or empty */

        اگر (memcmp(buf, DCS_TERMINFO_R1, STRLEN(DCS_TERMINFO_R1)) != 0)
                واپس -EINVAL;  /* The answer is not valid */

        _cleanup_free_ خالی *dec = تہی;
        size_t dec_سائز;
        r = unhexmem_بھرا(buf + STRLEN(DCS_TERMINFO_R1), سائز - STRLEN(DCS_TERMINFO_R1 ANSI_ST),
                          /* secure= */ false,
                          &dec, &dec_سائز);
        اگر (r < 0)
                واپس r;

        أكد(((ثابت حرف *) dec)[dec_سائز] == '\0'); /* unhexmem appends NUL for our convenience */
        اگر (memchr(dec, '\0', dec_سائز) || string_has_cc(dec, تہی) || !filename_is_valid(dec))
                واپس -EUCLEAN;

        *ret_name = TAKE_PTR(dec);
        واپس 0;
}

صحیح terminal_get_terminfo_by_dcs(صحیح fd, حرف **ret_name) {
        صحیح r;

        أكد(fd >= 0);
        أكد(ret_name);

        /* Note: fd must be in non-blocking read-write mode! */

        ساخت termios old_termios = TERMIOS_NULL;
        CLEANUP_TERMIOS_RESET(fd, old_termios);

        اگر (tcgetattr(fd, &old_termios) < 0)
                واپس -errno;

        ساخت termios new_termios = old_termios;
        termios_disable_echo(&new_termios);

        اگر (tcsetattr(fd, TCSANOW, &new_termios) < 0)
                واپس -errno;

        r = loop_write(fd, DCS_TERMINFO_Q, SIZE_MAX);
        اگر (r < 0)
                واپس r;

        usec_t انتہا = usec_جوڑو(now(CLOCK_MONOTONIC), CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        حرف buf[STRLEN(DCS_TERMINFO_R1) + MAX_TERMINFO_لمبائی + STRLEN(ANSI_ST)];
        size_t بائٹیں = 0;

        برائے (;;) {
                usec_t n = now(CLOCK_MONOTONIC);
                اگر (n >= انتہا)
                        واپس -EOPNOTSUPP;

                r = fd_wait_for_event(fd, POLLIN, usec_sub_unsigned(انتہا, n));
                اگر (r < 0)
                        واپس r;
                اگر (r == 0)
                        واپس -EOPNOTSUPP;

                /* On the first read, read multiple characters, i.e. the shortest valid reply. Afterwards
                 * read byte by byte, since we don't want to read too much and drop characters from the input
                 * queue. */
                ssize_t l = اقرأ(fd, buf + بائٹیں, بائٹیں == 0 ? STRLEN(DCS_TERMINFO_R0) : 1);
                اگر (l < 0) {
                        اگر (errno == EAGAIN)
                                جاری;
                        واپس -errno;
                }

                أكد((size_t) l <= حجم(buf) - بائٹیں);
                بائٹیں += l;

                r = جائزہ_terminfo_response(buf, بائٹیں, ret_name);
                اگر (r != -EAGAIN)
                        واپس r;

                اگر (بائٹیں == حجم(buf))
                        واپس -EOPNOTSUPP; /* The response has the right prefix, but we didn't find a valid
                                             * answer with a terminator in the allotted space. Something is
                                             * wrong, possibly some unrelated bytes got injected into the
                                             * answer. */
        }
}

صحیح have_terminfo_file(ثابت حرف *name) {
        /* This is a heuristic check if we have the file, using the directory layout used on
         * current Linux systems. Checks for other layouts can be added later if appropriate. */
        صحیح r;

        أكد(filename_is_valid(name));

        _cleanup_free_ حرف *p = راستہ_join("/usr/share/terminfo", CHAR_مقصد_STR(name[0]), name);
        اگر (!p)
                واپس log_oom_debug();

        r = RET_NERRNO(access(p, F_OK));
        اگر (r == -ENOENT)
                واپس false;
        اگر (r < 0)
                واپس r;
        واپس true;
}

صحیح query_رکن_for_tty(ثابت حرف *tty, حرف **ret_رکن) {
        _cleanup_free_ حرف *dcs_رکن = تہی;
        صحیح r;

        أكد(tty);
        أكد(ret_رکن);

        اگر (tty_is_vc_resolve(tty))
                واپس strdup_مقصد(ret_رکن, "linux");

        /* Try to query the terminal implementation that we're on. This will not work in all
         * cases, which is fine, since this is intended to be used as a fallback. */

        _cleanup_close_ صحیح tty_fd = open_terminal(tty, O_RDWR|O_NOCTTY|O_CLOEXEC|O_NONBLOCK);
        اگر (tty_fd < 0)
                واپس log_debug_errno(tty_fd, "Failed to open %s to query terminfo: %m", tty);

        r = terminal_get_terminfo_by_dcs(tty_fd, &dcs_رکن);
        اگر (r < 0)
                واپس log_debug_errno(r, "Failed to query %s for terminfo: %m", tty);

        r = have_terminfo_file(dcs_رکن);
        اگر (r < 0)
                واپس log_debug_errno(r, "Failed to look for terminfo %s: %m", dcs_رکن);
        اگر (r == 0)
                واپس log_info_errno(SYNTHETIC_ERRNO(ENODATA),
                                      "Terminfo %s not found for %s.", dcs_رکن, tty);

        *ret_رکن = TAKE_PTR(dcs_رکن);
        واپس 0;
}

صحیح terminal_is_pty_fd(صحیح fd) {
        صحیح r;

        أكد(fd >= 0);

        /* Returns true if we are looking at a pty, i.e. if it's backed by the /dev/pts/ file system */

        اگر (!isatty_محفوظ(fd))
                واپس false;

        r = is_fs_type_at(fd, تہی, DEVPTS_SUPER_MAGIC);
        اگر (r != 0)
                واپس r;

        /* The ptmx device is weird, it exists twice, once inside and once outside devpts. To detect the
         * latter case, let's fire off an ioctl() that only works on ptmx devices. */

        صحیح v;
        اگر (ioctl(fd, TIOCGPKT, &v) < 0) {
                اگر (ERRNO_IS_NOT_SUPPORTED(errno))
                        واپس false;

                واپس -errno;
        }

        واپس true;
}

صحیح pty_open_peer(صحیح fd, صحیح منوال) {
        أكد(fd >= 0);

        /* Opens the peer PTY using the new race-free TIOCGPTPEER ioctl() (kernel 4.13).
         *
         * This is safe to be called on TTYs from other namespaces. */

        أكد((منوال & (O_CREAT|O_راستہ|O_DIRECTORY|O_TMPFILE)) == 0);

        /* This replicates the EIO retry logic of open_terminal() in a modified way. */
        برائے (غیرعلامتی c = 0;; c++) {
                صحیح peer_fd = ioctl(fd, TIOCGPTPEER, منوال);
                اگر (peer_fd >= 0)
                        واپس peer_fd;

                اگر (errno != EIO)
                        واپس -errno;

                /* Max 1s in total */
                اگر (c >= 20)
                        واپس -EIO;

                (خالی) usleep_محفوظ(50 * USEC_فی_MSEC);
        }
}
