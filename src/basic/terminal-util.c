/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ <fcntl.h>
#അന്തര്ഭാവ <linux/kd.h>
#അന്തര്ഭാവ <linux/magic.h>
#അന്തര്ഭാവ <linux/tiocl.h>
#അന്തര്ഭാവ <linux/vt.h>
#അന്തര്ഭാവ <poll.h>
#അന്തര്ഭാവ <signal.h>
#അന്തര്ഭാവ <stdlib.h>
#അന്തര്ഭാവ <sys/inotify.h>
#അന്തര്ഭാവ <sys/ioctl.h>
#അന്തര്ഭാവ <sys/sysmacros.h>
#അന്തര്ഭാവ <termios.h>
#അന്തര്ഭാവ <time.h>
#അന്തര്ഭാവ <unistd.h>

#അന്തര്ഭാവ "alloc-util.h"
#അന്തര്ഭാവ "ansi-color.h"
#അന്തര്ഭാവ "chase.h"
#അന്തര്ഭാവ "devnum-util.h"
#അന്തര്ഭാവ "errno-util.h"
#അന്തര്ഭാവ "extract-word.h"
#അന്തര്ഭാവ "fd-util.h"
#അന്തര്ഭാവ "fileio.h"
#അന്തര്ഭാവ "fs-util.h"
#അന്തര്ഭാവ "hexdecoct.h"
#അന്തര്ഭാവ "inotify-util.h"
#അന്തര്ഭാവ "io-util.h"
#അന്തര്ഭാവ "log.h"
#അന്തര്ഭാവ "namespace-util.h"
#അന്തര്ഭാവ "parse-util.h"
#അന്തര്ഭാവ "path-util.h"
#അന്തര്ഭാവ "pidref.h"
#അന്തര്ഭാവ "proc-cmdline.h"
#അന്തര്ഭാവ "process-util.h"
#അന്തര്ഭാവ "signal-util.h"
#അന്തര്ഭാവ "socket-util.h"
#അന്തര്ഭാവ "stat-util.h"
#അന്തര്ഭാവ "stdio-util.h"
#അന്തര്ഭാവ "string-util.h"
#അന്തര്ഭാവ "strv.h"
#അന്തര്ഭാവ "terminal-util.h"
#അന്തര്ഭാവ "time-util.h"
#അന്തര്ഭാവ "utf8.h"

/* How much to wait when reading/writing ANSI sequences from/to the console */
#define CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC (333 * USEC_PER_MSEC)

സ്ഥിതം അസ്ഥിര unsigned cached_columns = 0;
സ്ഥിതം അസ്ഥിര unsigned cached_വരികൾ = 0;

സ്ഥിതം അസ്ഥിര പൂർണ്ണസംഖ്യ cached_on_tty = -1;
സ്ഥിതം അസ്ഥിര പൂർണ്ണസംഖ്യ cached_on_dev_null = -1;

bool isatty_safe(പൂർണ്ണസംഖ്യ fd) {
        പ്രതിജ്ഞാ(fd >= 0);

        എങ്കിൽ (isatty(fd))
                തിരികെ true;

        /* Linux/glibc returns EIO for hung up TTY on isatty(). Which is wrong, the thing doesn't stop being
         * a TTY after all, just because it is temporarily hung up. Let's work around this here, until this
         * is fixed in glibc. See: https://sourceware.org/bugzilla/show_bug.cgi?id=32103 */
        എങ്കിൽ (errno == EIO)
                തിരികെ true;

        /* Be resilient if we're working on stdio, since they're set up by parent process. */
        പ്രതിജ്ഞാ(errno != EBADF || IN_SET(fd, STDIN_FILENO, STDOUT_FILENO, STDERR_FILENO));

        തിരികെ false;
}

പൂർണ്ണസംഖ്യ chvt(പൂർണ്ണസംഖ്യ vt) {
        _cleanup_close_ പൂർണ്ണസംഖ്യ fd = -EBADF;

        /* Switch to the specified vt number. If the VT is specified <= 0 switch to the VT the kernel log messages go,
         * if that's configured. */

        fd = open_terminal("/dev/tty0", O_RDWR|O_NOCTTY|O_CLOEXEC|O_NONBLOCK);
        എങ്കിൽ (fd < 0)
                തിരികെ fd;

        എങ്കിൽ (vt <= 0) {
                പൂർണ്ണസംഖ്യ tiocl[2] = {
                        TIOCL_GETKMSGREDIRECT,
                        0
                };

                എങ്കിൽ (ioctl(fd, TIOCLINUX, tiocl) < 0)
                        തിരികെ -errno;

                vt = tiocl[0] <= 0 ? 1 : tiocl[0];
        }

        തിരികെ RET_NERRNO(ioctl(fd, VT_ACTIVATE, vt));
}

പൂർണ്ണസംഖ്യ read_one_char(FILE *f, അക്ഷരം *ret, usec_t t, bool echo, bool *need_nl) {
        _cleanup_free_ അക്ഷരം *വരി = ശൂന്യ;
        ഘടന termios old_termios;
        പൂർണ്ണസംഖ്യ r, fd;

        പ്രതിജ്ഞാ(ret);

        എങ്കിൽ (!f)
                f = stdin;

        /* If this is a terminal, then switch canonical mode off, so that we can read a single
         * character. (Note that fmemopen() streams do not have an fd associated with them, let's handle that
         * nicely.) If 'echo' is false we'll also disable ECHO mode so that the pressed key is not made
         * visible to the user. */
        fd = fileno(f);
        എങ്കിൽ (fd >= 0 && tcgetattr(fd, &old_termios) >= 0) {
                ഘടന termios new_termios = old_termios;

                new_termios.c_lflag &= ~(ICANON|(echo ? 0 : ECHO));
                new_termios.c_cc[VMIN] = 1;
                new_termios.c_cc[VTIME] = 0;

                എങ്കിൽ (tcsetattr(fd, TCSANOW, &new_termios) >= 0) {
                        അക്ഷരം c;

                        എങ്കിൽ (t != USEC_INFINITY) {
                                എങ്കിൽ (fd_wait_for_event(fd, POLLIN, t) <= 0) {
                                        (ശൂന്യം) tcsetattr(fd, TCSANOW, &old_termios);
                                        തിരികെ -ETIMEDOUT;
                                }
                        }

                        r = safe_fgetc(f, &c);
                        (ശൂന്യം) tcsetattr(fd, TCSANOW, &old_termios);
                        എങ്കിൽ (r < 0)
                                തിരികെ r;
                        എങ്കിൽ (r == 0)
                                തിരികെ -EIO;

                        എങ്കിൽ (need_nl)
                                *need_nl = c != '\n';

                        *ret = c;
                        തിരികെ 0;
                }
        }

        എങ്കിൽ (t != USEC_INFINITY && fd >= 0) {
                /* Let's wait the specified amount of time for input. When we have no fd we skip this, under
                 * the assumption that this is an fmemopen() stream or so where waiting doesn't make sense
                 * anyway, as the data is either already in the stream or cannot possible be placed there
                 * while we access the stream */

                എങ്കിൽ (fd_wait_for_event(fd, POLLIN, t) <= 0)
                        തിരികെ -ETIMEDOUT;
        }

        /* If this is not a terminal, then read a full line instead */

        r = read_വരി(f, 16, &വരി); /* longer than necessary, to eat up UTF-8 chars/vt100 key sequences */
        എങ്കിൽ (r < 0)
                തിരികെ r;
        എങ്കിൽ (r == 0)
                തിരികെ -EIO;

        എങ്കിൽ (strlen(വരി) != 1)
                തിരികെ -EBADMSG;

        എങ്കിൽ (need_nl)
                *need_nl = false;

        *ret = വരി[0];
        തിരികെ 0;
}

#define DEFAULT_ASK_REFRESH_USEC (2*USEC_PER_SEC)

പൂർണ്ണസംഖ്യ ask_char(അക്ഷരം *ret, സ്ഥിരം അക്ഷരം *replies, സ്ഥിരം അക്ഷരം *fmt, ...) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(ret);
        പ്രതിജ്ഞാ(replies);
        പ്രതിജ്ഞാ(fmt);

        വേണ്ടി (;;) {
                va_list ap;
                അക്ഷരം c;
                bool need_nl = true;

                fputs(ansi_highlight(), stdout);

                putchar('\r');

                va_start(ap, fmt);
                vprintf(fmt, ap);
                va_end(ap);

                fputs(ansi_normal(), stdout);

                fflush(stdout);

                r = read_one_char(stdin, &c, DEFAULT_ASK_REFRESH_USEC, /* echo= */ true, &need_nl);
                എങ്കിൽ (r < 0) {

                        എങ്കിൽ (r == -ETIMEDOUT)
                                തുടരുക;

                        എങ്കിൽ (r == -EBADMSG) {
                                puts("Bad input, please try again.");
                                തുടരുക;
                        }

                        putchar('\n');
                        തിരികെ r;
                }

                എങ്കിൽ (need_nl)
                        putchar('\n');

                എങ്കിൽ (strchr(replies, c)) {
                        *ret = c;
                        തിരികെ 0;
                }

                puts("Read unexpected character, please try again.");
        }
}

തരനിർവചനം എണ്ണൽ CompletionResult{
        COMPLETION_ALREADY,       /* the input string is already complete */
        COMPLETION_FULL,          /* completed the input string to be complete now */
        COMPLETION_PARTIAL,       /* completed the input string so that is still incomplete */
        COMPLETION_NONE,          /* found no matching completion */
        _COMPLETION_RESULT_MAX,
        _COMPLETION_RESULT_INVALID = -EINVAL,
        _COMPLETION_RESULT_ERRNO_MAX = -ERRNO_MAX,
} CompletionResult;

സ്ഥിതം CompletionResult pick_completion(സ്ഥിരം അക്ഷരം *string, അക്ഷരം *സ്ഥിരം*completions, അക്ഷരം **ret) {
        _cleanup_free_ അക്ഷരം *found = ശൂന്യ;
        bool partial = false;

        പ്രതിജ്ഞാ(ret);

        string = strempty(string);

        STRV_FOREACH(c, completions) {

                /* Ignore entries that are not actually completions */
                എങ്കിൽ (!startswith(*c, string))
                        തുടരുക;

                /* Store first completion that matches */
                എങ്കിൽ (!found) {
                        found = strdup(*c);
                        എങ്കിൽ (!found)
                                തിരികെ -ENOMEM;

                        തുടരുക;
                }

                /* If there's another completion that works truncate the one we already found by common
                 * prefix */
                size_t n = str_common_prefix(found, *c);
                എങ്കിൽ (n == SIZE_MAX)
                        തുടരുക;

                found[n] = 0;
                partial = true;
        }

        *ret = TAKE_PTR(found);

        എങ്കിൽ (!*ret)
                തിരികെ COMPLETION_NONE;
        എങ്കിൽ (partial)
                തിരികെ COMPLETION_PARTIAL;

        തിരികെ streq(string, *ret) ? COMPLETION_ALREADY : COMPLETION_FULL;
}

സ്ഥിതം ശൂന്യം clear_by_backspace(size_t n) {
        /* Erase the specified number of character cells backwards on the terminal */
        വേണ്ടി (size_t i = 0; i < n; i++)
                fputs("\b \b", stdout);
}

പൂർണ്ണസംഖ്യ ask_string_full(
                അക്ഷരം **ret,
                സ്ഥിരം അക്ഷരം *prefill,
                GetCompletionsCallback get_completions,
                ശൂന്യം *userdata,
                സ്ഥിരം അക്ഷരം *text, ...) {

        va_list ap;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(ret);
        പ്രതിജ്ഞാ(text);

        _cleanup_free_ അക്ഷരം *string = ശൂന്യ;
        size_t n = 0;

        എങ്കിൽ (prefill) {
                /* Prefill query with explicit data if specified */

                string = strdup(prefill);
                എങ്കിൽ (!string)
                        തിരികെ -ENOMEM;

                n = strlen(string);

        } അല്ലെങ്കിൽ എങ്കിൽ (get_completions) {
                /* Otherwise, figure out what string to preselect the query with */
                _cleanup_strv_free_ അക്ഷരം **completions = ശൂന്യ;
                r = get_completions("", GET_COMPLETIONS_PRESELECT, &completions, userdata);
                എങ്കിൽ (r < 0)
                        തിരികെ r;

                CompletionResult cr = pick_completion(string, completions, &string);
                എങ്കിൽ (cr < 0)
                        തിരികെ cr;

                n = strlen_ptr(string);
        }

        /* Output the prompt */
        fputs(ansi_highlight(), stdout);
        va_start(ap, text);
        vprintf(text, ap);
        va_end(ap);
        fputs(ansi_normal(), stdout);
        എങ്കിൽ (string)
                fputs(string, stdout);
        fflush(stdout);

        /* Do interactive logic only if stdin + stdout are connected to the same place. And yes, we could use
         * STDIN_FILENO and STDOUT_FILENO here, but let's be overly correct for once, after all libc allows
         * swapping out stdin/stdout. */
        പൂർണ്ണസംഖ്യ fd_നിവേശം = fileno(stdin);
        പൂർണ്ണസംഖ്യ fd_output = fileno(stdout);
        ഘടന termios old_termios = TERMIOS_NULL;
        CLEANUP_TERMIOS_RESET(fd_നിവേശം, old_termios);

        എങ്കിൽ (fd_നിവേശം < 0 || fd_output < 0 || same_fd(fd_നിവേശം, fd_output) <= 0)
                പോകുക fallback;

        /* Try to disable echo, which also tells us if this even is a terminal */
        എങ്കിൽ (tcgetattr(fd_നിവേശം, &old_termios) < 0) {
                old_termios = TERMIOS_NULL;
                പോകുക fallback;
        }

        ഘടന termios new_termios = old_termios;
        termios_disable_echo(&new_termios);
        എങ്കിൽ (tcsetattr(fd_നിവേശം, TCSANOW, &new_termios) < 0)
                തിരികെ -errno;

        വേണ്ടി (;;) {
                പൂർണ്ണസംഖ്യ c = fgetc(stdin);

                /* On EOF or NUL, end the request, don't output anything anymore */
                എങ്കിൽ (IN_SET(c, EOF, 0))
                        നിർത്തുക;

                /* On Return also end the request, but make this visible */
                എങ്കിൽ (IN_SET(c, '\n', '\r')) {
                        fputc('\n', stdout);
                        നിർത്തുക;
                }

                എങ്കിൽ (c == '\t') {
                        /* Tab */

                        _cleanup_strv_free_ അക്ഷരം **completions = ശൂന്യ;
                        എങ്കിൽ (get_completions) {
                                r = get_completions(string, /* flags= */ 0, &completions, userdata);
                                എങ്കിൽ (r < 0)
                                        തിരികെ r;
                        }

                        _cleanup_free_ അക്ഷരം *new_string = ശൂന്യ;
                        CompletionResult cr = pick_completion(string, completions, &new_string);
                        എങ്കിൽ (cr < 0)
                                തിരികെ cr;
                        എങ്കിൽ (IN_SET(cr, COMPLETION_PARTIAL, COMPLETION_FULL)) {
                                /* Output the new suffix we learned */
                                fputs(ASSERT_PTR(startswith(new_string, strempty(string))), stdout);

                                /* And update the whole string */
                                free_and_replace(string, new_string);
                                n = strlen(string);
                        }
                        എങ്കിൽ (cr == COMPLETION_NONE)
                                fputc('\a', stdout); /* BEL */

                        എങ്കിൽ (IN_SET(cr, COMPLETION_PARTIAL, COMPLETION_ALREADY)) {
                                /* If this worked only partially, or if the user hit TAB even though we were
                                 * complete already, then show the remaining options (in the latter case just
                                 * the one). */
                                fputc('\n', stdout);

                                _cleanup_strv_free_ അക്ഷരം **filtered = strv_filter_prefix(completions, string);
                                എങ്കിൽ (!filtered)
                                        തിരികെ -ENOMEM;

                                r = show_menu(filtered,
                                              /* n_columns= */ SIZE_MAX,
                                              /* column_width= */ SIZE_MAX,
                                              /* ellipsize_percentage= */ 0,
                                              /* grey_prefix= */ string,
                                              /* with_numbers= */ false);
                                എങ്കിൽ (r < 0)
                                        തിരികെ r;

                                /* Show the prompt again */
                                fputs(ansi_highlight(), stdout);
                                va_start(ap, text);
                                vprintf(text, ap);
                                va_end(ap);
                                fputs(ansi_normal(), stdout);
                                fputs(string, stdout);
                        }

                } അല്ലെങ്കിൽ എങ്കിൽ (IN_SET(c, '\b', 127)) {
                        /* Backspace */

                        എങ്കിൽ (n == 0)
                                fputc('\a', stdout); /* BEL */
                        അല്ലെങ്കിൽ {
                                size_t m = utf8_last_length(string, n);

                                അക്ഷരം *e = string + n - m;
                                clear_by_backspace(utf8_console_width(e));

                                *e = 0;
                                n -= m;
                        }

                } അല്ലെങ്കിൽ എങ്കിൽ (c == 21) {
                        /* Ctrl-u → erase all input */

                        clear_by_backspace(utf8_console_width(string));
                        എങ്കിൽ (string)
                                string[n = 0] = 0;
                        അല്ലെങ്കിൽ
                                പ്രതിജ്ഞാ(n == 0);

                } അല്ലെങ്കിൽ എങ്കിൽ (c == 4) {
                        /* Ctrl-d → cancel this field input */

                        തിരികെ -ECANCELED;

                } അല്ലെങ്കിൽ എങ്കിൽ (char_is_cc(c) || n >= LINE_MAX)
                        /* refuse control characters and too long strings */
                        fputc('\a', stdout); /* BEL */
                അല്ലെങ്കിൽ {
                        /* Regular char */

                        എങ്കിൽ (!GREEDY_REALLOC(string, n+2))
                                തിരികെ -ENOMEM;

                        string[n++] = (അക്ഷരം) c;
                        string[n] = 0;

                        fputc(c, stdout);
                }

                fflush(stdout);
        }

        എങ്കിൽ (!string) {
                string = strdup("");
                എങ്കിൽ (!string)
                        തിരികെ -ENOMEM;
        }

        *ret = TAKE_PTR(string);
        തിരികെ 0;

fallback:
        /* A simple fallback without TTY magic */
        string = mfree(string);
        r = read_വരി(stdin, LONG_വരി_MAX, &string);
        എങ്കിൽ (r < 0)
                തിരികെ r;
        എങ്കിൽ (r == 0)
                തിരികെ -EIO;

        *ret = TAKE_PTR(string);
        തിരികെ 0;
}

bool any_താക്കോൽ_to_proceed(ശൂന്യം) {

        /* Insert a new line here as well as to when the user inputs, as this is also used during the boot up
         * sequence when status messages may be interleaved with the current program output. This ensures
         * that the status messages aren't appended on the same line as this message. */

        fputc('\n', stdout);
        fputs(ansi_highlight_magenta(), stdout);
        fputs("-- Press any key to proceed --", stdout);
        fputs(ansi_normal(), stdout);
        fputc('\n', stdout);
        fflush(stdout);

        അക്ഷരം താക്കോൽ = 0;
        (ശൂന്യം) read_one_char(stdin, &താക്കോൽ, USEC_INFINITY, /* echo= */ false, /* need_nl= */ ശൂന്യ);

        fputc('\n', stdout);
        fflush(stdout);

        തിരികെ താക്കോൽ != 'q';
}

സ്ഥിതം size_t widest_list_element(അക്ഷരം *സ്ഥിരം*l) {
        size_t w = 0;

        /* Returns the largest console width of all elements in 'l' */

        STRV_FOREACH(i, l)
                w = MAX(w, utf8_console_width(*i));

        തിരികെ w;
}

പൂർണ്ണസംഖ്യ show_menu(അക്ഷരം **x,
              size_t n_columns,
              size_t column_width,
              unsigned ellipsize_percentage,
              സ്ഥിരം അക്ഷരം *grey_prefix,
              bool with_numbers) {

        പ്രതിജ്ഞാ(n_columns > 0);
        പ്രതിജ്ഞാ(column_width > 0);

        എങ്കിൽ (n_columns == SIZE_MAX)
                n_columns = 3;

        എങ്കിൽ (column_width == SIZE_MAX) {
                size_t widest = widest_list_element(x);

                /* If not specified, derive column width from screen width */
                size_t column_max = (columns()-1) / n_columns;

                /* Subtract room for numbers */
                എങ്കിൽ (with_numbers)
                        column_max = LESS_BY(column_max, 6U);

                /* If columns would get too tight let's make this a linear list instead. */
                എങ്കിൽ (column_max < 10 && widest > 10) {
                        n_columns = 1;
                        column_max = columns()-1;

                        എങ്കിൽ (with_numbers)
                                column_max = LESS_BY(column_max, 6U);
                }

                /* Never make this narrower than 10 characters */
                column_max = MAX(column_max, 10U);
                column_width = CLAMP(widest+1, 10U, column_max);
        }

        size_t n = strv_length(x);
        size_t per_column = DIV_ROUND_UP(n, n_columns);

        size_t break_വരികൾ = വരികൾ();
        എങ്കിൽ (break_വരികൾ > 2)
                break_വരികൾ--;

        /* The first page gets two extra lines, since we want to show a title */
        size_t break_modulo = break_വരികൾ;
        എങ്കിൽ (break_modulo > 3)
                break_modulo -= 3;

        വേണ്ടി (size_t i = 0; i < per_column; i++) {

                വേണ്ടി (size_t j = 0; j < n_columns; j++) {
                        _cleanup_free_ അക്ഷരം *e = ശൂന്യ;

                        size_t p = j * per_column + i;
                        എങ്കിൽ (p >= n)
                                നിർത്തുക;

                        e = ellipsize(x[p], column_width, ellipsize_percentage);
                        എങ്കിൽ (!e)
                                തിരികെ -ENOMEM;

                        എങ്കിൽ (with_numbers)
                                printf("%s%4zu)%s ",
                                       ansi_grey(),
                                       p + 1,
                                       ansi_normal());

                        എങ്കിൽ (grey_prefix && startswith(e, grey_prefix)) {
                                size_t k = MIN(strlen(grey_prefix), column_width);
                                printf("%s%.*s%s",
                                       ansi_grey(),
                                       (പൂർണ്ണസംഖ്യ) k, e,
                                       ansi_normal());
                                printf("%-*s",
                                       (പൂർണ്ണസംഖ്യ) (column_width - k), e+k);
                        } അല്ലെങ്കിൽ
                                printf("%-*s", (പൂർണ്ണസംഖ്യ) column_width, e);
                }

                putchar('\n');

                /* on the first screen we reserve 2 extra lines for the title */
                എങ്കിൽ (i % break_വരികൾ == break_modulo)
                        എങ്കിൽ (!any_താക്കോൽ_to_proceed())
                                തിരികെ 0;
        }

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ open_terminal(സ്ഥിരം അക്ഷരം *name, പൂർണ്ണസംഖ്യ mode) {
        _cleanup_close_ പൂർണ്ണസംഖ്യ fd = -EBADF;

        /*
         * If a TTY is in the process of being closed opening it might cause EIO. This is horribly awful, but
         * unlikely to be changed in the kernel. Hence we work around this problem by retrying a couple of
         * times.
         *
         * https://bugs.launchpad.net/ubuntu/+source/linux/+bug/554172/comments/245
         */

        പ്രതിജ്ഞാ((mode & (O_CREAT|O_PATH|O_DIRECTORY|O_TMPFILE)) == 0);

        വേണ്ടി (unsigned c = 0;; c++) {
                fd = open(name, mode, 0);
                എങ്കിൽ (fd >= 0)
                        നിർത്തുക;

                എങ്കിൽ (errno != EIO)
                        തിരികെ -errno;

                /* Max 1s in total */
                എങ്കിൽ (c >= 20)
                        തിരികെ -EIO;

                (ശൂന്യം) usleep_safe(50 * USEC_PER_MSEC);
        }

        എങ്കിൽ (!isatty_safe(fd))
                തിരികെ -ENOTTY;

        തിരികെ TAKE_FD(fd);
}

പൂർണ്ണസംഖ്യ acquire_terminal(
                സ്ഥിരം അക്ഷരം *name,
                AcquireTerminalFlags flags,
                usec_t timeout) {

        _cleanup_close_ പൂർണ്ണസംഖ്യ notify = -EBADF, fd = -EBADF;
        usec_t ts = USEC_INFINITY;
        പൂർണ്ണസംഖ്യ r, wd = -1;

        പ്രതിജ്ഞാ(name);

        AcquireTerminalFlags mode = flags & _ACQUIRE_TERMINAL_MODE_MASK;
        പ്രതിജ്ഞാ(IN_SET(mode, ACQUIRE_TERMINAL_TRY, ACQUIRE_TERMINAL_FORCE, ACQUIRE_TERMINAL_WAIT));
        പ്രതിജ്ഞാ(mode == ACQUIRE_TERMINAL_WAIT || !FLAGS_SET(flags, ACQUIRE_TERMINAL_WATCH_SIGTERM));

        /* We use inotify to be notified when the tty is closed. We create the watch before checking if we can actually
         * acquire it, so that we don't lose any event.
         *
         * Note: strictly speaking this actually watches for the device being closed, it does *not* really watch
         * whether a tty loses its controlling process. However, unless some rogue process uses TIOCNOTTY on /dev/tty
         * *after* closing its tty otherwise this will not become a problem. As long as the administrator makes sure to
         * not configure any service on the same tty as an untrusted user this should not be a problem. (Which they
         * probably should not do anyway.) */

        എങ്കിൽ (mode == ACQUIRE_TERMINAL_WAIT) {
                notify = inotify_init1(IN_CLOEXEC | IN_NONBLOCK);
                എങ്കിൽ (notify < 0)
                        തിരികെ -errno;

                wd = inotify_add_watch(notify, name, IN_CLOSE);
                എങ്കിൽ (wd < 0)
                        തിരികെ -errno;

                എങ്കിൽ (timeout != USEC_INFINITY)
                        ts = now(CLOCK_MONOTONIC);
        }

        /* If we are called with ACQUIRE_TERMINAL_WATCH_SIGTERM we'll unblock SIGTERM during ppoll() temporarily */
        sigset_t poll_ss;
        assert_se(sigprocmask(SIG_SETMASK, /* newset= */ ശൂന്യ, &poll_ss) >= 0);
        എങ്കിൽ (flags & ACQUIRE_TERMINAL_WATCH_SIGTERM) {
                assert_se(sigismember(&poll_ss, SIGTERM) > 0);
                assert_se(sigdelset(&poll_ss, SIGTERM) >= 0);
        }

        വേണ്ടി (;;) {
                എങ്കിൽ (notify >= 0) {
                        r = flush_fd(notify);
                        എങ്കിൽ (r < 0)
                                തിരികെ r;
                }

                /* We pass here O_NOCTTY only so that we can check the return value TIOCSCTTY and have a reliable way
                 * to figure out if we successfully became the controlling process of the tty */
                fd = open_terminal(name, O_RDWR|O_NOCTTY|O_CLOEXEC);
                എങ്കിൽ (fd < 0)
                        തിരികെ fd;

                /* Temporarily ignore SIGHUP, so that we don't get SIGHUP'ed if we already own the tty. */
                ഘടന sigaction sa_old;
                assert_se(sigaction(SIGHUP, &sigaction_ignore, &sa_old) >= 0);

                /* First, try to get the tty */
                r = RET_NERRNO(ioctl(fd, TIOCSCTTY, mode == ACQUIRE_TERMINAL_FORCE));

                /* Reset signal handler to old value */
                assert_se(sigaction(SIGHUP, &sa_old, ശൂന്യ) >= 0);

                /* Success? Exit the loop now! */
                എങ്കിൽ (r >= 0)
                        നിർത്തുക;

                /* Any failure besides -EPERM? Fail, regardless of the mode. */
                എങ്കിൽ (r != -EPERM)
                        തിരികെ r;

                എങ്കിൽ (flags & ACQUIRE_TERMINAL_PERMISSIVE) /* If we are in permissive mode, then EPERM is fine, turn this
                                                          * into a success. Note that EPERM is also returned if we
                                                          * already are the owner of the TTY. */
                        നിർത്തുക;

                എങ്കിൽ (mode != ACQUIRE_TERMINAL_WAIT) /* If we are in TRY or FORCE mode, then propagate EPERM as EPERM */
                        തിരികെ r;

                പ്രതിജ്ഞാ(notify >= 0);
                പ്രതിജ്ഞാ(wd >= 0);

                വേണ്ടി (;;) {
                        usec_t ഇടത്;
                        എങ്കിൽ (timeout == USEC_INFINITY)
                                ഇടത് = USEC_INFINITY;
                        അല്ലെങ്കിൽ {
                                പ്രതിജ്ഞാ(ts != USEC_INFINITY);

                                usec_t n = usec_sub_unsigned(now(CLOCK_MONOTONIC), ts);
                                എങ്കിൽ (n >= timeout)
                                        തിരികെ -ETIMEDOUT;

                                ഇടത് = timeout - n;
                        }

                        r = ppoll_usec_full(
                                        &(ഘടന pollfd) {
                                                .fd = notify,
                                                .events = POLLIN,
                                        },
                                        /* n_fds= */ 1,
                                        ഇടത്,
                                        &poll_ss);
                        എങ്കിൽ (r < 0)
                                തിരികെ r;
                        എങ്കിൽ (r == 0)
                                തിരികെ -ETIMEDOUT;

                        സംഘം inotify_event_buffer buffer;
                        ssize_t l;
                        l = പഠ(notify, &buffer, വലുപ്പം(buffer));
                        എങ്കിൽ (l < 0) {
                                എങ്കിൽ (ERRNO_IS_TRANSIENT(errno))
                                        തുടരുക;

                                തിരികെ -errno;
                        }

                        FOREACH_INOTIFY_EVENT(e, buffer, l) {
                                എങ്കിൽ (e->mask & IN_Q_OVERFLOW) /* If we hit an inotify queue overflow, simply check if the terminal is up for grabs now. */
                                        നിർത്തുക;

                                എങ്കിൽ (e->wd != wd || !(e->mask & IN_CLOSE)) /* Safety checks */
                                        തിരികെ -EIO;
                        }

                        നിർത്തുക;
                }

                /* We close the tty fd here since if the old session ended our handle will be dead. It's important that
                 * we do this after sleeping, so that we don't enter an endless loop. */
                fd = safe_close(fd);
        }

        തിരികെ TAKE_FD(fd);
}

പൂർണ്ണസംഖ്യ release_terminal(ശൂന്യം) {
        _cleanup_close_ പൂർണ്ണസംഖ്യ fd = -EBADF;
        പൂർണ്ണസംഖ്യ r;

        fd = open("/dev/tty", O_RDWR|O_NOCTTY|O_CLOEXEC|O_NONBLOCK);
        എങ്കിൽ (fd < 0)
                തിരികെ -errno;

        /* Temporarily ignore SIGHUP, so that we don't get SIGHUP'ed
         * by our own TIOCNOTTY */
        ഘടന sigaction sa_old;
        assert_se(sigaction(SIGHUP, &sigaction_ignore, &sa_old) >= 0);

        r = RET_NERRNO(ioctl(fd, TIOCNOTTY));

        assert_se(sigaction(SIGHUP, &sa_old, ശൂന്യ) >= 0);

        തിരികെ r;
}

പൂർണ്ണസംഖ്യ terminal_new_session(ശൂന്യം) {

        /* Make us the new session leader, and set stdin tty to be our controlling terminal.
         *
         * Why stdin? Well, the ctty logic is relevant for signal delivery mostly, i.e. if people hit C-c
         * or the line is hung up. Such events are basically just a form of input, via a side channel
         * (that side channel being signal delivery, i.e. SIGINT, SIGHUP et al). Hence we focus on input,
         * not output here. */

        എങ്കിൽ (!isatty_safe(STDIN_FILENO))
                തിരികെ -ENXIO;

        (ശൂന്യം) setsid();
        തിരികെ RET_NERRNO(ioctl(STDIN_FILENO, TIOCSCTTY, 0));
}

ശൂന്യം terminal_detach_session(ശൂന്യം) {
        (ശൂന്യം) setsid();
        (ശൂന്യം) release_terminal();
}

പൂർണ്ണസംഖ്യ terminal_vhangup_fd(പൂർണ്ണസംഖ്യ fd) {
        പ്രതിജ്ഞാ(fd >= 0);
        തിരികെ RET_NERRNO(ioctl(fd, TIOCVHANGUP));
}

പൂർണ്ണസംഖ്യ terminal_vhangup(സ്ഥിരം അക്ഷരം *tty) {
        _cleanup_close_ പൂർണ്ണസംഖ്യ fd = -EBADF;

        പ്രതിജ്ഞാ(tty);

        fd = open_terminal(tty, O_RDWR|O_NOCTTY|O_CLOEXEC);
        എങ്കിൽ (fd < 0)
                തിരികെ fd;

        തിരികെ terminal_vhangup_fd(fd);
}

പൂർണ്ണസംഖ്യ vt_disallocate(സ്ഥിരം അക്ഷരം *tty_path) {
        പ്രതിജ്ഞാ(tty_path);

        /* Deallocate the VT if possible. If not possible (i.e. because it is the active one), at least clear
         * it entirely (including the scrollback buffer). */

        പൂർണ്ണസംഖ്യ ttynr = vtnr_from_tty(tty_path);
        എങ്കിൽ (ttynr > 0) {
                _cleanup_close_ പൂർണ്ണസംഖ്യ fd = open_terminal("/dev/tty0", O_RDWR|O_NOCTTY|O_CLOEXEC|O_NONBLOCK);
                എങ്കിൽ (fd < 0)
                        തിരികെ fd;

                /* Try to deallocate */
                എങ്കിൽ (ioctl(fd, VT_DISALLOCATE, ttynr) >= 0)
                        തിരികെ 0;
                എങ്കിൽ (errno != EBUSY)
                        തിരികെ -errno;
        }

        /* So this is not a VT (in which case we cannot deallocate it), or we failed to deallocate. Let's at
         * least clear the screen. */

        _cleanup_close_ പൂർണ്ണസംഖ്യ fd2 = open_terminal(tty_path, O_WRONLY|O_NOCTTY|O_CLOEXEC|O_NONBLOCK);
        എങ്കിൽ (fd2 < 0)
                തിരികെ fd2;

        തിരികെ loop_write_full(fd2,
                               "\033[r"   /* clear scrolling region */
                               "\033[H"   /* move home */
                               "\033[3J"  /* clear screen including scrollback, requires Linux 2.6.40 */
                               "\033c",   /* reset to initial state */
                               SIZE_MAX,
                               CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ vt_default_utf8(ശൂന്യം) {
        /* Read the default VT UTF8 setting from the kernel */
        തിരികെ read_boolean_file("/sys/module/vt/parameters/default_utf8");
}

സ്ഥിതം പൂർണ്ണസംഖ്യ vt_reset_keyboard(പൂർണ്ണസംഖ്യ fd) {
        പൂർണ്ണസംഖ്യ r, kb;

        പ്രതിജ്ഞാ(fd >= 0);

        /* If we can't read the default, then default to Unicode. It's 2024 after all. */
        r = vt_default_utf8();
        എങ്കിൽ (r < 0)
                log_debug_errno(r, "Failed to determine kernel VT UTF-8 mode, assuming enabled: %m");

        kb = vt_default_utf8() != 0 ? K_UNICODE : K_XLATE;
        തിരികെ RET_NERRNO(ioctl(fd, KDSKBMODE, kb));
}

സ്ഥിതം പൂർണ്ണസംഖ്യ terminal_reset_ioctl(പൂർണ്ണസംഖ്യ fd, bool switch_to_text) {
        ഘടന termios termios;
        പൂർണ്ണസംഖ്യ r;

        /* Set terminal to some sane defaults */

        പ്രതിജ്ഞാ(fd >= 0);

        /* We leave locked terminal attributes untouched, so that Plymouth may set whatever it wants to set,
         * and we don't interfere with that. */

        /* Disable exclusive mode, just in case */
        എങ്കിൽ (ioctl(fd, TIOCNXCL) < 0)
                log_debug_errno(errno, "TIOCNXCL ioctl failed on TTY, ignoring: %m");

        /* Switch to text mode */
        എങ്കിൽ (switch_to_text)
                എങ്കിൽ (ioctl(fd, KDSETMODE, KD_TEXT) < 0)
                        log_debug_errno(errno, "KDSETMODE ioctl for switching to text mode failed on TTY, ignoring: %m");

        /* Set default keyboard mode */
        r = vt_reset_keyboard(fd);
        എങ്കിൽ (r < 0)
                log_debug_errno(r, "Failed to reset VT keyboard, ignoring: %m");

        എങ്കിൽ (tcgetattr(fd, &termios) < 0) {
                r = log_debug_errno(errno, "Failed to get terminal parameters: %m");
                പോകുക finish;
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
        എങ്കിൽ (r < 0)
                log_debug_errno(r, "Failed to set terminal parameters: %m");

finish:
        /* Just in case, flush all crap out */
        (ശൂന്യം) tcflush(fd, TCIOFLUSH);

        തിരികെ r;
}

പൂർണ്ണസംഖ്യ terminal_reset_ansi_seq(പൂർണ്ണസംഖ്യ fd) {
        _cleanup_(nonblock_resetp) പൂർണ്ണസംഖ്യ nonblock_reset = -EBADF;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(fd >= 0);

        എങ്കിൽ (getenv_terminal_is_dumb())
                തിരികെ 0;

        r = fd_nonblock(fd, true);
        എങ്കിൽ (r < 0)
                തിരികെ log_debug_errno(r, "Failed to set terminal to non-blocking mode: %m");
        എങ്കിൽ (r > 0)
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
        എങ്കിൽ (r < 0)
                log_debug_errno(r, "Failed to reset terminal through ANSI sequences: %m");

        തിരികെ r;
}

ശൂന്യം reset_dev_console_fd(പൂർണ്ണസംഖ്യ fd, bool switch_to_text) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(fd >= 0);

        _cleanup_close_ പൂർണ്ണസംഖ്യ lock_fd = lock_dev_console();
        എങ്കിൽ (lock_fd < 0)
                log_debug_errno(lock_fd, "Failed to lock /dev/console, ignoring: %m");

        r = terminal_reset_ioctl(fd, switch_to_text);
        എങ്കിൽ (r < 0)
                log_warning_errno(r, "Failed to reset /dev/console, ignoring: %m");

        unsigned rows, cols;
        r = proc_cmdline_tty_size("/dev/console", &rows, &cols);
        എങ്കിൽ (r < 0)
                log_warning_errno(r, "Failed to get /dev/console size, ignoring: %m");
        അല്ലെങ്കിൽ എങ്കിൽ (r > 0) {
                r = terminal_set_size_fd(fd, ശൂന്യ, rows, cols);
                എങ്കിൽ (r < 0)
                        log_warning_errno(r, "Failed to set configured terminal size on /dev/console, ignoring: %m");
        } അല്ലെങ്കിൽ
                (ശൂന്യം) terminal_fix_size(fd, fd);

        r = terminal_reset_ansi_seq(fd);
        എങ്കിൽ (r < 0)
                log_warning_errno(r, "Failed to reset /dev/console using ANSI sequences, ignoring: %m");
}

പൂർണ്ണസംഖ്യ lock_dev_console(ശൂന്യം) {
        _cleanup_close_ പൂർണ്ണസംഖ്യ fd = -EBADF;
        പൂർണ്ണസംഖ്യ r;

        /* NB: We do not use O_NOFOLLOW here, because some container managers might place a symlink to some
         * pty in /dev/console, in which case it should be fine to lock the target TTY. */
        fd = open_terminal("/dev/console", O_RDONLY|O_CLOEXEC|O_NOCTTY);
        എങ്കിൽ (fd < 0)
                തിരികെ fd;

        r = lock_generic(fd, LOCK_BSD, LOCK_EX);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ TAKE_FD(fd);
}

പൂർണ്ണസംഖ്യ make_console_stdio(bool switch_to_text) {
        പൂർണ്ണസംഖ്യ fd, r;

        /* Make /dev/console the controlling terminal and stdin/stdout/stderr, if we can. If we can't use
         * /dev/null instead. This is particularly useful if /dev/console is turned off, e.g. if console=null
         * is specified on the kernel command line. */

        fd = acquire_terminal("/dev/console", ACQUIRE_TERMINAL_FORCE|ACQUIRE_TERMINAL_PERMISSIVE, USEC_INFINITY);
        എങ്കിൽ (fd < 0) {
                log_warning_errno(fd, "Failed to acquire terminal, using /dev/null stdin/stdout/stderr instead: %m");

                r = make_null_stdio();
                എങ്കിൽ (r < 0)
                        തിരികെ log_error_errno(r, "Failed to make /dev/null stdin/stdout/stderr: %m");

        } അല്ലെങ്കിൽ {
                reset_dev_console_fd(fd, switch_to_text);

                r = rearrange_stdio(fd, fd, fd); /* This invalidates 'fd' both on success and on failure. */
                എങ്കിൽ (r < 0)
                        തിരികെ log_error_errno(r, "Failed to make terminal stdin/stdout/stderr: %m");
        }

        reset_terminal_feature_caches();
        തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ vtnr_from_tty_raw(സ്ഥിരം അക്ഷരം *tty, unsigned *ret) {
        പ്രതിജ്ഞാ(tty);

        tty = skip_dev_prefix(tty);

        സ്ഥിരം അക്ഷരം *e = startswith(tty, "tty");
        എങ്കിൽ (!e)
                തിരികെ -EINVAL;

        തിരികെ safe_atou(e, ret);
}

പൂർണ്ണസംഖ്യ vtnr_from_tty(സ്ഥിരം അക്ഷരം *tty) {
        unsigned u;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(tty);

        r = vtnr_from_tty_raw(tty, &u);
        എങ്കിൽ (r < 0)
                തിരികെ r;
        എങ്കിൽ (!vtnr_is_valid(u))
                തിരികെ -ERANGE;

        തിരികെ (പൂർണ്ണസംഖ്യ) u;
}

bool tty_is_vc(സ്ഥിരം അക്ഷരം *tty) {
        പ്രതിജ്ഞാ(tty);

        /* NB: for >= 0 values no range check is conducted here, on the assumption that the caller will
         * either extract vtnr through vtnr_from_tty() later where ERANGE would be reported, or doesn't care
         * about whether it's strictly valid, but only asking "does this fall into the vt category?", for which
         * "yes" seems to be a better answer. */

        തിരികെ vtnr_from_tty_raw(tty, /* ret= */ ശൂന്യ) >= 0;
}

bool tty_is_console(സ്ഥിരം അക്ഷരം *tty) {
        പ്രതിജ്ഞാ(tty);

        തിരികെ streq(skip_dev_prefix(tty), "console");
}

പൂർണ്ണസംഖ്യ resolve_dev_console(അക്ഷരം **ret) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(ret);

        /* Resolve where /dev/console is pointing to. If /dev/console is a symlink (like in container
         * managers), we'll just resolve the symlink. If it's a real device node, we'll use if
         * /sys/class/tty/tty0/active, but only if /sys/ is actually ours (i.e. not read-only-mounted which
         * is a sign for container setups). */

        _cleanup_free_ അക്ഷരം *chased = ശൂന്യ;
        r = chase("/dev/console", /* root= */ ശൂന്യ, /* flags= */ 0, &chased, /* ret_fd= */ ശൂന്യ);
        എങ്കിൽ (r < 0)
                തിരികെ r;
        എങ്കിൽ (!path_equal(chased, "/dev/console")) {
                *ret = TAKE_PTR(chased);
                തിരികെ 0;
        }

        r = path_is_read_only_fs("/sys");
        എങ്കിൽ (r < 0)
                തിരികെ r;
        എങ്കിൽ (r > 0)
                തിരികെ -ENOMEDIUM;

        _cleanup_free_ അക്ഷരം *active = ശൂന്യ;
        r = read_one_വരി_file("/sys/class/tty/console/active", &active);
        എങ്കിൽ (r < 0)
                തിരികെ r;
        എങ്കിൽ (r == 0)
                തിരികെ -ENXIO;

        /* If multiple log outputs are configured the last one is what /dev/console points to */
        സ്ഥിരം അക്ഷരം *tty = strrchr(active, ' ');
        എങ്കിൽ (tty)
                tty++;
        അല്ലെങ്കിൽ
                tty = active;

        എങ്കിൽ (streq(tty, "tty0")) {
                active = mfree(active);

                /* Get the active VC (e.g. tty1) */
                r = read_one_വരി_file("/sys/class/tty/tty0/active", &active);
                എങ്കിൽ (r < 0)
                        തിരികെ r;
                എങ്കിൽ (r == 0)
                        തിരികെ -ENXIO;

                tty = active;
        }

        _cleanup_free_ അക്ഷരം *path = ശൂന്യ;
        path = path_join("/dev", tty);
        എങ്കിൽ (!path)
                തിരികെ -ENOMEM;

        *ret = TAKE_PTR(path);
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ get_kernel_consoles(അക്ഷരം ***ret) {
        _cleanup_strv_free_ അക്ഷരം **l = ശൂന്യ;
        _cleanup_free_ അക്ഷരം *വരി = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(ret);

        /* If /sys/ is mounted read-only this means we are running in some kind of container environment.
         * In that case /sys/ would reflect the host system, not us, hence ignore the data we can read from it. */
        എങ്കിൽ (path_is_read_only_fs("/sys") > 0)
                പോകുക fallback;

        r = read_one_വരി_file("/sys/class/tty/console/active", &വരി);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        വേണ്ടി (സ്ഥിരം അക്ഷരം *p = വരി;;) {
                _cleanup_free_ അക്ഷരം *tty = ശൂന്യ, *path = ശൂന്യ;

                r = extract_first_word(&p, &tty, ശൂന്യ, 0);
                എങ്കിൽ (r < 0)
                        തിരികെ r;
                എങ്കിൽ (r == 0)
                        നിർത്തുക;

                എങ്കിൽ (streq(tty, "tty0")) {
                        tty = mfree(tty);
                        r = read_one_വരി_file("/sys/class/tty/tty0/active", &tty);
                        എങ്കിൽ (r < 0)
                                തിരികെ r;
                        എങ്കിൽ (r == 0) {
                                log_debug("No VT active, skipping /dev/tty0.");
                                തുടരുക;
                        }
                }

                path = path_join("/dev", tty);
                എങ്കിൽ (!path)
                        തിരികെ -ENOMEM;

                എങ്കിൽ (access(path, F_OK) < 0) {
                        log_debug_errno(errno, "Console device %s is not accessible, skipping: %m", path);
                        തുടരുക;
                }

                r = strv_consume(&l, TAKE_PTR(path));
                എങ്കിൽ (r < 0)
                        തിരികെ r;
        }

        എങ്കിൽ (strv_isempty(l)) {
                log_debug("No devices found for system console");
                പോകുക fallback;
        }

        *ret = TAKE_PTR(l);
        തിരികെ strv_length(*ret);

fallback:
        r = strv_extend(&l, "/dev/console");
        എങ്കിൽ (r < 0)
                തിരികെ r;

        *ret = TAKE_PTR(l);
        തിരികെ 0;
}

bool tty_is_vc_resolve(സ്ഥിരം അക്ഷരം *tty) {
        _cleanup_free_ അക്ഷരം *resolved = ശൂന്യ;

        പ്രതിജ്ഞാ(tty);

        എങ്കിൽ (streq(skip_dev_prefix(tty), "console")) {
                എങ്കിൽ (resolve_dev_console(&resolved) < 0)
                        തിരികെ false;

                tty = resolved;
        }

        തിരികെ tty_is_vc(tty);
}

പൂർണ്ണസംഖ്യ fd_columns(പൂർണ്ണസംഖ്യ fd) {
        ഘടന winsize ws = {};

        എങ്കിൽ (fd < 0)
                തിരികെ -EBADF;

        എങ്കിൽ (ioctl(fd, TIOCGWINSZ, &ws) < 0)
                തിരികെ -errno;

        എങ്കിൽ (ws.ws_col <= 0)
                തിരികെ -ENODATA; /* some tty types come up with invalid row/column initially, return a recognizable error for that */

        തിരികെ ws.ws_col;
}

പൂർണ്ണസംഖ്യ getenv_columns(ശൂന്യം) {
        പൂർണ്ണസംഖ്യ r;

        സ്ഥിരം അക്ഷരം *e = getenv("COLUMNS");
        എങ്കിൽ (!e)
                തിരികെ -ENXIO;

        unsigned c;
        r = safe_atou_bounded(e, 1, USHRT_MAX, &c);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ (പൂർണ്ണസംഖ്യ) c;
}

unsigned columns(ശൂന്യം) {

        എങ്കിൽ (cached_columns > 0)
                തിരികെ cached_columns;

        പൂർണ്ണസംഖ്യ c = getenv_columns();
        എങ്കിൽ (c < 0) {
                c = fd_columns(STDOUT_FILENO);
                എങ്കിൽ (c < 0)
                        c = 80;
        }

        പ്രതിജ്ഞാ(c > 0);

        cached_columns = c;
        തിരികെ cached_columns;
}

പൂർണ്ണസംഖ്യ fd_വരികൾ(പൂർണ്ണസംഖ്യ fd) {
        ഘടന winsize ws = {};

        എങ്കിൽ (fd < 0)
                തിരികെ -EBADF;

        എങ്കിൽ (ioctl(fd, TIOCGWINSZ, &ws) < 0)
                തിരികെ -errno;

        എങ്കിൽ (ws.ws_row <= 0)
                തിരികെ -ENODATA; /* some tty types come up with invalid row/column initially, return a recognizable error for that */

        തിരികെ ws.ws_row;
}

unsigned വരികൾ(ശൂന്യം) {
        സ്ഥിരം അക്ഷരം *e;
        പൂർണ്ണസംഖ്യ l;

        എങ്കിൽ (cached_വരികൾ > 0)
                തിരികെ cached_വരികൾ;

        l = 0;
        e = getenv("LINES");
        എങ്കിൽ (e)
                (ശൂന്യം) safe_atoi(e, &l);

        എങ്കിൽ (l <= 0 || l > USHRT_MAX) {
                l = fd_വരികൾ(STDOUT_FILENO);
                എങ്കിൽ (l <= 0)
                        l = 24;
        }

        cached_വരികൾ = l;
        തിരികെ cached_വരികൾ;
}

പൂർണ്ണസംഖ്യ terminal_set_size_fd(പൂർണ്ണസംഖ്യ fd, സ്ഥിരം അക്ഷരം *ident, unsigned rows, unsigned cols) {
        ഘടന winsize ws;

        പ്രതിജ്ഞാ(fd >= 0);

        എങ്കിൽ (!ident)
                ident = "TTY";

        എങ്കിൽ (rows == UINT_MAX && cols == UINT_MAX)
                തിരികെ 0;

        എങ്കിൽ (ioctl(fd, TIOCGWINSZ, &ws) < 0)
                തിരികെ log_debug_errno(errno,
                                       "TIOCGWINSZ ioctl for getting %s size failed, not setting terminal size: %m",
                                       ident);

        എങ്കിൽ (rows == UINT_MAX)
                rows = ws.ws_row;
        അല്ലെങ്കിൽ എങ്കിൽ (rows > USHRT_MAX)
                rows = USHRT_MAX;

        എങ്കിൽ (cols == UINT_MAX)
                cols = ws.ws_col;
        അല്ലെങ്കിൽ എങ്കിൽ (cols > USHRT_MAX)
                cols = USHRT_MAX;

        എങ്കിൽ (rows == ws.ws_row && cols == ws.ws_col)
                തിരികെ 0;

        ws.ws_row = rows;
        ws.ws_col = cols;

        എങ്കിൽ (ioctl(fd, TIOCSWINSZ, &ws) < 0)
                തിരികെ log_debug_errno(errno, "TIOCSWINSZ ioctl for setting %s size failed: %m", ident);

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ proc_cmdline_tty_size(സ്ഥിരം അക്ഷരം *tty, unsigned *ret_rows, unsigned *ret_cols) {
        _cleanup_free_ അക്ഷരം *rowskey = ശൂന്യ, *rowsvalue = ശൂന്യ, *colskey = ശൂന്യ, *colsvalue = ശൂന്യ;
        unsigned rows = UINT_MAX, cols = UINT_MAX;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(tty);

        എങ്കിൽ (!ret_rows && !ret_cols)
                തിരികെ 0;

        tty = skip_dev_prefix(tty);
        എങ്കിൽ (path_startswith(tty, "pts/"))
                തിരികെ -EMEDIUMTYPE;
        എങ്കിൽ (!in_charset(tty, ALPHANUMERICAL))
                തിരികെ log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "TTY name '%s' contains non-alphanumeric characters, not searching kernel cmdline for size.", tty);

        rowskey = strjoin("systemd.tty.rows.", tty);
        എങ്കിൽ (!rowskey)
                തിരികെ -ENOMEM;

        colskey = strjoin("systemd.tty.columns.", tty);
        എങ്കിൽ (!colskey)
                തിരികെ -ENOMEM;

        r = proc_cmdline_get_താക്കോൽ_many(/* flags= */ 0,
                                      rowskey, &rowsvalue,
                                      colskey, &colsvalue);
        എങ്കിൽ (r < 0)
                തിരികെ log_debug_errno(r, "Failed to read TTY size of %s from kernel cmdline: %m", tty);

        എങ്കിൽ (rowsvalue) {
                r = safe_atou(rowsvalue, &rows);
                എങ്കിൽ (r < 0)
                        തിരികെ log_debug_errno(r, "Failed to parse %s=%s: %m", rowskey, rowsvalue);
        }

        എങ്കിൽ (colsvalue) {
                r = safe_atou(colsvalue, &cols);
                എങ്കിൽ (r < 0)
                        തിരികെ log_debug_errno(r, "Failed to parse %s=%s: %m", colskey, colsvalue);
        }

        എങ്കിൽ (ret_rows)
                *ret_rows = rows;
        എങ്കിൽ (ret_cols)
                *ret_cols = cols;

        തിരികെ rows != UINT_MAX || cols != UINT_MAX;
}

/* intended to be used as a SIGWINCH sighandler */
ശൂന്യം columns_വരികൾ_cache_reset(പൂർണ്ണസംഖ്യ signum) {
        cached_columns = 0;
        cached_വരികൾ = 0;
}

ശൂന്യം reset_terminal_feature_caches(ശൂന്യം) {
        cached_columns = 0;
        cached_വരികൾ = 0;

        cached_on_tty = -1;
        cached_on_dev_null = -1;

        reset_ansi_feature_caches();
}

bool on_tty(ശൂന്യം) {

        /* We check both stdout and stderr, so that situations where pipes on the shell are used are reliably
         * recognized, regardless if only the output or the errors are piped to some place. Since on_tty() is generally
         * used to default to a safer, non-interactive, non-color mode of operation it's probably good to be defensive
         * here, and check for both. Note that we don't check for STDIN_FILENO, because it should fine to use fancy
         * terminal functionality when outputting stuff, even if the input is piped to us. */

        എങ്കിൽ (cached_on_tty < 0)
                cached_on_tty =
                        isatty_safe(STDOUT_FILENO) &&
                        isatty_safe(STDERR_FILENO);

        തിരികെ cached_on_tty;
}

പൂർണ്ണസംഖ്യ getttyname_malloc(പൂർണ്ണസംഖ്യ fd, അക്ഷരം **ret) {
        അക്ഷരം path[PATH_MAX]; /* PATH_MAX is counted *with* the trailing NUL byte */
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(fd >= 0);
        പ്രതിജ്ഞാ(ret);

        r = ttyname_r(fd, path, വലുപ്പം path); /* positive error */
        പ്രതിജ്ഞാ(r >= 0);
        എങ്കിൽ (r == ERANGE)
                തിരികെ -ENAMETOOLONG;
        എങ്കിൽ (r > 0)
                തിരികെ -r;

        തിരികെ strdup_to(ret, skip_dev_prefix(path));
}

പൂർണ്ണസംഖ്യ getttyname_harder(പൂർണ്ണസംഖ്യ fd, അക്ഷരം **ret) {
        _cleanup_free_ അക്ഷരം *s = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(ret);

        r = getttyname_malloc(fd, &s);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (streq(s, "tty"))
                തിരികെ get_ctty(0, ശൂന്യ, ret);

        *ret = TAKE_PTR(s);
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ get_ctty_devnr(pid_t pid, dev_t *ret) {
        _cleanup_free_ അക്ഷരം *വരി = ശൂന്യ;
        unsigned long ttynr;
        സ്ഥിരം അക്ഷരം *p;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(pid >= 0);

        p = procfs_file_alloca(pid, "stat");
        r = read_one_വരി_file(p, &വരി);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        p = strrchr(വരി, ')');
        എങ്കിൽ (!p)
                തിരികെ -EIO;

        p++;

        എങ്കിൽ (sscanf(p, " "
                   "%*c "  /* state */
                   "%*d "  /* ppid */
                   "%*d "  /* pgrp */
                   "%*d "  /* session */
                   "%lu ", /* ttynr */
                   &ttynr) != 1)
                തിരികെ -EIO;

        എങ്കിൽ (devnum_is_zero(ttynr))
                തിരികെ -ENXIO;

        എങ്കിൽ (ret)
                *ret = (dev_t) ttynr;

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ get_ctty(pid_t pid, dev_t *ret_devnr, അക്ഷരം **ret) {
        അക്ഷരം pty[STRLEN("/dev/pts/") + DECIMAL_STR_MAX(dev_t) + 1];
        _cleanup_free_ അക്ഷരം *buf = ശൂന്യ;
        സ്ഥിരം അക്ഷരം *fn = ശൂന്യ, *w;
        dev_t devnr;
        പൂർണ്ണസംഖ്യ r;

        r = get_ctty_devnr(pid, &devnr);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        r = device_path_make_canonical(S_IFCHR, devnr, &buf);
        എങ്കിൽ (r < 0) {
                ഘടന stat st;

                എങ്കിൽ (r != -ENOENT) /* No symlink for this in /dev/char/? */
                        തിരികെ r;

                /* Maybe this is PTY? PTY devices are not listed in /dev/char/, as they don't follow the
                 * Linux device model and hence device_path_make_canonical() doesn't work for them. Let's
                 * assume this is a PTY for a moment, and check if the device node this would then map to in
                 * /dev/pts/ matches the one we are looking for. This way we don't have to hardcode the major
                 * number (which is 136 btw), but we still rely on the fact that PTY numbers map directly to
                 * the minor number of the pty. */
                xsprintf(pty, "/dev/pts/%u", minor(devnr));

                എങ്കിൽ (stat(pty, &st) < 0) {
                        എങ്കിൽ (errno != ENOENT)
                                തിരികെ -errno;

                } അല്ലെങ്കിൽ എങ്കിൽ (S_ISCHR(st.st_mode) && devnr == st.st_rdev) /* Bingo! */
                        fn = pty;

                എങ്കിൽ (!fn) {
                        /* Doesn't exist, or not a PTY? Probably something similar to the PTYs which have no
                         * symlink in /dev/char/. Let's return something vaguely useful. */
                        r = device_path_make_major_minor(S_IFCHR, devnr, &buf);
                        എങ്കിൽ (r < 0)
                                തിരികെ r;

                        fn = buf;
                }
        } അല്ലെങ്കിൽ
                fn = buf;

        w = path_startswith(fn, "/dev/");
        എങ്കിൽ (!w)
                തിരികെ -EINVAL;

        എങ്കിൽ (ret) {
                r = strdup_to(ret, w);
                എങ്കിൽ (r < 0)
                        തിരികെ r;
        }

        എങ്കിൽ (ret_devnr)
                *ret_devnr = devnr;

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ ptsname_malloc(പൂർണ്ണസംഖ്യ fd, അക്ഷരം **ret) {
        പ്രതിജ്ഞാ(fd >= 0);
        പ്രതിജ്ഞാ(ret);

        വേണ്ടി (size_t l = 50;;) {
                _cleanup_free_ അക്ഷരം *c = ശൂന്യ;

                c = new(അക്ഷരം, l);
                എങ്കിൽ (!c)
                        തിരികെ -ENOMEM;

                എങ്കിൽ (ptsname_r(fd, c, l) >= 0) {
                        *ret = TAKE_PTR(c);
                        തിരികെ 0;
                }
                എങ്കിൽ (errno != ERANGE)
                        തിരികെ -errno;

                എങ്കിൽ (!MUL_ASSIGN_SAFE(&l, 2))
                        തിരികെ -ENOMEM;
        }
}

പൂർണ്ണസംഖ്യ openpt_allocate(പൂർണ്ണസംഖ്യ flags, അക്ഷരം **ret_peer_path) {
        _cleanup_close_ പൂർണ്ണസംഖ്യ fd = -EBADF;
        പൂർണ്ണസംഖ്യ r;

        fd = posix_openpt(flags|O_NOCTTY|O_CLOEXEC);
        എങ്കിൽ (fd < 0)
                തിരികെ -errno;

        _cleanup_free_ അക്ഷരം *p = ശൂന്യ;
        എങ്കിൽ (ret_peer_path) {
                r = ptsname_malloc(fd, &p);
                എങ്കിൽ (r < 0)
                        തിരികെ r;

                എങ്കിൽ (!path_startswith(p, "/dev/pts/"))
                        തിരികെ -EINVAL;
        }

        എങ്കിൽ (unlockpt(fd) < 0)
                തിരികെ -errno;

        എങ്കിൽ (ret_peer_path)
                *ret_peer_path = TAKE_PTR(p);

        തിരികെ TAKE_FD(fd);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ ptsname_namespace(പൂർണ്ണസംഖ്യ pty, അക്ഷരം **ret) {
        പൂർണ്ണസംഖ്യ no = -1;

        പ്രതിജ്ഞാ(pty >= 0);
        പ്രതിജ്ഞാ(ret);

        /* Like ptsname(), but doesn't assume that the path is
         * accessible in the local namespace. */

        എങ്കിൽ (ioctl(pty, TIOCGPTN, &no) < 0)
                തിരികെ -errno;

        എങ്കിൽ (no < 0)
                തിരികെ -EIO;

        എങ്കിൽ (asprintf(ret, "/dev/pts/%i", no) < 0)
                തിരികെ -ENOMEM;

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ openpt_allocate_in_namespace(
                സ്ഥിരം PidRef *pidref,
                പൂർണ്ണസംഖ്യ flags,
                അക്ഷരം **ret_peer_path) {

        _cleanup_close_ പൂർണ്ണസംഖ്യ pidnsfd = -EBADF, mntnsfd = -EBADF, usernsfd = -EBADF, rootfd = -EBADF, fd = -EBADF;
        _cleanup_close_pair_ പൂർണ്ണസംഖ്യ pair[2] = EBADF_PAIR;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(pidref);

        r = pidref_namespace_open(pidref, &pidnsfd, &mntnsfd, /* ret_netns_fd= */ ശൂന്യ, &usernsfd, &rootfd);
        എങ്കിൽ (r < 0)
                തിരികെ log_debug_errno(r, "Failed to open namespaces of PID "PID_FMT": %m", pidref->pid);

        എങ്കിൽ (socketpair(AF_UNIX, SOCK_DGRAM|SOCK_CLOEXEC, 0, pair) < 0)
                തിരികെ -errno;

        r = namespace_fork(
                        "(sd-openptns)",
                        "(sd-openpt)",
                        FORK_RESET_SIGNALS|FORK_DEATHSIG_SIGKILL|FORK_WAIT,
                        pidnsfd,
                        mntnsfd,
                        /* netns_fd= */ -EBADF,
                        usernsfd,
                        rootfd,
                        /* ret= */ ശൂന്യ);
        എങ്കിൽ (r < 0)
                തിരികെ r;
        എങ്കിൽ (r == 0) {
                pair[0] = safe_close(pair[0]);

                fd = openpt_allocate(flags, /* ret_peer_path= */ ശൂന്യ);
                എങ്കിൽ (fd < 0)
                        _exit(EXIT_FAILURE);

                എങ്കിൽ (send_one_fd(pair[1], fd, 0) < 0)
                        _exit(EXIT_FAILURE);

                _exit(EXIT_SUCCESS);
        }

        pair[1] = safe_close(pair[1]);

        fd = receive_one_fd(pair[0], 0);
        എങ്കിൽ (fd < 0)
                തിരികെ fd;

        എങ്കിൽ (ret_peer_path) {
                r = ptsname_namespace(fd, ret_peer_path);
                എങ്കിൽ (r < 0)
                        തിരികെ r;
        }

        തിരികെ TAKE_FD(fd);
}

സ്ഥിതം bool on_dev_null(ശൂന്യം) {
        ഘടന stat dst, ost, est;

        എങ്കിൽ (cached_on_dev_null >= 0)
                തിരികെ cached_on_dev_null;

        എങ്കിൽ (stat("/dev/null", &dst) < 0 || fstat(STDOUT_FILENO, &ost) < 0 || fstat(STDERR_FILENO, &est) < 0)
                cached_on_dev_null = false;
        അല്ലെങ്കിൽ
                cached_on_dev_null = stat_inode_same(&dst, &ost) && stat_inode_same(&dst, &est);

        തിരികെ cached_on_dev_null;
}

bool term_env_valid(സ്ഥിരം അക്ഷരം *term) {
        /* Checks if the specified $TERM value is suitable for propagation, i.e. is not empty, not set to
         * "unknown" (as is common in CI), and only contains characters valid in terminal type names.
         * Valid $TERM values are things like "xterm-256color", "linux", "screen.xterm-256color", i.e.
         * alphanumeric characters, hyphens, underscores, dots, and plus signs. */
        തിരികെ !isempty(term) &&
                !streq(term, "unknown") &&
                in_charset(term, ALPHANUMERICAL "-_+.");
}

bool getenv_terminal_is_dumb(ശൂന്യം) {
        സ്ഥിരം അക്ഷരം *e;

        e = getenv("TERM");
        എങ്കിൽ (!e)
                തിരികെ true;

        തിരികെ streq(e, "dumb");
}

bool terminal_is_dumb(ശൂന്യം) {
        എങ്കിൽ (!on_tty() && !on_dev_null())
                തിരികെ true;

        തിരികെ getenv_terminal_is_dumb();
}

bool dev_console_colors_enabled(ശൂന്യം) {
        _cleanup_free_ അക്ഷരം *s = ശൂന്യ;
        ColorMode m;

        /* Returns true if we assume that color is supported on /dev/console.
         *
         * For that we first check if we explicitly got told to use colors or not, by checking $SYSTEMD_COLORS. If that
         * isn't set we check whether PID 1 has $TERM set, and if not, whether TERM is set on the kernel command
         * line. If we find $TERM set we assume color if it's not set to "dumb", similarly to how regular
         * colors_enabled() operates. */

        m = parse_systemd_colors();
        എങ്കിൽ (m >= 0)
                തിരികെ m;

        എങ്കിൽ (getenv("NO_COLOR"))
                തിരികെ false;

        എങ്കിൽ (getenv_for_pid(1, "TERM", &s) <= 0)
                (ശൂന്യം) proc_cmdline_get_താക്കോൽ("TERM", 0, &s);

        തിരികെ !streq_ptr(s, "dumb");
}

പൂർണ്ണസംഖ്യ vt_restore(പൂർണ്ണസംഖ്യ fd) {

        സ്ഥിതം സ്ഥിരം ഘടന vt_mode mode = {
                .mode = VT_AUTO,
        };

        പൂർണ്ണസംഖ്യ r, ret = 0;

        പ്രതിജ്ഞാ(fd >= 0);

        എങ്കിൽ (!isatty_safe(fd))
                തിരികെ log_debug_errno(SYNTHETIC_ERRNO(ENOTTY), "Asked to restore the VT for an fd that does not refer to a terminal.");

        എങ്കിൽ (ioctl(fd, KDSETMODE, KD_TEXT) < 0)
                RET_GATHER(ret, log_debug_errno(errno, "Failed to set VT to text mode, ignoring: %m"));

        r = vt_reset_keyboard(fd);
        എങ്കിൽ (r < 0)
                RET_GATHER(ret, log_debug_errno(r, "Failed to reset keyboard mode, ignoring: %m"));

        എങ്കിൽ (ioctl(fd, VT_SETMODE, &mode) < 0)
                RET_GATHER(ret, log_debug_errno(errno, "Failed to set VT_AUTO mode, ignoring: %m"));

        r = fchmod_and_chown(fd, TTY_MODE, 0, GID_INVALID);
        എങ്കിൽ (r < 0)
                RET_GATHER(ret, log_debug_errno(r, "Failed to chmod()/chown() VT, ignoring: %m"));

        തിരികെ ret;
}

പൂർണ്ണസംഖ്യ vt_release(പൂർണ്ണസംഖ്യ fd, bool restore) {
        പ്രതിജ്ഞാ(fd >= 0);

        /* This function releases the VT by acknowledging the VT-switch signal
         * sent by the kernel and optionally reset the VT in text and auto
         * VT-switching modes. */

        എങ്കിൽ (!isatty_safe(fd))
                തിരികെ log_debug_errno(SYNTHETIC_ERRNO(ENOTTY), "Asked to release the VT for an fd that does not refer to a terminal.");

        എങ്കിൽ (ioctl(fd, VT_RELDISP, 1) < 0)
                തിരികെ -errno;

        എങ്കിൽ (restore)
                തിരികെ vt_restore(fd);

        തിരികെ 0;
}

ശൂന്യം get_log_colors(പൂർണ്ണസംഖ്യ priority, സ്ഥിരം അക്ഷരം **on, സ്ഥിരം അക്ഷരം **off, സ്ഥിരം അക്ഷരം **highlight) {
        /* Note that this will initialize output variables only when there's something to output.
         * The caller must pre-initialize to "" or NULL as appropriate. */

        എങ്കിൽ (priority <= LOG_ERR) {
                എങ്കിൽ (on)
                        *on = ansi_highlight_red();
                എങ്കിൽ (off)
                        *off = ansi_normal();
                എങ്കിൽ (highlight)
                        *highlight = ansi_highlight();

        } അല്ലെങ്കിൽ എങ്കിൽ (priority <= LOG_WARNING) {
                എങ്കിൽ (on)
                        *on = ansi_highlight_yellow();
                എങ്കിൽ (off)
                        *off = ansi_normal();
                എങ്കിൽ (highlight)
                        *highlight = ansi_highlight();

        } അല്ലെങ്കിൽ എങ്കിൽ (priority <= LOG_NOTICE) {
                എങ്കിൽ (on)
                        *on = ansi_highlight();
                എങ്കിൽ (off)
                        *off = ansi_normal();
                എങ്കിൽ (highlight)
                        *highlight = ansi_highlight_red();

        } അല്ലെങ്കിൽ എങ്കിൽ (priority >= LOG_DEBUG) {
                എങ്കിൽ (on)
                        *on = ansi_grey();
                എങ്കിൽ (off)
                        *off = ansi_normal();
                എങ്കിൽ (highlight)
                        *highlight = ansi_highlight_red();
        }
}

പൂർണ്ണസംഖ്യ terminal_set_cursor_position(പൂർണ്ണസംഖ്യ fd, unsigned row, unsigned column) {
        പ്രതിജ്ഞാ(fd >= 0);

        അക്ഷരം cursor_position[STRLEN("\x1B[" ";" "H") + DECIMAL_STR_MAX(unsigned) * 2 + 1];
        xsprintf(cursor_position, "\x1B[%u;%uH", row, column);

        തിരികെ loop_write(fd, cursor_position, SIZE_MAX);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ terminal_verify_same(പൂർണ്ണസംഖ്യ നിവേശം_fd, പൂർണ്ണസംഖ്യ output_fd) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(നിവേശം_fd >= 0);
        പ്രതിജ്ഞാ(output_fd >= 0);

        /* Validates that the specified fds reference the same TTY */

        എങ്കിൽ (നിവേശം_fd != output_fd) {
                ഘടന stat sti;
                എങ്കിൽ (fstat(നിവേശം_fd, &sti) < 0)
                        തിരികെ -errno;

                r = stat_verify_char(&sti); /* TTYs are character devices */
                എങ്കിൽ (r < 0)
                        തിരികെ r;

                ഘടന stat sto;
                എങ്കിൽ (fstat(output_fd, &sto) < 0)
                        തിരികെ -errno;

                r = stat_verify_char(&sto);
                എങ്കിൽ (r < 0)
                        തിരികെ r;

                എങ്കിൽ (sti.st_rdev != sto.st_rdev)
                        തിരികെ -ENOLINK;
        }

        എങ്കിൽ (!isatty_safe(നിവേശം_fd)) /* The check above was just for char device, but now let's ensure it's actually a tty */
                തിരികെ -ENOTTY;

        തിരികെ 0;
}

തരനിർവചനം എണ്ണൽ CursorPositionState {
        CURSOR_TEXT,
        CURSOR_ESCAPE,
        CURSOR_ROW,
        CURSOR_COLUMN,
} CursorPositionState;

തരനിർവചനം ഘടന CursorPositionContext {
        CursorPositionState state;
        unsigned row, column;
} CursorPositionContext;

സ്ഥിതം പൂർണ്ണസംഖ്യ scan_cursor_position_response(
                CursorPositionContext *context,
                സ്ഥിരം അക്ഷരം *buf,
                size_t size,
                size_t *ret_processed) {

        പ്രതിജ്ഞാ(context);
        പ്രതിജ്ഞാ(buf);
        പ്രതിജ്ഞാ(ret_processed);

        വേണ്ടി (size_t i = 0; i < size; i++) {
                അക്ഷരം c = buf[i];

                തിരഞ്ഞെടുപ്പ് (context->state) {

                അവസ്ഥ CURSOR_TEXT:
                        context->state = c == '\x1B' ? CURSOR_ESCAPE : CURSOR_TEXT;
                        നിർത്തുക;

                അവസ്ഥ CURSOR_ESCAPE:
                        context->state = c == '[' ? CURSOR_ROW : CURSOR_TEXT;
                        നിർത്തുക;

                അവസ്ഥ CURSOR_ROW:
                        എങ്കിൽ (c == ';')
                                context->state = context->row > 0 ? CURSOR_COLUMN : CURSOR_TEXT;
                        അല്ലെങ്കിൽ {
                                പൂർണ്ണസംഖ്യ d = undecchar(c);

                                /* We read a decimal character, let's suffix it to the number we so far read,
                                 * but let's do an overflow check first. */
                                എങ്കിൽ (d < 0 || context->row > (UINT_MAX-d)/10)
                                        context->state = CURSOR_TEXT;
                                അല്ലെങ്കിൽ
                                        context->row = context->row * 10 + d;
                        }
                        നിർത്തുക;

                അവസ്ഥ CURSOR_COLUMN:
                        എങ്കിൽ (c == 'R') {
                                എങ്കിൽ (context->column > 0) {
                                        *ret_processed = i + 1;
                                        തിരികെ 1; /* success! */
                                }

                                context->state = CURSOR_TEXT;
                        } അല്ലെങ്കിൽ {
                                പൂർണ്ണസംഖ്യ d = undecchar(c);

                                /* As above, add the decimal character to our column number */
                                എങ്കിൽ (d < 0 || context->column > (UINT_MAX-d)/10)
                                        context->state = CURSOR_TEXT;
                                അല്ലെങ്കിൽ
                                        context->column = context->column * 10 + d;
                        }

                        നിർത്തുക;
                }

                /* Reset any positions we might have picked up */
                എങ്കിൽ (IN_SET(context->state, CURSOR_TEXT, CURSOR_ESCAPE))
                        context->row = context->column = 0;
        }

        *ret_processed = size;
        തിരികെ 0; /* all good, but not enough data yet */
}

പൂർണ്ണസംഖ്യ terminal_get_cursor_position(
                പൂർണ്ണസംഖ്യ നിവേശം_fd,
                പൂർണ്ണസംഖ്യ output_fd,
                unsigned *ret_row,
                unsigned *ret_column) {

        _cleanup_close_ പൂർണ്ണസംഖ്യ nonblock_നിവേശം_fd = -EBADF;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(നിവേശം_fd >= 0);
        പ്രതിജ്ഞാ(output_fd >= 0);

        എങ്കിൽ (getenv_terminal_is_dumb())
                തിരികെ -EOPNOTSUPP;

        r = terminal_verify_same(നിവേശം_fd, output_fd);
        എങ്കിൽ (r < 0)
                തിരികെ log_debug_errno(r, "Called with distinct input/output fds: %m");

        /* Failure to reset the terminal is ignored here and in similar cases below.
         * We already have our result; if cleanup fails it doesn't change the validity of the result. */
        ഘടന termios old_termios = TERMIOS_NULL;
        CLEANUP_TERMIOS_RESET(നിവേശം_fd, old_termios);

        എങ്കിൽ (tcgetattr(നിവേശം_fd, &old_termios) < 0)
                തിരികെ log_debug_errno(errno, "Failed to get terminal settings: %m");

        ഘടന termios new_termios = old_termios;
        termios_disable_echo(&new_termios);

        എങ്കിൽ (tcsetattr(നിവേശം_fd, TCSANOW, &new_termios) < 0)
                തിരികെ log_debug_errno(errno, "Failed to set new terminal settings: %m");

        /* Request cursor position (DSR/CPR) */
        r = loop_write(output_fd, "\x1B[6n", SIZE_MAX);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        /* Open a 2nd input fd, in non-blocking mode, so that we won't ever hang in read() should someone
         * else process the POLLIN. */

        nonblock_നിവേശം_fd = r = fd_reopen(നിവേശം_fd, O_RDONLY|O_CLOEXEC|O_NONBLOCK|O_NOCTTY);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        usec_t end = usec_add(now(CLOCK_MONOTONIC), CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        അക്ഷരം buf[STRLEN("\x1B[1;1R")]; /* The shortest valid reply possible */
        size_t buf_full = 0;
        CursorPositionContext context = {};

        വേണ്ടി (bool first = true;; first = false) {
                എങ്കിൽ (buf_full == 0) {
                        usec_t n = now(CLOCK_MONOTONIC);
                        എങ്കിൽ (n >= end)
                                തിരികെ -EOPNOTSUPP;

                        r = fd_wait_for_event(nonblock_നിവേശം_fd, POLLIN, usec_sub_unsigned(end, n));
                        എങ്കിൽ (r < 0)
                                തിരികെ r;
                        എങ്കിൽ (r == 0)
                                തിരികെ -EOPNOTSUPP;

                        /* On the first try, read multiple characters, i.e. the shortest valid
                         * reply. Afterwards read byte-wise, since we don't want to read too much, and
                         * unnecessarily drop too many characters from the input queue. */
                        ssize_t l = പഠ(nonblock_നിവേശം_fd, buf, first ? വലുപ്പം(buf) : 1);
                        എങ്കിൽ (l < 0) {
                                എങ്കിൽ (errno == EAGAIN)
                                        തുടരുക;

                                തിരികെ -errno;
                        }

                        പ്രതിജ്ഞാ((size_t) l <= വലുപ്പം(buf));
                        buf_full = l;
                }

                size_t processed;
                r = scan_cursor_position_response(&context, buf, buf_full, &processed);
                എങ്കിൽ (r < 0)
                        തിരികെ r;

                പ്രതിജ്ഞാ(processed <= buf_full);
                buf_full -= processed;
                memmove(buf, buf + processed, buf_full);

                എങ്കിൽ (r > 0) {
                        /* Superficial validity check */
                        എങ്കിൽ (context.row >= 32766 || context.column >= 32766)
                                തിരികെ -ENODATA;

                        എങ്കിൽ (ret_row)
                                *ret_row = context.row;
                        എങ്കിൽ (ret_column)
                                *ret_column = context.column;

                        തിരികെ 0;
                }
        }
}

പൂർണ്ണസംഖ്യ terminal_reset_defensive(പൂർണ്ണസംഖ്യ fd, TerminalResetFlags flags) {
        പൂർണ്ണസംഖ്യ r = 0;

        പ്രതിജ്ഞാ(fd >= 0);
        പ്രതിജ്ഞാ(!FLAGS_SET(flags, TERMINAL_RESET_AVOID_ANSI_SEQ|TERMINAL_RESET_FORCE_ANSI_SEQ));

        /* Resets the terminal comprehensively, i.e. via both ioctl()s and via ANSI sequences, but do so only
         * if $TERM is unset or set to "dumb" */

        എങ്കിൽ (!isatty_safe(fd))
                തിരികെ -ENOTTY;

        RET_GATHER(r, terminal_reset_ioctl(fd, FLAGS_SET(flags, TERMINAL_RESET_SWITCH_TO_TEXT)));

        എങ്കിൽ (!FLAGS_SET(flags, TERMINAL_RESET_AVOID_ANSI_SEQ) &&
            (FLAGS_SET(flags, TERMINAL_RESET_FORCE_ANSI_SEQ) || !getenv_terminal_is_dumb()))
                RET_GATHER(r, terminal_reset_ansi_seq(fd));

        തിരികെ r;
}

പൂർണ്ണസംഖ്യ terminal_reset_defensive_locked(പൂർണ്ണസംഖ്യ fd, TerminalResetFlags flags) {
        പ്രതിജ്ഞാ(fd >= 0);

        _cleanup_close_ പൂർണ്ണസംഖ്യ lock_fd = lock_dev_console();
        എങ്കിൽ (lock_fd < 0)
                log_debug_errno(lock_fd, "Failed to acquire lock for /dev/console, ignoring: %m");

        തിരികെ terminal_reset_defensive(fd, flags);
}

ശൂന്യം termios_disable_echo(ഘടന termios *termios) {
        പ്രതിജ്ഞാ(termios);

        termios->c_lflag &= ~(ICANON|ECHO);
        termios->c_cc[VMIN] = 1;
        termios->c_cc[VTIME] = 0;
}

സ്ഥിതം bool termios_is_null(സ്ഥിരം ഘടന termios *t) {
        എങ്കിൽ (!t)
                തിരികെ true;

        തിരികെ t->c_iflag == UINT_MAX &&
               t->c_oflag == UINT_MAX &&
               t->c_cflag == UINT_MAX &&
               t->c_lflag == UINT_MAX;
}

ശൂന്യം termios_reset(സ്ഥിരം TermiosResetContext *c) {
        പ്രതിജ്ഞാ(c);

        PROTECT_ERRNO;

        എങ്കിൽ (c->fd && *c->fd >= 0 && !termios_is_null(c->termios))
                (ശൂന്യം) tcsetattr(*c->fd, TCSANOW, c->termios);
}

തരനിർവചനം എണ്ണൽ BackgroundColorState {
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

തരനിർവചനം ഘടന BackgroundColorContext {
        BackgroundColorState state;
        uint32_t red, green, blue;
        unsigned red_bits, green_bits, blue_bits;
} BackgroundColorContext;

സ്ഥിതം പൂർണ്ണസംഖ്യ scan_background_color_response(
                BackgroundColorContext *context,
                സ്ഥിരം അക്ഷരം *buf,
                size_t size,
                size_t *ret_processed) {

        പ്രതിജ്ഞാ(context);
        പ്രതിജ്ഞാ(buf);
        പ്രതിജ്ഞാ(ret_processed);

        വേണ്ടി (size_t i = 0; i < size; i++) {
                അക്ഷരം c = buf[i];

                തിരഞ്ഞെടുപ്പ് (context->state) {

                അവസ്ഥ BACKGROUND_TEXT:
                        context->state = c == '\x1B' ? BACKGROUND_ESCAPE : BACKGROUND_TEXT;
                        നിർത്തുക;

                അവസ്ഥ BACKGROUND_ESCAPE:
                        context->state = c == ']' ? BACKGROUND_BRACKET : BACKGROUND_TEXT;
                        നിർത്തുക;

                അവസ്ഥ BACKGROUND_BRACKET:
                        context->state = c == '1' ? BACKGROUND_FIRST_ONE : BACKGROUND_TEXT;
                        നിർത്തുക;

                അവസ്ഥ BACKGROUND_FIRST_ONE:
                        context->state = c == '1' ? BACKGROUND_SECOND_ONE : BACKGROUND_TEXT;
                        നിർത്തുക;

                അവസ്ഥ BACKGROUND_SECOND_ONE:
                        context->state = c == ';' ? BACKGROUND_SEMICOLON : BACKGROUND_TEXT;
                        നിർത്തുക;

                അവസ്ഥ BACKGROUND_SEMICOLON:
                        context->state = c == 'r' ? BACKGROUND_R : BACKGROUND_TEXT;
                        നിർത്തുക;

                അവസ്ഥ BACKGROUND_R:
                        context->state = c == 'g' ? BACKGROUND_G : BACKGROUND_TEXT;
                        നിർത്തുക;

                അവസ്ഥ BACKGROUND_G:
                        context->state = c == 'b' ? BACKGROUND_B : BACKGROUND_TEXT;
                        നിർത്തുക;

                അവസ്ഥ BACKGROUND_B:
                        context->state = c == ':' ? BACKGROUND_RED : BACKGROUND_TEXT;
                        നിർത്തുക;

                അവസ്ഥ BACKGROUND_RED:
                        എങ്കിൽ (c == '/')
                                context->state = context->red_bits > 0 ? BACKGROUND_GREEN : BACKGROUND_TEXT;
                        അല്ലെങ്കിൽ {
                                പൂർണ്ണസംഖ്യ d = unhexchar(c);
                                എങ്കിൽ (d < 0 || context->red_bits >= വലുപ്പം(context->red)*8)
                                        context->state = BACKGROUND_TEXT;
                                അല്ലെങ്കിൽ {
                                        context->red = (context->red << 4) | d;
                                        context->red_bits += 4;
                                }
                        }
                        നിർത്തുക;

                അവസ്ഥ BACKGROUND_GREEN:
                        എങ്കിൽ (c == '/')
                                context->state = context->green_bits > 0 ? BACKGROUND_BLUE : BACKGROUND_TEXT;
                        അല്ലെങ്കിൽ {
                                പൂർണ്ണസംഖ്യ d = unhexchar(c);
                                എങ്കിൽ (d < 0 || context->green_bits >= വലുപ്പം(context->green)*8)
                                        context->state = BACKGROUND_TEXT;
                                അല്ലെങ്കിൽ {
                                        context->green = (context->green << 4) | d;
                                        context->green_bits += 4;
                                }
                        }
                        നിർത്തുക;

                അവസ്ഥ BACKGROUND_BLUE:
                        എങ്കിൽ (c == '\x07') {
                                എങ്കിൽ (context->blue_bits > 0) {
                                        *ret_processed = i + 1;
                                        തിരികെ 1; /* success! */
                                }

                                context->state = BACKGROUND_TEXT;
                        } അല്ലെങ്കിൽ എങ്കിൽ (c == '\x1b')
                                context->state = context->blue_bits > 0 ? BACKGROUND_STRING_TERMINATOR : BACKGROUND_TEXT;
                        അല്ലെങ്കിൽ {
                                പൂർണ്ണസംഖ്യ d = unhexchar(c);
                                എങ്കിൽ (d < 0 || context->blue_bits >= വലുപ്പം(context->blue)*8)
                                        context->state = BACKGROUND_TEXT;
                                അല്ലെങ്കിൽ {
                                        context->blue = (context->blue << 4) | d;
                                        context->blue_bits += 4;
                                }
                        }
                        നിർത്തുക;

                അവസ്ഥ BACKGROUND_STRING_TERMINATOR:
                        എങ്കിൽ (c == '\\') {
                                *ret_processed = i + 1;
                                തിരികെ 1; /* success! */
                        }

                        context->state = c == ']' ? BACKGROUND_ESCAPE : BACKGROUND_TEXT;
                        നിർത്തുക;

                }

                /* Reset any colors we might have picked up */
                എങ്കിൽ (IN_SET(context->state, BACKGROUND_TEXT, BACKGROUND_ESCAPE)) {
                        /* reset color */
                        context->red = context->green = context->blue = 0;
                        context->red_bits = context->green_bits = context->blue_bits = 0;
                }
        }

        *ret_processed = size;
        തിരികെ 0; /* all good, but not enough data yet */
}

പൂർണ്ണസംഖ്യ get_default_background_color(ഇരട്ട *ret_red, ഇരട്ട *ret_green, ഇരട്ട *ret_blue) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(ret_red);
        പ്രതിജ്ഞാ(ret_green);
        പ്രതിജ്ഞാ(ret_blue);

        എങ്കിൽ (!colors_enabled())
                തിരികെ -EOPNOTSUPP;

        r = terminal_verify_same(STDIN_FILENO, STDOUT_FILENO);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (streq_ptr(getenv("TERM"), "linux")) {
                /* Linux console is black */
                *ret_red = *ret_green = *ret_blue = 0.0;
                തിരികെ 0;
        }

        /* Open a 2nd input fd, in non-blocking mode, so that we won't ever hang in read()
         * should someone else process the POLLIN. Do all subsequent operations on the new fd. */
        _cleanup_close_ പൂർണ്ണസംഖ്യ nonblock_നിവേശം_fd = r = fd_reopen(STDIN_FILENO, O_RDONLY|O_CLOEXEC|O_NONBLOCK|O_NOCTTY);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        ഘടന termios old_termios = TERMIOS_NULL;
        CLEANUP_TERMIOS_RESET(nonblock_നിവേശം_fd, old_termios);

        എങ്കിൽ (tcgetattr(nonblock_നിവേശം_fd, &old_termios) < 0)
                തിരികെ -errno;

        ഘടന termios new_termios = old_termios;
        termios_disable_echo(&new_termios);

        എങ്കിൽ (tcsetattr(nonblock_നിവേശം_fd, TCSANOW, &new_termios) < 0)
                തിരികെ -errno;

        r = loop_write(STDOUT_FILENO, ANSI_OSC "11;?" ANSI_ST, SIZE_MAX);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        usec_t end = usec_add(now(CLOCK_MONOTONIC), CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        അക്ഷരം buf[STRLEN(ANSI_OSC "11;rgb:0/0/0" ANSI_ST)]; /* shortest possible reply */
        size_t buf_full = 0;
        BackgroundColorContext context = {};

        വേണ്ടി (bool first = true;; first = false) {
                എങ്കിൽ (buf_full == 0) {
                        usec_t n = now(CLOCK_MONOTONIC);
                        എങ്കിൽ (n >= end)
                                തിരികെ -EOPNOTSUPP;

                        r = fd_wait_for_event(nonblock_നിവേശം_fd, POLLIN, usec_sub_unsigned(end, n));
                        എങ്കിൽ (r < 0)
                                തിരികെ r;
                        എങ്കിൽ (r == 0)
                                തിരികെ -EOPNOTSUPP;

                        /* On the first try, read multiple characters, i.e. the shortest valid
                         * reply. Afterwards read byte-wise, since we don't want to read too much, and
                         * unnecessarily drop too many characters from the input queue. */
                        ssize_t l = പഠ(nonblock_നിവേശം_fd, buf, first ? വലുപ്പം(buf) : 1);
                        എങ്കിൽ (l < 0) {
                                എങ്കിൽ (errno == EAGAIN)
                                        തുടരുക;
                                തിരികെ -errno;
                        }

                        പ്രതിജ്ഞാ((size_t) l <= വലുപ്പം(buf));
                        buf_full = l;
                }

                size_t processed;
                r = scan_background_color_response(&context, buf, buf_full, &processed);
                എങ്കിൽ (r < 0)
                        തിരികെ r;

                പ്രതിജ്ഞാ(processed <= buf_full);
                buf_full -= processed;
                memmove(buf, buf + processed, buf_full);

                എങ്കിൽ (r > 0) {
                        പ്രതിജ്ഞാ(context.red_bits > 0);
                        *ret_red = (ഇരട്ട) context.red / ((UINT64_C(1) << context.red_bits) - 1);
                        പ്രതിജ്ഞാ(context.green_bits > 0);
                        *ret_green = (ഇരട്ട) context.green / ((UINT64_C(1) << context.green_bits) - 1);
                        പ്രതിജ്ഞാ(context.blue_bits > 0);
                        *ret_blue = (ഇരട്ട) context.blue / ((UINT64_C(1) << context.blue_bits) - 1);
                        തിരികെ 0;
                }
        }
}

/* Determine terminal dimensions by means of ANSI sequences: save the cursor via DECSC, position it far
 * to the bottom right (clamped to actual terminal dimensions), read back via DSR where we ended up, and
 * restore cursor via DECRC. Only needs a single DSR round-trip, and always restores the cursor regardless
 * of whether the response is received.
 *
 * Caller must have already opened a non-blocking input fd and configured termios (echo/icanon off). */
സ്ഥിതം പൂർണ്ണസംഖ്യ terminal_query_size_by_dsr(
                പൂർണ്ണസംഖ്യ nonblock_നിവേശം_fd,
                പൂർണ്ണസംഖ്യ output_fd,
                unsigned *ret_rows,
                unsigned *ret_columns) {

        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(nonblock_നിവേശം_fd >= 0);
        പ്രതിജ്ഞാ(output_fd >= 0);

        /* Use DECSC/DECRC to save/restore cursor instead of querying position via DSR. This way the cursor
         * is always restored — even on timeout — and we only need one DSR response instead of two. */
        r = loop_write_full(output_fd,
                            "\x1B" "7"              /* DECSC: save cursor position */
                            "\x1B[32766;32766H"     /* CUP: position cursor far to the right and to the bottom, staying within 16bit signed range */
                            "\x1B[6n"               /* DSR: request cursor position (CPR) */
                            "\x1B" "8",             /* DECRC: restore cursor position */
                            SIZE_MAX,
                            CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        usec_t end = usec_add(now(CLOCK_MONOTONIC), CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        അക്ഷരം buf[STRLEN("\x1B[1;1R")]; /* The shortest valid reply possible */
        size_t buf_full = 0;
        CursorPositionContext context = {};

        വേണ്ടി (bool first = true;; first = false) {
                എങ്കിൽ (buf_full == 0) {
                        usec_t n = now(CLOCK_MONOTONIC);
                        എങ്കിൽ (n >= end)
                                തിരികെ -EOPNOTSUPP;

                        r = fd_wait_for_event(nonblock_നിവേശം_fd, POLLIN, usec_sub_unsigned(end, n));
                        എങ്കിൽ (r < 0)
                                തിരികെ r;
                        എങ്കിൽ (r == 0)
                                തിരികെ -EOPNOTSUPP;

                        /* On the first try, read multiple characters, i.e. the shortest valid
                         * reply. Afterwards read byte-wise, since we don't want to read too much, and
                         * unnecessarily drop too many characters from the input queue. */
                        ssize_t l = പഠ(nonblock_നിവേശം_fd, buf, first ? വലുപ്പം(buf) : 1);
                        എങ്കിൽ (l < 0) {
                                എങ്കിൽ (errno == EAGAIN)
                                        തുടരുക;

                                തിരികെ -errno;
                        }

                        പ്രതിജ്ഞാ((size_t) l <= വലുപ്പം(buf));
                        buf_full = l;
                }

                size_t processed;
                r = scan_cursor_position_response(&context, buf, buf_full, &processed);
                എങ്കിൽ (r < 0)
                        തിരികെ r;

                പ്രതിജ്ഞാ(processed <= buf_full);
                buf_full -= processed;
                memmove(buf, buf + processed, buf_full);

                എങ്കിൽ (r > 0) {
                        /* Superficial validity checks (no particular reason to check for < 4, it's
                         * just a way to look for unreasonably small values) */
                        എങ്കിൽ (context.row < 4 || context.column < 4 || context.row >= 32766 || context.column >= 32766)
                                തിരികെ -ENODATA;

                        എങ്കിൽ (ret_rows)
                                *ret_rows = context.row;
                        എങ്കിൽ (ret_columns)
                                *ret_columns = context.column;

                        തിരികെ 0;
                }
        }
}

/* Common setup for ANSI terminal queries: validate the fds, open a non-blocking input fd, and configure
 * termios with echo and canonical mode disabled. Caller must restore termios and close the fd when done. */
സ്ഥിതം പൂർണ്ണസംഖ്യ terminal_prepare_query(
                പൂർണ്ണസംഖ്യ നിവേശം_fd,
                പൂർണ്ണസംഖ്യ output_fd,
                പൂർണ്ണസംഖ്യ *ret_nonblock_fd,
                ഘടന termios *ret_saved_termios) {

        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(നിവേശം_fd >= 0);
        പ്രതിജ്ഞാ(output_fd >= 0);
        പ്രതിജ്ഞാ(ret_nonblock_fd);
        പ്രതിജ്ഞാ(ret_saved_termios);

        /* Use getenv_terminal_is_dumb() instead of terminal_is_dumb() here since we operate on an
         * explicitly passed fd, not on stdio. terminal_is_dumb() additionally checks on_tty() which
         * tests whether *stderr* is a tty — that's irrelevant when we're querying a directly opened
         * terminal such as /dev/console. */
        എങ്കിൽ (getenv_terminal_is_dumb())
                തിരികെ -EOPNOTSUPP;

        r = terminal_verify_same(നിവേശം_fd, output_fd);
        എങ്കിൽ (r < 0)
                തിരികെ log_debug_errno(r, "Called with distinct input/output fds: %m");

        /* Open a 2nd input fd, in non-blocking mode, so that we won't ever hang in read()
         * should someone else process the POLLIN. Do all subsequent operations on the new fd. */
        _cleanup_close_ പൂർണ്ണസംഖ്യ nonblock_നിവേശം_fd = r = fd_reopen(നിവേശം_fd, O_RDONLY|O_CLOEXEC|O_NONBLOCK|O_NOCTTY);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (tcgetattr(nonblock_നിവേശം_fd, ret_saved_termios) < 0)
                തിരികെ log_debug_errno(errno, "Failed to get terminal settings: %m");

        ഘടന termios new_termios = *ret_saved_termios;
        termios_disable_echo(&new_termios);

        എങ്കിൽ (tcsetattr(nonblock_നിവേശം_fd, TCSANOW, &new_termios) < 0)
                തിരികെ log_debug_errno(errno, "Failed to set new terminal settings: %m");

        *ret_nonblock_fd = TAKE_FD(nonblock_നിവേശം_fd);
        തിരികെ 0;
}

/*
 * See https://terminalguide.namepad.de/seq/csi_st-18/,
 * https://invisible-island.net/xterm/ctlseqs/ctlseqs.html#h3-Functions-using-CSI-_-ordered-by-the-final-character_s_.
 */
#define CSI18_Q  "\x1B[18t"               /* Report the size of the text area in characters */
#define CSI18_Rp "\x1B[8;"                /* Reply prefix */
#define CSI18_R0 CSI18_Rp "1;1t"          /* Shortest reply */
#define CSI18_R1 CSI18_Rp "32766;32766t"  /* Longest reply */

സ്ഥിതം പൂർണ്ണസംഖ്യ scan_text_area_size_response(
                സ്ഥിരം അക്ഷരം *buf,
                size_t size,
                unsigned *ret_rows,
                unsigned *ret_columns) {

        പ്രതിജ്ഞാ(buf);
        പ്രതിജ്ഞാ(ret_rows);
        പ്രതിജ്ഞാ(ret_columns);

        /* Check if we have enough space for the shortest possible answer. */
        എങ്കിൽ (size < STRLEN(CSI18_R0))
                തിരികെ -EAGAIN;

        /* Check if the terminating sequence is present */
        എങ്കിൽ (buf[size - 1] != 't')
                തിരികെ -EAGAIN;

        unsigned short rows, columns;
        എങ്കിൽ (sscanf(buf, CSI18_Rp "%hu;%hut", &rows, &columns) != 2)
                തിരികെ -EINVAL;

        *ret_rows = rows;
        *ret_columns = columns;
        തിരികെ 0;
}

/* Determine terminal dimensions by means of an ANSI CSI 18 sequence.
 *
 * Caller must have already opened a non-blocking input fd and configured termios (echo/icanon off). */
സ്ഥിതം പൂർണ്ണസംഖ്യ terminal_query_size_by_csi18(
                പൂർണ്ണസംഖ്യ nonblock_നിവേശം_fd,
                പൂർണ്ണസംഖ്യ output_fd,
                unsigned *ret_rows,
                unsigned *ret_columns) {

        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(nonblock_നിവേശം_fd >= 0);
        പ്രതിജ്ഞാ(output_fd >= 0);

        r = loop_write_full(output_fd, CSI18_Q, SIZE_MAX, CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        usec_t end = usec_add(now(CLOCK_MONOTONIC), CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        അക്ഷരം buf[STRLEN(CSI18_R1)];
        size_t bytes = 0;

        വേണ്ടി (;;) {
                usec_t n = now(CLOCK_MONOTONIC);
                എങ്കിൽ (n >= end)
                        തിരികെ -EOPNOTSUPP;

                r = fd_wait_for_event(nonblock_നിവേശം_fd, POLLIN, usec_sub_unsigned(end, n));
                എങ്കിൽ (r < 0)
                        തിരികെ r;
                എങ്കിൽ (r == 0)
                        തിരികെ -EOPNOTSUPP;

                /* On the first read, read multiple characters, i.e. the shortest valid reply. Afterwards
                 * read byte by byte, since we don't want to read too much and drop characters from the input
                 * queue. */
                ssize_t l = പഠ(nonblock_നിവേശം_fd, buf + bytes, bytes == 0 ? STRLEN(CSI18_R0) : 1);
                എങ്കിൽ (l < 0) {
                        എങ്കിൽ (errno == EAGAIN)
                                തുടരുക;
                        തിരികെ -errno;
                }

                പ്രതിജ്ഞാ((size_t) l <= വലുപ്പം(buf) - bytes);
                bytes += l;

                r = scan_text_area_size_response(buf, bytes, ret_rows, ret_columns);
                എങ്കിൽ (r != -EAGAIN)
                        തിരികെ r;

                എങ്കിൽ (bytes == വലുപ്പം(buf))
                        തിരികെ -EOPNOTSUPP; /* The response has the right prefix, but we didn't find a valid
                                             * answer with a terminator in the allotted space. Something is
                                             * wrong, possibly some unrelated bytes got injected into the
                                             * answer. */
        }
}

പൂർണ്ണസംഖ്യ terminal_get_size(
                പൂർണ്ണസംഖ്യ നിവേശം_fd,
                പൂർണ്ണസംഖ്യ output_fd,
                unsigned *ret_rows,
                unsigned *ret_columns,
                bool try_dsr,
                bool try_csi18) {

        _cleanup_close_ പൂർണ്ണസംഖ്യ nonblock_നിവേശം_fd = -EBADF;
        ഘടന termios old_termios = TERMIOS_NULL;
        CLEANUP_TERMIOS_RESET(nonblock_നിവേശം_fd, old_termios);
        _cleanup_(nonblock_resetp) പൂർണ്ണസംഖ്യ nonblock_reset = -EBADF;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(try_dsr || try_csi18);

        r = terminal_prepare_query(നിവേശം_fd, output_fd, &nonblock_നിവേശം_fd, &old_termios);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        /* Put the output fd in non-blocking mode with a write timeout, to avoid blocking indefinitely on
         * write if the terminal is not consuming data (e.g. serial console with flow control). */
        r = fd_nonblock(output_fd, true);
        എങ്കിൽ (r < 0)
                തിരികെ log_debug_errno(r, "Failed to set terminal to non-blocking mode: %m");
        എങ്കിൽ (r > 0)
                nonblock_reset = output_fd;

        /* Flush any stale input that might confuse the response parsers. */
        (ശൂന്യം) tcflush(nonblock_നിവേശം_fd, TCIFLUSH);

        എങ്കിൽ (try_csi18) {
                r = terminal_query_size_by_csi18(nonblock_നിവേശം_fd, output_fd, ret_rows, ret_columns);
                എങ്കിൽ (r >= 0)
                        തിരികെ r;

                /* Query failed. Flush any outstanding input. */
                (ശൂന്യം) tcflush(nonblock_നിവേശം_fd, TCIFLUSH);

                എങ്കിൽ (!IN_SET(r, -EOPNOTSUPP, -EINVAL))
                        തിരികെ r;
        }

        എങ്കിൽ (try_dsr) {
                r = terminal_query_size_by_dsr(nonblock_നിവേശം_fd, output_fd, ret_rows, ret_columns);
                എങ്കിൽ (r >= 0)
                        തിരികെ r;

                /* Query failed. Flush any outstanding input. */
                (ശൂന്യം) tcflush(nonblock_നിവേശം_fd, TCIFLUSH);
        }

        തിരികെ r;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ fd_is_vc(പൂർണ്ണസംഖ്യ fd) {
        _cleanup_free_ അക്ഷരം *name = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(fd >= 0);

        r = getttyname_harder(fd, &name);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ tty_is_vc_resolve(name);
}

പൂർണ്ണസംഖ്യ terminal_fix_size(പൂർണ്ണസംഖ്യ നിവേശം_fd, പൂർണ്ണസംഖ്യ output_fd) {
        unsigned rows, columns;
        പൂർണ്ണസംഖ്യ r;

        /* Don't ask a virtual console at all. There is no intermediary here that could be wrong about the
         * dimensions: the kernel renders the screen itself, so its winsize is authoritative. */
        r = fd_is_vc(നിവേശം_fd);
        എങ്കിൽ (r < 0)
                log_debug_errno(r, "Failed to determine whether terminal is a virtual console, assuming it is not: %m");
        അല്ലെങ്കിൽ എങ്കിൽ (r > 0) {
                log_debug("Not querying dimensions of a virtual console, kernel dimensions are authoritative.");
                തിരികെ 0;
        }

        ഘടന winsize ws = {};
        എങ്കിൽ (ioctl(output_fd, TIOCGWINSZ, &ws) < 0)
                തിരികെ log_debug_errno(errno, "Failed to query terminal dimensions, ignoring: %m");

        r = terminal_get_size(നിവേശം_fd, output_fd, &rows, &columns, /* try_dsr= */ true, /* try_csi18= */ true);
        എങ്കിൽ (r < 0)
                തിരികെ log_debug_errno(r, "Failed to acquire terminal dimensions via ANSI sequences, not adjusting terminal dimensions: %m");

        എങ്കിൽ (ws.ws_row == rows && ws.ws_col == columns) {
                log_debug("Terminal dimensions reported via ANSI sequences match currently set terminal dimensions, not changing.");
                തിരികെ 0;
        }

        ws.ws_col = columns;
        ws.ws_row = rows;

        എങ്കിൽ (ioctl(output_fd, TIOCSWINSZ, &ws) < 0)
                തിരികെ log_debug_errno(errno, "Failed to update terminal dimensions, ignoring: %m");

        log_debug("Fixed terminal dimensions to %ux%u based on ANSI sequence information.", columns, rows);
        തിരികെ 1;
}

#define MAX_TERMINFO_LENGTH 64
/* python -c 'print("".join(hex(ord(i))[2:] for i in "name").upper())' */
#define DCS_TERMINFO_Q ANSI_DCS "+q" "6E616D65" ANSI_ST
/* The answer is either 0+r… (invalid) or 1+r… (OK). */
#define DCS_TERMINFO_R0 ANSI_DCS "0+r" ANSI_ST
#define DCS_TERMINFO_R1 ANSI_DCS "1+r" "6E616D65" "=" /* This is followed by Pt ST. */
assert_cc(STRLEN(DCS_TERMINFO_R0) <= STRLEN(DCS_TERMINFO_R1 ANSI_ST));

സ്ഥിതം പൂർണ്ണസംഖ്യ scan_terminfo_response(
                സ്ഥിരം അക്ഷരം *buf,
                size_t size,
                അക്ഷരം **ret_name) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(buf);
        പ്രതിജ്ഞാ(ret_name);

        /* Check if we have enough space for the shortest possible answer. */
        എങ്കിൽ (size < STRLEN(DCS_TERMINFO_R0))
                തിരികെ -EAGAIN;

        /* Check if the terminating sequence is present */
        എങ്കിൽ (memcmp(buf + size - STRLEN(ANSI_ST), ANSI_ST, STRLEN(ANSI_ST)) != 0)
                തിരികെ -EAGAIN;

        എങ്കിൽ (size <= STRLEN(DCS_TERMINFO_R1 ANSI_ST))
                തിരികെ -EINVAL;  /* The answer is invalid or empty */

        എങ്കിൽ (memcmp(buf, DCS_TERMINFO_R1, STRLEN(DCS_TERMINFO_R1)) != 0)
                തിരികെ -EINVAL;  /* The answer is not valid */

        _cleanup_free_ ശൂന്യം *dec = ശൂന്യ;
        size_t dec_size;
        r = unhexmem_full(buf + STRLEN(DCS_TERMINFO_R1), size - STRLEN(DCS_TERMINFO_R1 ANSI_ST),
                          /* secure= */ false,
                          &dec, &dec_size);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        പ്രതിജ്ഞാ(((സ്ഥിരം അക്ഷരം *) dec)[dec_size] == '\0'); /* unhexmem appends NUL for our convenience */
        എങ്കിൽ (memchr(dec, '\0', dec_size) || string_has_cc(dec, ശൂന്യ) || !filename_is_valid(dec))
                തിരികെ -EUCLEAN;

        *ret_name = TAKE_PTR(dec);
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ terminal_get_terminfo_by_dcs(പൂർണ്ണസംഖ്യ fd, അക്ഷരം **ret_name) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(fd >= 0);
        പ്രതിജ്ഞാ(ret_name);

        /* Note: fd must be in non-blocking read-write mode! */

        ഘടന termios old_termios = TERMIOS_NULL;
        CLEANUP_TERMIOS_RESET(fd, old_termios);

        എങ്കിൽ (tcgetattr(fd, &old_termios) < 0)
                തിരികെ -errno;

        ഘടന termios new_termios = old_termios;
        termios_disable_echo(&new_termios);

        എങ്കിൽ (tcsetattr(fd, TCSANOW, &new_termios) < 0)
                തിരികെ -errno;

        r = loop_write(fd, DCS_TERMINFO_Q, SIZE_MAX);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        usec_t end = usec_add(now(CLOCK_MONOTONIC), CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        അക്ഷരം buf[STRLEN(DCS_TERMINFO_R1) + MAX_TERMINFO_LENGTH + STRLEN(ANSI_ST)];
        size_t bytes = 0;

        വേണ്ടി (;;) {
                usec_t n = now(CLOCK_MONOTONIC);
                എങ്കിൽ (n >= end)
                        തിരികെ -EOPNOTSUPP;

                r = fd_wait_for_event(fd, POLLIN, usec_sub_unsigned(end, n));
                എങ്കിൽ (r < 0)
                        തിരികെ r;
                എങ്കിൽ (r == 0)
                        തിരികെ -EOPNOTSUPP;

                /* On the first read, read multiple characters, i.e. the shortest valid reply. Afterwards
                 * read byte by byte, since we don't want to read too much and drop characters from the input
                 * queue. */
                ssize_t l = പഠ(fd, buf + bytes, bytes == 0 ? STRLEN(DCS_TERMINFO_R0) : 1);
                എങ്കിൽ (l < 0) {
                        എങ്കിൽ (errno == EAGAIN)
                                തുടരുക;
                        തിരികെ -errno;
                }

                പ്രതിജ്ഞാ((size_t) l <= വലുപ്പം(buf) - bytes);
                bytes += l;

                r = scan_terminfo_response(buf, bytes, ret_name);
                എങ്കിൽ (r != -EAGAIN)
                        തിരികെ r;

                എങ്കിൽ (bytes == വലുപ്പം(buf))
                        തിരികെ -EOPNOTSUPP; /* The response has the right prefix, but we didn't find a valid
                                             * answer with a terminator in the allotted space. Something is
                                             * wrong, possibly some unrelated bytes got injected into the
                                             * answer. */
        }
}

പൂർണ്ണസംഖ്യ have_terminfo_file(സ്ഥിരം അക്ഷരം *name) {
        /* This is a heuristic check if we have the file, using the directory layout used on
         * current Linux systems. Checks for other layouts can be added later if appropriate. */
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(filename_is_valid(name));

        _cleanup_free_ അക്ഷരം *p = path_join("/usr/share/terminfo", CHAR_TO_STR(name[0]), name);
        എങ്കിൽ (!p)
                തിരികെ log_oom_debug();

        r = RET_NERRNO(access(p, F_OK));
        എങ്കിൽ (r == -ENOENT)
                തിരികെ false;
        എങ്കിൽ (r < 0)
                തിരികെ r;
        തിരികെ true;
}

പൂർണ്ണസംഖ്യ query_term_for_tty(സ്ഥിരം അക്ഷരം *tty, അക്ഷരം **ret_term) {
        _cleanup_free_ അക്ഷരം *dcs_term = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(tty);
        പ്രതിജ്ഞാ(ret_term);

        എങ്കിൽ (tty_is_vc_resolve(tty))
                തിരികെ strdup_to(ret_term, "linux");

        /* Try to query the terminal implementation that we're on. This will not work in all
         * cases, which is fine, since this is intended to be used as a fallback. */

        _cleanup_close_ പൂർണ്ണസംഖ്യ tty_fd = open_terminal(tty, O_RDWR|O_NOCTTY|O_CLOEXEC|O_NONBLOCK);
        എങ്കിൽ (tty_fd < 0)
                തിരികെ log_debug_errno(tty_fd, "Failed to open %s to query terminfo: %m", tty);

        r = terminal_get_terminfo_by_dcs(tty_fd, &dcs_term);
        എങ്കിൽ (r < 0)
                തിരികെ log_debug_errno(r, "Failed to query %s for terminfo: %m", tty);

        r = have_terminfo_file(dcs_term);
        എങ്കിൽ (r < 0)
                തിരികെ log_debug_errno(r, "Failed to look for terminfo %s: %m", dcs_term);
        എങ്കിൽ (r == 0)
                തിരികെ log_info_errno(SYNTHETIC_ERRNO(ENODATA),
                                      "Terminfo %s not found for %s.", dcs_term, tty);

        *ret_term = TAKE_PTR(dcs_term);
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ terminal_is_pty_fd(പൂർണ്ണസംഖ്യ fd) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(fd >= 0);

        /* Returns true if we are looking at a pty, i.e. if it's backed by the /dev/pts/ file system */

        എങ്കിൽ (!isatty_safe(fd))
                തിരികെ false;

        r = is_fs_type_at(fd, ശൂന്യ, DEVPTS_SUPER_MAGIC);
        എങ്കിൽ (r != 0)
                തിരികെ r;

        /* The ptmx device is weird, it exists twice, once inside and once outside devpts. To detect the
         * latter case, let's fire off an ioctl() that only works on ptmx devices. */

        പൂർണ്ണസംഖ്യ v;
        എങ്കിൽ (ioctl(fd, TIOCGPKT, &v) < 0) {
                എങ്കിൽ (ERRNO_IS_NOT_SUPPORTED(errno))
                        തിരികെ false;

                തിരികെ -errno;
        }

        തിരികെ true;
}

പൂർണ്ണസംഖ്യ pty_open_peer(പൂർണ്ണസംഖ്യ fd, പൂർണ്ണസംഖ്യ mode) {
        പ്രതിജ്ഞാ(fd >= 0);

        /* Opens the peer PTY using the new race-free TIOCGPTPEER ioctl() (kernel 4.13).
         *
         * This is safe to be called on TTYs from other namespaces. */

        പ്രതിജ്ഞാ((mode & (O_CREAT|O_PATH|O_DIRECTORY|O_TMPFILE)) == 0);

        /* This replicates the EIO retry logic of open_terminal() in a modified way. */
        വേണ്ടി (unsigned c = 0;; c++) {
                പൂർണ്ണസംഖ്യ peer_fd = ioctl(fd, TIOCGPTPEER, mode);
                എങ്കിൽ (peer_fd >= 0)
                        തിരികെ peer_fd;

                എങ്കിൽ (errno != EIO)
                        തിരികെ -errno;

                /* Max 1s in total */
                എങ്കിൽ (c >= 20)
                        തിരികെ -EIO;

                (ശൂന്യം) usleep_safe(50 * USEC_PER_MSEC);
        }
}
