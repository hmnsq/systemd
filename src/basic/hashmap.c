/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <fnmatch.h>
#ضمن <pthread.h>
#ضمن <unistd.h>
#إذا HAVE_VALGRIND_VALGRIND_H
#  include <valgrind/valgrind.h>
#جے_انت

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
#جے_انت

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
#تعریف_کرو INV_KEEP_FREE            5U

/* Fields common to entries of all hashmap/set types */
بنية hashmap_ادھار_اندراج {
        ثابت فراغ *کلید;
};

/* Entry types for specific hashmap/set types
 * hashmap_base_entry must be at the beginning of each entry struct. */

بنية plain_hashmap_اندراج {
        بنية hashmap_ادھار_اندراج b;
        فراغ *قدر;
};

بنية ordered_hashmap_اندراج {
        بنية plain_hashmap_اندراج p;
        بنانشانی iterate_اگلا, iterate_پچھلا;
};

بنية سیٹ_اندراج {
        بنية hashmap_ادھار_اندراج b;
};

/* In several functions it is advantageous to have the hash table extended
 * virtually by a couple of additional buckets. We reserve special index values
 * for these "swap" buckets. */
#تعریف_کرو IDX_وٹاندرا_BEGIN     (UINT_MAX - 3)
#تعریف_کرو IDX_PUT             (IDX_وٹاندرا_BEGIN + 0)
#تعریف_کرو IDX_TMP             (IDX_وٹاندرا_BEGIN + 1)
#تعریف_کرو IDX_وٹاندرا_سرا       (IDX_وٹاندرا_BEGIN + 2)

#تعریف_کرو IDX_پہلا           (UINT_MAX - 1) /* special index for freshly initialized iterators */
#تعریف_کرو IDX_NIL             UINT_MAX       /* special index value meaning "none" or "end" */

assert_cc(IDX_پہلا == IDX_وٹاندرا_سرا);
assert_cc(IDX_پہلا == IDX_ITERATOR_پہلا);

/* Storage space for the "swap" buckets.
 * All entry types can fit into an ordered_hashmap_entry. */
بنية وٹاندرا_entries {
        بنية ordered_hashmap_اندراج e[IDX_وٹاندرا_سرا - IDX_وٹاندرا_BEGIN];
};

/* Distance from Initial Bucket */
تعريفنوع uint8_t dib_raw_t;
#تعریف_کرو DIB_RAW_حد_توں_ودھ ((dib_raw_t)0xfdU)   /* indicates DIB value is greater than representable */
#تعریف_کرو DIB_RAW_REHASH   ((dib_raw_t)0xfeU)   /* entry yet to be rehashed during in-place resize */
#تعریف_کرو DIB_RAW_FREE     ((dib_raw_t)0xffU)   /* a free bucket */
#تعریف_کرو DIB_RAW_INIT     ((حرف)DIB_RAW_FREE) /* a byte to memset a DIB store with when initializing */

#تعریف_کرو DIB_FREE UINT_MAX

#إذا ENABLE_DEBUG_HASHMAP
بنية hashmap_debug_info {
        LIST_FIELDS(بنية hashmap_debug_info, debug_list);
        بنانشانی max_entries;  /* high watermark of n_entries */

        /* fields to detect modification while iterating */
        بنانشانی put_گنتی;    /* counts puts into the hashmap */
        بنانشانی rem_گنتی;    /* counts removals from hashmap */
        بنانشانی آخری_rem_idx; /* remembers last removal index */
};

/* Tracks all existing hashmaps. Get at it from gdb. See sd_dump_hashmaps.py */
ساكن LIST_سر(بنية hashmap_debug_info, hashmap_debug_list);
ساكن pthread_mutex_t hashmap_debug_list_mutex = PTHREAD_MUTEX_INITIALIZER;
#جے_انت

تعداد HashmapType {
        HASHMAP_TYPE_PLAIN,
        HASHMAP_TYPE_ORDERED,
        HASHMAP_TYPE_سیٹ,
        _HASHMAP_TYPE_MAX
};

بنية _packed_ اسدھا_storage {
        فراغ *storage;                     /* where buckets and DIBs are stored */
        uint8_t  hash_کلید[HASH_کلید_ناپ];  /* hash key; changes during resize */

        بنانشانی n_entries;                /* number of stored entries */
        بنانشانی n_buckets;                /* number of buckets */

        بنانشانی idx_سبھ_توں_نیواں_اندراج;         /* Index below which all buckets are free.
                                              Makes "while (hashmap_steal_first())" loops
                                              O(n) instead of O(n^2) for unordered hashmaps. */
        uint8_t  _pad[3];                  /* padding for the whole HashmapBase */
        /* The bitfields in HashmapBase complete the alignment of the whole thing. */
};

بنية سدھا_storage {
        /* This gives us 39 bytes on 64-bit, or 35 bytes on 32-bit.
         * That's room for 4 set_entries + 4 DIB bytes + 3 unused bytes on 64-bit,
         *              or 7 set_entries + 7 DIB bytes + 0 unused bytes on 32-bit. */
        uint8_t storage[حجم(بنية اسدھا_storage)];
};

#تعریف_کرو سدھا_BUCKETS(اندراج_t) \
        (حجم(بنية سدھا_storage) / (حجم(اندراج_t) + حجم(dib_raw_t)))

/* We should be able to store at least one entry directly. */
assert_cc(سدھا_BUCKETS(بنية ordered_hashmap_اندراج) >= 1);

/* We have 3 bits for n_direct_entries. */
assert_cc(سدھا_BUCKETS(بنية سیٹ_اندراج) < (1 << 3));

/* Hashmaps with directly stored entries all use this shared hash key.
 * It's no big deal if the key is guessed, because there can be only
 * a handful of directly stored entries in a hashmap. When a hashmap
 * outgrows direct storage, it gets its own key for indirect storage. */
ساكن uint8_t shared_hash_کلید[HASH_کلید_ناپ];

/* Fields that all hashmap/set types must have */
بنية Hashmap_ادھار {
        ثابت بنية hash_ops *hash_ops;  /* hash and compare ops to use */

        اتحاد _packed_ {
                بنية اسدھا_storage اسدھا; /* if  has_indirect */
                بنية سدھا_storage سدھا;     /* if !has_indirect */
        };

        تعداد HashmapType type:2;     /* HASHMAP_TYPE_* */
        bool has_اسدھا:1;         /* whether indirect storage is used */
        بنانشانی n_سدھا_entries:3; /* Number of entries in direct storage.
                                      * Only valid if !has_indirect. */
        bool from_pool:1;            /* whether was allocated from mempool */
        bool dirty:1;                /* whether dirtied since last iterated_cache_get() */
        bool cached:1;               /* whether this hashmap is being cached */

#إذا ENABLE_DEBUG_HASHMAP
        بنية hashmap_debug_info debug;
#جے_انت
};

/* Specific hash types
 * HashmapBase must be at the beginning of each hashmap struct. */

بنية Hashmap {
        بنية Hashmap_ادھار b;
};

بنية OrderedHashmap {
        بنية Hashmap_ادھار b;
        بنانشانی iterate_list_سر, iterate_list_پوچھ;
};

بنية سیٹ_2 {
        بنية Hashmap_ادھار b;
};

تعريفنوع بنية CacheMem {
        ثابت فراغ **ptr;
        size_t n_populated;
        bool سرگرم:1;
} CacheMem;

بنية IteratedCache {
        Hashmap_ادھار *hashmap;
        CacheMem کلیدیں, values;
};

DEFINE_MEMPOOL(hashmap_pool,         Hashmap,        8);
DEFINE_MEMPOOL(ordered_hashmap_pool, OrderedHashmap, 8);
/* No need for a separate Set pool */
assert_cc(حجم(Hashmap) == حجم(سیٹ_2));

بنية hashmap_type_info {
        size_t سر_ناپ;
        size_t اندراج_ناپ;
        بنية mempool *mempool;
        بنانشانی n_سدھا_buckets;
};

ساكن ورتیا_2 ثابت بنية hashmap_type_info hashmap_type_info[_HASHMAP_TYPE_MAX] = {
        [HASHMAP_TYPE_PLAIN] = {
                .سر_ناپ        = حجم(Hashmap),
                .اندراج_ناپ       = حجم(بنية plain_hashmap_اندراج),
                .mempool          = &hashmap_pool,
                .n_سدھا_buckets = سدھا_BUCKETS(بنية plain_hashmap_اندراج),
        },
        [HASHMAP_TYPE_ORDERED] = {
                .سر_ناپ        = حجم(OrderedHashmap),
                .اندراج_ناپ       = حجم(بنية ordered_hashmap_اندراج),
                .mempool          = &ordered_hashmap_pool,
                .n_سدھا_buckets = سدھا_BUCKETS(بنية ordered_hashmap_اندراج),
        },
        [HASHMAP_TYPE_سیٹ] = {
                .سر_ناپ        = حجم(سیٹ_2),
                .اندراج_ناپ       = حجم(بنية سیٹ_اندراج),
                .mempool          = &hashmap_pool,
                .n_سدھا_buckets = سدھا_BUCKETS(بنية سیٹ_اندراج),
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

        r = get_process_دھاگے(0);
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
#جے_انت

ساكن مضمن بنانشانی n_buckets(Hashmap_ادھار *h) {
        أكد(h);
        أرجع h->has_اسدھا ? h->اسدھا.n_buckets
                               : hashmap_type_info[h->type].n_سدھا_buckets;
}

ساكن بنانشانی n_entries(Hashmap_ادھار *h) {
        أكد(h);
        أرجع h->has_اسدھا ? h->اسدھا.n_entries
                               : h->n_سدھا_entries;
}

ساكن فراغ n_entries_inc(Hashmap_ادھار *h) {
        أكد(h);

        إذا (h->has_اسدھا)
                h->اسدھا.n_entries++;
        وإلا
                h->n_سدھا_entries++;
}

ساكن فراغ n_entries_dec(Hashmap_ادھار *h) {
        أكد(h);

        إذا (h->has_اسدھا)
                h->اسدھا.n_entries--;
        وإلا
                h->n_سدھا_entries--;
}

ساكن مضمن فراغ* storage_ptr(Hashmap_ادھار *h) {
        أكد(h);
        أرجع h->has_اسدھا ? h->اسدھا.storage
                               : h->سدھا.storage;
}

ساكن مضمن uint8_t* hash_کلید(Hashmap_ادھار *h) {
        أكد(h);
        أرجع h->has_اسدھا ? h->اسدھا.hash_کلید
                               : shared_hash_کلید;
}

ساكن بنانشانی ادھار_بالٹی_hash(Hashmap_ادھار *h, ثابت فراغ *p) {
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
        أرجع (بنانشانی) (((hash >> 32) * n_buckets(h)) >> 32);
}
#تعریف_کرو بالٹی_hash(h, p) ادھار_بالٹی_hash(HASHMAP_ادھار(h), p)

ساكن فراغ ادھار_سیٹ_dirty(Hashmap_ادھار *h) {
        أكد(h);

        h->dirty = true;
}
#تعریف_کرو hashmap_سیٹ_dirty(h) ادھار_سیٹ_dirty(HASHMAP_ادھار(h))

ساكن فراغ get_hash_کلید(uint8_t hash_کلید[HASH_کلید_ناپ], bool reuse_is_ok) {
        ساكن uint8_t current[HASH_کلید_ناپ];
        ساكن bool current_initialized = false;

        /* Returns a hash function key to use. In order to keep things
         * fast we will not generate a new key each time we allocate a
         * new hash table. Instead, we'll just reuse the most recently
         * generated one, except if we never generated one or when we
         * are rehashing an entire hash table because we reached a
         * fill level */

        إذا (!current_initialized || !reuse_is_ok) {
                random_بائٹاں(current, حجم(current));
                current_initialized = true;
        }

        حافظہ_نقل(hash_کلید, current, حجم(current));
}

ساكن مضمن بنية hashmap_ادھار_اندراج* بالٹی_at(Hashmap_ادھار *h, بنانشانی idx) {
        أكد(h);
        أرجع CAST_ALIGN_PTR(
                        بنية hashmap_ادھار_اندراج,
                        (uint8_t *) storage_ptr(h) + idx * hashmap_type_info[h->type].اندراج_ناپ);
}

ساكن بنية plain_hashmap_اندراج* plain_بالٹی_at(Hashmap *h, بنانشانی idx) {
        أرجع (بنية plain_hashmap_اندراج*) بالٹی_at(HASHMAP_ادھار(h), idx);
}

ساكن بنية ordered_hashmap_اندراج* ordered_بالٹی_at(OrderedHashmap *h, بنانشانی idx) {
        أرجع (بنية ordered_hashmap_اندراج*) بالٹی_at(HASHMAP_ادھار(h), idx);
}

ساكن بنية سیٹ_اندراج *سیٹ_بالٹی_at(سیٹ_2 *h, بنانشانی idx) {
        أرجع (بنية سیٹ_اندراج*) بالٹی_at(HASHMAP_ادھار(h), idx);
}

ساكن بنية ordered_hashmap_اندراج* بالٹی_at_وٹاندرا(بنية وٹاندرا_entries *وٹاندرا, بنانشانی idx) {
        أكد(وٹاندرا);
        أرجع &وٹاندرا->e[idx - IDX_وٹاندرا_BEGIN];
}

/* Returns a pointer to the bucket at index idx.
 * Understands real indexes and swap indexes, hence "_virtual". */
ساكن بنية hashmap_ادھار_اندراج* بالٹی_at_virtual(Hashmap_ادھار *h, بنية وٹاندرا_entries *وٹاندرا,
                                                    بنانشانی idx) {
        إذا (idx < IDX_وٹاندرا_BEGIN)
                أرجع بالٹی_at(h, idx);

        إذا (idx < IDX_وٹاندرا_سرا)
                أرجع &بالٹی_at_وٹاندرا(وٹاندرا, idx)->p.b;

        assert_not_reached();
}

ساكن مضمن dib_raw_t* dib_raw_ptr(Hashmap_ادھار *h) {
        أكد(h);
        أرجع (dib_raw_t*)
                ((uint8_t*) storage_ptr(h) + hashmap_type_info[h->type].اندراج_ناپ * n_buckets(h));
}

ساكن بنانشانی بالٹی_فاصلہ(Hashmap_ادھار *h, بنانشانی idx, بنانشانی from) {
        أرجع idx >= from ? idx - from
                           : n_buckets(h) + idx - from;
}

ساكن بنانشانی بالٹی_calculate_dib(Hashmap_ادھار *h, بنانشانی idx, dib_raw_t raw_dib) {
        بنانشانی initial_بالٹی;

        إذا (raw_dib == DIB_RAW_FREE)
                أرجع DIB_FREE;

        إذا (_likely_(raw_dib < DIB_RAW_حد_توں_ودھ))
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
        initial_بالٹی = بالٹی_hash(h, بالٹی_at(h, idx)->کلید);
        أرجع بالٹی_فاصلہ(h, idx, initial_بالٹی);
}

ساكن فراغ بالٹی_سیٹ_dib(Hashmap_ادھار *h, بنانشانی idx, بنانشانی dib) {
        dib_raw_ptr(h)[idx] = dib != DIB_FREE ? MIN(dib, DIB_RAW_حد_توں_ودھ) : DIB_RAW_FREE;
}

ساكن بنانشانی skip_free_buckets(Hashmap_ادھار *h, بنانشانی idx) {
        dib_raw_t *dibs;

        dibs = dib_raw_ptr(h);

        لكل ( ; idx < n_buckets(h); idx++)
                إذا (dibs[idx] != DIB_RAW_FREE)
                        أرجع idx;

        أرجع IDX_NIL;
}

ساكن فراغ بالٹی_mark_free(Hashmap_ادھار *h, بنانشانی idx) {
        أكد(h);

        memzero(بالٹی_at(h, idx), hashmap_type_info[h->type].اندراج_ناپ);
        بالٹی_سیٹ_dib(h, idx, DIB_FREE);
}

ساكن فراغ بالٹی_move_اندراج(Hashmap_ادھار *h, بنية وٹاندرا_entries *وٹاندرا,
                              بنانشانی from, بنانشانی to) {
        بنية hashmap_ادھار_اندراج *e_from, *e_منزل;

        أكد(h);
        أكد(from != to);

        e_from = بالٹی_at_virtual(h, وٹاندرا, from);
        e_منزل   = بالٹی_at_virtual(h, وٹاندرا, to);

        حافظہ_نقل(e_منزل, e_from, hashmap_type_info[h->type].اندراج_ناپ);

        إذا (h->type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*) h;
                بنية ordered_hashmap_اندراج *le, *le_منزل;

                le_منزل = (بنية ordered_hashmap_اندراج*) e_منزل;

                إذا (le_منزل->iterate_اگلا != IDX_NIL) {
                        le = (بنية ordered_hashmap_اندراج*)
                             بالٹی_at_virtual(h, وٹاندرا, le_منزل->iterate_اگلا);
                        le->iterate_پچھلا = to;
                }

                إذا (le_منزل->iterate_پچھلا != IDX_NIL) {
                        le = (بنية ordered_hashmap_اندراج*)
                             بالٹی_at_virtual(h, وٹاندرا, le_منزل->iterate_پچھلا);
                        le->iterate_اگلا = to;
                }

                إذا (lh->iterate_list_سر == from)
                        lh->iterate_list_سر = to;
                إذا (lh->iterate_list_پوچھ == from)
                        lh->iterate_list_پوچھ = to;
        }
}

ساكن مضمن بنانشانی اگلا_idx(Hashmap_ادھار *h, بنانشانی idx) {
        أرجع idx + 1U < n_buckets(h) ? idx + 1U : 0U;
}

ساكن بنانشانی prev_idx(Hashmap_ادھار *h, بنانشانی idx) {
        أرجع idx > 0U ? idx - 1U : n_buckets(h) - 1U;
}

ساكن مضمن فراغ* اندراج_قدر(Hashmap_ادھار *h, بنية hashmap_ادھار_اندراج *e) {
        أكد(h);
        أكد(e);

        بدل (h->type) {

        حالة HASHMAP_TYPE_PLAIN:
        حالة HASHMAP_TYPE_ORDERED:
                أرجع ((بنية plain_hashmap_اندراج*)e)->قدر;

        حالة HASHMAP_TYPE_سیٹ:
                أرجع (فراغ*) e->کلید;

        افتراضي:
                assert_not_reached();
        }
}

ساكن فراغ ادھار_remove_اندراج(Hashmap_ادھار *h, بنانشانی idx) {
        بنانشانی بایاں, دایاں, prev, dib;
        dib_raw_t raw_dib, *dibs;

        dibs = dib_raw_ptr(h);
        أكد(dibs[idx] != DIB_RAW_FREE);

#إذا ENABLE_DEBUG_HASHMAP
        أكد(h);
        h->debug.rem_گنتی++;
        h->debug.آخری_rem_idx = idx;
#جے_انت

        بایاں = idx;
        /* Find the stop bucket ("right"). It is either free or has DIB == 0. */
        لكل (دایاں = اگلا_idx(h, بایاں); ; دایاں = اگلا_idx(h, دایاں)) {
                raw_dib = dibs[دایاں];
                إذا (وچ_سیٹ(raw_dib, 0, DIB_RAW_FREE))
                        اكسر;

                /* The buckets are not supposed to be all occupied and with DIB > 0.
                 * That would mean we could make everyone better off by shifting them
                 * backward. This scenario is impossible. */
                أكد(بایاں != دایاں);
        }

        إذا (h->type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*) h;
                بنية ordered_hashmap_اندراج *le = ordered_بالٹی_at(lh, idx);

                إذا (le->iterate_اگلا != IDX_NIL)
                        ordered_بالٹی_at(lh, le->iterate_اگلا)->iterate_پچھلا = le->iterate_پچھلا;
                وإلا
                        lh->iterate_list_پوچھ = le->iterate_پچھلا;

                إذا (le->iterate_پچھلا != IDX_NIL)
                        ordered_بالٹی_at(lh, le->iterate_پچھلا)->iterate_اگلا = le->iterate_اگلا;
                وإلا
                        lh->iterate_list_سر = le->iterate_اگلا;
        }

        /* Now shift all buckets in the interval (left, right) one step backwards */
        لكل (prev = بایاں, بایاں = اگلا_idx(h, بایاں); بایاں != دایاں;
             prev = بایاں, بایاں = اگلا_idx(h, بایاں)) {
                dib = بالٹی_calculate_dib(h, بایاں, dibs[بایاں]);
                أكد(dib != 0);
                بالٹی_move_اندراج(h, عدم, بایاں, prev);
                بالٹی_سیٹ_dib(h, prev, dib - 1);
        }

        بالٹی_mark_free(h, prev);
        n_entries_dec(h);
        ادھار_سیٹ_dirty(h);
}
#تعریف_کرو remove_اندراج(h, idx) ادھار_remove_اندراج(HASHMAP_ادھار(h), idx)

ساكن بنانشانی hashmap_iterate_وچ_insertion_ترتیب(OrderedHashmap *h, Iterator *i) {
        بنية ordered_hashmap_اندراج *e;
        بنانشانی idx;

        أكد(h);
        أكد(i);

        إذا (i->idx == IDX_NIL)
                اذهب at_سرا;

        إذا (i->idx == IDX_پہلا && h->iterate_list_سر == IDX_NIL)
                اذهب at_سرا;

        إذا (i->idx == IDX_پہلا) {
                idx = h->iterate_list_سر;
                e = ordered_بالٹی_at(h, idx);
        } وإلا {
                idx = i->idx;
                e = ordered_بالٹی_at(h, idx);
                /*
                 * We allow removing the current entry while iterating, but removal may cause
                 * a backward shift. The next entry may thus move one bucket to the left.
                 * To detect when it happens, we remember the key pointer of the entry we were
                 * going to iterate next. If it does not match, there was a backward shift.
                 */
                إذا (e->p.b.کلید != i->اگلا_کلید) {
                        idx = prev_idx(HASHMAP_ادھار(h), idx);
                        e = ordered_بالٹی_at(h, idx);
                }
                أكد(e->p.b.کلید == i->اگلا_کلید);
        }

#إذا ENABLE_DEBUG_HASHMAP
        i->prev_idx = idx;
#جے_انت

        إذا (e->iterate_اگلا != IDX_NIL) {
                بنية ordered_hashmap_اندراج *n;
                i->idx = e->iterate_اگلا;
                n = ordered_بالٹی_at(h, i->idx);
                i->اگلا_کلید = n->p.b.کلید;
        } وإلا
                i->idx = IDX_NIL;

        أرجع idx;

at_سرا:
        i->idx = IDX_NIL;
        أرجع IDX_NIL;
}

ساكن بنانشانی hashmap_iterate_وچ_اندرونی_ترتیب(Hashmap_ادھار *h, Iterator *i) {
        بنانشانی idx;

        أكد(h);
        أكد(i);

        إذا (i->idx == IDX_NIL)
                اذهب at_سرا;

        إذا (i->idx == IDX_پہلا) {
                /* fast forward to the first occupied bucket */
                إذا (h->has_اسدھا) {
                        i->idx = skip_free_buckets(h, h->اسدھا.idx_سبھ_توں_نیواں_اندراج);
                        h->اسدھا.idx_سبھ_توں_نیواں_اندراج = i->idx;
                } وإلا
                        i->idx = skip_free_buckets(h, 0);

                إذا (i->idx == IDX_NIL)
                        اذهب at_سرا;
        } وإلا {
                بنية hashmap_ادھار_اندراج *e;

                أكد(i->idx > 0);

                e = بالٹی_at(h, i->idx);
                /*
                 * We allow removing the current entry while iterating, but removal may cause
                 * a backward shift. The next entry may thus move one bucket to the left.
                 * To detect when it happens, we remember the key pointer of the entry we were
                 * going to iterate next. If it does not match, there was a backward shift.
                 */
                إذا (e->کلید != i->اگلا_کلید)
                        e = بالٹی_at(h, --i->idx);

                أكد(e->کلید == i->اگلا_کلید);
        }

        idx = i->idx;
#إذا ENABLE_DEBUG_HASHMAP
        i->prev_idx = idx;
#جے_انت

        i->idx = skip_free_buckets(h, i->idx + 1);
        إذا (i->idx != IDX_NIL)
                i->اگلا_کلید = بالٹی_at(h, i->idx)->کلید;
        وإلا
                i->idx = IDX_NIL;

        أرجع idx;

at_سرا:
        i->idx = IDX_NIL;
        أرجع IDX_NIL;
}

ساكن بنانشانی hashmap_iterate_اندراج(Hashmap_ادھار *h, Iterator *i) {
        إذا (!h) {
                i->idx = IDX_NIL;
                أرجع IDX_NIL;
        }

#إذا ENABLE_DEBUG_HASHMAP
        إذا (i->idx == IDX_پہلا) {
                i->put_گنتی = h->debug.put_گنتی;
                i->rem_گنتی = h->debug.rem_گنتی;
        } وإلا {
                /* While iterating, must not add any new entries */
                أكد(i->put_گنتی == h->debug.put_گنتی);
                /* ... or remove entries other than the current one */
                أكد(i->rem_گنتی == h->debug.rem_گنتی ||
                       (i->rem_گنتی == h->debug.rem_گنتی - 1 &&
                        i->prev_idx == h->debug.آخری_rem_idx));
                /* Reset our removals counter */
                i->rem_گنتی = h->debug.rem_گنتی;
        }
#جے_انت

        أرجع h->type == HASHMAP_TYPE_ORDERED ? hashmap_iterate_وچ_insertion_ترتیب((OrderedHashmap*) h, i)
                                               : hashmap_iterate_وچ_اندرونی_ترتیب(h, i);
}

bool _hashmap_iterate(Hashmap_ادھار *h, Iterator *i, فراغ **قدر, ثابت فراغ **کلید) {
        بنية hashmap_ادھار_اندراج *e;
        فراغ *انکڑے;
        بنانشانی idx;

        idx = hashmap_iterate_اندراج(h, i);
        إذا (idx == IDX_NIL) {
                إذا (قدر)
                        *قدر = عدم;
                إذا (کلید)
                        *کلید = عدم;

                أرجع false;
        }

        e = بالٹی_at(h, idx);
        انکڑے = اندراج_قدر(h, e);
        إذا (قدر)
                *قدر = انکڑے;
        إذا (کلید)
                *کلید = e->کلید;

        أرجع true;
}

#تعریف_کرو HASHMAP_FOREACH_IDX(idx, h, i) \
        لكل ((i) = ITERATOR_پہلا, (idx) = hashmap_iterate_اندراج((h), &(i)); \
             (idx != IDX_NIL); \
             (idx) = hashmap_iterate_اندراج((h), &(i)))

IteratedCache* _hashmap_iterated_cache_new(Hashmap_ادھار *h) {
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

ساكن فراغ reset_سدھا_storage(Hashmap_ادھار *h) {
        ثابت بنية hashmap_type_info *hi = &hashmap_type_info[h->type];
        فراغ *p;

        أكد(!h->has_اسدھا);

        p = mempset(h->سدھا.storage, 0, hi->اندراج_ناپ * hi->n_سدھا_buckets);
        memset(p, DIB_RAW_INIT, حجم(dib_raw_t) * hi->n_سدھا_buckets);
}

ساكن فراغ shared_hash_کلید_initialize(فراغ) {
        random_بائٹاں(shared_hash_کلید, حجم(shared_hash_کلید));
}

ساكن بنية Hashmap_ادھار* hashmap_ادھار_new(ثابت بنية hash_ops *hash_ops, تعداد HashmapType type) {
        Hashmap_ادھار *h;
        ثابت بنية hashmap_type_info *hi = &hashmap_type_info[type];

        bool use_pool = mempool_enabled && mempool_enabled();  /* mempool_enabled is a weak symbol */

        h = use_pool ? mempool_alloc0_tile(hi->mempool) : malloc0(hi->سر_ناپ);
        إذا (!h)
                أرجع عدم;

        h->type = type;
        h->from_pool = use_pool;
        h->hash_ops = hash_ops ?: &trivial_hash_ops;

        إذا (type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*)h;
                lh->iterate_list_سر = lh->iterate_list_پوچھ = IDX_NIL;
        }

        reset_سدھا_storage(h);

        ساكن pthread_once_t once = PTHREAD_ONCE_INIT;
        assert_se(pthread_once(&once, shared_hash_کلید_initialize) == 0);

#إذا ENABLE_DEBUG_HASHMAP
        assert_se(pthread_mutex_lock(&hashmap_debug_list_mutex) == 0);
        LIST_PREPEND(debug_list, hashmap_debug_list, &h->debug);
        assert_se(pthread_mutex_unlock(&hashmap_debug_list_mutex) == 0);
#جے_انت

        أرجع h;
}

Hashmap *hashmap_new(ثابت بنية hash_ops *hash_ops) {
        أرجع (Hashmap*)        hashmap_ادھار_new(hash_ops, HASHMAP_TYPE_PLAIN);
}

OrderedHashmap *ordered_hashmap_new(ثابت بنية hash_ops *hash_ops) {
        أرجع (OrderedHashmap*) hashmap_ادھار_new(hash_ops, HASHMAP_TYPE_ORDERED);
}

سیٹ_2 *سیٹ_new(ثابت بنية hash_ops *hash_ops) {
        أرجع (سیٹ_2*)            hashmap_ادھار_new(hash_ops, HASHMAP_TYPE_سیٹ);
}

ساكن صحيح hashmap_ادھار_ensure_allocated(Hashmap_ادھار **h, ثابت بنية hash_ops *hash_ops,
                                         تعداد HashmapType type) {
        Hashmap_ادھار *q;

        أكد(h);

        إذا (*h) {
                أكد((*h)->hash_ops == (hash_ops ?: &trivial_hash_ops));
                أرجع 0;
        }

        q = hashmap_ادھار_new(hash_ops, type);
        إذا (!q)
                أرجع -ENOMEM;

        *h = q;
        أرجع 1;
}

صحيح hashmap_ensure_allocated(Hashmap **h, ثابت بنية hash_ops *hash_ops) {
        أرجع hashmap_ادھار_ensure_allocated((Hashmap_ادھار**)h, hash_ops, HASHMAP_TYPE_PLAIN);
}

صحيح ordered_hashmap_ensure_allocated(OrderedHashmap **h, ثابت بنية hash_ops *hash_ops) {
        أرجع hashmap_ادھار_ensure_allocated((Hashmap_ادھار**)h, hash_ops, HASHMAP_TYPE_ORDERED);
}

صحيح سیٹ_ensure_allocated(سیٹ_2 **s, ثابت بنية hash_ops *hash_ops) {
        أرجع hashmap_ادھار_ensure_allocated((Hashmap_ادھار**)s, hash_ops, HASHMAP_TYPE_سیٹ);
}

صحيح hashmap_ensure_put(Hashmap **h, ثابت بنية hash_ops *hash_ops, ثابت فراغ *کلید, فراغ *قدر) {
        صحيح r;

        أكد(h);

        r = hashmap_ensure_allocated(h, hash_ops);
        إذا (r < 0)
                أرجع r;

        أرجع hashmap_put(*h, کلید, قدر);
}

صحيح ordered_hashmap_ensure_put(OrderedHashmap **h, ثابت بنية hash_ops *hash_ops, ثابت فراغ *کلید, فراغ *قدر) {
        صحيح r;

        أكد(h);

        r = ordered_hashmap_ensure_allocated(h, hash_ops);
        إذا (r < 0)
                أرجع r;

        أرجع ordered_hashmap_put(*h, کلید, قدر);
}

صحيح ordered_hashmap_ensure_replace(OrderedHashmap **h, ثابت بنية hash_ops *hash_ops, ثابت فراغ *کلید, فراغ *قدر) {
        صحيح r;

        أكد(h);

        r = ordered_hashmap_ensure_allocated(h, hash_ops);
        إذا (r < 0)
                أرجع r;

        أرجع ordered_hashmap_replace(*h, کلید, قدر);
}

صحيح hashmap_ensure_replace(Hashmap **h, ثابت بنية hash_ops *hash_ops, ثابت فراغ *کلید, فراغ *قدر) {
        صحيح r;

        أكد(h);

        r = hashmap_ensure_allocated(h, hash_ops);
        إذا (r < 0)
                أرجع r;

        أرجع hashmap_replace(*h, کلید, قدر);
}

ساكن فراغ hashmap_free_no_clear(Hashmap_ادھار *h) {
        أكد(!h->has_اسدھا);
        أكد(h->n_سدھا_entries == 0);

#إذا ENABLE_DEBUG_HASHMAP
        assert_se(pthread_mutex_lock(&hashmap_debug_list_mutex) == 0);
        LIST_REMOVE(debug_list, hashmap_debug_list, &h->debug);
        assert_se(pthread_mutex_unlock(&hashmap_debug_list_mutex) == 0);
#جے_انت

        إذا (h->from_pool) {
                /* Ensure that the object didn't get migrated between threads. */
                assert_se(is_main_thread());
                mempool_free_tile(hashmap_type_info[h->type].mempool, h);
        } وإلا
                حرر(h);
}

Hashmap_ادھار* _hashmap_free(Hashmap_ادھار *h) {
        إذا (h) {
                _hashmap_clear(h);
                hashmap_free_no_clear(h);
        }

        أرجع عدم;
}

فراغ _hashmap_clear(Hashmap_ادھار *h) {
        إذا (!h)
                أرجع;

        إذا (h->hash_ops->free_کلید || h->hash_ops->free_قدر) {

                /* If destructor calls are defined, let's destroy things defensively: let's take the item out of the
                 * hash table, and only then call the destructor functions. If these destructors then try to unregister
                 * themselves from our hash table a second time, the entry is already gone. */

                طالما (hashmap_ناپ(h) > 0) {
                        فراغ *k = عدم;
                        فراغ *v;

                        v = hashmap_پہلا_کلید_and_قدر(h, true, &k);

                        إذا (h->hash_ops->free_کلید)
                                h->hash_ops->free_کلید(k);

                        إذا (h->hash_ops->free_قدر)
                                h->hash_ops->free_قدر(v);
                }
        }

        إذا (h->has_اسدھا) {
                حرر(h->اسدھا.storage);
                h->has_اسدھا = false;
        }

        h->n_سدھا_entries = 0;
        reset_سدھا_storage(h);

        إذا (h->type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*) h;
                lh->iterate_list_سر = lh->iterate_list_پوچھ = IDX_NIL;
        }

        ادھار_سیٹ_dirty(h);
}

ساكن صحيح resize_buckets(Hashmap_ادھار *h, بنانشانی entries_جوڑو);

/*
 * Finds an empty bucket to put an entry into, starting the scan at 'idx'.
 * Performs Robin Hood swaps as it goes. The entry to put must be placed
 * by the caller into swap slot IDX_PUT.
 * If used for in-place resizing, may leave a displaced entry in swap slot
 * IDX_PUT. Caller must rehash it next.
 * Returns: true if it left a displaced entry to rehash next in IDX_PUT,
 *          false otherwise.
 */
ساكن bool hashmap_put_robin_hood(Hashmap_ادھار *h, بنانشانی idx,
                                   بنية وٹاندرا_entries *وٹاندرا) {
        dib_raw_t raw_dib, *dibs;
        بنانشانی dib, فاصلہ;

#إذا ENABLE_DEBUG_HASHMAP
        أكد(h);
        h->debug.put_گنتی++;
#جے_انت

        dibs = dib_raw_ptr(h);

        لكل (فاصلہ = 0; ; فاصلہ++) {
                raw_dib = dibs[idx];
                إذا (وچ_سیٹ(raw_dib, DIB_RAW_FREE, DIB_RAW_REHASH)) {
                        إذا (raw_dib == DIB_RAW_REHASH)
                                بالٹی_move_اندراج(h, وٹاندرا, idx, IDX_TMP);

                        إذا (h->has_اسدھا && h->اسدھا.idx_سبھ_توں_نیواں_اندراج > idx)
                                h->اسدھا.idx_سبھ_توں_نیواں_اندراج = idx;

                        بالٹی_سیٹ_dib(h, idx, فاصلہ);
                        بالٹی_move_اندراج(h, وٹاندرا, IDX_PUT, idx);
                        إذا (raw_dib == DIB_RAW_REHASH) {
                                بالٹی_move_اندراج(h, وٹاندرا, IDX_TMP, IDX_PUT);
                                أرجع true;
                        }

                        أرجع false;
                }

                dib = بالٹی_calculate_dib(h, idx, raw_dib);

                إذا (dib < فاصلہ) {
                        /* Found a wealthier entry. Go Robin Hood! */
                        بالٹی_سیٹ_dib(h, idx, فاصلہ);

                        /* swap the entries */
                        بالٹی_move_اندراج(h, وٹاندرا, idx, IDX_TMP);
                        بالٹی_move_اندراج(h, وٹاندرا, IDX_PUT, idx);
                        بالٹی_move_اندراج(h, وٹاندرا, IDX_TMP, IDX_PUT);

                        فاصلہ = dib;
                }

                idx = اگلا_idx(h, idx);
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
ساكن صحيح hashmap_ادھار_put_boldly(Hashmap_ادھار *h, بنانشانی idx,
                                   بنية وٹاندرا_entries *وٹاندرا, bool may_resize) {
        بنية ordered_hashmap_اندراج *new_اندراج;
        صحيح r;

        أكد(idx < n_buckets(h));

        new_اندراج = بالٹی_at_وٹاندرا(وٹاندرا, IDX_PUT);

        إذا (may_resize) {
                r = resize_buckets(h, 1);
                إذا (r < 0)
                        أرجع r;
                إذا (r > 0)
                        idx = بالٹی_hash(h, new_اندراج->p.b.کلید);
        }
        أكد(n_entries(h) < n_buckets(h));

        إذا (h->type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*) h;

                new_اندراج->iterate_اگلا = IDX_NIL;
                new_اندراج->iterate_پچھلا = lh->iterate_list_پوچھ;

                إذا (lh->iterate_list_پوچھ != IDX_NIL) {
                        بنية ordered_hashmap_اندراج *old_پوچھ;

                        old_پوچھ = ordered_بالٹی_at(lh, lh->iterate_list_پوچھ);
                        أكد(old_پوچھ->iterate_اگلا == IDX_NIL);
                        old_پوچھ->iterate_اگلا = IDX_PUT;
                }

                lh->iterate_list_پوچھ = IDX_PUT;
                إذا (lh->iterate_list_سر == IDX_NIL)
                        lh->iterate_list_سر = IDX_PUT;
        }

        assert_se(hashmap_put_robin_hood(h, idx, وٹاندرا) == false);

        n_entries_inc(h);
#إذا ENABLE_DEBUG_HASHMAP
        h->debug.max_entries = MAX(h->debug.max_entries, n_entries(h));
#جے_انت

        ادھار_سیٹ_dirty(h);

        أرجع 1;
}
#تعریف_کرو hashmap_put_boldly(h, idx, وٹاندرا, may_resize) \
        hashmap_ادھار_put_boldly(HASHMAP_ادھار(h), idx, وٹاندرا, may_resize)

/*
 * Returns 0 if resize is not needed.
 *         1 if successfully resized.
 *         -ENOMEM on allocation failure.
 */
ساكن صحيح resize_buckets(Hashmap_ادھار *h, بنانشانی entries_جوڑو) {
        بنية وٹاندرا_entries وٹاندرا;
        فراغ *new_storage;
        dib_raw_t *old_dibs, *new_dibs;
        ثابت بنية hashmap_type_info *hi;
        بنانشانی idx, optimal_idx;
        بنانشانی old_n_buckets, new_n_buckets, n_rehashed, new_n_entries;
        uint8_t new_کھسکاؤ;
        bool rehash_اگلا;

        أكد(h);

        hi = &hashmap_type_info[h->type];
        new_n_entries = n_entries(h) + entries_جوڑو;

        /* overflow? */
        إذا (_unlikely_(new_n_entries < entries_جوڑو))
                أرجع -ENOMEM;

        /* For direct storage we allow 100% load, because it's tiny. */
        إذا (!h->has_اسدھا && new_n_entries <= hi->n_سدھا_buckets)
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

        إذا (_unlikely_(new_n_buckets > UINT_MAX / (hi->اندراج_ناپ + حجم(dib_raw_t))))
                أرجع -ENOMEM;

        new_کھسکاؤ = log2u_گیڑا_اتے(MAX(
                        new_n_buckets * (hi->اندراج_ناپ + حجم(dib_raw_t)),
                        2 * حجم(بنية سدھا_storage)));

        /* Realloc storage (buckets and DIB array). */
        new_storage = realloc(h->has_اسدھا ? h->اسدھا.storage : عدم,
                              1U << new_کھسکاؤ);
        إذا (!new_storage)
                أرجع -ENOMEM;

        /* Must upgrade direct to indirect storage. */
        إذا (!h->has_اسدھا) {
                حافظہ_نقل(new_storage, h->سدھا.storage,
                       old_n_buckets * (hi->اندراج_ناپ + حجم(dib_raw_t)));
                h->اسدھا.n_entries = h->n_سدھا_entries;
                h->اسدھا.idx_سبھ_توں_نیواں_اندراج = 0;
                h->n_سدھا_entries = 0;
        }

        /* Get a new hash key. If we've just upgraded to indirect storage,
         * allow reusing a previously generated key. It's still a different key
         * from the shared one that we used for direct storage. */
        get_hash_کلید(h->اسدھا.hash_کلید, !h->has_اسدھا);

        h->has_اسدھا = true;
        h->اسدھا.storage = new_storage;
        h->اسدھا.n_buckets = (1U << new_کھسکاؤ) /
                                (hi->اندراج_ناپ + حجم(dib_raw_t));

        old_dibs = (dib_raw_t*)((uint8_t*) new_storage + hi->اندراج_ناپ * old_n_buckets);
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
        memzero(بالٹی_at(h, old_n_buckets),
               (n_buckets(h) - old_n_buckets) * hi->اندراج_ناپ);

        /* The upper half of the new DIB array needs initialization */
        memset(&new_dibs[old_n_buckets], DIB_RAW_INIT,
               (n_buckets(h) - old_n_buckets) * حجم(dib_raw_t));

        /* Rehash entries that need it */
        n_rehashed = 0;
        لكل (idx = 0; idx < old_n_buckets; idx++) {
                إذا (new_dibs[idx] != DIB_RAW_REHASH)
                        استمر;

                optimal_idx = بالٹی_hash(h, بالٹی_at(h, idx)->کلید);

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
                بالٹی_move_اندراج(h, &وٹاندرا, idx, IDX_PUT);
                /* bucket_move_entry does not clear the source */
                memzero(بالٹی_at(h, idx), hi->اندراج_ناپ);

                افعل {
                        /*
                         * Find the new bucket for the current entry. This may make
                         * another entry homeless and load it into IDX_PUT.
                         */
                        rehash_اگلا = hashmap_put_robin_hood(h, optimal_idx, &وٹاندرا);
                        n_rehashed++;

                        /* Did the current entry displace another one? */
                        إذا (rehash_اگلا)
                                optimal_idx = بالٹی_hash(h, بالٹی_at_وٹاندرا(&وٹاندرا, IDX_PUT)->p.b.کلید);
                } طالما (rehash_اگلا);
        }

        assert_se(n_rehashed == n_entries(h));

        أرجع 1;
}

/*
 * Finds an entry with a matching key
 * Returns: index of the found entry, or IDX_NIL if not found.
 */
ساكن بنانشانی ادھار_بالٹی_جانچ(Hashmap_ادھار *h, بنانشانی idx, ثابت فراغ *کلید) {
        compare_func_t compare = h->hash_ops->compare;
        بنية hashmap_ادھار_اندراج *e;
        بنانشانی dib, فاصلہ;
        dib_raw_t *dibs = dib_raw_ptr(h);

        أكد(idx < n_buckets(h));

        لكل (فاصلہ = 0; ; فاصلہ++) {
                إذا (dibs[idx] == DIB_RAW_FREE)
                        أرجع IDX_NIL;

                dib = بالٹی_calculate_dib(h, idx, dibs[idx]);

                إذا (dib < فاصلہ)
                        أرجع IDX_NIL;
                إذا (dib == فاصلہ) {
                        e = بالٹی_at(h, idx);

                        /* Most maps compare keys by pointer. Do that here, without the indirect call. */
                        إذا (compare == trivial_compare_func ? e->کلید == کلید : compare(e->کلید, کلید) == 0)
                                أرجع idx;
                }

                idx = اگلا_idx(h, idx);
        }
}
#تعریف_کرو بالٹی_جانچ(h, idx, کلید) ادھار_بالٹی_جانچ(HASHMAP_ادھار(h), idx, کلید)

صحيح hashmap_put(Hashmap *h, ثابت فراغ *کلید, فراغ *قدر) {
        بنية وٹاندرا_entries وٹاندرا;
        بنية plain_hashmap_اندراج *e;
        بنانشانی hash, idx;

        أكد(h);

        hash = بالٹی_hash(h, کلید);
        idx = بالٹی_جانچ(h, hash, کلید);
        إذا (idx != IDX_NIL) {
                e = plain_بالٹی_at(h, idx);
                إذا (e->قدر == قدر)
                        أرجع 0;
                أرجع -EEXIST;
        }

        e = &بالٹی_at_وٹاندرا(&وٹاندرا, IDX_PUT)->p;
        e->b.کلید = کلید;
        e->قدر = قدر;
        أرجع hashmap_put_boldly(h, hash, &وٹاندرا, true);
}

صحيح سیٹ_put(سیٹ_2 *s, ثابت فراغ *کلید) {
        بنية وٹاندرا_entries وٹاندرا;
        بنية hashmap_ادھار_اندراج *e;
        بنانشانی hash, idx;

        أكد(s);

        hash = بالٹی_hash(s, کلید);
        idx = بالٹی_جانچ(s, hash, کلید);
        إذا (idx != IDX_NIL)
                أرجع 0;

        e = &بالٹی_at_وٹاندرا(&وٹاندرا, IDX_PUT)->p.b;
        e->کلید = کلید;
        أرجع hashmap_put_boldly(s, hash, &وٹاندرا, true);
}

صحيح سیٹ_ensure_put(سیٹ_2 **s, ثابت بنية hash_ops *hash_ops, ثابت فراغ *کلید) {
        صحيح r;

        أكد(s);

        r = سیٹ_ensure_allocated(s, hash_ops);
        إذا (r < 0)
                أرجع r;

        أرجع سیٹ_put(*s, کلید);
}

صحيح سیٹ_ensure_consume(سیٹ_2 **s, ثابت بنية hash_ops *hash_ops, فراغ *کلید) {
        صحيح r;

        r = سیٹ_ensure_put(s, hash_ops, کلید);
        إذا (r <= 0) {
                إذا (hash_ops && hash_ops->free_کلید)
                        hash_ops->free_کلید(کلید);
                وإلا إذا (hash_ops && hash_ops->free_قدر)
                        /* Sets store their element in the key slot but may carry a value destructor. */
                        hash_ops->free_قدر(کلید);
                وإلا
                        حرر(کلید);
        }

        أرجع r;
}

صحيح hashmap_replace(Hashmap *h, ثابت فراغ *کلید, فراغ *قدر) {
        بنية وٹاندرا_entries وٹاندرا;
        بنية plain_hashmap_اندراج *e;
        بنانشانی hash, idx;

        أكد(h);

        hash = بالٹی_hash(h, کلید);
        idx = بالٹی_جانچ(h, hash, کلید);
        إذا (idx != IDX_NIL) {
                e = plain_بالٹی_at(h, idx);
#إذا ENABLE_DEBUG_HASHMAP
                /* Although the key is equal, the key pointer may have changed,
                 * and this would break our assumption for iterating. So count
                 * this operation as incompatible with iteration. */
                إذا (e->b.کلید != کلید) {
                        h->b.debug.put_گنتی++;
                        h->b.debug.rem_گنتی++;
                        h->b.debug.آخری_rem_idx = idx;
                }
#جے_انت
                e->b.کلید = کلید;
                e->قدر = قدر;
                hashmap_سیٹ_dirty(h);

                أرجع 0;
        }

        e = &بالٹی_at_وٹاندرا(&وٹاندرا, IDX_PUT)->p;
        e->b.کلید = کلید;
        e->قدر = قدر;
        أرجع hashmap_put_boldly(h, hash, &وٹاندرا, true);
}

صحيح hashmap_update(Hashmap *h, ثابت فراغ *کلید, فراغ *قدر) {
        بنية plain_hashmap_اندراج *e;
        بنانشانی hash, idx;

        أكد(h);

        hash = بالٹی_hash(h, کلید);
        idx = بالٹی_جانچ(h, hash, کلید);
        إذا (idx == IDX_NIL)
                أرجع -ENOENT;

        e = plain_بالٹی_at(h, idx);
        e->قدر = قدر;
        hashmap_سیٹ_dirty(h);

        أرجع 0;
}

فراغ* _hashmap_get(Hashmap_ادھار *h, ثابت فراغ *کلید) {
        بنية hashmap_ادھار_اندراج *e;
        بنانشانی hash, idx;

        إذا (!h)
                أرجع عدم;

        hash = بالٹی_hash(h, کلید);
        idx = بالٹی_جانچ(h, hash, کلید);
        إذا (idx == IDX_NIL)
                أرجع عدم;

        e = بالٹی_at(h, idx);
        أرجع اندراج_قدر(h, e);
}

فراغ* hashmap_get2(Hashmap *h, ثابت فراغ *کلید, فراغ **ret) {
        بنية plain_hashmap_اندراج *e;
        بنانشانی hash, idx;

        إذا (!h)
                أرجع عدم;

        hash = بالٹی_hash(h, کلید);
        idx = بالٹی_جانچ(h, hash, کلید);
        إذا (idx == IDX_NIL)
                أرجع عدم;

        e = plain_بالٹی_at(h, idx);
        إذا (ret)
                *ret = (فراغ*) e->b.کلید;

        أرجع e->قدر;
}

bool hashmap_شامل_اے_2(Hashmap_ادھار *h, ثابت فراغ *کلید) {
        بنانشانی hash;

        إذا (!h)
                أرجع false;

        hash = بالٹی_hash(h, کلید);
        أرجع بالٹی_جانچ(h, hash, کلید) != IDX_NIL;
}

فراغ* _hashmap_remove(Hashmap_ادھار *h, ثابت فراغ *کلید) {
        بنية hashmap_ادھار_اندراج *e;
        بنانشانی hash, idx;
        فراغ *انکڑے;

        إذا (!h)
                أرجع عدم;

        hash = بالٹی_hash(h, کلید);
        idx = بالٹی_جانچ(h, hash, کلید);
        إذا (idx == IDX_NIL)
                أرجع عدم;

        e = بالٹی_at(h, idx);
        انکڑے = اندراج_قدر(h, e);
        remove_اندراج(h, idx);

        أرجع انکڑے;
}

فراغ* hashmap_remove2(Hashmap *h, ثابت فراغ *کلید, فراغ **ret) {
        بنية plain_hashmap_اندراج *e;
        بنانشانی hash, idx;
        فراغ *انکڑے;

        إذا (!h) {
                إذا (ret)
                        *ret = عدم;
                أرجع عدم;
        }

        hash = بالٹی_hash(h, کلید);
        idx = بالٹی_جانچ(h, hash, کلید);
        إذا (idx == IDX_NIL) {
                إذا (ret)
                        *ret = عدم;
                أرجع عدم;
        }

        e = plain_بالٹی_at(h, idx);
        انکڑے = e->قدر;
        إذا (ret)
                *ret = (فراغ*) e->b.کلید;

        remove_اندراج(h, idx);

        أرجع انکڑے;
}

صحيح hashmap_remove_and_put(Hashmap *h, ثابت فراغ *old_کلید, ثابت فراغ *new_کلید, فراغ *قدر) {
        بنية وٹاندرا_entries وٹاندرا;
        بنية plain_hashmap_اندراج *e;
        بنانشانی old_hash, new_hash, idx;

        إذا (!h)
                أرجع -ENOENT;

        old_hash = بالٹی_hash(h, old_کلید);
        idx = بالٹی_جانچ(h, old_hash, old_کلید);
        إذا (idx == IDX_NIL)
                أرجع -ENOENT;

        new_hash = بالٹی_hash(h, new_کلید);
        إذا (بالٹی_جانچ(h, new_hash, new_کلید) != IDX_NIL)
                أرجع -EEXIST;

        remove_اندراج(h, idx);

        e = &بالٹی_at_وٹاندرا(&وٹاندرا, IDX_PUT)->p;
        e->b.کلید = new_کلید;
        e->قدر = قدر;
        assert_se(hashmap_put_boldly(h, new_hash, &وٹاندرا, false) == 1);

        أرجع 0;
}

صحيح سیٹ_remove_and_put(سیٹ_2 *s, ثابت فراغ *old_کلید, ثابت فراغ *new_کلید) {
        بنية وٹاندرا_entries وٹاندرا;
        بنية hashmap_ادھار_اندراج *e;
        بنانشانی old_hash, new_hash, idx;

        إذا (!s)
                أرجع -ENOENT;

        old_hash = بالٹی_hash(s, old_کلید);
        idx = بالٹی_جانچ(s, old_hash, old_کلید);
        إذا (idx == IDX_NIL)
                أرجع -ENOENT;

        new_hash = بالٹی_hash(s, new_کلید);
        إذا (بالٹی_جانچ(s, new_hash, new_کلید) != IDX_NIL)
                أرجع -EEXIST;

        remove_اندراج(s, idx);

        e = &بالٹی_at_وٹاندرا(&وٹاندرا, IDX_PUT)->p.b;
        e->کلید = new_کلید;
        assert_se(hashmap_put_boldly(s, new_hash, &وٹاندرا, false) == 1);

        أرجع 0;
}

صحيح hashmap_remove_and_replace(Hashmap *h, ثابت فراغ *old_کلید, ثابت فراغ *new_کلید, فراغ *قدر) {
        بنية وٹاندرا_entries وٹاندرا;
        بنية plain_hashmap_اندراج *e;
        بنانشانی old_hash, new_hash, idx_old, idx_new;

        إذا (!h)
                أرجع -ENOENT;

        old_hash = بالٹی_hash(h, old_کلید);
        idx_old = بالٹی_جانچ(h, old_hash, old_کلید);
        إذا (idx_old == IDX_NIL)
                أرجع -ENOENT;

        old_کلید = بالٹی_at(HASHMAP_ادھار(h), idx_old)->کلید;

        new_hash = بالٹی_hash(h, new_کلید);
        idx_new = بالٹی_جانچ(h, new_hash, new_کلید);
        إذا (idx_new != IDX_NIL)
                إذا (idx_old != idx_new) {
                        remove_اندراج(h, idx_new);
                        /* Compensate for a possible backward shift. */
                        إذا (old_کلید != بالٹی_at(HASHMAP_ادھار(h), idx_old)->کلید)
                                idx_old = prev_idx(HASHMAP_ادھار(h), idx_old);
                        أكد(old_کلید == بالٹی_at(HASHMAP_ادھار(h), idx_old)->کلید);
                }

        remove_اندراج(h, idx_old);

        e = &بالٹی_at_وٹاندرا(&وٹاندرا, IDX_PUT)->p;
        e->b.کلید = new_کلید;
        e->قدر = قدر;
        assert_se(hashmap_put_boldly(h, new_hash, &وٹاندرا, false) == 1);

        أرجع 0;
}

فراغ* hashmap_remove_قدر(Hashmap_ادھار *h, ثابت فراغ *کلید, فراغ *قدر) {
        بنية hashmap_ادھار_اندراج *e;
        بنانشانی hash, idx;

        إذا (!h)
                أرجع عدم;

        hash = بالٹی_hash(h, کلید);
        idx = بالٹی_جانچ(h, hash, کلید);
        إذا (idx == IDX_NIL)
                أرجع عدم;

        e = بالٹی_at(h, idx);
        إذا (اندراج_قدر(h, e) != قدر)
                أرجع عدم;

        remove_اندراج(h, idx);

        أرجع قدر;
}

ساكن بنانشانی لبھو_پہلا_اندراج(Hashmap_ادھار *h) {
        Iterator i = ITERATOR_پہلا;

        إذا (!h || !n_entries(h))
                أرجع IDX_NIL;

        أرجع hashmap_iterate_اندراج(h, &i);
}

فراغ* hashmap_پہلا_کلید_and_قدر(Hashmap_ادھار *h, bool remove, فراغ **ret_کلید) {
        بنية hashmap_ادھار_اندراج *e;
        فراغ *کلید, *انکڑے;
        بنانشانی idx;

        idx = لبھو_پہلا_اندراج(h);
        إذا (idx == IDX_NIL) {
                إذا (ret_کلید)
                        *ret_کلید = عدم;
                أرجع عدم;
        }

        e = بالٹی_at(h, idx);
        کلید = (فراغ*) e->کلید;
        انکڑے = اندراج_قدر(h, e);

        إذا (remove)
                remove_اندراج(h, idx);

        إذا (ret_کلید)
                *ret_کلید = کلید;

        أرجع انکڑے;
}

بنانشانی hashmap_ناپ(Hashmap_ادھار *h) {
        إذا (!h)
                أرجع 0;

        أرجع n_entries(h);
}

بنانشانی _hashmap_buckets(Hashmap_ادھار *h) {
        إذا (!h)
                أرجع 0;

        أرجع n_buckets(h);
}

صحيح hashmap_ملاؤ(Hashmap *h, Hashmap *other) {
        Iterator i;
        بنانشانی idx;

        أكد(h);

        HASHMAP_FOREACH_IDX(idx, HASHMAP_ادھار(other), i) {
                بنية plain_hashmap_اندراج *pe = plain_بالٹی_at(other, idx);
                صحيح r;

                r = hashmap_put(h, pe->b.کلید, pe->قدر);
                إذا (r < 0 && r != -EEXIST)
                        أرجع r;
        }

        أرجع 0;
}

صحيح سیٹ_ملاؤ(سیٹ_2 *s, سیٹ_2 *other) {
        Iterator i;
        بنانشانی idx;

        أكد(s);

        HASHMAP_FOREACH_IDX(idx, HASHMAP_ادھار(other), i) {
                بنية سیٹ_اندراج *se = سیٹ_بالٹی_at(other, idx);
                صحيح r;

                r = سیٹ_put(s, se->b.کلید);
                إذا (r < 0)
                        أرجع r;
        }

        أرجع 0;
}

صحيح _hashmap_reserve(Hashmap_ادھار *h, بنانشانی entries_جوڑو) {
        صحيح r;

        أكد(h);

        r = resize_buckets(h, entries_جوڑو);
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
صحيح _hashmap_move(Hashmap_ادھار *h, Hashmap_ادھار *other) {
        بنية وٹاندرا_entries وٹاندرا;
        بنية hashmap_ادھار_اندراج *e, *n;
        Iterator i;
        بنانشانی idx;
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
                بنانشانی h_hash;

                e = بالٹی_at(other, idx);
                h_hash = بالٹی_hash(h, e->کلید);
                إذا (بالٹی_جانچ(h, h_hash, e->کلید) != IDX_NIL)
                        استمر;

                n = &بالٹی_at_وٹاندرا(&وٹاندرا, IDX_PUT)->p.b;
                n->کلید = e->کلید;
                إذا (h->type != HASHMAP_TYPE_سیٹ)
                        ((بنية plain_hashmap_اندراج*) n)->قدر =
                                ((بنية plain_hashmap_اندراج*) e)->قدر;
                assert_se(hashmap_put_boldly(h, h_hash, &وٹاندرا, false) == 1);

                remove_اندراج(other, idx);
        }

        أرجع 0;
}

صحيح _hashmap_move_one(Hashmap_ادھار *h, Hashmap_ادھار *other, ثابت فراغ *کلید) {
        بنية وٹاندرا_entries وٹاندرا;
        بنانشانی h_hash, other_hash, idx;
        بنية hashmap_ادھار_اندراج *e, *n;
        صحيح r;

        أكد(h);

        h_hash = بالٹی_hash(h, کلید);
        إذا (بالٹی_جانچ(h, h_hash, کلید) != IDX_NIL)
                أرجع -EEXIST;

        إذا (!other)
                أرجع -ENOENT;

        أكد(other->type == h->type);

        other_hash = بالٹی_hash(other, کلید);
        idx = بالٹی_جانچ(other, other_hash, کلید);
        إذا (idx == IDX_NIL)
                أرجع -ENOENT;

        e = بالٹی_at(other, idx);

        n = &بالٹی_at_وٹاندرا(&وٹاندرا, IDX_PUT)->p.b;
        n->کلید = e->کلید;
        إذا (h->type != HASHMAP_TYPE_سیٹ)
                ((بنية plain_hashmap_اندراج*) n)->قدر =
                        ((بنية plain_hashmap_اندراج*) e)->قدر;
        r = hashmap_put_boldly(h, h_hash, &وٹاندرا, true);
        إذا (r < 0)
                أرجع r;

        remove_اندراج(other, idx);
        أرجع 0;
}

Hashmap_ادھار* _hashmap_copy(Hashmap_ادھار *h) {
        Hashmap_ادھار *copy;
        صحيح r;

        أكد(h);

        copy = hashmap_ادھار_new(h->hash_ops, h->type);
        إذا (!copy)
                أرجع عدم;

        بدل (h->type) {
        حالة HASHMAP_TYPE_PLAIN:
        حالة HASHMAP_TYPE_ORDERED:
                r = hashmap_ملاؤ_2((Hashmap*)copy, (Hashmap*)h);
                اكسر;
        حالة HASHMAP_TYPE_سیٹ:
                r = سیٹ_ملاؤ((سیٹ_2*)copy, (سیٹ_2*)h);
                اكسر;
        افتراضي:
                assert_not_reached();
        }

        إذا (r < 0)
                أرجع _hashmap_free(copy);

        أرجع copy;
}

حرف** _hashmap_get_strv(Hashmap_ادھار *h) {
        حرف **sv;
        Iterator i;
        بنانشانی idx, n;

        إذا (!h)
                أرجع new0(حرف*, 1);

        sv = new(حرف*, n_entries(h)+1);
        إذا (!sv)
                أرجع عدم;

        n = 0;
        HASHMAP_FOREACH_IDX(idx, h, i)
                sv[n++] = اندراج_قدر(h, بالٹی_at(h, idx));
        sv[n] = عدم;

        أرجع sv;
}

حرف** سیٹ_منزل_strv(سیٹ_2 **s) {
        أكد(s);

        /* This is similar to set_get_strv(), but invalidates the set on success. */

        حرف **v = new(حرف*, سیٹ_ناپ(*s) + 1);
        إذا (!v)
                أرجع عدم;

        لكل (حرف **p = v; (*p = سیٹ_steal_پہلا(*s)); p++)
                ;

        أكد(سیٹ_isempty(*s));
        *s = سیٹ_free(*s);
        أرجع v;
}

فراغ* ordered_hashmap_اگلا(OrderedHashmap *h, ثابت فراغ *کلید) {
        بنية ordered_hashmap_اندراج *e;
        بنانشانی hash, idx;

        إذا (!h)
                أرجع عدم;

        hash = بالٹی_hash(h, کلید);
        idx = بالٹی_جانچ(h, hash, کلید);
        إذا (idx == IDX_NIL)
                أرجع عدم;

        e = ordered_بالٹی_at(h, idx);
        إذا (e->iterate_اگلا == IDX_NIL)
                أرجع عدم;
        أرجع ordered_بالٹی_at(h, e->iterate_اگلا)->p.قدر;
}

صحيح سیٹ_consume(سیٹ_2 *s, فراغ *قدر) {
        صحيح r;

        أكد(s);
        أكد(قدر);

        r = سیٹ_put(s, قدر);
        إذا (r <= 0)
                حرر(قدر);

        أرجع r;
}

صحيح hashmap_put_strdup_بھریا(Hashmap **h, ثابت بنية hash_ops *hash_ops, ثابت حرف *k, ثابت حرف *v) {
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

صحيح سیٹ_put_strndup_بھریا(سیٹ_2 **s, ثابت بنية hash_ops *hash_ops, ثابت حرف *p, size_t n) {
        حرف *c;
        صحيح r;

        أكد(s);
        أكد(p);

        r = سیٹ_ensure_allocated(s, hash_ops);
        إذا (r < 0)
                أرجع r;

        إذا (n == SIZE_MAX) {
                إذا (سیٹ_شامل_اے(*s, (حرف*) p))
                        أرجع 0;

                c = strdup(p);
        } وإلا
                c = strndup(p, n);
        إذا (!c)
                أرجع -ENOMEM;

        أرجع سیٹ_consume(*s, c);
}

صحيح سیٹ_put_strdupv_بھریا(سیٹ_2 **s, ثابت بنية hash_ops *hash_ops, حرف **l) {
        صحيح n = 0, r;

        أكد(s);

        STRV_FOREACH(i, l) {
                r = سیٹ_put_strndup_بھریا(s, hash_ops, *i, SIZE_MAX);
                إذا (r < 0)
                        أرجع r;

                n += r;
        }

        أرجع n;
}

صحيح سیٹ_put_strsplit(سیٹ_2 *s, ثابت حرف *v, ثابت حرف *separators, کڈھو_Flags flags) {
        ثابت حرف *p = ASSERT_PTR(v);
        صحيح r;

        أكد(s);

        لكل (;;) {
                حرف *لفظ;

                r = کڈھو_پہلا_لفظ(&p, &لفظ, separators, flags);
                إذا (r <= 0)
                        أرجع r;

                r = سیٹ_consume(s, لفظ);
                إذا (r < 0)
                        أرجع r;
        }
}

/* expand the cachemem if needed, return true if newly (re)activated. */
ساكن صحيح cachemem_maintain(CacheMem *mem, size_t ناپ) {
        أكد(mem);

        إذا (!GREEDY_REALLOC(mem->ptr, ناپ)) {
                إذا (ناپ > 0)
                        أرجع -ENOMEM;
        }

        إذا (!mem->سرگرم) {
                mem->سرگرم = true;
                أرجع true;
        }

        أرجع false;
}

صحيح iterated_cache_get(IteratedCache *cache, ثابت فراغ ***res_کلیدیں, ثابت فراغ ***res_اقدار, بنانشانی *res_n_entries) {
        bool sync_کلیدیں = false, sync_اقدار = false;
        size_t ناپ;
        صحيح r;

        أكد(cache);
        أكد(cache->hashmap);

        ناپ = n_entries(cache->hashmap);

        إذا (res_کلیدیں) {
                r = cachemem_maintain(&cache->کلیدیں, ناپ);
                إذا (r < 0)
                        أرجع r;

                sync_کلیدیں = r;
        } وإلا
                cache->کلیدیں.سرگرم = false;

        إذا (res_اقدار) {
                r = cachemem_maintain(&cache->values, ناپ);
                إذا (r < 0)
                        أرجع r;

                sync_اقدار = r;
        } وإلا
                cache->values.سرگرم = false;

        إذا (cache->hashmap->dirty) {
                إذا (cache->کلیدیں.سرگرم)
                        sync_کلیدیں = true;
                إذا (cache->values.سرگرم)
                        sync_اقدار = true;

                cache->hashmap->dirty = false;
        }

        إذا (sync_کلیدیں || sync_اقدار) {
                بنانشانی i, idx;
                Iterator iter;

                i = 0;
                HASHMAP_FOREACH_IDX(idx, cache->hashmap, iter) {
                        بنية hashmap_ادھار_اندراج *e;

                        e = بالٹی_at(cache->hashmap, idx);

                        إذا (sync_کلیدیں)
                                cache->کلیدیں.ptr[i] = e->کلید;
                        إذا (sync_اقدار)
                                cache->values.ptr[i] = اندراج_قدر(cache->hashmap, e);
                        i++;
                }
        }

        إذا (res_کلیدیں)
                *res_کلیدیں = cache->کلیدیں.ptr;
        إذا (res_اقدار)
                *res_اقدار = cache->values.ptr;
        إذا (res_n_entries)
                *res_n_entries = ناپ;

        أرجع 0;
}

IteratedCache* iterated_cache_free(IteratedCache *cache) {
        إذا (cache) {
                حرر(cache->کلیدیں.ptr);
                حرر(cache->values.ptr);
        }

        أرجع mfree(cache);
}

صحيح سیٹ_strjoin(سیٹ_2 *s, ثابت حرف *separator, bool wrap_with_separator, حرف **ret) {
        _cleanup_free_ حرف *str = عدم;
        size_t separator_len, len = 0;
        ثابت حرف *قدر;
        bool پہلا;

        أكد(ret);

        إذا (سیٹ_isempty(s)) {
                *ret = عدم;
                أرجع 0;
        }

        separator_len = strlen_ptr(separator);

        إذا (separator_len == 0)
                wrap_with_separator = false;

        پہلا = !wrap_with_separator;

        سیٹ_FOREACH(قدر, s) {
                size_t l = strlen_ptr(قدر);

                إذا (l == 0)
                        استمر;

                إذا (!GREEDY_REALLOC(str, len + l + (پہلا ? 0 : separator_len) + (wrap_with_separator ? separator_len : 0) + 1))
                        أرجع -ENOMEM;

                إذا (separator_len > 0 && !پہلا) {
                        حافظہ_نقل(str + len, separator, separator_len);
                        len += separator_len;
                }

                حافظہ_نقل(str + len, قدر, l);
                len += l;
                پہلا = false;
        }

        إذا (wrap_with_separator) {
                حافظہ_نقل(str + len, separator, separator_len);
                len += separator_len;
        }

        str[len] = '\0';

        *ret = TAKE_PTR(str);
        أرجع 0;
}

bool سیٹ_equal(سیٹ_2 *a, سیٹ_2 *b) {
        فراغ *p;

        /* Checks whether each entry of 'a' is also in 'b' and vice versa, i.e. the two sets contain the same
         * entries */

        إذا (a == b)
                أرجع true;

        إذا (سیٹ_isempty(a) && سیٹ_isempty(b))
                أرجع true;

        إذا (سیٹ_ناپ(a) != سیٹ_ناپ(b)) /* Cheap check that hopefully catches a lot of inequality cases
                                         * already */
                أرجع false;

        سیٹ_FOREACH(p, a)
                إذا (!سیٹ_شامل_اے(b, p))
                        أرجع false;

        /* If we have the same hashops, then we don't need to check things backwards given we compared the
         * size and that all of a is in b. */
        إذا (a->b.hash_ops == b->b.hash_ops)
                أرجع true;

        سیٹ_FOREACH(p, b)
                إذا (!سیٹ_شامل_اے(a, p))
                        أرجع false;

        أرجع true;
}

ساكن bool سیٹ_fnmatch_one(سیٹ_2 *patterns, ثابت حرف *needle) {
        ثابت حرف *p;

        أكد(needle);

        /* Any failure of fnmatch() is treated as equivalent to FNM_NOMATCH, i.e. as non-matching pattern */

        سیٹ_FOREACH(p, patterns)
                إذا (fnmatch(p, needle, 0) == 0)
                        أرجع true;

        أرجع false;
}

bool سیٹ_fnmatch(سیٹ_2 *include_patterns, سیٹ_2 *exclude_patterns, ثابت حرف *needle) {
        أكد(needle);

        إذا (سیٹ_fnmatch_one(exclude_patterns, needle))
                أرجع false;

        إذا (سیٹ_isempty(include_patterns))
                أرجع true;

        أرجع سیٹ_fnmatch_one(include_patterns, needle);
}

ساكن صحيح hashmap_اندراج_compare(
                بنية hashmap_ادھار_اندراج * ثابت *a,
                بنية hashmap_ادھار_اندراج * ثابت *b,
                compare_func_t compare) {

        أكد(a && *a);
        أكد(b && *b);
        أكد(compare);

        أرجع compare((*a)->کلید, (*b)->کلید);
}

ساكن صحيح _hashmap_dump_entries_sorted(
                Hashmap_ادھار *h,
                فراغ ***ret,
                size_t *ret_n) {
        _cleanup_free_ فراغ **entries = عدم;
        Iterator iter;
        بنانشانی idx;
        size_t n = 0;

        أكد(ret);
        أكد(ret_n);

        إذا (hashmap_ناپ(h) == 0) {
                *ret = عدم;
                *ret_n = 0;
                أرجع 0;
        }

        /* We append one more element than needed so that the resulting array can be used as a strv. We
         * don't count this entry in the returned size. */
        entries = new(فراغ*, hashmap_ناپ(h) + 1);
        إذا (!entries)
                أرجع -ENOMEM;

        HASHMAP_FOREACH_IDX(idx, h, iter)
                entries[n++] = بالٹی_at(h, idx);

        أكد(n == hashmap_ناپ(h));
        entries[n] = عدم;

        typesafe_qsort_r((بنية hashmap_ادھار_اندراج**) entries, n,
                         hashmap_اندراج_compare, h->hash_ops->compare);

        *ret = TAKE_PTR(entries);
        *ret_n = n;
        أرجع 0;
}

صحيح hashmap_dump_کلیدیں_sorted(Hashmap_ادھار *h, فراغ ***ret, size_t *ret_n) {
        _cleanup_free_ فراغ **entries = عدم;
        size_t n;
        صحيح r;

        أكد(ret);

        r = _hashmap_dump_entries_sorted(h, &entries, &n);
        إذا (r < 0)
                أرجع r;

        /* Reuse the array. */
        FOREACH_ARRAY(e, entries, n)
                *e = (فراغ*) (*(بنية hashmap_ادھار_اندراج**) e)->کلید;

        *ret = TAKE_PTR(entries);
        إذا (ret_n)
                *ret_n = n;
        أرجع 0;
}

صحيح _hashmap_dump_sorted(Hashmap_ادھار *h, فراغ ***ret, size_t *ret_n) {
        _cleanup_free_ فراغ **entries = عدم;
        size_t n;
        صحيح r;

        أكد(ret);

        r = _hashmap_dump_entries_sorted(h, &entries, &n);
        إذا (r < 0)
                أرجع r;

        /* Reuse the array. */
        FOREACH_ARRAY(e, entries, n)
                *e = اندراج_قدر(h, *(بنية hashmap_ادھار_اندراج**) e);

        *ret = TAKE_PTR(entries);
        إذا (ret_n)
                *ret_n = n;
        أرجع 0;
}
