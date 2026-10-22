/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <fcntl.h>
#ಅನ್ತರ್ಭಾವ <linux/kd.h>
#ಅನ್ತರ್ಭಾವ <linux/magic.h>
#ಅನ್ತರ್ಭಾವ <linux/tiocl.h>
#ಅನ್ತರ್ಭಾವ <linux/vt.h>
#ಅನ್ತರ್ಭಾವ <poll.h>
#ಅನ್ತರ್ಭಾವ <signal.h>
#ಅನ್ತರ್ಭಾವ <stdlib.h>
#ಅನ್ತರ್ಭಾವ <sys/inotify.h>
#ಅನ್ತರ್ಭಾವ <sys/ioctl.h>
#ಅನ್ತರ್ಭಾವ <sys/sysmacros.h>
#ಅನ್ತರ್ಭಾವ <termios.h>
#ಅನ್ತರ್ಭಾವ <time.h>
#ಅನ್ತರ್ಭಾವ <unistd.h>

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "ansi-color.h"
#ಅನ್ತರ್ಭಾವ "chase.h"
#ಅನ್ತರ್ಭಾವ "devnum-util.h"
#ಅನ್ತರ್ಭಾವ "errno-util.h"
#ಅನ್ತರ್ಭಾವ "extract-word.h"
#ಅನ್ತರ್ಭಾವ "fd-util.h"
#ಅನ್ತರ್ಭಾವ "fileio.h"
#ಅನ್ತರ್ಭಾವ "fs-util.h"
#ಅನ್ತರ್ಭಾವ "hexdecoct.h"
#ಅನ್ತರ್ಭಾವ "inotify-util.h"
#ಅನ್ತರ್ಭಾವ "io-util.h"
#ಅನ್ತರ್ಭಾವ "log.h"
#ಅನ್ತರ್ಭಾವ "namespace-util.h"
#ಅನ್ತರ್ಭಾವ "parse-util.h"
#ಅನ್ತರ್ಭಾವ "path-util.h"
#ಅನ್ತರ್ಭಾವ "pidref.h"
#ಅನ್ತರ್ಭಾವ "proc-cmdline.h"
#ಅನ್ತರ್ಭಾವ "process-util.h"
#ಅನ್ತರ್ಭಾವ "signal-util.h"
#ಅನ್ತರ್ಭಾವ "socket-util.h"
#ಅನ್ತರ್ಭಾವ "stat-util.h"
#ಅನ್ತರ್ಭಾವ "stdio-util.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"
#ಅನ್ತರ್ಭಾವ "strv.h"
#ಅನ್ತರ್ಭಾವ "terminal-util.h"
#ಅನ್ತರ್ಭಾವ "time-util.h"
#ಅನ್ತರ್ಭಾವ "utf8.h"

/* How much to wait when reading/writing ANSI sequences from/to the console */
#define CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC (333 * USEC_PER_MSEC)

ಸ್ಥಿರ ಚಞ್ಚಲ unsigned cached_columns = 0;
ಸ್ಥಿರ ಚಞ್ಚಲ unsigned cached_ಸಾಲುಗಳು = 0;

ಸ್ಥಿರ ಚಞ್ಚಲ ಪೂರ್ಣಾಙ್ಕ cached_on_tty = -1;
ಸ್ಥಿರ ಚಞ್ಚಲ ಪೂರ್ಣಾಙ್ಕ cached_on_dev_null = -1;

bool isatty_safe(ಪೂರ್ಣಾಙ್ಕ fd) {
        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        ಯದಿ (isatty(fd))
                ಪ್ರತ್ಯಾಗಮ true;

        /* Linux/glibc returns EIO for hung up TTY on isatty(). Which is wrong, the thing doesn't stop being
         * a TTY after all, just because it is temporarily hung up. Let's work around this here, until this
         * is fixed in glibc. See: https://sourceware.org/bugzilla/show_bug.cgi?id=32103 */
        ಯದಿ (errno == EIO)
                ಪ್ರತ್ಯಾಗಮ true;

        /* Be resilient if we're working on stdio, since they're set up by parent process. */
        ಪ್ರತಿಜ್ಞಾ(errno != EBADF || IN_SET(fd, STDIN_FILENO, STDOUT_FILENO, STDERR_FILENO));

        ಪ್ರತ್ಯಾಗಮ false;
}

ಪೂರ್ಣಾಙ್ಕ chvt(ಪೂರ್ಣಾಙ್ಕ vt) {
        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ fd = -EBADF;

        /* Switch to the specified vt number. If the VT is specified <= 0 switch to the VT the kernel log messages go,
         * if that's configured. */

        fd = open_terminal("/dev/tty0", O_RDWR|O_NOCTTY|O_CLOEXEC|O_NONBLOCK);
        ಯದಿ (fd < 0)
                ಪ್ರತ್ಯಾಗಮ fd;

        ಯದಿ (vt <= 0) {
                ಪೂರ್ಣಾಙ್ಕ tiocl[2] = {
                        TIOCL_GETKMSGREDIRECT,
                        0
                };

                ಯದಿ (ioctl(fd, TIOCLINUX, tiocl) < 0)
                        ಪ್ರತ್ಯಾಗಮ -errno;

                vt = tiocl[0] <= 0 ? 1 : tiocl[0];
        }

        ಪ್ರತ್ಯಾಗಮ RET_NERRNO(ioctl(fd, VT_ACTIVATE, vt));
}

ಪೂರ್ಣಾಙ್ಕ read_one_char(FILE *f, ವರ್ಣ *ret, usec_t t, bool echo, bool *need_nl) {
        _cleanup_free_ ವರ್ಣ *ಸಾಲು = ರಿಕ್ತ;
        ಸಂರಚನಾ termios old_termios;
        ಪೂರ್ಣಾಙ್ಕ r, fd;

        ಪ್ರತಿಜ್ಞಾ(ret);

        ಯದಿ (!f)
                f = stdin;

        /* If this is a terminal, then switch canonical mode off, so that we can read a single
         * character. (Note that fmemopen() streams do not have an fd associated with them, let's handle that
         * nicely.) If 'echo' is false we'll also disable ECHO mode so that the pressed key is not made
         * visible to the user. */
        fd = fileno(f);
        ಯದಿ (fd >= 0 && tcgetattr(fd, &old_termios) >= 0) {
                ಸಂರಚನಾ termios new_termios = old_termios;

                new_termios.c_lflag &= ~(ICANON|(echo ? 0 : ECHO));
                new_termios.c_cc[VMIN] = 1;
                new_termios.c_cc[VTIME] = 0;

                ಯದಿ (tcsetattr(fd, TCSANOW, &new_termios) >= 0) {
                        ವರ್ಣ c;

                        ಯದಿ (t != USEC_INFINITY) {
                                ಯದಿ (fd_wait_for_event(fd, POLLIN, t) <= 0) {
                                        (ಶೂನ್ಯ) tcsetattr(fd, TCSANOW, &old_termios);
                                        ಪ್ರತ್ಯಾಗಮ -ETIMEDOUT;
                                }
                        }

                        r = safe_fgetc(f, &c);
                        (ಶೂನ್ಯ) tcsetattr(fd, TCSANOW, &old_termios);
                        ಯದಿ (r < 0)
                                ಪ್ರತ್ಯಾಗಮ r;
                        ಯದಿ (r == 0)
                                ಪ್ರತ್ಯಾಗಮ -EIO;

                        ಯದಿ (need_nl)
                                *need_nl = c != '\n';

                        *ret = c;
                        ಪ್ರತ್ಯಾಗಮ 0;
                }
        }

        ಯದಿ (t != USEC_INFINITY && fd >= 0) {
                /* Let's wait the specified amount of time for input. When we have no fd we skip this, under
                 * the assumption that this is an fmemopen() stream or so where waiting doesn't make sense
                 * anyway, as the data is either already in the stream or cannot possible be placed there
                 * while we access the stream */

                ಯದಿ (fd_wait_for_event(fd, POLLIN, t) <= 0)
                        ಪ್ರತ್ಯಾಗಮ -ETIMEDOUT;
        }

        /* If this is not a terminal, then read a full line instead */

        r = read_ಸಾಲು(f, 16, &ಸಾಲು); /* longer than necessary, to eat up UTF-8 chars/vt100 key sequences */
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;
        ಯದಿ (r == 0)
                ಪ್ರತ್ಯಾಗಮ -EIO;

        ಯದಿ (strlen(ಸಾಲು) != 1)
                ಪ್ರತ್ಯಾಗಮ -EBADMSG;

        ಯದಿ (need_nl)
                *need_nl = false;

        *ret = ಸಾಲು[0];
        ಪ್ರತ್ಯಾಗಮ 0;
}

#define DEFAULT_ASK_REFRESH_USEC (2*USEC_PER_SEC)

ಪೂರ್ಣಾಙ್ಕ ask_char(ವರ್ಣ *ret, ಧ್ರುವ ವರ್ಣ *replies, ಧ್ರುವ ವರ್ಣ *fmt, ...) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);
        ಪ್ರತಿಜ್ಞಾ(replies);
        ಪ್ರತಿಜ್ಞಾ(fmt);

        ಪ್ರತಿ (;;) {
                va_list ap;
                ವರ್ಣ c;
                bool need_nl = true;

                fputs(ansi_highlight(), stdout);

                putchar('\r');

                va_start(ap, fmt);
                vprintf(fmt, ap);
                va_end(ap);

                fputs(ansi_normal(), stdout);

                fflush(stdout);

                r = read_one_char(stdin, &c, DEFAULT_ASK_REFRESH_USEC, /* echo= */ true, &need_nl);
                ಯದಿ (r < 0) {

                        ಯದಿ (r == -ETIMEDOUT)
                                ಅನುವರ್ತ;

                        ಯದಿ (r == -EBADMSG) {
                                puts("Bad input, please try again.");
                                ಅನುವರ್ತ;
                        }

                        putchar('\n');
                        ಪ್ರತ್ಯಾಗಮ r;
                }

                ಯದಿ (need_nl)
                        putchar('\n');

                ಯದಿ (strchr(replies, c)) {
                        *ret = c;
                        ಪ್ರತ್ಯಾಗಮ 0;
                }

                puts("Read unexpected character, please try again.");
        }
}

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ CompletionResult{
        COMPLETION_ALREADY,       /* the input string is already complete */
        COMPLETION_FULL,          /* completed the input string to be complete now */
        COMPLETION_PARTIAL,       /* completed the input string so that is still incomplete */
        COMPLETION_NONE,          /* found no matching completion */
        _COMPLETION_RESULT_MAX,
        _COMPLETION_RESULT_INVALID = -EINVAL,
        _COMPLETION_RESULT_ERRNO_MAX = -ERRNO_MAX,
} CompletionResult;

ಸ್ಥಿರ CompletionResult pick_completion(ಧ್ರುವ ವರ್ಣ *string, ವರ್ಣ *ಧ್ರುವ*completions, ವರ್ಣ **ret) {
        _cleanup_free_ ವರ್ಣ *found = ರಿಕ್ತ;
        bool partial = false;

        ಪ್ರತಿಜ್ಞಾ(ret);

        string = strempty(string);

        STRV_FOREACH(c, completions) {

                /* Ignore entries that are not actually completions */
                ಯದಿ (!startswith(*c, string))
                        ಅನುವರ್ತ;

                /* Store first completion that matches */
                ಯದಿ (!found) {
                        found = strdup(*c);
                        ಯದಿ (!found)
                                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                        ಅನುವರ್ತ;
                }

                /* If there's another completion that works truncate the one we already found by common
                 * prefix */
                size_t n = str_common_prefix(found, *c);
                ಯದಿ (n == SIZE_MAX)
                        ಅನುವರ್ತ;

                found[n] = 0;
                partial = true;
        }

        *ret = TAKE_PTR(found);

        ಯದಿ (!*ret)
                ಪ್ರತ್ಯಾಗಮ COMPLETION_NONE;
        ಯದಿ (partial)
                ಪ್ರತ್ಯಾಗಮ COMPLETION_PARTIAL;

        ಪ್ರತ್ಯಾಗಮ streq(string, *ret) ? COMPLETION_ALREADY : COMPLETION_FULL;
}

ಸ್ಥಿರ ಶೂನ್ಯ clear_by_backspace(size_t n) {
        /* Erase the specified number of character cells backwards on the terminal */
        ಪ್ರತಿ (size_t i = 0; i < n; i++)
                fputs("\b \b", stdout);
}

ಪೂರ್ಣಾಙ್ಕ ask_string_full(
                ವರ್ಣ **ret,
                ಧ್ರುವ ವರ್ಣ *prefill,
                GetCompletionsCallback get_completions,
                ಶೂನ್ಯ *userdata,
                ಧ್ರುವ ವರ್ಣ *text, ...) {

        va_list ap;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);
        ಪ್ರತಿಜ್ಞಾ(text);

        _cleanup_free_ ವರ್ಣ *string = ರಿಕ್ತ;
        size_t n = 0;

        ಯದಿ (prefill) {
                /* Prefill query with explicit data if specified */

                string = strdup(prefill);
                ಯದಿ (!string)
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                n = strlen(string);

        } ಅನ್ಯಥಾ ಯದಿ (get_completions) {
                /* Otherwise, figure out what string to preselect the query with */
                _cleanup_strv_free_ ವರ್ಣ **completions = ರಿಕ್ತ;
                r = get_completions("", GET_COMPLETIONS_PRESELECT, &completions, userdata);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                CompletionResult cr = pick_completion(string, completions, &string);
                ಯದಿ (cr < 0)
                        ಪ್ರತ್ಯಾಗಮ cr;

                n = strlen_ptr(string);
        }

        /* Output the prompt */
        fputs(ansi_highlight(), stdout);
        va_start(ap, text);
        vprintf(text, ap);
        va_end(ap);
        fputs(ansi_normal(), stdout);
        ಯದಿ (string)
                fputs(string, stdout);
        fflush(stdout);

        /* Do interactive logic only if stdin + stdout are connected to the same place. And yes, we could use
         * STDIN_FILENO and STDOUT_FILENO here, but let's be overly correct for once, after all libc allows
         * swapping out stdin/stdout. */
        ಪೂರ್ಣಾಙ್ಕ fd_ಒಳಹರಿವು = fileno(stdin);
        ಪೂರ್ಣಾಙ್ಕ fd_output = fileno(stdout);
        ಸಂರಚನಾ termios old_termios = TERMIOS_NULL;
        CLEANUP_TERMIOS_RESET(fd_ಒಳಹರಿವು, old_termios);

        ಯದಿ (fd_ಒಳಹರಿವು < 0 || fd_output < 0 || same_fd(fd_ಒಳಹರಿವು, fd_output) <= 0)
                ಗಚ್ಛ fallback;

        /* Try to disable echo, which also tells us if this even is a terminal */
        ಯದಿ (tcgetattr(fd_ಒಳಹರಿವು, &old_termios) < 0) {
                old_termios = TERMIOS_NULL;
                ಗಚ್ಛ fallback;
        }

        ಸಂರಚನಾ termios new_termios = old_termios;
        termios_disable_echo(&new_termios);
        ಯದಿ (tcsetattr(fd_ಒಳಹರಿವು, TCSANOW, &new_termios) < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        ಪ್ರತಿ (;;) {
                ಪೂರ್ಣಾಙ್ಕ c = fgetc(stdin);

                /* On EOF or NUL, end the request, don't output anything anymore */
                ಯದಿ (IN_SET(c, EOF, 0))
                        ಭಙ್ಗ;

                /* On Return also end the request, but make this visible */
                ಯದಿ (IN_SET(c, '\n', '\r')) {
                        fputc('\n', stdout);
                        ಭಙ್ಗ;
                }

                ಯದಿ (c == '\t') {
                        /* Tab */

                        _cleanup_strv_free_ ವರ್ಣ **completions = ರಿಕ್ತ;
                        ಯದಿ (get_completions) {
                                r = get_completions(string, /* flags= */ 0, &completions, userdata);
                                ಯದಿ (r < 0)
                                        ಪ್ರತ್ಯಾಗಮ r;
                        }

                        _cleanup_free_ ವರ್ಣ *new_string = ರಿಕ್ತ;
                        CompletionResult cr = pick_completion(string, completions, &new_string);
                        ಯದಿ (cr < 0)
                                ಪ್ರತ್ಯಾಗಮ cr;
                        ಯದಿ (IN_SET(cr, COMPLETION_PARTIAL, COMPLETION_FULL)) {
                                /* Output the new suffix we learned */
                                fputs(ASSERT_PTR(startswith(new_string, strempty(string))), stdout);

                                /* And update the whole string */
                                free_and_replace(string, new_string);
                                n = strlen(string);
                        }
                        ಯದಿ (cr == COMPLETION_NONE)
                                fputc('\a', stdout); /* BEL */

                        ಯದಿ (IN_SET(cr, COMPLETION_PARTIAL, COMPLETION_ALREADY)) {
                                /* If this worked only partially, or if the user hit TAB even though we were
                                 * complete already, then show the remaining options (in the latter case just
                                 * the one). */
                                fputc('\n', stdout);

                                _cleanup_strv_free_ ವರ್ಣ **filtered = strv_filter_prefix(completions, string);
                                ಯದಿ (!filtered)
                                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                                r = show_menu(filtered,
                                              /* n_columns= */ SIZE_MAX,
                                              /* column_width= */ SIZE_MAX,
                                              /* ellipsize_percentage= */ 0,
                                              /* grey_prefix= */ string,
                                              /* with_numbers= */ false);
                                ಯದಿ (r < 0)
                                        ಪ್ರತ್ಯಾಗಮ r;

                                /* Show the prompt again */
                                fputs(ansi_highlight(), stdout);
                                va_start(ap, text);
                                vprintf(text, ap);
                                va_end(ap);
                                fputs(ansi_normal(), stdout);
                                fputs(string, stdout);
                        }

                } ಅನ್ಯಥಾ ಯದಿ (IN_SET(c, '\b', 127)) {
                        /* Backspace */

                        ಯದಿ (n == 0)
                                fputc('\a', stdout); /* BEL */
                        ಅನ್ಯಥಾ {
                                size_t m = utf8_last_length(string, n);

                                ವರ್ಣ *e = string + n - m;
                                clear_by_backspace(utf8_console_width(e));

                                *e = 0;
                                n -= m;
                        }

                } ಅನ್ಯಥಾ ಯದಿ (c == 21) {
                        /* Ctrl-u → erase all input */

                        clear_by_backspace(utf8_console_width(string));
                        ಯದಿ (string)
                                string[n = 0] = 0;
                        ಅನ್ಯಥಾ
                                ಪ್ರತಿಜ್ಞಾ(n == 0);

                } ಅನ್ಯಥಾ ಯದಿ (c == 4) {
                        /* Ctrl-d → cancel this field input */

                        ಪ್ರತ್ಯಾಗಮ -ECANCELED;

                } ಅನ್ಯಥಾ ಯದಿ (char_is_cc(c) || n >= LINE_MAX)
                        /* refuse control characters and too long strings */
                        fputc('\a', stdout); /* BEL */
                ಅನ್ಯಥಾ {
                        /* Regular char */

                        ಯದಿ (!GREEDY_REALLOC(string, n+2))
                                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                        string[n++] = (ವರ್ಣ) c;
                        string[n] = 0;

                        fputc(c, stdout);
                }

                fflush(stdout);
        }

        ಯದಿ (!string) {
                string = strdup("");
                ಯದಿ (!string)
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;
        }

        *ret = TAKE_PTR(string);
        ಪ್ರತ್ಯಾಗಮ 0;

fallback:
        /* A simple fallback without TTY magic */
        string = mfree(string);
        r = read_ಸಾಲು(stdin, LONG_ಸಾಲು_MAX, &string);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;
        ಯದಿ (r == 0)
                ಪ್ರತ್ಯಾಗಮ -EIO;

        *ret = TAKE_PTR(string);
        ಪ್ರತ್ಯಾಗಮ 0;
}

bool any_ಕೀಲಿ_to_proceed(ಶೂನ್ಯ) {

        /* Insert a new line here as well as to when the user inputs, as this is also used during the boot up
         * sequence when status messages may be interleaved with the current program output. This ensures
         * that the status messages aren't appended on the same line as this message. */

        fputc('\n', stdout);
        fputs(ansi_highlight_magenta(), stdout);
        fputs("-- Press any key to proceed --", stdout);
        fputs(ansi_normal(), stdout);
        fputc('\n', stdout);
        fflush(stdout);

        ವರ್ಣ ಕೀಲಿ = 0;
        (ಶೂನ್ಯ) read_one_char(stdin, &ಕೀಲಿ, USEC_INFINITY, /* echo= */ false, /* need_nl= */ ರಿಕ್ತ);

        fputc('\n', stdout);
        fflush(stdout);

        ಪ್ರತ್ಯಾಗಮ ಕೀಲಿ != 'q';
}

ಸ್ಥಿರ size_t widest_list_element(ವರ್ಣ *ಧ್ರುವ*l) {
        size_t w = 0;

        /* Returns the largest console width of all elements in 'l' */

        STRV_FOREACH(i, l)
                w = MAX(w, utf8_console_width(*i));

        ಪ್ರತ್ಯಾಗಮ w;
}

ಪೂರ್ಣಾಙ್ಕ show_menu(ವರ್ಣ **x,
              size_t n_columns,
              size_t column_width,
              unsigned ellipsize_percentage,
              ಧ್ರುವ ವರ್ಣ *grey_prefix,
              bool with_numbers) {

        ಪ್ರತಿಜ್ಞಾ(n_columns > 0);
        ಪ್ರತಿಜ್ಞಾ(column_width > 0);

        ಯದಿ (n_columns == SIZE_MAX)
                n_columns = 3;

        ಯದಿ (column_width == SIZE_MAX) {
                size_t widest = widest_list_element(x);

                /* If not specified, derive column width from screen width */
                size_t column_max = (columns()-1) / n_columns;

                /* Subtract room for numbers */
                ಯದಿ (with_numbers)
                        column_max = LESS_BY(column_max, 6U);

                /* If columns would get too tight let's make this a linear list instead. */
                ಯದಿ (column_max < 10 && widest > 10) {
                        n_columns = 1;
                        column_max = columns()-1;

                        ಯದಿ (with_numbers)
                                column_max = LESS_BY(column_max, 6U);
                }

                /* Never make this narrower than 10 characters */
                column_max = MAX(column_max, 10U);
                column_width = CLAMP(widest+1, 10U, column_max);
        }

        size_t n = strv_length(x);
        size_t per_column = DIV_ROUND_UP(n, n_columns);

        size_t break_ಸಾಲುಗಳು = ಸಾಲುಗಳು();
        ಯದಿ (break_ಸಾಲುಗಳು > 2)
                break_ಸಾಲುಗಳು--;

        /* The first page gets two extra lines, since we want to show a title */
        size_t break_modulo = break_ಸಾಲುಗಳು;
        ಯದಿ (break_modulo > 3)
                break_modulo -= 3;

        ಪ್ರತಿ (size_t i = 0; i < per_column; i++) {

                ಪ್ರತಿ (size_t j = 0; j < n_columns; j++) {
                        _cleanup_free_ ವರ್ಣ *e = ರಿಕ್ತ;

                        size_t p = j * per_column + i;
                        ಯದಿ (p >= n)
                                ಭಙ್ಗ;

                        e = ellipsize(x[p], column_width, ellipsize_percentage);
                        ಯದಿ (!e)
                                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                        ಯದಿ (with_numbers)
                                printf("%s%4zu)%s ",
                                       ansi_grey(),
                                       p + 1,
                                       ansi_normal());

                        ಯದಿ (grey_prefix && startswith(e, grey_prefix)) {
                                size_t k = MIN(strlen(grey_prefix), column_width);
                                printf("%s%.*s%s",
                                       ansi_grey(),
                                       (ಪೂರ್ಣಾಙ್ಕ) k, e,
                                       ansi_normal());
                                printf("%-*s",
                                       (ಪೂರ್ಣಾಙ್ಕ) (column_width - k), e+k);
                        } ಅನ್ಯಥಾ
                                printf("%-*s", (ಪೂರ್ಣಾಙ್ಕ) column_width, e);
                }

                putchar('\n');

                /* on the first screen we reserve 2 extra lines for the title */
                ಯದಿ (i % break_ಸಾಲುಗಳು == break_modulo)
                        ಯದಿ (!any_ಕೀಲಿ_to_proceed())
                                ಪ್ರತ್ಯಾಗಮ 0;
        }

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ open_terminal(ಧ್ರುವ ವರ್ಣ *name, ಪೂರ್ಣಾಙ್ಕ mode) {
        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ fd = -EBADF;

        /*
         * If a TTY is in the process of being closed opening it might cause EIO. This is horribly awful, but
         * unlikely to be changed in the kernel. Hence we work around this problem by retrying a couple of
         * times.
         *
         * https://bugs.launchpad.net/ubuntu/+source/linux/+bug/554172/comments/245
         */

        ಪ್ರತಿಜ್ಞಾ((mode & (O_CREAT|O_PATH|O_DIRECTORY|O_TMPFILE)) == 0);

        ಪ್ರತಿ (unsigned c = 0;; c++) {
                fd = open(name, mode, 0);
                ಯದಿ (fd >= 0)
                        ಭಙ್ಗ;

                ಯದಿ (errno != EIO)
                        ಪ್ರತ್ಯಾಗಮ -errno;

                /* Max 1s in total */
                ಯದಿ (c >= 20)
                        ಪ್ರತ್ಯಾಗಮ -EIO;

                (ಶೂನ್ಯ) usleep_safe(50 * USEC_PER_MSEC);
        }

        ಯದಿ (!isatty_safe(fd))
                ಪ್ರತ್ಯಾಗಮ -ENOTTY;

        ಪ್ರತ್ಯಾಗಮ TAKE_FD(fd);
}

ಪೂರ್ಣಾಙ್ಕ acquire_terminal(
                ಧ್ರುವ ವರ್ಣ *name,
                AcquireTerminalFlags flags,
                usec_t timeout) {

        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ notify = -EBADF, fd = -EBADF;
        usec_t ts = USEC_INFINITY;
        ಪೂರ್ಣಾಙ್ಕ r, wd = -1;

        ಪ್ರತಿಜ್ಞಾ(name);

        AcquireTerminalFlags mode = flags & _ACQUIRE_TERMINAL_MODE_MASK;
        ಪ್ರತಿಜ್ಞಾ(IN_SET(mode, ACQUIRE_TERMINAL_TRY, ACQUIRE_TERMINAL_FORCE, ACQUIRE_TERMINAL_WAIT));
        ಪ್ರತಿಜ್ಞಾ(mode == ACQUIRE_TERMINAL_WAIT || !FLAGS_SET(flags, ACQUIRE_TERMINAL_WATCH_SIGTERM));

        /* We use inotify to be notified when the tty is closed. We create the watch before checking if we can actually
         * acquire it, so that we don't lose any event.
         *
         * Note: strictly speaking this actually watches for the device being closed, it does *not* really watch
         * whether a tty loses its controlling process. However, unless some rogue process uses TIOCNOTTY on /dev/tty
         * *after* closing its tty otherwise this will not become a problem. As long as the administrator makes sure to
         * not configure any service on the same tty as an untrusted user this should not be a problem. (Which they
         * probably should not do anyway.) */

        ಯದಿ (mode == ACQUIRE_TERMINAL_WAIT) {
                notify = inotify_init1(IN_CLOEXEC | IN_NONBLOCK);
                ಯದಿ (notify < 0)
                        ಪ್ರತ್ಯಾಗಮ -errno;

                wd = inotify_add_watch(notify, name, IN_CLOSE);
                ಯದಿ (wd < 0)
                        ಪ್ರತ್ಯಾಗಮ -errno;

                ಯದಿ (timeout != USEC_INFINITY)
                        ts = now(CLOCK_MONOTONIC);
        }

        /* If we are called with ACQUIRE_TERMINAL_WATCH_SIGTERM we'll unblock SIGTERM during ppoll() temporarily */
        sigset_t poll_ss;
        assert_se(sigprocmask(SIG_SETMASK, /* newset= */ ರಿಕ್ತ, &poll_ss) >= 0);
        ಯದಿ (flags & ACQUIRE_TERMINAL_WATCH_SIGTERM) {
                assert_se(sigismember(&poll_ss, SIGTERM) > 0);
                assert_se(sigdelset(&poll_ss, SIGTERM) >= 0);
        }

        ಪ್ರತಿ (;;) {
                ಯದಿ (notify >= 0) {
                        r = flush_fd(notify);
                        ಯದಿ (r < 0)
                                ಪ್ರತ್ಯಾಗಮ r;
                }

                /* We pass here O_NOCTTY only so that we can check the return value TIOCSCTTY and have a reliable way
                 * to figure out if we successfully became the controlling process of the tty */
                fd = open_terminal(name, O_RDWR|O_NOCTTY|O_CLOEXEC);
                ಯದಿ (fd < 0)
                        ಪ್ರತ್ಯಾಗಮ fd;

                /* Temporarily ignore SIGHUP, so that we don't get SIGHUP'ed if we already own the tty. */
                ಸಂರಚನಾ sigaction sa_old;
                assert_se(sigaction(SIGHUP, &sigaction_ignore, &sa_old) >= 0);

                /* First, try to get the tty */
                r = RET_NERRNO(ioctl(fd, TIOCSCTTY, mode == ACQUIRE_TERMINAL_FORCE));

                /* Reset signal handler to old value */
                assert_se(sigaction(SIGHUP, &sa_old, ರಿಕ್ತ) >= 0);

                /* Success? Exit the loop now! */
                ಯದಿ (r >= 0)
                        ಭಙ್ಗ;

                /* Any failure besides -EPERM? Fail, regardless of the mode. */
                ಯದಿ (r != -EPERM)
                        ಪ್ರತ್ಯಾಗಮ r;

                ಯದಿ (flags & ACQUIRE_TERMINAL_PERMISSIVE) /* If we are in permissive mode, then EPERM is fine, turn this
                                                          * into a success. Note that EPERM is also returned if we
                                                          * already are the owner of the TTY. */
                        ಭಙ್ಗ;

                ಯದಿ (mode != ACQUIRE_TERMINAL_WAIT) /* If we are in TRY or FORCE mode, then propagate EPERM as EPERM */
                        ಪ್ರತ್ಯಾಗಮ r;

                ಪ್ರತಿಜ್ಞಾ(notify >= 0);
                ಪ್ರತಿಜ್ಞಾ(wd >= 0);

                ಪ್ರತಿ (;;) {
                        usec_t ಎಡ;
                        ಯದಿ (timeout == USEC_INFINITY)
                                ಎಡ = USEC_INFINITY;
                        ಅನ್ಯಥಾ {
                                ಪ್ರತಿಜ್ಞಾ(ts != USEC_INFINITY);

                                usec_t n = usec_sub_unsigned(now(CLOCK_MONOTONIC), ts);
                                ಯದಿ (n >= timeout)
                                        ಪ್ರತ್ಯಾಗಮ -ETIMEDOUT;

                                ಎಡ = timeout - n;
                        }

                        r = ppoll_usec_full(
                                        &(ಸಂರಚನಾ pollfd) {
                                                .fd = notify,
                                                .events = POLLIN,
                                        },
                                        /* n_fds= */ 1,
                                        ಎಡ,
                                        &poll_ss);
                        ಯದಿ (r < 0)
                                ಪ್ರತ್ಯಾಗಮ r;
                        ಯದಿ (r == 0)
                                ಪ್ರತ್ಯಾಗಮ -ETIMEDOUT;

                        ಸಙ್ಘ inotify_event_buffer buffer;
                        ssize_t l;
                        l = ಪಠ(notify, &buffer, ಪರಿಮಾಣ(buffer));
                        ಯದಿ (l < 0) {
                                ಯದಿ (ERRNO_IS_TRANSIENT(errno))
                                        ಅನುವರ್ತ;

                                ಪ್ರತ್ಯಾಗಮ -errno;
                        }

                        FOREACH_INOTIFY_EVENT(e, buffer, l) {
                                ಯದಿ (e->mask & IN_Q_OVERFLOW) /* If we hit an inotify queue overflow, simply check if the terminal is up for grabs now. */
                                        ಭಙ್ಗ;

                                ಯದಿ (e->wd != wd || !(e->mask & IN_CLOSE)) /* Safety checks */
                                        ಪ್ರತ್ಯಾಗಮ -EIO;
                        }

                        ಭಙ್ಗ;
                }

                /* We close the tty fd here since if the old session ended our handle will be dead. It's important that
                 * we do this after sleeping, so that we don't enter an endless loop. */
                fd = safe_close(fd);
        }

        ಪ್ರತ್ಯಾಗಮ TAKE_FD(fd);
}

ಪೂರ್ಣಾಙ್ಕ release_terminal(ಶೂನ್ಯ) {
        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ fd = -EBADF;
        ಪೂರ್ಣಾಙ್ಕ r;

        fd = open("/dev/tty", O_RDWR|O_NOCTTY|O_CLOEXEC|O_NONBLOCK);
        ಯದಿ (fd < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        /* Temporarily ignore SIGHUP, so that we don't get SIGHUP'ed
         * by our own TIOCNOTTY */
        ಸಂರಚನಾ sigaction sa_old;
        assert_se(sigaction(SIGHUP, &sigaction_ignore, &sa_old) >= 0);

        r = RET_NERRNO(ioctl(fd, TIOCNOTTY));

        assert_se(sigaction(SIGHUP, &sa_old, ರಿಕ್ತ) >= 0);

        ಪ್ರತ್ಯಾಗಮ r;
}

ಪೂರ್ಣಾಙ್ಕ terminal_new_session(ಶೂನ್ಯ) {

        /* Make us the new session leader, and set stdin tty to be our controlling terminal.
         *
         * Why stdin? Well, the ctty logic is relevant for signal delivery mostly, i.e. if people hit C-c
         * or the line is hung up. Such events are basically just a form of input, via a side channel
         * (that side channel being signal delivery, i.e. SIGINT, SIGHUP et al). Hence we focus on input,
         * not output here. */

        ಯದಿ (!isatty_safe(STDIN_FILENO))
                ಪ್ರತ್ಯಾಗಮ -ENXIO;

        (ಶೂನ್ಯ) setsid();
        ಪ್ರತ್ಯಾಗಮ RET_NERRNO(ioctl(STDIN_FILENO, TIOCSCTTY, 0));
}

ಶೂನ್ಯ terminal_detach_session(ಶೂನ್ಯ) {
        (ಶೂನ್ಯ) setsid();
        (ಶೂನ್ಯ) release_terminal();
}

ಪೂರ್ಣಾಙ್ಕ terminal_vhangup_fd(ಪೂರ್ಣಾಙ್ಕ fd) {
        ಪ್ರತಿಜ್ಞಾ(fd >= 0);
        ಪ್ರತ್ಯಾಗಮ RET_NERRNO(ioctl(fd, TIOCVHANGUP));
}

ಪೂರ್ಣಾಙ್ಕ terminal_vhangup(ಧ್ರುವ ವರ್ಣ *tty) {
        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ fd = -EBADF;

        ಪ್ರತಿಜ್ಞಾ(tty);

        fd = open_terminal(tty, O_RDWR|O_NOCTTY|O_CLOEXEC);
        ಯದಿ (fd < 0)
                ಪ್ರತ್ಯಾಗಮ fd;

        ಪ್ರತ್ಯಾಗಮ terminal_vhangup_fd(fd);
}

ಪೂರ್ಣಾಙ್ಕ vt_disallocate(ಧ್ರುವ ವರ್ಣ *tty_path) {
        ಪ್ರತಿಜ್ಞಾ(tty_path);

        /* Deallocate the VT if possible. If not possible (i.e. because it is the active one), at least clear
         * it entirely (including the scrollback buffer). */

        ಪೂರ್ಣಾಙ್ಕ ttynr = vtnr_from_tty(tty_path);
        ಯದಿ (ttynr > 0) {
                _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ fd = open_terminal("/dev/tty0", O_RDWR|O_NOCTTY|O_CLOEXEC|O_NONBLOCK);
                ಯದಿ (fd < 0)
                        ಪ್ರತ್ಯಾಗಮ fd;

                /* Try to deallocate */
                ಯದಿ (ioctl(fd, VT_DISALLOCATE, ttynr) >= 0)
                        ಪ್ರತ್ಯಾಗಮ 0;
                ಯದಿ (errno != EBUSY)
                        ಪ್ರತ್ಯಾಗಮ -errno;
        }

        /* So this is not a VT (in which case we cannot deallocate it), or we failed to deallocate. Let's at
         * least clear the screen. */

        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ fd2 = open_terminal(tty_path, O_WRONLY|O_NOCTTY|O_CLOEXEC|O_NONBLOCK);
        ಯದಿ (fd2 < 0)
                ಪ್ರತ್ಯಾಗಮ fd2;

        ಪ್ರತ್ಯಾಗಮ loop_write_full(fd2,
                               "\033[r"   /* clear scrolling region */
                               "\033[H"   /* move home */
                               "\033[3J"  /* clear screen including scrollback, requires Linux 2.6.40 */
                               "\033c",   /* reset to initial state */
                               SIZE_MAX,
                               CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ vt_default_utf8(ಶೂನ್ಯ) {
        /* Read the default VT UTF8 setting from the kernel */
        ಪ್ರತ್ಯಾಗಮ read_boolean_file("/sys/module/vt/parameters/default_utf8");
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ vt_reset_keyboard(ಪೂರ್ಣಾಙ್ಕ fd) {
        ಪೂರ್ಣಾಙ್ಕ r, kb;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        /* If we can't read the default, then default to Unicode. It's 2024 after all. */
        r = vt_default_utf8();
        ಯದಿ (r < 0)
                log_debug_errno(r, "Failed to determine kernel VT UTF-8 mode, assuming enabled: %m");

        kb = vt_default_utf8() != 0 ? K_UNICODE : K_XLATE;
        ಪ್ರತ್ಯಾಗಮ RET_NERRNO(ioctl(fd, KDSKBMODE, kb));
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ terminal_reset_ioctl(ಪೂರ್ಣಾಙ್ಕ fd, bool switch_to_text) {
        ಸಂರಚನಾ termios termios;
        ಪೂರ್ಣಾಙ್ಕ r;

        /* Set terminal to some sane defaults */

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        /* We leave locked terminal attributes untouched, so that Plymouth may set whatever it wants to set,
         * and we don't interfere with that. */

        /* Disable exclusive mode, just in case */
        ಯದಿ (ioctl(fd, TIOCNXCL) < 0)
                log_debug_errno(errno, "TIOCNXCL ioctl failed on TTY, ignoring: %m");

        /* Switch to text mode */
        ಯದಿ (switch_to_text)
                ಯದಿ (ioctl(fd, KDSETMODE, KD_TEXT) < 0)
                        log_debug_errno(errno, "KDSETMODE ioctl for switching to text mode failed on TTY, ignoring: %m");

        /* Set default keyboard mode */
        r = vt_reset_keyboard(fd);
        ಯದಿ (r < 0)
                log_debug_errno(r, "Failed to reset VT keyboard, ignoring: %m");

        ಯದಿ (tcgetattr(fd, &termios) < 0) {
                r = log_debug_errno(errno, "Failed to get terminal parameters: %m");
                ಗಚ್ಛ finish;
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
        ಯದಿ (r < 0)
                log_debug_errno(r, "Failed to set terminal parameters: %m");

finish:
        /* Just in case, flush all crap out */
        (ಶೂನ್ಯ) tcflush(fd, TCIOFLUSH);

        ಪ್ರತ್ಯಾಗಮ r;
}

ಪೂರ್ಣಾಙ್ಕ terminal_reset_ansi_seq(ಪೂರ್ಣಾಙ್ಕ fd) {
        _cleanup_(nonblock_resetp) ಪೂರ್ಣಾಙ್ಕ nonblock_reset = -EBADF;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        ಯದಿ (getenv_terminal_is_dumb())
                ಪ್ರತ್ಯಾಗಮ 0;

        r = fd_nonblock(fd, true);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ log_debug_errno(r, "Failed to set terminal to non-blocking mode: %m");
        ಯದಿ (r > 0)
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
        ಯದಿ (r < 0)
                log_debug_errno(r, "Failed to reset terminal through ANSI sequences: %m");

        ಪ್ರತ್ಯಾಗಮ r;
}

ಶೂನ್ಯ reset_dev_console_fd(ಪೂರ್ಣಾಙ್ಕ fd, bool switch_to_text) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ lock_fd = lock_dev_console();
        ಯದಿ (lock_fd < 0)
                log_debug_errno(lock_fd, "Failed to lock /dev/console, ignoring: %m");

        r = terminal_reset_ioctl(fd, switch_to_text);
        ಯದಿ (r < 0)
                log_warning_errno(r, "Failed to reset /dev/console, ignoring: %m");

        unsigned rows, cols;
        r = proc_cmdline_tty_size("/dev/console", &rows, &cols);
        ಯದಿ (r < 0)
                log_warning_errno(r, "Failed to get /dev/console size, ignoring: %m");
        ಅನ್ಯಥಾ ಯದಿ (r > 0) {
                r = terminal_set_size_fd(fd, ರಿಕ್ತ, rows, cols);
                ಯದಿ (r < 0)
                        log_warning_errno(r, "Failed to set configured terminal size on /dev/console, ignoring: %m");
        } ಅನ್ಯಥಾ
                (ಶೂನ್ಯ) terminal_fix_size(fd, fd);

        r = terminal_reset_ansi_seq(fd);
        ಯದಿ (r < 0)
                log_warning_errno(r, "Failed to reset /dev/console using ANSI sequences, ignoring: %m");
}

ಪೂರ್ಣಾಙ್ಕ lock_dev_console(ಶೂನ್ಯ) {
        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ fd = -EBADF;
        ಪೂರ್ಣಾಙ್ಕ r;

        /* NB: We do not use O_NOFOLLOW here, because some container managers might place a symlink to some
         * pty in /dev/console, in which case it should be fine to lock the target TTY. */
        fd = open_terminal("/dev/console", O_RDONLY|O_CLOEXEC|O_NOCTTY);
        ಯದಿ (fd < 0)
                ಪ್ರತ್ಯಾಗಮ fd;

        r = lock_generic(fd, LOCK_BSD, LOCK_EX);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ TAKE_FD(fd);
}

ಪೂರ್ಣಾಙ್ಕ make_console_stdio(bool switch_to_text) {
        ಪೂರ್ಣಾಙ್ಕ fd, r;

        /* Make /dev/console the controlling terminal and stdin/stdout/stderr, if we can. If we can't use
         * /dev/null instead. This is particularly useful if /dev/console is turned off, e.g. if console=null
         * is specified on the kernel command line. */

        fd = acquire_terminal("/dev/console", ACQUIRE_TERMINAL_FORCE|ACQUIRE_TERMINAL_PERMISSIVE, USEC_INFINITY);
        ಯದಿ (fd < 0) {
                log_warning_errno(fd, "Failed to acquire terminal, using /dev/null stdin/stdout/stderr instead: %m");

                r = make_null_stdio();
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ log_error_errno(r, "Failed to make /dev/null stdin/stdout/stderr: %m");

        } ಅನ್ಯಥಾ {
                reset_dev_console_fd(fd, switch_to_text);

                r = rearrange_stdio(fd, fd, fd); /* This invalidates 'fd' both on success and on failure. */
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ log_error_errno(r, "Failed to make terminal stdin/stdout/stderr: %m");
        }

        reset_terminal_feature_caches();
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ vtnr_from_tty_raw(ಧ್ರುವ ವರ್ಣ *tty, unsigned *ret) {
        ಪ್ರತಿಜ್ಞಾ(tty);

        tty = skip_dev_prefix(tty);

        ಧ್ರುವ ವರ್ಣ *e = startswith(tty, "tty");
        ಯದಿ (!e)
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        ಪ್ರತ್ಯಾಗಮ safe_atou(e, ret);
}

ಪೂರ್ಣಾಙ್ಕ vtnr_from_tty(ಧ್ರುವ ವರ್ಣ *tty) {
        unsigned u;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(tty);

        r = vtnr_from_tty_raw(tty, &u);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;
        ಯದಿ (!vtnr_is_valid(u))
                ಪ್ರತ್ಯಾಗಮ -ERANGE;

        ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ) u;
}

bool tty_is_vc(ಧ್ರುವ ವರ್ಣ *tty) {
        ಪ್ರತಿಜ್ಞಾ(tty);

        /* NB: for >= 0 values no range check is conducted here, on the assumption that the caller will
         * either extract vtnr through vtnr_from_tty() later where ERANGE would be reported, or doesn't care
         * about whether it's strictly valid, but only asking "does this fall into the vt category?", for which
         * "yes" seems to be a better answer. */

        ಪ್ರತ್ಯಾಗಮ vtnr_from_tty_raw(tty, /* ret= */ ರಿಕ್ತ) >= 0;
}

bool tty_is_console(ಧ್ರುವ ವರ್ಣ *tty) {
        ಪ್ರತಿಜ್ಞಾ(tty);

        ಪ್ರತ್ಯಾಗಮ streq(skip_dev_prefix(tty), "console");
}

ಪೂರ್ಣಾಙ್ಕ resolve_dev_console(ವರ್ಣ **ret) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        /* Resolve where /dev/console is pointing to. If /dev/console is a symlink (like in container
         * managers), we'll just resolve the symlink. If it's a real device node, we'll use if
         * /sys/class/tty/tty0/active, but only if /sys/ is actually ours (i.e. not read-only-mounted which
         * is a sign for container setups). */

        _cleanup_free_ ವರ್ಣ *chased = ರಿಕ್ತ;
        r = chase("/dev/console", /* root= */ ರಿಕ್ತ, /* flags= */ 0, &chased, /* ret_fd= */ ರಿಕ್ತ);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;
        ಯದಿ (!path_equal(chased, "/dev/console")) {
                *ret = TAKE_PTR(chased);
                ಪ್ರತ್ಯಾಗಮ 0;
        }

        r = path_is_read_only_fs("/sys");
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;
        ಯದಿ (r > 0)
                ಪ್ರತ್ಯಾಗಮ -ENOMEDIUM;

        _cleanup_free_ ವರ್ಣ *active = ರಿಕ್ತ;
        r = read_one_ಸಾಲು_file("/sys/class/tty/console/active", &active);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;
        ಯದಿ (r == 0)
                ಪ್ರತ್ಯಾಗಮ -ENXIO;

        /* If multiple log outputs are configured the last one is what /dev/console points to */
        ಧ್ರುವ ವರ್ಣ *tty = strrchr(active, ' ');
        ಯದಿ (tty)
                tty++;
        ಅನ್ಯಥಾ
                tty = active;

        ಯದಿ (streq(tty, "tty0")) {
                active = mfree(active);

                /* Get the active VC (e.g. tty1) */
                r = read_one_ಸಾಲು_file("/sys/class/tty/tty0/active", &active);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;
                ಯದಿ (r == 0)
                        ಪ್ರತ್ಯಾಗಮ -ENXIO;

                tty = active;
        }

        _cleanup_free_ ವರ್ಣ *path = ರಿಕ್ತ;
        path = path_join("/dev", tty);
        ಯದಿ (!path)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        *ret = TAKE_PTR(path);
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ get_kernel_consoles(ವರ್ಣ ***ret) {
        _cleanup_strv_free_ ವರ್ಣ **l = ರಿಕ್ತ;
        _cleanup_free_ ವರ್ಣ *ಸಾಲು = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        /* If /sys/ is mounted read-only this means we are running in some kind of container environment.
         * In that case /sys/ would reflect the host system, not us, hence ignore the data we can read from it. */
        ಯದಿ (path_is_read_only_fs("/sys") > 0)
                ಗಚ್ಛ fallback;

        r = read_one_ಸಾಲು_file("/sys/class/tty/console/active", &ಸಾಲು);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತಿ (ಧ್ರುವ ವರ್ಣ *p = ಸಾಲು;;) {
                _cleanup_free_ ವರ್ಣ *tty = ರಿಕ್ತ, *path = ರಿಕ್ತ;

                r = extract_first_word(&p, &tty, ರಿಕ್ತ, 0);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;
                ಯದಿ (r == 0)
                        ಭಙ್ಗ;

                ಯದಿ (streq(tty, "tty0")) {
                        tty = mfree(tty);
                        r = read_one_ಸಾಲು_file("/sys/class/tty/tty0/active", &tty);
                        ಯದಿ (r < 0)
                                ಪ್ರತ್ಯಾಗಮ r;
                        ಯದಿ (r == 0) {
                                log_debug("No VT active, skipping /dev/tty0.");
                                ಅನುವರ್ತ;
                        }
                }

                path = path_join("/dev", tty);
                ಯದಿ (!path)
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                ಯದಿ (access(path, F_OK) < 0) {
                        log_debug_errno(errno, "Console device %s is not accessible, skipping: %m", path);
                        ಅನುವರ್ತ;
                }

                r = strv_consume(&l, TAKE_PTR(path));
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;
        }

        ಯದಿ (strv_isempty(l)) {
                log_debug("No devices found for system console");
                ಗಚ್ಛ fallback;
        }

        *ret = TAKE_PTR(l);
        ಪ್ರತ್ಯಾಗಮ strv_length(*ret);

fallback:
        r = strv_extend(&l, "/dev/console");
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        *ret = TAKE_PTR(l);
        ಪ್ರತ್ಯಾಗಮ 0;
}

bool tty_is_vc_resolve(ಧ್ರುವ ವರ್ಣ *tty) {
        _cleanup_free_ ವರ್ಣ *resolved = ರಿಕ್ತ;

        ಪ್ರತಿಜ್ಞಾ(tty);

        ಯದಿ (streq(skip_dev_prefix(tty), "console")) {
                ಯದಿ (resolve_dev_console(&resolved) < 0)
                        ಪ್ರತ್ಯಾಗಮ false;

                tty = resolved;
        }

        ಪ್ರತ್ಯಾಗಮ tty_is_vc(tty);
}

ಪೂರ್ಣಾಙ್ಕ fd_columns(ಪೂರ್ಣಾಙ್ಕ fd) {
        ಸಂರಚನಾ winsize ws = {};

        ಯದಿ (fd < 0)
                ಪ್ರತ್ಯಾಗಮ -EBADF;

        ಯದಿ (ioctl(fd, TIOCGWINSZ, &ws) < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        ಯದಿ (ws.ws_col <= 0)
                ಪ್ರತ್ಯಾಗಮ -ENODATA; /* some tty types come up with invalid row/column initially, return a recognizable error for that */

        ಪ್ರತ್ಯಾಗಮ ws.ws_col;
}

ಪೂರ್ಣಾಙ್ಕ getenv_columns(ಶೂನ್ಯ) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಧ್ರುವ ವರ್ಣ *e = getenv("COLUMNS");
        ಯದಿ (!e)
                ಪ್ರತ್ಯಾಗಮ -ENXIO;

        unsigned c;
        r = safe_atou_bounded(e, 1, USHRT_MAX, &c);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ) c;
}

unsigned columns(ಶೂನ್ಯ) {

        ಯದಿ (cached_columns > 0)
                ಪ್ರತ್ಯಾಗಮ cached_columns;

        ಪೂರ್ಣಾಙ್ಕ c = getenv_columns();
        ಯದಿ (c < 0) {
                c = fd_columns(STDOUT_FILENO);
                ಯದಿ (c < 0)
                        c = 80;
        }

        ಪ್ರತಿಜ್ಞಾ(c > 0);

        cached_columns = c;
        ಪ್ರತ್ಯಾಗಮ cached_columns;
}

ಪೂರ್ಣಾಙ್ಕ fd_ಸಾಲುಗಳು(ಪೂರ್ಣಾಙ್ಕ fd) {
        ಸಂರಚನಾ winsize ws = {};

        ಯದಿ (fd < 0)
                ಪ್ರತ್ಯಾಗಮ -EBADF;

        ಯದಿ (ioctl(fd, TIOCGWINSZ, &ws) < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        ಯದಿ (ws.ws_row <= 0)
                ಪ್ರತ್ಯಾಗಮ -ENODATA; /* some tty types come up with invalid row/column initially, return a recognizable error for that */

        ಪ್ರತ್ಯಾಗಮ ws.ws_row;
}

unsigned ಸಾಲುಗಳು(ಶೂನ್ಯ) {
        ಧ್ರುವ ವರ್ಣ *e;
        ಪೂರ್ಣಾಙ್ಕ l;

        ಯದಿ (cached_ಸಾಲುಗಳು > 0)
                ಪ್ರತ್ಯಾಗಮ cached_ಸಾಲುಗಳು;

        l = 0;
        e = getenv("LINES");
        ಯದಿ (e)
                (ಶೂನ್ಯ) safe_atoi(e, &l);

        ಯದಿ (l <= 0 || l > USHRT_MAX) {
                l = fd_ಸಾಲುಗಳು(STDOUT_FILENO);
                ಯದಿ (l <= 0)
                        l = 24;
        }

        cached_ಸಾಲುಗಳು = l;
        ಪ್ರತ್ಯಾಗಮ cached_ಸಾಲುಗಳು;
}

ಪೂರ್ಣಾಙ್ಕ terminal_set_size_fd(ಪೂರ್ಣಾಙ್ಕ fd, ಧ್ರುವ ವರ್ಣ *ident, unsigned rows, unsigned cols) {
        ಸಂರಚನಾ winsize ws;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        ಯದಿ (!ident)
                ident = "TTY";

        ಯದಿ (rows == UINT_MAX && cols == UINT_MAX)
                ಪ್ರತ್ಯಾಗಮ 0;

        ಯದಿ (ioctl(fd, TIOCGWINSZ, &ws) < 0)
                ಪ್ರತ್ಯಾಗಮ log_debug_errno(errno,
                                       "TIOCGWINSZ ioctl for getting %s size failed, not setting terminal size: %m",
                                       ident);

        ಯದಿ (rows == UINT_MAX)
                rows = ws.ws_row;
        ಅನ್ಯಥಾ ಯದಿ (rows > USHRT_MAX)
                rows = USHRT_MAX;

        ಯದಿ (cols == UINT_MAX)
                cols = ws.ws_col;
        ಅನ್ಯಥಾ ಯದಿ (cols > USHRT_MAX)
                cols = USHRT_MAX;

        ಯದಿ (rows == ws.ws_row && cols == ws.ws_col)
                ಪ್ರತ್ಯಾಗಮ 0;

        ws.ws_row = rows;
        ws.ws_col = cols;

        ಯದಿ (ioctl(fd, TIOCSWINSZ, &ws) < 0)
                ಪ್ರತ್ಯಾಗಮ log_debug_errno(errno, "TIOCSWINSZ ioctl for setting %s size failed: %m", ident);

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ proc_cmdline_tty_size(ಧ್ರುವ ವರ್ಣ *tty, unsigned *ret_rows, unsigned *ret_cols) {
        _cleanup_free_ ವರ್ಣ *rowskey = ರಿಕ್ತ, *rowsvalue = ರಿಕ್ತ, *colskey = ರಿಕ್ತ, *colsvalue = ರಿಕ್ತ;
        unsigned rows = UINT_MAX, cols = UINT_MAX;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(tty);

        ಯದಿ (!ret_rows && !ret_cols)
                ಪ್ರತ್ಯಾಗಮ 0;

        tty = skip_dev_prefix(tty);
        ಯದಿ (path_startswith(tty, "pts/"))
                ಪ್ರತ್ಯಾಗಮ -EMEDIUMTYPE;
        ಯದಿ (!in_charset(tty, ALPHANUMERICAL))
                ಪ್ರತ್ಯಾಗಮ log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "TTY name '%s' contains non-alphanumeric characters, not searching kernel cmdline for size.", tty);

        rowskey = strjoin("systemd.tty.rows.", tty);
        ಯದಿ (!rowskey)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        colskey = strjoin("systemd.tty.columns.", tty);
        ಯದಿ (!colskey)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        r = proc_cmdline_get_ಕೀಲಿ_many(/* flags= */ 0,
                                      rowskey, &rowsvalue,
                                      colskey, &colsvalue);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ log_debug_errno(r, "Failed to read TTY size of %s from kernel cmdline: %m", tty);

        ಯದಿ (rowsvalue) {
                r = safe_atou(rowsvalue, &rows);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ log_debug_errno(r, "Failed to parse %s=%s: %m", rowskey, rowsvalue);
        }

        ಯದಿ (colsvalue) {
                r = safe_atou(colsvalue, &cols);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ log_debug_errno(r, "Failed to parse %s=%s: %m", colskey, colsvalue);
        }

        ಯದಿ (ret_rows)
                *ret_rows = rows;
        ಯದಿ (ret_cols)
                *ret_cols = cols;

        ಪ್ರತ್ಯಾಗಮ rows != UINT_MAX || cols != UINT_MAX;
}

/* intended to be used as a SIGWINCH sighandler */
ಶೂನ್ಯ columns_ಸಾಲುಗಳು_cache_reset(ಪೂರ್ಣಾಙ್ಕ signum) {
        cached_columns = 0;
        cached_ಸಾಲುಗಳು = 0;
}

ಶೂನ್ಯ reset_terminal_feature_caches(ಶೂನ್ಯ) {
        cached_columns = 0;
        cached_ಸಾಲುಗಳು = 0;

        cached_on_tty = -1;
        cached_on_dev_null = -1;

        reset_ansi_feature_caches();
}

bool on_tty(ಶೂನ್ಯ) {

        /* We check both stdout and stderr, so that situations where pipes on the shell are used are reliably
         * recognized, regardless if only the output or the errors are piped to some place. Since on_tty() is generally
         * used to default to a safer, non-interactive, non-color mode of operation it's probably good to be defensive
         * here, and check for both. Note that we don't check for STDIN_FILENO, because it should fine to use fancy
         * terminal functionality when outputting stuff, even if the input is piped to us. */

        ಯದಿ (cached_on_tty < 0)
                cached_on_tty =
                        isatty_safe(STDOUT_FILENO) &&
                        isatty_safe(STDERR_FILENO);

        ಪ್ರತ್ಯಾಗಮ cached_on_tty;
}

ಪೂರ್ಣಾಙ್ಕ getttyname_malloc(ಪೂರ್ಣಾಙ್ಕ fd, ವರ್ಣ **ret) {
        ವರ್ಣ path[PATH_MAX]; /* PATH_MAX is counted *with* the trailing NUL byte */
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);
        ಪ್ರತಿಜ್ಞಾ(ret);

        r = ttyname_r(fd, path, ಪರಿಮಾಣ path); /* positive error */
        ಪ್ರತಿಜ್ಞಾ(r >= 0);
        ಯದಿ (r == ERANGE)
                ಪ್ರತ್ಯಾಗಮ -ENAMETOOLONG;
        ಯದಿ (r > 0)
                ಪ್ರತ್ಯಾಗಮ -r;

        ಪ್ರತ್ಯಾಗಮ strdup_to(ret, skip_dev_prefix(path));
}

ಪೂರ್ಣಾಙ್ಕ getttyname_harder(ಪೂರ್ಣಾಙ್ಕ fd, ವರ್ಣ **ret) {
        _cleanup_free_ ವರ್ಣ *s = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        r = getttyname_malloc(fd, &s);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (streq(s, "tty"))
                ಪ್ರತ್ಯಾಗಮ get_ctty(0, ರಿಕ್ತ, ret);

        *ret = TAKE_PTR(s);
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ get_ctty_devnr(pid_t pid, dev_t *ret) {
        _cleanup_free_ ವರ್ಣ *ಸಾಲು = ರಿಕ್ತ;
        unsigned long ttynr;
        ಧ್ರುವ ವರ್ಣ *p;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(pid >= 0);

        p = procfs_file_alloca(pid, "stat");
        r = read_one_ಸಾಲು_file(p, &ಸಾಲು);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        p = strrchr(ಸಾಲು, ')');
        ಯದಿ (!p)
                ಪ್ರತ್ಯಾಗಮ -EIO;

        p++;

        ಯದಿ (sscanf(p, " "
                   "%*c "  /* state */
                   "%*d "  /* ppid */
                   "%*d "  /* pgrp */
                   "%*d "  /* session */
                   "%lu ", /* ttynr */
                   &ttynr) != 1)
                ಪ್ರತ್ಯಾಗಮ -EIO;

        ಯದಿ (devnum_is_zero(ttynr))
                ಪ್ರತ್ಯಾಗಮ -ENXIO;

        ಯದಿ (ret)
                *ret = (dev_t) ttynr;

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ get_ctty(pid_t pid, dev_t *ret_devnr, ವರ್ಣ **ret) {
        ವರ್ಣ pty[STRLEN("/dev/pts/") + DECIMAL_STR_MAX(dev_t) + 1];
        _cleanup_free_ ವರ್ಣ *buf = ರಿಕ್ತ;
        ಧ್ರುವ ವರ್ಣ *fn = ರಿಕ್ತ, *w;
        dev_t devnr;
        ಪೂರ್ಣಾಙ್ಕ r;

        r = get_ctty_devnr(pid, &devnr);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        r = device_path_make_canonical(S_IFCHR, devnr, &buf);
        ಯದಿ (r < 0) {
                ಸಂರಚನಾ stat st;

                ಯದಿ (r != -ENOENT) /* No symlink for this in /dev/char/? */
                        ಪ್ರತ್ಯಾಗಮ r;

                /* Maybe this is PTY? PTY devices are not listed in /dev/char/, as they don't follow the
                 * Linux device model and hence device_path_make_canonical() doesn't work for them. Let's
                 * assume this is a PTY for a moment, and check if the device node this would then map to in
                 * /dev/pts/ matches the one we are looking for. This way we don't have to hardcode the major
                 * number (which is 136 btw), but we still rely on the fact that PTY numbers map directly to
                 * the minor number of the pty. */
                xsprintf(pty, "/dev/pts/%u", minor(devnr));

                ಯದಿ (stat(pty, &st) < 0) {
                        ಯದಿ (errno != ENOENT)
                                ಪ್ರತ್ಯಾಗಮ -errno;

                } ಅನ್ಯಥಾ ಯದಿ (S_ISCHR(st.st_mode) && devnr == st.st_rdev) /* Bingo! */
                        fn = pty;

                ಯದಿ (!fn) {
                        /* Doesn't exist, or not a PTY? Probably something similar to the PTYs which have no
                         * symlink in /dev/char/. Let's return something vaguely useful. */
                        r = device_path_make_major_minor(S_IFCHR, devnr, &buf);
                        ಯದಿ (r < 0)
                                ಪ್ರತ್ಯಾಗಮ r;

                        fn = buf;
                }
        } ಅನ್ಯಥಾ
                fn = buf;

        w = path_startswith(fn, "/dev/");
        ಯದಿ (!w)
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        ಯದಿ (ret) {
                r = strdup_to(ret, w);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;
        }

        ಯದಿ (ret_devnr)
                *ret_devnr = devnr;

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ ptsname_malloc(ಪೂರ್ಣಾಙ್ಕ fd, ವರ್ಣ **ret) {
        ಪ್ರತಿಜ್ಞಾ(fd >= 0);
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಪ್ರತಿ (size_t l = 50;;) {
                _cleanup_free_ ವರ್ಣ *c = ರಿಕ್ತ;

                c = new(ವರ್ಣ, l);
                ಯದಿ (!c)
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                ಯದಿ (ptsname_r(fd, c, l) >= 0) {
                        *ret = TAKE_PTR(c);
                        ಪ್ರತ್ಯಾಗಮ 0;
                }
                ಯದಿ (errno != ERANGE)
                        ಪ್ರತ್ಯಾಗಮ -errno;

                ಯದಿ (!MUL_ASSIGN_SAFE(&l, 2))
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;
        }
}

ಪೂರ್ಣಾಙ್ಕ openpt_allocate(ಪೂರ್ಣಾಙ್ಕ flags, ವರ್ಣ **ret_peer_path) {
        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ fd = -EBADF;
        ಪೂರ್ಣಾಙ್ಕ r;

        fd = posix_openpt(flags|O_NOCTTY|O_CLOEXEC);
        ಯದಿ (fd < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        _cleanup_free_ ವರ್ಣ *p = ರಿಕ್ತ;
        ಯದಿ (ret_peer_path) {
                r = ptsname_malloc(fd, &p);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                ಯದಿ (!path_startswith(p, "/dev/pts/"))
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;
        }

        ಯದಿ (unlockpt(fd) < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        ಯದಿ (ret_peer_path)
                *ret_peer_path = TAKE_PTR(p);

        ಪ್ರತ್ಯಾಗಮ TAKE_FD(fd);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ ptsname_namespace(ಪೂರ್ಣಾಙ್ಕ pty, ವರ್ಣ **ret) {
        ಪೂರ್ಣಾಙ್ಕ no = -1;

        ಪ್ರತಿಜ್ಞಾ(pty >= 0);
        ಪ್ರತಿಜ್ಞಾ(ret);

        /* Like ptsname(), but doesn't assume that the path is
         * accessible in the local namespace. */

        ಯದಿ (ioctl(pty, TIOCGPTN, &no) < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        ಯದಿ (no < 0)
                ಪ್ರತ್ಯಾಗಮ -EIO;

        ಯದಿ (asprintf(ret, "/dev/pts/%i", no) < 0)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ openpt_allocate_in_namespace(
                ಧ್ರುವ PidRef *pidref,
                ಪೂರ್ಣಾಙ್ಕ flags,
                ವರ್ಣ **ret_peer_path) {

        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ pidnsfd = -EBADF, mntnsfd = -EBADF, usernsfd = -EBADF, rootfd = -EBADF, fd = -EBADF;
        _cleanup_close_pair_ ಪೂರ್ಣಾಙ್ಕ pair[2] = EBADF_PAIR;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(pidref);

        r = pidref_namespace_open(pidref, &pidnsfd, &mntnsfd, /* ret_netns_fd= */ ರಿಕ್ತ, &usernsfd, &rootfd);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ log_debug_errno(r, "Failed to open namespaces of PID "PID_FMT": %m", pidref->pid);

        ಯದಿ (socketpair(AF_UNIX, SOCK_DGRAM|SOCK_CLOEXEC, 0, pair) < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        r = namespace_fork(
                        "(sd-openptns)",
                        "(sd-openpt)",
                        FORK_RESET_SIGNALS|FORK_DEATHSIG_SIGKILL|FORK_WAIT,
                        pidnsfd,
                        mntnsfd,
                        /* netns_fd= */ -EBADF,
                        usernsfd,
                        rootfd,
                        /* ret= */ ರಿಕ್ತ);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;
        ಯದಿ (r == 0) {
                pair[0] = safe_close(pair[0]);

                fd = openpt_allocate(flags, /* ret_peer_path= */ ರಿಕ್ತ);
                ಯದಿ (fd < 0)
                        _exit(EXIT_FAILURE);

                ಯದಿ (send_one_fd(pair[1], fd, 0) < 0)
                        _exit(EXIT_FAILURE);

                _exit(EXIT_SUCCESS);
        }

        pair[1] = safe_close(pair[1]);

        fd = receive_one_fd(pair[0], 0);
        ಯದಿ (fd < 0)
                ಪ್ರತ್ಯಾಗಮ fd;

        ಯದಿ (ret_peer_path) {
                r = ptsname_namespace(fd, ret_peer_path);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;
        }

        ಪ್ರತ್ಯಾಗಮ TAKE_FD(fd);
}

ಸ್ಥಿರ bool on_dev_null(ಶೂನ್ಯ) {
        ಸಂರಚನಾ stat dst, ost, est;

        ಯದಿ (cached_on_dev_null >= 0)
                ಪ್ರತ್ಯಾಗಮ cached_on_dev_null;

        ಯದಿ (stat("/dev/null", &dst) < 0 || fstat(STDOUT_FILENO, &ost) < 0 || fstat(STDERR_FILENO, &est) < 0)
                cached_on_dev_null = false;
        ಅನ್ಯಥಾ
                cached_on_dev_null = stat_inode_same(&dst, &ost) && stat_inode_same(&dst, &est);

        ಪ್ರತ್ಯಾಗಮ cached_on_dev_null;
}

bool term_env_valid(ಧ್ರುವ ವರ್ಣ *term) {
        /* Checks if the specified $TERM value is suitable for propagation, i.e. is not empty, not set to
         * "unknown" (as is common in CI), and only contains characters valid in terminal type names.
         * Valid $TERM values are things like "xterm-256color", "linux", "screen.xterm-256color", i.e.
         * alphanumeric characters, hyphens, underscores, dots, and plus signs. */
        ಪ್ರತ್ಯಾಗಮ !isempty(term) &&
                !streq(term, "unknown") &&
                in_charset(term, ALPHANUMERICAL "-_+.");
}

bool getenv_terminal_is_dumb(ಶೂನ್ಯ) {
        ಧ್ರುವ ವರ್ಣ *e;

        e = getenv("TERM");
        ಯದಿ (!e)
                ಪ್ರತ್ಯಾಗಮ true;

        ಪ್ರತ್ಯಾಗಮ streq(e, "dumb");
}

bool terminal_is_dumb(ಶೂನ್ಯ) {
        ಯದಿ (!on_tty() && !on_dev_null())
                ಪ್ರತ್ಯಾಗಮ true;

        ಪ್ರತ್ಯಾಗಮ getenv_terminal_is_dumb();
}

bool dev_console_colors_enabled(ಶೂನ್ಯ) {
        _cleanup_free_ ವರ್ಣ *s = ರಿಕ್ತ;
        ColorMode m;

        /* Returns true if we assume that color is supported on /dev/console.
         *
         * For that we first check if we explicitly got told to use colors or not, by checking $SYSTEMD_COLORS. If that
         * isn't set we check whether PID 1 has $TERM set, and if not, whether TERM is set on the kernel command
         * line. If we find $TERM set we assume color if it's not set to "dumb", similarly to how regular
         * colors_enabled() operates. */

        m = parse_systemd_colors();
        ಯದಿ (m >= 0)
                ಪ್ರತ್ಯಾಗಮ m;

        ಯದಿ (getenv("NO_COLOR"))
                ಪ್ರತ್ಯಾಗಮ false;

        ಯದಿ (getenv_for_pid(1, "TERM", &s) <= 0)
                (ಶೂನ್ಯ) proc_cmdline_get_ಕೀಲಿ("TERM", 0, &s);

        ಪ್ರತ್ಯಾಗಮ !streq_ptr(s, "dumb");
}

ಪೂರ್ಣಾಙ್ಕ vt_restore(ಪೂರ್ಣಾಙ್ಕ fd) {

        ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ vt_mode mode = {
                .mode = VT_AUTO,
        };

        ಪೂರ್ಣಾಙ್ಕ r, ret = 0;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        ಯದಿ (!isatty_safe(fd))
                ಪ್ರತ್ಯಾಗಮ log_debug_errno(SYNTHETIC_ERRNO(ENOTTY), "Asked to restore the VT for an fd that does not refer to a terminal.");

        ಯದಿ (ioctl(fd, KDSETMODE, KD_TEXT) < 0)
                RET_GATHER(ret, log_debug_errno(errno, "Failed to set VT to text mode, ignoring: %m"));

        r = vt_reset_keyboard(fd);
        ಯದಿ (r < 0)
                RET_GATHER(ret, log_debug_errno(r, "Failed to reset keyboard mode, ignoring: %m"));

        ಯದಿ (ioctl(fd, VT_SETMODE, &mode) < 0)
                RET_GATHER(ret, log_debug_errno(errno, "Failed to set VT_AUTO mode, ignoring: %m"));

        r = fchmod_and_chown(fd, TTY_MODE, 0, GID_INVALID);
        ಯದಿ (r < 0)
                RET_GATHER(ret, log_debug_errno(r, "Failed to chmod()/chown() VT, ignoring: %m"));

        ಪ್ರತ್ಯಾಗಮ ret;
}

ಪೂರ್ಣಾಙ್ಕ vt_release(ಪೂರ್ಣಾಙ್ಕ fd, bool restore) {
        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        /* This function releases the VT by acknowledging the VT-switch signal
         * sent by the kernel and optionally reset the VT in text and auto
         * VT-switching modes. */

        ಯದಿ (!isatty_safe(fd))
                ಪ್ರತ್ಯಾಗಮ log_debug_errno(SYNTHETIC_ERRNO(ENOTTY), "Asked to release the VT for an fd that does not refer to a terminal.");

        ಯದಿ (ioctl(fd, VT_RELDISP, 1) < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        ಯದಿ (restore)
                ಪ್ರತ್ಯಾಗಮ vt_restore(fd);

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಶೂನ್ಯ get_log_colors(ಪೂರ್ಣಾಙ್ಕ priority, ಧ್ರುವ ವರ್ಣ **on, ಧ್ರುವ ವರ್ಣ **off, ಧ್ರುವ ವರ್ಣ **highlight) {
        /* Note that this will initialize output variables only when there's something to output.
         * The caller must pre-initialize to "" or NULL as appropriate. */

        ಯದಿ (priority <= LOG_ERR) {
                ಯದಿ (on)
                        *on = ansi_highlight_red();
                ಯದಿ (off)
                        *off = ansi_normal();
                ಯದಿ (highlight)
                        *highlight = ansi_highlight();

        } ಅನ್ಯಥಾ ಯದಿ (priority <= LOG_WARNING) {
                ಯದಿ (on)
                        *on = ansi_highlight_yellow();
                ಯದಿ (off)
                        *off = ansi_normal();
                ಯದಿ (highlight)
                        *highlight = ansi_highlight();

        } ಅನ್ಯಥಾ ಯದಿ (priority <= LOG_NOTICE) {
                ಯದಿ (on)
                        *on = ansi_highlight();
                ಯದಿ (off)
                        *off = ansi_normal();
                ಯದಿ (highlight)
                        *highlight = ansi_highlight_red();

        } ಅನ್ಯಥಾ ಯದಿ (priority >= LOG_DEBUG) {
                ಯದಿ (on)
                        *on = ansi_grey();
                ಯದಿ (off)
                        *off = ansi_normal();
                ಯದಿ (highlight)
                        *highlight = ansi_highlight_red();
        }
}

ಪೂರ್ಣಾಙ್ಕ terminal_set_cursor_position(ಪೂರ್ಣಾಙ್ಕ fd, unsigned row, unsigned column) {
        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        ವರ್ಣ cursor_position[STRLEN("\x1B[" ";" "H") + DECIMAL_STR_MAX(unsigned) * 2 + 1];
        xsprintf(cursor_position, "\x1B[%u;%uH", row, column);

        ಪ್ರತ್ಯಾಗಮ loop_write(fd, cursor_position, SIZE_MAX);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ terminal_verify_same(ಪೂರ್ಣಾಙ್ಕ ಒಳಹರಿವು_fd, ಪೂರ್ಣಾಙ್ಕ output_fd) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(ಒಳಹರಿವು_fd >= 0);
        ಪ್ರತಿಜ್ಞಾ(output_fd >= 0);

        /* Validates that the specified fds reference the same TTY */

        ಯದಿ (ಒಳಹರಿವು_fd != output_fd) {
                ಸಂರಚನಾ stat sti;
                ಯದಿ (fstat(ಒಳಹರಿವು_fd, &sti) < 0)
                        ಪ್ರತ್ಯಾಗಮ -errno;

                r = stat_verify_char(&sti); /* TTYs are character devices */
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                ಸಂರಚನಾ stat sto;
                ಯದಿ (fstat(output_fd, &sto) < 0)
                        ಪ್ರತ್ಯಾಗಮ -errno;

                r = stat_verify_char(&sto);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                ಯದಿ (sti.st_rdev != sto.st_rdev)
                        ಪ್ರತ್ಯಾಗಮ -ENOLINK;
        }

        ಯದಿ (!isatty_safe(ಒಳಹರಿವು_fd)) /* The check above was just for char device, but now let's ensure it's actually a tty */
                ಪ್ರತ್ಯಾಗಮ -ENOTTY;

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ CursorPositionState {
        CURSOR_TEXT,
        CURSOR_ESCAPE,
        CURSOR_ROW,
        CURSOR_COLUMN,
} CursorPositionState;

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ CursorPositionContext {
        CursorPositionState state;
        unsigned row, column;
} CursorPositionContext;

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ scan_cursor_position_response(
                CursorPositionContext *context,
                ಧ್ರುವ ವರ್ಣ *buf,
                size_t size,
                size_t *ret_processed) {

        ಪ್ರತಿಜ್ಞಾ(context);
        ಪ್ರತಿಜ್ಞಾ(buf);
        ಪ್ರತಿಜ್ಞಾ(ret_processed);

        ಪ್ರತಿ (size_t i = 0; i < size; i++) {
                ವರ್ಣ c = buf[i];

                ವಿಕಲ್ಪ (context->state) {

                ಪಕ್ಷ CURSOR_TEXT:
                        context->state = c == '\x1B' ? CURSOR_ESCAPE : CURSOR_TEXT;
                        ಭಙ್ಗ;

                ಪಕ್ಷ CURSOR_ESCAPE:
                        context->state = c == '[' ? CURSOR_ROW : CURSOR_TEXT;
                        ಭಙ್ಗ;

                ಪಕ್ಷ CURSOR_ROW:
                        ಯದಿ (c == ';')
                                context->state = context->row > 0 ? CURSOR_COLUMN : CURSOR_TEXT;
                        ಅನ್ಯಥಾ {
                                ಪೂರ್ಣಾಙ್ಕ d = undecchar(c);

                                /* We read a decimal character, let's suffix it to the number we so far read,
                                 * but let's do an overflow check first. */
                                ಯದಿ (d < 0 || context->row > (UINT_MAX-d)/10)
                                        context->state = CURSOR_TEXT;
                                ಅನ್ಯಥಾ
                                        context->row = context->row * 10 + d;
                        }
                        ಭಙ್ಗ;

                ಪಕ್ಷ CURSOR_COLUMN:
                        ಯದಿ (c == 'R') {
                                ಯದಿ (context->column > 0) {
                                        *ret_processed = i + 1;
                                        ಪ್ರತ್ಯಾಗಮ 1; /* success! */
                                }

                                context->state = CURSOR_TEXT;
                        } ಅನ್ಯಥಾ {
                                ಪೂರ್ಣಾಙ್ಕ d = undecchar(c);

                                /* As above, add the decimal character to our column number */
                                ಯದಿ (d < 0 || context->column > (UINT_MAX-d)/10)
                                        context->state = CURSOR_TEXT;
                                ಅನ್ಯಥಾ
                                        context->column = context->column * 10 + d;
                        }

                        ಭಙ್ಗ;
                }

                /* Reset any positions we might have picked up */
                ಯದಿ (IN_SET(context->state, CURSOR_TEXT, CURSOR_ESCAPE))
                        context->row = context->column = 0;
        }

        *ret_processed = size;
        ಪ್ರತ್ಯಾಗಮ 0; /* all good, but not enough data yet */
}

ಪೂರ್ಣಾಙ್ಕ terminal_get_cursor_position(
                ಪೂರ್ಣಾಙ್ಕ ಒಳಹರಿವು_fd,
                ಪೂರ್ಣಾಙ್ಕ output_fd,
                unsigned *ret_row,
                unsigned *ret_column) {

        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ nonblock_ಒಳಹರಿವು_fd = -EBADF;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(ಒಳಹರಿವು_fd >= 0);
        ಪ್ರತಿಜ್ಞಾ(output_fd >= 0);

        ಯದಿ (getenv_terminal_is_dumb())
                ಪ್ರತ್ಯಾಗಮ -EOPNOTSUPP;

        r = terminal_verify_same(ಒಳಹರಿವು_fd, output_fd);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ log_debug_errno(r, "Called with distinct input/output fds: %m");

        /* Failure to reset the terminal is ignored here and in similar cases below.
         * We already have our result; if cleanup fails it doesn't change the validity of the result. */
        ಸಂರಚನಾ termios old_termios = TERMIOS_NULL;
        CLEANUP_TERMIOS_RESET(ಒಳಹರಿವು_fd, old_termios);

        ಯದಿ (tcgetattr(ಒಳಹರಿವು_fd, &old_termios) < 0)
                ಪ್ರತ್ಯಾಗಮ log_debug_errno(errno, "Failed to get terminal settings: %m");

        ಸಂರಚನಾ termios new_termios = old_termios;
        termios_disable_echo(&new_termios);

        ಯದಿ (tcsetattr(ಒಳಹರಿವು_fd, TCSANOW, &new_termios) < 0)
                ಪ್ರತ್ಯಾಗಮ log_debug_errno(errno, "Failed to set new terminal settings: %m");

        /* Request cursor position (DSR/CPR) */
        r = loop_write(output_fd, "\x1B[6n", SIZE_MAX);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        /* Open a 2nd input fd, in non-blocking mode, so that we won't ever hang in read() should someone
         * else process the POLLIN. */

        nonblock_ಒಳಹರಿವು_fd = r = fd_reopen(ಒಳಹರಿವು_fd, O_RDONLY|O_CLOEXEC|O_NONBLOCK|O_NOCTTY);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        usec_t end = usec_add(now(CLOCK_MONOTONIC), CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        ವರ್ಣ buf[STRLEN("\x1B[1;1R")]; /* The shortest valid reply possible */
        size_t buf_full = 0;
        CursorPositionContext context = {};

        ಪ್ರತಿ (bool first = true;; first = false) {
                ಯದಿ (buf_full == 0) {
                        usec_t n = now(CLOCK_MONOTONIC);
                        ಯದಿ (n >= end)
                                ಪ್ರತ್ಯಾಗಮ -EOPNOTSUPP;

                        r = fd_wait_for_event(nonblock_ಒಳಹರಿವು_fd, POLLIN, usec_sub_unsigned(end, n));
                        ಯದಿ (r < 0)
                                ಪ್ರತ್ಯಾಗಮ r;
                        ಯದಿ (r == 0)
                                ಪ್ರತ್ಯಾಗಮ -EOPNOTSUPP;

                        /* On the first try, read multiple characters, i.e. the shortest valid
                         * reply. Afterwards read byte-wise, since we don't want to read too much, and
                         * unnecessarily drop too many characters from the input queue. */
                        ssize_t l = ಪಠ(nonblock_ಒಳಹರಿವು_fd, buf, first ? ಪರಿಮಾಣ(buf) : 1);
                        ಯದಿ (l < 0) {
                                ಯದಿ (errno == EAGAIN)
                                        ಅನುವರ್ತ;

                                ಪ್ರತ್ಯಾಗಮ -errno;
                        }

                        ಪ್ರತಿಜ್ಞಾ((size_t) l <= ಪರಿಮಾಣ(buf));
                        buf_full = l;
                }

                size_t processed;
                r = scan_cursor_position_response(&context, buf, buf_full, &processed);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                ಪ್ರತಿಜ್ಞಾ(processed <= buf_full);
                buf_full -= processed;
                memmove(buf, buf + processed, buf_full);

                ಯದಿ (r > 0) {
                        /* Superficial validity check */
                        ಯದಿ (context.row >= 32766 || context.column >= 32766)
                                ಪ್ರತ್ಯಾಗಮ -ENODATA;

                        ಯದಿ (ret_row)
                                *ret_row = context.row;
                        ಯದಿ (ret_column)
                                *ret_column = context.column;

                        ಪ್ರತ್ಯಾಗಮ 0;
                }
        }
}

ಪೂರ್ಣಾಙ್ಕ terminal_reset_defensive(ಪೂರ್ಣಾಙ್ಕ fd, TerminalResetFlags flags) {
        ಪೂರ್ಣಾಙ್ಕ r = 0;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);
        ಪ್ರತಿಜ್ಞಾ(!FLAGS_SET(flags, TERMINAL_RESET_AVOID_ANSI_SEQ|TERMINAL_RESET_FORCE_ANSI_SEQ));

        /* Resets the terminal comprehensively, i.e. via both ioctl()s and via ANSI sequences, but do so only
         * if $TERM is unset or set to "dumb" */

        ಯದಿ (!isatty_safe(fd))
                ಪ್ರತ್ಯಾಗಮ -ENOTTY;

        RET_GATHER(r, terminal_reset_ioctl(fd, FLAGS_SET(flags, TERMINAL_RESET_SWITCH_TO_TEXT)));

        ಯದಿ (!FLAGS_SET(flags, TERMINAL_RESET_AVOID_ANSI_SEQ) &&
            (FLAGS_SET(flags, TERMINAL_RESET_FORCE_ANSI_SEQ) || !getenv_terminal_is_dumb()))
                RET_GATHER(r, terminal_reset_ansi_seq(fd));

        ಪ್ರತ್ಯಾಗಮ r;
}

ಪೂರ್ಣಾಙ್ಕ terminal_reset_defensive_locked(ಪೂರ್ಣಾಙ್ಕ fd, TerminalResetFlags flags) {
        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ lock_fd = lock_dev_console();
        ಯದಿ (lock_fd < 0)
                log_debug_errno(lock_fd, "Failed to acquire lock for /dev/console, ignoring: %m");

        ಪ್ರತ್ಯಾಗಮ terminal_reset_defensive(fd, flags);
}

ಶೂನ್ಯ termios_disable_echo(ಸಂರಚನಾ termios *termios) {
        ಪ್ರತಿಜ್ಞಾ(termios);

        termios->c_lflag &= ~(ICANON|ECHO);
        termios->c_cc[VMIN] = 1;
        termios->c_cc[VTIME] = 0;
}

ಸ್ಥಿರ bool termios_is_null(ಧ್ರುವ ಸಂರಚನಾ termios *t) {
        ಯದಿ (!t)
                ಪ್ರತ್ಯಾಗಮ true;

        ಪ್ರತ್ಯಾಗಮ t->c_iflag == UINT_MAX &&
               t->c_oflag == UINT_MAX &&
               t->c_cflag == UINT_MAX &&
               t->c_lflag == UINT_MAX;
}

ಶೂನ್ಯ termios_reset(ಧ್ರುವ TermiosResetContext *c) {
        ಪ್ರತಿಜ್ಞಾ(c);

        PROTECT_ERRNO;

        ಯದಿ (c->fd && *c->fd >= 0 && !termios_is_null(c->termios))
                (ಶೂನ್ಯ) tcsetattr(*c->fd, TCSANOW, c->termios);
}

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ BackgroundColorState {
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

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ BackgroundColorContext {
        BackgroundColorState state;
        uint32_t red, green, blue;
        unsigned red_bits, green_bits, blue_bits;
} BackgroundColorContext;

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ scan_background_color_response(
                BackgroundColorContext *context,
                ಧ್ರುವ ವರ್ಣ *buf,
                size_t size,
                size_t *ret_processed) {

        ಪ್ರತಿಜ್ಞಾ(context);
        ಪ್ರತಿಜ್ಞಾ(buf);
        ಪ್ರತಿಜ್ಞಾ(ret_processed);

        ಪ್ರತಿ (size_t i = 0; i < size; i++) {
                ವರ್ಣ c = buf[i];

                ವಿಕಲ್ಪ (context->state) {

                ಪಕ್ಷ BACKGROUND_TEXT:
                        context->state = c == '\x1B' ? BACKGROUND_ESCAPE : BACKGROUND_TEXT;
                        ಭಙ್ಗ;

                ಪಕ್ಷ BACKGROUND_ESCAPE:
                        context->state = c == ']' ? BACKGROUND_BRACKET : BACKGROUND_TEXT;
                        ಭಙ್ಗ;

                ಪಕ್ಷ BACKGROUND_BRACKET:
                        context->state = c == '1' ? BACKGROUND_FIRST_ONE : BACKGROUND_TEXT;
                        ಭಙ್ಗ;

                ಪಕ್ಷ BACKGROUND_FIRST_ONE:
                        context->state = c == '1' ? BACKGROUND_SECOND_ONE : BACKGROUND_TEXT;
                        ಭಙ್ಗ;

                ಪಕ್ಷ BACKGROUND_SECOND_ONE:
                        context->state = c == ';' ? BACKGROUND_SEMICOLON : BACKGROUND_TEXT;
                        ಭಙ್ಗ;

                ಪಕ್ಷ BACKGROUND_SEMICOLON:
                        context->state = c == 'r' ? BACKGROUND_R : BACKGROUND_TEXT;
                        ಭಙ್ಗ;

                ಪಕ್ಷ BACKGROUND_R:
                        context->state = c == 'g' ? BACKGROUND_G : BACKGROUND_TEXT;
                        ಭಙ್ಗ;

                ಪಕ್ಷ BACKGROUND_G:
                        context->state = c == 'b' ? BACKGROUND_B : BACKGROUND_TEXT;
                        ಭಙ್ಗ;

                ಪಕ್ಷ BACKGROUND_B:
                        context->state = c == ':' ? BACKGROUND_RED : BACKGROUND_TEXT;
                        ಭಙ್ಗ;

                ಪಕ್ಷ BACKGROUND_RED:
                        ಯದಿ (c == '/')
                                context->state = context->red_bits > 0 ? BACKGROUND_GREEN : BACKGROUND_TEXT;
                        ಅನ್ಯಥಾ {
                                ಪೂರ್ಣಾಙ್ಕ d = unhexchar(c);
                                ಯದಿ (d < 0 || context->red_bits >= ಪರಿಮಾಣ(context->red)*8)
                                        context->state = BACKGROUND_TEXT;
                                ಅನ್ಯಥಾ {
                                        context->red = (context->red << 4) | d;
                                        context->red_bits += 4;
                                }
                        }
                        ಭಙ್ಗ;

                ಪಕ್ಷ BACKGROUND_GREEN:
                        ಯದಿ (c == '/')
                                context->state = context->green_bits > 0 ? BACKGROUND_BLUE : BACKGROUND_TEXT;
                        ಅನ್ಯಥಾ {
                                ಪೂರ್ಣಾಙ್ಕ d = unhexchar(c);
                                ಯದಿ (d < 0 || context->green_bits >= ಪರಿಮಾಣ(context->green)*8)
                                        context->state = BACKGROUND_TEXT;
                                ಅನ್ಯಥಾ {
                                        context->green = (context->green << 4) | d;
                                        context->green_bits += 4;
                                }
                        }
                        ಭಙ್ಗ;

                ಪಕ್ಷ BACKGROUND_BLUE:
                        ಯದಿ (c == '\x07') {
                                ಯದಿ (context->blue_bits > 0) {
                                        *ret_processed = i + 1;
                                        ಪ್ರತ್ಯಾಗಮ 1; /* success! */
                                }

                                context->state = BACKGROUND_TEXT;
                        } ಅನ್ಯಥಾ ಯದಿ (c == '\x1b')
                                context->state = context->blue_bits > 0 ? BACKGROUND_STRING_TERMINATOR : BACKGROUND_TEXT;
                        ಅನ್ಯಥಾ {
                                ಪೂರ್ಣಾಙ್ಕ d = unhexchar(c);
                                ಯದಿ (d < 0 || context->blue_bits >= ಪರಿಮಾಣ(context->blue)*8)
                                        context->state = BACKGROUND_TEXT;
                                ಅನ್ಯಥಾ {
                                        context->blue = (context->blue << 4) | d;
                                        context->blue_bits += 4;
                                }
                        }
                        ಭಙ್ಗ;

                ಪಕ್ಷ BACKGROUND_STRING_TERMINATOR:
                        ಯದಿ (c == '\\') {
                                *ret_processed = i + 1;
                                ಪ್ರತ್ಯಾಗಮ 1; /* success! */
                        }

                        context->state = c == ']' ? BACKGROUND_ESCAPE : BACKGROUND_TEXT;
                        ಭಙ್ಗ;

                }

                /* Reset any colors we might have picked up */
                ಯದಿ (IN_SET(context->state, BACKGROUND_TEXT, BACKGROUND_ESCAPE)) {
                        /* reset color */
                        context->red = context->green = context->blue = 0;
                        context->red_bits = context->green_bits = context->blue_bits = 0;
                }
        }

        *ret_processed = size;
        ಪ್ರತ್ಯಾಗಮ 0; /* all good, but not enough data yet */
}

ಪೂರ್ಣಾಙ್ಕ get_default_background_color(ದ್ವಿಗುಣ *ret_red, ದ್ವಿಗುಣ *ret_green, ದ್ವಿಗುಣ *ret_blue) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret_red);
        ಪ್ರತಿಜ್ಞಾ(ret_green);
        ಪ್ರತಿಜ್ಞಾ(ret_blue);

        ಯದಿ (!colors_enabled())
                ಪ್ರತ್ಯಾಗಮ -EOPNOTSUPP;

        r = terminal_verify_same(STDIN_FILENO, STDOUT_FILENO);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (streq_ptr(getenv("TERM"), "linux")) {
                /* Linux console is black */
                *ret_red = *ret_green = *ret_blue = 0.0;
                ಪ್ರತ್ಯಾಗಮ 0;
        }

        /* Open a 2nd input fd, in non-blocking mode, so that we won't ever hang in read()
         * should someone else process the POLLIN. Do all subsequent operations on the new fd. */
        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ nonblock_ಒಳಹರಿವು_fd = r = fd_reopen(STDIN_FILENO, O_RDONLY|O_CLOEXEC|O_NONBLOCK|O_NOCTTY);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಸಂರಚನಾ termios old_termios = TERMIOS_NULL;
        CLEANUP_TERMIOS_RESET(nonblock_ಒಳಹರಿವು_fd, old_termios);

        ಯದಿ (tcgetattr(nonblock_ಒಳಹರಿವು_fd, &old_termios) < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        ಸಂರಚನಾ termios new_termios = old_termios;
        termios_disable_echo(&new_termios);

        ಯದಿ (tcsetattr(nonblock_ಒಳಹರಿವು_fd, TCSANOW, &new_termios) < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        r = loop_write(STDOUT_FILENO, ANSI_OSC "11;?" ANSI_ST, SIZE_MAX);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        usec_t end = usec_add(now(CLOCK_MONOTONIC), CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        ವರ್ಣ buf[STRLEN(ANSI_OSC "11;rgb:0/0/0" ANSI_ST)]; /* shortest possible reply */
        size_t buf_full = 0;
        BackgroundColorContext context = {};

        ಪ್ರತಿ (bool first = true;; first = false) {
                ಯದಿ (buf_full == 0) {
                        usec_t n = now(CLOCK_MONOTONIC);
                        ಯದಿ (n >= end)
                                ಪ್ರತ್ಯಾಗಮ -EOPNOTSUPP;

                        r = fd_wait_for_event(nonblock_ಒಳಹರಿವು_fd, POLLIN, usec_sub_unsigned(end, n));
                        ಯದಿ (r < 0)
                                ಪ್ರತ್ಯಾಗಮ r;
                        ಯದಿ (r == 0)
                                ಪ್ರತ್ಯಾಗಮ -EOPNOTSUPP;

                        /* On the first try, read multiple characters, i.e. the shortest valid
                         * reply. Afterwards read byte-wise, since we don't want to read too much, and
                         * unnecessarily drop too many characters from the input queue. */
                        ssize_t l = ಪಠ(nonblock_ಒಳಹರಿವು_fd, buf, first ? ಪರಿಮಾಣ(buf) : 1);
                        ಯದಿ (l < 0) {
                                ಯದಿ (errno == EAGAIN)
                                        ಅನುವರ್ತ;
                                ಪ್ರತ್ಯಾಗಮ -errno;
                        }

                        ಪ್ರತಿಜ್ಞಾ((size_t) l <= ಪರಿಮಾಣ(buf));
                        buf_full = l;
                }

                size_t processed;
                r = scan_background_color_response(&context, buf, buf_full, &processed);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                ಪ್ರತಿಜ್ಞಾ(processed <= buf_full);
                buf_full -= processed;
                memmove(buf, buf + processed, buf_full);

                ಯದಿ (r > 0) {
                        ಪ್ರತಿಜ್ಞಾ(context.red_bits > 0);
                        *ret_red = (ದ್ವಿಗುಣ) context.red / ((UINT64_C(1) << context.red_bits) - 1);
                        ಪ್ರತಿಜ್ಞಾ(context.green_bits > 0);
                        *ret_green = (ದ್ವಿಗುಣ) context.green / ((UINT64_C(1) << context.green_bits) - 1);
                        ಪ್ರತಿಜ್ಞಾ(context.blue_bits > 0);
                        *ret_blue = (ದ್ವಿಗುಣ) context.blue / ((UINT64_C(1) << context.blue_bits) - 1);
                        ಪ್ರತ್ಯಾಗಮ 0;
                }
        }
}

/* Determine terminal dimensions by means of ANSI sequences: save the cursor via DECSC, position it far
 * to the bottom right (clamped to actual terminal dimensions), read back via DSR where we ended up, and
 * restore cursor via DECRC. Only needs a single DSR round-trip, and always restores the cursor regardless
 * of whether the response is received.
 *
 * Caller must have already opened a non-blocking input fd and configured termios (echo/icanon off). */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ terminal_query_size_by_dsr(
                ಪೂರ್ಣಾಙ್ಕ nonblock_ಒಳಹರಿವು_fd,
                ಪೂರ್ಣಾಙ್ಕ output_fd,
                unsigned *ret_rows,
                unsigned *ret_columns) {

        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(nonblock_ಒಳಹರಿವು_fd >= 0);
        ಪ್ರತಿಜ್ಞಾ(output_fd >= 0);

        /* Use DECSC/DECRC to save/restore cursor instead of querying position via DSR. This way the cursor
         * is always restored — even on timeout — and we only need one DSR response instead of two. */
        r = loop_write_full(output_fd,
                            "\x1B" "7"              /* DECSC: save cursor position */
                            "\x1B[32766;32766H"     /* CUP: position cursor far to the right and to the bottom, staying within 16bit signed range */
                            "\x1B[6n"               /* DSR: request cursor position (CPR) */
                            "\x1B" "8",             /* DECRC: restore cursor position */
                            SIZE_MAX,
                            CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        usec_t end = usec_add(now(CLOCK_MONOTONIC), CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        ವರ್ಣ buf[STRLEN("\x1B[1;1R")]; /* The shortest valid reply possible */
        size_t buf_full = 0;
        CursorPositionContext context = {};

        ಪ್ರತಿ (bool first = true;; first = false) {
                ಯದಿ (buf_full == 0) {
                        usec_t n = now(CLOCK_MONOTONIC);
                        ಯದಿ (n >= end)
                                ಪ್ರತ್ಯಾಗಮ -EOPNOTSUPP;

                        r = fd_wait_for_event(nonblock_ಒಳಹರಿವು_fd, POLLIN, usec_sub_unsigned(end, n));
                        ಯದಿ (r < 0)
                                ಪ್ರತ್ಯಾಗಮ r;
                        ಯದಿ (r == 0)
                                ಪ್ರತ್ಯಾಗಮ -EOPNOTSUPP;

                        /* On the first try, read multiple characters, i.e. the shortest valid
                         * reply. Afterwards read byte-wise, since we don't want to read too much, and
                         * unnecessarily drop too many characters from the input queue. */
                        ssize_t l = ಪಠ(nonblock_ಒಳಹರಿವು_fd, buf, first ? ಪರಿಮಾಣ(buf) : 1);
                        ಯದಿ (l < 0) {
                                ಯದಿ (errno == EAGAIN)
                                        ಅನುವರ್ತ;

                                ಪ್ರತ್ಯಾಗಮ -errno;
                        }

                        ಪ್ರತಿಜ್ಞಾ((size_t) l <= ಪರಿಮಾಣ(buf));
                        buf_full = l;
                }

                size_t processed;
                r = scan_cursor_position_response(&context, buf, buf_full, &processed);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                ಪ್ರತಿಜ್ಞಾ(processed <= buf_full);
                buf_full -= processed;
                memmove(buf, buf + processed, buf_full);

                ಯದಿ (r > 0) {
                        /* Superficial validity checks (no particular reason to check for < 4, it's
                         * just a way to look for unreasonably small values) */
                        ಯದಿ (context.row < 4 || context.column < 4 || context.row >= 32766 || context.column >= 32766)
                                ಪ್ರತ್ಯಾಗಮ -ENODATA;

                        ಯದಿ (ret_rows)
                                *ret_rows = context.row;
                        ಯದಿ (ret_columns)
                                *ret_columns = context.column;

                        ಪ್ರತ್ಯಾಗಮ 0;
                }
        }
}

/* Common setup for ANSI terminal queries: validate the fds, open a non-blocking input fd, and configure
 * termios with echo and canonical mode disabled. Caller must restore termios and close the fd when done. */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ terminal_prepare_query(
                ಪೂರ್ಣಾಙ್ಕ ಒಳಹರಿವು_fd,
                ಪೂರ್ಣಾಙ್ಕ output_fd,
                ಪೂರ್ಣಾಙ್ಕ *ret_nonblock_fd,
                ಸಂರಚನಾ termios *ret_saved_termios) {

        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(ಒಳಹರಿವು_fd >= 0);
        ಪ್ರತಿಜ್ಞಾ(output_fd >= 0);
        ಪ್ರತಿಜ್ಞಾ(ret_nonblock_fd);
        ಪ್ರತಿಜ್ಞಾ(ret_saved_termios);

        /* Use getenv_terminal_is_dumb() instead of terminal_is_dumb() here since we operate on an
         * explicitly passed fd, not on stdio. terminal_is_dumb() additionally checks on_tty() which
         * tests whether *stderr* is a tty — that's irrelevant when we're querying a directly opened
         * terminal such as /dev/console. */
        ಯದಿ (getenv_terminal_is_dumb())
                ಪ್ರತ್ಯಾಗಮ -EOPNOTSUPP;

        r = terminal_verify_same(ಒಳಹರಿವು_fd, output_fd);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ log_debug_errno(r, "Called with distinct input/output fds: %m");

        /* Open a 2nd input fd, in non-blocking mode, so that we won't ever hang in read()
         * should someone else process the POLLIN. Do all subsequent operations on the new fd. */
        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ nonblock_ಒಳಹರಿವು_fd = r = fd_reopen(ಒಳಹರಿವು_fd, O_RDONLY|O_CLOEXEC|O_NONBLOCK|O_NOCTTY);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (tcgetattr(nonblock_ಒಳಹರಿವು_fd, ret_saved_termios) < 0)
                ಪ್ರತ್ಯಾಗಮ log_debug_errno(errno, "Failed to get terminal settings: %m");

        ಸಂರಚನಾ termios new_termios = *ret_saved_termios;
        termios_disable_echo(&new_termios);

        ಯದಿ (tcsetattr(nonblock_ಒಳಹರಿವು_fd, TCSANOW, &new_termios) < 0)
                ಪ್ರತ್ಯಾಗಮ log_debug_errno(errno, "Failed to set new terminal settings: %m");

        *ret_nonblock_fd = TAKE_FD(nonblock_ಒಳಹರಿವು_fd);
        ಪ್ರತ್ಯಾಗಮ 0;
}

/*
 * See https://terminalguide.namepad.de/seq/csi_st-18/,
 * https://invisible-island.net/xterm/ctlseqs/ctlseqs.html#h3-Functions-using-CSI-_-ordered-by-the-final-character_s_.
 */
#define CSI18_Q  "\x1B[18t"               /* Report the size of the text area in characters */
#define CSI18_Rp "\x1B[8;"                /* Reply prefix */
#define CSI18_R0 CSI18_Rp "1;1t"          /* Shortest reply */
#define CSI18_R1 CSI18_Rp "32766;32766t"  /* Longest reply */

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ scan_text_area_size_response(
                ಧ್ರುವ ವರ್ಣ *buf,
                size_t size,
                unsigned *ret_rows,
                unsigned *ret_columns) {

        ಪ್ರತಿಜ್ಞಾ(buf);
        ಪ್ರತಿಜ್ಞಾ(ret_rows);
        ಪ್ರತಿಜ್ಞಾ(ret_columns);

        /* Check if we have enough space for the shortest possible answer. */
        ಯದಿ (size < STRLEN(CSI18_R0))
                ಪ್ರತ್ಯಾಗಮ -EAGAIN;

        /* Check if the terminating sequence is present */
        ಯದಿ (buf[size - 1] != 't')
                ಪ್ರತ್ಯಾಗಮ -EAGAIN;

        unsigned short rows, columns;
        ಯದಿ (sscanf(buf, CSI18_Rp "%hu;%hut", &rows, &columns) != 2)
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        *ret_rows = rows;
        *ret_columns = columns;
        ಪ್ರತ್ಯಾಗಮ 0;
}

/* Determine terminal dimensions by means of an ANSI CSI 18 sequence.
 *
 * Caller must have already opened a non-blocking input fd and configured termios (echo/icanon off). */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ terminal_query_size_by_csi18(
                ಪೂರ್ಣಾಙ್ಕ nonblock_ಒಳಹರಿವು_fd,
                ಪೂರ್ಣಾಙ್ಕ output_fd,
                unsigned *ret_rows,
                unsigned *ret_columns) {

        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(nonblock_ಒಳಹರಿವು_fd >= 0);
        ಪ್ರತಿಜ್ಞಾ(output_fd >= 0);

        r = loop_write_full(output_fd, CSI18_Q, SIZE_MAX, CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        usec_t end = usec_add(now(CLOCK_MONOTONIC), CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        ವರ್ಣ buf[STRLEN(CSI18_R1)];
        size_t bytes = 0;

        ಪ್ರತಿ (;;) {
                usec_t n = now(CLOCK_MONOTONIC);
                ಯದಿ (n >= end)
                        ಪ್ರತ್ಯಾಗಮ -EOPNOTSUPP;

                r = fd_wait_for_event(nonblock_ಒಳಹರಿವು_fd, POLLIN, usec_sub_unsigned(end, n));
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;
                ಯದಿ (r == 0)
                        ಪ್ರತ್ಯಾಗಮ -EOPNOTSUPP;

                /* On the first read, read multiple characters, i.e. the shortest valid reply. Afterwards
                 * read byte by byte, since we don't want to read too much and drop characters from the input
                 * queue. */
                ssize_t l = ಪಠ(nonblock_ಒಳಹರಿವು_fd, buf + bytes, bytes == 0 ? STRLEN(CSI18_R0) : 1);
                ಯದಿ (l < 0) {
                        ಯದಿ (errno == EAGAIN)
                                ಅನುವರ್ತ;
                        ಪ್ರತ್ಯಾಗಮ -errno;
                }

                ಪ್ರತಿಜ್ಞಾ((size_t) l <= ಪರಿಮಾಣ(buf) - bytes);
                bytes += l;

                r = scan_text_area_size_response(buf, bytes, ret_rows, ret_columns);
                ಯದಿ (r != -EAGAIN)
                        ಪ್ರತ್ಯಾಗಮ r;

                ಯದಿ (bytes == ಪರಿಮಾಣ(buf))
                        ಪ್ರತ್ಯಾಗಮ -EOPNOTSUPP; /* The response has the right prefix, but we didn't find a valid
                                             * answer with a terminator in the allotted space. Something is
                                             * wrong, possibly some unrelated bytes got injected into the
                                             * answer. */
        }
}

ಪೂರ್ಣಾಙ್ಕ terminal_get_size(
                ಪೂರ್ಣಾಙ್ಕ ಒಳಹರಿವು_fd,
                ಪೂರ್ಣಾಙ್ಕ output_fd,
                unsigned *ret_rows,
                unsigned *ret_columns,
                bool try_dsr,
                bool try_csi18) {

        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ nonblock_ಒಳಹರಿವು_fd = -EBADF;
        ಸಂರಚನಾ termios old_termios = TERMIOS_NULL;
        CLEANUP_TERMIOS_RESET(nonblock_ಒಳಹರಿವು_fd, old_termios);
        _cleanup_(nonblock_resetp) ಪೂರ್ಣಾಙ್ಕ nonblock_reset = -EBADF;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(try_dsr || try_csi18);

        r = terminal_prepare_query(ಒಳಹರಿವು_fd, output_fd, &nonblock_ಒಳಹರಿವು_fd, &old_termios);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        /* Put the output fd in non-blocking mode with a write timeout, to avoid blocking indefinitely on
         * write if the terminal is not consuming data (e.g. serial console with flow control). */
        r = fd_nonblock(output_fd, true);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ log_debug_errno(r, "Failed to set terminal to non-blocking mode: %m");
        ಯದಿ (r > 0)
                nonblock_reset = output_fd;

        /* Flush any stale input that might confuse the response parsers. */
        (ಶೂನ್ಯ) tcflush(nonblock_ಒಳಹರಿವು_fd, TCIFLUSH);

        ಯದಿ (try_csi18) {
                r = terminal_query_size_by_csi18(nonblock_ಒಳಹರಿವು_fd, output_fd, ret_rows, ret_columns);
                ಯದಿ (r >= 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                /* Query failed. Flush any outstanding input. */
                (ಶೂನ್ಯ) tcflush(nonblock_ಒಳಹರಿವು_fd, TCIFLUSH);

                ಯದಿ (!IN_SET(r, -EOPNOTSUPP, -EINVAL))
                        ಪ್ರತ್ಯಾಗಮ r;
        }

        ಯದಿ (try_dsr) {
                r = terminal_query_size_by_dsr(nonblock_ಒಳಹರಿವು_fd, output_fd, ret_rows, ret_columns);
                ಯದಿ (r >= 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                /* Query failed. Flush any outstanding input. */
                (ಶೂನ್ಯ) tcflush(nonblock_ಒಳಹರಿವು_fd, TCIFLUSH);
        }

        ಪ್ರತ್ಯಾಗಮ r;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ fd_is_vc(ಪೂರ್ಣಾಙ್ಕ fd) {
        _cleanup_free_ ವರ್ಣ *name = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        r = getttyname_harder(fd, &name);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ tty_is_vc_resolve(name);
}

ಪೂರ್ಣಾಙ್ಕ terminal_fix_size(ಪೂರ್ಣಾಙ್ಕ ಒಳಹರಿವು_fd, ಪೂರ್ಣಾಙ್ಕ output_fd) {
        unsigned rows, columns;
        ಪೂರ್ಣಾಙ್ಕ r;

        /* Don't ask a virtual console at all. There is no intermediary here that could be wrong about the
         * dimensions: the kernel renders the screen itself, so its winsize is authoritative. */
        r = fd_is_vc(ಒಳಹರಿವು_fd);
        ಯದಿ (r < 0)
                log_debug_errno(r, "Failed to determine whether terminal is a virtual console, assuming it is not: %m");
        ಅನ್ಯಥಾ ಯದಿ (r > 0) {
                log_debug("Not querying dimensions of a virtual console, kernel dimensions are authoritative.");
                ಪ್ರತ್ಯಾಗಮ 0;
        }

        ಸಂರಚನಾ winsize ws = {};
        ಯದಿ (ioctl(output_fd, TIOCGWINSZ, &ws) < 0)
                ಪ್ರತ್ಯಾಗಮ log_debug_errno(errno, "Failed to query terminal dimensions, ignoring: %m");

        r = terminal_get_size(ಒಳಹರಿವು_fd, output_fd, &rows, &columns, /* try_dsr= */ true, /* try_csi18= */ true);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ log_debug_errno(r, "Failed to acquire terminal dimensions via ANSI sequences, not adjusting terminal dimensions: %m");

        ಯದಿ (ws.ws_row == rows && ws.ws_col == columns) {
                log_debug("Terminal dimensions reported via ANSI sequences match currently set terminal dimensions, not changing.");
                ಪ್ರತ್ಯಾಗಮ 0;
        }

        ws.ws_col = columns;
        ws.ws_row = rows;

        ಯದಿ (ioctl(output_fd, TIOCSWINSZ, &ws) < 0)
                ಪ್ರತ್ಯಾಗಮ log_debug_errno(errno, "Failed to update terminal dimensions, ignoring: %m");

        log_debug("Fixed terminal dimensions to %ux%u based on ANSI sequence information.", columns, rows);
        ಪ್ರತ್ಯಾಗಮ 1;
}

#define MAX_TERMINFO_LENGTH 64
/* python -c 'print("".join(hex(ord(i))[2:] for i in "name").upper())' */
#define DCS_TERMINFO_Q ANSI_DCS "+q" "6E616D65" ANSI_ST
/* The answer is either 0+r… (invalid) or 1+r… (OK). */
#define DCS_TERMINFO_R0 ANSI_DCS "0+r" ANSI_ST
#define DCS_TERMINFO_R1 ANSI_DCS "1+r" "6E616D65" "=" /* This is followed by Pt ST. */
assert_cc(STRLEN(DCS_TERMINFO_R0) <= STRLEN(DCS_TERMINFO_R1 ANSI_ST));

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ scan_terminfo_response(
                ಧ್ರುವ ವರ್ಣ *buf,
                size_t size,
                ವರ್ಣ **ret_name) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(buf);
        ಪ್ರತಿಜ್ಞಾ(ret_name);

        /* Check if we have enough space for the shortest possible answer. */
        ಯದಿ (size < STRLEN(DCS_TERMINFO_R0))
                ಪ್ರತ್ಯಾಗಮ -EAGAIN;

        /* Check if the terminating sequence is present */
        ಯದಿ (memcmp(buf + size - STRLEN(ANSI_ST), ANSI_ST, STRLEN(ANSI_ST)) != 0)
                ಪ್ರತ್ಯಾಗಮ -EAGAIN;

        ಯದಿ (size <= STRLEN(DCS_TERMINFO_R1 ANSI_ST))
                ಪ್ರತ್ಯಾಗಮ -EINVAL;  /* The answer is invalid or empty */

        ಯದಿ (memcmp(buf, DCS_TERMINFO_R1, STRLEN(DCS_TERMINFO_R1)) != 0)
                ಪ್ರತ್ಯಾಗಮ -EINVAL;  /* The answer is not valid */

        _cleanup_free_ ಶೂನ್ಯ *dec = ರಿಕ್ತ;
        size_t dec_size;
        r = unhexmem_full(buf + STRLEN(DCS_TERMINFO_R1), size - STRLEN(DCS_TERMINFO_R1 ANSI_ST),
                          /* secure= */ false,
                          &dec, &dec_size);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತಿಜ್ಞಾ(((ಧ್ರುವ ವರ್ಣ *) dec)[dec_size] == '\0'); /* unhexmem appends NUL for our convenience */
        ಯದಿ (memchr(dec, '\0', dec_size) || string_has_cc(dec, ರಿಕ್ತ) || !filename_is_valid(dec))
                ಪ್ರತ್ಯಾಗಮ -EUCLEAN;

        *ret_name = TAKE_PTR(dec);
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ terminal_get_terminfo_by_dcs(ಪೂರ್ಣಾಙ್ಕ fd, ವರ್ಣ **ret_name) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);
        ಪ್ರತಿಜ್ಞಾ(ret_name);

        /* Note: fd must be in non-blocking read-write mode! */

        ಸಂರಚನಾ termios old_termios = TERMIOS_NULL;
        CLEANUP_TERMIOS_RESET(fd, old_termios);

        ಯದಿ (tcgetattr(fd, &old_termios) < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        ಸಂರಚನಾ termios new_termios = old_termios;
        termios_disable_echo(&new_termios);

        ಯದಿ (tcsetattr(fd, TCSANOW, &new_termios) < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        r = loop_write(fd, DCS_TERMINFO_Q, SIZE_MAX);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        usec_t end = usec_add(now(CLOCK_MONOTONIC), CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        ವರ್ಣ buf[STRLEN(DCS_TERMINFO_R1) + MAX_TERMINFO_LENGTH + STRLEN(ANSI_ST)];
        size_t bytes = 0;

        ಪ್ರತಿ (;;) {
                usec_t n = now(CLOCK_MONOTONIC);
                ಯದಿ (n >= end)
                        ಪ್ರತ್ಯಾಗಮ -EOPNOTSUPP;

                r = fd_wait_for_event(fd, POLLIN, usec_sub_unsigned(end, n));
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;
                ಯದಿ (r == 0)
                        ಪ್ರತ್ಯಾಗಮ -EOPNOTSUPP;

                /* On the first read, read multiple characters, i.e. the shortest valid reply. Afterwards
                 * read byte by byte, since we don't want to read too much and drop characters from the input
                 * queue. */
                ssize_t l = ಪಠ(fd, buf + bytes, bytes == 0 ? STRLEN(DCS_TERMINFO_R0) : 1);
                ಯದಿ (l < 0) {
                        ಯದಿ (errno == EAGAIN)
                                ಅನುವರ್ತ;
                        ಪ್ರತ್ಯಾಗಮ -errno;
                }

                ಪ್ರತಿಜ್ಞಾ((size_t) l <= ಪರಿಮಾಣ(buf) - bytes);
                bytes += l;

                r = scan_terminfo_response(buf, bytes, ret_name);
                ಯದಿ (r != -EAGAIN)
                        ಪ್ರತ್ಯಾಗಮ r;

                ಯದಿ (bytes == ಪರಿಮಾಣ(buf))
                        ಪ್ರತ್ಯಾಗಮ -EOPNOTSUPP; /* The response has the right prefix, but we didn't find a valid
                                             * answer with a terminator in the allotted space. Something is
                                             * wrong, possibly some unrelated bytes got injected into the
                                             * answer. */
        }
}

ಪೂರ್ಣಾಙ್ಕ have_terminfo_file(ಧ್ರುವ ವರ್ಣ *name) {
        /* This is a heuristic check if we have the file, using the directory layout used on
         * current Linux systems. Checks for other layouts can be added later if appropriate. */
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(filename_is_valid(name));

        _cleanup_free_ ವರ್ಣ *p = path_join("/usr/share/terminfo", CHAR_TO_STR(name[0]), name);
        ಯದಿ (!p)
                ಪ್ರತ್ಯಾಗಮ log_oom_debug();

        r = RET_NERRNO(access(p, F_OK));
        ಯದಿ (r == -ENOENT)
                ಪ್ರತ್ಯಾಗಮ false;
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;
        ಪ್ರತ್ಯಾಗಮ true;
}

ಪೂರ್ಣಾಙ್ಕ query_term_for_tty(ಧ್ರುವ ವರ್ಣ *tty, ವರ್ಣ **ret_term) {
        _cleanup_free_ ವರ್ಣ *dcs_term = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(tty);
        ಪ್ರತಿಜ್ಞಾ(ret_term);

        ಯದಿ (tty_is_vc_resolve(tty))
                ಪ್ರತ್ಯಾಗಮ strdup_to(ret_term, "linux");

        /* Try to query the terminal implementation that we're on. This will not work in all
         * cases, which is fine, since this is intended to be used as a fallback. */

        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ tty_fd = open_terminal(tty, O_RDWR|O_NOCTTY|O_CLOEXEC|O_NONBLOCK);
        ಯದಿ (tty_fd < 0)
                ಪ್ರತ್ಯಾಗಮ log_debug_errno(tty_fd, "Failed to open %s to query terminfo: %m", tty);

        r = terminal_get_terminfo_by_dcs(tty_fd, &dcs_term);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ log_debug_errno(r, "Failed to query %s for terminfo: %m", tty);

        r = have_terminfo_file(dcs_term);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ log_debug_errno(r, "Failed to look for terminfo %s: %m", dcs_term);
        ಯದಿ (r == 0)
                ಪ್ರತ್ಯಾಗಮ log_info_errno(SYNTHETIC_ERRNO(ENODATA),
                                      "Terminfo %s not found for %s.", dcs_term, tty);

        *ret_term = TAKE_PTR(dcs_term);
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ terminal_is_pty_fd(ಪೂರ್ಣಾಙ್ಕ fd) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        /* Returns true if we are looking at a pty, i.e. if it's backed by the /dev/pts/ file system */

        ಯದಿ (!isatty_safe(fd))
                ಪ್ರತ್ಯಾಗಮ false;

        r = is_fs_type_at(fd, ರಿಕ್ತ, DEVPTS_SUPER_MAGIC);
        ಯದಿ (r != 0)
                ಪ್ರತ್ಯಾಗಮ r;

        /* The ptmx device is weird, it exists twice, once inside and once outside devpts. To detect the
         * latter case, let's fire off an ioctl() that only works on ptmx devices. */

        ಪೂರ್ಣಾಙ್ಕ v;
        ಯದಿ (ioctl(fd, TIOCGPKT, &v) < 0) {
                ಯದಿ (ERRNO_IS_NOT_SUPPORTED(errno))
                        ಪ್ರತ್ಯಾಗಮ false;

                ಪ್ರತ್ಯಾಗಮ -errno;
        }

        ಪ್ರತ್ಯಾಗಮ true;
}

ಪೂರ್ಣಾಙ್ಕ pty_open_peer(ಪೂರ್ಣಾಙ್ಕ fd, ಪೂರ್ಣಾಙ್ಕ mode) {
        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        /* Opens the peer PTY using the new race-free TIOCGPTPEER ioctl() (kernel 4.13).
         *
         * This is safe to be called on TTYs from other namespaces. */

        ಪ್ರತಿಜ್ಞಾ((mode & (O_CREAT|O_PATH|O_DIRECTORY|O_TMPFILE)) == 0);

        /* This replicates the EIO retry logic of open_terminal() in a modified way. */
        ಪ್ರತಿ (unsigned c = 0;; c++) {
                ಪೂರ್ಣಾಙ್ಕ peer_fd = ioctl(fd, TIOCGPTPEER, mode);
                ಯದಿ (peer_fd >= 0)
                        ಪ್ರತ್ಯಾಗಮ peer_fd;

                ಯದಿ (errno != EIO)
                        ಪ್ರತ್ಯಾಗಮ -errno;

                /* Max 1s in total */
                ಯದಿ (c >= 20)
                        ಪ್ರತ್ಯಾಗಮ -EIO;

                (ಶೂನ್ಯ) usleep_safe(50 * USEC_PER_MSEC);
        }
}
