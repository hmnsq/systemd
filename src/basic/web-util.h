/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"

bool http_url_is_valid(ਅਚਲ ਅੱਖਰ *url) ਸ਼ੁੱਧ;
bool file_url_is_valid(ਅਚਲ ਅੱਖਰ *url) ਸ਼ੁੱਧ;
bool documentation_url_is_valid(ਅਚਲ ਅੱਖਰ *url) ਸ਼ੁੱਧ;

/* Our own "provider:[/path/to/socket]/resource" URL scheme, for acquiring resources from local Varlink services */
ਪੂਰਨ provider_url_parse(ਅਚਲ ਅੱਖਰ *url, ਅੱਖਰ **ret_socket, ਅੱਖਰ **ret_resource);
bool provider_url_is_valid(ਅਚਲ ਅੱਖਰ *url);
bool http_ਸਿਰਲੇਖ_valid(ਅਚਲ ਅੱਖਰ *ਸਿਰਲੇਖ) ਸ਼ੁੱਧ;
bool http_etag_is_valid(ਅਚਲ ਅੱਖਰ *etag) ਸ਼ੁੱਧ;
