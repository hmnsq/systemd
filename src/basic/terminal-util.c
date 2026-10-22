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

static volatile целое cached_on_tty = -1;
static volatile целое cached_on_dev_null = -1;

bool isatty_safe(целое fd) {
        assert(fd >= 0);

        если (isatty(fd))
                вернуть true;

        /* Linux/glibc returns EIO for hung up TTY on isatty(). Which is wrong, the thing doesn't stop being
         * a TTY after all, just because it is temporarily hung up. Let's work around this here, until this
         * is fixed in glibc. See: https://sourceware.org/bugzilla/show_bug.cgi?id=32103 */
        если (errno == EIO)
                вернуть true;

        /* Be resilient if we're working on stdio, since they're set up by parent process. */
        assert(errno != EBADF || IN_SET(fd, STDIN_FILENO, STDOUT_FILENO, STDERR_FILENO));

        вернуть false;
}

целое chvt(целое vt) {
        _cleanup_close_ целое fd = -EBADF;

        /* Switch to the specified vt number. If the VT is specified <= 0 switch to the VT the kernel log messages go,
         * if that's configured. */

        fd = open_terminal("/dev/tty0", O_RDWR|O_NOCTTY|O_CLOEXEC|O_NONBLOCK);
        если (fd < 0)
                вернуть fd;

        если (vt <= 0) {
                целое tiocl[2] = {
                        TIOCL_GETKMSGREDIRECT,
                        0
                };

                если (ioctl(fd, TIOCLINUX, tiocl) < 0)
                        вернуть -errno;

                vt = tiocl[0] <= 0 ? 1 : tiocl[0];
        }

        вернуть RET_NERRNO(ioctl(fd, VT_ACTIVATE, vt));
}

целое read_one_char(FILE *f, char *ret, usec_t t, bool echo, bool *need_nl) {
        _cleanup_free_ char *line = NULL;
        struct termios old_termios;
        целое r, fd;

        assert(ret);

        если (!f)
                f = stdin;

        /* If this is a terminal, then switch canonical mode off, so that we can read a single
         * character. (Note that fmemopen() streams do not have an fd associated with them, let's handle that
         * nicely.) If 'echo' is false we'll also disable ECHO mode so that the pressed key is not made
         * visible to the user. */
        fd = fileno(f);
        если (fd >= 0 && tcgetattr(fd, &old_termios) >= 0) {
                struct termios new_termios = old_termios;

                new_termios.c_lflag &= ~(ICANON|(echo ? 0 : ECHO));
                new_termios.c_cc[VMIN] = 1;
                new_termios.c_cc[VTIME] = 0;

                если (tcsetattr(fd, TCSANOW, &new_termios) >= 0) {
                        char c;

                        если (t != USEC_INFINITY) {
                                если (fd_wait_for_event(fd, POLLIN, t) <= 0) {
                                        (void) tcsetattr(fd, TCSANOW, &old_termios);
                                        вернуть -ETIMEDOUT;
                                }
                        }

                        r = safe_fgetc(f, &c);
                        (void) tcsetattr(fd, TCSANOW, &old_termios);
                        если (r < 0)
                                вернуть r;
                        если (r == 0)
                                вернуть -EIO;

                        если (need_nl)
                                *need_nl = c != '\n';

                        *ret = c;
                        вернуть 0;
                }
        }

        если (t != USEC_INFINITY && fd >= 0) {
                /* Let's wait the specified amount of time for input. When we have no fd we skip this, under
                 * the assumption that this is an fmemopen() stream or so where waiting doesn't make sense
                 * anyway, as the data is either already in the stream or cannot possible be placed there
                 * while we access the stream */

                если (fd_wait_for_event(fd, POLLIN, t) <= 0)
                        вернуть -ETIMEDOUT;
        }

        /* If this is not a terminal, then read a full line instead */

        r = read_line(f, 16, &line); /* longer than necessary, to eat up UTF-8 chars/vt100 key sequences */
        если (r < 0)
                вернуть r;
        если (r == 0)
                вернуть -EIO;

        если (strlen(line) != 1)
                вернуть -EBADMSG;

        если (need_nl)
                *need_nl = false;

        *ret = line[0];
        вернуть 0;
}

#define DEFAULT_ASK_REFRESH_USEC (2*USEC_PER_SEC)

целое ask_char(char *ret, const char *replies, const char *fmt, ...) {
        целое r;

        assert(ret);
        assert(replies);
        assert(fmt);

        для (;;) {
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
                если (r < 0) {

                        если (r == -ETIMEDOUT)
                                continue;

                        если (r == -EBADMSG) {
                                puts("Bad input, please try again.");
                                continue;
                        }

                        putchar('\n');
                        вернуть r;
                }

                если (need_nl)
                        putchar('\n');

                если (strchr(replies, c)) {
                        *ret = c;
                        вернуть 0;
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
                если (!startswith(*c, string))
                        continue;

                /* Store first completion that matches */
                если (!found) {
                        found = strdup(*c);
                        если (!found)
                                вернуть -ENOMEM;

                        continue;
                }

                /* If there's another completion that works truncate the one we already found by common
                 * prefix */
                size_t n = str_common_prefix(found, *c);
                если (n == SIZE_MAX)
                        continue;

                found[n] = 0;
                partial = true;
        }

        *ret = TAKE_PTR(found);

        если (!*ret)
                вернуть COMPLETION_NONE;
        если (partial)
                вернуть COMPLETION_PARTIAL;

        вернуть streq(string, *ret) ? COMPLETION_ALREADY : COMPLETION_FULL;
}

static void clear_by_backspace(size_t n) {
        /* Erase the specified number of character cells backwards on the terminal */
        для (size_t i = 0; i < n; i++)
                fputs("\b \b", stdout);
}

целое ask_string_full(
                char **ret,
                const char *prefill,
                GetCompletionsCallback get_completions,
                void *userdata,
                const char *text, ...) {

        va_list ap;
        целое r;

        assert(ret);
        assert(text);

        _cleanup_free_ char *string = NULL;
        size_t n = 0;

        если (prefill) {
                /* Prefill query with explicit data if specified */

                string = strdup(prefill);
                если (!string)
                        вернуть -ENOMEM;

                n = strlen(string);

        } иначе если (get_completions) {
                /* Otherwise, figure out what string to preselect the query with */
                _cleanup_strv_free_ char **completions = NULL;
                r = get_completions("", GET_COMPLETIONS_PRESELECT, &completions, userdata);
                если (r < 0)
                        вернуть r;

                CompletionResult cr = pick_completion(string, completions, &string);
                если (cr < 0)
                        вернуть cr;

                n = strlen_ptr(string);
        }

        /* Output the prompt */
        fputs(ansi_highlight(), stdout);
        va_start(ap, text);
        vprintf(text, ap);
        va_end(ap);
        fputs(ansi_normal(), stdout);
        если (string)
                fputs(string, stdout);
        fflush(stdout);

        /* Do interactive logic only if stdin + stdout are connected to the same place. And yes, we could use
         * STDIN_FILENO and STDOUT_FILENO here, but let's be overly correct for once, after all libc allows
         * swapping out stdin/stdout. */
        целое fd_input = fileno(stdin);
        целое fd_output = fileno(stdout);
        struct termios old_termios = TERMIOS_NULL;
        CLEANUP_TERMIOS_RESET(fd_input, old_termios);

        если (fd_input < 0 || fd_output < 0 || same_fd(fd_input, fd_output) <= 0)
                goto fallback;

        /* Try to disable echo, which also tells us if this even is a terminal */
        если (tcgetattr(fd_input, &old_termios) < 0) {
                old_termios = TERMIOS_NULL;
                goto fallback;
        }

        struct termios new_termios = old_termios;
        termios_disable_echo(&new_termios);
        если (tcsetattr(fd_input, TCSANOW, &new_termios) < 0)
                вернуть -errno;

        для (;;) {
                целое c = fgetc(stdin);

                /* On EOF or NUL, end the request, don't output anything anymore */
                если (IN_SET(c, EOF, 0))
                        break;

                /* On Return also end the request, but make this visible */
                если (IN_SET(c, '\n', '\r')) {
                        fputc('\n', stdout);
                        break;
                }

                если (c == '\t') {
                        /* Tab */

                        _cleanup_strv_free_ char **completions = NULL;
                        если (get_completions) {
                                r = get_completions(string, /* flags= */ 0, &completions, userdata);
                                если (r < 0)
                                        вернуть r;
                        }

                        _cleanup_free_ char *new_string = NULL;
                        CompletionResult cr = pick_completion(string, completions, &new_string);
                        если (cr < 0)
                                вернуть cr;
                        если (IN_SET(cr, COMPLETION_PARTIAL, COMPLETION_FULL)) {
                                /* Output the new suffix we learned */
                                fputs(ASSERT_PTR(startswith(new_string, strempty(string))), stdout);

                                /* And update the whole string */
                                free_and_replace(string, new_string);
                                n = strlen(string);
                        }
                        если (cr == COMPLETION_NONE)
                                fputc('\a', stdout); /* BEL */

                        если (IN_SET(cr, COMPLETION_PARTIAL, COMPLETION_ALREADY)) {
                                /* If this worked only partially, or if the user hit TAB even though we were
                                 * complete already, then show the remaining options (in the latter case just
                                 * the one). */
                                fputc('\n', stdout);

                                _cleanup_strv_free_ char **filtered = strv_filter_prefix(completions, string);
                                если (!filtered)
                                        вернуть -ENOMEM;

                                r = show_menu(filtered,
                                              /* n_columns= */ SIZE_MAX,
                                              /* column_width= */ SIZE_MAX,
                                              /* ellipsize_percentage= */ 0,
                                              /* grey_prefix= */ string,
                                              /* with_numbers= */ false);
                                если (r < 0)
                                        вернуть r;

                                /* Show the prompt again */
                                fputs(ansi_highlight(), stdout);
                                va_start(ap, text);
                                vprintf(text, ap);
                                va_end(ap);
                                fputs(ansi_normal(), stdout);
                                fputs(string, stdout);
                        }

                } иначе если (IN_SET(c, '\b', 127)) {
                        /* Backspace */

                        если (n == 0)
                                fputc('\a', stdout); /* BEL */
                        иначе {
                                size_t m = utf8_last_length(string, n);

                                char *e = string + n - m;
                                clear_by_backspace(utf8_console_width(e));

                                *e = 0;
                                n -= m;
                        }

                } иначе если (c == 21) {
                        /* Ctrl-u → erase all input */

                        clear_by_backspace(utf8_console_width(string));
                        если (string)
                                string[n = 0] = 0;
                        иначе
                                assert(n == 0);

                } иначе если (c == 4) {
                        /* Ctrl-d → cancel this field input */

                        вернуть -ECANCELED;

                } иначе если (char_is_cc(c) || n >= LINE_MAX)
                        /* refuse control characters and too long strings */
                        fputc('\a', stdout); /* BEL */
                иначе {
                        /* Regular char */

                        если (!GREEDY_REALLOC(string, n+2))
                                вернуть -ENOMEM;

                        string[n++] = (char) c;
                        string[n] = 0;

                        fputc(c, stdout);
                }

                fflush(stdout);
        }

        если (!string) {
                string = strdup("");
                если (!string)
                        вернуть -ENOMEM;
        }

        *ret = TAKE_PTR(string);
        вернуть 0;

fallback:
        /* A simple fallback without TTY magic */
        string = mfree(string);
        r = read_line(stdin, LONG_LINE_MAX, &string);
        если (r < 0)
                вернуть r;
        если (r == 0)
                вернуть -EIO;

        *ret = TAKE_PTR(string);
        вернуть 0;
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

        вернуть key != 'q';
}

static size_t widest_list_element(char *const*l) {
        size_t w = 0;

        /* Returns the largest console width of all elements in 'l' */

        STRV_FOREACH(i, l)
                w = MAX(w, utf8_console_width(*i));

        вернуть w;
}

целое show_menu(char **x,
              size_t n_columns,
              size_t column_width,
              unsigned ellipsize_percentage,
              const char *grey_prefix,
              bool with_numbers) {

        assert(n_columns > 0);
        assert(column_width > 0);

        если (n_columns == SIZE_MAX)
                n_columns = 3;

        если (column_width == SIZE_MAX) {
                size_t widest = widest_list_element(x);

                /* If not specified, derive column width from screen width */
                size_t column_max = (columns()-1) / n_columns;

                /* Subtract room for numbers */
                если (with_numbers)
                        column_max = LESS_BY(column_max, 6U);

                /* If columns would get too tight let's make this a linear list instead. */
                если (column_max < 10 && widest > 10) {
                        n_columns = 1;
                        column_max = columns()-1;

                        если (with_numbers)
                                column_max = LESS_BY(column_max, 6U);
                }

                /* Never make this narrower than 10 characters */
                column_max = MAX(column_max, 10U);
                column_width = CLAMP(widest+1, 10U, column_max);
        }

        size_t n = strv_length(x);
        size_t per_column = DIV_ROUND_UP(n, n_columns);

        size_t break_lines = lines();
        если (break_lines > 2)
                break_lines--;

        /* The first page gets two extra lines, since we want to show a title */
        size_t break_modulo = break_lines;
        если (break_modulo > 3)
                break_modulo -= 3;

        для (size_t i = 0; i < per_column; i++) {

                для (size_t j = 0; j < n_columns; j++) {
                        _cleanup_free_ char *e = NULL;

                        size_t p = j * per_column + i;
                        если (p >= n)
                                break;

                        e = ellipsize(x[p], column_width, ellipsize_percentage);
                        если (!e)
                                вернуть -ENOMEM;

                        если (with_numbers)
                                printf("%s%4zu)%s ",
                                       ansi_grey(),
                                       p + 1,
                                       ansi_normal());

                        если (grey_prefix && startswith(e, grey_prefix)) {
                                size_t k = MIN(strlen(grey_prefix), column_width);
                                printf("%s%.*s%s",
                                       ansi_grey(),
                                       (целое) k, e,
                                       ansi_normal());
                                printf("%-*s",
                                       (целое) (column_width - k), e+k);
                        } иначе
                                printf("%-*s", (целое) column_width, e);
                }

                putchar('\n');

                /* on the first screen we reserve 2 extra lines for the title */
                если (i % break_lines == break_modulo)
                        если (!any_key_to_proceed())
                                вернуть 0;
        }

        вернуть 0;
}

целое open_terminal(const char *name, целое mode) {
        _cleanup_close_ целое fd = -EBADF;

        /*
         * If a TTY is in the process of being closed opening it might cause EIO. This is horribly awful, but
         * unlikely to be changed in the kernel. Hence we work around this problem by retrying a couple of
         * times.
         *
         * https://bugs.launchpad.net/ubuntu/+source/linux/+bug/554172/comments/245
         */

        assert((mode & (O_CREAT|O_PATH|O_DIRECTORY|O_TMPFILE)) == 0);

        для (unsigned c = 0;; c++) {
                fd = open(name, mode, 0);
                если (fd >= 0)
                        break;

                если (errno != EIO)
                        вернуть -errno;

                /* Max 1s in total */
                если (c >= 20)
                        вернуть -EIO;

                (void) usleep_safe(50 * USEC_PER_MSEC);
        }

        если (!isatty_safe(fd))
                вернуть -ENOTTY;

        вернуть TAKE_FD(fd);
}

целое acquire_terminal(
                const char *name,
                AcquireTerminalFlags flags,
                usec_t timeout) {

        _cleanup_close_ целое notify = -EBADF, fd = -EBADF;
        usec_t ts = USEC_INFINITY;
        целое r, wd = -1;

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

        если (mode == ACQUIRE_TERMINAL_WAIT) {
                notify = inotify_init1(IN_CLOEXEC | IN_NONBLOCK);
                если (notify < 0)
                        вернуть -errno;

                wd = inotify_add_watch(notify, name, IN_CLOSE);
                если (wd < 0)
                        вернуть -errno;

                если (timeout != USEC_INFINITY)
                        ts = now(CLOCK_MONOTONIC);
        }

        /* If we are called with ACQUIRE_TERMINAL_WATCH_SIGTERM we'll unblock SIGTERM during ppoll() temporarily */
        sigset_t poll_ss;
        assert_se(sigprocmask(SIG_SETMASK, /* newset= */ NULL, &poll_ss) >= 0);
        если (flags & ACQUIRE_TERMINAL_WATCH_SIGTERM) {
                assert_se(sigismember(&poll_ss, SIGTERM) > 0);
                assert_se(sigdelset(&poll_ss, SIGTERM) >= 0);
        }

        для (;;) {
                если (notify >= 0) {
                        r = flush_fd(notify);
                        если (r < 0)
                                вернуть r;
                }

                /* We pass here O_NOCTTY only so that we can check the return value TIOCSCTTY and have a reliable way
                 * to figure out if we successfully became the controlling process of the tty */
                fd = open_terminal(name, O_RDWR|O_NOCTTY|O_CLOEXEC);
                если (fd < 0)
                        вернуть fd;

                /* Temporarily ignore SIGHUP, so that we don't get SIGHUP'ed if we already own the tty. */
                struct sigaction sa_old;
                assert_se(sigaction(SIGHUP, &sigaction_ignore, &sa_old) >= 0);

                /* First, try to get the tty */
                r = RET_NERRNO(ioctl(fd, TIOCSCTTY, mode == ACQUIRE_TERMINAL_FORCE));

                /* Reset signal handler to old value */
                assert_se(sigaction(SIGHUP, &sa_old, NULL) >= 0);

                /* Success? Exit the loop now! */
                если (r >= 0)
                        break;

                /* Any failure besides -EPERM? Fail, regardless of the mode. */
                если (r != -EPERM)
                        вернуть r;

                если (flags & ACQUIRE_TERMINAL_PERMISSIVE) /* If we are in permissive mode, then EPERM is fine, turn this
                                                          * into a success. Note that EPERM is also returned if we
                                                          * already are the owner of the TTY. */
                        break;

                если (mode != ACQUIRE_TERMINAL_WAIT) /* If we are in TRY or FORCE mode, then propagate EPERM as EPERM */
                        вернуть r;

                assert(notify >= 0);
                assert(wd >= 0);

                для (;;) {
                        usec_t left;
                        если (timeout == USEC_INFINITY)
                                left = USEC_INFINITY;
                        иначе {
                                assert(ts != USEC_INFINITY);

                                usec_t n = usec_sub_unsigned(now(CLOCK_MONOTONIC), ts);
                                если (n >= timeout)
                                        вернуть -ETIMEDOUT;

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
                        если (r < 0)
                                вернуть r;
                        если (r == 0)
                                вернуть -ETIMEDOUT;

                        union inotify_event_buffer buffer;
                        ssize_t l;
                        l = read(notify, &buffer, sizeof(buffer));
                        если (l < 0) {
                                если (ERRNO_IS_TRANSIENT(errno))
                                        continue;

                                вернуть -errno;
                        }

                        FOREACH_INOTIFY_EVENT(e, buffer, l) {
                                если (e->mask & IN_Q_OVERFLOW) /* If we hit an inotify queue overflow, simply check if the terminal is up for grabs now. */
                                        break;

                                если (e->wd != wd || !(e->mask & IN_CLOSE)) /* Safety checks */
                                        вернуть -EIO;
                        }

                        break;
                }

                /* We close the tty fd here since if the old session ended our handle will be dead. It's important that
                 * we do this after sleeping, so that we don't enter an endless loop. */
                fd = safe_close(fd);
        }

        вернуть TAKE_FD(fd);
}

целое release_terminal(void) {
        _cleanup_close_ целое fd = -EBADF;
        целое r;

        fd = open("/dev/tty", O_RDWR|O_NOCTTY|O_CLOEXEC|O_NONBLOCK);
        если (fd < 0)
                вернуть -errno;

        /* Temporarily ignore SIGHUP, so that we don't get SIGHUP'ed
         * by our own TIOCNOTTY */
        struct sigaction sa_old;
        assert_se(sigaction(SIGHUP, &sigaction_ignore, &sa_old) >= 0);

        r = RET_NERRNO(ioctl(fd, TIOCNOTTY));

        assert_se(sigaction(SIGHUP, &sa_old, NULL) >= 0);

        вернуть r;
}

целое terminal_new_session(void) {

        /* Make us the new session leader, and set stdin tty to be our controlling terminal.
         *
         * Why stdin? Well, the ctty logic is relevant for signal delivery mostly, i.e. if people hit C-c
         * or the line is hung up. Such events are basically just a form of input, via a side channel
         * (that side channel being signal delivery, i.e. SIGINT, SIGHUP et al). Hence we focus on input,
         * not output here. */

        если (!isatty_safe(STDIN_FILENO))
                вернуть -ENXIO;

        (void) setsid();
        вернуть RET_NERRNO(ioctl(STDIN_FILENO, TIOCSCTTY, 0));
}

void terminal_detach_session(void) {
        (void) setsid();
        (void) release_terminal();
}

целое terminal_vhangup_fd(целое fd) {
        assert(fd >= 0);
        вернуть RET_NERRNO(ioctl(fd, TIOCVHANGUP));
}

целое terminal_vhangup(const char *tty) {
        _cleanup_close_ целое fd = -EBADF;

        assert(tty);

        fd = open_terminal(tty, O_RDWR|O_NOCTTY|O_CLOEXEC);
        если (fd < 0)
                вернуть fd;

        вернуть terminal_vhangup_fd(fd);
}

целое vt_disallocate(const char *tty_path) {
        assert(tty_path);

        /* Deallocate the VT if possible. If not possible (i.e. because it is the active one), at least clear
         * it entirely (including the scrollback buffer). */

        целое ttynr = vtnr_from_tty(tty_path);
        если (ttynr > 0) {
                _cleanup_close_ целое fd = open_terminal("/dev/tty0", O_RDWR|O_NOCTTY|O_CLOEXEC|O_NONBLOCK);
                если (fd < 0)
                        вернуть fd;

                /* Try to deallocate */
                если (ioctl(fd, VT_DISALLOCATE, ttynr) >= 0)
                        вернуть 0;
                если (errno != EBUSY)
                        вернуть -errno;
        }

        /* So this is not a VT (in which case we cannot deallocate it), or we failed to deallocate. Let's at
         * least clear the screen. */

        _cleanup_close_ целое fd2 = open_terminal(tty_path, O_WRONLY|O_NOCTTY|O_CLOEXEC|O_NONBLOCK);
        если (fd2 < 0)
                вернуть fd2;

        вернуть loop_write_full(fd2,
                               "\033[r"   /* clear scrolling region */
                               "\033[H"   /* move home */
                               "\033[3J"  /* clear screen including scrollback, requires Linux 2.6.40 */
                               "\033c",   /* reset to initial state */
                               SIZE_MAX,
                               CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
}

static целое vt_default_utf8(void) {
        /* Read the default VT UTF8 setting from the kernel */
        вернуть read_boolean_file("/sys/module/vt/parameters/default_utf8");
}

static целое vt_reset_keyboard(целое fd) {
        целое r, kb;

        assert(fd >= 0);

        /* If we can't read the default, then default to Unicode. It's 2024 after all. */
        r = vt_default_utf8();
        если (r < 0)
                log_debug_errno(r, "Failed to determine kernel VT UTF-8 mode, assuming enabled: %m");

        kb = vt_default_utf8() != 0 ? K_UNICODE : K_XLATE;
        вернуть RET_NERRNO(ioctl(fd, KDSKBMODE, kb));
}

static целое terminal_reset_ioctl(целое fd, bool switch_to_text) {
        struct termios termios;
        целое r;

        /* Set terminal to some sane defaults */

        assert(fd >= 0);

        /* We leave locked terminal attributes untouched, so that Plymouth may set whatever it wants to set,
         * and we don't interfere with that. */

        /* Disable exclusive mode, just in case */
        если (ioctl(fd, TIOCNXCL) < 0)
                log_debug_errno(errno, "TIOCNXCL ioctl failed on TTY, ignoring: %m");

        /* Switch to text mode */
        если (switch_to_text)
                если (ioctl(fd, KDSETMODE, KD_TEXT) < 0)
                        log_debug_errno(errno, "KDSETMODE ioctl for switching to text mode failed on TTY, ignoring: %m");

        /* Set default keyboard mode */
        r = vt_reset_keyboard(fd);
        если (r < 0)
                log_debug_errno(r, "Failed to reset VT keyboard, ignoring: %m");

        если (tcgetattr(fd, &termios) < 0) {
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
        если (r < 0)
                log_debug_errno(r, "Failed to set terminal parameters: %m");

finish:
        /* Just in case, flush all crap out */
        (void) tcflush(fd, TCIOFLUSH);

        вернуть r;
}

целое terminal_reset_ansi_seq(целое fd) {
        _cleanup_(nonblock_resetp) целое nonblock_reset = -EBADF;
        целое r;

        assert(fd >= 0);

        если (getenv_terminal_is_dumb())
                вернуть 0;

        r = fd_nonblock(fd, true);
        если (r < 0)
                вернуть log_debug_errno(r, "Failed to set terminal to non-blocking mode: %m");
        если (r > 0)
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
        если (r < 0)
                log_debug_errno(r, "Failed to reset terminal through ANSI sequences: %m");

        вернуть r;
}

void reset_dev_console_fd(целое fd, bool switch_to_text) {
        целое r;

        assert(fd >= 0);

        _cleanup_close_ целое lock_fd = lock_dev_console();
        если (lock_fd < 0)
                log_debug_errno(lock_fd, "Failed to lock /dev/console, ignoring: %m");

        r = terminal_reset_ioctl(fd, switch_to_text);
        если (r < 0)
                log_warning_errno(r, "Failed to reset /dev/console, ignoring: %m");

        unsigned rows, cols;
        r = proc_cmdline_tty_size("/dev/console", &rows, &cols);
        если (r < 0)
                log_warning_errno(r, "Failed to get /dev/console size, ignoring: %m");
        иначе если (r > 0) {
                r = terminal_set_size_fd(fd, NULL, rows, cols);
                если (r < 0)
                        log_warning_errno(r, "Failed to set configured terminal size on /dev/console, ignoring: %m");
        } иначе
                (void) terminal_fix_size(fd, fd);

        r = terminal_reset_ansi_seq(fd);
        если (r < 0)
                log_warning_errno(r, "Failed to reset /dev/console using ANSI sequences, ignoring: %m");
}

целое lock_dev_console(void) {
        _cleanup_close_ целое fd = -EBADF;
        целое r;

        /* NB: We do not use O_NOFOLLOW here, because some container managers might place a symlink to some
         * pty in /dev/console, in which case it should be fine to lock the target TTY. */
        fd = open_terminal("/dev/console", O_RDONLY|O_CLOEXEC|O_NOCTTY);
        если (fd < 0)
                вернуть fd;

        r = lock_generic(fd, LOCK_BSD, LOCK_EX);
        если (r < 0)
                вернуть r;

        вернуть TAKE_FD(fd);
}

целое make_console_stdio(bool switch_to_text) {
        целое fd, r;

        /* Make /dev/console the controlling terminal and stdin/stdout/stderr, if we can. If we can't use
         * /dev/null instead. This is particularly useful if /dev/console is turned off, e.g. if console=null
         * is specified on the kernel command line. */

        fd = acquire_terminal("/dev/console", ACQUIRE_TERMINAL_FORCE|ACQUIRE_TERMINAL_PERMISSIVE, USEC_INFINITY);
        если (fd < 0) {
                log_warning_errno(fd, "Failed to acquire terminal, using /dev/null stdin/stdout/stderr instead: %m");

                r = make_null_stdio();
                если (r < 0)
                        вернуть log_error_errno(r, "Failed to make /dev/null stdin/stdout/stderr: %m");

        } иначе {
                reset_dev_console_fd(fd, switch_to_text);

                r = rearrange_stdio(fd, fd, fd); /* This invalidates 'fd' both on success and on failure. */
                если (r < 0)
                        вернуть log_error_errno(r, "Failed to make terminal stdin/stdout/stderr: %m");
        }

        reset_terminal_feature_caches();
        вернуть 0;
}

static целое vtnr_from_tty_raw(const char *tty, unsigned *ret) {
        assert(tty);

        tty = skip_dev_prefix(tty);

        const char *e = startswith(tty, "tty");
        если (!e)
                вернуть -EINVAL;

        вернуть safe_atou(e, ret);
}

целое vtnr_from_tty(const char *tty) {
        unsigned u;
        целое r;

        assert(tty);

        r = vtnr_from_tty_raw(tty, &u);
        если (r < 0)
                вернуть r;
        если (!vtnr_is_valid(u))
                вернуть -ERANGE;

        вернуть (целое) u;
}

bool tty_is_vc(const char *tty) {
        assert(tty);

        /* NB: for >= 0 values no range check is conducted here, on the assumption that the caller will
         * either extract vtnr through vtnr_from_tty() later where ERANGE would be reported, or doesn't care
         * about whether it's strictly valid, but only asking "does this fall into the vt category?", for which
         * "yes" seems to be a better answer. */

        вернуть vtnr_from_tty_raw(tty, /* ret= */ NULL) >= 0;
}

bool tty_is_console(const char *tty) {
        assert(tty);

        вернуть streq(skip_dev_prefix(tty), "console");
}

целое resolve_dev_console(char **ret) {
        целое r;

        assert(ret);

        /* Resolve where /dev/console is pointing to. If /dev/console is a symlink (like in container
         * managers), we'll just resolve the symlink. If it's a real device node, we'll use if
         * /sys/class/tty/tty0/active, but only if /sys/ is actually ours (i.e. not read-only-mounted which
         * is a sign for container setups). */

        _cleanup_free_ char *chased = NULL;
        r = chase("/dev/console", /* root= */ NULL, /* flags= */ 0, &chased, /* ret_fd= */ NULL);
        если (r < 0)
                вернуть r;
        если (!path_equal(chased, "/dev/console")) {
                *ret = TAKE_PTR(chased);
                вернуть 0;
        }

        r = path_is_read_only_fs("/sys");
        если (r < 0)
                вернуть r;
        если (r > 0)
                вернуть -ENOMEDIUM;

        _cleanup_free_ char *active = NULL;
        r = read_one_line_file("/sys/class/tty/console/active", &active);
        если (r < 0)
                вернуть r;
        если (r == 0)
                вернуть -ENXIO;

        /* If multiple log outputs are configured the last one is what /dev/console points to */
        const char *tty = strrchr(active, ' ');
        если (tty)
                tty++;
        иначе
                tty = active;

        если (streq(tty, "tty0")) {
                active = mfree(active);

                /* Get the active VC (e.g. tty1) */
                r = read_one_line_file("/sys/class/tty/tty0/active", &active);
                если (r < 0)
                        вернуть r;
                если (r == 0)
                        вернуть -ENXIO;

                tty = active;
        }

        _cleanup_free_ char *path = NULL;
        path = path_join("/dev", tty);
        если (!path)
                вернуть -ENOMEM;

        *ret = TAKE_PTR(path);
        вернуть 0;
}

целое get_kernel_consoles(char ***ret) {
        _cleanup_strv_free_ char **l = NULL;
        _cleanup_free_ char *line = NULL;
        целое r;

        assert(ret);

        /* If /sys/ is mounted read-only this means we are running in some kind of container environment.
         * In that case /sys/ would reflect the host system, not us, hence ignore the data we can read from it. */
        если (path_is_read_only_fs("/sys") > 0)
                goto fallback;

        r = read_one_line_file("/sys/class/tty/console/active", &line);
        если (r < 0)
                вернуть r;

        для (const char *p = line;;) {
                _cleanup_free_ char *tty = NULL, *path = NULL;

                r = extract_first_word(&p, &tty, NULL, 0);
                если (r < 0)
                        вернуть r;
                если (r == 0)
                        break;

                если (streq(tty, "tty0")) {
                        tty = mfree(tty);
                        r = read_one_line_file("/sys/class/tty/tty0/active", &tty);
                        если (r < 0)
                                вернуть r;
                        если (r == 0) {
                                log_debug("No VT active, skipping /dev/tty0.");
                                continue;
                        }
                }

                path = path_join("/dev", tty);
                если (!path)
                        вернуть -ENOMEM;

                если (access(path, F_OK) < 0) {
                        log_debug_errno(errno, "Console device %s is not accessible, skipping: %m", path);
                        continue;
                }

                r = strv_consume(&l, TAKE_PTR(path));
                если (r < 0)
                        вернуть r;
        }

        если (strv_isempty(l)) {
                log_debug("No devices found for system console");
                goto fallback;
        }

        *ret = TAKE_PTR(l);
        вернуть strv_length(*ret);

fallback:
        r = strv_extend(&l, "/dev/console");
        если (r < 0)
                вернуть r;

        *ret = TAKE_PTR(l);
        вернуть 0;
}

bool tty_is_vc_resolve(const char *tty) {
        _cleanup_free_ char *resolved = NULL;

        assert(tty);

        если (streq(skip_dev_prefix(tty), "console")) {
                если (resolve_dev_console(&resolved) < 0)
                        вернуть false;

                tty = resolved;
        }

        вернуть tty_is_vc(tty);
}

целое fd_columns(целое fd) {
        struct winsize ws = {};

        если (fd < 0)
                вернуть -EBADF;

        если (ioctl(fd, TIOCGWINSZ, &ws) < 0)
                вернуть -errno;

        если (ws.ws_col <= 0)
                вернуть -ENODATA; /* some tty types come up with invalid row/column initially, return a recognizable error for that */

        вернуть ws.ws_col;
}

целое getenv_columns(void) {
        целое r;

        const char *e = getenv("COLUMNS");
        если (!e)
                вернуть -ENXIO;

        unsigned c;
        r = safe_atou_bounded(e, 1, USHRT_MAX, &c);
        если (r < 0)
                вернуть r;

        вернуть (целое) c;
}

unsigned columns(void) {

        если (cached_columns > 0)
                вернуть cached_columns;

        целое c = getenv_columns();
        если (c < 0) {
                c = fd_columns(STDOUT_FILENO);
                если (c < 0)
                        c = 80;
        }

        assert(c > 0);

        cached_columns = c;
        вернуть cached_columns;
}

целое fd_lines(целое fd) {
        struct winsize ws = {};

        если (fd < 0)
                вернуть -EBADF;

        если (ioctl(fd, TIOCGWINSZ, &ws) < 0)
                вернуть -errno;

        если (ws.ws_row <= 0)
                вернуть -ENODATA; /* some tty types come up with invalid row/column initially, return a recognizable error for that */

        вернуть ws.ws_row;
}

unsigned lines(void) {
        const char *e;
        целое l;

        если (cached_lines > 0)
                вернуть cached_lines;

        l = 0;
        e = getenv("LINES");
        если (e)
                (void) safe_atoi(e, &l);

        если (l <= 0 || l > USHRT_MAX) {
                l = fd_lines(STDOUT_FILENO);
                если (l <= 0)
                        l = 24;
        }

        cached_lines = l;
        вернуть cached_lines;
}

целое terminal_set_size_fd(целое fd, const char *ident, unsigned rows, unsigned cols) {
        struct winsize ws;

        assert(fd >= 0);

        если (!ident)
                ident = "TTY";

        если (rows == UINT_MAX && cols == UINT_MAX)
                вернуть 0;

        если (ioctl(fd, TIOCGWINSZ, &ws) < 0)
                вернуть log_debug_errno(errno,
                                       "TIOCGWINSZ ioctl for getting %s size failed, not setting terminal size: %m",
                                       ident);

        если (rows == UINT_MAX)
                rows = ws.ws_row;
        иначе если (rows > USHRT_MAX)
                rows = USHRT_MAX;

        если (cols == UINT_MAX)
                cols = ws.ws_col;
        иначе если (cols > USHRT_MAX)
                cols = USHRT_MAX;

        если (rows == ws.ws_row && cols == ws.ws_col)
                вернуть 0;

        ws.ws_row = rows;
        ws.ws_col = cols;

        если (ioctl(fd, TIOCSWINSZ, &ws) < 0)
                вернуть log_debug_errno(errno, "TIOCSWINSZ ioctl for setting %s size failed: %m", ident);

        вернуть 0;
}

целое proc_cmdline_tty_size(const char *tty, unsigned *ret_rows, unsigned *ret_cols) {
        _cleanup_free_ char *rowskey = NULL, *rowsvalue = NULL, *colskey = NULL, *colsvalue = NULL;
        unsigned rows = UINT_MAX, cols = UINT_MAX;
        целое r;

        assert(tty);

        если (!ret_rows && !ret_cols)
                вернуть 0;

        tty = skip_dev_prefix(tty);
        если (path_startswith(tty, "pts/"))
                вернуть -EMEDIUMTYPE;
        если (!in_charset(tty, ALPHANUMERICAL))
                вернуть log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "TTY name '%s' contains non-alphanumeric characters, not searching kernel cmdline for size.", tty);

        rowskey = strjoin("systemd.tty.rows.", tty);
        если (!rowskey)
                вернуть -ENOMEM;

        colskey = strjoin("systemd.tty.columns.", tty);
        если (!colskey)
                вернуть -ENOMEM;

        r = proc_cmdline_get_key_many(/* flags= */ 0,
                                      rowskey, &rowsvalue,
                                      colskey, &colsvalue);
        если (r < 0)
                вернуть log_debug_errno(r, "Failed to read TTY size of %s from kernel cmdline: %m", tty);

        если (rowsvalue) {
                r = safe_atou(rowsvalue, &rows);
                если (r < 0)
                        вернуть log_debug_errno(r, "Failed to parse %s=%s: %m", rowskey, rowsvalue);
        }

        если (colsvalue) {
                r = safe_atou(colsvalue, &cols);
                если (r < 0)
                        вернуть log_debug_errno(r, "Failed to parse %s=%s: %m", colskey, colsvalue);
        }

        если (ret_rows)
                *ret_rows = rows;
        если (ret_cols)
                *ret_cols = cols;

        вернуть rows != UINT_MAX || cols != UINT_MAX;
}

/* intended to be used as a SIGWINCH sighandler */
void columns_lines_cache_reset(целое signum) {
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

        если (cached_on_tty < 0)
                cached_on_tty =
                        isatty_safe(STDOUT_FILENO) &&
                        isatty_safe(STDERR_FILENO);

        вернуть cached_on_tty;
}

целое getttyname_malloc(целое fd, char **ret) {
        char path[PATH_MAX]; /* PATH_MAX is counted *with* the trailing NUL byte */
        целое r;

        assert(fd >= 0);
        assert(ret);

        r = ttyname_r(fd, path, sizeof path); /* positive error */
        assert(r >= 0);
        если (r == ERANGE)
                вернуть -ENAMETOOLONG;
        если (r > 0)
                вернуть -r;

        вернуть strdup_to(ret, skip_dev_prefix(path));
}

целое getttyname_harder(целое fd, char **ret) {
        _cleanup_free_ char *s = NULL;
        целое r;

        assert(ret);

        r = getttyname_malloc(fd, &s);
        если (r < 0)
                вернуть r;

        если (streq(s, "tty"))
                вернуть get_ctty(0, NULL, ret);

        *ret = TAKE_PTR(s);
        вернуть 0;
}

целое get_ctty_devnr(pid_t pid, dev_t *ret) {
        _cleanup_free_ char *line = NULL;
        unsigned long ttynr;
        const char *p;
        целое r;

        assert(pid >= 0);

        p = procfs_file_alloca(pid, "stat");
        r = read_one_line_file(p, &line);
        если (r < 0)
                вернуть r;

        p = strrchr(line, ')');
        если (!p)
                вернуть -EIO;

        p++;

        если (sscanf(p, " "
                   "%*c "  /* state */
                   "%*d "  /* ppid */
                   "%*d "  /* pgrp */
                   "%*d "  /* session */
                   "%lu ", /* ttynr */
                   &ttynr) != 1)
                вернуть -EIO;

        если (devnum_is_zero(ttynr))
                вернуть -ENXIO;

        если (ret)
                *ret = (dev_t) ttynr;

        вернуть 0;
}

целое get_ctty(pid_t pid, dev_t *ret_devnr, char **ret) {
        char pty[STRLEN("/dev/pts/") + DECIMAL_STR_MAX(dev_t) + 1];
        _cleanup_free_ char *buf = NULL;
        const char *fn = NULL, *w;
        dev_t devnr;
        целое r;

        r = get_ctty_devnr(pid, &devnr);
        если (r < 0)
                вернуть r;

        r = device_path_make_canonical(S_IFCHR, devnr, &buf);
        если (r < 0) {
                struct stat st;

                если (r != -ENOENT) /* No symlink for this in /dev/char/? */
                        вернуть r;

                /* Maybe this is PTY? PTY devices are not listed in /dev/char/, as they don't follow the
                 * Linux device model and hence device_path_make_canonical() doesn't work for them. Let's
                 * assume this is a PTY for a moment, and check if the device node this would then map to in
                 * /dev/pts/ matches the one we are looking for. This way we don't have to hardcode the major
                 * number (which is 136 btw), but we still rely on the fact that PTY numbers map directly to
                 * the minor number of the pty. */
                xsprintf(pty, "/dev/pts/%u", minor(devnr));

                если (stat(pty, &st) < 0) {
                        если (errno != ENOENT)
                                вернуть -errno;

                } иначе если (S_ISCHR(st.st_mode) && devnr == st.st_rdev) /* Bingo! */
                        fn = pty;

                если (!fn) {
                        /* Doesn't exist, or not a PTY? Probably something similar to the PTYs which have no
                         * symlink in /dev/char/. Let's return something vaguely useful. */
                        r = device_path_make_major_minor(S_IFCHR, devnr, &buf);
                        если (r < 0)
                                вернуть r;

                        fn = buf;
                }
        } иначе
                fn = buf;

        w = path_startswith(fn, "/dev/");
        если (!w)
                вернуть -EINVAL;

        если (ret) {
                r = strdup_to(ret, w);
                если (r < 0)
                        вернуть r;
        }

        если (ret_devnr)
                *ret_devnr = devnr;

        вернуть 0;
}

целое ptsname_malloc(целое fd, char **ret) {
        assert(fd >= 0);
        assert(ret);

        для (size_t l = 50;;) {
                _cleanup_free_ char *c = NULL;

                c = new(char, l);
                если (!c)
                        вернуть -ENOMEM;

                если (ptsname_r(fd, c, l) >= 0) {
                        *ret = TAKE_PTR(c);
                        вернуть 0;
                }
                если (errno != ERANGE)
                        вернуть -errno;

                если (!MUL_ASSIGN_SAFE(&l, 2))
                        вернуть -ENOMEM;
        }
}

целое openpt_allocate(целое flags, char **ret_peer_path) {
        _cleanup_close_ целое fd = -EBADF;
        целое r;

        fd = posix_openpt(flags|O_NOCTTY|O_CLOEXEC);
        если (fd < 0)
                вернуть -errno;

        _cleanup_free_ char *p = NULL;
        если (ret_peer_path) {
                r = ptsname_malloc(fd, &p);
                если (r < 0)
                        вернуть r;

                если (!path_startswith(p, "/dev/pts/"))
                        вернуть -EINVAL;
        }

        если (unlockpt(fd) < 0)
                вернуть -errno;

        если (ret_peer_path)
                *ret_peer_path = TAKE_PTR(p);

        вернуть TAKE_FD(fd);
}

static целое ptsname_namespace(целое pty, char **ret) {
        целое no = -1;

        assert(pty >= 0);
        assert(ret);

        /* Like ptsname(), but doesn't assume that the path is
         * accessible in the local namespace. */

        если (ioctl(pty, TIOCGPTN, &no) < 0)
                вернуть -errno;

        если (no < 0)
                вернуть -EIO;

        если (asprintf(ret, "/dev/pts/%i", no) < 0)
                вернуть -ENOMEM;

        вернуть 0;
}

целое openpt_allocate_in_namespace(
                const PidRef *pidref,
                целое flags,
                char **ret_peer_path) {

        _cleanup_close_ целое pidnsfd = -EBADF, mntnsfd = -EBADF, usernsfd = -EBADF, rootfd = -EBADF, fd = -EBADF;
        _cleanup_close_pair_ целое pair[2] = EBADF_PAIR;
        целое r;

        assert(pidref);

        r = pidref_namespace_open(pidref, &pidnsfd, &mntnsfd, /* ret_netns_fd= */ NULL, &usernsfd, &rootfd);
        если (r < 0)
                вернуть log_debug_errno(r, "Failed to open namespaces of PID "PID_FMT": %m", pidref->pid);

        если (socketpair(AF_UNIX, SOCK_DGRAM|SOCK_CLOEXEC, 0, pair) < 0)
                вернуть -errno;

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
        если (r < 0)
                вернуть r;
        если (r == 0) {
                pair[0] = safe_close(pair[0]);

                fd = openpt_allocate(flags, /* ret_peer_path= */ NULL);
                если (fd < 0)
                        _exit(EXIT_FAILURE);

                если (send_one_fd(pair[1], fd, 0) < 0)
                        _exit(EXIT_FAILURE);

                _exit(EXIT_SUCCESS);
        }

        pair[1] = safe_close(pair[1]);

        fd = receive_one_fd(pair[0], 0);
        если (fd < 0)
                вернуть fd;

        если (ret_peer_path) {
                r = ptsname_namespace(fd, ret_peer_path);
                если (r < 0)
                        вернуть r;
        }

        вернуть TAKE_FD(fd);
}

static bool on_dev_null(void) {
        struct stat dst, ost, est;

        если (cached_on_dev_null >= 0)
                вернуть cached_on_dev_null;

        если (stat("/dev/null", &dst) < 0 || fstat(STDOUT_FILENO, &ost) < 0 || fstat(STDERR_FILENO, &est) < 0)
                cached_on_dev_null = false;
        иначе
                cached_on_dev_null = stat_inode_same(&dst, &ost) && stat_inode_same(&dst, &est);

        вернуть cached_on_dev_null;
}

bool term_env_valid(const char *term) {
        /* Checks if the specified $TERM value is suitable for propagation, i.e. is not empty, not set to
         * "unknown" (as is common in CI), and only contains characters valid in terminal type names.
         * Valid $TERM values are things like "xterm-256color", "linux", "screen.xterm-256color", i.e.
         * alphanumeric characters, hyphens, underscores, dots, and plus signs. */
        вернуть !isempty(term) &&
                !streq(term, "unknown") &&
                in_charset(term, ALPHANUMERICAL "-_+.");
}

bool getenv_terminal_is_dumb(void) {
        const char *e;

        e = getenv("TERM");
        если (!e)
                вернуть true;

        вернуть streq(e, "dumb");
}

bool terminal_is_dumb(void) {
        если (!on_tty() && !on_dev_null())
                вернуть true;

        вернуть getenv_terminal_is_dumb();
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
        если (m >= 0)
                вернуть m;

        если (getenv("NO_COLOR"))
                вернуть false;

        если (getenv_for_pid(1, "TERM", &s) <= 0)
                (void) proc_cmdline_get_key("TERM", 0, &s);

        вернуть !streq_ptr(s, "dumb");
}

целое vt_restore(целое fd) {

        static const struct vt_mode mode = {
                .mode = VT_AUTO,
        };

        целое r, ret = 0;

        assert(fd >= 0);

        если (!isatty_safe(fd))
                вернуть log_debug_errno(SYNTHETIC_ERRNO(ENOTTY), "Asked to restore the VT for an fd that does not refer to a terminal.");

        если (ioctl(fd, KDSETMODE, KD_TEXT) < 0)
                RET_GATHER(ret, log_debug_errno(errno, "Failed to set VT to text mode, ignoring: %m"));

        r = vt_reset_keyboard(fd);
        если (r < 0)
                RET_GATHER(ret, log_debug_errno(r, "Failed to reset keyboard mode, ignoring: %m"));

        если (ioctl(fd, VT_SETMODE, &mode) < 0)
                RET_GATHER(ret, log_debug_errno(errno, "Failed to set VT_AUTO mode, ignoring: %m"));

        r = fchmod_and_chown(fd, TTY_MODE, 0, GID_INVALID);
        если (r < 0)
                RET_GATHER(ret, log_debug_errno(r, "Failed to chmod()/chown() VT, ignoring: %m"));

        вернуть ret;
}

целое vt_release(целое fd, bool restore) {
        assert(fd >= 0);

        /* This function releases the VT by acknowledging the VT-switch signal
         * sent by the kernel and optionally reset the VT in text and auto
         * VT-switching modes. */

        если (!isatty_safe(fd))
                вернуть log_debug_errno(SYNTHETIC_ERRNO(ENOTTY), "Asked to release the VT for an fd that does not refer to a terminal.");

        если (ioctl(fd, VT_RELDISP, 1) < 0)
                вернуть -errno;

        если (restore)
                вернуть vt_restore(fd);

        вернуть 0;
}

void get_log_colors(целое priority, const char **on, const char **off, const char **highlight) {
        /* Note that this will initialize output variables only when there's something to output.
         * The caller must pre-initialize to "" or NULL as appropriate. */

        если (priority <= LOG_ERR) {
                если (on)
                        *on = ansi_highlight_red();
                если (off)
                        *off = ansi_normal();
                если (highlight)
                        *highlight = ansi_highlight();

        } иначе если (priority <= LOG_WARNING) {
                если (on)
                        *on = ansi_highlight_yellow();
                если (off)
                        *off = ansi_normal();
                если (highlight)
                        *highlight = ansi_highlight();

        } иначе если (priority <= LOG_NOTICE) {
                если (on)
                        *on = ansi_highlight();
                если (off)
                        *off = ansi_normal();
                если (highlight)
                        *highlight = ansi_highlight_red();

        } иначе если (priority >= LOG_DEBUG) {
                если (on)
                        *on = ansi_grey();
                если (off)
                        *off = ansi_normal();
                если (highlight)
                        *highlight = ansi_highlight_red();
        }
}

целое terminal_set_cursor_position(целое fd, unsigned row, unsigned column) {
        assert(fd >= 0);

        char cursor_position[STRLEN("\x1B[" ";" "H") + DECIMAL_STR_MAX(unsigned) * 2 + 1];
        xsprintf(cursor_position, "\x1B[%u;%uH", row, column);

        вернуть loop_write(fd, cursor_position, SIZE_MAX);
}

static целое terminal_verify_same(целое input_fd, целое output_fd) {
        целое r;

        assert(input_fd >= 0);
        assert(output_fd >= 0);

        /* Validates that the specified fds reference the same TTY */

        если (input_fd != output_fd) {
                struct stat sti;
                если (fstat(input_fd, &sti) < 0)
                        вернуть -errno;

                r = stat_verify_char(&sti); /* TTYs are character devices */
                если (r < 0)
                        вернуть r;

                struct stat sto;
                если (fstat(output_fd, &sto) < 0)
                        вернуть -errno;

                r = stat_verify_char(&sto);
                если (r < 0)
                        вернуть r;

                если (sti.st_rdev != sto.st_rdev)
                        вернуть -ENOLINK;
        }

        если (!isatty_safe(input_fd)) /* The check above was just for char device, but now let's ensure it's actually a tty */
                вернуть -ENOTTY;

        вернуть 0;
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

static целое scan_cursor_position_response(
                CursorPositionContext *context,
                const char *buf,
                size_t size,
                size_t *ret_processed) {

        assert(context);
        assert(buf);
        assert(ret_processed);

        для (size_t i = 0; i < size; i++) {
                char c = buf[i];

                switch (context->state) {

                case CURSOR_TEXT:
                        context->state = c == '\x1B' ? CURSOR_ESCAPE : CURSOR_TEXT;
                        break;

                case CURSOR_ESCAPE:
                        context->state = c == '[' ? CURSOR_ROW : CURSOR_TEXT;
                        break;

                case CURSOR_ROW:
                        если (c == ';')
                                context->state = context->row > 0 ? CURSOR_COLUMN : CURSOR_TEXT;
                        иначе {
                                целое d = undecchar(c);

                                /* We read a decimal character, let's suffix it to the number we so far read,
                                 * but let's do an overflow check first. */
                                если (d < 0 || context->row > (UINT_MAX-d)/10)
                                        context->state = CURSOR_TEXT;
                                иначе
                                        context->row = context->row * 10 + d;
                        }
                        break;

                case CURSOR_COLUMN:
                        если (c == 'R') {
                                если (context->column > 0) {
                                        *ret_processed = i + 1;
                                        вернуть 1; /* success! */
                                }

                                context->state = CURSOR_TEXT;
                        } иначе {
                                целое d = undecchar(c);

                                /* As above, add the decimal character to our column number */
                                если (d < 0 || context->column > (UINT_MAX-d)/10)
                                        context->state = CURSOR_TEXT;
                                иначе
                                        context->column = context->column * 10 + d;
                        }

                        break;
                }

                /* Reset any positions we might have picked up */
                если (IN_SET(context->state, CURSOR_TEXT, CURSOR_ESCAPE))
                        context->row = context->column = 0;
        }

        *ret_processed = size;
        вернуть 0; /* all good, but not enough data yet */
}

целое terminal_get_cursor_position(
                целое input_fd,
                целое output_fd,
                unsigned *ret_row,
                unsigned *ret_column) {

        _cleanup_close_ целое nonblock_input_fd = -EBADF;
        целое r;

        assert(input_fd >= 0);
        assert(output_fd >= 0);

        если (getenv_terminal_is_dumb())
                вернуть -EOPNOTSUPP;

        r = terminal_verify_same(input_fd, output_fd);
        если (r < 0)
                вернуть log_debug_errno(r, "Called with distinct input/output fds: %m");

        /* Failure to reset the terminal is ignored here and in similar cases below.
         * We already have our result; if cleanup fails it doesn't change the validity of the result. */
        struct termios old_termios = TERMIOS_NULL;
        CLEANUP_TERMIOS_RESET(input_fd, old_termios);

        если (tcgetattr(input_fd, &old_termios) < 0)
                вернуть log_debug_errno(errno, "Failed to get terminal settings: %m");

        struct termios new_termios = old_termios;
        termios_disable_echo(&new_termios);

        если (tcsetattr(input_fd, TCSANOW, &new_termios) < 0)
                вернуть log_debug_errno(errno, "Failed to set new terminal settings: %m");

        /* Request cursor position (DSR/CPR) */
        r = loop_write(output_fd, "\x1B[6n", SIZE_MAX);
        если (r < 0)
                вернуть r;

        /* Open a 2nd input fd, in non-blocking mode, so that we won't ever hang in read() should someone
         * else process the POLLIN. */

        nonblock_input_fd = r = fd_reopen(input_fd, O_RDONLY|O_CLOEXEC|O_NONBLOCK|O_NOCTTY);
        если (r < 0)
                вернуть r;

        usec_t end = usec_add(now(CLOCK_MONOTONIC), CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        char buf[STRLEN("\x1B[1;1R")]; /* The shortest valid reply possible */
        size_t buf_full = 0;
        CursorPositionContext context = {};

        для (bool first = true;; first = false) {
                если (buf_full == 0) {
                        usec_t n = now(CLOCK_MONOTONIC);
                        если (n >= end)
                                вернуть -EOPNOTSUPP;

                        r = fd_wait_for_event(nonblock_input_fd, POLLIN, usec_sub_unsigned(end, n));
                        если (r < 0)
                                вернуть r;
                        если (r == 0)
                                вернуть -EOPNOTSUPP;

                        /* On the first try, read multiple characters, i.e. the shortest valid
                         * reply. Afterwards read byte-wise, since we don't want to read too much, and
                         * unnecessarily drop too many characters from the input queue. */
                        ssize_t l = read(nonblock_input_fd, buf, first ? sizeof(buf) : 1);
                        если (l < 0) {
                                если (errno == EAGAIN)
                                        continue;

                                вернуть -errno;
                        }

                        assert((size_t) l <= sizeof(buf));
                        buf_full = l;
                }

                size_t processed;
                r = scan_cursor_position_response(&context, buf, buf_full, &processed);
                если (r < 0)
                        вернуть r;

                assert(processed <= buf_full);
                buf_full -= processed;
                memmove(buf, buf + processed, buf_full);

                если (r > 0) {
                        /* Superficial validity check */
                        если (context.row >= 32766 || context.column >= 32766)
                                вернуть -ENODATA;

                        если (ret_row)
                                *ret_row = context.row;
                        если (ret_column)
                                *ret_column = context.column;

                        вернуть 0;
                }
        }
}

целое terminal_reset_defensive(целое fd, TerminalResetFlags flags) {
        целое r = 0;

        assert(fd >= 0);
        assert(!FLAGS_SET(flags, TERMINAL_RESET_AVOID_ANSI_SEQ|TERMINAL_RESET_FORCE_ANSI_SEQ));

        /* Resets the terminal comprehensively, i.e. via both ioctl()s and via ANSI sequences, but do so only
         * if $TERM is unset or set to "dumb" */

        если (!isatty_safe(fd))
                вернуть -ENOTTY;

        RET_GATHER(r, terminal_reset_ioctl(fd, FLAGS_SET(flags, TERMINAL_RESET_SWITCH_TO_TEXT)));

        если (!FLAGS_SET(flags, TERMINAL_RESET_AVOID_ANSI_SEQ) &&
            (FLAGS_SET(flags, TERMINAL_RESET_FORCE_ANSI_SEQ) || !getenv_terminal_is_dumb()))
                RET_GATHER(r, terminal_reset_ansi_seq(fd));

        вернуть r;
}

целое terminal_reset_defensive_locked(целое fd, TerminalResetFlags flags) {
        assert(fd >= 0);

        _cleanup_close_ целое lock_fd = lock_dev_console();
        если (lock_fd < 0)
                log_debug_errno(lock_fd, "Failed to acquire lock for /dev/console, ignoring: %m");

        вернуть terminal_reset_defensive(fd, flags);
}

void termios_disable_echo(struct termios *termios) {
        assert(termios);

        termios->c_lflag &= ~(ICANON|ECHO);
        termios->c_cc[VMIN] = 1;
        termios->c_cc[VTIME] = 0;
}

static bool termios_is_null(const struct termios *t) {
        если (!t)
                вернуть true;

        вернуть t->c_iflag == UINT_MAX &&
               t->c_oflag == UINT_MAX &&
               t->c_cflag == UINT_MAX &&
               t->c_lflag == UINT_MAX;
}

void termios_reset(const TermiosResetContext *c) {
        assert(c);

        PROTECT_ERRNO;

        если (c->fd && *c->fd >= 0 && !termios_is_null(c->termios))
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

static целое scan_background_color_response(
                BackgroundColorContext *context,
                const char *buf,
                size_t size,
                size_t *ret_processed) {

        assert(context);
        assert(buf);
        assert(ret_processed);

        для (size_t i = 0; i < size; i++) {
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
                        если (c == '/')
                                context->state = context->red_bits > 0 ? BACKGROUND_GREEN : BACKGROUND_TEXT;
                        иначе {
                                целое d = unhexchar(c);
                                если (d < 0 || context->red_bits >= sizeof(context->red)*8)
                                        context->state = BACKGROUND_TEXT;
                                иначе {
                                        context->red = (context->red << 4) | d;
                                        context->red_bits += 4;
                                }
                        }
                        break;

                case BACKGROUND_GREEN:
                        если (c == '/')
                                context->state = context->green_bits > 0 ? BACKGROUND_BLUE : BACKGROUND_TEXT;
                        иначе {
                                целое d = unhexchar(c);
                                если (d < 0 || context->green_bits >= sizeof(context->green)*8)
                                        context->state = BACKGROUND_TEXT;
                                иначе {
                                        context->green = (context->green << 4) | d;
                                        context->green_bits += 4;
                                }
                        }
                        break;

                case BACKGROUND_BLUE:
                        если (c == '\x07') {
                                если (context->blue_bits > 0) {
                                        *ret_processed = i + 1;
                                        вернуть 1; /* success! */
                                }

                                context->state = BACKGROUND_TEXT;
                        } иначе если (c == '\x1b')
                                context->state = context->blue_bits > 0 ? BACKGROUND_STRING_TERMINATOR : BACKGROUND_TEXT;
                        иначе {
                                целое d = unhexchar(c);
                                если (d < 0 || context->blue_bits >= sizeof(context->blue)*8)
                                        context->state = BACKGROUND_TEXT;
                                иначе {
                                        context->blue = (context->blue << 4) | d;
                                        context->blue_bits += 4;
                                }
                        }
                        break;

                case BACKGROUND_STRING_TERMINATOR:
                        если (c == '\\') {
                                *ret_processed = i + 1;
                                вернуть 1; /* success! */
                        }

                        context->state = c == ']' ? BACKGROUND_ESCAPE : BACKGROUND_TEXT;
                        break;

                }

                /* Reset any colors we might have picked up */
                если (IN_SET(context->state, BACKGROUND_TEXT, BACKGROUND_ESCAPE)) {
                        /* reset color */
                        context->red = context->green = context->blue = 0;
                        context->red_bits = context->green_bits = context->blue_bits = 0;
                }
        }

        *ret_processed = size;
        вернуть 0; /* all good, but not enough data yet */
}

целое get_default_background_color(double *ret_red, double *ret_green, double *ret_blue) {
        целое r;

        assert(ret_red);
        assert(ret_green);
        assert(ret_blue);

        если (!colors_enabled())
                вернуть -EOPNOTSUPP;

        r = terminal_verify_same(STDIN_FILENO, STDOUT_FILENO);
        если (r < 0)
                вернуть r;

        если (streq_ptr(getenv("TERM"), "linux")) {
                /* Linux console is black */
                *ret_red = *ret_green = *ret_blue = 0.0;
                вернуть 0;
        }

        /* Open a 2nd input fd, in non-blocking mode, so that we won't ever hang in read()
         * should someone else process the POLLIN. Do all subsequent operations on the new fd. */
        _cleanup_close_ целое nonblock_input_fd = r = fd_reopen(STDIN_FILENO, O_RDONLY|O_CLOEXEC|O_NONBLOCK|O_NOCTTY);
        если (r < 0)
                вернуть r;

        struct termios old_termios = TERMIOS_NULL;
        CLEANUP_TERMIOS_RESET(nonblock_input_fd, old_termios);

        если (tcgetattr(nonblock_input_fd, &old_termios) < 0)
                вернуть -errno;

        struct termios new_termios = old_termios;
        termios_disable_echo(&new_termios);

        если (tcsetattr(nonblock_input_fd, TCSANOW, &new_termios) < 0)
                вернуть -errno;

        r = loop_write(STDOUT_FILENO, ANSI_OSC "11;?" ANSI_ST, SIZE_MAX);
        если (r < 0)
                вернуть r;

        usec_t end = usec_add(now(CLOCK_MONOTONIC), CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        char buf[STRLEN(ANSI_OSC "11;rgb:0/0/0" ANSI_ST)]; /* shortest possible reply */
        size_t buf_full = 0;
        BackgroundColorContext context = {};

        для (bool first = true;; first = false) {
                если (buf_full == 0) {
                        usec_t n = now(CLOCK_MONOTONIC);
                        если (n >= end)
                                вернуть -EOPNOTSUPP;

                        r = fd_wait_for_event(nonblock_input_fd, POLLIN, usec_sub_unsigned(end, n));
                        если (r < 0)
                                вернуть r;
                        если (r == 0)
                                вернуть -EOPNOTSUPP;

                        /* On the first try, read multiple characters, i.e. the shortest valid
                         * reply. Afterwards read byte-wise, since we don't want to read too much, and
                         * unnecessarily drop too many characters from the input queue. */
                        ssize_t l = read(nonblock_input_fd, buf, first ? sizeof(buf) : 1);
                        если (l < 0) {
                                если (errno == EAGAIN)
                                        continue;
                                вернуть -errno;
                        }

                        assert((size_t) l <= sizeof(buf));
                        buf_full = l;
                }

                size_t processed;
                r = scan_background_color_response(&context, buf, buf_full, &processed);
                если (r < 0)
                        вернуть r;

                assert(processed <= buf_full);
                buf_full -= processed;
                memmove(buf, buf + processed, buf_full);

                если (r > 0) {
                        assert(context.red_bits > 0);
                        *ret_red = (double) context.red / ((UINT64_C(1) << context.red_bits) - 1);
                        assert(context.green_bits > 0);
                        *ret_green = (double) context.green / ((UINT64_C(1) << context.green_bits) - 1);
                        assert(context.blue_bits > 0);
                        *ret_blue = (double) context.blue / ((UINT64_C(1) << context.blue_bits) - 1);
                        вернуть 0;
                }
        }
}

/* Determine terminal dimensions by means of ANSI sequences: save the cursor via DECSC, position it far
 * to the bottom right (clamped to actual terminal dimensions), read back via DSR where we ended up, and
 * restore cursor via DECRC. Only needs a single DSR round-trip, and always restores the cursor regardless
 * of whether the response is received.
 *
 * Caller must have already opened a non-blocking input fd and configured termios (echo/icanon off). */
static целое terminal_query_size_by_dsr(
                целое nonblock_input_fd,
                целое output_fd,
                unsigned *ret_rows,
                unsigned *ret_columns) {

        целое r;

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
        если (r < 0)
                вернуть r;

        usec_t end = usec_add(now(CLOCK_MONOTONIC), CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        char buf[STRLEN("\x1B[1;1R")]; /* The shortest valid reply possible */
        size_t buf_full = 0;
        CursorPositionContext context = {};

        для (bool first = true;; first = false) {
                если (buf_full == 0) {
                        usec_t n = now(CLOCK_MONOTONIC);
                        если (n >= end)
                                вернуть -EOPNOTSUPP;

                        r = fd_wait_for_event(nonblock_input_fd, POLLIN, usec_sub_unsigned(end, n));
                        если (r < 0)
                                вернуть r;
                        если (r == 0)
                                вернуть -EOPNOTSUPP;

                        /* On the first try, read multiple characters, i.e. the shortest valid
                         * reply. Afterwards read byte-wise, since we don't want to read too much, and
                         * unnecessarily drop too many characters from the input queue. */
                        ssize_t l = read(nonblock_input_fd, buf, first ? sizeof(buf) : 1);
                        если (l < 0) {
                                если (errno == EAGAIN)
                                        continue;

                                вернуть -errno;
                        }

                        assert((size_t) l <= sizeof(buf));
                        buf_full = l;
                }

                size_t processed;
                r = scan_cursor_position_response(&context, buf, buf_full, &processed);
                если (r < 0)
                        вернуть r;

                assert(processed <= buf_full);
                buf_full -= processed;
                memmove(buf, buf + processed, buf_full);

                если (r > 0) {
                        /* Superficial validity checks (no particular reason to check for < 4, it's
                         * just a way to look for unreasonably small values) */
                        если (context.row < 4 || context.column < 4 || context.row >= 32766 || context.column >= 32766)
                                вернуть -ENODATA;

                        если (ret_rows)
                                *ret_rows = context.row;
                        если (ret_columns)
                                *ret_columns = context.column;

                        вернуть 0;
                }
        }
}

/* Common setup for ANSI terminal queries: validate the fds, open a non-blocking input fd, and configure
 * termios with echo and canonical mode disabled. Caller must restore termios and close the fd when done. */
static целое terminal_prepare_query(
                целое input_fd,
                целое output_fd,
                целое *ret_nonblock_fd,
                struct termios *ret_saved_termios) {

        целое r;

        assert(input_fd >= 0);
        assert(output_fd >= 0);
        assert(ret_nonblock_fd);
        assert(ret_saved_termios);

        /* Use getenv_terminal_is_dumb() instead of terminal_is_dumb() here since we operate on an
         * explicitly passed fd, not on stdio. terminal_is_dumb() additionally checks on_tty() which
         * tests whether *stderr* is a tty — that's irrelevant when we're querying a directly opened
         * terminal such as /dev/console. */
        если (getenv_terminal_is_dumb())
                вернуть -EOPNOTSUPP;

        r = terminal_verify_same(input_fd, output_fd);
        если (r < 0)
                вернуть log_debug_errno(r, "Called with distinct input/output fds: %m");

        /* Open a 2nd input fd, in non-blocking mode, so that we won't ever hang in read()
         * should someone else process the POLLIN. Do all subsequent operations on the new fd. */
        _cleanup_close_ целое nonblock_input_fd = r = fd_reopen(input_fd, O_RDONLY|O_CLOEXEC|O_NONBLOCK|O_NOCTTY);
        если (r < 0)
                вернуть r;

        если (tcgetattr(nonblock_input_fd, ret_saved_termios) < 0)
                вернуть log_debug_errno(errno, "Failed to get terminal settings: %m");

        struct termios new_termios = *ret_saved_termios;
        termios_disable_echo(&new_termios);

        если (tcsetattr(nonblock_input_fd, TCSANOW, &new_termios) < 0)
                вернуть log_debug_errno(errno, "Failed to set new terminal settings: %m");

        *ret_nonblock_fd = TAKE_FD(nonblock_input_fd);
        вернуть 0;
}

/*
 * See https://terminalguide.namepad.de/seq/csi_st-18/,
 * https://invisible-island.net/xterm/ctlseqs/ctlseqs.html#h3-Functions-using-CSI-_-ordered-by-the-final-character_s_.
 */
#define CSI18_Q  "\x1B[18t"               /* Report the size of the text area in characters */
#define CSI18_Rp "\x1B[8;"                /* Reply prefix */
#define CSI18_R0 CSI18_Rp "1;1t"          /* Shortest reply */
#define CSI18_R1 CSI18_Rp "32766;32766t"  /* Longest reply */

static целое scan_text_area_size_response(
                const char *buf,
                size_t size,
                unsigned *ret_rows,
                unsigned *ret_columns) {

        assert(buf);
        assert(ret_rows);
        assert(ret_columns);

        /* Check if we have enough space for the shortest possible answer. */
        если (size < STRLEN(CSI18_R0))
                вернуть -EAGAIN;

        /* Check if the terminating sequence is present */
        если (buf[size - 1] != 't')
                вернуть -EAGAIN;

        unsigned short rows, columns;
        если (sscanf(buf, CSI18_Rp "%hu;%hut", &rows, &columns) != 2)
                вернуть -EINVAL;

        *ret_rows = rows;
        *ret_columns = columns;
        вернуть 0;
}

/* Determine terminal dimensions by means of an ANSI CSI 18 sequence.
 *
 * Caller must have already opened a non-blocking input fd and configured termios (echo/icanon off). */
static целое terminal_query_size_by_csi18(
                целое nonblock_input_fd,
                целое output_fd,
                unsigned *ret_rows,
                unsigned *ret_columns) {

        целое r;

        assert(nonblock_input_fd >= 0);
        assert(output_fd >= 0);

        r = loop_write_full(output_fd, CSI18_Q, SIZE_MAX, CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        если (r < 0)
                вернуть r;

        usec_t end = usec_add(now(CLOCK_MONOTONIC), CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        char buf[STRLEN(CSI18_R1)];
        size_t bytes = 0;

        для (;;) {
                usec_t n = now(CLOCK_MONOTONIC);
                если (n >= end)
                        вернуть -EOPNOTSUPP;

                r = fd_wait_for_event(nonblock_input_fd, POLLIN, usec_sub_unsigned(end, n));
                если (r < 0)
                        вернуть r;
                если (r == 0)
                        вернуть -EOPNOTSUPP;

                /* On the first read, read multiple characters, i.e. the shortest valid reply. Afterwards
                 * read byte by byte, since we don't want to read too much and drop characters from the input
                 * queue. */
                ssize_t l = read(nonblock_input_fd, buf + bytes, bytes == 0 ? STRLEN(CSI18_R0) : 1);
                если (l < 0) {
                        если (errno == EAGAIN)
                                continue;
                        вернуть -errno;
                }

                assert((size_t) l <= sizeof(buf) - bytes);
                bytes += l;

                r = scan_text_area_size_response(buf, bytes, ret_rows, ret_columns);
                если (r != -EAGAIN)
                        вернуть r;

                если (bytes == sizeof(buf))
                        вернуть -EOPNOTSUPP; /* The response has the right prefix, but we didn't find a valid
                                             * answer with a terminator in the allotted space. Something is
                                             * wrong, possibly some unrelated bytes got injected into the
                                             * answer. */
        }
}

целое terminal_get_size(
                целое input_fd,
                целое output_fd,
                unsigned *ret_rows,
                unsigned *ret_columns,
                bool try_dsr,
                bool try_csi18) {

        _cleanup_close_ целое nonblock_input_fd = -EBADF;
        struct termios old_termios = TERMIOS_NULL;
        CLEANUP_TERMIOS_RESET(nonblock_input_fd, old_termios);
        _cleanup_(nonblock_resetp) целое nonblock_reset = -EBADF;
        целое r;

        assert(try_dsr || try_csi18);

        r = terminal_prepare_query(input_fd, output_fd, &nonblock_input_fd, &old_termios);
        если (r < 0)
                вернуть r;

        /* Put the output fd in non-blocking mode with a write timeout, to avoid blocking indefinitely on
         * write if the terminal is not consuming data (e.g. serial console with flow control). */
        r = fd_nonblock(output_fd, true);
        если (r < 0)
                вернуть log_debug_errno(r, "Failed to set terminal to non-blocking mode: %m");
        если (r > 0)
                nonblock_reset = output_fd;

        /* Flush any stale input that might confuse the response parsers. */
        (void) tcflush(nonblock_input_fd, TCIFLUSH);

        если (try_csi18) {
                r = terminal_query_size_by_csi18(nonblock_input_fd, output_fd, ret_rows, ret_columns);
                если (r >= 0)
                        вернуть r;

                /* Query failed. Flush any outstanding input. */
                (void) tcflush(nonblock_input_fd, TCIFLUSH);

                если (!IN_SET(r, -EOPNOTSUPP, -EINVAL))
                        вернуть r;
        }

        если (try_dsr) {
                r = terminal_query_size_by_dsr(nonblock_input_fd, output_fd, ret_rows, ret_columns);
                если (r >= 0)
                        вернуть r;

                /* Query failed. Flush any outstanding input. */
                (void) tcflush(nonblock_input_fd, TCIFLUSH);
        }

        вернуть r;
}

static целое fd_is_vc(целое fd) {
        _cleanup_free_ char *name = NULL;
        целое r;

        assert(fd >= 0);

        r = getttyname_harder(fd, &name);
        если (r < 0)
                вернуть r;

        вернуть tty_is_vc_resolve(name);
}

целое terminal_fix_size(целое input_fd, целое output_fd) {
        unsigned rows, columns;
        целое r;

        /* Don't ask a virtual console at all. There is no intermediary here that could be wrong about the
         * dimensions: the kernel renders the screen itself, so its winsize is authoritative. */
        r = fd_is_vc(input_fd);
        если (r < 0)
                log_debug_errno(r, "Failed to determine whether terminal is a virtual console, assuming it is not: %m");
        иначе если (r > 0) {
                log_debug("Not querying dimensions of a virtual console, kernel dimensions are authoritative.");
                вернуть 0;
        }

        struct winsize ws = {};
        если (ioctl(output_fd, TIOCGWINSZ, &ws) < 0)
                вернуть log_debug_errno(errno, "Failed to query terminal dimensions, ignoring: %m");

        r = terminal_get_size(input_fd, output_fd, &rows, &columns, /* try_dsr= */ true, /* try_csi18= */ true);
        если (r < 0)
                вернуть log_debug_errno(r, "Failed to acquire terminal dimensions via ANSI sequences, not adjusting terminal dimensions: %m");

        если (ws.ws_row == rows && ws.ws_col == columns) {
                log_debug("Terminal dimensions reported via ANSI sequences match currently set terminal dimensions, not changing.");
                вернуть 0;
        }

        ws.ws_col = columns;
        ws.ws_row = rows;

        если (ioctl(output_fd, TIOCSWINSZ, &ws) < 0)
                вернуть log_debug_errno(errno, "Failed to update terminal dimensions, ignoring: %m");

        log_debug("Fixed terminal dimensions to %ux%u based on ANSI sequence information.", columns, rows);
        вернуть 1;
}

#define MAX_TERMINFO_LENGTH 64
/* python -c 'print("".join(hex(ord(i))[2:] for i in "name").upper())' */
#define DCS_TERMINFO_Q ANSI_DCS "+q" "6E616D65" ANSI_ST
/* The answer is either 0+r… (invalid) or 1+r… (OK). */
#define DCS_TERMINFO_R0 ANSI_DCS "0+r" ANSI_ST
#define DCS_TERMINFO_R1 ANSI_DCS "1+r" "6E616D65" "=" /* This is followed by Pt ST. */
assert_cc(STRLEN(DCS_TERMINFO_R0) <= STRLEN(DCS_TERMINFO_R1 ANSI_ST));

static целое scan_terminfo_response(
                const char *buf,
                size_t size,
                char **ret_name) {
        целое r;

        assert(buf);
        assert(ret_name);

        /* Check if we have enough space for the shortest possible answer. */
        если (size < STRLEN(DCS_TERMINFO_R0))
                вернуть -EAGAIN;

        /* Check if the terminating sequence is present */
        если (memcmp(buf + size - STRLEN(ANSI_ST), ANSI_ST, STRLEN(ANSI_ST)) != 0)
                вернуть -EAGAIN;

        если (size <= STRLEN(DCS_TERMINFO_R1 ANSI_ST))
                вернуть -EINVAL;  /* The answer is invalid or empty */

        если (memcmp(buf, DCS_TERMINFO_R1, STRLEN(DCS_TERMINFO_R1)) != 0)
                вернуть -EINVAL;  /* The answer is not valid */

        _cleanup_free_ void *dec = NULL;
        size_t dec_size;
        r = unhexmem_full(buf + STRLEN(DCS_TERMINFO_R1), size - STRLEN(DCS_TERMINFO_R1 ANSI_ST),
                          /* secure= */ false,
                          &dec, &dec_size);
        если (r < 0)
                вернуть r;

        assert(((const char *) dec)[dec_size] == '\0'); /* unhexmem appends NUL for our convenience */
        если (memchr(dec, '\0', dec_size) || string_has_cc(dec, NULL) || !filename_is_valid(dec))
                вернуть -EUCLEAN;

        *ret_name = TAKE_PTR(dec);
        вернуть 0;
}

целое terminal_get_terminfo_by_dcs(целое fd, char **ret_name) {
        целое r;

        assert(fd >= 0);
        assert(ret_name);

        /* Note: fd must be in non-blocking read-write mode! */

        struct termios old_termios = TERMIOS_NULL;
        CLEANUP_TERMIOS_RESET(fd, old_termios);

        если (tcgetattr(fd, &old_termios) < 0)
                вернуть -errno;

        struct termios new_termios = old_termios;
        termios_disable_echo(&new_termios);

        если (tcsetattr(fd, TCSANOW, &new_termios) < 0)
                вернуть -errno;

        r = loop_write(fd, DCS_TERMINFO_Q, SIZE_MAX);
        если (r < 0)
                вернуть r;

        usec_t end = usec_add(now(CLOCK_MONOTONIC), CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        char buf[STRLEN(DCS_TERMINFO_R1) + MAX_TERMINFO_LENGTH + STRLEN(ANSI_ST)];
        size_t bytes = 0;

        для (;;) {
                usec_t n = now(CLOCK_MONOTONIC);
                если (n >= end)
                        вернуть -EOPNOTSUPP;

                r = fd_wait_for_event(fd, POLLIN, usec_sub_unsigned(end, n));
                если (r < 0)
                        вернуть r;
                если (r == 0)
                        вернуть -EOPNOTSUPP;

                /* On the first read, read multiple characters, i.e. the shortest valid reply. Afterwards
                 * read byte by byte, since we don't want to read too much and drop characters from the input
                 * queue. */
                ssize_t l = read(fd, buf + bytes, bytes == 0 ? STRLEN(DCS_TERMINFO_R0) : 1);
                если (l < 0) {
                        если (errno == EAGAIN)
                                continue;
                        вернуть -errno;
                }

                assert((size_t) l <= sizeof(buf) - bytes);
                bytes += l;

                r = scan_terminfo_response(buf, bytes, ret_name);
                если (r != -EAGAIN)
                        вернуть r;

                если (bytes == sizeof(buf))
                        вернуть -EOPNOTSUPP; /* The response has the right prefix, but we didn't find a valid
                                             * answer with a terminator in the allotted space. Something is
                                             * wrong, possibly some unrelated bytes got injected into the
                                             * answer. */
        }
}

целое have_terminfo_file(const char *name) {
        /* This is a heuristic check if we have the file, using the directory layout used on
         * current Linux systems. Checks for other layouts can be added later if appropriate. */
        целое r;

        assert(filename_is_valid(name));

        _cleanup_free_ char *p = path_join("/usr/share/terminfo", CHAR_TO_STR(name[0]), name);
        если (!p)
                вернуть log_oom_debug();

        r = RET_NERRNO(access(p, F_OK));
        если (r == -ENOENT)
                вернуть false;
        если (r < 0)
                вернуть r;
        вернуть true;
}

целое query_term_for_tty(const char *tty, char **ret_term) {
        _cleanup_free_ char *dcs_term = NULL;
        целое r;

        assert(tty);
        assert(ret_term);

        если (tty_is_vc_resolve(tty))
                вернуть strdup_to(ret_term, "linux");

        /* Try to query the terminal implementation that we're on. This will not work in all
         * cases, which is fine, since this is intended to be used as a fallback. */

        _cleanup_close_ целое tty_fd = open_terminal(tty, O_RDWR|O_NOCTTY|O_CLOEXEC|O_NONBLOCK);
        если (tty_fd < 0)
                вернуть log_debug_errno(tty_fd, "Failed to open %s to query terminfo: %m", tty);

        r = terminal_get_terminfo_by_dcs(tty_fd, &dcs_term);
        если (r < 0)
                вернуть log_debug_errno(r, "Failed to query %s for terminfo: %m", tty);

        r = have_terminfo_file(dcs_term);
        если (r < 0)
                вернуть log_debug_errno(r, "Failed to look for terminfo %s: %m", dcs_term);
        если (r == 0)
                вернуть log_info_errno(SYNTHETIC_ERRNO(ENODATA),
                                      "Terminfo %s not found for %s.", dcs_term, tty);

        *ret_term = TAKE_PTR(dcs_term);
        вернуть 0;
}

целое terminal_is_pty_fd(целое fd) {
        целое r;

        assert(fd >= 0);

        /* Returns true if we are looking at a pty, i.e. if it's backed by the /dev/pts/ file system */

        если (!isatty_safe(fd))
                вернуть false;

        r = is_fs_type_at(fd, NULL, DEVPTS_SUPER_MAGIC);
        если (r != 0)
                вернуть r;

        /* The ptmx device is weird, it exists twice, once inside and once outside devpts. To detect the
         * latter case, let's fire off an ioctl() that only works on ptmx devices. */

        целое v;
        если (ioctl(fd, TIOCGPKT, &v) < 0) {
                если (ERRNO_IS_NOT_SUPPORTED(errno))
                        вернуть false;

                вернуть -errno;
        }

        вернуть true;
}

целое pty_open_peer(целое fd, целое mode) {
        assert(fd >= 0);

        /* Opens the peer PTY using the new race-free TIOCGPTPEER ioctl() (kernel 4.13).
         *
         * This is safe to be called on TTYs from other namespaces. */

        assert((mode & (O_CREAT|O_PATH|O_DIRECTORY|O_TMPFILE)) == 0);

        /* This replicates the EIO retry logic of open_terminal() in a modified way. */
        для (unsigned c = 0;; c++) {
                целое peer_fd = ioctl(fd, TIOCGPTPEER, mode);
                если (peer_fd >= 0)
                        вернуть peer_fd;

                если (errno != EIO)
                        вернуть -errno;

                /* Max 1s in total */
                если (c >= 20)
                        вернуть -EIO;

                (void) usleep_safe(50 * USEC_PER_MSEC);
        }
}
