/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#અન્તર્ભાવ "forward.h"

bool http_url_is_valid(અચલ અક્ષર *url) _pure_;
bool file_url_is_valid(અચલ અક્ષર *url) _pure_;
bool documentation_url_is_valid(અચલ અક્ષર *url) _pure_;

/* Our own "provider:[/path/to/socket]/resource" URL scheme, for acquiring resources from local Varlink services */
પૂર્ણાંક provider_url_parse(અચલ અક્ષર *url, અક્ષર **ret_socket, અક્ષર **ret_resource);
bool provider_url_is_valid(અચલ અક્ષર *url);
bool http_header_valid(અચલ અક્ષર *header) _pure_;
bool http_etag_is_valid(અચલ અક્ષર *etag) _pure_;
