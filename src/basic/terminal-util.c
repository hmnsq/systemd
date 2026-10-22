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

স্থির অস্থির unsigned cached_columns = 0;
স্থির অস্থির unsigned cached_সারিগুলি = 0;

স্থির অস্থির পূর্ণসংখ্যা cached_on_tty = -1;
স্থির অস্থির পূর্ণসংখ্যা cached_on_dev_null = -1;

bool isatty_safe(পূর্ণসংখ্যা fd) {
        প্রতিজ্ঞা(fd >= 0);

        যদি (isatty(fd))
                ফেরত true;

        /* Linux/glibc returns EIO for hung up TTY on isatty(). Which is wrong, the thing doesn't stop being
         * a TTY after all, just because it is temporarily hung up. Let's work around this here, until this
         * is fixed in glibc. See: https://sourceware.org/bugzilla/show_bug.cgi?id=32103 */
        যদি (errno == EIO)
                ফেরত true;

        /* Be resilient if we're working on stdio, since they're set up by parent process. */
        প্রতিজ্ঞা(errno != EBADF || IN_SET(fd, STDIN_FILENO, STDOUT_FILENO, STDERR_FILENO));

        ফেরত false;
}

পূর্ণসংখ্যা chvt(পূর্ণসংখ্যা vt) {
        _cleanup_close_ পূর্ণসংখ্যা fd = -EBADF;

        /* Switch to the specified vt number. If the VT is specified <= 0 switch to the VT the kernel log messages go,
         * if that's configured. */

        fd = open_terminal("/dev/tty0", O_RDWR|O_NOCTTY|O_CLOEXEC|O_NONBLOCK);
        যদি (fd < 0)
                ফেরত fd;

        যদি (vt <= 0) {
                পূর্ণসংখ্যা tiocl[2] = {
                        TIOCL_GETKMSGREDIRECT,
                        0
                };

                যদি (ioctl(fd, TIOCLINUX, tiocl) < 0)
                        ফেরত -errno;

                vt = tiocl[0] <= 0 ? 1 : tiocl[0];
        }

        ফেরত RET_NERRNO(ioctl(fd, VT_ACTIVATE, vt));
}

পূর্ণসংখ্যা read_one_char(FILE *f, অক্ষর *ret, usec_t t, bool echo, bool *need_nl) {
        _cleanup_free_ অক্ষর *সারি = ফাঁকা;
        কাঠামো termios old_termios;
        পূর্ণসংখ্যা r, fd;

        প্রতিজ্ঞা(ret);

        যদি (!f)
                f = stdin;

        /* If this is a terminal, then switch canonical mode off, so that we can read a single
         * character. (Note that fmemopen() streams do not have an fd associated with them, let's handle that
         * nicely.) If 'echo' is false we'll also disable ECHO mode so that the pressed key is not made
         * visible to the user. */
        fd = fileno(f);
        যদি (fd >= 0 && tcgetattr(fd, &old_termios) >= 0) {
                কাঠামো termios new_termios = old_termios;

                new_termios.c_lflag &= ~(ICANON|(echo ? 0 : ECHO));
                new_termios.c_cc[VMIN] = 1;
                new_termios.c_cc[VTIME] = 0;

                যদি (tcsetattr(fd, TCSANOW, &new_termios) >= 0) {
                        অক্ষর c;

                        যদি (t != USEC_INFINITY) {
                                যদি (fd_wait_for_event(fd, POLLIN, t) <= 0) {
                                        (শূন্য) tcsetattr(fd, TCSANOW, &old_termios);
                                        ফেরত -ETIMEDOUT;
                                }
                        }

                        r = safe_fgetc(f, &c);
                        (শূন্য) tcsetattr(fd, TCSANOW, &old_termios);
                        যদি (r < 0)
                                ফেরত r;
                        যদি (r == 0)
                                ফেরত -EIO;

                        যদি (need_nl)
                                *need_nl = c != '\n';

                        *ret = c;
                        ফেরত 0;
                }
        }

        যদি (t != USEC_INFINITY && fd >= 0) {
                /* Let's wait the specified amount of time for input. When we have no fd we skip this, under
                 * the assumption that this is an fmemopen() stream or so where waiting doesn't make sense
                 * anyway, as the data is either already in the stream or cannot possible be placed there
                 * while we access the stream */

                যদি (fd_wait_for_event(fd, POLLIN, t) <= 0)
                        ফেরত -ETIMEDOUT;
        }

        /* If this is not a terminal, then read a full line instead */

        r = read_সারি(f, 16, &সারি); /* longer than necessary, to eat up UTF-8 chars/vt100 key sequences */
        যদি (r < 0)
                ফেরত r;
        যদি (r == 0)
                ফেরত -EIO;

        যদি (strlen(সারি) != 1)
                ফেরত -EBADMSG;

        যদি (need_nl)
                *need_nl = false;

        *ret = সারি[0];
        ফেরত 0;
}

#define DEFAULT_ASK_REFRESH_USEC (2*USEC_PER_SEC)

পূর্ণসংখ্যা ask_char(অক্ষর *ret, ধ্রুবক অক্ষর *replies, ধ্রুবক অক্ষর *fmt, ...) {
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(ret);
        প্রতিজ্ঞা(replies);
        প্রতিজ্ঞা(fmt);

        জন্য (;;) {
                va_list ap;
                অক্ষর c;
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
                                চালু;

                        যদি (r == -EBADMSG) {
                                puts("Bad input, please try again.");
                                চালু;
                        }

                        putchar('\n');
                        ফেরত r;
                }

                যদি (need_nl)
                        putchar('\n');

                যদি (strchr(replies, c)) {
                        *ret = c;
                        ফেরত 0;
                }

                puts("Read unexpected character, please try again.");
        }
}

প্রকারলেখ গণনা Completion_ফলাফল{
        COMPLETION_ALREADY,       /* the input string is already complete */
        COMPLETION_FULL,          /* completed the input string to be complete now */
        COMPLETION_PARTIAL,       /* completed the input string so that is still incomplete */
        COMPLETION_NONE,          /* found no matching completion */
        COMPLETION_ফলাফল_MAX,
        COMPLETION_ফলাফল_INVALID = -EINVAL,
        COMPLETION_ফলাফল_ERRNO_MAX = -ERRNO_MAX,
} Completion_ফলাফল;

স্থির Completion_ফলাফল pick_completion(ধ্রুবক অক্ষর *string, অক্ষর *ধ্রুবক*completions, অক্ষর **ret) {
        _cleanup_free_ অক্ষর *found = ফাঁকা;
        bool partial = false;

        প্রতিজ্ঞা(ret);

        string = strempty(string);

        STRV_FOREACH(c, completions) {

                /* Ignore entries that are not actually completions */
                যদি (!startswith(*c, string))
                        চালু;

                /* Store first completion that matches */
                যদি (!found) {
                        found = strdup(*c);
                        যদি (!found)
                                ফেরত -ENOMEM;

                        চালু;
                }

                /* If there's another completion that works truncate the one we already found by common
                 * prefix */
                size_t n = str_common_prefix(found, *c);
                যদি (n == SIZE_MAX)
                        চালু;

                found[n] = 0;
                partial = true;
        }

        *ret = TAKE_PTR(found);

        যদি (!*ret)
                ফেরত COMPLETION_NONE;
        যদি (partial)
                ফেরত COMPLETION_PARTIAL;

        ফেরত streq(string, *ret) ? COMPLETION_ALREADY : COMPLETION_FULL;
}

স্থির শূন্য clear_by_backspace(size_t n) {
        /* Erase the specified number of character cells backwards on the terminal */
        জন্য (size_t i = 0; i < n; i++)
                fputs("\b \b", stdout);
}

পূর্ণসংখ্যা ask_string_full(
                অক্ষর **ret,
                ধ্রুবক অক্ষর *prefill,
                GetCompletionsCallback get_completions,
                শূন্য *userdata,
                ধ্রুবক অক্ষর *text, ...) {

        va_list ap;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(ret);
        প্রতিজ্ঞা(text);

        _cleanup_free_ অক্ষর *string = ফাঁকা;
        size_t n = 0;

        যদি (prefill) {
                /* Prefill query with explicit data if specified */

                string = strdup(prefill);
                যদি (!string)
                        ফেরত -ENOMEM;

                n = strlen(string);

        } অন্যথা যদি (get_completions) {
                /* Otherwise, figure out what string to preselect the query with */
                _cleanup_strv_free_ অক্ষর **completions = ফাঁকা;
                r = get_completions("", GET_COMPLETIONS_PRESELECT, &completions, userdata);
                যদি (r < 0)
                        ফেরত r;

                Completion_ফলাফল cr = pick_completion(string, completions, &string);
                যদি (cr < 0)
                        ফেরত cr;

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
        পূর্ণসংখ্যা fd_প্রবেশ = fileno(stdin);
        পূর্ণসংখ্যা fd_output = fileno(stdout);
        কাঠামো termios old_termios = TERMIOS_NULL;
        CLEANUP_TERMIOS_RESET(fd_প্রবেশ, old_termios);

        যদি (fd_প্রবেশ < 0 || fd_output < 0 || same_fd(fd_প্রবেশ, fd_output) <= 0)
                যাও fallback;

        /* Try to disable echo, which also tells us if this even is a terminal */
        যদি (tcgetattr(fd_প্রবেশ, &old_termios) < 0) {
                old_termios = TERMIOS_NULL;
                যাও fallback;
        }

        কাঠামো termios new_termios = old_termios;
        termios_disable_echo(&new_termios);
        যদি (tcsetattr(fd_প্রবেশ, TCSANOW, &new_termios) < 0)
                ফেরত -errno;

        জন্য (;;) {
                পূর্ণসংখ্যা c = fgetc(stdin);

                /* On EOF or NUL, end the request, don't output anything anymore */
                যদি (IN_SET(c, EOF, 0))
                        বিরতি;

                /* On Return also end the request, but make this visible */
                যদি (IN_SET(c, '\n', '\r')) {
                        fputc('\n', stdout);
                        বিরতি;
                }

                যদি (c == '\t') {
                        /* Tab */

                        _cleanup_strv_free_ অক্ষর **completions = ফাঁকা;
                        যদি (get_completions) {
                                r = get_completions(string, /* flags= */ 0, &completions, userdata);
                                যদি (r < 0)
                                        ফেরত r;
                        }

                        _cleanup_free_ অক্ষর *new_string = ফাঁকা;
                        Completion_ফলাফল cr = pick_completion(string, completions, &new_string);
                        যদি (cr < 0)
                                ফেরত cr;
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

                                _cleanup_strv_free_ অক্ষর **filtered = strv_filter_prefix(completions, string);
                                যদি (!filtered)
                                        ফেরত -ENOMEM;

                                r = show_menu(filtered,
                                              /* n_columns= */ SIZE_MAX,
                                              /* column_width= */ SIZE_MAX,
                                              /* ellipsize_percentage= */ 0,
                                              /* grey_prefix= */ string,
                                              /* with_numbers= */ false);
                                যদি (r < 0)
                                        ফেরত r;

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
                                size_t m = utf8_last_দৈর্ঘ্য(string, n);

                                অক্ষর *e = string + n - m;
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

                        ফেরত -ECANCELED;

                } অন্যথা যদি (char_is_cc(c) || n >= LINE_MAX)
                        /* refuse control characters and too long strings */
                        fputc('\a', stdout); /* BEL */
                অন্যথা {
                        /* Regular char */

                        যদি (!GREEDY_REALLOC(string, n+2))
                                ফেরত -ENOMEM;

                        string[n++] = (অক্ষর) c;
                        string[n] = 0;

                        fputc(c, stdout);
                }

                fflush(stdout);
        }

        যদি (!string) {
                string = strdup("");
                যদি (!string)
                        ফেরত -ENOMEM;
        }

        *ret = TAKE_PTR(string);
        ফেরত 0;

fallback:
        /* A simple fallback without TTY magic */
        string = mfree(string);
        r = read_সারি(stdin, LONG_সারি_MAX, &string);
        যদি (r < 0)
                ফেরত r;
        যদি (r == 0)
                ফেরত -EIO;

        *ret = TAKE_PTR(string);
        ফেরত 0;
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

        অক্ষর চাবি = 0;
        (শূন্য) read_one_char(stdin, &চাবি, USEC_INFINITY, /* echo= */ false, /* need_nl= */ ফাঁকা);

        fputc('\n', stdout);
        fflush(stdout);

        ফেরত চাবি != 'q';
}

স্থির size_t widest_list_element(অক্ষর *ধ্রুবক*l) {
        size_t w = 0;

        /* Returns the largest console width of all elements in 'l' */

        STRV_FOREACH(i, l)
                w = MAX(w, utf8_console_width(*i));

        ফেরত w;
}

পূর্ণসংখ্যা show_menu(অক্ষর **x,
              size_t n_columns,
              size_t column_width,
              unsigned ellipsize_percentage,
              ধ্রুবক অক্ষর *grey_prefix,
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

        size_t n = strv_দৈর্ঘ্য(x);
        size_t per_column = DIV_ROUND_UP(n, n_columns);

        size_t break_সারিগুলি = সারিগুলি();
        যদি (break_সারিগুলি > 2)
                break_সারিগুলি--;

        /* The first page gets two extra lines, since we want to show a title */
        size_t break_modulo = break_সারিগুলি;
        যদি (break_modulo > 3)
                break_modulo -= 3;

        জন্য (size_t i = 0; i < per_column; i++) {

                জন্য (size_t j = 0; j < n_columns; j++) {
                        _cleanup_free_ অক্ষর *e = ফাঁকা;

                        size_t p = j * per_column + i;
                        যদি (p >= n)
                                বিরতি;

                        e = ellipsize(x[p], column_width, ellipsize_percentage);
                        যদি (!e)
                                ফেরত -ENOMEM;

                        যদি (with_numbers)
                                printf("%s%4zu)%s ",
                                       ansi_grey(),
                                       p + 1,
                                       ansi_normal());

                        যদি (grey_prefix && startswith(e, grey_prefix)) {
                                size_t k = MIN(strlen(grey_prefix), column_width);
                                printf("%s%.*s%s",
                                       ansi_grey(),
                                       (পূর্ণসংখ্যা) k, e,
                                       ansi_normal());
                                printf("%-*s",
                                       (পূর্ণসংখ্যা) (column_width - k), e+k);
                        } অন্যথা
                                printf("%-*s", (পূর্ণসংখ্যা) column_width, e);
                }

                putchar('\n');

                /* on the first screen we reserve 2 extra lines for the title */
                যদি (i % break_সারিগুলি == break_modulo)
                        যদি (!any_চাবি_to_proceed())
                                ফেরত 0;
        }

        ফেরত 0;
}

পূর্ণসংখ্যা open_terminal(ধ্রুবক অক্ষর *name, পূর্ণসংখ্যা mode) {
        _cleanup_close_ পূর্ণসংখ্যা fd = -EBADF;

        /*
         * If a TTY is in the process of being closed opening it might cause EIO. This is horribly awful, but
         * unlikely to be changed in the kernel. Hence we work around this problem by retrying a couple of
         * times.
         *
         * https://bugs.launchpad.net/ubuntu/+source/linux/+bug/554172/comments/245
         */

        প্রতিজ্ঞা((mode & (O_CREAT|O_PATH|O_DIRECTORY|O_TMPFILE)) == 0);

        জন্য (unsigned c = 0;; c++) {
                fd = open(name, mode, 0);
                যদি (fd >= 0)
                        বিরতি;

                যদি (errno != EIO)
                        ফেরত -errno;

                /* Max 1s in total */
                যদি (c >= 20)
                        ফেরত -EIO;

                (শূন্য) usleep_safe(50 * USEC_PER_MSEC);
        }

        যদি (!isatty_safe(fd))
                ফেরত -ENOTTY;

        ফেরত TAKE_FD(fd);
}

পূর্ণসংখ্যা acquire_terminal(
                ধ্রুবক অক্ষর *name,
                AcquireTerminalFlags flags,
                usec_t timeout) {

        _cleanup_close_ পূর্ণসংখ্যা notify = -EBADF, fd = -EBADF;
        usec_t ts = USEC_INFINITY;
        পূর্ণসংখ্যা r, wd = -1;

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
                        ফেরত -errno;

                wd = inotify_add_watch(notify, name, IN_CLOSE);
                যদি (wd < 0)
                        ফেরত -errno;

                যদি (timeout != USEC_INFINITY)
                        ts = now(CLOCK_MONOTONIC);
        }

        /* If we are called with ACQUIRE_TERMINAL_WATCH_SIGTERM we'll unblock SIGTERM during ppoll() temporarily */
        sigset_t poll_ss;
        assert_se(sigprocmask(SIG_SETMASK, /* newset= */ ফাঁকা, &poll_ss) >= 0);
        যদি (flags & ACQUIRE_TERMINAL_WATCH_SIGTERM) {
                assert_se(sigismember(&poll_ss, SIGTERM) > 0);
                assert_se(sigdelset(&poll_ss, SIGTERM) >= 0);
        }

        জন্য (;;) {
                যদি (notify >= 0) {
                        r = flush_fd(notify);
                        যদি (r < 0)
                                ফেরত r;
                }

                /* We pass here O_NOCTTY only so that we can check the return value TIOCSCTTY and have a reliable way
                 * to figure out if we successfully became the controlling process of the tty */
                fd = open_terminal(name, O_RDWR|O_NOCTTY|O_CLOEXEC);
                যদি (fd < 0)
                        ফেরত fd;

                /* Temporarily ignore SIGHUP, so that we don't get SIGHUP'ed if we already own the tty. */
                কাঠামো sigaction sa_old;
                assert_se(sigaction(SIGHUP, &sigaction_ignore, &sa_old) >= 0);

                /* First, try to get the tty */
                r = RET_NERRNO(ioctl(fd, TIOCSCTTY, mode == ACQUIRE_TERMINAL_FORCE));

                /* Reset signal handler to old value */
                assert_se(sigaction(SIGHUP, &sa_old, ফাঁকা) >= 0);

                /* Success? Exit the loop now! */
                যদি (r >= 0)
                        বিরতি;

                /* Any failure besides -EPERM? Fail, regardless of the mode. */
                যদি (r != -EPERM)
                        ফেরত r;

                যদি (flags & ACQUIRE_TERMINAL_PERMISSIVE) /* If we are in permissive mode, then EPERM is fine, turn this
                                                          * into a success. Note that EPERM is also returned if we
                                                          * already are the owner of the TTY. */
                        বিরতি;

                যদি (mode != ACQUIRE_TERMINAL_WAIT) /* If we are in TRY or FORCE mode, then propagate EPERM as EPERM */
                        ফেরত r;

                প্রতিজ্ঞা(notify >= 0);
                প্রতিজ্ঞা(wd >= 0);

                জন্য (;;) {
                        usec_t বাম;
                        যদি (timeout == USEC_INFINITY)
                                বাম = USEC_INFINITY;
                        অন্যথা {
                                প্রতিজ্ঞা(ts != USEC_INFINITY);

                                usec_t n = usec_sub_unsigned(now(CLOCK_MONOTONIC), ts);
                                যদি (n >= timeout)
                                        ফেরত -ETIMEDOUT;

                                বাম = timeout - n;
                        }

                        r = ppoll_usec_full(
                                        &(কাঠামো pollfd) {
                                                .fd = notify,
                                                .events = POLLIN,
                                        },
                                        /* n_fds= */ 1,
                                        বাম,
                                        &poll_ss);
                        যদি (r < 0)
                                ফেরত r;
                        যদি (r == 0)
                                ফেরত -ETIMEDOUT;

                        সংঘ inotify_event_বাফার বাফার;
                        ssize_t l;
                        l = পঠ(notify, &বাফার, আকার(বাফার));
                        যদি (l < 0) {
                                যদি (ERRNO_IS_TRANSIENT(errno))
                                        চালু;

                                ফেরত -errno;
                        }

                        FOREACH_INOTIFY_EVENT(e, বাফার, l) {
                                যদি (e->mask & IN_Q_OVERFLOW) /* If we hit an inotify queue overflow, simply check if the terminal is up for grabs now. */
                                        বিরতি;

                                যদি (e->wd != wd || !(e->mask & IN_CLOSE)) /* Safety checks */
                                        ফেরত -EIO;
                        }

                        বিরতি;
                }

                /* We close the tty fd here since if the old session ended our handle will be dead. It's important that
                 * we do this after sleeping, so that we don't enter an endless loop. */
                fd = safe_close(fd);
        }

        ফেরত TAKE_FD(fd);
}

পূর্ণসংখ্যা release_terminal(শূন্য) {
        _cleanup_close_ পূর্ণসংখ্যা fd = -EBADF;
        পূর্ণসংখ্যা r;

        fd = open("/dev/tty", O_RDWR|O_NOCTTY|O_CLOEXEC|O_NONBLOCK);
        যদি (fd < 0)
                ফেরত -errno;

        /* Temporarily ignore SIGHUP, so that we don't get SIGHUP'ed
         * by our own TIOCNOTTY */
        কাঠামো sigaction sa_old;
        assert_se(sigaction(SIGHUP, &sigaction_ignore, &sa_old) >= 0);

        r = RET_NERRNO(ioctl(fd, TIOCNOTTY));

        assert_se(sigaction(SIGHUP, &sa_old, ফাঁকা) >= 0);

        ফেরত r;
}

পূর্ণসংখ্যা terminal_new_session(শূন্য) {

        /* Make us the new session leader, and set stdin tty to be our controlling terminal.
         *
         * Why stdin? Well, the ctty logic is relevant for signal delivery mostly, i.e. if people hit C-c
         * or the line is hung up. Such events are basically just a form of input, via a side channel
         * (that side channel being signal delivery, i.e. SIGINT, SIGHUP et al). Hence we focus on input,
         * not output here. */

        যদি (!isatty_safe(STDIN_FILENO))
                ফেরত -ENXIO;

        (শূন্য) setsid();
        ফেরত RET_NERRNO(ioctl(STDIN_FILENO, TIOCSCTTY, 0));
}

শূন্য terminal_detach_session(শূন্য) {
        (শূন্য) setsid();
        (শূন্য) release_terminal();
}

পূর্ণসংখ্যা terminal_vhangup_fd(পূর্ণসংখ্যা fd) {
        প্রতিজ্ঞা(fd >= 0);
        ফেরত RET_NERRNO(ioctl(fd, TIOCVHANGUP));
}

পূর্ণসংখ্যা terminal_vhangup(ধ্রুবক অক্ষর *tty) {
        _cleanup_close_ পূর্ণসংখ্যা fd = -EBADF;

        প্রতিজ্ঞা(tty);

        fd = open_terminal(tty, O_RDWR|O_NOCTTY|O_CLOEXEC);
        যদি (fd < 0)
                ফেরত fd;

        ফেরত terminal_vhangup_fd(fd);
}

পূর্ণসংখ্যা vt_disallocate(ধ্রুবক অক্ষর *tty_path) {
        প্রতিজ্ঞা(tty_path);

        /* Deallocate the VT if possible. If not possible (i.e. because it is the active one), at least clear
         * it entirely (including the scrollback buffer). */

        পূর্ণসংখ্যা ttynr = vtnr_from_tty(tty_path);
        যদি (ttynr > 0) {
                _cleanup_close_ পূর্ণসংখ্যা fd = open_terminal("/dev/tty0", O_RDWR|O_NOCTTY|O_CLOEXEC|O_NONBLOCK);
                যদি (fd < 0)
                        ফেরত fd;

                /* Try to deallocate */
                যদি (ioctl(fd, VT_DISALLOCATE, ttynr) >= 0)
                        ফেরত 0;
                যদি (errno != EBUSY)
                        ফেরত -errno;
        }

        /* So this is not a VT (in which case we cannot deallocate it), or we failed to deallocate. Let's at
         * least clear the screen. */

        _cleanup_close_ পূর্ণসংখ্যা fd2 = open_terminal(tty_path, O_WRONLY|O_NOCTTY|O_CLOEXEC|O_NONBLOCK);
        যদি (fd2 < 0)
                ফেরত fd2;

        ফেরত loop_write_full(fd2,
                               "\033[r"   /* clear scrolling region */
                               "\033[H"   /* move home */
                               "\033[3J"  /* clear screen including scrollback, requires Linux 2.6.40 */
                               "\033c",   /* reset to initial state */
                               SIZE_MAX,
                               CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
}

স্থির পূর্ণসংখ্যা vt_default_utf8(শূন্য) {
        /* Read the default VT UTF8 setting from the kernel */
        ফেরত read_boolean_file("/sys/module/vt/parameters/default_utf8");
}

স্থির পূর্ণসংখ্যা vt_reset_keyboard(পূর্ণসংখ্যা fd) {
        পূর্ণসংখ্যা r, kb;

        প্রতিজ্ঞা(fd >= 0);

        /* If we can't read the default, then default to Unicode. It's 2024 after all. */
        r = vt_default_utf8();
        যদি (r < 0)
                log_debug_errno(r, "Failed to determine kernel VT UTF-8 mode, assuming enabled: %m");

        kb = vt_default_utf8() != 0 ? K_UNICODE : K_XLATE;
        ফেরত RET_NERRNO(ioctl(fd, KDSKBMODE, kb));
}

স্থির পূর্ণসংখ্যা terminal_reset_ioctl(পূর্ণসংখ্যা fd, bool switch_to_text) {
        কাঠামো termios termios;
        পূর্ণসংখ্যা r;

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
                যাও finish;
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

        ফেরত r;
}

পূর্ণসংখ্যা terminal_reset_ansi_seq(পূর্ণসংখ্যা fd) {
        _cleanup_(nonblock_resetp) পূর্ণসংখ্যা nonblock_reset = -EBADF;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(fd >= 0);

        যদি (getenv_terminal_is_dumb())
                ফেরত 0;

        r = fd_nonblock(fd, true);
        যদি (r < 0)
                ফেরত log_debug_errno(r, "Failed to set terminal to non-blocking mode: %m");
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

        ফেরত r;
}

শূন্য reset_dev_console_fd(পূর্ণসংখ্যা fd, bool switch_to_text) {
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(fd >= 0);

        _cleanup_close_ পূর্ণসংখ্যা lock_fd = lock_dev_console();
        যদি (lock_fd < 0)
                log_debug_errno(lock_fd, "Failed to lock /dev/console, ignoring: %m");

        r = terminal_reset_ioctl(fd, switch_to_text);
        যদি (r < 0)
                log_warning_errno(r, "Failed to reset /dev/console, ignoring: %m");

        unsigned rows, cols;
        r = proc_cmdline_tty_আকার("/dev/console", &rows, &cols);
        যদি (r < 0)
                log_warning_errno(r, "Failed to get /dev/console size, ignoring: %m");
        অন্যথা যদি (r > 0) {
                r = terminal_set_আকার_fd(fd, ফাঁকা, rows, cols);
                যদি (r < 0)
                        log_warning_errno(r, "Failed to set configured terminal size on /dev/console, ignoring: %m");
        } অন্যথা
                (শূন্য) terminal_fix_আকার(fd, fd);

        r = terminal_reset_ansi_seq(fd);
        যদি (r < 0)
                log_warning_errno(r, "Failed to reset /dev/console using ANSI sequences, ignoring: %m");
}

পূর্ণসংখ্যা lock_dev_console(শূন্য) {
        _cleanup_close_ পূর্ণসংখ্যা fd = -EBADF;
        পূর্ণসংখ্যা r;

        /* NB: We do not use O_NOFOLLOW here, because some container managers might place a symlink to some
         * pty in /dev/console, in which case it should be fine to lock the target TTY. */
        fd = open_terminal("/dev/console", O_RDONLY|O_CLOEXEC|O_NOCTTY);
        যদি (fd < 0)
                ফেরত fd;

        r = lock_generic(fd, LOCK_BSD, LOCK_EX);
        যদি (r < 0)
                ফেরত r;

        ফেরত TAKE_FD(fd);
}

পূর্ণসংখ্যা make_console_stdio(bool switch_to_text) {
        পূর্ণসংখ্যা fd, r;

        /* Make /dev/console the controlling terminal and stdin/stdout/stderr, if we can. If we can't use
         * /dev/null instead. This is particularly useful if /dev/console is turned off, e.g. if console=null
         * is specified on the kernel command line. */

        fd = acquire_terminal("/dev/console", ACQUIRE_TERMINAL_FORCE|ACQUIRE_TERMINAL_PERMISSIVE, USEC_INFINITY);
        যদি (fd < 0) {
                log_warning_errno(fd, "Failed to acquire terminal, using /dev/null stdin/stdout/stderr instead: %m");

                r = make_null_stdio();
                যদি (r < 0)
                        ফেরত log_error_errno(r, "Failed to make /dev/null stdin/stdout/stderr: %m");

        } অন্যথা {
                reset_dev_console_fd(fd, switch_to_text);

                r = rearrange_stdio(fd, fd, fd); /* This invalidates 'fd' both on success and on failure. */
                যদি (r < 0)
                        ফেরত log_error_errno(r, "Failed to make terminal stdin/stdout/stderr: %m");
        }

        reset_terminal_feature_caches();
        ফেরত 0;
}

স্থির পূর্ণসংখ্যা vtnr_from_tty_raw(ধ্রুবক অক্ষর *tty, unsigned *ret) {
        প্রতিজ্ঞা(tty);

        tty = skip_dev_prefix(tty);

        ধ্রুবক অক্ষর *e = startswith(tty, "tty");
        যদি (!e)
                ফেরত -EINVAL;

        ফেরত safe_atou(e, ret);
}

পূর্ণসংখ্যা vtnr_from_tty(ধ্রুবক অক্ষর *tty) {
        unsigned u;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(tty);

        r = vtnr_from_tty_raw(tty, &u);
        যদি (r < 0)
                ফেরত r;
        যদি (!vtnr_is_valid(u))
                ফেরত -ERANGE;

        ফেরত (পূর্ণসংখ্যা) u;
}

bool tty_is_vc(ধ্রুবক অক্ষর *tty) {
        প্রতিজ্ঞা(tty);

        /* NB: for >= 0 values no range check is conducted here, on the assumption that the caller will
         * either extract vtnr through vtnr_from_tty() later where ERANGE would be reported, or doesn't care
         * about whether it's strictly valid, but only asking "does this fall into the vt category?", for which
         * "yes" seems to be a better answer. */

        ফেরত vtnr_from_tty_raw(tty, /* ret= */ ফাঁকা) >= 0;
}

bool tty_is_console(ধ্রুবক অক্ষর *tty) {
        প্রতিজ্ঞা(tty);

        ফেরত streq(skip_dev_prefix(tty), "console");
}

পূর্ণসংখ্যা resolve_dev_console(অক্ষর **ret) {
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(ret);

        /* Resolve where /dev/console is pointing to. If /dev/console is a symlink (like in container
         * managers), we'll just resolve the symlink. If it's a real device node, we'll use if
         * /sys/class/tty/tty0/active, but only if /sys/ is actually ours (i.e. not read-only-mounted which
         * is a sign for container setups). */

        _cleanup_free_ অক্ষর *chased = ফাঁকা;
        r = chase("/dev/console", /* root= */ ফাঁকা, /* flags= */ 0, &chased, /* ret_fd= */ ফাঁকা);
        যদি (r < 0)
                ফেরত r;
        যদি (!path_equal(chased, "/dev/console")) {
                *ret = TAKE_PTR(chased);
                ফেরত 0;
        }

        r = path_is_read_only_fs("/sys");
        যদি (r < 0)
                ফেরত r;
        যদি (r > 0)
                ফেরত -ENOMEDIUM;

        _cleanup_free_ অক্ষর *active = ফাঁকা;
        r = read_one_সারি_file("/sys/class/tty/console/active", &active);
        যদি (r < 0)
                ফেরত r;
        যদি (r == 0)
                ফেরত -ENXIO;

        /* If multiple log outputs are configured the last one is what /dev/console points to */
        ধ্রুবক অক্ষর *tty = strrchr(active, ' ');
        যদি (tty)
                tty++;
        অন্যথা
                tty = active;

        যদি (streq(tty, "tty0")) {
                active = mfree(active);

                /* Get the active VC (e.g. tty1) */
                r = read_one_সারি_file("/sys/class/tty/tty0/active", &active);
                যদি (r < 0)
                        ফেরত r;
                যদি (r == 0)
                        ফেরত -ENXIO;

                tty = active;
        }

        _cleanup_free_ অক্ষর *path = ফাঁকা;
        path = path_join("/dev", tty);
        যদি (!path)
                ফেরত -ENOMEM;

        *ret = TAKE_PTR(path);
        ফেরত 0;
}

পূর্ণসংখ্যা get_kernel_consoles(অক্ষর ***ret) {
        _cleanup_strv_free_ অক্ষর **l = ফাঁকা;
        _cleanup_free_ অক্ষর *সারি = ফাঁকা;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(ret);

        /* If /sys/ is mounted read-only this means we are running in some kind of container environment.
         * In that case /sys/ would reflect the host system, not us, hence ignore the data we can read from it. */
        যদি (path_is_read_only_fs("/sys") > 0)
                যাও fallback;

        r = read_one_সারি_file("/sys/class/tty/console/active", &সারি);
        যদি (r < 0)
                ফেরত r;

        জন্য (ধ্রুবক অক্ষর *p = সারি;;) {
                _cleanup_free_ অক্ষর *tty = ফাঁকা, *path = ফাঁকা;

                r = extract_first_word(&p, &tty, ফাঁকা, 0);
                যদি (r < 0)
                        ফেরত r;
                যদি (r == 0)
                        বিরতি;

                যদি (streq(tty, "tty0")) {
                        tty = mfree(tty);
                        r = read_one_সারি_file("/sys/class/tty/tty0/active", &tty);
                        যদি (r < 0)
                                ফেরত r;
                        যদি (r == 0) {
                                log_debug("No VT active, skipping /dev/tty0.");
                                চালু;
                        }
                }

                path = path_join("/dev", tty);
                যদি (!path)
                        ফেরত -ENOMEM;

                যদি (access(path, F_OK) < 0) {
                        log_debug_errno(errno, "Console device %s is not accessible, skipping: %m", path);
                        চালু;
                }

                r = strv_consume(&l, TAKE_PTR(path));
                যদি (r < 0)
                        ফেরত r;
        }

        যদি (strv_isempty(l)) {
                log_debug("No devices found for system console");
                যাও fallback;
        }

        *ret = TAKE_PTR(l);
        ফেরত strv_দৈর্ঘ্য(*ret);

fallback:
        r = strv_extend(&l, "/dev/console");
        যদি (r < 0)
                ফেরত r;

        *ret = TAKE_PTR(l);
        ফেরত 0;
}

bool tty_is_vc_resolve(ধ্রুবক অক্ষর *tty) {
        _cleanup_free_ অক্ষর *resolved = ফাঁকা;

        প্রতিজ্ঞা(tty);

        যদি (streq(skip_dev_prefix(tty), "console")) {
                যদি (resolve_dev_console(&resolved) < 0)
                        ফেরত false;

                tty = resolved;
        }

        ফেরত tty_is_vc(tty);
}

পূর্ণসংখ্যা fd_columns(পূর্ণসংখ্যা fd) {
        কাঠামো winsize ws = {};

        যদি (fd < 0)
                ফেরত -EBADF;

        যদি (ioctl(fd, TIOCGWINSZ, &ws) < 0)
                ফেরত -errno;

        যদি (ws.ws_col <= 0)
                ফেরত -ENODATA; /* some tty types come up with invalid row/column initially, return a recognizable error for that */

        ফেরত ws.ws_col;
}

পূর্ণসংখ্যা getenv_columns(শূন্য) {
        পূর্ণসংখ্যা r;

        ধ্রুবক অক্ষর *e = getenv("COLUMNS");
        যদি (!e)
                ফেরত -ENXIO;

        unsigned c;
        r = safe_atou_bounded(e, 1, USHRT_MAX, &c);
        যদি (r < 0)
                ফেরত r;

        ফেরত (পূর্ণসংখ্যা) c;
}

unsigned columns(শূন্য) {

        যদি (cached_columns > 0)
                ফেরত cached_columns;

        পূর্ণসংখ্যা c = getenv_columns();
        যদি (c < 0) {
                c = fd_columns(STDOUT_FILENO);
                যদি (c < 0)
                        c = 80;
        }

        প্রতিজ্ঞা(c > 0);

        cached_columns = c;
        ফেরত cached_columns;
}

পূর্ণসংখ্যা fd_সারিগুলি(পূর্ণসংখ্যা fd) {
        কাঠামো winsize ws = {};

        যদি (fd < 0)
                ফেরত -EBADF;

        যদি (ioctl(fd, TIOCGWINSZ, &ws) < 0)
                ফেরত -errno;

        যদি (ws.ws_row <= 0)
                ফেরত -ENODATA; /* some tty types come up with invalid row/column initially, return a recognizable error for that */

        ফেরত ws.ws_row;
}

unsigned সারিগুলি(শূন্য) {
        ধ্রুবক অক্ষর *e;
        পূর্ণসংখ্যা l;

        যদি (cached_সারিগুলি > 0)
                ফেরত cached_সারিগুলি;

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
        ফেরত cached_সারিগুলি;
}

পূর্ণসংখ্যা terminal_set_আকার_fd(পূর্ণসংখ্যা fd, ধ্রুবক অক্ষর *ident, unsigned rows, unsigned cols) {
        কাঠামো winsize ws;

        প্রতিজ্ঞা(fd >= 0);

        যদি (!ident)
                ident = "TTY";

        যদি (rows == UINT_MAX && cols == UINT_MAX)
                ফেরত 0;

        যদি (ioctl(fd, TIOCGWINSZ, &ws) < 0)
                ফেরত log_debug_errno(errno,
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
                ফেরত 0;

        ws.ws_row = rows;
        ws.ws_col = cols;

        যদি (ioctl(fd, TIOCSWINSZ, &ws) < 0)
                ফেরত log_debug_errno(errno, "TIOCSWINSZ ioctl for setting %s size failed: %m", ident);

        ফেরত 0;
}

পূর্ণসংখ্যা proc_cmdline_tty_আকার(ধ্রুবক অক্ষর *tty, unsigned *ret_rows, unsigned *ret_cols) {
        _cleanup_free_ অক্ষর *rowskey = ফাঁকা, *rowsvalue = ফাঁকা, *colskey = ফাঁকা, *colsvalue = ফাঁকা;
        unsigned rows = UINT_MAX, cols = UINT_MAX;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(tty);

        যদি (!ret_rows && !ret_cols)
                ফেরত 0;

        tty = skip_dev_prefix(tty);
        যদি (path_startswith(tty, "pts/"))
                ফেরত -EMEDIUMTYPE;
        যদি (!in_charset(tty, ALPHANUMERICAL))
                ফেরত log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "TTY name '%s' contains non-alphanumeric characters, not searching kernel cmdline for size.", tty);

        rowskey = strjoin("systemd.tty.rows.", tty);
        যদি (!rowskey)
                ফেরত -ENOMEM;

        colskey = strjoin("systemd.tty.columns.", tty);
        যদি (!colskey)
                ফেরত -ENOMEM;

        r = proc_cmdline_get_চাবি_many(/* flags= */ 0,
                                      rowskey, &rowsvalue,
                                      colskey, &colsvalue);
        যদি (r < 0)
                ফেরত log_debug_errno(r, "Failed to read TTY size of %s from kernel cmdline: %m", tty);

        যদি (rowsvalue) {
                r = safe_atou(rowsvalue, &rows);
                যদি (r < 0)
                        ফেরত log_debug_errno(r, "Failed to parse %s=%s: %m", rowskey, rowsvalue);
        }

        যদি (colsvalue) {
                r = safe_atou(colsvalue, &cols);
                যদি (r < 0)
                        ফেরত log_debug_errno(r, "Failed to parse %s=%s: %m", colskey, colsvalue);
        }

        যদি (ret_rows)
                *ret_rows = rows;
        যদি (ret_cols)
                *ret_cols = cols;

        ফেরত rows != UINT_MAX || cols != UINT_MAX;
}

/* intended to be used as a SIGWINCH sighandler */
শূন্য columns_সারিগুলি_cache_reset(পূর্ণসংখ্যা signum) {
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

        ফেরত cached_on_tty;
}

পূর্ণসংখ্যা getttyname_malloc(পূর্ণসংখ্যা fd, অক্ষর **ret) {
        অক্ষর path[PATH_MAX]; /* PATH_MAX is counted *with* the trailing NUL byte */
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(fd >= 0);
        প্রতিজ্ঞা(ret);

        r = ttyname_r(fd, path, আকার path); /* positive error */
        প্রতিজ্ঞা(r >= 0);
        যদি (r == ERANGE)
                ফেরত -ENAMETOOLONG;
        যদি (r > 0)
                ফেরত -r;

        ফেরত strdup_to(ret, skip_dev_prefix(path));
}

পূর্ণসংখ্যা getttyname_harder(পূর্ণসংখ্যা fd, অক্ষর **ret) {
        _cleanup_free_ অক্ষর *s = ফাঁকা;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(ret);

        r = getttyname_malloc(fd, &s);
        যদি (r < 0)
                ফেরত r;

        যদি (streq(s, "tty"))
                ফেরত get_ctty(0, ফাঁকা, ret);

        *ret = TAKE_PTR(s);
        ফেরত 0;
}

পূর্ণসংখ্যা get_ctty_devnr(pid_t pid, dev_t *ret) {
        _cleanup_free_ অক্ষর *সারি = ফাঁকা;
        unsigned long ttynr;
        ধ্রুবক অক্ষর *p;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(pid >= 0);

        p = procfs_file_alloca(pid, "stat");
        r = read_one_সারি_file(p, &সারি);
        যদি (r < 0)
                ফেরত r;

        p = strrchr(সারি, ')');
        যদি (!p)
                ফেরত -EIO;

        p++;

        যদি (sscanf(p, " "
                   "%*c "  /* state */
                   "%*d "  /* ppid */
                   "%*d "  /* pgrp */
                   "%*d "  /* session */
                   "%lu ", /* ttynr */
                   &ttynr) != 1)
                ফেরত -EIO;

        যদি (devnum_is_zero(ttynr))
                ফেরত -ENXIO;

        যদি (ret)
                *ret = (dev_t) ttynr;

        ফেরত 0;
}

পূর্ণসংখ্যা get_ctty(pid_t pid, dev_t *ret_devnr, অক্ষর **ret) {
        অক্ষর pty[STRLEN("/dev/pts/") + DECIMAL_STR_MAX(dev_t) + 1];
        _cleanup_free_ অক্ষর *buf = ফাঁকা;
        ধ্রুবক অক্ষর *fn = ফাঁকা, *w;
        dev_t devnr;
        পূর্ণসংখ্যা r;

        r = get_ctty_devnr(pid, &devnr);
        যদি (r < 0)
                ফেরত r;

        r = device_path_make_canonical(S_IFCHR, devnr, &buf);
        যদি (r < 0) {
                কাঠামো stat st;

                যদি (r != -ENOENT) /* No symlink for this in /dev/char/? */
                        ফেরত r;

                /* Maybe this is PTY? PTY devices are not listed in /dev/char/, as they don't follow the
                 * Linux device model and hence device_path_make_canonical() doesn't work for them. Let's
                 * assume this is a PTY for a moment, and check if the device node this would then map to in
                 * /dev/pts/ matches the one we are looking for. This way we don't have to hardcode the major
                 * number (which is 136 btw), but we still rely on the fact that PTY numbers map directly to
                 * the minor number of the pty. */
                xsprintf(pty, "/dev/pts/%u", minor(devnr));

                যদি (stat(pty, &st) < 0) {
                        যদি (errno != ENOENT)
                                ফেরত -errno;

                } অন্যথা যদি (S_ISCHR(st.st_mode) && devnr == st.st_rdev) /* Bingo! */
                        fn = pty;

                যদি (!fn) {
                        /* Doesn't exist, or not a PTY? Probably something similar to the PTYs which have no
                         * symlink in /dev/char/. Let's return something vaguely useful. */
                        r = device_path_make_major_minor(S_IFCHR, devnr, &buf);
                        যদি (r < 0)
                                ফেরত r;

                        fn = buf;
                }
        } অন্যথা
                fn = buf;

        w = path_startswith(fn, "/dev/");
        যদি (!w)
                ফেরত -EINVAL;

        যদি (ret) {
                r = strdup_to(ret, w);
                যদি (r < 0)
                        ফেরত r;
        }

        যদি (ret_devnr)
                *ret_devnr = devnr;

        ফেরত 0;
}

পূর্ণসংখ্যা ptsname_malloc(পূর্ণসংখ্যা fd, অক্ষর **ret) {
        প্রতিজ্ঞা(fd >= 0);
        প্রতিজ্ঞা(ret);

        জন্য (size_t l = 50;;) {
                _cleanup_free_ অক্ষর *c = ফাঁকা;

                c = new(অক্ষর, l);
                যদি (!c)
                        ফেরত -ENOMEM;

                যদি (ptsname_r(fd, c, l) >= 0) {
                        *ret = TAKE_PTR(c);
                        ফেরত 0;
                }
                যদি (errno != ERANGE)
                        ফেরত -errno;

                যদি (!MUL_ASSIGN_SAFE(&l, 2))
                        ফেরত -ENOMEM;
        }
}

পূর্ণসংখ্যা openpt_allocate(পূর্ণসংখ্যা flags, অক্ষর **ret_peer_path) {
        _cleanup_close_ পূর্ণসংখ্যা fd = -EBADF;
        পূর্ণসংখ্যা r;

        fd = posix_openpt(flags|O_NOCTTY|O_CLOEXEC);
        যদি (fd < 0)
                ফেরত -errno;

        _cleanup_free_ অক্ষর *p = ফাঁকা;
        যদি (ret_peer_path) {
                r = ptsname_malloc(fd, &p);
                যদি (r < 0)
                        ফেরত r;

                যদি (!path_startswith(p, "/dev/pts/"))
                        ফেরত -EINVAL;
        }

        যদি (unlockpt(fd) < 0)
                ফেরত -errno;

        যদি (ret_peer_path)
                *ret_peer_path = TAKE_PTR(p);

        ফেরত TAKE_FD(fd);
}

স্থির পূর্ণসংখ্যা ptsname_namespace(পূর্ণসংখ্যা pty, অক্ষর **ret) {
        পূর্ণসংখ্যা no = -1;

        প্রতিজ্ঞা(pty >= 0);
        প্রতিজ্ঞা(ret);

        /* Like ptsname(), but doesn't assume that the path is
         * accessible in the local namespace. */

        যদি (ioctl(pty, TIOCGPTN, &no) < 0)
                ফেরত -errno;

        যদি (no < 0)
                ফেরত -EIO;

        যদি (asprintf(ret, "/dev/pts/%i", no) < 0)
                ফেরত -ENOMEM;

        ফেরত 0;
}

পূর্ণসংখ্যা openpt_allocate_in_namespace(
                ধ্রুবক PidRef *pidref,
                পূর্ণসংখ্যা flags,
                অক্ষর **ret_peer_path) {

        _cleanup_close_ পূর্ণসংখ্যা pidnsfd = -EBADF, mntnsfd = -EBADF, usernsfd = -EBADF, rootfd = -EBADF, fd = -EBADF;
        _cleanup_close_pair_ পূর্ণসংখ্যা pair[2] = EBADF_PAIR;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(pidref);

        r = pidref_namespace_open(pidref, &pidnsfd, &mntnsfd, /* ret_netns_fd= */ ফাঁকা, &usernsfd, &rootfd);
        যদি (r < 0)
                ফেরত log_debug_errno(r, "Failed to open namespaces of PID "PID_FMT": %m", pidref->pid);

        যদি (socketpair(AF_UNIX, SOCK_DGRAM|SOCK_CLOEXEC, 0, pair) < 0)
                ফেরত -errno;

        r = namespace_fork(
                        "(sd-openptns)",
                        "(sd-openpt)",
                        FORK_RESET_SIGNALS|FORK_DEATHSIG_SIGKILL|FORK_WAIT,
                        pidnsfd,
                        mntnsfd,
                        /* netns_fd= */ -EBADF,
                        usernsfd,
                        rootfd,
                        /* ret= */ ফাঁকা);
        যদি (r < 0)
                ফেরত r;
        যদি (r == 0) {
                pair[0] = safe_close(pair[0]);

                fd = openpt_allocate(flags, /* ret_peer_path= */ ফাঁকা);
                যদি (fd < 0)
                        _exit(EXIT_FAILURE);

                যদি (send_one_fd(pair[1], fd, 0) < 0)
                        _exit(EXIT_FAILURE);

                _exit(EXIT_SUCCESS);
        }

        pair[1] = safe_close(pair[1]);

        fd = receive_one_fd(pair[0], 0);
        যদি (fd < 0)
                ফেরত fd;

        যদি (ret_peer_path) {
                r = ptsname_namespace(fd, ret_peer_path);
                যদি (r < 0)
                        ফেরত r;
        }

        ফেরত TAKE_FD(fd);
}

স্থির bool on_dev_null(শূন্য) {
        কাঠামো stat dst, ost, est;

        যদি (cached_on_dev_null >= 0)
                ফেরত cached_on_dev_null;

        যদি (stat("/dev/null", &dst) < 0 || fstat(STDOUT_FILENO, &ost) < 0 || fstat(STDERR_FILENO, &est) < 0)
                cached_on_dev_null = false;
        অন্যথা
                cached_on_dev_null = stat_inode_same(&dst, &ost) && stat_inode_same(&dst, &est);

        ফেরত cached_on_dev_null;
}

bool term_env_valid(ধ্রুবক অক্ষর *term) {
        /* Checks if the specified $TERM value is suitable for propagation, i.e. is not empty, not set to
         * "unknown" (as is common in CI), and only contains characters valid in terminal type names.
         * Valid $TERM values are things like "xterm-256color", "linux", "screen.xterm-256color", i.e.
         * alphanumeric characters, hyphens, underscores, dots, and plus signs. */
        ফেরত !isempty(term) &&
                !streq(term, "unknown") &&
                in_charset(term, ALPHANUMERICAL "-_+.");
}

bool getenv_terminal_is_dumb(শূন্য) {
        ধ্রুবক অক্ষর *e;

        e = getenv("TERM");
        যদি (!e)
                ফেরত true;

        ফেরত streq(e, "dumb");
}

bool terminal_is_dumb(শূন্য) {
        যদি (!on_tty() && !on_dev_null())
                ফেরত true;

        ফেরত getenv_terminal_is_dumb();
}

bool dev_console_colors_enabled(শূন্য) {
        _cleanup_free_ অক্ষর *s = ফাঁকা;
        ColorMode m;

        /* Returns true if we assume that color is supported on /dev/console.
         *
         * For that we first check if we explicitly got told to use colors or not, by checking $SYSTEMD_COLORS. If that
         * isn't set we check whether PID 1 has $TERM set, and if not, whether TERM is set on the kernel command
         * line. If we find $TERM set we assume color if it's not set to "dumb", similarly to how regular
         * colors_enabled() operates. */

        m = parse_systemd_colors();
        যদি (m >= 0)
                ফেরত m;

        যদি (getenv("NO_COLOR"))
                ফেরত false;

        যদি (getenv_for_pid(1, "TERM", &s) <= 0)
                (শূন্য) proc_cmdline_get_চাবি("TERM", 0, &s);

        ফেরত !streq_ptr(s, "dumb");
}

পূর্ণসংখ্যা vt_restore(পূর্ণসংখ্যা fd) {

        স্থির ধ্রুবক কাঠামো vt_mode mode = {
                .mode = VT_AUTO,
        };

        পূর্ণসংখ্যা r, ret = 0;

        প্রতিজ্ঞা(fd >= 0);

        যদি (!isatty_safe(fd))
                ফেরত log_debug_errno(SYNTHETIC_ERRNO(ENOTTY), "Asked to restore the VT for an fd that does not refer to a terminal.");

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

        ফেরত ret;
}

পূর্ণসংখ্যা vt_release(পূর্ণসংখ্যা fd, bool restore) {
        প্রতিজ্ঞা(fd >= 0);

        /* This function releases the VT by acknowledging the VT-switch signal
         * sent by the kernel and optionally reset the VT in text and auto
         * VT-switching modes. */

        যদি (!isatty_safe(fd))
                ফেরত log_debug_errno(SYNTHETIC_ERRNO(ENOTTY), "Asked to release the VT for an fd that does not refer to a terminal.");

        যদি (ioctl(fd, VT_RELDISP, 1) < 0)
                ফেরত -errno;

        যদি (restore)
                ফেরত vt_restore(fd);

        ফেরত 0;
}

শূন্য get_log_colors(পূর্ণসংখ্যা priority, ধ্রুবক অক্ষর **on, ধ্রুবক অক্ষর **off, ধ্রুবক অক্ষর **highlight) {
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

পূর্ণসংখ্যা terminal_set_cursor_position(পূর্ণসংখ্যা fd, unsigned row, unsigned column) {
        প্রতিজ্ঞা(fd >= 0);

        অক্ষর cursor_position[STRLEN("\x1B[" ";" "H") + DECIMAL_STR_MAX(unsigned) * 2 + 1];
        xsprintf(cursor_position, "\x1B[%u;%uH", row, column);

        ফেরত loop_write(fd, cursor_position, SIZE_MAX);
}

স্থির পূর্ণসংখ্যা terminal_verify_same(পূর্ণসংখ্যা প্রবেশ_fd, পূর্ণসংখ্যা output_fd) {
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(প্রবেশ_fd >= 0);
        প্রতিজ্ঞা(output_fd >= 0);

        /* Validates that the specified fds reference the same TTY */

        যদি (প্রবেশ_fd != output_fd) {
                কাঠামো stat sti;
                যদি (fstat(প্রবেশ_fd, &sti) < 0)
                        ফেরত -errno;

                r = stat_verify_char(&sti); /* TTYs are character devices */
                যদি (r < 0)
                        ফেরত r;

                কাঠামো stat sto;
                যদি (fstat(output_fd, &sto) < 0)
                        ফেরত -errno;

                r = stat_verify_char(&sto);
                যদি (r < 0)
                        ফেরত r;

                যদি (sti.st_rdev != sto.st_rdev)
                        ফেরত -ENOLINK;
        }

        যদি (!isatty_safe(প্রবেশ_fd)) /* The check above was just for char device, but now let's ensure it's actually a tty */
                ফেরত -ENOTTY;

        ফেরত 0;
}

প্রকারলেখ গণনা CursorPositionState {
        CURSOR_TEXT,
        CURSOR_ESCAPE,
        CURSOR_ROW,
        CURSOR_COLUMN,
} CursorPositionState;

প্রকারলেখ কাঠামো CursorPositionContext {
        CursorPositionState state;
        unsigned row, column;
} CursorPositionContext;

স্থির পূর্ণসংখ্যা scan_cursor_position_response(
                CursorPositionContext *context,
                ধ্রুবক অক্ষর *buf,
                size_t আকার_2,
                size_t *ret_processed) {

        প্রতিজ্ঞা(context);
        প্রতিজ্ঞা(buf);
        প্রতিজ্ঞা(ret_processed);

        জন্য (size_t i = 0; i < আকার_2; i++) {
                অক্ষর c = buf[i];

                নির্বাচন (context->state) {

                ক্ষেত্র CURSOR_TEXT:
                        context->state = c == '\x1B' ? CURSOR_ESCAPE : CURSOR_TEXT;
                        বিরতি;

                ক্ষেত্র CURSOR_ESCAPE:
                        context->state = c == '[' ? CURSOR_ROW : CURSOR_TEXT;
                        বিরতি;

                ক্ষেত্র CURSOR_ROW:
                        যদি (c == ';')
                                context->state = context->row > 0 ? CURSOR_COLUMN : CURSOR_TEXT;
                        অন্যথা {
                                পূর্ণসংখ্যা d = undecchar(c);

                                /* We read a decimal character, let's suffix it to the number we so far read,
                                 * but let's do an overflow check first. */
                                যদি (d < 0 || context->row > (UINT_MAX-d)/10)
                                        context->state = CURSOR_TEXT;
                                অন্যথা
                                        context->row = context->row * 10 + d;
                        }
                        বিরতি;

                ক্ষেত্র CURSOR_COLUMN:
                        যদি (c == 'R') {
                                যদি (context->column > 0) {
                                        *ret_processed = i + 1;
                                        ফেরত 1; /* success! */
                                }

                                context->state = CURSOR_TEXT;
                        } অন্যথা {
                                পূর্ণসংখ্যা d = undecchar(c);

                                /* As above, add the decimal character to our column number */
                                যদি (d < 0 || context->column > (UINT_MAX-d)/10)
                                        context->state = CURSOR_TEXT;
                                অন্যথা
                                        context->column = context->column * 10 + d;
                        }

                        বিরতি;
                }

                /* Reset any positions we might have picked up */
                যদি (IN_SET(context->state, CURSOR_TEXT, CURSOR_ESCAPE))
                        context->row = context->column = 0;
        }

        *ret_processed = আকার_2;
        ফেরত 0; /* all good, but not enough data yet */
}

পূর্ণসংখ্যা terminal_get_cursor_position(
                পূর্ণসংখ্যা প্রবেশ_fd,
                পূর্ণসংখ্যা output_fd,
                unsigned *ret_row,
                unsigned *ret_column) {

        _cleanup_close_ পূর্ণসংখ্যা nonblock_প্রবেশ_fd = -EBADF;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(প্রবেশ_fd >= 0);
        প্রতিজ্ঞা(output_fd >= 0);

        যদি (getenv_terminal_is_dumb())
                ফেরত -EOPNOTSUPP;

        r = terminal_verify_same(প্রবেশ_fd, output_fd);
        যদি (r < 0)
                ফেরত log_debug_errno(r, "Called with distinct input/output fds: %m");

        /* Failure to reset the terminal is ignored here and in similar cases below.
         * We already have our result; if cleanup fails it doesn't change the validity of the result. */
        কাঠামো termios old_termios = TERMIOS_NULL;
        CLEANUP_TERMIOS_RESET(প্রবেশ_fd, old_termios);

        যদি (tcgetattr(প্রবেশ_fd, &old_termios) < 0)
                ফেরত log_debug_errno(errno, "Failed to get terminal settings: %m");

        কাঠামো termios new_termios = old_termios;
        termios_disable_echo(&new_termios);

        যদি (tcsetattr(প্রবেশ_fd, TCSANOW, &new_termios) < 0)
                ফেরত log_debug_errno(errno, "Failed to set new terminal settings: %m");

        /* Request cursor position (DSR/CPR) */
        r = loop_write(output_fd, "\x1B[6n", SIZE_MAX);
        যদি (r < 0)
                ফেরত r;

        /* Open a 2nd input fd, in non-blocking mode, so that we won't ever hang in read() should someone
         * else process the POLLIN. */

        nonblock_প্রবেশ_fd = r = fd_reopen(প্রবেশ_fd, O_RDONLY|O_CLOEXEC|O_NONBLOCK|O_NOCTTY);
        যদি (r < 0)
                ফেরত r;

        usec_t end = usec_add(now(CLOCK_MONOTONIC), CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        অক্ষর buf[STRLEN("\x1B[1;1R")]; /* The shortest valid reply possible */
        size_t buf_full = 0;
        CursorPositionContext context = {};

        জন্য (bool first = true;; first = false) {
                যদি (buf_full == 0) {
                        usec_t n = now(CLOCK_MONOTONIC);
                        যদি (n >= end)
                                ফেরত -EOPNOTSUPP;

                        r = fd_wait_for_event(nonblock_প্রবেশ_fd, POLLIN, usec_sub_unsigned(end, n));
                        যদি (r < 0)
                                ফেরত r;
                        যদি (r == 0)
                                ফেরত -EOPNOTSUPP;

                        /* On the first try, read multiple characters, i.e. the shortest valid
                         * reply. Afterwards read byte-wise, since we don't want to read too much, and
                         * unnecessarily drop too many characters from the input queue. */
                        ssize_t l = পঠ(nonblock_প্রবেশ_fd, buf, first ? আকার(buf) : 1);
                        যদি (l < 0) {
                                যদি (errno == EAGAIN)
                                        চালু;

                                ফেরত -errno;
                        }

                        প্রতিজ্ঞা((size_t) l <= আকার(buf));
                        buf_full = l;
                }

                size_t processed;
                r = scan_cursor_position_response(&context, buf, buf_full, &processed);
                যদি (r < 0)
                        ফেরত r;

                প্রতিজ্ঞা(processed <= buf_full);
                buf_full -= processed;
                memmove(buf, buf + processed, buf_full);

                যদি (r > 0) {
                        /* Superficial validity check */
                        যদি (context.row >= 32766 || context.column >= 32766)
                                ফেরত -ENODATA;

                        যদি (ret_row)
                                *ret_row = context.row;
                        যদি (ret_column)
                                *ret_column = context.column;

                        ফেরত 0;
                }
        }
}

পূর্ণসংখ্যা terminal_reset_defensive(পূর্ণসংখ্যা fd, TerminalResetFlags flags) {
        পূর্ণসংখ্যা r = 0;

        প্রতিজ্ঞা(fd >= 0);
        প্রতিজ্ঞা(!FLAGS_SET(flags, TERMINAL_RESET_AVOID_ANSI_SEQ|TERMINAL_RESET_FORCE_ANSI_SEQ));

        /* Resets the terminal comprehensively, i.e. via both ioctl()s and via ANSI sequences, but do so only
         * if $TERM is unset or set to "dumb" */

        যদি (!isatty_safe(fd))
                ফেরত -ENOTTY;

        RET_GATHER(r, terminal_reset_ioctl(fd, FLAGS_SET(flags, TERMINAL_RESET_SWITCH_TO_TEXT)));

        যদি (!FLAGS_SET(flags, TERMINAL_RESET_AVOID_ANSI_SEQ) &&
            (FLAGS_SET(flags, TERMINAL_RESET_FORCE_ANSI_SEQ) || !getenv_terminal_is_dumb()))
                RET_GATHER(r, terminal_reset_ansi_seq(fd));

        ফেরত r;
}

পূর্ণসংখ্যা terminal_reset_defensive_locked(পূর্ণসংখ্যা fd, TerminalResetFlags flags) {
        প্রতিজ্ঞা(fd >= 0);

        _cleanup_close_ পূর্ণসংখ্যা lock_fd = lock_dev_console();
        যদি (lock_fd < 0)
                log_debug_errno(lock_fd, "Failed to acquire lock for /dev/console, ignoring: %m");

        ফেরত terminal_reset_defensive(fd, flags);
}

শূন্য termios_disable_echo(কাঠামো termios *termios) {
        প্রতিজ্ঞা(termios);

        termios->c_lflag &= ~(ICANON|ECHO);
        termios->c_cc[VMIN] = 1;
        termios->c_cc[VTIME] = 0;
}

স্থির bool termios_is_null(ধ্রুবক কাঠামো termios *t) {
        যদি (!t)
                ফেরত true;

        ফেরত t->c_iflag == UINT_MAX &&
               t->c_oflag == UINT_MAX &&
               t->c_cflag == UINT_MAX &&
               t->c_lflag == UINT_MAX;
}

শূন্য termios_reset(ধ্রুবক TermiosResetContext *c) {
        প্রতিজ্ঞা(c);

        PROTECT_ERRNO;

        যদি (c->fd && *c->fd >= 0 && !termios_is_null(c->termios))
                (শূন্য) tcsetattr(*c->fd, TCSANOW, c->termios);
}

প্রকারলেখ গণনা BackgroundColorState {
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

প্রকারলেখ কাঠামো BackgroundColorContext {
        BackgroundColorState state;
        uint32_t red, green, blue;
        unsigned red_bits, green_bits, blue_bits;
} BackgroundColorContext;

স্থির পূর্ণসংখ্যা scan_background_color_response(
                BackgroundColorContext *context,
                ধ্রুবক অক্ষর *buf,
                size_t আকার_2,
                size_t *ret_processed) {

        প্রতিজ্ঞা(context);
        প্রতিজ্ঞা(buf);
        প্রতিজ্ঞা(ret_processed);

        জন্য (size_t i = 0; i < আকার_2; i++) {
                অক্ষর c = buf[i];

                নির্বাচন (context->state) {

                ক্ষেত্র BACKGROUND_TEXT:
                        context->state = c == '\x1B' ? BACKGROUND_ESCAPE : BACKGROUND_TEXT;
                        বিরতি;

                ক্ষেত্র BACKGROUND_ESCAPE:
                        context->state = c == ']' ? BACKGROUND_BRACKET : BACKGROUND_TEXT;
                        বিরতি;

                ক্ষেত্র BACKGROUND_BRACKET:
                        context->state = c == '1' ? BACKGROUND_FIRST_ONE : BACKGROUND_TEXT;
                        বিরতি;

                ক্ষেত্র BACKGROUND_FIRST_ONE:
                        context->state = c == '1' ? BACKGROUND_SECOND_ONE : BACKGROUND_TEXT;
                        বিরতি;

                ক্ষেত্র BACKGROUND_SECOND_ONE:
                        context->state = c == ';' ? BACKGROUND_SEMICOLON : BACKGROUND_TEXT;
                        বিরতি;

                ক্ষেত্র BACKGROUND_SEMICOLON:
                        context->state = c == 'r' ? BACKGROUND_R : BACKGROUND_TEXT;
                        বিরতি;

                ক্ষেত্র BACKGROUND_R:
                        context->state = c == 'g' ? BACKGROUND_G : BACKGROUND_TEXT;
                        বিরতি;

                ক্ষেত্র BACKGROUND_G:
                        context->state = c == 'b' ? BACKGROUND_B : BACKGROUND_TEXT;
                        বিরতি;

                ক্ষেত্র BACKGROUND_B:
                        context->state = c == ':' ? BACKGROUND_RED : BACKGROUND_TEXT;
                        বিরতি;

                ক্ষেত্র BACKGROUND_RED:
                        যদি (c == '/')
                                context->state = context->red_bits > 0 ? BACKGROUND_GREEN : BACKGROUND_TEXT;
                        অন্যথা {
                                পূর্ণসংখ্যা d = unhexchar(c);
                                যদি (d < 0 || context->red_bits >= আকার(context->red)*8)
                                        context->state = BACKGROUND_TEXT;
                                অন্যথা {
                                        context->red = (context->red << 4) | d;
                                        context->red_bits += 4;
                                }
                        }
                        বিরতি;

                ক্ষেত্র BACKGROUND_GREEN:
                        যদি (c == '/')
                                context->state = context->green_bits > 0 ? BACKGROUND_BLUE : BACKGROUND_TEXT;
                        অন্যথা {
                                পূর্ণসংখ্যা d = unhexchar(c);
                                যদি (d < 0 || context->green_bits >= আকার(context->green)*8)
                                        context->state = BACKGROUND_TEXT;
                                অন্যথা {
                                        context->green = (context->green << 4) | d;
                                        context->green_bits += 4;
                                }
                        }
                        বিরতি;

                ক্ষেত্র BACKGROUND_BLUE:
                        যদি (c == '\x07') {
                                যদি (context->blue_bits > 0) {
                                        *ret_processed = i + 1;
                                        ফেরত 1; /* success! */
                                }

                                context->state = BACKGROUND_TEXT;
                        } অন্যথা যদি (c == '\x1b')
                                context->state = context->blue_bits > 0 ? BACKGROUND_STRING_TERMINATOR : BACKGROUND_TEXT;
                        অন্যথা {
                                পূর্ণসংখ্যা d = unhexchar(c);
                                যদি (d < 0 || context->blue_bits >= আকার(context->blue)*8)
                                        context->state = BACKGROUND_TEXT;
                                অন্যথা {
                                        context->blue = (context->blue << 4) | d;
                                        context->blue_bits += 4;
                                }
                        }
                        বিরতি;

                ক্ষেত্র BACKGROUND_STRING_TERMINATOR:
                        যদি (c == '\\') {
                                *ret_processed = i + 1;
                                ফেরত 1; /* success! */
                        }

                        context->state = c == ']' ? BACKGROUND_ESCAPE : BACKGROUND_TEXT;
                        বিরতি;

                }

                /* Reset any colors we might have picked up */
                যদি (IN_SET(context->state, BACKGROUND_TEXT, BACKGROUND_ESCAPE)) {
                        /* reset color */
                        context->red = context->green = context->blue = 0;
                        context->red_bits = context->green_bits = context->blue_bits = 0;
                }
        }

        *ret_processed = আকার_2;
        ফেরত 0; /* all good, but not enough data yet */
}

পূর্ণসংখ্যা get_default_background_color(দ্বিগুণ *ret_red, দ্বিগুণ *ret_green, দ্বিগুণ *ret_blue) {
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(ret_red);
        প্রতিজ্ঞা(ret_green);
        প্রতিজ্ঞা(ret_blue);

        যদি (!colors_enabled())
                ফেরত -EOPNOTSUPP;

        r = terminal_verify_same(STDIN_FILENO, STDOUT_FILENO);
        যদি (r < 0)
                ফেরত r;

        যদি (streq_ptr(getenv("TERM"), "linux")) {
                /* Linux console is black */
                *ret_red = *ret_green = *ret_blue = 0.0;
                ফেরত 0;
        }

        /* Open a 2nd input fd, in non-blocking mode, so that we won't ever hang in read()
         * should someone else process the POLLIN. Do all subsequent operations on the new fd. */
        _cleanup_close_ পূর্ণসংখ্যা nonblock_প্রবেশ_fd = r = fd_reopen(STDIN_FILENO, O_RDONLY|O_CLOEXEC|O_NONBLOCK|O_NOCTTY);
        যদি (r < 0)
                ফেরত r;

        কাঠামো termios old_termios = TERMIOS_NULL;
        CLEANUP_TERMIOS_RESET(nonblock_প্রবেশ_fd, old_termios);

        যদি (tcgetattr(nonblock_প্রবেশ_fd, &old_termios) < 0)
                ফেরত -errno;

        কাঠামো termios new_termios = old_termios;
        termios_disable_echo(&new_termios);

        যদি (tcsetattr(nonblock_প্রবেশ_fd, TCSANOW, &new_termios) < 0)
                ফেরত -errno;

        r = loop_write(STDOUT_FILENO, ANSI_OSC "11;?" ANSI_ST, SIZE_MAX);
        যদি (r < 0)
                ফেরত r;

        usec_t end = usec_add(now(CLOCK_MONOTONIC), CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        অক্ষর buf[STRLEN(ANSI_OSC "11;rgb:0/0/0" ANSI_ST)]; /* shortest possible reply */
        size_t buf_full = 0;
        BackgroundColorContext context = {};

        জন্য (bool first = true;; first = false) {
                যদি (buf_full == 0) {
                        usec_t n = now(CLOCK_MONOTONIC);
                        যদি (n >= end)
                                ফেরত -EOPNOTSUPP;

                        r = fd_wait_for_event(nonblock_প্রবেশ_fd, POLLIN, usec_sub_unsigned(end, n));
                        যদি (r < 0)
                                ফেরত r;
                        যদি (r == 0)
                                ফেরত -EOPNOTSUPP;

                        /* On the first try, read multiple characters, i.e. the shortest valid
                         * reply. Afterwards read byte-wise, since we don't want to read too much, and
                         * unnecessarily drop too many characters from the input queue. */
                        ssize_t l = পঠ(nonblock_প্রবেশ_fd, buf, first ? আকার(buf) : 1);
                        যদি (l < 0) {
                                যদি (errno == EAGAIN)
                                        চালু;
                                ফেরত -errno;
                        }

                        প্রতিজ্ঞা((size_t) l <= আকার(buf));
                        buf_full = l;
                }

                size_t processed;
                r = scan_background_color_response(&context, buf, buf_full, &processed);
                যদি (r < 0)
                        ফেরত r;

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
                        ফেরত 0;
                }
        }
}

/* Determine terminal dimensions by means of ANSI sequences: save the cursor via DECSC, position it far
 * to the bottom right (clamped to actual terminal dimensions), read back via DSR where we ended up, and
 * restore cursor via DECRC. Only needs a single DSR round-trip, and always restores the cursor regardless
 * of whether the response is received.
 *
 * Caller must have already opened a non-blocking input fd and configured termios (echo/icanon off). */
স্থির পূর্ণসংখ্যা terminal_query_আকার_by_dsr(
                পূর্ণসংখ্যা nonblock_প্রবেশ_fd,
                পূর্ণসংখ্যা output_fd,
                unsigned *ret_rows,
                unsigned *ret_columns) {

        পূর্ণসংখ্যা r;

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
                ফেরত r;

        usec_t end = usec_add(now(CLOCK_MONOTONIC), CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        অক্ষর buf[STRLEN("\x1B[1;1R")]; /* The shortest valid reply possible */
        size_t buf_full = 0;
        CursorPositionContext context = {};

        জন্য (bool first = true;; first = false) {
                যদি (buf_full == 0) {
                        usec_t n = now(CLOCK_MONOTONIC);
                        যদি (n >= end)
                                ফেরত -EOPNOTSUPP;

                        r = fd_wait_for_event(nonblock_প্রবেশ_fd, POLLIN, usec_sub_unsigned(end, n));
                        যদি (r < 0)
                                ফেরত r;
                        যদি (r == 0)
                                ফেরত -EOPNOTSUPP;

                        /* On the first try, read multiple characters, i.e. the shortest valid
                         * reply. Afterwards read byte-wise, since we don't want to read too much, and
                         * unnecessarily drop too many characters from the input queue. */
                        ssize_t l = পঠ(nonblock_প্রবেশ_fd, buf, first ? আকার(buf) : 1);
                        যদি (l < 0) {
                                যদি (errno == EAGAIN)
                                        চালু;

                                ফেরত -errno;
                        }

                        প্রতিজ্ঞা((size_t) l <= আকার(buf));
                        buf_full = l;
                }

                size_t processed;
                r = scan_cursor_position_response(&context, buf, buf_full, &processed);
                যদি (r < 0)
                        ফেরত r;

                প্রতিজ্ঞা(processed <= buf_full);
                buf_full -= processed;
                memmove(buf, buf + processed, buf_full);

                যদি (r > 0) {
                        /* Superficial validity checks (no particular reason to check for < 4, it's
                         * just a way to look for unreasonably small values) */
                        যদি (context.row < 4 || context.column < 4 || context.row >= 32766 || context.column >= 32766)
                                ফেরত -ENODATA;

                        যদি (ret_rows)
                                *ret_rows = context.row;
                        যদি (ret_columns)
                                *ret_columns = context.column;

                        ফেরত 0;
                }
        }
}

/* Common setup for ANSI terminal queries: validate the fds, open a non-blocking input fd, and configure
 * termios with echo and canonical mode disabled. Caller must restore termios and close the fd when done. */
স্থির পূর্ণসংখ্যা terminal_prepare_query(
                পূর্ণসংখ্যা প্রবেশ_fd,
                পূর্ণসংখ্যা output_fd,
                পূর্ণসংখ্যা *ret_nonblock_fd,
                কাঠামো termios *ret_saved_termios) {

        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(প্রবেশ_fd >= 0);
        প্রতিজ্ঞা(output_fd >= 0);
        প্রতিজ্ঞা(ret_nonblock_fd);
        প্রতিজ্ঞা(ret_saved_termios);

        /* Use getenv_terminal_is_dumb() instead of terminal_is_dumb() here since we operate on an
         * explicitly passed fd, not on stdio. terminal_is_dumb() additionally checks on_tty() which
         * tests whether *stderr* is a tty — that's irrelevant when we're querying a directly opened
         * terminal such as /dev/console. */
        যদি (getenv_terminal_is_dumb())
                ফেরত -EOPNOTSUPP;

        r = terminal_verify_same(প্রবেশ_fd, output_fd);
        যদি (r < 0)
                ফেরত log_debug_errno(r, "Called with distinct input/output fds: %m");

        /* Open a 2nd input fd, in non-blocking mode, so that we won't ever hang in read()
         * should someone else process the POLLIN. Do all subsequent operations on the new fd. */
        _cleanup_close_ পূর্ণসংখ্যা nonblock_প্রবেশ_fd = r = fd_reopen(প্রবেশ_fd, O_RDONLY|O_CLOEXEC|O_NONBLOCK|O_NOCTTY);
        যদি (r < 0)
                ফেরত r;

        যদি (tcgetattr(nonblock_প্রবেশ_fd, ret_saved_termios) < 0)
                ফেরত log_debug_errno(errno, "Failed to get terminal settings: %m");

        কাঠামো termios new_termios = *ret_saved_termios;
        termios_disable_echo(&new_termios);

        যদি (tcsetattr(nonblock_প্রবেশ_fd, TCSANOW, &new_termios) < 0)
                ফেরত log_debug_errno(errno, "Failed to set new terminal settings: %m");

        *ret_nonblock_fd = TAKE_FD(nonblock_প্রবেশ_fd);
        ফেরত 0;
}

/*
 * See https://terminalguide.namepad.de/seq/csi_st-18/,
 * https://invisible-island.net/xterm/ctlseqs/ctlseqs.html#h3-Functions-using-CSI-_-ordered-by-the-final-character_s_.
 */
#define CSI18_Q  "\x1B[18t"               /* Report the size of the text area in characters */
#define CSI18_Rp "\x1B[8;"                /* Reply prefix */
#define CSI18_R0 CSI18_Rp "1;1t"          /* Shortest reply */
#define CSI18_R1 CSI18_Rp "32766;32766t"  /* Longest reply */

স্থির পূর্ণসংখ্যা scan_text_area_আকার_response(
                ধ্রুবক অক্ষর *buf,
                size_t আকার_2,
                unsigned *ret_rows,
                unsigned *ret_columns) {

        প্রতিজ্ঞা(buf);
        প্রতিজ্ঞা(ret_rows);
        প্রতিজ্ঞা(ret_columns);

        /* Check if we have enough space for the shortest possible answer. */
        যদি (আকার_2 < STRLEN(CSI18_R0))
                ফেরত -EAGAIN;

        /* Check if the terminating sequence is present */
        যদি (buf[আকার_2 - 1] != 't')
                ফেরত -EAGAIN;

        unsigned short rows, columns;
        যদি (sscanf(buf, CSI18_Rp "%hu;%hut", &rows, &columns) != 2)
                ফেরত -EINVAL;

        *ret_rows = rows;
        *ret_columns = columns;
        ফেরত 0;
}

/* Determine terminal dimensions by means of an ANSI CSI 18 sequence.
 *
 * Caller must have already opened a non-blocking input fd and configured termios (echo/icanon off). */
স্থির পূর্ণসংখ্যা terminal_query_আকার_by_csi18(
                পূর্ণসংখ্যা nonblock_প্রবেশ_fd,
                পূর্ণসংখ্যা output_fd,
                unsigned *ret_rows,
                unsigned *ret_columns) {

        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(nonblock_প্রবেশ_fd >= 0);
        প্রতিজ্ঞা(output_fd >= 0);

        r = loop_write_full(output_fd, CSI18_Q, SIZE_MAX, CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        যদি (r < 0)
                ফেরত r;

        usec_t end = usec_add(now(CLOCK_MONOTONIC), CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        অক্ষর buf[STRLEN(CSI18_R1)];
        size_t bytes = 0;

        জন্য (;;) {
                usec_t n = now(CLOCK_MONOTONIC);
                যদি (n >= end)
                        ফেরত -EOPNOTSUPP;

                r = fd_wait_for_event(nonblock_প্রবেশ_fd, POLLIN, usec_sub_unsigned(end, n));
                যদি (r < 0)
                        ফেরত r;
                যদি (r == 0)
                        ফেরত -EOPNOTSUPP;

                /* On the first read, read multiple characters, i.e. the shortest valid reply. Afterwards
                 * read byte by byte, since we don't want to read too much and drop characters from the input
                 * queue. */
                ssize_t l = পঠ(nonblock_প্রবেশ_fd, buf + bytes, bytes == 0 ? STRLEN(CSI18_R0) : 1);
                যদি (l < 0) {
                        যদি (errno == EAGAIN)
                                চালু;
                        ফেরত -errno;
                }

                প্রতিজ্ঞা((size_t) l <= আকার(buf) - bytes);
                bytes += l;

                r = scan_text_area_আকার_response(buf, bytes, ret_rows, ret_columns);
                যদি (r != -EAGAIN)
                        ফেরত r;

                যদি (bytes == আকার(buf))
                        ফেরত -EOPNOTSUPP; /* The response has the right prefix, but we didn't find a valid
                                             * answer with a terminator in the allotted space. Something is
                                             * wrong, possibly some unrelated bytes got injected into the
                                             * answer. */
        }
}

পূর্ণসংখ্যা terminal_get_আকার(
                পূর্ণসংখ্যা প্রবেশ_fd,
                পূর্ণসংখ্যা output_fd,
                unsigned *ret_rows,
                unsigned *ret_columns,
                bool try_dsr,
                bool try_csi18) {

        _cleanup_close_ পূর্ণসংখ্যা nonblock_প্রবেশ_fd = -EBADF;
        কাঠামো termios old_termios = TERMIOS_NULL;
        CLEANUP_TERMIOS_RESET(nonblock_প্রবেশ_fd, old_termios);
        _cleanup_(nonblock_resetp) পূর্ণসংখ্যা nonblock_reset = -EBADF;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(try_dsr || try_csi18);

        r = terminal_prepare_query(প্রবেশ_fd, output_fd, &nonblock_প্রবেশ_fd, &old_termios);
        যদি (r < 0)
                ফেরত r;

        /* Put the output fd in non-blocking mode with a write timeout, to avoid blocking indefinitely on
         * write if the terminal is not consuming data (e.g. serial console with flow control). */
        r = fd_nonblock(output_fd, true);
        যদি (r < 0)
                ফেরত log_debug_errno(r, "Failed to set terminal to non-blocking mode: %m");
        যদি (r > 0)
                nonblock_reset = output_fd;

        /* Flush any stale input that might confuse the response parsers. */
        (শূন্য) tcflush(nonblock_প্রবেশ_fd, TCIFLUSH);

        যদি (try_csi18) {
                r = terminal_query_আকার_by_csi18(nonblock_প্রবেশ_fd, output_fd, ret_rows, ret_columns);
                যদি (r >= 0)
                        ফেরত r;

                /* Query failed. Flush any outstanding input. */
                (শূন্য) tcflush(nonblock_প্রবেশ_fd, TCIFLUSH);

                যদি (!IN_SET(r, -EOPNOTSUPP, -EINVAL))
                        ফেরত r;
        }

        যদি (try_dsr) {
                r = terminal_query_আকার_by_dsr(nonblock_প্রবেশ_fd, output_fd, ret_rows, ret_columns);
                যদি (r >= 0)
                        ফেরত r;

                /* Query failed. Flush any outstanding input. */
                (শূন্য) tcflush(nonblock_প্রবেশ_fd, TCIFLUSH);
        }

        ফেরত r;
}

স্থির পূর্ণসংখ্যা fd_is_vc(পূর্ণসংখ্যা fd) {
        _cleanup_free_ অক্ষর *name = ফাঁকা;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(fd >= 0);

        r = getttyname_harder(fd, &name);
        যদি (r < 0)
                ফেরত r;

        ফেরত tty_is_vc_resolve(name);
}

পূর্ণসংখ্যা terminal_fix_আকার(পূর্ণসংখ্যা প্রবেশ_fd, পূর্ণসংখ্যা output_fd) {
        unsigned rows, columns;
        পূর্ণসংখ্যা r;

        /* Don't ask a virtual console at all. There is no intermediary here that could be wrong about the
         * dimensions: the kernel renders the screen itself, so its winsize is authoritative. */
        r = fd_is_vc(প্রবেশ_fd);
        যদি (r < 0)
                log_debug_errno(r, "Failed to determine whether terminal is a virtual console, assuming it is not: %m");
        অন্যথা যদি (r > 0) {
                log_debug("Not querying dimensions of a virtual console, kernel dimensions are authoritative.");
                ফেরত 0;
        }

        কাঠামো winsize ws = {};
        যদি (ioctl(output_fd, TIOCGWINSZ, &ws) < 0)
                ফেরত log_debug_errno(errno, "Failed to query terminal dimensions, ignoring: %m");

        r = terminal_get_আকার(প্রবেশ_fd, output_fd, &rows, &columns, /* try_dsr= */ true, /* try_csi18= */ true);
        যদি (r < 0)
                ফেরত log_debug_errno(r, "Failed to acquire terminal dimensions via ANSI sequences, not adjusting terminal dimensions: %m");

        যদি (ws.ws_row == rows && ws.ws_col == columns) {
                log_debug("Terminal dimensions reported via ANSI sequences match currently set terminal dimensions, not changing.");
                ফেরত 0;
        }

        ws.ws_col = columns;
        ws.ws_row = rows;

        যদি (ioctl(output_fd, TIOCSWINSZ, &ws) < 0)
                ফেরত log_debug_errno(errno, "Failed to update terminal dimensions, ignoring: %m");

        log_debug("Fixed terminal dimensions to %ux%u based on ANSI sequence information.", columns, rows);
        ফেরত 1;
}

#define MAX_TERMINFO_দৈর্ঘ্য 64
/* python -c 'print("".join(hex(ord(i))[2:] for i in "name").upper())' */
#define DCS_TERMINFO_Q ANSI_DCS "+q" "6E616D65" ANSI_ST
/* The answer is either 0+r… (invalid) or 1+r… (OK). */
#define DCS_TERMINFO_R0 ANSI_DCS "0+r" ANSI_ST
#define DCS_TERMINFO_R1 ANSI_DCS "1+r" "6E616D65" "=" /* This is followed by Pt ST. */
assert_cc(STRLEN(DCS_TERMINFO_R0) <= STRLEN(DCS_TERMINFO_R1 ANSI_ST));

স্থির পূর্ণসংখ্যা scan_terminfo_response(
                ধ্রুবক অক্ষর *buf,
                size_t আকার_2,
                অক্ষর **ret_name) {
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(buf);
        প্রতিজ্ঞা(ret_name);

        /* Check if we have enough space for the shortest possible answer. */
        যদি (আকার_2 < STRLEN(DCS_TERMINFO_R0))
                ফেরত -EAGAIN;

        /* Check if the terminating sequence is present */
        যদি (memcmp(buf + আকার_2 - STRLEN(ANSI_ST), ANSI_ST, STRLEN(ANSI_ST)) != 0)
                ফেরত -EAGAIN;

        যদি (আকার_2 <= STRLEN(DCS_TERMINFO_R1 ANSI_ST))
                ফেরত -EINVAL;  /* The answer is invalid or empty */

        যদি (memcmp(buf, DCS_TERMINFO_R1, STRLEN(DCS_TERMINFO_R1)) != 0)
                ফেরত -EINVAL;  /* The answer is not valid */

        _cleanup_free_ শূন্য *dec = ফাঁকা;
        size_t dec_আকার;
        r = unhexmem_full(buf + STRLEN(DCS_TERMINFO_R1), আকার_2 - STRLEN(DCS_TERMINFO_R1 ANSI_ST),
                          /* secure= */ false,
                          &dec, &dec_আকার);
        যদি (r < 0)
                ফেরত r;

        প্রতিজ্ঞা(((ধ্রুবক অক্ষর *) dec)[dec_আকার] == '\0'); /* unhexmem appends NUL for our convenience */
        যদি (memchr(dec, '\0', dec_আকার) || string_has_cc(dec, ফাঁকা) || !filename_is_valid(dec))
                ফেরত -EUCLEAN;

        *ret_name = TAKE_PTR(dec);
        ফেরত 0;
}

পূর্ণসংখ্যা terminal_get_terminfo_by_dcs(পূর্ণসংখ্যা fd, অক্ষর **ret_name) {
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(fd >= 0);
        প্রতিজ্ঞা(ret_name);

        /* Note: fd must be in non-blocking read-write mode! */

        কাঠামো termios old_termios = TERMIOS_NULL;
        CLEANUP_TERMIOS_RESET(fd, old_termios);

        যদি (tcgetattr(fd, &old_termios) < 0)
                ফেরত -errno;

        কাঠামো termios new_termios = old_termios;
        termios_disable_echo(&new_termios);

        যদি (tcsetattr(fd, TCSANOW, &new_termios) < 0)
                ফেরত -errno;

        r = loop_write(fd, DCS_TERMINFO_Q, SIZE_MAX);
        যদি (r < 0)
                ফেরত r;

        usec_t end = usec_add(now(CLOCK_MONOTONIC), CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        অক্ষর buf[STRLEN(DCS_TERMINFO_R1) + MAX_TERMINFO_দৈর্ঘ্য + STRLEN(ANSI_ST)];
        size_t bytes = 0;

        জন্য (;;) {
                usec_t n = now(CLOCK_MONOTONIC);
                যদি (n >= end)
                        ফেরত -EOPNOTSUPP;

                r = fd_wait_for_event(fd, POLLIN, usec_sub_unsigned(end, n));
                যদি (r < 0)
                        ফেরত r;
                যদি (r == 0)
                        ফেরত -EOPNOTSUPP;

                /* On the first read, read multiple characters, i.e. the shortest valid reply. Afterwards
                 * read byte by byte, since we don't want to read too much and drop characters from the input
                 * queue. */
                ssize_t l = পঠ(fd, buf + bytes, bytes == 0 ? STRLEN(DCS_TERMINFO_R0) : 1);
                যদি (l < 0) {
                        যদি (errno == EAGAIN)
                                চালু;
                        ফেরত -errno;
                }

                প্রতিজ্ঞা((size_t) l <= আকার(buf) - bytes);
                bytes += l;

                r = scan_terminfo_response(buf, bytes, ret_name);
                যদি (r != -EAGAIN)
                        ফেরত r;

                যদি (bytes == আকার(buf))
                        ফেরত -EOPNOTSUPP; /* The response has the right prefix, but we didn't find a valid
                                             * answer with a terminator in the allotted space. Something is
                                             * wrong, possibly some unrelated bytes got injected into the
                                             * answer. */
        }
}

পূর্ণসংখ্যা have_terminfo_file(ধ্রুবক অক্ষর *name) {
        /* This is a heuristic check if we have the file, using the directory layout used on
         * current Linux systems. Checks for other layouts can be added later if appropriate. */
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(filename_is_valid(name));

        _cleanup_free_ অক্ষর *p = path_join("/usr/share/terminfo", CHAR_TO_STR(name[0]), name);
        যদি (!p)
                ফেরত log_oom_debug();

        r = RET_NERRNO(access(p, F_OK));
        যদি (r == -ENOENT)
                ফেরত false;
        যদি (r < 0)
                ফেরত r;
        ফেরত true;
}

পূর্ণসংখ্যা query_term_for_tty(ধ্রুবক অক্ষর *tty, অক্ষর **ret_term) {
        _cleanup_free_ অক্ষর *dcs_term = ফাঁকা;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(tty);
        প্রতিজ্ঞা(ret_term);

        যদি (tty_is_vc_resolve(tty))
                ফেরত strdup_to(ret_term, "linux");

        /* Try to query the terminal implementation that we're on. This will not work in all
         * cases, which is fine, since this is intended to be used as a fallback. */

        _cleanup_close_ পূর্ণসংখ্যা tty_fd = open_terminal(tty, O_RDWR|O_NOCTTY|O_CLOEXEC|O_NONBLOCK);
        যদি (tty_fd < 0)
                ফেরত log_debug_errno(tty_fd, "Failed to open %s to query terminfo: %m", tty);

        r = terminal_get_terminfo_by_dcs(tty_fd, &dcs_term);
        যদি (r < 0)
                ফেরত log_debug_errno(r, "Failed to query %s for terminfo: %m", tty);

        r = have_terminfo_file(dcs_term);
        যদি (r < 0)
                ফেরত log_debug_errno(r, "Failed to look for terminfo %s: %m", dcs_term);
        যদি (r == 0)
                ফেরত log_info_errno(SYNTHETIC_ERRNO(ENODATA),
                                      "Terminfo %s not found for %s.", dcs_term, tty);

        *ret_term = TAKE_PTR(dcs_term);
        ফেরত 0;
}

পূর্ণসংখ্যা terminal_is_pty_fd(পূর্ণসংখ্যা fd) {
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(fd >= 0);

        /* Returns true if we are looking at a pty, i.e. if it's backed by the /dev/pts/ file system */

        যদি (!isatty_safe(fd))
                ফেরত false;

        r = is_fs_type_at(fd, ফাঁকা, DEVPTS_SUPER_MAGIC);
        যদি (r != 0)
                ফেরত r;

        /* The ptmx device is weird, it exists twice, once inside and once outside devpts. To detect the
         * latter case, let's fire off an ioctl() that only works on ptmx devices. */

        পূর্ণসংখ্যা v;
        যদি (ioctl(fd, TIOCGPKT, &v) < 0) {
                যদি (ERRNO_IS_NOT_SUPPORTED(errno))
                        ফেরত false;

                ফেরত -errno;
        }

        ফেরত true;
}

পূর্ণসংখ্যা pty_open_peer(পূর্ণসংখ্যা fd, পূর্ণসংখ্যা mode) {
        প্রতিজ্ঞা(fd >= 0);

        /* Opens the peer PTY using the new race-free TIOCGPTPEER ioctl() (kernel 4.13).
         *
         * This is safe to be called on TTYs from other namespaces. */

        প্রতিজ্ঞা((mode & (O_CREAT|O_PATH|O_DIRECTORY|O_TMPFILE)) == 0);

        /* This replicates the EIO retry logic of open_terminal() in a modified way. */
        জন্য (unsigned c = 0;; c++) {
                পূর্ণসংখ্যা peer_fd = ioctl(fd, TIOCGPTPEER, mode);
                যদি (peer_fd >= 0)
                        ফেরত peer_fd;

                যদি (errno != EIO)
                        ফেরত -errno;

                /* Max 1s in total */
                যদি (c >= 20)
                        ফেরত -EIO;

                (শূন্য) usleep_safe(50 * USEC_PER_MSEC);
        }
}
