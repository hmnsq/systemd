/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#અન્તર્ભાવ "forward.h"

અચલ અક્ષર* errno_name_no_fallback(પૂર્ણાંક id) _const_;
પૂર્ણાંક errno_from_name(અચલ અક્ષર *name) _pure_;

સ્થિર અંતઃરેખ bool errno_is_valid(પૂર્ણાંક n) {
        પાછા n > 0 && n <= ERRNO_MAX;
}

#define ERRNO_NAME_BUF_LEN DECIMAL_STR_MAX(પૂર્ણાંક)
/* Like errno_name, but always returns a string. */
અચલ અક્ષર* errno_name(પૂર્ણાંક id, અક્ષર buf[સ્થિર ERRNO_NAME_BUF_LEN]);

/* A helper to print the errno "name" or number if name is not defined. */
#define ERRNO_NAME(errnum) errno_name(errnum, (અક્ષર[ERRNO_NAME_BUF_LEN]){})
