/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব <fnmatch.h>
#অন্তর্ভাব <pthread.h>
#অন্তর্ভাব <unistd.h>
#যদি HAVE_VALGRIND_VALGRIND_H
#  include <valgrind/valgrind.h>
#endif

#অন্তর্ভাব "alloc-util.h"
#অন্তর্ভাব "extract-word.h"
#অন্তর্ভাব "hashmap.h"
#অন্তর্ভাব "log.h"
#অন্তর্ভাব "logarithm.h"
#অন্তর্ভাব "memory-util.h"
#অন্তর্ভাব "mempool.h"
#অন্তর্ভাব "process-util.h"
#অন্তর্ভাব "random-util.h"
#অন্তর্ভাব "set.h"
#অন্তর্ভাব "siphash24.h"
#অন্তর্ভাব "sort-util.h"
#অন্তর্ভাব "string-util.h"
#অন্তর্ভাব "strv.h"

#যদি ENABLE_DEBUG_HASHMAP
#অন্তর্ভাব "list.h"
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
কাঠামো hashmap_base_entry {
        ধ্রুবক শূন্য *চাবি;
};

/* Entry types for specific hashmap/set types
 * hashmap_base_entry must be at the beginning of each entry struct. */

কাঠামো plain_hashmap_entry {
        কাঠামো hashmap_base_entry b;
        শূন্য *মান;
};

কাঠামো ordered_hashmap_entry {
        কাঠামো plain_hashmap_entry p;
        unsigned iterate_পরবর্তী, iterate_previous;
};

কাঠামো set_entry {
        কাঠামো hashmap_base_entry b;
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
কাঠামো swap_entries {
        কাঠামো ordered_hashmap_entry e[_IDX_SWAP_END - _IDX_SWAP_BEGIN];
};

/* Distance from Initial Bucket */
প্রকারলেখ uint8_t dib_raw_t;
#define DIB_RAW_OVERFLOW ((dib_raw_t)0xfdU)   /* indicates DIB value is greater than representable */
#define DIB_RAW_REHASH   ((dib_raw_t)0xfeU)   /* entry yet to be rehashed during in-place resize */
#define DIB_RAW_FREE     ((dib_raw_t)0xffU)   /* a free bucket */
#define DIB_RAW_INIT     ((অক্ষর)DIB_RAW_FREE) /* a byte to memset a DIB store with when initializing */

#define DIB_FREE UINT_MAX

#যদি ENABLE_DEBUG_HASHMAP
কাঠামো hashmap_debug_info {
        LIST_FIELDS(কাঠামো hashmap_debug_info, debug_list);
        unsigned max_entries;  /* high watermark of n_entries */

        /* fields to detect modification while iterating */
        unsigned put_গুনতি;    /* counts puts into the hashmap */
        unsigned rem_গুনতি;    /* counts removals from hashmap */
        unsigned last_rem_idx; /* remembers last removal index */
};

/* Tracks all existing hashmaps. Get at it from gdb. See sd_dump_hashmaps.py */
স্থির LIST_শীর্ষ(কাঠামো hashmap_debug_info, hashmap_debug_list);
স্থির pthread_mutex_t hashmap_debug_list_mutex = PTHREAD_MUTEX_INITIALIZER;
#endif

গণনা HashmapType {
        HASHMAP_TYPE_PLAIN,
        HASHMAP_TYPE_ORDERED,
        HASHMAP_TYPE_SET,
        _HASHMAP_TYPE_MAX
};

কাঠামো _packed_ indirect_storage {
        শূন্য *storage;                     /* where buckets and DIBs are stored */
        uint8_t  hash_চাবি[HASH_চাবি_আকার];  /* hash key; changes during resize */

        unsigned n_entries;                /* number of stored entries */
        unsigned n_buckets;                /* number of buckets */

        unsigned idx_lowest_entry;         /* Index below which all buckets are free.
                                              Makes "while (hashmap_steal_first())" loops
                                              O(n) instead of O(n^2) for unordered hashmaps. */
        uint8_t  _pad[3];                  /* padding for the whole HashmapBase */
        /* The bitfields in HashmapBase complete the alignment of the whole thing. */
};

কাঠামো direct_storage {
        /* This gives us 39 bytes on 64-bit, or 35 bytes on 32-bit.
         * That's room for 4 set_entries + 4 DIB bytes + 3 unused bytes on 64-bit,
         *              or 7 set_entries + 7 DIB bytes + 0 unused bytes on 32-bit. */
        uint8_t storage[আকার(কাঠামো indirect_storage)];
};

#define DIRECT_BUCKETS(entry_t) \
        (আকার(কাঠামো direct_storage) / (আকার(entry_t) + আকার(dib_raw_t)))

/* We should be able to store at least one entry directly. */
assert_cc(DIRECT_BUCKETS(কাঠামো ordered_hashmap_entry) >= 1);

/* We have 3 bits for n_direct_entries. */
assert_cc(DIRECT_BUCKETS(কাঠামো set_entry) < (1 << 3));

/* Hashmaps with directly stored entries all use this shared hash key.
 * It's no big deal if the key is guessed, because there can be only
 * a handful of directly stored entries in a hashmap. When a hashmap
 * outgrows direct storage, it gets its own key for indirect storage. */
স্থির uint8_t shared_hash_চাবি[HASH_চাবি_আকার];

/* Fields that all hashmap/set types must have */
কাঠামো HashmapBase {
        ধ্রুবক কাঠামো hash_ops *hash_ops;  /* hash and compare ops to use */

        সংঘ _packed_ {
                কাঠামো indirect_storage indirect; /* if  has_indirect */
                কাঠামো direct_storage direct;     /* if !has_indirect */
        };

        গণনা HashmapType type:2;     /* HASHMAP_TYPE_* */
        bool has_indirect:1;         /* whether indirect storage is used */
        unsigned n_direct_entries:3; /* Number of entries in direct storage.
                                      * Only valid if !has_indirect. */
        bool from_pool:1;            /* whether was allocated from mempool */
        bool dirty:1;                /* whether dirtied since last iterated_cache_get() */
        bool cached:1;               /* whether this hashmap is being cached */

#যদি ENABLE_DEBUG_HASHMAP
        কাঠামো hashmap_debug_info debug;
#endif
};

/* Specific hash types
 * HashmapBase must be at the beginning of each hashmap struct. */

কাঠামো Hashmap {
        কাঠামো HashmapBase b;
};

কাঠামো OrderedHashmap {
        কাঠামো HashmapBase b;
        unsigned iterate_list_শীর্ষ, iterate_list_লেজ;
};

কাঠামো Set {
        কাঠামো HashmapBase b;
};

প্রকারলেখ কাঠামো CacheMem {
        ধ্রুবক শূন্য **ptr;
        size_t n_populated;
        bool active:1;
} CacheMem;

কাঠামো IteratedCache {
        HashmapBase *hashmap;
        CacheMem চাবিগুলি, values;
};

DEFINE_MEMPOOL(hashmap_pool,         Hashmap,        8);
DEFINE_MEMPOOL(ordered_hashmap_pool, OrderedHashmap, 8);
/* No need for a separate Set pool */
assert_cc(আকার(Hashmap) == আকার(Set));

কাঠামো hashmap_type_info {
        size_t শীর্ষ_আকার;
        size_t entry_আকার;
        কাঠামো mempool *mempool;
        unsigned n_direct_buckets;
};

স্থির _used_ ধ্রুবক কাঠামো hashmap_type_info hashmap_type_info[_HASHMAP_TYPE_MAX] = {
        [HASHMAP_TYPE_PLAIN] = {
                .শীর্ষ_আকার        = আকার(Hashmap),
                .entry_আকার       = আকার(কাঠামো plain_hashmap_entry),
                .mempool          = &hashmap_pool,
                .n_direct_buckets = DIRECT_BUCKETS(কাঠামো plain_hashmap_entry),
        },
        [HASHMAP_TYPE_ORDERED] = {
                .শীর্ষ_আকার        = আকার(OrderedHashmap),
                .entry_আকার       = আকার(কাঠামো ordered_hashmap_entry),
                .mempool          = &ordered_hashmap_pool,
                .n_direct_buckets = DIRECT_BUCKETS(কাঠামো ordered_hashmap_entry),
        },
        [HASHMAP_TYPE_SET] = {
                .শীর্ষ_আকার        = আকার(Set),
                .entry_আকার       = আকার(কাঠামো set_entry),
                .mempool          = &hashmap_pool,
                .n_direct_buckets = DIRECT_BUCKETS(কাঠামো set_entry),
        },
};

শূন্য hashmap_trim_pools(শূন্য) {
        পূর্ণসংখ্যা r;

        /* The pool is only allocated by the main thread, but the memory can be passed to other
         * threads. Let's clean up if we are the main thread and no other threads are live. */

        /* We build our own is_main_thread() here, which doesn't use C11 TLS based caching of the
         * result. That's because valgrind apparently doesn't like TLS to be used from a GCC destructor. */
        যদি (getpid() != gettid())
                ফেরত (শূন্য) log_debug("Not cleaning up memory pools, not in main thread.");

        r = get_process_threads(0);
        যদি (r < 0)
                ফেরত (শূন্য) log_debug_errno(r, "Failed to determine number of threads, not cleaning up memory pools: %m");
        যদি (r != 1)
                ফেরত (শূন্য) log_debug("Not cleaning up memory pools, running in multi-threaded process.");

        mempool_trim(&hashmap_pool);
        mempool_trim(&ordered_hashmap_pool);
}

#যদি HAVE_VALGRIND_VALGRIND_H
_destructor_ স্থির শূন্য cleanup_pools(শূন্য) {
        /* Be nice to valgrind */
        যদি (RUNNING_ON_VALGRIND)
                hashmap_trim_pools();
}
#endif

স্থির অন্তঃরেখ unsigned n_buckets(HashmapBase *h) {
        প্রতিজ্ঞা(h);
        ফেরত h->has_indirect ? h->indirect.n_buckets
                               : hashmap_type_info[h->type].n_direct_buckets;
}

স্থির unsigned n_entries(HashmapBase *h) {
        প্রতিজ্ঞা(h);
        ফেরত h->has_indirect ? h->indirect.n_entries
                               : h->n_direct_entries;
}

স্থির শূন্য n_entries_inc(HashmapBase *h) {
        প্রতিজ্ঞা(h);

        যদি (h->has_indirect)
                h->indirect.n_entries++;
        অন্যথা
                h->n_direct_entries++;
}

স্থির শূন্য n_entries_dec(HashmapBase *h) {
        প্রতিজ্ঞা(h);

        যদি (h->has_indirect)
                h->indirect.n_entries--;
        অন্যথা
                h->n_direct_entries--;
}

স্থির অন্তঃরেখ শূন্য* storage_ptr(HashmapBase *h) {
        প্রতিজ্ঞা(h);
        ফেরত h->has_indirect ? h->indirect.storage
                               : h->direct.storage;
}

স্থির অন্তঃরেখ uint8_t* hash_চাবি(HashmapBase *h) {
        প্রতিজ্ঞা(h);
        ফেরত h->has_indirect ? h->indirect.hash_চাবি
                               : shared_hash_চাবি;
}

স্থির unsigned base_bucket_hash(HashmapBase *h, ধ্রুবক শূন্য *p) {
        কাঠামো siphash state;
        uint64_t hash;

        প্রতিজ্ঞা(h);

        /* The keys of trivial_hash_ops and uint64_hash_ops fit in 64 bits. siphash24_uint64() hashes such
         * a key without the indirect call through hash_ops and keeps the SipHash state in registers. On
         * 32-bit and big-endian architectures, the result differs from the result of trivial_hash_func(),
         * which hashes the key in native byte order. Only this function maps keys to buckets, so the
         * difference does not matter. */
        যদি (h->hash_ops->hash == trivial_hash_func)
                hash = siphash24_uint64((uintptr_t) p, hash_চাবি(h));
        অন্যথা যদি (h->hash_ops->hash == (hash_func_t) uint64_hash_func)
                hash = siphash24_uint64(*(ধ্রুবক uint64_t*) p, hash_চাবি(h));
        অন্যথা {
                siphash24_init(&state, hash_চাবি(h));
                h->hash_ops->hash(p, &state);
                hash = siphash24_finalize(&state);
        }

        /* n_buckets is not a power of two, so a modulo needs a division. Multiplying the upper 32 bits of
         * the hash with n_buckets and keeping the upper 32 bits of the product maps the hash to
         * [0, n_buckets) almost as evenly, without a division. */
        ফেরত (unsigned) (((hash >> 32) * n_buckets(h)) >> 32);
}
#define bucket_hash(h, p) base_bucket_hash(HASHMAP_BASE(h), p)

স্থির শূন্য base_set_dirty(HashmapBase *h) {
        প্রতিজ্ঞা(h);

        h->dirty = true;
}
#define hashmap_set_dirty(h) base_set_dirty(HASHMAP_BASE(h))

স্থির শূন্য get_hash_চাবি(uint8_t hash_চাবি[HASH_চাবি_আকার], bool reuse_is_ok) {
        স্থির uint8_t current[HASH_চাবি_আকার];
        স্থির bool current_initialized = false;

        /* Returns a hash function key to use. In order to keep things
         * fast we will not generate a new key each time we allocate a
         * new hash table. Instead, we'll just reuse the most recently
         * generated one, except if we never generated one or when we
         * are rehashing an entire hash table because we reached a
         * fill level */

        যদি (!current_initialized || !reuse_is_ok) {
                random_bytes(current, আকার(current));
                current_initialized = true;
        }

        memcpy(hash_চাবি, current, আকার(current));
}

স্থির অন্তঃরেখ কাঠামো hashmap_base_entry* bucket_at(HashmapBase *h, unsigned idx) {
        প্রতিজ্ঞা(h);
        ফেরত CAST_ALIGN_PTR(
                        কাঠামো hashmap_base_entry,
                        (uint8_t *) storage_ptr(h) + idx * hashmap_type_info[h->type].entry_আকার);
}

স্থির কাঠামো plain_hashmap_entry* plain_bucket_at(Hashmap *h, unsigned idx) {
        ফেরত (কাঠামো plain_hashmap_entry*) bucket_at(HASHMAP_BASE(h), idx);
}

স্থির কাঠামো ordered_hashmap_entry* ordered_bucket_at(OrderedHashmap *h, unsigned idx) {
        ফেরত (কাঠামো ordered_hashmap_entry*) bucket_at(HASHMAP_BASE(h), idx);
}

স্থির কাঠামো set_entry *set_bucket_at(Set *h, unsigned idx) {
        ফেরত (কাঠামো set_entry*) bucket_at(HASHMAP_BASE(h), idx);
}

স্থির কাঠামো ordered_hashmap_entry* bucket_at_swap(কাঠামো swap_entries *swap, unsigned idx) {
        প্রতিজ্ঞা(swap);
        ফেরত &swap->e[idx - _IDX_SWAP_BEGIN];
}

/* Returns a pointer to the bucket at index idx.
 * Understands real indexes and swap indexes, hence "_virtual". */
স্থির কাঠামো hashmap_base_entry* bucket_at_virtual(HashmapBase *h, কাঠামো swap_entries *swap,
                                                    unsigned idx) {
        যদি (idx < _IDX_SWAP_BEGIN)
                ফেরত bucket_at(h, idx);

        যদি (idx < _IDX_SWAP_END)
                ফেরত &bucket_at_swap(swap, idx)->p.b;

        assert_not_reached();
}

স্থির অন্তঃরেখ dib_raw_t* dib_raw_ptr(HashmapBase *h) {
        প্রতিজ্ঞা(h);
        ফেরত (dib_raw_t*)
                ((uint8_t*) storage_ptr(h) + hashmap_type_info[h->type].entry_আকার * n_buckets(h));
}

স্থির unsigned bucket_distance(HashmapBase *h, unsigned idx, unsigned from) {
        ফেরত idx >= from ? idx - from
                           : n_buckets(h) + idx - from;
}

স্থির unsigned bucket_calculate_dib(HashmapBase *h, unsigned idx, dib_raw_t raw_dib) {
        unsigned initial_bucket;

        যদি (raw_dib == DIB_RAW_FREE)
                ফেরত DIB_FREE;

        যদি (_likely_(raw_dib < DIB_RAW_OVERFLOW))
                ফেরত raw_dib;

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
        initial_bucket = bucket_hash(h, bucket_at(h, idx)->চাবি);
        ফেরত bucket_distance(h, idx, initial_bucket);
}

স্থির শূন্য bucket_set_dib(HashmapBase *h, unsigned idx, unsigned dib) {
        dib_raw_ptr(h)[idx] = dib != DIB_FREE ? MIN(dib, DIB_RAW_OVERFLOW) : DIB_RAW_FREE;
}

স্থির unsigned skip_free_buckets(HashmapBase *h, unsigned idx) {
        dib_raw_t *dibs;

        dibs = dib_raw_ptr(h);

        জন্য ( ; idx < n_buckets(h); idx++)
                যদি (dibs[idx] != DIB_RAW_FREE)
                        ফেরত idx;

        ফেরত IDX_NIL;
}

স্থির শূন্য bucket_mark_free(HashmapBase *h, unsigned idx) {
        প্রতিজ্ঞা(h);

        memzero(bucket_at(h, idx), hashmap_type_info[h->type].entry_আকার);
        bucket_set_dib(h, idx, DIB_FREE);
}

স্থির শূন্য bucket_move_entry(HashmapBase *h, কাঠামো swap_entries *swap,
                              unsigned from, unsigned to) {
        কাঠামো hashmap_base_entry *e_from, *e_to;

        প্রতিজ্ঞা(h);
        প্রতিজ্ঞা(from != to);

        e_from = bucket_at_virtual(h, swap, from);
        e_to   = bucket_at_virtual(h, swap, to);

        memcpy(e_to, e_from, hashmap_type_info[h->type].entry_আকার);

        যদি (h->type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*) h;
                কাঠামো ordered_hashmap_entry *le, *le_to;

                le_to = (কাঠামো ordered_hashmap_entry*) e_to;

                যদি (le_to->iterate_পরবর্তী != IDX_NIL) {
                        le = (কাঠামো ordered_hashmap_entry*)
                             bucket_at_virtual(h, swap, le_to->iterate_পরবর্তী);
                        le->iterate_previous = to;
                }

                যদি (le_to->iterate_previous != IDX_NIL) {
                        le = (কাঠামো ordered_hashmap_entry*)
                             bucket_at_virtual(h, swap, le_to->iterate_previous);
                        le->iterate_পরবর্তী = to;
                }

                যদি (lh->iterate_list_শীর্ষ == from)
                        lh->iterate_list_শীর্ষ = to;
                যদি (lh->iterate_list_লেজ == from)
                        lh->iterate_list_লেজ = to;
        }
}

স্থির অন্তঃরেখ unsigned পরবর্তী_idx(HashmapBase *h, unsigned idx) {
        ফেরত idx + 1U < n_buckets(h) ? idx + 1U : 0U;
}

স্থির unsigned পূর্ব_idx(HashmapBase *h, unsigned idx) {
        ফেরত idx > 0U ? idx - 1U : n_buckets(h) - 1U;
}

স্থির অন্তঃরেখ শূন্য* entry_মান(HashmapBase *h, কাঠামো hashmap_base_entry *e) {
        প্রতিজ্ঞা(h);
        প্রতিজ্ঞা(e);

        নির্বাচন (h->type) {

        ক্ষেত্র HASHMAP_TYPE_PLAIN:
        ক্ষেত্র HASHMAP_TYPE_ORDERED:
                ফেরত ((কাঠামো plain_hashmap_entry*)e)->মান;

        ক্ষেত্র HASHMAP_TYPE_SET:
                ফেরত (শূন্য*) e->চাবি;

        পূর্বনির্ধারিত:
                assert_not_reached();
        }
}

স্থির শূন্য base_remove_entry(HashmapBase *h, unsigned idx) {
        unsigned বাম, ডান, পূর্ব, dib;
        dib_raw_t raw_dib, *dibs;

        dibs = dib_raw_ptr(h);
        প্রতিজ্ঞা(dibs[idx] != DIB_RAW_FREE);

#যদি ENABLE_DEBUG_HASHMAP
        প্রতিজ্ঞা(h);
        h->debug.rem_গুনতি++;
        h->debug.last_rem_idx = idx;
#endif

        বাম = idx;
        /* Find the stop bucket ("right"). It is either free or has DIB == 0. */
        জন্য (ডান = পরবর্তী_idx(h, বাম); ; ডান = পরবর্তী_idx(h, ডান)) {
                raw_dib = dibs[ডান];
                যদি (IN_SET(raw_dib, 0, DIB_RAW_FREE))
                        বিরতি;

                /* The buckets are not supposed to be all occupied and with DIB > 0.
                 * That would mean we could make everyone better off by shifting them
                 * backward. This scenario is impossible. */
                প্রতিজ্ঞা(বাম != ডান);
        }

        যদি (h->type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*) h;
                কাঠামো ordered_hashmap_entry *le = ordered_bucket_at(lh, idx);

                যদি (le->iterate_পরবর্তী != IDX_NIL)
                        ordered_bucket_at(lh, le->iterate_পরবর্তী)->iterate_previous = le->iterate_previous;
                অন্যথা
                        lh->iterate_list_লেজ = le->iterate_previous;

                যদি (le->iterate_previous != IDX_NIL)
                        ordered_bucket_at(lh, le->iterate_previous)->iterate_পরবর্তী = le->iterate_পরবর্তী;
                অন্যথা
                        lh->iterate_list_শীর্ষ = le->iterate_পরবর্তী;
        }

        /* Now shift all buckets in the interval (left, right) one step backwards */
        জন্য (পূর্ব = বাম, বাম = পরবর্তী_idx(h, বাম); বাম != ডান;
             পূর্ব = বাম, বাম = পরবর্তী_idx(h, বাম)) {
                dib = bucket_calculate_dib(h, বাম, dibs[বাম]);
                প্রতিজ্ঞা(dib != 0);
                bucket_move_entry(h, ফাঁকা, বাম, পূর্ব);
                bucket_set_dib(h, পূর্ব, dib - 1);
        }

        bucket_mark_free(h, পূর্ব);
        n_entries_dec(h);
        base_set_dirty(h);
}
#define remove_entry(h, idx) base_remove_entry(HASHMAP_BASE(h), idx)

স্থির unsigned hashmap_iterate_in_insertion_order(OrderedHashmap *h, Iterator *i) {
        কাঠামো ordered_hashmap_entry *e;
        unsigned idx;

        প্রতিজ্ঞা(h);
        প্রতিজ্ঞা(i);

        যদি (i->idx == IDX_NIL)
                যাও at_end;

        যদি (i->idx == IDX_FIRST && h->iterate_list_শীর্ষ == IDX_NIL)
                যাও at_end;

        যদি (i->idx == IDX_FIRST) {
                idx = h->iterate_list_শীর্ষ;
                e = ordered_bucket_at(h, idx);
        } অন্যথা {
                idx = i->idx;
                e = ordered_bucket_at(h, idx);
                /*
                 * We allow removing the current entry while iterating, but removal may cause
                 * a backward shift. The next entry may thus move one bucket to the left.
                 * To detect when it happens, we remember the key pointer of the entry we were
                 * going to iterate next. If it does not match, there was a backward shift.
                 */
                যদি (e->p.b.চাবি != i->পরবর্তী_চাবি) {
                        idx = পূর্ব_idx(HASHMAP_BASE(h), idx);
                        e = ordered_bucket_at(h, idx);
                }
                প্রতিজ্ঞা(e->p.b.চাবি == i->পরবর্তী_চাবি);
        }

#যদি ENABLE_DEBUG_HASHMAP
        i->পূর্ব_idx = idx;
#endif

        যদি (e->iterate_পরবর্তী != IDX_NIL) {
                কাঠামো ordered_hashmap_entry *n;
                i->idx = e->iterate_পরবর্তী;
                n = ordered_bucket_at(h, i->idx);
                i->পরবর্তী_চাবি = n->p.b.চাবি;
        } অন্যথা
                i->idx = IDX_NIL;

        ফেরত idx;

at_end:
        i->idx = IDX_NIL;
        ফেরত IDX_NIL;
}

স্থির unsigned hashmap_iterate_in_internal_order(HashmapBase *h, Iterator *i) {
        unsigned idx;

        প্রতিজ্ঞা(h);
        প্রতিজ্ঞা(i);

        যদি (i->idx == IDX_NIL)
                যাও at_end;

        যদি (i->idx == IDX_FIRST) {
                /* fast forward to the first occupied bucket */
                যদি (h->has_indirect) {
                        i->idx = skip_free_buckets(h, h->indirect.idx_lowest_entry);
                        h->indirect.idx_lowest_entry = i->idx;
                } অন্যথা
                        i->idx = skip_free_buckets(h, 0);

                যদি (i->idx == IDX_NIL)
                        যাও at_end;
        } অন্যথা {
                কাঠামো hashmap_base_entry *e;

                প্রতিজ্ঞা(i->idx > 0);

                e = bucket_at(h, i->idx);
                /*
                 * We allow removing the current entry while iterating, but removal may cause
                 * a backward shift. The next entry may thus move one bucket to the left.
                 * To detect when it happens, we remember the key pointer of the entry we were
                 * going to iterate next. If it does not match, there was a backward shift.
                 */
                যদি (e->চাবি != i->পরবর্তী_চাবি)
                        e = bucket_at(h, --i->idx);

                প্রতিজ্ঞা(e->চাবি == i->পরবর্তী_চাবি);
        }

        idx = i->idx;
#যদি ENABLE_DEBUG_HASHMAP
        i->পূর্ব_idx = idx;
#endif

        i->idx = skip_free_buckets(h, i->idx + 1);
        যদি (i->idx != IDX_NIL)
                i->পরবর্তী_চাবি = bucket_at(h, i->idx)->চাবি;
        অন্যথা
                i->idx = IDX_NIL;

        ফেরত idx;

at_end:
        i->idx = IDX_NIL;
        ফেরত IDX_NIL;
}

স্থির unsigned hashmap_iterate_entry(HashmapBase *h, Iterator *i) {
        যদি (!h) {
                i->idx = IDX_NIL;
                ফেরত IDX_NIL;
        }

#যদি ENABLE_DEBUG_HASHMAP
        যদি (i->idx == IDX_FIRST) {
                i->put_গুনতি = h->debug.put_গুনতি;
                i->rem_গুনতি = h->debug.rem_গুনতি;
        } অন্যথা {
                /* While iterating, must not add any new entries */
                প্রতিজ্ঞা(i->put_গুনতি == h->debug.put_গুনতি);
                /* ... or remove entries other than the current one */
                প্রতিজ্ঞা(i->rem_গুনতি == h->debug.rem_গুনতি ||
                       (i->rem_গুনতি == h->debug.rem_গুনতি - 1 &&
                        i->পূর্ব_idx == h->debug.last_rem_idx));
                /* Reset our removals counter */
                i->rem_গুনতি = h->debug.rem_গুনতি;
        }
#endif

        ফেরত h->type == HASHMAP_TYPE_ORDERED ? hashmap_iterate_in_insertion_order((OrderedHashmap*) h, i)
                                               : hashmap_iterate_in_internal_order(h, i);
}

bool _hashmap_iterate(HashmapBase *h, Iterator *i, শূন্য **মান, ধ্রুবক শূন্য **চাবি) {
        কাঠামো hashmap_base_entry *e;
        শূন্য *তথ্য;
        unsigned idx;

        idx = hashmap_iterate_entry(h, i);
        যদি (idx == IDX_NIL) {
                যদি (মান)
                        *মান = ফাঁকা;
                যদি (চাবি)
                        *চাবি = ফাঁকা;

                ফেরত false;
        }

        e = bucket_at(h, idx);
        তথ্য = entry_মান(h, e);
        যদি (মান)
                *মান = তথ্য;
        যদি (চাবি)
                *চাবি = e->চাবি;

        ফেরত true;
}

#define HASHMAP_FOREACH_IDX(idx, h, i) \
        জন্য ((i) = ITERATOR_FIRST, (idx) = hashmap_iterate_entry((h), &(i)); \
             (idx != IDX_NIL); \
             (idx) = hashmap_iterate_entry((h), &(i)))

IteratedCache* _hashmap_iterated_cache_new(HashmapBase *h) {
        IteratedCache *cache;

        প্রতিজ্ঞা(h);
        প্রতিজ্ঞা(!h->cached);

        যদি (h->cached)
                ফেরত ফাঁকা;

        cache = new0(IteratedCache, 1);
        যদি (!cache)
                ফেরত ফাঁকা;

        cache->hashmap = h;
        h->cached = true;

        ফেরত cache;
}

স্থির শূন্য reset_direct_storage(HashmapBase *h) {
        ধ্রুবক কাঠামো hashmap_type_info *hi = &hashmap_type_info[h->type];
        শূন্য *p;

        প্রতিজ্ঞা(!h->has_indirect);

        p = mempset(h->direct.storage, 0, hi->entry_আকার * hi->n_direct_buckets);
        memset(p, DIB_RAW_INIT, আকার(dib_raw_t) * hi->n_direct_buckets);
}

স্থির শূন্য shared_hash_চাবি_initialize(শূন্য) {
        random_bytes(shared_hash_চাবি, আকার(shared_hash_চাবি));
}

স্থির কাঠামো HashmapBase* hashmap_base_new(ধ্রুবক কাঠামো hash_ops *hash_ops, গণনা HashmapType type) {
        HashmapBase *h;
        ধ্রুবক কাঠামো hashmap_type_info *hi = &hashmap_type_info[type];

        bool use_pool = mempool_enabled && mempool_enabled();  /* mempool_enabled is a weak symbol */

        h = use_pool ? mempool_alloc0_tile(hi->mempool) : malloc0(hi->শীর্ষ_আকার);
        যদি (!h)
                ফেরত ফাঁকা;

        h->type = type;
        h->from_pool = use_pool;
        h->hash_ops = hash_ops ?: &trivial_hash_ops;

        যদি (type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*)h;
                lh->iterate_list_শীর্ষ = lh->iterate_list_লেজ = IDX_NIL;
        }

        reset_direct_storage(h);

        স্থির pthread_once_t once = PTHREAD_ONCE_INIT;
        assert_se(pthread_once(&once, shared_hash_চাবি_initialize) == 0);

#যদি ENABLE_DEBUG_HASHMAP
        assert_se(pthread_mutex_lock(&hashmap_debug_list_mutex) == 0);
        LIST_PREPEND(debug_list, hashmap_debug_list, &h->debug);
        assert_se(pthread_mutex_unlock(&hashmap_debug_list_mutex) == 0);
#endif

        ফেরত h;
}

Hashmap *hashmap_new(ধ্রুবক কাঠামো hash_ops *hash_ops) {
        ফেরত (Hashmap*)        hashmap_base_new(hash_ops, HASHMAP_TYPE_PLAIN);
}

OrderedHashmap *ordered_hashmap_new(ধ্রুবক কাঠামো hash_ops *hash_ops) {
        ফেরত (OrderedHashmap*) hashmap_base_new(hash_ops, HASHMAP_TYPE_ORDERED);
}

Set *set_new(ধ্রুবক কাঠামো hash_ops *hash_ops) {
        ফেরত (Set*)            hashmap_base_new(hash_ops, HASHMAP_TYPE_SET);
}

স্থির পূর্ণসংখ্যা hashmap_base_ensure_allocated(HashmapBase **h, ধ্রুবক কাঠামো hash_ops *hash_ops,
                                         গণনা HashmapType type) {
        HashmapBase *q;

        প্রতিজ্ঞা(h);

        যদি (*h) {
                প্রতিজ্ঞা((*h)->hash_ops == (hash_ops ?: &trivial_hash_ops));
                ফেরত 0;
        }

        q = hashmap_base_new(hash_ops, type);
        যদি (!q)
                ফেরত -ENOMEM;

        *h = q;
        ফেরত 1;
}

পূর্ণসংখ্যা hashmap_ensure_allocated(Hashmap **h, ধ্রুবক কাঠামো hash_ops *hash_ops) {
        ফেরত hashmap_base_ensure_allocated((HashmapBase**)h, hash_ops, HASHMAP_TYPE_PLAIN);
}

পূর্ণসংখ্যা ordered_hashmap_ensure_allocated(OrderedHashmap **h, ধ্রুবক কাঠামো hash_ops *hash_ops) {
        ফেরত hashmap_base_ensure_allocated((HashmapBase**)h, hash_ops, HASHMAP_TYPE_ORDERED);
}

পূর্ণসংখ্যা set_ensure_allocated(Set **s, ধ্রুবক কাঠামো hash_ops *hash_ops) {
        ফেরত hashmap_base_ensure_allocated((HashmapBase**)s, hash_ops, HASHMAP_TYPE_SET);
}

পূর্ণসংখ্যা hashmap_ensure_put(Hashmap **h, ধ্রুবক কাঠামো hash_ops *hash_ops, ধ্রুবক শূন্য *চাবি, শূন্য *মান) {
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(h);

        r = hashmap_ensure_allocated(h, hash_ops);
        যদি (r < 0)
                ফেরত r;

        ফেরত hashmap_put(*h, চাবি, মান);
}

পূর্ণসংখ্যা ordered_hashmap_ensure_put(OrderedHashmap **h, ধ্রুবক কাঠামো hash_ops *hash_ops, ধ্রুবক শূন্য *চাবি, শূন্য *মান) {
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(h);

        r = ordered_hashmap_ensure_allocated(h, hash_ops);
        যদি (r < 0)
                ফেরত r;

        ফেরত ordered_hashmap_put(*h, চাবি, মান);
}

পূর্ণসংখ্যা ordered_hashmap_ensure_replace(OrderedHashmap **h, ধ্রুবক কাঠামো hash_ops *hash_ops, ধ্রুবক শূন্য *চাবি, শূন্য *মান) {
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(h);

        r = ordered_hashmap_ensure_allocated(h, hash_ops);
        যদি (r < 0)
                ফেরত r;

        ফেরত ordered_hashmap_replace(*h, চাবি, মান);
}

পূর্ণসংখ্যা hashmap_ensure_replace(Hashmap **h, ধ্রুবক কাঠামো hash_ops *hash_ops, ধ্রুবক শূন্য *চাবি, শূন্য *মান) {
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(h);

        r = hashmap_ensure_allocated(h, hash_ops);
        যদি (r < 0)
                ফেরত r;

        ফেরত hashmap_replace(*h, চাবি, মান);
}

স্থির শূন্য hashmap_free_no_clear(HashmapBase *h) {
        প্রতিজ্ঞা(!h->has_indirect);
        প্রতিজ্ঞা(h->n_direct_entries == 0);

#যদি ENABLE_DEBUG_HASHMAP
        assert_se(pthread_mutex_lock(&hashmap_debug_list_mutex) == 0);
        LIST_REMOVE(debug_list, hashmap_debug_list, &h->debug);
        assert_se(pthread_mutex_unlock(&hashmap_debug_list_mutex) == 0);
#endif

        যদি (h->from_pool) {
                /* Ensure that the object didn't get migrated between threads. */
                assert_se(is_main_thread());
                mempool_free_tile(hashmap_type_info[h->type].mempool, h);
        } অন্যথা
                মুক্ত(h);
}

HashmapBase* _hashmap_free(HashmapBase *h) {
        যদি (h) {
                _hashmap_clear(h);
                hashmap_free_no_clear(h);
        }

        ফেরত ফাঁকা;
}

শূন্য _hashmap_clear(HashmapBase *h) {
        যদি (!h)
                ফেরত;

        যদি (h->hash_ops->free_চাবি || h->hash_ops->free_মান) {

                /* If destructor calls are defined, let's destroy things defensively: let's take the item out of the
                 * hash table, and only then call the destructor functions. If these destructors then try to unregister
                 * themselves from our hash table a second time, the entry is already gone. */

                যখন (hashmap_আকার(h) > 0) {
                        শূন্য *k = ফাঁকা;
                        শূন্য *v;

                        v = hashmap_first_চাবি_and_মান(h, true, &k);

                        যদি (h->hash_ops->free_চাবি)
                                h->hash_ops->free_চাবি(k);

                        যদি (h->hash_ops->free_মান)
                                h->hash_ops->free_মান(v);
                }
        }

        যদি (h->has_indirect) {
                মুক্ত(h->indirect.storage);
                h->has_indirect = false;
        }

        h->n_direct_entries = 0;
        reset_direct_storage(h);

        যদি (h->type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*) h;
                lh->iterate_list_শীর্ষ = lh->iterate_list_লেজ = IDX_NIL;
        }

        base_set_dirty(h);
}

স্থির পূর্ণসংখ্যা resize_buckets(HashmapBase *h, unsigned entries_add);

/*
 * Finds an empty bucket to put an entry into, starting the scan at 'idx'.
 * Performs Robin Hood swaps as it goes. The entry to put must be placed
 * by the caller into swap slot IDX_PUT.
 * If used for in-place resizing, may leave a displaced entry in swap slot
 * IDX_PUT. Caller must rehash it next.
 * Returns: true if it left a displaced entry to rehash next in IDX_PUT,
 *          false otherwise.
 */
স্থির bool hashmap_put_robin_hood(HashmapBase *h, unsigned idx,
                                   কাঠামো swap_entries *swap) {
        dib_raw_t raw_dib, *dibs;
        unsigned dib, distance;

#যদি ENABLE_DEBUG_HASHMAP
        প্রতিজ্ঞা(h);
        h->debug.put_গুনতি++;
#endif

        dibs = dib_raw_ptr(h);

        জন্য (distance = 0; ; distance++) {
                raw_dib = dibs[idx];
                যদি (IN_SET(raw_dib, DIB_RAW_FREE, DIB_RAW_REHASH)) {
                        যদি (raw_dib == DIB_RAW_REHASH)
                                bucket_move_entry(h, swap, idx, IDX_TMP);

                        যদি (h->has_indirect && h->indirect.idx_lowest_entry > idx)
                                h->indirect.idx_lowest_entry = idx;

                        bucket_set_dib(h, idx, distance);
                        bucket_move_entry(h, swap, IDX_PUT, idx);
                        যদি (raw_dib == DIB_RAW_REHASH) {
                                bucket_move_entry(h, swap, IDX_TMP, IDX_PUT);
                                ফেরত true;
                        }

                        ফেরত false;
                }

                dib = bucket_calculate_dib(h, idx, raw_dib);

                যদি (dib < distance) {
                        /* Found a wealthier entry. Go Robin Hood! */
                        bucket_set_dib(h, idx, distance);

                        /* swap the entries */
                        bucket_move_entry(h, swap, idx, IDX_TMP);
                        bucket_move_entry(h, swap, IDX_PUT, idx);
                        bucket_move_entry(h, swap, IDX_TMP, IDX_PUT);

                        distance = dib;
                }

                idx = পরবর্তী_idx(h, idx);
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
স্থির পূর্ণসংখ্যা hashmap_base_put_boldly(HashmapBase *h, unsigned idx,
                                   কাঠামো swap_entries *swap, bool may_resize) {
        কাঠামো ordered_hashmap_entry *new_entry;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(idx < n_buckets(h));

        new_entry = bucket_at_swap(swap, IDX_PUT);

        যদি (may_resize) {
                r = resize_buckets(h, 1);
                যদি (r < 0)
                        ফেরত r;
                যদি (r > 0)
                        idx = bucket_hash(h, new_entry->p.b.চাবি);
        }
        প্রতিজ্ঞা(n_entries(h) < n_buckets(h));

        যদি (h->type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*) h;

                new_entry->iterate_পরবর্তী = IDX_NIL;
                new_entry->iterate_previous = lh->iterate_list_লেজ;

                যদি (lh->iterate_list_লেজ != IDX_NIL) {
                        কাঠামো ordered_hashmap_entry *old_লেজ;

                        old_লেজ = ordered_bucket_at(lh, lh->iterate_list_লেজ);
                        প্রতিজ্ঞা(old_লেজ->iterate_পরবর্তী == IDX_NIL);
                        old_লেজ->iterate_পরবর্তী = IDX_PUT;
                }

                lh->iterate_list_লেজ = IDX_PUT;
                যদি (lh->iterate_list_শীর্ষ == IDX_NIL)
                        lh->iterate_list_শীর্ষ = IDX_PUT;
        }

        assert_se(hashmap_put_robin_hood(h, idx, swap) == false);

        n_entries_inc(h);
#যদি ENABLE_DEBUG_HASHMAP
        h->debug.max_entries = MAX(h->debug.max_entries, n_entries(h));
#endif

        base_set_dirty(h);

        ফেরত 1;
}
#define hashmap_put_boldly(h, idx, swap, may_resize) \
        hashmap_base_put_boldly(HASHMAP_BASE(h), idx, swap, may_resize)

/*
 * Returns 0 if resize is not needed.
 *         1 if successfully resized.
 *         -ENOMEM on allocation failure.
 */
স্থির পূর্ণসংখ্যা resize_buckets(HashmapBase *h, unsigned entries_add) {
        কাঠামো swap_entries swap;
        শূন্য *new_storage;
        dib_raw_t *old_dibs, *new_dibs;
        ধ্রুবক কাঠামো hashmap_type_info *hi;
        unsigned idx, optimal_idx;
        unsigned old_n_buckets, new_n_buckets, n_rehashed, new_n_entries;
        uint8_t new_shift;
        bool rehash_পরবর্তী;

        প্রতিজ্ঞা(h);

        hi = &hashmap_type_info[h->type];
        new_n_entries = n_entries(h) + entries_add;

        /* overflow? */
        যদি (_unlikely_(new_n_entries < entries_add))
                ফেরত -ENOMEM;

        /* For direct storage we allow 100% load, because it's tiny. */
        যদি (!h->has_indirect && new_n_entries <= hi->n_direct_buckets)
                ফেরত 0;

        /*
         * Load factor = n/m = 1 - (1/INV_KEEP_FREE).
         * From it follows: m = n + n/(INV_KEEP_FREE - 1)
         */
        new_n_buckets = new_n_entries + new_n_entries / (INV_KEEP_FREE - 1);
        /* overflow? */
        যদি (_unlikely_(new_n_buckets < new_n_entries))
                ফেরত -ENOMEM;

        old_n_buckets = n_buckets(h);

        যদি (_likely_(new_n_buckets <= old_n_buckets))
                ফেরত 0;

        যদি (_unlikely_(new_n_buckets > UINT_MAX / (hi->entry_আকার + আকার(dib_raw_t))))
                ফেরত -ENOMEM;

        new_shift = log2u_round_up(MAX(
                        new_n_buckets * (hi->entry_আকার + আকার(dib_raw_t)),
                        2 * আকার(কাঠামো direct_storage)));

        /* Realloc storage (buckets and DIB array). */
        new_storage = realloc(h->has_indirect ? h->indirect.storage : ফাঁকা,
                              1U << new_shift);
        যদি (!new_storage)
                ফেরত -ENOMEM;

        /* Must upgrade direct to indirect storage. */
        যদি (!h->has_indirect) {
                memcpy(new_storage, h->direct.storage,
                       old_n_buckets * (hi->entry_আকার + আকার(dib_raw_t)));
                h->indirect.n_entries = h->n_direct_entries;
                h->indirect.idx_lowest_entry = 0;
                h->n_direct_entries = 0;
        }

        /* Get a new hash key. If we've just upgraded to indirect storage,
         * allow reusing a previously generated key. It's still a different key
         * from the shared one that we used for direct storage. */
        get_hash_চাবি(h->indirect.hash_চাবি, !h->has_indirect);

        h->has_indirect = true;
        h->indirect.storage = new_storage;
        h->indirect.n_buckets = (1U << new_shift) /
                                (hi->entry_আকার + আকার(dib_raw_t));

        old_dibs = (dib_raw_t*)((uint8_t*) new_storage + hi->entry_আকার * old_n_buckets);
        new_dibs = dib_raw_ptr(h);

        /*
         * Move the DIB array to the new place, replacing valid DIB values with
         * DIB_RAW_REHASH to indicate all of the used buckets need rehashing.
         * Note: Overlap is not possible, because we have at least doubled the
         * number of buckets and dib_raw_t is smaller than any entry type.
         */
        জন্য (idx = 0; idx < old_n_buckets; idx++) {
                প্রতিজ্ঞা(old_dibs[idx] != DIB_RAW_REHASH);
                new_dibs[idx] = old_dibs[idx] == DIB_RAW_FREE ? DIB_RAW_FREE
                                                              : DIB_RAW_REHASH;
        }

        /* Zero the area of newly added entries (including the old DIB area) */
        memzero(bucket_at(h, old_n_buckets),
               (n_buckets(h) - old_n_buckets) * hi->entry_আকার);

        /* The upper half of the new DIB array needs initialization */
        memset(&new_dibs[old_n_buckets], DIB_RAW_INIT,
               (n_buckets(h) - old_n_buckets) * আকার(dib_raw_t));

        /* Rehash entries that need it */
        n_rehashed = 0;
        জন্য (idx = 0; idx < old_n_buckets; idx++) {
                যদি (new_dibs[idx] != DIB_RAW_REHASH)
                        চালু;

                optimal_idx = bucket_hash(h, bucket_at(h, idx)->চাবি);

                /*
                 * Not much to do if by luck the entry hashes to its current
                 * location. Just set its DIB.
                 */
                যদি (optimal_idx == idx) {
                        new_dibs[idx] = 0;
                        n_rehashed++;
                        চালু;
                }

                new_dibs[idx] = DIB_RAW_FREE;
                bucket_move_entry(h, &swap, idx, IDX_PUT);
                /* bucket_move_entry does not clear the source */
                memzero(bucket_at(h, idx), hi->entry_আকার);

                করো {
                        /*
                         * Find the new bucket for the current entry. This may make
                         * another entry homeless and load it into IDX_PUT.
                         */
                        rehash_পরবর্তী = hashmap_put_robin_hood(h, optimal_idx, &swap);
                        n_rehashed++;

                        /* Did the current entry displace another one? */
                        যদি (rehash_পরবর্তী)
                                optimal_idx = bucket_hash(h, bucket_at_swap(&swap, IDX_PUT)->p.b.চাবি);
                } যখন (rehash_পরবর্তী);
        }

        assert_se(n_rehashed == n_entries(h));

        ফেরত 1;
}

/*
 * Finds an entry with a matching key
 * Returns: index of the found entry, or IDX_NIL if not found.
 */
স্থির unsigned base_bucket_scan(HashmapBase *h, unsigned idx, ধ্রুবক শূন্য *চাবি) {
        compare_func_t compare = h->hash_ops->compare;
        কাঠামো hashmap_base_entry *e;
        unsigned dib, distance;
        dib_raw_t *dibs = dib_raw_ptr(h);

        প্রতিজ্ঞা(idx < n_buckets(h));

        জন্য (distance = 0; ; distance++) {
                যদি (dibs[idx] == DIB_RAW_FREE)
                        ফেরত IDX_NIL;

                dib = bucket_calculate_dib(h, idx, dibs[idx]);

                যদি (dib < distance)
                        ফেরত IDX_NIL;
                যদি (dib == distance) {
                        e = bucket_at(h, idx);

                        /* Most maps compare keys by pointer. Do that here, without the indirect call. */
                        যদি (compare == trivial_compare_func ? e->চাবি == চাবি : compare(e->চাবি, চাবি) == 0)
                                ফেরত idx;
                }

                idx = পরবর্তী_idx(h, idx);
        }
}
#define bucket_scan(h, idx, চাবি) base_bucket_scan(HASHMAP_BASE(h), idx, চাবি)

পূর্ণসংখ্যা hashmap_put(Hashmap *h, ধ্রুবক শূন্য *চাবি, শূন্য *মান) {
        কাঠামো swap_entries swap;
        কাঠামো plain_hashmap_entry *e;
        unsigned hash, idx;

        প্রতিজ্ঞা(h);

        hash = bucket_hash(h, চাবি);
        idx = bucket_scan(h, hash, চাবি);
        যদি (idx != IDX_NIL) {
                e = plain_bucket_at(h, idx);
                যদি (e->মান == মান)
                        ফেরত 0;
                ফেরত -EEXIST;
        }

        e = &bucket_at_swap(&swap, IDX_PUT)->p;
        e->b.চাবি = চাবি;
        e->মান = মান;
        ফেরত hashmap_put_boldly(h, hash, &swap, true);
}

পূর্ণসংখ্যা set_put(Set *s, ধ্রুবক শূন্য *চাবি) {
        কাঠামো swap_entries swap;
        কাঠামো hashmap_base_entry *e;
        unsigned hash, idx;

        প্রতিজ্ঞা(s);

        hash = bucket_hash(s, চাবি);
        idx = bucket_scan(s, hash, চাবি);
        যদি (idx != IDX_NIL)
                ফেরত 0;

        e = &bucket_at_swap(&swap, IDX_PUT)->p.b;
        e->চাবি = চাবি;
        ফেরত hashmap_put_boldly(s, hash, &swap, true);
}

পূর্ণসংখ্যা set_ensure_put(Set **s, ধ্রুবক কাঠামো hash_ops *hash_ops, ধ্রুবক শূন্য *চাবি) {
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(s);

        r = set_ensure_allocated(s, hash_ops);
        যদি (r < 0)
                ফেরত r;

        ফেরত set_put(*s, চাবি);
}

পূর্ণসংখ্যা set_ensure_consume(Set **s, ধ্রুবক কাঠামো hash_ops *hash_ops, শূন্য *চাবি) {
        পূর্ণসংখ্যা r;

        r = set_ensure_put(s, hash_ops, চাবি);
        যদি (r <= 0) {
                যদি (hash_ops && hash_ops->free_চাবি)
                        hash_ops->free_চাবি(চাবি);
                অন্যথা যদি (hash_ops && hash_ops->free_মান)
                        /* Sets store their element in the key slot but may carry a value destructor. */
                        hash_ops->free_মান(চাবি);
                অন্যথা
                        মুক্ত(চাবি);
        }

        ফেরত r;
}

পূর্ণসংখ্যা hashmap_replace(Hashmap *h, ধ্রুবক শূন্য *চাবি, শূন্য *মান) {
        কাঠামো swap_entries swap;
        কাঠামো plain_hashmap_entry *e;
        unsigned hash, idx;

        প্রতিজ্ঞা(h);

        hash = bucket_hash(h, চাবি);
        idx = bucket_scan(h, hash, চাবি);
        যদি (idx != IDX_NIL) {
                e = plain_bucket_at(h, idx);
#যদি ENABLE_DEBUG_HASHMAP
                /* Although the key is equal, the key pointer may have changed,
                 * and this would break our assumption for iterating. So count
                 * this operation as incompatible with iteration. */
                যদি (e->b.চাবি != চাবি) {
                        h->b.debug.put_গুনতি++;
                        h->b.debug.rem_গুনতি++;
                        h->b.debug.last_rem_idx = idx;
                }
#endif
                e->b.চাবি = চাবি;
                e->মান = মান;
                hashmap_set_dirty(h);

                ফেরত 0;
        }

        e = &bucket_at_swap(&swap, IDX_PUT)->p;
        e->b.চাবি = চাবি;
        e->মান = মান;
        ফেরত hashmap_put_boldly(h, hash, &swap, true);
}

পূর্ণসংখ্যা hashmap_update(Hashmap *h, ধ্রুবক শূন্য *চাবি, শূন্য *মান) {
        কাঠামো plain_hashmap_entry *e;
        unsigned hash, idx;

        প্রতিজ্ঞা(h);

        hash = bucket_hash(h, চাবি);
        idx = bucket_scan(h, hash, চাবি);
        যদি (idx == IDX_NIL)
                ফেরত -ENOENT;

        e = plain_bucket_at(h, idx);
        e->মান = মান;
        hashmap_set_dirty(h);

        ফেরত 0;
}

শূন্য* _hashmap_get(HashmapBase *h, ধ্রুবক শূন্য *চাবি) {
        কাঠামো hashmap_base_entry *e;
        unsigned hash, idx;

        যদি (!h)
                ফেরত ফাঁকা;

        hash = bucket_hash(h, চাবি);
        idx = bucket_scan(h, hash, চাবি);
        যদি (idx == IDX_NIL)
                ফেরত ফাঁকা;

        e = bucket_at(h, idx);
        ফেরত entry_মান(h, e);
}

শূন্য* hashmap_get2(Hashmap *h, ধ্রুবক শূন্য *চাবি, শূন্য **ret) {
        কাঠামো plain_hashmap_entry *e;
        unsigned hash, idx;

        যদি (!h)
                ফেরত ফাঁকা;

        hash = bucket_hash(h, চাবি);
        idx = bucket_scan(h, hash, চাবি);
        যদি (idx == IDX_NIL)
                ফেরত ফাঁকা;

        e = plain_bucket_at(h, idx);
        যদি (ret)
                *ret = (শূন্য*) e->b.চাবি;

        ফেরত e->মান;
}

bool _hashmap_contains(HashmapBase *h, ধ্রুবক শূন্য *চাবি) {
        unsigned hash;

        যদি (!h)
                ফেরত false;

        hash = bucket_hash(h, চাবি);
        ফেরত bucket_scan(h, hash, চাবি) != IDX_NIL;
}

শূন্য* _hashmap_remove(HashmapBase *h, ধ্রুবক শূন্য *চাবি) {
        কাঠামো hashmap_base_entry *e;
        unsigned hash, idx;
        শূন্য *তথ্য;

        যদি (!h)
                ফেরত ফাঁকা;

        hash = bucket_hash(h, চাবি);
        idx = bucket_scan(h, hash, চাবি);
        যদি (idx == IDX_NIL)
                ফেরত ফাঁকা;

        e = bucket_at(h, idx);
        তথ্য = entry_মান(h, e);
        remove_entry(h, idx);

        ফেরত তথ্য;
}

শূন্য* hashmap_remove2(Hashmap *h, ধ্রুবক শূন্য *চাবি, শূন্য **ret) {
        কাঠামো plain_hashmap_entry *e;
        unsigned hash, idx;
        শূন্য *তথ্য;

        যদি (!h) {
                যদি (ret)
                        *ret = ফাঁকা;
                ফেরত ফাঁকা;
        }

        hash = bucket_hash(h, চাবি);
        idx = bucket_scan(h, hash, চাবি);
        যদি (idx == IDX_NIL) {
                যদি (ret)
                        *ret = ফাঁকা;
                ফেরত ফাঁকা;
        }

        e = plain_bucket_at(h, idx);
        তথ্য = e->মান;
        যদি (ret)
                *ret = (শূন্য*) e->b.চাবি;

        remove_entry(h, idx);

        ফেরত তথ্য;
}

পূর্ণসংখ্যা hashmap_remove_and_put(Hashmap *h, ধ্রুবক শূন্য *old_চাবি, ধ্রুবক শূন্য *new_চাবি, শূন্য *মান) {
        কাঠামো swap_entries swap;
        কাঠামো plain_hashmap_entry *e;
        unsigned old_hash, new_hash, idx;

        যদি (!h)
                ফেরত -ENOENT;

        old_hash = bucket_hash(h, old_চাবি);
        idx = bucket_scan(h, old_hash, old_চাবি);
        যদি (idx == IDX_NIL)
                ফেরত -ENOENT;

        new_hash = bucket_hash(h, new_চাবি);
        যদি (bucket_scan(h, new_hash, new_চাবি) != IDX_NIL)
                ফেরত -EEXIST;

        remove_entry(h, idx);

        e = &bucket_at_swap(&swap, IDX_PUT)->p;
        e->b.চাবি = new_চাবি;
        e->মান = মান;
        assert_se(hashmap_put_boldly(h, new_hash, &swap, false) == 1);

        ফেরত 0;
}

পূর্ণসংখ্যা set_remove_and_put(Set *s, ধ্রুবক শূন্য *old_চাবি, ধ্রুবক শূন্য *new_চাবি) {
        কাঠামো swap_entries swap;
        কাঠামো hashmap_base_entry *e;
        unsigned old_hash, new_hash, idx;

        যদি (!s)
                ফেরত -ENOENT;

        old_hash = bucket_hash(s, old_চাবি);
        idx = bucket_scan(s, old_hash, old_চাবি);
        যদি (idx == IDX_NIL)
                ফেরত -ENOENT;

        new_hash = bucket_hash(s, new_চাবি);
        যদি (bucket_scan(s, new_hash, new_চাবি) != IDX_NIL)
                ফেরত -EEXIST;

        remove_entry(s, idx);

        e = &bucket_at_swap(&swap, IDX_PUT)->p.b;
        e->চাবি = new_চাবি;
        assert_se(hashmap_put_boldly(s, new_hash, &swap, false) == 1);

        ফেরত 0;
}

পূর্ণসংখ্যা hashmap_remove_and_replace(Hashmap *h, ধ্রুবক শূন্য *old_চাবি, ধ্রুবক শূন্য *new_চাবি, শূন্য *মান) {
        কাঠামো swap_entries swap;
        কাঠামো plain_hashmap_entry *e;
        unsigned old_hash, new_hash, idx_old, idx_new;

        যদি (!h)
                ফেরত -ENOENT;

        old_hash = bucket_hash(h, old_চাবি);
        idx_old = bucket_scan(h, old_hash, old_চাবি);
        যদি (idx_old == IDX_NIL)
                ফেরত -ENOENT;

        old_চাবি = bucket_at(HASHMAP_BASE(h), idx_old)->চাবি;

        new_hash = bucket_hash(h, new_চাবি);
        idx_new = bucket_scan(h, new_hash, new_চাবি);
        যদি (idx_new != IDX_NIL)
                যদি (idx_old != idx_new) {
                        remove_entry(h, idx_new);
                        /* Compensate for a possible backward shift. */
                        যদি (old_চাবি != bucket_at(HASHMAP_BASE(h), idx_old)->চাবি)
                                idx_old = পূর্ব_idx(HASHMAP_BASE(h), idx_old);
                        প্রতিজ্ঞা(old_চাবি == bucket_at(HASHMAP_BASE(h), idx_old)->চাবি);
                }

        remove_entry(h, idx_old);

        e = &bucket_at_swap(&swap, IDX_PUT)->p;
        e->b.চাবি = new_চাবি;
        e->মান = মান;
        assert_se(hashmap_put_boldly(h, new_hash, &swap, false) == 1);

        ফেরত 0;
}

শূন্য* hashmap_remove_মান(HashmapBase *h, ধ্রুবক শূন্য *চাবি, শূন্য *মান) {
        কাঠামো hashmap_base_entry *e;
        unsigned hash, idx;

        যদি (!h)
                ফেরত ফাঁকা;

        hash = bucket_hash(h, চাবি);
        idx = bucket_scan(h, hash, চাবি);
        যদি (idx == IDX_NIL)
                ফেরত ফাঁকা;

        e = bucket_at(h, idx);
        যদি (entry_মান(h, e) != মান)
                ফেরত ফাঁকা;

        remove_entry(h, idx);

        ফেরত মান;
}

স্থির unsigned find_first_entry(HashmapBase *h) {
        Iterator i = ITERATOR_FIRST;

        যদি (!h || !n_entries(h))
                ফেরত IDX_NIL;

        ফেরত hashmap_iterate_entry(h, &i);
}

শূন্য* hashmap_first_চাবি_and_মান(HashmapBase *h, bool remove, শূন্য **ret_চাবি) {
        কাঠামো hashmap_base_entry *e;
        শূন্য *চাবি, *তথ্য;
        unsigned idx;

        idx = find_first_entry(h);
        যদি (idx == IDX_NIL) {
                যদি (ret_চাবি)
                        *ret_চাবি = ফাঁকা;
                ফেরত ফাঁকা;
        }

        e = bucket_at(h, idx);
        চাবি = (শূন্য*) e->চাবি;
        তথ্য = entry_মান(h, e);

        যদি (remove)
                remove_entry(h, idx);

        যদি (ret_চাবি)
                *ret_চাবি = চাবি;

        ফেরত তথ্য;
}

unsigned hashmap_আকার(HashmapBase *h) {
        যদি (!h)
                ফেরত 0;

        ফেরত n_entries(h);
}

unsigned _hashmap_buckets(HashmapBase *h) {
        যদি (!h)
                ফেরত 0;

        ফেরত n_buckets(h);
}

পূর্ণসংখ্যা _hashmap_merge(Hashmap *h, Hashmap *other) {
        Iterator i;
        unsigned idx;

        প্রতিজ্ঞা(h);

        HASHMAP_FOREACH_IDX(idx, HASHMAP_BASE(other), i) {
                কাঠামো plain_hashmap_entry *pe = plain_bucket_at(other, idx);
                পূর্ণসংখ্যা r;

                r = hashmap_put(h, pe->b.চাবি, pe->মান);
                যদি (r < 0 && r != -EEXIST)
                        ফেরত r;
        }

        ফেরত 0;
}

পূর্ণসংখ্যা set_merge(Set *s, Set *other) {
        Iterator i;
        unsigned idx;

        প্রতিজ্ঞা(s);

        HASHMAP_FOREACH_IDX(idx, HASHMAP_BASE(other), i) {
                কাঠামো set_entry *se = set_bucket_at(other, idx);
                পূর্ণসংখ্যা r;

                r = set_put(s, se->b.চাবি);
                যদি (r < 0)
                        ফেরত r;
        }

        ফেরত 0;
}

পূর্ণসংখ্যা _hashmap_reserve(HashmapBase *h, unsigned entries_add) {
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(h);

        r = resize_buckets(h, entries_add);
        যদি (r < 0)
                ফেরত r;

        ফেরত 0;
}

/*
 * The same as hashmap_merge(), but every new item from other is moved to h.
 * Keys already in h are skipped and stay in other.
 * Returns: 0 on success.
 *          -ENOMEM on alloc failure, in which case no move has been done.
 */
পূর্ণসংখ্যা _hashmap_move(HashmapBase *h, HashmapBase *other) {
        কাঠামো swap_entries swap;
        কাঠামো hashmap_base_entry *e, *n;
        Iterator i;
        unsigned idx;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(h);

        যদি (!other)
                ফেরত 0;

        প্রতিজ্ঞা(other->type == h->type);

        /*
         * This reserves buckets for the worst case, where none of other's
         * entries are yet present in h. This is preferable to risking
         * an allocation failure in the middle of the moving and having to
         * rollback or return a partial result.
         */
        r = resize_buckets(h, n_entries(other));
        যদি (r < 0)
                ফেরত r;

        HASHMAP_FOREACH_IDX(idx, other, i) {
                unsigned h_hash;

                e = bucket_at(other, idx);
                h_hash = bucket_hash(h, e->চাবি);
                যদি (bucket_scan(h, h_hash, e->চাবি) != IDX_NIL)
                        চালু;

                n = &bucket_at_swap(&swap, IDX_PUT)->p.b;
                n->চাবি = e->চাবি;
                যদি (h->type != HASHMAP_TYPE_SET)
                        ((কাঠামো plain_hashmap_entry*) n)->মান =
                                ((কাঠামো plain_hashmap_entry*) e)->মান;
                assert_se(hashmap_put_boldly(h, h_hash, &swap, false) == 1);

                remove_entry(other, idx);
        }

        ফেরত 0;
}

পূর্ণসংখ্যা _hashmap_move_one(HashmapBase *h, HashmapBase *other, ধ্রুবক শূন্য *চাবি) {
        কাঠামো swap_entries swap;
        unsigned h_hash, other_hash, idx;
        কাঠামো hashmap_base_entry *e, *n;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(h);

        h_hash = bucket_hash(h, চাবি);
        যদি (bucket_scan(h, h_hash, চাবি) != IDX_NIL)
                ফেরত -EEXIST;

        যদি (!other)
                ফেরত -ENOENT;

        প্রতিজ্ঞা(other->type == h->type);

        other_hash = bucket_hash(other, চাবি);
        idx = bucket_scan(other, other_hash, চাবি);
        যদি (idx == IDX_NIL)
                ফেরত -ENOENT;

        e = bucket_at(other, idx);

        n = &bucket_at_swap(&swap, IDX_PUT)->p.b;
        n->চাবি = e->চাবি;
        যদি (h->type != HASHMAP_TYPE_SET)
                ((কাঠামো plain_hashmap_entry*) n)->মান =
                        ((কাঠামো plain_hashmap_entry*) e)->মান;
        r = hashmap_put_boldly(h, h_hash, &swap, true);
        যদি (r < 0)
                ফেরত r;

        remove_entry(other, idx);
        ফেরত 0;
}

HashmapBase* _hashmap_copy(HashmapBase *h) {
        HashmapBase *copy;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(h);

        copy = hashmap_base_new(h->hash_ops, h->type);
        যদি (!copy)
                ফেরত ফাঁকা;

        নির্বাচন (h->type) {
        ক্ষেত্র HASHMAP_TYPE_PLAIN:
        ক্ষেত্র HASHMAP_TYPE_ORDERED:
                r = hashmap_merge((Hashmap*)copy, (Hashmap*)h);
                বিরতি;
        ক্ষেত্র HASHMAP_TYPE_SET:
                r = set_merge((Set*)copy, (Set*)h);
                বিরতি;
        পূর্বনির্ধারিত:
                assert_not_reached();
        }

        যদি (r < 0)
                ফেরত _hashmap_free(copy);

        ফেরত copy;
}

অক্ষর** _hashmap_get_strv(HashmapBase *h) {
        অক্ষর **sv;
        Iterator i;
        unsigned idx, n;

        যদি (!h)
                ফেরত new0(অক্ষর*, 1);

        sv = new(অক্ষর*, n_entries(h)+1);
        যদি (!sv)
                ফেরত ফাঁকা;

        n = 0;
        HASHMAP_FOREACH_IDX(idx, h, i)
                sv[n++] = entry_মান(h, bucket_at(h, idx));
        sv[n] = ফাঁকা;

        ফেরত sv;
}

অক্ষর** set_to_strv(Set **s) {
        প্রতিজ্ঞা(s);

        /* This is similar to set_get_strv(), but invalidates the set on success. */

        অক্ষর **v = new(অক্ষর*, set_আকার(*s) + 1);
        যদি (!v)
                ফেরত ফাঁকা;

        জন্য (অক্ষর **p = v; (*p = set_steal_first(*s)); p++)
                ;

        প্রতিজ্ঞা(set_isempty(*s));
        *s = set_free(*s);
        ফেরত v;
}

শূন্য* ordered_hashmap_পরবর্তী(OrderedHashmap *h, ধ্রুবক শূন্য *চাবি) {
        কাঠামো ordered_hashmap_entry *e;
        unsigned hash, idx;

        যদি (!h)
                ফেরত ফাঁকা;

        hash = bucket_hash(h, চাবি);
        idx = bucket_scan(h, hash, চাবি);
        যদি (idx == IDX_NIL)
                ফেরত ফাঁকা;

        e = ordered_bucket_at(h, idx);
        যদি (e->iterate_পরবর্তী == IDX_NIL)
                ফেরত ফাঁকা;
        ফেরত ordered_bucket_at(h, e->iterate_পরবর্তী)->p.মান;
}

পূর্ণসংখ্যা set_consume(Set *s, শূন্য *মান) {
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(s);
        প্রতিজ্ঞা(মান);

        r = set_put(s, মান);
        যদি (r <= 0)
                মুক্ত(মান);

        ফেরত r;
}

পূর্ণসংখ্যা hashmap_put_strdup_full(Hashmap **h, ধ্রুবক কাঠামো hash_ops *hash_ops, ধ্রুবক অক্ষর *k, ধ্রুবক অক্ষর *v) {
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(h);

        r = hashmap_ensure_allocated(h, hash_ops);
        যদি (r < 0)
                ফেরত r;

        _cleanup_free_ অক্ষর *kdup = ফাঁকা, *vdup = ফাঁকা;

        kdup = strdup(k);
        যদি (!kdup)
                ফেরত -ENOMEM;

        যদি (v) {
                vdup = strdup(v);
                যদি (!vdup)
                        ফেরত -ENOMEM;
        }

        r = hashmap_put(*h, kdup, vdup);
        যদি (r < 0) {
                যদি (r == -EEXIST && streq_ptr(v, hashmap_get(*h, kdup)))
                        ফেরত 0;
                ফেরত r;
        }

        /* 0 with non-null vdup would mean vdup is already in the hashmap, which cannot be */
        প্রতিজ্ঞা(vdup == ফাঁকা || r > 0);
        যদি (r > 0)
                kdup = vdup = ফাঁকা;

        ফেরত r;
}

পূর্ণসংখ্যা set_put_strndup_full(Set **s, ধ্রুবক কাঠামো hash_ops *hash_ops, ধ্রুবক অক্ষর *p, size_t n) {
        অক্ষর *c;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(s);
        প্রতিজ্ঞা(p);

        r = set_ensure_allocated(s, hash_ops);
        যদি (r < 0)
                ফেরত r;

        যদি (n == SIZE_MAX) {
                যদি (set_contains(*s, (অক্ষর*) p))
                        ফেরত 0;

                c = strdup(p);
        } অন্যথা
                c = strndup(p, n);
        যদি (!c)
                ফেরত -ENOMEM;

        ফেরত set_consume(*s, c);
}

পূর্ণসংখ্যা set_put_strdupv_full(Set **s, ধ্রুবক কাঠামো hash_ops *hash_ops, অক্ষর **l) {
        পূর্ণসংখ্যা n = 0, r;

        প্রতিজ্ঞা(s);

        STRV_FOREACH(i, l) {
                r = set_put_strndup_full(s, hash_ops, *i, SIZE_MAX);
                যদি (r < 0)
                        ফেরত r;

                n += r;
        }

        ফেরত n;
}

পূর্ণসংখ্যা set_put_strsplit(Set *s, ধ্রুবক অক্ষর *v, ধ্রুবক অক্ষর *separators, ExtractFlags flags) {
        ধ্রুবক অক্ষর *p = ASSERT_PTR(v);
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(s);

        জন্য (;;) {
                অক্ষর *word;

                r = extract_first_word(&p, &word, separators, flags);
                যদি (r <= 0)
                        ফেরত r;

                r = set_consume(s, word);
                যদি (r < 0)
                        ফেরত r;
        }
}

/* expand the cachemem if needed, return true if newly (re)activated. */
স্থির পূর্ণসংখ্যা cachemem_maintain(CacheMem *mem, size_t আকার_2) {
        প্রতিজ্ঞা(mem);

        যদি (!GREEDY_REALLOC(mem->ptr, আকার_2)) {
                যদি (আকার_2 > 0)
                        ফেরত -ENOMEM;
        }

        যদি (!mem->active) {
                mem->active = true;
                ফেরত true;
        }

        ফেরত false;
}

পূর্ণসংখ্যা iterated_cache_get(IteratedCache *cache, ধ্রুবক শূন্য ***res_চাবিগুলি, ধ্রুবক শূন্য ***res_মানসমূহ, unsigned *res_n_entries) {
        bool sync_চাবিগুলি = false, sync_মানসমূহ = false;
        size_t আকার_2;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(cache);
        প্রতিজ্ঞা(cache->hashmap);

        আকার_2 = n_entries(cache->hashmap);

        যদি (res_চাবিগুলি) {
                r = cachemem_maintain(&cache->চাবিগুলি, আকার_2);
                যদি (r < 0)
                        ফেরত r;

                sync_চাবিগুলি = r;
        } অন্যথা
                cache->চাবিগুলি.active = false;

        যদি (res_মানসমূহ) {
                r = cachemem_maintain(&cache->values, আকার_2);
                যদি (r < 0)
                        ফেরত r;

                sync_মানসমূহ = r;
        } অন্যথা
                cache->values.active = false;

        যদি (cache->hashmap->dirty) {
                যদি (cache->চাবিগুলি.active)
                        sync_চাবিগুলি = true;
                যদি (cache->values.active)
                        sync_মানসমূহ = true;

                cache->hashmap->dirty = false;
        }

        যদি (sync_চাবিগুলি || sync_মানসমূহ) {
                unsigned i, idx;
                Iterator iter;

                i = 0;
                HASHMAP_FOREACH_IDX(idx, cache->hashmap, iter) {
                        কাঠামো hashmap_base_entry *e;

                        e = bucket_at(cache->hashmap, idx);

                        যদি (sync_চাবিগুলি)
                                cache->চাবিগুলি.ptr[i] = e->চাবি;
                        যদি (sync_মানসমূহ)
                                cache->values.ptr[i] = entry_মান(cache->hashmap, e);
                        i++;
                }
        }

        যদি (res_চাবিগুলি)
                *res_চাবিগুলি = cache->চাবিগুলি.ptr;
        যদি (res_মানসমূহ)
                *res_মানসমূহ = cache->values.ptr;
        যদি (res_n_entries)
                *res_n_entries = আকার_2;

        ফেরত 0;
}

IteratedCache* iterated_cache_free(IteratedCache *cache) {
        যদি (cache) {
                মুক্ত(cache->চাবিগুলি.ptr);
                মুক্ত(cache->values.ptr);
        }

        ফেরত mfree(cache);
}

পূর্ণসংখ্যা set_strjoin(Set *s, ধ্রুবক অক্ষর *separator, bool wrap_with_separator, অক্ষর **ret) {
        _cleanup_free_ অক্ষর *str = ফাঁকা;
        size_t separator_len, len = 0;
        ধ্রুবক অক্ষর *মান;
        bool first;

        প্রতিজ্ঞা(ret);

        যদি (set_isempty(s)) {
                *ret = ফাঁকা;
                ফেরত 0;
        }

        separator_len = strlen_ptr(separator);

        যদি (separator_len == 0)
                wrap_with_separator = false;

        first = !wrap_with_separator;

        SET_FOREACH(মান, s) {
                size_t l = strlen_ptr(মান);

                যদি (l == 0)
                        চালু;

                যদি (!GREEDY_REALLOC(str, len + l + (first ? 0 : separator_len) + (wrap_with_separator ? separator_len : 0) + 1))
                        ফেরত -ENOMEM;

                যদি (separator_len > 0 && !first) {
                        memcpy(str + len, separator, separator_len);
                        len += separator_len;
                }

                memcpy(str + len, মান, l);
                len += l;
                first = false;
        }

        যদি (wrap_with_separator) {
                memcpy(str + len, separator, separator_len);
                len += separator_len;
        }

        str[len] = '\0';

        *ret = TAKE_PTR(str);
        ফেরত 0;
}

bool set_equal(Set *a, Set *b) {
        শূন্য *p;

        /* Checks whether each entry of 'a' is also in 'b' and vice versa, i.e. the two sets contain the same
         * entries */

        যদি (a == b)
                ফেরত true;

        যদি (set_isempty(a) && set_isempty(b))
                ফেরত true;

        যদি (set_আকার(a) != set_আকার(b)) /* Cheap check that hopefully catches a lot of inequality cases
                                         * already */
                ফেরত false;

        SET_FOREACH(p, a)
                যদি (!set_contains(b, p))
                        ফেরত false;

        /* If we have the same hashops, then we don't need to check things backwards given we compared the
         * size and that all of a is in b. */
        যদি (a->b.hash_ops == b->b.hash_ops)
                ফেরত true;

        SET_FOREACH(p, b)
                যদি (!set_contains(a, p))
                        ফেরত false;

        ফেরত true;
}

স্থির bool set_fnmatch_one(Set *patterns, ধ্রুবক অক্ষর *needle) {
        ধ্রুবক অক্ষর *p;

        প্রতিজ্ঞা(needle);

        /* Any failure of fnmatch() is treated as equivalent to FNM_NOMATCH, i.e. as non-matching pattern */

        SET_FOREACH(p, patterns)
                যদি (fnmatch(p, needle, 0) == 0)
                        ফেরত true;

        ফেরত false;
}

bool set_fnmatch(Set *include_patterns, Set *exclude_patterns, ধ্রুবক অক্ষর *needle) {
        প্রতিজ্ঞা(needle);

        যদি (set_fnmatch_one(exclude_patterns, needle))
                ফেরত false;

        যদি (set_isempty(include_patterns))
                ফেরত true;

        ফেরত set_fnmatch_one(include_patterns, needle);
}

স্থির পূর্ণসংখ্যা hashmap_entry_compare(
                কাঠামো hashmap_base_entry * ধ্রুবক *a,
                কাঠামো hashmap_base_entry * ধ্রুবক *b,
                compare_func_t compare) {

        প্রতিজ্ঞা(a && *a);
        প্রতিজ্ঞা(b && *b);
        প্রতিজ্ঞা(compare);

        ফেরত compare((*a)->চাবি, (*b)->চাবি);
}

স্থির পূর্ণসংখ্যা _hashmap_dump_entries_sorted(
                HashmapBase *h,
                শূন্য ***ret,
                size_t *ret_n) {
        _cleanup_free_ শূন্য **entries = ফাঁকা;
        Iterator iter;
        unsigned idx;
        size_t n = 0;

        প্রতিজ্ঞা(ret);
        প্রতিজ্ঞা(ret_n);

        যদি (hashmap_আকার(h) == 0) {
                *ret = ফাঁকা;
                *ret_n = 0;
                ফেরত 0;
        }

        /* We append one more element than needed so that the resulting array can be used as a strv. We
         * don't count this entry in the returned size. */
        entries = new(শূন্য*, hashmap_আকার(h) + 1);
        যদি (!entries)
                ফেরত -ENOMEM;

        HASHMAP_FOREACH_IDX(idx, h, iter)
                entries[n++] = bucket_at(h, idx);

        প্রতিজ্ঞা(n == hashmap_আকার(h));
        entries[n] = ফাঁকা;

        typesafe_qsort_r((কাঠামো hashmap_base_entry**) entries, n,
                         hashmap_entry_compare, h->hash_ops->compare);

        *ret = TAKE_PTR(entries);
        *ret_n = n;
        ফেরত 0;
}

পূর্ণসংখ্যা hashmap_dump_চাবিগুলি_sorted(HashmapBase *h, শূন্য ***ret, size_t *ret_n) {
        _cleanup_free_ শূন্য **entries = ফাঁকা;
        size_t n;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(ret);

        r = _hashmap_dump_entries_sorted(h, &entries, &n);
        যদি (r < 0)
                ফেরত r;

        /* Reuse the array. */
        FOREACH_ARRAY(e, entries, n)
                *e = (শূন্য*) (*(কাঠামো hashmap_base_entry**) e)->চাবি;

        *ret = TAKE_PTR(entries);
        যদি (ret_n)
                *ret_n = n;
        ফেরত 0;
}

পূর্ণসংখ্যা _hashmap_dump_sorted(HashmapBase *h, শূন্য ***ret, size_t *ret_n) {
        _cleanup_free_ শূন্য **entries = ফাঁকা;
        size_t n;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(ret);

        r = _hashmap_dump_entries_sorted(h, &entries, &n);
        যদি (r < 0)
                ফেরত r;

        /* Reuse the array. */
        FOREACH_ARRAY(e, entries, n)
                *e = entry_মান(h, *(কাঠামো hashmap_base_entry**) e);

        *ret = TAKE_PTR(entries);
        যদি (ret_n)
                *ret_n = n;
        ফেরত 0;
}
