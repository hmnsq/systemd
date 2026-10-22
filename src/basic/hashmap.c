/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <fnmatch.h>
#ಅನ್ತರ್ಭಾವ <pthread.h>
#ಅನ್ತರ್ಭಾವ <unistd.h>
#ಆದರೆ HAVE_VALGRIND_VALGRIND_H
#  include <valgrind/valgrind.h>
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "extract-word.h"
#ಅನ್ತರ್ಭಾವ "hashmap.h"
#ಅನ್ತರ್ಭಾವ "log.h"
#ಅನ್ತರ್ಭಾವ "logarithm.h"
#ಅನ್ತರ್ಭಾವ "memory-util.h"
#ಅನ್ತರ್ಭಾವ "mempool.h"
#ಅನ್ತರ್ಭಾವ "process-util.h"
#ಅನ್ತರ್ಭಾವ "random-util.h"
#ಅನ್ತರ್ಭಾವ "set.h"
#ಅನ್ತರ್ಭಾವ "siphash24.h"
#ಅನ್ತರ್ಭಾವ "sort-util.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"
#ಅನ್ತರ್ಭಾವ "strv.h"

#ಆದರೆ ENABLE_DEBUG_HASHMAP
#ಅನ್ತರ್ಭಾವ "list.h"
#ಆದರೆ_ಅಂತ್ಯ

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
#ವ್ಯಾಖ್ಯಾನಿಸು INV_KEEP_FREE            5U

/* Fields common to entries of all hashmap/set types */
ರಚನೆ hashmap_ತಳ_ನಮೂದು {
        ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ;
};

/* Entry types for specific hashmap/set types
 * hashmap_base_entry must be at the beginning of each entry struct. */

ರಚನೆ plain_hashmap_ನಮೂದು {
        ರಚನೆ hashmap_ತಳ_ನಮೂದು b;
        ಶೂನ್ಯ *ಮೌಲ್ಯ;
};

ರಚನೆ ordered_hashmap_ನಮೂದು {
        ರಚನೆ plain_hashmap_ನಮೂದು p;
        ಚಿಹ್ನೆರಹಿತ iterate_ಮುಂದಿನ, iterate_ಹಿಂದಿನ;
};

ರಚನೆ ಗಣ_ನಮೂದು {
        ರಚನೆ hashmap_ತಳ_ನಮೂದು b;
};

/* In several functions it is advantageous to have the hash table extended
 * virtually by a couple of additional buckets. We reserve special index values
 * for these "swap" buckets. */
#ವ್ಯಾಖ್ಯಾನಿಸು IDX_ಅದಲುಬದಲು_BEGIN     (UINT_MAX - 3)
#ವ್ಯಾಖ್ಯಾನಿಸು IDX_PUT             (IDX_ಅದಲುಬದಲು_BEGIN + 0)
#ವ್ಯಾಖ್ಯಾನಿಸು IDX_TMP             (IDX_ಅದಲುಬದಲು_BEGIN + 1)
#ವ್ಯಾಖ್ಯಾನಿಸು IDX_ಅದಲುಬದಲು_ಕೊನೆ       (IDX_ಅದಲುಬದಲು_BEGIN + 2)

#ವ್ಯಾಖ್ಯಾನಿಸು IDX_ಮೊದಲ           (UINT_MAX - 1) /* special index for freshly initialized iterators */
#ವ್ಯಾಖ್ಯಾನಿಸು IDX_NIL             UINT_MAX       /* special index value meaning "none" or "end" */

assert_cc(IDX_ಮೊದಲ == IDX_ಅದಲುಬದಲು_ಕೊನೆ);
assert_cc(IDX_ಮೊದಲ == IDX_ITERATOR_ಮೊದಲ);

/* Storage space for the "swap" buckets.
 * All entry types can fit into an ordered_hashmap_entry. */
ರಚನೆ ಅದಲುಬದಲು_entries {
        ರಚನೆ ordered_hashmap_ನಮೂದು e[IDX_ಅದಲುಬದಲು_ಕೊನೆ - IDX_ಅದಲುಬದಲು_BEGIN];
};

/* Distance from Initial Bucket */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ uint8_t dib_raw_t;
#ವ್ಯಾಖ್ಯಾನಿಸು DIB_RAW_ಅತಿಪ್ರವಾಹ ((dib_raw_t)0xfdU)   /* indicates DIB value is greater than representable */
#ವ್ಯಾಖ್ಯಾನಿಸು DIB_RAW_REHASH   ((dib_raw_t)0xfeU)   /* entry yet to be rehashed during in-place resize */
#ವ್ಯಾಖ್ಯಾನಿಸು DIB_RAW_FREE     ((dib_raw_t)0xffU)   /* a free bucket */
#ವ್ಯಾಖ್ಯಾನಿಸು DIB_RAW_INIT     ((ಅಕ್ಷರ)DIB_RAW_FREE) /* a byte to memset a DIB store with when initializing */

#ವ್ಯಾಖ್ಯಾನಿಸು DIB_FREE UINT_MAX

#ಆದರೆ ENABLE_DEBUG_HASHMAP
ರಚನೆ hashmap_debug_info {
        LIST_FIELDS(ರಚನೆ hashmap_debug_info, debug_list);
        ಚಿಹ್ನೆರಹಿತ max_entries;  /* high watermark of n_entries */

        /* fields to detect modification while iterating */
        ಚಿಹ್ನೆರಹಿತ put_ಎಣಿಕೆ;    /* counts puts into the hashmap */
        ಚಿಹ್ನೆರಹಿತ rem_ಎಣಿಕೆ;    /* counts removals from hashmap */
        ಚಿಹ್ನೆರಹಿತ ಕೊನೆಯ_rem_idx; /* remembers last removal index */
};

/* Tracks all existing hashmaps. Get at it from gdb. See sd_dump_hashmaps.py */
ಸ್ಥಿರ LIST_ತಲೆ(ರಚನೆ hashmap_debug_info, hashmap_debug_list);
ಸ್ಥಿರ pthread_mutex_t hashmap_debug_list_mutex = PTHREAD_MUTEX_INITIALIZER;
#ಆದರೆ_ಅಂತ್ಯ

ಎಣಿಕೆ HashmapType {
        HASHMAP_TYPE_PLAIN,
        HASHMAP_TYPE_ORDERED,
        HASHMAP_TYPE_ಗಣ,
        _HASHMAP_TYPE_MAX
};

ರಚನೆ _packed_ ಪರೋಕ್ಷ_storage {
        ಶೂನ್ಯ *storage;                     /* where buckets and DIBs are stored */
        uint8_t  hash_ಕೀಲಿ[HASH_ಕೀಲಿ_ಗಾತ್ರ];  /* hash key; changes during resize */

        ಚಿಹ್ನೆರಹಿತ n_entries;                /* number of stored entries */
        ಚಿಹ್ನೆರಹಿತ n_buckets;                /* number of buckets */

        ಚಿಹ್ನೆರಹಿತ idx_ಅತಿಕಡಿಮೆ_ನಮೂದು;         /* Index below which all buckets are free.
                                              Makes "while (hashmap_steal_first())" loops
                                              O(n) instead of O(n^2) for unordered hashmaps. */
        uint8_t  _pad[3];                  /* padding for the whole HashmapBase */
        /* The bitfields in HashmapBase complete the alignment of the whole thing. */
};

ರಚನೆ ನೇರ_storage {
        /* This gives us 39 bytes on 64-bit, or 35 bytes on 32-bit.
         * That's room for 4 set_entries + 4 DIB bytes + 3 unused bytes on 64-bit,
         *              or 7 set_entries + 7 DIB bytes + 0 unused bytes on 32-bit. */
        uint8_t storage[ಗಾತ್ರ(ರಚನೆ ಪರೋಕ್ಷ_storage)];
};

#ವ್ಯಾಖ್ಯಾನಿಸು ನೇರ_BUCKETS(ನಮೂದು_t) \
        (ಗಾತ್ರ(ರಚನೆ ನೇರ_storage) / (ಗಾತ್ರ(ನಮೂದು_t) + ಗಾತ್ರ(dib_raw_t)))

/* We should be able to store at least one entry directly. */
assert_cc(ನೇರ_BUCKETS(ರಚನೆ ordered_hashmap_ನಮೂದು) >= 1);

/* We have 3 bits for n_direct_entries. */
assert_cc(ನೇರ_BUCKETS(ರಚನೆ ಗಣ_ನಮೂದು) < (1 << 3));

/* Hashmaps with directly stored entries all use this shared hash key.
 * It's no big deal if the key is guessed, because there can be only
 * a handful of directly stored entries in a hashmap. When a hashmap
 * outgrows direct storage, it gets its own key for indirect storage. */
ಸ್ಥಿರ uint8_t shared_hash_ಕೀಲಿ[HASH_ಕೀಲಿ_ಗಾತ್ರ];

/* Fields that all hashmap/set types must have */
ರಚನೆ Hashmap_ತಳ {
        ಸ್ಥಿರಾಂಕ ರಚನೆ hash_ops *hash_ops;  /* hash and compare ops to use */

        ಒಕ್ಕೂಟ _packed_ {
                ರಚನೆ ಪರೋಕ್ಷ_storage ಪರೋಕ್ಷ; /* if  has_indirect */
                ರಚನೆ ನೇರ_storage ನೇರ;     /* if !has_indirect */
        };

        ಎಣಿಕೆ HashmapType type:2;     /* HASHMAP_TYPE_* */
        bool has_ಪರೋಕ್ಷ:1;         /* whether indirect storage is used */
        ಚಿಹ್ನೆರಹಿತ n_ನೇರ_entries:3; /* Number of entries in direct storage.
                                      * Only valid if !has_indirect. */
        bool from_pool:1;            /* whether was allocated from mempool */
        bool dirty:1;                /* whether dirtied since last iterated_cache_get() */
        bool cached:1;               /* whether this hashmap is being cached */

#ಆದರೆ ENABLE_DEBUG_HASHMAP
        ರಚನೆ hashmap_debug_info debug;
#ಆದರೆ_ಅಂತ್ಯ
};

/* Specific hash types
 * HashmapBase must be at the beginning of each hashmap struct. */

ರಚನೆ Hashmap {
        ರಚನೆ Hashmap_ತಳ b;
};

ರಚನೆ OrderedHashmap {
        ರಚನೆ Hashmap_ತಳ b;
        ಚಿಹ್ನೆರಹಿತ iterate_list_ತಲೆ, iterate_list_ಬಾಲ;
};

ರಚನೆ ಗಣ_2 {
        ರಚನೆ Hashmap_ತಳ b;
};

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ CacheMem {
        ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ **ptr;
        size_t n_populated;
        bool ಸಕ್ರಿಯ:1;
} CacheMem;

ರಚನೆ IteratedCache {
        Hashmap_ತಳ *hashmap;
        CacheMem ಕೀಲಿಗಳು, values;
};

DEFINE_MEMPOOL(hashmap_pool,         Hashmap,        8);
DEFINE_MEMPOOL(ordered_hashmap_pool, OrderedHashmap, 8);
/* No need for a separate Set pool */
assert_cc(ಗಾತ್ರ(Hashmap) == ಗಾತ್ರ(ಗಣ_2));

ರಚನೆ hashmap_type_info {
        size_t ತಲೆ_ಗಾತ್ರ;
        size_t ನಮೂದು_ಗಾತ್ರ;
        ರಚನೆ mempool *mempool;
        ಚಿಹ್ನೆರಹಿತ n_ನೇರ_buckets;
};

ಸ್ಥಿರ ಬಳಸಿದ_2 ಸ್ಥಿರಾಂಕ ರಚನೆ hashmap_type_info hashmap_type_info[_HASHMAP_TYPE_MAX] = {
        [HASHMAP_TYPE_PLAIN] = {
                .ತಲೆ_ಗಾತ್ರ        = ಗಾತ್ರ(Hashmap),
                .ನಮೂದು_ಗಾತ್ರ       = ಗಾತ್ರ(ರಚನೆ plain_hashmap_ನಮೂದು),
                .mempool          = &hashmap_pool,
                .n_ನೇರ_buckets = ನೇರ_BUCKETS(ರಚನೆ plain_hashmap_ನಮೂದು),
        },
        [HASHMAP_TYPE_ORDERED] = {
                .ತಲೆ_ಗಾತ್ರ        = ಗಾತ್ರ(OrderedHashmap),
                .ನಮೂದು_ಗಾತ್ರ       = ಗಾತ್ರ(ರಚನೆ ordered_hashmap_ನಮೂದು),
                .mempool          = &ordered_hashmap_pool,
                .n_ನೇರ_buckets = ನೇರ_BUCKETS(ರಚನೆ ordered_hashmap_ನಮೂದು),
        },
        [HASHMAP_TYPE_ಗಣ] = {
                .ತಲೆ_ಗಾತ್ರ        = ಗಾತ್ರ(ಗಣ_2),
                .ನಮೂದು_ಗಾತ್ರ       = ಗಾತ್ರ(ರಚನೆ ಗಣ_ನಮೂದು),
                .mempool          = &hashmap_pool,
                .n_ನೇರ_buckets = ನೇರ_BUCKETS(ರಚನೆ ಗಣ_ನಮೂದು),
        },
};

ಶೂನ್ಯ hashmap_trim_pools(ಶೂನ್ಯ) {
        ಪೂರ್ಣಾಂಕ r;

        /* The pool is only allocated by the main thread, but the memory can be passed to other
         * threads. Let's clean up if we are the main thread and no other threads are live. */

        /* We build our own is_main_thread() here, which doesn't use C11 TLS based caching of the
         * result. That's because valgrind apparently doesn't like TLS to be used from a GCC destructor. */
        ಆದರೆ (getpid() != gettid())
                ಹಿಂತಿರುಗಿ (ಶೂನ್ಯ) log_debug("Not cleaning up memory pools, not in main thread.");

        r = get_process_ಎಳೆಗಳು(0);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ (ಶೂನ್ಯ) log_debug_errno(r, "Failed to determine number of threads, not cleaning up memory pools: %m");
        ಆದರೆ (r != 1)
                ಹಿಂತಿರುಗಿ (ಶೂನ್ಯ) log_debug("Not cleaning up memory pools, running in multi-threaded process.");

        mempool_trim(&hashmap_pool);
        mempool_trim(&ordered_hashmap_pool);
}

#ಆದರೆ HAVE_VALGRIND_VALGRIND_H
_destructor_ ಸ್ಥಿರ ಶೂನ್ಯ cleanup_pools(ಶೂನ್ಯ) {
        /* Be nice to valgrind */
        ಆದರೆ (RUNNING_ON_VALGRIND)
                hashmap_trim_pools();
}
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಚಿಹ್ನೆರಹಿತ n_buckets(Hashmap_ತಳ *h) {
        ಪ್ರತಿಜ್ಞಾ(h);
        ಹಿಂತಿರುಗಿ h->has_ಪರೋಕ್ಷ ? h->ಪರೋಕ್ಷ.n_buckets
                               : hashmap_type_info[h->type].n_ನೇರ_buckets;
}

ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ n_entries(Hashmap_ತಳ *h) {
        ಪ್ರತಿಜ್ಞಾ(h);
        ಹಿಂತಿರುಗಿ h->has_ಪರೋಕ್ಷ ? h->ಪರೋಕ್ಷ.n_entries
                               : h->n_ನೇರ_entries;
}

ಸ್ಥಿರ ಶೂನ್ಯ n_entries_inc(Hashmap_ತಳ *h) {
        ಪ್ರತಿಜ್ಞಾ(h);

        ಆದರೆ (h->has_ಪರೋಕ್ಷ)
                h->ಪರೋಕ್ಷ.n_entries++;
        ಇಲ್ಲದಿದ್ದರೆ
                h->n_ನೇರ_entries++;
}

ಸ್ಥಿರ ಶೂನ್ಯ n_entries_dec(Hashmap_ತಳ *h) {
        ಪ್ರತಿಜ್ಞಾ(h);

        ಆದರೆ (h->has_ಪರೋಕ್ಷ)
                h->ಪರೋಕ್ಷ.n_entries--;
        ಇಲ್ಲದಿದ್ದರೆ
                h->n_ನೇರ_entries--;
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ* storage_ptr(Hashmap_ತಳ *h) {
        ಪ್ರತಿಜ್ಞಾ(h);
        ಹಿಂತಿರುಗಿ h->has_ಪರೋಕ್ಷ ? h->ಪರೋಕ್ಷ.storage
                               : h->ನೇರ.storage;
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ uint8_t* hash_ಕೀಲಿ(Hashmap_ತಳ *h) {
        ಪ್ರತಿಜ್ಞಾ(h);
        ಹಿಂತಿರುಗಿ h->has_ಪರೋಕ್ಷ ? h->ಪರೋಕ್ಷ.hash_ಕೀಲಿ
                               : shared_hash_ಕೀಲಿ;
}

ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ತಳ_ಪಾತ್ರೆ_hash(Hashmap_ತಳ *h, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *p) {
        ರಚನೆ siphash state;
        uint64_t hash;

        ಪ್ರತಿಜ್ಞಾ(h);

        /* The keys of trivial_hash_ops and uint64_hash_ops fit in 64 bits. siphash24_uint64() hashes such
         * a key without the indirect call through hash_ops and keeps the SipHash state in registers. On
         * 32-bit and big-endian architectures, the result differs from the result of trivial_hash_func(),
         * which hashes the key in native byte order. Only this function maps keys to buckets, so the
         * difference does not matter. */
        ಆದರೆ (h->hash_ops->hash == trivial_hash_func)
                hash = siphash24_uint64((uintptr_t) p, hash_ಕೀಲಿ(h));
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (h->hash_ops->hash == (hash_func_t) uint64_hash_func)
                hash = siphash24_uint64(*(ಸ್ಥಿರಾಂಕ uint64_t*) p, hash_ಕೀಲಿ(h));
        ಇಲ್ಲದಿದ್ದರೆ {
                siphash24_init(&state, hash_ಕೀಲಿ(h));
                h->hash_ops->hash(p, &state);
                hash = siphash24_finalize(&state);
        }

        /* n_buckets is not a power of two, so a modulo needs a division. Multiplying the upper 32 bits of
         * the hash with n_buckets and keeping the upper 32 bits of the product maps the hash to
         * [0, n_buckets) almost as evenly, without a division. */
        ಹಿಂತಿರುಗಿ (ಚಿಹ್ನೆರಹಿತ) (((hash >> 32) * n_buckets(h)) >> 32);
}
#ವ್ಯಾಖ್ಯಾನಿಸು ಪಾತ್ರೆ_hash(h, p) ತಳ_ಪಾತ್ರೆ_hash(HASHMAP_ತಳ(h), p)

ಸ್ಥಿರ ಶೂನ್ಯ ತಳ_ಗಣ_dirty(Hashmap_ತಳ *h) {
        ಪ್ರತಿಜ್ಞಾ(h);

        h->dirty = true;
}
#ವ್ಯಾಖ್ಯಾನಿಸು hashmap_ಗಣ_dirty(h) ತಳ_ಗಣ_dirty(HASHMAP_ತಳ(h))

ಸ್ಥಿರ ಶೂನ್ಯ get_hash_ಕೀಲಿ(uint8_t hash_ಕೀಲಿ[HASH_ಕೀಲಿ_ಗಾತ್ರ], bool reuse_is_ok) {
        ಸ್ಥಿರ uint8_t current[HASH_ಕೀಲಿ_ಗಾತ್ರ];
        ಸ್ಥಿರ bool current_initialized = false;

        /* Returns a hash function key to use. In order to keep things
         * fast we will not generate a new key each time we allocate a
         * new hash table. Instead, we'll just reuse the most recently
         * generated one, except if we never generated one or when we
         * are rehashing an entire hash table because we reached a
         * fill level */

        ಆದರೆ (!current_initialized || !reuse_is_ok) {
                random_ಬೈಟುಗಳು(current, ಗಾತ್ರ(current));
                current_initialized = true;
        }

        ಸ್ಮರಣೆ_ನಕಲು(hash_ಕೀಲಿ, current, ಗಾತ್ರ(current));
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ರಚನೆ hashmap_ತಳ_ನಮೂದು* ಪಾತ್ರೆ_at(Hashmap_ತಳ *h, ಚಿಹ್ನೆರಹಿತ idx) {
        ಪ್ರತಿಜ್ಞಾ(h);
        ಹಿಂತಿರುಗಿ CAST_ALIGN_PTR(
                        ರಚನೆ hashmap_ತಳ_ನಮೂದು,
                        (uint8_t *) storage_ptr(h) + idx * hashmap_type_info[h->type].ನಮೂದು_ಗಾತ್ರ);
}

ಸ್ಥಿರ ರಚನೆ plain_hashmap_ನಮೂದು* plain_ಪಾತ್ರೆ_at(Hashmap *h, ಚಿಹ್ನೆರಹಿತ idx) {
        ಹಿಂತಿರುಗಿ (ರಚನೆ plain_hashmap_ನಮೂದು*) ಪಾತ್ರೆ_at(HASHMAP_ತಳ(h), idx);
}

ಸ್ಥಿರ ರಚನೆ ordered_hashmap_ನಮೂದು* ordered_ಪಾತ್ರೆ_at(OrderedHashmap *h, ಚಿಹ್ನೆರಹಿತ idx) {
        ಹಿಂತಿರುಗಿ (ರಚನೆ ordered_hashmap_ನಮೂದು*) ಪಾತ್ರೆ_at(HASHMAP_ತಳ(h), idx);
}

ಸ್ಥಿರ ರಚನೆ ಗಣ_ನಮೂದು *ಗಣ_ಪಾತ್ರೆ_at(ಗಣ_2 *h, ಚಿಹ್ನೆರಹಿತ idx) {
        ಹಿಂತಿರುಗಿ (ರಚನೆ ಗಣ_ನಮೂದು*) ಪಾತ್ರೆ_at(HASHMAP_ತಳ(h), idx);
}

ಸ್ಥಿರ ರಚನೆ ordered_hashmap_ನಮೂದು* ಪಾತ್ರೆ_at_ಅದಲುಬದಲು(ರಚನೆ ಅದಲುಬದಲು_entries *ಅದಲುಬದಲು, ಚಿಹ್ನೆರಹಿತ idx) {
        ಪ್ರತಿಜ್ಞಾ(ಅದಲುಬದಲು);
        ಹಿಂತಿರುಗಿ &ಅದಲುಬದಲು->e[idx - IDX_ಅದಲುಬದಲು_BEGIN];
}

/* Returns a pointer to the bucket at index idx.
 * Understands real indexes and swap indexes, hence "_virtual". */
ಸ್ಥಿರ ರಚನೆ hashmap_ತಳ_ನಮೂದು* ಪಾತ್ರೆ_at_virtual(Hashmap_ತಳ *h, ರಚನೆ ಅದಲುಬದಲು_entries *ಅದಲುಬದಲು,
                                                    ಚಿಹ್ನೆರಹಿತ idx) {
        ಆದರೆ (idx < IDX_ಅದಲುಬದಲು_BEGIN)
                ಹಿಂತಿರುಗಿ ಪಾತ್ರೆ_at(h, idx);

        ಆದರೆ (idx < IDX_ಅದಲುಬದಲು_ಕೊನೆ)
                ಹಿಂತಿರುಗಿ &ಪಾತ್ರೆ_at_ಅದಲುಬದಲು(ಅದಲುಬದಲು, idx)->p.b;

        assert_not_reached();
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ dib_raw_t* dib_raw_ptr(Hashmap_ತಳ *h) {
        ಪ್ರತಿಜ್ಞಾ(h);
        ಹಿಂತಿರುಗಿ (dib_raw_t*)
                ((uint8_t*) storage_ptr(h) + hashmap_type_info[h->type].ನಮೂದು_ಗಾತ್ರ * n_buckets(h));
}

ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ಪಾತ್ರೆ_ದೂರ(Hashmap_ತಳ *h, ಚಿಹ್ನೆರಹಿತ idx, ಚಿಹ್ನೆರಹಿತ from) {
        ಹಿಂತಿರುಗಿ idx >= from ? idx - from
                           : n_buckets(h) + idx - from;
}

ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ಪಾತ್ರೆ_calculate_dib(Hashmap_ತಳ *h, ಚಿಹ್ನೆರಹಿತ idx, dib_raw_t raw_dib) {
        ಚಿಹ್ನೆರಹಿತ initial_ಪಾತ್ರೆ;

        ಆದರೆ (raw_dib == DIB_RAW_FREE)
                ಹಿಂತಿರುಗಿ DIB_FREE;

        ಆದರೆ (_likely_(raw_dib < DIB_RAW_ಅತಿಪ್ರವಾಹ))
                ಹಿಂತಿರುಗಿ raw_dib;

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
        initial_ಪಾತ್ರೆ = ಪಾತ್ರೆ_hash(h, ಪಾತ್ರೆ_at(h, idx)->ಕೀಲಿ);
        ಹಿಂತಿರುಗಿ ಪಾತ್ರೆ_ದೂರ(h, idx, initial_ಪಾತ್ರೆ);
}

ಸ್ಥಿರ ಶೂನ್ಯ ಪಾತ್ರೆ_ಗಣ_dib(Hashmap_ತಳ *h, ಚಿಹ್ನೆರಹಿತ idx, ಚಿಹ್ನೆರಹಿತ dib) {
        dib_raw_ptr(h)[idx] = dib != DIB_FREE ? MIN(dib, DIB_RAW_ಅತಿಪ್ರವಾಹ) : DIB_RAW_FREE;
}

ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ skip_free_buckets(Hashmap_ತಳ *h, ಚಿಹ್ನೆರಹಿತ idx) {
        dib_raw_t *dibs;

        dibs = dib_raw_ptr(h);

        ಗಾಗಿ ( ; idx < n_buckets(h); idx++)
                ಆದರೆ (dibs[idx] != DIB_RAW_FREE)
                        ಹಿಂತಿರುಗಿ idx;

        ಹಿಂತಿರುಗಿ IDX_NIL;
}

ಸ್ಥಿರ ಶೂನ್ಯ ಪಾತ್ರೆ_mark_free(Hashmap_ತಳ *h, ಚಿಹ್ನೆರಹಿತ idx) {
        ಪ್ರತಿಜ್ಞಾ(h);

        memzero(ಪಾತ್ರೆ_at(h, idx), hashmap_type_info[h->type].ನಮೂದು_ಗಾತ್ರ);
        ಪಾತ್ರೆ_ಗಣ_dib(h, idx, DIB_FREE);
}

ಸ್ಥಿರ ಶೂನ್ಯ ಪಾತ್ರೆ_move_ನಮೂದು(Hashmap_ತಳ *h, ರಚನೆ ಅದಲುಬದಲು_entries *ಅದಲುಬದಲು,
                              ಚಿಹ್ನೆರಹಿತ from, ಚಿಹ್ನೆರಹಿತ to) {
        ರಚನೆ hashmap_ತಳ_ನಮೂದು *e_from, *e_ಗಮ್ಯ;

        ಪ್ರತಿಜ್ಞಾ(h);
        ಪ್ರತಿಜ್ಞಾ(from != to);

        e_from = ಪಾತ್ರೆ_at_virtual(h, ಅದಲುಬದಲು, from);
        e_ಗಮ್ಯ   = ಪಾತ್ರೆ_at_virtual(h, ಅದಲುಬದಲು, to);

        ಸ್ಮರಣೆ_ನಕಲು(e_ಗಮ್ಯ, e_from, hashmap_type_info[h->type].ನಮೂದು_ಗಾತ್ರ);

        ಆದರೆ (h->type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*) h;
                ರಚನೆ ordered_hashmap_ನಮೂದು *le, *le_ಗಮ್ಯ;

                le_ಗಮ್ಯ = (ರಚನೆ ordered_hashmap_ನಮೂದು*) e_ಗಮ್ಯ;

                ಆದರೆ (le_ಗಮ್ಯ->iterate_ಮುಂದಿನ != IDX_NIL) {
                        le = (ರಚನೆ ordered_hashmap_ನಮೂದು*)
                             ಪಾತ್ರೆ_at_virtual(h, ಅದಲುಬದಲು, le_ಗಮ್ಯ->iterate_ಮುಂದಿನ);
                        le->iterate_ಹಿಂದಿನ = to;
                }

                ಆದರೆ (le_ಗಮ್ಯ->iterate_ಹಿಂದಿನ != IDX_NIL) {
                        le = (ರಚನೆ ordered_hashmap_ನಮೂದು*)
                             ಪಾತ್ರೆ_at_virtual(h, ಅದಲುಬದಲು, le_ಗಮ್ಯ->iterate_ಹಿಂದಿನ);
                        le->iterate_ಮುಂದಿನ = to;
                }

                ಆದರೆ (lh->iterate_list_ತಲೆ == from)
                        lh->iterate_list_ತಲೆ = to;
                ಆದರೆ (lh->iterate_list_ಬಾಲ == from)
                        lh->iterate_list_ಬಾಲ = to;
        }
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಚಿಹ್ನೆರಹಿತ ಮುಂದಿನ_idx(Hashmap_ತಳ *h, ಚಿಹ್ನೆರಹಿತ idx) {
        ಹಿಂತಿರುಗಿ idx + 1U < n_buckets(h) ? idx + 1U : 0U;
}

ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ prev_idx(Hashmap_ತಳ *h, ಚಿಹ್ನೆರಹಿತ idx) {
        ಹಿಂತಿರುಗಿ idx > 0U ? idx - 1U : n_buckets(h) - 1U;
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ* ನಮೂದು_ಮೌಲ್ಯ(Hashmap_ತಳ *h, ರಚನೆ hashmap_ತಳ_ನಮೂದು *e) {
        ಪ್ರತಿಜ್ಞಾ(h);
        ಪ್ರತಿಜ್ಞಾ(e);

        ಆಯ್ಕೆ (h->type) {

        ಸ್ಥಿತಿ HASHMAP_TYPE_PLAIN:
        ಸ್ಥಿತಿ HASHMAP_TYPE_ORDERED:
                ಹಿಂತಿರುಗಿ ((ರಚನೆ plain_hashmap_ನಮೂದು*)e)->ಮೌಲ್ಯ;

        ಸ್ಥಿತಿ HASHMAP_TYPE_ಗಣ:
                ಹಿಂತಿರುಗಿ (ಶೂನ್ಯ*) e->ಕೀಲಿ;

        ಪೂರ್ವನಿಯೋಜಿತ:
                assert_not_reached();
        }
}

ಸ್ಥಿರ ಶೂನ್ಯ ತಳ_remove_ನಮೂದು(Hashmap_ತಳ *h, ಚಿಹ್ನೆರಹಿತ idx) {
        ಚಿಹ್ನೆರಹಿತ ಎಡ, ಬಲ, prev, dib;
        dib_raw_t raw_dib, *dibs;

        dibs = dib_raw_ptr(h);
        ಪ್ರತಿಜ್ಞಾ(dibs[idx] != DIB_RAW_FREE);

#ಆದರೆ ENABLE_DEBUG_HASHMAP
        ಪ್ರತಿಜ್ಞಾ(h);
        h->debug.rem_ಎಣಿಕೆ++;
        h->debug.ಕೊನೆಯ_rem_idx = idx;
#ಆದರೆ_ಅಂತ್ಯ

        ಎಡ = idx;
        /* Find the stop bucket ("right"). It is either free or has DIB == 0. */
        ಗಾಗಿ (ಬಲ = ಮುಂದಿನ_idx(h, ಎಡ); ; ಬಲ = ಮುಂದಿನ_idx(h, ಬಲ)) {
                raw_dib = dibs[ಬಲ];
                ಆದರೆ (ಒಳ_ಗಣ(raw_dib, 0, DIB_RAW_FREE))
                        ನಿಲ್ಲಿಸು;

                /* The buckets are not supposed to be all occupied and with DIB > 0.
                 * That would mean we could make everyone better off by shifting them
                 * backward. This scenario is impossible. */
                ಪ್ರತಿಜ್ಞಾ(ಎಡ != ಬಲ);
        }

        ಆದರೆ (h->type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*) h;
                ರಚನೆ ordered_hashmap_ನಮೂದು *le = ordered_ಪಾತ್ರೆ_at(lh, idx);

                ಆದರೆ (le->iterate_ಮುಂದಿನ != IDX_NIL)
                        ordered_ಪಾತ್ರೆ_at(lh, le->iterate_ಮುಂದಿನ)->iterate_ಹಿಂದಿನ = le->iterate_ಹಿಂದಿನ;
                ಇಲ್ಲದಿದ್ದರೆ
                        lh->iterate_list_ಬಾಲ = le->iterate_ಹಿಂದಿನ;

                ಆದರೆ (le->iterate_ಹಿಂದಿನ != IDX_NIL)
                        ordered_ಪಾತ್ರೆ_at(lh, le->iterate_ಹಿಂದಿನ)->iterate_ಮುಂದಿನ = le->iterate_ಮುಂದಿನ;
                ಇಲ್ಲದಿದ್ದರೆ
                        lh->iterate_list_ತಲೆ = le->iterate_ಮುಂದಿನ;
        }

        /* Now shift all buckets in the interval (left, right) one step backwards */
        ಗಾಗಿ (prev = ಎಡ, ಎಡ = ಮುಂದಿನ_idx(h, ಎಡ); ಎಡ != ಬಲ;
             prev = ಎಡ, ಎಡ = ಮುಂದಿನ_idx(h, ಎಡ)) {
                dib = ಪಾತ್ರೆ_calculate_dib(h, ಎಡ, dibs[ಎಡ]);
                ಪ್ರತಿಜ್ಞಾ(dib != 0);
                ಪಾತ್ರೆ_move_ನಮೂದು(h, ಖಾಲಿ, ಎಡ, prev);
                ಪಾತ್ರೆ_ಗಣ_dib(h, prev, dib - 1);
        }

        ಪಾತ್ರೆ_mark_free(h, prev);
        n_entries_dec(h);
        ತಳ_ಗಣ_dirty(h);
}
#ವ್ಯಾಖ್ಯಾನಿಸು remove_ನಮೂದು(h, idx) ತಳ_remove_ನಮೂದು(HASHMAP_ತಳ(h), idx)

ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ hashmap_iterate_ಒಳ_insertion_ಕ್ರಮ(OrderedHashmap *h, Iterator *i) {
        ರಚನೆ ordered_hashmap_ನಮೂದು *e;
        ಚಿಹ್ನೆರಹಿತ idx;

        ಪ್ರತಿಜ್ಞಾ(h);
        ಪ್ರತಿಜ್ಞಾ(i);

        ಆದರೆ (i->idx == IDX_NIL)
                ಹೋಗು at_ಕೊನೆ;

        ಆದರೆ (i->idx == IDX_ಮೊದಲ && h->iterate_list_ತಲೆ == IDX_NIL)
                ಹೋಗು at_ಕೊನೆ;

        ಆದರೆ (i->idx == IDX_ಮೊದಲ) {
                idx = h->iterate_list_ತಲೆ;
                e = ordered_ಪಾತ್ರೆ_at(h, idx);
        } ಇಲ್ಲದಿದ್ದರೆ {
                idx = i->idx;
                e = ordered_ಪಾತ್ರೆ_at(h, idx);
                /*
                 * We allow removing the current entry while iterating, but removal may cause
                 * a backward shift. The next entry may thus move one bucket to the left.
                 * To detect when it happens, we remember the key pointer of the entry we were
                 * going to iterate next. If it does not match, there was a backward shift.
                 */
                ಆದರೆ (e->p.b.ಕೀಲಿ != i->ಮುಂದಿನ_ಕೀಲಿ) {
                        idx = prev_idx(HASHMAP_ತಳ(h), idx);
                        e = ordered_ಪಾತ್ರೆ_at(h, idx);
                }
                ಪ್ರತಿಜ್ಞಾ(e->p.b.ಕೀಲಿ == i->ಮುಂದಿನ_ಕೀಲಿ);
        }

#ಆದರೆ ENABLE_DEBUG_HASHMAP
        i->prev_idx = idx;
#ಆದರೆ_ಅಂತ್ಯ

        ಆದರೆ (e->iterate_ಮುಂದಿನ != IDX_NIL) {
                ರಚನೆ ordered_hashmap_ನಮೂದು *n;
                i->idx = e->iterate_ಮುಂದಿನ;
                n = ordered_ಪಾತ್ರೆ_at(h, i->idx);
                i->ಮುಂದಿನ_ಕೀಲಿ = n->p.b.ಕೀಲಿ;
        } ಇಲ್ಲದಿದ್ದರೆ
                i->idx = IDX_NIL;

        ಹಿಂತಿರುಗಿ idx;

at_ಕೊನೆ:
        i->idx = IDX_NIL;
        ಹಿಂತಿರುಗಿ IDX_NIL;
}

ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ hashmap_iterate_ಒಳ_ಆಂತರಿಕ_ಕ್ರಮ(Hashmap_ತಳ *h, Iterator *i) {
        ಚಿಹ್ನೆರಹಿತ idx;

        ಪ್ರತಿಜ್ಞಾ(h);
        ಪ್ರತಿಜ್ಞಾ(i);

        ಆದರೆ (i->idx == IDX_NIL)
                ಹೋಗು at_ಕೊನೆ;

        ಆದರೆ (i->idx == IDX_ಮೊದಲ) {
                /* fast forward to the first occupied bucket */
                ಆದರೆ (h->has_ಪರೋಕ್ಷ) {
                        i->idx = skip_free_buckets(h, h->ಪರೋಕ್ಷ.idx_ಅತಿಕಡಿಮೆ_ನಮೂದು);
                        h->ಪರೋಕ್ಷ.idx_ಅತಿಕಡಿಮೆ_ನಮೂದು = i->idx;
                } ಇಲ್ಲದಿದ್ದರೆ
                        i->idx = skip_free_buckets(h, 0);

                ಆದರೆ (i->idx == IDX_NIL)
                        ಹೋಗು at_ಕೊನೆ;
        } ಇಲ್ಲದಿದ್ದರೆ {
                ರಚನೆ hashmap_ತಳ_ನಮೂದು *e;

                ಪ್ರತಿಜ್ಞಾ(i->idx > 0);

                e = ಪಾತ್ರೆ_at(h, i->idx);
                /*
                 * We allow removing the current entry while iterating, but removal may cause
                 * a backward shift. The next entry may thus move one bucket to the left.
                 * To detect when it happens, we remember the key pointer of the entry we were
                 * going to iterate next. If it does not match, there was a backward shift.
                 */
                ಆದರೆ (e->ಕೀಲಿ != i->ಮುಂದಿನ_ಕೀಲಿ)
                        e = ಪಾತ್ರೆ_at(h, --i->idx);

                ಪ್ರತಿಜ್ಞಾ(e->ಕೀಲಿ == i->ಮುಂದಿನ_ಕೀಲಿ);
        }

        idx = i->idx;
#ಆದರೆ ENABLE_DEBUG_HASHMAP
        i->prev_idx = idx;
#ಆದರೆ_ಅಂತ್ಯ

        i->idx = skip_free_buckets(h, i->idx + 1);
        ಆದರೆ (i->idx != IDX_NIL)
                i->ಮುಂದಿನ_ಕೀಲಿ = ಪಾತ್ರೆ_at(h, i->idx)->ಕೀಲಿ;
        ಇಲ್ಲದಿದ್ದರೆ
                i->idx = IDX_NIL;

        ಹಿಂತಿರುಗಿ idx;

at_ಕೊನೆ:
        i->idx = IDX_NIL;
        ಹಿಂತಿರುಗಿ IDX_NIL;
}

ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ hashmap_iterate_ನಮೂದು(Hashmap_ತಳ *h, Iterator *i) {
        ಆದರೆ (!h) {
                i->idx = IDX_NIL;
                ಹಿಂತಿರುಗಿ IDX_NIL;
        }

#ಆದರೆ ENABLE_DEBUG_HASHMAP
        ಆದರೆ (i->idx == IDX_ಮೊದಲ) {
                i->put_ಎಣಿಕೆ = h->debug.put_ಎಣಿಕೆ;
                i->rem_ಎಣಿಕೆ = h->debug.rem_ಎಣಿಕೆ;
        } ಇಲ್ಲದಿದ್ದರೆ {
                /* While iterating, must not add any new entries */
                ಪ್ರತಿಜ್ಞಾ(i->put_ಎಣಿಕೆ == h->debug.put_ಎಣಿಕೆ);
                /* ... or remove entries other than the current one */
                ಪ್ರತಿಜ್ಞಾ(i->rem_ಎಣಿಕೆ == h->debug.rem_ಎಣಿಕೆ ||
                       (i->rem_ಎಣಿಕೆ == h->debug.rem_ಎಣಿಕೆ - 1 &&
                        i->prev_idx == h->debug.ಕೊನೆಯ_rem_idx));
                /* Reset our removals counter */
                i->rem_ಎಣಿಕೆ = h->debug.rem_ಎಣಿಕೆ;
        }
#ಆದರೆ_ಅಂತ್ಯ

        ಹಿಂತಿರುಗಿ h->type == HASHMAP_TYPE_ORDERED ? hashmap_iterate_ಒಳ_insertion_ಕ್ರಮ((OrderedHashmap*) h, i)
                                               : hashmap_iterate_ಒಳ_ಆಂತರಿಕ_ಕ್ರಮ(h, i);
}

bool _hashmap_iterate(Hashmap_ತಳ *h, Iterator *i, ಶೂನ್ಯ **ಮೌಲ್ಯ, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ **ಕೀಲಿ) {
        ರಚನೆ hashmap_ತಳ_ನಮೂದು *e;
        ಶೂನ್ಯ *ದತ್ತಾಂಶ;
        ಚಿಹ್ನೆರಹಿತ idx;

        idx = hashmap_iterate_ನಮೂದು(h, i);
        ಆದರೆ (idx == IDX_NIL) {
                ಆದರೆ (ಮೌಲ್ಯ)
                        *ಮೌಲ್ಯ = ಖಾಲಿ;
                ಆದರೆ (ಕೀಲಿ)
                        *ಕೀಲಿ = ಖಾಲಿ;

                ಹಿಂತಿರುಗಿ false;
        }

        e = ಪಾತ್ರೆ_at(h, idx);
        ದತ್ತಾಂಶ = ನಮೂದು_ಮೌಲ್ಯ(h, e);
        ಆದರೆ (ಮೌಲ್ಯ)
                *ಮೌಲ್ಯ = ದತ್ತಾಂಶ;
        ಆದರೆ (ಕೀಲಿ)
                *ಕೀಲಿ = e->ಕೀಲಿ;

        ಹಿಂತಿರುಗಿ true;
}

#ವ್ಯಾಖ್ಯಾನಿಸು HASHMAP_FOREACH_IDX(idx, h, i) \
        ಗಾಗಿ ((i) = ITERATOR_ಮೊದಲ, (idx) = hashmap_iterate_ನಮೂದು((h), &(i)); \
             (idx != IDX_NIL); \
             (idx) = hashmap_iterate_ನಮೂದು((h), &(i)))

IteratedCache* _hashmap_iterated_cache_new(Hashmap_ತಳ *h) {
        IteratedCache *cache;

        ಪ್ರತಿಜ್ಞಾ(h);
        ಪ್ರತಿಜ್ಞಾ(!h->cached);

        ಆದರೆ (h->cached)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        cache = new0(IteratedCache, 1);
        ಆದರೆ (!cache)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        cache->hashmap = h;
        h->cached = true;

        ಹಿಂತಿರುಗಿ cache;
}

ಸ್ಥಿರ ಶೂನ್ಯ reset_ನೇರ_storage(Hashmap_ತಳ *h) {
        ಸ್ಥಿರಾಂಕ ರಚನೆ hashmap_type_info *hi = &hashmap_type_info[h->type];
        ಶೂನ್ಯ *p;

        ಪ್ರತಿಜ್ಞಾ(!h->has_ಪರೋಕ್ಷ);

        p = mempset(h->ನೇರ.storage, 0, hi->ನಮೂದು_ಗಾತ್ರ * hi->n_ನೇರ_buckets);
        memset(p, DIB_RAW_INIT, ಗಾತ್ರ(dib_raw_t) * hi->n_ನೇರ_buckets);
}

ಸ್ಥಿರ ಶೂನ್ಯ shared_hash_ಕೀಲಿ_initialize(ಶೂನ್ಯ) {
        random_ಬೈಟುಗಳು(shared_hash_ಕೀಲಿ, ಗಾತ್ರ(shared_hash_ಕೀಲಿ));
}

ಸ್ಥಿರ ರಚನೆ Hashmap_ತಳ* hashmap_ತಳ_new(ಸ್ಥಿರಾಂಕ ರಚನೆ hash_ops *hash_ops, ಎಣಿಕೆ HashmapType type) {
        Hashmap_ತಳ *h;
        ಸ್ಥಿರಾಂಕ ರಚನೆ hashmap_type_info *hi = &hashmap_type_info[type];

        bool use_pool = mempool_enabled && mempool_enabled();  /* mempool_enabled is a weak symbol */

        h = use_pool ? mempool_alloc0_tile(hi->mempool) : malloc0(hi->ತಲೆ_ಗಾತ್ರ);
        ಆದರೆ (!h)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        h->type = type;
        h->from_pool = use_pool;
        h->hash_ops = hash_ops ?: &trivial_hash_ops;

        ಆದರೆ (type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*)h;
                lh->iterate_list_ತಲೆ = lh->iterate_list_ಬಾಲ = IDX_NIL;
        }

        reset_ನೇರ_storage(h);

        ಸ್ಥಿರ pthread_once_t once = PTHREAD_ONCE_INIT;
        assert_se(pthread_once(&once, shared_hash_ಕೀಲಿ_initialize) == 0);

#ಆದರೆ ENABLE_DEBUG_HASHMAP
        assert_se(pthread_mutex_lock(&hashmap_debug_list_mutex) == 0);
        LIST_PREPEND(debug_list, hashmap_debug_list, &h->debug);
        assert_se(pthread_mutex_unlock(&hashmap_debug_list_mutex) == 0);
#ಆದರೆ_ಅಂತ್ಯ

        ಹಿಂತಿರುಗಿ h;
}

Hashmap *hashmap_new(ಸ್ಥಿರಾಂಕ ರಚನೆ hash_ops *hash_ops) {
        ಹಿಂತಿರುಗಿ (Hashmap*)        hashmap_ತಳ_new(hash_ops, HASHMAP_TYPE_PLAIN);
}

OrderedHashmap *ordered_hashmap_new(ಸ್ಥಿರಾಂಕ ರಚನೆ hash_ops *hash_ops) {
        ಹಿಂತಿರುಗಿ (OrderedHashmap*) hashmap_ತಳ_new(hash_ops, HASHMAP_TYPE_ORDERED);
}

ಗಣ_2 *ಗಣ_new(ಸ್ಥಿರಾಂಕ ರಚನೆ hash_ops *hash_ops) {
        ಹಿಂತಿರುಗಿ (ಗಣ_2*)            hashmap_ತಳ_new(hash_ops, HASHMAP_TYPE_ಗಣ);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ hashmap_ತಳ_ensure_allocated(Hashmap_ತಳ **h, ಸ್ಥಿರಾಂಕ ರಚನೆ hash_ops *hash_ops,
                                         ಎಣಿಕೆ HashmapType type) {
        Hashmap_ತಳ *q;

        ಪ್ರತಿಜ್ಞಾ(h);

        ಆದರೆ (*h) {
                ಪ್ರತಿಜ್ಞಾ((*h)->hash_ops == (hash_ops ?: &trivial_hash_ops));
                ಹಿಂತಿರುಗಿ 0;
        }

        q = hashmap_ತಳ_new(hash_ops, type);
        ಆದರೆ (!q)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        *h = q;
        ಹಿಂತಿರುಗಿ 1;
}

ಪೂರ್ಣಾಂಕ hashmap_ensure_allocated(Hashmap **h, ಸ್ಥಿರಾಂಕ ರಚನೆ hash_ops *hash_ops) {
        ಹಿಂತಿರುಗಿ hashmap_ತಳ_ensure_allocated((Hashmap_ತಳ**)h, hash_ops, HASHMAP_TYPE_PLAIN);
}

ಪೂರ್ಣಾಂಕ ordered_hashmap_ensure_allocated(OrderedHashmap **h, ಸ್ಥಿರಾಂಕ ರಚನೆ hash_ops *hash_ops) {
        ಹಿಂತಿರುಗಿ hashmap_ತಳ_ensure_allocated((Hashmap_ತಳ**)h, hash_ops, HASHMAP_TYPE_ORDERED);
}

ಪೂರ್ಣಾಂಕ ಗಣ_ensure_allocated(ಗಣ_2 **s, ಸ್ಥಿರಾಂಕ ರಚನೆ hash_ops *hash_ops) {
        ಹಿಂತಿರುಗಿ hashmap_ತಳ_ensure_allocated((Hashmap_ತಳ**)s, hash_ops, HASHMAP_TYPE_ಗಣ);
}

ಪೂರ್ಣಾಂಕ hashmap_ensure_put(Hashmap **h, ಸ್ಥಿರಾಂಕ ರಚನೆ hash_ops *hash_ops, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ, ಶೂನ್ಯ *ಮೌಲ್ಯ) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(h);

        r = hashmap_ensure_allocated(h, hash_ops);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ hashmap_put(*h, ಕೀಲಿ, ಮೌಲ್ಯ);
}

ಪೂರ್ಣಾಂಕ ordered_hashmap_ensure_put(OrderedHashmap **h, ಸ್ಥಿರಾಂಕ ರಚನೆ hash_ops *hash_ops, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ, ಶೂನ್ಯ *ಮೌಲ್ಯ) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(h);

        r = ordered_hashmap_ensure_allocated(h, hash_ops);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ ordered_hashmap_put(*h, ಕೀಲಿ, ಮೌಲ್ಯ);
}

ಪೂರ್ಣಾಂಕ ordered_hashmap_ensure_replace(OrderedHashmap **h, ಸ್ಥಿರಾಂಕ ರಚನೆ hash_ops *hash_ops, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ, ಶೂನ್ಯ *ಮೌಲ್ಯ) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(h);

        r = ordered_hashmap_ensure_allocated(h, hash_ops);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ ordered_hashmap_replace(*h, ಕೀಲಿ, ಮೌಲ್ಯ);
}

ಪೂರ್ಣಾಂಕ hashmap_ensure_replace(Hashmap **h, ಸ್ಥಿರಾಂಕ ರಚನೆ hash_ops *hash_ops, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ, ಶೂನ್ಯ *ಮೌಲ್ಯ) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(h);

        r = hashmap_ensure_allocated(h, hash_ops);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ hashmap_replace(*h, ಕೀಲಿ, ಮೌಲ್ಯ);
}

ಸ್ಥಿರ ಶೂನ್ಯ hashmap_free_no_clear(Hashmap_ತಳ *h) {
        ಪ್ರತಿಜ್ಞಾ(!h->has_ಪರೋಕ್ಷ);
        ಪ್ರತಿಜ್ಞಾ(h->n_ನೇರ_entries == 0);

#ಆದರೆ ENABLE_DEBUG_HASHMAP
        assert_se(pthread_mutex_lock(&hashmap_debug_list_mutex) == 0);
        LIST_REMOVE(debug_list, hashmap_debug_list, &h->debug);
        assert_se(pthread_mutex_unlock(&hashmap_debug_list_mutex) == 0);
#ಆದರೆ_ಅಂತ್ಯ

        ಆದರೆ (h->from_pool) {
                /* Ensure that the object didn't get migrated between threads. */
                assert_se(is_main_thread());
                mempool_free_tile(hashmap_type_info[h->type].mempool, h);
        } ಇಲ್ಲದಿದ್ದರೆ
                ಬಿಡುಗಡೆ(h);
}

Hashmap_ತಳ* _hashmap_free(Hashmap_ತಳ *h) {
        ಆದರೆ (h) {
                _hashmap_clear(h);
                hashmap_free_no_clear(h);
        }

        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

ಶೂನ್ಯ _hashmap_clear(Hashmap_ತಳ *h) {
        ಆದರೆ (!h)
                ಹಿಂತಿರುಗಿ;

        ಆದರೆ (h->hash_ops->free_ಕೀಲಿ || h->hash_ops->free_ಮೌಲ್ಯ) {

                /* If destructor calls are defined, let's destroy things defensively: let's take the item out of the
                 * hash table, and only then call the destructor functions. If these destructors then try to unregister
                 * themselves from our hash table a second time, the entry is already gone. */

                ಆಗ (hashmap_ಗಾತ್ರ(h) > 0) {
                        ಶೂನ್ಯ *k = ಖಾಲಿ;
                        ಶೂನ್ಯ *v;

                        v = hashmap_ಮೊದಲ_ಕೀಲಿ_and_ಮೌಲ್ಯ(h, true, &k);

                        ಆದರೆ (h->hash_ops->free_ಕೀಲಿ)
                                h->hash_ops->free_ಕೀಲಿ(k);

                        ಆದರೆ (h->hash_ops->free_ಮೌಲ್ಯ)
                                h->hash_ops->free_ಮೌಲ್ಯ(v);
                }
        }

        ಆದರೆ (h->has_ಪರೋಕ್ಷ) {
                ಬಿಡುಗಡೆ(h->ಪರೋಕ್ಷ.storage);
                h->has_ಪರೋಕ್ಷ = false;
        }

        h->n_ನೇರ_entries = 0;
        reset_ನೇರ_storage(h);

        ಆದರೆ (h->type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*) h;
                lh->iterate_list_ತಲೆ = lh->iterate_list_ಬಾಲ = IDX_NIL;
        }

        ತಳ_ಗಣ_dirty(h);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ resize_buckets(Hashmap_ತಳ *h, ಚಿಹ್ನೆರಹಿತ entries_ಕೂಡಿಸು);

/*
 * Finds an empty bucket to put an entry into, starting the scan at 'idx'.
 * Performs Robin Hood swaps as it goes. The entry to put must be placed
 * by the caller into swap slot IDX_PUT.
 * If used for in-place resizing, may leave a displaced entry in swap slot
 * IDX_PUT. Caller must rehash it next.
 * Returns: true if it left a displaced entry to rehash next in IDX_PUT,
 *          false otherwise.
 */
ಸ್ಥಿರ bool hashmap_put_robin_hood(Hashmap_ತಳ *h, ಚಿಹ್ನೆರಹಿತ idx,
                                   ರಚನೆ ಅದಲುಬದಲು_entries *ಅದಲುಬದಲು) {
        dib_raw_t raw_dib, *dibs;
        ಚಿಹ್ನೆರಹಿತ dib, ದೂರ;

#ಆದರೆ ENABLE_DEBUG_HASHMAP
        ಪ್ರತಿಜ್ಞಾ(h);
        h->debug.put_ಎಣಿಕೆ++;
#ಆದರೆ_ಅಂತ್ಯ

        dibs = dib_raw_ptr(h);

        ಗಾಗಿ (ದೂರ = 0; ; ದೂರ++) {
                raw_dib = dibs[idx];
                ಆದರೆ (ಒಳ_ಗಣ(raw_dib, DIB_RAW_FREE, DIB_RAW_REHASH)) {
                        ಆದರೆ (raw_dib == DIB_RAW_REHASH)
                                ಪಾತ್ರೆ_move_ನಮೂದು(h, ಅದಲುಬದಲು, idx, IDX_TMP);

                        ಆದರೆ (h->has_ಪರೋಕ್ಷ && h->ಪರೋಕ್ಷ.idx_ಅತಿಕಡಿಮೆ_ನಮೂದು > idx)
                                h->ಪರೋಕ್ಷ.idx_ಅತಿಕಡಿಮೆ_ನಮೂದು = idx;

                        ಪಾತ್ರೆ_ಗಣ_dib(h, idx, ದೂರ);
                        ಪಾತ್ರೆ_move_ನಮೂದು(h, ಅದಲುಬದಲು, IDX_PUT, idx);
                        ಆದರೆ (raw_dib == DIB_RAW_REHASH) {
                                ಪಾತ್ರೆ_move_ನಮೂದು(h, ಅದಲುಬದಲು, IDX_TMP, IDX_PUT);
                                ಹಿಂತಿರುಗಿ true;
                        }

                        ಹಿಂತಿರುಗಿ false;
                }

                dib = ಪಾತ್ರೆ_calculate_dib(h, idx, raw_dib);

                ಆದರೆ (dib < ದೂರ) {
                        /* Found a wealthier entry. Go Robin Hood! */
                        ಪಾತ್ರೆ_ಗಣ_dib(h, idx, ದೂರ);

                        /* swap the entries */
                        ಪಾತ್ರೆ_move_ನಮೂದು(h, ಅದಲುಬದಲು, idx, IDX_TMP);
                        ಪಾತ್ರೆ_move_ನಮೂದು(h, ಅದಲುಬದಲು, IDX_PUT, idx);
                        ಪಾತ್ರೆ_move_ನಮೂದು(h, ಅದಲುಬದಲು, IDX_TMP, IDX_PUT);

                        ದೂರ = dib;
                }

                idx = ಮುಂದಿನ_idx(h, idx);
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
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ hashmap_ತಳ_put_boldly(Hashmap_ತಳ *h, ಚಿಹ್ನೆರಹಿತ idx,
                                   ರಚನೆ ಅದಲುಬದಲು_entries *ಅದಲುಬದಲು, bool may_resize) {
        ರಚನೆ ordered_hashmap_ನಮೂದು *new_ನಮೂದು;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(idx < n_buckets(h));

        new_ನಮೂದು = ಪಾತ್ರೆ_at_ಅದಲುಬದಲು(ಅದಲುಬದಲು, IDX_PUT);

        ಆದರೆ (may_resize) {
                r = resize_buckets(h, 1);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
                ಆದರೆ (r > 0)
                        idx = ಪಾತ್ರೆ_hash(h, new_ನಮೂದು->p.b.ಕೀಲಿ);
        }
        ಪ್ರತಿಜ್ಞಾ(n_entries(h) < n_buckets(h));

        ಆದರೆ (h->type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*) h;

                new_ನಮೂದು->iterate_ಮುಂದಿನ = IDX_NIL;
                new_ನಮೂದು->iterate_ಹಿಂದಿನ = lh->iterate_list_ಬಾಲ;

                ಆದರೆ (lh->iterate_list_ಬಾಲ != IDX_NIL) {
                        ರಚನೆ ordered_hashmap_ನಮೂದು *old_ಬಾಲ;

                        old_ಬಾಲ = ordered_ಪಾತ್ರೆ_at(lh, lh->iterate_list_ಬಾಲ);
                        ಪ್ರತಿಜ್ಞಾ(old_ಬಾಲ->iterate_ಮುಂದಿನ == IDX_NIL);
                        old_ಬಾಲ->iterate_ಮುಂದಿನ = IDX_PUT;
                }

                lh->iterate_list_ಬಾಲ = IDX_PUT;
                ಆದರೆ (lh->iterate_list_ತಲೆ == IDX_NIL)
                        lh->iterate_list_ತಲೆ = IDX_PUT;
        }

        assert_se(hashmap_put_robin_hood(h, idx, ಅದಲುಬದಲು) == false);

        n_entries_inc(h);
#ಆದರೆ ENABLE_DEBUG_HASHMAP
        h->debug.max_entries = MAX(h->debug.max_entries, n_entries(h));
#ಆದರೆ_ಅಂತ್ಯ

        ತಳ_ಗಣ_dirty(h);

        ಹಿಂತಿರುಗಿ 1;
}
#ವ್ಯಾಖ್ಯಾನಿಸು hashmap_put_boldly(h, idx, ಅದಲುಬದಲು, may_resize) \
        hashmap_ತಳ_put_boldly(HASHMAP_ತಳ(h), idx, ಅದಲುಬದಲು, may_resize)

/*
 * Returns 0 if resize is not needed.
 *         1 if successfully resized.
 *         -ENOMEM on allocation failure.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ resize_buckets(Hashmap_ತಳ *h, ಚಿಹ್ನೆರಹಿತ entries_ಕೂಡಿಸು) {
        ರಚನೆ ಅದಲುಬದಲು_entries ಅದಲುಬದಲು;
        ಶೂನ್ಯ *new_storage;
        dib_raw_t *old_dibs, *new_dibs;
        ಸ್ಥಿರಾಂಕ ರಚನೆ hashmap_type_info *hi;
        ಚಿಹ್ನೆರಹಿತ idx, optimal_idx;
        ಚಿಹ್ನೆರಹಿತ old_n_buckets, new_n_buckets, n_rehashed, new_n_entries;
        uint8_t new_ಸರಿಸು;
        bool rehash_ಮುಂದಿನ;

        ಪ್ರತಿಜ್ಞಾ(h);

        hi = &hashmap_type_info[h->type];
        new_n_entries = n_entries(h) + entries_ಕೂಡಿಸು;

        /* overflow? */
        ಆದರೆ (_unlikely_(new_n_entries < entries_ಕೂಡಿಸು))
                ಹಿಂತಿರುಗಿ -ENOMEM;

        /* For direct storage we allow 100% load, because it's tiny. */
        ಆದರೆ (!h->has_ಪರೋಕ್ಷ && new_n_entries <= hi->n_ನೇರ_buckets)
                ಹಿಂತಿರುಗಿ 0;

        /*
         * Load factor = n/m = 1 - (1/INV_KEEP_FREE).
         * From it follows: m = n + n/(INV_KEEP_FREE - 1)
         */
        new_n_buckets = new_n_entries + new_n_entries / (INV_KEEP_FREE - 1);
        /* overflow? */
        ಆದರೆ (_unlikely_(new_n_buckets < new_n_entries))
                ಹಿಂತಿರುಗಿ -ENOMEM;

        old_n_buckets = n_buckets(h);

        ಆದರೆ (_likely_(new_n_buckets <= old_n_buckets))
                ಹಿಂತಿರುಗಿ 0;

        ಆದರೆ (_unlikely_(new_n_buckets > UINT_MAX / (hi->ನಮೂದು_ಗಾತ್ರ + ಗಾತ್ರ(dib_raw_t))))
                ಹಿಂತಿರುಗಿ -ENOMEM;

        new_ಸರಿಸು = log2u_ಸುತ್ತು_ಮೇಲೆ(MAX(
                        new_n_buckets * (hi->ನಮೂದು_ಗಾತ್ರ + ಗಾತ್ರ(dib_raw_t)),
                        2 * ಗಾತ್ರ(ರಚನೆ ನೇರ_storage)));

        /* Realloc storage (buckets and DIB array). */
        new_storage = realloc(h->has_ಪರೋಕ್ಷ ? h->ಪರೋಕ್ಷ.storage : ಖಾಲಿ,
                              1U << new_ಸರಿಸು);
        ಆದರೆ (!new_storage)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        /* Must upgrade direct to indirect storage. */
        ಆದರೆ (!h->has_ಪರೋಕ್ಷ) {
                ಸ್ಮರಣೆ_ನಕಲು(new_storage, h->ನೇರ.storage,
                       old_n_buckets * (hi->ನಮೂದು_ಗಾತ್ರ + ಗಾತ್ರ(dib_raw_t)));
                h->ಪರೋಕ್ಷ.n_entries = h->n_ನೇರ_entries;
                h->ಪರೋಕ್ಷ.idx_ಅತಿಕಡಿಮೆ_ನಮೂದು = 0;
                h->n_ನೇರ_entries = 0;
        }

        /* Get a new hash key. If we've just upgraded to indirect storage,
         * allow reusing a previously generated key. It's still a different key
         * from the shared one that we used for direct storage. */
        get_hash_ಕೀಲಿ(h->ಪರೋಕ್ಷ.hash_ಕೀಲಿ, !h->has_ಪರೋಕ್ಷ);

        h->has_ಪರೋಕ್ಷ = true;
        h->ಪರೋಕ್ಷ.storage = new_storage;
        h->ಪರೋಕ್ಷ.n_buckets = (1U << new_ಸರಿಸು) /
                                (hi->ನಮೂದು_ಗಾತ್ರ + ಗಾತ್ರ(dib_raw_t));

        old_dibs = (dib_raw_t*)((uint8_t*) new_storage + hi->ನಮೂದು_ಗಾತ್ರ * old_n_buckets);
        new_dibs = dib_raw_ptr(h);

        /*
         * Move the DIB array to the new place, replacing valid DIB values with
         * DIB_RAW_REHASH to indicate all of the used buckets need rehashing.
         * Note: Overlap is not possible, because we have at least doubled the
         * number of buckets and dib_raw_t is smaller than any entry type.
         */
        ಗಾಗಿ (idx = 0; idx < old_n_buckets; idx++) {
                ಪ್ರತಿಜ್ಞಾ(old_dibs[idx] != DIB_RAW_REHASH);
                new_dibs[idx] = old_dibs[idx] == DIB_RAW_FREE ? DIB_RAW_FREE
                                                              : DIB_RAW_REHASH;
        }

        /* Zero the area of newly added entries (including the old DIB area) */
        memzero(ಪಾತ್ರೆ_at(h, old_n_buckets),
               (n_buckets(h) - old_n_buckets) * hi->ನಮೂದು_ಗಾತ್ರ);

        /* The upper half of the new DIB array needs initialization */
        memset(&new_dibs[old_n_buckets], DIB_RAW_INIT,
               (n_buckets(h) - old_n_buckets) * ಗಾತ್ರ(dib_raw_t));

        /* Rehash entries that need it */
        n_rehashed = 0;
        ಗಾಗಿ (idx = 0; idx < old_n_buckets; idx++) {
                ಆದರೆ (new_dibs[idx] != DIB_RAW_REHASH)
                        ಮುಂದುವರಿ;

                optimal_idx = ಪಾತ್ರೆ_hash(h, ಪಾತ್ರೆ_at(h, idx)->ಕೀಲಿ);

                /*
                 * Not much to do if by luck the entry hashes to its current
                 * location. Just set its DIB.
                 */
                ಆದರೆ (optimal_idx == idx) {
                        new_dibs[idx] = 0;
                        n_rehashed++;
                        ಮುಂದುವರಿ;
                }

                new_dibs[idx] = DIB_RAW_FREE;
                ಪಾತ್ರೆ_move_ನಮೂದು(h, &ಅದಲುಬದಲು, idx, IDX_PUT);
                /* bucket_move_entry does not clear the source */
                memzero(ಪಾತ್ರೆ_at(h, idx), hi->ನಮೂದು_ಗಾತ್ರ);

                ಮಾಡು {
                        /*
                         * Find the new bucket for the current entry. This may make
                         * another entry homeless and load it into IDX_PUT.
                         */
                        rehash_ಮುಂದಿನ = hashmap_put_robin_hood(h, optimal_idx, &ಅದಲುಬದಲು);
                        n_rehashed++;

                        /* Did the current entry displace another one? */
                        ಆದರೆ (rehash_ಮುಂದಿನ)
                                optimal_idx = ಪಾತ್ರೆ_hash(h, ಪಾತ್ರೆ_at_ಅದಲುಬದಲು(&ಅದಲುಬದಲು, IDX_PUT)->p.b.ಕೀಲಿ);
                } ಆಗ (rehash_ಮುಂದಿನ);
        }

        assert_se(n_rehashed == n_entries(h));

        ಹಿಂತಿರುಗಿ 1;
}

/*
 * Finds an entry with a matching key
 * Returns: index of the found entry, or IDX_NIL if not found.
 */
ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ತಳ_ಪಾತ್ರೆ_ಪರಿಶೀಲನೆ(Hashmap_ತಳ *h, ಚಿಹ್ನೆರಹಿತ idx, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ) {
        compare_func_t compare = h->hash_ops->compare;
        ರಚನೆ hashmap_ತಳ_ನಮೂದು *e;
        ಚಿಹ್ನೆರಹಿತ dib, ದೂರ;
        dib_raw_t *dibs = dib_raw_ptr(h);

        ಪ್ರತಿಜ್ಞಾ(idx < n_buckets(h));

        ಗಾಗಿ (ದೂರ = 0; ; ದೂರ++) {
                ಆದರೆ (dibs[idx] == DIB_RAW_FREE)
                        ಹಿಂತಿರುಗಿ IDX_NIL;

                dib = ಪಾತ್ರೆ_calculate_dib(h, idx, dibs[idx]);

                ಆದರೆ (dib < ದೂರ)
                        ಹಿಂತಿರುಗಿ IDX_NIL;
                ಆದರೆ (dib == ದೂರ) {
                        e = ಪಾತ್ರೆ_at(h, idx);

                        /* Most maps compare keys by pointer. Do that here, without the indirect call. */
                        ಆದರೆ (compare == trivial_compare_func ? e->ಕೀಲಿ == ಕೀಲಿ : compare(e->ಕೀಲಿ, ಕೀಲಿ) == 0)
                                ಹಿಂತಿರುಗಿ idx;
                }

                idx = ಮುಂದಿನ_idx(h, idx);
        }
}
#ವ್ಯಾಖ್ಯಾನಿಸು ಪಾತ್ರೆ_ಪರಿಶೀಲನೆ(h, idx, ಕೀಲಿ) ತಳ_ಪಾತ್ರೆ_ಪರಿಶೀಲನೆ(HASHMAP_ತಳ(h), idx, ಕೀಲಿ)

ಪೂರ್ಣಾಂಕ hashmap_put(Hashmap *h, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ, ಶೂನ್ಯ *ಮೌಲ್ಯ) {
        ರಚನೆ ಅದಲುಬದಲು_entries ಅದಲುಬದಲು;
        ರಚನೆ plain_hashmap_ನಮೂದು *e;
        ಚಿಹ್ನೆರಹಿತ hash, idx;

        ಪ್ರತಿಜ್ಞಾ(h);

        hash = ಪಾತ್ರೆ_hash(h, ಕೀಲಿ);
        idx = ಪಾತ್ರೆ_ಪರಿಶೀಲನೆ(h, hash, ಕೀಲಿ);
        ಆದರೆ (idx != IDX_NIL) {
                e = plain_ಪಾತ್ರೆ_at(h, idx);
                ಆದರೆ (e->ಮೌಲ್ಯ == ಮೌಲ್ಯ)
                        ಹಿಂತಿರುಗಿ 0;
                ಹಿಂತಿರುಗಿ -EEXIST;
        }

        e = &ಪಾತ್ರೆ_at_ಅದಲುಬದಲು(&ಅದಲುಬದಲು, IDX_PUT)->p;
        e->b.ಕೀಲಿ = ಕೀಲಿ;
        e->ಮೌಲ್ಯ = ಮೌಲ್ಯ;
        ಹಿಂತಿರುಗಿ hashmap_put_boldly(h, hash, &ಅದಲುಬದಲು, true);
}

ಪೂರ್ಣಾಂಕ ಗಣ_put(ಗಣ_2 *s, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ) {
        ರಚನೆ ಅದಲುಬದಲು_entries ಅದಲುಬದಲು;
        ರಚನೆ hashmap_ತಳ_ನಮೂದು *e;
        ಚಿಹ್ನೆರಹಿತ hash, idx;

        ಪ್ರತಿಜ್ಞಾ(s);

        hash = ಪಾತ್ರೆ_hash(s, ಕೀಲಿ);
        idx = ಪಾತ್ರೆ_ಪರಿಶೀಲನೆ(s, hash, ಕೀಲಿ);
        ಆದರೆ (idx != IDX_NIL)
                ಹಿಂತಿರುಗಿ 0;

        e = &ಪಾತ್ರೆ_at_ಅದಲುಬದಲು(&ಅದಲುಬದಲು, IDX_PUT)->p.b;
        e->ಕೀಲಿ = ಕೀಲಿ;
        ಹಿಂತಿರುಗಿ hashmap_put_boldly(s, hash, &ಅದಲುಬದಲು, true);
}

ಪೂರ್ಣಾಂಕ ಗಣ_ensure_put(ಗಣ_2 **s, ಸ್ಥಿರಾಂಕ ರಚನೆ hash_ops *hash_ops, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(s);

        r = ಗಣ_ensure_allocated(s, hash_ops);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ ಗಣ_put(*s, ಕೀಲಿ);
}

ಪೂರ್ಣಾಂಕ ಗಣ_ensure_consume(ಗಣ_2 **s, ಸ್ಥಿರಾಂಕ ರಚನೆ hash_ops *hash_ops, ಶೂನ್ಯ *ಕೀಲಿ) {
        ಪೂರ್ಣಾಂಕ r;

        r = ಗಣ_ensure_put(s, hash_ops, ಕೀಲಿ);
        ಆದರೆ (r <= 0) {
                ಆದರೆ (hash_ops && hash_ops->free_ಕೀಲಿ)
                        hash_ops->free_ಕೀಲಿ(ಕೀಲಿ);
                ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (hash_ops && hash_ops->free_ಮೌಲ್ಯ)
                        /* Sets store their element in the key slot but may carry a value destructor. */
                        hash_ops->free_ಮೌಲ್ಯ(ಕೀಲಿ);
                ಇಲ್ಲದಿದ್ದರೆ
                        ಬಿಡುಗಡೆ(ಕೀಲಿ);
        }

        ಹಿಂತಿರುಗಿ r;
}

ಪೂರ್ಣಾಂಕ hashmap_replace(Hashmap *h, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ, ಶೂನ್ಯ *ಮೌಲ್ಯ) {
        ರಚನೆ ಅದಲುಬದಲು_entries ಅದಲುಬದಲು;
        ರಚನೆ plain_hashmap_ನಮೂದು *e;
        ಚಿಹ್ನೆರಹಿತ hash, idx;

        ಪ್ರತಿಜ್ಞಾ(h);

        hash = ಪಾತ್ರೆ_hash(h, ಕೀಲಿ);
        idx = ಪಾತ್ರೆ_ಪರಿಶೀಲನೆ(h, hash, ಕೀಲಿ);
        ಆದರೆ (idx != IDX_NIL) {
                e = plain_ಪಾತ್ರೆ_at(h, idx);
#ಆದರೆ ENABLE_DEBUG_HASHMAP
                /* Although the key is equal, the key pointer may have changed,
                 * and this would break our assumption for iterating. So count
                 * this operation as incompatible with iteration. */
                ಆದರೆ (e->b.ಕೀಲಿ != ಕೀಲಿ) {
                        h->b.debug.put_ಎಣಿಕೆ++;
                        h->b.debug.rem_ಎಣಿಕೆ++;
                        h->b.debug.ಕೊನೆಯ_rem_idx = idx;
                }
#ಆದರೆ_ಅಂತ್ಯ
                e->b.ಕೀಲಿ = ಕೀಲಿ;
                e->ಮೌಲ್ಯ = ಮೌಲ್ಯ;
                hashmap_ಗಣ_dirty(h);

                ಹಿಂತಿರುಗಿ 0;
        }

        e = &ಪಾತ್ರೆ_at_ಅದಲುಬದಲು(&ಅದಲುಬದಲು, IDX_PUT)->p;
        e->b.ಕೀಲಿ = ಕೀಲಿ;
        e->ಮೌಲ್ಯ = ಮೌಲ್ಯ;
        ಹಿಂತಿರುಗಿ hashmap_put_boldly(h, hash, &ಅದಲುಬದಲು, true);
}

ಪೂರ್ಣಾಂಕ hashmap_update(Hashmap *h, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ, ಶೂನ್ಯ *ಮೌಲ್ಯ) {
        ರಚನೆ plain_hashmap_ನಮೂದು *e;
        ಚಿಹ್ನೆರಹಿತ hash, idx;

        ಪ್ರತಿಜ್ಞಾ(h);

        hash = ಪಾತ್ರೆ_hash(h, ಕೀಲಿ);
        idx = ಪಾತ್ರೆ_ಪರಿಶೀಲನೆ(h, hash, ಕೀಲಿ);
        ಆದರೆ (idx == IDX_NIL)
                ಹಿಂತಿರುಗಿ -ENOENT;

        e = plain_ಪಾತ್ರೆ_at(h, idx);
        e->ಮೌಲ್ಯ = ಮೌಲ್ಯ;
        hashmap_ಗಣ_dirty(h);

        ಹಿಂತಿರುಗಿ 0;
}

ಶೂನ್ಯ* _hashmap_get(Hashmap_ತಳ *h, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ) {
        ರಚನೆ hashmap_ತಳ_ನಮೂದು *e;
        ಚಿಹ್ನೆರಹಿತ hash, idx;

        ಆದರೆ (!h)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        hash = ಪಾತ್ರೆ_hash(h, ಕೀಲಿ);
        idx = ಪಾತ್ರೆ_ಪರಿಶೀಲನೆ(h, hash, ಕೀಲಿ);
        ಆದರೆ (idx == IDX_NIL)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        e = ಪಾತ್ರೆ_at(h, idx);
        ಹಿಂತಿರುಗಿ ನಮೂದು_ಮೌಲ್ಯ(h, e);
}

ಶೂನ್ಯ* hashmap_get2(Hashmap *h, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ, ಶೂನ್ಯ **ret) {
        ರಚನೆ plain_hashmap_ನಮೂದು *e;
        ಚಿಹ್ನೆರಹಿತ hash, idx;

        ಆದರೆ (!h)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        hash = ಪಾತ್ರೆ_hash(h, ಕೀಲಿ);
        idx = ಪಾತ್ರೆ_ಪರಿಶೀಲನೆ(h, hash, ಕೀಲಿ);
        ಆದರೆ (idx == IDX_NIL)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        e = plain_ಪಾತ್ರೆ_at(h, idx);
        ಆದರೆ (ret)
                *ret = (ಶೂನ್ಯ*) e->b.ಕೀಲಿ;

        ಹಿಂತಿರುಗಿ e->ಮೌಲ್ಯ;
}

bool hashmap_ಒಳಗೊಂಡಿದೆ_2(Hashmap_ತಳ *h, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ) {
        ಚಿಹ್ನೆರಹಿತ hash;

        ಆದರೆ (!h)
                ಹಿಂತಿರುಗಿ false;

        hash = ಪಾತ್ರೆ_hash(h, ಕೀಲಿ);
        ಹಿಂತಿರುಗಿ ಪಾತ್ರೆ_ಪರಿಶೀಲನೆ(h, hash, ಕೀಲಿ) != IDX_NIL;
}

ಶೂನ್ಯ* _hashmap_remove(Hashmap_ತಳ *h, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ) {
        ರಚನೆ hashmap_ತಳ_ನಮೂದು *e;
        ಚಿಹ್ನೆರಹಿತ hash, idx;
        ಶೂನ್ಯ *ದತ್ತಾಂಶ;

        ಆದರೆ (!h)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        hash = ಪಾತ್ರೆ_hash(h, ಕೀಲಿ);
        idx = ಪಾತ್ರೆ_ಪರಿಶೀಲನೆ(h, hash, ಕೀಲಿ);
        ಆದರೆ (idx == IDX_NIL)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        e = ಪಾತ್ರೆ_at(h, idx);
        ದತ್ತಾಂಶ = ನಮೂದು_ಮೌಲ್ಯ(h, e);
        remove_ನಮೂದು(h, idx);

        ಹಿಂತಿರುಗಿ ದತ್ತಾಂಶ;
}

ಶೂನ್ಯ* hashmap_remove2(Hashmap *h, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ, ಶೂನ್ಯ **ret) {
        ರಚನೆ plain_hashmap_ನಮೂದು *e;
        ಚಿಹ್ನೆರಹಿತ hash, idx;
        ಶೂನ್ಯ *ದತ್ತಾಂಶ;

        ಆದರೆ (!h) {
                ಆದರೆ (ret)
                        *ret = ಖಾಲಿ;
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;
        }

        hash = ಪಾತ್ರೆ_hash(h, ಕೀಲಿ);
        idx = ಪಾತ್ರೆ_ಪರಿಶೀಲನೆ(h, hash, ಕೀಲಿ);
        ಆದರೆ (idx == IDX_NIL) {
                ಆದರೆ (ret)
                        *ret = ಖಾಲಿ;
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;
        }

        e = plain_ಪಾತ್ರೆ_at(h, idx);
        ದತ್ತಾಂಶ = e->ಮೌಲ್ಯ;
        ಆದರೆ (ret)
                *ret = (ಶೂನ್ಯ*) e->b.ಕೀಲಿ;

        remove_ನಮೂದು(h, idx);

        ಹಿಂತಿರುಗಿ ದತ್ತಾಂಶ;
}

ಪೂರ್ಣಾಂಕ hashmap_remove_and_put(Hashmap *h, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *old_ಕೀಲಿ, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *new_ಕೀಲಿ, ಶೂನ್ಯ *ಮೌಲ್ಯ) {
        ರಚನೆ ಅದಲುಬದಲು_entries ಅದಲುಬದಲು;
        ರಚನೆ plain_hashmap_ನಮೂದು *e;
        ಚಿಹ್ನೆರಹಿತ old_hash, new_hash, idx;

        ಆದರೆ (!h)
                ಹಿಂತಿರುಗಿ -ENOENT;

        old_hash = ಪಾತ್ರೆ_hash(h, old_ಕೀಲಿ);
        idx = ಪಾತ್ರೆ_ಪರಿಶೀಲನೆ(h, old_hash, old_ಕೀಲಿ);
        ಆದರೆ (idx == IDX_NIL)
                ಹಿಂತಿರುಗಿ -ENOENT;

        new_hash = ಪಾತ್ರೆ_hash(h, new_ಕೀಲಿ);
        ಆದರೆ (ಪಾತ್ರೆ_ಪರಿಶೀಲನೆ(h, new_hash, new_ಕೀಲಿ) != IDX_NIL)
                ಹಿಂತಿರುಗಿ -EEXIST;

        remove_ನಮೂದು(h, idx);

        e = &ಪಾತ್ರೆ_at_ಅದಲುಬದಲು(&ಅದಲುಬದಲು, IDX_PUT)->p;
        e->b.ಕೀಲಿ = new_ಕೀಲಿ;
        e->ಮೌಲ್ಯ = ಮೌಲ್ಯ;
        assert_se(hashmap_put_boldly(h, new_hash, &ಅದಲುಬದಲು, false) == 1);

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ ಗಣ_remove_and_put(ಗಣ_2 *s, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *old_ಕೀಲಿ, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *new_ಕೀಲಿ) {
        ರಚನೆ ಅದಲುಬದಲು_entries ಅದಲುಬದಲು;
        ರಚನೆ hashmap_ತಳ_ನಮೂದು *e;
        ಚಿಹ್ನೆರಹಿತ old_hash, new_hash, idx;

        ಆದರೆ (!s)
                ಹಿಂತಿರುಗಿ -ENOENT;

        old_hash = ಪಾತ್ರೆ_hash(s, old_ಕೀಲಿ);
        idx = ಪಾತ್ರೆ_ಪರಿಶೀಲನೆ(s, old_hash, old_ಕೀಲಿ);
        ಆದರೆ (idx == IDX_NIL)
                ಹಿಂತಿರುಗಿ -ENOENT;

        new_hash = ಪಾತ್ರೆ_hash(s, new_ಕೀಲಿ);
        ಆದರೆ (ಪಾತ್ರೆ_ಪರಿಶೀಲನೆ(s, new_hash, new_ಕೀಲಿ) != IDX_NIL)
                ಹಿಂತಿರುಗಿ -EEXIST;

        remove_ನಮೂದು(s, idx);

        e = &ಪಾತ್ರೆ_at_ಅದಲುಬದಲು(&ಅದಲುಬದಲು, IDX_PUT)->p.b;
        e->ಕೀಲಿ = new_ಕೀಲಿ;
        assert_se(hashmap_put_boldly(s, new_hash, &ಅದಲುಬದಲು, false) == 1);

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ hashmap_remove_and_replace(Hashmap *h, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *old_ಕೀಲಿ, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *new_ಕೀಲಿ, ಶೂನ್ಯ *ಮೌಲ್ಯ) {
        ರಚನೆ ಅದಲುಬದಲು_entries ಅದಲುಬದಲು;
        ರಚನೆ plain_hashmap_ನಮೂದು *e;
        ಚಿಹ್ನೆರಹಿತ old_hash, new_hash, idx_old, idx_new;

        ಆದರೆ (!h)
                ಹಿಂತಿರುಗಿ -ENOENT;

        old_hash = ಪಾತ್ರೆ_hash(h, old_ಕೀಲಿ);
        idx_old = ಪಾತ್ರೆ_ಪರಿಶೀಲನೆ(h, old_hash, old_ಕೀಲಿ);
        ಆದರೆ (idx_old == IDX_NIL)
                ಹಿಂತಿರುಗಿ -ENOENT;

        old_ಕೀಲಿ = ಪಾತ್ರೆ_at(HASHMAP_ತಳ(h), idx_old)->ಕೀಲಿ;

        new_hash = ಪಾತ್ರೆ_hash(h, new_ಕೀಲಿ);
        idx_new = ಪಾತ್ರೆ_ಪರಿಶೀಲನೆ(h, new_hash, new_ಕೀಲಿ);
        ಆದರೆ (idx_new != IDX_NIL)
                ಆದರೆ (idx_old != idx_new) {
                        remove_ನಮೂದು(h, idx_new);
                        /* Compensate for a possible backward shift. */
                        ಆದರೆ (old_ಕೀಲಿ != ಪಾತ್ರೆ_at(HASHMAP_ತಳ(h), idx_old)->ಕೀಲಿ)
                                idx_old = prev_idx(HASHMAP_ತಳ(h), idx_old);
                        ಪ್ರತಿಜ್ಞಾ(old_ಕೀಲಿ == ಪಾತ್ರೆ_at(HASHMAP_ತಳ(h), idx_old)->ಕೀಲಿ);
                }

        remove_ನಮೂದು(h, idx_old);

        e = &ಪಾತ್ರೆ_at_ಅದಲುಬದಲು(&ಅದಲುಬದಲು, IDX_PUT)->p;
        e->b.ಕೀಲಿ = new_ಕೀಲಿ;
        e->ಮೌಲ್ಯ = ಮೌಲ್ಯ;
        assert_se(hashmap_put_boldly(h, new_hash, &ಅದಲುಬದಲು, false) == 1);

        ಹಿಂತಿರುಗಿ 0;
}

ಶೂನ್ಯ* hashmap_remove_ಮೌಲ್ಯ(Hashmap_ತಳ *h, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ, ಶೂನ್ಯ *ಮೌಲ್ಯ) {
        ರಚನೆ hashmap_ತಳ_ನಮೂದು *e;
        ಚಿಹ್ನೆರಹಿತ hash, idx;

        ಆದರೆ (!h)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        hash = ಪಾತ್ರೆ_hash(h, ಕೀಲಿ);
        idx = ಪಾತ್ರೆ_ಪರಿಶೀಲನೆ(h, hash, ಕೀಲಿ);
        ಆದರೆ (idx == IDX_NIL)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        e = ಪಾತ್ರೆ_at(h, idx);
        ಆದರೆ (ನಮೂದು_ಮೌಲ್ಯ(h, e) != ಮೌಲ್ಯ)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        remove_ನಮೂದು(h, idx);

        ಹಿಂತಿರುಗಿ ಮೌಲ್ಯ;
}

ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ಕಂಡುಹಿಡಿ_ಮೊದಲ_ನಮೂದು(Hashmap_ತಳ *h) {
        Iterator i = ITERATOR_ಮೊದಲ;

        ಆದರೆ (!h || !n_entries(h))
                ಹಿಂತಿರುಗಿ IDX_NIL;

        ಹಿಂತಿರುಗಿ hashmap_iterate_ನಮೂದು(h, &i);
}

ಶೂನ್ಯ* hashmap_ಮೊದಲ_ಕೀಲಿ_and_ಮೌಲ್ಯ(Hashmap_ತಳ *h, bool remove, ಶೂನ್ಯ **ret_ಕೀಲಿ) {
        ರಚನೆ hashmap_ತಳ_ನಮೂದು *e;
        ಶೂನ್ಯ *ಕೀಲಿ, *ದತ್ತಾಂಶ;
        ಚಿಹ್ನೆರಹಿತ idx;

        idx = ಕಂಡುಹಿಡಿ_ಮೊದಲ_ನಮೂದು(h);
        ಆದರೆ (idx == IDX_NIL) {
                ಆದರೆ (ret_ಕೀಲಿ)
                        *ret_ಕೀಲಿ = ಖಾಲಿ;
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;
        }

        e = ಪಾತ್ರೆ_at(h, idx);
        ಕೀಲಿ = (ಶೂನ್ಯ*) e->ಕೀಲಿ;
        ದತ್ತಾಂಶ = ನಮೂದು_ಮೌಲ್ಯ(h, e);

        ಆದರೆ (remove)
                remove_ನಮೂದು(h, idx);

        ಆದರೆ (ret_ಕೀಲಿ)
                *ret_ಕೀಲಿ = ಕೀಲಿ;

        ಹಿಂತಿರುಗಿ ದತ್ತಾಂಶ;
}

ಚಿಹ್ನೆರಹಿತ hashmap_ಗಾತ್ರ(Hashmap_ತಳ *h) {
        ಆದರೆ (!h)
                ಹಿಂತಿರುಗಿ 0;

        ಹಿಂತಿರುಗಿ n_entries(h);
}

ಚಿಹ್ನೆರಹಿತ _hashmap_buckets(Hashmap_ತಳ *h) {
        ಆದರೆ (!h)
                ಹಿಂತಿರುಗಿ 0;

        ಹಿಂತಿರುಗಿ n_buckets(h);
}

ಪೂರ್ಣಾಂಕ hashmap_ವಿಲೀನ(Hashmap *h, Hashmap *other) {
        Iterator i;
        ಚಿಹ್ನೆರಹಿತ idx;

        ಪ್ರತಿಜ್ಞಾ(h);

        HASHMAP_FOREACH_IDX(idx, HASHMAP_ತಳ(other), i) {
                ರಚನೆ plain_hashmap_ನಮೂದು *pe = plain_ಪಾತ್ರೆ_at(other, idx);
                ಪೂರ್ಣಾಂಕ r;

                r = hashmap_put(h, pe->b.ಕೀಲಿ, pe->ಮೌಲ್ಯ);
                ಆದರೆ (r < 0 && r != -EEXIST)
                        ಹಿಂತಿರುಗಿ r;
        }

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ ಗಣ_ವಿಲೀನ(ಗಣ_2 *s, ಗಣ_2 *other) {
        Iterator i;
        ಚಿಹ್ನೆರಹಿತ idx;

        ಪ್ರತಿಜ್ಞಾ(s);

        HASHMAP_FOREACH_IDX(idx, HASHMAP_ತಳ(other), i) {
                ರಚನೆ ಗಣ_ನಮೂದು *se = ಗಣ_ಪಾತ್ರೆ_at(other, idx);
                ಪೂರ್ಣಾಂಕ r;

                r = ಗಣ_put(s, se->b.ಕೀಲಿ);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
        }

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ _hashmap_reserve(Hashmap_ತಳ *h, ಚಿಹ್ನೆರಹಿತ entries_ಕೂಡಿಸು) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(h);

        r = resize_buckets(h, entries_ಕೂಡಿಸು);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ 0;
}

/*
 * The same as hashmap_merge(), but every new item from other is moved to h.
 * Keys already in h are skipped and stay in other.
 * Returns: 0 on success.
 *          -ENOMEM on alloc failure, in which case no move has been done.
 */
ಪೂರ್ಣಾಂಕ _hashmap_move(Hashmap_ತಳ *h, Hashmap_ತಳ *other) {
        ರಚನೆ ಅದಲುಬದಲು_entries ಅದಲುಬದಲು;
        ರಚನೆ hashmap_ತಳ_ನಮೂದು *e, *n;
        Iterator i;
        ಚಿಹ್ನೆರಹಿತ idx;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(h);

        ಆದರೆ (!other)
                ಹಿಂತಿರುಗಿ 0;

        ಪ್ರತಿಜ್ಞಾ(other->type == h->type);

        /*
         * This reserves buckets for the worst case, where none of other's
         * entries are yet present in h. This is preferable to risking
         * an allocation failure in the middle of the moving and having to
         * rollback or return a partial result.
         */
        r = resize_buckets(h, n_entries(other));
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        HASHMAP_FOREACH_IDX(idx, other, i) {
                ಚಿಹ್ನೆರಹಿತ h_hash;

                e = ಪಾತ್ರೆ_at(other, idx);
                h_hash = ಪಾತ್ರೆ_hash(h, e->ಕೀಲಿ);
                ಆದರೆ (ಪಾತ್ರೆ_ಪರಿಶೀಲನೆ(h, h_hash, e->ಕೀಲಿ) != IDX_NIL)
                        ಮುಂದುವರಿ;

                n = &ಪಾತ್ರೆ_at_ಅದಲುಬದಲು(&ಅದಲುಬದಲು, IDX_PUT)->p.b;
                n->ಕೀಲಿ = e->ಕೀಲಿ;
                ಆದರೆ (h->type != HASHMAP_TYPE_ಗಣ)
                        ((ರಚನೆ plain_hashmap_ನಮೂದು*) n)->ಮೌಲ್ಯ =
                                ((ರಚನೆ plain_hashmap_ನಮೂದು*) e)->ಮೌಲ್ಯ;
                assert_se(hashmap_put_boldly(h, h_hash, &ಅದಲುಬದಲು, false) == 1);

                remove_ನಮೂದು(other, idx);
        }

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ _hashmap_move_one(Hashmap_ತಳ *h, Hashmap_ತಳ *other, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ) {
        ರಚನೆ ಅದಲುಬದಲು_entries ಅದಲುಬದಲು;
        ಚಿಹ್ನೆರಹಿತ h_hash, other_hash, idx;
        ರಚನೆ hashmap_ತಳ_ನಮೂದು *e, *n;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(h);

        h_hash = ಪಾತ್ರೆ_hash(h, ಕೀಲಿ);
        ಆದರೆ (ಪಾತ್ರೆ_ಪರಿಶೀಲನೆ(h, h_hash, ಕೀಲಿ) != IDX_NIL)
                ಹಿಂತಿರುಗಿ -EEXIST;

        ಆದರೆ (!other)
                ಹಿಂತಿರುಗಿ -ENOENT;

        ಪ್ರತಿಜ್ಞಾ(other->type == h->type);

        other_hash = ಪಾತ್ರೆ_hash(other, ಕೀಲಿ);
        idx = ಪಾತ್ರೆ_ಪರಿಶೀಲನೆ(other, other_hash, ಕೀಲಿ);
        ಆದರೆ (idx == IDX_NIL)
                ಹಿಂತಿರುಗಿ -ENOENT;

        e = ಪಾತ್ರೆ_at(other, idx);

        n = &ಪಾತ್ರೆ_at_ಅದಲುಬದಲು(&ಅದಲುಬದಲು, IDX_PUT)->p.b;
        n->ಕೀಲಿ = e->ಕೀಲಿ;
        ಆದರೆ (h->type != HASHMAP_TYPE_ಗಣ)
                ((ರಚನೆ plain_hashmap_ನಮೂದು*) n)->ಮೌಲ್ಯ =
                        ((ರಚನೆ plain_hashmap_ನಮೂದು*) e)->ಮೌಲ್ಯ;
        r = hashmap_put_boldly(h, h_hash, &ಅದಲುಬದಲು, true);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        remove_ನಮೂದು(other, idx);
        ಹಿಂತಿರುಗಿ 0;
}

Hashmap_ತಳ* _hashmap_copy(Hashmap_ತಳ *h) {
        Hashmap_ತಳ *copy;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(h);

        copy = hashmap_ತಳ_new(h->hash_ops, h->type);
        ಆದರೆ (!copy)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        ಆಯ್ಕೆ (h->type) {
        ಸ್ಥಿತಿ HASHMAP_TYPE_PLAIN:
        ಸ್ಥಿತಿ HASHMAP_TYPE_ORDERED:
                r = hashmap_ವಿಲೀನ_2((Hashmap*)copy, (Hashmap*)h);
                ನಿಲ್ಲಿಸು;
        ಸ್ಥಿತಿ HASHMAP_TYPE_ಗಣ:
                r = ಗಣ_ವಿಲೀನ((ಗಣ_2*)copy, (ಗಣ_2*)h);
                ನಿಲ್ಲಿಸು;
        ಪೂರ್ವನಿಯೋಜಿತ:
                assert_not_reached();
        }

        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ _hashmap_free(copy);

        ಹಿಂತಿರುಗಿ copy;
}

ಅಕ್ಷರ** _hashmap_get_strv(Hashmap_ತಳ *h) {
        ಅಕ್ಷರ **sv;
        Iterator i;
        ಚಿಹ್ನೆರಹಿತ idx, n;

        ಆದರೆ (!h)
                ಹಿಂತಿರುಗಿ new0(ಅಕ್ಷರ*, 1);

        sv = new(ಅಕ್ಷರ*, n_entries(h)+1);
        ಆದರೆ (!sv)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        n = 0;
        HASHMAP_FOREACH_IDX(idx, h, i)
                sv[n++] = ನಮೂದು_ಮೌಲ್ಯ(h, ಪಾತ್ರೆ_at(h, idx));
        sv[n] = ಖಾಲಿ;

        ಹಿಂತಿರುಗಿ sv;
}

ಅಕ್ಷರ** ಗಣ_ಗಮ್ಯ_strv(ಗಣ_2 **s) {
        ಪ್ರತಿಜ್ಞಾ(s);

        /* This is similar to set_get_strv(), but invalidates the set on success. */

        ಅಕ್ಷರ **v = new(ಅಕ್ಷರ*, ಗಣ_ಗಾತ್ರ(*s) + 1);
        ಆದರೆ (!v)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        ಗಾಗಿ (ಅಕ್ಷರ **p = v; (*p = ಗಣ_steal_ಮೊದಲ(*s)); p++)
                ;

        ಪ್ರತಿಜ್ಞಾ(ಗಣ_isempty(*s));
        *s = ಗಣ_free(*s);
        ಹಿಂತಿರುಗಿ v;
}

ಶೂನ್ಯ* ordered_hashmap_ಮುಂದಿನ(OrderedHashmap *h, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ) {
        ರಚನೆ ordered_hashmap_ನಮೂದು *e;
        ಚಿಹ್ನೆರಹಿತ hash, idx;

        ಆದರೆ (!h)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        hash = ಪಾತ್ರೆ_hash(h, ಕೀಲಿ);
        idx = ಪಾತ್ರೆ_ಪರಿಶೀಲನೆ(h, hash, ಕೀಲಿ);
        ಆದರೆ (idx == IDX_NIL)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        e = ordered_ಪಾತ್ರೆ_at(h, idx);
        ಆದರೆ (e->iterate_ಮುಂದಿನ == IDX_NIL)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;
        ಹಿಂತಿರುಗಿ ordered_ಪಾತ್ರೆ_at(h, e->iterate_ಮುಂದಿನ)->p.ಮೌಲ್ಯ;
}

ಪೂರ್ಣಾಂಕ ಗಣ_consume(ಗಣ_2 *s, ಶೂನ್ಯ *ಮೌಲ್ಯ) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(s);
        ಪ್ರತಿಜ್ಞಾ(ಮೌಲ್ಯ);

        r = ಗಣ_put(s, ಮೌಲ್ಯ);
        ಆದರೆ (r <= 0)
                ಬಿಡುಗಡೆ(ಮೌಲ್ಯ);

        ಹಿಂತಿರುಗಿ r;
}

ಪೂರ್ಣಾಂಕ hashmap_put_strdup_ತುಂಬಿದ(Hashmap **h, ಸ್ಥಿರಾಂಕ ರಚನೆ hash_ops *hash_ops, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *k, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *v) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(h);

        r = hashmap_ensure_allocated(h, hash_ops);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        _cleanup_free_ ಅಕ್ಷರ *kdup = ಖಾಲಿ, *vdup = ಖಾಲಿ;

        kdup = strdup(k);
        ಆದರೆ (!kdup)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        ಆದರೆ (v) {
                vdup = strdup(v);
                ಆದರೆ (!vdup)
                        ಹಿಂತಿರುಗಿ -ENOMEM;
        }

        r = hashmap_put(*h, kdup, vdup);
        ಆದರೆ (r < 0) {
                ಆದರೆ (r == -EEXIST && streq_ptr(v, hashmap_get(*h, kdup)))
                        ಹಿಂತಿರುಗಿ 0;
                ಹಿಂತಿರುಗಿ r;
        }

        /* 0 with non-null vdup would mean vdup is already in the hashmap, which cannot be */
        ಪ್ರತಿಜ್ಞಾ(vdup == ಖಾಲಿ || r > 0);
        ಆದರೆ (r > 0)
                kdup = vdup = ಖಾಲಿ;

        ಹಿಂತಿರುಗಿ r;
}

ಪೂರ್ಣಾಂಕ ಗಣ_put_strndup_ತುಂಬಿದ(ಗಣ_2 **s, ಸ್ಥಿರಾಂಕ ರಚನೆ hash_ops *hash_ops, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, size_t n) {
        ಅಕ್ಷರ *c;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(s);
        ಪ್ರತಿಜ್ಞಾ(p);

        r = ಗಣ_ensure_allocated(s, hash_ops);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (n == SIZE_MAX) {
                ಆದರೆ (ಗಣ_ಒಳಗೊಂಡಿದೆ(*s, (ಅಕ್ಷರ*) p))
                        ಹಿಂತಿರುಗಿ 0;

                c = strdup(p);
        } ಇಲ್ಲದಿದ್ದರೆ
                c = strndup(p, n);
        ಆದರೆ (!c)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        ಹಿಂತಿರುಗಿ ಗಣ_consume(*s, c);
}

ಪೂರ್ಣಾಂಕ ಗಣ_put_strdupv_ತುಂಬಿದ(ಗಣ_2 **s, ಸ್ಥಿರಾಂಕ ರಚನೆ hash_ops *hash_ops, ಅಕ್ಷರ **l) {
        ಪೂರ್ಣಾಂಕ n = 0, r;

        ಪ್ರತಿಜ್ಞಾ(s);

        STRV_FOREACH(i, l) {
                r = ಗಣ_put_strndup_ತುಂಬಿದ(s, hash_ops, *i, SIZE_MAX);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;

                n += r;
        }

        ಹಿಂತಿರುಗಿ n;
}

ಪೂರ್ಣಾಂಕ ಗಣ_put_strsplit(ಗಣ_2 *s, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *v, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *separators, ಹೊರತೆಗೆ_Flags flags) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = ASSERT_PTR(v);
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(s);

        ಗಾಗಿ (;;) {
                ಅಕ್ಷರ *ಶಬ್ದ;

                r = ಹೊರತೆಗೆ_ಮೊದಲ_ಶಬ್ದ(&p, &ಶಬ್ದ, separators, flags);
                ಆದರೆ (r <= 0)
                        ಹಿಂತಿರುಗಿ r;

                r = ಗಣ_consume(s, ಶಬ್ದ);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
        }
}

/* expand the cachemem if needed, return true if newly (re)activated. */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ cachemem_maintain(CacheMem *mem, size_t ಗಾತ್ರ_2) {
        ಪ್ರತಿಜ್ಞಾ(mem);

        ಆದರೆ (!GREEDY_REALLOC(mem->ptr, ಗಾತ್ರ_2)) {
                ಆದರೆ (ಗಾತ್ರ_2 > 0)
                        ಹಿಂತಿರುಗಿ -ENOMEM;
        }

        ಆದರೆ (!mem->ಸಕ್ರಿಯ) {
                mem->ಸಕ್ರಿಯ = true;
                ಹಿಂತಿರುಗಿ true;
        }

        ಹಿಂತಿರುಗಿ false;
}

ಪೂರ್ಣಾಂಕ iterated_cache_get(IteratedCache *cache, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ ***res_ಕೀಲಿಗಳು, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ ***res_ಮೌಲ್ಯಗಳು, ಚಿಹ್ನೆರಹಿತ *res_n_entries) {
        bool sync_ಕೀಲಿಗಳು = false, sync_ಮೌಲ್ಯಗಳು = false;
        size_t ಗಾತ್ರ_2;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(cache);
        ಪ್ರತಿಜ್ಞಾ(cache->hashmap);

        ಗಾತ್ರ_2 = n_entries(cache->hashmap);

        ಆದರೆ (res_ಕೀಲಿಗಳು) {
                r = cachemem_maintain(&cache->ಕೀಲಿಗಳು, ಗಾತ್ರ_2);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;

                sync_ಕೀಲಿಗಳು = r;
        } ಇಲ್ಲದಿದ್ದರೆ
                cache->ಕೀಲಿಗಳು.ಸಕ್ರಿಯ = false;

        ಆದರೆ (res_ಮೌಲ್ಯಗಳು) {
                r = cachemem_maintain(&cache->values, ಗಾತ್ರ_2);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;

                sync_ಮೌಲ್ಯಗಳು = r;
        } ಇಲ್ಲದಿದ್ದರೆ
                cache->values.ಸಕ್ರಿಯ = false;

        ಆದರೆ (cache->hashmap->dirty) {
                ಆದರೆ (cache->ಕೀಲಿಗಳು.ಸಕ್ರಿಯ)
                        sync_ಕೀಲಿಗಳು = true;
                ಆದರೆ (cache->values.ಸಕ್ರಿಯ)
                        sync_ಮೌಲ್ಯಗಳು = true;

                cache->hashmap->dirty = false;
        }

        ಆದರೆ (sync_ಕೀಲಿಗಳು || sync_ಮೌಲ್ಯಗಳು) {
                ಚಿಹ್ನೆರಹಿತ i, idx;
                Iterator iter;

                i = 0;
                HASHMAP_FOREACH_IDX(idx, cache->hashmap, iter) {
                        ರಚನೆ hashmap_ತಳ_ನಮೂದು *e;

                        e = ಪಾತ್ರೆ_at(cache->hashmap, idx);

                        ಆದರೆ (sync_ಕೀಲಿಗಳು)
                                cache->ಕೀಲಿಗಳು.ptr[i] = e->ಕೀಲಿ;
                        ಆದರೆ (sync_ಮೌಲ್ಯಗಳು)
                                cache->values.ptr[i] = ನಮೂದು_ಮೌಲ್ಯ(cache->hashmap, e);
                        i++;
                }
        }

        ಆದರೆ (res_ಕೀಲಿಗಳು)
                *res_ಕೀಲಿಗಳು = cache->ಕೀಲಿಗಳು.ptr;
        ಆದರೆ (res_ಮೌಲ್ಯಗಳು)
                *res_ಮೌಲ್ಯಗಳು = cache->values.ptr;
        ಆದರೆ (res_n_entries)
                *res_n_entries = ಗಾತ್ರ_2;

        ಹಿಂತಿರುಗಿ 0;
}

IteratedCache* iterated_cache_free(IteratedCache *cache) {
        ಆದರೆ (cache) {
                ಬಿಡುಗಡೆ(cache->ಕೀಲಿಗಳು.ptr);
                ಬಿಡುಗಡೆ(cache->values.ptr);
        }

        ಹಿಂತಿರುಗಿ mfree(cache);
}

ಪೂರ್ಣಾಂಕ ಗಣ_strjoin(ಗಣ_2 *s, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *separator, bool wrap_with_separator, ಅಕ್ಷರ **ret) {
        _cleanup_free_ ಅಕ್ಷರ *str = ಖಾಲಿ;
        size_t separator_len, len = 0;
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ;
        bool ಮೊದಲ;

        ಪ್ರತಿಜ್ಞಾ(ret);

        ಆದರೆ (ಗಣ_isempty(s)) {
                *ret = ಖಾಲಿ;
                ಹಿಂತಿರುಗಿ 0;
        }

        separator_len = strlen_ptr(separator);

        ಆದರೆ (separator_len == 0)
                wrap_with_separator = false;

        ಮೊದಲ = !wrap_with_separator;

        ಗಣ_FOREACH(ಮೌಲ್ಯ, s) {
                size_t l = strlen_ptr(ಮೌಲ್ಯ);

                ಆದರೆ (l == 0)
                        ಮುಂದುವರಿ;

                ಆದರೆ (!GREEDY_REALLOC(str, len + l + (ಮೊದಲ ? 0 : separator_len) + (wrap_with_separator ? separator_len : 0) + 1))
                        ಹಿಂತಿರುಗಿ -ENOMEM;

                ಆದರೆ (separator_len > 0 && !ಮೊದಲ) {
                        ಸ್ಮರಣೆ_ನಕಲು(str + len, separator, separator_len);
                        len += separator_len;
                }

                ಸ್ಮರಣೆ_ನಕಲು(str + len, ಮೌಲ್ಯ, l);
                len += l;
                ಮೊದಲ = false;
        }

        ಆದರೆ (wrap_with_separator) {
                ಸ್ಮರಣೆ_ನಕಲು(str + len, separator, separator_len);
                len += separator_len;
        }

        str[len] = '\0';

        *ret = TAKE_PTR(str);
        ಹಿಂತಿರುಗಿ 0;
}

bool ಗಣ_equal(ಗಣ_2 *a, ಗಣ_2 *b) {
        ಶೂನ್ಯ *p;

        /* Checks whether each entry of 'a' is also in 'b' and vice versa, i.e. the two sets contain the same
         * entries */

        ಆದರೆ (a == b)
                ಹಿಂತಿರುಗಿ true;

        ಆದರೆ (ಗಣ_isempty(a) && ಗಣ_isempty(b))
                ಹಿಂತಿರುಗಿ true;

        ಆದರೆ (ಗಣ_ಗಾತ್ರ(a) != ಗಣ_ಗಾತ್ರ(b)) /* Cheap check that hopefully catches a lot of inequality cases
                                         * already */
                ಹಿಂತಿರುಗಿ false;

        ಗಣ_FOREACH(p, a)
                ಆದರೆ (!ಗಣ_ಒಳಗೊಂಡಿದೆ(b, p))
                        ಹಿಂತಿರುಗಿ false;

        /* If we have the same hashops, then we don't need to check things backwards given we compared the
         * size and that all of a is in b. */
        ಆದರೆ (a->b.hash_ops == b->b.hash_ops)
                ಹಿಂತಿರುಗಿ true;

        ಗಣ_FOREACH(p, b)
                ಆದರೆ (!ಗಣ_ಒಳಗೊಂಡಿದೆ(a, p))
                        ಹಿಂತಿರುಗಿ false;

        ಹಿಂತಿರುಗಿ true;
}

ಸ್ಥಿರ bool ಗಣ_fnmatch_one(ಗಣ_2 *patterns, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *needle) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p;

        ಪ್ರತಿಜ್ಞಾ(needle);

        /* Any failure of fnmatch() is treated as equivalent to FNM_NOMATCH, i.e. as non-matching pattern */

        ಗಣ_FOREACH(p, patterns)
                ಆದರೆ (fnmatch(p, needle, 0) == 0)
                        ಹಿಂತಿರುಗಿ true;

        ಹಿಂತಿರುಗಿ false;
}

bool ಗಣ_fnmatch(ಗಣ_2 *include_patterns, ಗಣ_2 *exclude_patterns, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *needle) {
        ಪ್ರತಿಜ್ಞಾ(needle);

        ಆದರೆ (ಗಣ_fnmatch_one(exclude_patterns, needle))
                ಹಿಂತಿರುಗಿ false;

        ಆದರೆ (ಗಣ_isempty(include_patterns))
                ಹಿಂತಿರುಗಿ true;

        ಹಿಂತಿರುಗಿ ಗಣ_fnmatch_one(include_patterns, needle);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ hashmap_ನಮೂದು_compare(
                ರಚನೆ hashmap_ತಳ_ನಮೂದು * ಸ್ಥಿರಾಂಕ *a,
                ರಚನೆ hashmap_ತಳ_ನಮೂದು * ಸ್ಥಿರಾಂಕ *b,
                compare_func_t compare) {

        ಪ್ರತಿಜ್ಞಾ(a && *a);
        ಪ್ರತಿಜ್ಞಾ(b && *b);
        ಪ್ರತಿಜ್ಞಾ(compare);

        ಹಿಂತಿರುಗಿ compare((*a)->ಕೀಲಿ, (*b)->ಕೀಲಿ);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ _hashmap_dump_entries_sorted(
                Hashmap_ತಳ *h,
                ಶೂನ್ಯ ***ret,
                size_t *ret_n) {
        _cleanup_free_ ಶೂನ್ಯ **entries = ಖಾಲಿ;
        Iterator iter;
        ಚಿಹ್ನೆರಹಿತ idx;
        size_t n = 0;

        ಪ್ರತಿಜ್ಞಾ(ret);
        ಪ್ರತಿಜ್ಞಾ(ret_n);

        ಆದರೆ (hashmap_ಗಾತ್ರ(h) == 0) {
                *ret = ಖಾಲಿ;
                *ret_n = 0;
                ಹಿಂತಿರುಗಿ 0;
        }

        /* We append one more element than needed so that the resulting array can be used as a strv. We
         * don't count this entry in the returned size. */
        entries = new(ಶೂನ್ಯ*, hashmap_ಗಾತ್ರ(h) + 1);
        ಆದರೆ (!entries)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        HASHMAP_FOREACH_IDX(idx, h, iter)
                entries[n++] = ಪಾತ್ರೆ_at(h, idx);

        ಪ್ರತಿಜ್ಞಾ(n == hashmap_ಗಾತ್ರ(h));
        entries[n] = ಖಾಲಿ;

        typesafe_qsort_r((ರಚನೆ hashmap_ತಳ_ನಮೂದು**) entries, n,
                         hashmap_ನಮೂದು_compare, h->hash_ops->compare);

        *ret = TAKE_PTR(entries);
        *ret_n = n;
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ hashmap_dump_ಕೀಲಿಗಳು_sorted(Hashmap_ತಳ *h, ಶೂನ್ಯ ***ret, size_t *ret_n) {
        _cleanup_free_ ಶೂನ್ಯ **entries = ಖಾಲಿ;
        size_t n;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        r = _hashmap_dump_entries_sorted(h, &entries, &n);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        /* Reuse the array. */
        FOREACH_ARRAY(e, entries, n)
                *e = (ಶೂನ್ಯ*) (*(ರಚನೆ hashmap_ತಳ_ನಮೂದು**) e)->ಕೀಲಿ;

        *ret = TAKE_PTR(entries);
        ಆದರೆ (ret_n)
                *ret_n = n;
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ _hashmap_dump_sorted(Hashmap_ತಳ *h, ಶೂನ್ಯ ***ret, size_t *ret_n) {
        _cleanup_free_ ಶೂನ್ಯ **entries = ಖಾಲಿ;
        size_t n;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        r = _hashmap_dump_entries_sorted(h, &entries, &n);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        /* Reuse the array. */
        FOREACH_ARRAY(e, entries, n)
                *e = ನಮೂದು_ಮೌಲ್ಯ(h, *(ರಚನೆ hashmap_ತಳ_ನಮೂದು**) e);

        *ret = TAKE_PTR(entries);
        ಆದರೆ (ret_n)
                *ret_n = n;
        ಹಿಂತಿರುಗಿ 0;
}
