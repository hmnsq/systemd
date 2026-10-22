/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ضمن "forward.h"

bool http_url_is_valid(ثابت حرف *url) _pure_;
bool file_url_is_valid(ثابت حرف *url) _pure_;
bool documentation_url_is_valid(ثابت حرف *url) _pure_;

/* Our own "provider:[/path/to/socket]/resource" URL scheme, for acquiring resources from local Varlink services */
صحيح provider_url_parse(ثابت حرف *url, حرف **ret_socket, حرف **ret_resource);
bool provider_url_is_valid(ثابت حرف *url);
bool http_header_valid(ثابت حرف *header) _pure_;
bool http_etag_is_valid(ثابت حرف *etag) _pure_;
