/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#അന്തര്ഭാവ "forward.h"

bool http_url_is_valid(സ്ഥിരം അക്ഷരം *url) _pure_;
bool file_url_is_valid(സ്ഥിരം അക്ഷരം *url) _pure_;
bool documentation_url_is_valid(സ്ഥിരം അക്ഷരം *url) _pure_;

/* Our own "provider:[/path/to/socket]/resource" URL scheme, for acquiring resources from local Varlink services */
പൂർണ്ണസംഖ്യ provider_url_parse(സ്ഥിരം അക്ഷരം *url, അക്ഷരം **ret_socket, അക്ഷരം **ret_resource);
bool provider_url_is_valid(സ്ഥിരം അക്ഷരം *url);
bool http_header_valid(സ്ഥിരം അക്ഷരം *header) _pure_;
bool http_etag_is_valid(സ്ഥിരം അക്ഷരം *etag) _pure_;
