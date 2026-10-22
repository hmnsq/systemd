/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <fcntl.h>
#include <linux/kd.h>
#include <linux/magic.h>
#include <linux/tiocl.h>
#include <linux/vt.h>
#include <poll.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/inotify.h>
#include <sys/ioctl.h>
#include <sys/sysmacros.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

#include "alloc-util.h"
#include "ansi-color.h"
#include "chase.h"
#include "devnum-util.h"
#include "errno-util.h"
#include "extract-word.h"
#include "fd-util.h"
#include "fileio.h"
#include "fs-util.h"
#include "hexdecoct.h"
#include "inotify-util.h"
#include "io-util.h"
#include "log.h"
#include "namespace-util.h"
#include "parse-util.h"
#include "path-util.h"
#include "pidref.h"
#include "proc-cmdline.h"
#include "process-util.h"
#include "signal-util.h"
#include "socket-util.h"
#include "stat-util.h"
#include "stdio-util.h"
#include "string-util.h"
#include "strv.h"
#include "terminal-util.h"
#include "time-util.h"
#include "utf8.h"

/* How much to wait when reading/writing ANSI sequences from/to the console */
#define CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC (333 * USEC_PER_MSEC)

static volatile unsigned cached_columns = 0;
static volatile unsigned cached_lines = 0;

static volatile 整数 cached_on_tty = -1;
static volatile 整数 cached_on_dev_null = -1;

bool isatty_safe(整数 fd) {
        assert(fd >= 0);

        如果 (isatty(fd))
                返回 true;

        /* Linux/glibc returns EIO for hung up TTY on isatty(). Which is wrong, the thing doesn't stop being
         * a TTY after all, just because it is temporarily hung up. Let's work around this here, until this
         * is fixed in glibc. See: https://sourceware.org/bugzilla/show_bug.cgi?id=32103 */
        如果 (errno == EIO)
                返回 true;

        /* Be resilient if we're working on stdio, since they're set up by parent process. */
        assert(errno != EBADF || IN_SET(fd, STDIN_FILENO, STDOUT_FILENO, STDERR_FILENO));

        返回 false;
}

整数 chvt(整数 vt) {
        _cleanup_close_ 整数 fd = -EBADF;

        /* Switch to the specified vt number. If the VT is specified <= 0 switch to the VT the kernel log messages go,
         * if that's configured. */

        fd = open_terminal("/dev/tty0", O_RDWR|O_NOCTTY|O_CLOEXEC|O_NONBLOCK);
        如果 (fd < 0)
                返回 fd;

        如果 (vt <= 0) {
                整数 tiocl[2] = {
                        TIOCL_GETKMSGREDIRECT,
                        0
                };

                如果 (ioctl(fd, TIOCLINUX, tiocl) < 0)
                        返回 -errno;

                vt = tiocl[0] <= 0 ? 1 : tiocl[0];
        }

        返回 RET_NERRNO(ioctl(fd, VT_ACTIVATE, vt));
}

整数 read_one_char(FILE *f, char *ret, usec_t t, bool echo, bool *need_nl) {
        _cleanup_free_ char *line = NULL;
        struct termios old_termios;
        整数 r, fd;

        assert(ret);

        如果 (!f)
                f = stdin;

        /* If this is a terminal, then switch canonical mode off, so that we can read a single
         * character. (Note that fmemopen() streams do not have an fd associated with them, let's handle that
         * nicely.) If 'echo' is false we'll also disable ECHO mode so that the pressed key is not made
         * visible to the user. */
        fd = fileno(f);
        如果 (fd >= 0 && tcgetattr(fd, &old_termios) >= 0) {
                struct termios new_termios = old_termios;

                new_termios.c_lflag &= ~(ICANON|(echo ? 0 : ECHO));
                new_termios.c_cc[VMIN] = 1;
                new_termios.c_cc[VTIME] = 0;

                如果 (tcsetattr(fd, TCSANOW, &new_termios) >= 0) {
                        char c;

                        如果 (t != USEC_INFINITY) {
                                如果 (fd_wait_for_event(fd, POLLIN, t) <= 0) {
                                        (void) tcsetattr(fd, TCSANOW, &old_termios);
                                        返回 -ETIMEDOUT;
                                }
                        }

                        r = safe_fgetc(f, &c);
                        (void) tcsetattr(fd, TCSANOW, &old_termios);
                        如果 (r < 0)
                                返回 r;
                        如果 (r == 0)
                                返回 -EIO;

                        如果 (need_nl)
                                *need_nl = c != '\n';

                        *ret = c;
                        返回 0;
                }
        }

        如果 (t != USEC_INFINITY && fd >= 0) {
                /* Let's wait the specified amount of time for input. When we have no fd we skip this, under
                 * the assumption that this is an fmemopen() stream or so where waiting doesn't make sense
                 * anyway, as the data is either already in the stream or cannot possible be placed there
                 * while we access the stream */

                如果 (fd_wait_for_event(fd, POLLIN, t) <= 0)
                        返回 -ETIMEDOUT;
        }

        /* If this is not a terminal, then read a full line instead */

        r = read_line(f, 16, &line); /* longer than necessary, to eat up UTF-8 chars/vt100 key sequences */
        如果 (r < 0)
                返回 r;
        如果 (r == 0)
                返回 -EIO;

        如果 (strlen(line) != 1)
                返回 -EBADMSG;

        如果 (need_nl)
                *need_nl = false;

        *ret = line[0];
        返回 0;
}

#define DEFAULT_ASK_REFRESH_USEC (2*USEC_PER_SEC)

整数 ask_char(char *ret, const char *replies, const char *fmt, ...) {
        整数 r;

        assert(ret);
        assert(replies);
        assert(fmt);

        为 (;;) {
                va_list ap;
                char c;
                bool need_nl = true;

                fputs(ansi_highlight(), stdout);

                putchar('\r');

                va_start(ap, fmt);
                vprintf(fmt, ap);
                va_end(ap);

                fputs(ansi_normal(), stdout);

                fflush(stdout);

                r = read_one_char(stdin, &c, DEFAULT_ASK_REFRESH_USEC, /* echo= */ true, &need_nl);
                如果 (r < 0) {

                        如果 (r == -ETIMEDOUT)
                                continue;

                        如果 (r == -EBADMSG) {
                                puts("Bad input, please try again.");
                                continue;
                        }

                        putchar('\n');
                        返回 r;
                }

                如果 (need_nl)
                        putchar('\n');

                如果 (strchr(replies, c)) {
                        *ret = c;
                        返回 0;
                }

                puts("Read unexpected character, please try again.");
        }
}

typedef enum CompletionResult{
        COMPLETION_ALREADY,       /* the input string is already complete */
        COMPLETION_FULL,          /* completed the input string to be complete now */
        COMPLETION_PARTIAL,       /* completed the input string so that is still incomplete */
        COMPLETION_NONE,          /* found no matching completion */
        _COMPLETION_RESULT_MAX,
        _COMPLETION_RESULT_INVALID = -EINVAL,
        _COMPLETION_RESULT_ERRNO_MAX = -ERRNO_MAX,
} CompletionResult;

static CompletionResult pick_completion(const char *string, char *const*completions, char **ret) {
        _cleanup_free_ char *found = NULL;
        bool partial = false;

        assert(ret);

        string = strempty(string);

        STRV_FOREACH(c, completions) {

                /* Ignore entries that are not actually completions */
                如果 (!startswith(*c, string))
                        continue;

                /* Store first completion that matches */
                如果 (!found) {
                        found = strdup(*c);
                        如果 (!found)
                                返回 -ENOMEM;

                        continue;
                }

                /* If there's another completion that works truncate the one we already found by common
                 * prefix */
                size_t n = str_common_prefix(found, *c);
                如果 (n == SIZE_MAX)
                        continue;

                found[n] = 0;
                partial = true;
        }

        *ret = TAKE_PTR(found);

        如果 (!*ret)
                返回 COMPLETION_NONE;
        如果 (partial)
                返回 COMPLETION_PARTIAL;

        返回 streq(string, *ret) ? COMPLETION_ALREADY : COMPLETION_FULL;
}

static void clear_by_backspace(size_t n) {
        /* Erase the specified number of character cells backwards on the terminal */
        为 (size_t i = 0; i < n; i++)
                fputs("\b \b", stdout);
}

整数 ask_string_full(
                char **ret,
                const char *prefill,
                GetCompletionsCallback get_completions,
                void *userdata,
                const char *text, ...) {

        va_list ap;
        整数 r;

        assert(ret);
        assert(text);

        _cleanup_free_ char *string = NULL;
        size_t n = 0;

        如果 (prefill) {
                /* Prefill query with explicit data if specified */

                string = strdup(prefill);
                如果 (!string)
                        返回 -ENOMEM;

                n = strlen(string);

        } 否则 如果 (get_completions) {
                /* Otherwise, figure out what string to preselect the query with */
                _cleanup_strv_free_ char **completions = NULL;
                r = get_completions("", GET_COMPLETIONS_PRESELECT, &completions, userdata);
                如果 (r < 0)
                        返回 r;

                CompletionResult cr = pick_completion(string, completions, &string);
                如果 (cr < 0)
                        返回 cr;

                n = strlen_ptr(string);
        }

        /* Output the prompt */
        fputs(ansi_highlight(), stdout);
        va_start(ap, text);
        vprintf(text, ap);
        va_end(ap);
        fputs(ansi_normal(), stdout);
        如果 (string)
                fputs(string, stdout);
        fflush(stdout);

        /* Do interactive logic only if stdin + stdout are connected to the same place. And yes, we could use
         * STDIN_FILENO and STDOUT_FILENO here, but let's be overly correct for once, after all libc allows
         * swapping out stdin/stdout. */
        整数 fd_input = fileno(stdin);
        整数 fd_output = fileno(stdout);
        struct termios old_termios = TERMIOS_NULL;
        CLEANUP_TERMIOS_RESET(fd_input, old_termios);

        如果 (fd_input < 0 || fd_output < 0 || same_fd(fd_input, fd_output) <= 0)
                goto fallback;

        /* Try to disable echo, which also tells us if this even is a terminal */
        如果 (tcgetattr(fd_input, &old_termios) < 0) {
                old_termios = TERMIOS_NULL;
                goto fallback;
        }

        struct termios new_termios = old_termios;
        termios_disable_echo(&new_termios);
        如果 (tcsetattr(fd_input, TCSANOW, &new_termios) < 0)
                返回 -errno;

        为 (;;) {
                整数 c = fgetc(stdin);

                /* On EOF or NUL, end the request, don't output anything anymore */
                如果 (IN_SET(c, EOF, 0))
                        break;

                /* On Return also end the request, but make this visible */
                如果 (IN_SET(c, '\n', '\r')) {
                        fputc('\n', stdout);
                        break;
                }

                如果 (c == '\t') {
                        /* Tab */

                        _cleanup_strv_free_ char **completions = NULL;
                        如果 (get_completions) {
                                r = get_completions(string, /* flags= */ 0, &completions, userdata);
                                如果 (r < 0)
                                        返回 r;
                        }

                        _cleanup_free_ char *new_string = NULL;
                        CompletionResult cr = pick_completion(string, completions, &new_string);
                        如果 (cr < 0)
                                返回 cr;
                        如果 (IN_SET(cr, COMPLETION_PARTIAL, COMPLETION_FULL)) {
                                /* Output the new suffix we learned */
                                fputs(ASSERT_PTR(startswith(new_string, strempty(string))), stdout);

                                /* And update the whole string */
                                free_and_replace(string, new_string);
                                n = strlen(string);
                        }
                        如果 (cr == COMPLETION_NONE)
                                fputc('\a', stdout); /* BEL */

                        如果 (IN_SET(cr, COMPLETION_PARTIAL, COMPLETION_ALREADY)) {
                                /* If this worked only partially, or if the user hit TAB even though we were
                                 * complete already, then show the remaining options (in the latter case just
                                 * the one). */
                                fputc('\n', stdout);

                                _cleanup_strv_free_ char **filtered = strv_filter_prefix(completions, string);
                                如果 (!filtered)
                                        返回 -ENOMEM;

                                r = show_menu(filtered,
                                              /* n_columns= */ SIZE_MAX,
                                              /* column_width= */ SIZE_MAX,
                                              /* ellipsize_percentage= */ 0,
                                              /* grey_prefix= */ string,
                                              /* with_numbers= */ false);
                                如果 (r < 0)
                                        返回 r;

                                /* Show the prompt again */
                                fputs(ansi_highlight(), stdout);
                                va_start(ap, text);
                                vprintf(text, ap);
                                va_end(ap);
                                fputs(ansi_normal(), stdout);
                                fputs(string, stdout);
                        }

                } 否则 如果 (IN_SET(c, '\b', 127)) {
                        /* Backspace */

                        如果 (n == 0)
                                fputc('\a', stdout); /* BEL */
                        否则 {
                                size_t m = utf8_last_length(string, n);

                                char *e = string + n - m;
                                clear_by_backspace(utf8_console_width(e));

                                *e = 0;
                                n -= m;
                        }

                } 否则 如果 (c == 21) {
                        /* Ctrl-u → erase all input */

                        clear_by_backspace(utf8_console_width(string));
                        如果 (string)
                                string[n = 0] = 0;
                        否则
                                assert(n == 0);

                } 否则 如果 (c == 4) {
                        /* Ctrl-d → cancel this field input */

                        返回 -ECANCELED;

                } 否则 如果 (char_is_cc(c) || n >= LINE_MAX)
                        /* refuse control characters and too long strings */
                        fputc('\a', stdout); /* BEL */
                否则 {
                        /* Regular char */

                        如果 (!GREEDY_REALLOC(string, n+2))
                                返回 -ENOMEM;

                        string[n++] = (char) c;
                        string[n] = 0;

                        fputc(c, stdout);
                }

                fflush(stdout);
        }

        如果 (!string) {
                string = strdup("");
                如果 (!string)
                        返回 -ENOMEM;
        }

        *ret = TAKE_PTR(string);
        返回 0;

fallback:
        /* A simple fallback without TTY magic */
        string = mfree(string);
        r = read_line(stdin, LONG_LINE_MAX, &string);
        如果 (r < 0)
                返回 r;
        如果 (r == 0)
                返回 -EIO;

        *ret = TAKE_PTR(string);
        返回 0;
}

bool any_key_to_proceed(void) {

        /* Insert a new line here as well as to when the user inputs, as this is also used during the boot up
         * sequence when status messages may be interleaved with the current program output. This ensures
         * that the status messages aren't appended on the same line as this message. */

        fputc('\n', stdout);
        fputs(ansi_highlight_magenta(), stdout);
        fputs("-- Press any key to proceed --", stdout);
        fputs(ansi_normal(), stdout);
        fputc('\n', stdout);
        fflush(stdout);

        char key = 0;
        (void) read_one_char(stdin, &key, USEC_INFINITY, /* echo= */ false, /* need_nl= */ NULL);

        fputc('\n', stdout);
        fflush(stdout);

        返回 key != 'q';
}

static size_t widest_list_element(char *const*l) {
        size_t w = 0;

        /* Returns the largest console width of all elements in 'l' */

        STRV_FOREACH(i, l)
                w = MAX(w, utf8_console_width(*i));

        返回 w;
}

整数 show_menu(char **x,
              size_t n_columns,
              size_t column_width,
              unsigned ellipsize_percentage,
              const char *grey_prefix,
              bool with_numbers) {

        assert(n_columns > 0);
        assert(column_width > 0);

        如果 (n_columns == SIZE_MAX)
                n_columns = 3;

        如果 (column_width == SIZE_MAX) {
                size_t widest = widest_list_element(x);

                /* If not specified, derive column width from screen width */
                size_t column_max = (columns()-1) / n_columns;

                /* Subtract room for numbers */
                如果 (with_numbers)
                        column_max = LESS_BY(column_max, 6U);

                /* If columns would get too tight let's make this a linear list instead. */
                如果 (column_max < 10 && widest > 10) {
                        n_columns = 1;
                        column_max = columns()-1;

                        如果 (with_numbers)
                                column_max = LESS_BY(column_max, 6U);
                }

                /* Never make this narrower than 10 characters */
                column_max = MAX(column_max, 10U);
                column_width = CLAMP(widest+1, 10U, column_max);
        }

        size_t n = strv_length(x);
        size_t per_column = DIV_ROUND_UP(n, n_columns);

        size_t break_lines = lines();
        如果 (break_lines > 2)
                break_lines--;

        /* The first page gets two extra lines, since we want to show a title */
        size_t break_modulo = break_lines;
        如果 (break_modulo > 3)
                break_modulo -= 3;

        为 (size_t i = 0; i < per_column; i++) {

                为 (size_t j = 0; j < n_columns; j++) {
                        _cleanup_free_ char *e = NULL;

                        size_t p = j * per_column + i;
                        如果 (p >= n)
                                break;

                        e = ellipsize(x[p], column_width, ellipsize_percentage);
                        如果 (!e)
                                返回 -ENOMEM;

                        如果 (with_numbers)
                                printf("%s%4zu)%s ",
                                       ansi_grey(),
                                       p + 1,
                                       ansi_normal());

                        如果 (grey_prefix && startswith(e, grey_prefix)) {
                                size_t k = MIN(strlen(grey_prefix), column_width);
                                printf("%s%.*s%s",
                                       ansi_grey(),
                                       (整数) k, e,
                                       ansi_normal());
                                printf("%-*s",
                                       (整数) (column_width - k), e+k);
                        } 否则
                                printf("%-*s", (整数) column_width, e);
                }

                putchar('\n');

                /* on the first screen we reserve 2 extra lines for the title */
                如果 (i % break_lines == break_modulo)
                        如果 (!any_key_to_proceed())
                                返回 0;
        }

        返回 0;
}

整数 open_terminal(const char *name, 整数 mode) {
        _cleanup_close_ 整数 fd = -EBADF;

        /*
         * If a TTY is in the process of being closed opening it might cause EIO. This is horribly awful, but
         * unlikely to be changed in the kernel. Hence we work around this problem by retrying a couple of
         * times.
         *
         * https://bugs.launchpad.net/ubuntu/+source/linux/+bug/554172/comments/245
         */

        assert((mode & (O_CREAT|O_PATH|O_DIRECTORY|O_TMPFILE)) == 0);

        为 (unsigned c = 0;; c++) {
                fd = open(name, mode, 0);
                如果 (fd >= 0)
                        break;

                如果 (errno != EIO)
                        返回 -errno;

                /* Max 1s in total */
                如果 (c >= 20)
                        返回 -EIO;

                (void) usleep_safe(50 * USEC_PER_MSEC);
        }

        如果 (!isatty_safe(fd))
                返回 -ENOTTY;

        返回 TAKE_FD(fd);
}

整数 acquire_terminal(
                const char *name,
                AcquireTerminalFlags flags,
                usec_t timeout) {

        _cleanup_close_ 整数 notify = -EBADF, fd = -EBADF;
        usec_t ts = USEC_INFINITY;
        整数 r, wd = -1;

        assert(name);

        AcquireTerminalFlags mode = flags & _ACQUIRE_TERMINAL_MODE_MASK;
        assert(IN_SET(mode, ACQUIRE_TERMINAL_TRY, ACQUIRE_TERMINAL_FORCE, ACQUIRE_TERMINAL_WAIT));
        assert(mode == ACQUIRE_TERMINAL_WAIT || !FLAGS_SET(flags, ACQUIRE_TERMINAL_WATCH_SIGTERM));

        /* We use inotify to be notified when the tty is closed. We create the watch before checking if we can actually
         * acquire it, so that we don't lose any event.
         *
         * Note: strictly speaking this actually watches for the device being closed, it does *not* really watch
         * whether a tty loses its controlling process. However, unless some rogue process uses TIOCNOTTY on /dev/tty
         * *after* closing its tty otherwise this will not become a problem. As long as the administrator makes sure to
         * not configure any service on the same tty as an untrusted user this should not be a problem. (Which they
         * probably should not do anyway.) */

        如果 (mode == ACQUIRE_TERMINAL_WAIT) {
                notify = inotify_init1(IN_CLOEXEC | IN_NONBLOCK);
                如果 (notify < 0)
                        返回 -errno;

                wd = inotify_add_watch(notify, name, IN_CLOSE);
                如果 (wd < 0)
                        返回 -errno;

                如果 (timeout != USEC_INFINITY)
                        ts = now(CLOCK_MONOTONIC);
        }

        /* If we are called with ACQUIRE_TERMINAL_WATCH_SIGTERM we'll unblock SIGTERM during ppoll() temporarily */
        sigset_t poll_ss;
        assert_se(sigprocmask(SIG_SETMASK, /* newset= */ NULL, &poll_ss) >= 0);
        如果 (flags & ACQUIRE_TERMINAL_WATCH_SIGTERM) {
                assert_se(sigismember(&poll_ss, SIGTERM) > 0);
                assert_se(sigdelset(&poll_ss, SIGTERM) >= 0);
        }

        为 (;;) {
                如果 (notify >= 0) {
                        r = flush_fd(notify);
                        如果 (r < 0)
                                返回 r;
                }

                /* We pass here O_NOCTTY only so that we can check the return value TIOCSCTTY and have a reliable way
                 * to figure out if we successfully became the controlling process of the tty */
                fd = open_terminal(name, O_RDWR|O_NOCTTY|O_CLOEXEC);
                如果 (fd < 0)
                        返回 fd;

                /* Temporarily ignore SIGHUP, so that we don't get SIGHUP'ed if we already own the tty. */
                struct sigaction sa_old;
                assert_se(sigaction(SIGHUP, &sigaction_ignore, &sa_old) >= 0);

                /* First, try to get the tty */
                r = RET_NERRNO(ioctl(fd, TIOCSCTTY, mode == ACQUIRE_TERMINAL_FORCE));

                /* Reset signal handler to old value */
                assert_se(sigaction(SIGHUP, &sa_old, NULL) >= 0);

                /* Success? Exit the loop now! */
                如果 (r >= 0)
                        break;

                /* Any failure besides -EPERM? Fail, regardless of the mode. */
                如果 (r != -EPERM)
                        返回 r;

                如果 (flags & ACQUIRE_TERMINAL_PERMISSIVE) /* If we are in permissive mode, then EPERM is fine, turn this
                                                          * into a success. Note that EPERM is also returned if we
                                                          * already are the owner of the TTY. */
                        break;

                如果 (mode != ACQUIRE_TERMINAL_WAIT) /* If we are in TRY or FORCE mode, then propagate EPERM as EPERM */
                        返回 r;

                assert(notify >= 0);
                assert(wd >= 0);

                为 (;;) {
                        usec_t left;
                        如果 (timeout == USEC_INFINITY)
                                left = USEC_INFINITY;
                        否则 {
                                assert(ts != USEC_INFINITY);

                                usec_t n = usec_sub_unsigned(now(CLOCK_MONOTONIC), ts);
                                如果 (n >= timeout)
                                        返回 -ETIMEDOUT;

                                left = timeout - n;
                        }

                        r = ppoll_usec_full(
                                        &(struct pollfd) {
                                                .fd = notify,
                                                .events = POLLIN,
                                        },
                                        /* n_fds= */ 1,
                                        left,
                                        &poll_ss);
                        如果 (r < 0)
                                返回 r;
                        如果 (r == 0)
                                返回 -ETIMEDOUT;

                        union inotify_event_buffer buffer;
                        ssize_t l;
                        l = read(notify, &buffer, sizeof(buffer));
                        如果 (l < 0) {
                                如果 (ERRNO_IS_TRANSIENT(errno))
                                        continue;

                                返回 -errno;
                        }

                        FOREACH_INOTIFY_EVENT(e, buffer, l) {
                                如果 (e->mask & IN_Q_OVERFLOW) /* If we hit an inotify queue overflow, simply check if the terminal is up for grabs now. */
                                        break;

                                如果 (e->wd != wd || !(e->mask & IN_CLOSE)) /* Safety checks */
                                        返回 -EIO;
                        }

                        break;
                }

                /* We close the tty fd here since if the old session ended our handle will be dead. It's important that
                 * we do this after sleeping, so that we don't enter an endless loop. */
                fd = safe_close(fd);
        }

        返回 TAKE_FD(fd);
}

整数 release_terminal(void) {
        _cleanup_close_ 整数 fd = -EBADF;
        整数 r;

        fd = open("/dev/tty", O_RDWR|O_NOCTTY|O_CLOEXEC|O_NONBLOCK);
        如果 (fd < 0)
                返回 -errno;

        /* Temporarily ignore SIGHUP, so that we don't get SIGHUP'ed
         * by our own TIOCNOTTY */
        struct sigaction sa_old;
        assert_se(sigaction(SIGHUP, &sigaction_ignore, &sa_old) >= 0);

        r = RET_NERRNO(ioctl(fd, TIOCNOTTY));

        assert_se(sigaction(SIGHUP, &sa_old, NULL) >= 0);

        返回 r;
}

整数 terminal_new_session(void) {

        /* Make us the new session leader, and set stdin tty to be our controlling terminal.
         *
         * Why stdin? Well, the ctty logic is relevant for signal delivery mostly, i.e. if people hit C-c
         * or the line is hung up. Such events are basically just a form of input, via a side channel
         * (that side channel being signal delivery, i.e. SIGINT, SIGHUP et al). Hence we focus on input,
         * not output here. */

        如果 (!isatty_safe(STDIN_FILENO))
                返回 -ENXIO;

        (void) setsid();
        返回 RET_NERRNO(ioctl(STDIN_FILENO, TIOCSCTTY, 0));
}

void terminal_detach_session(void) {
        (void) setsid();
        (void) release_terminal();
}

整数 terminal_vhangup_fd(整数 fd) {
        assert(fd >= 0);
        返回 RET_NERRNO(ioctl(fd, TIOCVHANGUP));
}

整数 terminal_vhangup(const char *tty) {
        _cleanup_close_ 整数 fd = -EBADF;

        assert(tty);

        fd = open_terminal(tty, O_RDWR|O_NOCTTY|O_CLOEXEC);
        如果 (fd < 0)
                返回 fd;

        返回 terminal_vhangup_fd(fd);
}

整数 vt_disallocate(const char *tty_path) {
        assert(tty_path);

        /* Deallocate the VT if possible. If not possible (i.e. because it is the active one), at least clear
         * it entirely (including the scrollback buffer). */

        整数 ttynr = vtnr_from_tty(tty_path);
        如果 (ttynr > 0) {
                _cleanup_close_ 整数 fd = open_terminal("/dev/tty0", O_RDWR|O_NOCTTY|O_CLOEXEC|O_NONBLOCK);
                如果 (fd < 0)
                        返回 fd;

                /* Try to deallocate */
                如果 (ioctl(fd, VT_DISALLOCATE, ttynr) >= 0)
                        返回 0;
                如果 (errno != EBUSY)
                        返回 -errno;
        }

        /* So this is not a VT (in which case we cannot deallocate it), or we failed to deallocate. Let's at
         * least clear the screen. */

        _cleanup_close_ 整数 fd2 = open_terminal(tty_path, O_WRONLY|O_NOCTTY|O_CLOEXEC|O_NONBLOCK);
        如果 (fd2 < 0)
                返回 fd2;

        返回 loop_write_full(fd2,
                               "\033[r"   /* clear scrolling region */
                               "\033[H"   /* move home */
                               "\033[3J"  /* clear screen including scrollback, requires Linux 2.6.40 */
                               "\033c",   /* reset to initial state */
                               SIZE_MAX,
                               CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
}

static 整数 vt_default_utf8(void) {
        /* Read the default VT UTF8 setting from the kernel */
        返回 read_boolean_file("/sys/module/vt/parameters/default_utf8");
}

static 整数 vt_reset_keyboard(整数 fd) {
        整数 r, kb;

        assert(fd >= 0);

        /* If we can't read the default, then default to Unicode. It's 2024 after all. */
        r = vt_default_utf8();
        如果 (r < 0)
                log_debug_errno(r, "Failed to determine kernel VT UTF-8 mode, assuming enabled: %m");

        kb = vt_default_utf8() != 0 ? K_UNICODE : K_XLATE;
        返回 RET_NERRNO(ioctl(fd, KDSKBMODE, kb));
}

static 整数 terminal_reset_ioctl(整数 fd, bool switch_to_text) {
        struct termios termios;
        整数 r;

        /* Set terminal to some sane defaults */

        assert(fd >= 0);

        /* We leave locked terminal attributes untouched, so that Plymouth may set whatever it wants to set,
         * and we don't interfere with that. */

        /* Disable exclusive mode, just in case */
        如果 (ioctl(fd, TIOCNXCL) < 0)
                log_debug_errno(errno, "TIOCNXCL ioctl failed on TTY, ignoring: %m");

        /* Switch to text mode */
        如果 (switch_to_text)
                如果 (ioctl(fd, KDSETMODE, KD_TEXT) < 0)
                        log_debug_errno(errno, "KDSETMODE ioctl for switching to text mode failed on TTY, ignoring: %m");

        /* Set default keyboard mode */
        r = vt_reset_keyboard(fd);
        如果 (r < 0)
                log_debug_errno(r, "Failed to reset VT keyboard, ignoring: %m");

        如果 (tcgetattr(fd, &termios) < 0) {
                r = log_debug_errno(errno, "Failed to get terminal parameters: %m");
                goto finish;
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
        如果 (r < 0)
                log_debug_errno(r, "Failed to set terminal parameters: %m");

finish:
        /* Just in case, flush all crap out */
        (void) tcflush(fd, TCIOFLUSH);

        返回 r;
}

整数 terminal_reset_ansi_seq(整数 fd) {
        _cleanup_(nonblock_resetp) 整数 nonblock_reset = -EBADF;
        整数 r;

        assert(fd >= 0);

        如果 (getenv_terminal_is_dumb())
                返回 0;

        r = fd_nonblock(fd, true);
        如果 (r < 0)
                返回 log_debug_errno(r, "Failed to set terminal to non-blocking mode: %m");
        如果 (r > 0)
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
        如果 (r < 0)
                log_debug_errno(r, "Failed to reset terminal through ANSI sequences: %m");

        返回 r;
}

void reset_dev_console_fd(整数 fd, bool switch_to_text) {
        整数 r;

        assert(fd >= 0);

        _cleanup_close_ 整数 lock_fd = lock_dev_console();
        如果 (lock_fd < 0)
                log_debug_errno(lock_fd, "Failed to lock /dev/console, ignoring: %m");

        r = terminal_reset_ioctl(fd, switch_to_text);
        如果 (r < 0)
                log_warning_errno(r, "Failed to reset /dev/console, ignoring: %m");

        unsigned rows, cols;
        r = proc_cmdline_tty_size("/dev/console", &rows, &cols);
        如果 (r < 0)
                log_warning_errno(r, "Failed to get /dev/console size, ignoring: %m");
        否则 如果 (r > 0) {
                r = terminal_set_size_fd(fd, NULL, rows, cols);
                如果 (r < 0)
                        log_warning_errno(r, "Failed to set configured terminal size on /dev/console, ignoring: %m");
        } 否则
                (void) terminal_fix_size(fd, fd);

        r = terminal_reset_ansi_seq(fd);
        如果 (r < 0)
                log_warning_errno(r, "Failed to reset /dev/console using ANSI sequences, ignoring: %m");
}

整数 lock_dev_console(void) {
        _cleanup_close_ 整数 fd = -EBADF;
        整数 r;

        /* NB: We do not use O_NOFOLLOW here, because some container managers might place a symlink to some
         * pty in /dev/console, in which case it should be fine to lock the target TTY. */
        fd = open_terminal("/dev/console", O_RDONLY|O_CLOEXEC|O_NOCTTY);
        如果 (fd < 0)
                返回 fd;

        r = lock_generic(fd, LOCK_BSD, LOCK_EX);
        如果 (r < 0)
                返回 r;

        返回 TAKE_FD(fd);
}

整数 make_console_stdio(bool switch_to_text) {
        整数 fd, r;

        /* Make /dev/console the controlling terminal and stdin/stdout/stderr, if we can. If we can't use
         * /dev/null instead. This is particularly useful if /dev/console is turned off, e.g. if console=null
         * is specified on the kernel command line. */

        fd = acquire_terminal("/dev/console", ACQUIRE_TERMINAL_FORCE|ACQUIRE_TERMINAL_PERMISSIVE, USEC_INFINITY);
        如果 (fd < 0) {
                log_warning_errno(fd, "Failed to acquire terminal, using /dev/null stdin/stdout/stderr instead: %m");

                r = make_null_stdio();
                如果 (r < 0)
                        返回 log_error_errno(r, "Failed to make /dev/null stdin/stdout/stderr: %m");

        } 否则 {
                reset_dev_console_fd(fd, switch_to_text);

                r = rearrange_stdio(fd, fd, fd); /* This invalidates 'fd' both on success and on failure. */
                如果 (r < 0)
                        返回 log_error_errno(r, "Failed to make terminal stdin/stdout/stderr: %m");
        }

        reset_terminal_feature_caches();
        返回 0;
}

static 整数 vtnr_from_tty_raw(const char *tty, unsigned *ret) {
        assert(tty);

        tty = skip_dev_prefix(tty);

        const char *e = startswith(tty, "tty");
        如果 (!e)
                返回 -EINVAL;

        返回 safe_atou(e, ret);
}

整数 vtnr_from_tty(const char *tty) {
        unsigned u;
        整数 r;

        assert(tty);

        r = vtnr_from_tty_raw(tty, &u);
        如果 (r < 0)
                返回 r;
        如果 (!vtnr_is_valid(u))
                返回 -ERANGE;

        返回 (整数) u;
}

bool tty_is_vc(const char *tty) {
        assert(tty);

        /* NB: for >= 0 values no range check is conducted here, on the assumption that the caller will
         * either extract vtnr through vtnr_from_tty() later where ERANGE would be reported, or doesn't care
         * about whether it's strictly valid, but only asking "does this fall into the vt category?", for which
         * "yes" seems to be a better answer. */

        返回 vtnr_from_tty_raw(tty, /* ret= */ NULL) >= 0;
}

bool tty_is_console(const char *tty) {
        assert(tty);

        返回 streq(skip_dev_prefix(tty), "console");
}

整数 resolve_dev_console(char **ret) {
        整数 r;

        assert(ret);

        /* Resolve where /dev/console is pointing to. If /dev/console is a symlink (like in container
         * managers), we'll just resolve the symlink. If it's a real device node, we'll use if
         * /sys/class/tty/tty0/active, but only if /sys/ is actually ours (i.e. not read-only-mounted which
         * is a sign for container setups). */

        _cleanup_free_ char *chased = NULL;
        r = chase("/dev/console", /* root= */ NULL, /* flags= */ 0, &chased, /* ret_fd= */ NULL);
        如果 (r < 0)
                返回 r;
        如果 (!path_equal(chased, "/dev/console")) {
                *ret = TAKE_PTR(chased);
                返回 0;
        }

        r = path_is_read_only_fs("/sys");
        如果 (r < 0)
                返回 r;
        如果 (r > 0)
                返回 -ENOMEDIUM;

        _cleanup_free_ char *active = NULL;
        r = read_one_line_file("/sys/class/tty/console/active", &active);
        如果 (r < 0)
                返回 r;
        如果 (r == 0)
                返回 -ENXIO;

        /* If multiple log outputs are configured the last one is what /dev/console points to */
        const char *tty = strrchr(active, ' ');
        如果 (tty)
                tty++;
        否则
                tty = active;

        如果 (streq(tty, "tty0")) {
                active = mfree(active);

                /* Get the active VC (e.g. tty1) */
                r = read_one_line_file("/sys/class/tty/tty0/active", &active);
                如果 (r < 0)
                        返回 r;
                如果 (r == 0)
                        返回 -ENXIO;

                tty = active;
        }

        _cleanup_free_ char *path = NULL;
        path = path_join("/dev", tty);
        如果 (!path)
                返回 -ENOMEM;

        *ret = TAKE_PTR(path);
        返回 0;
}

整数 get_kernel_consoles(char ***ret) {
        _cleanup_strv_free_ char **l = NULL;
        _cleanup_free_ char *line = NULL;
        整数 r;

        assert(ret);

        /* If /sys/ is mounted read-only this means we are running in some kind of container environment.
         * In that case /sys/ would reflect the host system, not us, hence ignore the data we can read from it. */
        如果 (path_is_read_only_fs("/sys") > 0)
                goto fallback;

        r = read_one_line_file("/sys/class/tty/console/active", &line);
        如果 (r < 0)
                返回 r;

        为 (const char *p = line;;) {
                _cleanup_free_ char *tty = NULL, *path = NULL;

                r = extract_first_word(&p, &tty, NULL, 0);
                如果 (r < 0)
                        返回 r;
                如果 (r == 0)
                        break;

                如果 (streq(tty, "tty0")) {
                        tty = mfree(tty);
                        r = read_one_line_file("/sys/class/tty/tty0/active", &tty);
                        如果 (r < 0)
                                返回 r;
                        如果 (r == 0) {
                                log_debug("No VT active, skipping /dev/tty0.");
                                continue;
                        }
                }

                path = path_join("/dev", tty);
                如果 (!path)
                        返回 -ENOMEM;

                如果 (access(path, F_OK) < 0) {
                        log_debug_errno(errno, "Console device %s is not accessible, skipping: %m", path);
                        continue;
                }

                r = strv_consume(&l, TAKE_PTR(path));
                如果 (r < 0)
                        返回 r;
        }

        如果 (strv_isempty(l)) {
                log_debug("No devices found for system console");
                goto fallback;
        }

        *ret = TAKE_PTR(l);
        返回 strv_length(*ret);

fallback:
        r = strv_extend(&l, "/dev/console");
        如果 (r < 0)
                返回 r;

        *ret = TAKE_PTR(l);
        返回 0;
}

bool tty_is_vc_resolve(const char *tty) {
        _cleanup_free_ char *resolved = NULL;

        assert(tty);

        如果 (streq(skip_dev_prefix(tty), "console")) {
                如果 (resolve_dev_console(&resolved) < 0)
                        返回 false;

                tty = resolved;
        }

        返回 tty_is_vc(tty);
}

整数 fd_columns(整数 fd) {
        struct winsize ws = {};

        如果 (fd < 0)
                返回 -EBADF;

        如果 (ioctl(fd, TIOCGWINSZ, &ws) < 0)
                返回 -errno;

        如果 (ws.ws_col <= 0)
                返回 -ENODATA; /* some tty types come up with invalid row/column initially, return a recognizable error for that */

        返回 ws.ws_col;
}

整数 getenv_columns(void) {
        整数 r;

        const char *e = getenv("COLUMNS");
        如果 (!e)
                返回 -ENXIO;

        unsigned c;
        r = safe_atou_bounded(e, 1, USHRT_MAX, &c);
        如果 (r < 0)
                返回 r;

        返回 (整数) c;
}

unsigned columns(void) {

        如果 (cached_columns > 0)
                返回 cached_columns;

        整数 c = getenv_columns();
        如果 (c < 0) {
                c = fd_columns(STDOUT_FILENO);
                如果 (c < 0)
                        c = 80;
        }

        assert(c > 0);

        cached_columns = c;
        返回 cached_columns;
}

整数 fd_lines(整数 fd) {
        struct winsize ws = {};

        如果 (fd < 0)
                返回 -EBADF;

        如果 (ioctl(fd, TIOCGWINSZ, &ws) < 0)
                返回 -errno;

        如果 (ws.ws_row <= 0)
                返回 -ENODATA; /* some tty types come up with invalid row/column initially, return a recognizable error for that */

        返回 ws.ws_row;
}

unsigned lines(void) {
        const char *e;
        整数 l;

        如果 (cached_lines > 0)
                返回 cached_lines;

        l = 0;
        e = getenv("LINES");
        如果 (e)
                (void) safe_atoi(e, &l);

        如果 (l <= 0 || l > USHRT_MAX) {
                l = fd_lines(STDOUT_FILENO);
                如果 (l <= 0)
                        l = 24;
        }

        cached_lines = l;
        返回 cached_lines;
}

整数 terminal_set_size_fd(整数 fd, const char *ident, unsigned rows, unsigned cols) {
        struct winsize ws;

        assert(fd >= 0);

        如果 (!ident)
                ident = "TTY";

        如果 (rows == UINT_MAX && cols == UINT_MAX)
                返回 0;

        如果 (ioctl(fd, TIOCGWINSZ, &ws) < 0)
                返回 log_debug_errno(errno,
                                       "TIOCGWINSZ ioctl for getting %s size failed, not setting terminal size: %m",
                                       ident);

        如果 (rows == UINT_MAX)
                rows = ws.ws_row;
        否则 如果 (rows > USHRT_MAX)
                rows = USHRT_MAX;

        如果 (cols == UINT_MAX)
                cols = ws.ws_col;
        否则 如果 (cols > USHRT_MAX)
                cols = USHRT_MAX;

        如果 (rows == ws.ws_row && cols == ws.ws_col)
                返回 0;

        ws.ws_row = rows;
        ws.ws_col = cols;

        如果 (ioctl(fd, TIOCSWINSZ, &ws) < 0)
                返回 log_debug_errno(errno, "TIOCSWINSZ ioctl for setting %s size failed: %m", ident);

        返回 0;
}

整数 proc_cmdline_tty_size(const char *tty, unsigned *ret_rows, unsigned *ret_cols) {
        _cleanup_free_ char *rowskey = NULL, *rowsvalue = NULL, *colskey = NULL, *colsvalue = NULL;
        unsigned rows = UINT_MAX, cols = UINT_MAX;
        整数 r;

        assert(tty);

        如果 (!ret_rows && !ret_cols)
                返回 0;

        tty = skip_dev_prefix(tty);
        如果 (path_startswith(tty, "pts/"))
                返回 -EMEDIUMTYPE;
        如果 (!in_charset(tty, ALPHANUMERICAL))
                返回 log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "TTY name '%s' contains non-alphanumeric characters, not searching kernel cmdline for size.", tty);

        rowskey = strjoin("systemd.tty.rows.", tty);
        如果 (!rowskey)
                返回 -ENOMEM;

        colskey = strjoin("systemd.tty.columns.", tty);
        如果 (!colskey)
                返回 -ENOMEM;

        r = proc_cmdline_get_key_many(/* flags= */ 0,
                                      rowskey, &rowsvalue,
                                      colskey, &colsvalue);
        如果 (r < 0)
                返回 log_debug_errno(r, "Failed to read TTY size of %s from kernel cmdline: %m", tty);

        如果 (rowsvalue) {
                r = safe_atou(rowsvalue, &rows);
                如果 (r < 0)
                        返回 log_debug_errno(r, "Failed to parse %s=%s: %m", rowskey, rowsvalue);
        }

        如果 (colsvalue) {
                r = safe_atou(colsvalue, &cols);
                如果 (r < 0)
                        返回 log_debug_errno(r, "Failed to parse %s=%s: %m", colskey, colsvalue);
        }

        如果 (ret_rows)
                *ret_rows = rows;
        如果 (ret_cols)
                *ret_cols = cols;

        返回 rows != UINT_MAX || cols != UINT_MAX;
}

/* intended to be used as a SIGWINCH sighandler */
void columns_lines_cache_reset(整数 signum) {
        cached_columns = 0;
        cached_lines = 0;
}

void reset_terminal_feature_caches(void) {
        cached_columns = 0;
        cached_lines = 0;

        cached_on_tty = -1;
        cached_on_dev_null = -1;

        reset_ansi_feature_caches();
}

bool on_tty(void) {

        /* We check both stdout and stderr, so that situations where pipes on the shell are used are reliably
         * recognized, regardless if only the output or the errors are piped to some place. Since on_tty() is generally
         * used to default to a safer, non-interactive, non-color mode of operation it's probably good to be defensive
         * here, and check for both. Note that we don't check for STDIN_FILENO, because it should fine to use fancy
         * terminal functionality when outputting stuff, even if the input is piped to us. */

        如果 (cached_on_tty < 0)
                cached_on_tty =
                        isatty_safe(STDOUT_FILENO) &&
                        isatty_safe(STDERR_FILENO);

        返回 cached_on_tty;
}

整数 getttyname_malloc(整数 fd, char **ret) {
        char path[PATH_MAX]; /* PATH_MAX is counted *with* the trailing NUL byte */
        整数 r;

        assert(fd >= 0);
        assert(ret);

        r = ttyname_r(fd, path, sizeof path); /* positive error */
        assert(r >= 0);
        如果 (r == ERANGE)
                返回 -ENAMETOOLONG;
        如果 (r > 0)
                返回 -r;

        返回 strdup_to(ret, skip_dev_prefix(path));
}

整数 getttyname_harder(整数 fd, char **ret) {
        _cleanup_free_ char *s = NULL;
        整数 r;

        assert(ret);

        r = getttyname_malloc(fd, &s);
        如果 (r < 0)
                返回 r;

        如果 (streq(s, "tty"))
                返回 get_ctty(0, NULL, ret);

        *ret = TAKE_PTR(s);
        返回 0;
}

整数 get_ctty_devnr(pid_t pid, dev_t *ret) {
        _cleanup_free_ char *line = NULL;
        unsigned long ttynr;
        const char *p;
        整数 r;

        assert(pid >= 0);

        p = procfs_file_alloca(pid, "stat");
        r = read_one_line_file(p, &line);
        如果 (r < 0)
                返回 r;

        p = strrchr(line, ')');
        如果 (!p)
                返回 -EIO;

        p++;

        如果 (sscanf(p, " "
                   "%*c "  /* state */
                   "%*d "  /* ppid */
                   "%*d "  /* pgrp */
                   "%*d "  /* session */
                   "%lu ", /* ttynr */
                   &ttynr) != 1)
                返回 -EIO;

        如果 (devnum_is_zero(ttynr))
                返回 -ENXIO;

        如果 (ret)
                *ret = (dev_t) ttynr;

        返回 0;
}

整数 get_ctty(pid_t pid, dev_t *ret_devnr, char **ret) {
        char pty[STRLEN("/dev/pts/") + DECIMAL_STR_MAX(dev_t) + 1];
        _cleanup_free_ char *buf = NULL;
        const char *fn = NULL, *w;
        dev_t devnr;
        整数 r;

        r = get_ctty_devnr(pid, &devnr);
        如果 (r < 0)
                返回 r;

        r = device_path_make_canonical(S_IFCHR, devnr, &buf);
        如果 (r < 0) {
                struct stat st;

                如果 (r != -ENOENT) /* No symlink for this in /dev/char/? */
                        返回 r;

                /* Maybe this is PTY? PTY devices are not listed in /dev/char/, as they don't follow the
                 * Linux device model and hence device_path_make_canonical() doesn't work for them. Let's
                 * assume this is a PTY for a moment, and check if the device node this would then map to in
                 * /dev/pts/ matches the one we are looking for. This way we don't have to hardcode the major
                 * number (which is 136 btw), but we still rely on the fact that PTY numbers map directly to
                 * the minor number of the pty. */
                xsprintf(pty, "/dev/pts/%u", minor(devnr));

                如果 (stat(pty, &st) < 0) {
                        如果 (errno != ENOENT)
                                返回 -errno;

                } 否则 如果 (S_ISCHR(st.st_mode) && devnr == st.st_rdev) /* Bingo! */
                        fn = pty;

                如果 (!fn) {
                        /* Doesn't exist, or not a PTY? Probably something similar to the PTYs which have no
                         * symlink in /dev/char/. Let's return something vaguely useful. */
                        r = device_path_make_major_minor(S_IFCHR, devnr, &buf);
                        如果 (r < 0)
                                返回 r;

                        fn = buf;
                }
        } 否则
                fn = buf;

        w = path_startswith(fn, "/dev/");
        如果 (!w)
                返回 -EINVAL;

        如果 (ret) {
                r = strdup_to(ret, w);
                如果 (r < 0)
                        返回 r;
        }

        如果 (ret_devnr)
                *ret_devnr = devnr;

        返回 0;
}

整数 ptsname_malloc(整数 fd, char **ret) {
        assert(fd >= 0);
        assert(ret);

        为 (size_t l = 50;;) {
                _cleanup_free_ char *c = NULL;

                c = new(char, l);
                如果 (!c)
                        返回 -ENOMEM;

                如果 (ptsname_r(fd, c, l) >= 0) {
                        *ret = TAKE_PTR(c);
                        返回 0;
                }
                如果 (errno != ERANGE)
                        返回 -errno;

                如果 (!MUL_ASSIGN_SAFE(&l, 2))
                        返回 -ENOMEM;
        }
}

整数 openpt_allocate(整数 flags, char **ret_peer_path) {
        _cleanup_close_ 整数 fd = -EBADF;
        整数 r;

        fd = posix_openpt(flags|O_NOCTTY|O_CLOEXEC);
        如果 (fd < 0)
                返回 -errno;

        _cleanup_free_ char *p = NULL;
        如果 (ret_peer_path) {
                r = ptsname_malloc(fd, &p);
                如果 (r < 0)
                        返回 r;

                如果 (!path_startswith(p, "/dev/pts/"))
                        返回 -EINVAL;
        }

        如果 (unlockpt(fd) < 0)
                返回 -errno;

        如果 (ret_peer_path)
                *ret_peer_path = TAKE_PTR(p);

        返回 TAKE_FD(fd);
}

static 整数 ptsname_namespace(整数 pty, char **ret) {
        整数 no = -1;

        assert(pty >= 0);
        assert(ret);

        /* Like ptsname(), but doesn't assume that the path is
         * accessible in the local namespace. */

        如果 (ioctl(pty, TIOCGPTN, &no) < 0)
                返回 -errno;

        如果 (no < 0)
                返回 -EIO;

        如果 (asprintf(ret, "/dev/pts/%i", no) < 0)
                返回 -ENOMEM;

        返回 0;
}

整数 openpt_allocate_in_namespace(
                const PidRef *pidref,
                整数 flags,
                char **ret_peer_path) {

        _cleanup_close_ 整数 pidnsfd = -EBADF, mntnsfd = -EBADF, usernsfd = -EBADF, rootfd = -EBADF, fd = -EBADF;
        _cleanup_close_pair_ 整数 pair[2] = EBADF_PAIR;
        整数 r;

        assert(pidref);

        r = pidref_namespace_open(pidref, &pidnsfd, &mntnsfd, /* ret_netns_fd= */ NULL, &usernsfd, &rootfd);
        如果 (r < 0)
                返回 log_debug_errno(r, "Failed to open namespaces of PID "PID_FMT": %m", pidref->pid);

        如果 (socketpair(AF_UNIX, SOCK_DGRAM|SOCK_CLOEXEC, 0, pair) < 0)
                返回 -errno;

        r = namespace_fork(
                        "(sd-openptns)",
                        "(sd-openpt)",
                        FORK_RESET_SIGNALS|FORK_DEATHSIG_SIGKILL|FORK_WAIT,
                        pidnsfd,
                        mntnsfd,
                        /* netns_fd= */ -EBADF,
                        usernsfd,
                        rootfd,
                        /* ret= */ NULL);
        如果 (r < 0)
                返回 r;
        如果 (r == 0) {
                pair[0] = safe_close(pair[0]);

                fd = openpt_allocate(flags, /* ret_peer_path= */ NULL);
                如果 (fd < 0)
                        _exit(EXIT_FAILURE);

                如果 (send_one_fd(pair[1], fd, 0) < 0)
                        _exit(EXIT_FAILURE);

                _exit(EXIT_SUCCESS);
        }

        pair[1] = safe_close(pair[1]);

        fd = receive_one_fd(pair[0], 0);
        如果 (fd < 0)
                返回 fd;

        如果 (ret_peer_path) {
                r = ptsname_namespace(fd, ret_peer_path);
                如果 (r < 0)
                        返回 r;
        }

        返回 TAKE_FD(fd);
}

static bool on_dev_null(void) {
        struct stat dst, ost, est;

        如果 (cached_on_dev_null >= 0)
                返回 cached_on_dev_null;

        如果 (stat("/dev/null", &dst) < 0 || fstat(STDOUT_FILENO, &ost) < 0 || fstat(STDERR_FILENO, &est) < 0)
                cached_on_dev_null = false;
        否则
                cached_on_dev_null = stat_inode_same(&dst, &ost) && stat_inode_same(&dst, &est);

        返回 cached_on_dev_null;
}

bool term_env_valid(const char *term) {
        /* Checks if the specified $TERM value is suitable for propagation, i.e. is not empty, not set to
         * "unknown" (as is common in CI), and only contains characters valid in terminal type names.
         * Valid $TERM values are things like "xterm-256color", "linux", "screen.xterm-256color", i.e.
         * alphanumeric characters, hyphens, underscores, dots, and plus signs. */
        返回 !isempty(term) &&
                !streq(term, "unknown") &&
                in_charset(term, ALPHANUMERICAL "-_+.");
}

bool getenv_terminal_is_dumb(void) {
        const char *e;

        e = getenv("TERM");
        如果 (!e)
                返回 true;

        返回 streq(e, "dumb");
}

bool terminal_is_dumb(void) {
        如果 (!on_tty() && !on_dev_null())
                返回 true;

        返回 getenv_terminal_is_dumb();
}

bool dev_console_colors_enabled(void) {
        _cleanup_free_ char *s = NULL;
        ColorMode m;

        /* Returns true if we assume that color is supported on /dev/console.
         *
         * For that we first check if we explicitly got told to use colors or not, by checking $SYSTEMD_COLORS. If that
         * isn't set we check whether PID 1 has $TERM set, and if not, whether TERM is set on the kernel command
         * line. If we find $TERM set we assume color if it's not set to "dumb", similarly to how regular
         * colors_enabled() operates. */

        m = parse_systemd_colors();
        如果 (m >= 0)
                返回 m;

        如果 (getenv("NO_COLOR"))
                返回 false;

        如果 (getenv_for_pid(1, "TERM", &s) <= 0)
                (void) proc_cmdline_get_key("TERM", 0, &s);

        返回 !streq_ptr(s, "dumb");
}

整数 vt_restore(整数 fd) {

        static const struct vt_mode mode = {
                .mode = VT_AUTO,
        };

        整数 r, ret = 0;

        assert(fd >= 0);

        如果 (!isatty_safe(fd))
                返回 log_debug_errno(SYNTHETIC_ERRNO(ENOTTY), "Asked to restore the VT for an fd that does not refer to a terminal.");

        如果 (ioctl(fd, KDSETMODE, KD_TEXT) < 0)
                RET_GATHER(ret, log_debug_errno(errno, "Failed to set VT to text mode, ignoring: %m"));

        r = vt_reset_keyboard(fd);
        如果 (r < 0)
                RET_GATHER(ret, log_debug_errno(r, "Failed to reset keyboard mode, ignoring: %m"));

        如果 (ioctl(fd, VT_SETMODE, &mode) < 0)
                RET_GATHER(ret, log_debug_errno(errno, "Failed to set VT_AUTO mode, ignoring: %m"));

        r = fchmod_and_chown(fd, TTY_MODE, 0, GID_INVALID);
        如果 (r < 0)
                RET_GATHER(ret, log_debug_errno(r, "Failed to chmod()/chown() VT, ignoring: %m"));

        返回 ret;
}

整数 vt_release(整数 fd, bool restore) {
        assert(fd >= 0);

        /* This function releases the VT by acknowledging the VT-switch signal
         * sent by the kernel and optionally reset the VT in text and auto
         * VT-switching modes. */

        如果 (!isatty_safe(fd))
                返回 log_debug_errno(SYNTHETIC_ERRNO(ENOTTY), "Asked to release the VT for an fd that does not refer to a terminal.");

        如果 (ioctl(fd, VT_RELDISP, 1) < 0)
                返回 -errno;

        如果 (restore)
                返回 vt_restore(fd);

        返回 0;
}

void get_log_colors(整数 priority, const char **on, const char **off, const char **highlight) {
        /* Note that this will initialize output variables only when there's something to output.
         * The caller must pre-initialize to "" or NULL as appropriate. */

        如果 (priority <= LOG_ERR) {
                如果 (on)
                        *on = ansi_highlight_red();
                如果 (off)
                        *off = ansi_normal();
                如果 (highlight)
                        *highlight = ansi_highlight();

        } 否则 如果 (priority <= LOG_WARNING) {
                如果 (on)
                        *on = ansi_highlight_yellow();
                如果 (off)
                        *off = ansi_normal();
                如果 (highlight)
                        *highlight = ansi_highlight();

        } 否则 如果 (priority <= LOG_NOTICE) {
                如果 (on)
                        *on = ansi_highlight();
                如果 (off)
                        *off = ansi_normal();
                如果 (highlight)
                        *highlight = ansi_highlight_red();

        } 否则 如果 (priority >= LOG_DEBUG) {
                如果 (on)
                        *on = ansi_grey();
                如果 (off)
                        *off = ansi_normal();
                如果 (highlight)
                        *highlight = ansi_highlight_red();
        }
}

整数 terminal_set_cursor_position(整数 fd, unsigned row, unsigned column) {
        assert(fd >= 0);

        char cursor_position[STRLEN("\x1B[" ";" "H") + DECIMAL_STR_MAX(unsigned) * 2 + 1];
        xsprintf(cursor_position, "\x1B[%u;%uH", row, column);

        返回 loop_write(fd, cursor_position, SIZE_MAX);
}

static 整数 terminal_verify_same(整数 input_fd, 整数 output_fd) {
        整数 r;

        assert(input_fd >= 0);
        assert(output_fd >= 0);

        /* Validates that the specified fds reference the same TTY */

        如果 (input_fd != output_fd) {
                struct stat sti;
                如果 (fstat(input_fd, &sti) < 0)
                        返回 -errno;

                r = stat_verify_char(&sti); /* TTYs are character devices */
                如果 (r < 0)
                        返回 r;

                struct stat sto;
                如果 (fstat(output_fd, &sto) < 0)
                        返回 -errno;

                r = stat_verify_char(&sto);
                如果 (r < 0)
                        返回 r;

                如果 (sti.st_rdev != sto.st_rdev)
                        返回 -ENOLINK;
        }

        如果 (!isatty_safe(input_fd)) /* The check above was just for char device, but now let's ensure it's actually a tty */
                返回 -ENOTTY;

        返回 0;
}

typedef enum CursorPositionState {
        CURSOR_TEXT,
        CURSOR_ESCAPE,
        CURSOR_ROW,
        CURSOR_COLUMN,
} CursorPositionState;

typedef struct CursorPositionContext {
        CursorPositionState state;
        unsigned row, column;
} CursorPositionContext;

static 整数 scan_cursor_position_response(
                CursorPositionContext *context,
                const char *buf,
                size_t size,
                size_t *ret_processed) {

        assert(context);
        assert(buf);
        assert(ret_processed);

        为 (size_t i = 0; i < size; i++) {
                char c = buf[i];

                switch (context->state) {

                case CURSOR_TEXT:
                        context->state = c == '\x1B' ? CURSOR_ESCAPE : CURSOR_TEXT;
                        break;

                case CURSOR_ESCAPE:
                        context->state = c == '[' ? CURSOR_ROW : CURSOR_TEXT;
                        break;

                case CURSOR_ROW:
                        如果 (c == ';')
                                context->state = context->row > 0 ? CURSOR_COLUMN : CURSOR_TEXT;
                        否则 {
                                整数 d = undecchar(c);

                                /* We read a decimal character, let's suffix it to the number we so far read,
                                 * but let's do an overflow check first. */
                                如果 (d < 0 || context->row > (UINT_MAX-d)/10)
                                        context->state = CURSOR_TEXT;
                                否则
                                        context->row = context->row * 10 + d;
                        }
                        break;

                case CURSOR_COLUMN:
                        如果 (c == 'R') {
                                如果 (context->column > 0) {
                                        *ret_processed = i + 1;
                                        返回 1; /* success! */
                                }

                                context->state = CURSOR_TEXT;
                        } 否则 {
                                整数 d = undecchar(c);

                                /* As above, add the decimal character to our column number */
                                如果 (d < 0 || context->column > (UINT_MAX-d)/10)
                                        context->state = CURSOR_TEXT;
                                否则
                                        context->column = context->column * 10 + d;
                        }

                        break;
                }

                /* Reset any positions we might have picked up */
                如果 (IN_SET(context->state, CURSOR_TEXT, CURSOR_ESCAPE))
                        context->row = context->column = 0;
        }

        *ret_processed = size;
        返回 0; /* all good, but not enough data yet */
}

整数 terminal_get_cursor_position(
                整数 input_fd,
                整数 output_fd,
                unsigned *ret_row,
                unsigned *ret_column) {

        _cleanup_close_ 整数 nonblock_input_fd = -EBADF;
        整数 r;

        assert(input_fd >= 0);
        assert(output_fd >= 0);

        如果 (getenv_terminal_is_dumb())
                返回 -EOPNOTSUPP;

        r = terminal_verify_same(input_fd, output_fd);
        如果 (r < 0)
                返回 log_debug_errno(r, "Called with distinct input/output fds: %m");

        /* Failure to reset the terminal is ignored here and in similar cases below.
         * We already have our result; if cleanup fails it doesn't change the validity of the result. */
        struct termios old_termios = TERMIOS_NULL;
        CLEANUP_TERMIOS_RESET(input_fd, old_termios);

        如果 (tcgetattr(input_fd, &old_termios) < 0)
                返回 log_debug_errno(errno, "Failed to get terminal settings: %m");

        struct termios new_termios = old_termios;
        termios_disable_echo(&new_termios);

        如果 (tcsetattr(input_fd, TCSANOW, &new_termios) < 0)
                返回 log_debug_errno(errno, "Failed to set new terminal settings: %m");

        /* Request cursor position (DSR/CPR) */
        r = loop_write(output_fd, "\x1B[6n", SIZE_MAX);
        如果 (r < 0)
                返回 r;

        /* Open a 2nd input fd, in non-blocking mode, so that we won't ever hang in read() should someone
         * else process the POLLIN. */

        nonblock_input_fd = r = fd_reopen(input_fd, O_RDONLY|O_CLOEXEC|O_NONBLOCK|O_NOCTTY);
        如果 (r < 0)
                返回 r;

        usec_t end = usec_add(now(CLOCK_MONOTONIC), CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        char buf[STRLEN("\x1B[1;1R")]; /* The shortest valid reply possible */
        size_t buf_full = 0;
        CursorPositionContext context = {};

        为 (bool first = true;; first = false) {
                如果 (buf_full == 0) {
                        usec_t n = now(CLOCK_MONOTONIC);
                        如果 (n >= end)
                                返回 -EOPNOTSUPP;

                        r = fd_wait_for_event(nonblock_input_fd, POLLIN, usec_sub_unsigned(end, n));
                        如果 (r < 0)
                                返回 r;
                        如果 (r == 0)
                                返回 -EOPNOTSUPP;

                        /* On the first try, read multiple characters, i.e. the shortest valid
                         * reply. Afterwards read byte-wise, since we don't want to read too much, and
                         * unnecessarily drop too many characters from the input queue. */
                        ssize_t l = read(nonblock_input_fd, buf, first ? sizeof(buf) : 1);
                        如果 (l < 0) {
                                如果 (errno == EAGAIN)
                                        continue;

                                返回 -errno;
                        }

                        assert((size_t) l <= sizeof(buf));
                        buf_full = l;
                }

                size_t processed;
                r = scan_cursor_position_response(&context, buf, buf_full, &processed);
                如果 (r < 0)
                        返回 r;

                assert(processed <= buf_full);
                buf_full -= processed;
                memmove(buf, buf + processed, buf_full);

                如果 (r > 0) {
                        /* Superficial validity check */
                        如果 (context.row >= 32766 || context.column >= 32766)
                                返回 -ENODATA;

                        如果 (ret_row)
                                *ret_row = context.row;
                        如果 (ret_column)
                                *ret_column = context.column;

                        返回 0;
                }
        }
}

整数 terminal_reset_defensive(整数 fd, TerminalResetFlags flags) {
        整数 r = 0;

        assert(fd >= 0);
        assert(!FLAGS_SET(flags, TERMINAL_RESET_AVOID_ANSI_SEQ|TERMINAL_RESET_FORCE_ANSI_SEQ));

        /* Resets the terminal comprehensively, i.e. via both ioctl()s and via ANSI sequences, but do so only
         * if $TERM is unset or set to "dumb" */

        如果 (!isatty_safe(fd))
                返回 -ENOTTY;

        RET_GATHER(r, terminal_reset_ioctl(fd, FLAGS_SET(flags, TERMINAL_RESET_SWITCH_TO_TEXT)));

        如果 (!FLAGS_SET(flags, TERMINAL_RESET_AVOID_ANSI_SEQ) &&
            (FLAGS_SET(flags, TERMINAL_RESET_FORCE_ANSI_SEQ) || !getenv_terminal_is_dumb()))
                RET_GATHER(r, terminal_reset_ansi_seq(fd));

        返回 r;
}

整数 terminal_reset_defensive_locked(整数 fd, TerminalResetFlags flags) {
        assert(fd >= 0);

        _cleanup_close_ 整数 lock_fd = lock_dev_console();
        如果 (lock_fd < 0)
                log_debug_errno(lock_fd, "Failed to acquire lock for /dev/console, ignoring: %m");

        返回 terminal_reset_defensive(fd, flags);
}

void termios_disable_echo(struct termios *termios) {
        assert(termios);

        termios->c_lflag &= ~(ICANON|ECHO);
        termios->c_cc[VMIN] = 1;
        termios->c_cc[VTIME] = 0;
}

static bool termios_is_null(const struct termios *t) {
        如果 (!t)
                返回 true;

        返回 t->c_iflag == UINT_MAX &&
               t->c_oflag == UINT_MAX &&
               t->c_cflag == UINT_MAX &&
               t->c_lflag == UINT_MAX;
}

void termios_reset(const TermiosResetContext *c) {
        assert(c);

        PROTECT_ERRNO;

        如果 (c->fd && *c->fd >= 0 && !termios_is_null(c->termios))
                (void) tcsetattr(*c->fd, TCSANOW, c->termios);
}

typedef enum BackgroundColorState {
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

typedef struct BackgroundColorContext {
        BackgroundColorState state;
        uint32_t red, green, blue;
        unsigned red_bits, green_bits, blue_bits;
} BackgroundColorContext;

static 整数 scan_background_color_response(
                BackgroundColorContext *context,
                const char *buf,
                size_t size,
                size_t *ret_processed) {

        assert(context);
        assert(buf);
        assert(ret_processed);

        为 (size_t i = 0; i < size; i++) {
                char c = buf[i];

                switch (context->state) {

                case BACKGROUND_TEXT:
                        context->state = c == '\x1B' ? BACKGROUND_ESCAPE : BACKGROUND_TEXT;
                        break;

                case BACKGROUND_ESCAPE:
                        context->state = c == ']' ? BACKGROUND_BRACKET : BACKGROUND_TEXT;
                        break;

                case BACKGROUND_BRACKET:
                        context->state = c == '1' ? BACKGROUND_FIRST_ONE : BACKGROUND_TEXT;
                        break;

                case BACKGROUND_FIRST_ONE:
                        context->state = c == '1' ? BACKGROUND_SECOND_ONE : BACKGROUND_TEXT;
                        break;

                case BACKGROUND_SECOND_ONE:
                        context->state = c == ';' ? BACKGROUND_SEMICOLON : BACKGROUND_TEXT;
                        break;

                case BACKGROUND_SEMICOLON:
                        context->state = c == 'r' ? BACKGROUND_R : BACKGROUND_TEXT;
                        break;

                case BACKGROUND_R:
                        context->state = c == 'g' ? BACKGROUND_G : BACKGROUND_TEXT;
                        break;

                case BACKGROUND_G:
                        context->state = c == 'b' ? BACKGROUND_B : BACKGROUND_TEXT;
                        break;

                case BACKGROUND_B:
                        context->state = c == ':' ? BACKGROUND_RED : BACKGROUND_TEXT;
                        break;

                case BACKGROUND_RED:
                        如果 (c == '/')
                                context->state = context->red_bits > 0 ? BACKGROUND_GREEN : BACKGROUND_TEXT;
                        否则 {
                                整数 d = unhexchar(c);
                                如果 (d < 0 || context->red_bits >= sizeof(context->red)*8)
                                        context->state = BACKGROUND_TEXT;
                                否则 {
                                        context->red = (context->red << 4) | d;
                                        context->red_bits += 4;
                                }
                        }
                        break;

                case BACKGROUND_GREEN:
                        如果 (c == '/')
                                context->state = context->green_bits > 0 ? BACKGROUND_BLUE : BACKGROUND_TEXT;
                        否则 {
                                整数 d = unhexchar(c);
                                如果 (d < 0 || context->green_bits >= sizeof(context->green)*8)
                                        context->state = BACKGROUND_TEXT;
                                否则 {
                                        context->green = (context->green << 4) | d;
                                        context->green_bits += 4;
                                }
                        }
                        break;

                case BACKGROUND_BLUE:
                        如果 (c == '\x07') {
                                如果 (context->blue_bits > 0) {
                                        *ret_processed = i + 1;
                                        返回 1; /* success! */
                                }

                                context->state = BACKGROUND_TEXT;
                        } 否则 如果 (c == '\x1b')
                                context->state = context->blue_bits > 0 ? BACKGROUND_STRING_TERMINATOR : BACKGROUND_TEXT;
                        否则 {
                                整数 d = unhexchar(c);
                                如果 (d < 0 || context->blue_bits >= sizeof(context->blue)*8)
                                        context->state = BACKGROUND_TEXT;
                                否则 {
                                        context->blue = (context->blue << 4) | d;
                                        context->blue_bits += 4;
                                }
                        }
                        break;

                case BACKGROUND_STRING_TERMINATOR:
                        如果 (c == '\\') {
                                *ret_processed = i + 1;
                                返回 1; /* success! */
                        }

                        context->state = c == ']' ? BACKGROUND_ESCAPE : BACKGROUND_TEXT;
                        break;

                }

                /* Reset any colors we might have picked up */
                如果 (IN_SET(context->state, BACKGROUND_TEXT, BACKGROUND_ESCAPE)) {
                        /* reset color */
                        context->red = context->green = context->blue = 0;
                        context->red_bits = context->green_bits = context->blue_bits = 0;
                }
        }

        *ret_processed = size;
        返回 0; /* all good, but not enough data yet */
}

整数 get_default_background_color(double *ret_red, double *ret_green, double *ret_blue) {
        整数 r;

        assert(ret_red);
        assert(ret_green);
        assert(ret_blue);

        如果 (!colors_enabled())
                返回 -EOPNOTSUPP;

        r = terminal_verify_same(STDIN_FILENO, STDOUT_FILENO);
        如果 (r < 0)
                返回 r;

        如果 (streq_ptr(getenv("TERM"), "linux")) {
                /* Linux console is black */
                *ret_red = *ret_green = *ret_blue = 0.0;
                返回 0;
        }

        /* Open a 2nd input fd, in non-blocking mode, so that we won't ever hang in read()
         * should someone else process the POLLIN. Do all subsequent operations on the new fd. */
        _cleanup_close_ 整数 nonblock_input_fd = r = fd_reopen(STDIN_FILENO, O_RDONLY|O_CLOEXEC|O_NONBLOCK|O_NOCTTY);
        如果 (r < 0)
                返回 r;

        struct termios old_termios = TERMIOS_NULL;
        CLEANUP_TERMIOS_RESET(nonblock_input_fd, old_termios);

        如果 (tcgetattr(nonblock_input_fd, &old_termios) < 0)
                返回 -errno;

        struct termios new_termios = old_termios;
        termios_disable_echo(&new_termios);

        如果 (tcsetattr(nonblock_input_fd, TCSANOW, &new_termios) < 0)
                返回 -errno;

        r = loop_write(STDOUT_FILENO, ANSI_OSC "11;?" ANSI_ST, SIZE_MAX);
        如果 (r < 0)
                返回 r;

        usec_t end = usec_add(now(CLOCK_MONOTONIC), CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        char buf[STRLEN(ANSI_OSC "11;rgb:0/0/0" ANSI_ST)]; /* shortest possible reply */
        size_t buf_full = 0;
        BackgroundColorContext context = {};

        为 (bool first = true;; first = false) {
                如果 (buf_full == 0) {
                        usec_t n = now(CLOCK_MONOTONIC);
                        如果 (n >= end)
                                返回 -EOPNOTSUPP;

                        r = fd_wait_for_event(nonblock_input_fd, POLLIN, usec_sub_unsigned(end, n));
                        如果 (r < 0)
                                返回 r;
                        如果 (r == 0)
                                返回 -EOPNOTSUPP;

                        /* On the first try, read multiple characters, i.e. the shortest valid
                         * reply. Afterwards read byte-wise, since we don't want to read too much, and
                         * unnecessarily drop too many characters from the input queue. */
                        ssize_t l = read(nonblock_input_fd, buf, first ? sizeof(buf) : 1);
                        如果 (l < 0) {
                                如果 (errno == EAGAIN)
                                        continue;
                                返回 -errno;
                        }

                        assert((size_t) l <= sizeof(buf));
                        buf_full = l;
                }

                size_t processed;
                r = scan_background_color_response(&context, buf, buf_full, &processed);
                如果 (r < 0)
                        返回 r;

                assert(processed <= buf_full);
                buf_full -= processed;
                memmove(buf, buf + processed, buf_full);

                如果 (r > 0) {
                        assert(context.red_bits > 0);
                        *ret_red = (double) context.red / ((UINT64_C(1) << context.red_bits) - 1);
                        assert(context.green_bits > 0);
                        *ret_green = (double) context.green / ((UINT64_C(1) << context.green_bits) - 1);
                        assert(context.blue_bits > 0);
                        *ret_blue = (double) context.blue / ((UINT64_C(1) << context.blue_bits) - 1);
                        返回 0;
                }
        }
}

/* Determine terminal dimensions by means of ANSI sequences: save the cursor via DECSC, position it far
 * to the bottom right (clamped to actual terminal dimensions), read back via DSR where we ended up, and
 * restore cursor via DECRC. Only needs a single DSR round-trip, and always restores the cursor regardless
 * of whether the response is received.
 *
 * Caller must have already opened a non-blocking input fd and configured termios (echo/icanon off). */
static 整数 terminal_query_size_by_dsr(
                整数 nonblock_input_fd,
                整数 output_fd,
                unsigned *ret_rows,
                unsigned *ret_columns) {

        整数 r;

        assert(nonblock_input_fd >= 0);
        assert(output_fd >= 0);

        /* Use DECSC/DECRC to save/restore cursor instead of querying position via DSR. This way the cursor
         * is always restored — even on timeout — and we only need one DSR response instead of two. */
        r = loop_write_full(output_fd,
                            "\x1B" "7"              /* DECSC: save cursor position */
                            "\x1B[32766;32766H"     /* CUP: position cursor far to the right and to the bottom, staying within 16bit signed range */
                            "\x1B[6n"               /* DSR: request cursor position (CPR) */
                            "\x1B" "8",             /* DECRC: restore cursor position */
                            SIZE_MAX,
                            CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        如果 (r < 0)
                返回 r;

        usec_t end = usec_add(now(CLOCK_MONOTONIC), CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        char buf[STRLEN("\x1B[1;1R")]; /* The shortest valid reply possible */
        size_t buf_full = 0;
        CursorPositionContext context = {};

        为 (bool first = true;; first = false) {
                如果 (buf_full == 0) {
                        usec_t n = now(CLOCK_MONOTONIC);
                        如果 (n >= end)
                                返回 -EOPNOTSUPP;

                        r = fd_wait_for_event(nonblock_input_fd, POLLIN, usec_sub_unsigned(end, n));
                        如果 (r < 0)
                                返回 r;
                        如果 (r == 0)
                                返回 -EOPNOTSUPP;

                        /* On the first try, read multiple characters, i.e. the shortest valid
                         * reply. Afterwards read byte-wise, since we don't want to read too much, and
                         * unnecessarily drop too many characters from the input queue. */
                        ssize_t l = read(nonblock_input_fd, buf, first ? sizeof(buf) : 1);
                        如果 (l < 0) {
                                如果 (errno == EAGAIN)
                                        continue;

                                返回 -errno;
                        }

                        assert((size_t) l <= sizeof(buf));
                        buf_full = l;
                }

                size_t processed;
                r = scan_cursor_position_response(&context, buf, buf_full, &processed);
                如果 (r < 0)
                        返回 r;

                assert(processed <= buf_full);
                buf_full -= processed;
                memmove(buf, buf + processed, buf_full);

                如果 (r > 0) {
                        /* Superficial validity checks (no particular reason to check for < 4, it's
                         * just a way to look for unreasonably small values) */
                        如果 (context.row < 4 || context.column < 4 || context.row >= 32766 || context.column >= 32766)
                                返回 -ENODATA;

                        如果 (ret_rows)
                                *ret_rows = context.row;
                        如果 (ret_columns)
                                *ret_columns = context.column;

                        返回 0;
                }
        }
}

/* Common setup for ANSI terminal queries: validate the fds, open a non-blocking input fd, and configure
 * termios with echo and canonical mode disabled. Caller must restore termios and close the fd when done. */
static 整数 terminal_prepare_query(
                整数 input_fd,
                整数 output_fd,
                整数 *ret_nonblock_fd,
                struct termios *ret_saved_termios) {

        整数 r;

        assert(input_fd >= 0);
        assert(output_fd >= 0);
        assert(ret_nonblock_fd);
        assert(ret_saved_termios);

        /* Use getenv_terminal_is_dumb() instead of terminal_is_dumb() here since we operate on an
         * explicitly passed fd, not on stdio. terminal_is_dumb() additionally checks on_tty() which
         * tests whether *stderr* is a tty — that's irrelevant when we're querying a directly opened
         * terminal such as /dev/console. */
        如果 (getenv_terminal_is_dumb())
                返回 -EOPNOTSUPP;

        r = terminal_verify_same(input_fd, output_fd);
        如果 (r < 0)
                返回 log_debug_errno(r, "Called with distinct input/output fds: %m");

        /* Open a 2nd input fd, in non-blocking mode, so that we won't ever hang in read()
         * should someone else process the POLLIN. Do all subsequent operations on the new fd. */
        _cleanup_close_ 整数 nonblock_input_fd = r = fd_reopen(input_fd, O_RDONLY|O_CLOEXEC|O_NONBLOCK|O_NOCTTY);
        如果 (r < 0)
                返回 r;

        如果 (tcgetattr(nonblock_input_fd, ret_saved_termios) < 0)
                返回 log_debug_errno(errno, "Failed to get terminal settings: %m");

        struct termios new_termios = *ret_saved_termios;
        termios_disable_echo(&new_termios);

        如果 (tcsetattr(nonblock_input_fd, TCSANOW, &new_termios) < 0)
                返回 log_debug_errno(errno, "Failed to set new terminal settings: %m");

        *ret_nonblock_fd = TAKE_FD(nonblock_input_fd);
        返回 0;
}

/*
 * See https://terminalguide.namepad.de/seq/csi_st-18/,
 * https://invisible-island.net/xterm/ctlseqs/ctlseqs.html#h3-Functions-using-CSI-_-ordered-by-the-final-character_s_.
 */
#define CSI18_Q  "\x1B[18t"               /* Report the size of the text area in characters */
#define CSI18_Rp "\x1B[8;"                /* Reply prefix */
#define CSI18_R0 CSI18_Rp "1;1t"          /* Shortest reply */
#define CSI18_R1 CSI18_Rp "32766;32766t"  /* Longest reply */

static 整数 scan_text_area_size_response(
                const char *buf,
                size_t size,
                unsigned *ret_rows,
                unsigned *ret_columns) {

        assert(buf);
        assert(ret_rows);
        assert(ret_columns);

        /* Check if we have enough space for the shortest possible answer. */
        如果 (size < STRLEN(CSI18_R0))
                返回 -EAGAIN;

        /* Check if the terminating sequence is present */
        如果 (buf[size - 1] != 't')
                返回 -EAGAIN;

        unsigned short rows, columns;
        如果 (sscanf(buf, CSI18_Rp "%hu;%hut", &rows, &columns) != 2)
                返回 -EINVAL;

        *ret_rows = rows;
        *ret_columns = columns;
        返回 0;
}

/* Determine terminal dimensions by means of an ANSI CSI 18 sequence.
 *
 * Caller must have already opened a non-blocking input fd and configured termios (echo/icanon off). */
static 整数 terminal_query_size_by_csi18(
                整数 nonblock_input_fd,
                整数 output_fd,
                unsigned *ret_rows,
                unsigned *ret_columns) {

        整数 r;

        assert(nonblock_input_fd >= 0);
        assert(output_fd >= 0);

        r = loop_write_full(output_fd, CSI18_Q, SIZE_MAX, CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        如果 (r < 0)
                返回 r;

        usec_t end = usec_add(now(CLOCK_MONOTONIC), CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        char buf[STRLEN(CSI18_R1)];
        size_t bytes = 0;

        为 (;;) {
                usec_t n = now(CLOCK_MONOTONIC);
                如果 (n >= end)
                        返回 -EOPNOTSUPP;

                r = fd_wait_for_event(nonblock_input_fd, POLLIN, usec_sub_unsigned(end, n));
                如果 (r < 0)
                        返回 r;
                如果 (r == 0)
                        返回 -EOPNOTSUPP;

                /* On the first read, read multiple characters, i.e. the shortest valid reply. Afterwards
                 * read byte by byte, since we don't want to read too much and drop characters from the input
                 * queue. */
                ssize_t l = read(nonblock_input_fd, buf + bytes, bytes == 0 ? STRLEN(CSI18_R0) : 1);
                如果 (l < 0) {
                        如果 (errno == EAGAIN)
                                continue;
                        返回 -errno;
                }

                assert((size_t) l <= sizeof(buf) - bytes);
                bytes += l;

                r = scan_text_area_size_response(buf, bytes, ret_rows, ret_columns);
                如果 (r != -EAGAIN)
                        返回 r;

                如果 (bytes == sizeof(buf))
                        返回 -EOPNOTSUPP; /* The response has the right prefix, but we didn't find a valid
                                             * answer with a terminator in the allotted space. Something is
                                             * wrong, possibly some unrelated bytes got injected into the
                                             * answer. */
        }
}

整数 terminal_get_size(
                整数 input_fd,
                整数 output_fd,
                unsigned *ret_rows,
                unsigned *ret_columns,
                bool try_dsr,
                bool try_csi18) {

        _cleanup_close_ 整数 nonblock_input_fd = -EBADF;
        struct termios old_termios = TERMIOS_NULL;
        CLEANUP_TERMIOS_RESET(nonblock_input_fd, old_termios);
        _cleanup_(nonblock_resetp) 整数 nonblock_reset = -EBADF;
        整数 r;

        assert(try_dsr || try_csi18);

        r = terminal_prepare_query(input_fd, output_fd, &nonblock_input_fd, &old_termios);
        如果 (r < 0)
                返回 r;

        /* Put the output fd in non-blocking mode with a write timeout, to avoid blocking indefinitely on
         * write if the terminal is not consuming data (e.g. serial console with flow control). */
        r = fd_nonblock(output_fd, true);
        如果 (r < 0)
                返回 log_debug_errno(r, "Failed to set terminal to non-blocking mode: %m");
        如果 (r > 0)
                nonblock_reset = output_fd;

        /* Flush any stale input that might confuse the response parsers. */
        (void) tcflush(nonblock_input_fd, TCIFLUSH);

        如果 (try_csi18) {
                r = terminal_query_size_by_csi18(nonblock_input_fd, output_fd, ret_rows, ret_columns);
                如果 (r >= 0)
                        返回 r;

                /* Query failed. Flush any outstanding input. */
                (void) tcflush(nonblock_input_fd, TCIFLUSH);

                如果 (!IN_SET(r, -EOPNOTSUPP, -EINVAL))
                        返回 r;
        }

        如果 (try_dsr) {
                r = terminal_query_size_by_dsr(nonblock_input_fd, output_fd, ret_rows, ret_columns);
                如果 (r >= 0)
                        返回 r;

                /* Query failed. Flush any outstanding input. */
                (void) tcflush(nonblock_input_fd, TCIFLUSH);
        }

        返回 r;
}

static 整数 fd_is_vc(整数 fd) {
        _cleanup_free_ char *name = NULL;
        整数 r;

        assert(fd >= 0);

        r = getttyname_harder(fd, &name);
        如果 (r < 0)
                返回 r;

        返回 tty_is_vc_resolve(name);
}

整数 terminal_fix_size(整数 input_fd, 整数 output_fd) {
        unsigned rows, columns;
        整数 r;

        /* Don't ask a virtual console at all. There is no intermediary here that could be wrong about the
         * dimensions: the kernel renders the screen itself, so its winsize is authoritative. */
        r = fd_is_vc(input_fd);
        如果 (r < 0)
                log_debug_errno(r, "Failed to determine whether terminal is a virtual console, assuming it is not: %m");
        否则 如果 (r > 0) {
                log_debug("Not querying dimensions of a virtual console, kernel dimensions are authoritative.");
                返回 0;
        }

        struct winsize ws = {};
        如果 (ioctl(output_fd, TIOCGWINSZ, &ws) < 0)
                返回 log_debug_errno(errno, "Failed to query terminal dimensions, ignoring: %m");

        r = terminal_get_size(input_fd, output_fd, &rows, &columns, /* try_dsr= */ true, /* try_csi18= */ true);
        如果 (r < 0)
                返回 log_debug_errno(r, "Failed to acquire terminal dimensions via ANSI sequences, not adjusting terminal dimensions: %m");

        如果 (ws.ws_row == rows && ws.ws_col == columns) {
                log_debug("Terminal dimensions reported via ANSI sequences match currently set terminal dimensions, not changing.");
                返回 0;
        }

        ws.ws_col = columns;
        ws.ws_row = rows;

        如果 (ioctl(output_fd, TIOCSWINSZ, &ws) < 0)
                返回 log_debug_errno(errno, "Failed to update terminal dimensions, ignoring: %m");

        log_debug("Fixed terminal dimensions to %ux%u based on ANSI sequence information.", columns, rows);
        返回 1;
}

#define MAX_TERMINFO_LENGTH 64
/* python -c 'print("".join(hex(ord(i))[2:] for i in "name").upper())' */
#define DCS_TERMINFO_Q ANSI_DCS "+q" "6E616D65" ANSI_ST
/* The answer is either 0+r… (invalid) or 1+r… (OK). */
#define DCS_TERMINFO_R0 ANSI_DCS "0+r" ANSI_ST
#define DCS_TERMINFO_R1 ANSI_DCS "1+r" "6E616D65" "=" /* This is followed by Pt ST. */
assert_cc(STRLEN(DCS_TERMINFO_R0) <= STRLEN(DCS_TERMINFO_R1 ANSI_ST));

static 整数 scan_terminfo_response(
                const char *buf,
                size_t size,
                char **ret_name) {
        整数 r;

        assert(buf);
        assert(ret_name);

        /* Check if we have enough space for the shortest possible answer. */
        如果 (size < STRLEN(DCS_TERMINFO_R0))
                返回 -EAGAIN;

        /* Check if the terminating sequence is present */
        如果 (memcmp(buf + size - STRLEN(ANSI_ST), ANSI_ST, STRLEN(ANSI_ST)) != 0)
                返回 -EAGAIN;

        如果 (size <= STRLEN(DCS_TERMINFO_R1 ANSI_ST))
                返回 -EINVAL;  /* The answer is invalid or empty */

        如果 (memcmp(buf, DCS_TERMINFO_R1, STRLEN(DCS_TERMINFO_R1)) != 0)
                返回 -EINVAL;  /* The answer is not valid */

        _cleanup_free_ void *dec = NULL;
        size_t dec_size;
        r = unhexmem_full(buf + STRLEN(DCS_TERMINFO_R1), size - STRLEN(DCS_TERMINFO_R1 ANSI_ST),
                          /* secure= */ false,
                          &dec, &dec_size);
        如果 (r < 0)
                返回 r;

        assert(((const char *) dec)[dec_size] == '\0'); /* unhexmem appends NUL for our convenience */
        如果 (memchr(dec, '\0', dec_size) || string_has_cc(dec, NULL) || !filename_is_valid(dec))
                返回 -EUCLEAN;

        *ret_name = TAKE_PTR(dec);
        返回 0;
}

整数 terminal_get_terminfo_by_dcs(整数 fd, char **ret_name) {
        整数 r;

        assert(fd >= 0);
        assert(ret_name);

        /* Note: fd must be in non-blocking read-write mode! */

        struct termios old_termios = TERMIOS_NULL;
        CLEANUP_TERMIOS_RESET(fd, old_termios);

        如果 (tcgetattr(fd, &old_termios) < 0)
                返回 -errno;

        struct termios new_termios = old_termios;
        termios_disable_echo(&new_termios);

        如果 (tcsetattr(fd, TCSANOW, &new_termios) < 0)
                返回 -errno;

        r = loop_write(fd, DCS_TERMINFO_Q, SIZE_MAX);
        如果 (r < 0)
                返回 r;

        usec_t end = usec_add(now(CLOCK_MONOTONIC), CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        char buf[STRLEN(DCS_TERMINFO_R1) + MAX_TERMINFO_LENGTH + STRLEN(ANSI_ST)];
        size_t bytes = 0;

        为 (;;) {
                usec_t n = now(CLOCK_MONOTONIC);
                如果 (n >= end)
                        返回 -EOPNOTSUPP;

                r = fd_wait_for_event(fd, POLLIN, usec_sub_unsigned(end, n));
                如果 (r < 0)
                        返回 r;
                如果 (r == 0)
                        返回 -EOPNOTSUPP;

                /* On the first read, read multiple characters, i.e. the shortest valid reply. Afterwards
                 * read byte by byte, since we don't want to read too much and drop characters from the input
                 * queue. */
                ssize_t l = read(fd, buf + bytes, bytes == 0 ? STRLEN(DCS_TERMINFO_R0) : 1);
                如果 (l < 0) {
                        如果 (errno == EAGAIN)
                                continue;
                        返回 -errno;
                }

                assert((size_t) l <= sizeof(buf) - bytes);
                bytes += l;

                r = scan_terminfo_response(buf, bytes, ret_name);
                如果 (r != -EAGAIN)
                        返回 r;

                如果 (bytes == sizeof(buf))
                        返回 -EOPNOTSUPP; /* The response has the right prefix, but we didn't find a valid
                                             * answer with a terminator in the allotted space. Something is
                                             * wrong, possibly some unrelated bytes got injected into the
                                             * answer. */
        }
}

整数 have_terminfo_file(const char *name) {
        /* This is a heuristic check if we have the file, using the directory layout used on
         * current Linux systems. Checks for other layouts can be added later if appropriate. */
        整数 r;

        assert(filename_is_valid(name));

        _cleanup_free_ char *p = path_join("/usr/share/terminfo", CHAR_TO_STR(name[0]), name);
        如果 (!p)
                返回 log_oom_debug();

        r = RET_NERRNO(access(p, F_OK));
        如果 (r == -ENOENT)
                返回 false;
        如果 (r < 0)
                返回 r;
        返回 true;
}

整数 query_term_for_tty(const char *tty, char **ret_term) {
        _cleanup_free_ char *dcs_term = NULL;
        整数 r;

        assert(tty);
        assert(ret_term);

        如果 (tty_is_vc_resolve(tty))
                返回 strdup_to(ret_term, "linux");

        /* Try to query the terminal implementation that we're on. This will not work in all
         * cases, which is fine, since this is intended to be used as a fallback. */

        _cleanup_close_ 整数 tty_fd = open_terminal(tty, O_RDWR|O_NOCTTY|O_CLOEXEC|O_NONBLOCK);
        如果 (tty_fd < 0)
                返回 log_debug_errno(tty_fd, "Failed to open %s to query terminfo: %m", tty);

        r = terminal_get_terminfo_by_dcs(tty_fd, &dcs_term);
        如果 (r < 0)
                返回 log_debug_errno(r, "Failed to query %s for terminfo: %m", tty);

        r = have_terminfo_file(dcs_term);
        如果 (r < 0)
                返回 log_debug_errno(r, "Failed to look for terminfo %s: %m", dcs_term);
        如果 (r == 0)
                返回 log_info_errno(SYNTHETIC_ERRNO(ENODATA),
                                      "Terminfo %s not found for %s.", dcs_term, tty);

        *ret_term = TAKE_PTR(dcs_term);
        返回 0;
}

整数 terminal_is_pty_fd(整数 fd) {
        整数 r;

        assert(fd >= 0);

        /* Returns true if we are looking at a pty, i.e. if it's backed by the /dev/pts/ file system */

        如果 (!isatty_safe(fd))
                返回 false;

        r = is_fs_type_at(fd, NULL, DEVPTS_SUPER_MAGIC);
        如果 (r != 0)
                返回 r;

        /* The ptmx device is weird, it exists twice, once inside and once outside devpts. To detect the
         * latter case, let's fire off an ioctl() that only works on ptmx devices. */

        整数 v;
        如果 (ioctl(fd, TIOCGPKT, &v) < 0) {
                如果 (ERRNO_IS_NOT_SUPPORTED(errno))
                        返回 false;

                返回 -errno;
        }

        返回 true;
}

整数 pty_open_peer(整数 fd, 整数 mode) {
        assert(fd >= 0);

        /* Opens the peer PTY using the new race-free TIOCGPTPEER ioctl() (kernel 4.13).
         *
         * This is safe to be called on TTYs from other namespaces. */

        assert((mode & (O_CREAT|O_PATH|O_DIRECTORY|O_TMPFILE)) == 0);

        /* This replicates the EIO retry logic of open_terminal() in a modified way. */
        为 (unsigned c = 0;; c++) {
                整数 peer_fd = ioctl(fd, TIOCGPTPEER, mode);
                如果 (peer_fd >= 0)
                        返回 peer_fd;

                如果 (errno != EIO)
                        返回 -errno;

                /* Max 1s in total */
                如果 (c >= 20)
                        返回 -EIO;

                (void) usleep_safe(50 * USEC_PER_MSEC);
        }
}
