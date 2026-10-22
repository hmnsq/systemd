/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <fnmatch.h>
#अन्तर्भाव <pthread.h>
#अन्तर्भाव <unistd.h>
#सचे HAVE_VALGRIND_VALGRIND_H
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

#सचे ENABLE_DEBUG_HASHMAP
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
        ध्रुव शून्य *कुञ्चिका;
};

/* Entry types for specific hashmap/set types
 * hashmap_base_entry must be at the beginning of each entry struct. */

संरचना plain_hashmap_entry {
        संरचना hashmap_base_entry b;
        शून्य *मानम्;
};

संरचना ordered_hashmap_entry {
        संरचना plain_hashmap_entry p;
        unsigned iterate_next, iterate_previous;
};

संरचना set_entry {
        संरचना hashmap_base_entry b;
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
संरचना swap_entries {
        संरचना ordered_hashmap_entry e[_IDX_SWAP_END - _IDX_SWAP_BEGIN];
};

/* Distance from Initial Bucket */
प्रकारनिर्देश uint8_t dib_raw_t;
#define DIB_RAW_OVERFLOW ((dib_raw_t)0xfdU)   /* indicates DIB value is greater than representable */
#define DIB_RAW_REHASH   ((dib_raw_t)0xfeU)   /* entry yet to be rehashed during in-place resize */
#define DIB_RAW_FREE     ((dib_raw_t)0xffU)   /* a free bucket */
#define DIB_RAW_INIT     ((वर्ण)DIB_RAW_FREE) /* a byte to memset a DIB store with when initializing */

#define DIB_FREE UINT_MAX

#सचे ENABLE_DEBUG_HASHMAP
संरचना hashmap_debug_info {
        LIST_FIELDS(संरचना hashmap_debug_info, debug_list);
        unsigned max_entries;  /* high watermark of n_entries */

        /* fields to detect modification while iterating */
        unsigned put_गणनम्;    /* counts puts into the hashmap */
        unsigned rem_गणनम्;    /* counts removals from hashmap */
        unsigned last_rem_idx; /* remembers last removal index */
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
        uint8_t  hash_कुञ्चिका[HASH_कुञ्चिका_SIZE];  /* hash key; changes during resize */

        unsigned n_entries;                /* number of stored entries */
        unsigned n_buckets;                /* number of buckets */

        unsigned idx_lowest_entry;         /* Index below which all buckets are free.
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
स्थिर uint8_t shared_hash_कुञ्चिका[HASH_कुञ्चिका_SIZE];

/* Fields that all hashmap/set types must have */
संरचना HashmapBase {
        ध्रुव संरचना hash_ops *hash_ops;  /* hash and compare ops to use */

        सङ्घ _packed_ {
                संरचना indirect_storage indirect; /* if  has_indirect */
                संरचना direct_storage direct;     /* if !has_indirect */
        };

        गणना HashmapType type:2;     /* HASHMAP_TYPE_* */
        bool has_indirect:1;         /* whether indirect storage is used */
        unsigned n_direct_entries:3; /* Number of entries in direct storage.
                                      * Only valid if !has_indirect. */
        bool from_pool:1;            /* whether was allocated from mempool */
        bool dirty:1;                /* whether dirtied since last iterated_cache_get() */
        bool cached:1;               /* whether this hashmap is being cached */

#सचे ENABLE_DEBUG_HASHMAP
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
        unsigned iterate_list_head, iterate_list_tail;
};

संरचना Set {
        संरचना HashmapBase b;
};

प्रकारनिर्देश संरचना CacheMem {
        ध्रुव शून्य **ptr;
        size_t n_populated;
        bool active:1;
} CacheMem;

संरचना IteratedCache {
        HashmapBase *hashmap;
        CacheMem कुञ्चिकाः, values;
};

DEFINE_MEMPOOL(hashmap_pool,         Hashmap,        8);
DEFINE_MEMPOOL(ordered_hashmap_pool, OrderedHashmap, 8);
/* No need for a separate Set pool */
assert_cc(परिमाण(Hashmap) == परिमाण(Set));

संरचना hashmap_type_info {
        size_t head_size;
        size_t entry_size;
        संरचना mempool *mempool;
        unsigned n_direct_buckets;
};

स्थिर _used_ ध्रुव संरचना hashmap_type_info hashmap_type_info[_HASHMAP_TYPE_MAX] = {
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
        सचे (getpid() != gettid())
                पटिनिवत्त (शून्य) log_debug("Not cleaning up memory pools, not in main thread.");

        r = get_process_threads(0);
        सचे (r < 0)
                पटिनिवत्त (शून्य) log_debug_errno(r, "Failed to determine number of threads, not cleaning up memory pools: %m");
        सचे (r != 1)
                पटिनिवत्त (शून्य) log_debug("Not cleaning up memory pools, running in multi-threaded process.");

        mempool_trim(&hashmap_pool);
        mempool_trim(&ordered_hashmap_pool);
}

#सचे HAVE_VALGRIND_VALGRIND_H
_destructor_ स्थिर शून्य cleanup_pools(शून्य) {
        /* Be nice to valgrind */
        सचे (RUNNING_ON_VALGRIND)
                hashmap_trim_pools();
}
#endif

स्थिर अन्तःस्थ unsigned n_buckets(HashmapBase *h) {
        प्रतिज्ञा(h);
        पटिनिवत्त h->has_indirect ? h->indirect.n_buckets
                               : hashmap_type_info[h->type].n_direct_buckets;
}

स्थिर unsigned n_entries(HashmapBase *h) {
        प्रतिज्ञा(h);
        पटिनिवत्त h->has_indirect ? h->indirect.n_entries
                               : h->n_direct_entries;
}

स्थिर शून्य n_entries_inc(HashmapBase *h) {
        प्रतिज्ञा(h);

        सचे (h->has_indirect)
                h->indirect.n_entries++;
        अञ्ञथा
                h->n_direct_entries++;
}

स्थिर शून्य n_entries_dec(HashmapBase *h) {
        प्रतिज्ञा(h);

        सचे (h->has_indirect)
                h->indirect.n_entries--;
        अञ्ञथा
                h->n_direct_entries--;
}

स्थिर अन्तःस्थ शून्य* storage_ptr(HashmapBase *h) {
        प्रतिज्ञा(h);
        पटिनिवत्त h->has_indirect ? h->indirect.storage
                               : h->direct.storage;
}

स्थिर अन्तःस्थ uint8_t* hash_कुञ्चिका(HashmapBase *h) {
        प्रतिज्ञा(h);
        पटिनिवत्त h->has_indirect ? h->indirect.hash_कुञ्चिका
                               : shared_hash_कुञ्चिका;
}

स्थिर unsigned base_bucket_hash(HashmapBase *h, ध्रुव शून्य *p) {
        संरचना siphash state;
        uint64_t hash;

        प्रतिज्ञा(h);

        /* The keys of trivial_hash_ops and uint64_hash_ops fit in 64 bits. siphash24_uint64() hashes such
         * a key without the indirect call through hash_ops and keeps the SipHash state in registers. On
         * 32-bit and big-endian architectures, the result differs from the result of trivial_hash_func(),
         * which hashes the key in native byte order. Only this function maps keys to buckets, so the
         * difference does not matter. */
        सचे (h->hash_ops->hash == trivial_hash_func)
                hash = siphash24_uint64((uintptr_t) p, hash_कुञ्चिका(h));
        अञ्ञथा सचे (h->hash_ops->hash == (hash_func_t) uint64_hash_func)
                hash = siphash24_uint64(*(ध्रुव uint64_t*) p, hash_कुञ्चिका(h));
        अञ्ञथा {
                siphash24_init(&state, hash_कुञ्चिका(h));
                h->hash_ops->hash(p, &state);
                hash = siphash24_finalize(&state);
        }

        /* n_buckets is not a power of two, so a modulo needs a division. Multiplying the upper 32 bits of
         * the hash with n_buckets and keeping the upper 32 bits of the product maps the hash to
         * [0, n_buckets) almost as evenly, without a division. */
        पटिनिवत्त (unsigned) (((hash >> 32) * n_buckets(h)) >> 32);
}
#define bucket_hash(h, p) base_bucket_hash(HASHMAP_BASE(h), p)

स्थिर शून्य base_set_dirty(HashmapBase *h) {
        प्रतिज्ञा(h);

        h->dirty = true;
}
#define hashmap_set_dirty(h) base_set_dirty(HASHMAP_BASE(h))

स्थिर शून्य get_hash_कुञ्चिका(uint8_t hash_कुञ्चिका[HASH_कुञ्चिका_SIZE], bool reuse_is_ok) {
        स्थिर uint8_t current[HASH_कुञ्चिका_SIZE];
        स्थिर bool current_initialized = false;

        /* Returns a hash function key to use. In order to keep things
         * fast we will not generate a new key each time we allocate a
         * new hash table. Instead, we'll just reuse the most recently
         * generated one, except if we never generated one or when we
         * are rehashing an entire hash table because we reached a
         * fill level */

        सचे (!current_initialized || !reuse_is_ok) {
                random_bytes(current, परिमाण(current));
                current_initialized = true;
        }

        memcpy(hash_कुञ्चिका, current, परिमाण(current));
}

स्थिर अन्तःस्थ संरचना hashmap_base_entry* bucket_at(HashmapBase *h, unsigned idx) {
        प्रतिज्ञा(h);
        पटिनिवत्त CAST_ALIGN_PTR(
                        संरचना hashmap_base_entry,
                        (uint8_t *) storage_ptr(h) + idx * hashmap_type_info[h->type].entry_size);
}

स्थिर संरचना plain_hashmap_entry* plain_bucket_at(Hashmap *h, unsigned idx) {
        पटिनिवत्त (संरचना plain_hashmap_entry*) bucket_at(HASHMAP_BASE(h), idx);
}

स्थिर संरचना ordered_hashmap_entry* ordered_bucket_at(OrderedHashmap *h, unsigned idx) {
        पटिनिवत्त (संरचना ordered_hashmap_entry*) bucket_at(HASHMAP_BASE(h), idx);
}

स्थिर संरचना set_entry *set_bucket_at(Set *h, unsigned idx) {
        पटिनिवत्त (संरचना set_entry*) bucket_at(HASHMAP_BASE(h), idx);
}

स्थिर संरचना ordered_hashmap_entry* bucket_at_swap(संरचना swap_entries *swap, unsigned idx) {
        प्रतिज्ञा(swap);
        पटिनिवत्त &swap->e[idx - _IDX_SWAP_BEGIN];
}

/* Returns a pointer to the bucket at index idx.
 * Understands real indexes and swap indexes, hence "_virtual". */
स्थिर संरचना hashmap_base_entry* bucket_at_virtual(HashmapBase *h, संरचना swap_entries *swap,
                                                    unsigned idx) {
        सचे (idx < _IDX_SWAP_BEGIN)
                पटिनिवत्त bucket_at(h, idx);

        सचे (idx < _IDX_SWAP_END)
                पटिनिवत्त &bucket_at_swap(swap, idx)->p.b;

        assert_not_reached();
}

स्थिर अन्तःस्थ dib_raw_t* dib_raw_ptr(HashmapBase *h) {
        प्रतिज्ञा(h);
        पटिनिवत्त (dib_raw_t*)
                ((uint8_t*) storage_ptr(h) + hashmap_type_info[h->type].entry_size * n_buckets(h));
}

स्थिर unsigned bucket_distance(HashmapBase *h, unsigned idx, unsigned from) {
        पटिनिवत्त idx >= from ? idx - from
                           : n_buckets(h) + idx - from;
}

स्थिर unsigned bucket_calculate_dib(HashmapBase *h, unsigned idx, dib_raw_t raw_dib) {
        unsigned initial_bucket;

        सचे (raw_dib == DIB_RAW_FREE)
                पटिनिवत्त DIB_FREE;

        सचे (_likely_(raw_dib < DIB_RAW_OVERFLOW))
                पटिनिवत्त raw_dib;

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
        initial_bucket = bucket_hash(h, bucket_at(h, idx)->कुञ्चिका);
        पटिनिवत्त bucket_distance(h, idx, initial_bucket);
}

स्थिर शून्य bucket_set_dib(HashmapBase *h, unsigned idx, unsigned dib) {
        dib_raw_ptr(h)[idx] = dib != DIB_FREE ? MIN(dib, DIB_RAW_OVERFLOW) : DIB_RAW_FREE;
}

स्थिर unsigned skip_free_buckets(HashmapBase *h, unsigned idx) {
        dib_raw_t *dibs;

        dibs = dib_raw_ptr(h);

        अत्थाय ( ; idx < n_buckets(h); idx++)
                सचे (dibs[idx] != DIB_RAW_FREE)
                        पटिनिवत्त idx;

        पटिनिवत्त IDX_NIL;
}

स्थिर शून्य bucket_mark_free(HashmapBase *h, unsigned idx) {
        प्रतिज्ञा(h);

        memzero(bucket_at(h, idx), hashmap_type_info[h->type].entry_size);
        bucket_set_dib(h, idx, DIB_FREE);
}

स्थिर शून्य bucket_move_entry(HashmapBase *h, संरचना swap_entries *swap,
                              unsigned from, unsigned to) {
        संरचना hashmap_base_entry *e_from, *e_to;

        प्रतिज्ञा(h);
        प्रतिज्ञा(from != to);

        e_from = bucket_at_virtual(h, swap, from);
        e_to   = bucket_at_virtual(h, swap, to);

        memcpy(e_to, e_from, hashmap_type_info[h->type].entry_size);

        सचे (h->type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*) h;
                संरचना ordered_hashmap_entry *le, *le_to;

                le_to = (संरचना ordered_hashmap_entry*) e_to;

                सचे (le_to->iterate_next != IDX_NIL) {
                        le = (संरचना ordered_hashmap_entry*)
                             bucket_at_virtual(h, swap, le_to->iterate_next);
                        le->iterate_previous = to;
                }

                सचे (le_to->iterate_previous != IDX_NIL) {
                        le = (संरचना ordered_hashmap_entry*)
                             bucket_at_virtual(h, swap, le_to->iterate_previous);
                        le->iterate_next = to;
                }

                सचे (lh->iterate_list_head == from)
                        lh->iterate_list_head = to;
                सचे (lh->iterate_list_tail == from)
                        lh->iterate_list_tail = to;
        }
}

स्थिर अन्तःस्थ unsigned next_idx(HashmapBase *h, unsigned idx) {
        पटिनिवत्त idx + 1U < n_buckets(h) ? idx + 1U : 0U;
}

स्थिर unsigned prev_idx(HashmapBase *h, unsigned idx) {
        पटिनिवत्त idx > 0U ? idx - 1U : n_buckets(h) - 1U;
}

स्थिर अन्तःस्थ शून्य* entry_मानम्(HashmapBase *h, संरचना hashmap_base_entry *e) {
        प्रतिज्ञा(h);
        प्रतिज्ञा(e);

        विकल्प (h->type) {

        पक्ष HASHMAP_TYPE_PLAIN:
        पक्ष HASHMAP_TYPE_ORDERED:
                पटिनिवत्त ((संरचना plain_hashmap_entry*)e)->मानम्;

        पक्ष HASHMAP_TYPE_SET:
                पटिनिवत्त (शून्य*) e->कुञ्चिका;

        स्वभाव:
                assert_not_reached();
        }
}

स्थिर शून्य base_remove_entry(HashmapBase *h, unsigned idx) {
        unsigned वामः, दक्षिणः, prev, dib;
        dib_raw_t raw_dib, *dibs;

        dibs = dib_raw_ptr(h);
        प्रतिज्ञा(dibs[idx] != DIB_RAW_FREE);

#सचे ENABLE_DEBUG_HASHMAP
        प्रतिज्ञा(h);
        h->debug.rem_गणनम्++;
        h->debug.last_rem_idx = idx;
#endif

        वामः = idx;
        /* Find the stop bucket ("right"). It is either free or has DIB == 0. */
        अत्थाय (दक्षिणः = next_idx(h, वामः); ; दक्षिणः = next_idx(h, दक्षिणः)) {
                raw_dib = dibs[दक्षिणः];
                सचे (IN_SET(raw_dib, 0, DIB_RAW_FREE))
                        भङ्ग;

                /* The buckets are not supposed to be all occupied and with DIB > 0.
                 * That would mean we could make everyone better off by shifting them
                 * backward. This scenario is impossible. */
                प्रतिज्ञा(वामः != दक्षिणः);
        }

        सचे (h->type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*) h;
                संरचना ordered_hashmap_entry *le = ordered_bucket_at(lh, idx);

                सचे (le->iterate_next != IDX_NIL)
                        ordered_bucket_at(lh, le->iterate_next)->iterate_previous = le->iterate_previous;
                अञ्ञथा
                        lh->iterate_list_tail = le->iterate_previous;

                सचे (le->iterate_previous != IDX_NIL)
                        ordered_bucket_at(lh, le->iterate_previous)->iterate_next = le->iterate_next;
                अञ्ञथा
                        lh->iterate_list_head = le->iterate_next;
        }

        /* Now shift all buckets in the interval (left, right) one step backwards */
        अत्थाय (prev = वामः, वामः = next_idx(h, वामः); वामः != दक्षिणः;
             prev = वामः, वामः = next_idx(h, वामः)) {
                dib = bucket_calculate_dib(h, वामः, dibs[वामः]);
                प्रतिज्ञा(dib != 0);
                bucket_move_entry(h, रिक्त, वामः, prev);
                bucket_set_dib(h, prev, dib - 1);
        }

        bucket_mark_free(h, prev);
        n_entries_dec(h);
        base_set_dirty(h);
}
#define remove_entry(h, idx) base_remove_entry(HASHMAP_BASE(h), idx)

स्थिर unsigned hashmap_iterate_in_insertion_order(OrderedHashmap *h, Iterator *i) {
        संरचना ordered_hashmap_entry *e;
        unsigned idx;

        प्रतिज्ञा(h);
        प्रतिज्ञा(i);

        सचे (i->idx == IDX_NIL)
                गच्छ at_end;

        सचे (i->idx == IDX_FIRST && h->iterate_list_head == IDX_NIL)
                गच्छ at_end;

        सचे (i->idx == IDX_FIRST) {
                idx = h->iterate_list_head;
                e = ordered_bucket_at(h, idx);
        } अञ्ञथा {
                idx = i->idx;
                e = ordered_bucket_at(h, idx);
                /*
                 * We allow removing the current entry while iterating, but removal may cause
                 * a backward shift. The next entry may thus move one bucket to the left.
                 * To detect when it happens, we remember the key pointer of the entry we were
                 * going to iterate next. If it does not match, there was a backward shift.
                 */
                सचे (e->p.b.कुञ्चिका != i->next_कुञ्चिका) {
                        idx = prev_idx(HASHMAP_BASE(h), idx);
                        e = ordered_bucket_at(h, idx);
                }
                प्रतिज्ञा(e->p.b.कुञ्चिका == i->next_कुञ्चिका);
        }

#सचे ENABLE_DEBUG_HASHMAP
        i->prev_idx = idx;
#endif

        सचे (e->iterate_next != IDX_NIL) {
                संरचना ordered_hashmap_entry *n;
                i->idx = e->iterate_next;
                n = ordered_bucket_at(h, i->idx);
                i->next_कुञ्चिका = n->p.b.कुञ्चिका;
        } अञ्ञथा
                i->idx = IDX_NIL;

        पटिनिवत्त idx;

at_end:
        i->idx = IDX_NIL;
        पटिनिवत्त IDX_NIL;
}

स्थिर unsigned hashmap_iterate_in_internal_order(HashmapBase *h, Iterator *i) {
        unsigned idx;

        प्रतिज्ञा(h);
        प्रतिज्ञा(i);

        सचे (i->idx == IDX_NIL)
                गच्छ at_end;

        सचे (i->idx == IDX_FIRST) {
                /* fast forward to the first occupied bucket */
                सचे (h->has_indirect) {
                        i->idx = skip_free_buckets(h, h->indirect.idx_lowest_entry);
                        h->indirect.idx_lowest_entry = i->idx;
                } अञ्ञथा
                        i->idx = skip_free_buckets(h, 0);

                सचे (i->idx == IDX_NIL)
                        गच्छ at_end;
        } अञ्ञथा {
                संरचना hashmap_base_entry *e;

                प्रतिज्ञा(i->idx > 0);

                e = bucket_at(h, i->idx);
                /*
                 * We allow removing the current entry while iterating, but removal may cause
                 * a backward shift. The next entry may thus move one bucket to the left.
                 * To detect when it happens, we remember the key pointer of the entry we were
                 * going to iterate next. If it does not match, there was a backward shift.
                 */
                सचे (e->कुञ्चिका != i->next_कुञ्चिका)
                        e = bucket_at(h, --i->idx);

                प्रतिज्ञा(e->कुञ्चिका == i->next_कुञ्चिका);
        }

        idx = i->idx;
#सचे ENABLE_DEBUG_HASHMAP
        i->prev_idx = idx;
#endif

        i->idx = skip_free_buckets(h, i->idx + 1);
        सचे (i->idx != IDX_NIL)
                i->next_कुञ्चिका = bucket_at(h, i->idx)->कुञ्चिका;
        अञ्ञथा
                i->idx = IDX_NIL;

        पटिनिवत्त idx;

at_end:
        i->idx = IDX_NIL;
        पटिनिवत्त IDX_NIL;
}

स्थिर unsigned hashmap_iterate_entry(HashmapBase *h, Iterator *i) {
        सचे (!h) {
                i->idx = IDX_NIL;
                पटिनिवत्त IDX_NIL;
        }

#सचे ENABLE_DEBUG_HASHMAP
        सचे (i->idx == IDX_FIRST) {
                i->put_गणनम् = h->debug.put_गणनम्;
                i->rem_गणनम् = h->debug.rem_गणनम्;
        } अञ्ञथा {
                /* While iterating, must not add any new entries */
                प्रतिज्ञा(i->put_गणनम् == h->debug.put_गणनम्);
                /* ... or remove entries other than the current one */
                प्रतिज्ञा(i->rem_गणनम् == h->debug.rem_गणनम् ||
                       (i->rem_गणनम् == h->debug.rem_गणनम् - 1 &&
                        i->prev_idx == h->debug.last_rem_idx));
                /* Reset our removals counter */
                i->rem_गणनम् = h->debug.rem_गणनम्;
        }
#endif

        पटिनिवत्त h->type == HASHMAP_TYPE_ORDERED ? hashmap_iterate_in_insertion_order((OrderedHashmap*) h, i)
                                               : hashmap_iterate_in_internal_order(h, i);
}

bool _hashmap_iterate(HashmapBase *h, Iterator *i, शून्य **मानम्, ध्रुव शून्य **कुञ्चिका) {
        संरचना hashmap_base_entry *e;
        शून्य *data;
        unsigned idx;

        idx = hashmap_iterate_entry(h, i);
        सचे (idx == IDX_NIL) {
                सचे (मानम्)
                        *मानम् = रिक्त;
                सचे (कुञ्चिका)
                        *कुञ्चिका = रिक्त;

                पटिनिवत्त false;
        }

        e = bucket_at(h, idx);
        data = entry_मानम्(h, e);
        सचे (मानम्)
                *मानम् = data;
        सचे (कुञ्चिका)
                *कुञ्चिका = e->कुञ्चिका;

        पटिनिवत्त true;
}

#define HASHMAP_FOREACH_IDX(idx, h, i) \
        अत्थाय ((i) = ITERATOR_FIRST, (idx) = hashmap_iterate_entry((h), &(i)); \
             (idx != IDX_NIL); \
             (idx) = hashmap_iterate_entry((h), &(i)))

IteratedCache* _hashmap_iterated_cache_new(HashmapBase *h) {
        IteratedCache *cache;

        प्रतिज्ञा(h);
        प्रतिज्ञा(!h->cached);

        सचे (h->cached)
                पटिनिवत्त रिक्त;

        cache = new0(IteratedCache, 1);
        सचे (!cache)
                पटिनिवत्त रिक्त;

        cache->hashmap = h;
        h->cached = true;

        पटिनिवत्त cache;
}

स्थिर शून्य reset_direct_storage(HashmapBase *h) {
        ध्रुव संरचना hashmap_type_info *hi = &hashmap_type_info[h->type];
        शून्य *p;

        प्रतिज्ञा(!h->has_indirect);

        p = mempset(h->direct.storage, 0, hi->entry_size * hi->n_direct_buckets);
        memset(p, DIB_RAW_INIT, परिमाण(dib_raw_t) * hi->n_direct_buckets);
}

स्थिर शून्य shared_hash_कुञ्चिका_initialize(शून्य) {
        random_bytes(shared_hash_कुञ्चिका, परिमाण(shared_hash_कुञ्चिका));
}

स्थिर संरचना HashmapBase* hashmap_base_new(ध्रुव संरचना hash_ops *hash_ops, गणना HashmapType type) {
        HashmapBase *h;
        ध्रुव संरचना hashmap_type_info *hi = &hashmap_type_info[type];

        bool use_pool = mempool_enabled && mempool_enabled();  /* mempool_enabled is a weak symbol */

        h = use_pool ? mempool_alloc0_tile(hi->mempool) : malloc0(hi->head_size);
        सचे (!h)
                पटिनिवत्त रिक्त;

        h->type = type;
        h->from_pool = use_pool;
        h->hash_ops = hash_ops ?: &trivial_hash_ops;

        सचे (type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*)h;
                lh->iterate_list_head = lh->iterate_list_tail = IDX_NIL;
        }

        reset_direct_storage(h);

        स्थिर pthread_once_t once = PTHREAD_ONCE_INIT;
        assert_se(pthread_once(&once, shared_hash_कुञ्चिका_initialize) == 0);

#सचे ENABLE_DEBUG_HASHMAP
        assert_se(pthread_mutex_lock(&hashmap_debug_list_mutex) == 0);
        LIST_PREPEND(debug_list, hashmap_debug_list, &h->debug);
        assert_se(pthread_mutex_unlock(&hashmap_debug_list_mutex) == 0);
#endif

        पटिनिवत्त h;
}

Hashmap *hashmap_new(ध्रुव संरचना hash_ops *hash_ops) {
        पटिनिवत्त (Hashmap*)        hashmap_base_new(hash_ops, HASHMAP_TYPE_PLAIN);
}

OrderedHashmap *ordered_hashmap_new(ध्रुव संरचना hash_ops *hash_ops) {
        पटिनिवत्त (OrderedHashmap*) hashmap_base_new(hash_ops, HASHMAP_TYPE_ORDERED);
}

Set *set_new(ध्रुव संरचना hash_ops *hash_ops) {
        पटिनिवत्त (Set*)            hashmap_base_new(hash_ops, HASHMAP_TYPE_SET);
}

स्थिर पूर्णाङ्क hashmap_base_ensure_allocated(HashmapBase **h, ध्रुव संरचना hash_ops *hash_ops,
                                         गणना HashmapType type) {
        HashmapBase *q;

        प्रतिज्ञा(h);

        सचे (*h) {
                प्रतिज्ञा((*h)->hash_ops == (hash_ops ?: &trivial_hash_ops));
                पटिनिवत्त 0;
        }

        q = hashmap_base_new(hash_ops, type);
        सचे (!q)
                पटिनिवत्त -ENOMEM;

        *h = q;
        पटिनिवत्त 1;
}

पूर्णाङ्क hashmap_ensure_allocated(Hashmap **h, ध्रुव संरचना hash_ops *hash_ops) {
        पटिनिवत्त hashmap_base_ensure_allocated((HashmapBase**)h, hash_ops, HASHMAP_TYPE_PLAIN);
}

पूर्णाङ्क ordered_hashmap_ensure_allocated(OrderedHashmap **h, ध्रुव संरचना hash_ops *hash_ops) {
        पटिनिवत्त hashmap_base_ensure_allocated((HashmapBase**)h, hash_ops, HASHMAP_TYPE_ORDERED);
}

पूर्णाङ्क set_ensure_allocated(Set **s, ध्रुव संरचना hash_ops *hash_ops) {
        पटिनिवत्त hashmap_base_ensure_allocated((HashmapBase**)s, hash_ops, HASHMAP_TYPE_SET);
}

पूर्णाङ्क hashmap_ensure_put(Hashmap **h, ध्रुव संरचना hash_ops *hash_ops, ध्रुव शून्य *कुञ्चिका, शून्य *मानम्) {
        पूर्णाङ्क r;

        प्रतिज्ञा(h);

        r = hashmap_ensure_allocated(h, hash_ops);
        सचे (r < 0)
                पटिनिवत्त r;

        पटिनिवत्त hashmap_put(*h, कुञ्चिका, मानम्);
}

पूर्णाङ्क ordered_hashmap_ensure_put(OrderedHashmap **h, ध्रुव संरचना hash_ops *hash_ops, ध्रुव शून्य *कुञ्चिका, शून्य *मानम्) {
        पूर्णाङ्क r;

        प्रतिज्ञा(h);

        r = ordered_hashmap_ensure_allocated(h, hash_ops);
        सचे (r < 0)
                पटिनिवत्त r;

        पटिनिवत्त ordered_hashmap_put(*h, कुञ्चिका, मानम्);
}

पूर्णाङ्क ordered_hashmap_ensure_replace(OrderedHashmap **h, ध्रुव संरचना hash_ops *hash_ops, ध्रुव शून्य *कुञ्चिका, शून्य *मानम्) {
        पूर्णाङ्क r;

        प्रतिज्ञा(h);

        r = ordered_hashmap_ensure_allocated(h, hash_ops);
        सचे (r < 0)
                पटिनिवत्त r;

        पटिनिवत्त ordered_hashmap_replace(*h, कुञ्चिका, मानम्);
}

पूर्णाङ्क hashmap_ensure_replace(Hashmap **h, ध्रुव संरचना hash_ops *hash_ops, ध्रुव शून्य *कुञ्चिका, शून्य *मानम्) {
        पूर्णाङ्क r;

        प्रतिज्ञा(h);

        r = hashmap_ensure_allocated(h, hash_ops);
        सचे (r < 0)
                पटिनिवत्त r;

        पटिनिवत्त hashmap_replace(*h, कुञ्चिका, मानम्);
}

स्थिर शून्य hashmap_free_no_clear(HashmapBase *h) {
        प्रतिज्ञा(!h->has_indirect);
        प्रतिज्ञा(h->n_direct_entries == 0);

#सचे ENABLE_DEBUG_HASHMAP
        assert_se(pthread_mutex_lock(&hashmap_debug_list_mutex) == 0);
        LIST_REMOVE(debug_list, hashmap_debug_list, &h->debug);
        assert_se(pthread_mutex_unlock(&hashmap_debug_list_mutex) == 0);
#endif

        सचे (h->from_pool) {
                /* Ensure that the object didn't get migrated between threads. */
                assert_se(is_main_thread());
                mempool_free_tile(hashmap_type_info[h->type].mempool, h);
        } अञ्ञथा
                मुक्त(h);
}

HashmapBase* _hashmap_free(HashmapBase *h) {
        सचे (h) {
                _hashmap_clear(h);
                hashmap_free_no_clear(h);
        }

        पटिनिवत्त रिक्त;
}

शून्य _hashmap_clear(HashmapBase *h) {
        सचे (!h)
                पटिनिवत्त;

        सचे (h->hash_ops->free_कुञ्चिका || h->hash_ops->free_मानम्) {

                /* If destructor calls are defined, let's destroy things defensively: let's take the item out of the
                 * hash table, and only then call the destructor functions. If these destructors then try to unregister
                 * themselves from our hash table a second time, the entry is already gone. */

                याव (_hashmap_size(h) > 0) {
                        शून्य *k = रिक्त;
                        शून्य *v;

                        v = hashmap_first_कुञ्चिका_and_मानम्(h, true, &k);

                        सचे (h->hash_ops->free_कुञ्चिका)
                                h->hash_ops->free_कुञ्चिका(k);

                        सचे (h->hash_ops->free_मानम्)
                                h->hash_ops->free_मानम्(v);
                }
        }

        सचे (h->has_indirect) {
                मुक्त(h->indirect.storage);
                h->has_indirect = false;
        }

        h->n_direct_entries = 0;
        reset_direct_storage(h);

        सचे (h->type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*) h;
                lh->iterate_list_head = lh->iterate_list_tail = IDX_NIL;
        }

        base_set_dirty(h);
}

स्थिर पूर्णाङ्क resize_buckets(HashmapBase *h, unsigned entries_add);

/*
 * Finds an empty bucket to put an entry into, starting the scan at 'idx'.
 * Performs Robin Hood swaps as it goes. The entry to put must be placed
 * by the caller into swap slot IDX_PUT.
 * If used for in-place resizing, may leave a displaced entry in swap slot
 * IDX_PUT. Caller must rehash it next.
 * Returns: true if it left a displaced entry to rehash next in IDX_PUT,
 *          false otherwise.
 */
स्थिर bool hashmap_put_robin_hood(HashmapBase *h, unsigned idx,
                                   संरचना swap_entries *swap) {
        dib_raw_t raw_dib, *dibs;
        unsigned dib, distance;

#सचे ENABLE_DEBUG_HASHMAP
        प्रतिज्ञा(h);
        h->debug.put_गणनम्++;
#endif

        dibs = dib_raw_ptr(h);

        अत्थाय (distance = 0; ; distance++) {
                raw_dib = dibs[idx];
                सचे (IN_SET(raw_dib, DIB_RAW_FREE, DIB_RAW_REHASH)) {
                        सचे (raw_dib == DIB_RAW_REHASH)
                                bucket_move_entry(h, swap, idx, IDX_TMP);

                        सचे (h->has_indirect && h->indirect.idx_lowest_entry > idx)
                                h->indirect.idx_lowest_entry = idx;

                        bucket_set_dib(h, idx, distance);
                        bucket_move_entry(h, swap, IDX_PUT, idx);
                        सचे (raw_dib == DIB_RAW_REHASH) {
                                bucket_move_entry(h, swap, IDX_TMP, IDX_PUT);
                                पटिनिवत्त true;
                        }

                        पटिनिवत्त false;
                }

                dib = bucket_calculate_dib(h, idx, raw_dib);

                सचे (dib < distance) {
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
स्थिर पूर्णाङ्क hashmap_base_put_boldly(HashmapBase *h, unsigned idx,
                                   संरचना swap_entries *swap, bool may_resize) {
        संरचना ordered_hashmap_entry *new_entry;
        पूर्णाङ्क r;

        प्रतिज्ञा(idx < n_buckets(h));

        new_entry = bucket_at_swap(swap, IDX_PUT);

        सचे (may_resize) {
                r = resize_buckets(h, 1);
                सचे (r < 0)
                        पटिनिवत्त r;
                सचे (r > 0)
                        idx = bucket_hash(h, new_entry->p.b.कुञ्चिका);
        }
        प्रतिज्ञा(n_entries(h) < n_buckets(h));

        सचे (h->type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*) h;

                new_entry->iterate_next = IDX_NIL;
                new_entry->iterate_previous = lh->iterate_list_tail;

                सचे (lh->iterate_list_tail != IDX_NIL) {
                        संरचना ordered_hashmap_entry *old_tail;

                        old_tail = ordered_bucket_at(lh, lh->iterate_list_tail);
                        प्रतिज्ञा(old_tail->iterate_next == IDX_NIL);
                        old_tail->iterate_next = IDX_PUT;
                }

                lh->iterate_list_tail = IDX_PUT;
                सचे (lh->iterate_list_head == IDX_NIL)
                        lh->iterate_list_head = IDX_PUT;
        }

        assert_se(hashmap_put_robin_hood(h, idx, swap) == false);

        n_entries_inc(h);
#सचे ENABLE_DEBUG_HASHMAP
        h->debug.max_entries = MAX(h->debug.max_entries, n_entries(h));
#endif

        base_set_dirty(h);

        पटिनिवत्त 1;
}
#define hashmap_put_boldly(h, idx, swap, may_resize) \
        hashmap_base_put_boldly(HASHMAP_BASE(h), idx, swap, may_resize)

/*
 * Returns 0 if resize is not needed.
 *         1 if successfully resized.
 *         -ENOMEM on allocation failure.
 */
स्थिर पूर्णाङ्क resize_buckets(HashmapBase *h, unsigned entries_add) {
        संरचना swap_entries swap;
        शून्य *new_storage;
        dib_raw_t *old_dibs, *new_dibs;
        ध्रुव संरचना hashmap_type_info *hi;
        unsigned idx, optimal_idx;
        unsigned old_n_buckets, new_n_buckets, n_rehashed, new_n_entries;
        uint8_t new_shift;
        bool rehash_next;

        प्रतिज्ञा(h);

        hi = &hashmap_type_info[h->type];
        new_n_entries = n_entries(h) + entries_add;

        /* overflow? */
        सचे (_unlikely_(new_n_entries < entries_add))
                पटिनिवत्त -ENOMEM;

        /* For direct storage we allow 100% load, because it's tiny. */
        सचे (!h->has_indirect && new_n_entries <= hi->n_direct_buckets)
                पटिनिवत्त 0;

        /*
         * Load factor = n/m = 1 - (1/INV_KEEP_FREE).
         * From it follows: m = n + n/(INV_KEEP_FREE - 1)
         */
        new_n_buckets = new_n_entries + new_n_entries / (INV_KEEP_FREE - 1);
        /* overflow? */
        सचे (_unlikely_(new_n_buckets < new_n_entries))
                पटिनिवत्त -ENOMEM;

        old_n_buckets = n_buckets(h);

        सचे (_likely_(new_n_buckets <= old_n_buckets))
                पटिनिवत्त 0;

        सचे (_unlikely_(new_n_buckets > UINT_MAX / (hi->entry_size + परिमाण(dib_raw_t))))
                पटिनिवत्त -ENOMEM;

        new_shift = log2u_round_up(MAX(
                        new_n_buckets * (hi->entry_size + परिमाण(dib_raw_t)),
                        2 * परिमाण(संरचना direct_storage)));

        /* Realloc storage (buckets and DIB array). */
        new_storage = realloc(h->has_indirect ? h->indirect.storage : रिक्त,
                              1U << new_shift);
        सचे (!new_storage)
                पटिनिवत्त -ENOMEM;

        /* Must upgrade direct to indirect storage. */
        सचे (!h->has_indirect) {
                memcpy(new_storage, h->direct.storage,
                       old_n_buckets * (hi->entry_size + परिमाण(dib_raw_t)));
                h->indirect.n_entries = h->n_direct_entries;
                h->indirect.idx_lowest_entry = 0;
                h->n_direct_entries = 0;
        }

        /* Get a new hash key. If we've just upgraded to indirect storage,
         * allow reusing a previously generated key. It's still a different key
         * from the shared one that we used for direct storage. */
        get_hash_कुञ्चिका(h->indirect.hash_कुञ्चिका, !h->has_indirect);

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
        अत्थाय (idx = 0; idx < old_n_buckets; idx++) {
                प्रतिज्ञा(old_dibs[idx] != DIB_RAW_REHASH);
                new_dibs[idx] = old_dibs[idx] == DIB_RAW_FREE ? DIB_RAW_FREE
                                                              : DIB_RAW_REHASH;
        }

        /* Zero the area of newly added entries (including the old DIB area) */
        memzero(bucket_at(h, old_n_buckets),
               (n_buckets(h) - old_n_buckets) * hi->entry_size);

        /* The upper half of the new DIB array needs initialization */
        memset(&new_dibs[old_n_buckets], DIB_RAW_INIT,
               (n_buckets(h) - old_n_buckets) * परिमाण(dib_raw_t));

        /* Rehash entries that need it */
        n_rehashed = 0;
        अत्थाय (idx = 0; idx < old_n_buckets; idx++) {
                सचे (new_dibs[idx] != DIB_RAW_REHASH)
                        अनुवर्त;

                optimal_idx = bucket_hash(h, bucket_at(h, idx)->कुञ्चिका);

                /*
                 * Not much to do if by luck the entry hashes to its current
                 * location. Just set its DIB.
                 */
                सचे (optimal_idx == idx) {
                        new_dibs[idx] = 0;
                        n_rehashed++;
                        अनुवर्त;
                }

                new_dibs[idx] = DIB_RAW_FREE;
                bucket_move_entry(h, &swap, idx, IDX_PUT);
                /* bucket_move_entry does not clear the source */
                memzero(bucket_at(h, idx), hi->entry_size);

                करोहि {
                        /*
                         * Find the new bucket for the current entry. This may make
                         * another entry homeless and load it into IDX_PUT.
                         */
                        rehash_next = hashmap_put_robin_hood(h, optimal_idx, &swap);
                        n_rehashed++;

                        /* Did the current entry displace another one? */
                        सचे (rehash_next)
                                optimal_idx = bucket_hash(h, bucket_at_swap(&swap, IDX_PUT)->p.b.कुञ्चिका);
                } याव (rehash_next);
        }

        assert_se(n_rehashed == n_entries(h));

        पटिनिवत्त 1;
}

/*
 * Finds an entry with a matching key
 * Returns: index of the found entry, or IDX_NIL if not found.
 */
स्थिर unsigned base_bucket_scan(HashmapBase *h, unsigned idx, ध्रुव शून्य *कुञ्चिका) {
        compare_func_t compare = h->hash_ops->compare;
        संरचना hashmap_base_entry *e;
        unsigned dib, distance;
        dib_raw_t *dibs = dib_raw_ptr(h);

        प्रतिज्ञा(idx < n_buckets(h));

        अत्थाय (distance = 0; ; distance++) {
                सचे (dibs[idx] == DIB_RAW_FREE)
                        पटिनिवत्त IDX_NIL;

                dib = bucket_calculate_dib(h, idx, dibs[idx]);

                सचे (dib < distance)
                        पटिनिवत्त IDX_NIL;
                सचे (dib == distance) {
                        e = bucket_at(h, idx);

                        /* Most maps compare keys by pointer. Do that here, without the indirect call. */
                        सचे (compare == trivial_compare_func ? e->कुञ्चिका == कुञ्चिका : compare(e->कुञ्चिका, कुञ्चिका) == 0)
                                पटिनिवत्त idx;
                }

                idx = next_idx(h, idx);
        }
}
#define bucket_scan(h, idx, कुञ्चिका) base_bucket_scan(HASHMAP_BASE(h), idx, कुञ्चिका)

पूर्णाङ्क hashmap_put(Hashmap *h, ध्रुव शून्य *कुञ्चिका, शून्य *मानम्) {
        संरचना swap_entries swap;
        संरचना plain_hashmap_entry *e;
        unsigned hash, idx;

        प्रतिज्ञा(h);

        hash = bucket_hash(h, कुञ्चिका);
        idx = bucket_scan(h, hash, कुञ्चिका);
        सचे (idx != IDX_NIL) {
                e = plain_bucket_at(h, idx);
                सचे (e->मानम् == मानम्)
                        पटिनिवत्त 0;
                पटिनिवत्त -EEXIST;
        }

        e = &bucket_at_swap(&swap, IDX_PUT)->p;
        e->b.कुञ्चिका = कुञ्चिका;
        e->मानम् = मानम्;
        पटिनिवत्त hashmap_put_boldly(h, hash, &swap, true);
}

पूर्णाङ्क set_put(Set *s, ध्रुव शून्य *कुञ्चिका) {
        संरचना swap_entries swap;
        संरचना hashmap_base_entry *e;
        unsigned hash, idx;

        प्रतिज्ञा(s);

        hash = bucket_hash(s, कुञ्चिका);
        idx = bucket_scan(s, hash, कुञ्चिका);
        सचे (idx != IDX_NIL)
                पटिनिवत्त 0;

        e = &bucket_at_swap(&swap, IDX_PUT)->p.b;
        e->कुञ्चिका = कुञ्चिका;
        पटिनिवत्त hashmap_put_boldly(s, hash, &swap, true);
}

पूर्णाङ्क set_ensure_put(Set **s, ध्रुव संरचना hash_ops *hash_ops, ध्रुव शून्य *कुञ्चिका) {
        पूर्णाङ्क r;

        प्रतिज्ञा(s);

        r = set_ensure_allocated(s, hash_ops);
        सचे (r < 0)
                पटिनिवत्त r;

        पटिनिवत्त set_put(*s, कुञ्चिका);
}

पूर्णाङ्क set_ensure_consume(Set **s, ध्रुव संरचना hash_ops *hash_ops, शून्य *कुञ्चिका) {
        पूर्णाङ्क r;

        r = set_ensure_put(s, hash_ops, कुञ्चिका);
        सचे (r <= 0) {
                सचे (hash_ops && hash_ops->free_कुञ्चिका)
                        hash_ops->free_कुञ्चिका(कुञ्चिका);
                अञ्ञथा सचे (hash_ops && hash_ops->free_मानम्)
                        /* Sets store their element in the key slot but may carry a value destructor. */
                        hash_ops->free_मानम्(कुञ्चिका);
                अञ्ञथा
                        मुक्त(कुञ्चिका);
        }

        पटिनिवत्त r;
}

पूर्णाङ्क hashmap_replace(Hashmap *h, ध्रुव शून्य *कुञ्चिका, शून्य *मानम्) {
        संरचना swap_entries swap;
        संरचना plain_hashmap_entry *e;
        unsigned hash, idx;

        प्रतिज्ञा(h);

        hash = bucket_hash(h, कुञ्चिका);
        idx = bucket_scan(h, hash, कुञ्चिका);
        सचे (idx != IDX_NIL) {
                e = plain_bucket_at(h, idx);
#सचे ENABLE_DEBUG_HASHMAP
                /* Although the key is equal, the key pointer may have changed,
                 * and this would break our assumption for iterating. So count
                 * this operation as incompatible with iteration. */
                सचे (e->b.कुञ्चिका != कुञ्चिका) {
                        h->b.debug.put_गणनम्++;
                        h->b.debug.rem_गणनम्++;
                        h->b.debug.last_rem_idx = idx;
                }
#endif
                e->b.कुञ्चिका = कुञ्चिका;
                e->मानम् = मानम्;
                hashmap_set_dirty(h);

                पटिनिवत्त 0;
        }

        e = &bucket_at_swap(&swap, IDX_PUT)->p;
        e->b.कुञ्चिका = कुञ्चिका;
        e->मानम् = मानम्;
        पटिनिवत्त hashmap_put_boldly(h, hash, &swap, true);
}

पूर्णाङ्क hashmap_update(Hashmap *h, ध्रुव शून्य *कुञ्चिका, शून्य *मानम्) {
        संरचना plain_hashmap_entry *e;
        unsigned hash, idx;

        प्रतिज्ञा(h);

        hash = bucket_hash(h, कुञ्चिका);
        idx = bucket_scan(h, hash, कुञ्चिका);
        सचे (idx == IDX_NIL)
                पटिनिवत्त -ENOENT;

        e = plain_bucket_at(h, idx);
        e->मानम् = मानम्;
        hashmap_set_dirty(h);

        पटिनिवत्त 0;
}

शून्य* _hashmap_get(HashmapBase *h, ध्रुव शून्य *कुञ्चिका) {
        संरचना hashmap_base_entry *e;
        unsigned hash, idx;

        सचे (!h)
                पटिनिवत्त रिक्त;

        hash = bucket_hash(h, कुञ्चिका);
        idx = bucket_scan(h, hash, कुञ्चिका);
        सचे (idx == IDX_NIL)
                पटिनिवत्त रिक्त;

        e = bucket_at(h, idx);
        पटिनिवत्त entry_मानम्(h, e);
}

शून्य* hashmap_get2(Hashmap *h, ध्रुव शून्य *कुञ्चिका, शून्य **ret) {
        संरचना plain_hashmap_entry *e;
        unsigned hash, idx;

        सचे (!h)
                पटिनिवत्त रिक्त;

        hash = bucket_hash(h, कुञ्चिका);
        idx = bucket_scan(h, hash, कुञ्चिका);
        सचे (idx == IDX_NIL)
                पटिनिवत्त रिक्त;

        e = plain_bucket_at(h, idx);
        सचे (ret)
                *ret = (शून्य*) e->b.कुञ्चिका;

        पटिनिवत्त e->मानम्;
}

bool _hashmap_contains(HashmapBase *h, ध्रुव शून्य *कुञ्चिका) {
        unsigned hash;

        सचे (!h)
                पटिनिवत्त false;

        hash = bucket_hash(h, कुञ्चिका);
        पटिनिवत्त bucket_scan(h, hash, कुञ्चिका) != IDX_NIL;
}

शून्य* _hashmap_remove(HashmapBase *h, ध्रुव शून्य *कुञ्चिका) {
        संरचना hashmap_base_entry *e;
        unsigned hash, idx;
        शून्य *data;

        सचे (!h)
                पटिनिवत्त रिक्त;

        hash = bucket_hash(h, कुञ्चिका);
        idx = bucket_scan(h, hash, कुञ्चिका);
        सचे (idx == IDX_NIL)
                पटिनिवत्त रिक्त;

        e = bucket_at(h, idx);
        data = entry_मानम्(h, e);
        remove_entry(h, idx);

        पटिनिवत्त data;
}

शून्य* hashmap_remove2(Hashmap *h, ध्रुव शून्य *कुञ्चिका, शून्य **ret) {
        संरचना plain_hashmap_entry *e;
        unsigned hash, idx;
        शून्य *data;

        सचे (!h) {
                सचे (ret)
                        *ret = रिक्त;
                पटिनिवत्त रिक्त;
        }

        hash = bucket_hash(h, कुञ्चिका);
        idx = bucket_scan(h, hash, कुञ्चिका);
        सचे (idx == IDX_NIL) {
                सचे (ret)
                        *ret = रिक्त;
                पटिनिवत्त रिक्त;
        }

        e = plain_bucket_at(h, idx);
        data = e->मानम्;
        सचे (ret)
                *ret = (शून्य*) e->b.कुञ्चिका;

        remove_entry(h, idx);

        पटिनिवत्त data;
}

पूर्णाङ्क hashmap_remove_and_put(Hashmap *h, ध्रुव शून्य *old_कुञ्चिका, ध्रुव शून्य *new_कुञ्चिका, शून्य *मानम्) {
        संरचना swap_entries swap;
        संरचना plain_hashmap_entry *e;
        unsigned old_hash, new_hash, idx;

        सचे (!h)
                पटिनिवत्त -ENOENT;

        old_hash = bucket_hash(h, old_कुञ्चिका);
        idx = bucket_scan(h, old_hash, old_कुञ्चिका);
        सचे (idx == IDX_NIL)
                पटिनिवत्त -ENOENT;

        new_hash = bucket_hash(h, new_कुञ्चिका);
        सचे (bucket_scan(h, new_hash, new_कुञ्चिका) != IDX_NIL)
                पटिनिवत्त -EEXIST;

        remove_entry(h, idx);

        e = &bucket_at_swap(&swap, IDX_PUT)->p;
        e->b.कुञ्चिका = new_कुञ्चिका;
        e->मानम् = मानम्;
        assert_se(hashmap_put_boldly(h, new_hash, &swap, false) == 1);

        पटिनिवत्त 0;
}

पूर्णाङ्क set_remove_and_put(Set *s, ध्रुव शून्य *old_कुञ्चिका, ध्रुव शून्य *new_कुञ्चिका) {
        संरचना swap_entries swap;
        संरचना hashmap_base_entry *e;
        unsigned old_hash, new_hash, idx;

        सचे (!s)
                पटिनिवत्त -ENOENT;

        old_hash = bucket_hash(s, old_कुञ्चिका);
        idx = bucket_scan(s, old_hash, old_कुञ्चिका);
        सचे (idx == IDX_NIL)
                पटिनिवत्त -ENOENT;

        new_hash = bucket_hash(s, new_कुञ्चिका);
        सचे (bucket_scan(s, new_hash, new_कुञ्चिका) != IDX_NIL)
                पटिनिवत्त -EEXIST;

        remove_entry(s, idx);

        e = &bucket_at_swap(&swap, IDX_PUT)->p.b;
        e->कुञ्चिका = new_कुञ्चिका;
        assert_se(hashmap_put_boldly(s, new_hash, &swap, false) == 1);

        पटिनिवत्त 0;
}

पूर्णाङ्क hashmap_remove_and_replace(Hashmap *h, ध्रुव शून्य *old_कुञ्चिका, ध्रुव शून्य *new_कुञ्चिका, शून्य *मानम्) {
        संरचना swap_entries swap;
        संरचना plain_hashmap_entry *e;
        unsigned old_hash, new_hash, idx_old, idx_new;

        सचे (!h)
                पटिनिवत्त -ENOENT;

        old_hash = bucket_hash(h, old_कुञ्चिका);
        idx_old = bucket_scan(h, old_hash, old_कुञ्चिका);
        सचे (idx_old == IDX_NIL)
                पटिनिवत्त -ENOENT;

        old_कुञ्चिका = bucket_at(HASHMAP_BASE(h), idx_old)->कुञ्चिका;

        new_hash = bucket_hash(h, new_कुञ्चिका);
        idx_new = bucket_scan(h, new_hash, new_कुञ्चिका);
        सचे (idx_new != IDX_NIL)
                सचे (idx_old != idx_new) {
                        remove_entry(h, idx_new);
                        /* Compensate for a possible backward shift. */
                        सचे (old_कुञ्चिका != bucket_at(HASHMAP_BASE(h), idx_old)->कुञ्चिका)
                                idx_old = prev_idx(HASHMAP_BASE(h), idx_old);
                        प्रतिज्ञा(old_कुञ्चिका == bucket_at(HASHMAP_BASE(h), idx_old)->कुञ्चिका);
                }

        remove_entry(h, idx_old);

        e = &bucket_at_swap(&swap, IDX_PUT)->p;
        e->b.कुञ्चिका = new_कुञ्चिका;
        e->मानम् = मानम्;
        assert_se(hashmap_put_boldly(h, new_hash, &swap, false) == 1);

        पटिनिवत्त 0;
}

शून्य* hashmap_remove_मानम्(HashmapBase *h, ध्रुव शून्य *कुञ्चिका, शून्य *मानम्) {
        संरचना hashmap_base_entry *e;
        unsigned hash, idx;

        सचे (!h)
                पटिनिवत्त रिक्त;

        hash = bucket_hash(h, कुञ्चिका);
        idx = bucket_scan(h, hash, कुञ्चिका);
        सचे (idx == IDX_NIL)
                पटिनिवत्त रिक्त;

        e = bucket_at(h, idx);
        सचे (entry_मानम्(h, e) != मानम्)
                पटिनिवत्त रिक्त;

        remove_entry(h, idx);

        पटिनिवत्त मानम्;
}

स्थिर unsigned find_first_entry(HashmapBase *h) {
        Iterator i = ITERATOR_FIRST;

        सचे (!h || !n_entries(h))
                पटिनिवत्त IDX_NIL;

        पटिनिवत्त hashmap_iterate_entry(h, &i);
}

शून्य* hashmap_first_कुञ्चिका_and_मानम्(HashmapBase *h, bool remove, शून्य **ret_कुञ्चिका) {
        संरचना hashmap_base_entry *e;
        शून्य *कुञ्चिका, *data;
        unsigned idx;

        idx = find_first_entry(h);
        सचे (idx == IDX_NIL) {
                सचे (ret_कुञ्चिका)
                        *ret_कुञ्चिका = रिक्त;
                पटिनिवत्त रिक्त;
        }

        e = bucket_at(h, idx);
        कुञ्चिका = (शून्य*) e->कुञ्चिका;
        data = entry_मानम्(h, e);

        सचे (remove)
                remove_entry(h, idx);

        सचे (ret_कुञ्चिका)
                *ret_कुञ्चिका = कुञ्चिका;

        पटिनिवत्त data;
}

unsigned _hashmap_size(HashmapBase *h) {
        सचे (!h)
                पटिनिवत्त 0;

        पटिनिवत्त n_entries(h);
}

unsigned _hashmap_buckets(HashmapBase *h) {
        सचे (!h)
                पटिनिवत्त 0;

        पटिनिवत्त n_buckets(h);
}

पूर्णाङ्क _hashmap_merge(Hashmap *h, Hashmap *other) {
        Iterator i;
        unsigned idx;

        प्रतिज्ञा(h);

        HASHMAP_FOREACH_IDX(idx, HASHMAP_BASE(other), i) {
                संरचना plain_hashmap_entry *pe = plain_bucket_at(other, idx);
                पूर्णाङ्क r;

                r = hashmap_put(h, pe->b.कुञ्चिका, pe->मानम्);
                सचे (r < 0 && r != -EEXIST)
                        पटिनिवत्त r;
        }

        पटिनिवत्त 0;
}

पूर्णाङ्क set_merge(Set *s, Set *other) {
        Iterator i;
        unsigned idx;

        प्रतिज्ञा(s);

        HASHMAP_FOREACH_IDX(idx, HASHMAP_BASE(other), i) {
                संरचना set_entry *se = set_bucket_at(other, idx);
                पूर्णाङ्क r;

                r = set_put(s, se->b.कुञ्चिका);
                सचे (r < 0)
                        पटिनिवत्त r;
        }

        पटिनिवत्त 0;
}

पूर्णाङ्क _hashmap_reserve(HashmapBase *h, unsigned entries_add) {
        पूर्णाङ्क r;

        प्रतिज्ञा(h);

        r = resize_buckets(h, entries_add);
        सचे (r < 0)
                पटिनिवत्त r;

        पटिनिवत्त 0;
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
        unsigned idx;
        पूर्णाङ्क r;

        प्रतिज्ञा(h);

        सचे (!other)
                पटिनिवत्त 0;

        प्रतिज्ञा(other->type == h->type);

        /*
         * This reserves buckets for the worst case, where none of other's
         * entries are yet present in h. This is preferable to risking
         * an allocation failure in the middle of the moving and having to
         * rollback or return a partial result.
         */
        r = resize_buckets(h, n_entries(other));
        सचे (r < 0)
                पटिनिवत्त r;

        HASHMAP_FOREACH_IDX(idx, other, i) {
                unsigned h_hash;

                e = bucket_at(other, idx);
                h_hash = bucket_hash(h, e->कुञ्चिका);
                सचे (bucket_scan(h, h_hash, e->कुञ्चिका) != IDX_NIL)
                        अनुवर्त;

                n = &bucket_at_swap(&swap, IDX_PUT)->p.b;
                n->कुञ्चिका = e->कुञ्चिका;
                सचे (h->type != HASHMAP_TYPE_SET)
                        ((संरचना plain_hashmap_entry*) n)->मानम् =
                                ((संरचना plain_hashmap_entry*) e)->मानम्;
                assert_se(hashmap_put_boldly(h, h_hash, &swap, false) == 1);

                remove_entry(other, idx);
        }

        पटिनिवत्त 0;
}

पूर्णाङ्क _hashmap_move_one(HashmapBase *h, HashmapBase *other, ध्रुव शून्य *कुञ्चिका) {
        संरचना swap_entries swap;
        unsigned h_hash, other_hash, idx;
        संरचना hashmap_base_entry *e, *n;
        पूर्णाङ्क r;

        प्रतिज्ञा(h);

        h_hash = bucket_hash(h, कुञ्चिका);
        सचे (bucket_scan(h, h_hash, कुञ्चिका) != IDX_NIL)
                पटिनिवत्त -EEXIST;

        सचे (!other)
                पटिनिवत्त -ENOENT;

        प्रतिज्ञा(other->type == h->type);

        other_hash = bucket_hash(other, कुञ्चिका);
        idx = bucket_scan(other, other_hash, कुञ्चिका);
        सचे (idx == IDX_NIL)
                पटिनिवत्त -ENOENT;

        e = bucket_at(other, idx);

        n = &bucket_at_swap(&swap, IDX_PUT)->p.b;
        n->कुञ्चिका = e->कुञ्चिका;
        सचे (h->type != HASHMAP_TYPE_SET)
                ((संरचना plain_hashmap_entry*) n)->मानम् =
                        ((संरचना plain_hashmap_entry*) e)->मानम्;
        r = hashmap_put_boldly(h, h_hash, &swap, true);
        सचे (r < 0)
                पटिनिवत्त r;

        remove_entry(other, idx);
        पटिनिवत्त 0;
}

HashmapBase* _hashmap_copy(HashmapBase *h) {
        HashmapBase *copy;
        पूर्णाङ्क r;

        प्रतिज्ञा(h);

        copy = hashmap_base_new(h->hash_ops, h->type);
        सचे (!copy)
                पटिनिवत्त रिक्त;

        विकल्प (h->type) {
        पक्ष HASHMAP_TYPE_PLAIN:
        पक्ष HASHMAP_TYPE_ORDERED:
                r = hashmap_merge((Hashmap*)copy, (Hashmap*)h);
                भङ्ग;
        पक्ष HASHMAP_TYPE_SET:
                r = set_merge((Set*)copy, (Set*)h);
                भङ्ग;
        स्वभाव:
                assert_not_reached();
        }

        सचे (r < 0)
                पटिनिवत्त _hashmap_free(copy);

        पटिनिवत्त copy;
}

वर्ण** _hashmap_get_strv(HashmapBase *h) {
        वर्ण **sv;
        Iterator i;
        unsigned idx, n;

        सचे (!h)
                पटिनिवत्त new0(वर्ण*, 1);

        sv = new(वर्ण*, n_entries(h)+1);
        सचे (!sv)
                पटिनिवत्त रिक्त;

        n = 0;
        HASHMAP_FOREACH_IDX(idx, h, i)
                sv[n++] = entry_मानम्(h, bucket_at(h, idx));
        sv[n] = रिक्त;

        पटिनिवत्त sv;
}

वर्ण** set_to_strv(Set **s) {
        प्रतिज्ञा(s);

        /* This is similar to set_get_strv(), but invalidates the set on success. */

        वर्ण **v = new(वर्ण*, set_size(*s) + 1);
        सचे (!v)
                पटिनिवत्त रिक्त;

        अत्थाय (वर्ण **p = v; (*p = set_steal_first(*s)); p++)
                ;

        प्रतिज्ञा(set_isempty(*s));
        *s = set_free(*s);
        पटिनिवत्त v;
}

शून्य* ordered_hashmap_next(OrderedHashmap *h, ध्रुव शून्य *कुञ्चिका) {
        संरचना ordered_hashmap_entry *e;
        unsigned hash, idx;

        सचे (!h)
                पटिनिवत्त रिक्त;

        hash = bucket_hash(h, कुञ्चिका);
        idx = bucket_scan(h, hash, कुञ्चिका);
        सचे (idx == IDX_NIL)
                पटिनिवत्त रिक्त;

        e = ordered_bucket_at(h, idx);
        सचे (e->iterate_next == IDX_NIL)
                पटिनिवत्त रिक्त;
        पटिनिवत्त ordered_bucket_at(h, e->iterate_next)->p.मानम्;
}

पूर्णाङ्क set_consume(Set *s, शून्य *मानम्) {
        पूर्णाङ्क r;

        प्रतिज्ञा(s);
        प्रतिज्ञा(मानम्);

        r = set_put(s, मानम्);
        सचे (r <= 0)
                मुक्त(मानम्);

        पटिनिवत्त r;
}

पूर्णाङ्क hashmap_put_strdup_full(Hashmap **h, ध्रुव संरचना hash_ops *hash_ops, ध्रुव वर्ण *k, ध्रुव वर्ण *v) {
        पूर्णाङ्क r;

        प्रतिज्ञा(h);

        r = hashmap_ensure_allocated(h, hash_ops);
        सचे (r < 0)
                पटिनिवत्त r;

        _cleanup_free_ वर्ण *kdup = रिक्त, *vdup = रिक्त;

        kdup = strdup(k);
        सचे (!kdup)
                पटिनिवत्त -ENOMEM;

        सचे (v) {
                vdup = strdup(v);
                सचे (!vdup)
                        पटिनिवत्त -ENOMEM;
        }

        r = hashmap_put(*h, kdup, vdup);
        सचे (r < 0) {
                सचे (r == -EEXIST && streq_ptr(v, hashmap_get(*h, kdup)))
                        पटिनिवत्त 0;
                पटिनिवत्त r;
        }

        /* 0 with non-null vdup would mean vdup is already in the hashmap, which cannot be */
        प्रतिज्ञा(vdup == रिक्त || r > 0);
        सचे (r > 0)
                kdup = vdup = रिक्त;

        पटिनिवत्त r;
}

पूर्णाङ्क set_put_strndup_full(Set **s, ध्रुव संरचना hash_ops *hash_ops, ध्रुव वर्ण *p, size_t n) {
        वर्ण *c;
        पूर्णाङ्क r;

        प्रतिज्ञा(s);
        प्रतिज्ञा(p);

        r = set_ensure_allocated(s, hash_ops);
        सचे (r < 0)
                पटिनिवत्त r;

        सचे (n == SIZE_MAX) {
                सचे (set_contains(*s, (वर्ण*) p))
                        पटिनिवत्त 0;

                c = strdup(p);
        } अञ्ञथा
                c = strndup(p, n);
        सचे (!c)
                पटिनिवत्त -ENOMEM;

        पटिनिवत्त set_consume(*s, c);
}

पूर्णाङ्क set_put_strdupv_full(Set **s, ध्रुव संरचना hash_ops *hash_ops, वर्ण **l) {
        पूर्णाङ्क n = 0, r;

        प्रतिज्ञा(s);

        STRV_FOREACH(i, l) {
                r = set_put_strndup_full(s, hash_ops, *i, SIZE_MAX);
                सचे (r < 0)
                        पटिनिवत्त r;

                n += r;
        }

        पटिनिवत्त n;
}

पूर्णाङ्क set_put_strsplit(Set *s, ध्रुव वर्ण *v, ध्रुव वर्ण *separators, ExtractFlags flags) {
        ध्रुव वर्ण *p = ASSERT_PTR(v);
        पूर्णाङ्क r;

        प्रतिज्ञा(s);

        अत्थाय (;;) {
                वर्ण *word;

                r = extract_first_word(&p, &word, separators, flags);
                सचे (r <= 0)
                        पटिनिवत्त r;

                r = set_consume(s, word);
                सचे (r < 0)
                        पटिनिवत्त r;
        }
}

/* expand the cachemem if needed, return true if newly (re)activated. */
स्थिर पूर्णाङ्क cachemem_maintain(CacheMem *mem, size_t size) {
        प्रतिज्ञा(mem);

        सचे (!GREEDY_REALLOC(mem->ptr, size)) {
                सचे (size > 0)
                        पटिनिवत्त -ENOMEM;
        }

        सचे (!mem->active) {
                mem->active = true;
                पटिनिवत्त true;
        }

        पटिनिवत्त false;
}

पूर्णाङ्क iterated_cache_get(IteratedCache *cache, ध्रुव शून्य ***res_कुञ्चिकाः, ध्रुव शून्य ***res_मानानि, unsigned *res_n_entries) {
        bool sync_कुञ्चिकाः = false, sync_मानानि = false;
        size_t size;
        पूर्णाङ्क r;

        प्रतिज्ञा(cache);
        प्रतिज्ञा(cache->hashmap);

        size = n_entries(cache->hashmap);

        सचे (res_कुञ्चिकाः) {
                r = cachemem_maintain(&cache->कुञ्चिकाः, size);
                सचे (r < 0)
                        पटिनिवत्त r;

                sync_कुञ्चिकाः = r;
        } अञ्ञथा
                cache->कुञ्चिकाः.active = false;

        सचे (res_मानानि) {
                r = cachemem_maintain(&cache->values, size);
                सचे (r < 0)
                        पटिनिवत्त r;

                sync_मानानि = r;
        } अञ्ञथा
                cache->values.active = false;

        सचे (cache->hashmap->dirty) {
                सचे (cache->कुञ्चिकाः.active)
                        sync_कुञ्चिकाः = true;
                सचे (cache->values.active)
                        sync_मानानि = true;

                cache->hashmap->dirty = false;
        }

        सचे (sync_कुञ्चिकाः || sync_मानानि) {
                unsigned i, idx;
                Iterator iter;

                i = 0;
                HASHMAP_FOREACH_IDX(idx, cache->hashmap, iter) {
                        संरचना hashmap_base_entry *e;

                        e = bucket_at(cache->hashmap, idx);

                        सचे (sync_कुञ्चिकाः)
                                cache->कुञ्चिकाः.ptr[i] = e->कुञ्चिका;
                        सचे (sync_मानानि)
                                cache->values.ptr[i] = entry_मानम्(cache->hashmap, e);
                        i++;
                }
        }

        सचे (res_कुञ्चिकाः)
                *res_कुञ्चिकाः = cache->कुञ्चिकाः.ptr;
        सचे (res_मानानि)
                *res_मानानि = cache->values.ptr;
        सचे (res_n_entries)
                *res_n_entries = size;

        पटिनिवत्त 0;
}

IteratedCache* iterated_cache_free(IteratedCache *cache) {
        सचे (cache) {
                मुक्त(cache->कुञ्चिकाः.ptr);
                मुक्त(cache->values.ptr);
        }

        पटिनिवत्त mfree(cache);
}

पूर्णाङ्क set_strjoin(Set *s, ध्रुव वर्ण *separator, bool wrap_with_separator, वर्ण **ret) {
        _cleanup_free_ वर्ण *str = रिक्त;
        size_t separator_len, len = 0;
        ध्रुव वर्ण *मानम्;
        bool first;

        प्रतिज्ञा(ret);

        सचे (set_isempty(s)) {
                *ret = रिक्त;
                पटिनिवत्त 0;
        }

        separator_len = strlen_ptr(separator);

        सचे (separator_len == 0)
                wrap_with_separator = false;

        first = !wrap_with_separator;

        SET_FOREACH(मानम्, s) {
                size_t l = strlen_ptr(मानम्);

                सचे (l == 0)
                        अनुवर्त;

                सचे (!GREEDY_REALLOC(str, len + l + (first ? 0 : separator_len) + (wrap_with_separator ? separator_len : 0) + 1))
                        पटिनिवत्त -ENOMEM;

                सचे (separator_len > 0 && !first) {
                        memcpy(str + len, separator, separator_len);
                        len += separator_len;
                }

                memcpy(str + len, मानम्, l);
                len += l;
                first = false;
        }

        सचे (wrap_with_separator) {
                memcpy(str + len, separator, separator_len);
                len += separator_len;
        }

        str[len] = '\0';

        *ret = TAKE_PTR(str);
        पटिनिवत्त 0;
}

bool set_equal(Set *a, Set *b) {
        शून्य *p;

        /* Checks whether each entry of 'a' is also in 'b' and vice versa, i.e. the two sets contain the same
         * entries */

        सचे (a == b)
                पटिनिवत्त true;

        सचे (set_isempty(a) && set_isempty(b))
                पटिनिवत्त true;

        सचे (set_size(a) != set_size(b)) /* Cheap check that hopefully catches a lot of inequality cases
                                         * already */
                पटिनिवत्त false;

        SET_FOREACH(p, a)
                सचे (!set_contains(b, p))
                        पटिनिवत्त false;

        /* If we have the same hashops, then we don't need to check things backwards given we compared the
         * size and that all of a is in b. */
        सचे (a->b.hash_ops == b->b.hash_ops)
                पटिनिवत्त true;

        SET_FOREACH(p, b)
                सचे (!set_contains(a, p))
                        पटिनिवत्त false;

        पटिनिवत्त true;
}

स्थिर bool set_fnmatch_one(Set *patterns, ध्रुव वर्ण *needle) {
        ध्रुव वर्ण *p;

        प्रतिज्ञा(needle);

        /* Any failure of fnmatch() is treated as equivalent to FNM_NOMATCH, i.e. as non-matching pattern */

        SET_FOREACH(p, patterns)
                सचे (fnmatch(p, needle, 0) == 0)
                        पटिनिवत्त true;

        पटिनिवत्त false;
}

bool set_fnmatch(Set *include_patterns, Set *exclude_patterns, ध्रुव वर्ण *needle) {
        प्रतिज्ञा(needle);

        सचे (set_fnmatch_one(exclude_patterns, needle))
                पटिनिवत्त false;

        सचे (set_isempty(include_patterns))
                पटिनिवत्त true;

        पटिनिवत्त set_fnmatch_one(include_patterns, needle);
}

स्थिर पूर्णाङ्क hashmap_entry_compare(
                संरचना hashmap_base_entry * ध्रुव *a,
                संरचना hashmap_base_entry * ध्रुव *b,
                compare_func_t compare) {

        प्रतिज्ञा(a && *a);
        प्रतिज्ञा(b && *b);
        प्रतिज्ञा(compare);

        पटिनिवत्त compare((*a)->कुञ्चिका, (*b)->कुञ्चिका);
}

स्थिर पूर्णाङ्क _hashmap_dump_entries_sorted(
                HashmapBase *h,
                शून्य ***ret,
                size_t *ret_n) {
        _cleanup_free_ शून्य **entries = रिक्त;
        Iterator iter;
        unsigned idx;
        size_t n = 0;

        प्रतिज्ञा(ret);
        प्रतिज्ञा(ret_n);

        सचे (_hashmap_size(h) == 0) {
                *ret = रिक्त;
                *ret_n = 0;
                पटिनिवत्त 0;
        }

        /* We append one more element than needed so that the resulting array can be used as a strv. We
         * don't count this entry in the returned size. */
        entries = new(शून्य*, _hashmap_size(h) + 1);
        सचे (!entries)
                पटिनिवत्त -ENOMEM;

        HASHMAP_FOREACH_IDX(idx, h, iter)
                entries[n++] = bucket_at(h, idx);

        प्रतिज्ञा(n == _hashmap_size(h));
        entries[n] = रिक्त;

        typesafe_qsort_r((संरचना hashmap_base_entry**) entries, n,
                         hashmap_entry_compare, h->hash_ops->compare);

        *ret = TAKE_PTR(entries);
        *ret_n = n;
        पटिनिवत्त 0;
}

पूर्णाङ्क hashmap_dump_कुञ्चिकाः_sorted(HashmapBase *h, शून्य ***ret, size_t *ret_n) {
        _cleanup_free_ शून्य **entries = रिक्त;
        size_t n;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        r = _hashmap_dump_entries_sorted(h, &entries, &n);
        सचे (r < 0)
                पटिनिवत्त r;

        /* Reuse the array. */
        FOREACH_ARRAY(e, entries, n)
                *e = (शून्य*) (*(संरचना hashmap_base_entry**) e)->कुञ्चिका;

        *ret = TAKE_PTR(entries);
        सचे (ret_n)
                *ret_n = n;
        पटिनिवत्त 0;
}

पूर्णाङ्क _hashmap_dump_sorted(HashmapBase *h, शून्य ***ret, size_t *ret_n) {
        _cleanup_free_ शून्य **entries = रिक्त;
        size_t n;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        r = _hashmap_dump_entries_sorted(h, &entries, &n);
        सचे (r < 0)
                पटिनिवत्त r;

        /* Reuse the array. */
        FOREACH_ARRAY(e, entries, n)
                *e = entry_मानम्(h, *(संरचना hashmap_base_entry**) e);

        *ret = TAKE_PTR(entries);
        सचे (ret_n)
                *ret_n = n;
        पटिनिवत्त 0;
}
