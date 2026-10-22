/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل "alloc-util.h"
#شامل "path-util.h"
#شامل "string-util.h"
#شامل "strv.h"
#شامل "utf8.h"
#شامل "web-util.h"

bool http_url_is_valid(ثابت حرف *url) {
        ثابت حرف *p;

        اگر (isempty(url))
                واپس false;

        p = STARTSWITH_سیٹ(url, "http://", "https://");
        اگر (!p)
                واپس false;

        اگر (isempty(p))
                واپس false;

        واپس ascii_is_valid(p);
}

bool file_url_is_valid(ثابت حرف *url) {
        ثابت حرف *p;

        اگر (isempty(url))
                واپس false;

        p = startswith(url, "file:/");
        اگر (isempty(p))
                واپس false;

        واپس ascii_is_valid(p);
}

صحیح provider_url_parse(ثابت حرف *url, حرف **ret_socket, حرف **ret_resource) {
        ثابت حرف *p, *e;
        صحیح r;

        /* Parses a URL of the form "provider:[/path/to/socket]/resource", which refers to a resource
         * offered by a local Varlink service implementing the io.systemd.ResourceProvider interface on the
         * specified AF_UNIX socket. The socket path must be absolute and normalized. The resource name is
         * an opaque identifier that may contain slashes, but must not be empty, absolute, or contain
         * control characters, "." or ".." components. Query and fragment separators are not permitted
         * either, so that generic URL path manipulation works reliably on the resource name. */

        اگر (isempty(url))
                واپس -EINVAL;

        p = startswith(url, "provider:[");
        اگر (!p)
                واپس -EINVAL;

        e = strchr(p, ']');
        اگر (!e || e == p)
                واپس -EINVAL;
        اگر (e[1] != '/')
                واپس -EINVAL;

        _cleanup_free_ حرف *s = strndup(p, e - p);
        اگر (!s)
                واپس -ENOMEM;

        اگر (!راستہ_is_absolute(s) || !راستہ_is_normalized(s))
                واپس -EINVAL;

        ثابت حرف *resource = e + 2;
        اگر (isempty(resource))
                واپس -EINVAL;
        اگر (راستہ_is_absolute(resource) || !راستہ_is_normalized(resource))
                واپس -EINVAL;
        اگر (string_has_cc(resource, /* ok= */ تہی) || strpbrk(resource, "?#"))
                واپس -EINVAL;

        r = strdup_مقصد_بھرا(ret_resource, resource);
        اگر (r < 0)
                واپس r;

        اگر (ret_socket)
                *ret_socket = TAKE_PTR(s);

        واپس 0;
}

bool provider_url_is_valid(ثابت حرف *url) {
        واپس provider_url_parse(url, /* ret_socket= */ تہی, /* ret_resource= */ تہی) >= 0;
}

bool documentation_url_is_valid(ثابت حرف *url) {
        ثابت حرف *p;

        اگر (isempty(url))
                واپس false;

        اگر (http_url_is_valid(url) || file_url_is_valid(url))
                واپس true;

        p = STARTSWITH_سیٹ(url, "info:", "man:");
        اگر (isempty(p))
                واپس false;

        واپس ascii_is_valid(p);
}

bool http_سرخی_valid(ثابت حرف *سرخی) {
        واپس سرخی &&
                ascii_is_valid(سرخی) &&
                !string_has_cc(سرخی, /* ok= */ تہی) &&
                strchr(سرخی, ':');
}

bool http_etag_is_valid(ثابت حرف *etag) {
        اگر (isempty(etag))
                واپس false;

        اگر (!endswith(etag, "\""))
                واپس false;

        اگر (!STARTSWITH_سیٹ(etag, "\"", "W/\""))
                واپس false;

        واپس true;
}
