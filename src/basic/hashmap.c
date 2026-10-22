/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <fnmatch.h>
#ضمن <pthread.h>
#ضمن <unistd.h>
#إذا HAVE_VALGRIND_VALGRIND_H
#  include <valgrind/valgrind.h>
#پایان_اگر

#ضمن "alloc-util.h"
#ضمن "extract-word.h"
#ضمن "hashmap.h"
#ضمن "log.h"
#ضمن "logarithm.h"
#ضمن "memory-util.h"
#ضمن "mempool.h"
#ضمن "process-util.h"
#ضمن "random-util.h"
#ضمن "set.h"
#ضمن "siphash24.h"
#ضمن "sort-util.h"
#ضمن "string-util.h"
#ضمن "strv.h"

#إذا ENABLE_DEBUG_HASHMAP
#ضمن "list.h"
#پایان_اگر

/*
 * Implementation of hashmaps.
 * Addressing: open
 *   - uses less RAM compared to closed addressing (chaining), because
 *     our entries are small (especially in Sets, which tend to contain
 *     the majority of entries in systemd).
 * Collision resolution: Robin Hood
 *   - tends to equalize displacement of entries from their optimal buckets.
 * Probe sequence: linear
 *   - though theoretically worse than random probing/uniform hashing/double
 *     hashing, it is good for cache locality.
 *
 * References:
 * Celis, P. 1986. Robin Hood Hashing.
 * Ph.D. Dissertation. University of Waterloo, Waterloo, Ont., Canada, Canada.
 * https://cs.uwaterloo.ca/research/tr/1986/CS-86-14.pdf
 * - The results are derived for random probing. Suggests deletion with
 *   tombstones and two mean-centered search methods. None of that works
 *   well for linear probing.
 *
 * Janson, S. 2005. Individual displacements for linear probing hashing with different insertion policies.
 * ACM Trans. Algorithms 1, 2 (October 2005), 177-213.
 * DOI=10.1145/1103963.1103964 http://doi.acm.org/10.1145/1103963.1103964
 * http://www.math.uu.se/~svante/papers/sj157.pdf
 * - Applies to Robin Hood with linear probing. Contains remarks on
 *   the unsuitability of mean-centered search with linear probing.
 *
 * Viola, A. 2005. Exact distribution of individual displacements in linear probing hashing.
 * ACM Trans. Algorithms 1, 2 (October 2005), 214-242.
 * DOI=10.1145/1103963.1103965 http://doi.acm.org/10.1145/1103963.1103965
 * - Similar to Janson. Note that Viola writes about C_{m,n} (number of probes
 *   in a successful search), and Janson writes about displacement. C = d + 1.
 *
 * Goossaert, E. 2013. Robin Hood hashing: backward shift deletion.
 * http://codecapsule.com/2013/11/17/robin-hood-hashing-backward-shift-deletion/
 * - Explanation of backward shift deletion with pictures.
 *
 * Khuong, P. 2013. The Other Robin Hood Hashing.
 * http://www.pvk.ca/Blog/2013/11/26/the-other-robin-hood-hashing/
 * - Short summary of random vs. linear probing, and tombstones vs. backward shift.
 */

/*
 * XXX Ideas for improvement:
 * For unordered hashmaps, randomize iteration order, similarly to Perl:
 * http://blog.booking.com/hardening-perls-hash-function.html
 */

/* INV_KEEP_FREE = 1 / (1 - max_load_factor)
 * e.g. 1 / (1 - 0.8) = 5 ... keep one fifth of the buckets free. */
#تعریف_کن INV_KEEP_FREE            5U

/* Fields common to entries of all hashmap/set types */
بنية hashmap_پایه_مدخل {
        ثابت فراغ *کلید;
};

/* Entry types for specific hashmap/set types
 * hashmap_base_entry must be at the beginning of each entry struct. */

بنية plain_hashmap_مدخل {
        بنية hashmap_پایه_مدخل b;
        فراغ *مقدار;
};

بنية ordered_hashmap_مدخل {
        بنية plain_hashmap_مدخل p;
        بیعلامت iterate_بعدی, iterate_قبلی;
};

بنية مجموعه_مدخل {
        بنية hashmap_پایه_مدخل b;
};

/* In several functions it is advantageous to have the hash table extended
 * virtually by a couple of additional buckets. We reserve special index values
 * for these "swap" buckets. */
#تعریف_کن IDX_جابجا_BEGIN     (UINT_MAX - 3)
#تعریف_کن IDX_PUT             (IDX_جابجا_BEGIN + 0)
#تعریف_کن IDX_TMP             (IDX_جابجا_BEGIN + 1)
#تعریف_کن IDX_جابجا_انتها       (IDX_جابجا_BEGIN + 2)

#تعریف_کن IDX_اولی           (UINT_MAX - 1) /* special index for freshly initialized iterators */
#تعریف_کن IDX_NIL             UINT_MAX       /* special index value meaning "none" or "end" */

assert_cc(IDX_اولی == IDX_جابجا_انتها);
assert_cc(IDX_اولی == IDX_ITERATOR_اولی);

/* Storage space for the "swap" buckets.
 * All entry types can fit into an ordered_hashmap_entry. */
بنية جابجا_entries {
        بنية ordered_hashmap_مدخل e[IDX_جابجا_انتها - IDX_جابجا_BEGIN];
};

/* Distance from Initial Bucket */
تعريفنوع uint8_t dib_raw_t;
#تعریف_کن DIB_RAW_سرریز ((dib_raw_t)0xfdU)   /* indicates DIB value is greater than representable */
#تعریف_کن DIB_RAW_REHASH   ((dib_raw_t)0xfeU)   /* entry yet to be rehashed during in-place resize */
#تعریف_کن DIB_RAW_FREE     ((dib_raw_t)0xffU)   /* a free bucket */
#تعریف_کن DIB_RAW_INIT     ((حرف)DIB_RAW_FREE) /* a byte to memset a DIB store with when initializing */

#تعریف_کن DIB_FREE UINT_MAX

#إذا ENABLE_DEBUG_HASHMAP
بنية hashmap_debug_info {
        LIST_FIELDS(بنية hashmap_debug_info, debug_list);
        بیعلامت max_entries;  /* high watermark of n_entries */

        /* fields to detect modification while iterating */
        بیعلامت put_شمار;    /* counts puts into the hashmap */
        بیعلامت rem_شمار;    /* counts removals from hashmap */
        بیعلامت آخرین_rem_idx; /* remembers last removal index */
};

/* Tracks all existing hashmaps. Get at it from gdb. See sd_dump_hashmaps.py */
ساكن LIST_سر(بنية hashmap_debug_info, hashmap_debug_list);
ساكن pthread_mutex_t hashmap_debug_list_mutex = PTHREAD_MUTEX_INITIALIZER;
#پایان_اگر

تعداد HashmapType {
        HASHMAP_TYPE_PLAIN,
        HASHMAP_TYPE_ORDERED,
        HASHMAP_TYPE_مجموعه,
        _HASHMAP_TYPE_MAX
};

بنية _packed_ غیرمستقیم_storage {
        فراغ *storage;                     /* where buckets and DIBs are stored */
        uint8_t  hash_کلید[HASH_کلید_اندازه];  /* hash key; changes during resize */

        بیعلامت n_entries;                /* number of stored entries */
        بیعلامت n_buckets;                /* number of buckets */

        بیعلامت idx_پایینترین_مدخل;         /* Index below which all buckets are free.
                                              Makes "while (hashmap_steal_first())" loops
                                              O(n) instead of O(n^2) for unordered hashmaps. */
        uint8_t  _pad[3];                  /* padding for the whole HashmapBase */
        /* The bitfields in HashmapBase complete the alignment of the whole thing. */
};

بنية مستقیم_storage {
        /* This gives us 39 bytes on 64-bit, or 35 bytes on 32-bit.
         * That's room for 4 set_entries + 4 DIB bytes + 3 unused bytes on 64-bit,
         *              or 7 set_entries + 7 DIB bytes + 0 unused bytes on 32-bit. */
        uint8_t storage[حجم(بنية غیرمستقیم_storage)];
};

#تعریف_کن مستقیم_BUCKETS(مدخل_t) \
        (حجم(بنية مستقیم_storage) / (حجم(مدخل_t) + حجم(dib_raw_t)))

/* We should be able to store at least one entry directly. */
assert_cc(مستقیم_BUCKETS(بنية ordered_hashmap_مدخل) >= 1);

/* We have 3 bits for n_direct_entries. */
assert_cc(مستقیم_BUCKETS(بنية مجموعه_مدخل) < (1 << 3));

/* Hashmaps with directly stored entries all use this shared hash key.
 * It's no big deal if the key is guessed, because there can be only
 * a handful of directly stored entries in a hashmap. When a hashmap
 * outgrows direct storage, it gets its own key for indirect storage. */
ساكن uint8_t shared_hash_کلید[HASH_کلید_اندازه];

/* Fields that all hashmap/set types must have */
بنية Hashmap_پایه {
        ثابت بنية hash_ops *hash_ops;  /* hash and compare ops to use */

        اتحاد _packed_ {
                بنية غیرمستقیم_storage غیرمستقیم; /* if  has_indirect */
                بنية مستقیم_storage مستقیم;     /* if !has_indirect */
        };

        تعداد HashmapType type:2;     /* HASHMAP_TYPE_* */
        bool has_غیرمستقیم:1;         /* whether indirect storage is used */
        بیعلامت n_مستقیم_entries:3; /* Number of entries in direct storage.
                                      * Only valid if !has_indirect. */
        bool from_pool:1;            /* whether was allocated from mempool */
        bool dirty:1;                /* whether dirtied since last iterated_cache_get() */
        bool cached:1;               /* whether this hashmap is being cached */

#إذا ENABLE_DEBUG_HASHMAP
        بنية hashmap_debug_info debug;
#پایان_اگر
};

/* Specific hash types
 * HashmapBase must be at the beginning of each hashmap struct. */

بنية Hashmap {
        بنية Hashmap_پایه b;
};

بنية OrderedHashmap {
        بنية Hashmap_پایه b;
        بیعلامت iterate_list_سر, iterate_list_دم;
};

بنية مجموعه_2 {
        بنية Hashmap_پایه b;
};

تعريفنوع بنية CacheMem {
        ثابت فراغ **ptr;
        size_t n_populated;
        bool فعال:1;
} CacheMem;

بنية IteratedCache {
        Hashmap_پایه *hashmap;
        CacheMem کلیدها, values;
};

DEFINE_MEMPOOL(hashmap_pool,         Hashmap,        8);
DEFINE_MEMPOOL(ordered_hashmap_pool, OrderedHashmap, 8);
/* No need for a separate Set pool */
assert_cc(حجم(Hashmap) == حجم(مجموعه_2));

بنية hashmap_type_info {
        size_t سر_اندازه;
        size_t مدخل_اندازه;
        بنية mempool *mempool;
        بیعلامت n_مستقیم_buckets;
};

ساكن استفاده_شده_2 ثابت بنية hashmap_type_info hashmap_type_info[_HASHMAP_TYPE_MAX] = {
        [HASHMAP_TYPE_PLAIN] = {
                .سر_اندازه        = حجم(Hashmap),
                .مدخل_اندازه       = حجم(بنية plain_hashmap_مدخل),
                .mempool          = &hashmap_pool,
                .n_مستقیم_buckets = مستقیم_BUCKETS(بنية plain_hashmap_مدخل),
        },
        [HASHMAP_TYPE_ORDERED] = {
                .سر_اندازه        = حجم(OrderedHashmap),
                .مدخل_اندازه       = حجم(بنية ordered_hashmap_مدخل),
                .mempool          = &ordered_hashmap_pool,
                .n_مستقیم_buckets = مستقیم_BUCKETS(بنية ordered_hashmap_مدخل),
        },
        [HASHMAP_TYPE_مجموعه] = {
                .سر_اندازه        = حجم(مجموعه_2),
                .مدخل_اندازه       = حجم(بنية مجموعه_مدخل),
                .mempool          = &hashmap_pool,
                .n_مستقیم_buckets = مستقیم_BUCKETS(بنية مجموعه_مدخل),
        },
};

فراغ hashmap_trim_pools(فراغ) {
        صحيح r;

        /* The pool is only allocated by the main thread, but the memory can be passed to other
         * threads. Let's clean up if we are the main thread and no other threads are live. */

        /* We build our own is_main_thread() here, which doesn't use C11 TLS based caching of the
         * result. That's because valgrind apparently doesn't like TLS to be used from a GCC destructor. */
        إذا (getpid() != gettid())
                أرجع (فراغ) log_debug("Not cleaning up memory pools, not in main thread.");

        r = get_process_نخها(0);
        إذا (r < 0)
                أرجع (فراغ) log_debug_errno(r, "Failed to determine number of threads, not cleaning up memory pools: %m");
        إذا (r != 1)
                أرجع (فراغ) log_debug("Not cleaning up memory pools, running in multi-threaded process.");

        mempool_trim(&hashmap_pool);
        mempool_trim(&ordered_hashmap_pool);
}

#إذا HAVE_VALGRIND_VALGRIND_H
_destructor_ ساكن فراغ cleanup_pools(فراغ) {
        /* Be nice to valgrind */
        إذا (RUNNING_ON_VALGRIND)
                hashmap_trim_pools();
}
#پایان_اگر

ساكن مضمن بیعلامت n_buckets(Hashmap_پایه *h) {
        أكد(h);
        أرجع h->has_غیرمستقیم ? h->غیرمستقیم.n_buckets
                               : hashmap_type_info[h->type].n_مستقیم_buckets;
}

ساكن بیعلامت n_entries(Hashmap_پایه *h) {
        أكد(h);
        أرجع h->has_غیرمستقیم ? h->غیرمستقیم.n_entries
                               : h->n_مستقیم_entries;
}

ساكن فراغ n_entries_inc(Hashmap_پایه *h) {
        أكد(h);

        إذا (h->has_غیرمستقیم)
                h->غیرمستقیم.n_entries++;
        وإلا
                h->n_مستقیم_entries++;
}

ساكن فراغ n_entries_dec(Hashmap_پایه *h) {
        أكد(h);

        إذا (h->has_غیرمستقیم)
                h->غیرمستقیم.n_entries--;
        وإلا
                h->n_مستقیم_entries--;
}

ساكن مضمن فراغ* storage_ptr(Hashmap_پایه *h) {
        أكد(h);
        أرجع h->has_غیرمستقیم ? h->غیرمستقیم.storage
                               : h->مستقیم.storage;
}

ساكن مضمن uint8_t* hash_کلید(Hashmap_پایه *h) {
        أكد(h);
        أرجع h->has_غیرمستقیم ? h->غیرمستقیم.hash_کلید
                               : shared_hash_کلید;
}

ساكن بیعلامت پایه_سطل_hash(Hashmap_پایه *h, ثابت فراغ *p) {
        بنية siphash state;
        uint64_t hash;

        أكد(h);

        /* The keys of trivial_hash_ops and uint64_hash_ops fit in 64 bits. siphash24_uint64() hashes such
         * a key without the indirect call through hash_ops and keeps the SipHash state in registers. On
         * 32-bit and big-endian architectures, the result differs from the result of trivial_hash_func(),
         * which hashes the key in native byte order. Only this function maps keys to buckets, so the
         * difference does not matter. */
        إذا (h->hash_ops->hash == trivial_hash_func)
                hash = siphash24_uint64((uintptr_t) p, hash_کلید(h));
        وإلا إذا (h->hash_ops->hash == (hash_func_t) uint64_hash_func)
                hash = siphash24_uint64(*(ثابت uint64_t*) p, hash_کلید(h));
        وإلا {
                siphash24_init(&state, hash_کلید(h));
                h->hash_ops->hash(p, &state);
                hash = siphash24_finalize(&state);
        }

        /* n_buckets is not a power of two, so a modulo needs a division. Multiplying the upper 32 bits of
         * the hash with n_buckets and keeping the upper 32 bits of the product maps the hash to
         * [0, n_buckets) almost as evenly, without a division. */
        أرجع (بیعلامت) (((hash >> 32) * n_buckets(h)) >> 32);
}
#تعریف_کن سطل_hash(h, p) پایه_سطل_hash(HASHMAP_پایه(h), p)

ساكن فراغ پایه_مجموعه_dirty(Hashmap_پایه *h) {
        أكد(h);

        h->dirty = true;
}
#تعریف_کن hashmap_مجموعه_dirty(h) پایه_مجموعه_dirty(HASHMAP_پایه(h))

ساكن فراغ get_hash_کلید(uint8_t hash_کلید[HASH_کلید_اندازه], bool reuse_is_ok) {
        ساكن uint8_t current[HASH_کلید_اندازه];
        ساكن bool current_initialized = false;

        /* Returns a hash function key to use. In order to keep things
         * fast we will not generate a new key each time we allocate a
         * new hash table. Instead, we'll just reuse the most recently
         * generated one, except if we never generated one or when we
         * are rehashing an entire hash table because we reached a
         * fill level */

        إذا (!current_initialized || !reuse_is_ok) {
                random_بایتها(current, حجم(current));
                current_initialized = true;
        }

        کپی_حافظه(hash_کلید, current, حجم(current));
}

ساكن مضمن بنية hashmap_پایه_مدخل* سطل_at(Hashmap_پایه *h, بیعلامت idx) {
        أكد(h);
        أرجع CAST_ALIGN_PTR(
                        بنية hashmap_پایه_مدخل,
                        (uint8_t *) storage_ptr(h) + idx * hashmap_type_info[h->type].مدخل_اندازه);
}

ساكن بنية plain_hashmap_مدخل* plain_سطل_at(Hashmap *h, بیعلامت idx) {
        أرجع (بنية plain_hashmap_مدخل*) سطل_at(HASHMAP_پایه(h), idx);
}

ساكن بنية ordered_hashmap_مدخل* ordered_سطل_at(OrderedHashmap *h, بیعلامت idx) {
        أرجع (بنية ordered_hashmap_مدخل*) سطل_at(HASHMAP_پایه(h), idx);
}

ساكن بنية مجموعه_مدخل *مجموعه_سطل_at(مجموعه_2 *h, بیعلامت idx) {
        أرجع (بنية مجموعه_مدخل*) سطل_at(HASHMAP_پایه(h), idx);
}

ساكن بنية ordered_hashmap_مدخل* سطل_at_جابجا(بنية جابجا_entries *جابجا, بیعلامت idx) {
        أكد(جابجا);
        أرجع &جابجا->e[idx - IDX_جابجا_BEGIN];
}

/* Returns a pointer to the bucket at index idx.
 * Understands real indexes and swap indexes, hence "_virtual". */
ساكن بنية hashmap_پایه_مدخل* سطل_at_virtual(Hashmap_پایه *h, بنية جابجا_entries *جابجا,
                                                    بیعلامت idx) {
        إذا (idx < IDX_جابجا_BEGIN)
                أرجع سطل_at(h, idx);

        إذا (idx < IDX_جابجا_انتها)
                أرجع &سطل_at_جابجا(جابجا, idx)->p.b;

        assert_not_reached();
}

ساكن مضمن dib_raw_t* dib_raw_ptr(Hashmap_پایه *h) {
        أكد(h);
        أرجع (dib_raw_t*)
                ((uint8_t*) storage_ptr(h) + hashmap_type_info[h->type].مدخل_اندازه * n_buckets(h));
}

ساكن بیعلامت سطل_فاصله(Hashmap_پایه *h, بیعلامت idx, بیعلامت from) {
        أرجع idx >= from ? idx - from
                           : n_buckets(h) + idx - from;
}

ساكن بیعلامت سطل_calculate_dib(Hashmap_پایه *h, بیعلامت idx, dib_raw_t raw_dib) {
        بیعلامت initial_سطل;

        إذا (raw_dib == DIB_RAW_FREE)
                أرجع DIB_FREE;

        إذا (_likely_(raw_dib < DIB_RAW_سرریز))
                أرجع raw_dib;

        /*
         * Having an overflow DIB value is very unlikely. The hash function
         * would have to be bad. For example, in a table of size 2^24 filled
         * to load factor 0.9 the maximum observed DIB is only about 60.
         * In theory (assuming I used Maxima correctly), for an infinite size
         * hash table with load factor 0.8 the probability of a given entry
         * having DIB > 40 is 1.9e-8.
         * This returns the correct DIB value by recomputing the hash value in
         * the unlikely case. XXX Hitting this case could be a hint to rehash.
         */
        initial_سطل = سطل_hash(h, سطل_at(h, idx)->کلید);
        أرجع سطل_فاصله(h, idx, initial_سطل);
}

ساكن فراغ سطل_مجموعه_dib(Hashmap_پایه *h, بیعلامت idx, بیعلامت dib) {
        dib_raw_ptr(h)[idx] = dib != DIB_FREE ? MIN(dib, DIB_RAW_سرریز) : DIB_RAW_FREE;
}

ساكن بیعلامت skip_free_buckets(Hashmap_پایه *h, بیعلامت idx) {
        dib_raw_t *dibs;

        dibs = dib_raw_ptr(h);

        لكل ( ; idx < n_buckets(h); idx++)
                إذا (dibs[idx] != DIB_RAW_FREE)
                        أرجع idx;

        أرجع IDX_NIL;
}

ساكن فراغ سطل_mark_free(Hashmap_پایه *h, بیعلامت idx) {
        أكد(h);

        memzero(سطل_at(h, idx), hashmap_type_info[h->type].مدخل_اندازه);
        سطل_مجموعه_dib(h, idx, DIB_FREE);
}

ساكن فراغ سطل_move_مدخل(Hashmap_پایه *h, بنية جابجا_entries *جابجا,
                              بیعلامت from, بیعلامت to) {
        بنية hashmap_پایه_مدخل *e_from, *e_مقصد;

        أكد(h);
        أكد(from != to);

        e_from = سطل_at_virtual(h, جابجا, from);
        e_مقصد   = سطل_at_virtual(h, جابجا, to);

        کپی_حافظه(e_مقصد, e_from, hashmap_type_info[h->type].مدخل_اندازه);

        إذا (h->type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*) h;
                بنية ordered_hashmap_مدخل *le, *le_مقصد;

                le_مقصد = (بنية ordered_hashmap_مدخل*) e_مقصد;

                إذا (le_مقصد->iterate_بعدی != IDX_NIL) {
                        le = (بنية ordered_hashmap_مدخل*)
                             سطل_at_virtual(h, جابجا, le_مقصد->iterate_بعدی);
                        le->iterate_قبلی = to;
                }

                إذا (le_مقصد->iterate_قبلی != IDX_NIL) {
                        le = (بنية ordered_hashmap_مدخل*)
                             سطل_at_virtual(h, جابجا, le_مقصد->iterate_قبلی);
                        le->iterate_بعدی = to;
                }

                إذا (lh->iterate_list_سر == from)
                        lh->iterate_list_سر = to;
                إذا (lh->iterate_list_دم == from)
                        lh->iterate_list_دم = to;
        }
}

ساكن مضمن بیعلامت بعدی_idx(Hashmap_پایه *h, بیعلامت idx) {
        أرجع idx + 1U < n_buckets(h) ? idx + 1U : 0U;
}

ساكن بیعلامت prev_idx(Hashmap_پایه *h, بیعلامت idx) {
        أرجع idx > 0U ? idx - 1U : n_buckets(h) - 1U;
}

ساكن مضمن فراغ* مدخل_مقدار(Hashmap_پایه *h, بنية hashmap_پایه_مدخل *e) {
        أكد(h);
        أكد(e);

        بدل (h->type) {

        حالة HASHMAP_TYPE_PLAIN:
        حالة HASHMAP_TYPE_ORDERED:
                أرجع ((بنية plain_hashmap_مدخل*)e)->مقدار;

        حالة HASHMAP_TYPE_مجموعه:
                أرجع (فراغ*) e->کلید;

        افتراضي:
                assert_not_reached();
        }
}

ساكن فراغ پایه_remove_مدخل(Hashmap_پایه *h, بیعلامت idx) {
        بیعلامت چپ, راست, prev, dib;
        dib_raw_t raw_dib, *dibs;

        dibs = dib_raw_ptr(h);
        أكد(dibs[idx] != DIB_RAW_FREE);

#إذا ENABLE_DEBUG_HASHMAP
        أكد(h);
        h->debug.rem_شمار++;
        h->debug.آخرین_rem_idx = idx;
#پایان_اگر

        چپ = idx;
        /* Find the stop bucket ("right"). It is either free or has DIB == 0. */
        لكل (راست = بعدی_idx(h, چپ); ; راست = بعدی_idx(h, راست)) {
                raw_dib = dibs[راست];
                إذا (درون_مجموعه(raw_dib, 0, DIB_RAW_FREE))
                        اكسر;

                /* The buckets are not supposed to be all occupied and with DIB > 0.
                 * That would mean we could make everyone better off by shifting them
                 * backward. This scenario is impossible. */
                أكد(چپ != راست);
        }

        إذا (h->type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*) h;
                بنية ordered_hashmap_مدخل *le = ordered_سطل_at(lh, idx);

                إذا (le->iterate_بعدی != IDX_NIL)
                        ordered_سطل_at(lh, le->iterate_بعدی)->iterate_قبلی = le->iterate_قبلی;
                وإلا
                        lh->iterate_list_دم = le->iterate_قبلی;

                إذا (le->iterate_قبلی != IDX_NIL)
                        ordered_سطل_at(lh, le->iterate_قبلی)->iterate_بعدی = le->iterate_بعدی;
                وإلا
                        lh->iterate_list_سر = le->iterate_بعدی;
        }

        /* Now shift all buckets in the interval (left, right) one step backwards */
        لكل (prev = چپ, چپ = بعدی_idx(h, چپ); چپ != راست;
             prev = چپ, چپ = بعدی_idx(h, چپ)) {
                dib = سطل_calculate_dib(h, چپ, dibs[چپ]);
                أكد(dib != 0);
                سطل_move_مدخل(h, عدم, چپ, prev);
                سطل_مجموعه_dib(h, prev, dib - 1);
        }

        سطل_mark_free(h, prev);
        n_entries_dec(h);
        پایه_مجموعه_dirty(h);
}
#تعریف_کن remove_مدخل(h, idx) پایه_remove_مدخل(HASHMAP_پایه(h), idx)

ساكن بیعلامت hashmap_iterate_درون_insertion_ترتیب(OrderedHashmap *h, Iterator *i) {
        بنية ordered_hashmap_مدخل *e;
        بیعلامت idx;

        أكد(h);
        أكد(i);

        إذا (i->idx == IDX_NIL)
                اذهب at_انتها;

        إذا (i->idx == IDX_اولی && h->iterate_list_سر == IDX_NIL)
                اذهب at_انتها;

        إذا (i->idx == IDX_اولی) {
                idx = h->iterate_list_سر;
                e = ordered_سطل_at(h, idx);
        } وإلا {
                idx = i->idx;
                e = ordered_سطل_at(h, idx);
                /*
                 * We allow removing the current entry while iterating, but removal may cause
                 * a backward shift. The next entry may thus move one bucket to the left.
                 * To detect when it happens, we remember the key pointer of the entry we were
                 * going to iterate next. If it does not match, there was a backward shift.
                 */
                إذا (e->p.b.کلید != i->بعدی_کلید) {
                        idx = prev_idx(HASHMAP_پایه(h), idx);
                        e = ordered_سطل_at(h, idx);
                }
                أكد(e->p.b.کلید == i->بعدی_کلید);
        }

#إذا ENABLE_DEBUG_HASHMAP
        i->prev_idx = idx;
#پایان_اگر

        إذا (e->iterate_بعدی != IDX_NIL) {
                بنية ordered_hashmap_مدخل *n;
                i->idx = e->iterate_بعدی;
                n = ordered_سطل_at(h, i->idx);
                i->بعدی_کلید = n->p.b.کلید;
        } وإلا
                i->idx = IDX_NIL;

        أرجع idx;

at_انتها:
        i->idx = IDX_NIL;
        أرجع IDX_NIL;
}

ساكن بیعلامت hashmap_iterate_درون_داخلی_ترتیب(Hashmap_پایه *h, Iterator *i) {
        بیعلامت idx;

        أكد(h);
        أكد(i);

        إذا (i->idx == IDX_NIL)
                اذهب at_انتها;

        إذا (i->idx == IDX_اولی) {
                /* fast forward to the first occupied bucket */
                إذا (h->has_غیرمستقیم) {
                        i->idx = skip_free_buckets(h, h->غیرمستقیم.idx_پایینترین_مدخل);
                        h->غیرمستقیم.idx_پایینترین_مدخل = i->idx;
                } وإلا
                        i->idx = skip_free_buckets(h, 0);

                إذا (i->idx == IDX_NIL)
                        اذهب at_انتها;
        } وإلا {
                بنية hashmap_پایه_مدخل *e;

                أكد(i->idx > 0);

                e = سطل_at(h, i->idx);
                /*
                 * We allow removing the current entry while iterating, but removal may cause
                 * a backward shift. The next entry may thus move one bucket to the left.
                 * To detect when it happens, we remember the key pointer of the entry we were
                 * going to iterate next. If it does not match, there was a backward shift.
                 */
                إذا (e->کلید != i->بعدی_کلید)
                        e = سطل_at(h, --i->idx);

                أكد(e->کلید == i->بعدی_کلید);
        }

        idx = i->idx;
#إذا ENABLE_DEBUG_HASHMAP
        i->prev_idx = idx;
#پایان_اگر

        i->idx = skip_free_buckets(h, i->idx + 1);
        إذا (i->idx != IDX_NIL)
                i->بعدی_کلید = سطل_at(h, i->idx)->کلید;
        وإلا
                i->idx = IDX_NIL;

        أرجع idx;

at_انتها:
        i->idx = IDX_NIL;
        أرجع IDX_NIL;
}

ساكن بیعلامت hashmap_iterate_مدخل(Hashmap_پایه *h, Iterator *i) {
        إذا (!h) {
                i->idx = IDX_NIL;
                أرجع IDX_NIL;
        }

#إذا ENABLE_DEBUG_HASHMAP
        إذا (i->idx == IDX_اولی) {
                i->put_شمار = h->debug.put_شمار;
                i->rem_شمار = h->debug.rem_شمار;
        } وإلا {
                /* While iterating, must not add any new entries */
                أكد(i->put_شمار == h->debug.put_شمار);
                /* ... or remove entries other than the current one */
                أكد(i->rem_شمار == h->debug.rem_شمار ||
                       (i->rem_شمار == h->debug.rem_شمار - 1 &&
                        i->prev_idx == h->debug.آخرین_rem_idx));
                /* Reset our removals counter */
                i->rem_شمار = h->debug.rem_شمار;
        }
#پایان_اگر

        أرجع h->type == HASHMAP_TYPE_ORDERED ? hashmap_iterate_درون_insertion_ترتیب((OrderedHashmap*) h, i)
                                               : hashmap_iterate_درون_داخلی_ترتیب(h, i);
}

bool _hashmap_iterate(Hashmap_پایه *h, Iterator *i, فراغ **مقدار, ثابت فراغ **کلید) {
        بنية hashmap_پایه_مدخل *e;
        فراغ *داده;
        بیعلامت idx;

        idx = hashmap_iterate_مدخل(h, i);
        إذا (idx == IDX_NIL) {
                إذا (مقدار)
                        *مقدار = عدم;
                إذا (کلید)
                        *کلید = عدم;

                أرجع false;
        }

        e = سطل_at(h, idx);
        داده = مدخل_مقدار(h, e);
        إذا (مقدار)
                *مقدار = داده;
        إذا (کلید)
                *کلید = e->کلید;

        أرجع true;
}

#تعریف_کن HASHMAP_FOREACH_IDX(idx, h, i) \
        لكل ((i) = ITERATOR_اولی, (idx) = hashmap_iterate_مدخل((h), &(i)); \
             (idx != IDX_NIL); \
             (idx) = hashmap_iterate_مدخل((h), &(i)))

IteratedCache* _hashmap_iterated_cache_new(Hashmap_پایه *h) {
        IteratedCache *cache;

        أكد(h);
        أكد(!h->cached);

        إذا (h->cached)
                أرجع عدم;

        cache = new0(IteratedCache, 1);
        إذا (!cache)
                أرجع عدم;

        cache->hashmap = h;
        h->cached = true;

        أرجع cache;
}

ساكن فراغ reset_مستقیم_storage(Hashmap_پایه *h) {
        ثابت بنية hashmap_type_info *hi = &hashmap_type_info[h->type];
        فراغ *p;

        أكد(!h->has_غیرمستقیم);

        p = mempset(h->مستقیم.storage, 0, hi->مدخل_اندازه * hi->n_مستقیم_buckets);
        memset(p, DIB_RAW_INIT, حجم(dib_raw_t) * hi->n_مستقیم_buckets);
}

ساكن فراغ shared_hash_کلید_initialize(فراغ) {
        random_بایتها(shared_hash_کلید, حجم(shared_hash_کلید));
}

ساكن بنية Hashmap_پایه* hashmap_پایه_new(ثابت بنية hash_ops *hash_ops, تعداد HashmapType type) {
        Hashmap_پایه *h;
        ثابت بنية hashmap_type_info *hi = &hashmap_type_info[type];

        bool use_pool = mempool_enabled && mempool_enabled();  /* mempool_enabled is a weak symbol */

        h = use_pool ? mempool_alloc0_tile(hi->mempool) : malloc0(hi->سر_اندازه);
        إذا (!h)
                أرجع عدم;

        h->type = type;
        h->from_pool = use_pool;
        h->hash_ops = hash_ops ?: &trivial_hash_ops;

        إذا (type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*)h;
                lh->iterate_list_سر = lh->iterate_list_دم = IDX_NIL;
        }

        reset_مستقیم_storage(h);

        ساكن pthread_once_t once = PTHREAD_ONCE_INIT;
        assert_se(pthread_once(&once, shared_hash_کلید_initialize) == 0);

#إذا ENABLE_DEBUG_HASHMAP
        assert_se(pthread_mutex_lock(&hashmap_debug_list_mutex) == 0);
        LIST_PREPEND(debug_list, hashmap_debug_list, &h->debug);
        assert_se(pthread_mutex_unlock(&hashmap_debug_list_mutex) == 0);
#پایان_اگر

        أرجع h;
}

Hashmap *hashmap_new(ثابت بنية hash_ops *hash_ops) {
        أرجع (Hashmap*)        hashmap_پایه_new(hash_ops, HASHMAP_TYPE_PLAIN);
}

OrderedHashmap *ordered_hashmap_new(ثابت بنية hash_ops *hash_ops) {
        أرجع (OrderedHashmap*) hashmap_پایه_new(hash_ops, HASHMAP_TYPE_ORDERED);
}

مجموعه_2 *مجموعه_new(ثابت بنية hash_ops *hash_ops) {
        أرجع (مجموعه_2*)            hashmap_پایه_new(hash_ops, HASHMAP_TYPE_مجموعه);
}

ساكن صحيح hashmap_پایه_ensure_allocated(Hashmap_پایه **h, ثابت بنية hash_ops *hash_ops,
                                         تعداد HashmapType type) {
        Hashmap_پایه *q;

        أكد(h);

        إذا (*h) {
                أكد((*h)->hash_ops == (hash_ops ?: &trivial_hash_ops));
                أرجع 0;
        }

        q = hashmap_پایه_new(hash_ops, type);
        إذا (!q)
                أرجع -ENOMEM;

        *h = q;
        أرجع 1;
}

صحيح hashmap_ensure_allocated(Hashmap **h, ثابت بنية hash_ops *hash_ops) {
        أرجع hashmap_پایه_ensure_allocated((Hashmap_پایه**)h, hash_ops, HASHMAP_TYPE_PLAIN);
}

صحيح ordered_hashmap_ensure_allocated(OrderedHashmap **h, ثابت بنية hash_ops *hash_ops) {
        أرجع hashmap_پایه_ensure_allocated((Hashmap_پایه**)h, hash_ops, HASHMAP_TYPE_ORDERED);
}

صحيح مجموعه_ensure_allocated(مجموعه_2 **s, ثابت بنية hash_ops *hash_ops) {
        أرجع hashmap_پایه_ensure_allocated((Hashmap_پایه**)s, hash_ops, HASHMAP_TYPE_مجموعه);
}

صحيح hashmap_ensure_put(Hashmap **h, ثابت بنية hash_ops *hash_ops, ثابت فراغ *کلید, فراغ *مقدار) {
        صحيح r;

        أكد(h);

        r = hashmap_ensure_allocated(h, hash_ops);
        إذا (r < 0)
                أرجع r;

        أرجع hashmap_put(*h, کلید, مقدار);
}

صحيح ordered_hashmap_ensure_put(OrderedHashmap **h, ثابت بنية hash_ops *hash_ops, ثابت فراغ *کلید, فراغ *مقدار) {
        صحيح r;

        أكد(h);

        r = ordered_hashmap_ensure_allocated(h, hash_ops);
        إذا (r < 0)
                أرجع r;

        أرجع ordered_hashmap_put(*h, کلید, مقدار);
}

صحيح ordered_hashmap_ensure_replace(OrderedHashmap **h, ثابت بنية hash_ops *hash_ops, ثابت فراغ *کلید, فراغ *مقدار) {
        صحيح r;

        أكد(h);

        r = ordered_hashmap_ensure_allocated(h, hash_ops);
        إذا (r < 0)
                أرجع r;

        أرجع ordered_hashmap_replace(*h, کلید, مقدار);
}

صحيح hashmap_ensure_replace(Hashmap **h, ثابت بنية hash_ops *hash_ops, ثابت فراغ *کلید, فراغ *مقدار) {
        صحيح r;

        أكد(h);

        r = hashmap_ensure_allocated(h, hash_ops);
        إذا (r < 0)
                أرجع r;

        أرجع hashmap_replace(*h, کلید, مقدار);
}

ساكن فراغ hashmap_free_no_clear(Hashmap_پایه *h) {
        أكد(!h->has_غیرمستقیم);
        أكد(h->n_مستقیم_entries == 0);

#إذا ENABLE_DEBUG_HASHMAP
        assert_se(pthread_mutex_lock(&hashmap_debug_list_mutex) == 0);
        LIST_REMOVE(debug_list, hashmap_debug_list, &h->debug);
        assert_se(pthread_mutex_unlock(&hashmap_debug_list_mutex) == 0);
#پایان_اگر

        إذا (h->from_pool) {
                /* Ensure that the object didn't get migrated between threads. */
                assert_se(is_main_thread());
                mempool_free_tile(hashmap_type_info[h->type].mempool, h);
        } وإلا
                حرر(h);
}

Hashmap_پایه* _hashmap_free(Hashmap_پایه *h) {
        إذا (h) {
                _hashmap_clear(h);
                hashmap_free_no_clear(h);
        }

        أرجع عدم;
}

فراغ _hashmap_clear(Hashmap_پایه *h) {
        إذا (!h)
                أرجع;

        إذا (h->hash_ops->free_کلید || h->hash_ops->free_مقدار) {

                /* If destructor calls are defined, let's destroy things defensively: let's take the item out of the
                 * hash table, and only then call the destructor functions. If these destructors then try to unregister
                 * themselves from our hash table a second time, the entry is already gone. */

                طالما (hashmap_اندازه(h) > 0) {
                        فراغ *k = عدم;
                        فراغ *v;

                        v = hashmap_اولی_کلید_and_مقدار(h, true, &k);

                        إذا (h->hash_ops->free_کلید)
                                h->hash_ops->free_کلید(k);

                        إذا (h->hash_ops->free_مقدار)
                                h->hash_ops->free_مقدار(v);
                }
        }

        إذا (h->has_غیرمستقیم) {
                حرر(h->غیرمستقیم.storage);
                h->has_غیرمستقیم = false;
        }

        h->n_مستقیم_entries = 0;
        reset_مستقیم_storage(h);

        إذا (h->type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*) h;
                lh->iterate_list_سر = lh->iterate_list_دم = IDX_NIL;
        }

        پایه_مجموعه_dirty(h);
}

ساكن صحيح resize_buckets(Hashmap_پایه *h, بیعلامت entries_جمع_کن);

/*
 * Finds an empty bucket to put an entry into, starting the scan at 'idx'.
 * Performs Robin Hood swaps as it goes. The entry to put must be placed
 * by the caller into swap slot IDX_PUT.
 * If used for in-place resizing, may leave a displaced entry in swap slot
 * IDX_PUT. Caller must rehash it next.
 * Returns: true if it left a displaced entry to rehash next in IDX_PUT,
 *          false otherwise.
 */
ساكن bool hashmap_put_robin_hood(Hashmap_پایه *h, بیعلامت idx,
                                   بنية جابجا_entries *جابجا) {
        dib_raw_t raw_dib, *dibs;
        بیعلامت dib, فاصله;

#إذا ENABLE_DEBUG_HASHMAP
        أكد(h);
        h->debug.put_شمار++;
#پایان_اگر

        dibs = dib_raw_ptr(h);

        لكل (فاصله = 0; ; فاصله++) {
                raw_dib = dibs[idx];
                إذا (درون_مجموعه(raw_dib, DIB_RAW_FREE, DIB_RAW_REHASH)) {
                        إذا (raw_dib == DIB_RAW_REHASH)
                                سطل_move_مدخل(h, جابجا, idx, IDX_TMP);

                        إذا (h->has_غیرمستقیم && h->غیرمستقیم.idx_پایینترین_مدخل > idx)
                                h->غیرمستقیم.idx_پایینترین_مدخل = idx;

                        سطل_مجموعه_dib(h, idx, فاصله);
                        سطل_move_مدخل(h, جابجا, IDX_PUT, idx);
                        إذا (raw_dib == DIB_RAW_REHASH) {
                                سطل_move_مدخل(h, جابجا, IDX_TMP, IDX_PUT);
                                أرجع true;
                        }

                        أرجع false;
                }

                dib = سطل_calculate_dib(h, idx, raw_dib);

                إذا (dib < فاصله) {
                        /* Found a wealthier entry. Go Robin Hood! */
                        سطل_مجموعه_dib(h, idx, فاصله);

                        /* swap the entries */
                        سطل_move_مدخل(h, جابجا, idx, IDX_TMP);
                        سطل_move_مدخل(h, جابجا, IDX_PUT, idx);
                        سطل_move_مدخل(h, جابجا, IDX_TMP, IDX_PUT);

                        فاصله = dib;
                }

                idx = بعدی_idx(h, idx);
        }
}

/*
 * Puts an entry into a hashmap, boldly - no check whether key already exists.
 * The caller must place the entry (only its key and value, not link indexes)
 * in swap slot IDX_PUT.
 * Caller must ensure: the key does not exist yet in the hashmap.
 *                     that resize is not needed if !may_resize.
 * Returns: 1 if entry was put successfully.
 *          -ENOMEM if may_resize==true and resize failed with -ENOMEM.
 *          Cannot return -ENOMEM if !may_resize.
 */
ساكن صحيح hashmap_پایه_put_boldly(Hashmap_پایه *h, بیعلامت idx,
                                   بنية جابجا_entries *جابجا, bool may_resize) {
        بنية ordered_hashmap_مدخل *new_مدخل;
        صحيح r;

        أكد(idx < n_buckets(h));

        new_مدخل = سطل_at_جابجا(جابجا, IDX_PUT);

        إذا (may_resize) {
                r = resize_buckets(h, 1);
                إذا (r < 0)
                        أرجع r;
                إذا (r > 0)
                        idx = سطل_hash(h, new_مدخل->p.b.کلید);
        }
        أكد(n_entries(h) < n_buckets(h));

        إذا (h->type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*) h;

                new_مدخل->iterate_بعدی = IDX_NIL;
                new_مدخل->iterate_قبلی = lh->iterate_list_دم;

                إذا (lh->iterate_list_دم != IDX_NIL) {
                        بنية ordered_hashmap_مدخل *old_دم;

                        old_دم = ordered_سطل_at(lh, lh->iterate_list_دم);
                        أكد(old_دم->iterate_بعدی == IDX_NIL);
                        old_دم->iterate_بعدی = IDX_PUT;
                }

                lh->iterate_list_دم = IDX_PUT;
                إذا (lh->iterate_list_سر == IDX_NIL)
                        lh->iterate_list_سر = IDX_PUT;
        }

        assert_se(hashmap_put_robin_hood(h, idx, جابجا) == false);

        n_entries_inc(h);
#إذا ENABLE_DEBUG_HASHMAP
        h->debug.max_entries = MAX(h->debug.max_entries, n_entries(h));
#پایان_اگر

        پایه_مجموعه_dirty(h);

        أرجع 1;
}
#تعریف_کن hashmap_put_boldly(h, idx, جابجا, may_resize) \
        hashmap_پایه_put_boldly(HASHMAP_پایه(h), idx, جابجا, may_resize)

/*
 * Returns 0 if resize is not needed.
 *         1 if successfully resized.
 *         -ENOMEM on allocation failure.
 */
ساكن صحيح resize_buckets(Hashmap_پایه *h, بیعلامت entries_جمع_کن) {
        بنية جابجا_entries جابجا;
        فراغ *new_storage;
        dib_raw_t *old_dibs, *new_dibs;
        ثابت بنية hashmap_type_info *hi;
        بیعلامت idx, optimal_idx;
        بیعلامت old_n_buckets, new_n_buckets, n_rehashed, new_n_entries;
        uint8_t new_لغزش;
        bool rehash_بعدی;

        أكد(h);

        hi = &hashmap_type_info[h->type];
        new_n_entries = n_entries(h) + entries_جمع_کن;

        /* overflow? */
        إذا (_unlikely_(new_n_entries < entries_جمع_کن))
                أرجع -ENOMEM;

        /* For direct storage we allow 100% load, because it's tiny. */
        إذا (!h->has_غیرمستقیم && new_n_entries <= hi->n_مستقیم_buckets)
                أرجع 0;

        /*
         * Load factor = n/m = 1 - (1/INV_KEEP_FREE).
         * From it follows: m = n + n/(INV_KEEP_FREE - 1)
         */
        new_n_buckets = new_n_entries + new_n_entries / (INV_KEEP_FREE - 1);
        /* overflow? */
        إذا (_unlikely_(new_n_buckets < new_n_entries))
                أرجع -ENOMEM;

        old_n_buckets = n_buckets(h);

        إذا (_likely_(new_n_buckets <= old_n_buckets))
                أرجع 0;

        إذا (_unlikely_(new_n_buckets > UINT_MAX / (hi->مدخل_اندازه + حجم(dib_raw_t))))
                أرجع -ENOMEM;

        new_لغزش = log2u_دور_به_بالا(MAX(
                        new_n_buckets * (hi->مدخل_اندازه + حجم(dib_raw_t)),
                        2 * حجم(بنية مستقیم_storage)));

        /* Realloc storage (buckets and DIB array). */
        new_storage = realloc(h->has_غیرمستقیم ? h->غیرمستقیم.storage : عدم,
                              1U << new_لغزش);
        إذا (!new_storage)
                أرجع -ENOMEM;

        /* Must upgrade direct to indirect storage. */
        إذا (!h->has_غیرمستقیم) {
                کپی_حافظه(new_storage, h->مستقیم.storage,
                       old_n_buckets * (hi->مدخل_اندازه + حجم(dib_raw_t)));
                h->غیرمستقیم.n_entries = h->n_مستقیم_entries;
                h->غیرمستقیم.idx_پایینترین_مدخل = 0;
                h->n_مستقیم_entries = 0;
        }

        /* Get a new hash key. If we've just upgraded to indirect storage,
         * allow reusing a previously generated key. It's still a different key
         * from the shared one that we used for direct storage. */
        get_hash_کلید(h->غیرمستقیم.hash_کلید, !h->has_غیرمستقیم);

        h->has_غیرمستقیم = true;
        h->غیرمستقیم.storage = new_storage;
        h->غیرمستقیم.n_buckets = (1U << new_لغزش) /
                                (hi->مدخل_اندازه + حجم(dib_raw_t));

        old_dibs = (dib_raw_t*)((uint8_t*) new_storage + hi->مدخل_اندازه * old_n_buckets);
        new_dibs = dib_raw_ptr(h);

        /*
         * Move the DIB array to the new place, replacing valid DIB values with
         * DIB_RAW_REHASH to indicate all of the used buckets need rehashing.
         * Note: Overlap is not possible, because we have at least doubled the
         * number of buckets and dib_raw_t is smaller than any entry type.
         */
        لكل (idx = 0; idx < old_n_buckets; idx++) {
                أكد(old_dibs[idx] != DIB_RAW_REHASH);
                new_dibs[idx] = old_dibs[idx] == DIB_RAW_FREE ? DIB_RAW_FREE
                                                              : DIB_RAW_REHASH;
        }

        /* Zero the area of newly added entries (including the old DIB area) */
        memzero(سطل_at(h, old_n_buckets),
               (n_buckets(h) - old_n_buckets) * hi->مدخل_اندازه);

        /* The upper half of the new DIB array needs initialization */
        memset(&new_dibs[old_n_buckets], DIB_RAW_INIT,
               (n_buckets(h) - old_n_buckets) * حجم(dib_raw_t));

        /* Rehash entries that need it */
        n_rehashed = 0;
        لكل (idx = 0; idx < old_n_buckets; idx++) {
                إذا (new_dibs[idx] != DIB_RAW_REHASH)
                        استمر;

                optimal_idx = سطل_hash(h, سطل_at(h, idx)->کلید);

                /*
                 * Not much to do if by luck the entry hashes to its current
                 * location. Just set its DIB.
                 */
                إذا (optimal_idx == idx) {
                        new_dibs[idx] = 0;
                        n_rehashed++;
                        استمر;
                }

                new_dibs[idx] = DIB_RAW_FREE;
                سطل_move_مدخل(h, &جابجا, idx, IDX_PUT);
                /* bucket_move_entry does not clear the source */
                memzero(سطل_at(h, idx), hi->مدخل_اندازه);

                افعل {
                        /*
                         * Find the new bucket for the current entry. This may make
                         * another entry homeless and load it into IDX_PUT.
                         */
                        rehash_بعدی = hashmap_put_robin_hood(h, optimal_idx, &جابجا);
                        n_rehashed++;

                        /* Did the current entry displace another one? */
                        إذا (rehash_بعدی)
                                optimal_idx = سطل_hash(h, سطل_at_جابجا(&جابجا, IDX_PUT)->p.b.کلید);
                } طالما (rehash_بعدی);
        }

        assert_se(n_rehashed == n_entries(h));

        أرجع 1;
}

/*
 * Finds an entry with a matching key
 * Returns: index of the found entry, or IDX_NIL if not found.
 */
ساكن بیعلامت پایه_سطل_پیمایش(Hashmap_پایه *h, بیعلامت idx, ثابت فراغ *کلید) {
        compare_func_t compare = h->hash_ops->compare;
        بنية hashmap_پایه_مدخل *e;
        بیعلامت dib, فاصله;
        dib_raw_t *dibs = dib_raw_ptr(h);

        أكد(idx < n_buckets(h));

        لكل (فاصله = 0; ; فاصله++) {
                إذا (dibs[idx] == DIB_RAW_FREE)
                        أرجع IDX_NIL;

                dib = سطل_calculate_dib(h, idx, dibs[idx]);

                إذا (dib < فاصله)
                        أرجع IDX_NIL;
                إذا (dib == فاصله) {
                        e = سطل_at(h, idx);

                        /* Most maps compare keys by pointer. Do that here, without the indirect call. */
                        إذا (compare == trivial_compare_func ? e->کلید == کلید : compare(e->کلید, کلید) == 0)
                                أرجع idx;
                }

                idx = بعدی_idx(h, idx);
        }
}
#تعریف_کن سطل_پیمایش(h, idx, کلید) پایه_سطل_پیمایش(HASHMAP_پایه(h), idx, کلید)

صحيح hashmap_put(Hashmap *h, ثابت فراغ *کلید, فراغ *مقدار) {
        بنية جابجا_entries جابجا;
        بنية plain_hashmap_مدخل *e;
        بیعلامت hash, idx;

        أكد(h);

        hash = سطل_hash(h, کلید);
        idx = سطل_پیمایش(h, hash, کلید);
        إذا (idx != IDX_NIL) {
                e = plain_سطل_at(h, idx);
                إذا (e->مقدار == مقدار)
                        أرجع 0;
                أرجع -EEXIST;
        }

        e = &سطل_at_جابجا(&جابجا, IDX_PUT)->p;
        e->b.کلید = کلید;
        e->مقدار = مقدار;
        أرجع hashmap_put_boldly(h, hash, &جابجا, true);
}

صحيح مجموعه_put(مجموعه_2 *s, ثابت فراغ *کلید) {
        بنية جابجا_entries جابجا;
        بنية hashmap_پایه_مدخل *e;
        بیعلامت hash, idx;

        أكد(s);

        hash = سطل_hash(s, کلید);
        idx = سطل_پیمایش(s, hash, کلید);
        إذا (idx != IDX_NIL)
                أرجع 0;

        e = &سطل_at_جابجا(&جابجا, IDX_PUT)->p.b;
        e->کلید = کلید;
        أرجع hashmap_put_boldly(s, hash, &جابجا, true);
}

صحيح مجموعه_ensure_put(مجموعه_2 **s, ثابت بنية hash_ops *hash_ops, ثابت فراغ *کلید) {
        صحيح r;

        أكد(s);

        r = مجموعه_ensure_allocated(s, hash_ops);
        إذا (r < 0)
                أرجع r;

        أرجع مجموعه_put(*s, کلید);
}

صحيح مجموعه_ensure_consume(مجموعه_2 **s, ثابت بنية hash_ops *hash_ops, فراغ *کلید) {
        صحيح r;

        r = مجموعه_ensure_put(s, hash_ops, کلید);
        إذا (r <= 0) {
                إذا (hash_ops && hash_ops->free_کلید)
                        hash_ops->free_کلید(کلید);
                وإلا إذا (hash_ops && hash_ops->free_مقدار)
                        /* Sets store their element in the key slot but may carry a value destructor. */
                        hash_ops->free_مقدار(کلید);
                وإلا
                        حرر(کلید);
        }

        أرجع r;
}

صحيح hashmap_replace(Hashmap *h, ثابت فراغ *کلید, فراغ *مقدار) {
        بنية جابجا_entries جابجا;
        بنية plain_hashmap_مدخل *e;
        بیعلامت hash, idx;

        أكد(h);

        hash = سطل_hash(h, کلید);
        idx = سطل_پیمایش(h, hash, کلید);
        إذا (idx != IDX_NIL) {
                e = plain_سطل_at(h, idx);
#إذا ENABLE_DEBUG_HASHMAP
                /* Although the key is equal, the key pointer may have changed,
                 * and this would break our assumption for iterating. So count
                 * this operation as incompatible with iteration. */
                إذا (e->b.کلید != کلید) {
                        h->b.debug.put_شمار++;
                        h->b.debug.rem_شمار++;
                        h->b.debug.آخرین_rem_idx = idx;
                }
#پایان_اگر
                e->b.کلید = کلید;
                e->مقدار = مقدار;
                hashmap_مجموعه_dirty(h);

                أرجع 0;
        }

        e = &سطل_at_جابجا(&جابجا, IDX_PUT)->p;
        e->b.کلید = کلید;
        e->مقدار = مقدار;
        أرجع hashmap_put_boldly(h, hash, &جابجا, true);
}

صحيح hashmap_update(Hashmap *h, ثابت فراغ *کلید, فراغ *مقدار) {
        بنية plain_hashmap_مدخل *e;
        بیعلامت hash, idx;

        أكد(h);

        hash = سطل_hash(h, کلید);
        idx = سطل_پیمایش(h, hash, کلید);
        إذا (idx == IDX_NIL)
                أرجع -ENOENT;

        e = plain_سطل_at(h, idx);
        e->مقدار = مقدار;
        hashmap_مجموعه_dirty(h);

        أرجع 0;
}

فراغ* _hashmap_get(Hashmap_پایه *h, ثابت فراغ *کلید) {
        بنية hashmap_پایه_مدخل *e;
        بیعلامت hash, idx;

        إذا (!h)
                أرجع عدم;

        hash = سطل_hash(h, کلید);
        idx = سطل_پیمایش(h, hash, کلید);
        إذا (idx == IDX_NIL)
                أرجع عدم;

        e = سطل_at(h, idx);
        أرجع مدخل_مقدار(h, e);
}

فراغ* hashmap_get2(Hashmap *h, ثابت فراغ *کلید, فراغ **ret) {
        بنية plain_hashmap_مدخل *e;
        بیعلامت hash, idx;

        إذا (!h)
                أرجع عدم;

        hash = سطل_hash(h, کلید);
        idx = سطل_پیمایش(h, hash, کلید);
        إذا (idx == IDX_NIL)
                أرجع عدم;

        e = plain_سطل_at(h, idx);
        إذا (ret)
                *ret = (فراغ*) e->b.کلید;

        أرجع e->مقدار;
}

bool hashmap_شامل_است_2(Hashmap_پایه *h, ثابت فراغ *کلید) {
        بیعلامت hash;

        إذا (!h)
                أرجع false;

        hash = سطل_hash(h, کلید);
        أرجع سطل_پیمایش(h, hash, کلید) != IDX_NIL;
}

فراغ* _hashmap_remove(Hashmap_پایه *h, ثابت فراغ *کلید) {
        بنية hashmap_پایه_مدخل *e;
        بیعلامت hash, idx;
        فراغ *داده;

        إذا (!h)
                أرجع عدم;

        hash = سطل_hash(h, کلید);
        idx = سطل_پیمایش(h, hash, کلید);
        إذا (idx == IDX_NIL)
                أرجع عدم;

        e = سطل_at(h, idx);
        داده = مدخل_مقدار(h, e);
        remove_مدخل(h, idx);

        أرجع داده;
}

فراغ* hashmap_remove2(Hashmap *h, ثابت فراغ *کلید, فراغ **ret) {
        بنية plain_hashmap_مدخل *e;
        بیعلامت hash, idx;
        فراغ *داده;

        إذا (!h) {
                إذا (ret)
                        *ret = عدم;
                أرجع عدم;
        }

        hash = سطل_hash(h, کلید);
        idx = سطل_پیمایش(h, hash, کلید);
        إذا (idx == IDX_NIL) {
                إذا (ret)
                        *ret = عدم;
                أرجع عدم;
        }

        e = plain_سطل_at(h, idx);
        داده = e->مقدار;
        إذا (ret)
                *ret = (فراغ*) e->b.کلید;

        remove_مدخل(h, idx);

        أرجع داده;
}

صحيح hashmap_remove_and_put(Hashmap *h, ثابت فراغ *old_کلید, ثابت فراغ *new_کلید, فراغ *مقدار) {
        بنية جابجا_entries جابجا;
        بنية plain_hashmap_مدخل *e;
        بیعلامت old_hash, new_hash, idx;

        إذا (!h)
                أرجع -ENOENT;

        old_hash = سطل_hash(h, old_کلید);
        idx = سطل_پیمایش(h, old_hash, old_کلید);
        إذا (idx == IDX_NIL)
                أرجع -ENOENT;

        new_hash = سطل_hash(h, new_کلید);
        إذا (سطل_پیمایش(h, new_hash, new_کلید) != IDX_NIL)
                أرجع -EEXIST;

        remove_مدخل(h, idx);

        e = &سطل_at_جابجا(&جابجا, IDX_PUT)->p;
        e->b.کلید = new_کلید;
        e->مقدار = مقدار;
        assert_se(hashmap_put_boldly(h, new_hash, &جابجا, false) == 1);

        أرجع 0;
}

صحيح مجموعه_remove_and_put(مجموعه_2 *s, ثابت فراغ *old_کلید, ثابت فراغ *new_کلید) {
        بنية جابجا_entries جابجا;
        بنية hashmap_پایه_مدخل *e;
        بیعلامت old_hash, new_hash, idx;

        إذا (!s)
                أرجع -ENOENT;

        old_hash = سطل_hash(s, old_کلید);
        idx = سطل_پیمایش(s, old_hash, old_کلید);
        إذا (idx == IDX_NIL)
                أرجع -ENOENT;

        new_hash = سطل_hash(s, new_کلید);
        إذا (سطل_پیمایش(s, new_hash, new_کلید) != IDX_NIL)
                أرجع -EEXIST;

        remove_مدخل(s, idx);

        e = &سطل_at_جابجا(&جابجا, IDX_PUT)->p.b;
        e->کلید = new_کلید;
        assert_se(hashmap_put_boldly(s, new_hash, &جابجا, false) == 1);

        أرجع 0;
}

صحيح hashmap_remove_and_replace(Hashmap *h, ثابت فراغ *old_کلید, ثابت فراغ *new_کلید, فراغ *مقدار) {
        بنية جابجا_entries جابجا;
        بنية plain_hashmap_مدخل *e;
        بیعلامت old_hash, new_hash, idx_old, idx_new;

        إذا (!h)
                أرجع -ENOENT;

        old_hash = سطل_hash(h, old_کلید);
        idx_old = سطل_پیمایش(h, old_hash, old_کلید);
        إذا (idx_old == IDX_NIL)
                أرجع -ENOENT;

        old_کلید = سطل_at(HASHMAP_پایه(h), idx_old)->کلید;

        new_hash = سطل_hash(h, new_کلید);
        idx_new = سطل_پیمایش(h, new_hash, new_کلید);
        إذا (idx_new != IDX_NIL)
                إذا (idx_old != idx_new) {
                        remove_مدخل(h, idx_new);
                        /* Compensate for a possible backward shift. */
                        إذا (old_کلید != سطل_at(HASHMAP_پایه(h), idx_old)->کلید)
                                idx_old = prev_idx(HASHMAP_پایه(h), idx_old);
                        أكد(old_کلید == سطل_at(HASHMAP_پایه(h), idx_old)->کلید);
                }

        remove_مدخل(h, idx_old);

        e = &سطل_at_جابجا(&جابجا, IDX_PUT)->p;
        e->b.کلید = new_کلید;
        e->مقدار = مقدار;
        assert_se(hashmap_put_boldly(h, new_hash, &جابجا, false) == 1);

        أرجع 0;
}

فراغ* hashmap_remove_مقدار(Hashmap_پایه *h, ثابت فراغ *کلید, فراغ *مقدار) {
        بنية hashmap_پایه_مدخل *e;
        بیعلامت hash, idx;

        إذا (!h)
                أرجع عدم;

        hash = سطل_hash(h, کلید);
        idx = سطل_پیمایش(h, hash, کلید);
        إذا (idx == IDX_NIL)
                أرجع عدم;

        e = سطل_at(h, idx);
        إذا (مدخل_مقدار(h, e) != مقدار)
                أرجع عدم;

        remove_مدخل(h, idx);

        أرجع مقدار;
}

ساكن بیعلامت پیدا_کن_اولی_مدخل(Hashmap_پایه *h) {
        Iterator i = ITERATOR_اولی;

        إذا (!h || !n_entries(h))
                أرجع IDX_NIL;

        أرجع hashmap_iterate_مدخل(h, &i);
}

فراغ* hashmap_اولی_کلید_and_مقدار(Hashmap_پایه *h, bool remove, فراغ **ret_کلید) {
        بنية hashmap_پایه_مدخل *e;
        فراغ *کلید, *داده;
        بیعلامت idx;

        idx = پیدا_کن_اولی_مدخل(h);
        إذا (idx == IDX_NIL) {
                إذا (ret_کلید)
                        *ret_کلید = عدم;
                أرجع عدم;
        }

        e = سطل_at(h, idx);
        کلید = (فراغ*) e->کلید;
        داده = مدخل_مقدار(h, e);

        إذا (remove)
                remove_مدخل(h, idx);

        إذا (ret_کلید)
                *ret_کلید = کلید;

        أرجع داده;
}

بیعلامت hashmap_اندازه(Hashmap_پایه *h) {
        إذا (!h)
                أرجع 0;

        أرجع n_entries(h);
}

بیعلامت _hashmap_buckets(Hashmap_پایه *h) {
        إذا (!h)
                أرجع 0;

        أرجع n_buckets(h);
}

صحيح hashmap_ادغام(Hashmap *h, Hashmap *other) {
        Iterator i;
        بیعلامت idx;

        أكد(h);

        HASHMAP_FOREACH_IDX(idx, HASHMAP_پایه(other), i) {
                بنية plain_hashmap_مدخل *pe = plain_سطل_at(other, idx);
                صحيح r;

                r = hashmap_put(h, pe->b.کلید, pe->مقدار);
                إذا (r < 0 && r != -EEXIST)
                        أرجع r;
        }

        أرجع 0;
}

صحيح مجموعه_ادغام(مجموعه_2 *s, مجموعه_2 *other) {
        Iterator i;
        بیعلامت idx;

        أكد(s);

        HASHMAP_FOREACH_IDX(idx, HASHMAP_پایه(other), i) {
                بنية مجموعه_مدخل *se = مجموعه_سطل_at(other, idx);
                صحيح r;

                r = مجموعه_put(s, se->b.کلید);
                إذا (r < 0)
                        أرجع r;
        }

        أرجع 0;
}

صحيح _hashmap_reserve(Hashmap_پایه *h, بیعلامت entries_جمع_کن) {
        صحيح r;

        أكد(h);

        r = resize_buckets(h, entries_جمع_کن);
        إذا (r < 0)
                أرجع r;

        أرجع 0;
}

/*
 * The same as hashmap_merge(), but every new item from other is moved to h.
 * Keys already in h are skipped and stay in other.
 * Returns: 0 on success.
 *          -ENOMEM on alloc failure, in which case no move has been done.
 */
صحيح _hashmap_move(Hashmap_پایه *h, Hashmap_پایه *other) {
        بنية جابجا_entries جابجا;
        بنية hashmap_پایه_مدخل *e, *n;
        Iterator i;
        بیعلامت idx;
        صحيح r;

        أكد(h);

        إذا (!other)
                أرجع 0;

        أكد(other->type == h->type);

        /*
         * This reserves buckets for the worst case, where none of other's
         * entries are yet present in h. This is preferable to risking
         * an allocation failure in the middle of the moving and having to
         * rollback or return a partial result.
         */
        r = resize_buckets(h, n_entries(other));
        إذا (r < 0)
                أرجع r;

        HASHMAP_FOREACH_IDX(idx, other, i) {
                بیعلامت h_hash;

                e = سطل_at(other, idx);
                h_hash = سطل_hash(h, e->کلید);
                إذا (سطل_پیمایش(h, h_hash, e->کلید) != IDX_NIL)
                        استمر;

                n = &سطل_at_جابجا(&جابجا, IDX_PUT)->p.b;
                n->کلید = e->کلید;
                إذا (h->type != HASHMAP_TYPE_مجموعه)
                        ((بنية plain_hashmap_مدخل*) n)->مقدار =
                                ((بنية plain_hashmap_مدخل*) e)->مقدار;
                assert_se(hashmap_put_boldly(h, h_hash, &جابجا, false) == 1);

                remove_مدخل(other, idx);
        }

        أرجع 0;
}

صحيح _hashmap_move_one(Hashmap_پایه *h, Hashmap_پایه *other, ثابت فراغ *کلید) {
        بنية جابجا_entries جابجا;
        بیعلامت h_hash, other_hash, idx;
        بنية hashmap_پایه_مدخل *e, *n;
        صحيح r;

        أكد(h);

        h_hash = سطل_hash(h, کلید);
        إذا (سطل_پیمایش(h, h_hash, کلید) != IDX_NIL)
                أرجع -EEXIST;

        إذا (!other)
                أرجع -ENOENT;

        أكد(other->type == h->type);

        other_hash = سطل_hash(other, کلید);
        idx = سطل_پیمایش(other, other_hash, کلید);
        إذا (idx == IDX_NIL)
                أرجع -ENOENT;

        e = سطل_at(other, idx);

        n = &سطل_at_جابجا(&جابجا, IDX_PUT)->p.b;
        n->کلید = e->کلید;
        إذا (h->type != HASHMAP_TYPE_مجموعه)
                ((بنية plain_hashmap_مدخل*) n)->مقدار =
                        ((بنية plain_hashmap_مدخل*) e)->مقدار;
        r = hashmap_put_boldly(h, h_hash, &جابجا, true);
        إذا (r < 0)
                أرجع r;

        remove_مدخل(other, idx);
        أرجع 0;
}

Hashmap_پایه* _hashmap_copy(Hashmap_پایه *h) {
        Hashmap_پایه *copy;
        صحيح r;

        أكد(h);

        copy = hashmap_پایه_new(h->hash_ops, h->type);
        إذا (!copy)
                أرجع عدم;

        بدل (h->type) {
        حالة HASHMAP_TYPE_PLAIN:
        حالة HASHMAP_TYPE_ORDERED:
                r = hashmap_ادغام_2((Hashmap*)copy, (Hashmap*)h);
                اكسر;
        حالة HASHMAP_TYPE_مجموعه:
                r = مجموعه_ادغام((مجموعه_2*)copy, (مجموعه_2*)h);
                اكسر;
        افتراضي:
                assert_not_reached();
        }

        إذا (r < 0)
                أرجع _hashmap_free(copy);

        أرجع copy;
}

حرف** _hashmap_get_strv(Hashmap_پایه *h) {
        حرف **sv;
        Iterator i;
        بیعلامت idx, n;

        إذا (!h)
                أرجع new0(حرف*, 1);

        sv = new(حرف*, n_entries(h)+1);
        إذا (!sv)
                أرجع عدم;

        n = 0;
        HASHMAP_FOREACH_IDX(idx, h, i)
                sv[n++] = مدخل_مقدار(h, سطل_at(h, idx));
        sv[n] = عدم;

        أرجع sv;
}

حرف** مجموعه_مقصد_strv(مجموعه_2 **s) {
        أكد(s);

        /* This is similar to set_get_strv(), but invalidates the set on success. */

        حرف **v = new(حرف*, مجموعه_اندازه(*s) + 1);
        إذا (!v)
                أرجع عدم;

        لكل (حرف **p = v; (*p = مجموعه_steal_اولی(*s)); p++)
                ;

        أكد(مجموعه_isempty(*s));
        *s = مجموعه_free(*s);
        أرجع v;
}

فراغ* ordered_hashmap_بعدی(OrderedHashmap *h, ثابت فراغ *کلید) {
        بنية ordered_hashmap_مدخل *e;
        بیعلامت hash, idx;

        إذا (!h)
                أرجع عدم;

        hash = سطل_hash(h, کلید);
        idx = سطل_پیمایش(h, hash, کلید);
        إذا (idx == IDX_NIL)
                أرجع عدم;

        e = ordered_سطل_at(h, idx);
        إذا (e->iterate_بعدی == IDX_NIL)
                أرجع عدم;
        أرجع ordered_سطل_at(h, e->iterate_بعدی)->p.مقدار;
}

صحيح مجموعه_consume(مجموعه_2 *s, فراغ *مقدار) {
        صحيح r;

        أكد(s);
        أكد(مقدار);

        r = مجموعه_put(s, مقدار);
        إذا (r <= 0)
                حرر(مقدار);

        أرجع r;
}

صحيح hashmap_put_strdup_پر(Hashmap **h, ثابت بنية hash_ops *hash_ops, ثابت حرف *k, ثابت حرف *v) {
        صحيح r;

        أكد(h);

        r = hashmap_ensure_allocated(h, hash_ops);
        إذا (r < 0)
                أرجع r;

        _cleanup_free_ حرف *kdup = عدم, *vdup = عدم;

        kdup = strdup(k);
        إذا (!kdup)
                أرجع -ENOMEM;

        إذا (v) {
                vdup = strdup(v);
                إذا (!vdup)
                        أرجع -ENOMEM;
        }

        r = hashmap_put(*h, kdup, vdup);
        إذا (r < 0) {
                إذا (r == -EEXIST && streq_ptr(v, hashmap_get(*h, kdup)))
                        أرجع 0;
                أرجع r;
        }

        /* 0 with non-null vdup would mean vdup is already in the hashmap, which cannot be */
        أكد(vdup == عدم || r > 0);
        إذا (r > 0)
                kdup = vdup = عدم;

        أرجع r;
}

صحيح مجموعه_put_strndup_پر(مجموعه_2 **s, ثابت بنية hash_ops *hash_ops, ثابت حرف *p, size_t n) {
        حرف *c;
        صحيح r;

        أكد(s);
        أكد(p);

        r = مجموعه_ensure_allocated(s, hash_ops);
        إذا (r < 0)
                أرجع r;

        إذا (n == SIZE_MAX) {
                إذا (مجموعه_شامل_است(*s, (حرف*) p))
                        أرجع 0;

                c = strdup(p);
        } وإلا
                c = strndup(p, n);
        إذا (!c)
                أرجع -ENOMEM;

        أرجع مجموعه_consume(*s, c);
}

صحيح مجموعه_put_strdupv_پر(مجموعه_2 **s, ثابت بنية hash_ops *hash_ops, حرف **l) {
        صحيح n = 0, r;

        أكد(s);

        STRV_FOREACH(i, l) {
                r = مجموعه_put_strndup_پر(s, hash_ops, *i, SIZE_MAX);
                إذا (r < 0)
                        أرجع r;

                n += r;
        }

        أرجع n;
}

صحيح مجموعه_put_strsplit(مجموعه_2 *s, ثابت حرف *v, ثابت حرف *separators, استخراج_Flags flags) {
        ثابت حرف *p = ASSERT_PTR(v);
        صحيح r;

        أكد(s);

        لكل (;;) {
                حرف *کلمه;

                r = استخراج_اولی_کلمه(&p, &کلمه, separators, flags);
                إذا (r <= 0)
                        أرجع r;

                r = مجموعه_consume(s, کلمه);
                إذا (r < 0)
                        أرجع r;
        }
}

/* expand the cachemem if needed, return true if newly (re)activated. */
ساكن صحيح cachemem_maintain(CacheMem *mem, size_t اندازه) {
        أكد(mem);

        إذا (!GREEDY_REALLOC(mem->ptr, اندازه)) {
                إذا (اندازه > 0)
                        أرجع -ENOMEM;
        }

        إذا (!mem->فعال) {
                mem->فعال = true;
                أرجع true;
        }

        أرجع false;
}

صحيح iterated_cache_get(IteratedCache *cache, ثابت فراغ ***res_کلیدها, ثابت فراغ ***res_مقادیر, بیعلامت *res_n_entries) {
        bool sync_کلیدها = false, sync_مقادیر = false;
        size_t اندازه;
        صحيح r;

        أكد(cache);
        أكد(cache->hashmap);

        اندازه = n_entries(cache->hashmap);

        إذا (res_کلیدها) {
                r = cachemem_maintain(&cache->کلیدها, اندازه);
                إذا (r < 0)
                        أرجع r;

                sync_کلیدها = r;
        } وإلا
                cache->کلیدها.فعال = false;

        إذا (res_مقادیر) {
                r = cachemem_maintain(&cache->values, اندازه);
                إذا (r < 0)
                        أرجع r;

                sync_مقادیر = r;
        } وإلا
                cache->values.فعال = false;

        إذا (cache->hashmap->dirty) {
                إذا (cache->کلیدها.فعال)
                        sync_کلیدها = true;
                إذا (cache->values.فعال)
                        sync_مقادیر = true;

                cache->hashmap->dirty = false;
        }

        إذا (sync_کلیدها || sync_مقادیر) {
                بیعلامت i, idx;
                Iterator iter;

                i = 0;
                HASHMAP_FOREACH_IDX(idx, cache->hashmap, iter) {
                        بنية hashmap_پایه_مدخل *e;

                        e = سطل_at(cache->hashmap, idx);

                        إذا (sync_کلیدها)
                                cache->کلیدها.ptr[i] = e->کلید;
                        إذا (sync_مقادیر)
                                cache->values.ptr[i] = مدخل_مقدار(cache->hashmap, e);
                        i++;
                }
        }

        إذا (res_کلیدها)
                *res_کلیدها = cache->کلیدها.ptr;
        إذا (res_مقادیر)
                *res_مقادیر = cache->values.ptr;
        إذا (res_n_entries)
                *res_n_entries = اندازه;

        أرجع 0;
}

IteratedCache* iterated_cache_free(IteratedCache *cache) {
        إذا (cache) {
                حرر(cache->کلیدها.ptr);
                حرر(cache->values.ptr);
        }

        أرجع mfree(cache);
}

صحيح مجموعه_strjoin(مجموعه_2 *s, ثابت حرف *separator, bool wrap_with_separator, حرف **ret) {
        _cleanup_free_ حرف *str = عدم;
        size_t separator_len, len = 0;
        ثابت حرف *مقدار;
        bool اولی;

        أكد(ret);

        إذا (مجموعه_isempty(s)) {
                *ret = عدم;
                أرجع 0;
        }

        separator_len = strlen_ptr(separator);

        إذا (separator_len == 0)
                wrap_with_separator = false;

        اولی = !wrap_with_separator;

        مجموعه_FOREACH(مقدار, s) {
                size_t l = strlen_ptr(مقدار);

                إذا (l == 0)
                        استمر;

                إذا (!GREEDY_REALLOC(str, len + l + (اولی ? 0 : separator_len) + (wrap_with_separator ? separator_len : 0) + 1))
                        أرجع -ENOMEM;

                إذا (separator_len > 0 && !اولی) {
                        کپی_حافظه(str + len, separator, separator_len);
                        len += separator_len;
                }

                کپی_حافظه(str + len, مقدار, l);
                len += l;
                اولی = false;
        }

        إذا (wrap_with_separator) {
                کپی_حافظه(str + len, separator, separator_len);
                len += separator_len;
        }

        str[len] = '\0';

        *ret = TAKE_PTR(str);
        أرجع 0;
}

bool مجموعه_equal(مجموعه_2 *a, مجموعه_2 *b) {
        فراغ *p;

        /* Checks whether each entry of 'a' is also in 'b' and vice versa, i.e. the two sets contain the same
         * entries */

        إذا (a == b)
                أرجع true;

        إذا (مجموعه_isempty(a) && مجموعه_isempty(b))
                أرجع true;

        إذا (مجموعه_اندازه(a) != مجموعه_اندازه(b)) /* Cheap check that hopefully catches a lot of inequality cases
                                         * already */
                أرجع false;

        مجموعه_FOREACH(p, a)
                إذا (!مجموعه_شامل_است(b, p))
                        أرجع false;

        /* If we have the same hashops, then we don't need to check things backwards given we compared the
         * size and that all of a is in b. */
        إذا (a->b.hash_ops == b->b.hash_ops)
                أرجع true;

        مجموعه_FOREACH(p, b)
                إذا (!مجموعه_شامل_است(a, p))
                        أرجع false;

        أرجع true;
}

ساكن bool مجموعه_fnmatch_one(مجموعه_2 *patterns, ثابت حرف *needle) {
        ثابت حرف *p;

        أكد(needle);

        /* Any failure of fnmatch() is treated as equivalent to FNM_NOMATCH, i.e. as non-matching pattern */

        مجموعه_FOREACH(p, patterns)
                إذا (fnmatch(p, needle, 0) == 0)
                        أرجع true;

        أرجع false;
}

bool مجموعه_fnmatch(مجموعه_2 *include_patterns, مجموعه_2 *exclude_patterns, ثابت حرف *needle) {
        أكد(needle);

        إذا (مجموعه_fnmatch_one(exclude_patterns, needle))
                أرجع false;

        إذا (مجموعه_isempty(include_patterns))
                أرجع true;

        أرجع مجموعه_fnmatch_one(include_patterns, needle);
}

ساكن صحيح hashmap_مدخل_compare(
                بنية hashmap_پایه_مدخل * ثابت *a,
                بنية hashmap_پایه_مدخل * ثابت *b,
                compare_func_t compare) {

        أكد(a && *a);
        أكد(b && *b);
        أكد(compare);

        أرجع compare((*a)->کلید, (*b)->کلید);
}

ساكن صحيح _hashmap_dump_entries_sorted(
                Hashmap_پایه *h,
                فراغ ***ret,
                size_t *ret_n) {
        _cleanup_free_ فراغ **entries = عدم;
        Iterator iter;
        بیعلامت idx;
        size_t n = 0;

        أكد(ret);
        أكد(ret_n);

        إذا (hashmap_اندازه(h) == 0) {
                *ret = عدم;
                *ret_n = 0;
                أرجع 0;
        }

        /* We append one more element than needed so that the resulting array can be used as a strv. We
         * don't count this entry in the returned size. */
        entries = new(فراغ*, hashmap_اندازه(h) + 1);
        إذا (!entries)
                أرجع -ENOMEM;

        HASHMAP_FOREACH_IDX(idx, h, iter)
                entries[n++] = سطل_at(h, idx);

        أكد(n == hashmap_اندازه(h));
        entries[n] = عدم;

        typesafe_qsort_r((بنية hashmap_پایه_مدخل**) entries, n,
                         hashmap_مدخل_compare, h->hash_ops->compare);

        *ret = TAKE_PTR(entries);
        *ret_n = n;
        أرجع 0;
}

صحيح hashmap_dump_کلیدها_sorted(Hashmap_پایه *h, فراغ ***ret, size_t *ret_n) {
        _cleanup_free_ فراغ **entries = عدم;
        size_t n;
        صحيح r;

        أكد(ret);

        r = _hashmap_dump_entries_sorted(h, &entries, &n);
        إذا (r < 0)
                أرجع r;

        /* Reuse the array. */
        FOREACH_ARRAY(e, entries, n)
                *e = (فراغ*) (*(بنية hashmap_پایه_مدخل**) e)->کلید;

        *ret = TAKE_PTR(entries);
        إذا (ret_n)
                *ret_n = n;
        أرجع 0;
}

صحيح _hashmap_dump_sorted(Hashmap_پایه *h, فراغ ***ret, size_t *ret_n) {
        _cleanup_free_ فراغ **entries = عدم;
        size_t n;
        صحيح r;

        أكد(ret);

        r = _hashmap_dump_entries_sorted(h, &entries, &n);
        إذا (r < 0)
                أرجع r;

        /* Reuse the array. */
        FOREACH_ARRAY(e, entries, n)
                *e = مدخل_مقدار(h, *(بنية hashmap_پایه_مدخل**) e);

        *ret = TAKE_PTR(entries);
        إذا (ret_n)
                *ret_n = n;
        أرجع 0;
}
