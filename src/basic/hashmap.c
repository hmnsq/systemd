/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <fnmatch.h>
#شامل <pthread.h>
#شامل <unistd.h>
#اگر HAVE_VALGRIND_VALGRIND_H
#  include <valgrind/valgrind.h>
#endif

#شامل "alloc-util.h"
#شامل "extract-word.h"
#شامل "hashmap.h"
#شامل "log.h"
#شامل "logarithm.h"
#شامل "memory-util.h"
#شامل "mempool.h"
#شامل "process-util.h"
#شامل "random-util.h"
#شامل "set.h"
#شامل "siphash24.h"
#شامل "sort-util.h"
#شامل "string-util.h"
#شامل "strv.h"

#اگر ENABLE_DEBUG_HASHMAP
#شامل "list.h"
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
ساختار hashmap_base_entry {
        ثابت تهی *کلید;
};

/* Entry types for specific hashmap/set types
 * hashmap_base_entry must be at the beginning of each entry struct. */

ساختار plain_hashmap_entry {
        ساختار hashmap_base_entry b;
        تهی *مقدار;
};

ساختار ordered_hashmap_entry {
        ساختار plain_hashmap_entry p;
        unsigned iterate_next, iterate_previous;
};

ساختار set_entry {
        ساختار hashmap_base_entry b;
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
ساختار swap_entries {
        ساختار ordered_hashmap_entry e[_IDX_SWAP_END - _IDX_SWAP_BEGIN];
};

/* Distance from Initial Bucket */
تعریفنوع uint8_t dib_raw_t;
#define DIB_RAW_OVERFLOW ((dib_raw_t)0xfdU)   /* indicates DIB value is greater than representable */
#define DIB_RAW_REHASH   ((dib_raw_t)0xfeU)   /* entry yet to be rehashed during in-place resize */
#define DIB_RAW_FREE     ((dib_raw_t)0xffU)   /* a free bucket */
#define DIB_RAW_INIT     ((نویسه)DIB_RAW_FREE) /* a byte to memset a DIB store with when initializing */

#define DIB_FREE UINT_MAX

#اگر ENABLE_DEBUG_HASHMAP
ساختار hashmap_debug_info {
        LIST_FIELDS(ساختار hashmap_debug_info, debug_list);
        unsigned max_entries;  /* high watermark of n_entries */

        /* fields to detect modification while iterating */
        unsigned put_شمار;    /* counts puts into the hashmap */
        unsigned rem_شمار;    /* counts removals from hashmap */
        unsigned last_rem_idx; /* remembers last removal index */
};

/* Tracks all existing hashmaps. Get at it from gdb. See sd_dump_hashmaps.py */
ایستا LIST_HEAD(ساختار hashmap_debug_info, hashmap_debug_list);
ایستا pthread_mutex_t hashmap_debug_list_mutex = PTHREAD_MUTEX_INITIALIZER;
#endif

شمارش HashmapType {
        HASHMAP_TYPE_PLAIN,
        HASHMAP_TYPE_ORDERED,
        HASHMAP_TYPE_SET,
        _HASHMAP_TYPE_MAX
};

ساختار _packed_ indirect_storage {
        تهی *storage;                     /* where buckets and DIBs are stored */
        uint8_t  hash_کلید[HASH_کلید_SIZE];  /* hash key; changes during resize */

        unsigned n_entries;                /* number of stored entries */
        unsigned n_buckets;                /* number of buckets */

        unsigned idx_lowest_entry;         /* Index below which all buckets are free.
                                              Makes "while (hashmap_steal_first())" loops
                                              O(n) instead of O(n^2) for unordered hashmaps. */
        uint8_t  _pad[3];                  /* padding for the whole HashmapBase */
        /* The bitfields in HashmapBase complete the alignment of the whole thing. */
};

ساختار direct_storage {
        /* This gives us 39 bytes on 64-bit, or 35 bytes on 32-bit.
         * That's room for 4 set_entries + 4 DIB bytes + 3 unused bytes on 64-bit,
         *              or 7 set_entries + 7 DIB bytes + 0 unused bytes on 32-bit. */
        uint8_t storage[اندازه(ساختار indirect_storage)];
};

#define DIRECT_BUCKETS(entry_t) \
        (اندازه(ساختار direct_storage) / (اندازه(entry_t) + اندازه(dib_raw_t)))

/* We should be able to store at least one entry directly. */
assert_cc(DIRECT_BUCKETS(ساختار ordered_hashmap_entry) >= 1);

/* We have 3 bits for n_direct_entries. */
assert_cc(DIRECT_BUCKETS(ساختار set_entry) < (1 << 3));

/* Hashmaps with directly stored entries all use this shared hash key.
 * It's no big deal if the key is guessed, because there can be only
 * a handful of directly stored entries in a hashmap. When a hashmap
 * outgrows direct storage, it gets its own key for indirect storage. */
ایستا uint8_t shared_hash_کلید[HASH_کلید_SIZE];

/* Fields that all hashmap/set types must have */
ساختار HashmapBase {
        ثابت ساختار hash_ops *hash_ops;  /* hash and compare ops to use */

        اجتماع _packed_ {
                ساختار indirect_storage indirect; /* if  has_indirect */
                ساختار direct_storage direct;     /* if !has_indirect */
        };

        شمارش HashmapType type:2;     /* HASHMAP_TYPE_* */
        bool has_indirect:1;         /* whether indirect storage is used */
        unsigned n_direct_entries:3; /* Number of entries in direct storage.
                                      * Only valid if !has_indirect. */
        bool from_pool:1;            /* whether was allocated from mempool */
        bool dirty:1;                /* whether dirtied since last iterated_cache_get() */
        bool cached:1;               /* whether this hashmap is being cached */

#اگر ENABLE_DEBUG_HASHMAP
        ساختار hashmap_debug_info debug;
#endif
};

/* Specific hash types
 * HashmapBase must be at the beginning of each hashmap struct. */

ساختار Hashmap {
        ساختار HashmapBase b;
};

ساختار OrderedHashmap {
        ساختار HashmapBase b;
        unsigned iterate_list_head, iterate_list_tail;
};

ساختار Set {
        ساختار HashmapBase b;
};

تعریفنوع ساختار CacheMem {
        ثابت تهی **ptr;
        size_t n_populated;
        bool active:1;
} CacheMem;

ساختار IteratedCache {
        HashmapBase *hashmap;
        CacheMem کلیدها, values;
};

DEFINE_MEMPOOL(hashmap_pool,         Hashmap,        8);
DEFINE_MEMPOOL(ordered_hashmap_pool, OrderedHashmap, 8);
/* No need for a separate Set pool */
assert_cc(اندازه(Hashmap) == اندازه(Set));

ساختار hashmap_type_info {
        size_t head_size;
        size_t entry_size;
        ساختار mempool *mempool;
        unsigned n_direct_buckets;
};

ایستا _used_ ثابت ساختار hashmap_type_info hashmap_type_info[_HASHMAP_TYPE_MAX] = {
        [HASHMAP_TYPE_PLAIN] = {
                .head_size        = اندازه(Hashmap),
                .entry_size       = اندازه(ساختار plain_hashmap_entry),
                .mempool          = &hashmap_pool,
                .n_direct_buckets = DIRECT_BUCKETS(ساختار plain_hashmap_entry),
        },
        [HASHMAP_TYPE_ORDERED] = {
                .head_size        = اندازه(OrderedHashmap),
                .entry_size       = اندازه(ساختار ordered_hashmap_entry),
                .mempool          = &ordered_hashmap_pool,
                .n_direct_buckets = DIRECT_BUCKETS(ساختار ordered_hashmap_entry),
        },
        [HASHMAP_TYPE_SET] = {
                .head_size        = اندازه(Set),
                .entry_size       = اندازه(ساختار set_entry),
                .mempool          = &hashmap_pool,
                .n_direct_buckets = DIRECT_BUCKETS(ساختار set_entry),
        },
};

تهی hashmap_trim_pools(تهی) {
        صحیح r;

        /* The pool is only allocated by the main thread, but the memory can be passed to other
         * threads. Let's clean up if we are the main thread and no other threads are live. */

        /* We build our own is_main_thread() here, which doesn't use C11 TLS based caching of the
         * result. That's because valgrind apparently doesn't like TLS to be used from a GCC destructor. */
        اگر (getpid() != gettid())
                بازگشت (تهی) log_debug("Not cleaning up memory pools, not in main thread.");

        r = get_process_threads(0);
        اگر (r < 0)
                بازگشت (تهی) log_debug_errno(r, "Failed to determine number of threads, not cleaning up memory pools: %m");
        اگر (r != 1)
                بازگشت (تهی) log_debug("Not cleaning up memory pools, running in multi-threaded process.");

        mempool_trim(&hashmap_pool);
        mempool_trim(&ordered_hashmap_pool);
}

#اگر HAVE_VALGRIND_VALGRIND_H
_destructor_ ایستا تهی cleanup_pools(تهی) {
        /* Be nice to valgrind */
        اگر (RUNNING_ON_VALGRIND)
                hashmap_trim_pools();
}
#endif

ایستا درونخطی unsigned n_buckets(HashmapBase *h) {
        أكد(h);
        بازگشت h->has_indirect ? h->indirect.n_buckets
                               : hashmap_type_info[h->type].n_direct_buckets;
}

ایستا unsigned n_entries(HashmapBase *h) {
        أكد(h);
        بازگشت h->has_indirect ? h->indirect.n_entries
                               : h->n_direct_entries;
}

ایستا تهی n_entries_inc(HashmapBase *h) {
        أكد(h);

        اگر (h->has_indirect)
                h->indirect.n_entries++;
        وگرنه
                h->n_direct_entries++;
}

ایستا تهی n_entries_dec(HashmapBase *h) {
        أكد(h);

        اگر (h->has_indirect)
                h->indirect.n_entries--;
        وگرنه
                h->n_direct_entries--;
}

ایستا درونخطی تهی* storage_ptr(HashmapBase *h) {
        أكد(h);
        بازگشت h->has_indirect ? h->indirect.storage
                               : h->direct.storage;
}

ایستا درونخطی uint8_t* hash_کلید(HashmapBase *h) {
        أكد(h);
        بازگشت h->has_indirect ? h->indirect.hash_کلید
                               : shared_hash_کلید;
}

ایستا unsigned base_bucket_hash(HashmapBase *h, ثابت تهی *p) {
        ساختار siphash state;
        uint64_t hash;

        أكد(h);

        /* The keys of trivial_hash_ops and uint64_hash_ops fit in 64 bits. siphash24_uint64() hashes such
         * a key without the indirect call through hash_ops and keeps the SipHash state in registers. On
         * 32-bit and big-endian architectures, the result differs from the result of trivial_hash_func(),
         * which hashes the key in native byte order. Only this function maps keys to buckets, so the
         * difference does not matter. */
        اگر (h->hash_ops->hash == trivial_hash_func)
                hash = siphash24_uint64((uintptr_t) p, hash_کلید(h));
        وگرنه اگر (h->hash_ops->hash == (hash_func_t) uint64_hash_func)
                hash = siphash24_uint64(*(ثابت uint64_t*) p, hash_کلید(h));
        وگرنه {
                siphash24_init(&state, hash_کلید(h));
                h->hash_ops->hash(p, &state);
                hash = siphash24_finalize(&state);
        }

        /* n_buckets is not a power of two, so a modulo needs a division. Multiplying the upper 32 bits of
         * the hash with n_buckets and keeping the upper 32 bits of the product maps the hash to
         * [0, n_buckets) almost as evenly, without a division. */
        بازگشت (unsigned) (((hash >> 32) * n_buckets(h)) >> 32);
}
#define bucket_hash(h, p) base_bucket_hash(HASHMAP_BASE(h), p)

ایستا تهی base_set_dirty(HashmapBase *h) {
        أكد(h);

        h->dirty = true;
}
#define hashmap_set_dirty(h) base_set_dirty(HASHMAP_BASE(h))

ایستا تهی get_hash_کلید(uint8_t hash_کلید[HASH_کلید_SIZE], bool reuse_is_ok) {
        ایستا uint8_t current[HASH_کلید_SIZE];
        ایستا bool current_initialized = false;

        /* Returns a hash function key to use. In order to keep things
         * fast we will not generate a new key each time we allocate a
         * new hash table. Instead, we'll just reuse the most recently
         * generated one, except if we never generated one or when we
         * are rehashing an entire hash table because we reached a
         * fill level */

        اگر (!current_initialized || !reuse_is_ok) {
                random_bytes(current, اندازه(current));
                current_initialized = true;
        }

        memcpy(hash_کلید, current, اندازه(current));
}

ایستا درونخطی ساختار hashmap_base_entry* bucket_at(HashmapBase *h, unsigned idx) {
        أكد(h);
        بازگشت CAST_ALIGN_PTR(
                        ساختار hashmap_base_entry,
                        (uint8_t *) storage_ptr(h) + idx * hashmap_type_info[h->type].entry_size);
}

ایستا ساختار plain_hashmap_entry* plain_bucket_at(Hashmap *h, unsigned idx) {
        بازگشت (ساختار plain_hashmap_entry*) bucket_at(HASHMAP_BASE(h), idx);
}

ایستا ساختار ordered_hashmap_entry* ordered_bucket_at(OrderedHashmap *h, unsigned idx) {
        بازگشت (ساختار ordered_hashmap_entry*) bucket_at(HASHMAP_BASE(h), idx);
}

ایستا ساختار set_entry *set_bucket_at(Set *h, unsigned idx) {
        بازگشت (ساختار set_entry*) bucket_at(HASHMAP_BASE(h), idx);
}

ایستا ساختار ordered_hashmap_entry* bucket_at_swap(ساختار swap_entries *swap, unsigned idx) {
        أكد(swap);
        بازگشت &swap->e[idx - _IDX_SWAP_BEGIN];
}

/* Returns a pointer to the bucket at index idx.
 * Understands real indexes and swap indexes, hence "_virtual". */
ایستا ساختار hashmap_base_entry* bucket_at_virtual(HashmapBase *h, ساختار swap_entries *swap,
                                                    unsigned idx) {
        اگر (idx < _IDX_SWAP_BEGIN)
                بازگشت bucket_at(h, idx);

        اگر (idx < _IDX_SWAP_END)
                بازگشت &bucket_at_swap(swap, idx)->p.b;

        assert_not_reached();
}

ایستا درونخطی dib_raw_t* dib_raw_ptr(HashmapBase *h) {
        أكد(h);
        بازگشت (dib_raw_t*)
                ((uint8_t*) storage_ptr(h) + hashmap_type_info[h->type].entry_size * n_buckets(h));
}

ایستا unsigned bucket_distance(HashmapBase *h, unsigned idx, unsigned from) {
        بازگشت idx >= from ? idx - from
                           : n_buckets(h) + idx - from;
}

ایستا unsigned bucket_calculate_dib(HashmapBase *h, unsigned idx, dib_raw_t raw_dib) {
        unsigned initial_bucket;

        اگر (raw_dib == DIB_RAW_FREE)
                بازگشت DIB_FREE;

        اگر (_likely_(raw_dib < DIB_RAW_OVERFLOW))
                بازگشت raw_dib;

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
        initial_bucket = bucket_hash(h, bucket_at(h, idx)->کلید);
        بازگشت bucket_distance(h, idx, initial_bucket);
}

ایستا تهی bucket_set_dib(HashmapBase *h, unsigned idx, unsigned dib) {
        dib_raw_ptr(h)[idx] = dib != DIB_FREE ? MIN(dib, DIB_RAW_OVERFLOW) : DIB_RAW_FREE;
}

ایستا unsigned skip_free_buckets(HashmapBase *h, unsigned idx) {
        dib_raw_t *dibs;

        dibs = dib_raw_ptr(h);

        برای ( ; idx < n_buckets(h); idx++)
                اگر (dibs[idx] != DIB_RAW_FREE)
                        بازگشت idx;

        بازگشت IDX_NIL;
}

ایستا تهی bucket_mark_free(HashmapBase *h, unsigned idx) {
        أكد(h);

        memzero(bucket_at(h, idx), hashmap_type_info[h->type].entry_size);
        bucket_set_dib(h, idx, DIB_FREE);
}

ایستا تهی bucket_move_entry(HashmapBase *h, ساختار swap_entries *swap,
                              unsigned from, unsigned to) {
        ساختار hashmap_base_entry *e_from, *e_to;

        أكد(h);
        أكد(from != to);

        e_from = bucket_at_virtual(h, swap, from);
        e_to   = bucket_at_virtual(h, swap, to);

        memcpy(e_to, e_from, hashmap_type_info[h->type].entry_size);

        اگر (h->type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*) h;
                ساختار ordered_hashmap_entry *le, *le_to;

                le_to = (ساختار ordered_hashmap_entry*) e_to;

                اگر (le_to->iterate_next != IDX_NIL) {
                        le = (ساختار ordered_hashmap_entry*)
                             bucket_at_virtual(h, swap, le_to->iterate_next);
                        le->iterate_previous = to;
                }

                اگر (le_to->iterate_previous != IDX_NIL) {
                        le = (ساختار ordered_hashmap_entry*)
                             bucket_at_virtual(h, swap, le_to->iterate_previous);
                        le->iterate_next = to;
                }

                اگر (lh->iterate_list_head == from)
                        lh->iterate_list_head = to;
                اگر (lh->iterate_list_tail == from)
                        lh->iterate_list_tail = to;
        }
}

ایستا درونخطی unsigned next_idx(HashmapBase *h, unsigned idx) {
        بازگشت idx + 1U < n_buckets(h) ? idx + 1U : 0U;
}

ایستا unsigned prev_idx(HashmapBase *h, unsigned idx) {
        بازگشت idx > 0U ? idx - 1U : n_buckets(h) - 1U;
}

ایستا درونخطی تهی* entry_مقدار(HashmapBase *h, ساختار hashmap_base_entry *e) {
        أكد(h);
        أكد(e);

        انتخاب (h->type) {

        حالت HASHMAP_TYPE_PLAIN:
        حالت HASHMAP_TYPE_ORDERED:
                بازگشت ((ساختار plain_hashmap_entry*)e)->مقدار;

        حالت HASHMAP_TYPE_SET:
                بازگشت (تهی*) e->کلید;

        پیشفرض:
                assert_not_reached();
        }
}

ایستا تهی base_remove_entry(HashmapBase *h, unsigned idx) {
        unsigned چپ, راست, prev, dib;
        dib_raw_t raw_dib, *dibs;

        dibs = dib_raw_ptr(h);
        أكد(dibs[idx] != DIB_RAW_FREE);

#اگر ENABLE_DEBUG_HASHMAP
        أكد(h);
        h->debug.rem_شمار++;
        h->debug.last_rem_idx = idx;
#endif

        چپ = idx;
        /* Find the stop bucket ("right"). It is either free or has DIB == 0. */
        برای (راست = next_idx(h, چپ); ; راست = next_idx(h, راست)) {
                raw_dib = dibs[راست];
                اگر (IN_SET(raw_dib, 0, DIB_RAW_FREE))
                        بشکن;

                /* The buckets are not supposed to be all occupied and with DIB > 0.
                 * That would mean we could make everyone better off by shifting them
                 * backward. This scenario is impossible. */
                أكد(چپ != راست);
        }

        اگر (h->type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*) h;
                ساختار ordered_hashmap_entry *le = ordered_bucket_at(lh, idx);

                اگر (le->iterate_next != IDX_NIL)
                        ordered_bucket_at(lh, le->iterate_next)->iterate_previous = le->iterate_previous;
                وگرنه
                        lh->iterate_list_tail = le->iterate_previous;

                اگر (le->iterate_previous != IDX_NIL)
                        ordered_bucket_at(lh, le->iterate_previous)->iterate_next = le->iterate_next;
                وگرنه
                        lh->iterate_list_head = le->iterate_next;
        }

        /* Now shift all buckets in the interval (left, right) one step backwards */
        برای (prev = چپ, چپ = next_idx(h, چپ); چپ != راست;
             prev = چپ, چپ = next_idx(h, چپ)) {
                dib = bucket_calculate_dib(h, چپ, dibs[چپ]);
                أكد(dib != 0);
                bucket_move_entry(h, پوچ, چپ, prev);
                bucket_set_dib(h, prev, dib - 1);
        }

        bucket_mark_free(h, prev);
        n_entries_dec(h);
        base_set_dirty(h);
}
#define remove_entry(h, idx) base_remove_entry(HASHMAP_BASE(h), idx)

ایستا unsigned hashmap_iterate_in_insertion_order(OrderedHashmap *h, Iterator *i) {
        ساختار ordered_hashmap_entry *e;
        unsigned idx;

        أكد(h);
        أكد(i);

        اگر (i->idx == IDX_NIL)
                برو at_end;

        اگر (i->idx == IDX_FIRST && h->iterate_list_head == IDX_NIL)
                برو at_end;

        اگر (i->idx == IDX_FIRST) {
                idx = h->iterate_list_head;
                e = ordered_bucket_at(h, idx);
        } وگرنه {
                idx = i->idx;
                e = ordered_bucket_at(h, idx);
                /*
                 * We allow removing the current entry while iterating, but removal may cause
                 * a backward shift. The next entry may thus move one bucket to the left.
                 * To detect when it happens, we remember the key pointer of the entry we were
                 * going to iterate next. If it does not match, there was a backward shift.
                 */
                اگر (e->p.b.کلید != i->next_کلید) {
                        idx = prev_idx(HASHMAP_BASE(h), idx);
                        e = ordered_bucket_at(h, idx);
                }
                أكد(e->p.b.کلید == i->next_کلید);
        }

#اگر ENABLE_DEBUG_HASHMAP
        i->prev_idx = idx;
#endif

        اگر (e->iterate_next != IDX_NIL) {
                ساختار ordered_hashmap_entry *n;
                i->idx = e->iterate_next;
                n = ordered_bucket_at(h, i->idx);
                i->next_کلید = n->p.b.کلید;
        } وگرنه
                i->idx = IDX_NIL;

        بازگشت idx;

at_end:
        i->idx = IDX_NIL;
        بازگشت IDX_NIL;
}

ایستا unsigned hashmap_iterate_in_internal_order(HashmapBase *h, Iterator *i) {
        unsigned idx;

        أكد(h);
        أكد(i);

        اگر (i->idx == IDX_NIL)
                برو at_end;

        اگر (i->idx == IDX_FIRST) {
                /* fast forward to the first occupied bucket */
                اگر (h->has_indirect) {
                        i->idx = skip_free_buckets(h, h->indirect.idx_lowest_entry);
                        h->indirect.idx_lowest_entry = i->idx;
                } وگرنه
                        i->idx = skip_free_buckets(h, 0);

                اگر (i->idx == IDX_NIL)
                        برو at_end;
        } وگرنه {
                ساختار hashmap_base_entry *e;

                أكد(i->idx > 0);

                e = bucket_at(h, i->idx);
                /*
                 * We allow removing the current entry while iterating, but removal may cause
                 * a backward shift. The next entry may thus move one bucket to the left.
                 * To detect when it happens, we remember the key pointer of the entry we were
                 * going to iterate next. If it does not match, there was a backward shift.
                 */
                اگر (e->کلید != i->next_کلید)
                        e = bucket_at(h, --i->idx);

                أكد(e->کلید == i->next_کلید);
        }

        idx = i->idx;
#اگر ENABLE_DEBUG_HASHMAP
        i->prev_idx = idx;
#endif

        i->idx = skip_free_buckets(h, i->idx + 1);
        اگر (i->idx != IDX_NIL)
                i->next_کلید = bucket_at(h, i->idx)->کلید;
        وگرنه
                i->idx = IDX_NIL;

        بازگشت idx;

at_end:
        i->idx = IDX_NIL;
        بازگشت IDX_NIL;
}

ایستا unsigned hashmap_iterate_entry(HashmapBase *h, Iterator *i) {
        اگر (!h) {
                i->idx = IDX_NIL;
                بازگشت IDX_NIL;
        }

#اگر ENABLE_DEBUG_HASHMAP
        اگر (i->idx == IDX_FIRST) {
                i->put_شمار = h->debug.put_شمار;
                i->rem_شمار = h->debug.rem_شمار;
        } وگرنه {
                /* While iterating, must not add any new entries */
                أكد(i->put_شمار == h->debug.put_شمار);
                /* ... or remove entries other than the current one */
                أكد(i->rem_شمار == h->debug.rem_شمار ||
                       (i->rem_شمار == h->debug.rem_شمار - 1 &&
                        i->prev_idx == h->debug.last_rem_idx));
                /* Reset our removals counter */
                i->rem_شمار = h->debug.rem_شمار;
        }
#endif

        بازگشت h->type == HASHMAP_TYPE_ORDERED ? hashmap_iterate_in_insertion_order((OrderedHashmap*) h, i)
                                               : hashmap_iterate_in_internal_order(h, i);
}

bool _hashmap_iterate(HashmapBase *h, Iterator *i, تهی **مقدار, ثابت تهی **کلید) {
        ساختار hashmap_base_entry *e;
        تهی *data;
        unsigned idx;

        idx = hashmap_iterate_entry(h, i);
        اگر (idx == IDX_NIL) {
                اگر (مقدار)
                        *مقدار = پوچ;
                اگر (کلید)
                        *کلید = پوچ;

                بازگشت false;
        }

        e = bucket_at(h, idx);
        data = entry_مقدار(h, e);
        اگر (مقدار)
                *مقدار = data;
        اگر (کلید)
                *کلید = e->کلید;

        بازگشت true;
}

#define HASHMAP_FOREACH_IDX(idx, h, i) \
        برای ((i) = ITERATOR_FIRST, (idx) = hashmap_iterate_entry((h), &(i)); \
             (idx != IDX_NIL); \
             (idx) = hashmap_iterate_entry((h), &(i)))

IteratedCache* _hashmap_iterated_cache_new(HashmapBase *h) {
        IteratedCache *cache;

        أكد(h);
        أكد(!h->cached);

        اگر (h->cached)
                بازگشت پوچ;

        cache = new0(IteratedCache, 1);
        اگر (!cache)
                بازگشت پوچ;

        cache->hashmap = h;
        h->cached = true;

        بازگشت cache;
}

ایستا تهی reset_direct_storage(HashmapBase *h) {
        ثابت ساختار hashmap_type_info *hi = &hashmap_type_info[h->type];
        تهی *p;

        أكد(!h->has_indirect);

        p = mempset(h->direct.storage, 0, hi->entry_size * hi->n_direct_buckets);
        memset(p, DIB_RAW_INIT, اندازه(dib_raw_t) * hi->n_direct_buckets);
}

ایستا تهی shared_hash_کلید_initialize(تهی) {
        random_bytes(shared_hash_کلید, اندازه(shared_hash_کلید));
}

ایستا ساختار HashmapBase* hashmap_base_new(ثابت ساختار hash_ops *hash_ops, شمارش HashmapType type) {
        HashmapBase *h;
        ثابت ساختار hashmap_type_info *hi = &hashmap_type_info[type];

        bool use_pool = mempool_enabled && mempool_enabled();  /* mempool_enabled is a weak symbol */

        h = use_pool ? mempool_alloc0_tile(hi->mempool) : malloc0(hi->head_size);
        اگر (!h)
                بازگشت پوچ;

        h->type = type;
        h->from_pool = use_pool;
        h->hash_ops = hash_ops ?: &trivial_hash_ops;

        اگر (type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*)h;
                lh->iterate_list_head = lh->iterate_list_tail = IDX_NIL;
        }

        reset_direct_storage(h);

        ایستا pthread_once_t once = PTHREAD_ONCE_INIT;
        assert_se(pthread_once(&once, shared_hash_کلید_initialize) == 0);

#اگر ENABLE_DEBUG_HASHMAP
        assert_se(pthread_mutex_lock(&hashmap_debug_list_mutex) == 0);
        LIST_PREPEND(debug_list, hashmap_debug_list, &h->debug);
        assert_se(pthread_mutex_unlock(&hashmap_debug_list_mutex) == 0);
#endif

        بازگشت h;
}

Hashmap *hashmap_new(ثابت ساختار hash_ops *hash_ops) {
        بازگشت (Hashmap*)        hashmap_base_new(hash_ops, HASHMAP_TYPE_PLAIN);
}

OrderedHashmap *ordered_hashmap_new(ثابت ساختار hash_ops *hash_ops) {
        بازگشت (OrderedHashmap*) hashmap_base_new(hash_ops, HASHMAP_TYPE_ORDERED);
}

Set *set_new(ثابت ساختار hash_ops *hash_ops) {
        بازگشت (Set*)            hashmap_base_new(hash_ops, HASHMAP_TYPE_SET);
}

ایستا صحیح hashmap_base_ensure_allocated(HashmapBase **h, ثابت ساختار hash_ops *hash_ops,
                                         شمارش HashmapType type) {
        HashmapBase *q;

        أكد(h);

        اگر (*h) {
                أكد((*h)->hash_ops == (hash_ops ?: &trivial_hash_ops));
                بازگشت 0;
        }

        q = hashmap_base_new(hash_ops, type);
        اگر (!q)
                بازگشت -ENOMEM;

        *h = q;
        بازگشت 1;
}

صحیح hashmap_ensure_allocated(Hashmap **h, ثابت ساختار hash_ops *hash_ops) {
        بازگشت hashmap_base_ensure_allocated((HashmapBase**)h, hash_ops, HASHMAP_TYPE_PLAIN);
}

صحیح ordered_hashmap_ensure_allocated(OrderedHashmap **h, ثابت ساختار hash_ops *hash_ops) {
        بازگشت hashmap_base_ensure_allocated((HashmapBase**)h, hash_ops, HASHMAP_TYPE_ORDERED);
}

صحیح set_ensure_allocated(Set **s, ثابت ساختار hash_ops *hash_ops) {
        بازگشت hashmap_base_ensure_allocated((HashmapBase**)s, hash_ops, HASHMAP_TYPE_SET);
}

صحیح hashmap_ensure_put(Hashmap **h, ثابت ساختار hash_ops *hash_ops, ثابت تهی *کلید, تهی *مقدار) {
        صحیح r;

        أكد(h);

        r = hashmap_ensure_allocated(h, hash_ops);
        اگر (r < 0)
                بازگشت r;

        بازگشت hashmap_put(*h, کلید, مقدار);
}

صحیح ordered_hashmap_ensure_put(OrderedHashmap **h, ثابت ساختار hash_ops *hash_ops, ثابت تهی *کلید, تهی *مقدار) {
        صحیح r;

        أكد(h);

        r = ordered_hashmap_ensure_allocated(h, hash_ops);
        اگر (r < 0)
                بازگشت r;

        بازگشت ordered_hashmap_put(*h, کلید, مقدار);
}

صحیح ordered_hashmap_ensure_replace(OrderedHashmap **h, ثابت ساختار hash_ops *hash_ops, ثابت تهی *کلید, تهی *مقدار) {
        صحیح r;

        أكد(h);

        r = ordered_hashmap_ensure_allocated(h, hash_ops);
        اگر (r < 0)
                بازگشت r;

        بازگشت ordered_hashmap_replace(*h, کلید, مقدار);
}

صحیح hashmap_ensure_replace(Hashmap **h, ثابت ساختار hash_ops *hash_ops, ثابت تهی *کلید, تهی *مقدار) {
        صحیح r;

        أكد(h);

        r = hashmap_ensure_allocated(h, hash_ops);
        اگر (r < 0)
                بازگشت r;

        بازگشت hashmap_replace(*h, کلید, مقدار);
}

ایستا تهی hashmap_free_no_clear(HashmapBase *h) {
        أكد(!h->has_indirect);
        أكد(h->n_direct_entries == 0);

#اگر ENABLE_DEBUG_HASHMAP
        assert_se(pthread_mutex_lock(&hashmap_debug_list_mutex) == 0);
        LIST_REMOVE(debug_list, hashmap_debug_list, &h->debug);
        assert_se(pthread_mutex_unlock(&hashmap_debug_list_mutex) == 0);
#endif

        اگر (h->from_pool) {
                /* Ensure that the object didn't get migrated between threads. */
                assert_se(is_main_thread());
                mempool_free_tile(hashmap_type_info[h->type].mempool, h);
        } وگرنه
                آزاد(h);
}

HashmapBase* _hashmap_free(HashmapBase *h) {
        اگر (h) {
                _hashmap_clear(h);
                hashmap_free_no_clear(h);
        }

        بازگشت پوچ;
}

تهی _hashmap_clear(HashmapBase *h) {
        اگر (!h)
                بازگشت;

        اگر (h->hash_ops->free_کلید || h->hash_ops->free_مقدار) {

                /* If destructor calls are defined, let's destroy things defensively: let's take the item out of the
                 * hash table, and only then call the destructor functions. If these destructors then try to unregister
                 * themselves from our hash table a second time, the entry is already gone. */

                وقتی (_hashmap_size(h) > 0) {
                        تهی *k = پوچ;
                        تهی *v;

                        v = hashmap_first_کلید_and_مقدار(h, true, &k);

                        اگر (h->hash_ops->free_کلید)
                                h->hash_ops->free_کلید(k);

                        اگر (h->hash_ops->free_مقدار)
                                h->hash_ops->free_مقدار(v);
                }
        }

        اگر (h->has_indirect) {
                آزاد(h->indirect.storage);
                h->has_indirect = false;
        }

        h->n_direct_entries = 0;
        reset_direct_storage(h);

        اگر (h->type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*) h;
                lh->iterate_list_head = lh->iterate_list_tail = IDX_NIL;
        }

        base_set_dirty(h);
}

ایستا صحیح resize_buckets(HashmapBase *h, unsigned entries_add);

/*
 * Finds an empty bucket to put an entry into, starting the scan at 'idx'.
 * Performs Robin Hood swaps as it goes. The entry to put must be placed
 * by the caller into swap slot IDX_PUT.
 * If used for in-place resizing, may leave a displaced entry in swap slot
 * IDX_PUT. Caller must rehash it next.
 * Returns: true if it left a displaced entry to rehash next in IDX_PUT,
 *          false otherwise.
 */
ایستا bool hashmap_put_robin_hood(HashmapBase *h, unsigned idx,
                                   ساختار swap_entries *swap) {
        dib_raw_t raw_dib, *dibs;
        unsigned dib, distance;

#اگر ENABLE_DEBUG_HASHMAP
        أكد(h);
        h->debug.put_شمار++;
#endif

        dibs = dib_raw_ptr(h);

        برای (distance = 0; ; distance++) {
                raw_dib = dibs[idx];
                اگر (IN_SET(raw_dib, DIB_RAW_FREE, DIB_RAW_REHASH)) {
                        اگر (raw_dib == DIB_RAW_REHASH)
                                bucket_move_entry(h, swap, idx, IDX_TMP);

                        اگر (h->has_indirect && h->indirect.idx_lowest_entry > idx)
                                h->indirect.idx_lowest_entry = idx;

                        bucket_set_dib(h, idx, distance);
                        bucket_move_entry(h, swap, IDX_PUT, idx);
                        اگر (raw_dib == DIB_RAW_REHASH) {
                                bucket_move_entry(h, swap, IDX_TMP, IDX_PUT);
                                بازگشت true;
                        }

                        بازگشت false;
                }

                dib = bucket_calculate_dib(h, idx, raw_dib);

                اگر (dib < distance) {
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
ایستا صحیح hashmap_base_put_boldly(HashmapBase *h, unsigned idx,
                                   ساختار swap_entries *swap, bool may_resize) {
        ساختار ordered_hashmap_entry *new_entry;
        صحیح r;

        أكد(idx < n_buckets(h));

        new_entry = bucket_at_swap(swap, IDX_PUT);

        اگر (may_resize) {
                r = resize_buckets(h, 1);
                اگر (r < 0)
                        بازگشت r;
                اگر (r > 0)
                        idx = bucket_hash(h, new_entry->p.b.کلید);
        }
        أكد(n_entries(h) < n_buckets(h));

        اگر (h->type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*) h;

                new_entry->iterate_next = IDX_NIL;
                new_entry->iterate_previous = lh->iterate_list_tail;

                اگر (lh->iterate_list_tail != IDX_NIL) {
                        ساختار ordered_hashmap_entry *old_tail;

                        old_tail = ordered_bucket_at(lh, lh->iterate_list_tail);
                        أكد(old_tail->iterate_next == IDX_NIL);
                        old_tail->iterate_next = IDX_PUT;
                }

                lh->iterate_list_tail = IDX_PUT;
                اگر (lh->iterate_list_head == IDX_NIL)
                        lh->iterate_list_head = IDX_PUT;
        }

        assert_se(hashmap_put_robin_hood(h, idx, swap) == false);

        n_entries_inc(h);
#اگر ENABLE_DEBUG_HASHMAP
        h->debug.max_entries = MAX(h->debug.max_entries, n_entries(h));
#endif

        base_set_dirty(h);

        بازگشت 1;
}
#define hashmap_put_boldly(h, idx, swap, may_resize) \
        hashmap_base_put_boldly(HASHMAP_BASE(h), idx, swap, may_resize)

/*
 * Returns 0 if resize is not needed.
 *         1 if successfully resized.
 *         -ENOMEM on allocation failure.
 */
ایستا صحیح resize_buckets(HashmapBase *h, unsigned entries_add) {
        ساختار swap_entries swap;
        تهی *new_storage;
        dib_raw_t *old_dibs, *new_dibs;
        ثابت ساختار hashmap_type_info *hi;
        unsigned idx, optimal_idx;
        unsigned old_n_buckets, new_n_buckets, n_rehashed, new_n_entries;
        uint8_t new_shift;
        bool rehash_next;

        أكد(h);

        hi = &hashmap_type_info[h->type];
        new_n_entries = n_entries(h) + entries_add;

        /* overflow? */
        اگر (_unlikely_(new_n_entries < entries_add))
                بازگشت -ENOMEM;

        /* For direct storage we allow 100% load, because it's tiny. */
        اگر (!h->has_indirect && new_n_entries <= hi->n_direct_buckets)
                بازگشت 0;

        /*
         * Load factor = n/m = 1 - (1/INV_KEEP_FREE).
         * From it follows: m = n + n/(INV_KEEP_FREE - 1)
         */
        new_n_buckets = new_n_entries + new_n_entries / (INV_KEEP_FREE - 1);
        /* overflow? */
        اگر (_unlikely_(new_n_buckets < new_n_entries))
                بازگشت -ENOMEM;

        old_n_buckets = n_buckets(h);

        اگر (_likely_(new_n_buckets <= old_n_buckets))
                بازگشت 0;

        اگر (_unlikely_(new_n_buckets > UINT_MAX / (hi->entry_size + اندازه(dib_raw_t))))
                بازگشت -ENOMEM;

        new_shift = log2u_round_up(MAX(
                        new_n_buckets * (hi->entry_size + اندازه(dib_raw_t)),
                        2 * اندازه(ساختار direct_storage)));

        /* Realloc storage (buckets and DIB array). */
        new_storage = realloc(h->has_indirect ? h->indirect.storage : پوچ,
                              1U << new_shift);
        اگر (!new_storage)
                بازگشت -ENOMEM;

        /* Must upgrade direct to indirect storage. */
        اگر (!h->has_indirect) {
                memcpy(new_storage, h->direct.storage,
                       old_n_buckets * (hi->entry_size + اندازه(dib_raw_t)));
                h->indirect.n_entries = h->n_direct_entries;
                h->indirect.idx_lowest_entry = 0;
                h->n_direct_entries = 0;
        }

        /* Get a new hash key. If we've just upgraded to indirect storage,
         * allow reusing a previously generated key. It's still a different key
         * from the shared one that we used for direct storage. */
        get_hash_کلید(h->indirect.hash_کلید, !h->has_indirect);

        h->has_indirect = true;
        h->indirect.storage = new_storage;
        h->indirect.n_buckets = (1U << new_shift) /
                                (hi->entry_size + اندازه(dib_raw_t));

        old_dibs = (dib_raw_t*)((uint8_t*) new_storage + hi->entry_size * old_n_buckets);
        new_dibs = dib_raw_ptr(h);

        /*
         * Move the DIB array to the new place, replacing valid DIB values with
         * DIB_RAW_REHASH to indicate all of the used buckets need rehashing.
         * Note: Overlap is not possible, because we have at least doubled the
         * number of buckets and dib_raw_t is smaller than any entry type.
         */
        برای (idx = 0; idx < old_n_buckets; idx++) {
                أكد(old_dibs[idx] != DIB_RAW_REHASH);
                new_dibs[idx] = old_dibs[idx] == DIB_RAW_FREE ? DIB_RAW_FREE
                                                              : DIB_RAW_REHASH;
        }

        /* Zero the area of newly added entries (including the old DIB area) */
        memzero(bucket_at(h, old_n_buckets),
               (n_buckets(h) - old_n_buckets) * hi->entry_size);

        /* The upper half of the new DIB array needs initialization */
        memset(&new_dibs[old_n_buckets], DIB_RAW_INIT,
               (n_buckets(h) - old_n_buckets) * اندازه(dib_raw_t));

        /* Rehash entries that need it */
        n_rehashed = 0;
        برای (idx = 0; idx < old_n_buckets; idx++) {
                اگر (new_dibs[idx] != DIB_RAW_REHASH)
                        ادامه;

                optimal_idx = bucket_hash(h, bucket_at(h, idx)->کلید);

                /*
                 * Not much to do if by luck the entry hashes to its current
                 * location. Just set its DIB.
                 */
                اگر (optimal_idx == idx) {
                        new_dibs[idx] = 0;
                        n_rehashed++;
                        ادامه;
                }

                new_dibs[idx] = DIB_RAW_FREE;
                bucket_move_entry(h, &swap, idx, IDX_PUT);
                /* bucket_move_entry does not clear the source */
                memzero(bucket_at(h, idx), hi->entry_size);

                انجامبده {
                        /*
                         * Find the new bucket for the current entry. This may make
                         * another entry homeless and load it into IDX_PUT.
                         */
                        rehash_next = hashmap_put_robin_hood(h, optimal_idx, &swap);
                        n_rehashed++;

                        /* Did the current entry displace another one? */
                        اگر (rehash_next)
                                optimal_idx = bucket_hash(h, bucket_at_swap(&swap, IDX_PUT)->p.b.کلید);
                } وقتی (rehash_next);
        }

        assert_se(n_rehashed == n_entries(h));

        بازگشت 1;
}

/*
 * Finds an entry with a matching key
 * Returns: index of the found entry, or IDX_NIL if not found.
 */
ایستا unsigned base_bucket_scan(HashmapBase *h, unsigned idx, ثابت تهی *کلید) {
        compare_func_t compare = h->hash_ops->compare;
        ساختار hashmap_base_entry *e;
        unsigned dib, distance;
        dib_raw_t *dibs = dib_raw_ptr(h);

        أكد(idx < n_buckets(h));

        برای (distance = 0; ; distance++) {
                اگر (dibs[idx] == DIB_RAW_FREE)
                        بازگشت IDX_NIL;

                dib = bucket_calculate_dib(h, idx, dibs[idx]);

                اگر (dib < distance)
                        بازگشت IDX_NIL;
                اگر (dib == distance) {
                        e = bucket_at(h, idx);

                        /* Most maps compare keys by pointer. Do that here, without the indirect call. */
                        اگر (compare == trivial_compare_func ? e->کلید == کلید : compare(e->کلید, کلید) == 0)
                                بازگشت idx;
                }

                idx = next_idx(h, idx);
        }
}
#define bucket_scan(h, idx, کلید) base_bucket_scan(HASHMAP_BASE(h), idx, کلید)

صحیح hashmap_put(Hashmap *h, ثابت تهی *کلید, تهی *مقدار) {
        ساختار swap_entries swap;
        ساختار plain_hashmap_entry *e;
        unsigned hash, idx;

        أكد(h);

        hash = bucket_hash(h, کلید);
        idx = bucket_scan(h, hash, کلید);
        اگر (idx != IDX_NIL) {
                e = plain_bucket_at(h, idx);
                اگر (e->مقدار == مقدار)
                        بازگشت 0;
                بازگشت -EEXIST;
        }

        e = &bucket_at_swap(&swap, IDX_PUT)->p;
        e->b.کلید = کلید;
        e->مقدار = مقدار;
        بازگشت hashmap_put_boldly(h, hash, &swap, true);
}

صحیح set_put(Set *s, ثابت تهی *کلید) {
        ساختار swap_entries swap;
        ساختار hashmap_base_entry *e;
        unsigned hash, idx;

        أكد(s);

        hash = bucket_hash(s, کلید);
        idx = bucket_scan(s, hash, کلید);
        اگر (idx != IDX_NIL)
                بازگشت 0;

        e = &bucket_at_swap(&swap, IDX_PUT)->p.b;
        e->کلید = کلید;
        بازگشت hashmap_put_boldly(s, hash, &swap, true);
}

صحیح set_ensure_put(Set **s, ثابت ساختار hash_ops *hash_ops, ثابت تهی *کلید) {
        صحیح r;

        أكد(s);

        r = set_ensure_allocated(s, hash_ops);
        اگر (r < 0)
                بازگشت r;

        بازگشت set_put(*s, کلید);
}

صحیح set_ensure_consume(Set **s, ثابت ساختار hash_ops *hash_ops, تهی *کلید) {
        صحیح r;

        r = set_ensure_put(s, hash_ops, کلید);
        اگر (r <= 0) {
                اگر (hash_ops && hash_ops->free_کلید)
                        hash_ops->free_کلید(کلید);
                وگرنه اگر (hash_ops && hash_ops->free_مقدار)
                        /* Sets store their element in the key slot but may carry a value destructor. */
                        hash_ops->free_مقدار(کلید);
                وگرنه
                        آزاد(کلید);
        }

        بازگشت r;
}

صحیح hashmap_replace(Hashmap *h, ثابت تهی *کلید, تهی *مقدار) {
        ساختار swap_entries swap;
        ساختار plain_hashmap_entry *e;
        unsigned hash, idx;

        أكد(h);

        hash = bucket_hash(h, کلید);
        idx = bucket_scan(h, hash, کلید);
        اگر (idx != IDX_NIL) {
                e = plain_bucket_at(h, idx);
#اگر ENABLE_DEBUG_HASHMAP
                /* Although the key is equal, the key pointer may have changed,
                 * and this would break our assumption for iterating. So count
                 * this operation as incompatible with iteration. */
                اگر (e->b.کلید != کلید) {
                        h->b.debug.put_شمار++;
                        h->b.debug.rem_شمار++;
                        h->b.debug.last_rem_idx = idx;
                }
#endif
                e->b.کلید = کلید;
                e->مقدار = مقدار;
                hashmap_set_dirty(h);

                بازگشت 0;
        }

        e = &bucket_at_swap(&swap, IDX_PUT)->p;
        e->b.کلید = کلید;
        e->مقدار = مقدار;
        بازگشت hashmap_put_boldly(h, hash, &swap, true);
}

صحیح hashmap_update(Hashmap *h, ثابت تهی *کلید, تهی *مقدار) {
        ساختار plain_hashmap_entry *e;
        unsigned hash, idx;

        أكد(h);

        hash = bucket_hash(h, کلید);
        idx = bucket_scan(h, hash, کلید);
        اگر (idx == IDX_NIL)
                بازگشت -ENOENT;

        e = plain_bucket_at(h, idx);
        e->مقدار = مقدار;
        hashmap_set_dirty(h);

        بازگشت 0;
}

تهی* _hashmap_get(HashmapBase *h, ثابت تهی *کلید) {
        ساختار hashmap_base_entry *e;
        unsigned hash, idx;

        اگر (!h)
                بازگشت پوچ;

        hash = bucket_hash(h, کلید);
        idx = bucket_scan(h, hash, کلید);
        اگر (idx == IDX_NIL)
                بازگشت پوچ;

        e = bucket_at(h, idx);
        بازگشت entry_مقدار(h, e);
}

تهی* hashmap_get2(Hashmap *h, ثابت تهی *کلید, تهی **ret) {
        ساختار plain_hashmap_entry *e;
        unsigned hash, idx;

        اگر (!h)
                بازگشت پوچ;

        hash = bucket_hash(h, کلید);
        idx = bucket_scan(h, hash, کلید);
        اگر (idx == IDX_NIL)
                بازگشت پوچ;

        e = plain_bucket_at(h, idx);
        اگر (ret)
                *ret = (تهی*) e->b.کلید;

        بازگشت e->مقدار;
}

bool _hashmap_contains(HashmapBase *h, ثابت تهی *کلید) {
        unsigned hash;

        اگر (!h)
                بازگشت false;

        hash = bucket_hash(h, کلید);
        بازگشت bucket_scan(h, hash, کلید) != IDX_NIL;
}

تهی* _hashmap_remove(HashmapBase *h, ثابت تهی *کلید) {
        ساختار hashmap_base_entry *e;
        unsigned hash, idx;
        تهی *data;

        اگر (!h)
                بازگشت پوچ;

        hash = bucket_hash(h, کلید);
        idx = bucket_scan(h, hash, کلید);
        اگر (idx == IDX_NIL)
                بازگشت پوچ;

        e = bucket_at(h, idx);
        data = entry_مقدار(h, e);
        remove_entry(h, idx);

        بازگشت data;
}

تهی* hashmap_remove2(Hashmap *h, ثابت تهی *کلید, تهی **ret) {
        ساختار plain_hashmap_entry *e;
        unsigned hash, idx;
        تهی *data;

        اگر (!h) {
                اگر (ret)
                        *ret = پوچ;
                بازگشت پوچ;
        }

        hash = bucket_hash(h, کلید);
        idx = bucket_scan(h, hash, کلید);
        اگر (idx == IDX_NIL) {
                اگر (ret)
                        *ret = پوچ;
                بازگشت پوچ;
        }

        e = plain_bucket_at(h, idx);
        data = e->مقدار;
        اگر (ret)
                *ret = (تهی*) e->b.کلید;

        remove_entry(h, idx);

        بازگشت data;
}

صحیح hashmap_remove_and_put(Hashmap *h, ثابت تهی *old_کلید, ثابت تهی *new_کلید, تهی *مقدار) {
        ساختار swap_entries swap;
        ساختار plain_hashmap_entry *e;
        unsigned old_hash, new_hash, idx;

        اگر (!h)
                بازگشت -ENOENT;

        old_hash = bucket_hash(h, old_کلید);
        idx = bucket_scan(h, old_hash, old_کلید);
        اگر (idx == IDX_NIL)
                بازگشت -ENOENT;

        new_hash = bucket_hash(h, new_کلید);
        اگر (bucket_scan(h, new_hash, new_کلید) != IDX_NIL)
                بازگشت -EEXIST;

        remove_entry(h, idx);

        e = &bucket_at_swap(&swap, IDX_PUT)->p;
        e->b.کلید = new_کلید;
        e->مقدار = مقدار;
        assert_se(hashmap_put_boldly(h, new_hash, &swap, false) == 1);

        بازگشت 0;
}

صحیح set_remove_and_put(Set *s, ثابت تهی *old_کلید, ثابت تهی *new_کلید) {
        ساختار swap_entries swap;
        ساختار hashmap_base_entry *e;
        unsigned old_hash, new_hash, idx;

        اگر (!s)
                بازگشت -ENOENT;

        old_hash = bucket_hash(s, old_کلید);
        idx = bucket_scan(s, old_hash, old_کلید);
        اگر (idx == IDX_NIL)
                بازگشت -ENOENT;

        new_hash = bucket_hash(s, new_کلید);
        اگر (bucket_scan(s, new_hash, new_کلید) != IDX_NIL)
                بازگشت -EEXIST;

        remove_entry(s, idx);

        e = &bucket_at_swap(&swap, IDX_PUT)->p.b;
        e->کلید = new_کلید;
        assert_se(hashmap_put_boldly(s, new_hash, &swap, false) == 1);

        بازگشت 0;
}

صحیح hashmap_remove_and_replace(Hashmap *h, ثابت تهی *old_کلید, ثابت تهی *new_کلید, تهی *مقدار) {
        ساختار swap_entries swap;
        ساختار plain_hashmap_entry *e;
        unsigned old_hash, new_hash, idx_old, idx_new;

        اگر (!h)
                بازگشت -ENOENT;

        old_hash = bucket_hash(h, old_کلید);
        idx_old = bucket_scan(h, old_hash, old_کلید);
        اگر (idx_old == IDX_NIL)
                بازگشت -ENOENT;

        old_کلید = bucket_at(HASHMAP_BASE(h), idx_old)->کلید;

        new_hash = bucket_hash(h, new_کلید);
        idx_new = bucket_scan(h, new_hash, new_کلید);
        اگر (idx_new != IDX_NIL)
                اگر (idx_old != idx_new) {
                        remove_entry(h, idx_new);
                        /* Compensate for a possible backward shift. */
                        اگر (old_کلید != bucket_at(HASHMAP_BASE(h), idx_old)->کلید)
                                idx_old = prev_idx(HASHMAP_BASE(h), idx_old);
                        أكد(old_کلید == bucket_at(HASHMAP_BASE(h), idx_old)->کلید);
                }

        remove_entry(h, idx_old);

        e = &bucket_at_swap(&swap, IDX_PUT)->p;
        e->b.کلید = new_کلید;
        e->مقدار = مقدار;
        assert_se(hashmap_put_boldly(h, new_hash, &swap, false) == 1);

        بازگشت 0;
}

تهی* hashmap_remove_مقدار(HashmapBase *h, ثابت تهی *کلید, تهی *مقدار) {
        ساختار hashmap_base_entry *e;
        unsigned hash, idx;

        اگر (!h)
                بازگشت پوچ;

        hash = bucket_hash(h, کلید);
        idx = bucket_scan(h, hash, کلید);
        اگر (idx == IDX_NIL)
                بازگشت پوچ;

        e = bucket_at(h, idx);
        اگر (entry_مقدار(h, e) != مقدار)
                بازگشت پوچ;

        remove_entry(h, idx);

        بازگشت مقدار;
}

ایستا unsigned find_first_entry(HashmapBase *h) {
        Iterator i = ITERATOR_FIRST;

        اگر (!h || !n_entries(h))
                بازگشت IDX_NIL;

        بازگشت hashmap_iterate_entry(h, &i);
}

تهی* hashmap_first_کلید_and_مقدار(HashmapBase *h, bool remove, تهی **ret_کلید) {
        ساختار hashmap_base_entry *e;
        تهی *کلید, *data;
        unsigned idx;

        idx = find_first_entry(h);
        اگر (idx == IDX_NIL) {
                اگر (ret_کلید)
                        *ret_کلید = پوچ;
                بازگشت پوچ;
        }

        e = bucket_at(h, idx);
        کلید = (تهی*) e->کلید;
        data = entry_مقدار(h, e);

        اگر (remove)
                remove_entry(h, idx);

        اگر (ret_کلید)
                *ret_کلید = کلید;

        بازگشت data;
}

unsigned _hashmap_size(HashmapBase *h) {
        اگر (!h)
                بازگشت 0;

        بازگشت n_entries(h);
}

unsigned _hashmap_buckets(HashmapBase *h) {
        اگر (!h)
                بازگشت 0;

        بازگشت n_buckets(h);
}

صحیح _hashmap_merge(Hashmap *h, Hashmap *other) {
        Iterator i;
        unsigned idx;

        أكد(h);

        HASHMAP_FOREACH_IDX(idx, HASHMAP_BASE(other), i) {
                ساختار plain_hashmap_entry *pe = plain_bucket_at(other, idx);
                صحیح r;

                r = hashmap_put(h, pe->b.کلید, pe->مقدار);
                اگر (r < 0 && r != -EEXIST)
                        بازگشت r;
        }

        بازگشت 0;
}

صحیح set_merge(Set *s, Set *other) {
        Iterator i;
        unsigned idx;

        أكد(s);

        HASHMAP_FOREACH_IDX(idx, HASHMAP_BASE(other), i) {
                ساختار set_entry *se = set_bucket_at(other, idx);
                صحیح r;

                r = set_put(s, se->b.کلید);
                اگر (r < 0)
                        بازگشت r;
        }

        بازگشت 0;
}

صحیح _hashmap_reserve(HashmapBase *h, unsigned entries_add) {
        صحیح r;

        أكد(h);

        r = resize_buckets(h, entries_add);
        اگر (r < 0)
                بازگشت r;

        بازگشت 0;
}

/*
 * The same as hashmap_merge(), but every new item from other is moved to h.
 * Keys already in h are skipped and stay in other.
 * Returns: 0 on success.
 *          -ENOMEM on alloc failure, in which case no move has been done.
 */
صحیح _hashmap_move(HashmapBase *h, HashmapBase *other) {
        ساختار swap_entries swap;
        ساختار hashmap_base_entry *e, *n;
        Iterator i;
        unsigned idx;
        صحیح r;

        أكد(h);

        اگر (!other)
                بازگشت 0;

        أكد(other->type == h->type);

        /*
         * This reserves buckets for the worst case, where none of other's
         * entries are yet present in h. This is preferable to risking
         * an allocation failure in the middle of the moving and having to
         * rollback or return a partial result.
         */
        r = resize_buckets(h, n_entries(other));
        اگر (r < 0)
                بازگشت r;

        HASHMAP_FOREACH_IDX(idx, other, i) {
                unsigned h_hash;

                e = bucket_at(other, idx);
                h_hash = bucket_hash(h, e->کلید);
                اگر (bucket_scan(h, h_hash, e->کلید) != IDX_NIL)
                        ادامه;

                n = &bucket_at_swap(&swap, IDX_PUT)->p.b;
                n->کلید = e->کلید;
                اگر (h->type != HASHMAP_TYPE_SET)
                        ((ساختار plain_hashmap_entry*) n)->مقدار =
                                ((ساختار plain_hashmap_entry*) e)->مقدار;
                assert_se(hashmap_put_boldly(h, h_hash, &swap, false) == 1);

                remove_entry(other, idx);
        }

        بازگشت 0;
}

صحیح _hashmap_move_one(HashmapBase *h, HashmapBase *other, ثابت تهی *کلید) {
        ساختار swap_entries swap;
        unsigned h_hash, other_hash, idx;
        ساختار hashmap_base_entry *e, *n;
        صحیح r;

        أكد(h);

        h_hash = bucket_hash(h, کلید);
        اگر (bucket_scan(h, h_hash, کلید) != IDX_NIL)
                بازگشت -EEXIST;

        اگر (!other)
                بازگشت -ENOENT;

        أكد(other->type == h->type);

        other_hash = bucket_hash(other, کلید);
        idx = bucket_scan(other, other_hash, کلید);
        اگر (idx == IDX_NIL)
                بازگشت -ENOENT;

        e = bucket_at(other, idx);

        n = &bucket_at_swap(&swap, IDX_PUT)->p.b;
        n->کلید = e->کلید;
        اگر (h->type != HASHMAP_TYPE_SET)
                ((ساختار plain_hashmap_entry*) n)->مقدار =
                        ((ساختار plain_hashmap_entry*) e)->مقدار;
        r = hashmap_put_boldly(h, h_hash, &swap, true);
        اگر (r < 0)
                بازگشت r;

        remove_entry(other, idx);
        بازگشت 0;
}

HashmapBase* _hashmap_copy(HashmapBase *h) {
        HashmapBase *copy;
        صحیح r;

        أكد(h);

        copy = hashmap_base_new(h->hash_ops, h->type);
        اگر (!copy)
                بازگشت پوچ;

        انتخاب (h->type) {
        حالت HASHMAP_TYPE_PLAIN:
        حالت HASHMAP_TYPE_ORDERED:
                r = hashmap_merge((Hashmap*)copy, (Hashmap*)h);
                بشکن;
        حالت HASHMAP_TYPE_SET:
                r = set_merge((Set*)copy, (Set*)h);
                بشکن;
        پیشفرض:
                assert_not_reached();
        }

        اگر (r < 0)
                بازگشت _hashmap_free(copy);

        بازگشت copy;
}

نویسه** _hashmap_get_strv(HashmapBase *h) {
        نویسه **sv;
        Iterator i;
        unsigned idx, n;

        اگر (!h)
                بازگشت new0(نویسه*, 1);

        sv = new(نویسه*, n_entries(h)+1);
        اگر (!sv)
                بازگشت پوچ;

        n = 0;
        HASHMAP_FOREACH_IDX(idx, h, i)
                sv[n++] = entry_مقدار(h, bucket_at(h, idx));
        sv[n] = پوچ;

        بازگشت sv;
}

نویسه** set_to_strv(Set **s) {
        أكد(s);

        /* This is similar to set_get_strv(), but invalidates the set on success. */

        نویسه **v = new(نویسه*, set_size(*s) + 1);
        اگر (!v)
                بازگشت پوچ;

        برای (نویسه **p = v; (*p = set_steal_first(*s)); p++)
                ;

        أكد(set_isempty(*s));
        *s = set_free(*s);
        بازگشت v;
}

تهی* ordered_hashmap_next(OrderedHashmap *h, ثابت تهی *کلید) {
        ساختار ordered_hashmap_entry *e;
        unsigned hash, idx;

        اگر (!h)
                بازگشت پوچ;

        hash = bucket_hash(h, کلید);
        idx = bucket_scan(h, hash, کلید);
        اگر (idx == IDX_NIL)
                بازگشت پوچ;

        e = ordered_bucket_at(h, idx);
        اگر (e->iterate_next == IDX_NIL)
                بازگشت پوچ;
        بازگشت ordered_bucket_at(h, e->iterate_next)->p.مقدار;
}

صحیح set_consume(Set *s, تهی *مقدار) {
        صحیح r;

        أكد(s);
        أكد(مقدار);

        r = set_put(s, مقدار);
        اگر (r <= 0)
                آزاد(مقدار);

        بازگشت r;
}

صحیح hashmap_put_strdup_full(Hashmap **h, ثابت ساختار hash_ops *hash_ops, ثابت نویسه *k, ثابت نویسه *v) {
        صحیح r;

        أكد(h);

        r = hashmap_ensure_allocated(h, hash_ops);
        اگر (r < 0)
                بازگشت r;

        _cleanup_free_ نویسه *kdup = پوچ, *vdup = پوچ;

        kdup = strdup(k);
        اگر (!kdup)
                بازگشت -ENOMEM;

        اگر (v) {
                vdup = strdup(v);
                اگر (!vdup)
                        بازگشت -ENOMEM;
        }

        r = hashmap_put(*h, kdup, vdup);
        اگر (r < 0) {
                اگر (r == -EEXIST && streq_ptr(v, hashmap_get(*h, kdup)))
                        بازگشت 0;
                بازگشت r;
        }

        /* 0 with non-null vdup would mean vdup is already in the hashmap, which cannot be */
        أكد(vdup == پوچ || r > 0);
        اگر (r > 0)
                kdup = vdup = پوچ;

        بازگشت r;
}

صحیح set_put_strndup_full(Set **s, ثابت ساختار hash_ops *hash_ops, ثابت نویسه *p, size_t n) {
        نویسه *c;
        صحیح r;

        أكد(s);
        أكد(p);

        r = set_ensure_allocated(s, hash_ops);
        اگر (r < 0)
                بازگشت r;

        اگر (n == SIZE_MAX) {
                اگر (set_contains(*s, (نویسه*) p))
                        بازگشت 0;

                c = strdup(p);
        } وگرنه
                c = strndup(p, n);
        اگر (!c)
                بازگشت -ENOMEM;

        بازگشت set_consume(*s, c);
}

صحیح set_put_strdupv_full(Set **s, ثابت ساختار hash_ops *hash_ops, نویسه **l) {
        صحیح n = 0, r;

        أكد(s);

        STRV_FOREACH(i, l) {
                r = set_put_strndup_full(s, hash_ops, *i, SIZE_MAX);
                اگر (r < 0)
                        بازگشت r;

                n += r;
        }

        بازگشت n;
}

صحیح set_put_strsplit(Set *s, ثابت نویسه *v, ثابت نویسه *separators, ExtractFlags flags) {
        ثابت نویسه *p = ASSERT_PTR(v);
        صحیح r;

        أكد(s);

        برای (;;) {
                نویسه *word;

                r = extract_first_word(&p, &word, separators, flags);
                اگر (r <= 0)
                        بازگشت r;

                r = set_consume(s, word);
                اگر (r < 0)
                        بازگشت r;
        }
}

/* expand the cachemem if needed, return true if newly (re)activated. */
ایستا صحیح cachemem_maintain(CacheMem *mem, size_t size) {
        أكد(mem);

        اگر (!GREEDY_REALLOC(mem->ptr, size)) {
                اگر (size > 0)
                        بازگشت -ENOMEM;
        }

        اگر (!mem->active) {
                mem->active = true;
                بازگشت true;
        }

        بازگشت false;
}

صحیح iterated_cache_get(IteratedCache *cache, ثابت تهی ***res_کلیدها, ثابت تهی ***res_مقادیر, unsigned *res_n_entries) {
        bool sync_کلیدها = false, sync_مقادیر = false;
        size_t size;
        صحیح r;

        أكد(cache);
        أكد(cache->hashmap);

        size = n_entries(cache->hashmap);

        اگر (res_کلیدها) {
                r = cachemem_maintain(&cache->کلیدها, size);
                اگر (r < 0)
                        بازگشت r;

                sync_کلیدها = r;
        } وگرنه
                cache->کلیدها.active = false;

        اگر (res_مقادیر) {
                r = cachemem_maintain(&cache->values, size);
                اگر (r < 0)
                        بازگشت r;

                sync_مقادیر = r;
        } وگرنه
                cache->values.active = false;

        اگر (cache->hashmap->dirty) {
                اگر (cache->کلیدها.active)
                        sync_کلیدها = true;
                اگر (cache->values.active)
                        sync_مقادیر = true;

                cache->hashmap->dirty = false;
        }

        اگر (sync_کلیدها || sync_مقادیر) {
                unsigned i, idx;
                Iterator iter;

                i = 0;
                HASHMAP_FOREACH_IDX(idx, cache->hashmap, iter) {
                        ساختار hashmap_base_entry *e;

                        e = bucket_at(cache->hashmap, idx);

                        اگر (sync_کلیدها)
                                cache->کلیدها.ptr[i] = e->کلید;
                        اگر (sync_مقادیر)
                                cache->values.ptr[i] = entry_مقدار(cache->hashmap, e);
                        i++;
                }
        }

        اگر (res_کلیدها)
                *res_کلیدها = cache->کلیدها.ptr;
        اگر (res_مقادیر)
                *res_مقادیر = cache->values.ptr;
        اگر (res_n_entries)
                *res_n_entries = size;

        بازگشت 0;
}

IteratedCache* iterated_cache_free(IteratedCache *cache) {
        اگر (cache) {
                آزاد(cache->کلیدها.ptr);
                آزاد(cache->values.ptr);
        }

        بازگشت mfree(cache);
}

صحیح set_strjoin(Set *s, ثابت نویسه *separator, bool wrap_with_separator, نویسه **ret) {
        _cleanup_free_ نویسه *str = پوچ;
        size_t separator_len, len = 0;
        ثابت نویسه *مقدار;
        bool first;

        أكد(ret);

        اگر (set_isempty(s)) {
                *ret = پوچ;
                بازگشت 0;
        }

        separator_len = strlen_ptr(separator);

        اگر (separator_len == 0)
                wrap_with_separator = false;

        first = !wrap_with_separator;

        SET_FOREACH(مقدار, s) {
                size_t l = strlen_ptr(مقدار);

                اگر (l == 0)
                        ادامه;

                اگر (!GREEDY_REALLOC(str, len + l + (first ? 0 : separator_len) + (wrap_with_separator ? separator_len : 0) + 1))
                        بازگشت -ENOMEM;

                اگر (separator_len > 0 && !first) {
                        memcpy(str + len, separator, separator_len);
                        len += separator_len;
                }

                memcpy(str + len, مقدار, l);
                len += l;
                first = false;
        }

        اگر (wrap_with_separator) {
                memcpy(str + len, separator, separator_len);
                len += separator_len;
        }

        str[len] = '\0';

        *ret = TAKE_PTR(str);
        بازگشت 0;
}

bool set_equal(Set *a, Set *b) {
        تهی *p;

        /* Checks whether each entry of 'a' is also in 'b' and vice versa, i.e. the two sets contain the same
         * entries */

        اگر (a == b)
                بازگشت true;

        اگر (set_isempty(a) && set_isempty(b))
                بازگشت true;

        اگر (set_size(a) != set_size(b)) /* Cheap check that hopefully catches a lot of inequality cases
                                         * already */
                بازگشت false;

        SET_FOREACH(p, a)
                اگر (!set_contains(b, p))
                        بازگشت false;

        /* If we have the same hashops, then we don't need to check things backwards given we compared the
         * size and that all of a is in b. */
        اگر (a->b.hash_ops == b->b.hash_ops)
                بازگشت true;

        SET_FOREACH(p, b)
                اگر (!set_contains(a, p))
                        بازگشت false;

        بازگشت true;
}

ایستا bool set_fnmatch_one(Set *patterns, ثابت نویسه *needle) {
        ثابت نویسه *p;

        أكد(needle);

        /* Any failure of fnmatch() is treated as equivalent to FNM_NOMATCH, i.e. as non-matching pattern */

        SET_FOREACH(p, patterns)
                اگر (fnmatch(p, needle, 0) == 0)
                        بازگشت true;

        بازگشت false;
}

bool set_fnmatch(Set *include_patterns, Set *exclude_patterns, ثابت نویسه *needle) {
        أكد(needle);

        اگر (set_fnmatch_one(exclude_patterns, needle))
                بازگشت false;

        اگر (set_isempty(include_patterns))
                بازگشت true;

        بازگشت set_fnmatch_one(include_patterns, needle);
}

ایستا صحیح hashmap_entry_compare(
                ساختار hashmap_base_entry * ثابت *a,
                ساختار hashmap_base_entry * ثابت *b,
                compare_func_t compare) {

        أكد(a && *a);
        أكد(b && *b);
        أكد(compare);

        بازگشت compare((*a)->کلید, (*b)->کلید);
}

ایستا صحیح _hashmap_dump_entries_sorted(
                HashmapBase *h,
                تهی ***ret,
                size_t *ret_n) {
        _cleanup_free_ تهی **entries = پوچ;
        Iterator iter;
        unsigned idx;
        size_t n = 0;

        أكد(ret);
        أكد(ret_n);

        اگر (_hashmap_size(h) == 0) {
                *ret = پوچ;
                *ret_n = 0;
                بازگشت 0;
        }

        /* We append one more element than needed so that the resulting array can be used as a strv. We
         * don't count this entry in the returned size. */
        entries = new(تهی*, _hashmap_size(h) + 1);
        اگر (!entries)
                بازگشت -ENOMEM;

        HASHMAP_FOREACH_IDX(idx, h, iter)
                entries[n++] = bucket_at(h, idx);

        أكد(n == _hashmap_size(h));
        entries[n] = پوچ;

        typesafe_qsort_r((ساختار hashmap_base_entry**) entries, n,
                         hashmap_entry_compare, h->hash_ops->compare);

        *ret = TAKE_PTR(entries);
        *ret_n = n;
        بازگشت 0;
}

صحیح hashmap_dump_کلیدها_sorted(HashmapBase *h, تهی ***ret, size_t *ret_n) {
        _cleanup_free_ تهی **entries = پوچ;
        size_t n;
        صحیح r;

        أكد(ret);

        r = _hashmap_dump_entries_sorted(h, &entries, &n);
        اگر (r < 0)
                بازگشت r;

        /* Reuse the array. */
        FOREACH_ARRAY(e, entries, n)
                *e = (تهی*) (*(ساختار hashmap_base_entry**) e)->کلید;

        *ret = TAKE_PTR(entries);
        اگر (ret_n)
                *ret_n = n;
        بازگشت 0;
}

صحیح _hashmap_dump_sorted(HashmapBase *h, تهی ***ret, size_t *ret_n) {
        _cleanup_free_ تهی **entries = پوچ;
        size_t n;
        صحیح r;

        أكد(ret);

        r = _hashmap_dump_entries_sorted(h, &entries, &n);
        اگر (r < 0)
                بازگشت r;

        /* Reuse the array. */
        FOREACH_ARRAY(e, entries, n)
                *e = entry_مقدار(h, *(ساختار hashmap_base_entry**) e);

        *ret = TAKE_PTR(entries);
        اگر (ret_n)
                *ret_n = n;
        بازگشت 0;
}
