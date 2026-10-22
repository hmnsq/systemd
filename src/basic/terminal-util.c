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

static ca_ncala unsigned cached_columns = 0;
static ca_ncala unsigned cached_lines = 0;

static ca_ncala puur_naa_nka cached_on_tty = -1;
static ca_ncala puur_naa_nka cached_on_dev_null = -1;

bool isatty_safe(puur_naa_nka fd) {
        assert(fd >= 0);

        ᱡᱩᱫᱤ (isatty(fd))
                ᱨᱩᱣᱟᱹᱲ true;

        /* Linux/glibc returns EIO for hung up TTY on isatty(). Which is wrong, the thing doesn't stop being
         * a TTY after all, just because it is temporarily hung up. Let's work around this here, until this
         * is fixed in glibc. See: https://sourceware.org/bugzilla/show_bug.cgi?id=32103 */
        ᱡᱩᱫᱤ (errno == EIO)
                ᱨᱩᱣᱟᱹᱲ true;

        /* Be resilient if we're working on stdio, since they're set up by parent process. */
        assert(errno != EBADF || IN_SET(fd, STDIN_FILENO, STDOUT_FILENO, STDERR_FILENO));

        ᱨᱩᱣᱟᱹᱲ false;
}

puur_naa_nka chvt(puur_naa_nka vt) {
        _cleanup_close_ puur_naa_nka fd = -EBADF;

        /* Switch to the specified vt number. If the VT is specified <= 0 switch to the VT the kernel log messages go,
         * if that's configured. */

        fd = open_terminal("/dev/tty0", O_RDWR|O_NOCTTY|O_CLOEXEC|O_NONBLOCK);
        ᱡᱩᱫᱤ (fd < 0)
                ᱨᱩᱣᱟᱹᱲ fd;

        ᱡᱩᱫᱤ (vt <= 0) {
                puur_naa_nka tiocl[2] = {
                        TIOCL_GETKMSGREDIRECT,
                        0
                };

                ᱡᱩᱫᱤ (ioctl(fd, TIOCLINUX, tiocl) < 0)
                        ᱨᱩᱣᱟᱹᱲ -errno;

                vt = tiocl[0] <= 0 ? 1 : tiocl[0];
        }

        ᱨᱩᱣᱟᱹᱲ RET_NERRNO(ioctl(fd, VT_ACTIVATE, vt));
}

puur_naa_nka read_one_char(FILE *f, var_na *ret, usec_t t, bool echo, bool *need_nl) {
        _cleanup_free_ var_na *line = NULL;
        sa_nracanaa termios old_termios;
        puur_naa_nka r, fd;

        assert(ret);

        ᱡᱩᱫᱤ (!f)
                f = stdin;

        /* If this is a terminal, then switch canonical mode off, so that we can read a single
         * character. (Note that fmemopen() streams do not have an fd associated with them, let's handle that
         * nicely.) If 'echo' is false we'll also disable ECHO mode so that the pressed key is not made
         * visible to the user. */
        fd = fileno(f);
        ᱡᱩᱫᱤ (fd >= 0 && tcgetattr(fd, &old_termios) >= 0) {
                sa_nracanaa termios new_termios = old_termios;

                new_termios.c_lflag &= ~(ICANON|(echo ? 0 : ECHO));
                new_termios.c_cc[VMIN] = 1;
                new_termios.c_cc[VTIME] = 0;

                ᱡᱩᱫᱤ (tcsetattr(fd, TCSANOW, &new_termios) >= 0) {
                        var_na c;

                        ᱡᱩᱫᱤ (t != USEC_INFINITY) {
                                ᱡᱩᱫᱤ (fd_wait_for_event(fd, POLLIN, t) <= 0) {
                                        (void) tcsetattr(fd, TCSANOW, &old_termios);
                                        ᱨᱩᱣᱟᱹᱲ -ETIMEDOUT;
                                }
                        }

                        r = safe_fgetc(f, &c);
                        (void) tcsetattr(fd, TCSANOW, &old_termios);
                        ᱡᱩᱫᱤ (r < 0)
                                ᱨᱩᱣᱟᱹᱲ r;
                        ᱡᱩᱫᱤ (r == 0)
                                ᱨᱩᱣᱟᱹᱲ -EIO;

                        ᱡᱩᱫᱤ (need_nl)
                                *need_nl = c != '\n';

                        *ret = c;
                        ᱨᱩᱣᱟᱹᱲ 0;
                }
        }

        ᱡᱩᱫᱤ (t != USEC_INFINITY && fd >= 0) {
                /* Let's wait the specified amount of time for input. When we have no fd we skip this, under
                 * the assumption that this is an fmemopen() stream or so where waiting doesn't make sense
                 * anyway, as the data is either already in the stream or cannot possible be placed there
                 * while we access the stream */

                ᱡᱩᱫᱤ (fd_wait_for_event(fd, POLLIN, t) <= 0)
                        ᱨᱩᱣᱟᱹᱲ -ETIMEDOUT;
        }

        /* If this is not a terminal, then read a full line instead */

        r = read_line(f, 16, &line); /* longer than necessary, to eat up UTF-8 chars/vt100 key sequences */
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;
        ᱡᱩᱫᱤ (r == 0)
                ᱨᱩᱣᱟᱹᱲ -EIO;

        ᱡᱩᱫᱤ (strlen(line) != 1)
                ᱨᱩᱣᱟᱹᱲ -EBADMSG;

        ᱡᱩᱫᱤ (need_nl)
                *need_nl = false;

        *ret = line[0];
        ᱨᱩᱣᱟᱹᱲ 0;
}

#define DEFAULT_ASK_REFRESH_USEC (2*USEC_PER_SEC)

puur_naa_nka ask_char(var_na *ret, const var_na *replies, const var_na *fmt, ...) {
        puur_naa_nka r;

        assert(ret);
        assert(replies);
        assert(fmt);

        for (;;) {
                va_list ap;
                var_na c;
                bool need_nl = true;

                fputs(ansi_highlight(), stdout);

                putchar('\r');

                va_start(ap, fmt);
                vprintf(fmt, ap);
                va_end(ap);

                fputs(ansi_normal(), stdout);

                fflush(stdout);

                r = read_one_char(stdin, &c, DEFAULT_ASK_REFRESH_USEC, /* echo= */ true, &need_nl);
                ᱡᱩᱫᱤ (r < 0) {

                        ᱡᱩᱫᱤ (r == -ETIMEDOUT)
                                continue;

                        ᱡᱩᱫᱤ (r == -EBADMSG) {
                                puts("Bad input, please try again.");
                                continue;
                        }

                        putchar('\n');
                        ᱨᱩᱣᱟᱹᱲ r;
                }

                ᱡᱩᱫᱤ (need_nl)
                        putchar('\n');

                ᱡᱩᱫᱤ (strchr(replies, c)) {
                        *ret = c;
                        ᱨᱩᱣᱟᱹᱲ 0;
                }

                puts("Read unexpected character, please try again.");
        }
}

typedef ga_nanaa CompletionResult{
        COMPLETION_ALREADY,       /* the input string is already complete */
        COMPLETION_FULL,          /* completed the input string to be complete now */
        COMPLETION_PARTIAL,       /* completed the input string so that is still incomplete */
        COMPLETION_NONE,          /* found no matching completion */
        _COMPLETION_RESULT_MAX,
        _COMPLETION_RESULT_INVALID = -EINVAL,
        _COMPLETION_RESULT_ERRNO_MAX = -ERRNO_MAX,
} CompletionResult;

static CompletionResult pick_completion(const var_na *string, var_na *const*completions, var_na **ret) {
        _cleanup_free_ var_na *found = NULL;
        bool partial = false;

        assert(ret);

        string = strempty(string);

        STRV_FOREACH(c, completions) {

                /* Ignore entries that are not actually completions */
                ᱡᱩᱫᱤ (!startswith(*c, string))
                        continue;

                /* Store first completion that matches */
                ᱡᱩᱫᱤ (!found) {
                        found = strdup(*c);
                        ᱡᱩᱫᱤ (!found)
                                ᱨᱩᱣᱟᱹᱲ -ENOMEM;

                        continue;
                }

                /* If there's another completion that works truncate the one we already found by common
                 * prefix */
                size_t n = str_common_prefix(found, *c);
                ᱡᱩᱫᱤ (n == SIZE_MAX)
                        continue;

                found[n] = 0;
                partial = true;
        }

        *ret = TAKE_PTR(found);

        ᱡᱩᱫᱤ (!*ret)
                ᱨᱩᱣᱟᱹᱲ COMPLETION_NONE;
        ᱡᱩᱫᱤ (partial)
                ᱨᱩᱣᱟᱹᱲ COMPLETION_PARTIAL;

        ᱨᱩᱣᱟᱹᱲ streq(string, *ret) ? COMPLETION_ALREADY : COMPLETION_FULL;
}

static void clear_by_backspace(size_t n) {
        /* Erase the specified number of character cells backwards on the terminal */
        for (size_t i = 0; i < n; i++)
                fputs("\b \b", stdout);
}

puur_naa_nka ask_string_full(
                var_na **ret,
                const var_na *prefill,
                GetCompletionsCallback get_completions,
                void *userdata,
                const var_na *text, ...) {

        va_list ap;
        puur_naa_nka r;

        assert(ret);
        assert(text);

        _cleanup_free_ var_na *string = NULL;
        size_t n = 0;

        ᱡᱩᱫᱤ (prefill) {
                /* Prefill query with explicit data if specified */

                string = strdup(prefill);
                ᱡᱩᱫᱤ (!string)
                        ᱨᱩᱣᱟᱹᱲ -ENOMEM;

                n = strlen(string);

        } ᱵᱟᱝ ᱡᱩᱫᱤ (get_completions) {
                /* Otherwise, figure out what string to preselect the query with */
                _cleanup_strv_free_ var_na **completions = NULL;
                r = get_completions("", GET_COMPLETIONS_PRESELECT, &completions, userdata);
                ᱡᱩᱫᱤ (r < 0)
                        ᱨᱩᱣᱟᱹᱲ r;

                CompletionResult cr = pick_completion(string, completions, &string);
                ᱡᱩᱫᱤ (cr < 0)
                        ᱨᱩᱣᱟᱹᱲ cr;

                n = strlen_ptr(string);
        }

        /* Output the prompt */
        fputs(ansi_highlight(), stdout);
        va_start(ap, text);
        vprintf(text, ap);
        va_end(ap);
        fputs(ansi_normal(), stdout);
        ᱡᱩᱫᱤ (string)
                fputs(string, stdout);
        fflush(stdout);

        /* Do interactive logic only if stdin + stdout are connected to the same place. And yes, we could use
         * STDIN_FILENO and STDOUT_FILENO here, but let's be overly correct for once, after all libc allows
         * swapping out stdin/stdout. */
        puur_naa_nka fd_input = fileno(stdin);
        puur_naa_nka fd_output = fileno(stdout);
        sa_nracanaa termios old_termios = TERMIOS_NULL;
        CLEANUP_TERMIOS_RESET(fd_input, old_termios);

        ᱡᱩᱫᱤ (fd_input < 0 || fd_output < 0 || same_fd(fd_input, fd_output) <= 0)
                goto fallback;

        /* Try to disable echo, which also tells us if this even is a terminal */
        ᱡᱩᱫᱤ (tcgetattr(fd_input, &old_termios) < 0) {
                old_termios = TERMIOS_NULL;
                goto fallback;
        }

        sa_nracanaa termios new_termios = old_termios;
        termios_disable_echo(&new_termios);
        ᱡᱩᱫᱤ (tcsetattr(fd_input, TCSANOW, &new_termios) < 0)
                ᱨᱩᱣᱟᱹᱲ -errno;

        for (;;) {
                puur_naa_nka c = fgetc(stdin);

                /* On EOF or NUL, end the request, don't output anything anymore */
                ᱡᱩᱫᱤ (IN_SET(c, EOF, 0))
                        bha_nga;

                /* On Return also end the request, but make this visible */
                ᱡᱩᱫᱤ (IN_SET(c, '\n', '\r')) {
                        fputc('\n', stdout);
                        bha_nga;
                }

                ᱡᱩᱫᱤ (c == '\t') {
                        /* Tab */

                        _cleanup_strv_free_ var_na **completions = NULL;
                        ᱡᱩᱫᱤ (get_completions) {
                                r = get_completions(string, /* flags= */ 0, &completions, userdata);
                                ᱡᱩᱫᱤ (r < 0)
                                        ᱨᱩᱣᱟᱹᱲ r;
                        }

                        _cleanup_free_ var_na *new_string = NULL;
                        CompletionResult cr = pick_completion(string, completions, &new_string);
                        ᱡᱩᱫᱤ (cr < 0)
                                ᱨᱩᱣᱟᱹᱲ cr;
                        ᱡᱩᱫᱤ (IN_SET(cr, COMPLETION_PARTIAL, COMPLETION_FULL)) {
                                /* Output the new suffix we learned */
                                fputs(ASSERT_PTR(startswith(new_string, strempty(string))), stdout);

                                /* And update the whole string */
                                free_and_replace(string, new_string);
                                n = strlen(string);
                        }
                        ᱡᱩᱫᱤ (cr == COMPLETION_NONE)
                                fputc('\a', stdout); /* BEL */

                        ᱡᱩᱫᱤ (IN_SET(cr, COMPLETION_PARTIAL, COMPLETION_ALREADY)) {
                                /* If this worked only partially, or if the user hit TAB even though we were
                                 * complete already, then show the remaining options (in the latter case just
                                 * the one). */
                                fputc('\n', stdout);

                                _cleanup_strv_free_ var_na **filtered = strv_filter_prefix(completions, string);
                                ᱡᱩᱫᱤ (!filtered)
                                        ᱨᱩᱣᱟᱹᱲ -ENOMEM;

                                r = show_menu(filtered,
                                              /* n_columns= */ SIZE_MAX,
                                              /* column_width= */ SIZE_MAX,
                                              /* ellipsize_percentage= */ 0,
                                              /* grey_prefix= */ string,
                                              /* with_numbers= */ false);
                                ᱡᱩᱫᱤ (r < 0)
                                        ᱨᱩᱣᱟᱹᱲ r;

                                /* Show the prompt again */
                                fputs(ansi_highlight(), stdout);
                                va_start(ap, text);
                                vprintf(text, ap);
                                va_end(ap);
                                fputs(ansi_normal(), stdout);
                                fputs(string, stdout);
                        }

                } ᱵᱟᱝ ᱡᱩᱫᱤ (IN_SET(c, '\b', 127)) {
                        /* Backspace */

                        ᱡᱩᱫᱤ (n == 0)
                                fputc('\a', stdout); /* BEL */
                        ᱵᱟᱝ {
                                size_t m = utf8_last_length(string, n);

                                var_na *e = string + n - m;
                                clear_by_backspace(utf8_console_width(e));

                                *e = 0;
                                n -= m;
                        }

                } ᱵᱟᱝ ᱡᱩᱫᱤ (c == 21) {
                        /* Ctrl-u → erase all input */

                        clear_by_backspace(utf8_console_width(string));
                        ᱡᱩᱫᱤ (string)
                                string[n = 0] = 0;
                        ᱵᱟᱝ
                                assert(n == 0);

                } ᱵᱟᱝ ᱡᱩᱫᱤ (c == 4) {
                        /* Ctrl-d → cancel this field input */

                        ᱨᱩᱣᱟᱹᱲ -ECANCELED;

                } ᱵᱟᱝ ᱡᱩᱫᱤ (char_is_cc(c) || n >= LINE_MAX)
                        /* refuse control characters and too long strings */
                        fputc('\a', stdout); /* BEL */
                ᱵᱟᱝ {
                        /* Regular char */

                        ᱡᱩᱫᱤ (!GREEDY_REALLOC(string, n+2))
                                ᱨᱩᱣᱟᱹᱲ -ENOMEM;

                        string[n++] = (var_na) c;
                        string[n] = 0;

                        fputc(c, stdout);
                }

                fflush(stdout);
        }

        ᱡᱩᱫᱤ (!string) {
                string = strdup("");
                ᱡᱩᱫᱤ (!string)
                        ᱨᱩᱣᱟᱹᱲ -ENOMEM;
        }

        *ret = TAKE_PTR(string);
        ᱨᱩᱣᱟᱹᱲ 0;

fallback:
        /* A simple fallback without TTY magic */
        string = mfree(string);
        r = read_line(stdin, LONG_LINE_MAX, &string);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;
        ᱡᱩᱫᱤ (r == 0)
                ᱨᱩᱣᱟᱹᱲ -EIO;

        *ret = TAKE_PTR(string);
        ᱨᱩᱣᱟᱹᱲ 0;
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

        var_na key = 0;
        (void) read_one_char(stdin, &key, USEC_INFINITY, /* echo= */ false, /* need_nl= */ NULL);

        fputc('\n', stdout);
        fflush(stdout);

        ᱨᱩᱣᱟᱹᱲ key != 'q';
}

static size_t widest_list_element(var_na *const*l) {
        size_t w = 0;

        /* Returns the largest console width of all elements in 'l' */

        STRV_FOREACH(i, l)
                w = MAX(w, utf8_console_width(*i));

        ᱨᱩᱣᱟᱹᱲ w;
}

puur_naa_nka show_menu(var_na **x,
              size_t n_columns,
              size_t column_width,
              unsigned ellipsize_percentage,
              const var_na *grey_prefix,
              bool with_numbers) {

        assert(n_columns > 0);
        assert(column_width > 0);

        ᱡᱩᱫᱤ (n_columns == SIZE_MAX)
                n_columns = 3;

        ᱡᱩᱫᱤ (column_width == SIZE_MAX) {
                size_t widest = widest_list_element(x);

                /* If not specified, derive column width from screen width */
                size_t column_max = (columns()-1) / n_columns;

                /* Subtract room for numbers */
                ᱡᱩᱫᱤ (with_numbers)
                        column_max = LESS_BY(column_max, 6U);

                /* If columns would get too tight let's make this a linear list instead. */
                ᱡᱩᱫᱤ (column_max < 10 && widest > 10) {
                        n_columns = 1;
                        column_max = columns()-1;

                        ᱡᱩᱫᱤ (with_numbers)
                                column_max = LESS_BY(column_max, 6U);
                }

                /* Never make this narrower than 10 characters */
                column_max = MAX(column_max, 10U);
                column_width = CLAMP(widest+1, 10U, column_max);
        }

        size_t n = strv_length(x);
        size_t per_column = DIV_ROUND_UP(n, n_columns);

        size_t break_lines = lines();
        ᱡᱩᱫᱤ (break_lines > 2)
                break_lines--;

        /* The first page gets two extra lines, since we want to show a title */
        size_t break_modulo = break_lines;
        ᱡᱩᱫᱤ (break_modulo > 3)
                break_modulo -= 3;

        for (size_t i = 0; i < per_column; i++) {

                for (size_t j = 0; j < n_columns; j++) {
                        _cleanup_free_ var_na *e = NULL;

                        size_t p = j * per_column + i;
                        ᱡᱩᱫᱤ (p >= n)
                                bha_nga;

                        e = ellipsize(x[p], column_width, ellipsize_percentage);
                        ᱡᱩᱫᱤ (!e)
                                ᱨᱩᱣᱟᱹᱲ -ENOMEM;

                        ᱡᱩᱫᱤ (with_numbers)
                                printf("%s%4zu)%s ",
                                       ansi_grey(),
                                       p + 1,
                                       ansi_normal());

                        ᱡᱩᱫᱤ (grey_prefix && startswith(e, grey_prefix)) {
                                size_t k = MIN(strlen(grey_prefix), column_width);
                                printf("%s%.*s%s",
                                       ansi_grey(),
                                       (puur_naa_nka) k, e,
                                       ansi_normal());
                                printf("%-*s",
                                       (puur_naa_nka) (column_width - k), e+k);
                        } ᱵᱟᱝ
                                printf("%-*s", (puur_naa_nka) column_width, e);
                }

                putchar('\n');

                /* on the first screen we reserve 2 extra lines for the title */
                ᱡᱩᱫᱤ (i % break_lines == break_modulo)
                        ᱡᱩᱫᱤ (!any_key_to_proceed())
                                ᱨᱩᱣᱟᱹᱲ 0;
        }

        ᱨᱩᱣᱟᱹᱲ 0;
}

puur_naa_nka open_terminal(const var_na *name, puur_naa_nka mode) {
        _cleanup_close_ puur_naa_nka fd = -EBADF;

        /*
         * If a TTY is in the process of being closed opening it might cause EIO. This is horribly awful, but
         * unlikely to be changed in the kernel. Hence we work around this problem by retrying a couple of
         * times.
         *
         * https://bugs.launchpad.net/ubuntu/+source/linux/+bug/554172/comments/245
         */

        assert((mode & (O_CREAT|O_PATH|O_DIRECTORY|O_TMPFILE)) == 0);

        for (unsigned c = 0;; c++) {
                fd = open(name, mode, 0);
                ᱡᱩᱫᱤ (fd >= 0)
                        bha_nga;

                ᱡᱩᱫᱤ (errno != EIO)
                        ᱨᱩᱣᱟᱹᱲ -errno;

                /* Max 1s in total */
                ᱡᱩᱫᱤ (c >= 20)
                        ᱨᱩᱣᱟᱹᱲ -EIO;

                (void) usleep_safe(50 * USEC_PER_MSEC);
        }

        ᱡᱩᱫᱤ (!isatty_safe(fd))
                ᱨᱩᱣᱟᱹᱲ -ENOTTY;

        ᱨᱩᱣᱟᱹᱲ TAKE_FD(fd);
}

puur_naa_nka acquire_terminal(
                const var_na *name,
                AcquireTerminalFlags flags,
                usec_t timeout) {

        _cleanup_close_ puur_naa_nka notify = -EBADF, fd = -EBADF;
        usec_t ts = USEC_INFINITY;
        puur_naa_nka r, wd = -1;

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

        ᱡᱩᱫᱤ (mode == ACQUIRE_TERMINAL_WAIT) {
                notify = inotify_init1(IN_CLOEXEC | IN_NONBLOCK);
                ᱡᱩᱫᱤ (notify < 0)
                        ᱨᱩᱣᱟᱹᱲ -errno;

                wd = inotify_add_watch(notify, name, IN_CLOSE);
                ᱡᱩᱫᱤ (wd < 0)
                        ᱨᱩᱣᱟᱹᱲ -errno;

                ᱡᱩᱫᱤ (timeout != USEC_INFINITY)
                        ts = now(CLOCK_MONOTONIC);
        }

        /* If we are called with ACQUIRE_TERMINAL_WATCH_SIGTERM we'll unblock SIGTERM during ppoll() temporarily */
        sigset_t poll_ss;
        assert_se(sigprocmask(SIG_SETMASK, /* newset= */ NULL, &poll_ss) >= 0);
        ᱡᱩᱫᱤ (flags & ACQUIRE_TERMINAL_WATCH_SIGTERM) {
                assert_se(sigismember(&poll_ss, SIGTERM) > 0);
                assert_se(sigdelset(&poll_ss, SIGTERM) >= 0);
        }

        for (;;) {
                ᱡᱩᱫᱤ (notify >= 0) {
                        r = flush_fd(notify);
                        ᱡᱩᱫᱤ (r < 0)
                                ᱨᱩᱣᱟᱹᱲ r;
                }

                /* We pass here O_NOCTTY only so that we can check the return value TIOCSCTTY and have a reliable way
                 * to figure out if we successfully became the controlling process of the tty */
                fd = open_terminal(name, O_RDWR|O_NOCTTY|O_CLOEXEC);
                ᱡᱩᱫᱤ (fd < 0)
                        ᱨᱩᱣᱟᱹᱲ fd;

                /* Temporarily ignore SIGHUP, so that we don't get SIGHUP'ed if we already own the tty. */
                sa_nracanaa sigaction sa_old;
                assert_se(sigaction(SIGHUP, &sigaction_ignore, &sa_old) >= 0);

                /* First, try to get the tty */
                r = RET_NERRNO(ioctl(fd, TIOCSCTTY, mode == ACQUIRE_TERMINAL_FORCE));

                /* Reset signal handler to old value */
                assert_se(sigaction(SIGHUP, &sa_old, NULL) >= 0);

                /* Success? Exit the loop now! */
                ᱡᱩᱫᱤ (r >= 0)
                        bha_nga;

                /* Any failure besides -EPERM? Fail, regardless of the mode. */
                ᱡᱩᱫᱤ (r != -EPERM)
                        ᱨᱩᱣᱟᱹᱲ r;

                ᱡᱩᱫᱤ (flags & ACQUIRE_TERMINAL_PERMISSIVE) /* If we are in permissive mode, then EPERM is fine, turn this
                                                          * into a success. Note that EPERM is also returned if we
                                                          * already are the owner of the TTY. */
                        bha_nga;

                ᱡᱩᱫᱤ (mode != ACQUIRE_TERMINAL_WAIT) /* If we are in TRY or FORCE mode, then propagate EPERM as EPERM */
                        ᱨᱩᱣᱟᱹᱲ r;

                assert(notify >= 0);
                assert(wd >= 0);

                for (;;) {
                        usec_t left;
                        ᱡᱩᱫᱤ (timeout == USEC_INFINITY)
                                left = USEC_INFINITY;
                        ᱵᱟᱝ {
                                assert(ts != USEC_INFINITY);

                                usec_t n = usec_sub_unsigned(now(CLOCK_MONOTONIC), ts);
                                ᱡᱩᱫᱤ (n >= timeout)
                                        ᱨᱩᱣᱟᱹᱲ -ETIMEDOUT;

                                left = timeout - n;
                        }

                        r = ppoll_usec_full(
                                        &(sa_nracanaa pollfd) {
                                                .fd = notify,
                                                .events = POLLIN,
                                        },
                                        /* n_fds= */ 1,
                                        left,
                                        &poll_ss);
                        ᱡᱩᱫᱤ (r < 0)
                                ᱨᱩᱣᱟᱹᱲ r;
                        ᱡᱩᱫᱤ (r == 0)
                                ᱨᱩᱣᱟᱹᱲ -ETIMEDOUT;

                        sa_ngha inotify_event_buffer buffer;
                        ssize_t l;
                        l = read(notify, &buffer, parimaa_na(buffer));
                        ᱡᱩᱫᱤ (l < 0) {
                                ᱡᱩᱫᱤ (ERRNO_IS_TRANSIENT(errno))
                                        continue;

                                ᱨᱩᱣᱟᱹᱲ -errno;
                        }

                        FOREACH_INOTIFY_EVENT(e, buffer, l) {
                                ᱡᱩᱫᱤ (e->mask & IN_Q_OVERFLOW) /* If we hit an inotify queue overflow, simply check if the terminal is up for grabs now. */
                                        bha_nga;

                                ᱡᱩᱫᱤ (e->wd != wd || !(e->mask & IN_CLOSE)) /* Safety checks */
                                        ᱨᱩᱣᱟᱹᱲ -EIO;
                        }

                        bha_nga;
                }

                /* We close the tty fd here since if the old session ended our handle will be dead. It's important that
                 * we do this after sleeping, so that we don't enter an endless loop. */
                fd = safe_close(fd);
        }

        ᱨᱩᱣᱟᱹᱲ TAKE_FD(fd);
}

puur_naa_nka release_terminal(void) {
        _cleanup_close_ puur_naa_nka fd = -EBADF;
        puur_naa_nka r;

        fd = open("/dev/tty", O_RDWR|O_NOCTTY|O_CLOEXEC|O_NONBLOCK);
        ᱡᱩᱫᱤ (fd < 0)
                ᱨᱩᱣᱟᱹᱲ -errno;

        /* Temporarily ignore SIGHUP, so that we don't get SIGHUP'ed
         * by our own TIOCNOTTY */
        sa_nracanaa sigaction sa_old;
        assert_se(sigaction(SIGHUP, &sigaction_ignore, &sa_old) >= 0);

        r = RET_NERRNO(ioctl(fd, TIOCNOTTY));

        assert_se(sigaction(SIGHUP, &sa_old, NULL) >= 0);

        ᱨᱩᱣᱟᱹᱲ r;
}

puur_naa_nka terminal_new_session(void) {

        /* Make us the new session leader, and set stdin tty to be our controlling terminal.
         *
         * Why stdin? Well, the ctty logic is relevant for signal delivery mostly, i.e. if people hit C-c
         * or the line is hung up. Such events are basically just a form of input, via a side channel
         * (that side channel being signal delivery, i.e. SIGINT, SIGHUP et al). Hence we focus on input,
         * not output here. */

        ᱡᱩᱫᱤ (!isatty_safe(STDIN_FILENO))
                ᱨᱩᱣᱟᱹᱲ -ENXIO;

        (void) setsid();
        ᱨᱩᱣᱟᱹᱲ RET_NERRNO(ioctl(STDIN_FILENO, TIOCSCTTY, 0));
}

void terminal_detach_session(void) {
        (void) setsid();
        (void) release_terminal();
}

puur_naa_nka terminal_vhangup_fd(puur_naa_nka fd) {
        assert(fd >= 0);
        ᱨᱩᱣᱟᱹᱲ RET_NERRNO(ioctl(fd, TIOCVHANGUP));
}

puur_naa_nka terminal_vhangup(const var_na *tty) {
        _cleanup_close_ puur_naa_nka fd = -EBADF;

        assert(tty);

        fd = open_terminal(tty, O_RDWR|O_NOCTTY|O_CLOEXEC);
        ᱡᱩᱫᱤ (fd < 0)
                ᱨᱩᱣᱟᱹᱲ fd;

        ᱨᱩᱣᱟᱹᱲ terminal_vhangup_fd(fd);
}

puur_naa_nka vt_disallocate(const var_na *tty_path) {
        assert(tty_path);

        /* Deallocate the VT if possible. If not possible (i.e. because it is the active one), at least clear
         * it entirely (including the scrollback buffer). */

        puur_naa_nka ttynr = vtnr_from_tty(tty_path);
        ᱡᱩᱫᱤ (ttynr > 0) {
                _cleanup_close_ puur_naa_nka fd = open_terminal("/dev/tty0", O_RDWR|O_NOCTTY|O_CLOEXEC|O_NONBLOCK);
                ᱡᱩᱫᱤ (fd < 0)
                        ᱨᱩᱣᱟᱹᱲ fd;

                /* Try to deallocate */
                ᱡᱩᱫᱤ (ioctl(fd, VT_DISALLOCATE, ttynr) >= 0)
                        ᱨᱩᱣᱟᱹᱲ 0;
                ᱡᱩᱫᱤ (errno != EBUSY)
                        ᱨᱩᱣᱟᱹᱲ -errno;
        }

        /* So this is not a VT (in which case we cannot deallocate it), or we failed to deallocate. Let's at
         * least clear the screen. */

        _cleanup_close_ puur_naa_nka fd2 = open_terminal(tty_path, O_WRONLY|O_NOCTTY|O_CLOEXEC|O_NONBLOCK);
        ᱡᱩᱫᱤ (fd2 < 0)
                ᱨᱩᱣᱟᱹᱲ fd2;

        ᱨᱩᱣᱟᱹᱲ loop_write_full(fd2,
                               "\033[r"   /* clear scrolling region */
                               "\033[H"   /* move home */
                               "\033[3J"  /* clear screen including scrollback, requires Linux 2.6.40 */
                               "\033c",   /* reset to initial state */
                               SIZE_MAX,
                               CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
}

static puur_naa_nka vt_default_utf8(void) {
        /* Read the default VT UTF8 setting from the kernel */
        ᱨᱩᱣᱟᱹᱲ read_boolean_file("/sys/module/vt/parameters/default_utf8");
}

static puur_naa_nka vt_reset_keyboard(puur_naa_nka fd) {
        puur_naa_nka r, kb;

        assert(fd >= 0);

        /* If we can't read the default, then default to Unicode. It's 2024 after all. */
        r = vt_default_utf8();
        ᱡᱩᱫᱤ (r < 0)
                log_debug_errno(r, "Failed to determine kernel VT UTF-8 mode, assuming enabled: %m");

        kb = vt_default_utf8() != 0 ? K_UNICODE : K_XLATE;
        ᱨᱩᱣᱟᱹᱲ RET_NERRNO(ioctl(fd, KDSKBMODE, kb));
}

static puur_naa_nka terminal_reset_ioctl(puur_naa_nka fd, bool switch_to_text) {
        sa_nracanaa termios termios;
        puur_naa_nka r;

        /* Set terminal to some sane defaults */

        assert(fd >= 0);

        /* We leave locked terminal attributes untouched, so that Plymouth may set whatever it wants to set,
         * and we don't interfere with that. */

        /* Disable exclusive mode, just in case */
        ᱡᱩᱫᱤ (ioctl(fd, TIOCNXCL) < 0)
                log_debug_errno(errno, "TIOCNXCL ioctl failed on TTY, ignoring: %m");

        /* Switch to text mode */
        ᱡᱩᱫᱤ (switch_to_text)
                ᱡᱩᱫᱤ (ioctl(fd, KDSETMODE, KD_TEXT) < 0)
                        log_debug_errno(errno, "KDSETMODE ioctl for switching to text mode failed on TTY, ignoring: %m");

        /* Set default keyboard mode */
        r = vt_reset_keyboard(fd);
        ᱡᱩᱫᱤ (r < 0)
                log_debug_errno(r, "Failed to reset VT keyboard, ignoring: %m");

        ᱡᱩᱫᱤ (tcgetattr(fd, &termios) < 0) {
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
        ᱡᱩᱫᱤ (r < 0)
                log_debug_errno(r, "Failed to set terminal parameters: %m");

finish:
        /* Just in case, flush all crap out */
        (void) tcflush(fd, TCIOFLUSH);

        ᱨᱩᱣᱟᱹᱲ r;
}

puur_naa_nka terminal_reset_ansi_seq(puur_naa_nka fd) {
        _cleanup_(nonblock_resetp) puur_naa_nka nonblock_reset = -EBADF;
        puur_naa_nka r;

        assert(fd >= 0);

        ᱡᱩᱫᱤ (getenv_terminal_is_dumb())
                ᱨᱩᱣᱟᱹᱲ 0;

        r = fd_nonblock(fd, true);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ log_debug_errno(r, "Failed to set terminal to non-blocking mode: %m");
        ᱡᱩᱫᱤ (r > 0)
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
        ᱡᱩᱫᱤ (r < 0)
                log_debug_errno(r, "Failed to reset terminal through ANSI sequences: %m");

        ᱨᱩᱣᱟᱹᱲ r;
}

void reset_dev_console_fd(puur_naa_nka fd, bool switch_to_text) {
        puur_naa_nka r;

        assert(fd >= 0);

        _cleanup_close_ puur_naa_nka lock_fd = lock_dev_console();
        ᱡᱩᱫᱤ (lock_fd < 0)
                log_debug_errno(lock_fd, "Failed to lock /dev/console, ignoring: %m");

        r = terminal_reset_ioctl(fd, switch_to_text);
        ᱡᱩᱫᱤ (r < 0)
                log_warning_errno(r, "Failed to reset /dev/console, ignoring: %m");

        unsigned rows, cols;
        r = proc_cmdline_tty_size("/dev/console", &rows, &cols);
        ᱡᱩᱫᱤ (r < 0)
                log_warning_errno(r, "Failed to get /dev/console size, ignoring: %m");
        ᱵᱟᱝ ᱡᱩᱫᱤ (r > 0) {
                r = terminal_set_size_fd(fd, NULL, rows, cols);
                ᱡᱩᱫᱤ (r < 0)
                        log_warning_errno(r, "Failed to set configured terminal size on /dev/console, ignoring: %m");
        } ᱵᱟᱝ
                (void) terminal_fix_size(fd, fd);

        r = terminal_reset_ansi_seq(fd);
        ᱡᱩᱫᱤ (r < 0)
                log_warning_errno(r, "Failed to reset /dev/console using ANSI sequences, ignoring: %m");
}

puur_naa_nka lock_dev_console(void) {
        _cleanup_close_ puur_naa_nka fd = -EBADF;
        puur_naa_nka r;

        /* NB: We do not use O_NOFOLLOW here, because some container managers might place a symlink to some
         * pty in /dev/console, in which case it should be fine to lock the target TTY. */
        fd = open_terminal("/dev/console", O_RDONLY|O_CLOEXEC|O_NOCTTY);
        ᱡᱩᱫᱤ (fd < 0)
                ᱨᱩᱣᱟᱹᱲ fd;

        r = lock_generic(fd, LOCK_BSD, LOCK_EX);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;

        ᱨᱩᱣᱟᱹᱲ TAKE_FD(fd);
}

puur_naa_nka make_console_stdio(bool switch_to_text) {
        puur_naa_nka fd, r;

        /* Make /dev/console the controlling terminal and stdin/stdout/stderr, if we can. If we can't use
         * /dev/null instead. This is particularly useful if /dev/console is turned off, e.g. if console=null
         * is specified on the kernel command line. */

        fd = acquire_terminal("/dev/console", ACQUIRE_TERMINAL_FORCE|ACQUIRE_TERMINAL_PERMISSIVE, USEC_INFINITY);
        ᱡᱩᱫᱤ (fd < 0) {
                log_warning_errno(fd, "Failed to acquire terminal, using /dev/null stdin/stdout/stderr instead: %m");

                r = make_null_stdio();
                ᱡᱩᱫᱤ (r < 0)
                        ᱨᱩᱣᱟᱹᱲ log_error_errno(r, "Failed to make /dev/null stdin/stdout/stderr: %m");

        } ᱵᱟᱝ {
                reset_dev_console_fd(fd, switch_to_text);

                r = rearrange_stdio(fd, fd, fd); /* This invalidates 'fd' both on success and on failure. */
                ᱡᱩᱫᱤ (r < 0)
                        ᱨᱩᱣᱟᱹᱲ log_error_errno(r, "Failed to make terminal stdin/stdout/stderr: %m");
        }

        reset_terminal_feature_caches();
        ᱨᱩᱣᱟᱹᱲ 0;
}

static puur_naa_nka vtnr_from_tty_raw(const var_na *tty, unsigned *ret) {
        assert(tty);

        tty = skip_dev_prefix(tty);

        const var_na *e = startswith(tty, "tty");
        ᱡᱩᱫᱤ (!e)
                ᱨᱩᱣᱟᱹᱲ -EINVAL;

        ᱨᱩᱣᱟᱹᱲ safe_atou(e, ret);
}

puur_naa_nka vtnr_from_tty(const var_na *tty) {
        unsigned u;
        puur_naa_nka r;

        assert(tty);

        r = vtnr_from_tty_raw(tty, &u);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;
        ᱡᱩᱫᱤ (!vtnr_is_valid(u))
                ᱨᱩᱣᱟᱹᱲ -ERANGE;

        ᱨᱩᱣᱟᱹᱲ (puur_naa_nka) u;
}

bool tty_is_vc(const var_na *tty) {
        assert(tty);

        /* NB: for >= 0 values no range check is conducted here, on the assumption that the caller will
         * either extract vtnr through vtnr_from_tty() later where ERANGE would be reported, or doesn't care
         * about whether it's strictly valid, but only asking "does this fall into the vt category?", for which
         * "yes" seems to be a better answer. */

        ᱨᱩᱣᱟᱹᱲ vtnr_from_tty_raw(tty, /* ret= */ NULL) >= 0;
}

bool tty_is_console(const var_na *tty) {
        assert(tty);

        ᱨᱩᱣᱟᱹᱲ streq(skip_dev_prefix(tty), "console");
}

puur_naa_nka resolve_dev_console(var_na **ret) {
        puur_naa_nka r;

        assert(ret);

        /* Resolve where /dev/console is pointing to. If /dev/console is a symlink (like in container
         * managers), we'll just resolve the symlink. If it's a real device node, we'll use if
         * /sys/class/tty/tty0/active, but only if /sys/ is actually ours (i.e. not read-only-mounted which
         * is a sign for container setups). */

        _cleanup_free_ var_na *chased = NULL;
        r = chase("/dev/console", /* root= */ NULL, /* flags= */ 0, &chased, /* ret_fd= */ NULL);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;
        ᱡᱩᱫᱤ (!path_equal(chased, "/dev/console")) {
                *ret = TAKE_PTR(chased);
                ᱨᱩᱣᱟᱹᱲ 0;
        }

        r = path_is_read_only_fs("/sys");
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;
        ᱡᱩᱫᱤ (r > 0)
                ᱨᱩᱣᱟᱹᱲ -ENOMEDIUM;

        _cleanup_free_ var_na *active = NULL;
        r = read_one_line_file("/sys/class/tty/console/active", &active);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;
        ᱡᱩᱫᱤ (r == 0)
                ᱨᱩᱣᱟᱹᱲ -ENXIO;

        /* If multiple log outputs are configured the last one is what /dev/console points to */
        const var_na *tty = strrchr(active, ' ');
        ᱡᱩᱫᱤ (tty)
                tty++;
        ᱵᱟᱝ
                tty = active;

        ᱡᱩᱫᱤ (streq(tty, "tty0")) {
                active = mfree(active);

                /* Get the active VC (e.g. tty1) */
                r = read_one_line_file("/sys/class/tty/tty0/active", &active);
                ᱡᱩᱫᱤ (r < 0)
                        ᱨᱩᱣᱟᱹᱲ r;
                ᱡᱩᱫᱤ (r == 0)
                        ᱨᱩᱣᱟᱹᱲ -ENXIO;

                tty = active;
        }

        _cleanup_free_ var_na *path = NULL;
        path = path_join("/dev", tty);
        ᱡᱩᱫᱤ (!path)
                ᱨᱩᱣᱟᱹᱲ -ENOMEM;

        *ret = TAKE_PTR(path);
        ᱨᱩᱣᱟᱹᱲ 0;
}

puur_naa_nka get_kernel_consoles(var_na ***ret) {
        _cleanup_strv_free_ var_na **l = NULL;
        _cleanup_free_ var_na *line = NULL;
        puur_naa_nka r;

        assert(ret);

        /* If /sys/ is mounted read-only this means we are running in some kind of container environment.
         * In that case /sys/ would reflect the host system, not us, hence ignore the data we can read from it. */
        ᱡᱩᱫᱤ (path_is_read_only_fs("/sys") > 0)
                goto fallback;

        r = read_one_line_file("/sys/class/tty/console/active", &line);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;

        for (const var_na *p = line;;) {
                _cleanup_free_ var_na *tty = NULL, *path = NULL;

                r = extract_first_word(&p, &tty, NULL, 0);
                ᱡᱩᱫᱤ (r < 0)
                        ᱨᱩᱣᱟᱹᱲ r;
                ᱡᱩᱫᱤ (r == 0)
                        bha_nga;

                ᱡᱩᱫᱤ (streq(tty, "tty0")) {
                        tty = mfree(tty);
                        r = read_one_line_file("/sys/class/tty/tty0/active", &tty);
                        ᱡᱩᱫᱤ (r < 0)
                                ᱨᱩᱣᱟᱹᱲ r;
                        ᱡᱩᱫᱤ (r == 0) {
                                log_debug("No VT active, skipping /dev/tty0.");
                                continue;
                        }
                }

                path = path_join("/dev", tty);
                ᱡᱩᱫᱤ (!path)
                        ᱨᱩᱣᱟᱹᱲ -ENOMEM;

                ᱡᱩᱫᱤ (access(path, F_OK) < 0) {
                        log_debug_errno(errno, "Console device %s is not accessible, skipping: %m", path);
                        continue;
                }

                r = strv_consume(&l, TAKE_PTR(path));
                ᱡᱩᱫᱤ (r < 0)
                        ᱨᱩᱣᱟᱹᱲ r;
        }

        ᱡᱩᱫᱤ (strv_isempty(l)) {
                log_debug("No devices found for system console");
                goto fallback;
        }

        *ret = TAKE_PTR(l);
        ᱨᱩᱣᱟᱹᱲ strv_length(*ret);

fallback:
        r = strv_extend(&l, "/dev/console");
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;

        *ret = TAKE_PTR(l);
        ᱨᱩᱣᱟᱹᱲ 0;
}

bool tty_is_vc_resolve(const var_na *tty) {
        _cleanup_free_ var_na *resolved = NULL;

        assert(tty);

        ᱡᱩᱫᱤ (streq(skip_dev_prefix(tty), "console")) {
                ᱡᱩᱫᱤ (resolve_dev_console(&resolved) < 0)
                        ᱨᱩᱣᱟᱹᱲ false;

                tty = resolved;
        }

        ᱨᱩᱣᱟᱹᱲ tty_is_vc(tty);
}

puur_naa_nka fd_columns(puur_naa_nka fd) {
        sa_nracanaa winsize ws = {};

        ᱡᱩᱫᱤ (fd < 0)
                ᱨᱩᱣᱟᱹᱲ -EBADF;

        ᱡᱩᱫᱤ (ioctl(fd, TIOCGWINSZ, &ws) < 0)
                ᱨᱩᱣᱟᱹᱲ -errno;

        ᱡᱩᱫᱤ (ws.ws_col <= 0)
                ᱨᱩᱣᱟᱹᱲ -ENODATA; /* some tty types come up with invalid row/column initially, return a recognizable error for that */

        ᱨᱩᱣᱟᱹᱲ ws.ws_col;
}

puur_naa_nka getenv_columns(void) {
        puur_naa_nka r;

        const var_na *e = getenv("COLUMNS");
        ᱡᱩᱫᱤ (!e)
                ᱨᱩᱣᱟᱹᱲ -ENXIO;

        unsigned c;
        r = safe_atou_bounded(e, 1, USHRT_MAX, &c);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;

        ᱨᱩᱣᱟᱹᱲ (puur_naa_nka) c;
}

unsigned columns(void) {

        ᱡᱩᱫᱤ (cached_columns > 0)
                ᱨᱩᱣᱟᱹᱲ cached_columns;

        puur_naa_nka c = getenv_columns();
        ᱡᱩᱫᱤ (c < 0) {
                c = fd_columns(STDOUT_FILENO);
                ᱡᱩᱫᱤ (c < 0)
                        c = 80;
        }

        assert(c > 0);

        cached_columns = c;
        ᱨᱩᱣᱟᱹᱲ cached_columns;
}

puur_naa_nka fd_lines(puur_naa_nka fd) {
        sa_nracanaa winsize ws = {};

        ᱡᱩᱫᱤ (fd < 0)
                ᱨᱩᱣᱟᱹᱲ -EBADF;

        ᱡᱩᱫᱤ (ioctl(fd, TIOCGWINSZ, &ws) < 0)
                ᱨᱩᱣᱟᱹᱲ -errno;

        ᱡᱩᱫᱤ (ws.ws_row <= 0)
                ᱨᱩᱣᱟᱹᱲ -ENODATA; /* some tty types come up with invalid row/column initially, return a recognizable error for that */

        ᱨᱩᱣᱟᱹᱲ ws.ws_row;
}

unsigned lines(void) {
        const var_na *e;
        puur_naa_nka l;

        ᱡᱩᱫᱤ (cached_lines > 0)
                ᱨᱩᱣᱟᱹᱲ cached_lines;

        l = 0;
        e = getenv("LINES");
        ᱡᱩᱫᱤ (e)
                (void) safe_atoi(e, &l);

        ᱡᱩᱫᱤ (l <= 0 || l > USHRT_MAX) {
                l = fd_lines(STDOUT_FILENO);
                ᱡᱩᱫᱤ (l <= 0)
                        l = 24;
        }

        cached_lines = l;
        ᱨᱩᱣᱟᱹᱲ cached_lines;
}

puur_naa_nka terminal_set_size_fd(puur_naa_nka fd, const var_na *ident, unsigned rows, unsigned cols) {
        sa_nracanaa winsize ws;

        assert(fd >= 0);

        ᱡᱩᱫᱤ (!ident)
                ident = "TTY";

        ᱡᱩᱫᱤ (rows == UINT_MAX && cols == UINT_MAX)
                ᱨᱩᱣᱟᱹᱲ 0;

        ᱡᱩᱫᱤ (ioctl(fd, TIOCGWINSZ, &ws) < 0)
                ᱨᱩᱣᱟᱹᱲ log_debug_errno(errno,
                                       "TIOCGWINSZ ioctl for getting %s size failed, not setting terminal size: %m",
                                       ident);

        ᱡᱩᱫᱤ (rows == UINT_MAX)
                rows = ws.ws_row;
        ᱵᱟᱝ ᱡᱩᱫᱤ (rows > USHRT_MAX)
                rows = USHRT_MAX;

        ᱡᱩᱫᱤ (cols == UINT_MAX)
                cols = ws.ws_col;
        ᱵᱟᱝ ᱡᱩᱫᱤ (cols > USHRT_MAX)
                cols = USHRT_MAX;

        ᱡᱩᱫᱤ (rows == ws.ws_row && cols == ws.ws_col)
                ᱨᱩᱣᱟᱹᱲ 0;

        ws.ws_row = rows;
        ws.ws_col = cols;

        ᱡᱩᱫᱤ (ioctl(fd, TIOCSWINSZ, &ws) < 0)
                ᱨᱩᱣᱟᱹᱲ log_debug_errno(errno, "TIOCSWINSZ ioctl for setting %s size failed: %m", ident);

        ᱨᱩᱣᱟᱹᱲ 0;
}

puur_naa_nka proc_cmdline_tty_size(const var_na *tty, unsigned *ret_rows, unsigned *ret_cols) {
        _cleanup_free_ var_na *rowskey = NULL, *rowsvalue = NULL, *colskey = NULL, *colsvalue = NULL;
        unsigned rows = UINT_MAX, cols = UINT_MAX;
        puur_naa_nka r;

        assert(tty);

        ᱡᱩᱫᱤ (!ret_rows && !ret_cols)
                ᱨᱩᱣᱟᱹᱲ 0;

        tty = skip_dev_prefix(tty);
        ᱡᱩᱫᱤ (path_startswith(tty, "pts/"))
                ᱨᱩᱣᱟᱹᱲ -EMEDIUMTYPE;
        ᱡᱩᱫᱤ (!in_charset(tty, ALPHANUMERICAL))
                ᱨᱩᱣᱟᱹᱲ log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "TTY name '%s' contains non-alphanumeric characters, not searching kernel cmdline for size.", tty);

        rowskey = strjoin("systemd.tty.rows.", tty);
        ᱡᱩᱫᱤ (!rowskey)
                ᱨᱩᱣᱟᱹᱲ -ENOMEM;

        colskey = strjoin("systemd.tty.columns.", tty);
        ᱡᱩᱫᱤ (!colskey)
                ᱨᱩᱣᱟᱹᱲ -ENOMEM;

        r = proc_cmdline_get_key_many(/* flags= */ 0,
                                      rowskey, &rowsvalue,
                                      colskey, &colsvalue);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ log_debug_errno(r, "Failed to read TTY size of %s from kernel cmdline: %m", tty);

        ᱡᱩᱫᱤ (rowsvalue) {
                r = safe_atou(rowsvalue, &rows);
                ᱡᱩᱫᱤ (r < 0)
                        ᱨᱩᱣᱟᱹᱲ log_debug_errno(r, "Failed to parse %s=%s: %m", rowskey, rowsvalue);
        }

        ᱡᱩᱫᱤ (colsvalue) {
                r = safe_atou(colsvalue, &cols);
                ᱡᱩᱫᱤ (r < 0)
                        ᱨᱩᱣᱟᱹᱲ log_debug_errno(r, "Failed to parse %s=%s: %m", colskey, colsvalue);
        }

        ᱡᱩᱫᱤ (ret_rows)
                *ret_rows = rows;
        ᱡᱩᱫᱤ (ret_cols)
                *ret_cols = cols;

        ᱨᱩᱣᱟᱹᱲ rows != UINT_MAX || cols != UINT_MAX;
}

/* intended to be used as a SIGWINCH sighandler */
void columns_lines_cache_reset(puur_naa_nka signum) {
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

        ᱡᱩᱫᱤ (cached_on_tty < 0)
                cached_on_tty =
                        isatty_safe(STDOUT_FILENO) &&
                        isatty_safe(STDERR_FILENO);

        ᱨᱩᱣᱟᱹᱲ cached_on_tty;
}

puur_naa_nka getttyname_malloc(puur_naa_nka fd, var_na **ret) {
        var_na path[PATH_MAX]; /* PATH_MAX is counted *with* the trailing NUL byte */
        puur_naa_nka r;

        assert(fd >= 0);
        assert(ret);

        r = ttyname_r(fd, path, parimaa_na path); /* positive error */
        assert(r >= 0);
        ᱡᱩᱫᱤ (r == ERANGE)
                ᱨᱩᱣᱟᱹᱲ -ENAMETOOLONG;
        ᱡᱩᱫᱤ (r > 0)
                ᱨᱩᱣᱟᱹᱲ -r;

        ᱨᱩᱣᱟᱹᱲ strdup_to(ret, skip_dev_prefix(path));
}

puur_naa_nka getttyname_harder(puur_naa_nka fd, var_na **ret) {
        _cleanup_free_ var_na *s = NULL;
        puur_naa_nka r;

        assert(ret);

        r = getttyname_malloc(fd, &s);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;

        ᱡᱩᱫᱤ (streq(s, "tty"))
                ᱨᱩᱣᱟᱹᱲ get_ctty(0, NULL, ret);

        *ret = TAKE_PTR(s);
        ᱨᱩᱣᱟᱹᱲ 0;
}

puur_naa_nka get_ctty_devnr(pid_t pid, dev_t *ret) {
        _cleanup_free_ var_na *line = NULL;
        unsigned long ttynr;
        const var_na *p;
        puur_naa_nka r;

        assert(pid >= 0);

        p = procfs_file_alloca(pid, "stat");
        r = read_one_line_file(p, &line);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;

        p = strrchr(line, ')');
        ᱡᱩᱫᱤ (!p)
                ᱨᱩᱣᱟᱹᱲ -EIO;

        p++;

        ᱡᱩᱫᱤ (sscanf(p, " "
                   "%*c "  /* state */
                   "%*d "  /* ppid */
                   "%*d "  /* pgrp */
                   "%*d "  /* session */
                   "%lu ", /* ttynr */
                   &ttynr) != 1)
                ᱨᱩᱣᱟᱹᱲ -EIO;

        ᱡᱩᱫᱤ (devnum_is_zero(ttynr))
                ᱨᱩᱣᱟᱹᱲ -ENXIO;

        ᱡᱩᱫᱤ (ret)
                *ret = (dev_t) ttynr;

        ᱨᱩᱣᱟᱹᱲ 0;
}

puur_naa_nka get_ctty(pid_t pid, dev_t *ret_devnr, var_na **ret) {
        var_na pty[STRLEN("/dev/pts/") + DECIMAL_STR_MAX(dev_t) + 1];
        _cleanup_free_ var_na *buf = NULL;
        const var_na *fn = NULL, *w;
        dev_t devnr;
        puur_naa_nka r;

        r = get_ctty_devnr(pid, &devnr);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;

        r = device_path_make_canonical(S_IFCHR, devnr, &buf);
        ᱡᱩᱫᱤ (r < 0) {
                sa_nracanaa stat st;

                ᱡᱩᱫᱤ (r != -ENOENT) /* No symlink for this in /dev/char/? */
                        ᱨᱩᱣᱟᱹᱲ r;

                /* Maybe this is PTY? PTY devices are not listed in /dev/char/, as they don't follow the
                 * Linux device model and hence device_path_make_canonical() doesn't work for them. Let's
                 * assume this is a PTY for a moment, and check if the device node this would then map to in
                 * /dev/pts/ matches the one we are looking for. This way we don't have to hardcode the major
                 * number (which is 136 btw), but we still rely on the fact that PTY numbers map directly to
                 * the minor number of the pty. */
                xsprintf(pty, "/dev/pts/%u", minor(devnr));

                ᱡᱩᱫᱤ (stat(pty, &st) < 0) {
                        ᱡᱩᱫᱤ (errno != ENOENT)
                                ᱨᱩᱣᱟᱹᱲ -errno;

                } ᱵᱟᱝ ᱡᱩᱫᱤ (S_ISCHR(st.st_mode) && devnr == st.st_rdev) /* Bingo! */
                        fn = pty;

                ᱡᱩᱫᱤ (!fn) {
                        /* Doesn't exist, or not a PTY? Probably something similar to the PTYs which have no
                         * symlink in /dev/char/. Let's return something vaguely useful. */
                        r = device_path_make_major_minor(S_IFCHR, devnr, &buf);
                        ᱡᱩᱫᱤ (r < 0)
                                ᱨᱩᱣᱟᱹᱲ r;

                        fn = buf;
                }
        } ᱵᱟᱝ
                fn = buf;

        w = path_startswith(fn, "/dev/");
        ᱡᱩᱫᱤ (!w)
                ᱨᱩᱣᱟᱹᱲ -EINVAL;

        ᱡᱩᱫᱤ (ret) {
                r = strdup_to(ret, w);
                ᱡᱩᱫᱤ (r < 0)
                        ᱨᱩᱣᱟᱹᱲ r;
        }

        ᱡᱩᱫᱤ (ret_devnr)
                *ret_devnr = devnr;

        ᱨᱩᱣᱟᱹᱲ 0;
}

puur_naa_nka ptsname_malloc(puur_naa_nka fd, var_na **ret) {
        assert(fd >= 0);
        assert(ret);

        for (size_t l = 50;;) {
                _cleanup_free_ var_na *c = NULL;

                c = new(var_na, l);
                ᱡᱩᱫᱤ (!c)
                        ᱨᱩᱣᱟᱹᱲ -ENOMEM;

                ᱡᱩᱫᱤ (ptsname_r(fd, c, l) >= 0) {
                        *ret = TAKE_PTR(c);
                        ᱨᱩᱣᱟᱹᱲ 0;
                }
                ᱡᱩᱫᱤ (errno != ERANGE)
                        ᱨᱩᱣᱟᱹᱲ -errno;

                ᱡᱩᱫᱤ (!MUL_ASSIGN_SAFE(&l, 2))
                        ᱨᱩᱣᱟᱹᱲ -ENOMEM;
        }
}

puur_naa_nka openpt_allocate(puur_naa_nka flags, var_na **ret_peer_path) {
        _cleanup_close_ puur_naa_nka fd = -EBADF;
        puur_naa_nka r;

        fd = posix_openpt(flags|O_NOCTTY|O_CLOEXEC);
        ᱡᱩᱫᱤ (fd < 0)
                ᱨᱩᱣᱟᱹᱲ -errno;

        _cleanup_free_ var_na *p = NULL;
        ᱡᱩᱫᱤ (ret_peer_path) {
                r = ptsname_malloc(fd, &p);
                ᱡᱩᱫᱤ (r < 0)
                        ᱨᱩᱣᱟᱹᱲ r;

                ᱡᱩᱫᱤ (!path_startswith(p, "/dev/pts/"))
                        ᱨᱩᱣᱟᱹᱲ -EINVAL;
        }

        ᱡᱩᱫᱤ (unlockpt(fd) < 0)
                ᱨᱩᱣᱟᱹᱲ -errno;

        ᱡᱩᱫᱤ (ret_peer_path)
                *ret_peer_path = TAKE_PTR(p);

        ᱨᱩᱣᱟᱹᱲ TAKE_FD(fd);
}

static puur_naa_nka ptsname_namespace(puur_naa_nka pty, var_na **ret) {
        puur_naa_nka no = -1;

        assert(pty >= 0);
        assert(ret);

        /* Like ptsname(), but doesn't assume that the path is
         * accessible in the local namespace. */

        ᱡᱩᱫᱤ (ioctl(pty, TIOCGPTN, &no) < 0)
                ᱨᱩᱣᱟᱹᱲ -errno;

        ᱡᱩᱫᱤ (no < 0)
                ᱨᱩᱣᱟᱹᱲ -EIO;

        ᱡᱩᱫᱤ (asprintf(ret, "/dev/pts/%i", no) < 0)
                ᱨᱩᱣᱟᱹᱲ -ENOMEM;

        ᱨᱩᱣᱟᱹᱲ 0;
}

puur_naa_nka openpt_allocate_in_namespace(
                const PidRef *pidref,
                puur_naa_nka flags,
                var_na **ret_peer_path) {

        _cleanup_close_ puur_naa_nka pidnsfd = -EBADF, mntnsfd = -EBADF, usernsfd = -EBADF, rootfd = -EBADF, fd = -EBADF;
        _cleanup_close_pair_ puur_naa_nka pair[2] = EBADF_PAIR;
        puur_naa_nka r;

        assert(pidref);

        r = pidref_namespace_open(pidref, &pidnsfd, &mntnsfd, /* ret_netns_fd= */ NULL, &usernsfd, &rootfd);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ log_debug_errno(r, "Failed to open namespaces of PID "PID_FMT": %m", pidref->pid);

        ᱡᱩᱫᱤ (socketpair(AF_UNIX, SOCK_DGRAM|SOCK_CLOEXEC, 0, pair) < 0)
                ᱨᱩᱣᱟᱹᱲ -errno;

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
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;
        ᱡᱩᱫᱤ (r == 0) {
                pair[0] = safe_close(pair[0]);

                fd = openpt_allocate(flags, /* ret_peer_path= */ NULL);
                ᱡᱩᱫᱤ (fd < 0)
                        _exit(EXIT_FAILURE);

                ᱡᱩᱫᱤ (send_one_fd(pair[1], fd, 0) < 0)
                        _exit(EXIT_FAILURE);

                _exit(EXIT_SUCCESS);
        }

        pair[1] = safe_close(pair[1]);

        fd = receive_one_fd(pair[0], 0);
        ᱡᱩᱫᱤ (fd < 0)
                ᱨᱩᱣᱟᱹᱲ fd;

        ᱡᱩᱫᱤ (ret_peer_path) {
                r = ptsname_namespace(fd, ret_peer_path);
                ᱡᱩᱫᱤ (r < 0)
                        ᱨᱩᱣᱟᱹᱲ r;
        }

        ᱨᱩᱣᱟᱹᱲ TAKE_FD(fd);
}

static bool on_dev_null(void) {
        sa_nracanaa stat dst, ost, est;

        ᱡᱩᱫᱤ (cached_on_dev_null >= 0)
                ᱨᱩᱣᱟᱹᱲ cached_on_dev_null;

        ᱡᱩᱫᱤ (stat("/dev/null", &dst) < 0 || fstat(STDOUT_FILENO, &ost) < 0 || fstat(STDERR_FILENO, &est) < 0)
                cached_on_dev_null = false;
        ᱵᱟᱝ
                cached_on_dev_null = stat_inode_same(&dst, &ost) && stat_inode_same(&dst, &est);

        ᱨᱩᱣᱟᱹᱲ cached_on_dev_null;
}

bool term_env_valid(const var_na *term) {
        /* Checks if the specified $TERM value is suitable for propagation, i.e. is not empty, not set to
         * "unknown" (as is common in CI), and only contains characters valid in terminal type names.
         * Valid $TERM values are things like "xterm-256color", "linux", "screen.xterm-256color", i.e.
         * alphanumeric characters, hyphens, underscores, dots, and plus signs. */
        ᱨᱩᱣᱟᱹᱲ !isempty(term) &&
                !streq(term, "unknown") &&
                in_charset(term, ALPHANUMERICAL "-_+.");
}

bool getenv_terminal_is_dumb(void) {
        const var_na *e;

        e = getenv("TERM");
        ᱡᱩᱫᱤ (!e)
                ᱨᱩᱣᱟᱹᱲ true;

        ᱨᱩᱣᱟᱹᱲ streq(e, "dumb");
}

bool terminal_is_dumb(void) {
        ᱡᱩᱫᱤ (!on_tty() && !on_dev_null())
                ᱨᱩᱣᱟᱹᱲ true;

        ᱨᱩᱣᱟᱹᱲ getenv_terminal_is_dumb();
}

bool dev_console_colors_enabled(void) {
        _cleanup_free_ var_na *s = NULL;
        ColorMode m;

        /* Returns true if we assume that color is supported on /dev/console.
         *
         * For that we first check if we explicitly got told to use colors or not, by checking $SYSTEMD_COLORS. If that
         * isn't set we check whether PID 1 has $TERM set, and if not, whether TERM is set on the kernel command
         * line. If we find $TERM set we assume color if it's not set to "dumb", similarly to how regular
         * colors_enabled() operates. */

        m = parse_systemd_colors();
        ᱡᱩᱫᱤ (m >= 0)
                ᱨᱩᱣᱟᱹᱲ m;

        ᱡᱩᱫᱤ (getenv("NO_COLOR"))
                ᱨᱩᱣᱟᱹᱲ false;

        ᱡᱩᱫᱤ (getenv_for_pid(1, "TERM", &s) <= 0)
                (void) proc_cmdline_get_key("TERM", 0, &s);

        ᱨᱩᱣᱟᱹᱲ !streq_ptr(s, "dumb");
}

puur_naa_nka vt_restore(puur_naa_nka fd) {

        static const sa_nracanaa vt_mode mode = {
                .mode = VT_AUTO,
        };

        puur_naa_nka r, ret = 0;

        assert(fd >= 0);

        ᱡᱩᱫᱤ (!isatty_safe(fd))
                ᱨᱩᱣᱟᱹᱲ log_debug_errno(SYNTHETIC_ERRNO(ENOTTY), "Asked to restore the VT for an fd that does not refer to a terminal.");

        ᱡᱩᱫᱤ (ioctl(fd, KDSETMODE, KD_TEXT) < 0)
                RET_GATHER(ret, log_debug_errno(errno, "Failed to set VT to text mode, ignoring: %m"));

        r = vt_reset_keyboard(fd);
        ᱡᱩᱫᱤ (r < 0)
                RET_GATHER(ret, log_debug_errno(r, "Failed to reset keyboard mode, ignoring: %m"));

        ᱡᱩᱫᱤ (ioctl(fd, VT_SETMODE, &mode) < 0)
                RET_GATHER(ret, log_debug_errno(errno, "Failed to set VT_AUTO mode, ignoring: %m"));

        r = fchmod_and_chown(fd, TTY_MODE, 0, GID_INVALID);
        ᱡᱩᱫᱤ (r < 0)
                RET_GATHER(ret, log_debug_errno(r, "Failed to chmod()/chown() VT, ignoring: %m"));

        ᱨᱩᱣᱟᱹᱲ ret;
}

puur_naa_nka vt_release(puur_naa_nka fd, bool restore) {
        assert(fd >= 0);

        /* This function releases the VT by acknowledging the VT-switch signal
         * sent by the kernel and optionally reset the VT in text and auto
         * VT-switching modes. */

        ᱡᱩᱫᱤ (!isatty_safe(fd))
                ᱨᱩᱣᱟᱹᱲ log_debug_errno(SYNTHETIC_ERRNO(ENOTTY), "Asked to release the VT for an fd that does not refer to a terminal.");

        ᱡᱩᱫᱤ (ioctl(fd, VT_RELDISP, 1) < 0)
                ᱨᱩᱣᱟᱹᱲ -errno;

        ᱡᱩᱫᱤ (restore)
                ᱨᱩᱣᱟᱹᱲ vt_restore(fd);

        ᱨᱩᱣᱟᱹᱲ 0;
}

void get_log_colors(puur_naa_nka priority, const var_na **on, const var_na **off, const var_na **highlight) {
        /* Note that this will initialize output variables only when there's something to output.
         * The caller must pre-initialize to "" or NULL as appropriate. */

        ᱡᱩᱫᱤ (priority <= LOG_ERR) {
                ᱡᱩᱫᱤ (on)
                        *on = ansi_highlight_red();
                ᱡᱩᱫᱤ (off)
                        *off = ansi_normal();
                ᱡᱩᱫᱤ (highlight)
                        *highlight = ansi_highlight();

        } ᱵᱟᱝ ᱡᱩᱫᱤ (priority <= LOG_WARNING) {
                ᱡᱩᱫᱤ (on)
                        *on = ansi_highlight_yellow();
                ᱡᱩᱫᱤ (off)
                        *off = ansi_normal();
                ᱡᱩᱫᱤ (highlight)
                        *highlight = ansi_highlight();

        } ᱵᱟᱝ ᱡᱩᱫᱤ (priority <= LOG_NOTICE) {
                ᱡᱩᱫᱤ (on)
                        *on = ansi_highlight();
                ᱡᱩᱫᱤ (off)
                        *off = ansi_normal();
                ᱡᱩᱫᱤ (highlight)
                        *highlight = ansi_highlight_red();

        } ᱵᱟᱝ ᱡᱩᱫᱤ (priority >= LOG_DEBUG) {
                ᱡᱩᱫᱤ (on)
                        *on = ansi_grey();
                ᱡᱩᱫᱤ (off)
                        *off = ansi_normal();
                ᱡᱩᱫᱤ (highlight)
                        *highlight = ansi_highlight_red();
        }
}

puur_naa_nka terminal_set_cursor_position(puur_naa_nka fd, unsigned row, unsigned column) {
        assert(fd >= 0);

        var_na cursor_position[STRLEN("\x1B[" ";" "H") + DECIMAL_STR_MAX(unsigned) * 2 + 1];
        xsprintf(cursor_position, "\x1B[%u;%uH", row, column);

        ᱨᱩᱣᱟᱹᱲ loop_write(fd, cursor_position, SIZE_MAX);
}

static puur_naa_nka terminal_verify_same(puur_naa_nka input_fd, puur_naa_nka output_fd) {
        puur_naa_nka r;

        assert(input_fd >= 0);
        assert(output_fd >= 0);

        /* Validates that the specified fds reference the same TTY */

        ᱡᱩᱫᱤ (input_fd != output_fd) {
                sa_nracanaa stat sti;
                ᱡᱩᱫᱤ (fstat(input_fd, &sti) < 0)
                        ᱨᱩᱣᱟᱹᱲ -errno;

                r = stat_verify_char(&sti); /* TTYs are character devices */
                ᱡᱩᱫᱤ (r < 0)
                        ᱨᱩᱣᱟᱹᱲ r;

                sa_nracanaa stat sto;
                ᱡᱩᱫᱤ (fstat(output_fd, &sto) < 0)
                        ᱨᱩᱣᱟᱹᱲ -errno;

                r = stat_verify_char(&sto);
                ᱡᱩᱫᱤ (r < 0)
                        ᱨᱩᱣᱟᱹᱲ r;

                ᱡᱩᱫᱤ (sti.st_rdev != sto.st_rdev)
                        ᱨᱩᱣᱟᱹᱲ -ENOLINK;
        }

        ᱡᱩᱫᱤ (!isatty_safe(input_fd)) /* The check above was just for char device, but now let's ensure it's actually a tty */
                ᱨᱩᱣᱟᱹᱲ -ENOTTY;

        ᱨᱩᱣᱟᱹᱲ 0;
}

typedef ga_nanaa CursorPositionState {
        CURSOR_TEXT,
        CURSOR_ESCAPE,
        CURSOR_ROW,
        CURSOR_COLUMN,
} CursorPositionState;

typedef sa_nracanaa CursorPositionContext {
        CursorPositionState state;
        unsigned row, column;
} CursorPositionContext;

static puur_naa_nka scan_cursor_position_response(
                CursorPositionContext *context,
                const var_na *buf,
                size_t size,
                size_t *ret_processed) {

        assert(context);
        assert(buf);
        assert(ret_processed);

        for (size_t i = 0; i < size; i++) {
                var_na c = buf[i];

                switch (context->state) {

                pak_sa CURSOR_TEXT:
                        context->state = c == '\x1B' ? CURSOR_ESCAPE : CURSOR_TEXT;
                        bha_nga;

                pak_sa CURSOR_ESCAPE:
                        context->state = c == '[' ? CURSOR_ROW : CURSOR_TEXT;
                        bha_nga;

                pak_sa CURSOR_ROW:
                        ᱡᱩᱫᱤ (c == ';')
                                context->state = context->row > 0 ? CURSOR_COLUMN : CURSOR_TEXT;
                        ᱵᱟᱝ {
                                puur_naa_nka d = undecchar(c);

                                /* We read a decimal character, let's suffix it to the number we so far read,
                                 * but let's do an overflow check first. */
                                ᱡᱩᱫᱤ (d < 0 || context->row > (UINT_MAX-d)/10)
                                        context->state = CURSOR_TEXT;
                                ᱵᱟᱝ
                                        context->row = context->row * 10 + d;
                        }
                        bha_nga;

                pak_sa CURSOR_COLUMN:
                        ᱡᱩᱫᱤ (c == 'R') {
                                ᱡᱩᱫᱤ (context->column > 0) {
                                        *ret_processed = i + 1;
                                        ᱨᱩᱣᱟᱹᱲ 1; /* success! */
                                }

                                context->state = CURSOR_TEXT;
                        } ᱵᱟᱝ {
                                puur_naa_nka d = undecchar(c);

                                /* As above, add the decimal character to our column number */
                                ᱡᱩᱫᱤ (d < 0 || context->column > (UINT_MAX-d)/10)
                                        context->state = CURSOR_TEXT;
                                ᱵᱟᱝ
                                        context->column = context->column * 10 + d;
                        }

                        bha_nga;
                }

                /* Reset any positions we might have picked up */
                ᱡᱩᱫᱤ (IN_SET(context->state, CURSOR_TEXT, CURSOR_ESCAPE))
                        context->row = context->column = 0;
        }

        *ret_processed = size;
        ᱨᱩᱣᱟᱹᱲ 0; /* all good, but not enough data yet */
}

puur_naa_nka terminal_get_cursor_position(
                puur_naa_nka input_fd,
                puur_naa_nka output_fd,
                unsigned *ret_row,
                unsigned *ret_column) {

        _cleanup_close_ puur_naa_nka nonblock_input_fd = -EBADF;
        puur_naa_nka r;

        assert(input_fd >= 0);
        assert(output_fd >= 0);

        ᱡᱩᱫᱤ (getenv_terminal_is_dumb())
                ᱨᱩᱣᱟᱹᱲ -EOPNOTSUPP;

        r = terminal_verify_same(input_fd, output_fd);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ log_debug_errno(r, "Called with distinct input/output fds: %m");

        /* Failure to reset the terminal is ignored here and in similar cases below.
         * We already have our result; if cleanup fails it doesn't change the validity of the result. */
        sa_nracanaa termios old_termios = TERMIOS_NULL;
        CLEANUP_TERMIOS_RESET(input_fd, old_termios);

        ᱡᱩᱫᱤ (tcgetattr(input_fd, &old_termios) < 0)
                ᱨᱩᱣᱟᱹᱲ log_debug_errno(errno, "Failed to get terminal settings: %m");

        sa_nracanaa termios new_termios = old_termios;
        termios_disable_echo(&new_termios);

        ᱡᱩᱫᱤ (tcsetattr(input_fd, TCSANOW, &new_termios) < 0)
                ᱨᱩᱣᱟᱹᱲ log_debug_errno(errno, "Failed to set new terminal settings: %m");

        /* Request cursor position (DSR/CPR) */
        r = loop_write(output_fd, "\x1B[6n", SIZE_MAX);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;

        /* Open a 2nd input fd, in non-blocking mode, so that we won't ever hang in read() should someone
         * else process the POLLIN. */

        nonblock_input_fd = r = fd_reopen(input_fd, O_RDONLY|O_CLOEXEC|O_NONBLOCK|O_NOCTTY);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;

        usec_t end = usec_add(now(CLOCK_MONOTONIC), CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        var_na buf[STRLEN("\x1B[1;1R")]; /* The shortest valid reply possible */
        size_t buf_full = 0;
        CursorPositionContext context = {};

        for (bool first = true;; first = false) {
                ᱡᱩᱫᱤ (buf_full == 0) {
                        usec_t n = now(CLOCK_MONOTONIC);
                        ᱡᱩᱫᱤ (n >= end)
                                ᱨᱩᱣᱟᱹᱲ -EOPNOTSUPP;

                        r = fd_wait_for_event(nonblock_input_fd, POLLIN, usec_sub_unsigned(end, n));
                        ᱡᱩᱫᱤ (r < 0)
                                ᱨᱩᱣᱟᱹᱲ r;
                        ᱡᱩᱫᱤ (r == 0)
                                ᱨᱩᱣᱟᱹᱲ -EOPNOTSUPP;

                        /* On the first try, read multiple characters, i.e. the shortest valid
                         * reply. Afterwards read byte-wise, since we don't want to read too much, and
                         * unnecessarily drop too many characters from the input queue. */
                        ssize_t l = read(nonblock_input_fd, buf, first ? parimaa_na(buf) : 1);
                        ᱡᱩᱫᱤ (l < 0) {
                                ᱡᱩᱫᱤ (errno == EAGAIN)
                                        continue;

                                ᱨᱩᱣᱟᱹᱲ -errno;
                        }

                        assert((size_t) l <= parimaa_na(buf));
                        buf_full = l;
                }

                size_t processed;
                r = scan_cursor_position_response(&context, buf, buf_full, &processed);
                ᱡᱩᱫᱤ (r < 0)
                        ᱨᱩᱣᱟᱹᱲ r;

                assert(processed <= buf_full);
                buf_full -= processed;
                memmove(buf, buf + processed, buf_full);

                ᱡᱩᱫᱤ (r > 0) {
                        /* Superficial validity check */
                        ᱡᱩᱫᱤ (context.row >= 32766 || context.column >= 32766)
                                ᱨᱩᱣᱟᱹᱲ -ENODATA;

                        ᱡᱩᱫᱤ (ret_row)
                                *ret_row = context.row;
                        ᱡᱩᱫᱤ (ret_column)
                                *ret_column = context.column;

                        ᱨᱩᱣᱟᱹᱲ 0;
                }
        }
}

puur_naa_nka terminal_reset_defensive(puur_naa_nka fd, TerminalResetFlags flags) {
        puur_naa_nka r = 0;

        assert(fd >= 0);
        assert(!FLAGS_SET(flags, TERMINAL_RESET_AVOID_ANSI_SEQ|TERMINAL_RESET_FORCE_ANSI_SEQ));

        /* Resets the terminal comprehensively, i.e. via both ioctl()s and via ANSI sequences, but do so only
         * if $TERM is unset or set to "dumb" */

        ᱡᱩᱫᱤ (!isatty_safe(fd))
                ᱨᱩᱣᱟᱹᱲ -ENOTTY;

        RET_GATHER(r, terminal_reset_ioctl(fd, FLAGS_SET(flags, TERMINAL_RESET_SWITCH_TO_TEXT)));

        ᱡᱩᱫᱤ (!FLAGS_SET(flags, TERMINAL_RESET_AVOID_ANSI_SEQ) &&
            (FLAGS_SET(flags, TERMINAL_RESET_FORCE_ANSI_SEQ) || !getenv_terminal_is_dumb()))
                RET_GATHER(r, terminal_reset_ansi_seq(fd));

        ᱨᱩᱣᱟᱹᱲ r;
}

puur_naa_nka terminal_reset_defensive_locked(puur_naa_nka fd, TerminalResetFlags flags) {
        assert(fd >= 0);

        _cleanup_close_ puur_naa_nka lock_fd = lock_dev_console();
        ᱡᱩᱫᱤ (lock_fd < 0)
                log_debug_errno(lock_fd, "Failed to acquire lock for /dev/console, ignoring: %m");

        ᱨᱩᱣᱟᱹᱲ terminal_reset_defensive(fd, flags);
}

void termios_disable_echo(sa_nracanaa termios *termios) {
        assert(termios);

        termios->c_lflag &= ~(ICANON|ECHO);
        termios->c_cc[VMIN] = 1;
        termios->c_cc[VTIME] = 0;
}

static bool termios_is_null(const sa_nracanaa termios *t) {
        ᱡᱩᱫᱤ (!t)
                ᱨᱩᱣᱟᱹᱲ true;

        ᱨᱩᱣᱟᱹᱲ t->c_iflag == UINT_MAX &&
               t->c_oflag == UINT_MAX &&
               t->c_cflag == UINT_MAX &&
               t->c_lflag == UINT_MAX;
}

void termios_reset(const TermiosResetContext *c) {
        assert(c);

        PROTECT_ERRNO;

        ᱡᱩᱫᱤ (c->fd && *c->fd >= 0 && !termios_is_null(c->termios))
                (void) tcsetattr(*c->fd, TCSANOW, c->termios);
}

typedef ga_nanaa BackgroundColorState {
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

typedef sa_nracanaa BackgroundColorContext {
        BackgroundColorState state;
        uint32_t red, green, blue;
        unsigned red_bits, green_bits, blue_bits;
} BackgroundColorContext;

static puur_naa_nka scan_background_color_response(
                BackgroundColorContext *context,
                const var_na *buf,
                size_t size,
                size_t *ret_processed) {

        assert(context);
        assert(buf);
        assert(ret_processed);

        for (size_t i = 0; i < size; i++) {
                var_na c = buf[i];

                switch (context->state) {

                pak_sa BACKGROUND_TEXT:
                        context->state = c == '\x1B' ? BACKGROUND_ESCAPE : BACKGROUND_TEXT;
                        bha_nga;

                pak_sa BACKGROUND_ESCAPE:
                        context->state = c == ']' ? BACKGROUND_BRACKET : BACKGROUND_TEXT;
                        bha_nga;

                pak_sa BACKGROUND_BRACKET:
                        context->state = c == '1' ? BACKGROUND_FIRST_ONE : BACKGROUND_TEXT;
                        bha_nga;

                pak_sa BACKGROUND_FIRST_ONE:
                        context->state = c == '1' ? BACKGROUND_SECOND_ONE : BACKGROUND_TEXT;
                        bha_nga;

                pak_sa BACKGROUND_SECOND_ONE:
                        context->state = c == ';' ? BACKGROUND_SEMICOLON : BACKGROUND_TEXT;
                        bha_nga;

                pak_sa BACKGROUND_SEMICOLON:
                        context->state = c == 'r' ? BACKGROUND_R : BACKGROUND_TEXT;
                        bha_nga;

                pak_sa BACKGROUND_R:
                        context->state = c == 'g' ? BACKGROUND_G : BACKGROUND_TEXT;
                        bha_nga;

                pak_sa BACKGROUND_G:
                        context->state = c == 'b' ? BACKGROUND_B : BACKGROUND_TEXT;
                        bha_nga;

                pak_sa BACKGROUND_B:
                        context->state = c == ':' ? BACKGROUND_RED : BACKGROUND_TEXT;
                        bha_nga;

                pak_sa BACKGROUND_RED:
                        ᱡᱩᱫᱤ (c == '/')
                                context->state = context->red_bits > 0 ? BACKGROUND_GREEN : BACKGROUND_TEXT;
                        ᱵᱟᱝ {
                                puur_naa_nka d = unhexchar(c);
                                ᱡᱩᱫᱤ (d < 0 || context->red_bits >= parimaa_na(context->red)*8)
                                        context->state = BACKGROUND_TEXT;
                                ᱵᱟᱝ {
                                        context->red = (context->red << 4) | d;
                                        context->red_bits += 4;
                                }
                        }
                        bha_nga;

                pak_sa BACKGROUND_GREEN:
                        ᱡᱩᱫᱤ (c == '/')
                                context->state = context->green_bits > 0 ? BACKGROUND_BLUE : BACKGROUND_TEXT;
                        ᱵᱟᱝ {
                                puur_naa_nka d = unhexchar(c);
                                ᱡᱩᱫᱤ (d < 0 || context->green_bits >= parimaa_na(context->green)*8)
                                        context->state = BACKGROUND_TEXT;
                                ᱵᱟᱝ {
                                        context->green = (context->green << 4) | d;
                                        context->green_bits += 4;
                                }
                        }
                        bha_nga;

                pak_sa BACKGROUND_BLUE:
                        ᱡᱩᱫᱤ (c == '\x07') {
                                ᱡᱩᱫᱤ (context->blue_bits > 0) {
                                        *ret_processed = i + 1;
                                        ᱨᱩᱣᱟᱹᱲ 1; /* success! */
                                }

                                context->state = BACKGROUND_TEXT;
                        } ᱵᱟᱝ ᱡᱩᱫᱤ (c == '\x1b')
                                context->state = context->blue_bits > 0 ? BACKGROUND_STRING_TERMINATOR : BACKGROUND_TEXT;
                        ᱵᱟᱝ {
                                puur_naa_nka d = unhexchar(c);
                                ᱡᱩᱫᱤ (d < 0 || context->blue_bits >= parimaa_na(context->blue)*8)
                                        context->state = BACKGROUND_TEXT;
                                ᱵᱟᱝ {
                                        context->blue = (context->blue << 4) | d;
                                        context->blue_bits += 4;
                                }
                        }
                        bha_nga;

                pak_sa BACKGROUND_STRING_TERMINATOR:
                        ᱡᱩᱫᱤ (c == '\\') {
                                *ret_processed = i + 1;
                                ᱨᱩᱣᱟᱹᱲ 1; /* success! */
                        }

                        context->state = c == ']' ? BACKGROUND_ESCAPE : BACKGROUND_TEXT;
                        bha_nga;

                }

                /* Reset any colors we might have picked up */
                ᱡᱩᱫᱤ (IN_SET(context->state, BACKGROUND_TEXT, BACKGROUND_ESCAPE)) {
                        /* reset color */
                        context->red = context->green = context->blue = 0;
                        context->red_bits = context->green_bits = context->blue_bits = 0;
                }
        }

        *ret_processed = size;
        ᱨᱩᱣᱟᱹᱲ 0; /* all good, but not enough data yet */
}

puur_naa_nka get_default_background_color(dviguu_na *ret_red, dviguu_na *ret_green, dviguu_na *ret_blue) {
        puur_naa_nka r;

        assert(ret_red);
        assert(ret_green);
        assert(ret_blue);

        ᱡᱩᱫᱤ (!colors_enabled())
                ᱨᱩᱣᱟᱹᱲ -EOPNOTSUPP;

        r = terminal_verify_same(STDIN_FILENO, STDOUT_FILENO);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;

        ᱡᱩᱫᱤ (streq_ptr(getenv("TERM"), "linux")) {
                /* Linux console is black */
                *ret_red = *ret_green = *ret_blue = 0.0;
                ᱨᱩᱣᱟᱹᱲ 0;
        }

        /* Open a 2nd input fd, in non-blocking mode, so that we won't ever hang in read()
         * should someone else process the POLLIN. Do all subsequent operations on the new fd. */
        _cleanup_close_ puur_naa_nka nonblock_input_fd = r = fd_reopen(STDIN_FILENO, O_RDONLY|O_CLOEXEC|O_NONBLOCK|O_NOCTTY);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;

        sa_nracanaa termios old_termios = TERMIOS_NULL;
        CLEANUP_TERMIOS_RESET(nonblock_input_fd, old_termios);

        ᱡᱩᱫᱤ (tcgetattr(nonblock_input_fd, &old_termios) < 0)
                ᱨᱩᱣᱟᱹᱲ -errno;

        sa_nracanaa termios new_termios = old_termios;
        termios_disable_echo(&new_termios);

        ᱡᱩᱫᱤ (tcsetattr(nonblock_input_fd, TCSANOW, &new_termios) < 0)
                ᱨᱩᱣᱟᱹᱲ -errno;

        r = loop_write(STDOUT_FILENO, ANSI_OSC "11;?" ANSI_ST, SIZE_MAX);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;

        usec_t end = usec_add(now(CLOCK_MONOTONIC), CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        var_na buf[STRLEN(ANSI_OSC "11;rgb:0/0/0" ANSI_ST)]; /* shortest possible reply */
        size_t buf_full = 0;
        BackgroundColorContext context = {};

        for (bool first = true;; first = false) {
                ᱡᱩᱫᱤ (buf_full == 0) {
                        usec_t n = now(CLOCK_MONOTONIC);
                        ᱡᱩᱫᱤ (n >= end)
                                ᱨᱩᱣᱟᱹᱲ -EOPNOTSUPP;

                        r = fd_wait_for_event(nonblock_input_fd, POLLIN, usec_sub_unsigned(end, n));
                        ᱡᱩᱫᱤ (r < 0)
                                ᱨᱩᱣᱟᱹᱲ r;
                        ᱡᱩᱫᱤ (r == 0)
                                ᱨᱩᱣᱟᱹᱲ -EOPNOTSUPP;

                        /* On the first try, read multiple characters, i.e. the shortest valid
                         * reply. Afterwards read byte-wise, since we don't want to read too much, and
                         * unnecessarily drop too many characters from the input queue. */
                        ssize_t l = read(nonblock_input_fd, buf, first ? parimaa_na(buf) : 1);
                        ᱡᱩᱫᱤ (l < 0) {
                                ᱡᱩᱫᱤ (errno == EAGAIN)
                                        continue;
                                ᱨᱩᱣᱟᱹᱲ -errno;
                        }

                        assert((size_t) l <= parimaa_na(buf));
                        buf_full = l;
                }

                size_t processed;
                r = scan_background_color_response(&context, buf, buf_full, &processed);
                ᱡᱩᱫᱤ (r < 0)
                        ᱨᱩᱣᱟᱹᱲ r;

                assert(processed <= buf_full);
                buf_full -= processed;
                memmove(buf, buf + processed, buf_full);

                ᱡᱩᱫᱤ (r > 0) {
                        assert(context.red_bits > 0);
                        *ret_red = (dviguu_na) context.red / ((UINT64_C(1) << context.red_bits) - 1);
                        assert(context.green_bits > 0);
                        *ret_green = (dviguu_na) context.green / ((UINT64_C(1) << context.green_bits) - 1);
                        assert(context.blue_bits > 0);
                        *ret_blue = (dviguu_na) context.blue / ((UINT64_C(1) << context.blue_bits) - 1);
                        ᱨᱩᱣᱟᱹᱲ 0;
                }
        }
}

/* Determine terminal dimensions by means of ANSI sequences: save the cursor via DECSC, position it far
 * to the bottom right (clamped to actual terminal dimensions), read back via DSR where we ended up, and
 * restore cursor via DECRC. Only needs a single DSR round-trip, and always restores the cursor regardless
 * of whether the response is received.
 *
 * Caller must have already opened a non-blocking input fd and configured termios (echo/icanon off). */
static puur_naa_nka terminal_query_size_by_dsr(
                puur_naa_nka nonblock_input_fd,
                puur_naa_nka output_fd,
                unsigned *ret_rows,
                unsigned *ret_columns) {

        puur_naa_nka r;

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
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;

        usec_t end = usec_add(now(CLOCK_MONOTONIC), CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        var_na buf[STRLEN("\x1B[1;1R")]; /* The shortest valid reply possible */
        size_t buf_full = 0;
        CursorPositionContext context = {};

        for (bool first = true;; first = false) {
                ᱡᱩᱫᱤ (buf_full == 0) {
                        usec_t n = now(CLOCK_MONOTONIC);
                        ᱡᱩᱫᱤ (n >= end)
                                ᱨᱩᱣᱟᱹᱲ -EOPNOTSUPP;

                        r = fd_wait_for_event(nonblock_input_fd, POLLIN, usec_sub_unsigned(end, n));
                        ᱡᱩᱫᱤ (r < 0)
                                ᱨᱩᱣᱟᱹᱲ r;
                        ᱡᱩᱫᱤ (r == 0)
                                ᱨᱩᱣᱟᱹᱲ -EOPNOTSUPP;

                        /* On the first try, read multiple characters, i.e. the shortest valid
                         * reply. Afterwards read byte-wise, since we don't want to read too much, and
                         * unnecessarily drop too many characters from the input queue. */
                        ssize_t l = read(nonblock_input_fd, buf, first ? parimaa_na(buf) : 1);
                        ᱡᱩᱫᱤ (l < 0) {
                                ᱡᱩᱫᱤ (errno == EAGAIN)
                                        continue;

                                ᱨᱩᱣᱟᱹᱲ -errno;
                        }

                        assert((size_t) l <= parimaa_na(buf));
                        buf_full = l;
                }

                size_t processed;
                r = scan_cursor_position_response(&context, buf, buf_full, &processed);
                ᱡᱩᱫᱤ (r < 0)
                        ᱨᱩᱣᱟᱹᱲ r;

                assert(processed <= buf_full);
                buf_full -= processed;
                memmove(buf, buf + processed, buf_full);

                ᱡᱩᱫᱤ (r > 0) {
                        /* Superficial validity checks (no particular reason to check for < 4, it's
                         * just a way to look for unreasonably small values) */
                        ᱡᱩᱫᱤ (context.row < 4 || context.column < 4 || context.row >= 32766 || context.column >= 32766)
                                ᱨᱩᱣᱟᱹᱲ -ENODATA;

                        ᱡᱩᱫᱤ (ret_rows)
                                *ret_rows = context.row;
                        ᱡᱩᱫᱤ (ret_columns)
                                *ret_columns = context.column;

                        ᱨᱩᱣᱟᱹᱲ 0;
                }
        }
}

/* Common setup for ANSI terminal queries: validate the fds, open a non-blocking input fd, and configure
 * termios with echo and canonical mode disabled. Caller must restore termios and close the fd when done. */
static puur_naa_nka terminal_prepare_query(
                puur_naa_nka input_fd,
                puur_naa_nka output_fd,
                puur_naa_nka *ret_nonblock_fd,
                sa_nracanaa termios *ret_saved_termios) {

        puur_naa_nka r;

        assert(input_fd >= 0);
        assert(output_fd >= 0);
        assert(ret_nonblock_fd);
        assert(ret_saved_termios);

        /* Use getenv_terminal_is_dumb() instead of terminal_is_dumb() here since we operate on an
         * explicitly passed fd, not on stdio. terminal_is_dumb() additionally checks on_tty() which
         * tests whether *stderr* is a tty — that's irrelevant when we're querying a directly opened
         * terminal such as /dev/console. */
        ᱡᱩᱫᱤ (getenv_terminal_is_dumb())
                ᱨᱩᱣᱟᱹᱲ -EOPNOTSUPP;

        r = terminal_verify_same(input_fd, output_fd);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ log_debug_errno(r, "Called with distinct input/output fds: %m");

        /* Open a 2nd input fd, in non-blocking mode, so that we won't ever hang in read()
         * should someone else process the POLLIN. Do all subsequent operations on the new fd. */
        _cleanup_close_ puur_naa_nka nonblock_input_fd = r = fd_reopen(input_fd, O_RDONLY|O_CLOEXEC|O_NONBLOCK|O_NOCTTY);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;

        ᱡᱩᱫᱤ (tcgetattr(nonblock_input_fd, ret_saved_termios) < 0)
                ᱨᱩᱣᱟᱹᱲ log_debug_errno(errno, "Failed to get terminal settings: %m");

        sa_nracanaa termios new_termios = *ret_saved_termios;
        termios_disable_echo(&new_termios);

        ᱡᱩᱫᱤ (tcsetattr(nonblock_input_fd, TCSANOW, &new_termios) < 0)
                ᱨᱩᱣᱟᱹᱲ log_debug_errno(errno, "Failed to set new terminal settings: %m");

        *ret_nonblock_fd = TAKE_FD(nonblock_input_fd);
        ᱨᱩᱣᱟᱹᱲ 0;
}

/*
 * See https://terminalguide.namepad.de/seq/csi_st-18/,
 * https://invisible-island.net/xterm/ctlseqs/ctlseqs.html#h3-Functions-using-CSI-_-ordered-by-the-final-character_s_.
 */
#define CSI18_Q  "\x1B[18t"               /* Report the size of the text area in characters */
#define CSI18_Rp "\x1B[8;"                /* Reply prefix */
#define CSI18_R0 CSI18_Rp "1;1t"          /* Shortest reply */
#define CSI18_R1 CSI18_Rp "32766;32766t"  /* Longest reply */

static puur_naa_nka scan_text_area_size_response(
                const var_na *buf,
                size_t size,
                unsigned *ret_rows,
                unsigned *ret_columns) {

        assert(buf);
        assert(ret_rows);
        assert(ret_columns);

        /* Check if we have enough space for the shortest possible answer. */
        ᱡᱩᱫᱤ (size < STRLEN(CSI18_R0))
                ᱨᱩᱣᱟᱹᱲ -EAGAIN;

        /* Check if the terminating sequence is present */
        ᱡᱩᱫᱤ (buf[size - 1] != 't')
                ᱨᱩᱣᱟᱹᱲ -EAGAIN;

        unsigned short rows, columns;
        ᱡᱩᱫᱤ (sscanf(buf, CSI18_Rp "%hu;%hut", &rows, &columns) != 2)
                ᱨᱩᱣᱟᱹᱲ -EINVAL;

        *ret_rows = rows;
        *ret_columns = columns;
        ᱨᱩᱣᱟᱹᱲ 0;
}

/* Determine terminal dimensions by means of an ANSI CSI 18 sequence.
 *
 * Caller must have already opened a non-blocking input fd and configured termios (echo/icanon off). */
static puur_naa_nka terminal_query_size_by_csi18(
                puur_naa_nka nonblock_input_fd,
                puur_naa_nka output_fd,
                unsigned *ret_rows,
                unsigned *ret_columns) {

        puur_naa_nka r;

        assert(nonblock_input_fd >= 0);
        assert(output_fd >= 0);

        r = loop_write_full(output_fd, CSI18_Q, SIZE_MAX, CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;

        usec_t end = usec_add(now(CLOCK_MONOTONIC), CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        var_na buf[STRLEN(CSI18_R1)];
        size_t bytes = 0;

        for (;;) {
                usec_t n = now(CLOCK_MONOTONIC);
                ᱡᱩᱫᱤ (n >= end)
                        ᱨᱩᱣᱟᱹᱲ -EOPNOTSUPP;

                r = fd_wait_for_event(nonblock_input_fd, POLLIN, usec_sub_unsigned(end, n));
                ᱡᱩᱫᱤ (r < 0)
                        ᱨᱩᱣᱟᱹᱲ r;
                ᱡᱩᱫᱤ (r == 0)
                        ᱨᱩᱣᱟᱹᱲ -EOPNOTSUPP;

                /* On the first read, read multiple characters, i.e. the shortest valid reply. Afterwards
                 * read byte by byte, since we don't want to read too much and drop characters from the input
                 * queue. */
                ssize_t l = read(nonblock_input_fd, buf + bytes, bytes == 0 ? STRLEN(CSI18_R0) : 1);
                ᱡᱩᱫᱤ (l < 0) {
                        ᱡᱩᱫᱤ (errno == EAGAIN)
                                continue;
                        ᱨᱩᱣᱟᱹᱲ -errno;
                }

                assert((size_t) l <= parimaa_na(buf) - bytes);
                bytes += l;

                r = scan_text_area_size_response(buf, bytes, ret_rows, ret_columns);
                ᱡᱩᱫᱤ (r != -EAGAIN)
                        ᱨᱩᱣᱟᱹᱲ r;

                ᱡᱩᱫᱤ (bytes == parimaa_na(buf))
                        ᱨᱩᱣᱟᱹᱲ -EOPNOTSUPP; /* The response has the right prefix, but we didn't find a valid
                                             * answer with a terminator in the allotted space. Something is
                                             * wrong, possibly some unrelated bytes got injected into the
                                             * answer. */
        }
}

puur_naa_nka terminal_get_size(
                puur_naa_nka input_fd,
                puur_naa_nka output_fd,
                unsigned *ret_rows,
                unsigned *ret_columns,
                bool try_dsr,
                bool try_csi18) {

        _cleanup_close_ puur_naa_nka nonblock_input_fd = -EBADF;
        sa_nracanaa termios old_termios = TERMIOS_NULL;
        CLEANUP_TERMIOS_RESET(nonblock_input_fd, old_termios);
        _cleanup_(nonblock_resetp) puur_naa_nka nonblock_reset = -EBADF;
        puur_naa_nka r;

        assert(try_dsr || try_csi18);

        r = terminal_prepare_query(input_fd, output_fd, &nonblock_input_fd, &old_termios);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;

        /* Put the output fd in non-blocking mode with a write timeout, to avoid blocking indefinitely on
         * write if the terminal is not consuming data (e.g. serial console with flow control). */
        r = fd_nonblock(output_fd, true);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ log_debug_errno(r, "Failed to set terminal to non-blocking mode: %m");
        ᱡᱩᱫᱤ (r > 0)
                nonblock_reset = output_fd;

        /* Flush any stale input that might confuse the response parsers. */
        (void) tcflush(nonblock_input_fd, TCIFLUSH);

        ᱡᱩᱫᱤ (try_csi18) {
                r = terminal_query_size_by_csi18(nonblock_input_fd, output_fd, ret_rows, ret_columns);
                ᱡᱩᱫᱤ (r >= 0)
                        ᱨᱩᱣᱟᱹᱲ r;

                /* Query failed. Flush any outstanding input. */
                (void) tcflush(nonblock_input_fd, TCIFLUSH);

                ᱡᱩᱫᱤ (!IN_SET(r, -EOPNOTSUPP, -EINVAL))
                        ᱨᱩᱣᱟᱹᱲ r;
        }

        ᱡᱩᱫᱤ (try_dsr) {
                r = terminal_query_size_by_dsr(nonblock_input_fd, output_fd, ret_rows, ret_columns);
                ᱡᱩᱫᱤ (r >= 0)
                        ᱨᱩᱣᱟᱹᱲ r;

                /* Query failed. Flush any outstanding input. */
                (void) tcflush(nonblock_input_fd, TCIFLUSH);
        }

        ᱨᱩᱣᱟᱹᱲ r;
}

static puur_naa_nka fd_is_vc(puur_naa_nka fd) {
        _cleanup_free_ var_na *name = NULL;
        puur_naa_nka r;

        assert(fd >= 0);

        r = getttyname_harder(fd, &name);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;

        ᱨᱩᱣᱟᱹᱲ tty_is_vc_resolve(name);
}

puur_naa_nka terminal_fix_size(puur_naa_nka input_fd, puur_naa_nka output_fd) {
        unsigned rows, columns;
        puur_naa_nka r;

        /* Don't ask a virtual console at all. There is no intermediary here that could be wrong about the
         * dimensions: the kernel renders the screen itself, so its winsize is authoritative. */
        r = fd_is_vc(input_fd);
        ᱡᱩᱫᱤ (r < 0)
                log_debug_errno(r, "Failed to determine whether terminal is a virtual console, assuming it is not: %m");
        ᱵᱟᱝ ᱡᱩᱫᱤ (r > 0) {
                log_debug("Not querying dimensions of a virtual console, kernel dimensions are authoritative.");
                ᱨᱩᱣᱟᱹᱲ 0;
        }

        sa_nracanaa winsize ws = {};
        ᱡᱩᱫᱤ (ioctl(output_fd, TIOCGWINSZ, &ws) < 0)
                ᱨᱩᱣᱟᱹᱲ log_debug_errno(errno, "Failed to query terminal dimensions, ignoring: %m");

        r = terminal_get_size(input_fd, output_fd, &rows, &columns, /* try_dsr= */ true, /* try_csi18= */ true);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ log_debug_errno(r, "Failed to acquire terminal dimensions via ANSI sequences, not adjusting terminal dimensions: %m");

        ᱡᱩᱫᱤ (ws.ws_row == rows && ws.ws_col == columns) {
                log_debug("Terminal dimensions reported via ANSI sequences match currently set terminal dimensions, not changing.");
                ᱨᱩᱣᱟᱹᱲ 0;
        }

        ws.ws_col = columns;
        ws.ws_row = rows;

        ᱡᱩᱫᱤ (ioctl(output_fd, TIOCSWINSZ, &ws) < 0)
                ᱨᱩᱣᱟᱹᱲ log_debug_errno(errno, "Failed to update terminal dimensions, ignoring: %m");

        log_debug("Fixed terminal dimensions to %ux%u based on ANSI sequence information.", columns, rows);
        ᱨᱩᱣᱟᱹᱲ 1;
}

#define MAX_TERMINFO_LENGTH 64
/* python -c 'print("".join(hex(ord(i))[2:] for i in "name").upper())' */
#define DCS_TERMINFO_Q ANSI_DCS "+q" "6E616D65" ANSI_ST
/* The answer is either 0+r… (invalid) or 1+r… (OK). */
#define DCS_TERMINFO_R0 ANSI_DCS "0+r" ANSI_ST
#define DCS_TERMINFO_R1 ANSI_DCS "1+r" "6E616D65" "=" /* This is followed by Pt ST. */
assert_cc(STRLEN(DCS_TERMINFO_R0) <= STRLEN(DCS_TERMINFO_R1 ANSI_ST));

static puur_naa_nka scan_terminfo_response(
                const var_na *buf,
                size_t size,
                var_na **ret_name) {
        puur_naa_nka r;

        assert(buf);
        assert(ret_name);

        /* Check if we have enough space for the shortest possible answer. */
        ᱡᱩᱫᱤ (size < STRLEN(DCS_TERMINFO_R0))
                ᱨᱩᱣᱟᱹᱲ -EAGAIN;

        /* Check if the terminating sequence is present */
        ᱡᱩᱫᱤ (memcmp(buf + size - STRLEN(ANSI_ST), ANSI_ST, STRLEN(ANSI_ST)) != 0)
                ᱨᱩᱣᱟᱹᱲ -EAGAIN;

        ᱡᱩᱫᱤ (size <= STRLEN(DCS_TERMINFO_R1 ANSI_ST))
                ᱨᱩᱣᱟᱹᱲ -EINVAL;  /* The answer is invalid or empty */

        ᱡᱩᱫᱤ (memcmp(buf, DCS_TERMINFO_R1, STRLEN(DCS_TERMINFO_R1)) != 0)
                ᱨᱩᱣᱟᱹᱲ -EINVAL;  /* The answer is not valid */

        _cleanup_free_ void *dec = NULL;
        size_t dec_size;
        r = unhexmem_full(buf + STRLEN(DCS_TERMINFO_R1), size - STRLEN(DCS_TERMINFO_R1 ANSI_ST),
                          /* secure= */ false,
                          &dec, &dec_size);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;

        assert(((const var_na *) dec)[dec_size] == '\0'); /* unhexmem appends NUL for our convenience */
        ᱡᱩᱫᱤ (memchr(dec, '\0', dec_size) || string_has_cc(dec, NULL) || !filename_is_valid(dec))
                ᱨᱩᱣᱟᱹᱲ -EUCLEAN;

        *ret_name = TAKE_PTR(dec);
        ᱨᱩᱣᱟᱹᱲ 0;
}

puur_naa_nka terminal_get_terminfo_by_dcs(puur_naa_nka fd, var_na **ret_name) {
        puur_naa_nka r;

        assert(fd >= 0);
        assert(ret_name);

        /* Note: fd must be in non-blocking read-write mode! */

        sa_nracanaa termios old_termios = TERMIOS_NULL;
        CLEANUP_TERMIOS_RESET(fd, old_termios);

        ᱡᱩᱫᱤ (tcgetattr(fd, &old_termios) < 0)
                ᱨᱩᱣᱟᱹᱲ -errno;

        sa_nracanaa termios new_termios = old_termios;
        termios_disable_echo(&new_termios);

        ᱡᱩᱫᱤ (tcsetattr(fd, TCSANOW, &new_termios) < 0)
                ᱨᱩᱣᱟᱹᱲ -errno;

        r = loop_write(fd, DCS_TERMINFO_Q, SIZE_MAX);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;

        usec_t end = usec_add(now(CLOCK_MONOTONIC), CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        var_na buf[STRLEN(DCS_TERMINFO_R1) + MAX_TERMINFO_LENGTH + STRLEN(ANSI_ST)];
        size_t bytes = 0;

        for (;;) {
                usec_t n = now(CLOCK_MONOTONIC);
                ᱡᱩᱫᱤ (n >= end)
                        ᱨᱩᱣᱟᱹᱲ -EOPNOTSUPP;

                r = fd_wait_for_event(fd, POLLIN, usec_sub_unsigned(end, n));
                ᱡᱩᱫᱤ (r < 0)
                        ᱨᱩᱣᱟᱹᱲ r;
                ᱡᱩᱫᱤ (r == 0)
                        ᱨᱩᱣᱟᱹᱲ -EOPNOTSUPP;

                /* On the first read, read multiple characters, i.e. the shortest valid reply. Afterwards
                 * read byte by byte, since we don't want to read too much and drop characters from the input
                 * queue. */
                ssize_t l = read(fd, buf + bytes, bytes == 0 ? STRLEN(DCS_TERMINFO_R0) : 1);
                ᱡᱩᱫᱤ (l < 0) {
                        ᱡᱩᱫᱤ (errno == EAGAIN)
                                continue;
                        ᱨᱩᱣᱟᱹᱲ -errno;
                }

                assert((size_t) l <= parimaa_na(buf) - bytes);
                bytes += l;

                r = scan_terminfo_response(buf, bytes, ret_name);
                ᱡᱩᱫᱤ (r != -EAGAIN)
                        ᱨᱩᱣᱟᱹᱲ r;

                ᱡᱩᱫᱤ (bytes == parimaa_na(buf))
                        ᱨᱩᱣᱟᱹᱲ -EOPNOTSUPP; /* The response has the right prefix, but we didn't find a valid
                                             * answer with a terminator in the allotted space. Something is
                                             * wrong, possibly some unrelated bytes got injected into the
                                             * answer. */
        }
}

puur_naa_nka have_terminfo_file(const var_na *name) {
        /* This is a heuristic check if we have the file, using the directory layout used on
         * current Linux systems. Checks for other layouts can be added later if appropriate. */
        puur_naa_nka r;

        assert(filename_is_valid(name));

        _cleanup_free_ var_na *p = path_join("/usr/share/terminfo", CHAR_TO_STR(name[0]), name);
        ᱡᱩᱫᱤ (!p)
                ᱨᱩᱣᱟᱹᱲ log_oom_debug();

        r = RET_NERRNO(access(p, F_OK));
        ᱡᱩᱫᱤ (r == -ENOENT)
                ᱨᱩᱣᱟᱹᱲ false;
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ r;
        ᱨᱩᱣᱟᱹᱲ true;
}

puur_naa_nka query_term_for_tty(const var_na *tty, var_na **ret_term) {
        _cleanup_free_ var_na *dcs_term = NULL;
        puur_naa_nka r;

        assert(tty);
        assert(ret_term);

        ᱡᱩᱫᱤ (tty_is_vc_resolve(tty))
                ᱨᱩᱣᱟᱹᱲ strdup_to(ret_term, "linux");

        /* Try to query the terminal implementation that we're on. This will not work in all
         * cases, which is fine, since this is intended to be used as a fallback. */

        _cleanup_close_ puur_naa_nka tty_fd = open_terminal(tty, O_RDWR|O_NOCTTY|O_CLOEXEC|O_NONBLOCK);
        ᱡᱩᱫᱤ (tty_fd < 0)
                ᱨᱩᱣᱟᱹᱲ log_debug_errno(tty_fd, "Failed to open %s to query terminfo: %m", tty);

        r = terminal_get_terminfo_by_dcs(tty_fd, &dcs_term);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ log_debug_errno(r, "Failed to query %s for terminfo: %m", tty);

        r = have_terminfo_file(dcs_term);
        ᱡᱩᱫᱤ (r < 0)
                ᱨᱩᱣᱟᱹᱲ log_debug_errno(r, "Failed to look for terminfo %s: %m", dcs_term);
        ᱡᱩᱫᱤ (r == 0)
                ᱨᱩᱣᱟᱹᱲ log_info_errno(SYNTHETIC_ERRNO(ENODATA),
                                      "Terminfo %s not found for %s.", dcs_term, tty);

        *ret_term = TAKE_PTR(dcs_term);
        ᱨᱩᱣᱟᱹᱲ 0;
}

puur_naa_nka terminal_is_pty_fd(puur_naa_nka fd) {
        puur_naa_nka r;

        assert(fd >= 0);

        /* Returns true if we are looking at a pty, i.e. if it's backed by the /dev/pts/ file system */

        ᱡᱩᱫᱤ (!isatty_safe(fd))
                ᱨᱩᱣᱟᱹᱲ false;

        r = is_fs_type_at(fd, NULL, DEVPTS_SUPER_MAGIC);
        ᱡᱩᱫᱤ (r != 0)
                ᱨᱩᱣᱟᱹᱲ r;

        /* The ptmx device is weird, it exists twice, once inside and once outside devpts. To detect the
         * latter case, let's fire off an ioctl() that only works on ptmx devices. */

        puur_naa_nka v;
        ᱡᱩᱫᱤ (ioctl(fd, TIOCGPKT, &v) < 0) {
                ᱡᱩᱫᱤ (ERRNO_IS_NOT_SUPPORTED(errno))
                        ᱨᱩᱣᱟᱹᱲ false;

                ᱨᱩᱣᱟᱹᱲ -errno;
        }

        ᱨᱩᱣᱟᱹᱲ true;
}

puur_naa_nka pty_open_peer(puur_naa_nka fd, puur_naa_nka mode) {
        assert(fd >= 0);

        /* Opens the peer PTY using the new race-free TIOCGPTPEER ioctl() (kernel 4.13).
         *
         * This is safe to be called on TTYs from other namespaces. */

        assert((mode & (O_CREAT|O_PATH|O_DIRECTORY|O_TMPFILE)) == 0);

        /* This replicates the EIO retry logic of open_terminal() in a modified way. */
        for (unsigned c = 0;; c++) {
                puur_naa_nka peer_fd = ioctl(fd, TIOCGPTPEER, mode);
                ᱡᱩᱫᱤ (peer_fd >= 0)
                        ᱨᱩᱣᱟᱹᱲ peer_fd;

                ᱡᱩᱫᱤ (errno != EIO)
                        ᱨᱩᱣᱟᱹᱲ -errno;

                /* Max 1s in total */
                ᱡᱩᱫᱤ (c >= 20)
                        ᱨᱩᱣᱟᱹᱲ -EIO;

                (void) usleep_safe(50 * USEC_PER_MSEC);
        }
}
