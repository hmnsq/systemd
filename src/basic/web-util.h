/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ଅନ୍ତର୍ଭାଵ "forward.h"

bool http_url_is_valid(ଅଚଳ ଅକ୍ଷର *url) _pure_;
bool file_url_is_valid(ଅଚଳ ଅକ୍ଷର *url) _pure_;
bool documentation_url_is_valid(ଅଚଳ ଅକ୍ଷର *url) _pure_;

/* Our own "provider:[/path/to/socket]/resource" URL scheme, for acquiring resources from local Varlink services */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା provider_url_parse(ଅଚଳ ଅକ୍ଷର *url, ଅକ୍ଷର **ret_socket, ଅକ୍ଷର **ret_resource);
bool provider_url_is_valid(ଅଚଳ ଅକ୍ଷର *url);
bool http_header_valid(ଅଚଳ ଅକ୍ଷର *header) _pure_;
bool http_etag_is_valid(ଅଚଳ ଅକ୍ଷର *etag) _pure_;
