/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ <fnmatch.h>
#અન્તર્ભાવ <pthread.h>
#અન્તર્ભાવ <unistd.h>
#જો HAVE_VALGRIND_VALGRIND_H
#  include <valgrind/valgrind.h>
#endif

#અન્તર્ભાવ "alloc-util.h"
#અન્તર્ભાવ "extract-word.h"
#અન્તર્ભાવ "hashmap.h"
#અન્તર્ભાવ "log.h"
#અન્તર્ભાવ "logarithm.h"
#અન્તર્ભાવ "memory-util.h"
#અન્તર્ભાવ "mempool.h"
#અન્તર્ભાવ "process-util.h"
#અન્તર્ભાવ "random-util.h"
#અન્તર્ભાવ "set.h"
#અન્તર્ભાવ "siphash24.h"
#અન્તર્ભાવ "sort-util.h"
#અન્તર્ભાવ "string-util.h"
#અન્તર્ભાવ "strv.h"

#જો ENABLE_DEBUG_HASHMAP
#અન્તર્ભાવ "list.h"
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
સંરચના hashmap_base_entry {
        અચલ શૂન્ય *ચાવી;
};

/* Entry types for specific hashmap/set types
 * hashmap_base_entry must be at the beginning of each entry struct. */

સંરચના plain_hashmap_entry {
        સંરચના hashmap_base_entry b;
        શૂન્ય *કિંમત;
};

સંરચના ordered_hashmap_entry {
        સંરચના plain_hashmap_entry p;
        unsigned iterate_next, iterate_previous;
};

સંરચના set_entry {
        સંરચના hashmap_base_entry b;
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
સંરચના swap_entries {
        સંરચના ordered_hashmap_entry e[_IDX_SWAP_END - _IDX_SWAP_BEGIN];
};

/* Distance from Initial Bucket */
પ્રકારવ્યાખ્યા uint8_t dib_raw_t;
#define DIB_RAW_OVERFLOW ((dib_raw_t)0xfdU)   /* indicates DIB value is greater than representable */
#define DIB_RAW_REHASH   ((dib_raw_t)0xfeU)   /* entry yet to be rehashed during in-place resize */
#define DIB_RAW_FREE     ((dib_raw_t)0xffU)   /* a free bucket */
#define DIB_RAW_INIT     ((અક્ષર)DIB_RAW_FREE) /* a byte to memset a DIB store with when initializing */

#define DIB_FREE UINT_MAX

#જો ENABLE_DEBUG_HASHMAP
સંરચના hashmap_debug_info {
        LIST_FIELDS(સંરચના hashmap_debug_info, debug_list);
        unsigned max_entries;  /* high watermark of n_entries */

        /* fields to detect modification while iterating */
        unsigned put_ગણતરી;    /* counts puts into the hashmap */
        unsigned rem_ગણતરી;    /* counts removals from hashmap */
        unsigned last_rem_idx; /* remembers last removal index */
};

/* Tracks all existing hashmaps. Get at it from gdb. See sd_dump_hashmaps.py */
સ્થિર LIST_HEAD(સંરચના hashmap_debug_info, hashmap_debug_list);
સ્થિર pthread_mutex_t hashmap_debug_list_mutex = PTHREAD_MUTEX_INITIALIZER;
#endif

ગણના HashmapType {
        HASHMAP_TYPE_PLAIN,
        HASHMAP_TYPE_ORDERED,
        HASHMAP_TYPE_SET,
        _HASHMAP_TYPE_MAX
};

સંરચના _packed_ indirect_storage {
        શૂન્ય *storage;                     /* where buckets and DIBs are stored */
        uint8_t  hash_ચાવી[HASH_ચાવી_SIZE];  /* hash key; changes during resize */

        unsigned n_entries;                /* number of stored entries */
        unsigned n_buckets;                /* number of buckets */

        unsigned idx_lowest_entry;         /* Index below which all buckets are free.
                                              Makes "while (hashmap_steal_first())" loops
                                              O(n) instead of O(n^2) for unordered hashmaps. */
        uint8_t  _pad[3];                  /* padding for the whole HashmapBase */
        /* The bitfields in HashmapBase complete the alignment of the whole thing. */
};

સંરચના direct_storage {
        /* This gives us 39 bytes on 64-bit, or 35 bytes on 32-bit.
         * That's room for 4 set_entries + 4 DIB bytes + 3 unused bytes on 64-bit,
         *              or 7 set_entries + 7 DIB bytes + 0 unused bytes on 32-bit. */
        uint8_t storage[કદ(સંરચના indirect_storage)];
};

#define DIRECT_BUCKETS(entry_t) \
        (કદ(સંરચના direct_storage) / (કદ(entry_t) + કદ(dib_raw_t)))

/* We should be able to store at least one entry directly. */
assert_cc(DIRECT_BUCKETS(સંરચના ordered_hashmap_entry) >= 1);

/* We have 3 bits for n_direct_entries. */
assert_cc(DIRECT_BUCKETS(સંરચના set_entry) < (1 << 3));

/* Hashmaps with directly stored entries all use this shared hash key.
 * It's no big deal if the key is guessed, because there can be only
 * a handful of directly stored entries in a hashmap. When a hashmap
 * outgrows direct storage, it gets its own key for indirect storage. */
સ્થિર uint8_t shared_hash_ચાવી[HASH_ચાવી_SIZE];

/* Fields that all hashmap/set types must have */
સંરચના HashmapBase {
        અચલ સંરચના hash_ops *hash_ops;  /* hash and compare ops to use */

        સંઘ _packed_ {
                સંરચના indirect_storage indirect; /* if  has_indirect */
                સંરચના direct_storage direct;     /* if !has_indirect */
        };

        ગણના HashmapType type:2;     /* HASHMAP_TYPE_* */
        bool has_indirect:1;         /* whether indirect storage is used */
        unsigned n_direct_entries:3; /* Number of entries in direct storage.
                                      * Only valid if !has_indirect. */
        bool from_pool:1;            /* whether was allocated from mempool */
        bool dirty:1;                /* whether dirtied since last iterated_cache_get() */
        bool cached:1;               /* whether this hashmap is being cached */

#જો ENABLE_DEBUG_HASHMAP
        સંરચના hashmap_debug_info debug;
#endif
};

/* Specific hash types
 * HashmapBase must be at the beginning of each hashmap struct. */

સંરચના Hashmap {
        સંરચના HashmapBase b;
};

સંરચના OrderedHashmap {
        સંરચના HashmapBase b;
        unsigned iterate_list_head, iterate_list_tail;
};

સંરચના Set {
        સંરચના HashmapBase b;
};

પ્રકારવ્યાખ્યા સંરચના CacheMem {
        અચલ શૂન્ય **ptr;
        size_t n_populated;
        bool active:1;
} CacheMem;

સંરચના IteratedCache {
        HashmapBase *hashmap;
        CacheMem ચાવીઓ, values;
};

DEFINE_MEMPOOL(hashmap_pool,         Hashmap,        8);
DEFINE_MEMPOOL(ordered_hashmap_pool, OrderedHashmap, 8);
/* No need for a separate Set pool */
assert_cc(કદ(Hashmap) == કદ(Set));

સંરચના hashmap_type_info {
        size_t head_size;
        size_t entry_size;
        સંરચના mempool *mempool;
        unsigned n_direct_buckets;
};

સ્થિર _used_ અચલ સંરચના hashmap_type_info hashmap_type_info[_HASHMAP_TYPE_MAX] = {
        [HASHMAP_TYPE_PLAIN] = {
                .head_size        = કદ(Hashmap),
                .entry_size       = કદ(સંરચના plain_hashmap_entry),
                .mempool          = &hashmap_pool,
                .n_direct_buckets = DIRECT_BUCKETS(સંરચના plain_hashmap_entry),
        },
        [HASHMAP_TYPE_ORDERED] = {
                .head_size        = કદ(OrderedHashmap),
                .entry_size       = કદ(સંરચના ordered_hashmap_entry),
                .mempool          = &ordered_hashmap_pool,
                .n_direct_buckets = DIRECT_BUCKETS(સંરચના ordered_hashmap_entry),
        },
        [HASHMAP_TYPE_SET] = {
                .head_size        = કદ(Set),
                .entry_size       = કદ(સંરચના set_entry),
                .mempool          = &hashmap_pool,
                .n_direct_buckets = DIRECT_BUCKETS(સંરચના set_entry),
        },
};

શૂન્ય hashmap_trim_pools(શૂન્ય) {
        પૂર્ણાંક r;

        /* The pool is only allocated by the main thread, but the memory can be passed to other
         * threads. Let's clean up if we are the main thread and no other threads are live. */

        /* We build our own is_main_thread() here, which doesn't use C11 TLS based caching of the
         * result. That's because valgrind apparently doesn't like TLS to be used from a GCC destructor. */
        જો (getpid() != gettid())
                પાછા (શૂન્ય) log_debug("Not cleaning up memory pools, not in main thread.");

        r = get_process_threads(0);
        જો (r < 0)
                પાછા (શૂન્ય) log_debug_errno(r, "Failed to determine number of threads, not cleaning up memory pools: %m");
        જો (r != 1)
                પાછા (શૂન્ય) log_debug("Not cleaning up memory pools, running in multi-threaded process.");

        mempool_trim(&hashmap_pool);
        mempool_trim(&ordered_hashmap_pool);
}

#જો HAVE_VALGRIND_VALGRIND_H
_destructor_ સ્થિર શૂન્ય cleanup_pools(શૂન્ય) {
        /* Be nice to valgrind */
        જો (RUNNING_ON_VALGRIND)
                hashmap_trim_pools();
}
#endif

સ્થિર અંતઃરેખ unsigned n_buckets(HashmapBase *h) {
        પ્રતિજ્ઞા(h);
        પાછા h->has_indirect ? h->indirect.n_buckets
                               : hashmap_type_info[h->type].n_direct_buckets;
}

સ્થિર unsigned n_entries(HashmapBase *h) {
        પ્રતિજ્ઞા(h);
        પાછા h->has_indirect ? h->indirect.n_entries
                               : h->n_direct_entries;
}

સ્થિર શૂન્ય n_entries_inc(HashmapBase *h) {
        પ્રતિજ્ઞા(h);

        જો (h->has_indirect)
                h->indirect.n_entries++;
        નહીંતર
                h->n_direct_entries++;
}

સ્થિર શૂન્ય n_entries_dec(HashmapBase *h) {
        પ્રતિજ્ઞા(h);

        જો (h->has_indirect)
                h->indirect.n_entries--;
        નહીંતર
                h->n_direct_entries--;
}

સ્થિર અંતઃરેખ શૂન્ય* storage_ptr(HashmapBase *h) {
        પ્રતિજ્ઞા(h);
        પાછા h->has_indirect ? h->indirect.storage
                               : h->direct.storage;
}

સ્થિર અંતઃરેખ uint8_t* hash_ચાવી(HashmapBase *h) {
        પ્રતિજ્ઞા(h);
        પાછા h->has_indirect ? h->indirect.hash_ચાવી
                               : shared_hash_ચાવી;
}

સ્થિર unsigned base_bucket_hash(HashmapBase *h, અચલ શૂન્ય *p) {
        સંરચના siphash state;
        uint64_t hash;

        પ્રતિજ્ઞા(h);

        /* The keys of trivial_hash_ops and uint64_hash_ops fit in 64 bits. siphash24_uint64() hashes such
         * a key without the indirect call through hash_ops and keeps the SipHash state in registers. On
         * 32-bit and big-endian architectures, the result differs from the result of trivial_hash_func(),
         * which hashes the key in native byte order. Only this function maps keys to buckets, so the
         * difference does not matter. */
        જો (h->hash_ops->hash == trivial_hash_func)
                hash = siphash24_uint64((uintptr_t) p, hash_ચાવી(h));
        નહીંતર જો (h->hash_ops->hash == (hash_func_t) uint64_hash_func)
                hash = siphash24_uint64(*(અચલ uint64_t*) p, hash_ચાવી(h));
        નહીંતર {
                siphash24_init(&state, hash_ચાવી(h));
                h->hash_ops->hash(p, &state);
                hash = siphash24_finalize(&state);
        }

        /* n_buckets is not a power of two, so a modulo needs a division. Multiplying the upper 32 bits of
         * the hash with n_buckets and keeping the upper 32 bits of the product maps the hash to
         * [0, n_buckets) almost as evenly, without a division. */
        પાછા (unsigned) (((hash >> 32) * n_buckets(h)) >> 32);
}
#define bucket_hash(h, p) base_bucket_hash(HASHMAP_BASE(h), p)

સ્થિર શૂન્ય base_set_dirty(HashmapBase *h) {
        પ્રતિજ્ઞા(h);

        h->dirty = true;
}
#define hashmap_set_dirty(h) base_set_dirty(HASHMAP_BASE(h))

સ્થિર શૂન્ય get_hash_ચાવી(uint8_t hash_ચાવી[HASH_ચાવી_SIZE], bool reuse_is_ok) {
        સ્થિર uint8_t current[HASH_ચાવી_SIZE];
        સ્થિર bool current_initialized = false;

        /* Returns a hash function key to use. In order to keep things
         * fast we will not generate a new key each time we allocate a
         * new hash table. Instead, we'll just reuse the most recently
         * generated one, except if we never generated one or when we
         * are rehashing an entire hash table because we reached a
         * fill level */

        જો (!current_initialized || !reuse_is_ok) {
                random_bytes(current, કદ(current));
                current_initialized = true;
        }

        memcpy(hash_ચાવી, current, કદ(current));
}

સ્થિર અંતઃરેખ સંરચના hashmap_base_entry* bucket_at(HashmapBase *h, unsigned idx) {
        પ્રતિજ્ઞા(h);
        પાછા CAST_ALIGN_PTR(
                        સંરચના hashmap_base_entry,
                        (uint8_t *) storage_ptr(h) + idx * hashmap_type_info[h->type].entry_size);
}

સ્થિર સંરચના plain_hashmap_entry* plain_bucket_at(Hashmap *h, unsigned idx) {
        પાછા (સંરચના plain_hashmap_entry*) bucket_at(HASHMAP_BASE(h), idx);
}

સ્થિર સંરચના ordered_hashmap_entry* ordered_bucket_at(OrderedHashmap *h, unsigned idx) {
        પાછા (સંરચના ordered_hashmap_entry*) bucket_at(HASHMAP_BASE(h), idx);
}

સ્થિર સંરચના set_entry *set_bucket_at(Set *h, unsigned idx) {
        પાછા (સંરચના set_entry*) bucket_at(HASHMAP_BASE(h), idx);
}

સ્થિર સંરચના ordered_hashmap_entry* bucket_at_swap(સંરચના swap_entries *swap, unsigned idx) {
        પ્રતિજ્ઞા(swap);
        પાછા &swap->e[idx - _IDX_SWAP_BEGIN];
}

/* Returns a pointer to the bucket at index idx.
 * Understands real indexes and swap indexes, hence "_virtual". */
સ્થિર સંરચના hashmap_base_entry* bucket_at_virtual(HashmapBase *h, સંરચના swap_entries *swap,
                                                    unsigned idx) {
        જો (idx < _IDX_SWAP_BEGIN)
                પાછા bucket_at(h, idx);

        જો (idx < _IDX_SWAP_END)
                પાછા &bucket_at_swap(swap, idx)->p.b;

        assert_not_reached();
}

સ્થિર અંતઃરેખ dib_raw_t* dib_raw_ptr(HashmapBase *h) {
        પ્રતિજ્ઞા(h);
        પાછા (dib_raw_t*)
                ((uint8_t*) storage_ptr(h) + hashmap_type_info[h->type].entry_size * n_buckets(h));
}

સ્થિર unsigned bucket_distance(HashmapBase *h, unsigned idx, unsigned from) {
        પાછા idx >= from ? idx - from
                           : n_buckets(h) + idx - from;
}

સ્થિર unsigned bucket_calculate_dib(HashmapBase *h, unsigned idx, dib_raw_t raw_dib) {
        unsigned initial_bucket;

        જો (raw_dib == DIB_RAW_FREE)
                પાછા DIB_FREE;

        જો (_likely_(raw_dib < DIB_RAW_OVERFLOW))
                પાછા raw_dib;

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
        initial_bucket = bucket_hash(h, bucket_at(h, idx)->ચાવી);
        પાછા bucket_distance(h, idx, initial_bucket);
}

સ્થિર શૂન્ય bucket_set_dib(HashmapBase *h, unsigned idx, unsigned dib) {
        dib_raw_ptr(h)[idx] = dib != DIB_FREE ? MIN(dib, DIB_RAW_OVERFLOW) : DIB_RAW_FREE;
}

સ્થિર unsigned skip_free_buckets(HashmapBase *h, unsigned idx) {
        dib_raw_t *dibs;

        dibs = dib_raw_ptr(h);

        માટે ( ; idx < n_buckets(h); idx++)
                જો (dibs[idx] != DIB_RAW_FREE)
                        પાછા idx;

        પાછા IDX_NIL;
}

સ્થિર શૂન્ય bucket_mark_free(HashmapBase *h, unsigned idx) {
        પ્રતિજ્ઞા(h);

        memzero(bucket_at(h, idx), hashmap_type_info[h->type].entry_size);
        bucket_set_dib(h, idx, DIB_FREE);
}

સ્થિર શૂન્ય bucket_move_entry(HashmapBase *h, સંરચના swap_entries *swap,
                              unsigned from, unsigned to) {
        સંરચના hashmap_base_entry *e_from, *e_to;

        પ્રતિજ્ઞા(h);
        પ્રતિજ્ઞા(from != to);

        e_from = bucket_at_virtual(h, swap, from);
        e_to   = bucket_at_virtual(h, swap, to);

        memcpy(e_to, e_from, hashmap_type_info[h->type].entry_size);

        જો (h->type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*) h;
                સંરચના ordered_hashmap_entry *le, *le_to;

                le_to = (સંરચના ordered_hashmap_entry*) e_to;

                જો (le_to->iterate_next != IDX_NIL) {
                        le = (સંરચના ordered_hashmap_entry*)
                             bucket_at_virtual(h, swap, le_to->iterate_next);
                        le->iterate_previous = to;
                }

                જો (le_to->iterate_previous != IDX_NIL) {
                        le = (સંરચના ordered_hashmap_entry*)
                             bucket_at_virtual(h, swap, le_to->iterate_previous);
                        le->iterate_next = to;
                }

                જો (lh->iterate_list_head == from)
                        lh->iterate_list_head = to;
                જો (lh->iterate_list_tail == from)
                        lh->iterate_list_tail = to;
        }
}

સ્થિર અંતઃરેખ unsigned next_idx(HashmapBase *h, unsigned idx) {
        પાછા idx + 1U < n_buckets(h) ? idx + 1U : 0U;
}

સ્થિર unsigned prev_idx(HashmapBase *h, unsigned idx) {
        પાછા idx > 0U ? idx - 1U : n_buckets(h) - 1U;
}

સ્થિર અંતઃરેખ શૂન્ય* entry_કિંમત(HashmapBase *h, સંરચના hashmap_base_entry *e) {
        પ્રતિજ્ઞા(h);
        પ્રતિજ્ઞા(e);

        પસંદગી (h->type) {

        સ્થિતિ HASHMAP_TYPE_PLAIN:
        સ્થિતિ HASHMAP_TYPE_ORDERED:
                પાછા ((સંરચના plain_hashmap_entry*)e)->કિંમત;

        સ્થિતિ HASHMAP_TYPE_SET:
                પાછા (શૂન્ય*) e->ચાવી;

        મૂળભૂત:
                assert_not_reached();
        }
}

સ્થિર શૂન્ય base_remove_entry(HashmapBase *h, unsigned idx) {
        unsigned ડાબું, જમણું, prev, dib;
        dib_raw_t raw_dib, *dibs;

        dibs = dib_raw_ptr(h);
        પ્રતિજ્ઞા(dibs[idx] != DIB_RAW_FREE);

#જો ENABLE_DEBUG_HASHMAP
        પ્રતિજ્ઞા(h);
        h->debug.rem_ગણતરી++;
        h->debug.last_rem_idx = idx;
#endif

        ડાબું = idx;
        /* Find the stop bucket ("right"). It is either free or has DIB == 0. */
        માટે (જમણું = next_idx(h, ડાબું); ; જમણું = next_idx(h, જમણું)) {
                raw_dib = dibs[જમણું];
                જો (IN_SET(raw_dib, 0, DIB_RAW_FREE))
                        તોડો;

                /* The buckets are not supposed to be all occupied and with DIB > 0.
                 * That would mean we could make everyone better off by shifting them
                 * backward. This scenario is impossible. */
                પ્રતિજ્ઞા(ડાબું != જમણું);
        }

        જો (h->type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*) h;
                સંરચના ordered_hashmap_entry *le = ordered_bucket_at(lh, idx);

                જો (le->iterate_next != IDX_NIL)
                        ordered_bucket_at(lh, le->iterate_next)->iterate_previous = le->iterate_previous;
                નહીંતર
                        lh->iterate_list_tail = le->iterate_previous;

                જો (le->iterate_previous != IDX_NIL)
                        ordered_bucket_at(lh, le->iterate_previous)->iterate_next = le->iterate_next;
                નહીંતર
                        lh->iterate_list_head = le->iterate_next;
        }

        /* Now shift all buckets in the interval (left, right) one step backwards */
        માટે (prev = ડાબું, ડાબું = next_idx(h, ડાબું); ડાબું != જમણું;
             prev = ડાબું, ડાબું = next_idx(h, ડાબું)) {
                dib = bucket_calculate_dib(h, ડાબું, dibs[ડાબું]);
                પ્રતિજ્ઞા(dib != 0);
                bucket_move_entry(h, ખાલી, ડાબું, prev);
                bucket_set_dib(h, prev, dib - 1);
        }

        bucket_mark_free(h, prev);
        n_entries_dec(h);
        base_set_dirty(h);
}
#define remove_entry(h, idx) base_remove_entry(HASHMAP_BASE(h), idx)

સ્થિર unsigned hashmap_iterate_in_insertion_order(OrderedHashmap *h, Iterator *i) {
        સંરચના ordered_hashmap_entry *e;
        unsigned idx;

        પ્રતિજ્ઞા(h);
        પ્રતિજ્ઞા(i);

        જો (i->idx == IDX_NIL)
                જાઓ at_end;

        જો (i->idx == IDX_FIRST && h->iterate_list_head == IDX_NIL)
                જાઓ at_end;

        જો (i->idx == IDX_FIRST) {
                idx = h->iterate_list_head;
                e = ordered_bucket_at(h, idx);
        } નહીંતર {
                idx = i->idx;
                e = ordered_bucket_at(h, idx);
                /*
                 * We allow removing the current entry while iterating, but removal may cause
                 * a backward shift. The next entry may thus move one bucket to the left.
                 * To detect when it happens, we remember the key pointer of the entry we were
                 * going to iterate next. If it does not match, there was a backward shift.
                 */
                જો (e->p.b.ચાવી != i->next_ચાવી) {
                        idx = prev_idx(HASHMAP_BASE(h), idx);
                        e = ordered_bucket_at(h, idx);
                }
                પ્રતિજ્ઞા(e->p.b.ચાવી == i->next_ચાવી);
        }

#જો ENABLE_DEBUG_HASHMAP
        i->prev_idx = idx;
#endif

        જો (e->iterate_next != IDX_NIL) {
                સંરચના ordered_hashmap_entry *n;
                i->idx = e->iterate_next;
                n = ordered_bucket_at(h, i->idx);
                i->next_ચાવી = n->p.b.ચાવી;
        } નહીંતર
                i->idx = IDX_NIL;

        પાછા idx;

at_end:
        i->idx = IDX_NIL;
        પાછા IDX_NIL;
}

સ્થિર unsigned hashmap_iterate_in_internal_order(HashmapBase *h, Iterator *i) {
        unsigned idx;

        પ્રતિજ્ઞા(h);
        પ્રતિજ્ઞા(i);

        જો (i->idx == IDX_NIL)
                જાઓ at_end;

        જો (i->idx == IDX_FIRST) {
                /* fast forward to the first occupied bucket */
                જો (h->has_indirect) {
                        i->idx = skip_free_buckets(h, h->indirect.idx_lowest_entry);
                        h->indirect.idx_lowest_entry = i->idx;
                } નહીંતર
                        i->idx = skip_free_buckets(h, 0);

                જો (i->idx == IDX_NIL)
                        જાઓ at_end;
        } નહીંતર {
                સંરચના hashmap_base_entry *e;

                પ્રતિજ્ઞા(i->idx > 0);

                e = bucket_at(h, i->idx);
                /*
                 * We allow removing the current entry while iterating, but removal may cause
                 * a backward shift. The next entry may thus move one bucket to the left.
                 * To detect when it happens, we remember the key pointer of the entry we were
                 * going to iterate next. If it does not match, there was a backward shift.
                 */
                જો (e->ચાવી != i->next_ચાવી)
                        e = bucket_at(h, --i->idx);

                પ્રતિજ્ઞા(e->ચાવી == i->next_ચાવી);
        }

        idx = i->idx;
#જો ENABLE_DEBUG_HASHMAP
        i->prev_idx = idx;
#endif

        i->idx = skip_free_buckets(h, i->idx + 1);
        જો (i->idx != IDX_NIL)
                i->next_ચાવી = bucket_at(h, i->idx)->ચાવી;
        નહીંતર
                i->idx = IDX_NIL;

        પાછા idx;

at_end:
        i->idx = IDX_NIL;
        પાછા IDX_NIL;
}

સ્થિર unsigned hashmap_iterate_entry(HashmapBase *h, Iterator *i) {
        જો (!h) {
                i->idx = IDX_NIL;
                પાછા IDX_NIL;
        }

#જો ENABLE_DEBUG_HASHMAP
        જો (i->idx == IDX_FIRST) {
                i->put_ગણતરી = h->debug.put_ગણતરી;
                i->rem_ગણતરી = h->debug.rem_ગણતરી;
        } નહીંતર {
                /* While iterating, must not add any new entries */
                પ્રતિજ્ઞા(i->put_ગણતરી == h->debug.put_ગણતરી);
                /* ... or remove entries other than the current one */
                પ્રતિજ્ઞા(i->rem_ગણતરી == h->debug.rem_ગણતરી ||
                       (i->rem_ગણતરી == h->debug.rem_ગણતરી - 1 &&
                        i->prev_idx == h->debug.last_rem_idx));
                /* Reset our removals counter */
                i->rem_ગણતરી = h->debug.rem_ગણતરી;
        }
#endif

        પાછા h->type == HASHMAP_TYPE_ORDERED ? hashmap_iterate_in_insertion_order((OrderedHashmap*) h, i)
                                               : hashmap_iterate_in_internal_order(h, i);
}

bool _hashmap_iterate(HashmapBase *h, Iterator *i, શૂન્ય **કિંમત, અચલ શૂન્ય **ચાવી) {
        સંરચના hashmap_base_entry *e;
        શૂન્ય *data;
        unsigned idx;

        idx = hashmap_iterate_entry(h, i);
        જો (idx == IDX_NIL) {
                જો (કિંમત)
                        *કિંમત = ખાલી;
                જો (ચાવી)
                        *ચાવી = ખાલી;

                પાછા false;
        }

        e = bucket_at(h, idx);
        data = entry_કિંમત(h, e);
        જો (કિંમત)
                *કિંમત = data;
        જો (ચાવી)
                *ચાવી = e->ચાવી;

        પાછા true;
}

#define HASHMAP_FOREACH_IDX(idx, h, i) \
        માટે ((i) = ITERATOR_FIRST, (idx) = hashmap_iterate_entry((h), &(i)); \
             (idx != IDX_NIL); \
             (idx) = hashmap_iterate_entry((h), &(i)))

IteratedCache* _hashmap_iterated_cache_new(HashmapBase *h) {
        IteratedCache *cache;

        પ્રતિજ્ઞા(h);
        પ્રતિજ્ઞા(!h->cached);

        જો (h->cached)
                પાછા ખાલી;

        cache = new0(IteratedCache, 1);
        જો (!cache)
                પાછા ખાલી;

        cache->hashmap = h;
        h->cached = true;

        પાછા cache;
}

સ્થિર શૂન્ય reset_direct_storage(HashmapBase *h) {
        અચલ સંરચના hashmap_type_info *hi = &hashmap_type_info[h->type];
        શૂન્ય *p;

        પ્રતિજ્ઞા(!h->has_indirect);

        p = mempset(h->direct.storage, 0, hi->entry_size * hi->n_direct_buckets);
        memset(p, DIB_RAW_INIT, કદ(dib_raw_t) * hi->n_direct_buckets);
}

સ્થિર શૂન્ય shared_hash_ચાવી_initialize(શૂન્ય) {
        random_bytes(shared_hash_ચાવી, કદ(shared_hash_ચાવી));
}

સ્થિર સંરચના HashmapBase* hashmap_base_new(અચલ સંરચના hash_ops *hash_ops, ગણના HashmapType type) {
        HashmapBase *h;
        અચલ સંરચના hashmap_type_info *hi = &hashmap_type_info[type];

        bool use_pool = mempool_enabled && mempool_enabled();  /* mempool_enabled is a weak symbol */

        h = use_pool ? mempool_alloc0_tile(hi->mempool) : malloc0(hi->head_size);
        જો (!h)
                પાછા ખાલી;

        h->type = type;
        h->from_pool = use_pool;
        h->hash_ops = hash_ops ?: &trivial_hash_ops;

        જો (type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*)h;
                lh->iterate_list_head = lh->iterate_list_tail = IDX_NIL;
        }

        reset_direct_storage(h);

        સ્થિર pthread_once_t once = PTHREAD_ONCE_INIT;
        assert_se(pthread_once(&once, shared_hash_ચાવી_initialize) == 0);

#જો ENABLE_DEBUG_HASHMAP
        assert_se(pthread_mutex_lock(&hashmap_debug_list_mutex) == 0);
        LIST_PREPEND(debug_list, hashmap_debug_list, &h->debug);
        assert_se(pthread_mutex_unlock(&hashmap_debug_list_mutex) == 0);
#endif

        પાછા h;
}

Hashmap *hashmap_new(અચલ સંરચના hash_ops *hash_ops) {
        પાછા (Hashmap*)        hashmap_base_new(hash_ops, HASHMAP_TYPE_PLAIN);
}

OrderedHashmap *ordered_hashmap_new(અચલ સંરચના hash_ops *hash_ops) {
        પાછા (OrderedHashmap*) hashmap_base_new(hash_ops, HASHMAP_TYPE_ORDERED);
}

Set *set_new(અચલ સંરચના hash_ops *hash_ops) {
        પાછા (Set*)            hashmap_base_new(hash_ops, HASHMAP_TYPE_SET);
}

સ્થિર પૂર્ણાંક hashmap_base_ensure_allocated(HashmapBase **h, અચલ સંરચના hash_ops *hash_ops,
                                         ગણના HashmapType type) {
        HashmapBase *q;

        પ્રતિજ્ઞા(h);

        જો (*h) {
                પ્રતિજ્ઞા((*h)->hash_ops == (hash_ops ?: &trivial_hash_ops));
                પાછા 0;
        }

        q = hashmap_base_new(hash_ops, type);
        જો (!q)
                પાછા -ENOMEM;

        *h = q;
        પાછા 1;
}

પૂર્ણાંક hashmap_ensure_allocated(Hashmap **h, અચલ સંરચના hash_ops *hash_ops) {
        પાછા hashmap_base_ensure_allocated((HashmapBase**)h, hash_ops, HASHMAP_TYPE_PLAIN);
}

પૂર્ણાંક ordered_hashmap_ensure_allocated(OrderedHashmap **h, અચલ સંરચના hash_ops *hash_ops) {
        પાછા hashmap_base_ensure_allocated((HashmapBase**)h, hash_ops, HASHMAP_TYPE_ORDERED);
}

પૂર્ણાંક set_ensure_allocated(Set **s, અચલ સંરચના hash_ops *hash_ops) {
        પાછા hashmap_base_ensure_allocated((HashmapBase**)s, hash_ops, HASHMAP_TYPE_SET);
}

પૂર્ણાંક hashmap_ensure_put(Hashmap **h, અચલ સંરચના hash_ops *hash_ops, અચલ શૂન્ય *ચાવી, શૂન્ય *કિંમત) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(h);

        r = hashmap_ensure_allocated(h, hash_ops);
        જો (r < 0)
                પાછા r;

        પાછા hashmap_put(*h, ચાવી, કિંમત);
}

પૂર્ણાંક ordered_hashmap_ensure_put(OrderedHashmap **h, અચલ સંરચના hash_ops *hash_ops, અચલ શૂન્ય *ચાવી, શૂન્ય *કિંમત) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(h);

        r = ordered_hashmap_ensure_allocated(h, hash_ops);
        જો (r < 0)
                પાછા r;

        પાછા ordered_hashmap_put(*h, ચાવી, કિંમત);
}

પૂર્ણાંક ordered_hashmap_ensure_replace(OrderedHashmap **h, અચલ સંરચના hash_ops *hash_ops, અચલ શૂન્ય *ચાવી, શૂન્ય *કિંમત) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(h);

        r = ordered_hashmap_ensure_allocated(h, hash_ops);
        જો (r < 0)
                પાછા r;

        પાછા ordered_hashmap_replace(*h, ચાવી, કિંમત);
}

પૂર્ણાંક hashmap_ensure_replace(Hashmap **h, અચલ સંરચના hash_ops *hash_ops, અચલ શૂન્ય *ચાવી, શૂન્ય *કિંમત) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(h);

        r = hashmap_ensure_allocated(h, hash_ops);
        જો (r < 0)
                પાછા r;

        પાછા hashmap_replace(*h, ચાવી, કિંમત);
}

સ્થિર શૂન્ય hashmap_free_no_clear(HashmapBase *h) {
        પ્રતિજ્ઞા(!h->has_indirect);
        પ્રતિજ્ઞા(h->n_direct_entries == 0);

#જો ENABLE_DEBUG_HASHMAP
        assert_se(pthread_mutex_lock(&hashmap_debug_list_mutex) == 0);
        LIST_REMOVE(debug_list, hashmap_debug_list, &h->debug);
        assert_se(pthread_mutex_unlock(&hashmap_debug_list_mutex) == 0);
#endif

        જો (h->from_pool) {
                /* Ensure that the object didn't get migrated between threads. */
                assert_se(is_main_thread());
                mempool_free_tile(hashmap_type_info[h->type].mempool, h);
        } નહીંતર
                મુક્ત(h);
}

HashmapBase* _hashmap_free(HashmapBase *h) {
        જો (h) {
                _hashmap_clear(h);
                hashmap_free_no_clear(h);
        }

        પાછા ખાલી;
}

શૂન્ય _hashmap_clear(HashmapBase *h) {
        જો (!h)
                પાછા;

        જો (h->hash_ops->free_ચાવી || h->hash_ops->free_કિંમત) {

                /* If destructor calls are defined, let's destroy things defensively: let's take the item out of the
                 * hash table, and only then call the destructor functions. If these destructors then try to unregister
                 * themselves from our hash table a second time, the entry is already gone. */

                જ્યારે (_hashmap_size(h) > 0) {
                        શૂન્ય *k = ખાલી;
                        શૂન્ય *v;

                        v = hashmap_first_ચાવી_and_કિંમત(h, true, &k);

                        જો (h->hash_ops->free_ચાવી)
                                h->hash_ops->free_ચાવી(k);

                        જો (h->hash_ops->free_કિંમત)
                                h->hash_ops->free_કિંમત(v);
                }
        }

        જો (h->has_indirect) {
                મુક્ત(h->indirect.storage);
                h->has_indirect = false;
        }

        h->n_direct_entries = 0;
        reset_direct_storage(h);

        જો (h->type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*) h;
                lh->iterate_list_head = lh->iterate_list_tail = IDX_NIL;
        }

        base_set_dirty(h);
}

સ્થિર પૂર્ણાંક resize_buckets(HashmapBase *h, unsigned entries_add);

/*
 * Finds an empty bucket to put an entry into, starting the scan at 'idx'.
 * Performs Robin Hood swaps as it goes. The entry to put must be placed
 * by the caller into swap slot IDX_PUT.
 * If used for in-place resizing, may leave a displaced entry in swap slot
 * IDX_PUT. Caller must rehash it next.
 * Returns: true if it left a displaced entry to rehash next in IDX_PUT,
 *          false otherwise.
 */
સ્થિર bool hashmap_put_robin_hood(HashmapBase *h, unsigned idx,
                                   સંરચના swap_entries *swap) {
        dib_raw_t raw_dib, *dibs;
        unsigned dib, distance;

#જો ENABLE_DEBUG_HASHMAP
        પ્રતિજ્ઞા(h);
        h->debug.put_ગણતરી++;
#endif

        dibs = dib_raw_ptr(h);

        માટે (distance = 0; ; distance++) {
                raw_dib = dibs[idx];
                જો (IN_SET(raw_dib, DIB_RAW_FREE, DIB_RAW_REHASH)) {
                        જો (raw_dib == DIB_RAW_REHASH)
                                bucket_move_entry(h, swap, idx, IDX_TMP);

                        જો (h->has_indirect && h->indirect.idx_lowest_entry > idx)
                                h->indirect.idx_lowest_entry = idx;

                        bucket_set_dib(h, idx, distance);
                        bucket_move_entry(h, swap, IDX_PUT, idx);
                        જો (raw_dib == DIB_RAW_REHASH) {
                                bucket_move_entry(h, swap, IDX_TMP, IDX_PUT);
                                પાછા true;
                        }

                        પાછા false;
                }

                dib = bucket_calculate_dib(h, idx, raw_dib);

                જો (dib < distance) {
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
સ્થિર પૂર્ણાંક hashmap_base_put_boldly(HashmapBase *h, unsigned idx,
                                   સંરચના swap_entries *swap, bool may_resize) {
        સંરચના ordered_hashmap_entry *new_entry;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(idx < n_buckets(h));

        new_entry = bucket_at_swap(swap, IDX_PUT);

        જો (may_resize) {
                r = resize_buckets(h, 1);
                જો (r < 0)
                        પાછા r;
                જો (r > 0)
                        idx = bucket_hash(h, new_entry->p.b.ચાવી);
        }
        પ્રતિજ્ઞા(n_entries(h) < n_buckets(h));

        જો (h->type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*) h;

                new_entry->iterate_next = IDX_NIL;
                new_entry->iterate_previous = lh->iterate_list_tail;

                જો (lh->iterate_list_tail != IDX_NIL) {
                        સંરચના ordered_hashmap_entry *old_tail;

                        old_tail = ordered_bucket_at(lh, lh->iterate_list_tail);
                        પ્રતિજ્ઞા(old_tail->iterate_next == IDX_NIL);
                        old_tail->iterate_next = IDX_PUT;
                }

                lh->iterate_list_tail = IDX_PUT;
                જો (lh->iterate_list_head == IDX_NIL)
                        lh->iterate_list_head = IDX_PUT;
        }

        assert_se(hashmap_put_robin_hood(h, idx, swap) == false);

        n_entries_inc(h);
#જો ENABLE_DEBUG_HASHMAP
        h->debug.max_entries = MAX(h->debug.max_entries, n_entries(h));
#endif

        base_set_dirty(h);

        પાછા 1;
}
#define hashmap_put_boldly(h, idx, swap, may_resize) \
        hashmap_base_put_boldly(HASHMAP_BASE(h), idx, swap, may_resize)

/*
 * Returns 0 if resize is not needed.
 *         1 if successfully resized.
 *         -ENOMEM on allocation failure.
 */
સ્થિર પૂર્ણાંક resize_buckets(HashmapBase *h, unsigned entries_add) {
        સંરચના swap_entries swap;
        શૂન્ય *new_storage;
        dib_raw_t *old_dibs, *new_dibs;
        અચલ સંરચના hashmap_type_info *hi;
        unsigned idx, optimal_idx;
        unsigned old_n_buckets, new_n_buckets, n_rehashed, new_n_entries;
        uint8_t new_shift;
        bool rehash_next;

        પ્રતિજ્ઞા(h);

        hi = &hashmap_type_info[h->type];
        new_n_entries = n_entries(h) + entries_add;

        /* overflow? */
        જો (_unlikely_(new_n_entries < entries_add))
                પાછા -ENOMEM;

        /* For direct storage we allow 100% load, because it's tiny. */
        જો (!h->has_indirect && new_n_entries <= hi->n_direct_buckets)
                પાછા 0;

        /*
         * Load factor = n/m = 1 - (1/INV_KEEP_FREE).
         * From it follows: m = n + n/(INV_KEEP_FREE - 1)
         */
        new_n_buckets = new_n_entries + new_n_entries / (INV_KEEP_FREE - 1);
        /* overflow? */
        જો (_unlikely_(new_n_buckets < new_n_entries))
                પાછા -ENOMEM;

        old_n_buckets = n_buckets(h);

        જો (_likely_(new_n_buckets <= old_n_buckets))
                પાછા 0;

        જો (_unlikely_(new_n_buckets > UINT_MAX / (hi->entry_size + કદ(dib_raw_t))))
                પાછા -ENOMEM;

        new_shift = log2u_round_up(MAX(
                        new_n_buckets * (hi->entry_size + કદ(dib_raw_t)),
                        2 * કદ(સંરચના direct_storage)));

        /* Realloc storage (buckets and DIB array). */
        new_storage = realloc(h->has_indirect ? h->indirect.storage : ખાલી,
                              1U << new_shift);
        જો (!new_storage)
                પાછા -ENOMEM;

        /* Must upgrade direct to indirect storage. */
        જો (!h->has_indirect) {
                memcpy(new_storage, h->direct.storage,
                       old_n_buckets * (hi->entry_size + કદ(dib_raw_t)));
                h->indirect.n_entries = h->n_direct_entries;
                h->indirect.idx_lowest_entry = 0;
                h->n_direct_entries = 0;
        }

        /* Get a new hash key. If we've just upgraded to indirect storage,
         * allow reusing a previously generated key. It's still a different key
         * from the shared one that we used for direct storage. */
        get_hash_ચાવી(h->indirect.hash_ચાવી, !h->has_indirect);

        h->has_indirect = true;
        h->indirect.storage = new_storage;
        h->indirect.n_buckets = (1U << new_shift) /
                                (hi->entry_size + કદ(dib_raw_t));

        old_dibs = (dib_raw_t*)((uint8_t*) new_storage + hi->entry_size * old_n_buckets);
        new_dibs = dib_raw_ptr(h);

        /*
         * Move the DIB array to the new place, replacing valid DIB values with
         * DIB_RAW_REHASH to indicate all of the used buckets need rehashing.
         * Note: Overlap is not possible, because we have at least doubled the
         * number of buckets and dib_raw_t is smaller than any entry type.
         */
        માટે (idx = 0; idx < old_n_buckets; idx++) {
                પ્રતિજ્ઞા(old_dibs[idx] != DIB_RAW_REHASH);
                new_dibs[idx] = old_dibs[idx] == DIB_RAW_FREE ? DIB_RAW_FREE
                                                              : DIB_RAW_REHASH;
        }

        /* Zero the area of newly added entries (including the old DIB area) */
        memzero(bucket_at(h, old_n_buckets),
               (n_buckets(h) - old_n_buckets) * hi->entry_size);

        /* The upper half of the new DIB array needs initialization */
        memset(&new_dibs[old_n_buckets], DIB_RAW_INIT,
               (n_buckets(h) - old_n_buckets) * કદ(dib_raw_t));

        /* Rehash entries that need it */
        n_rehashed = 0;
        માટે (idx = 0; idx < old_n_buckets; idx++) {
                જો (new_dibs[idx] != DIB_RAW_REHASH)
                        ચાલુ;

                optimal_idx = bucket_hash(h, bucket_at(h, idx)->ચાવી);

                /*
                 * Not much to do if by luck the entry hashes to its current
                 * location. Just set its DIB.
                 */
                જો (optimal_idx == idx) {
                        new_dibs[idx] = 0;
                        n_rehashed++;
                        ચાલુ;
                }

                new_dibs[idx] = DIB_RAW_FREE;
                bucket_move_entry(h, &swap, idx, IDX_PUT);
                /* bucket_move_entry does not clear the source */
                memzero(bucket_at(h, idx), hi->entry_size);

                કરો {
                        /*
                         * Find the new bucket for the current entry. This may make
                         * another entry homeless and load it into IDX_PUT.
                         */
                        rehash_next = hashmap_put_robin_hood(h, optimal_idx, &swap);
                        n_rehashed++;

                        /* Did the current entry displace another one? */
                        જો (rehash_next)
                                optimal_idx = bucket_hash(h, bucket_at_swap(&swap, IDX_PUT)->p.b.ચાવી);
                } જ્યારે (rehash_next);
        }

        assert_se(n_rehashed == n_entries(h));

        પાછા 1;
}

/*
 * Finds an entry with a matching key
 * Returns: index of the found entry, or IDX_NIL if not found.
 */
સ્થિર unsigned base_bucket_scan(HashmapBase *h, unsigned idx, અચલ શૂન્ય *ચાવી) {
        compare_func_t compare = h->hash_ops->compare;
        સંરચના hashmap_base_entry *e;
        unsigned dib, distance;
        dib_raw_t *dibs = dib_raw_ptr(h);

        પ્રતિજ્ઞા(idx < n_buckets(h));

        માટે (distance = 0; ; distance++) {
                જો (dibs[idx] == DIB_RAW_FREE)
                        પાછા IDX_NIL;

                dib = bucket_calculate_dib(h, idx, dibs[idx]);

                જો (dib < distance)
                        પાછા IDX_NIL;
                જો (dib == distance) {
                        e = bucket_at(h, idx);

                        /* Most maps compare keys by pointer. Do that here, without the indirect call. */
                        જો (compare == trivial_compare_func ? e->ચાવી == ચાવી : compare(e->ચાવી, ચાવી) == 0)
                                પાછા idx;
                }

                idx = next_idx(h, idx);
        }
}
#define bucket_scan(h, idx, ચાવી) base_bucket_scan(HASHMAP_BASE(h), idx, ચાવી)

પૂર્ણાંક hashmap_put(Hashmap *h, અચલ શૂન્ય *ચાવી, શૂન્ય *કિંમત) {
        સંરચના swap_entries swap;
        સંરચના plain_hashmap_entry *e;
        unsigned hash, idx;

        પ્રતિજ્ઞા(h);

        hash = bucket_hash(h, ચાવી);
        idx = bucket_scan(h, hash, ચાવી);
        જો (idx != IDX_NIL) {
                e = plain_bucket_at(h, idx);
                જો (e->કિંમત == કિંમત)
                        પાછા 0;
                પાછા -EEXIST;
        }

        e = &bucket_at_swap(&swap, IDX_PUT)->p;
        e->b.ચાવી = ચાવી;
        e->કિંમત = કિંમત;
        પાછા hashmap_put_boldly(h, hash, &swap, true);
}

પૂર્ણાંક set_put(Set *s, અચલ શૂન્ય *ચાવી) {
        સંરચના swap_entries swap;
        સંરચના hashmap_base_entry *e;
        unsigned hash, idx;

        પ્રતિજ્ઞા(s);

        hash = bucket_hash(s, ચાવી);
        idx = bucket_scan(s, hash, ચાવી);
        જો (idx != IDX_NIL)
                પાછા 0;

        e = &bucket_at_swap(&swap, IDX_PUT)->p.b;
        e->ચાવી = ચાવી;
        પાછા hashmap_put_boldly(s, hash, &swap, true);
}

પૂર્ણાંક set_ensure_put(Set **s, અચલ સંરચના hash_ops *hash_ops, અચલ શૂન્ય *ચાવી) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(s);

        r = set_ensure_allocated(s, hash_ops);
        જો (r < 0)
                પાછા r;

        પાછા set_put(*s, ચાવી);
}

પૂર્ણાંક set_ensure_consume(Set **s, અચલ સંરચના hash_ops *hash_ops, શૂન્ય *ચાવી) {
        પૂર્ણાંક r;

        r = set_ensure_put(s, hash_ops, ચાવી);
        જો (r <= 0) {
                જો (hash_ops && hash_ops->free_ચાવી)
                        hash_ops->free_ચાવી(ચાવી);
                નહીંતર જો (hash_ops && hash_ops->free_કિંમત)
                        /* Sets store their element in the key slot but may carry a value destructor. */
                        hash_ops->free_કિંમત(ચાવી);
                નહીંતર
                        મુક્ત(ચાવી);
        }

        પાછા r;
}

પૂર્ણાંક hashmap_replace(Hashmap *h, અચલ શૂન્ય *ચાવી, શૂન્ય *કિંમત) {
        સંરચના swap_entries swap;
        સંરચના plain_hashmap_entry *e;
        unsigned hash, idx;

        પ્રતિજ્ઞા(h);

        hash = bucket_hash(h, ચાવી);
        idx = bucket_scan(h, hash, ચાવી);
        જો (idx != IDX_NIL) {
                e = plain_bucket_at(h, idx);
#જો ENABLE_DEBUG_HASHMAP
                /* Although the key is equal, the key pointer may have changed,
                 * and this would break our assumption for iterating. So count
                 * this operation as incompatible with iteration. */
                જો (e->b.ચાવી != ચાવી) {
                        h->b.debug.put_ગણતરી++;
                        h->b.debug.rem_ગણતરી++;
                        h->b.debug.last_rem_idx = idx;
                }
#endif
                e->b.ચાવી = ચાવી;
                e->કિંમત = કિંમત;
                hashmap_set_dirty(h);

                પાછા 0;
        }

        e = &bucket_at_swap(&swap, IDX_PUT)->p;
        e->b.ચાવી = ચાવી;
        e->કિંમત = કિંમત;
        પાછા hashmap_put_boldly(h, hash, &swap, true);
}

પૂર્ણાંક hashmap_update(Hashmap *h, અચલ શૂન્ય *ચાવી, શૂન્ય *કિંમત) {
        સંરચના plain_hashmap_entry *e;
        unsigned hash, idx;

        પ્રતિજ્ઞા(h);

        hash = bucket_hash(h, ચાવી);
        idx = bucket_scan(h, hash, ચાવી);
        જો (idx == IDX_NIL)
                પાછા -ENOENT;

        e = plain_bucket_at(h, idx);
        e->કિંમત = કિંમત;
        hashmap_set_dirty(h);

        પાછા 0;
}

શૂન્ય* _hashmap_get(HashmapBase *h, અચલ શૂન્ય *ચાવી) {
        સંરચના hashmap_base_entry *e;
        unsigned hash, idx;

        જો (!h)
                પાછા ખાલી;

        hash = bucket_hash(h, ચાવી);
        idx = bucket_scan(h, hash, ચાવી);
        જો (idx == IDX_NIL)
                પાછા ખાલી;

        e = bucket_at(h, idx);
        પાછા entry_કિંમત(h, e);
}

શૂન્ય* hashmap_get2(Hashmap *h, અચલ શૂન્ય *ચાવી, શૂન્ય **ret) {
        સંરચના plain_hashmap_entry *e;
        unsigned hash, idx;

        જો (!h)
                પાછા ખાલી;

        hash = bucket_hash(h, ચાવી);
        idx = bucket_scan(h, hash, ચાવી);
        જો (idx == IDX_NIL)
                પાછા ખાલી;

        e = plain_bucket_at(h, idx);
        જો (ret)
                *ret = (શૂન્ય*) e->b.ચાવી;

        પાછા e->કિંમત;
}

bool _hashmap_contains(HashmapBase *h, અચલ શૂન્ય *ચાવી) {
        unsigned hash;

        જો (!h)
                પાછા false;

        hash = bucket_hash(h, ચાવી);
        પાછા bucket_scan(h, hash, ચાવી) != IDX_NIL;
}

શૂન્ય* _hashmap_remove(HashmapBase *h, અચલ શૂન્ય *ચાવી) {
        સંરચના hashmap_base_entry *e;
        unsigned hash, idx;
        શૂન્ય *data;

        જો (!h)
                પાછા ખાલી;

        hash = bucket_hash(h, ચાવી);
        idx = bucket_scan(h, hash, ચાવી);
        જો (idx == IDX_NIL)
                પાછા ખાલી;

        e = bucket_at(h, idx);
        data = entry_કિંમત(h, e);
        remove_entry(h, idx);

        પાછા data;
}

શૂન્ય* hashmap_remove2(Hashmap *h, અચલ શૂન્ય *ચાવી, શૂન્ય **ret) {
        સંરચના plain_hashmap_entry *e;
        unsigned hash, idx;
        શૂન્ય *data;

        જો (!h) {
                જો (ret)
                        *ret = ખાલી;
                પાછા ખાલી;
        }

        hash = bucket_hash(h, ચાવી);
        idx = bucket_scan(h, hash, ચાવી);
        જો (idx == IDX_NIL) {
                જો (ret)
                        *ret = ખાલી;
                પાછા ખાલી;
        }

        e = plain_bucket_at(h, idx);
        data = e->કિંમત;
        જો (ret)
                *ret = (શૂન્ય*) e->b.ચાવી;

        remove_entry(h, idx);

        પાછા data;
}

પૂર્ણાંક hashmap_remove_and_put(Hashmap *h, અચલ શૂન્ય *old_ચાવી, અચલ શૂન્ય *new_ચાવી, શૂન્ય *કિંમત) {
        સંરચના swap_entries swap;
        સંરચના plain_hashmap_entry *e;
        unsigned old_hash, new_hash, idx;

        જો (!h)
                પાછા -ENOENT;

        old_hash = bucket_hash(h, old_ચાવી);
        idx = bucket_scan(h, old_hash, old_ચાવી);
        જો (idx == IDX_NIL)
                પાછા -ENOENT;

        new_hash = bucket_hash(h, new_ચાવી);
        જો (bucket_scan(h, new_hash, new_ચાવી) != IDX_NIL)
                પાછા -EEXIST;

        remove_entry(h, idx);

        e = &bucket_at_swap(&swap, IDX_PUT)->p;
        e->b.ચાવી = new_ચાવી;
        e->કિંમત = કિંમત;
        assert_se(hashmap_put_boldly(h, new_hash, &swap, false) == 1);

        પાછા 0;
}

પૂર્ણાંક set_remove_and_put(Set *s, અચલ શૂન્ય *old_ચાવી, અચલ શૂન્ય *new_ચાવી) {
        સંરચના swap_entries swap;
        સંરચના hashmap_base_entry *e;
        unsigned old_hash, new_hash, idx;

        જો (!s)
                પાછા -ENOENT;

        old_hash = bucket_hash(s, old_ચાવી);
        idx = bucket_scan(s, old_hash, old_ચાવી);
        જો (idx == IDX_NIL)
                પાછા -ENOENT;

        new_hash = bucket_hash(s, new_ચાવી);
        જો (bucket_scan(s, new_hash, new_ચાવી) != IDX_NIL)
                પાછા -EEXIST;

        remove_entry(s, idx);

        e = &bucket_at_swap(&swap, IDX_PUT)->p.b;
        e->ચાવી = new_ચાવી;
        assert_se(hashmap_put_boldly(s, new_hash, &swap, false) == 1);

        પાછા 0;
}

પૂર્ણાંક hashmap_remove_and_replace(Hashmap *h, અચલ શૂન્ય *old_ચાવી, અચલ શૂન્ય *new_ચાવી, શૂન્ય *કિંમત) {
        સંરચના swap_entries swap;
        સંરચના plain_hashmap_entry *e;
        unsigned old_hash, new_hash, idx_old, idx_new;

        જો (!h)
                પાછા -ENOENT;

        old_hash = bucket_hash(h, old_ચાવી);
        idx_old = bucket_scan(h, old_hash, old_ચાવી);
        જો (idx_old == IDX_NIL)
                પાછા -ENOENT;

        old_ચાવી = bucket_at(HASHMAP_BASE(h), idx_old)->ચાવી;

        new_hash = bucket_hash(h, new_ચાવી);
        idx_new = bucket_scan(h, new_hash, new_ચાવી);
        જો (idx_new != IDX_NIL)
                જો (idx_old != idx_new) {
                        remove_entry(h, idx_new);
                        /* Compensate for a possible backward shift. */
                        જો (old_ચાવી != bucket_at(HASHMAP_BASE(h), idx_old)->ચાવી)
                                idx_old = prev_idx(HASHMAP_BASE(h), idx_old);
                        પ્રતિજ્ઞા(old_ચાવી == bucket_at(HASHMAP_BASE(h), idx_old)->ચાવી);
                }

        remove_entry(h, idx_old);

        e = &bucket_at_swap(&swap, IDX_PUT)->p;
        e->b.ચાવી = new_ચાવી;
        e->કિંમત = કિંમત;
        assert_se(hashmap_put_boldly(h, new_hash, &swap, false) == 1);

        પાછા 0;
}

શૂન્ય* hashmap_remove_કિંમત(HashmapBase *h, અચલ શૂન્ય *ચાવી, શૂન્ય *કિંમત) {
        સંરચના hashmap_base_entry *e;
        unsigned hash, idx;

        જો (!h)
                પાછા ખાલી;

        hash = bucket_hash(h, ચાવી);
        idx = bucket_scan(h, hash, ચાવી);
        જો (idx == IDX_NIL)
                પાછા ખાલી;

        e = bucket_at(h, idx);
        જો (entry_કિંમત(h, e) != કિંમત)
                પાછા ખાલી;

        remove_entry(h, idx);

        પાછા કિંમત;
}

સ્થિર unsigned find_first_entry(HashmapBase *h) {
        Iterator i = ITERATOR_FIRST;

        જો (!h || !n_entries(h))
                પાછા IDX_NIL;

        પાછા hashmap_iterate_entry(h, &i);
}

શૂન્ય* hashmap_first_ચાવી_and_કિંમત(HashmapBase *h, bool remove, શૂન્ય **ret_ચાવી) {
        સંરચના hashmap_base_entry *e;
        શૂન્ય *ચાવી, *data;
        unsigned idx;

        idx = find_first_entry(h);
        જો (idx == IDX_NIL) {
                જો (ret_ચાવી)
                        *ret_ચાવી = ખાલી;
                પાછા ખાલી;
        }

        e = bucket_at(h, idx);
        ચાવી = (શૂન્ય*) e->ચાવી;
        data = entry_કિંમત(h, e);

        જો (remove)
                remove_entry(h, idx);

        જો (ret_ચાવી)
                *ret_ચાવી = ચાવી;

        પાછા data;
}

unsigned _hashmap_size(HashmapBase *h) {
        જો (!h)
                પાછા 0;

        પાછા n_entries(h);
}

unsigned _hashmap_buckets(HashmapBase *h) {
        જો (!h)
                પાછા 0;

        પાછા n_buckets(h);
}

પૂર્ણાંક _hashmap_merge(Hashmap *h, Hashmap *other) {
        Iterator i;
        unsigned idx;

        પ્રતિજ્ઞા(h);

        HASHMAP_FOREACH_IDX(idx, HASHMAP_BASE(other), i) {
                સંરચના plain_hashmap_entry *pe = plain_bucket_at(other, idx);
                પૂર્ણાંક r;

                r = hashmap_put(h, pe->b.ચાવી, pe->કિંમત);
                જો (r < 0 && r != -EEXIST)
                        પાછા r;
        }

        પાછા 0;
}

પૂર્ણાંક set_merge(Set *s, Set *other) {
        Iterator i;
        unsigned idx;

        પ્રતિજ્ઞા(s);

        HASHMAP_FOREACH_IDX(idx, HASHMAP_BASE(other), i) {
                સંરચના set_entry *se = set_bucket_at(other, idx);
                પૂર્ણાંક r;

                r = set_put(s, se->b.ચાવી);
                જો (r < 0)
                        પાછા r;
        }

        પાછા 0;
}

પૂર્ણાંક _hashmap_reserve(HashmapBase *h, unsigned entries_add) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(h);

        r = resize_buckets(h, entries_add);
        જો (r < 0)
                પાછા r;

        પાછા 0;
}

/*
 * The same as hashmap_merge(), but every new item from other is moved to h.
 * Keys already in h are skipped and stay in other.
 * Returns: 0 on success.
 *          -ENOMEM on alloc failure, in which case no move has been done.
 */
પૂર્ણાંક _hashmap_move(HashmapBase *h, HashmapBase *other) {
        સંરચના swap_entries swap;
        સંરચના hashmap_base_entry *e, *n;
        Iterator i;
        unsigned idx;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(h);

        જો (!other)
                પાછા 0;

        પ્રતિજ્ઞા(other->type == h->type);

        /*
         * This reserves buckets for the worst case, where none of other's
         * entries are yet present in h. This is preferable to risking
         * an allocation failure in the middle of the moving and having to
         * rollback or return a partial result.
         */
        r = resize_buckets(h, n_entries(other));
        જો (r < 0)
                પાછા r;

        HASHMAP_FOREACH_IDX(idx, other, i) {
                unsigned h_hash;

                e = bucket_at(other, idx);
                h_hash = bucket_hash(h, e->ચાવી);
                જો (bucket_scan(h, h_hash, e->ચાવી) != IDX_NIL)
                        ચાલુ;

                n = &bucket_at_swap(&swap, IDX_PUT)->p.b;
                n->ચાવી = e->ચાવી;
                જો (h->type != HASHMAP_TYPE_SET)
                        ((સંરચના plain_hashmap_entry*) n)->કિંમત =
                                ((સંરચના plain_hashmap_entry*) e)->કિંમત;
                assert_se(hashmap_put_boldly(h, h_hash, &swap, false) == 1);

                remove_entry(other, idx);
        }

        પાછા 0;
}

પૂર્ણાંક _hashmap_move_one(HashmapBase *h, HashmapBase *other, અચલ શૂન્ય *ચાવી) {
        સંરચના swap_entries swap;
        unsigned h_hash, other_hash, idx;
        સંરચના hashmap_base_entry *e, *n;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(h);

        h_hash = bucket_hash(h, ચાવી);
        જો (bucket_scan(h, h_hash, ચાવી) != IDX_NIL)
                પાછા -EEXIST;

        જો (!other)
                પાછા -ENOENT;

        પ્રતિજ્ઞા(other->type == h->type);

        other_hash = bucket_hash(other, ચાવી);
        idx = bucket_scan(other, other_hash, ચાવી);
        જો (idx == IDX_NIL)
                પાછા -ENOENT;

        e = bucket_at(other, idx);

        n = &bucket_at_swap(&swap, IDX_PUT)->p.b;
        n->ચાવી = e->ચાવી;
        જો (h->type != HASHMAP_TYPE_SET)
                ((સંરચના plain_hashmap_entry*) n)->કિંમત =
                        ((સંરચના plain_hashmap_entry*) e)->કિંમત;
        r = hashmap_put_boldly(h, h_hash, &swap, true);
        જો (r < 0)
                પાછા r;

        remove_entry(other, idx);
        પાછા 0;
}

HashmapBase* _hashmap_copy(HashmapBase *h) {
        HashmapBase *copy;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(h);

        copy = hashmap_base_new(h->hash_ops, h->type);
        જો (!copy)
                પાછા ખાલી;

        પસંદગી (h->type) {
        સ્થિતિ HASHMAP_TYPE_PLAIN:
        સ્થિતિ HASHMAP_TYPE_ORDERED:
                r = hashmap_merge((Hashmap*)copy, (Hashmap*)h);
                તોડો;
        સ્થિતિ HASHMAP_TYPE_SET:
                r = set_merge((Set*)copy, (Set*)h);
                તોડો;
        મૂળભૂત:
                assert_not_reached();
        }

        જો (r < 0)
                પાછા _hashmap_free(copy);

        પાછા copy;
}

અક્ષર** _hashmap_get_strv(HashmapBase *h) {
        અક્ષર **sv;
        Iterator i;
        unsigned idx, n;

        જો (!h)
                પાછા new0(અક્ષર*, 1);

        sv = new(અક્ષર*, n_entries(h)+1);
        જો (!sv)
                પાછા ખાલી;

        n = 0;
        HASHMAP_FOREACH_IDX(idx, h, i)
                sv[n++] = entry_કિંમત(h, bucket_at(h, idx));
        sv[n] = ખાલી;

        પાછા sv;
}

અક્ષર** set_to_strv(Set **s) {
        પ્રતિજ્ઞા(s);

        /* This is similar to set_get_strv(), but invalidates the set on success. */

        અક્ષર **v = new(અક્ષર*, set_size(*s) + 1);
        જો (!v)
                પાછા ખાલી;

        માટે (અક્ષર **p = v; (*p = set_steal_first(*s)); p++)
                ;

        પ્રતિજ્ઞા(set_isempty(*s));
        *s = set_free(*s);
        પાછા v;
}

શૂન્ય* ordered_hashmap_next(OrderedHashmap *h, અચલ શૂન્ય *ચાવી) {
        સંરચના ordered_hashmap_entry *e;
        unsigned hash, idx;

        જો (!h)
                પાછા ખાલી;

        hash = bucket_hash(h, ચાવી);
        idx = bucket_scan(h, hash, ચાવી);
        જો (idx == IDX_NIL)
                પાછા ખાલી;

        e = ordered_bucket_at(h, idx);
        જો (e->iterate_next == IDX_NIL)
                પાછા ખાલી;
        પાછા ordered_bucket_at(h, e->iterate_next)->p.કિંમત;
}

પૂર્ણાંક set_consume(Set *s, શૂન્ય *કિંમત) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(s);
        પ્રતિજ્ઞા(કિંમત);

        r = set_put(s, કિંમત);
        જો (r <= 0)
                મુક્ત(કિંમત);

        પાછા r;
}

પૂર્ણાંક hashmap_put_strdup_full(Hashmap **h, અચલ સંરચના hash_ops *hash_ops, અચલ અક્ષર *k, અચલ અક્ષર *v) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(h);

        r = hashmap_ensure_allocated(h, hash_ops);
        જો (r < 0)
                પાછા r;

        _cleanup_free_ અક્ષર *kdup = ખાલી, *vdup = ખાલી;

        kdup = strdup(k);
        જો (!kdup)
                પાછા -ENOMEM;

        જો (v) {
                vdup = strdup(v);
                જો (!vdup)
                        પાછા -ENOMEM;
        }

        r = hashmap_put(*h, kdup, vdup);
        જો (r < 0) {
                જો (r == -EEXIST && streq_ptr(v, hashmap_get(*h, kdup)))
                        પાછા 0;
                પાછા r;
        }

        /* 0 with non-null vdup would mean vdup is already in the hashmap, which cannot be */
        પ્રતિજ્ઞા(vdup == ખાલી || r > 0);
        જો (r > 0)
                kdup = vdup = ખાલી;

        પાછા r;
}

પૂર્ણાંક set_put_strndup_full(Set **s, અચલ સંરચના hash_ops *hash_ops, અચલ અક્ષર *p, size_t n) {
        અક્ષર *c;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(s);
        પ્રતિજ્ઞા(p);

        r = set_ensure_allocated(s, hash_ops);
        જો (r < 0)
                પાછા r;

        જો (n == SIZE_MAX) {
                જો (set_contains(*s, (અક્ષર*) p))
                        પાછા 0;

                c = strdup(p);
        } નહીંતર
                c = strndup(p, n);
        જો (!c)
                પાછા -ENOMEM;

        પાછા set_consume(*s, c);
}

પૂર્ણાંક set_put_strdupv_full(Set **s, અચલ સંરચના hash_ops *hash_ops, અક્ષર **l) {
        પૂર્ણાંક n = 0, r;

        પ્રતિજ્ઞા(s);

        STRV_FOREACH(i, l) {
                r = set_put_strndup_full(s, hash_ops, *i, SIZE_MAX);
                જો (r < 0)
                        પાછા r;

                n += r;
        }

        પાછા n;
}

પૂર્ણાંક set_put_strsplit(Set *s, અચલ અક્ષર *v, અચલ અક્ષર *separators, ExtractFlags flags) {
        અચલ અક્ષર *p = ASSERT_PTR(v);
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(s);

        માટે (;;) {
                અક્ષર *word;

                r = extract_first_word(&p, &word, separators, flags);
                જો (r <= 0)
                        પાછા r;

                r = set_consume(s, word);
                જો (r < 0)
                        પાછા r;
        }
}

/* expand the cachemem if needed, return true if newly (re)activated. */
સ્થિર પૂર્ણાંક cachemem_maintain(CacheMem *mem, size_t size) {
        પ્રતિજ્ઞા(mem);

        જો (!GREEDY_REALLOC(mem->ptr, size)) {
                જો (size > 0)
                        પાછા -ENOMEM;
        }

        જો (!mem->active) {
                mem->active = true;
                પાછા true;
        }

        પાછા false;
}

પૂર્ણાંક iterated_cache_get(IteratedCache *cache, અચલ શૂન્ય ***res_ચાવીઓ, અચલ શૂન્ય ***res_કિંમતો, unsigned *res_n_entries) {
        bool sync_ચાવીઓ = false, sync_કિંમતો = false;
        size_t size;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(cache);
        પ્રતિજ્ઞા(cache->hashmap);

        size = n_entries(cache->hashmap);

        જો (res_ચાવીઓ) {
                r = cachemem_maintain(&cache->ચાવીઓ, size);
                જો (r < 0)
                        પાછા r;

                sync_ચાવીઓ = r;
        } નહીંતર
                cache->ચાવીઓ.active = false;

        જો (res_કિંમતો) {
                r = cachemem_maintain(&cache->values, size);
                જો (r < 0)
                        પાછા r;

                sync_કિંમતો = r;
        } નહીંતર
                cache->values.active = false;

        જો (cache->hashmap->dirty) {
                જો (cache->ચાવીઓ.active)
                        sync_ચાવીઓ = true;
                જો (cache->values.active)
                        sync_કિંમતો = true;

                cache->hashmap->dirty = false;
        }

        જો (sync_ચાવીઓ || sync_કિંમતો) {
                unsigned i, idx;
                Iterator iter;

                i = 0;
                HASHMAP_FOREACH_IDX(idx, cache->hashmap, iter) {
                        સંરચના hashmap_base_entry *e;

                        e = bucket_at(cache->hashmap, idx);

                        જો (sync_ચાવીઓ)
                                cache->ચાવીઓ.ptr[i] = e->ચાવી;
                        જો (sync_કિંમતો)
                                cache->values.ptr[i] = entry_કિંમત(cache->hashmap, e);
                        i++;
                }
        }

        જો (res_ચાવીઓ)
                *res_ચાવીઓ = cache->ચાવીઓ.ptr;
        જો (res_કિંમતો)
                *res_કિંમતો = cache->values.ptr;
        જો (res_n_entries)
                *res_n_entries = size;

        પાછા 0;
}

IteratedCache* iterated_cache_free(IteratedCache *cache) {
        જો (cache) {
                મુક્ત(cache->ચાવીઓ.ptr);
                મુક્ત(cache->values.ptr);
        }

        પાછા mfree(cache);
}

પૂર્ણાંક set_strjoin(Set *s, અચલ અક્ષર *separator, bool wrap_with_separator, અક્ષર **ret) {
        _cleanup_free_ અક્ષર *str = ખાલી;
        size_t separator_len, len = 0;
        અચલ અક્ષર *કિંમત;
        bool first;

        પ્રતિજ્ઞા(ret);

        જો (set_isempty(s)) {
                *ret = ખાલી;
                પાછા 0;
        }

        separator_len = strlen_ptr(separator);

        જો (separator_len == 0)
                wrap_with_separator = false;

        first = !wrap_with_separator;

        SET_FOREACH(કિંમત, s) {
                size_t l = strlen_ptr(કિંમત);

                જો (l == 0)
                        ચાલુ;

                જો (!GREEDY_REALLOC(str, len + l + (first ? 0 : separator_len) + (wrap_with_separator ? separator_len : 0) + 1))
                        પાછા -ENOMEM;

                જો (separator_len > 0 && !first) {
                        memcpy(str + len, separator, separator_len);
                        len += separator_len;
                }

                memcpy(str + len, કિંમત, l);
                len += l;
                first = false;
        }

        જો (wrap_with_separator) {
                memcpy(str + len, separator, separator_len);
                len += separator_len;
        }

        str[len] = '\0';

        *ret = TAKE_PTR(str);
        પાછા 0;
}

bool set_equal(Set *a, Set *b) {
        શૂન્ય *p;

        /* Checks whether each entry of 'a' is also in 'b' and vice versa, i.e. the two sets contain the same
         * entries */

        જો (a == b)
                પાછા true;

        જો (set_isempty(a) && set_isempty(b))
                પાછા true;

        જો (set_size(a) != set_size(b)) /* Cheap check that hopefully catches a lot of inequality cases
                                         * already */
                પાછા false;

        SET_FOREACH(p, a)
                જો (!set_contains(b, p))
                        પાછા false;

        /* If we have the same hashops, then we don't need to check things backwards given we compared the
         * size and that all of a is in b. */
        જો (a->b.hash_ops == b->b.hash_ops)
                પાછા true;

        SET_FOREACH(p, b)
                જો (!set_contains(a, p))
                        પાછા false;

        પાછા true;
}

સ્થિર bool set_fnmatch_one(Set *patterns, અચલ અક્ષર *needle) {
        અચલ અક્ષર *p;

        પ્રતિજ્ઞા(needle);

        /* Any failure of fnmatch() is treated as equivalent to FNM_NOMATCH, i.e. as non-matching pattern */

        SET_FOREACH(p, patterns)
                જો (fnmatch(p, needle, 0) == 0)
                        પાછા true;

        પાછા false;
}

bool set_fnmatch(Set *include_patterns, Set *exclude_patterns, અચલ અક્ષર *needle) {
        પ્રતિજ્ઞા(needle);

        જો (set_fnmatch_one(exclude_patterns, needle))
                પાછા false;

        જો (set_isempty(include_patterns))
                પાછા true;

        પાછા set_fnmatch_one(include_patterns, needle);
}

સ્થિર પૂર્ણાંક hashmap_entry_compare(
                સંરચના hashmap_base_entry * અચલ *a,
                સંરચના hashmap_base_entry * અચલ *b,
                compare_func_t compare) {

        પ્રતિજ્ઞા(a && *a);
        પ્રતિજ્ઞા(b && *b);
        પ્રતિજ્ઞા(compare);

        પાછા compare((*a)->ચાવી, (*b)->ચાવી);
}

સ્થિર પૂર્ણાંક _hashmap_dump_entries_sorted(
                HashmapBase *h,
                શૂન્ય ***ret,
                size_t *ret_n) {
        _cleanup_free_ શૂન્ય **entries = ખાલી;
        Iterator iter;
        unsigned idx;
        size_t n = 0;

        પ્રતિજ્ઞા(ret);
        પ્રતિજ્ઞા(ret_n);

        જો (_hashmap_size(h) == 0) {
                *ret = ખાલી;
                *ret_n = 0;
                પાછા 0;
        }

        /* We append one more element than needed so that the resulting array can be used as a strv. We
         * don't count this entry in the returned size. */
        entries = new(શૂન્ય*, _hashmap_size(h) + 1);
        જો (!entries)
                પાછા -ENOMEM;

        HASHMAP_FOREACH_IDX(idx, h, iter)
                entries[n++] = bucket_at(h, idx);

        પ્રતિજ્ઞા(n == _hashmap_size(h));
        entries[n] = ખાલી;

        typesafe_qsort_r((સંરચના hashmap_base_entry**) entries, n,
                         hashmap_entry_compare, h->hash_ops->compare);

        *ret = TAKE_PTR(entries);
        *ret_n = n;
        પાછા 0;
}

પૂર્ણાંક hashmap_dump_ચાવીઓ_sorted(HashmapBase *h, શૂન્ય ***ret, size_t *ret_n) {
        _cleanup_free_ શૂન્ય **entries = ખાલી;
        size_t n;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(ret);

        r = _hashmap_dump_entries_sorted(h, &entries, &n);
        જો (r < 0)
                પાછા r;

        /* Reuse the array. */
        FOREACH_ARRAY(e, entries, n)
                *e = (શૂન્ય*) (*(સંરચના hashmap_base_entry**) e)->ચાવી;

        *ret = TAKE_PTR(entries);
        જો (ret_n)
                *ret_n = n;
        પાછા 0;
}

પૂર્ણાંક _hashmap_dump_sorted(HashmapBase *h, શૂન્ય ***ret, size_t *ret_n) {
        _cleanup_free_ શૂન્ય **entries = ખાલી;
        size_t n;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(ret);

        r = _hashmap_dump_entries_sorted(h, &entries, &n);
        જો (r < 0)
                પાછા r;

        /* Reuse the array. */
        FOREACH_ARRAY(e, entries, n)
                *e = entry_કિંમત(h, *(સંરચના hashmap_base_entry**) e);

        *ret = TAKE_PTR(entries);
        જો (ret_n)
                *ret_n = n;
        પાછા 0;
}
