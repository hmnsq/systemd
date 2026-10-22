/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <fnmatch.h>
#अन्तर्भाव <pthread.h>
#अन्तर्भाव <unistd.h>
#यदि HAVE_VALGRIND_VALGRIND_H
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

#यदि ENABLE_DEBUG_HASHMAP
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
        अचर शून्य *साँचो;
};

/* Entry types for specific hashmap/set types
 * hashmap_base_entry must be at the beginning of each entry struct. */

संरचना plain_hashmap_entry {
        संरचना hashmap_base_entry b;
        शून्य *मान;
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
प्रकारनिर्देश uint8_t dib_raw_t;
#define DIB_RAW_OVERFLOW ((dib_raw_t)0xfdU)   /* indicates DIB value is greater than representable */
#define DIB_RAW_REHASH   ((dib_raw_t)0xfeU)   /* entry yet to be rehashed during in-place resize */
#define DIB_RAW_FREE     ((dib_raw_t)0xffU)   /* a free bucket */
#define DIB_RAW_INIT     ((वर्ण)DIB_RAW_FREE) /* a byte to memset a DIB store with when initializing */

#define DIB_FREE अच_पूर्णांक_उच्च

#यदि ENABLE_DEBUG_HASHMAP
संरचना hashmap_debug_info {
        LIST_FIELDS(संरचना hashmap_debug_info, debug_list);
        अचिन्हित max_entries;  /* high watermark of n_entries */

        /* fields to detect modification while iterating */
        अचिन्हित put_गन्ती;    /* counts puts into the hashmap */
        अचिन्हित rem_गन्ती;    /* counts removals from hashmap */
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
        uint8_t  hash_साँचो[HASH_साँचो_SIZE];  /* hash key; changes during resize */

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
        uint8_t storage[परिमाण(संरचना indirect_storage)];
};

#define DIRECT_BUCKETS(entry_t) \
        (परिमाण(संरचना direct_storage) / (परिमाण(entry_t) + परिमाण(dib_raw_t)))

/* We should be able to store at least one entry directly. */
assert_cc(DIRECT_BUCKETS(संरचना ordered_hashmap_entry) >= 1);

/* We have 3 bits for n_direct_entries. */
assert_cc(DIRECT_BUCKETS(संरचना set_entry) < (1 << 3));

/* Hashmaps with directly stored entries all use this shared hash key.
 * It's no big deal if the key is guessed, because there can be only
 * a handful of directly stored entries in a hashmap. When a hashmap
 * outgrows direct storage, it gets its own key for indirect storage. */
स्थिर uint8_t shared_hash_साँचो[HASH_साँचो_SIZE];

/* Fields that all hashmap/set types must have */
संरचना HashmapBase {
        अचर संरचना hash_ops *hash_ops;  /* hash and compare ops to use */

        सङ्घ _packed_ {
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

#यदि ENABLE_DEBUG_HASHMAP
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

प्रकारनिर्देश संरचना CacheMem {
        अचर शून्य **ptr;
        माप_प्रकार n_populated;
        bool active:1;
} CacheMem;

संरचना IteratedCache {
        HashmapBase *hashmap;
        CacheMem साँचोहरू, values;
};

DEFINE_MEMPOOL(hashmap_pool,         Hashmap,        8);
DEFINE_MEMPOOL(ordered_hashmap_pool, OrderedHashmap, 8);
/* No need for a separate Set pool */
assert_cc(परिमाण(Hashmap) == परिमाण(Set));

संरचना hashmap_type_info {
        माप_प्रकार head_size;
        माप_प्रकार entry_size;
        संरचना mempool *mempool;
        अचिन्हित n_direct_buckets;
};

स्थिर _used_ अचर संरचना hashmap_type_info hashmap_type_info[_HASHMAP_TYPE_MAX] = {
        [HASHMAP_TYPE_PLAIN] = {
                .head_size        = परिमाण(Hashmap),
                .entry_size       = परिमाण(संरचना plain_hashmap_entry),
                .mempool          = &hashmap_pool,
                .n_direct_buckets = DIRECT_BUCKETS(संरचना plain_hashmap_entry),
        },
        [HASHMAP_TYPE_ORDERED] = {
                .head_size        = परिमाण(OrderedHashmap),
                .entry_size       = परिमाण(संरचना ordered_hashmap_entry),
                .mempool          = &ordered_hashmap_pool,
                .n_direct_buckets = DIRECT_BUCKETS(संरचना ordered_hashmap_entry),
        },
        [HASHMAP_TYPE_SET] = {
                .head_size        = परिमाण(Set),
                .entry_size       = परिमाण(संरचना set_entry),
                .mempool          = &hashmap_pool,
                .n_direct_buckets = DIRECT_BUCKETS(संरचना set_entry),
        },
};

शून्य hashmap_trim_pools(शून्य) {
        पूर्णाङ्क r;

        /* The pool is only allocated by the main thread, but the memory can be passed to other
         * threads. Let's clean up if we are the main thread and no other threads are live. */

        /* We build our own is_main_thread() here, which doesn't use C11 TLS based caching of the
         * result. That's because valgrind apparently doesn't like TLS to be used from a GCC destructor. */
        यदि (getpid() != gettid())
                फर्काऊ (शून्य) log_debug("Not cleaning up memory pools, not in main thread.");

        r = get_process_threads(0);
        यदि (r < 0)
                फर्काऊ (शून्य) log_debug_errno(r, "Failed to determine number of threads, not cleaning up memory pools: %m");
        यदि (r != 1)
                फर्काऊ (शून्य) log_debug("Not cleaning up memory pools, running in multi-threaded process.");

        mempool_trim(&hashmap_pool);
        mempool_trim(&ordered_hashmap_pool);
}

#यदि HAVE_VALGRIND_VALGRIND_H
_destructor_ स्थिर शून्य cleanup_pools(शून्य) {
        /* Be nice to valgrind */
        यदि (RUNNING_ON_VALGRIND)
                hashmap_trim_pools();
}
#endif

स्थिर अन्तःस्थ अचिन्हित n_buckets(HashmapBase *h) {
        प्रतिज्ञा(h);
        फर्काऊ h->has_indirect ? h->indirect.n_buckets
                               : hashmap_type_info[h->type].n_direct_buckets;
}

स्थिर अचिन्हित n_entries(HashmapBase *h) {
        प्रतिज्ञा(h);
        फर्काऊ h->has_indirect ? h->indirect.n_entries
                               : h->n_direct_entries;
}

स्थिर शून्य n_entries_inc(HashmapBase *h) {
        प्रतिज्ञा(h);

        यदि (h->has_indirect)
                h->indirect.n_entries++;
        अन्यथा
                h->n_direct_entries++;
}

स्थिर शून्य n_entries_dec(HashmapBase *h) {
        प्रतिज्ञा(h);

        यदि (h->has_indirect)
                h->indirect.n_entries--;
        अन्यथा
                h->n_direct_entries--;
}

स्थिर अन्तःस्थ शून्य* storage_ptr(HashmapBase *h) {
        प्रतिज्ञा(h);
        फर्काऊ h->has_indirect ? h->indirect.storage
                               : h->direct.storage;
}

स्थिर अन्तःस्थ uint8_t* hash_साँचो(HashmapBase *h) {
        प्रतिज्ञा(h);
        फर्काऊ h->has_indirect ? h->indirect.hash_साँचो
                               : shared_hash_साँचो;
}

स्थिर अचिन्हित base_bucket_hash(HashmapBase *h, अचर शून्य *p) {
        संरचना siphash state;
        uint64_t hash;

        प्रतिज्ञा(h);

        /* The keys of trivial_hash_ops and uint64_hash_ops fit in 64 bits. siphash24_uint64() hashes such
         * a key without the indirect call through hash_ops and keeps the SipHash state in registers. On
         * 32-bit and big-endian architectures, the result differs from the result of trivial_hash_func(),
         * which hashes the key in native byte order. Only this function maps keys to buckets, so the
         * difference does not matter. */
        यदि (h->hash_ops->hash == trivial_hash_func)
                hash = siphash24_uint64((uintptr_t) p, hash_साँचो(h));
        अन्यथा यदि (h->hash_ops->hash == (hash_func_t) uint64_hash_func)
                hash = siphash24_uint64(*(अचर uint64_t*) p, hash_साँचो(h));
        अन्यथा {
                siphash24_init(&state, hash_साँचो(h));
                h->hash_ops->hash(p, &state);
                hash = siphash24_finalize(&state);
        }

        /* n_buckets is not a power of two, so a modulo needs a division. Multiplying the upper 32 bits of
         * the hash with n_buckets and keeping the upper 32 bits of the product maps the hash to
         * [0, n_buckets) almost as evenly, without a division. */
        फर्काऊ (अचिन्हित) (((hash >> 32) * n_buckets(h)) >> 32);
}
#define bucket_hash(h, p) base_bucket_hash(HASHMAP_BASE(h), p)

स्थिर शून्य base_set_dirty(HashmapBase *h) {
        प्रतिज्ञा(h);

        h->dirty = true;
}
#define hashmap_set_dirty(h) base_set_dirty(HASHMAP_BASE(h))

स्थिर शून्य get_hash_साँचो(uint8_t hash_साँचो[HASH_साँचो_SIZE], bool reuse_is_ok) {
        स्थिर uint8_t current[HASH_साँचो_SIZE];
        स्थिर bool current_initialized = false;

        /* Returns a hash function key to use. In order to keep things
         * fast we will not generate a new key each time we allocate a
         * new hash table. Instead, we'll just reuse the most recently
         * generated one, except if we never generated one or when we
         * are rehashing an entire hash table because we reached a
         * fill level */

        यदि (!current_initialized || !reuse_is_ok) {
                random_bytes(current, परिमाण(current));
                current_initialized = true;
        }

        स_नकल(hash_साँचो, current, परिमाण(current));
}

स्थिर अन्तःस्थ संरचना hashmap_base_entry* bucket_at(HashmapBase *h, अचिन्हित idx) {
        प्रतिज्ञा(h);
        फर्काऊ CAST_ALIGN_PTR(
                        संरचना hashmap_base_entry,
                        (uint8_t *) storage_ptr(h) + idx * hashmap_type_info[h->type].entry_size);
}

स्थिर संरचना plain_hashmap_entry* plain_bucket_at(Hashmap *h, अचिन्हित idx) {
        फर्काऊ (संरचना plain_hashmap_entry*) bucket_at(HASHMAP_BASE(h), idx);
}

स्थिर संरचना ordered_hashmap_entry* ordered_bucket_at(OrderedHashmap *h, अचिन्हित idx) {
        फर्काऊ (संरचना ordered_hashmap_entry*) bucket_at(HASHMAP_BASE(h), idx);
}

स्थिर संरचना set_entry *set_bucket_at(Set *h, अचिन्हित idx) {
        फर्काऊ (संरचना set_entry*) bucket_at(HASHMAP_BASE(h), idx);
}

स्थिर संरचना ordered_hashmap_entry* bucket_at_swap(संरचना swap_entries *swap, अचिन्हित idx) {
        प्रतिज्ञा(swap);
        फर्काऊ &swap->e[idx - _IDX_SWAP_BEGIN];
}

/* Returns a pointer to the bucket at index idx.
 * Understands real indexes and swap indexes, hence "_virtual". */
स्थिर संरचना hashmap_base_entry* bucket_at_virtual(HashmapBase *h, संरचना swap_entries *swap,
                                                    अचिन्हित idx) {
        यदि (idx < _IDX_SWAP_BEGIN)
                फर्काऊ bucket_at(h, idx);

        यदि (idx < _IDX_SWAP_END)
                फर्काऊ &bucket_at_swap(swap, idx)->p.b;

        assert_not_reached();
}

स्थिर अन्तःस्थ dib_raw_t* dib_raw_ptr(HashmapBase *h) {
        प्रतिज्ञा(h);
        फर्काऊ (dib_raw_t*)
                ((uint8_t*) storage_ptr(h) + hashmap_type_info[h->type].entry_size * n_buckets(h));
}

स्थिर अचिन्हित bucket_distance(HashmapBase *h, अचिन्हित idx, अचिन्हित from) {
        फर्काऊ idx >= from ? idx - from
                           : n_buckets(h) + idx - from;
}

स्थिर अचिन्हित bucket_calculate_dib(HashmapBase *h, अचिन्हित idx, dib_raw_t raw_dib) {
        अचिन्हित initial_bucket;

        यदि (raw_dib == DIB_RAW_FREE)
                फर्काऊ DIB_FREE;

        यदि (_likely_(raw_dib < DIB_RAW_OVERFLOW))
                फर्काऊ raw_dib;

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
        initial_bucket = bucket_hash(h, bucket_at(h, idx)->साँचो);
        फर्काऊ bucket_distance(h, idx, initial_bucket);
}

स्थिर शून्य bucket_set_dib(HashmapBase *h, अचिन्हित idx, अचिन्हित dib) {
        dib_raw_ptr(h)[idx] = dib != DIB_FREE ? MIN(dib, DIB_RAW_OVERFLOW) : DIB_RAW_FREE;
}

स्थिर अचिन्हित skip_free_buckets(HashmapBase *h, अचिन्हित idx) {
        dib_raw_t *dibs;

        dibs = dib_raw_ptr(h);

        लागि ( ; idx < n_buckets(h); idx++)
                यदि (dibs[idx] != DIB_RAW_FREE)
                        फर्काऊ idx;

        फर्काऊ IDX_NIL;
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

        यदि (h->type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*) h;
                संरचना ordered_hashmap_entry *le, *le_to;

                le_to = (संरचना ordered_hashmap_entry*) e_to;

                यदि (le_to->iterate_next != IDX_NIL) {
                        le = (संरचना ordered_hashmap_entry*)
                             bucket_at_virtual(h, swap, le_to->iterate_next);
                        le->iterate_previous = to;
                }

                यदि (le_to->iterate_previous != IDX_NIL) {
                        le = (संरचना ordered_hashmap_entry*)
                             bucket_at_virtual(h, swap, le_to->iterate_previous);
                        le->iterate_next = to;
                }

                यदि (lh->iterate_list_head == from)
                        lh->iterate_list_head = to;
                यदि (lh->iterate_list_tail == from)
                        lh->iterate_list_tail = to;
        }
}

स्थिर अन्तःस्थ अचिन्हित next_idx(HashmapBase *h, अचिन्हित idx) {
        फर्काऊ idx + 1U < n_buckets(h) ? idx + 1U : 0U;
}

स्थिर अचिन्हित prev_idx(HashmapBase *h, अचिन्हित idx) {
        फर्काऊ idx > 0U ? idx - 1U : n_buckets(h) - 1U;
}

स्थिर अन्तःस्थ शून्य* entry_मान(HashmapBase *h, संरचना hashmap_base_entry *e) {
        प्रतिज्ञा(h);
        प्रतिज्ञा(e);

        विकल्प (h->type) {

        पक्ष HASHMAP_TYPE_PLAIN:
        पक्ष HASHMAP_TYPE_ORDERED:
                फर्काऊ ((संरचना plain_hashmap_entry*)e)->मान;

        पक्ष HASHMAP_TYPE_SET:
                फर्काऊ (शून्य*) e->साँचो;

        स्वभाव:
                assert_not_reached();
        }
}

स्थिर शून्य base_remove_entry(HashmapBase *h, अचिन्हित idx) {
        अचिन्हित बायाँ, दायाँ, prev, dib;
        dib_raw_t raw_dib, *dibs;

        dibs = dib_raw_ptr(h);
        प्रतिज्ञा(dibs[idx] != DIB_RAW_FREE);

#यदि ENABLE_DEBUG_HASHMAP
        प्रतिज्ञा(h);
        h->debug.rem_गन्ती++;
        h->debug.last_rem_idx = idx;
#endif

        बायाँ = idx;
        /* Find the stop bucket ("right"). It is either free or has DIB == 0. */
        लागि (दायाँ = next_idx(h, बायाँ); ; दायाँ = next_idx(h, दायाँ)) {
                raw_dib = dibs[दायाँ];
                यदि (IN_SET(raw_dib, 0, DIB_RAW_FREE))
                        रोक;

                /* The buckets are not supposed to be all occupied and with DIB > 0.
                 * That would mean we could make everyone better off by shifting them
                 * backward. This scenario is impossible. */
                प्रतिज्ञा(बायाँ != दायाँ);
        }

        यदि (h->type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*) h;
                संरचना ordered_hashmap_entry *le = ordered_bucket_at(lh, idx);

                यदि (le->iterate_next != IDX_NIL)
                        ordered_bucket_at(lh, le->iterate_next)->iterate_previous = le->iterate_previous;
                अन्यथा
                        lh->iterate_list_tail = le->iterate_previous;

                यदि (le->iterate_previous != IDX_NIL)
                        ordered_bucket_at(lh, le->iterate_previous)->iterate_next = le->iterate_next;
                अन्यथा
                        lh->iterate_list_head = le->iterate_next;
        }

        /* Now shift all buckets in the interval (left, right) one step backwards */
        लागि (prev = बायाँ, बायाँ = next_idx(h, बायाँ); बायाँ != दायाँ;
             prev = बायाँ, बायाँ = next_idx(h, बायाँ)) {
                dib = bucket_calculate_dib(h, बायाँ, dibs[बायाँ]);
                प्रतिज्ञा(dib != 0);
                bucket_move_entry(h, रिक्त, बायाँ, prev);
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

        यदि (i->idx == IDX_NIL)
                गच्छ at_end;

        यदि (i->idx == IDX_FIRST && h->iterate_list_head == IDX_NIL)
                गच्छ at_end;

        यदि (i->idx == IDX_FIRST) {
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
                यदि (e->p.b.साँचो != i->next_साँचो) {
                        idx = prev_idx(HASHMAP_BASE(h), idx);
                        e = ordered_bucket_at(h, idx);
                }
                प्रतिज्ञा(e->p.b.साँचो == i->next_साँचो);
        }

#यदि ENABLE_DEBUG_HASHMAP
        i->prev_idx = idx;
#endif

        यदि (e->iterate_next != IDX_NIL) {
                संरचना ordered_hashmap_entry *n;
                i->idx = e->iterate_next;
                n = ordered_bucket_at(h, i->idx);
                i->next_साँचो = n->p.b.साँचो;
        } अन्यथा
                i->idx = IDX_NIL;

        फर्काऊ idx;

at_end:
        i->idx = IDX_NIL;
        फर्काऊ IDX_NIL;
}

स्थिर अचिन्हित hashmap_iterate_in_internal_order(HashmapBase *h, Iterator *i) {
        अचिन्हित idx;

        प्रतिज्ञा(h);
        प्रतिज्ञा(i);

        यदि (i->idx == IDX_NIL)
                गच्छ at_end;

        यदि (i->idx == IDX_FIRST) {
                /* fast forward to the first occupied bucket */
                यदि (h->has_indirect) {
                        i->idx = skip_free_buckets(h, h->indirect.idx_lowest_entry);
                        h->indirect.idx_lowest_entry = i->idx;
                } अन्यथा
                        i->idx = skip_free_buckets(h, 0);

                यदि (i->idx == IDX_NIL)
                        गच्छ at_end;
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
                यदि (e->साँचो != i->next_साँचो)
                        e = bucket_at(h, --i->idx);

                प्रतिज्ञा(e->साँचो == i->next_साँचो);
        }

        idx = i->idx;
#यदि ENABLE_DEBUG_HASHMAP
        i->prev_idx = idx;
#endif

        i->idx = skip_free_buckets(h, i->idx + 1);
        यदि (i->idx != IDX_NIL)
                i->next_साँचो = bucket_at(h, i->idx)->साँचो;
        अन्यथा
                i->idx = IDX_NIL;

        फर्काऊ idx;

at_end:
        i->idx = IDX_NIL;
        फर्काऊ IDX_NIL;
}

स्थिर अचिन्हित hashmap_iterate_entry(HashmapBase *h, Iterator *i) {
        यदि (!h) {
                i->idx = IDX_NIL;
                फर्काऊ IDX_NIL;
        }

#यदि ENABLE_DEBUG_HASHMAP
        यदि (i->idx == IDX_FIRST) {
                i->put_गन्ती = h->debug.put_गन्ती;
                i->rem_गन्ती = h->debug.rem_गन्ती;
        } अन्यथा {
                /* While iterating, must not add any new entries */
                प्रतिज्ञा(i->put_गन्ती == h->debug.put_गन्ती);
                /* ... or remove entries other than the current one */
                प्रतिज्ञा(i->rem_गन्ती == h->debug.rem_गन्ती ||
                       (i->rem_गन्ती == h->debug.rem_गन्ती - 1 &&
                        i->prev_idx == h->debug.last_rem_idx));
                /* Reset our removals counter */
                i->rem_गन्ती = h->debug.rem_गन्ती;
        }
#endif

        फर्काऊ h->type == HASHMAP_TYPE_ORDERED ? hashmap_iterate_in_insertion_order((OrderedHashmap*) h, i)
                                               : hashmap_iterate_in_internal_order(h, i);
}

bool _hashmap_iterate(HashmapBase *h, Iterator *i, शून्य **मान, अचर शून्य **साँचो) {
        संरचना hashmap_base_entry *e;
        शून्य *data;
        अचिन्हित idx;

        idx = hashmap_iterate_entry(h, i);
        यदि (idx == IDX_NIL) {
                यदि (मान)
                        *मान = रिक्त;
                यदि (साँचो)
                        *साँचो = रिक्त;

                फर्काऊ false;
        }

        e = bucket_at(h, idx);
        data = entry_मान(h, e);
        यदि (मान)
                *मान = data;
        यदि (साँचो)
                *साँचो = e->साँचो;

        फर्काऊ true;
}

#define HASHMAP_FOREACH_IDX(idx, h, i) \
        लागि ((i) = ITERATOR_FIRST, (idx) = hashmap_iterate_entry((h), &(i)); \
             (idx != IDX_NIL); \
             (idx) = hashmap_iterate_entry((h), &(i)))

IteratedCache* _hashmap_iterated_cache_new(HashmapBase *h) {
        IteratedCache *cache;

        प्रतिज्ञा(h);
        प्रतिज्ञा(!h->cached);

        यदि (h->cached)
                फर्काऊ रिक्त;

        cache = new0(IteratedCache, 1);
        यदि (!cache)
                फर्काऊ रिक्त;

        cache->hashmap = h;
        h->cached = true;

        फर्काऊ cache;
}

स्थिर शून्य reset_direct_storage(HashmapBase *h) {
        अचर संरचना hashmap_type_info *hi = &hashmap_type_info[h->type];
        शून्य *p;

        प्रतिज्ञा(!h->has_indirect);

        p = mempset(h->direct.storage, 0, hi->entry_size * hi->n_direct_buckets);
        स_रखो(p, DIB_RAW_INIT, परिमाण(dib_raw_t) * hi->n_direct_buckets);
}

स्थिर शून्य shared_hash_साँचो_initialize(शून्य) {
        random_bytes(shared_hash_साँचो, परिमाण(shared_hash_साँचो));
}

स्थिर संरचना HashmapBase* hashmap_base_new(अचर संरचना hash_ops *hash_ops, गणना HashmapType type) {
        HashmapBase *h;
        अचर संरचना hashmap_type_info *hi = &hashmap_type_info[type];

        bool use_pool = mempool_enabled && mempool_enabled();  /* mempool_enabled is a weak symbol */

        h = use_pool ? mempool_alloc0_tile(hi->mempool) : malloc0(hi->head_size);
        यदि (!h)
                फर्काऊ रिक्त;

        h->type = type;
        h->from_pool = use_pool;
        h->hash_ops = hash_ops ?: &trivial_hash_ops;

        यदि (type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*)h;
                lh->iterate_list_head = lh->iterate_list_tail = IDX_NIL;
        }

        reset_direct_storage(h);

        स्थिर pthread_once_t once = PTHREAD_ONCE_INIT;
        assert_se(pthread_once(&once, shared_hash_साँचो_initialize) == 0);

#यदि ENABLE_DEBUG_HASHMAP
        assert_se(pthread_mutex_lock(&hashmap_debug_list_mutex) == 0);
        LIST_PREPEND(debug_list, hashmap_debug_list, &h->debug);
        assert_se(pthread_mutex_unlock(&hashmap_debug_list_mutex) == 0);
#endif

        फर्काऊ h;
}

Hashmap *hashmap_new(अचर संरचना hash_ops *hash_ops) {
        फर्काऊ (Hashmap*)        hashmap_base_new(hash_ops, HASHMAP_TYPE_PLAIN);
}

OrderedHashmap *ordered_hashmap_new(अचर संरचना hash_ops *hash_ops) {
        फर्काऊ (OrderedHashmap*) hashmap_base_new(hash_ops, HASHMAP_TYPE_ORDERED);
}

Set *set_new(अचर संरचना hash_ops *hash_ops) {
        फर्काऊ (Set*)            hashmap_base_new(hash_ops, HASHMAP_TYPE_SET);
}

स्थिर पूर्णाङ्क hashmap_base_ensure_allocated(HashmapBase **h, अचर संरचना hash_ops *hash_ops,
                                         गणना HashmapType type) {
        HashmapBase *q;

        प्रतिज्ञा(h);

        यदि (*h) {
                प्रतिज्ञा((*h)->hash_ops == (hash_ops ?: &trivial_hash_ops));
                फर्काऊ 0;
        }

        q = hashmap_base_new(hash_ops, type);
        यदि (!q)
                फर्काऊ -ENOMEM;

        *h = q;
        फर्काऊ 1;
}

पूर्णाङ्क hashmap_ensure_allocated(Hashmap **h, अचर संरचना hash_ops *hash_ops) {
        फर्काऊ hashmap_base_ensure_allocated((HashmapBase**)h, hash_ops, HASHMAP_TYPE_PLAIN);
}

पूर्णाङ्क ordered_hashmap_ensure_allocated(OrderedHashmap **h, अचर संरचना hash_ops *hash_ops) {
        फर्काऊ hashmap_base_ensure_allocated((HashmapBase**)h, hash_ops, HASHMAP_TYPE_ORDERED);
}

पूर्णाङ्क set_ensure_allocated(Set **s, अचर संरचना hash_ops *hash_ops) {
        फर्काऊ hashmap_base_ensure_allocated((HashmapBase**)s, hash_ops, HASHMAP_TYPE_SET);
}

पूर्णाङ्क hashmap_ensure_put(Hashmap **h, अचर संरचना hash_ops *hash_ops, अचर शून्य *साँचो, शून्य *मान) {
        पूर्णाङ्क r;

        प्रतिज्ञा(h);

        r = hashmap_ensure_allocated(h, hash_ops);
        यदि (r < 0)
                फर्काऊ r;

        फर्काऊ hashmap_put(*h, साँचो, मान);
}

पूर्णाङ्क ordered_hashmap_ensure_put(OrderedHashmap **h, अचर संरचना hash_ops *hash_ops, अचर शून्य *साँचो, शून्य *मान) {
        पूर्णाङ्क r;

        प्रतिज्ञा(h);

        r = ordered_hashmap_ensure_allocated(h, hash_ops);
        यदि (r < 0)
                फर्काऊ r;

        फर्काऊ ordered_hashmap_put(*h, साँचो, मान);
}

पूर्णाङ्क ordered_hashmap_ensure_replace(OrderedHashmap **h, अचर संरचना hash_ops *hash_ops, अचर शून्य *साँचो, शून्य *मान) {
        पूर्णाङ्क r;

        प्रतिज्ञा(h);

        r = ordered_hashmap_ensure_allocated(h, hash_ops);
        यदि (r < 0)
                फर्काऊ r;

        फर्काऊ ordered_hashmap_replace(*h, साँचो, मान);
}

पूर्णाङ्क hashmap_ensure_replace(Hashmap **h, अचर संरचना hash_ops *hash_ops, अचर शून्य *साँचो, शून्य *मान) {
        पूर्णाङ्क r;

        प्रतिज्ञा(h);

        r = hashmap_ensure_allocated(h, hash_ops);
        यदि (r < 0)
                फर्काऊ r;

        फर्काऊ hashmap_replace(*h, साँचो, मान);
}

स्थिर शून्य hashmap_free_no_clear(HashmapBase *h) {
        प्रतिज्ञा(!h->has_indirect);
        प्रतिज्ञा(h->n_direct_entries == 0);

#यदि ENABLE_DEBUG_HASHMAP
        assert_se(pthread_mutex_lock(&hashmap_debug_list_mutex) == 0);
        LIST_REMOVE(debug_list, hashmap_debug_list, &h->debug);
        assert_se(pthread_mutex_unlock(&hashmap_debug_list_mutex) == 0);
#endif

        यदि (h->from_pool) {
                /* Ensure that the object didn't get migrated between threads. */
                assert_se(is_main_thread());
                mempool_free_tile(hashmap_type_info[h->type].mempool, h);
        } अन्यथा
                मुक्त(h);
}

HashmapBase* _hashmap_free(HashmapBase *h) {
        यदि (h) {
                _hashmap_clear(h);
                hashmap_free_no_clear(h);
        }

        फर्काऊ रिक्त;
}

शून्य _hashmap_clear(HashmapBase *h) {
        यदि (!h)
                फर्काऊ;

        यदि (h->hash_ops->free_साँचो || h->hash_ops->free_मान) {

                /* If destructor calls are defined, let's destroy things defensively: let's take the item out of the
                 * hash table, and only then call the destructor functions. If these destructors then try to unregister
                 * themselves from our hash table a second time, the entry is already gone. */

                जबसम्म (_hashmap_size(h) > 0) {
                        शून्य *k = रिक्त;
                        शून्य *v;

                        v = hashmap_first_साँचो_and_मान(h, true, &k);

                        यदि (h->hash_ops->free_साँचो)
                                h->hash_ops->free_साँचो(k);

                        यदि (h->hash_ops->free_मान)
                                h->hash_ops->free_मान(v);
                }
        }

        यदि (h->has_indirect) {
                मुक्त(h->indirect.storage);
                h->has_indirect = false;
        }

        h->n_direct_entries = 0;
        reset_direct_storage(h);

        यदि (h->type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*) h;
                lh->iterate_list_head = lh->iterate_list_tail = IDX_NIL;
        }

        base_set_dirty(h);
}

स्थिर पूर्णाङ्क resize_buckets(HashmapBase *h, अचिन्हित entries_add);

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

#यदि ENABLE_DEBUG_HASHMAP
        प्रतिज्ञा(h);
        h->debug.put_गन्ती++;
#endif

        dibs = dib_raw_ptr(h);

        लागि (distance = 0; ; distance++) {
                raw_dib = dibs[idx];
                यदि (IN_SET(raw_dib, DIB_RAW_FREE, DIB_RAW_REHASH)) {
                        यदि (raw_dib == DIB_RAW_REHASH)
                                bucket_move_entry(h, swap, idx, IDX_TMP);

                        यदि (h->has_indirect && h->indirect.idx_lowest_entry > idx)
                                h->indirect.idx_lowest_entry = idx;

                        bucket_set_dib(h, idx, distance);
                        bucket_move_entry(h, swap, IDX_PUT, idx);
                        यदि (raw_dib == DIB_RAW_REHASH) {
                                bucket_move_entry(h, swap, IDX_TMP, IDX_PUT);
                                फर्काऊ true;
                        }

                        फर्काऊ false;
                }

                dib = bucket_calculate_dib(h, idx, raw_dib);

                यदि (dib < distance) {
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
स्थिर पूर्णाङ्क hashmap_base_put_boldly(HashmapBase *h, अचिन्हित idx,
                                   संरचना swap_entries *swap, bool may_resize) {
        संरचना ordered_hashmap_entry *new_entry;
        पूर्णाङ्क r;

        प्रतिज्ञा(idx < n_buckets(h));

        new_entry = bucket_at_swap(swap, IDX_PUT);

        यदि (may_resize) {
                r = resize_buckets(h, 1);
                यदि (r < 0)
                        फर्काऊ r;
                यदि (r > 0)
                        idx = bucket_hash(h, new_entry->p.b.साँचो);
        }
        प्रतिज्ञा(n_entries(h) < n_buckets(h));

        यदि (h->type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*) h;

                new_entry->iterate_next = IDX_NIL;
                new_entry->iterate_previous = lh->iterate_list_tail;

                यदि (lh->iterate_list_tail != IDX_NIL) {
                        संरचना ordered_hashmap_entry *old_tail;

                        old_tail = ordered_bucket_at(lh, lh->iterate_list_tail);
                        प्रतिज्ञा(old_tail->iterate_next == IDX_NIL);
                        old_tail->iterate_next = IDX_PUT;
                }

                lh->iterate_list_tail = IDX_PUT;
                यदि (lh->iterate_list_head == IDX_NIL)
                        lh->iterate_list_head = IDX_PUT;
        }

        assert_se(hashmap_put_robin_hood(h, idx, swap) == false);

        n_entries_inc(h);
#यदि ENABLE_DEBUG_HASHMAP
        h->debug.max_entries = MAX(h->debug.max_entries, n_entries(h));
#endif

        base_set_dirty(h);

        फर्काऊ 1;
}
#define hashmap_put_boldly(h, idx, swap, may_resize) \
        hashmap_base_put_boldly(HASHMAP_BASE(h), idx, swap, may_resize)

/*
 * Returns 0 if resize is not needed.
 *         1 if successfully resized.
 *         -ENOMEM on allocation failure.
 */
स्थिर पूर्णाङ्क resize_buckets(HashmapBase *h, अचिन्हित entries_add) {
        संरचना swap_entries swap;
        शून्य *new_storage;
        dib_raw_t *old_dibs, *new_dibs;
        अचर संरचना hashmap_type_info *hi;
        अचिन्हित idx, optimal_idx;
        अचिन्हित old_n_buckets, new_n_buckets, n_rehashed, new_n_entries;
        uint8_t new_shift;
        bool rehash_next;

        प्रतिज्ञा(h);

        hi = &hashmap_type_info[h->type];
        new_n_entries = n_entries(h) + entries_add;

        /* overflow? */
        यदि (_unlikely_(new_n_entries < entries_add))
                फर्काऊ -ENOMEM;

        /* For direct storage we allow 100% load, because it's tiny. */
        यदि (!h->has_indirect && new_n_entries <= hi->n_direct_buckets)
                फर्काऊ 0;

        /*
         * Load factor = n/m = 1 - (1/INV_KEEP_FREE).
         * From it follows: m = n + n/(INV_KEEP_FREE - 1)
         */
        new_n_buckets = new_n_entries + new_n_entries / (INV_KEEP_FREE - 1);
        /* overflow? */
        यदि (_unlikely_(new_n_buckets < new_n_entries))
                फर्काऊ -ENOMEM;

        old_n_buckets = n_buckets(h);

        यदि (_likely_(new_n_buckets <= old_n_buckets))
                फर्काऊ 0;

        यदि (_unlikely_(new_n_buckets > अच_पूर्णांक_उच्च / (hi->entry_size + परिमाण(dib_raw_t))))
                फर्काऊ -ENOMEM;

        new_shift = log2u_round_up(MAX(
                        new_n_buckets * (hi->entry_size + परिमाण(dib_raw_t)),
                        2 * परिमाण(संरचना direct_storage)));

        /* Realloc storage (buckets and DIB array). */
        new_storage = पुनः_स्मृति(h->has_indirect ? h->indirect.storage : रिक्त,
                              1U << new_shift);
        यदि (!new_storage)
                फर्काऊ -ENOMEM;

        /* Must upgrade direct to indirect storage. */
        यदि (!h->has_indirect) {
                स_नकल(new_storage, h->direct.storage,
                       old_n_buckets * (hi->entry_size + परिमाण(dib_raw_t)));
                h->indirect.n_entries = h->n_direct_entries;
                h->indirect.idx_lowest_entry = 0;
                h->n_direct_entries = 0;
        }

        /* Get a new hash key. If we've just upgraded to indirect storage,
         * allow reusing a previously generated key. It's still a different key
         * from the shared one that we used for direct storage. */
        get_hash_साँचो(h->indirect.hash_साँचो, !h->has_indirect);

        h->has_indirect = true;
        h->indirect.storage = new_storage;
        h->indirect.n_buckets = (1U << new_shift) /
                                (hi->entry_size + परिमाण(dib_raw_t));

        old_dibs = (dib_raw_t*)((uint8_t*) new_storage + hi->entry_size * old_n_buckets);
        new_dibs = dib_raw_ptr(h);

        /*
         * Move the DIB array to the new place, replacing valid DIB values with
         * DIB_RAW_REHASH to indicate all of the used buckets need rehashing.
         * Note: Overlap is not possible, because we have at least doubled the
         * number of buckets and dib_raw_t is smaller than any entry type.
         */
        लागि (idx = 0; idx < old_n_buckets; idx++) {
                प्रतिज्ञा(old_dibs[idx] != DIB_RAW_REHASH);
                new_dibs[idx] = old_dibs[idx] == DIB_RAW_FREE ? DIB_RAW_FREE
                                                              : DIB_RAW_REHASH;
        }

        /* Zero the area of newly added entries (including the old DIB area) */
        memzero(bucket_at(h, old_n_buckets),
               (n_buckets(h) - old_n_buckets) * hi->entry_size);

        /* The upper half of the new DIB array needs initialization */
        स_रखो(&new_dibs[old_n_buckets], DIB_RAW_INIT,
               (n_buckets(h) - old_n_buckets) * परिमाण(dib_raw_t));

        /* Rehash entries that need it */
        n_rehashed = 0;
        लागि (idx = 0; idx < old_n_buckets; idx++) {
                यदि (new_dibs[idx] != DIB_RAW_REHASH)
                        जारी;

                optimal_idx = bucket_hash(h, bucket_at(h, idx)->साँचो);

                /*
                 * Not much to do if by luck the entry hashes to its current
                 * location. Just set its DIB.
                 */
                यदि (optimal_idx == idx) {
                        new_dibs[idx] = 0;
                        n_rehashed++;
                        जारी;
                }

                new_dibs[idx] = DIB_RAW_FREE;
                bucket_move_entry(h, &swap, idx, IDX_PUT);
                /* bucket_move_entry does not clear the source */
                memzero(bucket_at(h, idx), hi->entry_size);

                गर {
                        /*
                         * Find the new bucket for the current entry. This may make
                         * another entry homeless and load it into IDX_PUT.
                         */
                        rehash_next = hashmap_put_robin_hood(h, optimal_idx, &swap);
                        n_rehashed++;

                        /* Did the current entry displace another one? */
                        यदि (rehash_next)
                                optimal_idx = bucket_hash(h, bucket_at_swap(&swap, IDX_PUT)->p.b.साँचो);
                } जबसम्म (rehash_next);
        }

        assert_se(n_rehashed == n_entries(h));

        फर्काऊ 1;
}

/*
 * Finds an entry with a matching key
 * Returns: index of the found entry, or IDX_NIL if not found.
 */
स्थिर अचिन्हित base_bucket_scan(HashmapBase *h, अचिन्हित idx, अचर शून्य *साँचो) {
        compare_func_t compare = h->hash_ops->compare;
        संरचना hashmap_base_entry *e;
        अचिन्हित dib, distance;
        dib_raw_t *dibs = dib_raw_ptr(h);

        प्रतिज्ञा(idx < n_buckets(h));

        लागि (distance = 0; ; distance++) {
                यदि (dibs[idx] == DIB_RAW_FREE)
                        फर्काऊ IDX_NIL;

                dib = bucket_calculate_dib(h, idx, dibs[idx]);

                यदि (dib < distance)
                        फर्काऊ IDX_NIL;
                यदि (dib == distance) {
                        e = bucket_at(h, idx);

                        /* Most maps compare keys by pointer. Do that here, without the indirect call. */
                        यदि (compare == trivial_compare_func ? e->साँचो == साँचो : compare(e->साँचो, साँचो) == 0)
                                फर्काऊ idx;
                }

                idx = next_idx(h, idx);
        }
}
#define bucket_scan(h, idx, साँचो) base_bucket_scan(HASHMAP_BASE(h), idx, साँचो)

पूर्णाङ्क hashmap_put(Hashmap *h, अचर शून्य *साँचो, शून्य *मान) {
        संरचना swap_entries swap;
        संरचना plain_hashmap_entry *e;
        अचिन्हित hash, idx;

        प्रतिज्ञा(h);

        hash = bucket_hash(h, साँचो);
        idx = bucket_scan(h, hash, साँचो);
        यदि (idx != IDX_NIL) {
                e = plain_bucket_at(h, idx);
                यदि (e->मान == मान)
                        फर्काऊ 0;
                फर्काऊ -EEXIST;
        }

        e = &bucket_at_swap(&swap, IDX_PUT)->p;
        e->b.साँचो = साँचो;
        e->मान = मान;
        फर्काऊ hashmap_put_boldly(h, hash, &swap, true);
}

पूर्णाङ्क set_put(Set *s, अचर शून्य *साँचो) {
        संरचना swap_entries swap;
        संरचना hashmap_base_entry *e;
        अचिन्हित hash, idx;

        प्रतिज्ञा(s);

        hash = bucket_hash(s, साँचो);
        idx = bucket_scan(s, hash, साँचो);
        यदि (idx != IDX_NIL)
                फर्काऊ 0;

        e = &bucket_at_swap(&swap, IDX_PUT)->p.b;
        e->साँचो = साँचो;
        फर्काऊ hashmap_put_boldly(s, hash, &swap, true);
}

पूर्णाङ्क set_ensure_put(Set **s, अचर संरचना hash_ops *hash_ops, अचर शून्य *साँचो) {
        पूर्णाङ्क r;

        प्रतिज्ञा(s);

        r = set_ensure_allocated(s, hash_ops);
        यदि (r < 0)
                फर्काऊ r;

        फर्काऊ set_put(*s, साँचो);
}

पूर्णाङ्क set_ensure_consume(Set **s, अचर संरचना hash_ops *hash_ops, शून्य *साँचो) {
        पूर्णाङ्क r;

        r = set_ensure_put(s, hash_ops, साँचो);
        यदि (r <= 0) {
                यदि (hash_ops && hash_ops->free_साँचो)
                        hash_ops->free_साँचो(साँचो);
                अन्यथा यदि (hash_ops && hash_ops->free_मान)
                        /* Sets store their element in the key slot but may carry a value destructor. */
                        hash_ops->free_मान(साँचो);
                अन्यथा
                        मुक्त(साँचो);
        }

        फर्काऊ r;
}

पूर्णाङ्क hashmap_replace(Hashmap *h, अचर शून्य *साँचो, शून्य *मान) {
        संरचना swap_entries swap;
        संरचना plain_hashmap_entry *e;
        अचिन्हित hash, idx;

        प्रतिज्ञा(h);

        hash = bucket_hash(h, साँचो);
        idx = bucket_scan(h, hash, साँचो);
        यदि (idx != IDX_NIL) {
                e = plain_bucket_at(h, idx);
#यदि ENABLE_DEBUG_HASHMAP
                /* Although the key is equal, the key pointer may have changed,
                 * and this would break our assumption for iterating. So count
                 * this operation as incompatible with iteration. */
                यदि (e->b.साँचो != साँचो) {
                        h->b.debug.put_गन्ती++;
                        h->b.debug.rem_गन्ती++;
                        h->b.debug.last_rem_idx = idx;
                }
#endif
                e->b.साँचो = साँचो;
                e->मान = मान;
                hashmap_set_dirty(h);

                फर्काऊ 0;
        }

        e = &bucket_at_swap(&swap, IDX_PUT)->p;
        e->b.साँचो = साँचो;
        e->मान = मान;
        फर्काऊ hashmap_put_boldly(h, hash, &swap, true);
}

पूर्णाङ्क hashmap_update(Hashmap *h, अचर शून्य *साँचो, शून्य *मान) {
        संरचना plain_hashmap_entry *e;
        अचिन्हित hash, idx;

        प्रतिज्ञा(h);

        hash = bucket_hash(h, साँचो);
        idx = bucket_scan(h, hash, साँचो);
        यदि (idx == IDX_NIL)
                फर्काऊ -ENOENT;

        e = plain_bucket_at(h, idx);
        e->मान = मान;
        hashmap_set_dirty(h);

        फर्काऊ 0;
}

शून्य* _hashmap_get(HashmapBase *h, अचर शून्य *साँचो) {
        संरचना hashmap_base_entry *e;
        अचिन्हित hash, idx;

        यदि (!h)
                फर्काऊ रिक्त;

        hash = bucket_hash(h, साँचो);
        idx = bucket_scan(h, hash, साँचो);
        यदि (idx == IDX_NIL)
                फर्काऊ रिक्त;

        e = bucket_at(h, idx);
        फर्काऊ entry_मान(h, e);
}

शून्य* hashmap_get2(Hashmap *h, अचर शून्य *साँचो, शून्य **ret) {
        संरचना plain_hashmap_entry *e;
        अचिन्हित hash, idx;

        यदि (!h)
                फर्काऊ रिक्त;

        hash = bucket_hash(h, साँचो);
        idx = bucket_scan(h, hash, साँचो);
        यदि (idx == IDX_NIL)
                फर्काऊ रिक्त;

        e = plain_bucket_at(h, idx);
        यदि (ret)
                *ret = (शून्य*) e->b.साँचो;

        फर्काऊ e->मान;
}

bool _hashmap_contains(HashmapBase *h, अचर शून्य *साँचो) {
        अचिन्हित hash;

        यदि (!h)
                फर्काऊ false;

        hash = bucket_hash(h, साँचो);
        फर्काऊ bucket_scan(h, hash, साँचो) != IDX_NIL;
}

शून्य* _hashmap_remove(HashmapBase *h, अचर शून्य *साँचो) {
        संरचना hashmap_base_entry *e;
        अचिन्हित hash, idx;
        शून्य *data;

        यदि (!h)
                फर्काऊ रिक्त;

        hash = bucket_hash(h, साँचो);
        idx = bucket_scan(h, hash, साँचो);
        यदि (idx == IDX_NIL)
                फर्काऊ रिक्त;

        e = bucket_at(h, idx);
        data = entry_मान(h, e);
        remove_entry(h, idx);

        फर्काऊ data;
}

शून्य* hashmap_remove2(Hashmap *h, अचर शून्य *साँचो, शून्य **ret) {
        संरचना plain_hashmap_entry *e;
        अचिन्हित hash, idx;
        शून्य *data;

        यदि (!h) {
                यदि (ret)
                        *ret = रिक्त;
                फर्काऊ रिक्त;
        }

        hash = bucket_hash(h, साँचो);
        idx = bucket_scan(h, hash, साँचो);
        यदि (idx == IDX_NIL) {
                यदि (ret)
                        *ret = रिक्त;
                फर्काऊ रिक्त;
        }

        e = plain_bucket_at(h, idx);
        data = e->मान;
        यदि (ret)
                *ret = (शून्य*) e->b.साँचो;

        remove_entry(h, idx);

        फर्काऊ data;
}

पूर्णाङ्क hashmap_remove_and_put(Hashmap *h, अचर शून्य *old_साँचो, अचर शून्य *new_साँचो, शून्य *मान) {
        संरचना swap_entries swap;
        संरचना plain_hashmap_entry *e;
        अचिन्हित old_hash, new_hash, idx;

        यदि (!h)
                फर्काऊ -ENOENT;

        old_hash = bucket_hash(h, old_साँचो);
        idx = bucket_scan(h, old_hash, old_साँचो);
        यदि (idx == IDX_NIL)
                फर्काऊ -ENOENT;

        new_hash = bucket_hash(h, new_साँचो);
        यदि (bucket_scan(h, new_hash, new_साँचो) != IDX_NIL)
                फर्काऊ -EEXIST;

        remove_entry(h, idx);

        e = &bucket_at_swap(&swap, IDX_PUT)->p;
        e->b.साँचो = new_साँचो;
        e->मान = मान;
        assert_se(hashmap_put_boldly(h, new_hash, &swap, false) == 1);

        फर्काऊ 0;
}

पूर्णाङ्क set_remove_and_put(Set *s, अचर शून्य *old_साँचो, अचर शून्य *new_साँचो) {
        संरचना swap_entries swap;
        संरचना hashmap_base_entry *e;
        अचिन्हित old_hash, new_hash, idx;

        यदि (!s)
                फर्काऊ -ENOENT;

        old_hash = bucket_hash(s, old_साँचो);
        idx = bucket_scan(s, old_hash, old_साँचो);
        यदि (idx == IDX_NIL)
                फर्काऊ -ENOENT;

        new_hash = bucket_hash(s, new_साँचो);
        यदि (bucket_scan(s, new_hash, new_साँचो) != IDX_NIL)
                फर्काऊ -EEXIST;

        remove_entry(s, idx);

        e = &bucket_at_swap(&swap, IDX_PUT)->p.b;
        e->साँचो = new_साँचो;
        assert_se(hashmap_put_boldly(s, new_hash, &swap, false) == 1);

        फर्काऊ 0;
}

पूर्णाङ्क hashmap_remove_and_replace(Hashmap *h, अचर शून्य *old_साँचो, अचर शून्य *new_साँचो, शून्य *मान) {
        संरचना swap_entries swap;
        संरचना plain_hashmap_entry *e;
        अचिन्हित old_hash, new_hash, idx_old, idx_new;

        यदि (!h)
                फर्काऊ -ENOENT;

        old_hash = bucket_hash(h, old_साँचो);
        idx_old = bucket_scan(h, old_hash, old_साँचो);
        यदि (idx_old == IDX_NIL)
                फर्काऊ -ENOENT;

        old_साँचो = bucket_at(HASHMAP_BASE(h), idx_old)->साँचो;

        new_hash = bucket_hash(h, new_साँचो);
        idx_new = bucket_scan(h, new_hash, new_साँचो);
        यदि (idx_new != IDX_NIL)
                यदि (idx_old != idx_new) {
                        remove_entry(h, idx_new);
                        /* Compensate for a possible backward shift. */
                        यदि (old_साँचो != bucket_at(HASHMAP_BASE(h), idx_old)->साँचो)
                                idx_old = prev_idx(HASHMAP_BASE(h), idx_old);
                        प्रतिज्ञा(old_साँचो == bucket_at(HASHMAP_BASE(h), idx_old)->साँचो);
                }

        remove_entry(h, idx_old);

        e = &bucket_at_swap(&swap, IDX_PUT)->p;
        e->b.साँचो = new_साँचो;
        e->मान = मान;
        assert_se(hashmap_put_boldly(h, new_hash, &swap, false) == 1);

        फर्काऊ 0;
}

शून्य* hashmap_remove_मान(HashmapBase *h, अचर शून्य *साँचो, शून्य *मान) {
        संरचना hashmap_base_entry *e;
        अचिन्हित hash, idx;

        यदि (!h)
                फर्काऊ रिक्त;

        hash = bucket_hash(h, साँचो);
        idx = bucket_scan(h, hash, साँचो);
        यदि (idx == IDX_NIL)
                फर्काऊ रिक्त;

        e = bucket_at(h, idx);
        यदि (entry_मान(h, e) != मान)
                फर्काऊ रिक्त;

        remove_entry(h, idx);

        फर्काऊ मान;
}

स्थिर अचिन्हित find_first_entry(HashmapBase *h) {
        Iterator i = ITERATOR_FIRST;

        यदि (!h || !n_entries(h))
                फर्काऊ IDX_NIL;

        फर्काऊ hashmap_iterate_entry(h, &i);
}

शून्य* hashmap_first_साँचो_and_मान(HashmapBase *h, bool हटाओ, शून्य **ret_साँचो) {
        संरचना hashmap_base_entry *e;
        शून्य *साँचो, *data;
        अचिन्हित idx;

        idx = find_first_entry(h);
        यदि (idx == IDX_NIL) {
                यदि (ret_साँचो)
                        *ret_साँचो = रिक्त;
                फर्काऊ रिक्त;
        }

        e = bucket_at(h, idx);
        साँचो = (शून्य*) e->साँचो;
        data = entry_मान(h, e);

        यदि (हटाओ)
                remove_entry(h, idx);

        यदि (ret_साँचो)
                *ret_साँचो = साँचो;

        फर्काऊ data;
}

अचिन्हित _hashmap_size(HashmapBase *h) {
        यदि (!h)
                फर्काऊ 0;

        फर्काऊ n_entries(h);
}

अचिन्हित _hashmap_buckets(HashmapBase *h) {
        यदि (!h)
                फर्काऊ 0;

        फर्काऊ n_buckets(h);
}

पूर्णाङ्क _hashmap_merge(Hashmap *h, Hashmap *other) {
        Iterator i;
        अचिन्हित idx;

        प्रतिज्ञा(h);

        HASHMAP_FOREACH_IDX(idx, HASHMAP_BASE(other), i) {
                संरचना plain_hashmap_entry *pe = plain_bucket_at(other, idx);
                पूर्णाङ्क r;

                r = hashmap_put(h, pe->b.साँचो, pe->मान);
                यदि (r < 0 && r != -EEXIST)
                        फर्काऊ r;
        }

        फर्काऊ 0;
}

पूर्णाङ्क set_merge(Set *s, Set *other) {
        Iterator i;
        अचिन्हित idx;

        प्रतिज्ञा(s);

        HASHMAP_FOREACH_IDX(idx, HASHMAP_BASE(other), i) {
                संरचना set_entry *se = set_bucket_at(other, idx);
                पूर्णाङ्क r;

                r = set_put(s, se->b.साँचो);
                यदि (r < 0)
                        फर्काऊ r;
        }

        फर्काऊ 0;
}

पूर्णाङ्क _hashmap_reserve(HashmapBase *h, अचिन्हित entries_add) {
        पूर्णाङ्क r;

        प्रतिज्ञा(h);

        r = resize_buckets(h, entries_add);
        यदि (r < 0)
                फर्काऊ r;

        फर्काऊ 0;
}

/*
 * The same as hashmap_merge(), but every new item from other is moved to h.
 * Keys already in h are skipped and stay in other.
 * Returns: 0 on success.
 *          -ENOMEM on alloc failure, in which case no move has been done.
 */
पूर्णाङ्क _hashmap_move(HashmapBase *h, HashmapBase *other) {
        संरचना swap_entries swap;
        संरचना hashmap_base_entry *e, *n;
        Iterator i;
        अचिन्हित idx;
        पूर्णाङ्क r;

        प्रतिज्ञा(h);

        यदि (!other)
                फर्काऊ 0;

        प्रतिज्ञा(other->type == h->type);

        /*
         * This reserves buckets for the worst case, where none of other's
         * entries are yet present in h. This is preferable to risking
         * an allocation failure in the middle of the moving and having to
         * rollback or return a partial result.
         */
        r = resize_buckets(h, n_entries(other));
        यदि (r < 0)
                फर्काऊ r;

        HASHMAP_FOREACH_IDX(idx, other, i) {
                अचिन्हित h_hash;

                e = bucket_at(other, idx);
                h_hash = bucket_hash(h, e->साँचो);
                यदि (bucket_scan(h, h_hash, e->साँचो) != IDX_NIL)
                        जारी;

                n = &bucket_at_swap(&swap, IDX_PUT)->p.b;
                n->साँचो = e->साँचो;
                यदि (h->type != HASHMAP_TYPE_SET)
                        ((संरचना plain_hashmap_entry*) n)->मान =
                                ((संरचना plain_hashmap_entry*) e)->मान;
                assert_se(hashmap_put_boldly(h, h_hash, &swap, false) == 1);

                remove_entry(other, idx);
        }

        फर्काऊ 0;
}

पूर्णाङ्क _hashmap_move_one(HashmapBase *h, HashmapBase *other, अचर शून्य *साँचो) {
        संरचना swap_entries swap;
        अचिन्हित h_hash, other_hash, idx;
        संरचना hashmap_base_entry *e, *n;
        पूर्णाङ्क r;

        प्रतिज्ञा(h);

        h_hash = bucket_hash(h, साँचो);
        यदि (bucket_scan(h, h_hash, साँचो) != IDX_NIL)
                फर्काऊ -EEXIST;

        यदि (!other)
                फर्काऊ -ENOENT;

        प्रतिज्ञा(other->type == h->type);

        other_hash = bucket_hash(other, साँचो);
        idx = bucket_scan(other, other_hash, साँचो);
        यदि (idx == IDX_NIL)
                फर्काऊ -ENOENT;

        e = bucket_at(other, idx);

        n = &bucket_at_swap(&swap, IDX_PUT)->p.b;
        n->साँचो = e->साँचो;
        यदि (h->type != HASHMAP_TYPE_SET)
                ((संरचना plain_hashmap_entry*) n)->मान =
                        ((संरचना plain_hashmap_entry*) e)->मान;
        r = hashmap_put_boldly(h, h_hash, &swap, true);
        यदि (r < 0)
                फर्काऊ r;

        remove_entry(other, idx);
        फर्काऊ 0;
}

HashmapBase* _hashmap_copy(HashmapBase *h) {
        HashmapBase *copy;
        पूर्णाङ्क r;

        प्रतिज्ञा(h);

        copy = hashmap_base_new(h->hash_ops, h->type);
        यदि (!copy)
                फर्काऊ रिक्त;

        विकल्प (h->type) {
        पक्ष HASHMAP_TYPE_PLAIN:
        पक्ष HASHMAP_TYPE_ORDERED:
                r = hashmap_merge((Hashmap*)copy, (Hashmap*)h);
                रोक;
        पक्ष HASHMAP_TYPE_SET:
                r = set_merge((Set*)copy, (Set*)h);
                रोक;
        स्वभाव:
                assert_not_reached();
        }

        यदि (r < 0)
                फर्काऊ _hashmap_free(copy);

        फर्काऊ copy;
}

वर्ण** _hashmap_get_strv(HashmapBase *h) {
        वर्ण **sv;
        Iterator i;
        अचिन्हित idx, n;

        यदि (!h)
                फर्काऊ new0(वर्ण*, 1);

        sv = new(वर्ण*, n_entries(h)+1);
        यदि (!sv)
                फर्काऊ रिक्त;

        n = 0;
        HASHMAP_FOREACH_IDX(idx, h, i)
                sv[n++] = entry_मान(h, bucket_at(h, idx));
        sv[n] = रिक्त;

        फर्काऊ sv;
}

वर्ण** set_to_strv(Set **s) {
        प्रतिज्ञा(s);

        /* This is similar to set_get_strv(), but invalidates the set on success. */

        वर्ण **v = new(वर्ण*, set_size(*s) + 1);
        यदि (!v)
                फर्काऊ रिक्त;

        लागि (वर्ण **p = v; (*p = set_steal_first(*s)); p++)
                ;

        प्रतिज्ञा(set_isempty(*s));
        *s = set_free(*s);
        फर्काऊ v;
}

शून्य* ordered_hashmap_next(OrderedHashmap *h, अचर शून्य *साँचो) {
        संरचना ordered_hashmap_entry *e;
        अचिन्हित hash, idx;

        यदि (!h)
                फर्काऊ रिक्त;

        hash = bucket_hash(h, साँचो);
        idx = bucket_scan(h, hash, साँचो);
        यदि (idx == IDX_NIL)
                फर्काऊ रिक्त;

        e = ordered_bucket_at(h, idx);
        यदि (e->iterate_next == IDX_NIL)
                फर्काऊ रिक्त;
        फर्काऊ ordered_bucket_at(h, e->iterate_next)->p.मान;
}

पूर्णाङ्क set_consume(Set *s, शून्य *मान) {
        पूर्णाङ्क r;

        प्रतिज्ञा(s);
        प्रतिज्ञा(मान);

        r = set_put(s, मान);
        यदि (r <= 0)
                मुक्त(मान);

        फर्काऊ r;
}

पूर्णाङ्क hashmap_put_strdup_full(Hashmap **h, अचर संरचना hash_ops *hash_ops, अचर वर्ण *k, अचर वर्ण *v) {
        पूर्णाङ्क r;

        प्रतिज्ञा(h);

        r = hashmap_ensure_allocated(h, hash_ops);
        यदि (r < 0)
                फर्काऊ r;

        _cleanup_free_ वर्ण *kdup = रिक्त, *vdup = रिक्त;

        kdup = strdup(k);
        यदि (!kdup)
                फर्काऊ -ENOMEM;

        यदि (v) {
                vdup = strdup(v);
                यदि (!vdup)
                        फर्काऊ -ENOMEM;
        }

        r = hashmap_put(*h, kdup, vdup);
        यदि (r < 0) {
                यदि (r == -EEXIST && streq_ptr(v, hashmap_get(*h, kdup)))
                        फर्काऊ 0;
                फर्काऊ r;
        }

        /* 0 with non-null vdup would mean vdup is already in the hashmap, which cannot be */
        प्रतिज्ञा(vdup == रिक्त || r > 0);
        यदि (r > 0)
                kdup = vdup = रिक्त;

        फर्काऊ r;
}

पूर्णाङ्क set_put_strndup_full(Set **s, अचर संरचना hash_ops *hash_ops, अचर वर्ण *p, माप_प्रकार n) {
        वर्ण *c;
        पूर्णाङ्क r;

        प्रतिज्ञा(s);
        प्रतिज्ञा(p);

        r = set_ensure_allocated(s, hash_ops);
        यदि (r < 0)
                फर्काऊ r;

        यदि (n == SIZE_MAX) {
                यदि (set_contains(*s, (वर्ण*) p))
                        फर्काऊ 0;

                c = strdup(p);
        } अन्यथा
                c = strndup(p, n);
        यदि (!c)
                फर्काऊ -ENOMEM;

        फर्काऊ set_consume(*s, c);
}

पूर्णाङ्क set_put_strdupv_full(Set **s, अचर संरचना hash_ops *hash_ops, वर्ण **l) {
        पूर्णाङ्क n = 0, r;

        प्रतिज्ञा(s);

        STRV_FOREACH(i, l) {
                r = set_put_strndup_full(s, hash_ops, *i, SIZE_MAX);
                यदि (r < 0)
                        फर्काऊ r;

                n += r;
        }

        फर्काऊ n;
}

पूर्णाङ्क set_put_strsplit(Set *s, अचर वर्ण *v, अचर वर्ण *separators, ExtractFlags flags) {
        अचर वर्ण *p = ASSERT_PTR(v);
        पूर्णाङ्क r;

        प्रतिज्ञा(s);

        लागि (;;) {
                वर्ण *word;

                r = extract_first_word(&p, &word, separators, flags);
                यदि (r <= 0)
                        फर्काऊ r;

                r = set_consume(s, word);
                यदि (r < 0)
                        फर्काऊ r;
        }
}

/* expand the cachemem if needed, return true if newly (re)activated. */
स्थिर पूर्णाङ्क cachemem_maintain(CacheMem *mem, माप_प्रकार size) {
        प्रतिज्ञा(mem);

        यदि (!GREEDY_REALLOC(mem->ptr, size)) {
                यदि (size > 0)
                        फर्काऊ -ENOMEM;
        }

        यदि (!mem->active) {
                mem->active = true;
                फर्काऊ true;
        }

        फर्काऊ false;
}

पूर्णाङ्क iterated_cache_get(IteratedCache *cache, अचर शून्य ***res_साँचोहरू, अचर शून्य ***res_मानहरू, अचिन्हित *res_n_entries) {
        bool sync_साँचोहरू = false, sync_मानहरू = false;
        माप_प्रकार size;
        पूर्णाङ्क r;

        प्रतिज्ञा(cache);
        प्रतिज्ञा(cache->hashmap);

        size = n_entries(cache->hashmap);

        यदि (res_साँचोहरू) {
                r = cachemem_maintain(&cache->साँचोहरू, size);
                यदि (r < 0)
                        फर्काऊ r;

                sync_साँचोहरू = r;
        } अन्यथा
                cache->साँचोहरू.active = false;

        यदि (res_मानहरू) {
                r = cachemem_maintain(&cache->values, size);
                यदि (r < 0)
                        फर्काऊ r;

                sync_मानहरू = r;
        } अन्यथा
                cache->values.active = false;

        यदि (cache->hashmap->dirty) {
                यदि (cache->साँचोहरू.active)
                        sync_साँचोहरू = true;
                यदि (cache->values.active)
                        sync_मानहरू = true;

                cache->hashmap->dirty = false;
        }

        यदि (sync_साँचोहरू || sync_मानहरू) {
                अचिन्हित i, idx;
                Iterator iter;

                i = 0;
                HASHMAP_FOREACH_IDX(idx, cache->hashmap, iter) {
                        संरचना hashmap_base_entry *e;

                        e = bucket_at(cache->hashmap, idx);

                        यदि (sync_साँचोहरू)
                                cache->साँचोहरू.ptr[i] = e->साँचो;
                        यदि (sync_मानहरू)
                                cache->values.ptr[i] = entry_मान(cache->hashmap, e);
                        i++;
                }
        }

        यदि (res_साँचोहरू)
                *res_साँचोहरू = cache->साँचोहरू.ptr;
        यदि (res_मानहरू)
                *res_मानहरू = cache->values.ptr;
        यदि (res_n_entries)
                *res_n_entries = size;

        फर्काऊ 0;
}

IteratedCache* iterated_cache_free(IteratedCache *cache) {
        यदि (cache) {
                मुक्त(cache->साँचोहरू.ptr);
                मुक्त(cache->values.ptr);
        }

        फर्काऊ mfree(cache);
}

पूर्णाङ्क set_strjoin(Set *s, अचर वर्ण *separator, bool wrap_with_separator, वर्ण **ret) {
        _cleanup_free_ वर्ण *str = रिक्त;
        माप_प्रकार separator_len, len = 0;
        अचर वर्ण *मान;
        bool first;

        प्रतिज्ञा(ret);

        यदि (set_isempty(s)) {
                *ret = रिक्त;
                फर्काऊ 0;
        }

        separator_len = strlen_ptr(separator);

        यदि (separator_len == 0)
                wrap_with_separator = false;

        first = !wrap_with_separator;

        SET_FOREACH(मान, s) {
                माप_प्रकार l = strlen_ptr(मान);

                यदि (l == 0)
                        जारी;

                यदि (!GREEDY_REALLOC(str, len + l + (first ? 0 : separator_len) + (wrap_with_separator ? separator_len : 0) + 1))
                        फर्काऊ -ENOMEM;

                यदि (separator_len > 0 && !first) {
                        स_नकल(str + len, separator, separator_len);
                        len += separator_len;
                }

                स_नकल(str + len, मान, l);
                len += l;
                first = false;
        }

        यदि (wrap_with_separator) {
                स_नकल(str + len, separator, separator_len);
                len += separator_len;
        }

        str[len] = '\0';

        *ret = TAKE_PTR(str);
        फर्काऊ 0;
}

bool set_equal(Set *a, Set *b) {
        शून्य *p;

        /* Checks whether each entry of 'a' is also in 'b' and vice versa, i.e. the two sets contain the same
         * entries */

        यदि (a == b)
                फर्काऊ true;

        यदि (set_isempty(a) && set_isempty(b))
                फर्काऊ true;

        यदि (set_size(a) != set_size(b)) /* Cheap check that hopefully catches a lot of inequality cases
                                         * already */
                फर्काऊ false;

        SET_FOREACH(p, a)
                यदि (!set_contains(b, p))
                        फर्काऊ false;

        /* If we have the same hashops, then we don't need to check things backwards given we compared the
         * size and that all of a is in b. */
        यदि (a->b.hash_ops == b->b.hash_ops)
                फर्काऊ true;

        SET_FOREACH(p, b)
                यदि (!set_contains(a, p))
                        फर्काऊ false;

        फर्काऊ true;
}

स्थिर bool set_fnmatch_one(Set *patterns, अचर वर्ण *needle) {
        अचर वर्ण *p;

        प्रतिज्ञा(needle);

        /* Any failure of fnmatch() is treated as equivalent to FNM_NOMATCH, i.e. as non-matching pattern */

        SET_FOREACH(p, patterns)
                यदि (fnmatch(p, needle, 0) == 0)
                        फर्काऊ true;

        फर्काऊ false;
}

bool set_fnmatch(Set *include_patterns, Set *exclude_patterns, अचर वर्ण *needle) {
        प्रतिज्ञा(needle);

        यदि (set_fnmatch_one(exclude_patterns, needle))
                फर्काऊ false;

        यदि (set_isempty(include_patterns))
                फर्काऊ true;

        फर्काऊ set_fnmatch_one(include_patterns, needle);
}

स्थिर पूर्णाङ्क hashmap_entry_compare(
                संरचना hashmap_base_entry * अचर *a,
                संरचना hashmap_base_entry * अचर *b,
                compare_func_t compare) {

        प्रतिज्ञा(a && *a);
        प्रतिज्ञा(b && *b);
        प्रतिज्ञा(compare);

        फर्काऊ compare((*a)->साँचो, (*b)->साँचो);
}

स्थिर पूर्णाङ्क _hashmap_dump_entries_sorted(
                HashmapBase *h,
                शून्य ***ret,
                माप_प्रकार *ret_n) {
        _cleanup_free_ शून्य **entries = रिक्त;
        Iterator iter;
        अचिन्हित idx;
        माप_प्रकार n = 0;

        प्रतिज्ञा(ret);
        प्रतिज्ञा(ret_n);

        यदि (_hashmap_size(h) == 0) {
                *ret = रिक्त;
                *ret_n = 0;
                फर्काऊ 0;
        }

        /* We append one more element than needed so that the resulting array can be used as a strv. We
         * don't count this entry in the returned size. */
        entries = new(शून्य*, _hashmap_size(h) + 1);
        यदि (!entries)
                फर्काऊ -ENOMEM;

        HASHMAP_FOREACH_IDX(idx, h, iter)
                entries[n++] = bucket_at(h, idx);

        प्रतिज्ञा(n == _hashmap_size(h));
        entries[n] = रिक्त;

        typesafe_qsort_r((संरचना hashmap_base_entry**) entries, n,
                         hashmap_entry_compare, h->hash_ops->compare);

        *ret = TAKE_PTR(entries);
        *ret_n = n;
        फर्काऊ 0;
}

पूर्णाङ्क hashmap_dump_साँचोहरू_sorted(HashmapBase *h, शून्य ***ret, माप_प्रकार *ret_n) {
        _cleanup_free_ शून्य **entries = रिक्त;
        माप_प्रकार n;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        r = _hashmap_dump_entries_sorted(h, &entries, &n);
        यदि (r < 0)
                फर्काऊ r;

        /* Reuse the array. */
        FOREACH_ARRAY(e, entries, n)
                *e = (शून्य*) (*(संरचना hashmap_base_entry**) e)->साँचो;

        *ret = TAKE_PTR(entries);
        यदि (ret_n)
                *ret_n = n;
        फर्काऊ 0;
}

पूर्णाङ्क _hashmap_dump_sorted(HashmapBase *h, शून्य ***ret, माप_प्रकार *ret_n) {
        _cleanup_free_ शून्य **entries = रिक्त;
        माप_प्रकार n;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        r = _hashmap_dump_entries_sorted(h, &entries, &n);
        यदि (r < 0)
                फर्काऊ r;

        /* Reuse the array. */
        FOREACH_ARRAY(e, entries, n)
                *e = entry_मान(h, *(संरचना hashmap_base_entry**) e);

        *ret = TAKE_PTR(entries);
        यदि (ret_n)
                *ret_n = n;
        फर्काऊ 0;
}
