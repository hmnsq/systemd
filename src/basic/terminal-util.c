/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ <fcntl.h>
#અન્તર્ભાવ <linux/kd.h>
#અન્તર્ભાવ <linux/magic.h>
#અન્તર્ભાવ <linux/tiocl.h>
#અન્તર્ભાવ <linux/vt.h>
#અન્તર્ભાવ <poll.h>
#અન્તર્ભાવ <signal.h>
#અન્તર્ભાવ <stdlib.h>
#અન્તર્ભાવ <sys/inotify.h>
#અન્તર્ભાવ <sys/ioctl.h>
#અન્તર્ભાવ <sys/sysmacros.h>
#અન્તર્ભાવ <termios.h>
#અન્તર્ભાવ <time.h>
#અન્તર્ભાવ <unistd.h>

#અન્તર્ભાવ "alloc-util.h"
#અન્તર્ભાવ "ansi-color.h"
#અન્તર્ભાવ "chase.h"
#અન્તર્ભાવ "devnum-util.h"
#અન્તર્ભાવ "errno-util.h"
#અન્તર્ભાવ "extract-word.h"
#અન્તર્ભાવ "fd-util.h"
#અન્તર્ભાવ "fileio.h"
#અન્તર્ભાવ "fs-util.h"
#અન્તર્ભાવ "hexdecoct.h"
#અન્તર્ભાવ "inotify-util.h"
#અન્તર્ભાવ "io-util.h"
#અન્તર્ભાવ "log.h"
#અન્તર્ભાવ "namespace-util.h"
#અન્તર્ભાવ "parse-util.h"
#અન્તર્ભાવ "path-util.h"
#અન્તર્ભાવ "pidref.h"
#અન્તર્ભાવ "proc-cmdline.h"
#અન્તર્ભાવ "process-util.h"
#અન્તર્ભાવ "signal-util.h"
#અન્તર્ભાવ "socket-util.h"
#અન્તર્ભાવ "stat-util.h"
#અન્તર્ભાવ "stdio-util.h"
#અન્તર્ભાવ "string-util.h"
#અન્તર્ભાવ "strv.h"
#અન્તર્ભાવ "terminal-util.h"
#અન્તર્ભાવ "time-util.h"
#અન્તર્ભાવ "utf8.h"

/* How much to wait when reading/writing ANSI sequences from/to the console */
#define CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC (333 * USEC_PER_MSEC)

સ્થિર અસ્થિર unsigned cached_columns = 0;
સ્થિર અસ્થિર unsigned cached_લીટીઓ = 0;

સ્થિર અસ્થિર પૂર્ણાંક cached_on_tty = -1;
સ્થિર અસ્થિર પૂર્ણાંક cached_on_dev_null = -1;

bool isatty_safe(પૂર્ણાંક fd) {
        પ્રતિજ્ઞા(fd >= 0);

        જો (isatty(fd))
                પાછા true;

        /* Linux/glibc returns EIO for hung up TTY on isatty(). Which is wrong, the thing doesn't stop being
         * a TTY after all, just because it is temporarily hung up. Let's work around this here, until this
         * is fixed in glibc. See: https://sourceware.org/bugzilla/show_bug.cgi?id=32103 */
        જો (errno == EIO)
                પાછા true;

        /* Be resilient if we're working on stdio, since they're set up by parent process. */
        પ્રતિજ્ઞા(errno != EBADF || IN_SET(fd, STDIN_FILENO, STDOUT_FILENO, STDERR_FILENO));

        પાછા false;
}

પૂર્ણાંક chvt(પૂર્ણાંક vt) {
        _cleanup_close_ પૂર્ણાંક fd = -EBADF;

        /* Switch to the specified vt number. If the VT is specified <= 0 switch to the VT the kernel log messages go,
         * if that's configured. */

        fd = open_terminal("/dev/tty0", O_RDWR|O_NOCTTY|O_CLOEXEC|O_NONBLOCK);
        જો (fd < 0)
                પાછા fd;

        જો (vt <= 0) {
                પૂર્ણાંક tiocl[2] = {
                        TIOCL_GETKMSGREDIRECT,
                        0
                };

                જો (ioctl(fd, TIOCLINUX, tiocl) < 0)
                        પાછા -errno;

                vt = tiocl[0] <= 0 ? 1 : tiocl[0];
        }

        પાછા RET_NERRNO(ioctl(fd, VT_ACTIVATE, vt));
}

પૂર્ણાંક read_one_char(FILE *f, અક્ષર *ret, usec_t t, bool echo, bool *need_nl) {
        _cleanup_free_ અક્ષર *લીટી = ખાલી;
        સંરચના termios old_termios;
        પૂર્ણાંક r, fd;

        પ્રતિજ્ઞા(ret);

        જો (!f)
                f = stdin;

        /* If this is a terminal, then switch canonical mode off, so that we can read a single
         * character. (Note that fmemopen() streams do not have an fd associated with them, let's handle that
         * nicely.) If 'echo' is false we'll also disable ECHO mode so that the pressed key is not made
         * visible to the user. */
        fd = fileno(f);
        જો (fd >= 0 && tcgetattr(fd, &old_termios) >= 0) {
                સંરચના termios new_termios = old_termios;

                new_termios.c_lflag &= ~(ICANON|(echo ? 0 : ECHO));
                new_termios.c_cc[VMIN] = 1;
                new_termios.c_cc[VTIME] = 0;

                જો (tcsetattr(fd, TCSANOW, &new_termios) >= 0) {
                        અક્ષર c;

                        જો (t != USEC_INFINITY) {
                                જો (fd_wait_for_event(fd, POLLIN, t) <= 0) {
                                        (શૂન્ય) tcsetattr(fd, TCSANOW, &old_termios);
                                        પાછા -ETIMEDOUT;
                                }
                        }

                        r = safe_fgetc(f, &c);
                        (શૂન્ય) tcsetattr(fd, TCSANOW, &old_termios);
                        જો (r < 0)
                                પાછા r;
                        જો (r == 0)
                                પાછા -EIO;

                        જો (need_nl)
                                *need_nl = c != '\n';

                        *ret = c;
                        પાછા 0;
                }
        }

        જો (t != USEC_INFINITY && fd >= 0) {
                /* Let's wait the specified amount of time for input. When we have no fd we skip this, under
                 * the assumption that this is an fmemopen() stream or so where waiting doesn't make sense
                 * anyway, as the data is either already in the stream or cannot possible be placed there
                 * while we access the stream */

                જો (fd_wait_for_event(fd, POLLIN, t) <= 0)
                        પાછા -ETIMEDOUT;
        }

        /* If this is not a terminal, then read a full line instead */

        r = read_લીટી(f, 16, &લીટી); /* longer than necessary, to eat up UTF-8 chars/vt100 key sequences */
        જો (r < 0)
                પાછા r;
        જો (r == 0)
                પાછા -EIO;

        જો (strlen(લીટી) != 1)
                પાછા -EBADMSG;

        જો (need_nl)
                *need_nl = false;

        *ret = લીટી[0];
        પાછા 0;
}

#define DEFAULT_ASK_REFRESH_USEC (2*USEC_PER_SEC)

પૂર્ણાંક ask_char(અક્ષર *ret, અચલ અક્ષર *replies, અચલ અક્ષર *fmt, ...) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(ret);
        પ્રતિજ્ઞા(replies);
        પ્રતિજ્ઞા(fmt);

        માટે (;;) {
                va_list ap;
                અક્ષર c;
                bool need_nl = true;

                fputs(ansi_highlight(), stdout);

                putchar('\r');

                va_start(ap, fmt);
                vprintf(fmt, ap);
                va_end(ap);

                fputs(ansi_normal(), stdout);

                fflush(stdout);

                r = read_one_char(stdin, &c, DEFAULT_ASK_REFRESH_USEC, /* echo= */ true, &need_nl);
                જો (r < 0) {

                        જો (r == -ETIMEDOUT)
                                ચાલુ;

                        જો (r == -EBADMSG) {
                                puts("Bad input, please try again.");
                                ચાલુ;
                        }

                        putchar('\n');
                        પાછા r;
                }

                જો (need_nl)
                        putchar('\n');

                જો (strchr(replies, c)) {
                        *ret = c;
                        પાછા 0;
                }

                puts("Read unexpected character, please try again.");
        }
}

પ્રકારવ્યાખ્યા ગણના CompletionResult{
        COMPLETION_ALREADY,       /* the input string is already complete */
        COMPLETION_FULL,          /* completed the input string to be complete now */
        COMPLETION_PARTIAL,       /* completed the input string so that is still incomplete */
        COMPLETION_NONE,          /* found no matching completion */
        _COMPLETION_RESULT_MAX,
        _COMPLETION_RESULT_INVALID = -EINVAL,
        _COMPLETION_RESULT_ERRNO_MAX = -ERRNO_MAX,
} CompletionResult;

સ્થિર CompletionResult pick_completion(અચલ અક્ષર *string, અક્ષર *અચલ*completions, અક્ષર **ret) {
        _cleanup_free_ અક્ષર *found = ખાલી;
        bool partial = false;

        પ્રતિજ્ઞા(ret);

        string = strempty(string);

        STRV_FOREACH(c, completions) {

                /* Ignore entries that are not actually completions */
                જો (!startswith(*c, string))
                        ચાલુ;

                /* Store first completion that matches */
                જો (!found) {
                        found = strdup(*c);
                        જો (!found)
                                પાછા -ENOMEM;

                        ચાલુ;
                }

                /* If there's another completion that works truncate the one we already found by common
                 * prefix */
                size_t n = str_common_prefix(found, *c);
                જો (n == SIZE_MAX)
                        ચાલુ;

                found[n] = 0;
                partial = true;
        }

        *ret = TAKE_PTR(found);

        જો (!*ret)
                પાછા COMPLETION_NONE;
        જો (partial)
                પાછા COMPLETION_PARTIAL;

        પાછા streq(string, *ret) ? COMPLETION_ALREADY : COMPLETION_FULL;
}

સ્થિર શૂન્ય clear_by_backspace(size_t n) {
        /* Erase the specified number of character cells backwards on the terminal */
        માટે (size_t i = 0; i < n; i++)
                fputs("\b \b", stdout);
}

પૂર્ણાંક ask_string_full(
                અક્ષર **ret,
                અચલ અક્ષર *prefill,
                GetCompletionsCallback get_completions,
                શૂન્ય *userdata,
                અચલ અક્ષર *text, ...) {

        va_list ap;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(ret);
        પ્રતિજ્ઞા(text);

        _cleanup_free_ અક્ષર *string = ખાલી;
        size_t n = 0;

        જો (prefill) {
                /* Prefill query with explicit data if specified */

                string = strdup(prefill);
                જો (!string)
                        પાછા -ENOMEM;

                n = strlen(string);

        } નહીંતર જો (get_completions) {
                /* Otherwise, figure out what string to preselect the query with */
                _cleanup_strv_free_ અક્ષર **completions = ખાલી;
                r = get_completions("", GET_COMPLETIONS_PRESELECT, &completions, userdata);
                જો (r < 0)
                        પાછા r;

                CompletionResult cr = pick_completion(string, completions, &string);
                જો (cr < 0)
                        પાછા cr;

                n = strlen_ptr(string);
        }

        /* Output the prompt */
        fputs(ansi_highlight(), stdout);
        va_start(ap, text);
        vprintf(text, ap);
        va_end(ap);
        fputs(ansi_normal(), stdout);
        જો (string)
                fputs(string, stdout);
        fflush(stdout);

        /* Do interactive logic only if stdin + stdout are connected to the same place. And yes, we could use
         * STDIN_FILENO and STDOUT_FILENO here, but let's be overly correct for once, after all libc allows
         * swapping out stdin/stdout. */
        પૂર્ણાંક fd_આગત = fileno(stdin);
        પૂર્ણાંક fd_output = fileno(stdout);
        સંરચના termios old_termios = TERMIOS_NULL;
        CLEANUP_TERMIOS_RESET(fd_આગત, old_termios);

        જો (fd_આગત < 0 || fd_output < 0 || same_fd(fd_આગત, fd_output) <= 0)
                જાઓ fallback;

        /* Try to disable echo, which also tells us if this even is a terminal */
        જો (tcgetattr(fd_આગત, &old_termios) < 0) {
                old_termios = TERMIOS_NULL;
                જાઓ fallback;
        }

        સંરચના termios new_termios = old_termios;
        termios_disable_echo(&new_termios);
        જો (tcsetattr(fd_આગત, TCSANOW, &new_termios) < 0)
                પાછા -errno;

        માટે (;;) {
                પૂર્ણાંક c = fgetc(stdin);

                /* On EOF or NUL, end the request, don't output anything anymore */
                જો (IN_SET(c, EOF, 0))
                        તોડો;

                /* On Return also end the request, but make this visible */
                જો (IN_SET(c, '\n', '\r')) {
                        fputc('\n', stdout);
                        તોડો;
                }

                જો (c == '\t') {
                        /* Tab */

                        _cleanup_strv_free_ અક્ષર **completions = ખાલી;
                        જો (get_completions) {
                                r = get_completions(string, /* flags= */ 0, &completions, userdata);
                                જો (r < 0)
                                        પાછા r;
                        }

                        _cleanup_free_ અક્ષર *new_string = ખાલી;
                        CompletionResult cr = pick_completion(string, completions, &new_string);
                        જો (cr < 0)
                                પાછા cr;
                        જો (IN_SET(cr, COMPLETION_PARTIAL, COMPLETION_FULL)) {
                                /* Output the new suffix we learned */
                                fputs(ASSERT_PTR(startswith(new_string, strempty(string))), stdout);

                                /* And update the whole string */
                                free_and_replace(string, new_string);
                                n = strlen(string);
                        }
                        જો (cr == COMPLETION_NONE)
                                fputc('\a', stdout); /* BEL */

                        જો (IN_SET(cr, COMPLETION_PARTIAL, COMPLETION_ALREADY)) {
                                /* If this worked only partially, or if the user hit TAB even though we were
                                 * complete already, then show the remaining options (in the latter case just
                                 * the one). */
                                fputc('\n', stdout);

                                _cleanup_strv_free_ અક્ષર **filtered = strv_filter_prefix(completions, string);
                                જો (!filtered)
                                        પાછા -ENOMEM;

                                r = show_menu(filtered,
                                              /* n_columns= */ SIZE_MAX,
                                              /* column_width= */ SIZE_MAX,
                                              /* ellipsize_percentage= */ 0,
                                              /* grey_prefix= */ string,
                                              /* with_numbers= */ false);
                                જો (r < 0)
                                        પાછા r;

                                /* Show the prompt again */
                                fputs(ansi_highlight(), stdout);
                                va_start(ap, text);
                                vprintf(text, ap);
                                va_end(ap);
                                fputs(ansi_normal(), stdout);
                                fputs(string, stdout);
                        }

                } નહીંતર જો (IN_SET(c, '\b', 127)) {
                        /* Backspace */

                        જો (n == 0)
                                fputc('\a', stdout); /* BEL */
                        નહીંતર {
                                size_t m = utf8_last_length(string, n);

                                અક્ષર *e = string + n - m;
                                clear_by_backspace(utf8_console_width(e));

                                *e = 0;
                                n -= m;
                        }

                } નહીંતર જો (c == 21) {
                        /* Ctrl-u → erase all input */

                        clear_by_backspace(utf8_console_width(string));
                        જો (string)
                                string[n = 0] = 0;
                        નહીંતર
                                પ્રતિજ્ઞા(n == 0);

                } નહીંતર જો (c == 4) {
                        /* Ctrl-d → cancel this field input */

                        પાછા -ECANCELED;

                } નહીંતર જો (char_is_cc(c) || n >= LINE_MAX)
                        /* refuse control characters and too long strings */
                        fputc('\a', stdout); /* BEL */
                નહીંતર {
                        /* Regular char */

                        જો (!GREEDY_REALLOC(string, n+2))
                                પાછા -ENOMEM;

                        string[n++] = (અક્ષર) c;
                        string[n] = 0;

                        fputc(c, stdout);
                }

                fflush(stdout);
        }

        જો (!string) {
                string = strdup("");
                જો (!string)
                        પાછા -ENOMEM;
        }

        *ret = TAKE_PTR(string);
        પાછા 0;

fallback:
        /* A simple fallback without TTY magic */
        string = mfree(string);
        r = read_લીટી(stdin, LONG_લીટી_MAX, &string);
        જો (r < 0)
                પાછા r;
        જો (r == 0)
                પાછા -EIO;

        *ret = TAKE_PTR(string);
        પાછા 0;
}

bool any_ચાવી_to_proceed(શૂન્ય) {

        /* Insert a new line here as well as to when the user inputs, as this is also used during the boot up
         * sequence when status messages may be interleaved with the current program output. This ensures
         * that the status messages aren't appended on the same line as this message. */

        fputc('\n', stdout);
        fputs(ansi_highlight_magenta(), stdout);
        fputs("-- Press any key to proceed --", stdout);
        fputs(ansi_normal(), stdout);
        fputc('\n', stdout);
        fflush(stdout);

        અક્ષર ચાવી = 0;
        (શૂન્ય) read_one_char(stdin, &ચાવી, USEC_INFINITY, /* echo= */ false, /* need_nl= */ ખાલી);

        fputc('\n', stdout);
        fflush(stdout);

        પાછા ચાવી != 'q';
}

સ્થિર size_t widest_list_element(અક્ષર *અચલ*l) {
        size_t w = 0;

        /* Returns the largest console width of all elements in 'l' */

        STRV_FOREACH(i, l)
                w = MAX(w, utf8_console_width(*i));

        પાછા w;
}

પૂર્ણાંક show_menu(અક્ષર **x,
              size_t n_columns,
              size_t column_width,
              unsigned ellipsize_percentage,
              અચલ અક્ષર *grey_prefix,
              bool with_numbers) {

        પ્રતિજ્ઞા(n_columns > 0);
        પ્રતિજ્ઞા(column_width > 0);

        જો (n_columns == SIZE_MAX)
                n_columns = 3;

        જો (column_width == SIZE_MAX) {
                size_t widest = widest_list_element(x);

                /* If not specified, derive column width from screen width */
                size_t column_max = (columns()-1) / n_columns;

                /* Subtract room for numbers */
                જો (with_numbers)
                        column_max = LESS_BY(column_max, 6U);

                /* If columns would get too tight let's make this a linear list instead. */
                જો (column_max < 10 && widest > 10) {
                        n_columns = 1;
                        column_max = columns()-1;

                        જો (with_numbers)
                                column_max = LESS_BY(column_max, 6U);
                }

                /* Never make this narrower than 10 characters */
                column_max = MAX(column_max, 10U);
                column_width = CLAMP(widest+1, 10U, column_max);
        }

        size_t n = strv_length(x);
        size_t per_column = DIV_ROUND_UP(n, n_columns);

        size_t break_લીટીઓ = લીટીઓ();
        જો (break_લીટીઓ > 2)
                break_લીટીઓ--;

        /* The first page gets two extra lines, since we want to show a title */
        size_t break_modulo = break_લીટીઓ;
        જો (break_modulo > 3)
                break_modulo -= 3;

        માટે (size_t i = 0; i < per_column; i++) {

                માટે (size_t j = 0; j < n_columns; j++) {
                        _cleanup_free_ અક્ષર *e = ખાલી;

                        size_t p = j * per_column + i;
                        જો (p >= n)
                                તોડો;

                        e = ellipsize(x[p], column_width, ellipsize_percentage);
                        જો (!e)
                                પાછા -ENOMEM;

                        જો (with_numbers)
                                printf("%s%4zu)%s ",
                                       ansi_grey(),
                                       p + 1,
                                       ansi_normal());

                        જો (grey_prefix && startswith(e, grey_prefix)) {
                                size_t k = MIN(strlen(grey_prefix), column_width);
                                printf("%s%.*s%s",
                                       ansi_grey(),
                                       (પૂર્ણાંક) k, e,
                                       ansi_normal());
                                printf("%-*s",
                                       (પૂર્ણાંક) (column_width - k), e+k);
                        } નહીંતર
                                printf("%-*s", (પૂર્ણાંક) column_width, e);
                }

                putchar('\n');

                /* on the first screen we reserve 2 extra lines for the title */
                જો (i % break_લીટીઓ == break_modulo)
                        જો (!any_ચાવી_to_proceed())
                                પાછા 0;
        }

        પાછા 0;
}

પૂર્ણાંક open_terminal(અચલ અક્ષર *name, પૂર્ણાંક mode) {
        _cleanup_close_ પૂર્ણાંક fd = -EBADF;

        /*
         * If a TTY is in the process of being closed opening it might cause EIO. This is horribly awful, but
         * unlikely to be changed in the kernel. Hence we work around this problem by retrying a couple of
         * times.
         *
         * https://bugs.launchpad.net/ubuntu/+source/linux/+bug/554172/comments/245
         */

        પ્રતિજ્ઞા((mode & (O_CREAT|O_PATH|O_DIRECTORY|O_TMPFILE)) == 0);

        માટે (unsigned c = 0;; c++) {
                fd = open(name, mode, 0);
                જો (fd >= 0)
                        તોડો;

                જો (errno != EIO)
                        પાછા -errno;

                /* Max 1s in total */
                જો (c >= 20)
                        પાછા -EIO;

                (શૂન્ય) usleep_safe(50 * USEC_PER_MSEC);
        }

        જો (!isatty_safe(fd))
                પાછા -ENOTTY;

        પાછા TAKE_FD(fd);
}

પૂર્ણાંક acquire_terminal(
                અચલ અક્ષર *name,
                AcquireTerminalFlags flags,
                usec_t timeout) {

        _cleanup_close_ પૂર્ણાંક notify = -EBADF, fd = -EBADF;
        usec_t ts = USEC_INFINITY;
        પૂર્ણાંક r, wd = -1;

        પ્રતિજ્ઞા(name);

        AcquireTerminalFlags mode = flags & _ACQUIRE_TERMINAL_MODE_MASK;
        પ્રતિજ્ઞા(IN_SET(mode, ACQUIRE_TERMINAL_TRY, ACQUIRE_TERMINAL_FORCE, ACQUIRE_TERMINAL_WAIT));
        પ્રતિજ્ઞા(mode == ACQUIRE_TERMINAL_WAIT || !FLAGS_SET(flags, ACQUIRE_TERMINAL_WATCH_SIGTERM));

        /* We use inotify to be notified when the tty is closed. We create the watch before checking if we can actually
         * acquire it, so that we don't lose any event.
         *
         * Note: strictly speaking this actually watches for the device being closed, it does *not* really watch
         * whether a tty loses its controlling process. However, unless some rogue process uses TIOCNOTTY on /dev/tty
         * *after* closing its tty otherwise this will not become a problem. As long as the administrator makes sure to
         * not configure any service on the same tty as an untrusted user this should not be a problem. (Which they
         * probably should not do anyway.) */

        જો (mode == ACQUIRE_TERMINAL_WAIT) {
                notify = inotify_init1(IN_CLOEXEC | IN_NONBLOCK);
                જો (notify < 0)
                        પાછા -errno;

                wd = inotify_add_watch(notify, name, IN_CLOSE);
                જો (wd < 0)
                        પાછા -errno;

                જો (timeout != USEC_INFINITY)
                        ts = now(CLOCK_MONOTONIC);
        }

        /* If we are called with ACQUIRE_TERMINAL_WATCH_SIGTERM we'll unblock SIGTERM during ppoll() temporarily */
        sigset_t poll_ss;
        assert_se(sigprocmask(SIG_SETMASK, /* newset= */ ખાલી, &poll_ss) >= 0);
        જો (flags & ACQUIRE_TERMINAL_WATCH_SIGTERM) {
                assert_se(sigismember(&poll_ss, SIGTERM) > 0);
                assert_se(sigdelset(&poll_ss, SIGTERM) >= 0);
        }

        માટે (;;) {
                જો (notify >= 0) {
                        r = flush_fd(notify);
                        જો (r < 0)
                                પાછા r;
                }

                /* We pass here O_NOCTTY only so that we can check the return value TIOCSCTTY and have a reliable way
                 * to figure out if we successfully became the controlling process of the tty */
                fd = open_terminal(name, O_RDWR|O_NOCTTY|O_CLOEXEC);
                જો (fd < 0)
                        પાછા fd;

                /* Temporarily ignore SIGHUP, so that we don't get SIGHUP'ed if we already own the tty. */
                સંરચના sigaction sa_old;
                assert_se(sigaction(SIGHUP, &sigaction_ignore, &sa_old) >= 0);

                /* First, try to get the tty */
                r = RET_NERRNO(ioctl(fd, TIOCSCTTY, mode == ACQUIRE_TERMINAL_FORCE));

                /* Reset signal handler to old value */
                assert_se(sigaction(SIGHUP, &sa_old, ખાલી) >= 0);

                /* Success? Exit the loop now! */
                જો (r >= 0)
                        તોડો;

                /* Any failure besides -EPERM? Fail, regardless of the mode. */
                જો (r != -EPERM)
                        પાછા r;

                જો (flags & ACQUIRE_TERMINAL_PERMISSIVE) /* If we are in permissive mode, then EPERM is fine, turn this
                                                          * into a success. Note that EPERM is also returned if we
                                                          * already are the owner of the TTY. */
                        તોડો;

                જો (mode != ACQUIRE_TERMINAL_WAIT) /* If we are in TRY or FORCE mode, then propagate EPERM as EPERM */
                        પાછા r;

                પ્રતિજ્ઞા(notify >= 0);
                પ્રતિજ્ઞા(wd >= 0);

                માટે (;;) {
                        usec_t ડાબું;
                        જો (timeout == USEC_INFINITY)
                                ડાબું = USEC_INFINITY;
                        નહીંતર {
                                પ્રતિજ્ઞા(ts != USEC_INFINITY);

                                usec_t n = usec_sub_unsigned(now(CLOCK_MONOTONIC), ts);
                                જો (n >= timeout)
                                        પાછા -ETIMEDOUT;

                                ડાબું = timeout - n;
                        }

                        r = ppoll_usec_full(
                                        &(સંરચના pollfd) {
                                                .fd = notify,
                                                .events = POLLIN,
                                        },
                                        /* n_fds= */ 1,
                                        ડાબું,
                                        &poll_ss);
                        જો (r < 0)
                                પાછા r;
                        જો (r == 0)
                                પાછા -ETIMEDOUT;

                        સંઘ inotify_event_buffer buffer;
                        ssize_t l;
                        l = પઠ(notify, &buffer, કદ(buffer));
                        જો (l < 0) {
                                જો (ERRNO_IS_TRANSIENT(errno))
                                        ચાલુ;

                                પાછા -errno;
                        }

                        FOREACH_INOTIFY_EVENT(e, buffer, l) {
                                જો (e->mask & IN_Q_OVERFLOW) /* If we hit an inotify queue overflow, simply check if the terminal is up for grabs now. */
                                        તોડો;

                                જો (e->wd != wd || !(e->mask & IN_CLOSE)) /* Safety checks */
                                        પાછા -EIO;
                        }

                        તોડો;
                }

                /* We close the tty fd here since if the old session ended our handle will be dead. It's important that
                 * we do this after sleeping, so that we don't enter an endless loop. */
                fd = safe_close(fd);
        }

        પાછા TAKE_FD(fd);
}

પૂર્ણાંક release_terminal(શૂન્ય) {
        _cleanup_close_ પૂર્ણાંક fd = -EBADF;
        પૂર્ણાંક r;

        fd = open("/dev/tty", O_RDWR|O_NOCTTY|O_CLOEXEC|O_NONBLOCK);
        જો (fd < 0)
                પાછા -errno;

        /* Temporarily ignore SIGHUP, so that we don't get SIGHUP'ed
         * by our own TIOCNOTTY */
        સંરચના sigaction sa_old;
        assert_se(sigaction(SIGHUP, &sigaction_ignore, &sa_old) >= 0);

        r = RET_NERRNO(ioctl(fd, TIOCNOTTY));

        assert_se(sigaction(SIGHUP, &sa_old, ખાલી) >= 0);

        પાછા r;
}

પૂર્ણાંક terminal_new_session(શૂન્ય) {

        /* Make us the new session leader, and set stdin tty to be our controlling terminal.
         *
         * Why stdin? Well, the ctty logic is relevant for signal delivery mostly, i.e. if people hit C-c
         * or the line is hung up. Such events are basically just a form of input, via a side channel
         * (that side channel being signal delivery, i.e. SIGINT, SIGHUP et al). Hence we focus on input,
         * not output here. */

        જો (!isatty_safe(STDIN_FILENO))
                પાછા -ENXIO;

        (શૂન્ય) setsid();
        પાછા RET_NERRNO(ioctl(STDIN_FILENO, TIOCSCTTY, 0));
}

શૂન્ય terminal_detach_session(શૂન્ય) {
        (શૂન્ય) setsid();
        (શૂન્ય) release_terminal();
}

પૂર્ણાંક terminal_vhangup_fd(પૂર્ણાંક fd) {
        પ્રતિજ્ઞા(fd >= 0);
        પાછા RET_NERRNO(ioctl(fd, TIOCVHANGUP));
}

પૂર્ણાંક terminal_vhangup(અચલ અક્ષર *tty) {
        _cleanup_close_ પૂર્ણાંક fd = -EBADF;

        પ્રતિજ્ઞા(tty);

        fd = open_terminal(tty, O_RDWR|O_NOCTTY|O_CLOEXEC);
        જો (fd < 0)
                પાછા fd;

        પાછા terminal_vhangup_fd(fd);
}

પૂર્ણાંક vt_disallocate(અચલ અક્ષર *tty_path) {
        પ્રતિજ્ઞા(tty_path);

        /* Deallocate the VT if possible. If not possible (i.e. because it is the active one), at least clear
         * it entirely (including the scrollback buffer). */

        પૂર્ણાંક ttynr = vtnr_from_tty(tty_path);
        જો (ttynr > 0) {
                _cleanup_close_ પૂર્ણાંક fd = open_terminal("/dev/tty0", O_RDWR|O_NOCTTY|O_CLOEXEC|O_NONBLOCK);
                જો (fd < 0)
                        પાછા fd;

                /* Try to deallocate */
                જો (ioctl(fd, VT_DISALLOCATE, ttynr) >= 0)
                        પાછા 0;
                જો (errno != EBUSY)
                        પાછા -errno;
        }

        /* So this is not a VT (in which case we cannot deallocate it), or we failed to deallocate. Let's at
         * least clear the screen. */

        _cleanup_close_ પૂર્ણાંક fd2 = open_terminal(tty_path, O_WRONLY|O_NOCTTY|O_CLOEXEC|O_NONBLOCK);
        જો (fd2 < 0)
                પાછા fd2;

        પાછા loop_write_full(fd2,
                               "\033[r"   /* clear scrolling region */
                               "\033[H"   /* move home */
                               "\033[3J"  /* clear screen including scrollback, requires Linux 2.6.40 */
                               "\033c",   /* reset to initial state */
                               SIZE_MAX,
                               CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
}

સ્થિર પૂર્ણાંક vt_default_utf8(શૂન્ય) {
        /* Read the default VT UTF8 setting from the kernel */
        પાછા read_boolean_file("/sys/module/vt/parameters/default_utf8");
}

સ્થિર પૂર્ણાંક vt_reset_keyboard(પૂર્ણાંક fd) {
        પૂર્ણાંક r, kb;

        પ્રતિજ્ઞા(fd >= 0);

        /* If we can't read the default, then default to Unicode. It's 2024 after all. */
        r = vt_default_utf8();
        જો (r < 0)
                log_debug_errno(r, "Failed to determine kernel VT UTF-8 mode, assuming enabled: %m");

        kb = vt_default_utf8() != 0 ? K_UNICODE : K_XLATE;
        પાછા RET_NERRNO(ioctl(fd, KDSKBMODE, kb));
}

સ્થિર પૂર્ણાંક terminal_reset_ioctl(પૂર્ણાંક fd, bool switch_to_text) {
        સંરચના termios termios;
        પૂર્ણાંક r;

        /* Set terminal to some sane defaults */

        પ્રતિજ્ઞા(fd >= 0);

        /* We leave locked terminal attributes untouched, so that Plymouth may set whatever it wants to set,
         * and we don't interfere with that. */

        /* Disable exclusive mode, just in case */
        જો (ioctl(fd, TIOCNXCL) < 0)
                log_debug_errno(errno, "TIOCNXCL ioctl failed on TTY, ignoring: %m");

        /* Switch to text mode */
        જો (switch_to_text)
                જો (ioctl(fd, KDSETMODE, KD_TEXT) < 0)
                        log_debug_errno(errno, "KDSETMODE ioctl for switching to text mode failed on TTY, ignoring: %m");

        /* Set default keyboard mode */
        r = vt_reset_keyboard(fd);
        જો (r < 0)
                log_debug_errno(r, "Failed to reset VT keyboard, ignoring: %m");

        જો (tcgetattr(fd, &termios) < 0) {
                r = log_debug_errno(errno, "Failed to get terminal parameters: %m");
                જાઓ finish;
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
        જો (r < 0)
                log_debug_errno(r, "Failed to set terminal parameters: %m");

finish:
        /* Just in case, flush all crap out */
        (શૂન્ય) tcflush(fd, TCIOFLUSH);

        પાછા r;
}

પૂર્ણાંક terminal_reset_ansi_seq(પૂર્ણાંક fd) {
        _cleanup_(nonblock_resetp) પૂર્ણાંક nonblock_reset = -EBADF;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(fd >= 0);

        જો (getenv_terminal_is_dumb())
                પાછા 0;

        r = fd_nonblock(fd, true);
        જો (r < 0)
                પાછા log_debug_errno(r, "Failed to set terminal to non-blocking mode: %m");
        જો (r > 0)
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
        જો (r < 0)
                log_debug_errno(r, "Failed to reset terminal through ANSI sequences: %m");

        પાછા r;
}

શૂન્ય reset_dev_console_fd(પૂર્ણાંક fd, bool switch_to_text) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(fd >= 0);

        _cleanup_close_ પૂર્ણાંક lock_fd = lock_dev_console();
        જો (lock_fd < 0)
                log_debug_errno(lock_fd, "Failed to lock /dev/console, ignoring: %m");

        r = terminal_reset_ioctl(fd, switch_to_text);
        જો (r < 0)
                log_warning_errno(r, "Failed to reset /dev/console, ignoring: %m");

        unsigned rows, cols;
        r = proc_cmdline_tty_size("/dev/console", &rows, &cols);
        જો (r < 0)
                log_warning_errno(r, "Failed to get /dev/console size, ignoring: %m");
        નહીંતર જો (r > 0) {
                r = terminal_set_size_fd(fd, ખાલી, rows, cols);
                જો (r < 0)
                        log_warning_errno(r, "Failed to set configured terminal size on /dev/console, ignoring: %m");
        } નહીંતર
                (શૂન્ય) terminal_fix_size(fd, fd);

        r = terminal_reset_ansi_seq(fd);
        જો (r < 0)
                log_warning_errno(r, "Failed to reset /dev/console using ANSI sequences, ignoring: %m");
}

પૂર્ણાંક lock_dev_console(શૂન્ય) {
        _cleanup_close_ પૂર્ણાંક fd = -EBADF;
        પૂર્ણાંક r;

        /* NB: We do not use O_NOFOLLOW here, because some container managers might place a symlink to some
         * pty in /dev/console, in which case it should be fine to lock the target TTY. */
        fd = open_terminal("/dev/console", O_RDONLY|O_CLOEXEC|O_NOCTTY);
        જો (fd < 0)
                પાછા fd;

        r = lock_generic(fd, LOCK_BSD, LOCK_EX);
        જો (r < 0)
                પાછા r;

        પાછા TAKE_FD(fd);
}

પૂર્ણાંક make_console_stdio(bool switch_to_text) {
        પૂર્ણાંક fd, r;

        /* Make /dev/console the controlling terminal and stdin/stdout/stderr, if we can. If we can't use
         * /dev/null instead. This is particularly useful if /dev/console is turned off, e.g. if console=null
         * is specified on the kernel command line. */

        fd = acquire_terminal("/dev/console", ACQUIRE_TERMINAL_FORCE|ACQUIRE_TERMINAL_PERMISSIVE, USEC_INFINITY);
        જો (fd < 0) {
                log_warning_errno(fd, "Failed to acquire terminal, using /dev/null stdin/stdout/stderr instead: %m");

                r = make_null_stdio();
                જો (r < 0)
                        પાછા log_error_errno(r, "Failed to make /dev/null stdin/stdout/stderr: %m");

        } નહીંતર {
                reset_dev_console_fd(fd, switch_to_text);

                r = rearrange_stdio(fd, fd, fd); /* This invalidates 'fd' both on success and on failure. */
                જો (r < 0)
                        પાછા log_error_errno(r, "Failed to make terminal stdin/stdout/stderr: %m");
        }

        reset_terminal_feature_caches();
        પાછા 0;
}

સ્થિર પૂર્ણાંક vtnr_from_tty_raw(અચલ અક્ષર *tty, unsigned *ret) {
        પ્રતિજ્ઞા(tty);

        tty = skip_dev_prefix(tty);

        અચલ અક્ષર *e = startswith(tty, "tty");
        જો (!e)
                પાછા -EINVAL;

        પાછા safe_atou(e, ret);
}

પૂર્ણાંક vtnr_from_tty(અચલ અક્ષર *tty) {
        unsigned u;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(tty);

        r = vtnr_from_tty_raw(tty, &u);
        જો (r < 0)
                પાછા r;
        જો (!vtnr_is_valid(u))
                પાછા -ERANGE;

        પાછા (પૂર્ણાંક) u;
}

bool tty_is_vc(અચલ અક્ષર *tty) {
        પ્રતિજ્ઞા(tty);

        /* NB: for >= 0 values no range check is conducted here, on the assumption that the caller will
         * either extract vtnr through vtnr_from_tty() later where ERANGE would be reported, or doesn't care
         * about whether it's strictly valid, but only asking "does this fall into the vt category?", for which
         * "yes" seems to be a better answer. */

        પાછા vtnr_from_tty_raw(tty, /* ret= */ ખાલી) >= 0;
}

bool tty_is_console(અચલ અક્ષર *tty) {
        પ્રતિજ્ઞા(tty);

        પાછા streq(skip_dev_prefix(tty), "console");
}

પૂર્ણાંક resolve_dev_console(અક્ષર **ret) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(ret);

        /* Resolve where /dev/console is pointing to. If /dev/console is a symlink (like in container
         * managers), we'll just resolve the symlink. If it's a real device node, we'll use if
         * /sys/class/tty/tty0/active, but only if /sys/ is actually ours (i.e. not read-only-mounted which
         * is a sign for container setups). */

        _cleanup_free_ અક્ષર *chased = ખાલી;
        r = chase("/dev/console", /* root= */ ખાલી, /* flags= */ 0, &chased, /* ret_fd= */ ખાલી);
        જો (r < 0)
                પાછા r;
        જો (!path_equal(chased, "/dev/console")) {
                *ret = TAKE_PTR(chased);
                પાછા 0;
        }

        r = path_is_read_only_fs("/sys");
        જો (r < 0)
                પાછા r;
        જો (r > 0)
                પાછા -ENOMEDIUM;

        _cleanup_free_ અક્ષર *active = ખાલી;
        r = read_one_લીટી_file("/sys/class/tty/console/active", &active);
        જો (r < 0)
                પાછા r;
        જો (r == 0)
                પાછા -ENXIO;

        /* If multiple log outputs are configured the last one is what /dev/console points to */
        અચલ અક્ષર *tty = strrchr(active, ' ');
        જો (tty)
                tty++;
        નહીંતર
                tty = active;

        જો (streq(tty, "tty0")) {
                active = mfree(active);

                /* Get the active VC (e.g. tty1) */
                r = read_one_લીટી_file("/sys/class/tty/tty0/active", &active);
                જો (r < 0)
                        પાછા r;
                જો (r == 0)
                        પાછા -ENXIO;

                tty = active;
        }

        _cleanup_free_ અક્ષર *path = ખાલી;
        path = path_join("/dev", tty);
        જો (!path)
                પાછા -ENOMEM;

        *ret = TAKE_PTR(path);
        પાછા 0;
}

પૂર્ણાંક get_kernel_consoles(અક્ષર ***ret) {
        _cleanup_strv_free_ અક્ષર **l = ખાલી;
        _cleanup_free_ અક્ષર *લીટી = ખાલી;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(ret);

        /* If /sys/ is mounted read-only this means we are running in some kind of container environment.
         * In that case /sys/ would reflect the host system, not us, hence ignore the data we can read from it. */
        જો (path_is_read_only_fs("/sys") > 0)
                જાઓ fallback;

        r = read_one_લીટી_file("/sys/class/tty/console/active", &લીટી);
        જો (r < 0)
                પાછા r;

        માટે (અચલ અક્ષર *p = લીટી;;) {
                _cleanup_free_ અક્ષર *tty = ખાલી, *path = ખાલી;

                r = extract_first_word(&p, &tty, ખાલી, 0);
                જો (r < 0)
                        પાછા r;
                જો (r == 0)
                        તોડો;

                જો (streq(tty, "tty0")) {
                        tty = mfree(tty);
                        r = read_one_લીટી_file("/sys/class/tty/tty0/active", &tty);
                        જો (r < 0)
                                પાછા r;
                        જો (r == 0) {
                                log_debug("No VT active, skipping /dev/tty0.");
                                ચાલુ;
                        }
                }

                path = path_join("/dev", tty);
                જો (!path)
                        પાછા -ENOMEM;

                જો (access(path, F_OK) < 0) {
                        log_debug_errno(errno, "Console device %s is not accessible, skipping: %m", path);
                        ચાલુ;
                }

                r = strv_consume(&l, TAKE_PTR(path));
                જો (r < 0)
                        પાછા r;
        }

        જો (strv_isempty(l)) {
                log_debug("No devices found for system console");
                જાઓ fallback;
        }

        *ret = TAKE_PTR(l);
        પાછા strv_length(*ret);

fallback:
        r = strv_extend(&l, "/dev/console");
        જો (r < 0)
                પાછા r;

        *ret = TAKE_PTR(l);
        પાછા 0;
}

bool tty_is_vc_resolve(અચલ અક્ષર *tty) {
        _cleanup_free_ અક્ષર *resolved = ખાલી;

        પ્રતિજ્ઞા(tty);

        જો (streq(skip_dev_prefix(tty), "console")) {
                જો (resolve_dev_console(&resolved) < 0)
                        પાછા false;

                tty = resolved;
        }

        પાછા tty_is_vc(tty);
}

પૂર્ણાંક fd_columns(પૂર્ણાંક fd) {
        સંરચના winsize ws = {};

        જો (fd < 0)
                પાછા -EBADF;

        જો (ioctl(fd, TIOCGWINSZ, &ws) < 0)
                પાછા -errno;

        જો (ws.ws_col <= 0)
                પાછા -ENODATA; /* some tty types come up with invalid row/column initially, return a recognizable error for that */

        પાછા ws.ws_col;
}

પૂર્ણાંક getenv_columns(શૂન્ય) {
        પૂર્ણાંક r;

        અચલ અક્ષર *e = getenv("COLUMNS");
        જો (!e)
                પાછા -ENXIO;

        unsigned c;
        r = safe_atou_bounded(e, 1, USHRT_MAX, &c);
        જો (r < 0)
                પાછા r;

        પાછા (પૂર્ણાંક) c;
}

unsigned columns(શૂન્ય) {

        જો (cached_columns > 0)
                પાછા cached_columns;

        પૂર્ણાંક c = getenv_columns();
        જો (c < 0) {
                c = fd_columns(STDOUT_FILENO);
                જો (c < 0)
                        c = 80;
        }

        પ્રતિજ્ઞા(c > 0);

        cached_columns = c;
        પાછા cached_columns;
}

પૂર્ણાંક fd_લીટીઓ(પૂર્ણાંક fd) {
        સંરચના winsize ws = {};

        જો (fd < 0)
                પાછા -EBADF;

        જો (ioctl(fd, TIOCGWINSZ, &ws) < 0)
                પાછા -errno;

        જો (ws.ws_row <= 0)
                પાછા -ENODATA; /* some tty types come up with invalid row/column initially, return a recognizable error for that */

        પાછા ws.ws_row;
}

unsigned લીટીઓ(શૂન્ય) {
        અચલ અક્ષર *e;
        પૂર્ણાંક l;

        જો (cached_લીટીઓ > 0)
                પાછા cached_લીટીઓ;

        l = 0;
        e = getenv("LINES");
        જો (e)
                (શૂન્ય) safe_atoi(e, &l);

        જો (l <= 0 || l > USHRT_MAX) {
                l = fd_લીટીઓ(STDOUT_FILENO);
                જો (l <= 0)
                        l = 24;
        }

        cached_લીટીઓ = l;
        પાછા cached_લીટીઓ;
}

પૂર્ણાંક terminal_set_size_fd(પૂર્ણાંક fd, અચલ અક્ષર *ident, unsigned rows, unsigned cols) {
        સંરચના winsize ws;

        પ્રતિજ્ઞા(fd >= 0);

        જો (!ident)
                ident = "TTY";

        જો (rows == UINT_MAX && cols == UINT_MAX)
                પાછા 0;

        જો (ioctl(fd, TIOCGWINSZ, &ws) < 0)
                પાછા log_debug_errno(errno,
                                       "TIOCGWINSZ ioctl for getting %s size failed, not setting terminal size: %m",
                                       ident);

        જો (rows == UINT_MAX)
                rows = ws.ws_row;
        નહીંતર જો (rows > USHRT_MAX)
                rows = USHRT_MAX;

        જો (cols == UINT_MAX)
                cols = ws.ws_col;
        નહીંતર જો (cols > USHRT_MAX)
                cols = USHRT_MAX;

        જો (rows == ws.ws_row && cols == ws.ws_col)
                પાછા 0;

        ws.ws_row = rows;
        ws.ws_col = cols;

        જો (ioctl(fd, TIOCSWINSZ, &ws) < 0)
                પાછા log_debug_errno(errno, "TIOCSWINSZ ioctl for setting %s size failed: %m", ident);

        પાછા 0;
}

પૂર્ણાંક proc_cmdline_tty_size(અચલ અક્ષર *tty, unsigned *ret_rows, unsigned *ret_cols) {
        _cleanup_free_ અક્ષર *rowskey = ખાલી, *rowsvalue = ખાલી, *colskey = ખાલી, *colsvalue = ખાલી;
        unsigned rows = UINT_MAX, cols = UINT_MAX;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(tty);

        જો (!ret_rows && !ret_cols)
                પાછા 0;

        tty = skip_dev_prefix(tty);
        જો (path_startswith(tty, "pts/"))
                પાછા -EMEDIUMTYPE;
        જો (!in_charset(tty, ALPHANUMERICAL))
                પાછા log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "TTY name '%s' contains non-alphanumeric characters, not searching kernel cmdline for size.", tty);

        rowskey = strjoin("systemd.tty.rows.", tty);
        જો (!rowskey)
                પાછા -ENOMEM;

        colskey = strjoin("systemd.tty.columns.", tty);
        જો (!colskey)
                પાછા -ENOMEM;

        r = proc_cmdline_get_ચાવી_many(/* flags= */ 0,
                                      rowskey, &rowsvalue,
                                      colskey, &colsvalue);
        જો (r < 0)
                પાછા log_debug_errno(r, "Failed to read TTY size of %s from kernel cmdline: %m", tty);

        જો (rowsvalue) {
                r = safe_atou(rowsvalue, &rows);
                જો (r < 0)
                        પાછા log_debug_errno(r, "Failed to parse %s=%s: %m", rowskey, rowsvalue);
        }

        જો (colsvalue) {
                r = safe_atou(colsvalue, &cols);
                જો (r < 0)
                        પાછા log_debug_errno(r, "Failed to parse %s=%s: %m", colskey, colsvalue);
        }

        જો (ret_rows)
                *ret_rows = rows;
        જો (ret_cols)
                *ret_cols = cols;

        પાછા rows != UINT_MAX || cols != UINT_MAX;
}

/* intended to be used as a SIGWINCH sighandler */
શૂન્ય columns_લીટીઓ_cache_reset(પૂર્ણાંક signum) {
        cached_columns = 0;
        cached_લીટીઓ = 0;
}

શૂન્ય reset_terminal_feature_caches(શૂન્ય) {
        cached_columns = 0;
        cached_લીટીઓ = 0;

        cached_on_tty = -1;
        cached_on_dev_null = -1;

        reset_ansi_feature_caches();
}

bool on_tty(શૂન્ય) {

        /* We check both stdout and stderr, so that situations where pipes on the shell are used are reliably
         * recognized, regardless if only the output or the errors are piped to some place. Since on_tty() is generally
         * used to default to a safer, non-interactive, non-color mode of operation it's probably good to be defensive
         * here, and check for both. Note that we don't check for STDIN_FILENO, because it should fine to use fancy
         * terminal functionality when outputting stuff, even if the input is piped to us. */

        જો (cached_on_tty < 0)
                cached_on_tty =
                        isatty_safe(STDOUT_FILENO) &&
                        isatty_safe(STDERR_FILENO);

        પાછા cached_on_tty;
}

પૂર્ણાંક getttyname_malloc(પૂર્ણાંક fd, અક્ષર **ret) {
        અક્ષર path[PATH_MAX]; /* PATH_MAX is counted *with* the trailing NUL byte */
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(fd >= 0);
        પ્રતિજ્ઞા(ret);

        r = ttyname_r(fd, path, કદ path); /* positive error */
        પ્રતિજ્ઞા(r >= 0);
        જો (r == ERANGE)
                પાછા -ENAMETOOLONG;
        જો (r > 0)
                પાછા -r;

        પાછા strdup_to(ret, skip_dev_prefix(path));
}

પૂર્ણાંક getttyname_harder(પૂર્ણાંક fd, અક્ષર **ret) {
        _cleanup_free_ અક્ષર *s = ખાલી;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(ret);

        r = getttyname_malloc(fd, &s);
        જો (r < 0)
                પાછા r;

        જો (streq(s, "tty"))
                પાછા get_ctty(0, ખાલી, ret);

        *ret = TAKE_PTR(s);
        પાછા 0;
}

પૂર્ણાંક get_ctty_devnr(pid_t pid, dev_t *ret) {
        _cleanup_free_ અક્ષર *લીટી = ખાલી;
        unsigned long ttynr;
        અચલ અક્ષર *p;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(pid >= 0);

        p = procfs_file_alloca(pid, "stat");
        r = read_one_લીટી_file(p, &લીટી);
        જો (r < 0)
                પાછા r;

        p = strrchr(લીટી, ')');
        જો (!p)
                પાછા -EIO;

        p++;

        જો (sscanf(p, " "
                   "%*c "  /* state */
                   "%*d "  /* ppid */
                   "%*d "  /* pgrp */
                   "%*d "  /* session */
                   "%lu ", /* ttynr */
                   &ttynr) != 1)
                પાછા -EIO;

        જો (devnum_is_zero(ttynr))
                પાછા -ENXIO;

        જો (ret)
                *ret = (dev_t) ttynr;

        પાછા 0;
}

પૂર્ણાંક get_ctty(pid_t pid, dev_t *ret_devnr, અક્ષર **ret) {
        અક્ષર pty[STRLEN("/dev/pts/") + DECIMAL_STR_MAX(dev_t) + 1];
        _cleanup_free_ અક્ષર *buf = ખાલી;
        અચલ અક્ષર *fn = ખાલી, *w;
        dev_t devnr;
        પૂર્ણાંક r;

        r = get_ctty_devnr(pid, &devnr);
        જો (r < 0)
                પાછા r;

        r = device_path_make_canonical(S_IFCHR, devnr, &buf);
        જો (r < 0) {
                સંરચના stat st;

                જો (r != -ENOENT) /* No symlink for this in /dev/char/? */
                        પાછા r;

                /* Maybe this is PTY? PTY devices are not listed in /dev/char/, as they don't follow the
                 * Linux device model and hence device_path_make_canonical() doesn't work for them. Let's
                 * assume this is a PTY for a moment, and check if the device node this would then map to in
                 * /dev/pts/ matches the one we are looking for. This way we don't have to hardcode the major
                 * number (which is 136 btw), but we still rely on the fact that PTY numbers map directly to
                 * the minor number of the pty. */
                xsprintf(pty, "/dev/pts/%u", minor(devnr));

                જો (stat(pty, &st) < 0) {
                        જો (errno != ENOENT)
                                પાછા -errno;

                } નહીંતર જો (S_ISCHR(st.st_mode) && devnr == st.st_rdev) /* Bingo! */
                        fn = pty;

                જો (!fn) {
                        /* Doesn't exist, or not a PTY? Probably something similar to the PTYs which have no
                         * symlink in /dev/char/. Let's return something vaguely useful. */
                        r = device_path_make_major_minor(S_IFCHR, devnr, &buf);
                        જો (r < 0)
                                પાછા r;

                        fn = buf;
                }
        } નહીંતર
                fn = buf;

        w = path_startswith(fn, "/dev/");
        જો (!w)
                પાછા -EINVAL;

        જો (ret) {
                r = strdup_to(ret, w);
                જો (r < 0)
                        પાછા r;
        }

        જો (ret_devnr)
                *ret_devnr = devnr;

        પાછા 0;
}

પૂર્ણાંક ptsname_malloc(પૂર્ણાંક fd, અક્ષર **ret) {
        પ્રતિજ્ઞા(fd >= 0);
        પ્રતિજ્ઞા(ret);

        માટે (size_t l = 50;;) {
                _cleanup_free_ અક્ષર *c = ખાલી;

                c = new(અક્ષર, l);
                જો (!c)
                        પાછા -ENOMEM;

                જો (ptsname_r(fd, c, l) >= 0) {
                        *ret = TAKE_PTR(c);
                        પાછા 0;
                }
                જો (errno != ERANGE)
                        પાછા -errno;

                જો (!MUL_ASSIGN_SAFE(&l, 2))
                        પાછા -ENOMEM;
        }
}

પૂર્ણાંક openpt_allocate(પૂર્ણાંક flags, અક્ષર **ret_peer_path) {
        _cleanup_close_ પૂર્ણાંક fd = -EBADF;
        પૂર્ણાંક r;

        fd = posix_openpt(flags|O_NOCTTY|O_CLOEXEC);
        જો (fd < 0)
                પાછા -errno;

        _cleanup_free_ અક્ષર *p = ખાલી;
        જો (ret_peer_path) {
                r = ptsname_malloc(fd, &p);
                જો (r < 0)
                        પાછા r;

                જો (!path_startswith(p, "/dev/pts/"))
                        પાછા -EINVAL;
        }

        જો (unlockpt(fd) < 0)
                પાછા -errno;

        જો (ret_peer_path)
                *ret_peer_path = TAKE_PTR(p);

        પાછા TAKE_FD(fd);
}

સ્થિર પૂર્ણાંક ptsname_namespace(પૂર્ણાંક pty, અક્ષર **ret) {
        પૂર્ણાંક no = -1;

        પ્રતિજ્ઞા(pty >= 0);
        પ્રતિજ્ઞા(ret);

        /* Like ptsname(), but doesn't assume that the path is
         * accessible in the local namespace. */

        જો (ioctl(pty, TIOCGPTN, &no) < 0)
                પાછા -errno;

        જો (no < 0)
                પાછા -EIO;

        જો (asprintf(ret, "/dev/pts/%i", no) < 0)
                પાછા -ENOMEM;

        પાછા 0;
}

પૂર્ણાંક openpt_allocate_in_namespace(
                અચલ PidRef *pidref,
                પૂર્ણાંક flags,
                અક્ષર **ret_peer_path) {

        _cleanup_close_ પૂર્ણાંક pidnsfd = -EBADF, mntnsfd = -EBADF, usernsfd = -EBADF, rootfd = -EBADF, fd = -EBADF;
        _cleanup_close_pair_ પૂર્ણાંક pair[2] = EBADF_PAIR;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(pidref);

        r = pidref_namespace_open(pidref, &pidnsfd, &mntnsfd, /* ret_netns_fd= */ ખાલી, &usernsfd, &rootfd);
        જો (r < 0)
                પાછા log_debug_errno(r, "Failed to open namespaces of PID "PID_FMT": %m", pidref->pid);

        જો (socketpair(AF_UNIX, SOCK_DGRAM|SOCK_CLOEXEC, 0, pair) < 0)
                પાછા -errno;

        r = namespace_fork(
                        "(sd-openptns)",
                        "(sd-openpt)",
                        FORK_RESET_SIGNALS|FORK_DEATHSIG_SIGKILL|FORK_WAIT,
                        pidnsfd,
                        mntnsfd,
                        /* netns_fd= */ -EBADF,
                        usernsfd,
                        rootfd,
                        /* ret= */ ખાલી);
        જો (r < 0)
                પાછા r;
        જો (r == 0) {
                pair[0] = safe_close(pair[0]);

                fd = openpt_allocate(flags, /* ret_peer_path= */ ખાલી);
                જો (fd < 0)
                        _exit(EXIT_FAILURE);

                જો (send_one_fd(pair[1], fd, 0) < 0)
                        _exit(EXIT_FAILURE);

                _exit(EXIT_SUCCESS);
        }

        pair[1] = safe_close(pair[1]);

        fd = receive_one_fd(pair[0], 0);
        જો (fd < 0)
                પાછા fd;

        જો (ret_peer_path) {
                r = ptsname_namespace(fd, ret_peer_path);
                જો (r < 0)
                        પાછા r;
        }

        પાછા TAKE_FD(fd);
}

સ્થિર bool on_dev_null(શૂન્ય) {
        સંરચના stat dst, ost, est;

        જો (cached_on_dev_null >= 0)
                પાછા cached_on_dev_null;

        જો (stat("/dev/null", &dst) < 0 || fstat(STDOUT_FILENO, &ost) < 0 || fstat(STDERR_FILENO, &est) < 0)
                cached_on_dev_null = false;
        નહીંતર
                cached_on_dev_null = stat_inode_same(&dst, &ost) && stat_inode_same(&dst, &est);

        પાછા cached_on_dev_null;
}

bool term_env_valid(અચલ અક્ષર *term) {
        /* Checks if the specified $TERM value is suitable for propagation, i.e. is not empty, not set to
         * "unknown" (as is common in CI), and only contains characters valid in terminal type names.
         * Valid $TERM values are things like "xterm-256color", "linux", "screen.xterm-256color", i.e.
         * alphanumeric characters, hyphens, underscores, dots, and plus signs. */
        પાછા !isempty(term) &&
                !streq(term, "unknown") &&
                in_charset(term, ALPHANUMERICAL "-_+.");
}

bool getenv_terminal_is_dumb(શૂન્ય) {
        અચલ અક્ષર *e;

        e = getenv("TERM");
        જો (!e)
                પાછા true;

        પાછા streq(e, "dumb");
}

bool terminal_is_dumb(શૂન્ય) {
        જો (!on_tty() && !on_dev_null())
                પાછા true;

        પાછા getenv_terminal_is_dumb();
}

bool dev_console_colors_enabled(શૂન્ય) {
        _cleanup_free_ અક્ષર *s = ખાલી;
        ColorMode m;

        /* Returns true if we assume that color is supported on /dev/console.
         *
         * For that we first check if we explicitly got told to use colors or not, by checking $SYSTEMD_COLORS. If that
         * isn't set we check whether PID 1 has $TERM set, and if not, whether TERM is set on the kernel command
         * line. If we find $TERM set we assume color if it's not set to "dumb", similarly to how regular
         * colors_enabled() operates. */

        m = parse_systemd_colors();
        જો (m >= 0)
                પાછા m;

        જો (getenv("NO_COLOR"))
                પાછા false;

        જો (getenv_for_pid(1, "TERM", &s) <= 0)
                (શૂન્ય) proc_cmdline_get_ચાવી("TERM", 0, &s);

        પાછા !streq_ptr(s, "dumb");
}

પૂર્ણાંક vt_restore(પૂર્ણાંક fd) {

        સ્થિર અચલ સંરચના vt_mode mode = {
                .mode = VT_AUTO,
        };

        પૂર્ણાંક r, ret = 0;

        પ્રતિજ્ઞા(fd >= 0);

        જો (!isatty_safe(fd))
                પાછા log_debug_errno(SYNTHETIC_ERRNO(ENOTTY), "Asked to restore the VT for an fd that does not refer to a terminal.");

        જો (ioctl(fd, KDSETMODE, KD_TEXT) < 0)
                RET_GATHER(ret, log_debug_errno(errno, "Failed to set VT to text mode, ignoring: %m"));

        r = vt_reset_keyboard(fd);
        જો (r < 0)
                RET_GATHER(ret, log_debug_errno(r, "Failed to reset keyboard mode, ignoring: %m"));

        જો (ioctl(fd, VT_SETMODE, &mode) < 0)
                RET_GATHER(ret, log_debug_errno(errno, "Failed to set VT_AUTO mode, ignoring: %m"));

        r = fchmod_and_chown(fd, TTY_MODE, 0, GID_INVALID);
        જો (r < 0)
                RET_GATHER(ret, log_debug_errno(r, "Failed to chmod()/chown() VT, ignoring: %m"));

        પાછા ret;
}

પૂર્ણાંક vt_release(પૂર્ણાંક fd, bool restore) {
        પ્રતિજ્ઞા(fd >= 0);

        /* This function releases the VT by acknowledging the VT-switch signal
         * sent by the kernel and optionally reset the VT in text and auto
         * VT-switching modes. */

        જો (!isatty_safe(fd))
                પાછા log_debug_errno(SYNTHETIC_ERRNO(ENOTTY), "Asked to release the VT for an fd that does not refer to a terminal.");

        જો (ioctl(fd, VT_RELDISP, 1) < 0)
                પાછા -errno;

        જો (restore)
                પાછા vt_restore(fd);

        પાછા 0;
}

શૂન્ય get_log_colors(પૂર્ણાંક priority, અચલ અક્ષર **on, અચલ અક્ષર **off, અચલ અક્ષર **highlight) {
        /* Note that this will initialize output variables only when there's something to output.
         * The caller must pre-initialize to "" or NULL as appropriate. */

        જો (priority <= LOG_ERR) {
                જો (on)
                        *on = ansi_highlight_red();
                જો (off)
                        *off = ansi_normal();
                જો (highlight)
                        *highlight = ansi_highlight();

        } નહીંતર જો (priority <= LOG_WARNING) {
                જો (on)
                        *on = ansi_highlight_yellow();
                જો (off)
                        *off = ansi_normal();
                જો (highlight)
                        *highlight = ansi_highlight();

        } નહીંતર જો (priority <= LOG_NOTICE) {
                જો (on)
                        *on = ansi_highlight();
                જો (off)
                        *off = ansi_normal();
                જો (highlight)
                        *highlight = ansi_highlight_red();

        } નહીંતર જો (priority >= LOG_DEBUG) {
                જો (on)
                        *on = ansi_grey();
                જો (off)
                        *off = ansi_normal();
                જો (highlight)
                        *highlight = ansi_highlight_red();
        }
}

પૂર્ણાંક terminal_set_cursor_position(પૂર્ણાંક fd, unsigned row, unsigned column) {
        પ્રતિજ્ઞા(fd >= 0);

        અક્ષર cursor_position[STRLEN("\x1B[" ";" "H") + DECIMAL_STR_MAX(unsigned) * 2 + 1];
        xsprintf(cursor_position, "\x1B[%u;%uH", row, column);

        પાછા loop_write(fd, cursor_position, SIZE_MAX);
}

સ્થિર પૂર્ણાંક terminal_verify_same(પૂર્ણાંક આગત_fd, પૂર્ણાંક output_fd) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(આગત_fd >= 0);
        પ્રતિજ્ઞા(output_fd >= 0);

        /* Validates that the specified fds reference the same TTY */

        જો (આગત_fd != output_fd) {
                સંરચના stat sti;
                જો (fstat(આગત_fd, &sti) < 0)
                        પાછા -errno;

                r = stat_verify_char(&sti); /* TTYs are character devices */
                જો (r < 0)
                        પાછા r;

                સંરચના stat sto;
                જો (fstat(output_fd, &sto) < 0)
                        પાછા -errno;

                r = stat_verify_char(&sto);
                જો (r < 0)
                        પાછા r;

                જો (sti.st_rdev != sto.st_rdev)
                        પાછા -ENOLINK;
        }

        જો (!isatty_safe(આગત_fd)) /* The check above was just for char device, but now let's ensure it's actually a tty */
                પાછા -ENOTTY;

        પાછા 0;
}

પ્રકારવ્યાખ્યા ગણના CursorPositionState {
        CURSOR_TEXT,
        CURSOR_ESCAPE,
        CURSOR_ROW,
        CURSOR_COLUMN,
} CursorPositionState;

પ્રકારવ્યાખ્યા સંરચના CursorPositionContext {
        CursorPositionState state;
        unsigned row, column;
} CursorPositionContext;

સ્થિર પૂર્ણાંક scan_cursor_position_response(
                CursorPositionContext *context,
                અચલ અક્ષર *buf,
                size_t size,
                size_t *ret_processed) {

        પ્રતિજ્ઞા(context);
        પ્રતિજ્ઞા(buf);
        પ્રતિજ્ઞા(ret_processed);

        માટે (size_t i = 0; i < size; i++) {
                અક્ષર c = buf[i];

                પસંદગી (context->state) {

                સ્થિતિ CURSOR_TEXT:
                        context->state = c == '\x1B' ? CURSOR_ESCAPE : CURSOR_TEXT;
                        તોડો;

                સ્થિતિ CURSOR_ESCAPE:
                        context->state = c == '[' ? CURSOR_ROW : CURSOR_TEXT;
                        તોડો;

                સ્થિતિ CURSOR_ROW:
                        જો (c == ';')
                                context->state = context->row > 0 ? CURSOR_COLUMN : CURSOR_TEXT;
                        નહીંતર {
                                પૂર્ણાંક d = undecchar(c);

                                /* We read a decimal character, let's suffix it to the number we so far read,
                                 * but let's do an overflow check first. */
                                જો (d < 0 || context->row > (UINT_MAX-d)/10)
                                        context->state = CURSOR_TEXT;
                                નહીંતર
                                        context->row = context->row * 10 + d;
                        }
                        તોડો;

                સ્થિતિ CURSOR_COLUMN:
                        જો (c == 'R') {
                                જો (context->column > 0) {
                                        *ret_processed = i + 1;
                                        પાછા 1; /* success! */
                                }

                                context->state = CURSOR_TEXT;
                        } નહીંતર {
                                પૂર્ણાંક d = undecchar(c);

                                /* As above, add the decimal character to our column number */
                                જો (d < 0 || context->column > (UINT_MAX-d)/10)
                                        context->state = CURSOR_TEXT;
                                નહીંતર
                                        context->column = context->column * 10 + d;
                        }

                        તોડો;
                }

                /* Reset any positions we might have picked up */
                જો (IN_SET(context->state, CURSOR_TEXT, CURSOR_ESCAPE))
                        context->row = context->column = 0;
        }

        *ret_processed = size;
        પાછા 0; /* all good, but not enough data yet */
}

પૂર્ણાંક terminal_get_cursor_position(
                પૂર્ણાંક આગત_fd,
                પૂર્ણાંક output_fd,
                unsigned *ret_row,
                unsigned *ret_column) {

        _cleanup_close_ પૂર્ણાંક nonblock_આગત_fd = -EBADF;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(આગત_fd >= 0);
        પ્રતિજ્ઞા(output_fd >= 0);

        જો (getenv_terminal_is_dumb())
                પાછા -EOPNOTSUPP;

        r = terminal_verify_same(આગત_fd, output_fd);
        જો (r < 0)
                પાછા log_debug_errno(r, "Called with distinct input/output fds: %m");

        /* Failure to reset the terminal is ignored here and in similar cases below.
         * We already have our result; if cleanup fails it doesn't change the validity of the result. */
        સંરચના termios old_termios = TERMIOS_NULL;
        CLEANUP_TERMIOS_RESET(આગત_fd, old_termios);

        જો (tcgetattr(આગત_fd, &old_termios) < 0)
                પાછા log_debug_errno(errno, "Failed to get terminal settings: %m");

        સંરચના termios new_termios = old_termios;
        termios_disable_echo(&new_termios);

        જો (tcsetattr(આગત_fd, TCSANOW, &new_termios) < 0)
                પાછા log_debug_errno(errno, "Failed to set new terminal settings: %m");

        /* Request cursor position (DSR/CPR) */
        r = loop_write(output_fd, "\x1B[6n", SIZE_MAX);
        જો (r < 0)
                પાછા r;

        /* Open a 2nd input fd, in non-blocking mode, so that we won't ever hang in read() should someone
         * else process the POLLIN. */

        nonblock_આગત_fd = r = fd_reopen(આગત_fd, O_RDONLY|O_CLOEXEC|O_NONBLOCK|O_NOCTTY);
        જો (r < 0)
                પાછા r;

        usec_t end = usec_add(now(CLOCK_MONOTONIC), CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        અક્ષર buf[STRLEN("\x1B[1;1R")]; /* The shortest valid reply possible */
        size_t buf_full = 0;
        CursorPositionContext context = {};

        માટે (bool first = true;; first = false) {
                જો (buf_full == 0) {
                        usec_t n = now(CLOCK_MONOTONIC);
                        જો (n >= end)
                                પાછા -EOPNOTSUPP;

                        r = fd_wait_for_event(nonblock_આગત_fd, POLLIN, usec_sub_unsigned(end, n));
                        જો (r < 0)
                                પાછા r;
                        જો (r == 0)
                                પાછા -EOPNOTSUPP;

                        /* On the first try, read multiple characters, i.e. the shortest valid
                         * reply. Afterwards read byte-wise, since we don't want to read too much, and
                         * unnecessarily drop too many characters from the input queue. */
                        ssize_t l = પઠ(nonblock_આગત_fd, buf, first ? કદ(buf) : 1);
                        જો (l < 0) {
                                જો (errno == EAGAIN)
                                        ચાલુ;

                                પાછા -errno;
                        }

                        પ્રતિજ્ઞા((size_t) l <= કદ(buf));
                        buf_full = l;
                }

                size_t processed;
                r = scan_cursor_position_response(&context, buf, buf_full, &processed);
                જો (r < 0)
                        પાછા r;

                પ્રતિજ્ઞા(processed <= buf_full);
                buf_full -= processed;
                memmove(buf, buf + processed, buf_full);

                જો (r > 0) {
                        /* Superficial validity check */
                        જો (context.row >= 32766 || context.column >= 32766)
                                પાછા -ENODATA;

                        જો (ret_row)
                                *ret_row = context.row;
                        જો (ret_column)
                                *ret_column = context.column;

                        પાછા 0;
                }
        }
}

પૂર્ણાંક terminal_reset_defensive(પૂર્ણાંક fd, TerminalResetFlags flags) {
        પૂર્ણાંક r = 0;

        પ્રતિજ્ઞા(fd >= 0);
        પ્રતિજ્ઞા(!FLAGS_SET(flags, TERMINAL_RESET_AVOID_ANSI_SEQ|TERMINAL_RESET_FORCE_ANSI_SEQ));

        /* Resets the terminal comprehensively, i.e. via both ioctl()s and via ANSI sequences, but do so only
         * if $TERM is unset or set to "dumb" */

        જો (!isatty_safe(fd))
                પાછા -ENOTTY;

        RET_GATHER(r, terminal_reset_ioctl(fd, FLAGS_SET(flags, TERMINAL_RESET_SWITCH_TO_TEXT)));

        જો (!FLAGS_SET(flags, TERMINAL_RESET_AVOID_ANSI_SEQ) &&
            (FLAGS_SET(flags, TERMINAL_RESET_FORCE_ANSI_SEQ) || !getenv_terminal_is_dumb()))
                RET_GATHER(r, terminal_reset_ansi_seq(fd));

        પાછા r;
}

પૂર્ણાંક terminal_reset_defensive_locked(પૂર્ણાંક fd, TerminalResetFlags flags) {
        પ્રતિજ્ઞા(fd >= 0);

        _cleanup_close_ પૂર્ણાંક lock_fd = lock_dev_console();
        જો (lock_fd < 0)
                log_debug_errno(lock_fd, "Failed to acquire lock for /dev/console, ignoring: %m");

        પાછા terminal_reset_defensive(fd, flags);
}

શૂન્ય termios_disable_echo(સંરચના termios *termios) {
        પ્રતિજ્ઞા(termios);

        termios->c_lflag &= ~(ICANON|ECHO);
        termios->c_cc[VMIN] = 1;
        termios->c_cc[VTIME] = 0;
}

સ્થિર bool termios_is_null(અચલ સંરચના termios *t) {
        જો (!t)
                પાછા true;

        પાછા t->c_iflag == UINT_MAX &&
               t->c_oflag == UINT_MAX &&
               t->c_cflag == UINT_MAX &&
               t->c_lflag == UINT_MAX;
}

શૂન્ય termios_reset(અચલ TermiosResetContext *c) {
        પ્રતિજ્ઞા(c);

        PROTECT_ERRNO;

        જો (c->fd && *c->fd >= 0 && !termios_is_null(c->termios))
                (શૂન્ય) tcsetattr(*c->fd, TCSANOW, c->termios);
}

પ્રકારવ્યાખ્યા ગણના BackgroundColorState {
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

પ્રકારવ્યાખ્યા સંરચના BackgroundColorContext {
        BackgroundColorState state;
        uint32_t red, green, blue;
        unsigned red_bits, green_bits, blue_bits;
} BackgroundColorContext;

સ્થિર પૂર્ણાંક scan_background_color_response(
                BackgroundColorContext *context,
                અચલ અક્ષર *buf,
                size_t size,
                size_t *ret_processed) {

        પ્રતિજ્ઞા(context);
        પ્રતિજ્ઞા(buf);
        પ્રતિજ્ઞા(ret_processed);

        માટે (size_t i = 0; i < size; i++) {
                અક્ષર c = buf[i];

                પસંદગી (context->state) {

                સ્થિતિ BACKGROUND_TEXT:
                        context->state = c == '\x1B' ? BACKGROUND_ESCAPE : BACKGROUND_TEXT;
                        તોડો;

                સ્થિતિ BACKGROUND_ESCAPE:
                        context->state = c == ']' ? BACKGROUND_BRACKET : BACKGROUND_TEXT;
                        તોડો;

                સ્થિતિ BACKGROUND_BRACKET:
                        context->state = c == '1' ? BACKGROUND_FIRST_ONE : BACKGROUND_TEXT;
                        તોડો;

                સ્થિતિ BACKGROUND_FIRST_ONE:
                        context->state = c == '1' ? BACKGROUND_SECOND_ONE : BACKGROUND_TEXT;
                        તોડો;

                સ્થિતિ BACKGROUND_SECOND_ONE:
                        context->state = c == ';' ? BACKGROUND_SEMICOLON : BACKGROUND_TEXT;
                        તોડો;

                સ્થિતિ BACKGROUND_SEMICOLON:
                        context->state = c == 'r' ? BACKGROUND_R : BACKGROUND_TEXT;
                        તોડો;

                સ્થિતિ BACKGROUND_R:
                        context->state = c == 'g' ? BACKGROUND_G : BACKGROUND_TEXT;
                        તોડો;

                સ્થિતિ BACKGROUND_G:
                        context->state = c == 'b' ? BACKGROUND_B : BACKGROUND_TEXT;
                        તોડો;

                સ્થિતિ BACKGROUND_B:
                        context->state = c == ':' ? BACKGROUND_RED : BACKGROUND_TEXT;
                        તોડો;

                સ્થિતિ BACKGROUND_RED:
                        જો (c == '/')
                                context->state = context->red_bits > 0 ? BACKGROUND_GREEN : BACKGROUND_TEXT;
                        નહીંતર {
                                પૂર્ણાંક d = unhexchar(c);
                                જો (d < 0 || context->red_bits >= કદ(context->red)*8)
                                        context->state = BACKGROUND_TEXT;
                                નહીંતર {
                                        context->red = (context->red << 4) | d;
                                        context->red_bits += 4;
                                }
                        }
                        તોડો;

                સ્થિતિ BACKGROUND_GREEN:
                        જો (c == '/')
                                context->state = context->green_bits > 0 ? BACKGROUND_BLUE : BACKGROUND_TEXT;
                        નહીંતર {
                                પૂર્ણાંક d = unhexchar(c);
                                જો (d < 0 || context->green_bits >= કદ(context->green)*8)
                                        context->state = BACKGROUND_TEXT;
                                નહીંતર {
                                        context->green = (context->green << 4) | d;
                                        context->green_bits += 4;
                                }
                        }
                        તોડો;

                સ્થિતિ BACKGROUND_BLUE:
                        જો (c == '\x07') {
                                જો (context->blue_bits > 0) {
                                        *ret_processed = i + 1;
                                        પાછા 1; /* success! */
                                }

                                context->state = BACKGROUND_TEXT;
                        } નહીંતર જો (c == '\x1b')
                                context->state = context->blue_bits > 0 ? BACKGROUND_STRING_TERMINATOR : BACKGROUND_TEXT;
                        નહીંતર {
                                પૂર્ણાંક d = unhexchar(c);
                                જો (d < 0 || context->blue_bits >= કદ(context->blue)*8)
                                        context->state = BACKGROUND_TEXT;
                                નહીંતર {
                                        context->blue = (context->blue << 4) | d;
                                        context->blue_bits += 4;
                                }
                        }
                        તોડો;

                સ્થિતિ BACKGROUND_STRING_TERMINATOR:
                        જો (c == '\\') {
                                *ret_processed = i + 1;
                                પાછા 1; /* success! */
                        }

                        context->state = c == ']' ? BACKGROUND_ESCAPE : BACKGROUND_TEXT;
                        તોડો;

                }

                /* Reset any colors we might have picked up */
                જો (IN_SET(context->state, BACKGROUND_TEXT, BACKGROUND_ESCAPE)) {
                        /* reset color */
                        context->red = context->green = context->blue = 0;
                        context->red_bits = context->green_bits = context->blue_bits = 0;
                }
        }

        *ret_processed = size;
        પાછા 0; /* all good, but not enough data yet */
}

પૂર્ણાંક get_default_background_color(દ્વિગુણ *ret_red, દ્વિગુણ *ret_green, દ્વિગુણ *ret_blue) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(ret_red);
        પ્રતિજ્ઞા(ret_green);
        પ્રતિજ્ઞા(ret_blue);

        જો (!colors_enabled())
                પાછા -EOPNOTSUPP;

        r = terminal_verify_same(STDIN_FILENO, STDOUT_FILENO);
        જો (r < 0)
                પાછા r;

        જો (streq_ptr(getenv("TERM"), "linux")) {
                /* Linux console is black */
                *ret_red = *ret_green = *ret_blue = 0.0;
                પાછા 0;
        }

        /* Open a 2nd input fd, in non-blocking mode, so that we won't ever hang in read()
         * should someone else process the POLLIN. Do all subsequent operations on the new fd. */
        _cleanup_close_ પૂર્ણાંક nonblock_આગત_fd = r = fd_reopen(STDIN_FILENO, O_RDONLY|O_CLOEXEC|O_NONBLOCK|O_NOCTTY);
        જો (r < 0)
                પાછા r;

        સંરચના termios old_termios = TERMIOS_NULL;
        CLEANUP_TERMIOS_RESET(nonblock_આગત_fd, old_termios);

        જો (tcgetattr(nonblock_આગત_fd, &old_termios) < 0)
                પાછા -errno;

        સંરચના termios new_termios = old_termios;
        termios_disable_echo(&new_termios);

        જો (tcsetattr(nonblock_આગત_fd, TCSANOW, &new_termios) < 0)
                પાછા -errno;

        r = loop_write(STDOUT_FILENO, ANSI_OSC "11;?" ANSI_ST, SIZE_MAX);
        જો (r < 0)
                પાછા r;

        usec_t end = usec_add(now(CLOCK_MONOTONIC), CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        અક્ષર buf[STRLEN(ANSI_OSC "11;rgb:0/0/0" ANSI_ST)]; /* shortest possible reply */
        size_t buf_full = 0;
        BackgroundColorContext context = {};

        માટે (bool first = true;; first = false) {
                જો (buf_full == 0) {
                        usec_t n = now(CLOCK_MONOTONIC);
                        જો (n >= end)
                                પાછા -EOPNOTSUPP;

                        r = fd_wait_for_event(nonblock_આગત_fd, POLLIN, usec_sub_unsigned(end, n));
                        જો (r < 0)
                                પાછા r;
                        જો (r == 0)
                                પાછા -EOPNOTSUPP;

                        /* On the first try, read multiple characters, i.e. the shortest valid
                         * reply. Afterwards read byte-wise, since we don't want to read too much, and
                         * unnecessarily drop too many characters from the input queue. */
                        ssize_t l = પઠ(nonblock_આગત_fd, buf, first ? કદ(buf) : 1);
                        જો (l < 0) {
                                જો (errno == EAGAIN)
                                        ચાલુ;
                                પાછા -errno;
                        }

                        પ્રતિજ્ઞા((size_t) l <= કદ(buf));
                        buf_full = l;
                }

                size_t processed;
                r = scan_background_color_response(&context, buf, buf_full, &processed);
                જો (r < 0)
                        પાછા r;

                પ્રતિજ્ઞા(processed <= buf_full);
                buf_full -= processed;
                memmove(buf, buf + processed, buf_full);

                જો (r > 0) {
                        પ્રતિજ્ઞા(context.red_bits > 0);
                        *ret_red = (દ્વિગુણ) context.red / ((UINT64_C(1) << context.red_bits) - 1);
                        પ્રતિજ્ઞા(context.green_bits > 0);
                        *ret_green = (દ્વિગુણ) context.green / ((UINT64_C(1) << context.green_bits) - 1);
                        પ્રતિજ્ઞા(context.blue_bits > 0);
                        *ret_blue = (દ્વિગુણ) context.blue / ((UINT64_C(1) << context.blue_bits) - 1);
                        પાછા 0;
                }
        }
}

/* Determine terminal dimensions by means of ANSI sequences: save the cursor via DECSC, position it far
 * to the bottom right (clamped to actual terminal dimensions), read back via DSR where we ended up, and
 * restore cursor via DECRC. Only needs a single DSR round-trip, and always restores the cursor regardless
 * of whether the response is received.
 *
 * Caller must have already opened a non-blocking input fd and configured termios (echo/icanon off). */
સ્થિર પૂર્ણાંક terminal_query_size_by_dsr(
                પૂર્ણાંક nonblock_આગત_fd,
                પૂર્ણાંક output_fd,
                unsigned *ret_rows,
                unsigned *ret_columns) {

        પૂર્ણાંક r;

        પ્રતિજ્ઞા(nonblock_આગત_fd >= 0);
        પ્રતિજ્ઞા(output_fd >= 0);

        /* Use DECSC/DECRC to save/restore cursor instead of querying position via DSR. This way the cursor
         * is always restored — even on timeout — and we only need one DSR response instead of two. */
        r = loop_write_full(output_fd,
                            "\x1B" "7"              /* DECSC: save cursor position */
                            "\x1B[32766;32766H"     /* CUP: position cursor far to the right and to the bottom, staying within 16bit signed range */
                            "\x1B[6n"               /* DSR: request cursor position (CPR) */
                            "\x1B" "8",             /* DECRC: restore cursor position */
                            SIZE_MAX,
                            CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        જો (r < 0)
                પાછા r;

        usec_t end = usec_add(now(CLOCK_MONOTONIC), CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        અક્ષર buf[STRLEN("\x1B[1;1R")]; /* The shortest valid reply possible */
        size_t buf_full = 0;
        CursorPositionContext context = {};

        માટે (bool first = true;; first = false) {
                જો (buf_full == 0) {
                        usec_t n = now(CLOCK_MONOTONIC);
                        જો (n >= end)
                                પાછા -EOPNOTSUPP;

                        r = fd_wait_for_event(nonblock_આગત_fd, POLLIN, usec_sub_unsigned(end, n));
                        જો (r < 0)
                                પાછા r;
                        જો (r == 0)
                                પાછા -EOPNOTSUPP;

                        /* On the first try, read multiple characters, i.e. the shortest valid
                         * reply. Afterwards read byte-wise, since we don't want to read too much, and
                         * unnecessarily drop too many characters from the input queue. */
                        ssize_t l = પઠ(nonblock_આગત_fd, buf, first ? કદ(buf) : 1);
                        જો (l < 0) {
                                જો (errno == EAGAIN)
                                        ચાલુ;

                                પાછા -errno;
                        }

                        પ્રતિજ્ઞા((size_t) l <= કદ(buf));
                        buf_full = l;
                }

                size_t processed;
                r = scan_cursor_position_response(&context, buf, buf_full, &processed);
                જો (r < 0)
                        પાછા r;

                પ્રતિજ્ઞા(processed <= buf_full);
                buf_full -= processed;
                memmove(buf, buf + processed, buf_full);

                જો (r > 0) {
                        /* Superficial validity checks (no particular reason to check for < 4, it's
                         * just a way to look for unreasonably small values) */
                        જો (context.row < 4 || context.column < 4 || context.row >= 32766 || context.column >= 32766)
                                પાછા -ENODATA;

                        જો (ret_rows)
                                *ret_rows = context.row;
                        જો (ret_columns)
                                *ret_columns = context.column;

                        પાછા 0;
                }
        }
}

/* Common setup for ANSI terminal queries: validate the fds, open a non-blocking input fd, and configure
 * termios with echo and canonical mode disabled. Caller must restore termios and close the fd when done. */
સ્થિર પૂર્ણાંક terminal_prepare_query(
                પૂર્ણાંક આગત_fd,
                પૂર્ણાંક output_fd,
                પૂર્ણાંક *ret_nonblock_fd,
                સંરચના termios *ret_saved_termios) {

        પૂર્ણાંક r;

        પ્રતિજ્ઞા(આગત_fd >= 0);
        પ્રતિજ્ઞા(output_fd >= 0);
        પ્રતિજ્ઞા(ret_nonblock_fd);
        પ્રતિજ્ઞા(ret_saved_termios);

        /* Use getenv_terminal_is_dumb() instead of terminal_is_dumb() here since we operate on an
         * explicitly passed fd, not on stdio. terminal_is_dumb() additionally checks on_tty() which
         * tests whether *stderr* is a tty — that's irrelevant when we're querying a directly opened
         * terminal such as /dev/console. */
        જો (getenv_terminal_is_dumb())
                પાછા -EOPNOTSUPP;

        r = terminal_verify_same(આગત_fd, output_fd);
        જો (r < 0)
                પાછા log_debug_errno(r, "Called with distinct input/output fds: %m");

        /* Open a 2nd input fd, in non-blocking mode, so that we won't ever hang in read()
         * should someone else process the POLLIN. Do all subsequent operations on the new fd. */
        _cleanup_close_ પૂર્ણાંક nonblock_આગત_fd = r = fd_reopen(આગત_fd, O_RDONLY|O_CLOEXEC|O_NONBLOCK|O_NOCTTY);
        જો (r < 0)
                પાછા r;

        જો (tcgetattr(nonblock_આગત_fd, ret_saved_termios) < 0)
                પાછા log_debug_errno(errno, "Failed to get terminal settings: %m");

        સંરચના termios new_termios = *ret_saved_termios;
        termios_disable_echo(&new_termios);

        જો (tcsetattr(nonblock_આગત_fd, TCSANOW, &new_termios) < 0)
                પાછા log_debug_errno(errno, "Failed to set new terminal settings: %m");

        *ret_nonblock_fd = TAKE_FD(nonblock_આગત_fd);
        પાછા 0;
}

/*
 * See https://terminalguide.namepad.de/seq/csi_st-18/,
 * https://invisible-island.net/xterm/ctlseqs/ctlseqs.html#h3-Functions-using-CSI-_-ordered-by-the-final-character_s_.
 */
#define CSI18_Q  "\x1B[18t"               /* Report the size of the text area in characters */
#define CSI18_Rp "\x1B[8;"                /* Reply prefix */
#define CSI18_R0 CSI18_Rp "1;1t"          /* Shortest reply */
#define CSI18_R1 CSI18_Rp "32766;32766t"  /* Longest reply */

સ્થિર પૂર્ણાંક scan_text_area_size_response(
                અચલ અક્ષર *buf,
                size_t size,
                unsigned *ret_rows,
                unsigned *ret_columns) {

        પ્રતિજ્ઞા(buf);
        પ્રતિજ્ઞા(ret_rows);
        પ્રતિજ્ઞા(ret_columns);

        /* Check if we have enough space for the shortest possible answer. */
        જો (size < STRLEN(CSI18_R0))
                પાછા -EAGAIN;

        /* Check if the terminating sequence is present */
        જો (buf[size - 1] != 't')
                પાછા -EAGAIN;

        unsigned short rows, columns;
        જો (sscanf(buf, CSI18_Rp "%hu;%hut", &rows, &columns) != 2)
                પાછા -EINVAL;

        *ret_rows = rows;
        *ret_columns = columns;
        પાછા 0;
}

/* Determine terminal dimensions by means of an ANSI CSI 18 sequence.
 *
 * Caller must have already opened a non-blocking input fd and configured termios (echo/icanon off). */
સ્થિર પૂર્ણાંક terminal_query_size_by_csi18(
                પૂર્ણાંક nonblock_આગત_fd,
                પૂર્ણાંક output_fd,
                unsigned *ret_rows,
                unsigned *ret_columns) {

        પૂર્ણાંક r;

        પ્રતિજ્ઞા(nonblock_આગત_fd >= 0);
        પ્રતિજ્ઞા(output_fd >= 0);

        r = loop_write_full(output_fd, CSI18_Q, SIZE_MAX, CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        જો (r < 0)
                પાછા r;

        usec_t end = usec_add(now(CLOCK_MONOTONIC), CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        અક્ષર buf[STRLEN(CSI18_R1)];
        size_t bytes = 0;

        માટે (;;) {
                usec_t n = now(CLOCK_MONOTONIC);
                જો (n >= end)
                        પાછા -EOPNOTSUPP;

                r = fd_wait_for_event(nonblock_આગત_fd, POLLIN, usec_sub_unsigned(end, n));
                જો (r < 0)
                        પાછા r;
                જો (r == 0)
                        પાછા -EOPNOTSUPP;

                /* On the first read, read multiple characters, i.e. the shortest valid reply. Afterwards
                 * read byte by byte, since we don't want to read too much and drop characters from the input
                 * queue. */
                ssize_t l = પઠ(nonblock_આગત_fd, buf + bytes, bytes == 0 ? STRLEN(CSI18_R0) : 1);
                જો (l < 0) {
                        જો (errno == EAGAIN)
                                ચાલુ;
                        પાછા -errno;
                }

                પ્રતિજ્ઞા((size_t) l <= કદ(buf) - bytes);
                bytes += l;

                r = scan_text_area_size_response(buf, bytes, ret_rows, ret_columns);
                જો (r != -EAGAIN)
                        પાછા r;

                જો (bytes == કદ(buf))
                        પાછા -EOPNOTSUPP; /* The response has the right prefix, but we didn't find a valid
                                             * answer with a terminator in the allotted space. Something is
                                             * wrong, possibly some unrelated bytes got injected into the
                                             * answer. */
        }
}

પૂર્ણાંક terminal_get_size(
                પૂર્ણાંક આગત_fd,
                પૂર્ણાંક output_fd,
                unsigned *ret_rows,
                unsigned *ret_columns,
                bool try_dsr,
                bool try_csi18) {

        _cleanup_close_ પૂર્ણાંક nonblock_આગત_fd = -EBADF;
        સંરચના termios old_termios = TERMIOS_NULL;
        CLEANUP_TERMIOS_RESET(nonblock_આગત_fd, old_termios);
        _cleanup_(nonblock_resetp) પૂર્ણાંક nonblock_reset = -EBADF;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(try_dsr || try_csi18);

        r = terminal_prepare_query(આગત_fd, output_fd, &nonblock_આગત_fd, &old_termios);
        જો (r < 0)
                પાછા r;

        /* Put the output fd in non-blocking mode with a write timeout, to avoid blocking indefinitely on
         * write if the terminal is not consuming data (e.g. serial console with flow control). */
        r = fd_nonblock(output_fd, true);
        જો (r < 0)
                પાછા log_debug_errno(r, "Failed to set terminal to non-blocking mode: %m");
        જો (r > 0)
                nonblock_reset = output_fd;

        /* Flush any stale input that might confuse the response parsers. */
        (શૂન્ય) tcflush(nonblock_આગત_fd, TCIFLUSH);

        જો (try_csi18) {
                r = terminal_query_size_by_csi18(nonblock_આગત_fd, output_fd, ret_rows, ret_columns);
                જો (r >= 0)
                        પાછા r;

                /* Query failed. Flush any outstanding input. */
                (શૂન્ય) tcflush(nonblock_આગત_fd, TCIFLUSH);

                જો (!IN_SET(r, -EOPNOTSUPP, -EINVAL))
                        પાછા r;
        }

        જો (try_dsr) {
                r = terminal_query_size_by_dsr(nonblock_આગત_fd, output_fd, ret_rows, ret_columns);
                જો (r >= 0)
                        પાછા r;

                /* Query failed. Flush any outstanding input. */
                (શૂન્ય) tcflush(nonblock_આગત_fd, TCIFLUSH);
        }

        પાછા r;
}

સ્થિર પૂર્ણાંક fd_is_vc(પૂર્ણાંક fd) {
        _cleanup_free_ અક્ષર *name = ખાલી;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(fd >= 0);

        r = getttyname_harder(fd, &name);
        જો (r < 0)
                પાછા r;

        પાછા tty_is_vc_resolve(name);
}

પૂર્ણાંક terminal_fix_size(પૂર્ણાંક આગત_fd, પૂર્ણાંક output_fd) {
        unsigned rows, columns;
        પૂર્ણાંક r;

        /* Don't ask a virtual console at all. There is no intermediary here that could be wrong about the
         * dimensions: the kernel renders the screen itself, so its winsize is authoritative. */
        r = fd_is_vc(આગત_fd);
        જો (r < 0)
                log_debug_errno(r, "Failed to determine whether terminal is a virtual console, assuming it is not: %m");
        નહીંતર જો (r > 0) {
                log_debug("Not querying dimensions of a virtual console, kernel dimensions are authoritative.");
                પાછા 0;
        }

        સંરચના winsize ws = {};
        જો (ioctl(output_fd, TIOCGWINSZ, &ws) < 0)
                પાછા log_debug_errno(errno, "Failed to query terminal dimensions, ignoring: %m");

        r = terminal_get_size(આગત_fd, output_fd, &rows, &columns, /* try_dsr= */ true, /* try_csi18= */ true);
        જો (r < 0)
                પાછા log_debug_errno(r, "Failed to acquire terminal dimensions via ANSI sequences, not adjusting terminal dimensions: %m");

        જો (ws.ws_row == rows && ws.ws_col == columns) {
                log_debug("Terminal dimensions reported via ANSI sequences match currently set terminal dimensions, not changing.");
                પાછા 0;
        }

        ws.ws_col = columns;
        ws.ws_row = rows;

        જો (ioctl(output_fd, TIOCSWINSZ, &ws) < 0)
                પાછા log_debug_errno(errno, "Failed to update terminal dimensions, ignoring: %m");

        log_debug("Fixed terminal dimensions to %ux%u based on ANSI sequence information.", columns, rows);
        પાછા 1;
}

#define MAX_TERMINFO_LENGTH 64
/* python -c 'print("".join(hex(ord(i))[2:] for i in "name").upper())' */
#define DCS_TERMINFO_Q ANSI_DCS "+q" "6E616D65" ANSI_ST
/* The answer is either 0+r… (invalid) or 1+r… (OK). */
#define DCS_TERMINFO_R0 ANSI_DCS "0+r" ANSI_ST
#define DCS_TERMINFO_R1 ANSI_DCS "1+r" "6E616D65" "=" /* This is followed by Pt ST. */
assert_cc(STRLEN(DCS_TERMINFO_R0) <= STRLEN(DCS_TERMINFO_R1 ANSI_ST));

સ્થિર પૂર્ણાંક scan_terminfo_response(
                અચલ અક્ષર *buf,
                size_t size,
                અક્ષર **ret_name) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(buf);
        પ્રતિજ્ઞા(ret_name);

        /* Check if we have enough space for the shortest possible answer. */
        જો (size < STRLEN(DCS_TERMINFO_R0))
                પાછા -EAGAIN;

        /* Check if the terminating sequence is present */
        જો (memcmp(buf + size - STRLEN(ANSI_ST), ANSI_ST, STRLEN(ANSI_ST)) != 0)
                પાછા -EAGAIN;

        જો (size <= STRLEN(DCS_TERMINFO_R1 ANSI_ST))
                પાછા -EINVAL;  /* The answer is invalid or empty */

        જો (memcmp(buf, DCS_TERMINFO_R1, STRLEN(DCS_TERMINFO_R1)) != 0)
                પાછા -EINVAL;  /* The answer is not valid */

        _cleanup_free_ શૂન્ય *dec = ખાલી;
        size_t dec_size;
        r = unhexmem_full(buf + STRLEN(DCS_TERMINFO_R1), size - STRLEN(DCS_TERMINFO_R1 ANSI_ST),
                          /* secure= */ false,
                          &dec, &dec_size);
        જો (r < 0)
                પાછા r;

        પ્રતિજ્ઞા(((અચલ અક્ષર *) dec)[dec_size] == '\0'); /* unhexmem appends NUL for our convenience */
        જો (memchr(dec, '\0', dec_size) || string_has_cc(dec, ખાલી) || !filename_is_valid(dec))
                પાછા -EUCLEAN;

        *ret_name = TAKE_PTR(dec);
        પાછા 0;
}

પૂર્ણાંક terminal_get_terminfo_by_dcs(પૂર્ણાંક fd, અક્ષર **ret_name) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(fd >= 0);
        પ્રતિજ્ઞા(ret_name);

        /* Note: fd must be in non-blocking read-write mode! */

        સંરચના termios old_termios = TERMIOS_NULL;
        CLEANUP_TERMIOS_RESET(fd, old_termios);

        જો (tcgetattr(fd, &old_termios) < 0)
                પાછા -errno;

        સંરચના termios new_termios = old_termios;
        termios_disable_echo(&new_termios);

        જો (tcsetattr(fd, TCSANOW, &new_termios) < 0)
                પાછા -errno;

        r = loop_write(fd, DCS_TERMINFO_Q, SIZE_MAX);
        જો (r < 0)
                પાછા r;

        usec_t end = usec_add(now(CLOCK_MONOTONIC), CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        અક્ષર buf[STRLEN(DCS_TERMINFO_R1) + MAX_TERMINFO_LENGTH + STRLEN(ANSI_ST)];
        size_t bytes = 0;

        માટે (;;) {
                usec_t n = now(CLOCK_MONOTONIC);
                જો (n >= end)
                        પાછા -EOPNOTSUPP;

                r = fd_wait_for_event(fd, POLLIN, usec_sub_unsigned(end, n));
                જો (r < 0)
                        પાછા r;
                જો (r == 0)
                        પાછા -EOPNOTSUPP;

                /* On the first read, read multiple characters, i.e. the shortest valid reply. Afterwards
                 * read byte by byte, since we don't want to read too much and drop characters from the input
                 * queue. */
                ssize_t l = પઠ(fd, buf + bytes, bytes == 0 ? STRLEN(DCS_TERMINFO_R0) : 1);
                જો (l < 0) {
                        જો (errno == EAGAIN)
                                ચાલુ;
                        પાછા -errno;
                }

                પ્રતિજ્ઞા((size_t) l <= કદ(buf) - bytes);
                bytes += l;

                r = scan_terminfo_response(buf, bytes, ret_name);
                જો (r != -EAGAIN)
                        પાછા r;

                જો (bytes == કદ(buf))
                        પાછા -EOPNOTSUPP; /* The response has the right prefix, but we didn't find a valid
                                             * answer with a terminator in the allotted space. Something is
                                             * wrong, possibly some unrelated bytes got injected into the
                                             * answer. */
        }
}

પૂર્ણાંક have_terminfo_file(અચલ અક્ષર *name) {
        /* This is a heuristic check if we have the file, using the directory layout used on
         * current Linux systems. Checks for other layouts can be added later if appropriate. */
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(filename_is_valid(name));

        _cleanup_free_ અક્ષર *p = path_join("/usr/share/terminfo", CHAR_TO_STR(name[0]), name);
        જો (!p)
                પાછા log_oom_debug();

        r = RET_NERRNO(access(p, F_OK));
        જો (r == -ENOENT)
                પાછા false;
        જો (r < 0)
                પાછા r;
        પાછા true;
}

પૂર્ણાંક query_term_for_tty(અચલ અક્ષર *tty, અક્ષર **ret_term) {
        _cleanup_free_ અક્ષર *dcs_term = ખાલી;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(tty);
        પ્રતિજ્ઞા(ret_term);

        જો (tty_is_vc_resolve(tty))
                પાછા strdup_to(ret_term, "linux");

        /* Try to query the terminal implementation that we're on. This will not work in all
         * cases, which is fine, since this is intended to be used as a fallback. */

        _cleanup_close_ પૂર્ણાંક tty_fd = open_terminal(tty, O_RDWR|O_NOCTTY|O_CLOEXEC|O_NONBLOCK);
        જો (tty_fd < 0)
                પાછા log_debug_errno(tty_fd, "Failed to open %s to query terminfo: %m", tty);

        r = terminal_get_terminfo_by_dcs(tty_fd, &dcs_term);
        જો (r < 0)
                પાછા log_debug_errno(r, "Failed to query %s for terminfo: %m", tty);

        r = have_terminfo_file(dcs_term);
        જો (r < 0)
                પાછા log_debug_errno(r, "Failed to look for terminfo %s: %m", dcs_term);
        જો (r == 0)
                પાછા log_info_errno(SYNTHETIC_ERRNO(ENODATA),
                                      "Terminfo %s not found for %s.", dcs_term, tty);

        *ret_term = TAKE_PTR(dcs_term);
        પાછા 0;
}

પૂર્ણાંક terminal_is_pty_fd(પૂર્ણાંક fd) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(fd >= 0);

        /* Returns true if we are looking at a pty, i.e. if it's backed by the /dev/pts/ file system */

        જો (!isatty_safe(fd))
                પાછા false;

        r = is_fs_type_at(fd, ખાલી, DEVPTS_SUPER_MAGIC);
        જો (r != 0)
                પાછા r;

        /* The ptmx device is weird, it exists twice, once inside and once outside devpts. To detect the
         * latter case, let's fire off an ioctl() that only works on ptmx devices. */

        પૂર્ણાંક v;
        જો (ioctl(fd, TIOCGPKT, &v) < 0) {
                જો (ERRNO_IS_NOT_SUPPORTED(errno))
                        પાછા false;

                પાછા -errno;
        }

        પાછા true;
}

પૂર્ણાંક pty_open_peer(પૂર્ણાંક fd, પૂર્ણાંક mode) {
        પ્રતિજ્ઞા(fd >= 0);

        /* Opens the peer PTY using the new race-free TIOCGPTPEER ioctl() (kernel 4.13).
         *
         * This is safe to be called on TTYs from other namespaces. */

        પ્રતિજ્ઞા((mode & (O_CREAT|O_PATH|O_DIRECTORY|O_TMPFILE)) == 0);

        /* This replicates the EIO retry logic of open_terminal() in a modified way. */
        માટે (unsigned c = 0;; c++) {
                પૂર્ણાંક peer_fd = ioctl(fd, TIOCGPTPEER, mode);
                જો (peer_fd >= 0)
                        પાછા peer_fd;

                જો (errno != EIO)
                        પાછા -errno;

                /* Max 1s in total */
                જો (c >= 20)
                        પાછા -EIO;

                (શૂન્ય) usleep_safe(50 * USEC_PER_MSEC);
        }
}
