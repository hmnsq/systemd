/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ "forward.h"

bool http_url_is_valid(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *url) ಶುದ್ಧ;
bool file_url_is_valid(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *url) ಶುದ್ಧ;
bool documentation_url_is_valid(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *url) ಶುದ್ಧ;

/* Our own "provider:[/path/to/socket]/resource" URL scheme, for acquiring resources from local Varlink services */
ಪೂರ್ಣಾಂಕ provider_url_parse(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *url, ಅಕ್ಷರ **ret_socket, ಅಕ್ಷರ **ret_resource);
bool provider_url_is_valid(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *url);
bool http_ಶೀರ್ಷಿಕೆ_valid(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಶೀರ್ಷಿಕೆ) ಶುದ್ಧ;
bool http_etag_is_valid(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *etag) ಶುದ್ಧ;
