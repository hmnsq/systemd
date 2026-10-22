/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ <fcntl.h>
#ଅନ୍ତର୍ଭାଵ <linux/kd.h>
#ଅନ୍ତର୍ଭାଵ <linux/magic.h>
#ଅନ୍ତର୍ଭାଵ <linux/tiocl.h>
#ଅନ୍ତର୍ଭାଵ <linux/vt.h>
#ଅନ୍ତର୍ଭାଵ <poll.h>
#ଅନ୍ତର୍ଭାଵ <signal.h>
#ଅନ୍ତର୍ଭାଵ <stdlib.h>
#ଅନ୍ତର୍ଭାଵ <sys/inotify.h>
#ଅନ୍ତର୍ଭାଵ <sys/ioctl.h>
#ଅନ୍ତର୍ଭାଵ <sys/sysmacros.h>
#ଅନ୍ତର୍ଭାଵ <termios.h>
#ଅନ୍ତର୍ଭାଵ <time.h>
#ଅନ୍ତର୍ଭାଵ <unistd.h>

#ଅନ୍ତର୍ଭାଵ "alloc-util.h"
#ଅନ୍ତର୍ଭାଵ "ansi-color.h"
#ଅନ୍ତର୍ଭାଵ "chase.h"
#ଅନ୍ତର୍ଭାଵ "devnum-util.h"
#ଅନ୍ତର୍ଭାଵ "errno-util.h"
#ଅନ୍ତର୍ଭାଵ "extract-word.h"
#ଅନ୍ତର୍ଭାଵ "fd-util.h"
#ଅନ୍ତର୍ଭାଵ "fileio.h"
#ଅନ୍ତର୍ଭାଵ "fs-util.h"
#ଅନ୍ତର୍ଭାଵ "hexdecoct.h"
#ଅନ୍ତର୍ଭାଵ "inotify-util.h"
#ଅନ୍ତର୍ଭାଵ "io-util.h"
#ଅନ୍ତର୍ଭାଵ "log.h"
#ଅନ୍ତର୍ଭାଵ "namespace-util.h"
#ଅନ୍ତର୍ଭାଵ "parse-util.h"
#ଅନ୍ତର୍ଭାଵ "path-util.h"
#ଅନ୍ତର୍ଭାଵ "pidref.h"
#ଅନ୍ତର୍ଭାଵ "proc-cmdline.h"
#ଅନ୍ତର୍ଭାଵ "process-util.h"
#ଅନ୍ତର୍ଭାଵ "signal-util.h"
#ଅନ୍ତର୍ଭାଵ "socket-util.h"
#ଅନ୍ତର୍ଭାଵ "stat-util.h"
#ଅନ୍ତର୍ଭାଵ "stdio-util.h"
#ଅନ୍ତର୍ଭାଵ "string-util.h"
#ଅନ୍ତର୍ଭାଵ "strv.h"
#ଅନ୍ତର୍ଭାଵ "terminal-util.h"
#ଅନ୍ତର୍ଭାଵ "time-util.h"
#ଅନ୍ତର୍ଭାଵ "utf8.h"

/* How much to wait when reading/writing ANSI sequences from/to the console */
#define CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC (333 * USEC_PER_MSEC)

ସ୍ଥିର ଅସ୍ଥିର unsigned cached_columns = 0;
ସ୍ଥିର ଅସ୍ଥିର unsigned cached_ଧାଡ଼ିଗୁଡ଼ିକ = 0;

ସ୍ଥିର ଅସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା cached_on_tty = -1;
ସ୍ଥିର ଅସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା cached_on_dev_null = -1;

bool isatty_safe(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd) {
        ପ୍ରତିଜ୍ଞା(fd >= 0);

        ଯଦି (isatty(fd))
                ଫେରନ୍ତୁ true;

        /* Linux/glibc returns EIO for hung up TTY on isatty(). Which is wrong, the thing doesn't stop being
         * a TTY after all, just because it is temporarily hung up. Let's work around this here, until this
         * is fixed in glibc. See: https://sourceware.org/bugzilla/show_bug.cgi?id=32103 */
        ଯଦି (errno == EIO)
                ଫେରନ୍ତୁ true;

        /* Be resilient if we're working on stdio, since they're set up by parent process. */
        ପ୍ରତିଜ୍ଞା(errno != EBADF || IN_SET(fd, STDIN_FILENO, STDOUT_FILENO, STDERR_FILENO));

        ଫେରନ୍ତୁ false;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା chvt(ପୂର୍ଣ୍ଣସଂଖ୍ୟା vt) {
        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = -EBADF;

        /* Switch to the specified vt number. If the VT is specified <= 0 switch to the VT the kernel log messages go,
         * if that's configured. */

        fd = open_terminal("/dev/tty0", O_RDWR|O_NOCTTY|O_CLOEXEC|O_NONBLOCK);
        ଯଦି (fd < 0)
                ଫେରନ୍ତୁ fd;

        ଯଦି (vt <= 0) {
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା tiocl[2] = {
                        TIOCL_GETKMSGREDIRECT,
                        0
                };

                ଯଦି (ioctl(fd, TIOCLINUX, tiocl) < 0)
                        ଫେରନ୍ତୁ -errno;

                vt = tiocl[0] <= 0 ? 1 : tiocl[0];
        }

        ଫେରନ୍ତୁ RET_NERRNO(ioctl(fd, VT_ACTIVATE, vt));
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା read_one_char(FILE *f, ଅକ୍ଷର *ret, usec_t t, bool echo, bool *need_nl) {
        _cleanup_free_ ଅକ୍ଷର *ଧାଡ଼ି = ଖାଲି;
        ସଂରଚନା termios old_termios;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r, fd;

        ପ୍ରତିଜ୍ଞା(ret);

        ଯଦି (!f)
                f = stdin;

        /* If this is a terminal, then switch canonical mode off, so that we can read a single
         * character. (Note that fmemopen() streams do not have an fd associated with them, let's handle that
         * nicely.) If 'echo' is false we'll also disable ECHO mode so that the pressed key is not made
         * visible to the user. */
        fd = fileno(f);
        ଯଦି (fd >= 0 && tcgetattr(fd, &old_termios) >= 0) {
                ସଂରଚନା termios new_termios = old_termios;

                new_termios.c_lflag &= ~(ICANON|(echo ? 0 : ECHO));
                new_termios.c_cc[VMIN] = 1;
                new_termios.c_cc[VTIME] = 0;

                ଯଦି (tcsetattr(fd, TCSANOW, &new_termios) >= 0) {
                        ଅକ୍ଷର c;

                        ଯଦି (t != USEC_INFINITY) {
                                ଯଦି (fd_wait_for_event(fd, POLLIN, t) <= 0) {
                                        (ଶୂନ୍ୟ) tcsetattr(fd, TCSANOW, &old_termios);
                                        ଫେରନ୍ତୁ -ETIMEDOUT;
                                }
                        }

                        r = safe_fgetc(f, &c);
                        (ଶୂନ୍ୟ) tcsetattr(fd, TCSANOW, &old_termios);
                        ଯଦି (r < 0)
                                ଫେରନ୍ତୁ r;
                        ଯଦି (r == 0)
                                ଫେରନ୍ତୁ -EIO;

                        ଯଦି (need_nl)
                                *need_nl = c != '\n';

                        *ret = c;
                        ଫେରନ୍ତୁ 0;
                }
        }

        ଯଦି (t != USEC_INFINITY && fd >= 0) {
                /* Let's wait the specified amount of time for input. When we have no fd we skip this, under
                 * the assumption that this is an fmemopen() stream or so where waiting doesn't make sense
                 * anyway, as the data is either already in the stream or cannot possible be placed there
                 * while we access the stream */

                ଯଦି (fd_wait_for_event(fd, POLLIN, t) <= 0)
                        ଫେରନ୍ତୁ -ETIMEDOUT;
        }

        /* If this is not a terminal, then read a full line instead */

        r = read_ଧାଡ଼ି(f, 16, &ଧାଡ଼ି); /* longer than necessary, to eat up UTF-8 chars/vt100 key sequences */
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;
        ଯଦି (r == 0)
                ଫେରନ୍ତୁ -EIO;

        ଯଦି (strlen(ଧାଡ଼ି) != 1)
                ଫେରନ୍ତୁ -EBADMSG;

        ଯଦି (need_nl)
                *need_nl = false;

        *ret = ଧାଡ଼ି[0];
        ଫେରନ୍ତୁ 0;
}

#define DEFAULT_ASK_REFRESH_USEC (2*USEC_PER_SEC)

ପୂର୍ଣ୍ଣସଂଖ୍ୟା ask_char(ଅକ୍ଷର *ret, ଅଚଳ ଅକ୍ଷର *replies, ଅଚଳ ଅକ୍ଷର *fmt, ...) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(ret);
        ପ୍ରତିଜ୍ଞା(replies);
        ପ୍ରତିଜ୍ଞା(fmt);

        ପାଇଁ (;;) {
                va_list ap;
                ଅକ୍ଷର c;
                bool need_nl = true;

                fputs(ansi_highlight(), stdout);

                putchar('\r');

                va_start(ap, fmt);
                vprintf(fmt, ap);
                va_end(ap);

                fputs(ansi_normal(), stdout);

                fflush(stdout);

                r = read_one_char(stdin, &c, DEFAULT_ASK_REFRESH_USEC, /* echo= */ true, &need_nl);
                ଯଦି (r < 0) {

                        ଯଦି (r == -ETIMEDOUT)
                                ଜାରି;

                        ଯଦି (r == -EBADMSG) {
                                puts("Bad input, please try again.");
                                ଜାରି;
                        }

                        putchar('\n');
                        ଫେରନ୍ତୁ r;
                }

                ଯଦି (need_nl)
                        putchar('\n');

                ଯଦି (strchr(replies, c)) {
                        *ret = c;
                        ଫେରନ୍ତୁ 0;
                }

                puts("Read unexpected character, please try again.");
        }
}

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା CompletionResult{
        COMPLETION_ALREADY,       /* the input string is already complete */
        COMPLETION_FULL,          /* completed the input string to be complete now */
        COMPLETION_PARTIAL,       /* completed the input string so that is still incomplete */
        COMPLETION_NONE,          /* found no matching completion */
        _COMPLETION_RESULT_MAX,
        _COMPLETION_RESULT_INVALID = -EINVAL,
        _COMPLETION_RESULT_ERRNO_MAX = -ERRNO_MAX,
} CompletionResult;

ସ୍ଥିର CompletionResult pick_completion(ଅଚଳ ଅକ୍ଷର *string, ଅକ୍ଷର *ଅଚଳ*completions, ଅକ୍ଷର **ret) {
        _cleanup_free_ ଅକ୍ଷର *found = ଖାଲି;
        bool partial = false;

        ପ୍ରତିଜ୍ଞା(ret);

        string = strempty(string);

        STRV_FOREACH(c, completions) {

                /* Ignore entries that are not actually completions */
                ଯଦି (!startswith(*c, string))
                        ଜାରି;

                /* Store first completion that matches */
                ଯଦି (!found) {
                        found = strdup(*c);
                        ଯଦି (!found)
                                ଫେରନ୍ତୁ -ENOMEM;

                        ଜାରି;
                }

                /* If there's another completion that works truncate the one we already found by common
                 * prefix */
                size_t n = str_common_prefix(found, *c);
                ଯଦି (n == SIZE_MAX)
                        ଜାରି;

                found[n] = 0;
                partial = true;
        }

        *ret = TAKE_PTR(found);

        ଯଦି (!*ret)
                ଫେରନ୍ତୁ COMPLETION_NONE;
        ଯଦି (partial)
                ଫେରନ୍ତୁ COMPLETION_PARTIAL;

        ଫେରନ୍ତୁ streq(string, *ret) ? COMPLETION_ALREADY : COMPLETION_FULL;
}

ସ୍ଥିର ଶୂନ୍ୟ clear_by_backspace(size_t n) {
        /* Erase the specified number of character cells backwards on the terminal */
        ପାଇଁ (size_t i = 0; i < n; i++)
                fputs("\b \b", stdout);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା ask_string_full(
                ଅକ୍ଷର **ret,
                ଅଚଳ ଅକ୍ଷର *prefill,
                GetCompletionsCallback get_completions,
                ଶୂନ୍ୟ *userdata,
                ଅଚଳ ଅକ୍ଷର *text, ...) {

        va_list ap;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(ret);
        ପ୍ରତିଜ୍ଞା(text);

        _cleanup_free_ ଅକ୍ଷର *string = ଖାଲି;
        size_t n = 0;

        ଯଦି (prefill) {
                /* Prefill query with explicit data if specified */

                string = strdup(prefill);
                ଯଦି (!string)
                        ଫେରନ୍ତୁ -ENOMEM;

                n = strlen(string);

        } ଅନ୍ୟଥା ଯଦି (get_completions) {
                /* Otherwise, figure out what string to preselect the query with */
                _cleanup_strv_free_ ଅକ୍ଷର **completions = ଖାଲି;
                r = get_completions("", GET_COMPLETIONS_PRESELECT, &completions, userdata);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;

                CompletionResult cr = pick_completion(string, completions, &string);
                ଯଦି (cr < 0)
                        ଫେରନ୍ତୁ cr;

                n = strlen_ptr(string);
        }

        /* Output the prompt */
        fputs(ansi_highlight(), stdout);
        va_start(ap, text);
        vprintf(text, ap);
        va_end(ap);
        fputs(ansi_normal(), stdout);
        ଯଦି (string)
                fputs(string, stdout);
        fflush(stdout);

        /* Do interactive logic only if stdin + stdout are connected to the same place. And yes, we could use
         * STDIN_FILENO and STDOUT_FILENO here, but let's be overly correct for once, after all libc allows
         * swapping out stdin/stdout. */
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd_ନିବେଶ = fileno(stdin);
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd_output = fileno(stdout);
        ସଂରଚନା termios old_termios = TERMIOS_NULL;
        CLEANUP_TERMIOS_RESET(fd_ନିବେଶ, old_termios);

        ଯଦି (fd_ନିବେଶ < 0 || fd_output < 0 || same_fd(fd_ନିବେଶ, fd_output) <= 0)
                ଯାଅ fallback;

        /* Try to disable echo, which also tells us if this even is a terminal */
        ଯଦି (tcgetattr(fd_ନିବେଶ, &old_termios) < 0) {
                old_termios = TERMIOS_NULL;
                ଯାଅ fallback;
        }

        ସଂରଚନା termios new_termios = old_termios;
        termios_disable_echo(&new_termios);
        ଯଦି (tcsetattr(fd_ନିବେଶ, TCSANOW, &new_termios) < 0)
                ଫେରନ୍ତୁ -errno;

        ପାଇଁ (;;) {
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା c = fgetc(stdin);

                /* On EOF or NUL, end the request, don't output anything anymore */
                ଯଦି (IN_SET(c, EOF, 0))
                        ଭାଙ୍ଗ;

                /* On Return also end the request, but make this visible */
                ଯଦି (IN_SET(c, '\n', '\r')) {
                        fputc('\n', stdout);
                        ଭାଙ୍ଗ;
                }

                ଯଦି (c == '\t') {
                        /* Tab */

                        _cleanup_strv_free_ ଅକ୍ଷର **completions = ଖାଲି;
                        ଯଦି (get_completions) {
                                r = get_completions(string, /* flags= */ 0, &completions, userdata);
                                ଯଦି (r < 0)
                                        ଫେରନ୍ତୁ r;
                        }

                        _cleanup_free_ ଅକ୍ଷର *new_string = ଖାଲି;
                        CompletionResult cr = pick_completion(string, completions, &new_string);
                        ଯଦି (cr < 0)
                                ଫେରନ୍ତୁ cr;
                        ଯଦି (IN_SET(cr, COMPLETION_PARTIAL, COMPLETION_FULL)) {
                                /* Output the new suffix we learned */
                                fputs(ASSERT_PTR(startswith(new_string, strempty(string))), stdout);

                                /* And update the whole string */
                                free_and_replace(string, new_string);
                                n = strlen(string);
                        }
                        ଯଦି (cr == COMPLETION_NONE)
                                fputc('\a', stdout); /* BEL */

                        ଯଦି (IN_SET(cr, COMPLETION_PARTIAL, COMPLETION_ALREADY)) {
                                /* If this worked only partially, or if the user hit TAB even though we were
                                 * complete already, then show the remaining options (in the latter case just
                                 * the one). */
                                fputc('\n', stdout);

                                _cleanup_strv_free_ ଅକ୍ଷର **filtered = strv_filter_prefix(completions, string);
                                ଯଦି (!filtered)
                                        ଫେରନ୍ତୁ -ENOMEM;

                                r = show_menu(filtered,
                                              /* n_columns= */ SIZE_MAX,
                                              /* column_width= */ SIZE_MAX,
                                              /* ellipsize_percentage= */ 0,
                                              /* grey_prefix= */ string,
                                              /* with_numbers= */ false);
                                ଯଦି (r < 0)
                                        ଫେରନ୍ତୁ r;

                                /* Show the prompt again */
                                fputs(ansi_highlight(), stdout);
                                va_start(ap, text);
                                vprintf(text, ap);
                                va_end(ap);
                                fputs(ansi_normal(), stdout);
                                fputs(string, stdout);
                        }

                } ଅନ୍ୟଥା ଯଦି (IN_SET(c, '\b', 127)) {
                        /* Backspace */

                        ଯଦି (n == 0)
                                fputc('\a', stdout); /* BEL */
                        ଅନ୍ୟଥା {
                                size_t m = utf8_last_length(string, n);

                                ଅକ୍ଷର *e = string + n - m;
                                clear_by_backspace(utf8_console_width(e));

                                *e = 0;
                                n -= m;
                        }

                } ଅନ୍ୟଥା ଯଦି (c == 21) {
                        /* Ctrl-u → erase all input */

                        clear_by_backspace(utf8_console_width(string));
                        ଯଦି (string)
                                string[n = 0] = 0;
                        ଅନ୍ୟଥା
                                ପ୍ରତିଜ୍ଞା(n == 0);

                } ଅନ୍ୟଥା ଯଦି (c == 4) {
                        /* Ctrl-d → cancel this field input */

                        ଫେରନ୍ତୁ -ECANCELED;

                } ଅନ୍ୟଥା ଯଦି (char_is_cc(c) || n >= LINE_MAX)
                        /* refuse control characters and too long strings */
                        fputc('\a', stdout); /* BEL */
                ଅନ୍ୟଥା {
                        /* Regular char */

                        ଯଦି (!GREEDY_REALLOC(string, n+2))
                                ଫେରନ୍ତୁ -ENOMEM;

                        string[n++] = (ଅକ୍ଷର) c;
                        string[n] = 0;

                        fputc(c, stdout);
                }

                fflush(stdout);
        }

        ଯଦି (!string) {
                string = strdup("");
                ଯଦି (!string)
                        ଫେରନ୍ତୁ -ENOMEM;
        }

        *ret = TAKE_PTR(string);
        ଫେରନ୍ତୁ 0;

fallback:
        /* A simple fallback without TTY magic */
        string = mfree(string);
        r = read_ଧାଡ଼ି(stdin, LONG_ଧାଡ଼ି_MAX, &string);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;
        ଯଦି (r == 0)
                ଫେରନ୍ତୁ -EIO;

        *ret = TAKE_PTR(string);
        ଫେରନ୍ତୁ 0;
}

bool any_ଚାବି_to_proceed(ଶୂନ୍ୟ) {

        /* Insert a new line here as well as to when the user inputs, as this is also used during the boot up
         * sequence when status messages may be interleaved with the current program output. This ensures
         * that the status messages aren't appended on the same line as this message. */

        fputc('\n', stdout);
        fputs(ansi_highlight_magenta(), stdout);
        fputs("-- Press any key to proceed --", stdout);
        fputs(ansi_normal(), stdout);
        fputc('\n', stdout);
        fflush(stdout);

        ଅକ୍ଷର ଚାବି = 0;
        (ଶୂନ୍ୟ) read_one_char(stdin, &ଚାବି, USEC_INFINITY, /* echo= */ false, /* need_nl= */ ଖାଲି);

        fputc('\n', stdout);
        fflush(stdout);

        ଫେରନ୍ତୁ ଚାବି != 'q';
}

ସ୍ଥିର size_t widest_list_element(ଅକ୍ଷର *ଅଚଳ*l) {
        size_t w = 0;

        /* Returns the largest console width of all elements in 'l' */

        STRV_FOREACH(i, l)
                w = MAX(w, utf8_console_width(*i));

        ଫେରନ୍ତୁ w;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା show_menu(ଅକ୍ଷର **x,
              size_t n_columns,
              size_t column_width,
              unsigned ellipsize_percentage,
              ଅଚଳ ଅକ୍ଷର *grey_prefix,
              bool with_numbers) {

        ପ୍ରତିଜ୍ଞା(n_columns > 0);
        ପ୍ରତିଜ୍ଞା(column_width > 0);

        ଯଦି (n_columns == SIZE_MAX)
                n_columns = 3;

        ଯଦି (column_width == SIZE_MAX) {
                size_t widest = widest_list_element(x);

                /* If not specified, derive column width from screen width */
                size_t column_max = (columns()-1) / n_columns;

                /* Subtract room for numbers */
                ଯଦି (with_numbers)
                        column_max = LESS_BY(column_max, 6U);

                /* If columns would get too tight let's make this a linear list instead. */
                ଯଦି (column_max < 10 && widest > 10) {
                        n_columns = 1;
                        column_max = columns()-1;

                        ଯଦି (with_numbers)
                                column_max = LESS_BY(column_max, 6U);
                }

                /* Never make this narrower than 10 characters */
                column_max = MAX(column_max, 10U);
                column_width = CLAMP(widest+1, 10U, column_max);
        }

        size_t n = strv_length(x);
        size_t per_column = DIV_ROUND_UP(n, n_columns);

        size_t break_ଧାଡ଼ିଗୁଡ଼ିକ = ଧାଡ଼ିଗୁଡ଼ିକ();
        ଯଦି (break_ଧାଡ଼ିଗୁଡ଼ିକ > 2)
                break_ଧାଡ଼ିଗୁଡ଼ିକ--;

        /* The first page gets two extra lines, since we want to show a title */
        size_t break_modulo = break_ଧାଡ଼ିଗୁଡ଼ିକ;
        ଯଦି (break_modulo > 3)
                break_modulo -= 3;

        ପାଇଁ (size_t i = 0; i < per_column; i++) {

                ପାଇଁ (size_t j = 0; j < n_columns; j++) {
                        _cleanup_free_ ଅକ୍ଷର *e = ଖାଲି;

                        size_t p = j * per_column + i;
                        ଯଦି (p >= n)
                                ଭାଙ୍ଗ;

                        e = ellipsize(x[p], column_width, ellipsize_percentage);
                        ଯଦି (!e)
                                ଫେରନ୍ତୁ -ENOMEM;

                        ଯଦି (with_numbers)
                                printf("%s%4zu)%s ",
                                       ansi_grey(),
                                       p + 1,
                                       ansi_normal());

                        ଯଦି (grey_prefix && startswith(e, grey_prefix)) {
                                size_t k = MIN(strlen(grey_prefix), column_width);
                                printf("%s%.*s%s",
                                       ansi_grey(),
                                       (ପୂର୍ଣ୍ଣସଂଖ୍ୟା) k, e,
                                       ansi_normal());
                                printf("%-*s",
                                       (ପୂର୍ଣ୍ଣସଂଖ୍ୟା) (column_width - k), e+k);
                        } ଅନ୍ୟଥା
                                printf("%-*s", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା) column_width, e);
                }

                putchar('\n');

                /* on the first screen we reserve 2 extra lines for the title */
                ଯଦି (i % break_ଧାଡ଼ିଗୁଡ଼ିକ == break_modulo)
                        ଯଦି (!any_ଚାବି_to_proceed())
                                ଫେରନ୍ତୁ 0;
        }

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା open_terminal(ଅଚଳ ଅକ୍ଷର *name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା mode) {
        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = -EBADF;

        /*
         * If a TTY is in the process of being closed opening it might cause EIO. This is horribly awful, but
         * unlikely to be changed in the kernel. Hence we work around this problem by retrying a couple of
         * times.
         *
         * https://bugs.launchpad.net/ubuntu/+source/linux/+bug/554172/comments/245
         */

        ପ୍ରତିଜ୍ଞା((mode & (O_CREAT|O_PATH|O_DIRECTORY|O_TMPFILE)) == 0);

        ପାଇଁ (unsigned c = 0;; c++) {
                fd = open(name, mode, 0);
                ଯଦି (fd >= 0)
                        ଭାଙ୍ଗ;

                ଯଦି (errno != EIO)
                        ଫେରନ୍ତୁ -errno;

                /* Max 1s in total */
                ଯଦି (c >= 20)
                        ଫେରନ୍ତୁ -EIO;

                (ଶୂନ୍ୟ) usleep_safe(50 * USEC_PER_MSEC);
        }

        ଯଦି (!isatty_safe(fd))
                ଫେରନ୍ତୁ -ENOTTY;

        ଫେରନ୍ତୁ TAKE_FD(fd);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା acquire_terminal(
                ଅଚଳ ଅକ୍ଷର *name,
                AcquireTerminalFlags flags,
                usec_t timeout) {

        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା notify = -EBADF, fd = -EBADF;
        usec_t ts = USEC_INFINITY;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r, wd = -1;

        ପ୍ରତିଜ୍ଞା(name);

        AcquireTerminalFlags mode = flags & _ACQUIRE_TERMINAL_MODE_MASK;
        ପ୍ରତିଜ୍ଞା(IN_SET(mode, ACQUIRE_TERMINAL_TRY, ACQUIRE_TERMINAL_FORCE, ACQUIRE_TERMINAL_WAIT));
        ପ୍ରତିଜ୍ଞା(mode == ACQUIRE_TERMINAL_WAIT || !FLAGS_SET(flags, ACQUIRE_TERMINAL_WATCH_SIGTERM));

        /* We use inotify to be notified when the tty is closed. We create the watch before checking if we can actually
         * acquire it, so that we don't lose any event.
         *
         * Note: strictly speaking this actually watches for the device being closed, it does *not* really watch
         * whether a tty loses its controlling process. However, unless some rogue process uses TIOCNOTTY on /dev/tty
         * *after* closing its tty otherwise this will not become a problem. As long as the administrator makes sure to
         * not configure any service on the same tty as an untrusted user this should not be a problem. (Which they
         * probably should not do anyway.) */

        ଯଦି (mode == ACQUIRE_TERMINAL_WAIT) {
                notify = inotify_init1(IN_CLOEXEC | IN_NONBLOCK);
                ଯଦି (notify < 0)
                        ଫେରନ୍ତୁ -errno;

                wd = inotify_add_watch(notify, name, IN_CLOSE);
                ଯଦି (wd < 0)
                        ଫେରନ୍ତୁ -errno;

                ଯଦି (timeout != USEC_INFINITY)
                        ts = now(CLOCK_MONOTONIC);
        }

        /* If we are called with ACQUIRE_TERMINAL_WATCH_SIGTERM we'll unblock SIGTERM during ppoll() temporarily */
        sigset_t poll_ss;
        assert_se(sigprocmask(SIG_SETMASK, /* newset= */ ଖାଲି, &poll_ss) >= 0);
        ଯଦି (flags & ACQUIRE_TERMINAL_WATCH_SIGTERM) {
                assert_se(sigismember(&poll_ss, SIGTERM) > 0);
                assert_se(sigdelset(&poll_ss, SIGTERM) >= 0);
        }

        ପାଇଁ (;;) {
                ଯଦି (notify >= 0) {
                        r = flush_fd(notify);
                        ଯଦି (r < 0)
                                ଫେରନ୍ତୁ r;
                }

                /* We pass here O_NOCTTY only so that we can check the return value TIOCSCTTY and have a reliable way
                 * to figure out if we successfully became the controlling process of the tty */
                fd = open_terminal(name, O_RDWR|O_NOCTTY|O_CLOEXEC);
                ଯଦି (fd < 0)
                        ଫେରନ୍ତୁ fd;

                /* Temporarily ignore SIGHUP, so that we don't get SIGHUP'ed if we already own the tty. */
                ସଂରଚନା sigaction sa_old;
                assert_se(sigaction(SIGHUP, &sigaction_ignore, &sa_old) >= 0);

                /* First, try to get the tty */
                r = RET_NERRNO(ioctl(fd, TIOCSCTTY, mode == ACQUIRE_TERMINAL_FORCE));

                /* Reset signal handler to old value */
                assert_se(sigaction(SIGHUP, &sa_old, ଖାଲି) >= 0);

                /* Success? Exit the loop now! */
                ଯଦି (r >= 0)
                        ଭାଙ୍ଗ;

                /* Any failure besides -EPERM? Fail, regardless of the mode. */
                ଯଦି (r != -EPERM)
                        ଫେରନ୍ତୁ r;

                ଯଦି (flags & ACQUIRE_TERMINAL_PERMISSIVE) /* If we are in permissive mode, then EPERM is fine, turn this
                                                          * into a success. Note that EPERM is also returned if we
                                                          * already are the owner of the TTY. */
                        ଭାଙ୍ଗ;

                ଯଦି (mode != ACQUIRE_TERMINAL_WAIT) /* If we are in TRY or FORCE mode, then propagate EPERM as EPERM */
                        ଫେରନ୍ତୁ r;

                ପ୍ରତିଜ୍ଞା(notify >= 0);
                ପ୍ରତିଜ୍ଞା(wd >= 0);

                ପାଇଁ (;;) {
                        usec_t ବାମ;
                        ଯଦି (timeout == USEC_INFINITY)
                                ବାମ = USEC_INFINITY;
                        ଅନ୍ୟଥା {
                                ପ୍ରତିଜ୍ଞା(ts != USEC_INFINITY);

                                usec_t n = usec_sub_unsigned(now(CLOCK_MONOTONIC), ts);
                                ଯଦି (n >= timeout)
                                        ଫେରନ୍ତୁ -ETIMEDOUT;

                                ବାମ = timeout - n;
                        }

                        r = ppoll_usec_full(
                                        &(ସଂରଚନା pollfd) {
                                                .fd = notify,
                                                .events = POLLIN,
                                        },
                                        /* n_fds= */ 1,
                                        ବାମ,
                                        &poll_ss);
                        ଯଦି (r < 0)
                                ଫେରନ୍ତୁ r;
                        ଯଦି (r == 0)
                                ଫେରନ୍ତୁ -ETIMEDOUT;

                        ସଂଘ inotify_event_buffer buffer;
                        ssize_t l;
                        l = ପଠ(notify, &buffer, ଆକାର(buffer));
                        ଯଦି (l < 0) {
                                ଯଦି (ERRNO_IS_TRANSIENT(errno))
                                        ଜାରି;

                                ଫେରନ୍ତୁ -errno;
                        }

                        FOREACH_INOTIFY_EVENT(e, buffer, l) {
                                ଯଦି (e->mask & IN_Q_OVERFLOW) /* If we hit an inotify queue overflow, simply check if the terminal is up for grabs now. */
                                        ଭାଙ୍ଗ;

                                ଯଦି (e->wd != wd || !(e->mask & IN_CLOSE)) /* Safety checks */
                                        ଫେରନ୍ତୁ -EIO;
                        }

                        ଭାଙ୍ଗ;
                }

                /* We close the tty fd here since if the old session ended our handle will be dead. It's important that
                 * we do this after sleeping, so that we don't enter an endless loop. */
                fd = safe_close(fd);
        }

        ଫେରନ୍ତୁ TAKE_FD(fd);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା release_terminal(ଶୂନ୍ୟ) {
        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = -EBADF;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        fd = open("/dev/tty", O_RDWR|O_NOCTTY|O_CLOEXEC|O_NONBLOCK);
        ଯଦି (fd < 0)
                ଫେରନ୍ତୁ -errno;

        /* Temporarily ignore SIGHUP, so that we don't get SIGHUP'ed
         * by our own TIOCNOTTY */
        ସଂରଚନା sigaction sa_old;
        assert_se(sigaction(SIGHUP, &sigaction_ignore, &sa_old) >= 0);

        r = RET_NERRNO(ioctl(fd, TIOCNOTTY));

        assert_se(sigaction(SIGHUP, &sa_old, ଖାଲି) >= 0);

        ଫେରନ୍ତୁ r;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା terminal_new_session(ଶୂନ୍ୟ) {

        /* Make us the new session leader, and set stdin tty to be our controlling terminal.
         *
         * Why stdin? Well, the ctty logic is relevant for signal delivery mostly, i.e. if people hit C-c
         * or the line is hung up. Such events are basically just a form of input, via a side channel
         * (that side channel being signal delivery, i.e. SIGINT, SIGHUP et al). Hence we focus on input,
         * not output here. */

        ଯଦି (!isatty_safe(STDIN_FILENO))
                ଫେରନ୍ତୁ -ENXIO;

        (ଶୂନ୍ୟ) setsid();
        ଫେରନ୍ତୁ RET_NERRNO(ioctl(STDIN_FILENO, TIOCSCTTY, 0));
}

ଶୂନ୍ୟ terminal_detach_session(ଶୂନ୍ୟ) {
        (ଶୂନ୍ୟ) setsid();
        (ଶୂନ୍ୟ) release_terminal();
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା terminal_vhangup_fd(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd) {
        ପ୍ରତିଜ୍ଞା(fd >= 0);
        ଫେରନ୍ତୁ RET_NERRNO(ioctl(fd, TIOCVHANGUP));
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା terminal_vhangup(ଅଚଳ ଅକ୍ଷର *tty) {
        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = -EBADF;

        ପ୍ରତିଜ୍ଞା(tty);

        fd = open_terminal(tty, O_RDWR|O_NOCTTY|O_CLOEXEC);
        ଯଦି (fd < 0)
                ଫେରନ୍ତୁ fd;

        ଫେରନ୍ତୁ terminal_vhangup_fd(fd);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା vt_disallocate(ଅଚଳ ଅକ୍ଷର *tty_path) {
        ପ୍ରତିଜ୍ଞା(tty_path);

        /* Deallocate the VT if possible. If not possible (i.e. because it is the active one), at least clear
         * it entirely (including the scrollback buffer). */

        ପୂର୍ଣ୍ଣସଂଖ୍ୟା ttynr = vtnr_from_tty(tty_path);
        ଯଦି (ttynr > 0) {
                _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = open_terminal("/dev/tty0", O_RDWR|O_NOCTTY|O_CLOEXEC|O_NONBLOCK);
                ଯଦି (fd < 0)
                        ଫେରନ୍ତୁ fd;

                /* Try to deallocate */
                ଯଦି (ioctl(fd, VT_DISALLOCATE, ttynr) >= 0)
                        ଫେରନ୍ତୁ 0;
                ଯଦି (errno != EBUSY)
                        ଫେରନ୍ତୁ -errno;
        }

        /* So this is not a VT (in which case we cannot deallocate it), or we failed to deallocate. Let's at
         * least clear the screen. */

        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd2 = open_terminal(tty_path, O_WRONLY|O_NOCTTY|O_CLOEXEC|O_NONBLOCK);
        ଯଦି (fd2 < 0)
                ଫେରନ୍ତୁ fd2;

        ଫେରନ୍ତୁ loop_write_full(fd2,
                               "\033[r"   /* clear scrolling region */
                               "\033[H"   /* move home */
                               "\033[3J"  /* clear screen including scrollback, requires Linux 2.6.40 */
                               "\033c",   /* reset to initial state */
                               SIZE_MAX,
                               CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା vt_default_utf8(ଶୂନ୍ୟ) {
        /* Read the default VT UTF8 setting from the kernel */
        ଫେରନ୍ତୁ read_boolean_file("/sys/module/vt/parameters/default_utf8");
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା vt_reset_keyboard(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r, kb;

        ପ୍ରତିଜ୍ଞା(fd >= 0);

        /* If we can't read the default, then default to Unicode. It's 2024 after all. */
        r = vt_default_utf8();
        ଯଦି (r < 0)
                log_debug_errno(r, "Failed to determine kernel VT UTF-8 mode, assuming enabled: %m");

        kb = vt_default_utf8() != 0 ? K_UNICODE : K_XLATE;
        ଫେରନ୍ତୁ RET_NERRNO(ioctl(fd, KDSKBMODE, kb));
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା terminal_reset_ioctl(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, bool switch_to_text) {
        ସଂରଚନା termios termios;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        /* Set terminal to some sane defaults */

        ପ୍ରତିଜ୍ଞା(fd >= 0);

        /* We leave locked terminal attributes untouched, so that Plymouth may set whatever it wants to set,
         * and we don't interfere with that. */

        /* Disable exclusive mode, just in case */
        ଯଦି (ioctl(fd, TIOCNXCL) < 0)
                log_debug_errno(errno, "TIOCNXCL ioctl failed on TTY, ignoring: %m");

        /* Switch to text mode */
        ଯଦି (switch_to_text)
                ଯଦି (ioctl(fd, KDSETMODE, KD_TEXT) < 0)
                        log_debug_errno(errno, "KDSETMODE ioctl for switching to text mode failed on TTY, ignoring: %m");

        /* Set default keyboard mode */
        r = vt_reset_keyboard(fd);
        ଯଦି (r < 0)
                log_debug_errno(r, "Failed to reset VT keyboard, ignoring: %m");

        ଯଦି (tcgetattr(fd, &termios) < 0) {
                r = log_debug_errno(errno, "Failed to get terminal parameters: %m");
                ଯାଅ finish;
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
        ଯଦି (r < 0)
                log_debug_errno(r, "Failed to set terminal parameters: %m");

finish:
        /* Just in case, flush all crap out */
        (ଶୂନ୍ୟ) tcflush(fd, TCIOFLUSH);

        ଫେରନ୍ତୁ r;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା terminal_reset_ansi_seq(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd) {
        _cleanup_(nonblock_resetp) ପୂର୍ଣ୍ଣସଂଖ୍ୟା nonblock_reset = -EBADF;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(fd >= 0);

        ଯଦି (getenv_terminal_is_dumb())
                ଫେରନ୍ତୁ 0;

        r = fd_nonblock(fd, true);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ log_debug_errno(r, "Failed to set terminal to non-blocking mode: %m");
        ଯଦି (r > 0)
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
        ଯଦି (r < 0)
                log_debug_errno(r, "Failed to reset terminal through ANSI sequences: %m");

        ଫେରନ୍ତୁ r;
}

ଶୂନ୍ୟ reset_dev_console_fd(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, bool switch_to_text) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(fd >= 0);

        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା lock_fd = lock_dev_console();
        ଯଦି (lock_fd < 0)
                log_debug_errno(lock_fd, "Failed to lock /dev/console, ignoring: %m");

        r = terminal_reset_ioctl(fd, switch_to_text);
        ଯଦି (r < 0)
                log_warning_errno(r, "Failed to reset /dev/console, ignoring: %m");

        unsigned rows, cols;
        r = proc_cmdline_tty_size("/dev/console", &rows, &cols);
        ଯଦି (r < 0)
                log_warning_errno(r, "Failed to get /dev/console size, ignoring: %m");
        ଅନ୍ୟଥା ଯଦି (r > 0) {
                r = terminal_set_size_fd(fd, ଖାଲି, rows, cols);
                ଯଦି (r < 0)
                        log_warning_errno(r, "Failed to set configured terminal size on /dev/console, ignoring: %m");
        } ଅନ୍ୟଥା
                (ଶୂନ୍ୟ) terminal_fix_size(fd, fd);

        r = terminal_reset_ansi_seq(fd);
        ଯଦି (r < 0)
                log_warning_errno(r, "Failed to reset /dev/console using ANSI sequences, ignoring: %m");
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା lock_dev_console(ଶୂନ୍ୟ) {
        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = -EBADF;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        /* NB: We do not use O_NOFOLLOW here, because some container managers might place a symlink to some
         * pty in /dev/console, in which case it should be fine to lock the target TTY. */
        fd = open_terminal("/dev/console", O_RDONLY|O_CLOEXEC|O_NOCTTY);
        ଯଦି (fd < 0)
                ଫେରନ୍ତୁ fd;

        r = lock_generic(fd, LOCK_BSD, LOCK_EX);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ TAKE_FD(fd);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା make_console_stdio(bool switch_to_text) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, r;

        /* Make /dev/console the controlling terminal and stdin/stdout/stderr, if we can. If we can't use
         * /dev/null instead. This is particularly useful if /dev/console is turned off, e.g. if console=null
         * is specified on the kernel command line. */

        fd = acquire_terminal("/dev/console", ACQUIRE_TERMINAL_FORCE|ACQUIRE_TERMINAL_PERMISSIVE, USEC_INFINITY);
        ଯଦି (fd < 0) {
                log_warning_errno(fd, "Failed to acquire terminal, using /dev/null stdin/stdout/stderr instead: %m");

                r = make_null_stdio();
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ log_error_errno(r, "Failed to make /dev/null stdin/stdout/stderr: %m");

        } ଅନ୍ୟଥା {
                reset_dev_console_fd(fd, switch_to_text);

                r = rearrange_stdio(fd, fd, fd); /* This invalidates 'fd' both on success and on failure. */
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ log_error_errno(r, "Failed to make terminal stdin/stdout/stderr: %m");
        }

        reset_terminal_feature_caches();
        ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା vtnr_from_tty_raw(ଅଚଳ ଅକ୍ଷର *tty, unsigned *ret) {
        ପ୍ରତିଜ୍ଞା(tty);

        tty = skip_dev_prefix(tty);

        ଅଚଳ ଅକ୍ଷର *e = startswith(tty, "tty");
        ଯଦି (!e)
                ଫେରନ୍ତୁ -EINVAL;

        ଫେରନ୍ତୁ safe_atou(e, ret);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା vtnr_from_tty(ଅଚଳ ଅକ୍ଷର *tty) {
        unsigned u;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(tty);

        r = vtnr_from_tty_raw(tty, &u);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;
        ଯଦି (!vtnr_is_valid(u))
                ଫେରନ୍ତୁ -ERANGE;

        ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା) u;
}

bool tty_is_vc(ଅଚଳ ଅକ୍ଷର *tty) {
        ପ୍ରତିଜ୍ଞା(tty);

        /* NB: for >= 0 values no range check is conducted here, on the assumption that the caller will
         * either extract vtnr through vtnr_from_tty() later where ERANGE would be reported, or doesn't care
         * about whether it's strictly valid, but only asking "does this fall into the vt category?", for which
         * "yes" seems to be a better answer. */

        ଫେରନ୍ତୁ vtnr_from_tty_raw(tty, /* ret= */ ଖାଲି) >= 0;
}

bool tty_is_console(ଅଚଳ ଅକ୍ଷର *tty) {
        ପ୍ରତିଜ୍ଞା(tty);

        ଫେରନ୍ତୁ streq(skip_dev_prefix(tty), "console");
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା resolve_dev_console(ଅକ୍ଷର **ret) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(ret);

        /* Resolve where /dev/console is pointing to. If /dev/console is a symlink (like in container
         * managers), we'll just resolve the symlink. If it's a real device node, we'll use if
         * /sys/class/tty/tty0/active, but only if /sys/ is actually ours (i.e. not read-only-mounted which
         * is a sign for container setups). */

        _cleanup_free_ ଅକ୍ଷର *chased = ଖାଲି;
        r = chase("/dev/console", /* root= */ ଖାଲି, /* flags= */ 0, &chased, /* ret_fd= */ ଖାଲି);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;
        ଯଦି (!path_equal(chased, "/dev/console")) {
                *ret = TAKE_PTR(chased);
                ଫେରନ୍ତୁ 0;
        }

        r = path_is_read_only_fs("/sys");
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;
        ଯଦି (r > 0)
                ଫେରନ୍ତୁ -ENOMEDIUM;

        _cleanup_free_ ଅକ୍ଷର *active = ଖାଲି;
        r = read_one_ଧାଡ଼ି_file("/sys/class/tty/console/active", &active);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;
        ଯଦି (r == 0)
                ଫେରନ୍ତୁ -ENXIO;

        /* If multiple log outputs are configured the last one is what /dev/console points to */
        ଅଚଳ ଅକ୍ଷର *tty = strrchr(active, ' ');
        ଯଦି (tty)
                tty++;
        ଅନ୍ୟଥା
                tty = active;

        ଯଦି (streq(tty, "tty0")) {
                active = mfree(active);

                /* Get the active VC (e.g. tty1) */
                r = read_one_ଧାଡ଼ି_file("/sys/class/tty/tty0/active", &active);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
                ଯଦି (r == 0)
                        ଫେରନ୍ତୁ -ENXIO;

                tty = active;
        }

        _cleanup_free_ ଅକ୍ଷର *path = ଖାଲି;
        path = path_join("/dev", tty);
        ଯଦି (!path)
                ଫେରନ୍ତୁ -ENOMEM;

        *ret = TAKE_PTR(path);
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_kernel_consoles(ଅକ୍ଷର ***ret) {
        _cleanup_strv_free_ ଅକ୍ଷର **l = ଖାଲି;
        _cleanup_free_ ଅକ୍ଷର *ଧାଡ଼ି = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(ret);

        /* If /sys/ is mounted read-only this means we are running in some kind of container environment.
         * In that case /sys/ would reflect the host system, not us, hence ignore the data we can read from it. */
        ଯଦି (path_is_read_only_fs("/sys") > 0)
                ଯାଅ fallback;

        r = read_one_ଧାଡ଼ି_file("/sys/class/tty/console/active", &ଧାଡ଼ି);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ପାଇଁ (ଅଚଳ ଅକ୍ଷର *p = ଧାଡ଼ି;;) {
                _cleanup_free_ ଅକ୍ଷର *tty = ଖାଲି, *path = ଖାଲି;

                r = extract_first_word(&p, &tty, ଖାଲି, 0);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
                ଯଦି (r == 0)
                        ଭାଙ୍ଗ;

                ଯଦି (streq(tty, "tty0")) {
                        tty = mfree(tty);
                        r = read_one_ଧାଡ଼ି_file("/sys/class/tty/tty0/active", &tty);
                        ଯଦି (r < 0)
                                ଫେରନ୍ତୁ r;
                        ଯଦି (r == 0) {
                                log_debug("No VT active, skipping /dev/tty0.");
                                ଜାରି;
                        }
                }

                path = path_join("/dev", tty);
                ଯଦି (!path)
                        ଫେରନ୍ତୁ -ENOMEM;

                ଯଦି (access(path, F_OK) < 0) {
                        log_debug_errno(errno, "Console device %s is not accessible, skipping: %m", path);
                        ଜାରି;
                }

                r = strv_consume(&l, TAKE_PTR(path));
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
        }

        ଯଦି (strv_isempty(l)) {
                log_debug("No devices found for system console");
                ଯାଅ fallback;
        }

        *ret = TAKE_PTR(l);
        ଫେରନ୍ତୁ strv_length(*ret);

fallback:
        r = strv_extend(&l, "/dev/console");
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        *ret = TAKE_PTR(l);
        ଫେରନ୍ତୁ 0;
}

bool tty_is_vc_resolve(ଅଚଳ ଅକ୍ଷର *tty) {
        _cleanup_free_ ଅକ୍ଷର *resolved = ଖାଲି;

        ପ୍ରତିଜ୍ଞା(tty);

        ଯଦି (streq(skip_dev_prefix(tty), "console")) {
                ଯଦି (resolve_dev_console(&resolved) < 0)
                        ଫେରନ୍ତୁ false;

                tty = resolved;
        }

        ଫେରନ୍ତୁ tty_is_vc(tty);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd_columns(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd) {
        ସଂରଚନା winsize ws = {};

        ଯଦି (fd < 0)
                ଫେରନ୍ତୁ -EBADF;

        ଯଦି (ioctl(fd, TIOCGWINSZ, &ws) < 0)
                ଫେରନ୍ତୁ -errno;

        ଯଦି (ws.ws_col <= 0)
                ଫେରନ୍ତୁ -ENODATA; /* some tty types come up with invalid row/column initially, return a recognizable error for that */

        ଫେରନ୍ତୁ ws.ws_col;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା getenv_columns(ଶୂନ୍ୟ) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ଅଚଳ ଅକ୍ଷର *e = getenv("COLUMNS");
        ଯଦି (!e)
                ଫେରନ୍ତୁ -ENXIO;

        unsigned c;
        r = safe_atou_bounded(e, 1, USHRT_MAX, &c);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା) c;
}

unsigned columns(ଶୂନ୍ୟ) {

        ଯଦି (cached_columns > 0)
                ଫେରନ୍ତୁ cached_columns;

        ପୂର୍ଣ୍ଣସଂଖ୍ୟା c = getenv_columns();
        ଯଦି (c < 0) {
                c = fd_columns(STDOUT_FILENO);
                ଯଦି (c < 0)
                        c = 80;
        }

        ପ୍ରତିଜ୍ଞା(c > 0);

        cached_columns = c;
        ଫେରନ୍ତୁ cached_columns;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd_ଧାଡ଼ିଗୁଡ଼ିକ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd) {
        ସଂରଚନା winsize ws = {};

        ଯଦି (fd < 0)
                ଫେରନ୍ତୁ -EBADF;

        ଯଦି (ioctl(fd, TIOCGWINSZ, &ws) < 0)
                ଫେରନ୍ତୁ -errno;

        ଯଦି (ws.ws_row <= 0)
                ଫେରନ୍ତୁ -ENODATA; /* some tty types come up with invalid row/column initially, return a recognizable error for that */

        ଫେରନ୍ତୁ ws.ws_row;
}

unsigned ଧାଡ଼ିଗୁଡ଼ିକ(ଶୂନ୍ୟ) {
        ଅଚଳ ଅକ୍ଷର *e;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା l;

        ଯଦି (cached_ଧାଡ଼ିଗୁଡ଼ିକ > 0)
                ଫେରନ୍ତୁ cached_ଧାଡ଼ିଗୁଡ଼ିକ;

        l = 0;
        e = getenv("LINES");
        ଯଦି (e)
                (ଶୂନ୍ୟ) safe_atoi(e, &l);

        ଯଦି (l <= 0 || l > USHRT_MAX) {
                l = fd_ଧାଡ଼ିଗୁଡ଼ିକ(STDOUT_FILENO);
                ଯଦି (l <= 0)
                        l = 24;
        }

        cached_ଧାଡ଼ିଗୁଡ଼ିକ = l;
        ଫେରନ୍ତୁ cached_ଧାଡ଼ିଗୁଡ଼ିକ;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା terminal_set_size_fd(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ଅଚଳ ଅକ୍ଷର *ident, unsigned rows, unsigned cols) {
        ସଂରଚନା winsize ws;

        ପ୍ରତିଜ୍ଞା(fd >= 0);

        ଯଦି (!ident)
                ident = "TTY";

        ଯଦି (rows == UINT_MAX && cols == UINT_MAX)
                ଫେରନ୍ତୁ 0;

        ଯଦି (ioctl(fd, TIOCGWINSZ, &ws) < 0)
                ଫେରନ୍ତୁ log_debug_errno(errno,
                                       "TIOCGWINSZ ioctl for getting %s size failed, not setting terminal size: %m",
                                       ident);

        ଯଦି (rows == UINT_MAX)
                rows = ws.ws_row;
        ଅନ୍ୟଥା ଯଦି (rows > USHRT_MAX)
                rows = USHRT_MAX;

        ଯଦି (cols == UINT_MAX)
                cols = ws.ws_col;
        ଅନ୍ୟଥା ଯଦି (cols > USHRT_MAX)
                cols = USHRT_MAX;

        ଯଦି (rows == ws.ws_row && cols == ws.ws_col)
                ଫେରନ୍ତୁ 0;

        ws.ws_row = rows;
        ws.ws_col = cols;

        ଯଦି (ioctl(fd, TIOCSWINSZ, &ws) < 0)
                ଫେରନ୍ତୁ log_debug_errno(errno, "TIOCSWINSZ ioctl for setting %s size failed: %m", ident);

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା proc_cmdline_tty_size(ଅଚଳ ଅକ୍ଷର *tty, unsigned *ret_rows, unsigned *ret_cols) {
        _cleanup_free_ ଅକ୍ଷର *rowskey = ଖାଲି, *rowsvalue = ଖାଲି, *colskey = ଖାଲି, *colsvalue = ଖାଲି;
        unsigned rows = UINT_MAX, cols = UINT_MAX;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(tty);

        ଯଦି (!ret_rows && !ret_cols)
                ଫେରନ୍ତୁ 0;

        tty = skip_dev_prefix(tty);
        ଯଦି (path_startswith(tty, "pts/"))
                ଫେରନ୍ତୁ -EMEDIUMTYPE;
        ଯଦି (!in_charset(tty, ALPHANUMERICAL))
                ଫେରନ୍ତୁ log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "TTY name '%s' contains non-alphanumeric characters, not searching kernel cmdline for size.", tty);

        rowskey = strjoin("systemd.tty.rows.", tty);
        ଯଦି (!rowskey)
                ଫେରନ୍ତୁ -ENOMEM;

        colskey = strjoin("systemd.tty.columns.", tty);
        ଯଦି (!colskey)
                ଫେରନ୍ତୁ -ENOMEM;

        r = proc_cmdline_get_ଚାବି_many(/* flags= */ 0,
                                      rowskey, &rowsvalue,
                                      colskey, &colsvalue);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ log_debug_errno(r, "Failed to read TTY size of %s from kernel cmdline: %m", tty);

        ଯଦି (rowsvalue) {
                r = safe_atou(rowsvalue, &rows);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ log_debug_errno(r, "Failed to parse %s=%s: %m", rowskey, rowsvalue);
        }

        ଯଦି (colsvalue) {
                r = safe_atou(colsvalue, &cols);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ log_debug_errno(r, "Failed to parse %s=%s: %m", colskey, colsvalue);
        }

        ଯଦି (ret_rows)
                *ret_rows = rows;
        ଯଦି (ret_cols)
                *ret_cols = cols;

        ଫେରନ୍ତୁ rows != UINT_MAX || cols != UINT_MAX;
}

/* intended to be used as a SIGWINCH sighandler */
ଶୂନ୍ୟ columns_ଧାଡ଼ିଗୁଡ଼ିକ_cache_reset(ପୂର୍ଣ୍ଣସଂଖ୍ୟା signum) {
        cached_columns = 0;
        cached_ଧାଡ଼ିଗୁଡ଼ିକ = 0;
}

ଶୂନ୍ୟ reset_terminal_feature_caches(ଶୂନ୍ୟ) {
        cached_columns = 0;
        cached_ଧାଡ଼ିଗୁଡ଼ିକ = 0;

        cached_on_tty = -1;
        cached_on_dev_null = -1;

        reset_ansi_feature_caches();
}

bool on_tty(ଶୂନ୍ୟ) {

        /* We check both stdout and stderr, so that situations where pipes on the shell are used are reliably
         * recognized, regardless if only the output or the errors are piped to some place. Since on_tty() is generally
         * used to default to a safer, non-interactive, non-color mode of operation it's probably good to be defensive
         * here, and check for both. Note that we don't check for STDIN_FILENO, because it should fine to use fancy
         * terminal functionality when outputting stuff, even if the input is piped to us. */

        ଯଦି (cached_on_tty < 0)
                cached_on_tty =
                        isatty_safe(STDOUT_FILENO) &&
                        isatty_safe(STDERR_FILENO);

        ଫେରନ୍ତୁ cached_on_tty;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା getttyname_malloc(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ଅକ୍ଷର **ret) {
        ଅକ୍ଷର path[PATH_MAX]; /* PATH_MAX is counted *with* the trailing NUL byte */
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(fd >= 0);
        ପ୍ରତିଜ୍ଞା(ret);

        r = ttyname_r(fd, path, ଆକାର path); /* positive error */
        ପ୍ରତିଜ୍ଞା(r >= 0);
        ଯଦି (r == ERANGE)
                ଫେରନ୍ତୁ -ENAMETOOLONG;
        ଯଦି (r > 0)
                ଫେରନ୍ତୁ -r;

        ଫେରନ୍ତୁ strdup_to(ret, skip_dev_prefix(path));
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା getttyname_harder(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ଅକ୍ଷର **ret) {
        _cleanup_free_ ଅକ୍ଷର *s = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(ret);

        r = getttyname_malloc(fd, &s);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (streq(s, "tty"))
                ଫେରନ୍ତୁ get_ctty(0, ଖାଲି, ret);

        *ret = TAKE_PTR(s);
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_ctty_devnr(pid_t pid, dev_t *ret) {
        _cleanup_free_ ଅକ୍ଷର *ଧାଡ଼ି = ଖାଲି;
        unsigned long ttynr;
        ଅଚଳ ଅକ୍ଷର *p;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(pid >= 0);

        p = procfs_file_alloca(pid, "stat");
        r = read_one_ଧାଡ଼ି_file(p, &ଧାଡ଼ି);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        p = strrchr(ଧାଡ଼ି, ')');
        ଯଦି (!p)
                ଫେରନ୍ତୁ -EIO;

        p++;

        ଯଦି (sscanf(p, " "
                   "%*c "  /* state */
                   "%*d "  /* ppid */
                   "%*d "  /* pgrp */
                   "%*d "  /* session */
                   "%lu ", /* ttynr */
                   &ttynr) != 1)
                ଫେରନ୍ତୁ -EIO;

        ଯଦି (devnum_is_zero(ttynr))
                ଫେରନ୍ତୁ -ENXIO;

        ଯଦି (ret)
                *ret = (dev_t) ttynr;

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_ctty(pid_t pid, dev_t *ret_devnr, ଅକ୍ଷର **ret) {
        ଅକ୍ଷର pty[STRLEN("/dev/pts/") + DECIMAL_STR_MAX(dev_t) + 1];
        _cleanup_free_ ଅକ୍ଷର *buf = ଖାଲି;
        ଅଚଳ ଅକ୍ଷର *fn = ଖାଲି, *w;
        dev_t devnr;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        r = get_ctty_devnr(pid, &devnr);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        r = device_path_make_canonical(S_IFCHR, devnr, &buf);
        ଯଦି (r < 0) {
                ସଂରଚନା stat st;

                ଯଦି (r != -ENOENT) /* No symlink for this in /dev/char/? */
                        ଫେରନ୍ତୁ r;

                /* Maybe this is PTY? PTY devices are not listed in /dev/char/, as they don't follow the
                 * Linux device model and hence device_path_make_canonical() doesn't work for them. Let's
                 * assume this is a PTY for a moment, and check if the device node this would then map to in
                 * /dev/pts/ matches the one we are looking for. This way we don't have to hardcode the major
                 * number (which is 136 btw), but we still rely on the fact that PTY numbers map directly to
                 * the minor number of the pty. */
                xsprintf(pty, "/dev/pts/%u", minor(devnr));

                ଯଦି (stat(pty, &st) < 0) {
                        ଯଦି (errno != ENOENT)
                                ଫେରନ୍ତୁ -errno;

                } ଅନ୍ୟଥା ଯଦି (S_ISCHR(st.st_mode) && devnr == st.st_rdev) /* Bingo! */
                        fn = pty;

                ଯଦି (!fn) {
                        /* Doesn't exist, or not a PTY? Probably something similar to the PTYs which have no
                         * symlink in /dev/char/. Let's return something vaguely useful. */
                        r = device_path_make_major_minor(S_IFCHR, devnr, &buf);
                        ଯଦି (r < 0)
                                ଫେରନ୍ତୁ r;

                        fn = buf;
                }
        } ଅନ୍ୟଥା
                fn = buf;

        w = path_startswith(fn, "/dev/");
        ଯଦି (!w)
                ଫେରନ୍ତୁ -EINVAL;

        ଯଦି (ret) {
                r = strdup_to(ret, w);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
        }

        ଯଦି (ret_devnr)
                *ret_devnr = devnr;

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା ptsname_malloc(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ଅକ୍ଷର **ret) {
        ପ୍ରତିଜ୍ଞା(fd >= 0);
        ପ୍ରତିଜ୍ଞା(ret);

        ପାଇଁ (size_t l = 50;;) {
                _cleanup_free_ ଅକ୍ଷର *c = ଖାଲି;

                c = new(ଅକ୍ଷର, l);
                ଯଦି (!c)
                        ଫେରନ୍ତୁ -ENOMEM;

                ଯଦି (ptsname_r(fd, c, l) >= 0) {
                        *ret = TAKE_PTR(c);
                        ଫେରନ୍ତୁ 0;
                }
                ଯଦି (errno != ERANGE)
                        ଫେରନ୍ତୁ -errno;

                ଯଦି (!MUL_ASSIGN_SAFE(&l, 2))
                        ଫେରନ୍ତୁ -ENOMEM;
        }
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା openpt_allocate(ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags, ଅକ୍ଷର **ret_peer_path) {
        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = -EBADF;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        fd = posix_openpt(flags|O_NOCTTY|O_CLOEXEC);
        ଯଦି (fd < 0)
                ଫେରନ୍ତୁ -errno;

        _cleanup_free_ ଅକ୍ଷର *p = ଖାଲି;
        ଯଦି (ret_peer_path) {
                r = ptsname_malloc(fd, &p);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;

                ଯଦି (!path_startswith(p, "/dev/pts/"))
                        ଫେରନ୍ତୁ -EINVAL;
        }

        ଯଦି (unlockpt(fd) < 0)
                ଫେରନ୍ତୁ -errno;

        ଯଦି (ret_peer_path)
                *ret_peer_path = TAKE_PTR(p);

        ଫେରନ୍ତୁ TAKE_FD(fd);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା ptsname_namespace(ପୂର୍ଣ୍ଣସଂଖ୍ୟା pty, ଅକ୍ଷର **ret) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା no = -1;

        ପ୍ରତିଜ୍ଞା(pty >= 0);
        ପ୍ରତିଜ୍ଞା(ret);

        /* Like ptsname(), but doesn't assume that the path is
         * accessible in the local namespace. */

        ଯଦି (ioctl(pty, TIOCGPTN, &no) < 0)
                ଫେରନ୍ତୁ -errno;

        ଯଦି (no < 0)
                ଫେରନ୍ତୁ -EIO;

        ଯଦି (asprintf(ret, "/dev/pts/%i", no) < 0)
                ଫେରନ୍ତୁ -ENOMEM;

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା openpt_allocate_in_namespace(
                ଅଚଳ PidRef *pidref,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags,
                ଅକ୍ଷର **ret_peer_path) {

        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidnsfd = -EBADF, mntnsfd = -EBADF, usernsfd = -EBADF, rootfd = -EBADF, fd = -EBADF;
        _cleanup_close_pair_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା pair[2] = EBADF_PAIR;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(pidref);

        r = pidref_namespace_open(pidref, &pidnsfd, &mntnsfd, /* ret_netns_fd= */ ଖାଲି, &usernsfd, &rootfd);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ log_debug_errno(r, "Failed to open namespaces of PID "PID_FMT": %m", pidref->pid);

        ଯଦି (socketpair(AF_UNIX, SOCK_DGRAM|SOCK_CLOEXEC, 0, pair) < 0)
                ଫେରନ୍ତୁ -errno;

        r = namespace_fork(
                        "(sd-openptns)",
                        "(sd-openpt)",
                        FORK_RESET_SIGNALS|FORK_DEATHSIG_SIGKILL|FORK_WAIT,
                        pidnsfd,
                        mntnsfd,
                        /* netns_fd= */ -EBADF,
                        usernsfd,
                        rootfd,
                        /* ret= */ ଖାଲି);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;
        ଯଦି (r == 0) {
                pair[0] = safe_close(pair[0]);

                fd = openpt_allocate(flags, /* ret_peer_path= */ ଖାଲି);
                ଯଦି (fd < 0)
                        _exit(EXIT_FAILURE);

                ଯଦି (send_one_fd(pair[1], fd, 0) < 0)
                        _exit(EXIT_FAILURE);

                _exit(EXIT_SUCCESS);
        }

        pair[1] = safe_close(pair[1]);

        fd = receive_one_fd(pair[0], 0);
        ଯଦି (fd < 0)
                ଫେରନ୍ତୁ fd;

        ଯଦି (ret_peer_path) {
                r = ptsname_namespace(fd, ret_peer_path);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
        }

        ଫେରନ୍ତୁ TAKE_FD(fd);
}

ସ୍ଥିର bool on_dev_null(ଶୂନ୍ୟ) {
        ସଂରଚନା stat dst, ost, est;

        ଯଦି (cached_on_dev_null >= 0)
                ଫେରନ୍ତୁ cached_on_dev_null;

        ଯଦି (stat("/dev/null", &dst) < 0 || fstat(STDOUT_FILENO, &ost) < 0 || fstat(STDERR_FILENO, &est) < 0)
                cached_on_dev_null = false;
        ଅନ୍ୟଥା
                cached_on_dev_null = stat_inode_same(&dst, &ost) && stat_inode_same(&dst, &est);

        ଫେରନ୍ତୁ cached_on_dev_null;
}

bool term_env_valid(ଅଚଳ ଅକ୍ଷର *term) {
        /* Checks if the specified $TERM value is suitable for propagation, i.e. is not empty, not set to
         * "unknown" (as is common in CI), and only contains characters valid in terminal type names.
         * Valid $TERM values are things like "xterm-256color", "linux", "screen.xterm-256color", i.e.
         * alphanumeric characters, hyphens, underscores, dots, and plus signs. */
        ଫେରନ୍ତୁ !isempty(term) &&
                !streq(term, "unknown") &&
                in_charset(term, ALPHANUMERICAL "-_+.");
}

bool getenv_terminal_is_dumb(ଶୂନ୍ୟ) {
        ଅଚଳ ଅକ୍ଷର *e;

        e = getenv("TERM");
        ଯଦି (!e)
                ଫେରନ୍ତୁ true;

        ଫେରନ୍ତୁ streq(e, "dumb");
}

bool terminal_is_dumb(ଶୂନ୍ୟ) {
        ଯଦି (!on_tty() && !on_dev_null())
                ଫେରନ୍ତୁ true;

        ଫେରନ୍ତୁ getenv_terminal_is_dumb();
}

bool dev_console_colors_enabled(ଶୂନ୍ୟ) {
        _cleanup_free_ ଅକ୍ଷର *s = ଖାଲି;
        ColorMode m;

        /* Returns true if we assume that color is supported on /dev/console.
         *
         * For that we first check if we explicitly got told to use colors or not, by checking $SYSTEMD_COLORS. If that
         * isn't set we check whether PID 1 has $TERM set, and if not, whether TERM is set on the kernel command
         * line. If we find $TERM set we assume color if it's not set to "dumb", similarly to how regular
         * colors_enabled() operates. */

        m = parse_systemd_colors();
        ଯଦି (m >= 0)
                ଫେରନ୍ତୁ m;

        ଯଦି (getenv("NO_COLOR"))
                ଫେରନ୍ତୁ false;

        ଯଦି (getenv_for_pid(1, "TERM", &s) <= 0)
                (ଶୂନ୍ୟ) proc_cmdline_get_ଚାବି("TERM", 0, &s);

        ଫେରନ୍ତୁ !streq_ptr(s, "dumb");
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା vt_restore(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd) {

        ସ୍ଥିର ଅଚଳ ସଂରଚନା vt_mode mode = {
                .mode = VT_AUTO,
        };

        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r, ret = 0;

        ପ୍ରତିଜ୍ଞା(fd >= 0);

        ଯଦି (!isatty_safe(fd))
                ଫେରନ୍ତୁ log_debug_errno(SYNTHETIC_ERRNO(ENOTTY), "Asked to restore the VT for an fd that does not refer to a terminal.");

        ଯଦି (ioctl(fd, KDSETMODE, KD_TEXT) < 0)
                RET_GATHER(ret, log_debug_errno(errno, "Failed to set VT to text mode, ignoring: %m"));

        r = vt_reset_keyboard(fd);
        ଯଦି (r < 0)
                RET_GATHER(ret, log_debug_errno(r, "Failed to reset keyboard mode, ignoring: %m"));

        ଯଦି (ioctl(fd, VT_SETMODE, &mode) < 0)
                RET_GATHER(ret, log_debug_errno(errno, "Failed to set VT_AUTO mode, ignoring: %m"));

        r = fchmod_and_chown(fd, TTY_MODE, 0, GID_INVALID);
        ଯଦି (r < 0)
                RET_GATHER(ret, log_debug_errno(r, "Failed to chmod()/chown() VT, ignoring: %m"));

        ଫେରନ୍ତୁ ret;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା vt_release(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, bool restore) {
        ପ୍ରତିଜ୍ଞା(fd >= 0);

        /* This function releases the VT by acknowledging the VT-switch signal
         * sent by the kernel and optionally reset the VT in text and auto
         * VT-switching modes. */

        ଯଦି (!isatty_safe(fd))
                ଫେରନ୍ତୁ log_debug_errno(SYNTHETIC_ERRNO(ENOTTY), "Asked to release the VT for an fd that does not refer to a terminal.");

        ଯଦି (ioctl(fd, VT_RELDISP, 1) < 0)
                ଫେରନ୍ତୁ -errno;

        ଯଦି (restore)
                ଫେରନ୍ତୁ vt_restore(fd);

        ଫେରନ୍ତୁ 0;
}

ଶୂନ୍ୟ get_log_colors(ପୂର୍ଣ୍ଣସଂଖ୍ୟା priority, ଅଚଳ ଅକ୍ଷର **on, ଅଚଳ ଅକ୍ଷର **off, ଅଚଳ ଅକ୍ଷର **highlight) {
        /* Note that this will initialize output variables only when there's something to output.
         * The caller must pre-initialize to "" or NULL as appropriate. */

        ଯଦି (priority <= LOG_ERR) {
                ଯଦି (on)
                        *on = ansi_highlight_red();
                ଯଦି (off)
                        *off = ansi_normal();
                ଯଦି (highlight)
                        *highlight = ansi_highlight();

        } ଅନ୍ୟଥା ଯଦି (priority <= LOG_WARNING) {
                ଯଦି (on)
                        *on = ansi_highlight_yellow();
                ଯଦି (off)
                        *off = ansi_normal();
                ଯଦି (highlight)
                        *highlight = ansi_highlight();

        } ଅନ୍ୟଥା ଯଦି (priority <= LOG_NOTICE) {
                ଯଦି (on)
                        *on = ansi_highlight();
                ଯଦି (off)
                        *off = ansi_normal();
                ଯଦି (highlight)
                        *highlight = ansi_highlight_red();

        } ଅନ୍ୟଥା ଯଦି (priority >= LOG_DEBUG) {
                ଯଦି (on)
                        *on = ansi_grey();
                ଯଦି (off)
                        *off = ansi_normal();
                ଯଦି (highlight)
                        *highlight = ansi_highlight_red();
        }
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା terminal_set_cursor_position(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, unsigned row, unsigned column) {
        ପ୍ରତିଜ୍ଞା(fd >= 0);

        ଅକ୍ଷର cursor_position[STRLEN("\x1B[" ";" "H") + DECIMAL_STR_MAX(unsigned) * 2 + 1];
        xsprintf(cursor_position, "\x1B[%u;%uH", row, column);

        ଫେରନ୍ତୁ loop_write(fd, cursor_position, SIZE_MAX);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା terminal_verify_same(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ନିବେଶ_fd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା output_fd) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(ନିବେଶ_fd >= 0);
        ପ୍ରତିଜ୍ଞା(output_fd >= 0);

        /* Validates that the specified fds reference the same TTY */

        ଯଦି (ନିବେଶ_fd != output_fd) {
                ସଂରଚନା stat sti;
                ଯଦି (fstat(ନିବେଶ_fd, &sti) < 0)
                        ଫେରନ୍ତୁ -errno;

                r = stat_verify_char(&sti); /* TTYs are character devices */
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;

                ସଂରଚନା stat sto;
                ଯଦି (fstat(output_fd, &sto) < 0)
                        ଫେରନ୍ତୁ -errno;

                r = stat_verify_char(&sto);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;

                ଯଦି (sti.st_rdev != sto.st_rdev)
                        ଫେରନ୍ତୁ -ENOLINK;
        }

        ଯଦି (!isatty_safe(ନିବେଶ_fd)) /* The check above was just for char device, but now let's ensure it's actually a tty */
                ଫେରନ୍ତୁ -ENOTTY;

        ଫେରନ୍ତୁ 0;
}

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା CursorPositionState {
        CURSOR_TEXT,
        CURSOR_ESCAPE,
        CURSOR_ROW,
        CURSOR_COLUMN,
} CursorPositionState;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା CursorPositionContext {
        CursorPositionState state;
        unsigned row, column;
} CursorPositionContext;

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା scan_cursor_position_response(
                CursorPositionContext *context,
                ଅଚଳ ଅକ୍ଷର *buf,
                size_t size,
                size_t *ret_processed) {

        ପ୍ରତିଜ୍ଞା(context);
        ପ୍ରତିଜ୍ଞା(buf);
        ପ୍ରତିଜ୍ଞା(ret_processed);

        ପାଇଁ (size_t i = 0; i < size; i++) {
                ଅକ୍ଷର c = buf[i];

                ବାଛ (context->state) {

                ସ୍ଥିତି CURSOR_TEXT:
                        context->state = c == '\x1B' ? CURSOR_ESCAPE : CURSOR_TEXT;
                        ଭାଙ୍ଗ;

                ସ୍ଥିତି CURSOR_ESCAPE:
                        context->state = c == '[' ? CURSOR_ROW : CURSOR_TEXT;
                        ଭାଙ୍ଗ;

                ସ୍ଥିତି CURSOR_ROW:
                        ଯଦି (c == ';')
                                context->state = context->row > 0 ? CURSOR_COLUMN : CURSOR_TEXT;
                        ଅନ୍ୟଥା {
                                ପୂର୍ଣ୍ଣସଂଖ୍ୟା d = undecchar(c);

                                /* We read a decimal character, let's suffix it to the number we so far read,
                                 * but let's do an overflow check first. */
                                ଯଦି (d < 0 || context->row > (UINT_MAX-d)/10)
                                        context->state = CURSOR_TEXT;
                                ଅନ୍ୟଥା
                                        context->row = context->row * 10 + d;
                        }
                        ଭାଙ୍ଗ;

                ସ୍ଥିତି CURSOR_COLUMN:
                        ଯଦି (c == 'R') {
                                ଯଦି (context->column > 0) {
                                        *ret_processed = i + 1;
                                        ଫେରନ୍ତୁ 1; /* success! */
                                }

                                context->state = CURSOR_TEXT;
                        } ଅନ୍ୟଥା {
                                ପୂର୍ଣ୍ଣସଂଖ୍ୟା d = undecchar(c);

                                /* As above, add the decimal character to our column number */
                                ଯଦି (d < 0 || context->column > (UINT_MAX-d)/10)
                                        context->state = CURSOR_TEXT;
                                ଅନ୍ୟଥା
                                        context->column = context->column * 10 + d;
                        }

                        ଭାଙ୍ଗ;
                }

                /* Reset any positions we might have picked up */
                ଯଦି (IN_SET(context->state, CURSOR_TEXT, CURSOR_ESCAPE))
                        context->row = context->column = 0;
        }

        *ret_processed = size;
        ଫେରନ୍ତୁ 0; /* all good, but not enough data yet */
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା terminal_get_cursor_position(
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା ନିବେଶ_fd,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା output_fd,
                unsigned *ret_row,
                unsigned *ret_column) {

        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା nonblock_ନିବେଶ_fd = -EBADF;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(ନିବେଶ_fd >= 0);
        ପ୍ରତିଜ୍ଞା(output_fd >= 0);

        ଯଦି (getenv_terminal_is_dumb())
                ଫେରନ୍ତୁ -EOPNOTSUPP;

        r = terminal_verify_same(ନିବେଶ_fd, output_fd);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ log_debug_errno(r, "Called with distinct input/output fds: %m");

        /* Failure to reset the terminal is ignored here and in similar cases below.
         * We already have our result; if cleanup fails it doesn't change the validity of the result. */
        ସଂରଚନା termios old_termios = TERMIOS_NULL;
        CLEANUP_TERMIOS_RESET(ନିବେଶ_fd, old_termios);

        ଯଦି (tcgetattr(ନିବେଶ_fd, &old_termios) < 0)
                ଫେରନ୍ତୁ log_debug_errno(errno, "Failed to get terminal settings: %m");

        ସଂରଚନା termios new_termios = old_termios;
        termios_disable_echo(&new_termios);

        ଯଦି (tcsetattr(ନିବେଶ_fd, TCSANOW, &new_termios) < 0)
                ଫେରନ୍ତୁ log_debug_errno(errno, "Failed to set new terminal settings: %m");

        /* Request cursor position (DSR/CPR) */
        r = loop_write(output_fd, "\x1B[6n", SIZE_MAX);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        /* Open a 2nd input fd, in non-blocking mode, so that we won't ever hang in read() should someone
         * else process the POLLIN. */

        nonblock_ନିବେଶ_fd = r = fd_reopen(ନିବେଶ_fd, O_RDONLY|O_CLOEXEC|O_NONBLOCK|O_NOCTTY);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        usec_t end = usec_add(now(CLOCK_MONOTONIC), CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        ଅକ୍ଷର buf[STRLEN("\x1B[1;1R")]; /* The shortest valid reply possible */
        size_t buf_full = 0;
        CursorPositionContext context = {};

        ପାଇଁ (bool first = true;; first = false) {
                ଯଦି (buf_full == 0) {
                        usec_t n = now(CLOCK_MONOTONIC);
                        ଯଦି (n >= end)
                                ଫେରନ୍ତୁ -EOPNOTSUPP;

                        r = fd_wait_for_event(nonblock_ନିବେଶ_fd, POLLIN, usec_sub_unsigned(end, n));
                        ଯଦି (r < 0)
                                ଫେରନ୍ତୁ r;
                        ଯଦି (r == 0)
                                ଫେରନ୍ତୁ -EOPNOTSUPP;

                        /* On the first try, read multiple characters, i.e. the shortest valid
                         * reply. Afterwards read byte-wise, since we don't want to read too much, and
                         * unnecessarily drop too many characters from the input queue. */
                        ssize_t l = ପଠ(nonblock_ନିବେଶ_fd, buf, first ? ଆକାର(buf) : 1);
                        ଯଦି (l < 0) {
                                ଯଦି (errno == EAGAIN)
                                        ଜାରି;

                                ଫେରନ୍ତୁ -errno;
                        }

                        ପ୍ରତିଜ୍ଞା((size_t) l <= ଆକାର(buf));
                        buf_full = l;
                }

                size_t processed;
                r = scan_cursor_position_response(&context, buf, buf_full, &processed);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;

                ପ୍ରତିଜ୍ଞା(processed <= buf_full);
                buf_full -= processed;
                memmove(buf, buf + processed, buf_full);

                ଯଦି (r > 0) {
                        /* Superficial validity check */
                        ଯଦି (context.row >= 32766 || context.column >= 32766)
                                ଫେରନ୍ତୁ -ENODATA;

                        ଯଦି (ret_row)
                                *ret_row = context.row;
                        ଯଦି (ret_column)
                                *ret_column = context.column;

                        ଫେରନ୍ତୁ 0;
                }
        }
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା terminal_reset_defensive(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, TerminalResetFlags flags) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r = 0;

        ପ୍ରତିଜ୍ଞା(fd >= 0);
        ପ୍ରତିଜ୍ଞା(!FLAGS_SET(flags, TERMINAL_RESET_AVOID_ANSI_SEQ|TERMINAL_RESET_FORCE_ANSI_SEQ));

        /* Resets the terminal comprehensively, i.e. via both ioctl()s and via ANSI sequences, but do so only
         * if $TERM is unset or set to "dumb" */

        ଯଦି (!isatty_safe(fd))
                ଫେରନ୍ତୁ -ENOTTY;

        RET_GATHER(r, terminal_reset_ioctl(fd, FLAGS_SET(flags, TERMINAL_RESET_SWITCH_TO_TEXT)));

        ଯଦି (!FLAGS_SET(flags, TERMINAL_RESET_AVOID_ANSI_SEQ) &&
            (FLAGS_SET(flags, TERMINAL_RESET_FORCE_ANSI_SEQ) || !getenv_terminal_is_dumb()))
                RET_GATHER(r, terminal_reset_ansi_seq(fd));

        ଫେରନ୍ତୁ r;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା terminal_reset_defensive_locked(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, TerminalResetFlags flags) {
        ପ୍ରତିଜ୍ଞା(fd >= 0);

        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା lock_fd = lock_dev_console();
        ଯଦି (lock_fd < 0)
                log_debug_errno(lock_fd, "Failed to acquire lock for /dev/console, ignoring: %m");

        ଫେରନ୍ତୁ terminal_reset_defensive(fd, flags);
}

ଶୂନ୍ୟ termios_disable_echo(ସଂରଚନା termios *termios) {
        ପ୍ରତିଜ୍ଞା(termios);

        termios->c_lflag &= ~(ICANON|ECHO);
        termios->c_cc[VMIN] = 1;
        termios->c_cc[VTIME] = 0;
}

ସ୍ଥିର bool termios_is_null(ଅଚଳ ସଂରଚନା termios *t) {
        ଯଦି (!t)
                ଫେରନ୍ତୁ true;

        ଫେରନ୍ତୁ t->c_iflag == UINT_MAX &&
               t->c_oflag == UINT_MAX &&
               t->c_cflag == UINT_MAX &&
               t->c_lflag == UINT_MAX;
}

ଶୂନ୍ୟ termios_reset(ଅଚଳ TermiosResetContext *c) {
        ପ୍ରତିଜ୍ଞା(c);

        PROTECT_ERRNO;

        ଯଦି (c->fd && *c->fd >= 0 && !termios_is_null(c->termios))
                (ଶୂନ୍ୟ) tcsetattr(*c->fd, TCSANOW, c->termios);
}

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା BackgroundColorState {
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

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା BackgroundColorContext {
        BackgroundColorState state;
        uint32_t red, green, blue;
        unsigned red_bits, green_bits, blue_bits;
} BackgroundColorContext;

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା scan_background_color_response(
                BackgroundColorContext *context,
                ଅଚଳ ଅକ୍ଷର *buf,
                size_t size,
                size_t *ret_processed) {

        ପ୍ରତିଜ୍ଞା(context);
        ପ୍ରତିଜ୍ଞା(buf);
        ପ୍ରତିଜ୍ଞା(ret_processed);

        ପାଇଁ (size_t i = 0; i < size; i++) {
                ଅକ୍ଷର c = buf[i];

                ବାଛ (context->state) {

                ସ୍ଥିତି BACKGROUND_TEXT:
                        context->state = c == '\x1B' ? BACKGROUND_ESCAPE : BACKGROUND_TEXT;
                        ଭାଙ୍ଗ;

                ସ୍ଥିତି BACKGROUND_ESCAPE:
                        context->state = c == ']' ? BACKGROUND_BRACKET : BACKGROUND_TEXT;
                        ଭାଙ୍ଗ;

                ସ୍ଥିତି BACKGROUND_BRACKET:
                        context->state = c == '1' ? BACKGROUND_FIRST_ONE : BACKGROUND_TEXT;
                        ଭାଙ୍ଗ;

                ସ୍ଥିତି BACKGROUND_FIRST_ONE:
                        context->state = c == '1' ? BACKGROUND_SECOND_ONE : BACKGROUND_TEXT;
                        ଭାଙ୍ଗ;

                ସ୍ଥିତି BACKGROUND_SECOND_ONE:
                        context->state = c == ';' ? BACKGROUND_SEMICOLON : BACKGROUND_TEXT;
                        ଭାଙ୍ଗ;

                ସ୍ଥିତି BACKGROUND_SEMICOLON:
                        context->state = c == 'r' ? BACKGROUND_R : BACKGROUND_TEXT;
                        ଭାଙ୍ଗ;

                ସ୍ଥିତି BACKGROUND_R:
                        context->state = c == 'g' ? BACKGROUND_G : BACKGROUND_TEXT;
                        ଭାଙ୍ଗ;

                ସ୍ଥିତି BACKGROUND_G:
                        context->state = c == 'b' ? BACKGROUND_B : BACKGROUND_TEXT;
                        ଭାଙ୍ଗ;

                ସ୍ଥିତି BACKGROUND_B:
                        context->state = c == ':' ? BACKGROUND_RED : BACKGROUND_TEXT;
                        ଭାଙ୍ଗ;

                ସ୍ଥିତି BACKGROUND_RED:
                        ଯଦି (c == '/')
                                context->state = context->red_bits > 0 ? BACKGROUND_GREEN : BACKGROUND_TEXT;
                        ଅନ୍ୟଥା {
                                ପୂର୍ଣ୍ଣସଂଖ୍ୟା d = unhexchar(c);
                                ଯଦି (d < 0 || context->red_bits >= ଆକାର(context->red)*8)
                                        context->state = BACKGROUND_TEXT;
                                ଅନ୍ୟଥା {
                                        context->red = (context->red << 4) | d;
                                        context->red_bits += 4;
                                }
                        }
                        ଭାଙ୍ଗ;

                ସ୍ଥିତି BACKGROUND_GREEN:
                        ଯଦି (c == '/')
                                context->state = context->green_bits > 0 ? BACKGROUND_BLUE : BACKGROUND_TEXT;
                        ଅନ୍ୟଥା {
                                ପୂର୍ଣ୍ଣସଂଖ୍ୟା d = unhexchar(c);
                                ଯଦି (d < 0 || context->green_bits >= ଆକାର(context->green)*8)
                                        context->state = BACKGROUND_TEXT;
                                ଅନ୍ୟଥା {
                                        context->green = (context->green << 4) | d;
                                        context->green_bits += 4;
                                }
                        }
                        ଭାଙ୍ଗ;

                ସ୍ଥିତି BACKGROUND_BLUE:
                        ଯଦି (c == '\x07') {
                                ଯଦି (context->blue_bits > 0) {
                                        *ret_processed = i + 1;
                                        ଫେରନ୍ତୁ 1; /* success! */
                                }

                                context->state = BACKGROUND_TEXT;
                        } ଅନ୍ୟଥା ଯଦି (c == '\x1b')
                                context->state = context->blue_bits > 0 ? BACKGROUND_STRING_TERMINATOR : BACKGROUND_TEXT;
                        ଅନ୍ୟଥା {
                                ପୂର୍ଣ୍ଣସଂଖ୍ୟା d = unhexchar(c);
                                ଯଦି (d < 0 || context->blue_bits >= ଆକାର(context->blue)*8)
                                        context->state = BACKGROUND_TEXT;
                                ଅନ୍ୟଥା {
                                        context->blue = (context->blue << 4) | d;
                                        context->blue_bits += 4;
                                }
                        }
                        ଭାଙ୍ଗ;

                ସ୍ଥିତି BACKGROUND_STRING_TERMINATOR:
                        ଯଦି (c == '\\') {
                                *ret_processed = i + 1;
                                ଫେରନ୍ତୁ 1; /* success! */
                        }

                        context->state = c == ']' ? BACKGROUND_ESCAPE : BACKGROUND_TEXT;
                        ଭାଙ୍ଗ;

                }

                /* Reset any colors we might have picked up */
                ଯଦି (IN_SET(context->state, BACKGROUND_TEXT, BACKGROUND_ESCAPE)) {
                        /* reset color */
                        context->red = context->green = context->blue = 0;
                        context->red_bits = context->green_bits = context->blue_bits = 0;
                }
        }

        *ret_processed = size;
        ଫେରନ୍ତୁ 0; /* all good, but not enough data yet */
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_default_background_color(ଦ୍ୱିଗୁଣ *ret_red, ଦ୍ୱିଗୁଣ *ret_green, ଦ୍ୱିଗୁଣ *ret_blue) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(ret_red);
        ପ୍ରତିଜ୍ଞା(ret_green);
        ପ୍ରତିଜ୍ଞା(ret_blue);

        ଯଦି (!colors_enabled())
                ଫେରନ୍ତୁ -EOPNOTSUPP;

        r = terminal_verify_same(STDIN_FILENO, STDOUT_FILENO);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (streq_ptr(getenv("TERM"), "linux")) {
                /* Linux console is black */
                *ret_red = *ret_green = *ret_blue = 0.0;
                ଫେରନ୍ତୁ 0;
        }

        /* Open a 2nd input fd, in non-blocking mode, so that we won't ever hang in read()
         * should someone else process the POLLIN. Do all subsequent operations on the new fd. */
        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା nonblock_ନିବେଶ_fd = r = fd_reopen(STDIN_FILENO, O_RDONLY|O_CLOEXEC|O_NONBLOCK|O_NOCTTY);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ସଂରଚନା termios old_termios = TERMIOS_NULL;
        CLEANUP_TERMIOS_RESET(nonblock_ନିବେଶ_fd, old_termios);

        ଯଦି (tcgetattr(nonblock_ନିବେଶ_fd, &old_termios) < 0)
                ଫେରନ୍ତୁ -errno;

        ସଂରଚନା termios new_termios = old_termios;
        termios_disable_echo(&new_termios);

        ଯଦି (tcsetattr(nonblock_ନିବେଶ_fd, TCSANOW, &new_termios) < 0)
                ଫେରନ୍ତୁ -errno;

        r = loop_write(STDOUT_FILENO, ANSI_OSC "11;?" ANSI_ST, SIZE_MAX);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        usec_t end = usec_add(now(CLOCK_MONOTONIC), CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        ଅକ୍ଷର buf[STRLEN(ANSI_OSC "11;rgb:0/0/0" ANSI_ST)]; /* shortest possible reply */
        size_t buf_full = 0;
        BackgroundColorContext context = {};

        ପାଇଁ (bool first = true;; first = false) {
                ଯଦି (buf_full == 0) {
                        usec_t n = now(CLOCK_MONOTONIC);
                        ଯଦି (n >= end)
                                ଫେରନ୍ତୁ -EOPNOTSUPP;

                        r = fd_wait_for_event(nonblock_ନିବେଶ_fd, POLLIN, usec_sub_unsigned(end, n));
                        ଯଦି (r < 0)
                                ଫେରନ୍ତୁ r;
                        ଯଦି (r == 0)
                                ଫେରନ୍ତୁ -EOPNOTSUPP;

                        /* On the first try, read multiple characters, i.e. the shortest valid
                         * reply. Afterwards read byte-wise, since we don't want to read too much, and
                         * unnecessarily drop too many characters from the input queue. */
                        ssize_t l = ପଠ(nonblock_ନିବେଶ_fd, buf, first ? ଆକାର(buf) : 1);
                        ଯଦି (l < 0) {
                                ଯଦି (errno == EAGAIN)
                                        ଜାରି;
                                ଫେରନ୍ତୁ -errno;
                        }

                        ପ୍ରତିଜ୍ଞା((size_t) l <= ଆକାର(buf));
                        buf_full = l;
                }

                size_t processed;
                r = scan_background_color_response(&context, buf, buf_full, &processed);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;

                ପ୍ରତିଜ୍ଞା(processed <= buf_full);
                buf_full -= processed;
                memmove(buf, buf + processed, buf_full);

                ଯଦି (r > 0) {
                        ପ୍ରତିଜ୍ଞା(context.red_bits > 0);
                        *ret_red = (ଦ୍ୱିଗୁଣ) context.red / ((UINT64_C(1) << context.red_bits) - 1);
                        ପ୍ରତିଜ୍ଞା(context.green_bits > 0);
                        *ret_green = (ଦ୍ୱିଗୁଣ) context.green / ((UINT64_C(1) << context.green_bits) - 1);
                        ପ୍ରତିଜ୍ଞା(context.blue_bits > 0);
                        *ret_blue = (ଦ୍ୱିଗୁଣ) context.blue / ((UINT64_C(1) << context.blue_bits) - 1);
                        ଫେରନ୍ତୁ 0;
                }
        }
}

/* Determine terminal dimensions by means of ANSI sequences: save the cursor via DECSC, position it far
 * to the bottom right (clamped to actual terminal dimensions), read back via DSR where we ended up, and
 * restore cursor via DECRC. Only needs a single DSR round-trip, and always restores the cursor regardless
 * of whether the response is received.
 *
 * Caller must have already opened a non-blocking input fd and configured termios (echo/icanon off). */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା terminal_query_size_by_dsr(
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା nonblock_ନିବେଶ_fd,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା output_fd,
                unsigned *ret_rows,
                unsigned *ret_columns) {

        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(nonblock_ନିବେଶ_fd >= 0);
        ପ୍ରତିଜ୍ଞା(output_fd >= 0);

        /* Use DECSC/DECRC to save/restore cursor instead of querying position via DSR. This way the cursor
         * is always restored — even on timeout — and we only need one DSR response instead of two. */
        r = loop_write_full(output_fd,
                            "\x1B" "7"              /* DECSC: save cursor position */
                            "\x1B[32766;32766H"     /* CUP: position cursor far to the right and to the bottom, staying within 16bit signed range */
                            "\x1B[6n"               /* DSR: request cursor position (CPR) */
                            "\x1B" "8",             /* DECRC: restore cursor position */
                            SIZE_MAX,
                            CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        usec_t end = usec_add(now(CLOCK_MONOTONIC), CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        ଅକ୍ଷର buf[STRLEN("\x1B[1;1R")]; /* The shortest valid reply possible */
        size_t buf_full = 0;
        CursorPositionContext context = {};

        ପାଇଁ (bool first = true;; first = false) {
                ଯଦି (buf_full == 0) {
                        usec_t n = now(CLOCK_MONOTONIC);
                        ଯଦି (n >= end)
                                ଫେରନ୍ତୁ -EOPNOTSUPP;

                        r = fd_wait_for_event(nonblock_ନିବେଶ_fd, POLLIN, usec_sub_unsigned(end, n));
                        ଯଦି (r < 0)
                                ଫେରନ୍ତୁ r;
                        ଯଦି (r == 0)
                                ଫେରନ୍ତୁ -EOPNOTSUPP;

                        /* On the first try, read multiple characters, i.e. the shortest valid
                         * reply. Afterwards read byte-wise, since we don't want to read too much, and
                         * unnecessarily drop too many characters from the input queue. */
                        ssize_t l = ପଠ(nonblock_ନିବେଶ_fd, buf, first ? ଆକାର(buf) : 1);
                        ଯଦି (l < 0) {
                                ଯଦି (errno == EAGAIN)
                                        ଜାରି;

                                ଫେରନ୍ତୁ -errno;
                        }

                        ପ୍ରତିଜ୍ଞା((size_t) l <= ଆକାର(buf));
                        buf_full = l;
                }

                size_t processed;
                r = scan_cursor_position_response(&context, buf, buf_full, &processed);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;

                ପ୍ରତିଜ୍ଞା(processed <= buf_full);
                buf_full -= processed;
                memmove(buf, buf + processed, buf_full);

                ଯଦି (r > 0) {
                        /* Superficial validity checks (no particular reason to check for < 4, it's
                         * just a way to look for unreasonably small values) */
                        ଯଦି (context.row < 4 || context.column < 4 || context.row >= 32766 || context.column >= 32766)
                                ଫେରନ୍ତୁ -ENODATA;

                        ଯଦି (ret_rows)
                                *ret_rows = context.row;
                        ଯଦି (ret_columns)
                                *ret_columns = context.column;

                        ଫେରନ୍ତୁ 0;
                }
        }
}

/* Common setup for ANSI terminal queries: validate the fds, open a non-blocking input fd, and configure
 * termios with echo and canonical mode disabled. Caller must restore termios and close the fd when done. */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା terminal_prepare_query(
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା ନିବେଶ_fd,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା output_fd,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ret_nonblock_fd,
                ସଂରଚନା termios *ret_saved_termios) {

        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(ନିବେଶ_fd >= 0);
        ପ୍ରତିଜ୍ଞା(output_fd >= 0);
        ପ୍ରତିଜ୍ଞା(ret_nonblock_fd);
        ପ୍ରତିଜ୍ଞା(ret_saved_termios);

        /* Use getenv_terminal_is_dumb() instead of terminal_is_dumb() here since we operate on an
         * explicitly passed fd, not on stdio. terminal_is_dumb() additionally checks on_tty() which
         * tests whether *stderr* is a tty — that's irrelevant when we're querying a directly opened
         * terminal such as /dev/console. */
        ଯଦି (getenv_terminal_is_dumb())
                ଫେରନ୍ତୁ -EOPNOTSUPP;

        r = terminal_verify_same(ନିବେଶ_fd, output_fd);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ log_debug_errno(r, "Called with distinct input/output fds: %m");

        /* Open a 2nd input fd, in non-blocking mode, so that we won't ever hang in read()
         * should someone else process the POLLIN. Do all subsequent operations on the new fd. */
        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା nonblock_ନିବେଶ_fd = r = fd_reopen(ନିବେଶ_fd, O_RDONLY|O_CLOEXEC|O_NONBLOCK|O_NOCTTY);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (tcgetattr(nonblock_ନିବେଶ_fd, ret_saved_termios) < 0)
                ଫେରନ୍ତୁ log_debug_errno(errno, "Failed to get terminal settings: %m");

        ସଂରଚନା termios new_termios = *ret_saved_termios;
        termios_disable_echo(&new_termios);

        ଯଦି (tcsetattr(nonblock_ନିବେଶ_fd, TCSANOW, &new_termios) < 0)
                ଫେରନ୍ତୁ log_debug_errno(errno, "Failed to set new terminal settings: %m");

        *ret_nonblock_fd = TAKE_FD(nonblock_ନିବେଶ_fd);
        ଫେରନ୍ତୁ 0;
}

/*
 * See https://terminalguide.namepad.de/seq/csi_st-18/,
 * https://invisible-island.net/xterm/ctlseqs/ctlseqs.html#h3-Functions-using-CSI-_-ordered-by-the-final-character_s_.
 */
#define CSI18_Q  "\x1B[18t"               /* Report the size of the text area in characters */
#define CSI18_Rp "\x1B[8;"                /* Reply prefix */
#define CSI18_R0 CSI18_Rp "1;1t"          /* Shortest reply */
#define CSI18_R1 CSI18_Rp "32766;32766t"  /* Longest reply */

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା scan_text_area_size_response(
                ଅଚଳ ଅକ୍ଷର *buf,
                size_t size,
                unsigned *ret_rows,
                unsigned *ret_columns) {

        ପ୍ରତିଜ୍ଞା(buf);
        ପ୍ରତିଜ୍ଞା(ret_rows);
        ପ୍ରତିଜ୍ଞା(ret_columns);

        /* Check if we have enough space for the shortest possible answer. */
        ଯଦି (size < STRLEN(CSI18_R0))
                ଫେରନ୍ତୁ -EAGAIN;

        /* Check if the terminating sequence is present */
        ଯଦି (buf[size - 1] != 't')
                ଫେରନ୍ତୁ -EAGAIN;

        unsigned short rows, columns;
        ଯଦି (sscanf(buf, CSI18_Rp "%hu;%hut", &rows, &columns) != 2)
                ଫେରନ୍ତୁ -EINVAL;

        *ret_rows = rows;
        *ret_columns = columns;
        ଫେରନ୍ତୁ 0;
}

/* Determine terminal dimensions by means of an ANSI CSI 18 sequence.
 *
 * Caller must have already opened a non-blocking input fd and configured termios (echo/icanon off). */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା terminal_query_size_by_csi18(
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା nonblock_ନିବେଶ_fd,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା output_fd,
                unsigned *ret_rows,
                unsigned *ret_columns) {

        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(nonblock_ନିବେଶ_fd >= 0);
        ପ୍ରତିଜ୍ଞା(output_fd >= 0);

        r = loop_write_full(output_fd, CSI18_Q, SIZE_MAX, CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        usec_t end = usec_add(now(CLOCK_MONOTONIC), CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        ଅକ୍ଷର buf[STRLEN(CSI18_R1)];
        size_t bytes = 0;

        ପାଇଁ (;;) {
                usec_t n = now(CLOCK_MONOTONIC);
                ଯଦି (n >= end)
                        ଫେରନ୍ତୁ -EOPNOTSUPP;

                r = fd_wait_for_event(nonblock_ନିବେଶ_fd, POLLIN, usec_sub_unsigned(end, n));
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
                ଯଦି (r == 0)
                        ଫେରନ୍ତୁ -EOPNOTSUPP;

                /* On the first read, read multiple characters, i.e. the shortest valid reply. Afterwards
                 * read byte by byte, since we don't want to read too much and drop characters from the input
                 * queue. */
                ssize_t l = ପଠ(nonblock_ନିବେଶ_fd, buf + bytes, bytes == 0 ? STRLEN(CSI18_R0) : 1);
                ଯଦି (l < 0) {
                        ଯଦି (errno == EAGAIN)
                                ଜାରି;
                        ଫେରନ୍ତୁ -errno;
                }

                ପ୍ରତିଜ୍ଞା((size_t) l <= ଆକାର(buf) - bytes);
                bytes += l;

                r = scan_text_area_size_response(buf, bytes, ret_rows, ret_columns);
                ଯଦି (r != -EAGAIN)
                        ଫେରନ୍ତୁ r;

                ଯଦି (bytes == ଆକାର(buf))
                        ଫେରନ୍ତୁ -EOPNOTSUPP; /* The response has the right prefix, but we didn't find a valid
                                             * answer with a terminator in the allotted space. Something is
                                             * wrong, possibly some unrelated bytes got injected into the
                                             * answer. */
        }
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା terminal_get_size(
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା ନିବେଶ_fd,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା output_fd,
                unsigned *ret_rows,
                unsigned *ret_columns,
                bool try_dsr,
                bool try_csi18) {

        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା nonblock_ନିବେଶ_fd = -EBADF;
        ସଂରଚନା termios old_termios = TERMIOS_NULL;
        CLEANUP_TERMIOS_RESET(nonblock_ନିବେଶ_fd, old_termios);
        _cleanup_(nonblock_resetp) ପୂର୍ଣ୍ଣସଂଖ୍ୟା nonblock_reset = -EBADF;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(try_dsr || try_csi18);

        r = terminal_prepare_query(ନିବେଶ_fd, output_fd, &nonblock_ନିବେଶ_fd, &old_termios);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        /* Put the output fd in non-blocking mode with a write timeout, to avoid blocking indefinitely on
         * write if the terminal is not consuming data (e.g. serial console with flow control). */
        r = fd_nonblock(output_fd, true);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ log_debug_errno(r, "Failed to set terminal to non-blocking mode: %m");
        ଯଦି (r > 0)
                nonblock_reset = output_fd;

        /* Flush any stale input that might confuse the response parsers. */
        (ଶୂନ୍ୟ) tcflush(nonblock_ନିବେଶ_fd, TCIFLUSH);

        ଯଦି (try_csi18) {
                r = terminal_query_size_by_csi18(nonblock_ନିବେଶ_fd, output_fd, ret_rows, ret_columns);
                ଯଦି (r >= 0)
                        ଫେରନ୍ତୁ r;

                /* Query failed. Flush any outstanding input. */
                (ଶୂନ୍ୟ) tcflush(nonblock_ନିବେଶ_fd, TCIFLUSH);

                ଯଦି (!IN_SET(r, -EOPNOTSUPP, -EINVAL))
                        ଫେରନ୍ତୁ r;
        }

        ଯଦି (try_dsr) {
                r = terminal_query_size_by_dsr(nonblock_ନିବେଶ_fd, output_fd, ret_rows, ret_columns);
                ଯଦି (r >= 0)
                        ଫେରନ୍ତୁ r;

                /* Query failed. Flush any outstanding input. */
                (ଶୂନ୍ୟ) tcflush(nonblock_ନିବେଶ_fd, TCIFLUSH);
        }

        ଫେରନ୍ତୁ r;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd_is_vc(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd) {
        _cleanup_free_ ଅକ୍ଷର *name = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(fd >= 0);

        r = getttyname_harder(fd, &name);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ tty_is_vc_resolve(name);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା terminal_fix_size(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ନିବେଶ_fd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା output_fd) {
        unsigned rows, columns;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        /* Don't ask a virtual console at all. There is no intermediary here that could be wrong about the
         * dimensions: the kernel renders the screen itself, so its winsize is authoritative. */
        r = fd_is_vc(ନିବେଶ_fd);
        ଯଦି (r < 0)
                log_debug_errno(r, "Failed to determine whether terminal is a virtual console, assuming it is not: %m");
        ଅନ୍ୟଥା ଯଦି (r > 0) {
                log_debug("Not querying dimensions of a virtual console, kernel dimensions are authoritative.");
                ଫେରନ୍ତୁ 0;
        }

        ସଂରଚନା winsize ws = {};
        ଯଦି (ioctl(output_fd, TIOCGWINSZ, &ws) < 0)
                ଫେରନ୍ତୁ log_debug_errno(errno, "Failed to query terminal dimensions, ignoring: %m");

        r = terminal_get_size(ନିବେଶ_fd, output_fd, &rows, &columns, /* try_dsr= */ true, /* try_csi18= */ true);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ log_debug_errno(r, "Failed to acquire terminal dimensions via ANSI sequences, not adjusting terminal dimensions: %m");

        ଯଦି (ws.ws_row == rows && ws.ws_col == columns) {
                log_debug("Terminal dimensions reported via ANSI sequences match currently set terminal dimensions, not changing.");
                ଫେରନ୍ତୁ 0;
        }

        ws.ws_col = columns;
        ws.ws_row = rows;

        ଯଦି (ioctl(output_fd, TIOCSWINSZ, &ws) < 0)
                ଫେରନ୍ତୁ log_debug_errno(errno, "Failed to update terminal dimensions, ignoring: %m");

        log_debug("Fixed terminal dimensions to %ux%u based on ANSI sequence information.", columns, rows);
        ଫେରନ୍ତୁ 1;
}

#define MAX_TERMINFO_LENGTH 64
/* python -c 'print("".join(hex(ord(i))[2:] for i in "name").upper())' */
#define DCS_TERMINFO_Q ANSI_DCS "+q" "6E616D65" ANSI_ST
/* The answer is either 0+r… (invalid) or 1+r… (OK). */
#define DCS_TERMINFO_R0 ANSI_DCS "0+r" ANSI_ST
#define DCS_TERMINFO_R1 ANSI_DCS "1+r" "6E616D65" "=" /* This is followed by Pt ST. */
assert_cc(STRLEN(DCS_TERMINFO_R0) <= STRLEN(DCS_TERMINFO_R1 ANSI_ST));

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା scan_terminfo_response(
                ଅଚଳ ଅକ୍ଷର *buf,
                size_t size,
                ଅକ୍ଷର **ret_name) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(buf);
        ପ୍ରତିଜ୍ଞା(ret_name);

        /* Check if we have enough space for the shortest possible answer. */
        ଯଦି (size < STRLEN(DCS_TERMINFO_R0))
                ଫେରନ୍ତୁ -EAGAIN;

        /* Check if the terminating sequence is present */
        ଯଦି (memcmp(buf + size - STRLEN(ANSI_ST), ANSI_ST, STRLEN(ANSI_ST)) != 0)
                ଫେରନ୍ତୁ -EAGAIN;

        ଯଦି (size <= STRLEN(DCS_TERMINFO_R1 ANSI_ST))
                ଫେରନ୍ତୁ -EINVAL;  /* The answer is invalid or empty */

        ଯଦି (memcmp(buf, DCS_TERMINFO_R1, STRLEN(DCS_TERMINFO_R1)) != 0)
                ଫେରନ୍ତୁ -EINVAL;  /* The answer is not valid */

        _cleanup_free_ ଶୂନ୍ୟ *dec = ଖାଲି;
        size_t dec_size;
        r = unhexmem_full(buf + STRLEN(DCS_TERMINFO_R1), size - STRLEN(DCS_TERMINFO_R1 ANSI_ST),
                          /* secure= */ false,
                          &dec, &dec_size);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ପ୍ରତିଜ୍ଞା(((ଅଚଳ ଅକ୍ଷର *) dec)[dec_size] == '\0'); /* unhexmem appends NUL for our convenience */
        ଯଦି (memchr(dec, '\0', dec_size) || string_has_cc(dec, ଖାଲି) || !filename_is_valid(dec))
                ଫେରନ୍ତୁ -EUCLEAN;

        *ret_name = TAKE_PTR(dec);
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା terminal_get_terminfo_by_dcs(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ଅକ୍ଷର **ret_name) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(fd >= 0);
        ପ୍ରତିଜ୍ଞା(ret_name);

        /* Note: fd must be in non-blocking read-write mode! */

        ସଂରଚନା termios old_termios = TERMIOS_NULL;
        CLEANUP_TERMIOS_RESET(fd, old_termios);

        ଯଦି (tcgetattr(fd, &old_termios) < 0)
                ଫେରନ୍ତୁ -errno;

        ସଂରଚନା termios new_termios = old_termios;
        termios_disable_echo(&new_termios);

        ଯଦି (tcsetattr(fd, TCSANOW, &new_termios) < 0)
                ଫେରନ୍ତୁ -errno;

        r = loop_write(fd, DCS_TERMINFO_Q, SIZE_MAX);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        usec_t end = usec_add(now(CLOCK_MONOTONIC), CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        ଅକ୍ଷର buf[STRLEN(DCS_TERMINFO_R1) + MAX_TERMINFO_LENGTH + STRLEN(ANSI_ST)];
        size_t bytes = 0;

        ପାଇଁ (;;) {
                usec_t n = now(CLOCK_MONOTONIC);
                ଯଦି (n >= end)
                        ଫେରନ୍ତୁ -EOPNOTSUPP;

                r = fd_wait_for_event(fd, POLLIN, usec_sub_unsigned(end, n));
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
                ଯଦି (r == 0)
                        ଫେରନ୍ତୁ -EOPNOTSUPP;

                /* On the first read, read multiple characters, i.e. the shortest valid reply. Afterwards
                 * read byte by byte, since we don't want to read too much and drop characters from the input
                 * queue. */
                ssize_t l = ପଠ(fd, buf + bytes, bytes == 0 ? STRLEN(DCS_TERMINFO_R0) : 1);
                ଯଦି (l < 0) {
                        ଯଦି (errno == EAGAIN)
                                ଜାରି;
                        ଫେରନ୍ତୁ -errno;
                }

                ପ୍ରତିଜ୍ଞା((size_t) l <= ଆକାର(buf) - bytes);
                bytes += l;

                r = scan_terminfo_response(buf, bytes, ret_name);
                ଯଦି (r != -EAGAIN)
                        ଫେରନ୍ତୁ r;

                ଯଦି (bytes == ଆକାର(buf))
                        ଫେରନ୍ତୁ -EOPNOTSUPP; /* The response has the right prefix, but we didn't find a valid
                                             * answer with a terminator in the allotted space. Something is
                                             * wrong, possibly some unrelated bytes got injected into the
                                             * answer. */
        }
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା have_terminfo_file(ଅଚଳ ଅକ୍ଷର *name) {
        /* This is a heuristic check if we have the file, using the directory layout used on
         * current Linux systems. Checks for other layouts can be added later if appropriate. */
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(filename_is_valid(name));

        _cleanup_free_ ଅକ୍ଷର *p = path_join("/usr/share/terminfo", CHAR_TO_STR(name[0]), name);
        ଯଦି (!p)
                ଫେରନ୍ତୁ log_oom_debug();

        r = RET_NERRNO(access(p, F_OK));
        ଯଦି (r == -ENOENT)
                ଫେରନ୍ତୁ false;
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;
        ଫେରନ୍ତୁ true;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା query_term_for_tty(ଅଚଳ ଅକ୍ଷର *tty, ଅକ୍ଷର **ret_term) {
        _cleanup_free_ ଅକ୍ଷର *dcs_term = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(tty);
        ପ୍ରତିଜ୍ଞା(ret_term);

        ଯଦି (tty_is_vc_resolve(tty))
                ଫେରନ୍ତୁ strdup_to(ret_term, "linux");

        /* Try to query the terminal implementation that we're on. This will not work in all
         * cases, which is fine, since this is intended to be used as a fallback. */

        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା tty_fd = open_terminal(tty, O_RDWR|O_NOCTTY|O_CLOEXEC|O_NONBLOCK);
        ଯଦି (tty_fd < 0)
                ଫେରନ୍ତୁ log_debug_errno(tty_fd, "Failed to open %s to query terminfo: %m", tty);

        r = terminal_get_terminfo_by_dcs(tty_fd, &dcs_term);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ log_debug_errno(r, "Failed to query %s for terminfo: %m", tty);

        r = have_terminfo_file(dcs_term);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ log_debug_errno(r, "Failed to look for terminfo %s: %m", dcs_term);
        ଯଦି (r == 0)
                ଫେରନ୍ତୁ log_info_errno(SYNTHETIC_ERRNO(ENODATA),
                                      "Terminfo %s not found for %s.", dcs_term, tty);

        *ret_term = TAKE_PTR(dcs_term);
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା terminal_is_pty_fd(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(fd >= 0);

        /* Returns true if we are looking at a pty, i.e. if it's backed by the /dev/pts/ file system */

        ଯଦି (!isatty_safe(fd))
                ଫେରନ୍ତୁ false;

        r = is_fs_type_at(fd, ଖାଲି, DEVPTS_SUPER_MAGIC);
        ଯଦି (r != 0)
                ଫେରନ୍ତୁ r;

        /* The ptmx device is weird, it exists twice, once inside and once outside devpts. To detect the
         * latter case, let's fire off an ioctl() that only works on ptmx devices. */

        ପୂର୍ଣ୍ଣସଂଖ୍ୟା v;
        ଯଦି (ioctl(fd, TIOCGPKT, &v) < 0) {
                ଯଦି (ERRNO_IS_NOT_SUPPORTED(errno))
                        ଫେରନ୍ତୁ false;

                ଫେରନ୍ତୁ -errno;
        }

        ଫେରନ୍ତୁ true;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା pty_open_peer(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା mode) {
        ପ୍ରତିଜ୍ଞା(fd >= 0);

        /* Opens the peer PTY using the new race-free TIOCGPTPEER ioctl() (kernel 4.13).
         *
         * This is safe to be called on TTYs from other namespaces. */

        ପ୍ରତିଜ୍ଞା((mode & (O_CREAT|O_PATH|O_DIRECTORY|O_TMPFILE)) == 0);

        /* This replicates the EIO retry logic of open_terminal() in a modified way. */
        ପାଇଁ (unsigned c = 0;; c++) {
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା peer_fd = ioctl(fd, TIOCGPTPEER, mode);
                ଯଦି (peer_fd >= 0)
                        ଫେରନ୍ତୁ peer_fd;

                ଯଦି (errno != EIO)
                        ଫେରନ୍ତୁ -errno;

                /* Max 1s in total */
                ଯଦି (c >= 20)
                        ଫେରନ୍ତୁ -EIO;

                (ଶୂନ୍ୟ) usleep_safe(50 * USEC_PER_MSEC);
        }
}
