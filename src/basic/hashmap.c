/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ <fnmatch.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <pthread.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <unistd.h>
#ਜੇ HAVE_VALGRIND_VALGRIND_H
#  include <valgrind/valgrind.h>
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ "alloc-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "extract-word.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "hashmap.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "log.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "logarithm.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "memory-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mempool.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "process-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "random-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "set.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "siphash24.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "sort-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "strv.h"

#ਜੇ ENABLE_DEBUG_HASHMAP
#ਅਨ੍ਤਰ੍ਭਾਵ "list.h"
#ਜੇ_ਅੰਤ

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
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ INV_KEEP_FREE            5U

/* Fields common to entries of all hashmap/set types */
ਬਣਤਰ hashmap_ਅਧਾਰ_ਇੰਦਰਾਜ {
        ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ;
};

/* Entry types for specific hashmap/set types
 * hashmap_base_entry must be at the beginning of each entry struct. */

ਬਣਤਰ plain_hashmap_ਇੰਦਰਾਜ {
        ਬਣਤਰ hashmap_ਅਧਾਰ_ਇੰਦਰਾਜ b;
        ਖਾਲੀ *ਮੁੱਲ;
};

ਬਣਤਰ ordered_hashmap_ਇੰਦਰਾਜ {
        ਬਣਤਰ plain_hashmap_ਇੰਦਰਾਜ p;
        ਬਿਨਾਨਿਸ਼ਾਨੀ iterate_ਅਗਲਾ, iterate_ਪਿਛਲਾ;
};

ਬਣਤਰ ਸੈੱਟ_ਇੰਦਰਾਜ {
        ਬਣਤਰ hashmap_ਅਧਾਰ_ਇੰਦਰਾਜ b;
};

/* In several functions it is advantageous to have the hash table extended
 * virtually by a couple of additional buckets. We reserve special index values
 * for these "swap" buckets. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IDX_ਅਦਲਾ_ਬਦਲੀ_BEGIN     (UINT_MAX - 3)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IDX_PUT             (IDX_ਅਦਲਾ_ਬਦਲੀ_BEGIN + 0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IDX_TMP             (IDX_ਅਦਲਾ_ਬਦਲੀ_BEGIN + 1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IDX_ਅਦਲਾ_ਬਦਲੀ_ਸਿਰਾ       (IDX_ਅਦਲਾ_ਬਦਲੀ_BEGIN + 2)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IDX_ਪਹਿਲਾ           (UINT_MAX - 1) /* special index for freshly initialized iterators */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IDX_NIL             UINT_MAX       /* special index value meaning "none" or "end" */

assert_cc(IDX_ਪਹਿਲਾ == IDX_ਅਦਲਾ_ਬਦਲੀ_ਸਿਰਾ);
assert_cc(IDX_ਪਹਿਲਾ == IDX_ITERATOR_ਪਹਿਲਾ);

/* Storage space for the "swap" buckets.
 * All entry types can fit into an ordered_hashmap_entry. */
ਬਣਤਰ ਅਦਲਾ_ਬਦਲੀ_entries {
        ਬਣਤਰ ordered_hashmap_ਇੰਦਰਾਜ e[IDX_ਅਦਲਾ_ਬਦਲੀ_ਸਿਰਾ - IDX_ਅਦਲਾ_ਬਦਲੀ_BEGIN];
};

/* Distance from Initial Bucket */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ uint8_t dib_raw_t;
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DIB_RAW_ਉਛਾਲ ((dib_raw_t)0xfdU)   /* indicates DIB value is greater than representable */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DIB_RAW_REHASH   ((dib_raw_t)0xfeU)   /* entry yet to be rehashed during in-place resize */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DIB_RAW_FREE     ((dib_raw_t)0xffU)   /* a free bucket */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DIB_RAW_INIT     ((ਅੱਖਰ)DIB_RAW_FREE) /* a byte to memset a DIB store with when initializing */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DIB_FREE UINT_MAX

#ਜੇ ENABLE_DEBUG_HASHMAP
ਬਣਤਰ hashmap_debug_info {
        LIST_FIELDS(ਬਣਤਰ hashmap_debug_info, debug_list);
        ਬਿਨਾਨਿਸ਼ਾਨੀ max_entries;  /* high watermark of n_entries */

        /* fields to detect modification while iterating */
        ਬਿਨਾਨਿਸ਼ਾਨੀ put_ਗਿਣਤੀ;    /* counts puts into the hashmap */
        ਬਿਨਾਨਿਸ਼ਾਨੀ rem_ਗਿਣਤੀ;    /* counts removals from hashmap */
        ਬਿਨਾਨਿਸ਼ਾਨੀ ਆਖਰੀ_rem_idx; /* remembers last removal index */
};

/* Tracks all existing hashmaps. Get at it from gdb. See sd_dump_hashmaps.py */
ਸਥਿਰ LIST_ਸਿਰ(ਬਣਤਰ hashmap_debug_info, hashmap_debug_list);
ਸਥਿਰ pthread_mutex_t hashmap_debug_list_mutex = PTHREAD_MUTEX_INITIALIZER;
#ਜੇ_ਅੰਤ

ਗਿਣਤੀ HashmapType {
        HASHMAP_TYPE_PLAIN,
        HASHMAP_TYPE_ORDERED,
        HASHMAP_TYPE_ਸੈੱਟ,
        _HASHMAP_TYPE_MAX
};

ਬਣਤਰ _packed_ ਅਸਿੱਧਾ_storage {
        ਖਾਲੀ *storage;                     /* where buckets and DIBs are stored */
        uint8_t  hash_ਕੁੰਜੀ[HASH_ਕੁੰਜੀ_ਮਾਪ];  /* hash key; changes during resize */

        ਬਿਨਾਨਿਸ਼ਾਨੀ n_entries;                /* number of stored entries */
        ਬਿਨਾਨਿਸ਼ਾਨੀ n_buckets;                /* number of buckets */

        ਬਿਨਾਨਿਸ਼ਾਨੀ idx_ਸਭ_ਤੋਂ_ਨੀਵਾਂ_ਇੰਦਰਾਜ;         /* Index below which all buckets are free.
                                              Makes "while (hashmap_steal_first())" loops
                                              O(n) instead of O(n^2) for unordered hashmaps. */
        uint8_t  _pad[3];                  /* padding for the whole HashmapBase */
        /* The bitfields in HashmapBase complete the alignment of the whole thing. */
};

ਬਣਤਰ ਸਿੱਧਾ_storage {
        /* This gives us 39 bytes on 64-bit, or 35 bytes on 32-bit.
         * That's room for 4 set_entries + 4 DIB bytes + 3 unused bytes on 64-bit,
         *              or 7 set_entries + 7 DIB bytes + 0 unused bytes on 32-bit. */
        uint8_t storage[ਆਕਾਰ(ਬਣਤਰ ਅਸਿੱਧਾ_storage)];
};

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿੱਧਾ_BUCKETS(ਇੰਦਰਾਜ_t) \
        (ਆਕਾਰ(ਬਣਤਰ ਸਿੱਧਾ_storage) / (ਆਕਾਰ(ਇੰਦਰਾਜ_t) + ਆਕਾਰ(dib_raw_t)))

/* We should be able to store at least one entry directly. */
assert_cc(ਸਿੱਧਾ_BUCKETS(ਬਣਤਰ ordered_hashmap_ਇੰਦਰਾਜ) >= 1);

/* We have 3 bits for n_direct_entries. */
assert_cc(ਸਿੱਧਾ_BUCKETS(ਬਣਤਰ ਸੈੱਟ_ਇੰਦਰਾਜ) < (1 << 3));

/* Hashmaps with directly stored entries all use this shared hash key.
 * It's no big deal if the key is guessed, because there can be only
 * a handful of directly stored entries in a hashmap. When a hashmap
 * outgrows direct storage, it gets its own key for indirect storage. */
ਸਥਿਰ uint8_t shared_hash_ਕੁੰਜੀ[HASH_ਕੁੰਜੀ_ਮਾਪ];

/* Fields that all hashmap/set types must have */
ਬਣਤਰ Hashmap_ਅਧਾਰ {
        ਅਚਲ ਬਣਤਰ hash_ops *hash_ops;  /* hash and compare ops to use */

        ਸੰਘ _packed_ {
                ਬਣਤਰ ਅਸਿੱਧਾ_storage ਅਸਿੱਧਾ; /* if  has_indirect */
                ਬਣਤਰ ਸਿੱਧਾ_storage ਸਿੱਧਾ;     /* if !has_indirect */
        };

        ਗਿਣਤੀ HashmapType type:2;     /* HASHMAP_TYPE_* */
        bool has_ਅਸਿੱਧਾ:1;         /* whether indirect storage is used */
        ਬਿਨਾਨਿਸ਼ਾਨੀ n_ਸਿੱਧਾ_entries:3; /* Number of entries in direct storage.
                                      * Only valid if !has_indirect. */
        bool from_pool:1;            /* whether was allocated from mempool */
        bool dirty:1;                /* whether dirtied since last iterated_cache_get() */
        bool cached:1;               /* whether this hashmap is being cached */

#ਜੇ ENABLE_DEBUG_HASHMAP
        ਬਣਤਰ hashmap_debug_info debug;
#ਜੇ_ਅੰਤ
};

/* Specific hash types
 * HashmapBase must be at the beginning of each hashmap struct. */

ਬਣਤਰ Hashmap {
        ਬਣਤਰ Hashmap_ਅਧਾਰ b;
};

ਬਣਤਰ OrderedHashmap {
        ਬਣਤਰ Hashmap_ਅਧਾਰ b;
        ਬਿਨਾਨਿਸ਼ਾਨੀ iterate_list_ਸਿਰ, iterate_list_ਪੂਛ;
};

ਬਣਤਰ ਸੈੱਟ_2 {
        ਬਣਤਰ Hashmap_ਅਧਾਰ b;
};

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ CacheMem {
        ਅਚਲ ਖਾਲੀ **ptr;
        size_t n_populated;
        bool ਸਰਗਰਮ:1;
} CacheMem;

ਬਣਤਰ IteratedCache {
        Hashmap_ਅਧਾਰ *hashmap;
        CacheMem ਕੁੰਜੀਆਂ, values;
};

DEFINE_MEMPOOL(hashmap_pool,         Hashmap,        8);
DEFINE_MEMPOOL(ordered_hashmap_pool, OrderedHashmap, 8);
/* No need for a separate Set pool */
assert_cc(ਆਕਾਰ(Hashmap) == ਆਕਾਰ(ਸੈੱਟ_2));

ਬਣਤਰ hashmap_type_info {
        size_t ਸਿਰ_ਮਾਪ;
        size_t ਇੰਦਰਾਜ_ਮਾਪ;
        ਬਣਤਰ mempool *mempool;
        ਬਿਨਾਨਿਸ਼ਾਨੀ n_ਸਿੱਧਾ_buckets;
};

ਸਥਿਰ ਵਰਤਿਆ_2 ਅਚਲ ਬਣਤਰ hashmap_type_info hashmap_type_info[_HASHMAP_TYPE_MAX] = {
        [HASHMAP_TYPE_PLAIN] = {
                .ਸਿਰ_ਮਾਪ        = ਆਕਾਰ(Hashmap),
                .ਇੰਦਰਾਜ_ਮਾਪ       = ਆਕਾਰ(ਬਣਤਰ plain_hashmap_ਇੰਦਰਾਜ),
                .mempool          = &hashmap_pool,
                .n_ਸਿੱਧਾ_buckets = ਸਿੱਧਾ_BUCKETS(ਬਣਤਰ plain_hashmap_ਇੰਦਰਾਜ),
        },
        [HASHMAP_TYPE_ORDERED] = {
                .ਸਿਰ_ਮਾਪ        = ਆਕਾਰ(OrderedHashmap),
                .ਇੰਦਰਾਜ_ਮਾਪ       = ਆਕਾਰ(ਬਣਤਰ ordered_hashmap_ਇੰਦਰਾਜ),
                .mempool          = &ordered_hashmap_pool,
                .n_ਸਿੱਧਾ_buckets = ਸਿੱਧਾ_BUCKETS(ਬਣਤਰ ordered_hashmap_ਇੰਦਰਾਜ),
        },
        [HASHMAP_TYPE_ਸੈੱਟ] = {
                .ਸਿਰ_ਮਾਪ        = ਆਕਾਰ(ਸੈੱਟ_2),
                .ਇੰਦਰਾਜ_ਮਾਪ       = ਆਕਾਰ(ਬਣਤਰ ਸੈੱਟ_ਇੰਦਰਾਜ),
                .mempool          = &hashmap_pool,
                .n_ਸਿੱਧਾ_buckets = ਸਿੱਧਾ_BUCKETS(ਬਣਤਰ ਸੈੱਟ_ਇੰਦਰਾਜ),
        },
};

ਖਾਲੀ hashmap_trim_pools(ਖਾਲੀ) {
        ਪੂਰਨ r;

        /* The pool is only allocated by the main thread, but the memory can be passed to other
         * threads. Let's clean up if we are the main thread and no other threads are live. */

        /* We build our own is_main_thread() here, which doesn't use C11 TLS based caching of the
         * result. That's because valgrind apparently doesn't like TLS to be used from a GCC destructor. */
        ਜੇ (getpid() != gettid())
                ਵਾਪਸ (ਖਾਲੀ) log_debug("Not cleaning up memory pools, not in main thread.");

        r = get_process_ਧਾਗੇ(0);
        ਜੇ (r < 0)
                ਵਾਪਸ (ਖਾਲੀ) log_debug_errno(r, "Failed to determine number of threads, not cleaning up memory pools: %m");
        ਜੇ (r != 1)
                ਵਾਪਸ (ਖਾਲੀ) log_debug("Not cleaning up memory pools, running in multi-threaded process.");

        mempool_trim(&hashmap_pool);
        mempool_trim(&ordered_hashmap_pool);
}

#ਜੇ HAVE_VALGRIND_VALGRIND_H
_destructor_ ਸਥਿਰ ਖਾਲੀ cleanup_pools(ਖਾਲੀ) {
        /* Be nice to valgrind */
        ਜੇ (RUNNING_ON_VALGRIND)
                hashmap_trim_pools();
}
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਸਤਰਅੰਦਰ ਬਿਨਾਨਿਸ਼ਾਨੀ n_buckets(Hashmap_ਅਧਾਰ *h) {
        ਪ੍ਰਤਿਜ੍ਞਾ(h);
        ਵਾਪਸ h->has_ਅਸਿੱਧਾ ? h->ਅਸਿੱਧਾ.n_buckets
                               : hashmap_type_info[h->type].n_ਸਿੱਧਾ_buckets;
}

ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ n_entries(Hashmap_ਅਧਾਰ *h) {
        ਪ੍ਰਤਿਜ੍ਞਾ(h);
        ਵਾਪਸ h->has_ਅਸਿੱਧਾ ? h->ਅਸਿੱਧਾ.n_entries
                               : h->n_ਸਿੱਧਾ_entries;
}

ਸਥਿਰ ਖਾਲੀ n_entries_inc(Hashmap_ਅਧਾਰ *h) {
        ਪ੍ਰਤਿਜ੍ਞਾ(h);

        ਜੇ (h->has_ਅਸਿੱਧਾ)
                h->ਅਸਿੱਧਾ.n_entries++;
        ਨਹੀਂਤਾਂ
                h->n_ਸਿੱਧਾ_entries++;
}

ਸਥਿਰ ਖਾਲੀ n_entries_dec(Hashmap_ਅਧਾਰ *h) {
        ਪ੍ਰਤਿਜ੍ਞਾ(h);

        ਜੇ (h->has_ਅਸਿੱਧਾ)
                h->ਅਸਿੱਧਾ.n_entries--;
        ਨਹੀਂਤਾਂ
                h->n_ਸਿੱਧਾ_entries--;
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ* storage_ptr(Hashmap_ਅਧਾਰ *h) {
        ਪ੍ਰਤਿਜ੍ਞਾ(h);
        ਵਾਪਸ h->has_ਅਸਿੱਧਾ ? h->ਅਸਿੱਧਾ.storage
                               : h->ਸਿੱਧਾ.storage;
}

ਸਥਿਰ ਸਤਰਅੰਦਰ uint8_t* hash_ਕੁੰਜੀ(Hashmap_ਅਧਾਰ *h) {
        ਪ੍ਰਤਿਜ੍ਞਾ(h);
        ਵਾਪਸ h->has_ਅਸਿੱਧਾ ? h->ਅਸਿੱਧਾ.hash_ਕੁੰਜੀ
                               : shared_hash_ਕੁੰਜੀ;
}

ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ_ਬਾਲਟੀ_hash(Hashmap_ਅਧਾਰ *h, ਅਚਲ ਖਾਲੀ *p) {
        ਬਣਤਰ siphash state;
        uint64_t hash;

        ਪ੍ਰਤਿਜ੍ਞਾ(h);

        /* The keys of trivial_hash_ops and uint64_hash_ops fit in 64 bits. siphash24_uint64() hashes such
         * a key without the indirect call through hash_ops and keeps the SipHash state in registers. On
         * 32-bit and big-endian architectures, the result differs from the result of trivial_hash_func(),
         * which hashes the key in native byte order. Only this function maps keys to buckets, so the
         * difference does not matter. */
        ਜੇ (h->hash_ops->hash == trivial_hash_func)
                hash = siphash24_uint64((uintptr_t) p, hash_ਕੁੰਜੀ(h));
        ਨਹੀਂਤਾਂ ਜੇ (h->hash_ops->hash == (hash_func_t) uint64_hash_func)
                hash = siphash24_uint64(*(ਅਚਲ uint64_t*) p, hash_ਕੁੰਜੀ(h));
        ਨਹੀਂਤਾਂ {
                siphash24_init(&state, hash_ਕੁੰਜੀ(h));
                h->hash_ops->hash(p, &state);
                hash = siphash24_finalize(&state);
        }

        /* n_buckets is not a power of two, so a modulo needs a division. Multiplying the upper 32 bits of
         * the hash with n_buckets and keeping the upper 32 bits of the product maps the hash to
         * [0, n_buckets) almost as evenly, without a division. */
        ਵਾਪਸ (ਬਿਨਾਨਿਸ਼ਾਨੀ) (((hash >> 32) * n_buckets(h)) >> 32);
}
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਬਾਲਟੀ_hash(h, p) ਅਧਾਰ_ਬਾਲਟੀ_hash(HASHMAP_ਅਧਾਰ(h), p)

ਸਥਿਰ ਖਾਲੀ ਅਧਾਰ_ਸੈੱਟ_dirty(Hashmap_ਅਧਾਰ *h) {
        ਪ੍ਰਤਿਜ੍ਞਾ(h);

        h->dirty = true;
}
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ hashmap_ਸੈੱਟ_dirty(h) ਅਧਾਰ_ਸੈੱਟ_dirty(HASHMAP_ਅਧਾਰ(h))

ਸਥਿਰ ਖਾਲੀ get_hash_ਕੁੰਜੀ(uint8_t hash_ਕੁੰਜੀ[HASH_ਕੁੰਜੀ_ਮਾਪ], bool reuse_is_ok) {
        ਸਥਿਰ uint8_t current[HASH_ਕੁੰਜੀ_ਮਾਪ];
        ਸਥਿਰ bool current_initialized = false;

        /* Returns a hash function key to use. In order to keep things
         * fast we will not generate a new key each time we allocate a
         * new hash table. Instead, we'll just reuse the most recently
         * generated one, except if we never generated one or when we
         * are rehashing an entire hash table because we reached a
         * fill level */

        ਜੇ (!current_initialized || !reuse_is_ok) {
                random_ਬਾਈਟਾਂ(current, ਆਕਾਰ(current));
                current_initialized = true;
        }

        ਸਮ੍ਰਿਤੀ_ਨਕਲ(hash_ਕੁੰਜੀ, current, ਆਕਾਰ(current));
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਬਣਤਰ hashmap_ਅਧਾਰ_ਇੰਦਰਾਜ* ਬਾਲਟੀ_at(Hashmap_ਅਧਾਰ *h, ਬਿਨਾਨਿਸ਼ਾਨੀ idx) {
        ਪ੍ਰਤਿਜ੍ਞਾ(h);
        ਵਾਪਸ CAST_ALIGN_PTR(
                        ਬਣਤਰ hashmap_ਅਧਾਰ_ਇੰਦਰਾਜ,
                        (uint8_t *) storage_ptr(h) + idx * hashmap_type_info[h->type].ਇੰਦਰਾਜ_ਮਾਪ);
}

ਸਥਿਰ ਬਣਤਰ plain_hashmap_ਇੰਦਰਾਜ* plain_ਬਾਲਟੀ_at(Hashmap *h, ਬਿਨਾਨਿਸ਼ਾਨੀ idx) {
        ਵਾਪਸ (ਬਣਤਰ plain_hashmap_ਇੰਦਰਾਜ*) ਬਾਲਟੀ_at(HASHMAP_ਅਧਾਰ(h), idx);
}

ਸਥਿਰ ਬਣਤਰ ordered_hashmap_ਇੰਦਰਾਜ* ordered_ਬਾਲਟੀ_at(OrderedHashmap *h, ਬਿਨਾਨਿਸ਼ਾਨੀ idx) {
        ਵਾਪਸ (ਬਣਤਰ ordered_hashmap_ਇੰਦਰਾਜ*) ਬਾਲਟੀ_at(HASHMAP_ਅਧਾਰ(h), idx);
}

ਸਥਿਰ ਬਣਤਰ ਸੈੱਟ_ਇੰਦਰਾਜ *ਸੈੱਟ_ਬਾਲਟੀ_at(ਸੈੱਟ_2 *h, ਬਿਨਾਨਿਸ਼ਾਨੀ idx) {
        ਵਾਪਸ (ਬਣਤਰ ਸੈੱਟ_ਇੰਦਰਾਜ*) ਬਾਲਟੀ_at(HASHMAP_ਅਧਾਰ(h), idx);
}

ਸਥਿਰ ਬਣਤਰ ordered_hashmap_ਇੰਦਰਾਜ* ਬਾਲਟੀ_at_ਅਦਲਾ_ਬਦਲੀ(ਬਣਤਰ ਅਦਲਾ_ਬਦਲੀ_entries *ਅਦਲਾ_ਬਦਲੀ, ਬਿਨਾਨਿਸ਼ਾਨੀ idx) {
        ਪ੍ਰਤਿਜ੍ਞਾ(ਅਦਲਾ_ਬਦਲੀ);
        ਵਾਪਸ &ਅਦਲਾ_ਬਦਲੀ->e[idx - IDX_ਅਦਲਾ_ਬਦਲੀ_BEGIN];
}

/* Returns a pointer to the bucket at index idx.
 * Understands real indexes and swap indexes, hence "_virtual". */
ਸਥਿਰ ਬਣਤਰ hashmap_ਅਧਾਰ_ਇੰਦਰਾਜ* ਬਾਲਟੀ_at_virtual(Hashmap_ਅਧਾਰ *h, ਬਣਤਰ ਅਦਲਾ_ਬਦਲੀ_entries *ਅਦਲਾ_ਬਦਲੀ,
                                                    ਬਿਨਾਨਿਸ਼ਾਨੀ idx) {
        ਜੇ (idx < IDX_ਅਦਲਾ_ਬਦਲੀ_BEGIN)
                ਵਾਪਸ ਬਾਲਟੀ_at(h, idx);

        ਜੇ (idx < IDX_ਅਦਲਾ_ਬਦਲੀ_ਸਿਰਾ)
                ਵਾਪਸ &ਬਾਲਟੀ_at_ਅਦਲਾ_ਬਦਲੀ(ਅਦਲਾ_ਬਦਲੀ, idx)->p.b;

        assert_not_reached();
}

ਸਥਿਰ ਸਤਰਅੰਦਰ dib_raw_t* dib_raw_ptr(Hashmap_ਅਧਾਰ *h) {
        ਪ੍ਰਤਿਜ੍ਞਾ(h);
        ਵਾਪਸ (dib_raw_t*)
                ((uint8_t*) storage_ptr(h) + hashmap_type_info[h->type].ਇੰਦਰਾਜ_ਮਾਪ * n_buckets(h));
}

ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਬਾਲਟੀ_ਦੂਰੀ(Hashmap_ਅਧਾਰ *h, ਬਿਨਾਨਿਸ਼ਾਨੀ idx, ਬਿਨਾਨਿਸ਼ਾਨੀ from) {
        ਵਾਪਸ idx >= from ? idx - from
                           : n_buckets(h) + idx - from;
}

ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਬਾਲਟੀ_calculate_dib(Hashmap_ਅਧਾਰ *h, ਬਿਨਾਨਿਸ਼ਾਨੀ idx, dib_raw_t raw_dib) {
        ਬਿਨਾਨਿਸ਼ਾਨੀ initial_ਬਾਲਟੀ;

        ਜੇ (raw_dib == DIB_RAW_FREE)
                ਵਾਪਸ DIB_FREE;

        ਜੇ (_likely_(raw_dib < DIB_RAW_ਉਛਾਲ))
                ਵਾਪਸ raw_dib;

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
        initial_ਬਾਲਟੀ = ਬਾਲਟੀ_hash(h, ਬਾਲਟੀ_at(h, idx)->ਕੁੰਜੀ);
        ਵਾਪਸ ਬਾਲਟੀ_ਦੂਰੀ(h, idx, initial_ਬਾਲਟੀ);
}

ਸਥਿਰ ਖਾਲੀ ਬਾਲਟੀ_ਸੈੱਟ_dib(Hashmap_ਅਧਾਰ *h, ਬਿਨਾਨਿਸ਼ਾਨੀ idx, ਬਿਨਾਨਿਸ਼ਾਨੀ dib) {
        dib_raw_ptr(h)[idx] = dib != DIB_FREE ? MIN(dib, DIB_RAW_ਉਛਾਲ) : DIB_RAW_FREE;
}

ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ skip_free_buckets(Hashmap_ਅਧਾਰ *h, ਬਿਨਾਨਿਸ਼ਾਨੀ idx) {
        dib_raw_t *dibs;

        dibs = dib_raw_ptr(h);

        ਲਈ ( ; idx < n_buckets(h); idx++)
                ਜੇ (dibs[idx] != DIB_RAW_FREE)
                        ਵਾਪਸ idx;

        ਵਾਪਸ IDX_NIL;
}

ਸਥਿਰ ਖਾਲੀ ਬਾਲਟੀ_mark_free(Hashmap_ਅਧਾਰ *h, ਬਿਨਾਨਿਸ਼ਾਨੀ idx) {
        ਪ੍ਰਤਿਜ੍ਞਾ(h);

        memzero(ਬਾਲਟੀ_at(h, idx), hashmap_type_info[h->type].ਇੰਦਰਾਜ_ਮਾਪ);
        ਬਾਲਟੀ_ਸੈੱਟ_dib(h, idx, DIB_FREE);
}

ਸਥਿਰ ਖਾਲੀ ਬਾਲਟੀ_move_ਇੰਦਰਾਜ(Hashmap_ਅਧਾਰ *h, ਬਣਤਰ ਅਦਲਾ_ਬਦਲੀ_entries *ਅਦਲਾ_ਬਦਲੀ,
                              ਬਿਨਾਨਿਸ਼ਾਨੀ from, ਬਿਨਾਨਿਸ਼ਾਨੀ to) {
        ਬਣਤਰ hashmap_ਅਧਾਰ_ਇੰਦਰਾਜ *e_from, *e_ਮੰਜ਼ਿਲ;

        ਪ੍ਰਤਿਜ੍ਞਾ(h);
        ਪ੍ਰਤਿਜ੍ਞਾ(from != to);

        e_from = ਬਾਲਟੀ_at_virtual(h, ਅਦਲਾ_ਬਦਲੀ, from);
        e_ਮੰਜ਼ਿਲ   = ਬਾਲਟੀ_at_virtual(h, ਅਦਲਾ_ਬਦਲੀ, to);

        ਸਮ੍ਰਿਤੀ_ਨਕਲ(e_ਮੰਜ਼ਿਲ, e_from, hashmap_type_info[h->type].ਇੰਦਰਾਜ_ਮਾਪ);

        ਜੇ (h->type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*) h;
                ਬਣਤਰ ordered_hashmap_ਇੰਦਰਾਜ *le, *le_ਮੰਜ਼ਿਲ;

                le_ਮੰਜ਼ਿਲ = (ਬਣਤਰ ordered_hashmap_ਇੰਦਰਾਜ*) e_ਮੰਜ਼ਿਲ;

                ਜੇ (le_ਮੰਜ਼ਿਲ->iterate_ਅਗਲਾ != IDX_NIL) {
                        le = (ਬਣਤਰ ordered_hashmap_ਇੰਦਰਾਜ*)
                             ਬਾਲਟੀ_at_virtual(h, ਅਦਲਾ_ਬਦਲੀ, le_ਮੰਜ਼ਿਲ->iterate_ਅਗਲਾ);
                        le->iterate_ਪਿਛਲਾ = to;
                }

                ਜੇ (le_ਮੰਜ਼ਿਲ->iterate_ਪਿਛਲਾ != IDX_NIL) {
                        le = (ਬਣਤਰ ordered_hashmap_ਇੰਦਰਾਜ*)
                             ਬਾਲਟੀ_at_virtual(h, ਅਦਲਾ_ਬਦਲੀ, le_ਮੰਜ਼ਿਲ->iterate_ਪਿਛਲਾ);
                        le->iterate_ਅਗਲਾ = to;
                }

                ਜੇ (lh->iterate_list_ਸਿਰ == from)
                        lh->iterate_list_ਸਿਰ = to;
                ਜੇ (lh->iterate_list_ਪੂਛ == from)
                        lh->iterate_list_ਪੂਛ = to;
        }
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਗਲਾ_idx(Hashmap_ਅਧਾਰ *h, ਬਿਨਾਨਿਸ਼ਾਨੀ idx) {
        ਵਾਪਸ idx + 1U < n_buckets(h) ? idx + 1U : 0U;
}

ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ prev_idx(Hashmap_ਅਧਾਰ *h, ਬਿਨਾਨਿਸ਼ਾਨੀ idx) {
        ਵਾਪਸ idx > 0U ? idx - 1U : n_buckets(h) - 1U;
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ* ਇੰਦਰਾਜ_ਮੁੱਲ(Hashmap_ਅਧਾਰ *h, ਬਣਤਰ hashmap_ਅਧਾਰ_ਇੰਦਰਾਜ *e) {
        ਪ੍ਰਤਿਜ੍ਞਾ(h);
        ਪ੍ਰਤਿਜ੍ਞਾ(e);

        ਚੋਣ (h->type) {

        ਹਾਲਤ HASHMAP_TYPE_PLAIN:
        ਹਾਲਤ HASHMAP_TYPE_ORDERED:
                ਵਾਪਸ ((ਬਣਤਰ plain_hashmap_ਇੰਦਰਾਜ*)e)->ਮੁੱਲ;

        ਹਾਲਤ HASHMAP_TYPE_ਸੈੱਟ:
                ਵਾਪਸ (ਖਾਲੀ*) e->ਕੁੰਜੀ;

        ਮੂਲ:
                assert_not_reached();
        }
}

ਸਥਿਰ ਖਾਲੀ ਅਧਾਰ_remove_ਇੰਦਰਾਜ(Hashmap_ਅਧਾਰ *h, ਬਿਨਾਨਿਸ਼ਾਨੀ idx) {
        ਬਿਨਾਨਿਸ਼ਾਨੀ ਖੱਬਾ, ਸੱਜਾ, prev, dib;
        dib_raw_t raw_dib, *dibs;

        dibs = dib_raw_ptr(h);
        ਪ੍ਰਤਿਜ੍ਞਾ(dibs[idx] != DIB_RAW_FREE);

#ਜੇ ENABLE_DEBUG_HASHMAP
        ਪ੍ਰਤਿਜ੍ਞਾ(h);
        h->debug.rem_ਗਿਣਤੀ++;
        h->debug.ਆਖਰੀ_rem_idx = idx;
#ਜੇ_ਅੰਤ

        ਖੱਬਾ = idx;
        /* Find the stop bucket ("right"). It is either free or has DIB == 0. */
        ਲਈ (ਸੱਜਾ = ਅਗਲਾ_idx(h, ਖੱਬਾ); ; ਸੱਜਾ = ਅਗਲਾ_idx(h, ਸੱਜਾ)) {
                raw_dib = dibs[ਸੱਜਾ];
                ਜੇ (ਵਿੱਚ_ਸੈੱਟ(raw_dib, 0, DIB_RAW_FREE))
                        ਰੋਕੋ;

                /* The buckets are not supposed to be all occupied and with DIB > 0.
                 * That would mean we could make everyone better off by shifting them
                 * backward. This scenario is impossible. */
                ਪ੍ਰਤਿਜ੍ਞਾ(ਖੱਬਾ != ਸੱਜਾ);
        }

        ਜੇ (h->type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*) h;
                ਬਣਤਰ ordered_hashmap_ਇੰਦਰਾਜ *le = ordered_ਬਾਲਟੀ_at(lh, idx);

                ਜੇ (le->iterate_ਅਗਲਾ != IDX_NIL)
                        ordered_ਬਾਲਟੀ_at(lh, le->iterate_ਅਗਲਾ)->iterate_ਪਿਛਲਾ = le->iterate_ਪਿਛਲਾ;
                ਨਹੀਂਤਾਂ
                        lh->iterate_list_ਪੂਛ = le->iterate_ਪਿਛਲਾ;

                ਜੇ (le->iterate_ਪਿਛਲਾ != IDX_NIL)
                        ordered_ਬਾਲਟੀ_at(lh, le->iterate_ਪਿਛਲਾ)->iterate_ਅਗਲਾ = le->iterate_ਅਗਲਾ;
                ਨਹੀਂਤਾਂ
                        lh->iterate_list_ਸਿਰ = le->iterate_ਅਗਲਾ;
        }

        /* Now shift all buckets in the interval (left, right) one step backwards */
        ਲਈ (prev = ਖੱਬਾ, ਖੱਬਾ = ਅਗਲਾ_idx(h, ਖੱਬਾ); ਖੱਬਾ != ਸੱਜਾ;
             prev = ਖੱਬਾ, ਖੱਬਾ = ਅਗਲਾ_idx(h, ਖੱਬਾ)) {
                dib = ਬਾਲਟੀ_calculate_dib(h, ਖੱਬਾ, dibs[ਖੱਬਾ]);
                ਪ੍ਰਤਿਜ੍ਞਾ(dib != 0);
                ਬਾਲਟੀ_move_ਇੰਦਰਾਜ(h, ਨਿਰਾ, ਖੱਬਾ, prev);
                ਬਾਲਟੀ_ਸੈੱਟ_dib(h, prev, dib - 1);
        }

        ਬਾਲਟੀ_mark_free(h, prev);
        n_entries_dec(h);
        ਅਧਾਰ_ਸੈੱਟ_dirty(h);
}
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ remove_ਇੰਦਰਾਜ(h, idx) ਅਧਾਰ_remove_ਇੰਦਰਾਜ(HASHMAP_ਅਧਾਰ(h), idx)

ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ hashmap_iterate_ਵਿੱਚ_insertion_ਕ੍ਰਮ(OrderedHashmap *h, Iterator *i) {
        ਬਣਤਰ ordered_hashmap_ਇੰਦਰਾਜ *e;
        ਬਿਨਾਨਿਸ਼ਾਨੀ idx;

        ਪ੍ਰਤਿਜ੍ਞਾ(h);
        ਪ੍ਰਤਿਜ੍ਞਾ(i);

        ਜੇ (i->idx == IDX_NIL)
                ਜਾਓ at_ਸਿਰਾ;

        ਜੇ (i->idx == IDX_ਪਹਿਲਾ && h->iterate_list_ਸਿਰ == IDX_NIL)
                ਜਾਓ at_ਸਿਰਾ;

        ਜੇ (i->idx == IDX_ਪਹਿਲਾ) {
                idx = h->iterate_list_ਸਿਰ;
                e = ordered_ਬਾਲਟੀ_at(h, idx);
        } ਨਹੀਂਤਾਂ {
                idx = i->idx;
                e = ordered_ਬਾਲਟੀ_at(h, idx);
                /*
                 * We allow removing the current entry while iterating, but removal may cause
                 * a backward shift. The next entry may thus move one bucket to the left.
                 * To detect when it happens, we remember the key pointer of the entry we were
                 * going to iterate next. If it does not match, there was a backward shift.
                 */
                ਜੇ (e->p.b.ਕੁੰਜੀ != i->ਅਗਲਾ_ਕੁੰਜੀ) {
                        idx = prev_idx(HASHMAP_ਅਧਾਰ(h), idx);
                        e = ordered_ਬਾਲਟੀ_at(h, idx);
                }
                ਪ੍ਰਤਿਜ੍ਞਾ(e->p.b.ਕੁੰਜੀ == i->ਅਗਲਾ_ਕੁੰਜੀ);
        }

#ਜੇ ENABLE_DEBUG_HASHMAP
        i->prev_idx = idx;
#ਜੇ_ਅੰਤ

        ਜੇ (e->iterate_ਅਗਲਾ != IDX_NIL) {
                ਬਣਤਰ ordered_hashmap_ਇੰਦਰਾਜ *n;
                i->idx = e->iterate_ਅਗਲਾ;
                n = ordered_ਬਾਲਟੀ_at(h, i->idx);
                i->ਅਗਲਾ_ਕੁੰਜੀ = n->p.b.ਕੁੰਜੀ;
        } ਨਹੀਂਤਾਂ
                i->idx = IDX_NIL;

        ਵਾਪਸ idx;

at_ਸਿਰਾ:
        i->idx = IDX_NIL;
        ਵਾਪਸ IDX_NIL;
}

ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ hashmap_iterate_ਵਿੱਚ_ਅੰਦਰੂਨੀ_ਕ੍ਰਮ(Hashmap_ਅਧਾਰ *h, Iterator *i) {
        ਬਿਨਾਨਿਸ਼ਾਨੀ idx;

        ਪ੍ਰਤਿਜ੍ਞਾ(h);
        ਪ੍ਰਤਿਜ੍ਞਾ(i);

        ਜੇ (i->idx == IDX_NIL)
                ਜਾਓ at_ਸਿਰਾ;

        ਜੇ (i->idx == IDX_ਪਹਿਲਾ) {
                /* fast forward to the first occupied bucket */
                ਜੇ (h->has_ਅਸਿੱਧਾ) {
                        i->idx = skip_free_buckets(h, h->ਅਸਿੱਧਾ.idx_ਸਭ_ਤੋਂ_ਨੀਵਾਂ_ਇੰਦਰਾਜ);
                        h->ਅਸਿੱਧਾ.idx_ਸਭ_ਤੋਂ_ਨੀਵਾਂ_ਇੰਦਰਾਜ = i->idx;
                } ਨਹੀਂਤਾਂ
                        i->idx = skip_free_buckets(h, 0);

                ਜੇ (i->idx == IDX_NIL)
                        ਜਾਓ at_ਸਿਰਾ;
        } ਨਹੀਂਤਾਂ {
                ਬਣਤਰ hashmap_ਅਧਾਰ_ਇੰਦਰਾਜ *e;

                ਪ੍ਰਤਿਜ੍ਞਾ(i->idx > 0);

                e = ਬਾਲਟੀ_at(h, i->idx);
                /*
                 * We allow removing the current entry while iterating, but removal may cause
                 * a backward shift. The next entry may thus move one bucket to the left.
                 * To detect when it happens, we remember the key pointer of the entry we were
                 * going to iterate next. If it does not match, there was a backward shift.
                 */
                ਜੇ (e->ਕੁੰਜੀ != i->ਅਗਲਾ_ਕੁੰਜੀ)
                        e = ਬਾਲਟੀ_at(h, --i->idx);

                ਪ੍ਰਤਿਜ੍ਞਾ(e->ਕੁੰਜੀ == i->ਅਗਲਾ_ਕੁੰਜੀ);
        }

        idx = i->idx;
#ਜੇ ENABLE_DEBUG_HASHMAP
        i->prev_idx = idx;
#ਜੇ_ਅੰਤ

        i->idx = skip_free_buckets(h, i->idx + 1);
        ਜੇ (i->idx != IDX_NIL)
                i->ਅਗਲਾ_ਕੁੰਜੀ = ਬਾਲਟੀ_at(h, i->idx)->ਕੁੰਜੀ;
        ਨਹੀਂਤਾਂ
                i->idx = IDX_NIL;

        ਵਾਪਸ idx;

at_ਸਿਰਾ:
        i->idx = IDX_NIL;
        ਵਾਪਸ IDX_NIL;
}

ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ hashmap_iterate_ਇੰਦਰਾਜ(Hashmap_ਅਧਾਰ *h, Iterator *i) {
        ਜੇ (!h) {
                i->idx = IDX_NIL;
                ਵਾਪਸ IDX_NIL;
        }

#ਜੇ ENABLE_DEBUG_HASHMAP
        ਜੇ (i->idx == IDX_ਪਹਿਲਾ) {
                i->put_ਗਿਣਤੀ = h->debug.put_ਗਿਣਤੀ;
                i->rem_ਗਿਣਤੀ = h->debug.rem_ਗਿਣਤੀ;
        } ਨਹੀਂਤਾਂ {
                /* While iterating, must not add any new entries */
                ਪ੍ਰਤਿਜ੍ਞਾ(i->put_ਗਿਣਤੀ == h->debug.put_ਗਿਣਤੀ);
                /* ... or remove entries other than the current one */
                ਪ੍ਰਤਿਜ੍ਞਾ(i->rem_ਗਿਣਤੀ == h->debug.rem_ਗਿਣਤੀ ||
                       (i->rem_ਗਿਣਤੀ == h->debug.rem_ਗਿਣਤੀ - 1 &&
                        i->prev_idx == h->debug.ਆਖਰੀ_rem_idx));
                /* Reset our removals counter */
                i->rem_ਗਿਣਤੀ = h->debug.rem_ਗਿਣਤੀ;
        }
#ਜੇ_ਅੰਤ

        ਵਾਪਸ h->type == HASHMAP_TYPE_ORDERED ? hashmap_iterate_ਵਿੱਚ_insertion_ਕ੍ਰਮ((OrderedHashmap*) h, i)
                                               : hashmap_iterate_ਵਿੱਚ_ਅੰਦਰੂਨੀ_ਕ੍ਰਮ(h, i);
}

bool _hashmap_iterate(Hashmap_ਅਧਾਰ *h, Iterator *i, ਖਾਲੀ **ਮੁੱਲ, ਅਚਲ ਖਾਲੀ **ਕੁੰਜੀ) {
        ਬਣਤਰ hashmap_ਅਧਾਰ_ਇੰਦਰਾਜ *e;
        ਖਾਲੀ *ਅੰਕੜੇ;
        ਬਿਨਾਨਿਸ਼ਾਨੀ idx;

        idx = hashmap_iterate_ਇੰਦਰਾਜ(h, i);
        ਜੇ (idx == IDX_NIL) {
                ਜੇ (ਮੁੱਲ)
                        *ਮੁੱਲ = ਨਿਰਾ;
                ਜੇ (ਕੁੰਜੀ)
                        *ਕੁੰਜੀ = ਨਿਰਾ;

                ਵਾਪਸ false;
        }

        e = ਬਾਲਟੀ_at(h, idx);
        ਅੰਕੜੇ = ਇੰਦਰਾਜ_ਮੁੱਲ(h, e);
        ਜੇ (ਮੁੱਲ)
                *ਮੁੱਲ = ਅੰਕੜੇ;
        ਜੇ (ਕੁੰਜੀ)
                *ਕੁੰਜੀ = e->ਕੁੰਜੀ;

        ਵਾਪਸ true;
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HASHMAP_FOREACH_IDX(idx, h, i) \
        ਲਈ ((i) = ITERATOR_ਪਹਿਲਾ, (idx) = hashmap_iterate_ਇੰਦਰਾਜ((h), &(i)); \
             (idx != IDX_NIL); \
             (idx) = hashmap_iterate_ਇੰਦਰਾਜ((h), &(i)))

IteratedCache* _hashmap_iterated_cache_new(Hashmap_ਅਧਾਰ *h) {
        IteratedCache *cache;

        ਪ੍ਰਤਿਜ੍ਞਾ(h);
        ਪ੍ਰਤਿਜ੍ਞਾ(!h->cached);

        ਜੇ (h->cached)
                ਵਾਪਸ ਨਿਰਾ;

        cache = new0(IteratedCache, 1);
        ਜੇ (!cache)
                ਵਾਪਸ ਨਿਰਾ;

        cache->hashmap = h;
        h->cached = true;

        ਵਾਪਸ cache;
}

ਸਥਿਰ ਖਾਲੀ reset_ਸਿੱਧਾ_storage(Hashmap_ਅਧਾਰ *h) {
        ਅਚਲ ਬਣਤਰ hashmap_type_info *hi = &hashmap_type_info[h->type];
        ਖਾਲੀ *p;

        ਪ੍ਰਤਿਜ੍ਞਾ(!h->has_ਅਸਿੱਧਾ);

        p = mempset(h->ਸਿੱਧਾ.storage, 0, hi->ਇੰਦਰਾਜ_ਮਾਪ * hi->n_ਸਿੱਧਾ_buckets);
        memset(p, DIB_RAW_INIT, ਆਕਾਰ(dib_raw_t) * hi->n_ਸਿੱਧਾ_buckets);
}

ਸਥਿਰ ਖਾਲੀ shared_hash_ਕੁੰਜੀ_initialize(ਖਾਲੀ) {
        random_ਬਾਈਟਾਂ(shared_hash_ਕੁੰਜੀ, ਆਕਾਰ(shared_hash_ਕੁੰਜੀ));
}

ਸਥਿਰ ਬਣਤਰ Hashmap_ਅਧਾਰ* hashmap_ਅਧਾਰ_new(ਅਚਲ ਬਣਤਰ hash_ops *hash_ops, ਗਿਣਤੀ HashmapType type) {
        Hashmap_ਅਧਾਰ *h;
        ਅਚਲ ਬਣਤਰ hashmap_type_info *hi = &hashmap_type_info[type];

        bool use_pool = mempool_enabled && mempool_enabled();  /* mempool_enabled is a weak symbol */

        h = use_pool ? mempool_alloc0_tile(hi->mempool) : malloc0(hi->ਸਿਰ_ਮਾਪ);
        ਜੇ (!h)
                ਵਾਪਸ ਨਿਰਾ;

        h->type = type;
        h->from_pool = use_pool;
        h->hash_ops = hash_ops ?: &trivial_hash_ops;

        ਜੇ (type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*)h;
                lh->iterate_list_ਸਿਰ = lh->iterate_list_ਪੂਛ = IDX_NIL;
        }

        reset_ਸਿੱਧਾ_storage(h);

        ਸਥਿਰ pthread_once_t once = PTHREAD_ONCE_INIT;
        assert_se(pthread_once(&once, shared_hash_ਕੁੰਜੀ_initialize) == 0);

#ਜੇ ENABLE_DEBUG_HASHMAP
        assert_se(pthread_mutex_lock(&hashmap_debug_list_mutex) == 0);
        LIST_PREPEND(debug_list, hashmap_debug_list, &h->debug);
        assert_se(pthread_mutex_unlock(&hashmap_debug_list_mutex) == 0);
#ਜੇ_ਅੰਤ

        ਵਾਪਸ h;
}

Hashmap *hashmap_new(ਅਚਲ ਬਣਤਰ hash_ops *hash_ops) {
        ਵਾਪਸ (Hashmap*)        hashmap_ਅਧਾਰ_new(hash_ops, HASHMAP_TYPE_PLAIN);
}

OrderedHashmap *ordered_hashmap_new(ਅਚਲ ਬਣਤਰ hash_ops *hash_ops) {
        ਵਾਪਸ (OrderedHashmap*) hashmap_ਅਧਾਰ_new(hash_ops, HASHMAP_TYPE_ORDERED);
}

ਸੈੱਟ_2 *ਸੈੱਟ_new(ਅਚਲ ਬਣਤਰ hash_ops *hash_ops) {
        ਵਾਪਸ (ਸੈੱਟ_2*)            hashmap_ਅਧਾਰ_new(hash_ops, HASHMAP_TYPE_ਸੈੱਟ);
}

ਸਥਿਰ ਪੂਰਨ hashmap_ਅਧਾਰ_ensure_allocated(Hashmap_ਅਧਾਰ **h, ਅਚਲ ਬਣਤਰ hash_ops *hash_ops,
                                         ਗਿਣਤੀ HashmapType type) {
        Hashmap_ਅਧਾਰ *q;

        ਪ੍ਰਤਿਜ੍ਞਾ(h);

        ਜੇ (*h) {
                ਪ੍ਰਤਿਜ੍ਞਾ((*h)->hash_ops == (hash_ops ?: &trivial_hash_ops));
                ਵਾਪਸ 0;
        }

        q = hashmap_ਅਧਾਰ_new(hash_ops, type);
        ਜੇ (!q)
                ਵਾਪਸ -ENOMEM;

        *h = q;
        ਵਾਪਸ 1;
}

ਪੂਰਨ hashmap_ensure_allocated(Hashmap **h, ਅਚਲ ਬਣਤਰ hash_ops *hash_ops) {
        ਵਾਪਸ hashmap_ਅਧਾਰ_ensure_allocated((Hashmap_ਅਧਾਰ**)h, hash_ops, HASHMAP_TYPE_PLAIN);
}

ਪੂਰਨ ordered_hashmap_ensure_allocated(OrderedHashmap **h, ਅਚਲ ਬਣਤਰ hash_ops *hash_ops) {
        ਵਾਪਸ hashmap_ਅਧਾਰ_ensure_allocated((Hashmap_ਅਧਾਰ**)h, hash_ops, HASHMAP_TYPE_ORDERED);
}

ਪੂਰਨ ਸੈੱਟ_ensure_allocated(ਸੈੱਟ_2 **s, ਅਚਲ ਬਣਤਰ hash_ops *hash_ops) {
        ਵਾਪਸ hashmap_ਅਧਾਰ_ensure_allocated((Hashmap_ਅਧਾਰ**)s, hash_ops, HASHMAP_TYPE_ਸੈੱਟ);
}

ਪੂਰਨ hashmap_ensure_put(Hashmap **h, ਅਚਲ ਬਣਤਰ hash_ops *hash_ops, ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ, ਖਾਲੀ *ਮੁੱਲ) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(h);

        r = hashmap_ensure_allocated(h, hash_ops);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ hashmap_put(*h, ਕੁੰਜੀ, ਮੁੱਲ);
}

ਪੂਰਨ ordered_hashmap_ensure_put(OrderedHashmap **h, ਅਚਲ ਬਣਤਰ hash_ops *hash_ops, ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ, ਖਾਲੀ *ਮੁੱਲ) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(h);

        r = ordered_hashmap_ensure_allocated(h, hash_ops);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ ordered_hashmap_put(*h, ਕੁੰਜੀ, ਮੁੱਲ);
}

ਪੂਰਨ ordered_hashmap_ensure_replace(OrderedHashmap **h, ਅਚਲ ਬਣਤਰ hash_ops *hash_ops, ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ, ਖਾਲੀ *ਮੁੱਲ) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(h);

        r = ordered_hashmap_ensure_allocated(h, hash_ops);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ ordered_hashmap_replace(*h, ਕੁੰਜੀ, ਮੁੱਲ);
}

ਪੂਰਨ hashmap_ensure_replace(Hashmap **h, ਅਚਲ ਬਣਤਰ hash_ops *hash_ops, ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ, ਖਾਲੀ *ਮੁੱਲ) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(h);

        r = hashmap_ensure_allocated(h, hash_ops);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ hashmap_replace(*h, ਕੁੰਜੀ, ਮੁੱਲ);
}

ਸਥਿਰ ਖਾਲੀ hashmap_free_no_clear(Hashmap_ਅਧਾਰ *h) {
        ਪ੍ਰਤਿਜ੍ਞਾ(!h->has_ਅਸਿੱਧਾ);
        ਪ੍ਰਤਿਜ੍ਞਾ(h->n_ਸਿੱਧਾ_entries == 0);

#ਜੇ ENABLE_DEBUG_HASHMAP
        assert_se(pthread_mutex_lock(&hashmap_debug_list_mutex) == 0);
        LIST_REMOVE(debug_list, hashmap_debug_list, &h->debug);
        assert_se(pthread_mutex_unlock(&hashmap_debug_list_mutex) == 0);
#ਜੇ_ਅੰਤ

        ਜੇ (h->from_pool) {
                /* Ensure that the object didn't get migrated between threads. */
                assert_se(is_main_thread());
                mempool_free_tile(hashmap_type_info[h->type].mempool, h);
        } ਨਹੀਂਤਾਂ
                ਮੁਕਤ(h);
}

Hashmap_ਅਧਾਰ* _hashmap_free(Hashmap_ਅਧਾਰ *h) {
        ਜੇ (h) {
                _hashmap_clear(h);
                hashmap_free_no_clear(h);
        }

        ਵਾਪਸ ਨਿਰਾ;
}

ਖਾਲੀ _hashmap_clear(Hashmap_ਅਧਾਰ *h) {
        ਜੇ (!h)
                ਵਾਪਸ;

        ਜੇ (h->hash_ops->free_ਕੁੰਜੀ || h->hash_ops->free_ਮੁੱਲ) {

                /* If destructor calls are defined, let's destroy things defensively: let's take the item out of the
                 * hash table, and only then call the destructor functions. If these destructors then try to unregister
                 * themselves from our hash table a second time, the entry is already gone. */

                ਜਦੋਂ (hashmap_ਮਾਪ(h) > 0) {
                        ਖਾਲੀ *k = ਨਿਰਾ;
                        ਖਾਲੀ *v;

                        v = hashmap_ਪਹਿਲਾ_ਕੁੰਜੀ_and_ਮੁੱਲ(h, true, &k);

                        ਜੇ (h->hash_ops->free_ਕੁੰਜੀ)
                                h->hash_ops->free_ਕੁੰਜੀ(k);

                        ਜੇ (h->hash_ops->free_ਮੁੱਲ)
                                h->hash_ops->free_ਮੁੱਲ(v);
                }
        }

        ਜੇ (h->has_ਅਸਿੱਧਾ) {
                ਮੁਕਤ(h->ਅਸਿੱਧਾ.storage);
                h->has_ਅਸਿੱਧਾ = false;
        }

        h->n_ਸਿੱਧਾ_entries = 0;
        reset_ਸਿੱਧਾ_storage(h);

        ਜੇ (h->type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*) h;
                lh->iterate_list_ਸਿਰ = lh->iterate_list_ਪੂਛ = IDX_NIL;
        }

        ਅਧਾਰ_ਸੈੱਟ_dirty(h);
}

ਸਥਿਰ ਪੂਰਨ resize_buckets(Hashmap_ਅਧਾਰ *h, ਬਿਨਾਨਿਸ਼ਾਨੀ entries_ਜੋੜੋ);

/*
 * Finds an empty bucket to put an entry into, starting the scan at 'idx'.
 * Performs Robin Hood swaps as it goes. The entry to put must be placed
 * by the caller into swap slot IDX_PUT.
 * If used for in-place resizing, may leave a displaced entry in swap slot
 * IDX_PUT. Caller must rehash it next.
 * Returns: true if it left a displaced entry to rehash next in IDX_PUT,
 *          false otherwise.
 */
ਸਥਿਰ bool hashmap_put_robin_hood(Hashmap_ਅਧਾਰ *h, ਬਿਨਾਨਿਸ਼ਾਨੀ idx,
                                   ਬਣਤਰ ਅਦਲਾ_ਬਦਲੀ_entries *ਅਦਲਾ_ਬਦਲੀ) {
        dib_raw_t raw_dib, *dibs;
        ਬਿਨਾਨਿਸ਼ਾਨੀ dib, ਦੂਰੀ;

#ਜੇ ENABLE_DEBUG_HASHMAP
        ਪ੍ਰਤਿਜ੍ਞਾ(h);
        h->debug.put_ਗਿਣਤੀ++;
#ਜੇ_ਅੰਤ

        dibs = dib_raw_ptr(h);

        ਲਈ (ਦੂਰੀ = 0; ; ਦੂਰੀ++) {
                raw_dib = dibs[idx];
                ਜੇ (ਵਿੱਚ_ਸੈੱਟ(raw_dib, DIB_RAW_FREE, DIB_RAW_REHASH)) {
                        ਜੇ (raw_dib == DIB_RAW_REHASH)
                                ਬਾਲਟੀ_move_ਇੰਦਰਾਜ(h, ਅਦਲਾ_ਬਦਲੀ, idx, IDX_TMP);

                        ਜੇ (h->has_ਅਸਿੱਧਾ && h->ਅਸਿੱਧਾ.idx_ਸਭ_ਤੋਂ_ਨੀਵਾਂ_ਇੰਦਰਾਜ > idx)
                                h->ਅਸਿੱਧਾ.idx_ਸਭ_ਤੋਂ_ਨੀਵਾਂ_ਇੰਦਰਾਜ = idx;

                        ਬਾਲਟੀ_ਸੈੱਟ_dib(h, idx, ਦੂਰੀ);
                        ਬਾਲਟੀ_move_ਇੰਦਰਾਜ(h, ਅਦਲਾ_ਬਦਲੀ, IDX_PUT, idx);
                        ਜੇ (raw_dib == DIB_RAW_REHASH) {
                                ਬਾਲਟੀ_move_ਇੰਦਰਾਜ(h, ਅਦਲਾ_ਬਦਲੀ, IDX_TMP, IDX_PUT);
                                ਵਾਪਸ true;
                        }

                        ਵਾਪਸ false;
                }

                dib = ਬਾਲਟੀ_calculate_dib(h, idx, raw_dib);

                ਜੇ (dib < ਦੂਰੀ) {
                        /* Found a wealthier entry. Go Robin Hood! */
                        ਬਾਲਟੀ_ਸੈੱਟ_dib(h, idx, ਦੂਰੀ);

                        /* swap the entries */
                        ਬਾਲਟੀ_move_ਇੰਦਰਾਜ(h, ਅਦਲਾ_ਬਦਲੀ, idx, IDX_TMP);
                        ਬਾਲਟੀ_move_ਇੰਦਰਾਜ(h, ਅਦਲਾ_ਬਦਲੀ, IDX_PUT, idx);
                        ਬਾਲਟੀ_move_ਇੰਦਰਾਜ(h, ਅਦਲਾ_ਬਦਲੀ, IDX_TMP, IDX_PUT);

                        ਦੂਰੀ = dib;
                }

                idx = ਅਗਲਾ_idx(h, idx);
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
ਸਥਿਰ ਪੂਰਨ hashmap_ਅਧਾਰ_put_boldly(Hashmap_ਅਧਾਰ *h, ਬਿਨਾਨਿਸ਼ਾਨੀ idx,
                                   ਬਣਤਰ ਅਦਲਾ_ਬਦਲੀ_entries *ਅਦਲਾ_ਬਦਲੀ, bool may_resize) {
        ਬਣਤਰ ordered_hashmap_ਇੰਦਰਾਜ *new_ਇੰਦਰਾਜ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(idx < n_buckets(h));

        new_ਇੰਦਰਾਜ = ਬਾਲਟੀ_at_ਅਦਲਾ_ਬਦਲੀ(ਅਦਲਾ_ਬਦਲੀ, IDX_PUT);

        ਜੇ (may_resize) {
                r = resize_buckets(h, 1);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
                ਜੇ (r > 0)
                        idx = ਬਾਲਟੀ_hash(h, new_ਇੰਦਰਾਜ->p.b.ਕੁੰਜੀ);
        }
        ਪ੍ਰਤਿਜ੍ਞਾ(n_entries(h) < n_buckets(h));

        ਜੇ (h->type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*) h;

                new_ਇੰਦਰਾਜ->iterate_ਅਗਲਾ = IDX_NIL;
                new_ਇੰਦਰਾਜ->iterate_ਪਿਛਲਾ = lh->iterate_list_ਪੂਛ;

                ਜੇ (lh->iterate_list_ਪੂਛ != IDX_NIL) {
                        ਬਣਤਰ ordered_hashmap_ਇੰਦਰਾਜ *old_ਪੂਛ;

                        old_ਪੂਛ = ordered_ਬਾਲਟੀ_at(lh, lh->iterate_list_ਪੂਛ);
                        ਪ੍ਰਤਿਜ੍ਞਾ(old_ਪੂਛ->iterate_ਅਗਲਾ == IDX_NIL);
                        old_ਪੂਛ->iterate_ਅਗਲਾ = IDX_PUT;
                }

                lh->iterate_list_ਪੂਛ = IDX_PUT;
                ਜੇ (lh->iterate_list_ਸਿਰ == IDX_NIL)
                        lh->iterate_list_ਸਿਰ = IDX_PUT;
        }

        assert_se(hashmap_put_robin_hood(h, idx, ਅਦਲਾ_ਬਦਲੀ) == false);

        n_entries_inc(h);
#ਜੇ ENABLE_DEBUG_HASHMAP
        h->debug.max_entries = MAX(h->debug.max_entries, n_entries(h));
#ਜੇ_ਅੰਤ

        ਅਧਾਰ_ਸੈੱਟ_dirty(h);

        ਵਾਪਸ 1;
}
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ hashmap_put_boldly(h, idx, ਅਦਲਾ_ਬਦਲੀ, may_resize) \
        hashmap_ਅਧਾਰ_put_boldly(HASHMAP_ਅਧਾਰ(h), idx, ਅਦਲਾ_ਬਦਲੀ, may_resize)

/*
 * Returns 0 if resize is not needed.
 *         1 if successfully resized.
 *         -ENOMEM on allocation failure.
 */
ਸਥਿਰ ਪੂਰਨ resize_buckets(Hashmap_ਅਧਾਰ *h, ਬਿਨਾਨਿਸ਼ਾਨੀ entries_ਜੋੜੋ) {
        ਬਣਤਰ ਅਦਲਾ_ਬਦਲੀ_entries ਅਦਲਾ_ਬਦਲੀ;
        ਖਾਲੀ *new_storage;
        dib_raw_t *old_dibs, *new_dibs;
        ਅਚਲ ਬਣਤਰ hashmap_type_info *hi;
        ਬਿਨਾਨਿਸ਼ਾਨੀ idx, optimal_idx;
        ਬਿਨਾਨਿਸ਼ਾਨੀ old_n_buckets, new_n_buckets, n_rehashed, new_n_entries;
        uint8_t new_ਖਿਸਕਾਓ;
        bool rehash_ਅਗਲਾ;

        ਪ੍ਰਤਿਜ੍ਞਾ(h);

        hi = &hashmap_type_info[h->type];
        new_n_entries = n_entries(h) + entries_ਜੋੜੋ;

        /* overflow? */
        ਜੇ (_unlikely_(new_n_entries < entries_ਜੋੜੋ))
                ਵਾਪਸ -ENOMEM;

        /* For direct storage we allow 100% load, because it's tiny. */
        ਜੇ (!h->has_ਅਸਿੱਧਾ && new_n_entries <= hi->n_ਸਿੱਧਾ_buckets)
                ਵਾਪਸ 0;

        /*
         * Load factor = n/m = 1 - (1/INV_KEEP_FREE).
         * From it follows: m = n + n/(INV_KEEP_FREE - 1)
         */
        new_n_buckets = new_n_entries + new_n_entries / (INV_KEEP_FREE - 1);
        /* overflow? */
        ਜੇ (_unlikely_(new_n_buckets < new_n_entries))
                ਵਾਪਸ -ENOMEM;

        old_n_buckets = n_buckets(h);

        ਜੇ (_likely_(new_n_buckets <= old_n_buckets))
                ਵਾਪਸ 0;

        ਜੇ (_unlikely_(new_n_buckets > UINT_MAX / (hi->ਇੰਦਰਾਜ_ਮਾਪ + ਆਕਾਰ(dib_raw_t))))
                ਵਾਪਸ -ENOMEM;

        new_ਖਿਸਕਾਓ = log2u_ਗੇੜਾ_ਉੱਪਰ(MAX(
                        new_n_buckets * (hi->ਇੰਦਰਾਜ_ਮਾਪ + ਆਕਾਰ(dib_raw_t)),
                        2 * ਆਕਾਰ(ਬਣਤਰ ਸਿੱਧਾ_storage)));

        /* Realloc storage (buckets and DIB array). */
        new_storage = realloc(h->has_ਅਸਿੱਧਾ ? h->ਅਸਿੱਧਾ.storage : ਨਿਰਾ,
                              1U << new_ਖਿਸਕਾਓ);
        ਜੇ (!new_storage)
                ਵਾਪਸ -ENOMEM;

        /* Must upgrade direct to indirect storage. */
        ਜੇ (!h->has_ਅਸਿੱਧਾ) {
                ਸਮ੍ਰਿਤੀ_ਨਕਲ(new_storage, h->ਸਿੱਧਾ.storage,
                       old_n_buckets * (hi->ਇੰਦਰਾਜ_ਮਾਪ + ਆਕਾਰ(dib_raw_t)));
                h->ਅਸਿੱਧਾ.n_entries = h->n_ਸਿੱਧਾ_entries;
                h->ਅਸਿੱਧਾ.idx_ਸਭ_ਤੋਂ_ਨੀਵਾਂ_ਇੰਦਰਾਜ = 0;
                h->n_ਸਿੱਧਾ_entries = 0;
        }

        /* Get a new hash key. If we've just upgraded to indirect storage,
         * allow reusing a previously generated key. It's still a different key
         * from the shared one that we used for direct storage. */
        get_hash_ਕੁੰਜੀ(h->ਅਸਿੱਧਾ.hash_ਕੁੰਜੀ, !h->has_ਅਸਿੱਧਾ);

        h->has_ਅਸਿੱਧਾ = true;
        h->ਅਸਿੱਧਾ.storage = new_storage;
        h->ਅਸਿੱਧਾ.n_buckets = (1U << new_ਖਿਸਕਾਓ) /
                                (hi->ਇੰਦਰਾਜ_ਮਾਪ + ਆਕਾਰ(dib_raw_t));

        old_dibs = (dib_raw_t*)((uint8_t*) new_storage + hi->ਇੰਦਰਾਜ_ਮਾਪ * old_n_buckets);
        new_dibs = dib_raw_ptr(h);

        /*
         * Move the DIB array to the new place, replacing valid DIB values with
         * DIB_RAW_REHASH to indicate all of the used buckets need rehashing.
         * Note: Overlap is not possible, because we have at least doubled the
         * number of buckets and dib_raw_t is smaller than any entry type.
         */
        ਲਈ (idx = 0; idx < old_n_buckets; idx++) {
                ਪ੍ਰਤਿਜ੍ਞਾ(old_dibs[idx] != DIB_RAW_REHASH);
                new_dibs[idx] = old_dibs[idx] == DIB_RAW_FREE ? DIB_RAW_FREE
                                                              : DIB_RAW_REHASH;
        }

        /* Zero the area of newly added entries (including the old DIB area) */
        memzero(ਬਾਲਟੀ_at(h, old_n_buckets),
               (n_buckets(h) - old_n_buckets) * hi->ਇੰਦਰਾਜ_ਮਾਪ);

        /* The upper half of the new DIB array needs initialization */
        memset(&new_dibs[old_n_buckets], DIB_RAW_INIT,
               (n_buckets(h) - old_n_buckets) * ਆਕਾਰ(dib_raw_t));

        /* Rehash entries that need it */
        n_rehashed = 0;
        ਲਈ (idx = 0; idx < old_n_buckets; idx++) {
                ਜੇ (new_dibs[idx] != DIB_RAW_REHASH)
                        ਜਾਰੀ;

                optimal_idx = ਬਾਲਟੀ_hash(h, ਬਾਲਟੀ_at(h, idx)->ਕੁੰਜੀ);

                /*
                 * Not much to do if by luck the entry hashes to its current
                 * location. Just set its DIB.
                 */
                ਜੇ (optimal_idx == idx) {
                        new_dibs[idx] = 0;
                        n_rehashed++;
                        ਜਾਰੀ;
                }

                new_dibs[idx] = DIB_RAW_FREE;
                ਬਾਲਟੀ_move_ਇੰਦਰਾਜ(h, &ਅਦਲਾ_ਬਦਲੀ, idx, IDX_PUT);
                /* bucket_move_entry does not clear the source */
                memzero(ਬਾਲਟੀ_at(h, idx), hi->ਇੰਦਰਾਜ_ਮਾਪ);

                ਕਰੋ {
                        /*
                         * Find the new bucket for the current entry. This may make
                         * another entry homeless and load it into IDX_PUT.
                         */
                        rehash_ਅਗਲਾ = hashmap_put_robin_hood(h, optimal_idx, &ਅਦਲਾ_ਬਦਲੀ);
                        n_rehashed++;

                        /* Did the current entry displace another one? */
                        ਜੇ (rehash_ਅਗਲਾ)
                                optimal_idx = ਬਾਲਟੀ_hash(h, ਬਾਲਟੀ_at_ਅਦਲਾ_ਬਦਲੀ(&ਅਦਲਾ_ਬਦਲੀ, IDX_PUT)->p.b.ਕੁੰਜੀ);
                } ਜਦੋਂ (rehash_ਅਗਲਾ);
        }

        assert_se(n_rehashed == n_entries(h));

        ਵਾਪਸ 1;
}

/*
 * Finds an entry with a matching key
 * Returns: index of the found entry, or IDX_NIL if not found.
 */
ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅਧਾਰ_ਬਾਲਟੀ_ਜਾਂਚ(Hashmap_ਅਧਾਰ *h, ਬਿਨਾਨਿਸ਼ਾਨੀ idx, ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ) {
        compare_func_t compare = h->hash_ops->compare;
        ਬਣਤਰ hashmap_ਅਧਾਰ_ਇੰਦਰਾਜ *e;
        ਬਿਨਾਨਿਸ਼ਾਨੀ dib, ਦੂਰੀ;
        dib_raw_t *dibs = dib_raw_ptr(h);

        ਪ੍ਰਤਿਜ੍ਞਾ(idx < n_buckets(h));

        ਲਈ (ਦੂਰੀ = 0; ; ਦੂਰੀ++) {
                ਜੇ (dibs[idx] == DIB_RAW_FREE)
                        ਵਾਪਸ IDX_NIL;

                dib = ਬਾਲਟੀ_calculate_dib(h, idx, dibs[idx]);

                ਜੇ (dib < ਦੂਰੀ)
                        ਵਾਪਸ IDX_NIL;
                ਜੇ (dib == ਦੂਰੀ) {
                        e = ਬਾਲਟੀ_at(h, idx);

                        /* Most maps compare keys by pointer. Do that here, without the indirect call. */
                        ਜੇ (compare == trivial_compare_func ? e->ਕੁੰਜੀ == ਕੁੰਜੀ : compare(e->ਕੁੰਜੀ, ਕੁੰਜੀ) == 0)
                                ਵਾਪਸ idx;
                }

                idx = ਅਗਲਾ_idx(h, idx);
        }
}
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਬਾਲਟੀ_ਜਾਂਚ(h, idx, ਕੁੰਜੀ) ਅਧਾਰ_ਬਾਲਟੀ_ਜਾਂਚ(HASHMAP_ਅਧਾਰ(h), idx, ਕੁੰਜੀ)

ਪੂਰਨ hashmap_put(Hashmap *h, ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ, ਖਾਲੀ *ਮੁੱਲ) {
        ਬਣਤਰ ਅਦਲਾ_ਬਦਲੀ_entries ਅਦਲਾ_ਬਦਲੀ;
        ਬਣਤਰ plain_hashmap_ਇੰਦਰਾਜ *e;
        ਬਿਨਾਨਿਸ਼ਾਨੀ hash, idx;

        ਪ੍ਰਤਿਜ੍ਞਾ(h);

        hash = ਬਾਲਟੀ_hash(h, ਕੁੰਜੀ);
        idx = ਬਾਲਟੀ_ਜਾਂਚ(h, hash, ਕੁੰਜੀ);
        ਜੇ (idx != IDX_NIL) {
                e = plain_ਬਾਲਟੀ_at(h, idx);
                ਜੇ (e->ਮੁੱਲ == ਮੁੱਲ)
                        ਵਾਪਸ 0;
                ਵਾਪਸ -EEXIST;
        }

        e = &ਬਾਲਟੀ_at_ਅਦਲਾ_ਬਦਲੀ(&ਅਦਲਾ_ਬਦਲੀ, IDX_PUT)->p;
        e->b.ਕੁੰਜੀ = ਕੁੰਜੀ;
        e->ਮੁੱਲ = ਮੁੱਲ;
        ਵਾਪਸ hashmap_put_boldly(h, hash, &ਅਦਲਾ_ਬਦਲੀ, true);
}

ਪੂਰਨ ਸੈੱਟ_put(ਸੈੱਟ_2 *s, ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ) {
        ਬਣਤਰ ਅਦਲਾ_ਬਦਲੀ_entries ਅਦਲਾ_ਬਦਲੀ;
        ਬਣਤਰ hashmap_ਅਧਾਰ_ਇੰਦਰਾਜ *e;
        ਬਿਨਾਨਿਸ਼ਾਨੀ hash, idx;

        ਪ੍ਰਤਿਜ੍ਞਾ(s);

        hash = ਬਾਲਟੀ_hash(s, ਕੁੰਜੀ);
        idx = ਬਾਲਟੀ_ਜਾਂਚ(s, hash, ਕੁੰਜੀ);
        ਜੇ (idx != IDX_NIL)
                ਵਾਪਸ 0;

        e = &ਬਾਲਟੀ_at_ਅਦਲਾ_ਬਦਲੀ(&ਅਦਲਾ_ਬਦਲੀ, IDX_PUT)->p.b;
        e->ਕੁੰਜੀ = ਕੁੰਜੀ;
        ਵਾਪਸ hashmap_put_boldly(s, hash, &ਅਦਲਾ_ਬਦਲੀ, true);
}

ਪੂਰਨ ਸੈੱਟ_ensure_put(ਸੈੱਟ_2 **s, ਅਚਲ ਬਣਤਰ hash_ops *hash_ops, ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(s);

        r = ਸੈੱਟ_ensure_allocated(s, hash_ops);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ ਸੈੱਟ_put(*s, ਕੁੰਜੀ);
}

ਪੂਰਨ ਸੈੱਟ_ensure_consume(ਸੈੱਟ_2 **s, ਅਚਲ ਬਣਤਰ hash_ops *hash_ops, ਖਾਲੀ *ਕੁੰਜੀ) {
        ਪੂਰਨ r;

        r = ਸੈੱਟ_ensure_put(s, hash_ops, ਕੁੰਜੀ);
        ਜੇ (r <= 0) {
                ਜੇ (hash_ops && hash_ops->free_ਕੁੰਜੀ)
                        hash_ops->free_ਕੁੰਜੀ(ਕੁੰਜੀ);
                ਨਹੀਂਤਾਂ ਜੇ (hash_ops && hash_ops->free_ਮੁੱਲ)
                        /* Sets store their element in the key slot but may carry a value destructor. */
                        hash_ops->free_ਮੁੱਲ(ਕੁੰਜੀ);
                ਨਹੀਂਤਾਂ
                        ਮੁਕਤ(ਕੁੰਜੀ);
        }

        ਵਾਪਸ r;
}

ਪੂਰਨ hashmap_replace(Hashmap *h, ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ, ਖਾਲੀ *ਮੁੱਲ) {
        ਬਣਤਰ ਅਦਲਾ_ਬਦਲੀ_entries ਅਦਲਾ_ਬਦਲੀ;
        ਬਣਤਰ plain_hashmap_ਇੰਦਰਾਜ *e;
        ਬਿਨਾਨਿਸ਼ਾਨੀ hash, idx;

        ਪ੍ਰਤਿਜ੍ਞਾ(h);

        hash = ਬਾਲਟੀ_hash(h, ਕੁੰਜੀ);
        idx = ਬਾਲਟੀ_ਜਾਂਚ(h, hash, ਕੁੰਜੀ);
        ਜੇ (idx != IDX_NIL) {
                e = plain_ਬਾਲਟੀ_at(h, idx);
#ਜੇ ENABLE_DEBUG_HASHMAP
                /* Although the key is equal, the key pointer may have changed,
                 * and this would break our assumption for iterating. So count
                 * this operation as incompatible with iteration. */
                ਜੇ (e->b.ਕੁੰਜੀ != ਕੁੰਜੀ) {
                        h->b.debug.put_ਗਿਣਤੀ++;
                        h->b.debug.rem_ਗਿਣਤੀ++;
                        h->b.debug.ਆਖਰੀ_rem_idx = idx;
                }
#ਜੇ_ਅੰਤ
                e->b.ਕੁੰਜੀ = ਕੁੰਜੀ;
                e->ਮੁੱਲ = ਮੁੱਲ;
                hashmap_ਸੈੱਟ_dirty(h);

                ਵਾਪਸ 0;
        }

        e = &ਬਾਲਟੀ_at_ਅਦਲਾ_ਬਦਲੀ(&ਅਦਲਾ_ਬਦਲੀ, IDX_PUT)->p;
        e->b.ਕੁੰਜੀ = ਕੁੰਜੀ;
        e->ਮੁੱਲ = ਮੁੱਲ;
        ਵਾਪਸ hashmap_put_boldly(h, hash, &ਅਦਲਾ_ਬਦਲੀ, true);
}

ਪੂਰਨ hashmap_update(Hashmap *h, ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ, ਖਾਲੀ *ਮੁੱਲ) {
        ਬਣਤਰ plain_hashmap_ਇੰਦਰਾਜ *e;
        ਬਿਨਾਨਿਸ਼ਾਨੀ hash, idx;

        ਪ੍ਰਤਿਜ੍ਞਾ(h);

        hash = ਬਾਲਟੀ_hash(h, ਕੁੰਜੀ);
        idx = ਬਾਲਟੀ_ਜਾਂਚ(h, hash, ਕੁੰਜੀ);
        ਜੇ (idx == IDX_NIL)
                ਵਾਪਸ -ENOENT;

        e = plain_ਬਾਲਟੀ_at(h, idx);
        e->ਮੁੱਲ = ਮੁੱਲ;
        hashmap_ਸੈੱਟ_dirty(h);

        ਵਾਪਸ 0;
}

ਖਾਲੀ* _hashmap_get(Hashmap_ਅਧਾਰ *h, ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ) {
        ਬਣਤਰ hashmap_ਅਧਾਰ_ਇੰਦਰਾਜ *e;
        ਬਿਨਾਨਿਸ਼ਾਨੀ hash, idx;

        ਜੇ (!h)
                ਵਾਪਸ ਨਿਰਾ;

        hash = ਬਾਲਟੀ_hash(h, ਕੁੰਜੀ);
        idx = ਬਾਲਟੀ_ਜਾਂਚ(h, hash, ਕੁੰਜੀ);
        ਜੇ (idx == IDX_NIL)
                ਵਾਪਸ ਨਿਰਾ;

        e = ਬਾਲਟੀ_at(h, idx);
        ਵਾਪਸ ਇੰਦਰਾਜ_ਮੁੱਲ(h, e);
}

ਖਾਲੀ* hashmap_get2(Hashmap *h, ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ, ਖਾਲੀ **ret) {
        ਬਣਤਰ plain_hashmap_ਇੰਦਰਾਜ *e;
        ਬਿਨਾਨਿਸ਼ਾਨੀ hash, idx;

        ਜੇ (!h)
                ਵਾਪਸ ਨਿਰਾ;

        hash = ਬਾਲਟੀ_hash(h, ਕੁੰਜੀ);
        idx = ਬਾਲਟੀ_ਜਾਂਚ(h, hash, ਕੁੰਜੀ);
        ਜੇ (idx == IDX_NIL)
                ਵਾਪਸ ਨਿਰਾ;

        e = plain_ਬਾਲਟੀ_at(h, idx);
        ਜੇ (ret)
                *ret = (ਖਾਲੀ*) e->b.ਕੁੰਜੀ;

        ਵਾਪਸ e->ਮੁੱਲ;
}

bool hashmap_ਸ਼ਾਮਲ_ਹੈ_2(Hashmap_ਅਧਾਰ *h, ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ) {
        ਬਿਨਾਨਿਸ਼ਾਨੀ hash;

        ਜੇ (!h)
                ਵਾਪਸ false;

        hash = ਬਾਲਟੀ_hash(h, ਕੁੰਜੀ);
        ਵਾਪਸ ਬਾਲਟੀ_ਜਾਂਚ(h, hash, ਕੁੰਜੀ) != IDX_NIL;
}

ਖਾਲੀ* _hashmap_remove(Hashmap_ਅਧਾਰ *h, ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ) {
        ਬਣਤਰ hashmap_ਅਧਾਰ_ਇੰਦਰਾਜ *e;
        ਬਿਨਾਨਿਸ਼ਾਨੀ hash, idx;
        ਖਾਲੀ *ਅੰਕੜੇ;

        ਜੇ (!h)
                ਵਾਪਸ ਨਿਰਾ;

        hash = ਬਾਲਟੀ_hash(h, ਕੁੰਜੀ);
        idx = ਬਾਲਟੀ_ਜਾਂਚ(h, hash, ਕੁੰਜੀ);
        ਜੇ (idx == IDX_NIL)
                ਵਾਪਸ ਨਿਰਾ;

        e = ਬਾਲਟੀ_at(h, idx);
        ਅੰਕੜੇ = ਇੰਦਰਾਜ_ਮੁੱਲ(h, e);
        remove_ਇੰਦਰਾਜ(h, idx);

        ਵਾਪਸ ਅੰਕੜੇ;
}

ਖਾਲੀ* hashmap_remove2(Hashmap *h, ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ, ਖਾਲੀ **ret) {
        ਬਣਤਰ plain_hashmap_ਇੰਦਰਾਜ *e;
        ਬਿਨਾਨਿਸ਼ਾਨੀ hash, idx;
        ਖਾਲੀ *ਅੰਕੜੇ;

        ਜੇ (!h) {
                ਜੇ (ret)
                        *ret = ਨਿਰਾ;
                ਵਾਪਸ ਨਿਰਾ;
        }

        hash = ਬਾਲਟੀ_hash(h, ਕੁੰਜੀ);
        idx = ਬਾਲਟੀ_ਜਾਂਚ(h, hash, ਕੁੰਜੀ);
        ਜੇ (idx == IDX_NIL) {
                ਜੇ (ret)
                        *ret = ਨਿਰਾ;
                ਵਾਪਸ ਨਿਰਾ;
        }

        e = plain_ਬਾਲਟੀ_at(h, idx);
        ਅੰਕੜੇ = e->ਮੁੱਲ;
        ਜੇ (ret)
                *ret = (ਖਾਲੀ*) e->b.ਕੁੰਜੀ;

        remove_ਇੰਦਰਾਜ(h, idx);

        ਵਾਪਸ ਅੰਕੜੇ;
}

ਪੂਰਨ hashmap_remove_and_put(Hashmap *h, ਅਚਲ ਖਾਲੀ *old_ਕੁੰਜੀ, ਅਚਲ ਖਾਲੀ *new_ਕੁੰਜੀ, ਖਾਲੀ *ਮੁੱਲ) {
        ਬਣਤਰ ਅਦਲਾ_ਬਦਲੀ_entries ਅਦਲਾ_ਬਦਲੀ;
        ਬਣਤਰ plain_hashmap_ਇੰਦਰਾਜ *e;
        ਬਿਨਾਨਿਸ਼ਾਨੀ old_hash, new_hash, idx;

        ਜੇ (!h)
                ਵਾਪਸ -ENOENT;

        old_hash = ਬਾਲਟੀ_hash(h, old_ਕੁੰਜੀ);
        idx = ਬਾਲਟੀ_ਜਾਂਚ(h, old_hash, old_ਕੁੰਜੀ);
        ਜੇ (idx == IDX_NIL)
                ਵਾਪਸ -ENOENT;

        new_hash = ਬਾਲਟੀ_hash(h, new_ਕੁੰਜੀ);
        ਜੇ (ਬਾਲਟੀ_ਜਾਂਚ(h, new_hash, new_ਕੁੰਜੀ) != IDX_NIL)
                ਵਾਪਸ -EEXIST;

        remove_ਇੰਦਰਾਜ(h, idx);

        e = &ਬਾਲਟੀ_at_ਅਦਲਾ_ਬਦਲੀ(&ਅਦਲਾ_ਬਦਲੀ, IDX_PUT)->p;
        e->b.ਕੁੰਜੀ = new_ਕੁੰਜੀ;
        e->ਮੁੱਲ = ਮੁੱਲ;
        assert_se(hashmap_put_boldly(h, new_hash, &ਅਦਲਾ_ਬਦਲੀ, false) == 1);

        ਵਾਪਸ 0;
}

ਪੂਰਨ ਸੈੱਟ_remove_and_put(ਸੈੱਟ_2 *s, ਅਚਲ ਖਾਲੀ *old_ਕੁੰਜੀ, ਅਚਲ ਖਾਲੀ *new_ਕੁੰਜੀ) {
        ਬਣਤਰ ਅਦਲਾ_ਬਦਲੀ_entries ਅਦਲਾ_ਬਦਲੀ;
        ਬਣਤਰ hashmap_ਅਧਾਰ_ਇੰਦਰਾਜ *e;
        ਬਿਨਾਨਿਸ਼ਾਨੀ old_hash, new_hash, idx;

        ਜੇ (!s)
                ਵਾਪਸ -ENOENT;

        old_hash = ਬਾਲਟੀ_hash(s, old_ਕੁੰਜੀ);
        idx = ਬਾਲਟੀ_ਜਾਂਚ(s, old_hash, old_ਕੁੰਜੀ);
        ਜੇ (idx == IDX_NIL)
                ਵਾਪਸ -ENOENT;

        new_hash = ਬਾਲਟੀ_hash(s, new_ਕੁੰਜੀ);
        ਜੇ (ਬਾਲਟੀ_ਜਾਂਚ(s, new_hash, new_ਕੁੰਜੀ) != IDX_NIL)
                ਵਾਪਸ -EEXIST;

        remove_ਇੰਦਰਾਜ(s, idx);

        e = &ਬਾਲਟੀ_at_ਅਦਲਾ_ਬਦਲੀ(&ਅਦਲਾ_ਬਦਲੀ, IDX_PUT)->p.b;
        e->ਕੁੰਜੀ = new_ਕੁੰਜੀ;
        assert_se(hashmap_put_boldly(s, new_hash, &ਅਦਲਾ_ਬਦਲੀ, false) == 1);

        ਵਾਪਸ 0;
}

ਪੂਰਨ hashmap_remove_and_replace(Hashmap *h, ਅਚਲ ਖਾਲੀ *old_ਕੁੰਜੀ, ਅਚਲ ਖਾਲੀ *new_ਕੁੰਜੀ, ਖਾਲੀ *ਮੁੱਲ) {
        ਬਣਤਰ ਅਦਲਾ_ਬਦਲੀ_entries ਅਦਲਾ_ਬਦਲੀ;
        ਬਣਤਰ plain_hashmap_ਇੰਦਰਾਜ *e;
        ਬਿਨਾਨਿਸ਼ਾਨੀ old_hash, new_hash, idx_old, idx_new;

        ਜੇ (!h)
                ਵਾਪਸ -ENOENT;

        old_hash = ਬਾਲਟੀ_hash(h, old_ਕੁੰਜੀ);
        idx_old = ਬਾਲਟੀ_ਜਾਂਚ(h, old_hash, old_ਕੁੰਜੀ);
        ਜੇ (idx_old == IDX_NIL)
                ਵਾਪਸ -ENOENT;

        old_ਕੁੰਜੀ = ਬਾਲਟੀ_at(HASHMAP_ਅਧਾਰ(h), idx_old)->ਕੁੰਜੀ;

        new_hash = ਬਾਲਟੀ_hash(h, new_ਕੁੰਜੀ);
        idx_new = ਬਾਲਟੀ_ਜਾਂਚ(h, new_hash, new_ਕੁੰਜੀ);
        ਜੇ (idx_new != IDX_NIL)
                ਜੇ (idx_old != idx_new) {
                        remove_ਇੰਦਰਾਜ(h, idx_new);
                        /* Compensate for a possible backward shift. */
                        ਜੇ (old_ਕੁੰਜੀ != ਬਾਲਟੀ_at(HASHMAP_ਅਧਾਰ(h), idx_old)->ਕੁੰਜੀ)
                                idx_old = prev_idx(HASHMAP_ਅਧਾਰ(h), idx_old);
                        ਪ੍ਰਤਿਜ੍ਞਾ(old_ਕੁੰਜੀ == ਬਾਲਟੀ_at(HASHMAP_ਅਧਾਰ(h), idx_old)->ਕੁੰਜੀ);
                }

        remove_ਇੰਦਰਾਜ(h, idx_old);

        e = &ਬਾਲਟੀ_at_ਅਦਲਾ_ਬਦਲੀ(&ਅਦਲਾ_ਬਦਲੀ, IDX_PUT)->p;
        e->b.ਕੁੰਜੀ = new_ਕੁੰਜੀ;
        e->ਮੁੱਲ = ਮੁੱਲ;
        assert_se(hashmap_put_boldly(h, new_hash, &ਅਦਲਾ_ਬਦਲੀ, false) == 1);

        ਵਾਪਸ 0;
}

ਖਾਲੀ* hashmap_remove_ਮੁੱਲ(Hashmap_ਅਧਾਰ *h, ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ, ਖਾਲੀ *ਮੁੱਲ) {
        ਬਣਤਰ hashmap_ਅਧਾਰ_ਇੰਦਰਾਜ *e;
        ਬਿਨਾਨਿਸ਼ਾਨੀ hash, idx;

        ਜੇ (!h)
                ਵਾਪਸ ਨਿਰਾ;

        hash = ਬਾਲਟੀ_hash(h, ਕੁੰਜੀ);
        idx = ਬਾਲਟੀ_ਜਾਂਚ(h, hash, ਕੁੰਜੀ);
        ਜੇ (idx == IDX_NIL)
                ਵਾਪਸ ਨਿਰਾ;

        e = ਬਾਲਟੀ_at(h, idx);
        ਜੇ (ਇੰਦਰਾਜ_ਮੁੱਲ(h, e) != ਮੁੱਲ)
                ਵਾਪਸ ਨਿਰਾ;

        remove_ਇੰਦਰਾਜ(h, idx);

        ਵਾਪਸ ਮੁੱਲ;
}

ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੱਭੋ_ਪਹਿਲਾ_ਇੰਦਰਾਜ(Hashmap_ਅਧਾਰ *h) {
        Iterator i = ITERATOR_ਪਹਿਲਾ;

        ਜੇ (!h || !n_entries(h))
                ਵਾਪਸ IDX_NIL;

        ਵਾਪਸ hashmap_iterate_ਇੰਦਰਾਜ(h, &i);
}

ਖਾਲੀ* hashmap_ਪਹਿਲਾ_ਕੁੰਜੀ_and_ਮੁੱਲ(Hashmap_ਅਧਾਰ *h, bool remove, ਖਾਲੀ **ret_ਕੁੰਜੀ) {
        ਬਣਤਰ hashmap_ਅਧਾਰ_ਇੰਦਰਾਜ *e;
        ਖਾਲੀ *ਕੁੰਜੀ, *ਅੰਕੜੇ;
        ਬਿਨਾਨਿਸ਼ਾਨੀ idx;

        idx = ਲੱਭੋ_ਪਹਿਲਾ_ਇੰਦਰਾਜ(h);
        ਜੇ (idx == IDX_NIL) {
                ਜੇ (ret_ਕੁੰਜੀ)
                        *ret_ਕੁੰਜੀ = ਨਿਰਾ;
                ਵਾਪਸ ਨਿਰਾ;
        }

        e = ਬਾਲਟੀ_at(h, idx);
        ਕੁੰਜੀ = (ਖਾਲੀ*) e->ਕੁੰਜੀ;
        ਅੰਕੜੇ = ਇੰਦਰਾਜ_ਮੁੱਲ(h, e);

        ਜੇ (remove)
                remove_ਇੰਦਰਾਜ(h, idx);

        ਜੇ (ret_ਕੁੰਜੀ)
                *ret_ਕੁੰਜੀ = ਕੁੰਜੀ;

        ਵਾਪਸ ਅੰਕੜੇ;
}

ਬਿਨਾਨਿਸ਼ਾਨੀ hashmap_ਮਾਪ(Hashmap_ਅਧਾਰ *h) {
        ਜੇ (!h)
                ਵਾਪਸ 0;

        ਵਾਪਸ n_entries(h);
}

ਬਿਨਾਨਿਸ਼ਾਨੀ _hashmap_buckets(Hashmap_ਅਧਾਰ *h) {
        ਜੇ (!h)
                ਵਾਪਸ 0;

        ਵਾਪਸ n_buckets(h);
}

ਪੂਰਨ hashmap_ਮਿਲਾਓ(Hashmap *h, Hashmap *other) {
        Iterator i;
        ਬਿਨਾਨਿਸ਼ਾਨੀ idx;

        ਪ੍ਰਤਿਜ੍ਞਾ(h);

        HASHMAP_FOREACH_IDX(idx, HASHMAP_ਅਧਾਰ(other), i) {
                ਬਣਤਰ plain_hashmap_ਇੰਦਰਾਜ *pe = plain_ਬਾਲਟੀ_at(other, idx);
                ਪੂਰਨ r;

                r = hashmap_put(h, pe->b.ਕੁੰਜੀ, pe->ਮੁੱਲ);
                ਜੇ (r < 0 && r != -EEXIST)
                        ਵਾਪਸ r;
        }

        ਵਾਪਸ 0;
}

ਪੂਰਨ ਸੈੱਟ_ਮਿਲਾਓ(ਸੈੱਟ_2 *s, ਸੈੱਟ_2 *other) {
        Iterator i;
        ਬਿਨਾਨਿਸ਼ਾਨੀ idx;

        ਪ੍ਰਤਿਜ੍ਞਾ(s);

        HASHMAP_FOREACH_IDX(idx, HASHMAP_ਅਧਾਰ(other), i) {
                ਬਣਤਰ ਸੈੱਟ_ਇੰਦਰਾਜ *se = ਸੈੱਟ_ਬਾਲਟੀ_at(other, idx);
                ਪੂਰਨ r;

                r = ਸੈੱਟ_put(s, se->b.ਕੁੰਜੀ);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
        }

        ਵਾਪਸ 0;
}

ਪੂਰਨ _hashmap_reserve(Hashmap_ਅਧਾਰ *h, ਬਿਨਾਨਿਸ਼ਾਨੀ entries_ਜੋੜੋ) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(h);

        r = resize_buckets(h, entries_ਜੋੜੋ);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ 0;
}

/*
 * The same as hashmap_merge(), but every new item from other is moved to h.
 * Keys already in h are skipped and stay in other.
 * Returns: 0 on success.
 *          -ENOMEM on alloc failure, in which case no move has been done.
 */
ਪੂਰਨ _hashmap_move(Hashmap_ਅਧਾਰ *h, Hashmap_ਅਧਾਰ *other) {
        ਬਣਤਰ ਅਦਲਾ_ਬਦਲੀ_entries ਅਦਲਾ_ਬਦਲੀ;
        ਬਣਤਰ hashmap_ਅਧਾਰ_ਇੰਦਰਾਜ *e, *n;
        Iterator i;
        ਬਿਨਾਨਿਸ਼ਾਨੀ idx;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(h);

        ਜੇ (!other)
                ਵਾਪਸ 0;

        ਪ੍ਰਤਿਜ੍ਞਾ(other->type == h->type);

        /*
         * This reserves buckets for the worst case, where none of other's
         * entries are yet present in h. This is preferable to risking
         * an allocation failure in the middle of the moving and having to
         * rollback or return a partial result.
         */
        r = resize_buckets(h, n_entries(other));
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        HASHMAP_FOREACH_IDX(idx, other, i) {
                ਬਿਨਾਨਿਸ਼ਾਨੀ h_hash;

                e = ਬਾਲਟੀ_at(other, idx);
                h_hash = ਬਾਲਟੀ_hash(h, e->ਕੁੰਜੀ);
                ਜੇ (ਬਾਲਟੀ_ਜਾਂਚ(h, h_hash, e->ਕੁੰਜੀ) != IDX_NIL)
                        ਜਾਰੀ;

                n = &ਬਾਲਟੀ_at_ਅਦਲਾ_ਬਦਲੀ(&ਅਦਲਾ_ਬਦਲੀ, IDX_PUT)->p.b;
                n->ਕੁੰਜੀ = e->ਕੁੰਜੀ;
                ਜੇ (h->type != HASHMAP_TYPE_ਸੈੱਟ)
                        ((ਬਣਤਰ plain_hashmap_ਇੰਦਰਾਜ*) n)->ਮੁੱਲ =
                                ((ਬਣਤਰ plain_hashmap_ਇੰਦਰਾਜ*) e)->ਮੁੱਲ;
                assert_se(hashmap_put_boldly(h, h_hash, &ਅਦਲਾ_ਬਦਲੀ, false) == 1);

                remove_ਇੰਦਰਾਜ(other, idx);
        }

        ਵਾਪਸ 0;
}

ਪੂਰਨ _hashmap_move_one(Hashmap_ਅਧਾਰ *h, Hashmap_ਅਧਾਰ *other, ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ) {
        ਬਣਤਰ ਅਦਲਾ_ਬਦਲੀ_entries ਅਦਲਾ_ਬਦਲੀ;
        ਬਿਨਾਨਿਸ਼ਾਨੀ h_hash, other_hash, idx;
        ਬਣਤਰ hashmap_ਅਧਾਰ_ਇੰਦਰਾਜ *e, *n;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(h);

        h_hash = ਬਾਲਟੀ_hash(h, ਕੁੰਜੀ);
        ਜੇ (ਬਾਲਟੀ_ਜਾਂਚ(h, h_hash, ਕੁੰਜੀ) != IDX_NIL)
                ਵਾਪਸ -EEXIST;

        ਜੇ (!other)
                ਵਾਪਸ -ENOENT;

        ਪ੍ਰਤਿਜ੍ਞਾ(other->type == h->type);

        other_hash = ਬਾਲਟੀ_hash(other, ਕੁੰਜੀ);
        idx = ਬਾਲਟੀ_ਜਾਂਚ(other, other_hash, ਕੁੰਜੀ);
        ਜੇ (idx == IDX_NIL)
                ਵਾਪਸ -ENOENT;

        e = ਬਾਲਟੀ_at(other, idx);

        n = &ਬਾਲਟੀ_at_ਅਦਲਾ_ਬਦਲੀ(&ਅਦਲਾ_ਬਦਲੀ, IDX_PUT)->p.b;
        n->ਕੁੰਜੀ = e->ਕੁੰਜੀ;
        ਜੇ (h->type != HASHMAP_TYPE_ਸੈੱਟ)
                ((ਬਣਤਰ plain_hashmap_ਇੰਦਰਾਜ*) n)->ਮੁੱਲ =
                        ((ਬਣਤਰ plain_hashmap_ਇੰਦਰਾਜ*) e)->ਮੁੱਲ;
        r = hashmap_put_boldly(h, h_hash, &ਅਦਲਾ_ਬਦਲੀ, true);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        remove_ਇੰਦਰਾਜ(other, idx);
        ਵਾਪਸ 0;
}

Hashmap_ਅਧਾਰ* _hashmap_copy(Hashmap_ਅਧਾਰ *h) {
        Hashmap_ਅਧਾਰ *copy;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(h);

        copy = hashmap_ਅਧਾਰ_new(h->hash_ops, h->type);
        ਜੇ (!copy)
                ਵਾਪਸ ਨਿਰਾ;

        ਚੋਣ (h->type) {
        ਹਾਲਤ HASHMAP_TYPE_PLAIN:
        ਹਾਲਤ HASHMAP_TYPE_ORDERED:
                r = hashmap_ਮਿਲਾਓ_2((Hashmap*)copy, (Hashmap*)h);
                ਰੋਕੋ;
        ਹਾਲਤ HASHMAP_TYPE_ਸੈੱਟ:
                r = ਸੈੱਟ_ਮਿਲਾਓ((ਸੈੱਟ_2*)copy, (ਸੈੱਟ_2*)h);
                ਰੋਕੋ;
        ਮੂਲ:
                assert_not_reached();
        }

        ਜੇ (r < 0)
                ਵਾਪਸ _hashmap_free(copy);

        ਵਾਪਸ copy;
}

ਅੱਖਰ** _hashmap_get_strv(Hashmap_ਅਧਾਰ *h) {
        ਅੱਖਰ **sv;
        Iterator i;
        ਬਿਨਾਨਿਸ਼ਾਨੀ idx, n;

        ਜੇ (!h)
                ਵਾਪਸ new0(ਅੱਖਰ*, 1);

        sv = new(ਅੱਖਰ*, n_entries(h)+1);
        ਜੇ (!sv)
                ਵਾਪਸ ਨਿਰਾ;

        n = 0;
        HASHMAP_FOREACH_IDX(idx, h, i)
                sv[n++] = ਇੰਦਰਾਜ_ਮੁੱਲ(h, ਬਾਲਟੀ_at(h, idx));
        sv[n] = ਨਿਰਾ;

        ਵਾਪਸ sv;
}

ਅੱਖਰ** ਸੈੱਟ_ਮੰਜ਼ਿਲ_strv(ਸੈੱਟ_2 **s) {
        ਪ੍ਰਤਿਜ੍ਞਾ(s);

        /* This is similar to set_get_strv(), but invalidates the set on success. */

        ਅੱਖਰ **v = new(ਅੱਖਰ*, ਸੈੱਟ_ਮਾਪ(*s) + 1);
        ਜੇ (!v)
                ਵਾਪਸ ਨਿਰਾ;

        ਲਈ (ਅੱਖਰ **p = v; (*p = ਸੈੱਟ_steal_ਪਹਿਲਾ(*s)); p++)
                ;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਸੈੱਟ_isempty(*s));
        *s = ਸੈੱਟ_free(*s);
        ਵਾਪਸ v;
}

ਖਾਲੀ* ordered_hashmap_ਅਗਲਾ(OrderedHashmap *h, ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ) {
        ਬਣਤਰ ordered_hashmap_ਇੰਦਰਾਜ *e;
        ਬਿਨਾਨਿਸ਼ਾਨੀ hash, idx;

        ਜੇ (!h)
                ਵਾਪਸ ਨਿਰਾ;

        hash = ਬਾਲਟੀ_hash(h, ਕੁੰਜੀ);
        idx = ਬਾਲਟੀ_ਜਾਂਚ(h, hash, ਕੁੰਜੀ);
        ਜੇ (idx == IDX_NIL)
                ਵਾਪਸ ਨਿਰਾ;

        e = ordered_ਬਾਲਟੀ_at(h, idx);
        ਜੇ (e->iterate_ਅਗਲਾ == IDX_NIL)
                ਵਾਪਸ ਨਿਰਾ;
        ਵਾਪਸ ordered_ਬਾਲਟੀ_at(h, e->iterate_ਅਗਲਾ)->p.ਮੁੱਲ;
}

ਪੂਰਨ ਸੈੱਟ_consume(ਸੈੱਟ_2 *s, ਖਾਲੀ *ਮੁੱਲ) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(s);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਮੁੱਲ);

        r = ਸੈੱਟ_put(s, ਮੁੱਲ);
        ਜੇ (r <= 0)
                ਮੁਕਤ(ਮੁੱਲ);

        ਵਾਪਸ r;
}

ਪੂਰਨ hashmap_put_strdup_ਭਰਿਆ(Hashmap **h, ਅਚਲ ਬਣਤਰ hash_ops *hash_ops, ਅਚਲ ਅੱਖਰ *k, ਅਚਲ ਅੱਖਰ *v) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(h);

        r = hashmap_ensure_allocated(h, hash_ops);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        _cleanup_free_ ਅੱਖਰ *kdup = ਨਿਰਾ, *vdup = ਨਿਰਾ;

        kdup = strdup(k);
        ਜੇ (!kdup)
                ਵਾਪਸ -ENOMEM;

        ਜੇ (v) {
                vdup = strdup(v);
                ਜੇ (!vdup)
                        ਵਾਪਸ -ENOMEM;
        }

        r = hashmap_put(*h, kdup, vdup);
        ਜੇ (r < 0) {
                ਜੇ (r == -EEXIST && streq_ptr(v, hashmap_get(*h, kdup)))
                        ਵਾਪਸ 0;
                ਵਾਪਸ r;
        }

        /* 0 with non-null vdup would mean vdup is already in the hashmap, which cannot be */
        ਪ੍ਰਤਿਜ੍ਞਾ(vdup == ਨਿਰਾ || r > 0);
        ਜੇ (r > 0)
                kdup = vdup = ਨਿਰਾ;

        ਵਾਪਸ r;
}

ਪੂਰਨ ਸੈੱਟ_put_strndup_ਭਰਿਆ(ਸੈੱਟ_2 **s, ਅਚਲ ਬਣਤਰ hash_ops *hash_ops, ਅਚਲ ਅੱਖਰ *p, size_t n) {
        ਅੱਖਰ *c;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(s);
        ਪ੍ਰਤਿਜ੍ਞਾ(p);

        r = ਸੈੱਟ_ensure_allocated(s, hash_ops);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (n == SIZE_MAX) {
                ਜੇ (ਸੈੱਟ_ਸ਼ਾਮਲ_ਹੈ(*s, (ਅੱਖਰ*) p))
                        ਵਾਪਸ 0;

                c = strdup(p);
        } ਨਹੀਂਤਾਂ
                c = strndup(p, n);
        ਜੇ (!c)
                ਵਾਪਸ -ENOMEM;

        ਵਾਪਸ ਸੈੱਟ_consume(*s, c);
}

ਪੂਰਨ ਸੈੱਟ_put_strdupv_ਭਰਿਆ(ਸੈੱਟ_2 **s, ਅਚਲ ਬਣਤਰ hash_ops *hash_ops, ਅੱਖਰ **l) {
        ਪੂਰਨ n = 0, r;

        ਪ੍ਰਤਿਜ੍ਞਾ(s);

        STRV_FOREACH(i, l) {
                r = ਸੈੱਟ_put_strndup_ਭਰਿਆ(s, hash_ops, *i, SIZE_MAX);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;

                n += r;
        }

        ਵਾਪਸ n;
}

ਪੂਰਨ ਸੈੱਟ_put_strsplit(ਸੈੱਟ_2 *s, ਅਚਲ ਅੱਖਰ *v, ਅਚਲ ਅੱਖਰ *separators, ਕੱਢੋ_Flags flags) {
        ਅਚਲ ਅੱਖਰ *p = ASSERT_PTR(v);
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(s);

        ਲਈ (;;) {
                ਅੱਖਰ *ਸ਼ਬਦ;

                r = ਕੱਢੋ_ਪਹਿਲਾ_ਸ਼ਬਦ(&p, &ਸ਼ਬਦ, separators, flags);
                ਜੇ (r <= 0)
                        ਵਾਪਸ r;

                r = ਸੈੱਟ_consume(s, ਸ਼ਬਦ);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
        }
}

/* expand the cachemem if needed, return true if newly (re)activated. */
ਸਥਿਰ ਪੂਰਨ cachemem_maintain(CacheMem *mem, size_t ਮਾਪ) {
        ਪ੍ਰਤਿਜ੍ਞਾ(mem);

        ਜੇ (!GREEDY_REALLOC(mem->ptr, ਮਾਪ)) {
                ਜੇ (ਮਾਪ > 0)
                        ਵਾਪਸ -ENOMEM;
        }

        ਜੇ (!mem->ਸਰਗਰਮ) {
                mem->ਸਰਗਰਮ = true;
                ਵਾਪਸ true;
        }

        ਵਾਪਸ false;
}

ਪੂਰਨ iterated_cache_get(IteratedCache *cache, ਅਚਲ ਖਾਲੀ ***res_ਕੁੰਜੀਆਂ, ਅਚਲ ਖਾਲੀ ***res_ਮੁੱਲਾਂ, ਬਿਨਾਨਿਸ਼ਾਨੀ *res_n_entries) {
        bool sync_ਕੁੰਜੀਆਂ = false, sync_ਮੁੱਲਾਂ = false;
        size_t ਮਾਪ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(cache);
        ਪ੍ਰਤਿਜ੍ਞਾ(cache->hashmap);

        ਮਾਪ = n_entries(cache->hashmap);

        ਜੇ (res_ਕੁੰਜੀਆਂ) {
                r = cachemem_maintain(&cache->ਕੁੰਜੀਆਂ, ਮਾਪ);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;

                sync_ਕੁੰਜੀਆਂ = r;
        } ਨਹੀਂਤਾਂ
                cache->ਕੁੰਜੀਆਂ.ਸਰਗਰਮ = false;

        ਜੇ (res_ਮੁੱਲਾਂ) {
                r = cachemem_maintain(&cache->values, ਮਾਪ);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;

                sync_ਮੁੱਲਾਂ = r;
        } ਨਹੀਂਤਾਂ
                cache->values.ਸਰਗਰਮ = false;

        ਜੇ (cache->hashmap->dirty) {
                ਜੇ (cache->ਕੁੰਜੀਆਂ.ਸਰਗਰਮ)
                        sync_ਕੁੰਜੀਆਂ = true;
                ਜੇ (cache->values.ਸਰਗਰਮ)
                        sync_ਮੁੱਲਾਂ = true;

                cache->hashmap->dirty = false;
        }

        ਜੇ (sync_ਕੁੰਜੀਆਂ || sync_ਮੁੱਲਾਂ) {
                ਬਿਨਾਨਿਸ਼ਾਨੀ i, idx;
                Iterator iter;

                i = 0;
                HASHMAP_FOREACH_IDX(idx, cache->hashmap, iter) {
                        ਬਣਤਰ hashmap_ਅਧਾਰ_ਇੰਦਰਾਜ *e;

                        e = ਬਾਲਟੀ_at(cache->hashmap, idx);

                        ਜੇ (sync_ਕੁੰਜੀਆਂ)
                                cache->ਕੁੰਜੀਆਂ.ptr[i] = e->ਕੁੰਜੀ;
                        ਜੇ (sync_ਮੁੱਲਾਂ)
                                cache->values.ptr[i] = ਇੰਦਰਾਜ_ਮੁੱਲ(cache->hashmap, e);
                        i++;
                }
        }

        ਜੇ (res_ਕੁੰਜੀਆਂ)
                *res_ਕੁੰਜੀਆਂ = cache->ਕੁੰਜੀਆਂ.ptr;
        ਜੇ (res_ਮੁੱਲਾਂ)
                *res_ਮੁੱਲਾਂ = cache->values.ptr;
        ਜੇ (res_n_entries)
                *res_n_entries = ਮਾਪ;

        ਵਾਪਸ 0;
}

IteratedCache* iterated_cache_free(IteratedCache *cache) {
        ਜੇ (cache) {
                ਮੁਕਤ(cache->ਕੁੰਜੀਆਂ.ptr);
                ਮੁਕਤ(cache->values.ptr);
        }

        ਵਾਪਸ mfree(cache);
}

ਪੂਰਨ ਸੈੱਟ_strjoin(ਸੈੱਟ_2 *s, ਅਚਲ ਅੱਖਰ *separator, bool wrap_with_separator, ਅੱਖਰ **ret) {
        _cleanup_free_ ਅੱਖਰ *str = ਨਿਰਾ;
        size_t separator_len, len = 0;
        ਅਚਲ ਅੱਖਰ *ਮੁੱਲ;
        bool ਪਹਿਲਾ;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        ਜੇ (ਸੈੱਟ_isempty(s)) {
                *ret = ਨਿਰਾ;
                ਵਾਪਸ 0;
        }

        separator_len = strlen_ptr(separator);

        ਜੇ (separator_len == 0)
                wrap_with_separator = false;

        ਪਹਿਲਾ = !wrap_with_separator;

        ਸੈੱਟ_FOREACH(ਮੁੱਲ, s) {
                size_t l = strlen_ptr(ਮੁੱਲ);

                ਜੇ (l == 0)
                        ਜਾਰੀ;

                ਜੇ (!GREEDY_REALLOC(str, len + l + (ਪਹਿਲਾ ? 0 : separator_len) + (wrap_with_separator ? separator_len : 0) + 1))
                        ਵਾਪਸ -ENOMEM;

                ਜੇ (separator_len > 0 && !ਪਹਿਲਾ) {
                        ਸਮ੍ਰਿਤੀ_ਨਕਲ(str + len, separator, separator_len);
                        len += separator_len;
                }

                ਸਮ੍ਰਿਤੀ_ਨਕਲ(str + len, ਮੁੱਲ, l);
                len += l;
                ਪਹਿਲਾ = false;
        }

        ਜੇ (wrap_with_separator) {
                ਸਮ੍ਰਿਤੀ_ਨਕਲ(str + len, separator, separator_len);
                len += separator_len;
        }

        str[len] = '\0';

        *ret = TAKE_PTR(str);
        ਵਾਪਸ 0;
}

bool ਸੈੱਟ_equal(ਸੈੱਟ_2 *a, ਸੈੱਟ_2 *b) {
        ਖਾਲੀ *p;

        /* Checks whether each entry of 'a' is also in 'b' and vice versa, i.e. the two sets contain the same
         * entries */

        ਜੇ (a == b)
                ਵਾਪਸ true;

        ਜੇ (ਸੈੱਟ_isempty(a) && ਸੈੱਟ_isempty(b))
                ਵਾਪਸ true;

        ਜੇ (ਸੈੱਟ_ਮਾਪ(a) != ਸੈੱਟ_ਮਾਪ(b)) /* Cheap check that hopefully catches a lot of inequality cases
                                         * already */
                ਵਾਪਸ false;

        ਸੈੱਟ_FOREACH(p, a)
                ਜੇ (!ਸੈੱਟ_ਸ਼ਾਮਲ_ਹੈ(b, p))
                        ਵਾਪਸ false;

        /* If we have the same hashops, then we don't need to check things backwards given we compared the
         * size and that all of a is in b. */
        ਜੇ (a->b.hash_ops == b->b.hash_ops)
                ਵਾਪਸ true;

        ਸੈੱਟ_FOREACH(p, b)
                ਜੇ (!ਸੈੱਟ_ਸ਼ਾਮਲ_ਹੈ(a, p))
                        ਵਾਪਸ false;

        ਵਾਪਸ true;
}

ਸਥਿਰ bool ਸੈੱਟ_fnmatch_one(ਸੈੱਟ_2 *patterns, ਅਚਲ ਅੱਖਰ *needle) {
        ਅਚਲ ਅੱਖਰ *p;

        ਪ੍ਰਤਿਜ੍ਞਾ(needle);

        /* Any failure of fnmatch() is treated as equivalent to FNM_NOMATCH, i.e. as non-matching pattern */

        ਸੈੱਟ_FOREACH(p, patterns)
                ਜੇ (fnmatch(p, needle, 0) == 0)
                        ਵਾਪਸ true;

        ਵਾਪਸ false;
}

bool ਸੈੱਟ_fnmatch(ਸੈੱਟ_2 *include_patterns, ਸੈੱਟ_2 *exclude_patterns, ਅਚਲ ਅੱਖਰ *needle) {
        ਪ੍ਰਤਿਜ੍ਞਾ(needle);

        ਜੇ (ਸੈੱਟ_fnmatch_one(exclude_patterns, needle))
                ਵਾਪਸ false;

        ਜੇ (ਸੈੱਟ_isempty(include_patterns))
                ਵਾਪਸ true;

        ਵਾਪਸ ਸੈੱਟ_fnmatch_one(include_patterns, needle);
}

ਸਥਿਰ ਪੂਰਨ hashmap_ਇੰਦਰਾਜ_compare(
                ਬਣਤਰ hashmap_ਅਧਾਰ_ਇੰਦਰਾਜ * ਅਚਲ *a,
                ਬਣਤਰ hashmap_ਅਧਾਰ_ਇੰਦਰਾਜ * ਅਚਲ *b,
                compare_func_t compare) {

        ਪ੍ਰਤਿਜ੍ਞਾ(a && *a);
        ਪ੍ਰਤਿਜ੍ਞਾ(b && *b);
        ਪ੍ਰਤਿਜ੍ਞਾ(compare);

        ਵਾਪਸ compare((*a)->ਕੁੰਜੀ, (*b)->ਕੁੰਜੀ);
}

ਸਥਿਰ ਪੂਰਨ _hashmap_dump_entries_sorted(
                Hashmap_ਅਧਾਰ *h,
                ਖਾਲੀ ***ret,
                size_t *ret_n) {
        _cleanup_free_ ਖਾਲੀ **entries = ਨਿਰਾ;
        Iterator iter;
        ਬਿਨਾਨਿਸ਼ਾਨੀ idx;
        size_t n = 0;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret_n);

        ਜੇ (hashmap_ਮਾਪ(h) == 0) {
                *ret = ਨਿਰਾ;
                *ret_n = 0;
                ਵਾਪਸ 0;
        }

        /* We append one more element than needed so that the resulting array can be used as a strv. We
         * don't count this entry in the returned size. */
        entries = new(ਖਾਲੀ*, hashmap_ਮਾਪ(h) + 1);
        ਜੇ (!entries)
                ਵਾਪਸ -ENOMEM;

        HASHMAP_FOREACH_IDX(idx, h, iter)
                entries[n++] = ਬਾਲਟੀ_at(h, idx);

        ਪ੍ਰਤਿਜ੍ਞਾ(n == hashmap_ਮਾਪ(h));
        entries[n] = ਨਿਰਾ;

        typesafe_qsort_r((ਬਣਤਰ hashmap_ਅਧਾਰ_ਇੰਦਰਾਜ**) entries, n,
                         hashmap_ਇੰਦਰਾਜ_compare, h->hash_ops->compare);

        *ret = TAKE_PTR(entries);
        *ret_n = n;
        ਵਾਪਸ 0;
}

ਪੂਰਨ hashmap_dump_ਕੁੰਜੀਆਂ_sorted(Hashmap_ਅਧਾਰ *h, ਖਾਲੀ ***ret, size_t *ret_n) {
        _cleanup_free_ ਖਾਲੀ **entries = ਨਿਰਾ;
        size_t n;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        r = _hashmap_dump_entries_sorted(h, &entries, &n);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        /* Reuse the array. */
        FOREACH_ARRAY(e, entries, n)
                *e = (ਖਾਲੀ*) (*(ਬਣਤਰ hashmap_ਅਧਾਰ_ਇੰਦਰਾਜ**) e)->ਕੁੰਜੀ;

        *ret = TAKE_PTR(entries);
        ਜੇ (ret_n)
                *ret_n = n;
        ਵਾਪਸ 0;
}

ਪੂਰਨ _hashmap_dump_sorted(Hashmap_ਅਧਾਰ *h, ਖਾਲੀ ***ret, size_t *ret_n) {
        _cleanup_free_ ਖਾਲੀ **entries = ਨਿਰਾ;
        size_t n;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        r = _hashmap_dump_entries_sorted(h, &entries, &n);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        /* Reuse the array. */
        FOREACH_ARRAY(e, entries, n)
                *e = ਇੰਦਰਾਜ_ਮੁੱਲ(h, *(ਬਣਤਰ hashmap_ਅਧਾਰ_ਇੰਦਰਾਜ**) e);

        *ret = TAKE_PTR(entries);
        ਜੇ (ret_n)
                *ret_n = n;
        ਵਾਪਸ 0;
}
