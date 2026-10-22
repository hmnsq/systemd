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
स्थिर चञ्चल unsigned cached_पंक्तियाँ = 0;

स्थिर चञ्चल पूर्णाङ्क cached_on_tty = -1;
स्थिर चञ्चल पूर्णाङ्क cached_on_dev_null = -1;

bool isatty_safe(पूर्णाङ्क fd) {
        प्रतिज्ञा(fd >= 0);

        यदि (isatty(fd))
                प्रत्यागम true;

        /* Linux/glibc returns EIO for hung up TTY on isatty(). Which is wrong, the thing doesn't stop being
         * a TTY after all, just because it is temporarily hung up. Let's work around this here, until this
         * is fixed in glibc. See: https://sourceware.org/bugzilla/show_bug.cgi?id=32103 */
        यदि (errno == EIO)
                प्रत्यागम true;

        /* Be resilient if we're working on stdio, since they're set up by parent process. */
        प्रतिज्ञा(errno != EBADF || IN_SET(fd, STDIN_FILENO, STDOUT_FILENO, STDERR_FILENO));

        प्रत्यागम false;
}

पूर्णाङ्क chvt(पूर्णाङ्क vt) {
        _cleanup_close_ पूर्णाङ्क fd = -EBADF;

        /* Switch to the specified vt number. If the VT is specified <= 0 switch to the VT the kernel log messages go,
         * if that's configured. */

        fd = open_terminal("/dev/tty0", O_RDWR|O_NOCTTY|O_CLOEXEC|O_NONBLOCK);
        यदि (fd < 0)
                प्रत्यागम fd;

        यदि (vt <= 0) {
                पूर्णाङ्क tiocl[2] = {
                        TIOCL_GETKMSGREDIRECT,
                        0
                };

                यदि (ioctl(fd, TIOCLINUX, tiocl) < 0)
                        प्रत्यागम -errno;

                vt = tiocl[0] <= 0 ? 1 : tiocl[0];
        }

        प्रत्यागम RET_NERRNO(ioctl(fd, VT_ACTIVATE, vt));
}

पूर्णाङ्क read_one_char(FILE *f, वर्ण *ret, usec_t t, bool echo, bool *need_nl) {
        _cleanup_free_ वर्ण *पंक्ति = रिक्त;
        संरचना termios old_termios;
        पूर्णाङ्क r, fd;

        प्रतिज्ञा(ret);

        यदि (!f)
                f = stdin;

        /* If this is a terminal, then switch canonical mode off, so that we can read a single
         * character. (Note that fmemopen() streams do not have an fd associated with them, let's handle that
         * nicely.) If 'echo' is false we'll also disable ECHO mode so that the pressed key is not made
         * visible to the user. */
        fd = fileno(f);
        यदि (fd >= 0 && tcgetattr(fd, &old_termios) >= 0) {
                संरचना termios new_termios = old_termios;

                new_termios.c_lflag &= ~(ICANON|(echo ? 0 : ECHO));
                new_termios.c_cc[VMIN] = 1;
                new_termios.c_cc[VTIME] = 0;

                यदि (tcsetattr(fd, TCSANOW, &new_termios) >= 0) {
                        वर्ण c;

                        यदि (t != USEC_INFINITY) {
                                यदि (fd_wait_for_event(fd, POLLIN, t) <= 0) {
                                        (शून्य) tcsetattr(fd, TCSANOW, &old_termios);
                                        प्रत्यागम -ETIMEDOUT;
                                }
                        }

                        r = safe_fgetc(f, &c);
                        (शून्य) tcsetattr(fd, TCSANOW, &old_termios);
                        यदि (r < 0)
                                प्रत्यागम r;
                        यदि (r == 0)
                                प्रत्यागम -EIO;

                        यदि (need_nl)
                                *need_nl = c != '\n';

                        *ret = c;
                        प्रत्यागम 0;
                }
        }

        यदि (t != USEC_INFINITY && fd >= 0) {
                /* Let's wait the specified amount of time for input. When we have no fd we skip this, under
                 * the assumption that this is an fmemopen() stream or so where waiting doesn't make sense
                 * anyway, as the data is either already in the stream or cannot possible be placed there
                 * while we access the stream */

                यदि (fd_wait_for_event(fd, POLLIN, t) <= 0)
                        प्रत्यागम -ETIMEDOUT;
        }

        /* If this is not a terminal, then read a full line instead */

        r = read_पंक्ति(f, 16, &पंक्ति); /* longer than necessary, to eat up UTF-8 chars/vt100 key sequences */
        यदि (r < 0)
                प्रत्यागम r;
        यदि (r == 0)
                प्रत्यागम -EIO;

        यदि (strlen(पंक्ति) != 1)
                प्रत्यागम -EBADMSG;

        यदि (need_nl)
                *need_nl = false;

        *ret = पंक्ति[0];
        प्रत्यागम 0;
}

#define DEFAULT_ASK_REFRESH_USEC (2*USEC_PER_SEC)

पूर्णाङ्क ask_char(वर्ण *ret, ध्रुव वर्ण *replies, ध्रुव वर्ण *fmt, ...) {
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);
        प्रतिज्ञा(replies);
        प्रतिज्ञा(fmt);

        प्रति (;;) {
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
                यदि (r < 0) {

                        यदि (r == -ETIMEDOUT)
                                अनुवर्त;

                        यदि (r == -EBADMSG) {
                                puts("Bad input, please try again.");
                                अनुवर्त;
                        }

                        putchar('\n');
                        प्रत्यागम r;
                }

                यदि (need_nl)
                        putchar('\n');

                यदि (strchr(replies, c)) {
                        *ret = c;
                        प्रत्यागम 0;
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
                यदि (!startswith(*c, string))
                        अनुवर्त;

                /* Store first completion that matches */
                यदि (!found) {
                        found = strdup(*c);
                        यदि (!found)
                                प्रत्यागम -ENOMEM;

                        अनुवर्त;
                }

                /* If there's another completion that works truncate the one we already found by common
                 * prefix */
                size_t n = str_common_prefix(found, *c);
                यदि (n == SIZE_MAX)
                        अनुवर्त;

                found[n] = 0;
                partial = true;
        }

        *ret = TAKE_PTR(found);

        यदि (!*ret)
                प्रत्यागम COMPLETION_NONE;
        यदि (partial)
                प्रत्यागम COMPLETION_PARTIAL;

        प्रत्यागम streq(string, *ret) ? COMPLETION_ALREADY : COMPLETION_FULL;
}

स्थिर शून्य clear_by_backspace(size_t n) {
        /* Erase the specified number of character cells backwards on the terminal */
        प्रति (size_t i = 0; i < n; i++)
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

        यदि (prefill) {
                /* Prefill query with explicit data if specified */

                string = strdup(prefill);
                यदि (!string)
                        प्रत्यागम -ENOMEM;

                n = strlen(string);

        } अन्यथा यदि (get_completions) {
                /* Otherwise, figure out what string to preselect the query with */
                _cleanup_strv_free_ वर्ण **completions = रिक्त;
                r = get_completions("", GET_COMPLETIONS_PRESELECT, &completions, userdata);
                यदि (r < 0)
                        प्रत्यागम r;

                CompletionResult cr = pick_completion(string, completions, &string);
                यदि (cr < 0)
                        प्रत्यागम cr;

                n = strlen_ptr(string);
        }

        /* Output the prompt */
        fputs(ansi_highlight(), stdout);
        va_start(ap, text);
        vprintf(text, ap);
        va_end(ap);
        fputs(ansi_normal(), stdout);
        यदि (string)
                fputs(string, stdout);
        fflush(stdout);

        /* Do interactive logic only if stdin + stdout are connected to the same place. And yes, we could use
         * STDIN_FILENO and STDOUT_FILENO here, but let's be overly correct for once, after all libc allows
         * swapping out stdin/stdout. */
        पूर्णाङ्क fd_निवेश = fileno(stdin);
        पूर्णाङ्क fd_output = fileno(stdout);
        संरचना termios old_termios = TERMIOS_NULL;
        CLEANUP_TERMIOS_RESET(fd_निवेश, old_termios);

        यदि (fd_निवेश < 0 || fd_output < 0 || same_fd(fd_निवेश, fd_output) <= 0)
                गच्छ fallback;

        /* Try to disable echo, which also tells us if this even is a terminal */
        यदि (tcgetattr(fd_निवेश, &old_termios) < 0) {
                old_termios = TERMIOS_NULL;
                गच्छ fallback;
        }

        संरचना termios new_termios = old_termios;
        termios_disable_echo(&new_termios);
        यदि (tcsetattr(fd_निवेश, TCSANOW, &new_termios) < 0)
                प्रत्यागम -errno;

        प्रति (;;) {
                पूर्णाङ्क c = fgetc(stdin);

                /* On EOF or NUL, end the request, don't output anything anymore */
                यदि (IN_SET(c, EOF, 0))
                        भङ्ग;

                /* On Return also end the request, but make this visible */
                यदि (IN_SET(c, '\n', '\r')) {
                        fputc('\n', stdout);
                        भङ्ग;
                }

                यदि (c == '\t') {
                        /* Tab */

                        _cleanup_strv_free_ वर्ण **completions = रिक्त;
                        यदि (get_completions) {
                                r = get_completions(string, /* flags= */ 0, &completions, userdata);
                                यदि (r < 0)
                                        प्रत्यागम r;
                        }

                        _cleanup_free_ वर्ण *new_string = रिक्त;
                        CompletionResult cr = pick_completion(string, completions, &new_string);
                        यदि (cr < 0)
                                प्रत्यागम cr;
                        यदि (IN_SET(cr, COMPLETION_PARTIAL, COMPLETION_FULL)) {
                                /* Output the new suffix we learned */
                                fputs(ASSERT_PTR(startswith(new_string, strempty(string))), stdout);

                                /* And update the whole string */
                                free_and_replace(string, new_string);
                                n = strlen(string);
                        }
                        यदि (cr == COMPLETION_NONE)
                                fputc('\a', stdout); /* BEL */

                        यदि (IN_SET(cr, COMPLETION_PARTIAL, COMPLETION_ALREADY)) {
                                /* If this worked only partially, or if the user hit TAB even though we were
                                 * complete already, then show the remaining options (in the latter case just
                                 * the one). */
                                fputc('\n', stdout);

                                _cleanup_strv_free_ वर्ण **filtered = strv_filter_prefix(completions, string);
                                यदि (!filtered)
                                        प्रत्यागम -ENOMEM;

                                r = show_menu(filtered,
                                              /* n_columns= */ SIZE_MAX,
                                              /* column_width= */ SIZE_MAX,
                                              /* ellipsize_percentage= */ 0,
                                              /* grey_prefix= */ string,
                                              /* with_numbers= */ false);
                                यदि (r < 0)
                                        प्रत्यागम r;

                                /* Show the prompt again */
                                fputs(ansi_highlight(), stdout);
                                va_start(ap, text);
                                vprintf(text, ap);
                                va_end(ap);
                                fputs(ansi_normal(), stdout);
                                fputs(string, stdout);
                        }

                } अन्यथा यदि (IN_SET(c, '\b', 127)) {
                        /* Backspace */

                        यदि (n == 0)
                                fputc('\a', stdout); /* BEL */
                        अन्यथा {
                                size_t m = utf8_last_length(string, n);

                                वर्ण *e = string + n - m;
                                clear_by_backspace(utf8_console_width(e));

                                *e = 0;
                                n -= m;
                        }

                } अन्यथा यदि (c == 21) {
                        /* Ctrl-u → erase all input */

                        clear_by_backspace(utf8_console_width(string));
                        यदि (string)
                                string[n = 0] = 0;
                        अन्यथा
                                प्रतिज्ञा(n == 0);

                } अन्यथा यदि (c == 4) {
                        /* Ctrl-d → cancel this field input */

                        प्रत्यागम -ECANCELED;

                } अन्यथा यदि (char_is_cc(c) || n >= LINE_MAX)
                        /* refuse control characters and too long strings */
                        fputc('\a', stdout); /* BEL */
                अन्यथा {
                        /* Regular char */

                        यदि (!GREEDY_REALLOC(string, n+2))
                                प्रत्यागम -ENOMEM;

                        string[n++] = (वर्ण) c;
                        string[n] = 0;

                        fputc(c, stdout);
                }

                fflush(stdout);
        }

        यदि (!string) {
                string = strdup("");
                यदि (!string)
                        प्रत्यागम -ENOMEM;
        }

        *ret = TAKE_PTR(string);
        प्रत्यागम 0;

fallback:
        /* A simple fallback without TTY magic */
        string = mfree(string);
        r = read_पंक्ति(stdin, LONG_पंक्ति_MAX, &string);
        यदि (r < 0)
                प्रत्यागम r;
        यदि (r == 0)
                प्रत्यागम -EIO;

        *ret = TAKE_PTR(string);
        प्रत्यागम 0;
}

bool any_कुंजी_to_proceed(शून्य) {

        /* Insert a new line here as well as to when the user inputs, as this is also used during the boot up
         * sequence when status messages may be interleaved with the current program output. This ensures
         * that the status messages aren't appended on the same line as this message. */

        fputc('\n', stdout);
        fputs(ansi_highlight_magenta(), stdout);
        fputs("-- Press any key to proceed --", stdout);
        fputs(ansi_normal(), stdout);
        fputc('\n', stdout);
        fflush(stdout);

        वर्ण कुंजी = 0;
        (शून्य) read_one_char(stdin, &कुंजी, USEC_INFINITY, /* echo= */ false, /* need_nl= */ रिक्त);

        fputc('\n', stdout);
        fflush(stdout);

        प्रत्यागम कुंजी != 'q';
}

स्थिर size_t widest_list_element(वर्ण *ध्रुव*l) {
        size_t w = 0;

        /* Returns the largest console width of all elements in 'l' */

        STRV_FOREACH(i, l)
                w = MAX(w, utf8_console_width(*i));

        प्रत्यागम w;
}

पूर्णाङ्क show_menu(वर्ण **x,
              size_t n_columns,
              size_t column_width,
              unsigned ellipsize_percentage,
              ध्रुव वर्ण *grey_prefix,
              bool with_numbers) {

        प्रतिज्ञा(n_columns > 0);
        प्रतिज्ञा(column_width > 0);

        यदि (n_columns == SIZE_MAX)
                n_columns = 3;

        यदि (column_width == SIZE_MAX) {
                size_t widest = widest_list_element(x);

                /* If not specified, derive column width from screen width */
                size_t column_max = (columns()-1) / n_columns;

                /* Subtract room for numbers */
                यदि (with_numbers)
                        column_max = LESS_BY(column_max, 6U);

                /* If columns would get too tight let's make this a linear list instead. */
                यदि (column_max < 10 && widest > 10) {
                        n_columns = 1;
                        column_max = columns()-1;

                        यदि (with_numbers)
                                column_max = LESS_BY(column_max, 6U);
                }

                /* Never make this narrower than 10 characters */
                column_max = MAX(column_max, 10U);
                column_width = CLAMP(widest+1, 10U, column_max);
        }

        size_t n = strv_length(x);
        size_t per_column = DIV_ROUND_UP(n, n_columns);

        size_t break_पंक्तियाँ = पंक्तियाँ();
        यदि (break_पंक्तियाँ > 2)
                break_पंक्तियाँ--;

        /* The first page gets two extra lines, since we want to show a title */
        size_t break_modulo = break_पंक्तियाँ;
        यदि (break_modulo > 3)
                break_modulo -= 3;

        प्रति (size_t i = 0; i < per_column; i++) {

                प्रति (size_t j = 0; j < n_columns; j++) {
                        _cleanup_free_ वर्ण *e = रिक्त;

                        size_t p = j * per_column + i;
                        यदि (p >= n)
                                भङ्ग;

                        e = ellipsize(x[p], column_width, ellipsize_percentage);
                        यदि (!e)
                                प्रत्यागम -ENOMEM;

                        यदि (with_numbers)
                                printf("%s%4zu)%s ",
                                       ansi_grey(),
                                       p + 1,
                                       ansi_normal());

                        यदि (grey_prefix && startswith(e, grey_prefix)) {
                                size_t k = MIN(strlen(grey_prefix), column_width);
                                printf("%s%.*s%s",
                                       ansi_grey(),
                                       (पूर्णाङ्क) k, e,
                                       ansi_normal());
                                printf("%-*s",
                                       (पूर्णाङ्क) (column_width - k), e+k);
                        } अन्यथा
                                printf("%-*s", (पूर्णाङ्क) column_width, e);
                }

                putchar('\n');

                /* on the first screen we reserve 2 extra lines for the title */
                यदि (i % break_पंक्तियाँ == break_modulo)
                        यदि (!any_कुंजी_to_proceed())
                                प्रत्यागम 0;
        }

        प्रत्यागम 0;
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

        प्रति (unsigned c = 0;; c++) {
                fd = open(name, mode, 0);
                यदि (fd >= 0)
                        भङ्ग;

                यदि (errno != EIO)
                        प्रत्यागम -errno;

                /* Max 1s in total */
                यदि (c >= 20)
                        प्रत्यागम -EIO;

                (शून्य) usleep_safe(50 * USEC_PER_MSEC);
        }

        यदि (!isatty_safe(fd))
                प्रत्यागम -ENOTTY;

        प्रत्यागम TAKE_FD(fd);
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

        यदि (mode == ACQUIRE_TERMINAL_WAIT) {
                notify = inotify_init1(IN_CLOEXEC | IN_NONBLOCK);
                यदि (notify < 0)
                        प्रत्यागम -errno;

                wd = inotify_add_watch(notify, name, IN_CLOSE);
                यदि (wd < 0)
                        प्रत्यागम -errno;

                यदि (timeout != USEC_INFINITY)
                        ts = now(CLOCK_MONOTONIC);
        }

        /* If we are called with ACQUIRE_TERMINAL_WATCH_SIGTERM we'll unblock SIGTERM during ppoll() temporarily */
        sigset_t poll_ss;
        assert_se(sigprocmask(SIG_SETMASK, /* newset= */ रिक्त, &poll_ss) >= 0);
        यदि (flags & ACQUIRE_TERMINAL_WATCH_SIGTERM) {
                assert_se(sigismember(&poll_ss, SIGTERM) > 0);
                assert_se(sigdelset(&poll_ss, SIGTERM) >= 0);
        }

        प्रति (;;) {
                यदि (notify >= 0) {
                        r = flush_fd(notify);
                        यदि (r < 0)
                                प्रत्यागम r;
                }

                /* We pass here O_NOCTTY only so that we can check the return value TIOCSCTTY and have a reliable way
                 * to figure out if we successfully became the controlling process of the tty */
                fd = open_terminal(name, O_RDWR|O_NOCTTY|O_CLOEXEC);
                यदि (fd < 0)
                        प्रत्यागम fd;

                /* Temporarily ignore SIGHUP, so that we don't get SIGHUP'ed if we already own the tty. */
                संरचना sigaction sa_old;
                assert_se(sigaction(SIGHUP, &sigaction_ignore, &sa_old) >= 0);

                /* First, try to get the tty */
                r = RET_NERRNO(ioctl(fd, TIOCSCTTY, mode == ACQUIRE_TERMINAL_FORCE));

                /* Reset signal handler to old value */
                assert_se(sigaction(SIGHUP, &sa_old, रिक्त) >= 0);

                /* Success? Exit the loop now! */
                यदि (r >= 0)
                        भङ्ग;

                /* Any failure besides -EPERM? Fail, regardless of the mode. */
                यदि (r != -EPERM)
                        प्रत्यागम r;

                यदि (flags & ACQUIRE_TERMINAL_PERMISSIVE) /* If we are in permissive mode, then EPERM is fine, turn this
                                                          * into a success. Note that EPERM is also returned if we
                                                          * already are the owner of the TTY. */
                        भङ्ग;

                यदि (mode != ACQUIRE_TERMINAL_WAIT) /* If we are in TRY or FORCE mode, then propagate EPERM as EPERM */
                        प्रत्यागम r;

                प्रतिज्ञा(notify >= 0);
                प्रतिज्ञा(wd >= 0);

                प्रति (;;) {
                        usec_t बायाँ;
                        यदि (timeout == USEC_INFINITY)
                                बायाँ = USEC_INFINITY;
                        अन्यथा {
                                प्रतिज्ञा(ts != USEC_INFINITY);

                                usec_t n = usec_sub_unsigned(now(CLOCK_MONOTONIC), ts);
                                यदि (n >= timeout)
                                        प्रत्यागम -ETIMEDOUT;

                                बायाँ = timeout - n;
                        }

                        r = ppoll_usec_full(
                                        &(संरचना pollfd) {
                                                .fd = notify,
                                                .events = POLLIN,
                                        },
                                        /* n_fds= */ 1,
                                        बायाँ,
                                        &poll_ss);
                        यदि (r < 0)
                                प्रत्यागम r;
                        यदि (r == 0)
                                प्रत्यागम -ETIMEDOUT;

                        सङ्घ inotify_event_buffer buffer;
                        ssize_t l;
                        l = पठ(notify, &buffer, परिमाण(buffer));
                        यदि (l < 0) {
                                यदि (ERRNO_IS_TRANSIENT(errno))
                                        अनुवर्त;

                                प्रत्यागम -errno;
                        }

                        FOREACH_INOTIFY_EVENT(e, buffer, l) {
                                यदि (e->mask & IN_Q_OVERFLOW) /* If we hit an inotify queue overflow, simply check if the terminal is up for grabs now. */
                                        भङ्ग;

                                यदि (e->wd != wd || !(e->mask & IN_CLOSE)) /* Safety checks */
                                        प्रत्यागम -EIO;
                        }

                        भङ्ग;
                }

                /* We close the tty fd here since if the old session ended our handle will be dead. It's important that
                 * we do this after sleeping, so that we don't enter an endless loop. */
                fd = safe_close(fd);
        }

        प्रत्यागम TAKE_FD(fd);
}

पूर्णाङ्क release_terminal(शून्य) {
        _cleanup_close_ पूर्णाङ्क fd = -EBADF;
        पूर्णाङ्क r;

        fd = open("/dev/tty", O_RDWR|O_NOCTTY|O_CLOEXEC|O_NONBLOCK);
        यदि (fd < 0)
                प्रत्यागम -errno;

        /* Temporarily ignore SIGHUP, so that we don't get SIGHUP'ed
         * by our own TIOCNOTTY */
        संरचना sigaction sa_old;
        assert_se(sigaction(SIGHUP, &sigaction_ignore, &sa_old) >= 0);

        r = RET_NERRNO(ioctl(fd, TIOCNOTTY));

        assert_se(sigaction(SIGHUP, &sa_old, रिक्त) >= 0);

        प्रत्यागम r;
}

पूर्णाङ्क terminal_new_session(शून्य) {

        /* Make us the new session leader, and set stdin tty to be our controlling terminal.
         *
         * Why stdin? Well, the ctty logic is relevant for signal delivery mostly, i.e. if people hit C-c
         * or the line is hung up. Such events are basically just a form of input, via a side channel
         * (that side channel being signal delivery, i.e. SIGINT, SIGHUP et al). Hence we focus on input,
         * not output here. */

        यदि (!isatty_safe(STDIN_FILENO))
                प्रत्यागम -ENXIO;

        (शून्य) setsid();
        प्रत्यागम RET_NERRNO(ioctl(STDIN_FILENO, TIOCSCTTY, 0));
}

शून्य terminal_detach_session(शून्य) {
        (शून्य) setsid();
        (शून्य) release_terminal();
}

पूर्णाङ्क terminal_vhangup_fd(पूर्णाङ्क fd) {
        प्रतिज्ञा(fd >= 0);
        प्रत्यागम RET_NERRNO(ioctl(fd, TIOCVHANGUP));
}

पूर्णाङ्क terminal_vhangup(ध्रुव वर्ण *tty) {
        _cleanup_close_ पूर्णाङ्क fd = -EBADF;

        प्रतिज्ञा(tty);

        fd = open_terminal(tty, O_RDWR|O_NOCTTY|O_CLOEXEC);
        यदि (fd < 0)
                प्रत्यागम fd;

        प्रत्यागम terminal_vhangup_fd(fd);
}

पूर्णाङ्क vt_disallocate(ध्रुव वर्ण *tty_path) {
        प्रतिज्ञा(tty_path);

        /* Deallocate the VT if possible. If not possible (i.e. because it is the active one), at least clear
         * it entirely (including the scrollback buffer). */

        पूर्णाङ्क ttynr = vtnr_from_tty(tty_path);
        यदि (ttynr > 0) {
                _cleanup_close_ पूर्णाङ्क fd = open_terminal("/dev/tty0", O_RDWR|O_NOCTTY|O_CLOEXEC|O_NONBLOCK);
                यदि (fd < 0)
                        प्रत्यागम fd;

                /* Try to deallocate */
                यदि (ioctl(fd, VT_DISALLOCATE, ttynr) >= 0)
                        प्रत्यागम 0;
                यदि (errno != EBUSY)
                        प्रत्यागम -errno;
        }

        /* So this is not a VT (in which case we cannot deallocate it), or we failed to deallocate. Let's at
         * least clear the screen. */

        _cleanup_close_ पूर्णाङ्क fd2 = open_terminal(tty_path, O_WRONLY|O_NOCTTY|O_CLOEXEC|O_NONBLOCK);
        यदि (fd2 < 0)
                प्रत्यागम fd2;

        प्रत्यागम loop_write_full(fd2,
                               "\033[r"   /* clear scrolling region */
                               "\033[H"   /* move home */
                               "\033[3J"  /* clear screen including scrollback, requires Linux 2.6.40 */
                               "\033c",   /* reset to initial state */
                               SIZE_MAX,
                               CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
}

स्थिर पूर्णाङ्क vt_default_utf8(शून्य) {
        /* Read the default VT UTF8 setting from the kernel */
        प्रत्यागम read_boolean_file("/sys/module/vt/parameters/default_utf8");
}

स्थिर पूर्णाङ्क vt_reset_keyboard(पूर्णाङ्क fd) {
        पूर्णाङ्क r, kb;

        प्रतिज्ञा(fd >= 0);

        /* If we can't read the default, then default to Unicode. It's 2024 after all. */
        r = vt_default_utf8();
        यदि (r < 0)
                log_debug_errno(r, "Failed to determine kernel VT UTF-8 mode, assuming enabled: %m");

        kb = vt_default_utf8() != 0 ? K_UNICODE : K_XLATE;
        प्रत्यागम RET_NERRNO(ioctl(fd, KDSKBMODE, kb));
}

स्थिर पूर्णाङ्क terminal_reset_ioctl(पूर्णाङ्क fd, bool switch_to_text) {
        संरचना termios termios;
        पूर्णाङ्क r;

        /* Set terminal to some sane defaults */

        प्रतिज्ञा(fd >= 0);

        /* We leave locked terminal attributes untouched, so that Plymouth may set whatever it wants to set,
         * and we don't interfere with that. */

        /* Disable exclusive mode, just in case */
        यदि (ioctl(fd, TIOCNXCL) < 0)
                log_debug_errno(errno, "TIOCNXCL ioctl failed on TTY, ignoring: %m");

        /* Switch to text mode */
        यदि (switch_to_text)
                यदि (ioctl(fd, KDSETMODE, KD_TEXT) < 0)
                        log_debug_errno(errno, "KDSETMODE ioctl for switching to text mode failed on TTY, ignoring: %m");

        /* Set default keyboard mode */
        r = vt_reset_keyboard(fd);
        यदि (r < 0)
                log_debug_errno(r, "Failed to reset VT keyboard, ignoring: %m");

        यदि (tcgetattr(fd, &termios) < 0) {
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
        यदि (r < 0)
                log_debug_errno(r, "Failed to set terminal parameters: %m");

finish:
        /* Just in case, flush all crap out */
        (शून्य) tcflush(fd, TCIOFLUSH);

        प्रत्यागम r;
}

पूर्णाङ्क terminal_reset_ansi_seq(पूर्णाङ्क fd) {
        _cleanup_(nonblock_resetp) पूर्णाङ्क nonblock_reset = -EBADF;
        पूर्णाङ्क r;

        प्रतिज्ञा(fd >= 0);

        यदि (getenv_terminal_is_dumb())
                प्रत्यागम 0;

        r = fd_nonblock(fd, true);
        यदि (r < 0)
                प्रत्यागम log_debug_errno(r, "Failed to set terminal to non-blocking mode: %m");
        यदि (r > 0)
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
        यदि (r < 0)
                log_debug_errno(r, "Failed to reset terminal through ANSI sequences: %m");

        प्रत्यागम r;
}

शून्य reset_dev_console_fd(पूर्णाङ्क fd, bool switch_to_text) {
        पूर्णाङ्क r;

        प्रतिज्ञा(fd >= 0);

        _cleanup_close_ पूर्णाङ्क lock_fd = lock_dev_console();
        यदि (lock_fd < 0)
                log_debug_errno(lock_fd, "Failed to lock /dev/console, ignoring: %m");

        r = terminal_reset_ioctl(fd, switch_to_text);
        यदि (r < 0)
                log_warning_errno(r, "Failed to reset /dev/console, ignoring: %m");

        unsigned rows, cols;
        r = proc_cmdline_tty_size("/dev/console", &rows, &cols);
        यदि (r < 0)
                log_warning_errno(r, "Failed to get /dev/console size, ignoring: %m");
        अन्यथा यदि (r > 0) {
                r = terminal_set_size_fd(fd, रिक्त, rows, cols);
                यदि (r < 0)
                        log_warning_errno(r, "Failed to set configured terminal size on /dev/console, ignoring: %m");
        } अन्यथा
                (शून्य) terminal_fix_size(fd, fd);

        r = terminal_reset_ansi_seq(fd);
        यदि (r < 0)
                log_warning_errno(r, "Failed to reset /dev/console using ANSI sequences, ignoring: %m");
}

पूर्णाङ्क lock_dev_console(शून्य) {
        _cleanup_close_ पूर्णाङ्क fd = -EBADF;
        पूर्णाङ्क r;

        /* NB: We do not use O_NOFOLLOW here, because some container managers might place a symlink to some
         * pty in /dev/console, in which case it should be fine to lock the target TTY. */
        fd = open_terminal("/dev/console", O_RDONLY|O_CLOEXEC|O_NOCTTY);
        यदि (fd < 0)
                प्रत्यागम fd;

        r = lock_generic(fd, LOCK_BSD, LOCK_EX);
        यदि (r < 0)
                प्रत्यागम r;

        प्रत्यागम TAKE_FD(fd);
}

पूर्णाङ्क make_console_stdio(bool switch_to_text) {
        पूर्णाङ्क fd, r;

        /* Make /dev/console the controlling terminal and stdin/stdout/stderr, if we can. If we can't use
         * /dev/null instead. This is particularly useful if /dev/console is turned off, e.g. if console=null
         * is specified on the kernel command line. */

        fd = acquire_terminal("/dev/console", ACQUIRE_TERMINAL_FORCE|ACQUIRE_TERMINAL_PERMISSIVE, USEC_INFINITY);
        यदि (fd < 0) {
                log_warning_errno(fd, "Failed to acquire terminal, using /dev/null stdin/stdout/stderr instead: %m");

                r = make_null_stdio();
                यदि (r < 0)
                        प्रत्यागम log_error_errno(r, "Failed to make /dev/null stdin/stdout/stderr: %m");

        } अन्यथा {
                reset_dev_console_fd(fd, switch_to_text);

                r = rearrange_stdio(fd, fd, fd); /* This invalidates 'fd' both on success and on failure. */
                यदि (r < 0)
                        प्रत्यागम log_error_errno(r, "Failed to make terminal stdin/stdout/stderr: %m");
        }

        reset_terminal_feature_caches();
        प्रत्यागम 0;
}

स्थिर पूर्णाङ्क vtnr_from_tty_raw(ध्रुव वर्ण *tty, unsigned *ret) {
        प्रतिज्ञा(tty);

        tty = skip_dev_prefix(tty);

        ध्रुव वर्ण *e = startswith(tty, "tty");
        यदि (!e)
                प्रत्यागम -EINVAL;

        प्रत्यागम safe_atou(e, ret);
}

पूर्णाङ्क vtnr_from_tty(ध्रुव वर्ण *tty) {
        unsigned u;
        पूर्णाङ्क r;

        प्रतिज्ञा(tty);

        r = vtnr_from_tty_raw(tty, &u);
        यदि (r < 0)
                प्रत्यागम r;
        यदि (!vtnr_is_valid(u))
                प्रत्यागम -ERANGE;

        प्रत्यागम (पूर्णाङ्क) u;
}

bool tty_is_vc(ध्रुव वर्ण *tty) {
        प्रतिज्ञा(tty);

        /* NB: for >= 0 values no range check is conducted here, on the assumption that the caller will
         * either extract vtnr through vtnr_from_tty() later where ERANGE would be reported, or doesn't care
         * about whether it's strictly valid, but only asking "does this fall into the vt category?", for which
         * "yes" seems to be a better answer. */

        प्रत्यागम vtnr_from_tty_raw(tty, /* ret= */ रिक्त) >= 0;
}

bool tty_is_console(ध्रुव वर्ण *tty) {
        प्रतिज्ञा(tty);

        प्रत्यागम streq(skip_dev_prefix(tty), "console");
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
        यदि (r < 0)
                प्रत्यागम r;
        यदि (!path_equal(chased, "/dev/console")) {
                *ret = TAKE_PTR(chased);
                प्रत्यागम 0;
        }

        r = path_is_read_only_fs("/sys");
        यदि (r < 0)
                प्रत्यागम r;
        यदि (r > 0)
                प्रत्यागम -ENOMEDIUM;

        _cleanup_free_ वर्ण *active = रिक्त;
        r = read_one_पंक्ति_file("/sys/class/tty/console/active", &active);
        यदि (r < 0)
                प्रत्यागम r;
        यदि (r == 0)
                प्रत्यागम -ENXIO;

        /* If multiple log outputs are configured the last one is what /dev/console points to */
        ध्रुव वर्ण *tty = strrchr(active, ' ');
        यदि (tty)
                tty++;
        अन्यथा
                tty = active;

        यदि (streq(tty, "tty0")) {
                active = mfree(active);

                /* Get the active VC (e.g. tty1) */
                r = read_one_पंक्ति_file("/sys/class/tty/tty0/active", &active);
                यदि (r < 0)
                        प्रत्यागम r;
                यदि (r == 0)
                        प्रत्यागम -ENXIO;

                tty = active;
        }

        _cleanup_free_ वर्ण *path = रिक्त;
        path = path_join("/dev", tty);
        यदि (!path)
                प्रत्यागम -ENOMEM;

        *ret = TAKE_PTR(path);
        प्रत्यागम 0;
}

पूर्णाङ्क get_kernel_consoles(वर्ण ***ret) {
        _cleanup_strv_free_ वर्ण **l = रिक्त;
        _cleanup_free_ वर्ण *पंक्ति = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        /* If /sys/ is mounted read-only this means we are running in some kind of container environment.
         * In that case /sys/ would reflect the host system, not us, hence ignore the data we can read from it. */
        यदि (path_is_read_only_fs("/sys") > 0)
                गच्छ fallback;

        r = read_one_पंक्ति_file("/sys/class/tty/console/active", &पंक्ति);
        यदि (r < 0)
                प्रत्यागम r;

        प्रति (ध्रुव वर्ण *p = पंक्ति;;) {
                _cleanup_free_ वर्ण *tty = रिक्त, *path = रिक्त;

                r = extract_first_word(&p, &tty, रिक्त, 0);
                यदि (r < 0)
                        प्रत्यागम r;
                यदि (r == 0)
                        भङ्ग;

                यदि (streq(tty, "tty0")) {
                        tty = mfree(tty);
                        r = read_one_पंक्ति_file("/sys/class/tty/tty0/active", &tty);
                        यदि (r < 0)
                                प्रत्यागम r;
                        यदि (r == 0) {
                                log_debug("No VT active, skipping /dev/tty0.");
                                अनुवर्त;
                        }
                }

                path = path_join("/dev", tty);
                यदि (!path)
                        प्रत्यागम -ENOMEM;

                यदि (access(path, F_OK) < 0) {
                        log_debug_errno(errno, "Console device %s is not accessible, skipping: %m", path);
                        अनुवर्त;
                }

                r = strv_consume(&l, TAKE_PTR(path));
                यदि (r < 0)
                        प्रत्यागम r;
        }

        यदि (strv_isempty(l)) {
                log_debug("No devices found for system console");
                गच्छ fallback;
        }

        *ret = TAKE_PTR(l);
        प्रत्यागम strv_length(*ret);

fallback:
        r = strv_extend(&l, "/dev/console");
        यदि (r < 0)
                प्रत्यागम r;

        *ret = TAKE_PTR(l);
        प्रत्यागम 0;
}

bool tty_is_vc_resolve(ध्रुव वर्ण *tty) {
        _cleanup_free_ वर्ण *resolved = रिक्त;

        प्रतिज्ञा(tty);

        यदि (streq(skip_dev_prefix(tty), "console")) {
                यदि (resolve_dev_console(&resolved) < 0)
                        प्रत्यागम false;

                tty = resolved;
        }

        प्रत्यागम tty_is_vc(tty);
}

पूर्णाङ्क fd_columns(पूर्णाङ्क fd) {
        संरचना winsize ws = {};

        यदि (fd < 0)
                प्रत्यागम -EBADF;

        यदि (ioctl(fd, TIOCGWINSZ, &ws) < 0)
                प्रत्यागम -errno;

        यदि (ws.ws_col <= 0)
                प्रत्यागम -ENODATA; /* some tty types come up with invalid row/column initially, return a recognizable error for that */

        प्रत्यागम ws.ws_col;
}

पूर्णाङ्क getenv_columns(शून्य) {
        पूर्णाङ्क r;

        ध्रुव वर्ण *e = getenv("COLUMNS");
        यदि (!e)
                प्रत्यागम -ENXIO;

        unsigned c;
        r = safe_atou_bounded(e, 1, USHRT_MAX, &c);
        यदि (r < 0)
                प्रत्यागम r;

        प्रत्यागम (पूर्णाङ्क) c;
}

unsigned columns(शून्य) {

        यदि (cached_columns > 0)
                प्रत्यागम cached_columns;

        पूर्णाङ्क c = getenv_columns();
        यदि (c < 0) {
                c = fd_columns(STDOUT_FILENO);
                यदि (c < 0)
                        c = 80;
        }

        प्रतिज्ञा(c > 0);

        cached_columns = c;
        प्रत्यागम cached_columns;
}

पूर्णाङ्क fd_पंक्तियाँ(पूर्णाङ्क fd) {
        संरचना winsize ws = {};

        यदि (fd < 0)
                प्रत्यागम -EBADF;

        यदि (ioctl(fd, TIOCGWINSZ, &ws) < 0)
                प्रत्यागम -errno;

        यदि (ws.ws_row <= 0)
                प्रत्यागम -ENODATA; /* some tty types come up with invalid row/column initially, return a recognizable error for that */

        प्रत्यागम ws.ws_row;
}

unsigned पंक्तियाँ(शून्य) {
        ध्रुव वर्ण *e;
        पूर्णाङ्क l;

        यदि (cached_पंक्तियाँ > 0)
                प्रत्यागम cached_पंक्तियाँ;

        l = 0;
        e = getenv("LINES");
        यदि (e)
                (शून्य) safe_atoi(e, &l);

        यदि (l <= 0 || l > USHRT_MAX) {
                l = fd_पंक्तियाँ(STDOUT_FILENO);
                यदि (l <= 0)
                        l = 24;
        }

        cached_पंक्तियाँ = l;
        प्रत्यागम cached_पंक्तियाँ;
}

पूर्णाङ्क terminal_set_size_fd(पूर्णाङ्क fd, ध्रुव वर्ण *ident, unsigned rows, unsigned cols) {
        संरचना winsize ws;

        प्रतिज्ञा(fd >= 0);

        यदि (!ident)
                ident = "TTY";

        यदि (rows == UINT_MAX && cols == UINT_MAX)
                प्रत्यागम 0;

        यदि (ioctl(fd, TIOCGWINSZ, &ws) < 0)
                प्रत्यागम log_debug_errno(errno,
                                       "TIOCGWINSZ ioctl for getting %s size failed, not setting terminal size: %m",
                                       ident);

        यदि (rows == UINT_MAX)
                rows = ws.ws_row;
        अन्यथा यदि (rows > USHRT_MAX)
                rows = USHRT_MAX;

        यदि (cols == UINT_MAX)
                cols = ws.ws_col;
        अन्यथा यदि (cols > USHRT_MAX)
                cols = USHRT_MAX;

        यदि (rows == ws.ws_row && cols == ws.ws_col)
                प्रत्यागम 0;

        ws.ws_row = rows;
        ws.ws_col = cols;

        यदि (ioctl(fd, TIOCSWINSZ, &ws) < 0)
                प्रत्यागम log_debug_errno(errno, "TIOCSWINSZ ioctl for setting %s size failed: %m", ident);

        प्रत्यागम 0;
}

पूर्णाङ्क proc_cmdline_tty_size(ध्रुव वर्ण *tty, unsigned *ret_rows, unsigned *ret_cols) {
        _cleanup_free_ वर्ण *rowskey = रिक्त, *rowsvalue = रिक्त, *colskey = रिक्त, *colsvalue = रिक्त;
        unsigned rows = UINT_MAX, cols = UINT_MAX;
        पूर्णाङ्क r;

        प्रतिज्ञा(tty);

        यदि (!ret_rows && !ret_cols)
                प्रत्यागम 0;

        tty = skip_dev_prefix(tty);
        यदि (path_startswith(tty, "pts/"))
                प्रत्यागम -EMEDIUMTYPE;
        यदि (!in_charset(tty, ALPHANUMERICAL))
                प्रत्यागम log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "TTY name '%s' contains non-alphanumeric characters, not searching kernel cmdline for size.", tty);

        rowskey = strjoin("systemd.tty.rows.", tty);
        यदि (!rowskey)
                प्रत्यागम -ENOMEM;

        colskey = strjoin("systemd.tty.columns.", tty);
        यदि (!colskey)
                प्रत्यागम -ENOMEM;

        r = proc_cmdline_get_कुंजी_many(/* flags= */ 0,
                                      rowskey, &rowsvalue,
                                      colskey, &colsvalue);
        यदि (r < 0)
                प्रत्यागम log_debug_errno(r, "Failed to read TTY size of %s from kernel cmdline: %m", tty);

        यदि (rowsvalue) {
                r = safe_atou(rowsvalue, &rows);
                यदि (r < 0)
                        प्रत्यागम log_debug_errno(r, "Failed to parse %s=%s: %m", rowskey, rowsvalue);
        }

        यदि (colsvalue) {
                r = safe_atou(colsvalue, &cols);
                यदि (r < 0)
                        प्रत्यागम log_debug_errno(r, "Failed to parse %s=%s: %m", colskey, colsvalue);
        }

        यदि (ret_rows)
                *ret_rows = rows;
        यदि (ret_cols)
                *ret_cols = cols;

        प्रत्यागम rows != UINT_MAX || cols != UINT_MAX;
}

/* intended to be used as a SIGWINCH sighandler */
शून्य columns_पंक्तियाँ_cache_reset(पूर्णाङ्क signum) {
        cached_columns = 0;
        cached_पंक्तियाँ = 0;
}

शून्य reset_terminal_feature_caches(शून्य) {
        cached_columns = 0;
        cached_पंक्तियाँ = 0;

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

        यदि (cached_on_tty < 0)
                cached_on_tty =
                        isatty_safe(STDOUT_FILENO) &&
                        isatty_safe(STDERR_FILENO);

        प्रत्यागम cached_on_tty;
}

पूर्णाङ्क getttyname_malloc(पूर्णाङ्क fd, वर्ण **ret) {
        वर्ण path[PATH_MAX]; /* PATH_MAX is counted *with* the trailing NUL byte */
        पूर्णाङ्क r;

        प्रतिज्ञा(fd >= 0);
        प्रतिज्ञा(ret);

        r = ttyname_r(fd, path, परिमाण path); /* positive error */
        प्रतिज्ञा(r >= 0);
        यदि (r == ERANGE)
                प्रत्यागम -ENAMETOOLONG;
        यदि (r > 0)
                प्रत्यागम -r;

        प्रत्यागम strdup_to(ret, skip_dev_prefix(path));
}

पूर्णाङ्क getttyname_harder(पूर्णाङ्क fd, वर्ण **ret) {
        _cleanup_free_ वर्ण *s = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        r = getttyname_malloc(fd, &s);
        यदि (r < 0)
                प्रत्यागम r;

        यदि (streq(s, "tty"))
                प्रत्यागम get_ctty(0, रिक्त, ret);

        *ret = TAKE_PTR(s);
        प्रत्यागम 0;
}

पूर्णाङ्क get_ctty_devnr(pid_t pid, dev_t *ret) {
        _cleanup_free_ वर्ण *पंक्ति = रिक्त;
        unsigned long ttynr;
        ध्रुव वर्ण *p;
        पूर्णाङ्क r;

        प्रतिज्ञा(pid >= 0);

        p = procfs_file_alloca(pid, "stat");
        r = read_one_पंक्ति_file(p, &पंक्ति);
        यदि (r < 0)
                प्रत्यागम r;

        p = strrchr(पंक्ति, ')');
        यदि (!p)
                प्रत्यागम -EIO;

        p++;

        यदि (sscanf(p, " "
                   "%*c "  /* state */
                   "%*d "  /* ppid */
                   "%*d "  /* pgrp */
                   "%*d "  /* session */
                   "%lu ", /* ttynr */
                   &ttynr) != 1)
                प्रत्यागम -EIO;

        यदि (devnum_is_zero(ttynr))
                प्रत्यागम -ENXIO;

        यदि (ret)
                *ret = (dev_t) ttynr;

        प्रत्यागम 0;
}

पूर्णाङ्क get_ctty(pid_t pid, dev_t *ret_devnr, वर्ण **ret) {
        वर्ण pty[STRLEN("/dev/pts/") + DECIMAL_STR_MAX(dev_t) + 1];
        _cleanup_free_ वर्ण *buf = रिक्त;
        ध्रुव वर्ण *fn = रिक्त, *w;
        dev_t devnr;
        पूर्णाङ्क r;

        r = get_ctty_devnr(pid, &devnr);
        यदि (r < 0)
                प्रत्यागम r;

        r = device_path_make_canonical(S_IFCHR, devnr, &buf);
        यदि (r < 0) {
                संरचना stat st;

                यदि (r != -ENOENT) /* No symlink for this in /dev/char/? */
                        प्रत्यागम r;

                /* Maybe this is PTY? PTY devices are not listed in /dev/char/, as they don't follow the
                 * Linux device model and hence device_path_make_canonical() doesn't work for them. Let's
                 * assume this is a PTY for a moment, and check if the device node this would then map to in
                 * /dev/pts/ matches the one we are looking for. This way we don't have to hardcode the major
                 * number (which is 136 btw), but we still rely on the fact that PTY numbers map directly to
                 * the minor number of the pty. */
                xsprintf(pty, "/dev/pts/%u", minor(devnr));

                यदि (stat(pty, &st) < 0) {
                        यदि (errno != ENOENT)
                                प्रत्यागम -errno;

                } अन्यथा यदि (S_ISCHR(st.st_mode) && devnr == st.st_rdev) /* Bingo! */
                        fn = pty;

                यदि (!fn) {
                        /* Doesn't exist, or not a PTY? Probably something similar to the PTYs which have no
                         * symlink in /dev/char/. Let's return something vaguely useful. */
                        r = device_path_make_major_minor(S_IFCHR, devnr, &buf);
                        यदि (r < 0)
                                प्रत्यागम r;

                        fn = buf;
                }
        } अन्यथा
                fn = buf;

        w = path_startswith(fn, "/dev/");
        यदि (!w)
                प्रत्यागम -EINVAL;

        यदि (ret) {
                r = strdup_to(ret, w);
                यदि (r < 0)
                        प्रत्यागम r;
        }

        यदि (ret_devnr)
                *ret_devnr = devnr;

        प्रत्यागम 0;
}

पूर्णाङ्क ptsname_malloc(पूर्णाङ्क fd, वर्ण **ret) {
        प्रतिज्ञा(fd >= 0);
        प्रतिज्ञा(ret);

        प्रति (size_t l = 50;;) {
                _cleanup_free_ वर्ण *c = रिक्त;

                c = new(वर्ण, l);
                यदि (!c)
                        प्रत्यागम -ENOMEM;

                यदि (ptsname_r(fd, c, l) >= 0) {
                        *ret = TAKE_PTR(c);
                        प्रत्यागम 0;
                }
                यदि (errno != ERANGE)
                        प्रत्यागम -errno;

                यदि (!MUL_ASSIGN_SAFE(&l, 2))
                        प्रत्यागम -ENOMEM;
        }
}

पूर्णाङ्क openpt_allocate(पूर्णाङ्क flags, वर्ण **ret_peer_path) {
        _cleanup_close_ पूर्णाङ्क fd = -EBADF;
        पूर्णाङ्क r;

        fd = posix_openpt(flags|O_NOCTTY|O_CLOEXEC);
        यदि (fd < 0)
                प्रत्यागम -errno;

        _cleanup_free_ वर्ण *p = रिक्त;
        यदि (ret_peer_path) {
                r = ptsname_malloc(fd, &p);
                यदि (r < 0)
                        प्रत्यागम r;

                यदि (!path_startswith(p, "/dev/pts/"))
                        प्रत्यागम -EINVAL;
        }

        यदि (unlockpt(fd) < 0)
                प्रत्यागम -errno;

        यदि (ret_peer_path)
                *ret_peer_path = TAKE_PTR(p);

        प्रत्यागम TAKE_FD(fd);
}

स्थिर पूर्णाङ्क ptsname_namespace(पूर्णाङ्क pty, वर्ण **ret) {
        पूर्णाङ्क no = -1;

        प्रतिज्ञा(pty >= 0);
        प्रतिज्ञा(ret);

        /* Like ptsname(), but doesn't assume that the path is
         * accessible in the local namespace. */

        यदि (ioctl(pty, TIOCGPTN, &no) < 0)
                प्रत्यागम -errno;

        यदि (no < 0)
                प्रत्यागम -EIO;

        यदि (asprintf(ret, "/dev/pts/%i", no) < 0)
                प्रत्यागम -ENOMEM;

        प्रत्यागम 0;
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
        यदि (r < 0)
                प्रत्यागम log_debug_errno(r, "Failed to open namespaces of PID "PID_FMT": %m", pidref->pid);

        यदि (socketpair(AF_UNIX, SOCK_DGRAM|SOCK_CLOEXEC, 0, pair) < 0)
                प्रत्यागम -errno;

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
        यदि (r < 0)
                प्रत्यागम r;
        यदि (r == 0) {
                pair[0] = safe_close(pair[0]);

                fd = openpt_allocate(flags, /* ret_peer_path= */ रिक्त);
                यदि (fd < 0)
                        _exit(EXIT_FAILURE);

                यदि (send_one_fd(pair[1], fd, 0) < 0)
                        _exit(EXIT_FAILURE);

                _exit(EXIT_SUCCESS);
        }

        pair[1] = safe_close(pair[1]);

        fd = receive_one_fd(pair[0], 0);
        यदि (fd < 0)
                प्रत्यागम fd;

        यदि (ret_peer_path) {
                r = ptsname_namespace(fd, ret_peer_path);
                यदि (r < 0)
                        प्रत्यागम r;
        }

        प्रत्यागम TAKE_FD(fd);
}

स्थिर bool on_dev_null(शून्य) {
        संरचना stat dst, ost, est;

        यदि (cached_on_dev_null >= 0)
                प्रत्यागम cached_on_dev_null;

        यदि (stat("/dev/null", &dst) < 0 || fstat(STDOUT_FILENO, &ost) < 0 || fstat(STDERR_FILENO, &est) < 0)
                cached_on_dev_null = false;
        अन्यथा
                cached_on_dev_null = stat_inode_same(&dst, &ost) && stat_inode_same(&dst, &est);

        प्रत्यागम cached_on_dev_null;
}

bool term_env_valid(ध्रुव वर्ण *term) {
        /* Checks if the specified $TERM value is suitable for propagation, i.e. is not empty, not set to
         * "unknown" (as is common in CI), and only contains characters valid in terminal type names.
         * Valid $TERM values are things like "xterm-256color", "linux", "screen.xterm-256color", i.e.
         * alphanumeric characters, hyphens, underscores, dots, and plus signs. */
        प्रत्यागम !isempty(term) &&
                !streq(term, "unknown") &&
                in_charset(term, ALPHANUMERICAL "-_+.");
}

bool getenv_terminal_is_dumb(शून्य) {
        ध्रुव वर्ण *e;

        e = getenv("TERM");
        यदि (!e)
                प्रत्यागम true;

        प्रत्यागम streq(e, "dumb");
}

bool terminal_is_dumb(शून्य) {
        यदि (!on_tty() && !on_dev_null())
                प्रत्यागम true;

        प्रत्यागम getenv_terminal_is_dumb();
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
        यदि (m >= 0)
                प्रत्यागम m;

        यदि (getenv("NO_COLOR"))
                प्रत्यागम false;

        यदि (getenv_for_pid(1, "TERM", &s) <= 0)
                (शून्य) proc_cmdline_get_कुंजी("TERM", 0, &s);

        प्रत्यागम !streq_ptr(s, "dumb");
}

पूर्णाङ्क vt_restore(पूर्णाङ्क fd) {

        स्थिर ध्रुव संरचना vt_mode mode = {
                .mode = VT_AUTO,
        };

        पूर्णाङ्क r, ret = 0;

        प्रतिज्ञा(fd >= 0);

        यदि (!isatty_safe(fd))
                प्रत्यागम log_debug_errno(SYNTHETIC_ERRNO(ENOTTY), "Asked to restore the VT for an fd that does not refer to a terminal.");

        यदि (ioctl(fd, KDSETMODE, KD_TEXT) < 0)
                RET_GATHER(ret, log_debug_errno(errno, "Failed to set VT to text mode, ignoring: %m"));

        r = vt_reset_keyboard(fd);
        यदि (r < 0)
                RET_GATHER(ret, log_debug_errno(r, "Failed to reset keyboard mode, ignoring: %m"));

        यदि (ioctl(fd, VT_SETMODE, &mode) < 0)
                RET_GATHER(ret, log_debug_errno(errno, "Failed to set VT_AUTO mode, ignoring: %m"));

        r = fchmod_and_chown(fd, TTY_MODE, 0, GID_INVALID);
        यदि (r < 0)
                RET_GATHER(ret, log_debug_errno(r, "Failed to chmod()/chown() VT, ignoring: %m"));

        प्रत्यागम ret;
}

पूर्णाङ्क vt_release(पूर्णाङ्क fd, bool restore) {
        प्रतिज्ञा(fd >= 0);

        /* This function releases the VT by acknowledging the VT-switch signal
         * sent by the kernel and optionally reset the VT in text and auto
         * VT-switching modes. */

        यदि (!isatty_safe(fd))
                प्रत्यागम log_debug_errno(SYNTHETIC_ERRNO(ENOTTY), "Asked to release the VT for an fd that does not refer to a terminal.");

        यदि (ioctl(fd, VT_RELDISP, 1) < 0)
                प्रत्यागम -errno;

        यदि (restore)
                प्रत्यागम vt_restore(fd);

        प्रत्यागम 0;
}

शून्य get_log_colors(पूर्णाङ्क priority, ध्रुव वर्ण **on, ध्रुव वर्ण **off, ध्रुव वर्ण **highlight) {
        /* Note that this will initialize output variables only when there's something to output.
         * The caller must pre-initialize to "" or NULL as appropriate. */

        यदि (priority <= LOG_ERR) {
                यदि (on)
                        *on = ansi_highlight_red();
                यदि (off)
                        *off = ansi_normal();
                यदि (highlight)
                        *highlight = ansi_highlight();

        } अन्यथा यदि (priority <= LOG_WARNING) {
                यदि (on)
                        *on = ansi_highlight_yellow();
                यदि (off)
                        *off = ansi_normal();
                यदि (highlight)
                        *highlight = ansi_highlight();

        } अन्यथा यदि (priority <= LOG_NOTICE) {
                यदि (on)
                        *on = ansi_highlight();
                यदि (off)
                        *off = ansi_normal();
                यदि (highlight)
                        *highlight = ansi_highlight_red();

        } अन्यथा यदि (priority >= LOG_DEBUG) {
                यदि (on)
                        *on = ansi_grey();
                यदि (off)
                        *off = ansi_normal();
                यदि (highlight)
                        *highlight = ansi_highlight_red();
        }
}

पूर्णाङ्क terminal_set_cursor_position(पूर्णाङ्क fd, unsigned row, unsigned column) {
        प्रतिज्ञा(fd >= 0);

        वर्ण cursor_position[STRLEN("\x1B[" ";" "H") + DECIMAL_STR_MAX(unsigned) * 2 + 1];
        xsprintf(cursor_position, "\x1B[%u;%uH", row, column);

        प्रत्यागम loop_write(fd, cursor_position, SIZE_MAX);
}

स्थिर पूर्णाङ्क terminal_verify_same(पूर्णाङ्क निवेश_fd, पूर्णाङ्क output_fd) {
        पूर्णाङ्क r;

        प्रतिज्ञा(निवेश_fd >= 0);
        प्रतिज्ञा(output_fd >= 0);

        /* Validates that the specified fds reference the same TTY */

        यदि (निवेश_fd != output_fd) {
                संरचना stat sti;
                यदि (fstat(निवेश_fd, &sti) < 0)
                        प्रत्यागम -errno;

                r = stat_verify_char(&sti); /* TTYs are character devices */
                यदि (r < 0)
                        प्रत्यागम r;

                संरचना stat sto;
                यदि (fstat(output_fd, &sto) < 0)
                        प्रत्यागम -errno;

                r = stat_verify_char(&sto);
                यदि (r < 0)
                        प्रत्यागम r;

                यदि (sti.st_rdev != sto.st_rdev)
                        प्रत्यागम -ENOLINK;
        }

        यदि (!isatty_safe(निवेश_fd)) /* The check above was just for char device, but now let's ensure it's actually a tty */
                प्रत्यागम -ENOTTY;

        प्रत्यागम 0;
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

        प्रति (size_t i = 0; i < size; i++) {
                वर्ण c = buf[i];

                विकल्प (context->state) {

                पक्ष CURSOR_TEXT:
                        context->state = c == '\x1B' ? CURSOR_ESCAPE : CURSOR_TEXT;
                        भङ्ग;

                पक्ष CURSOR_ESCAPE:
                        context->state = c == '[' ? CURSOR_ROW : CURSOR_TEXT;
                        भङ्ग;

                पक्ष CURSOR_ROW:
                        यदि (c == ';')
                                context->state = context->row > 0 ? CURSOR_COLUMN : CURSOR_TEXT;
                        अन्यथा {
                                पूर्णाङ्क d = undecchar(c);

                                /* We read a decimal character, let's suffix it to the number we so far read,
                                 * but let's do an overflow check first. */
                                यदि (d < 0 || context->row > (UINT_MAX-d)/10)
                                        context->state = CURSOR_TEXT;
                                अन्यथा
                                        context->row = context->row * 10 + d;
                        }
                        भङ्ग;

                पक्ष CURSOR_COLUMN:
                        यदि (c == 'R') {
                                यदि (context->column > 0) {
                                        *ret_processed = i + 1;
                                        प्रत्यागम 1; /* success! */
                                }

                                context->state = CURSOR_TEXT;
                        } अन्यथा {
                                पूर्णाङ्क d = undecchar(c);

                                /* As above, add the decimal character to our column number */
                                यदि (d < 0 || context->column > (UINT_MAX-d)/10)
                                        context->state = CURSOR_TEXT;
                                अन्यथा
                                        context->column = context->column * 10 + d;
                        }

                        भङ्ग;
                }

                /* Reset any positions we might have picked up */
                यदि (IN_SET(context->state, CURSOR_TEXT, CURSOR_ESCAPE))
                        context->row = context->column = 0;
        }

        *ret_processed = size;
        प्रत्यागम 0; /* all good, but not enough data yet */
}

पूर्णाङ्क terminal_get_cursor_position(
                पूर्णाङ्क निवेश_fd,
                पूर्णाङ्क output_fd,
                unsigned *ret_row,
                unsigned *ret_column) {

        _cleanup_close_ पूर्णाङ्क nonblock_निवेश_fd = -EBADF;
        पूर्णाङ्क r;

        प्रतिज्ञा(निवेश_fd >= 0);
        प्रतिज्ञा(output_fd >= 0);

        यदि (getenv_terminal_is_dumb())
                प्रत्यागम -EOPNOTSUPP;

        r = terminal_verify_same(निवेश_fd, output_fd);
        यदि (r < 0)
                प्रत्यागम log_debug_errno(r, "Called with distinct input/output fds: %m");

        /* Failure to reset the terminal is ignored here and in similar cases below.
         * We already have our result; if cleanup fails it doesn't change the validity of the result. */
        संरचना termios old_termios = TERMIOS_NULL;
        CLEANUP_TERMIOS_RESET(निवेश_fd, old_termios);

        यदि (tcgetattr(निवेश_fd, &old_termios) < 0)
                प्रत्यागम log_debug_errno(errno, "Failed to get terminal settings: %m");

        संरचना termios new_termios = old_termios;
        termios_disable_echo(&new_termios);

        यदि (tcsetattr(निवेश_fd, TCSANOW, &new_termios) < 0)
                प्रत्यागम log_debug_errno(errno, "Failed to set new terminal settings: %m");

        /* Request cursor position (DSR/CPR) */
        r = loop_write(output_fd, "\x1B[6n", SIZE_MAX);
        यदि (r < 0)
                प्रत्यागम r;

        /* Open a 2nd input fd, in non-blocking mode, so that we won't ever hang in read() should someone
         * else process the POLLIN. */

        nonblock_निवेश_fd = r = fd_reopen(निवेश_fd, O_RDONLY|O_CLOEXEC|O_NONBLOCK|O_NOCTTY);
        यदि (r < 0)
                प्रत्यागम r;

        usec_t end = usec_add(now(CLOCK_MONOTONIC), CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        वर्ण buf[STRLEN("\x1B[1;1R")]; /* The shortest valid reply possible */
        size_t buf_full = 0;
        CursorPositionContext context = {};

        प्रति (bool first = true;; first = false) {
                यदि (buf_full == 0) {
                        usec_t n = now(CLOCK_MONOTONIC);
                        यदि (n >= end)
                                प्रत्यागम -EOPNOTSUPP;

                        r = fd_wait_for_event(nonblock_निवेश_fd, POLLIN, usec_sub_unsigned(end, n));
                        यदि (r < 0)
                                प्रत्यागम r;
                        यदि (r == 0)
                                प्रत्यागम -EOPNOTSUPP;

                        /* On the first try, read multiple characters, i.e. the shortest valid
                         * reply. Afterwards read byte-wise, since we don't want to read too much, and
                         * unnecessarily drop too many characters from the input queue. */
                        ssize_t l = पठ(nonblock_निवेश_fd, buf, first ? परिमाण(buf) : 1);
                        यदि (l < 0) {
                                यदि (errno == EAGAIN)
                                        अनुवर्त;

                                प्रत्यागम -errno;
                        }

                        प्रतिज्ञा((size_t) l <= परिमाण(buf));
                        buf_full = l;
                }

                size_t processed;
                r = scan_cursor_position_response(&context, buf, buf_full, &processed);
                यदि (r < 0)
                        प्रत्यागम r;

                प्रतिज्ञा(processed <= buf_full);
                buf_full -= processed;
                memmove(buf, buf + processed, buf_full);

                यदि (r > 0) {
                        /* Superficial validity check */
                        यदि (context.row >= 32766 || context.column >= 32766)
                                प्रत्यागम -ENODATA;

                        यदि (ret_row)
                                *ret_row = context.row;
                        यदि (ret_column)
                                *ret_column = context.column;

                        प्रत्यागम 0;
                }
        }
}

पूर्णाङ्क terminal_reset_defensive(पूर्णाङ्क fd, TerminalResetFlags flags) {
        पूर्णाङ्क r = 0;

        प्रतिज्ञा(fd >= 0);
        प्रतिज्ञा(!FLAGS_SET(flags, TERMINAL_RESET_AVOID_ANSI_SEQ|TERMINAL_RESET_FORCE_ANSI_SEQ));

        /* Resets the terminal comprehensively, i.e. via both ioctl()s and via ANSI sequences, but do so only
         * if $TERM is unset or set to "dumb" */

        यदि (!isatty_safe(fd))
                प्रत्यागम -ENOTTY;

        RET_GATHER(r, terminal_reset_ioctl(fd, FLAGS_SET(flags, TERMINAL_RESET_SWITCH_TO_TEXT)));

        यदि (!FLAGS_SET(flags, TERMINAL_RESET_AVOID_ANSI_SEQ) &&
            (FLAGS_SET(flags, TERMINAL_RESET_FORCE_ANSI_SEQ) || !getenv_terminal_is_dumb()))
                RET_GATHER(r, terminal_reset_ansi_seq(fd));

        प्रत्यागम r;
}

पूर्णाङ्क terminal_reset_defensive_locked(पूर्णाङ्क fd, TerminalResetFlags flags) {
        प्रतिज्ञा(fd >= 0);

        _cleanup_close_ पूर्णाङ्क lock_fd = lock_dev_console();
        यदि (lock_fd < 0)
                log_debug_errno(lock_fd, "Failed to acquire lock for /dev/console, ignoring: %m");

        प्रत्यागम terminal_reset_defensive(fd, flags);
}

शून्य termios_disable_echo(संरचना termios *termios) {
        प्रतिज्ञा(termios);

        termios->c_lflag &= ~(ICANON|ECHO);
        termios->c_cc[VMIN] = 1;
        termios->c_cc[VTIME] = 0;
}

स्थिर bool termios_is_null(ध्रुव संरचना termios *t) {
        यदि (!t)
                प्रत्यागम true;

        प्रत्यागम t->c_iflag == UINT_MAX &&
               t->c_oflag == UINT_MAX &&
               t->c_cflag == UINT_MAX &&
               t->c_lflag == UINT_MAX;
}

शून्य termios_reset(ध्रुव TermiosResetContext *c) {
        प्रतिज्ञा(c);

        PROTECT_ERRNO;

        यदि (c->fd && *c->fd >= 0 && !termios_is_null(c->termios))
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

        प्रति (size_t i = 0; i < size; i++) {
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
                        यदि (c == '/')
                                context->state = context->red_bits > 0 ? BACKGROUND_GREEN : BACKGROUND_TEXT;
                        अन्यथा {
                                पूर्णाङ्क d = unhexchar(c);
                                यदि (d < 0 || context->red_bits >= परिमाण(context->red)*8)
                                        context->state = BACKGROUND_TEXT;
                                अन्यथा {
                                        context->red = (context->red << 4) | d;
                                        context->red_bits += 4;
                                }
                        }
                        भङ्ग;

                पक्ष BACKGROUND_GREEN:
                        यदि (c == '/')
                                context->state = context->green_bits > 0 ? BACKGROUND_BLUE : BACKGROUND_TEXT;
                        अन्यथा {
                                पूर्णाङ्क d = unhexchar(c);
                                यदि (d < 0 || context->green_bits >= परिमाण(context->green)*8)
                                        context->state = BACKGROUND_TEXT;
                                अन्यथा {
                                        context->green = (context->green << 4) | d;
                                        context->green_bits += 4;
                                }
                        }
                        भङ्ग;

                पक्ष BACKGROUND_BLUE:
                        यदि (c == '\x07') {
                                यदि (context->blue_bits > 0) {
                                        *ret_processed = i + 1;
                                        प्रत्यागम 1; /* success! */
                                }

                                context->state = BACKGROUND_TEXT;
                        } अन्यथा यदि (c == '\x1b')
                                context->state = context->blue_bits > 0 ? BACKGROUND_STRING_TERMINATOR : BACKGROUND_TEXT;
                        अन्यथा {
                                पूर्णाङ्क d = unhexchar(c);
                                यदि (d < 0 || context->blue_bits >= परिमाण(context->blue)*8)
                                        context->state = BACKGROUND_TEXT;
                                अन्यथा {
                                        context->blue = (context->blue << 4) | d;
                                        context->blue_bits += 4;
                                }
                        }
                        भङ्ग;

                पक्ष BACKGROUND_STRING_TERMINATOR:
                        यदि (c == '\\') {
                                *ret_processed = i + 1;
                                प्रत्यागम 1; /* success! */
                        }

                        context->state = c == ']' ? BACKGROUND_ESCAPE : BACKGROUND_TEXT;
                        भङ्ग;

                }

                /* Reset any colors we might have picked up */
                यदि (IN_SET(context->state, BACKGROUND_TEXT, BACKGROUND_ESCAPE)) {
                        /* reset color */
                        context->red = context->green = context->blue = 0;
                        context->red_bits = context->green_bits = context->blue_bits = 0;
                }
        }

        *ret_processed = size;
        प्रत्यागम 0; /* all good, but not enough data yet */
}

पूर्णाङ्क get_default_background_color(द्विगुण *ret_red, द्विगुण *ret_green, द्विगुण *ret_blue) {
        पूर्णाङ्क r;

        प्रतिज्ञा(ret_red);
        प्रतिज्ञा(ret_green);
        प्रतिज्ञा(ret_blue);

        यदि (!colors_enabled())
                प्रत्यागम -EOPNOTSUPP;

        r = terminal_verify_same(STDIN_FILENO, STDOUT_FILENO);
        यदि (r < 0)
                प्रत्यागम r;

        यदि (streq_ptr(getenv("TERM"), "linux")) {
                /* Linux console is black */
                *ret_red = *ret_green = *ret_blue = 0.0;
                प्रत्यागम 0;
        }

        /* Open a 2nd input fd, in non-blocking mode, so that we won't ever hang in read()
         * should someone else process the POLLIN. Do all subsequent operations on the new fd. */
        _cleanup_close_ पूर्णाङ्क nonblock_निवेश_fd = r = fd_reopen(STDIN_FILENO, O_RDONLY|O_CLOEXEC|O_NONBLOCK|O_NOCTTY);
        यदि (r < 0)
                प्रत्यागम r;

        संरचना termios old_termios = TERMIOS_NULL;
        CLEANUP_TERMIOS_RESET(nonblock_निवेश_fd, old_termios);

        यदि (tcgetattr(nonblock_निवेश_fd, &old_termios) < 0)
                प्रत्यागम -errno;

        संरचना termios new_termios = old_termios;
        termios_disable_echo(&new_termios);

        यदि (tcsetattr(nonblock_निवेश_fd, TCSANOW, &new_termios) < 0)
                प्रत्यागम -errno;

        r = loop_write(STDOUT_FILENO, ANSI_OSC "11;?" ANSI_ST, SIZE_MAX);
        यदि (r < 0)
                प्रत्यागम r;

        usec_t end = usec_add(now(CLOCK_MONOTONIC), CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        वर्ण buf[STRLEN(ANSI_OSC "11;rgb:0/0/0" ANSI_ST)]; /* shortest possible reply */
        size_t buf_full = 0;
        BackgroundColorContext context = {};

        प्रति (bool first = true;; first = false) {
                यदि (buf_full == 0) {
                        usec_t n = now(CLOCK_MONOTONIC);
                        यदि (n >= end)
                                प्रत्यागम -EOPNOTSUPP;

                        r = fd_wait_for_event(nonblock_निवेश_fd, POLLIN, usec_sub_unsigned(end, n));
                        यदि (r < 0)
                                प्रत्यागम r;
                        यदि (r == 0)
                                प्रत्यागम -EOPNOTSUPP;

                        /* On the first try, read multiple characters, i.e. the shortest valid
                         * reply. Afterwards read byte-wise, since we don't want to read too much, and
                         * unnecessarily drop too many characters from the input queue. */
                        ssize_t l = पठ(nonblock_निवेश_fd, buf, first ? परिमाण(buf) : 1);
                        यदि (l < 0) {
                                यदि (errno == EAGAIN)
                                        अनुवर्त;
                                प्रत्यागम -errno;
                        }

                        प्रतिज्ञा((size_t) l <= परिमाण(buf));
                        buf_full = l;
                }

                size_t processed;
                r = scan_background_color_response(&context, buf, buf_full, &processed);
                यदि (r < 0)
                        प्रत्यागम r;

                प्रतिज्ञा(processed <= buf_full);
                buf_full -= processed;
                memmove(buf, buf + processed, buf_full);

                यदि (r > 0) {
                        प्रतिज्ञा(context.red_bits > 0);
                        *ret_red = (द्विगुण) context.red / ((UINT64_C(1) << context.red_bits) - 1);
                        प्रतिज्ञा(context.green_bits > 0);
                        *ret_green = (द्विगुण) context.green / ((UINT64_C(1) << context.green_bits) - 1);
                        प्रतिज्ञा(context.blue_bits > 0);
                        *ret_blue = (द्विगुण) context.blue / ((UINT64_C(1) << context.blue_bits) - 1);
                        प्रत्यागम 0;
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
                पूर्णाङ्क nonblock_निवेश_fd,
                पूर्णाङ्क output_fd,
                unsigned *ret_rows,
                unsigned *ret_columns) {

        पूर्णाङ्क r;

        प्रतिज्ञा(nonblock_निवेश_fd >= 0);
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
        यदि (r < 0)
                प्रत्यागम r;

        usec_t end = usec_add(now(CLOCK_MONOTONIC), CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        वर्ण buf[STRLEN("\x1B[1;1R")]; /* The shortest valid reply possible */
        size_t buf_full = 0;
        CursorPositionContext context = {};

        प्रति (bool first = true;; first = false) {
                यदि (buf_full == 0) {
                        usec_t n = now(CLOCK_MONOTONIC);
                        यदि (n >= end)
                                प्रत्यागम -EOPNOTSUPP;

                        r = fd_wait_for_event(nonblock_निवेश_fd, POLLIN, usec_sub_unsigned(end, n));
                        यदि (r < 0)
                                प्रत्यागम r;
                        यदि (r == 0)
                                प्रत्यागम -EOPNOTSUPP;

                        /* On the first try, read multiple characters, i.e. the shortest valid
                         * reply. Afterwards read byte-wise, since we don't want to read too much, and
                         * unnecessarily drop too many characters from the input queue. */
                        ssize_t l = पठ(nonblock_निवेश_fd, buf, first ? परिमाण(buf) : 1);
                        यदि (l < 0) {
                                यदि (errno == EAGAIN)
                                        अनुवर्त;

                                प्रत्यागम -errno;
                        }

                        प्रतिज्ञा((size_t) l <= परिमाण(buf));
                        buf_full = l;
                }

                size_t processed;
                r = scan_cursor_position_response(&context, buf, buf_full, &processed);
                यदि (r < 0)
                        प्रत्यागम r;

                प्रतिज्ञा(processed <= buf_full);
                buf_full -= processed;
                memmove(buf, buf + processed, buf_full);

                यदि (r > 0) {
                        /* Superficial validity checks (no particular reason to check for < 4, it's
                         * just a way to look for unreasonably small values) */
                        यदि (context.row < 4 || context.column < 4 || context.row >= 32766 || context.column >= 32766)
                                प्रत्यागम -ENODATA;

                        यदि (ret_rows)
                                *ret_rows = context.row;
                        यदि (ret_columns)
                                *ret_columns = context.column;

                        प्रत्यागम 0;
                }
        }
}

/* Common setup for ANSI terminal queries: validate the fds, open a non-blocking input fd, and configure
 * termios with echo and canonical mode disabled. Caller must restore termios and close the fd when done. */
स्थिर पूर्णाङ्क terminal_prepare_query(
                पूर्णाङ्क निवेश_fd,
                पूर्णाङ्क output_fd,
                पूर्णाङ्क *ret_nonblock_fd,
                संरचना termios *ret_saved_termios) {

        पूर्णाङ्क r;

        प्रतिज्ञा(निवेश_fd >= 0);
        प्रतिज्ञा(output_fd >= 0);
        प्रतिज्ञा(ret_nonblock_fd);
        प्रतिज्ञा(ret_saved_termios);

        /* Use getenv_terminal_is_dumb() instead of terminal_is_dumb() here since we operate on an
         * explicitly passed fd, not on stdio. terminal_is_dumb() additionally checks on_tty() which
         * tests whether *stderr* is a tty — that's irrelevant when we're querying a directly opened
         * terminal such as /dev/console. */
        यदि (getenv_terminal_is_dumb())
                प्रत्यागम -EOPNOTSUPP;

        r = terminal_verify_same(निवेश_fd, output_fd);
        यदि (r < 0)
                प्रत्यागम log_debug_errno(r, "Called with distinct input/output fds: %m");

        /* Open a 2nd input fd, in non-blocking mode, so that we won't ever hang in read()
         * should someone else process the POLLIN. Do all subsequent operations on the new fd. */
        _cleanup_close_ पूर्णाङ्क nonblock_निवेश_fd = r = fd_reopen(निवेश_fd, O_RDONLY|O_CLOEXEC|O_NONBLOCK|O_NOCTTY);
        यदि (r < 0)
                प्रत्यागम r;

        यदि (tcgetattr(nonblock_निवेश_fd, ret_saved_termios) < 0)
                प्रत्यागम log_debug_errno(errno, "Failed to get terminal settings: %m");

        संरचना termios new_termios = *ret_saved_termios;
        termios_disable_echo(&new_termios);

        यदि (tcsetattr(nonblock_निवेश_fd, TCSANOW, &new_termios) < 0)
                प्रत्यागम log_debug_errno(errno, "Failed to set new terminal settings: %m");

        *ret_nonblock_fd = TAKE_FD(nonblock_निवेश_fd);
        प्रत्यागम 0;
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
        यदि (size < STRLEN(CSI18_R0))
                प्रत्यागम -EAGAIN;

        /* Check if the terminating sequence is present */
        यदि (buf[size - 1] != 't')
                प्रत्यागम -EAGAIN;

        unsigned short rows, columns;
        यदि (sscanf(buf, CSI18_Rp "%hu;%hut", &rows, &columns) != 2)
                प्रत्यागम -EINVAL;

        *ret_rows = rows;
        *ret_columns = columns;
        प्रत्यागम 0;
}

/* Determine terminal dimensions by means of an ANSI CSI 18 sequence.
 *
 * Caller must have already opened a non-blocking input fd and configured termios (echo/icanon off). */
स्थिर पूर्णाङ्क terminal_query_size_by_csi18(
                पूर्णाङ्क nonblock_निवेश_fd,
                पूर्णाङ्क output_fd,
                unsigned *ret_rows,
                unsigned *ret_columns) {

        पूर्णाङ्क r;

        प्रतिज्ञा(nonblock_निवेश_fd >= 0);
        प्रतिज्ञा(output_fd >= 0);

        r = loop_write_full(output_fd, CSI18_Q, SIZE_MAX, CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        यदि (r < 0)
                प्रत्यागम r;

        usec_t end = usec_add(now(CLOCK_MONOTONIC), CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        वर्ण buf[STRLEN(CSI18_R1)];
        size_t bytes = 0;

        प्रति (;;) {
                usec_t n = now(CLOCK_MONOTONIC);
                यदि (n >= end)
                        प्रत्यागम -EOPNOTSUPP;

                r = fd_wait_for_event(nonblock_निवेश_fd, POLLIN, usec_sub_unsigned(end, n));
                यदि (r < 0)
                        प्रत्यागम r;
                यदि (r == 0)
                        प्रत्यागम -EOPNOTSUPP;

                /* On the first read, read multiple characters, i.e. the shortest valid reply. Afterwards
                 * read byte by byte, since we don't want to read too much and drop characters from the input
                 * queue. */
                ssize_t l = पठ(nonblock_निवेश_fd, buf + bytes, bytes == 0 ? STRLEN(CSI18_R0) : 1);
                यदि (l < 0) {
                        यदि (errno == EAGAIN)
                                अनुवर्त;
                        प्रत्यागम -errno;
                }

                प्रतिज्ञा((size_t) l <= परिमाण(buf) - bytes);
                bytes += l;

                r = scan_text_area_size_response(buf, bytes, ret_rows, ret_columns);
                यदि (r != -EAGAIN)
                        प्रत्यागम r;

                यदि (bytes == परिमाण(buf))
                        प्रत्यागम -EOPNOTSUPP; /* The response has the right prefix, but we didn't find a valid
                                             * answer with a terminator in the allotted space. Something is
                                             * wrong, possibly some unrelated bytes got injected into the
                                             * answer. */
        }
}

पूर्णाङ्क terminal_get_size(
                पूर्णाङ्क निवेश_fd,
                पूर्णाङ्क output_fd,
                unsigned *ret_rows,
                unsigned *ret_columns,
                bool try_dsr,
                bool try_csi18) {

        _cleanup_close_ पूर्णाङ्क nonblock_निवेश_fd = -EBADF;
        संरचना termios old_termios = TERMIOS_NULL;
        CLEANUP_TERMIOS_RESET(nonblock_निवेश_fd, old_termios);
        _cleanup_(nonblock_resetp) पूर्णाङ्क nonblock_reset = -EBADF;
        पूर्णाङ्क r;

        प्रतिज्ञा(try_dsr || try_csi18);

        r = terminal_prepare_query(निवेश_fd, output_fd, &nonblock_निवेश_fd, &old_termios);
        यदि (r < 0)
                प्रत्यागम r;

        /* Put the output fd in non-blocking mode with a write timeout, to avoid blocking indefinitely on
         * write if the terminal is not consuming data (e.g. serial console with flow control). */
        r = fd_nonblock(output_fd, true);
        यदि (r < 0)
                प्रत्यागम log_debug_errno(r, "Failed to set terminal to non-blocking mode: %m");
        यदि (r > 0)
                nonblock_reset = output_fd;

        /* Flush any stale input that might confuse the response parsers. */
        (शून्य) tcflush(nonblock_निवेश_fd, TCIFLUSH);

        यदि (try_csi18) {
                r = terminal_query_size_by_csi18(nonblock_निवेश_fd, output_fd, ret_rows, ret_columns);
                यदि (r >= 0)
                        प्रत्यागम r;

                /* Query failed. Flush any outstanding input. */
                (शून्य) tcflush(nonblock_निवेश_fd, TCIFLUSH);

                यदि (!IN_SET(r, -EOPNOTSUPP, -EINVAL))
                        प्रत्यागम r;
        }

        यदि (try_dsr) {
                r = terminal_query_size_by_dsr(nonblock_निवेश_fd, output_fd, ret_rows, ret_columns);
                यदि (r >= 0)
                        प्रत्यागम r;

                /* Query failed. Flush any outstanding input. */
                (शून्य) tcflush(nonblock_निवेश_fd, TCIFLUSH);
        }

        प्रत्यागम r;
}

स्थिर पूर्णाङ्क fd_is_vc(पूर्णाङ्क fd) {
        _cleanup_free_ वर्ण *name = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(fd >= 0);

        r = getttyname_harder(fd, &name);
        यदि (r < 0)
                प्रत्यागम r;

        प्रत्यागम tty_is_vc_resolve(name);
}

पूर्णाङ्क terminal_fix_size(पूर्णाङ्क निवेश_fd, पूर्णाङ्क output_fd) {
        unsigned rows, columns;
        पूर्णाङ्क r;

        /* Don't ask a virtual console at all. There is no intermediary here that could be wrong about the
         * dimensions: the kernel renders the screen itself, so its winsize is authoritative. */
        r = fd_is_vc(निवेश_fd);
        यदि (r < 0)
                log_debug_errno(r, "Failed to determine whether terminal is a virtual console, assuming it is not: %m");
        अन्यथा यदि (r > 0) {
                log_debug("Not querying dimensions of a virtual console, kernel dimensions are authoritative.");
                प्रत्यागम 0;
        }

        संरचना winsize ws = {};
        यदि (ioctl(output_fd, TIOCGWINSZ, &ws) < 0)
                प्रत्यागम log_debug_errno(errno, "Failed to query terminal dimensions, ignoring: %m");

        r = terminal_get_size(निवेश_fd, output_fd, &rows, &columns, /* try_dsr= */ true, /* try_csi18= */ true);
        यदि (r < 0)
                प्रत्यागम log_debug_errno(r, "Failed to acquire terminal dimensions via ANSI sequences, not adjusting terminal dimensions: %m");

        यदि (ws.ws_row == rows && ws.ws_col == columns) {
                log_debug("Terminal dimensions reported via ANSI sequences match currently set terminal dimensions, not changing.");
                प्रत्यागम 0;
        }

        ws.ws_col = columns;
        ws.ws_row = rows;

        यदि (ioctl(output_fd, TIOCSWINSZ, &ws) < 0)
                प्रत्यागम log_debug_errno(errno, "Failed to update terminal dimensions, ignoring: %m");

        log_debug("Fixed terminal dimensions to %ux%u based on ANSI sequence information.", columns, rows);
        प्रत्यागम 1;
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
        यदि (size < STRLEN(DCS_TERMINFO_R0))
                प्रत्यागम -EAGAIN;

        /* Check if the terminating sequence is present */
        यदि (memcmp(buf + size - STRLEN(ANSI_ST), ANSI_ST, STRLEN(ANSI_ST)) != 0)
                प्रत्यागम -EAGAIN;

        यदि (size <= STRLEN(DCS_TERMINFO_R1 ANSI_ST))
                प्रत्यागम -EINVAL;  /* The answer is invalid or empty */

        यदि (memcmp(buf, DCS_TERMINFO_R1, STRLEN(DCS_TERMINFO_R1)) != 0)
                प्रत्यागम -EINVAL;  /* The answer is not valid */

        _cleanup_free_ शून्य *dec = रिक्त;
        size_t dec_size;
        r = unhexmem_full(buf + STRLEN(DCS_TERMINFO_R1), size - STRLEN(DCS_TERMINFO_R1 ANSI_ST),
                          /* secure= */ false,
                          &dec, &dec_size);
        यदि (r < 0)
                प्रत्यागम r;

        प्रतिज्ञा(((ध्रुव वर्ण *) dec)[dec_size] == '\0'); /* unhexmem appends NUL for our convenience */
        यदि (memchr(dec, '\0', dec_size) || string_has_cc(dec, रिक्त) || !filename_is_valid(dec))
                प्रत्यागम -EUCLEAN;

        *ret_name = TAKE_PTR(dec);
        प्रत्यागम 0;
}

पूर्णाङ्क terminal_get_terminfo_by_dcs(पूर्णाङ्क fd, वर्ण **ret_name) {
        पूर्णाङ्क r;

        प्रतिज्ञा(fd >= 0);
        प्रतिज्ञा(ret_name);

        /* Note: fd must be in non-blocking read-write mode! */

        संरचना termios old_termios = TERMIOS_NULL;
        CLEANUP_TERMIOS_RESET(fd, old_termios);

        यदि (tcgetattr(fd, &old_termios) < 0)
                प्रत्यागम -errno;

        संरचना termios new_termios = old_termios;
        termios_disable_echo(&new_termios);

        यदि (tcsetattr(fd, TCSANOW, &new_termios) < 0)
                प्रत्यागम -errno;

        r = loop_write(fd, DCS_TERMINFO_Q, SIZE_MAX);
        यदि (r < 0)
                प्रत्यागम r;

        usec_t end = usec_add(now(CLOCK_MONOTONIC), CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        वर्ण buf[STRLEN(DCS_TERMINFO_R1) + MAX_TERMINFO_LENGTH + STRLEN(ANSI_ST)];
        size_t bytes = 0;

        प्रति (;;) {
                usec_t n = now(CLOCK_MONOTONIC);
                यदि (n >= end)
                        प्रत्यागम -EOPNOTSUPP;

                r = fd_wait_for_event(fd, POLLIN, usec_sub_unsigned(end, n));
                यदि (r < 0)
                        प्रत्यागम r;
                यदि (r == 0)
                        प्रत्यागम -EOPNOTSUPP;

                /* On the first read, read multiple characters, i.e. the shortest valid reply. Afterwards
                 * read byte by byte, since we don't want to read too much and drop characters from the input
                 * queue. */
                ssize_t l = पठ(fd, buf + bytes, bytes == 0 ? STRLEN(DCS_TERMINFO_R0) : 1);
                यदि (l < 0) {
                        यदि (errno == EAGAIN)
                                अनुवर्त;
                        प्रत्यागम -errno;
                }

                प्रतिज्ञा((size_t) l <= परिमाण(buf) - bytes);
                bytes += l;

                r = scan_terminfo_response(buf, bytes, ret_name);
                यदि (r != -EAGAIN)
                        प्रत्यागम r;

                यदि (bytes == परिमाण(buf))
                        प्रत्यागम -EOPNOTSUPP; /* The response has the right prefix, but we didn't find a valid
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
        यदि (!p)
                प्रत्यागम log_oom_debug();

        r = RET_NERRNO(access(p, F_OK));
        यदि (r == -ENOENT)
                प्रत्यागम false;
        यदि (r < 0)
                प्रत्यागम r;
        प्रत्यागम true;
}

पूर्णाङ्क query_term_for_tty(ध्रुव वर्ण *tty, वर्ण **ret_term) {
        _cleanup_free_ वर्ण *dcs_term = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(tty);
        प्रतिज्ञा(ret_term);

        यदि (tty_is_vc_resolve(tty))
                प्रत्यागम strdup_to(ret_term, "linux");

        /* Try to query the terminal implementation that we're on. This will not work in all
         * cases, which is fine, since this is intended to be used as a fallback. */

        _cleanup_close_ पूर्णाङ्क tty_fd = open_terminal(tty, O_RDWR|O_NOCTTY|O_CLOEXEC|O_NONBLOCK);
        यदि (tty_fd < 0)
                प्रत्यागम log_debug_errno(tty_fd, "Failed to open %s to query terminfo: %m", tty);

        r = terminal_get_terminfo_by_dcs(tty_fd, &dcs_term);
        यदि (r < 0)
                प्रत्यागम log_debug_errno(r, "Failed to query %s for terminfo: %m", tty);

        r = have_terminfo_file(dcs_term);
        यदि (r < 0)
                प्रत्यागम log_debug_errno(r, "Failed to look for terminfo %s: %m", dcs_term);
        यदि (r == 0)
                प्रत्यागम log_info_errno(SYNTHETIC_ERRNO(ENODATA),
                                      "Terminfo %s not found for %s.", dcs_term, tty);

        *ret_term = TAKE_PTR(dcs_term);
        प्रत्यागम 0;
}

पूर्णाङ्क terminal_is_pty_fd(पूर्णाङ्क fd) {
        पूर्णाङ्क r;

        प्रतिज्ञा(fd >= 0);

        /* Returns true if we are looking at a pty, i.e. if it's backed by the /dev/pts/ file system */

        यदि (!isatty_safe(fd))
                प्रत्यागम false;

        r = is_fs_type_at(fd, रिक्त, DEVPTS_SUPER_MAGIC);
        यदि (r != 0)
                प्रत्यागम r;

        /* The ptmx device is weird, it exists twice, once inside and once outside devpts. To detect the
         * latter case, let's fire off an ioctl() that only works on ptmx devices. */

        पूर्णाङ्क v;
        यदि (ioctl(fd, TIOCGPKT, &v) < 0) {
                यदि (ERRNO_IS_NOT_SUPPORTED(errno))
                        प्रत्यागम false;

                प्रत्यागम -errno;
        }

        प्रत्यागम true;
}

पूर्णाङ्क pty_open_peer(पूर्णाङ्क fd, पूर्णाङ्क mode) {
        प्रतिज्ञा(fd >= 0);

        /* Opens the peer PTY using the new race-free TIOCGPTPEER ioctl() (kernel 4.13).
         *
         * This is safe to be called on TTYs from other namespaces. */

        प्रतिज्ञा((mode & (O_CREAT|O_PATH|O_DIRECTORY|O_TMPFILE)) == 0);

        /* This replicates the EIO retry logic of open_terminal() in a modified way. */
        प्रति (unsigned c = 0;; c++) {
                पूर्णाङ्क peer_fd = ioctl(fd, TIOCGPTPEER, mode);
                यदि (peer_fd >= 0)
                        प्रत्यागम peer_fd;

                यदि (errno != EIO)
                        प्रत्यागम -errno;

                /* Max 1s in total */
                यदि (c >= 20)
                        प्रत्यागम -EIO;

                (शून्य) usleep_safe(50 * USEC_PER_MSEC);
        }
}
