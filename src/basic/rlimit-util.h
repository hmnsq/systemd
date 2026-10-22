/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#அந்தர்பாவ <sys/resource.h>       /* IWYU pragma: export */

#அந்தர்பாவ "forward.h"

#define _RLIMIT_MAX RLIMIT_NLIMITS

DECLARE_STRING_TABLE_LOOKUP(rlimit, முழு);
முழு rlimit_from_string_harder(மாறிலி எழுத்து *s) _pure_;
வெறுமை rlimits_list(மாறிலி எழுத்து *prefix);

முழு setrlimit_closest(முழு resource, மாறிலி கட்டமைப்பு rlimit *rlim);
முழு setrlimit_closest_all(மாறிலி கட்டமைப்பு rlimit * மாறிலி *rlim, முழு *which_failed);

முழு rlimit_parse_one(முழு resource, மாறிலி எழுத்து *val, rlim_t *ret);
முழு rlimit_parse(முழு resource, மாறிலி எழுத்து *val, கட்டமைப்பு rlimit *ret);

முழு rlimit_format(மாறிலி கட்டமைப்பு rlimit *rl, எழுத்து **ret);

முழு rlimit_copy_all(கட்டமைப்பு rlimit* target[நிலையான _RLIMIT_MAX], கட்டமைப்பு rlimit* மாறிலி source[நிலையான _RLIMIT_MAX]);
வெறுமை rlimit_free_all(கட்டமைப்பு rlimit **rl);

#define RLIMIT_MAKE_CONST(lim) ((கட்டமைப்பு rlimit) { lim, lim })

முழு rlimit_nofile_bump(முழு limit);
முழு rlimit_nofile_safe(வெறுமை);

முழு pid_getrlimit(pid_t pid, முழு resource, கட்டமைப்பு rlimit *ret);
