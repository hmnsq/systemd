/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব <fcntl.h>
#অন্তর্ভাব <linux/kd.h>
#অন্তর্ভাব <linux/magic.h>
#অন্তর্ভাব <linux/tiocl.h>
#অন্তর্ভাব <linux/vt.h>
#অন্তর্ভাব <poll.h>
#অন্তর্ভাব <signal.h>
#অন্তর্ভাব <stdlib.h>
#অন্তর্ভাব <sys/inotify.h>
#অন্তর্ভাব <sys/ioctl.h>
#অন্তর্ভাব <sys/sysmacros.h>
#অন্তর্ভাব <termios.h>
#অন্তর্ভাব <time.h>
#অন্তর্ভাব <unistd.h>

#অন্তর্ভাব "alloc-util.h"
#অন্তর্ভাব "ansi-color.h"
#অন্তর্ভাব "chase.h"
#অন্তর্ভাব "devnum-util.h"
#অন্তর্ভাব "errno-util.h"
#অন্তর্ভাব "extract-word.h"
#অন্তর্ভাব "fd-util.h"
#অন্তর্ভাব "fileio.h"
#অন্তর্ভাব "fs-util.h"
#অন্তর্ভাব "hexdecoct.h"
#অন্তর্ভাব "inotify-util.h"
#অন্তর্ভাব "io-util.h"
#অন্তর্ভাব "log.h"
#অন্তর্ভাব "namespace-util.h"
#অন্তর্ভাব "parse-util.h"
#অন্তর্ভাব "path-util.h"
#অন্তর্ভাব "pidref.h"
#অন্তর্ভাব "proc-cmdline.h"
#অন্তর্ভাব "process-util.h"
#অন্তর্ভাব "signal-util.h"
#অন্তর্ভাব "socket-util.h"
#অন্তর্ভাব "stat-util.h"
#অন্তর্ভাব "stdio-util.h"
#অন্তর্ভাব "string-util.h"
#অন্তর্ভাব "strv.h"
#অন্তর্ভাব "terminal-util.h"
#অন্তর্ভাব "time-util.h"
#অন্তর্ভাব "utf8.h"

/* How much to wait when reading/writing ANSI sequences from/to the console */
#define CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC (333 * USEC_PER_MSEC)

স্থির চঞ্চল unsigned cached_columns = 0;
স্থির চঞ্চল unsigned cached_সারিগুলি = 0;

স্থির চঞ্চল পূর্ণাঙ্ক cached_on_tty = -1;
স্থির চঞ্চল পূর্ণাঙ্ক cached_on_dev_null = -1;

bool isatty_safe(পূর্ণাঙ্ক fd) {
        প্রতিজ্ঞা(fd >= 0);

        যদি (isatty(fd))
                প্রত্যাগম true;

        /* Linux/glibc returns EIO for hung up TTY on isatty(). Which is wrong, the thing doesn't stop being
         * a TTY after all, just because it is temporarily hung up. Let's work around this here, until this
         * is fixed in glibc. See: https://sourceware.org/bugzilla/show_bug.cgi?id=32103 */
        যদি (errno == EIO)
                প্রত্যাগম true;

        /* Be resilient if we're working on stdio, since they're set up by parent process. */
        প্রতিজ্ঞা(errno != EBADF || IN_SET(fd, STDIN_FILENO, STDOUT_FILENO, STDERR_FILENO));

        প্রত্যাগম false;
}

পূর্ণাঙ্ক chvt(পূর্ণাঙ্ক vt) {
        _cleanup_close_ পূর্ণাঙ্ক fd = -EBADF;

        /* Switch to the specified vt number. If the VT is specified <= 0 switch to the VT the kernel log messages go,
         * if that's configured. */

        fd = open_terminal("/dev/tty0", O_RDWR|O_NOCTTY|O_CLOEXEC|O_NONBLOCK);
        যদি (fd < 0)
                প্রত্যাগম fd;

        যদি (vt <= 0) {
                পূর্ণাঙ্ক tiocl[2] = {
                        TIOCL_GETKMSGREDIRECT,
                        0
                };

                যদি (ioctl(fd, TIOCLINUX, tiocl) < 0)
                        প্রত্যাগম -errno;

                vt = tiocl[0] <= 0 ? 1 : tiocl[0];
        }

        প্রত্যাগম RET_NERRNO(ioctl(fd, VT_ACTIVATE, vt));
}

পূর্ণাঙ্ক read_one_char(FILE *f, বর্ণ *ret, usec_t t, bool echo, bool *need_nl) {
        _cleanup_free_ বর্ণ *সারি = রিক্ত;
        সংরচনা termios old_termios;
        পূর্ণাঙ্ক r, fd;

        প্রতিজ্ঞা(ret);

        যদি (!f)
                f = stdin;

        /* If this is a terminal, then switch canonical mode off, so that we can read a single
         * character. (Note that fmemopen() streams do not have an fd associated with them, let's handle that
         * nicely.) If 'echo' is false we'll also disable ECHO mode so that the pressed key is not made
         * visible to the user. */
        fd = fileno(f);
        যদি (fd >= 0 && tcgetattr(fd, &old_termios) >= 0) {
                সংরচনা termios new_termios = old_termios;

                new_termios.c_lflag &= ~(ICANON|(echo ? 0 : ECHO));
                new_termios.c_cc[VMIN] = 1;
                new_termios.c_cc[VTIME] = 0;

                যদি (tcsetattr(fd, TCSANOW, &new_termios) >= 0) {
                        বর্ণ c;

                        যদি (t != USEC_INFINITY) {
                                যদি (fd_wait_for_event(fd, POLLIN, t) <= 0) {
                                        (শূন্য) tcsetattr(fd, TCSANOW, &old_termios);
                                        প্রত্যাগম -ETIMEDOUT;
                                }
                        }

                        r = safe_fgetc(f, &c);
                        (শূন্য) tcsetattr(fd, TCSANOW, &old_termios);
                        যদি (r < 0)
                                প্রত্যাগম r;
                        যদি (r == 0)
                                প্রত্যাগম -EIO;

                        যদি (need_nl)
                                *need_nl = c != '\n';

                        *ret = c;
                        প্রত্যাগম 0;
                }
        }

        যদি (t != USEC_INFINITY && fd >= 0) {
                /* Let's wait the specified amount of time for input. When we have no fd we skip this, under
                 * the assumption that this is an fmemopen() stream or so where waiting doesn't make sense
                 * anyway, as the data is either already in the stream or cannot possible be placed there
                 * while we access the stream */

                যদি (fd_wait_for_event(fd, POLLIN, t) <= 0)
                        প্রত্যাগম -ETIMEDOUT;
        }

        /* If this is not a terminal, then read a full line instead */

        r = read_সারি(f, 16, &সারি); /* longer than necessary, to eat up UTF-8 chars/vt100 key sequences */
        যদি (r < 0)
                প্রত্যাগম r;
        যদি (r == 0)
                প্রত্যাগম -EIO;

        যদি (strlen(সারি) != 1)
                প্রত্যাগম -EBADMSG;

        যদি (need_nl)
                *need_nl = false;

        *ret = সারি[0];
        প্রত্যাগম 0;
}

#define DEFAULT_ASK_REFRESH_USEC (2*USEC_PER_SEC)

পূর্ণাঙ্ক ask_char(বর্ণ *ret, ধ্রুব বর্ণ *replies, ধ্রুব বর্ণ *fmt, ...) {
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(ret);
        প্রতিজ্ঞা(replies);
        প্রতিজ্ঞা(fmt);

        প্রতি (;;) {
                va_list ap;
                বর্ণ c;
                bool need_nl = true;

                fputs(ansi_highlight(), stdout);

                putchar('\r');

                va_start(ap, fmt);
                vprintf(fmt, ap);
                va_end(ap);

                fputs(ansi_normal(), stdout);

                fflush(stdout);

                r = read_one_char(stdin, &c, DEFAULT_ASK_REFRESH_USEC, /* echo= */ true, &need_nl);
                যদি (r < 0) {

                        যদি (r == -ETIMEDOUT)
                                অনুবর্ত;

                        যদি (r == -EBADMSG) {
                                puts("Bad input, please try again.");
                                অনুবর্ত;
                        }

                        putchar('\n');
                        প্রত্যাগম r;
                }

                যদি (need_nl)
                        putchar('\n');

                যদি (strchr(replies, c)) {
                        *ret = c;
                        প্রত্যাগম 0;
                }

                puts("Read unexpected character, please try again.");
        }
}

প্রকারনির্দেশ গণনা CompletionResult{
        COMPLETION_ALREADY,       /* the input string is already complete */
        COMPLETION_FULL,          /* completed the input string to be complete now */
        COMPLETION_PARTIAL,       /* completed the input string so that is still incomplete */
        COMPLETION_NONE,          /* found no matching completion */
        _COMPLETION_RESULT_MAX,
        _COMPLETION_RESULT_INVALID = -EINVAL,
        _COMPLETION_RESULT_ERRNO_MAX = -ERRNO_MAX,
} CompletionResult;

স্থির CompletionResult pick_completion(ধ্রুব বর্ণ *string, বর্ণ *ধ্রুব*completions, বর্ণ **ret) {
        _cleanup_free_ বর্ণ *found = রিক্ত;
        bool partial = false;

        প্রতিজ্ঞা(ret);

        string = strempty(string);

        STRV_FOREACH(c, completions) {

                /* Ignore entries that are not actually completions */
                যদি (!startswith(*c, string))
                        অনুবর্ত;

                /* Store first completion that matches */
                যদি (!found) {
                        found = strdup(*c);
                        যদি (!found)
                                প্রত্যাগম -ENOMEM;

                        অনুবর্ত;
                }

                /* If there's another completion that works truncate the one we already found by common
                 * prefix */
                size_t n = str_common_prefix(found, *c);
                যদি (n == SIZE_MAX)
                        অনুবর্ত;

                found[n] = 0;
                partial = true;
        }

        *ret = TAKE_PTR(found);

        যদি (!*ret)
                প্রত্যাগম COMPLETION_NONE;
        যদি (partial)
                প্রত্যাগম COMPLETION_PARTIAL;

        প্রত্যাগম streq(string, *ret) ? COMPLETION_ALREADY : COMPLETION_FULL;
}

স্থির শূন্য clear_by_backspace(size_t n) {
        /* Erase the specified number of character cells backwards on the terminal */
        প্রতি (size_t i = 0; i < n; i++)
                fputs("\b \b", stdout);
}

পূর্ণাঙ্ক ask_string_full(
                বর্ণ **ret,
                ধ্রুব বর্ণ *prefill,
                GetCompletionsCallback get_completions,
                শূন্য *userdata,
                ধ্রুব বর্ণ *text, ...) {

        va_list ap;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(ret);
        প্রতিজ্ঞা(text);

        _cleanup_free_ বর্ণ *string = রিক্ত;
        size_t n = 0;

        যদি (prefill) {
                /* Prefill query with explicit data if specified */

                string = strdup(prefill);
                যদি (!string)
                        প্রত্যাগম -ENOMEM;

                n = strlen(string);

        } অন্যথা যদি (get_completions) {
                /* Otherwise, figure out what string to preselect the query with */
                _cleanup_strv_free_ বর্ণ **completions = রিক্ত;
                r = get_completions("", GET_COMPLETIONS_PRESELECT, &completions, userdata);
                যদি (r < 0)
                        প্রত্যাগম r;

                CompletionResult cr = pick_completion(string, completions, &string);
                যদি (cr < 0)
                        প্রত্যাগম cr;

                n = strlen_ptr(string);
        }

        /* Output the prompt */
        fputs(ansi_highlight(), stdout);
        va_start(ap, text);
        vprintf(text, ap);
        va_end(ap);
        fputs(ansi_normal(), stdout);
        যদি (string)
                fputs(string, stdout);
        fflush(stdout);

        /* Do interactive logic only if stdin + stdout are connected to the same place. And yes, we could use
         * STDIN_FILENO and STDOUT_FILENO here, but let's be overly correct for once, after all libc allows
         * swapping out stdin/stdout. */
        পূর্ণাঙ্ক fd_প্রবেশ = fileno(stdin);
        পূর্ণাঙ্ক fd_output = fileno(stdout);
        সংরচনা termios old_termios = TERMIOS_NULL;
        CLEANUP_TERMIOS_RESET(fd_প্রবেশ, old_termios);

        যদি (fd_প্রবেশ < 0 || fd_output < 0 || same_fd(fd_প্রবেশ, fd_output) <= 0)
                গচ্ছ fallback;

        /* Try to disable echo, which also tells us if this even is a terminal */
        যদি (tcgetattr(fd_প্রবেশ, &old_termios) < 0) {
                old_termios = TERMIOS_NULL;
                গচ্ছ fallback;
        }

        সংরচনা termios new_termios = old_termios;
        termios_disable_echo(&new_termios);
        যদি (tcsetattr(fd_প্রবেশ, TCSANOW, &new_termios) < 0)
                প্রত্যাগম -errno;

        প্রতি (;;) {
                পূর্ণাঙ্ক c = fgetc(stdin);

                /* On EOF or NUL, end the request, don't output anything anymore */
                যদি (IN_SET(c, EOF, 0))
                        ভঙ্গ;

                /* On Return also end the request, but make this visible */
                যদি (IN_SET(c, '\n', '\r')) {
                        fputc('\n', stdout);
                        ভঙ্গ;
                }

                যদি (c == '\t') {
                        /* Tab */

                        _cleanup_strv_free_ বর্ণ **completions = রিক্ত;
                        যদি (get_completions) {
                                r = get_completions(string, /* flags= */ 0, &completions, userdata);
                                যদি (r < 0)
                                        প্রত্যাগম r;
                        }

                        _cleanup_free_ বর্ণ *new_string = রিক্ত;
                        CompletionResult cr = pick_completion(string, completions, &new_string);
                        যদি (cr < 0)
                                প্রত্যাগম cr;
                        যদি (IN_SET(cr, COMPLETION_PARTIAL, COMPLETION_FULL)) {
                                /* Output the new suffix we learned */
                                fputs(ASSERT_PTR(startswith(new_string, strempty(string))), stdout);

                                /* And update the whole string */
                                free_and_replace(string, new_string);
                                n = strlen(string);
                        }
                        যদি (cr == COMPLETION_NONE)
                                fputc('\a', stdout); /* BEL */

                        যদি (IN_SET(cr, COMPLETION_PARTIAL, COMPLETION_ALREADY)) {
                                /* If this worked only partially, or if the user hit TAB even though we were
                                 * complete already, then show the remaining options (in the latter case just
                                 * the one). */
                                fputc('\n', stdout);

                                _cleanup_strv_free_ বর্ণ **filtered = strv_filter_prefix(completions, string);
                                যদি (!filtered)
                                        প্রত্যাগম -ENOMEM;

                                r = show_menu(filtered,
                                              /* n_columns= */ SIZE_MAX,
                                              /* column_width= */ SIZE_MAX,
                                              /* ellipsize_percentage= */ 0,
                                              /* grey_prefix= */ string,
                                              /* with_numbers= */ false);
                                যদি (r < 0)
                                        প্রত্যাগম r;

                                /* Show the prompt again */
                                fputs(ansi_highlight(), stdout);
                                va_start(ap, text);
                                vprintf(text, ap);
                                va_end(ap);
                                fputs(ansi_normal(), stdout);
                                fputs(string, stdout);
                        }

                } অন্যথা যদি (IN_SET(c, '\b', 127)) {
                        /* Backspace */

                        যদি (n == 0)
                                fputc('\a', stdout); /* BEL */
                        অন্যথা {
                                size_t m = utf8_last_length(string, n);

                                বর্ণ *e = string + n - m;
                                clear_by_backspace(utf8_console_width(e));

                                *e = 0;
                                n -= m;
                        }

                } অন্যথা যদি (c == 21) {
                        /* Ctrl-u → erase all input */

                        clear_by_backspace(utf8_console_width(string));
                        যদি (string)
                                string[n = 0] = 0;
                        অন্যথা
                                প্রতিজ্ঞা(n == 0);

                } অন্যথা যদি (c == 4) {
                        /* Ctrl-d → cancel this field input */

                        প্রত্যাগম -ECANCELED;

                } অন্যথা যদি (char_is_cc(c) || n >= LINE_MAX)
                        /* refuse control characters and too long strings */
                        fputc('\a', stdout); /* BEL */
                অন্যথা {
                        /* Regular char */

                        যদি (!GREEDY_REALLOC(string, n+2))
                                প্রত্যাগম -ENOMEM;

                        string[n++] = (বর্ণ) c;
                        string[n] = 0;

                        fputc(c, stdout);
                }

                fflush(stdout);
        }

        যদি (!string) {
                string = strdup("");
                যদি (!string)
                        প্রত্যাগম -ENOMEM;
        }

        *ret = TAKE_PTR(string);
        প্রত্যাগম 0;

fallback:
        /* A simple fallback without TTY magic */
        string = mfree(string);
        r = read_সারি(stdin, LONG_সারি_MAX, &string);
        যদি (r < 0)
                প্রত্যাগম r;
        যদি (r == 0)
                প্রত্যাগম -EIO;

        *ret = TAKE_PTR(string);
        প্রত্যাগম 0;
}

bool any_চাবি_to_proceed(শূন্য) {

        /* Insert a new line here as well as to when the user inputs, as this is also used during the boot up
         * sequence when status messages may be interleaved with the current program output. This ensures
         * that the status messages aren't appended on the same line as this message. */

        fputc('\n', stdout);
        fputs(ansi_highlight_magenta(), stdout);
        fputs("-- Press any key to proceed --", stdout);
        fputs(ansi_normal(), stdout);
        fputc('\n', stdout);
        fflush(stdout);

        বর্ণ চাবি = 0;
        (শূন্য) read_one_char(stdin, &চাবি, USEC_INFINITY, /* echo= */ false, /* need_nl= */ রিক্ত);

        fputc('\n', stdout);
        fflush(stdout);

        প্রত্যাগম চাবি != 'q';
}

স্থির size_t widest_list_element(বর্ণ *ধ্রুব*l) {
        size_t w = 0;

        /* Returns the largest console width of all elements in 'l' */

        STRV_FOREACH(i, l)
                w = MAX(w, utf8_console_width(*i));

        প্রত্যাগম w;
}

পূর্ণাঙ্ক show_menu(বর্ণ **x,
              size_t n_columns,
              size_t column_width,
              unsigned ellipsize_percentage,
              ধ্রুব বর্ণ *grey_prefix,
              bool with_numbers) {

        প্রতিজ্ঞা(n_columns > 0);
        প্রতিজ্ঞা(column_width > 0);

        যদি (n_columns == SIZE_MAX)
                n_columns = 3;

        যদি (column_width == SIZE_MAX) {
                size_t widest = widest_list_element(x);

                /* If not specified, derive column width from screen width */
                size_t column_max = (columns()-1) / n_columns;

                /* Subtract room for numbers */
                যদি (with_numbers)
                        column_max = LESS_BY(column_max, 6U);

                /* If columns would get too tight let's make this a linear list instead. */
                যদি (column_max < 10 && widest > 10) {
                        n_columns = 1;
                        column_max = columns()-1;

                        যদি (with_numbers)
                                column_max = LESS_BY(column_max, 6U);
                }

                /* Never make this narrower than 10 characters */
                column_max = MAX(column_max, 10U);
                column_width = CLAMP(widest+1, 10U, column_max);
        }

        size_t n = strv_length(x);
        size_t per_column = DIV_ROUND_UP(n, n_columns);

        size_t break_সারিগুলি = সারিগুলি();
        যদি (break_সারিগুলি > 2)
                break_সারিগুলি--;

        /* The first page gets two extra lines, since we want to show a title */
        size_t break_modulo = break_সারিগুলি;
        যদি (break_modulo > 3)
                break_modulo -= 3;

        প্রতি (size_t i = 0; i < per_column; i++) {

                প্রতি (size_t j = 0; j < n_columns; j++) {
                        _cleanup_free_ বর্ণ *e = রিক্ত;

                        size_t p = j * per_column + i;
                        যদি (p >= n)
                                ভঙ্গ;

                        e = ellipsize(x[p], column_width, ellipsize_percentage);
                        যদি (!e)
                                প্রত্যাগম -ENOMEM;

                        যদি (with_numbers)
                                printf("%s%4zu)%s ",
                                       ansi_grey(),
                                       p + 1,
                                       ansi_normal());

                        যদি (grey_prefix && startswith(e, grey_prefix)) {
                                size_t k = MIN(strlen(grey_prefix), column_width);
                                printf("%s%.*s%s",
                                       ansi_grey(),
                                       (পূর্ণাঙ্ক) k, e,
                                       ansi_normal());
                                printf("%-*s",
                                       (পূর্ণাঙ্ক) (column_width - k), e+k);
                        } অন্যথা
                                printf("%-*s", (পূর্ণাঙ্ক) column_width, e);
                }

                putchar('\n');

                /* on the first screen we reserve 2 extra lines for the title */
                যদি (i % break_সারিগুলি == break_modulo)
                        যদি (!any_চাবি_to_proceed())
                                প্রত্যাগম 0;
        }

        প্রত্যাগম 0;
}

পূর্ণাঙ্ক open_terminal(ধ্রুব বর্ণ *name, পূর্ণাঙ্ক mode) {
        _cleanup_close_ পূর্ণাঙ্ক fd = -EBADF;

        /*
         * If a TTY is in the process of being closed opening it might cause EIO. This is horribly awful, but
         * unlikely to be changed in the kernel. Hence we work around this problem by retrying a couple of
         * times.
         *
         * https://bugs.launchpad.net/ubuntu/+source/linux/+bug/554172/comments/245
         */

        প্রতিজ্ঞা((mode & (O_CREAT|O_PATH|O_DIRECTORY|O_TMPFILE)) == 0);

        প্রতি (unsigned c = 0;; c++) {
                fd = open(name, mode, 0);
                যদি (fd >= 0)
                        ভঙ্গ;

                যদি (errno != EIO)
                        প্রত্যাগম -errno;

                /* Max 1s in total */
                যদি (c >= 20)
                        প্রত্যাগম -EIO;

                (শূন্য) usleep_safe(50 * USEC_PER_MSEC);
        }

        যদি (!isatty_safe(fd))
                প্রত্যাগম -ENOTTY;

        প্রত্যাগম TAKE_FD(fd);
}

পূর্ণাঙ্ক acquire_terminal(
                ধ্রুব বর্ণ *name,
                AcquireTerminalFlags flags,
                usec_t timeout) {

        _cleanup_close_ পূর্ণাঙ্ক notify = -EBADF, fd = -EBADF;
        usec_t ts = USEC_INFINITY;
        পূর্ণাঙ্ক r, wd = -1;

        প্রতিজ্ঞা(name);

        AcquireTerminalFlags mode = flags & _ACQUIRE_TERMINAL_MODE_MASK;
        প্রতিজ্ঞা(IN_SET(mode, ACQUIRE_TERMINAL_TRY, ACQUIRE_TERMINAL_FORCE, ACQUIRE_TERMINAL_WAIT));
        প্রতিজ্ঞা(mode == ACQUIRE_TERMINAL_WAIT || !FLAGS_SET(flags, ACQUIRE_TERMINAL_WATCH_SIGTERM));

        /* We use inotify to be notified when the tty is closed. We create the watch before checking if we can actually
         * acquire it, so that we don't lose any event.
         *
         * Note: strictly speaking this actually watches for the device being closed, it does *not* really watch
         * whether a tty loses its controlling process. However, unless some rogue process uses TIOCNOTTY on /dev/tty
         * *after* closing its tty otherwise this will not become a problem. As long as the administrator makes sure to
         * not configure any service on the same tty as an untrusted user this should not be a problem. (Which they
         * probably should not do anyway.) */

        যদি (mode == ACQUIRE_TERMINAL_WAIT) {
                notify = inotify_init1(IN_CLOEXEC | IN_NONBLOCK);
                যদি (notify < 0)
                        প্রত্যাগম -errno;

                wd = inotify_add_watch(notify, name, IN_CLOSE);
                যদি (wd < 0)
                        প্রত্যাগম -errno;

                যদি (timeout != USEC_INFINITY)
                        ts = now(CLOCK_MONOTONIC);
        }

        /* If we are called with ACQUIRE_TERMINAL_WATCH_SIGTERM we'll unblock SIGTERM during ppoll() temporarily */
        sigset_t poll_ss;
        assert_se(sigprocmask(SIG_SETMASK, /* newset= */ রিক্ত, &poll_ss) >= 0);
        যদি (flags & ACQUIRE_TERMINAL_WATCH_SIGTERM) {
                assert_se(sigismember(&poll_ss, SIGTERM) > 0);
                assert_se(sigdelset(&poll_ss, SIGTERM) >= 0);
        }

        প্রতি (;;) {
                যদি (notify >= 0) {
                        r = flush_fd(notify);
                        যদি (r < 0)
                                প্রত্যাগম r;
                }

                /* We pass here O_NOCTTY only so that we can check the return value TIOCSCTTY and have a reliable way
                 * to figure out if we successfully became the controlling process of the tty */
                fd = open_terminal(name, O_RDWR|O_NOCTTY|O_CLOEXEC);
                যদি (fd < 0)
                        প্রত্যাগম fd;

                /* Temporarily ignore SIGHUP, so that we don't get SIGHUP'ed if we already own the tty. */
                সংরচনা sigaction sa_old;
                assert_se(sigaction(SIGHUP, &sigaction_ignore, &sa_old) >= 0);

                /* First, try to get the tty */
                r = RET_NERRNO(ioctl(fd, TIOCSCTTY, mode == ACQUIRE_TERMINAL_FORCE));

                /* Reset signal handler to old value */
                assert_se(sigaction(SIGHUP, &sa_old, রিক্ত) >= 0);

                /* Success? Exit the loop now! */
                যদি (r >= 0)
                        ভঙ্গ;

                /* Any failure besides -EPERM? Fail, regardless of the mode. */
                যদি (r != -EPERM)
                        প্রত্যাগম r;

                যদি (flags & ACQUIRE_TERMINAL_PERMISSIVE) /* If we are in permissive mode, then EPERM is fine, turn this
                                                          * into a success. Note that EPERM is also returned if we
                                                          * already are the owner of the TTY. */
                        ভঙ্গ;

                যদি (mode != ACQUIRE_TERMINAL_WAIT) /* If we are in TRY or FORCE mode, then propagate EPERM as EPERM */
                        প্রত্যাগম r;

                প্রতিজ্ঞা(notify >= 0);
                প্রতিজ্ঞা(wd >= 0);

                প্রতি (;;) {
                        usec_t বাম;
                        যদি (timeout == USEC_INFINITY)
                                বাম = USEC_INFINITY;
                        অন্যথা {
                                প্রতিজ্ঞা(ts != USEC_INFINITY);

                                usec_t n = usec_sub_unsigned(now(CLOCK_MONOTONIC), ts);
                                যদি (n >= timeout)
                                        প্রত্যাগম -ETIMEDOUT;

                                বাম = timeout - n;
                        }

                        r = ppoll_usec_full(
                                        &(সংরচনা pollfd) {
                                                .fd = notify,
                                                .events = POLLIN,
                                        },
                                        /* n_fds= */ 1,
                                        বাম,
                                        &poll_ss);
                        যদি (r < 0)
                                প্রত্যাগম r;
                        যদি (r == 0)
                                প্রত্যাগম -ETIMEDOUT;

                        সঙ্ঘ inotify_event_buffer buffer;
                        ssize_t l;
                        l = পঠ(notify, &buffer, পরিমাণ(buffer));
                        যদি (l < 0) {
                                যদি (ERRNO_IS_TRANSIENT(errno))
                                        অনুবর্ত;

                                প্রত্যাগম -errno;
                        }

                        FOREACH_INOTIFY_EVENT(e, buffer, l) {
                                যদি (e->mask & IN_Q_OVERFLOW) /* If we hit an inotify queue overflow, simply check if the terminal is up for grabs now. */
                                        ভঙ্গ;

                                যদি (e->wd != wd || !(e->mask & IN_CLOSE)) /* Safety checks */
                                        প্রত্যাগম -EIO;
                        }

                        ভঙ্গ;
                }

                /* We close the tty fd here since if the old session ended our handle will be dead. It's important that
                 * we do this after sleeping, so that we don't enter an endless loop. */
                fd = safe_close(fd);
        }

        প্রত্যাগম TAKE_FD(fd);
}

পূর্ণাঙ্ক release_terminal(শূন্য) {
        _cleanup_close_ পূর্ণাঙ্ক fd = -EBADF;
        পূর্ণাঙ্ক r;

        fd = open("/dev/tty", O_RDWR|O_NOCTTY|O_CLOEXEC|O_NONBLOCK);
        যদি (fd < 0)
                প্রত্যাগম -errno;

        /* Temporarily ignore SIGHUP, so that we don't get SIGHUP'ed
         * by our own TIOCNOTTY */
        সংরচনা sigaction sa_old;
        assert_se(sigaction(SIGHUP, &sigaction_ignore, &sa_old) >= 0);

        r = RET_NERRNO(ioctl(fd, TIOCNOTTY));

        assert_se(sigaction(SIGHUP, &sa_old, রিক্ত) >= 0);

        প্রত্যাগম r;
}

পূর্ণাঙ্ক terminal_new_session(শূন্য) {

        /* Make us the new session leader, and set stdin tty to be our controlling terminal.
         *
         * Why stdin? Well, the ctty logic is relevant for signal delivery mostly, i.e. if people hit C-c
         * or the line is hung up. Such events are basically just a form of input, via a side channel
         * (that side channel being signal delivery, i.e. SIGINT, SIGHUP et al). Hence we focus on input,
         * not output here. */

        যদি (!isatty_safe(STDIN_FILENO))
                প্রত্যাগম -ENXIO;

        (শূন্য) setsid();
        প্রত্যাগম RET_NERRNO(ioctl(STDIN_FILENO, TIOCSCTTY, 0));
}

শূন্য terminal_detach_session(শূন্য) {
        (শূন্য) setsid();
        (শূন্য) release_terminal();
}

পূর্ণাঙ্ক terminal_vhangup_fd(পূর্ণাঙ্ক fd) {
        প্রতিজ্ঞা(fd >= 0);
        প্রত্যাগম RET_NERRNO(ioctl(fd, TIOCVHANGUP));
}

পূর্ণাঙ্ক terminal_vhangup(ধ্রুব বর্ণ *tty) {
        _cleanup_close_ পূর্ণাঙ্ক fd = -EBADF;

        প্রতিজ্ঞা(tty);

        fd = open_terminal(tty, O_RDWR|O_NOCTTY|O_CLOEXEC);
        যদি (fd < 0)
                প্রত্যাগম fd;

        প্রত্যাগম terminal_vhangup_fd(fd);
}

পূর্ণাঙ্ক vt_disallocate(ধ্রুব বর্ণ *tty_path) {
        প্রতিজ্ঞা(tty_path);

        /* Deallocate the VT if possible. If not possible (i.e. because it is the active one), at least clear
         * it entirely (including the scrollback buffer). */

        পূর্ণাঙ্ক ttynr = vtnr_from_tty(tty_path);
        যদি (ttynr > 0) {
                _cleanup_close_ পূর্ণাঙ্ক fd = open_terminal("/dev/tty0", O_RDWR|O_NOCTTY|O_CLOEXEC|O_NONBLOCK);
                যদি (fd < 0)
                        প্রত্যাগম fd;

                /* Try to deallocate */
                যদি (ioctl(fd, VT_DISALLOCATE, ttynr) >= 0)
                        প্রত্যাগম 0;
                যদি (errno != EBUSY)
                        প্রত্যাগম -errno;
        }

        /* So this is not a VT (in which case we cannot deallocate it), or we failed to deallocate. Let's at
         * least clear the screen. */

        _cleanup_close_ পূর্ণাঙ্ক fd2 = open_terminal(tty_path, O_WRONLY|O_NOCTTY|O_CLOEXEC|O_NONBLOCK);
        যদি (fd2 < 0)
                প্রত্যাগম fd2;

        প্রত্যাগম loop_write_full(fd2,
                               "\033[r"   /* clear scrolling region */
                               "\033[H"   /* move home */
                               "\033[3J"  /* clear screen including scrollback, requires Linux 2.6.40 */
                               "\033c",   /* reset to initial state */
                               SIZE_MAX,
                               CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
}

স্থির পূর্ণাঙ্ক vt_default_utf8(শূন্য) {
        /* Read the default VT UTF8 setting from the kernel */
        প্রত্যাগম read_boolean_file("/sys/module/vt/parameters/default_utf8");
}

স্থির পূর্ণাঙ্ক vt_reset_keyboard(পূর্ণাঙ্ক fd) {
        পূর্ণাঙ্ক r, kb;

        প্রতিজ্ঞা(fd >= 0);

        /* If we can't read the default, then default to Unicode. It's 2024 after all. */
        r = vt_default_utf8();
        যদি (r < 0)
                log_debug_errno(r, "Failed to determine kernel VT UTF-8 mode, assuming enabled: %m");

        kb = vt_default_utf8() != 0 ? K_UNICODE : K_XLATE;
        প্রত্যাগম RET_NERRNO(ioctl(fd, KDSKBMODE, kb));
}

স্থির পূর্ণাঙ্ক terminal_reset_ioctl(পূর্ণাঙ্ক fd, bool switch_to_text) {
        সংরচনা termios termios;
        পূর্ণাঙ্ক r;

        /* Set terminal to some sane defaults */

        প্রতিজ্ঞা(fd >= 0);

        /* We leave locked terminal attributes untouched, so that Plymouth may set whatever it wants to set,
         * and we don't interfere with that. */

        /* Disable exclusive mode, just in case */
        যদি (ioctl(fd, TIOCNXCL) < 0)
                log_debug_errno(errno, "TIOCNXCL ioctl failed on TTY, ignoring: %m");

        /* Switch to text mode */
        যদি (switch_to_text)
                যদি (ioctl(fd, KDSETMODE, KD_TEXT) < 0)
                        log_debug_errno(errno, "KDSETMODE ioctl for switching to text mode failed on TTY, ignoring: %m");

        /* Set default keyboard mode */
        r = vt_reset_keyboard(fd);
        যদি (r < 0)
                log_debug_errno(r, "Failed to reset VT keyboard, ignoring: %m");

        যদি (tcgetattr(fd, &termios) < 0) {
                r = log_debug_errno(errno, "Failed to get terminal parameters: %m");
                গচ্ছ finish;
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
        যদি (r < 0)
                log_debug_errno(r, "Failed to set terminal parameters: %m");

finish:
        /* Just in case, flush all crap out */
        (শূন্য) tcflush(fd, TCIOFLUSH);

        প্রত্যাগম r;
}

পূর্ণাঙ্ক terminal_reset_ansi_seq(পূর্ণাঙ্ক fd) {
        _cleanup_(nonblock_resetp) পূর্ণাঙ্ক nonblock_reset = -EBADF;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(fd >= 0);

        যদি (getenv_terminal_is_dumb())
                প্রত্যাগম 0;

        r = fd_nonblock(fd, true);
        যদি (r < 0)
                প্রত্যাগম log_debug_errno(r, "Failed to set terminal to non-blocking mode: %m");
        যদি (r > 0)
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
        যদি (r < 0)
                log_debug_errno(r, "Failed to reset terminal through ANSI sequences: %m");

        প্রত্যাগম r;
}

শূন্য reset_dev_console_fd(পূর্ণাঙ্ক fd, bool switch_to_text) {
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(fd >= 0);

        _cleanup_close_ পূর্ণাঙ্ক lock_fd = lock_dev_console();
        যদি (lock_fd < 0)
                log_debug_errno(lock_fd, "Failed to lock /dev/console, ignoring: %m");

        r = terminal_reset_ioctl(fd, switch_to_text);
        যদি (r < 0)
                log_warning_errno(r, "Failed to reset /dev/console, ignoring: %m");

        unsigned rows, cols;
        r = proc_cmdline_tty_size("/dev/console", &rows, &cols);
        যদি (r < 0)
                log_warning_errno(r, "Failed to get /dev/console size, ignoring: %m");
        অন্যথা যদি (r > 0) {
                r = terminal_set_size_fd(fd, রিক্ত, rows, cols);
                যদি (r < 0)
                        log_warning_errno(r, "Failed to set configured terminal size on /dev/console, ignoring: %m");
        } অন্যথা
                (শূন্য) terminal_fix_size(fd, fd);

        r = terminal_reset_ansi_seq(fd);
        যদি (r < 0)
                log_warning_errno(r, "Failed to reset /dev/console using ANSI sequences, ignoring: %m");
}

পূর্ণাঙ্ক lock_dev_console(শূন্য) {
        _cleanup_close_ পূর্ণাঙ্ক fd = -EBADF;
        পূর্ণাঙ্ক r;

        /* NB: We do not use O_NOFOLLOW here, because some container managers might place a symlink to some
         * pty in /dev/console, in which case it should be fine to lock the target TTY. */
        fd = open_terminal("/dev/console", O_RDONLY|O_CLOEXEC|O_NOCTTY);
        যদি (fd < 0)
                প্রত্যাগম fd;

        r = lock_generic(fd, LOCK_BSD, LOCK_EX);
        যদি (r < 0)
                প্রত্যাগম r;

        প্রত্যাগম TAKE_FD(fd);
}

পূর্ণাঙ্ক make_console_stdio(bool switch_to_text) {
        পূর্ণাঙ্ক fd, r;

        /* Make /dev/console the controlling terminal and stdin/stdout/stderr, if we can. If we can't use
         * /dev/null instead. This is particularly useful if /dev/console is turned off, e.g. if console=null
         * is specified on the kernel command line. */

        fd = acquire_terminal("/dev/console", ACQUIRE_TERMINAL_FORCE|ACQUIRE_TERMINAL_PERMISSIVE, USEC_INFINITY);
        যদি (fd < 0) {
                log_warning_errno(fd, "Failed to acquire terminal, using /dev/null stdin/stdout/stderr instead: %m");

                r = make_null_stdio();
                যদি (r < 0)
                        প্রত্যাগম log_error_errno(r, "Failed to make /dev/null stdin/stdout/stderr: %m");

        } অন্যথা {
                reset_dev_console_fd(fd, switch_to_text);

                r = rearrange_stdio(fd, fd, fd); /* This invalidates 'fd' both on success and on failure. */
                যদি (r < 0)
                        প্রত্যাগম log_error_errno(r, "Failed to make terminal stdin/stdout/stderr: %m");
        }

        reset_terminal_feature_caches();
        প্রত্যাগম 0;
}

স্থির পূর্ণাঙ্ক vtnr_from_tty_raw(ধ্রুব বর্ণ *tty, unsigned *ret) {
        প্রতিজ্ঞা(tty);

        tty = skip_dev_prefix(tty);

        ধ্রুব বর্ণ *e = startswith(tty, "tty");
        যদি (!e)
                প্রত্যাগম -EINVAL;

        প্রত্যাগম safe_atou(e, ret);
}

পূর্ণাঙ্ক vtnr_from_tty(ধ্রুব বর্ণ *tty) {
        unsigned u;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(tty);

        r = vtnr_from_tty_raw(tty, &u);
        যদি (r < 0)
                প্রত্যাগম r;
        যদি (!vtnr_is_valid(u))
                প্রত্যাগম -ERANGE;

        প্রত্যাগম (পূর্ণাঙ্ক) u;
}

bool tty_is_vc(ধ্রুব বর্ণ *tty) {
        প্রতিজ্ঞা(tty);

        /* NB: for >= 0 values no range check is conducted here, on the assumption that the caller will
         * either extract vtnr through vtnr_from_tty() later where ERANGE would be reported, or doesn't care
         * about whether it's strictly valid, but only asking "does this fall into the vt category?", for which
         * "yes" seems to be a better answer. */

        প্রত্যাগম vtnr_from_tty_raw(tty, /* ret= */ রিক্ত) >= 0;
}

bool tty_is_console(ধ্রুব বর্ণ *tty) {
        প্রতিজ্ঞা(tty);

        প্রত্যাগম streq(skip_dev_prefix(tty), "console");
}

পূর্ণাঙ্ক resolve_dev_console(বর্ণ **ret) {
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(ret);

        /* Resolve where /dev/console is pointing to. If /dev/console is a symlink (like in container
         * managers), we'll just resolve the symlink. If it's a real device node, we'll use if
         * /sys/class/tty/tty0/active, but only if /sys/ is actually ours (i.e. not read-only-mounted which
         * is a sign for container setups). */

        _cleanup_free_ বর্ণ *chased = রিক্ত;
        r = chase("/dev/console", /* root= */ রিক্ত, /* flags= */ 0, &chased, /* ret_fd= */ রিক্ত);
        যদি (r < 0)
                প্রত্যাগম r;
        যদি (!path_equal(chased, "/dev/console")) {
                *ret = TAKE_PTR(chased);
                প্রত্যাগম 0;
        }

        r = path_is_read_only_fs("/sys");
        যদি (r < 0)
                প্রত্যাগম r;
        যদি (r > 0)
                প্রত্যাগম -ENOMEDIUM;

        _cleanup_free_ বর্ণ *active = রিক্ত;
        r = read_one_সারি_file("/sys/class/tty/console/active", &active);
        যদি (r < 0)
                প্রত্যাগম r;
        যদি (r == 0)
                প্রত্যাগম -ENXIO;

        /* If multiple log outputs are configured the last one is what /dev/console points to */
        ধ্রুব বর্ণ *tty = strrchr(active, ' ');
        যদি (tty)
                tty++;
        অন্যথা
                tty = active;

        যদি (streq(tty, "tty0")) {
                active = mfree(active);

                /* Get the active VC (e.g. tty1) */
                r = read_one_সারি_file("/sys/class/tty/tty0/active", &active);
                যদি (r < 0)
                        প্রত্যাগম r;
                যদি (r == 0)
                        প্রত্যাগম -ENXIO;

                tty = active;
        }

        _cleanup_free_ বর্ণ *path = রিক্ত;
        path = path_join("/dev", tty);
        যদি (!path)
                প্রত্যাগম -ENOMEM;

        *ret = TAKE_PTR(path);
        প্রত্যাগম 0;
}

পূর্ণাঙ্ক get_kernel_consoles(বর্ণ ***ret) {
        _cleanup_strv_free_ বর্ণ **l = রিক্ত;
        _cleanup_free_ বর্ণ *সারি = রিক্ত;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(ret);

        /* If /sys/ is mounted read-only this means we are running in some kind of container environment.
         * In that case /sys/ would reflect the host system, not us, hence ignore the data we can read from it. */
        যদি (path_is_read_only_fs("/sys") > 0)
                গচ্ছ fallback;

        r = read_one_সারি_file("/sys/class/tty/console/active", &সারি);
        যদি (r < 0)
                প্রত্যাগম r;

        প্রতি (ধ্রুব বর্ণ *p = সারি;;) {
                _cleanup_free_ বর্ণ *tty = রিক্ত, *path = রিক্ত;

                r = extract_first_word(&p, &tty, রিক্ত, 0);
                যদি (r < 0)
                        প্রত্যাগম r;
                যদি (r == 0)
                        ভঙ্গ;

                যদি (streq(tty, "tty0")) {
                        tty = mfree(tty);
                        r = read_one_সারি_file("/sys/class/tty/tty0/active", &tty);
                        যদি (r < 0)
                                প্রত্যাগম r;
                        যদি (r == 0) {
                                log_debug("No VT active, skipping /dev/tty0.");
                                অনুবর্ত;
                        }
                }

                path = path_join("/dev", tty);
                যদি (!path)
                        প্রত্যাগম -ENOMEM;

                যদি (access(path, F_OK) < 0) {
                        log_debug_errno(errno, "Console device %s is not accessible, skipping: %m", path);
                        অনুবর্ত;
                }

                r = strv_consume(&l, TAKE_PTR(path));
                যদি (r < 0)
                        প্রত্যাগম r;
        }

        যদি (strv_isempty(l)) {
                log_debug("No devices found for system console");
                গচ্ছ fallback;
        }

        *ret = TAKE_PTR(l);
        প্রত্যাগম strv_length(*ret);

fallback:
        r = strv_extend(&l, "/dev/console");
        যদি (r < 0)
                প্রত্যাগম r;

        *ret = TAKE_PTR(l);
        প্রত্যাগম 0;
}

bool tty_is_vc_resolve(ধ্রুব বর্ণ *tty) {
        _cleanup_free_ বর্ণ *resolved = রিক্ত;

        প্রতিজ্ঞা(tty);

        যদি (streq(skip_dev_prefix(tty), "console")) {
                যদি (resolve_dev_console(&resolved) < 0)
                        প্রত্যাগম false;

                tty = resolved;
        }

        প্রত্যাগম tty_is_vc(tty);
}

পূর্ণাঙ্ক fd_columns(পূর্ণাঙ্ক fd) {
        সংরচনা winsize ws = {};

        যদি (fd < 0)
                প্রত্যাগম -EBADF;

        যদি (ioctl(fd, TIOCGWINSZ, &ws) < 0)
                প্রত্যাগম -errno;

        যদি (ws.ws_col <= 0)
                প্রত্যাগম -ENODATA; /* some tty types come up with invalid row/column initially, return a recognizable error for that */

        প্রত্যাগম ws.ws_col;
}

পূর্ণাঙ্ক getenv_columns(শূন্য) {
        পূর্ণাঙ্ক r;

        ধ্রুব বর্ণ *e = getenv("COLUMNS");
        যদি (!e)
                প্রত্যাগম -ENXIO;

        unsigned c;
        r = safe_atou_bounded(e, 1, USHRT_MAX, &c);
        যদি (r < 0)
                প্রত্যাগম r;

        প্রত্যাগম (পূর্ণাঙ্ক) c;
}

unsigned columns(শূন্য) {

        যদি (cached_columns > 0)
                প্রত্যাগম cached_columns;

        পূর্ণাঙ্ক c = getenv_columns();
        যদি (c < 0) {
                c = fd_columns(STDOUT_FILENO);
                যদি (c < 0)
                        c = 80;
        }

        প্রতিজ্ঞা(c > 0);

        cached_columns = c;
        প্রত্যাগম cached_columns;
}

পূর্ণাঙ্ক fd_সারিগুলি(পূর্ণাঙ্ক fd) {
        সংরচনা winsize ws = {};

        যদি (fd < 0)
                প্রত্যাগম -EBADF;

        যদি (ioctl(fd, TIOCGWINSZ, &ws) < 0)
                প্রত্যাগম -errno;

        যদি (ws.ws_row <= 0)
                প্রত্যাগম -ENODATA; /* some tty types come up with invalid row/column initially, return a recognizable error for that */

        প্রত্যাগম ws.ws_row;
}

unsigned সারিগুলি(শূন্য) {
        ধ্রুব বর্ণ *e;
        পূর্ণাঙ্ক l;

        যদি (cached_সারিগুলি > 0)
                প্রত্যাগম cached_সারিগুলি;

        l = 0;
        e = getenv("LINES");
        যদি (e)
                (শূন্য) safe_atoi(e, &l);

        যদি (l <= 0 || l > USHRT_MAX) {
                l = fd_সারিগুলি(STDOUT_FILENO);
                যদি (l <= 0)
                        l = 24;
        }

        cached_সারিগুলি = l;
        প্রত্যাগম cached_সারিগুলি;
}

পূর্ণাঙ্ক terminal_set_size_fd(পূর্ণাঙ্ক fd, ধ্রুব বর্ণ *ident, unsigned rows, unsigned cols) {
        সংরচনা winsize ws;

        প্রতিজ্ঞা(fd >= 0);

        যদি (!ident)
                ident = "TTY";

        যদি (rows == UINT_MAX && cols == UINT_MAX)
                প্রত্যাগম 0;

        যদি (ioctl(fd, TIOCGWINSZ, &ws) < 0)
                প্রত্যাগম log_debug_errno(errno,
                                       "TIOCGWINSZ ioctl for getting %s size failed, not setting terminal size: %m",
                                       ident);

        যদি (rows == UINT_MAX)
                rows = ws.ws_row;
        অন্যথা যদি (rows > USHRT_MAX)
                rows = USHRT_MAX;

        যদি (cols == UINT_MAX)
                cols = ws.ws_col;
        অন্যথা যদি (cols > USHRT_MAX)
                cols = USHRT_MAX;

        যদি (rows == ws.ws_row && cols == ws.ws_col)
                প্রত্যাগম 0;

        ws.ws_row = rows;
        ws.ws_col = cols;

        যদি (ioctl(fd, TIOCSWINSZ, &ws) < 0)
                প্রত্যাগম log_debug_errno(errno, "TIOCSWINSZ ioctl for setting %s size failed: %m", ident);

        প্রত্যাগম 0;
}

পূর্ণাঙ্ক proc_cmdline_tty_size(ধ্রুব বর্ণ *tty, unsigned *ret_rows, unsigned *ret_cols) {
        _cleanup_free_ বর্ণ *rowskey = রিক্ত, *rowsvalue = রিক্ত, *colskey = রিক্ত, *colsvalue = রিক্ত;
        unsigned rows = UINT_MAX, cols = UINT_MAX;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(tty);

        যদি (!ret_rows && !ret_cols)
                প্রত্যাগম 0;

        tty = skip_dev_prefix(tty);
        যদি (path_startswith(tty, "pts/"))
                প্রত্যাগম -EMEDIUMTYPE;
        যদি (!in_charset(tty, ALPHANUMERICAL))
                প্রত্যাগম log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "TTY name '%s' contains non-alphanumeric characters, not searching kernel cmdline for size.", tty);

        rowskey = strjoin("systemd.tty.rows.", tty);
        যদি (!rowskey)
                প্রত্যাগম -ENOMEM;

        colskey = strjoin("systemd.tty.columns.", tty);
        যদি (!colskey)
                প্রত্যাগম -ENOMEM;

        r = proc_cmdline_get_চাবি_many(/* flags= */ 0,
                                      rowskey, &rowsvalue,
                                      colskey, &colsvalue);
        যদি (r < 0)
                প্রত্যাগম log_debug_errno(r, "Failed to read TTY size of %s from kernel cmdline: %m", tty);

        যদি (rowsvalue) {
                r = safe_atou(rowsvalue, &rows);
                যদি (r < 0)
                        প্রত্যাগম log_debug_errno(r, "Failed to parse %s=%s: %m", rowskey, rowsvalue);
        }

        যদি (colsvalue) {
                r = safe_atou(colsvalue, &cols);
                যদি (r < 0)
                        প্রত্যাগম log_debug_errno(r, "Failed to parse %s=%s: %m", colskey, colsvalue);
        }

        যদি (ret_rows)
                *ret_rows = rows;
        যদি (ret_cols)
                *ret_cols = cols;

        প্রত্যাগম rows != UINT_MAX || cols != UINT_MAX;
}

/* intended to be used as a SIGWINCH sighandler */
শূন্য columns_সারিগুলি_cache_reset(পূর্ণাঙ্ক signum) {
        cached_columns = 0;
        cached_সারিগুলি = 0;
}

শূন্য reset_terminal_feature_caches(শূন্য) {
        cached_columns = 0;
        cached_সারিগুলি = 0;

        cached_on_tty = -1;
        cached_on_dev_null = -1;

        reset_ansi_feature_caches();
}

bool on_tty(শূন্য) {

        /* We check both stdout and stderr, so that situations where pipes on the shell are used are reliably
         * recognized, regardless if only the output or the errors are piped to some place. Since on_tty() is generally
         * used to default to a safer, non-interactive, non-color mode of operation it's probably good to be defensive
         * here, and check for both. Note that we don't check for STDIN_FILENO, because it should fine to use fancy
         * terminal functionality when outputting stuff, even if the input is piped to us. */

        যদি (cached_on_tty < 0)
                cached_on_tty =
                        isatty_safe(STDOUT_FILENO) &&
                        isatty_safe(STDERR_FILENO);

        প্রত্যাগম cached_on_tty;
}

পূর্ণাঙ্ক getttyname_malloc(পূর্ণাঙ্ক fd, বর্ণ **ret) {
        বর্ণ path[PATH_MAX]; /* PATH_MAX is counted *with* the trailing NUL byte */
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(fd >= 0);
        প্রতিজ্ঞা(ret);

        r = ttyname_r(fd, path, পরিমাণ path); /* positive error */
        প্রতিজ্ঞা(r >= 0);
        যদি (r == ERANGE)
                প্রত্যাগম -ENAMETOOLONG;
        যদি (r > 0)
                প্রত্যাগম -r;

        প্রত্যাগম strdup_to(ret, skip_dev_prefix(path));
}

পূর্ণাঙ্ক getttyname_harder(পূর্ণাঙ্ক fd, বর্ণ **ret) {
        _cleanup_free_ বর্ণ *s = রিক্ত;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(ret);

        r = getttyname_malloc(fd, &s);
        যদি (r < 0)
                প্রত্যাগম r;

        যদি (streq(s, "tty"))
                প্রত্যাগম get_ctty(0, রিক্ত, ret);

        *ret = TAKE_PTR(s);
        প্রত্যাগম 0;
}

পূর্ণাঙ্ক get_ctty_devnr(pid_t pid, dev_t *ret) {
        _cleanup_free_ বর্ণ *সারি = রিক্ত;
        unsigned long ttynr;
        ধ্রুব বর্ণ *p;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(pid >= 0);

        p = procfs_file_alloca(pid, "stat");
        r = read_one_সারি_file(p, &সারি);
        যদি (r < 0)
                প্রত্যাগম r;

        p = strrchr(সারি, ')');
        যদি (!p)
                প্রত্যাগম -EIO;

        p++;

        যদি (sscanf(p, " "
                   "%*c "  /* state */
                   "%*d "  /* ppid */
                   "%*d "  /* pgrp */
                   "%*d "  /* session */
                   "%lu ", /* ttynr */
                   &ttynr) != 1)
                প্রত্যাগম -EIO;

        যদি (devnum_is_zero(ttynr))
                প্রত্যাগম -ENXIO;

        যদি (ret)
                *ret = (dev_t) ttynr;

        প্রত্যাগম 0;
}

পূর্ণাঙ্ক get_ctty(pid_t pid, dev_t *ret_devnr, বর্ণ **ret) {
        বর্ণ pty[STRLEN("/dev/pts/") + DECIMAL_STR_MAX(dev_t) + 1];
        _cleanup_free_ বর্ণ *buf = রিক্ত;
        ধ্রুব বর্ণ *fn = রিক্ত, *w;
        dev_t devnr;
        পূর্ণাঙ্ক r;

        r = get_ctty_devnr(pid, &devnr);
        যদি (r < 0)
                প্রত্যাগম r;

        r = device_path_make_canonical(S_IFCHR, devnr, &buf);
        যদি (r < 0) {
                সংরচনা stat st;

                যদি (r != -ENOENT) /* No symlink for this in /dev/char/? */
                        প্রত্যাগম r;

                /* Maybe this is PTY? PTY devices are not listed in /dev/char/, as they don't follow the
                 * Linux device model and hence device_path_make_canonical() doesn't work for them. Let's
                 * assume this is a PTY for a moment, and check if the device node this would then map to in
                 * /dev/pts/ matches the one we are looking for. This way we don't have to hardcode the major
                 * number (which is 136 btw), but we still rely on the fact that PTY numbers map directly to
                 * the minor number of the pty. */
                xsprintf(pty, "/dev/pts/%u", minor(devnr));

                যদি (stat(pty, &st) < 0) {
                        যদি (errno != ENOENT)
                                প্রত্যাগম -errno;

                } অন্যথা যদি (S_ISCHR(st.st_mode) && devnr == st.st_rdev) /* Bingo! */
                        fn = pty;

                যদি (!fn) {
                        /* Doesn't exist, or not a PTY? Probably something similar to the PTYs which have no
                         * symlink in /dev/char/. Let's return something vaguely useful. */
                        r = device_path_make_major_minor(S_IFCHR, devnr, &buf);
                        যদি (r < 0)
                                প্রত্যাগম r;

                        fn = buf;
                }
        } অন্যথা
                fn = buf;

        w = path_startswith(fn, "/dev/");
        যদি (!w)
                প্রত্যাগম -EINVAL;

        যদি (ret) {
                r = strdup_to(ret, w);
                যদি (r < 0)
                        প্রত্যাগম r;
        }

        যদি (ret_devnr)
                *ret_devnr = devnr;

        প্রত্যাগম 0;
}

পূর্ণাঙ্ক ptsname_malloc(পূর্ণাঙ্ক fd, বর্ণ **ret) {
        প্রতিজ্ঞা(fd >= 0);
        প্রতিজ্ঞা(ret);

        প্রতি (size_t l = 50;;) {
                _cleanup_free_ বর্ণ *c = রিক্ত;

                c = new(বর্ণ, l);
                যদি (!c)
                        প্রত্যাগম -ENOMEM;

                যদি (ptsname_r(fd, c, l) >= 0) {
                        *ret = TAKE_PTR(c);
                        প্রত্যাগম 0;
                }
                যদি (errno != ERANGE)
                        প্রত্যাগম -errno;

                যদি (!MUL_ASSIGN_SAFE(&l, 2))
                        প্রত্যাগম -ENOMEM;
        }
}

পূর্ণাঙ্ক openpt_allocate(পূর্ণাঙ্ক flags, বর্ণ **ret_peer_path) {
        _cleanup_close_ পূর্ণাঙ্ক fd = -EBADF;
        পূর্ণাঙ্ক r;

        fd = posix_openpt(flags|O_NOCTTY|O_CLOEXEC);
        যদি (fd < 0)
                প্রত্যাগম -errno;

        _cleanup_free_ বর্ণ *p = রিক্ত;
        যদি (ret_peer_path) {
                r = ptsname_malloc(fd, &p);
                যদি (r < 0)
                        প্রত্যাগম r;

                যদি (!path_startswith(p, "/dev/pts/"))
                        প্রত্যাগম -EINVAL;
        }

        যদি (unlockpt(fd) < 0)
                প্রত্যাগম -errno;

        যদি (ret_peer_path)
                *ret_peer_path = TAKE_PTR(p);

        প্রত্যাগম TAKE_FD(fd);
}

স্থির পূর্ণাঙ্ক ptsname_namespace(পূর্ণাঙ্ক pty, বর্ণ **ret) {
        পূর্ণাঙ্ক no = -1;

        প্রতিজ্ঞা(pty >= 0);
        প্রতিজ্ঞা(ret);

        /* Like ptsname(), but doesn't assume that the path is
         * accessible in the local namespace. */

        যদি (ioctl(pty, TIOCGPTN, &no) < 0)
                প্রত্যাগম -errno;

        যদি (no < 0)
                প্রত্যাগম -EIO;

        যদি (asprintf(ret, "/dev/pts/%i", no) < 0)
                প্রত্যাগম -ENOMEM;

        প্রত্যাগম 0;
}

পূর্ণাঙ্ক openpt_allocate_in_namespace(
                ধ্রুব PidRef *pidref,
                পূর্ণাঙ্ক flags,
                বর্ণ **ret_peer_path) {

        _cleanup_close_ পূর্ণাঙ্ক pidnsfd = -EBADF, mntnsfd = -EBADF, usernsfd = -EBADF, rootfd = -EBADF, fd = -EBADF;
        _cleanup_close_pair_ পূর্ণাঙ্ক pair[2] = EBADF_PAIR;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(pidref);

        r = pidref_namespace_open(pidref, &pidnsfd, &mntnsfd, /* ret_netns_fd= */ রিক্ত, &usernsfd, &rootfd);
        যদি (r < 0)
                প্রত্যাগম log_debug_errno(r, "Failed to open namespaces of PID "PID_FMT": %m", pidref->pid);

        যদি (socketpair(AF_UNIX, SOCK_DGRAM|SOCK_CLOEXEC, 0, pair) < 0)
                প্রত্যাগম -errno;

        r = namespace_fork(
                        "(sd-openptns)",
                        "(sd-openpt)",
                        FORK_RESET_SIGNALS|FORK_DEATHSIG_SIGKILL|FORK_WAIT,
                        pidnsfd,
                        mntnsfd,
                        /* netns_fd= */ -EBADF,
                        usernsfd,
                        rootfd,
                        /* ret= */ রিক্ত);
        যদি (r < 0)
                প্রত্যাগম r;
        যদি (r == 0) {
                pair[0] = safe_close(pair[0]);

                fd = openpt_allocate(flags, /* ret_peer_path= */ রিক্ত);
                যদি (fd < 0)
                        _exit(EXIT_FAILURE);

                যদি (send_one_fd(pair[1], fd, 0) < 0)
                        _exit(EXIT_FAILURE);

                _exit(EXIT_SUCCESS);
        }

        pair[1] = safe_close(pair[1]);

        fd = receive_one_fd(pair[0], 0);
        যদি (fd < 0)
                প্রত্যাগম fd;

        যদি (ret_peer_path) {
                r = ptsname_namespace(fd, ret_peer_path);
                যদি (r < 0)
                        প্রত্যাগম r;
        }

        প্রত্যাগম TAKE_FD(fd);
}

স্থির bool on_dev_null(শূন্য) {
        সংরচনা stat dst, ost, est;

        যদি (cached_on_dev_null >= 0)
                প্রত্যাগম cached_on_dev_null;

        যদি (stat("/dev/null", &dst) < 0 || fstat(STDOUT_FILENO, &ost) < 0 || fstat(STDERR_FILENO, &est) < 0)
                cached_on_dev_null = false;
        অন্যথা
                cached_on_dev_null = stat_inode_same(&dst, &ost) && stat_inode_same(&dst, &est);

        প্রত্যাগম cached_on_dev_null;
}

bool term_env_valid(ধ্রুব বর্ণ *term) {
        /* Checks if the specified $TERM value is suitable for propagation, i.e. is not empty, not set to
         * "unknown" (as is common in CI), and only contains characters valid in terminal type names.
         * Valid $TERM values are things like "xterm-256color", "linux", "screen.xterm-256color", i.e.
         * alphanumeric characters, hyphens, underscores, dots, and plus signs. */
        প্রত্যাগম !isempty(term) &&
                !streq(term, "unknown") &&
                in_charset(term, ALPHANUMERICAL "-_+.");
}

bool getenv_terminal_is_dumb(শূন্য) {
        ধ্রুব বর্ণ *e;

        e = getenv("TERM");
        যদি (!e)
                প্রত্যাগম true;

        প্রত্যাগম streq(e, "dumb");
}

bool terminal_is_dumb(শূন্য) {
        যদি (!on_tty() && !on_dev_null())
                প্রত্যাগম true;

        প্রত্যাগম getenv_terminal_is_dumb();
}

bool dev_console_colors_enabled(শূন্য) {
        _cleanup_free_ বর্ণ *s = রিক্ত;
        ColorMode m;

        /* Returns true if we assume that color is supported on /dev/console.
         *
         * For that we first check if we explicitly got told to use colors or not, by checking $SYSTEMD_COLORS. If that
         * isn't set we check whether PID 1 has $TERM set, and if not, whether TERM is set on the kernel command
         * line. If we find $TERM set we assume color if it's not set to "dumb", similarly to how regular
         * colors_enabled() operates. */

        m = parse_systemd_colors();
        যদি (m >= 0)
                প্রত্যাগম m;

        যদি (getenv("NO_COLOR"))
                প্রত্যাগম false;

        যদি (getenv_for_pid(1, "TERM", &s) <= 0)
                (শূন্য) proc_cmdline_get_চাবি("TERM", 0, &s);

        প্রত্যাগম !streq_ptr(s, "dumb");
}

পূর্ণাঙ্ক vt_restore(পূর্ণাঙ্ক fd) {

        স্থির ধ্রুব সংরচনা vt_mode mode = {
                .mode = VT_AUTO,
        };

        পূর্ণাঙ্ক r, ret = 0;

        প্রতিজ্ঞা(fd >= 0);

        যদি (!isatty_safe(fd))
                প্রত্যাগম log_debug_errno(SYNTHETIC_ERRNO(ENOTTY), "Asked to restore the VT for an fd that does not refer to a terminal.");

        যদি (ioctl(fd, KDSETMODE, KD_TEXT) < 0)
                RET_GATHER(ret, log_debug_errno(errno, "Failed to set VT to text mode, ignoring: %m"));

        r = vt_reset_keyboard(fd);
        যদি (r < 0)
                RET_GATHER(ret, log_debug_errno(r, "Failed to reset keyboard mode, ignoring: %m"));

        যদি (ioctl(fd, VT_SETMODE, &mode) < 0)
                RET_GATHER(ret, log_debug_errno(errno, "Failed to set VT_AUTO mode, ignoring: %m"));

        r = fchmod_and_chown(fd, TTY_MODE, 0, GID_INVALID);
        যদি (r < 0)
                RET_GATHER(ret, log_debug_errno(r, "Failed to chmod()/chown() VT, ignoring: %m"));

        প্রত্যাগম ret;
}

পূর্ণাঙ্ক vt_release(পূর্ণাঙ্ক fd, bool restore) {
        প্রতিজ্ঞা(fd >= 0);

        /* This function releases the VT by acknowledging the VT-switch signal
         * sent by the kernel and optionally reset the VT in text and auto
         * VT-switching modes. */

        যদি (!isatty_safe(fd))
                প্রত্যাগম log_debug_errno(SYNTHETIC_ERRNO(ENOTTY), "Asked to release the VT for an fd that does not refer to a terminal.");

        যদি (ioctl(fd, VT_RELDISP, 1) < 0)
                প্রত্যাগম -errno;

        যদি (restore)
                প্রত্যাগম vt_restore(fd);

        প্রত্যাগম 0;
}

শূন্য get_log_colors(পূর্ণাঙ্ক priority, ধ্রুব বর্ণ **on, ধ্রুব বর্ণ **off, ধ্রুব বর্ণ **highlight) {
        /* Note that this will initialize output variables only when there's something to output.
         * The caller must pre-initialize to "" or NULL as appropriate. */

        যদি (priority <= LOG_ERR) {
                যদি (on)
                        *on = ansi_highlight_red();
                যদি (off)
                        *off = ansi_normal();
                যদি (highlight)
                        *highlight = ansi_highlight();

        } অন্যথা যদি (priority <= LOG_WARNING) {
                যদি (on)
                        *on = ansi_highlight_yellow();
                যদি (off)
                        *off = ansi_normal();
                যদি (highlight)
                        *highlight = ansi_highlight();

        } অন্যথা যদি (priority <= LOG_NOTICE) {
                যদি (on)
                        *on = ansi_highlight();
                যদি (off)
                        *off = ansi_normal();
                যদি (highlight)
                        *highlight = ansi_highlight_red();

        } অন্যথা যদি (priority >= LOG_DEBUG) {
                যদি (on)
                        *on = ansi_grey();
                যদি (off)
                        *off = ansi_normal();
                যদি (highlight)
                        *highlight = ansi_highlight_red();
        }
}

পূর্ণাঙ্ক terminal_set_cursor_position(পূর্ণাঙ্ক fd, unsigned row, unsigned column) {
        প্রতিজ্ঞা(fd >= 0);

        বর্ণ cursor_position[STRLEN("\x1B[" ";" "H") + DECIMAL_STR_MAX(unsigned) * 2 + 1];
        xsprintf(cursor_position, "\x1B[%u;%uH", row, column);

        প্রত্যাগম loop_write(fd, cursor_position, SIZE_MAX);
}

স্থির পূর্ণাঙ্ক terminal_verify_same(পূর্ণাঙ্ক প্রবেশ_fd, পূর্ণাঙ্ক output_fd) {
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(প্রবেশ_fd >= 0);
        প্রতিজ্ঞা(output_fd >= 0);

        /* Validates that the specified fds reference the same TTY */

        যদি (প্রবেশ_fd != output_fd) {
                সংরচনা stat sti;
                যদি (fstat(প্রবেশ_fd, &sti) < 0)
                        প্রত্যাগম -errno;

                r = stat_verify_char(&sti); /* TTYs are character devices */
                যদি (r < 0)
                        প্রত্যাগম r;

                সংরচনা stat sto;
                যদি (fstat(output_fd, &sto) < 0)
                        প্রত্যাগম -errno;

                r = stat_verify_char(&sto);
                যদি (r < 0)
                        প্রত্যাগম r;

                যদি (sti.st_rdev != sto.st_rdev)
                        প্রত্যাগম -ENOLINK;
        }

        যদি (!isatty_safe(প্রবেশ_fd)) /* The check above was just for char device, but now let's ensure it's actually a tty */
                প্রত্যাগম -ENOTTY;

        প্রত্যাগম 0;
}

প্রকারনির্দেশ গণনা CursorPositionState {
        CURSOR_TEXT,
        CURSOR_ESCAPE,
        CURSOR_ROW,
        CURSOR_COLUMN,
} CursorPositionState;

প্রকারনির্দেশ সংরচনা CursorPositionContext {
        CursorPositionState state;
        unsigned row, column;
} CursorPositionContext;

স্থির পূর্ণাঙ্ক scan_cursor_position_response(
                CursorPositionContext *context,
                ধ্রুব বর্ণ *buf,
                size_t size,
                size_t *ret_processed) {

        প্রতিজ্ঞা(context);
        প্রতিজ্ঞা(buf);
        প্রতিজ্ঞা(ret_processed);

        প্রতি (size_t i = 0; i < size; i++) {
                বর্ণ c = buf[i];

                বিকল্প (context->state) {

                পক্ষ CURSOR_TEXT:
                        context->state = c == '\x1B' ? CURSOR_ESCAPE : CURSOR_TEXT;
                        ভঙ্গ;

                পক্ষ CURSOR_ESCAPE:
                        context->state = c == '[' ? CURSOR_ROW : CURSOR_TEXT;
                        ভঙ্গ;

                পক্ষ CURSOR_ROW:
                        যদি (c == ';')
                                context->state = context->row > 0 ? CURSOR_COLUMN : CURSOR_TEXT;
                        অন্যথা {
                                পূর্ণাঙ্ক d = undecchar(c);

                                /* We read a decimal character, let's suffix it to the number we so far read,
                                 * but let's do an overflow check first. */
                                যদি (d < 0 || context->row > (UINT_MAX-d)/10)
                                        context->state = CURSOR_TEXT;
                                অন্যথা
                                        context->row = context->row * 10 + d;
                        }
                        ভঙ্গ;

                পক্ষ CURSOR_COLUMN:
                        যদি (c == 'R') {
                                যদি (context->column > 0) {
                                        *ret_processed = i + 1;
                                        প্রত্যাগম 1; /* success! */
                                }

                                context->state = CURSOR_TEXT;
                        } অন্যথা {
                                পূর্ণাঙ্ক d = undecchar(c);

                                /* As above, add the decimal character to our column number */
                                যদি (d < 0 || context->column > (UINT_MAX-d)/10)
                                        context->state = CURSOR_TEXT;
                                অন্যথা
                                        context->column = context->column * 10 + d;
                        }

                        ভঙ্গ;
                }

                /* Reset any positions we might have picked up */
                যদি (IN_SET(context->state, CURSOR_TEXT, CURSOR_ESCAPE))
                        context->row = context->column = 0;
        }

        *ret_processed = size;
        প্রত্যাগম 0; /* all good, but not enough data yet */
}

পূর্ণাঙ্ক terminal_get_cursor_position(
                পূর্ণাঙ্ক প্রবেশ_fd,
                পূর্ণাঙ্ক output_fd,
                unsigned *ret_row,
                unsigned *ret_column) {

        _cleanup_close_ পূর্ণাঙ্ক nonblock_প্রবেশ_fd = -EBADF;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(প্রবেশ_fd >= 0);
        প্রতিজ্ঞা(output_fd >= 0);

        যদি (getenv_terminal_is_dumb())
                প্রত্যাগম -EOPNOTSUPP;

        r = terminal_verify_same(প্রবেশ_fd, output_fd);
        যদি (r < 0)
                প্রত্যাগম log_debug_errno(r, "Called with distinct input/output fds: %m");

        /* Failure to reset the terminal is ignored here and in similar cases below.
         * We already have our result; if cleanup fails it doesn't change the validity of the result. */
        সংরচনা termios old_termios = TERMIOS_NULL;
        CLEANUP_TERMIOS_RESET(প্রবেশ_fd, old_termios);

        যদি (tcgetattr(প্রবেশ_fd, &old_termios) < 0)
                প্রত্যাগম log_debug_errno(errno, "Failed to get terminal settings: %m");

        সংরচনা termios new_termios = old_termios;
        termios_disable_echo(&new_termios);

        যদি (tcsetattr(প্রবেশ_fd, TCSANOW, &new_termios) < 0)
                প্রত্যাগম log_debug_errno(errno, "Failed to set new terminal settings: %m");

        /* Request cursor position (DSR/CPR) */
        r = loop_write(output_fd, "\x1B[6n", SIZE_MAX);
        যদি (r < 0)
                প্রত্যাগম r;

        /* Open a 2nd input fd, in non-blocking mode, so that we won't ever hang in read() should someone
         * else process the POLLIN. */

        nonblock_প্রবেশ_fd = r = fd_reopen(প্রবেশ_fd, O_RDONLY|O_CLOEXEC|O_NONBLOCK|O_NOCTTY);
        যদি (r < 0)
                প্রত্যাগম r;

        usec_t end = usec_add(now(CLOCK_MONOTONIC), CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        বর্ণ buf[STRLEN("\x1B[1;1R")]; /* The shortest valid reply possible */
        size_t buf_full = 0;
        CursorPositionContext context = {};

        প্রতি (bool first = true;; first = false) {
                যদি (buf_full == 0) {
                        usec_t n = now(CLOCK_MONOTONIC);
                        যদি (n >= end)
                                প্রত্যাগম -EOPNOTSUPP;

                        r = fd_wait_for_event(nonblock_প্রবেশ_fd, POLLIN, usec_sub_unsigned(end, n));
                        যদি (r < 0)
                                প্রত্যাগম r;
                        যদি (r == 0)
                                প্রত্যাগম -EOPNOTSUPP;

                        /* On the first try, read multiple characters, i.e. the shortest valid
                         * reply. Afterwards read byte-wise, since we don't want to read too much, and
                         * unnecessarily drop too many characters from the input queue. */
                        ssize_t l = পঠ(nonblock_প্রবেশ_fd, buf, first ? পরিমাণ(buf) : 1);
                        যদি (l < 0) {
                                যদি (errno == EAGAIN)
                                        অনুবর্ত;

                                প্রত্যাগম -errno;
                        }

                        প্রতিজ্ঞা((size_t) l <= পরিমাণ(buf));
                        buf_full = l;
                }

                size_t processed;
                r = scan_cursor_position_response(&context, buf, buf_full, &processed);
                যদি (r < 0)
                        প্রত্যাগম r;

                প্রতিজ্ঞা(processed <= buf_full);
                buf_full -= processed;
                memmove(buf, buf + processed, buf_full);

                যদি (r > 0) {
                        /* Superficial validity check */
                        যদি (context.row >= 32766 || context.column >= 32766)
                                প্রত্যাগম -ENODATA;

                        যদি (ret_row)
                                *ret_row = context.row;
                        যদি (ret_column)
                                *ret_column = context.column;

                        প্রত্যাগম 0;
                }
        }
}

পূর্ণাঙ্ক terminal_reset_defensive(পূর্ণাঙ্ক fd, TerminalResetFlags flags) {
        পূর্ণাঙ্ক r = 0;

        প্রতিজ্ঞা(fd >= 0);
        প্রতিজ্ঞা(!FLAGS_SET(flags, TERMINAL_RESET_AVOID_ANSI_SEQ|TERMINAL_RESET_FORCE_ANSI_SEQ));

        /* Resets the terminal comprehensively, i.e. via both ioctl()s and via ANSI sequences, but do so only
         * if $TERM is unset or set to "dumb" */

        যদি (!isatty_safe(fd))
                প্রত্যাগম -ENOTTY;

        RET_GATHER(r, terminal_reset_ioctl(fd, FLAGS_SET(flags, TERMINAL_RESET_SWITCH_TO_TEXT)));

        যদি (!FLAGS_SET(flags, TERMINAL_RESET_AVOID_ANSI_SEQ) &&
            (FLAGS_SET(flags, TERMINAL_RESET_FORCE_ANSI_SEQ) || !getenv_terminal_is_dumb()))
                RET_GATHER(r, terminal_reset_ansi_seq(fd));

        প্রত্যাগম r;
}

পূর্ণাঙ্ক terminal_reset_defensive_locked(পূর্ণাঙ্ক fd, TerminalResetFlags flags) {
        প্রতিজ্ঞা(fd >= 0);

        _cleanup_close_ পূর্ণাঙ্ক lock_fd = lock_dev_console();
        যদি (lock_fd < 0)
                log_debug_errno(lock_fd, "Failed to acquire lock for /dev/console, ignoring: %m");

        প্রত্যাগম terminal_reset_defensive(fd, flags);
}

শূন্য termios_disable_echo(সংরচনা termios *termios) {
        প্রতিজ্ঞা(termios);

        termios->c_lflag &= ~(ICANON|ECHO);
        termios->c_cc[VMIN] = 1;
        termios->c_cc[VTIME] = 0;
}

স্থির bool termios_is_null(ধ্রুব সংরচনা termios *t) {
        যদি (!t)
                প্রত্যাগম true;

        প্রত্যাগম t->c_iflag == UINT_MAX &&
               t->c_oflag == UINT_MAX &&
               t->c_cflag == UINT_MAX &&
               t->c_lflag == UINT_MAX;
}

শূন্য termios_reset(ধ্রুব TermiosResetContext *c) {
        প্রতিজ্ঞা(c);

        PROTECT_ERRNO;

        যদি (c->fd && *c->fd >= 0 && !termios_is_null(c->termios))
                (শূন্য) tcsetattr(*c->fd, TCSANOW, c->termios);
}

প্রকারনির্দেশ গণনা BackgroundColorState {
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

প্রকারনির্দেশ সংরচনা BackgroundColorContext {
        BackgroundColorState state;
        uint32_t red, green, blue;
        unsigned red_bits, green_bits, blue_bits;
} BackgroundColorContext;

স্থির পূর্ণাঙ্ক scan_background_color_response(
                BackgroundColorContext *context,
                ধ্রুব বর্ণ *buf,
                size_t size,
                size_t *ret_processed) {

        প্রতিজ্ঞা(context);
        প্রতিজ্ঞা(buf);
        প্রতিজ্ঞা(ret_processed);

        প্রতি (size_t i = 0; i < size; i++) {
                বর্ণ c = buf[i];

                বিকল্প (context->state) {

                পক্ষ BACKGROUND_TEXT:
                        context->state = c == '\x1B' ? BACKGROUND_ESCAPE : BACKGROUND_TEXT;
                        ভঙ্গ;

                পক্ষ BACKGROUND_ESCAPE:
                        context->state = c == ']' ? BACKGROUND_BRACKET : BACKGROUND_TEXT;
                        ভঙ্গ;

                পক্ষ BACKGROUND_BRACKET:
                        context->state = c == '1' ? BACKGROUND_FIRST_ONE : BACKGROUND_TEXT;
                        ভঙ্গ;

                পক্ষ BACKGROUND_FIRST_ONE:
                        context->state = c == '1' ? BACKGROUND_SECOND_ONE : BACKGROUND_TEXT;
                        ভঙ্গ;

                পক্ষ BACKGROUND_SECOND_ONE:
                        context->state = c == ';' ? BACKGROUND_SEMICOLON : BACKGROUND_TEXT;
                        ভঙ্গ;

                পক্ষ BACKGROUND_SEMICOLON:
                        context->state = c == 'r' ? BACKGROUND_R : BACKGROUND_TEXT;
                        ভঙ্গ;

                পক্ষ BACKGROUND_R:
                        context->state = c == 'g' ? BACKGROUND_G : BACKGROUND_TEXT;
                        ভঙ্গ;

                পক্ষ BACKGROUND_G:
                        context->state = c == 'b' ? BACKGROUND_B : BACKGROUND_TEXT;
                        ভঙ্গ;

                পক্ষ BACKGROUND_B:
                        context->state = c == ':' ? BACKGROUND_RED : BACKGROUND_TEXT;
                        ভঙ্গ;

                পক্ষ BACKGROUND_RED:
                        যদি (c == '/')
                                context->state = context->red_bits > 0 ? BACKGROUND_GREEN : BACKGROUND_TEXT;
                        অন্যথা {
                                পূর্ণাঙ্ক d = unhexchar(c);
                                যদি (d < 0 || context->red_bits >= পরিমাণ(context->red)*8)
                                        context->state = BACKGROUND_TEXT;
                                অন্যথা {
                                        context->red = (context->red << 4) | d;
                                        context->red_bits += 4;
                                }
                        }
                        ভঙ্গ;

                পক্ষ BACKGROUND_GREEN:
                        যদি (c == '/')
                                context->state = context->green_bits > 0 ? BACKGROUND_BLUE : BACKGROUND_TEXT;
                        অন্যথা {
                                পূর্ণাঙ্ক d = unhexchar(c);
                                যদি (d < 0 || context->green_bits >= পরিমাণ(context->green)*8)
                                        context->state = BACKGROUND_TEXT;
                                অন্যথা {
                                        context->green = (context->green << 4) | d;
                                        context->green_bits += 4;
                                }
                        }
                        ভঙ্গ;

                পক্ষ BACKGROUND_BLUE:
                        যদি (c == '\x07') {
                                যদি (context->blue_bits > 0) {
                                        *ret_processed = i + 1;
                                        প্রত্যাগম 1; /* success! */
                                }

                                context->state = BACKGROUND_TEXT;
                        } অন্যথা যদি (c == '\x1b')
                                context->state = context->blue_bits > 0 ? BACKGROUND_STRING_TERMINATOR : BACKGROUND_TEXT;
                        অন্যথা {
                                পূর্ণাঙ্ক d = unhexchar(c);
                                যদি (d < 0 || context->blue_bits >= পরিমাণ(context->blue)*8)
                                        context->state = BACKGROUND_TEXT;
                                অন্যথা {
                                        context->blue = (context->blue << 4) | d;
                                        context->blue_bits += 4;
                                }
                        }
                        ভঙ্গ;

                পক্ষ BACKGROUND_STRING_TERMINATOR:
                        যদি (c == '\\') {
                                *ret_processed = i + 1;
                                প্রত্যাগম 1; /* success! */
                        }

                        context->state = c == ']' ? BACKGROUND_ESCAPE : BACKGROUND_TEXT;
                        ভঙ্গ;

                }

                /* Reset any colors we might have picked up */
                যদি (IN_SET(context->state, BACKGROUND_TEXT, BACKGROUND_ESCAPE)) {
                        /* reset color */
                        context->red = context->green = context->blue = 0;
                        context->red_bits = context->green_bits = context->blue_bits = 0;
                }
        }

        *ret_processed = size;
        প্রত্যাগম 0; /* all good, but not enough data yet */
}

পূর্ণাঙ্ক get_default_background_color(দ্বিগুণ *ret_red, দ্বিগুণ *ret_green, দ্বিগুণ *ret_blue) {
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(ret_red);
        প্রতিজ্ঞা(ret_green);
        প্রতিজ্ঞা(ret_blue);

        যদি (!colors_enabled())
                প্রত্যাগম -EOPNOTSUPP;

        r = terminal_verify_same(STDIN_FILENO, STDOUT_FILENO);
        যদি (r < 0)
                প্রত্যাগম r;

        যদি (streq_ptr(getenv("TERM"), "linux")) {
                /* Linux console is black */
                *ret_red = *ret_green = *ret_blue = 0.0;
                প্রত্যাগম 0;
        }

        /* Open a 2nd input fd, in non-blocking mode, so that we won't ever hang in read()
         * should someone else process the POLLIN. Do all subsequent operations on the new fd. */
        _cleanup_close_ পূর্ণাঙ্ক nonblock_প্রবেশ_fd = r = fd_reopen(STDIN_FILENO, O_RDONLY|O_CLOEXEC|O_NONBLOCK|O_NOCTTY);
        যদি (r < 0)
                প্রত্যাগম r;

        সংরচনা termios old_termios = TERMIOS_NULL;
        CLEANUP_TERMIOS_RESET(nonblock_প্রবেশ_fd, old_termios);

        যদি (tcgetattr(nonblock_প্রবেশ_fd, &old_termios) < 0)
                প্রত্যাগম -errno;

        সংরচনা termios new_termios = old_termios;
        termios_disable_echo(&new_termios);

        যদি (tcsetattr(nonblock_প্রবেশ_fd, TCSANOW, &new_termios) < 0)
                প্রত্যাগম -errno;

        r = loop_write(STDOUT_FILENO, ANSI_OSC "11;?" ANSI_ST, SIZE_MAX);
        যদি (r < 0)
                প্রত্যাগম r;

        usec_t end = usec_add(now(CLOCK_MONOTONIC), CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        বর্ণ buf[STRLEN(ANSI_OSC "11;rgb:0/0/0" ANSI_ST)]; /* shortest possible reply */
        size_t buf_full = 0;
        BackgroundColorContext context = {};

        প্রতি (bool first = true;; first = false) {
                যদি (buf_full == 0) {
                        usec_t n = now(CLOCK_MONOTONIC);
                        যদি (n >= end)
                                প্রত্যাগম -EOPNOTSUPP;

                        r = fd_wait_for_event(nonblock_প্রবেশ_fd, POLLIN, usec_sub_unsigned(end, n));
                        যদি (r < 0)
                                প্রত্যাগম r;
                        যদি (r == 0)
                                প্রত্যাগম -EOPNOTSUPP;

                        /* On the first try, read multiple characters, i.e. the shortest valid
                         * reply. Afterwards read byte-wise, since we don't want to read too much, and
                         * unnecessarily drop too many characters from the input queue. */
                        ssize_t l = পঠ(nonblock_প্রবেশ_fd, buf, first ? পরিমাণ(buf) : 1);
                        যদি (l < 0) {
                                যদি (errno == EAGAIN)
                                        অনুবর্ত;
                                প্রত্যাগম -errno;
                        }

                        প্রতিজ্ঞা((size_t) l <= পরিমাণ(buf));
                        buf_full = l;
                }

                size_t processed;
                r = scan_background_color_response(&context, buf, buf_full, &processed);
                যদি (r < 0)
                        প্রত্যাগম r;

                প্রতিজ্ঞা(processed <= buf_full);
                buf_full -= processed;
                memmove(buf, buf + processed, buf_full);

                যদি (r > 0) {
                        প্রতিজ্ঞা(context.red_bits > 0);
                        *ret_red = (দ্বিগুণ) context.red / ((UINT64_C(1) << context.red_bits) - 1);
                        প্রতিজ্ঞা(context.green_bits > 0);
                        *ret_green = (দ্বিগুণ) context.green / ((UINT64_C(1) << context.green_bits) - 1);
                        প্রতিজ্ঞা(context.blue_bits > 0);
                        *ret_blue = (দ্বিগুণ) context.blue / ((UINT64_C(1) << context.blue_bits) - 1);
                        প্রত্যাগম 0;
                }
        }
}

/* Determine terminal dimensions by means of ANSI sequences: save the cursor via DECSC, position it far
 * to the bottom right (clamped to actual terminal dimensions), read back via DSR where we ended up, and
 * restore cursor via DECRC. Only needs a single DSR round-trip, and always restores the cursor regardless
 * of whether the response is received.
 *
 * Caller must have already opened a non-blocking input fd and configured termios (echo/icanon off). */
স্থির পূর্ণাঙ্ক terminal_query_size_by_dsr(
                পূর্ণাঙ্ক nonblock_প্রবেশ_fd,
                পূর্ণাঙ্ক output_fd,
                unsigned *ret_rows,
                unsigned *ret_columns) {

        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(nonblock_প্রবেশ_fd >= 0);
        প্রতিজ্ঞা(output_fd >= 0);

        /* Use DECSC/DECRC to save/restore cursor instead of querying position via DSR. This way the cursor
         * is always restored — even on timeout — and we only need one DSR response instead of two. */
        r = loop_write_full(output_fd,
                            "\x1B" "7"              /* DECSC: save cursor position */
                            "\x1B[32766;32766H"     /* CUP: position cursor far to the right and to the bottom, staying within 16bit signed range */
                            "\x1B[6n"               /* DSR: request cursor position (CPR) */
                            "\x1B" "8",             /* DECRC: restore cursor position */
                            SIZE_MAX,
                            CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        যদি (r < 0)
                প্রত্যাগম r;

        usec_t end = usec_add(now(CLOCK_MONOTONIC), CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        বর্ণ buf[STRLEN("\x1B[1;1R")]; /* The shortest valid reply possible */
        size_t buf_full = 0;
        CursorPositionContext context = {};

        প্রতি (bool first = true;; first = false) {
                যদি (buf_full == 0) {
                        usec_t n = now(CLOCK_MONOTONIC);
                        যদি (n >= end)
                                প্রত্যাগম -EOPNOTSUPP;

                        r = fd_wait_for_event(nonblock_প্রবেশ_fd, POLLIN, usec_sub_unsigned(end, n));
                        যদি (r < 0)
                                প্রত্যাগম r;
                        যদি (r == 0)
                                প্রত্যাগম -EOPNOTSUPP;

                        /* On the first try, read multiple characters, i.e. the shortest valid
                         * reply. Afterwards read byte-wise, since we don't want to read too much, and
                         * unnecessarily drop too many characters from the input queue. */
                        ssize_t l = পঠ(nonblock_প্রবেশ_fd, buf, first ? পরিমাণ(buf) : 1);
                        যদি (l < 0) {
                                যদি (errno == EAGAIN)
                                        অনুবর্ত;

                                প্রত্যাগম -errno;
                        }

                        প্রতিজ্ঞা((size_t) l <= পরিমাণ(buf));
                        buf_full = l;
                }

                size_t processed;
                r = scan_cursor_position_response(&context, buf, buf_full, &processed);
                যদি (r < 0)
                        প্রত্যাগম r;

                প্রতিজ্ঞা(processed <= buf_full);
                buf_full -= processed;
                memmove(buf, buf + processed, buf_full);

                যদি (r > 0) {
                        /* Superficial validity checks (no particular reason to check for < 4, it's
                         * just a way to look for unreasonably small values) */
                        যদি (context.row < 4 || context.column < 4 || context.row >= 32766 || context.column >= 32766)
                                প্রত্যাগম -ENODATA;

                        যদি (ret_rows)
                                *ret_rows = context.row;
                        যদি (ret_columns)
                                *ret_columns = context.column;

                        প্রত্যাগম 0;
                }
        }
}

/* Common setup for ANSI terminal queries: validate the fds, open a non-blocking input fd, and configure
 * termios with echo and canonical mode disabled. Caller must restore termios and close the fd when done. */
স্থির পূর্ণাঙ্ক terminal_prepare_query(
                পূর্ণাঙ্ক প্রবেশ_fd,
                পূর্ণাঙ্ক output_fd,
                পূর্ণাঙ্ক *ret_nonblock_fd,
                সংরচনা termios *ret_saved_termios) {

        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(প্রবেশ_fd >= 0);
        প্রতিজ্ঞা(output_fd >= 0);
        প্রতিজ্ঞা(ret_nonblock_fd);
        প্রতিজ্ঞা(ret_saved_termios);

        /* Use getenv_terminal_is_dumb() instead of terminal_is_dumb() here since we operate on an
         * explicitly passed fd, not on stdio. terminal_is_dumb() additionally checks on_tty() which
         * tests whether *stderr* is a tty — that's irrelevant when we're querying a directly opened
         * terminal such as /dev/console. */
        যদি (getenv_terminal_is_dumb())
                প্রত্যাগম -EOPNOTSUPP;

        r = terminal_verify_same(প্রবেশ_fd, output_fd);
        যদি (r < 0)
                প্রত্যাগম log_debug_errno(r, "Called with distinct input/output fds: %m");

        /* Open a 2nd input fd, in non-blocking mode, so that we won't ever hang in read()
         * should someone else process the POLLIN. Do all subsequent operations on the new fd. */
        _cleanup_close_ পূর্ণাঙ্ক nonblock_প্রবেশ_fd = r = fd_reopen(প্রবেশ_fd, O_RDONLY|O_CLOEXEC|O_NONBLOCK|O_NOCTTY);
        যদি (r < 0)
                প্রত্যাগম r;

        যদি (tcgetattr(nonblock_প্রবেশ_fd, ret_saved_termios) < 0)
                প্রত্যাগম log_debug_errno(errno, "Failed to get terminal settings: %m");

        সংরচনা termios new_termios = *ret_saved_termios;
        termios_disable_echo(&new_termios);

        যদি (tcsetattr(nonblock_প্রবেশ_fd, TCSANOW, &new_termios) < 0)
                প্রত্যাগম log_debug_errno(errno, "Failed to set new terminal settings: %m");

        *ret_nonblock_fd = TAKE_FD(nonblock_প্রবেশ_fd);
        প্রত্যাগম 0;
}

/*
 * See https://terminalguide.namepad.de/seq/csi_st-18/,
 * https://invisible-island.net/xterm/ctlseqs/ctlseqs.html#h3-Functions-using-CSI-_-ordered-by-the-final-character_s_.
 */
#define CSI18_Q  "\x1B[18t"               /* Report the size of the text area in characters */
#define CSI18_Rp "\x1B[8;"                /* Reply prefix */
#define CSI18_R0 CSI18_Rp "1;1t"          /* Shortest reply */
#define CSI18_R1 CSI18_Rp "32766;32766t"  /* Longest reply */

স্থির পূর্ণাঙ্ক scan_text_area_size_response(
                ধ্রুব বর্ণ *buf,
                size_t size,
                unsigned *ret_rows,
                unsigned *ret_columns) {

        প্রতিজ্ঞা(buf);
        প্রতিজ্ঞা(ret_rows);
        প্রতিজ্ঞা(ret_columns);

        /* Check if we have enough space for the shortest possible answer. */
        যদি (size < STRLEN(CSI18_R0))
                প্রত্যাগম -EAGAIN;

        /* Check if the terminating sequence is present */
        যদি (buf[size - 1] != 't')
                প্রত্যাগম -EAGAIN;

        unsigned short rows, columns;
        যদি (sscanf(buf, CSI18_Rp "%hu;%hut", &rows, &columns) != 2)
                প্রত্যাগম -EINVAL;

        *ret_rows = rows;
        *ret_columns = columns;
        প্রত্যাগম 0;
}

/* Determine terminal dimensions by means of an ANSI CSI 18 sequence.
 *
 * Caller must have already opened a non-blocking input fd and configured termios (echo/icanon off). */
স্থির পূর্ণাঙ্ক terminal_query_size_by_csi18(
                পূর্ণাঙ্ক nonblock_প্রবেশ_fd,
                পূর্ণাঙ্ক output_fd,
                unsigned *ret_rows,
                unsigned *ret_columns) {

        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(nonblock_প্রবেশ_fd >= 0);
        প্রতিজ্ঞা(output_fd >= 0);

        r = loop_write_full(output_fd, CSI18_Q, SIZE_MAX, CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        যদি (r < 0)
                প্রত্যাগম r;

        usec_t end = usec_add(now(CLOCK_MONOTONIC), CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        বর্ণ buf[STRLEN(CSI18_R1)];
        size_t bytes = 0;

        প্রতি (;;) {
                usec_t n = now(CLOCK_MONOTONIC);
                যদি (n >= end)
                        প্রত্যাগম -EOPNOTSUPP;

                r = fd_wait_for_event(nonblock_প্রবেশ_fd, POLLIN, usec_sub_unsigned(end, n));
                যদি (r < 0)
                        প্রত্যাগম r;
                যদি (r == 0)
                        প্রত্যাগম -EOPNOTSUPP;

                /* On the first read, read multiple characters, i.e. the shortest valid reply. Afterwards
                 * read byte by byte, since we don't want to read too much and drop characters from the input
                 * queue. */
                ssize_t l = পঠ(nonblock_প্রবেশ_fd, buf + bytes, bytes == 0 ? STRLEN(CSI18_R0) : 1);
                যদি (l < 0) {
                        যদি (errno == EAGAIN)
                                অনুবর্ত;
                        প্রত্যাগম -errno;
                }

                প্রতিজ্ঞা((size_t) l <= পরিমাণ(buf) - bytes);
                bytes += l;

                r = scan_text_area_size_response(buf, bytes, ret_rows, ret_columns);
                যদি (r != -EAGAIN)
                        প্রত্যাগম r;

                যদি (bytes == পরিমাণ(buf))
                        প্রত্যাগম -EOPNOTSUPP; /* The response has the right prefix, but we didn't find a valid
                                             * answer with a terminator in the allotted space. Something is
                                             * wrong, possibly some unrelated bytes got injected into the
                                             * answer. */
        }
}

পূর্ণাঙ্ক terminal_get_size(
                পূর্ণাঙ্ক প্রবেশ_fd,
                পূর্ণাঙ্ক output_fd,
                unsigned *ret_rows,
                unsigned *ret_columns,
                bool try_dsr,
                bool try_csi18) {

        _cleanup_close_ পূর্ণাঙ্ক nonblock_প্রবেশ_fd = -EBADF;
        সংরচনা termios old_termios = TERMIOS_NULL;
        CLEANUP_TERMIOS_RESET(nonblock_প্রবেশ_fd, old_termios);
        _cleanup_(nonblock_resetp) পূর্ণাঙ্ক nonblock_reset = -EBADF;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(try_dsr || try_csi18);

        r = terminal_prepare_query(প্রবেশ_fd, output_fd, &nonblock_প্রবেশ_fd, &old_termios);
        যদি (r < 0)
                প্রত্যাগম r;

        /* Put the output fd in non-blocking mode with a write timeout, to avoid blocking indefinitely on
         * write if the terminal is not consuming data (e.g. serial console with flow control). */
        r = fd_nonblock(output_fd, true);
        যদি (r < 0)
                প্রত্যাগম log_debug_errno(r, "Failed to set terminal to non-blocking mode: %m");
        যদি (r > 0)
                nonblock_reset = output_fd;

        /* Flush any stale input that might confuse the response parsers. */
        (শূন্য) tcflush(nonblock_প্রবেশ_fd, TCIFLUSH);

        যদি (try_csi18) {
                r = terminal_query_size_by_csi18(nonblock_প্রবেশ_fd, output_fd, ret_rows, ret_columns);
                যদি (r >= 0)
                        প্রত্যাগম r;

                /* Query failed. Flush any outstanding input. */
                (শূন্য) tcflush(nonblock_প্রবেশ_fd, TCIFLUSH);

                যদি (!IN_SET(r, -EOPNOTSUPP, -EINVAL))
                        প্রত্যাগম r;
        }

        যদি (try_dsr) {
                r = terminal_query_size_by_dsr(nonblock_প্রবেশ_fd, output_fd, ret_rows, ret_columns);
                যদি (r >= 0)
                        প্রত্যাগম r;

                /* Query failed. Flush any outstanding input. */
                (শূন্য) tcflush(nonblock_প্রবেশ_fd, TCIFLUSH);
        }

        প্রত্যাগম r;
}

স্থির পূর্ণাঙ্ক fd_is_vc(পূর্ণাঙ্ক fd) {
        _cleanup_free_ বর্ণ *name = রিক্ত;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(fd >= 0);

        r = getttyname_harder(fd, &name);
        যদি (r < 0)
                প্রত্যাগম r;

        প্রত্যাগম tty_is_vc_resolve(name);
}

পূর্ণাঙ্ক terminal_fix_size(পূর্ণাঙ্ক প্রবেশ_fd, পূর্ণাঙ্ক output_fd) {
        unsigned rows, columns;
        পূর্ণাঙ্ক r;

        /* Don't ask a virtual console at all. There is no intermediary here that could be wrong about the
         * dimensions: the kernel renders the screen itself, so its winsize is authoritative. */
        r = fd_is_vc(প্রবেশ_fd);
        যদি (r < 0)
                log_debug_errno(r, "Failed to determine whether terminal is a virtual console, assuming it is not: %m");
        অন্যথা যদি (r > 0) {
                log_debug("Not querying dimensions of a virtual console, kernel dimensions are authoritative.");
                প্রত্যাগম 0;
        }

        সংরচনা winsize ws = {};
        যদি (ioctl(output_fd, TIOCGWINSZ, &ws) < 0)
                প্রত্যাগম log_debug_errno(errno, "Failed to query terminal dimensions, ignoring: %m");

        r = terminal_get_size(প্রবেশ_fd, output_fd, &rows, &columns, /* try_dsr= */ true, /* try_csi18= */ true);
        যদি (r < 0)
                প্রত্যাগম log_debug_errno(r, "Failed to acquire terminal dimensions via ANSI sequences, not adjusting terminal dimensions: %m");

        যদি (ws.ws_row == rows && ws.ws_col == columns) {
                log_debug("Terminal dimensions reported via ANSI sequences match currently set terminal dimensions, not changing.");
                প্রত্যাগম 0;
        }

        ws.ws_col = columns;
        ws.ws_row = rows;

        যদি (ioctl(output_fd, TIOCSWINSZ, &ws) < 0)
                প্রত্যাগম log_debug_errno(errno, "Failed to update terminal dimensions, ignoring: %m");

        log_debug("Fixed terminal dimensions to %ux%u based on ANSI sequence information.", columns, rows);
        প্রত্যাগম 1;
}

#define MAX_TERMINFO_LENGTH 64
/* python -c 'print("".join(hex(ord(i))[2:] for i in "name").upper())' */
#define DCS_TERMINFO_Q ANSI_DCS "+q" "6E616D65" ANSI_ST
/* The answer is either 0+r… (invalid) or 1+r… (OK). */
#define DCS_TERMINFO_R0 ANSI_DCS "0+r" ANSI_ST
#define DCS_TERMINFO_R1 ANSI_DCS "1+r" "6E616D65" "=" /* This is followed by Pt ST. */
assert_cc(STRLEN(DCS_TERMINFO_R0) <= STRLEN(DCS_TERMINFO_R1 ANSI_ST));

স্থির পূর্ণাঙ্ক scan_terminfo_response(
                ধ্রুব বর্ণ *buf,
                size_t size,
                বর্ণ **ret_name) {
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(buf);
        প্রতিজ্ঞা(ret_name);

        /* Check if we have enough space for the shortest possible answer. */
        যদি (size < STRLEN(DCS_TERMINFO_R0))
                প্রত্যাগম -EAGAIN;

        /* Check if the terminating sequence is present */
        যদি (memcmp(buf + size - STRLEN(ANSI_ST), ANSI_ST, STRLEN(ANSI_ST)) != 0)
                প্রত্যাগম -EAGAIN;

        যদি (size <= STRLEN(DCS_TERMINFO_R1 ANSI_ST))
                প্রত্যাগম -EINVAL;  /* The answer is invalid or empty */

        যদি (memcmp(buf, DCS_TERMINFO_R1, STRLEN(DCS_TERMINFO_R1)) != 0)
                প্রত্যাগম -EINVAL;  /* The answer is not valid */

        _cleanup_free_ শূন্য *dec = রিক্ত;
        size_t dec_size;
        r = unhexmem_full(buf + STRLEN(DCS_TERMINFO_R1), size - STRLEN(DCS_TERMINFO_R1 ANSI_ST),
                          /* secure= */ false,
                          &dec, &dec_size);
        যদি (r < 0)
                প্রত্যাগম r;

        প্রতিজ্ঞা(((ধ্রুব বর্ণ *) dec)[dec_size] == '\0'); /* unhexmem appends NUL for our convenience */
        যদি (memchr(dec, '\0', dec_size) || string_has_cc(dec, রিক্ত) || !filename_is_valid(dec))
                প্রত্যাগম -EUCLEAN;

        *ret_name = TAKE_PTR(dec);
        প্রত্যাগম 0;
}

পূর্ণাঙ্ক terminal_get_terminfo_by_dcs(পূর্ণাঙ্ক fd, বর্ণ **ret_name) {
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(fd >= 0);
        প্রতিজ্ঞা(ret_name);

        /* Note: fd must be in non-blocking read-write mode! */

        সংরচনা termios old_termios = TERMIOS_NULL;
        CLEANUP_TERMIOS_RESET(fd, old_termios);

        যদি (tcgetattr(fd, &old_termios) < 0)
                প্রত্যাগম -errno;

        সংরচনা termios new_termios = old_termios;
        termios_disable_echo(&new_termios);

        যদি (tcsetattr(fd, TCSANOW, &new_termios) < 0)
                প্রত্যাগম -errno;

        r = loop_write(fd, DCS_TERMINFO_Q, SIZE_MAX);
        যদি (r < 0)
                প্রত্যাগম r;

        usec_t end = usec_add(now(CLOCK_MONOTONIC), CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        বর্ণ buf[STRLEN(DCS_TERMINFO_R1) + MAX_TERMINFO_LENGTH + STRLEN(ANSI_ST)];
        size_t bytes = 0;

        প্রতি (;;) {
                usec_t n = now(CLOCK_MONOTONIC);
                যদি (n >= end)
                        প্রত্যাগম -EOPNOTSUPP;

                r = fd_wait_for_event(fd, POLLIN, usec_sub_unsigned(end, n));
                যদি (r < 0)
                        প্রত্যাগম r;
                যদি (r == 0)
                        প্রত্যাগম -EOPNOTSUPP;

                /* On the first read, read multiple characters, i.e. the shortest valid reply. Afterwards
                 * read byte by byte, since we don't want to read too much and drop characters from the input
                 * queue. */
                ssize_t l = পঠ(fd, buf + bytes, bytes == 0 ? STRLEN(DCS_TERMINFO_R0) : 1);
                যদি (l < 0) {
                        যদি (errno == EAGAIN)
                                অনুবর্ত;
                        প্রত্যাগম -errno;
                }

                প্রতিজ্ঞা((size_t) l <= পরিমাণ(buf) - bytes);
                bytes += l;

                r = scan_terminfo_response(buf, bytes, ret_name);
                যদি (r != -EAGAIN)
                        প্রত্যাগম r;

                যদি (bytes == পরিমাণ(buf))
                        প্রত্যাগম -EOPNOTSUPP; /* The response has the right prefix, but we didn't find a valid
                                             * answer with a terminator in the allotted space. Something is
                                             * wrong, possibly some unrelated bytes got injected into the
                                             * answer. */
        }
}

পূর্ণাঙ্ক have_terminfo_file(ধ্রুব বর্ণ *name) {
        /* This is a heuristic check if we have the file, using the directory layout used on
         * current Linux systems. Checks for other layouts can be added later if appropriate. */
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(filename_is_valid(name));

        _cleanup_free_ বর্ণ *p = path_join("/usr/share/terminfo", CHAR_TO_STR(name[0]), name);
        যদি (!p)
                প্রত্যাগম log_oom_debug();

        r = RET_NERRNO(access(p, F_OK));
        যদি (r == -ENOENT)
                প্রত্যাগম false;
        যদি (r < 0)
                প্রত্যাগম r;
        প্রত্যাগম true;
}

পূর্ণাঙ্ক query_term_for_tty(ধ্রুব বর্ণ *tty, বর্ণ **ret_term) {
        _cleanup_free_ বর্ণ *dcs_term = রিক্ত;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(tty);
        প্রতিজ্ঞা(ret_term);

        যদি (tty_is_vc_resolve(tty))
                প্রত্যাগম strdup_to(ret_term, "linux");

        /* Try to query the terminal implementation that we're on. This will not work in all
         * cases, which is fine, since this is intended to be used as a fallback. */

        _cleanup_close_ পূর্ণাঙ্ক tty_fd = open_terminal(tty, O_RDWR|O_NOCTTY|O_CLOEXEC|O_NONBLOCK);
        যদি (tty_fd < 0)
                প্রত্যাগম log_debug_errno(tty_fd, "Failed to open %s to query terminfo: %m", tty);

        r = terminal_get_terminfo_by_dcs(tty_fd, &dcs_term);
        যদি (r < 0)
                প্রত্যাগম log_debug_errno(r, "Failed to query %s for terminfo: %m", tty);

        r = have_terminfo_file(dcs_term);
        যদি (r < 0)
                প্রত্যাগম log_debug_errno(r, "Failed to look for terminfo %s: %m", dcs_term);
        যদি (r == 0)
                প্রত্যাগম log_info_errno(SYNTHETIC_ERRNO(ENODATA),
                                      "Terminfo %s not found for %s.", dcs_term, tty);

        *ret_term = TAKE_PTR(dcs_term);
        প্রত্যাগম 0;
}

পূর্ণাঙ্ক terminal_is_pty_fd(পূর্ণাঙ্ক fd) {
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(fd >= 0);

        /* Returns true if we are looking at a pty, i.e. if it's backed by the /dev/pts/ file system */

        যদি (!isatty_safe(fd))
                প্রত্যাগম false;

        r = is_fs_type_at(fd, রিক্ত, DEVPTS_SUPER_MAGIC);
        যদি (r != 0)
                প্রত্যাগম r;

        /* The ptmx device is weird, it exists twice, once inside and once outside devpts. To detect the
         * latter case, let's fire off an ioctl() that only works on ptmx devices. */

        পূর্ণাঙ্ক v;
        যদি (ioctl(fd, TIOCGPKT, &v) < 0) {
                যদি (ERRNO_IS_NOT_SUPPORTED(errno))
                        প্রত্যাগম false;

                প্রত্যাগম -errno;
        }

        প্রত্যাগম true;
}

পূর্ণাঙ্ক pty_open_peer(পূর্ণাঙ্ক fd, পূর্ণাঙ্ক mode) {
        প্রতিজ্ঞা(fd >= 0);

        /* Opens the peer PTY using the new race-free TIOCGPTPEER ioctl() (kernel 4.13).
         *
         * This is safe to be called on TTYs from other namespaces. */

        প্রতিজ্ঞা((mode & (O_CREAT|O_PATH|O_DIRECTORY|O_TMPFILE)) == 0);

        /* This replicates the EIO retry logic of open_terminal() in a modified way. */
        প্রতি (unsigned c = 0;; c++) {
                পূর্ণাঙ্ক peer_fd = ioctl(fd, TIOCGPTPEER, mode);
                যদি (peer_fd >= 0)
                        প্রত্যাগম peer_fd;

                যদি (errno != EIO)
                        প্রত্যাগম -errno;

                /* Max 1s in total */
                যদি (c >= 20)
                        প্রত্যাগম -EIO;

                (শূন্য) usleep_safe(50 * USEC_PER_MSEC);
        }
}
