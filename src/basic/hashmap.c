/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश <fnmatch.h>
#समावेश <pthread.h>
#समावेश <unistd.h>
#अगर HAVE_VALGRIND_VALGRIND_H
#  include <valgrind/valgrind.h>
#पूर्ण_अगर

#समावेश "alloc-util.h"
#समावेश "extract-word.h"
#समावेश "hashmap.h"
#समावेश "log.h"
#समावेश "logarithm.h"
#समावेश "memory-util.h"
#समावेश "mempool.h"
#समावेश "process-util.h"
#समावेश "random-util.h"
#समावेश "set.h"
#समावेश "siphash24.h"
#समावेश "sort-util.h"
#समावेश "string-util.h"
#समावेश "strv.h"

#अगर ENABLE_DEBUG_HASHMAP
#समावेश "list.h"
#पूर्ण_अगर

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
#घोषणा INV_KEEP_FREE            5U

/* Fields common to entries of all hashmap/set types */
काष्ठा hashmap_base_entry {
        स्थिर व्योम *कुंजी;
};

/* Entry types for specific hashmap/set types
 * hashmap_base_entry must be at the beginning of each entry struct. */

काष्ठा plain_hashmap_entry {
        काष्ठा hashmap_base_entry b;
        व्योम *मूल्य;
};

काष्ठा ordered_hashmap_entry {
        काष्ठा plain_hashmap_entry p;
        अचिन्हित iterate_अग्र, iterate_previous;
};

काष्ठा set_entry {
        काष्ठा hashmap_base_entry b;
};

/* In several functions it is advantageous to have the hash table extended
 * virtually by a couple of additional buckets. We reserve special index values
 * for these "swap" buckets. */
#घोषणा _IDX_SWAP_BEGIN     (अच_पूर्णांक_उच्च - 3)
#घोषणा IDX_PUT             (_IDX_SWAP_BEGIN + 0)
#घोषणा IDX_TMP             (_IDX_SWAP_BEGIN + 1)
#घोषणा _IDX_SWAP_END       (_IDX_SWAP_BEGIN + 2)

#घोषणा IDX_FIRST           (अच_पूर्णांक_उच्च - 1) /* special index for freshly initialized iterators */
#घोषणा IDX_NIL             अच_पूर्णांक_उच्च       /* special index value meaning "none" or "end" */

assert_cc(IDX_FIRST == _IDX_SWAP_END);
assert_cc(IDX_FIRST == _IDX_ITERATOR_FIRST);

/* Storage space for the "swap" buckets.
 * All entry types can fit into an ordered_hashmap_entry. */
काष्ठा swap_entries {
        काष्ठा ordered_hashmap_entry e[_IDX_SWAP_END - _IDX_SWAP_BEGIN];
};

/* Distance from Initial Bucket */
प्रकार uint8_t dib_raw_t;
#घोषणा DIB_RAW_OVERFLOW ((dib_raw_t)0xfdU)   /* indicates DIB value is greater than representable */
#घोषणा DIB_RAW_REHASH   ((dib_raw_t)0xfeU)   /* entry yet to be rehashed during in-place resize */
#घोषणा DIB_RAW_FREE     ((dib_raw_t)0xffU)   /* a free bucket */
#घोषणा DIB_RAW_INIT     ((अक्षर)DIB_RAW_FREE) /* a byte to memset a DIB store with when initializing */

#घोषणा DIB_FREE अच_पूर्णांक_उच्च

#अगर ENABLE_DEBUG_HASHMAP
काष्ठा hashmap_debug_info {
        LIST_FIELDS(काष्ठा hashmap_debug_info, debug_list);
        अचिन्हित max_entries;  /* high watermark of n_entries */

        /* fields to detect modification while iterating */
        अचिन्हित put_गिनती;    /* counts puts into the hashmap */
        अचिन्हित rem_गिनती;    /* counts removals from hashmap */
        अचिन्हित last_rem_idx; /* remembers last removal index */
};

/* Tracks all existing hashmaps. Get at it from gdb. See sd_dump_hashmaps.py */
अटल LIST_शीर्ष(काष्ठा hashmap_debug_info, hashmap_debug_list);
अटल pthread_mutex_t hashmap_debug_list_mutex = PTHREAD_MUTEX_INITIALIZER;
#पूर्ण_अगर

क्रमागत HashmapType {
        HASHMAP_TYPE_PLAIN,
        HASHMAP_TYPE_ORDERED,
        HASHMAP_TYPE_SET,
        _HASHMAP_TYPE_MAX
};

काष्ठा _packed_ indirect_storage {
        व्योम *storage;                     /* where buckets and DIBs are stored */
        uint8_t  hash_कुंजी[HASH_कुंजी_आकार];  /* hash key; changes during resize */

        अचिन्हित n_entries;                /* number of stored entries */
        अचिन्हित n_buckets;                /* number of buckets */

        अचिन्हित idx_lowest_entry;         /* Index below which all buckets are free.
                                              Makes "while (hashmap_steal_first())" loops
                                              O(n) instead of O(n^2) for unordered hashmaps. */
        uint8_t  _pad[3];                  /* padding for the whole HashmapBase */
        /* The bitfields in HashmapBase complete the alignment of the whole thing. */
};

काष्ठा direct_storage {
        /* This gives us 39 bytes on 64-bit, or 35 bytes on 32-bit.
         * That's room for 4 set_entries + 4 DIB bytes + 3 unused bytes on 64-bit,
         *              or 7 set_entries + 7 DIB bytes + 0 unused bytes on 32-bit. */
        uint8_t storage[माप(काष्ठा indirect_storage)];
};

#घोषणा DIRECT_BUCKETS(entry_t) \
        (माप(काष्ठा direct_storage) / (माप(entry_t) + माप(dib_raw_t)))

/* We should be able to store at least one entry directly. */
assert_cc(DIRECT_BUCKETS(काष्ठा ordered_hashmap_entry) >= 1);

/* We have 3 bits for n_direct_entries. */
assert_cc(DIRECT_BUCKETS(काष्ठा set_entry) < (1 << 3));

/* Hashmaps with directly stored entries all use this shared hash key.
 * It's no big deal if the key is guessed, because there can be only
 * a handful of directly stored entries in a hashmap. When a hashmap
 * outgrows direct storage, it gets its own key for indirect storage. */
अटल uint8_t shared_hash_कुंजी[HASH_कुंजी_आकार];

/* Fields that all hashmap/set types must have */
काष्ठा HashmapBase {
        स्थिर काष्ठा hash_ops *hash_ops;  /* hash and compare ops to use */

        जोड़ _packed_ {
                काष्ठा indirect_storage indirect; /* if  has_indirect */
                काष्ठा direct_storage direct;     /* if !has_indirect */
        };

        क्रमागत HashmapType type:2;     /* HASHMAP_TYPE_* */
        bool has_indirect:1;         /* whether indirect storage is used */
        अचिन्हित n_direct_entries:3; /* Number of entries in direct storage.
                                      * Only valid if !has_indirect. */
        bool from_pool:1;            /* whether was allocated from mempool */
        bool dirty:1;                /* whether dirtied since last iterated_cache_get() */
        bool cached:1;               /* whether this hashmap is being cached */

#अगर ENABLE_DEBUG_HASHMAP
        काष्ठा hashmap_debug_info debug;
#पूर्ण_अगर
};

/* Specific hash types
 * HashmapBase must be at the beginning of each hashmap struct. */

काष्ठा Hashmap {
        काष्ठा HashmapBase b;
};

काष्ठा OrderedHashmap {
        काष्ठा HashmapBase b;
        अचिन्हित iterate_list_शीर्ष, iterate_list_पुच्छ;
};

काष्ठा Set {
        काष्ठा HashmapBase b;
};

प्रकार काष्ठा CacheMem {
        स्थिर व्योम **ptr;
        माप_प्रकार n_populated;
        bool active:1;
} CacheMem;

काष्ठा IteratedCache {
        HashmapBase *hashmap;
        CacheMem कुंजियाँ, values;
};

DEFINE_MEMPOOL(hashmap_pool,         Hashmap,        8);
DEFINE_MEMPOOL(ordered_hashmap_pool, OrderedHashmap, 8);
/* No need for a separate Set pool */
assert_cc(माप(Hashmap) == माप(Set));

काष्ठा hashmap_type_info {
        माप_प्रकार शीर्ष_आकार;
        माप_प्रकार entry_आकार;
        काष्ठा mempool *mempool;
        अचिन्हित n_direct_buckets;
};

अटल _used_ स्थिर काष्ठा hashmap_type_info hashmap_type_info[_HASHMAP_TYPE_MAX] = {
        [HASHMAP_TYPE_PLAIN] = {
                .शीर्ष_आकार        = माप(Hashmap),
                .entry_आकार       = माप(काष्ठा plain_hashmap_entry),
                .mempool          = &hashmap_pool,
                .n_direct_buckets = DIRECT_BUCKETS(काष्ठा plain_hashmap_entry),
        },
        [HASHMAP_TYPE_ORDERED] = {
                .शीर्ष_आकार        = माप(OrderedHashmap),
                .entry_आकार       = माप(काष्ठा ordered_hashmap_entry),
                .mempool          = &ordered_hashmap_pool,
                .n_direct_buckets = DIRECT_BUCKETS(काष्ठा ordered_hashmap_entry),
        },
        [HASHMAP_TYPE_SET] = {
                .शीर्ष_आकार        = माप(Set),
                .entry_आकार       = माप(काष्ठा set_entry),
                .mempool          = &hashmap_pool,
                .n_direct_buckets = DIRECT_BUCKETS(काष्ठा set_entry),
        },
};

व्योम hashmap_trim_pools(व्योम) {
        पूर्णांक r;

        /* The pool is only allocated by the main thread, but the memory can be passed to other
         * threads. Let's clean up if we are the main thread and no other threads are live. */

        /* We build our own is_main_thread() here, which doesn't use C11 TLS based caching of the
         * result. That's because valgrind apparently doesn't like TLS to be used from a GCC destructor. */
        अगर (getpid() != gettid())
                वापस (व्योम) log_debug("Not cleaning up memory pools, not in main thread.");

        r = get_process_threads(0);
        अगर (r < 0)
                वापस (व्योम) log_debug_errno(r, "Failed to determine number of threads, not cleaning up memory pools: %m");
        अगर (r != 1)
                वापस (व्योम) log_debug("Not cleaning up memory pools, running in multi-threaded process.");

        mempool_trim(&hashmap_pool);
        mempool_trim(&ordered_hashmap_pool);
}

#अगर HAVE_VALGRIND_VALGRIND_H
_destructor_ अटल व्योम cleanup_pools(व्योम) {
        /* Be nice to valgrind */
        अगर (RUNNING_ON_VALGRIND)
                hashmap_trim_pools();
}
#पूर्ण_अगर

अटल अंतरभूत अचिन्हित n_buckets(HashmapBase *h) {
        निश्चित(h);
        वापस h->has_indirect ? h->indirect.n_buckets
                               : hashmap_type_info[h->type].n_direct_buckets;
}

अटल अचिन्हित n_entries(HashmapBase *h) {
        निश्चित(h);
        वापस h->has_indirect ? h->indirect.n_entries
                               : h->n_direct_entries;
}

अटल व्योम n_entries_inc(HashmapBase *h) {
        निश्चित(h);

        अगर (h->has_indirect)
                h->indirect.n_entries++;
        अन्यथा
                h->n_direct_entries++;
}

अटल व्योम n_entries_dec(HashmapBase *h) {
        निश्चित(h);

        अगर (h->has_indirect)
                h->indirect.n_entries--;
        अन्यथा
                h->n_direct_entries--;
}

अटल अंतरभूत व्योम* storage_ptr(HashmapBase *h) {
        निश्चित(h);
        वापस h->has_indirect ? h->indirect.storage
                               : h->direct.storage;
}

अटल अंतरभूत uint8_t* hash_कुंजी(HashmapBase *h) {
        निश्चित(h);
        वापस h->has_indirect ? h->indirect.hash_कुंजी
                               : shared_hash_कुंजी;
}

अटल अचिन्हित base_bucket_hash(HashmapBase *h, स्थिर व्योम *p) {
        काष्ठा siphash state;
        uint64_t hash;

        निश्चित(h);

        /* The keys of trivial_hash_ops and uint64_hash_ops fit in 64 bits. siphash24_uint64() hashes such
         * a key without the indirect call through hash_ops and keeps the SipHash state in registers. On
         * 32-bit and big-endian architectures, the result differs from the result of trivial_hash_func(),
         * which hashes the key in native byte order. Only this function maps keys to buckets, so the
         * difference does not matter. */
        अगर (h->hash_ops->hash == trivial_hash_func)
                hash = siphash24_uint64((uintptr_t) p, hash_कुंजी(h));
        अन्यथा अगर (h->hash_ops->hash == (hash_func_t) uint64_hash_func)
                hash = siphash24_uint64(*(स्थिर uint64_t*) p, hash_कुंजी(h));
        अन्यथा {
                siphash24_init(&state, hash_कुंजी(h));
                h->hash_ops->hash(p, &state);
                hash = siphash24_finalize(&state);
        }

        /* n_buckets is not a power of two, so a modulo needs a division. Multiplying the upper 32 bits of
         * the hash with n_buckets and keeping the upper 32 bits of the product maps the hash to
         * [0, n_buckets) almost as evenly, without a division. */
        वापस (अचिन्हित) (((hash >> 32) * n_buckets(h)) >> 32);
}
#घोषणा bucket_hash(h, p) base_bucket_hash(HASHMAP_BASE(h), p)

अटल व्योम base_set_dirty(HashmapBase *h) {
        निश्चित(h);

        h->dirty = true;
}
#घोषणा hashmap_set_dirty(h) base_set_dirty(HASHMAP_BASE(h))

अटल व्योम get_hash_कुंजी(uint8_t hash_कुंजी[HASH_कुंजी_आकार], bool reuse_is_ok) {
        अटल uint8_t current[HASH_कुंजी_आकार];
        अटल bool current_initialized = false;

        /* Returns a hash function key to use. In order to keep things
         * fast we will not generate a new key each time we allocate a
         * new hash table. Instead, we'll just reuse the most recently
         * generated one, except if we never generated one or when we
         * are rehashing an entire hash table because we reached a
         * fill level */

        अगर (!current_initialized || !reuse_is_ok) {
                random_bytes(current, माप(current));
                current_initialized = true;
        }

        स_नकल(hash_कुंजी, current, माप(current));
}

अटल अंतरभूत काष्ठा hashmap_base_entry* bucket_at(HashmapBase *h, अचिन्हित idx) {
        निश्चित(h);
        वापस CAST_ALIGN_PTR(
                        काष्ठा hashmap_base_entry,
                        (uint8_t *) storage_ptr(h) + idx * hashmap_type_info[h->type].entry_आकार);
}

अटल काष्ठा plain_hashmap_entry* plain_bucket_at(Hashmap *h, अचिन्हित idx) {
        वापस (काष्ठा plain_hashmap_entry*) bucket_at(HASHMAP_BASE(h), idx);
}

अटल काष्ठा ordered_hashmap_entry* ordered_bucket_at(OrderedHashmap *h, अचिन्हित idx) {
        वापस (काष्ठा ordered_hashmap_entry*) bucket_at(HASHMAP_BASE(h), idx);
}

अटल काष्ठा set_entry *set_bucket_at(Set *h, अचिन्हित idx) {
        वापस (काष्ठा set_entry*) bucket_at(HASHMAP_BASE(h), idx);
}

अटल काष्ठा ordered_hashmap_entry* bucket_at_swap(काष्ठा swap_entries *swap, अचिन्हित idx) {
        निश्चित(swap);
        वापस &swap->e[idx - _IDX_SWAP_BEGIN];
}

/* Returns a pointer to the bucket at index idx.
 * Understands real indexes and swap indexes, hence "_virtual". */
अटल काष्ठा hashmap_base_entry* bucket_at_virtual(HashmapBase *h, काष्ठा swap_entries *swap,
                                                    अचिन्हित idx) {
        अगर (idx < _IDX_SWAP_BEGIN)
                वापस bucket_at(h, idx);

        अगर (idx < _IDX_SWAP_END)
                वापस &bucket_at_swap(swap, idx)->p.b;

        assert_not_reached();
}

अटल अंतरभूत dib_raw_t* dib_raw_ptr(HashmapBase *h) {
        निश्चित(h);
        वापस (dib_raw_t*)
                ((uint8_t*) storage_ptr(h) + hashmap_type_info[h->type].entry_आकार * n_buckets(h));
}

अटल अचिन्हित bucket_distance(HashmapBase *h, अचिन्हित idx, अचिन्हित from) {
        वापस idx >= from ? idx - from
                           : n_buckets(h) + idx - from;
}

अटल अचिन्हित bucket_calculate_dib(HashmapBase *h, अचिन्हित idx, dib_raw_t raw_dib) {
        अचिन्हित initial_bucket;

        अगर (raw_dib == DIB_RAW_FREE)
                वापस DIB_FREE;

        अगर (_likely_(raw_dib < DIB_RAW_OVERFLOW))
                वापस raw_dib;

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
        initial_bucket = bucket_hash(h, bucket_at(h, idx)->कुंजी);
        वापस bucket_distance(h, idx, initial_bucket);
}

अटल व्योम bucket_set_dib(HashmapBase *h, अचिन्हित idx, अचिन्हित dib) {
        dib_raw_ptr(h)[idx] = dib != DIB_FREE ? MIN(dib, DIB_RAW_OVERFLOW) : DIB_RAW_FREE;
}

अटल अचिन्हित skip_free_buckets(HashmapBase *h, अचिन्हित idx) {
        dib_raw_t *dibs;

        dibs = dib_raw_ptr(h);

        क्रम ( ; idx < n_buckets(h); idx++)
                अगर (dibs[idx] != DIB_RAW_FREE)
                        वापस idx;

        वापस IDX_NIL;
}

अटल व्योम bucket_mark_free(HashmapBase *h, अचिन्हित idx) {
        निश्चित(h);

        memzero(bucket_at(h, idx), hashmap_type_info[h->type].entry_आकार);
        bucket_set_dib(h, idx, DIB_FREE);
}

अटल व्योम bucket_move_entry(HashmapBase *h, काष्ठा swap_entries *swap,
                              अचिन्हित from, अचिन्हित to) {
        काष्ठा hashmap_base_entry *e_from, *e_to;

        निश्चित(h);
        निश्चित(from != to);

        e_from = bucket_at_virtual(h, swap, from);
        e_to   = bucket_at_virtual(h, swap, to);

        स_नकल(e_to, e_from, hashmap_type_info[h->type].entry_आकार);

        अगर (h->type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*) h;
                काष्ठा ordered_hashmap_entry *le, *le_to;

                le_to = (काष्ठा ordered_hashmap_entry*) e_to;

                अगर (le_to->iterate_अग्र != IDX_NIL) {
                        le = (काष्ठा ordered_hashmap_entry*)
                             bucket_at_virtual(h, swap, le_to->iterate_अग्र);
                        le->iterate_previous = to;
                }

                अगर (le_to->iterate_previous != IDX_NIL) {
                        le = (काष्ठा ordered_hashmap_entry*)
                             bucket_at_virtual(h, swap, le_to->iterate_previous);
                        le->iterate_अग्र = to;
                }

                अगर (lh->iterate_list_शीर्ष == from)
                        lh->iterate_list_शीर्ष = to;
                अगर (lh->iterate_list_पुच्छ == from)
                        lh->iterate_list_पुच्छ = to;
        }
}

अटल अंतरभूत अचिन्हित अग्र_idx(HashmapBase *h, अचिन्हित idx) {
        वापस idx + 1U < n_buckets(h) ? idx + 1U : 0U;
}

अटल अचिन्हित पूर्व_idx(HashmapBase *h, अचिन्हित idx) {
        वापस idx > 0U ? idx - 1U : n_buckets(h) - 1U;
}

अटल अंतरभूत व्योम* entry_मूल्य(HashmapBase *h, काष्ठा hashmap_base_entry *e) {
        निश्चित(h);
        निश्चित(e);

        चयन (h->type) {

        हाल HASHMAP_TYPE_PLAIN:
        हाल HASHMAP_TYPE_ORDERED:
                वापस ((काष्ठा plain_hashmap_entry*)e)->मूल्य;

        हाल HASHMAP_TYPE_SET:
                वापस (व्योम*) e->कुंजी;

        शेष:
                assert_not_reached();
        }
}

अटल व्योम base_remove_entry(HashmapBase *h, अचिन्हित idx) {
        अचिन्हित बायाँ, दायाँ, पूर्व, dib;
        dib_raw_t raw_dib, *dibs;

        dibs = dib_raw_ptr(h);
        निश्चित(dibs[idx] != DIB_RAW_FREE);

#अगर ENABLE_DEBUG_HASHMAP
        निश्चित(h);
        h->debug.rem_गिनती++;
        h->debug.last_rem_idx = idx;
#पूर्ण_अगर

        बायाँ = idx;
        /* Find the stop bucket ("right"). It is either free or has DIB == 0. */
        क्रम (दायाँ = अग्र_idx(h, बायाँ); ; दायाँ = अग्र_idx(h, दायाँ)) {
                raw_dib = dibs[दायाँ];
                अगर (IN_SET(raw_dib, 0, DIB_RAW_FREE))
                        अवरोध;

                /* The buckets are not supposed to be all occupied and with DIB > 0.
                 * That would mean we could make everyone better off by shifting them
                 * backward. This scenario is impossible. */
                निश्चित(बायाँ != दायाँ);
        }

        अगर (h->type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*) h;
                काष्ठा ordered_hashmap_entry *le = ordered_bucket_at(lh, idx);

                अगर (le->iterate_अग्र != IDX_NIL)
                        ordered_bucket_at(lh, le->iterate_अग्र)->iterate_previous = le->iterate_previous;
                अन्यथा
                        lh->iterate_list_पुच्छ = le->iterate_previous;

                अगर (le->iterate_previous != IDX_NIL)
                        ordered_bucket_at(lh, le->iterate_previous)->iterate_अग्र = le->iterate_अग्र;
                अन्यथा
                        lh->iterate_list_शीर्ष = le->iterate_अग्र;
        }

        /* Now shift all buckets in the interval (left, right) one step backwards */
        क्रम (पूर्व = बायाँ, बायाँ = अग्र_idx(h, बायाँ); बायाँ != दायाँ;
             पूर्व = बायाँ, बायाँ = अग्र_idx(h, बायाँ)) {
                dib = bucket_calculate_dib(h, बायाँ, dibs[बायाँ]);
                निश्चित(dib != 0);
                bucket_move_entry(h, शून्य, बायाँ, पूर्व);
                bucket_set_dib(h, पूर्व, dib - 1);
        }

        bucket_mark_free(h, पूर्व);
        n_entries_dec(h);
        base_set_dirty(h);
}
#घोषणा remove_entry(h, idx) base_remove_entry(HASHMAP_BASE(h), idx)

अटल अचिन्हित hashmap_iterate_in_insertion_order(OrderedHashmap *h, Iterator *i) {
        काष्ठा ordered_hashmap_entry *e;
        अचिन्हित idx;

        निश्चित(h);
        निश्चित(i);

        अगर (i->idx == IDX_NIL)
                जाओ at_end;

        अगर (i->idx == IDX_FIRST && h->iterate_list_शीर्ष == IDX_NIL)
                जाओ at_end;

        अगर (i->idx == IDX_FIRST) {
                idx = h->iterate_list_शीर्ष;
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
                अगर (e->p.b.कुंजी != i->अग्र_कुंजी) {
                        idx = पूर्व_idx(HASHMAP_BASE(h), idx);
                        e = ordered_bucket_at(h, idx);
                }
                निश्चित(e->p.b.कुंजी == i->अग्र_कुंजी);
        }

#अगर ENABLE_DEBUG_HASHMAP
        i->पूर्व_idx = idx;
#पूर्ण_अगर

        अगर (e->iterate_अग्र != IDX_NIL) {
                काष्ठा ordered_hashmap_entry *n;
                i->idx = e->iterate_अग्र;
                n = ordered_bucket_at(h, i->idx);
                i->अग्र_कुंजी = n->p.b.कुंजी;
        } अन्यथा
                i->idx = IDX_NIL;

        वापस idx;

at_end:
        i->idx = IDX_NIL;
        वापस IDX_NIL;
}

अटल अचिन्हित hashmap_iterate_in_internal_order(HashmapBase *h, Iterator *i) {
        अचिन्हित idx;

        निश्चित(h);
        निश्चित(i);

        अगर (i->idx == IDX_NIL)
                जाओ at_end;

        अगर (i->idx == IDX_FIRST) {
                /* fast forward to the first occupied bucket */
                अगर (h->has_indirect) {
                        i->idx = skip_free_buckets(h, h->indirect.idx_lowest_entry);
                        h->indirect.idx_lowest_entry = i->idx;
                } अन्यथा
                        i->idx = skip_free_buckets(h, 0);

                अगर (i->idx == IDX_NIL)
                        जाओ at_end;
        } अन्यथा {
                काष्ठा hashmap_base_entry *e;

                निश्चित(i->idx > 0);

                e = bucket_at(h, i->idx);
                /*
                 * We allow removing the current entry while iterating, but removal may cause
                 * a backward shift. The next entry may thus move one bucket to the left.
                 * To detect when it happens, we remember the key pointer of the entry we were
                 * going to iterate next. If it does not match, there was a backward shift.
                 */
                अगर (e->कुंजी != i->अग्र_कुंजी)
                        e = bucket_at(h, --i->idx);

                निश्चित(e->कुंजी == i->अग्र_कुंजी);
        }

        idx = i->idx;
#अगर ENABLE_DEBUG_HASHMAP
        i->पूर्व_idx = idx;
#पूर्ण_अगर

        i->idx = skip_free_buckets(h, i->idx + 1);
        अगर (i->idx != IDX_NIL)
                i->अग्र_कुंजी = bucket_at(h, i->idx)->कुंजी;
        अन्यथा
                i->idx = IDX_NIL;

        वापस idx;

at_end:
        i->idx = IDX_NIL;
        वापस IDX_NIL;
}

अटल अचिन्हित hashmap_iterate_entry(HashmapBase *h, Iterator *i) {
        अगर (!h) {
                i->idx = IDX_NIL;
                वापस IDX_NIL;
        }

#अगर ENABLE_DEBUG_HASHMAP
        अगर (i->idx == IDX_FIRST) {
                i->put_गिनती = h->debug.put_गिनती;
                i->rem_गिनती = h->debug.rem_गिनती;
        } अन्यथा {
                /* While iterating, must not add any new entries */
                निश्चित(i->put_गिनती == h->debug.put_गिनती);
                /* ... or remove entries other than the current one */
                निश्चित(i->rem_गिनती == h->debug.rem_गिनती ||
                       (i->rem_गिनती == h->debug.rem_गिनती - 1 &&
                        i->पूर्व_idx == h->debug.last_rem_idx));
                /* Reset our removals counter */
                i->rem_गिनती = h->debug.rem_गिनती;
        }
#पूर्ण_अगर

        वापस h->type == HASHMAP_TYPE_ORDERED ? hashmap_iterate_in_insertion_order((OrderedHashmap*) h, i)
                                               : hashmap_iterate_in_internal_order(h, i);
}

bool _hashmap_iterate(HashmapBase *h, Iterator *i, व्योम **मूल्य, स्थिर व्योम **कुंजी) {
        काष्ठा hashmap_base_entry *e;
        व्योम *तथ्य;
        अचिन्हित idx;

        idx = hashmap_iterate_entry(h, i);
        अगर (idx == IDX_NIL) {
                अगर (मूल्य)
                        *मूल्य = शून्य;
                अगर (कुंजी)
                        *कुंजी = शून्य;

                वापस false;
        }

        e = bucket_at(h, idx);
        तथ्य = entry_मूल्य(h, e);
        अगर (मूल्य)
                *मूल्य = तथ्य;
        अगर (कुंजी)
                *कुंजी = e->कुंजी;

        वापस true;
}

#घोषणा HASHMAP_FOREACH_IDX(idx, h, i) \
        क्रम ((i) = ITERATOR_FIRST, (idx) = hashmap_iterate_entry((h), &(i)); \
             (idx != IDX_NIL); \
             (idx) = hashmap_iterate_entry((h), &(i)))

IteratedCache* _hashmap_iterated_cache_new(HashmapBase *h) {
        IteratedCache *cache;

        निश्चित(h);
        निश्चित(!h->cached);

        अगर (h->cached)
                वापस शून्य;

        cache = new0(IteratedCache, 1);
        अगर (!cache)
                वापस शून्य;

        cache->hashmap = h;
        h->cached = true;

        वापस cache;
}

अटल व्योम reset_direct_storage(HashmapBase *h) {
        स्थिर काष्ठा hashmap_type_info *hi = &hashmap_type_info[h->type];
        व्योम *p;

        निश्चित(!h->has_indirect);

        p = mempset(h->direct.storage, 0, hi->entry_आकार * hi->n_direct_buckets);
        स_रखो(p, DIB_RAW_INIT, माप(dib_raw_t) * hi->n_direct_buckets);
}

अटल व्योम shared_hash_कुंजी_initialize(व्योम) {
        random_bytes(shared_hash_कुंजी, माप(shared_hash_कुंजी));
}

अटल काष्ठा HashmapBase* hashmap_base_new(स्थिर काष्ठा hash_ops *hash_ops, क्रमागत HashmapType type) {
        HashmapBase *h;
        स्थिर काष्ठा hashmap_type_info *hi = &hashmap_type_info[type];

        bool use_pool = mempool_enabled && mempool_enabled();  /* mempool_enabled is a weak symbol */

        h = use_pool ? mempool_alloc0_tile(hi->mempool) : malloc0(hi->शीर्ष_आकार);
        अगर (!h)
                वापस शून्य;

        h->type = type;
        h->from_pool = use_pool;
        h->hash_ops = hash_ops ?: &trivial_hash_ops;

        अगर (type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*)h;
                lh->iterate_list_शीर्ष = lh->iterate_list_पुच्छ = IDX_NIL;
        }

        reset_direct_storage(h);

        अटल pthread_once_t once = PTHREAD_ONCE_INIT;
        assert_se(pthread_once(&once, shared_hash_कुंजी_initialize) == 0);

#अगर ENABLE_DEBUG_HASHMAP
        assert_se(pthread_mutex_lock(&hashmap_debug_list_mutex) == 0);
        LIST_PREPEND(debug_list, hashmap_debug_list, &h->debug);
        assert_se(pthread_mutex_unlock(&hashmap_debug_list_mutex) == 0);
#पूर्ण_अगर

        वापस h;
}

Hashmap *hashmap_new(स्थिर काष्ठा hash_ops *hash_ops) {
        वापस (Hashmap*)        hashmap_base_new(hash_ops, HASHMAP_TYPE_PLAIN);
}

OrderedHashmap *ordered_hashmap_new(स्थिर काष्ठा hash_ops *hash_ops) {
        वापस (OrderedHashmap*) hashmap_base_new(hash_ops, HASHMAP_TYPE_ORDERED);
}

Set *set_new(स्थिर काष्ठा hash_ops *hash_ops) {
        वापस (Set*)            hashmap_base_new(hash_ops, HASHMAP_TYPE_SET);
}

अटल पूर्णांक hashmap_base_ensure_allocated(HashmapBase **h, स्थिर काष्ठा hash_ops *hash_ops,
                                         क्रमागत HashmapType type) {
        HashmapBase *q;

        निश्चित(h);

        अगर (*h) {
                निश्चित((*h)->hash_ops == (hash_ops ?: &trivial_hash_ops));
                वापस 0;
        }

        q = hashmap_base_new(hash_ops, type);
        अगर (!q)
                वापस -ENOMEM;

        *h = q;
        वापस 1;
}

पूर्णांक hashmap_ensure_allocated(Hashmap **h, स्थिर काष्ठा hash_ops *hash_ops) {
        वापस hashmap_base_ensure_allocated((HashmapBase**)h, hash_ops, HASHMAP_TYPE_PLAIN);
}

पूर्णांक ordered_hashmap_ensure_allocated(OrderedHashmap **h, स्थिर काष्ठा hash_ops *hash_ops) {
        वापस hashmap_base_ensure_allocated((HashmapBase**)h, hash_ops, HASHMAP_TYPE_ORDERED);
}

पूर्णांक set_ensure_allocated(Set **s, स्थिर काष्ठा hash_ops *hash_ops) {
        वापस hashmap_base_ensure_allocated((HashmapBase**)s, hash_ops, HASHMAP_TYPE_SET);
}

पूर्णांक hashmap_ensure_put(Hashmap **h, स्थिर काष्ठा hash_ops *hash_ops, स्थिर व्योम *कुंजी, व्योम *मूल्य) {
        पूर्णांक r;

        निश्चित(h);

        r = hashmap_ensure_allocated(h, hash_ops);
        अगर (r < 0)
                वापस r;

        वापस hashmap_put(*h, कुंजी, मूल्य);
}

पूर्णांक ordered_hashmap_ensure_put(OrderedHashmap **h, स्थिर काष्ठा hash_ops *hash_ops, स्थिर व्योम *कुंजी, व्योम *मूल्य) {
        पूर्णांक r;

        निश्चित(h);

        r = ordered_hashmap_ensure_allocated(h, hash_ops);
        अगर (r < 0)
                वापस r;

        वापस ordered_hashmap_put(*h, कुंजी, मूल्य);
}

पूर्णांक ordered_hashmap_ensure_replace(OrderedHashmap **h, स्थिर काष्ठा hash_ops *hash_ops, स्थिर व्योम *कुंजी, व्योम *मूल्य) {
        पूर्णांक r;

        निश्चित(h);

        r = ordered_hashmap_ensure_allocated(h, hash_ops);
        अगर (r < 0)
                वापस r;

        वापस ordered_hashmap_replace(*h, कुंजी, मूल्य);
}

पूर्णांक hashmap_ensure_replace(Hashmap **h, स्थिर काष्ठा hash_ops *hash_ops, स्थिर व्योम *कुंजी, व्योम *मूल्य) {
        पूर्णांक r;

        निश्चित(h);

        r = hashmap_ensure_allocated(h, hash_ops);
        अगर (r < 0)
                वापस r;

        वापस hashmap_replace(*h, कुंजी, मूल्य);
}

अटल व्योम hashmap_free_no_clear(HashmapBase *h) {
        निश्चित(!h->has_indirect);
        निश्चित(h->n_direct_entries == 0);

#अगर ENABLE_DEBUG_HASHMAP
        assert_se(pthread_mutex_lock(&hashmap_debug_list_mutex) == 0);
        LIST_REMOVE(debug_list, hashmap_debug_list, &h->debug);
        assert_se(pthread_mutex_unlock(&hashmap_debug_list_mutex) == 0);
#पूर्ण_अगर

        अगर (h->from_pool) {
                /* Ensure that the object didn't get migrated between threads. */
                assert_se(is_main_thread());
                mempool_free_tile(hashmap_type_info[h->type].mempool, h);
        } अन्यथा
                मुक्त(h);
}

HashmapBase* _hashmap_free(HashmapBase *h) {
        अगर (h) {
                _hashmap_clear(h);
                hashmap_free_no_clear(h);
        }

        वापस शून्य;
}

व्योम _hashmap_clear(HashmapBase *h) {
        अगर (!h)
                वापस;

        अगर (h->hash_ops->free_कुंजी || h->hash_ops->free_मूल्य) {

                /* If destructor calls are defined, let's destroy things defensively: let's take the item out of the
                 * hash table, and only then call the destructor functions. If these destructors then try to unregister
                 * themselves from our hash table a second time, the entry is already gone. */

                जबतक (hashmap_आकार(h) > 0) {
                        व्योम *k = शून्य;
                        व्योम *v;

                        v = hashmap_first_कुंजी_and_मूल्य(h, true, &k);

                        अगर (h->hash_ops->free_कुंजी)
                                h->hash_ops->free_कुंजी(k);

                        अगर (h->hash_ops->free_मूल्य)
                                h->hash_ops->free_मूल्य(v);
                }
        }

        अगर (h->has_indirect) {
                मुक्त(h->indirect.storage);
                h->has_indirect = false;
        }

        h->n_direct_entries = 0;
        reset_direct_storage(h);

        अगर (h->type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*) h;
                lh->iterate_list_शीर्ष = lh->iterate_list_पुच्छ = IDX_NIL;
        }

        base_set_dirty(h);
}

अटल पूर्णांक resize_buckets(HashmapBase *h, अचिन्हित entries_add);

/*
 * Finds an empty bucket to put an entry into, starting the scan at 'idx'.
 * Performs Robin Hood swaps as it goes. The entry to put must be placed
 * by the caller into swap slot IDX_PUT.
 * If used for in-place resizing, may leave a displaced entry in swap slot
 * IDX_PUT. Caller must rehash it next.
 * Returns: true if it left a displaced entry to rehash next in IDX_PUT,
 *          false otherwise.
 */
अटल bool hashmap_put_robin_hood(HashmapBase *h, अचिन्हित idx,
                                   काष्ठा swap_entries *swap) {
        dib_raw_t raw_dib, *dibs;
        अचिन्हित dib, distance;

#अगर ENABLE_DEBUG_HASHMAP
        निश्चित(h);
        h->debug.put_गिनती++;
#पूर्ण_अगर

        dibs = dib_raw_ptr(h);

        क्रम (distance = 0; ; distance++) {
                raw_dib = dibs[idx];
                अगर (IN_SET(raw_dib, DIB_RAW_FREE, DIB_RAW_REHASH)) {
                        अगर (raw_dib == DIB_RAW_REHASH)
                                bucket_move_entry(h, swap, idx, IDX_TMP);

                        अगर (h->has_indirect && h->indirect.idx_lowest_entry > idx)
                                h->indirect.idx_lowest_entry = idx;

                        bucket_set_dib(h, idx, distance);
                        bucket_move_entry(h, swap, IDX_PUT, idx);
                        अगर (raw_dib == DIB_RAW_REHASH) {
                                bucket_move_entry(h, swap, IDX_TMP, IDX_PUT);
                                वापस true;
                        }

                        वापस false;
                }

                dib = bucket_calculate_dib(h, idx, raw_dib);

                अगर (dib < distance) {
                        /* Found a wealthier entry. Go Robin Hood! */
                        bucket_set_dib(h, idx, distance);

                        /* swap the entries */
                        bucket_move_entry(h, swap, idx, IDX_TMP);
                        bucket_move_entry(h, swap, IDX_PUT, idx);
                        bucket_move_entry(h, swap, IDX_TMP, IDX_PUT);

                        distance = dib;
                }

                idx = अग्र_idx(h, idx);
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
अटल पूर्णांक hashmap_base_put_boldly(HashmapBase *h, अचिन्हित idx,
                                   काष्ठा swap_entries *swap, bool may_resize) {
        काष्ठा ordered_hashmap_entry *new_entry;
        पूर्णांक r;

        निश्चित(idx < n_buckets(h));

        new_entry = bucket_at_swap(swap, IDX_PUT);

        अगर (may_resize) {
                r = resize_buckets(h, 1);
                अगर (r < 0)
                        वापस r;
                अगर (r > 0)
                        idx = bucket_hash(h, new_entry->p.b.कुंजी);
        }
        निश्चित(n_entries(h) < n_buckets(h));

        अगर (h->type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*) h;

                new_entry->iterate_अग्र = IDX_NIL;
                new_entry->iterate_previous = lh->iterate_list_पुच्छ;

                अगर (lh->iterate_list_पुच्छ != IDX_NIL) {
                        काष्ठा ordered_hashmap_entry *old_पुच्छ;

                        old_पुच्छ = ordered_bucket_at(lh, lh->iterate_list_पुच्छ);
                        निश्चित(old_पुच्छ->iterate_अग्र == IDX_NIL);
                        old_पुच्छ->iterate_अग्र = IDX_PUT;
                }

                lh->iterate_list_पुच्छ = IDX_PUT;
                अगर (lh->iterate_list_शीर्ष == IDX_NIL)
                        lh->iterate_list_शीर्ष = IDX_PUT;
        }

        assert_se(hashmap_put_robin_hood(h, idx, swap) == false);

        n_entries_inc(h);
#अगर ENABLE_DEBUG_HASHMAP
        h->debug.max_entries = MAX(h->debug.max_entries, n_entries(h));
#पूर्ण_अगर

        base_set_dirty(h);

        वापस 1;
}
#घोषणा hashmap_put_boldly(h, idx, swap, may_resize) \
        hashmap_base_put_boldly(HASHMAP_BASE(h), idx, swap, may_resize)

/*
 * Returns 0 if resize is not needed.
 *         1 if successfully resized.
 *         -ENOMEM on allocation failure.
 */
अटल पूर्णांक resize_buckets(HashmapBase *h, अचिन्हित entries_add) {
        काष्ठा swap_entries swap;
        व्योम *new_storage;
        dib_raw_t *old_dibs, *new_dibs;
        स्थिर काष्ठा hashmap_type_info *hi;
        अचिन्हित idx, optimal_idx;
        अचिन्हित old_n_buckets, new_n_buckets, n_rehashed, new_n_entries;
        uint8_t new_shift;
        bool rehash_अग्र;

        निश्चित(h);

        hi = &hashmap_type_info[h->type];
        new_n_entries = n_entries(h) + entries_add;

        /* overflow? */
        अगर (_unlikely_(new_n_entries < entries_add))
                वापस -ENOMEM;

        /* For direct storage we allow 100% load, because it's tiny. */
        अगर (!h->has_indirect && new_n_entries <= hi->n_direct_buckets)
                वापस 0;

        /*
         * Load factor = n/m = 1 - (1/INV_KEEP_FREE).
         * From it follows: m = n + n/(INV_KEEP_FREE - 1)
         */
        new_n_buckets = new_n_entries + new_n_entries / (INV_KEEP_FREE - 1);
        /* overflow? */
        अगर (_unlikely_(new_n_buckets < new_n_entries))
                वापस -ENOMEM;

        old_n_buckets = n_buckets(h);

        अगर (_likely_(new_n_buckets <= old_n_buckets))
                वापस 0;

        अगर (_unlikely_(new_n_buckets > अच_पूर्णांक_उच्च / (hi->entry_आकार + माप(dib_raw_t))))
                वापस -ENOMEM;

        new_shift = log2u_round_up(MAX(
                        new_n_buckets * (hi->entry_आकार + माप(dib_raw_t)),
                        2 * माप(काष्ठा direct_storage)));

        /* Realloc storage (buckets and DIB array). */
        new_storage = पुनः_स्मृति(h->has_indirect ? h->indirect.storage : शून्य,
                              1U << new_shift);
        अगर (!new_storage)
                वापस -ENOMEM;

        /* Must upgrade direct to indirect storage. */
        अगर (!h->has_indirect) {
                स_नकल(new_storage, h->direct.storage,
                       old_n_buckets * (hi->entry_आकार + माप(dib_raw_t)));
                h->indirect.n_entries = h->n_direct_entries;
                h->indirect.idx_lowest_entry = 0;
                h->n_direct_entries = 0;
        }

        /* Get a new hash key. If we've just upgraded to indirect storage,
         * allow reusing a previously generated key. It's still a different key
         * from the shared one that we used for direct storage. */
        get_hash_कुंजी(h->indirect.hash_कुंजी, !h->has_indirect);

        h->has_indirect = true;
        h->indirect.storage = new_storage;
        h->indirect.n_buckets = (1U << new_shift) /
                                (hi->entry_आकार + माप(dib_raw_t));

        old_dibs = (dib_raw_t*)((uint8_t*) new_storage + hi->entry_आकार * old_n_buckets);
        new_dibs = dib_raw_ptr(h);

        /*
         * Move the DIB array to the new place, replacing valid DIB values with
         * DIB_RAW_REHASH to indicate all of the used buckets need rehashing.
         * Note: Overlap is not possible, because we have at least doubled the
         * number of buckets and dib_raw_t is smaller than any entry type.
         */
        क्रम (idx = 0; idx < old_n_buckets; idx++) {
                निश्चित(old_dibs[idx] != DIB_RAW_REHASH);
                new_dibs[idx] = old_dibs[idx] == DIB_RAW_FREE ? DIB_RAW_FREE
                                                              : DIB_RAW_REHASH;
        }

        /* Zero the area of newly added entries (including the old DIB area) */
        memzero(bucket_at(h, old_n_buckets),
               (n_buckets(h) - old_n_buckets) * hi->entry_आकार);

        /* The upper half of the new DIB array needs initialization */
        स_रखो(&new_dibs[old_n_buckets], DIB_RAW_INIT,
               (n_buckets(h) - old_n_buckets) * माप(dib_raw_t));

        /* Rehash entries that need it */
        n_rehashed = 0;
        क्रम (idx = 0; idx < old_n_buckets; idx++) {
                अगर (new_dibs[idx] != DIB_RAW_REHASH)
                        जारी;

                optimal_idx = bucket_hash(h, bucket_at(h, idx)->कुंजी);

                /*
                 * Not much to do if by luck the entry hashes to its current
                 * location. Just set its DIB.
                 */
                अगर (optimal_idx == idx) {
                        new_dibs[idx] = 0;
                        n_rehashed++;
                        जारी;
                }

                new_dibs[idx] = DIB_RAW_FREE;
                bucket_move_entry(h, &swap, idx, IDX_PUT);
                /* bucket_move_entry does not clear the source */
                memzero(bucket_at(h, idx), hi->entry_आकार);

                करो {
                        /*
                         * Find the new bucket for the current entry. This may make
                         * another entry homeless and load it into IDX_PUT.
                         */
                        rehash_अग्र = hashmap_put_robin_hood(h, optimal_idx, &swap);
                        n_rehashed++;

                        /* Did the current entry displace another one? */
                        अगर (rehash_अग्र)
                                optimal_idx = bucket_hash(h, bucket_at_swap(&swap, IDX_PUT)->p.b.कुंजी);
                } जबतक (rehash_अग्र);
        }

        assert_se(n_rehashed == n_entries(h));

        वापस 1;
}

/*
 * Finds an entry with a matching key
 * Returns: index of the found entry, or IDX_NIL if not found.
 */
अटल अचिन्हित base_bucket_scan(HashmapBase *h, अचिन्हित idx, स्थिर व्योम *कुंजी) {
        compare_func_t compare = h->hash_ops->compare;
        काष्ठा hashmap_base_entry *e;
        अचिन्हित dib, distance;
        dib_raw_t *dibs = dib_raw_ptr(h);

        निश्चित(idx < n_buckets(h));

        क्रम (distance = 0; ; distance++) {
                अगर (dibs[idx] == DIB_RAW_FREE)
                        वापस IDX_NIL;

                dib = bucket_calculate_dib(h, idx, dibs[idx]);

                अगर (dib < distance)
                        वापस IDX_NIL;
                अगर (dib == distance) {
                        e = bucket_at(h, idx);

                        /* Most maps compare keys by pointer. Do that here, without the indirect call. */
                        अगर (compare == trivial_compare_func ? e->कुंजी == कुंजी : compare(e->कुंजी, कुंजी) == 0)
                                वापस idx;
                }

                idx = अग्र_idx(h, idx);
        }
}
#घोषणा bucket_scan(h, idx, कुंजी) base_bucket_scan(HASHMAP_BASE(h), idx, कुंजी)

पूर्णांक hashmap_put(Hashmap *h, स्थिर व्योम *कुंजी, व्योम *मूल्य) {
        काष्ठा swap_entries swap;
        काष्ठा plain_hashmap_entry *e;
        अचिन्हित hash, idx;

        निश्चित(h);

        hash = bucket_hash(h, कुंजी);
        idx = bucket_scan(h, hash, कुंजी);
        अगर (idx != IDX_NIL) {
                e = plain_bucket_at(h, idx);
                अगर (e->मूल्य == मूल्य)
                        वापस 0;
                वापस -EEXIST;
        }

        e = &bucket_at_swap(&swap, IDX_PUT)->p;
        e->b.कुंजी = कुंजी;
        e->मूल्य = मूल्य;
        वापस hashmap_put_boldly(h, hash, &swap, true);
}

पूर्णांक set_put(Set *s, स्थिर व्योम *कुंजी) {
        काष्ठा swap_entries swap;
        काष्ठा hashmap_base_entry *e;
        अचिन्हित hash, idx;

        निश्चित(s);

        hash = bucket_hash(s, कुंजी);
        idx = bucket_scan(s, hash, कुंजी);
        अगर (idx != IDX_NIL)
                वापस 0;

        e = &bucket_at_swap(&swap, IDX_PUT)->p.b;
        e->कुंजी = कुंजी;
        वापस hashmap_put_boldly(s, hash, &swap, true);
}

पूर्णांक set_ensure_put(Set **s, स्थिर काष्ठा hash_ops *hash_ops, स्थिर व्योम *कुंजी) {
        पूर्णांक r;

        निश्चित(s);

        r = set_ensure_allocated(s, hash_ops);
        अगर (r < 0)
                वापस r;

        वापस set_put(*s, कुंजी);
}

पूर्णांक set_ensure_consume(Set **s, स्थिर काष्ठा hash_ops *hash_ops, व्योम *कुंजी) {
        पूर्णांक r;

        r = set_ensure_put(s, hash_ops, कुंजी);
        अगर (r <= 0) {
                अगर (hash_ops && hash_ops->free_कुंजी)
                        hash_ops->free_कुंजी(कुंजी);
                अन्यथा अगर (hash_ops && hash_ops->free_मूल्य)
                        /* Sets store their element in the key slot but may carry a value destructor. */
                        hash_ops->free_मूल्य(कुंजी);
                अन्यथा
                        मुक्त(कुंजी);
        }

        वापस r;
}

पूर्णांक hashmap_replace(Hashmap *h, स्थिर व्योम *कुंजी, व्योम *मूल्य) {
        काष्ठा swap_entries swap;
        काष्ठा plain_hashmap_entry *e;
        अचिन्हित hash, idx;

        निश्चित(h);

        hash = bucket_hash(h, कुंजी);
        idx = bucket_scan(h, hash, कुंजी);
        अगर (idx != IDX_NIL) {
                e = plain_bucket_at(h, idx);
#अगर ENABLE_DEBUG_HASHMAP
                /* Although the key is equal, the key pointer may have changed,
                 * and this would break our assumption for iterating. So count
                 * this operation as incompatible with iteration. */
                अगर (e->b.कुंजी != कुंजी) {
                        h->b.debug.put_गिनती++;
                        h->b.debug.rem_गिनती++;
                        h->b.debug.last_rem_idx = idx;
                }
#पूर्ण_अगर
                e->b.कुंजी = कुंजी;
                e->मूल्य = मूल्य;
                hashmap_set_dirty(h);

                वापस 0;
        }

        e = &bucket_at_swap(&swap, IDX_PUT)->p;
        e->b.कुंजी = कुंजी;
        e->मूल्य = मूल्य;
        वापस hashmap_put_boldly(h, hash, &swap, true);
}

पूर्णांक hashmap_update(Hashmap *h, स्थिर व्योम *कुंजी, व्योम *मूल्य) {
        काष्ठा plain_hashmap_entry *e;
        अचिन्हित hash, idx;

        निश्चित(h);

        hash = bucket_hash(h, कुंजी);
        idx = bucket_scan(h, hash, कुंजी);
        अगर (idx == IDX_NIL)
                वापस -ENOENT;

        e = plain_bucket_at(h, idx);
        e->मूल्य = मूल्य;
        hashmap_set_dirty(h);

        वापस 0;
}

व्योम* _hashmap_get(HashmapBase *h, स्थिर व्योम *कुंजी) {
        काष्ठा hashmap_base_entry *e;
        अचिन्हित hash, idx;

        अगर (!h)
                वापस शून्य;

        hash = bucket_hash(h, कुंजी);
        idx = bucket_scan(h, hash, कुंजी);
        अगर (idx == IDX_NIL)
                वापस शून्य;

        e = bucket_at(h, idx);
        वापस entry_मूल्य(h, e);
}

व्योम* hashmap_get2(Hashmap *h, स्थिर व्योम *कुंजी, व्योम **ret) {
        काष्ठा plain_hashmap_entry *e;
        अचिन्हित hash, idx;

        अगर (!h)
                वापस शून्य;

        hash = bucket_hash(h, कुंजी);
        idx = bucket_scan(h, hash, कुंजी);
        अगर (idx == IDX_NIL)
                वापस शून्य;

        e = plain_bucket_at(h, idx);
        अगर (ret)
                *ret = (व्योम*) e->b.कुंजी;

        वापस e->मूल्य;
}

bool _hashmap_contains(HashmapBase *h, स्थिर व्योम *कुंजी) {
        अचिन्हित hash;

        अगर (!h)
                वापस false;

        hash = bucket_hash(h, कुंजी);
        वापस bucket_scan(h, hash, कुंजी) != IDX_NIL;
}

व्योम* _hashmap_remove(HashmapBase *h, स्थिर व्योम *कुंजी) {
        काष्ठा hashmap_base_entry *e;
        अचिन्हित hash, idx;
        व्योम *तथ्य;

        अगर (!h)
                वापस शून्य;

        hash = bucket_hash(h, कुंजी);
        idx = bucket_scan(h, hash, कुंजी);
        अगर (idx == IDX_NIL)
                वापस शून्य;

        e = bucket_at(h, idx);
        तथ्य = entry_मूल्य(h, e);
        remove_entry(h, idx);

        वापस तथ्य;
}

व्योम* hashmap_remove2(Hashmap *h, स्थिर व्योम *कुंजी, व्योम **ret) {
        काष्ठा plain_hashmap_entry *e;
        अचिन्हित hash, idx;
        व्योम *तथ्य;

        अगर (!h) {
                अगर (ret)
                        *ret = शून्य;
                वापस शून्य;
        }

        hash = bucket_hash(h, कुंजी);
        idx = bucket_scan(h, hash, कुंजी);
        अगर (idx == IDX_NIL) {
                अगर (ret)
                        *ret = शून्य;
                वापस शून्य;
        }

        e = plain_bucket_at(h, idx);
        तथ्य = e->मूल्य;
        अगर (ret)
                *ret = (व्योम*) e->b.कुंजी;

        remove_entry(h, idx);

        वापस तथ्य;
}

पूर्णांक hashmap_remove_and_put(Hashmap *h, स्थिर व्योम *old_कुंजी, स्थिर व्योम *new_कुंजी, व्योम *मूल्य) {
        काष्ठा swap_entries swap;
        काष्ठा plain_hashmap_entry *e;
        अचिन्हित old_hash, new_hash, idx;

        अगर (!h)
                वापस -ENOENT;

        old_hash = bucket_hash(h, old_कुंजी);
        idx = bucket_scan(h, old_hash, old_कुंजी);
        अगर (idx == IDX_NIL)
                वापस -ENOENT;

        new_hash = bucket_hash(h, new_कुंजी);
        अगर (bucket_scan(h, new_hash, new_कुंजी) != IDX_NIL)
                वापस -EEXIST;

        remove_entry(h, idx);

        e = &bucket_at_swap(&swap, IDX_PUT)->p;
        e->b.कुंजी = new_कुंजी;
        e->मूल्य = मूल्य;
        assert_se(hashmap_put_boldly(h, new_hash, &swap, false) == 1);

        वापस 0;
}

पूर्णांक set_remove_and_put(Set *s, स्थिर व्योम *old_कुंजी, स्थिर व्योम *new_कुंजी) {
        काष्ठा swap_entries swap;
        काष्ठा hashmap_base_entry *e;
        अचिन्हित old_hash, new_hash, idx;

        अगर (!s)
                वापस -ENOENT;

        old_hash = bucket_hash(s, old_कुंजी);
        idx = bucket_scan(s, old_hash, old_कुंजी);
        अगर (idx == IDX_NIL)
                वापस -ENOENT;

        new_hash = bucket_hash(s, new_कुंजी);
        अगर (bucket_scan(s, new_hash, new_कुंजी) != IDX_NIL)
                वापस -EEXIST;

        remove_entry(s, idx);

        e = &bucket_at_swap(&swap, IDX_PUT)->p.b;
        e->कुंजी = new_कुंजी;
        assert_se(hashmap_put_boldly(s, new_hash, &swap, false) == 1);

        वापस 0;
}

पूर्णांक hashmap_remove_and_replace(Hashmap *h, स्थिर व्योम *old_कुंजी, स्थिर व्योम *new_कुंजी, व्योम *मूल्य) {
        काष्ठा swap_entries swap;
        काष्ठा plain_hashmap_entry *e;
        अचिन्हित old_hash, new_hash, idx_old, idx_new;

        अगर (!h)
                वापस -ENOENT;

        old_hash = bucket_hash(h, old_कुंजी);
        idx_old = bucket_scan(h, old_hash, old_कुंजी);
        अगर (idx_old == IDX_NIL)
                वापस -ENOENT;

        old_कुंजी = bucket_at(HASHMAP_BASE(h), idx_old)->कुंजी;

        new_hash = bucket_hash(h, new_कुंजी);
        idx_new = bucket_scan(h, new_hash, new_कुंजी);
        अगर (idx_new != IDX_NIL)
                अगर (idx_old != idx_new) {
                        remove_entry(h, idx_new);
                        /* Compensate for a possible backward shift. */
                        अगर (old_कुंजी != bucket_at(HASHMAP_BASE(h), idx_old)->कुंजी)
                                idx_old = पूर्व_idx(HASHMAP_BASE(h), idx_old);
                        निश्चित(old_कुंजी == bucket_at(HASHMAP_BASE(h), idx_old)->कुंजी);
                }

        remove_entry(h, idx_old);

        e = &bucket_at_swap(&swap, IDX_PUT)->p;
        e->b.कुंजी = new_कुंजी;
        e->मूल्य = मूल्य;
        assert_se(hashmap_put_boldly(h, new_hash, &swap, false) == 1);

        वापस 0;
}

व्योम* hashmap_remove_मूल्य(HashmapBase *h, स्थिर व्योम *कुंजी, व्योम *मूल्य) {
        काष्ठा hashmap_base_entry *e;
        अचिन्हित hash, idx;

        अगर (!h)
                वापस शून्य;

        hash = bucket_hash(h, कुंजी);
        idx = bucket_scan(h, hash, कुंजी);
        अगर (idx == IDX_NIL)
                वापस शून्य;

        e = bucket_at(h, idx);
        अगर (entry_मूल्य(h, e) != मूल्य)
                वापस शून्य;

        remove_entry(h, idx);

        वापस मूल्य;
}

अटल अचिन्हित find_first_entry(HashmapBase *h) {
        Iterator i = ITERATOR_FIRST;

        अगर (!h || !n_entries(h))
                वापस IDX_NIL;

        वापस hashmap_iterate_entry(h, &i);
}

व्योम* hashmap_first_कुंजी_and_मूल्य(HashmapBase *h, bool हटाओ, व्योम **ret_कुंजी) {
        काष्ठा hashmap_base_entry *e;
        व्योम *कुंजी, *तथ्य;
        अचिन्हित idx;

        idx = find_first_entry(h);
        अगर (idx == IDX_NIL) {
                अगर (ret_कुंजी)
                        *ret_कुंजी = शून्य;
                वापस शून्य;
        }

        e = bucket_at(h, idx);
        कुंजी = (व्योम*) e->कुंजी;
        तथ्य = entry_मूल्य(h, e);

        अगर (हटाओ)
                remove_entry(h, idx);

        अगर (ret_कुंजी)
                *ret_कुंजी = कुंजी;

        वापस तथ्य;
}

अचिन्हित hashmap_आकार(HashmapBase *h) {
        अगर (!h)
                वापस 0;

        वापस n_entries(h);
}

अचिन्हित _hashmap_buckets(HashmapBase *h) {
        अगर (!h)
                वापस 0;

        वापस n_buckets(h);
}

पूर्णांक _hashmap_merge(Hashmap *h, Hashmap *other) {
        Iterator i;
        अचिन्हित idx;

        निश्चित(h);

        HASHMAP_FOREACH_IDX(idx, HASHMAP_BASE(other), i) {
                काष्ठा plain_hashmap_entry *pe = plain_bucket_at(other, idx);
                पूर्णांक r;

                r = hashmap_put(h, pe->b.कुंजी, pe->मूल्य);
                अगर (r < 0 && r != -EEXIST)
                        वापस r;
        }

        वापस 0;
}

पूर्णांक set_merge(Set *s, Set *other) {
        Iterator i;
        अचिन्हित idx;

        निश्चित(s);

        HASHMAP_FOREACH_IDX(idx, HASHMAP_BASE(other), i) {
                काष्ठा set_entry *se = set_bucket_at(other, idx);
                पूर्णांक r;

                r = set_put(s, se->b.कुंजी);
                अगर (r < 0)
                        वापस r;
        }

        वापस 0;
}

पूर्णांक _hashmap_reserve(HashmapBase *h, अचिन्हित entries_add) {
        पूर्णांक r;

        निश्चित(h);

        r = resize_buckets(h, entries_add);
        अगर (r < 0)
                वापस r;

        वापस 0;
}

/*
 * The same as hashmap_merge(), but every new item from other is moved to h.
 * Keys already in h are skipped and stay in other.
 * Returns: 0 on success.
 *          -ENOMEM on alloc failure, in which case no move has been done.
 */
पूर्णांक _hashmap_move(HashmapBase *h, HashmapBase *other) {
        काष्ठा swap_entries swap;
        काष्ठा hashmap_base_entry *e, *n;
        Iterator i;
        अचिन्हित idx;
        पूर्णांक r;

        निश्चित(h);

        अगर (!other)
                वापस 0;

        निश्चित(other->type == h->type);

        /*
         * This reserves buckets for the worst case, where none of other's
         * entries are yet present in h. This is preferable to risking
         * an allocation failure in the middle of the moving and having to
         * rollback or return a partial result.
         */
        r = resize_buckets(h, n_entries(other));
        अगर (r < 0)
                वापस r;

        HASHMAP_FOREACH_IDX(idx, other, i) {
                अचिन्हित h_hash;

                e = bucket_at(other, idx);
                h_hash = bucket_hash(h, e->कुंजी);
                अगर (bucket_scan(h, h_hash, e->कुंजी) != IDX_NIL)
                        जारी;

                n = &bucket_at_swap(&swap, IDX_PUT)->p.b;
                n->कुंजी = e->कुंजी;
                अगर (h->type != HASHMAP_TYPE_SET)
                        ((काष्ठा plain_hashmap_entry*) n)->मूल्य =
                                ((काष्ठा plain_hashmap_entry*) e)->मूल्य;
                assert_se(hashmap_put_boldly(h, h_hash, &swap, false) == 1);

                remove_entry(other, idx);
        }

        वापस 0;
}

पूर्णांक _hashmap_move_one(HashmapBase *h, HashmapBase *other, स्थिर व्योम *कुंजी) {
        काष्ठा swap_entries swap;
        अचिन्हित h_hash, other_hash, idx;
        काष्ठा hashmap_base_entry *e, *n;
        पूर्णांक r;

        निश्चित(h);

        h_hash = bucket_hash(h, कुंजी);
        अगर (bucket_scan(h, h_hash, कुंजी) != IDX_NIL)
                वापस -EEXIST;

        अगर (!other)
                वापस -ENOENT;

        निश्चित(other->type == h->type);

        other_hash = bucket_hash(other, कुंजी);
        idx = bucket_scan(other, other_hash, कुंजी);
        अगर (idx == IDX_NIL)
                वापस -ENOENT;

        e = bucket_at(other, idx);

        n = &bucket_at_swap(&swap, IDX_PUT)->p.b;
        n->कुंजी = e->कुंजी;
        अगर (h->type != HASHMAP_TYPE_SET)
                ((काष्ठा plain_hashmap_entry*) n)->मूल्य =
                        ((काष्ठा plain_hashmap_entry*) e)->मूल्य;
        r = hashmap_put_boldly(h, h_hash, &swap, true);
        अगर (r < 0)
                वापस r;

        remove_entry(other, idx);
        वापस 0;
}

HashmapBase* _hashmap_copy(HashmapBase *h) {
        HashmapBase *copy;
        पूर्णांक r;

        निश्चित(h);

        copy = hashmap_base_new(h->hash_ops, h->type);
        अगर (!copy)
                वापस शून्य;

        चयन (h->type) {
        हाल HASHMAP_TYPE_PLAIN:
        हाल HASHMAP_TYPE_ORDERED:
                r = hashmap_merge((Hashmap*)copy, (Hashmap*)h);
                अवरोध;
        हाल HASHMAP_TYPE_SET:
                r = set_merge((Set*)copy, (Set*)h);
                अवरोध;
        शेष:
                assert_not_reached();
        }

        अगर (r < 0)
                वापस _hashmap_free(copy);

        वापस copy;
}

अक्षर** _hashmap_get_strv(HashmapBase *h) {
        अक्षर **sv;
        Iterator i;
        अचिन्हित idx, n;

        अगर (!h)
                वापस new0(अक्षर*, 1);

        sv = new(अक्षर*, n_entries(h)+1);
        अगर (!sv)
                वापस शून्य;

        n = 0;
        HASHMAP_FOREACH_IDX(idx, h, i)
                sv[n++] = entry_मूल्य(h, bucket_at(h, idx));
        sv[n] = शून्य;

        वापस sv;
}

अक्षर** set_to_strv(Set **s) {
        निश्चित(s);

        /* This is similar to set_get_strv(), but invalidates the set on success. */

        अक्षर **v = new(अक्षर*, set_आकार(*s) + 1);
        अगर (!v)
                वापस शून्य;

        क्रम (अक्षर **p = v; (*p = set_steal_first(*s)); p++)
                ;

        निश्चित(set_isempty(*s));
        *s = set_free(*s);
        वापस v;
}

व्योम* ordered_hashmap_अग्र(OrderedHashmap *h, स्थिर व्योम *कुंजी) {
        काष्ठा ordered_hashmap_entry *e;
        अचिन्हित hash, idx;

        अगर (!h)
                वापस शून्य;

        hash = bucket_hash(h, कुंजी);
        idx = bucket_scan(h, hash, कुंजी);
        अगर (idx == IDX_NIL)
                वापस शून्य;

        e = ordered_bucket_at(h, idx);
        अगर (e->iterate_अग्र == IDX_NIL)
                वापस शून्य;
        वापस ordered_bucket_at(h, e->iterate_अग्र)->p.मूल्य;
}

पूर्णांक set_consume(Set *s, व्योम *मूल्य) {
        पूर्णांक r;

        निश्चित(s);
        निश्चित(मूल्य);

        r = set_put(s, मूल्य);
        अगर (r <= 0)
                मुक्त(मूल्य);

        वापस r;
}

पूर्णांक hashmap_put_strdup_full(Hashmap **h, स्थिर काष्ठा hash_ops *hash_ops, स्थिर अक्षर *k, स्थिर अक्षर *v) {
        पूर्णांक r;

        निश्चित(h);

        r = hashmap_ensure_allocated(h, hash_ops);
        अगर (r < 0)
                वापस r;

        _cleanup_free_ अक्षर *kdup = शून्य, *vdup = शून्य;

        kdup = strdup(k);
        अगर (!kdup)
                वापस -ENOMEM;

        अगर (v) {
                vdup = strdup(v);
                अगर (!vdup)
                        वापस -ENOMEM;
        }

        r = hashmap_put(*h, kdup, vdup);
        अगर (r < 0) {
                अगर (r == -EEXIST && streq_ptr(v, hashmap_get(*h, kdup)))
                        वापस 0;
                वापस r;
        }

        /* 0 with non-null vdup would mean vdup is already in the hashmap, which cannot be */
        निश्चित(vdup == शून्य || r > 0);
        अगर (r > 0)
                kdup = vdup = शून्य;

        वापस r;
}

पूर्णांक set_put_strndup_full(Set **s, स्थिर काष्ठा hash_ops *hash_ops, स्थिर अक्षर *p, माप_प्रकार n) {
        अक्षर *c;
        पूर्णांक r;

        निश्चित(s);
        निश्चित(p);

        r = set_ensure_allocated(s, hash_ops);
        अगर (r < 0)
                वापस r;

        अगर (n == SIZE_MAX) {
                अगर (set_contains(*s, (अक्षर*) p))
                        वापस 0;

                c = strdup(p);
        } अन्यथा
                c = strndup(p, n);
        अगर (!c)
                वापस -ENOMEM;

        वापस set_consume(*s, c);
}

पूर्णांक set_put_strdupv_full(Set **s, स्थिर काष्ठा hash_ops *hash_ops, अक्षर **l) {
        पूर्णांक n = 0, r;

        निश्चित(s);

        STRV_FOREACH(i, l) {
                r = set_put_strndup_full(s, hash_ops, *i, SIZE_MAX);
                अगर (r < 0)
                        वापस r;

                n += r;
        }

        वापस n;
}

पूर्णांक set_put_strsplit(Set *s, स्थिर अक्षर *v, स्थिर अक्षर *separators, ExtractFlags flags) {
        स्थिर अक्षर *p = ASSERT_PTR(v);
        पूर्णांक r;

        निश्चित(s);

        क्रम (;;) {
                अक्षर *word;

                r = extract_first_word(&p, &word, separators, flags);
                अगर (r <= 0)
                        वापस r;

                r = set_consume(s, word);
                अगर (r < 0)
                        वापस r;
        }
}

/* expand the cachemem if needed, return true if newly (re)activated. */
अटल पूर्णांक cachemem_maintain(CacheMem *mem, माप_प्रकार आकार) {
        निश्चित(mem);

        अगर (!GREEDY_REALLOC(mem->ptr, आकार)) {
                अगर (आकार > 0)
                        वापस -ENOMEM;
        }

        अगर (!mem->active) {
                mem->active = true;
                वापस true;
        }

        वापस false;
}

पूर्णांक iterated_cache_get(IteratedCache *cache, स्थिर व्योम ***res_कुंजियाँ, स्थिर व्योम ***res_मूल्यावली, अचिन्हित *res_n_entries) {
        bool sync_कुंजियाँ = false, sync_मूल्यावली = false;
        माप_प्रकार आकार;
        पूर्णांक r;

        निश्चित(cache);
        निश्चित(cache->hashmap);

        आकार = n_entries(cache->hashmap);

        अगर (res_कुंजियाँ) {
                r = cachemem_maintain(&cache->कुंजियाँ, आकार);
                अगर (r < 0)
                        वापस r;

                sync_कुंजियाँ = r;
        } अन्यथा
                cache->कुंजियाँ.active = false;

        अगर (res_मूल्यावली) {
                r = cachemem_maintain(&cache->values, आकार);
                अगर (r < 0)
                        वापस r;

                sync_मूल्यावली = r;
        } अन्यथा
                cache->values.active = false;

        अगर (cache->hashmap->dirty) {
                अगर (cache->कुंजियाँ.active)
                        sync_कुंजियाँ = true;
                अगर (cache->values.active)
                        sync_मूल्यावली = true;

                cache->hashmap->dirty = false;
        }

        अगर (sync_कुंजियाँ || sync_मूल्यावली) {
                अचिन्हित i, idx;
                Iterator iter;

                i = 0;
                HASHMAP_FOREACH_IDX(idx, cache->hashmap, iter) {
                        काष्ठा hashmap_base_entry *e;

                        e = bucket_at(cache->hashmap, idx);

                        अगर (sync_कुंजियाँ)
                                cache->कुंजियाँ.ptr[i] = e->कुंजी;
                        अगर (sync_मूल्यावली)
                                cache->values.ptr[i] = entry_मूल्य(cache->hashmap, e);
                        i++;
                }
        }

        अगर (res_कुंजियाँ)
                *res_कुंजियाँ = cache->कुंजियाँ.ptr;
        अगर (res_मूल्यावली)
                *res_मूल्यावली = cache->values.ptr;
        अगर (res_n_entries)
                *res_n_entries = आकार;

        वापस 0;
}

IteratedCache* iterated_cache_free(IteratedCache *cache) {
        अगर (cache) {
                मुक्त(cache->कुंजियाँ.ptr);
                मुक्त(cache->values.ptr);
        }

        वापस mfree(cache);
}

पूर्णांक set_strjoin(Set *s, स्थिर अक्षर *separator, bool wrap_with_separator, अक्षर **ret) {
        _cleanup_free_ अक्षर *str = शून्य;
        माप_प्रकार separator_len, len = 0;
        स्थिर अक्षर *मूल्य;
        bool first;

        निश्चित(ret);

        अगर (set_isempty(s)) {
                *ret = शून्य;
                वापस 0;
        }

        separator_len = strlen_ptr(separator);

        अगर (separator_len == 0)
                wrap_with_separator = false;

        first = !wrap_with_separator;

        SET_FOREACH(मूल्य, s) {
                माप_प्रकार l = strlen_ptr(मूल्य);

                अगर (l == 0)
                        जारी;

                अगर (!GREEDY_REALLOC(str, len + l + (first ? 0 : separator_len) + (wrap_with_separator ? separator_len : 0) + 1))
                        वापस -ENOMEM;

                अगर (separator_len > 0 && !first) {
                        स_नकल(str + len, separator, separator_len);
                        len += separator_len;
                }

                स_नकल(str + len, मूल्य, l);
                len += l;
                first = false;
        }

        अगर (wrap_with_separator) {
                स_नकल(str + len, separator, separator_len);
                len += separator_len;
        }

        str[len] = '\0';

        *ret = TAKE_PTR(str);
        वापस 0;
}

bool set_equal(Set *a, Set *b) {
        व्योम *p;

        /* Checks whether each entry of 'a' is also in 'b' and vice versa, i.e. the two sets contain the same
         * entries */

        अगर (a == b)
                वापस true;

        अगर (set_isempty(a) && set_isempty(b))
                वापस true;

        अगर (set_आकार(a) != set_आकार(b)) /* Cheap check that hopefully catches a lot of inequality cases
                                         * already */
                वापस false;

        SET_FOREACH(p, a)
                अगर (!set_contains(b, p))
                        वापस false;

        /* If we have the same hashops, then we don't need to check things backwards given we compared the
         * size and that all of a is in b. */
        अगर (a->b.hash_ops == b->b.hash_ops)
                वापस true;

        SET_FOREACH(p, b)
                अगर (!set_contains(a, p))
                        वापस false;

        वापस true;
}

अटल bool set_fnmatch_one(Set *patterns, स्थिर अक्षर *needle) {
        स्थिर अक्षर *p;

        निश्चित(needle);

        /* Any failure of fnmatch() is treated as equivalent to FNM_NOMATCH, i.e. as non-matching pattern */

        SET_FOREACH(p, patterns)
                अगर (fnmatch(p, needle, 0) == 0)
                        वापस true;

        वापस false;
}

bool set_fnmatch(Set *include_patterns, Set *exclude_patterns, स्थिर अक्षर *needle) {
        निश्चित(needle);

        अगर (set_fnmatch_one(exclude_patterns, needle))
                वापस false;

        अगर (set_isempty(include_patterns))
                वापस true;

        वापस set_fnmatch_one(include_patterns, needle);
}

अटल पूर्णांक hashmap_entry_compare(
                काष्ठा hashmap_base_entry * स्थिर *a,
                काष्ठा hashmap_base_entry * स्थिर *b,
                compare_func_t compare) {

        निश्चित(a && *a);
        निश्चित(b && *b);
        निश्चित(compare);

        वापस compare((*a)->कुंजी, (*b)->कुंजी);
}

अटल पूर्णांक _hashmap_dump_entries_sorted(
                HashmapBase *h,
                व्योम ***ret,
                माप_प्रकार *ret_n) {
        _cleanup_free_ व्योम **entries = शून्य;
        Iterator iter;
        अचिन्हित idx;
        माप_प्रकार n = 0;

        निश्चित(ret);
        निश्चित(ret_n);

        अगर (hashmap_आकार(h) == 0) {
                *ret = शून्य;
                *ret_n = 0;
                वापस 0;
        }

        /* We append one more element than needed so that the resulting array can be used as a strv. We
         * don't count this entry in the returned size. */
        entries = new(व्योम*, hashmap_आकार(h) + 1);
        अगर (!entries)
                वापस -ENOMEM;

        HASHMAP_FOREACH_IDX(idx, h, iter)
                entries[n++] = bucket_at(h, idx);

        निश्चित(n == hashmap_आकार(h));
        entries[n] = शून्य;

        typesafe_qsort_r((काष्ठा hashmap_base_entry**) entries, n,
                         hashmap_entry_compare, h->hash_ops->compare);

        *ret = TAKE_PTR(entries);
        *ret_n = n;
        वापस 0;
}

पूर्णांक hashmap_dump_कुंजियाँ_sorted(HashmapBase *h, व्योम ***ret, माप_प्रकार *ret_n) {
        _cleanup_free_ व्योम **entries = शून्य;
        माप_प्रकार n;
        पूर्णांक r;

        निश्चित(ret);

        r = _hashmap_dump_entries_sorted(h, &entries, &n);
        अगर (r < 0)
                वापस r;

        /* Reuse the array. */
        FOREACH_ARRAY(e, entries, n)
                *e = (व्योम*) (*(काष्ठा hashmap_base_entry**) e)->कुंजी;

        *ret = TAKE_PTR(entries);
        अगर (ret_n)
                *ret_n = n;
        वापस 0;
}

पूर्णांक _hashmap_dump_sorted(HashmapBase *h, व्योम ***ret, माप_प्रकार *ret_n) {
        _cleanup_free_ व्योम **entries = शून्य;
        माप_प्रकार n;
        पूर्णांक r;

        निश्चित(ret);

        r = _hashmap_dump_entries_sorted(h, &entries, &n);
        अगर (r < 0)
                वापस r;

        /* Reuse the array. */
        FOREACH_ARRAY(e, entries, n)
                *e = entry_मूल्य(h, *(काष्ठा hashmap_base_entry**) e);

        *ret = TAKE_PTR(entries);
        अगर (ret_n)
                *ret_n = n;
        वापस 0;
}
