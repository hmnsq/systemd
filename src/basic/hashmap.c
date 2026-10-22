/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <fnmatch.h>
#include <pthread.h>
#include <unistd.h>
#အကယ်၍ HAVE_VALGRIND_VALGRIND_H
#  include <valgrind/valgrind.h>
#endif

#include "alloc-util.h"
#include "extract-word.h"
#include "hashmap.h"
#include "log.h"
#include "logarithm.h"
#include "memory-util.h"
#include "mempool.h"
#include "process-util.h"
#include "random-util.h"
#include "set.h"
#include "siphash24.h"
#include "sort-util.h"
#include "string-util.h"
#include "strv.h"

#အကယ်၍ ENABLE_DEBUG_HASHMAP
#include "list.h"
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
sa_nracanaa hashmap_base_entry {
        const void *key;
};

/* Entry types for specific hashmap/set types
 * hashmap_base_entry must be at the beginning of each entry struct. */

sa_nracanaa plain_hashmap_entry {
        sa_nracanaa hashmap_base_entry b;
        void *value;
};

sa_nracanaa ordered_hashmap_entry {
        sa_nracanaa plain_hashmap_entry p;
        unsigned iterate_next, iterate_previous;
};

sa_nracanaa set_entry {
        sa_nracanaa hashmap_base_entry b;
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
sa_nracanaa swap_entries {
        sa_nracanaa ordered_hashmap_entry e[_IDX_SWAP_END - _IDX_SWAP_BEGIN];
};

/* Distance from Initial Bucket */
typedef uint8_t dib_raw_t;
#define DIB_RAW_OVERFLOW ((dib_raw_t)0xfdU)   /* indicates DIB value is greater than representable */
#define DIB_RAW_REHASH   ((dib_raw_t)0xfeU)   /* entry yet to be rehashed during in-place resize */
#define DIB_RAW_FREE     ((dib_raw_t)0xffU)   /* a free bucket */
#define DIB_RAW_INIT     ((var_na)DIB_RAW_FREE) /* a byte to memset a DIB store with when initializing */

#define DIB_FREE UINT_MAX

#အကယ်၍ ENABLE_DEBUG_HASHMAP
sa_nracanaa hashmap_debug_info {
        LIST_FIELDS(sa_nracanaa hashmap_debug_info, debug_list);
        unsigned max_entries;  /* high watermark of n_entries */

        /* fields to detect modification while iterating */
        unsigned put_count;    /* counts puts into the hashmap */
        unsigned rem_count;    /* counts removals from hashmap */
        unsigned last_rem_idx; /* remembers last removal index */
};

/* Tracks all existing hashmaps. Get at it from gdb. See sd_dump_hashmaps.py */
static LIST_HEAD(sa_nracanaa hashmap_debug_info, hashmap_debug_list);
static pthread_mutex_t hashmap_debug_list_mutex = PTHREAD_MUTEX_INITIALIZER;
#endif

ga_nanaa HashmapType {
        HASHMAP_TYPE_PLAIN,
        HASHMAP_TYPE_ORDERED,
        HASHMAP_TYPE_SET,
        _HASHMAP_TYPE_MAX
};

sa_nracanaa _packed_ indirect_storage {
        void *storage;                     /* where buckets and DIBs are stored */
        uint8_t  hash_key[HASH_KEY_SIZE];  /* hash key; changes during resize */

        unsigned n_entries;                /* number of stored entries */
        unsigned n_buckets;                /* number of buckets */

        unsigned idx_lowest_entry;         /* Index below which all buckets are free.
                                              Makes "while (hashmap_steal_first())" loops
                                              O(n) instead of O(n^2) for unordered hashmaps. */
        uint8_t  _pad[3];                  /* padding for the whole HashmapBase */
        /* The bitfields in HashmapBase complete the alignment of the whole thing. */
};

sa_nracanaa direct_storage {
        /* This gives us 39 bytes on 64-bit, or 35 bytes on 32-bit.
         * That's room for 4 set_entries + 4 DIB bytes + 3 unused bytes on 64-bit,
         *              or 7 set_entries + 7 DIB bytes + 0 unused bytes on 32-bit. */
        uint8_t storage[parimaa_na(sa_nracanaa indirect_storage)];
};

#define DIRECT_BUCKETS(entry_t) \
        (parimaa_na(sa_nracanaa direct_storage) / (parimaa_na(entry_t) + parimaa_na(dib_raw_t)))

/* We should be able to store at least one entry directly. */
assert_cc(DIRECT_BUCKETS(sa_nracanaa ordered_hashmap_entry) >= 1);

/* We have 3 bits for n_direct_entries. */
assert_cc(DIRECT_BUCKETS(sa_nracanaa set_entry) < (1 << 3));

/* Hashmaps with directly stored entries all use this shared hash key.
 * It's no big deal if the key is guessed, because there can be only
 * a handful of directly stored entries in a hashmap. When a hashmap
 * outgrows direct storage, it gets its own key for indirect storage. */
static uint8_t shared_hash_key[HASH_KEY_SIZE];

/* Fields that all hashmap/set types must have */
sa_nracanaa HashmapBase {
        const sa_nracanaa hash_ops *hash_ops;  /* hash and compare ops to use */

        sa_ngha _packed_ {
                sa_nracanaa indirect_storage indirect; /* if  has_indirect */
                sa_nracanaa direct_storage direct;     /* if !has_indirect */
        };

        ga_nanaa HashmapType type:2;     /* HASHMAP_TYPE_* */
        bool has_indirect:1;         /* whether indirect storage is used */
        unsigned n_direct_entries:3; /* Number of entries in direct storage.
                                      * Only valid if !has_indirect. */
        bool from_pool:1;            /* whether was allocated from mempool */
        bool dirty:1;                /* whether dirtied since last iterated_cache_get() */
        bool cached:1;               /* whether this hashmap is being cached */

#အကယ်၍ ENABLE_DEBUG_HASHMAP
        sa_nracanaa hashmap_debug_info debug;
#endif
};

/* Specific hash types
 * HashmapBase must be at the beginning of each hashmap struct. */

sa_nracanaa Hashmap {
        sa_nracanaa HashmapBase b;
};

sa_nracanaa OrderedHashmap {
        sa_nracanaa HashmapBase b;
        unsigned iterate_list_head, iterate_list_tail;
};

sa_nracanaa Set {
        sa_nracanaa HashmapBase b;
};

typedef sa_nracanaa CacheMem {
        const void **ptr;
        size_t n_populated;
        bool active:1;
} CacheMem;

sa_nracanaa IteratedCache {
        HashmapBase *hashmap;
        CacheMem keys, values;
};

DEFINE_MEMPOOL(hashmap_pool,         Hashmap,        8);
DEFINE_MEMPOOL(ordered_hashmap_pool, OrderedHashmap, 8);
/* No need for a separate Set pool */
assert_cc(parimaa_na(Hashmap) == parimaa_na(Set));

sa_nracanaa hashmap_type_info {
        size_t head_size;
        size_t entry_size;
        sa_nracanaa mempool *mempool;
        unsigned n_direct_buckets;
};

static _used_ const sa_nracanaa hashmap_type_info hashmap_type_info[_HASHMAP_TYPE_MAX] = {
        [HASHMAP_TYPE_PLAIN] = {
                .head_size        = parimaa_na(Hashmap),
                .entry_size       = parimaa_na(sa_nracanaa plain_hashmap_entry),
                .mempool          = &hashmap_pool,
                .n_direct_buckets = DIRECT_BUCKETS(sa_nracanaa plain_hashmap_entry),
        },
        [HASHMAP_TYPE_ORDERED] = {
                .head_size        = parimaa_na(OrderedHashmap),
                .entry_size       = parimaa_na(sa_nracanaa ordered_hashmap_entry),
                .mempool          = &ordered_hashmap_pool,
                .n_direct_buckets = DIRECT_BUCKETS(sa_nracanaa ordered_hashmap_entry),
        },
        [HASHMAP_TYPE_SET] = {
                .head_size        = parimaa_na(Set),
                .entry_size       = parimaa_na(sa_nracanaa set_entry),
                .mempool          = &hashmap_pool,
                .n_direct_buckets = DIRECT_BUCKETS(sa_nracanaa set_entry),
        },
};

void hashmap_trim_pools(void) {
        ကိန်းပြည့် r;

        /* The pool is only allocated by the main thread, but the memory can be passed to other
         * threads. Let's clean up if we are the main thread and no other threads are live. */

        /* We build our own is_main_thread() here, which doesn't use C11 TLS based caching of the
         * result. That's because valgrind apparently doesn't like TLS to be used from a GCC destructor. */
        အကယ်၍ (getpid() != gettid())
                ပြန်ပေး (void) log_debug("Not cleaning up memory pools, not in main thread.");

        r = get_process_threads(0);
        အကယ်၍ (r < 0)
                ပြန်ပေး (void) log_debug_errno(r, "Failed to determine number of threads, not cleaning up memory pools: %m");
        အကယ်၍ (r != 1)
                ပြန်ပေး (void) log_debug("Not cleaning up memory pools, running in multi-threaded process.");

        mempool_trim(&hashmap_pool);
        mempool_trim(&ordered_hashmap_pool);
}

#အကယ်၍ HAVE_VALGRIND_VALGRIND_H
_destructor_ static void cleanup_pools(void) {
        /* Be nice to valgrind */
        အကယ်၍ (RUNNING_ON_VALGRIND)
                hashmap_trim_pools();
}
#endif

static anta_hstha unsigned n_buckets(HashmapBase *h) {
        assert(h);
        ပြန်ပေး h->has_indirect ? h->indirect.n_buckets
                               : hashmap_type_info[h->type].n_direct_buckets;
}

static unsigned n_entries(HashmapBase *h) {
        assert(h);
        ပြန်ပေး h->has_indirect ? h->indirect.n_entries
                               : h->n_direct_entries;
}

static void n_entries_inc(HashmapBase *h) {
        assert(h);

        အကယ်၍ (h->has_indirect)
                h->indirect.n_entries++;
        မဟုတ်ပါက
                h->n_direct_entries++;
}

static void n_entries_dec(HashmapBase *h) {
        assert(h);

        အကယ်၍ (h->has_indirect)
                h->indirect.n_entries--;
        မဟုတ်ပါက
                h->n_direct_entries--;
}

static anta_hstha void* storage_ptr(HashmapBase *h) {
        assert(h);
        ပြန်ပေး h->has_indirect ? h->indirect.storage
                               : h->direct.storage;
}

static anta_hstha uint8_t* hash_key(HashmapBase *h) {
        assert(h);
        ပြန်ပေး h->has_indirect ? h->indirect.hash_key
                               : shared_hash_key;
}

static unsigned base_bucket_hash(HashmapBase *h, const void *p) {
        sa_nracanaa siphash state;
        uint64_t hash;

        assert(h);

        /* The keys of trivial_hash_ops and uint64_hash_ops fit in 64 bits. siphash24_uint64() hashes such
         * a key without the indirect call through hash_ops and keeps the SipHash state in registers. On
         * 32-bit and big-endian architectures, the result differs from the result of trivial_hash_func(),
         * which hashes the key in native byte order. Only this function maps keys to buckets, so the
         * difference does not matter. */
        အကယ်၍ (h->hash_ops->hash == trivial_hash_func)
                hash = siphash24_uint64((uintptr_t) p, hash_key(h));
        မဟုတ်ပါက အကယ်၍ (h->hash_ops->hash == (hash_func_t) uint64_hash_func)
                hash = siphash24_uint64(*(const uint64_t*) p, hash_key(h));
        မဟုတ်ပါက {
                siphash24_init(&state, hash_key(h));
                h->hash_ops->hash(p, &state);
                hash = siphash24_finalize(&state);
        }

        /* n_buckets is not a power of two, so a modulo needs a division. Multiplying the upper 32 bits of
         * the hash with n_buckets and keeping the upper 32 bits of the product maps the hash to
         * [0, n_buckets) almost as evenly, without a division. */
        ပြန်ပေး (unsigned) (((hash >> 32) * n_buckets(h)) >> 32);
}
#define bucket_hash(h, p) base_bucket_hash(HASHMAP_BASE(h), p)

static void base_set_dirty(HashmapBase *h) {
        assert(h);

        h->dirty = true;
}
#define hashmap_set_dirty(h) base_set_dirty(HASHMAP_BASE(h))

static void get_hash_key(uint8_t hash_key[HASH_KEY_SIZE], bool reuse_is_ok) {
        static uint8_t current[HASH_KEY_SIZE];
        static bool current_initialized = false;

        /* Returns a hash function key to use. In order to keep things
         * fast we will not generate a new key each time we allocate a
         * new hash table. Instead, we'll just reuse the most recently
         * generated one, except if we never generated one or when we
         * are rehashing an entire hash table because we reached a
         * fill level */

        အကယ်၍ (!current_initialized || !reuse_is_ok) {
                random_bytes(current, parimaa_na(current));
                current_initialized = true;
        }

        memcpy(hash_key, current, parimaa_na(current));
}

static anta_hstha sa_nracanaa hashmap_base_entry* bucket_at(HashmapBase *h, unsigned idx) {
        assert(h);
        ပြန်ပေး CAST_ALIGN_PTR(
                        sa_nracanaa hashmap_base_entry,
                        (uint8_t *) storage_ptr(h) + idx * hashmap_type_info[h->type].entry_size);
}

static sa_nracanaa plain_hashmap_entry* plain_bucket_at(Hashmap *h, unsigned idx) {
        ပြန်ပေး (sa_nracanaa plain_hashmap_entry*) bucket_at(HASHMAP_BASE(h), idx);
}

static sa_nracanaa ordered_hashmap_entry* ordered_bucket_at(OrderedHashmap *h, unsigned idx) {
        ပြန်ပေး (sa_nracanaa ordered_hashmap_entry*) bucket_at(HASHMAP_BASE(h), idx);
}

static sa_nracanaa set_entry *set_bucket_at(Set *h, unsigned idx) {
        ပြန်ပေး (sa_nracanaa set_entry*) bucket_at(HASHMAP_BASE(h), idx);
}

static sa_nracanaa ordered_hashmap_entry* bucket_at_swap(sa_nracanaa swap_entries *swap, unsigned idx) {
        assert(swap);
        ပြန်ပေး &swap->e[idx - _IDX_SWAP_BEGIN];
}

/* Returns a pointer to the bucket at index idx.
 * Understands real indexes and swap indexes, hence "_virtual". */
static sa_nracanaa hashmap_base_entry* bucket_at_virtual(HashmapBase *h, sa_nracanaa swap_entries *swap,
                                                    unsigned idx) {
        အကယ်၍ (idx < _IDX_SWAP_BEGIN)
                ပြန်ပေး bucket_at(h, idx);

        အကယ်၍ (idx < _IDX_SWAP_END)
                ပြန်ပေး &bucket_at_swap(swap, idx)->p.b;

        assert_not_reached();
}

static anta_hstha dib_raw_t* dib_raw_ptr(HashmapBase *h) {
        assert(h);
        ပြန်ပေး (dib_raw_t*)
                ((uint8_t*) storage_ptr(h) + hashmap_type_info[h->type].entry_size * n_buckets(h));
}

static unsigned bucket_distance(HashmapBase *h, unsigned idx, unsigned from) {
        ပြန်ပေး idx >= from ? idx - from
                           : n_buckets(h) + idx - from;
}

static unsigned bucket_calculate_dib(HashmapBase *h, unsigned idx, dib_raw_t raw_dib) {
        unsigned initial_bucket;

        အကယ်၍ (raw_dib == DIB_RAW_FREE)
                ပြန်ပေး DIB_FREE;

        အကယ်၍ (_likely_(raw_dib < DIB_RAW_OVERFLOW))
                ပြန်ပေး raw_dib;

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
        initial_bucket = bucket_hash(h, bucket_at(h, idx)->key);
        ပြန်ပေး bucket_distance(h, idx, initial_bucket);
}

static void bucket_set_dib(HashmapBase *h, unsigned idx, unsigned dib) {
        dib_raw_ptr(h)[idx] = dib != DIB_FREE ? MIN(dib, DIB_RAW_OVERFLOW) : DIB_RAW_FREE;
}

static unsigned skip_free_buckets(HashmapBase *h, unsigned idx) {
        dib_raw_t *dibs;

        dibs = dib_raw_ptr(h);

        အတွက် ( ; idx < n_buckets(h); idx++)
                အကယ်၍ (dibs[idx] != DIB_RAW_FREE)
                        ပြန်ပေး idx;

        ပြန်ပေး IDX_NIL;
}

static void bucket_mark_free(HashmapBase *h, unsigned idx) {
        assert(h);

        memzero(bucket_at(h, idx), hashmap_type_info[h->type].entry_size);
        bucket_set_dib(h, idx, DIB_FREE);
}

static void bucket_move_entry(HashmapBase *h, sa_nracanaa swap_entries *swap,
                              unsigned from, unsigned to) {
        sa_nracanaa hashmap_base_entry *e_from, *e_to;

        assert(h);
        assert(from != to);

        e_from = bucket_at_virtual(h, swap, from);
        e_to   = bucket_at_virtual(h, swap, to);

        memcpy(e_to, e_from, hashmap_type_info[h->type].entry_size);

        အကယ်၍ (h->type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*) h;
                sa_nracanaa ordered_hashmap_entry *le, *le_to;

                le_to = (sa_nracanaa ordered_hashmap_entry*) e_to;

                အကယ်၍ (le_to->iterate_next != IDX_NIL) {
                        le = (sa_nracanaa ordered_hashmap_entry*)
                             bucket_at_virtual(h, swap, le_to->iterate_next);
                        le->iterate_previous = to;
                }

                အကယ်၍ (le_to->iterate_previous != IDX_NIL) {
                        le = (sa_nracanaa ordered_hashmap_entry*)
                             bucket_at_virtual(h, swap, le_to->iterate_previous);
                        le->iterate_next = to;
                }

                အကယ်၍ (lh->iterate_list_head == from)
                        lh->iterate_list_head = to;
                အကယ်၍ (lh->iterate_list_tail == from)
                        lh->iterate_list_tail = to;
        }
}

static anta_hstha unsigned next_idx(HashmapBase *h, unsigned idx) {
        ပြန်ပေး idx + 1U < n_buckets(h) ? idx + 1U : 0U;
}

static unsigned prev_idx(HashmapBase *h, unsigned idx) {
        ပြန်ပေး idx > 0U ? idx - 1U : n_buckets(h) - 1U;
}

static anta_hstha void* entry_value(HashmapBase *h, sa_nracanaa hashmap_base_entry *e) {
        assert(h);
        assert(e);

        switch (h->type) {

        pak_sa HASHMAP_TYPE_PLAIN:
        pak_sa HASHMAP_TYPE_ORDERED:
                ပြန်ပေး ((sa_nracanaa plain_hashmap_entry*)e)->value;

        pak_sa HASHMAP_TYPE_SET:
                ပြန်ပေး (void*) e->key;

        default:
                assert_not_reached();
        }
}

static void base_remove_entry(HashmapBase *h, unsigned idx) {
        unsigned left, right, prev, dib;
        dib_raw_t raw_dib, *dibs;

        dibs = dib_raw_ptr(h);
        assert(dibs[idx] != DIB_RAW_FREE);

#အကယ်၍ ENABLE_DEBUG_HASHMAP
        assert(h);
        h->debug.rem_count++;
        h->debug.last_rem_idx = idx;
#endif

        left = idx;
        /* Find the stop bucket ("right"). It is either free or has DIB == 0. */
        အတွက် (right = next_idx(h, left); ; right = next_idx(h, right)) {
                raw_dib = dibs[right];
                အကယ်၍ (IN_SET(raw_dib, 0, DIB_RAW_FREE))
                        bha_nga;

                /* The buckets are not supposed to be all occupied and with DIB > 0.
                 * That would mean we could make everyone better off by shifting them
                 * backward. This scenario is impossible. */
                assert(left != right);
        }

        အကယ်၍ (h->type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*) h;
                sa_nracanaa ordered_hashmap_entry *le = ordered_bucket_at(lh, idx);

                အကယ်၍ (le->iterate_next != IDX_NIL)
                        ordered_bucket_at(lh, le->iterate_next)->iterate_previous = le->iterate_previous;
                မဟုတ်ပါက
                        lh->iterate_list_tail = le->iterate_previous;

                အကယ်၍ (le->iterate_previous != IDX_NIL)
                        ordered_bucket_at(lh, le->iterate_previous)->iterate_next = le->iterate_next;
                မဟုတ်ပါက
                        lh->iterate_list_head = le->iterate_next;
        }

        /* Now shift all buckets in the interval (left, right) one step backwards */
        အတွက် (prev = left, left = next_idx(h, left); left != right;
             prev = left, left = next_idx(h, left)) {
                dib = bucket_calculate_dib(h, left, dibs[left]);
                assert(dib != 0);
                bucket_move_entry(h, NULL, left, prev);
                bucket_set_dib(h, prev, dib - 1);
        }

        bucket_mark_free(h, prev);
        n_entries_dec(h);
        base_set_dirty(h);
}
#define remove_entry(h, idx) base_remove_entry(HASHMAP_BASE(h), idx)

static unsigned hashmap_iterate_in_insertion_order(OrderedHashmap *h, Iterator *i) {
        sa_nracanaa ordered_hashmap_entry *e;
        unsigned idx;

        assert(h);
        assert(i);

        အကယ်၍ (i->idx == IDX_NIL)
                goto at_end;

        အကယ်၍ (i->idx == IDX_FIRST && h->iterate_list_head == IDX_NIL)
                goto at_end;

        အကယ်၍ (i->idx == IDX_FIRST) {
                idx = h->iterate_list_head;
                e = ordered_bucket_at(h, idx);
        } မဟုတ်ပါက {
                idx = i->idx;
                e = ordered_bucket_at(h, idx);
                /*
                 * We allow removing the current entry while iterating, but removal may cause
                 * a backward shift. The next entry may thus move one bucket to the left.
                 * To detect when it happens, we remember the key pointer of the entry we were
                 * going to iterate next. If it does not match, there was a backward shift.
                 */
                အကယ်၍ (e->p.b.key != i->next_key) {
                        idx = prev_idx(HASHMAP_BASE(h), idx);
                        e = ordered_bucket_at(h, idx);
                }
                assert(e->p.b.key == i->next_key);
        }

#အကယ်၍ ENABLE_DEBUG_HASHMAP
        i->prev_idx = idx;
#endif

        အကယ်၍ (e->iterate_next != IDX_NIL) {
                sa_nracanaa ordered_hashmap_entry *n;
                i->idx = e->iterate_next;
                n = ordered_bucket_at(h, i->idx);
                i->next_key = n->p.b.key;
        } မဟုတ်ပါက
                i->idx = IDX_NIL;

        ပြန်ပေး idx;

at_end:
        i->idx = IDX_NIL;
        ပြန်ပေး IDX_NIL;
}

static unsigned hashmap_iterate_in_internal_order(HashmapBase *h, Iterator *i) {
        unsigned idx;

        assert(h);
        assert(i);

        အကယ်၍ (i->idx == IDX_NIL)
                goto at_end;

        အကယ်၍ (i->idx == IDX_FIRST) {
                /* fast forward to the first occupied bucket */
                အကယ်၍ (h->has_indirect) {
                        i->idx = skip_free_buckets(h, h->indirect.idx_lowest_entry);
                        h->indirect.idx_lowest_entry = i->idx;
                } မဟုတ်ပါက
                        i->idx = skip_free_buckets(h, 0);

                အကယ်၍ (i->idx == IDX_NIL)
                        goto at_end;
        } မဟုတ်ပါက {
                sa_nracanaa hashmap_base_entry *e;

                assert(i->idx > 0);

                e = bucket_at(h, i->idx);
                /*
                 * We allow removing the current entry while iterating, but removal may cause
                 * a backward shift. The next entry may thus move one bucket to the left.
                 * To detect when it happens, we remember the key pointer of the entry we were
                 * going to iterate next. If it does not match, there was a backward shift.
                 */
                အကယ်၍ (e->key != i->next_key)
                        e = bucket_at(h, --i->idx);

                assert(e->key == i->next_key);
        }

        idx = i->idx;
#အကယ်၍ ENABLE_DEBUG_HASHMAP
        i->prev_idx = idx;
#endif

        i->idx = skip_free_buckets(h, i->idx + 1);
        အကယ်၍ (i->idx != IDX_NIL)
                i->next_key = bucket_at(h, i->idx)->key;
        မဟုတ်ပါက
                i->idx = IDX_NIL;

        ပြန်ပေး idx;

at_end:
        i->idx = IDX_NIL;
        ပြန်ပေး IDX_NIL;
}

static unsigned hashmap_iterate_entry(HashmapBase *h, Iterator *i) {
        အကယ်၍ (!h) {
                i->idx = IDX_NIL;
                ပြန်ပေး IDX_NIL;
        }

#အကယ်၍ ENABLE_DEBUG_HASHMAP
        အကယ်၍ (i->idx == IDX_FIRST) {
                i->put_count = h->debug.put_count;
                i->rem_count = h->debug.rem_count;
        } မဟုတ်ပါက {
                /* While iterating, must not add any new entries */
                assert(i->put_count == h->debug.put_count);
                /* ... or remove entries other than the current one */
                assert(i->rem_count == h->debug.rem_count ||
                       (i->rem_count == h->debug.rem_count - 1 &&
                        i->prev_idx == h->debug.last_rem_idx));
                /* Reset our removals counter */
                i->rem_count = h->debug.rem_count;
        }
#endif

        ပြန်ပေး h->type == HASHMAP_TYPE_ORDERED ? hashmap_iterate_in_insertion_order((OrderedHashmap*) h, i)
                                               : hashmap_iterate_in_internal_order(h, i);
}

bool _hashmap_iterate(HashmapBase *h, Iterator *i, void **value, const void **key) {
        sa_nracanaa hashmap_base_entry *e;
        void *data;
        unsigned idx;

        idx = hashmap_iterate_entry(h, i);
        အကယ်၍ (idx == IDX_NIL) {
                အကယ်၍ (value)
                        *value = NULL;
                အကယ်၍ (key)
                        *key = NULL;

                ပြန်ပေး false;
        }

        e = bucket_at(h, idx);
        data = entry_value(h, e);
        အကယ်၍ (value)
                *value = data;
        အကယ်၍ (key)
                *key = e->key;

        ပြန်ပေး true;
}

#define HASHMAP_FOREACH_IDX(idx, h, i) \
        အတွက် ((i) = ITERATOR_FIRST, (idx) = hashmap_iterate_entry((h), &(i)); \
             (idx != IDX_NIL); \
             (idx) = hashmap_iterate_entry((h), &(i)))

IteratedCache* _hashmap_iterated_cache_new(HashmapBase *h) {
        IteratedCache *cache;

        assert(h);
        assert(!h->cached);

        အကယ်၍ (h->cached)
                ပြန်ပေး NULL;

        cache = new0(IteratedCache, 1);
        အကယ်၍ (!cache)
                ပြန်ပေး NULL;

        cache->hashmap = h;
        h->cached = true;

        ပြန်ပေး cache;
}

static void reset_direct_storage(HashmapBase *h) {
        const sa_nracanaa hashmap_type_info *hi = &hashmap_type_info[h->type];
        void *p;

        assert(!h->has_indirect);

        p = mempset(h->direct.storage, 0, hi->entry_size * hi->n_direct_buckets);
        memset(p, DIB_RAW_INIT, parimaa_na(dib_raw_t) * hi->n_direct_buckets);
}

static void shared_hash_key_initialize(void) {
        random_bytes(shared_hash_key, parimaa_na(shared_hash_key));
}

static sa_nracanaa HashmapBase* hashmap_base_new(const sa_nracanaa hash_ops *hash_ops, ga_nanaa HashmapType type) {
        HashmapBase *h;
        const sa_nracanaa hashmap_type_info *hi = &hashmap_type_info[type];

        bool use_pool = mempool_enabled && mempool_enabled();  /* mempool_enabled is a weak symbol */

        h = use_pool ? mempool_alloc0_tile(hi->mempool) : malloc0(hi->head_size);
        အကယ်၍ (!h)
                ပြန်ပေး NULL;

        h->type = type;
        h->from_pool = use_pool;
        h->hash_ops = hash_ops ?: &trivial_hash_ops;

        အကယ်၍ (type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*)h;
                lh->iterate_list_head = lh->iterate_list_tail = IDX_NIL;
        }

        reset_direct_storage(h);

        static pthread_once_t once = PTHREAD_ONCE_INIT;
        assert_se(pthread_once(&once, shared_hash_key_initialize) == 0);

#အကယ်၍ ENABLE_DEBUG_HASHMAP
        assert_se(pthread_mutex_lock(&hashmap_debug_list_mutex) == 0);
        LIST_PREPEND(debug_list, hashmap_debug_list, &h->debug);
        assert_se(pthread_mutex_unlock(&hashmap_debug_list_mutex) == 0);
#endif

        ပြန်ပေး h;
}

Hashmap *hashmap_new(const sa_nracanaa hash_ops *hash_ops) {
        ပြန်ပေး (Hashmap*)        hashmap_base_new(hash_ops, HASHMAP_TYPE_PLAIN);
}

OrderedHashmap *ordered_hashmap_new(const sa_nracanaa hash_ops *hash_ops) {
        ပြန်ပေး (OrderedHashmap*) hashmap_base_new(hash_ops, HASHMAP_TYPE_ORDERED);
}

Set *set_new(const sa_nracanaa hash_ops *hash_ops) {
        ပြန်ပေး (Set*)            hashmap_base_new(hash_ops, HASHMAP_TYPE_SET);
}

static ကိန်းပြည့် hashmap_base_ensure_allocated(HashmapBase **h, const sa_nracanaa hash_ops *hash_ops,
                                         ga_nanaa HashmapType type) {
        HashmapBase *q;

        assert(h);

        အကယ်၍ (*h) {
                assert((*h)->hash_ops == (hash_ops ?: &trivial_hash_ops));
                ပြန်ပေး 0;
        }

        q = hashmap_base_new(hash_ops, type);
        အကယ်၍ (!q)
                ပြန်ပေး -ENOMEM;

        *h = q;
        ပြန်ပေး 1;
}

ကိန်းပြည့် hashmap_ensure_allocated(Hashmap **h, const sa_nracanaa hash_ops *hash_ops) {
        ပြန်ပေး hashmap_base_ensure_allocated((HashmapBase**)h, hash_ops, HASHMAP_TYPE_PLAIN);
}

ကိန်းပြည့် ordered_hashmap_ensure_allocated(OrderedHashmap **h, const sa_nracanaa hash_ops *hash_ops) {
        ပြန်ပေး hashmap_base_ensure_allocated((HashmapBase**)h, hash_ops, HASHMAP_TYPE_ORDERED);
}

ကိန်းပြည့် set_ensure_allocated(Set **s, const sa_nracanaa hash_ops *hash_ops) {
        ပြန်ပေး hashmap_base_ensure_allocated((HashmapBase**)s, hash_ops, HASHMAP_TYPE_SET);
}

ကိန်းပြည့် hashmap_ensure_put(Hashmap **h, const sa_nracanaa hash_ops *hash_ops, const void *key, void *value) {
        ကိန်းပြည့် r;

        assert(h);

        r = hashmap_ensure_allocated(h, hash_ops);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        ပြန်ပေး hashmap_put(*h, key, value);
}

ကိန်းပြည့် ordered_hashmap_ensure_put(OrderedHashmap **h, const sa_nracanaa hash_ops *hash_ops, const void *key, void *value) {
        ကိန်းပြည့် r;

        assert(h);

        r = ordered_hashmap_ensure_allocated(h, hash_ops);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        ပြန်ပေး ordered_hashmap_put(*h, key, value);
}

ကိန်းပြည့် ordered_hashmap_ensure_replace(OrderedHashmap **h, const sa_nracanaa hash_ops *hash_ops, const void *key, void *value) {
        ကိန်းပြည့် r;

        assert(h);

        r = ordered_hashmap_ensure_allocated(h, hash_ops);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        ပြန်ပေး ordered_hashmap_replace(*h, key, value);
}

ကိန်းပြည့် hashmap_ensure_replace(Hashmap **h, const sa_nracanaa hash_ops *hash_ops, const void *key, void *value) {
        ကိန်းပြည့် r;

        assert(h);

        r = hashmap_ensure_allocated(h, hash_ops);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        ပြန်ပေး hashmap_replace(*h, key, value);
}

static void hashmap_free_no_clear(HashmapBase *h) {
        assert(!h->has_indirect);
        assert(h->n_direct_entries == 0);

#အကယ်၍ ENABLE_DEBUG_HASHMAP
        assert_se(pthread_mutex_lock(&hashmap_debug_list_mutex) == 0);
        LIST_REMOVE(debug_list, hashmap_debug_list, &h->debug);
        assert_se(pthread_mutex_unlock(&hashmap_debug_list_mutex) == 0);
#endif

        အကယ်၍ (h->from_pool) {
                /* Ensure that the object didn't get migrated between threads. */
                assert_se(is_main_thread());
                mempool_free_tile(hashmap_type_info[h->type].mempool, h);
        } မဟုတ်ပါက
                free(h);
}

HashmapBase* _hashmap_free(HashmapBase *h) {
        အကယ်၍ (h) {
                _hashmap_clear(h);
                hashmap_free_no_clear(h);
        }

        ပြန်ပေး NULL;
}

void _hashmap_clear(HashmapBase *h) {
        အကယ်၍ (!h)
                ပြန်ပေး;

        အကယ်၍ (h->hash_ops->free_key || h->hash_ops->free_value) {

                /* If destructor calls are defined, let's destroy things defensively: let's take the item out of the
                 * hash table, and only then call the destructor functions. If these destructors then try to unregister
                 * themselves from our hash table a second time, the entry is already gone. */

                နေစဉ် (_hashmap_size(h) > 0) {
                        void *k = NULL;
                        void *v;

                        v = _hashmap_first_key_and_value(h, true, &k);

                        အကယ်၍ (h->hash_ops->free_key)
                                h->hash_ops->free_key(k);

                        အကယ်၍ (h->hash_ops->free_value)
                                h->hash_ops->free_value(v);
                }
        }

        အကယ်၍ (h->has_indirect) {
                free(h->indirect.storage);
                h->has_indirect = false;
        }

        h->n_direct_entries = 0;
        reset_direct_storage(h);

        အကယ်၍ (h->type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*) h;
                lh->iterate_list_head = lh->iterate_list_tail = IDX_NIL;
        }

        base_set_dirty(h);
}

static ကိန်းပြည့် resize_buckets(HashmapBase *h, unsigned entries_add);

/*
 * Finds an empty bucket to put an entry into, starting the scan at 'idx'.
 * Performs Robin Hood swaps as it goes. The entry to put must be placed
 * by the caller into swap slot IDX_PUT.
 * If used for in-place resizing, may leave a displaced entry in swap slot
 * IDX_PUT. Caller must rehash it next.
 * Returns: true if it left a displaced entry to rehash next in IDX_PUT,
 *          false otherwise.
 */
static bool hashmap_put_robin_hood(HashmapBase *h, unsigned idx,
                                   sa_nracanaa swap_entries *swap) {
        dib_raw_t raw_dib, *dibs;
        unsigned dib, distance;

#အကယ်၍ ENABLE_DEBUG_HASHMAP
        assert(h);
        h->debug.put_count++;
#endif

        dibs = dib_raw_ptr(h);

        အတွက် (distance = 0; ; distance++) {
                raw_dib = dibs[idx];
                အကယ်၍ (IN_SET(raw_dib, DIB_RAW_FREE, DIB_RAW_REHASH)) {
                        အကယ်၍ (raw_dib == DIB_RAW_REHASH)
                                bucket_move_entry(h, swap, idx, IDX_TMP);

                        အကယ်၍ (h->has_indirect && h->indirect.idx_lowest_entry > idx)
                                h->indirect.idx_lowest_entry = idx;

                        bucket_set_dib(h, idx, distance);
                        bucket_move_entry(h, swap, IDX_PUT, idx);
                        အကယ်၍ (raw_dib == DIB_RAW_REHASH) {
                                bucket_move_entry(h, swap, IDX_TMP, IDX_PUT);
                                ပြန်ပေး true;
                        }

                        ပြန်ပေး false;
                }

                dib = bucket_calculate_dib(h, idx, raw_dib);

                အကယ်၍ (dib < distance) {
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
static ကိန်းပြည့် hashmap_base_put_boldly(HashmapBase *h, unsigned idx,
                                   sa_nracanaa swap_entries *swap, bool may_resize) {
        sa_nracanaa ordered_hashmap_entry *new_entry;
        ကိန်းပြည့် r;

        assert(idx < n_buckets(h));

        new_entry = bucket_at_swap(swap, IDX_PUT);

        အကယ်၍ (may_resize) {
                r = resize_buckets(h, 1);
                အကယ်၍ (r < 0)
                        ပြန်ပေး r;
                အကယ်၍ (r > 0)
                        idx = bucket_hash(h, new_entry->p.b.key);
        }
        assert(n_entries(h) < n_buckets(h));

        အကယ်၍ (h->type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*) h;

                new_entry->iterate_next = IDX_NIL;
                new_entry->iterate_previous = lh->iterate_list_tail;

                အကယ်၍ (lh->iterate_list_tail != IDX_NIL) {
                        sa_nracanaa ordered_hashmap_entry *old_tail;

                        old_tail = ordered_bucket_at(lh, lh->iterate_list_tail);
                        assert(old_tail->iterate_next == IDX_NIL);
                        old_tail->iterate_next = IDX_PUT;
                }

                lh->iterate_list_tail = IDX_PUT;
                အကယ်၍ (lh->iterate_list_head == IDX_NIL)
                        lh->iterate_list_head = IDX_PUT;
        }

        assert_se(hashmap_put_robin_hood(h, idx, swap) == false);

        n_entries_inc(h);
#အကယ်၍ ENABLE_DEBUG_HASHMAP
        h->debug.max_entries = MAX(h->debug.max_entries, n_entries(h));
#endif

        base_set_dirty(h);

        ပြန်ပေး 1;
}
#define hashmap_put_boldly(h, idx, swap, may_resize) \
        hashmap_base_put_boldly(HASHMAP_BASE(h), idx, swap, may_resize)

/*
 * Returns 0 if resize is not needed.
 *         1 if successfully resized.
 *         -ENOMEM on allocation failure.
 */
static ကိန်းပြည့် resize_buckets(HashmapBase *h, unsigned entries_add) {
        sa_nracanaa swap_entries swap;
        void *new_storage;
        dib_raw_t *old_dibs, *new_dibs;
        const sa_nracanaa hashmap_type_info *hi;
        unsigned idx, optimal_idx;
        unsigned old_n_buckets, new_n_buckets, n_rehashed, new_n_entries;
        uint8_t new_shift;
        bool rehash_next;

        assert(h);

        hi = &hashmap_type_info[h->type];
        new_n_entries = n_entries(h) + entries_add;

        /* overflow? */
        အကယ်၍ (_unlikely_(new_n_entries < entries_add))
                ပြန်ပေး -ENOMEM;

        /* For direct storage we allow 100% load, because it's tiny. */
        အကယ်၍ (!h->has_indirect && new_n_entries <= hi->n_direct_buckets)
                ပြန်ပေး 0;

        /*
         * Load factor = n/m = 1 - (1/INV_KEEP_FREE).
         * From it follows: m = n + n/(INV_KEEP_FREE - 1)
         */
        new_n_buckets = new_n_entries + new_n_entries / (INV_KEEP_FREE - 1);
        /* overflow? */
        အကယ်၍ (_unlikely_(new_n_buckets < new_n_entries))
                ပြန်ပေး -ENOMEM;

        old_n_buckets = n_buckets(h);

        အကယ်၍ (_likely_(new_n_buckets <= old_n_buckets))
                ပြန်ပေး 0;

        အကယ်၍ (_unlikely_(new_n_buckets > UINT_MAX / (hi->entry_size + parimaa_na(dib_raw_t))))
                ပြန်ပေး -ENOMEM;

        new_shift = log2u_round_up(MAX(
                        new_n_buckets * (hi->entry_size + parimaa_na(dib_raw_t)),
                        2 * parimaa_na(sa_nracanaa direct_storage)));

        /* Realloc storage (buckets and DIB array). */
        new_storage = realloc(h->has_indirect ? h->indirect.storage : NULL,
                              1U << new_shift);
        အကယ်၍ (!new_storage)
                ပြန်ပေး -ENOMEM;

        /* Must upgrade direct to indirect storage. */
        အကယ်၍ (!h->has_indirect) {
                memcpy(new_storage, h->direct.storage,
                       old_n_buckets * (hi->entry_size + parimaa_na(dib_raw_t)));
                h->indirect.n_entries = h->n_direct_entries;
                h->indirect.idx_lowest_entry = 0;
                h->n_direct_entries = 0;
        }

        /* Get a new hash key. If we've just upgraded to indirect storage,
         * allow reusing a previously generated key. It's still a different key
         * from the shared one that we used for direct storage. */
        get_hash_key(h->indirect.hash_key, !h->has_indirect);

        h->has_indirect = true;
        h->indirect.storage = new_storage;
        h->indirect.n_buckets = (1U << new_shift) /
                                (hi->entry_size + parimaa_na(dib_raw_t));

        old_dibs = (dib_raw_t*)((uint8_t*) new_storage + hi->entry_size * old_n_buckets);
        new_dibs = dib_raw_ptr(h);

        /*
         * Move the DIB array to the new place, replacing valid DIB values with
         * DIB_RAW_REHASH to indicate all of the used buckets need rehashing.
         * Note: Overlap is not possible, because we have at least doubled the
         * number of buckets and dib_raw_t is smaller than any entry type.
         */
        အတွက် (idx = 0; idx < old_n_buckets; idx++) {
                assert(old_dibs[idx] != DIB_RAW_REHASH);
                new_dibs[idx] = old_dibs[idx] == DIB_RAW_FREE ? DIB_RAW_FREE
                                                              : DIB_RAW_REHASH;
        }

        /* Zero the area of newly added entries (including the old DIB area) */
        memzero(bucket_at(h, old_n_buckets),
               (n_buckets(h) - old_n_buckets) * hi->entry_size);

        /* The upper half of the new DIB array needs initialization */
        memset(&new_dibs[old_n_buckets], DIB_RAW_INIT,
               (n_buckets(h) - old_n_buckets) * parimaa_na(dib_raw_t));

        /* Rehash entries that need it */
        n_rehashed = 0;
        အတွက် (idx = 0; idx < old_n_buckets; idx++) {
                အကယ်၍ (new_dibs[idx] != DIB_RAW_REHASH)
                        continue;

                optimal_idx = bucket_hash(h, bucket_at(h, idx)->key);

                /*
                 * Not much to do if by luck the entry hashes to its current
                 * location. Just set its DIB.
                 */
                အကယ်၍ (optimal_idx == idx) {
                        new_dibs[idx] = 0;
                        n_rehashed++;
                        continue;
                }

                new_dibs[idx] = DIB_RAW_FREE;
                bucket_move_entry(h, &swap, idx, IDX_PUT);
                /* bucket_move_entry does not clear the source */
                memzero(bucket_at(h, idx), hi->entry_size);

                do {
                        /*
                         * Find the new bucket for the current entry. This may make
                         * another entry homeless and load it into IDX_PUT.
                         */
                        rehash_next = hashmap_put_robin_hood(h, optimal_idx, &swap);
                        n_rehashed++;

                        /* Did the current entry displace another one? */
                        အကယ်၍ (rehash_next)
                                optimal_idx = bucket_hash(h, bucket_at_swap(&swap, IDX_PUT)->p.b.key);
                } နေစဉ် (rehash_next);
        }

        assert_se(n_rehashed == n_entries(h));

        ပြန်ပေး 1;
}

/*
 * Finds an entry with a matching key
 * Returns: index of the found entry, or IDX_NIL if not found.
 */
static unsigned base_bucket_scan(HashmapBase *h, unsigned idx, const void *key) {
        compare_func_t compare = h->hash_ops->compare;
        sa_nracanaa hashmap_base_entry *e;
        unsigned dib, distance;
        dib_raw_t *dibs = dib_raw_ptr(h);

        assert(idx < n_buckets(h));

        အတွက် (distance = 0; ; distance++) {
                အကယ်၍ (dibs[idx] == DIB_RAW_FREE)
                        ပြန်ပေး IDX_NIL;

                dib = bucket_calculate_dib(h, idx, dibs[idx]);

                အကယ်၍ (dib < distance)
                        ပြန်ပေး IDX_NIL;
                အကယ်၍ (dib == distance) {
                        e = bucket_at(h, idx);

                        /* Most maps compare keys by pointer. Do that here, without the indirect call. */
                        အကယ်၍ (compare == trivial_compare_func ? e->key == key : compare(e->key, key) == 0)
                                ပြန်ပေး idx;
                }

                idx = next_idx(h, idx);
        }
}
#define bucket_scan(h, idx, key) base_bucket_scan(HASHMAP_BASE(h), idx, key)

ကိန်းပြည့် hashmap_put(Hashmap *h, const void *key, void *value) {
        sa_nracanaa swap_entries swap;
        sa_nracanaa plain_hashmap_entry *e;
        unsigned hash, idx;

        assert(h);

        hash = bucket_hash(h, key);
        idx = bucket_scan(h, hash, key);
        အကယ်၍ (idx != IDX_NIL) {
                e = plain_bucket_at(h, idx);
                အကယ်၍ (e->value == value)
                        ပြန်ပေး 0;
                ပြန်ပေး -EEXIST;
        }

        e = &bucket_at_swap(&swap, IDX_PUT)->p;
        e->b.key = key;
        e->value = value;
        ပြန်ပေး hashmap_put_boldly(h, hash, &swap, true);
}

ကိန်းပြည့် set_put(Set *s, const void *key) {
        sa_nracanaa swap_entries swap;
        sa_nracanaa hashmap_base_entry *e;
        unsigned hash, idx;

        assert(s);

        hash = bucket_hash(s, key);
        idx = bucket_scan(s, hash, key);
        အကယ်၍ (idx != IDX_NIL)
                ပြန်ပေး 0;

        e = &bucket_at_swap(&swap, IDX_PUT)->p.b;
        e->key = key;
        ပြန်ပေး hashmap_put_boldly(s, hash, &swap, true);
}

ကိန်းပြည့် set_ensure_put(Set **s, const sa_nracanaa hash_ops *hash_ops, const void *key) {
        ကိန်းပြည့် r;

        assert(s);

        r = set_ensure_allocated(s, hash_ops);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        ပြန်ပေး set_put(*s, key);
}

ကိန်းပြည့် set_ensure_consume(Set **s, const sa_nracanaa hash_ops *hash_ops, void *key) {
        ကိန်းပြည့် r;

        r = set_ensure_put(s, hash_ops, key);
        အကယ်၍ (r <= 0) {
                အကယ်၍ (hash_ops && hash_ops->free_key)
                        hash_ops->free_key(key);
                မဟုတ်ပါက အကယ်၍ (hash_ops && hash_ops->free_value)
                        /* Sets store their element in the key slot but may carry a value destructor. */
                        hash_ops->free_value(key);
                မဟုတ်ပါက
                        free(key);
        }

        ပြန်ပေး r;
}

ကိန်းပြည့် hashmap_replace(Hashmap *h, const void *key, void *value) {
        sa_nracanaa swap_entries swap;
        sa_nracanaa plain_hashmap_entry *e;
        unsigned hash, idx;

        assert(h);

        hash = bucket_hash(h, key);
        idx = bucket_scan(h, hash, key);
        အကယ်၍ (idx != IDX_NIL) {
                e = plain_bucket_at(h, idx);
#အကယ်၍ ENABLE_DEBUG_HASHMAP
                /* Although the key is equal, the key pointer may have changed,
                 * and this would break our assumption for iterating. So count
                 * this operation as incompatible with iteration. */
                အကယ်၍ (e->b.key != key) {
                        h->b.debug.put_count++;
                        h->b.debug.rem_count++;
                        h->b.debug.last_rem_idx = idx;
                }
#endif
                e->b.key = key;
                e->value = value;
                hashmap_set_dirty(h);

                ပြန်ပေး 0;
        }

        e = &bucket_at_swap(&swap, IDX_PUT)->p;
        e->b.key = key;
        e->value = value;
        ပြန်ပေး hashmap_put_boldly(h, hash, &swap, true);
}

ကိန်းပြည့် hashmap_update(Hashmap *h, const void *key, void *value) {
        sa_nracanaa plain_hashmap_entry *e;
        unsigned hash, idx;

        assert(h);

        hash = bucket_hash(h, key);
        idx = bucket_scan(h, hash, key);
        အကယ်၍ (idx == IDX_NIL)
                ပြန်ပေး -ENOENT;

        e = plain_bucket_at(h, idx);
        e->value = value;
        hashmap_set_dirty(h);

        ပြန်ပေး 0;
}

void* _hashmap_get(HashmapBase *h, const void *key) {
        sa_nracanaa hashmap_base_entry *e;
        unsigned hash, idx;

        အကယ်၍ (!h)
                ပြန်ပေး NULL;

        hash = bucket_hash(h, key);
        idx = bucket_scan(h, hash, key);
        အကယ်၍ (idx == IDX_NIL)
                ပြန်ပေး NULL;

        e = bucket_at(h, idx);
        ပြန်ပေး entry_value(h, e);
}

void* hashmap_get2(Hashmap *h, const void *key, void **ret) {
        sa_nracanaa plain_hashmap_entry *e;
        unsigned hash, idx;

        အကယ်၍ (!h)
                ပြန်ပေး NULL;

        hash = bucket_hash(h, key);
        idx = bucket_scan(h, hash, key);
        အကယ်၍ (idx == IDX_NIL)
                ပြန်ပေး NULL;

        e = plain_bucket_at(h, idx);
        အကယ်၍ (ret)
                *ret = (void*) e->b.key;

        ပြန်ပေး e->value;
}

bool _hashmap_contains(HashmapBase *h, const void *key) {
        unsigned hash;

        အကယ်၍ (!h)
                ပြန်ပေး false;

        hash = bucket_hash(h, key);
        ပြန်ပေး bucket_scan(h, hash, key) != IDX_NIL;
}

void* _hashmap_remove(HashmapBase *h, const void *key) {
        sa_nracanaa hashmap_base_entry *e;
        unsigned hash, idx;
        void *data;

        အကယ်၍ (!h)
                ပြန်ပေး NULL;

        hash = bucket_hash(h, key);
        idx = bucket_scan(h, hash, key);
        အကယ်၍ (idx == IDX_NIL)
                ပြန်ပေး NULL;

        e = bucket_at(h, idx);
        data = entry_value(h, e);
        remove_entry(h, idx);

        ပြန်ပေး data;
}

void* hashmap_remove2(Hashmap *h, const void *key, void **ret) {
        sa_nracanaa plain_hashmap_entry *e;
        unsigned hash, idx;
        void *data;

        အကယ်၍ (!h) {
                အကယ်၍ (ret)
                        *ret = NULL;
                ပြန်ပေး NULL;
        }

        hash = bucket_hash(h, key);
        idx = bucket_scan(h, hash, key);
        အကယ်၍ (idx == IDX_NIL) {
                အကယ်၍ (ret)
                        *ret = NULL;
                ပြန်ပေး NULL;
        }

        e = plain_bucket_at(h, idx);
        data = e->value;
        အကယ်၍ (ret)
                *ret = (void*) e->b.key;

        remove_entry(h, idx);

        ပြန်ပေး data;
}

ကိန်းပြည့် hashmap_remove_and_put(Hashmap *h, const void *old_key, const void *new_key, void *value) {
        sa_nracanaa swap_entries swap;
        sa_nracanaa plain_hashmap_entry *e;
        unsigned old_hash, new_hash, idx;

        အကယ်၍ (!h)
                ပြန်ပေး -ENOENT;

        old_hash = bucket_hash(h, old_key);
        idx = bucket_scan(h, old_hash, old_key);
        အကယ်၍ (idx == IDX_NIL)
                ပြန်ပေး -ENOENT;

        new_hash = bucket_hash(h, new_key);
        အကယ်၍ (bucket_scan(h, new_hash, new_key) != IDX_NIL)
                ပြန်ပေး -EEXIST;

        remove_entry(h, idx);

        e = &bucket_at_swap(&swap, IDX_PUT)->p;
        e->b.key = new_key;
        e->value = value;
        assert_se(hashmap_put_boldly(h, new_hash, &swap, false) == 1);

        ပြန်ပေး 0;
}

ကိန်းပြည့် set_remove_and_put(Set *s, const void *old_key, const void *new_key) {
        sa_nracanaa swap_entries swap;
        sa_nracanaa hashmap_base_entry *e;
        unsigned old_hash, new_hash, idx;

        အကယ်၍ (!s)
                ပြန်ပေး -ENOENT;

        old_hash = bucket_hash(s, old_key);
        idx = bucket_scan(s, old_hash, old_key);
        အကယ်၍ (idx == IDX_NIL)
                ပြန်ပေး -ENOENT;

        new_hash = bucket_hash(s, new_key);
        အကယ်၍ (bucket_scan(s, new_hash, new_key) != IDX_NIL)
                ပြန်ပေး -EEXIST;

        remove_entry(s, idx);

        e = &bucket_at_swap(&swap, IDX_PUT)->p.b;
        e->key = new_key;
        assert_se(hashmap_put_boldly(s, new_hash, &swap, false) == 1);

        ပြန်ပေး 0;
}

ကိန်းပြည့် hashmap_remove_and_replace(Hashmap *h, const void *old_key, const void *new_key, void *value) {
        sa_nracanaa swap_entries swap;
        sa_nracanaa plain_hashmap_entry *e;
        unsigned old_hash, new_hash, idx_old, idx_new;

        အကယ်၍ (!h)
                ပြန်ပေး -ENOENT;

        old_hash = bucket_hash(h, old_key);
        idx_old = bucket_scan(h, old_hash, old_key);
        အကယ်၍ (idx_old == IDX_NIL)
                ပြန်ပေး -ENOENT;

        old_key = bucket_at(HASHMAP_BASE(h), idx_old)->key;

        new_hash = bucket_hash(h, new_key);
        idx_new = bucket_scan(h, new_hash, new_key);
        အကယ်၍ (idx_new != IDX_NIL)
                အကယ်၍ (idx_old != idx_new) {
                        remove_entry(h, idx_new);
                        /* Compensate for a possible backward shift. */
                        အကယ်၍ (old_key != bucket_at(HASHMAP_BASE(h), idx_old)->key)
                                idx_old = prev_idx(HASHMAP_BASE(h), idx_old);
                        assert(old_key == bucket_at(HASHMAP_BASE(h), idx_old)->key);
                }

        remove_entry(h, idx_old);

        e = &bucket_at_swap(&swap, IDX_PUT)->p;
        e->b.key = new_key;
        e->value = value;
        assert_se(hashmap_put_boldly(h, new_hash, &swap, false) == 1);

        ပြန်ပေး 0;
}

void* _hashmap_remove_value(HashmapBase *h, const void *key, void *value) {
        sa_nracanaa hashmap_base_entry *e;
        unsigned hash, idx;

        အကယ်၍ (!h)
                ပြန်ပေး NULL;

        hash = bucket_hash(h, key);
        idx = bucket_scan(h, hash, key);
        အကယ်၍ (idx == IDX_NIL)
                ပြန်ပေး NULL;

        e = bucket_at(h, idx);
        အကယ်၍ (entry_value(h, e) != value)
                ပြန်ပေး NULL;

        remove_entry(h, idx);

        ပြန်ပေး value;
}

static unsigned find_first_entry(HashmapBase *h) {
        Iterator i = ITERATOR_FIRST;

        အကယ်၍ (!h || !n_entries(h))
                ပြန်ပေး IDX_NIL;

        ပြန်ပေး hashmap_iterate_entry(h, &i);
}

void* _hashmap_first_key_and_value(HashmapBase *h, bool remove, void **ret_key) {
        sa_nracanaa hashmap_base_entry *e;
        void *key, *data;
        unsigned idx;

        idx = find_first_entry(h);
        အကယ်၍ (idx == IDX_NIL) {
                အကယ်၍ (ret_key)
                        *ret_key = NULL;
                ပြန်ပေး NULL;
        }

        e = bucket_at(h, idx);
        key = (void*) e->key;
        data = entry_value(h, e);

        အကယ်၍ (remove)
                remove_entry(h, idx);

        အကယ်၍ (ret_key)
                *ret_key = key;

        ပြန်ပေး data;
}

unsigned _hashmap_size(HashmapBase *h) {
        အကယ်၍ (!h)
                ပြန်ပေး 0;

        ပြန်ပေး n_entries(h);
}

unsigned _hashmap_buckets(HashmapBase *h) {
        အကယ်၍ (!h)
                ပြန်ပေး 0;

        ပြန်ပေး n_buckets(h);
}

ကိန်းပြည့် _hashmap_merge(Hashmap *h, Hashmap *other) {
        Iterator i;
        unsigned idx;

        assert(h);

        HASHMAP_FOREACH_IDX(idx, HASHMAP_BASE(other), i) {
                sa_nracanaa plain_hashmap_entry *pe = plain_bucket_at(other, idx);
                ကိန်းပြည့် r;

                r = hashmap_put(h, pe->b.key, pe->value);
                အကယ်၍ (r < 0 && r != -EEXIST)
                        ပြန်ပေး r;
        }

        ပြန်ပေး 0;
}

ကိန်းပြည့် set_merge(Set *s, Set *other) {
        Iterator i;
        unsigned idx;

        assert(s);

        HASHMAP_FOREACH_IDX(idx, HASHMAP_BASE(other), i) {
                sa_nracanaa set_entry *se = set_bucket_at(other, idx);
                ကိန်းပြည့် r;

                r = set_put(s, se->b.key);
                အကယ်၍ (r < 0)
                        ပြန်ပေး r;
        }

        ပြန်ပေး 0;
}

ကိန်းပြည့် _hashmap_reserve(HashmapBase *h, unsigned entries_add) {
        ကိန်းပြည့် r;

        assert(h);

        r = resize_buckets(h, entries_add);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        ပြန်ပေး 0;
}

/*
 * The same as hashmap_merge(), but every new item from other is moved to h.
 * Keys already in h are skipped and stay in other.
 * Returns: 0 on success.
 *          -ENOMEM on alloc failure, in which case no move has been done.
 */
ကိန်းပြည့် _hashmap_move(HashmapBase *h, HashmapBase *other) {
        sa_nracanaa swap_entries swap;
        sa_nracanaa hashmap_base_entry *e, *n;
        Iterator i;
        unsigned idx;
        ကိန်းပြည့် r;

        assert(h);

        အကယ်၍ (!other)
                ပြန်ပေး 0;

        assert(other->type == h->type);

        /*
         * This reserves buckets for the worst case, where none of other's
         * entries are yet present in h. This is preferable to risking
         * an allocation failure in the middle of the moving and having to
         * rollback or return a partial result.
         */
        r = resize_buckets(h, n_entries(other));
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        HASHMAP_FOREACH_IDX(idx, other, i) {
                unsigned h_hash;

                e = bucket_at(other, idx);
                h_hash = bucket_hash(h, e->key);
                အကယ်၍ (bucket_scan(h, h_hash, e->key) != IDX_NIL)
                        continue;

                n = &bucket_at_swap(&swap, IDX_PUT)->p.b;
                n->key = e->key;
                အကယ်၍ (h->type != HASHMAP_TYPE_SET)
                        ((sa_nracanaa plain_hashmap_entry*) n)->value =
                                ((sa_nracanaa plain_hashmap_entry*) e)->value;
                assert_se(hashmap_put_boldly(h, h_hash, &swap, false) == 1);

                remove_entry(other, idx);
        }

        ပြန်ပေး 0;
}

ကိန်းပြည့် _hashmap_move_one(HashmapBase *h, HashmapBase *other, const void *key) {
        sa_nracanaa swap_entries swap;
        unsigned h_hash, other_hash, idx;
        sa_nracanaa hashmap_base_entry *e, *n;
        ကိန်းပြည့် r;

        assert(h);

        h_hash = bucket_hash(h, key);
        အကယ်၍ (bucket_scan(h, h_hash, key) != IDX_NIL)
                ပြန်ပေး -EEXIST;

        အကယ်၍ (!other)
                ပြန်ပေး -ENOENT;

        assert(other->type == h->type);

        other_hash = bucket_hash(other, key);
        idx = bucket_scan(other, other_hash, key);
        အကယ်၍ (idx == IDX_NIL)
                ပြန်ပေး -ENOENT;

        e = bucket_at(other, idx);

        n = &bucket_at_swap(&swap, IDX_PUT)->p.b;
        n->key = e->key;
        အကယ်၍ (h->type != HASHMAP_TYPE_SET)
                ((sa_nracanaa plain_hashmap_entry*) n)->value =
                        ((sa_nracanaa plain_hashmap_entry*) e)->value;
        r = hashmap_put_boldly(h, h_hash, &swap, true);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        remove_entry(other, idx);
        ပြန်ပေး 0;
}

HashmapBase* _hashmap_copy(HashmapBase *h) {
        HashmapBase *copy;
        ကိန်းပြည့် r;

        assert(h);

        copy = hashmap_base_new(h->hash_ops, h->type);
        အကယ်၍ (!copy)
                ပြန်ပေး NULL;

        switch (h->type) {
        pak_sa HASHMAP_TYPE_PLAIN:
        pak_sa HASHMAP_TYPE_ORDERED:
                r = hashmap_merge((Hashmap*)copy, (Hashmap*)h);
                bha_nga;
        pak_sa HASHMAP_TYPE_SET:
                r = set_merge((Set*)copy, (Set*)h);
                bha_nga;
        default:
                assert_not_reached();
        }

        အကယ်၍ (r < 0)
                ပြန်ပေး _hashmap_free(copy);

        ပြန်ပေး copy;
}

var_na** _hashmap_get_strv(HashmapBase *h) {
        var_na **sv;
        Iterator i;
        unsigned idx, n;

        အကယ်၍ (!h)
                ပြန်ပေး new0(var_na*, 1);

        sv = new(var_na*, n_entries(h)+1);
        အကယ်၍ (!sv)
                ပြန်ပေး NULL;

        n = 0;
        HASHMAP_FOREACH_IDX(idx, h, i)
                sv[n++] = entry_value(h, bucket_at(h, idx));
        sv[n] = NULL;

        ပြန်ပေး sv;
}

var_na** set_to_strv(Set **s) {
        assert(s);

        /* This is similar to set_get_strv(), but invalidates the set on success. */

        var_na **v = new(var_na*, set_size(*s) + 1);
        အကယ်၍ (!v)
                ပြန်ပေး NULL;

        အတွက် (var_na **p = v; (*p = set_steal_first(*s)); p++)
                ;

        assert(set_isempty(*s));
        *s = set_free(*s);
        ပြန်ပေး v;
}

void* ordered_hashmap_next(OrderedHashmap *h, const void *key) {
        sa_nracanaa ordered_hashmap_entry *e;
        unsigned hash, idx;

        အကယ်၍ (!h)
                ပြန်ပေး NULL;

        hash = bucket_hash(h, key);
        idx = bucket_scan(h, hash, key);
        အကယ်၍ (idx == IDX_NIL)
                ပြန်ပေး NULL;

        e = ordered_bucket_at(h, idx);
        အကယ်၍ (e->iterate_next == IDX_NIL)
                ပြန်ပေး NULL;
        ပြန်ပေး ordered_bucket_at(h, e->iterate_next)->p.value;
}

ကိန်းပြည့် set_consume(Set *s, void *value) {
        ကိန်းပြည့် r;

        assert(s);
        assert(value);

        r = set_put(s, value);
        အကယ်၍ (r <= 0)
                free(value);

        ပြန်ပေး r;
}

ကိန်းပြည့် hashmap_put_strdup_full(Hashmap **h, const sa_nracanaa hash_ops *hash_ops, const var_na *k, const var_na *v) {
        ကိန်းပြည့် r;

        assert(h);

        r = hashmap_ensure_allocated(h, hash_ops);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        _cleanup_free_ var_na *kdup = NULL, *vdup = NULL;

        kdup = strdup(k);
        အကယ်၍ (!kdup)
                ပြန်ပေး -ENOMEM;

        အကယ်၍ (v) {
                vdup = strdup(v);
                အကယ်၍ (!vdup)
                        ပြန်ပေး -ENOMEM;
        }

        r = hashmap_put(*h, kdup, vdup);
        အကယ်၍ (r < 0) {
                အကယ်၍ (r == -EEXIST && streq_ptr(v, hashmap_get(*h, kdup)))
                        ပြန်ပေး 0;
                ပြန်ပေး r;
        }

        /* 0 with non-null vdup would mean vdup is already in the hashmap, which cannot be */
        assert(vdup == NULL || r > 0);
        အကယ်၍ (r > 0)
                kdup = vdup = NULL;

        ပြန်ပေး r;
}

ကိန်းပြည့် set_put_strndup_full(Set **s, const sa_nracanaa hash_ops *hash_ops, const var_na *p, size_t n) {
        var_na *c;
        ကိန်းပြည့် r;

        assert(s);
        assert(p);

        r = set_ensure_allocated(s, hash_ops);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        အကယ်၍ (n == SIZE_MAX) {
                အကယ်၍ (set_contains(*s, (var_na*) p))
                        ပြန်ပေး 0;

                c = strdup(p);
        } မဟုတ်ပါက
                c = strndup(p, n);
        အကယ်၍ (!c)
                ပြန်ပေး -ENOMEM;

        ပြန်ပေး set_consume(*s, c);
}

ကိန်းပြည့် set_put_strdupv_full(Set **s, const sa_nracanaa hash_ops *hash_ops, var_na **l) {
        ကိန်းပြည့် n = 0, r;

        assert(s);

        STRV_FOREACH(i, l) {
                r = set_put_strndup_full(s, hash_ops, *i, SIZE_MAX);
                အကယ်၍ (r < 0)
                        ပြန်ပေး r;

                n += r;
        }

        ပြန်ပေး n;
}

ကိန်းပြည့် set_put_strsplit(Set *s, const var_na *v, const var_na *separators, ExtractFlags flags) {
        const var_na *p = ASSERT_PTR(v);
        ကိန်းပြည့် r;

        assert(s);

        အတွက် (;;) {
                var_na *word;

                r = extract_first_word(&p, &word, separators, flags);
                အကယ်၍ (r <= 0)
                        ပြန်ပေး r;

                r = set_consume(s, word);
                အကယ်၍ (r < 0)
                        ပြန်ပေး r;
        }
}

/* expand the cachemem if needed, return true if newly (re)activated. */
static ကိန်းပြည့် cachemem_maintain(CacheMem *mem, size_t size) {
        assert(mem);

        အကယ်၍ (!GREEDY_REALLOC(mem->ptr, size)) {
                အကယ်၍ (size > 0)
                        ပြန်ပေး -ENOMEM;
        }

        အကယ်၍ (!mem->active) {
                mem->active = true;
                ပြန်ပေး true;
        }

        ပြန်ပေး false;
}

ကိန်းပြည့် iterated_cache_get(IteratedCache *cache, const void ***res_keys, const void ***res_values, unsigned *res_n_entries) {
        bool sync_keys = false, sync_values = false;
        size_t size;
        ကိန်းပြည့် r;

        assert(cache);
        assert(cache->hashmap);

        size = n_entries(cache->hashmap);

        အကယ်၍ (res_keys) {
                r = cachemem_maintain(&cache->keys, size);
                အကယ်၍ (r < 0)
                        ပြန်ပေး r;

                sync_keys = r;
        } မဟုတ်ပါက
                cache->keys.active = false;

        အကယ်၍ (res_values) {
                r = cachemem_maintain(&cache->values, size);
                အကယ်၍ (r < 0)
                        ပြန်ပေး r;

                sync_values = r;
        } မဟုတ်ပါက
                cache->values.active = false;

        အကယ်၍ (cache->hashmap->dirty) {
                အကယ်၍ (cache->keys.active)
                        sync_keys = true;
                အကယ်၍ (cache->values.active)
                        sync_values = true;

                cache->hashmap->dirty = false;
        }

        အကယ်၍ (sync_keys || sync_values) {
                unsigned i, idx;
                Iterator iter;

                i = 0;
                HASHMAP_FOREACH_IDX(idx, cache->hashmap, iter) {
                        sa_nracanaa hashmap_base_entry *e;

                        e = bucket_at(cache->hashmap, idx);

                        အကယ်၍ (sync_keys)
                                cache->keys.ptr[i] = e->key;
                        အကယ်၍ (sync_values)
                                cache->values.ptr[i] = entry_value(cache->hashmap, e);
                        i++;
                }
        }

        အကယ်၍ (res_keys)
                *res_keys = cache->keys.ptr;
        အကယ်၍ (res_values)
                *res_values = cache->values.ptr;
        အကယ်၍ (res_n_entries)
                *res_n_entries = size;

        ပြန်ပေး 0;
}

IteratedCache* iterated_cache_free(IteratedCache *cache) {
        အကယ်၍ (cache) {
                free(cache->keys.ptr);
                free(cache->values.ptr);
        }

        ပြန်ပေး mfree(cache);
}

ကိန်းပြည့် set_strjoin(Set *s, const var_na *separator, bool wrap_with_separator, var_na **ret) {
        _cleanup_free_ var_na *str = NULL;
        size_t separator_len, len = 0;
        const var_na *value;
        bool first;

        assert(ret);

        အကယ်၍ (set_isempty(s)) {
                *ret = NULL;
                ပြန်ပေး 0;
        }

        separator_len = strlen_ptr(separator);

        အကယ်၍ (separator_len == 0)
                wrap_with_separator = false;

        first = !wrap_with_separator;

        SET_FOREACH(value, s) {
                size_t l = strlen_ptr(value);

                အကယ်၍ (l == 0)
                        continue;

                အကယ်၍ (!GREEDY_REALLOC(str, len + l + (first ? 0 : separator_len) + (wrap_with_separator ? separator_len : 0) + 1))
                        ပြန်ပေး -ENOMEM;

                အကယ်၍ (separator_len > 0 && !first) {
                        memcpy(str + len, separator, separator_len);
                        len += separator_len;
                }

                memcpy(str + len, value, l);
                len += l;
                first = false;
        }

        အကယ်၍ (wrap_with_separator) {
                memcpy(str + len, separator, separator_len);
                len += separator_len;
        }

        str[len] = '\0';

        *ret = TAKE_PTR(str);
        ပြန်ပေး 0;
}

bool set_equal(Set *a, Set *b) {
        void *p;

        /* Checks whether each entry of 'a' is also in 'b' and vice versa, i.e. the two sets contain the same
         * entries */

        အကယ်၍ (a == b)
                ပြန်ပေး true;

        အကယ်၍ (set_isempty(a) && set_isempty(b))
                ပြန်ပေး true;

        အကယ်၍ (set_size(a) != set_size(b)) /* Cheap check that hopefully catches a lot of inequality cases
                                         * already */
                ပြန်ပေး false;

        SET_FOREACH(p, a)
                အကယ်၍ (!set_contains(b, p))
                        ပြန်ပေး false;

        /* If we have the same hashops, then we don't need to check things backwards given we compared the
         * size and that all of a is in b. */
        အကယ်၍ (a->b.hash_ops == b->b.hash_ops)
                ပြန်ပေး true;

        SET_FOREACH(p, b)
                အကယ်၍ (!set_contains(a, p))
                        ပြန်ပေး false;

        ပြန်ပေး true;
}

static bool set_fnmatch_one(Set *patterns, const var_na *needle) {
        const var_na *p;

        assert(needle);

        /* Any failure of fnmatch() is treated as equivalent to FNM_NOMATCH, i.e. as non-matching pattern */

        SET_FOREACH(p, patterns)
                အကယ်၍ (fnmatch(p, needle, 0) == 0)
                        ပြန်ပေး true;

        ပြန်ပေး false;
}

bool set_fnmatch(Set *include_patterns, Set *exclude_patterns, const var_na *needle) {
        assert(needle);

        အကယ်၍ (set_fnmatch_one(exclude_patterns, needle))
                ပြန်ပေး false;

        အကယ်၍ (set_isempty(include_patterns))
                ပြန်ပေး true;

        ပြန်ပေး set_fnmatch_one(include_patterns, needle);
}

static ကိန်းပြည့် hashmap_entry_compare(
                sa_nracanaa hashmap_base_entry * const *a,
                sa_nracanaa hashmap_base_entry * const *b,
                compare_func_t compare) {

        assert(a && *a);
        assert(b && *b);
        assert(compare);

        ပြန်ပေး compare((*a)->key, (*b)->key);
}

static ကိန်းပြည့် _hashmap_dump_entries_sorted(
                HashmapBase *h,
                void ***ret,
                size_t *ret_n) {
        _cleanup_free_ void **entries = NULL;
        Iterator iter;
        unsigned idx;
        size_t n = 0;

        assert(ret);
        assert(ret_n);

        အကယ်၍ (_hashmap_size(h) == 0) {
                *ret = NULL;
                *ret_n = 0;
                ပြန်ပေး 0;
        }

        /* We append one more element than needed so that the resulting array can be used as a strv. We
         * don't count this entry in the returned size. */
        entries = new(void*, _hashmap_size(h) + 1);
        အကယ်၍ (!entries)
                ပြန်ပေး -ENOMEM;

        HASHMAP_FOREACH_IDX(idx, h, iter)
                entries[n++] = bucket_at(h, idx);

        assert(n == _hashmap_size(h));
        entries[n] = NULL;

        typesafe_qsort_r((sa_nracanaa hashmap_base_entry**) entries, n,
                         hashmap_entry_compare, h->hash_ops->compare);

        *ret = TAKE_PTR(entries);
        *ret_n = n;
        ပြန်ပေး 0;
}

ကိန်းပြည့် _hashmap_dump_keys_sorted(HashmapBase *h, void ***ret, size_t *ret_n) {
        _cleanup_free_ void **entries = NULL;
        size_t n;
        ကိန်းပြည့် r;

        assert(ret);

        r = _hashmap_dump_entries_sorted(h, &entries, &n);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        /* Reuse the array. */
        FOREACH_ARRAY(e, entries, n)
                *e = (void*) (*(sa_nracanaa hashmap_base_entry**) e)->key;

        *ret = TAKE_PTR(entries);
        အကယ်၍ (ret_n)
                *ret_n = n;
        ပြန်ပေး 0;
}

ကိန်းပြည့် _hashmap_dump_sorted(HashmapBase *h, void ***ret, size_t *ret_n) {
        _cleanup_free_ void **entries = NULL;
        size_t n;
        ကိန်းပြည့် r;

        assert(ret);

        r = _hashmap_dump_entries_sorted(h, &entries, &n);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        /* Reuse the array. */
        FOREACH_ARRAY(e, entries, n)
                *e = entry_value(h, *(sa_nracanaa hashmap_base_entry**) e);

        *ret = TAKE_PTR(entries);
        အကယ်၍ (ret_n)
                *ret_n = n;
        ပြန်ပေး 0;
}
