/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن "alloc-util.h"
#ضمن "path-util.h"
#ضمن "string-util.h"
#ضمن "strv.h"
#ضمن "utf8.h"
#ضمن "web-util.h"

bool http_url_is_valid(ثابت حرف *url) {
        ثابت حرف *p;

        إذا (isempty(url))
                أرجع false;

        p = STARTSWITH_مجموعه(url, "http://", "https://");
        إذا (!p)
                أرجع false;

        إذا (isempty(p))
                أرجع false;

        أرجع ascii_is_valid(p);
}

bool file_url_is_valid(ثابت حرف *url) {
        ثابت حرف *p;

        إذا (isempty(url))
                أرجع false;

        p = startswith(url, "file:/");
        إذا (isempty(p))
                أرجع false;

        أرجع ascii_is_valid(p);
}

صحيح provider_url_parse(ثابت حرف *url, حرف **ret_socket, حرف **ret_resource) {
        ثابت حرف *p, *e;
        صحيح r;

        /* Parses a URL of the form "provider:[/path/to/socket]/resource", which refers to a resource
         * offered by a local Varlink service implementing the io.systemd.ResourceProvider interface on the
         * specified AF_UNIX socket. The socket path must be absolute and normalized. The resource name is
         * an opaque identifier that may contain slashes, but must not be empty, absolute, or contain
         * control characters, "." or ".." components. Query and fragment separators are not permitted
         * either, so that generic URL path manipulation works reliably on the resource name. */

        إذا (isempty(url))
                أرجع -EINVAL;

        p = startswith(url, "provider:[");
        إذا (!p)
                أرجع -EINVAL;

        e = strchr(p, ']');
        إذا (!e || e == p)
                أرجع -EINVAL;
        إذا (e[1] != '/')
                أرجع -EINVAL;

        _cleanup_free_ حرف *s = strndup(p, e - p);
        إذا (!s)
                أرجع -ENOMEM;

        إذا (!مسیر_is_absolute(s) || !مسیر_is_normalized(s))
                أرجع -EINVAL;

        ثابت حرف *resource = e + 2;
        إذا (isempty(resource))
                أرجع -EINVAL;
        إذا (مسیر_is_absolute(resource) || !مسیر_is_normalized(resource))
                أرجع -EINVAL;
        إذا (string_has_cc(resource, /* ok= */ عدم) || strpbrk(resource, "?#"))
                أرجع -EINVAL;

        r = strdup_مقصد_پر(ret_resource, resource);
        إذا (r < 0)
                أرجع r;

        إذا (ret_socket)
                *ret_socket = TAKE_PTR(s);

        أرجع 0;
}

bool provider_url_is_valid(ثابت حرف *url) {
        أرجع provider_url_parse(url, /* ret_socket= */ عدم, /* ret_resource= */ عدم) >= 0;
}

bool documentation_url_is_valid(ثابت حرف *url) {
        ثابت حرف *p;

        إذا (isempty(url))
                أرجع false;

        إذا (http_url_is_valid(url) || file_url_is_valid(url))
                أرجع true;

        p = STARTSWITH_مجموعه(url, "info:", "man:");
        إذا (isempty(p))
                أرجع false;

        أرجع ascii_is_valid(p);
}

bool http_سرآیند_valid(ثابت حرف *سرآیند) {
        أرجع سرآیند &&
                ascii_is_valid(سرآیند) &&
                !string_has_cc(سرآیند, /* ok= */ عدم) &&
                strchr(سرآیند, ':');
}

bool http_etag_is_valid(ثابت حرف *etag) {
        إذا (isempty(etag))
                أرجع false;

        إذا (!endswith(etag, "\""))
                أرجع false;

        إذا (!STARTSWITH_مجموعه(etag, "\"", "W/\""))
                أرجع false;

        أرجع true;
}
