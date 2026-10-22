/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ <fcntl.h>
#அந்தர்பாவ <linux/kd.h>
#அந்தர்பாவ <linux/magic.h>
#அந்தர்பாவ <linux/tiocl.h>
#அந்தர்பாவ <linux/vt.h>
#அந்தர்பாவ <poll.h>
#அந்தர்பாவ <signal.h>
#அந்தர்பாவ <stdlib.h>
#அந்தர்பாவ <sys/inotify.h>
#அந்தர்பாவ <sys/ioctl.h>
#அந்தர்பாவ <sys/sysmacros.h>
#அந்தர்பாவ <termios.h>
#அந்தர்பாவ <time.h>
#அந்தர்பாவ <unistd.h>

#அந்தர்பாவ "alloc-util.h"
#அந்தர்பாவ "ansi-color.h"
#அந்தர்பாவ "chase.h"
#அந்தர்பாவ "devnum-util.h"
#அந்தர்பாவ "errno-util.h"
#அந்தர்பாவ "extract-word.h"
#அந்தர்பாவ "fd-util.h"
#அந்தர்பாவ "fileio.h"
#அந்தர்பாவ "fs-util.h"
#அந்தர்பாவ "hexdecoct.h"
#அந்தர்பாவ "inotify-util.h"
#அந்தர்பாவ "io-util.h"
#அந்தர்பாவ "log.h"
#அந்தர்பாவ "namespace-util.h"
#அந்தர்பாவ "parse-util.h"
#அந்தர்பாவ "path-util.h"
#அந்தர்பாவ "pidref.h"
#அந்தர்பாவ "proc-cmdline.h"
#அந்தர்பாவ "process-util.h"
#அந்தர்பாவ "signal-util.h"
#அந்தர்பாவ "socket-util.h"
#அந்தர்பாவ "stat-util.h"
#அந்தர்பாவ "stdio-util.h"
#அந்தர்பாவ "string-util.h"
#அந்தர்பாவ "strv.h"
#அந்தர்பாவ "terminal-util.h"
#அந்தர்பாவ "time-util.h"
#அந்தர்பாவ "utf8.h"

/* How much to wait when reading/writing ANSI sequences from/to the console */
#define CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC (333 * USEC_PER_MSEC)

நிலையான நிலையற்ற unsigned cached_columns = 0;
நிலையான நிலையற்ற unsigned cached_வரிகள் = 0;

நிலையான நிலையற்ற முழு cached_on_tty = -1;
நிலையான நிலையற்ற முழு cached_on_dev_null = -1;

bool isatty_safe(முழு fd) {
        ப்ரதிஜ்ஞா(fd >= 0);

        எனில் (isatty(fd))
                திரும்பு true;

        /* Linux/glibc returns EIO for hung up TTY on isatty(). Which is wrong, the thing doesn't stop being
         * a TTY after all, just because it is temporarily hung up. Let's work around this here, until this
         * is fixed in glibc. See: https://sourceware.org/bugzilla/show_bug.cgi?id=32103 */
        எனில் (errno == EIO)
                திரும்பு true;

        /* Be resilient if we're working on stdio, since they're set up by parent process. */
        ப்ரதிஜ்ஞா(errno != EBADF || IN_SET(fd, STDIN_FILENO, STDOUT_FILENO, STDERR_FILENO));

        திரும்பு false;
}

முழு chvt(முழு vt) {
        _cleanup_close_ முழு fd = -EBADF;

        /* Switch to the specified vt number. If the VT is specified <= 0 switch to the VT the kernel log messages go,
         * if that's configured. */

        fd = open_terminal("/dev/tty0", O_RDWR|O_NOCTTY|O_CLOEXEC|O_NONBLOCK);
        எனில் (fd < 0)
                திரும்பு fd;

        எனில் (vt <= 0) {
                முழு tiocl[2] = {
                        TIOCL_GETKMSGREDIRECT,
                        0
                };

                எனில் (ioctl(fd, TIOCLINUX, tiocl) < 0)
                        திரும்பு -errno;

                vt = tiocl[0] <= 0 ? 1 : tiocl[0];
        }

        திரும்பு RET_NERRNO(ioctl(fd, VT_ACTIVATE, vt));
}

முழு read_one_char(FILE *f, எழுத்து *ret, usec_t t, bool echo, bool *need_nl) {
        _cleanup_free_ எழுத்து *வரி = வெற்று;
        கட்டமைப்பு termios old_termios;
        முழு r, fd;

        ப்ரதிஜ்ஞா(ret);

        எனில் (!f)
                f = stdin;

        /* If this is a terminal, then switch canonical mode off, so that we can read a single
         * character. (Note that fmemopen() streams do not have an fd associated with them, let's handle that
         * nicely.) If 'echo' is false we'll also disable ECHO mode so that the pressed key is not made
         * visible to the user. */
        fd = fileno(f);
        எனில் (fd >= 0 && tcgetattr(fd, &old_termios) >= 0) {
                கட்டமைப்பு termios new_termios = old_termios;

                new_termios.c_lflag &= ~(ICANON|(echo ? 0 : ECHO));
                new_termios.c_cc[VMIN] = 1;
                new_termios.c_cc[VTIME] = 0;

                எனில் (tcsetattr(fd, TCSANOW, &new_termios) >= 0) {
                        எழுத்து c;

                        எனில் (t != USEC_INFINITY) {
                                எனில் (fd_wait_for_event(fd, POLLIN, t) <= 0) {
                                        (வெறுமை) tcsetattr(fd, TCSANOW, &old_termios);
                                        திரும்பு -ETIMEDOUT;
                                }
                        }

                        r = safe_fgetc(f, &c);
                        (வெறுமை) tcsetattr(fd, TCSANOW, &old_termios);
                        எனில் (r < 0)
                                திரும்பு r;
                        எனில் (r == 0)
                                திரும்பு -EIO;

                        எனில் (need_nl)
                                *need_nl = c != '\n';

                        *ret = c;
                        திரும்பு 0;
                }
        }

        எனில் (t != USEC_INFINITY && fd >= 0) {
                /* Let's wait the specified amount of time for input. When we have no fd we skip this, under
                 * the assumption that this is an fmemopen() stream or so where waiting doesn't make sense
                 * anyway, as the data is either already in the stream or cannot possible be placed there
                 * while we access the stream */

                எனில் (fd_wait_for_event(fd, POLLIN, t) <= 0)
                        திரும்பு -ETIMEDOUT;
        }

        /* If this is not a terminal, then read a full line instead */

        r = read_வரி(f, 16, &வரி); /* longer than necessary, to eat up UTF-8 chars/vt100 key sequences */
        எனில் (r < 0)
                திரும்பு r;
        எனில் (r == 0)
                திரும்பு -EIO;

        எனில் (strlen(வரி) != 1)
                திரும்பு -EBADMSG;

        எனில் (need_nl)
                *need_nl = false;

        *ret = வரி[0];
        திரும்பு 0;
}

#define DEFAULT_ASK_REFRESH_USEC (2*USEC_PER_SEC)

முழு ask_char(எழுத்து *ret, மாறிலி எழுத்து *replies, மாறிலி எழுத்து *fmt, ...) {
        முழு r;

        ப்ரதிஜ்ஞா(ret);
        ப்ரதிஜ்ஞா(replies);
        ப்ரதிஜ்ஞா(fmt);

        க்கு (;;) {
                va_list ap;
                எழுத்து c;
                bool need_nl = true;

                fputs(ansi_highlight(), stdout);

                putchar('\r');

                va_start(ap, fmt);
                vprintf(fmt, ap);
                va_end(ap);

                fputs(ansi_normal(), stdout);

                fflush(stdout);

                r = read_one_char(stdin, &c, DEFAULT_ASK_REFRESH_USEC, /* echo= */ true, &need_nl);
                எனில் (r < 0) {

                        எனில் (r == -ETIMEDOUT)
                                தொடர்;

                        எனில் (r == -EBADMSG) {
                                puts("Bad input, please try again.");
                                தொடர்;
                        }

                        putchar('\n');
                        திரும்பு r;
                }

                எனில் (need_nl)
                        putchar('\n');

                எனில் (strchr(replies, c)) {
                        *ret = c;
                        திரும்பு 0;
                }

                puts("Read unexpected character, please try again.");
        }
}

வகைவரை எண்ணிக்கை CompletionResult{
        COMPLETION_ALREADY,       /* the input string is already complete */
        COMPLETION_FULL,          /* completed the input string to be complete now */
        COMPLETION_PARTIAL,       /* completed the input string so that is still incomplete */
        COMPLETION_NONE,          /* found no matching completion */
        _COMPLETION_RESULT_MAX,
        _COMPLETION_RESULT_INVALID = -EINVAL,
        _COMPLETION_RESULT_ERRNO_MAX = -ERRNO_MAX,
} CompletionResult;

நிலையான CompletionResult pick_completion(மாறிலி எழுத்து *string, எழுத்து *மாறிலி*completions, எழுத்து **ret) {
        _cleanup_free_ எழுத்து *found = வெற்று;
        bool partial = false;

        ப்ரதிஜ்ஞா(ret);

        string = strempty(string);

        STRV_FOREACH(c, completions) {

                /* Ignore entries that are not actually completions */
                எனில் (!startswith(*c, string))
                        தொடர்;

                /* Store first completion that matches */
                எனில் (!found) {
                        found = strdup(*c);
                        எனில் (!found)
                                திரும்பு -ENOMEM;

                        தொடர்;
                }

                /* If there's another completion that works truncate the one we already found by common
                 * prefix */
                size_t n = str_common_prefix(found, *c);
                எனில் (n == SIZE_MAX)
                        தொடர்;

                found[n] = 0;
                partial = true;
        }

        *ret = TAKE_PTR(found);

        எனில் (!*ret)
                திரும்பு COMPLETION_NONE;
        எனில் (partial)
                திரும்பு COMPLETION_PARTIAL;

        திரும்பு streq(string, *ret) ? COMPLETION_ALREADY : COMPLETION_FULL;
}

நிலையான வெறுமை clear_by_backspace(size_t n) {
        /* Erase the specified number of character cells backwards on the terminal */
        க்கு (size_t i = 0; i < n; i++)
                fputs("\b \b", stdout);
}

முழு ask_string_full(
                எழுத்து **ret,
                மாறிலி எழுத்து *prefill,
                GetCompletionsCallback get_completions,
                வெறுமை *userdata,
                மாறிலி எழுத்து *text, ...) {

        va_list ap;
        முழு r;

        ப்ரதிஜ்ஞா(ret);
        ப்ரதிஜ்ஞா(text);

        _cleanup_free_ எழுத்து *string = வெற்று;
        size_t n = 0;

        எனில் (prefill) {
                /* Prefill query with explicit data if specified */

                string = strdup(prefill);
                எனில் (!string)
                        திரும்பு -ENOMEM;

                n = strlen(string);

        } இல்லையெனில் எனில் (get_completions) {
                /* Otherwise, figure out what string to preselect the query with */
                _cleanup_strv_free_ எழுத்து **completions = வெற்று;
                r = get_completions("", GET_COMPLETIONS_PRESELECT, &completions, userdata);
                எனில் (r < 0)
                        திரும்பு r;

                CompletionResult cr = pick_completion(string, completions, &string);
                எனில் (cr < 0)
                        திரும்பு cr;

                n = strlen_ptr(string);
        }

        /* Output the prompt */
        fputs(ansi_highlight(), stdout);
        va_start(ap, text);
        vprintf(text, ap);
        va_end(ap);
        fputs(ansi_normal(), stdout);
        எனில் (string)
                fputs(string, stdout);
        fflush(stdout);

        /* Do interactive logic only if stdin + stdout are connected to the same place. And yes, we could use
         * STDIN_FILENO and STDOUT_FILENO here, but let's be overly correct for once, after all libc allows
         * swapping out stdin/stdout. */
        முழு fd_உள்ளீடு = fileno(stdin);
        முழு fd_output = fileno(stdout);
        கட்டமைப்பு termios old_termios = TERMIOS_NULL;
        CLEANUP_TERMIOS_RESET(fd_உள்ளீடு, old_termios);

        எனில் (fd_உள்ளீடு < 0 || fd_output < 0 || same_fd(fd_உள்ளீடு, fd_output) <= 0)
                செல் fallback;

        /* Try to disable echo, which also tells us if this even is a terminal */
        எனில் (tcgetattr(fd_உள்ளீடு, &old_termios) < 0) {
                old_termios = TERMIOS_NULL;
                செல் fallback;
        }

        கட்டமைப்பு termios new_termios = old_termios;
        termios_disable_echo(&new_termios);
        எனில் (tcsetattr(fd_உள்ளீடு, TCSANOW, &new_termios) < 0)
                திரும்பு -errno;

        க்கு (;;) {
                முழு c = fgetc(stdin);

                /* On EOF or NUL, end the request, don't output anything anymore */
                எனில் (IN_SET(c, EOF, 0))
                        நிறுத்து;

                /* On Return also end the request, but make this visible */
                எனில் (IN_SET(c, '\n', '\r')) {
                        fputc('\n', stdout);
                        நிறுத்து;
                }

                எனில் (c == '\t') {
                        /* Tab */

                        _cleanup_strv_free_ எழுத்து **completions = வெற்று;
                        எனில் (get_completions) {
                                r = get_completions(string, /* flags= */ 0, &completions, userdata);
                                எனில் (r < 0)
                                        திரும்பு r;
                        }

                        _cleanup_free_ எழுத்து *new_string = வெற்று;
                        CompletionResult cr = pick_completion(string, completions, &new_string);
                        எனில் (cr < 0)
                                திரும்பு cr;
                        எனில் (IN_SET(cr, COMPLETION_PARTIAL, COMPLETION_FULL)) {
                                /* Output the new suffix we learned */
                                fputs(ASSERT_PTR(startswith(new_string, strempty(string))), stdout);

                                /* And update the whole string */
                                free_and_replace(string, new_string);
                                n = strlen(string);
                        }
                        எனில் (cr == COMPLETION_NONE)
                                fputc('\a', stdout); /* BEL */

                        எனில் (IN_SET(cr, COMPLETION_PARTIAL, COMPLETION_ALREADY)) {
                                /* If this worked only partially, or if the user hit TAB even though we were
                                 * complete already, then show the remaining options (in the latter case just
                                 * the one). */
                                fputc('\n', stdout);

                                _cleanup_strv_free_ எழுத்து **filtered = strv_filter_prefix(completions, string);
                                எனில் (!filtered)
                                        திரும்பு -ENOMEM;

                                r = show_menu(filtered,
                                              /* n_columns= */ SIZE_MAX,
                                              /* column_width= */ SIZE_MAX,
                                              /* ellipsize_percentage= */ 0,
                                              /* grey_prefix= */ string,
                                              /* with_numbers= */ false);
                                எனில் (r < 0)
                                        திரும்பு r;

                                /* Show the prompt again */
                                fputs(ansi_highlight(), stdout);
                                va_start(ap, text);
                                vprintf(text, ap);
                                va_end(ap);
                                fputs(ansi_normal(), stdout);
                                fputs(string, stdout);
                        }

                } இல்லையெனில் எனில் (IN_SET(c, '\b', 127)) {
                        /* Backspace */

                        எனில் (n == 0)
                                fputc('\a', stdout); /* BEL */
                        இல்லையெனில் {
                                size_t m = utf8_last_length(string, n);

                                எழுத்து *e = string + n - m;
                                clear_by_backspace(utf8_console_width(e));

                                *e = 0;
                                n -= m;
                        }

                } இல்லையெனில் எனில் (c == 21) {
                        /* Ctrl-u → erase all input */

                        clear_by_backspace(utf8_console_width(string));
                        எனில் (string)
                                string[n = 0] = 0;
                        இல்லையெனில்
                                ப்ரதிஜ்ஞா(n == 0);

                } இல்லையெனில் எனில் (c == 4) {
                        /* Ctrl-d → cancel this field input */

                        திரும்பு -ECANCELED;

                } இல்லையெனில் எனில் (char_is_cc(c) || n >= LINE_MAX)
                        /* refuse control characters and too long strings */
                        fputc('\a', stdout); /* BEL */
                இல்லையெனில் {
                        /* Regular char */

                        எனில் (!GREEDY_REALLOC(string, n+2))
                                திரும்பு -ENOMEM;

                        string[n++] = (எழுத்து) c;
                        string[n] = 0;

                        fputc(c, stdout);
                }

                fflush(stdout);
        }

        எனில் (!string) {
                string = strdup("");
                எனில் (!string)
                        திரும்பு -ENOMEM;
        }

        *ret = TAKE_PTR(string);
        திரும்பு 0;

fallback:
        /* A simple fallback without TTY magic */
        string = mfree(string);
        r = read_வரி(stdin, LONG_வரி_MAX, &string);
        எனில் (r < 0)
                திரும்பு r;
        எனில் (r == 0)
                திரும்பு -EIO;

        *ret = TAKE_PTR(string);
        திரும்பு 0;
}

bool any_சாவி_to_proceed(வெறுமை) {

        /* Insert a new line here as well as to when the user inputs, as this is also used during the boot up
         * sequence when status messages may be interleaved with the current program output. This ensures
         * that the status messages aren't appended on the same line as this message. */

        fputc('\n', stdout);
        fputs(ansi_highlight_magenta(), stdout);
        fputs("-- Press any key to proceed --", stdout);
        fputs(ansi_normal(), stdout);
        fputc('\n', stdout);
        fflush(stdout);

        எழுத்து சாவி = 0;
        (வெறுமை) read_one_char(stdin, &சாவி, USEC_INFINITY, /* echo= */ false, /* need_nl= */ வெற்று);

        fputc('\n', stdout);
        fflush(stdout);

        திரும்பு சாவி != 'q';
}

நிலையான size_t widest_list_element(எழுத்து *மாறிலி*l) {
        size_t w = 0;

        /* Returns the largest console width of all elements in 'l' */

        STRV_FOREACH(i, l)
                w = MAX(w, utf8_console_width(*i));

        திரும்பு w;
}

முழு show_menu(எழுத்து **x,
              size_t n_columns,
              size_t column_width,
              unsigned ellipsize_percentage,
              மாறிலி எழுத்து *grey_prefix,
              bool with_numbers) {

        ப்ரதிஜ்ஞா(n_columns > 0);
        ப்ரதிஜ்ஞா(column_width > 0);

        எனில் (n_columns == SIZE_MAX)
                n_columns = 3;

        எனில் (column_width == SIZE_MAX) {
                size_t widest = widest_list_element(x);

                /* If not specified, derive column width from screen width */
                size_t column_max = (columns()-1) / n_columns;

                /* Subtract room for numbers */
                எனில் (with_numbers)
                        column_max = LESS_BY(column_max, 6U);

                /* If columns would get too tight let's make this a linear list instead. */
                எனில் (column_max < 10 && widest > 10) {
                        n_columns = 1;
                        column_max = columns()-1;

                        எனில் (with_numbers)
                                column_max = LESS_BY(column_max, 6U);
                }

                /* Never make this narrower than 10 characters */
                column_max = MAX(column_max, 10U);
                column_width = CLAMP(widest+1, 10U, column_max);
        }

        size_t n = strv_length(x);
        size_t per_column = DIV_ROUND_UP(n, n_columns);

        size_t break_வரிகள் = வரிகள்();
        எனில் (break_வரிகள் > 2)
                break_வரிகள்--;

        /* The first page gets two extra lines, since we want to show a title */
        size_t break_modulo = break_வரிகள்;
        எனில் (break_modulo > 3)
                break_modulo -= 3;

        க்கு (size_t i = 0; i < per_column; i++) {

                க்கு (size_t j = 0; j < n_columns; j++) {
                        _cleanup_free_ எழுத்து *e = வெற்று;

                        size_t p = j * per_column + i;
                        எனில் (p >= n)
                                நிறுத்து;

                        e = ellipsize(x[p], column_width, ellipsize_percentage);
                        எனில் (!e)
                                திரும்பு -ENOMEM;

                        எனில் (with_numbers)
                                printf("%s%4zu)%s ",
                                       ansi_grey(),
                                       p + 1,
                                       ansi_normal());

                        எனில் (grey_prefix && startswith(e, grey_prefix)) {
                                size_t k = MIN(strlen(grey_prefix), column_width);
                                printf("%s%.*s%s",
                                       ansi_grey(),
                                       (முழு) k, e,
                                       ansi_normal());
                                printf("%-*s",
                                       (முழு) (column_width - k), e+k);
                        } இல்லையெனில்
                                printf("%-*s", (முழு) column_width, e);
                }

                putchar('\n');

                /* on the first screen we reserve 2 extra lines for the title */
                எனில் (i % break_வரிகள் == break_modulo)
                        எனில் (!any_சாவி_to_proceed())
                                திரும்பு 0;
        }

        திரும்பு 0;
}

முழு open_terminal(மாறிலி எழுத்து *name, முழு mode) {
        _cleanup_close_ முழு fd = -EBADF;

        /*
         * If a TTY is in the process of being closed opening it might cause EIO. This is horribly awful, but
         * unlikely to be changed in the kernel. Hence we work around this problem by retrying a couple of
         * times.
         *
         * https://bugs.launchpad.net/ubuntu/+source/linux/+bug/554172/comments/245
         */

        ப்ரதிஜ்ஞா((mode & (O_CREAT|O_PATH|O_DIRECTORY|O_TMPFILE)) == 0);

        க்கு (unsigned c = 0;; c++) {
                fd = open(name, mode, 0);
                எனில் (fd >= 0)
                        நிறுத்து;

                எனில் (errno != EIO)
                        திரும்பு -errno;

                /* Max 1s in total */
                எனில் (c >= 20)
                        திரும்பு -EIO;

                (வெறுமை) usleep_safe(50 * USEC_PER_MSEC);
        }

        எனில் (!isatty_safe(fd))
                திரும்பு -ENOTTY;

        திரும்பு TAKE_FD(fd);
}

முழு acquire_terminal(
                மாறிலி எழுத்து *name,
                AcquireTerminalFlags flags,
                usec_t timeout) {

        _cleanup_close_ முழு notify = -EBADF, fd = -EBADF;
        usec_t ts = USEC_INFINITY;
        முழு r, wd = -1;

        ப்ரதிஜ்ஞா(name);

        AcquireTerminalFlags mode = flags & _ACQUIRE_TERMINAL_MODE_MASK;
        ப்ரதிஜ்ஞா(IN_SET(mode, ACQUIRE_TERMINAL_TRY, ACQUIRE_TERMINAL_FORCE, ACQUIRE_TERMINAL_WAIT));
        ப்ரதிஜ்ஞா(mode == ACQUIRE_TERMINAL_WAIT || !FLAGS_SET(flags, ACQUIRE_TERMINAL_WATCH_SIGTERM));

        /* We use inotify to be notified when the tty is closed. We create the watch before checking if we can actually
         * acquire it, so that we don't lose any event.
         *
         * Note: strictly speaking this actually watches for the device being closed, it does *not* really watch
         * whether a tty loses its controlling process. However, unless some rogue process uses TIOCNOTTY on /dev/tty
         * *after* closing its tty otherwise this will not become a problem. As long as the administrator makes sure to
         * not configure any service on the same tty as an untrusted user this should not be a problem. (Which they
         * probably should not do anyway.) */

        எனில் (mode == ACQUIRE_TERMINAL_WAIT) {
                notify = inotify_init1(IN_CLOEXEC | IN_NONBLOCK);
                எனில் (notify < 0)
                        திரும்பு -errno;

                wd = inotify_add_watch(notify, name, IN_CLOSE);
                எனில் (wd < 0)
                        திரும்பு -errno;

                எனில் (timeout != USEC_INFINITY)
                        ts = now(CLOCK_MONOTONIC);
        }

        /* If we are called with ACQUIRE_TERMINAL_WATCH_SIGTERM we'll unblock SIGTERM during ppoll() temporarily */
        sigset_t poll_ss;
        assert_se(sigprocmask(SIG_SETMASK, /* newset= */ வெற்று, &poll_ss) >= 0);
        எனில் (flags & ACQUIRE_TERMINAL_WATCH_SIGTERM) {
                assert_se(sigismember(&poll_ss, SIGTERM) > 0);
                assert_se(sigdelset(&poll_ss, SIGTERM) >= 0);
        }

        க்கு (;;) {
                எனில் (notify >= 0) {
                        r = flush_fd(notify);
                        எனில் (r < 0)
                                திரும்பு r;
                }

                /* We pass here O_NOCTTY only so that we can check the return value TIOCSCTTY and have a reliable way
                 * to figure out if we successfully became the controlling process of the tty */
                fd = open_terminal(name, O_RDWR|O_NOCTTY|O_CLOEXEC);
                எனில் (fd < 0)
                        திரும்பு fd;

                /* Temporarily ignore SIGHUP, so that we don't get SIGHUP'ed if we already own the tty. */
                கட்டமைப்பு sigaction sa_old;
                assert_se(sigaction(SIGHUP, &sigaction_ignore, &sa_old) >= 0);

                /* First, try to get the tty */
                r = RET_NERRNO(ioctl(fd, TIOCSCTTY, mode == ACQUIRE_TERMINAL_FORCE));

                /* Reset signal handler to old value */
                assert_se(sigaction(SIGHUP, &sa_old, வெற்று) >= 0);

                /* Success? Exit the loop now! */
                எனில் (r >= 0)
                        நிறுத்து;

                /* Any failure besides -EPERM? Fail, regardless of the mode. */
                எனில் (r != -EPERM)
                        திரும்பு r;

                எனில் (flags & ACQUIRE_TERMINAL_PERMISSIVE) /* If we are in permissive mode, then EPERM is fine, turn this
                                                          * into a success. Note that EPERM is also returned if we
                                                          * already are the owner of the TTY. */
                        நிறுத்து;

                எனில் (mode != ACQUIRE_TERMINAL_WAIT) /* If we are in TRY or FORCE mode, then propagate EPERM as EPERM */
                        திரும்பு r;

                ப்ரதிஜ்ஞா(notify >= 0);
                ப்ரதிஜ்ஞா(wd >= 0);

                க்கு (;;) {
                        usec_t இடது;
                        எனில் (timeout == USEC_INFINITY)
                                இடது = USEC_INFINITY;
                        இல்லையெனில் {
                                ப்ரதிஜ்ஞா(ts != USEC_INFINITY);

                                usec_t n = usec_sub_unsigned(now(CLOCK_MONOTONIC), ts);
                                எனில் (n >= timeout)
                                        திரும்பு -ETIMEDOUT;

                                இடது = timeout - n;
                        }

                        r = ppoll_usec_full(
                                        &(கட்டமைப்பு pollfd) {
                                                .fd = notify,
                                                .events = POLLIN,
                                        },
                                        /* n_fds= */ 1,
                                        இடது,
                                        &poll_ss);
                        எனில் (r < 0)
                                திரும்பு r;
                        எனில் (r == 0)
                                திரும்பு -ETIMEDOUT;

                        ஒன்றிப்பு inotify_event_buffer buffer;
                        ssize_t l;
                        l = பட(notify, &buffer, அளவு(buffer));
                        எனில் (l < 0) {
                                எனில் (ERRNO_IS_TRANSIENT(errno))
                                        தொடர்;

                                திரும்பு -errno;
                        }

                        FOREACH_INOTIFY_EVENT(e, buffer, l) {
                                எனில் (e->mask & IN_Q_OVERFLOW) /* If we hit an inotify queue overflow, simply check if the terminal is up for grabs now. */
                                        நிறுத்து;

                                எனில் (e->wd != wd || !(e->mask & IN_CLOSE)) /* Safety checks */
                                        திரும்பு -EIO;
                        }

                        நிறுத்து;
                }

                /* We close the tty fd here since if the old session ended our handle will be dead. It's important that
                 * we do this after sleeping, so that we don't enter an endless loop. */
                fd = safe_close(fd);
        }

        திரும்பு TAKE_FD(fd);
}

முழு release_terminal(வெறுமை) {
        _cleanup_close_ முழு fd = -EBADF;
        முழு r;

        fd = open("/dev/tty", O_RDWR|O_NOCTTY|O_CLOEXEC|O_NONBLOCK);
        எனில் (fd < 0)
                திரும்பு -errno;

        /* Temporarily ignore SIGHUP, so that we don't get SIGHUP'ed
         * by our own TIOCNOTTY */
        கட்டமைப்பு sigaction sa_old;
        assert_se(sigaction(SIGHUP, &sigaction_ignore, &sa_old) >= 0);

        r = RET_NERRNO(ioctl(fd, TIOCNOTTY));

        assert_se(sigaction(SIGHUP, &sa_old, வெற்று) >= 0);

        திரும்பு r;
}

முழு terminal_new_session(வெறுமை) {

        /* Make us the new session leader, and set stdin tty to be our controlling terminal.
         *
         * Why stdin? Well, the ctty logic is relevant for signal delivery mostly, i.e. if people hit C-c
         * or the line is hung up. Such events are basically just a form of input, via a side channel
         * (that side channel being signal delivery, i.e. SIGINT, SIGHUP et al). Hence we focus on input,
         * not output here. */

        எனில் (!isatty_safe(STDIN_FILENO))
                திரும்பு -ENXIO;

        (வெறுமை) setsid();
        திரும்பு RET_NERRNO(ioctl(STDIN_FILENO, TIOCSCTTY, 0));
}

வெறுமை terminal_detach_session(வெறுமை) {
        (வெறுமை) setsid();
        (வெறுமை) release_terminal();
}

முழு terminal_vhangup_fd(முழு fd) {
        ப்ரதிஜ்ஞா(fd >= 0);
        திரும்பு RET_NERRNO(ioctl(fd, TIOCVHANGUP));
}

முழு terminal_vhangup(மாறிலி எழுத்து *tty) {
        _cleanup_close_ முழு fd = -EBADF;

        ப்ரதிஜ்ஞா(tty);

        fd = open_terminal(tty, O_RDWR|O_NOCTTY|O_CLOEXEC);
        எனில் (fd < 0)
                திரும்பு fd;

        திரும்பு terminal_vhangup_fd(fd);
}

முழு vt_disallocate(மாறிலி எழுத்து *tty_path) {
        ப்ரதிஜ்ஞா(tty_path);

        /* Deallocate the VT if possible. If not possible (i.e. because it is the active one), at least clear
         * it entirely (including the scrollback buffer). */

        முழு ttynr = vtnr_from_tty(tty_path);
        எனில் (ttynr > 0) {
                _cleanup_close_ முழு fd = open_terminal("/dev/tty0", O_RDWR|O_NOCTTY|O_CLOEXEC|O_NONBLOCK);
                எனில் (fd < 0)
                        திரும்பு fd;

                /* Try to deallocate */
                எனில் (ioctl(fd, VT_DISALLOCATE, ttynr) >= 0)
                        திரும்பு 0;
                எனில் (errno != EBUSY)
                        திரும்பு -errno;
        }

        /* So this is not a VT (in which case we cannot deallocate it), or we failed to deallocate. Let's at
         * least clear the screen. */

        _cleanup_close_ முழு fd2 = open_terminal(tty_path, O_WRONLY|O_NOCTTY|O_CLOEXEC|O_NONBLOCK);
        எனில் (fd2 < 0)
                திரும்பு fd2;

        திரும்பு loop_write_full(fd2,
                               "\033[r"   /* clear scrolling region */
                               "\033[H"   /* move home */
                               "\033[3J"  /* clear screen including scrollback, requires Linux 2.6.40 */
                               "\033c",   /* reset to initial state */
                               SIZE_MAX,
                               CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
}

நிலையான முழு vt_default_utf8(வெறுமை) {
        /* Read the default VT UTF8 setting from the kernel */
        திரும்பு read_boolean_file("/sys/module/vt/parameters/default_utf8");
}

நிலையான முழு vt_reset_keyboard(முழு fd) {
        முழு r, kb;

        ப்ரதிஜ்ஞா(fd >= 0);

        /* If we can't read the default, then default to Unicode. It's 2024 after all. */
        r = vt_default_utf8();
        எனில் (r < 0)
                log_debug_errno(r, "Failed to determine kernel VT UTF-8 mode, assuming enabled: %m");

        kb = vt_default_utf8() != 0 ? K_UNICODE : K_XLATE;
        திரும்பு RET_NERRNO(ioctl(fd, KDSKBMODE, kb));
}

நிலையான முழு terminal_reset_ioctl(முழு fd, bool switch_to_text) {
        கட்டமைப்பு termios termios;
        முழு r;

        /* Set terminal to some sane defaults */

        ப்ரதிஜ்ஞா(fd >= 0);

        /* We leave locked terminal attributes untouched, so that Plymouth may set whatever it wants to set,
         * and we don't interfere with that. */

        /* Disable exclusive mode, just in case */
        எனில் (ioctl(fd, TIOCNXCL) < 0)
                log_debug_errno(errno, "TIOCNXCL ioctl failed on TTY, ignoring: %m");

        /* Switch to text mode */
        எனில் (switch_to_text)
                எனில் (ioctl(fd, KDSETMODE, KD_TEXT) < 0)
                        log_debug_errno(errno, "KDSETMODE ioctl for switching to text mode failed on TTY, ignoring: %m");

        /* Set default keyboard mode */
        r = vt_reset_keyboard(fd);
        எனில் (r < 0)
                log_debug_errno(r, "Failed to reset VT keyboard, ignoring: %m");

        எனில் (tcgetattr(fd, &termios) < 0) {
                r = log_debug_errno(errno, "Failed to get terminal parameters: %m");
                செல் finish;
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
        எனில் (r < 0)
                log_debug_errno(r, "Failed to set terminal parameters: %m");

finish:
        /* Just in case, flush all crap out */
        (வெறுமை) tcflush(fd, TCIOFLUSH);

        திரும்பு r;
}

முழு terminal_reset_ansi_seq(முழு fd) {
        _cleanup_(nonblock_resetp) முழு nonblock_reset = -EBADF;
        முழு r;

        ப்ரதிஜ்ஞா(fd >= 0);

        எனில் (getenv_terminal_is_dumb())
                திரும்பு 0;

        r = fd_nonblock(fd, true);
        எனில் (r < 0)
                திரும்பு log_debug_errno(r, "Failed to set terminal to non-blocking mode: %m");
        எனில் (r > 0)
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
        எனில் (r < 0)
                log_debug_errno(r, "Failed to reset terminal through ANSI sequences: %m");

        திரும்பு r;
}

வெறுமை reset_dev_console_fd(முழு fd, bool switch_to_text) {
        முழு r;

        ப்ரதிஜ்ஞா(fd >= 0);

        _cleanup_close_ முழு lock_fd = lock_dev_console();
        எனில் (lock_fd < 0)
                log_debug_errno(lock_fd, "Failed to lock /dev/console, ignoring: %m");

        r = terminal_reset_ioctl(fd, switch_to_text);
        எனில் (r < 0)
                log_warning_errno(r, "Failed to reset /dev/console, ignoring: %m");

        unsigned rows, cols;
        r = proc_cmdline_tty_size("/dev/console", &rows, &cols);
        எனில் (r < 0)
                log_warning_errno(r, "Failed to get /dev/console size, ignoring: %m");
        இல்லையெனில் எனில் (r > 0) {
                r = terminal_set_size_fd(fd, வெற்று, rows, cols);
                எனில் (r < 0)
                        log_warning_errno(r, "Failed to set configured terminal size on /dev/console, ignoring: %m");
        } இல்லையெனில்
                (வெறுமை) terminal_fix_size(fd, fd);

        r = terminal_reset_ansi_seq(fd);
        எனில் (r < 0)
                log_warning_errno(r, "Failed to reset /dev/console using ANSI sequences, ignoring: %m");
}

முழு lock_dev_console(வெறுமை) {
        _cleanup_close_ முழு fd = -EBADF;
        முழு r;

        /* NB: We do not use O_NOFOLLOW here, because some container managers might place a symlink to some
         * pty in /dev/console, in which case it should be fine to lock the target TTY. */
        fd = open_terminal("/dev/console", O_RDONLY|O_CLOEXEC|O_NOCTTY);
        எனில் (fd < 0)
                திரும்பு fd;

        r = lock_generic(fd, LOCK_BSD, LOCK_EX);
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு TAKE_FD(fd);
}

முழு make_console_stdio(bool switch_to_text) {
        முழு fd, r;

        /* Make /dev/console the controlling terminal and stdin/stdout/stderr, if we can. If we can't use
         * /dev/null instead. This is particularly useful if /dev/console is turned off, e.g. if console=null
         * is specified on the kernel command line. */

        fd = acquire_terminal("/dev/console", ACQUIRE_TERMINAL_FORCE|ACQUIRE_TERMINAL_PERMISSIVE, USEC_INFINITY);
        எனில் (fd < 0) {
                log_warning_errno(fd, "Failed to acquire terminal, using /dev/null stdin/stdout/stderr instead: %m");

                r = make_null_stdio();
                எனில் (r < 0)
                        திரும்பு log_error_errno(r, "Failed to make /dev/null stdin/stdout/stderr: %m");

        } இல்லையெனில் {
                reset_dev_console_fd(fd, switch_to_text);

                r = rearrange_stdio(fd, fd, fd); /* This invalidates 'fd' both on success and on failure. */
                எனில் (r < 0)
                        திரும்பு log_error_errno(r, "Failed to make terminal stdin/stdout/stderr: %m");
        }

        reset_terminal_feature_caches();
        திரும்பு 0;
}

நிலையான முழு vtnr_from_tty_raw(மாறிலி எழுத்து *tty, unsigned *ret) {
        ப்ரதிஜ்ஞா(tty);

        tty = skip_dev_prefix(tty);

        மாறிலி எழுத்து *e = startswith(tty, "tty");
        எனில் (!e)
                திரும்பு -EINVAL;

        திரும்பு safe_atou(e, ret);
}

முழு vtnr_from_tty(மாறிலி எழுத்து *tty) {
        unsigned u;
        முழு r;

        ப்ரதிஜ்ஞா(tty);

        r = vtnr_from_tty_raw(tty, &u);
        எனில் (r < 0)
                திரும்பு r;
        எனில் (!vtnr_is_valid(u))
                திரும்பு -ERANGE;

        திரும்பு (முழு) u;
}

bool tty_is_vc(மாறிலி எழுத்து *tty) {
        ப்ரதிஜ்ஞா(tty);

        /* NB: for >= 0 values no range check is conducted here, on the assumption that the caller will
         * either extract vtnr through vtnr_from_tty() later where ERANGE would be reported, or doesn't care
         * about whether it's strictly valid, but only asking "does this fall into the vt category?", for which
         * "yes" seems to be a better answer. */

        திரும்பு vtnr_from_tty_raw(tty, /* ret= */ வெற்று) >= 0;
}

bool tty_is_console(மாறிலி எழுத்து *tty) {
        ப்ரதிஜ்ஞா(tty);

        திரும்பு streq(skip_dev_prefix(tty), "console");
}

முழு resolve_dev_console(எழுத்து **ret) {
        முழு r;

        ப்ரதிஜ்ஞா(ret);

        /* Resolve where /dev/console is pointing to. If /dev/console is a symlink (like in container
         * managers), we'll just resolve the symlink. If it's a real device node, we'll use if
         * /sys/class/tty/tty0/active, but only if /sys/ is actually ours (i.e. not read-only-mounted which
         * is a sign for container setups). */

        _cleanup_free_ எழுத்து *chased = வெற்று;
        r = chase("/dev/console", /* root= */ வெற்று, /* flags= */ 0, &chased, /* ret_fd= */ வெற்று);
        எனில் (r < 0)
                திரும்பு r;
        எனில் (!path_equal(chased, "/dev/console")) {
                *ret = TAKE_PTR(chased);
                திரும்பு 0;
        }

        r = path_is_read_only_fs("/sys");
        எனில் (r < 0)
                திரும்பு r;
        எனில் (r > 0)
                திரும்பு -ENOMEDIUM;

        _cleanup_free_ எழுத்து *active = வெற்று;
        r = read_one_வரி_file("/sys/class/tty/console/active", &active);
        எனில் (r < 0)
                திரும்பு r;
        எனில் (r == 0)
                திரும்பு -ENXIO;

        /* If multiple log outputs are configured the last one is what /dev/console points to */
        மாறிலி எழுத்து *tty = strrchr(active, ' ');
        எனில் (tty)
                tty++;
        இல்லையெனில்
                tty = active;

        எனில் (streq(tty, "tty0")) {
                active = mfree(active);

                /* Get the active VC (e.g. tty1) */
                r = read_one_வரி_file("/sys/class/tty/tty0/active", &active);
                எனில் (r < 0)
                        திரும்பு r;
                எனில் (r == 0)
                        திரும்பு -ENXIO;

                tty = active;
        }

        _cleanup_free_ எழுத்து *path = வெற்று;
        path = path_join("/dev", tty);
        எனில் (!path)
                திரும்பு -ENOMEM;

        *ret = TAKE_PTR(path);
        திரும்பு 0;
}

முழு get_kernel_consoles(எழுத்து ***ret) {
        _cleanup_strv_free_ எழுத்து **l = வெற்று;
        _cleanup_free_ எழுத்து *வரி = வெற்று;
        முழு r;

        ப்ரதிஜ்ஞா(ret);

        /* If /sys/ is mounted read-only this means we are running in some kind of container environment.
         * In that case /sys/ would reflect the host system, not us, hence ignore the data we can read from it. */
        எனில் (path_is_read_only_fs("/sys") > 0)
                செல் fallback;

        r = read_one_வரி_file("/sys/class/tty/console/active", &வரி);
        எனில் (r < 0)
                திரும்பு r;

        க்கு (மாறிலி எழுத்து *p = வரி;;) {
                _cleanup_free_ எழுத்து *tty = வெற்று, *path = வெற்று;

                r = extract_first_word(&p, &tty, வெற்று, 0);
                எனில் (r < 0)
                        திரும்பு r;
                எனில் (r == 0)
                        நிறுத்து;

                எனில் (streq(tty, "tty0")) {
                        tty = mfree(tty);
                        r = read_one_வரி_file("/sys/class/tty/tty0/active", &tty);
                        எனில் (r < 0)
                                திரும்பு r;
                        எனில் (r == 0) {
                                log_debug("No VT active, skipping /dev/tty0.");
                                தொடர்;
                        }
                }

                path = path_join("/dev", tty);
                எனில் (!path)
                        திரும்பு -ENOMEM;

                எனில் (access(path, F_OK) < 0) {
                        log_debug_errno(errno, "Console device %s is not accessible, skipping: %m", path);
                        தொடர்;
                }

                r = strv_consume(&l, TAKE_PTR(path));
                எனில் (r < 0)
                        திரும்பு r;
        }

        எனில் (strv_isempty(l)) {
                log_debug("No devices found for system console");
                செல் fallback;
        }

        *ret = TAKE_PTR(l);
        திரும்பு strv_length(*ret);

fallback:
        r = strv_extend(&l, "/dev/console");
        எனில் (r < 0)
                திரும்பு r;

        *ret = TAKE_PTR(l);
        திரும்பு 0;
}

bool tty_is_vc_resolve(மாறிலி எழுத்து *tty) {
        _cleanup_free_ எழுத்து *resolved = வெற்று;

        ப்ரதிஜ்ஞா(tty);

        எனில் (streq(skip_dev_prefix(tty), "console")) {
                எனில் (resolve_dev_console(&resolved) < 0)
                        திரும்பு false;

                tty = resolved;
        }

        திரும்பு tty_is_vc(tty);
}

முழு fd_columns(முழு fd) {
        கட்டமைப்பு winsize ws = {};

        எனில் (fd < 0)
                திரும்பு -EBADF;

        எனில் (ioctl(fd, TIOCGWINSZ, &ws) < 0)
                திரும்பு -errno;

        எனில் (ws.ws_col <= 0)
                திரும்பு -ENODATA; /* some tty types come up with invalid row/column initially, return a recognizable error for that */

        திரும்பு ws.ws_col;
}

முழு getenv_columns(வெறுமை) {
        முழு r;

        மாறிலி எழுத்து *e = getenv("COLUMNS");
        எனில் (!e)
                திரும்பு -ENXIO;

        unsigned c;
        r = safe_atou_bounded(e, 1, USHRT_MAX, &c);
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு (முழு) c;
}

unsigned columns(வெறுமை) {

        எனில் (cached_columns > 0)
                திரும்பு cached_columns;

        முழு c = getenv_columns();
        எனில் (c < 0) {
                c = fd_columns(STDOUT_FILENO);
                எனில் (c < 0)
                        c = 80;
        }

        ப்ரதிஜ்ஞா(c > 0);

        cached_columns = c;
        திரும்பு cached_columns;
}

முழு fd_வரிகள்(முழு fd) {
        கட்டமைப்பு winsize ws = {};

        எனில் (fd < 0)
                திரும்பு -EBADF;

        எனில் (ioctl(fd, TIOCGWINSZ, &ws) < 0)
                திரும்பு -errno;

        எனில் (ws.ws_row <= 0)
                திரும்பு -ENODATA; /* some tty types come up with invalid row/column initially, return a recognizable error for that */

        திரும்பு ws.ws_row;
}

unsigned வரிகள்(வெறுமை) {
        மாறிலி எழுத்து *e;
        முழு l;

        எனில் (cached_வரிகள் > 0)
                திரும்பு cached_வரிகள்;

        l = 0;
        e = getenv("LINES");
        எனில் (e)
                (வெறுமை) safe_atoi(e, &l);

        எனில் (l <= 0 || l > USHRT_MAX) {
                l = fd_வரிகள்(STDOUT_FILENO);
                எனில் (l <= 0)
                        l = 24;
        }

        cached_வரிகள் = l;
        திரும்பு cached_வரிகள்;
}

முழு terminal_set_size_fd(முழு fd, மாறிலி எழுத்து *ident, unsigned rows, unsigned cols) {
        கட்டமைப்பு winsize ws;

        ப்ரதிஜ்ஞா(fd >= 0);

        எனில் (!ident)
                ident = "TTY";

        எனில் (rows == UINT_MAX && cols == UINT_MAX)
                திரும்பு 0;

        எனில் (ioctl(fd, TIOCGWINSZ, &ws) < 0)
                திரும்பு log_debug_errno(errno,
                                       "TIOCGWINSZ ioctl for getting %s size failed, not setting terminal size: %m",
                                       ident);

        எனில் (rows == UINT_MAX)
                rows = ws.ws_row;
        இல்லையெனில் எனில் (rows > USHRT_MAX)
                rows = USHRT_MAX;

        எனில் (cols == UINT_MAX)
                cols = ws.ws_col;
        இல்லையெனில் எனில் (cols > USHRT_MAX)
                cols = USHRT_MAX;

        எனில் (rows == ws.ws_row && cols == ws.ws_col)
                திரும்பு 0;

        ws.ws_row = rows;
        ws.ws_col = cols;

        எனில் (ioctl(fd, TIOCSWINSZ, &ws) < 0)
                திரும்பு log_debug_errno(errno, "TIOCSWINSZ ioctl for setting %s size failed: %m", ident);

        திரும்பு 0;
}

முழு proc_cmdline_tty_size(மாறிலி எழுத்து *tty, unsigned *ret_rows, unsigned *ret_cols) {
        _cleanup_free_ எழுத்து *rowskey = வெற்று, *rowsvalue = வெற்று, *colskey = வெற்று, *colsvalue = வெற்று;
        unsigned rows = UINT_MAX, cols = UINT_MAX;
        முழு r;

        ப்ரதிஜ்ஞா(tty);

        எனில் (!ret_rows && !ret_cols)
                திரும்பு 0;

        tty = skip_dev_prefix(tty);
        எனில் (path_startswith(tty, "pts/"))
                திரும்பு -EMEDIUMTYPE;
        எனில் (!in_charset(tty, ALPHANUMERICAL))
                திரும்பு log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "TTY name '%s' contains non-alphanumeric characters, not searching kernel cmdline for size.", tty);

        rowskey = strjoin("systemd.tty.rows.", tty);
        எனில் (!rowskey)
                திரும்பு -ENOMEM;

        colskey = strjoin("systemd.tty.columns.", tty);
        எனில் (!colskey)
                திரும்பு -ENOMEM;

        r = proc_cmdline_get_சாவி_many(/* flags= */ 0,
                                      rowskey, &rowsvalue,
                                      colskey, &colsvalue);
        எனில் (r < 0)
                திரும்பு log_debug_errno(r, "Failed to read TTY size of %s from kernel cmdline: %m", tty);

        எனில் (rowsvalue) {
                r = safe_atou(rowsvalue, &rows);
                எனில் (r < 0)
                        திரும்பு log_debug_errno(r, "Failed to parse %s=%s: %m", rowskey, rowsvalue);
        }

        எனில் (colsvalue) {
                r = safe_atou(colsvalue, &cols);
                எனில் (r < 0)
                        திரும்பு log_debug_errno(r, "Failed to parse %s=%s: %m", colskey, colsvalue);
        }

        எனில் (ret_rows)
                *ret_rows = rows;
        எனில் (ret_cols)
                *ret_cols = cols;

        திரும்பு rows != UINT_MAX || cols != UINT_MAX;
}

/* intended to be used as a SIGWINCH sighandler */
வெறுமை columns_வரிகள்_cache_reset(முழு signum) {
        cached_columns = 0;
        cached_வரிகள் = 0;
}

வெறுமை reset_terminal_feature_caches(வெறுமை) {
        cached_columns = 0;
        cached_வரிகள் = 0;

        cached_on_tty = -1;
        cached_on_dev_null = -1;

        reset_ansi_feature_caches();
}

bool on_tty(வெறுமை) {

        /* We check both stdout and stderr, so that situations where pipes on the shell are used are reliably
         * recognized, regardless if only the output or the errors are piped to some place. Since on_tty() is generally
         * used to default to a safer, non-interactive, non-color mode of operation it's probably good to be defensive
         * here, and check for both. Note that we don't check for STDIN_FILENO, because it should fine to use fancy
         * terminal functionality when outputting stuff, even if the input is piped to us. */

        எனில் (cached_on_tty < 0)
                cached_on_tty =
                        isatty_safe(STDOUT_FILENO) &&
                        isatty_safe(STDERR_FILENO);

        திரும்பு cached_on_tty;
}

முழு getttyname_malloc(முழு fd, எழுத்து **ret) {
        எழுத்து path[PATH_MAX]; /* PATH_MAX is counted *with* the trailing NUL byte */
        முழு r;

        ப்ரதிஜ்ஞா(fd >= 0);
        ப்ரதிஜ்ஞா(ret);

        r = ttyname_r(fd, path, அளவு path); /* positive error */
        ப்ரதிஜ்ஞா(r >= 0);
        எனில் (r == ERANGE)
                திரும்பு -ENAMETOOLONG;
        எனில் (r > 0)
                திரும்பு -r;

        திரும்பு strdup_to(ret, skip_dev_prefix(path));
}

முழு getttyname_harder(முழு fd, எழுத்து **ret) {
        _cleanup_free_ எழுத்து *s = வெற்று;
        முழு r;

        ப்ரதிஜ்ஞா(ret);

        r = getttyname_malloc(fd, &s);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (streq(s, "tty"))
                திரும்பு get_ctty(0, வெற்று, ret);

        *ret = TAKE_PTR(s);
        திரும்பு 0;
}

முழு get_ctty_devnr(pid_t pid, dev_t *ret) {
        _cleanup_free_ எழுத்து *வரி = வெற்று;
        unsigned long ttynr;
        மாறிலி எழுத்து *p;
        முழு r;

        ப்ரதிஜ்ஞா(pid >= 0);

        p = procfs_file_alloca(pid, "stat");
        r = read_one_வரி_file(p, &வரி);
        எனில் (r < 0)
                திரும்பு r;

        p = strrchr(வரி, ')');
        எனில் (!p)
                திரும்பு -EIO;

        p++;

        எனில் (sscanf(p, " "
                   "%*c "  /* state */
                   "%*d "  /* ppid */
                   "%*d "  /* pgrp */
                   "%*d "  /* session */
                   "%lu ", /* ttynr */
                   &ttynr) != 1)
                திரும்பு -EIO;

        எனில் (devnum_is_zero(ttynr))
                திரும்பு -ENXIO;

        எனில் (ret)
                *ret = (dev_t) ttynr;

        திரும்பு 0;
}

முழு get_ctty(pid_t pid, dev_t *ret_devnr, எழுத்து **ret) {
        எழுத்து pty[STRLEN("/dev/pts/") + DECIMAL_STR_MAX(dev_t) + 1];
        _cleanup_free_ எழுத்து *buf = வெற்று;
        மாறிலி எழுத்து *fn = வெற்று, *w;
        dev_t devnr;
        முழு r;

        r = get_ctty_devnr(pid, &devnr);
        எனில் (r < 0)
                திரும்பு r;

        r = device_path_make_canonical(S_IFCHR, devnr, &buf);
        எனில் (r < 0) {
                கட்டமைப்பு stat st;

                எனில் (r != -ENOENT) /* No symlink for this in /dev/char/? */
                        திரும்பு r;

                /* Maybe this is PTY? PTY devices are not listed in /dev/char/, as they don't follow the
                 * Linux device model and hence device_path_make_canonical() doesn't work for them. Let's
                 * assume this is a PTY for a moment, and check if the device node this would then map to in
                 * /dev/pts/ matches the one we are looking for. This way we don't have to hardcode the major
                 * number (which is 136 btw), but we still rely on the fact that PTY numbers map directly to
                 * the minor number of the pty. */
                xsprintf(pty, "/dev/pts/%u", minor(devnr));

                எனில் (stat(pty, &st) < 0) {
                        எனில் (errno != ENOENT)
                                திரும்பு -errno;

                } இல்லையெனில் எனில் (S_ISCHR(st.st_mode) && devnr == st.st_rdev) /* Bingo! */
                        fn = pty;

                எனில் (!fn) {
                        /* Doesn't exist, or not a PTY? Probably something similar to the PTYs which have no
                         * symlink in /dev/char/. Let's return something vaguely useful. */
                        r = device_path_make_major_minor(S_IFCHR, devnr, &buf);
                        எனில் (r < 0)
                                திரும்பு r;

                        fn = buf;
                }
        } இல்லையெனில்
                fn = buf;

        w = path_startswith(fn, "/dev/");
        எனில் (!w)
                திரும்பு -EINVAL;

        எனில் (ret) {
                r = strdup_to(ret, w);
                எனில் (r < 0)
                        திரும்பு r;
        }

        எனில் (ret_devnr)
                *ret_devnr = devnr;

        திரும்பு 0;
}

முழு ptsname_malloc(முழு fd, எழுத்து **ret) {
        ப்ரதிஜ்ஞா(fd >= 0);
        ப்ரதிஜ்ஞா(ret);

        க்கு (size_t l = 50;;) {
                _cleanup_free_ எழுத்து *c = வெற்று;

                c = new(எழுத்து, l);
                எனில் (!c)
                        திரும்பு -ENOMEM;

                எனில் (ptsname_r(fd, c, l) >= 0) {
                        *ret = TAKE_PTR(c);
                        திரும்பு 0;
                }
                எனில் (errno != ERANGE)
                        திரும்பு -errno;

                எனில் (!MUL_ASSIGN_SAFE(&l, 2))
                        திரும்பு -ENOMEM;
        }
}

முழு openpt_allocate(முழு flags, எழுத்து **ret_peer_path) {
        _cleanup_close_ முழு fd = -EBADF;
        முழு r;

        fd = posix_openpt(flags|O_NOCTTY|O_CLOEXEC);
        எனில் (fd < 0)
                திரும்பு -errno;

        _cleanup_free_ எழுத்து *p = வெற்று;
        எனில் (ret_peer_path) {
                r = ptsname_malloc(fd, &p);
                எனில் (r < 0)
                        திரும்பு r;

                எனில் (!path_startswith(p, "/dev/pts/"))
                        திரும்பு -EINVAL;
        }

        எனில் (unlockpt(fd) < 0)
                திரும்பு -errno;

        எனில் (ret_peer_path)
                *ret_peer_path = TAKE_PTR(p);

        திரும்பு TAKE_FD(fd);
}

நிலையான முழு ptsname_namespace(முழு pty, எழுத்து **ret) {
        முழு no = -1;

        ப்ரதிஜ்ஞா(pty >= 0);
        ப்ரதிஜ்ஞா(ret);

        /* Like ptsname(), but doesn't assume that the path is
         * accessible in the local namespace. */

        எனில் (ioctl(pty, TIOCGPTN, &no) < 0)
                திரும்பு -errno;

        எனில் (no < 0)
                திரும்பு -EIO;

        எனில் (asprintf(ret, "/dev/pts/%i", no) < 0)
                திரும்பு -ENOMEM;

        திரும்பு 0;
}

முழு openpt_allocate_in_namespace(
                மாறிலி PidRef *pidref,
                முழு flags,
                எழுத்து **ret_peer_path) {

        _cleanup_close_ முழு pidnsfd = -EBADF, mntnsfd = -EBADF, usernsfd = -EBADF, rootfd = -EBADF, fd = -EBADF;
        _cleanup_close_pair_ முழு pair[2] = EBADF_PAIR;
        முழு r;

        ப்ரதிஜ்ஞா(pidref);

        r = pidref_namespace_open(pidref, &pidnsfd, &mntnsfd, /* ret_netns_fd= */ வெற்று, &usernsfd, &rootfd);
        எனில் (r < 0)
                திரும்பு log_debug_errno(r, "Failed to open namespaces of PID "PID_FMT": %m", pidref->pid);

        எனில் (socketpair(AF_UNIX, SOCK_DGRAM|SOCK_CLOEXEC, 0, pair) < 0)
                திரும்பு -errno;

        r = namespace_fork(
                        "(sd-openptns)",
                        "(sd-openpt)",
                        FORK_RESET_SIGNALS|FORK_DEATHSIG_SIGKILL|FORK_WAIT,
                        pidnsfd,
                        mntnsfd,
                        /* netns_fd= */ -EBADF,
                        usernsfd,
                        rootfd,
                        /* ret= */ வெற்று);
        எனில் (r < 0)
                திரும்பு r;
        எனில் (r == 0) {
                pair[0] = safe_close(pair[0]);

                fd = openpt_allocate(flags, /* ret_peer_path= */ வெற்று);
                எனில் (fd < 0)
                        _exit(EXIT_FAILURE);

                எனில் (send_one_fd(pair[1], fd, 0) < 0)
                        _exit(EXIT_FAILURE);

                _exit(EXIT_SUCCESS);
        }

        pair[1] = safe_close(pair[1]);

        fd = receive_one_fd(pair[0], 0);
        எனில் (fd < 0)
                திரும்பு fd;

        எனில் (ret_peer_path) {
                r = ptsname_namespace(fd, ret_peer_path);
                எனில் (r < 0)
                        திரும்பு r;
        }

        திரும்பு TAKE_FD(fd);
}

நிலையான bool on_dev_null(வெறுமை) {
        கட்டமைப்பு stat dst, ost, est;

        எனில் (cached_on_dev_null >= 0)
                திரும்பு cached_on_dev_null;

        எனில் (stat("/dev/null", &dst) < 0 || fstat(STDOUT_FILENO, &ost) < 0 || fstat(STDERR_FILENO, &est) < 0)
                cached_on_dev_null = false;
        இல்லையெனில்
                cached_on_dev_null = stat_inode_same(&dst, &ost) && stat_inode_same(&dst, &est);

        திரும்பு cached_on_dev_null;
}

bool term_env_valid(மாறிலி எழுத்து *term) {
        /* Checks if the specified $TERM value is suitable for propagation, i.e. is not empty, not set to
         * "unknown" (as is common in CI), and only contains characters valid in terminal type names.
         * Valid $TERM values are things like "xterm-256color", "linux", "screen.xterm-256color", i.e.
         * alphanumeric characters, hyphens, underscores, dots, and plus signs. */
        திரும்பு !isempty(term) &&
                !streq(term, "unknown") &&
                in_charset(term, ALPHANUMERICAL "-_+.");
}

bool getenv_terminal_is_dumb(வெறுமை) {
        மாறிலி எழுத்து *e;

        e = getenv("TERM");
        எனில் (!e)
                திரும்பு true;

        திரும்பு streq(e, "dumb");
}

bool terminal_is_dumb(வெறுமை) {
        எனில் (!on_tty() && !on_dev_null())
                திரும்பு true;

        திரும்பு getenv_terminal_is_dumb();
}

bool dev_console_colors_enabled(வெறுமை) {
        _cleanup_free_ எழுத்து *s = வெற்று;
        ColorMode m;

        /* Returns true if we assume that color is supported on /dev/console.
         *
         * For that we first check if we explicitly got told to use colors or not, by checking $SYSTEMD_COLORS. If that
         * isn't set we check whether PID 1 has $TERM set, and if not, whether TERM is set on the kernel command
         * line. If we find $TERM set we assume color if it's not set to "dumb", similarly to how regular
         * colors_enabled() operates. */

        m = parse_systemd_colors();
        எனில் (m >= 0)
                திரும்பு m;

        எனில் (getenv("NO_COLOR"))
                திரும்பு false;

        எனில் (getenv_for_pid(1, "TERM", &s) <= 0)
                (வெறுமை) proc_cmdline_get_சாவி("TERM", 0, &s);

        திரும்பு !streq_ptr(s, "dumb");
}

முழு vt_restore(முழு fd) {

        நிலையான மாறிலி கட்டமைப்பு vt_mode mode = {
                .mode = VT_AUTO,
        };

        முழு r, ret = 0;

        ப்ரதிஜ்ஞா(fd >= 0);

        எனில் (!isatty_safe(fd))
                திரும்பு log_debug_errno(SYNTHETIC_ERRNO(ENOTTY), "Asked to restore the VT for an fd that does not refer to a terminal.");

        எனில் (ioctl(fd, KDSETMODE, KD_TEXT) < 0)
                RET_GATHER(ret, log_debug_errno(errno, "Failed to set VT to text mode, ignoring: %m"));

        r = vt_reset_keyboard(fd);
        எனில் (r < 0)
                RET_GATHER(ret, log_debug_errno(r, "Failed to reset keyboard mode, ignoring: %m"));

        எனில் (ioctl(fd, VT_SETMODE, &mode) < 0)
                RET_GATHER(ret, log_debug_errno(errno, "Failed to set VT_AUTO mode, ignoring: %m"));

        r = fchmod_and_chown(fd, TTY_MODE, 0, GID_INVALID);
        எனில் (r < 0)
                RET_GATHER(ret, log_debug_errno(r, "Failed to chmod()/chown() VT, ignoring: %m"));

        திரும்பு ret;
}

முழு vt_release(முழு fd, bool restore) {
        ப்ரதிஜ்ஞா(fd >= 0);

        /* This function releases the VT by acknowledging the VT-switch signal
         * sent by the kernel and optionally reset the VT in text and auto
         * VT-switching modes. */

        எனில் (!isatty_safe(fd))
                திரும்பு log_debug_errno(SYNTHETIC_ERRNO(ENOTTY), "Asked to release the VT for an fd that does not refer to a terminal.");

        எனில் (ioctl(fd, VT_RELDISP, 1) < 0)
                திரும்பு -errno;

        எனில் (restore)
                திரும்பு vt_restore(fd);

        திரும்பு 0;
}

வெறுமை get_log_colors(முழு priority, மாறிலி எழுத்து **on, மாறிலி எழுத்து **off, மாறிலி எழுத்து **highlight) {
        /* Note that this will initialize output variables only when there's something to output.
         * The caller must pre-initialize to "" or NULL as appropriate. */

        எனில் (priority <= LOG_ERR) {
                எனில் (on)
                        *on = ansi_highlight_red();
                எனில் (off)
                        *off = ansi_normal();
                எனில் (highlight)
                        *highlight = ansi_highlight();

        } இல்லையெனில் எனில் (priority <= LOG_WARNING) {
                எனில் (on)
                        *on = ansi_highlight_yellow();
                எனில் (off)
                        *off = ansi_normal();
                எனில் (highlight)
                        *highlight = ansi_highlight();

        } இல்லையெனில் எனில் (priority <= LOG_NOTICE) {
                எனில் (on)
                        *on = ansi_highlight();
                எனில் (off)
                        *off = ansi_normal();
                எனில் (highlight)
                        *highlight = ansi_highlight_red();

        } இல்லையெனில் எனில் (priority >= LOG_DEBUG) {
                எனில் (on)
                        *on = ansi_grey();
                எனில் (off)
                        *off = ansi_normal();
                எனில் (highlight)
                        *highlight = ansi_highlight_red();
        }
}

முழு terminal_set_cursor_position(முழு fd, unsigned row, unsigned column) {
        ப்ரதிஜ்ஞா(fd >= 0);

        எழுத்து cursor_position[STRLEN("\x1B[" ";" "H") + DECIMAL_STR_MAX(unsigned) * 2 + 1];
        xsprintf(cursor_position, "\x1B[%u;%uH", row, column);

        திரும்பு loop_write(fd, cursor_position, SIZE_MAX);
}

நிலையான முழு terminal_verify_same(முழு உள்ளீடு_fd, முழு output_fd) {
        முழு r;

        ப்ரதிஜ்ஞா(உள்ளீடு_fd >= 0);
        ப்ரதிஜ்ஞா(output_fd >= 0);

        /* Validates that the specified fds reference the same TTY */

        எனில் (உள்ளீடு_fd != output_fd) {
                கட்டமைப்பு stat sti;
                எனில் (fstat(உள்ளீடு_fd, &sti) < 0)
                        திரும்பு -errno;

                r = stat_verify_char(&sti); /* TTYs are character devices */
                எனில் (r < 0)
                        திரும்பு r;

                கட்டமைப்பு stat sto;
                எனில் (fstat(output_fd, &sto) < 0)
                        திரும்பு -errno;

                r = stat_verify_char(&sto);
                எனில் (r < 0)
                        திரும்பு r;

                எனில் (sti.st_rdev != sto.st_rdev)
                        திரும்பு -ENOLINK;
        }

        எனில் (!isatty_safe(உள்ளீடு_fd)) /* The check above was just for char device, but now let's ensure it's actually a tty */
                திரும்பு -ENOTTY;

        திரும்பு 0;
}

வகைவரை எண்ணிக்கை CursorPositionState {
        CURSOR_TEXT,
        CURSOR_ESCAPE,
        CURSOR_ROW,
        CURSOR_COLUMN,
} CursorPositionState;

வகைவரை கட்டமைப்பு CursorPositionContext {
        CursorPositionState state;
        unsigned row, column;
} CursorPositionContext;

நிலையான முழு scan_cursor_position_response(
                CursorPositionContext *context,
                மாறிலி எழுத்து *buf,
                size_t size,
                size_t *ret_processed) {

        ப்ரதிஜ்ஞா(context);
        ப்ரதிஜ்ஞா(buf);
        ப்ரதிஜ்ஞா(ret_processed);

        க்கு (size_t i = 0; i < size; i++) {
                எழுத்து c = buf[i];

                தேர்வு (context->state) {

                நிலை CURSOR_TEXT:
                        context->state = c == '\x1B' ? CURSOR_ESCAPE : CURSOR_TEXT;
                        நிறுத்து;

                நிலை CURSOR_ESCAPE:
                        context->state = c == '[' ? CURSOR_ROW : CURSOR_TEXT;
                        நிறுத்து;

                நிலை CURSOR_ROW:
                        எனில் (c == ';')
                                context->state = context->row > 0 ? CURSOR_COLUMN : CURSOR_TEXT;
                        இல்லையெனில் {
                                முழு d = undecchar(c);

                                /* We read a decimal character, let's suffix it to the number we so far read,
                                 * but let's do an overflow check first. */
                                எனில் (d < 0 || context->row > (UINT_MAX-d)/10)
                                        context->state = CURSOR_TEXT;
                                இல்லையெனில்
                                        context->row = context->row * 10 + d;
                        }
                        நிறுத்து;

                நிலை CURSOR_COLUMN:
                        எனில் (c == 'R') {
                                எனில் (context->column > 0) {
                                        *ret_processed = i + 1;
                                        திரும்பு 1; /* success! */
                                }

                                context->state = CURSOR_TEXT;
                        } இல்லையெனில் {
                                முழு d = undecchar(c);

                                /* As above, add the decimal character to our column number */
                                எனில் (d < 0 || context->column > (UINT_MAX-d)/10)
                                        context->state = CURSOR_TEXT;
                                இல்லையெனில்
                                        context->column = context->column * 10 + d;
                        }

                        நிறுத்து;
                }

                /* Reset any positions we might have picked up */
                எனில் (IN_SET(context->state, CURSOR_TEXT, CURSOR_ESCAPE))
                        context->row = context->column = 0;
        }

        *ret_processed = size;
        திரும்பு 0; /* all good, but not enough data yet */
}

முழு terminal_get_cursor_position(
                முழு உள்ளீடு_fd,
                முழு output_fd,
                unsigned *ret_row,
                unsigned *ret_column) {

        _cleanup_close_ முழு nonblock_உள்ளீடு_fd = -EBADF;
        முழு r;

        ப்ரதிஜ்ஞா(உள்ளீடு_fd >= 0);
        ப்ரதிஜ்ஞா(output_fd >= 0);

        எனில் (getenv_terminal_is_dumb())
                திரும்பு -EOPNOTSUPP;

        r = terminal_verify_same(உள்ளீடு_fd, output_fd);
        எனில் (r < 0)
                திரும்பு log_debug_errno(r, "Called with distinct input/output fds: %m");

        /* Failure to reset the terminal is ignored here and in similar cases below.
         * We already have our result; if cleanup fails it doesn't change the validity of the result. */
        கட்டமைப்பு termios old_termios = TERMIOS_NULL;
        CLEANUP_TERMIOS_RESET(உள்ளீடு_fd, old_termios);

        எனில் (tcgetattr(உள்ளீடு_fd, &old_termios) < 0)
                திரும்பு log_debug_errno(errno, "Failed to get terminal settings: %m");

        கட்டமைப்பு termios new_termios = old_termios;
        termios_disable_echo(&new_termios);

        எனில் (tcsetattr(உள்ளீடு_fd, TCSANOW, &new_termios) < 0)
                திரும்பு log_debug_errno(errno, "Failed to set new terminal settings: %m");

        /* Request cursor position (DSR/CPR) */
        r = loop_write(output_fd, "\x1B[6n", SIZE_MAX);
        எனில் (r < 0)
                திரும்பு r;

        /* Open a 2nd input fd, in non-blocking mode, so that we won't ever hang in read() should someone
         * else process the POLLIN. */

        nonblock_உள்ளீடு_fd = r = fd_reopen(உள்ளீடு_fd, O_RDONLY|O_CLOEXEC|O_NONBLOCK|O_NOCTTY);
        எனில் (r < 0)
                திரும்பு r;

        usec_t end = usec_add(now(CLOCK_MONOTONIC), CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        எழுத்து buf[STRLEN("\x1B[1;1R")]; /* The shortest valid reply possible */
        size_t buf_full = 0;
        CursorPositionContext context = {};

        க்கு (bool first = true;; first = false) {
                எனில் (buf_full == 0) {
                        usec_t n = now(CLOCK_MONOTONIC);
                        எனில் (n >= end)
                                திரும்பு -EOPNOTSUPP;

                        r = fd_wait_for_event(nonblock_உள்ளீடு_fd, POLLIN, usec_sub_unsigned(end, n));
                        எனில் (r < 0)
                                திரும்பு r;
                        எனில் (r == 0)
                                திரும்பு -EOPNOTSUPP;

                        /* On the first try, read multiple characters, i.e. the shortest valid
                         * reply. Afterwards read byte-wise, since we don't want to read too much, and
                         * unnecessarily drop too many characters from the input queue. */
                        ssize_t l = பட(nonblock_உள்ளீடு_fd, buf, first ? அளவு(buf) : 1);
                        எனில் (l < 0) {
                                எனில் (errno == EAGAIN)
                                        தொடர்;

                                திரும்பு -errno;
                        }

                        ப்ரதிஜ்ஞா((size_t) l <= அளவு(buf));
                        buf_full = l;
                }

                size_t processed;
                r = scan_cursor_position_response(&context, buf, buf_full, &processed);
                எனில் (r < 0)
                        திரும்பு r;

                ப்ரதிஜ்ஞா(processed <= buf_full);
                buf_full -= processed;
                memmove(buf, buf + processed, buf_full);

                எனில் (r > 0) {
                        /* Superficial validity check */
                        எனில் (context.row >= 32766 || context.column >= 32766)
                                திரும்பு -ENODATA;

                        எனில் (ret_row)
                                *ret_row = context.row;
                        எனில் (ret_column)
                                *ret_column = context.column;

                        திரும்பு 0;
                }
        }
}

முழு terminal_reset_defensive(முழு fd, TerminalResetFlags flags) {
        முழு r = 0;

        ப்ரதிஜ்ஞா(fd >= 0);
        ப்ரதிஜ்ஞா(!FLAGS_SET(flags, TERMINAL_RESET_AVOID_ANSI_SEQ|TERMINAL_RESET_FORCE_ANSI_SEQ));

        /* Resets the terminal comprehensively, i.e. via both ioctl()s and via ANSI sequences, but do so only
         * if $TERM is unset or set to "dumb" */

        எனில் (!isatty_safe(fd))
                திரும்பு -ENOTTY;

        RET_GATHER(r, terminal_reset_ioctl(fd, FLAGS_SET(flags, TERMINAL_RESET_SWITCH_TO_TEXT)));

        எனில் (!FLAGS_SET(flags, TERMINAL_RESET_AVOID_ANSI_SEQ) &&
            (FLAGS_SET(flags, TERMINAL_RESET_FORCE_ANSI_SEQ) || !getenv_terminal_is_dumb()))
                RET_GATHER(r, terminal_reset_ansi_seq(fd));

        திரும்பு r;
}

முழு terminal_reset_defensive_locked(முழு fd, TerminalResetFlags flags) {
        ப்ரதிஜ்ஞா(fd >= 0);

        _cleanup_close_ முழு lock_fd = lock_dev_console();
        எனில் (lock_fd < 0)
                log_debug_errno(lock_fd, "Failed to acquire lock for /dev/console, ignoring: %m");

        திரும்பு terminal_reset_defensive(fd, flags);
}

வெறுமை termios_disable_echo(கட்டமைப்பு termios *termios) {
        ப்ரதிஜ்ஞா(termios);

        termios->c_lflag &= ~(ICANON|ECHO);
        termios->c_cc[VMIN] = 1;
        termios->c_cc[VTIME] = 0;
}

நிலையான bool termios_is_null(மாறிலி கட்டமைப்பு termios *t) {
        எனில் (!t)
                திரும்பு true;

        திரும்பு t->c_iflag == UINT_MAX &&
               t->c_oflag == UINT_MAX &&
               t->c_cflag == UINT_MAX &&
               t->c_lflag == UINT_MAX;
}

வெறுமை termios_reset(மாறிலி TermiosResetContext *c) {
        ப்ரதிஜ்ஞா(c);

        PROTECT_ERRNO;

        எனில் (c->fd && *c->fd >= 0 && !termios_is_null(c->termios))
                (வெறுமை) tcsetattr(*c->fd, TCSANOW, c->termios);
}

வகைவரை எண்ணிக்கை BackgroundColorState {
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

வகைவரை கட்டமைப்பு BackgroundColorContext {
        BackgroundColorState state;
        uint32_t red, green, blue;
        unsigned red_bits, green_bits, blue_bits;
} BackgroundColorContext;

நிலையான முழு scan_background_color_response(
                BackgroundColorContext *context,
                மாறிலி எழுத்து *buf,
                size_t size,
                size_t *ret_processed) {

        ப்ரதிஜ்ஞா(context);
        ப்ரதிஜ்ஞா(buf);
        ப்ரதிஜ்ஞா(ret_processed);

        க்கு (size_t i = 0; i < size; i++) {
                எழுத்து c = buf[i];

                தேர்வு (context->state) {

                நிலை BACKGROUND_TEXT:
                        context->state = c == '\x1B' ? BACKGROUND_ESCAPE : BACKGROUND_TEXT;
                        நிறுத்து;

                நிலை BACKGROUND_ESCAPE:
                        context->state = c == ']' ? BACKGROUND_BRACKET : BACKGROUND_TEXT;
                        நிறுத்து;

                நிலை BACKGROUND_BRACKET:
                        context->state = c == '1' ? BACKGROUND_FIRST_ONE : BACKGROUND_TEXT;
                        நிறுத்து;

                நிலை BACKGROUND_FIRST_ONE:
                        context->state = c == '1' ? BACKGROUND_SECOND_ONE : BACKGROUND_TEXT;
                        நிறுத்து;

                நிலை BACKGROUND_SECOND_ONE:
                        context->state = c == ';' ? BACKGROUND_SEMICOLON : BACKGROUND_TEXT;
                        நிறுத்து;

                நிலை BACKGROUND_SEMICOLON:
                        context->state = c == 'r' ? BACKGROUND_R : BACKGROUND_TEXT;
                        நிறுத்து;

                நிலை BACKGROUND_R:
                        context->state = c == 'g' ? BACKGROUND_G : BACKGROUND_TEXT;
                        நிறுத்து;

                நிலை BACKGROUND_G:
                        context->state = c == 'b' ? BACKGROUND_B : BACKGROUND_TEXT;
                        நிறுத்து;

                நிலை BACKGROUND_B:
                        context->state = c == ':' ? BACKGROUND_RED : BACKGROUND_TEXT;
                        நிறுத்து;

                நிலை BACKGROUND_RED:
                        எனில் (c == '/')
                                context->state = context->red_bits > 0 ? BACKGROUND_GREEN : BACKGROUND_TEXT;
                        இல்லையெனில் {
                                முழு d = unhexchar(c);
                                எனில் (d < 0 || context->red_bits >= அளவு(context->red)*8)
                                        context->state = BACKGROUND_TEXT;
                                இல்லையெனில் {
                                        context->red = (context->red << 4) | d;
                                        context->red_bits += 4;
                                }
                        }
                        நிறுத்து;

                நிலை BACKGROUND_GREEN:
                        எனில் (c == '/')
                                context->state = context->green_bits > 0 ? BACKGROUND_BLUE : BACKGROUND_TEXT;
                        இல்லையெனில் {
                                முழு d = unhexchar(c);
                                எனில் (d < 0 || context->green_bits >= அளவு(context->green)*8)
                                        context->state = BACKGROUND_TEXT;
                                இல்லையெனில் {
                                        context->green = (context->green << 4) | d;
                                        context->green_bits += 4;
                                }
                        }
                        நிறுத்து;

                நிலை BACKGROUND_BLUE:
                        எனில் (c == '\x07') {
                                எனில் (context->blue_bits > 0) {
                                        *ret_processed = i + 1;
                                        திரும்பு 1; /* success! */
                                }

                                context->state = BACKGROUND_TEXT;
                        } இல்லையெனில் எனில் (c == '\x1b')
                                context->state = context->blue_bits > 0 ? BACKGROUND_STRING_TERMINATOR : BACKGROUND_TEXT;
                        இல்லையெனில் {
                                முழு d = unhexchar(c);
                                எனில் (d < 0 || context->blue_bits >= அளவு(context->blue)*8)
                                        context->state = BACKGROUND_TEXT;
                                இல்லையெனில் {
                                        context->blue = (context->blue << 4) | d;
                                        context->blue_bits += 4;
                                }
                        }
                        நிறுத்து;

                நிலை BACKGROUND_STRING_TERMINATOR:
                        எனில் (c == '\\') {
                                *ret_processed = i + 1;
                                திரும்பு 1; /* success! */
                        }

                        context->state = c == ']' ? BACKGROUND_ESCAPE : BACKGROUND_TEXT;
                        நிறுத்து;

                }

                /* Reset any colors we might have picked up */
                எனில் (IN_SET(context->state, BACKGROUND_TEXT, BACKGROUND_ESCAPE)) {
                        /* reset color */
                        context->red = context->green = context->blue = 0;
                        context->red_bits = context->green_bits = context->blue_bits = 0;
                }
        }

        *ret_processed = size;
        திரும்பு 0; /* all good, but not enough data yet */
}

முழு get_default_background_color(இரட்டை *ret_red, இரட்டை *ret_green, இரட்டை *ret_blue) {
        முழு r;

        ப்ரதிஜ்ஞா(ret_red);
        ப்ரதிஜ்ஞா(ret_green);
        ப்ரதிஜ்ஞா(ret_blue);

        எனில் (!colors_enabled())
                திரும்பு -EOPNOTSUPP;

        r = terminal_verify_same(STDIN_FILENO, STDOUT_FILENO);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (streq_ptr(getenv("TERM"), "linux")) {
                /* Linux console is black */
                *ret_red = *ret_green = *ret_blue = 0.0;
                திரும்பு 0;
        }

        /* Open a 2nd input fd, in non-blocking mode, so that we won't ever hang in read()
         * should someone else process the POLLIN. Do all subsequent operations on the new fd. */
        _cleanup_close_ முழு nonblock_உள்ளீடு_fd = r = fd_reopen(STDIN_FILENO, O_RDONLY|O_CLOEXEC|O_NONBLOCK|O_NOCTTY);
        எனில் (r < 0)
                திரும்பு r;

        கட்டமைப்பு termios old_termios = TERMIOS_NULL;
        CLEANUP_TERMIOS_RESET(nonblock_உள்ளீடு_fd, old_termios);

        எனில் (tcgetattr(nonblock_உள்ளீடு_fd, &old_termios) < 0)
                திரும்பு -errno;

        கட்டமைப்பு termios new_termios = old_termios;
        termios_disable_echo(&new_termios);

        எனில் (tcsetattr(nonblock_உள்ளீடு_fd, TCSANOW, &new_termios) < 0)
                திரும்பு -errno;

        r = loop_write(STDOUT_FILENO, ANSI_OSC "11;?" ANSI_ST, SIZE_MAX);
        எனில் (r < 0)
                திரும்பு r;

        usec_t end = usec_add(now(CLOCK_MONOTONIC), CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        எழுத்து buf[STRLEN(ANSI_OSC "11;rgb:0/0/0" ANSI_ST)]; /* shortest possible reply */
        size_t buf_full = 0;
        BackgroundColorContext context = {};

        க்கு (bool first = true;; first = false) {
                எனில் (buf_full == 0) {
                        usec_t n = now(CLOCK_MONOTONIC);
                        எனில் (n >= end)
                                திரும்பு -EOPNOTSUPP;

                        r = fd_wait_for_event(nonblock_உள்ளீடு_fd, POLLIN, usec_sub_unsigned(end, n));
                        எனில் (r < 0)
                                திரும்பு r;
                        எனில் (r == 0)
                                திரும்பு -EOPNOTSUPP;

                        /* On the first try, read multiple characters, i.e. the shortest valid
                         * reply. Afterwards read byte-wise, since we don't want to read too much, and
                         * unnecessarily drop too many characters from the input queue. */
                        ssize_t l = பட(nonblock_உள்ளீடு_fd, buf, first ? அளவு(buf) : 1);
                        எனில் (l < 0) {
                                எனில் (errno == EAGAIN)
                                        தொடர்;
                                திரும்பு -errno;
                        }

                        ப்ரதிஜ்ஞா((size_t) l <= அளவு(buf));
                        buf_full = l;
                }

                size_t processed;
                r = scan_background_color_response(&context, buf, buf_full, &processed);
                எனில் (r < 0)
                        திரும்பு r;

                ப்ரதிஜ்ஞா(processed <= buf_full);
                buf_full -= processed;
                memmove(buf, buf + processed, buf_full);

                எனில் (r > 0) {
                        ப்ரதிஜ்ஞா(context.red_bits > 0);
                        *ret_red = (இரட்டை) context.red / ((UINT64_C(1) << context.red_bits) - 1);
                        ப்ரதிஜ்ஞா(context.green_bits > 0);
                        *ret_green = (இரட்டை) context.green / ((UINT64_C(1) << context.green_bits) - 1);
                        ப்ரதிஜ்ஞா(context.blue_bits > 0);
                        *ret_blue = (இரட்டை) context.blue / ((UINT64_C(1) << context.blue_bits) - 1);
                        திரும்பு 0;
                }
        }
}

/* Determine terminal dimensions by means of ANSI sequences: save the cursor via DECSC, position it far
 * to the bottom right (clamped to actual terminal dimensions), read back via DSR where we ended up, and
 * restore cursor via DECRC. Only needs a single DSR round-trip, and always restores the cursor regardless
 * of whether the response is received.
 *
 * Caller must have already opened a non-blocking input fd and configured termios (echo/icanon off). */
நிலையான முழு terminal_query_size_by_dsr(
                முழு nonblock_உள்ளீடு_fd,
                முழு output_fd,
                unsigned *ret_rows,
                unsigned *ret_columns) {

        முழு r;

        ப்ரதிஜ்ஞா(nonblock_உள்ளீடு_fd >= 0);
        ப்ரதிஜ்ஞா(output_fd >= 0);

        /* Use DECSC/DECRC to save/restore cursor instead of querying position via DSR. This way the cursor
         * is always restored — even on timeout — and we only need one DSR response instead of two. */
        r = loop_write_full(output_fd,
                            "\x1B" "7"              /* DECSC: save cursor position */
                            "\x1B[32766;32766H"     /* CUP: position cursor far to the right and to the bottom, staying within 16bit signed range */
                            "\x1B[6n"               /* DSR: request cursor position (CPR) */
                            "\x1B" "8",             /* DECRC: restore cursor position */
                            SIZE_MAX,
                            CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        எனில் (r < 0)
                திரும்பு r;

        usec_t end = usec_add(now(CLOCK_MONOTONIC), CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        எழுத்து buf[STRLEN("\x1B[1;1R")]; /* The shortest valid reply possible */
        size_t buf_full = 0;
        CursorPositionContext context = {};

        க்கு (bool first = true;; first = false) {
                எனில் (buf_full == 0) {
                        usec_t n = now(CLOCK_MONOTONIC);
                        எனில் (n >= end)
                                திரும்பு -EOPNOTSUPP;

                        r = fd_wait_for_event(nonblock_உள்ளீடு_fd, POLLIN, usec_sub_unsigned(end, n));
                        எனில் (r < 0)
                                திரும்பு r;
                        எனில் (r == 0)
                                திரும்பு -EOPNOTSUPP;

                        /* On the first try, read multiple characters, i.e. the shortest valid
                         * reply. Afterwards read byte-wise, since we don't want to read too much, and
                         * unnecessarily drop too many characters from the input queue. */
                        ssize_t l = பட(nonblock_உள்ளீடு_fd, buf, first ? அளவு(buf) : 1);
                        எனில் (l < 0) {
                                எனில் (errno == EAGAIN)
                                        தொடர்;

                                திரும்பு -errno;
                        }

                        ப்ரதிஜ்ஞா((size_t) l <= அளவு(buf));
                        buf_full = l;
                }

                size_t processed;
                r = scan_cursor_position_response(&context, buf, buf_full, &processed);
                எனில் (r < 0)
                        திரும்பு r;

                ப்ரதிஜ்ஞா(processed <= buf_full);
                buf_full -= processed;
                memmove(buf, buf + processed, buf_full);

                எனில் (r > 0) {
                        /* Superficial validity checks (no particular reason to check for < 4, it's
                         * just a way to look for unreasonably small values) */
                        எனில் (context.row < 4 || context.column < 4 || context.row >= 32766 || context.column >= 32766)
                                திரும்பு -ENODATA;

                        எனில் (ret_rows)
                                *ret_rows = context.row;
                        எனில் (ret_columns)
                                *ret_columns = context.column;

                        திரும்பு 0;
                }
        }
}

/* Common setup for ANSI terminal queries: validate the fds, open a non-blocking input fd, and configure
 * termios with echo and canonical mode disabled. Caller must restore termios and close the fd when done. */
நிலையான முழு terminal_prepare_query(
                முழு உள்ளீடு_fd,
                முழு output_fd,
                முழு *ret_nonblock_fd,
                கட்டமைப்பு termios *ret_saved_termios) {

        முழு r;

        ப்ரதிஜ்ஞா(உள்ளீடு_fd >= 0);
        ப்ரதிஜ்ஞா(output_fd >= 0);
        ப்ரதிஜ்ஞா(ret_nonblock_fd);
        ப்ரதிஜ்ஞா(ret_saved_termios);

        /* Use getenv_terminal_is_dumb() instead of terminal_is_dumb() here since we operate on an
         * explicitly passed fd, not on stdio. terminal_is_dumb() additionally checks on_tty() which
         * tests whether *stderr* is a tty — that's irrelevant when we're querying a directly opened
         * terminal such as /dev/console. */
        எனில் (getenv_terminal_is_dumb())
                திரும்பு -EOPNOTSUPP;

        r = terminal_verify_same(உள்ளீடு_fd, output_fd);
        எனில் (r < 0)
                திரும்பு log_debug_errno(r, "Called with distinct input/output fds: %m");

        /* Open a 2nd input fd, in non-blocking mode, so that we won't ever hang in read()
         * should someone else process the POLLIN. Do all subsequent operations on the new fd. */
        _cleanup_close_ முழு nonblock_உள்ளீடு_fd = r = fd_reopen(உள்ளீடு_fd, O_RDONLY|O_CLOEXEC|O_NONBLOCK|O_NOCTTY);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (tcgetattr(nonblock_உள்ளீடு_fd, ret_saved_termios) < 0)
                திரும்பு log_debug_errno(errno, "Failed to get terminal settings: %m");

        கட்டமைப்பு termios new_termios = *ret_saved_termios;
        termios_disable_echo(&new_termios);

        எனில் (tcsetattr(nonblock_உள்ளீடு_fd, TCSANOW, &new_termios) < 0)
                திரும்பு log_debug_errno(errno, "Failed to set new terminal settings: %m");

        *ret_nonblock_fd = TAKE_FD(nonblock_உள்ளீடு_fd);
        திரும்பு 0;
}

/*
 * See https://terminalguide.namepad.de/seq/csi_st-18/,
 * https://invisible-island.net/xterm/ctlseqs/ctlseqs.html#h3-Functions-using-CSI-_-ordered-by-the-final-character_s_.
 */
#define CSI18_Q  "\x1B[18t"               /* Report the size of the text area in characters */
#define CSI18_Rp "\x1B[8;"                /* Reply prefix */
#define CSI18_R0 CSI18_Rp "1;1t"          /* Shortest reply */
#define CSI18_R1 CSI18_Rp "32766;32766t"  /* Longest reply */

நிலையான முழு scan_text_area_size_response(
                மாறிலி எழுத்து *buf,
                size_t size,
                unsigned *ret_rows,
                unsigned *ret_columns) {

        ப்ரதிஜ்ஞா(buf);
        ப்ரதிஜ்ஞா(ret_rows);
        ப்ரதிஜ்ஞா(ret_columns);

        /* Check if we have enough space for the shortest possible answer. */
        எனில் (size < STRLEN(CSI18_R0))
                திரும்பு -EAGAIN;

        /* Check if the terminating sequence is present */
        எனில் (buf[size - 1] != 't')
                திரும்பு -EAGAIN;

        unsigned short rows, columns;
        எனில் (sscanf(buf, CSI18_Rp "%hu;%hut", &rows, &columns) != 2)
                திரும்பு -EINVAL;

        *ret_rows = rows;
        *ret_columns = columns;
        திரும்பு 0;
}

/* Determine terminal dimensions by means of an ANSI CSI 18 sequence.
 *
 * Caller must have already opened a non-blocking input fd and configured termios (echo/icanon off). */
நிலையான முழு terminal_query_size_by_csi18(
                முழு nonblock_உள்ளீடு_fd,
                முழு output_fd,
                unsigned *ret_rows,
                unsigned *ret_columns) {

        முழு r;

        ப்ரதிஜ்ஞா(nonblock_உள்ளீடு_fd >= 0);
        ப்ரதிஜ்ஞா(output_fd >= 0);

        r = loop_write_full(output_fd, CSI18_Q, SIZE_MAX, CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        எனில் (r < 0)
                திரும்பு r;

        usec_t end = usec_add(now(CLOCK_MONOTONIC), CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        எழுத்து buf[STRLEN(CSI18_R1)];
        size_t bytes = 0;

        க்கு (;;) {
                usec_t n = now(CLOCK_MONOTONIC);
                எனில் (n >= end)
                        திரும்பு -EOPNOTSUPP;

                r = fd_wait_for_event(nonblock_உள்ளீடு_fd, POLLIN, usec_sub_unsigned(end, n));
                எனில் (r < 0)
                        திரும்பு r;
                எனில் (r == 0)
                        திரும்பு -EOPNOTSUPP;

                /* On the first read, read multiple characters, i.e. the shortest valid reply. Afterwards
                 * read byte by byte, since we don't want to read too much and drop characters from the input
                 * queue. */
                ssize_t l = பட(nonblock_உள்ளீடு_fd, buf + bytes, bytes == 0 ? STRLEN(CSI18_R0) : 1);
                எனில் (l < 0) {
                        எனில் (errno == EAGAIN)
                                தொடர்;
                        திரும்பு -errno;
                }

                ப்ரதிஜ்ஞா((size_t) l <= அளவு(buf) - bytes);
                bytes += l;

                r = scan_text_area_size_response(buf, bytes, ret_rows, ret_columns);
                எனில் (r != -EAGAIN)
                        திரும்பு r;

                எனில் (bytes == அளவு(buf))
                        திரும்பு -EOPNOTSUPP; /* The response has the right prefix, but we didn't find a valid
                                             * answer with a terminator in the allotted space. Something is
                                             * wrong, possibly some unrelated bytes got injected into the
                                             * answer. */
        }
}

முழு terminal_get_size(
                முழு உள்ளீடு_fd,
                முழு output_fd,
                unsigned *ret_rows,
                unsigned *ret_columns,
                bool try_dsr,
                bool try_csi18) {

        _cleanup_close_ முழு nonblock_உள்ளீடு_fd = -EBADF;
        கட்டமைப்பு termios old_termios = TERMIOS_NULL;
        CLEANUP_TERMIOS_RESET(nonblock_உள்ளீடு_fd, old_termios);
        _cleanup_(nonblock_resetp) முழு nonblock_reset = -EBADF;
        முழு r;

        ப்ரதிஜ்ஞா(try_dsr || try_csi18);

        r = terminal_prepare_query(உள்ளீடு_fd, output_fd, &nonblock_உள்ளீடு_fd, &old_termios);
        எனில் (r < 0)
                திரும்பு r;

        /* Put the output fd in non-blocking mode with a write timeout, to avoid blocking indefinitely on
         * write if the terminal is not consuming data (e.g. serial console with flow control). */
        r = fd_nonblock(output_fd, true);
        எனில் (r < 0)
                திரும்பு log_debug_errno(r, "Failed to set terminal to non-blocking mode: %m");
        எனில் (r > 0)
                nonblock_reset = output_fd;

        /* Flush any stale input that might confuse the response parsers. */
        (வெறுமை) tcflush(nonblock_உள்ளீடு_fd, TCIFLUSH);

        எனில் (try_csi18) {
                r = terminal_query_size_by_csi18(nonblock_உள்ளீடு_fd, output_fd, ret_rows, ret_columns);
                எனில் (r >= 0)
                        திரும்பு r;

                /* Query failed. Flush any outstanding input. */
                (வெறுமை) tcflush(nonblock_உள்ளீடு_fd, TCIFLUSH);

                எனில் (!IN_SET(r, -EOPNOTSUPP, -EINVAL))
                        திரும்பு r;
        }

        எனில் (try_dsr) {
                r = terminal_query_size_by_dsr(nonblock_உள்ளீடு_fd, output_fd, ret_rows, ret_columns);
                எனில் (r >= 0)
                        திரும்பு r;

                /* Query failed. Flush any outstanding input. */
                (வெறுமை) tcflush(nonblock_உள்ளீடு_fd, TCIFLUSH);
        }

        திரும்பு r;
}

நிலையான முழு fd_is_vc(முழு fd) {
        _cleanup_free_ எழுத்து *name = வெற்று;
        முழு r;

        ப்ரதிஜ்ஞா(fd >= 0);

        r = getttyname_harder(fd, &name);
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு tty_is_vc_resolve(name);
}

முழு terminal_fix_size(முழு உள்ளீடு_fd, முழு output_fd) {
        unsigned rows, columns;
        முழு r;

        /* Don't ask a virtual console at all. There is no intermediary here that could be wrong about the
         * dimensions: the kernel renders the screen itself, so its winsize is authoritative. */
        r = fd_is_vc(உள்ளீடு_fd);
        எனில் (r < 0)
                log_debug_errno(r, "Failed to determine whether terminal is a virtual console, assuming it is not: %m");
        இல்லையெனில் எனில் (r > 0) {
                log_debug("Not querying dimensions of a virtual console, kernel dimensions are authoritative.");
                திரும்பு 0;
        }

        கட்டமைப்பு winsize ws = {};
        எனில் (ioctl(output_fd, TIOCGWINSZ, &ws) < 0)
                திரும்பு log_debug_errno(errno, "Failed to query terminal dimensions, ignoring: %m");

        r = terminal_get_size(உள்ளீடு_fd, output_fd, &rows, &columns, /* try_dsr= */ true, /* try_csi18= */ true);
        எனில் (r < 0)
                திரும்பு log_debug_errno(r, "Failed to acquire terminal dimensions via ANSI sequences, not adjusting terminal dimensions: %m");

        எனில் (ws.ws_row == rows && ws.ws_col == columns) {
                log_debug("Terminal dimensions reported via ANSI sequences match currently set terminal dimensions, not changing.");
                திரும்பு 0;
        }

        ws.ws_col = columns;
        ws.ws_row = rows;

        எனில் (ioctl(output_fd, TIOCSWINSZ, &ws) < 0)
                திரும்பு log_debug_errno(errno, "Failed to update terminal dimensions, ignoring: %m");

        log_debug("Fixed terminal dimensions to %ux%u based on ANSI sequence information.", columns, rows);
        திரும்பு 1;
}

#define MAX_TERMINFO_LENGTH 64
/* python -c 'print("".join(hex(ord(i))[2:] for i in "name").upper())' */
#define DCS_TERMINFO_Q ANSI_DCS "+q" "6E616D65" ANSI_ST
/* The answer is either 0+r… (invalid) or 1+r… (OK). */
#define DCS_TERMINFO_R0 ANSI_DCS "0+r" ANSI_ST
#define DCS_TERMINFO_R1 ANSI_DCS "1+r" "6E616D65" "=" /* This is followed by Pt ST. */
assert_cc(STRLEN(DCS_TERMINFO_R0) <= STRLEN(DCS_TERMINFO_R1 ANSI_ST));

நிலையான முழு scan_terminfo_response(
                மாறிலி எழுத்து *buf,
                size_t size,
                எழுத்து **ret_name) {
        முழு r;

        ப்ரதிஜ்ஞா(buf);
        ப்ரதிஜ்ஞா(ret_name);

        /* Check if we have enough space for the shortest possible answer. */
        எனில் (size < STRLEN(DCS_TERMINFO_R0))
                திரும்பு -EAGAIN;

        /* Check if the terminating sequence is present */
        எனில் (memcmp(buf + size - STRLEN(ANSI_ST), ANSI_ST, STRLEN(ANSI_ST)) != 0)
                திரும்பு -EAGAIN;

        எனில் (size <= STRLEN(DCS_TERMINFO_R1 ANSI_ST))
                திரும்பு -EINVAL;  /* The answer is invalid or empty */

        எனில் (memcmp(buf, DCS_TERMINFO_R1, STRLEN(DCS_TERMINFO_R1)) != 0)
                திரும்பு -EINVAL;  /* The answer is not valid */

        _cleanup_free_ வெறுமை *dec = வெற்று;
        size_t dec_size;
        r = unhexmem_full(buf + STRLEN(DCS_TERMINFO_R1), size - STRLEN(DCS_TERMINFO_R1 ANSI_ST),
                          /* secure= */ false,
                          &dec, &dec_size);
        எனில் (r < 0)
                திரும்பு r;

        ப்ரதிஜ்ஞா(((மாறிலி எழுத்து *) dec)[dec_size] == '\0'); /* unhexmem appends NUL for our convenience */
        எனில் (memchr(dec, '\0', dec_size) || string_has_cc(dec, வெற்று) || !filename_is_valid(dec))
                திரும்பு -EUCLEAN;

        *ret_name = TAKE_PTR(dec);
        திரும்பு 0;
}

முழு terminal_get_terminfo_by_dcs(முழு fd, எழுத்து **ret_name) {
        முழு r;

        ப்ரதிஜ்ஞா(fd >= 0);
        ப்ரதிஜ்ஞா(ret_name);

        /* Note: fd must be in non-blocking read-write mode! */

        கட்டமைப்பு termios old_termios = TERMIOS_NULL;
        CLEANUP_TERMIOS_RESET(fd, old_termios);

        எனில் (tcgetattr(fd, &old_termios) < 0)
                திரும்பு -errno;

        கட்டமைப்பு termios new_termios = old_termios;
        termios_disable_echo(&new_termios);

        எனில் (tcsetattr(fd, TCSANOW, &new_termios) < 0)
                திரும்பு -errno;

        r = loop_write(fd, DCS_TERMINFO_Q, SIZE_MAX);
        எனில் (r < 0)
                திரும்பு r;

        usec_t end = usec_add(now(CLOCK_MONOTONIC), CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        எழுத்து buf[STRLEN(DCS_TERMINFO_R1) + MAX_TERMINFO_LENGTH + STRLEN(ANSI_ST)];
        size_t bytes = 0;

        க்கு (;;) {
                usec_t n = now(CLOCK_MONOTONIC);
                எனில் (n >= end)
                        திரும்பு -EOPNOTSUPP;

                r = fd_wait_for_event(fd, POLLIN, usec_sub_unsigned(end, n));
                எனில் (r < 0)
                        திரும்பு r;
                எனில் (r == 0)
                        திரும்பு -EOPNOTSUPP;

                /* On the first read, read multiple characters, i.e. the shortest valid reply. Afterwards
                 * read byte by byte, since we don't want to read too much and drop characters from the input
                 * queue. */
                ssize_t l = பட(fd, buf + bytes, bytes == 0 ? STRLEN(DCS_TERMINFO_R0) : 1);
                எனில் (l < 0) {
                        எனில் (errno == EAGAIN)
                                தொடர்;
                        திரும்பு -errno;
                }

                ப்ரதிஜ்ஞா((size_t) l <= அளவு(buf) - bytes);
                bytes += l;

                r = scan_terminfo_response(buf, bytes, ret_name);
                எனில் (r != -EAGAIN)
                        திரும்பு r;

                எனில் (bytes == அளவு(buf))
                        திரும்பு -EOPNOTSUPP; /* The response has the right prefix, but we didn't find a valid
                                             * answer with a terminator in the allotted space. Something is
                                             * wrong, possibly some unrelated bytes got injected into the
                                             * answer. */
        }
}

முழு have_terminfo_file(மாறிலி எழுத்து *name) {
        /* This is a heuristic check if we have the file, using the directory layout used on
         * current Linux systems. Checks for other layouts can be added later if appropriate. */
        முழு r;

        ப்ரதிஜ்ஞா(filename_is_valid(name));

        _cleanup_free_ எழுத்து *p = path_join("/usr/share/terminfo", CHAR_TO_STR(name[0]), name);
        எனில் (!p)
                திரும்பு log_oom_debug();

        r = RET_NERRNO(access(p, F_OK));
        எனில் (r == -ENOENT)
                திரும்பு false;
        எனில் (r < 0)
                திரும்பு r;
        திரும்பு true;
}

முழு query_term_for_tty(மாறிலி எழுத்து *tty, எழுத்து **ret_term) {
        _cleanup_free_ எழுத்து *dcs_term = வெற்று;
        முழு r;

        ப்ரதிஜ்ஞா(tty);
        ப்ரதிஜ்ஞா(ret_term);

        எனில் (tty_is_vc_resolve(tty))
                திரும்பு strdup_to(ret_term, "linux");

        /* Try to query the terminal implementation that we're on. This will not work in all
         * cases, which is fine, since this is intended to be used as a fallback. */

        _cleanup_close_ முழு tty_fd = open_terminal(tty, O_RDWR|O_NOCTTY|O_CLOEXEC|O_NONBLOCK);
        எனில் (tty_fd < 0)
                திரும்பு log_debug_errno(tty_fd, "Failed to open %s to query terminfo: %m", tty);

        r = terminal_get_terminfo_by_dcs(tty_fd, &dcs_term);
        எனில் (r < 0)
                திரும்பு log_debug_errno(r, "Failed to query %s for terminfo: %m", tty);

        r = have_terminfo_file(dcs_term);
        எனில் (r < 0)
                திரும்பு log_debug_errno(r, "Failed to look for terminfo %s: %m", dcs_term);
        எனில் (r == 0)
                திரும்பு log_info_errno(SYNTHETIC_ERRNO(ENODATA),
                                      "Terminfo %s not found for %s.", dcs_term, tty);

        *ret_term = TAKE_PTR(dcs_term);
        திரும்பு 0;
}

முழு terminal_is_pty_fd(முழு fd) {
        முழு r;

        ப்ரதிஜ்ஞா(fd >= 0);

        /* Returns true if we are looking at a pty, i.e. if it's backed by the /dev/pts/ file system */

        எனில் (!isatty_safe(fd))
                திரும்பு false;

        r = is_fs_type_at(fd, வெற்று, DEVPTS_SUPER_MAGIC);
        எனில் (r != 0)
                திரும்பு r;

        /* The ptmx device is weird, it exists twice, once inside and once outside devpts. To detect the
         * latter case, let's fire off an ioctl() that only works on ptmx devices. */

        முழு v;
        எனில் (ioctl(fd, TIOCGPKT, &v) < 0) {
                எனில் (ERRNO_IS_NOT_SUPPORTED(errno))
                        திரும்பு false;

                திரும்பு -errno;
        }

        திரும்பு true;
}

முழு pty_open_peer(முழு fd, முழு mode) {
        ப்ரதிஜ்ஞா(fd >= 0);

        /* Opens the peer PTY using the new race-free TIOCGPTPEER ioctl() (kernel 4.13).
         *
         * This is safe to be called on TTYs from other namespaces. */

        ப்ரதிஜ்ஞா((mode & (O_CREAT|O_PATH|O_DIRECTORY|O_TMPFILE)) == 0);

        /* This replicates the EIO retry logic of open_terminal() in a modified way. */
        க்கு (unsigned c = 0;; c++) {
                முழு peer_fd = ioctl(fd, TIOCGPTPEER, mode);
                எனில் (peer_fd >= 0)
                        திரும்பு peer_fd;

                எனில் (errno != EIO)
                        திரும்பு -errno;

                /* Max 1s in total */
                எனில் (c >= 20)
                        திரும்பு -EIO;

                (வெறுமை) usleep_safe(50 * USEC_PER_MSEC);
        }
}
