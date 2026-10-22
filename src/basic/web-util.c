/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن "alloc-util.h"
#ضمن "path-util.h"
#ضمن "string-util.h"
#ضمن "strv.h"
#ضمن "utf8.h"
#ضمن "web-util.h"

bool http_url_is_valid(ثابت حرف *url) {
        ثابت حرف *p;

        ܐܢ (isempty(url))
                ܦܢܐ false;

        p = STARTSWITH_SET(url, "http://", "https://");
        ܐܢ (!p)
                ܦܢܐ false;

        ܐܢ (isempty(p))
                ܦܢܐ false;

        ܦܢܐ ascii_is_valid(p);
}

bool file_url_is_valid(ثابت حرف *url) {
        ثابت حرف *p;

        ܐܢ (isempty(url))
                ܦܢܐ false;

        p = startswith(url, "file:/");
        ܐܢ (isempty(p))
                ܦܢܐ false;

        ܦܢܐ ascii_is_valid(p);
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

        ܐܢ (isempty(url))
                ܦܢܐ -EINVAL;

        p = startswith(url, "provider:[");
        ܐܢ (!p)
                ܦܢܐ -EINVAL;

        e = strchr(p, ']');
        ܐܢ (!e || e == p)
                ܦܢܐ -EINVAL;
        ܐܢ (e[1] != '/')
                ܦܢܐ -EINVAL;

        _cleanup_free_ حرف *s = strndup(p, e - p);
        ܐܢ (!s)
                ܦܢܐ -ENOMEM;

        ܐܢ (!path_is_absolute(s) || !path_is_normalized(s))
                ܦܢܐ -EINVAL;

        ثابت حرف *resource = e + 2;
        ܐܢ (isempty(resource))
                ܦܢܐ -EINVAL;
        ܐܢ (path_is_absolute(resource) || !path_is_normalized(resource))
                ܦܢܐ -EINVAL;
        ܐܢ (string_has_cc(resource, /* ok= */ عدم) || strpbrk(resource, "?#"))
                ܦܢܐ -EINVAL;

        r = strdup_to_full(ret_resource, resource);
        ܐܢ (r < 0)
                ܦܢܐ r;

        ܐܢ (ret_socket)
                *ret_socket = TAKE_PTR(s);

        ܦܢܐ 0;
}

bool provider_url_is_valid(ثابت حرف *url) {
        ܦܢܐ provider_url_parse(url, /* ret_socket= */ عدم, /* ret_resource= */ عدم) >= 0;
}

bool documentation_url_is_valid(ثابت حرف *url) {
        ثابت حرف *p;

        ܐܢ (isempty(url))
                ܦܢܐ false;

        ܐܢ (http_url_is_valid(url) || file_url_is_valid(url))
                ܦܢܐ true;

        p = STARTSWITH_SET(url, "info:", "man:");
        ܐܢ (isempty(p))
                ܦܢܐ false;

        ܦܢܐ ascii_is_valid(p);
}

bool http_header_valid(ثابت حرف *header) {
        ܦܢܐ header &&
                ascii_is_valid(header) &&
                !string_has_cc(header, /* ok= */ عدم) &&
                strchr(header, ':');
}

bool http_etag_is_valid(ثابت حرف *etag) {
        ܐܢ (isempty(etag))
                ܦܢܐ false;

        ܐܢ (!endswith(etag, "\""))
                ܦܢܐ false;

        ܐܢ (!STARTSWITH_SET(etag, "\"", "W/\""))
                ܦܢܐ false;

        ܦܢܐ true;
}
