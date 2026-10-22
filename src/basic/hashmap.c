/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <fnmatch.h>
#ضمن <pthread.h>
#ضمن <unistd.h>
#إذا HAVE_VALGRIND_VALGRIND_H
#  include <valgrind/valgrind.h>
#جيڪڏهن_پورو

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
#جيڪڏهن_پورو

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
#وصف_ڪريو INV_KEEP_FREE            5U

/* Fields common to entries of all hashmap/set types */
بنية hashmap_بنياد_داخلا {
        ثابت فراغ *ڪنجي;
};

/* Entry types for specific hashmap/set types
 * hashmap_base_entry must be at the beginning of each entry struct. */

بنية plain_hashmap_داخلا {
        بنية hashmap_بنياد_داخلا b;
        فراغ *قدر;
};

بنية ordered_hashmap_داخلا {
        بنية plain_hashmap_داخلا p;
        بنانشان iterate_ايندڙ, iterate_اڳيون;
};

بنية سيٽ_داخلا {
        بنية hashmap_بنياد_داخلا b;
};

/* In several functions it is advantageous to have the hash table extended
 * virtually by a couple of additional buckets. We reserve special index values
 * for these "swap" buckets. */
#وصف_ڪريو IDX_مٽا_سٽا_BEGIN     (UINT_MAX - 3)
#وصف_ڪريو IDX_PUT             (IDX_مٽا_سٽا_BEGIN + 0)
#وصف_ڪريو IDX_TMP             (IDX_مٽا_سٽا_BEGIN + 1)
#وصف_ڪريو IDX_مٽا_سٽا_پڇاڙي       (IDX_مٽا_سٽا_BEGIN + 2)

#وصف_ڪريو IDX_پهريون           (UINT_MAX - 1) /* special index for freshly initialized iterators */
#وصف_ڪريو IDX_NIL             UINT_MAX       /* special index value meaning "none" or "end" */

assert_cc(IDX_پهريون == IDX_مٽا_سٽا_پڇاڙي);
assert_cc(IDX_پهريون == IDX_ITERATOR_پهريون);

/* Storage space for the "swap" buckets.
 * All entry types can fit into an ordered_hashmap_entry. */
بنية مٽا_سٽا_entries {
        بنية ordered_hashmap_داخلا e[IDX_مٽا_سٽا_پڇاڙي - IDX_مٽا_سٽا_BEGIN];
};

/* Distance from Initial Bucket */
تعريفنوع uint8_t dib_raw_t;
#وصف_ڪريو DIB_RAW_حد_کان_مٿي ((dib_raw_t)0xfdU)   /* indicates DIB value is greater than representable */
#وصف_ڪريو DIB_RAW_REHASH   ((dib_raw_t)0xfeU)   /* entry yet to be rehashed during in-place resize */
#وصف_ڪريو DIB_RAW_FREE     ((dib_raw_t)0xffU)   /* a free bucket */
#وصف_ڪريو DIB_RAW_INIT     ((حرف)DIB_RAW_FREE) /* a byte to memset a DIB store with when initializing */

#وصف_ڪريو DIB_FREE UINT_MAX

#إذا ENABLE_DEBUG_HASHMAP
بنية hashmap_debug_info {
        LIST_FIELDS(بنية hashmap_debug_info, debug_list);
        بنانشان max_entries;  /* high watermark of n_entries */

        /* fields to detect modification while iterating */
        بنانشان put_ڳڻپ;    /* counts puts into the hashmap */
        بنانشان rem_ڳڻپ;    /* counts removals from hashmap */
        بنانشان آخري_rem_idx; /* remembers last removal index */
};

/* Tracks all existing hashmaps. Get at it from gdb. See sd_dump_hashmaps.py */
ساكن LIST_سر(بنية hashmap_debug_info, hashmap_debug_list);
ساكن pthread_mutex_t hashmap_debug_list_mutex = PTHREAD_MUTEX_INITIALIZER;
#جيڪڏهن_پورو

تعداد HashmapType {
        HASHMAP_TYPE_PLAIN,
        HASHMAP_TYPE_ORDERED,
        HASHMAP_TYPE_سيٽ,
        _HASHMAP_TYPE_MAX
};

بنية _packed_ اڻسڌو_storage {
        فراغ *storage;                     /* where buckets and DIBs are stored */
        uint8_t  hash_ڪنجي[HASH_ڪنجي_سائيز];  /* hash key; changes during resize */

        بنانشان n_entries;                /* number of stored entries */
        بنانشان n_buckets;                /* number of buckets */

        بنانشان idx_سڀ_کان_هيٺ_داخلا;         /* Index below which all buckets are free.
                                              Makes "while (hashmap_steal_first())" loops
                                              O(n) instead of O(n^2) for unordered hashmaps. */
        uint8_t  _pad[3];                  /* padding for the whole HashmapBase */
        /* The bitfields in HashmapBase complete the alignment of the whole thing. */
};

بنية سڌو_storage {
        /* This gives us 39 bytes on 64-bit, or 35 bytes on 32-bit.
         * That's room for 4 set_entries + 4 DIB bytes + 3 unused bytes on 64-bit,
         *              or 7 set_entries + 7 DIB bytes + 0 unused bytes on 32-bit. */
        uint8_t storage[حجم(بنية اڻسڌو_storage)];
};

#وصف_ڪريو سڌو_BUCKETS(داخلا_t) \
        (حجم(بنية سڌو_storage) / (حجم(داخلا_t) + حجم(dib_raw_t)))

/* We should be able to store at least one entry directly. */
assert_cc(سڌو_BUCKETS(بنية ordered_hashmap_داخلا) >= 1);

/* We have 3 bits for n_direct_entries. */
assert_cc(سڌو_BUCKETS(بنية سيٽ_داخلا) < (1 << 3));

/* Hashmaps with directly stored entries all use this shared hash key.
 * It's no big deal if the key is guessed, because there can be only
 * a handful of directly stored entries in a hashmap. When a hashmap
 * outgrows direct storage, it gets its own key for indirect storage. */
ساكن uint8_t shared_hash_ڪنجي[HASH_ڪنجي_سائيز];

/* Fields that all hashmap/set types must have */
بنية Hashmap_بنياد {
        ثابت بنية hash_ops *hash_ops;  /* hash and compare ops to use */

        اتحاد _packed_ {
                بنية اڻسڌو_storage اڻسڌو; /* if  has_indirect */
                بنية سڌو_storage سڌو;     /* if !has_indirect */
        };

        تعداد HashmapType type:2;     /* HASHMAP_TYPE_* */
        bool has_اڻسڌو:1;         /* whether indirect storage is used */
        بنانشان n_سڌو_entries:3; /* Number of entries in direct storage.
                                      * Only valid if !has_indirect. */
        bool from_pool:1;            /* whether was allocated from mempool */
        bool dirty:1;                /* whether dirtied since last iterated_cache_get() */
        bool cached:1;               /* whether this hashmap is being cached */

#إذا ENABLE_DEBUG_HASHMAP
        بنية hashmap_debug_info debug;
#جيڪڏهن_پورو
};

/* Specific hash types
 * HashmapBase must be at the beginning of each hashmap struct. */

بنية Hashmap {
        بنية Hashmap_بنياد b;
};

بنية OrderedHashmap {
        بنية Hashmap_بنياد b;
        بنانشان iterate_list_سر, iterate_list_پڇ;
};

بنية سيٽ_2 {
        بنية Hashmap_بنياد b;
};

تعريفنوع بنية CacheMem {
        ثابت فراغ **ptr;
        size_t n_populated;
        bool سرگرم:1;
} CacheMem;

بنية IteratedCache {
        Hashmap_بنياد *hashmap;
        CacheMem ڪنجيون, values;
};

DEFINE_MEMPOOL(hashmap_pool,         Hashmap,        8);
DEFINE_MEMPOOL(ordered_hashmap_pool, OrderedHashmap, 8);
/* No need for a separate Set pool */
assert_cc(حجم(Hashmap) == حجم(سيٽ_2));

بنية hashmap_type_info {
        size_t سر_سائيز;
        size_t داخلا_سائيز;
        بنية mempool *mempool;
        بنانشان n_سڌو_buckets;
};

ساكن استعمال_ٿيل_2 ثابت بنية hashmap_type_info hashmap_type_info[_HASHMAP_TYPE_MAX] = {
        [HASHMAP_TYPE_PLAIN] = {
                .سر_سائيز        = حجم(Hashmap),
                .داخلا_سائيز       = حجم(بنية plain_hashmap_داخلا),
                .mempool          = &hashmap_pool,
                .n_سڌو_buckets = سڌو_BUCKETS(بنية plain_hashmap_داخلا),
        },
        [HASHMAP_TYPE_ORDERED] = {
                .سر_سائيز        = حجم(OrderedHashmap),
                .داخلا_سائيز       = حجم(بنية ordered_hashmap_داخلا),
                .mempool          = &ordered_hashmap_pool,
                .n_سڌو_buckets = سڌو_BUCKETS(بنية ordered_hashmap_داخلا),
        },
        [HASHMAP_TYPE_سيٽ] = {
                .سر_سائيز        = حجم(سيٽ_2),
                .داخلا_سائيز       = حجم(بنية سيٽ_داخلا),
                .mempool          = &hashmap_pool,
                .n_سڌو_buckets = سڌو_BUCKETS(بنية سيٽ_داخلا),
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

        r = get_process_ڌاڳا(0);
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
#جيڪڏهن_پورو

ساكن مضمن بنانشان n_buckets(Hashmap_بنياد *h) {
        أكد(h);
        أرجع h->has_اڻسڌو ? h->اڻسڌو.n_buckets
                               : hashmap_type_info[h->type].n_سڌو_buckets;
}

ساكن بنانشان n_entries(Hashmap_بنياد *h) {
        أكد(h);
        أرجع h->has_اڻسڌو ? h->اڻسڌو.n_entries
                               : h->n_سڌو_entries;
}

ساكن فراغ n_entries_inc(Hashmap_بنياد *h) {
        أكد(h);

        إذا (h->has_اڻسڌو)
                h->اڻسڌو.n_entries++;
        وإلا
                h->n_سڌو_entries++;
}

ساكن فراغ n_entries_dec(Hashmap_بنياد *h) {
        أكد(h);

        إذا (h->has_اڻسڌو)
                h->اڻسڌو.n_entries--;
        وإلا
                h->n_سڌو_entries--;
}

ساكن مضمن فراغ* storage_ptr(Hashmap_بنياد *h) {
        أكد(h);
        أرجع h->has_اڻسڌو ? h->اڻسڌو.storage
                               : h->سڌو.storage;
}

ساكن مضمن uint8_t* hash_ڪنجي(Hashmap_بنياد *h) {
        أكد(h);
        أرجع h->has_اڻسڌو ? h->اڻسڌو.hash_ڪنجي
                               : shared_hash_ڪنجي;
}

ساكن بنانشان بنياد_ٻالٽي_hash(Hashmap_بنياد *h, ثابت فراغ *p) {
        بنية siphash state;
        uint64_t hash;

        أكد(h);

        /* The keys of trivial_hash_ops and uint64_hash_ops fit in 64 bits. siphash24_uint64() hashes such
         * a key without the indirect call through hash_ops and keeps the SipHash state in registers. On
         * 32-bit and big-endian architectures, the result differs from the result of trivial_hash_func(),
         * which hashes the key in native byte order. Only this function maps keys to buckets, so the
         * difference does not matter. */
        إذا (h->hash_ops->hash == trivial_hash_func)
                hash = siphash24_uint64((uintptr_t) p, hash_ڪنجي(h));
        وإلا إذا (h->hash_ops->hash == (hash_func_t) uint64_hash_func)
                hash = siphash24_uint64(*(ثابت uint64_t*) p, hash_ڪنجي(h));
        وإلا {
                siphash24_init(&state, hash_ڪنجي(h));
                h->hash_ops->hash(p, &state);
                hash = siphash24_finalize(&state);
        }

        /* n_buckets is not a power of two, so a modulo needs a division. Multiplying the upper 32 bits of
         * the hash with n_buckets and keeping the upper 32 bits of the product maps the hash to
         * [0, n_buckets) almost as evenly, without a division. */
        أرجع (بنانشان) (((hash >> 32) * n_buckets(h)) >> 32);
}
#وصف_ڪريو ٻالٽي_hash(h, p) بنياد_ٻالٽي_hash(HASHMAP_بنياد(h), p)

ساكن فراغ بنياد_سيٽ_dirty(Hashmap_بنياد *h) {
        أكد(h);

        h->dirty = true;
}
#وصف_ڪريو hashmap_سيٽ_dirty(h) بنياد_سيٽ_dirty(HASHMAP_بنياد(h))

ساكن فراغ get_hash_ڪنجي(uint8_t hash_ڪنجي[HASH_ڪنجي_سائيز], bool reuse_is_ok) {
        ساكن uint8_t current[HASH_ڪنجي_سائيز];
        ساكن bool current_initialized = false;

        /* Returns a hash function key to use. In order to keep things
         * fast we will not generate a new key each time we allocate a
         * new hash table. Instead, we'll just reuse the most recently
         * generated one, except if we never generated one or when we
         * are rehashing an entire hash table because we reached a
         * fill level */

        إذا (!current_initialized || !reuse_is_ok) {
                random_بائيٽون(current, حجم(current));
                current_initialized = true;
        }

        يادگيري_نقل(hash_ڪنجي, current, حجم(current));
}

ساكن مضمن بنية hashmap_بنياد_داخلا* ٻالٽي_at(Hashmap_بنياد *h, بنانشان idx) {
        أكد(h);
        أرجع CAST_ALIGN_PTR(
                        بنية hashmap_بنياد_داخلا,
                        (uint8_t *) storage_ptr(h) + idx * hashmap_type_info[h->type].داخلا_سائيز);
}

ساكن بنية plain_hashmap_داخلا* plain_ٻالٽي_at(Hashmap *h, بنانشان idx) {
        أرجع (بنية plain_hashmap_داخلا*) ٻالٽي_at(HASHMAP_بنياد(h), idx);
}

ساكن بنية ordered_hashmap_داخلا* ordered_ٻالٽي_at(OrderedHashmap *h, بنانشان idx) {
        أرجع (بنية ordered_hashmap_داخلا*) ٻالٽي_at(HASHMAP_بنياد(h), idx);
}

ساكن بنية سيٽ_داخلا *سيٽ_ٻالٽي_at(سيٽ_2 *h, بنانشان idx) {
        أرجع (بنية سيٽ_داخلا*) ٻالٽي_at(HASHMAP_بنياد(h), idx);
}

ساكن بنية ordered_hashmap_داخلا* ٻالٽي_at_مٽا_سٽا(بنية مٽا_سٽا_entries *مٽا_سٽا, بنانشان idx) {
        أكد(مٽا_سٽا);
        أرجع &مٽا_سٽا->e[idx - IDX_مٽا_سٽا_BEGIN];
}

/* Returns a pointer to the bucket at index idx.
 * Understands real indexes and swap indexes, hence "_virtual". */
ساكن بنية hashmap_بنياد_داخلا* ٻالٽي_at_virtual(Hashmap_بنياد *h, بنية مٽا_سٽا_entries *مٽا_سٽا,
                                                    بنانشان idx) {
        إذا (idx < IDX_مٽا_سٽا_BEGIN)
                أرجع ٻالٽي_at(h, idx);

        إذا (idx < IDX_مٽا_سٽا_پڇاڙي)
                أرجع &ٻالٽي_at_مٽا_سٽا(مٽا_سٽا, idx)->p.b;

        assert_not_reached();
}

ساكن مضمن dib_raw_t* dib_raw_ptr(Hashmap_بنياد *h) {
        أكد(h);
        أرجع (dib_raw_t*)
                ((uint8_t*) storage_ptr(h) + hashmap_type_info[h->type].داخلا_سائيز * n_buckets(h));
}

ساكن بنانشان ٻالٽي_مفاصلو(Hashmap_بنياد *h, بنانشان idx, بنانشان from) {
        أرجع idx >= from ? idx - from
                           : n_buckets(h) + idx - from;
}

ساكن بنانشان ٻالٽي_calculate_dib(Hashmap_بنياد *h, بنانشان idx, dib_raw_t raw_dib) {
        بنانشان initial_ٻالٽي;

        إذا (raw_dib == DIB_RAW_FREE)
                أرجع DIB_FREE;

        إذا (_likely_(raw_dib < DIB_RAW_حد_کان_مٿي))
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
        initial_ٻالٽي = ٻالٽي_hash(h, ٻالٽي_at(h, idx)->ڪنجي);
        أرجع ٻالٽي_مفاصلو(h, idx, initial_ٻالٽي);
}

ساكن فراغ ٻالٽي_سيٽ_dib(Hashmap_بنياد *h, بنانشان idx, بنانشان dib) {
        dib_raw_ptr(h)[idx] = dib != DIB_FREE ? MIN(dib, DIB_RAW_حد_کان_مٿي) : DIB_RAW_FREE;
}

ساكن بنانشان skip_free_buckets(Hashmap_بنياد *h, بنانشان idx) {
        dib_raw_t *dibs;

        dibs = dib_raw_ptr(h);

        لكل ( ; idx < n_buckets(h); idx++)
                إذا (dibs[idx] != DIB_RAW_FREE)
                        أرجع idx;

        أرجع IDX_NIL;
}

ساكن فراغ ٻالٽي_mark_free(Hashmap_بنياد *h, بنانشان idx) {
        أكد(h);

        memzero(ٻالٽي_at(h, idx), hashmap_type_info[h->type].داخلا_سائيز);
        ٻالٽي_سيٽ_dib(h, idx, DIB_FREE);
}

ساكن فراغ ٻالٽي_move_داخلا(Hashmap_بنياد *h, بنية مٽا_سٽا_entries *مٽا_سٽا,
                              بنانشان from, بنانشان to) {
        بنية hashmap_بنياد_داخلا *e_from, *e_منزل;

        أكد(h);
        أكد(from != to);

        e_from = ٻالٽي_at_virtual(h, مٽا_سٽا, from);
        e_منزل   = ٻالٽي_at_virtual(h, مٽا_سٽا, to);

        يادگيري_نقل(e_منزل, e_from, hashmap_type_info[h->type].داخلا_سائيز);

        إذا (h->type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*) h;
                بنية ordered_hashmap_داخلا *le, *le_منزل;

                le_منزل = (بنية ordered_hashmap_داخلا*) e_منزل;

                إذا (le_منزل->iterate_ايندڙ != IDX_NIL) {
                        le = (بنية ordered_hashmap_داخلا*)
                             ٻالٽي_at_virtual(h, مٽا_سٽا, le_منزل->iterate_ايندڙ);
                        le->iterate_اڳيون = to;
                }

                إذا (le_منزل->iterate_اڳيون != IDX_NIL) {
                        le = (بنية ordered_hashmap_داخلا*)
                             ٻالٽي_at_virtual(h, مٽا_سٽا, le_منزل->iterate_اڳيون);
                        le->iterate_ايندڙ = to;
                }

                إذا (lh->iterate_list_سر == from)
                        lh->iterate_list_سر = to;
                إذا (lh->iterate_list_پڇ == from)
                        lh->iterate_list_پڇ = to;
        }
}

ساكن مضمن بنانشان ايندڙ_idx(Hashmap_بنياد *h, بنانشان idx) {
        أرجع idx + 1U < n_buckets(h) ? idx + 1U : 0U;
}

ساكن بنانشان prev_idx(Hashmap_بنياد *h, بنانشان idx) {
        أرجع idx > 0U ? idx - 1U : n_buckets(h) - 1U;
}

ساكن مضمن فراغ* داخلا_قدر(Hashmap_بنياد *h, بنية hashmap_بنياد_داخلا *e) {
        أكد(h);
        أكد(e);

        بدل (h->type) {

        حالة HASHMAP_TYPE_PLAIN:
        حالة HASHMAP_TYPE_ORDERED:
                أرجع ((بنية plain_hashmap_داخلا*)e)->قدر;

        حالة HASHMAP_TYPE_سيٽ:
                أرجع (فراغ*) e->ڪنجي;

        افتراضي:
                assert_not_reached();
        }
}

ساكن فراغ بنياد_remove_داخلا(Hashmap_بنياد *h, بنانشان idx) {
        بنانشان کاٻو, ساڄو, prev, dib;
        dib_raw_t raw_dib, *dibs;

        dibs = dib_raw_ptr(h);
        أكد(dibs[idx] != DIB_RAW_FREE);

#إذا ENABLE_DEBUG_HASHMAP
        أكد(h);
        h->debug.rem_ڳڻپ++;
        h->debug.آخري_rem_idx = idx;
#جيڪڏهن_پورو

        کاٻو = idx;
        /* Find the stop bucket ("right"). It is either free or has DIB == 0. */
        لكل (ساڄو = ايندڙ_idx(h, کاٻو); ; ساڄو = ايندڙ_idx(h, ساڄو)) {
                raw_dib = dibs[ساڄو];
                إذا (اندريون_سيٽ(raw_dib, 0, DIB_RAW_FREE))
                        اكسر;

                /* The buckets are not supposed to be all occupied and with DIB > 0.
                 * That would mean we could make everyone better off by shifting them
                 * backward. This scenario is impossible. */
                أكد(کاٻو != ساڄو);
        }

        إذا (h->type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*) h;
                بنية ordered_hashmap_داخلا *le = ordered_ٻالٽي_at(lh, idx);

                إذا (le->iterate_ايندڙ != IDX_NIL)
                        ordered_ٻالٽي_at(lh, le->iterate_ايندڙ)->iterate_اڳيون = le->iterate_اڳيون;
                وإلا
                        lh->iterate_list_پڇ = le->iterate_اڳيون;

                إذا (le->iterate_اڳيون != IDX_NIL)
                        ordered_ٻالٽي_at(lh, le->iterate_اڳيون)->iterate_ايندڙ = le->iterate_ايندڙ;
                وإلا
                        lh->iterate_list_سر = le->iterate_ايندڙ;
        }

        /* Now shift all buckets in the interval (left, right) one step backwards */
        لكل (prev = کاٻو, کاٻو = ايندڙ_idx(h, کاٻو); کاٻو != ساڄو;
             prev = کاٻو, کاٻو = ايندڙ_idx(h, کاٻو)) {
                dib = ٻالٽي_calculate_dib(h, کاٻو, dibs[کاٻو]);
                أكد(dib != 0);
                ٻالٽي_move_داخلا(h, عدم, کاٻو, prev);
                ٻالٽي_سيٽ_dib(h, prev, dib - 1);
        }

        ٻالٽي_mark_free(h, prev);
        n_entries_dec(h);
        بنياد_سيٽ_dirty(h);
}
#وصف_ڪريو remove_داخلا(h, idx) بنياد_remove_داخلا(HASHMAP_بنياد(h), idx)

ساكن بنانشان hashmap_iterate_اندريون_insertion_ترتيب(OrderedHashmap *h, Iterator *i) {
        بنية ordered_hashmap_داخلا *e;
        بنانشان idx;

        أكد(h);
        أكد(i);

        إذا (i->idx == IDX_NIL)
                اذهب at_پڇاڙي;

        إذا (i->idx == IDX_پهريون && h->iterate_list_سر == IDX_NIL)
                اذهب at_پڇاڙي;

        إذا (i->idx == IDX_پهريون) {
                idx = h->iterate_list_سر;
                e = ordered_ٻالٽي_at(h, idx);
        } وإلا {
                idx = i->idx;
                e = ordered_ٻالٽي_at(h, idx);
                /*
                 * We allow removing the current entry while iterating, but removal may cause
                 * a backward shift. The next entry may thus move one bucket to the left.
                 * To detect when it happens, we remember the key pointer of the entry we were
                 * going to iterate next. If it does not match, there was a backward shift.
                 */
                إذا (e->p.b.ڪنجي != i->ايندڙ_ڪنجي) {
                        idx = prev_idx(HASHMAP_بنياد(h), idx);
                        e = ordered_ٻالٽي_at(h, idx);
                }
                أكد(e->p.b.ڪنجي == i->ايندڙ_ڪنجي);
        }

#إذا ENABLE_DEBUG_HASHMAP
        i->prev_idx = idx;
#جيڪڏهن_پورو

        إذا (e->iterate_ايندڙ != IDX_NIL) {
                بنية ordered_hashmap_داخلا *n;
                i->idx = e->iterate_ايندڙ;
                n = ordered_ٻالٽي_at(h, i->idx);
                i->ايندڙ_ڪنجي = n->p.b.ڪنجي;
        } وإلا
                i->idx = IDX_NIL;

        أرجع idx;

at_پڇاڙي:
        i->idx = IDX_NIL;
        أرجع IDX_NIL;
}

ساكن بنانشان hashmap_iterate_اندريون_اندروني_ترتيب(Hashmap_بنياد *h, Iterator *i) {
        بنانشان idx;

        أكد(h);
        أكد(i);

        إذا (i->idx == IDX_NIL)
                اذهب at_پڇاڙي;

        إذا (i->idx == IDX_پهريون) {
                /* fast forward to the first occupied bucket */
                إذا (h->has_اڻسڌو) {
                        i->idx = skip_free_buckets(h, h->اڻسڌو.idx_سڀ_کان_هيٺ_داخلا);
                        h->اڻسڌو.idx_سڀ_کان_هيٺ_داخلا = i->idx;
                } وإلا
                        i->idx = skip_free_buckets(h, 0);

                إذا (i->idx == IDX_NIL)
                        اذهب at_پڇاڙي;
        } وإلا {
                بنية hashmap_بنياد_داخلا *e;

                أكد(i->idx > 0);

                e = ٻالٽي_at(h, i->idx);
                /*
                 * We allow removing the current entry while iterating, but removal may cause
                 * a backward shift. The next entry may thus move one bucket to the left.
                 * To detect when it happens, we remember the key pointer of the entry we were
                 * going to iterate next. If it does not match, there was a backward shift.
                 */
                إذا (e->ڪنجي != i->ايندڙ_ڪنجي)
                        e = ٻالٽي_at(h, --i->idx);

                أكد(e->ڪنجي == i->ايندڙ_ڪنجي);
        }

        idx = i->idx;
#إذا ENABLE_DEBUG_HASHMAP
        i->prev_idx = idx;
#جيڪڏهن_پورو

        i->idx = skip_free_buckets(h, i->idx + 1);
        إذا (i->idx != IDX_NIL)
                i->ايندڙ_ڪنجي = ٻالٽي_at(h, i->idx)->ڪنجي;
        وإلا
                i->idx = IDX_NIL;

        أرجع idx;

at_پڇاڙي:
        i->idx = IDX_NIL;
        أرجع IDX_NIL;
}

ساكن بنانشان hashmap_iterate_داخلا(Hashmap_بنياد *h, Iterator *i) {
        إذا (!h) {
                i->idx = IDX_NIL;
                أرجع IDX_NIL;
        }

#إذا ENABLE_DEBUG_HASHMAP
        إذا (i->idx == IDX_پهريون) {
                i->put_ڳڻپ = h->debug.put_ڳڻپ;
                i->rem_ڳڻپ = h->debug.rem_ڳڻپ;
        } وإلا {
                /* While iterating, must not add any new entries */
                أكد(i->put_ڳڻپ == h->debug.put_ڳڻپ);
                /* ... or remove entries other than the current one */
                أكد(i->rem_ڳڻپ == h->debug.rem_ڳڻپ ||
                       (i->rem_ڳڻپ == h->debug.rem_ڳڻپ - 1 &&
                        i->prev_idx == h->debug.آخري_rem_idx));
                /* Reset our removals counter */
                i->rem_ڳڻپ = h->debug.rem_ڳڻپ;
        }
#جيڪڏهن_پورو

        أرجع h->type == HASHMAP_TYPE_ORDERED ? hashmap_iterate_اندريون_insertion_ترتيب((OrderedHashmap*) h, i)
                                               : hashmap_iterate_اندريون_اندروني_ترتيب(h, i);
}

bool _hashmap_iterate(Hashmap_بنياد *h, Iterator *i, فراغ **قدر, ثابت فراغ **ڪنجي) {
        بنية hashmap_بنياد_داخلا *e;
        فراغ *معلومات;
        بنانشان idx;

        idx = hashmap_iterate_داخلا(h, i);
        إذا (idx == IDX_NIL) {
                إذا (قدر)
                        *قدر = عدم;
                إذا (ڪنجي)
                        *ڪنجي = عدم;

                أرجع false;
        }

        e = ٻالٽي_at(h, idx);
        معلومات = داخلا_قدر(h, e);
        إذا (قدر)
                *قدر = معلومات;
        إذا (ڪنجي)
                *ڪنجي = e->ڪنجي;

        أرجع true;
}

#وصف_ڪريو HASHMAP_FOREACH_IDX(idx, h, i) \
        لكل ((i) = ITERATOR_پهريون, (idx) = hashmap_iterate_داخلا((h), &(i)); \
             (idx != IDX_NIL); \
             (idx) = hashmap_iterate_داخلا((h), &(i)))

IteratedCache* _hashmap_iterated_cache_new(Hashmap_بنياد *h) {
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

ساكن فراغ reset_سڌو_storage(Hashmap_بنياد *h) {
        ثابت بنية hashmap_type_info *hi = &hashmap_type_info[h->type];
        فراغ *p;

        أكد(!h->has_اڻسڌو);

        p = mempset(h->سڌو.storage, 0, hi->داخلا_سائيز * hi->n_سڌو_buckets);
        memset(p, DIB_RAW_INIT, حجم(dib_raw_t) * hi->n_سڌو_buckets);
}

ساكن فراغ shared_hash_ڪنجي_initialize(فراغ) {
        random_بائيٽون(shared_hash_ڪنجي, حجم(shared_hash_ڪنجي));
}

ساكن بنية Hashmap_بنياد* hashmap_بنياد_new(ثابت بنية hash_ops *hash_ops, تعداد HashmapType type) {
        Hashmap_بنياد *h;
        ثابت بنية hashmap_type_info *hi = &hashmap_type_info[type];

        bool use_pool = mempool_enabled && mempool_enabled();  /* mempool_enabled is a weak symbol */

        h = use_pool ? mempool_alloc0_tile(hi->mempool) : malloc0(hi->سر_سائيز);
        إذا (!h)
                أرجع عدم;

        h->type = type;
        h->from_pool = use_pool;
        h->hash_ops = hash_ops ?: &trivial_hash_ops;

        إذا (type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*)h;
                lh->iterate_list_سر = lh->iterate_list_پڇ = IDX_NIL;
        }

        reset_سڌو_storage(h);

        ساكن pthread_once_t once = PTHREAD_ONCE_INIT;
        assert_se(pthread_once(&once, shared_hash_ڪنجي_initialize) == 0);

#إذا ENABLE_DEBUG_HASHMAP
        assert_se(pthread_mutex_lock(&hashmap_debug_list_mutex) == 0);
        LIST_PREPEND(debug_list, hashmap_debug_list, &h->debug);
        assert_se(pthread_mutex_unlock(&hashmap_debug_list_mutex) == 0);
#جيڪڏهن_پورو

        أرجع h;
}

Hashmap *hashmap_new(ثابت بنية hash_ops *hash_ops) {
        أرجع (Hashmap*)        hashmap_بنياد_new(hash_ops, HASHMAP_TYPE_PLAIN);
}

OrderedHashmap *ordered_hashmap_new(ثابت بنية hash_ops *hash_ops) {
        أرجع (OrderedHashmap*) hashmap_بنياد_new(hash_ops, HASHMAP_TYPE_ORDERED);
}

سيٽ_2 *سيٽ_new(ثابت بنية hash_ops *hash_ops) {
        أرجع (سيٽ_2*)            hashmap_بنياد_new(hash_ops, HASHMAP_TYPE_سيٽ);
}

ساكن صحيح hashmap_بنياد_ensure_allocated(Hashmap_بنياد **h, ثابت بنية hash_ops *hash_ops,
                                         تعداد HashmapType type) {
        Hashmap_بنياد *q;

        أكد(h);

        إذا (*h) {
                أكد((*h)->hash_ops == (hash_ops ?: &trivial_hash_ops));
                أرجع 0;
        }

        q = hashmap_بنياد_new(hash_ops, type);
        إذا (!q)
                أرجع -ENOMEM;

        *h = q;
        أرجع 1;
}

صحيح hashmap_ensure_allocated(Hashmap **h, ثابت بنية hash_ops *hash_ops) {
        أرجع hashmap_بنياد_ensure_allocated((Hashmap_بنياد**)h, hash_ops, HASHMAP_TYPE_PLAIN);
}

صحيح ordered_hashmap_ensure_allocated(OrderedHashmap **h, ثابت بنية hash_ops *hash_ops) {
        أرجع hashmap_بنياد_ensure_allocated((Hashmap_بنياد**)h, hash_ops, HASHMAP_TYPE_ORDERED);
}

صحيح سيٽ_ensure_allocated(سيٽ_2 **s, ثابت بنية hash_ops *hash_ops) {
        أرجع hashmap_بنياد_ensure_allocated((Hashmap_بنياد**)s, hash_ops, HASHMAP_TYPE_سيٽ);
}

صحيح hashmap_ensure_put(Hashmap **h, ثابت بنية hash_ops *hash_ops, ثابت فراغ *ڪنجي, فراغ *قدر) {
        صحيح r;

        أكد(h);

        r = hashmap_ensure_allocated(h, hash_ops);
        إذا (r < 0)
                أرجع r;

        أرجع hashmap_put(*h, ڪنجي, قدر);
}

صحيح ordered_hashmap_ensure_put(OrderedHashmap **h, ثابت بنية hash_ops *hash_ops, ثابت فراغ *ڪنجي, فراغ *قدر) {
        صحيح r;

        أكد(h);

        r = ordered_hashmap_ensure_allocated(h, hash_ops);
        إذا (r < 0)
                أرجع r;

        أرجع ordered_hashmap_put(*h, ڪنجي, قدر);
}

صحيح ordered_hashmap_ensure_replace(OrderedHashmap **h, ثابت بنية hash_ops *hash_ops, ثابت فراغ *ڪنجي, فراغ *قدر) {
        صحيح r;

        أكد(h);

        r = ordered_hashmap_ensure_allocated(h, hash_ops);
        إذا (r < 0)
                أرجع r;

        أرجع ordered_hashmap_replace(*h, ڪنجي, قدر);
}

صحيح hashmap_ensure_replace(Hashmap **h, ثابت بنية hash_ops *hash_ops, ثابت فراغ *ڪنجي, فراغ *قدر) {
        صحيح r;

        أكد(h);

        r = hashmap_ensure_allocated(h, hash_ops);
        إذا (r < 0)
                أرجع r;

        أرجع hashmap_replace(*h, ڪنجي, قدر);
}

ساكن فراغ hashmap_free_no_clear(Hashmap_بنياد *h) {
        أكد(!h->has_اڻسڌو);
        أكد(h->n_سڌو_entries == 0);

#إذا ENABLE_DEBUG_HASHMAP
        assert_se(pthread_mutex_lock(&hashmap_debug_list_mutex) == 0);
        LIST_REMOVE(debug_list, hashmap_debug_list, &h->debug);
        assert_se(pthread_mutex_unlock(&hashmap_debug_list_mutex) == 0);
#جيڪڏهن_پورو

        إذا (h->from_pool) {
                /* Ensure that the object didn't get migrated between threads. */
                assert_se(is_main_thread());
                mempool_free_tile(hashmap_type_info[h->type].mempool, h);
        } وإلا
                حرر(h);
}

Hashmap_بنياد* _hashmap_free(Hashmap_بنياد *h) {
        إذا (h) {
                _hashmap_clear(h);
                hashmap_free_no_clear(h);
        }

        أرجع عدم;
}

فراغ _hashmap_clear(Hashmap_بنياد *h) {
        إذا (!h)
                أرجع;

        إذا (h->hash_ops->free_ڪنجي || h->hash_ops->free_قدر) {

                /* If destructor calls are defined, let's destroy things defensively: let's take the item out of the
                 * hash table, and only then call the destructor functions. If these destructors then try to unregister
                 * themselves from our hash table a second time, the entry is already gone. */

                طالما (hashmap_سائيز(h) > 0) {
                        فراغ *k = عدم;
                        فراغ *v;

                        v = hashmap_پهريون_ڪنجي_and_قدر(h, true, &k);

                        إذا (h->hash_ops->free_ڪنجي)
                                h->hash_ops->free_ڪنجي(k);

                        إذا (h->hash_ops->free_قدر)
                                h->hash_ops->free_قدر(v);
                }
        }

        إذا (h->has_اڻسڌو) {
                حرر(h->اڻسڌو.storage);
                h->has_اڻسڌو = false;
        }

        h->n_سڌو_entries = 0;
        reset_سڌو_storage(h);

        إذا (h->type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*) h;
                lh->iterate_list_سر = lh->iterate_list_پڇ = IDX_NIL;
        }

        بنياد_سيٽ_dirty(h);
}

ساكن صحيح resize_buckets(Hashmap_بنياد *h, بنانشان entries_جوڙيو);

/*
 * Finds an empty bucket to put an entry into, starting the scan at 'idx'.
 * Performs Robin Hood swaps as it goes. The entry to put must be placed
 * by the caller into swap slot IDX_PUT.
 * If used for in-place resizing, may leave a displaced entry in swap slot
 * IDX_PUT. Caller must rehash it next.
 * Returns: true if it left a displaced entry to rehash next in IDX_PUT,
 *          false otherwise.
 */
ساكن bool hashmap_put_robin_hood(Hashmap_بنياد *h, بنانشان idx,
                                   بنية مٽا_سٽا_entries *مٽا_سٽا) {
        dib_raw_t raw_dib, *dibs;
        بنانشان dib, مفاصلو;

#إذا ENABLE_DEBUG_HASHMAP
        أكد(h);
        h->debug.put_ڳڻپ++;
#جيڪڏهن_پورو

        dibs = dib_raw_ptr(h);

        لكل (مفاصلو = 0; ; مفاصلو++) {
                raw_dib = dibs[idx];
                إذا (اندريون_سيٽ(raw_dib, DIB_RAW_FREE, DIB_RAW_REHASH)) {
                        إذا (raw_dib == DIB_RAW_REHASH)
                                ٻالٽي_move_داخلا(h, مٽا_سٽا, idx, IDX_TMP);

                        إذا (h->has_اڻسڌو && h->اڻسڌو.idx_سڀ_کان_هيٺ_داخلا > idx)
                                h->اڻسڌو.idx_سڀ_کان_هيٺ_داخلا = idx;

                        ٻالٽي_سيٽ_dib(h, idx, مفاصلو);
                        ٻالٽي_move_داخلا(h, مٽا_سٽا, IDX_PUT, idx);
                        إذا (raw_dib == DIB_RAW_REHASH) {
                                ٻالٽي_move_داخلا(h, مٽا_سٽا, IDX_TMP, IDX_PUT);
                                أرجع true;
                        }

                        أرجع false;
                }

                dib = ٻالٽي_calculate_dib(h, idx, raw_dib);

                إذا (dib < مفاصلو) {
                        /* Found a wealthier entry. Go Robin Hood! */
                        ٻالٽي_سيٽ_dib(h, idx, مفاصلو);

                        /* swap the entries */
                        ٻالٽي_move_داخلا(h, مٽا_سٽا, idx, IDX_TMP);
                        ٻالٽي_move_داخلا(h, مٽا_سٽا, IDX_PUT, idx);
                        ٻالٽي_move_داخلا(h, مٽا_سٽا, IDX_TMP, IDX_PUT);

                        مفاصلو = dib;
                }

                idx = ايندڙ_idx(h, idx);
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
ساكن صحيح hashmap_بنياد_put_boldly(Hashmap_بنياد *h, بنانشان idx,
                                   بنية مٽا_سٽا_entries *مٽا_سٽا, bool may_resize) {
        بنية ordered_hashmap_داخلا *new_داخلا;
        صحيح r;

        أكد(idx < n_buckets(h));

        new_داخلا = ٻالٽي_at_مٽا_سٽا(مٽا_سٽا, IDX_PUT);

        إذا (may_resize) {
                r = resize_buckets(h, 1);
                إذا (r < 0)
                        أرجع r;
                إذا (r > 0)
                        idx = ٻالٽي_hash(h, new_داخلا->p.b.ڪنجي);
        }
        أكد(n_entries(h) < n_buckets(h));

        إذا (h->type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*) h;

                new_داخلا->iterate_ايندڙ = IDX_NIL;
                new_داخلا->iterate_اڳيون = lh->iterate_list_پڇ;

                إذا (lh->iterate_list_پڇ != IDX_NIL) {
                        بنية ordered_hashmap_داخلا *old_پڇ;

                        old_پڇ = ordered_ٻالٽي_at(lh, lh->iterate_list_پڇ);
                        أكد(old_پڇ->iterate_ايندڙ == IDX_NIL);
                        old_پڇ->iterate_ايندڙ = IDX_PUT;
                }

                lh->iterate_list_پڇ = IDX_PUT;
                إذا (lh->iterate_list_سر == IDX_NIL)
                        lh->iterate_list_سر = IDX_PUT;
        }

        assert_se(hashmap_put_robin_hood(h, idx, مٽا_سٽا) == false);

        n_entries_inc(h);
#إذا ENABLE_DEBUG_HASHMAP
        h->debug.max_entries = MAX(h->debug.max_entries, n_entries(h));
#جيڪڏهن_پورو

        بنياد_سيٽ_dirty(h);

        أرجع 1;
}
#وصف_ڪريو hashmap_put_boldly(h, idx, مٽا_سٽا, may_resize) \
        hashmap_بنياد_put_boldly(HASHMAP_بنياد(h), idx, مٽا_سٽا, may_resize)

/*
 * Returns 0 if resize is not needed.
 *         1 if successfully resized.
 *         -ENOMEM on allocation failure.
 */
ساكن صحيح resize_buckets(Hashmap_بنياد *h, بنانشان entries_جوڙيو) {
        بنية مٽا_سٽا_entries مٽا_سٽا;
        فراغ *new_storage;
        dib_raw_t *old_dibs, *new_dibs;
        ثابت بنية hashmap_type_info *hi;
        بنانشان idx, optimal_idx;
        بنانشان old_n_buckets, new_n_buckets, n_rehashed, new_n_entries;
        uint8_t new_سرڪايو;
        bool rehash_ايندڙ;

        أكد(h);

        hi = &hashmap_type_info[h->type];
        new_n_entries = n_entries(h) + entries_جوڙيو;

        /* overflow? */
        إذا (_unlikely_(new_n_entries < entries_جوڙيو))
                أرجع -ENOMEM;

        /* For direct storage we allow 100% load, because it's tiny. */
        إذا (!h->has_اڻسڌو && new_n_entries <= hi->n_سڌو_buckets)
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

        إذا (_unlikely_(new_n_buckets > UINT_MAX / (hi->داخلا_سائيز + حجم(dib_raw_t))))
                أرجع -ENOMEM;

        new_سرڪايو = log2u_ڦيرو_مٿي(MAX(
                        new_n_buckets * (hi->داخلا_سائيز + حجم(dib_raw_t)),
                        2 * حجم(بنية سڌو_storage)));

        /* Realloc storage (buckets and DIB array). */
        new_storage = realloc(h->has_اڻسڌو ? h->اڻسڌو.storage : عدم,
                              1U << new_سرڪايو);
        إذا (!new_storage)
                أرجع -ENOMEM;

        /* Must upgrade direct to indirect storage. */
        إذا (!h->has_اڻسڌو) {
                يادگيري_نقل(new_storage, h->سڌو.storage,
                       old_n_buckets * (hi->داخلا_سائيز + حجم(dib_raw_t)));
                h->اڻسڌو.n_entries = h->n_سڌو_entries;
                h->اڻسڌو.idx_سڀ_کان_هيٺ_داخلا = 0;
                h->n_سڌو_entries = 0;
        }

        /* Get a new hash key. If we've just upgraded to indirect storage,
         * allow reusing a previously generated key. It's still a different key
         * from the shared one that we used for direct storage. */
        get_hash_ڪنجي(h->اڻسڌو.hash_ڪنجي, !h->has_اڻسڌو);

        h->has_اڻسڌو = true;
        h->اڻسڌو.storage = new_storage;
        h->اڻسڌو.n_buckets = (1U << new_سرڪايو) /
                                (hi->داخلا_سائيز + حجم(dib_raw_t));

        old_dibs = (dib_raw_t*)((uint8_t*) new_storage + hi->داخلا_سائيز * old_n_buckets);
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
        memzero(ٻالٽي_at(h, old_n_buckets),
               (n_buckets(h) - old_n_buckets) * hi->داخلا_سائيز);

        /* The upper half of the new DIB array needs initialization */
        memset(&new_dibs[old_n_buckets], DIB_RAW_INIT,
               (n_buckets(h) - old_n_buckets) * حجم(dib_raw_t));

        /* Rehash entries that need it */
        n_rehashed = 0;
        لكل (idx = 0; idx < old_n_buckets; idx++) {
                إذا (new_dibs[idx] != DIB_RAW_REHASH)
                        استمر;

                optimal_idx = ٻالٽي_hash(h, ٻالٽي_at(h, idx)->ڪنجي);

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
                ٻالٽي_move_داخلا(h, &مٽا_سٽا, idx, IDX_PUT);
                /* bucket_move_entry does not clear the source */
                memzero(ٻالٽي_at(h, idx), hi->داخلا_سائيز);

                افعل {
                        /*
                         * Find the new bucket for the current entry. This may make
                         * another entry homeless and load it into IDX_PUT.
                         */
                        rehash_ايندڙ = hashmap_put_robin_hood(h, optimal_idx, &مٽا_سٽا);
                        n_rehashed++;

                        /* Did the current entry displace another one? */
                        إذا (rehash_ايندڙ)
                                optimal_idx = ٻالٽي_hash(h, ٻالٽي_at_مٽا_سٽا(&مٽا_سٽا, IDX_PUT)->p.b.ڪنجي);
                } طالما (rehash_ايندڙ);
        }

        assert_se(n_rehashed == n_entries(h));

        أرجع 1;
}

/*
 * Finds an entry with a matching key
 * Returns: index of the found entry, or IDX_NIL if not found.
 */
ساكن بنانشان بنياد_ٻالٽي_جاچ(Hashmap_بنياد *h, بنانشان idx, ثابت فراغ *ڪنجي) {
        compare_func_t compare = h->hash_ops->compare;
        بنية hashmap_بنياد_داخلا *e;
        بنانشان dib, مفاصلو;
        dib_raw_t *dibs = dib_raw_ptr(h);

        أكد(idx < n_buckets(h));

        لكل (مفاصلو = 0; ; مفاصلو++) {
                إذا (dibs[idx] == DIB_RAW_FREE)
                        أرجع IDX_NIL;

                dib = ٻالٽي_calculate_dib(h, idx, dibs[idx]);

                إذا (dib < مفاصلو)
                        أرجع IDX_NIL;
                إذا (dib == مفاصلو) {
                        e = ٻالٽي_at(h, idx);

                        /* Most maps compare keys by pointer. Do that here, without the indirect call. */
                        إذا (compare == trivial_compare_func ? e->ڪنجي == ڪنجي : compare(e->ڪنجي, ڪنجي) == 0)
                                أرجع idx;
                }

                idx = ايندڙ_idx(h, idx);
        }
}
#وصف_ڪريو ٻالٽي_جاچ(h, idx, ڪنجي) بنياد_ٻالٽي_جاچ(HASHMAP_بنياد(h), idx, ڪنجي)

صحيح hashmap_put(Hashmap *h, ثابت فراغ *ڪنجي, فراغ *قدر) {
        بنية مٽا_سٽا_entries مٽا_سٽا;
        بنية plain_hashmap_داخلا *e;
        بنانشان hash, idx;

        أكد(h);

        hash = ٻالٽي_hash(h, ڪنجي);
        idx = ٻالٽي_جاچ(h, hash, ڪنجي);
        إذا (idx != IDX_NIL) {
                e = plain_ٻالٽي_at(h, idx);
                إذا (e->قدر == قدر)
                        أرجع 0;
                أرجع -EEXIST;
        }

        e = &ٻالٽي_at_مٽا_سٽا(&مٽا_سٽا, IDX_PUT)->p;
        e->b.ڪنجي = ڪنجي;
        e->قدر = قدر;
        أرجع hashmap_put_boldly(h, hash, &مٽا_سٽا, true);
}

صحيح سيٽ_put(سيٽ_2 *s, ثابت فراغ *ڪنجي) {
        بنية مٽا_سٽا_entries مٽا_سٽا;
        بنية hashmap_بنياد_داخلا *e;
        بنانشان hash, idx;

        أكد(s);

        hash = ٻالٽي_hash(s, ڪنجي);
        idx = ٻالٽي_جاچ(s, hash, ڪنجي);
        إذا (idx != IDX_NIL)
                أرجع 0;

        e = &ٻالٽي_at_مٽا_سٽا(&مٽا_سٽا, IDX_PUT)->p.b;
        e->ڪنجي = ڪنجي;
        أرجع hashmap_put_boldly(s, hash, &مٽا_سٽا, true);
}

صحيح سيٽ_ensure_put(سيٽ_2 **s, ثابت بنية hash_ops *hash_ops, ثابت فراغ *ڪنجي) {
        صحيح r;

        أكد(s);

        r = سيٽ_ensure_allocated(s, hash_ops);
        إذا (r < 0)
                أرجع r;

        أرجع سيٽ_put(*s, ڪنجي);
}

صحيح سيٽ_ensure_consume(سيٽ_2 **s, ثابت بنية hash_ops *hash_ops, فراغ *ڪنجي) {
        صحيح r;

        r = سيٽ_ensure_put(s, hash_ops, ڪنجي);
        إذا (r <= 0) {
                إذا (hash_ops && hash_ops->free_ڪنجي)
                        hash_ops->free_ڪنجي(ڪنجي);
                وإلا إذا (hash_ops && hash_ops->free_قدر)
                        /* Sets store their element in the key slot but may carry a value destructor. */
                        hash_ops->free_قدر(ڪنجي);
                وإلا
                        حرر(ڪنجي);
        }

        أرجع r;
}

صحيح hashmap_replace(Hashmap *h, ثابت فراغ *ڪنجي, فراغ *قدر) {
        بنية مٽا_سٽا_entries مٽا_سٽا;
        بنية plain_hashmap_داخلا *e;
        بنانشان hash, idx;

        أكد(h);

        hash = ٻالٽي_hash(h, ڪنجي);
        idx = ٻالٽي_جاچ(h, hash, ڪنجي);
        إذا (idx != IDX_NIL) {
                e = plain_ٻالٽي_at(h, idx);
#إذا ENABLE_DEBUG_HASHMAP
                /* Although the key is equal, the key pointer may have changed,
                 * and this would break our assumption for iterating. So count
                 * this operation as incompatible with iteration. */
                إذا (e->b.ڪنجي != ڪنجي) {
                        h->b.debug.put_ڳڻپ++;
                        h->b.debug.rem_ڳڻپ++;
                        h->b.debug.آخري_rem_idx = idx;
                }
#جيڪڏهن_پورو
                e->b.ڪنجي = ڪنجي;
                e->قدر = قدر;
                hashmap_سيٽ_dirty(h);

                أرجع 0;
        }

        e = &ٻالٽي_at_مٽا_سٽا(&مٽا_سٽا, IDX_PUT)->p;
        e->b.ڪنجي = ڪنجي;
        e->قدر = قدر;
        أرجع hashmap_put_boldly(h, hash, &مٽا_سٽا, true);
}

صحيح hashmap_update(Hashmap *h, ثابت فراغ *ڪنجي, فراغ *قدر) {
        بنية plain_hashmap_داخلا *e;
        بنانشان hash, idx;

        أكد(h);

        hash = ٻالٽي_hash(h, ڪنجي);
        idx = ٻالٽي_جاچ(h, hash, ڪنجي);
        إذا (idx == IDX_NIL)
                أرجع -ENOENT;

        e = plain_ٻالٽي_at(h, idx);
        e->قدر = قدر;
        hashmap_سيٽ_dirty(h);

        أرجع 0;
}

فراغ* _hashmap_get(Hashmap_بنياد *h, ثابت فراغ *ڪنجي) {
        بنية hashmap_بنياد_داخلا *e;
        بنانشان hash, idx;

        إذا (!h)
                أرجع عدم;

        hash = ٻالٽي_hash(h, ڪنجي);
        idx = ٻالٽي_جاچ(h, hash, ڪنجي);
        إذا (idx == IDX_NIL)
                أرجع عدم;

        e = ٻالٽي_at(h, idx);
        أرجع داخلا_قدر(h, e);
}

فراغ* hashmap_get2(Hashmap *h, ثابت فراغ *ڪنجي, فراغ **ret) {
        بنية plain_hashmap_داخلا *e;
        بنانشان hash, idx;

        إذا (!h)
                أرجع عدم;

        hash = ٻالٽي_hash(h, ڪنجي);
        idx = ٻالٽي_جاچ(h, hash, ڪنجي);
        إذا (idx == IDX_NIL)
                أرجع عدم;

        e = plain_ٻالٽي_at(h, idx);
        إذا (ret)
                *ret = (فراغ*) e->b.ڪنجي;

        أرجع e->قدر;
}

bool hashmap_شامل_آهي_2(Hashmap_بنياد *h, ثابت فراغ *ڪنجي) {
        بنانشان hash;

        إذا (!h)
                أرجع false;

        hash = ٻالٽي_hash(h, ڪنجي);
        أرجع ٻالٽي_جاچ(h, hash, ڪنجي) != IDX_NIL;
}

فراغ* _hashmap_remove(Hashmap_بنياد *h, ثابت فراغ *ڪنجي) {
        بنية hashmap_بنياد_داخلا *e;
        بنانشان hash, idx;
        فراغ *معلومات;

        إذا (!h)
                أرجع عدم;

        hash = ٻالٽي_hash(h, ڪنجي);
        idx = ٻالٽي_جاچ(h, hash, ڪنجي);
        إذا (idx == IDX_NIL)
                أرجع عدم;

        e = ٻالٽي_at(h, idx);
        معلومات = داخلا_قدر(h, e);
        remove_داخلا(h, idx);

        أرجع معلومات;
}

فراغ* hashmap_remove2(Hashmap *h, ثابت فراغ *ڪنجي, فراغ **ret) {
        بنية plain_hashmap_داخلا *e;
        بنانشان hash, idx;
        فراغ *معلومات;

        إذا (!h) {
                إذا (ret)
                        *ret = عدم;
                أرجع عدم;
        }

        hash = ٻالٽي_hash(h, ڪنجي);
        idx = ٻالٽي_جاچ(h, hash, ڪنجي);
        إذا (idx == IDX_NIL) {
                إذا (ret)
                        *ret = عدم;
                أرجع عدم;
        }

        e = plain_ٻالٽي_at(h, idx);
        معلومات = e->قدر;
        إذا (ret)
                *ret = (فراغ*) e->b.ڪنجي;

        remove_داخلا(h, idx);

        أرجع معلومات;
}

صحيح hashmap_remove_and_put(Hashmap *h, ثابت فراغ *old_ڪنجي, ثابت فراغ *new_ڪنجي, فراغ *قدر) {
        بنية مٽا_سٽا_entries مٽا_سٽا;
        بنية plain_hashmap_داخلا *e;
        بنانشان old_hash, new_hash, idx;

        إذا (!h)
                أرجع -ENOENT;

        old_hash = ٻالٽي_hash(h, old_ڪنجي);
        idx = ٻالٽي_جاچ(h, old_hash, old_ڪنجي);
        إذا (idx == IDX_NIL)
                أرجع -ENOENT;

        new_hash = ٻالٽي_hash(h, new_ڪنجي);
        إذا (ٻالٽي_جاچ(h, new_hash, new_ڪنجي) != IDX_NIL)
                أرجع -EEXIST;

        remove_داخلا(h, idx);

        e = &ٻالٽي_at_مٽا_سٽا(&مٽا_سٽا, IDX_PUT)->p;
        e->b.ڪنجي = new_ڪنجي;
        e->قدر = قدر;
        assert_se(hashmap_put_boldly(h, new_hash, &مٽا_سٽا, false) == 1);

        أرجع 0;
}

صحيح سيٽ_remove_and_put(سيٽ_2 *s, ثابت فراغ *old_ڪنجي, ثابت فراغ *new_ڪنجي) {
        بنية مٽا_سٽا_entries مٽا_سٽا;
        بنية hashmap_بنياد_داخلا *e;
        بنانشان old_hash, new_hash, idx;

        إذا (!s)
                أرجع -ENOENT;

        old_hash = ٻالٽي_hash(s, old_ڪنجي);
        idx = ٻالٽي_جاچ(s, old_hash, old_ڪنجي);
        إذا (idx == IDX_NIL)
                أرجع -ENOENT;

        new_hash = ٻالٽي_hash(s, new_ڪنجي);
        إذا (ٻالٽي_جاچ(s, new_hash, new_ڪنجي) != IDX_NIL)
                أرجع -EEXIST;

        remove_داخلا(s, idx);

        e = &ٻالٽي_at_مٽا_سٽا(&مٽا_سٽا, IDX_PUT)->p.b;
        e->ڪنجي = new_ڪنجي;
        assert_se(hashmap_put_boldly(s, new_hash, &مٽا_سٽا, false) == 1);

        أرجع 0;
}

صحيح hashmap_remove_and_replace(Hashmap *h, ثابت فراغ *old_ڪنجي, ثابت فراغ *new_ڪنجي, فراغ *قدر) {
        بنية مٽا_سٽا_entries مٽا_سٽا;
        بنية plain_hashmap_داخلا *e;
        بنانشان old_hash, new_hash, idx_old, idx_new;

        إذا (!h)
                أرجع -ENOENT;

        old_hash = ٻالٽي_hash(h, old_ڪنجي);
        idx_old = ٻالٽي_جاچ(h, old_hash, old_ڪنجي);
        إذا (idx_old == IDX_NIL)
                أرجع -ENOENT;

        old_ڪنجي = ٻالٽي_at(HASHMAP_بنياد(h), idx_old)->ڪنجي;

        new_hash = ٻالٽي_hash(h, new_ڪنجي);
        idx_new = ٻالٽي_جاچ(h, new_hash, new_ڪنجي);
        إذا (idx_new != IDX_NIL)
                إذا (idx_old != idx_new) {
                        remove_داخلا(h, idx_new);
                        /* Compensate for a possible backward shift. */
                        إذا (old_ڪنجي != ٻالٽي_at(HASHMAP_بنياد(h), idx_old)->ڪنجي)
                                idx_old = prev_idx(HASHMAP_بنياد(h), idx_old);
                        أكد(old_ڪنجي == ٻالٽي_at(HASHMAP_بنياد(h), idx_old)->ڪنجي);
                }

        remove_داخلا(h, idx_old);

        e = &ٻالٽي_at_مٽا_سٽا(&مٽا_سٽا, IDX_PUT)->p;
        e->b.ڪنجي = new_ڪنجي;
        e->قدر = قدر;
        assert_se(hashmap_put_boldly(h, new_hash, &مٽا_سٽا, false) == 1);

        أرجع 0;
}

فراغ* hashmap_remove_قدر(Hashmap_بنياد *h, ثابت فراغ *ڪنجي, فراغ *قدر) {
        بنية hashmap_بنياد_داخلا *e;
        بنانشان hash, idx;

        إذا (!h)
                أرجع عدم;

        hash = ٻالٽي_hash(h, ڪنجي);
        idx = ٻالٽي_جاچ(h, hash, ڪنجي);
        إذا (idx == IDX_NIL)
                أرجع عدم;

        e = ٻالٽي_at(h, idx);
        إذا (داخلا_قدر(h, e) != قدر)
                أرجع عدم;

        remove_داخلا(h, idx);

        أرجع قدر;
}

ساكن بنانشان ڳوليو_پهريون_داخلا(Hashmap_بنياد *h) {
        Iterator i = ITERATOR_پهريون;

        إذا (!h || !n_entries(h))
                أرجع IDX_NIL;

        أرجع hashmap_iterate_داخلا(h, &i);
}

فراغ* hashmap_پهريون_ڪنجي_and_قدر(Hashmap_بنياد *h, bool remove, فراغ **ret_ڪنجي) {
        بنية hashmap_بنياد_داخلا *e;
        فراغ *ڪنجي, *معلومات;
        بنانشان idx;

        idx = ڳوليو_پهريون_داخلا(h);
        إذا (idx == IDX_NIL) {
                إذا (ret_ڪنجي)
                        *ret_ڪنجي = عدم;
                أرجع عدم;
        }

        e = ٻالٽي_at(h, idx);
        ڪنجي = (فراغ*) e->ڪنجي;
        معلومات = داخلا_قدر(h, e);

        إذا (remove)
                remove_داخلا(h, idx);

        إذا (ret_ڪنجي)
                *ret_ڪنجي = ڪنجي;

        أرجع معلومات;
}

بنانشان hashmap_سائيز(Hashmap_بنياد *h) {
        إذا (!h)
                أرجع 0;

        أرجع n_entries(h);
}

بنانشان _hashmap_buckets(Hashmap_بنياد *h) {
        إذا (!h)
                أرجع 0;

        أرجع n_buckets(h);
}

صحيح hashmap_ملايو(Hashmap *h, Hashmap *other) {
        Iterator i;
        بنانشان idx;

        أكد(h);

        HASHMAP_FOREACH_IDX(idx, HASHMAP_بنياد(other), i) {
                بنية plain_hashmap_داخلا *pe = plain_ٻالٽي_at(other, idx);
                صحيح r;

                r = hashmap_put(h, pe->b.ڪنجي, pe->قدر);
                إذا (r < 0 && r != -EEXIST)
                        أرجع r;
        }

        أرجع 0;
}

صحيح سيٽ_ملايو(سيٽ_2 *s, سيٽ_2 *other) {
        Iterator i;
        بنانشان idx;

        أكد(s);

        HASHMAP_FOREACH_IDX(idx, HASHMAP_بنياد(other), i) {
                بنية سيٽ_داخلا *se = سيٽ_ٻالٽي_at(other, idx);
                صحيح r;

                r = سيٽ_put(s, se->b.ڪنجي);
                إذا (r < 0)
                        أرجع r;
        }

        أرجع 0;
}

صحيح _hashmap_reserve(Hashmap_بنياد *h, بنانشان entries_جوڙيو) {
        صحيح r;

        أكد(h);

        r = resize_buckets(h, entries_جوڙيو);
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
صحيح _hashmap_move(Hashmap_بنياد *h, Hashmap_بنياد *other) {
        بنية مٽا_سٽا_entries مٽا_سٽا;
        بنية hashmap_بنياد_داخلا *e, *n;
        Iterator i;
        بنانشان idx;
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
                بنانشان h_hash;

                e = ٻالٽي_at(other, idx);
                h_hash = ٻالٽي_hash(h, e->ڪنجي);
                إذا (ٻالٽي_جاچ(h, h_hash, e->ڪنجي) != IDX_NIL)
                        استمر;

                n = &ٻالٽي_at_مٽا_سٽا(&مٽا_سٽا, IDX_PUT)->p.b;
                n->ڪنجي = e->ڪنجي;
                إذا (h->type != HASHMAP_TYPE_سيٽ)
                        ((بنية plain_hashmap_داخلا*) n)->قدر =
                                ((بنية plain_hashmap_داخلا*) e)->قدر;
                assert_se(hashmap_put_boldly(h, h_hash, &مٽا_سٽا, false) == 1);

                remove_داخلا(other, idx);
        }

        أرجع 0;
}

صحيح _hashmap_move_one(Hashmap_بنياد *h, Hashmap_بنياد *other, ثابت فراغ *ڪنجي) {
        بنية مٽا_سٽا_entries مٽا_سٽا;
        بنانشان h_hash, other_hash, idx;
        بنية hashmap_بنياد_داخلا *e, *n;
        صحيح r;

        أكد(h);

        h_hash = ٻالٽي_hash(h, ڪنجي);
        إذا (ٻالٽي_جاچ(h, h_hash, ڪنجي) != IDX_NIL)
                أرجع -EEXIST;

        إذا (!other)
                أرجع -ENOENT;

        أكد(other->type == h->type);

        other_hash = ٻالٽي_hash(other, ڪنجي);
        idx = ٻالٽي_جاچ(other, other_hash, ڪنجي);
        إذا (idx == IDX_NIL)
                أرجع -ENOENT;

        e = ٻالٽي_at(other, idx);

        n = &ٻالٽي_at_مٽا_سٽا(&مٽا_سٽا, IDX_PUT)->p.b;
        n->ڪنجي = e->ڪنجي;
        إذا (h->type != HASHMAP_TYPE_سيٽ)
                ((بنية plain_hashmap_داخلا*) n)->قدر =
                        ((بنية plain_hashmap_داخلا*) e)->قدر;
        r = hashmap_put_boldly(h, h_hash, &مٽا_سٽا, true);
        إذا (r < 0)
                أرجع r;

        remove_داخلا(other, idx);
        أرجع 0;
}

Hashmap_بنياد* _hashmap_copy(Hashmap_بنياد *h) {
        Hashmap_بنياد *copy;
        صحيح r;

        أكد(h);

        copy = hashmap_بنياد_new(h->hash_ops, h->type);
        إذا (!copy)
                أرجع عدم;

        بدل (h->type) {
        حالة HASHMAP_TYPE_PLAIN:
        حالة HASHMAP_TYPE_ORDERED:
                r = hashmap_ملايو_2((Hashmap*)copy, (Hashmap*)h);
                اكسر;
        حالة HASHMAP_TYPE_سيٽ:
                r = سيٽ_ملايو((سيٽ_2*)copy, (سيٽ_2*)h);
                اكسر;
        افتراضي:
                assert_not_reached();
        }

        إذا (r < 0)
                أرجع _hashmap_free(copy);

        أرجع copy;
}

حرف** _hashmap_get_strv(Hashmap_بنياد *h) {
        حرف **sv;
        Iterator i;
        بنانشان idx, n;

        إذا (!h)
                أرجع new0(حرف*, 1);

        sv = new(حرف*, n_entries(h)+1);
        إذا (!sv)
                أرجع عدم;

        n = 0;
        HASHMAP_FOREACH_IDX(idx, h, i)
                sv[n++] = داخلا_قدر(h, ٻالٽي_at(h, idx));
        sv[n] = عدم;

        أرجع sv;
}

حرف** سيٽ_منزل_strv(سيٽ_2 **s) {
        أكد(s);

        /* This is similar to set_get_strv(), but invalidates the set on success. */

        حرف **v = new(حرف*, سيٽ_سائيز(*s) + 1);
        إذا (!v)
                أرجع عدم;

        لكل (حرف **p = v; (*p = سيٽ_steal_پهريون(*s)); p++)
                ;

        أكد(سيٽ_isempty(*s));
        *s = سيٽ_free(*s);
        أرجع v;
}

فراغ* ordered_hashmap_ايندڙ(OrderedHashmap *h, ثابت فراغ *ڪنجي) {
        بنية ordered_hashmap_داخلا *e;
        بنانشان hash, idx;

        إذا (!h)
                أرجع عدم;

        hash = ٻالٽي_hash(h, ڪنجي);
        idx = ٻالٽي_جاچ(h, hash, ڪنجي);
        إذا (idx == IDX_NIL)
                أرجع عدم;

        e = ordered_ٻالٽي_at(h, idx);
        إذا (e->iterate_ايندڙ == IDX_NIL)
                أرجع عدم;
        أرجع ordered_ٻالٽي_at(h, e->iterate_ايندڙ)->p.قدر;
}

صحيح سيٽ_consume(سيٽ_2 *s, فراغ *قدر) {
        صحيح r;

        أكد(s);
        أكد(قدر);

        r = سيٽ_put(s, قدر);
        إذا (r <= 0)
                حرر(قدر);

        أرجع r;
}

صحيح hashmap_put_strdup_ڀريل(Hashmap **h, ثابت بنية hash_ops *hash_ops, ثابت حرف *k, ثابت حرف *v) {
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

صحيح سيٽ_put_strndup_ڀريل(سيٽ_2 **s, ثابت بنية hash_ops *hash_ops, ثابت حرف *p, size_t n) {
        حرف *c;
        صحيح r;

        أكد(s);
        أكد(p);

        r = سيٽ_ensure_allocated(s, hash_ops);
        إذا (r < 0)
                أرجع r;

        إذا (n == SIZE_MAX) {
                إذا (سيٽ_شامل_آهي(*s, (حرف*) p))
                        أرجع 0;

                c = strdup(p);
        } وإلا
                c = strndup(p, n);
        إذا (!c)
                أرجع -ENOMEM;

        أرجع سيٽ_consume(*s, c);
}

صحيح سيٽ_put_strdupv_ڀريل(سيٽ_2 **s, ثابت بنية hash_ops *hash_ops, حرف **l) {
        صحيح n = 0, r;

        أكد(s);

        STRV_FOREACH(i, l) {
                r = سيٽ_put_strndup_ڀريل(s, hash_ops, *i, SIZE_MAX);
                إذا (r < 0)
                        أرجع r;

                n += r;
        }

        أرجع n;
}

صحيح سيٽ_put_strsplit(سيٽ_2 *s, ثابت حرف *v, ثابت حرف *separators, ڪڍو_Flags flags) {
        ثابت حرف *p = ASSERT_PTR(v);
        صحيح r;

        أكد(s);

        لكل (;;) {
                حرف *لفظ;

                r = ڪڍو_پهريون_لفظ(&p, &لفظ, separators, flags);
                إذا (r <= 0)
                        أرجع r;

                r = سيٽ_consume(s, لفظ);
                إذا (r < 0)
                        أرجع r;
        }
}

/* expand the cachemem if needed, return true if newly (re)activated. */
ساكن صحيح cachemem_maintain(CacheMem *mem, size_t سائيز) {
        أكد(mem);

        إذا (!GREEDY_REALLOC(mem->ptr, سائيز)) {
                إذا (سائيز > 0)
                        أرجع -ENOMEM;
        }

        إذا (!mem->سرگرم) {
                mem->سرگرم = true;
                أرجع true;
        }

        أرجع false;
}

صحيح iterated_cache_get(IteratedCache *cache, ثابت فراغ ***res_ڪنجيون, ثابت فراغ ***res_قدرون, بنانشان *res_n_entries) {
        bool sync_ڪنجيون = false, sync_قدرون = false;
        size_t سائيز;
        صحيح r;

        أكد(cache);
        أكد(cache->hashmap);

        سائيز = n_entries(cache->hashmap);

        إذا (res_ڪنجيون) {
                r = cachemem_maintain(&cache->ڪنجيون, سائيز);
                إذا (r < 0)
                        أرجع r;

                sync_ڪنجيون = r;
        } وإلا
                cache->ڪنجيون.سرگرم = false;

        إذا (res_قدرون) {
                r = cachemem_maintain(&cache->values, سائيز);
                إذا (r < 0)
                        أرجع r;

                sync_قدرون = r;
        } وإلا
                cache->values.سرگرم = false;

        إذا (cache->hashmap->dirty) {
                إذا (cache->ڪنجيون.سرگرم)
                        sync_ڪنجيون = true;
                إذا (cache->values.سرگرم)
                        sync_قدرون = true;

                cache->hashmap->dirty = false;
        }

        إذا (sync_ڪنجيون || sync_قدرون) {
                بنانشان i, idx;
                Iterator iter;

                i = 0;
                HASHMAP_FOREACH_IDX(idx, cache->hashmap, iter) {
                        بنية hashmap_بنياد_داخلا *e;

                        e = ٻالٽي_at(cache->hashmap, idx);

                        إذا (sync_ڪنجيون)
                                cache->ڪنجيون.ptr[i] = e->ڪنجي;
                        إذا (sync_قدرون)
                                cache->values.ptr[i] = داخلا_قدر(cache->hashmap, e);
                        i++;
                }
        }

        إذا (res_ڪنجيون)
                *res_ڪنجيون = cache->ڪنجيون.ptr;
        إذا (res_قدرون)
                *res_قدرون = cache->values.ptr;
        إذا (res_n_entries)
                *res_n_entries = سائيز;

        أرجع 0;
}

IteratedCache* iterated_cache_free(IteratedCache *cache) {
        إذا (cache) {
                حرر(cache->ڪنجيون.ptr);
                حرر(cache->values.ptr);
        }

        أرجع mfree(cache);
}

صحيح سيٽ_strjoin(سيٽ_2 *s, ثابت حرف *separator, bool wrap_with_separator, حرف **ret) {
        _cleanup_free_ حرف *str = عدم;
        size_t separator_len, len = 0;
        ثابت حرف *قدر;
        bool پهريون;

        أكد(ret);

        إذا (سيٽ_isempty(s)) {
                *ret = عدم;
                أرجع 0;
        }

        separator_len = strlen_ptr(separator);

        إذا (separator_len == 0)
                wrap_with_separator = false;

        پهريون = !wrap_with_separator;

        سيٽ_FOREACH(قدر, s) {
                size_t l = strlen_ptr(قدر);

                إذا (l == 0)
                        استمر;

                إذا (!GREEDY_REALLOC(str, len + l + (پهريون ? 0 : separator_len) + (wrap_with_separator ? separator_len : 0) + 1))
                        أرجع -ENOMEM;

                إذا (separator_len > 0 && !پهريون) {
                        يادگيري_نقل(str + len, separator, separator_len);
                        len += separator_len;
                }

                يادگيري_نقل(str + len, قدر, l);
                len += l;
                پهريون = false;
        }

        إذا (wrap_with_separator) {
                يادگيري_نقل(str + len, separator, separator_len);
                len += separator_len;
        }

        str[len] = '\0';

        *ret = TAKE_PTR(str);
        أرجع 0;
}

bool سيٽ_equal(سيٽ_2 *a, سيٽ_2 *b) {
        فراغ *p;

        /* Checks whether each entry of 'a' is also in 'b' and vice versa, i.e. the two sets contain the same
         * entries */

        إذا (a == b)
                أرجع true;

        إذا (سيٽ_isempty(a) && سيٽ_isempty(b))
                أرجع true;

        إذا (سيٽ_سائيز(a) != سيٽ_سائيز(b)) /* Cheap check that hopefully catches a lot of inequality cases
                                         * already */
                أرجع false;

        سيٽ_FOREACH(p, a)
                إذا (!سيٽ_شامل_آهي(b, p))
                        أرجع false;

        /* If we have the same hashops, then we don't need to check things backwards given we compared the
         * size and that all of a is in b. */
        إذا (a->b.hash_ops == b->b.hash_ops)
                أرجع true;

        سيٽ_FOREACH(p, b)
                إذا (!سيٽ_شامل_آهي(a, p))
                        أرجع false;

        أرجع true;
}

ساكن bool سيٽ_fnmatch_one(سيٽ_2 *patterns, ثابت حرف *needle) {
        ثابت حرف *p;

        أكد(needle);

        /* Any failure of fnmatch() is treated as equivalent to FNM_NOMATCH, i.e. as non-matching pattern */

        سيٽ_FOREACH(p, patterns)
                إذا (fnmatch(p, needle, 0) == 0)
                        أرجع true;

        أرجع false;
}

bool سيٽ_fnmatch(سيٽ_2 *include_patterns, سيٽ_2 *exclude_patterns, ثابت حرف *needle) {
        أكد(needle);

        إذا (سيٽ_fnmatch_one(exclude_patterns, needle))
                أرجع false;

        إذا (سيٽ_isempty(include_patterns))
                أرجع true;

        أرجع سيٽ_fnmatch_one(include_patterns, needle);
}

ساكن صحيح hashmap_داخلا_compare(
                بنية hashmap_بنياد_داخلا * ثابت *a,
                بنية hashmap_بنياد_داخلا * ثابت *b,
                compare_func_t compare) {

        أكد(a && *a);
        أكد(b && *b);
        أكد(compare);

        أرجع compare((*a)->ڪنجي, (*b)->ڪنجي);
}

ساكن صحيح _hashmap_dump_entries_sorted(
                Hashmap_بنياد *h,
                فراغ ***ret,
                size_t *ret_n) {
        _cleanup_free_ فراغ **entries = عدم;
        Iterator iter;
        بنانشان idx;
        size_t n = 0;

        أكد(ret);
        أكد(ret_n);

        إذا (hashmap_سائيز(h) == 0) {
                *ret = عدم;
                *ret_n = 0;
                أرجع 0;
        }

        /* We append one more element than needed so that the resulting array can be used as a strv. We
         * don't count this entry in the returned size. */
        entries = new(فراغ*, hashmap_سائيز(h) + 1);
        إذا (!entries)
                أرجع -ENOMEM;

        HASHMAP_FOREACH_IDX(idx, h, iter)
                entries[n++] = ٻالٽي_at(h, idx);

        أكد(n == hashmap_سائيز(h));
        entries[n] = عدم;

        typesafe_qsort_r((بنية hashmap_بنياد_داخلا**) entries, n,
                         hashmap_داخلا_compare, h->hash_ops->compare);

        *ret = TAKE_PTR(entries);
        *ret_n = n;
        أرجع 0;
}

صحيح hashmap_dump_ڪنجيون_sorted(Hashmap_بنياد *h, فراغ ***ret, size_t *ret_n) {
        _cleanup_free_ فراغ **entries = عدم;
        size_t n;
        صحيح r;

        أكد(ret);

        r = _hashmap_dump_entries_sorted(h, &entries, &n);
        إذا (r < 0)
                أرجع r;

        /* Reuse the array. */
        FOREACH_ARRAY(e, entries, n)
                *e = (فراغ*) (*(بنية hashmap_بنياد_داخلا**) e)->ڪنجي;

        *ret = TAKE_PTR(entries);
        إذا (ret_n)
                *ret_n = n;
        أرجع 0;
}

صحيح _hashmap_dump_sorted(Hashmap_بنياد *h, فراغ ***ret, size_t *ret_n) {
        _cleanup_free_ فراغ **entries = عدم;
        size_t n;
        صحيح r;

        أكد(ret);

        r = _hashmap_dump_entries_sorted(h, &entries, &n);
        إذا (r < 0)
                أرجع r;

        /* Reuse the array. */
        FOREACH_ARRAY(e, entries, n)
                *e = داخلا_قدر(h, *(بنية hashmap_بنياد_داخلا**) e);

        *ret = TAKE_PTR(entries);
        إذا (ret_n)
                *ret_n = n;
        أرجع 0;
}
