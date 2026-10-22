/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <fnmatch.h>
#ಅನ್ತರ್ಭಾವ <pthread.h>
#ಅನ್ತರ್ಭಾವ <unistd.h>
#ಯದಿ HAVE_VALGRIND_VALGRIND_H
#  include <valgrind/valgrind.h>
#endif

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

#ಯದಿ ENABLE_DEBUG_HASHMAP
#ಅನ್ತರ್ಭಾವ "list.h"
#endif

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
#define INV_KEEP_FREE            5U

/* Fields common to entries of all hashmap/set types */
ಸಂರಚನಾ hashmap_base_entry {
        ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ;
};

/* Entry types for specific hashmap/set types
 * hashmap_base_entry must be at the beginning of each entry struct. */

ಸಂರಚನಾ plain_hashmap_entry {
        ಸಂರಚನಾ hashmap_base_entry b;
        ಶೂನ್ಯ *ಮೌಲ್ಯ;
};

ಸಂರಚನಾ ordered_hashmap_entry {
        ಸಂರಚನಾ plain_hashmap_entry p;
        unsigned iterate_next, iterate_previous;
};

ಸಂರಚನಾ set_entry {
        ಸಂರಚನಾ hashmap_base_entry b;
};

/* In several functions it is advantageous to have the hash table extended
 * virtually by a couple of additional buckets. We reserve special index values
 * for these "swap" buckets. */
#define _IDX_SWAP_BEGIN     (UINT_MAX - 3)
#define IDX_PUT             (_IDX_SWAP_BEGIN + 0)
#define IDX_TMP             (_IDX_SWAP_BEGIN + 1)
#define _IDX_SWAP_END       (_IDX_SWAP_BEGIN + 2)

#define IDX_FIRST           (UINT_MAX - 1) /* special index for freshly initialized iterators */
#define IDX_NIL             UINT_MAX       /* special index value meaning "none" or "end" */

assert_cc(IDX_FIRST == _IDX_SWAP_END);
assert_cc(IDX_FIRST == _IDX_ITERATOR_FIRST);

/* Storage space for the "swap" buckets.
 * All entry types can fit into an ordered_hashmap_entry. */
ಸಂರಚನಾ swap_entries {
        ಸಂರಚನಾ ordered_hashmap_entry e[_IDX_SWAP_END - _IDX_SWAP_BEGIN];
};

/* Distance from Initial Bucket */
ಪ್ರಕಾರನಿರ್ದೇಶ uint8_t dib_raw_t;
#define DIB_RAW_OVERFLOW ((dib_raw_t)0xfdU)   /* indicates DIB value is greater than representable */
#define DIB_RAW_REHASH   ((dib_raw_t)0xfeU)   /* entry yet to be rehashed during in-place resize */
#define DIB_RAW_FREE     ((dib_raw_t)0xffU)   /* a free bucket */
#define DIB_RAW_INIT     ((ವರ್ಣ)DIB_RAW_FREE) /* a byte to memset a DIB store with when initializing */

#define DIB_FREE UINT_MAX

#ಯದಿ ENABLE_DEBUG_HASHMAP
ಸಂರಚನಾ hashmap_debug_info {
        LIST_FIELDS(ಸಂರಚನಾ hashmap_debug_info, debug_list);
        unsigned max_entries;  /* high watermark of n_entries */

        /* fields to detect modification while iterating */
        unsigned put_ಎಣಿಕೆ;    /* counts puts into the hashmap */
        unsigned rem_ಎಣಿಕೆ;    /* counts removals from hashmap */
        unsigned last_rem_idx; /* remembers last removal index */
};

/* Tracks all existing hashmaps. Get at it from gdb. See sd_dump_hashmaps.py */
ಸ್ಥಿರ LIST_HEAD(ಸಂರಚನಾ hashmap_debug_info, hashmap_debug_list);
ಸ್ಥಿರ pthread_mutex_t hashmap_debug_list_mutex = PTHREAD_MUTEX_INITIALIZER;
#endif

ಗಣನಾ HashmapType {
        HASHMAP_TYPE_PLAIN,
        HASHMAP_TYPE_ORDERED,
        HASHMAP_TYPE_SET,
        _HASHMAP_TYPE_MAX
};

ಸಂರಚನಾ _packed_ indirect_storage {
        ಶೂನ್ಯ *storage;                     /* where buckets and DIBs are stored */
        uint8_t  hash_ಕೀಲಿ[HASH_ಕೀಲಿ_SIZE];  /* hash key; changes during resize */

        unsigned n_entries;                /* number of stored entries */
        unsigned n_buckets;                /* number of buckets */

        unsigned idx_lowest_entry;         /* Index below which all buckets are free.
                                              Makes "while (hashmap_steal_first())" loops
                                              O(n) instead of O(n^2) for unordered hashmaps. */
        uint8_t  _pad[3];                  /* padding for the whole HashmapBase */
        /* The bitfields in HashmapBase complete the alignment of the whole thing. */
};

ಸಂರಚನಾ direct_storage {
        /* This gives us 39 bytes on 64-bit, or 35 bytes on 32-bit.
         * That's room for 4 set_entries + 4 DIB bytes + 3 unused bytes on 64-bit,
         *              or 7 set_entries + 7 DIB bytes + 0 unused bytes on 32-bit. */
        uint8_t storage[ಪರಿಮಾಣ(ಸಂರಚನಾ indirect_storage)];
};

#define DIRECT_BUCKETS(entry_t) \
        (ಪರಿಮಾಣ(ಸಂರಚನಾ direct_storage) / (ಪರಿಮಾಣ(entry_t) + ಪರಿಮಾಣ(dib_raw_t)))

/* We should be able to store at least one entry directly. */
assert_cc(DIRECT_BUCKETS(ಸಂರಚನಾ ordered_hashmap_entry) >= 1);

/* We have 3 bits for n_direct_entries. */
assert_cc(DIRECT_BUCKETS(ಸಂರಚನಾ set_entry) < (1 << 3));

/* Hashmaps with directly stored entries all use this shared hash key.
 * It's no big deal if the key is guessed, because there can be only
 * a handful of directly stored entries in a hashmap. When a hashmap
 * outgrows direct storage, it gets its own key for indirect storage. */
ಸ್ಥಿರ uint8_t shared_hash_ಕೀಲಿ[HASH_ಕೀಲಿ_SIZE];

/* Fields that all hashmap/set types must have */
ಸಂರಚನಾ HashmapBase {
        ಧ್ರುವ ಸಂರಚನಾ hash_ops *hash_ops;  /* hash and compare ops to use */

        ಸಙ್ಘ _packed_ {
                ಸಂರಚನಾ indirect_storage indirect; /* if  has_indirect */
                ಸಂರಚನಾ direct_storage direct;     /* if !has_indirect */
        };

        ಗಣನಾ HashmapType type:2;     /* HASHMAP_TYPE_* */
        bool has_indirect:1;         /* whether indirect storage is used */
        unsigned n_direct_entries:3; /* Number of entries in direct storage.
                                      * Only valid if !has_indirect. */
        bool from_pool:1;            /* whether was allocated from mempool */
        bool dirty:1;                /* whether dirtied since last iterated_cache_get() */
        bool cached:1;               /* whether this hashmap is being cached */

#ಯದಿ ENABLE_DEBUG_HASHMAP
        ಸಂರಚನಾ hashmap_debug_info debug;
#endif
};

/* Specific hash types
 * HashmapBase must be at the beginning of each hashmap struct. */

ಸಂರಚನಾ Hashmap {
        ಸಂರಚನಾ HashmapBase b;
};

ಸಂರಚನಾ OrderedHashmap {
        ಸಂರಚನಾ HashmapBase b;
        unsigned iterate_list_head, iterate_list_tail;
};

ಸಂರಚನಾ Set {
        ಸಂರಚನಾ HashmapBase b;
};

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ CacheMem {
        ಧ್ರುವ ಶೂನ್ಯ **ptr;
        size_t n_populated;
        bool active:1;
} CacheMem;

ಸಂರಚನಾ IteratedCache {
        HashmapBase *hashmap;
        CacheMem ಕೀಲಿಗಳು, values;
};

DEFINE_MEMPOOL(hashmap_pool,         Hashmap,        8);
DEFINE_MEMPOOL(ordered_hashmap_pool, OrderedHashmap, 8);
/* No need for a separate Set pool */
assert_cc(ಪರಿಮಾಣ(Hashmap) == ಪರಿಮಾಣ(Set));

ಸಂರಚನಾ hashmap_type_info {
        size_t head_size;
        size_t entry_size;
        ಸಂರಚನಾ mempool *mempool;
        unsigned n_direct_buckets;
};

ಸ್ಥಿರ _used_ ಧ್ರುವ ಸಂರಚನಾ hashmap_type_info hashmap_type_info[_HASHMAP_TYPE_MAX] = {
        [HASHMAP_TYPE_PLAIN] = {
                .head_size        = ಪರಿಮಾಣ(Hashmap),
                .entry_size       = ಪರಿಮಾಣ(ಸಂರಚನಾ plain_hashmap_entry),
                .mempool          = &hashmap_pool,
                .n_direct_buckets = DIRECT_BUCKETS(ಸಂರಚನಾ plain_hashmap_entry),
        },
        [HASHMAP_TYPE_ORDERED] = {
                .head_size        = ಪರಿಮಾಣ(OrderedHashmap),
                .entry_size       = ಪರಿಮಾಣ(ಸಂರಚನಾ ordered_hashmap_entry),
                .mempool          = &ordered_hashmap_pool,
                .n_direct_buckets = DIRECT_BUCKETS(ಸಂರಚನಾ ordered_hashmap_entry),
        },
        [HASHMAP_TYPE_SET] = {
                .head_size        = ಪರಿಮಾಣ(Set),
                .entry_size       = ಪರಿಮಾಣ(ಸಂರಚನಾ set_entry),
                .mempool          = &hashmap_pool,
                .n_direct_buckets = DIRECT_BUCKETS(ಸಂರಚನಾ set_entry),
        },
};

ಶೂನ್ಯ hashmap_trim_pools(ಶೂನ್ಯ) {
        ಪೂರ್ಣಾಙ್ಕ r;

        /* The pool is only allocated by the main thread, but the memory can be passed to other
         * threads. Let's clean up if we are the main thread and no other threads are live. */

        /* We build our own is_main_thread() here, which doesn't use C11 TLS based caching of the
         * result. That's because valgrind apparently doesn't like TLS to be used from a GCC destructor. */
        ಯದಿ (getpid() != gettid())
                ಪ್ರತ್ಯಾಗಮ (ಶೂನ್ಯ) log_debug("Not cleaning up memory pools, not in main thread.");

        r = get_process_threads(0);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ (ಶೂನ್ಯ) log_debug_errno(r, "Failed to determine number of threads, not cleaning up memory pools: %m");
        ಯದಿ (r != 1)
                ಪ್ರತ್ಯಾಗಮ (ಶೂನ್ಯ) log_debug("Not cleaning up memory pools, running in multi-threaded process.");

        mempool_trim(&hashmap_pool);
        mempool_trim(&ordered_hashmap_pool);
}

#ಯದಿ HAVE_VALGRIND_VALGRIND_H
_destructor_ ಸ್ಥಿರ ಶೂನ್ಯ cleanup_pools(ಶೂನ್ಯ) {
        /* Be nice to valgrind */
        ಯದಿ (RUNNING_ON_VALGRIND)
                hashmap_trim_pools();
}
#endif

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ unsigned n_buckets(HashmapBase *h) {
        ಪ್ರತಿಜ್ಞಾ(h);
        ಪ್ರತ್ಯಾಗಮ h->has_indirect ? h->indirect.n_buckets
                               : hashmap_type_info[h->type].n_direct_buckets;
}

ಸ್ಥಿರ unsigned n_entries(HashmapBase *h) {
        ಪ್ರತಿಜ್ಞಾ(h);
        ಪ್ರತ್ಯಾಗಮ h->has_indirect ? h->indirect.n_entries
                               : h->n_direct_entries;
}

ಸ್ಥಿರ ಶೂನ್ಯ n_entries_inc(HashmapBase *h) {
        ಪ್ರತಿಜ್ಞಾ(h);

        ಯದಿ (h->has_indirect)
                h->indirect.n_entries++;
        ಅನ್ಯಥಾ
                h->n_direct_entries++;
}

ಸ್ಥಿರ ಶೂನ್ಯ n_entries_dec(HashmapBase *h) {
        ಪ್ರತಿಜ್ಞಾ(h);

        ಯದಿ (h->has_indirect)
                h->indirect.n_entries--;
        ಅನ್ಯಥಾ
                h->n_direct_entries--;
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ* storage_ptr(HashmapBase *h) {
        ಪ್ರತಿಜ್ಞಾ(h);
        ಪ್ರತ್ಯಾಗಮ h->has_indirect ? h->indirect.storage
                               : h->direct.storage;
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ uint8_t* hash_ಕೀಲಿ(HashmapBase *h) {
        ಪ್ರತಿಜ್ಞಾ(h);
        ಪ್ರತ್ಯಾಗಮ h->has_indirect ? h->indirect.hash_ಕೀಲಿ
                               : shared_hash_ಕೀಲಿ;
}

ಸ್ಥಿರ unsigned base_bucket_hash(HashmapBase *h, ಧ್ರುವ ಶೂನ್ಯ *p) {
        ಸಂರಚನಾ siphash state;
        uint64_t hash;

        ಪ್ರತಿಜ್ಞಾ(h);

        /* The keys of trivial_hash_ops and uint64_hash_ops fit in 64 bits. siphash24_uint64() hashes such
         * a key without the indirect call through hash_ops and keeps the SipHash state in registers. On
         * 32-bit and big-endian architectures, the result differs from the result of trivial_hash_func(),
         * which hashes the key in native byte order. Only this function maps keys to buckets, so the
         * difference does not matter. */
        ಯದಿ (h->hash_ops->hash == trivial_hash_func)
                hash = siphash24_uint64((uintptr_t) p, hash_ಕೀಲಿ(h));
        ಅನ್ಯಥಾ ಯದಿ (h->hash_ops->hash == (hash_func_t) uint64_hash_func)
                hash = siphash24_uint64(*(ಧ್ರುವ uint64_t*) p, hash_ಕೀಲಿ(h));
        ಅನ್ಯಥಾ {
                siphash24_init(&state, hash_ಕೀಲಿ(h));
                h->hash_ops->hash(p, &state);
                hash = siphash24_finalize(&state);
        }

        /* n_buckets is not a power of two, so a modulo needs a division. Multiplying the upper 32 bits of
         * the hash with n_buckets and keeping the upper 32 bits of the product maps the hash to
         * [0, n_buckets) almost as evenly, without a division. */
        ಪ್ರತ್ಯಾಗಮ (unsigned) (((hash >> 32) * n_buckets(h)) >> 32);
}
#define bucket_hash(h, p) base_bucket_hash(HASHMAP_BASE(h), p)

ಸ್ಥಿರ ಶೂನ್ಯ base_set_dirty(HashmapBase *h) {
        ಪ್ರತಿಜ್ಞಾ(h);

        h->dirty = true;
}
#define hashmap_set_dirty(h) base_set_dirty(HASHMAP_BASE(h))

ಸ್ಥಿರ ಶೂನ್ಯ get_hash_ಕೀಲಿ(uint8_t hash_ಕೀಲಿ[HASH_ಕೀಲಿ_SIZE], bool reuse_is_ok) {
        ಸ್ಥಿರ uint8_t current[HASH_ಕೀಲಿ_SIZE];
        ಸ್ಥಿರ bool current_initialized = false;

        /* Returns a hash function key to use. In order to keep things
         * fast we will not generate a new key each time we allocate a
         * new hash table. Instead, we'll just reuse the most recently
         * generated one, except if we never generated one or when we
         * are rehashing an entire hash table because we reached a
         * fill level */

        ಯದಿ (!current_initialized || !reuse_is_ok) {
                random_bytes(current, ಪರಿಮಾಣ(current));
                current_initialized = true;
        }

        memcpy(hash_ಕೀಲಿ, current, ಪರಿಮಾಣ(current));
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಸಂರಚನಾ hashmap_base_entry* bucket_at(HashmapBase *h, unsigned idx) {
        ಪ್ರತಿಜ್ಞಾ(h);
        ಪ್ರತ್ಯಾಗಮ CAST_ALIGN_PTR(
                        ಸಂರಚನಾ hashmap_base_entry,
                        (uint8_t *) storage_ptr(h) + idx * hashmap_type_info[h->type].entry_size);
}

ಸ್ಥಿರ ಸಂರಚನಾ plain_hashmap_entry* plain_bucket_at(Hashmap *h, unsigned idx) {
        ಪ್ರತ್ಯಾಗಮ (ಸಂರಚನಾ plain_hashmap_entry*) bucket_at(HASHMAP_BASE(h), idx);
}

ಸ್ಥಿರ ಸಂರಚನಾ ordered_hashmap_entry* ordered_bucket_at(OrderedHashmap *h, unsigned idx) {
        ಪ್ರತ್ಯಾಗಮ (ಸಂರಚನಾ ordered_hashmap_entry*) bucket_at(HASHMAP_BASE(h), idx);
}

ಸ್ಥಿರ ಸಂರಚನಾ set_entry *set_bucket_at(Set *h, unsigned idx) {
        ಪ್ರತ್ಯಾಗಮ (ಸಂರಚನಾ set_entry*) bucket_at(HASHMAP_BASE(h), idx);
}

ಸ್ಥಿರ ಸಂರಚನಾ ordered_hashmap_entry* bucket_at_swap(ಸಂರಚನಾ swap_entries *swap, unsigned idx) {
        ಪ್ರತಿಜ್ಞಾ(swap);
        ಪ್ರತ್ಯಾಗಮ &swap->e[idx - _IDX_SWAP_BEGIN];
}

/* Returns a pointer to the bucket at index idx.
 * Understands real indexes and swap indexes, hence "_virtual". */
ಸ್ಥಿರ ಸಂರಚನಾ hashmap_base_entry* bucket_at_virtual(HashmapBase *h, ಸಂರಚನಾ swap_entries *swap,
                                                    unsigned idx) {
        ಯದಿ (idx < _IDX_SWAP_BEGIN)
                ಪ್ರತ್ಯಾಗಮ bucket_at(h, idx);

        ಯದಿ (idx < _IDX_SWAP_END)
                ಪ್ರತ್ಯಾಗಮ &bucket_at_swap(swap, idx)->p.b;

        assert_not_reached();
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ dib_raw_t* dib_raw_ptr(HashmapBase *h) {
        ಪ್ರತಿಜ್ಞಾ(h);
        ಪ್ರತ್ಯಾಗಮ (dib_raw_t*)
                ((uint8_t*) storage_ptr(h) + hashmap_type_info[h->type].entry_size * n_buckets(h));
}

ಸ್ಥಿರ unsigned bucket_distance(HashmapBase *h, unsigned idx, unsigned from) {
        ಪ್ರತ್ಯಾಗಮ idx >= from ? idx - from
                           : n_buckets(h) + idx - from;
}

ಸ್ಥಿರ unsigned bucket_calculate_dib(HashmapBase *h, unsigned idx, dib_raw_t raw_dib) {
        unsigned initial_bucket;

        ಯದಿ (raw_dib == DIB_RAW_FREE)
                ಪ್ರತ್ಯಾಗಮ DIB_FREE;

        ಯದಿ (_likely_(raw_dib < DIB_RAW_OVERFLOW))
                ಪ್ರತ್ಯಾಗಮ raw_dib;

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
        initial_bucket = bucket_hash(h, bucket_at(h, idx)->ಕೀಲಿ);
        ಪ್ರತ್ಯಾಗಮ bucket_distance(h, idx, initial_bucket);
}

ಸ್ಥಿರ ಶೂನ್ಯ bucket_set_dib(HashmapBase *h, unsigned idx, unsigned dib) {
        dib_raw_ptr(h)[idx] = dib != DIB_FREE ? MIN(dib, DIB_RAW_OVERFLOW) : DIB_RAW_FREE;
}

ಸ್ಥಿರ unsigned skip_free_buckets(HashmapBase *h, unsigned idx) {
        dib_raw_t *dibs;

        dibs = dib_raw_ptr(h);

        ಪ್ರತಿ ( ; idx < n_buckets(h); idx++)
                ಯದಿ (dibs[idx] != DIB_RAW_FREE)
                        ಪ್ರತ್ಯಾಗಮ idx;

        ಪ್ರತ್ಯಾಗಮ IDX_NIL;
}

ಸ್ಥಿರ ಶೂನ್ಯ bucket_mark_free(HashmapBase *h, unsigned idx) {
        ಪ್ರತಿಜ್ಞಾ(h);

        memzero(bucket_at(h, idx), hashmap_type_info[h->type].entry_size);
        bucket_set_dib(h, idx, DIB_FREE);
}

ಸ್ಥಿರ ಶೂನ್ಯ bucket_move_entry(HashmapBase *h, ಸಂರಚನಾ swap_entries *swap,
                              unsigned from, unsigned to) {
        ಸಂರಚನಾ hashmap_base_entry *e_from, *e_to;

        ಪ್ರತಿಜ್ಞಾ(h);
        ಪ್ರತಿಜ್ಞಾ(from != to);

        e_from = bucket_at_virtual(h, swap, from);
        e_to   = bucket_at_virtual(h, swap, to);

        memcpy(e_to, e_from, hashmap_type_info[h->type].entry_size);

        ಯದಿ (h->type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*) h;
                ಸಂರಚನಾ ordered_hashmap_entry *le, *le_to;

                le_to = (ಸಂರಚನಾ ordered_hashmap_entry*) e_to;

                ಯದಿ (le_to->iterate_next != IDX_NIL) {
                        le = (ಸಂರಚನಾ ordered_hashmap_entry*)
                             bucket_at_virtual(h, swap, le_to->iterate_next);
                        le->iterate_previous = to;
                }

                ಯದಿ (le_to->iterate_previous != IDX_NIL) {
                        le = (ಸಂರಚನಾ ordered_hashmap_entry*)
                             bucket_at_virtual(h, swap, le_to->iterate_previous);
                        le->iterate_next = to;
                }

                ಯದಿ (lh->iterate_list_head == from)
                        lh->iterate_list_head = to;
                ಯದಿ (lh->iterate_list_tail == from)
                        lh->iterate_list_tail = to;
        }
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ unsigned next_idx(HashmapBase *h, unsigned idx) {
        ಪ್ರತ್ಯಾಗಮ idx + 1U < n_buckets(h) ? idx + 1U : 0U;
}

ಸ್ಥಿರ unsigned prev_idx(HashmapBase *h, unsigned idx) {
        ಪ್ರತ್ಯಾಗಮ idx > 0U ? idx - 1U : n_buckets(h) - 1U;
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ* entry_ಮೌಲ್ಯ(HashmapBase *h, ಸಂರಚನಾ hashmap_base_entry *e) {
        ಪ್ರತಿಜ್ಞಾ(h);
        ಪ್ರತಿಜ್ಞಾ(e);

        ವಿಕಲ್ಪ (h->type) {

        ಪಕ್ಷ HASHMAP_TYPE_PLAIN:
        ಪಕ್ಷ HASHMAP_TYPE_ORDERED:
                ಪ್ರತ್ಯಾಗಮ ((ಸಂರಚನಾ plain_hashmap_entry*)e)->ಮೌಲ್ಯ;

        ಪಕ್ಷ HASHMAP_TYPE_SET:
                ಪ್ರತ್ಯಾಗಮ (ಶೂನ್ಯ*) e->ಕೀಲಿ;

        ಸ್ವಭಾವ:
                assert_not_reached();
        }
}

ಸ್ಥಿರ ಶೂನ್ಯ base_remove_entry(HashmapBase *h, unsigned idx) {
        unsigned ಎಡ, ಬಲ, prev, dib;
        dib_raw_t raw_dib, *dibs;

        dibs = dib_raw_ptr(h);
        ಪ್ರತಿಜ್ಞಾ(dibs[idx] != DIB_RAW_FREE);

#ಯದಿ ENABLE_DEBUG_HASHMAP
        ಪ್ರತಿಜ್ಞಾ(h);
        h->debug.rem_ಎಣಿಕೆ++;
        h->debug.last_rem_idx = idx;
#endif

        ಎಡ = idx;
        /* Find the stop bucket ("right"). It is either free or has DIB == 0. */
        ಪ್ರತಿ (ಬಲ = next_idx(h, ಎಡ); ; ಬಲ = next_idx(h, ಬಲ)) {
                raw_dib = dibs[ಬಲ];
                ಯದಿ (IN_SET(raw_dib, 0, DIB_RAW_FREE))
                        ಭಙ್ಗ;

                /* The buckets are not supposed to be all occupied and with DIB > 0.
                 * That would mean we could make everyone better off by shifting them
                 * backward. This scenario is impossible. */
                ಪ್ರತಿಜ್ಞಾ(ಎಡ != ಬಲ);
        }

        ಯದಿ (h->type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*) h;
                ಸಂರಚನಾ ordered_hashmap_entry *le = ordered_bucket_at(lh, idx);

                ಯದಿ (le->iterate_next != IDX_NIL)
                        ordered_bucket_at(lh, le->iterate_next)->iterate_previous = le->iterate_previous;
                ಅನ್ಯಥಾ
                        lh->iterate_list_tail = le->iterate_previous;

                ಯದಿ (le->iterate_previous != IDX_NIL)
                        ordered_bucket_at(lh, le->iterate_previous)->iterate_next = le->iterate_next;
                ಅನ್ಯಥಾ
                        lh->iterate_list_head = le->iterate_next;
        }

        /* Now shift all buckets in the interval (left, right) one step backwards */
        ಪ್ರತಿ (prev = ಎಡ, ಎಡ = next_idx(h, ಎಡ); ಎಡ != ಬಲ;
             prev = ಎಡ, ಎಡ = next_idx(h, ಎಡ)) {
                dib = bucket_calculate_dib(h, ಎಡ, dibs[ಎಡ]);
                ಪ್ರತಿಜ್ಞಾ(dib != 0);
                bucket_move_entry(h, ರಿಕ್ತ, ಎಡ, prev);
                bucket_set_dib(h, prev, dib - 1);
        }

        bucket_mark_free(h, prev);
        n_entries_dec(h);
        base_set_dirty(h);
}
#define remove_entry(h, idx) base_remove_entry(HASHMAP_BASE(h), idx)

ಸ್ಥಿರ unsigned hashmap_iterate_in_insertion_order(OrderedHashmap *h, Iterator *i) {
        ಸಂರಚನಾ ordered_hashmap_entry *e;
        unsigned idx;

        ಪ್ರತಿಜ್ಞಾ(h);
        ಪ್ರತಿಜ್ಞಾ(i);

        ಯದಿ (i->idx == IDX_NIL)
                ಗಚ್ಛ at_end;

        ಯದಿ (i->idx == IDX_FIRST && h->iterate_list_head == IDX_NIL)
                ಗಚ್ಛ at_end;

        ಯದಿ (i->idx == IDX_FIRST) {
                idx = h->iterate_list_head;
                e = ordered_bucket_at(h, idx);
        } ಅನ್ಯಥಾ {
                idx = i->idx;
                e = ordered_bucket_at(h, idx);
                /*
                 * We allow removing the current entry while iterating, but removal may cause
                 * a backward shift. The next entry may thus move one bucket to the left.
                 * To detect when it happens, we remember the key pointer of the entry we were
                 * going to iterate next. If it does not match, there was a backward shift.
                 */
                ಯದಿ (e->p.b.ಕೀಲಿ != i->next_ಕೀಲಿ) {
                        idx = prev_idx(HASHMAP_BASE(h), idx);
                        e = ordered_bucket_at(h, idx);
                }
                ಪ್ರತಿಜ್ಞಾ(e->p.b.ಕೀಲಿ == i->next_ಕೀಲಿ);
        }

#ಯದಿ ENABLE_DEBUG_HASHMAP
        i->prev_idx = idx;
#endif

        ಯದಿ (e->iterate_next != IDX_NIL) {
                ಸಂರಚನಾ ordered_hashmap_entry *n;
                i->idx = e->iterate_next;
                n = ordered_bucket_at(h, i->idx);
                i->next_ಕೀಲಿ = n->p.b.ಕೀಲಿ;
        } ಅನ್ಯಥಾ
                i->idx = IDX_NIL;

        ಪ್ರತ್ಯಾಗಮ idx;

at_end:
        i->idx = IDX_NIL;
        ಪ್ರತ್ಯಾಗಮ IDX_NIL;
}

ಸ್ಥಿರ unsigned hashmap_iterate_in_internal_order(HashmapBase *h, Iterator *i) {
        unsigned idx;

        ಪ್ರತಿಜ್ಞಾ(h);
        ಪ್ರತಿಜ್ಞಾ(i);

        ಯದಿ (i->idx == IDX_NIL)
                ಗಚ್ಛ at_end;

        ಯದಿ (i->idx == IDX_FIRST) {
                /* fast forward to the first occupied bucket */
                ಯದಿ (h->has_indirect) {
                        i->idx = skip_free_buckets(h, h->indirect.idx_lowest_entry);
                        h->indirect.idx_lowest_entry = i->idx;
                } ಅನ್ಯಥಾ
                        i->idx = skip_free_buckets(h, 0);

                ಯದಿ (i->idx == IDX_NIL)
                        ಗಚ್ಛ at_end;
        } ಅನ್ಯಥಾ {
                ಸಂರಚನಾ hashmap_base_entry *e;

                ಪ್ರತಿಜ್ಞಾ(i->idx > 0);

                e = bucket_at(h, i->idx);
                /*
                 * We allow removing the current entry while iterating, but removal may cause
                 * a backward shift. The next entry may thus move one bucket to the left.
                 * To detect when it happens, we remember the key pointer of the entry we were
                 * going to iterate next. If it does not match, there was a backward shift.
                 */
                ಯದಿ (e->ಕೀಲಿ != i->next_ಕೀಲಿ)
                        e = bucket_at(h, --i->idx);

                ಪ್ರತಿಜ್ಞಾ(e->ಕೀಲಿ == i->next_ಕೀಲಿ);
        }

        idx = i->idx;
#ಯದಿ ENABLE_DEBUG_HASHMAP
        i->prev_idx = idx;
#endif

        i->idx = skip_free_buckets(h, i->idx + 1);
        ಯದಿ (i->idx != IDX_NIL)
                i->next_ಕೀಲಿ = bucket_at(h, i->idx)->ಕೀಲಿ;
        ಅನ್ಯಥಾ
                i->idx = IDX_NIL;

        ಪ್ರತ್ಯಾಗಮ idx;

at_end:
        i->idx = IDX_NIL;
        ಪ್ರತ್ಯಾಗಮ IDX_NIL;
}

ಸ್ಥಿರ unsigned hashmap_iterate_entry(HashmapBase *h, Iterator *i) {
        ಯದಿ (!h) {
                i->idx = IDX_NIL;
                ಪ್ರತ್ಯಾಗಮ IDX_NIL;
        }

#ಯದಿ ENABLE_DEBUG_HASHMAP
        ಯದಿ (i->idx == IDX_FIRST) {
                i->put_ಎಣಿಕೆ = h->debug.put_ಎಣಿಕೆ;
                i->rem_ಎಣಿಕೆ = h->debug.rem_ಎಣಿಕೆ;
        } ಅನ್ಯಥಾ {
                /* While iterating, must not add any new entries */
                ಪ್ರತಿಜ್ಞಾ(i->put_ಎಣಿಕೆ == h->debug.put_ಎಣಿಕೆ);
                /* ... or remove entries other than the current one */
                ಪ್ರತಿಜ್ಞಾ(i->rem_ಎಣಿಕೆ == h->debug.rem_ಎಣಿಕೆ ||
                       (i->rem_ಎಣಿಕೆ == h->debug.rem_ಎಣಿಕೆ - 1 &&
                        i->prev_idx == h->debug.last_rem_idx));
                /* Reset our removals counter */
                i->rem_ಎಣಿಕೆ = h->debug.rem_ಎಣಿಕೆ;
        }
#endif

        ಪ್ರತ್ಯಾಗಮ h->type == HASHMAP_TYPE_ORDERED ? hashmap_iterate_in_insertion_order((OrderedHashmap*) h, i)
                                               : hashmap_iterate_in_internal_order(h, i);
}

bool _hashmap_iterate(HashmapBase *h, Iterator *i, ಶೂನ್ಯ **ಮೌಲ್ಯ, ಧ್ರುವ ಶೂನ್ಯ **ಕೀಲಿ) {
        ಸಂರಚನಾ hashmap_base_entry *e;
        ಶೂನ್ಯ *data;
        unsigned idx;

        idx = hashmap_iterate_entry(h, i);
        ಯದಿ (idx == IDX_NIL) {
                ಯದಿ (ಮೌಲ್ಯ)
                        *ಮೌಲ್ಯ = ರಿಕ್ತ;
                ಯದಿ (ಕೀಲಿ)
                        *ಕೀಲಿ = ರಿಕ್ತ;

                ಪ್ರತ್ಯಾಗಮ false;
        }

        e = bucket_at(h, idx);
        data = entry_ಮೌಲ್ಯ(h, e);
        ಯದಿ (ಮೌಲ್ಯ)
                *ಮೌಲ್ಯ = data;
        ಯದಿ (ಕೀಲಿ)
                *ಕೀಲಿ = e->ಕೀಲಿ;

        ಪ್ರತ್ಯಾಗಮ true;
}

#define HASHMAP_FOREACH_IDX(idx, h, i) \
        ಪ್ರತಿ ((i) = ITERATOR_FIRST, (idx) = hashmap_iterate_entry((h), &(i)); \
             (idx != IDX_NIL); \
             (idx) = hashmap_iterate_entry((h), &(i)))

IteratedCache* _hashmap_iterated_cache_new(HashmapBase *h) {
        IteratedCache *cache;

        ಪ್ರತಿಜ್ಞಾ(h);
        ಪ್ರತಿಜ್ಞಾ(!h->cached);

        ಯದಿ (h->cached)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        cache = new0(IteratedCache, 1);
        ಯದಿ (!cache)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        cache->hashmap = h;
        h->cached = true;

        ಪ್ರತ್ಯಾಗಮ cache;
}

ಸ್ಥಿರ ಶೂನ್ಯ reset_direct_storage(HashmapBase *h) {
        ಧ್ರುವ ಸಂರಚನಾ hashmap_type_info *hi = &hashmap_type_info[h->type];
        ಶೂನ್ಯ *p;

        ಪ್ರತಿಜ್ಞಾ(!h->has_indirect);

        p = mempset(h->direct.storage, 0, hi->entry_size * hi->n_direct_buckets);
        memset(p, DIB_RAW_INIT, ಪರಿಮಾಣ(dib_raw_t) * hi->n_direct_buckets);
}

ಸ್ಥಿರ ಶೂನ್ಯ shared_hash_ಕೀಲಿ_initialize(ಶೂನ್ಯ) {
        random_bytes(shared_hash_ಕೀಲಿ, ಪರಿಮಾಣ(shared_hash_ಕೀಲಿ));
}

ಸ್ಥಿರ ಸಂರಚನಾ HashmapBase* hashmap_base_new(ಧ್ರುವ ಸಂರಚನಾ hash_ops *hash_ops, ಗಣನಾ HashmapType type) {
        HashmapBase *h;
        ಧ್ರುವ ಸಂರಚನಾ hashmap_type_info *hi = &hashmap_type_info[type];

        bool use_pool = mempool_enabled && mempool_enabled();  /* mempool_enabled is a weak symbol */

        h = use_pool ? mempool_alloc0_tile(hi->mempool) : malloc0(hi->head_size);
        ಯದಿ (!h)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        h->type = type;
        h->from_pool = use_pool;
        h->hash_ops = hash_ops ?: &trivial_hash_ops;

        ಯದಿ (type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*)h;
                lh->iterate_list_head = lh->iterate_list_tail = IDX_NIL;
        }

        reset_direct_storage(h);

        ಸ್ಥಿರ pthread_once_t once = PTHREAD_ONCE_INIT;
        assert_se(pthread_once(&once, shared_hash_ಕೀಲಿ_initialize) == 0);

#ಯದಿ ENABLE_DEBUG_HASHMAP
        assert_se(pthread_mutex_lock(&hashmap_debug_list_mutex) == 0);
        LIST_PREPEND(debug_list, hashmap_debug_list, &h->debug);
        assert_se(pthread_mutex_unlock(&hashmap_debug_list_mutex) == 0);
#endif

        ಪ್ರತ್ಯಾಗಮ h;
}

Hashmap *hashmap_new(ಧ್ರುವ ಸಂರಚನಾ hash_ops *hash_ops) {
        ಪ್ರತ್ಯಾಗಮ (Hashmap*)        hashmap_base_new(hash_ops, HASHMAP_TYPE_PLAIN);
}

OrderedHashmap *ordered_hashmap_new(ಧ್ರುವ ಸಂರಚನಾ hash_ops *hash_ops) {
        ಪ್ರತ್ಯಾಗಮ (OrderedHashmap*) hashmap_base_new(hash_ops, HASHMAP_TYPE_ORDERED);
}

Set *set_new(ಧ್ರುವ ಸಂರಚನಾ hash_ops *hash_ops) {
        ಪ್ರತ್ಯಾಗಮ (Set*)            hashmap_base_new(hash_ops, HASHMAP_TYPE_SET);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ hashmap_base_ensure_allocated(HashmapBase **h, ಧ್ರುವ ಸಂರಚನಾ hash_ops *hash_ops,
                                         ಗಣನಾ HashmapType type) {
        HashmapBase *q;

        ಪ್ರತಿಜ್ಞಾ(h);

        ಯದಿ (*h) {
                ಪ್ರತಿಜ್ಞಾ((*h)->hash_ops == (hash_ops ?: &trivial_hash_ops));
                ಪ್ರತ್ಯಾಗಮ 0;
        }

        q = hashmap_base_new(hash_ops, type);
        ಯದಿ (!q)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        *h = q;
        ಪ್ರತ್ಯಾಗಮ 1;
}

ಪೂರ್ಣಾಙ್ಕ hashmap_ensure_allocated(Hashmap **h, ಧ್ರುವ ಸಂರಚನಾ hash_ops *hash_ops) {
        ಪ್ರತ್ಯಾಗಮ hashmap_base_ensure_allocated((HashmapBase**)h, hash_ops, HASHMAP_TYPE_PLAIN);
}

ಪೂರ್ಣಾಙ್ಕ ordered_hashmap_ensure_allocated(OrderedHashmap **h, ಧ್ರುವ ಸಂರಚನಾ hash_ops *hash_ops) {
        ಪ್ರತ್ಯಾಗಮ hashmap_base_ensure_allocated((HashmapBase**)h, hash_ops, HASHMAP_TYPE_ORDERED);
}

ಪೂರ್ಣಾಙ್ಕ set_ensure_allocated(Set **s, ಧ್ರುವ ಸಂರಚನಾ hash_ops *hash_ops) {
        ಪ್ರತ್ಯಾಗಮ hashmap_base_ensure_allocated((HashmapBase**)s, hash_ops, HASHMAP_TYPE_SET);
}

ಪೂರ್ಣಾಙ್ಕ hashmap_ensure_put(Hashmap **h, ಧ್ರುವ ಸಂರಚನಾ hash_ops *hash_ops, ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ, ಶೂನ್ಯ *ಮೌಲ್ಯ) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(h);

        r = hashmap_ensure_allocated(h, hash_ops);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ hashmap_put(*h, ಕೀಲಿ, ಮೌಲ್ಯ);
}

ಪೂರ್ಣಾಙ್ಕ ordered_hashmap_ensure_put(OrderedHashmap **h, ಧ್ರುವ ಸಂರಚನಾ hash_ops *hash_ops, ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ, ಶೂನ್ಯ *ಮೌಲ್ಯ) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(h);

        r = ordered_hashmap_ensure_allocated(h, hash_ops);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ ordered_hashmap_put(*h, ಕೀಲಿ, ಮೌಲ್ಯ);
}

ಪೂರ್ಣಾಙ್ಕ ordered_hashmap_ensure_replace(OrderedHashmap **h, ಧ್ರುವ ಸಂರಚನಾ hash_ops *hash_ops, ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ, ಶೂನ್ಯ *ಮೌಲ್ಯ) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(h);

        r = ordered_hashmap_ensure_allocated(h, hash_ops);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ ordered_hashmap_replace(*h, ಕೀಲಿ, ಮೌಲ್ಯ);
}

ಪೂರ್ಣಾಙ್ಕ hashmap_ensure_replace(Hashmap **h, ಧ್ರುವ ಸಂರಚನಾ hash_ops *hash_ops, ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ, ಶೂನ್ಯ *ಮೌಲ್ಯ) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(h);

        r = hashmap_ensure_allocated(h, hash_ops);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ hashmap_replace(*h, ಕೀಲಿ, ಮೌಲ್ಯ);
}

ಸ್ಥಿರ ಶೂನ್ಯ hashmap_free_no_clear(HashmapBase *h) {
        ಪ್ರತಿಜ್ಞಾ(!h->has_indirect);
        ಪ್ರತಿಜ್ಞಾ(h->n_direct_entries == 0);

#ಯದಿ ENABLE_DEBUG_HASHMAP
        assert_se(pthread_mutex_lock(&hashmap_debug_list_mutex) == 0);
        LIST_REMOVE(debug_list, hashmap_debug_list, &h->debug);
        assert_se(pthread_mutex_unlock(&hashmap_debug_list_mutex) == 0);
#endif

        ಯದಿ (h->from_pool) {
                /* Ensure that the object didn't get migrated between threads. */
                assert_se(is_main_thread());
                mempool_free_tile(hashmap_type_info[h->type].mempool, h);
        } ಅನ್ಯಥಾ
                ಮುಕ್ತ(h);
}

HashmapBase* _hashmap_free(HashmapBase *h) {
        ಯದಿ (h) {
                _hashmap_clear(h);
                hashmap_free_no_clear(h);
        }

        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

ಶೂನ್ಯ _hashmap_clear(HashmapBase *h) {
        ಯದಿ (!h)
                ಪ್ರತ್ಯಾಗಮ;

        ಯದಿ (h->hash_ops->free_ಕೀಲಿ || h->hash_ops->free_ಮೌಲ್ಯ) {

                /* If destructor calls are defined, let's destroy things defensively: let's take the item out of the
                 * hash table, and only then call the destructor functions. If these destructors then try to unregister
                 * themselves from our hash table a second time, the entry is already gone. */

                ಯಾವತ್ (_hashmap_size(h) > 0) {
                        ಶೂನ್ಯ *k = ರಿಕ್ತ;
                        ಶೂನ್ಯ *v;

                        v = hashmap_first_ಕೀಲಿ_and_ಮೌಲ್ಯ(h, true, &k);

                        ಯದಿ (h->hash_ops->free_ಕೀಲಿ)
                                h->hash_ops->free_ಕೀಲಿ(k);

                        ಯದಿ (h->hash_ops->free_ಮೌಲ್ಯ)
                                h->hash_ops->free_ಮೌಲ್ಯ(v);
                }
        }

        ಯದಿ (h->has_indirect) {
                ಮುಕ್ತ(h->indirect.storage);
                h->has_indirect = false;
        }

        h->n_direct_entries = 0;
        reset_direct_storage(h);

        ಯದಿ (h->type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*) h;
                lh->iterate_list_head = lh->iterate_list_tail = IDX_NIL;
        }

        base_set_dirty(h);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ resize_buckets(HashmapBase *h, unsigned entries_add);

/*
 * Finds an empty bucket to put an entry into, starting the scan at 'idx'.
 * Performs Robin Hood swaps as it goes. The entry to put must be placed
 * by the caller into swap slot IDX_PUT.
 * If used for in-place resizing, may leave a displaced entry in swap slot
 * IDX_PUT. Caller must rehash it next.
 * Returns: true if it left a displaced entry to rehash next in IDX_PUT,
 *          false otherwise.
 */
ಸ್ಥಿರ bool hashmap_put_robin_hood(HashmapBase *h, unsigned idx,
                                   ಸಂರಚನಾ swap_entries *swap) {
        dib_raw_t raw_dib, *dibs;
        unsigned dib, distance;

#ಯದಿ ENABLE_DEBUG_HASHMAP
        ಪ್ರತಿಜ್ಞಾ(h);
        h->debug.put_ಎಣಿಕೆ++;
#endif

        dibs = dib_raw_ptr(h);

        ಪ್ರತಿ (distance = 0; ; distance++) {
                raw_dib = dibs[idx];
                ಯದಿ (IN_SET(raw_dib, DIB_RAW_FREE, DIB_RAW_REHASH)) {
                        ಯದಿ (raw_dib == DIB_RAW_REHASH)
                                bucket_move_entry(h, swap, idx, IDX_TMP);

                        ಯದಿ (h->has_indirect && h->indirect.idx_lowest_entry > idx)
                                h->indirect.idx_lowest_entry = idx;

                        bucket_set_dib(h, idx, distance);
                        bucket_move_entry(h, swap, IDX_PUT, idx);
                        ಯದಿ (raw_dib == DIB_RAW_REHASH) {
                                bucket_move_entry(h, swap, IDX_TMP, IDX_PUT);
                                ಪ್ರತ್ಯಾಗಮ true;
                        }

                        ಪ್ರತ್ಯಾಗಮ false;
                }

                dib = bucket_calculate_dib(h, idx, raw_dib);

                ಯದಿ (dib < distance) {
                        /* Found a wealthier entry. Go Robin Hood! */
                        bucket_set_dib(h, idx, distance);

                        /* swap the entries */
                        bucket_move_entry(h, swap, idx, IDX_TMP);
                        bucket_move_entry(h, swap, IDX_PUT, idx);
                        bucket_move_entry(h, swap, IDX_TMP, IDX_PUT);

                        distance = dib;
                }

                idx = next_idx(h, idx);
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
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ hashmap_base_put_boldly(HashmapBase *h, unsigned idx,
                                   ಸಂರಚನಾ swap_entries *swap, bool may_resize) {
        ಸಂರಚನಾ ordered_hashmap_entry *new_entry;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(idx < n_buckets(h));

        new_entry = bucket_at_swap(swap, IDX_PUT);

        ಯದಿ (may_resize) {
                r = resize_buckets(h, 1);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;
                ಯದಿ (r > 0)
                        idx = bucket_hash(h, new_entry->p.b.ಕೀಲಿ);
        }
        ಪ್ರತಿಜ್ಞಾ(n_entries(h) < n_buckets(h));

        ಯದಿ (h->type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*) h;

                new_entry->iterate_next = IDX_NIL;
                new_entry->iterate_previous = lh->iterate_list_tail;

                ಯದಿ (lh->iterate_list_tail != IDX_NIL) {
                        ಸಂರಚನಾ ordered_hashmap_entry *old_tail;

                        old_tail = ordered_bucket_at(lh, lh->iterate_list_tail);
                        ಪ್ರತಿಜ್ಞಾ(old_tail->iterate_next == IDX_NIL);
                        old_tail->iterate_next = IDX_PUT;
                }

                lh->iterate_list_tail = IDX_PUT;
                ಯದಿ (lh->iterate_list_head == IDX_NIL)
                        lh->iterate_list_head = IDX_PUT;
        }

        assert_se(hashmap_put_robin_hood(h, idx, swap) == false);

        n_entries_inc(h);
#ಯದಿ ENABLE_DEBUG_HASHMAP
        h->debug.max_entries = MAX(h->debug.max_entries, n_entries(h));
#endif

        base_set_dirty(h);

        ಪ್ರತ್ಯಾಗಮ 1;
}
#define hashmap_put_boldly(h, idx, swap, may_resize) \
        hashmap_base_put_boldly(HASHMAP_BASE(h), idx, swap, may_resize)

/*
 * Returns 0 if resize is not needed.
 *         1 if successfully resized.
 *         -ENOMEM on allocation failure.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ resize_buckets(HashmapBase *h, unsigned entries_add) {
        ಸಂರಚನಾ swap_entries swap;
        ಶೂನ್ಯ *new_storage;
        dib_raw_t *old_dibs, *new_dibs;
        ಧ್ರುವ ಸಂರಚನಾ hashmap_type_info *hi;
        unsigned idx, optimal_idx;
        unsigned old_n_buckets, new_n_buckets, n_rehashed, new_n_entries;
        uint8_t new_shift;
        bool rehash_next;

        ಪ್ರತಿಜ್ಞಾ(h);

        hi = &hashmap_type_info[h->type];
        new_n_entries = n_entries(h) + entries_add;

        /* overflow? */
        ಯದಿ (_unlikely_(new_n_entries < entries_add))
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        /* For direct storage we allow 100% load, because it's tiny. */
        ಯದಿ (!h->has_indirect && new_n_entries <= hi->n_direct_buckets)
                ಪ್ರತ್ಯಾಗಮ 0;

        /*
         * Load factor = n/m = 1 - (1/INV_KEEP_FREE).
         * From it follows: m = n + n/(INV_KEEP_FREE - 1)
         */
        new_n_buckets = new_n_entries + new_n_entries / (INV_KEEP_FREE - 1);
        /* overflow? */
        ಯದಿ (_unlikely_(new_n_buckets < new_n_entries))
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        old_n_buckets = n_buckets(h);

        ಯದಿ (_likely_(new_n_buckets <= old_n_buckets))
                ಪ್ರತ್ಯಾಗಮ 0;

        ಯದಿ (_unlikely_(new_n_buckets > UINT_MAX / (hi->entry_size + ಪರಿಮಾಣ(dib_raw_t))))
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        new_shift = log2u_round_up(MAX(
                        new_n_buckets * (hi->entry_size + ಪರಿಮಾಣ(dib_raw_t)),
                        2 * ಪರಿಮಾಣ(ಸಂರಚನಾ direct_storage)));

        /* Realloc storage (buckets and DIB array). */
        new_storage = realloc(h->has_indirect ? h->indirect.storage : ರಿಕ್ತ,
                              1U << new_shift);
        ಯದಿ (!new_storage)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        /* Must upgrade direct to indirect storage. */
        ಯದಿ (!h->has_indirect) {
                memcpy(new_storage, h->direct.storage,
                       old_n_buckets * (hi->entry_size + ಪರಿಮಾಣ(dib_raw_t)));
                h->indirect.n_entries = h->n_direct_entries;
                h->indirect.idx_lowest_entry = 0;
                h->n_direct_entries = 0;
        }

        /* Get a new hash key. If we've just upgraded to indirect storage,
         * allow reusing a previously generated key. It's still a different key
         * from the shared one that we used for direct storage. */
        get_hash_ಕೀಲಿ(h->indirect.hash_ಕೀಲಿ, !h->has_indirect);

        h->has_indirect = true;
        h->indirect.storage = new_storage;
        h->indirect.n_buckets = (1U << new_shift) /
                                (hi->entry_size + ಪರಿಮಾಣ(dib_raw_t));

        old_dibs = (dib_raw_t*)((uint8_t*) new_storage + hi->entry_size * old_n_buckets);
        new_dibs = dib_raw_ptr(h);

        /*
         * Move the DIB array to the new place, replacing valid DIB values with
         * DIB_RAW_REHASH to indicate all of the used buckets need rehashing.
         * Note: Overlap is not possible, because we have at least doubled the
         * number of buckets and dib_raw_t is smaller than any entry type.
         */
        ಪ್ರತಿ (idx = 0; idx < old_n_buckets; idx++) {
                ಪ್ರತಿಜ್ಞಾ(old_dibs[idx] != DIB_RAW_REHASH);
                new_dibs[idx] = old_dibs[idx] == DIB_RAW_FREE ? DIB_RAW_FREE
                                                              : DIB_RAW_REHASH;
        }

        /* Zero the area of newly added entries (including the old DIB area) */
        memzero(bucket_at(h, old_n_buckets),
               (n_buckets(h) - old_n_buckets) * hi->entry_size);

        /* The upper half of the new DIB array needs initialization */
        memset(&new_dibs[old_n_buckets], DIB_RAW_INIT,
               (n_buckets(h) - old_n_buckets) * ಪರಿಮಾಣ(dib_raw_t));

        /* Rehash entries that need it */
        n_rehashed = 0;
        ಪ್ರತಿ (idx = 0; idx < old_n_buckets; idx++) {
                ಯದಿ (new_dibs[idx] != DIB_RAW_REHASH)
                        ಅನುವರ್ತ;

                optimal_idx = bucket_hash(h, bucket_at(h, idx)->ಕೀಲಿ);

                /*
                 * Not much to do if by luck the entry hashes to its current
                 * location. Just set its DIB.
                 */
                ಯದಿ (optimal_idx == idx) {
                        new_dibs[idx] = 0;
                        n_rehashed++;
                        ಅನುವರ್ತ;
                }

                new_dibs[idx] = DIB_RAW_FREE;
                bucket_move_entry(h, &swap, idx, IDX_PUT);
                /* bucket_move_entry does not clear the source */
                memzero(bucket_at(h, idx), hi->entry_size);

                ಕುರು {
                        /*
                         * Find the new bucket for the current entry. This may make
                         * another entry homeless and load it into IDX_PUT.
                         */
                        rehash_next = hashmap_put_robin_hood(h, optimal_idx, &swap);
                        n_rehashed++;

                        /* Did the current entry displace another one? */
                        ಯದಿ (rehash_next)
                                optimal_idx = bucket_hash(h, bucket_at_swap(&swap, IDX_PUT)->p.b.ಕೀಲಿ);
                } ಯಾವತ್ (rehash_next);
        }

        assert_se(n_rehashed == n_entries(h));

        ಪ್ರತ್ಯಾಗಮ 1;
}

/*
 * Finds an entry with a matching key
 * Returns: index of the found entry, or IDX_NIL if not found.
 */
ಸ್ಥಿರ unsigned base_bucket_scan(HashmapBase *h, unsigned idx, ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ) {
        compare_func_t compare = h->hash_ops->compare;
        ಸಂರಚನಾ hashmap_base_entry *e;
        unsigned dib, distance;
        dib_raw_t *dibs = dib_raw_ptr(h);

        ಪ್ರತಿಜ್ಞಾ(idx < n_buckets(h));

        ಪ್ರತಿ (distance = 0; ; distance++) {
                ಯದಿ (dibs[idx] == DIB_RAW_FREE)
                        ಪ್ರತ್ಯಾಗಮ IDX_NIL;

                dib = bucket_calculate_dib(h, idx, dibs[idx]);

                ಯದಿ (dib < distance)
                        ಪ್ರತ್ಯಾಗಮ IDX_NIL;
                ಯದಿ (dib == distance) {
                        e = bucket_at(h, idx);

                        /* Most maps compare keys by pointer. Do that here, without the indirect call. */
                        ಯದಿ (compare == trivial_compare_func ? e->ಕೀಲಿ == ಕೀಲಿ : compare(e->ಕೀಲಿ, ಕೀಲಿ) == 0)
                                ಪ್ರತ್ಯಾಗಮ idx;
                }

                idx = next_idx(h, idx);
        }
}
#define bucket_scan(h, idx, ಕೀಲಿ) base_bucket_scan(HASHMAP_BASE(h), idx, ಕೀಲಿ)

ಪೂರ್ಣಾಙ್ಕ hashmap_put(Hashmap *h, ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ, ಶೂನ್ಯ *ಮೌಲ್ಯ) {
        ಸಂರಚನಾ swap_entries swap;
        ಸಂರಚನಾ plain_hashmap_entry *e;
        unsigned hash, idx;

        ಪ್ರತಿಜ್ಞಾ(h);

        hash = bucket_hash(h, ಕೀಲಿ);
        idx = bucket_scan(h, hash, ಕೀಲಿ);
        ಯದಿ (idx != IDX_NIL) {
                e = plain_bucket_at(h, idx);
                ಯದಿ (e->ಮೌಲ್ಯ == ಮೌಲ್ಯ)
                        ಪ್ರತ್ಯಾಗಮ 0;
                ಪ್ರತ್ಯಾಗಮ -EEXIST;
        }

        e = &bucket_at_swap(&swap, IDX_PUT)->p;
        e->b.ಕೀಲಿ = ಕೀಲಿ;
        e->ಮೌಲ್ಯ = ಮೌಲ್ಯ;
        ಪ್ರತ್ಯಾಗಮ hashmap_put_boldly(h, hash, &swap, true);
}

ಪೂರ್ಣಾಙ್ಕ set_put(Set *s, ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ) {
        ಸಂರಚನಾ swap_entries swap;
        ಸಂರಚನಾ hashmap_base_entry *e;
        unsigned hash, idx;

        ಪ್ರತಿಜ್ಞಾ(s);

        hash = bucket_hash(s, ಕೀಲಿ);
        idx = bucket_scan(s, hash, ಕೀಲಿ);
        ಯದಿ (idx != IDX_NIL)
                ಪ್ರತ್ಯಾಗಮ 0;

        e = &bucket_at_swap(&swap, IDX_PUT)->p.b;
        e->ಕೀಲಿ = ಕೀಲಿ;
        ಪ್ರತ್ಯಾಗಮ hashmap_put_boldly(s, hash, &swap, true);
}

ಪೂರ್ಣಾಙ್ಕ set_ensure_put(Set **s, ಧ್ರುವ ಸಂರಚನಾ hash_ops *hash_ops, ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(s);

        r = set_ensure_allocated(s, hash_ops);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ set_put(*s, ಕೀಲಿ);
}

ಪೂರ್ಣಾಙ್ಕ set_ensure_consume(Set **s, ಧ್ರುವ ಸಂರಚನಾ hash_ops *hash_ops, ಶೂನ್ಯ *ಕೀಲಿ) {
        ಪೂರ್ಣಾಙ್ಕ r;

        r = set_ensure_put(s, hash_ops, ಕೀಲಿ);
        ಯದಿ (r <= 0) {
                ಯದಿ (hash_ops && hash_ops->free_ಕೀಲಿ)
                        hash_ops->free_ಕೀಲಿ(ಕೀಲಿ);
                ಅನ್ಯಥಾ ಯದಿ (hash_ops && hash_ops->free_ಮೌಲ್ಯ)
                        /* Sets store their element in the key slot but may carry a value destructor. */
                        hash_ops->free_ಮೌಲ್ಯ(ಕೀಲಿ);
                ಅನ್ಯಥಾ
                        ಮುಕ್ತ(ಕೀಲಿ);
        }

        ಪ್ರತ್ಯಾಗಮ r;
}

ಪೂರ್ಣಾಙ್ಕ hashmap_replace(Hashmap *h, ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ, ಶೂನ್ಯ *ಮೌಲ್ಯ) {
        ಸಂರಚನಾ swap_entries swap;
        ಸಂರಚನಾ plain_hashmap_entry *e;
        unsigned hash, idx;

        ಪ್ರತಿಜ್ಞಾ(h);

        hash = bucket_hash(h, ಕೀಲಿ);
        idx = bucket_scan(h, hash, ಕೀಲಿ);
        ಯದಿ (idx != IDX_NIL) {
                e = plain_bucket_at(h, idx);
#ಯದಿ ENABLE_DEBUG_HASHMAP
                /* Although the key is equal, the key pointer may have changed,
                 * and this would break our assumption for iterating. So count
                 * this operation as incompatible with iteration. */
                ಯದಿ (e->b.ಕೀಲಿ != ಕೀಲಿ) {
                        h->b.debug.put_ಎಣಿಕೆ++;
                        h->b.debug.rem_ಎಣಿಕೆ++;
                        h->b.debug.last_rem_idx = idx;
                }
#endif
                e->b.ಕೀಲಿ = ಕೀಲಿ;
                e->ಮೌಲ್ಯ = ಮೌಲ್ಯ;
                hashmap_set_dirty(h);

                ಪ್ರತ್ಯಾಗಮ 0;
        }

        e = &bucket_at_swap(&swap, IDX_PUT)->p;
        e->b.ಕೀಲಿ = ಕೀಲಿ;
        e->ಮೌಲ್ಯ = ಮೌಲ್ಯ;
        ಪ್ರತ್ಯಾಗಮ hashmap_put_boldly(h, hash, &swap, true);
}

ಪೂರ್ಣಾಙ್ಕ hashmap_update(Hashmap *h, ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ, ಶೂನ್ಯ *ಮೌಲ್ಯ) {
        ಸಂರಚನಾ plain_hashmap_entry *e;
        unsigned hash, idx;

        ಪ್ರತಿಜ್ಞಾ(h);

        hash = bucket_hash(h, ಕೀಲಿ);
        idx = bucket_scan(h, hash, ಕೀಲಿ);
        ಯದಿ (idx == IDX_NIL)
                ಪ್ರತ್ಯಾಗಮ -ENOENT;

        e = plain_bucket_at(h, idx);
        e->ಮೌಲ್ಯ = ಮೌಲ್ಯ;
        hashmap_set_dirty(h);

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಶೂನ್ಯ* _hashmap_get(HashmapBase *h, ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ) {
        ಸಂರಚನಾ hashmap_base_entry *e;
        unsigned hash, idx;

        ಯದಿ (!h)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        hash = bucket_hash(h, ಕೀಲಿ);
        idx = bucket_scan(h, hash, ಕೀಲಿ);
        ಯದಿ (idx == IDX_NIL)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        e = bucket_at(h, idx);
        ಪ್ರತ್ಯಾಗಮ entry_ಮೌಲ್ಯ(h, e);
}

ಶೂನ್ಯ* hashmap_get2(Hashmap *h, ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ, ಶೂನ್ಯ **ret) {
        ಸಂರಚನಾ plain_hashmap_entry *e;
        unsigned hash, idx;

        ಯದಿ (!h)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        hash = bucket_hash(h, ಕೀಲಿ);
        idx = bucket_scan(h, hash, ಕೀಲಿ);
        ಯದಿ (idx == IDX_NIL)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        e = plain_bucket_at(h, idx);
        ಯದಿ (ret)
                *ret = (ಶೂನ್ಯ*) e->b.ಕೀಲಿ;

        ಪ್ರತ್ಯಾಗಮ e->ಮೌಲ್ಯ;
}

bool _hashmap_contains(HashmapBase *h, ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ) {
        unsigned hash;

        ಯದಿ (!h)
                ಪ್ರತ್ಯಾಗಮ false;

        hash = bucket_hash(h, ಕೀಲಿ);
        ಪ್ರತ್ಯಾಗಮ bucket_scan(h, hash, ಕೀಲಿ) != IDX_NIL;
}

ಶೂನ್ಯ* _hashmap_remove(HashmapBase *h, ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ) {
        ಸಂರಚನಾ hashmap_base_entry *e;
        unsigned hash, idx;
        ಶೂನ್ಯ *data;

        ಯದಿ (!h)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        hash = bucket_hash(h, ಕೀಲಿ);
        idx = bucket_scan(h, hash, ಕೀಲಿ);
        ಯದಿ (idx == IDX_NIL)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        e = bucket_at(h, idx);
        data = entry_ಮೌಲ್ಯ(h, e);
        remove_entry(h, idx);

        ಪ್ರತ್ಯಾಗಮ data;
}

ಶೂನ್ಯ* hashmap_remove2(Hashmap *h, ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ, ಶೂನ್ಯ **ret) {
        ಸಂರಚನಾ plain_hashmap_entry *e;
        unsigned hash, idx;
        ಶೂನ್ಯ *data;

        ಯದಿ (!h) {
                ಯದಿ (ret)
                        *ret = ರಿಕ್ತ;
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
        }

        hash = bucket_hash(h, ಕೀಲಿ);
        idx = bucket_scan(h, hash, ಕೀಲಿ);
        ಯದಿ (idx == IDX_NIL) {
                ಯದಿ (ret)
                        *ret = ರಿಕ್ತ;
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
        }

        e = plain_bucket_at(h, idx);
        data = e->ಮೌಲ್ಯ;
        ಯದಿ (ret)
                *ret = (ಶೂನ್ಯ*) e->b.ಕೀಲಿ;

        remove_entry(h, idx);

        ಪ್ರತ್ಯಾಗಮ data;
}

ಪೂರ್ಣಾಙ್ಕ hashmap_remove_and_put(Hashmap *h, ಧ್ರುವ ಶೂನ್ಯ *old_ಕೀಲಿ, ಧ್ರುವ ಶೂನ್ಯ *new_ಕೀಲಿ, ಶೂನ್ಯ *ಮೌಲ್ಯ) {
        ಸಂರಚನಾ swap_entries swap;
        ಸಂರಚನಾ plain_hashmap_entry *e;
        unsigned old_hash, new_hash, idx;

        ಯದಿ (!h)
                ಪ್ರತ್ಯಾಗಮ -ENOENT;

        old_hash = bucket_hash(h, old_ಕೀಲಿ);
        idx = bucket_scan(h, old_hash, old_ಕೀಲಿ);
        ಯದಿ (idx == IDX_NIL)
                ಪ್ರತ್ಯಾಗಮ -ENOENT;

        new_hash = bucket_hash(h, new_ಕೀಲಿ);
        ಯದಿ (bucket_scan(h, new_hash, new_ಕೀಲಿ) != IDX_NIL)
                ಪ್ರತ್ಯಾಗಮ -EEXIST;

        remove_entry(h, idx);

        e = &bucket_at_swap(&swap, IDX_PUT)->p;
        e->b.ಕೀಲಿ = new_ಕೀಲಿ;
        e->ಮೌಲ್ಯ = ಮೌಲ್ಯ;
        assert_se(hashmap_put_boldly(h, new_hash, &swap, false) == 1);

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ set_remove_and_put(Set *s, ಧ್ರುವ ಶೂನ್ಯ *old_ಕೀಲಿ, ಧ್ರುವ ಶೂನ್ಯ *new_ಕೀಲಿ) {
        ಸಂರಚನಾ swap_entries swap;
        ಸಂರಚನಾ hashmap_base_entry *e;
        unsigned old_hash, new_hash, idx;

        ಯದಿ (!s)
                ಪ್ರತ್ಯಾಗಮ -ENOENT;

        old_hash = bucket_hash(s, old_ಕೀಲಿ);
        idx = bucket_scan(s, old_hash, old_ಕೀಲಿ);
        ಯದಿ (idx == IDX_NIL)
                ಪ್ರತ್ಯಾಗಮ -ENOENT;

        new_hash = bucket_hash(s, new_ಕೀಲಿ);
        ಯದಿ (bucket_scan(s, new_hash, new_ಕೀಲಿ) != IDX_NIL)
                ಪ್ರತ್ಯಾಗಮ -EEXIST;

        remove_entry(s, idx);

        e = &bucket_at_swap(&swap, IDX_PUT)->p.b;
        e->ಕೀಲಿ = new_ಕೀಲಿ;
        assert_se(hashmap_put_boldly(s, new_hash, &swap, false) == 1);

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ hashmap_remove_and_replace(Hashmap *h, ಧ್ರುವ ಶೂನ್ಯ *old_ಕೀಲಿ, ಧ್ರುವ ಶೂನ್ಯ *new_ಕೀಲಿ, ಶೂನ್ಯ *ಮೌಲ್ಯ) {
        ಸಂರಚನಾ swap_entries swap;
        ಸಂರಚನಾ plain_hashmap_entry *e;
        unsigned old_hash, new_hash, idx_old, idx_new;

        ಯದಿ (!h)
                ಪ್ರತ್ಯಾಗಮ -ENOENT;

        old_hash = bucket_hash(h, old_ಕೀಲಿ);
        idx_old = bucket_scan(h, old_hash, old_ಕೀಲಿ);
        ಯದಿ (idx_old == IDX_NIL)
                ಪ್ರತ್ಯಾಗಮ -ENOENT;

        old_ಕೀಲಿ = bucket_at(HASHMAP_BASE(h), idx_old)->ಕೀಲಿ;

        new_hash = bucket_hash(h, new_ಕೀಲಿ);
        idx_new = bucket_scan(h, new_hash, new_ಕೀಲಿ);
        ಯದಿ (idx_new != IDX_NIL)
                ಯದಿ (idx_old != idx_new) {
                        remove_entry(h, idx_new);
                        /* Compensate for a possible backward shift. */
                        ಯದಿ (old_ಕೀಲಿ != bucket_at(HASHMAP_BASE(h), idx_old)->ಕೀಲಿ)
                                idx_old = prev_idx(HASHMAP_BASE(h), idx_old);
                        ಪ್ರತಿಜ್ಞಾ(old_ಕೀಲಿ == bucket_at(HASHMAP_BASE(h), idx_old)->ಕೀಲಿ);
                }

        remove_entry(h, idx_old);

        e = &bucket_at_swap(&swap, IDX_PUT)->p;
        e->b.ಕೀಲಿ = new_ಕೀಲಿ;
        e->ಮೌಲ್ಯ = ಮೌಲ್ಯ;
        assert_se(hashmap_put_boldly(h, new_hash, &swap, false) == 1);

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಶೂನ್ಯ* hashmap_remove_ಮೌಲ್ಯ(HashmapBase *h, ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ, ಶೂನ್ಯ *ಮೌಲ್ಯ) {
        ಸಂರಚನಾ hashmap_base_entry *e;
        unsigned hash, idx;

        ಯದಿ (!h)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        hash = bucket_hash(h, ಕೀಲಿ);
        idx = bucket_scan(h, hash, ಕೀಲಿ);
        ಯದಿ (idx == IDX_NIL)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        e = bucket_at(h, idx);
        ಯದಿ (entry_ಮೌಲ್ಯ(h, e) != ಮೌಲ್ಯ)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        remove_entry(h, idx);

        ಪ್ರತ್ಯಾಗಮ ಮೌಲ್ಯ;
}

ಸ್ಥಿರ unsigned find_first_entry(HashmapBase *h) {
        Iterator i = ITERATOR_FIRST;

        ಯದಿ (!h || !n_entries(h))
                ಪ್ರತ್ಯಾಗಮ IDX_NIL;

        ಪ್ರತ್ಯಾಗಮ hashmap_iterate_entry(h, &i);
}

ಶೂನ್ಯ* hashmap_first_ಕೀಲಿ_and_ಮೌಲ್ಯ(HashmapBase *h, bool remove, ಶೂನ್ಯ **ret_ಕೀಲಿ) {
        ಸಂರಚನಾ hashmap_base_entry *e;
        ಶೂನ್ಯ *ಕೀಲಿ, *data;
        unsigned idx;

        idx = find_first_entry(h);
        ಯದಿ (idx == IDX_NIL) {
                ಯದಿ (ret_ಕೀಲಿ)
                        *ret_ಕೀಲಿ = ರಿಕ್ತ;
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
        }

        e = bucket_at(h, idx);
        ಕೀಲಿ = (ಶೂನ್ಯ*) e->ಕೀಲಿ;
        data = entry_ಮೌಲ್ಯ(h, e);

        ಯದಿ (remove)
                remove_entry(h, idx);

        ಯದಿ (ret_ಕೀಲಿ)
                *ret_ಕೀಲಿ = ಕೀಲಿ;

        ಪ್ರತ್ಯಾಗಮ data;
}

unsigned _hashmap_size(HashmapBase *h) {
        ಯದಿ (!h)
                ಪ್ರತ್ಯಾಗಮ 0;

        ಪ್ರತ್ಯಾಗಮ n_entries(h);
}

unsigned _hashmap_buckets(HashmapBase *h) {
        ಯದಿ (!h)
                ಪ್ರತ್ಯಾಗಮ 0;

        ಪ್ರತ್ಯಾಗಮ n_buckets(h);
}

ಪೂರ್ಣಾಙ್ಕ _hashmap_merge(Hashmap *h, Hashmap *other) {
        Iterator i;
        unsigned idx;

        ಪ್ರತಿಜ್ಞಾ(h);

        HASHMAP_FOREACH_IDX(idx, HASHMAP_BASE(other), i) {
                ಸಂರಚನಾ plain_hashmap_entry *pe = plain_bucket_at(other, idx);
                ಪೂರ್ಣಾಙ್ಕ r;

                r = hashmap_put(h, pe->b.ಕೀಲಿ, pe->ಮೌಲ್ಯ);
                ಯದಿ (r < 0 && r != -EEXIST)
                        ಪ್ರತ್ಯಾಗಮ r;
        }

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ set_merge(Set *s, Set *other) {
        Iterator i;
        unsigned idx;

        ಪ್ರತಿಜ್ಞಾ(s);

        HASHMAP_FOREACH_IDX(idx, HASHMAP_BASE(other), i) {
                ಸಂರಚನಾ set_entry *se = set_bucket_at(other, idx);
                ಪೂರ್ಣಾಙ್ಕ r;

                r = set_put(s, se->b.ಕೀಲಿ);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;
        }

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ _hashmap_reserve(HashmapBase *h, unsigned entries_add) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(h);

        r = resize_buckets(h, entries_add);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ 0;
}

/*
 * The same as hashmap_merge(), but every new item from other is moved to h.
 * Keys already in h are skipped and stay in other.
 * Returns: 0 on success.
 *          -ENOMEM on alloc failure, in which case no move has been done.
 */
ಪೂರ್ಣಾಙ್ಕ _hashmap_move(HashmapBase *h, HashmapBase *other) {
        ಸಂರಚನಾ swap_entries swap;
        ಸಂರಚನಾ hashmap_base_entry *e, *n;
        Iterator i;
        unsigned idx;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(h);

        ಯದಿ (!other)
                ಪ್ರತ್ಯಾಗಮ 0;

        ಪ್ರತಿಜ್ಞಾ(other->type == h->type);

        /*
         * This reserves buckets for the worst case, where none of other's
         * entries are yet present in h. This is preferable to risking
         * an allocation failure in the middle of the moving and having to
         * rollback or return a partial result.
         */
        r = resize_buckets(h, n_entries(other));
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        HASHMAP_FOREACH_IDX(idx, other, i) {
                unsigned h_hash;

                e = bucket_at(other, idx);
                h_hash = bucket_hash(h, e->ಕೀಲಿ);
                ಯದಿ (bucket_scan(h, h_hash, e->ಕೀಲಿ) != IDX_NIL)
                        ಅನುವರ್ತ;

                n = &bucket_at_swap(&swap, IDX_PUT)->p.b;
                n->ಕೀಲಿ = e->ಕೀಲಿ;
                ಯದಿ (h->type != HASHMAP_TYPE_SET)
                        ((ಸಂರಚನಾ plain_hashmap_entry*) n)->ಮೌಲ್ಯ =
                                ((ಸಂರಚನಾ plain_hashmap_entry*) e)->ಮೌಲ್ಯ;
                assert_se(hashmap_put_boldly(h, h_hash, &swap, false) == 1);

                remove_entry(other, idx);
        }

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ _hashmap_move_one(HashmapBase *h, HashmapBase *other, ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ) {
        ಸಂರಚನಾ swap_entries swap;
        unsigned h_hash, other_hash, idx;
        ಸಂರಚನಾ hashmap_base_entry *e, *n;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(h);

        h_hash = bucket_hash(h, ಕೀಲಿ);
        ಯದಿ (bucket_scan(h, h_hash, ಕೀಲಿ) != IDX_NIL)
                ಪ್ರತ್ಯಾಗಮ -EEXIST;

        ಯದಿ (!other)
                ಪ್ರತ್ಯಾಗಮ -ENOENT;

        ಪ್ರತಿಜ್ಞಾ(other->type == h->type);

        other_hash = bucket_hash(other, ಕೀಲಿ);
        idx = bucket_scan(other, other_hash, ಕೀಲಿ);
        ಯದಿ (idx == IDX_NIL)
                ಪ್ರತ್ಯಾಗಮ -ENOENT;

        e = bucket_at(other, idx);

        n = &bucket_at_swap(&swap, IDX_PUT)->p.b;
        n->ಕೀಲಿ = e->ಕೀಲಿ;
        ಯದಿ (h->type != HASHMAP_TYPE_SET)
                ((ಸಂರಚನಾ plain_hashmap_entry*) n)->ಮೌಲ್ಯ =
                        ((ಸಂರಚನಾ plain_hashmap_entry*) e)->ಮೌಲ್ಯ;
        r = hashmap_put_boldly(h, h_hash, &swap, true);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        remove_entry(other, idx);
        ಪ್ರತ್ಯಾಗಮ 0;
}

HashmapBase* _hashmap_copy(HashmapBase *h) {
        HashmapBase *copy;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(h);

        copy = hashmap_base_new(h->hash_ops, h->type);
        ಯದಿ (!copy)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        ವಿಕಲ್ಪ (h->type) {
        ಪಕ್ಷ HASHMAP_TYPE_PLAIN:
        ಪಕ್ಷ HASHMAP_TYPE_ORDERED:
                r = hashmap_merge((Hashmap*)copy, (Hashmap*)h);
                ಭಙ್ಗ;
        ಪಕ್ಷ HASHMAP_TYPE_SET:
                r = set_merge((Set*)copy, (Set*)h);
                ಭಙ್ಗ;
        ಸ್ವಭಾವ:
                assert_not_reached();
        }

        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ _hashmap_free(copy);

        ಪ್ರತ್ಯಾಗಮ copy;
}

ವರ್ಣ** _hashmap_get_strv(HashmapBase *h) {
        ವರ್ಣ **sv;
        Iterator i;
        unsigned idx, n;

        ಯದಿ (!h)
                ಪ್ರತ್ಯಾಗಮ new0(ವರ್ಣ*, 1);

        sv = new(ವರ್ಣ*, n_entries(h)+1);
        ಯದಿ (!sv)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        n = 0;
        HASHMAP_FOREACH_IDX(idx, h, i)
                sv[n++] = entry_ಮೌಲ್ಯ(h, bucket_at(h, idx));
        sv[n] = ರಿಕ್ತ;

        ಪ್ರತ್ಯಾಗಮ sv;
}

ವರ್ಣ** set_to_strv(Set **s) {
        ಪ್ರತಿಜ್ಞಾ(s);

        /* This is similar to set_get_strv(), but invalidates the set on success. */

        ವರ್ಣ **v = new(ವರ್ಣ*, set_size(*s) + 1);
        ಯದಿ (!v)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        ಪ್ರತಿ (ವರ್ಣ **p = v; (*p = set_steal_first(*s)); p++)
                ;

        ಪ್ರತಿಜ್ಞಾ(set_isempty(*s));
        *s = set_free(*s);
        ಪ್ರತ್ಯಾಗಮ v;
}

ಶೂನ್ಯ* ordered_hashmap_next(OrderedHashmap *h, ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ) {
        ಸಂರಚನಾ ordered_hashmap_entry *e;
        unsigned hash, idx;

        ಯದಿ (!h)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        hash = bucket_hash(h, ಕೀಲಿ);
        idx = bucket_scan(h, hash, ಕೀಲಿ);
        ಯದಿ (idx == IDX_NIL)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        e = ordered_bucket_at(h, idx);
        ಯದಿ (e->iterate_next == IDX_NIL)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
        ಪ್ರತ್ಯಾಗಮ ordered_bucket_at(h, e->iterate_next)->p.ಮೌಲ್ಯ;
}

ಪೂರ್ಣಾಙ್ಕ set_consume(Set *s, ಶೂನ್ಯ *ಮೌಲ್ಯ) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(s);
        ಪ್ರತಿಜ್ಞಾ(ಮೌಲ್ಯ);

        r = set_put(s, ಮೌಲ್ಯ);
        ಯದಿ (r <= 0)
                ಮುಕ್ತ(ಮೌಲ್ಯ);

        ಪ್ರತ್ಯಾಗಮ r;
}

ಪೂರ್ಣಾಙ್ಕ hashmap_put_strdup_full(Hashmap **h, ಧ್ರುವ ಸಂರಚನಾ hash_ops *hash_ops, ಧ್ರುವ ವರ್ಣ *k, ಧ್ರುವ ವರ್ಣ *v) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(h);

        r = hashmap_ensure_allocated(h, hash_ops);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        _cleanup_free_ ವರ್ಣ *kdup = ರಿಕ್ತ, *vdup = ರಿಕ್ತ;

        kdup = strdup(k);
        ಯದಿ (!kdup)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        ಯದಿ (v) {
                vdup = strdup(v);
                ಯದಿ (!vdup)
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;
        }

        r = hashmap_put(*h, kdup, vdup);
        ಯದಿ (r < 0) {
                ಯದಿ (r == -EEXIST && streq_ptr(v, hashmap_get(*h, kdup)))
                        ಪ್ರತ್ಯಾಗಮ 0;
                ಪ್ರತ್ಯಾಗಮ r;
        }

        /* 0 with non-null vdup would mean vdup is already in the hashmap, which cannot be */
        ಪ್ರತಿಜ್ಞಾ(vdup == ರಿಕ್ತ || r > 0);
        ಯದಿ (r > 0)
                kdup = vdup = ರಿಕ್ತ;

        ಪ್ರತ್ಯಾಗಮ r;
}

ಪೂರ್ಣಾಙ್ಕ set_put_strndup_full(Set **s, ಧ್ರುವ ಸಂರಚನಾ hash_ops *hash_ops, ಧ್ರುವ ವರ್ಣ *p, size_t n) {
        ವರ್ಣ *c;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(s);
        ಪ್ರತಿಜ್ಞಾ(p);

        r = set_ensure_allocated(s, hash_ops);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (n == SIZE_MAX) {
                ಯದಿ (set_contains(*s, (ವರ್ಣ*) p))
                        ಪ್ರತ್ಯಾಗಮ 0;

                c = strdup(p);
        } ಅನ್ಯಥಾ
                c = strndup(p, n);
        ಯದಿ (!c)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        ಪ್ರತ್ಯಾಗಮ set_consume(*s, c);
}

ಪೂರ್ಣಾಙ್ಕ set_put_strdupv_full(Set **s, ಧ್ರುವ ಸಂರಚನಾ hash_ops *hash_ops, ವರ್ಣ **l) {
        ಪೂರ್ಣಾಙ್ಕ n = 0, r;

        ಪ್ರತಿಜ್ಞಾ(s);

        STRV_FOREACH(i, l) {
                r = set_put_strndup_full(s, hash_ops, *i, SIZE_MAX);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                n += r;
        }

        ಪ್ರತ್ಯಾಗಮ n;
}

ಪೂರ್ಣಾಙ್ಕ set_put_strsplit(Set *s, ಧ್ರುವ ವರ್ಣ *v, ಧ್ರುವ ವರ್ಣ *separators, ExtractFlags flags) {
        ಧ್ರುವ ವರ್ಣ *p = ASSERT_PTR(v);
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(s);

        ಪ್ರತಿ (;;) {
                ವರ್ಣ *word;

                r = extract_first_word(&p, &word, separators, flags);
                ಯದಿ (r <= 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                r = set_consume(s, word);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;
        }
}

/* expand the cachemem if needed, return true if newly (re)activated. */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ cachemem_maintain(CacheMem *mem, size_t size) {
        ಪ್ರತಿಜ್ಞಾ(mem);

        ಯದಿ (!GREEDY_REALLOC(mem->ptr, size)) {
                ಯದಿ (size > 0)
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;
        }

        ಯದಿ (!mem->active) {
                mem->active = true;
                ಪ್ರತ್ಯಾಗಮ true;
        }

        ಪ್ರತ್ಯಾಗಮ false;
}

ಪೂರ್ಣಾಙ್ಕ iterated_cache_get(IteratedCache *cache, ಧ್ರುವ ಶೂನ್ಯ ***res_ಕೀಲಿಗಳು, ಧ್ರುವ ಶೂನ್ಯ ***res_ಮೌಲ್ಯಗಳು, unsigned *res_n_entries) {
        bool sync_ಕೀಲಿಗಳು = false, sync_ಮೌಲ್ಯಗಳು = false;
        size_t size;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(cache);
        ಪ್ರತಿಜ್ಞಾ(cache->hashmap);

        size = n_entries(cache->hashmap);

        ಯದಿ (res_ಕೀಲಿಗಳು) {
                r = cachemem_maintain(&cache->ಕೀಲಿಗಳು, size);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                sync_ಕೀಲಿಗಳು = r;
        } ಅನ್ಯಥಾ
                cache->ಕೀಲಿಗಳು.active = false;

        ಯದಿ (res_ಮೌಲ್ಯಗಳು) {
                r = cachemem_maintain(&cache->values, size);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                sync_ಮೌಲ್ಯಗಳು = r;
        } ಅನ್ಯಥಾ
                cache->values.active = false;

        ಯದಿ (cache->hashmap->dirty) {
                ಯದಿ (cache->ಕೀಲಿಗಳು.active)
                        sync_ಕೀಲಿಗಳು = true;
                ಯದಿ (cache->values.active)
                        sync_ಮೌಲ್ಯಗಳು = true;

                cache->hashmap->dirty = false;
        }

        ಯದಿ (sync_ಕೀಲಿಗಳು || sync_ಮೌಲ್ಯಗಳು) {
                unsigned i, idx;
                Iterator iter;

                i = 0;
                HASHMAP_FOREACH_IDX(idx, cache->hashmap, iter) {
                        ಸಂರಚನಾ hashmap_base_entry *e;

                        e = bucket_at(cache->hashmap, idx);

                        ಯದಿ (sync_ಕೀಲಿಗಳು)
                                cache->ಕೀಲಿಗಳು.ptr[i] = e->ಕೀಲಿ;
                        ಯದಿ (sync_ಮೌಲ್ಯಗಳು)
                                cache->values.ptr[i] = entry_ಮೌಲ್ಯ(cache->hashmap, e);
                        i++;
                }
        }

        ಯದಿ (res_ಕೀಲಿಗಳು)
                *res_ಕೀಲಿಗಳು = cache->ಕೀಲಿಗಳು.ptr;
        ಯದಿ (res_ಮೌಲ್ಯಗಳು)
                *res_ಮೌಲ್ಯಗಳು = cache->values.ptr;
        ಯದಿ (res_n_entries)
                *res_n_entries = size;

        ಪ್ರತ್ಯಾಗಮ 0;
}

IteratedCache* iterated_cache_free(IteratedCache *cache) {
        ಯದಿ (cache) {
                ಮುಕ್ತ(cache->ಕೀಲಿಗಳು.ptr);
                ಮುಕ್ತ(cache->values.ptr);
        }

        ಪ್ರತ್ಯಾಗಮ mfree(cache);
}

ಪೂರ್ಣಾಙ್ಕ set_strjoin(Set *s, ಧ್ರುವ ವರ್ಣ *separator, bool wrap_with_separator, ವರ್ಣ **ret) {
        _cleanup_free_ ವರ್ಣ *str = ರಿಕ್ತ;
        size_t separator_len, len = 0;
        ಧ್ರುವ ವರ್ಣ *ಮೌಲ್ಯ;
        bool first;

        ಪ್ರತಿಜ್ಞಾ(ret);

        ಯದಿ (set_isempty(s)) {
                *ret = ರಿಕ್ತ;
                ಪ್ರತ್ಯಾಗಮ 0;
        }

        separator_len = strlen_ptr(separator);

        ಯದಿ (separator_len == 0)
                wrap_with_separator = false;

        first = !wrap_with_separator;

        SET_FOREACH(ಮೌಲ್ಯ, s) {
                size_t l = strlen_ptr(ಮೌಲ್ಯ);

                ಯದಿ (l == 0)
                        ಅನುವರ್ತ;

                ಯದಿ (!GREEDY_REALLOC(str, len + l + (first ? 0 : separator_len) + (wrap_with_separator ? separator_len : 0) + 1))
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                ಯದಿ (separator_len > 0 && !first) {
                        memcpy(str + len, separator, separator_len);
                        len += separator_len;
                }

                memcpy(str + len, ಮೌಲ್ಯ, l);
                len += l;
                first = false;
        }

        ಯದಿ (wrap_with_separator) {
                memcpy(str + len, separator, separator_len);
                len += separator_len;
        }

        str[len] = '\0';

        *ret = TAKE_PTR(str);
        ಪ್ರತ್ಯಾಗಮ 0;
}

bool set_equal(Set *a, Set *b) {
        ಶೂನ್ಯ *p;

        /* Checks whether each entry of 'a' is also in 'b' and vice versa, i.e. the two sets contain the same
         * entries */

        ಯದಿ (a == b)
                ಪ್ರತ್ಯಾಗಮ true;

        ಯದಿ (set_isempty(a) && set_isempty(b))
                ಪ್ರತ್ಯಾಗಮ true;

        ಯದಿ (set_size(a) != set_size(b)) /* Cheap check that hopefully catches a lot of inequality cases
                                         * already */
                ಪ್ರತ್ಯಾಗಮ false;

        SET_FOREACH(p, a)
                ಯದಿ (!set_contains(b, p))
                        ಪ್ರತ್ಯಾಗಮ false;

        /* If we have the same hashops, then we don't need to check things backwards given we compared the
         * size and that all of a is in b. */
        ಯದಿ (a->b.hash_ops == b->b.hash_ops)
                ಪ್ರತ್ಯಾಗಮ true;

        SET_FOREACH(p, b)
                ಯದಿ (!set_contains(a, p))
                        ಪ್ರತ್ಯಾಗಮ false;

        ಪ್ರತ್ಯಾಗಮ true;
}

ಸ್ಥಿರ bool set_fnmatch_one(Set *patterns, ಧ್ರುವ ವರ್ಣ *needle) {
        ಧ್ರುವ ವರ್ಣ *p;

        ಪ್ರತಿಜ್ಞಾ(needle);

        /* Any failure of fnmatch() is treated as equivalent to FNM_NOMATCH, i.e. as non-matching pattern */

        SET_FOREACH(p, patterns)
                ಯದಿ (fnmatch(p, needle, 0) == 0)
                        ಪ್ರತ್ಯಾಗಮ true;

        ಪ್ರತ್ಯಾಗಮ false;
}

bool set_fnmatch(Set *include_patterns, Set *exclude_patterns, ಧ್ರುವ ವರ್ಣ *needle) {
        ಪ್ರತಿಜ್ಞಾ(needle);

        ಯದಿ (set_fnmatch_one(exclude_patterns, needle))
                ಪ್ರತ್ಯಾಗಮ false;

        ಯದಿ (set_isempty(include_patterns))
                ಪ್ರತ್ಯಾಗಮ true;

        ಪ್ರತ್ಯಾಗಮ set_fnmatch_one(include_patterns, needle);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ hashmap_entry_compare(
                ಸಂರಚನಾ hashmap_base_entry * ಧ್ರುವ *a,
                ಸಂರಚನಾ hashmap_base_entry * ಧ್ರುವ *b,
                compare_func_t compare) {

        ಪ್ರತಿಜ್ಞಾ(a && *a);
        ಪ್ರತಿಜ್ಞಾ(b && *b);
        ಪ್ರತಿಜ್ಞಾ(compare);

        ಪ್ರತ್ಯಾಗಮ compare((*a)->ಕೀಲಿ, (*b)->ಕೀಲಿ);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ _hashmap_dump_entries_sorted(
                HashmapBase *h,
                ಶೂನ್ಯ ***ret,
                size_t *ret_n) {
        _cleanup_free_ ಶೂನ್ಯ **entries = ರಿಕ್ತ;
        Iterator iter;
        unsigned idx;
        size_t n = 0;

        ಪ್ರತಿಜ್ಞಾ(ret);
        ಪ್ರತಿಜ್ಞಾ(ret_n);

        ಯದಿ (_hashmap_size(h) == 0) {
                *ret = ರಿಕ್ತ;
                *ret_n = 0;
                ಪ್ರತ್ಯಾಗಮ 0;
        }

        /* We append one more element than needed so that the resulting array can be used as a strv. We
         * don't count this entry in the returned size. */
        entries = new(ಶೂನ್ಯ*, _hashmap_size(h) + 1);
        ಯದಿ (!entries)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        HASHMAP_FOREACH_IDX(idx, h, iter)
                entries[n++] = bucket_at(h, idx);

        ಪ್ರತಿಜ್ಞಾ(n == _hashmap_size(h));
        entries[n] = ರಿಕ್ತ;

        typesafe_qsort_r((ಸಂರಚನಾ hashmap_base_entry**) entries, n,
                         hashmap_entry_compare, h->hash_ops->compare);

        *ret = TAKE_PTR(entries);
        *ret_n = n;
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ hashmap_dump_ಕೀಲಿಗಳು_sorted(HashmapBase *h, ಶೂನ್ಯ ***ret, size_t *ret_n) {
        _cleanup_free_ ಶೂನ್ಯ **entries = ರಿಕ್ತ;
        size_t n;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        r = _hashmap_dump_entries_sorted(h, &entries, &n);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        /* Reuse the array. */
        FOREACH_ARRAY(e, entries, n)
                *e = (ಶೂನ್ಯ*) (*(ಸಂರಚನಾ hashmap_base_entry**) e)->ಕೀಲಿ;

        *ret = TAKE_PTR(entries);
        ಯದಿ (ret_n)
                *ret_n = n;
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ _hashmap_dump_sorted(HashmapBase *h, ಶೂನ್ಯ ***ret, size_t *ret_n) {
        _cleanup_free_ ಶೂನ್ಯ **entries = ರಿಕ್ತ;
        size_t n;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(ret);

        r = _hashmap_dump_entries_sorted(h, &entries, &n);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        /* Reuse the array. */
        FOREACH_ARRAY(e, entries, n)
                *e = entry_ಮೌಲ್ಯ(h, *(ಸಂರಚನಾ hashmap_base_entry**) e);

        *ret = TAKE_PTR(entries);
        ಯದಿ (ret_n)
                *ret_n = n;
        ಪ್ರತ್ಯಾಗಮ 0;
}
