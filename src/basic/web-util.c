/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश "alloc-util.h"
#समावेश "path-util.h"
#समावेश "string-util.h"
#समावेश "strv.h"
#समावेश "utf8.h"
#समावेश "web-util.h"

bool http_url_is_valid(स्थिर अक्षर *url) {
        स्थिर अक्षर *p;

        अगर (isempty(url))
                वापस false;

        p = STARTSWITH_SET(url, "http://", "https://");
        अगर (!p)
                वापस false;

        अगर (isempty(p))
                वापस false;

        वापस ascii_is_valid(p);
}

bool file_url_is_valid(स्थिर अक्षर *url) {
        स्थिर अक्षर *p;

        अगर (isempty(url))
                वापस false;

        p = startswith(url, "file:/");
        अगर (isempty(p))
                वापस false;

        वापस ascii_is_valid(p);
}

पूर्णांक provider_url_parse(स्थिर अक्षर *url, अक्षर **ret_socket, अक्षर **ret_resource) {
        स्थिर अक्षर *p, *e;
        पूर्णांक r;

        /* Parses a URL of the form "provider:[/path/to/socket]/resource", which refers to a resource
         * offered by a local Varlink service implementing the io.systemd.ResourceProvider interface on the
         * specified AF_UNIX socket. The socket path must be absolute and normalized. The resource name is
         * an opaque identifier that may contain slashes, but must not be empty, absolute, or contain
         * control characters, "." or ".." components. Query and fragment separators are not permitted
         * either, so that generic URL path manipulation works reliably on the resource name. */

        अगर (isempty(url))
                वापस -EINVAL;

        p = startswith(url, "provider:[");
        अगर (!p)
                वापस -EINVAL;

        e = म_अक्षर(p, ']');
        अगर (!e || e == p)
                वापस -EINVAL;
        अगर (e[1] != '/')
                वापस -EINVAL;

        _cleanup_free_ अक्षर *s = strndup(p, e - p);
        अगर (!s)
                वापस -ENOMEM;

        अगर (!path_is_absolute(s) || !path_is_normalized(s))
                वापस -EINVAL;

        स्थिर अक्षर *resource = e + 2;
        अगर (isempty(resource))
                वापस -EINVAL;
        अगर (path_is_absolute(resource) || !path_is_normalized(resource))
                वापस -EINVAL;
        अगर (string_has_cc(resource, /* ok= */ शून्य) || strpbrk(resource, "?#"))
                वापस -EINVAL;

        r = strdup_to_full(ret_resource, resource);
        अगर (r < 0)
                वापस r;

        अगर (ret_socket)
                *ret_socket = TAKE_PTR(s);

        वापस 0;
}

bool provider_url_is_valid(स्थिर अक्षर *url) {
        वापस provider_url_parse(url, /* ret_socket= */ शून्य, /* ret_resource= */ शून्य) >= 0;
}

bool documentation_url_is_valid(स्थिर अक्षर *url) {
        स्थिर अक्षर *p;

        अगर (isempty(url))
                वापस false;

        अगर (http_url_is_valid(url) || file_url_is_valid(url))
                वापस true;

        p = STARTSWITH_SET(url, "info:", "man:");
        अगर (isempty(p))
                वापस false;

        वापस ascii_is_valid(p);
}

bool http_header_valid(स्थिर अक्षर *header) {
        वापस header &&
                ascii_is_valid(header) &&
                !string_has_cc(header, /* ok= */ शून्य) &&
                म_अक्षर(header, ':');
}

bool http_etag_is_valid(स्थिर अक्षर *etag) {
        अगर (isempty(etag))
                वापस false;

        अगर (!endswith(etag, "\""))
                वापस false;

        अगर (!STARTSWITH_SET(etag, "\"", "W/\""))
                वापस false;

        वापस true;
}
