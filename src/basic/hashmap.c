/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <fnmatch.h>
#अन्तर्भाव <pthread.h>
#अन्तर्भाव <unistd.h>
#जर HAVE_VALGRIND_VALGRIND_H
#  include <valgrind/valgrind.h>
#endif

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "extract-word.h"
#अन्तर्भाव "hashmap.h"
#अन्तर्भाव "log.h"
#अन्तर्भाव "logarithm.h"
#अन्तर्भाव "memory-util.h"
#अन्तर्भाव "mempool.h"
#अन्तर्भाव "process-util.h"
#अन्तर्भाव "random-util.h"
#अन्तर्भाव "set.h"
#अन्तर्भाव "siphash24.h"
#अन्तर्भाव "sort-util.h"
#अन्तर्भाव "string-util.h"
#अन्तर्भाव "strv.h"

#जर ENABLE_DEBUG_HASHMAP
#अन्तर्भाव "list.h"
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
संरचना hashmap_base_entry {
        स्थिरांक शून्य *किल्ली;
};

/* Entry types for specific hashmap/set types
 * hashmap_base_entry must be at the beginning of each entry struct. */

संरचना plain_hashmap_entry {
        संरचना hashmap_base_entry b;
        शून्य *मूल्य;
};

संरचना ordered_hashmap_entry {
        संरचना plain_hashmap_entry p;
        अचिन्हित iterate_next, iterate_previous;
};

संरचना set_entry {
        संरचना hashmap_base_entry b;
};

/* In several functions it is advantageous to have the hash table extended
 * virtually by a couple of additional buckets. We reserve special index values
 * for these "swap" buckets. */
#define _IDX_SWAP_BEGIN     (अच_पूर्णांक_उच्च - 3)
#define IDX_PUT             (_IDX_SWAP_BEGIN + 0)
#define IDX_TMP             (_IDX_SWAP_BEGIN + 1)
#define _IDX_SWAP_END       (_IDX_SWAP_BEGIN + 2)

#define IDX_FIRST           (अच_पूर्णांक_उच्च - 1) /* special index for freshly initialized iterators */
#define IDX_NIL             अच_पूर्णांक_उच्च       /* special index value meaning "none" or "end" */

assert_cc(IDX_FIRST == _IDX_SWAP_END);
assert_cc(IDX_FIRST == _IDX_ITERATOR_FIRST);

/* Storage space for the "swap" buckets.
 * All entry types can fit into an ordered_hashmap_entry. */
संरचना swap_entries {
        संरचना ordered_hashmap_entry e[_IDX_SWAP_END - _IDX_SWAP_BEGIN];
};

/* Distance from Initial Bucket */
प्रकारव्याख्या uint8_t dib_raw_t;
#define DIB_RAW_OVERFLOW ((dib_raw_t)0xfdU)   /* indicates DIB value is greater than representable */
#define DIB_RAW_REHASH   ((dib_raw_t)0xfeU)   /* entry yet to be rehashed during in-place resize */
#define DIB_RAW_FREE     ((dib_raw_t)0xffU)   /* a free bucket */
#define DIB_RAW_INIT     ((अक्षर)DIB_RAW_FREE) /* a byte to memset a DIB store with when initializing */

#define DIB_FREE अच_पूर्णांक_उच्च

#जर ENABLE_DEBUG_HASHMAP
संरचना hashmap_debug_info {
        LIST_FIELDS(संरचना hashmap_debug_info, debug_list);
        अचिन्हित max_entries;  /* high watermark of n_entries */

        /* fields to detect modification while iterating */
        अचिन्हित put_मोजणी;    /* counts puts into the hashmap */
        अचिन्हित rem_मोजणी;    /* counts removals from hashmap */
        अचिन्हित last_rem_idx; /* remembers last removal index */
};

/* Tracks all existing hashmaps. Get at it from gdb. See sd_dump_hashmaps.py */
स्थिर LIST_HEAD(संरचना hashmap_debug_info, hashmap_debug_list);
स्थिर pthread_mutex_t hashmap_debug_list_mutex = PTHREAD_MUTEX_INITIALIZER;
#endif

गणना HashmapType {
        HASHMAP_TYPE_PLAIN,
        HASHMAP_TYPE_ORDERED,
        HASHMAP_TYPE_SET,
        _HASHMAP_TYPE_MAX
};

संरचना _packed_ indirect_storage {
        शून्य *storage;                     /* where buckets and DIBs are stored */
        uint8_t  hash_किल्ली[HASH_किल्ली_SIZE];  /* hash key; changes during resize */

        अचिन्हित n_entries;                /* number of stored entries */
        अचिन्हित n_buckets;                /* number of buckets */

        अचिन्हित idx_lowest_entry;         /* Index below which all buckets are free.
                                              Makes "while (hashmap_steal_first())" loops
                                              O(n) instead of O(n^2) for unordered hashmaps. */
        uint8_t  _pad[3];                  /* padding for the whole HashmapBase */
        /* The bitfields in HashmapBase complete the alignment of the whole thing. */
};

संरचना direct_storage {
        /* This gives us 39 bytes on 64-bit, or 35 bytes on 32-bit.
         * That's room for 4 set_entries + 4 DIB bytes + 3 unused bytes on 64-bit,
         *              or 7 set_entries + 7 DIB bytes + 0 unused bytes on 32-bit. */
        uint8_t storage[आकार(संरचना indirect_storage)];
};

#define DIRECT_BUCKETS(entry_t) \
        (आकार(संरचना direct_storage) / (आकार(entry_t) + आकार(dib_raw_t)))

/* We should be able to store at least one entry directly. */
assert_cc(DIRECT_BUCKETS(संरचना ordered_hashmap_entry) >= 1);

/* We have 3 bits for n_direct_entries. */
assert_cc(DIRECT_BUCKETS(संरचना set_entry) < (1 << 3));

/* Hashmaps with directly stored entries all use this shared hash key.
 * It's no big deal if the key is guessed, because there can be only
 * a handful of directly stored entries in a hashmap. When a hashmap
 * outgrows direct storage, it gets its own key for indirect storage. */
स्थिर uint8_t shared_hash_किल्ली[HASH_किल्ली_SIZE];

/* Fields that all hashmap/set types must have */
संरचना HashmapBase {
        स्थिरांक संरचना hash_ops *hash_ops;  /* hash and compare ops to use */

        संघ _packed_ {
                संरचना indirect_storage indirect; /* if  has_indirect */
                संरचना direct_storage direct;     /* if !has_indirect */
        };

        गणना HashmapType type:2;     /* HASHMAP_TYPE_* */
        bool has_indirect:1;         /* whether indirect storage is used */
        अचिन्हित n_direct_entries:3; /* Number of entries in direct storage.
                                      * Only valid if !has_indirect. */
        bool from_pool:1;            /* whether was allocated from mempool */
        bool dirty:1;                /* whether dirtied since last iterated_cache_get() */
        bool cached:1;               /* whether this hashmap is being cached */

#जर ENABLE_DEBUG_HASHMAP
        संरचना hashmap_debug_info debug;
#endif
};

/* Specific hash types
 * HashmapBase must be at the beginning of each hashmap struct. */

संरचना Hashmap {
        संरचना HashmapBase b;
};

संरचना OrderedHashmap {
        संरचना HashmapBase b;
        अचिन्हित iterate_list_head, iterate_list_tail;
};

संरचना Set {
        संरचना HashmapBase b;
};

प्रकारव्याख्या संरचना CacheMem {
        स्थिरांक शून्य **ptr;
        माप_प्रकार n_populated;
        bool active:1;
} CacheMem;

संरचना IteratedCache {
        HashmapBase *hashmap;
        CacheMem किल्ल्या, values;
};

DEFINE_MEMPOOL(hashmap_pool,         Hashmap,        8);
DEFINE_MEMPOOL(ordered_hashmap_pool, OrderedHashmap, 8);
/* No need for a separate Set pool */
assert_cc(आकार(Hashmap) == आकार(Set));

संरचना hashmap_type_info {
        माप_प्रकार head_size;
        माप_प्रकार entry_size;
        संरचना mempool *mempool;
        अचिन्हित n_direct_buckets;
};

स्थिर _used_ स्थिरांक संरचना hashmap_type_info hashmap_type_info[_HASHMAP_TYPE_MAX] = {
        [HASHMAP_TYPE_PLAIN] = {
                .head_size        = आकार(Hashmap),
                .entry_size       = आकार(संरचना plain_hashmap_entry),
                .mempool          = &hashmap_pool,
                .n_direct_buckets = DIRECT_BUCKETS(संरचना plain_hashmap_entry),
        },
        [HASHMAP_TYPE_ORDERED] = {
                .head_size        = आकार(OrderedHashmap),
                .entry_size       = आकार(संरचना ordered_hashmap_entry),
                .mempool          = &ordered_hashmap_pool,
                .n_direct_buckets = DIRECT_BUCKETS(संरचना ordered_hashmap_entry),
        },
        [HASHMAP_TYPE_SET] = {
                .head_size        = आकार(Set),
                .entry_size       = आकार(संरचना set_entry),
                .mempool          = &hashmap_pool,
                .n_direct_buckets = DIRECT_BUCKETS(संरचना set_entry),
        },
};

शून्य hashmap_trim_pools(शून्य) {
        पूर्णांक r;

        /* The pool is only allocated by the main thread, but the memory can be passed to other
         * threads. Let's clean up if we are the main thread and no other threads are live. */

        /* We build our own is_main_thread() here, which doesn't use C11 TLS based caching of the
         * result. That's because valgrind apparently doesn't like TLS to be used from a GCC destructor. */
        जर (getpid() != gettid())
                परत (शून्य) log_debug("Not cleaning up memory pools, not in main thread.");

        r = get_process_threads(0);
        जर (r < 0)
                परत (शून्य) log_debug_errno(r, "Failed to determine number of threads, not cleaning up memory pools: %m");
        जर (r != 1)
                परत (शून्य) log_debug("Not cleaning up memory pools, running in multi-threaded process.");

        mempool_trim(&hashmap_pool);
        mempool_trim(&ordered_hashmap_pool);
}

#जर HAVE_VALGRIND_VALGRIND_H
_destructor_ स्थिर शून्य cleanup_pools(शून्य) {
        /* Be nice to valgrind */
        जर (RUNNING_ON_VALGRIND)
                hashmap_trim_pools();
}
#endif

स्थिर ओळीत अचिन्हित n_buckets(HashmapBase *h) {
        प्रतिज्ञा(h);
        परत h->has_indirect ? h->indirect.n_buckets
                               : hashmap_type_info[h->type].n_direct_buckets;
}

स्थिर अचिन्हित n_entries(HashmapBase *h) {
        प्रतिज्ञा(h);
        परत h->has_indirect ? h->indirect.n_entries
                               : h->n_direct_entries;
}

स्थिर शून्य n_entries_inc(HashmapBase *h) {
        प्रतिज्ञा(h);

        जर (h->has_indirect)
                h->indirect.n_entries++;
        अन्यथा
                h->n_direct_entries++;
}

स्थिर शून्य n_entries_dec(HashmapBase *h) {
        प्रतिज्ञा(h);

        जर (h->has_indirect)
                h->indirect.n_entries--;
        अन्यथा
                h->n_direct_entries--;
}

स्थिर ओळीत शून्य* storage_ptr(HashmapBase *h) {
        प्रतिज्ञा(h);
        परत h->has_indirect ? h->indirect.storage
                               : h->direct.storage;
}

स्थिर ओळीत uint8_t* hash_किल्ली(HashmapBase *h) {
        प्रतिज्ञा(h);
        परत h->has_indirect ? h->indirect.hash_किल्ली
                               : shared_hash_किल्ली;
}

स्थिर अचिन्हित base_bucket_hash(HashmapBase *h, स्थिरांक शून्य *p) {
        संरचना siphash state;
        uint64_t hash;

        प्रतिज्ञा(h);

        /* The keys of trivial_hash_ops and uint64_hash_ops fit in 64 bits. siphash24_uint64() hashes such
         * a key without the indirect call through hash_ops and keeps the SipHash state in registers. On
         * 32-bit and big-endian architectures, the result differs from the result of trivial_hash_func(),
         * which hashes the key in native byte order. Only this function maps keys to buckets, so the
         * difference does not matter. */
        जर (h->hash_ops->hash == trivial_hash_func)
                hash = siphash24_uint64((uintptr_t) p, hash_किल्ली(h));
        अन्यथा जर (h->hash_ops->hash == (hash_func_t) uint64_hash_func)
                hash = siphash24_uint64(*(स्थिरांक uint64_t*) p, hash_किल्ली(h));
        अन्यथा {
                siphash24_init(&state, hash_किल्ली(h));
                h->hash_ops->hash(p, &state);
                hash = siphash24_finalize(&state);
        }

        /* n_buckets is not a power of two, so a modulo needs a division. Multiplying the upper 32 bits of
         * the hash with n_buckets and keeping the upper 32 bits of the product maps the hash to
         * [0, n_buckets) almost as evenly, without a division. */
        परत (अचिन्हित) (((hash >> 32) * n_buckets(h)) >> 32);
}
#define bucket_hash(h, p) base_bucket_hash(HASHMAP_BASE(h), p)

स्थिर शून्य base_set_dirty(HashmapBase *h) {
        प्रतिज्ञा(h);

        h->dirty = true;
}
#define hashmap_set_dirty(h) base_set_dirty(HASHMAP_BASE(h))

स्थिर शून्य get_hash_किल्ली(uint8_t hash_किल्ली[HASH_किल्ली_SIZE], bool reuse_is_ok) {
        स्थिर uint8_t current[HASH_किल्ली_SIZE];
        स्थिर bool current_initialized = false;

        /* Returns a hash function key to use. In order to keep things
         * fast we will not generate a new key each time we allocate a
         * new hash table. Instead, we'll just reuse the most recently
         * generated one, except if we never generated one or when we
         * are rehashing an entire hash table because we reached a
         * fill level */

        जर (!current_initialized || !reuse_is_ok) {
                random_bytes(current, आकार(current));
                current_initialized = true;
        }

        स_नकल(hash_किल्ली, current, आकार(current));
}

स्थिर ओळीत संरचना hashmap_base_entry* bucket_at(HashmapBase *h, अचिन्हित idx) {
        प्रतिज्ञा(h);
        परत CAST_ALIGN_PTR(
                        संरचना hashmap_base_entry,
                        (uint8_t *) storage_ptr(h) + idx * hashmap_type_info[h->type].entry_size);
}

स्थिर संरचना plain_hashmap_entry* plain_bucket_at(Hashmap *h, अचिन्हित idx) {
        परत (संरचना plain_hashmap_entry*) bucket_at(HASHMAP_BASE(h), idx);
}

स्थिर संरचना ordered_hashmap_entry* ordered_bucket_at(OrderedHashmap *h, अचिन्हित idx) {
        परत (संरचना ordered_hashmap_entry*) bucket_at(HASHMAP_BASE(h), idx);
}

स्थिर संरचना set_entry *set_bucket_at(Set *h, अचिन्हित idx) {
        परत (संरचना set_entry*) bucket_at(HASHMAP_BASE(h), idx);
}

स्थिर संरचना ordered_hashmap_entry* bucket_at_swap(संरचना swap_entries *swap, अचिन्हित idx) {
        प्रतिज्ञा(swap);
        परत &swap->e[idx - _IDX_SWAP_BEGIN];
}

/* Returns a pointer to the bucket at index idx.
 * Understands real indexes and swap indexes, hence "_virtual". */
स्थिर संरचना hashmap_base_entry* bucket_at_virtual(HashmapBase *h, संरचना swap_entries *swap,
                                                    अचिन्हित idx) {
        जर (idx < _IDX_SWAP_BEGIN)
                परत bucket_at(h, idx);

        जर (idx < _IDX_SWAP_END)
                परत &bucket_at_swap(swap, idx)->p.b;

        assert_not_reached();
}

स्थिर ओळीत dib_raw_t* dib_raw_ptr(HashmapBase *h) {
        प्रतिज्ञा(h);
        परत (dib_raw_t*)
                ((uint8_t*) storage_ptr(h) + hashmap_type_info[h->type].entry_size * n_buckets(h));
}

स्थिर अचिन्हित bucket_distance(HashmapBase *h, अचिन्हित idx, अचिन्हित from) {
        परत idx >= from ? idx - from
                           : n_buckets(h) + idx - from;
}

स्थिर अचिन्हित bucket_calculate_dib(HashmapBase *h, अचिन्हित idx, dib_raw_t raw_dib) {
        अचिन्हित initial_bucket;

        जर (raw_dib == DIB_RAW_FREE)
                परत DIB_FREE;

        जर (_likely_(raw_dib < DIB_RAW_OVERFLOW))
                परत raw_dib;

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
        initial_bucket = bucket_hash(h, bucket_at(h, idx)->किल्ली);
        परत bucket_distance(h, idx, initial_bucket);
}

स्थिर शून्य bucket_set_dib(HashmapBase *h, अचिन्हित idx, अचिन्हित dib) {
        dib_raw_ptr(h)[idx] = dib != DIB_FREE ? MIN(dib, DIB_RAW_OVERFLOW) : DIB_RAW_FREE;
}

स्थिर अचिन्हित skip_free_buckets(HashmapBase *h, अचिन्हित idx) {
        dib_raw_t *dibs;

        dibs = dib_raw_ptr(h);

        साठी ( ; idx < n_buckets(h); idx++)
                जर (dibs[idx] != DIB_RAW_FREE)
                        परत idx;

        परत IDX_NIL;
}

स्थिर शून्य bucket_mark_free(HashmapBase *h, अचिन्हित idx) {
        प्रतिज्ञा(h);

        memzero(bucket_at(h, idx), hashmap_type_info[h->type].entry_size);
        bucket_set_dib(h, idx, DIB_FREE);
}

स्थिर शून्य bucket_move_entry(HashmapBase *h, संरचना swap_entries *swap,
                              अचिन्हित from, अचिन्हित to) {
        संरचना hashmap_base_entry *e_from, *e_to;

        प्रतिज्ञा(h);
        प्रतिज्ञा(from != to);

        e_from = bucket_at_virtual(h, swap, from);
        e_to   = bucket_at_virtual(h, swap, to);

        स_नकल(e_to, e_from, hashmap_type_info[h->type].entry_size);

        जर (h->type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*) h;
                संरचना ordered_hashmap_entry *le, *le_to;

                le_to = (संरचना ordered_hashmap_entry*) e_to;

                जर (le_to->iterate_next != IDX_NIL) {
                        le = (संरचना ordered_hashmap_entry*)
                             bucket_at_virtual(h, swap, le_to->iterate_next);
                        le->iterate_previous = to;
                }

                जर (le_to->iterate_previous != IDX_NIL) {
                        le = (संरचना ordered_hashmap_entry*)
                             bucket_at_virtual(h, swap, le_to->iterate_previous);
                        le->iterate_next = to;
                }

                जर (lh->iterate_list_head == from)
                        lh->iterate_list_head = to;
                जर (lh->iterate_list_tail == from)
                        lh->iterate_list_tail = to;
        }
}

स्थिर ओळीत अचिन्हित next_idx(HashmapBase *h, अचिन्हित idx) {
        परत idx + 1U < n_buckets(h) ? idx + 1U : 0U;
}

स्थिर अचिन्हित prev_idx(HashmapBase *h, अचिन्हित idx) {
        परत idx > 0U ? idx - 1U : n_buckets(h) - 1U;
}

स्थिर ओळीत शून्य* entry_मूल्य(HashmapBase *h, संरचना hashmap_base_entry *e) {
        प्रतिज्ञा(h);
        प्रतिज्ञा(e);

        निवड (h->type) {

        स्थिती HASHMAP_TYPE_PLAIN:
        स्थिती HASHMAP_TYPE_ORDERED:
                परत ((संरचना plain_hashmap_entry*)e)->मूल्य;

        स्थिती HASHMAP_TYPE_SET:
                परत (शून्य*) e->किल्ली;

        पूर्वनिर्धारित:
                assert_not_reached();
        }
}

स्थिर शून्य base_remove_entry(HashmapBase *h, अचिन्हित idx) {
        अचिन्हित डावा, उजवा, prev, dib;
        dib_raw_t raw_dib, *dibs;

        dibs = dib_raw_ptr(h);
        प्रतिज्ञा(dibs[idx] != DIB_RAW_FREE);

#जर ENABLE_DEBUG_HASHMAP
        प्रतिज्ञा(h);
        h->debug.rem_मोजणी++;
        h->debug.last_rem_idx = idx;
#endif

        डावा = idx;
        /* Find the stop bucket ("right"). It is either free or has DIB == 0. */
        साठी (उजवा = next_idx(h, डावा); ; उजवा = next_idx(h, उजवा)) {
                raw_dib = dibs[उजवा];
                जर (IN_SET(raw_dib, 0, DIB_RAW_FREE))
                        थांबा;

                /* The buckets are not supposed to be all occupied and with DIB > 0.
                 * That would mean we could make everyone better off by shifting them
                 * backward. This scenario is impossible. */
                प्रतिज्ञा(डावा != उजवा);
        }

        जर (h->type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*) h;
                संरचना ordered_hashmap_entry *le = ordered_bucket_at(lh, idx);

                जर (le->iterate_next != IDX_NIL)
                        ordered_bucket_at(lh, le->iterate_next)->iterate_previous = le->iterate_previous;
                अन्यथा
                        lh->iterate_list_tail = le->iterate_previous;

                जर (le->iterate_previous != IDX_NIL)
                        ordered_bucket_at(lh, le->iterate_previous)->iterate_next = le->iterate_next;
                अन्यथा
                        lh->iterate_list_head = le->iterate_next;
        }

        /* Now shift all buckets in the interval (left, right) one step backwards */
        साठी (prev = डावा, डावा = next_idx(h, डावा); डावा != उजवा;
             prev = डावा, डावा = next_idx(h, डावा)) {
                dib = bucket_calculate_dib(h, डावा, dibs[डावा]);
                प्रतिज्ञा(dib != 0);
                bucket_move_entry(h, रिक्त, डावा, prev);
                bucket_set_dib(h, prev, dib - 1);
        }

        bucket_mark_free(h, prev);
        n_entries_dec(h);
        base_set_dirty(h);
}
#define remove_entry(h, idx) base_remove_entry(HASHMAP_BASE(h), idx)

स्थिर अचिन्हित hashmap_iterate_in_insertion_order(OrderedHashmap *h, Iterator *i) {
        संरचना ordered_hashmap_entry *e;
        अचिन्हित idx;

        प्रतिज्ञा(h);
        प्रतिज्ञा(i);

        जर (i->idx == IDX_NIL)
                जा at_end;

        जर (i->idx == IDX_FIRST && h->iterate_list_head == IDX_NIL)
                जा at_end;

        जर (i->idx == IDX_FIRST) {
                idx = h->iterate_list_head;
                e = ordered_bucket_at(h, idx);
        } अन्यथा {
                idx = i->idx;
                e = ordered_bucket_at(h, idx);
                /*
                 * We allow removing the current entry while iterating, but removal may cause
                 * a backward shift. The next entry may thus move one bucket to the left.
                 * To detect when it happens, we remember the key pointer of the entry we were
                 * going to iterate next. If it does not match, there was a backward shift.
                 */
                जर (e->p.b.किल्ली != i->next_किल्ली) {
                        idx = prev_idx(HASHMAP_BASE(h), idx);
                        e = ordered_bucket_at(h, idx);
                }
                प्रतिज्ञा(e->p.b.किल्ली == i->next_किल्ली);
        }

#जर ENABLE_DEBUG_HASHMAP
        i->prev_idx = idx;
#endif

        जर (e->iterate_next != IDX_NIL) {
                संरचना ordered_hashmap_entry *n;
                i->idx = e->iterate_next;
                n = ordered_bucket_at(h, i->idx);
                i->next_किल्ली = n->p.b.किल्ली;
        } अन्यथा
                i->idx = IDX_NIL;

        परत idx;

at_end:
        i->idx = IDX_NIL;
        परत IDX_NIL;
}

स्थिर अचिन्हित hashmap_iterate_in_internal_order(HashmapBase *h, Iterator *i) {
        अचिन्हित idx;

        प्रतिज्ञा(h);
        प्रतिज्ञा(i);

        जर (i->idx == IDX_NIL)
                जा at_end;

        जर (i->idx == IDX_FIRST) {
                /* fast forward to the first occupied bucket */
                जर (h->has_indirect) {
                        i->idx = skip_free_buckets(h, h->indirect.idx_lowest_entry);
                        h->indirect.idx_lowest_entry = i->idx;
                } अन्यथा
                        i->idx = skip_free_buckets(h, 0);

                जर (i->idx == IDX_NIL)
                        जा at_end;
        } अन्यथा {
                संरचना hashmap_base_entry *e;

                प्रतिज्ञा(i->idx > 0);

                e = bucket_at(h, i->idx);
                /*
                 * We allow removing the current entry while iterating, but removal may cause
                 * a backward shift. The next entry may thus move one bucket to the left.
                 * To detect when it happens, we remember the key pointer of the entry we were
                 * going to iterate next. If it does not match, there was a backward shift.
                 */
                जर (e->किल्ली != i->next_किल्ली)
                        e = bucket_at(h, --i->idx);

                प्रतिज्ञा(e->किल्ली == i->next_किल्ली);
        }

        idx = i->idx;
#जर ENABLE_DEBUG_HASHMAP
        i->prev_idx = idx;
#endif

        i->idx = skip_free_buckets(h, i->idx + 1);
        जर (i->idx != IDX_NIL)
                i->next_किल्ली = bucket_at(h, i->idx)->किल्ली;
        अन्यथा
                i->idx = IDX_NIL;

        परत idx;

at_end:
        i->idx = IDX_NIL;
        परत IDX_NIL;
}

स्थिर अचिन्हित hashmap_iterate_entry(HashmapBase *h, Iterator *i) {
        जर (!h) {
                i->idx = IDX_NIL;
                परत IDX_NIL;
        }

#जर ENABLE_DEBUG_HASHMAP
        जर (i->idx == IDX_FIRST) {
                i->put_मोजणी = h->debug.put_मोजणी;
                i->rem_मोजणी = h->debug.rem_मोजणी;
        } अन्यथा {
                /* While iterating, must not add any new entries */
                प्रतिज्ञा(i->put_मोजणी == h->debug.put_मोजणी);
                /* ... or remove entries other than the current one */
                प्रतिज्ञा(i->rem_मोजणी == h->debug.rem_मोजणी ||
                       (i->rem_मोजणी == h->debug.rem_मोजणी - 1 &&
                        i->prev_idx == h->debug.last_rem_idx));
                /* Reset our removals counter */
                i->rem_मोजणी = h->debug.rem_मोजणी;
        }
#endif

        परत h->type == HASHMAP_TYPE_ORDERED ? hashmap_iterate_in_insertion_order((OrderedHashmap*) h, i)
                                               : hashmap_iterate_in_internal_order(h, i);
}

bool _hashmap_iterate(HashmapBase *h, Iterator *i, शून्य **मूल्य, स्थिरांक शून्य **किल्ली) {
        संरचना hashmap_base_entry *e;
        शून्य *data;
        अचिन्हित idx;

        idx = hashmap_iterate_entry(h, i);
        जर (idx == IDX_NIL) {
                जर (मूल्य)
                        *मूल्य = रिक्त;
                जर (किल्ली)
                        *किल्ली = रिक्त;

                परत false;
        }

        e = bucket_at(h, idx);
        data = entry_मूल्य(h, e);
        जर (मूल्य)
                *मूल्य = data;
        जर (किल्ली)
                *किल्ली = e->किल्ली;

        परत true;
}

#define HASHMAP_FOREACH_IDX(idx, h, i) \
        साठी ((i) = ITERATOR_FIRST, (idx) = hashmap_iterate_entry((h), &(i)); \
             (idx != IDX_NIL); \
             (idx) = hashmap_iterate_entry((h), &(i)))

IteratedCache* _hashmap_iterated_cache_new(HashmapBase *h) {
        IteratedCache *cache;

        प्रतिज्ञा(h);
        प्रतिज्ञा(!h->cached);

        जर (h->cached)
                परत रिक्त;

        cache = new0(IteratedCache, 1);
        जर (!cache)
                परत रिक्त;

        cache->hashmap = h;
        h->cached = true;

        परत cache;
}

स्थिर शून्य reset_direct_storage(HashmapBase *h) {
        स्थिरांक संरचना hashmap_type_info *hi = &hashmap_type_info[h->type];
        शून्य *p;

        प्रतिज्ञा(!h->has_indirect);

        p = mempset(h->direct.storage, 0, hi->entry_size * hi->n_direct_buckets);
        स_रखो(p, DIB_RAW_INIT, आकार(dib_raw_t) * hi->n_direct_buckets);
}

स्थिर शून्य shared_hash_किल्ली_initialize(शून्य) {
        random_bytes(shared_hash_किल्ली, आकार(shared_hash_किल्ली));
}

स्थिर संरचना HashmapBase* hashmap_base_new(स्थिरांक संरचना hash_ops *hash_ops, गणना HashmapType type) {
        HashmapBase *h;
        स्थिरांक संरचना hashmap_type_info *hi = &hashmap_type_info[type];

        bool use_pool = mempool_enabled && mempool_enabled();  /* mempool_enabled is a weak symbol */

        h = use_pool ? mempool_alloc0_tile(hi->mempool) : malloc0(hi->head_size);
        जर (!h)
                परत रिक्त;

        h->type = type;
        h->from_pool = use_pool;
        h->hash_ops = hash_ops ?: &trivial_hash_ops;

        जर (type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*)h;
                lh->iterate_list_head = lh->iterate_list_tail = IDX_NIL;
        }

        reset_direct_storage(h);

        स्थिर pthread_once_t once = PTHREAD_ONCE_INIT;
        assert_se(pthread_once(&once, shared_hash_किल्ली_initialize) == 0);

#जर ENABLE_DEBUG_HASHMAP
        assert_se(pthread_mutex_lock(&hashmap_debug_list_mutex) == 0);
        LIST_PREPEND(debug_list, hashmap_debug_list, &h->debug);
        assert_se(pthread_mutex_unlock(&hashmap_debug_list_mutex) == 0);
#endif

        परत h;
}

Hashmap *hashmap_new(स्थिरांक संरचना hash_ops *hash_ops) {
        परत (Hashmap*)        hashmap_base_new(hash_ops, HASHMAP_TYPE_PLAIN);
}

OrderedHashmap *ordered_hashmap_new(स्थिरांक संरचना hash_ops *hash_ops) {
        परत (OrderedHashmap*) hashmap_base_new(hash_ops, HASHMAP_TYPE_ORDERED);
}

Set *set_new(स्थिरांक संरचना hash_ops *hash_ops) {
        परत (Set*)            hashmap_base_new(hash_ops, HASHMAP_TYPE_SET);
}

स्थिर पूर्णांक hashmap_base_ensure_allocated(HashmapBase **h, स्थिरांक संरचना hash_ops *hash_ops,
                                         गणना HashmapType type) {
        HashmapBase *q;

        प्रतिज्ञा(h);

        जर (*h) {
                प्रतिज्ञा((*h)->hash_ops == (hash_ops ?: &trivial_hash_ops));
                परत 0;
        }

        q = hashmap_base_new(hash_ops, type);
        जर (!q)
                परत -ENOMEM;

        *h = q;
        परत 1;
}

पूर्णांक hashmap_ensure_allocated(Hashmap **h, स्थिरांक संरचना hash_ops *hash_ops) {
        परत hashmap_base_ensure_allocated((HashmapBase**)h, hash_ops, HASHMAP_TYPE_PLAIN);
}

पूर्णांक ordered_hashmap_ensure_allocated(OrderedHashmap **h, स्थिरांक संरचना hash_ops *hash_ops) {
        परत hashmap_base_ensure_allocated((HashmapBase**)h, hash_ops, HASHMAP_TYPE_ORDERED);
}

पूर्णांक set_ensure_allocated(Set **s, स्थिरांक संरचना hash_ops *hash_ops) {
        परत hashmap_base_ensure_allocated((HashmapBase**)s, hash_ops, HASHMAP_TYPE_SET);
}

पूर्णांक hashmap_ensure_put(Hashmap **h, स्थिरांक संरचना hash_ops *hash_ops, स्थिरांक शून्य *किल्ली, शून्य *मूल्य) {
        पूर्णांक r;

        प्रतिज्ञा(h);

        r = hashmap_ensure_allocated(h, hash_ops);
        जर (r < 0)
                परत r;

        परत hashmap_put(*h, किल्ली, मूल्य);
}

पूर्णांक ordered_hashmap_ensure_put(OrderedHashmap **h, स्थिरांक संरचना hash_ops *hash_ops, स्थिरांक शून्य *किल्ली, शून्य *मूल्य) {
        पूर्णांक r;

        प्रतिज्ञा(h);

        r = ordered_hashmap_ensure_allocated(h, hash_ops);
        जर (r < 0)
                परत r;

        परत ordered_hashmap_put(*h, किल्ली, मूल्य);
}

पूर्णांक ordered_hashmap_ensure_replace(OrderedHashmap **h, स्थिरांक संरचना hash_ops *hash_ops, स्थिरांक शून्य *किल्ली, शून्य *मूल्य) {
        पूर्णांक r;

        प्रतिज्ञा(h);

        r = ordered_hashmap_ensure_allocated(h, hash_ops);
        जर (r < 0)
                परत r;

        परत ordered_hashmap_replace(*h, किल्ली, मूल्य);
}

पूर्णांक hashmap_ensure_replace(Hashmap **h, स्थिरांक संरचना hash_ops *hash_ops, स्थिरांक शून्य *किल्ली, शून्य *मूल्य) {
        पूर्णांक r;

        प्रतिज्ञा(h);

        r = hashmap_ensure_allocated(h, hash_ops);
        जर (r < 0)
                परत r;

        परत hashmap_replace(*h, किल्ली, मूल्य);
}

स्थिर शून्य hashmap_free_no_clear(HashmapBase *h) {
        प्रतिज्ञा(!h->has_indirect);
        प्रतिज्ञा(h->n_direct_entries == 0);

#जर ENABLE_DEBUG_HASHMAP
        assert_se(pthread_mutex_lock(&hashmap_debug_list_mutex) == 0);
        LIST_REMOVE(debug_list, hashmap_debug_list, &h->debug);
        assert_se(pthread_mutex_unlock(&hashmap_debug_list_mutex) == 0);
#endif

        जर (h->from_pool) {
                /* Ensure that the object didn't get migrated between threads. */
                assert_se(is_main_thread());
                mempool_free_tile(hashmap_type_info[h->type].mempool, h);
        } अन्यथा
                मुक्त(h);
}

HashmapBase* _hashmap_free(HashmapBase *h) {
        जर (h) {
                _hashmap_clear(h);
                hashmap_free_no_clear(h);
        }

        परत रिक्त;
}

शून्य _hashmap_clear(HashmapBase *h) {
        जर (!h)
                परत;

        जर (h->hash_ops->free_किल्ली || h->hash_ops->free_मूल्य) {

                /* If destructor calls are defined, let's destroy things defensively: let's take the item out of the
                 * hash table, and only then call the destructor functions. If these destructors then try to unregister
                 * themselves from our hash table a second time, the entry is already gone. */

                पर्यंत (_hashmap_size(h) > 0) {
                        शून्य *k = रिक्त;
                        शून्य *v;

                        v = hashmap_first_किल्ली_and_मूल्य(h, true, &k);

                        जर (h->hash_ops->free_किल्ली)
                                h->hash_ops->free_किल्ली(k);

                        जर (h->hash_ops->free_मूल्य)
                                h->hash_ops->free_मूल्य(v);
                }
        }

        जर (h->has_indirect) {
                मुक्त(h->indirect.storage);
                h->has_indirect = false;
        }

        h->n_direct_entries = 0;
        reset_direct_storage(h);

        जर (h->type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*) h;
                lh->iterate_list_head = lh->iterate_list_tail = IDX_NIL;
        }

        base_set_dirty(h);
}

स्थिर पूर्णांक resize_buckets(HashmapBase *h, अचिन्हित entries_add);

/*
 * Finds an empty bucket to put an entry into, starting the scan at 'idx'.
 * Performs Robin Hood swaps as it goes. The entry to put must be placed
 * by the caller into swap slot IDX_PUT.
 * If used for in-place resizing, may leave a displaced entry in swap slot
 * IDX_PUT. Caller must rehash it next.
 * Returns: true if it left a displaced entry to rehash next in IDX_PUT,
 *          false otherwise.
 */
स्थिर bool hashmap_put_robin_hood(HashmapBase *h, अचिन्हित idx,
                                   संरचना swap_entries *swap) {
        dib_raw_t raw_dib, *dibs;
        अचिन्हित dib, distance;

#जर ENABLE_DEBUG_HASHMAP
        प्रतिज्ञा(h);
        h->debug.put_मोजणी++;
#endif

        dibs = dib_raw_ptr(h);

        साठी (distance = 0; ; distance++) {
                raw_dib = dibs[idx];
                जर (IN_SET(raw_dib, DIB_RAW_FREE, DIB_RAW_REHASH)) {
                        जर (raw_dib == DIB_RAW_REHASH)
                                bucket_move_entry(h, swap, idx, IDX_TMP);

                        जर (h->has_indirect && h->indirect.idx_lowest_entry > idx)
                                h->indirect.idx_lowest_entry = idx;

                        bucket_set_dib(h, idx, distance);
                        bucket_move_entry(h, swap, IDX_PUT, idx);
                        जर (raw_dib == DIB_RAW_REHASH) {
                                bucket_move_entry(h, swap, IDX_TMP, IDX_PUT);
                                परत true;
                        }

                        परत false;
                }

                dib = bucket_calculate_dib(h, idx, raw_dib);

                जर (dib < distance) {
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
स्थिर पूर्णांक hashmap_base_put_boldly(HashmapBase *h, अचिन्हित idx,
                                   संरचना swap_entries *swap, bool may_resize) {
        संरचना ordered_hashmap_entry *new_entry;
        पूर्णांक r;

        प्रतिज्ञा(idx < n_buckets(h));

        new_entry = bucket_at_swap(swap, IDX_PUT);

        जर (may_resize) {
                r = resize_buckets(h, 1);
                जर (r < 0)
                        परत r;
                जर (r > 0)
                        idx = bucket_hash(h, new_entry->p.b.किल्ली);
        }
        प्रतिज्ञा(n_entries(h) < n_buckets(h));

        जर (h->type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*) h;

                new_entry->iterate_next = IDX_NIL;
                new_entry->iterate_previous = lh->iterate_list_tail;

                जर (lh->iterate_list_tail != IDX_NIL) {
                        संरचना ordered_hashmap_entry *old_tail;

                        old_tail = ordered_bucket_at(lh, lh->iterate_list_tail);
                        प्रतिज्ञा(old_tail->iterate_next == IDX_NIL);
                        old_tail->iterate_next = IDX_PUT;
                }

                lh->iterate_list_tail = IDX_PUT;
                जर (lh->iterate_list_head == IDX_NIL)
                        lh->iterate_list_head = IDX_PUT;
        }

        assert_se(hashmap_put_robin_hood(h, idx, swap) == false);

        n_entries_inc(h);
#जर ENABLE_DEBUG_HASHMAP
        h->debug.max_entries = MAX(h->debug.max_entries, n_entries(h));
#endif

        base_set_dirty(h);

        परत 1;
}
#define hashmap_put_boldly(h, idx, swap, may_resize) \
        hashmap_base_put_boldly(HASHMAP_BASE(h), idx, swap, may_resize)

/*
 * Returns 0 if resize is not needed.
 *         1 if successfully resized.
 *         -ENOMEM on allocation failure.
 */
स्थिर पूर्णांक resize_buckets(HashmapBase *h, अचिन्हित entries_add) {
        संरचना swap_entries swap;
        शून्य *new_storage;
        dib_raw_t *old_dibs, *new_dibs;
        स्थिरांक संरचना hashmap_type_info *hi;
        अचिन्हित idx, optimal_idx;
        अचिन्हित old_n_buckets, new_n_buckets, n_rehashed, new_n_entries;
        uint8_t new_shift;
        bool rehash_next;

        प्रतिज्ञा(h);

        hi = &hashmap_type_info[h->type];
        new_n_entries = n_entries(h) + entries_add;

        /* overflow? */
        जर (_unlikely_(new_n_entries < entries_add))
                परत -ENOMEM;

        /* For direct storage we allow 100% load, because it's tiny. */
        जर (!h->has_indirect && new_n_entries <= hi->n_direct_buckets)
                परत 0;

        /*
         * Load factor = n/m = 1 - (1/INV_KEEP_FREE).
         * From it follows: m = n + n/(INV_KEEP_FREE - 1)
         */
        new_n_buckets = new_n_entries + new_n_entries / (INV_KEEP_FREE - 1);
        /* overflow? */
        जर (_unlikely_(new_n_buckets < new_n_entries))
                परत -ENOMEM;

        old_n_buckets = n_buckets(h);

        जर (_likely_(new_n_buckets <= old_n_buckets))
                परत 0;

        जर (_unlikely_(new_n_buckets > अच_पूर्णांक_उच्च / (hi->entry_size + आकार(dib_raw_t))))
                परत -ENOMEM;

        new_shift = log2u_round_up(MAX(
                        new_n_buckets * (hi->entry_size + आकार(dib_raw_t)),
                        2 * आकार(संरचना direct_storage)));

        /* Realloc storage (buckets and DIB array). */
        new_storage = पुनः_स्मृति(h->has_indirect ? h->indirect.storage : रिक्त,
                              1U << new_shift);
        जर (!new_storage)
                परत -ENOMEM;

        /* Must upgrade direct to indirect storage. */
        जर (!h->has_indirect) {
                स_नकल(new_storage, h->direct.storage,
                       old_n_buckets * (hi->entry_size + आकार(dib_raw_t)));
                h->indirect.n_entries = h->n_direct_entries;
                h->indirect.idx_lowest_entry = 0;
                h->n_direct_entries = 0;
        }

        /* Get a new hash key. If we've just upgraded to indirect storage,
         * allow reusing a previously generated key. It's still a different key
         * from the shared one that we used for direct storage. */
        get_hash_किल्ली(h->indirect.hash_किल्ली, !h->has_indirect);

        h->has_indirect = true;
        h->indirect.storage = new_storage;
        h->indirect.n_buckets = (1U << new_shift) /
                                (hi->entry_size + आकार(dib_raw_t));

        old_dibs = (dib_raw_t*)((uint8_t*) new_storage + hi->entry_size * old_n_buckets);
        new_dibs = dib_raw_ptr(h);

        /*
         * Move the DIB array to the new place, replacing valid DIB values with
         * DIB_RAW_REHASH to indicate all of the used buckets need rehashing.
         * Note: Overlap is not possible, because we have at least doubled the
         * number of buckets and dib_raw_t is smaller than any entry type.
         */
        साठी (idx = 0; idx < old_n_buckets; idx++) {
                प्रतिज्ञा(old_dibs[idx] != DIB_RAW_REHASH);
                new_dibs[idx] = old_dibs[idx] == DIB_RAW_FREE ? DIB_RAW_FREE
                                                              : DIB_RAW_REHASH;
        }

        /* Zero the area of newly added entries (including the old DIB area) */
        memzero(bucket_at(h, old_n_buckets),
               (n_buckets(h) - old_n_buckets) * hi->entry_size);

        /* The upper half of the new DIB array needs initialization */
        स_रखो(&new_dibs[old_n_buckets], DIB_RAW_INIT,
               (n_buckets(h) - old_n_buckets) * आकार(dib_raw_t));

        /* Rehash entries that need it */
        n_rehashed = 0;
        साठी (idx = 0; idx < old_n_buckets; idx++) {
                जर (new_dibs[idx] != DIB_RAW_REHASH)
                        सुरूठेवा;

                optimal_idx = bucket_hash(h, bucket_at(h, idx)->किल्ली);

                /*
                 * Not much to do if by luck the entry hashes to its current
                 * location. Just set its DIB.
                 */
                जर (optimal_idx == idx) {
                        new_dibs[idx] = 0;
                        n_rehashed++;
                        सुरूठेवा;
                }

                new_dibs[idx] = DIB_RAW_FREE;
                bucket_move_entry(h, &swap, idx, IDX_PUT);
                /* bucket_move_entry does not clear the source */
                memzero(bucket_at(h, idx), hi->entry_size);

                करा {
                        /*
                         * Find the new bucket for the current entry. This may make
                         * another entry homeless and load it into IDX_PUT.
                         */
                        rehash_next = hashmap_put_robin_hood(h, optimal_idx, &swap);
                        n_rehashed++;

                        /* Did the current entry displace another one? */
                        जर (rehash_next)
                                optimal_idx = bucket_hash(h, bucket_at_swap(&swap, IDX_PUT)->p.b.किल्ली);
                } पर्यंत (rehash_next);
        }

        assert_se(n_rehashed == n_entries(h));

        परत 1;
}

/*
 * Finds an entry with a matching key
 * Returns: index of the found entry, or IDX_NIL if not found.
 */
स्थिर अचिन्हित base_bucket_scan(HashmapBase *h, अचिन्हित idx, स्थिरांक शून्य *किल्ली) {
        compare_func_t compare = h->hash_ops->compare;
        संरचना hashmap_base_entry *e;
        अचिन्हित dib, distance;
        dib_raw_t *dibs = dib_raw_ptr(h);

        प्रतिज्ञा(idx < n_buckets(h));

        साठी (distance = 0; ; distance++) {
                जर (dibs[idx] == DIB_RAW_FREE)
                        परत IDX_NIL;

                dib = bucket_calculate_dib(h, idx, dibs[idx]);

                जर (dib < distance)
                        परत IDX_NIL;
                जर (dib == distance) {
                        e = bucket_at(h, idx);

                        /* Most maps compare keys by pointer. Do that here, without the indirect call. */
                        जर (compare == trivial_compare_func ? e->किल्ली == किल्ली : compare(e->किल्ली, किल्ली) == 0)
                                परत idx;
                }

                idx = next_idx(h, idx);
        }
}
#define bucket_scan(h, idx, किल्ली) base_bucket_scan(HASHMAP_BASE(h), idx, किल्ली)

पूर्णांक hashmap_put(Hashmap *h, स्थिरांक शून्य *किल्ली, शून्य *मूल्य) {
        संरचना swap_entries swap;
        संरचना plain_hashmap_entry *e;
        अचिन्हित hash, idx;

        प्रतिज्ञा(h);

        hash = bucket_hash(h, किल्ली);
        idx = bucket_scan(h, hash, किल्ली);
        जर (idx != IDX_NIL) {
                e = plain_bucket_at(h, idx);
                जर (e->मूल्य == मूल्य)
                        परत 0;
                परत -EEXIST;
        }

        e = &bucket_at_swap(&swap, IDX_PUT)->p;
        e->b.किल्ली = किल्ली;
        e->मूल्य = मूल्य;
        परत hashmap_put_boldly(h, hash, &swap, true);
}

पूर्णांक set_put(Set *s, स्थिरांक शून्य *किल्ली) {
        संरचना swap_entries swap;
        संरचना hashmap_base_entry *e;
        अचिन्हित hash, idx;

        प्रतिज्ञा(s);

        hash = bucket_hash(s, किल्ली);
        idx = bucket_scan(s, hash, किल्ली);
        जर (idx != IDX_NIL)
                परत 0;

        e = &bucket_at_swap(&swap, IDX_PUT)->p.b;
        e->किल्ली = किल्ली;
        परत hashmap_put_boldly(s, hash, &swap, true);
}

पूर्णांक set_ensure_put(Set **s, स्थिरांक संरचना hash_ops *hash_ops, स्थिरांक शून्य *किल्ली) {
        पूर्णांक r;

        प्रतिज्ञा(s);

        r = set_ensure_allocated(s, hash_ops);
        जर (r < 0)
                परत r;

        परत set_put(*s, किल्ली);
}

पूर्णांक set_ensure_consume(Set **s, स्थिरांक संरचना hash_ops *hash_ops, शून्य *किल्ली) {
        पूर्णांक r;

        r = set_ensure_put(s, hash_ops, किल्ली);
        जर (r <= 0) {
                जर (hash_ops && hash_ops->free_किल्ली)
                        hash_ops->free_किल्ली(किल्ली);
                अन्यथा जर (hash_ops && hash_ops->free_मूल्य)
                        /* Sets store their element in the key slot but may carry a value destructor. */
                        hash_ops->free_मूल्य(किल्ली);
                अन्यथा
                        मुक्त(किल्ली);
        }

        परत r;
}

पूर्णांक hashmap_replace(Hashmap *h, स्थिरांक शून्य *किल्ली, शून्य *मूल्य) {
        संरचना swap_entries swap;
        संरचना plain_hashmap_entry *e;
        अचिन्हित hash, idx;

        प्रतिज्ञा(h);

        hash = bucket_hash(h, किल्ली);
        idx = bucket_scan(h, hash, किल्ली);
        जर (idx != IDX_NIL) {
                e = plain_bucket_at(h, idx);
#जर ENABLE_DEBUG_HASHMAP
                /* Although the key is equal, the key pointer may have changed,
                 * and this would break our assumption for iterating. So count
                 * this operation as incompatible with iteration. */
                जर (e->b.किल्ली != किल्ली) {
                        h->b.debug.put_मोजणी++;
                        h->b.debug.rem_मोजणी++;
                        h->b.debug.last_rem_idx = idx;
                }
#endif
                e->b.किल्ली = किल्ली;
                e->मूल्य = मूल्य;
                hashmap_set_dirty(h);

                परत 0;
        }

        e = &bucket_at_swap(&swap, IDX_PUT)->p;
        e->b.किल्ली = किल्ली;
        e->मूल्य = मूल्य;
        परत hashmap_put_boldly(h, hash, &swap, true);
}

पूर्णांक hashmap_update(Hashmap *h, स्थिरांक शून्य *किल्ली, शून्य *मूल्य) {
        संरचना plain_hashmap_entry *e;
        अचिन्हित hash, idx;

        प्रतिज्ञा(h);

        hash = bucket_hash(h, किल्ली);
        idx = bucket_scan(h, hash, किल्ली);
        जर (idx == IDX_NIL)
                परत -ENOENT;

        e = plain_bucket_at(h, idx);
        e->मूल्य = मूल्य;
        hashmap_set_dirty(h);

        परत 0;
}

शून्य* _hashmap_get(HashmapBase *h, स्थिरांक शून्य *किल्ली) {
        संरचना hashmap_base_entry *e;
        अचिन्हित hash, idx;

        जर (!h)
                परत रिक्त;

        hash = bucket_hash(h, किल्ली);
        idx = bucket_scan(h, hash, किल्ली);
        जर (idx == IDX_NIL)
                परत रिक्त;

        e = bucket_at(h, idx);
        परत entry_मूल्य(h, e);
}

शून्य* hashmap_get2(Hashmap *h, स्थिरांक शून्य *किल्ली, शून्य **ret) {
        संरचना plain_hashmap_entry *e;
        अचिन्हित hash, idx;

        जर (!h)
                परत रिक्त;

        hash = bucket_hash(h, किल्ली);
        idx = bucket_scan(h, hash, किल्ली);
        जर (idx == IDX_NIL)
                परत रिक्त;

        e = plain_bucket_at(h, idx);
        जर (ret)
                *ret = (शून्य*) e->b.किल्ली;

        परत e->मूल्य;
}

bool _hashmap_contains(HashmapBase *h, स्थिरांक शून्य *किल्ली) {
        अचिन्हित hash;

        जर (!h)
                परत false;

        hash = bucket_hash(h, किल्ली);
        परत bucket_scan(h, hash, किल्ली) != IDX_NIL;
}

शून्य* _hashmap_remove(HashmapBase *h, स्थिरांक शून्य *किल्ली) {
        संरचना hashmap_base_entry *e;
        अचिन्हित hash, idx;
        शून्य *data;

        जर (!h)
                परत रिक्त;

        hash = bucket_hash(h, किल्ली);
        idx = bucket_scan(h, hash, किल्ली);
        जर (idx == IDX_NIL)
                परत रिक्त;

        e = bucket_at(h, idx);
        data = entry_मूल्य(h, e);
        remove_entry(h, idx);

        परत data;
}

शून्य* hashmap_remove2(Hashmap *h, स्थिरांक शून्य *किल्ली, शून्य **ret) {
        संरचना plain_hashmap_entry *e;
        अचिन्हित hash, idx;
        शून्य *data;

        जर (!h) {
                जर (ret)
                        *ret = रिक्त;
                परत रिक्त;
        }

        hash = bucket_hash(h, किल्ली);
        idx = bucket_scan(h, hash, किल्ली);
        जर (idx == IDX_NIL) {
                जर (ret)
                        *ret = रिक्त;
                परत रिक्त;
        }

        e = plain_bucket_at(h, idx);
        data = e->मूल्य;
        जर (ret)
                *ret = (शून्य*) e->b.किल्ली;

        remove_entry(h, idx);

        परत data;
}

पूर्णांक hashmap_remove_and_put(Hashmap *h, स्थिरांक शून्य *old_किल्ली, स्थिरांक शून्य *new_किल्ली, शून्य *मूल्य) {
        संरचना swap_entries swap;
        संरचना plain_hashmap_entry *e;
        अचिन्हित old_hash, new_hash, idx;

        जर (!h)
                परत -ENOENT;

        old_hash = bucket_hash(h, old_किल्ली);
        idx = bucket_scan(h, old_hash, old_किल्ली);
        जर (idx == IDX_NIL)
                परत -ENOENT;

        new_hash = bucket_hash(h, new_किल्ली);
        जर (bucket_scan(h, new_hash, new_किल्ली) != IDX_NIL)
                परत -EEXIST;

        remove_entry(h, idx);

        e = &bucket_at_swap(&swap, IDX_PUT)->p;
        e->b.किल्ली = new_किल्ली;
        e->मूल्य = मूल्य;
        assert_se(hashmap_put_boldly(h, new_hash, &swap, false) == 1);

        परत 0;
}

पूर्णांक set_remove_and_put(Set *s, स्थिरांक शून्य *old_किल्ली, स्थिरांक शून्य *new_किल्ली) {
        संरचना swap_entries swap;
        संरचना hashmap_base_entry *e;
        अचिन्हित old_hash, new_hash, idx;

        जर (!s)
                परत -ENOENT;

        old_hash = bucket_hash(s, old_किल्ली);
        idx = bucket_scan(s, old_hash, old_किल्ली);
        जर (idx == IDX_NIL)
                परत -ENOENT;

        new_hash = bucket_hash(s, new_किल्ली);
        जर (bucket_scan(s, new_hash, new_किल्ली) != IDX_NIL)
                परत -EEXIST;

        remove_entry(s, idx);

        e = &bucket_at_swap(&swap, IDX_PUT)->p.b;
        e->किल्ली = new_किल्ली;
        assert_se(hashmap_put_boldly(s, new_hash, &swap, false) == 1);

        परत 0;
}

पूर्णांक hashmap_remove_and_replace(Hashmap *h, स्थिरांक शून्य *old_किल्ली, स्थिरांक शून्य *new_किल्ली, शून्य *मूल्य) {
        संरचना swap_entries swap;
        संरचना plain_hashmap_entry *e;
        अचिन्हित old_hash, new_hash, idx_old, idx_new;

        जर (!h)
                परत -ENOENT;

        old_hash = bucket_hash(h, old_किल्ली);
        idx_old = bucket_scan(h, old_hash, old_किल्ली);
        जर (idx_old == IDX_NIL)
                परत -ENOENT;

        old_किल्ली = bucket_at(HASHMAP_BASE(h), idx_old)->किल्ली;

        new_hash = bucket_hash(h, new_किल्ली);
        idx_new = bucket_scan(h, new_hash, new_किल्ली);
        जर (idx_new != IDX_NIL)
                जर (idx_old != idx_new) {
                        remove_entry(h, idx_new);
                        /* Compensate for a possible backward shift. */
                        जर (old_किल्ली != bucket_at(HASHMAP_BASE(h), idx_old)->किल्ली)
                                idx_old = prev_idx(HASHMAP_BASE(h), idx_old);
                        प्रतिज्ञा(old_किल्ली == bucket_at(HASHMAP_BASE(h), idx_old)->किल्ली);
                }

        remove_entry(h, idx_old);

        e = &bucket_at_swap(&swap, IDX_PUT)->p;
        e->b.किल्ली = new_किल्ली;
        e->मूल्य = मूल्य;
        assert_se(hashmap_put_boldly(h, new_hash, &swap, false) == 1);

        परत 0;
}

शून्य* hashmap_remove_मूल्य(HashmapBase *h, स्थिरांक शून्य *किल्ली, शून्य *मूल्य) {
        संरचना hashmap_base_entry *e;
        अचिन्हित hash, idx;

        जर (!h)
                परत रिक्त;

        hash = bucket_hash(h, किल्ली);
        idx = bucket_scan(h, hash, किल्ली);
        जर (idx == IDX_NIL)
                परत रिक्त;

        e = bucket_at(h, idx);
        जर (entry_मूल्य(h, e) != मूल्य)
                परत रिक्त;

        remove_entry(h, idx);

        परत मूल्य;
}

स्थिर अचिन्हित find_first_entry(HashmapBase *h) {
        Iterator i = ITERATOR_FIRST;

        जर (!h || !n_entries(h))
                परत IDX_NIL;

        परत hashmap_iterate_entry(h, &i);
}

शून्य* hashmap_first_किल्ली_and_मूल्य(HashmapBase *h, bool हटाओ, शून्य **ret_किल्ली) {
        संरचना hashmap_base_entry *e;
        शून्य *किल्ली, *data;
        अचिन्हित idx;

        idx = find_first_entry(h);
        जर (idx == IDX_NIL) {
                जर (ret_किल्ली)
                        *ret_किल्ली = रिक्त;
                परत रिक्त;
        }

        e = bucket_at(h, idx);
        किल्ली = (शून्य*) e->किल्ली;
        data = entry_मूल्य(h, e);

        जर (हटाओ)
                remove_entry(h, idx);

        जर (ret_किल्ली)
                *ret_किल्ली = किल्ली;

        परत data;
}

अचिन्हित _hashmap_size(HashmapBase *h) {
        जर (!h)
                परत 0;

        परत n_entries(h);
}

अचिन्हित _hashmap_buckets(HashmapBase *h) {
        जर (!h)
                परत 0;

        परत n_buckets(h);
}

पूर्णांक _hashmap_merge(Hashmap *h, Hashmap *other) {
        Iterator i;
        अचिन्हित idx;

        प्रतिज्ञा(h);

        HASHMAP_FOREACH_IDX(idx, HASHMAP_BASE(other), i) {
                संरचना plain_hashmap_entry *pe = plain_bucket_at(other, idx);
                पूर्णांक r;

                r = hashmap_put(h, pe->b.किल्ली, pe->मूल्य);
                जर (r < 0 && r != -EEXIST)
                        परत r;
        }

        परत 0;
}

पूर्णांक set_merge(Set *s, Set *other) {
        Iterator i;
        अचिन्हित idx;

        प्रतिज्ञा(s);

        HASHMAP_FOREACH_IDX(idx, HASHMAP_BASE(other), i) {
                संरचना set_entry *se = set_bucket_at(other, idx);
                पूर्णांक r;

                r = set_put(s, se->b.किल्ली);
                जर (r < 0)
                        परत r;
        }

        परत 0;
}

पूर्णांक _hashmap_reserve(HashmapBase *h, अचिन्हित entries_add) {
        पूर्णांक r;

        प्रतिज्ञा(h);

        r = resize_buckets(h, entries_add);
        जर (r < 0)
                परत r;

        परत 0;
}

/*
 * The same as hashmap_merge(), but every new item from other is moved to h.
 * Keys already in h are skipped and stay in other.
 * Returns: 0 on success.
 *          -ENOMEM on alloc failure, in which case no move has been done.
 */
पूर्णांक _hashmap_move(HashmapBase *h, HashmapBase *other) {
        संरचना swap_entries swap;
        संरचना hashmap_base_entry *e, *n;
        Iterator i;
        अचिन्हित idx;
        पूर्णांक r;

        प्रतिज्ञा(h);

        जर (!other)
                परत 0;

        प्रतिज्ञा(other->type == h->type);

        /*
         * This reserves buckets for the worst case, where none of other's
         * entries are yet present in h. This is preferable to risking
         * an allocation failure in the middle of the moving and having to
         * rollback or return a partial result.
         */
        r = resize_buckets(h, n_entries(other));
        जर (r < 0)
                परत r;

        HASHMAP_FOREACH_IDX(idx, other, i) {
                अचिन्हित h_hash;

                e = bucket_at(other, idx);
                h_hash = bucket_hash(h, e->किल्ली);
                जर (bucket_scan(h, h_hash, e->किल्ली) != IDX_NIL)
                        सुरूठेवा;

                n = &bucket_at_swap(&swap, IDX_PUT)->p.b;
                n->किल्ली = e->किल्ली;
                जर (h->type != HASHMAP_TYPE_SET)
                        ((संरचना plain_hashmap_entry*) n)->मूल्य =
                                ((संरचना plain_hashmap_entry*) e)->मूल्य;
                assert_se(hashmap_put_boldly(h, h_hash, &swap, false) == 1);

                remove_entry(other, idx);
        }

        परत 0;
}

पूर्णांक _hashmap_move_one(HashmapBase *h, HashmapBase *other, स्थिरांक शून्य *किल्ली) {
        संरचना swap_entries swap;
        अचिन्हित h_hash, other_hash, idx;
        संरचना hashmap_base_entry *e, *n;
        पूर्णांक r;

        प्रतिज्ञा(h);

        h_hash = bucket_hash(h, किल्ली);
        जर (bucket_scan(h, h_hash, किल्ली) != IDX_NIL)
                परत -EEXIST;

        जर (!other)
                परत -ENOENT;

        प्रतिज्ञा(other->type == h->type);

        other_hash = bucket_hash(other, किल्ली);
        idx = bucket_scan(other, other_hash, किल्ली);
        जर (idx == IDX_NIL)
                परत -ENOENT;

        e = bucket_at(other, idx);

        n = &bucket_at_swap(&swap, IDX_PUT)->p.b;
        n->किल्ली = e->किल्ली;
        जर (h->type != HASHMAP_TYPE_SET)
                ((संरचना plain_hashmap_entry*) n)->मूल्य =
                        ((संरचना plain_hashmap_entry*) e)->मूल्य;
        r = hashmap_put_boldly(h, h_hash, &swap, true);
        जर (r < 0)
                परत r;

        remove_entry(other, idx);
        परत 0;
}

HashmapBase* _hashmap_copy(HashmapBase *h) {
        HashmapBase *copy;
        पूर्णांक r;

        प्रतिज्ञा(h);

        copy = hashmap_base_new(h->hash_ops, h->type);
        जर (!copy)
                परत रिक्त;

        निवड (h->type) {
        स्थिती HASHMAP_TYPE_PLAIN:
        स्थिती HASHMAP_TYPE_ORDERED:
                r = hashmap_merge((Hashmap*)copy, (Hashmap*)h);
                थांबा;
        स्थिती HASHMAP_TYPE_SET:
                r = set_merge((Set*)copy, (Set*)h);
                थांबा;
        पूर्वनिर्धारित:
                assert_not_reached();
        }

        जर (r < 0)
                परत _hashmap_free(copy);

        परत copy;
}

अक्षर** _hashmap_get_strv(HashmapBase *h) {
        अक्षर **sv;
        Iterator i;
        अचिन्हित idx, n;

        जर (!h)
                परत new0(अक्षर*, 1);

        sv = new(अक्षर*, n_entries(h)+1);
        जर (!sv)
                परत रिक्त;

        n = 0;
        HASHMAP_FOREACH_IDX(idx, h, i)
                sv[n++] = entry_मूल्य(h, bucket_at(h, idx));
        sv[n] = रिक्त;

        परत sv;
}

अक्षर** set_to_strv(Set **s) {
        प्रतिज्ञा(s);

        /* This is similar to set_get_strv(), but invalidates the set on success. */

        अक्षर **v = new(अक्षर*, set_size(*s) + 1);
        जर (!v)
                परत रिक्त;

        साठी (अक्षर **p = v; (*p = set_steal_first(*s)); p++)
                ;

        प्रतिज्ञा(set_isempty(*s));
        *s = set_free(*s);
        परत v;
}

शून्य* ordered_hashmap_next(OrderedHashmap *h, स्थिरांक शून्य *किल्ली) {
        संरचना ordered_hashmap_entry *e;
        अचिन्हित hash, idx;

        जर (!h)
                परत रिक्त;

        hash = bucket_hash(h, किल्ली);
        idx = bucket_scan(h, hash, किल्ली);
        जर (idx == IDX_NIL)
                परत रिक्त;

        e = ordered_bucket_at(h, idx);
        जर (e->iterate_next == IDX_NIL)
                परत रिक्त;
        परत ordered_bucket_at(h, e->iterate_next)->p.मूल्य;
}

पूर्णांक set_consume(Set *s, शून्य *मूल्य) {
        पूर्णांक r;

        प्रतिज्ञा(s);
        प्रतिज्ञा(मूल्य);

        r = set_put(s, मूल्य);
        जर (r <= 0)
                मुक्त(मूल्य);

        परत r;
}

पूर्णांक hashmap_put_strdup_full(Hashmap **h, स्थिरांक संरचना hash_ops *hash_ops, स्थिरांक अक्षर *k, स्थिरांक अक्षर *v) {
        पूर्णांक r;

        प्रतिज्ञा(h);

        r = hashmap_ensure_allocated(h, hash_ops);
        जर (r < 0)
                परत r;

        _cleanup_free_ अक्षर *kdup = रिक्त, *vdup = रिक्त;

        kdup = strdup(k);
        जर (!kdup)
                परत -ENOMEM;

        जर (v) {
                vdup = strdup(v);
                जर (!vdup)
                        परत -ENOMEM;
        }

        r = hashmap_put(*h, kdup, vdup);
        जर (r < 0) {
                जर (r == -EEXIST && streq_ptr(v, hashmap_get(*h, kdup)))
                        परत 0;
                परत r;
        }

        /* 0 with non-null vdup would mean vdup is already in the hashmap, which cannot be */
        प्रतिज्ञा(vdup == रिक्त || r > 0);
        जर (r > 0)
                kdup = vdup = रिक्त;

        परत r;
}

पूर्णांक set_put_strndup_full(Set **s, स्थिरांक संरचना hash_ops *hash_ops, स्थिरांक अक्षर *p, माप_प्रकार n) {
        अक्षर *c;
        पूर्णांक r;

        प्रतिज्ञा(s);
        प्रतिज्ञा(p);

        r = set_ensure_allocated(s, hash_ops);
        जर (r < 0)
                परत r;

        जर (n == SIZE_MAX) {
                जर (set_contains(*s, (अक्षर*) p))
                        परत 0;

                c = strdup(p);
        } अन्यथा
                c = strndup(p, n);
        जर (!c)
                परत -ENOMEM;

        परत set_consume(*s, c);
}

पूर्णांक set_put_strdupv_full(Set **s, स्थिरांक संरचना hash_ops *hash_ops, अक्षर **l) {
        पूर्णांक n = 0, r;

        प्रतिज्ञा(s);

        STRV_FOREACH(i, l) {
                r = set_put_strndup_full(s, hash_ops, *i, SIZE_MAX);
                जर (r < 0)
                        परत r;

                n += r;
        }

        परत n;
}

पूर्णांक set_put_strsplit(Set *s, स्थिरांक अक्षर *v, स्थिरांक अक्षर *separators, ExtractFlags flags) {
        स्थिरांक अक्षर *p = ASSERT_PTR(v);
        पूर्णांक r;

        प्रतिज्ञा(s);

        साठी (;;) {
                अक्षर *word;

                r = extract_first_word(&p, &word, separators, flags);
                जर (r <= 0)
                        परत r;

                r = set_consume(s, word);
                जर (r < 0)
                        परत r;
        }
}

/* expand the cachemem if needed, return true if newly (re)activated. */
स्थिर पूर्णांक cachemem_maintain(CacheMem *mem, माप_प्रकार size) {
        प्रतिज्ञा(mem);

        जर (!GREEDY_REALLOC(mem->ptr, size)) {
                जर (size > 0)
                        परत -ENOMEM;
        }

        जर (!mem->active) {
                mem->active = true;
                परत true;
        }

        परत false;
}

पूर्णांक iterated_cache_get(IteratedCache *cache, स्थिरांक शून्य ***res_किल्ल्या, स्थिरांक शून्य ***res_मूल्ये, अचिन्हित *res_n_entries) {
        bool sync_किल्ल्या = false, sync_मूल्ये = false;
        माप_प्रकार size;
        पूर्णांक r;

        प्रतिज्ञा(cache);
        प्रतिज्ञा(cache->hashmap);

        size = n_entries(cache->hashmap);

        जर (res_किल्ल्या) {
                r = cachemem_maintain(&cache->किल्ल्या, size);
                जर (r < 0)
                        परत r;

                sync_किल्ल्या = r;
        } अन्यथा
                cache->किल्ल्या.active = false;

        जर (res_मूल्ये) {
                r = cachemem_maintain(&cache->values, size);
                जर (r < 0)
                        परत r;

                sync_मूल्ये = r;
        } अन्यथा
                cache->values.active = false;

        जर (cache->hashmap->dirty) {
                जर (cache->किल्ल्या.active)
                        sync_किल्ल्या = true;
                जर (cache->values.active)
                        sync_मूल्ये = true;

                cache->hashmap->dirty = false;
        }

        जर (sync_किल्ल्या || sync_मूल्ये) {
                अचिन्हित i, idx;
                Iterator iter;

                i = 0;
                HASHMAP_FOREACH_IDX(idx, cache->hashmap, iter) {
                        संरचना hashmap_base_entry *e;

                        e = bucket_at(cache->hashmap, idx);

                        जर (sync_किल्ल्या)
                                cache->किल्ल्या.ptr[i] = e->किल्ली;
                        जर (sync_मूल्ये)
                                cache->values.ptr[i] = entry_मूल्य(cache->hashmap, e);
                        i++;
                }
        }

        जर (res_किल्ल्या)
                *res_किल्ल्या = cache->किल्ल्या.ptr;
        जर (res_मूल्ये)
                *res_मूल्ये = cache->values.ptr;
        जर (res_n_entries)
                *res_n_entries = size;

        परत 0;
}

IteratedCache* iterated_cache_free(IteratedCache *cache) {
        जर (cache) {
                मुक्त(cache->किल्ल्या.ptr);
                मुक्त(cache->values.ptr);
        }

        परत mfree(cache);
}

पूर्णांक set_strjoin(Set *s, स्थिरांक अक्षर *separator, bool wrap_with_separator, अक्षर **ret) {
        _cleanup_free_ अक्षर *str = रिक्त;
        माप_प्रकार separator_len, len = 0;
        स्थिरांक अक्षर *मूल्य;
        bool first;

        प्रतिज्ञा(ret);

        जर (set_isempty(s)) {
                *ret = रिक्त;
                परत 0;
        }

        separator_len = strlen_ptr(separator);

        जर (separator_len == 0)
                wrap_with_separator = false;

        first = !wrap_with_separator;

        SET_FOREACH(मूल्य, s) {
                माप_प्रकार l = strlen_ptr(मूल्य);

                जर (l == 0)
                        सुरूठेवा;

                जर (!GREEDY_REALLOC(str, len + l + (first ? 0 : separator_len) + (wrap_with_separator ? separator_len : 0) + 1))
                        परत -ENOMEM;

                जर (separator_len > 0 && !first) {
                        स_नकल(str + len, separator, separator_len);
                        len += separator_len;
                }

                स_नकल(str + len, मूल्य, l);
                len += l;
                first = false;
        }

        जर (wrap_with_separator) {
                स_नकल(str + len, separator, separator_len);
                len += separator_len;
        }

        str[len] = '\0';

        *ret = TAKE_PTR(str);
        परत 0;
}

bool set_equal(Set *a, Set *b) {
        शून्य *p;

        /* Checks whether each entry of 'a' is also in 'b' and vice versa, i.e. the two sets contain the same
         * entries */

        जर (a == b)
                परत true;

        जर (set_isempty(a) && set_isempty(b))
                परत true;

        जर (set_size(a) != set_size(b)) /* Cheap check that hopefully catches a lot of inequality cases
                                         * already */
                परत false;

        SET_FOREACH(p, a)
                जर (!set_contains(b, p))
                        परत false;

        /* If we have the same hashops, then we don't need to check things backwards given we compared the
         * size and that all of a is in b. */
        जर (a->b.hash_ops == b->b.hash_ops)
                परत true;

        SET_FOREACH(p, b)
                जर (!set_contains(a, p))
                        परत false;

        परत true;
}

स्थिर bool set_fnmatch_one(Set *patterns, स्थिरांक अक्षर *needle) {
        स्थिरांक अक्षर *p;

        प्रतिज्ञा(needle);

        /* Any failure of fnmatch() is treated as equivalent to FNM_NOMATCH, i.e. as non-matching pattern */

        SET_FOREACH(p, patterns)
                जर (fnmatch(p, needle, 0) == 0)
                        परत true;

        परत false;
}

bool set_fnmatch(Set *include_patterns, Set *exclude_patterns, स्थिरांक अक्षर *needle) {
        प्रतिज्ञा(needle);

        जर (set_fnmatch_one(exclude_patterns, needle))
                परत false;

        जर (set_isempty(include_patterns))
                परत true;

        परत set_fnmatch_one(include_patterns, needle);
}

स्थिर पूर्णांक hashmap_entry_compare(
                संरचना hashmap_base_entry * स्थिरांक *a,
                संरचना hashmap_base_entry * स्थिरांक *b,
                compare_func_t compare) {

        प्रतिज्ञा(a && *a);
        प्रतिज्ञा(b && *b);
        प्रतिज्ञा(compare);

        परत compare((*a)->किल्ली, (*b)->किल्ली);
}

स्थिर पूर्णांक _hashmap_dump_entries_sorted(
                HashmapBase *h,
                शून्य ***ret,
                माप_प्रकार *ret_n) {
        _cleanup_free_ शून्य **entries = रिक्त;
        Iterator iter;
        अचिन्हित idx;
        माप_प्रकार n = 0;

        प्रतिज्ञा(ret);
        प्रतिज्ञा(ret_n);

        जर (_hashmap_size(h) == 0) {
                *ret = रिक्त;
                *ret_n = 0;
                परत 0;
        }

        /* We append one more element than needed so that the resulting array can be used as a strv. We
         * don't count this entry in the returned size. */
        entries = new(शून्य*, _hashmap_size(h) + 1);
        जर (!entries)
                परत -ENOMEM;

        HASHMAP_FOREACH_IDX(idx, h, iter)
                entries[n++] = bucket_at(h, idx);

        प्रतिज्ञा(n == _hashmap_size(h));
        entries[n] = रिक्त;

        typesafe_qsort_r((संरचना hashmap_base_entry**) entries, n,
                         hashmap_entry_compare, h->hash_ops->compare);

        *ret = TAKE_PTR(entries);
        *ret_n = n;
        परत 0;
}

पूर्णांक hashmap_dump_किल्ल्या_sorted(HashmapBase *h, शून्य ***ret, माप_प्रकार *ret_n) {
        _cleanup_free_ शून्य **entries = रिक्त;
        माप_प्रकार n;
        पूर्णांक r;

        प्रतिज्ञा(ret);

        r = _hashmap_dump_entries_sorted(h, &entries, &n);
        जर (r < 0)
                परत r;

        /* Reuse the array. */
        FOREACH_ARRAY(e, entries, n)
                *e = (शून्य*) (*(संरचना hashmap_base_entry**) e)->किल्ली;

        *ret = TAKE_PTR(entries);
        जर (ret_n)
                *ret_n = n;
        परत 0;
}

पूर्णांक _hashmap_dump_sorted(HashmapBase *h, शून्य ***ret, माप_प्रकार *ret_n) {
        _cleanup_free_ शून्य **entries = रिक्त;
        माप_प्रकार n;
        पूर्णांक r;

        प्रतिज्ञा(ret);

        r = _hashmap_dump_entries_sorted(h, &entries, &n);
        जर (r < 0)
                परत r;

        /* Reuse the array. */
        FOREACH_ARRAY(e, entries, n)
                *e = entry_मूल्य(h, *(संरचना hashmap_base_entry**) e);

        *ret = TAKE_PTR(entries);
        जर (ret_n)
                *ret_n = n;
        परत 0;
}
