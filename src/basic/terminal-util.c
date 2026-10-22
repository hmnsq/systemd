/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश <fcntl.h>
#समावेश <linux/kd.h>
#समावेश <linux/magic.h>
#समावेश <linux/tiocl.h>
#समावेश <linux/vt.h>
#समावेश <poll.h>
#समावेश <signal.h>
#समावेश <stdlib.h>
#समावेश <sys/inotify.h>
#समावेश <sys/ioctl.h>
#समावेश <sys/sysmacros.h>
#समावेश <termios.h>
#समावेश <time.h>
#समावेश <unistd.h>

#समावेश "alloc-util.h"
#समावेश "ansi-color.h"
#समावेश "chase.h"
#समावेश "devnum-util.h"
#समावेश "errno-util.h"
#समावेश "extract-word.h"
#समावेश "fd-util.h"
#समावेश "fileio.h"
#समावेश "fs-util.h"
#समावेश "hexdecoct.h"
#समावेश "inotify-util.h"
#समावेश "io-util.h"
#समावेश "log.h"
#समावेश "namespace-util.h"
#समावेश "parse-util.h"
#समावेश "path-util.h"
#समावेश "pidref.h"
#समावेश "proc-cmdline.h"
#समावेश "process-util.h"
#समावेश "signal-util.h"
#समावेश "socket-util.h"
#समावेश "stat-util.h"
#समावेश "stdio-util.h"
#समावेश "string-util.h"
#समावेश "strv.h"
#समावेश "terminal-util.h"
#समावेश "time-util.h"
#समावेश "utf8.h"

/* How much to wait when reading/writing ANSI sequences from/to the console */
#घोषणा CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC (333 * USEC_PER_MSEC)

अटल अस्थिर अचिन्हित cached_columns = 0;
अटल अस्थिर अचिन्हित cached_पंक्तियाँ = 0;

अटल अस्थिर पूर्णांक cached_on_tty = -1;
अटल अस्थिर पूर्णांक cached_on_dev_null = -1;

bool isatty_safe(पूर्णांक fd) {
        निश्चित(fd >= 0);

        अगर (isatty(fd))
                वापस true;

        /* Linux/glibc returns EIO for hung up TTY on isatty(). Which is wrong, the thing doesn't stop being
         * a TTY after all, just because it is temporarily hung up. Let's work around this here, until this
         * is fixed in glibc. See: https://sourceware.org/bugzilla/show_bug.cgi?id=32103 */
        अगर (त्रुटि_सं == EIO)
                वापस true;

        /* Be resilient if we're working on stdio, since they're set up by parent process. */
        निश्चित(त्रुटि_सं != EBADF || IN_SET(fd, STDIN_FILENO, STDOUT_FILENO, STDERR_FILENO));

        वापस false;
}

पूर्णांक chvt(पूर्णांक vt) {
        _cleanup_close_ पूर्णांक fd = -EBADF;

        /* Switch to the specified vt number. If the VT is specified <= 0 switch to the VT the kernel log messages go,
         * if that's configured. */

        fd = open_terminal("/dev/tty0", O_RDWR|O_NOCTTY|O_CLOEXEC|O_NONBLOCK);
        अगर (fd < 0)
                वापस fd;

        अगर (vt <= 0) {
                पूर्णांक tiocl[2] = {
                        TIOCL_GETKMSGREDIRECT,
                        0
                };

                अगर (ioctl(fd, TIOCLINUX, tiocl) < 0)
                        वापस -त्रुटि_सं;

                vt = tiocl[0] <= 0 ? 1 : tiocl[0];
        }

        वापस RET_NERRNO(ioctl(fd, VT_ACTIVATE, vt));
}

पूर्णांक read_one_char(खाता *f, अक्षर *ret, usec_t t, bool echo, bool *need_nl) {
        _cleanup_free_ अक्षर *पंक्ति = शून्य;
        काष्ठा termios old_termios;
        पूर्णांक r, fd;

        निश्चित(ret);

        अगर (!f)
                f = मानक_निवेश;

        /* If this is a terminal, then switch canonical mode off, so that we can read a single
         * character. (Note that fmemopen() streams do not have an fd associated with them, let's handle that
         * nicely.) If 'echo' is false we'll also disable ECHO mode so that the pressed key is not made
         * visible to the user. */
        fd = fileno(f);
        अगर (fd >= 0 && tcgetattr(fd, &old_termios) >= 0) {
                काष्ठा termios new_termios = old_termios;

                new_termios.c_lflag &= ~(ICANON|(echo ? 0 : ECHO));
                new_termios.c_cc[VMIN] = 1;
                new_termios.c_cc[VTIME] = 0;

                अगर (tcsetattr(fd, TCSANOW, &new_termios) >= 0) {
                        अक्षर c;

                        अगर (t != USEC_INFINITY) {
                                अगर (fd_wait_for_event(fd, POLLIN, t) <= 0) {
                                        (व्योम) tcsetattr(fd, TCSANOW, &old_termios);
                                        वापस -ETIMEDOUT;
                                }
                        }

                        r = safe_fgetc(f, &c);
                        (व्योम) tcsetattr(fd, TCSANOW, &old_termios);
                        अगर (r < 0)
                                वापस r;
                        अगर (r == 0)
                                वापस -EIO;

                        अगर (need_nl)
                                *need_nl = c != '\n';

                        *ret = c;
                        वापस 0;
                }
        }

        अगर (t != USEC_INFINITY && fd >= 0) {
                /* Let's wait the specified amount of time for input. When we have no fd we skip this, under
                 * the assumption that this is an fmemopen() stream or so where waiting doesn't make sense
                 * anyway, as the data is either already in the stream or cannot possible be placed there
                 * while we access the stream */

                अगर (fd_wait_for_event(fd, POLLIN, t) <= 0)
                        वापस -ETIMEDOUT;
        }

        /* If this is not a terminal, then read a full line instead */

        r = read_पंक्ति(f, 16, &पंक्ति); /* longer than necessary, to eat up UTF-8 chars/vt100 key sequences */
        अगर (r < 0)
                वापस r;
        अगर (r == 0)
                वापस -EIO;

        अगर (म_माप(पंक्ति) != 1)
                वापस -EBADMSG;

        अगर (need_nl)
                *need_nl = false;

        *ret = पंक्ति[0];
        वापस 0;
}

#घोषणा DEFAULT_ASK_REFRESH_USEC (2*USEC_PER_SEC)

पूर्णांक ask_char(अक्षर *ret, स्थिर अक्षर *replies, स्थिर अक्षर *fmt, ...) {
        पूर्णांक r;

        निश्चित(ret);
        निश्चित(replies);
        निश्चित(fmt);

        क्रम (;;) {
                बहु_सूची ap;
                अक्षर c;
                bool need_nl = true;

                ख_माला_दो(ansi_highlight(), मानक_निकास);

                अक्षर_दो('\r');

                बहु_शुरू(ap, fmt);
                भ_लिखो(fmt, ap);
                बहु_पूर्ण(ap);

                ख_माला_दो(ansi_normal(), मानक_निकास);

                ख_साफ(मानक_निकास);

                r = read_one_char(मानक_निवेश, &c, DEFAULT_ASK_REFRESH_USEC, /* echo= */ true, &need_nl);
                अगर (r < 0) {

                        अगर (r == -ETIMEDOUT)
                                जारी;

                        अगर (r == -EBADMSG) {
                                माला_दो("Bad input, please try again.");
                                जारी;
                        }

                        अक्षर_दो('\n');
                        वापस r;
                }

                अगर (need_nl)
                        अक्षर_दो('\n');

                अगर (म_अक्षर(replies, c)) {
                        *ret = c;
                        वापस 0;
                }

                माला_दो("Read unexpected character, please try again.");
        }
}

प्रकार क्रमागत Completion_परिणाम{
        COMPLETION_ALREADY,       /* the input string is already complete */
        COMPLETION_FULL,          /* completed the input string to be complete now */
        COMPLETION_PARTIAL,       /* completed the input string so that is still incomplete */
        COMPLETION_NONE,          /* found no matching completion */
        COMPLETION_परिणाम_MAX,
        COMPLETION_परिणाम_INVALID = -EINVAL,
        COMPLETION_परिणाम_ERRNO_MAX = -ERRNO_MAX,
} Completion_परिणाम;

अटल Completion_परिणाम pick_completion(स्थिर अक्षर *string, अक्षर *स्थिर*completions, अक्षर **ret) {
        _cleanup_free_ अक्षर *found = शून्य;
        bool partial = false;

        निश्चित(ret);

        string = strempty(string);

        STRV_FOREACH(c, completions) {

                /* Ignore entries that are not actually completions */
                अगर (!startswith(*c, string))
                        जारी;

                /* Store first completion that matches */
                अगर (!found) {
                        found = strdup(*c);
                        अगर (!found)
                                वापस -ENOMEM;

                        जारी;
                }

                /* If there's another completion that works truncate the one we already found by common
                 * prefix */
                माप_प्रकार n = str_common_prefix(found, *c);
                अगर (n == SIZE_MAX)
                        जारी;

                found[n] = 0;
                partial = true;
        }

        *ret = TAKE_PTR(found);

        अगर (!*ret)
                वापस COMPLETION_NONE;
        अगर (partial)
                वापस COMPLETION_PARTIAL;

        वापस streq(string, *ret) ? COMPLETION_ALREADY : COMPLETION_FULL;
}

अटल व्योम clear_by_backspace(माप_प्रकार n) {
        /* Erase the specified number of character cells backwards on the terminal */
        क्रम (माप_प्रकार i = 0; i < n; i++)
                ख_माला_दो("\b \b", मानक_निकास);
}

पूर्णांक ask_string_full(
                अक्षर **ret,
                स्थिर अक्षर *prefill,
                GetCompletionsCallback get_completions,
                व्योम *userdata,
                स्थिर अक्षर *text, ...) {

        बहु_सूची ap;
        पूर्णांक r;

        निश्चित(ret);
        निश्चित(text);

        _cleanup_free_ अक्षर *string = शून्य;
        माप_प्रकार n = 0;

        अगर (prefill) {
                /* Prefill query with explicit data if specified */

                string = strdup(prefill);
                अगर (!string)
                        वापस -ENOMEM;

                n = म_माप(string);

        } अन्यथा अगर (get_completions) {
                /* Otherwise, figure out what string to preselect the query with */
                _cleanup_strv_free_ अक्षर **completions = शून्य;
                r = get_completions("", GET_COMPLETIONS_PRESELECT, &completions, userdata);
                अगर (r < 0)
                        वापस r;

                Completion_परिणाम cr = pick_completion(string, completions, &string);
                अगर (cr < 0)
                        वापस cr;

                n = strlen_ptr(string);
        }

        /* Output the prompt */
        ख_माला_दो(ansi_highlight(), मानक_निकास);
        बहु_शुरू(ap, text);
        भ_लिखो(text, ap);
        बहु_पूर्ण(ap);
        ख_माला_दो(ansi_normal(), मानक_निकास);
        अगर (string)
                ख_माला_दो(string, मानक_निकास);
        ख_साफ(मानक_निकास);

        /* Do interactive logic only if stdin + stdout are connected to the same place. And yes, we could use
         * STDIN_FILENO and STDOUT_FILENO here, but let's be overly correct for once, after all libc allows
         * swapping out stdin/stdout. */
        पूर्णांक fd_निवेश = fileno(मानक_निवेश);
        पूर्णांक fd_output = fileno(मानक_निकास);
        काष्ठा termios old_termios = TERMIOS_NULL;
        CLEANUP_TERMIOS_RESET(fd_निवेश, old_termios);

        अगर (fd_निवेश < 0 || fd_output < 0 || same_fd(fd_निवेश, fd_output) <= 0)
                जाओ fallback;

        /* Try to disable echo, which also tells us if this even is a terminal */
        अगर (tcgetattr(fd_निवेश, &old_termios) < 0) {
                old_termios = TERMIOS_NULL;
                जाओ fallback;
        }

        काष्ठा termios new_termios = old_termios;
        termios_disable_echo(&new_termios);
        अगर (tcsetattr(fd_निवेश, TCSANOW, &new_termios) < 0)
                वापस -त्रुटि_सं;

        क्रम (;;) {
                पूर्णांक c = ख_अक्षर_लो(मानक_निवेश);

                /* On EOF or NUL, end the request, don't output anything anymore */
                अगर (IN_SET(c, खातापूर्ण, 0))
                        अवरोध;

                /* On Return also end the request, but make this visible */
                अगर (IN_SET(c, '\n', '\r')) {
                        ख_अक्षर_दो('\n', मानक_निकास);
                        अवरोध;
                }

                अगर (c == '\t') {
                        /* Tab */

                        _cleanup_strv_free_ अक्षर **completions = शून्य;
                        अगर (get_completions) {
                                r = get_completions(string, /* flags= */ 0, &completions, userdata);
                                अगर (r < 0)
                                        वापस r;
                        }

                        _cleanup_free_ अक्षर *new_string = शून्य;
                        Completion_परिणाम cr = pick_completion(string, completions, &new_string);
                        अगर (cr < 0)
                                वापस cr;
                        अगर (IN_SET(cr, COMPLETION_PARTIAL, COMPLETION_FULL)) {
                                /* Output the new suffix we learned */
                                ख_माला_दो(ASSERT_PTR(startswith(new_string, strempty(string))), मानक_निकास);

                                /* And update the whole string */
                                free_and_replace(string, new_string);
                                n = म_माप(string);
                        }
                        अगर (cr == COMPLETION_NONE)
                                ख_अक्षर_दो('\a', मानक_निकास); /* BEL */

                        अगर (IN_SET(cr, COMPLETION_PARTIAL, COMPLETION_ALREADY)) {
                                /* If this worked only partially, or if the user hit TAB even though we were
                                 * complete already, then show the remaining options (in the latter case just
                                 * the one). */
                                ख_अक्षर_दो('\n', मानक_निकास);

                                _cleanup_strv_free_ अक्षर **filtered = strv_filter_prefix(completions, string);
                                अगर (!filtered)
                                        वापस -ENOMEM;

                                r = show_menu(filtered,
                                              /* n_columns= */ SIZE_MAX,
                                              /* column_width= */ SIZE_MAX,
                                              /* ellipsize_percentage= */ 0,
                                              /* grey_prefix= */ string,
                                              /* with_numbers= */ false);
                                अगर (r < 0)
                                        वापस r;

                                /* Show the prompt again */
                                ख_माला_दो(ansi_highlight(), मानक_निकास);
                                बहु_शुरू(ap, text);
                                भ_लिखो(text, ap);
                                बहु_पूर्ण(ap);
                                ख_माला_दो(ansi_normal(), मानक_निकास);
                                ख_माला_दो(string, मानक_निकास);
                        }

                } अन्यथा अगर (IN_SET(c, '\b', 127)) {
                        /* Backspace */

                        अगर (n == 0)
                                ख_अक्षर_दो('\a', मानक_निकास); /* BEL */
                        अन्यथा {
                                माप_प्रकार m = utf8_last_लंबाई(string, n);

                                अक्षर *e = string + n - m;
                                clear_by_backspace(utf8_console_width(e));

                                *e = 0;
                                n -= m;
                        }

                } अन्यथा अगर (c == 21) {
                        /* Ctrl-u → erase all input */

                        clear_by_backspace(utf8_console_width(string));
                        अगर (string)
                                string[n = 0] = 0;
                        अन्यथा
                                निश्चित(n == 0);

                } अन्यथा अगर (c == 4) {
                        /* Ctrl-d → cancel this field input */

                        वापस -ECANCELED;

                } अन्यथा अगर (char_is_cc(c) || n >= LINE_MAX)
                        /* refuse control characters and too long strings */
                        ख_अक्षर_दो('\a', मानक_निकास); /* BEL */
                अन्यथा {
                        /* Regular char */

                        अगर (!GREEDY_REALLOC(string, n+2))
                                वापस -ENOMEM;

                        string[n++] = (अक्षर) c;
                        string[n] = 0;

                        ख_अक्षर_दो(c, मानक_निकास);
                }

                ख_साफ(मानक_निकास);
        }

        अगर (!string) {
                string = strdup("");
                अगर (!string)
                        वापस -ENOMEM;
        }

        *ret = TAKE_PTR(string);
        वापस 0;

fallback:
        /* A simple fallback without TTY magic */
        string = mfree(string);
        r = read_पंक्ति(मानक_निवेश, LONG_पंक्ति_MAX, &string);
        अगर (r < 0)
                वापस r;
        अगर (r == 0)
                वापस -EIO;

        *ret = TAKE_PTR(string);
        वापस 0;
}

bool any_कुंजी_to_proceed(व्योम) {

        /* Insert a new line here as well as to when the user inputs, as this is also used during the boot up
         * sequence when status messages may be interleaved with the current program output. This ensures
         * that the status messages aren't appended on the same line as this message. */

        ख_अक्षर_दो('\n', मानक_निकास);
        ख_माला_दो(ansi_highlight_magenta(), मानक_निकास);
        ख_माला_दो("-- Press any key to proceed --", मानक_निकास);
        ख_माला_दो(ansi_normal(), मानक_निकास);
        ख_अक्षर_दो('\n', मानक_निकास);
        ख_साफ(मानक_निकास);

        अक्षर कुंजी = 0;
        (व्योम) read_one_char(मानक_निवेश, &कुंजी, USEC_INFINITY, /* echo= */ false, /* need_nl= */ शून्य);

        ख_अक्षर_दो('\n', मानक_निकास);
        ख_साफ(मानक_निकास);

        वापस कुंजी != 'q';
}

अटल माप_प्रकार widest_list_element(अक्षर *स्थिर*l) {
        माप_प्रकार w = 0;

        /* Returns the largest console width of all elements in 'l' */

        STRV_FOREACH(i, l)
                w = MAX(w, utf8_console_width(*i));

        वापस w;
}

पूर्णांक show_menu(अक्षर **x,
              माप_प्रकार n_columns,
              माप_प्रकार column_width,
              अचिन्हित ellipsize_percentage,
              स्थिर अक्षर *grey_prefix,
              bool with_numbers) {

        निश्चित(n_columns > 0);
        निश्चित(column_width > 0);

        अगर (n_columns == SIZE_MAX)
                n_columns = 3;

        अगर (column_width == SIZE_MAX) {
                माप_प्रकार widest = widest_list_element(x);

                /* If not specified, derive column width from screen width */
                माप_प्रकार column_max = (columns()-1) / n_columns;

                /* Subtract room for numbers */
                अगर (with_numbers)
                        column_max = LESS_BY(column_max, 6U);

                /* If columns would get too tight let's make this a linear list instead. */
                अगर (column_max < 10 && widest > 10) {
                        n_columns = 1;
                        column_max = columns()-1;

                        अगर (with_numbers)
                                column_max = LESS_BY(column_max, 6U);
                }

                /* Never make this narrower than 10 characters */
                column_max = MAX(column_max, 10U);
                column_width = CLAMP(widest+1, 10U, column_max);
        }

        माप_प्रकार n = strv_लंबाई(x);
        माप_प्रकार per_column = DIV_ROUND_UP(n, n_columns);

        माप_प्रकार break_पंक्तियाँ = पंक्तियाँ();
        अगर (break_पंक्तियाँ > 2)
                break_पंक्तियाँ--;

        /* The first page gets two extra lines, since we want to show a title */
        माप_प्रकार break_modulo = break_पंक्तियाँ;
        अगर (break_modulo > 3)
                break_modulo -= 3;

        क्रम (माप_प्रकार i = 0; i < per_column; i++) {

                क्रम (माप_प्रकार j = 0; j < n_columns; j++) {
                        _cleanup_free_ अक्षर *e = शून्य;

                        माप_प्रकार p = j * per_column + i;
                        अगर (p >= n)
                                अवरोध;

                        e = ellipsize(x[p], column_width, ellipsize_percentage);
                        अगर (!e)
                                वापस -ENOMEM;

                        अगर (with_numbers)
                                म_लिखो("%s%4zu)%s ",
                                       ansi_grey(),
                                       p + 1,
                                       ansi_normal());

                        अगर (grey_prefix && startswith(e, grey_prefix)) {
                                माप_प्रकार k = MIN(म_माप(grey_prefix), column_width);
                                म_लिखो("%s%.*s%s",
                                       ansi_grey(),
                                       (पूर्णांक) k, e,
                                       ansi_normal());
                                म_लिखो("%-*s",
                                       (पूर्णांक) (column_width - k), e+k);
                        } अन्यथा
                                म_लिखो("%-*s", (पूर्णांक) column_width, e);
                }

                अक्षर_दो('\n');

                /* on the first screen we reserve 2 extra lines for the title */
                अगर (i % break_पंक्तियाँ == break_modulo)
                        अगर (!any_कुंजी_to_proceed())
                                वापस 0;
        }

        वापस 0;
}

पूर्णांक open_terminal(स्थिर अक्षर *name, पूर्णांक mode) {
        _cleanup_close_ पूर्णांक fd = -EBADF;

        /*
         * If a TTY is in the process of being closed opening it might cause EIO. This is horribly awful, but
         * unlikely to be changed in the kernel. Hence we work around this problem by retrying a couple of
         * times.
         *
         * https://bugs.launchpad.net/ubuntu/+source/linux/+bug/554172/comments/245
         */

        निश्चित((mode & (O_CREAT|O_PATH|O_DIRECTORY|O_TMPFILE)) == 0);

        क्रम (अचिन्हित c = 0;; c++) {
                fd = खोलो(name, mode, 0);
                अगर (fd >= 0)
                        अवरोध;

                अगर (त्रुटि_सं != EIO)
                        वापस -त्रुटि_सं;

                /* Max 1s in total */
                अगर (c >= 20)
                        वापस -EIO;

                (व्योम) usleep_safe(50 * USEC_PER_MSEC);
        }

        अगर (!isatty_safe(fd))
                वापस -ENOTTY;

        वापस TAKE_FD(fd);
}

पूर्णांक acquire_terminal(
                स्थिर अक्षर *name,
                AcquireTerminalFlags flags,
                usec_t timeout) {

        _cleanup_close_ पूर्णांक notify = -EBADF, fd = -EBADF;
        usec_t ts = USEC_INFINITY;
        पूर्णांक r, wd = -1;

        निश्चित(name);

        AcquireTerminalFlags mode = flags & _ACQUIRE_TERMINAL_MODE_MASK;
        निश्चित(IN_SET(mode, ACQUIRE_TERMINAL_TRY, ACQUIRE_TERMINAL_FORCE, ACQUIRE_TERMINAL_WAIT));
        निश्चित(mode == ACQUIRE_TERMINAL_WAIT || !FLAGS_SET(flags, ACQUIRE_TERMINAL_WATCH_SIGTERM));

        /* We use inotify to be notified when the tty is closed. We create the watch before checking if we can actually
         * acquire it, so that we don't lose any event.
         *
         * Note: strictly speaking this actually watches for the device being closed, it does *not* really watch
         * whether a tty loses its controlling process. However, unless some rogue process uses TIOCNOTTY on /dev/tty
         * *after* closing its tty otherwise this will not become a problem. As long as the administrator makes sure to
         * not configure any service on the same tty as an untrusted user this should not be a problem. (Which they
         * probably should not do anyway.) */

        अगर (mode == ACQUIRE_TERMINAL_WAIT) {
                notify = inotify_init1(IN_CLOEXEC | IN_NONBLOCK);
                अगर (notify < 0)
                        वापस -त्रुटि_सं;

                wd = inotify_add_watch(notify, name, IN_CLOSE);
                अगर (wd < 0)
                        वापस -त्रुटि_सं;

                अगर (timeout != USEC_INFINITY)
                        ts = now(CLOCK_MONOTONIC);
        }

        /* If we are called with ACQUIRE_TERMINAL_WATCH_SIGTERM we'll unblock SIGTERM during ppoll() temporarily */
        sigset_t poll_ss;
        assert_se(sigprocmask(SIG_SETMASK, /* newset= */ शून्य, &poll_ss) >= 0);
        अगर (flags & ACQUIRE_TERMINAL_WATCH_SIGTERM) {
                assert_se(sigismember(&poll_ss, संक_इति) > 0);
                assert_se(sigdelset(&poll_ss, संक_इति) >= 0);
        }

        क्रम (;;) {
                अगर (notify >= 0) {
                        r = flush_fd(notify);
                        अगर (r < 0)
                                वापस r;
                }

                /* We pass here O_NOCTTY only so that we can check the return value TIOCSCTTY and have a reliable way
                 * to figure out if we successfully became the controlling process of the tty */
                fd = open_terminal(name, O_RDWR|O_NOCTTY|O_CLOEXEC);
                अगर (fd < 0)
                        वापस fd;

                /* Temporarily ignore SIGHUP, so that we don't get SIGHUP'ed if we already own the tty. */
                काष्ठा sigaction sa_old;
                assert_se(sigaction(SIGHUP, &sigaction_ignore, &sa_old) >= 0);

                /* First, try to get the tty */
                r = RET_NERRNO(ioctl(fd, TIOCSCTTY, mode == ACQUIRE_TERMINAL_FORCE));

                /* Reset signal handler to old value */
                assert_se(sigaction(SIGHUP, &sa_old, शून्य) >= 0);

                /* Success? Exit the loop now! */
                अगर (r >= 0)
                        अवरोध;

                /* Any failure besides -EPERM? Fail, regardless of the mode. */
                अगर (r != -EPERM)
                        वापस r;

                अगर (flags & ACQUIRE_TERMINAL_PERMISSIVE) /* If we are in permissive mode, then EPERM is fine, turn this
                                                          * into a success. Note that EPERM is also returned if we
                                                          * already are the owner of the TTY. */
                        अवरोध;

                अगर (mode != ACQUIRE_TERMINAL_WAIT) /* If we are in TRY or FORCE mode, then propagate EPERM as EPERM */
                        वापस r;

                निश्चित(notify >= 0);
                निश्चित(wd >= 0);

                क्रम (;;) {
                        usec_t बायाँ;
                        अगर (timeout == USEC_INFINITY)
                                बायाँ = USEC_INFINITY;
                        अन्यथा {
                                निश्चित(ts != USEC_INFINITY);

                                usec_t n = usec_sub_unsigned(now(CLOCK_MONOTONIC), ts);
                                अगर (n >= timeout)
                                        वापस -ETIMEDOUT;

                                बायाँ = timeout - n;
                        }

                        r = ppoll_usec_full(
                                        &(काष्ठा pollfd) {
                                                .fd = notify,
                                                .events = POLLIN,
                                        },
                                        /* n_fds= */ 1,
                                        बायाँ,
                                        &poll_ss);
                        अगर (r < 0)
                                वापस r;
                        अगर (r == 0)
                                वापस -ETIMEDOUT;

                        जोड़ inotify_event_बफर बफर;
                        ssize_t l;
                        l = पढ़ो(notify, &बफर, माप(बफर));
                        अगर (l < 0) {
                                अगर (ERRNO_IS_TRANSIENT(त्रुटि_सं))
                                        जारी;

                                वापस -त्रुटि_सं;
                        }

                        FOREACH_INOTIFY_EVENT(e, बफर, l) {
                                अगर (e->mask & IN_Q_OVERFLOW) /* If we hit an inotify queue overflow, simply check if the terminal is up for grabs now. */
                                        अवरोध;

                                अगर (e->wd != wd || !(e->mask & IN_CLOSE)) /* Safety checks */
                                        वापस -EIO;
                        }

                        अवरोध;
                }

                /* We close the tty fd here since if the old session ended our handle will be dead. It's important that
                 * we do this after sleeping, so that we don't enter an endless loop. */
                fd = safe_close(fd);
        }

        वापस TAKE_FD(fd);
}

पूर्णांक release_terminal(व्योम) {
        _cleanup_close_ पूर्णांक fd = -EBADF;
        पूर्णांक r;

        fd = खोलो("/dev/tty", O_RDWR|O_NOCTTY|O_CLOEXEC|O_NONBLOCK);
        अगर (fd < 0)
                वापस -त्रुटि_सं;

        /* Temporarily ignore SIGHUP, so that we don't get SIGHUP'ed
         * by our own TIOCNOTTY */
        काष्ठा sigaction sa_old;
        assert_se(sigaction(SIGHUP, &sigaction_ignore, &sa_old) >= 0);

        r = RET_NERRNO(ioctl(fd, TIOCNOTTY));

        assert_se(sigaction(SIGHUP, &sa_old, शून्य) >= 0);

        वापस r;
}

पूर्णांक terminal_new_session(व्योम) {

        /* Make us the new session leader, and set stdin tty to be our controlling terminal.
         *
         * Why stdin? Well, the ctty logic is relevant for signal delivery mostly, i.e. if people hit C-c
         * or the line is hung up. Such events are basically just a form of input, via a side channel
         * (that side channel being signal delivery, i.e. SIGINT, SIGHUP et al). Hence we focus on input,
         * not output here. */

        अगर (!isatty_safe(STDIN_FILENO))
                वापस -ENXIO;

        (व्योम) setsid();
        वापस RET_NERRNO(ioctl(STDIN_FILENO, TIOCSCTTY, 0));
}

व्योम terminal_detach_session(व्योम) {
        (व्योम) setsid();
        (व्योम) release_terminal();
}

पूर्णांक terminal_vhangup_fd(पूर्णांक fd) {
        निश्चित(fd >= 0);
        वापस RET_NERRNO(ioctl(fd, TIOCVHANGUP));
}

पूर्णांक terminal_vhangup(स्थिर अक्षर *tty) {
        _cleanup_close_ पूर्णांक fd = -EBADF;

        निश्चित(tty);

        fd = open_terminal(tty, O_RDWR|O_NOCTTY|O_CLOEXEC);
        अगर (fd < 0)
                वापस fd;

        वापस terminal_vhangup_fd(fd);
}

पूर्णांक vt_disallocate(स्थिर अक्षर *tty_path) {
        निश्चित(tty_path);

        /* Deallocate the VT if possible. If not possible (i.e. because it is the active one), at least clear
         * it entirely (including the scrollback buffer). */

        पूर्णांक ttynr = vtnr_from_tty(tty_path);
        अगर (ttynr > 0) {
                _cleanup_close_ पूर्णांक fd = open_terminal("/dev/tty0", O_RDWR|O_NOCTTY|O_CLOEXEC|O_NONBLOCK);
                अगर (fd < 0)
                        वापस fd;

                /* Try to deallocate */
                अगर (ioctl(fd, VT_DISALLOCATE, ttynr) >= 0)
                        वापस 0;
                अगर (त्रुटि_सं != EBUSY)
                        वापस -त्रुटि_सं;
        }

        /* So this is not a VT (in which case we cannot deallocate it), or we failed to deallocate. Let's at
         * least clear the screen. */

        _cleanup_close_ पूर्णांक fd2 = open_terminal(tty_path, O_WRONLY|O_NOCTTY|O_CLOEXEC|O_NONBLOCK);
        अगर (fd2 < 0)
                वापस fd2;

        वापस loop_write_full(fd2,
                               "\033[r"   /* clear scrolling region */
                               "\033[H"   /* move home */
                               "\033[3J"  /* clear screen including scrollback, requires Linux 2.6.40 */
                               "\033c",   /* reset to initial state */
                               SIZE_MAX,
                               CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
}

अटल पूर्णांक vt_default_utf8(व्योम) {
        /* Read the default VT UTF8 setting from the kernel */
        वापस read_boolean_file("/sys/module/vt/parameters/default_utf8");
}

अटल पूर्णांक vt_reset_keyboard(पूर्णांक fd) {
        पूर्णांक r, kb;

        निश्चित(fd >= 0);

        /* If we can't read the default, then default to Unicode. It's 2024 after all. */
        r = vt_default_utf8();
        अगर (r < 0)
                log_debug_errno(r, "Failed to determine kernel VT UTF-8 mode, assuming enabled: %m");

        kb = vt_default_utf8() != 0 ? K_UNICODE : K_XLATE;
        वापस RET_NERRNO(ioctl(fd, KDSKBMODE, kb));
}

अटल पूर्णांक terminal_reset_ioctl(पूर्णांक fd, bool switch_to_text) {
        काष्ठा termios termios;
        पूर्णांक r;

        /* Set terminal to some sane defaults */

        निश्चित(fd >= 0);

        /* We leave locked terminal attributes untouched, so that Plymouth may set whatever it wants to set,
         * and we don't interfere with that. */

        /* Disable exclusive mode, just in case */
        अगर (ioctl(fd, TIOCNXCL) < 0)
                log_debug_errno(त्रुटि_सं, "TIOCNXCL ioctl failed on TTY, ignoring: %m");

        /* Switch to text mode */
        अगर (switch_to_text)
                अगर (ioctl(fd, KDSETMODE, KD_TEXT) < 0)
                        log_debug_errno(त्रुटि_सं, "KDSETMODE ioctl for switching to text mode failed on TTY, ignoring: %m");

        /* Set default keyboard mode */
        r = vt_reset_keyboard(fd);
        अगर (r < 0)
                log_debug_errno(r, "Failed to reset VT keyboard, ignoring: %m");

        अगर (tcgetattr(fd, &termios) < 0) {
                r = log_debug_errno(त्रुटि_सं, "Failed to get terminal parameters: %m");
                जाओ finish;
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
        अगर (r < 0)
                log_debug_errno(r, "Failed to set terminal parameters: %m");

finish:
        /* Just in case, flush all crap out */
        (व्योम) tcflush(fd, TCIOFLUSH);

        वापस r;
}

पूर्णांक terminal_reset_ansi_seq(पूर्णांक fd) {
        _cleanup_(nonblock_resetp) पूर्णांक nonblock_reset = -EBADF;
        पूर्णांक r;

        निश्चित(fd >= 0);

        अगर (getenv_terminal_is_dumb())
                वापस 0;

        r = fd_nonblock(fd, true);
        अगर (r < 0)
                वापस log_debug_errno(r, "Failed to set terminal to non-blocking mode: %m");
        अगर (r > 0)
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
        अगर (r < 0)
                log_debug_errno(r, "Failed to reset terminal through ANSI sequences: %m");

        वापस r;
}

व्योम reset_dev_console_fd(पूर्णांक fd, bool switch_to_text) {
        पूर्णांक r;

        निश्चित(fd >= 0);

        _cleanup_close_ पूर्णांक lock_fd = lock_dev_console();
        अगर (lock_fd < 0)
                log_debug_errno(lock_fd, "Failed to lock /dev/console, ignoring: %m");

        r = terminal_reset_ioctl(fd, switch_to_text);
        अगर (r < 0)
                log_warning_errno(r, "Failed to reset /dev/console, ignoring: %m");

        अचिन्हित rows, cols;
        r = proc_cmdline_tty_आकार("/dev/console", &rows, &cols);
        अगर (r < 0)
                log_warning_errno(r, "Failed to get /dev/console size, ignoring: %m");
        अन्यथा अगर (r > 0) {
                r = terminal_set_आकार_fd(fd, शून्य, rows, cols);
                अगर (r < 0)
                        log_warning_errno(r, "Failed to set configured terminal size on /dev/console, ignoring: %m");
        } अन्यथा
                (व्योम) terminal_fix_आकार(fd, fd);

        r = terminal_reset_ansi_seq(fd);
        अगर (r < 0)
                log_warning_errno(r, "Failed to reset /dev/console using ANSI sequences, ignoring: %m");
}

पूर्णांक lock_dev_console(व्योम) {
        _cleanup_close_ पूर्णांक fd = -EBADF;
        पूर्णांक r;

        /* NB: We do not use O_NOFOLLOW here, because some container managers might place a symlink to some
         * pty in /dev/console, in which case it should be fine to lock the target TTY. */
        fd = open_terminal("/dev/console", O_RDONLY|O_CLOEXEC|O_NOCTTY);
        अगर (fd < 0)
                वापस fd;

        r = lock_generic(fd, LOCK_BSD, LOCK_EX);
        अगर (r < 0)
                वापस r;

        वापस TAKE_FD(fd);
}

पूर्णांक make_console_stdio(bool switch_to_text) {
        पूर्णांक fd, r;

        /* Make /dev/console the controlling terminal and stdin/stdout/stderr, if we can. If we can't use
         * /dev/null instead. This is particularly useful if /dev/console is turned off, e.g. if console=null
         * is specified on the kernel command line. */

        fd = acquire_terminal("/dev/console", ACQUIRE_TERMINAL_FORCE|ACQUIRE_TERMINAL_PERMISSIVE, USEC_INFINITY);
        अगर (fd < 0) {
                log_warning_errno(fd, "Failed to acquire terminal, using /dev/null stdin/stdout/stderr instead: %m");

                r = make_null_stdio();
                अगर (r < 0)
                        वापस log_error_errno(r, "Failed to make /dev/null stdin/stdout/stderr: %m");

        } अन्यथा {
                reset_dev_console_fd(fd, switch_to_text);

                r = rearrange_stdio(fd, fd, fd); /* This invalidates 'fd' both on success and on failure. */
                अगर (r < 0)
                        वापस log_error_errno(r, "Failed to make terminal stdin/stdout/stderr: %m");
        }

        reset_terminal_feature_caches();
        वापस 0;
}

अटल पूर्णांक vtnr_from_tty_raw(स्थिर अक्षर *tty, अचिन्हित *ret) {
        निश्चित(tty);

        tty = skip_dev_prefix(tty);

        स्थिर अक्षर *e = startswith(tty, "tty");
        अगर (!e)
                वापस -EINVAL;

        वापस safe_atou(e, ret);
}

पूर्णांक vtnr_from_tty(स्थिर अक्षर *tty) {
        अचिन्हित u;
        पूर्णांक r;

        निश्चित(tty);

        r = vtnr_from_tty_raw(tty, &u);
        अगर (r < 0)
                वापस r;
        अगर (!vtnr_is_valid(u))
                वापस -दुस्फल;

        वापस (पूर्णांक) u;
}

bool tty_is_vc(स्थिर अक्षर *tty) {
        निश्चित(tty);

        /* NB: for >= 0 values no range check is conducted here, on the assumption that the caller will
         * either extract vtnr through vtnr_from_tty() later where ERANGE would be reported, or doesn't care
         * about whether it's strictly valid, but only asking "does this fall into the vt category?", for which
         * "yes" seems to be a better answer. */

        वापस vtnr_from_tty_raw(tty, /* ret= */ शून्य) >= 0;
}

bool tty_is_console(स्थिर अक्षर *tty) {
        निश्चित(tty);

        वापस streq(skip_dev_prefix(tty), "console");
}

पूर्णांक resolve_dev_console(अक्षर **ret) {
        पूर्णांक r;

        निश्चित(ret);

        /* Resolve where /dev/console is pointing to. If /dev/console is a symlink (like in container
         * managers), we'll just resolve the symlink. If it's a real device node, we'll use if
         * /sys/class/tty/tty0/active, but only if /sys/ is actually ours (i.e. not read-only-mounted which
         * is a sign for container setups). */

        _cleanup_free_ अक्षर *chased = शून्य;
        r = chase("/dev/console", /* root= */ शून्य, /* flags= */ 0, &chased, /* ret_fd= */ शून्य);
        अगर (r < 0)
                वापस r;
        अगर (!path_equal(chased, "/dev/console")) {
                *ret = TAKE_PTR(chased);
                वापस 0;
        }

        r = path_is_read_only_fs("/sys");
        अगर (r < 0)
                वापस r;
        अगर (r > 0)
                वापस -ENOMEDIUM;

        _cleanup_free_ अक्षर *active = शून्य;
        r = read_one_पंक्ति_file("/sys/class/tty/console/active", &active);
        अगर (r < 0)
                वापस r;
        अगर (r == 0)
                वापस -ENXIO;

        /* If multiple log outputs are configured the last one is what /dev/console points to */
        स्थिर अक्षर *tty = म_खोजप(active, ' ');
        अगर (tty)
                tty++;
        अन्यथा
                tty = active;

        अगर (streq(tty, "tty0")) {
                active = mfree(active);

                /* Get the active VC (e.g. tty1) */
                r = read_one_पंक्ति_file("/sys/class/tty/tty0/active", &active);
                अगर (r < 0)
                        वापस r;
                अगर (r == 0)
                        वापस -ENXIO;

                tty = active;
        }

        _cleanup_free_ अक्षर *path = शून्य;
        path = path_join("/dev", tty);
        अगर (!path)
                वापस -ENOMEM;

        *ret = TAKE_PTR(path);
        वापस 0;
}

पूर्णांक get_kernel_consoles(अक्षर ***ret) {
        _cleanup_strv_free_ अक्षर **l = शून्य;
        _cleanup_free_ अक्षर *पंक्ति = शून्य;
        पूर्णांक r;

        निश्चित(ret);

        /* If /sys/ is mounted read-only this means we are running in some kind of container environment.
         * In that case /sys/ would reflect the host system, not us, hence ignore the data we can read from it. */
        अगर (path_is_read_only_fs("/sys") > 0)
                जाओ fallback;

        r = read_one_पंक्ति_file("/sys/class/tty/console/active", &पंक्ति);
        अगर (r < 0)
                वापस r;

        क्रम (स्थिर अक्षर *p = पंक्ति;;) {
                _cleanup_free_ अक्षर *tty = शून्य, *path = शून्य;

                r = extract_first_word(&p, &tty, शून्य, 0);
                अगर (r < 0)
                        वापस r;
                अगर (r == 0)
                        अवरोध;

                अगर (streq(tty, "tty0")) {
                        tty = mfree(tty);
                        r = read_one_पंक्ति_file("/sys/class/tty/tty0/active", &tty);
                        अगर (r < 0)
                                वापस r;
                        अगर (r == 0) {
                                log_debug("No VT active, skipping /dev/tty0.");
                                जारी;
                        }
                }

                path = path_join("/dev", tty);
                अगर (!path)
                        वापस -ENOMEM;

                अगर (access(path, F_OK) < 0) {
                        log_debug_errno(त्रुटि_सं, "Console device %s is not accessible, skipping: %m", path);
                        जारी;
                }

                r = strv_consume(&l, TAKE_PTR(path));
                अगर (r < 0)
                        वापस r;
        }

        अगर (strv_isempty(l)) {
                log_debug("No devices found for system console");
                जाओ fallback;
        }

        *ret = TAKE_PTR(l);
        वापस strv_लंबाई(*ret);

fallback:
        r = strv_extend(&l, "/dev/console");
        अगर (r < 0)
                वापस r;

        *ret = TAKE_PTR(l);
        वापस 0;
}

bool tty_is_vc_resolve(स्थिर अक्षर *tty) {
        _cleanup_free_ अक्षर *resolved = शून्य;

        निश्चित(tty);

        अगर (streq(skip_dev_prefix(tty), "console")) {
                अगर (resolve_dev_console(&resolved) < 0)
                        वापस false;

                tty = resolved;
        }

        वापस tty_is_vc(tty);
}

पूर्णांक fd_columns(पूर्णांक fd) {
        काष्ठा winsize ws = {};

        अगर (fd < 0)
                वापस -EBADF;

        अगर (ioctl(fd, TIOCGWINSZ, &ws) < 0)
                वापस -त्रुटि_सं;

        अगर (ws.ws_col <= 0)
                वापस -ENODATA; /* some tty types come up with invalid row/column initially, return a recognizable error for that */

        वापस ws.ws_col;
}

पूर्णांक getenv_columns(व्योम) {
        पूर्णांक r;

        स्थिर अक्षर *e = दो_पर्या("COLUMNS");
        अगर (!e)
                वापस -ENXIO;

        अचिन्हित c;
        r = safe_atou_bounded(e, 1, अच_लघु_उच्च, &c);
        अगर (r < 0)
                वापस r;

        वापस (पूर्णांक) c;
}

अचिन्हित columns(व्योम) {

        अगर (cached_columns > 0)
                वापस cached_columns;

        पूर्णांक c = getenv_columns();
        अगर (c < 0) {
                c = fd_columns(STDOUT_FILENO);
                अगर (c < 0)
                        c = 80;
        }

        निश्चित(c > 0);

        cached_columns = c;
        वापस cached_columns;
}

पूर्णांक fd_पंक्तियाँ(पूर्णांक fd) {
        काष्ठा winsize ws = {};

        अगर (fd < 0)
                वापस -EBADF;

        अगर (ioctl(fd, TIOCGWINSZ, &ws) < 0)
                वापस -त्रुटि_सं;

        अगर (ws.ws_row <= 0)
                वापस -ENODATA; /* some tty types come up with invalid row/column initially, return a recognizable error for that */

        वापस ws.ws_row;
}

अचिन्हित पंक्तियाँ(व्योम) {
        स्थिर अक्षर *e;
        पूर्णांक l;

        अगर (cached_पंक्तियाँ > 0)
                वापस cached_पंक्तियाँ;

        l = 0;
        e = दो_पर्या("LINES");
        अगर (e)
                (व्योम) safe_atoi(e, &l);

        अगर (l <= 0 || l > अच_लघु_उच्च) {
                l = fd_पंक्तियाँ(STDOUT_FILENO);
                अगर (l <= 0)
                        l = 24;
        }

        cached_पंक्तियाँ = l;
        वापस cached_पंक्तियाँ;
}

पूर्णांक terminal_set_आकार_fd(पूर्णांक fd, स्थिर अक्षर *ident, अचिन्हित rows, अचिन्हित cols) {
        काष्ठा winsize ws;

        निश्चित(fd >= 0);

        अगर (!ident)
                ident = "TTY";

        अगर (rows == अच_पूर्णांक_उच्च && cols == अच_पूर्णांक_उच्च)
                वापस 0;

        अगर (ioctl(fd, TIOCGWINSZ, &ws) < 0)
                वापस log_debug_errno(त्रुटि_सं,
                                       "TIOCGWINSZ ioctl for getting %s size failed, not setting terminal size: %m",
                                       ident);

        अगर (rows == अच_पूर्णांक_उच्च)
                rows = ws.ws_row;
        अन्यथा अगर (rows > अच_लघु_उच्च)
                rows = अच_लघु_उच्च;

        अगर (cols == अच_पूर्णांक_उच्च)
                cols = ws.ws_col;
        अन्यथा अगर (cols > अच_लघु_उच्च)
                cols = अच_लघु_उच्च;

        अगर (rows == ws.ws_row && cols == ws.ws_col)
                वापस 0;

        ws.ws_row = rows;
        ws.ws_col = cols;

        अगर (ioctl(fd, TIOCSWINSZ, &ws) < 0)
                वापस log_debug_errno(त्रुटि_सं, "TIOCSWINSZ ioctl for setting %s size failed: %m", ident);

        वापस 0;
}

पूर्णांक proc_cmdline_tty_आकार(स्थिर अक्षर *tty, अचिन्हित *ret_rows, अचिन्हित *ret_cols) {
        _cleanup_free_ अक्षर *rowskey = शून्य, *rowsvalue = शून्य, *colskey = शून्य, *colsvalue = शून्य;
        अचिन्हित rows = अच_पूर्णांक_उच्च, cols = अच_पूर्णांक_उच्च;
        पूर्णांक r;

        निश्चित(tty);

        अगर (!ret_rows && !ret_cols)
                वापस 0;

        tty = skip_dev_prefix(tty);
        अगर (path_startswith(tty, "pts/"))
                वापस -EMEDIUMTYPE;
        अगर (!in_charset(tty, ALPHANUMERICAL))
                वापस log_debug_errno(SYNTHETIC_ERRNO(EINVAL),
                                       "TTY name '%s' contains non-alphanumeric characters, not searching kernel cmdline for size.", tty);

        rowskey = strjoin("systemd.tty.rows.", tty);
        अगर (!rowskey)
                वापस -ENOMEM;

        colskey = strjoin("systemd.tty.columns.", tty);
        अगर (!colskey)
                वापस -ENOMEM;

        r = proc_cmdline_get_कुंजी_many(/* flags= */ 0,
                                      rowskey, &rowsvalue,
                                      colskey, &colsvalue);
        अगर (r < 0)
                वापस log_debug_errno(r, "Failed to read TTY size of %s from kernel cmdline: %m", tty);

        अगर (rowsvalue) {
                r = safe_atou(rowsvalue, &rows);
                अगर (r < 0)
                        वापस log_debug_errno(r, "Failed to parse %s=%s: %m", rowskey, rowsvalue);
        }

        अगर (colsvalue) {
                r = safe_atou(colsvalue, &cols);
                अगर (r < 0)
                        वापस log_debug_errno(r, "Failed to parse %s=%s: %m", colskey, colsvalue);
        }

        अगर (ret_rows)
                *ret_rows = rows;
        अगर (ret_cols)
                *ret_cols = cols;

        वापस rows != अच_पूर्णांक_उच्च || cols != अच_पूर्णांक_उच्च;
}

/* intended to be used as a SIGWINCH sighandler */
व्योम columns_पंक्तियाँ_cache_reset(पूर्णांक signum) {
        cached_columns = 0;
        cached_पंक्तियाँ = 0;
}

व्योम reset_terminal_feature_caches(व्योम) {
        cached_columns = 0;
        cached_पंक्तियाँ = 0;

        cached_on_tty = -1;
        cached_on_dev_null = -1;

        reset_ansi_feature_caches();
}

bool on_tty(व्योम) {

        /* We check both stdout and stderr, so that situations where pipes on the shell are used are reliably
         * recognized, regardless if only the output or the errors are piped to some place. Since on_tty() is generally
         * used to default to a safer, non-interactive, non-color mode of operation it's probably good to be defensive
         * here, and check for both. Note that we don't check for STDIN_FILENO, because it should fine to use fancy
         * terminal functionality when outputting stuff, even if the input is piped to us. */

        अगर (cached_on_tty < 0)
                cached_on_tty =
                        isatty_safe(STDOUT_FILENO) &&
                        isatty_safe(STDERR_FILENO);

        वापस cached_on_tty;
}

पूर्णांक getttyname_malloc(पूर्णांक fd, अक्षर **ret) {
        अक्षर path[PATH_MAX]; /* PATH_MAX is counted *with* the trailing NUL byte */
        पूर्णांक r;

        निश्चित(fd >= 0);
        निश्चित(ret);

        r = ttyname_r(fd, path, माप path); /* positive error */
        निश्चित(r >= 0);
        अगर (r == दुस्फल)
                वापस -ENAMETOOLONG;
        अगर (r > 0)
                वापस -r;

        वापस strdup_to(ret, skip_dev_prefix(path));
}

पूर्णांक getttyname_harder(पूर्णांक fd, अक्षर **ret) {
        _cleanup_free_ अक्षर *s = शून्य;
        पूर्णांक r;

        निश्चित(ret);

        r = getttyname_malloc(fd, &s);
        अगर (r < 0)
                वापस r;

        अगर (streq(s, "tty"))
                वापस get_ctty(0, शून्य, ret);

        *ret = TAKE_PTR(s);
        वापस 0;
}

पूर्णांक get_ctty_devnr(pid_t pid, dev_t *ret) {
        _cleanup_free_ अक्षर *पंक्ति = शून्य;
        अचिन्हित दीर्घ ttynr;
        स्थिर अक्षर *p;
        पूर्णांक r;

        निश्चित(pid >= 0);

        p = procfs_file_alloca(pid, "stat");
        r = read_one_पंक्ति_file(p, &पंक्ति);
        अगर (r < 0)
                वापस r;

        p = म_खोजप(पंक्ति, ')');
        अगर (!p)
                वापस -EIO;

        p++;

        अगर (माला_पूछो(p, " "
                   "%*c "  /* state */
                   "%*d "  /* ppid */
                   "%*d "  /* pgrp */
                   "%*d "  /* session */
                   "%lu ", /* ttynr */
                   &ttynr) != 1)
                वापस -EIO;

        अगर (devnum_is_zero(ttynr))
                वापस -ENXIO;

        अगर (ret)
                *ret = (dev_t) ttynr;

        वापस 0;
}

पूर्णांक get_ctty(pid_t pid, dev_t *ret_devnr, अक्षर **ret) {
        अक्षर pty[STRLEN("/dev/pts/") + DECIMAL_STR_MAX(dev_t) + 1];
        _cleanup_free_ अक्षर *buf = शून्य;
        स्थिर अक्षर *fn = शून्य, *w;
        dev_t devnr;
        पूर्णांक r;

        r = get_ctty_devnr(pid, &devnr);
        अगर (r < 0)
                वापस r;

        r = device_path_make_canonical(S_IFCHR, devnr, &buf);
        अगर (r < 0) {
                काष्ठा stat st;

                अगर (r != -ENOENT) /* No symlink for this in /dev/char/? */
                        वापस r;

                /* Maybe this is PTY? PTY devices are not listed in /dev/char/, as they don't follow the
                 * Linux device model and hence device_path_make_canonical() doesn't work for them. Let's
                 * assume this is a PTY for a moment, and check if the device node this would then map to in
                 * /dev/pts/ matches the one we are looking for. This way we don't have to hardcode the major
                 * number (which is 136 btw), but we still rely on the fact that PTY numbers map directly to
                 * the minor number of the pty. */
                xsprintf(pty, "/dev/pts/%u", minor(devnr));

                अगर (stat(pty, &st) < 0) {
                        अगर (त्रुटि_सं != ENOENT)
                                वापस -त्रुटि_सं;

                } अन्यथा अगर (S_ISCHR(st.st_mode) && devnr == st.st_rdev) /* Bingo! */
                        fn = pty;

                अगर (!fn) {
                        /* Doesn't exist, or not a PTY? Probably something similar to the PTYs which have no
                         * symlink in /dev/char/. Let's return something vaguely useful. */
                        r = device_path_make_major_minor(S_IFCHR, devnr, &buf);
                        अगर (r < 0)
                                वापस r;

                        fn = buf;
                }
        } अन्यथा
                fn = buf;

        w = path_startswith(fn, "/dev/");
        अगर (!w)
                वापस -EINVAL;

        अगर (ret) {
                r = strdup_to(ret, w);
                अगर (r < 0)
                        वापस r;
        }

        अगर (ret_devnr)
                *ret_devnr = devnr;

        वापस 0;
}

पूर्णांक ptsname_malloc(पूर्णांक fd, अक्षर **ret) {
        निश्चित(fd >= 0);
        निश्चित(ret);

        क्रम (माप_प्रकार l = 50;;) {
                _cleanup_free_ अक्षर *c = शून्य;

                c = new(अक्षर, l);
                अगर (!c)
                        वापस -ENOMEM;

                अगर (ptsname_r(fd, c, l) >= 0) {
                        *ret = TAKE_PTR(c);
                        वापस 0;
                }
                अगर (त्रुटि_सं != दुस्फल)
                        वापस -त्रुटि_सं;

                अगर (!MUL_ASSIGN_SAFE(&l, 2))
                        वापस -ENOMEM;
        }
}

पूर्णांक openpt_allocate(पूर्णांक flags, अक्षर **ret_peer_path) {
        _cleanup_close_ पूर्णांक fd = -EBADF;
        पूर्णांक r;

        fd = posix_openpt(flags|O_NOCTTY|O_CLOEXEC);
        अगर (fd < 0)
                वापस -त्रुटि_सं;

        _cleanup_free_ अक्षर *p = शून्य;
        अगर (ret_peer_path) {
                r = ptsname_malloc(fd, &p);
                अगर (r < 0)
                        वापस r;

                अगर (!path_startswith(p, "/dev/pts/"))
                        वापस -EINVAL;
        }

        अगर (unlockpt(fd) < 0)
                वापस -त्रुटि_सं;

        अगर (ret_peer_path)
                *ret_peer_path = TAKE_PTR(p);

        वापस TAKE_FD(fd);
}

अटल पूर्णांक ptsname_namespace(पूर्णांक pty, अक्षर **ret) {
        पूर्णांक no = -1;

        निश्चित(pty >= 0);
        निश्चित(ret);

        /* Like ptsname(), but doesn't assume that the path is
         * accessible in the local namespace. */

        अगर (ioctl(pty, TIOCGPTN, &no) < 0)
                वापस -त्रुटि_सं;

        अगर (no < 0)
                वापस -EIO;

        अगर (asprintf(ret, "/dev/pts/%i", no) < 0)
                वापस -ENOMEM;

        वापस 0;
}

पूर्णांक openpt_allocate_in_namespace(
                स्थिर PidRef *pidref,
                पूर्णांक flags,
                अक्षर **ret_peer_path) {

        _cleanup_close_ पूर्णांक pidnsfd = -EBADF, mntnsfd = -EBADF, usernsfd = -EBADF, rootfd = -EBADF, fd = -EBADF;
        _cleanup_close_pair_ पूर्णांक pair[2] = EBADF_PAIR;
        पूर्णांक r;

        निश्चित(pidref);

        r = pidref_namespace_open(pidref, &pidnsfd, &mntnsfd, /* ret_netns_fd= */ शून्य, &usernsfd, &rootfd);
        अगर (r < 0)
                वापस log_debug_errno(r, "Failed to open namespaces of PID "PID_FMT": %m", pidref->pid);

        अगर (socketpair(AF_UNIX, SOCK_DGRAM|SOCK_CLOEXEC, 0, pair) < 0)
                वापस -त्रुटि_सं;

        r = namespace_fork(
                        "(sd-openptns)",
                        "(sd-openpt)",
                        FORK_RESET_SIGNALS|FORK_DEATHSIG_SIGKILL|FORK_WAIT,
                        pidnsfd,
                        mntnsfd,
                        /* netns_fd= */ -EBADF,
                        usernsfd,
                        rootfd,
                        /* ret= */ शून्य);
        अगर (r < 0)
                वापस r;
        अगर (r == 0) {
                pair[0] = safe_close(pair[0]);

                fd = openpt_allocate(flags, /* ret_peer_path= */ शून्य);
                अगर (fd < 0)
                        _निकास(निकास_त्रुटि);

                अगर (send_one_fd(pair[1], fd, 0) < 0)
                        _निकास(निकास_त्रुटि);

                _निकास(निकास_सफल);
        }

        pair[1] = safe_close(pair[1]);

        fd = receive_one_fd(pair[0], 0);
        अगर (fd < 0)
                वापस fd;

        अगर (ret_peer_path) {
                r = ptsname_namespace(fd, ret_peer_path);
                अगर (r < 0)
                        वापस r;
        }

        वापस TAKE_FD(fd);
}

अटल bool on_dev_null(व्योम) {
        काष्ठा stat dst, ost, est;

        अगर (cached_on_dev_null >= 0)
                वापस cached_on_dev_null;

        अगर (stat("/dev/null", &dst) < 0 || ख_स्थिति(STDOUT_FILENO, &ost) < 0 || ख_स्थिति(STDERR_FILENO, &est) < 0)
                cached_on_dev_null = false;
        अन्यथा
                cached_on_dev_null = stat_inode_same(&dst, &ost) && stat_inode_same(&dst, &est);

        वापस cached_on_dev_null;
}

bool term_env_valid(स्थिर अक्षर *term) {
        /* Checks if the specified $TERM value is suitable for propagation, i.e. is not empty, not set to
         * "unknown" (as is common in CI), and only contains characters valid in terminal type names.
         * Valid $TERM values are things like "xterm-256color", "linux", "screen.xterm-256color", i.e.
         * alphanumeric characters, hyphens, underscores, dots, and plus signs. */
        वापस !isempty(term) &&
                !streq(term, "unknown") &&
                in_charset(term, ALPHANUMERICAL "-_+.");
}

bool getenv_terminal_is_dumb(व्योम) {
        स्थिर अक्षर *e;

        e = दो_पर्या("TERM");
        अगर (!e)
                वापस true;

        वापस streq(e, "dumb");
}

bool terminal_is_dumb(व्योम) {
        अगर (!on_tty() && !on_dev_null())
                वापस true;

        वापस getenv_terminal_is_dumb();
}

bool dev_console_colors_enabled(व्योम) {
        _cleanup_free_ अक्षर *s = शून्य;
        ColorMode m;

        /* Returns true if we assume that color is supported on /dev/console.
         *
         * For that we first check if we explicitly got told to use colors or not, by checking $SYSTEMD_COLORS. If that
         * isn't set we check whether PID 1 has $TERM set, and if not, whether TERM is set on the kernel command
         * line. If we find $TERM set we assume color if it's not set to "dumb", similarly to how regular
         * colors_enabled() operates. */

        m = parse_systemd_colors();
        अगर (m >= 0)
                वापस m;

        अगर (दो_पर्या("NO_COLOR"))
                वापस false;

        अगर (getenv_for_pid(1, "TERM", &s) <= 0)
                (व्योम) proc_cmdline_get_कुंजी("TERM", 0, &s);

        वापस !streq_ptr(s, "dumb");
}

पूर्णांक vt_restore(पूर्णांक fd) {

        अटल स्थिर काष्ठा vt_mode mode = {
                .mode = VT_AUTO,
        };

        पूर्णांक r, ret = 0;

        निश्चित(fd >= 0);

        अगर (!isatty_safe(fd))
                वापस log_debug_errno(SYNTHETIC_ERRNO(ENOTTY), "Asked to restore the VT for an fd that does not refer to a terminal.");

        अगर (ioctl(fd, KDSETMODE, KD_TEXT) < 0)
                RET_GATHER(ret, log_debug_errno(त्रुटि_सं, "Failed to set VT to text mode, ignoring: %m"));

        r = vt_reset_keyboard(fd);
        अगर (r < 0)
                RET_GATHER(ret, log_debug_errno(r, "Failed to reset keyboard mode, ignoring: %m"));

        अगर (ioctl(fd, VT_SETMODE, &mode) < 0)
                RET_GATHER(ret, log_debug_errno(त्रुटि_सं, "Failed to set VT_AUTO mode, ignoring: %m"));

        r = fchmod_and_chown(fd, TTY_MODE, 0, GID_INVALID);
        अगर (r < 0)
                RET_GATHER(ret, log_debug_errno(r, "Failed to chmod()/chown() VT, ignoring: %m"));

        वापस ret;
}

पूर्णांक vt_release(पूर्णांक fd, bool restore) {
        निश्चित(fd >= 0);

        /* This function releases the VT by acknowledging the VT-switch signal
         * sent by the kernel and optionally reset the VT in text and auto
         * VT-switching modes. */

        अगर (!isatty_safe(fd))
                वापस log_debug_errno(SYNTHETIC_ERRNO(ENOTTY), "Asked to release the VT for an fd that does not refer to a terminal.");

        अगर (ioctl(fd, VT_RELDISP, 1) < 0)
                वापस -त्रुटि_सं;

        अगर (restore)
                वापस vt_restore(fd);

        वापस 0;
}

व्योम get_log_colors(पूर्णांक priority, स्थिर अक्षर **on, स्थिर अक्षर **off, स्थिर अक्षर **highlight) {
        /* Note that this will initialize output variables only when there's something to output.
         * The caller must pre-initialize to "" or NULL as appropriate. */

        अगर (priority <= LOG_ERR) {
                अगर (on)
                        *on = ansi_highlight_red();
                अगर (off)
                        *off = ansi_normal();
                अगर (highlight)
                        *highlight = ansi_highlight();

        } अन्यथा अगर (priority <= LOG_WARNING) {
                अगर (on)
                        *on = ansi_highlight_yellow();
                अगर (off)
                        *off = ansi_normal();
                अगर (highlight)
                        *highlight = ansi_highlight();

        } अन्यथा अगर (priority <= LOG_NOTICE) {
                अगर (on)
                        *on = ansi_highlight();
                अगर (off)
                        *off = ansi_normal();
                अगर (highlight)
                        *highlight = ansi_highlight_red();

        } अन्यथा अगर (priority >= LOG_DEBUG) {
                अगर (on)
                        *on = ansi_grey();
                अगर (off)
                        *off = ansi_normal();
                अगर (highlight)
                        *highlight = ansi_highlight_red();
        }
}

पूर्णांक terminal_set_cursor_position(पूर्णांक fd, अचिन्हित row, अचिन्हित column) {
        निश्चित(fd >= 0);

        अक्षर cursor_position[STRLEN("\x1B[" ";" "H") + DECIMAL_STR_MAX(अचिन्हित) * 2 + 1];
        xsprintf(cursor_position, "\x1B[%u;%uH", row, column);

        वापस loop_write(fd, cursor_position, SIZE_MAX);
}

अटल पूर्णांक terminal_verify_same(पूर्णांक निवेश_fd, पूर्णांक output_fd) {
        पूर्णांक r;

        निश्चित(निवेश_fd >= 0);
        निश्चित(output_fd >= 0);

        /* Validates that the specified fds reference the same TTY */

        अगर (निवेश_fd != output_fd) {
                काष्ठा stat sti;
                अगर (ख_स्थिति(निवेश_fd, &sti) < 0)
                        वापस -त्रुटि_सं;

                r = stat_verify_char(&sti); /* TTYs are character devices */
                अगर (r < 0)
                        वापस r;

                काष्ठा stat sto;
                अगर (ख_स्थिति(output_fd, &sto) < 0)
                        वापस -त्रुटि_सं;

                r = stat_verify_char(&sto);
                अगर (r < 0)
                        वापस r;

                अगर (sti.st_rdev != sto.st_rdev)
                        वापस -ENOLINK;
        }

        अगर (!isatty_safe(निवेश_fd)) /* The check above was just for char device, but now let's ensure it's actually a tty */
                वापस -ENOTTY;

        वापस 0;
}

प्रकार क्रमागत CursorPositionState {
        CURSOR_TEXT,
        CURSOR_ESCAPE,
        CURSOR_ROW,
        CURSOR_COLUMN,
} CursorPositionState;

प्रकार काष्ठा CursorPositionContext {
        CursorPositionState state;
        अचिन्हित row, column;
} CursorPositionContext;

अटल पूर्णांक scan_cursor_position_response(
                CursorPositionContext *context,
                स्थिर अक्षर *buf,
                माप_प्रकार आकार,
                माप_प्रकार *ret_processed) {

        निश्चित(context);
        निश्चित(buf);
        निश्चित(ret_processed);

        क्रम (माप_प्रकार i = 0; i < आकार; i++) {
                अक्षर c = buf[i];

                चयन (context->state) {

                हाल CURSOR_TEXT:
                        context->state = c == '\x1B' ? CURSOR_ESCAPE : CURSOR_TEXT;
                        अवरोध;

                हाल CURSOR_ESCAPE:
                        context->state = c == '[' ? CURSOR_ROW : CURSOR_TEXT;
                        अवरोध;

                हाल CURSOR_ROW:
                        अगर (c == ';')
                                context->state = context->row > 0 ? CURSOR_COLUMN : CURSOR_TEXT;
                        अन्यथा {
                                पूर्णांक d = undecchar(c);

                                /* We read a decimal character, let's suffix it to the number we so far read,
                                 * but let's do an overflow check first. */
                                अगर (d < 0 || context->row > (अच_पूर्णांक_उच्च-d)/10)
                                        context->state = CURSOR_TEXT;
                                अन्यथा
                                        context->row = context->row * 10 + d;
                        }
                        अवरोध;

                हाल CURSOR_COLUMN:
                        अगर (c == 'R') {
                                अगर (context->column > 0) {
                                        *ret_processed = i + 1;
                                        वापस 1; /* success! */
                                }

                                context->state = CURSOR_TEXT;
                        } अन्यथा {
                                पूर्णांक d = undecchar(c);

                                /* As above, add the decimal character to our column number */
                                अगर (d < 0 || context->column > (अच_पूर्णांक_उच्च-d)/10)
                                        context->state = CURSOR_TEXT;
                                अन्यथा
                                        context->column = context->column * 10 + d;
                        }

                        अवरोध;
                }

                /* Reset any positions we might have picked up */
                अगर (IN_SET(context->state, CURSOR_TEXT, CURSOR_ESCAPE))
                        context->row = context->column = 0;
        }

        *ret_processed = आकार;
        वापस 0; /* all good, but not enough data yet */
}

पूर्णांक terminal_get_cursor_position(
                पूर्णांक निवेश_fd,
                पूर्णांक output_fd,
                अचिन्हित *ret_row,
                अचिन्हित *ret_column) {

        _cleanup_close_ पूर्णांक nonblock_निवेश_fd = -EBADF;
        पूर्णांक r;

        निश्चित(निवेश_fd >= 0);
        निश्चित(output_fd >= 0);

        अगर (getenv_terminal_is_dumb())
                वापस -EOPNOTSUPP;

        r = terminal_verify_same(निवेश_fd, output_fd);
        अगर (r < 0)
                वापस log_debug_errno(r, "Called with distinct input/output fds: %m");

        /* Failure to reset the terminal is ignored here and in similar cases below.
         * We already have our result; if cleanup fails it doesn't change the validity of the result. */
        काष्ठा termios old_termios = TERMIOS_NULL;
        CLEANUP_TERMIOS_RESET(निवेश_fd, old_termios);

        अगर (tcgetattr(निवेश_fd, &old_termios) < 0)
                वापस log_debug_errno(त्रुटि_सं, "Failed to get terminal settings: %m");

        काष्ठा termios new_termios = old_termios;
        termios_disable_echo(&new_termios);

        अगर (tcsetattr(निवेश_fd, TCSANOW, &new_termios) < 0)
                वापस log_debug_errno(त्रुटि_सं, "Failed to set new terminal settings: %m");

        /* Request cursor position (DSR/CPR) */
        r = loop_write(output_fd, "\x1B[6n", SIZE_MAX);
        अगर (r < 0)
                वापस r;

        /* Open a 2nd input fd, in non-blocking mode, so that we won't ever hang in read() should someone
         * else process the POLLIN. */

        nonblock_निवेश_fd = r = fd_reopen(निवेश_fd, O_RDONLY|O_CLOEXEC|O_NONBLOCK|O_NOCTTY);
        अगर (r < 0)
                वापस r;

        usec_t end = usec_add(now(CLOCK_MONOTONIC), CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        अक्षर buf[STRLEN("\x1B[1;1R")]; /* The shortest valid reply possible */
        माप_प्रकार buf_full = 0;
        CursorPositionContext context = {};

        क्रम (bool first = true;; first = false) {
                अगर (buf_full == 0) {
                        usec_t n = now(CLOCK_MONOTONIC);
                        अगर (n >= end)
                                वापस -EOPNOTSUPP;

                        r = fd_wait_for_event(nonblock_निवेश_fd, POLLIN, usec_sub_unsigned(end, n));
                        अगर (r < 0)
                                वापस r;
                        अगर (r == 0)
                                वापस -EOPNOTSUPP;

                        /* On the first try, read multiple characters, i.e. the shortest valid
                         * reply. Afterwards read byte-wise, since we don't want to read too much, and
                         * unnecessarily drop too many characters from the input queue. */
                        ssize_t l = पढ़ो(nonblock_निवेश_fd, buf, first ? माप(buf) : 1);
                        अगर (l < 0) {
                                अगर (त्रुटि_सं == EAGAIN)
                                        जारी;

                                वापस -त्रुटि_सं;
                        }

                        निश्चित((माप_प्रकार) l <= माप(buf));
                        buf_full = l;
                }

                माप_प्रकार processed;
                r = scan_cursor_position_response(&context, buf, buf_full, &processed);
                अगर (r < 0)
                        वापस r;

                निश्चित(processed <= buf_full);
                buf_full -= processed;
                स_हटाओ(buf, buf + processed, buf_full);

                अगर (r > 0) {
                        /* Superficial validity check */
                        अगर (context.row >= 32766 || context.column >= 32766)
                                वापस -ENODATA;

                        अगर (ret_row)
                                *ret_row = context.row;
                        अगर (ret_column)
                                *ret_column = context.column;

                        वापस 0;
                }
        }
}

पूर्णांक terminal_reset_defensive(पूर्णांक fd, TerminalResetFlags flags) {
        पूर्णांक r = 0;

        निश्चित(fd >= 0);
        निश्चित(!FLAGS_SET(flags, TERMINAL_RESET_AVOID_ANSI_SEQ|TERMINAL_RESET_FORCE_ANSI_SEQ));

        /* Resets the terminal comprehensively, i.e. via both ioctl()s and via ANSI sequences, but do so only
         * if $TERM is unset or set to "dumb" */

        अगर (!isatty_safe(fd))
                वापस -ENOTTY;

        RET_GATHER(r, terminal_reset_ioctl(fd, FLAGS_SET(flags, TERMINAL_RESET_SWITCH_TO_TEXT)));

        अगर (!FLAGS_SET(flags, TERMINAL_RESET_AVOID_ANSI_SEQ) &&
            (FLAGS_SET(flags, TERMINAL_RESET_FORCE_ANSI_SEQ) || !getenv_terminal_is_dumb()))
                RET_GATHER(r, terminal_reset_ansi_seq(fd));

        वापस r;
}

पूर्णांक terminal_reset_defensive_locked(पूर्णांक fd, TerminalResetFlags flags) {
        निश्चित(fd >= 0);

        _cleanup_close_ पूर्णांक lock_fd = lock_dev_console();
        अगर (lock_fd < 0)
                log_debug_errno(lock_fd, "Failed to acquire lock for /dev/console, ignoring: %m");

        वापस terminal_reset_defensive(fd, flags);
}

व्योम termios_disable_echo(काष्ठा termios *termios) {
        निश्चित(termios);

        termios->c_lflag &= ~(ICANON|ECHO);
        termios->c_cc[VMIN] = 1;
        termios->c_cc[VTIME] = 0;
}

अटल bool termios_is_null(स्थिर काष्ठा termios *t) {
        अगर (!t)
                वापस true;

        वापस t->c_iflag == अच_पूर्णांक_उच्च &&
               t->c_oflag == अच_पूर्णांक_उच्च &&
               t->c_cflag == अच_पूर्णांक_उच्च &&
               t->c_lflag == अच_पूर्णांक_उच्च;
}

व्योम termios_reset(स्थिर TermiosResetContext *c) {
        निश्चित(c);

        PROTECT_ERRNO;

        अगर (c->fd && *c->fd >= 0 && !termios_is_null(c->termios))
                (व्योम) tcsetattr(*c->fd, TCSANOW, c->termios);
}

प्रकार क्रमागत BackgroundColorState {
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

प्रकार काष्ठा BackgroundColorContext {
        BackgroundColorState state;
        uint32_t red, green, blue;
        अचिन्हित red_bits, green_bits, blue_bits;
} BackgroundColorContext;

अटल पूर्णांक scan_background_color_response(
                BackgroundColorContext *context,
                स्थिर अक्षर *buf,
                माप_प्रकार आकार,
                माप_प्रकार *ret_processed) {

        निश्चित(context);
        निश्चित(buf);
        निश्चित(ret_processed);

        क्रम (माप_प्रकार i = 0; i < आकार; i++) {
                अक्षर c = buf[i];

                चयन (context->state) {

                हाल BACKGROUND_TEXT:
                        context->state = c == '\x1B' ? BACKGROUND_ESCAPE : BACKGROUND_TEXT;
                        अवरोध;

                हाल BACKGROUND_ESCAPE:
                        context->state = c == ']' ? BACKGROUND_BRACKET : BACKGROUND_TEXT;
                        अवरोध;

                हाल BACKGROUND_BRACKET:
                        context->state = c == '1' ? BACKGROUND_FIRST_ONE : BACKGROUND_TEXT;
                        अवरोध;

                हाल BACKGROUND_FIRST_ONE:
                        context->state = c == '1' ? BACKGROUND_SECOND_ONE : BACKGROUND_TEXT;
                        अवरोध;

                हाल BACKGROUND_SECOND_ONE:
                        context->state = c == ';' ? BACKGROUND_SEMICOLON : BACKGROUND_TEXT;
                        अवरोध;

                हाल BACKGROUND_SEMICOLON:
                        context->state = c == 'r' ? BACKGROUND_R : BACKGROUND_TEXT;
                        अवरोध;

                हाल BACKGROUND_R:
                        context->state = c == 'g' ? BACKGROUND_G : BACKGROUND_TEXT;
                        अवरोध;

                हाल BACKGROUND_G:
                        context->state = c == 'b' ? BACKGROUND_B : BACKGROUND_TEXT;
                        अवरोध;

                हाल BACKGROUND_B:
                        context->state = c == ':' ? BACKGROUND_RED : BACKGROUND_TEXT;
                        अवरोध;

                हाल BACKGROUND_RED:
                        अगर (c == '/')
                                context->state = context->red_bits > 0 ? BACKGROUND_GREEN : BACKGROUND_TEXT;
                        अन्यथा {
                                पूर्णांक d = unhexchar(c);
                                अगर (d < 0 || context->red_bits >= माप(context->red)*8)
                                        context->state = BACKGROUND_TEXT;
                                अन्यथा {
                                        context->red = (context->red << 4) | d;
                                        context->red_bits += 4;
                                }
                        }
                        अवरोध;

                हाल BACKGROUND_GREEN:
                        अगर (c == '/')
                                context->state = context->green_bits > 0 ? BACKGROUND_BLUE : BACKGROUND_TEXT;
                        अन्यथा {
                                पूर्णांक d = unhexchar(c);
                                अगर (d < 0 || context->green_bits >= माप(context->green)*8)
                                        context->state = BACKGROUND_TEXT;
                                अन्यथा {
                                        context->green = (context->green << 4) | d;
                                        context->green_bits += 4;
                                }
                        }
                        अवरोध;

                हाल BACKGROUND_BLUE:
                        अगर (c == '\x07') {
                                अगर (context->blue_bits > 0) {
                                        *ret_processed = i + 1;
                                        वापस 1; /* success! */
                                }

                                context->state = BACKGROUND_TEXT;
                        } अन्यथा अगर (c == '\x1b')
                                context->state = context->blue_bits > 0 ? BACKGROUND_STRING_TERMINATOR : BACKGROUND_TEXT;
                        अन्यथा {
                                पूर्णांक d = unhexchar(c);
                                अगर (d < 0 || context->blue_bits >= माप(context->blue)*8)
                                        context->state = BACKGROUND_TEXT;
                                अन्यथा {
                                        context->blue = (context->blue << 4) | d;
                                        context->blue_bits += 4;
                                }
                        }
                        अवरोध;

                हाल BACKGROUND_STRING_TERMINATOR:
                        अगर (c == '\\') {
                                *ret_processed = i + 1;
                                वापस 1; /* success! */
                        }

                        context->state = c == ']' ? BACKGROUND_ESCAPE : BACKGROUND_TEXT;
                        अवरोध;

                }

                /* Reset any colors we might have picked up */
                अगर (IN_SET(context->state, BACKGROUND_TEXT, BACKGROUND_ESCAPE)) {
                        /* reset color */
                        context->red = context->green = context->blue = 0;
                        context->red_bits = context->green_bits = context->blue_bits = 0;
                }
        }

        *ret_processed = आकार;
        वापस 0; /* all good, but not enough data yet */
}

पूर्णांक get_default_background_color(द्विगुन *ret_red, द्विगुन *ret_green, द्विगुन *ret_blue) {
        पूर्णांक r;

        निश्चित(ret_red);
        निश्चित(ret_green);
        निश्चित(ret_blue);

        अगर (!colors_enabled())
                वापस -EOPNOTSUPP;

        r = terminal_verify_same(STDIN_FILENO, STDOUT_FILENO);
        अगर (r < 0)
                वापस r;

        अगर (streq_ptr(दो_पर्या("TERM"), "linux")) {
                /* Linux console is black */
                *ret_red = *ret_green = *ret_blue = 0.0;
                वापस 0;
        }

        /* Open a 2nd input fd, in non-blocking mode, so that we won't ever hang in read()
         * should someone else process the POLLIN. Do all subsequent operations on the new fd. */
        _cleanup_close_ पूर्णांक nonblock_निवेश_fd = r = fd_reopen(STDIN_FILENO, O_RDONLY|O_CLOEXEC|O_NONBLOCK|O_NOCTTY);
        अगर (r < 0)
                वापस r;

        काष्ठा termios old_termios = TERMIOS_NULL;
        CLEANUP_TERMIOS_RESET(nonblock_निवेश_fd, old_termios);

        अगर (tcgetattr(nonblock_निवेश_fd, &old_termios) < 0)
                वापस -त्रुटि_सं;

        काष्ठा termios new_termios = old_termios;
        termios_disable_echo(&new_termios);

        अगर (tcsetattr(nonblock_निवेश_fd, TCSANOW, &new_termios) < 0)
                वापस -त्रुटि_सं;

        r = loop_write(STDOUT_FILENO, ANSI_OSC "11;?" ANSI_ST, SIZE_MAX);
        अगर (r < 0)
                वापस r;

        usec_t end = usec_add(now(CLOCK_MONOTONIC), CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        अक्षर buf[STRLEN(ANSI_OSC "11;rgb:0/0/0" ANSI_ST)]; /* shortest possible reply */
        माप_प्रकार buf_full = 0;
        BackgroundColorContext context = {};

        क्रम (bool first = true;; first = false) {
                अगर (buf_full == 0) {
                        usec_t n = now(CLOCK_MONOTONIC);
                        अगर (n >= end)
                                वापस -EOPNOTSUPP;

                        r = fd_wait_for_event(nonblock_निवेश_fd, POLLIN, usec_sub_unsigned(end, n));
                        अगर (r < 0)
                                वापस r;
                        अगर (r == 0)
                                वापस -EOPNOTSUPP;

                        /* On the first try, read multiple characters, i.e. the shortest valid
                         * reply. Afterwards read byte-wise, since we don't want to read too much, and
                         * unnecessarily drop too many characters from the input queue. */
                        ssize_t l = पढ़ो(nonblock_निवेश_fd, buf, first ? माप(buf) : 1);
                        अगर (l < 0) {
                                अगर (त्रुटि_सं == EAGAIN)
                                        जारी;
                                वापस -त्रुटि_सं;
                        }

                        निश्चित((माप_प्रकार) l <= माप(buf));
                        buf_full = l;
                }

                माप_प्रकार processed;
                r = scan_background_color_response(&context, buf, buf_full, &processed);
                अगर (r < 0)
                        वापस r;

                निश्चित(processed <= buf_full);
                buf_full -= processed;
                स_हटाओ(buf, buf + processed, buf_full);

                अगर (r > 0) {
                        निश्चित(context.red_bits > 0);
                        *ret_red = (द्विगुन) context.red / ((UINT64_C(1) << context.red_bits) - 1);
                        निश्चित(context.green_bits > 0);
                        *ret_green = (द्विगुन) context.green / ((UINT64_C(1) << context.green_bits) - 1);
                        निश्चित(context.blue_bits > 0);
                        *ret_blue = (द्विगुन) context.blue / ((UINT64_C(1) << context.blue_bits) - 1);
                        वापस 0;
                }
        }
}

/* Determine terminal dimensions by means of ANSI sequences: save the cursor via DECSC, position it far
 * to the bottom right (clamped to actual terminal dimensions), read back via DSR where we ended up, and
 * restore cursor via DECRC. Only needs a single DSR round-trip, and always restores the cursor regardless
 * of whether the response is received.
 *
 * Caller must have already opened a non-blocking input fd and configured termios (echo/icanon off). */
अटल पूर्णांक terminal_query_आकार_by_dsr(
                पूर्णांक nonblock_निवेश_fd,
                पूर्णांक output_fd,
                अचिन्हित *ret_rows,
                अचिन्हित *ret_columns) {

        पूर्णांक r;

        निश्चित(nonblock_निवेश_fd >= 0);
        निश्चित(output_fd >= 0);

        /* Use DECSC/DECRC to save/restore cursor instead of querying position via DSR. This way the cursor
         * is always restored — even on timeout — and we only need one DSR response instead of two. */
        r = loop_write_full(output_fd,
                            "\x1B" "7"              /* DECSC: save cursor position */
                            "\x1B[32766;32766H"     /* CUP: position cursor far to the right and to the bottom, staying within 16bit signed range */
                            "\x1B[6n"               /* DSR: request cursor position (CPR) */
                            "\x1B" "8",             /* DECRC: restore cursor position */
                            SIZE_MAX,
                            CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        अगर (r < 0)
                वापस r;

        usec_t end = usec_add(now(CLOCK_MONOTONIC), CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        अक्षर buf[STRLEN("\x1B[1;1R")]; /* The shortest valid reply possible */
        माप_प्रकार buf_full = 0;
        CursorPositionContext context = {};

        क्रम (bool first = true;; first = false) {
                अगर (buf_full == 0) {
                        usec_t n = now(CLOCK_MONOTONIC);
                        अगर (n >= end)
                                वापस -EOPNOTSUPP;

                        r = fd_wait_for_event(nonblock_निवेश_fd, POLLIN, usec_sub_unsigned(end, n));
                        अगर (r < 0)
                                वापस r;
                        अगर (r == 0)
                                वापस -EOPNOTSUPP;

                        /* On the first try, read multiple characters, i.e. the shortest valid
                         * reply. Afterwards read byte-wise, since we don't want to read too much, and
                         * unnecessarily drop too many characters from the input queue. */
                        ssize_t l = पढ़ो(nonblock_निवेश_fd, buf, first ? माप(buf) : 1);
                        अगर (l < 0) {
                                अगर (त्रुटि_सं == EAGAIN)
                                        जारी;

                                वापस -त्रुटि_सं;
                        }

                        निश्चित((माप_प्रकार) l <= माप(buf));
                        buf_full = l;
                }

                माप_प्रकार processed;
                r = scan_cursor_position_response(&context, buf, buf_full, &processed);
                अगर (r < 0)
                        वापस r;

                निश्चित(processed <= buf_full);
                buf_full -= processed;
                स_हटाओ(buf, buf + processed, buf_full);

                अगर (r > 0) {
                        /* Superficial validity checks (no particular reason to check for < 4, it's
                         * just a way to look for unreasonably small values) */
                        अगर (context.row < 4 || context.column < 4 || context.row >= 32766 || context.column >= 32766)
                                वापस -ENODATA;

                        अगर (ret_rows)
                                *ret_rows = context.row;
                        अगर (ret_columns)
                                *ret_columns = context.column;

                        वापस 0;
                }
        }
}

/* Common setup for ANSI terminal queries: validate the fds, open a non-blocking input fd, and configure
 * termios with echo and canonical mode disabled. Caller must restore termios and close the fd when done. */
अटल पूर्णांक terminal_prepare_query(
                पूर्णांक निवेश_fd,
                पूर्णांक output_fd,
                पूर्णांक *ret_nonblock_fd,
                काष्ठा termios *ret_saved_termios) {

        पूर्णांक r;

        निश्चित(निवेश_fd >= 0);
        निश्चित(output_fd >= 0);
        निश्चित(ret_nonblock_fd);
        निश्चित(ret_saved_termios);

        /* Use getenv_terminal_is_dumb() instead of terminal_is_dumb() here since we operate on an
         * explicitly passed fd, not on stdio. terminal_is_dumb() additionally checks on_tty() which
         * tests whether *stderr* is a tty — that's irrelevant when we're querying a directly opened
         * terminal such as /dev/console. */
        अगर (getenv_terminal_is_dumb())
                वापस -EOPNOTSUPP;

        r = terminal_verify_same(निवेश_fd, output_fd);
        अगर (r < 0)
                वापस log_debug_errno(r, "Called with distinct input/output fds: %m");

        /* Open a 2nd input fd, in non-blocking mode, so that we won't ever hang in read()
         * should someone else process the POLLIN. Do all subsequent operations on the new fd. */
        _cleanup_close_ पूर्णांक nonblock_निवेश_fd = r = fd_reopen(निवेश_fd, O_RDONLY|O_CLOEXEC|O_NONBLOCK|O_NOCTTY);
        अगर (r < 0)
                वापस r;

        अगर (tcgetattr(nonblock_निवेश_fd, ret_saved_termios) < 0)
                वापस log_debug_errno(त्रुटि_सं, "Failed to get terminal settings: %m");

        काष्ठा termios new_termios = *ret_saved_termios;
        termios_disable_echo(&new_termios);

        अगर (tcsetattr(nonblock_निवेश_fd, TCSANOW, &new_termios) < 0)
                वापस log_debug_errno(त्रुटि_सं, "Failed to set new terminal settings: %m");

        *ret_nonblock_fd = TAKE_FD(nonblock_निवेश_fd);
        वापस 0;
}

/*
 * See https://terminalguide.namepad.de/seq/csi_st-18/,
 * https://invisible-island.net/xterm/ctlseqs/ctlseqs.html#h3-Functions-using-CSI-_-ordered-by-the-final-character_s_.
 */
#घोषणा CSI18_Q  "\x1B[18t"               /* Report the size of the text area in characters */
#घोषणा CSI18_Rp "\x1B[8;"                /* Reply prefix */
#घोषणा CSI18_R0 CSI18_Rp "1;1t"          /* Shortest reply */
#घोषणा CSI18_R1 CSI18_Rp "32766;32766t"  /* Longest reply */

अटल पूर्णांक scan_text_area_आकार_response(
                स्थिर अक्षर *buf,
                माप_प्रकार आकार,
                अचिन्हित *ret_rows,
                अचिन्हित *ret_columns) {

        निश्चित(buf);
        निश्चित(ret_rows);
        निश्चित(ret_columns);

        /* Check if we have enough space for the shortest possible answer. */
        अगर (आकार < STRLEN(CSI18_R0))
                वापस -EAGAIN;

        /* Check if the terminating sequence is present */
        अगर (buf[आकार - 1] != 't')
                वापस -EAGAIN;

        अचिन्हित लघु rows, columns;
        अगर (माला_पूछो(buf, CSI18_Rp "%hu;%hut", &rows, &columns) != 2)
                वापस -EINVAL;

        *ret_rows = rows;
        *ret_columns = columns;
        वापस 0;
}

/* Determine terminal dimensions by means of an ANSI CSI 18 sequence.
 *
 * Caller must have already opened a non-blocking input fd and configured termios (echo/icanon off). */
अटल पूर्णांक terminal_query_आकार_by_csi18(
                पूर्णांक nonblock_निवेश_fd,
                पूर्णांक output_fd,
                अचिन्हित *ret_rows,
                अचिन्हित *ret_columns) {

        पूर्णांक r;

        निश्चित(nonblock_निवेश_fd >= 0);
        निश्चित(output_fd >= 0);

        r = loop_write_full(output_fd, CSI18_Q, SIZE_MAX, CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        अगर (r < 0)
                वापस r;

        usec_t end = usec_add(now(CLOCK_MONOTONIC), CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        अक्षर buf[STRLEN(CSI18_R1)];
        माप_प्रकार bytes = 0;

        क्रम (;;) {
                usec_t n = now(CLOCK_MONOTONIC);
                अगर (n >= end)
                        वापस -EOPNOTSUPP;

                r = fd_wait_for_event(nonblock_निवेश_fd, POLLIN, usec_sub_unsigned(end, n));
                अगर (r < 0)
                        वापस r;
                अगर (r == 0)
                        वापस -EOPNOTSUPP;

                /* On the first read, read multiple characters, i.e. the shortest valid reply. Afterwards
                 * read byte by byte, since we don't want to read too much and drop characters from the input
                 * queue. */
                ssize_t l = पढ़ो(nonblock_निवेश_fd, buf + bytes, bytes == 0 ? STRLEN(CSI18_R0) : 1);
                अगर (l < 0) {
                        अगर (त्रुटि_सं == EAGAIN)
                                जारी;
                        वापस -त्रुटि_सं;
                }

                निश्चित((माप_प्रकार) l <= माप(buf) - bytes);
                bytes += l;

                r = scan_text_area_आकार_response(buf, bytes, ret_rows, ret_columns);
                अगर (r != -EAGAIN)
                        वापस r;

                अगर (bytes == माप(buf))
                        वापस -EOPNOTSUPP; /* The response has the right prefix, but we didn't find a valid
                                             * answer with a terminator in the allotted space. Something is
                                             * wrong, possibly some unrelated bytes got injected into the
                                             * answer. */
        }
}

पूर्णांक terminal_get_आकार(
                पूर्णांक निवेश_fd,
                पूर्णांक output_fd,
                अचिन्हित *ret_rows,
                अचिन्हित *ret_columns,
                bool try_dsr,
                bool try_csi18) {

        _cleanup_close_ पूर्णांक nonblock_निवेश_fd = -EBADF;
        काष्ठा termios old_termios = TERMIOS_NULL;
        CLEANUP_TERMIOS_RESET(nonblock_निवेश_fd, old_termios);
        _cleanup_(nonblock_resetp) पूर्णांक nonblock_reset = -EBADF;
        पूर्णांक r;

        निश्चित(try_dsr || try_csi18);

        r = terminal_prepare_query(निवेश_fd, output_fd, &nonblock_निवेश_fd, &old_termios);
        अगर (r < 0)
                वापस r;

        /* Put the output fd in non-blocking mode with a write timeout, to avoid blocking indefinitely on
         * write if the terminal is not consuming data (e.g. serial console with flow control). */
        r = fd_nonblock(output_fd, true);
        अगर (r < 0)
                वापस log_debug_errno(r, "Failed to set terminal to non-blocking mode: %m");
        अगर (r > 0)
                nonblock_reset = output_fd;

        /* Flush any stale input that might confuse the response parsers. */
        (व्योम) tcflush(nonblock_निवेश_fd, TCIFLUSH);

        अगर (try_csi18) {
                r = terminal_query_आकार_by_csi18(nonblock_निवेश_fd, output_fd, ret_rows, ret_columns);
                अगर (r >= 0)
                        वापस r;

                /* Query failed. Flush any outstanding input. */
                (व्योम) tcflush(nonblock_निवेश_fd, TCIFLUSH);

                अगर (!IN_SET(r, -EOPNOTSUPP, -EINVAL))
                        वापस r;
        }

        अगर (try_dsr) {
                r = terminal_query_आकार_by_dsr(nonblock_निवेश_fd, output_fd, ret_rows, ret_columns);
                अगर (r >= 0)
                        वापस r;

                /* Query failed. Flush any outstanding input. */
                (व्योम) tcflush(nonblock_निवेश_fd, TCIFLUSH);
        }

        वापस r;
}

अटल पूर्णांक fd_is_vc(पूर्णांक fd) {
        _cleanup_free_ अक्षर *name = शून्य;
        पूर्णांक r;

        निश्चित(fd >= 0);

        r = getttyname_harder(fd, &name);
        अगर (r < 0)
                वापस r;

        वापस tty_is_vc_resolve(name);
}

पूर्णांक terminal_fix_आकार(पूर्णांक निवेश_fd, पूर्णांक output_fd) {
        अचिन्हित rows, columns;
        पूर्णांक r;

        /* Don't ask a virtual console at all. There is no intermediary here that could be wrong about the
         * dimensions: the kernel renders the screen itself, so its winsize is authoritative. */
        r = fd_is_vc(निवेश_fd);
        अगर (r < 0)
                log_debug_errno(r, "Failed to determine whether terminal is a virtual console, assuming it is not: %m");
        अन्यथा अगर (r > 0) {
                log_debug("Not querying dimensions of a virtual console, kernel dimensions are authoritative.");
                वापस 0;
        }

        काष्ठा winsize ws = {};
        अगर (ioctl(output_fd, TIOCGWINSZ, &ws) < 0)
                वापस log_debug_errno(त्रुटि_सं, "Failed to query terminal dimensions, ignoring: %m");

        r = terminal_get_आकार(निवेश_fd, output_fd, &rows, &columns, /* try_dsr= */ true, /* try_csi18= */ true);
        अगर (r < 0)
                वापस log_debug_errno(r, "Failed to acquire terminal dimensions via ANSI sequences, not adjusting terminal dimensions: %m");

        अगर (ws.ws_row == rows && ws.ws_col == columns) {
                log_debug("Terminal dimensions reported via ANSI sequences match currently set terminal dimensions, not changing.");
                वापस 0;
        }

        ws.ws_col = columns;
        ws.ws_row = rows;

        अगर (ioctl(output_fd, TIOCSWINSZ, &ws) < 0)
                वापस log_debug_errno(त्रुटि_सं, "Failed to update terminal dimensions, ignoring: %m");

        log_debug("Fixed terminal dimensions to %ux%u based on ANSI sequence information.", columns, rows);
        वापस 1;
}

#घोषणा MAX_TERMINFO_लंबाई 64
/* python -c 'print("".join(hex(ord(i))[2:] for i in "name").upper())' */
#घोषणा DCS_TERMINFO_Q ANSI_DCS "+q" "6E616D65" ANSI_ST
/* The answer is either 0+r… (invalid) or 1+r… (OK). */
#घोषणा DCS_TERMINFO_R0 ANSI_DCS "0+r" ANSI_ST
#घोषणा DCS_TERMINFO_R1 ANSI_DCS "1+r" "6E616D65" "=" /* This is followed by Pt ST. */
assert_cc(STRLEN(DCS_TERMINFO_R0) <= STRLEN(DCS_TERMINFO_R1 ANSI_ST));

अटल पूर्णांक scan_terminfo_response(
                स्थिर अक्षर *buf,
                माप_प्रकार आकार,
                अक्षर **ret_name) {
        पूर्णांक r;

        निश्चित(buf);
        निश्चित(ret_name);

        /* Check if we have enough space for the shortest possible answer. */
        अगर (आकार < STRLEN(DCS_TERMINFO_R0))
                वापस -EAGAIN;

        /* Check if the terminating sequence is present */
        अगर (स_भेद(buf + आकार - STRLEN(ANSI_ST), ANSI_ST, STRLEN(ANSI_ST)) != 0)
                वापस -EAGAIN;

        अगर (आकार <= STRLEN(DCS_TERMINFO_R1 ANSI_ST))
                वापस -EINVAL;  /* The answer is invalid or empty */

        अगर (स_भेद(buf, DCS_TERMINFO_R1, STRLEN(DCS_TERMINFO_R1)) != 0)
                वापस -EINVAL;  /* The answer is not valid */

        _cleanup_free_ व्योम *dec = शून्य;
        माप_प्रकार dec_आकार;
        r = unhexmem_full(buf + STRLEN(DCS_TERMINFO_R1), आकार - STRLEN(DCS_TERMINFO_R1 ANSI_ST),
                          /* secure= */ false,
                          &dec, &dec_आकार);
        अगर (r < 0)
                वापस r;

        निश्चित(((स्थिर अक्षर *) dec)[dec_आकार] == '\0'); /* unhexmem appends NUL for our convenience */
        अगर (स_प्रथम(dec, '\0', dec_आकार) || string_has_cc(dec, शून्य) || !filename_is_valid(dec))
                वापस -EUCLEAN;

        *ret_name = TAKE_PTR(dec);
        वापस 0;
}

पूर्णांक terminal_get_terminfo_by_dcs(पूर्णांक fd, अक्षर **ret_name) {
        पूर्णांक r;

        निश्चित(fd >= 0);
        निश्चित(ret_name);

        /* Note: fd must be in non-blocking read-write mode! */

        काष्ठा termios old_termios = TERMIOS_NULL;
        CLEANUP_TERMIOS_RESET(fd, old_termios);

        अगर (tcgetattr(fd, &old_termios) < 0)
                वापस -त्रुटि_सं;

        काष्ठा termios new_termios = old_termios;
        termios_disable_echo(&new_termios);

        अगर (tcsetattr(fd, TCSANOW, &new_termios) < 0)
                वापस -त्रुटि_सं;

        r = loop_write(fd, DCS_TERMINFO_Q, SIZE_MAX);
        अगर (r < 0)
                वापस r;

        usec_t end = usec_add(now(CLOCK_MONOTONIC), CONSOLE_ANSI_SEQUENCE_TIMEOUT_USEC);
        अक्षर buf[STRLEN(DCS_TERMINFO_R1) + MAX_TERMINFO_लंबाई + STRLEN(ANSI_ST)];
        माप_प्रकार bytes = 0;

        क्रम (;;) {
                usec_t n = now(CLOCK_MONOTONIC);
                अगर (n >= end)
                        वापस -EOPNOTSUPP;

                r = fd_wait_for_event(fd, POLLIN, usec_sub_unsigned(end, n));
                अगर (r < 0)
                        वापस r;
                अगर (r == 0)
                        वापस -EOPNOTSUPP;

                /* On the first read, read multiple characters, i.e. the shortest valid reply. Afterwards
                 * read byte by byte, since we don't want to read too much and drop characters from the input
                 * queue. */
                ssize_t l = पढ़ो(fd, buf + bytes, bytes == 0 ? STRLEN(DCS_TERMINFO_R0) : 1);
                अगर (l < 0) {
                        अगर (त्रुटि_सं == EAGAIN)
                                जारी;
                        वापस -त्रुटि_सं;
                }

                निश्चित((माप_प्रकार) l <= माप(buf) - bytes);
                bytes += l;

                r = scan_terminfo_response(buf, bytes, ret_name);
                अगर (r != -EAGAIN)
                        वापस r;

                अगर (bytes == माप(buf))
                        वापस -EOPNOTSUPP; /* The response has the right prefix, but we didn't find a valid
                                             * answer with a terminator in the allotted space. Something is
                                             * wrong, possibly some unrelated bytes got injected into the
                                             * answer. */
        }
}

पूर्णांक have_terminfo_file(स्थिर अक्षर *name) {
        /* This is a heuristic check if we have the file, using the directory layout used on
         * current Linux systems. Checks for other layouts can be added later if appropriate. */
        पूर्णांक r;

        निश्चित(filename_is_valid(name));

        _cleanup_free_ अक्षर *p = path_join("/usr/share/terminfo", CHAR_TO_STR(name[0]), name);
        अगर (!p)
                वापस log_oom_debug();

        r = RET_NERRNO(access(p, F_OK));
        अगर (r == -ENOENT)
                वापस false;
        अगर (r < 0)
                वापस r;
        वापस true;
}

पूर्णांक query_term_for_tty(स्थिर अक्षर *tty, अक्षर **ret_term) {
        _cleanup_free_ अक्षर *dcs_term = शून्य;
        पूर्णांक r;

        निश्चित(tty);
        निश्चित(ret_term);

        अगर (tty_is_vc_resolve(tty))
                वापस strdup_to(ret_term, "linux");

        /* Try to query the terminal implementation that we're on. This will not work in all
         * cases, which is fine, since this is intended to be used as a fallback. */

        _cleanup_close_ पूर्णांक tty_fd = open_terminal(tty, O_RDWR|O_NOCTTY|O_CLOEXEC|O_NONBLOCK);
        अगर (tty_fd < 0)
                वापस log_debug_errno(tty_fd, "Failed to open %s to query terminfo: %m", tty);

        r = terminal_get_terminfo_by_dcs(tty_fd, &dcs_term);
        अगर (r < 0)
                वापस log_debug_errno(r, "Failed to query %s for terminfo: %m", tty);

        r = have_terminfo_file(dcs_term);
        अगर (r < 0)
                वापस log_debug_errno(r, "Failed to look for terminfo %s: %m", dcs_term);
        अगर (r == 0)
                वापस log_info_errno(SYNTHETIC_ERRNO(ENODATA),
                                      "Terminfo %s not found for %s.", dcs_term, tty);

        *ret_term = TAKE_PTR(dcs_term);
        वापस 0;
}

पूर्णांक terminal_is_pty_fd(पूर्णांक fd) {
        पूर्णांक r;

        निश्चित(fd >= 0);

        /* Returns true if we are looking at a pty, i.e. if it's backed by the /dev/pts/ file system */

        अगर (!isatty_safe(fd))
                वापस false;

        r = is_fs_type_at(fd, शून्य, DEVPTS_SUPER_MAGIC);
        अगर (r != 0)
                वापस r;

        /* The ptmx device is weird, it exists twice, once inside and once outside devpts. To detect the
         * latter case, let's fire off an ioctl() that only works on ptmx devices. */

        पूर्णांक v;
        अगर (ioctl(fd, TIOCGPKT, &v) < 0) {
                अगर (ERRNO_IS_NOT_SUPPORTED(त्रुटि_सं))
                        वापस false;

                वापस -त्रुटि_सं;
        }

        वापस true;
}

पूर्णांक pty_open_peer(पूर्णांक fd, पूर्णांक mode) {
        निश्चित(fd >= 0);

        /* Opens the peer PTY using the new race-free TIOCGPTPEER ioctl() (kernel 4.13).
         *
         * This is safe to be called on TTYs from other namespaces. */

        निश्चित((mode & (O_CREAT|O_PATH|O_DIRECTORY|O_TMPFILE)) == 0);

        /* This replicates the EIO retry logic of open_terminal() in a modified way. */
        क्रम (अचिन्हित c = 0;; c++) {
                पूर्णांक peer_fd = ioctl(fd, TIOCGPTPEER, mode);
                अगर (peer_fd >= 0)
                        वापस peer_fd;

                अगर (त्रुटि_सं != EIO)
                        वापस -त्रुटि_सं;

                /* Max 1s in total */
                अगर (c >= 20)
                        वापस -EIO;

                (व्योम) usleep_safe(50 * USEC_PER_MSEC);
        }
}
