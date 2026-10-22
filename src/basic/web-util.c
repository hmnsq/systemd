/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ "alloc-util.h"
#അന്തര്ഭാവ "path-util.h"
#അന്തര്ഭാവ "string-util.h"
#അന്തര്ഭാവ "strv.h"
#അന്തര്ഭാവ "utf8.h"
#അന്തര്ഭാവ "web-util.h"

bool http_url_is_valid(സ്ഥിരം അക്ഷരം *url) {
        സ്ഥിരം അക്ഷരം *p;

        എങ്കിൽ (isempty(url))
                തിരികെ false;

        p = STARTSWITH_SET(url, "http://", "https://");
        എങ്കിൽ (!p)
                തിരികെ false;

        എങ്കിൽ (isempty(p))
                തിരികെ false;

        തിരികെ ascii_is_valid(p);
}

bool file_url_is_valid(സ്ഥിരം അക്ഷരം *url) {
        സ്ഥിരം അക്ഷരം *p;

        എങ്കിൽ (isempty(url))
                തിരികെ false;

        p = startswith(url, "file:/");
        എങ്കിൽ (isempty(p))
                തിരികെ false;

        തിരികെ ascii_is_valid(p);
}

പൂർണ്ണസംഖ്യ provider_url_parse(സ്ഥിരം അക്ഷരം *url, അക്ഷരം **ret_socket, അക്ഷരം **ret_resource) {
        സ്ഥിരം അക്ഷരം *p, *e;
        പൂർണ്ണസംഖ്യ r;

        /* Parses a URL of the form "provider:[/path/to/socket]/resource", which refers to a resource
         * offered by a local Varlink service implementing the io.systemd.ResourceProvider interface on the
         * specified AF_UNIX socket. The socket path must be absolute and normalized. The resource name is
         * an opaque identifier that may contain slashes, but must not be empty, absolute, or contain
         * control characters, "." or ".." components. Query and fragment separators are not permitted
         * either, so that generic URL path manipulation works reliably on the resource name. */

        എങ്കിൽ (isempty(url))
                തിരികെ -EINVAL;

        p = startswith(url, "provider:[");
        എങ്കിൽ (!p)
                തിരികെ -EINVAL;

        e = strchr(p, ']');
        എങ്കിൽ (!e || e == p)
                തിരികെ -EINVAL;
        എങ്കിൽ (e[1] != '/')
                തിരികെ -EINVAL;

        _cleanup_free_ അക്ഷരം *s = strndup(p, e - p);
        എങ്കിൽ (!s)
                തിരികെ -ENOMEM;

        എങ്കിൽ (!path_is_absolute(s) || !path_is_normalized(s))
                തിരികെ -EINVAL;

        സ്ഥിരം അക്ഷരം *resource = e + 2;
        എങ്കിൽ (isempty(resource))
                തിരികെ -EINVAL;
        എങ്കിൽ (path_is_absolute(resource) || !path_is_normalized(resource))
                തിരികെ -EINVAL;
        എങ്കിൽ (string_has_cc(resource, /* ok= */ ശൂന്യ) || strpbrk(resource, "?#"))
                തിരികെ -EINVAL;

        r = strdup_to_full(ret_resource, resource);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (ret_socket)
                *ret_socket = TAKE_PTR(s);

        തിരികെ 0;
}

bool provider_url_is_valid(സ്ഥിരം അക്ഷരം *url) {
        തിരികെ provider_url_parse(url, /* ret_socket= */ ശൂന്യ, /* ret_resource= */ ശൂന്യ) >= 0;
}

bool documentation_url_is_valid(സ്ഥിരം അക്ഷരം *url) {
        സ്ഥിരം അക്ഷരം *p;

        എങ്കിൽ (isempty(url))
                തിരികെ false;

        എങ്കിൽ (http_url_is_valid(url) || file_url_is_valid(url))
                തിരികെ true;

        p = STARTSWITH_SET(url, "info:", "man:");
        എങ്കിൽ (isempty(p))
                തിരികെ false;

        തിരികെ ascii_is_valid(p);
}

bool http_header_valid(സ്ഥിരം അക്ഷരം *header) {
        തിരികെ header &&
                ascii_is_valid(header) &&
                !string_has_cc(header, /* ok= */ ശൂന്യ) &&
                strchr(header, ':');
}

bool http_etag_is_valid(സ്ഥിരം അക്ഷരം *etag) {
        എങ്കിൽ (isempty(etag))
                തിരികെ false;

        എങ്കിൽ (!endswith(etag, "\""))
                തിരികെ false;

        എങ്കിൽ (!STARTSWITH_SET(etag, "\"", "W/\""))
                തിരികെ false;

        തിരികെ true;
}
