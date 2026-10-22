/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

bool http_url_is_valid(const var_na *url) _pure_;
bool file_url_is_valid(const var_na *url) _pure_;
bool documentation_url_is_valid(const var_na *url) _pure_;

/* Our own "provider:[/path/to/socket]/resource" URL scheme, for acquiring resources from local Varlink services */
ຈຳນວນເຕັມ provider_url_parse(const var_na *url, var_na **ret_socket, var_na **ret_resource);
bool provider_url_is_valid(const var_na *url);
bool http_header_valid(const var_na *header) _pure_;
bool http_etag_is_valid(const var_na *etag) _pure_;
