/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#અન્તર્ભાવ "forward.h"

size_t strnpcpy_full(અક્ષર **dest, size_t size, અચલ અક્ષર *src, size_t len, bool *ret_truncated);
સ્થિર અંતઃરેખ size_t strnpcpy(અક્ષર **dest, size_t size, અચલ અક્ષર *src, size_t len) {
        પાછા strnpcpy_full(dest, size, src, len, ખાલી);
}
size_t strpcpy_full(અક્ષર **dest, size_t size, અચલ અક્ષર *src, bool *ret_truncated);
સ્થિર અંતઃરેખ size_t strpcpy(અક્ષર **dest, size_t size, અચલ અક્ષર *src) {
        પાછા strpcpy_full(dest, size, src, ખાલી);
}
size_t strpcpyf_full(અક્ષર **dest, size_t size, bool *ret_truncated, અચલ અક્ષર *src, ...) _printf_(4, 5);
#define strpcpyf(dest, size, src, ...) \
        strpcpyf_full((dest), (size), ખાલી, (src), ##__VA_ARGS__)
size_t strpcpyl_full(અક્ષર **dest, size_t size, bool *ret_truncated, અચલ અક્ષર *src, ...) _sentinel_;
#define strpcpyl(dest, size, src, ...) \
        strpcpyl_full((dest), (size), ખાલી, (src), ##__VA_ARGS__)
size_t strnscpy_full(અક્ષર *dest, size_t size, અચલ અક્ષર *src, size_t len, bool *ret_truncated);
સ્થિર અંતઃરેખ size_t strnscpy(અક્ષર *dest, size_t size, અચલ અક્ષર *src, size_t len) {
        પાછા strnscpy_full(dest, size, src, len, ખાલી);
}
size_t strscpy_full(અક્ષર *dest, size_t size, અચલ અક્ષર *src, bool *ret_truncated);
સ્થિર અંતઃરેખ size_t strscpy(અક્ષર *dest, size_t size, અચલ અક્ષર *src) {
        પાછા strscpy_full(dest, size, src, ખાલી);
}
size_t strscpyl_full(અક્ષર *dest, size_t size, bool *ret_truncated, અચલ અક્ષર *src, ...) _sentinel_;
#define strscpyl(dest, size, src, ...) \
        strscpyl_full(dest, size, ખાલી, src, ##__VA_ARGS__)
