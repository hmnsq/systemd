/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ <stdio.h>

#അന്തര്ഭാവ "alloc-util.h"
#അന്തര്ഭാവ "iovec-util.h"
#അന്തര്ഭാവ "iovec-wrapper.h"
#അന്തര്ഭാവ "string-util.h"
#അന്തര്ഭാവ "unaligned.h"

ശൂന്യം iovw_done(ഘടന iovec_wrapper *iovw) {
        പ്രതിജ്ഞാ(iovw);

        iovw->iovec = mfree(iovw->iovec);
        iovw->എണ്ണം = 0;
}

ശൂന്യം iovw_done_free(ഘടന iovec_wrapper *iovw) {
        പ്രതിജ്ഞാ(iovw);

        FOREACH_ARRAY(i, iovw->iovec, iovw->എണ്ണം)
                iovec_done(i);

        iovw_done(iovw);
}

ഘടന iovec_wrapper* iovw_free(ഘടന iovec_wrapper *iovw) {
        എങ്കിൽ (!iovw)
                തിരികെ ശൂന്യ;

        iovw_done(iovw);
        തിരികെ mfree(iovw);
}

ഘടന iovec_wrapper* iovw_free_free(ഘടന iovec_wrapper *iovw) {
        എങ്കിൽ (!iovw)
                തിരികെ ശൂന്യ;

        iovw_done_free(iovw);
        തിരികെ mfree(iovw);
}

പൂർണ്ണസംഖ്യ iovw_compare(സ്ഥിരം ഘടന iovec_wrapper *a, സ്ഥിരം ഘടന iovec_wrapper *b) {
        പൂർണ്ണസംഖ്യ r;

        എങ്കിൽ (a == b)
                തിരികെ 0;

        എങ്കിൽ (!a || !b)
                തിരികെ CMP(!!a, !!b);

        /* Note, this performs structural (element-by-element) comparison, not content-based comparison.
         * Two wrappers with identical concatenated content but different element boundaries
         * (e.g., ["fo","o"] vs ["f","oo"]) will not compare as equal. */

        വേണ്ടി (size_t i = 0, n = MIN(a->എണ്ണം, b->എണ്ണം); i < n; i++) {
                r = iovec_memcmp(a->iovec + i, b->iovec + i);
                എങ്കിൽ (r != 0)
                        തിരികെ r;
        }

        തിരികെ CMP(a->എണ്ണം, b->എണ്ണം);
}

പൂർണ്ണസംഖ്യ iovw_put_full(ഘടന iovec_wrapper *iovw, bool accept_zero, ശൂന്യം *data, size_t len) {
        പ്രതിജ്ഞാ(iovw);
        പ്രതിജ്ഞാ(data || len == 0);

        എങ്കിൽ (len == 0 && !accept_zero)
                തിരികെ 0;

        എങ്കിൽ (iovw->എണ്ണം >= IOV_MAX)
                തിരികെ -E2BIG;

        എങ്കിൽ (!GREEDY_REALLOC(iovw->iovec, iovw->എണ്ണം + 1))
                തിരികെ -ENOMEM;

        iovw->iovec[iovw->എണ്ണം++] = IOVEC_MAKE(data, len);
        തിരികെ 1;
}

പൂർണ്ണസംഖ്യ iovw_put_iov_full(ഘടന iovec_wrapper *iovw, bool accept_zero, സ്ഥിരം ഘടന iovec *iov) {
        പ്രതിജ്ഞാ(iovw);

        എങ്കിൽ (!iov)
                തിരികെ 0;

        തിരികെ iovw_put_full(iovw, accept_zero, iov->iov_base, iov->iov_len);
}

പൂർണ്ണസംഖ്യ iovw_put_iovw_full(ഘടന iovec_wrapper *iovw, bool accept_zero, സ്ഥിരം ഘടന iovec_wrapper *source) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(iovw);

        എങ്കിൽ (iovw_isempty(source))
                തിരികെ 0;

        /* We will reallocate iovw->iovec, hence the source cannot point to the same object. */
        എങ്കിൽ (iovw == source)
                തിരികെ -EINVAL;

        എങ്കിൽ (iovw->എണ്ണം > SIZE_MAX - source->എണ്ണം)
                തിരികെ -E2BIG;
        എങ്കിൽ (iovw->എണ്ണം + source->എണ്ണം > IOV_MAX)
                തിരികെ -E2BIG;

        എങ്കിൽ (accept_zero) {
                എങ്കിൽ (!GREEDY_REALLOC_APPEND(iovw->iovec, iovw->എണ്ണം, source->iovec, source->എണ്ണം))
                        തിരികെ -ENOMEM;

                തിരികെ 0;
        }

        /* When accept_zero is false, we need to filter zero length iovec in source. */
        size_t original_എണ്ണം = iovw->എണ്ണം;

        FOREACH_ARRAY(iovec, source->iovec, source->എണ്ണം) {
                r = iovw_put_iov_full(iovw, accept_zero, iovec);
                എങ്കിൽ (r < 0)
                        പോകുക rollback;
        }

        തിരികെ 0;

rollback:
        iovw->എണ്ണം = original_എണ്ണം;
        തിരികെ r;
}

പൂർണ്ണസംഖ്യ iovw_consume_full(ഘടന iovec_wrapper *iovw, bool accept_zero, ശൂന്യം *data, size_t len) {
        /* Move data into iovw or free on error */
        പൂർണ്ണസംഖ്യ r;

        r = iovw_put_full(iovw, accept_zero, data, len);
        എങ്കിൽ (r <= 0)
                സ്വതന്ത്രം(data);

        തിരികെ r;
}

പൂർണ്ണസംഖ്യ iovw_consume_iov_full(ഘടന iovec_wrapper *iovw, bool accept_zero, ഘടന iovec *iov) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(iovw);

        എങ്കിൽ (!iov)
                തിരികെ 0;

        r = iovw_put_iov_full(iovw, accept_zero, iov);
        എങ്കിൽ (r <= 0)
                iovec_done(iov);
        അല്ലെങ്കിൽ
                /* On success, iov->iov_base is now owned by iovw. Let's emptify iov, but do not call
                 * iovec_done(), of course. */
                *iov = (ഘടന iovec) {};

        തിരികെ r;
}

പൂർണ്ണസംഖ്യ iovw_extend_full(ഘടന iovec_wrapper *iovw, bool accept_zero, സ്ഥിരം ശൂന്യം *data, size_t len) {
        പ്രതിജ്ഞാ(iovw);
        പ്രതിജ്ഞാ(data || len == 0);

        എങ്കിൽ (len == 0)
                തിരികെ iovw_put_full(iovw, accept_zero, /* data= */ ശൂന്യ, /* len= */ 0);

        ശൂന്യം *c = memdup(data, len);
        എങ്കിൽ (!c)
                തിരികെ -ENOMEM;

        തിരികെ iovw_consume_full(iovw, accept_zero, c, len);
}

പൂർണ്ണസംഖ്യ iovw_extend_iov_full(ഘടന iovec_wrapper *iovw, bool accept_zero, സ്ഥിരം ഘടന iovec *iov) {
        പ്രതിജ്ഞാ(iovw);

        എങ്കിൽ (!iov)
                തിരികെ 0;

        തിരികെ iovw_extend_full(iovw, accept_zero, iov->iov_base, iov->iov_len);
}

പൂർണ്ണസംഖ്യ iovw_extend_iovw_full(ഘടന iovec_wrapper *iovw, bool accept_zero, സ്ഥിരം ഘടന iovec_wrapper *source) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(iovw);

        /* This duplicates the source and merges it into the iovw. */

        എങ്കിൽ (iovw_isempty(source))
                തിരികെ 0;

        /* iovw->iovec will be reallocated in the loop below, hence source cannot point to the same object. */
        എങ്കിൽ (iovw == source)
                തിരികെ -EINVAL;

        എങ്കിൽ (iovw->എണ്ണം > SIZE_MAX - source->എണ്ണം)
                തിരികെ -E2BIG;
        എങ്കിൽ (iovw->എണ്ണം + source->എണ്ണം > IOV_MAX)
                തിരികെ -E2BIG;

        size_t original_എണ്ണം = iovw->എണ്ണം;

        FOREACH_ARRAY(iovec, source->iovec, source->എണ്ണം) {
                r = iovw_extend_iov_full(iovw, accept_zero, iovec);
                എങ്കിൽ (r < 0)
                        പോകുക rollback;
        }

        തിരികെ 0;

rollback:
        വേണ്ടി (size_t i = original_എണ്ണം; i < iovw->എണ്ണം; i++)
                iovec_done(iovw->iovec + i);

        iovw->എണ്ണം = original_എണ്ണം;
        തിരികെ r;
}

പൂർണ്ണസംഖ്യ iovw_put_string_field_full(ഘടന iovec_wrapper *iovw, bool replace, സ്ഥിരം അക്ഷരം *field, സ്ഥിരം അക്ഷരം *മൂല്യം) {
        _cleanup_free_ അക്ഷരം *x = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(iovw);

        x = strjoin(field, മൂല്യം);
        എങ്കിൽ (!x)
                തിരികെ -ENOMEM;

        എങ്കിൽ (replace)
                FOREACH_ARRAY(iovec, iovw->iovec, iovw->എണ്ണം)
                        എങ്കിൽ (memory_startswith(iovec->iov_base, iovec->iov_len, field)) {
                                iovec->iov_len = strlen(x);
                                തിരികെ free_and_replace(iovec->iov_base, x);
                        }

        r = iovw_put(iovw, x, strlen(x));
        എങ്കിൽ (r >= 0)
                TAKE_PTR(x);

        തിരികെ r;
}

പൂർണ്ണസംഖ്യ iovw_put_string_fieldf_full(ഘടന iovec_wrapper *iovw, bool replace, സ്ഥിരം അക്ഷരം *field, സ്ഥിരം അക്ഷരം *format, ...) {
        _cleanup_free_ അക്ഷരം *മൂല്യം = ശൂന്യ;
        va_list ap;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(format);

        va_start(ap, format);
        r = vasprintf(&മൂല്യം, format, ap);
        va_end(ap);
        എങ്കിൽ (r < 0)
                തിരികെ -ENOMEM;

        തിരികെ iovw_put_string_field_full(iovw, replace, field, മൂല്യം);
}

പൂർണ്ണസംഖ്യ iovw_put_string_field_free(ഘടന iovec_wrapper *iovw, സ്ഥിരം അക്ഷരം *field, അക്ഷരം *മൂല്യം) {
        _cleanup_free_ _unused_ അക്ഷരം *free_ptr = മൂല്യം;

        തിരികെ iovw_put_string_field(iovw, field, മൂല്യം);
}

ശൂന്യം iovw_rebase(ഘടന iovec_wrapper *iovw, ശൂന്യം *old, ശൂന്യം *new) {
        പ്രതിജ്ഞാ(iovw);

        FOREACH_ARRAY(i, iovw->iovec, iovw->എണ്ണം) {
                പ്രതിജ്ഞാ(i->iov_base >= old);
                i->iov_base = (uint8_t*) i->iov_base - (uint8_t*) old + (uint8_t*) new;
        }
}

size_t iovw_size(സ്ഥിരം ഘടന iovec_wrapper *iovw) {
        എങ്കിൽ (iovw_isempty(iovw))
                തിരികെ 0;

        തിരികെ iovec_total_size(iovw->iovec, iovw->എണ്ണം);
}

പൂർണ്ണസംഖ്യ iovw_concat(സ്ഥിരം ഘടന iovec_wrapper *iovw, ഘടന iovec *ret) {
        പ്രതിജ്ഞാ(iovw);
        പ്രതിജ്ഞാ(ret);

        /* Squish a series of iovecs into a single iovec. */

        size_t len = iovw_size(iovw);
        എങ്കിൽ (len == SIZE_MAX)
                തിരികെ -E2BIG;  /* Prevent theoretical overflow */

        /* Always allocate one more byte to make the result usable as a NUL-terminated string. */
        _cleanup_free_ uint8_t *buf = malloc(len + 1);
        എങ്കിൽ (!buf)
                തിരികെ -ENOMEM;

        uint8_t *p = buf;
        FOREACH_ARRAY(i, iovw->iovec, iovw->എണ്ണം)
                p = mempcpy_safe(p, i->iov_base, i->iov_len);

        *p = 0;

        *ret = IOVEC_MAKE(TAKE_PTR(buf), len);
        തിരികെ 0;
}

അക്ഷരം* iovw_to_cstring(സ്ഥിരം ഘടന iovec_wrapper *iovw) {
        പ്രതിജ്ഞാ(iovw);

        /* Squish a series of iovecs into a C string. Embedded NULs are not allowed.
         * The caller is expected to filter them out when populating the data. */

        _cleanup_(iovec_done) ഘടന iovec iov = {};
        എങ്കിൽ (iovw_concat(iovw, &iov) < 0)
                തിരികെ ശൂന്യ;

        പ്രതിജ്ഞാ(!memchr(iov.iov_base, 0, iov.iov_len));
        തിരികെ TAKE_PTR(iov.iov_base);
}

പൂർണ്ണസംഖ്യ iovec_split(സ്ഥിരം ഘടന iovec *iov, size_t length_size, ഘടന iovec_wrapper *ret) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(IN_SET(length_size, 1, 2, 4));
        പ്രതിജ്ഞാ(ret);

        /* This parses the input iovec as length-prefixed data, and stores the result as iovec_wrapper.
         * Note, zero-length entries are silently dropped. */

        എങ്കിൽ (!iovec_is_set(iov)) {
                *ret = (ഘടന iovec_wrapper) {};
                തിരികെ 0;
        }

        _cleanup_(iovw_done_free) ഘടന iovec_wrapper iovw = {};
        വേണ്ടി (ഘടന iovec i = *iov; iovec_is_set(&i); ) {
                എങ്കിൽ (i.iov_len < length_size)
                        തിരികെ -EBADMSG;

                size_t len;
                തിരഞ്ഞെടുപ്പ് (length_size) {
                അവസ്ഥ 1:
                        len = *(uint8_t*) i.iov_base;
                        നിർത്തുക;
                അവസ്ഥ 2:
                        len = unaligned_read_be16(i.iov_base);
                        നിർത്തുക;
                അവസ്ഥ 4:
                        len = unaligned_read_be32(i.iov_base);
                        നിർത്തുക;
                സ്ഥിരസ്ഥിതി:
                        assert_not_reached();
                }

                iovec_inc(&i, length_size);

                എങ്കിൽ (len == 0)
                        തുടരുക;

                എങ്കിൽ (i.iov_len < len)
                        തിരികെ -EBADMSG;

                r = iovw_extend(&iovw, i.iov_base, len);
                എങ്കിൽ (r < 0)
                        തിരികെ r;

                iovec_inc(&i, len);
        }

        *ret = TAKE_STRUCT(iovw);
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ iovw_merge(സ്ഥിരം ഘടന iovec_wrapper *iovw, size_t length_size, ഘടന iovec *ret) {
        പ്രതിജ്ഞാ(IN_SET(length_size, 1, 2, 4));
        പ്രതിജ്ഞാ(ret);

        /* This is the inverse of iovec_split(), and builds a length-prefixed data from iovec_wrapper.
         * Note, zero-length entries are silently dropped. */

        size_t sz = iovw_size(iovw);
        എങ്കിൽ (sz == 0) {
                *ret = (ഘടന iovec) {};
                തിരികെ 0;
        }
        എങ്കിൽ (sz == SIZE_MAX)
                തിരികെ -E2BIG;

        എങ്കിൽ (size_multiply_overflow(length_size, iovw->എണ്ണം))
                തിരികെ -E2BIG;

        sz = size_add(sz, iovw->എണ്ണം * length_size);
        എങ്കിൽ (sz == SIZE_MAX)
                തിരികെ -E2BIG;

        _cleanup_free_ uint8_t *buf = new(uint8_t, sz);
        എങ്കിൽ (!buf)
                തിരികെ -ENOMEM;

        uint8_t *p = buf;
        FOREACH_ARRAY(iov, iovw->iovec, iovw->എണ്ണം) {
                എങ്കിൽ (iov->iov_len == 0)
                        തുടരുക;

                തിരഞ്ഞെടുപ്പ് (length_size) {
                അവസ്ഥ 1:
                        എങ്കിൽ (iov->iov_len > UINT8_MAX)
                                തിരികെ -ERANGE;

                        *p = iov->iov_len;
                        നിർത്തുക;
                അവസ്ഥ 2:
                        എങ്കിൽ (iov->iov_len > UINT16_MAX)
                                തിരികെ -ERANGE;

                        unaligned_write_be16(p, iov->iov_len);
                        നിർത്തുക;
                അവസ്ഥ 4:
                        എങ്കിൽ (iov->iov_len > UINT32_MAX)
                                തിരികെ -ERANGE;

                        unaligned_write_be32(p, iov->iov_len);
                        നിർത്തുക;
                സ്ഥിരസ്ഥിതി:
                        assert_not_reached();
                }
                p += length_size;

                p = mempcpy(p, iov->iov_base, iov->iov_len);
        }

        പ്രതിജ്ഞാ(sz >= (size_t) (p - buf));
        sz = p - buf;

        *ret = IOVEC_MAKE(TAKE_PTR(buf), sz);
        തിരികെ 0;
}
