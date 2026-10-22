/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#અન્તર્ભાવ <sys/resource.h>       /* IWYU pragma: export */

#અન્તર્ભાવ "forward.h"

#define _RLIMIT_MAX RLIMIT_NLIMITS

DECLARE_STRING_TABLE_LOOKUP(rlimit, પૂર્ણાંક);
પૂર્ણાંક rlimit_from_string_harder(અચલ અક્ષર *s) _pure_;
શૂન્ય rlimits_list(અચલ અક્ષર *prefix);

પૂર્ણાંક setrlimit_closest(પૂર્ણાંક resource, અચલ સંરચના rlimit *rlim);
પૂર્ણાંક setrlimit_closest_all(અચલ સંરચના rlimit * અચલ *rlim, પૂર્ણાંક *which_failed);

પૂર્ણાંક rlimit_parse_one(પૂર્ણાંક resource, અચલ અક્ષર *val, rlim_t *ret);
પૂર્ણાંક rlimit_parse(પૂર્ણાંક resource, અચલ અક્ષર *val, સંરચના rlimit *ret);

પૂર્ણાંક rlimit_format(અચલ સંરચના rlimit *rl, અક્ષર **ret);

પૂર્ણાંક rlimit_copy_all(સંરચના rlimit* target[સ્થિર _RLIMIT_MAX], સંરચના rlimit* અચલ source[સ્થિર _RLIMIT_MAX]);
શૂન્ય rlimit_free_all(સંરચના rlimit **rl);

#define RLIMIT_MAKE_CONST(lim) ((સંરચના rlimit) { lim, lim })

પૂર્ણાંક rlimit_nofile_bump(પૂર્ણાંક limit);
પૂર્ણાંક rlimit_nofile_safe(શૂન્ય);

પૂર્ણાંક pid_getrlimit(pid_t pid, પૂર્ણાંક resource, સંરચના rlimit *ret);
