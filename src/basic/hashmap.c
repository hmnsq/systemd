/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ <fnmatch.h>
#അന്തര്ഭാവ <pthread.h>
#അന്തര്ഭാവ <unistd.h>
#എങ്കിൽ HAVE_VALGRIND_VALGRIND_H
#  include <valgrind/valgrind.h>
#endif

#അന്തര്ഭാവ "alloc-util.h"
#അന്തര്ഭാവ "extract-word.h"
#അന്തര്ഭാവ "hashmap.h"
#അന്തര്ഭാവ "log.h"
#അന്തര്ഭാവ "logarithm.h"
#അന്തര്ഭാവ "memory-util.h"
#അന്തര്ഭാവ "mempool.h"
#അന്തര്ഭാവ "process-util.h"
#അന്തര്ഭാവ "random-util.h"
#അന്തര്ഭാവ "set.h"
#അന്തര്ഭാവ "siphash24.h"
#അന്തര്ഭാവ "sort-util.h"
#അന്തര്ഭാവ "string-util.h"
#അന്തര്ഭാവ "strv.h"

#എങ്കിൽ ENABLE_DEBUG_HASHMAP
#അന്തര്ഭാവ "list.h"
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
ഘടന hashmap_base_entry {
        സ്ഥിരം ശൂന്യം *താക്കോൽ;
};

/* Entry types for specific hashmap/set types
 * hashmap_base_entry must be at the beginning of each entry struct. */

ഘടന plain_hashmap_entry {
        ഘടന hashmap_base_entry b;
        ശൂന്യം *മൂല്യം;
};

ഘടന ordered_hashmap_entry {
        ഘടന plain_hashmap_entry p;
        unsigned iterate_next, iterate_previous;
};

ഘടന set_entry {
        ഘടന hashmap_base_entry b;
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
ഘടന swap_entries {
        ഘടന ordered_hashmap_entry e[_IDX_SWAP_END - _IDX_SWAP_BEGIN];
};

/* Distance from Initial Bucket */
തരനിർവചനം uint8_t dib_raw_t;
#define DIB_RAW_OVERFLOW ((dib_raw_t)0xfdU)   /* indicates DIB value is greater than representable */
#define DIB_RAW_REHASH   ((dib_raw_t)0xfeU)   /* entry yet to be rehashed during in-place resize */
#define DIB_RAW_FREE     ((dib_raw_t)0xffU)   /* a free bucket */
#define DIB_RAW_INIT     ((അക്ഷരം)DIB_RAW_FREE) /* a byte to memset a DIB store with when initializing */

#define DIB_FREE UINT_MAX

#എങ്കിൽ ENABLE_DEBUG_HASHMAP
ഘടന hashmap_debug_info {
        LIST_FIELDS(ഘടന hashmap_debug_info, debug_list);
        unsigned max_entries;  /* high watermark of n_entries */

        /* fields to detect modification while iterating */
        unsigned put_എണ്ണം;    /* counts puts into the hashmap */
        unsigned rem_എണ്ണം;    /* counts removals from hashmap */
        unsigned last_rem_idx; /* remembers last removal index */
};

/* Tracks all existing hashmaps. Get at it from gdb. See sd_dump_hashmaps.py */
സ്ഥിതം LIST_HEAD(ഘടന hashmap_debug_info, hashmap_debug_list);
സ്ഥിതം pthread_mutex_t hashmap_debug_list_mutex = PTHREAD_MUTEX_INITIALIZER;
#endif

എണ്ണൽ HashmapType {
        HASHMAP_TYPE_PLAIN,
        HASHMAP_TYPE_ORDERED,
        HASHMAP_TYPE_SET,
        _HASHMAP_TYPE_MAX
};

ഘടന _packed_ indirect_storage {
        ശൂന്യം *storage;                     /* where buckets and DIBs are stored */
        uint8_t  hash_താക്കോൽ[HASH_താക്കോൽ_SIZE];  /* hash key; changes during resize */

        unsigned n_entries;                /* number of stored entries */
        unsigned n_buckets;                /* number of buckets */

        unsigned idx_lowest_entry;         /* Index below which all buckets are free.
                                              Makes "while (hashmap_steal_first())" loops
                                              O(n) instead of O(n^2) for unordered hashmaps. */
        uint8_t  _pad[3];                  /* padding for the whole HashmapBase */
        /* The bitfields in HashmapBase complete the alignment of the whole thing. */
};

ഘടന direct_storage {
        /* This gives us 39 bytes on 64-bit, or 35 bytes on 32-bit.
         * That's room for 4 set_entries + 4 DIB bytes + 3 unused bytes on 64-bit,
         *              or 7 set_entries + 7 DIB bytes + 0 unused bytes on 32-bit. */
        uint8_t storage[വലുപ്പം(ഘടന indirect_storage)];
};

#define DIRECT_BUCKETS(entry_t) \
        (വലുപ്പം(ഘടന direct_storage) / (വലുപ്പം(entry_t) + വലുപ്പം(dib_raw_t)))

/* We should be able to store at least one entry directly. */
assert_cc(DIRECT_BUCKETS(ഘടന ordered_hashmap_entry) >= 1);

/* We have 3 bits for n_direct_entries. */
assert_cc(DIRECT_BUCKETS(ഘടന set_entry) < (1 << 3));

/* Hashmaps with directly stored entries all use this shared hash key.
 * It's no big deal if the key is guessed, because there can be only
 * a handful of directly stored entries in a hashmap. When a hashmap
 * outgrows direct storage, it gets its own key for indirect storage. */
സ്ഥിതം uint8_t shared_hash_താക്കോൽ[HASH_താക്കോൽ_SIZE];

/* Fields that all hashmap/set types must have */
ഘടന HashmapBase {
        സ്ഥിരം ഘടന hash_ops *hash_ops;  /* hash and compare ops to use */

        സംഘം _packed_ {
                ഘടന indirect_storage indirect; /* if  has_indirect */
                ഘടന direct_storage direct;     /* if !has_indirect */
        };

        എണ്ണൽ HashmapType type:2;     /* HASHMAP_TYPE_* */
        bool has_indirect:1;         /* whether indirect storage is used */
        unsigned n_direct_entries:3; /* Number of entries in direct storage.
                                      * Only valid if !has_indirect. */
        bool from_pool:1;            /* whether was allocated from mempool */
        bool dirty:1;                /* whether dirtied since last iterated_cache_get() */
        bool cached:1;               /* whether this hashmap is being cached */

#എങ്കിൽ ENABLE_DEBUG_HASHMAP
        ഘടന hashmap_debug_info debug;
#endif
};

/* Specific hash types
 * HashmapBase must be at the beginning of each hashmap struct. */

ഘടന Hashmap {
        ഘടന HashmapBase b;
};

ഘടന OrderedHashmap {
        ഘടന HashmapBase b;
        unsigned iterate_list_head, iterate_list_tail;
};

ഘടന Set {
        ഘടന HashmapBase b;
};

തരനിർവചനം ഘടന CacheMem {
        സ്ഥിരം ശൂന്യം **ptr;
        size_t n_populated;
        bool active:1;
} CacheMem;

ഘടന IteratedCache {
        HashmapBase *hashmap;
        CacheMem താക്കോലുകൾ, values;
};

DEFINE_MEMPOOL(hashmap_pool,         Hashmap,        8);
DEFINE_MEMPOOL(ordered_hashmap_pool, OrderedHashmap, 8);
/* No need for a separate Set pool */
assert_cc(വലുപ്പം(Hashmap) == വലുപ്പം(Set));

ഘടന hashmap_type_info {
        size_t head_size;
        size_t entry_size;
        ഘടന mempool *mempool;
        unsigned n_direct_buckets;
};

സ്ഥിതം _used_ സ്ഥിരം ഘടന hashmap_type_info hashmap_type_info[_HASHMAP_TYPE_MAX] = {
        [HASHMAP_TYPE_PLAIN] = {
                .head_size        = വലുപ്പം(Hashmap),
                .entry_size       = വലുപ്പം(ഘടന plain_hashmap_entry),
                .mempool          = &hashmap_pool,
                .n_direct_buckets = DIRECT_BUCKETS(ഘടന plain_hashmap_entry),
        },
        [HASHMAP_TYPE_ORDERED] = {
                .head_size        = വലുപ്പം(OrderedHashmap),
                .entry_size       = വലുപ്പം(ഘടന ordered_hashmap_entry),
                .mempool          = &ordered_hashmap_pool,
                .n_direct_buckets = DIRECT_BUCKETS(ഘടന ordered_hashmap_entry),
        },
        [HASHMAP_TYPE_SET] = {
                .head_size        = വലുപ്പം(Set),
                .entry_size       = വലുപ്പം(ഘടന set_entry),
                .mempool          = &hashmap_pool,
                .n_direct_buckets = DIRECT_BUCKETS(ഘടന set_entry),
        },
};

ശൂന്യം hashmap_trim_pools(ശൂന്യം) {
        പൂർണ്ണസംഖ്യ r;

        /* The pool is only allocated by the main thread, but the memory can be passed to other
         * threads. Let's clean up if we are the main thread and no other threads are live. */

        /* We build our own is_main_thread() here, which doesn't use C11 TLS based caching of the
         * result. That's because valgrind apparently doesn't like TLS to be used from a GCC destructor. */
        എങ്കിൽ (getpid() != gettid())
                തിരികെ (ശൂന്യം) log_debug("Not cleaning up memory pools, not in main thread.");

        r = get_process_threads(0);
        എങ്കിൽ (r < 0)
                തിരികെ (ശൂന്യം) log_debug_errno(r, "Failed to determine number of threads, not cleaning up memory pools: %m");
        എങ്കിൽ (r != 1)
                തിരികെ (ശൂന്യം) log_debug("Not cleaning up memory pools, running in multi-threaded process.");

        mempool_trim(&hashmap_pool);
        mempool_trim(&ordered_hashmap_pool);
}

#എങ്കിൽ HAVE_VALGRIND_VALGRIND_H
_destructor_ സ്ഥിതം ശൂന്യം cleanup_pools(ശൂന്യം) {
        /* Be nice to valgrind */
        എങ്കിൽ (RUNNING_ON_VALGRIND)
                hashmap_trim_pools();
}
#endif

സ്ഥിതം വരിയിൽ unsigned n_buckets(HashmapBase *h) {
        പ്രതിജ്ഞാ(h);
        തിരികെ h->has_indirect ? h->indirect.n_buckets
                               : hashmap_type_info[h->type].n_direct_buckets;
}

സ്ഥിതം unsigned n_entries(HashmapBase *h) {
        പ്രതിജ്ഞാ(h);
        തിരികെ h->has_indirect ? h->indirect.n_entries
                               : h->n_direct_entries;
}

സ്ഥിതം ശൂന്യം n_entries_inc(HashmapBase *h) {
        പ്രതിജ്ഞാ(h);

        എങ്കിൽ (h->has_indirect)
                h->indirect.n_entries++;
        അല്ലെങ്കിൽ
                h->n_direct_entries++;
}

സ്ഥിതം ശൂന്യം n_entries_dec(HashmapBase *h) {
        പ്രതിജ്ഞാ(h);

        എങ്കിൽ (h->has_indirect)
                h->indirect.n_entries--;
        അല്ലെങ്കിൽ
                h->n_direct_entries--;
}

സ്ഥിതം വരിയിൽ ശൂന്യം* storage_ptr(HashmapBase *h) {
        പ്രതിജ്ഞാ(h);
        തിരികെ h->has_indirect ? h->indirect.storage
                               : h->direct.storage;
}

സ്ഥിതം വരിയിൽ uint8_t* hash_താക്കോൽ(HashmapBase *h) {
        പ്രതിജ്ഞാ(h);
        തിരികെ h->has_indirect ? h->indirect.hash_താക്കോൽ
                               : shared_hash_താക്കോൽ;
}

സ്ഥിതം unsigned base_bucket_hash(HashmapBase *h, സ്ഥിരം ശൂന്യം *p) {
        ഘടന siphash state;
        uint64_t hash;

        പ്രതിജ്ഞാ(h);

        /* The keys of trivial_hash_ops and uint64_hash_ops fit in 64 bits. siphash24_uint64() hashes such
         * a key without the indirect call through hash_ops and keeps the SipHash state in registers. On
         * 32-bit and big-endian architectures, the result differs from the result of trivial_hash_func(),
         * which hashes the key in native byte order. Only this function maps keys to buckets, so the
         * difference does not matter. */
        എങ്കിൽ (h->hash_ops->hash == trivial_hash_func)
                hash = siphash24_uint64((uintptr_t) p, hash_താക്കോൽ(h));
        അല്ലെങ്കിൽ എങ്കിൽ (h->hash_ops->hash == (hash_func_t) uint64_hash_func)
                hash = siphash24_uint64(*(സ്ഥിരം uint64_t*) p, hash_താക്കോൽ(h));
        അല്ലെങ്കിൽ {
                siphash24_init(&state, hash_താക്കോൽ(h));
                h->hash_ops->hash(p, &state);
                hash = siphash24_finalize(&state);
        }

        /* n_buckets is not a power of two, so a modulo needs a division. Multiplying the upper 32 bits of
         * the hash with n_buckets and keeping the upper 32 bits of the product maps the hash to
         * [0, n_buckets) almost as evenly, without a division. */
        തിരികെ (unsigned) (((hash >> 32) * n_buckets(h)) >> 32);
}
#define bucket_hash(h, p) base_bucket_hash(HASHMAP_BASE(h), p)

സ്ഥിതം ശൂന്യം base_set_dirty(HashmapBase *h) {
        പ്രതിജ്ഞാ(h);

        h->dirty = true;
}
#define hashmap_set_dirty(h) base_set_dirty(HASHMAP_BASE(h))

സ്ഥിതം ശൂന്യം get_hash_താക്കോൽ(uint8_t hash_താക്കോൽ[HASH_താക്കോൽ_SIZE], bool reuse_is_ok) {
        സ്ഥിതം uint8_t current[HASH_താക്കോൽ_SIZE];
        സ്ഥിതം bool current_initialized = false;

        /* Returns a hash function key to use. In order to keep things
         * fast we will not generate a new key each time we allocate a
         * new hash table. Instead, we'll just reuse the most recently
         * generated one, except if we never generated one or when we
         * are rehashing an entire hash table because we reached a
         * fill level */

        എങ്കിൽ (!current_initialized || !reuse_is_ok) {
                random_bytes(current, വലുപ്പം(current));
                current_initialized = true;
        }

        memcpy(hash_താക്കോൽ, current, വലുപ്പം(current));
}

സ്ഥിതം വരിയിൽ ഘടന hashmap_base_entry* bucket_at(HashmapBase *h, unsigned idx) {
        പ്രതിജ്ഞാ(h);
        തിരികെ CAST_ALIGN_PTR(
                        ഘടന hashmap_base_entry,
                        (uint8_t *) storage_ptr(h) + idx * hashmap_type_info[h->type].entry_size);
}

സ്ഥിതം ഘടന plain_hashmap_entry* plain_bucket_at(Hashmap *h, unsigned idx) {
        തിരികെ (ഘടന plain_hashmap_entry*) bucket_at(HASHMAP_BASE(h), idx);
}

സ്ഥിതം ഘടന ordered_hashmap_entry* ordered_bucket_at(OrderedHashmap *h, unsigned idx) {
        തിരികെ (ഘടന ordered_hashmap_entry*) bucket_at(HASHMAP_BASE(h), idx);
}

സ്ഥിതം ഘടന set_entry *set_bucket_at(Set *h, unsigned idx) {
        തിരികെ (ഘടന set_entry*) bucket_at(HASHMAP_BASE(h), idx);
}

സ്ഥിതം ഘടന ordered_hashmap_entry* bucket_at_swap(ഘടന swap_entries *swap, unsigned idx) {
        പ്രതിജ്ഞാ(swap);
        തിരികെ &swap->e[idx - _IDX_SWAP_BEGIN];
}

/* Returns a pointer to the bucket at index idx.
 * Understands real indexes and swap indexes, hence "_virtual". */
സ്ഥിതം ഘടന hashmap_base_entry* bucket_at_virtual(HashmapBase *h, ഘടന swap_entries *swap,
                                                    unsigned idx) {
        എങ്കിൽ (idx < _IDX_SWAP_BEGIN)
                തിരികെ bucket_at(h, idx);

        എങ്കിൽ (idx < _IDX_SWAP_END)
                തിരികെ &bucket_at_swap(swap, idx)->p.b;

        assert_not_reached();
}

സ്ഥിതം വരിയിൽ dib_raw_t* dib_raw_ptr(HashmapBase *h) {
        പ്രതിജ്ഞാ(h);
        തിരികെ (dib_raw_t*)
                ((uint8_t*) storage_ptr(h) + hashmap_type_info[h->type].entry_size * n_buckets(h));
}

സ്ഥിതം unsigned bucket_distance(HashmapBase *h, unsigned idx, unsigned from) {
        തിരികെ idx >= from ? idx - from
                           : n_buckets(h) + idx - from;
}

സ്ഥിതം unsigned bucket_calculate_dib(HashmapBase *h, unsigned idx, dib_raw_t raw_dib) {
        unsigned initial_bucket;

        എങ്കിൽ (raw_dib == DIB_RAW_FREE)
                തിരികെ DIB_FREE;

        എങ്കിൽ (_likely_(raw_dib < DIB_RAW_OVERFLOW))
                തിരികെ raw_dib;

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
        initial_bucket = bucket_hash(h, bucket_at(h, idx)->താക്കോൽ);
        തിരികെ bucket_distance(h, idx, initial_bucket);
}

സ്ഥിതം ശൂന്യം bucket_set_dib(HashmapBase *h, unsigned idx, unsigned dib) {
        dib_raw_ptr(h)[idx] = dib != DIB_FREE ? MIN(dib, DIB_RAW_OVERFLOW) : DIB_RAW_FREE;
}

സ്ഥിതം unsigned skip_free_buckets(HashmapBase *h, unsigned idx) {
        dib_raw_t *dibs;

        dibs = dib_raw_ptr(h);

        വേണ്ടി ( ; idx < n_buckets(h); idx++)
                എങ്കിൽ (dibs[idx] != DIB_RAW_FREE)
                        തിരികെ idx;

        തിരികെ IDX_NIL;
}

സ്ഥിതം ശൂന്യം bucket_mark_free(HashmapBase *h, unsigned idx) {
        പ്രതിജ്ഞാ(h);

        memzero(bucket_at(h, idx), hashmap_type_info[h->type].entry_size);
        bucket_set_dib(h, idx, DIB_FREE);
}

സ്ഥിതം ശൂന്യം bucket_move_entry(HashmapBase *h, ഘടന swap_entries *swap,
                              unsigned from, unsigned to) {
        ഘടന hashmap_base_entry *e_from, *e_to;

        പ്രതിജ്ഞാ(h);
        പ്രതിജ്ഞാ(from != to);

        e_from = bucket_at_virtual(h, swap, from);
        e_to   = bucket_at_virtual(h, swap, to);

        memcpy(e_to, e_from, hashmap_type_info[h->type].entry_size);

        എങ്കിൽ (h->type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*) h;
                ഘടന ordered_hashmap_entry *le, *le_to;

                le_to = (ഘടന ordered_hashmap_entry*) e_to;

                എങ്കിൽ (le_to->iterate_next != IDX_NIL) {
                        le = (ഘടന ordered_hashmap_entry*)
                             bucket_at_virtual(h, swap, le_to->iterate_next);
                        le->iterate_previous = to;
                }

                എങ്കിൽ (le_to->iterate_previous != IDX_NIL) {
                        le = (ഘടന ordered_hashmap_entry*)
                             bucket_at_virtual(h, swap, le_to->iterate_previous);
                        le->iterate_next = to;
                }

                എങ്കിൽ (lh->iterate_list_head == from)
                        lh->iterate_list_head = to;
                എങ്കിൽ (lh->iterate_list_tail == from)
                        lh->iterate_list_tail = to;
        }
}

സ്ഥിതം വരിയിൽ unsigned next_idx(HashmapBase *h, unsigned idx) {
        തിരികെ idx + 1U < n_buckets(h) ? idx + 1U : 0U;
}

സ്ഥിതം unsigned prev_idx(HashmapBase *h, unsigned idx) {
        തിരികെ idx > 0U ? idx - 1U : n_buckets(h) - 1U;
}

സ്ഥിതം വരിയിൽ ശൂന്യം* entry_മൂല്യം(HashmapBase *h, ഘടന hashmap_base_entry *e) {
        പ്രതിജ്ഞാ(h);
        പ്രതിജ്ഞാ(e);

        തിരഞ്ഞെടുപ്പ് (h->type) {

        അവസ്ഥ HASHMAP_TYPE_PLAIN:
        അവസ്ഥ HASHMAP_TYPE_ORDERED:
                തിരികെ ((ഘടന plain_hashmap_entry*)e)->മൂല്യം;

        അവസ്ഥ HASHMAP_TYPE_SET:
                തിരികെ (ശൂന്യം*) e->താക്കോൽ;

        സ്ഥിരസ്ഥിതി:
                assert_not_reached();
        }
}

സ്ഥിതം ശൂന്യം base_remove_entry(HashmapBase *h, unsigned idx) {
        unsigned ഇടത്, വലത്, prev, dib;
        dib_raw_t raw_dib, *dibs;

        dibs = dib_raw_ptr(h);
        പ്രതിജ്ഞാ(dibs[idx] != DIB_RAW_FREE);

#എങ്കിൽ ENABLE_DEBUG_HASHMAP
        പ്രതിജ്ഞാ(h);
        h->debug.rem_എണ്ണം++;
        h->debug.last_rem_idx = idx;
#endif

        ഇടത് = idx;
        /* Find the stop bucket ("right"). It is either free or has DIB == 0. */
        വേണ്ടി (വലത് = next_idx(h, ഇടത്); ; വലത് = next_idx(h, വലത്)) {
                raw_dib = dibs[വലത്];
                എങ്കിൽ (IN_SET(raw_dib, 0, DIB_RAW_FREE))
                        നിർത്തുക;

                /* The buckets are not supposed to be all occupied and with DIB > 0.
                 * That would mean we could make everyone better off by shifting them
                 * backward. This scenario is impossible. */
                പ്രതിജ്ഞാ(ഇടത് != വലത്);
        }

        എങ്കിൽ (h->type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*) h;
                ഘടന ordered_hashmap_entry *le = ordered_bucket_at(lh, idx);

                എങ്കിൽ (le->iterate_next != IDX_NIL)
                        ordered_bucket_at(lh, le->iterate_next)->iterate_previous = le->iterate_previous;
                അല്ലെങ്കിൽ
                        lh->iterate_list_tail = le->iterate_previous;

                എങ്കിൽ (le->iterate_previous != IDX_NIL)
                        ordered_bucket_at(lh, le->iterate_previous)->iterate_next = le->iterate_next;
                അല്ലെങ്കിൽ
                        lh->iterate_list_head = le->iterate_next;
        }

        /* Now shift all buckets in the interval (left, right) one step backwards */
        വേണ്ടി (prev = ഇടത്, ഇടത് = next_idx(h, ഇടത്); ഇടത് != വലത്;
             prev = ഇടത്, ഇടത് = next_idx(h, ഇടത്)) {
                dib = bucket_calculate_dib(h, ഇടത്, dibs[ഇടത്]);
                പ്രതിജ്ഞാ(dib != 0);
                bucket_move_entry(h, ശൂന്യ, ഇടത്, prev);
                bucket_set_dib(h, prev, dib - 1);
        }

        bucket_mark_free(h, prev);
        n_entries_dec(h);
        base_set_dirty(h);
}
#define remove_entry(h, idx) base_remove_entry(HASHMAP_BASE(h), idx)

സ്ഥിതം unsigned hashmap_iterate_in_insertion_order(OrderedHashmap *h, Iterator *i) {
        ഘടന ordered_hashmap_entry *e;
        unsigned idx;

        പ്രതിജ്ഞാ(h);
        പ്രതിജ്ഞാ(i);

        എങ്കിൽ (i->idx == IDX_NIL)
                പോകുക at_end;

        എങ്കിൽ (i->idx == IDX_FIRST && h->iterate_list_head == IDX_NIL)
                പോകുക at_end;

        എങ്കിൽ (i->idx == IDX_FIRST) {
                idx = h->iterate_list_head;
                e = ordered_bucket_at(h, idx);
        } അല്ലെങ്കിൽ {
                idx = i->idx;
                e = ordered_bucket_at(h, idx);
                /*
                 * We allow removing the current entry while iterating, but removal may cause
                 * a backward shift. The next entry may thus move one bucket to the left.
                 * To detect when it happens, we remember the key pointer of the entry we were
                 * going to iterate next. If it does not match, there was a backward shift.
                 */
                എങ്കിൽ (e->p.b.താക്കോൽ != i->next_താക്കോൽ) {
                        idx = prev_idx(HASHMAP_BASE(h), idx);
                        e = ordered_bucket_at(h, idx);
                }
                പ്രതിജ്ഞാ(e->p.b.താക്കോൽ == i->next_താക്കോൽ);
        }

#എങ്കിൽ ENABLE_DEBUG_HASHMAP
        i->prev_idx = idx;
#endif

        എങ്കിൽ (e->iterate_next != IDX_NIL) {
                ഘടന ordered_hashmap_entry *n;
                i->idx = e->iterate_next;
                n = ordered_bucket_at(h, i->idx);
                i->next_താക്കോൽ = n->p.b.താക്കോൽ;
        } അല്ലെങ്കിൽ
                i->idx = IDX_NIL;

        തിരികെ idx;

at_end:
        i->idx = IDX_NIL;
        തിരികെ IDX_NIL;
}

സ്ഥിതം unsigned hashmap_iterate_in_internal_order(HashmapBase *h, Iterator *i) {
        unsigned idx;

        പ്രതിജ്ഞാ(h);
        പ്രതിജ്ഞാ(i);

        എങ്കിൽ (i->idx == IDX_NIL)
                പോകുക at_end;

        എങ്കിൽ (i->idx == IDX_FIRST) {
                /* fast forward to the first occupied bucket */
                എങ്കിൽ (h->has_indirect) {
                        i->idx = skip_free_buckets(h, h->indirect.idx_lowest_entry);
                        h->indirect.idx_lowest_entry = i->idx;
                } അല്ലെങ്കിൽ
                        i->idx = skip_free_buckets(h, 0);

                എങ്കിൽ (i->idx == IDX_NIL)
                        പോകുക at_end;
        } അല്ലെങ്കിൽ {
                ഘടന hashmap_base_entry *e;

                പ്രതിജ്ഞാ(i->idx > 0);

                e = bucket_at(h, i->idx);
                /*
                 * We allow removing the current entry while iterating, but removal may cause
                 * a backward shift. The next entry may thus move one bucket to the left.
                 * To detect when it happens, we remember the key pointer of the entry we were
                 * going to iterate next. If it does not match, there was a backward shift.
                 */
                എങ്കിൽ (e->താക്കോൽ != i->next_താക്കോൽ)
                        e = bucket_at(h, --i->idx);

                പ്രതിജ്ഞാ(e->താക്കോൽ == i->next_താക്കോൽ);
        }

        idx = i->idx;
#എങ്കിൽ ENABLE_DEBUG_HASHMAP
        i->prev_idx = idx;
#endif

        i->idx = skip_free_buckets(h, i->idx + 1);
        എങ്കിൽ (i->idx != IDX_NIL)
                i->next_താക്കോൽ = bucket_at(h, i->idx)->താക്കോൽ;
        അല്ലെങ്കിൽ
                i->idx = IDX_NIL;

        തിരികെ idx;

at_end:
        i->idx = IDX_NIL;
        തിരികെ IDX_NIL;
}

സ്ഥിതം unsigned hashmap_iterate_entry(HashmapBase *h, Iterator *i) {
        എങ്കിൽ (!h) {
                i->idx = IDX_NIL;
                തിരികെ IDX_NIL;
        }

#എങ്കിൽ ENABLE_DEBUG_HASHMAP
        എങ്കിൽ (i->idx == IDX_FIRST) {
                i->put_എണ്ണം = h->debug.put_എണ്ണം;
                i->rem_എണ്ണം = h->debug.rem_എണ്ണം;
        } അല്ലെങ്കിൽ {
                /* While iterating, must not add any new entries */
                പ്രതിജ്ഞാ(i->put_എണ്ണം == h->debug.put_എണ്ണം);
                /* ... or remove entries other than the current one */
                പ്രതിജ്ഞാ(i->rem_എണ്ണം == h->debug.rem_എണ്ണം ||
                       (i->rem_എണ്ണം == h->debug.rem_എണ്ണം - 1 &&
                        i->prev_idx == h->debug.last_rem_idx));
                /* Reset our removals counter */
                i->rem_എണ്ണം = h->debug.rem_എണ്ണം;
        }
#endif

        തിരികെ h->type == HASHMAP_TYPE_ORDERED ? hashmap_iterate_in_insertion_order((OrderedHashmap*) h, i)
                                               : hashmap_iterate_in_internal_order(h, i);
}

bool _hashmap_iterate(HashmapBase *h, Iterator *i, ശൂന്യം **മൂല്യം, സ്ഥിരം ശൂന്യം **താക്കോൽ) {
        ഘടന hashmap_base_entry *e;
        ശൂന്യം *data;
        unsigned idx;

        idx = hashmap_iterate_entry(h, i);
        എങ്കിൽ (idx == IDX_NIL) {
                എങ്കിൽ (മൂല്യം)
                        *മൂല്യം = ശൂന്യ;
                എങ്കിൽ (താക്കോൽ)
                        *താക്കോൽ = ശൂന്യ;

                തിരികെ false;
        }

        e = bucket_at(h, idx);
        data = entry_മൂല്യം(h, e);
        എങ്കിൽ (മൂല്യം)
                *മൂല്യം = data;
        എങ്കിൽ (താക്കോൽ)
                *താക്കോൽ = e->താക്കോൽ;

        തിരികെ true;
}

#define HASHMAP_FOREACH_IDX(idx, h, i) \
        വേണ്ടി ((i) = ITERATOR_FIRST, (idx) = hashmap_iterate_entry((h), &(i)); \
             (idx != IDX_NIL); \
             (idx) = hashmap_iterate_entry((h), &(i)))

IteratedCache* _hashmap_iterated_cache_new(HashmapBase *h) {
        IteratedCache *cache;

        പ്രതിജ്ഞാ(h);
        പ്രതിജ്ഞാ(!h->cached);

        എങ്കിൽ (h->cached)
                തിരികെ ശൂന്യ;

        cache = new0(IteratedCache, 1);
        എങ്കിൽ (!cache)
                തിരികെ ശൂന്യ;

        cache->hashmap = h;
        h->cached = true;

        തിരികെ cache;
}

സ്ഥിതം ശൂന്യം reset_direct_storage(HashmapBase *h) {
        സ്ഥിരം ഘടന hashmap_type_info *hi = &hashmap_type_info[h->type];
        ശൂന്യം *p;

        പ്രതിജ്ഞാ(!h->has_indirect);

        p = mempset(h->direct.storage, 0, hi->entry_size * hi->n_direct_buckets);
        memset(p, DIB_RAW_INIT, വലുപ്പം(dib_raw_t) * hi->n_direct_buckets);
}

സ്ഥിതം ശൂന്യം shared_hash_താക്കോൽ_initialize(ശൂന്യം) {
        random_bytes(shared_hash_താക്കോൽ, വലുപ്പം(shared_hash_താക്കോൽ));
}

സ്ഥിതം ഘടന HashmapBase* hashmap_base_new(സ്ഥിരം ഘടന hash_ops *hash_ops, എണ്ണൽ HashmapType type) {
        HashmapBase *h;
        സ്ഥിരം ഘടന hashmap_type_info *hi = &hashmap_type_info[type];

        bool use_pool = mempool_enabled && mempool_enabled();  /* mempool_enabled is a weak symbol */

        h = use_pool ? mempool_alloc0_tile(hi->mempool) : malloc0(hi->head_size);
        എങ്കിൽ (!h)
                തിരികെ ശൂന്യ;

        h->type = type;
        h->from_pool = use_pool;
        h->hash_ops = hash_ops ?: &trivial_hash_ops;

        എങ്കിൽ (type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*)h;
                lh->iterate_list_head = lh->iterate_list_tail = IDX_NIL;
        }

        reset_direct_storage(h);

        സ്ഥിതം pthread_once_t once = PTHREAD_ONCE_INIT;
        assert_se(pthread_once(&once, shared_hash_താക്കോൽ_initialize) == 0);

#എങ്കിൽ ENABLE_DEBUG_HASHMAP
        assert_se(pthread_mutex_lock(&hashmap_debug_list_mutex) == 0);
        LIST_PREPEND(debug_list, hashmap_debug_list, &h->debug);
        assert_se(pthread_mutex_unlock(&hashmap_debug_list_mutex) == 0);
#endif

        തിരികെ h;
}

Hashmap *hashmap_new(സ്ഥിരം ഘടന hash_ops *hash_ops) {
        തിരികെ (Hashmap*)        hashmap_base_new(hash_ops, HASHMAP_TYPE_PLAIN);
}

OrderedHashmap *ordered_hashmap_new(സ്ഥിരം ഘടന hash_ops *hash_ops) {
        തിരികെ (OrderedHashmap*) hashmap_base_new(hash_ops, HASHMAP_TYPE_ORDERED);
}

Set *set_new(സ്ഥിരം ഘടന hash_ops *hash_ops) {
        തിരികെ (Set*)            hashmap_base_new(hash_ops, HASHMAP_TYPE_SET);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ hashmap_base_ensure_allocated(HashmapBase **h, സ്ഥിരം ഘടന hash_ops *hash_ops,
                                         എണ്ണൽ HashmapType type) {
        HashmapBase *q;

        പ്രതിജ്ഞാ(h);

        എങ്കിൽ (*h) {
                പ്രതിജ്ഞാ((*h)->hash_ops == (hash_ops ?: &trivial_hash_ops));
                തിരികെ 0;
        }

        q = hashmap_base_new(hash_ops, type);
        എങ്കിൽ (!q)
                തിരികെ -ENOMEM;

        *h = q;
        തിരികെ 1;
}

പൂർണ്ണസംഖ്യ hashmap_ensure_allocated(Hashmap **h, സ്ഥിരം ഘടന hash_ops *hash_ops) {
        തിരികെ hashmap_base_ensure_allocated((HashmapBase**)h, hash_ops, HASHMAP_TYPE_PLAIN);
}

പൂർണ്ണസംഖ്യ ordered_hashmap_ensure_allocated(OrderedHashmap **h, സ്ഥിരം ഘടന hash_ops *hash_ops) {
        തിരികെ hashmap_base_ensure_allocated((HashmapBase**)h, hash_ops, HASHMAP_TYPE_ORDERED);
}

പൂർണ്ണസംഖ്യ set_ensure_allocated(Set **s, സ്ഥിരം ഘടന hash_ops *hash_ops) {
        തിരികെ hashmap_base_ensure_allocated((HashmapBase**)s, hash_ops, HASHMAP_TYPE_SET);
}

പൂർണ്ണസംഖ്യ hashmap_ensure_put(Hashmap **h, സ്ഥിരം ഘടന hash_ops *hash_ops, സ്ഥിരം ശൂന്യം *താക്കോൽ, ശൂന്യം *മൂല്യം) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(h);

        r = hashmap_ensure_allocated(h, hash_ops);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ hashmap_put(*h, താക്കോൽ, മൂല്യം);
}

പൂർണ്ണസംഖ്യ ordered_hashmap_ensure_put(OrderedHashmap **h, സ്ഥിരം ഘടന hash_ops *hash_ops, സ്ഥിരം ശൂന്യം *താക്കോൽ, ശൂന്യം *മൂല്യം) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(h);

        r = ordered_hashmap_ensure_allocated(h, hash_ops);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ ordered_hashmap_put(*h, താക്കോൽ, മൂല്യം);
}

പൂർണ്ണസംഖ്യ ordered_hashmap_ensure_replace(OrderedHashmap **h, സ്ഥിരം ഘടന hash_ops *hash_ops, സ്ഥിരം ശൂന്യം *താക്കോൽ, ശൂന്യം *മൂല്യം) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(h);

        r = ordered_hashmap_ensure_allocated(h, hash_ops);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ ordered_hashmap_replace(*h, താക്കോൽ, മൂല്യം);
}

പൂർണ്ണസംഖ്യ hashmap_ensure_replace(Hashmap **h, സ്ഥിരം ഘടന hash_ops *hash_ops, സ്ഥിരം ശൂന്യം *താക്കോൽ, ശൂന്യം *മൂല്യം) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(h);

        r = hashmap_ensure_allocated(h, hash_ops);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ hashmap_replace(*h, താക്കോൽ, മൂല്യം);
}

സ്ഥിതം ശൂന്യം hashmap_free_no_clear(HashmapBase *h) {
        പ്രതിജ്ഞാ(!h->has_indirect);
        പ്രതിജ്ഞാ(h->n_direct_entries == 0);

#എങ്കിൽ ENABLE_DEBUG_HASHMAP
        assert_se(pthread_mutex_lock(&hashmap_debug_list_mutex) == 0);
        LIST_REMOVE(debug_list, hashmap_debug_list, &h->debug);
        assert_se(pthread_mutex_unlock(&hashmap_debug_list_mutex) == 0);
#endif

        എങ്കിൽ (h->from_pool) {
                /* Ensure that the object didn't get migrated between threads. */
                assert_se(is_main_thread());
                mempool_free_tile(hashmap_type_info[h->type].mempool, h);
        } അല്ലെങ്കിൽ
                സ്വതന്ത്രം(h);
}

HashmapBase* _hashmap_free(HashmapBase *h) {
        എങ്കിൽ (h) {
                _hashmap_clear(h);
                hashmap_free_no_clear(h);
        }

        തിരികെ ശൂന്യ;
}

ശൂന്യം _hashmap_clear(HashmapBase *h) {
        എങ്കിൽ (!h)
                തിരികെ;

        എങ്കിൽ (h->hash_ops->free_താക്കോൽ || h->hash_ops->free_മൂല്യം) {

                /* If destructor calls are defined, let's destroy things defensively: let's take the item out of the
                 * hash table, and only then call the destructor functions. If these destructors then try to unregister
                 * themselves from our hash table a second time, the entry is already gone. */

                ആകുമ്പോൾ (_hashmap_size(h) > 0) {
                        ശൂന്യം *k = ശൂന്യ;
                        ശൂന്യം *v;

                        v = hashmap_first_താക്കോൽ_and_മൂല്യം(h, true, &k);

                        എങ്കിൽ (h->hash_ops->free_താക്കോൽ)
                                h->hash_ops->free_താക്കോൽ(k);

                        എങ്കിൽ (h->hash_ops->free_മൂല്യം)
                                h->hash_ops->free_മൂല്യം(v);
                }
        }

        എങ്കിൽ (h->has_indirect) {
                സ്വതന്ത്രം(h->indirect.storage);
                h->has_indirect = false;
        }

        h->n_direct_entries = 0;
        reset_direct_storage(h);

        എങ്കിൽ (h->type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*) h;
                lh->iterate_list_head = lh->iterate_list_tail = IDX_NIL;
        }

        base_set_dirty(h);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ resize_buckets(HashmapBase *h, unsigned entries_add);

/*
 * Finds an empty bucket to put an entry into, starting the scan at 'idx'.
 * Performs Robin Hood swaps as it goes. The entry to put must be placed
 * by the caller into swap slot IDX_PUT.
 * If used for in-place resizing, may leave a displaced entry in swap slot
 * IDX_PUT. Caller must rehash it next.
 * Returns: true if it left a displaced entry to rehash next in IDX_PUT,
 *          false otherwise.
 */
സ്ഥിതം bool hashmap_put_robin_hood(HashmapBase *h, unsigned idx,
                                   ഘടന swap_entries *swap) {
        dib_raw_t raw_dib, *dibs;
        unsigned dib, distance;

#എങ്കിൽ ENABLE_DEBUG_HASHMAP
        പ്രതിജ്ഞാ(h);
        h->debug.put_എണ്ണം++;
#endif

        dibs = dib_raw_ptr(h);

        വേണ്ടി (distance = 0; ; distance++) {
                raw_dib = dibs[idx];
                എങ്കിൽ (IN_SET(raw_dib, DIB_RAW_FREE, DIB_RAW_REHASH)) {
                        എങ്കിൽ (raw_dib == DIB_RAW_REHASH)
                                bucket_move_entry(h, swap, idx, IDX_TMP);

                        എങ്കിൽ (h->has_indirect && h->indirect.idx_lowest_entry > idx)
                                h->indirect.idx_lowest_entry = idx;

                        bucket_set_dib(h, idx, distance);
                        bucket_move_entry(h, swap, IDX_PUT, idx);
                        എങ്കിൽ (raw_dib == DIB_RAW_REHASH) {
                                bucket_move_entry(h, swap, IDX_TMP, IDX_PUT);
                                തിരികെ true;
                        }

                        തിരികെ false;
                }

                dib = bucket_calculate_dib(h, idx, raw_dib);

                എങ്കിൽ (dib < distance) {
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
സ്ഥിതം പൂർണ്ണസംഖ്യ hashmap_base_put_boldly(HashmapBase *h, unsigned idx,
                                   ഘടന swap_entries *swap, bool may_resize) {
        ഘടന ordered_hashmap_entry *new_entry;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(idx < n_buckets(h));

        new_entry = bucket_at_swap(swap, IDX_PUT);

        എങ്കിൽ (may_resize) {
                r = resize_buckets(h, 1);
                എങ്കിൽ (r < 0)
                        തിരികെ r;
                എങ്കിൽ (r > 0)
                        idx = bucket_hash(h, new_entry->p.b.താക്കോൽ);
        }
        പ്രതിജ്ഞാ(n_entries(h) < n_buckets(h));

        എങ്കിൽ (h->type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*) h;

                new_entry->iterate_next = IDX_NIL;
                new_entry->iterate_previous = lh->iterate_list_tail;

                എങ്കിൽ (lh->iterate_list_tail != IDX_NIL) {
                        ഘടന ordered_hashmap_entry *old_tail;

                        old_tail = ordered_bucket_at(lh, lh->iterate_list_tail);
                        പ്രതിജ്ഞാ(old_tail->iterate_next == IDX_NIL);
                        old_tail->iterate_next = IDX_PUT;
                }

                lh->iterate_list_tail = IDX_PUT;
                എങ്കിൽ (lh->iterate_list_head == IDX_NIL)
                        lh->iterate_list_head = IDX_PUT;
        }

        assert_se(hashmap_put_robin_hood(h, idx, swap) == false);

        n_entries_inc(h);
#എങ്കിൽ ENABLE_DEBUG_HASHMAP
        h->debug.max_entries = MAX(h->debug.max_entries, n_entries(h));
#endif

        base_set_dirty(h);

        തിരികെ 1;
}
#define hashmap_put_boldly(h, idx, swap, may_resize) \
        hashmap_base_put_boldly(HASHMAP_BASE(h), idx, swap, may_resize)

/*
 * Returns 0 if resize is not needed.
 *         1 if successfully resized.
 *         -ENOMEM on allocation failure.
 */
സ്ഥിതം പൂർണ്ണസംഖ്യ resize_buckets(HashmapBase *h, unsigned entries_add) {
        ഘടന swap_entries swap;
        ശൂന്യം *new_storage;
        dib_raw_t *old_dibs, *new_dibs;
        സ്ഥിരം ഘടന hashmap_type_info *hi;
        unsigned idx, optimal_idx;
        unsigned old_n_buckets, new_n_buckets, n_rehashed, new_n_entries;
        uint8_t new_shift;
        bool rehash_next;

        പ്രതിജ്ഞാ(h);

        hi = &hashmap_type_info[h->type];
        new_n_entries = n_entries(h) + entries_add;

        /* overflow? */
        എങ്കിൽ (_unlikely_(new_n_entries < entries_add))
                തിരികെ -ENOMEM;

        /* For direct storage we allow 100% load, because it's tiny. */
        എങ്കിൽ (!h->has_indirect && new_n_entries <= hi->n_direct_buckets)
                തിരികെ 0;

        /*
         * Load factor = n/m = 1 - (1/INV_KEEP_FREE).
         * From it follows: m = n + n/(INV_KEEP_FREE - 1)
         */
        new_n_buckets = new_n_entries + new_n_entries / (INV_KEEP_FREE - 1);
        /* overflow? */
        എങ്കിൽ (_unlikely_(new_n_buckets < new_n_entries))
                തിരികെ -ENOMEM;

        old_n_buckets = n_buckets(h);

        എങ്കിൽ (_likely_(new_n_buckets <= old_n_buckets))
                തിരികെ 0;

        എങ്കിൽ (_unlikely_(new_n_buckets > UINT_MAX / (hi->entry_size + വലുപ്പം(dib_raw_t))))
                തിരികെ -ENOMEM;

        new_shift = log2u_round_up(MAX(
                        new_n_buckets * (hi->entry_size + വലുപ്പം(dib_raw_t)),
                        2 * വലുപ്പം(ഘടന direct_storage)));

        /* Realloc storage (buckets and DIB array). */
        new_storage = realloc(h->has_indirect ? h->indirect.storage : ശൂന്യ,
                              1U << new_shift);
        എങ്കിൽ (!new_storage)
                തിരികെ -ENOMEM;

        /* Must upgrade direct to indirect storage. */
        എങ്കിൽ (!h->has_indirect) {
                memcpy(new_storage, h->direct.storage,
                       old_n_buckets * (hi->entry_size + വലുപ്പം(dib_raw_t)));
                h->indirect.n_entries = h->n_direct_entries;
                h->indirect.idx_lowest_entry = 0;
                h->n_direct_entries = 0;
        }

        /* Get a new hash key. If we've just upgraded to indirect storage,
         * allow reusing a previously generated key. It's still a different key
         * from the shared one that we used for direct storage. */
        get_hash_താക്കോൽ(h->indirect.hash_താക്കോൽ, !h->has_indirect);

        h->has_indirect = true;
        h->indirect.storage = new_storage;
        h->indirect.n_buckets = (1U << new_shift) /
                                (hi->entry_size + വലുപ്പം(dib_raw_t));

        old_dibs = (dib_raw_t*)((uint8_t*) new_storage + hi->entry_size * old_n_buckets);
        new_dibs = dib_raw_ptr(h);

        /*
         * Move the DIB array to the new place, replacing valid DIB values with
         * DIB_RAW_REHASH to indicate all of the used buckets need rehashing.
         * Note: Overlap is not possible, because we have at least doubled the
         * number of buckets and dib_raw_t is smaller than any entry type.
         */
        വേണ്ടി (idx = 0; idx < old_n_buckets; idx++) {
                പ്രതിജ്ഞാ(old_dibs[idx] != DIB_RAW_REHASH);
                new_dibs[idx] = old_dibs[idx] == DIB_RAW_FREE ? DIB_RAW_FREE
                                                              : DIB_RAW_REHASH;
        }

        /* Zero the area of newly added entries (including the old DIB area) */
        memzero(bucket_at(h, old_n_buckets),
               (n_buckets(h) - old_n_buckets) * hi->entry_size);

        /* The upper half of the new DIB array needs initialization */
        memset(&new_dibs[old_n_buckets], DIB_RAW_INIT,
               (n_buckets(h) - old_n_buckets) * വലുപ്പം(dib_raw_t));

        /* Rehash entries that need it */
        n_rehashed = 0;
        വേണ്ടി (idx = 0; idx < old_n_buckets; idx++) {
                എങ്കിൽ (new_dibs[idx] != DIB_RAW_REHASH)
                        തുടരുക;

                optimal_idx = bucket_hash(h, bucket_at(h, idx)->താക്കോൽ);

                /*
                 * Not much to do if by luck the entry hashes to its current
                 * location. Just set its DIB.
                 */
                എങ്കിൽ (optimal_idx == idx) {
                        new_dibs[idx] = 0;
                        n_rehashed++;
                        തുടരുക;
                }

                new_dibs[idx] = DIB_RAW_FREE;
                bucket_move_entry(h, &swap, idx, IDX_PUT);
                /* bucket_move_entry does not clear the source */
                memzero(bucket_at(h, idx), hi->entry_size);

                ചെയ്യുക {
                        /*
                         * Find the new bucket for the current entry. This may make
                         * another entry homeless and load it into IDX_PUT.
                         */
                        rehash_next = hashmap_put_robin_hood(h, optimal_idx, &swap);
                        n_rehashed++;

                        /* Did the current entry displace another one? */
                        എങ്കിൽ (rehash_next)
                                optimal_idx = bucket_hash(h, bucket_at_swap(&swap, IDX_PUT)->p.b.താക്കോൽ);
                } ആകുമ്പോൾ (rehash_next);
        }

        assert_se(n_rehashed == n_entries(h));

        തിരികെ 1;
}

/*
 * Finds an entry with a matching key
 * Returns: index of the found entry, or IDX_NIL if not found.
 */
സ്ഥിതം unsigned base_bucket_scan(HashmapBase *h, unsigned idx, സ്ഥിരം ശൂന്യം *താക്കോൽ) {
        compare_func_t compare = h->hash_ops->compare;
        ഘടന hashmap_base_entry *e;
        unsigned dib, distance;
        dib_raw_t *dibs = dib_raw_ptr(h);

        പ്രതിജ്ഞാ(idx < n_buckets(h));

        വേണ്ടി (distance = 0; ; distance++) {
                എങ്കിൽ (dibs[idx] == DIB_RAW_FREE)
                        തിരികെ IDX_NIL;

                dib = bucket_calculate_dib(h, idx, dibs[idx]);

                എങ്കിൽ (dib < distance)
                        തിരികെ IDX_NIL;
                എങ്കിൽ (dib == distance) {
                        e = bucket_at(h, idx);

                        /* Most maps compare keys by pointer. Do that here, without the indirect call. */
                        എങ്കിൽ (compare == trivial_compare_func ? e->താക്കോൽ == താക്കോൽ : compare(e->താക്കോൽ, താക്കോൽ) == 0)
                                തിരികെ idx;
                }

                idx = next_idx(h, idx);
        }
}
#define bucket_scan(h, idx, താക്കോൽ) base_bucket_scan(HASHMAP_BASE(h), idx, താക്കോൽ)

പൂർണ്ണസംഖ്യ hashmap_put(Hashmap *h, സ്ഥിരം ശൂന്യം *താക്കോൽ, ശൂന്യം *മൂല്യം) {
        ഘടന swap_entries swap;
        ഘടന plain_hashmap_entry *e;
        unsigned hash, idx;

        പ്രതിജ്ഞാ(h);

        hash = bucket_hash(h, താക്കോൽ);
        idx = bucket_scan(h, hash, താക്കോൽ);
        എങ്കിൽ (idx != IDX_NIL) {
                e = plain_bucket_at(h, idx);
                എങ്കിൽ (e->മൂല്യം == മൂല്യം)
                        തിരികെ 0;
                തിരികെ -EEXIST;
        }

        e = &bucket_at_swap(&swap, IDX_PUT)->p;
        e->b.താക്കോൽ = താക്കോൽ;
        e->മൂല്യം = മൂല്യം;
        തിരികെ hashmap_put_boldly(h, hash, &swap, true);
}

പൂർണ്ണസംഖ്യ set_put(Set *s, സ്ഥിരം ശൂന്യം *താക്കോൽ) {
        ഘടന swap_entries swap;
        ഘടന hashmap_base_entry *e;
        unsigned hash, idx;

        പ്രതിജ്ഞാ(s);

        hash = bucket_hash(s, താക്കോൽ);
        idx = bucket_scan(s, hash, താക്കോൽ);
        എങ്കിൽ (idx != IDX_NIL)
                തിരികെ 0;

        e = &bucket_at_swap(&swap, IDX_PUT)->p.b;
        e->താക്കോൽ = താക്കോൽ;
        തിരികെ hashmap_put_boldly(s, hash, &swap, true);
}

പൂർണ്ണസംഖ്യ set_ensure_put(Set **s, സ്ഥിരം ഘടന hash_ops *hash_ops, സ്ഥിരം ശൂന്യം *താക്കോൽ) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(s);

        r = set_ensure_allocated(s, hash_ops);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ set_put(*s, താക്കോൽ);
}

പൂർണ്ണസംഖ്യ set_ensure_consume(Set **s, സ്ഥിരം ഘടന hash_ops *hash_ops, ശൂന്യം *താക്കോൽ) {
        പൂർണ്ണസംഖ്യ r;

        r = set_ensure_put(s, hash_ops, താക്കോൽ);
        എങ്കിൽ (r <= 0) {
                എങ്കിൽ (hash_ops && hash_ops->free_താക്കോൽ)
                        hash_ops->free_താക്കോൽ(താക്കോൽ);
                അല്ലെങ്കിൽ എങ്കിൽ (hash_ops && hash_ops->free_മൂല്യം)
                        /* Sets store their element in the key slot but may carry a value destructor. */
                        hash_ops->free_മൂല്യം(താക്കോൽ);
                അല്ലെങ്കിൽ
                        സ്വതന്ത്രം(താക്കോൽ);
        }

        തിരികെ r;
}

പൂർണ്ണസംഖ്യ hashmap_replace(Hashmap *h, സ്ഥിരം ശൂന്യം *താക്കോൽ, ശൂന്യം *മൂല്യം) {
        ഘടന swap_entries swap;
        ഘടന plain_hashmap_entry *e;
        unsigned hash, idx;

        പ്രതിജ്ഞാ(h);

        hash = bucket_hash(h, താക്കോൽ);
        idx = bucket_scan(h, hash, താക്കോൽ);
        എങ്കിൽ (idx != IDX_NIL) {
                e = plain_bucket_at(h, idx);
#എങ്കിൽ ENABLE_DEBUG_HASHMAP
                /* Although the key is equal, the key pointer may have changed,
                 * and this would break our assumption for iterating. So count
                 * this operation as incompatible with iteration. */
                എങ്കിൽ (e->b.താക്കോൽ != താക്കോൽ) {
                        h->b.debug.put_എണ്ണം++;
                        h->b.debug.rem_എണ്ണം++;
                        h->b.debug.last_rem_idx = idx;
                }
#endif
                e->b.താക്കോൽ = താക്കോൽ;
                e->മൂല്യം = മൂല്യം;
                hashmap_set_dirty(h);

                തിരികെ 0;
        }

        e = &bucket_at_swap(&swap, IDX_PUT)->p;
        e->b.താക്കോൽ = താക്കോൽ;
        e->മൂല്യം = മൂല്യം;
        തിരികെ hashmap_put_boldly(h, hash, &swap, true);
}

പൂർണ്ണസംഖ്യ hashmap_update(Hashmap *h, സ്ഥിരം ശൂന്യം *താക്കോൽ, ശൂന്യം *മൂല്യം) {
        ഘടന plain_hashmap_entry *e;
        unsigned hash, idx;

        പ്രതിജ്ഞാ(h);

        hash = bucket_hash(h, താക്കോൽ);
        idx = bucket_scan(h, hash, താക്കോൽ);
        എങ്കിൽ (idx == IDX_NIL)
                തിരികെ -ENOENT;

        e = plain_bucket_at(h, idx);
        e->മൂല്യം = മൂല്യം;
        hashmap_set_dirty(h);

        തിരികെ 0;
}

ശൂന്യം* _hashmap_get(HashmapBase *h, സ്ഥിരം ശൂന്യം *താക്കോൽ) {
        ഘടന hashmap_base_entry *e;
        unsigned hash, idx;

        എങ്കിൽ (!h)
                തിരികെ ശൂന്യ;

        hash = bucket_hash(h, താക്കോൽ);
        idx = bucket_scan(h, hash, താക്കോൽ);
        എങ്കിൽ (idx == IDX_NIL)
                തിരികെ ശൂന്യ;

        e = bucket_at(h, idx);
        തിരികെ entry_മൂല്യം(h, e);
}

ശൂന്യം* hashmap_get2(Hashmap *h, സ്ഥിരം ശൂന്യം *താക്കോൽ, ശൂന്യം **ret) {
        ഘടന plain_hashmap_entry *e;
        unsigned hash, idx;

        എങ്കിൽ (!h)
                തിരികെ ശൂന്യ;

        hash = bucket_hash(h, താക്കോൽ);
        idx = bucket_scan(h, hash, താക്കോൽ);
        എങ്കിൽ (idx == IDX_NIL)
                തിരികെ ശൂന്യ;

        e = plain_bucket_at(h, idx);
        എങ്കിൽ (ret)
                *ret = (ശൂന്യം*) e->b.താക്കോൽ;

        തിരികെ e->മൂല്യം;
}

bool _hashmap_contains(HashmapBase *h, സ്ഥിരം ശൂന്യം *താക്കോൽ) {
        unsigned hash;

        എങ്കിൽ (!h)
                തിരികെ false;

        hash = bucket_hash(h, താക്കോൽ);
        തിരികെ bucket_scan(h, hash, താക്കോൽ) != IDX_NIL;
}

ശൂന്യം* _hashmap_remove(HashmapBase *h, സ്ഥിരം ശൂന്യം *താക്കോൽ) {
        ഘടന hashmap_base_entry *e;
        unsigned hash, idx;
        ശൂന്യം *data;

        എങ്കിൽ (!h)
                തിരികെ ശൂന്യ;

        hash = bucket_hash(h, താക്കോൽ);
        idx = bucket_scan(h, hash, താക്കോൽ);
        എങ്കിൽ (idx == IDX_NIL)
                തിരികെ ശൂന്യ;

        e = bucket_at(h, idx);
        data = entry_മൂല്യം(h, e);
        remove_entry(h, idx);

        തിരികെ data;
}

ശൂന്യം* hashmap_remove2(Hashmap *h, സ്ഥിരം ശൂന്യം *താക്കോൽ, ശൂന്യം **ret) {
        ഘടന plain_hashmap_entry *e;
        unsigned hash, idx;
        ശൂന്യം *data;

        എങ്കിൽ (!h) {
                എങ്കിൽ (ret)
                        *ret = ശൂന്യ;
                തിരികെ ശൂന്യ;
        }

        hash = bucket_hash(h, താക്കോൽ);
        idx = bucket_scan(h, hash, താക്കോൽ);
        എങ്കിൽ (idx == IDX_NIL) {
                എങ്കിൽ (ret)
                        *ret = ശൂന്യ;
                തിരികെ ശൂന്യ;
        }

        e = plain_bucket_at(h, idx);
        data = e->മൂല്യം;
        എങ്കിൽ (ret)
                *ret = (ശൂന്യം*) e->b.താക്കോൽ;

        remove_entry(h, idx);

        തിരികെ data;
}

പൂർണ്ണസംഖ്യ hashmap_remove_and_put(Hashmap *h, സ്ഥിരം ശൂന്യം *old_താക്കോൽ, സ്ഥിരം ശൂന്യം *new_താക്കോൽ, ശൂന്യം *മൂല്യം) {
        ഘടന swap_entries swap;
        ഘടന plain_hashmap_entry *e;
        unsigned old_hash, new_hash, idx;

        എങ്കിൽ (!h)
                തിരികെ -ENOENT;

        old_hash = bucket_hash(h, old_താക്കോൽ);
        idx = bucket_scan(h, old_hash, old_താക്കോൽ);
        എങ്കിൽ (idx == IDX_NIL)
                തിരികെ -ENOENT;

        new_hash = bucket_hash(h, new_താക്കോൽ);
        എങ്കിൽ (bucket_scan(h, new_hash, new_താക്കോൽ) != IDX_NIL)
                തിരികെ -EEXIST;

        remove_entry(h, idx);

        e = &bucket_at_swap(&swap, IDX_PUT)->p;
        e->b.താക്കോൽ = new_താക്കോൽ;
        e->മൂല്യം = മൂല്യം;
        assert_se(hashmap_put_boldly(h, new_hash, &swap, false) == 1);

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ set_remove_and_put(Set *s, സ്ഥിരം ശൂന്യം *old_താക്കോൽ, സ്ഥിരം ശൂന്യം *new_താക്കോൽ) {
        ഘടന swap_entries swap;
        ഘടന hashmap_base_entry *e;
        unsigned old_hash, new_hash, idx;

        എങ്കിൽ (!s)
                തിരികെ -ENOENT;

        old_hash = bucket_hash(s, old_താക്കോൽ);
        idx = bucket_scan(s, old_hash, old_താക്കോൽ);
        എങ്കിൽ (idx == IDX_NIL)
                തിരികെ -ENOENT;

        new_hash = bucket_hash(s, new_താക്കോൽ);
        എങ്കിൽ (bucket_scan(s, new_hash, new_താക്കോൽ) != IDX_NIL)
                തിരികെ -EEXIST;

        remove_entry(s, idx);

        e = &bucket_at_swap(&swap, IDX_PUT)->p.b;
        e->താക്കോൽ = new_താക്കോൽ;
        assert_se(hashmap_put_boldly(s, new_hash, &swap, false) == 1);

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ hashmap_remove_and_replace(Hashmap *h, സ്ഥിരം ശൂന്യം *old_താക്കോൽ, സ്ഥിരം ശൂന്യം *new_താക്കോൽ, ശൂന്യം *മൂല്യം) {
        ഘടന swap_entries swap;
        ഘടന plain_hashmap_entry *e;
        unsigned old_hash, new_hash, idx_old, idx_new;

        എങ്കിൽ (!h)
                തിരികെ -ENOENT;

        old_hash = bucket_hash(h, old_താക്കോൽ);
        idx_old = bucket_scan(h, old_hash, old_താക്കോൽ);
        എങ്കിൽ (idx_old == IDX_NIL)
                തിരികെ -ENOENT;

        old_താക്കോൽ = bucket_at(HASHMAP_BASE(h), idx_old)->താക്കോൽ;

        new_hash = bucket_hash(h, new_താക്കോൽ);
        idx_new = bucket_scan(h, new_hash, new_താക്കോൽ);
        എങ്കിൽ (idx_new != IDX_NIL)
                എങ്കിൽ (idx_old != idx_new) {
                        remove_entry(h, idx_new);
                        /* Compensate for a possible backward shift. */
                        എങ്കിൽ (old_താക്കോൽ != bucket_at(HASHMAP_BASE(h), idx_old)->താക്കോൽ)
                                idx_old = prev_idx(HASHMAP_BASE(h), idx_old);
                        പ്രതിജ്ഞാ(old_താക്കോൽ == bucket_at(HASHMAP_BASE(h), idx_old)->താക്കോൽ);
                }

        remove_entry(h, idx_old);

        e = &bucket_at_swap(&swap, IDX_PUT)->p;
        e->b.താക്കോൽ = new_താക്കോൽ;
        e->മൂല്യം = മൂല്യം;
        assert_se(hashmap_put_boldly(h, new_hash, &swap, false) == 1);

        തിരികെ 0;
}

ശൂന്യം* hashmap_remove_മൂല്യം(HashmapBase *h, സ്ഥിരം ശൂന്യം *താക്കോൽ, ശൂന്യം *മൂല്യം) {
        ഘടന hashmap_base_entry *e;
        unsigned hash, idx;

        എങ്കിൽ (!h)
                തിരികെ ശൂന്യ;

        hash = bucket_hash(h, താക്കോൽ);
        idx = bucket_scan(h, hash, താക്കോൽ);
        എങ്കിൽ (idx == IDX_NIL)
                തിരികെ ശൂന്യ;

        e = bucket_at(h, idx);
        എങ്കിൽ (entry_മൂല്യം(h, e) != മൂല്യം)
                തിരികെ ശൂന്യ;

        remove_entry(h, idx);

        തിരികെ മൂല്യം;
}

സ്ഥിതം unsigned find_first_entry(HashmapBase *h) {
        Iterator i = ITERATOR_FIRST;

        എങ്കിൽ (!h || !n_entries(h))
                തിരികെ IDX_NIL;

        തിരികെ hashmap_iterate_entry(h, &i);
}

ശൂന്യം* hashmap_first_താക്കോൽ_and_മൂല്യം(HashmapBase *h, bool remove, ശൂന്യം **ret_താക്കോൽ) {
        ഘടന hashmap_base_entry *e;
        ശൂന്യം *താക്കോൽ, *data;
        unsigned idx;

        idx = find_first_entry(h);
        എങ്കിൽ (idx == IDX_NIL) {
                എങ്കിൽ (ret_താക്കോൽ)
                        *ret_താക്കോൽ = ശൂന്യ;
                തിരികെ ശൂന്യ;
        }

        e = bucket_at(h, idx);
        താക്കോൽ = (ശൂന്യം*) e->താക്കോൽ;
        data = entry_മൂല്യം(h, e);

        എങ്കിൽ (remove)
                remove_entry(h, idx);

        എങ്കിൽ (ret_താക്കോൽ)
                *ret_താക്കോൽ = താക്കോൽ;

        തിരികെ data;
}

unsigned _hashmap_size(HashmapBase *h) {
        എങ്കിൽ (!h)
                തിരികെ 0;

        തിരികെ n_entries(h);
}

unsigned _hashmap_buckets(HashmapBase *h) {
        എങ്കിൽ (!h)
                തിരികെ 0;

        തിരികെ n_buckets(h);
}

പൂർണ്ണസംഖ്യ _hashmap_merge(Hashmap *h, Hashmap *other) {
        Iterator i;
        unsigned idx;

        പ്രതിജ്ഞാ(h);

        HASHMAP_FOREACH_IDX(idx, HASHMAP_BASE(other), i) {
                ഘടന plain_hashmap_entry *pe = plain_bucket_at(other, idx);
                പൂർണ്ണസംഖ്യ r;

                r = hashmap_put(h, pe->b.താക്കോൽ, pe->മൂല്യം);
                എങ്കിൽ (r < 0 && r != -EEXIST)
                        തിരികെ r;
        }

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ set_merge(Set *s, Set *other) {
        Iterator i;
        unsigned idx;

        പ്രതിജ്ഞാ(s);

        HASHMAP_FOREACH_IDX(idx, HASHMAP_BASE(other), i) {
                ഘടന set_entry *se = set_bucket_at(other, idx);
                പൂർണ്ണസംഖ്യ r;

                r = set_put(s, se->b.താക്കോൽ);
                എങ്കിൽ (r < 0)
                        തിരികെ r;
        }

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ _hashmap_reserve(HashmapBase *h, unsigned entries_add) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(h);

        r = resize_buckets(h, entries_add);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ 0;
}

/*
 * The same as hashmap_merge(), but every new item from other is moved to h.
 * Keys already in h are skipped and stay in other.
 * Returns: 0 on success.
 *          -ENOMEM on alloc failure, in which case no move has been done.
 */
പൂർണ്ണസംഖ്യ _hashmap_move(HashmapBase *h, HashmapBase *other) {
        ഘടന swap_entries swap;
        ഘടന hashmap_base_entry *e, *n;
        Iterator i;
        unsigned idx;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(h);

        എങ്കിൽ (!other)
                തിരികെ 0;

        പ്രതിജ്ഞാ(other->type == h->type);

        /*
         * This reserves buckets for the worst case, where none of other's
         * entries are yet present in h. This is preferable to risking
         * an allocation failure in the middle of the moving and having to
         * rollback or return a partial result.
         */
        r = resize_buckets(h, n_entries(other));
        എങ്കിൽ (r < 0)
                തിരികെ r;

        HASHMAP_FOREACH_IDX(idx, other, i) {
                unsigned h_hash;

                e = bucket_at(other, idx);
                h_hash = bucket_hash(h, e->താക്കോൽ);
                എങ്കിൽ (bucket_scan(h, h_hash, e->താക്കോൽ) != IDX_NIL)
                        തുടരുക;

                n = &bucket_at_swap(&swap, IDX_PUT)->p.b;
                n->താക്കോൽ = e->താക്കോൽ;
                എങ്കിൽ (h->type != HASHMAP_TYPE_SET)
                        ((ഘടന plain_hashmap_entry*) n)->മൂല്യം =
                                ((ഘടന plain_hashmap_entry*) e)->മൂല്യം;
                assert_se(hashmap_put_boldly(h, h_hash, &swap, false) == 1);

                remove_entry(other, idx);
        }

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ _hashmap_move_one(HashmapBase *h, HashmapBase *other, സ്ഥിരം ശൂന്യം *താക്കോൽ) {
        ഘടന swap_entries swap;
        unsigned h_hash, other_hash, idx;
        ഘടന hashmap_base_entry *e, *n;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(h);

        h_hash = bucket_hash(h, താക്കോൽ);
        എങ്കിൽ (bucket_scan(h, h_hash, താക്കോൽ) != IDX_NIL)
                തിരികെ -EEXIST;

        എങ്കിൽ (!other)
                തിരികെ -ENOENT;

        പ്രതിജ്ഞാ(other->type == h->type);

        other_hash = bucket_hash(other, താക്കോൽ);
        idx = bucket_scan(other, other_hash, താക്കോൽ);
        എങ്കിൽ (idx == IDX_NIL)
                തിരികെ -ENOENT;

        e = bucket_at(other, idx);

        n = &bucket_at_swap(&swap, IDX_PUT)->p.b;
        n->താക്കോൽ = e->താക്കോൽ;
        എങ്കിൽ (h->type != HASHMAP_TYPE_SET)
                ((ഘടന plain_hashmap_entry*) n)->മൂല്യം =
                        ((ഘടന plain_hashmap_entry*) e)->മൂല്യം;
        r = hashmap_put_boldly(h, h_hash, &swap, true);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        remove_entry(other, idx);
        തിരികെ 0;
}

HashmapBase* _hashmap_copy(HashmapBase *h) {
        HashmapBase *copy;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(h);

        copy = hashmap_base_new(h->hash_ops, h->type);
        എങ്കിൽ (!copy)
                തിരികെ ശൂന്യ;

        തിരഞ്ഞെടുപ്പ് (h->type) {
        അവസ്ഥ HASHMAP_TYPE_PLAIN:
        അവസ്ഥ HASHMAP_TYPE_ORDERED:
                r = hashmap_merge((Hashmap*)copy, (Hashmap*)h);
                നിർത്തുക;
        അവസ്ഥ HASHMAP_TYPE_SET:
                r = set_merge((Set*)copy, (Set*)h);
                നിർത്തുക;
        സ്ഥിരസ്ഥിതി:
                assert_not_reached();
        }

        എങ്കിൽ (r < 0)
                തിരികെ _hashmap_free(copy);

        തിരികെ copy;
}

അക്ഷരം** _hashmap_get_strv(HashmapBase *h) {
        അക്ഷരം **sv;
        Iterator i;
        unsigned idx, n;

        എങ്കിൽ (!h)
                തിരികെ new0(അക്ഷരം*, 1);

        sv = new(അക്ഷരം*, n_entries(h)+1);
        എങ്കിൽ (!sv)
                തിരികെ ശൂന്യ;

        n = 0;
        HASHMAP_FOREACH_IDX(idx, h, i)
                sv[n++] = entry_മൂല്യം(h, bucket_at(h, idx));
        sv[n] = ശൂന്യ;

        തിരികെ sv;
}

അക്ഷരം** set_to_strv(Set **s) {
        പ്രതിജ്ഞാ(s);

        /* This is similar to set_get_strv(), but invalidates the set on success. */

        അക്ഷരം **v = new(അക്ഷരം*, set_size(*s) + 1);
        എങ്കിൽ (!v)
                തിരികെ ശൂന്യ;

        വേണ്ടി (അക്ഷരം **p = v; (*p = set_steal_first(*s)); p++)
                ;

        പ്രതിജ്ഞാ(set_isempty(*s));
        *s = set_free(*s);
        തിരികെ v;
}

ശൂന്യം* ordered_hashmap_next(OrderedHashmap *h, സ്ഥിരം ശൂന്യം *താക്കോൽ) {
        ഘടന ordered_hashmap_entry *e;
        unsigned hash, idx;

        എങ്കിൽ (!h)
                തിരികെ ശൂന്യ;

        hash = bucket_hash(h, താക്കോൽ);
        idx = bucket_scan(h, hash, താക്കോൽ);
        എങ്കിൽ (idx == IDX_NIL)
                തിരികെ ശൂന്യ;

        e = ordered_bucket_at(h, idx);
        എങ്കിൽ (e->iterate_next == IDX_NIL)
                തിരികെ ശൂന്യ;
        തിരികെ ordered_bucket_at(h, e->iterate_next)->p.മൂല്യം;
}

പൂർണ്ണസംഖ്യ set_consume(Set *s, ശൂന്യം *മൂല്യം) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(s);
        പ്രതിജ്ഞാ(മൂല്യം);

        r = set_put(s, മൂല്യം);
        എങ്കിൽ (r <= 0)
                സ്വതന്ത്രം(മൂല്യം);

        തിരികെ r;
}

പൂർണ്ണസംഖ്യ hashmap_put_strdup_full(Hashmap **h, സ്ഥിരം ഘടന hash_ops *hash_ops, സ്ഥിരം അക്ഷരം *k, സ്ഥിരം അക്ഷരം *v) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(h);

        r = hashmap_ensure_allocated(h, hash_ops);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        _cleanup_free_ അക്ഷരം *kdup = ശൂന്യ, *vdup = ശൂന്യ;

        kdup = strdup(k);
        എങ്കിൽ (!kdup)
                തിരികെ -ENOMEM;

        എങ്കിൽ (v) {
                vdup = strdup(v);
                എങ്കിൽ (!vdup)
                        തിരികെ -ENOMEM;
        }

        r = hashmap_put(*h, kdup, vdup);
        എങ്കിൽ (r < 0) {
                എങ്കിൽ (r == -EEXIST && streq_ptr(v, hashmap_get(*h, kdup)))
                        തിരികെ 0;
                തിരികെ r;
        }

        /* 0 with non-null vdup would mean vdup is already in the hashmap, which cannot be */
        പ്രതിജ്ഞാ(vdup == ശൂന്യ || r > 0);
        എങ്കിൽ (r > 0)
                kdup = vdup = ശൂന്യ;

        തിരികെ r;
}

പൂർണ്ണസംഖ്യ set_put_strndup_full(Set **s, സ്ഥിരം ഘടന hash_ops *hash_ops, സ്ഥിരം അക്ഷരം *p, size_t n) {
        അക്ഷരം *c;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(s);
        പ്രതിജ്ഞാ(p);

        r = set_ensure_allocated(s, hash_ops);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (n == SIZE_MAX) {
                എങ്കിൽ (set_contains(*s, (അക്ഷരം*) p))
                        തിരികെ 0;

                c = strdup(p);
        } അല്ലെങ്കിൽ
                c = strndup(p, n);
        എങ്കിൽ (!c)
                തിരികെ -ENOMEM;

        തിരികെ set_consume(*s, c);
}

പൂർണ്ണസംഖ്യ set_put_strdupv_full(Set **s, സ്ഥിരം ഘടന hash_ops *hash_ops, അക്ഷരം **l) {
        പൂർണ്ണസംഖ്യ n = 0, r;

        പ്രതിജ്ഞാ(s);

        STRV_FOREACH(i, l) {
                r = set_put_strndup_full(s, hash_ops, *i, SIZE_MAX);
                എങ്കിൽ (r < 0)
                        തിരികെ r;

                n += r;
        }

        തിരികെ n;
}

പൂർണ്ണസംഖ്യ set_put_strsplit(Set *s, സ്ഥിരം അക്ഷരം *v, സ്ഥിരം അക്ഷരം *separators, ExtractFlags flags) {
        സ്ഥിരം അക്ഷരം *p = ASSERT_PTR(v);
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(s);

        വേണ്ടി (;;) {
                അക്ഷരം *word;

                r = extract_first_word(&p, &word, separators, flags);
                എങ്കിൽ (r <= 0)
                        തിരികെ r;

                r = set_consume(s, word);
                എങ്കിൽ (r < 0)
                        തിരികെ r;
        }
}

/* expand the cachemem if needed, return true if newly (re)activated. */
സ്ഥിതം പൂർണ്ണസംഖ്യ cachemem_maintain(CacheMem *mem, size_t size) {
        പ്രതിജ്ഞാ(mem);

        എങ്കിൽ (!GREEDY_REALLOC(mem->ptr, size)) {
                എങ്കിൽ (size > 0)
                        തിരികെ -ENOMEM;
        }

        എങ്കിൽ (!mem->active) {
                mem->active = true;
                തിരികെ true;
        }

        തിരികെ false;
}

പൂർണ്ണസംഖ്യ iterated_cache_get(IteratedCache *cache, സ്ഥിരം ശൂന്യം ***res_താക്കോലുകൾ, സ്ഥിരം ശൂന്യം ***res_മൂല്യങ്ങൾ, unsigned *res_n_entries) {
        bool sync_താക്കോലുകൾ = false, sync_മൂല്യങ്ങൾ = false;
        size_t size;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(cache);
        പ്രതിജ്ഞാ(cache->hashmap);

        size = n_entries(cache->hashmap);

        എങ്കിൽ (res_താക്കോലുകൾ) {
                r = cachemem_maintain(&cache->താക്കോലുകൾ, size);
                എങ്കിൽ (r < 0)
                        തിരികെ r;

                sync_താക്കോലുകൾ = r;
        } അല്ലെങ്കിൽ
                cache->താക്കോലുകൾ.active = false;

        എങ്കിൽ (res_മൂല്യങ്ങൾ) {
                r = cachemem_maintain(&cache->values, size);
                എങ്കിൽ (r < 0)
                        തിരികെ r;

                sync_മൂല്യങ്ങൾ = r;
        } അല്ലെങ്കിൽ
                cache->values.active = false;

        എങ്കിൽ (cache->hashmap->dirty) {
                എങ്കിൽ (cache->താക്കോലുകൾ.active)
                        sync_താക്കോലുകൾ = true;
                എങ്കിൽ (cache->values.active)
                        sync_മൂല്യങ്ങൾ = true;

                cache->hashmap->dirty = false;
        }

        എങ്കിൽ (sync_താക്കോലുകൾ || sync_മൂല്യങ്ങൾ) {
                unsigned i, idx;
                Iterator iter;

                i = 0;
                HASHMAP_FOREACH_IDX(idx, cache->hashmap, iter) {
                        ഘടന hashmap_base_entry *e;

                        e = bucket_at(cache->hashmap, idx);

                        എങ്കിൽ (sync_താക്കോലുകൾ)
                                cache->താക്കോലുകൾ.ptr[i] = e->താക്കോൽ;
                        എങ്കിൽ (sync_മൂല്യങ്ങൾ)
                                cache->values.ptr[i] = entry_മൂല്യം(cache->hashmap, e);
                        i++;
                }
        }

        എങ്കിൽ (res_താക്കോലുകൾ)
                *res_താക്കോലുകൾ = cache->താക്കോലുകൾ.ptr;
        എങ്കിൽ (res_മൂല്യങ്ങൾ)
                *res_മൂല്യങ്ങൾ = cache->values.ptr;
        എങ്കിൽ (res_n_entries)
                *res_n_entries = size;

        തിരികെ 0;
}

IteratedCache* iterated_cache_free(IteratedCache *cache) {
        എങ്കിൽ (cache) {
                സ്വതന്ത്രം(cache->താക്കോലുകൾ.ptr);
                സ്വതന്ത്രം(cache->values.ptr);
        }

        തിരികെ mfree(cache);
}

പൂർണ്ണസംഖ്യ set_strjoin(Set *s, സ്ഥിരം അക്ഷരം *separator, bool wrap_with_separator, അക്ഷരം **ret) {
        _cleanup_free_ അക്ഷരം *str = ശൂന്യ;
        size_t separator_len, len = 0;
        സ്ഥിരം അക്ഷരം *മൂല്യം;
        bool first;

        പ്രതിജ്ഞാ(ret);

        എങ്കിൽ (set_isempty(s)) {
                *ret = ശൂന്യ;
                തിരികെ 0;
        }

        separator_len = strlen_ptr(separator);

        എങ്കിൽ (separator_len == 0)
                wrap_with_separator = false;

        first = !wrap_with_separator;

        SET_FOREACH(മൂല്യം, s) {
                size_t l = strlen_ptr(മൂല്യം);

                എങ്കിൽ (l == 0)
                        തുടരുക;

                എങ്കിൽ (!GREEDY_REALLOC(str, len + l + (first ? 0 : separator_len) + (wrap_with_separator ? separator_len : 0) + 1))
                        തിരികെ -ENOMEM;

                എങ്കിൽ (separator_len > 0 && !first) {
                        memcpy(str + len, separator, separator_len);
                        len += separator_len;
                }

                memcpy(str + len, മൂല്യം, l);
                len += l;
                first = false;
        }

        എങ്കിൽ (wrap_with_separator) {
                memcpy(str + len, separator, separator_len);
                len += separator_len;
        }

        str[len] = '\0';

        *ret = TAKE_PTR(str);
        തിരികെ 0;
}

bool set_equal(Set *a, Set *b) {
        ശൂന്യം *p;

        /* Checks whether each entry of 'a' is also in 'b' and vice versa, i.e. the two sets contain the same
         * entries */

        എങ്കിൽ (a == b)
                തിരികെ true;

        എങ്കിൽ (set_isempty(a) && set_isempty(b))
                തിരികെ true;

        എങ്കിൽ (set_size(a) != set_size(b)) /* Cheap check that hopefully catches a lot of inequality cases
                                         * already */
                തിരികെ false;

        SET_FOREACH(p, a)
                എങ്കിൽ (!set_contains(b, p))
                        തിരികെ false;

        /* If we have the same hashops, then we don't need to check things backwards given we compared the
         * size and that all of a is in b. */
        എങ്കിൽ (a->b.hash_ops == b->b.hash_ops)
                തിരികെ true;

        SET_FOREACH(p, b)
                എങ്കിൽ (!set_contains(a, p))
                        തിരികെ false;

        തിരികെ true;
}

സ്ഥിതം bool set_fnmatch_one(Set *patterns, സ്ഥിരം അക്ഷരം *needle) {
        സ്ഥിരം അക്ഷരം *p;

        പ്രതിജ്ഞാ(needle);

        /* Any failure of fnmatch() is treated as equivalent to FNM_NOMATCH, i.e. as non-matching pattern */

        SET_FOREACH(p, patterns)
                എങ്കിൽ (fnmatch(p, needle, 0) == 0)
                        തിരികെ true;

        തിരികെ false;
}

bool set_fnmatch(Set *include_patterns, Set *exclude_patterns, സ്ഥിരം അക്ഷരം *needle) {
        പ്രതിജ്ഞാ(needle);

        എങ്കിൽ (set_fnmatch_one(exclude_patterns, needle))
                തിരികെ false;

        എങ്കിൽ (set_isempty(include_patterns))
                തിരികെ true;

        തിരികെ set_fnmatch_one(include_patterns, needle);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ hashmap_entry_compare(
                ഘടന hashmap_base_entry * സ്ഥിരം *a,
                ഘടന hashmap_base_entry * സ്ഥിരം *b,
                compare_func_t compare) {

        പ്രതിജ്ഞാ(a && *a);
        പ്രതിജ്ഞാ(b && *b);
        പ്രതിജ്ഞാ(compare);

        തിരികെ compare((*a)->താക്കോൽ, (*b)->താക്കോൽ);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ _hashmap_dump_entries_sorted(
                HashmapBase *h,
                ശൂന്യം ***ret,
                size_t *ret_n) {
        _cleanup_free_ ശൂന്യം **entries = ശൂന്യ;
        Iterator iter;
        unsigned idx;
        size_t n = 0;

        പ്രതിജ്ഞാ(ret);
        പ്രതിജ്ഞാ(ret_n);

        എങ്കിൽ (_hashmap_size(h) == 0) {
                *ret = ശൂന്യ;
                *ret_n = 0;
                തിരികെ 0;
        }

        /* We append one more element than needed so that the resulting array can be used as a strv. We
         * don't count this entry in the returned size. */
        entries = new(ശൂന്യം*, _hashmap_size(h) + 1);
        എങ്കിൽ (!entries)
                തിരികെ -ENOMEM;

        HASHMAP_FOREACH_IDX(idx, h, iter)
                entries[n++] = bucket_at(h, idx);

        പ്രതിജ്ഞാ(n == _hashmap_size(h));
        entries[n] = ശൂന്യ;

        typesafe_qsort_r((ഘടന hashmap_base_entry**) entries, n,
                         hashmap_entry_compare, h->hash_ops->compare);

        *ret = TAKE_PTR(entries);
        *ret_n = n;
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ hashmap_dump_താക്കോലുകൾ_sorted(HashmapBase *h, ശൂന്യം ***ret, size_t *ret_n) {
        _cleanup_free_ ശൂന്യം **entries = ശൂന്യ;
        size_t n;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(ret);

        r = _hashmap_dump_entries_sorted(h, &entries, &n);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        /* Reuse the array. */
        FOREACH_ARRAY(e, entries, n)
                *e = (ശൂന്യം*) (*(ഘടന hashmap_base_entry**) e)->താക്കോൽ;

        *ret = TAKE_PTR(entries);
        എങ്കിൽ (ret_n)
                *ret_n = n;
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ _hashmap_dump_sorted(HashmapBase *h, ശൂന്യം ***ret, size_t *ret_n) {
        _cleanup_free_ ശൂന്യം **entries = ശൂന്യ;
        size_t n;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(ret);

        r = _hashmap_dump_entries_sorted(h, &entries, &n);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        /* Reuse the array. */
        FOREACH_ARRAY(e, entries, n)
                *e = entry_മൂല്യം(h, *(ഘടന hashmap_base_entry**) e);

        *ret = TAKE_PTR(entries);
        എങ്കിൽ (ret_n)
                *ret_n = n;
        തിരികെ 0;
}
