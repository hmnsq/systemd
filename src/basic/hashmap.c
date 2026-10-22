/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ <fnmatch.h>
#ଅନ୍ତର୍ଭାଵ <pthread.h>
#ଅନ୍ତର୍ଭାଵ <unistd.h>
#ଯଦି HAVE_VALGRIND_VALGRIND_H
#  include <valgrind/valgrind.h>
#endif

#ଅନ୍ତର୍ଭାଵ "alloc-util.h"
#ଅନ୍ତର୍ଭାଵ "extract-word.h"
#ଅନ୍ତର୍ଭାଵ "hashmap.h"
#ଅନ୍ତର୍ଭାଵ "log.h"
#ଅନ୍ତର୍ଭାଵ "logarithm.h"
#ଅନ୍ତର୍ଭାଵ "memory-util.h"
#ଅନ୍ତର୍ଭାଵ "mempool.h"
#ଅନ୍ତର୍ଭାଵ "process-util.h"
#ଅନ୍ତର୍ଭାଵ "random-util.h"
#ଅନ୍ତର୍ଭାଵ "set.h"
#ଅନ୍ତର୍ଭାଵ "siphash24.h"
#ଅନ୍ତର୍ଭାଵ "sort-util.h"
#ଅନ୍ତର୍ଭାଵ "string-util.h"
#ଅନ୍ତର୍ଭାଵ "strv.h"

#ଯଦି ENABLE_DEBUG_HASHMAP
#ଅନ୍ତର୍ଭାଵ "list.h"
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
ସଂରଚନା hashmap_base_entry {
        ଅଚଳ ଶୂନ୍ୟ *ଚାବି;
};

/* Entry types for specific hashmap/set types
 * hashmap_base_entry must be at the beginning of each entry struct. */

ସଂରଚନା plain_hashmap_entry {
        ସଂରଚନା hashmap_base_entry b;
        ଶୂନ୍ୟ *ମୂଲ୍ୟ;
};

ସଂରଚନା ordered_hashmap_entry {
        ସଂରଚନା plain_hashmap_entry p;
        unsigned iterate_next, iterate_previous;
};

ସଂରଚନା set_entry {
        ସଂରଚନା hashmap_base_entry b;
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
ସଂରଚନା swap_entries {
        ସଂରଚନା ordered_hashmap_entry e[_IDX_SWAP_END - _IDX_SWAP_BEGIN];
};

/* Distance from Initial Bucket */
ପ୍ରକାରବ୍ୟାଖ୍ୟା uint8_t dib_raw_t;
#define DIB_RAW_OVERFLOW ((dib_raw_t)0xfdU)   /* indicates DIB value is greater than representable */
#define DIB_RAW_REHASH   ((dib_raw_t)0xfeU)   /* entry yet to be rehashed during in-place resize */
#define DIB_RAW_FREE     ((dib_raw_t)0xffU)   /* a free bucket */
#define DIB_RAW_INIT     ((ଅକ୍ଷର)DIB_RAW_FREE) /* a byte to memset a DIB store with when initializing */

#define DIB_FREE UINT_MAX

#ଯଦି ENABLE_DEBUG_HASHMAP
ସଂରଚନା hashmap_debug_info {
        LIST_FIELDS(ସଂରଚନା hashmap_debug_info, debug_list);
        unsigned max_entries;  /* high watermark of n_entries */

        /* fields to detect modification while iterating */
        unsigned put_ଗଣନା;    /* counts puts into the hashmap */
        unsigned rem_ଗଣନା;    /* counts removals from hashmap */
        unsigned last_rem_idx; /* remembers last removal index */
};

/* Tracks all existing hashmaps. Get at it from gdb. See sd_dump_hashmaps.py */
ସ୍ଥିର LIST_HEAD(ସଂରଚନା hashmap_debug_info, hashmap_debug_list);
ସ୍ଥିର pthread_mutex_t hashmap_debug_list_mutex = PTHREAD_MUTEX_INITIALIZER;
#endif

ଗଣନା HashmapType {
        HASHMAP_TYPE_PLAIN,
        HASHMAP_TYPE_ORDERED,
        HASHMAP_TYPE_SET,
        _HASHMAP_TYPE_MAX
};

ସଂରଚନା _packed_ indirect_storage {
        ଶୂନ୍ୟ *storage;                     /* where buckets and DIBs are stored */
        uint8_t  hash_ଚାବି[HASH_ଚାବି_SIZE];  /* hash key; changes during resize */

        unsigned n_entries;                /* number of stored entries */
        unsigned n_buckets;                /* number of buckets */

        unsigned idx_lowest_entry;         /* Index below which all buckets are free.
                                              Makes "while (hashmap_steal_first())" loops
                                              O(n) instead of O(n^2) for unordered hashmaps. */
        uint8_t  _pad[3];                  /* padding for the whole HashmapBase */
        /* The bitfields in HashmapBase complete the alignment of the whole thing. */
};

ସଂରଚନା direct_storage {
        /* This gives us 39 bytes on 64-bit, or 35 bytes on 32-bit.
         * That's room for 4 set_entries + 4 DIB bytes + 3 unused bytes on 64-bit,
         *              or 7 set_entries + 7 DIB bytes + 0 unused bytes on 32-bit. */
        uint8_t storage[ଆକାର(ସଂରଚନା indirect_storage)];
};

#define DIRECT_BUCKETS(entry_t) \
        (ଆକାର(ସଂରଚନା direct_storage) / (ଆକାର(entry_t) + ଆକାର(dib_raw_t)))

/* We should be able to store at least one entry directly. */
assert_cc(DIRECT_BUCKETS(ସଂରଚନା ordered_hashmap_entry) >= 1);

/* We have 3 bits for n_direct_entries. */
assert_cc(DIRECT_BUCKETS(ସଂରଚନା set_entry) < (1 << 3));

/* Hashmaps with directly stored entries all use this shared hash key.
 * It's no big deal if the key is guessed, because there can be only
 * a handful of directly stored entries in a hashmap. When a hashmap
 * outgrows direct storage, it gets its own key for indirect storage. */
ସ୍ଥିର uint8_t shared_hash_ଚାବି[HASH_ଚାବି_SIZE];

/* Fields that all hashmap/set types must have */
ସଂରଚନା HashmapBase {
        ଅଚଳ ସଂରଚନା hash_ops *hash_ops;  /* hash and compare ops to use */

        ସଂଘ _packed_ {
                ସଂରଚନା indirect_storage indirect; /* if  has_indirect */
                ସଂରଚନା direct_storage direct;     /* if !has_indirect */
        };

        ଗଣନା HashmapType type:2;     /* HASHMAP_TYPE_* */
        bool has_indirect:1;         /* whether indirect storage is used */
        unsigned n_direct_entries:3; /* Number of entries in direct storage.
                                      * Only valid if !has_indirect. */
        bool from_pool:1;            /* whether was allocated from mempool */
        bool dirty:1;                /* whether dirtied since last iterated_cache_get() */
        bool cached:1;               /* whether this hashmap is being cached */

#ଯଦି ENABLE_DEBUG_HASHMAP
        ସଂରଚନା hashmap_debug_info debug;
#endif
};

/* Specific hash types
 * HashmapBase must be at the beginning of each hashmap struct. */

ସଂରଚନା Hashmap {
        ସଂରଚନା HashmapBase b;
};

ସଂରଚନା OrderedHashmap {
        ସଂରଚନା HashmapBase b;
        unsigned iterate_list_head, iterate_list_tail;
};

ସଂରଚନା Set {
        ସଂରଚନା HashmapBase b;
};

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା CacheMem {
        ଅଚଳ ଶୂନ୍ୟ **ptr;
        size_t n_populated;
        bool active:1;
} CacheMem;

ସଂରଚନା IteratedCache {
        HashmapBase *hashmap;
        CacheMem ଚାବିଗୁଡ଼ିକ, values;
};

DEFINE_MEMPOOL(hashmap_pool,         Hashmap,        8);
DEFINE_MEMPOOL(ordered_hashmap_pool, OrderedHashmap, 8);
/* No need for a separate Set pool */
assert_cc(ଆକାର(Hashmap) == ଆକାର(Set));

ସଂରଚନା hashmap_type_info {
        size_t head_size;
        size_t entry_size;
        ସଂରଚନା mempool *mempool;
        unsigned n_direct_buckets;
};

ସ୍ଥିର _used_ ଅଚଳ ସଂରଚନା hashmap_type_info hashmap_type_info[_HASHMAP_TYPE_MAX] = {
        [HASHMAP_TYPE_PLAIN] = {
                .head_size        = ଆକାର(Hashmap),
                .entry_size       = ଆକାର(ସଂରଚନା plain_hashmap_entry),
                .mempool          = &hashmap_pool,
                .n_direct_buckets = DIRECT_BUCKETS(ସଂରଚନା plain_hashmap_entry),
        },
        [HASHMAP_TYPE_ORDERED] = {
                .head_size        = ଆକାର(OrderedHashmap),
                .entry_size       = ଆକାର(ସଂରଚନା ordered_hashmap_entry),
                .mempool          = &ordered_hashmap_pool,
                .n_direct_buckets = DIRECT_BUCKETS(ସଂରଚନା ordered_hashmap_entry),
        },
        [HASHMAP_TYPE_SET] = {
                .head_size        = ଆକାର(Set),
                .entry_size       = ଆକାର(ସଂରଚନା set_entry),
                .mempool          = &hashmap_pool,
                .n_direct_buckets = DIRECT_BUCKETS(ସଂରଚନା set_entry),
        },
};

ଶୂନ୍ୟ hashmap_trim_pools(ଶୂନ୍ୟ) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        /* The pool is only allocated by the main thread, but the memory can be passed to other
         * threads. Let's clean up if we are the main thread and no other threads are live. */

        /* We build our own is_main_thread() here, which doesn't use C11 TLS based caching of the
         * result. That's because valgrind apparently doesn't like TLS to be used from a GCC destructor. */
        ଯଦି (getpid() != gettid())
                ଫେରନ୍ତୁ (ଶୂନ୍ୟ) log_debug("Not cleaning up memory pools, not in main thread.");

        r = get_process_threads(0);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ (ଶୂନ୍ୟ) log_debug_errno(r, "Failed to determine number of threads, not cleaning up memory pools: %m");
        ଯଦି (r != 1)
                ଫେରନ୍ତୁ (ଶୂନ୍ୟ) log_debug("Not cleaning up memory pools, running in multi-threaded process.");

        mempool_trim(&hashmap_pool);
        mempool_trim(&ordered_hashmap_pool);
}

#ଯଦି HAVE_VALGRIND_VALGRIND_H
_destructor_ ସ୍ଥିର ଶୂନ୍ୟ cleanup_pools(ଶୂନ୍ୟ) {
        /* Be nice to valgrind */
        ଯଦି (RUNNING_ON_VALGRIND)
                hashmap_trim_pools();
}
#endif

ସ୍ଥିର ଧାଡ଼ିଭିତରେ unsigned n_buckets(HashmapBase *h) {
        ପ୍ରତିଜ୍ଞା(h);
        ଫେରନ୍ତୁ h->has_indirect ? h->indirect.n_buckets
                               : hashmap_type_info[h->type].n_direct_buckets;
}

ସ୍ଥିର unsigned n_entries(HashmapBase *h) {
        ପ୍ରତିଜ୍ଞା(h);
        ଫେରନ୍ତୁ h->has_indirect ? h->indirect.n_entries
                               : h->n_direct_entries;
}

ସ୍ଥିର ଶୂନ୍ୟ n_entries_inc(HashmapBase *h) {
        ପ୍ରତିଜ୍ଞା(h);

        ଯଦି (h->has_indirect)
                h->indirect.n_entries++;
        ଅନ୍ୟଥା
                h->n_direct_entries++;
}

ସ୍ଥିର ଶୂନ୍ୟ n_entries_dec(HashmapBase *h) {
        ପ୍ରତିଜ୍ଞା(h);

        ଯଦି (h->has_indirect)
                h->indirect.n_entries--;
        ଅନ୍ୟଥା
                h->n_direct_entries--;
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ* storage_ptr(HashmapBase *h) {
        ପ୍ରତିଜ୍ଞା(h);
        ଫେରନ୍ତୁ h->has_indirect ? h->indirect.storage
                               : h->direct.storage;
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ uint8_t* hash_ଚାବି(HashmapBase *h) {
        ପ୍ରତିଜ୍ଞା(h);
        ଫେରନ୍ତୁ h->has_indirect ? h->indirect.hash_ଚାବି
                               : shared_hash_ଚାବି;
}

ସ୍ଥିର unsigned base_bucket_hash(HashmapBase *h, ଅଚଳ ଶୂନ୍ୟ *p) {
        ସଂରଚନା siphash state;
        uint64_t hash;

        ପ୍ରତିଜ୍ଞା(h);

        /* The keys of trivial_hash_ops and uint64_hash_ops fit in 64 bits. siphash24_uint64() hashes such
         * a key without the indirect call through hash_ops and keeps the SipHash state in registers. On
         * 32-bit and big-endian architectures, the result differs from the result of trivial_hash_func(),
         * which hashes the key in native byte order. Only this function maps keys to buckets, so the
         * difference does not matter. */
        ଯଦି (h->hash_ops->hash == trivial_hash_func)
                hash = siphash24_uint64((uintptr_t) p, hash_ଚାବି(h));
        ଅନ୍ୟଥା ଯଦି (h->hash_ops->hash == (hash_func_t) uint64_hash_func)
                hash = siphash24_uint64(*(ଅଚଳ uint64_t*) p, hash_ଚାବି(h));
        ଅନ୍ୟଥା {
                siphash24_init(&state, hash_ଚାବି(h));
                h->hash_ops->hash(p, &state);
                hash = siphash24_finalize(&state);
        }

        /* n_buckets is not a power of two, so a modulo needs a division. Multiplying the upper 32 bits of
         * the hash with n_buckets and keeping the upper 32 bits of the product maps the hash to
         * [0, n_buckets) almost as evenly, without a division. */
        ଫେରନ୍ତୁ (unsigned) (((hash >> 32) * n_buckets(h)) >> 32);
}
#define bucket_hash(h, p) base_bucket_hash(HASHMAP_BASE(h), p)

ସ୍ଥିର ଶୂନ୍ୟ base_set_dirty(HashmapBase *h) {
        ପ୍ରତିଜ୍ଞା(h);

        h->dirty = true;
}
#define hashmap_set_dirty(h) base_set_dirty(HASHMAP_BASE(h))

ସ୍ଥିର ଶୂନ୍ୟ get_hash_ଚାବି(uint8_t hash_ଚାବି[HASH_ଚାବି_SIZE], bool reuse_is_ok) {
        ସ୍ଥିର uint8_t current[HASH_ଚାବି_SIZE];
        ସ୍ଥିର bool current_initialized = false;

        /* Returns a hash function key to use. In order to keep things
         * fast we will not generate a new key each time we allocate a
         * new hash table. Instead, we'll just reuse the most recently
         * generated one, except if we never generated one or when we
         * are rehashing an entire hash table because we reached a
         * fill level */

        ଯଦି (!current_initialized || !reuse_is_ok) {
                random_bytes(current, ଆକାର(current));
                current_initialized = true;
        }

        memcpy(hash_ଚାବି, current, ଆକାର(current));
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ସଂରଚନା hashmap_base_entry* bucket_at(HashmapBase *h, unsigned idx) {
        ପ୍ରତିଜ୍ଞା(h);
        ଫେରନ୍ତୁ CAST_ALIGN_PTR(
                        ସଂରଚନା hashmap_base_entry,
                        (uint8_t *) storage_ptr(h) + idx * hashmap_type_info[h->type].entry_size);
}

ସ୍ଥିର ସଂରଚନା plain_hashmap_entry* plain_bucket_at(Hashmap *h, unsigned idx) {
        ଫେରନ୍ତୁ (ସଂରଚନା plain_hashmap_entry*) bucket_at(HASHMAP_BASE(h), idx);
}

ସ୍ଥିର ସଂରଚନା ordered_hashmap_entry* ordered_bucket_at(OrderedHashmap *h, unsigned idx) {
        ଫେରନ୍ତୁ (ସଂରଚନା ordered_hashmap_entry*) bucket_at(HASHMAP_BASE(h), idx);
}

ସ୍ଥିର ସଂରଚନା set_entry *set_bucket_at(Set *h, unsigned idx) {
        ଫେରନ୍ତୁ (ସଂରଚନା set_entry*) bucket_at(HASHMAP_BASE(h), idx);
}

ସ୍ଥିର ସଂରଚନା ordered_hashmap_entry* bucket_at_swap(ସଂରଚନା swap_entries *swap, unsigned idx) {
        ପ୍ରତିଜ୍ଞା(swap);
        ଫେରନ୍ତୁ &swap->e[idx - _IDX_SWAP_BEGIN];
}

/* Returns a pointer to the bucket at index idx.
 * Understands real indexes and swap indexes, hence "_virtual". */
ସ୍ଥିର ସଂରଚନା hashmap_base_entry* bucket_at_virtual(HashmapBase *h, ସଂରଚନା swap_entries *swap,
                                                    unsigned idx) {
        ଯଦି (idx < _IDX_SWAP_BEGIN)
                ଫେରନ୍ତୁ bucket_at(h, idx);

        ଯଦି (idx < _IDX_SWAP_END)
                ଫେରନ୍ତୁ &bucket_at_swap(swap, idx)->p.b;

        assert_not_reached();
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ dib_raw_t* dib_raw_ptr(HashmapBase *h) {
        ପ୍ରତିଜ୍ଞା(h);
        ଫେରନ୍ତୁ (dib_raw_t*)
                ((uint8_t*) storage_ptr(h) + hashmap_type_info[h->type].entry_size * n_buckets(h));
}

ସ୍ଥିର unsigned bucket_distance(HashmapBase *h, unsigned idx, unsigned from) {
        ଫେରନ୍ତୁ idx >= from ? idx - from
                           : n_buckets(h) + idx - from;
}

ସ୍ଥିର unsigned bucket_calculate_dib(HashmapBase *h, unsigned idx, dib_raw_t raw_dib) {
        unsigned initial_bucket;

        ଯଦି (raw_dib == DIB_RAW_FREE)
                ଫେରନ୍ତୁ DIB_FREE;

        ଯଦି (_likely_(raw_dib < DIB_RAW_OVERFLOW))
                ଫେରନ୍ତୁ raw_dib;

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
        initial_bucket = bucket_hash(h, bucket_at(h, idx)->ଚାବି);
        ଫେରନ୍ତୁ bucket_distance(h, idx, initial_bucket);
}

ସ୍ଥିର ଶୂନ୍ୟ bucket_set_dib(HashmapBase *h, unsigned idx, unsigned dib) {
        dib_raw_ptr(h)[idx] = dib != DIB_FREE ? MIN(dib, DIB_RAW_OVERFLOW) : DIB_RAW_FREE;
}

ସ୍ଥିର unsigned skip_free_buckets(HashmapBase *h, unsigned idx) {
        dib_raw_t *dibs;

        dibs = dib_raw_ptr(h);

        ପାଇଁ ( ; idx < n_buckets(h); idx++)
                ଯଦି (dibs[idx] != DIB_RAW_FREE)
                        ଫେରନ୍ତୁ idx;

        ଫେରନ୍ତୁ IDX_NIL;
}

ସ୍ଥିର ଶୂନ୍ୟ bucket_mark_free(HashmapBase *h, unsigned idx) {
        ପ୍ରତିଜ୍ଞା(h);

        memzero(bucket_at(h, idx), hashmap_type_info[h->type].entry_size);
        bucket_set_dib(h, idx, DIB_FREE);
}

ସ୍ଥିର ଶୂନ୍ୟ bucket_move_entry(HashmapBase *h, ସଂରଚନା swap_entries *swap,
                              unsigned from, unsigned to) {
        ସଂରଚନା hashmap_base_entry *e_from, *e_to;

        ପ୍ରତିଜ୍ଞା(h);
        ପ୍ରତିଜ୍ଞା(from != to);

        e_from = bucket_at_virtual(h, swap, from);
        e_to   = bucket_at_virtual(h, swap, to);

        memcpy(e_to, e_from, hashmap_type_info[h->type].entry_size);

        ଯଦି (h->type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*) h;
                ସଂରଚନା ordered_hashmap_entry *le, *le_to;

                le_to = (ସଂରଚନା ordered_hashmap_entry*) e_to;

                ଯଦି (le_to->iterate_next != IDX_NIL) {
                        le = (ସଂରଚନା ordered_hashmap_entry*)
                             bucket_at_virtual(h, swap, le_to->iterate_next);
                        le->iterate_previous = to;
                }

                ଯଦି (le_to->iterate_previous != IDX_NIL) {
                        le = (ସଂରଚନା ordered_hashmap_entry*)
                             bucket_at_virtual(h, swap, le_to->iterate_previous);
                        le->iterate_next = to;
                }

                ଯଦି (lh->iterate_list_head == from)
                        lh->iterate_list_head = to;
                ଯଦି (lh->iterate_list_tail == from)
                        lh->iterate_list_tail = to;
        }
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ unsigned next_idx(HashmapBase *h, unsigned idx) {
        ଫେରନ୍ତୁ idx + 1U < n_buckets(h) ? idx + 1U : 0U;
}

ସ୍ଥିର unsigned prev_idx(HashmapBase *h, unsigned idx) {
        ଫେରନ୍ତୁ idx > 0U ? idx - 1U : n_buckets(h) - 1U;
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ* entry_ମୂଲ୍ୟ(HashmapBase *h, ସଂରଚନା hashmap_base_entry *e) {
        ପ୍ରତିଜ୍ଞା(h);
        ପ୍ରତିଜ୍ଞା(e);

        ବାଛ (h->type) {

        ସ୍ଥିତି HASHMAP_TYPE_PLAIN:
        ସ୍ଥିତି HASHMAP_TYPE_ORDERED:
                ଫେରନ୍ତୁ ((ସଂରଚନା plain_hashmap_entry*)e)->ମୂଲ୍ୟ;

        ସ୍ଥିତି HASHMAP_TYPE_SET:
                ଫେରନ୍ତୁ (ଶୂନ୍ୟ*) e->ଚାବି;

        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                assert_not_reached();
        }
}

ସ୍ଥିର ଶୂନ୍ୟ base_remove_entry(HashmapBase *h, unsigned idx) {
        unsigned ବାମ, ଡାହାଣ, prev, dib;
        dib_raw_t raw_dib, *dibs;

        dibs = dib_raw_ptr(h);
        ପ୍ରତିଜ୍ଞା(dibs[idx] != DIB_RAW_FREE);

#ଯଦି ENABLE_DEBUG_HASHMAP
        ପ୍ରତିଜ୍ଞା(h);
        h->debug.rem_ଗଣନା++;
        h->debug.last_rem_idx = idx;
#endif

        ବାମ = idx;
        /* Find the stop bucket ("right"). It is either free or has DIB == 0. */
        ପାଇଁ (ଡାହାଣ = next_idx(h, ବାମ); ; ଡାହାଣ = next_idx(h, ଡାହାଣ)) {
                raw_dib = dibs[ଡାହାଣ];
                ଯଦି (IN_SET(raw_dib, 0, DIB_RAW_FREE))
                        ଭାଙ୍ଗ;

                /* The buckets are not supposed to be all occupied and with DIB > 0.
                 * That would mean we could make everyone better off by shifting them
                 * backward. This scenario is impossible. */
                ପ୍ରତିଜ୍ଞା(ବାମ != ଡାହାଣ);
        }

        ଯଦି (h->type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*) h;
                ସଂରଚନା ordered_hashmap_entry *le = ordered_bucket_at(lh, idx);

                ଯଦି (le->iterate_next != IDX_NIL)
                        ordered_bucket_at(lh, le->iterate_next)->iterate_previous = le->iterate_previous;
                ଅନ୍ୟଥା
                        lh->iterate_list_tail = le->iterate_previous;

                ଯଦି (le->iterate_previous != IDX_NIL)
                        ordered_bucket_at(lh, le->iterate_previous)->iterate_next = le->iterate_next;
                ଅନ୍ୟଥା
                        lh->iterate_list_head = le->iterate_next;
        }

        /* Now shift all buckets in the interval (left, right) one step backwards */
        ପାଇଁ (prev = ବାମ, ବାମ = next_idx(h, ବାମ); ବାମ != ଡାହାଣ;
             prev = ବାମ, ବାମ = next_idx(h, ବାମ)) {
                dib = bucket_calculate_dib(h, ବାମ, dibs[ବାମ]);
                ପ୍ରତିଜ୍ଞା(dib != 0);
                bucket_move_entry(h, ଖାଲି, ବାମ, prev);
                bucket_set_dib(h, prev, dib - 1);
        }

        bucket_mark_free(h, prev);
        n_entries_dec(h);
        base_set_dirty(h);
}
#define remove_entry(h, idx) base_remove_entry(HASHMAP_BASE(h), idx)

ସ୍ଥିର unsigned hashmap_iterate_in_insertion_order(OrderedHashmap *h, Iterator *i) {
        ସଂରଚନା ordered_hashmap_entry *e;
        unsigned idx;

        ପ୍ରତିଜ୍ଞା(h);
        ପ୍ରତିଜ୍ଞା(i);

        ଯଦି (i->idx == IDX_NIL)
                ଯାଅ at_end;

        ଯଦି (i->idx == IDX_FIRST && h->iterate_list_head == IDX_NIL)
                ଯାଅ at_end;

        ଯଦି (i->idx == IDX_FIRST) {
                idx = h->iterate_list_head;
                e = ordered_bucket_at(h, idx);
        } ଅନ୍ୟଥା {
                idx = i->idx;
                e = ordered_bucket_at(h, idx);
                /*
                 * We allow removing the current entry while iterating, but removal may cause
                 * a backward shift. The next entry may thus move one bucket to the left.
                 * To detect when it happens, we remember the key pointer of the entry we were
                 * going to iterate next. If it does not match, there was a backward shift.
                 */
                ଯଦି (e->p.b.ଚାବି != i->next_ଚାବି) {
                        idx = prev_idx(HASHMAP_BASE(h), idx);
                        e = ordered_bucket_at(h, idx);
                }
                ପ୍ରତିଜ୍ଞା(e->p.b.ଚାବି == i->next_ଚାବି);
        }

#ଯଦି ENABLE_DEBUG_HASHMAP
        i->prev_idx = idx;
#endif

        ଯଦି (e->iterate_next != IDX_NIL) {
                ସଂରଚନା ordered_hashmap_entry *n;
                i->idx = e->iterate_next;
                n = ordered_bucket_at(h, i->idx);
                i->next_ଚାବି = n->p.b.ଚାବି;
        } ଅନ୍ୟଥା
                i->idx = IDX_NIL;

        ଫେରନ୍ତୁ idx;

at_end:
        i->idx = IDX_NIL;
        ଫେରନ୍ତୁ IDX_NIL;
}

ସ୍ଥିର unsigned hashmap_iterate_in_internal_order(HashmapBase *h, Iterator *i) {
        unsigned idx;

        ପ୍ରତିଜ୍ଞା(h);
        ପ୍ରତିଜ୍ଞା(i);

        ଯଦି (i->idx == IDX_NIL)
                ଯାଅ at_end;

        ଯଦି (i->idx == IDX_FIRST) {
                /* fast forward to the first occupied bucket */
                ଯଦି (h->has_indirect) {
                        i->idx = skip_free_buckets(h, h->indirect.idx_lowest_entry);
                        h->indirect.idx_lowest_entry = i->idx;
                } ଅନ୍ୟଥା
                        i->idx = skip_free_buckets(h, 0);

                ଯଦି (i->idx == IDX_NIL)
                        ଯାଅ at_end;
        } ଅନ୍ୟଥା {
                ସଂରଚନା hashmap_base_entry *e;

                ପ୍ରତିଜ୍ଞା(i->idx > 0);

                e = bucket_at(h, i->idx);
                /*
                 * We allow removing the current entry while iterating, but removal may cause
                 * a backward shift. The next entry may thus move one bucket to the left.
                 * To detect when it happens, we remember the key pointer of the entry we were
                 * going to iterate next. If it does not match, there was a backward shift.
                 */
                ଯଦି (e->ଚାବି != i->next_ଚାବି)
                        e = bucket_at(h, --i->idx);

                ପ୍ରତିଜ୍ଞା(e->ଚାବି == i->next_ଚାବି);
        }

        idx = i->idx;
#ଯଦି ENABLE_DEBUG_HASHMAP
        i->prev_idx = idx;
#endif

        i->idx = skip_free_buckets(h, i->idx + 1);
        ଯଦି (i->idx != IDX_NIL)
                i->next_ଚାବି = bucket_at(h, i->idx)->ଚାବି;
        ଅନ୍ୟଥା
                i->idx = IDX_NIL;

        ଫେରନ୍ତୁ idx;

at_end:
        i->idx = IDX_NIL;
        ଫେରନ୍ତୁ IDX_NIL;
}

ସ୍ଥିର unsigned hashmap_iterate_entry(HashmapBase *h, Iterator *i) {
        ଯଦି (!h) {
                i->idx = IDX_NIL;
                ଫେରନ୍ତୁ IDX_NIL;
        }

#ଯଦି ENABLE_DEBUG_HASHMAP
        ଯଦି (i->idx == IDX_FIRST) {
                i->put_ଗଣନା = h->debug.put_ଗଣନା;
                i->rem_ଗଣନା = h->debug.rem_ଗଣନା;
        } ଅନ୍ୟଥା {
                /* While iterating, must not add any new entries */
                ପ୍ରତିଜ୍ଞା(i->put_ଗଣନା == h->debug.put_ଗଣନା);
                /* ... or remove entries other than the current one */
                ପ୍ରତିଜ୍ଞା(i->rem_ଗଣନା == h->debug.rem_ଗଣନା ||
                       (i->rem_ଗଣନା == h->debug.rem_ଗଣନା - 1 &&
                        i->prev_idx == h->debug.last_rem_idx));
                /* Reset our removals counter */
                i->rem_ଗଣନା = h->debug.rem_ଗଣନା;
        }
#endif

        ଫେରନ୍ତୁ h->type == HASHMAP_TYPE_ORDERED ? hashmap_iterate_in_insertion_order((OrderedHashmap*) h, i)
                                               : hashmap_iterate_in_internal_order(h, i);
}

bool _hashmap_iterate(HashmapBase *h, Iterator *i, ଶୂନ୍ୟ **ମୂଲ୍ୟ, ଅଚଳ ଶୂନ୍ୟ **ଚାବି) {
        ସଂରଚନା hashmap_base_entry *e;
        ଶୂନ୍ୟ *data;
        unsigned idx;

        idx = hashmap_iterate_entry(h, i);
        ଯଦି (idx == IDX_NIL) {
                ଯଦି (ମୂଲ୍ୟ)
                        *ମୂଲ୍ୟ = ଖାଲି;
                ଯଦି (ଚାବି)
                        *ଚାବି = ଖାଲି;

                ଫେରନ୍ତୁ false;
        }

        e = bucket_at(h, idx);
        data = entry_ମୂଲ୍ୟ(h, e);
        ଯଦି (ମୂଲ୍ୟ)
                *ମୂଲ୍ୟ = data;
        ଯଦି (ଚାବି)
                *ଚାବି = e->ଚାବି;

        ଫେରନ୍ତୁ true;
}

#define HASHMAP_FOREACH_IDX(idx, h, i) \
        ପାଇଁ ((i) = ITERATOR_FIRST, (idx) = hashmap_iterate_entry((h), &(i)); \
             (idx != IDX_NIL); \
             (idx) = hashmap_iterate_entry((h), &(i)))

IteratedCache* _hashmap_iterated_cache_new(HashmapBase *h) {
        IteratedCache *cache;

        ପ୍ରତିଜ୍ଞା(h);
        ପ୍ରତିଜ୍ଞା(!h->cached);

        ଯଦି (h->cached)
                ଫେରନ୍ତୁ ଖାଲି;

        cache = new0(IteratedCache, 1);
        ଯଦି (!cache)
                ଫେରନ୍ତୁ ଖାଲି;

        cache->hashmap = h;
        h->cached = true;

        ଫେରନ୍ତୁ cache;
}

ସ୍ଥିର ଶୂନ୍ୟ reset_direct_storage(HashmapBase *h) {
        ଅଚଳ ସଂରଚନା hashmap_type_info *hi = &hashmap_type_info[h->type];
        ଶୂନ୍ୟ *p;

        ପ୍ରତିଜ୍ଞା(!h->has_indirect);

        p = mempset(h->direct.storage, 0, hi->entry_size * hi->n_direct_buckets);
        memset(p, DIB_RAW_INIT, ଆକାର(dib_raw_t) * hi->n_direct_buckets);
}

ସ୍ଥିର ଶୂନ୍ୟ shared_hash_ଚାବି_initialize(ଶୂନ୍ୟ) {
        random_bytes(shared_hash_ଚାବି, ଆକାର(shared_hash_ଚାବି));
}

ସ୍ଥିର ସଂରଚନା HashmapBase* hashmap_base_new(ଅଚଳ ସଂରଚନା hash_ops *hash_ops, ଗଣନା HashmapType type) {
        HashmapBase *h;
        ଅଚଳ ସଂରଚନା hashmap_type_info *hi = &hashmap_type_info[type];

        bool use_pool = mempool_enabled && mempool_enabled();  /* mempool_enabled is a weak symbol */

        h = use_pool ? mempool_alloc0_tile(hi->mempool) : malloc0(hi->head_size);
        ଯଦି (!h)
                ଫେରନ୍ତୁ ଖାଲି;

        h->type = type;
        h->from_pool = use_pool;
        h->hash_ops = hash_ops ?: &trivial_hash_ops;

        ଯଦି (type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*)h;
                lh->iterate_list_head = lh->iterate_list_tail = IDX_NIL;
        }

        reset_direct_storage(h);

        ସ୍ଥିର pthread_once_t once = PTHREAD_ONCE_INIT;
        assert_se(pthread_once(&once, shared_hash_ଚାବି_initialize) == 0);

#ଯଦି ENABLE_DEBUG_HASHMAP
        assert_se(pthread_mutex_lock(&hashmap_debug_list_mutex) == 0);
        LIST_PREPEND(debug_list, hashmap_debug_list, &h->debug);
        assert_se(pthread_mutex_unlock(&hashmap_debug_list_mutex) == 0);
#endif

        ଫେରନ୍ତୁ h;
}

Hashmap *hashmap_new(ଅଚଳ ସଂରଚନା hash_ops *hash_ops) {
        ଫେରନ୍ତୁ (Hashmap*)        hashmap_base_new(hash_ops, HASHMAP_TYPE_PLAIN);
}

OrderedHashmap *ordered_hashmap_new(ଅଚଳ ସଂରଚନା hash_ops *hash_ops) {
        ଫେରନ୍ତୁ (OrderedHashmap*) hashmap_base_new(hash_ops, HASHMAP_TYPE_ORDERED);
}

Set *set_new(ଅଚଳ ସଂରଚନା hash_ops *hash_ops) {
        ଫେରନ୍ତୁ (Set*)            hashmap_base_new(hash_ops, HASHMAP_TYPE_SET);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା hashmap_base_ensure_allocated(HashmapBase **h, ଅଚଳ ସଂରଚନା hash_ops *hash_ops,
                                         ଗଣନା HashmapType type) {
        HashmapBase *q;

        ପ୍ରତିଜ୍ଞା(h);

        ଯଦି (*h) {
                ପ୍ରତିଜ୍ଞା((*h)->hash_ops == (hash_ops ?: &trivial_hash_ops));
                ଫେରନ୍ତୁ 0;
        }

        q = hashmap_base_new(hash_ops, type);
        ଯଦି (!q)
                ଫେରନ୍ତୁ -ENOMEM;

        *h = q;
        ଫେରନ୍ତୁ 1;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା hashmap_ensure_allocated(Hashmap **h, ଅଚଳ ସଂରଚନା hash_ops *hash_ops) {
        ଫେରନ୍ତୁ hashmap_base_ensure_allocated((HashmapBase**)h, hash_ops, HASHMAP_TYPE_PLAIN);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା ordered_hashmap_ensure_allocated(OrderedHashmap **h, ଅଚଳ ସଂରଚନା hash_ops *hash_ops) {
        ଫେରନ୍ତୁ hashmap_base_ensure_allocated((HashmapBase**)h, hash_ops, HASHMAP_TYPE_ORDERED);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା set_ensure_allocated(Set **s, ଅଚଳ ସଂରଚନା hash_ops *hash_ops) {
        ଫେରନ୍ତୁ hashmap_base_ensure_allocated((HashmapBase**)s, hash_ops, HASHMAP_TYPE_SET);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା hashmap_ensure_put(Hashmap **h, ଅଚଳ ସଂରଚନା hash_ops *hash_ops, ଅଚଳ ଶୂନ୍ୟ *ଚାବି, ଶୂନ୍ୟ *ମୂଲ୍ୟ) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(h);

        r = hashmap_ensure_allocated(h, hash_ops);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ hashmap_put(*h, ଚାବି, ମୂଲ୍ୟ);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା ordered_hashmap_ensure_put(OrderedHashmap **h, ଅଚଳ ସଂରଚନା hash_ops *hash_ops, ଅଚଳ ଶୂନ୍ୟ *ଚାବି, ଶୂନ୍ୟ *ମୂଲ୍ୟ) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(h);

        r = ordered_hashmap_ensure_allocated(h, hash_ops);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ ordered_hashmap_put(*h, ଚାବି, ମୂଲ୍ୟ);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା ordered_hashmap_ensure_replace(OrderedHashmap **h, ଅଚଳ ସଂରଚନା hash_ops *hash_ops, ଅଚଳ ଶୂନ୍ୟ *ଚାବି, ଶୂନ୍ୟ *ମୂଲ୍ୟ) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(h);

        r = ordered_hashmap_ensure_allocated(h, hash_ops);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ ordered_hashmap_replace(*h, ଚାବି, ମୂଲ୍ୟ);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା hashmap_ensure_replace(Hashmap **h, ଅଚଳ ସଂରଚନା hash_ops *hash_ops, ଅଚଳ ଶୂନ୍ୟ *ଚାବି, ଶୂନ୍ୟ *ମୂଲ୍ୟ) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(h);

        r = hashmap_ensure_allocated(h, hash_ops);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ hashmap_replace(*h, ଚାବି, ମୂଲ୍ୟ);
}

ସ୍ଥିର ଶୂନ୍ୟ hashmap_free_no_clear(HashmapBase *h) {
        ପ୍ରତିଜ୍ଞା(!h->has_indirect);
        ପ୍ରତିଜ୍ଞା(h->n_direct_entries == 0);

#ଯଦି ENABLE_DEBUG_HASHMAP
        assert_se(pthread_mutex_lock(&hashmap_debug_list_mutex) == 0);
        LIST_REMOVE(debug_list, hashmap_debug_list, &h->debug);
        assert_se(pthread_mutex_unlock(&hashmap_debug_list_mutex) == 0);
#endif

        ଯଦି (h->from_pool) {
                /* Ensure that the object didn't get migrated between threads. */
                assert_se(is_main_thread());
                mempool_free_tile(hashmap_type_info[h->type].mempool, h);
        } ଅନ୍ୟଥା
                ମୁକ୍ତ(h);
}

HashmapBase* _hashmap_free(HashmapBase *h) {
        ଯଦି (h) {
                _hashmap_clear(h);
                hashmap_free_no_clear(h);
        }

        ଫେରନ୍ତୁ ଖାଲି;
}

ଶୂନ୍ୟ _hashmap_clear(HashmapBase *h) {
        ଯଦି (!h)
                ଫେରନ୍ତୁ;

        ଯଦି (h->hash_ops->free_ଚାବି || h->hash_ops->free_ମୂଲ୍ୟ) {

                /* If destructor calls are defined, let's destroy things defensively: let's take the item out of the
                 * hash table, and only then call the destructor functions. If these destructors then try to unregister
                 * themselves from our hash table a second time, the entry is already gone. */

                ଯେତେବେଳେ (_hashmap_size(h) > 0) {
                        ଶୂନ୍ୟ *k = ଖାଲି;
                        ଶୂନ୍ୟ *v;

                        v = hashmap_first_ଚାବି_and_ମୂଲ୍ୟ(h, true, &k);

                        ଯଦି (h->hash_ops->free_ଚାବି)
                                h->hash_ops->free_ଚାବି(k);

                        ଯଦି (h->hash_ops->free_ମୂଲ୍ୟ)
                                h->hash_ops->free_ମୂଲ୍ୟ(v);
                }
        }

        ଯଦି (h->has_indirect) {
                ମୁକ୍ତ(h->indirect.storage);
                h->has_indirect = false;
        }

        h->n_direct_entries = 0;
        reset_direct_storage(h);

        ଯଦି (h->type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*) h;
                lh->iterate_list_head = lh->iterate_list_tail = IDX_NIL;
        }

        base_set_dirty(h);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା resize_buckets(HashmapBase *h, unsigned entries_add);

/*
 * Finds an empty bucket to put an entry into, starting the scan at 'idx'.
 * Performs Robin Hood swaps as it goes. The entry to put must be placed
 * by the caller into swap slot IDX_PUT.
 * If used for in-place resizing, may leave a displaced entry in swap slot
 * IDX_PUT. Caller must rehash it next.
 * Returns: true if it left a displaced entry to rehash next in IDX_PUT,
 *          false otherwise.
 */
ସ୍ଥିର bool hashmap_put_robin_hood(HashmapBase *h, unsigned idx,
                                   ସଂରଚନା swap_entries *swap) {
        dib_raw_t raw_dib, *dibs;
        unsigned dib, distance;

#ଯଦି ENABLE_DEBUG_HASHMAP
        ପ୍ରତିଜ୍ଞା(h);
        h->debug.put_ଗଣନା++;
#endif

        dibs = dib_raw_ptr(h);

        ପାଇଁ (distance = 0; ; distance++) {
                raw_dib = dibs[idx];
                ଯଦି (IN_SET(raw_dib, DIB_RAW_FREE, DIB_RAW_REHASH)) {
                        ଯଦି (raw_dib == DIB_RAW_REHASH)
                                bucket_move_entry(h, swap, idx, IDX_TMP);

                        ଯଦି (h->has_indirect && h->indirect.idx_lowest_entry > idx)
                                h->indirect.idx_lowest_entry = idx;

                        bucket_set_dib(h, idx, distance);
                        bucket_move_entry(h, swap, IDX_PUT, idx);
                        ଯଦି (raw_dib == DIB_RAW_REHASH) {
                                bucket_move_entry(h, swap, IDX_TMP, IDX_PUT);
                                ଫେରନ୍ତୁ true;
                        }

                        ଫେରନ୍ତୁ false;
                }

                dib = bucket_calculate_dib(h, idx, raw_dib);

                ଯଦି (dib < distance) {
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
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା hashmap_base_put_boldly(HashmapBase *h, unsigned idx,
                                   ସଂରଚନା swap_entries *swap, bool may_resize) {
        ସଂରଚନା ordered_hashmap_entry *new_entry;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(idx < n_buckets(h));

        new_entry = bucket_at_swap(swap, IDX_PUT);

        ଯଦି (may_resize) {
                r = resize_buckets(h, 1);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
                ଯଦି (r > 0)
                        idx = bucket_hash(h, new_entry->p.b.ଚାବି);
        }
        ପ୍ରତିଜ୍ଞା(n_entries(h) < n_buckets(h));

        ଯଦି (h->type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*) h;

                new_entry->iterate_next = IDX_NIL;
                new_entry->iterate_previous = lh->iterate_list_tail;

                ଯଦି (lh->iterate_list_tail != IDX_NIL) {
                        ସଂରଚନା ordered_hashmap_entry *old_tail;

                        old_tail = ordered_bucket_at(lh, lh->iterate_list_tail);
                        ପ୍ରତିଜ୍ଞା(old_tail->iterate_next == IDX_NIL);
                        old_tail->iterate_next = IDX_PUT;
                }

                lh->iterate_list_tail = IDX_PUT;
                ଯଦି (lh->iterate_list_head == IDX_NIL)
                        lh->iterate_list_head = IDX_PUT;
        }

        assert_se(hashmap_put_robin_hood(h, idx, swap) == false);

        n_entries_inc(h);
#ଯଦି ENABLE_DEBUG_HASHMAP
        h->debug.max_entries = MAX(h->debug.max_entries, n_entries(h));
#endif

        base_set_dirty(h);

        ଫେରନ୍ତୁ 1;
}
#define hashmap_put_boldly(h, idx, swap, may_resize) \
        hashmap_base_put_boldly(HASHMAP_BASE(h), idx, swap, may_resize)

/*
 * Returns 0 if resize is not needed.
 *         1 if successfully resized.
 *         -ENOMEM on allocation failure.
 */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା resize_buckets(HashmapBase *h, unsigned entries_add) {
        ସଂରଚନା swap_entries swap;
        ଶୂନ୍ୟ *new_storage;
        dib_raw_t *old_dibs, *new_dibs;
        ଅଚଳ ସଂରଚନା hashmap_type_info *hi;
        unsigned idx, optimal_idx;
        unsigned old_n_buckets, new_n_buckets, n_rehashed, new_n_entries;
        uint8_t new_shift;
        bool rehash_next;

        ପ୍ରତିଜ୍ଞା(h);

        hi = &hashmap_type_info[h->type];
        new_n_entries = n_entries(h) + entries_add;

        /* overflow? */
        ଯଦି (_unlikely_(new_n_entries < entries_add))
                ଫେରନ୍ତୁ -ENOMEM;

        /* For direct storage we allow 100% load, because it's tiny. */
        ଯଦି (!h->has_indirect && new_n_entries <= hi->n_direct_buckets)
                ଫେରନ୍ତୁ 0;

        /*
         * Load factor = n/m = 1 - (1/INV_KEEP_FREE).
         * From it follows: m = n + n/(INV_KEEP_FREE - 1)
         */
        new_n_buckets = new_n_entries + new_n_entries / (INV_KEEP_FREE - 1);
        /* overflow? */
        ଯଦି (_unlikely_(new_n_buckets < new_n_entries))
                ଫେରନ୍ତୁ -ENOMEM;

        old_n_buckets = n_buckets(h);

        ଯଦି (_likely_(new_n_buckets <= old_n_buckets))
                ଫେରନ୍ତୁ 0;

        ଯଦି (_unlikely_(new_n_buckets > UINT_MAX / (hi->entry_size + ଆକାର(dib_raw_t))))
                ଫେରନ୍ତୁ -ENOMEM;

        new_shift = log2u_round_up(MAX(
                        new_n_buckets * (hi->entry_size + ଆକାର(dib_raw_t)),
                        2 * ଆକାର(ସଂରଚନା direct_storage)));

        /* Realloc storage (buckets and DIB array). */
        new_storage = realloc(h->has_indirect ? h->indirect.storage : ଖାଲି,
                              1U << new_shift);
        ଯଦି (!new_storage)
                ଫେରନ୍ତୁ -ENOMEM;

        /* Must upgrade direct to indirect storage. */
        ଯଦି (!h->has_indirect) {
                memcpy(new_storage, h->direct.storage,
                       old_n_buckets * (hi->entry_size + ଆକାର(dib_raw_t)));
                h->indirect.n_entries = h->n_direct_entries;
                h->indirect.idx_lowest_entry = 0;
                h->n_direct_entries = 0;
        }

        /* Get a new hash key. If we've just upgraded to indirect storage,
         * allow reusing a previously generated key. It's still a different key
         * from the shared one that we used for direct storage. */
        get_hash_ଚାବି(h->indirect.hash_ଚାବି, !h->has_indirect);

        h->has_indirect = true;
        h->indirect.storage = new_storage;
        h->indirect.n_buckets = (1U << new_shift) /
                                (hi->entry_size + ଆକାର(dib_raw_t));

        old_dibs = (dib_raw_t*)((uint8_t*) new_storage + hi->entry_size * old_n_buckets);
        new_dibs = dib_raw_ptr(h);

        /*
         * Move the DIB array to the new place, replacing valid DIB values with
         * DIB_RAW_REHASH to indicate all of the used buckets need rehashing.
         * Note: Overlap is not possible, because we have at least doubled the
         * number of buckets and dib_raw_t is smaller than any entry type.
         */
        ପାଇଁ (idx = 0; idx < old_n_buckets; idx++) {
                ପ୍ରତିଜ୍ଞା(old_dibs[idx] != DIB_RAW_REHASH);
                new_dibs[idx] = old_dibs[idx] == DIB_RAW_FREE ? DIB_RAW_FREE
                                                              : DIB_RAW_REHASH;
        }

        /* Zero the area of newly added entries (including the old DIB area) */
        memzero(bucket_at(h, old_n_buckets),
               (n_buckets(h) - old_n_buckets) * hi->entry_size);

        /* The upper half of the new DIB array needs initialization */
        memset(&new_dibs[old_n_buckets], DIB_RAW_INIT,
               (n_buckets(h) - old_n_buckets) * ଆକାର(dib_raw_t));

        /* Rehash entries that need it */
        n_rehashed = 0;
        ପାଇଁ (idx = 0; idx < old_n_buckets; idx++) {
                ଯଦି (new_dibs[idx] != DIB_RAW_REHASH)
                        ଜାରି;

                optimal_idx = bucket_hash(h, bucket_at(h, idx)->ଚାବି);

                /*
                 * Not much to do if by luck the entry hashes to its current
                 * location. Just set its DIB.
                 */
                ଯଦି (optimal_idx == idx) {
                        new_dibs[idx] = 0;
                        n_rehashed++;
                        ଜାରି;
                }

                new_dibs[idx] = DIB_RAW_FREE;
                bucket_move_entry(h, &swap, idx, IDX_PUT);
                /* bucket_move_entry does not clear the source */
                memzero(bucket_at(h, idx), hi->entry_size);

                କର {
                        /*
                         * Find the new bucket for the current entry. This may make
                         * another entry homeless and load it into IDX_PUT.
                         */
                        rehash_next = hashmap_put_robin_hood(h, optimal_idx, &swap);
                        n_rehashed++;

                        /* Did the current entry displace another one? */
                        ଯଦି (rehash_next)
                                optimal_idx = bucket_hash(h, bucket_at_swap(&swap, IDX_PUT)->p.b.ଚାବି);
                } ଯେତେବେଳେ (rehash_next);
        }

        assert_se(n_rehashed == n_entries(h));

        ଫେରନ୍ତୁ 1;
}

/*
 * Finds an entry with a matching key
 * Returns: index of the found entry, or IDX_NIL if not found.
 */
ସ୍ଥିର unsigned base_bucket_scan(HashmapBase *h, unsigned idx, ଅଚଳ ଶୂନ୍ୟ *ଚାବି) {
        compare_func_t compare = h->hash_ops->compare;
        ସଂରଚନା hashmap_base_entry *e;
        unsigned dib, distance;
        dib_raw_t *dibs = dib_raw_ptr(h);

        ପ୍ରତିଜ୍ଞା(idx < n_buckets(h));

        ପାଇଁ (distance = 0; ; distance++) {
                ଯଦି (dibs[idx] == DIB_RAW_FREE)
                        ଫେରନ୍ତୁ IDX_NIL;

                dib = bucket_calculate_dib(h, idx, dibs[idx]);

                ଯଦି (dib < distance)
                        ଫେରନ୍ତୁ IDX_NIL;
                ଯଦି (dib == distance) {
                        e = bucket_at(h, idx);

                        /* Most maps compare keys by pointer. Do that here, without the indirect call. */
                        ଯଦି (compare == trivial_compare_func ? e->ଚାବି == ଚାବି : compare(e->ଚାବି, ଚାବି) == 0)
                                ଫେରନ୍ତୁ idx;
                }

                idx = next_idx(h, idx);
        }
}
#define bucket_scan(h, idx, ଚାବି) base_bucket_scan(HASHMAP_BASE(h), idx, ଚାବି)

ପୂର୍ଣ୍ଣସଂଖ୍ୟା hashmap_put(Hashmap *h, ଅଚଳ ଶୂନ୍ୟ *ଚାବି, ଶୂନ୍ୟ *ମୂଲ୍ୟ) {
        ସଂରଚନା swap_entries swap;
        ସଂରଚନା plain_hashmap_entry *e;
        unsigned hash, idx;

        ପ୍ରତିଜ୍ଞା(h);

        hash = bucket_hash(h, ଚାବି);
        idx = bucket_scan(h, hash, ଚାବି);
        ଯଦି (idx != IDX_NIL) {
                e = plain_bucket_at(h, idx);
                ଯଦି (e->ମୂଲ୍ୟ == ମୂଲ୍ୟ)
                        ଫେରନ୍ତୁ 0;
                ଫେରନ୍ତୁ -EEXIST;
        }

        e = &bucket_at_swap(&swap, IDX_PUT)->p;
        e->b.ଚାବି = ଚାବି;
        e->ମୂଲ୍ୟ = ମୂଲ୍ୟ;
        ଫେରନ୍ତୁ hashmap_put_boldly(h, hash, &swap, true);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା set_put(Set *s, ଅଚଳ ଶୂନ୍ୟ *ଚାବି) {
        ସଂରଚନା swap_entries swap;
        ସଂରଚନା hashmap_base_entry *e;
        unsigned hash, idx;

        ପ୍ରତିଜ୍ଞା(s);

        hash = bucket_hash(s, ଚାବି);
        idx = bucket_scan(s, hash, ଚାବି);
        ଯଦି (idx != IDX_NIL)
                ଫେରନ୍ତୁ 0;

        e = &bucket_at_swap(&swap, IDX_PUT)->p.b;
        e->ଚାବି = ଚାବି;
        ଫେରନ୍ତୁ hashmap_put_boldly(s, hash, &swap, true);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା set_ensure_put(Set **s, ଅଚଳ ସଂରଚନା hash_ops *hash_ops, ଅଚଳ ଶୂନ୍ୟ *ଚାବି) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(s);

        r = set_ensure_allocated(s, hash_ops);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ set_put(*s, ଚାବି);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା set_ensure_consume(Set **s, ଅଚଳ ସଂରଚନା hash_ops *hash_ops, ଶୂନ୍ୟ *ଚାବି) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        r = set_ensure_put(s, hash_ops, ଚାବି);
        ଯଦି (r <= 0) {
                ଯଦି (hash_ops && hash_ops->free_ଚାବି)
                        hash_ops->free_ଚାବି(ଚାବି);
                ଅନ୍ୟଥା ଯଦି (hash_ops && hash_ops->free_ମୂଲ୍ୟ)
                        /* Sets store their element in the key slot but may carry a value destructor. */
                        hash_ops->free_ମୂଲ୍ୟ(ଚାବି);
                ଅନ୍ୟଥା
                        ମୁକ୍ତ(ଚାବି);
        }

        ଫେରନ୍ତୁ r;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା hashmap_replace(Hashmap *h, ଅଚଳ ଶୂନ୍ୟ *ଚାବି, ଶୂନ୍ୟ *ମୂଲ୍ୟ) {
        ସଂରଚନା swap_entries swap;
        ସଂରଚନା plain_hashmap_entry *e;
        unsigned hash, idx;

        ପ୍ରତିଜ୍ଞା(h);

        hash = bucket_hash(h, ଚାବି);
        idx = bucket_scan(h, hash, ଚାବି);
        ଯଦି (idx != IDX_NIL) {
                e = plain_bucket_at(h, idx);
#ଯଦି ENABLE_DEBUG_HASHMAP
                /* Although the key is equal, the key pointer may have changed,
                 * and this would break our assumption for iterating. So count
                 * this operation as incompatible with iteration. */
                ଯଦି (e->b.ଚାବି != ଚାବି) {
                        h->b.debug.put_ଗଣନା++;
                        h->b.debug.rem_ଗଣନା++;
                        h->b.debug.last_rem_idx = idx;
                }
#endif
                e->b.ଚାବି = ଚାବି;
                e->ମୂଲ୍ୟ = ମୂଲ୍ୟ;
                hashmap_set_dirty(h);

                ଫେରନ୍ତୁ 0;
        }

        e = &bucket_at_swap(&swap, IDX_PUT)->p;
        e->b.ଚାବି = ଚାବି;
        e->ମୂଲ୍ୟ = ମୂଲ୍ୟ;
        ଫେରନ୍ତୁ hashmap_put_boldly(h, hash, &swap, true);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା hashmap_update(Hashmap *h, ଅଚଳ ଶୂନ୍ୟ *ଚାବି, ଶୂନ୍ୟ *ମୂଲ୍ୟ) {
        ସଂରଚନା plain_hashmap_entry *e;
        unsigned hash, idx;

        ପ୍ରତିଜ୍ଞା(h);

        hash = bucket_hash(h, ଚାବି);
        idx = bucket_scan(h, hash, ଚାବି);
        ଯଦି (idx == IDX_NIL)
                ଫେରନ୍ତୁ -ENOENT;

        e = plain_bucket_at(h, idx);
        e->ମୂଲ୍ୟ = ମୂଲ୍ୟ;
        hashmap_set_dirty(h);

        ଫେରନ୍ତୁ 0;
}

ଶୂନ୍ୟ* _hashmap_get(HashmapBase *h, ଅଚଳ ଶୂନ୍ୟ *ଚାବି) {
        ସଂରଚନା hashmap_base_entry *e;
        unsigned hash, idx;

        ଯଦି (!h)
                ଫେରନ୍ତୁ ଖାଲି;

        hash = bucket_hash(h, ଚାବି);
        idx = bucket_scan(h, hash, ଚାବି);
        ଯଦି (idx == IDX_NIL)
                ଫେରନ୍ତୁ ଖାଲି;

        e = bucket_at(h, idx);
        ଫେରନ୍ତୁ entry_ମୂଲ୍ୟ(h, e);
}

ଶୂନ୍ୟ* hashmap_get2(Hashmap *h, ଅଚଳ ଶୂନ୍ୟ *ଚାବି, ଶୂନ୍ୟ **ret) {
        ସଂରଚନା plain_hashmap_entry *e;
        unsigned hash, idx;

        ଯଦି (!h)
                ଫେରନ୍ତୁ ଖାଲି;

        hash = bucket_hash(h, ଚାବି);
        idx = bucket_scan(h, hash, ଚାବି);
        ଯଦି (idx == IDX_NIL)
                ଫେରନ୍ତୁ ଖାଲି;

        e = plain_bucket_at(h, idx);
        ଯଦି (ret)
                *ret = (ଶୂନ୍ୟ*) e->b.ଚାବି;

        ଫେରନ୍ତୁ e->ମୂଲ୍ୟ;
}

bool _hashmap_contains(HashmapBase *h, ଅଚଳ ଶୂନ୍ୟ *ଚାବି) {
        unsigned hash;

        ଯଦି (!h)
                ଫେରନ୍ତୁ false;

        hash = bucket_hash(h, ଚାବି);
        ଫେରନ୍ତୁ bucket_scan(h, hash, ଚାବି) != IDX_NIL;
}

ଶୂନ୍ୟ* _hashmap_remove(HashmapBase *h, ଅଚଳ ଶୂନ୍ୟ *ଚାବି) {
        ସଂରଚନା hashmap_base_entry *e;
        unsigned hash, idx;
        ଶୂନ୍ୟ *data;

        ଯଦି (!h)
                ଫେରନ୍ତୁ ଖାଲି;

        hash = bucket_hash(h, ଚାବି);
        idx = bucket_scan(h, hash, ଚାବି);
        ଯଦି (idx == IDX_NIL)
                ଫେରନ୍ତୁ ଖାଲି;

        e = bucket_at(h, idx);
        data = entry_ମୂଲ୍ୟ(h, e);
        remove_entry(h, idx);

        ଫେରନ୍ତୁ data;
}

ଶୂନ୍ୟ* hashmap_remove2(Hashmap *h, ଅଚଳ ଶୂନ୍ୟ *ଚାବି, ଶୂନ୍ୟ **ret) {
        ସଂରଚନା plain_hashmap_entry *e;
        unsigned hash, idx;
        ଶୂନ୍ୟ *data;

        ଯଦି (!h) {
                ଯଦି (ret)
                        *ret = ଖାଲି;
                ଫେରନ୍ତୁ ଖାଲି;
        }

        hash = bucket_hash(h, ଚାବି);
        idx = bucket_scan(h, hash, ଚାବି);
        ଯଦି (idx == IDX_NIL) {
                ଯଦି (ret)
                        *ret = ଖାଲି;
                ଫେରନ୍ତୁ ଖାଲି;
        }

        e = plain_bucket_at(h, idx);
        data = e->ମୂଲ୍ୟ;
        ଯଦି (ret)
                *ret = (ଶୂନ୍ୟ*) e->b.ଚାବି;

        remove_entry(h, idx);

        ଫେରନ୍ତୁ data;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା hashmap_remove_and_put(Hashmap *h, ଅଚଳ ଶୂନ୍ୟ *old_ଚାବି, ଅଚଳ ଶୂନ୍ୟ *new_ଚାବି, ଶୂନ୍ୟ *ମୂଲ୍ୟ) {
        ସଂରଚନା swap_entries swap;
        ସଂରଚନା plain_hashmap_entry *e;
        unsigned old_hash, new_hash, idx;

        ଯଦି (!h)
                ଫେରନ୍ତୁ -ENOENT;

        old_hash = bucket_hash(h, old_ଚାବି);
        idx = bucket_scan(h, old_hash, old_ଚାବି);
        ଯଦି (idx == IDX_NIL)
                ଫେରନ୍ତୁ -ENOENT;

        new_hash = bucket_hash(h, new_ଚାବି);
        ଯଦି (bucket_scan(h, new_hash, new_ଚାବି) != IDX_NIL)
                ଫେରନ୍ତୁ -EEXIST;

        remove_entry(h, idx);

        e = &bucket_at_swap(&swap, IDX_PUT)->p;
        e->b.ଚାବି = new_ଚାବି;
        e->ମୂଲ୍ୟ = ମୂଲ୍ୟ;
        assert_se(hashmap_put_boldly(h, new_hash, &swap, false) == 1);

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା set_remove_and_put(Set *s, ଅଚଳ ଶୂନ୍ୟ *old_ଚାବି, ଅଚଳ ଶୂନ୍ୟ *new_ଚାବି) {
        ସଂରଚନା swap_entries swap;
        ସଂରଚନା hashmap_base_entry *e;
        unsigned old_hash, new_hash, idx;

        ଯଦି (!s)
                ଫେରନ୍ତୁ -ENOENT;

        old_hash = bucket_hash(s, old_ଚାବି);
        idx = bucket_scan(s, old_hash, old_ଚାବି);
        ଯଦି (idx == IDX_NIL)
                ଫେରନ୍ତୁ -ENOENT;

        new_hash = bucket_hash(s, new_ଚାବି);
        ଯଦି (bucket_scan(s, new_hash, new_ଚାବି) != IDX_NIL)
                ଫେରନ୍ତୁ -EEXIST;

        remove_entry(s, idx);

        e = &bucket_at_swap(&swap, IDX_PUT)->p.b;
        e->ଚାବି = new_ଚାବି;
        assert_se(hashmap_put_boldly(s, new_hash, &swap, false) == 1);

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା hashmap_remove_and_replace(Hashmap *h, ଅଚଳ ଶୂନ୍ୟ *old_ଚାବି, ଅଚଳ ଶୂନ୍ୟ *new_ଚାବି, ଶୂନ୍ୟ *ମୂଲ୍ୟ) {
        ସଂରଚନା swap_entries swap;
        ସଂରଚନା plain_hashmap_entry *e;
        unsigned old_hash, new_hash, idx_old, idx_new;

        ଯଦି (!h)
                ଫେରନ୍ତୁ -ENOENT;

        old_hash = bucket_hash(h, old_ଚାବି);
        idx_old = bucket_scan(h, old_hash, old_ଚାବି);
        ଯଦି (idx_old == IDX_NIL)
                ଫେରନ୍ତୁ -ENOENT;

        old_ଚାବି = bucket_at(HASHMAP_BASE(h), idx_old)->ଚାବି;

        new_hash = bucket_hash(h, new_ଚାବି);
        idx_new = bucket_scan(h, new_hash, new_ଚାବି);
        ଯଦି (idx_new != IDX_NIL)
                ଯଦି (idx_old != idx_new) {
                        remove_entry(h, idx_new);
                        /* Compensate for a possible backward shift. */
                        ଯଦି (old_ଚାବି != bucket_at(HASHMAP_BASE(h), idx_old)->ଚାବି)
                                idx_old = prev_idx(HASHMAP_BASE(h), idx_old);
                        ପ୍ରତିଜ୍ଞା(old_ଚାବି == bucket_at(HASHMAP_BASE(h), idx_old)->ଚାବି);
                }

        remove_entry(h, idx_old);

        e = &bucket_at_swap(&swap, IDX_PUT)->p;
        e->b.ଚାବି = new_ଚାବି;
        e->ମୂଲ୍ୟ = ମୂଲ୍ୟ;
        assert_se(hashmap_put_boldly(h, new_hash, &swap, false) == 1);

        ଫେରନ୍ତୁ 0;
}

ଶୂନ୍ୟ* hashmap_remove_ମୂଲ୍ୟ(HashmapBase *h, ଅଚଳ ଶୂନ୍ୟ *ଚାବି, ଶୂନ୍ୟ *ମୂଲ୍ୟ) {
        ସଂରଚନା hashmap_base_entry *e;
        unsigned hash, idx;

        ଯଦି (!h)
                ଫେରନ୍ତୁ ଖାଲି;

        hash = bucket_hash(h, ଚାବି);
        idx = bucket_scan(h, hash, ଚାବି);
        ଯଦି (idx == IDX_NIL)
                ଫେରନ୍ତୁ ଖାଲି;

        e = bucket_at(h, idx);
        ଯଦି (entry_ମୂଲ୍ୟ(h, e) != ମୂଲ୍ୟ)
                ଫେରନ୍ତୁ ଖାଲି;

        remove_entry(h, idx);

        ଫେରନ୍ତୁ ମୂଲ୍ୟ;
}

ସ୍ଥିର unsigned find_first_entry(HashmapBase *h) {
        Iterator i = ITERATOR_FIRST;

        ଯଦି (!h || !n_entries(h))
                ଫେରନ୍ତୁ IDX_NIL;

        ଫେରନ୍ତୁ hashmap_iterate_entry(h, &i);
}

ଶୂନ୍ୟ* hashmap_first_ଚାବି_and_ମୂଲ୍ୟ(HashmapBase *h, bool remove, ଶୂନ୍ୟ **ret_ଚାବି) {
        ସଂରଚନା hashmap_base_entry *e;
        ଶୂନ୍ୟ *ଚାବି, *data;
        unsigned idx;

        idx = find_first_entry(h);
        ଯଦି (idx == IDX_NIL) {
                ଯଦି (ret_ଚାବି)
                        *ret_ଚାବି = ଖାଲି;
                ଫେରନ୍ତୁ ଖାଲି;
        }

        e = bucket_at(h, idx);
        ଚାବି = (ଶୂନ୍ୟ*) e->ଚାବି;
        data = entry_ମୂଲ୍ୟ(h, e);

        ଯଦି (remove)
                remove_entry(h, idx);

        ଯଦି (ret_ଚାବି)
                *ret_ଚାବି = ଚାବି;

        ଫେରନ୍ତୁ data;
}

unsigned _hashmap_size(HashmapBase *h) {
        ଯଦି (!h)
                ଫେରନ୍ତୁ 0;

        ଫେରନ୍ତୁ n_entries(h);
}

unsigned _hashmap_buckets(HashmapBase *h) {
        ଯଦି (!h)
                ଫେରନ୍ତୁ 0;

        ଫେରନ୍ତୁ n_buckets(h);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା _hashmap_merge(Hashmap *h, Hashmap *other) {
        Iterator i;
        unsigned idx;

        ପ୍ରତିଜ୍ଞା(h);

        HASHMAP_FOREACH_IDX(idx, HASHMAP_BASE(other), i) {
                ସଂରଚନା plain_hashmap_entry *pe = plain_bucket_at(other, idx);
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

                r = hashmap_put(h, pe->b.ଚାବି, pe->ମୂଲ୍ୟ);
                ଯଦି (r < 0 && r != -EEXIST)
                        ଫେରନ୍ତୁ r;
        }

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା set_merge(Set *s, Set *other) {
        Iterator i;
        unsigned idx;

        ପ୍ରତିଜ୍ଞା(s);

        HASHMAP_FOREACH_IDX(idx, HASHMAP_BASE(other), i) {
                ସଂରଚନା set_entry *se = set_bucket_at(other, idx);
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

                r = set_put(s, se->b.ଚାବି);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
        }

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା _hashmap_reserve(HashmapBase *h, unsigned entries_add) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(h);

        r = resize_buckets(h, entries_add);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ 0;
}

/*
 * The same as hashmap_merge(), but every new item from other is moved to h.
 * Keys already in h are skipped and stay in other.
 * Returns: 0 on success.
 *          -ENOMEM on alloc failure, in which case no move has been done.
 */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା _hashmap_move(HashmapBase *h, HashmapBase *other) {
        ସଂରଚନା swap_entries swap;
        ସଂରଚନା hashmap_base_entry *e, *n;
        Iterator i;
        unsigned idx;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(h);

        ଯଦି (!other)
                ଫେରନ୍ତୁ 0;

        ପ୍ରତିଜ୍ଞା(other->type == h->type);

        /*
         * This reserves buckets for the worst case, where none of other's
         * entries are yet present in h. This is preferable to risking
         * an allocation failure in the middle of the moving and having to
         * rollback or return a partial result.
         */
        r = resize_buckets(h, n_entries(other));
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        HASHMAP_FOREACH_IDX(idx, other, i) {
                unsigned h_hash;

                e = bucket_at(other, idx);
                h_hash = bucket_hash(h, e->ଚାବି);
                ଯଦି (bucket_scan(h, h_hash, e->ଚାବି) != IDX_NIL)
                        ଜାରି;

                n = &bucket_at_swap(&swap, IDX_PUT)->p.b;
                n->ଚାବି = e->ଚାବି;
                ଯଦି (h->type != HASHMAP_TYPE_SET)
                        ((ସଂରଚନା plain_hashmap_entry*) n)->ମୂଲ୍ୟ =
                                ((ସଂରଚନା plain_hashmap_entry*) e)->ମୂଲ୍ୟ;
                assert_se(hashmap_put_boldly(h, h_hash, &swap, false) == 1);

                remove_entry(other, idx);
        }

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା _hashmap_move_one(HashmapBase *h, HashmapBase *other, ଅଚଳ ଶୂନ୍ୟ *ଚାବି) {
        ସଂରଚନା swap_entries swap;
        unsigned h_hash, other_hash, idx;
        ସଂରଚନା hashmap_base_entry *e, *n;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(h);

        h_hash = bucket_hash(h, ଚାବି);
        ଯଦି (bucket_scan(h, h_hash, ଚାବି) != IDX_NIL)
                ଫେରନ୍ତୁ -EEXIST;

        ଯଦି (!other)
                ଫେରନ୍ତୁ -ENOENT;

        ପ୍ରତିଜ୍ଞା(other->type == h->type);

        other_hash = bucket_hash(other, ଚାବି);
        idx = bucket_scan(other, other_hash, ଚାବି);
        ଯଦି (idx == IDX_NIL)
                ଫେରନ୍ତୁ -ENOENT;

        e = bucket_at(other, idx);

        n = &bucket_at_swap(&swap, IDX_PUT)->p.b;
        n->ଚାବି = e->ଚାବି;
        ଯଦି (h->type != HASHMAP_TYPE_SET)
                ((ସଂରଚନା plain_hashmap_entry*) n)->ମୂଲ୍ୟ =
                        ((ସଂରଚନା plain_hashmap_entry*) e)->ମୂଲ୍ୟ;
        r = hashmap_put_boldly(h, h_hash, &swap, true);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        remove_entry(other, idx);
        ଫେରନ୍ତୁ 0;
}

HashmapBase* _hashmap_copy(HashmapBase *h) {
        HashmapBase *copy;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(h);

        copy = hashmap_base_new(h->hash_ops, h->type);
        ଯଦି (!copy)
                ଫେରନ୍ତୁ ଖାଲି;

        ବାଛ (h->type) {
        ସ୍ଥିତି HASHMAP_TYPE_PLAIN:
        ସ୍ଥିତି HASHMAP_TYPE_ORDERED:
                r = hashmap_merge((Hashmap*)copy, (Hashmap*)h);
                ଭାଙ୍ଗ;
        ସ୍ଥିତି HASHMAP_TYPE_SET:
                r = set_merge((Set*)copy, (Set*)h);
                ଭାଙ୍ଗ;
        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                assert_not_reached();
        }

        ଯଦି (r < 0)
                ଫେରନ୍ତୁ _hashmap_free(copy);

        ଫେରନ୍ତୁ copy;
}

ଅକ୍ଷର** _hashmap_get_strv(HashmapBase *h) {
        ଅକ୍ଷର **sv;
        Iterator i;
        unsigned idx, n;

        ଯଦି (!h)
                ଫେରନ୍ତୁ new0(ଅକ୍ଷର*, 1);

        sv = new(ଅକ୍ଷର*, n_entries(h)+1);
        ଯଦି (!sv)
                ଫେରନ୍ତୁ ଖାଲି;

        n = 0;
        HASHMAP_FOREACH_IDX(idx, h, i)
                sv[n++] = entry_ମୂଲ୍ୟ(h, bucket_at(h, idx));
        sv[n] = ଖାଲି;

        ଫେରନ୍ତୁ sv;
}

ଅକ୍ଷର** set_to_strv(Set **s) {
        ପ୍ରତିଜ୍ଞା(s);

        /* This is similar to set_get_strv(), but invalidates the set on success. */

        ଅକ୍ଷର **v = new(ଅକ୍ଷର*, set_size(*s) + 1);
        ଯଦି (!v)
                ଫେରନ୍ତୁ ଖାଲି;

        ପାଇଁ (ଅକ୍ଷର **p = v; (*p = set_steal_first(*s)); p++)
                ;

        ପ୍ରତିଜ୍ଞା(set_isempty(*s));
        *s = set_free(*s);
        ଫେରନ୍ତୁ v;
}

ଶୂନ୍ୟ* ordered_hashmap_next(OrderedHashmap *h, ଅଚଳ ଶୂନ୍ୟ *ଚାବି) {
        ସଂରଚନା ordered_hashmap_entry *e;
        unsigned hash, idx;

        ଯଦି (!h)
                ଫେରନ୍ତୁ ଖାଲି;

        hash = bucket_hash(h, ଚାବି);
        idx = bucket_scan(h, hash, ଚାବି);
        ଯଦି (idx == IDX_NIL)
                ଫେରନ୍ତୁ ଖାଲି;

        e = ordered_bucket_at(h, idx);
        ଯଦି (e->iterate_next == IDX_NIL)
                ଫେରନ୍ତୁ ଖାଲି;
        ଫେରନ୍ତୁ ordered_bucket_at(h, e->iterate_next)->p.ମୂଲ୍ୟ;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା set_consume(Set *s, ଶୂନ୍ୟ *ମୂଲ୍ୟ) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(s);
        ପ୍ରତିଜ୍ଞା(ମୂଲ୍ୟ);

        r = set_put(s, ମୂଲ୍ୟ);
        ଯଦି (r <= 0)
                ମୁକ୍ତ(ମୂଲ୍ୟ);

        ଫେରନ୍ତୁ r;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା hashmap_put_strdup_full(Hashmap **h, ଅଚଳ ସଂରଚନା hash_ops *hash_ops, ଅଚଳ ଅକ୍ଷର *k, ଅଚଳ ଅକ୍ଷର *v) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(h);

        r = hashmap_ensure_allocated(h, hash_ops);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        _cleanup_free_ ଅକ୍ଷର *kdup = ଖାଲି, *vdup = ଖାଲି;

        kdup = strdup(k);
        ଯଦି (!kdup)
                ଫେରନ୍ତୁ -ENOMEM;

        ଯଦି (v) {
                vdup = strdup(v);
                ଯଦି (!vdup)
                        ଫେରନ୍ତୁ -ENOMEM;
        }

        r = hashmap_put(*h, kdup, vdup);
        ଯଦି (r < 0) {
                ଯଦି (r == -EEXIST && streq_ptr(v, hashmap_get(*h, kdup)))
                        ଫେରନ୍ତୁ 0;
                ଫେରନ୍ତୁ r;
        }

        /* 0 with non-null vdup would mean vdup is already in the hashmap, which cannot be */
        ପ୍ରତିଜ୍ଞା(vdup == ଖାଲି || r > 0);
        ଯଦି (r > 0)
                kdup = vdup = ଖାଲି;

        ଫେରନ୍ତୁ r;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା set_put_strndup_full(Set **s, ଅଚଳ ସଂରଚନା hash_ops *hash_ops, ଅଚଳ ଅକ୍ଷର *p, size_t n) {
        ଅକ୍ଷର *c;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(s);
        ପ୍ରତିଜ୍ଞା(p);

        r = set_ensure_allocated(s, hash_ops);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (n == SIZE_MAX) {
                ଯଦି (set_contains(*s, (ଅକ୍ଷର*) p))
                        ଫେରନ୍ତୁ 0;

                c = strdup(p);
        } ଅନ୍ୟଥା
                c = strndup(p, n);
        ଯଦି (!c)
                ଫେରନ୍ତୁ -ENOMEM;

        ଫେରନ୍ତୁ set_consume(*s, c);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା set_put_strdupv_full(Set **s, ଅଚଳ ସଂରଚନା hash_ops *hash_ops, ଅକ୍ଷର **l) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା n = 0, r;

        ପ୍ରତିଜ୍ଞା(s);

        STRV_FOREACH(i, l) {
                r = set_put_strndup_full(s, hash_ops, *i, SIZE_MAX);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;

                n += r;
        }

        ଫେରନ୍ତୁ n;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା set_put_strsplit(Set *s, ଅଚଳ ଅକ୍ଷର *v, ଅଚଳ ଅକ୍ଷର *separators, ExtractFlags flags) {
        ଅଚଳ ଅକ୍ଷର *p = ASSERT_PTR(v);
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(s);

        ପାଇଁ (;;) {
                ଅକ୍ଷର *word;

                r = extract_first_word(&p, &word, separators, flags);
                ଯଦି (r <= 0)
                        ଫେରନ୍ତୁ r;

                r = set_consume(s, word);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
        }
}

/* expand the cachemem if needed, return true if newly (re)activated. */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା cachemem_maintain(CacheMem *mem, size_t size) {
        ପ୍ରତିଜ୍ଞା(mem);

        ଯଦି (!GREEDY_REALLOC(mem->ptr, size)) {
                ଯଦି (size > 0)
                        ଫେରନ୍ତୁ -ENOMEM;
        }

        ଯଦି (!mem->active) {
                mem->active = true;
                ଫେରନ୍ତୁ true;
        }

        ଫେରନ୍ତୁ false;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା iterated_cache_get(IteratedCache *cache, ଅଚଳ ଶୂନ୍ୟ ***res_ଚାବିଗୁଡ଼ିକ, ଅଚଳ ଶୂନ୍ୟ ***res_ମୂଲ୍ୟଗୁଡ଼ିକ, unsigned *res_n_entries) {
        bool sync_ଚାବିଗୁଡ଼ିକ = false, sync_ମୂଲ୍ୟଗୁଡ଼ିକ = false;
        size_t size;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(cache);
        ପ୍ରତିଜ୍ଞା(cache->hashmap);

        size = n_entries(cache->hashmap);

        ଯଦି (res_ଚାବିଗୁଡ଼ିକ) {
                r = cachemem_maintain(&cache->ଚାବିଗୁଡ଼ିକ, size);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;

                sync_ଚାବିଗୁଡ଼ିକ = r;
        } ଅନ୍ୟଥା
                cache->ଚାବିଗୁଡ଼ିକ.active = false;

        ଯଦି (res_ମୂଲ୍ୟଗୁଡ଼ିକ) {
                r = cachemem_maintain(&cache->values, size);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;

                sync_ମୂଲ୍ୟଗୁଡ଼ିକ = r;
        } ଅନ୍ୟଥା
                cache->values.active = false;

        ଯଦି (cache->hashmap->dirty) {
                ଯଦି (cache->ଚାବିଗୁଡ଼ିକ.active)
                        sync_ଚାବିଗୁଡ଼ିକ = true;
                ଯଦି (cache->values.active)
                        sync_ମୂଲ୍ୟଗୁଡ଼ିକ = true;

                cache->hashmap->dirty = false;
        }

        ଯଦି (sync_ଚାବିଗୁଡ଼ିକ || sync_ମୂଲ୍ୟଗୁଡ଼ିକ) {
                unsigned i, idx;
                Iterator iter;

                i = 0;
                HASHMAP_FOREACH_IDX(idx, cache->hashmap, iter) {
                        ସଂରଚନା hashmap_base_entry *e;

                        e = bucket_at(cache->hashmap, idx);

                        ଯଦି (sync_ଚାବିଗୁଡ଼ିକ)
                                cache->ଚାବିଗୁଡ଼ିକ.ptr[i] = e->ଚାବି;
                        ଯଦି (sync_ମୂଲ୍ୟଗୁଡ଼ିକ)
                                cache->values.ptr[i] = entry_ମୂଲ୍ୟ(cache->hashmap, e);
                        i++;
                }
        }

        ଯଦି (res_ଚାବିଗୁଡ଼ିକ)
                *res_ଚାବିଗୁଡ଼ିକ = cache->ଚାବିଗୁଡ଼ିକ.ptr;
        ଯଦି (res_ମୂଲ୍ୟଗୁଡ଼ିକ)
                *res_ମୂଲ୍ୟଗୁଡ଼ିକ = cache->values.ptr;
        ଯଦି (res_n_entries)
                *res_n_entries = size;

        ଫେରନ୍ତୁ 0;
}

IteratedCache* iterated_cache_free(IteratedCache *cache) {
        ଯଦି (cache) {
                ମୁକ୍ତ(cache->ଚାବିଗୁଡ଼ିକ.ptr);
                ମୁକ୍ତ(cache->values.ptr);
        }

        ଫେରନ୍ତୁ mfree(cache);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା set_strjoin(Set *s, ଅଚଳ ଅକ୍ଷର *separator, bool wrap_with_separator, ଅକ୍ଷର **ret) {
        _cleanup_free_ ଅକ୍ଷର *str = ଖାଲି;
        size_t separator_len, len = 0;
        ଅଚଳ ଅକ୍ଷର *ମୂଲ୍ୟ;
        bool first;

        ପ୍ରତିଜ୍ଞା(ret);

        ଯଦି (set_isempty(s)) {
                *ret = ଖାଲି;
                ଫେରନ୍ତୁ 0;
        }

        separator_len = strlen_ptr(separator);

        ଯଦି (separator_len == 0)
                wrap_with_separator = false;

        first = !wrap_with_separator;

        SET_FOREACH(ମୂଲ୍ୟ, s) {
                size_t l = strlen_ptr(ମୂଲ୍ୟ);

                ଯଦି (l == 0)
                        ଜାରି;

                ଯଦି (!GREEDY_REALLOC(str, len + l + (first ? 0 : separator_len) + (wrap_with_separator ? separator_len : 0) + 1))
                        ଫେରନ୍ତୁ -ENOMEM;

                ଯଦି (separator_len > 0 && !first) {
                        memcpy(str + len, separator, separator_len);
                        len += separator_len;
                }

                memcpy(str + len, ମୂଲ୍ୟ, l);
                len += l;
                first = false;
        }

        ଯଦି (wrap_with_separator) {
                memcpy(str + len, separator, separator_len);
                len += separator_len;
        }

        str[len] = '\0';

        *ret = TAKE_PTR(str);
        ଫେରନ୍ତୁ 0;
}

bool set_equal(Set *a, Set *b) {
        ଶୂନ୍ୟ *p;

        /* Checks whether each entry of 'a' is also in 'b' and vice versa, i.e. the two sets contain the same
         * entries */

        ଯଦି (a == b)
                ଫେରନ୍ତୁ true;

        ଯଦି (set_isempty(a) && set_isempty(b))
                ଫେରନ୍ତୁ true;

        ଯଦି (set_size(a) != set_size(b)) /* Cheap check that hopefully catches a lot of inequality cases
                                         * already */
                ଫେରନ୍ତୁ false;

        SET_FOREACH(p, a)
                ଯଦି (!set_contains(b, p))
                        ଫେରନ୍ତୁ false;

        /* If we have the same hashops, then we don't need to check things backwards given we compared the
         * size and that all of a is in b. */
        ଯଦି (a->b.hash_ops == b->b.hash_ops)
                ଫେରନ୍ତୁ true;

        SET_FOREACH(p, b)
                ଯଦି (!set_contains(a, p))
                        ଫେରନ୍ତୁ false;

        ଫେରନ୍ତୁ true;
}

ସ୍ଥିର bool set_fnmatch_one(Set *patterns, ଅଚଳ ଅକ୍ଷର *needle) {
        ଅଚଳ ଅକ୍ଷର *p;

        ପ୍ରତିଜ୍ଞା(needle);

        /* Any failure of fnmatch() is treated as equivalent to FNM_NOMATCH, i.e. as non-matching pattern */

        SET_FOREACH(p, patterns)
                ଯଦି (fnmatch(p, needle, 0) == 0)
                        ଫେରନ୍ତୁ true;

        ଫେରନ୍ତୁ false;
}

bool set_fnmatch(Set *include_patterns, Set *exclude_patterns, ଅଚଳ ଅକ୍ଷର *needle) {
        ପ୍ରତିଜ୍ଞା(needle);

        ଯଦି (set_fnmatch_one(exclude_patterns, needle))
                ଫେରନ୍ତୁ false;

        ଯଦି (set_isempty(include_patterns))
                ଫେରନ୍ତୁ true;

        ଫେରନ୍ତୁ set_fnmatch_one(include_patterns, needle);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା hashmap_entry_compare(
                ସଂରଚନା hashmap_base_entry * ଅଚଳ *a,
                ସଂରଚନା hashmap_base_entry * ଅଚଳ *b,
                compare_func_t compare) {

        ପ୍ରତିଜ୍ଞା(a && *a);
        ପ୍ରତିଜ୍ଞା(b && *b);
        ପ୍ରତିଜ୍ଞା(compare);

        ଫେରନ୍ତୁ compare((*a)->ଚାବି, (*b)->ଚାବି);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା _hashmap_dump_entries_sorted(
                HashmapBase *h,
                ଶୂନ୍ୟ ***ret,
                size_t *ret_n) {
        _cleanup_free_ ଶୂନ୍ୟ **entries = ଖାଲି;
        Iterator iter;
        unsigned idx;
        size_t n = 0;

        ପ୍ରତିଜ୍ଞା(ret);
        ପ୍ରତିଜ୍ଞା(ret_n);

        ଯଦି (_hashmap_size(h) == 0) {
                *ret = ଖାଲି;
                *ret_n = 0;
                ଫେରନ୍ତୁ 0;
        }

        /* We append one more element than needed so that the resulting array can be used as a strv. We
         * don't count this entry in the returned size. */
        entries = new(ଶୂନ୍ୟ*, _hashmap_size(h) + 1);
        ଯଦି (!entries)
                ଫେରନ୍ତୁ -ENOMEM;

        HASHMAP_FOREACH_IDX(idx, h, iter)
                entries[n++] = bucket_at(h, idx);

        ପ୍ରତିଜ୍ଞା(n == _hashmap_size(h));
        entries[n] = ଖାଲି;

        typesafe_qsort_r((ସଂରଚନା hashmap_base_entry**) entries, n,
                         hashmap_entry_compare, h->hash_ops->compare);

        *ret = TAKE_PTR(entries);
        *ret_n = n;
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା hashmap_dump_ଚାବିଗୁଡ଼ିକ_sorted(HashmapBase *h, ଶୂନ୍ୟ ***ret, size_t *ret_n) {
        _cleanup_free_ ଶୂନ୍ୟ **entries = ଖାଲି;
        size_t n;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(ret);

        r = _hashmap_dump_entries_sorted(h, &entries, &n);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        /* Reuse the array. */
        FOREACH_ARRAY(e, entries, n)
                *e = (ଶୂନ୍ୟ*) (*(ସଂରଚନା hashmap_base_entry**) e)->ଚାବି;

        *ret = TAKE_PTR(entries);
        ଯଦି (ret_n)
                *ret_n = n;
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା _hashmap_dump_sorted(HashmapBase *h, ଶୂନ୍ୟ ***ret, size_t *ret_n) {
        _cleanup_free_ ଶୂନ୍ୟ **entries = ଖାଲି;
        size_t n;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(ret);

        r = _hashmap_dump_entries_sorted(h, &entries, &n);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        /* Reuse the array. */
        FOREACH_ARRAY(e, entries, n)
                *e = entry_ମୂଲ୍ୟ(h, *(ସଂରଚନା hashmap_base_entry**) e);

        *ret = TAKE_PTR(entries);
        ଯଦି (ret_n)
                *ret_n = n;
        ଫେରନ୍ତୁ 0;
}
