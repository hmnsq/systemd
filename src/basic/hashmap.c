/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <fnmatch.h>
#ضمن <pthread.h>
#ضمن <unistd.h>
#إذا HAVE_VALGRIND_VALGRIND_H
#  include <valgrind/valgrind.h>
#د_که_پای

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
#د_که_پای

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
#تعریف_کړه INV_KEEP_FREE            5U

/* Fields common to entries of all hashmap/set types */
بنية hashmap_بنسټ_ثبت {
        ثابت فراغ *کلی;
};

/* Entry types for specific hashmap/set types
 * hashmap_base_entry must be at the beginning of each entry struct. */

بنية plain_hashmap_ثبت {
        بنية hashmap_بنسټ_ثبت b;
        فراغ *ارزښت;
};

بنية ordered_hashmap_ثبت {
        بنية plain_hashmap_ثبت p;
        بینښه iterate_بل, iterate_پخوانی;
};

بنية سېټ_ثبت {
        بنية hashmap_بنسټ_ثبت b;
};

/* In several functions it is advantageous to have the hash table extended
 * virtually by a couple of additional buckets. We reserve special index values
 * for these "swap" buckets. */
#تعریف_کړه IDX_بدلون_BEGIN     (UINT_MAX - 3)
#تعریف_کړه IDX_PUT             (IDX_بدلون_BEGIN + 0)
#تعریف_کړه IDX_TMP             (IDX_بدلون_BEGIN + 1)
#تعریف_کړه IDX_بدلون_پای_ټکی       (IDX_بدلون_BEGIN + 2)

#تعریف_کړه IDX_لومړی           (UINT_MAX - 1) /* special index for freshly initialized iterators */
#تعریف_کړه IDX_NIL             UINT_MAX       /* special index value meaning "none" or "end" */

assert_cc(IDX_لومړی == IDX_بدلون_پای_ټکی);
assert_cc(IDX_لومړی == IDX_ITERATOR_لومړی);

/* Storage space for the "swap" buckets.
 * All entry types can fit into an ordered_hashmap_entry. */
بنية بدلون_entries {
        بنية ordered_hashmap_ثبت e[IDX_بدلون_پای_ټکی - IDX_بدلون_BEGIN];
};

/* Distance from Initial Bucket */
تعريفنوع uint8_t dib_raw_t;
#تعریف_کړه DIB_RAW_ډکېدنه ((dib_raw_t)0xfdU)   /* indicates DIB value is greater than representable */
#تعریف_کړه DIB_RAW_REHASH   ((dib_raw_t)0xfeU)   /* entry yet to be rehashed during in-place resize */
#تعریف_کړه DIB_RAW_FREE     ((dib_raw_t)0xffU)   /* a free bucket */
#تعریف_کړه DIB_RAW_INIT     ((حرف)DIB_RAW_FREE) /* a byte to memset a DIB store with when initializing */

#تعریف_کړه DIB_FREE UINT_MAX

#إذا ENABLE_DEBUG_HASHMAP
بنية hashmap_debug_info {
        LIST_FIELDS(بنية hashmap_debug_info, debug_list);
        بینښه max_entries;  /* high watermark of n_entries */

        /* fields to detect modification while iterating */
        بینښه put_شمېر;    /* counts puts into the hashmap */
        بینښه rem_شمېر;    /* counts removals from hashmap */
        بینښه وروستی_rem_idx; /* remembers last removal index */
};

/* Tracks all existing hashmaps. Get at it from gdb. See sd_dump_hashmaps.py */
ساكن LIST_سر(بنية hashmap_debug_info, hashmap_debug_list);
ساكن pthread_mutex_t hashmap_debug_list_mutex = PTHREAD_MUTEX_INITIALIZER;
#د_که_پای

تعداد HashmapType {
        HASHMAP_TYPE_PLAIN,
        HASHMAP_TYPE_ORDERED,
        HASHMAP_TYPE_سېټ,
        _HASHMAP_TYPE_MAX
};

بنية _packed_ غیرمستقیم_storage {
        فراغ *storage;                     /* where buckets and DIBs are stored */
        uint8_t  hash_کلی[HASH_کلی_اندازه];  /* hash key; changes during resize */

        بینښه n_entries;                /* number of stored entries */
        بینښه n_buckets;                /* number of buckets */

        بینښه idx_ټیټ_ترین_ثبت;         /* Index below which all buckets are free.
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

#تعریف_کړه مستقیم_BUCKETS(ثبت_t) \
        (حجم(بنية مستقیم_storage) / (حجم(ثبت_t) + حجم(dib_raw_t)))

/* We should be able to store at least one entry directly. */
assert_cc(مستقیم_BUCKETS(بنية ordered_hashmap_ثبت) >= 1);

/* We have 3 bits for n_direct_entries. */
assert_cc(مستقیم_BUCKETS(بنية سېټ_ثبت) < (1 << 3));

/* Hashmaps with directly stored entries all use this shared hash key.
 * It's no big deal if the key is guessed, because there can be only
 * a handful of directly stored entries in a hashmap. When a hashmap
 * outgrows direct storage, it gets its own key for indirect storage. */
ساكن uint8_t shared_hash_کلی[HASH_کلی_اندازه];

/* Fields that all hashmap/set types must have */
بنية Hashmap_بنسټ {
        ثابت بنية hash_ops *hash_ops;  /* hash and compare ops to use */

        اتحاد _packed_ {
                بنية غیرمستقیم_storage غیرمستقیم; /* if  has_indirect */
                بنية مستقیم_storage مستقیم;     /* if !has_indirect */
        };

        تعداد HashmapType type:2;     /* HASHMAP_TYPE_* */
        bool has_غیرمستقیم:1;         /* whether indirect storage is used */
        بینښه n_مستقیم_entries:3; /* Number of entries in direct storage.
                                      * Only valid if !has_indirect. */
        bool from_pool:1;            /* whether was allocated from mempool */
        bool dirty:1;                /* whether dirtied since last iterated_cache_get() */
        bool cached:1;               /* whether this hashmap is being cached */

#إذا ENABLE_DEBUG_HASHMAP
        بنية hashmap_debug_info debug;
#د_که_پای
};

/* Specific hash types
 * HashmapBase must be at the beginning of each hashmap struct. */

بنية Hashmap {
        بنية Hashmap_بنسټ b;
};

بنية OrderedHashmap {
        بنية Hashmap_بنسټ b;
        بینښه iterate_list_سر, iterate_list_لکۍ;
};

بنية سېټ_2 {
        بنية Hashmap_بنسټ b;
};

تعريفنوع بنية CacheMem {
        ثابت فراغ **ptr;
        size_t n_populated;
        bool فعال:1;
} CacheMem;

بنية IteratedCache {
        Hashmap_بنسټ *hashmap;
        CacheMem کلیګانې, values;
};

DEFINE_MEMPOOL(hashmap_pool,         Hashmap,        8);
DEFINE_MEMPOOL(ordered_hashmap_pool, OrderedHashmap, 8);
/* No need for a separate Set pool */
assert_cc(حجم(Hashmap) == حجم(سېټ_2));

بنية hashmap_type_info {
        size_t سر_اندازه;
        size_t ثبت_اندازه;
        بنية mempool *mempool;
        بینښه n_مستقیم_buckets;
};

ساكن کارول_شوی_2 ثابت بنية hashmap_type_info hashmap_type_info[_HASHMAP_TYPE_MAX] = {
        [HASHMAP_TYPE_PLAIN] = {
                .سر_اندازه        = حجم(Hashmap),
                .ثبت_اندازه       = حجم(بنية plain_hashmap_ثبت),
                .mempool          = &hashmap_pool,
                .n_مستقیم_buckets = مستقیم_BUCKETS(بنية plain_hashmap_ثبت),
        },
        [HASHMAP_TYPE_ORDERED] = {
                .سر_اندازه        = حجم(OrderedHashmap),
                .ثبت_اندازه       = حجم(بنية ordered_hashmap_ثبت),
                .mempool          = &ordered_hashmap_pool,
                .n_مستقیم_buckets = مستقیم_BUCKETS(بنية ordered_hashmap_ثبت),
        },
        [HASHMAP_TYPE_سېټ] = {
                .سر_اندازه        = حجم(سېټ_2),
                .ثبت_اندازه       = حجم(بنية سېټ_ثبت),
                .mempool          = &hashmap_pool,
                .n_مستقیم_buckets = مستقیم_BUCKETS(بنية سېټ_ثبت),
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

        r = get_process_تارونه(0);
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
#د_که_پای

ساكن مضمن بینښه n_buckets(Hashmap_بنسټ *h) {
        أكد(h);
        أرجع h->has_غیرمستقیم ? h->غیرمستقیم.n_buckets
                               : hashmap_type_info[h->type].n_مستقیم_buckets;
}

ساكن بینښه n_entries(Hashmap_بنسټ *h) {
        أكد(h);
        أرجع h->has_غیرمستقیم ? h->غیرمستقیم.n_entries
                               : h->n_مستقیم_entries;
}

ساكن فراغ n_entries_inc(Hashmap_بنسټ *h) {
        أكد(h);

        إذا (h->has_غیرمستقیم)
                h->غیرمستقیم.n_entries++;
        وإلا
                h->n_مستقیم_entries++;
}

ساكن فراغ n_entries_dec(Hashmap_بنسټ *h) {
        أكد(h);

        إذا (h->has_غیرمستقیم)
                h->غیرمستقیم.n_entries--;
        وإلا
                h->n_مستقیم_entries--;
}

ساكن مضمن فراغ* storage_ptr(Hashmap_بنسټ *h) {
        أكد(h);
        أرجع h->has_غیرمستقیم ? h->غیرمستقیم.storage
                               : h->مستقیم.storage;
}

ساكن مضمن uint8_t* hash_کلی(Hashmap_بنسټ *h) {
        أكد(h);
        أرجع h->has_غیرمستقیم ? h->غیرمستقیم.hash_کلی
                               : shared_hash_کلی;
}

ساكن بینښه بنسټ_لوښی_hash(Hashmap_بنسټ *h, ثابت فراغ *p) {
        بنية siphash state;
        uint64_t hash;

        أكد(h);

        /* The keys of trivial_hash_ops and uint64_hash_ops fit in 64 bits. siphash24_uint64() hashes such
         * a key without the indirect call through hash_ops and keeps the SipHash state in registers. On
         * 32-bit and big-endian architectures, the result differs from the result of trivial_hash_func(),
         * which hashes the key in native byte order. Only this function maps keys to buckets, so the
         * difference does not matter. */
        إذا (h->hash_ops->hash == trivial_hash_func)
                hash = siphash24_uint64((uintptr_t) p, hash_کلی(h));
        وإلا إذا (h->hash_ops->hash == (hash_func_t) uint64_hash_func)
                hash = siphash24_uint64(*(ثابت uint64_t*) p, hash_کلی(h));
        وإلا {
                siphash24_init(&state, hash_کلی(h));
                h->hash_ops->hash(p, &state);
                hash = siphash24_finalize(&state);
        }

        /* n_buckets is not a power of two, so a modulo needs a division. Multiplying the upper 32 bits of
         * the hash with n_buckets and keeping the upper 32 bits of the product maps the hash to
         * [0, n_buckets) almost as evenly, without a division. */
        أرجع (بینښه) (((hash >> 32) * n_buckets(h)) >> 32);
}
#تعریف_کړه لوښی_hash(h, p) بنسټ_لوښی_hash(HASHMAP_بنسټ(h), p)

ساكن فراغ بنسټ_سېټ_dirty(Hashmap_بنسټ *h) {
        أكد(h);

        h->dirty = true;
}
#تعریف_کړه hashmap_سېټ_dirty(h) بنسټ_سېټ_dirty(HASHMAP_بنسټ(h))

ساكن فراغ get_hash_کلی(uint8_t hash_کلی[HASH_کلی_اندازه], bool reuse_is_ok) {
        ساكن uint8_t current[HASH_کلی_اندازه];
        ساكن bool current_initialized = false;

        /* Returns a hash function key to use. In order to keep things
         * fast we will not generate a new key each time we allocate a
         * new hash table. Instead, we'll just reuse the most recently
         * generated one, except if we never generated one or when we
         * are rehashing an entire hash table because we reached a
         * fill level */

        إذا (!current_initialized || !reuse_is_ok) {
                random_بایټونه(current, حجم(current));
                current_initialized = true;
        }

        حافظه_نقل(hash_کلی, current, حجم(current));
}

ساكن مضمن بنية hashmap_بنسټ_ثبت* لوښی_at(Hashmap_بنسټ *h, بینښه idx) {
        أكد(h);
        أرجع CAST_ALIGN_PTR(
                        بنية hashmap_بنسټ_ثبت,
                        (uint8_t *) storage_ptr(h) + idx * hashmap_type_info[h->type].ثبت_اندازه);
}

ساكن بنية plain_hashmap_ثبت* plain_لوښی_at(Hashmap *h, بینښه idx) {
        أرجع (بنية plain_hashmap_ثبت*) لوښی_at(HASHMAP_بنسټ(h), idx);
}

ساكن بنية ordered_hashmap_ثبت* ordered_لوښی_at(OrderedHashmap *h, بینښه idx) {
        أرجع (بنية ordered_hashmap_ثبت*) لوښی_at(HASHMAP_بنسټ(h), idx);
}

ساكن بنية سېټ_ثبت *سېټ_لوښی_at(سېټ_2 *h, بینښه idx) {
        أرجع (بنية سېټ_ثبت*) لوښی_at(HASHMAP_بنسټ(h), idx);
}

ساكن بنية ordered_hashmap_ثبت* لوښی_at_بدلون(بنية بدلون_entries *بدلون, بینښه idx) {
        أكد(بدلون);
        أرجع &بدلون->e[idx - IDX_بدلون_BEGIN];
}

/* Returns a pointer to the bucket at index idx.
 * Understands real indexes and swap indexes, hence "_virtual". */
ساكن بنية hashmap_بنسټ_ثبت* لوښی_at_virtual(Hashmap_بنسټ *h, بنية بدلون_entries *بدلون,
                                                    بینښه idx) {
        إذا (idx < IDX_بدلون_BEGIN)
                أرجع لوښی_at(h, idx);

        إذا (idx < IDX_بدلون_پای_ټکی)
                أرجع &لوښی_at_بدلون(بدلون, idx)->p.b;

        assert_not_reached();
}

ساكن مضمن dib_raw_t* dib_raw_ptr(Hashmap_بنسټ *h) {
        أكد(h);
        أرجع (dib_raw_t*)
                ((uint8_t*) storage_ptr(h) + hashmap_type_info[h->type].ثبت_اندازه * n_buckets(h));
}

ساكن بینښه لوښی_واټن(Hashmap_بنسټ *h, بینښه idx, بینښه from) {
        أرجع idx >= from ? idx - from
                           : n_buckets(h) + idx - from;
}

ساكن بینښه لوښی_calculate_dib(Hashmap_بنسټ *h, بینښه idx, dib_raw_t raw_dib) {
        بینښه initial_لوښی;

        إذا (raw_dib == DIB_RAW_FREE)
                أرجع DIB_FREE;

        إذا (_likely_(raw_dib < DIB_RAW_ډکېدنه))
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
        initial_لوښی = لوښی_hash(h, لوښی_at(h, idx)->کلی);
        أرجع لوښی_واټن(h, idx, initial_لوښی);
}

ساكن فراغ لوښی_سېټ_dib(Hashmap_بنسټ *h, بینښه idx, بینښه dib) {
        dib_raw_ptr(h)[idx] = dib != DIB_FREE ? MIN(dib, DIB_RAW_ډکېدنه) : DIB_RAW_FREE;
}

ساكن بینښه skip_free_buckets(Hashmap_بنسټ *h, بینښه idx) {
        dib_raw_t *dibs;

        dibs = dib_raw_ptr(h);

        لكل ( ; idx < n_buckets(h); idx++)
                إذا (dibs[idx] != DIB_RAW_FREE)
                        أرجع idx;

        أرجع IDX_NIL;
}

ساكن فراغ لوښی_mark_free(Hashmap_بنسټ *h, بینښه idx) {
        أكد(h);

        memzero(لوښی_at(h, idx), hashmap_type_info[h->type].ثبت_اندازه);
        لوښی_سېټ_dib(h, idx, DIB_FREE);
}

ساكن فراغ لوښی_move_ثبت(Hashmap_بنسټ *h, بنية بدلون_entries *بدلون,
                              بینښه from, بینښه to) {
        بنية hashmap_بنسټ_ثبت *e_from, *e_منزل;

        أكد(h);
        أكد(from != to);

        e_from = لوښی_at_virtual(h, بدلون, from);
        e_منزل   = لوښی_at_virtual(h, بدلون, to);

        حافظه_نقل(e_منزل, e_from, hashmap_type_info[h->type].ثبت_اندازه);

        إذا (h->type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*) h;
                بنية ordered_hashmap_ثبت *le, *le_منزل;

                le_منزل = (بنية ordered_hashmap_ثبت*) e_منزل;

                إذا (le_منزل->iterate_بل != IDX_NIL) {
                        le = (بنية ordered_hashmap_ثبت*)
                             لوښی_at_virtual(h, بدلون, le_منزل->iterate_بل);
                        le->iterate_پخوانی = to;
                }

                إذا (le_منزل->iterate_پخوانی != IDX_NIL) {
                        le = (بنية ordered_hashmap_ثبت*)
                             لوښی_at_virtual(h, بدلون, le_منزل->iterate_پخوانی);
                        le->iterate_بل = to;
                }

                إذا (lh->iterate_list_سر == from)
                        lh->iterate_list_سر = to;
                إذا (lh->iterate_list_لکۍ == from)
                        lh->iterate_list_لکۍ = to;
        }
}

ساكن مضمن بینښه بل_idx(Hashmap_بنسټ *h, بینښه idx) {
        أرجع idx + 1U < n_buckets(h) ? idx + 1U : 0U;
}

ساكن بینښه prev_idx(Hashmap_بنسټ *h, بینښه idx) {
        أرجع idx > 0U ? idx - 1U : n_buckets(h) - 1U;
}

ساكن مضمن فراغ* ثبت_ارزښت(Hashmap_بنسټ *h, بنية hashmap_بنسټ_ثبت *e) {
        أكد(h);
        أكد(e);

        بدل (h->type) {

        حالة HASHMAP_TYPE_PLAIN:
        حالة HASHMAP_TYPE_ORDERED:
                أرجع ((بنية plain_hashmap_ثبت*)e)->ارزښت;

        حالة HASHMAP_TYPE_سېټ:
                أرجع (فراغ*) e->کلی;

        افتراضي:
                assert_not_reached();
        }
}

ساكن فراغ بنسټ_remove_ثبت(Hashmap_بنسټ *h, بینښه idx) {
        بینښه کیڼ, ښی, prev, dib;
        dib_raw_t raw_dib, *dibs;

        dibs = dib_raw_ptr(h);
        أكد(dibs[idx] != DIB_RAW_FREE);

#إذا ENABLE_DEBUG_HASHMAP
        أكد(h);
        h->debug.rem_شمېر++;
        h->debug.وروستی_rem_idx = idx;
#د_که_پای

        کیڼ = idx;
        /* Find the stop bucket ("right"). It is either free or has DIB == 0. */
        لكل (ښی = بل_idx(h, کیڼ); ; ښی = بل_idx(h, ښی)) {
                raw_dib = dibs[ښی];
                إذا (دننه_سېټ(raw_dib, 0, DIB_RAW_FREE))
                        اكسر;

                /* The buckets are not supposed to be all occupied and with DIB > 0.
                 * That would mean we could make everyone better off by shifting them
                 * backward. This scenario is impossible. */
                أكد(کیڼ != ښی);
        }

        إذا (h->type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*) h;
                بنية ordered_hashmap_ثبت *le = ordered_لوښی_at(lh, idx);

                إذا (le->iterate_بل != IDX_NIL)
                        ordered_لوښی_at(lh, le->iterate_بل)->iterate_پخوانی = le->iterate_پخوانی;
                وإلا
                        lh->iterate_list_لکۍ = le->iterate_پخوانی;

                إذا (le->iterate_پخوانی != IDX_NIL)
                        ordered_لوښی_at(lh, le->iterate_پخوانی)->iterate_بل = le->iterate_بل;
                وإلا
                        lh->iterate_list_سر = le->iterate_بل;
        }

        /* Now shift all buckets in the interval (left, right) one step backwards */
        لكل (prev = کیڼ, کیڼ = بل_idx(h, کیڼ); کیڼ != ښی;
             prev = کیڼ, کیڼ = بل_idx(h, کیڼ)) {
                dib = لوښی_calculate_dib(h, کیڼ, dibs[کیڼ]);
                أكد(dib != 0);
                لوښی_move_ثبت(h, عدم, کیڼ, prev);
                لوښی_سېټ_dib(h, prev, dib - 1);
        }

        لوښی_mark_free(h, prev);
        n_entries_dec(h);
        بنسټ_سېټ_dirty(h);
}
#تعریف_کړه remove_ثبت(h, idx) بنسټ_remove_ثبت(HASHMAP_بنسټ(h), idx)

ساكن بینښه hashmap_iterate_دننه_insertion_ترتیب(OrderedHashmap *h, Iterator *i) {
        بنية ordered_hashmap_ثبت *e;
        بینښه idx;

        أكد(h);
        أكد(i);

        إذا (i->idx == IDX_NIL)
                اذهب at_پای_ټکی;

        إذا (i->idx == IDX_لومړی && h->iterate_list_سر == IDX_NIL)
                اذهب at_پای_ټکی;

        إذا (i->idx == IDX_لومړی) {
                idx = h->iterate_list_سر;
                e = ordered_لوښی_at(h, idx);
        } وإلا {
                idx = i->idx;
                e = ordered_لوښی_at(h, idx);
                /*
                 * We allow removing the current entry while iterating, but removal may cause
                 * a backward shift. The next entry may thus move one bucket to the left.
                 * To detect when it happens, we remember the key pointer of the entry we were
                 * going to iterate next. If it does not match, there was a backward shift.
                 */
                إذا (e->p.b.کلی != i->بل_کلی) {
                        idx = prev_idx(HASHMAP_بنسټ(h), idx);
                        e = ordered_لوښی_at(h, idx);
                }
                أكد(e->p.b.کلی == i->بل_کلی);
        }

#إذا ENABLE_DEBUG_HASHMAP
        i->prev_idx = idx;
#د_که_پای

        إذا (e->iterate_بل != IDX_NIL) {
                بنية ordered_hashmap_ثبت *n;
                i->idx = e->iterate_بل;
                n = ordered_لوښی_at(h, i->idx);
                i->بل_کلی = n->p.b.کلی;
        } وإلا
                i->idx = IDX_NIL;

        أرجع idx;

at_پای_ټکی:
        i->idx = IDX_NIL;
        أرجع IDX_NIL;
}

ساكن بینښه hashmap_iterate_دننه_دننی_ترتیب(Hashmap_بنسټ *h, Iterator *i) {
        بینښه idx;

        أكد(h);
        أكد(i);

        إذا (i->idx == IDX_NIL)
                اذهب at_پای_ټکی;

        إذا (i->idx == IDX_لومړی) {
                /* fast forward to the first occupied bucket */
                إذا (h->has_غیرمستقیم) {
                        i->idx = skip_free_buckets(h, h->غیرمستقیم.idx_ټیټ_ترین_ثبت);
                        h->غیرمستقیم.idx_ټیټ_ترین_ثبت = i->idx;
                } وإلا
                        i->idx = skip_free_buckets(h, 0);

                إذا (i->idx == IDX_NIL)
                        اذهب at_پای_ټکی;
        } وإلا {
                بنية hashmap_بنسټ_ثبت *e;

                أكد(i->idx > 0);

                e = لوښی_at(h, i->idx);
                /*
                 * We allow removing the current entry while iterating, but removal may cause
                 * a backward shift. The next entry may thus move one bucket to the left.
                 * To detect when it happens, we remember the key pointer of the entry we were
                 * going to iterate next. If it does not match, there was a backward shift.
                 */
                إذا (e->کلی != i->بل_کلی)
                        e = لوښی_at(h, --i->idx);

                أكد(e->کلی == i->بل_کلی);
        }

        idx = i->idx;
#إذا ENABLE_DEBUG_HASHMAP
        i->prev_idx = idx;
#د_که_پای

        i->idx = skip_free_buckets(h, i->idx + 1);
        إذا (i->idx != IDX_NIL)
                i->بل_کلی = لوښی_at(h, i->idx)->کلی;
        وإلا
                i->idx = IDX_NIL;

        أرجع idx;

at_پای_ټکی:
        i->idx = IDX_NIL;
        أرجع IDX_NIL;
}

ساكن بینښه hashmap_iterate_ثبت(Hashmap_بنسټ *h, Iterator *i) {
        إذا (!h) {
                i->idx = IDX_NIL;
                أرجع IDX_NIL;
        }

#إذا ENABLE_DEBUG_HASHMAP
        إذا (i->idx == IDX_لومړی) {
                i->put_شمېر = h->debug.put_شمېر;
                i->rem_شمېر = h->debug.rem_شمېر;
        } وإلا {
                /* While iterating, must not add any new entries */
                أكد(i->put_شمېر == h->debug.put_شمېر);
                /* ... or remove entries other than the current one */
                أكد(i->rem_شمېر == h->debug.rem_شمېر ||
                       (i->rem_شمېر == h->debug.rem_شمېر - 1 &&
                        i->prev_idx == h->debug.وروستی_rem_idx));
                /* Reset our removals counter */
                i->rem_شمېر = h->debug.rem_شمېر;
        }
#د_که_پای

        أرجع h->type == HASHMAP_TYPE_ORDERED ? hashmap_iterate_دننه_insertion_ترتیب((OrderedHashmap*) h, i)
                                               : hashmap_iterate_دننه_دننی_ترتیب(h, i);
}

bool _hashmap_iterate(Hashmap_بنسټ *h, Iterator *i, فراغ **ارزښت, ثابت فراغ **کلی) {
        بنية hashmap_بنسټ_ثبت *e;
        فراغ *معلومات;
        بینښه idx;

        idx = hashmap_iterate_ثبت(h, i);
        إذا (idx == IDX_NIL) {
                إذا (ارزښت)
                        *ارزښت = عدم;
                إذا (کلی)
                        *کلی = عدم;

                أرجع false;
        }

        e = لوښی_at(h, idx);
        معلومات = ثبت_ارزښت(h, e);
        إذا (ارزښت)
                *ارزښت = معلومات;
        إذا (کلی)
                *کلی = e->کلی;

        أرجع true;
}

#تعریف_کړه HASHMAP_FOREACH_IDX(idx, h, i) \
        لكل ((i) = ITERATOR_لومړی, (idx) = hashmap_iterate_ثبت((h), &(i)); \
             (idx != IDX_NIL); \
             (idx) = hashmap_iterate_ثبت((h), &(i)))

IteratedCache* _hashmap_iterated_cache_new(Hashmap_بنسټ *h) {
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

ساكن فراغ reset_مستقیم_storage(Hashmap_بنسټ *h) {
        ثابت بنية hashmap_type_info *hi = &hashmap_type_info[h->type];
        فراغ *p;

        أكد(!h->has_غیرمستقیم);

        p = mempset(h->مستقیم.storage, 0, hi->ثبت_اندازه * hi->n_مستقیم_buckets);
        memset(p, DIB_RAW_INIT, حجم(dib_raw_t) * hi->n_مستقیم_buckets);
}

ساكن فراغ shared_hash_کلی_initialize(فراغ) {
        random_بایټونه(shared_hash_کلی, حجم(shared_hash_کلی));
}

ساكن بنية Hashmap_بنسټ* hashmap_بنسټ_new(ثابت بنية hash_ops *hash_ops, تعداد HashmapType type) {
        Hashmap_بنسټ *h;
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
                lh->iterate_list_سر = lh->iterate_list_لکۍ = IDX_NIL;
        }

        reset_مستقیم_storage(h);

        ساكن pthread_once_t once = PTHREAD_ONCE_INIT;
        assert_se(pthread_once(&once, shared_hash_کلی_initialize) == 0);

#إذا ENABLE_DEBUG_HASHMAP
        assert_se(pthread_mutex_lock(&hashmap_debug_list_mutex) == 0);
        LIST_PREPEND(debug_list, hashmap_debug_list, &h->debug);
        assert_se(pthread_mutex_unlock(&hashmap_debug_list_mutex) == 0);
#د_که_پای

        أرجع h;
}

Hashmap *hashmap_new(ثابت بنية hash_ops *hash_ops) {
        أرجع (Hashmap*)        hashmap_بنسټ_new(hash_ops, HASHMAP_TYPE_PLAIN);
}

OrderedHashmap *ordered_hashmap_new(ثابت بنية hash_ops *hash_ops) {
        أرجع (OrderedHashmap*) hashmap_بنسټ_new(hash_ops, HASHMAP_TYPE_ORDERED);
}

سېټ_2 *سېټ_new(ثابت بنية hash_ops *hash_ops) {
        أرجع (سېټ_2*)            hashmap_بنسټ_new(hash_ops, HASHMAP_TYPE_سېټ);
}

ساكن صحيح hashmap_بنسټ_ensure_allocated(Hashmap_بنسټ **h, ثابت بنية hash_ops *hash_ops,
                                         تعداد HashmapType type) {
        Hashmap_بنسټ *q;

        أكد(h);

        إذا (*h) {
                أكد((*h)->hash_ops == (hash_ops ?: &trivial_hash_ops));
                أرجع 0;
        }

        q = hashmap_بنسټ_new(hash_ops, type);
        إذا (!q)
                أرجع -ENOMEM;

        *h = q;
        أرجع 1;
}

صحيح hashmap_ensure_allocated(Hashmap **h, ثابت بنية hash_ops *hash_ops) {
        أرجع hashmap_بنسټ_ensure_allocated((Hashmap_بنسټ**)h, hash_ops, HASHMAP_TYPE_PLAIN);
}

صحيح ordered_hashmap_ensure_allocated(OrderedHashmap **h, ثابت بنية hash_ops *hash_ops) {
        أرجع hashmap_بنسټ_ensure_allocated((Hashmap_بنسټ**)h, hash_ops, HASHMAP_TYPE_ORDERED);
}

صحيح سېټ_ensure_allocated(سېټ_2 **s, ثابت بنية hash_ops *hash_ops) {
        أرجع hashmap_بنسټ_ensure_allocated((Hashmap_بنسټ**)s, hash_ops, HASHMAP_TYPE_سېټ);
}

صحيح hashmap_ensure_put(Hashmap **h, ثابت بنية hash_ops *hash_ops, ثابت فراغ *کلی, فراغ *ارزښت) {
        صحيح r;

        أكد(h);

        r = hashmap_ensure_allocated(h, hash_ops);
        إذا (r < 0)
                أرجع r;

        أرجع hashmap_put(*h, کلی, ارزښت);
}

صحيح ordered_hashmap_ensure_put(OrderedHashmap **h, ثابت بنية hash_ops *hash_ops, ثابت فراغ *کلی, فراغ *ارزښت) {
        صحيح r;

        أكد(h);

        r = ordered_hashmap_ensure_allocated(h, hash_ops);
        إذا (r < 0)
                أرجع r;

        أرجع ordered_hashmap_put(*h, کلی, ارزښت);
}

صحيح ordered_hashmap_ensure_replace(OrderedHashmap **h, ثابت بنية hash_ops *hash_ops, ثابت فراغ *کلی, فراغ *ارزښت) {
        صحيح r;

        أكد(h);

        r = ordered_hashmap_ensure_allocated(h, hash_ops);
        إذا (r < 0)
                أرجع r;

        أرجع ordered_hashmap_replace(*h, کلی, ارزښت);
}

صحيح hashmap_ensure_replace(Hashmap **h, ثابت بنية hash_ops *hash_ops, ثابت فراغ *کلی, فراغ *ارزښت) {
        صحيح r;

        أكد(h);

        r = hashmap_ensure_allocated(h, hash_ops);
        إذا (r < 0)
                أرجع r;

        أرجع hashmap_replace(*h, کلی, ارزښت);
}

ساكن فراغ hashmap_free_no_clear(Hashmap_بنسټ *h) {
        أكد(!h->has_غیرمستقیم);
        أكد(h->n_مستقیم_entries == 0);

#إذا ENABLE_DEBUG_HASHMAP
        assert_se(pthread_mutex_lock(&hashmap_debug_list_mutex) == 0);
        LIST_REMOVE(debug_list, hashmap_debug_list, &h->debug);
        assert_se(pthread_mutex_unlock(&hashmap_debug_list_mutex) == 0);
#د_که_پای

        إذا (h->from_pool) {
                /* Ensure that the object didn't get migrated between threads. */
                assert_se(is_main_thread());
                mempool_free_tile(hashmap_type_info[h->type].mempool, h);
        } وإلا
                حرر(h);
}

Hashmap_بنسټ* _hashmap_free(Hashmap_بنسټ *h) {
        إذا (h) {
                _hashmap_clear(h);
                hashmap_free_no_clear(h);
        }

        أرجع عدم;
}

فراغ _hashmap_clear(Hashmap_بنسټ *h) {
        إذا (!h)
                أرجع;

        إذا (h->hash_ops->free_کلی || h->hash_ops->free_ارزښت) {

                /* If destructor calls are defined, let's destroy things defensively: let's take the item out of the
                 * hash table, and only then call the destructor functions. If these destructors then try to unregister
                 * themselves from our hash table a second time, the entry is already gone. */

                طالما (hashmap_اندازه(h) > 0) {
                        فراغ *k = عدم;
                        فراغ *v;

                        v = hashmap_لومړی_کلی_and_ارزښت(h, true, &k);

                        إذا (h->hash_ops->free_کلی)
                                h->hash_ops->free_کلی(k);

                        إذا (h->hash_ops->free_ارزښت)
                                h->hash_ops->free_ارزښت(v);
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
                lh->iterate_list_سر = lh->iterate_list_لکۍ = IDX_NIL;
        }

        بنسټ_سېټ_dirty(h);
}

ساكن صحيح resize_buckets(Hashmap_بنسټ *h, بینښه entries_جمع_کړه);

/*
 * Finds an empty bucket to put an entry into, starting the scan at 'idx'.
 * Performs Robin Hood swaps as it goes. The entry to put must be placed
 * by the caller into swap slot IDX_PUT.
 * If used for in-place resizing, may leave a displaced entry in swap slot
 * IDX_PUT. Caller must rehash it next.
 * Returns: true if it left a displaced entry to rehash next in IDX_PUT,
 *          false otherwise.
 */
ساكن bool hashmap_put_robin_hood(Hashmap_بنسټ *h, بینښه idx,
                                   بنية بدلون_entries *بدلون) {
        dib_raw_t raw_dib, *dibs;
        بینښه dib, واټن;

#إذا ENABLE_DEBUG_HASHMAP
        أكد(h);
        h->debug.put_شمېر++;
#د_که_پای

        dibs = dib_raw_ptr(h);

        لكل (واټن = 0; ; واټن++) {
                raw_dib = dibs[idx];
                إذا (دننه_سېټ(raw_dib, DIB_RAW_FREE, DIB_RAW_REHASH)) {
                        إذا (raw_dib == DIB_RAW_REHASH)
                                لوښی_move_ثبت(h, بدلون, idx, IDX_TMP);

                        إذا (h->has_غیرمستقیم && h->غیرمستقیم.idx_ټیټ_ترین_ثبت > idx)
                                h->غیرمستقیم.idx_ټیټ_ترین_ثبت = idx;

                        لوښی_سېټ_dib(h, idx, واټن);
                        لوښی_move_ثبت(h, بدلون, IDX_PUT, idx);
                        إذا (raw_dib == DIB_RAW_REHASH) {
                                لوښی_move_ثبت(h, بدلون, IDX_TMP, IDX_PUT);
                                أرجع true;
                        }

                        أرجع false;
                }

                dib = لوښی_calculate_dib(h, idx, raw_dib);

                إذا (dib < واټن) {
                        /* Found a wealthier entry. Go Robin Hood! */
                        لوښی_سېټ_dib(h, idx, واټن);

                        /* swap the entries */
                        لوښی_move_ثبت(h, بدلون, idx, IDX_TMP);
                        لوښی_move_ثبت(h, بدلون, IDX_PUT, idx);
                        لوښی_move_ثبت(h, بدلون, IDX_TMP, IDX_PUT);

                        واټن = dib;
                }

                idx = بل_idx(h, idx);
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
ساكن صحيح hashmap_بنسټ_put_boldly(Hashmap_بنسټ *h, بینښه idx,
                                   بنية بدلون_entries *بدلون, bool may_resize) {
        بنية ordered_hashmap_ثبت *new_ثبت;
        صحيح r;

        أكد(idx < n_buckets(h));

        new_ثبت = لوښی_at_بدلون(بدلون, IDX_PUT);

        إذا (may_resize) {
                r = resize_buckets(h, 1);
                إذا (r < 0)
                        أرجع r;
                إذا (r > 0)
                        idx = لوښی_hash(h, new_ثبت->p.b.کلی);
        }
        أكد(n_entries(h) < n_buckets(h));

        إذا (h->type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*) h;

                new_ثبت->iterate_بل = IDX_NIL;
                new_ثبت->iterate_پخوانی = lh->iterate_list_لکۍ;

                إذا (lh->iterate_list_لکۍ != IDX_NIL) {
                        بنية ordered_hashmap_ثبت *old_لکۍ;

                        old_لکۍ = ordered_لوښی_at(lh, lh->iterate_list_لکۍ);
                        أكد(old_لکۍ->iterate_بل == IDX_NIL);
                        old_لکۍ->iterate_بل = IDX_PUT;
                }

                lh->iterate_list_لکۍ = IDX_PUT;
                إذا (lh->iterate_list_سر == IDX_NIL)
                        lh->iterate_list_سر = IDX_PUT;
        }

        assert_se(hashmap_put_robin_hood(h, idx, بدلون) == false);

        n_entries_inc(h);
#إذا ENABLE_DEBUG_HASHMAP
        h->debug.max_entries = MAX(h->debug.max_entries, n_entries(h));
#د_که_پای

        بنسټ_سېټ_dirty(h);

        أرجع 1;
}
#تعریف_کړه hashmap_put_boldly(h, idx, بدلون, may_resize) \
        hashmap_بنسټ_put_boldly(HASHMAP_بنسټ(h), idx, بدلون, may_resize)

/*
 * Returns 0 if resize is not needed.
 *         1 if successfully resized.
 *         -ENOMEM on allocation failure.
 */
ساكن صحيح resize_buckets(Hashmap_بنسټ *h, بینښه entries_جمع_کړه) {
        بنية بدلون_entries بدلون;
        فراغ *new_storage;
        dib_raw_t *old_dibs, *new_dibs;
        ثابت بنية hashmap_type_info *hi;
        بینښه idx, optimal_idx;
        بینښه old_n_buckets, new_n_buckets, n_rehashed, new_n_entries;
        uint8_t new_ښويدنه;
        bool rehash_بل;

        أكد(h);

        hi = &hashmap_type_info[h->type];
        new_n_entries = n_entries(h) + entries_جمع_کړه;

        /* overflow? */
        إذا (_unlikely_(new_n_entries < entries_جمع_کړه))
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

        إذا (_unlikely_(new_n_buckets > UINT_MAX / (hi->ثبت_اندازه + حجم(dib_raw_t))))
                أرجع -ENOMEM;

        new_ښويدنه = log2u_پړاو_پورته(MAX(
                        new_n_buckets * (hi->ثبت_اندازه + حجم(dib_raw_t)),
                        2 * حجم(بنية مستقیم_storage)));

        /* Realloc storage (buckets and DIB array). */
        new_storage = realloc(h->has_غیرمستقیم ? h->غیرمستقیم.storage : عدم,
                              1U << new_ښويدنه);
        إذا (!new_storage)
                أرجع -ENOMEM;

        /* Must upgrade direct to indirect storage. */
        إذا (!h->has_غیرمستقیم) {
                حافظه_نقل(new_storage, h->مستقیم.storage,
                       old_n_buckets * (hi->ثبت_اندازه + حجم(dib_raw_t)));
                h->غیرمستقیم.n_entries = h->n_مستقیم_entries;
                h->غیرمستقیم.idx_ټیټ_ترین_ثبت = 0;
                h->n_مستقیم_entries = 0;
        }

        /* Get a new hash key. If we've just upgraded to indirect storage,
         * allow reusing a previously generated key. It's still a different key
         * from the shared one that we used for direct storage. */
        get_hash_کلی(h->غیرمستقیم.hash_کلی, !h->has_غیرمستقیم);

        h->has_غیرمستقیم = true;
        h->غیرمستقیم.storage = new_storage;
        h->غیرمستقیم.n_buckets = (1U << new_ښويدنه) /
                                (hi->ثبت_اندازه + حجم(dib_raw_t));

        old_dibs = (dib_raw_t*)((uint8_t*) new_storage + hi->ثبت_اندازه * old_n_buckets);
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
        memzero(لوښی_at(h, old_n_buckets),
               (n_buckets(h) - old_n_buckets) * hi->ثبت_اندازه);

        /* The upper half of the new DIB array needs initialization */
        memset(&new_dibs[old_n_buckets], DIB_RAW_INIT,
               (n_buckets(h) - old_n_buckets) * حجم(dib_raw_t));

        /* Rehash entries that need it */
        n_rehashed = 0;
        لكل (idx = 0; idx < old_n_buckets; idx++) {
                إذا (new_dibs[idx] != DIB_RAW_REHASH)
                        استمر;

                optimal_idx = لوښی_hash(h, لوښی_at(h, idx)->کلی);

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
                لوښی_move_ثبت(h, &بدلون, idx, IDX_PUT);
                /* bucket_move_entry does not clear the source */
                memzero(لوښی_at(h, idx), hi->ثبت_اندازه);

                افعل {
                        /*
                         * Find the new bucket for the current entry. This may make
                         * another entry homeless and load it into IDX_PUT.
                         */
                        rehash_بل = hashmap_put_robin_hood(h, optimal_idx, &بدلون);
                        n_rehashed++;

                        /* Did the current entry displace another one? */
                        إذا (rehash_بل)
                                optimal_idx = لوښی_hash(h, لوښی_at_بدلون(&بدلون, IDX_PUT)->p.b.کلی);
                } طالما (rehash_بل);
        }

        assert_se(n_rehashed == n_entries(h));

        أرجع 1;
}

/*
 * Finds an entry with a matching key
 * Returns: index of the found entry, or IDX_NIL if not found.
 */
ساكن بینښه بنسټ_لوښی_لټون(Hashmap_بنسټ *h, بینښه idx, ثابت فراغ *کلی) {
        compare_func_t compare = h->hash_ops->compare;
        بنية hashmap_بنسټ_ثبت *e;
        بینښه dib, واټن;
        dib_raw_t *dibs = dib_raw_ptr(h);

        أكد(idx < n_buckets(h));

        لكل (واټن = 0; ; واټن++) {
                إذا (dibs[idx] == DIB_RAW_FREE)
                        أرجع IDX_NIL;

                dib = لوښی_calculate_dib(h, idx, dibs[idx]);

                إذا (dib < واټن)
                        أرجع IDX_NIL;
                إذا (dib == واټن) {
                        e = لوښی_at(h, idx);

                        /* Most maps compare keys by pointer. Do that here, without the indirect call. */
                        إذا (compare == trivial_compare_func ? e->کلی == کلی : compare(e->کلی, کلی) == 0)
                                أرجع idx;
                }

                idx = بل_idx(h, idx);
        }
}
#تعریف_کړه لوښی_لټون(h, idx, کلی) بنسټ_لوښی_لټون(HASHMAP_بنسټ(h), idx, کلی)

صحيح hashmap_put(Hashmap *h, ثابت فراغ *کلی, فراغ *ارزښت) {
        بنية بدلون_entries بدلون;
        بنية plain_hashmap_ثبت *e;
        بینښه hash, idx;

        أكد(h);

        hash = لوښی_hash(h, کلی);
        idx = لوښی_لټون(h, hash, کلی);
        إذا (idx != IDX_NIL) {
                e = plain_لوښی_at(h, idx);
                إذا (e->ارزښت == ارزښت)
                        أرجع 0;
                أرجع -EEXIST;
        }

        e = &لوښی_at_بدلون(&بدلون, IDX_PUT)->p;
        e->b.کلی = کلی;
        e->ارزښت = ارزښت;
        أرجع hashmap_put_boldly(h, hash, &بدلون, true);
}

صحيح سېټ_put(سېټ_2 *s, ثابت فراغ *کلی) {
        بنية بدلون_entries بدلون;
        بنية hashmap_بنسټ_ثبت *e;
        بینښه hash, idx;

        أكد(s);

        hash = لوښی_hash(s, کلی);
        idx = لوښی_لټون(s, hash, کلی);
        إذا (idx != IDX_NIL)
                أرجع 0;

        e = &لوښی_at_بدلون(&بدلون, IDX_PUT)->p.b;
        e->کلی = کلی;
        أرجع hashmap_put_boldly(s, hash, &بدلون, true);
}

صحيح سېټ_ensure_put(سېټ_2 **s, ثابت بنية hash_ops *hash_ops, ثابت فراغ *کلی) {
        صحيح r;

        أكد(s);

        r = سېټ_ensure_allocated(s, hash_ops);
        إذا (r < 0)
                أرجع r;

        أرجع سېټ_put(*s, کلی);
}

صحيح سېټ_ensure_consume(سېټ_2 **s, ثابت بنية hash_ops *hash_ops, فراغ *کلی) {
        صحيح r;

        r = سېټ_ensure_put(s, hash_ops, کلی);
        إذا (r <= 0) {
                إذا (hash_ops && hash_ops->free_کلی)
                        hash_ops->free_کلی(کلی);
                وإلا إذا (hash_ops && hash_ops->free_ارزښت)
                        /* Sets store their element in the key slot but may carry a value destructor. */
                        hash_ops->free_ارزښت(کلی);
                وإلا
                        حرر(کلی);
        }

        أرجع r;
}

صحيح hashmap_replace(Hashmap *h, ثابت فراغ *کلی, فراغ *ارزښت) {
        بنية بدلون_entries بدلون;
        بنية plain_hashmap_ثبت *e;
        بینښه hash, idx;

        أكد(h);

        hash = لوښی_hash(h, کلی);
        idx = لوښی_لټون(h, hash, کلی);
        إذا (idx != IDX_NIL) {
                e = plain_لوښی_at(h, idx);
#إذا ENABLE_DEBUG_HASHMAP
                /* Although the key is equal, the key pointer may have changed,
                 * and this would break our assumption for iterating. So count
                 * this operation as incompatible with iteration. */
                إذا (e->b.کلی != کلی) {
                        h->b.debug.put_شمېر++;
                        h->b.debug.rem_شمېر++;
                        h->b.debug.وروستی_rem_idx = idx;
                }
#د_که_پای
                e->b.کلی = کلی;
                e->ارزښت = ارزښت;
                hashmap_سېټ_dirty(h);

                أرجع 0;
        }

        e = &لوښی_at_بدلون(&بدلون, IDX_PUT)->p;
        e->b.کلی = کلی;
        e->ارزښت = ارزښت;
        أرجع hashmap_put_boldly(h, hash, &بدلون, true);
}

صحيح hashmap_update(Hashmap *h, ثابت فراغ *کلی, فراغ *ارزښت) {
        بنية plain_hashmap_ثبت *e;
        بینښه hash, idx;

        أكد(h);

        hash = لوښی_hash(h, کلی);
        idx = لوښی_لټون(h, hash, کلی);
        إذا (idx == IDX_NIL)
                أرجع -ENOENT;

        e = plain_لوښی_at(h, idx);
        e->ارزښت = ارزښت;
        hashmap_سېټ_dirty(h);

        أرجع 0;
}

فراغ* _hashmap_get(Hashmap_بنسټ *h, ثابت فراغ *کلی) {
        بنية hashmap_بنسټ_ثبت *e;
        بینښه hash, idx;

        إذا (!h)
                أرجع عدم;

        hash = لوښی_hash(h, کلی);
        idx = لوښی_لټون(h, hash, کلی);
        إذا (idx == IDX_NIL)
                أرجع عدم;

        e = لوښی_at(h, idx);
        أرجع ثبت_ارزښت(h, e);
}

فراغ* hashmap_get2(Hashmap *h, ثابت فراغ *کلی, فراغ **ret) {
        بنية plain_hashmap_ثبت *e;
        بینښه hash, idx;

        إذا (!h)
                أرجع عدم;

        hash = لوښی_hash(h, کلی);
        idx = لوښی_لټون(h, hash, کلی);
        إذا (idx == IDX_NIL)
                أرجع عدم;

        e = plain_لوښی_at(h, idx);
        إذا (ret)
                *ret = (فراغ*) e->b.کلی;

        أرجع e->ارزښت;
}

bool hashmap_لري_2(Hashmap_بنسټ *h, ثابت فراغ *کلی) {
        بینښه hash;

        إذا (!h)
                أرجع false;

        hash = لوښی_hash(h, کلی);
        أرجع لوښی_لټون(h, hash, کلی) != IDX_NIL;
}

فراغ* _hashmap_remove(Hashmap_بنسټ *h, ثابت فراغ *کلی) {
        بنية hashmap_بنسټ_ثبت *e;
        بینښه hash, idx;
        فراغ *معلومات;

        إذا (!h)
                أرجع عدم;

        hash = لوښی_hash(h, کلی);
        idx = لوښی_لټون(h, hash, کلی);
        إذا (idx == IDX_NIL)
                أرجع عدم;

        e = لوښی_at(h, idx);
        معلومات = ثبت_ارزښت(h, e);
        remove_ثبت(h, idx);

        أرجع معلومات;
}

فراغ* hashmap_remove2(Hashmap *h, ثابت فراغ *کلی, فراغ **ret) {
        بنية plain_hashmap_ثبت *e;
        بینښه hash, idx;
        فراغ *معلومات;

        إذا (!h) {
                إذا (ret)
                        *ret = عدم;
                أرجع عدم;
        }

        hash = لوښی_hash(h, کلی);
        idx = لوښی_لټون(h, hash, کلی);
        إذا (idx == IDX_NIL) {
                إذا (ret)
                        *ret = عدم;
                أرجع عدم;
        }

        e = plain_لوښی_at(h, idx);
        معلومات = e->ارزښت;
        إذا (ret)
                *ret = (فراغ*) e->b.کلی;

        remove_ثبت(h, idx);

        أرجع معلومات;
}

صحيح hashmap_remove_and_put(Hashmap *h, ثابت فراغ *old_کلی, ثابت فراغ *new_کلی, فراغ *ارزښت) {
        بنية بدلون_entries بدلون;
        بنية plain_hashmap_ثبت *e;
        بینښه old_hash, new_hash, idx;

        إذا (!h)
                أرجع -ENOENT;

        old_hash = لوښی_hash(h, old_کلی);
        idx = لوښی_لټون(h, old_hash, old_کلی);
        إذا (idx == IDX_NIL)
                أرجع -ENOENT;

        new_hash = لوښی_hash(h, new_کلی);
        إذا (لوښی_لټون(h, new_hash, new_کلی) != IDX_NIL)
                أرجع -EEXIST;

        remove_ثبت(h, idx);

        e = &لوښی_at_بدلون(&بدلون, IDX_PUT)->p;
        e->b.کلی = new_کلی;
        e->ارزښت = ارزښت;
        assert_se(hashmap_put_boldly(h, new_hash, &بدلون, false) == 1);

        أرجع 0;
}

صحيح سېټ_remove_and_put(سېټ_2 *s, ثابت فراغ *old_کلی, ثابت فراغ *new_کلی) {
        بنية بدلون_entries بدلون;
        بنية hashmap_بنسټ_ثبت *e;
        بینښه old_hash, new_hash, idx;

        إذا (!s)
                أرجع -ENOENT;

        old_hash = لوښی_hash(s, old_کلی);
        idx = لوښی_لټون(s, old_hash, old_کلی);
        إذا (idx == IDX_NIL)
                أرجع -ENOENT;

        new_hash = لوښی_hash(s, new_کلی);
        إذا (لوښی_لټون(s, new_hash, new_کلی) != IDX_NIL)
                أرجع -EEXIST;

        remove_ثبت(s, idx);

        e = &لوښی_at_بدلون(&بدلون, IDX_PUT)->p.b;
        e->کلی = new_کلی;
        assert_se(hashmap_put_boldly(s, new_hash, &بدلون, false) == 1);

        أرجع 0;
}

صحيح hashmap_remove_and_replace(Hashmap *h, ثابت فراغ *old_کلی, ثابت فراغ *new_کلی, فراغ *ارزښت) {
        بنية بدلون_entries بدلون;
        بنية plain_hashmap_ثبت *e;
        بینښه old_hash, new_hash, idx_old, idx_new;

        إذا (!h)
                أرجع -ENOENT;

        old_hash = لوښی_hash(h, old_کلی);
        idx_old = لوښی_لټون(h, old_hash, old_کلی);
        إذا (idx_old == IDX_NIL)
                أرجع -ENOENT;

        old_کلی = لوښی_at(HASHMAP_بنسټ(h), idx_old)->کلی;

        new_hash = لوښی_hash(h, new_کلی);
        idx_new = لوښی_لټون(h, new_hash, new_کلی);
        إذا (idx_new != IDX_NIL)
                إذا (idx_old != idx_new) {
                        remove_ثبت(h, idx_new);
                        /* Compensate for a possible backward shift. */
                        إذا (old_کلی != لوښی_at(HASHMAP_بنسټ(h), idx_old)->کلی)
                                idx_old = prev_idx(HASHMAP_بنسټ(h), idx_old);
                        أكد(old_کلی == لوښی_at(HASHMAP_بنسټ(h), idx_old)->کلی);
                }

        remove_ثبت(h, idx_old);

        e = &لوښی_at_بدلون(&بدلون, IDX_PUT)->p;
        e->b.کلی = new_کلی;
        e->ارزښت = ارزښت;
        assert_se(hashmap_put_boldly(h, new_hash, &بدلون, false) == 1);

        أرجع 0;
}

فراغ* hashmap_remove_ارزښت(Hashmap_بنسټ *h, ثابت فراغ *کلی, فراغ *ارزښت) {
        بنية hashmap_بنسټ_ثبت *e;
        بینښه hash, idx;

        إذا (!h)
                أرجع عدم;

        hash = لوښی_hash(h, کلی);
        idx = لوښی_لټون(h, hash, کلی);
        إذا (idx == IDX_NIL)
                أرجع عدم;

        e = لوښی_at(h, idx);
        إذا (ثبت_ارزښت(h, e) != ارزښت)
                أرجع عدم;

        remove_ثبت(h, idx);

        أرجع ارزښت;
}

ساكن بینښه پیدا_کړه_لومړی_ثبت(Hashmap_بنسټ *h) {
        Iterator i = ITERATOR_لومړی;

        إذا (!h || !n_entries(h))
                أرجع IDX_NIL;

        أرجع hashmap_iterate_ثبت(h, &i);
}

فراغ* hashmap_لومړی_کلی_and_ارزښت(Hashmap_بنسټ *h, bool remove, فراغ **ret_کلی) {
        بنية hashmap_بنسټ_ثبت *e;
        فراغ *کلی, *معلومات;
        بینښه idx;

        idx = پیدا_کړه_لومړی_ثبت(h);
        إذا (idx == IDX_NIL) {
                إذا (ret_کلی)
                        *ret_کلی = عدم;
                أرجع عدم;
        }

        e = لوښی_at(h, idx);
        کلی = (فراغ*) e->کلی;
        معلومات = ثبت_ارزښت(h, e);

        إذا (remove)
                remove_ثبت(h, idx);

        إذا (ret_کلی)
                *ret_کلی = کلی;

        أرجع معلومات;
}

بینښه hashmap_اندازه(Hashmap_بنسټ *h) {
        إذا (!h)
                أرجع 0;

        أرجع n_entries(h);
}

بینښه _hashmap_buckets(Hashmap_بنسټ *h) {
        إذا (!h)
                أرجع 0;

        أرجع n_buckets(h);
}

صحيح hashmap_یوځای(Hashmap *h, Hashmap *other) {
        Iterator i;
        بینښه idx;

        أكد(h);

        HASHMAP_FOREACH_IDX(idx, HASHMAP_بنسټ(other), i) {
                بنية plain_hashmap_ثبت *pe = plain_لوښی_at(other, idx);
                صحيح r;

                r = hashmap_put(h, pe->b.کلی, pe->ارزښت);
                إذا (r < 0 && r != -EEXIST)
                        أرجع r;
        }

        أرجع 0;
}

صحيح سېټ_یوځای(سېټ_2 *s, سېټ_2 *other) {
        Iterator i;
        بینښه idx;

        أكد(s);

        HASHMAP_FOREACH_IDX(idx, HASHMAP_بنسټ(other), i) {
                بنية سېټ_ثبت *se = سېټ_لوښی_at(other, idx);
                صحيح r;

                r = سېټ_put(s, se->b.کلی);
                إذا (r < 0)
                        أرجع r;
        }

        أرجع 0;
}

صحيح _hashmap_reserve(Hashmap_بنسټ *h, بینښه entries_جمع_کړه) {
        صحيح r;

        أكد(h);

        r = resize_buckets(h, entries_جمع_کړه);
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
صحيح _hashmap_move(Hashmap_بنسټ *h, Hashmap_بنسټ *other) {
        بنية بدلون_entries بدلون;
        بنية hashmap_بنسټ_ثبت *e, *n;
        Iterator i;
        بینښه idx;
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
                بینښه h_hash;

                e = لوښی_at(other, idx);
                h_hash = لوښی_hash(h, e->کلی);
                إذا (لوښی_لټون(h, h_hash, e->کلی) != IDX_NIL)
                        استمر;

                n = &لوښی_at_بدلون(&بدلون, IDX_PUT)->p.b;
                n->کلی = e->کلی;
                إذا (h->type != HASHMAP_TYPE_سېټ)
                        ((بنية plain_hashmap_ثبت*) n)->ارزښت =
                                ((بنية plain_hashmap_ثبت*) e)->ارزښت;
                assert_se(hashmap_put_boldly(h, h_hash, &بدلون, false) == 1);

                remove_ثبت(other, idx);
        }

        أرجع 0;
}

صحيح _hashmap_move_one(Hashmap_بنسټ *h, Hashmap_بنسټ *other, ثابت فراغ *کلی) {
        بنية بدلون_entries بدلون;
        بینښه h_hash, other_hash, idx;
        بنية hashmap_بنسټ_ثبت *e, *n;
        صحيح r;

        أكد(h);

        h_hash = لوښی_hash(h, کلی);
        إذا (لوښی_لټون(h, h_hash, کلی) != IDX_NIL)
                أرجع -EEXIST;

        إذا (!other)
                أرجع -ENOENT;

        أكد(other->type == h->type);

        other_hash = لوښی_hash(other, کلی);
        idx = لوښی_لټون(other, other_hash, کلی);
        إذا (idx == IDX_NIL)
                أرجع -ENOENT;

        e = لوښی_at(other, idx);

        n = &لوښی_at_بدلون(&بدلون, IDX_PUT)->p.b;
        n->کلی = e->کلی;
        إذا (h->type != HASHMAP_TYPE_سېټ)
                ((بنية plain_hashmap_ثبت*) n)->ارزښت =
                        ((بنية plain_hashmap_ثبت*) e)->ارزښت;
        r = hashmap_put_boldly(h, h_hash, &بدلون, true);
        إذا (r < 0)
                أرجع r;

        remove_ثبت(other, idx);
        أرجع 0;
}

Hashmap_بنسټ* _hashmap_copy(Hashmap_بنسټ *h) {
        Hashmap_بنسټ *copy;
        صحيح r;

        أكد(h);

        copy = hashmap_بنسټ_new(h->hash_ops, h->type);
        إذا (!copy)
                أرجع عدم;

        بدل (h->type) {
        حالة HASHMAP_TYPE_PLAIN:
        حالة HASHMAP_TYPE_ORDERED:
                r = hashmap_یوځای_2((Hashmap*)copy, (Hashmap*)h);
                اكسر;
        حالة HASHMAP_TYPE_سېټ:
                r = سېټ_یوځای((سېټ_2*)copy, (سېټ_2*)h);
                اكسر;
        افتراضي:
                assert_not_reached();
        }

        إذا (r < 0)
                أرجع _hashmap_free(copy);

        أرجع copy;
}

حرف** _hashmap_get_strv(Hashmap_بنسټ *h) {
        حرف **sv;
        Iterator i;
        بینښه idx, n;

        إذا (!h)
                أرجع new0(حرف*, 1);

        sv = new(حرف*, n_entries(h)+1);
        إذا (!sv)
                أرجع عدم;

        n = 0;
        HASHMAP_FOREACH_IDX(idx, h, i)
                sv[n++] = ثبت_ارزښت(h, لوښی_at(h, idx));
        sv[n] = عدم;

        أرجع sv;
}

حرف** سېټ_منزل_strv(سېټ_2 **s) {
        أكد(s);

        /* This is similar to set_get_strv(), but invalidates the set on success. */

        حرف **v = new(حرف*, سېټ_اندازه(*s) + 1);
        إذا (!v)
                أرجع عدم;

        لكل (حرف **p = v; (*p = سېټ_steal_لومړی(*s)); p++)
                ;

        أكد(سېټ_isempty(*s));
        *s = سېټ_free(*s);
        أرجع v;
}

فراغ* ordered_hashmap_بل(OrderedHashmap *h, ثابت فراغ *کلی) {
        بنية ordered_hashmap_ثبت *e;
        بینښه hash, idx;

        إذا (!h)
                أرجع عدم;

        hash = لوښی_hash(h, کلی);
        idx = لوښی_لټون(h, hash, کلی);
        إذا (idx == IDX_NIL)
                أرجع عدم;

        e = ordered_لوښی_at(h, idx);
        إذا (e->iterate_بل == IDX_NIL)
                أرجع عدم;
        أرجع ordered_لوښی_at(h, e->iterate_بل)->p.ارزښت;
}

صحيح سېټ_consume(سېټ_2 *s, فراغ *ارزښت) {
        صحيح r;

        أكد(s);
        أكد(ارزښت);

        r = سېټ_put(s, ارزښت);
        إذا (r <= 0)
                حرر(ارزښت);

        أرجع r;
}

صحيح hashmap_put_strdup_ډک(Hashmap **h, ثابت بنية hash_ops *hash_ops, ثابت حرف *k, ثابت حرف *v) {
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

صحيح سېټ_put_strndup_ډک(سېټ_2 **s, ثابت بنية hash_ops *hash_ops, ثابت حرف *p, size_t n) {
        حرف *c;
        صحيح r;

        أكد(s);
        أكد(p);

        r = سېټ_ensure_allocated(s, hash_ops);
        إذا (r < 0)
                أرجع r;

        إذا (n == SIZE_MAX) {
                إذا (سېټ_لري(*s, (حرف*) p))
                        أرجع 0;

                c = strdup(p);
        } وإلا
                c = strndup(p, n);
        إذا (!c)
                أرجع -ENOMEM;

        أرجع سېټ_consume(*s, c);
}

صحيح سېټ_put_strdupv_ډک(سېټ_2 **s, ثابت بنية hash_ops *hash_ops, حرف **l) {
        صحيح n = 0, r;

        أكد(s);

        STRV_FOREACH(i, l) {
                r = سېټ_put_strndup_ډک(s, hash_ops, *i, SIZE_MAX);
                إذا (r < 0)
                        أرجع r;

                n += r;
        }

        أرجع n;
}

صحيح سېټ_put_strsplit(سېټ_2 *s, ثابت حرف *v, ثابت حرف *separators, ایستل_Flags flags) {
        ثابت حرف *p = ASSERT_PTR(v);
        صحيح r;

        أكد(s);

        لكل (;;) {
                حرف *کلمه;

                r = ایستل_لومړی_کلمه(&p, &کلمه, separators, flags);
                إذا (r <= 0)
                        أرجع r;

                r = سېټ_consume(s, کلمه);
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

صحيح iterated_cache_get(IteratedCache *cache, ثابت فراغ ***res_کلیګانې, ثابت فراغ ***res_ارزښتونه, بینښه *res_n_entries) {
        bool sync_کلیګانې = false, sync_ارزښتونه = false;
        size_t اندازه;
        صحيح r;

        أكد(cache);
        أكد(cache->hashmap);

        اندازه = n_entries(cache->hashmap);

        إذا (res_کلیګانې) {
                r = cachemem_maintain(&cache->کلیګانې, اندازه);
                إذا (r < 0)
                        أرجع r;

                sync_کلیګانې = r;
        } وإلا
                cache->کلیګانې.فعال = false;

        إذا (res_ارزښتونه) {
                r = cachemem_maintain(&cache->values, اندازه);
                إذا (r < 0)
                        أرجع r;

                sync_ارزښتونه = r;
        } وإلا
                cache->values.فعال = false;

        إذا (cache->hashmap->dirty) {
                إذا (cache->کلیګانې.فعال)
                        sync_کلیګانې = true;
                إذا (cache->values.فعال)
                        sync_ارزښتونه = true;

                cache->hashmap->dirty = false;
        }

        إذا (sync_کلیګانې || sync_ارزښتونه) {
                بینښه i, idx;
                Iterator iter;

                i = 0;
                HASHMAP_FOREACH_IDX(idx, cache->hashmap, iter) {
                        بنية hashmap_بنسټ_ثبت *e;

                        e = لوښی_at(cache->hashmap, idx);

                        إذا (sync_کلیګانې)
                                cache->کلیګانې.ptr[i] = e->کلی;
                        إذا (sync_ارزښتونه)
                                cache->values.ptr[i] = ثبت_ارزښت(cache->hashmap, e);
                        i++;
                }
        }

        إذا (res_کلیګانې)
                *res_کلیګانې = cache->کلیګانې.ptr;
        إذا (res_ارزښتونه)
                *res_ارزښتونه = cache->values.ptr;
        إذا (res_n_entries)
                *res_n_entries = اندازه;

        أرجع 0;
}

IteratedCache* iterated_cache_free(IteratedCache *cache) {
        إذا (cache) {
                حرر(cache->کلیګانې.ptr);
                حرر(cache->values.ptr);
        }

        أرجع mfree(cache);
}

صحيح سېټ_strjoin(سېټ_2 *s, ثابت حرف *separator, bool wrap_with_separator, حرف **ret) {
        _cleanup_free_ حرف *str = عدم;
        size_t separator_len, len = 0;
        ثابت حرف *ارزښت;
        bool لومړی;

        أكد(ret);

        إذا (سېټ_isempty(s)) {
                *ret = عدم;
                أرجع 0;
        }

        separator_len = strlen_ptr(separator);

        إذا (separator_len == 0)
                wrap_with_separator = false;

        لومړی = !wrap_with_separator;

        سېټ_FOREACH(ارزښت, s) {
                size_t l = strlen_ptr(ارزښت);

                إذا (l == 0)
                        استمر;

                إذا (!GREEDY_REALLOC(str, len + l + (لومړی ? 0 : separator_len) + (wrap_with_separator ? separator_len : 0) + 1))
                        أرجع -ENOMEM;

                إذا (separator_len > 0 && !لومړی) {
                        حافظه_نقل(str + len, separator, separator_len);
                        len += separator_len;
                }

                حافظه_نقل(str + len, ارزښت, l);
                len += l;
                لومړی = false;
        }

        إذا (wrap_with_separator) {
                حافظه_نقل(str + len, separator, separator_len);
                len += separator_len;
        }

        str[len] = '\0';

        *ret = TAKE_PTR(str);
        أرجع 0;
}

bool سېټ_equal(سېټ_2 *a, سېټ_2 *b) {
        فراغ *p;

        /* Checks whether each entry of 'a' is also in 'b' and vice versa, i.e. the two sets contain the same
         * entries */

        إذا (a == b)
                أرجع true;

        إذا (سېټ_isempty(a) && سېټ_isempty(b))
                أرجع true;

        إذا (سېټ_اندازه(a) != سېټ_اندازه(b)) /* Cheap check that hopefully catches a lot of inequality cases
                                         * already */
                أرجع false;

        سېټ_FOREACH(p, a)
                إذا (!سېټ_لري(b, p))
                        أرجع false;

        /* If we have the same hashops, then we don't need to check things backwards given we compared the
         * size and that all of a is in b. */
        إذا (a->b.hash_ops == b->b.hash_ops)
                أرجع true;

        سېټ_FOREACH(p, b)
                إذا (!سېټ_لري(a, p))
                        أرجع false;

        أرجع true;
}

ساكن bool سېټ_fnmatch_one(سېټ_2 *patterns, ثابت حرف *needle) {
        ثابت حرف *p;

        أكد(needle);

        /* Any failure of fnmatch() is treated as equivalent to FNM_NOMATCH, i.e. as non-matching pattern */

        سېټ_FOREACH(p, patterns)
                إذا (fnmatch(p, needle, 0) == 0)
                        أرجع true;

        أرجع false;
}

bool سېټ_fnmatch(سېټ_2 *include_patterns, سېټ_2 *exclude_patterns, ثابت حرف *needle) {
        أكد(needle);

        إذا (سېټ_fnmatch_one(exclude_patterns, needle))
                أرجع false;

        إذا (سېټ_isempty(include_patterns))
                أرجع true;

        أرجع سېټ_fnmatch_one(include_patterns, needle);
}

ساكن صحيح hashmap_ثبت_compare(
                بنية hashmap_بنسټ_ثبت * ثابت *a,
                بنية hashmap_بنسټ_ثبت * ثابت *b,
                compare_func_t compare) {

        أكد(a && *a);
        أكد(b && *b);
        أكد(compare);

        أرجع compare((*a)->کلی, (*b)->کلی);
}

ساكن صحيح _hashmap_dump_entries_sorted(
                Hashmap_بنسټ *h,
                فراغ ***ret,
                size_t *ret_n) {
        _cleanup_free_ فراغ **entries = عدم;
        Iterator iter;
        بینښه idx;
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
                entries[n++] = لوښی_at(h, idx);

        أكد(n == hashmap_اندازه(h));
        entries[n] = عدم;

        typesafe_qsort_r((بنية hashmap_بنسټ_ثبت**) entries, n,
                         hashmap_ثبت_compare, h->hash_ops->compare);

        *ret = TAKE_PTR(entries);
        *ret_n = n;
        أرجع 0;
}

صحيح hashmap_dump_کلیګانې_sorted(Hashmap_بنسټ *h, فراغ ***ret, size_t *ret_n) {
        _cleanup_free_ فراغ **entries = عدم;
        size_t n;
        صحيح r;

        أكد(ret);

        r = _hashmap_dump_entries_sorted(h, &entries, &n);
        إذا (r < 0)
                أرجع r;

        /* Reuse the array. */
        FOREACH_ARRAY(e, entries, n)
                *e = (فراغ*) (*(بنية hashmap_بنسټ_ثبت**) e)->کلی;

        *ret = TAKE_PTR(entries);
        إذا (ret_n)
                *ret_n = n;
        أرجع 0;
}

صحيح _hashmap_dump_sorted(Hashmap_بنسټ *h, فراغ ***ret, size_t *ret_n) {
        _cleanup_free_ فراغ **entries = عدم;
        size_t n;
        صحيح r;

        أكد(ret);

        r = _hashmap_dump_entries_sorted(h, &entries, &n);
        إذا (r < 0)
                أرجع r;

        /* Reuse the array. */
        FOREACH_ARRAY(e, entries, n)
                *e = ثبت_ارزښت(h, *(بنية hashmap_بنسټ_ثبت**) e);

        *ret = TAKE_PTR(entries);
        إذا (ret_n)
                *ret_n = n;
        أرجع 0;
}
