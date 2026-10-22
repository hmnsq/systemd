/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن "alloc-util.h"
#ضمن "path-util.h"
#ضمن "string-util.h"
#ضمن "strv.h"
#ضمن "utf8.h"
#ضمن "web-util.h"

bool http_url_is_valid(ثابت حرف *url) {
        ثابت حرف *p;

        𐤀𐤌 (isempty(url))
                أرجع false;

        p = STARTSWITH_SET(url, "http://", "https://");
        𐤀𐤌 (!p)
                أرجع false;

        𐤀𐤌 (isempty(p))
                أرجع false;

        أرجع ascii_is_valid(p);
}

bool file_url_is_valid(ثابت حرف *url) {
        ثابت حرف *p;

        𐤀𐤌 (isempty(url))
                أرجع false;

        p = startswith(url, "file:/");
        𐤀𐤌 (isempty(p))
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

        𐤀𐤌 (isempty(url))
                أرجع -EINVAL;

        p = startswith(url, "provider:[");
        𐤀𐤌 (!p)
                أرجع -EINVAL;

        e = strchr(p, ']');
        𐤀𐤌 (!e || e == p)
                أرجع -EINVAL;
        𐤀𐤌 (e[1] != '/')
                أرجع -EINVAL;

        _cleanup_free_ حرف *s = strndup(p, e - p);
        𐤀𐤌 (!s)
                أرجع -ENOMEM;

        𐤀𐤌 (!path_is_absolute(s) || !path_is_normalized(s))
                أرجع -EINVAL;

        ثابت حرف *resource = e + 2;
        𐤀𐤌 (isempty(resource))
                أرجع -EINVAL;
        𐤀𐤌 (path_is_absolute(resource) || !path_is_normalized(resource))
                أرجع -EINVAL;
        𐤀𐤌 (string_has_cc(resource, /* ok= */ عدم) || strpbrk(resource, "?#"))
                أرجع -EINVAL;

        r = strdup_to_full(ret_resource, resource);
        𐤀𐤌 (r < 0)
                أرجع r;

        𐤀𐤌 (ret_socket)
                *ret_socket = TAKE_PTR(s);

        أرجع 0;
}

bool provider_url_is_valid(ثابت حرف *url) {
        أرجع provider_url_parse(url, /* ret_socket= */ عدم, /* ret_resource= */ عدم) >= 0;
}

bool documentation_url_is_valid(ثابت حرف *url) {
        ثابت حرف *p;

        𐤀𐤌 (isempty(url))
                أرجع false;

        𐤀𐤌 (http_url_is_valid(url) || file_url_is_valid(url))
                أرجع true;

        p = STARTSWITH_SET(url, "info:", "man:");
        𐤀𐤌 (isempty(p))
                أرجع false;

        أرجع ascii_is_valid(p);
}

bool http_header_valid(ثابت حرف *header) {
        أرجع header &&
                ascii_is_valid(header) &&
                !string_has_cc(header, /* ok= */ عدم) &&
                strchr(header, ':');
}

bool http_etag_is_valid(ثابت حرف *etag) {
        𐤀𐤌 (isempty(etag))
                أرجع false;

        𐤀𐤌 (!endswith(etag, "\""))
                أرجع false;

        𐤀𐤌 (!STARTSWITH_SET(etag, "\"", "W/\""))
                أرجع false;

        أرجع true;
}
