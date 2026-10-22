/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <fnmatch.h>
#شامل <pthread.h>
#شامل <unistd.h>
#اگر HAVE_VALGRIND_VALGRIND_H
#  include <valgrind/valgrind.h>
#اگر_ختم

#شامل "alloc-util.h"
#شامل "extract-word.h"
#شامل "hashmap.h"
#شامل "log.h"
#شامل "logarithm.h"
#شامل "memory-util.h"
#شامل "mempool.h"
#شامل "process-util.h"
#شامل "random-util.h"
#شامل "set.h"
#شامل "siphash24.h"
#شامل "sort-util.h"
#شامل "string-util.h"
#شامل "strv.h"

#اگر ENABLE_DEBUG_HASHMAP
#شامل "list.h"
#اگر_ختم

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
ساخت hashmap_بنیاد_اندراج {
        ثابت خالی *کلید;
};

/* Entry types for specific hashmap/set types
 * hashmap_base_entry must be at the beginning of each entry struct. */

ساخت plain_hashmap_اندراج {
        ساخت hashmap_بنیاد_اندراج b;
        خالی *قدر;
};

ساخت ordered_hashmap_اندراج {
        ساخت plain_hashmap_اندراج p;
        غیرعلامتی iterate_اگلا, iterate_پچھلا;
};

ساخت سیٹ_اندراج {
        ساخت hashmap_بنیاد_اندراج b;
};

/* In several functions it is advantageous to have the hash table extended
 * virtually by a couple of additional buckets. We reserve special index values
 * for these "swap" buckets. */
#تعریف_کرو IDX_تبادلہ_BEGIN     (UINT_MAX - 3)
#تعریف_کرو IDX_PUT             (IDX_تبادلہ_BEGIN + 0)
#تعریف_کرو IDX_TMP             (IDX_تبادلہ_BEGIN + 1)
#تعریف_کرو IDX_تبادلہ_انتہا       (IDX_تبادلہ_BEGIN + 2)

#تعریف_کرو IDX_پہلا           (UINT_MAX - 1) /* special index for freshly initialized iterators */
#تعریف_کرو IDX_NIL             UINT_MAX       /* special index value meaning "none" or "end" */

assert_cc(IDX_پہلا == IDX_تبادلہ_انتہا);
assert_cc(IDX_پہلا == IDX_ITERATOR_پہلا);

/* Storage space for the "swap" buckets.
 * All entry types can fit into an ordered_hashmap_entry. */
ساخت تبادلہ_entries {
        ساخت ordered_hashmap_اندراج e[IDX_تبادلہ_انتہا - IDX_تبادلہ_BEGIN];
};

/* Distance from Initial Bucket */
قسمتعریف uint8_t dib_raw_t;
#تعریف_کرو DIB_RAW_حد_سے_تجاوز ((dib_raw_t)0xfdU)   /* indicates DIB value is greater than representable */
#تعریف_کرو DIB_RAW_REHASH   ((dib_raw_t)0xfeU)   /* entry yet to be rehashed during in-place resize */
#تعریف_کرو DIB_RAW_FREE     ((dib_raw_t)0xffU)   /* a free bucket */
#تعریف_کرو DIB_RAW_INIT     ((حرف)DIB_RAW_FREE) /* a byte to memset a DIB store with when initializing */

#تعریف_کرو DIB_FREE UINT_MAX

#اگر ENABLE_DEBUG_HASHMAP
ساخت hashmap_debug_info {
        LIST_FIELDS(ساخت hashmap_debug_info, debug_list);
        غیرعلامتی max_entries;  /* high watermark of n_entries */

        /* fields to detect modification while iterating */
        غیرعلامتی put_گنتی;    /* counts puts into the hashmap */
        غیرعلامتی rem_گنتی;    /* counts removals from hashmap */
        غیرعلامتی آخری_rem_idx; /* remembers last removal index */
};

/* Tracks all existing hashmaps. Get at it from gdb. See sd_dump_hashmaps.py */
ساکن LIST_سر(ساخت hashmap_debug_info, hashmap_debug_list);
ساکن pthread_mutex_t hashmap_debug_list_mutex = PTHREAD_MUTEX_INITIALIZER;
#اگر_ختم

شمار HashmapType {
        HASHMAP_TYPE_PLAIN,
        HASHMAP_TYPE_ORDERED,
        HASHMAP_TYPE_سیٹ,
        _HASHMAP_TYPE_MAX
};

ساخت _packed_ بالواسطہ_storage {
        خالی *storage;                     /* where buckets and DIBs are stored */
        uint8_t  hash_کلید[HASH_کلید_سائز];  /* hash key; changes during resize */

        غیرعلامتی n_entries;                /* number of stored entries */
        غیرعلامتی n_buckets;                /* number of buckets */

        غیرعلامتی idx_سب_سے_نیچا_اندراج;         /* Index below which all buckets are free.
                                              Makes "while (hashmap_steal_first())" loops
                                              O(n) instead of O(n^2) for unordered hashmaps. */
        uint8_t  _pad[3];                  /* padding for the whole HashmapBase */
        /* The bitfields in HashmapBase complete the alignment of the whole thing. */
};

ساخت براہ_راست_storage {
        /* This gives us 39 bytes on 64-bit, or 35 bytes on 32-bit.
         * That's room for 4 set_entries + 4 DIB bytes + 3 unused bytes on 64-bit,
         *              or 7 set_entries + 7 DIB bytes + 0 unused bytes on 32-bit. */
        uint8_t storage[حجم(ساخت بالواسطہ_storage)];
};

#تعریف_کرو براہ_راست_BUCKETS(اندراج_t) \
        (حجم(ساخت براہ_راست_storage) / (حجم(اندراج_t) + حجم(dib_raw_t)))

/* We should be able to store at least one entry directly. */
assert_cc(براہ_راست_BUCKETS(ساخت ordered_hashmap_اندراج) >= 1);

/* We have 3 bits for n_direct_entries. */
assert_cc(براہ_راست_BUCKETS(ساخت سیٹ_اندراج) < (1 << 3));

/* Hashmaps with directly stored entries all use this shared hash key.
 * It's no big deal if the key is guessed, because there can be only
 * a handful of directly stored entries in a hashmap. When a hashmap
 * outgrows direct storage, it gets its own key for indirect storage. */
ساکن uint8_t shared_hash_کلید[HASH_کلید_سائز];

/* Fields that all hashmap/set types must have */
ساخت Hashmap_بنیاد {
        ثابت ساخت hash_ops *hash_ops;  /* hash and compare ops to use */

        اتحاد _packed_ {
                ساخت بالواسطہ_storage بالواسطہ; /* if  has_indirect */
                ساخت براہ_راست_storage براہ_راست;     /* if !has_indirect */
        };

        شمار HashmapType type:2;     /* HASHMAP_TYPE_* */
        bool has_بالواسطہ:1;         /* whether indirect storage is used */
        غیرعلامتی n_براہ_راست_entries:3; /* Number of entries in direct storage.
                                      * Only valid if !has_indirect. */
        bool from_pool:1;            /* whether was allocated from mempool */
        bool dirty:1;                /* whether dirtied since last iterated_cache_get() */
        bool cached:1;               /* whether this hashmap is being cached */

#اگر ENABLE_DEBUG_HASHMAP
        ساخت hashmap_debug_info debug;
#اگر_ختم
};

/* Specific hash types
 * HashmapBase must be at the beginning of each hashmap struct. */

ساخت Hashmap {
        ساخت Hashmap_بنیاد b;
};

ساخت OrderedHashmap {
        ساخت Hashmap_بنیاد b;
        غیرعلامتی iterate_list_سر, iterate_list_دم;
};

ساخت سیٹ_2 {
        ساخت Hashmap_بنیاد b;
};

قسمتعریف ساخت CacheMem {
        ثابت خالی **ptr;
        size_t n_populated;
        bool فعال:1;
} CacheMem;

ساخت IteratedCache {
        Hashmap_بنیاد *hashmap;
        CacheMem کلیدیں, values;
};

DEFINE_MEMPOOL(hashmap_pool,         Hashmap,        8);
DEFINE_MEMPOOL(ordered_hashmap_pool, OrderedHashmap, 8);
/* No need for a separate Set pool */
assert_cc(حجم(Hashmap) == حجم(سیٹ_2));

ساخت hashmap_type_info {
        size_t سر_سائز;
        size_t اندراج_سائز;
        ساخت mempool *mempool;
        غیرعلامتی n_براہ_راست_buckets;
};

ساکن مستعمل_2 ثابت ساخت hashmap_type_info hashmap_type_info[_HASHMAP_TYPE_MAX] = {
        [HASHMAP_TYPE_PLAIN] = {
                .سر_سائز        = حجم(Hashmap),
                .اندراج_سائز       = حجم(ساخت plain_hashmap_اندراج),
                .mempool          = &hashmap_pool,
                .n_براہ_راست_buckets = براہ_راست_BUCKETS(ساخت plain_hashmap_اندراج),
        },
        [HASHMAP_TYPE_ORDERED] = {
                .سر_سائز        = حجم(OrderedHashmap),
                .اندراج_سائز       = حجم(ساخت ordered_hashmap_اندراج),
                .mempool          = &ordered_hashmap_pool,
                .n_براہ_راست_buckets = براہ_راست_BUCKETS(ساخت ordered_hashmap_اندراج),
        },
        [HASHMAP_TYPE_سیٹ] = {
                .سر_سائز        = حجم(سیٹ_2),
                .اندراج_سائز       = حجم(ساخت سیٹ_اندراج),
                .mempool          = &hashmap_pool,
                .n_براہ_راست_buckets = براہ_راست_BUCKETS(ساخت سیٹ_اندراج),
        },
};

خالی hashmap_trim_pools(خالی) {
        صحیح r;

        /* The pool is only allocated by the main thread, but the memory can be passed to other
         * threads. Let's clean up if we are the main thread and no other threads are live. */

        /* We build our own is_main_thread() here, which doesn't use C11 TLS based caching of the
         * result. That's because valgrind apparently doesn't like TLS to be used from a GCC destructor. */
        اگر (getpid() != gettid())
                واپس (خالی) log_debug("Not cleaning up memory pools, not in main thread.");

        r = get_process_دھاگے(0);
        اگر (r < 0)
                واپس (خالی) log_debug_errno(r, "Failed to determine number of threads, not cleaning up memory pools: %m");
        اگر (r != 1)
                واپس (خالی) log_debug("Not cleaning up memory pools, running in multi-threaded process.");

        mempool_trim(&hashmap_pool);
        mempool_trim(&ordered_hashmap_pool);
}

#اگر HAVE_VALGRIND_VALGRIND_H
_destructor_ ساکن خالی cleanup_pools(خالی) {
        /* Be nice to valgrind */
        اگر (RUNNING_ON_VALGRIND)
                hashmap_trim_pools();
}
#اگر_ختم

ساکن اندرونی غیرعلامتی n_buckets(Hashmap_بنیاد *h) {
        أكد(h);
        واپس h->has_بالواسطہ ? h->بالواسطہ.n_buckets
                               : hashmap_type_info[h->type].n_براہ_راست_buckets;
}

ساکن غیرعلامتی n_entries(Hashmap_بنیاد *h) {
        أكد(h);
        واپس h->has_بالواسطہ ? h->بالواسطہ.n_entries
                               : h->n_براہ_راست_entries;
}

ساکن خالی n_entries_inc(Hashmap_بنیاد *h) {
        أكد(h);

        اگر (h->has_بالواسطہ)
                h->بالواسطہ.n_entries++;
        ورنہ
                h->n_براہ_راست_entries++;
}

ساکن خالی n_entries_dec(Hashmap_بنیاد *h) {
        أكد(h);

        اگر (h->has_بالواسطہ)
                h->بالواسطہ.n_entries--;
        ورنہ
                h->n_براہ_راست_entries--;
}

ساکن اندرونی خالی* storage_ptr(Hashmap_بنیاد *h) {
        أكد(h);
        واپس h->has_بالواسطہ ? h->بالواسطہ.storage
                               : h->براہ_راست.storage;
}

ساکن اندرونی uint8_t* hash_کلید(Hashmap_بنیاد *h) {
        أكد(h);
        واپس h->has_بالواسطہ ? h->بالواسطہ.hash_کلید
                               : shared_hash_کلید;
}

ساکن غیرعلامتی بنیاد_بالٹی_hash(Hashmap_بنیاد *h, ثابت خالی *p) {
        ساخت siphash state;
        uint64_t hash;

        أكد(h);

        /* The keys of trivial_hash_ops and uint64_hash_ops fit in 64 bits. siphash24_uint64() hashes such
         * a key without the indirect call through hash_ops and keeps the SipHash state in registers. On
         * 32-bit and big-endian architectures, the result differs from the result of trivial_hash_func(),
         * which hashes the key in native byte order. Only this function maps keys to buckets, so the
         * difference does not matter. */
        اگر (h->hash_ops->hash == trivial_hash_func)
                hash = siphash24_uint64((uintptr_t) p, hash_کلید(h));
        ورنہ اگر (h->hash_ops->hash == (hash_func_t) uint64_hash_func)
                hash = siphash24_uint64(*(ثابت uint64_t*) p, hash_کلید(h));
        ورنہ {
                siphash24_init(&state, hash_کلید(h));
                h->hash_ops->hash(p, &state);
                hash = siphash24_finalize(&state);
        }

        /* n_buckets is not a power of two, so a modulo needs a division. Multiplying the upper 32 bits of
         * the hash with n_buckets and keeping the upper 32 bits of the product maps the hash to
         * [0, n_buckets) almost as evenly, without a division. */
        واپس (غیرعلامتی) (((hash >> 32) * n_buckets(h)) >> 32);
}
#تعریف_کرو بالٹی_hash(h, p) بنیاد_بالٹی_hash(HASHMAP_بنیاد(h), p)

ساکن خالی بنیاد_سیٹ_dirty(Hashmap_بنیاد *h) {
        أكد(h);

        h->dirty = true;
}
#تعریف_کرو hashmap_سیٹ_dirty(h) بنیاد_سیٹ_dirty(HASHMAP_بنیاد(h))

ساکن خالی get_hash_کلید(uint8_t hash_کلید[HASH_کلید_سائز], bool reuse_is_ok) {
        ساکن uint8_t current[HASH_کلید_سائز];
        ساکن bool current_initialized = false;

        /* Returns a hash function key to use. In order to keep things
         * fast we will not generate a new key each time we allocate a
         * new hash table. Instead, we'll just reuse the most recently
         * generated one, except if we never generated one or when we
         * are rehashing an entire hash table because we reached a
         * fill level */

        اگر (!current_initialized || !reuse_is_ok) {
                random_بائٹیں(current, حجم(current));
                current_initialized = true;
        }

        حافظہ_نقل(hash_کلید, current, حجم(current));
}

ساکن اندرونی ساخت hashmap_بنیاد_اندراج* بالٹی_at(Hashmap_بنیاد *h, غیرعلامتی idx) {
        أكد(h);
        واپس CAST_ALIGN_PTR(
                        ساخت hashmap_بنیاد_اندراج,
                        (uint8_t *) storage_ptr(h) + idx * hashmap_type_info[h->type].اندراج_سائز);
}

ساکن ساخت plain_hashmap_اندراج* plain_بالٹی_at(Hashmap *h, غیرعلامتی idx) {
        واپس (ساخت plain_hashmap_اندراج*) بالٹی_at(HASHMAP_بنیاد(h), idx);
}

ساکن ساخت ordered_hashmap_اندراج* ordered_بالٹی_at(OrderedHashmap *h, غیرعلامتی idx) {
        واپس (ساخت ordered_hashmap_اندراج*) بالٹی_at(HASHMAP_بنیاد(h), idx);
}

ساکن ساخت سیٹ_اندراج *سیٹ_بالٹی_at(سیٹ_2 *h, غیرعلامتی idx) {
        واپس (ساخت سیٹ_اندراج*) بالٹی_at(HASHMAP_بنیاد(h), idx);
}

ساکن ساخت ordered_hashmap_اندراج* بالٹی_at_تبادلہ(ساخت تبادلہ_entries *تبادلہ, غیرعلامتی idx) {
        أكد(تبادلہ);
        واپس &تبادلہ->e[idx - IDX_تبادلہ_BEGIN];
}

/* Returns a pointer to the bucket at index idx.
 * Understands real indexes and swap indexes, hence "_virtual". */
ساکن ساخت hashmap_بنیاد_اندراج* بالٹی_at_virtual(Hashmap_بنیاد *h, ساخت تبادلہ_entries *تبادلہ,
                                                    غیرعلامتی idx) {
        اگر (idx < IDX_تبادلہ_BEGIN)
                واپس بالٹی_at(h, idx);

        اگر (idx < IDX_تبادلہ_انتہا)
                واپس &بالٹی_at_تبادلہ(تبادلہ, idx)->p.b;

        assert_not_reached();
}

ساکن اندرونی dib_raw_t* dib_raw_ptr(Hashmap_بنیاد *h) {
        أكد(h);
        واپس (dib_raw_t*)
                ((uint8_t*) storage_ptr(h) + hashmap_type_info[h->type].اندراج_سائز * n_buckets(h));
}

ساکن غیرعلامتی بالٹی_فاصلہ(Hashmap_بنیاد *h, غیرعلامتی idx, غیرعلامتی from) {
        واپس idx >= from ? idx - from
                           : n_buckets(h) + idx - from;
}

ساکن غیرعلامتی بالٹی_calculate_dib(Hashmap_بنیاد *h, غیرعلامتی idx, dib_raw_t raw_dib) {
        غیرعلامتی initial_بالٹی;

        اگر (raw_dib == DIB_RAW_FREE)
                واپس DIB_FREE;

        اگر (_likely_(raw_dib < DIB_RAW_حد_سے_تجاوز))
                واپس raw_dib;

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
        واپس بالٹی_فاصلہ(h, idx, initial_بالٹی);
}

ساکن خالی بالٹی_سیٹ_dib(Hashmap_بنیاد *h, غیرعلامتی idx, غیرعلامتی dib) {
        dib_raw_ptr(h)[idx] = dib != DIB_FREE ? MIN(dib, DIB_RAW_حد_سے_تجاوز) : DIB_RAW_FREE;
}

ساکن غیرعلامتی skip_free_buckets(Hashmap_بنیاد *h, غیرعلامتی idx) {
        dib_raw_t *dibs;

        dibs = dib_raw_ptr(h);

        برائے ( ; idx < n_buckets(h); idx++)
                اگر (dibs[idx] != DIB_RAW_FREE)
                        واپس idx;

        واپس IDX_NIL;
}

ساکن خالی بالٹی_mark_free(Hashmap_بنیاد *h, غیرعلامتی idx) {
        أكد(h);

        memzero(بالٹی_at(h, idx), hashmap_type_info[h->type].اندراج_سائز);
        بالٹی_سیٹ_dib(h, idx, DIB_FREE);
}

ساکن خالی بالٹی_move_اندراج(Hashmap_بنیاد *h, ساخت تبادلہ_entries *تبادلہ,
                              غیرعلامتی from, غیرعلامتی to) {
        ساخت hashmap_بنیاد_اندراج *e_from, *e_مقصد;

        أكد(h);
        أكد(from != to);

        e_from = بالٹی_at_virtual(h, تبادلہ, from);
        e_مقصد   = بالٹی_at_virtual(h, تبادلہ, to);

        حافظہ_نقل(e_مقصد, e_from, hashmap_type_info[h->type].اندراج_سائز);

        اگر (h->type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*) h;
                ساخت ordered_hashmap_اندراج *le, *le_مقصد;

                le_مقصد = (ساخت ordered_hashmap_اندراج*) e_مقصد;

                اگر (le_مقصد->iterate_اگلا != IDX_NIL) {
                        le = (ساخت ordered_hashmap_اندراج*)
                             بالٹی_at_virtual(h, تبادلہ, le_مقصد->iterate_اگلا);
                        le->iterate_پچھلا = to;
                }

                اگر (le_مقصد->iterate_پچھلا != IDX_NIL) {
                        le = (ساخت ordered_hashmap_اندراج*)
                             بالٹی_at_virtual(h, تبادلہ, le_مقصد->iterate_پچھلا);
                        le->iterate_اگلا = to;
                }

                اگر (lh->iterate_list_سر == from)
                        lh->iterate_list_سر = to;
                اگر (lh->iterate_list_دم == from)
                        lh->iterate_list_دم = to;
        }
}

ساکن اندرونی غیرعلامتی اگلا_idx(Hashmap_بنیاد *h, غیرعلامتی idx) {
        واپس idx + 1U < n_buckets(h) ? idx + 1U : 0U;
}

ساکن غیرعلامتی پچھلا_idx(Hashmap_بنیاد *h, غیرعلامتی idx) {
        واپس idx > 0U ? idx - 1U : n_buckets(h) - 1U;
}

ساکن اندرونی خالی* اندراج_قدر(Hashmap_بنیاد *h, ساخت hashmap_بنیاد_اندراج *e) {
        أكد(h);
        أكد(e);

        انتخاب (h->type) {

        حالت HASHMAP_TYPE_PLAIN:
        حالت HASHMAP_TYPE_ORDERED:
                واپس ((ساخت plain_hashmap_اندراج*)e)->قدر;

        حالت HASHMAP_TYPE_سیٹ:
                واپس (خالی*) e->کلید;

        طےشدہ:
                assert_not_reached();
        }
}

ساکن خالی بنیاد_remove_اندراج(Hashmap_بنیاد *h, غیرعلامتی idx) {
        غیرعلامتی بایاں, دایاں, پچھلا, dib;
        dib_raw_t raw_dib, *dibs;

        dibs = dib_raw_ptr(h);
        أكد(dibs[idx] != DIB_RAW_FREE);

#اگر ENABLE_DEBUG_HASHMAP
        أكد(h);
        h->debug.rem_گنتی++;
        h->debug.آخری_rem_idx = idx;
#اگر_ختم

        بایاں = idx;
        /* Find the stop bucket ("right"). It is either free or has DIB == 0. */
        برائے (دایاں = اگلا_idx(h, بایاں); ; دایاں = اگلا_idx(h, دایاں)) {
                raw_dib = dibs[دایاں];
                اگر (اندر_سیٹ(raw_dib, 0, DIB_RAW_FREE))
                        توڑو;

                /* The buckets are not supposed to be all occupied and with DIB > 0.
                 * That would mean we could make everyone better off by shifting them
                 * backward. This scenario is impossible. */
                أكد(بایاں != دایاں);
        }

        اگر (h->type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*) h;
                ساخت ordered_hashmap_اندراج *le = ordered_بالٹی_at(lh, idx);

                اگر (le->iterate_اگلا != IDX_NIL)
                        ordered_بالٹی_at(lh, le->iterate_اگلا)->iterate_پچھلا = le->iterate_پچھلا;
                ورنہ
                        lh->iterate_list_دم = le->iterate_پچھلا;

                اگر (le->iterate_پچھلا != IDX_NIL)
                        ordered_بالٹی_at(lh, le->iterate_پچھلا)->iterate_اگلا = le->iterate_اگلا;
                ورنہ
                        lh->iterate_list_سر = le->iterate_اگلا;
        }

        /* Now shift all buckets in the interval (left, right) one step backwards */
        برائے (پچھلا = بایاں, بایاں = اگلا_idx(h, بایاں); بایاں != دایاں;
             پچھلا = بایاں, بایاں = اگلا_idx(h, بایاں)) {
                dib = بالٹی_calculate_dib(h, بایاں, dibs[بایاں]);
                أكد(dib != 0);
                بالٹی_move_اندراج(h, تہی, بایاں, پچھلا);
                بالٹی_سیٹ_dib(h, پچھلا, dib - 1);
        }

        بالٹی_mark_free(h, پچھلا);
        n_entries_dec(h);
        بنیاد_سیٹ_dirty(h);
}
#تعریف_کرو remove_اندراج(h, idx) بنیاد_remove_اندراج(HASHMAP_بنیاد(h), idx)

ساکن غیرعلامتی hashmap_iterate_اندر_insertion_ترتیب(OrderedHashmap *h, Iterator *i) {
        ساخت ordered_hashmap_اندراج *e;
        غیرعلامتی idx;

        أكد(h);
        أكد(i);

        اگر (i->idx == IDX_NIL)
                جاؤ at_انتہا;

        اگر (i->idx == IDX_پہلا && h->iterate_list_سر == IDX_NIL)
                جاؤ at_انتہا;

        اگر (i->idx == IDX_پہلا) {
                idx = h->iterate_list_سر;
                e = ordered_بالٹی_at(h, idx);
        } ورنہ {
                idx = i->idx;
                e = ordered_بالٹی_at(h, idx);
                /*
                 * We allow removing the current entry while iterating, but removal may cause
                 * a backward shift. The next entry may thus move one bucket to the left.
                 * To detect when it happens, we remember the key pointer of the entry we were
                 * going to iterate next. If it does not match, there was a backward shift.
                 */
                اگر (e->p.b.کلید != i->اگلا_کلید) {
                        idx = پچھلا_idx(HASHMAP_بنیاد(h), idx);
                        e = ordered_بالٹی_at(h, idx);
                }
                أكد(e->p.b.کلید == i->اگلا_کلید);
        }

#اگر ENABLE_DEBUG_HASHMAP
        i->پچھلا_idx = idx;
#اگر_ختم

        اگر (e->iterate_اگلا != IDX_NIL) {
                ساخت ordered_hashmap_اندراج *n;
                i->idx = e->iterate_اگلا;
                n = ordered_بالٹی_at(h, i->idx);
                i->اگلا_کلید = n->p.b.کلید;
        } ورنہ
                i->idx = IDX_NIL;

        واپس idx;

at_انتہا:
        i->idx = IDX_NIL;
        واپس IDX_NIL;
}

ساکن غیرعلامتی hashmap_iterate_اندر_اندرونی_ترتیب(Hashmap_بنیاد *h, Iterator *i) {
        غیرعلامتی idx;

        أكد(h);
        أكد(i);

        اگر (i->idx == IDX_NIL)
                جاؤ at_انتہا;

        اگر (i->idx == IDX_پہلا) {
                /* fast forward to the first occupied bucket */
                اگر (h->has_بالواسطہ) {
                        i->idx = skip_free_buckets(h, h->بالواسطہ.idx_سب_سے_نیچا_اندراج);
                        h->بالواسطہ.idx_سب_سے_نیچا_اندراج = i->idx;
                } ورنہ
                        i->idx = skip_free_buckets(h, 0);

                اگر (i->idx == IDX_NIL)
                        جاؤ at_انتہا;
        } ورنہ {
                ساخت hashmap_بنیاد_اندراج *e;

                أكد(i->idx > 0);

                e = بالٹی_at(h, i->idx);
                /*
                 * We allow removing the current entry while iterating, but removal may cause
                 * a backward shift. The next entry may thus move one bucket to the left.
                 * To detect when it happens, we remember the key pointer of the entry we were
                 * going to iterate next. If it does not match, there was a backward shift.
                 */
                اگر (e->کلید != i->اگلا_کلید)
                        e = بالٹی_at(h, --i->idx);

                أكد(e->کلید == i->اگلا_کلید);
        }

        idx = i->idx;
#اگر ENABLE_DEBUG_HASHMAP
        i->پچھلا_idx = idx;
#اگر_ختم

        i->idx = skip_free_buckets(h, i->idx + 1);
        اگر (i->idx != IDX_NIL)
                i->اگلا_کلید = بالٹی_at(h, i->idx)->کلید;
        ورنہ
                i->idx = IDX_NIL;

        واپس idx;

at_انتہا:
        i->idx = IDX_NIL;
        واپس IDX_NIL;
}

ساکن غیرعلامتی hashmap_iterate_اندراج(Hashmap_بنیاد *h, Iterator *i) {
        اگر (!h) {
                i->idx = IDX_NIL;
                واپس IDX_NIL;
        }

#اگر ENABLE_DEBUG_HASHMAP
        اگر (i->idx == IDX_پہلا) {
                i->put_گنتی = h->debug.put_گنتی;
                i->rem_گنتی = h->debug.rem_گنتی;
        } ورنہ {
                /* While iterating, must not add any new entries */
                أكد(i->put_گنتی == h->debug.put_گنتی);
                /* ... or remove entries other than the current one */
                أكد(i->rem_گنتی == h->debug.rem_گنتی ||
                       (i->rem_گنتی == h->debug.rem_گنتی - 1 &&
                        i->پچھلا_idx == h->debug.آخری_rem_idx));
                /* Reset our removals counter */
                i->rem_گنتی = h->debug.rem_گنتی;
        }
#اگر_ختم

        واپس h->type == HASHMAP_TYPE_ORDERED ? hashmap_iterate_اندر_insertion_ترتیب((OrderedHashmap*) h, i)
                                               : hashmap_iterate_اندر_اندرونی_ترتیب(h, i);
}

bool _hashmap_iterate(Hashmap_بنیاد *h, Iterator *i, خالی **قدر, ثابت خالی **کلید) {
        ساخت hashmap_بنیاد_اندراج *e;
        خالی *ڈیٹا;
        غیرعلامتی idx;

        idx = hashmap_iterate_اندراج(h, i);
        اگر (idx == IDX_NIL) {
                اگر (قدر)
                        *قدر = تہی;
                اگر (کلید)
                        *کلید = تہی;

                واپس false;
        }

        e = بالٹی_at(h, idx);
        ڈیٹا = اندراج_قدر(h, e);
        اگر (قدر)
                *قدر = ڈیٹا;
        اگر (کلید)
                *کلید = e->کلید;

        واپس true;
}

#تعریف_کرو HASHMAP_FOREACH_IDX(idx, h, i) \
        برائے ((i) = ITERATOR_پہلا, (idx) = hashmap_iterate_اندراج((h), &(i)); \
             (idx != IDX_NIL); \
             (idx) = hashmap_iterate_اندراج((h), &(i)))

IteratedCache* _hashmap_iterated_cache_new(Hashmap_بنیاد *h) {
        IteratedCache *cache;

        أكد(h);
        أكد(!h->cached);

        اگر (h->cached)
                واپس تہی;

        cache = new0(IteratedCache, 1);
        اگر (!cache)
                واپس تہی;

        cache->hashmap = h;
        h->cached = true;

        واپس cache;
}

ساکن خالی reset_براہ_راست_storage(Hashmap_بنیاد *h) {
        ثابت ساخت hashmap_type_info *hi = &hashmap_type_info[h->type];
        خالی *p;

        أكد(!h->has_بالواسطہ);

        p = mempset(h->براہ_راست.storage, 0, hi->اندراج_سائز * hi->n_براہ_راست_buckets);
        memset(p, DIB_RAW_INIT, حجم(dib_raw_t) * hi->n_براہ_راست_buckets);
}

ساکن خالی shared_hash_کلید_initialize(خالی) {
        random_بائٹیں(shared_hash_کلید, حجم(shared_hash_کلید));
}

ساکن ساخت Hashmap_بنیاد* hashmap_بنیاد_new(ثابت ساخت hash_ops *hash_ops, شمار HashmapType type) {
        Hashmap_بنیاد *h;
        ثابت ساخت hashmap_type_info *hi = &hashmap_type_info[type];

        bool use_pool = mempool_enabled && mempool_enabled();  /* mempool_enabled is a weak symbol */

        h = use_pool ? mempool_alloc0_tile(hi->mempool) : malloc0(hi->سر_سائز);
        اگر (!h)
                واپس تہی;

        h->type = type;
        h->from_pool = use_pool;
        h->hash_ops = hash_ops ?: &trivial_hash_ops;

        اگر (type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*)h;
                lh->iterate_list_سر = lh->iterate_list_دم = IDX_NIL;
        }

        reset_براہ_راست_storage(h);

        ساکن pthread_once_t once = PTHREAD_ONCE_INIT;
        assert_se(pthread_once(&once, shared_hash_کلید_initialize) == 0);

#اگر ENABLE_DEBUG_HASHMAP
        assert_se(pthread_mutex_lock(&hashmap_debug_list_mutex) == 0);
        LIST_PREPEND(debug_list, hashmap_debug_list, &h->debug);
        assert_se(pthread_mutex_unlock(&hashmap_debug_list_mutex) == 0);
#اگر_ختم

        واپس h;
}

Hashmap *hashmap_new(ثابت ساخت hash_ops *hash_ops) {
        واپس (Hashmap*)        hashmap_بنیاد_new(hash_ops, HASHMAP_TYPE_PLAIN);
}

OrderedHashmap *ordered_hashmap_new(ثابت ساخت hash_ops *hash_ops) {
        واپس (OrderedHashmap*) hashmap_بنیاد_new(hash_ops, HASHMAP_TYPE_ORDERED);
}

سیٹ_2 *سیٹ_new(ثابت ساخت hash_ops *hash_ops) {
        واپس (سیٹ_2*)            hashmap_بنیاد_new(hash_ops, HASHMAP_TYPE_سیٹ);
}

ساکن صحیح hashmap_بنیاد_ensure_allocated(Hashmap_بنیاد **h, ثابت ساخت hash_ops *hash_ops,
                                         شمار HashmapType type) {
        Hashmap_بنیاد *q;

        أكد(h);

        اگر (*h) {
                أكد((*h)->hash_ops == (hash_ops ?: &trivial_hash_ops));
                واپس 0;
        }

        q = hashmap_بنیاد_new(hash_ops, type);
        اگر (!q)
                واپس -ENOMEM;

        *h = q;
        واپس 1;
}

صحیح hashmap_ensure_allocated(Hashmap **h, ثابت ساخت hash_ops *hash_ops) {
        واپس hashmap_بنیاد_ensure_allocated((Hashmap_بنیاد**)h, hash_ops, HASHMAP_TYPE_PLAIN);
}

صحیح ordered_hashmap_ensure_allocated(OrderedHashmap **h, ثابت ساخت hash_ops *hash_ops) {
        واپس hashmap_بنیاد_ensure_allocated((Hashmap_بنیاد**)h, hash_ops, HASHMAP_TYPE_ORDERED);
}

صحیح سیٹ_ensure_allocated(سیٹ_2 **s, ثابت ساخت hash_ops *hash_ops) {
        واپس hashmap_بنیاد_ensure_allocated((Hashmap_بنیاد**)s, hash_ops, HASHMAP_TYPE_سیٹ);
}

صحیح hashmap_ensure_put(Hashmap **h, ثابت ساخت hash_ops *hash_ops, ثابت خالی *کلید, خالی *قدر) {
        صحیح r;

        أكد(h);

        r = hashmap_ensure_allocated(h, hash_ops);
        اگر (r < 0)
                واپس r;

        واپس hashmap_put(*h, کلید, قدر);
}

صحیح ordered_hashmap_ensure_put(OrderedHashmap **h, ثابت ساخت hash_ops *hash_ops, ثابت خالی *کلید, خالی *قدر) {
        صحیح r;

        أكد(h);

        r = ordered_hashmap_ensure_allocated(h, hash_ops);
        اگر (r < 0)
                واپس r;

        واپس ordered_hashmap_put(*h, کلید, قدر);
}

صحیح ordered_hashmap_ensure_replace(OrderedHashmap **h, ثابت ساخت hash_ops *hash_ops, ثابت خالی *کلید, خالی *قدر) {
        صحیح r;

        أكد(h);

        r = ordered_hashmap_ensure_allocated(h, hash_ops);
        اگر (r < 0)
                واپس r;

        واپس ordered_hashmap_replace(*h, کلید, قدر);
}

صحیح hashmap_ensure_replace(Hashmap **h, ثابت ساخت hash_ops *hash_ops, ثابت خالی *کلید, خالی *قدر) {
        صحیح r;

        أكد(h);

        r = hashmap_ensure_allocated(h, hash_ops);
        اگر (r < 0)
                واپس r;

        واپس hashmap_replace(*h, کلید, قدر);
}

ساکن خالی hashmap_free_no_clear(Hashmap_بنیاد *h) {
        أكد(!h->has_بالواسطہ);
        أكد(h->n_براہ_راست_entries == 0);

#اگر ENABLE_DEBUG_HASHMAP
        assert_se(pthread_mutex_lock(&hashmap_debug_list_mutex) == 0);
        LIST_REMOVE(debug_list, hashmap_debug_list, &h->debug);
        assert_se(pthread_mutex_unlock(&hashmap_debug_list_mutex) == 0);
#اگر_ختم

        اگر (h->from_pool) {
                /* Ensure that the object didn't get migrated between threads. */
                assert_se(is_main_thread());
                mempool_free_tile(hashmap_type_info[h->type].mempool, h);
        } ورنہ
                آزاد(h);
}

Hashmap_بنیاد* _hashmap_free(Hashmap_بنیاد *h) {
        اگر (h) {
                _hashmap_clear(h);
                hashmap_free_no_clear(h);
        }

        واپس تہی;
}

خالی _hashmap_clear(Hashmap_بنیاد *h) {
        اگر (!h)
                واپس;

        اگر (h->hash_ops->free_کلید || h->hash_ops->free_قدر) {

                /* If destructor calls are defined, let's destroy things defensively: let's take the item out of the
                 * hash table, and only then call the destructor functions. If these destructors then try to unregister
                 * themselves from our hash table a second time, the entry is already gone. */

                جب (hashmap_سائز(h) > 0) {
                        خالی *k = تہی;
                        خالی *v;

                        v = hashmap_پہلا_کلید_and_قدر(h, true, &k);

                        اگر (h->hash_ops->free_کلید)
                                h->hash_ops->free_کلید(k);

                        اگر (h->hash_ops->free_قدر)
                                h->hash_ops->free_قدر(v);
                }
        }

        اگر (h->has_بالواسطہ) {
                آزاد(h->بالواسطہ.storage);
                h->has_بالواسطہ = false;
        }

        h->n_براہ_راست_entries = 0;
        reset_براہ_راست_storage(h);

        اگر (h->type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*) h;
                lh->iterate_list_سر = lh->iterate_list_دم = IDX_NIL;
        }

        بنیاد_سیٹ_dirty(h);
}

ساکن صحیح resize_buckets(Hashmap_بنیاد *h, غیرعلامتی entries_جوڑو);

/*
 * Finds an empty bucket to put an entry into, starting the scan at 'idx'.
 * Performs Robin Hood swaps as it goes. The entry to put must be placed
 * by the caller into swap slot IDX_PUT.
 * If used for in-place resizing, may leave a displaced entry in swap slot
 * IDX_PUT. Caller must rehash it next.
 * Returns: true if it left a displaced entry to rehash next in IDX_PUT,
 *          false otherwise.
 */
ساکن bool hashmap_put_robin_hood(Hashmap_بنیاد *h, غیرعلامتی idx,
                                   ساخت تبادلہ_entries *تبادلہ) {
        dib_raw_t raw_dib, *dibs;
        غیرعلامتی dib, فاصلہ;

#اگر ENABLE_DEBUG_HASHMAP
        أكد(h);
        h->debug.put_گنتی++;
#اگر_ختم

        dibs = dib_raw_ptr(h);

        برائے (فاصلہ = 0; ; فاصلہ++) {
                raw_dib = dibs[idx];
                اگر (اندر_سیٹ(raw_dib, DIB_RAW_FREE, DIB_RAW_REHASH)) {
                        اگر (raw_dib == DIB_RAW_REHASH)
                                بالٹی_move_اندراج(h, تبادلہ, idx, IDX_TMP);

                        اگر (h->has_بالواسطہ && h->بالواسطہ.idx_سب_سے_نیچا_اندراج > idx)
                                h->بالواسطہ.idx_سب_سے_نیچا_اندراج = idx;

                        بالٹی_سیٹ_dib(h, idx, فاصلہ);
                        بالٹی_move_اندراج(h, تبادلہ, IDX_PUT, idx);
                        اگر (raw_dib == DIB_RAW_REHASH) {
                                بالٹی_move_اندراج(h, تبادلہ, IDX_TMP, IDX_PUT);
                                واپس true;
                        }

                        واپس false;
                }

                dib = بالٹی_calculate_dib(h, idx, raw_dib);

                اگر (dib < فاصلہ) {
                        /* Found a wealthier entry. Go Robin Hood! */
                        بالٹی_سیٹ_dib(h, idx, فاصلہ);

                        /* swap the entries */
                        بالٹی_move_اندراج(h, تبادلہ, idx, IDX_TMP);
                        بالٹی_move_اندراج(h, تبادلہ, IDX_PUT, idx);
                        بالٹی_move_اندراج(h, تبادلہ, IDX_TMP, IDX_PUT);

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
ساکن صحیح hashmap_بنیاد_put_boldly(Hashmap_بنیاد *h, غیرعلامتی idx,
                                   ساخت تبادلہ_entries *تبادلہ, bool may_resize) {
        ساخت ordered_hashmap_اندراج *new_اندراج;
        صحیح r;

        أكد(idx < n_buckets(h));

        new_اندراج = بالٹی_at_تبادلہ(تبادلہ, IDX_PUT);

        اگر (may_resize) {
                r = resize_buckets(h, 1);
                اگر (r < 0)
                        واپس r;
                اگر (r > 0)
                        idx = بالٹی_hash(h, new_اندراج->p.b.کلید);
        }
        أكد(n_entries(h) < n_buckets(h));

        اگر (h->type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*) h;

                new_اندراج->iterate_اگلا = IDX_NIL;
                new_اندراج->iterate_پچھلا = lh->iterate_list_دم;

                اگر (lh->iterate_list_دم != IDX_NIL) {
                        ساخت ordered_hashmap_اندراج *old_دم;

                        old_دم = ordered_بالٹی_at(lh, lh->iterate_list_دم);
                        أكد(old_دم->iterate_اگلا == IDX_NIL);
                        old_دم->iterate_اگلا = IDX_PUT;
                }

                lh->iterate_list_دم = IDX_PUT;
                اگر (lh->iterate_list_سر == IDX_NIL)
                        lh->iterate_list_سر = IDX_PUT;
        }

        assert_se(hashmap_put_robin_hood(h, idx, تبادلہ) == false);

        n_entries_inc(h);
#اگر ENABLE_DEBUG_HASHMAP
        h->debug.max_entries = MAX(h->debug.max_entries, n_entries(h));
#اگر_ختم

        بنیاد_سیٹ_dirty(h);

        واپس 1;
}
#تعریف_کرو hashmap_put_boldly(h, idx, تبادلہ, may_resize) \
        hashmap_بنیاد_put_boldly(HASHMAP_بنیاد(h), idx, تبادلہ, may_resize)

/*
 * Returns 0 if resize is not needed.
 *         1 if successfully resized.
 *         -ENOMEM on allocation failure.
 */
ساکن صحیح resize_buckets(Hashmap_بنیاد *h, غیرعلامتی entries_جوڑو) {
        ساخت تبادلہ_entries تبادلہ;
        خالی *new_storage;
        dib_raw_t *old_dibs, *new_dibs;
        ثابت ساخت hashmap_type_info *hi;
        غیرعلامتی idx, optimal_idx;
        غیرعلامتی old_n_buckets, new_n_buckets, n_rehashed, new_n_entries;
        uint8_t new_سرکاؤ;
        bool rehash_اگلا;

        أكد(h);

        hi = &hashmap_type_info[h->type];
        new_n_entries = n_entries(h) + entries_جوڑو;

        /* overflow? */
        اگر (_unlikely_(new_n_entries < entries_جوڑو))
                واپس -ENOMEM;

        /* For direct storage we allow 100% load, because it's tiny. */
        اگر (!h->has_بالواسطہ && new_n_entries <= hi->n_براہ_راست_buckets)
                واپس 0;

        /*
         * Load factor = n/m = 1 - (1/INV_KEEP_FREE).
         * From it follows: m = n + n/(INV_KEEP_FREE - 1)
         */
        new_n_buckets = new_n_entries + new_n_entries / (INV_KEEP_FREE - 1);
        /* overflow? */
        اگر (_unlikely_(new_n_buckets < new_n_entries))
                واپس -ENOMEM;

        old_n_buckets = n_buckets(h);

        اگر (_likely_(new_n_buckets <= old_n_buckets))
                واپس 0;

        اگر (_unlikely_(new_n_buckets > UINT_MAX / (hi->اندراج_سائز + حجم(dib_raw_t))))
                واپس -ENOMEM;

        new_سرکاؤ = log2u_باری_اوپر(MAX(
                        new_n_buckets * (hi->اندراج_سائز + حجم(dib_raw_t)),
                        2 * حجم(ساخت براہ_راست_storage)));

        /* Realloc storage (buckets and DIB array). */
        new_storage = realloc(h->has_بالواسطہ ? h->بالواسطہ.storage : تہی,
                              1U << new_سرکاؤ);
        اگر (!new_storage)
                واپس -ENOMEM;

        /* Must upgrade direct to indirect storage. */
        اگر (!h->has_بالواسطہ) {
                حافظہ_نقل(new_storage, h->براہ_راست.storage,
                       old_n_buckets * (hi->اندراج_سائز + حجم(dib_raw_t)));
                h->بالواسطہ.n_entries = h->n_براہ_راست_entries;
                h->بالواسطہ.idx_سب_سے_نیچا_اندراج = 0;
                h->n_براہ_راست_entries = 0;
        }

        /* Get a new hash key. If we've just upgraded to indirect storage,
         * allow reusing a previously generated key. It's still a different key
         * from the shared one that we used for direct storage. */
        get_hash_کلید(h->بالواسطہ.hash_کلید, !h->has_بالواسطہ);

        h->has_بالواسطہ = true;
        h->بالواسطہ.storage = new_storage;
        h->بالواسطہ.n_buckets = (1U << new_سرکاؤ) /
                                (hi->اندراج_سائز + حجم(dib_raw_t));

        old_dibs = (dib_raw_t*)((uint8_t*) new_storage + hi->اندراج_سائز * old_n_buckets);
        new_dibs = dib_raw_ptr(h);

        /*
         * Move the DIB array to the new place, replacing valid DIB values with
         * DIB_RAW_REHASH to indicate all of the used buckets need rehashing.
         * Note: Overlap is not possible, because we have at least doubled the
         * number of buckets and dib_raw_t is smaller than any entry type.
         */
        برائے (idx = 0; idx < old_n_buckets; idx++) {
                أكد(old_dibs[idx] != DIB_RAW_REHASH);
                new_dibs[idx] = old_dibs[idx] == DIB_RAW_FREE ? DIB_RAW_FREE
                                                              : DIB_RAW_REHASH;
        }

        /* Zero the area of newly added entries (including the old DIB area) */
        memzero(بالٹی_at(h, old_n_buckets),
               (n_buckets(h) - old_n_buckets) * hi->اندراج_سائز);

        /* The upper half of the new DIB array needs initialization */
        memset(&new_dibs[old_n_buckets], DIB_RAW_INIT,
               (n_buckets(h) - old_n_buckets) * حجم(dib_raw_t));

        /* Rehash entries that need it */
        n_rehashed = 0;
        برائے (idx = 0; idx < old_n_buckets; idx++) {
                اگر (new_dibs[idx] != DIB_RAW_REHASH)
                        جاری;

                optimal_idx = بالٹی_hash(h, بالٹی_at(h, idx)->کلید);

                /*
                 * Not much to do if by luck the entry hashes to its current
                 * location. Just set its DIB.
                 */
                اگر (optimal_idx == idx) {
                        new_dibs[idx] = 0;
                        n_rehashed++;
                        جاری;
                }

                new_dibs[idx] = DIB_RAW_FREE;
                بالٹی_move_اندراج(h, &تبادلہ, idx, IDX_PUT);
                /* bucket_move_entry does not clear the source */
                memzero(بالٹی_at(h, idx), hi->اندراج_سائز);

                کرو {
                        /*
                         * Find the new bucket for the current entry. This may make
                         * another entry homeless and load it into IDX_PUT.
                         */
                        rehash_اگلا = hashmap_put_robin_hood(h, optimal_idx, &تبادلہ);
                        n_rehashed++;

                        /* Did the current entry displace another one? */
                        اگر (rehash_اگلا)
                                optimal_idx = بالٹی_hash(h, بالٹی_at_تبادلہ(&تبادلہ, IDX_PUT)->p.b.کلید);
                } جب (rehash_اگلا);
        }

        assert_se(n_rehashed == n_entries(h));

        واپس 1;
}

/*
 * Finds an entry with a matching key
 * Returns: index of the found entry, or IDX_NIL if not found.
 */
ساکن غیرعلامتی بنیاد_بالٹی_جائزہ(Hashmap_بنیاد *h, غیرعلامتی idx, ثابت خالی *کلید) {
        compare_func_t compare = h->hash_ops->compare;
        ساخت hashmap_بنیاد_اندراج *e;
        غیرعلامتی dib, فاصلہ;
        dib_raw_t *dibs = dib_raw_ptr(h);

        أكد(idx < n_buckets(h));

        برائے (فاصلہ = 0; ; فاصلہ++) {
                اگر (dibs[idx] == DIB_RAW_FREE)
                        واپس IDX_NIL;

                dib = بالٹی_calculate_dib(h, idx, dibs[idx]);

                اگر (dib < فاصلہ)
                        واپس IDX_NIL;
                اگر (dib == فاصلہ) {
                        e = بالٹی_at(h, idx);

                        /* Most maps compare keys by pointer. Do that here, without the indirect call. */
                        اگر (compare == trivial_compare_func ? e->کلید == کلید : compare(e->کلید, کلید) == 0)
                                واپس idx;
                }

                idx = اگلا_idx(h, idx);
        }
}
#تعریف_کرو بالٹی_جائزہ(h, idx, کلید) بنیاد_بالٹی_جائزہ(HASHMAP_بنیاد(h), idx, کلید)

صحیح hashmap_put(Hashmap *h, ثابت خالی *کلید, خالی *قدر) {
        ساخت تبادلہ_entries تبادلہ;
        ساخت plain_hashmap_اندراج *e;
        غیرعلامتی hash, idx;

        أكد(h);

        hash = بالٹی_hash(h, کلید);
        idx = بالٹی_جائزہ(h, hash, کلید);
        اگر (idx != IDX_NIL) {
                e = plain_بالٹی_at(h, idx);
                اگر (e->قدر == قدر)
                        واپس 0;
                واپس -EEXIST;
        }

        e = &بالٹی_at_تبادلہ(&تبادلہ, IDX_PUT)->p;
        e->b.کلید = کلید;
        e->قدر = قدر;
        واپس hashmap_put_boldly(h, hash, &تبادلہ, true);
}

صحیح سیٹ_put(سیٹ_2 *s, ثابت خالی *کلید) {
        ساخت تبادلہ_entries تبادلہ;
        ساخت hashmap_بنیاد_اندراج *e;
        غیرعلامتی hash, idx;

        أكد(s);

        hash = بالٹی_hash(s, کلید);
        idx = بالٹی_جائزہ(s, hash, کلید);
        اگر (idx != IDX_NIL)
                واپس 0;

        e = &بالٹی_at_تبادلہ(&تبادلہ, IDX_PUT)->p.b;
        e->کلید = کلید;
        واپس hashmap_put_boldly(s, hash, &تبادلہ, true);
}

صحیح سیٹ_ensure_put(سیٹ_2 **s, ثابت ساخت hash_ops *hash_ops, ثابت خالی *کلید) {
        صحیح r;

        أكد(s);

        r = سیٹ_ensure_allocated(s, hash_ops);
        اگر (r < 0)
                واپس r;

        واپس سیٹ_put(*s, کلید);
}

صحیح سیٹ_ensure_consume(سیٹ_2 **s, ثابت ساخت hash_ops *hash_ops, خالی *کلید) {
        صحیح r;

        r = سیٹ_ensure_put(s, hash_ops, کلید);
        اگر (r <= 0) {
                اگر (hash_ops && hash_ops->free_کلید)
                        hash_ops->free_کلید(کلید);
                ورنہ اگر (hash_ops && hash_ops->free_قدر)
                        /* Sets store their element in the key slot but may carry a value destructor. */
                        hash_ops->free_قدر(کلید);
                ورنہ
                        آزاد(کلید);
        }

        واپس r;
}

صحیح hashmap_replace(Hashmap *h, ثابت خالی *کلید, خالی *قدر) {
        ساخت تبادلہ_entries تبادلہ;
        ساخت plain_hashmap_اندراج *e;
        غیرعلامتی hash, idx;

        أكد(h);

        hash = بالٹی_hash(h, کلید);
        idx = بالٹی_جائزہ(h, hash, کلید);
        اگر (idx != IDX_NIL) {
                e = plain_بالٹی_at(h, idx);
#اگر ENABLE_DEBUG_HASHMAP
                /* Although the key is equal, the key pointer may have changed,
                 * and this would break our assumption for iterating. So count
                 * this operation as incompatible with iteration. */
                اگر (e->b.کلید != کلید) {
                        h->b.debug.put_گنتی++;
                        h->b.debug.rem_گنتی++;
                        h->b.debug.آخری_rem_idx = idx;
                }
#اگر_ختم
                e->b.کلید = کلید;
                e->قدر = قدر;
                hashmap_سیٹ_dirty(h);

                واپس 0;
        }

        e = &بالٹی_at_تبادلہ(&تبادلہ, IDX_PUT)->p;
        e->b.کلید = کلید;
        e->قدر = قدر;
        واپس hashmap_put_boldly(h, hash, &تبادلہ, true);
}

صحیح hashmap_update(Hashmap *h, ثابت خالی *کلید, خالی *قدر) {
        ساخت plain_hashmap_اندراج *e;
        غیرعلامتی hash, idx;

        أكد(h);

        hash = بالٹی_hash(h, کلید);
        idx = بالٹی_جائزہ(h, hash, کلید);
        اگر (idx == IDX_NIL)
                واپس -ENOENT;

        e = plain_بالٹی_at(h, idx);
        e->قدر = قدر;
        hashmap_سیٹ_dirty(h);

        واپس 0;
}

خالی* _hashmap_get(Hashmap_بنیاد *h, ثابت خالی *کلید) {
        ساخت hashmap_بنیاد_اندراج *e;
        غیرعلامتی hash, idx;

        اگر (!h)
                واپس تہی;

        hash = بالٹی_hash(h, کلید);
        idx = بالٹی_جائزہ(h, hash, کلید);
        اگر (idx == IDX_NIL)
                واپس تہی;

        e = بالٹی_at(h, idx);
        واپس اندراج_قدر(h, e);
}

خالی* hashmap_get2(Hashmap *h, ثابت خالی *کلید, خالی **ret) {
        ساخت plain_hashmap_اندراج *e;
        غیرعلامتی hash, idx;

        اگر (!h)
                واپس تہی;

        hash = بالٹی_hash(h, کلید);
        idx = بالٹی_جائزہ(h, hash, کلید);
        اگر (idx == IDX_NIL)
                واپس تہی;

        e = plain_بالٹی_at(h, idx);
        اگر (ret)
                *ret = (خالی*) e->b.کلید;

        واپس e->قدر;
}

bool hashmap_شامل_ہے_2(Hashmap_بنیاد *h, ثابت خالی *کلید) {
        غیرعلامتی hash;

        اگر (!h)
                واپس false;

        hash = بالٹی_hash(h, کلید);
        واپس بالٹی_جائزہ(h, hash, کلید) != IDX_NIL;
}

خالی* _hashmap_remove(Hashmap_بنیاد *h, ثابت خالی *کلید) {
        ساخت hashmap_بنیاد_اندراج *e;
        غیرعلامتی hash, idx;
        خالی *ڈیٹا;

        اگر (!h)
                واپس تہی;

        hash = بالٹی_hash(h, کلید);
        idx = بالٹی_جائزہ(h, hash, کلید);
        اگر (idx == IDX_NIL)
                واپس تہی;

        e = بالٹی_at(h, idx);
        ڈیٹا = اندراج_قدر(h, e);
        remove_اندراج(h, idx);

        واپس ڈیٹا;
}

خالی* hashmap_remove2(Hashmap *h, ثابت خالی *کلید, خالی **ret) {
        ساخت plain_hashmap_اندراج *e;
        غیرعلامتی hash, idx;
        خالی *ڈیٹا;

        اگر (!h) {
                اگر (ret)
                        *ret = تہی;
                واپس تہی;
        }

        hash = بالٹی_hash(h, کلید);
        idx = بالٹی_جائزہ(h, hash, کلید);
        اگر (idx == IDX_NIL) {
                اگر (ret)
                        *ret = تہی;
                واپس تہی;
        }

        e = plain_بالٹی_at(h, idx);
        ڈیٹا = e->قدر;
        اگر (ret)
                *ret = (خالی*) e->b.کلید;

        remove_اندراج(h, idx);

        واپس ڈیٹا;
}

صحیح hashmap_remove_and_put(Hashmap *h, ثابت خالی *old_کلید, ثابت خالی *new_کلید, خالی *قدر) {
        ساخت تبادلہ_entries تبادلہ;
        ساخت plain_hashmap_اندراج *e;
        غیرعلامتی old_hash, new_hash, idx;

        اگر (!h)
                واپس -ENOENT;

        old_hash = بالٹی_hash(h, old_کلید);
        idx = بالٹی_جائزہ(h, old_hash, old_کلید);
        اگر (idx == IDX_NIL)
                واپس -ENOENT;

        new_hash = بالٹی_hash(h, new_کلید);
        اگر (بالٹی_جائزہ(h, new_hash, new_کلید) != IDX_NIL)
                واپس -EEXIST;

        remove_اندراج(h, idx);

        e = &بالٹی_at_تبادلہ(&تبادلہ, IDX_PUT)->p;
        e->b.کلید = new_کلید;
        e->قدر = قدر;
        assert_se(hashmap_put_boldly(h, new_hash, &تبادلہ, false) == 1);

        واپس 0;
}

صحیح سیٹ_remove_and_put(سیٹ_2 *s, ثابت خالی *old_کلید, ثابت خالی *new_کلید) {
        ساخت تبادلہ_entries تبادلہ;
        ساخت hashmap_بنیاد_اندراج *e;
        غیرعلامتی old_hash, new_hash, idx;

        اگر (!s)
                واپس -ENOENT;

        old_hash = بالٹی_hash(s, old_کلید);
        idx = بالٹی_جائزہ(s, old_hash, old_کلید);
        اگر (idx == IDX_NIL)
                واپس -ENOENT;

        new_hash = بالٹی_hash(s, new_کلید);
        اگر (بالٹی_جائزہ(s, new_hash, new_کلید) != IDX_NIL)
                واپس -EEXIST;

        remove_اندراج(s, idx);

        e = &بالٹی_at_تبادلہ(&تبادلہ, IDX_PUT)->p.b;
        e->کلید = new_کلید;
        assert_se(hashmap_put_boldly(s, new_hash, &تبادلہ, false) == 1);

        واپس 0;
}

صحیح hashmap_remove_and_replace(Hashmap *h, ثابت خالی *old_کلید, ثابت خالی *new_کلید, خالی *قدر) {
        ساخت تبادلہ_entries تبادلہ;
        ساخت plain_hashmap_اندراج *e;
        غیرعلامتی old_hash, new_hash, idx_old, idx_new;

        اگر (!h)
                واپس -ENOENT;

        old_hash = بالٹی_hash(h, old_کلید);
        idx_old = بالٹی_جائزہ(h, old_hash, old_کلید);
        اگر (idx_old == IDX_NIL)
                واپس -ENOENT;

        old_کلید = بالٹی_at(HASHMAP_بنیاد(h), idx_old)->کلید;

        new_hash = بالٹی_hash(h, new_کلید);
        idx_new = بالٹی_جائزہ(h, new_hash, new_کلید);
        اگر (idx_new != IDX_NIL)
                اگر (idx_old != idx_new) {
                        remove_اندراج(h, idx_new);
                        /* Compensate for a possible backward shift. */
                        اگر (old_کلید != بالٹی_at(HASHMAP_بنیاد(h), idx_old)->کلید)
                                idx_old = پچھلا_idx(HASHMAP_بنیاد(h), idx_old);
                        أكد(old_کلید == بالٹی_at(HASHMAP_بنیاد(h), idx_old)->کلید);
                }

        remove_اندراج(h, idx_old);

        e = &بالٹی_at_تبادلہ(&تبادلہ, IDX_PUT)->p;
        e->b.کلید = new_کلید;
        e->قدر = قدر;
        assert_se(hashmap_put_boldly(h, new_hash, &تبادلہ, false) == 1);

        واپس 0;
}

خالی* hashmap_remove_قدر(Hashmap_بنیاد *h, ثابت خالی *کلید, خالی *قدر) {
        ساخت hashmap_بنیاد_اندراج *e;
        غیرعلامتی hash, idx;

        اگر (!h)
                واپس تہی;

        hash = بالٹی_hash(h, کلید);
        idx = بالٹی_جائزہ(h, hash, کلید);
        اگر (idx == IDX_NIL)
                واپس تہی;

        e = بالٹی_at(h, idx);
        اگر (اندراج_قدر(h, e) != قدر)
                واپس تہی;

        remove_اندراج(h, idx);

        واپس قدر;
}

ساکن غیرعلامتی ڈھونڈو_پہلا_اندراج(Hashmap_بنیاد *h) {
        Iterator i = ITERATOR_پہلا;

        اگر (!h || !n_entries(h))
                واپس IDX_NIL;

        واپس hashmap_iterate_اندراج(h, &i);
}

خالی* hashmap_پہلا_کلید_and_قدر(Hashmap_بنیاد *h, bool remove, خالی **ret_کلید) {
        ساخت hashmap_بنیاد_اندراج *e;
        خالی *کلید, *ڈیٹا;
        غیرعلامتی idx;

        idx = ڈھونڈو_پہلا_اندراج(h);
        اگر (idx == IDX_NIL) {
                اگر (ret_کلید)
                        *ret_کلید = تہی;
                واپس تہی;
        }

        e = بالٹی_at(h, idx);
        کلید = (خالی*) e->کلید;
        ڈیٹا = اندراج_قدر(h, e);

        اگر (remove)
                remove_اندراج(h, idx);

        اگر (ret_کلید)
                *ret_کلید = کلید;

        واپس ڈیٹا;
}

غیرعلامتی hashmap_سائز(Hashmap_بنیاد *h) {
        اگر (!h)
                واپس 0;

        واپس n_entries(h);
}

غیرعلامتی _hashmap_buckets(Hashmap_بنیاد *h) {
        اگر (!h)
                واپس 0;

        واپس n_buckets(h);
}

صحیح hashmap_ملاؤ(Hashmap *h, Hashmap *other) {
        Iterator i;
        غیرعلامتی idx;

        أكد(h);

        HASHMAP_FOREACH_IDX(idx, HASHMAP_بنیاد(other), i) {
                ساخت plain_hashmap_اندراج *pe = plain_بالٹی_at(other, idx);
                صحیح r;

                r = hashmap_put(h, pe->b.کلید, pe->قدر);
                اگر (r < 0 && r != -EEXIST)
                        واپس r;
        }

        واپس 0;
}

صحیح سیٹ_ملاؤ(سیٹ_2 *s, سیٹ_2 *other) {
        Iterator i;
        غیرعلامتی idx;

        أكد(s);

        HASHMAP_FOREACH_IDX(idx, HASHMAP_بنیاد(other), i) {
                ساخت سیٹ_اندراج *se = سیٹ_بالٹی_at(other, idx);
                صحیح r;

                r = سیٹ_put(s, se->b.کلید);
                اگر (r < 0)
                        واپس r;
        }

        واپس 0;
}

صحیح _hashmap_reserve(Hashmap_بنیاد *h, غیرعلامتی entries_جوڑو) {
        صحیح r;

        أكد(h);

        r = resize_buckets(h, entries_جوڑو);
        اگر (r < 0)
                واپس r;

        واپس 0;
}

/*
 * The same as hashmap_merge(), but every new item from other is moved to h.
 * Keys already in h are skipped and stay in other.
 * Returns: 0 on success.
 *          -ENOMEM on alloc failure, in which case no move has been done.
 */
صحیح _hashmap_move(Hashmap_بنیاد *h, Hashmap_بنیاد *other) {
        ساخت تبادلہ_entries تبادلہ;
        ساخت hashmap_بنیاد_اندراج *e, *n;
        Iterator i;
        غیرعلامتی idx;
        صحیح r;

        أكد(h);

        اگر (!other)
                واپس 0;

        أكد(other->type == h->type);

        /*
         * This reserves buckets for the worst case, where none of other's
         * entries are yet present in h. This is preferable to risking
         * an allocation failure in the middle of the moving and having to
         * rollback or return a partial result.
         */
        r = resize_buckets(h, n_entries(other));
        اگر (r < 0)
                واپس r;

        HASHMAP_FOREACH_IDX(idx, other, i) {
                غیرعلامتی h_hash;

                e = بالٹی_at(other, idx);
                h_hash = بالٹی_hash(h, e->کلید);
                اگر (بالٹی_جائزہ(h, h_hash, e->کلید) != IDX_NIL)
                        جاری;

                n = &بالٹی_at_تبادلہ(&تبادلہ, IDX_PUT)->p.b;
                n->کلید = e->کلید;
                اگر (h->type != HASHMAP_TYPE_سیٹ)
                        ((ساخت plain_hashmap_اندراج*) n)->قدر =
                                ((ساخت plain_hashmap_اندراج*) e)->قدر;
                assert_se(hashmap_put_boldly(h, h_hash, &تبادلہ, false) == 1);

                remove_اندراج(other, idx);
        }

        واپس 0;
}

صحیح _hashmap_move_one(Hashmap_بنیاد *h, Hashmap_بنیاد *other, ثابت خالی *کلید) {
        ساخت تبادلہ_entries تبادلہ;
        غیرعلامتی h_hash, other_hash, idx;
        ساخت hashmap_بنیاد_اندراج *e, *n;
        صحیح r;

        أكد(h);

        h_hash = بالٹی_hash(h, کلید);
        اگر (بالٹی_جائزہ(h, h_hash, کلید) != IDX_NIL)
                واپس -EEXIST;

        اگر (!other)
                واپس -ENOENT;

        أكد(other->type == h->type);

        other_hash = بالٹی_hash(other, کلید);
        idx = بالٹی_جائزہ(other, other_hash, کلید);
        اگر (idx == IDX_NIL)
                واپس -ENOENT;

        e = بالٹی_at(other, idx);

        n = &بالٹی_at_تبادلہ(&تبادلہ, IDX_PUT)->p.b;
        n->کلید = e->کلید;
        اگر (h->type != HASHMAP_TYPE_سیٹ)
                ((ساخت plain_hashmap_اندراج*) n)->قدر =
                        ((ساخت plain_hashmap_اندراج*) e)->قدر;
        r = hashmap_put_boldly(h, h_hash, &تبادلہ, true);
        اگر (r < 0)
                واپس r;

        remove_اندراج(other, idx);
        واپس 0;
}

Hashmap_بنیاد* _hashmap_copy(Hashmap_بنیاد *h) {
        Hashmap_بنیاد *copy;
        صحیح r;

        أكد(h);

        copy = hashmap_بنیاد_new(h->hash_ops, h->type);
        اگر (!copy)
                واپس تہی;

        انتخاب (h->type) {
        حالت HASHMAP_TYPE_PLAIN:
        حالت HASHMAP_TYPE_ORDERED:
                r = hashmap_ملاؤ_2((Hashmap*)copy, (Hashmap*)h);
                توڑو;
        حالت HASHMAP_TYPE_سیٹ:
                r = سیٹ_ملاؤ((سیٹ_2*)copy, (سیٹ_2*)h);
                توڑو;
        طےشدہ:
                assert_not_reached();
        }

        اگر (r < 0)
                واپس _hashmap_free(copy);

        واپس copy;
}

حرف** _hashmap_get_strv(Hashmap_بنیاد *h) {
        حرف **sv;
        Iterator i;
        غیرعلامتی idx, n;

        اگر (!h)
                واپس new0(حرف*, 1);

        sv = new(حرف*, n_entries(h)+1);
        اگر (!sv)
                واپس تہی;

        n = 0;
        HASHMAP_FOREACH_IDX(idx, h, i)
                sv[n++] = اندراج_قدر(h, بالٹی_at(h, idx));
        sv[n] = تہی;

        واپس sv;
}

حرف** سیٹ_مقصد_strv(سیٹ_2 **s) {
        أكد(s);

        /* This is similar to set_get_strv(), but invalidates the set on success. */

        حرف **v = new(حرف*, سیٹ_سائز(*s) + 1);
        اگر (!v)
                واپس تہی;

        برائے (حرف **p = v; (*p = سیٹ_steal_پہلا(*s)); p++)
                ;

        أكد(سیٹ_isempty(*s));
        *s = سیٹ_free(*s);
        واپس v;
}

خالی* ordered_hashmap_اگلا(OrderedHashmap *h, ثابت خالی *کلید) {
        ساخت ordered_hashmap_اندراج *e;
        غیرعلامتی hash, idx;

        اگر (!h)
                واپس تہی;

        hash = بالٹی_hash(h, کلید);
        idx = بالٹی_جائزہ(h, hash, کلید);
        اگر (idx == IDX_NIL)
                واپس تہی;

        e = ordered_بالٹی_at(h, idx);
        اگر (e->iterate_اگلا == IDX_NIL)
                واپس تہی;
        واپس ordered_بالٹی_at(h, e->iterate_اگلا)->p.قدر;
}

صحیح سیٹ_consume(سیٹ_2 *s, خالی *قدر) {
        صحیح r;

        أكد(s);
        أكد(قدر);

        r = سیٹ_put(s, قدر);
        اگر (r <= 0)
                آزاد(قدر);

        واپس r;
}

صحیح hashmap_put_strdup_بھرا(Hashmap **h, ثابت ساخت hash_ops *hash_ops, ثابت حرف *k, ثابت حرف *v) {
        صحیح r;

        أكد(h);

        r = hashmap_ensure_allocated(h, hash_ops);
        اگر (r < 0)
                واپس r;

        _cleanup_free_ حرف *kdup = تہی, *vdup = تہی;

        kdup = strdup(k);
        اگر (!kdup)
                واپس -ENOMEM;

        اگر (v) {
                vdup = strdup(v);
                اگر (!vdup)
                        واپس -ENOMEM;
        }

        r = hashmap_put(*h, kdup, vdup);
        اگر (r < 0) {
                اگر (r == -EEXIST && streq_ptr(v, hashmap_get(*h, kdup)))
                        واپس 0;
                واپس r;
        }

        /* 0 with non-null vdup would mean vdup is already in the hashmap, which cannot be */
        أكد(vdup == تہی || r > 0);
        اگر (r > 0)
                kdup = vdup = تہی;

        واپس r;
}

صحیح سیٹ_put_strndup_بھرا(سیٹ_2 **s, ثابت ساخت hash_ops *hash_ops, ثابت حرف *p, size_t n) {
        حرف *c;
        صحیح r;

        أكد(s);
        أكد(p);

        r = سیٹ_ensure_allocated(s, hash_ops);
        اگر (r < 0)
                واپس r;

        اگر (n == SIZE_MAX) {
                اگر (سیٹ_شامل_ہے(*s, (حرف*) p))
                        واپس 0;

                c = strdup(p);
        } ورنہ
                c = strndup(p, n);
        اگر (!c)
                واپس -ENOMEM;

        واپس سیٹ_consume(*s, c);
}

صحیح سیٹ_put_strdupv_بھرا(سیٹ_2 **s, ثابت ساخت hash_ops *hash_ops, حرف **l) {
        صحیح n = 0, r;

        أكد(s);

        STRV_FOREACH(i, l) {
                r = سیٹ_put_strndup_بھرا(s, hash_ops, *i, SIZE_MAX);
                اگر (r < 0)
                        واپس r;

                n += r;
        }

        واپس n;
}

صحیح سیٹ_put_strsplit(سیٹ_2 *s, ثابت حرف *v, ثابت حرف *separators, نکالو_Flags flags) {
        ثابت حرف *p = ASSERT_PTR(v);
        صحیح r;

        أكد(s);

        برائے (;;) {
                حرف *لفظ;

                r = نکالو_پہلا_لفظ(&p, &لفظ, separators, flags);
                اگر (r <= 0)
                        واپس r;

                r = سیٹ_consume(s, لفظ);
                اگر (r < 0)
                        واپس r;
        }
}

/* expand the cachemem if needed, return true if newly (re)activated. */
ساکن صحیح cachemem_maintain(CacheMem *mem, size_t سائز) {
        أكد(mem);

        اگر (!GREEDY_REALLOC(mem->ptr, سائز)) {
                اگر (سائز > 0)
                        واپس -ENOMEM;
        }

        اگر (!mem->فعال) {
                mem->فعال = true;
                واپس true;
        }

        واپس false;
}

صحیح iterated_cache_get(IteratedCache *cache, ثابت خالی ***res_کلیدیں, ثابت خالی ***res_اقدار, غیرعلامتی *res_n_entries) {
        bool sync_کلیدیں = false, sync_اقدار = false;
        size_t سائز;
        صحیح r;

        أكد(cache);
        أكد(cache->hashmap);

        سائز = n_entries(cache->hashmap);

        اگر (res_کلیدیں) {
                r = cachemem_maintain(&cache->کلیدیں, سائز);
                اگر (r < 0)
                        واپس r;

                sync_کلیدیں = r;
        } ورنہ
                cache->کلیدیں.فعال = false;

        اگر (res_اقدار) {
                r = cachemem_maintain(&cache->values, سائز);
                اگر (r < 0)
                        واپس r;

                sync_اقدار = r;
        } ورنہ
                cache->values.فعال = false;

        اگر (cache->hashmap->dirty) {
                اگر (cache->کلیدیں.فعال)
                        sync_کلیدیں = true;
                اگر (cache->values.فعال)
                        sync_اقدار = true;

                cache->hashmap->dirty = false;
        }

        اگر (sync_کلیدیں || sync_اقدار) {
                غیرعلامتی i, idx;
                Iterator iter;

                i = 0;
                HASHMAP_FOREACH_IDX(idx, cache->hashmap, iter) {
                        ساخت hashmap_بنیاد_اندراج *e;

                        e = بالٹی_at(cache->hashmap, idx);

                        اگر (sync_کلیدیں)
                                cache->کلیدیں.ptr[i] = e->کلید;
                        اگر (sync_اقدار)
                                cache->values.ptr[i] = اندراج_قدر(cache->hashmap, e);
                        i++;
                }
        }

        اگر (res_کلیدیں)
                *res_کلیدیں = cache->کلیدیں.ptr;
        اگر (res_اقدار)
                *res_اقدار = cache->values.ptr;
        اگر (res_n_entries)
                *res_n_entries = سائز;

        واپس 0;
}

IteratedCache* iterated_cache_free(IteratedCache *cache) {
        اگر (cache) {
                آزاد(cache->کلیدیں.ptr);
                آزاد(cache->values.ptr);
        }

        واپس mfree(cache);
}

صحیح سیٹ_strjoin(سیٹ_2 *s, ثابت حرف *separator, bool wrap_with_separator, حرف **ret) {
        _cleanup_free_ حرف *str = تہی;
        size_t separator_len, len = 0;
        ثابت حرف *قدر;
        bool پہلا;

        أكد(ret);

        اگر (سیٹ_isempty(s)) {
                *ret = تہی;
                واپس 0;
        }

        separator_len = strlen_ptr(separator);

        اگر (separator_len == 0)
                wrap_with_separator = false;

        پہلا = !wrap_with_separator;

        سیٹ_FOREACH(قدر, s) {
                size_t l = strlen_ptr(قدر);

                اگر (l == 0)
                        جاری;

                اگر (!GREEDY_REALLOC(str, len + l + (پہلا ? 0 : separator_len) + (wrap_with_separator ? separator_len : 0) + 1))
                        واپس -ENOMEM;

                اگر (separator_len > 0 && !پہلا) {
                        حافظہ_نقل(str + len, separator, separator_len);
                        len += separator_len;
                }

                حافظہ_نقل(str + len, قدر, l);
                len += l;
                پہلا = false;
        }

        اگر (wrap_with_separator) {
                حافظہ_نقل(str + len, separator, separator_len);
                len += separator_len;
        }

        str[len] = '\0';

        *ret = TAKE_PTR(str);
        واپس 0;
}

bool سیٹ_equal(سیٹ_2 *a, سیٹ_2 *b) {
        خالی *p;

        /* Checks whether each entry of 'a' is also in 'b' and vice versa, i.e. the two sets contain the same
         * entries */

        اگر (a == b)
                واپس true;

        اگر (سیٹ_isempty(a) && سیٹ_isempty(b))
                واپس true;

        اگر (سیٹ_سائز(a) != سیٹ_سائز(b)) /* Cheap check that hopefully catches a lot of inequality cases
                                         * already */
                واپس false;

        سیٹ_FOREACH(p, a)
                اگر (!سیٹ_شامل_ہے(b, p))
                        واپس false;

        /* If we have the same hashops, then we don't need to check things backwards given we compared the
         * size and that all of a is in b. */
        اگر (a->b.hash_ops == b->b.hash_ops)
                واپس true;

        سیٹ_FOREACH(p, b)
                اگر (!سیٹ_شامل_ہے(a, p))
                        واپس false;

        واپس true;
}

ساکن bool سیٹ_fnmatch_one(سیٹ_2 *patterns, ثابت حرف *needle) {
        ثابت حرف *p;

        أكد(needle);

        /* Any failure of fnmatch() is treated as equivalent to FNM_NOMATCH, i.e. as non-matching pattern */

        سیٹ_FOREACH(p, patterns)
                اگر (fnmatch(p, needle, 0) == 0)
                        واپس true;

        واپس false;
}

bool سیٹ_fnmatch(سیٹ_2 *include_patterns, سیٹ_2 *exclude_patterns, ثابت حرف *needle) {
        أكد(needle);

        اگر (سیٹ_fnmatch_one(exclude_patterns, needle))
                واپس false;

        اگر (سیٹ_isempty(include_patterns))
                واپس true;

        واپس سیٹ_fnmatch_one(include_patterns, needle);
}

ساکن صحیح hashmap_اندراج_compare(
                ساخت hashmap_بنیاد_اندراج * ثابت *a,
                ساخت hashmap_بنیاد_اندراج * ثابت *b,
                compare_func_t compare) {

        أكد(a && *a);
        أكد(b && *b);
        أكد(compare);

        واپس compare((*a)->کلید, (*b)->کلید);
}

ساکن صحیح _hashmap_dump_entries_sorted(
                Hashmap_بنیاد *h,
                خالی ***ret,
                size_t *ret_n) {
        _cleanup_free_ خالی **entries = تہی;
        Iterator iter;
        غیرعلامتی idx;
        size_t n = 0;

        أكد(ret);
        أكد(ret_n);

        اگر (hashmap_سائز(h) == 0) {
                *ret = تہی;
                *ret_n = 0;
                واپس 0;
        }

        /* We append one more element than needed so that the resulting array can be used as a strv. We
         * don't count this entry in the returned size. */
        entries = new(خالی*, hashmap_سائز(h) + 1);
        اگر (!entries)
                واپس -ENOMEM;

        HASHMAP_FOREACH_IDX(idx, h, iter)
                entries[n++] = بالٹی_at(h, idx);

        أكد(n == hashmap_سائز(h));
        entries[n] = تہی;

        typesafe_qsort_r((ساخت hashmap_بنیاد_اندراج**) entries, n,
                         hashmap_اندراج_compare, h->hash_ops->compare);

        *ret = TAKE_PTR(entries);
        *ret_n = n;
        واپس 0;
}

صحیح hashmap_dump_کلیدیں_sorted(Hashmap_بنیاد *h, خالی ***ret, size_t *ret_n) {
        _cleanup_free_ خالی **entries = تہی;
        size_t n;
        صحیح r;

        أكد(ret);

        r = _hashmap_dump_entries_sorted(h, &entries, &n);
        اگر (r < 0)
                واپس r;

        /* Reuse the array. */
        FOREACH_ARRAY(e, entries, n)
                *e = (خالی*) (*(ساخت hashmap_بنیاد_اندراج**) e)->کلید;

        *ret = TAKE_PTR(entries);
        اگر (ret_n)
                *ret_n = n;
        واپس 0;
}

صحیح _hashmap_dump_sorted(Hashmap_بنیاد *h, خالی ***ret, size_t *ret_n) {
        _cleanup_free_ خالی **entries = تہی;
        size_t n;
        صحیح r;

        أكد(ret);

        r = _hashmap_dump_entries_sorted(h, &entries, &n);
        اگر (r < 0)
                واپس r;

        /* Reuse the array. */
        FOREACH_ARRAY(e, entries, n)
                *e = اندراج_قدر(h, *(ساخت hashmap_بنیاد_اندراج**) e);

        *ret = TAKE_PTR(entries);
        اگر (ret_n)
                *ret_n = n;
        واپس 0;
}
