/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن "alloc-util.h"
#ضمن "path-util.h"
#ضمن "string-util.h"
#ضمن "strv.h"
#ضمن "utf8.h"
#ضمن "web-util.h"

bool http_url_is_valid(ثابت حرف *url) {
        ثابت حرف *p;

        אם (isempty(url))
                החזר false;

        p = STARTSWITH_SET(url, "http://", "https://");
        אם (!p)
                החזר false;

        אם (isempty(p))
                החזר false;

        החזר ascii_is_valid(p);
}

bool file_url_is_valid(ثابت حرف *url) {
        ثابت حرف *p;

        אם (isempty(url))
                החזר false;

        p = startswith(url, "file:/");
        אם (isempty(p))
                החזר false;

        החזר ascii_is_valid(p);
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

        אם (isempty(url))
                החזר -EINVAL;

        p = startswith(url, "provider:[");
        אם (!p)
                החזר -EINVAL;

        e = strchr(p, ']');
        אם (!e || e == p)
                החזר -EINVAL;
        אם (e[1] != '/')
                החזר -EINVAL;

        _cleanup_free_ حرف *s = strndup(p, e - p);
        אם (!s)
                החזר -ENOMEM;

        אם (!path_is_absolute(s) || !path_is_normalized(s))
                החזר -EINVAL;

        ثابت حرف *resource = e + 2;
        אם (isempty(resource))
                החזר -EINVAL;
        אם (path_is_absolute(resource) || !path_is_normalized(resource))
                החזר -EINVAL;
        אם (string_has_cc(resource, /* ok= */ عدم) || strpbrk(resource, "?#"))
                החזר -EINVAL;

        r = strdup_to_full(ret_resource, resource);
        אם (r < 0)
                החזר r;

        אם (ret_socket)
                *ret_socket = TAKE_PTR(s);

        החזר 0;
}

bool provider_url_is_valid(ثابت حرف *url) {
        החזר provider_url_parse(url, /* ret_socket= */ عدم, /* ret_resource= */ عدم) >= 0;
}

bool documentation_url_is_valid(ثابت حرف *url) {
        ثابت حرف *p;

        אם (isempty(url))
                החזר false;

        אם (http_url_is_valid(url) || file_url_is_valid(url))
                החזר true;

        p = STARTSWITH_SET(url, "info:", "man:");
        אם (isempty(p))
                החזר false;

        החזר ascii_is_valid(p);
}

bool http_header_valid(ثابت حرف *header) {
        החזר header &&
                ascii_is_valid(header) &&
                !string_has_cc(header, /* ok= */ عدم) &&
                strchr(header, ':');
}

bool http_etag_is_valid(ثابت حرف *etag) {
        אם (isempty(etag))
                החזר false;

        אם (!endswith(etag, "\""))
                החזר false;

        אם (!STARTSWITH_SET(etag, "\"", "W/\""))
                החזר false;

        החזר true;
}
