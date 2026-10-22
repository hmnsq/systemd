/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#அந்தர்பாவ "forward.h"

bool http_url_is_valid(மாறிலி எழுத்து *url) _pure_;
bool file_url_is_valid(மாறிலி எழுத்து *url) _pure_;
bool documentation_url_is_valid(மாறிலி எழுத்து *url) _pure_;

/* Our own "provider:[/path/to/socket]/resource" URL scheme, for acquiring resources from local Varlink services */
முழு provider_url_parse(மாறிலி எழுத்து *url, எழுத்து **ret_socket, எழுத்து **ret_resource);
bool provider_url_is_valid(மாறிலி எழுத்து *url);
bool http_header_valid(மாறிலி எழுத்து *header) _pure_;
bool http_etag_is_valid(மாறிலி எழுத்து *etag) _pure_;
