/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ଅନ୍ତର୍ଭାଵ "forward.h"

size_t strnpcpy_full(ଅକ୍ଷର **dest, size_t size, ଅଚଳ ଅକ୍ଷର *src, size_t len, bool *ret_truncated);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ size_t strnpcpy(ଅକ୍ଷର **dest, size_t size, ଅଚଳ ଅକ୍ଷର *src, size_t len) {
        ଫେରନ୍ତୁ strnpcpy_full(dest, size, src, len, ଖାଲି);
}
size_t strpcpy_full(ଅକ୍ଷର **dest, size_t size, ଅଚଳ ଅକ୍ଷର *src, bool *ret_truncated);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ size_t strpcpy(ଅକ୍ଷର **dest, size_t size, ଅଚଳ ଅକ୍ଷର *src) {
        ଫେରନ୍ତୁ strpcpy_full(dest, size, src, ଖାଲି);
}
size_t strpcpyf_full(ଅକ୍ଷର **dest, size_t size, bool *ret_truncated, ଅଚଳ ଅକ୍ଷର *src, ...) _printf_(4, 5);
#define strpcpyf(dest, size, src, ...) \
        strpcpyf_full((dest), (size), ଖାଲି, (src), ##__VA_ARGS__)
size_t strpcpyl_full(ଅକ୍ଷର **dest, size_t size, bool *ret_truncated, ଅଚଳ ଅକ୍ଷର *src, ...) _sentinel_;
#define strpcpyl(dest, size, src, ...) \
        strpcpyl_full((dest), (size), ଖାଲି, (src), ##__VA_ARGS__)
size_t strnscpy_full(ଅକ୍ଷର *dest, size_t size, ଅଚଳ ଅକ୍ଷର *src, size_t len, bool *ret_truncated);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ size_t strnscpy(ଅକ୍ଷର *dest, size_t size, ଅଚଳ ଅକ୍ଷର *src, size_t len) {
        ଫେରନ୍ତୁ strnscpy_full(dest, size, src, len, ଖାଲି);
}
size_t strscpy_full(ଅକ୍ଷର *dest, size_t size, ଅଚଳ ଅକ୍ଷର *src, bool *ret_truncated);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ size_t strscpy(ଅକ୍ଷର *dest, size_t size, ଅଚଳ ଅକ୍ଷର *src) {
        ଫେରନ୍ତୁ strscpy_full(dest, size, src, ଖାଲି);
}
size_t strscpyl_full(ଅକ୍ଷର *dest, size_t size, bool *ret_truncated, ଅଚଳ ଅକ୍ଷର *src, ...) _sentinel_;
#define strscpyl(dest, size, src, ...) \
        strscpyl_full(dest, size, ଖାଲି, src, ##__VA_ARGS__)
