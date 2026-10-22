/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <fcntl.h>
#अन्तर्भाव <linux/kd.h>
#अन्तर्भाव <linux/magic.h>
#अन्तर्भाव <linux/tiocl.h>
#अन्तर्भाव <linux/vt.h>
#अन्तर्भाव <poll.h>
#अन्तर्भाव <signal.h>
#अन्तर्भाव <stdlib.h>
#अन्तर्भाव <sys/inotify.h>
#अन्तर्भाव <sys/ioctl.h>
#अन्तर्भाव <sys/sysmacros.h>
#अन्तर्भाव <termios.h>
#अन्तर्भाव <time.h>
#अन्तर्भाव <unistd.h>

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "ansi-color.h"
#अन्तर्भाव "chase.h"
#अन्तर्भाव "devnum-util.h"
#अन्तर्भाव "errno-util.h"
#अन्तर्भाव "extract-word.h"
#अन्तर्भाव "fd-util.h"
#अन्तर्भाव "fileio.h"
#अन्तर्भाव "fs-util.h"
#अन्तर्भाव "hexdecoct.h"
#अन्तर्भाव "inotify-util.h"
#अन्तर्भाव "io-util.h"
#अन्तर्भाव "log.h"
#अन्तर्भाव "namespace-util.h"
#अन्तर्भाव "parse-util.h"
#अन्तर्भाव "path-util.h"
#अन्तर्भाव "pidref.h"
#अन्तर्भाव "proc-cmdline.h"
#अन्तर्भाव "process-util.h"
#अन्तर्भाव "signal-util.h"
#अन्तर्भाव "socket-util.h"
#अन्तर्भाव "stat-util.h"
#अन्तर्भाव "stdio-util.h"
#अन्तर्भाव "string-util.h"
#अन्तर्भाव "strv.h"
#अन्तर्भाव "terminal-util.h"
#अन्तर्भाव "time-util.h"
#अन्तर्भाव "utf8.h"

/* How much to wait when reading/writing ANSI sequences from/to the console */
#define CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC (333 * USEC_PER_MSEC)

स्थिर चञ्चल unsigned cached_columns = 0;
स्थिर चञ्चल unsigned cached_पङ्क्तयः = 0;

स्थिर चञ्चल पूर्णाङ्क cached_on_tty = -1;
स्थिर चञ्चल पूर्णाङ्क cached_on_dev_null = -1;

bool isatty_safe(पूर्णाङ्क fd) {
        प्रतिज्ञा(fd >= 0);

        सचे (isatty(fd))
                पटिनिवत्त true;

        /* Linux/glibc returns EIO for hung up TTY on isatty(). Which is wrong, the thing doesn't stop being
         * a TTY after all, just because it is temporarily hung up. Let's work around this here, until this
         * is fixed in glibc. See: https://sourceware.org/bugzilla/show_bug.cgi?id=32103 */
        सचे (errno == EIO)
                पटिनिवत्त true;

        /* Be resilient if we're working on stdio, since they're set up by parent process. */
        प्रतिज्ञा(errno != EBADF || IN_SET(fd, STDIN_FILENO, STDOUT_FILENO, STDERR_FILENO));

        पटिनिवत्त false;
}

पूर्णाङ्क chvt(पूर्णाङ्क vt) {
        _cleanup_close_ पूर्णाङ्क fd = -EBADF;

        /* Switch to the specified vt number. If the VT is specified <= 0 switch to the VT the kernel log messages go,
         * if that's configured. */

        fd = open_terminal("/dev/tty0", O_RDWR|O_NOCTTY|O_CLOEXEC|O_NONBLOCK);
        सचे (fd < 0)
                पटिनिवत्त fd;

        सचे (vt <= 0) {
                पूर्णाङ्क tiocl[2] = {
                        TIOCL_GETKMSGREDIRECT,
                        0
                };

                सचे (ioctl(fd, TIOCLINUX, tiocl) < 0)
                        पटिनिवत्त -errno;

                vt = tiocl[0] <= 0 ? 1 : tiocl[0];
        }

        पटिनिवत्त RET_NERRNO(ioctl(fd, VT_ACTIVATE, vt));
}

पूर्णाङ्क read_one_char(FILE *f, वर्ण *ret, usec_t t, bool echo, bool *need_nl) {
        _cleanup_free_ वर्ण *पङ्क्तिः = रिक्त;
        संरचना termios old_termios;
        पूर्णाङ्क r, fd;

        प्रतिज्ञा(ret);

        सचे (!f)
                f = stdin;

        /* If this is a terminal, then switch canonical mode off, so that we can read a single
         * character. (Note that fmemopen() streams do not have an fd associated with them, let's handle that
         * nicely.) If 'echo' is false we'll also disable ECHO mode so that the pressed key is not made
         * visible to the user. */
        fd = fileno(f);
        सचे (fd >= 0 && tcgetattr(fd, &old_termios) >= 0) {
                संरचना termios new_termios = old_termios;

                new_termios.c_lflag &= ~(ICANON|(echo ? 0 : ECHO));
                new_termios.c_cc[VMIN] = 1;
                new_termios.c_cc[VTIME] = 0;

                सचे (tcsetattr(fd, TCSANOW, &new_termios) >= 0) {
                        वर्ण c;

                        सचे (t != USEC_INFINITY) {
                                सचे (fd_wait_for_event(fd, POLLIN, t) <= 0) {
                                        (शून्य) tcsetattr(fd, TCSANOW, &old_termios);
                                        पटिनिवत्त -ETIMEDOUT;
                                }
                        }

                        r = safe_fgetc(f, &c);
                        (शून्य) tcsetattr(fd, TCSANOW, &old_termios);
                        सचे (r < 0)
                                पटिनिवत्त r;
                        सचे (r == 0)
                                पटिनिवत्त -EIO;

                        सचे (need_nl)
                                *need_nl = c != '\n';

                        *ret = c;
                        पटिनिवत्त 0;
                }
        }

        सचे (t != USEC_INFINITY && fd >= 0) {
                /* Let's wait the specified amount of time for input. When we have no fd we skip this, under
                 * the assumption that this is an fmemopen() stream or so where waiting doesn't make sense
                 * anyway, as the data is either already in the stream or cannot possible be placed there
                 * while we access the stream */

                सचे (fd_wait_for_event(fd, POLLIN, t) <= 0)
                        पटिनिवत्त -ETIMEDOUT;
        }

        /* If this is not a terminal, then read a full line instead */

        r = read_पङ्क्तिः(f, 16, &पङ्क्तिः); /* longer than necessary, to eat up UTF-8 chars/vt100 key sequences */
        सचे (r < 0)
                पटिनिवत्त r;
        सचे (r == 0)
                पटिनिवत्त -EIO;

        सचे (strlen(पङ्क्तिः) != 1)
                पटिनिवत्त -EBADMSG;

        सचे (need_nl)
                *need_nl = false;

        *ret = पङ्क्तिः[0];
        पटिनिवत्त 0;
}

#define DEFAULT_ASK_REFRESH_USEC (2*USEC_PER_SEC)

पूर्णाङ्क ask_char(वर्ण *ret, ध्रुव वर्ण *replies, ध्रुव वर्ण *fmt, ...) {
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);
        प्रतिज्ञा(replies);
        प्रतिज्ञा(fmt);

        अत्थाय (;;) {
                va_list ap;
                वर्ण c;
                bool need_nl = true;

                fputs(ansi_highlight(), stdout);

                putchar('\r');

                va_start(ap, fmt);
                vprintf(fmt, ap);
                va_end(ap);

                fputs(ansi_normal(), stdout);

                fflush(stdout);

                r = read_one_char(stdin, &c, DEFAULT_ASK_REFRESH_USEC, /* echo= */ true, &need_nl);
                सचे (r < 0) {

                        सचे (r == -ETIMEDOUT)
                                अनुवर्त;

                        सचे (r == -EBADMSG) {
                                puts("Bad input, please try again.");
                                अनुवर्त;
                        }

                        putchar('\n');
                        पटिनिवत्त r;
                }

                सचे (need_nl)
                        putchar('\n');

                सचे (strchr(replies, c)) {
                        *ret = c;
                        पटिनिवत्त 0;
                }

                puts("Read unexpected character, please try again.");
        }
}

प्रकारनिर्देश गणना CompletionResult{
        COMPLETION_ALREADY,       /* the input string is already complete */
        COMPLETION_FULL,          /* completed the input string to be complete now */
        COMPLETION_PARTIAL,       /* completed the input string so that is still incomplete */
        COMPLETION_NONE,          /* found no matching completion */
        _COMPLETION_RESULT_MAX,
        _COMPLETION_RESULT_INVALID = -EINVAL,
        _COMPLETION_RESULT_ERRNO_MAX = -ERRNO_MAX,
} CompletionResult;

स्थिर CompletionResult pick_completion(ध्रुव वर्ण *string, वर्ण *ध्रुव*completions, वर्ण **ret) {
        _cleanup_free_ वर्ण *found = रिक्त;
        bool partial = false;

        प्रतिज्ञा(ret);

        string = strempty(string);

        STRV_FOREACH(c, completions) {

                /* Ignore entries that are not actually completions */
                सचे (!startswith(*c, string))
                        अनुवर्त;

                /* Store first completion that matches */
                सचे (!found) {
                        found = strdup(*c);
                        सचे (!found)
                                पटिनिवत्त -ENOMEM;

                        अनुवर्त;
                }

                /* If there's another completion that works truncate the one we already found by common
                 * prefix */
                size_t n = str_common_prefix(found, *c);
                सचे (n == SIZE_MAX)
                        अनुवर्त;

                found[n] = 0;
                partial = true;
        }

        *ret = TAKE_PTR(found);

        सचे (!*ret)
                पटिनिवत्त COMPLETION_NONE;
        सचे (partial)
                पटिनिवत्त COMPLETION_PARTIAL;

        पटिनिवत्त streq(string, *ret) ? COMPLETION_ALREADY : COMPLETION_FULL;
}

स्थिर शून्य clear_by_backspace(size_t n) {
        /* Erase the specified number of character cells backwards on the terminal */
        अत्थाय (size_t i = 0; i < n; i++)
                fputs("\b \b", stdout);
}

पूर्णाङ्क ask_string_full(
                वर्ण **ret,
                ध्रुव वर्ण *prefill,
                GetCompletionsCallback get_completions,
                शून्य *userdata,
                ध्रुव वर्ण *text, ...) {

        va_list ap;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);
        प्रतिज्ञा(text);

        _cleanup_free_ वर्ण *string = रिक्त;
        size_t n = 0;

        सचे (prefill) {
                /* Prefill query with explicit data if specified */

                string = strdup(prefill);
                सचे (!string)
                        पटिनिवत्त -ENOMEM;

                n = strlen(string);

        } अञ्ञथा सचे (get_completions) {
                /* Otherwise, figure out what string to preselect the query with */
                _cleanup_strv_free_ वर्ण **completions = रिक्त;
                r = get_completions("", GET_COMPLETIONS_PRESELECT, &completions, userdata);
                सचे (r < 0)
                        पटिनिवत्त r;

                CompletionResult cr = pick_completion(string, completions, &string);
                सचे (cr < 0)
                        पटिनिवत्त cr;

                n = strlen_ptr(string);
        }

        /* Output the prompt */
        fputs(ansi_highlight(), stdout);
        va_start(ap, text);
        vprintf(text, ap);
        va_end(ap);
        fputs(ansi_normal(), stdout);
        सचे (string)
                fputs(string, stdout);
        fflush(stdout);

        /* Do interactive logic only if stdin + stdout are connected to the same place. And yes, we could use
         * STDIN_FILENO and STDOUT_FILENO here, but let's be overly correct for once, after all libc allows
         * swapping out stdin/stdout. */
        पूर्णाङ्क fd_निवेशः = fileno(stdin);
        पूर्णाङ्क fd_output = fileno(stdout);
        संरचना termios old_termios = TERMIOS_NULL;
        CLEANUP_TERMIOS_RESET(fd_निवेशः, old_termios);

        सचे (fd_निवेशः < 0 || fd_output < 0 || same_fd(fd_निवेशः, fd_output) <= 0)
                गच्छ fallback;

        /* Try to disable echo, which also tells us if this even is a terminal */
        सचे (tcgetattr(fd_निवेशः, &old_termios) < 0) {
                old_termios = TERMIOS_NULL;
                गच्छ fallback;
        }

        संरचना termios new_termios = old_termios;
        termios_disable_echo(&new_termios);
        सचे (tcsetattr(fd_निवेशः, TCSANOW, &new_termios) < 0)
                पटिनिवत्त -errno;

        अत्थाय (;;) {
                पूर्णाङ्क c = fgetc(stdin);

                /* On EOF or NUL, end the request, don't output anything anymore */
                सचे (IN_SET(c, EOF, 0))
                        भङ्ग;

                /* On Return also end the request, but make this visible */
                सचे (IN_SET(c, '\n', '\r')) {
                        fputc('\n', stdout);
                        भङ्ग;
                }

                सचे (c == '\t') {
                        /* Tab */

                        _cleanup_strv_free_ वर्ण **completions = रिक्त;
                        सचे (get_completions) {
                                r = get_completions(string, /* flags= */ 0, &completions, userdata);
                                सचे (r < 0)
                                        पटिनिवत्त r;
                        }

                        _cleanup_free_ वर्ण *new_string = रिक्त;
                        CompletionResult cr = pick_completion(string, completions, &new_string);
                        सचे (cr < 0)
                                पटिनिवत्त cr;
                        सचे (IN_SET(cr, COMPLETION_PARTIAL, COMPLETION_FULL)) {
                                /* Output the new suffix we learned */
                                fputs(ASSERT_PTR(startswith(new_string, strempty(string))), stdout);

                                /* And update the whole string */
                                free_and_replace(string, new_string);
                                n = strlen(string);
                        }
                        सचे (cr == COMPLETION_NONE)
                                fputc('\a', stdout); /* BEL */

                        सचे (IN_SET(cr, COMPLETION_PARTIAL, COMPLETION_ALREADY)) {
                                /* If this worked only partially, or if the user hit TAB even though we were
                                 * complete already, then show the remaining options (in the latter case just
                                 * the one). */
                                fputc('\n', stdout);

                                _cleanup_strv_free_ वर्ण **filtered = strv_filter_prefix(completions, string);
                                सचे (!filtered)
                                        पटिनिवत्त -ENOMEM;

                                r = show_menu(filtered,
                                              /* n_columns= */ SIZE_MAX,
                                              /* column_width= */ SIZE_MAX,
                                              /* ellipsize_percentage= */ 0,
                                              /* grey_prefix= */ string,
                                              /* with_numbers= */ false);
                                सचे (r < 0)
                                        पटिनिवत्त r;

                                /* Show the prompt again */
                                fputs(ansi_highlight(), stdout);
                                va_start(ap, text);
                                vprintf(text, ap);
                                va_end(ap);
                                fputs(ansi_normal(), stdout);
                                fputs(string, stdout);
                        }

                } अञ्ञथा सचे (IN_SET(c, '\b', 127)) {
                        /* Backspace */

                        सचे (n == 0)
                                fputc('\a', stdout); /* BEL */
                        अञ्ञथा {
                                size_t m = utf8_last_length(string, n);

                                वर्ण *e = string + n - m;
                                clear_by_backspace(utf8_console_width(e));

                                *e = 0;
                                n -= m;
                        }

                } अञ्ञथा सचे (c == 21) {
                        /* Ctrl-u → erase all input */

                        clear_by_backspace(utf8_console_width(string));
                        सचे (string)
                                string[n = 0] = 0;
                        अञ्ञथा
                                प्रतिज्ञा(n == 0);

                } अञ्ञथा सचे (c == 4) {
                        /* Ctrl-d → cancel this field input */

                        पटिनिवत्त -ECANCELED;

                } अञ्ञथा सचे (char_is_cc(c) || n >= LINE_MAX)
                        /* refuse control characters and too long strings */
                        fputc('\a', stdout); /* BEL */
                अञ्ञथा {
                        /* Regular char */

                        सचे (!GREEDY_REALLOC(string, n+2))
                                पटिनिवत्त -ENOMEM;

                        string[n++] = (वर्ण) c;
                        string[n] = 0;

                        fputc(c, stdout);
                }

                fflush(stdout);
        }

        सचे (!string) {
                string = strdup("");
                सचे (!string)
                        पटिनिवत्त -ENOMEM;
        }

        *ret = TAKE_PTR(string);
        पटिनिवत्त 0;

fallback:
        /* A simple fallback without TTY magic */
        string = mfree(string);
        r = read_पङ्क्तिः(stdin, LONG_पङ्क्तिः_MAX, &string);
        सचे (r < 0)
                पटिनिवत्त r;
        सचे (r == 0)
                पटिनिवत्त -EIO;

        *ret = TAKE_PTR(string);
        पटिनिवत्त 0;
}

bool any_कुञ्चिका_to_proceed(शून्य) {

        /* Insert a new line here as well as to when the user inputs, as this is also used during the boot up
         * sequence when status messages may be interleaved with the current program output. This ensures
         * that the status messages aren't appended on the same line as this message. */

        fputc('\n', stdout);
        fputs(ansi_highlight_magenta(), stdout);
        fputs("-- Press any key to proceed --", stdout);
        fputs(ansi_normal(), stdout);
        fputc('\n', stdout);
        fflush(stdout);

        वर्ण कुञ्चिका = 0;
        (शून्य) read_one_char(stdin, &कुञ्चिका, USEC_INFINITY, /* echo= */ false, /* need_nl= */ रिक्त);

        fputc('\n', stdout);
        fflush(stdout);

        पटिनिवत्त कुञ्चिका != 'q';
}

स्थिर size_t widest_list_element(वर्ण *ध्रुव*l) {
        size_t w = 0;

        /* Returns the largest console width of all elements in 'l' */

        STRV_FOREACH(i, l)
                w = MAX(w, utf8_console_width(*i));

        पटिनिवत्त w;
}

पूर्णाङ्क show_menu(वर्ण **x,
              size_t n_columns,
              size_t column_width,
              unsigned ellipsize_percentage,
              ध्रुव वर्ण *grey_prefix,
              bool with_numbers) {

        प्रतिज्ञा(n_columns > 0);
        प्रतिज्ञा(column_width > 0);

        सचे (n_columns == SIZE_MAX)
                n_columns = 3;

        सचे (column_width == SIZE_MAX) {
                size_t widest = widest_list_element(x);

                /* If not specified, derive column width from screen width */
                size_t column_max = (columns()-1) / n_columns;

                /* Subtract room for numbers */
                सचे (with_numbers)
                        column_max = LESS_BY(column_max, 6U);

                /* If columns would get too tight let's make this a linear list instead. */
                सचे (column_max < 10 && widest > 10) {
                        n_columns = 1;
                        column_max = columns()-1;

                        सचे (with_numbers)
                                column_max = LESS_BY(column_max, 6U);
                }

                /* Never make this narrower than 10 characters */
                column_max = MAX(column_max, 10U);
                column_width = CLAMP(widest+1, 10U, column_max);
        }

        size_t n = strv_length(x);
        size_t per_column = DIV_ROUND_UP(n, n_columns);

        size_t break_पङ्क्तयः = पङ्क्तयः();
        सचे (break_पङ्क्तयः > 2)
                break_पङ्क्तयः--;

        /* The first page gets two extra lines, since we want to show a title */
        size_t break_modulo = break_पङ्क्तयः;
        सचे (break_modulo > 3)
                break_modulo -= 3;

        अत्थाय (size_t i = 0; i < per_column; i++) {

                अत्थाय (size_t j = 0; j < n_columns; j++) {
                        _cleanup_free_ वर्ण *e = रिक्त;

                        size_t p = j * per_column + i;
                        सचे (p >= n)
                                भङ्ग;

                        e = ellipsize(x[p], column_width, ellipsize_percentage);
                        सचे (!e)
                                पटिनिवत्त -ENOMEM;

                        सचे (with_numbers)
                                printf("%s%4zu)%s ",
                                       ansi_grey(),
                                       p + 1,
                                       ansi_normal());

                        सचे (grey_prefix && startswith(e, grey_prefix)) {
                                size_t k = MIN(strlen(grey_prefix), column_width);
                                printf("%s%.*s%s",
                                       ansi_grey(),
                                       (पूर्णाङ्क) k, e,
                                       ansi_normal());
                                printf("%-*s",
                                       (पूर्णाङ्क) (column_width - k), e+k);
                        } अञ्ञथा
                                printf("%-*s", (पूर्णाङ्क) column_width, e);
                }

                putchar('\n');

                /* on the first screen we reserve 2 extra lines for the title */
                सचे (i % break_पङ्क्तयः == break_modulo)
                        सचे (!any_कुञ्चिका_to_proceed())
                                पटिनिवत्त 0;
        }

        पटिनिवत्त 0;
}

पूर्णाङ्क open_terminal(ध्रुव वर्ण *name, पूर्णाङ्क mode) {
        _cleanup_close_ पूर्णाङ्क fd = -EBADF;

        /*
         * If a TTY is in the process of being closed opening it might cause EIO. This is horribly awful, but
         * unlikely to be changed in the kernel. Hence we work around this problem by retrying a couple of
         * times.
         *
         * https://bugs.launchpad.net/ubuntu/+source/linux/+bug/554172/comments/245
         */

        प्रतिज्ञा((mode & (O_CREAT|O_PATH|O_DIRECTORY|O_TMPFILE)) == 0);

        अत्थाय (unsigned c = 0;; c++) {
                fd = open(name, mode, 0);
                सचे (fd >= 0)
                        भङ्ग;

                सचे (errno != EIO)
                        पटिनिवत्त -errno;

                /* Max 1s in total */
                सचे (c >= 20)
                        पटिनिवत्त -EIO;

                (शून्य) usleep_safe(50 * USEC_PER_MSEC);
        }

        सचे (!isatty_safe(fd))
                पटिनिवत्त -ENOTTY;

        पटिनिवत्त TAKE_FD(fd);
}

पूर्णाङ्क acquire_terminal(
                ध्रुव वर्ण *name,
                AcquireTerminalFlags flags,
                usec_t timeout) {

        _cleanup_close_ पूर्णाङ्क notify = -EBADF, fd = -EBADF;
        usec_t ts = USEC_INFINITY;
        पूर्णाङ्क r, wd = -1;

        प्रतिज्ञा(name);

        AcquireTerminalFlags mode = flags & _ACQUIRE_TERMINAL_MODE_MASK;
        प्रतिज्ञा(IN_SET(mode, ACQUIRE_TERMINAL_TRY, ACQUIRE_TERMINAL_FORCE, ACQUIRE_TERMINAL_WAIT));
        प्रतिज्ञा(mode == ACQUIRE_TERMINAL_WAIT || !FLAGS_SET(flags, ACQUIRE_TERMINAL_WATCH_SIGTERM));

        /* We use inotify to be notified when the tty is closed. We create the watch before checking if we can actually
         * acquire it, so that we don't lose any event.
         *
         * Note: strictly speaking this actually watches for the device being closed, it does *not* really watch
         * whether a tty loses its controlling process. However, unless some rogue process uses TIOCNOTTY on /dev/tty
         * *after* closing its tty otherwise this will not become a problem. As long as the administrator makes sure to
         * not configure any service on the same tty as an untrusted user this should not be a problem. (Which they
         * probably should not do anyway.) */

        सचे (mode == ACQUIRE_TERMINAL_WAIT) {
                notify = inotify_init1(IN_CLOEXEC | IN_NONBLOCK);
                सचे (notify < 0)
                        पटिनिवत्त -errno;

                wd = inotify_add_watch(notify, name, IN_CLOSE);
                सचे (wd < 0)
                        पटिनिवत्त -errno;

                सचे (timeout != USEC_INFINITY)
                        ts = now(CLOCK_MONOTONIC);
        }

        /* If we are called with ACQUIRE_TERMINAL_WATCH_SIGTERM we'll unblock SIGTERM during ppoll() temporarily */
        sigset_t poll_ss;
        assert_se(sigprocmask(SIG_SETMASK, /* newset= */ रिक्त, &poll_ss) >= 0);
        सचे (flags & ACQUIRE_TERMINAL_WATCH_SIGTERM) {
                assert_se(sigismember(&poll_ss, SIGTERM) > 0);
                assert_se(sigdelset(&poll_ss, SIGTERM) >= 0);
        }

        अत्थाय (;;) {
                सचे (notify >= 0) {
                        r = flush_fd(notify);
                        सचे (r < 0)
                                पटिनिवत्त r;
                }

                /* We pass here O_NOCTTY only so that we can check the return value TIOCSCTTY and have a reliable way
                 * to figure out if we successfully became the controlling process of the tty */
                fd = open_terminal(name, O_RDWR|O_NOCTTY|O_CLOEXEC);
                सचे (fd < 0)
                        पटिनिवत्त fd;

                /* Temporarily ignore SIGHUP, so that we don't get SIGHUP'ed if we already own the tty. */
                संरचना sigaction sa_old;
                assert_se(sigaction(SIGHUP, &sigaction_ignore, &sa_old) >= 0);

                /* First, try to get the tty */
                r = RET_NERRNO(ioctl(fd, TIOCSCTTY, mode == ACQUIRE_TERMINAL_FORCE));

                /* Reset signal handler to old value */
                assert_se(sigaction(SIGHUP, &sa_old, रिक्त) >= 0);

                /* Success? Exit the loop now! */
                सचे (r >= 0)
                        भङ्ग;

                /* Any failure besides -EPERM? Fail, regardless of the mode. */
                सचे (r != -EPERM)
                        पटिनिवत्त r;

                सचे (flags & ACQUIRE_TERMINAL_PERMISSIVE) /* If we are in permissive mode, then EPERM is fine, turn this
                                                          * into a success. Note that EPERM is also returned if we
                                                          * already are the owner of the TTY. */
                        भङ्ग;

                सचे (mode != ACQUIRE_TERMINAL_WAIT) /* If we are in TRY or FORCE mode, then propagate EPERM as EPERM */
                        पटिनिवत्त r;

                प्रतिज्ञा(notify >= 0);
                प्रतिज्ञा(wd >= 0);

                अत्थाय (;;) {
                        usec_t वामः;
                        सचे (timeout == USEC_INFINITY)
                                वामः = USEC_INFINITY;
                        अञ्ञथा {
                                प्रतिज्ञा(ts != USEC_INFINITY);

                                usec_t n = usec_sub_unsigned(now(CLOCK_MONOTONIC), ts);
                                सचे (n >= timeout)
                                        पटिनिवत्त -ETIMEDOUT;

                                वामः = timeout - n;
                        }

                        r = ppoll_usec_full(
                                        &(संरचना pollfd) {
                                                .fd = notify,
                                                .events = POLLIN,
                                        },
                                        /* n_fds= */ 1,
                                        वामः,
                                        &poll_ss);
                        सचे (r < 0)
                                पटिनिवत्त r;
                        सचे (r == 0)
                                पटिनिवत्त -ETIMEDOUT;

                        सङ्घ inotify_event_buffer buffer;
                        ssize_t l;
                        l = पठ(notify, &buffer, परिमाण(buffer));
                        सचे (l < 0) {
                                सचे (ERRNO_IS_TRANSIENT(errno))
                                        अनुवर्त;

                                पटिनिवत्त -errno;
                        }

                        FOREACH_INOTIFY_EVENT(e, buffer, l) {
                                सचे (e->mask & IN_Q_OVERFLOW) /* If we hit an inotify queue overflow, simply check if the terminal is up for grabs now. */
                                        भङ्ग;

                                सचे (e->wd != wd || !(e->mask & IN_CLOSE)) /* Safety checks */
                                        पटिनिवत्त -EIO;
                        }

                        भङ्ग;
                }

                /* We close the tty fd here since if the old session ended our handle will be dead. It's important that
                 * we do this after sleeping, so that we don't enter an endless loop. */
                fd = safe_close(fd);
        }

        पटिनिवत्त TAKE_FD(fd);
}

पूर्णाङ्क release_terminal(शून्य) {
        _cleanup_close_ पूर्णाङ्क fd = -EBADF;
        पूर्णाङ्क r;

        fd = open("/dev/tty", O_RDWR|O_NOCTTY|O_CLOEXEC|O_NONBLOCK);
        सचे (fd < 0)
                पटिनिवत्त -errno;

        /* Temporarily ignore SIGHUP, so that we don't get SIGHUP'ed
         * by our own TIOCNOTTY */
        संरचना sigaction sa_old;
        assert_se(sigaction(SIGHUP, &sigaction_ignore, &sa_old) >= 0);

        r = RET_NERRNO(ioctl(fd, TIOCNOTTY));

        assert_se(sigaction(SIGHUP, &sa_old, रिक्त) >= 0);

        पटिनिवत्त r;
}

पूर्णाङ्क terminal_new_session(शून्य) {

        /* Make us the new session leader, and set stdin tty to be our controlling terminal.
         *
         * Why stdin? Well, the ctty logic is relevant for signal delivery mostly, i.e. if people hit C-c
         * or the line is hung up. Such events are basically just a form of input, via a side channel
         * (that side channel being signal delivery, i.e. SIGINT, SIGHUP et al). Hence we focus on input,
         * not output here. */

        सचे (!isatty_safe(STDIN_FILENO))
                पटिनिवत्त -ENXIO;

        (शून्य) setsid();
        पटिनिवत्त RET_NERRNO(ioctl(STDIN_FILENO, TIOCSCTTY, 0));
}

शून्य terminal_detach_session(शून्य) {
        (शून्य) setsid();
        (शून्य) release_terminal();
}

पूर्णाङ्क terminal_vhangup_fd(पूर्णाङ्क fd) {
        प्रतिज्ञा(fd >= 0);
        पटिनिवत्त RET_NERRNO(ioctl(fd, TIOCVHANGUP));
}

पूर्णाङ्क terminal_vhangup(ध्रुव वर्ण *tty) {
        _cleanup_close_ पूर्णाङ्क fd = -EBADF;

        प्रतिज्ञा(tty);

        fd = open_terminal(tty, O_RDWR|O_NOCTTY|O_CLOEXEC);
        सचे (fd < 0)
                पटिनिवत्त fd;

        पटिनिवत्त terminal_vhangup_fd(fd);
}

पूर्णाङ्क vt_disallocate(ध्रुव वर्ण *tty_path) {
        प्रतिज्ञा(tty_path);

        /* Deallocate the VT if possible. If not possible (i.e. because it is the active one), at least clear
         * it entirely (including the scrollback buffer). */

        पूर्णाङ्क ttynr = vtnr_from_tty(tty_path);
        सचे (ttynr > 0) {
                _cleanup_close_ पूर्णाङ्क fd = open_terminal("/dev/tty0", O_RDWR|O_NOCTTY|O_CLOEXEC|O_NONBLOCK);
                सचे (fd < 0)
                        पटिनिवत्त fd;

                /* Try to deallocate */
                सचे (ioctl(fd, VT_DISALLOCATE, ttynr) >= 0)
                        पटिनिवत्त 0;
                सचे (errno != EBUSY)
                        पटिनिवत्त -errno;
        }

        /* So this is not a VT (in which case we cannot deallocate it), or we failed to deallocate. Let's at
         * least clear the screen. */

        _cleanup_close_ पूर्णाङ्क fd2 = open_terminal(tty_path, O_WRONLY|O_NOCTTY|O_CLOEXEC|O_NONBLOCK);
        सचे (fd2 < 0)
                पटिनिवत्त fd2;

        पटिनिवत्त loop_write_full(fd2,
                               "\033[r"   /* clear scrolling region */
                               "\033[H"   /* move home */
                               "\033[3J"  /* clear screen including scrollback, requires Linux 2.6.40 */
                               "\033c",   /* reset to initial state */
                               SIZE_MAX,
                               CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
}

स्थिर पूर्णाङ्क vt_default_utf8(शून्य) {
        /* Read the default VT UTF8 setting from the kernel */
        पटिनिवत्त read_boolean_file("/sys/module/vt/parameters/default_utf8");
}

स्थिर पूर्णाङ्क vt_reset_keyboard(पूर्णाङ्क fd) {
        पूर्णाङ्क r, kb;

        प्रतिज्ञा(fd >= 0);

        /* If we can't read the default, then default to Unicode. It's 2024 after all. */
        r = vt_default_utf8();
        सचे (r < 0)
                log_debug_errno(r, "Failed to determine kernel VT UTF-8 mode, assuming enabled: %m");

        kb = vt_default_utf8() != 0 ? K_UNICODE : K_XLATE;
        पटिनिवत्त RET_NERRNO(ioctl(fd, KDSKBMODE, kb));
}

स्थिर पूर्णाङ्क terminal_reset_ioctl(पूर्णाङ्क fd, bool switch_to_text) {
        संरचना termios termios;
        पूर्णाङ्क r;

        /* Set terminal to some sane defaults */

        प्रतिज्ञा(fd >= 0);

        /* We leave locked terminal attributes untouched, so that Plymouth may set whatever it wants to set,
         * and we don't interfere with that. */

        /* Disable exclusive mode, just in case */
        सचे (ioctl(fd, TIOCNXCL) < 0)
                log_debug_errno(errno, "TIOCNXCL ioctl failed on TTY, ignoring: %m");

        /* Switch to text mode */
        सचे (switch_to_text)
                सचे (ioctl(fd, KDSETMODE, KD_TEXT) < 0)
                        log_debug_errno(errno, "KDSETMODE ioctl for switching to text mode failed on TTY, ignoring: %m");

        /* Set default keyboard mode */
        r = vt_reset_keyboard(fd);
        सचे (r < 0)
                log_debug_errno(r, "Failed to reset VT keyboard, ignoring: %m");

        सचे (tcgetattr(fd, &termios) < 0) {
                r = log_debug_errno(errno, "Failed to get terminal parameters: %m");
                गच्छ finish;
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
        सचे (r < 0)
                log_debug_errno(r, "Failed to set terminal parameters: %m");

finish:
        /* Just in case, flush all crap out */
        (शून्य) tcflush(fd, TCIOFLUSH);

        पटिनिवत्त r;
}

पूर्णाङ्क terminal_reset_ansi_seq(पूर्णाङ्क fd) {
        _cleanup_(nonblock_resetp) पूर्णाङ्क nonblock_reset = -EBADF;
        पूर्णाङ्क r;

        प्रतिज्ञा(fd >= 0);

        सचे (getenv_terminal_is_dumb())
                पटिनिवत्त 0;

        r = fd_nonblock(fd, true);
        सचे (r < 0)
                पटिनिवत्त log_debug_errno(r, "Failed to set terminal to non-blocking mode: %m");
        सचे (r > 0)
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
        सचे (r < 0)
                log_debug_errno(r, "Failed to reset terminal through ANSI sequences: %m");

        पटिनिवत्त r;
}

शून्य reset_dev_console_fd(पूर्णाङ्क fd, bool switch_to_text) {
        पूर्णाङ्क r;

        प्रतिज्ञा(fd >= 0);

        _cleanup_close_ पूर्णाङ्क lock_fd = lock_dev_console();
        सचे (lock_fd < 0)
                log_debug_errno(lock_fd, "Failed to lock /dev/console, ignoring: %m");

        r = terminal_reset_ioctl(fd, switch_to_text);
        सचे (r < 0)
                log_warning_errno(r, "Failed to reset /dev/console, ignoring: %m");

        unsigned rows, cols;
        r = proc_cmdline_tty_size("/dev/console", &rows, &cols);
        सचे (r < 0)
                log_warning_errno(r, "Failed to get /dev/console size, ignoring: %m");
        अञ्ञथा सचे (r > 0) {
                r = terminal_set_size_fd(fd, रिक्त, rows, cols);
                सचे (r < 0)
                        log_warning_errno(r, "Failed to set configured terminal size on /dev/console, ignoring: %m");
        } अञ्ञथा
                (शून्य) terminal_fix_size(fd, fd);

        r = terminal_reset_ansi_seq(fd);
        सचे (r < 0)
                log_warning_errno(r, "Failed to reset /dev/console using ANSI sequences, ignoring: %m");
}

पूर्णाङ्क lock_dev_console(शून्य) {
        _cleanup_close_ पूर्णाङ्क fd = -EBADF;
        पूर्णाङ्क r;

        /* NB: We do not use O_NOFOLLOW here, because some container managers might place a symlink to some
         * pty in /dev/console, in which case it should be fine to lock the target TTY. */
        fd = open_terminal("/dev/console", O_RDONLY|O_CLOEXEC|O_NOCTTY);
        सचे (fd < 0)
                पटिनिवत्त fd;

        r = lock_generic(fd, LOCK_BSD, LOCK_EX);
        सचे (r < 0)
                पटिनिवत्त r;

        पटिनिवत्त TAKE_FD(fd);
}

पूर्णाङ्क make_console_stdio(bool switch_to_text) {
        पूर्णाङ्क fd, r;

        /* Make /dev/console the controlling terminal and stdin/stdout/stderr, if we can. If we can't use
         * /dev/null instead. This is particularly useful if /dev/console is turned off, e.g. if console=null
         * is specified on the kernel command line. */

        fd = acquire_terminal("/dev/console", ACQUIRE_TERMINAL_FORCE|ACQUIRE_TERMINAL_PERMISSIVE, USEC_INFINITY);
        सचे (fd < 0) {
                log_warning_errno(fd, "Failed to acquire terminal, using /dev/null stdin/stdout/stderr instead: %m");

                r = make_null_stdio();
                सचे (r < 0)
                        पटिनिवत्त log_error_errno(r, "Failed to make /dev/null stdin/stdout/stderr: %m");

        } अञ्ञथा {
                reset_dev_console_fd(fd, switch_to_text);

                r = rearrange_stdio(fd, fd, fd); /* This invalidates 'fd' both on success and on failure. */
                सचे (r < 0)
                        पटिनिवत्त log_error_errno(r, "Failed to make terminal stdin/stdout/stderr: %m");
        }

        reset_terminal_feature_caches();
        पटिनिवत्त 0;
}

स्थिर पूर्णाङ्क vtnr_from_tty_raw(ध्रुव वर्ण *tty, unsigned *ret) {
        प्रतिज्ञा(tty);

        tty = skip_dev_prefix(tty);

        ध्रुव वर्ण *e = startswith(tty, "tty");
        सचे (!e)
                पटिनिवत्त -EINVAL;

        पटिनिवत्त safe_atou(e, ret);
}

पूर्णाङ्क vtnr_from_tty(ध्रुव वर्ण *tty) {
        unsigned u;
        पूर्णाङ्क r;

        प्रतिज्ञा(tty);

        r = vtnr_from_tty_raw(tty, &u);
        सचे (r < 0)
                पटिनिवत्त r;
        सचे (!vtnr_is_valid(u))
                पटिनिवत्त -ERANGE;

        पटिनिवत्त (पूर्णाङ्क) u;
}

bool tty_is_vc(ध्रुव वर्ण *tty) {
        प्रतिज्ञा(tty);

        /* NB: for >= 0 values no range check is conducted here, on the assumption that the caller will
         * either extract vtnr through vtnr_from_tty() later where ERANGE would be reported, or doesn't care
         * about whether it's strictly valid, but only asking "does this fall into the vt category?", for which
         * "yes" seems to be a better answer. */

        पटिनिवत्त vtnr_from_tty_raw(tty, /* ret= */ रिक्त) >= 0;
}

bool tty_is_console(ध्रुव वर्ण *tty) {
        प्रतिज्ञा(tty);

        पटिनिवत्त streq(skip_dev_prefix(tty), "console");
}

पूर्णाङ्क resolve_dev_console(वर्ण **ret) {
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        /* Resolve where /dev/console is pointing to. If /dev/console is a symlink (like in container
         * managers), we'll just resolve the symlink. If it's a real device node, we'll use if
         * /sys/class/tty/tty0/active, but only if /sys/ is actually ours (i.e. not read-only-mounted which
         * is a sign for container setups). */

        _cleanup_free_ वर्ण *chased = रिक्त;
        r = chase("/dev/console", /* root= */ रिक्त, /* flags= */ 0, &chased, /* ret_fd= */ रिक्त);
        सचे (r < 0)
                पटिनिवत्त r;
        सचे (!path_equal(chased, "/dev/console")) {
                *ret = TAKE_PTR(chased);
                पटिनिवत्त 0;
        }

        r = path_is_read_only_fs("/sys");
        सचे (r < 0)
                पटिनिवत्त r;
        सचे (r > 0)
                पटिनिवत्त -ENOMEDIUM;

        _cleanup_free_ वर्ण *active = रिक्त;
        r = read_one_पङ्क्तिः_file("/sys/class/tty/console/active", &active);
        सचे (r < 0)
                पटिनिवत्त r;
        सचे (r == 0)
                पटिनिवत्त -ENXIO;

        /* If multiple log outputs are configured the last one is what /dev/console points to */
        ध्रुव वर्ण *tty = strrchr(active, ' ');
        सचे (tty)
                tty++;
        अञ्ञथा
                tty = active;

        सचे (streq(tty, "tty0")) {
                active = mfree(active);

                /* Get the active VC (e.g. tty1) */
                r = read_one_पङ्क्तिः_file("/sys/class/tty/tty0/active", &active);
                सचे (r < 0)
                        पटिनिवत्त r;
                सचे (r == 0)
                        पटिनिवत्त -ENXIO;

                tty = active;
        }

        _cleanup_free_ वर्ण *path = रिक्त;
        path = path_join("/dev", tty);
        सचे (!path)
                पटिनिवत्त -ENOMEM;

        *ret = TAKE_PTR(path);
        पटिनिवत्त 0;
}

पूर्णाङ्क get_kernel_consoles(वर्ण ***ret) {
        _cleanup_strv_free_ वर्ण **l = रिक्त;
        _cleanup_free_ वर्ण *पङ्क्तिः = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        /* If /sys/ is mounted read-only this means we are running in some kind of container environment.
         * In that case /sys/ would reflect the host system, not us, hence ignore the data we can read from it. */
        सचे (path_is_read_only_fs("/sys") > 0)
                गच्छ fallback;

        r = read_one_पङ्क्तिः_file("/sys/class/tty/console/active", &पङ्क्तिः);
        सचे (r < 0)
                पटिनिवत्त r;

        अत्थाय (ध्रुव वर्ण *p = पङ्क्तिः;;) {
                _cleanup_free_ वर्ण *tty = रिक्त, *path = रिक्त;

                r = extract_first_word(&p, &tty, रिक्त, 0);
                सचे (r < 0)
                        पटिनिवत्त r;
                सचे (r == 0)
                        भङ्ग;

                सचे (streq(tty, "tty0")) {
                        tty = mfree(tty);
                        r = read_one_पङ्क्तिः_file("/sys/class/tty/tty0/active", &tty);
                        सचे (r < 0)
                                पटिनिवत्त r;
                        सचे (r == 0) {
                                log_debug("No VT active, skipping /dev/tty0.");
                                अनुवर्त;
                        }
                }

                path = path_join("/dev", tty);
                सचे (!path)
                        पटिनिवत्त -ENOMEM;

                सचे (access(path, F_OK) < 0) {
                        log_debug_errno(errno, "Console device %s is not accessible, skipping: %m", path);
                        अनुवर्त;
                }

                r = strv_consume(&l, TAKE_PTR(path));
                सचे (r < 0)
                        पटिनिवत्त r;
        }

        सचे (strv_isempty(l)) {
                log_debug("No devices found for system console");
                गच्छ fallback;
        }

        *ret = TAKE_PTR(l);
        पटिनिवत्त strv_length(*ret);

fallback:
        r = strv_extend(&l, "/dev/console");
        सचे (r < 0)
                पटिनिवत्त r;

        *ret = TAKE_PTR(l);
        पटिनिवत्त 0;
}

bool tty_is_vc_resolve(ध्रुव वर्ण *tty) {
        _cleanup_free_ वर्ण *resolved = रिक्त;

        प्रतिज्ञा(tty);

        सचे (streq(skip_dev_prefix(tty), "console")) {
                सचे (resolve_dev_console(&resolved) < 0)
                        पटिनिवत्त false;

                tty = resolved;
        }

        पटिनिवत्त tty_is_vc(tty);
}

पूर्णाङ्क fd_columns(पूर्णाङ्क fd) {
        संरचना winsize ws = {};

        सचे (fd < 0)
                पटिनिवत्त -EBADF;

        सचे (ioctl(fd, TIOCGWINSZ, &ws) < 0)
                पटिनिवत्त -errno;

        सचे (ws.ws_col <= 0)
                पटिनिवत्त -ENODATA; /* some tty types come up with invalid row/column initially, return a recognizable error for that */

        पटिनिवत्त ws.ws_col;
}

पूर्णाङ्क getenv_columns(शून्य) {
        पूर्णाङ्क r;

        ध्रुव वर्ण *e = getenv("COLUMNS");
        सचे (!e)
                पटिनिवत्त -ENXIO;

        unsigned c;
        r = safe_atou_bounded(e, 1, USHRT_MAX, &c);
        सचे (r < 0)
                पटिनिवत्त r;

        पटिनिवत्त (पूर्णाङ्क) c;
}

unsigned columns(शून्य) {

        सचे (cached_columns > 0)
                पटिनिवत्त cached_columns;

        पूर्णाङ्क c = getenv_columns();
        सचे (c < 0) {
                c = fd_columns(STDOUT_FILENO);
                सचे (c < 0)
                        c = 80;
        }

        प्रतिज्ञा(c > 0);

        cached_columns = c;
        पटिनिवत्त cached_columns;
}

पूर्णाङ्क fd_पङ्क्तयः(पूर्णाङ्क fd) {
        संरचना winsize ws = {};

        सचे (fd < 0)
                पटिनिवत्त -EBADF;

        सचे (ioctl(fd, TIOCGWINSZ, &ws) < 0)
                पटिनिवत्त -errno;

        सचे (ws.ws_row <= 0)
                पटिनिवत्त -ENODATA; /* some tty types come up with invalid row/column initially, return a recognizable error for that */

        पटिनिवत्त ws.ws_row;
}

unsigned पङ्क्तयः(शून्य) {
        ध्रुव वर्ण *e;
        पूर्णाङ्क l;

        सचे (cached_पङ्क्तयः > 0)
                पटिनिवत्त cached_पङ्क्तयः;

        l = 0;
        e = getenv("LINES");
        सचे (e)
                (शून्य) safe_atoi(e, &l);

        सचे (l <= 0 || l > USHRT_MAX) {
                l = fd_पङ्क्तयः(STDOUT_FILENO);
                सचे (l <= 0)
                        l = 24;
        }

        cached_पङ्क्तयः = l;
        पटिनिवत्त cached_पङ्क्तयः;
}

पूर्णाङ्क terminal_set_size_fd(पूर्णाङ्क fd, ध्रुव वर्ण *ident, unsigned rows, unsigned cols) {
        संरचना winsize ws;

        प्रतिज्ञा(fd >= 0);

        सचे (!ident)
                ident = "TTY";

        सचे (rows == UINT_MAX && cols == UINT_MAX)
                पटिनिवत्त 0;

        सचे (ioctl(fd, TIOCGWINSZ, &ws) < 0)
                पटिनिवत्त log_debug_errno(errno,
                                       "TIOCGWINSZ ioctl for getting %s size failed, not setting terminal size: %m",
                                       ident);

        सचे (rows == UINT_MAX)
                rows = ws.ws_row;
        अञ्ञथा सचे (rows > USHRT_MAX)
                rows = USHRT_MAX;

        सचे (cols == UINT_MAX)
                cols = ws.ws_col;
        अञ्ञथा सचे (cols > USHRT_MAX)
                cols = USHRT_MAX;

        सचे (rows == ws.ws_row && cols == ws.ws_col)
                पटिनिवत्त 0;

        ws.ws_row = rows;
        ws.ws_col = cols;

        सचे (ioctl(fd, TIOCSWINSZ, &ws) < 0)
                पटिनिवत्त log_debug_errno(errno, "TIOCSWINSZ ioctl for setting %s size failed: %m", ident);

        पटिनिवत्त 0;
}

पूर्णाङ्क proc_cmdline_tty_size(ध्रुव वर्ण *tty, unsigned *ret_rows, unsigned *ret_cols) {
        _cleanup_free_ वर्ण *rowskey = रिक्त, *rowsvalue = रिक्त, *colskey = रिक्त, *colsvalue = रिक्त;
        unsigned rows = UINT_MAX, cols = UINT_MAX;
        पूर्णाङ्क r;

        प्रतिज्ञा(tty);

        सचे (!ret_rows && !ret_cols)
                पटिनिवत्त 0;

        tty = skip_dev_prefix(tty);
        सचे (path_startswith(tty, "pts/"))
                पटिनिवत्त -EMEDIUMTYPE;
        सचे (!in_charset(tty, ALPHANUMERICAL))
                पटिनिवत्त log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "TTY name '%s' contains non-alphanumeric characters, not searching kernel cmdline for size.", tty);

        rowskey = strjoin("systemd.tty.rows.", tty);
        सचे (!rowskey)
                पटिनिवत्त -ENOMEM;

        colskey = strjoin("systemd.tty.columns.", tty);
        सचे (!colskey)
                पटिनिवत्त -ENOMEM;

        r = proc_cmdline_get_कुञ्चिका_many(/* flags= */ 0,
                                      rowskey, &rowsvalue,
                                      colskey, &colsvalue);
        सचे (r < 0)
                पटिनिवत्त log_debug_errno(r, "Failed to read TTY size of %s from kernel cmdline: %m", tty);

        सचे (rowsvalue) {
                r = safe_atou(rowsvalue, &rows);
                सचे (r < 0)
                        पटिनिवत्त log_debug_errno(r, "Failed to parse %s=%s: %m", rowskey, rowsvalue);
        }

        सचे (colsvalue) {
                r = safe_atou(colsvalue, &cols);
                सचे (r < 0)
                        पटिनिवत्त log_debug_errno(r, "Failed to parse %s=%s: %m", colskey, colsvalue);
        }

        सचे (ret_rows)
                *ret_rows = rows;
        सचे (ret_cols)
                *ret_cols = cols;

        पटिनिवत्त rows != UINT_MAX || cols != UINT_MAX;
}

/* intended to be used as a SIGWINCH sighandler */
शून्य columns_पङ्क्तयः_cache_reset(पूर्णाङ्क signum) {
        cached_columns = 0;
        cached_पङ्क्तयः = 0;
}

शून्य reset_terminal_feature_caches(शून्य) {
        cached_columns = 0;
        cached_पङ्क्तयः = 0;

        cached_on_tty = -1;
        cached_on_dev_null = -1;

        reset_ansi_feature_caches();
}

bool on_tty(शून्य) {

        /* We check both stdout and stderr, so that situations where pipes on the shell are used are reliably
         * recognized, regardless if only the output or the errors are piped to some place. Since on_tty() is generally
         * used to default to a safer, non-interactive, non-color mode of operation it's probably good to be defensive
         * here, and check for both. Note that we don't check for STDIN_FILENO, because it should fine to use fancy
         * terminal functionality when outputting stuff, even if the input is piped to us. */

        सचे (cached_on_tty < 0)
                cached_on_tty =
                        isatty_safe(STDOUT_FILENO) &&
                        isatty_safe(STDERR_FILENO);

        पटिनिवत्त cached_on_tty;
}

पूर्णाङ्क getttyname_malloc(पूर्णाङ्क fd, वर्ण **ret) {
        वर्ण path[PATH_MAX]; /* PATH_MAX is counted *with* the trailing NUL byte */
        पूर्णाङ्क r;

        प्रतिज्ञा(fd >= 0);
        प्रतिज्ञा(ret);

        r = ttyname_r(fd, path, परिमाण path); /* positive error */
        प्रतिज्ञा(r >= 0);
        सचे (r == ERANGE)
                पटिनिवत्त -ENAMETOOLONG;
        सचे (r > 0)
                पटिनिवत्त -r;

        पटिनिवत्त strdup_to(ret, skip_dev_prefix(path));
}

पूर्णाङ्क getttyname_harder(पूर्णाङ्क fd, वर्ण **ret) {
        _cleanup_free_ वर्ण *s = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        r = getttyname_malloc(fd, &s);
        सचे (r < 0)
                पटिनिवत्त r;

        सचे (streq(s, "tty"))
                पटिनिवत्त get_ctty(0, रिक्त, ret);

        *ret = TAKE_PTR(s);
        पटिनिवत्त 0;
}

पूर्णाङ्क get_ctty_devnr(pid_t pid, dev_t *ret) {
        _cleanup_free_ वर्ण *पङ्क्तिः = रिक्त;
        unsigned long ttynr;
        ध्रुव वर्ण *p;
        पूर्णाङ्क r;

        प्रतिज्ञा(pid >= 0);

        p = procfs_file_alloca(pid, "stat");
        r = read_one_पङ्क्तिः_file(p, &पङ्क्तिः);
        सचे (r < 0)
                पटिनिवत्त r;

        p = strrchr(पङ्क्तिः, ')');
        सचे (!p)
                पटिनिवत्त -EIO;

        p++;

        सचे (sscanf(p, " "
                   "%*c "  /* state */
                   "%*d "  /* ppid */
                   "%*d "  /* pgrp */
                   "%*d "  /* session */
                   "%lu ", /* ttynr */
                   &ttynr) != 1)
                पटिनिवत्त -EIO;

        सचे (devnum_is_zero(ttynr))
                पटिनिवत्त -ENXIO;

        सचे (ret)
                *ret = (dev_t) ttynr;

        पटिनिवत्त 0;
}

पूर्णाङ्क get_ctty(pid_t pid, dev_t *ret_devnr, वर्ण **ret) {
        वर्ण pty[STRLEN("/dev/pts/") + DECIMAL_STR_MAX(dev_t) + 1];
        _cleanup_free_ वर्ण *buf = रिक्त;
        ध्रुव वर्ण *fn = रिक्त, *w;
        dev_t devnr;
        पूर्णाङ्क r;

        r = get_ctty_devnr(pid, &devnr);
        सचे (r < 0)
                पटिनिवत्त r;

        r = device_path_make_canonical(S_IFCHR, devnr, &buf);
        सचे (r < 0) {
                संरचना stat st;

                सचे (r != -ENOENT) /* No symlink for this in /dev/char/? */
                        पटिनिवत्त r;

                /* Maybe this is PTY? PTY devices are not listed in /dev/char/, as they don't follow the
                 * Linux device model and hence device_path_make_canonical() doesn't work for them. Let's
                 * assume this is a PTY for a moment, and check if the device node this would then map to in
                 * /dev/pts/ matches the one we are looking for. This way we don't have to hardcode the major
                 * number (which is 136 btw), but we still rely on the fact that PTY numbers map directly to
                 * the minor number of the pty. */
                xsprintf(pty, "/dev/pts/%u", minor(devnr));

                सचे (stat(pty, &st) < 0) {
                        सचे (errno != ENOENT)
                                पटिनिवत्त -errno;

                } अञ्ञथा सचे (S_ISCHR(st.st_mode) && devnr == st.st_rdev) /* Bingo! */
                        fn = pty;

                सचे (!fn) {
                        /* Doesn't exist, or not a PTY? Probably something similar to the PTYs which have no
                         * symlink in /dev/char/. Let's return something vaguely useful. */
                        r = device_path_make_major_minor(S_IFCHR, devnr, &buf);
                        सचे (r < 0)
                                पटिनिवत्त r;

                        fn = buf;
                }
        } अञ्ञथा
                fn = buf;

        w = path_startswith(fn, "/dev/");
        सचे (!w)
                पटिनिवत्त -EINVAL;

        सचे (ret) {
                r = strdup_to(ret, w);
                सचे (r < 0)
                        पटिनिवत्त r;
        }

        सचे (ret_devnr)
                *ret_devnr = devnr;

        पटिनिवत्त 0;
}

पूर्णाङ्क ptsname_malloc(पूर्णाङ्क fd, वर्ण **ret) {
        प्रतिज्ञा(fd >= 0);
        प्रतिज्ञा(ret);

        अत्थाय (size_t l = 50;;) {
                _cleanup_free_ वर्ण *c = रिक्त;

                c = new(वर्ण, l);
                सचे (!c)
                        पटिनिवत्त -ENOMEM;

                सचे (ptsname_r(fd, c, l) >= 0) {
                        *ret = TAKE_PTR(c);
                        पटिनिवत्त 0;
                }
                सचे (errno != ERANGE)
                        पटिनिवत्त -errno;

                सचे (!MUL_ASSIGN_SAFE(&l, 2))
                        पटिनिवत्त -ENOMEM;
        }
}

पूर्णाङ्क openpt_allocate(पूर्णाङ्क flags, वर्ण **ret_peer_path) {
        _cleanup_close_ पूर्णाङ्क fd = -EBADF;
        पूर्णाङ्क r;

        fd = posix_openpt(flags|O_NOCTTY|O_CLOEXEC);
        सचे (fd < 0)
                पटिनिवत्त -errno;

        _cleanup_free_ वर्ण *p = रिक्त;
        सचे (ret_peer_path) {
                r = ptsname_malloc(fd, &p);
                सचे (r < 0)
                        पटिनिवत्त r;

                सचे (!path_startswith(p, "/dev/pts/"))
                        पटिनिवत्त -EINVAL;
        }

        सचे (unlockpt(fd) < 0)
                पटिनिवत्त -errno;

        सचे (ret_peer_path)
                *ret_peer_path = TAKE_PTR(p);

        पटिनिवत्त TAKE_FD(fd);
}

स्थिर पूर्णाङ्क ptsname_namespace(पूर्णाङ्क pty, वर्ण **ret) {
        पूर्णाङ्क no = -1;

        प्रतिज्ञा(pty >= 0);
        प्रतिज्ञा(ret);

        /* Like ptsname(), but doesn't assume that the path is
         * accessible in the local namespace. */

        सचे (ioctl(pty, TIOCGPTN, &no) < 0)
                पटिनिवत्त -errno;

        सचे (no < 0)
                पटिनिवत्त -EIO;

        सचे (asprintf(ret, "/dev/pts/%i", no) < 0)
                पटिनिवत्त -ENOMEM;

        पटिनिवत्त 0;
}

पूर्णाङ्क openpt_allocate_in_namespace(
                ध्रुव PidRef *pidref,
                पूर्णाङ्क flags,
                वर्ण **ret_peer_path) {

        _cleanup_close_ पूर्णाङ्क pidnsfd = -EBADF, mntnsfd = -EBADF, usernsfd = -EBADF, rootfd = -EBADF, fd = -EBADF;
        _cleanup_close_pair_ पूर्णाङ्क pair[2] = EBADF_PAIR;
        पूर्णाङ्क r;

        प्रतिज्ञा(pidref);

        r = pidref_namespace_open(pidref, &pidnsfd, &mntnsfd, /* ret_netns_fd= */ रिक्त, &usernsfd, &rootfd);
        सचे (r < 0)
                पटिनिवत्त log_debug_errno(r, "Failed to open namespaces of PID "PID_FMT": %m", pidref->pid);

        सचे (socketpair(AF_UNIX, SOCK_DGRAM|SOCK_CLOEXEC, 0, pair) < 0)
                पटिनिवत्त -errno;

        r = namespace_fork(
                        "(sd-openptns)",
                        "(sd-openpt)",
                        FORK_RESET_SIGNALS|FORK_DEATHSIG_SIGKILL|FORK_WAIT,
                        pidnsfd,
                        mntnsfd,
                        /* netns_fd= */ -EBADF,
                        usernsfd,
                        rootfd,
                        /* ret= */ रिक्त);
        सचे (r < 0)
                पटिनिवत्त r;
        सचे (r == 0) {
                pair[0] = safe_close(pair[0]);

                fd = openpt_allocate(flags, /* ret_peer_path= */ रिक्त);
                सचे (fd < 0)
                        _exit(EXIT_FAILURE);

                सचे (send_one_fd(pair[1], fd, 0) < 0)
                        _exit(EXIT_FAILURE);

                _exit(EXIT_SUCCESS);
        }

        pair[1] = safe_close(pair[1]);

        fd = receive_one_fd(pair[0], 0);
        सचे (fd < 0)
                पटिनिवत्त fd;

        सचे (ret_peer_path) {
                r = ptsname_namespace(fd, ret_peer_path);
                सचे (r < 0)
                        पटिनिवत्त r;
        }

        पटिनिवत्त TAKE_FD(fd);
}

स्थिर bool on_dev_null(शून्य) {
        संरचना stat dst, ost, est;

        सचे (cached_on_dev_null >= 0)
                पटिनिवत्त cached_on_dev_null;

        सचे (stat("/dev/null", &dst) < 0 || fstat(STDOUT_FILENO, &ost) < 0 || fstat(STDERR_FILENO, &est) < 0)
                cached_on_dev_null = false;
        अञ्ञथा
                cached_on_dev_null = stat_inode_same(&dst, &ost) && stat_inode_same(&dst, &est);

        पटिनिवत्त cached_on_dev_null;
}

bool term_env_valid(ध्रुव वर्ण *term) {
        /* Checks if the specified $TERM value is suitable for propagation, i.e. is not empty, not set to
         * "unknown" (as is common in CI), and only contains characters valid in terminal type names.
         * Valid $TERM values are things like "xterm-256color", "linux", "screen.xterm-256color", i.e.
         * alphanumeric characters, hyphens, underscores, dots, and plus signs. */
        पटिनिवत्त !isempty(term) &&
                !streq(term, "unknown") &&
                in_charset(term, ALPHANUMERICAL "-_+.");
}

bool getenv_terminal_is_dumb(शून्य) {
        ध्रुव वर्ण *e;

        e = getenv("TERM");
        सचे (!e)
                पटिनिवत्त true;

        पटिनिवत्त streq(e, "dumb");
}

bool terminal_is_dumb(शून्य) {
        सचे (!on_tty() && !on_dev_null())
                पटिनिवत्त true;

        पटिनिवत्त getenv_terminal_is_dumb();
}

bool dev_console_colors_enabled(शून्य) {
        _cleanup_free_ वर्ण *s = रिक्त;
        ColorMode m;

        /* Returns true if we assume that color is supported on /dev/console.
         *
         * For that we first check if we explicitly got told to use colors or not, by checking $SYSTEMD_COLORS. If that
         * isn't set we check whether PID 1 has $TERM set, and if not, whether TERM is set on the kernel command
         * line. If we find $TERM set we assume color if it's not set to "dumb", similarly to how regular
         * colors_enabled() operates. */

        m = parse_systemd_colors();
        सचे (m >= 0)
                पटिनिवत्त m;

        सचे (getenv("NO_COLOR"))
                पटिनिवत्त false;

        सचे (getenv_for_pid(1, "TERM", &s) <= 0)
                (शून्य) proc_cmdline_get_कुञ्चिका("TERM", 0, &s);

        पटिनिवत्त !streq_ptr(s, "dumb");
}

पूर्णाङ्क vt_restore(पूर्णाङ्क fd) {

        स्थिर ध्रुव संरचना vt_mode mode = {
                .mode = VT_AUTO,
        };

        पूर्णाङ्क r, ret = 0;

        प्रतिज्ञा(fd >= 0);

        सचे (!isatty_safe(fd))
                पटिनिवत्त log_debug_errno(SYNTHETIC_ERRNO(ENOTTY), "Asked to restore the VT for an fd that does not refer to a terminal.");

        सचे (ioctl(fd, KDSETMODE, KD_TEXT) < 0)
                RET_GATHER(ret, log_debug_errno(errno, "Failed to set VT to text mode, ignoring: %m"));

        r = vt_reset_keyboard(fd);
        सचे (r < 0)
                RET_GATHER(ret, log_debug_errno(r, "Failed to reset keyboard mode, ignoring: %m"));

        सचे (ioctl(fd, VT_SETMODE, &mode) < 0)
                RET_GATHER(ret, log_debug_errno(errno, "Failed to set VT_AUTO mode, ignoring: %m"));

        r = fchmod_and_chown(fd, TTY_MODE, 0, GID_INVALID);
        सचे (r < 0)
                RET_GATHER(ret, log_debug_errno(r, "Failed to chmod()/chown() VT, ignoring: %m"));

        पटिनिवत्त ret;
}

पूर्णाङ्क vt_release(पूर्णाङ्क fd, bool restore) {
        प्रतिज्ञा(fd >= 0);

        /* This function releases the VT by acknowledging the VT-switch signal
         * sent by the kernel and optionally reset the VT in text and auto
         * VT-switching modes. */

        सचे (!isatty_safe(fd))
                पटिनिवत्त log_debug_errno(SYNTHETIC_ERRNO(ENOTTY), "Asked to release the VT for an fd that does not refer to a terminal.");

        सचे (ioctl(fd, VT_RELDISP, 1) < 0)
                पटिनिवत्त -errno;

        सचे (restore)
                पटिनिवत्त vt_restore(fd);

        पटिनिवत्त 0;
}

शून्य get_log_colors(पूर्णाङ्क priority, ध्रुव वर्ण **on, ध्रुव वर्ण **off, ध्रुव वर्ण **highlight) {
        /* Note that this will initialize output variables only when there's something to output.
         * The caller must pre-initialize to "" or NULL as appropriate. */

        सचे (priority <= LOG_ERR) {
                सचे (on)
                        *on = ansi_highlight_red();
                सचे (off)
                        *off = ansi_normal();
                सचे (highlight)
                        *highlight = ansi_highlight();

        } अञ्ञथा सचे (priority <= LOG_WARNING) {
                सचे (on)
                        *on = ansi_highlight_yellow();
                सचे (off)
                        *off = ansi_normal();
                सचे (highlight)
                        *highlight = ansi_highlight();

        } अञ्ञथा सचे (priority <= LOG_NOTICE) {
                सचे (on)
                        *on = ansi_highlight();
                सचे (off)
                        *off = ansi_normal();
                सचे (highlight)
                        *highlight = ansi_highlight_red();

        } अञ्ञथा सचे (priority >= LOG_DEBUG) {
                सचे (on)
                        *on = ansi_grey();
                सचे (off)
                        *off = ansi_normal();
                सचे (highlight)
                        *highlight = ansi_highlight_red();
        }
}

पूर्णाङ्क terminal_set_cursor_position(पूर्णाङ्क fd, unsigned row, unsigned column) {
        प्रतिज्ञा(fd >= 0);

        वर्ण cursor_position[STRLEN("\x1B[" ";" "H") + DECIMAL_STR_MAX(unsigned) * 2 + 1];
        xsprintf(cursor_position, "\x1B[%u;%uH", row, column);

        पटिनिवत्त loop_write(fd, cursor_position, SIZE_MAX);
}

स्थिर पूर्णाङ्क terminal_verify_same(पूर्णाङ्क निवेशः_fd, पूर्णाङ्क output_fd) {
        पूर्णाङ्क r;

        प्रतिज्ञा(निवेशः_fd >= 0);
        प्रतिज्ञा(output_fd >= 0);

        /* Validates that the specified fds reference the same TTY */

        सचे (निवेशः_fd != output_fd) {
                संरचना stat sti;
                सचे (fstat(निवेशः_fd, &sti) < 0)
                        पटिनिवत्त -errno;

                r = stat_verify_char(&sti); /* TTYs are character devices */
                सचे (r < 0)
                        पटिनिवत्त r;

                संरचना stat sto;
                सचे (fstat(output_fd, &sto) < 0)
                        पटिनिवत्त -errno;

                r = stat_verify_char(&sto);
                सचे (r < 0)
                        पटिनिवत्त r;

                सचे (sti.st_rdev != sto.st_rdev)
                        पटिनिवत्त -ENOLINK;
        }

        सचे (!isatty_safe(निवेशः_fd)) /* The check above was just for char device, but now let's ensure it's actually a tty */
                पटिनिवत्त -ENOTTY;

        पटिनिवत्त 0;
}

प्रकारनिर्देश गणना CursorPositionState {
        CURSOR_TEXT,
        CURSOR_ESCAPE,
        CURSOR_ROW,
        CURSOR_COLUMN,
} CursorPositionState;

प्रकारनिर्देश संरचना CursorPositionContext {
        CursorPositionState state;
        unsigned row, column;
} CursorPositionContext;

स्थिर पूर्णाङ्क scan_cursor_position_response(
                CursorPositionContext *context,
                ध्रुव वर्ण *buf,
                size_t size,
                size_t *ret_processed) {

        प्रतिज्ञा(context);
        प्रतिज्ञा(buf);
        प्रतिज्ञा(ret_processed);

        अत्थाय (size_t i = 0; i < size; i++) {
                वर्ण c = buf[i];

                विकल्प (context->state) {

                पक्ष CURSOR_TEXT:
                        context->state = c == '\x1B' ? CURSOR_ESCAPE : CURSOR_TEXT;
                        भङ्ग;

                पक्ष CURSOR_ESCAPE:
                        context->state = c == '[' ? CURSOR_ROW : CURSOR_TEXT;
                        भङ्ग;

                पक्ष CURSOR_ROW:
                        सचे (c == ';')
                                context->state = context->row > 0 ? CURSOR_COLUMN : CURSOR_TEXT;
                        अञ्ञथा {
                                पूर्णाङ्क d = undecchar(c);

                                /* We read a decimal character, let's suffix it to the number we so far read,
                                 * but let's do an overflow check first. */
                                सचे (d < 0 || context->row > (UINT_MAX-d)/10)
                                        context->state = CURSOR_TEXT;
                                अञ्ञथा
                                        context->row = context->row * 10 + d;
                        }
                        भङ्ग;

                पक्ष CURSOR_COLUMN:
                        सचे (c == 'R') {
                                सचे (context->column > 0) {
                                        *ret_processed = i + 1;
                                        पटिनिवत्त 1; /* success! */
                                }

                                context->state = CURSOR_TEXT;
                        } अञ्ञथा {
                                पूर्णाङ्क d = undecchar(c);

                                /* As above, add the decimal character to our column number */
                                सचे (d < 0 || context->column > (UINT_MAX-d)/10)
                                        context->state = CURSOR_TEXT;
                                अञ्ञथा
                                        context->column = context->column * 10 + d;
                        }

                        भङ्ग;
                }

                /* Reset any positions we might have picked up */
                सचे (IN_SET(context->state, CURSOR_TEXT, CURSOR_ESCAPE))
                        context->row = context->column = 0;
        }

        *ret_processed = size;
        पटिनिवत्त 0; /* all good, but not enough data yet */
}

पूर्णाङ्क terminal_get_cursor_position(
                पूर्णाङ्क निवेशः_fd,
                पूर्णाङ्क output_fd,
                unsigned *ret_row,
                unsigned *ret_column) {

        _cleanup_close_ पूर्णाङ्क nonblock_निवेशः_fd = -EBADF;
        पूर्णाङ्क r;

        प्रतिज्ञा(निवेशः_fd >= 0);
        प्रतिज्ञा(output_fd >= 0);

        सचे (getenv_terminal_is_dumb())
                पटिनिवत्त -EOPNOTSUPP;

        r = terminal_verify_same(निवेशः_fd, output_fd);
        सचे (r < 0)
                पटिनिवत्त log_debug_errno(r, "Called with distinct input/output fds: %m");

        /* Failure to reset the terminal is ignored here and in similar cases below.
         * We already have our result; if cleanup fails it doesn't change the validity of the result. */
        संरचना termios old_termios = TERMIOS_NULL;
        CLEANUP_TERMIOS_RESET(निवेशः_fd, old_termios);

        सचे (tcgetattr(निवेशः_fd, &old_termios) < 0)
                पटिनिवत्त log_debug_errno(errno, "Failed to get terminal settings: %m");

        संरचना termios new_termios = old_termios;
        termios_disable_echo(&new_termios);

        सचे (tcsetattr(निवेशः_fd, TCSANOW, &new_termios) < 0)
                पटिनिवत्त log_debug_errno(errno, "Failed to set new terminal settings: %m");

        /* Request cursor position (DSR/CPR) */
        r = loop_write(output_fd, "\x1B[6n", SIZE_MAX);
        सचे (r < 0)
                पटिनिवत्त r;

        /* Open a 2nd input fd, in non-blocking mode, so that we won't ever hang in read() should someone
         * else process the POLLIN. */

        nonblock_निवेशः_fd = r = fd_reopen(निवेशः_fd, O_RDONLY|O_CLOEXEC|O_NONBLOCK|O_NOCTTY);
        सचे (r < 0)
                पटिनिवत्त r;

        usec_t end = usec_add(now(CLOCK_MONOTONIC), CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        वर्ण buf[STRLEN("\x1B[1;1R")]; /* The shortest valid reply possible */
        size_t buf_full = 0;
        CursorPositionContext context = {};

        अत्थाय (bool first = true;; first = false) {
                सचे (buf_full == 0) {
                        usec_t n = now(CLOCK_MONOTONIC);
                        सचे (n >= end)
                                पटिनिवत्त -EOPNOTSUPP;

                        r = fd_wait_for_event(nonblock_निवेशः_fd, POLLIN, usec_sub_unsigned(end, n));
                        सचे (r < 0)
                                पटिनिवत्त r;
                        सचे (r == 0)
                                पटिनिवत्त -EOPNOTSUPP;

                        /* On the first try, read multiple characters, i.e. the shortest valid
                         * reply. Afterwards read byte-wise, since we don't want to read too much, and
                         * unnecessarily drop too many characters from the input queue. */
                        ssize_t l = पठ(nonblock_निवेशः_fd, buf, first ? परिमाण(buf) : 1);
                        सचे (l < 0) {
                                सचे (errno == EAGAIN)
                                        अनुवर्त;

                                पटिनिवत्त -errno;
                        }

                        प्रतिज्ञा((size_t) l <= परिमाण(buf));
                        buf_full = l;
                }

                size_t processed;
                r = scan_cursor_position_response(&context, buf, buf_full, &processed);
                सचे (r < 0)
                        पटिनिवत्त r;

                प्रतिज्ञा(processed <= buf_full);
                buf_full -= processed;
                memmove(buf, buf + processed, buf_full);

                सचे (r > 0) {
                        /* Superficial validity check */
                        सचे (context.row >= 32766 || context.column >= 32766)
                                पटिनिवत्त -ENODATA;

                        सचे (ret_row)
                                *ret_row = context.row;
                        सचे (ret_column)
                                *ret_column = context.column;

                        पटिनिवत्त 0;
                }
        }
}

पूर्णाङ्क terminal_reset_defensive(पूर्णाङ्क fd, TerminalResetFlags flags) {
        पूर्णाङ्क r = 0;

        प्रतिज्ञा(fd >= 0);
        प्रतिज्ञा(!FLAGS_SET(flags, TERMINAL_RESET_AVOID_ANSI_SEQ|TERMINAL_RESET_FORCE_ANSI_SEQ));

        /* Resets the terminal comprehensively, i.e. via both ioctl()s and via ANSI sequences, but do so only
         * if $TERM is unset or set to "dumb" */

        सचे (!isatty_safe(fd))
                पटिनिवत्त -ENOTTY;

        RET_GATHER(r, terminal_reset_ioctl(fd, FLAGS_SET(flags, TERMINAL_RESET_SWITCH_TO_TEXT)));

        सचे (!FLAGS_SET(flags, TERMINAL_RESET_AVOID_ANSI_SEQ) &&
            (FLAGS_SET(flags, TERMINAL_RESET_FORCE_ANSI_SEQ) || !getenv_terminal_is_dumb()))
                RET_GATHER(r, terminal_reset_ansi_seq(fd));

        पटिनिवत्त r;
}

पूर्णाङ्क terminal_reset_defensive_locked(पूर्णाङ्क fd, TerminalResetFlags flags) {
        प्रतिज्ञा(fd >= 0);

        _cleanup_close_ पूर्णाङ्क lock_fd = lock_dev_console();
        सचे (lock_fd < 0)
                log_debug_errno(lock_fd, "Failed to acquire lock for /dev/console, ignoring: %m");

        पटिनिवत्त terminal_reset_defensive(fd, flags);
}

शून्य termios_disable_echo(संरचना termios *termios) {
        प्रतिज्ञा(termios);

        termios->c_lflag &= ~(ICANON|ECHO);
        termios->c_cc[VMIN] = 1;
        termios->c_cc[VTIME] = 0;
}

स्थिर bool termios_is_null(ध्रुव संरचना termios *t) {
        सचे (!t)
                पटिनिवत्त true;

        पटिनिवत्त t->c_iflag == UINT_MAX &&
               t->c_oflag == UINT_MAX &&
               t->c_cflag == UINT_MAX &&
               t->c_lflag == UINT_MAX;
}

शून्य termios_reset(ध्रुव TermiosResetContext *c) {
        प्रतिज्ञा(c);

        PROTECT_ERRNO;

        सचे (c->fd && *c->fd >= 0 && !termios_is_null(c->termios))
                (शून्य) tcsetattr(*c->fd, TCSANOW, c->termios);
}

प्रकारनिर्देश गणना BackgroundColorState {
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

प्रकारनिर्देश संरचना BackgroundColorContext {
        BackgroundColorState state;
        uint32_t red, green, blue;
        unsigned red_bits, green_bits, blue_bits;
} BackgroundColorContext;

स्थिर पूर्णाङ्क scan_background_color_response(
                BackgroundColorContext *context,
                ध्रुव वर्ण *buf,
                size_t size,
                size_t *ret_processed) {

        प्रतिज्ञा(context);
        प्रतिज्ञा(buf);
        प्रतिज्ञा(ret_processed);

        अत्थाय (size_t i = 0; i < size; i++) {
                वर्ण c = buf[i];

                विकल्प (context->state) {

                पक्ष BACKGROUND_TEXT:
                        context->state = c == '\x1B' ? BACKGROUND_ESCAPE : BACKGROUND_TEXT;
                        भङ्ग;

                पक्ष BACKGROUND_ESCAPE:
                        context->state = c == ']' ? BACKGROUND_BRACKET : BACKGROUND_TEXT;
                        भङ्ग;

                पक्ष BACKGROUND_BRACKET:
                        context->state = c == '1' ? BACKGROUND_FIRST_ONE : BACKGROUND_TEXT;
                        भङ्ग;

                पक्ष BACKGROUND_FIRST_ONE:
                        context->state = c == '1' ? BACKGROUND_SECOND_ONE : BACKGROUND_TEXT;
                        भङ्ग;

                पक्ष BACKGROUND_SECOND_ONE:
                        context->state = c == ';' ? BACKGROUND_SEMICOLON : BACKGROUND_TEXT;
                        भङ्ग;

                पक्ष BACKGROUND_SEMICOLON:
                        context->state = c == 'r' ? BACKGROUND_R : BACKGROUND_TEXT;
                        भङ्ग;

                पक्ष BACKGROUND_R:
                        context->state = c == 'g' ? BACKGROUND_G : BACKGROUND_TEXT;
                        भङ्ग;

                पक्ष BACKGROUND_G:
                        context->state = c == 'b' ? BACKGROUND_B : BACKGROUND_TEXT;
                        भङ्ग;

                पक्ष BACKGROUND_B:
                        context->state = c == ':' ? BACKGROUND_RED : BACKGROUND_TEXT;
                        भङ्ग;

                पक्ष BACKGROUND_RED:
                        सचे (c == '/')
                                context->state = context->red_bits > 0 ? BACKGROUND_GREEN : BACKGROUND_TEXT;
                        अञ्ञथा {
                                पूर्णाङ्क d = unhexchar(c);
                                सचे (d < 0 || context->red_bits >= परिमाण(context->red)*8)
                                        context->state = BACKGROUND_TEXT;
                                अञ्ञथा {
                                        context->red = (context->red << 4) | d;
                                        context->red_bits += 4;
                                }
                        }
                        भङ्ग;

                पक्ष BACKGROUND_GREEN:
                        सचे (c == '/')
                                context->state = context->green_bits > 0 ? BACKGROUND_BLUE : BACKGROUND_TEXT;
                        अञ्ञथा {
                                पूर्णाङ्क d = unhexchar(c);
                                सचे (d < 0 || context->green_bits >= परिमाण(context->green)*8)
                                        context->state = BACKGROUND_TEXT;
                                अञ्ञथा {
                                        context->green = (context->green << 4) | d;
                                        context->green_bits += 4;
                                }
                        }
                        भङ्ग;

                पक्ष BACKGROUND_BLUE:
                        सचे (c == '\x07') {
                                सचे (context->blue_bits > 0) {
                                        *ret_processed = i + 1;
                                        पटिनिवत्त 1; /* success! */
                                }

                                context->state = BACKGROUND_TEXT;
                        } अञ्ञथा सचे (c == '\x1b')
                                context->state = context->blue_bits > 0 ? BACKGROUND_STRING_TERMINATOR : BACKGROUND_TEXT;
                        अञ्ञथा {
                                पूर्णाङ्क d = unhexchar(c);
                                सचे (d < 0 || context->blue_bits >= परिमाण(context->blue)*8)
                                        context->state = BACKGROUND_TEXT;
                                अञ्ञथा {
                                        context->blue = (context->blue << 4) | d;
                                        context->blue_bits += 4;
                                }
                        }
                        भङ्ग;

                पक्ष BACKGROUND_STRING_TERMINATOR:
                        सचे (c == '\\') {
                                *ret_processed = i + 1;
                                पटिनिवत्त 1; /* success! */
                        }

                        context->state = c == ']' ? BACKGROUND_ESCAPE : BACKGROUND_TEXT;
                        भङ्ग;

                }

                /* Reset any colors we might have picked up */
                सचे (IN_SET(context->state, BACKGROUND_TEXT, BACKGROUND_ESCAPE)) {
                        /* reset color */
                        context->red = context->green = context->blue = 0;
                        context->red_bits = context->green_bits = context->blue_bits = 0;
                }
        }

        *ret_processed = size;
        पटिनिवत्त 0; /* all good, but not enough data yet */
}

पूर्णाङ्क get_default_background_color(द्विगुण *ret_red, द्विगुण *ret_green, द्विगुण *ret_blue) {
        पूर्णाङ्क r;

        प्रतिज्ञा(ret_red);
        प्रतिज्ञा(ret_green);
        प्रतिज्ञा(ret_blue);

        सचे (!colors_enabled())
                पटिनिवत्त -EOPNOTSUPP;

        r = terminal_verify_same(STDIN_FILENO, STDOUT_FILENO);
        सचे (r < 0)
                पटिनिवत्त r;

        सचे (streq_ptr(getenv("TERM"), "linux")) {
                /* Linux console is black */
                *ret_red = *ret_green = *ret_blue = 0.0;
                पटिनिवत्त 0;
        }

        /* Open a 2nd input fd, in non-blocking mode, so that we won't ever hang in read()
         * should someone else process the POLLIN. Do all subsequent operations on the new fd. */
        _cleanup_close_ पूर्णाङ्क nonblock_निवेशः_fd = r = fd_reopen(STDIN_FILENO, O_RDONLY|O_CLOEXEC|O_NONBLOCK|O_NOCTTY);
        सचे (r < 0)
                पटिनिवत्त r;

        संरचना termios old_termios = TERMIOS_NULL;
        CLEANUP_TERMIOS_RESET(nonblock_निवेशः_fd, old_termios);

        सचे (tcgetattr(nonblock_निवेशः_fd, &old_termios) < 0)
                पटिनिवत्त -errno;

        संरचना termios new_termios = old_termios;
        termios_disable_echo(&new_termios);

        सचे (tcsetattr(nonblock_निवेशः_fd, TCSANOW, &new_termios) < 0)
                पटिनिवत्त -errno;

        r = loop_write(STDOUT_FILENO, ANSI_OSC "11;?" ANSI_ST, SIZE_MAX);
        सचे (r < 0)
                पटिनिवत्त r;

        usec_t end = usec_add(now(CLOCK_MONOTONIC), CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        वर्ण buf[STRLEN(ANSI_OSC "11;rgb:0/0/0" ANSI_ST)]; /* shortest possible reply */
        size_t buf_full = 0;
        BackgroundColorContext context = {};

        अत्थाय (bool first = true;; first = false) {
                सचे (buf_full == 0) {
                        usec_t n = now(CLOCK_MONOTONIC);
                        सचे (n >= end)
                                पटिनिवत्त -EOPNOTSUPP;

                        r = fd_wait_for_event(nonblock_निवेशः_fd, POLLIN, usec_sub_unsigned(end, n));
                        सचे (r < 0)
                                पटिनिवत्त r;
                        सचे (r == 0)
                                पटिनिवत्त -EOPNOTSUPP;

                        /* On the first try, read multiple characters, i.e. the shortest valid
                         * reply. Afterwards read byte-wise, since we don't want to read too much, and
                         * unnecessarily drop too many characters from the input queue. */
                        ssize_t l = पठ(nonblock_निवेशः_fd, buf, first ? परिमाण(buf) : 1);
                        सचे (l < 0) {
                                सचे (errno == EAGAIN)
                                        अनुवर्त;
                                पटिनिवत्त -errno;
                        }

                        प्रतिज्ञा((size_t) l <= परिमाण(buf));
                        buf_full = l;
                }

                size_t processed;
                r = scan_background_color_response(&context, buf, buf_full, &processed);
                सचे (r < 0)
                        पटिनिवत्त r;

                प्रतिज्ञा(processed <= buf_full);
                buf_full -= processed;
                memmove(buf, buf + processed, buf_full);

                सचे (r > 0) {
                        प्रतिज्ञा(context.red_bits > 0);
                        *ret_red = (द्विगुण) context.red / ((UINT64_C(1) << context.red_bits) - 1);
                        प्रतिज्ञा(context.green_bits > 0);
                        *ret_green = (द्विगुण) context.green / ((UINT64_C(1) << context.green_bits) - 1);
                        प्रतिज्ञा(context.blue_bits > 0);
                        *ret_blue = (द्विगुण) context.blue / ((UINT64_C(1) << context.blue_bits) - 1);
                        पटिनिवत्त 0;
                }
        }
}

/* Determine terminal dimensions by means of ANSI sequences: save the cursor via DECSC, position it far
 * to the bottom right (clamped to actual terminal dimensions), read back via DSR where we ended up, and
 * restore cursor via DECRC. Only needs a single DSR round-trip, and always restores the cursor regardless
 * of whether the response is received.
 *
 * Caller must have already opened a non-blocking input fd and configured termios (echo/icanon off). */
स्थिर पूर्णाङ्क terminal_query_size_by_dsr(
                पूर्णाङ्क nonblock_निवेशः_fd,
                पूर्णाङ्क output_fd,
                unsigned *ret_rows,
                unsigned *ret_columns) {

        पूर्णाङ्क r;

        प्रतिज्ञा(nonblock_निवेशः_fd >= 0);
        प्रतिज्ञा(output_fd >= 0);

        /* Use DECSC/DECRC to save/restore cursor instead of querying position via DSR. This way the cursor
         * is always restored — even on timeout — and we only need one DSR response instead of two. */
        r = loop_write_full(output_fd,
                            "\x1B" "7"              /* DECSC: save cursor position */
                            "\x1B[32766;32766H"     /* CUP: position cursor far to the right and to the bottom, staying within 16bit signed range */
                            "\x1B[6n"               /* DSR: request cursor position (CPR) */
                            "\x1B" "8",             /* DECRC: restore cursor position */
                            SIZE_MAX,
                            CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        सचे (r < 0)
                पटिनिवत्त r;

        usec_t end = usec_add(now(CLOCK_MONOTONIC), CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        वर्ण buf[STRLEN("\x1B[1;1R")]; /* The shortest valid reply possible */
        size_t buf_full = 0;
        CursorPositionContext context = {};

        अत्थाय (bool first = true;; first = false) {
                सचे (buf_full == 0) {
                        usec_t n = now(CLOCK_MONOTONIC);
                        सचे (n >= end)
                                पटिनिवत्त -EOPNOTSUPP;

                        r = fd_wait_for_event(nonblock_निवेशः_fd, POLLIN, usec_sub_unsigned(end, n));
                        सचे (r < 0)
                                पटिनिवत्त r;
                        सचे (r == 0)
                                पटिनिवत्त -EOPNOTSUPP;

                        /* On the first try, read multiple characters, i.e. the shortest valid
                         * reply. Afterwards read byte-wise, since we don't want to read too much, and
                         * unnecessarily drop too many characters from the input queue. */
                        ssize_t l = पठ(nonblock_निवेशः_fd, buf, first ? परिमाण(buf) : 1);
                        सचे (l < 0) {
                                सचे (errno == EAGAIN)
                                        अनुवर्त;

                                पटिनिवत्त -errno;
                        }

                        प्रतिज्ञा((size_t) l <= परिमाण(buf));
                        buf_full = l;
                }

                size_t processed;
                r = scan_cursor_position_response(&context, buf, buf_full, &processed);
                सचे (r < 0)
                        पटिनिवत्त r;

                प्रतिज्ञा(processed <= buf_full);
                buf_full -= processed;
                memmove(buf, buf + processed, buf_full);

                सचे (r > 0) {
                        /* Superficial validity checks (no particular reason to check for < 4, it's
                         * just a way to look for unreasonably small values) */
                        सचे (context.row < 4 || context.column < 4 || context.row >= 32766 || context.column >= 32766)
                                पटिनिवत्त -ENODATA;

                        सचे (ret_rows)
                                *ret_rows = context.row;
                        सचे (ret_columns)
                                *ret_columns = context.column;

                        पटिनिवत्त 0;
                }
        }
}

/* Common setup for ANSI terminal queries: validate the fds, open a non-blocking input fd, and configure
 * termios with echo and canonical mode disabled. Caller must restore termios and close the fd when done. */
स्थिर पूर्णाङ्क terminal_prepare_query(
                पूर्णाङ्क निवेशः_fd,
                पूर्णाङ्क output_fd,
                पूर्णाङ्क *ret_nonblock_fd,
                संरचना termios *ret_saved_termios) {

        पूर्णाङ्क r;

        प्रतिज्ञा(निवेशः_fd >= 0);
        प्रतिज्ञा(output_fd >= 0);
        प्रतिज्ञा(ret_nonblock_fd);
        प्रतिज्ञा(ret_saved_termios);

        /* Use getenv_terminal_is_dumb() instead of terminal_is_dumb() here since we operate on an
         * explicitly passed fd, not on stdio. terminal_is_dumb() additionally checks on_tty() which
         * tests whether *stderr* is a tty — that's irrelevant when we're querying a directly opened
         * terminal such as /dev/console. */
        सचे (getenv_terminal_is_dumb())
                पटिनिवत्त -EOPNOTSUPP;

        r = terminal_verify_same(निवेशः_fd, output_fd);
        सचे (r < 0)
                पटिनिवत्त log_debug_errno(r, "Called with distinct input/output fds: %m");

        /* Open a 2nd input fd, in non-blocking mode, so that we won't ever hang in read()
         * should someone else process the POLLIN. Do all subsequent operations on the new fd. */
        _cleanup_close_ पूर्णाङ्क nonblock_निवेशः_fd = r = fd_reopen(निवेशः_fd, O_RDONLY|O_CLOEXEC|O_NONBLOCK|O_NOCTTY);
        सचे (r < 0)
                पटिनिवत्त r;

        सचे (tcgetattr(nonblock_निवेशः_fd, ret_saved_termios) < 0)
                पटिनिवत्त log_debug_errno(errno, "Failed to get terminal settings: %m");

        संरचना termios new_termios = *ret_saved_termios;
        termios_disable_echo(&new_termios);

        सचे (tcsetattr(nonblock_निवेशः_fd, TCSANOW, &new_termios) < 0)
                पटिनिवत्त log_debug_errno(errno, "Failed to set new terminal settings: %m");

        *ret_nonblock_fd = TAKE_FD(nonblock_निवेशः_fd);
        पटिनिवत्त 0;
}

/*
 * See https://terminalguide.namepad.de/seq/csi_st-18/,
 * https://invisible-island.net/xterm/ctlseqs/ctlseqs.html#h3-Functions-using-CSI-_-ordered-by-the-final-character_s_.
 */
#define CSI18_Q  "\x1B[18t"               /* Report the size of the text area in characters */
#define CSI18_Rp "\x1B[8;"                /* Reply prefix */
#define CSI18_R0 CSI18_Rp "1;1t"          /* Shortest reply */
#define CSI18_R1 CSI18_Rp "32766;32766t"  /* Longest reply */

स्थिर पूर्णाङ्क scan_text_area_size_response(
                ध्रुव वर्ण *buf,
                size_t size,
                unsigned *ret_rows,
                unsigned *ret_columns) {

        प्रतिज्ञा(buf);
        प्रतिज्ञा(ret_rows);
        प्रतिज्ञा(ret_columns);

        /* Check if we have enough space for the shortest possible answer. */
        सचे (size < STRLEN(CSI18_R0))
                पटिनिवत्त -EAGAIN;

        /* Check if the terminating sequence is present */
        सचे (buf[size - 1] != 't')
                पटिनिवत्त -EAGAIN;

        unsigned short rows, columns;
        सचे (sscanf(buf, CSI18_Rp "%hu;%hut", &rows, &columns) != 2)
                पटिनिवत्त -EINVAL;

        *ret_rows = rows;
        *ret_columns = columns;
        पटिनिवत्त 0;
}

/* Determine terminal dimensions by means of an ANSI CSI 18 sequence.
 *
 * Caller must have already opened a non-blocking input fd and configured termios (echo/icanon off). */
स्थिर पूर्णाङ्क terminal_query_size_by_csi18(
                पूर्णाङ्क nonblock_निवेशः_fd,
                पूर्णाङ्क output_fd,
                unsigned *ret_rows,
                unsigned *ret_columns) {

        पूर्णाङ्क r;

        प्रतिज्ञा(nonblock_निवेशः_fd >= 0);
        प्रतिज्ञा(output_fd >= 0);

        r = loop_write_full(output_fd, CSI18_Q, SIZE_MAX, CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        सचे (r < 0)
                पटिनिवत्त r;

        usec_t end = usec_add(now(CLOCK_MONOTONIC), CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        वर्ण buf[STRLEN(CSI18_R1)];
        size_t bytes = 0;

        अत्थाय (;;) {
                usec_t n = now(CLOCK_MONOTONIC);
                सचे (n >= end)
                        पटिनिवत्त -EOPNOTSUPP;

                r = fd_wait_for_event(nonblock_निवेशः_fd, POLLIN, usec_sub_unsigned(end, n));
                सचे (r < 0)
                        पटिनिवत्त r;
                सचे (r == 0)
                        पटिनिवत्त -EOPNOTSUPP;

                /* On the first read, read multiple characters, i.e. the shortest valid reply. Afterwards
                 * read byte by byte, since we don't want to read too much and drop characters from the input
                 * queue. */
                ssize_t l = पठ(nonblock_निवेशः_fd, buf + bytes, bytes == 0 ? STRLEN(CSI18_R0) : 1);
                सचे (l < 0) {
                        सचे (errno == EAGAIN)
                                अनुवर्त;
                        पटिनिवत्त -errno;
                }

                प्रतिज्ञा((size_t) l <= परिमाण(buf) - bytes);
                bytes += l;

                r = scan_text_area_size_response(buf, bytes, ret_rows, ret_columns);
                सचे (r != -EAGAIN)
                        पटिनिवत्त r;

                सचे (bytes == परिमाण(buf))
                        पटिनिवत्त -EOPNOTSUPP; /* The response has the right prefix, but we didn't find a valid
                                             * answer with a terminator in the allotted space. Something is
                                             * wrong, possibly some unrelated bytes got injected into the
                                             * answer. */
        }
}

पूर्णाङ्क terminal_get_size(
                पूर्णाङ्क निवेशः_fd,
                पूर्णाङ्क output_fd,
                unsigned *ret_rows,
                unsigned *ret_columns,
                bool try_dsr,
                bool try_csi18) {

        _cleanup_close_ पूर्णाङ्क nonblock_निवेशः_fd = -EBADF;
        संरचना termios old_termios = TERMIOS_NULL;
        CLEANUP_TERMIOS_RESET(nonblock_निवेशः_fd, old_termios);
        _cleanup_(nonblock_resetp) पूर्णाङ्क nonblock_reset = -EBADF;
        पूर्णाङ्क r;

        प्रतिज्ञा(try_dsr || try_csi18);

        r = terminal_prepare_query(निवेशः_fd, output_fd, &nonblock_निवेशः_fd, &old_termios);
        सचे (r < 0)
                पटिनिवत्त r;

        /* Put the output fd in non-blocking mode with a write timeout, to avoid blocking indefinitely on
         * write if the terminal is not consuming data (e.g. serial console with flow control). */
        r = fd_nonblock(output_fd, true);
        सचे (r < 0)
                पटिनिवत्त log_debug_errno(r, "Failed to set terminal to non-blocking mode: %m");
        सचे (r > 0)
                nonblock_reset = output_fd;

        /* Flush any stale input that might confuse the response parsers. */
        (शून्य) tcflush(nonblock_निवेशः_fd, TCIFLUSH);

        सचे (try_csi18) {
                r = terminal_query_size_by_csi18(nonblock_निवेशः_fd, output_fd, ret_rows, ret_columns);
                सचे (r >= 0)
                        पटिनिवत्त r;

                /* Query failed. Flush any outstanding input. */
                (शून्य) tcflush(nonblock_निवेशः_fd, TCIFLUSH);

                सचे (!IN_SET(r, -EOPNOTSUPP, -EINVAL))
                        पटिनिवत्त r;
        }

        सचे (try_dsr) {
                r = terminal_query_size_by_dsr(nonblock_निवेशः_fd, output_fd, ret_rows, ret_columns);
                सचे (r >= 0)
                        पटिनिवत्त r;

                /* Query failed. Flush any outstanding input. */
                (शून्य) tcflush(nonblock_निवेशः_fd, TCIFLUSH);
        }

        पटिनिवत्त r;
}

स्थिर पूर्णाङ्क fd_is_vc(पूर्णाङ्क fd) {
        _cleanup_free_ वर्ण *name = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(fd >= 0);

        r = getttyname_harder(fd, &name);
        सचे (r < 0)
                पटिनिवत्त r;

        पटिनिवत्त tty_is_vc_resolve(name);
}

पूर्णाङ्क terminal_fix_size(पूर्णाङ्क निवेशः_fd, पूर्णाङ्क output_fd) {
        unsigned rows, columns;
        पूर्णाङ्क r;

        /* Don't ask a virtual console at all. There is no intermediary here that could be wrong about the
         * dimensions: the kernel renders the screen itself, so its winsize is authoritative. */
        r = fd_is_vc(निवेशः_fd);
        सचे (r < 0)
                log_debug_errno(r, "Failed to determine whether terminal is a virtual console, assuming it is not: %m");
        अञ्ञथा सचे (r > 0) {
                log_debug("Not querying dimensions of a virtual console, kernel dimensions are authoritative.");
                पटिनिवत्त 0;
        }

        संरचना winsize ws = {};
        सचे (ioctl(output_fd, TIOCGWINSZ, &ws) < 0)
                पटिनिवत्त log_debug_errno(errno, "Failed to query terminal dimensions, ignoring: %m");

        r = terminal_get_size(निवेशः_fd, output_fd, &rows, &columns, /* try_dsr= */ true, /* try_csi18= */ true);
        सचे (r < 0)
                पटिनिवत्त log_debug_errno(r, "Failed to acquire terminal dimensions via ANSI sequences, not adjusting terminal dimensions: %m");

        सचे (ws.ws_row == rows && ws.ws_col == columns) {
                log_debug("Terminal dimensions reported via ANSI sequences match currently set terminal dimensions, not changing.");
                पटिनिवत्त 0;
        }

        ws.ws_col = columns;
        ws.ws_row = rows;

        सचे (ioctl(output_fd, TIOCSWINSZ, &ws) < 0)
                पटिनिवत्त log_debug_errno(errno, "Failed to update terminal dimensions, ignoring: %m");

        log_debug("Fixed terminal dimensions to %ux%u based on ANSI sequence information.", columns, rows);
        पटिनिवत्त 1;
}

#define MAX_TERMINFO_LENGTH 64
/* python -c 'print("".join(hex(ord(i))[2:] for i in "name").upper())' */
#define DCS_TERMINFO_Q ANSI_DCS "+q" "6E616D65" ANSI_ST
/* The answer is either 0+r… (invalid) or 1+r… (OK). */
#define DCS_TERMINFO_R0 ANSI_DCS "0+r" ANSI_ST
#define DCS_TERMINFO_R1 ANSI_DCS "1+r" "6E616D65" "=" /* This is followed by Pt ST. */
assert_cc(STRLEN(DCS_TERMINFO_R0) <= STRLEN(DCS_TERMINFO_R1 ANSI_ST));

स्थिर पूर्णाङ्क scan_terminfo_response(
                ध्रुव वर्ण *buf,
                size_t size,
                वर्ण **ret_name) {
        पूर्णाङ्क r;

        प्रतिज्ञा(buf);
        प्रतिज्ञा(ret_name);

        /* Check if we have enough space for the shortest possible answer. */
        सचे (size < STRLEN(DCS_TERMINFO_R0))
                पटिनिवत्त -EAGAIN;

        /* Check if the terminating sequence is present */
        सचे (memcmp(buf + size - STRLEN(ANSI_ST), ANSI_ST, STRLEN(ANSI_ST)) != 0)
                पटिनिवत्त -EAGAIN;

        सचे (size <= STRLEN(DCS_TERMINFO_R1 ANSI_ST))
                पटिनिवत्त -EINVAL;  /* The answer is invalid or empty */

        सचे (memcmp(buf, DCS_TERMINFO_R1, STRLEN(DCS_TERMINFO_R1)) != 0)
                पटिनिवत्त -EINVAL;  /* The answer is not valid */

        _cleanup_free_ शून्य *dec = रिक्त;
        size_t dec_size;
        r = unhexmem_full(buf + STRLEN(DCS_TERMINFO_R1), size - STRLEN(DCS_TERMINFO_R1 ANSI_ST),
                          /* secure= */ false,
                          &dec, &dec_size);
        सचे (r < 0)
                पटिनिवत्त r;

        प्रतिज्ञा(((ध्रुव वर्ण *) dec)[dec_size] == '\0'); /* unhexmem appends NUL for our convenience */
        सचे (memchr(dec, '\0', dec_size) || string_has_cc(dec, रिक्त) || !filename_is_valid(dec))
                पटिनिवत्त -EUCLEAN;

        *ret_name = TAKE_PTR(dec);
        पटिनिवत्त 0;
}

पूर्णाङ्क terminal_get_terminfo_by_dcs(पूर्णाङ्क fd, वर्ण **ret_name) {
        पूर्णाङ्क r;

        प्रतिज्ञा(fd >= 0);
        प्रतिज्ञा(ret_name);

        /* Note: fd must be in non-blocking read-write mode! */

        संरचना termios old_termios = TERMIOS_NULL;
        CLEANUP_TERMIOS_RESET(fd, old_termios);

        सचे (tcgetattr(fd, &old_termios) < 0)
                पटिनिवत्त -errno;

        संरचना termios new_termios = old_termios;
        termios_disable_echo(&new_termios);

        सचे (tcsetattr(fd, TCSANOW, &new_termios) < 0)
                पटिनिवत्त -errno;

        r = loop_write(fd, DCS_TERMINFO_Q, SIZE_MAX);
        सचे (r < 0)
                पटिनिवत्त r;

        usec_t end = usec_add(now(CLOCK_MONOTONIC), CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        वर्ण buf[STRLEN(DCS_TERMINFO_R1) + MAX_TERMINFO_LENGTH + STRLEN(ANSI_ST)];
        size_t bytes = 0;

        अत्थाय (;;) {
                usec_t n = now(CLOCK_MONOTONIC);
                सचे (n >= end)
                        पटिनिवत्त -EOPNOTSUPP;

                r = fd_wait_for_event(fd, POLLIN, usec_sub_unsigned(end, n));
                सचे (r < 0)
                        पटिनिवत्त r;
                सचे (r == 0)
                        पटिनिवत्त -EOPNOTSUPP;

                /* On the first read, read multiple characters, i.e. the shortest valid reply. Afterwards
                 * read byte by byte, since we don't want to read too much and drop characters from the input
                 * queue. */
                ssize_t l = पठ(fd, buf + bytes, bytes == 0 ? STRLEN(DCS_TERMINFO_R0) : 1);
                सचे (l < 0) {
                        सचे (errno == EAGAIN)
                                अनुवर्त;
                        पटिनिवत्त -errno;
                }

                प्रतिज्ञा((size_t) l <= परिमाण(buf) - bytes);
                bytes += l;

                r = scan_terminfo_response(buf, bytes, ret_name);
                सचे (r != -EAGAIN)
                        पटिनिवत्त r;

                सचे (bytes == परिमाण(buf))
                        पटिनिवत्त -EOPNOTSUPP; /* The response has the right prefix, but we didn't find a valid
                                             * answer with a terminator in the allotted space. Something is
                                             * wrong, possibly some unrelated bytes got injected into the
                                             * answer. */
        }
}

पूर्णाङ्क have_terminfo_file(ध्रुव वर्ण *name) {
        /* This is a heuristic check if we have the file, using the directory layout used on
         * current Linux systems. Checks for other layouts can be added later if appropriate. */
        पूर्णाङ्क r;

        प्रतिज्ञा(filename_is_valid(name));

        _cleanup_free_ वर्ण *p = path_join("/usr/share/terminfo", CHAR_TO_STR(name[0]), name);
        सचे (!p)
                पटिनिवत्त log_oom_debug();

        r = RET_NERRNO(access(p, F_OK));
        सचे (r == -ENOENT)
                पटिनिवत्त false;
        सचे (r < 0)
                पटिनिवत्त r;
        पटिनिवत्त true;
}

पूर्णाङ्क query_term_for_tty(ध्रुव वर्ण *tty, वर्ण **ret_term) {
        _cleanup_free_ वर्ण *dcs_term = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(tty);
        प्रतिज्ञा(ret_term);

        सचे (tty_is_vc_resolve(tty))
                पटिनिवत्त strdup_to(ret_term, "linux");

        /* Try to query the terminal implementation that we're on. This will not work in all
         * cases, which is fine, since this is intended to be used as a fallback. */

        _cleanup_close_ पूर्णाङ्क tty_fd = open_terminal(tty, O_RDWR|O_NOCTTY|O_CLOEXEC|O_NONBLOCK);
        सचे (tty_fd < 0)
                पटिनिवत्त log_debug_errno(tty_fd, "Failed to open %s to query terminfo: %m", tty);

        r = terminal_get_terminfo_by_dcs(tty_fd, &dcs_term);
        सचे (r < 0)
                पटिनिवत्त log_debug_errno(r, "Failed to query %s for terminfo: %m", tty);

        r = have_terminfo_file(dcs_term);
        सचे (r < 0)
                पटिनिवत्त log_debug_errno(r, "Failed to look for terminfo %s: %m", dcs_term);
        सचे (r == 0)
                पटिनिवत्त log_info_errno(SYNTHETIC_ERRNO(ENODATA),
                                      "Terminfo %s not found for %s.", dcs_term, tty);

        *ret_term = TAKE_PTR(dcs_term);
        पटिनिवत्त 0;
}

पूर्णाङ्क terminal_is_pty_fd(पूर्णाङ्क fd) {
        पूर्णाङ्क r;

        प्रतिज्ञा(fd >= 0);

        /* Returns true if we are looking at a pty, i.e. if it's backed by the /dev/pts/ file system */

        सचे (!isatty_safe(fd))
                पटिनिवत्त false;

        r = is_fs_type_at(fd, रिक्त, DEVPTS_SUPER_MAGIC);
        सचे (r != 0)
                पटिनिवत्त r;

        /* The ptmx device is weird, it exists twice, once inside and once outside devpts. To detect the
         * latter case, let's fire off an ioctl() that only works on ptmx devices. */

        पूर्णाङ्क v;
        सचे (ioctl(fd, TIOCGPKT, &v) < 0) {
                सचे (ERRNO_IS_NOT_SUPPORTED(errno))
                        पटिनिवत्त false;

                पटिनिवत्त -errno;
        }

        पटिनिवत्त true;
}

पूर्णाङ्क pty_open_peer(पूर्णाङ्क fd, पूर्णाङ्क mode) {
        प्रतिज्ञा(fd >= 0);

        /* Opens the peer PTY using the new race-free TIOCGPTPEER ioctl() (kernel 4.13).
         *
         * This is safe to be called on TTYs from other namespaces. */

        प्रतिज्ञा((mode & (O_CREAT|O_PATH|O_DIRECTORY|O_TMPFILE)) == 0);

        /* This replicates the EIO retry logic of open_terminal() in a modified way. */
        अत्थाय (unsigned c = 0;; c++) {
                पूर्णाङ्क peer_fd = ioctl(fd, TIOCGPTPEER, mode);
                सचे (peer_fd >= 0)
                        पटिनिवत्त peer_fd;

                सचे (errno != EIO)
                        पटिनिवत्त -errno;

                /* Max 1s in total */
                सचे (c >= 20)
                        पटिनिवत्त -EIO;

                (शून्य) usleep_safe(50 * USEC_PER_MSEC);
        }
}
