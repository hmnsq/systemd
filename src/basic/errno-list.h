/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ଅନ୍ତର୍ଭାଵ "forward.h"

ଅଚଳ ଅକ୍ଷର* errno_name_no_fallback(ପୂର୍ଣ୍ଣସଂଖ୍ୟା id) _const_;
ପୂର୍ଣ୍ଣସଂଖ୍ୟା errno_from_name(ଅଚଳ ଅକ୍ଷର *name) _pure_;

ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool errno_is_valid(ପୂର୍ଣ୍ଣସଂଖ୍ୟା n) {
        ଫେରନ୍ତୁ n > 0 && n <= ERRNO_MAX;
}

#define ERRNO_NAME_BUF_LEN DECIMAL_STR_MAX(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)
/* Like errno_name, but always returns a string. */
ଅଚଳ ଅକ୍ଷର* errno_name(ପୂର୍ଣ୍ଣସଂଖ୍ୟା id, ଅକ୍ଷର buf[ସ୍ଥିର ERRNO_NAME_BUF_LEN]);

/* A helper to print the errno "name" or number if name is not defined. */
#define ERRNO_NAME(errnum) errno_name(errnum, (ଅକ୍ଷର[ERRNO_NAME_BUF_LEN]){})
