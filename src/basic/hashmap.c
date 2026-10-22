/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <fnmatch.h>
#ضمن <pthread.h>
#ضمن <unistd.h>
#ܐܢ HAVE_VALGRIND_VALGRIND_H
#  include <valgrind/valgrind.h>
#endif

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

#ܐܢ ENABLE_DEBUG_HASHMAP
#ضمن "list.h"
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
بنية hashmap_base_entry {
        ثابت فراغ *key;
};

/* Entry types for specific hashmap/set types
 * hashmap_base_entry must be at the beginning of each entry struct. */

بنية plain_hashmap_entry {
        بنية hashmap_base_entry b;
        فراغ *value;
};

بنية ordered_hashmap_entry {
        بنية plain_hashmap_entry p;
        unsigned iterate_next, iterate_previous;
};

بنية set_entry {
        بنية hashmap_base_entry b;
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
بنية swap_entries {
        بنية ordered_hashmap_entry e[_IDX_SWAP_END - _IDX_SWAP_BEGIN];
};

/* Distance from Initial Bucket */
تعريفنوع uint8_t dib_raw_t;
#define DIB_RAW_OVERFLOW ((dib_raw_t)0xfdU)   /* indicates DIB value is greater than representable */
#define DIB_RAW_REHASH   ((dib_raw_t)0xfeU)   /* entry yet to be rehashed during in-place resize */
#define DIB_RAW_FREE     ((dib_raw_t)0xffU)   /* a free bucket */
#define DIB_RAW_INIT     ((حرف)DIB_RAW_FREE) /* a byte to memset a DIB store with when initializing */

#define DIB_FREE UINT_MAX

#ܐܢ ENABLE_DEBUG_HASHMAP
بنية hashmap_debug_info {
        LIST_FIELDS(بنية hashmap_debug_info, debug_list);
        unsigned max_entries;  /* high watermark of n_entries */

        /* fields to detect modification while iterating */
        unsigned put_count;    /* counts puts into the hashmap */
        unsigned rem_count;    /* counts removals from hashmap */
        unsigned last_rem_idx; /* remembers last removal index */
};

/* Tracks all existing hashmaps. Get at it from gdb. See sd_dump_hashmaps.py */
ساكن LIST_HEAD(بنية hashmap_debug_info, hashmap_debug_list);
ساكن pthread_mutex_t hashmap_debug_list_mutex = PTHREAD_MUTEX_INITIALIZER;
#endif

تعداد HashmapType {
        HASHMAP_TYPE_PLAIN,
        HASHMAP_TYPE_ORDERED,
        HASHMAP_TYPE_SET,
        _HASHMAP_TYPE_MAX
};

بنية _packed_ indirect_storage {
        فراغ *storage;                     /* where buckets and DIBs are stored */
        uint8_t  hash_key[HASH_KEY_SIZE];  /* hash key; changes during resize */

        unsigned n_entries;                /* number of stored entries */
        unsigned n_buckets;                /* number of buckets */

        unsigned idx_lowest_entry;         /* Index below which all buckets are free.
                                              Makes "while (hashmap_steal_first())" loops
                                              O(n) instead of O(n^2) for unordered hashmaps. */
        uint8_t  _pad[3];                  /* padding for the whole HashmapBase */
        /* The bitfields in HashmapBase complete the alignment of the whole thing. */
};

بنية direct_storage {
        /* This gives us 39 bytes on 64-bit, or 35 bytes on 32-bit.
         * That's room for 4 set_entries + 4 DIB bytes + 3 unused bytes on 64-bit,
         *              or 7 set_entries + 7 DIB bytes + 0 unused bytes on 32-bit. */
        uint8_t storage[حجم(بنية indirect_storage)];
};

#define DIRECT_BUCKETS(entry_t) \
        (حجم(بنية direct_storage) / (حجم(entry_t) + حجم(dib_raw_t)))

/* We should be able to store at least one entry directly. */
assert_cc(DIRECT_BUCKETS(بنية ordered_hashmap_entry) >= 1);

/* We have 3 bits for n_direct_entries. */
assert_cc(DIRECT_BUCKETS(بنية set_entry) < (1 << 3));

/* Hashmaps with directly stored entries all use this shared hash key.
 * It's no big deal if the key is guessed, because there can be only
 * a handful of directly stored entries in a hashmap. When a hashmap
 * outgrows direct storage, it gets its own key for indirect storage. */
ساكن uint8_t shared_hash_key[HASH_KEY_SIZE];

/* Fields that all hashmap/set types must have */
بنية HashmapBase {
        ثابت بنية hash_ops *hash_ops;  /* hash and compare ops to use */

        اتحاد _packed_ {
                بنية indirect_storage indirect; /* if  has_indirect */
                بنية direct_storage direct;     /* if !has_indirect */
        };

        تعداد HashmapType type:2;     /* HASHMAP_TYPE_* */
        bool has_indirect:1;         /* whether indirect storage is used */
        unsigned n_direct_entries:3; /* Number of entries in direct storage.
                                      * Only valid if !has_indirect. */
        bool from_pool:1;            /* whether was allocated from mempool */
        bool dirty:1;                /* whether dirtied since last iterated_cache_get() */
        bool cached:1;               /* whether this hashmap is being cached */

#ܐܢ ENABLE_DEBUG_HASHMAP
        بنية hashmap_debug_info debug;
#endif
};

/* Specific hash types
 * HashmapBase must be at the beginning of each hashmap struct. */

بنية Hashmap {
        بنية HashmapBase b;
};

بنية OrderedHashmap {
        بنية HashmapBase b;
        unsigned iterate_list_head, iterate_list_tail;
};

بنية Set {
        بنية HashmapBase b;
};

تعريفنوع بنية CacheMem {
        ثابت فراغ **ptr;
        size_t n_populated;
        bool active:1;
} CacheMem;

بنية IteratedCache {
        HashmapBase *hashmap;
        CacheMem keys, values;
};

DEFINE_MEMPOOL(hashmap_pool,         Hashmap,        8);
DEFINE_MEMPOOL(ordered_hashmap_pool, OrderedHashmap, 8);
/* No need for a separate Set pool */
assert_cc(حجم(Hashmap) == حجم(Set));

بنية hashmap_type_info {
        size_t head_size;
        size_t entry_size;
        بنية mempool *mempool;
        unsigned n_direct_buckets;
};

ساكن _used_ ثابت بنية hashmap_type_info hashmap_type_info[_HASHMAP_TYPE_MAX] = {
        [HASHMAP_TYPE_PLAIN] = {
                .head_size        = حجم(Hashmap),
                .entry_size       = حجم(بنية plain_hashmap_entry),
                .mempool          = &hashmap_pool,
                .n_direct_buckets = DIRECT_BUCKETS(بنية plain_hashmap_entry),
        },
        [HASHMAP_TYPE_ORDERED] = {
                .head_size        = حجم(OrderedHashmap),
                .entry_size       = حجم(بنية ordered_hashmap_entry),
                .mempool          = &ordered_hashmap_pool,
                .n_direct_buckets = DIRECT_BUCKETS(بنية ordered_hashmap_entry),
        },
        [HASHMAP_TYPE_SET] = {
                .head_size        = حجم(Set),
                .entry_size       = حجم(بنية set_entry),
                .mempool          = &hashmap_pool,
                .n_direct_buckets = DIRECT_BUCKETS(بنية set_entry),
        },
};

فراغ hashmap_trim_pools(فراغ) {
        صحيح r;

        /* The pool is only allocated by the main thread, but the memory can be passed to other
         * threads. Let's clean up if we are the main thread and no other threads are live. */

        /* We build our own is_main_thread() here, which doesn't use C11 TLS based caching of the
         * result. That's because valgrind apparently doesn't like TLS to be used from a GCC destructor. */
        ܐܢ (getpid() != gettid())
                ܦܢܐ (فراغ) log_debug("Not cleaning up memory pools, not in main thread.");

        r = get_process_threads(0);
        ܐܢ (r < 0)
                ܦܢܐ (فراغ) log_debug_errno(r, "Failed to determine number of threads, not cleaning up memory pools: %m");
        ܐܢ (r != 1)
                ܦܢܐ (فراغ) log_debug("Not cleaning up memory pools, running in multi-threaded process.");

        mempool_trim(&hashmap_pool);
        mempool_trim(&ordered_hashmap_pool);
}

#ܐܢ HAVE_VALGRIND_VALGRIND_H
_destructor_ ساكن فراغ cleanup_pools(فراغ) {
        /* Be nice to valgrind */
        ܐܢ (RUNNING_ON_VALGRIND)
                hashmap_trim_pools();
}
#endif

ساكن مضمن unsigned n_buckets(HashmapBase *h) {
        أكد(h);
        ܦܢܐ h->has_indirect ? h->indirect.n_buckets
                               : hashmap_type_info[h->type].n_direct_buckets;
}

ساكن unsigned n_entries(HashmapBase *h) {
        أكد(h);
        ܦܢܐ h->has_indirect ? h->indirect.n_entries
                               : h->n_direct_entries;
}

ساكن فراغ n_entries_inc(HashmapBase *h) {
        أكد(h);

        ܐܢ (h->has_indirect)
                h->indirect.n_entries++;
        ܐܚܪܢܐ
                h->n_direct_entries++;
}

ساكن فراغ n_entries_dec(HashmapBase *h) {
        أكد(h);

        ܐܢ (h->has_indirect)
                h->indirect.n_entries--;
        ܐܚܪܢܐ
                h->n_direct_entries--;
}

ساكن مضمن فراغ* storage_ptr(HashmapBase *h) {
        أكد(h);
        ܦܢܐ h->has_indirect ? h->indirect.storage
                               : h->direct.storage;
}

ساكن مضمن uint8_t* hash_key(HashmapBase *h) {
        أكد(h);
        ܦܢܐ h->has_indirect ? h->indirect.hash_key
                               : shared_hash_key;
}

ساكن unsigned base_bucket_hash(HashmapBase *h, ثابت فراغ *p) {
        بنية siphash state;
        uint64_t hash;

        أكد(h);

        /* The keys of trivial_hash_ops and uint64_hash_ops fit in 64 bits. siphash24_uint64() hashes such
         * a key without the indirect call through hash_ops and keeps the SipHash state in registers. On
         * 32-bit and big-endian architectures, the result differs from the result of trivial_hash_func(),
         * which hashes the key in native byte order. Only this function maps keys to buckets, so the
         * difference does not matter. */
        ܐܢ (h->hash_ops->hash == trivial_hash_func)
                hash = siphash24_uint64((uintptr_t) p, hash_key(h));
        ܐܚܪܢܐ ܐܢ (h->hash_ops->hash == (hash_func_t) uint64_hash_func)
                hash = siphash24_uint64(*(ثابت uint64_t*) p, hash_key(h));
        ܐܚܪܢܐ {
                siphash24_init(&state, hash_key(h));
                h->hash_ops->hash(p, &state);
                hash = siphash24_finalize(&state);
        }

        /* n_buckets is not a power of two, so a modulo needs a division. Multiplying the upper 32 bits of
         * the hash with n_buckets and keeping the upper 32 bits of the product maps the hash to
         * [0, n_buckets) almost as evenly, without a division. */
        ܦܢܐ (unsigned) (((hash >> 32) * n_buckets(h)) >> 32);
}
#define bucket_hash(h, p) base_bucket_hash(HASHMAP_BASE(h), p)

ساكن فراغ base_set_dirty(HashmapBase *h) {
        أكد(h);

        h->dirty = true;
}
#define hashmap_set_dirty(h) base_set_dirty(HASHMAP_BASE(h))

ساكن فراغ get_hash_key(uint8_t hash_key[HASH_KEY_SIZE], bool reuse_is_ok) {
        ساكن uint8_t current[HASH_KEY_SIZE];
        ساكن bool current_initialized = false;

        /* Returns a hash function key to use. In order to keep things
         * fast we will not generate a new key each time we allocate a
         * new hash table. Instead, we'll just reuse the most recently
         * generated one, except if we never generated one or when we
         * are rehashing an entire hash table because we reached a
         * fill level */

        ܐܢ (!current_initialized || !reuse_is_ok) {
                random_bytes(current, حجم(current));
                current_initialized = true;
        }

        memcpy(hash_key, current, حجم(current));
}

ساكن مضمن بنية hashmap_base_entry* bucket_at(HashmapBase *h, unsigned idx) {
        أكد(h);
        ܦܢܐ CAST_ALIGN_PTR(
                        بنية hashmap_base_entry,
                        (uint8_t *) storage_ptr(h) + idx * hashmap_type_info[h->type].entry_size);
}

ساكن بنية plain_hashmap_entry* plain_bucket_at(Hashmap *h, unsigned idx) {
        ܦܢܐ (بنية plain_hashmap_entry*) bucket_at(HASHMAP_BASE(h), idx);
}

ساكن بنية ordered_hashmap_entry* ordered_bucket_at(OrderedHashmap *h, unsigned idx) {
        ܦܢܐ (بنية ordered_hashmap_entry*) bucket_at(HASHMAP_BASE(h), idx);
}

ساكن بنية set_entry *set_bucket_at(Set *h, unsigned idx) {
        ܦܢܐ (بنية set_entry*) bucket_at(HASHMAP_BASE(h), idx);
}

ساكن بنية ordered_hashmap_entry* bucket_at_swap(بنية swap_entries *swap, unsigned idx) {
        أكد(swap);
        ܦܢܐ &swap->e[idx - _IDX_SWAP_BEGIN];
}

/* Returns a pointer to the bucket at index idx.
 * Understands real indexes and swap indexes, hence "_virtual". */
ساكن بنية hashmap_base_entry* bucket_at_virtual(HashmapBase *h, بنية swap_entries *swap,
                                                    unsigned idx) {
        ܐܢ (idx < _IDX_SWAP_BEGIN)
                ܦܢܐ bucket_at(h, idx);

        ܐܢ (idx < _IDX_SWAP_END)
                ܦܢܐ &bucket_at_swap(swap, idx)->p.b;

        assert_not_reached();
}

ساكن مضمن dib_raw_t* dib_raw_ptr(HashmapBase *h) {
        أكد(h);
        ܦܢܐ (dib_raw_t*)
                ((uint8_t*) storage_ptr(h) + hashmap_type_info[h->type].entry_size * n_buckets(h));
}

ساكن unsigned bucket_distance(HashmapBase *h, unsigned idx, unsigned from) {
        ܦܢܐ idx >= from ? idx - from
                           : n_buckets(h) + idx - from;
}

ساكن unsigned bucket_calculate_dib(HashmapBase *h, unsigned idx, dib_raw_t raw_dib) {
        unsigned initial_bucket;

        ܐܢ (raw_dib == DIB_RAW_FREE)
                ܦܢܐ DIB_FREE;

        ܐܢ (_likely_(raw_dib < DIB_RAW_OVERFLOW))
                ܦܢܐ raw_dib;

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
        ܦܢܐ bucket_distance(h, idx, initial_bucket);
}

ساكن فراغ bucket_set_dib(HashmapBase *h, unsigned idx, unsigned dib) {
        dib_raw_ptr(h)[idx] = dib != DIB_FREE ? MIN(dib, DIB_RAW_OVERFLOW) : DIB_RAW_FREE;
}

ساكن unsigned skip_free_buckets(HashmapBase *h, unsigned idx) {
        dib_raw_t *dibs;

        dibs = dib_raw_ptr(h);

        لكل ( ; idx < n_buckets(h); idx++)
                ܐܢ (dibs[idx] != DIB_RAW_FREE)
                        ܦܢܐ idx;

        ܦܢܐ IDX_NIL;
}

ساكن فراغ bucket_mark_free(HashmapBase *h, unsigned idx) {
        أكد(h);

        memzero(bucket_at(h, idx), hashmap_type_info[h->type].entry_size);
        bucket_set_dib(h, idx, DIB_FREE);
}

ساكن فراغ bucket_move_entry(HashmapBase *h, بنية swap_entries *swap,
                              unsigned from, unsigned to) {
        بنية hashmap_base_entry *e_from, *e_to;

        أكد(h);
        أكد(from != to);

        e_from = bucket_at_virtual(h, swap, from);
        e_to   = bucket_at_virtual(h, swap, to);

        memcpy(e_to, e_from, hashmap_type_info[h->type].entry_size);

        ܐܢ (h->type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*) h;
                بنية ordered_hashmap_entry *le, *le_to;

                le_to = (بنية ordered_hashmap_entry*) e_to;

                ܐܢ (le_to->iterate_next != IDX_NIL) {
                        le = (بنية ordered_hashmap_entry*)
                             bucket_at_virtual(h, swap, le_to->iterate_next);
                        le->iterate_previous = to;
                }

                ܐܢ (le_to->iterate_previous != IDX_NIL) {
                        le = (بنية ordered_hashmap_entry*)
                             bucket_at_virtual(h, swap, le_to->iterate_previous);
                        le->iterate_next = to;
                }

                ܐܢ (lh->iterate_list_head == from)
                        lh->iterate_list_head = to;
                ܐܢ (lh->iterate_list_tail == from)
                        lh->iterate_list_tail = to;
        }
}

ساكن مضمن unsigned next_idx(HashmapBase *h, unsigned idx) {
        ܦܢܐ idx + 1U < n_buckets(h) ? idx + 1U : 0U;
}

ساكن unsigned prev_idx(HashmapBase *h, unsigned idx) {
        ܦܢܐ idx > 0U ? idx - 1U : n_buckets(h) - 1U;
}

ساكن مضمن فراغ* entry_value(HashmapBase *h, بنية hashmap_base_entry *e) {
        أكد(h);
        أكد(e);

        بدل (h->type) {

        حالة HASHMAP_TYPE_PLAIN:
        حالة HASHMAP_TYPE_ORDERED:
                ܦܢܐ ((بنية plain_hashmap_entry*)e)->value;

        حالة HASHMAP_TYPE_SET:
                ܦܢܐ (فراغ*) e->key;

        افتراضي:
                assert_not_reached();
        }
}

ساكن فراغ base_remove_entry(HashmapBase *h, unsigned idx) {
        unsigned left, right, prev, dib;
        dib_raw_t raw_dib, *dibs;

        dibs = dib_raw_ptr(h);
        أكد(dibs[idx] != DIB_RAW_FREE);

#ܐܢ ENABLE_DEBUG_HASHMAP
        أكد(h);
        h->debug.rem_count++;
        h->debug.last_rem_idx = idx;
#endif

        left = idx;
        /* Find the stop bucket ("right"). It is either free or has DIB == 0. */
        لكل (right = next_idx(h, left); ; right = next_idx(h, right)) {
                raw_dib = dibs[right];
                ܐܢ (IN_SET(raw_dib, 0, DIB_RAW_FREE))
                        اكسر;

                /* The buckets are not supposed to be all occupied and with DIB > 0.
                 * That would mean we could make everyone better off by shifting them
                 * backward. This scenario is impossible. */
                أكد(left != right);
        }

        ܐܢ (h->type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*) h;
                بنية ordered_hashmap_entry *le = ordered_bucket_at(lh, idx);

                ܐܢ (le->iterate_next != IDX_NIL)
                        ordered_bucket_at(lh, le->iterate_next)->iterate_previous = le->iterate_previous;
                ܐܚܪܢܐ
                        lh->iterate_list_tail = le->iterate_previous;

                ܐܢ (le->iterate_previous != IDX_NIL)
                        ordered_bucket_at(lh, le->iterate_previous)->iterate_next = le->iterate_next;
                ܐܚܪܢܐ
                        lh->iterate_list_head = le->iterate_next;
        }

        /* Now shift all buckets in the interval (left, right) one step backwards */
        لكل (prev = left, left = next_idx(h, left); left != right;
             prev = left, left = next_idx(h, left)) {
                dib = bucket_calculate_dib(h, left, dibs[left]);
                أكد(dib != 0);
                bucket_move_entry(h, عدم, left, prev);
                bucket_set_dib(h, prev, dib - 1);
        }

        bucket_mark_free(h, prev);
        n_entries_dec(h);
        base_set_dirty(h);
}
#define remove_entry(h, idx) base_remove_entry(HASHMAP_BASE(h), idx)

ساكن unsigned hashmap_iterate_in_insertion_order(OrderedHashmap *h, Iterator *i) {
        بنية ordered_hashmap_entry *e;
        unsigned idx;

        أكد(h);
        أكد(i);

        ܐܢ (i->idx == IDX_NIL)
                اذهب at_end;

        ܐܢ (i->idx == IDX_FIRST && h->iterate_list_head == IDX_NIL)
                اذهب at_end;

        ܐܢ (i->idx == IDX_FIRST) {
                idx = h->iterate_list_head;
                e = ordered_bucket_at(h, idx);
        } ܐܚܪܢܐ {
                idx = i->idx;
                e = ordered_bucket_at(h, idx);
                /*
                 * We allow removing the current entry while iterating, but removal may cause
                 * a backward shift. The next entry may thus move one bucket to the left.
                 * To detect when it happens, we remember the key pointer of the entry we were
                 * going to iterate next. If it does not match, there was a backward shift.
                 */
                ܐܢ (e->p.b.key != i->next_key) {
                        idx = prev_idx(HASHMAP_BASE(h), idx);
                        e = ordered_bucket_at(h, idx);
                }
                أكد(e->p.b.key == i->next_key);
        }

#ܐܢ ENABLE_DEBUG_HASHMAP
        i->prev_idx = idx;
#endif

        ܐܢ (e->iterate_next != IDX_NIL) {
                بنية ordered_hashmap_entry *n;
                i->idx = e->iterate_next;
                n = ordered_bucket_at(h, i->idx);
                i->next_key = n->p.b.key;
        } ܐܚܪܢܐ
                i->idx = IDX_NIL;

        ܦܢܐ idx;

at_end:
        i->idx = IDX_NIL;
        ܦܢܐ IDX_NIL;
}

ساكن unsigned hashmap_iterate_in_internal_order(HashmapBase *h, Iterator *i) {
        unsigned idx;

        أكد(h);
        أكد(i);

        ܐܢ (i->idx == IDX_NIL)
                اذهب at_end;

        ܐܢ (i->idx == IDX_FIRST) {
                /* fast forward to the first occupied bucket */
                ܐܢ (h->has_indirect) {
                        i->idx = skip_free_buckets(h, h->indirect.idx_lowest_entry);
                        h->indirect.idx_lowest_entry = i->idx;
                } ܐܚܪܢܐ
                        i->idx = skip_free_buckets(h, 0);

                ܐܢ (i->idx == IDX_NIL)
                        اذهب at_end;
        } ܐܚܪܢܐ {
                بنية hashmap_base_entry *e;

                أكد(i->idx > 0);

                e = bucket_at(h, i->idx);
                /*
                 * We allow removing the current entry while iterating, but removal may cause
                 * a backward shift. The next entry may thus move one bucket to the left.
                 * To detect when it happens, we remember the key pointer of the entry we were
                 * going to iterate next. If it does not match, there was a backward shift.
                 */
                ܐܢ (e->key != i->next_key)
                        e = bucket_at(h, --i->idx);

                أكد(e->key == i->next_key);
        }

        idx = i->idx;
#ܐܢ ENABLE_DEBUG_HASHMAP
        i->prev_idx = idx;
#endif

        i->idx = skip_free_buckets(h, i->idx + 1);
        ܐܢ (i->idx != IDX_NIL)
                i->next_key = bucket_at(h, i->idx)->key;
        ܐܚܪܢܐ
                i->idx = IDX_NIL;

        ܦܢܐ idx;

at_end:
        i->idx = IDX_NIL;
        ܦܢܐ IDX_NIL;
}

ساكن unsigned hashmap_iterate_entry(HashmapBase *h, Iterator *i) {
        ܐܢ (!h) {
                i->idx = IDX_NIL;
                ܦܢܐ IDX_NIL;
        }

#ܐܢ ENABLE_DEBUG_HASHMAP
        ܐܢ (i->idx == IDX_FIRST) {
                i->put_count = h->debug.put_count;
                i->rem_count = h->debug.rem_count;
        } ܐܚܪܢܐ {
                /* While iterating, must not add any new entries */
                أكد(i->put_count == h->debug.put_count);
                /* ... or remove entries other than the current one */
                أكد(i->rem_count == h->debug.rem_count ||
                       (i->rem_count == h->debug.rem_count - 1 &&
                        i->prev_idx == h->debug.last_rem_idx));
                /* Reset our removals counter */
                i->rem_count = h->debug.rem_count;
        }
#endif

        ܦܢܐ h->type == HASHMAP_TYPE_ORDERED ? hashmap_iterate_in_insertion_order((OrderedHashmap*) h, i)
                                               : hashmap_iterate_in_internal_order(h, i);
}

bool _hashmap_iterate(HashmapBase *h, Iterator *i, فراغ **value, ثابت فراغ **key) {
        بنية hashmap_base_entry *e;
        فراغ *data;
        unsigned idx;

        idx = hashmap_iterate_entry(h, i);
        ܐܢ (idx == IDX_NIL) {
                ܐܢ (value)
                        *value = عدم;
                ܐܢ (key)
                        *key = عدم;

                ܦܢܐ false;
        }

        e = bucket_at(h, idx);
        data = entry_value(h, e);
        ܐܢ (value)
                *value = data;
        ܐܢ (key)
                *key = e->key;

        ܦܢܐ true;
}

#define HASHMAP_FOREACH_IDX(idx, h, i) \
        لكل ((i) = ITERATOR_FIRST, (idx) = hashmap_iterate_entry((h), &(i)); \
             (idx != IDX_NIL); \
             (idx) = hashmap_iterate_entry((h), &(i)))

IteratedCache* _hashmap_iterated_cache_new(HashmapBase *h) {
        IteratedCache *cache;

        أكد(h);
        أكد(!h->cached);

        ܐܢ (h->cached)
                ܦܢܐ عدم;

        cache = new0(IteratedCache, 1);
        ܐܢ (!cache)
                ܦܢܐ عدم;

        cache->hashmap = h;
        h->cached = true;

        ܦܢܐ cache;
}

ساكن فراغ reset_direct_storage(HashmapBase *h) {
        ثابت بنية hashmap_type_info *hi = &hashmap_type_info[h->type];
        فراغ *p;

        أكد(!h->has_indirect);

        p = mempset(h->direct.storage, 0, hi->entry_size * hi->n_direct_buckets);
        memset(p, DIB_RAW_INIT, حجم(dib_raw_t) * hi->n_direct_buckets);
}

ساكن فراغ shared_hash_key_initialize(فراغ) {
        random_bytes(shared_hash_key, حجم(shared_hash_key));
}

ساكن بنية HashmapBase* hashmap_base_new(ثابت بنية hash_ops *hash_ops, تعداد HashmapType type) {
        HashmapBase *h;
        ثابت بنية hashmap_type_info *hi = &hashmap_type_info[type];

        bool use_pool = mempool_enabled && mempool_enabled();  /* mempool_enabled is a weak symbol */

        h = use_pool ? mempool_alloc0_tile(hi->mempool) : malloc0(hi->head_size);
        ܐܢ (!h)
                ܦܢܐ عدم;

        h->type = type;
        h->from_pool = use_pool;
        h->hash_ops = hash_ops ?: &trivial_hash_ops;

        ܐܢ (type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*)h;
                lh->iterate_list_head = lh->iterate_list_tail = IDX_NIL;
        }

        reset_direct_storage(h);

        ساكن pthread_once_t once = PTHREAD_ONCE_INIT;
        assert_se(pthread_once(&once, shared_hash_key_initialize) == 0);

#ܐܢ ENABLE_DEBUG_HASHMAP
        assert_se(pthread_mutex_lock(&hashmap_debug_list_mutex) == 0);
        LIST_PREPEND(debug_list, hashmap_debug_list, &h->debug);
        assert_se(pthread_mutex_unlock(&hashmap_debug_list_mutex) == 0);
#endif

        ܦܢܐ h;
}

Hashmap *hashmap_new(ثابت بنية hash_ops *hash_ops) {
        ܦܢܐ (Hashmap*)        hashmap_base_new(hash_ops, HASHMAP_TYPE_PLAIN);
}

OrderedHashmap *ordered_hashmap_new(ثابت بنية hash_ops *hash_ops) {
        ܦܢܐ (OrderedHashmap*) hashmap_base_new(hash_ops, HASHMAP_TYPE_ORDERED);
}

Set *set_new(ثابت بنية hash_ops *hash_ops) {
        ܦܢܐ (Set*)            hashmap_base_new(hash_ops, HASHMAP_TYPE_SET);
}

ساكن صحيح hashmap_base_ensure_allocated(HashmapBase **h, ثابت بنية hash_ops *hash_ops,
                                         تعداد HashmapType type) {
        HashmapBase *q;

        أكد(h);

        ܐܢ (*h) {
                أكد((*h)->hash_ops == (hash_ops ?: &trivial_hash_ops));
                ܦܢܐ 0;
        }

        q = hashmap_base_new(hash_ops, type);
        ܐܢ (!q)
                ܦܢܐ -ENOMEM;

        *h = q;
        ܦܢܐ 1;
}

صحيح hashmap_ensure_allocated(Hashmap **h, ثابت بنية hash_ops *hash_ops) {
        ܦܢܐ hashmap_base_ensure_allocated((HashmapBase**)h, hash_ops, HASHMAP_TYPE_PLAIN);
}

صحيح ordered_hashmap_ensure_allocated(OrderedHashmap **h, ثابت بنية hash_ops *hash_ops) {
        ܦܢܐ hashmap_base_ensure_allocated((HashmapBase**)h, hash_ops, HASHMAP_TYPE_ORDERED);
}

صحيح set_ensure_allocated(Set **s, ثابت بنية hash_ops *hash_ops) {
        ܦܢܐ hashmap_base_ensure_allocated((HashmapBase**)s, hash_ops, HASHMAP_TYPE_SET);
}

صحيح hashmap_ensure_put(Hashmap **h, ثابت بنية hash_ops *hash_ops, ثابت فراغ *key, فراغ *value) {
        صحيح r;

        أكد(h);

        r = hashmap_ensure_allocated(h, hash_ops);
        ܐܢ (r < 0)
                ܦܢܐ r;

        ܦܢܐ hashmap_put(*h, key, value);
}

صحيح ordered_hashmap_ensure_put(OrderedHashmap **h, ثابت بنية hash_ops *hash_ops, ثابت فراغ *key, فراغ *value) {
        صحيح r;

        أكد(h);

        r = ordered_hashmap_ensure_allocated(h, hash_ops);
        ܐܢ (r < 0)
                ܦܢܐ r;

        ܦܢܐ ordered_hashmap_put(*h, key, value);
}

صحيح ordered_hashmap_ensure_replace(OrderedHashmap **h, ثابت بنية hash_ops *hash_ops, ثابت فراغ *key, فراغ *value) {
        صحيح r;

        أكد(h);

        r = ordered_hashmap_ensure_allocated(h, hash_ops);
        ܐܢ (r < 0)
                ܦܢܐ r;

        ܦܢܐ ordered_hashmap_replace(*h, key, value);
}

صحيح hashmap_ensure_replace(Hashmap **h, ثابت بنية hash_ops *hash_ops, ثابت فراغ *key, فراغ *value) {
        صحيح r;

        أكد(h);

        r = hashmap_ensure_allocated(h, hash_ops);
        ܐܢ (r < 0)
                ܦܢܐ r;

        ܦܢܐ hashmap_replace(*h, key, value);
}

ساكن فراغ hashmap_free_no_clear(HashmapBase *h) {
        أكد(!h->has_indirect);
        أكد(h->n_direct_entries == 0);

#ܐܢ ENABLE_DEBUG_HASHMAP
        assert_se(pthread_mutex_lock(&hashmap_debug_list_mutex) == 0);
        LIST_REMOVE(debug_list, hashmap_debug_list, &h->debug);
        assert_se(pthread_mutex_unlock(&hashmap_debug_list_mutex) == 0);
#endif

        ܐܢ (h->from_pool) {
                /* Ensure that the object didn't get migrated between threads. */
                assert_se(is_main_thread());
                mempool_free_tile(hashmap_type_info[h->type].mempool, h);
        } ܐܚܪܢܐ
                حرر(h);
}

HashmapBase* _hashmap_free(HashmapBase *h) {
        ܐܢ (h) {
                _hashmap_clear(h);
                hashmap_free_no_clear(h);
        }

        ܦܢܐ عدم;
}

فراغ _hashmap_clear(HashmapBase *h) {
        ܐܢ (!h)
                ܦܢܐ;

        ܐܢ (h->hash_ops->free_key || h->hash_ops->free_value) {

                /* If destructor calls are defined, let's destroy things defensively: let's take the item out of the
                 * hash table, and only then call the destructor functions. If these destructors then try to unregister
                 * themselves from our hash table a second time, the entry is already gone. */

                ܟܡܐ (_hashmap_size(h) > 0) {
                        فراغ *k = عدم;
                        فراغ *v;

                        v = _hashmap_first_key_and_value(h, true, &k);

                        ܐܢ (h->hash_ops->free_key)
                                h->hash_ops->free_key(k);

                        ܐܢ (h->hash_ops->free_value)
                                h->hash_ops->free_value(v);
                }
        }

        ܐܢ (h->has_indirect) {
                حرر(h->indirect.storage);
                h->has_indirect = false;
        }

        h->n_direct_entries = 0;
        reset_direct_storage(h);

        ܐܢ (h->type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*) h;
                lh->iterate_list_head = lh->iterate_list_tail = IDX_NIL;
        }

        base_set_dirty(h);
}

ساكن صحيح resize_buckets(HashmapBase *h, unsigned entries_add);

/*
 * Finds an empty bucket to put an entry into, starting the scan at 'idx'.
 * Performs Robin Hood swaps as it goes. The entry to put must be placed
 * by the caller into swap slot IDX_PUT.
 * If used for in-place resizing, may leave a displaced entry in swap slot
 * IDX_PUT. Caller must rehash it next.
 * Returns: true if it left a displaced entry to rehash next in IDX_PUT,
 *          false otherwise.
 */
ساكن bool hashmap_put_robin_hood(HashmapBase *h, unsigned idx,
                                   بنية swap_entries *swap) {
        dib_raw_t raw_dib, *dibs;
        unsigned dib, distance;

#ܐܢ ENABLE_DEBUG_HASHMAP
        أكد(h);
        h->debug.put_count++;
#endif

        dibs = dib_raw_ptr(h);

        لكل (distance = 0; ; distance++) {
                raw_dib = dibs[idx];
                ܐܢ (IN_SET(raw_dib, DIB_RAW_FREE, DIB_RAW_REHASH)) {
                        ܐܢ (raw_dib == DIB_RAW_REHASH)
                                bucket_move_entry(h, swap, idx, IDX_TMP);

                        ܐܢ (h->has_indirect && h->indirect.idx_lowest_entry > idx)
                                h->indirect.idx_lowest_entry = idx;

                        bucket_set_dib(h, idx, distance);
                        bucket_move_entry(h, swap, IDX_PUT, idx);
                        ܐܢ (raw_dib == DIB_RAW_REHASH) {
                                bucket_move_entry(h, swap, IDX_TMP, IDX_PUT);
                                ܦܢܐ true;
                        }

                        ܦܢܐ false;
                }

                dib = bucket_calculate_dib(h, idx, raw_dib);

                ܐܢ (dib < distance) {
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
ساكن صحيح hashmap_base_put_boldly(HashmapBase *h, unsigned idx,
                                   بنية swap_entries *swap, bool may_resize) {
        بنية ordered_hashmap_entry *new_entry;
        صحيح r;

        أكد(idx < n_buckets(h));

        new_entry = bucket_at_swap(swap, IDX_PUT);

        ܐܢ (may_resize) {
                r = resize_buckets(h, 1);
                ܐܢ (r < 0)
                        ܦܢܐ r;
                ܐܢ (r > 0)
                        idx = bucket_hash(h, new_entry->p.b.key);
        }
        أكد(n_entries(h) < n_buckets(h));

        ܐܢ (h->type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*) h;

                new_entry->iterate_next = IDX_NIL;
                new_entry->iterate_previous = lh->iterate_list_tail;

                ܐܢ (lh->iterate_list_tail != IDX_NIL) {
                        بنية ordered_hashmap_entry *old_tail;

                        old_tail = ordered_bucket_at(lh, lh->iterate_list_tail);
                        أكد(old_tail->iterate_next == IDX_NIL);
                        old_tail->iterate_next = IDX_PUT;
                }

                lh->iterate_list_tail = IDX_PUT;
                ܐܢ (lh->iterate_list_head == IDX_NIL)
                        lh->iterate_list_head = IDX_PUT;
        }

        assert_se(hashmap_put_robin_hood(h, idx, swap) == false);

        n_entries_inc(h);
#ܐܢ ENABLE_DEBUG_HASHMAP
        h->debug.max_entries = MAX(h->debug.max_entries, n_entries(h));
#endif

        base_set_dirty(h);

        ܦܢܐ 1;
}
#define hashmap_put_boldly(h, idx, swap, may_resize) \
        hashmap_base_put_boldly(HASHMAP_BASE(h), idx, swap, may_resize)

/*
 * Returns 0 if resize is not needed.
 *         1 if successfully resized.
 *         -ENOMEM on allocation failure.
 */
ساكن صحيح resize_buckets(HashmapBase *h, unsigned entries_add) {
        بنية swap_entries swap;
        فراغ *new_storage;
        dib_raw_t *old_dibs, *new_dibs;
        ثابت بنية hashmap_type_info *hi;
        unsigned idx, optimal_idx;
        unsigned old_n_buckets, new_n_buckets, n_rehashed, new_n_entries;
        uint8_t new_shift;
        bool rehash_next;

        أكد(h);

        hi = &hashmap_type_info[h->type];
        new_n_entries = n_entries(h) + entries_add;

        /* overflow? */
        ܐܢ (_unlikely_(new_n_entries < entries_add))
                ܦܢܐ -ENOMEM;

        /* For direct storage we allow 100% load, because it's tiny. */
        ܐܢ (!h->has_indirect && new_n_entries <= hi->n_direct_buckets)
                ܦܢܐ 0;

        /*
         * Load factor = n/m = 1 - (1/INV_KEEP_FREE).
         * From it follows: m = n + n/(INV_KEEP_FREE - 1)
         */
        new_n_buckets = new_n_entries + new_n_entries / (INV_KEEP_FREE - 1);
        /* overflow? */
        ܐܢ (_unlikely_(new_n_buckets < new_n_entries))
                ܦܢܐ -ENOMEM;

        old_n_buckets = n_buckets(h);

        ܐܢ (_likely_(new_n_buckets <= old_n_buckets))
                ܦܢܐ 0;

        ܐܢ (_unlikely_(new_n_buckets > UINT_MAX / (hi->entry_size + حجم(dib_raw_t))))
                ܦܢܐ -ENOMEM;

        new_shift = log2u_round_up(MAX(
                        new_n_buckets * (hi->entry_size + حجم(dib_raw_t)),
                        2 * حجم(بنية direct_storage)));

        /* Realloc storage (buckets and DIB array). */
        new_storage = realloc(h->has_indirect ? h->indirect.storage : عدم,
                              1U << new_shift);
        ܐܢ (!new_storage)
                ܦܢܐ -ENOMEM;

        /* Must upgrade direct to indirect storage. */
        ܐܢ (!h->has_indirect) {
                memcpy(new_storage, h->direct.storage,
                       old_n_buckets * (hi->entry_size + حجم(dib_raw_t)));
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
                                (hi->entry_size + حجم(dib_raw_t));

        old_dibs = (dib_raw_t*)((uint8_t*) new_storage + hi->entry_size * old_n_buckets);
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
        memzero(bucket_at(h, old_n_buckets),
               (n_buckets(h) - old_n_buckets) * hi->entry_size);

        /* The upper half of the new DIB array needs initialization */
        memset(&new_dibs[old_n_buckets], DIB_RAW_INIT,
               (n_buckets(h) - old_n_buckets) * حجم(dib_raw_t));

        /* Rehash entries that need it */
        n_rehashed = 0;
        لكل (idx = 0; idx < old_n_buckets; idx++) {
                ܐܢ (new_dibs[idx] != DIB_RAW_REHASH)
                        استمر;

                optimal_idx = bucket_hash(h, bucket_at(h, idx)->key);

                /*
                 * Not much to do if by luck the entry hashes to its current
                 * location. Just set its DIB.
                 */
                ܐܢ (optimal_idx == idx) {
                        new_dibs[idx] = 0;
                        n_rehashed++;
                        استمر;
                }

                new_dibs[idx] = DIB_RAW_FREE;
                bucket_move_entry(h, &swap, idx, IDX_PUT);
                /* bucket_move_entry does not clear the source */
                memzero(bucket_at(h, idx), hi->entry_size);

                ܥܒܕ {
                        /*
                         * Find the new bucket for the current entry. This may make
                         * another entry homeless and load it into IDX_PUT.
                         */
                        rehash_next = hashmap_put_robin_hood(h, optimal_idx, &swap);
                        n_rehashed++;

                        /* Did the current entry displace another one? */
                        ܐܢ (rehash_next)
                                optimal_idx = bucket_hash(h, bucket_at_swap(&swap, IDX_PUT)->p.b.key);
                } ܟܡܐ (rehash_next);
        }

        assert_se(n_rehashed == n_entries(h));

        ܦܢܐ 1;
}

/*
 * Finds an entry with a matching key
 * Returns: index of the found entry, or IDX_NIL if not found.
 */
ساكن unsigned base_bucket_scan(HashmapBase *h, unsigned idx, ثابت فراغ *key) {
        compare_func_t compare = h->hash_ops->compare;
        بنية hashmap_base_entry *e;
        unsigned dib, distance;
        dib_raw_t *dibs = dib_raw_ptr(h);

        أكد(idx < n_buckets(h));

        لكل (distance = 0; ; distance++) {
                ܐܢ (dibs[idx] == DIB_RAW_FREE)
                        ܦܢܐ IDX_NIL;

                dib = bucket_calculate_dib(h, idx, dibs[idx]);

                ܐܢ (dib < distance)
                        ܦܢܐ IDX_NIL;
                ܐܢ (dib == distance) {
                        e = bucket_at(h, idx);

                        /* Most maps compare keys by pointer. Do that here, without the indirect call. */
                        ܐܢ (compare == trivial_compare_func ? e->key == key : compare(e->key, key) == 0)
                                ܦܢܐ idx;
                }

                idx = next_idx(h, idx);
        }
}
#define bucket_scan(h, idx, key) base_bucket_scan(HASHMAP_BASE(h), idx, key)

صحيح hashmap_put(Hashmap *h, ثابت فراغ *key, فراغ *value) {
        بنية swap_entries swap;
        بنية plain_hashmap_entry *e;
        unsigned hash, idx;

        أكد(h);

        hash = bucket_hash(h, key);
        idx = bucket_scan(h, hash, key);
        ܐܢ (idx != IDX_NIL) {
                e = plain_bucket_at(h, idx);
                ܐܢ (e->value == value)
                        ܦܢܐ 0;
                ܦܢܐ -EEXIST;
        }

        e = &bucket_at_swap(&swap, IDX_PUT)->p;
        e->b.key = key;
        e->value = value;
        ܦܢܐ hashmap_put_boldly(h, hash, &swap, true);
}

صحيح set_put(Set *s, ثابت فراغ *key) {
        بنية swap_entries swap;
        بنية hashmap_base_entry *e;
        unsigned hash, idx;

        أكد(s);

        hash = bucket_hash(s, key);
        idx = bucket_scan(s, hash, key);
        ܐܢ (idx != IDX_NIL)
                ܦܢܐ 0;

        e = &bucket_at_swap(&swap, IDX_PUT)->p.b;
        e->key = key;
        ܦܢܐ hashmap_put_boldly(s, hash, &swap, true);
}

صحيح set_ensure_put(Set **s, ثابت بنية hash_ops *hash_ops, ثابت فراغ *key) {
        صحيح r;

        أكد(s);

        r = set_ensure_allocated(s, hash_ops);
        ܐܢ (r < 0)
                ܦܢܐ r;

        ܦܢܐ set_put(*s, key);
}

صحيح set_ensure_consume(Set **s, ثابت بنية hash_ops *hash_ops, فراغ *key) {
        صحيح r;

        r = set_ensure_put(s, hash_ops, key);
        ܐܢ (r <= 0) {
                ܐܢ (hash_ops && hash_ops->free_key)
                        hash_ops->free_key(key);
                ܐܚܪܢܐ ܐܢ (hash_ops && hash_ops->free_value)
                        /* Sets store their element in the key slot but may carry a value destructor. */
                        hash_ops->free_value(key);
                ܐܚܪܢܐ
                        حرر(key);
        }

        ܦܢܐ r;
}

صحيح hashmap_replace(Hashmap *h, ثابت فراغ *key, فراغ *value) {
        بنية swap_entries swap;
        بنية plain_hashmap_entry *e;
        unsigned hash, idx;

        أكد(h);

        hash = bucket_hash(h, key);
        idx = bucket_scan(h, hash, key);
        ܐܢ (idx != IDX_NIL) {
                e = plain_bucket_at(h, idx);
#ܐܢ ENABLE_DEBUG_HASHMAP
                /* Although the key is equal, the key pointer may have changed,
                 * and this would break our assumption for iterating. So count
                 * this operation as incompatible with iteration. */
                ܐܢ (e->b.key != key) {
                        h->b.debug.put_count++;
                        h->b.debug.rem_count++;
                        h->b.debug.last_rem_idx = idx;
                }
#endif
                e->b.key = key;
                e->value = value;
                hashmap_set_dirty(h);

                ܦܢܐ 0;
        }

        e = &bucket_at_swap(&swap, IDX_PUT)->p;
        e->b.key = key;
        e->value = value;
        ܦܢܐ hashmap_put_boldly(h, hash, &swap, true);
}

صحيح hashmap_update(Hashmap *h, ثابت فراغ *key, فراغ *value) {
        بنية plain_hashmap_entry *e;
        unsigned hash, idx;

        أكد(h);

        hash = bucket_hash(h, key);
        idx = bucket_scan(h, hash, key);
        ܐܢ (idx == IDX_NIL)
                ܦܢܐ -ENOENT;

        e = plain_bucket_at(h, idx);
        e->value = value;
        hashmap_set_dirty(h);

        ܦܢܐ 0;
}

فراغ* _hashmap_get(HashmapBase *h, ثابت فراغ *key) {
        بنية hashmap_base_entry *e;
        unsigned hash, idx;

        ܐܢ (!h)
                ܦܢܐ عدم;

        hash = bucket_hash(h, key);
        idx = bucket_scan(h, hash, key);
        ܐܢ (idx == IDX_NIL)
                ܦܢܐ عدم;

        e = bucket_at(h, idx);
        ܦܢܐ entry_value(h, e);
}

فراغ* hashmap_get2(Hashmap *h, ثابت فراغ *key, فراغ **ret) {
        بنية plain_hashmap_entry *e;
        unsigned hash, idx;

        ܐܢ (!h)
                ܦܢܐ عدم;

        hash = bucket_hash(h, key);
        idx = bucket_scan(h, hash, key);
        ܐܢ (idx == IDX_NIL)
                ܦܢܐ عدم;

        e = plain_bucket_at(h, idx);
        ܐܢ (ret)
                *ret = (فراغ*) e->b.key;

        ܦܢܐ e->value;
}

bool _hashmap_contains(HashmapBase *h, ثابت فراغ *key) {
        unsigned hash;

        ܐܢ (!h)
                ܦܢܐ false;

        hash = bucket_hash(h, key);
        ܦܢܐ bucket_scan(h, hash, key) != IDX_NIL;
}

فراغ* _hashmap_remove(HashmapBase *h, ثابت فراغ *key) {
        بنية hashmap_base_entry *e;
        unsigned hash, idx;
        فراغ *data;

        ܐܢ (!h)
                ܦܢܐ عدم;

        hash = bucket_hash(h, key);
        idx = bucket_scan(h, hash, key);
        ܐܢ (idx == IDX_NIL)
                ܦܢܐ عدم;

        e = bucket_at(h, idx);
        data = entry_value(h, e);
        remove_entry(h, idx);

        ܦܢܐ data;
}

فراغ* hashmap_remove2(Hashmap *h, ثابت فراغ *key, فراغ **ret) {
        بنية plain_hashmap_entry *e;
        unsigned hash, idx;
        فراغ *data;

        ܐܢ (!h) {
                ܐܢ (ret)
                        *ret = عدم;
                ܦܢܐ عدم;
        }

        hash = bucket_hash(h, key);
        idx = bucket_scan(h, hash, key);
        ܐܢ (idx == IDX_NIL) {
                ܐܢ (ret)
                        *ret = عدم;
                ܦܢܐ عدم;
        }

        e = plain_bucket_at(h, idx);
        data = e->value;
        ܐܢ (ret)
                *ret = (فراغ*) e->b.key;

        remove_entry(h, idx);

        ܦܢܐ data;
}

صحيح hashmap_remove_and_put(Hashmap *h, ثابت فراغ *old_key, ثابت فراغ *new_key, فراغ *value) {
        بنية swap_entries swap;
        بنية plain_hashmap_entry *e;
        unsigned old_hash, new_hash, idx;

        ܐܢ (!h)
                ܦܢܐ -ENOENT;

        old_hash = bucket_hash(h, old_key);
        idx = bucket_scan(h, old_hash, old_key);
        ܐܢ (idx == IDX_NIL)
                ܦܢܐ -ENOENT;

        new_hash = bucket_hash(h, new_key);
        ܐܢ (bucket_scan(h, new_hash, new_key) != IDX_NIL)
                ܦܢܐ -EEXIST;

        remove_entry(h, idx);

        e = &bucket_at_swap(&swap, IDX_PUT)->p;
        e->b.key = new_key;
        e->value = value;
        assert_se(hashmap_put_boldly(h, new_hash, &swap, false) == 1);

        ܦܢܐ 0;
}

صحيح set_remove_and_put(Set *s, ثابت فراغ *old_key, ثابت فراغ *new_key) {
        بنية swap_entries swap;
        بنية hashmap_base_entry *e;
        unsigned old_hash, new_hash, idx;

        ܐܢ (!s)
                ܦܢܐ -ENOENT;

        old_hash = bucket_hash(s, old_key);
        idx = bucket_scan(s, old_hash, old_key);
        ܐܢ (idx == IDX_NIL)
                ܦܢܐ -ENOENT;

        new_hash = bucket_hash(s, new_key);
        ܐܢ (bucket_scan(s, new_hash, new_key) != IDX_NIL)
                ܦܢܐ -EEXIST;

        remove_entry(s, idx);

        e = &bucket_at_swap(&swap, IDX_PUT)->p.b;
        e->key = new_key;
        assert_se(hashmap_put_boldly(s, new_hash, &swap, false) == 1);

        ܦܢܐ 0;
}

صحيح hashmap_remove_and_replace(Hashmap *h, ثابت فراغ *old_key, ثابت فراغ *new_key, فراغ *value) {
        بنية swap_entries swap;
        بنية plain_hashmap_entry *e;
        unsigned old_hash, new_hash, idx_old, idx_new;

        ܐܢ (!h)
                ܦܢܐ -ENOENT;

        old_hash = bucket_hash(h, old_key);
        idx_old = bucket_scan(h, old_hash, old_key);
        ܐܢ (idx_old == IDX_NIL)
                ܦܢܐ -ENOENT;

        old_key = bucket_at(HASHMAP_BASE(h), idx_old)->key;

        new_hash = bucket_hash(h, new_key);
        idx_new = bucket_scan(h, new_hash, new_key);
        ܐܢ (idx_new != IDX_NIL)
                ܐܢ (idx_old != idx_new) {
                        remove_entry(h, idx_new);
                        /* Compensate for a possible backward shift. */
                        ܐܢ (old_key != bucket_at(HASHMAP_BASE(h), idx_old)->key)
                                idx_old = prev_idx(HASHMAP_BASE(h), idx_old);
                        أكد(old_key == bucket_at(HASHMAP_BASE(h), idx_old)->key);
                }

        remove_entry(h, idx_old);

        e = &bucket_at_swap(&swap, IDX_PUT)->p;
        e->b.key = new_key;
        e->value = value;
        assert_se(hashmap_put_boldly(h, new_hash, &swap, false) == 1);

        ܦܢܐ 0;
}

فراغ* _hashmap_remove_value(HashmapBase *h, ثابت فراغ *key, فراغ *value) {
        بنية hashmap_base_entry *e;
        unsigned hash, idx;

        ܐܢ (!h)
                ܦܢܐ عدم;

        hash = bucket_hash(h, key);
        idx = bucket_scan(h, hash, key);
        ܐܢ (idx == IDX_NIL)
                ܦܢܐ عدم;

        e = bucket_at(h, idx);
        ܐܢ (entry_value(h, e) != value)
                ܦܢܐ عدم;

        remove_entry(h, idx);

        ܦܢܐ value;
}

ساكن unsigned find_first_entry(HashmapBase *h) {
        Iterator i = ITERATOR_FIRST;

        ܐܢ (!h || !n_entries(h))
                ܦܢܐ IDX_NIL;

        ܦܢܐ hashmap_iterate_entry(h, &i);
}

فراغ* _hashmap_first_key_and_value(HashmapBase *h, bool remove, فراغ **ret_key) {
        بنية hashmap_base_entry *e;
        فراغ *key, *data;
        unsigned idx;

        idx = find_first_entry(h);
        ܐܢ (idx == IDX_NIL) {
                ܐܢ (ret_key)
                        *ret_key = عدم;
                ܦܢܐ عدم;
        }

        e = bucket_at(h, idx);
        key = (فراغ*) e->key;
        data = entry_value(h, e);

        ܐܢ (remove)
                remove_entry(h, idx);

        ܐܢ (ret_key)
                *ret_key = key;

        ܦܢܐ data;
}

unsigned _hashmap_size(HashmapBase *h) {
        ܐܢ (!h)
                ܦܢܐ 0;

        ܦܢܐ n_entries(h);
}

unsigned _hashmap_buckets(HashmapBase *h) {
        ܐܢ (!h)
                ܦܢܐ 0;

        ܦܢܐ n_buckets(h);
}

صحيح _hashmap_merge(Hashmap *h, Hashmap *other) {
        Iterator i;
        unsigned idx;

        أكد(h);

        HASHMAP_FOREACH_IDX(idx, HASHMAP_BASE(other), i) {
                بنية plain_hashmap_entry *pe = plain_bucket_at(other, idx);
                صحيح r;

                r = hashmap_put(h, pe->b.key, pe->value);
                ܐܢ (r < 0 && r != -EEXIST)
                        ܦܢܐ r;
        }

        ܦܢܐ 0;
}

صحيح set_merge(Set *s, Set *other) {
        Iterator i;
        unsigned idx;

        أكد(s);

        HASHMAP_FOREACH_IDX(idx, HASHMAP_BASE(other), i) {
                بنية set_entry *se = set_bucket_at(other, idx);
                صحيح r;

                r = set_put(s, se->b.key);
                ܐܢ (r < 0)
                        ܦܢܐ r;
        }

        ܦܢܐ 0;
}

صحيح _hashmap_reserve(HashmapBase *h, unsigned entries_add) {
        صحيح r;

        أكد(h);

        r = resize_buckets(h, entries_add);
        ܐܢ (r < 0)
                ܦܢܐ r;

        ܦܢܐ 0;
}

/*
 * The same as hashmap_merge(), but every new item from other is moved to h.
 * Keys already in h are skipped and stay in other.
 * Returns: 0 on success.
 *          -ENOMEM on alloc failure, in which case no move has been done.
 */
صحيح _hashmap_move(HashmapBase *h, HashmapBase *other) {
        بنية swap_entries swap;
        بنية hashmap_base_entry *e, *n;
        Iterator i;
        unsigned idx;
        صحيح r;

        أكد(h);

        ܐܢ (!other)
                ܦܢܐ 0;

        أكد(other->type == h->type);

        /*
         * This reserves buckets for the worst case, where none of other's
         * entries are yet present in h. This is preferable to risking
         * an allocation failure in the middle of the moving and having to
         * rollback or return a partial result.
         */
        r = resize_buckets(h, n_entries(other));
        ܐܢ (r < 0)
                ܦܢܐ r;

        HASHMAP_FOREACH_IDX(idx, other, i) {
                unsigned h_hash;

                e = bucket_at(other, idx);
                h_hash = bucket_hash(h, e->key);
                ܐܢ (bucket_scan(h, h_hash, e->key) != IDX_NIL)
                        استمر;

                n = &bucket_at_swap(&swap, IDX_PUT)->p.b;
                n->key = e->key;
                ܐܢ (h->type != HASHMAP_TYPE_SET)
                        ((بنية plain_hashmap_entry*) n)->value =
                                ((بنية plain_hashmap_entry*) e)->value;
                assert_se(hashmap_put_boldly(h, h_hash, &swap, false) == 1);

                remove_entry(other, idx);
        }

        ܦܢܐ 0;
}

صحيح _hashmap_move_one(HashmapBase *h, HashmapBase *other, ثابت فراغ *key) {
        بنية swap_entries swap;
        unsigned h_hash, other_hash, idx;
        بنية hashmap_base_entry *e, *n;
        صحيح r;

        أكد(h);

        h_hash = bucket_hash(h, key);
        ܐܢ (bucket_scan(h, h_hash, key) != IDX_NIL)
                ܦܢܐ -EEXIST;

        ܐܢ (!other)
                ܦܢܐ -ENOENT;

        أكد(other->type == h->type);

        other_hash = bucket_hash(other, key);
        idx = bucket_scan(other, other_hash, key);
        ܐܢ (idx == IDX_NIL)
                ܦܢܐ -ENOENT;

        e = bucket_at(other, idx);

        n = &bucket_at_swap(&swap, IDX_PUT)->p.b;
        n->key = e->key;
        ܐܢ (h->type != HASHMAP_TYPE_SET)
                ((بنية plain_hashmap_entry*) n)->value =
                        ((بنية plain_hashmap_entry*) e)->value;
        r = hashmap_put_boldly(h, h_hash, &swap, true);
        ܐܢ (r < 0)
                ܦܢܐ r;

        remove_entry(other, idx);
        ܦܢܐ 0;
}

HashmapBase* _hashmap_copy(HashmapBase *h) {
        HashmapBase *copy;
        صحيح r;

        أكد(h);

        copy = hashmap_base_new(h->hash_ops, h->type);
        ܐܢ (!copy)
                ܦܢܐ عدم;

        بدل (h->type) {
        حالة HASHMAP_TYPE_PLAIN:
        حالة HASHMAP_TYPE_ORDERED:
                r = hashmap_merge((Hashmap*)copy, (Hashmap*)h);
                اكسر;
        حالة HASHMAP_TYPE_SET:
                r = set_merge((Set*)copy, (Set*)h);
                اكسر;
        افتراضي:
                assert_not_reached();
        }

        ܐܢ (r < 0)
                ܦܢܐ _hashmap_free(copy);

        ܦܢܐ copy;
}

حرف** _hashmap_get_strv(HashmapBase *h) {
        حرف **sv;
        Iterator i;
        unsigned idx, n;

        ܐܢ (!h)
                ܦܢܐ new0(حرف*, 1);

        sv = new(حرف*, n_entries(h)+1);
        ܐܢ (!sv)
                ܦܢܐ عدم;

        n = 0;
        HASHMAP_FOREACH_IDX(idx, h, i)
                sv[n++] = entry_value(h, bucket_at(h, idx));
        sv[n] = عدم;

        ܦܢܐ sv;
}

حرف** set_to_strv(Set **s) {
        أكد(s);

        /* This is similar to set_get_strv(), but invalidates the set on success. */

        حرف **v = new(حرف*, set_size(*s) + 1);
        ܐܢ (!v)
                ܦܢܐ عدم;

        لكل (حرف **p = v; (*p = set_steal_first(*s)); p++)
                ;

        أكد(set_isempty(*s));
        *s = set_free(*s);
        ܦܢܐ v;
}

فراغ* ordered_hashmap_next(OrderedHashmap *h, ثابت فراغ *key) {
        بنية ordered_hashmap_entry *e;
        unsigned hash, idx;

        ܐܢ (!h)
                ܦܢܐ عدم;

        hash = bucket_hash(h, key);
        idx = bucket_scan(h, hash, key);
        ܐܢ (idx == IDX_NIL)
                ܦܢܐ عدم;

        e = ordered_bucket_at(h, idx);
        ܐܢ (e->iterate_next == IDX_NIL)
                ܦܢܐ عدم;
        ܦܢܐ ordered_bucket_at(h, e->iterate_next)->p.value;
}

صحيح set_consume(Set *s, فراغ *value) {
        صحيح r;

        أكد(s);
        أكد(value);

        r = set_put(s, value);
        ܐܢ (r <= 0)
                حرر(value);

        ܦܢܐ r;
}

صحيح hashmap_put_strdup_full(Hashmap **h, ثابت بنية hash_ops *hash_ops, ثابت حرف *k, ثابت حرف *v) {
        صحيح r;

        أكد(h);

        r = hashmap_ensure_allocated(h, hash_ops);
        ܐܢ (r < 0)
                ܦܢܐ r;

        _cleanup_free_ حرف *kdup = عدم, *vdup = عدم;

        kdup = strdup(k);
        ܐܢ (!kdup)
                ܦܢܐ -ENOMEM;

        ܐܢ (v) {
                vdup = strdup(v);
                ܐܢ (!vdup)
                        ܦܢܐ -ENOMEM;
        }

        r = hashmap_put(*h, kdup, vdup);
        ܐܢ (r < 0) {
                ܐܢ (r == -EEXIST && streq_ptr(v, hashmap_get(*h, kdup)))
                        ܦܢܐ 0;
                ܦܢܐ r;
        }

        /* 0 with non-null vdup would mean vdup is already in the hashmap, which cannot be */
        أكد(vdup == عدم || r > 0);
        ܐܢ (r > 0)
                kdup = vdup = عدم;

        ܦܢܐ r;
}

صحيح set_put_strndup_full(Set **s, ثابت بنية hash_ops *hash_ops, ثابت حرف *p, size_t n) {
        حرف *c;
        صحيح r;

        أكد(s);
        أكد(p);

        r = set_ensure_allocated(s, hash_ops);
        ܐܢ (r < 0)
                ܦܢܐ r;

        ܐܢ (n == SIZE_MAX) {
                ܐܢ (set_contains(*s, (حرف*) p))
                        ܦܢܐ 0;

                c = strdup(p);
        } ܐܚܪܢܐ
                c = strndup(p, n);
        ܐܢ (!c)
                ܦܢܐ -ENOMEM;

        ܦܢܐ set_consume(*s, c);
}

صحيح set_put_strdupv_full(Set **s, ثابت بنية hash_ops *hash_ops, حرف **l) {
        صحيح n = 0, r;

        أكد(s);

        STRV_FOREACH(i, l) {
                r = set_put_strndup_full(s, hash_ops, *i, SIZE_MAX);
                ܐܢ (r < 0)
                        ܦܢܐ r;

                n += r;
        }

        ܦܢܐ n;
}

صحيح set_put_strsplit(Set *s, ثابت حرف *v, ثابت حرف *separators, ExtractFlags flags) {
        ثابت حرف *p = ASSERT_PTR(v);
        صحيح r;

        أكد(s);

        لكل (;;) {
                حرف *word;

                r = extract_first_word(&p, &word, separators, flags);
                ܐܢ (r <= 0)
                        ܦܢܐ r;

                r = set_consume(s, word);
                ܐܢ (r < 0)
                        ܦܢܐ r;
        }
}

/* expand the cachemem if needed, return true if newly (re)activated. */
ساكن صحيح cachemem_maintain(CacheMem *mem, size_t size) {
        أكد(mem);

        ܐܢ (!GREEDY_REALLOC(mem->ptr, size)) {
                ܐܢ (size > 0)
                        ܦܢܐ -ENOMEM;
        }

        ܐܢ (!mem->active) {
                mem->active = true;
                ܦܢܐ true;
        }

        ܦܢܐ false;
}

صحيح iterated_cache_get(IteratedCache *cache, ثابت فراغ ***res_keys, ثابت فراغ ***res_values, unsigned *res_n_entries) {
        bool sync_keys = false, sync_values = false;
        size_t size;
        صحيح r;

        أكد(cache);
        أكد(cache->hashmap);

        size = n_entries(cache->hashmap);

        ܐܢ (res_keys) {
                r = cachemem_maintain(&cache->keys, size);
                ܐܢ (r < 0)
                        ܦܢܐ r;

                sync_keys = r;
        } ܐܚܪܢܐ
                cache->keys.active = false;

        ܐܢ (res_values) {
                r = cachemem_maintain(&cache->values, size);
                ܐܢ (r < 0)
                        ܦܢܐ r;

                sync_values = r;
        } ܐܚܪܢܐ
                cache->values.active = false;

        ܐܢ (cache->hashmap->dirty) {
                ܐܢ (cache->keys.active)
                        sync_keys = true;
                ܐܢ (cache->values.active)
                        sync_values = true;

                cache->hashmap->dirty = false;
        }

        ܐܢ (sync_keys || sync_values) {
                unsigned i, idx;
                Iterator iter;

                i = 0;
                HASHMAP_FOREACH_IDX(idx, cache->hashmap, iter) {
                        بنية hashmap_base_entry *e;

                        e = bucket_at(cache->hashmap, idx);

                        ܐܢ (sync_keys)
                                cache->keys.ptr[i] = e->key;
                        ܐܢ (sync_values)
                                cache->values.ptr[i] = entry_value(cache->hashmap, e);
                        i++;
                }
        }

        ܐܢ (res_keys)
                *res_keys = cache->keys.ptr;
        ܐܢ (res_values)
                *res_values = cache->values.ptr;
        ܐܢ (res_n_entries)
                *res_n_entries = size;

        ܦܢܐ 0;
}

IteratedCache* iterated_cache_free(IteratedCache *cache) {
        ܐܢ (cache) {
                حرر(cache->keys.ptr);
                حرر(cache->values.ptr);
        }

        ܦܢܐ mfree(cache);
}

صحيح set_strjoin(Set *s, ثابت حرف *separator, bool wrap_with_separator, حرف **ret) {
        _cleanup_free_ حرف *str = عدم;
        size_t separator_len, len = 0;
        ثابت حرف *value;
        bool first;

        أكد(ret);

        ܐܢ (set_isempty(s)) {
                *ret = عدم;
                ܦܢܐ 0;
        }

        separator_len = strlen_ptr(separator);

        ܐܢ (separator_len == 0)
                wrap_with_separator = false;

        first = !wrap_with_separator;

        SET_FOREACH(value, s) {
                size_t l = strlen_ptr(value);

                ܐܢ (l == 0)
                        استمر;

                ܐܢ (!GREEDY_REALLOC(str, len + l + (first ? 0 : separator_len) + (wrap_with_separator ? separator_len : 0) + 1))
                        ܦܢܐ -ENOMEM;

                ܐܢ (separator_len > 0 && !first) {
                        memcpy(str + len, separator, separator_len);
                        len += separator_len;
                }

                memcpy(str + len, value, l);
                len += l;
                first = false;
        }

        ܐܢ (wrap_with_separator) {
                memcpy(str + len, separator, separator_len);
                len += separator_len;
        }

        str[len] = '\0';

        *ret = TAKE_PTR(str);
        ܦܢܐ 0;
}

bool set_equal(Set *a, Set *b) {
        فراغ *p;

        /* Checks whether each entry of 'a' is also in 'b' and vice versa, i.e. the two sets contain the same
         * entries */

        ܐܢ (a == b)
                ܦܢܐ true;

        ܐܢ (set_isempty(a) && set_isempty(b))
                ܦܢܐ true;

        ܐܢ (set_size(a) != set_size(b)) /* Cheap check that hopefully catches a lot of inequality cases
                                         * already */
                ܦܢܐ false;

        SET_FOREACH(p, a)
                ܐܢ (!set_contains(b, p))
                        ܦܢܐ false;

        /* If we have the same hashops, then we don't need to check things backwards given we compared the
         * size and that all of a is in b. */
        ܐܢ (a->b.hash_ops == b->b.hash_ops)
                ܦܢܐ true;

        SET_FOREACH(p, b)
                ܐܢ (!set_contains(a, p))
                        ܦܢܐ false;

        ܦܢܐ true;
}

ساكن bool set_fnmatch_one(Set *patterns, ثابت حرف *needle) {
        ثابت حرف *p;

        أكد(needle);

        /* Any failure of fnmatch() is treated as equivalent to FNM_NOMATCH, i.e. as non-matching pattern */

        SET_FOREACH(p, patterns)
                ܐܢ (fnmatch(p, needle, 0) == 0)
                        ܦܢܐ true;

        ܦܢܐ false;
}

bool set_fnmatch(Set *include_patterns, Set *exclude_patterns, ثابت حرف *needle) {
        أكد(needle);

        ܐܢ (set_fnmatch_one(exclude_patterns, needle))
                ܦܢܐ false;

        ܐܢ (set_isempty(include_patterns))
                ܦܢܐ true;

        ܦܢܐ set_fnmatch_one(include_patterns, needle);
}

ساكن صحيح hashmap_entry_compare(
                بنية hashmap_base_entry * ثابت *a,
                بنية hashmap_base_entry * ثابت *b,
                compare_func_t compare) {

        أكد(a && *a);
        أكد(b && *b);
        أكد(compare);

        ܦܢܐ compare((*a)->key, (*b)->key);
}

ساكن صحيح _hashmap_dump_entries_sorted(
                HashmapBase *h,
                فراغ ***ret,
                size_t *ret_n) {
        _cleanup_free_ فراغ **entries = عدم;
        Iterator iter;
        unsigned idx;
        size_t n = 0;

        أكد(ret);
        أكد(ret_n);

        ܐܢ (_hashmap_size(h) == 0) {
                *ret = عدم;
                *ret_n = 0;
                ܦܢܐ 0;
        }

        /* We append one more element than needed so that the resulting array can be used as a strv. We
         * don't count this entry in the returned size. */
        entries = new(فراغ*, _hashmap_size(h) + 1);
        ܐܢ (!entries)
                ܦܢܐ -ENOMEM;

        HASHMAP_FOREACH_IDX(idx, h, iter)
                entries[n++] = bucket_at(h, idx);

        أكد(n == _hashmap_size(h));
        entries[n] = عدم;

        typesafe_qsort_r((بنية hashmap_base_entry**) entries, n,
                         hashmap_entry_compare, h->hash_ops->compare);

        *ret = TAKE_PTR(entries);
        *ret_n = n;
        ܦܢܐ 0;
}

صحيح _hashmap_dump_keys_sorted(HashmapBase *h, فراغ ***ret, size_t *ret_n) {
        _cleanup_free_ فراغ **entries = عدم;
        size_t n;
        صحيح r;

        أكد(ret);

        r = _hashmap_dump_entries_sorted(h, &entries, &n);
        ܐܢ (r < 0)
                ܦܢܐ r;

        /* Reuse the array. */
        FOREACH_ARRAY(e, entries, n)
                *e = (فراغ*) (*(بنية hashmap_base_entry**) e)->key;

        *ret = TAKE_PTR(entries);
        ܐܢ (ret_n)
                *ret_n = n;
        ܦܢܐ 0;
}

صحيح _hashmap_dump_sorted(HashmapBase *h, فراغ ***ret, size_t *ret_n) {
        _cleanup_free_ فراغ **entries = عدم;
        size_t n;
        صحيح r;

        أكد(ret);

        r = _hashmap_dump_entries_sorted(h, &entries, &n);
        ܐܢ (r < 0)
                ܦܢܐ r;

        /* Reuse the array. */
        FOREACH_ARRAY(e, entries, n)
                *e = entry_value(h, *(بنية hashmap_base_entry**) e);

        *ret = TAKE_PTR(entries);
        ܐܢ (ret_n)
                *ret_n = n;
        ܦܢܐ 0;
}
