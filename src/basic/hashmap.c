/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <fnmatch.h>
#अन्तर्भाव <pthread.h>
#अन्तर्भाव <unistd.h>
#यदि HAVE_VALGRIND_VALGRIND_H
#  include <valgrind/valgrind.h>
#यदि_अन्तः

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
#यदि_अन्तः

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
#परिभाषय INV_KEEP_FREE            5U

/* Fields common to entries of all hashmap/set types */
संरचना hashmap_आधारः_प्रविष्टिः {
        ध्रुव शून्य *कुञ्चिका;
};

/* Entry types for specific hashmap/set types
 * hashmap_base_entry must be at the beginning of each entry struct. */

संरचना plain_hashmap_प्रविष्टिः {
        संरचना hashmap_आधारः_प्रविष्टिः b;
        शून्य *मानम्;
};

संरचना ordered_hashmap_प्रविष्टिः {
        संरचना plain_hashmap_प्रविष्टिः p;
        अचिह्नित iterate_अग्र, iterate_previous;
};

संरचना समुच्चयः_प्रविष्टिः {
        संरचना hashmap_आधारः_प्रविष्टिः b;
};

/* In several functions it is advantageous to have the hash table extended
 * virtually by a couple of additional buckets. We reserve special index values
 * for these "swap" buckets. */
#परिभाषय _IDX_SWAP_BEGIN     (UINT_MAX - 3)
#परिभाषय IDX_PUT             (_IDX_SWAP_BEGIN + 0)
#परिभाषय IDX_TMP             (_IDX_SWAP_BEGIN + 1)
#परिभाषय IDX_SWAP_अन्तः       (_IDX_SWAP_BEGIN + 2)

#परिभाषय IDX_FIRST           (UINT_MAX - 1) /* special index for freshly initialized iterators */
#परिभाषय IDX_NIL             UINT_MAX       /* special index value meaning "none" or "end" */

assert_cc(IDX_FIRST == IDX_SWAP_अन्तः);
assert_cc(IDX_FIRST == _IDX_ITERATOR_FIRST);

/* Storage space for the "swap" buckets.
 * All entry types can fit into an ordered_hashmap_entry. */
संरचना swap_entries {
        संरचना ordered_hashmap_प्रविष्टिः e[IDX_SWAP_अन्तः - _IDX_SWAP_BEGIN];
};

/* Distance from Initial Bucket */
प्रकारनिर्देश uint8_t dib_raw_t;
#परिभाषय DIB_RAW_अतिप्रवाहः ((dib_raw_t)0xfdU)   /* indicates DIB value is greater than representable */
#परिभाषय DIB_RAW_REHASH   ((dib_raw_t)0xfeU)   /* entry yet to be rehashed during in-place resize */
#परिभाषय DIB_RAW_FREE     ((dib_raw_t)0xffU)   /* a free bucket */
#परिभाषय DIB_RAW_INIT     ((वर्ण)DIB_RAW_FREE) /* a byte to memset a DIB store with when initializing */

#परिभाषय DIB_FREE UINT_MAX

#यदि ENABLE_DEBUG_HASHMAP
संरचना hashmap_debug_info {
        LIST_FIELDS(संरचना hashmap_debug_info, debug_list);
        अचिह्नित max_entries;  /* high watermark of n_entries */

        /* fields to detect modification while iterating */
        अचिह्नित put_गणनम्;    /* counts puts into the hashmap */
        अचिह्नित rem_गणनम्;    /* counts removals from hashmap */
        अचिह्नित चरमः_rem_idx; /* remembers last removal index */
};

/* Tracks all existing hashmaps. Get at it from gdb. See sd_dump_hashmaps.py */
स्थिर LIST_शीर्ष(संरचना hashmap_debug_info, hashmap_debug_list);
स्थिर pthread_mutex_t hashmap_debug_list_mutex = PTHREAD_MUTEX_INITIALIZER;
#यदि_अन्तः

गणना HashmapType {
        HASHMAP_TYPE_PLAIN,
        HASHMAP_TYPE_ORDERED,
        HASHMAP_TYPE_समुच्चयः,
        _HASHMAP_TYPE_MAX
};

संरचना _packed_ परोक्षम्_storage {
        शून्य *storage;                     /* where buckets and DIBs are stored */
        uint8_t  hash_कुञ्चिका[HASH_कुञ्चिका_आकार];  /* hash key; changes during resize */

        अचिह्नित n_entries;                /* number of stored entries */
        अचिह्नित n_buckets;                /* number of buckets */

        अचिह्नित idx_निम्नतमम्_प्रविष्टिः;         /* Index below which all buckets are free.
                                              Makes "while (hashmap_steal_first())" loops
                                              O(n) instead of O(n^2) for unordered hashmaps. */
        uint8_t  _pad[3];                  /* padding for the whole HashmapBase */
        /* The bitfields in HashmapBase complete the alignment of the whole thing. */
};

संरचना प्रत्यक्षम्_storage {
        /* This gives us 39 bytes on 64-bit, or 35 bytes on 32-bit.
         * That's room for 4 set_entries + 4 DIB bytes + 3 unused bytes on 64-bit,
         *              or 7 set_entries + 7 DIB bytes + 0 unused bytes on 32-bit. */
        uint8_t storage[परिमाण(संरचना परोक्षम्_storage)];
};

#परिभाषय प्रत्यक्षम्_BUCKETS(प्रविष्टिः_t) \
        (परिमाण(संरचना प्रत्यक्षम्_storage) / (परिमाण(प्रविष्टिः_t) + परिमाण(dib_raw_t)))

/* We should be able to store at least one entry directly. */
assert_cc(प्रत्यक्षम्_BUCKETS(संरचना ordered_hashmap_प्रविष्टिः) >= 1);

/* We have 3 bits for n_direct_entries. */
assert_cc(प्रत्यक्षम्_BUCKETS(संरचना समुच्चयः_प्रविष्टिः) < (1 << 3));

/* Hashmaps with directly stored entries all use this shared hash key.
 * It's no big deal if the key is guessed, because there can be only
 * a handful of directly stored entries in a hashmap. When a hashmap
 * outgrows direct storage, it gets its own key for indirect storage. */
स्थिर uint8_t shared_hash_कुञ्चिका[HASH_कुञ्चिका_आकार];

/* Fields that all hashmap/set types must have */
संरचना Hashmap_आधारः {
        ध्रुव संरचना hash_ops *hash_ops;  /* hash and compare ops to use */

        सङ्घ _packed_ {
                संरचना परोक्षम्_storage परोक्षम्; /* if  has_indirect */
                संरचना प्रत्यक्षम्_storage प्रत्यक्षम्;     /* if !has_indirect */
        };

        गणना HashmapType type:2;     /* HASHMAP_TYPE_* */
        bool has_परोक्षम्:1;         /* whether indirect storage is used */
        अचिह्नित n_प्रत्यक्षम्_entries:3; /* Number of entries in direct storage.
                                      * Only valid if !has_indirect. */
        bool from_pool:1;            /* whether was allocated from mempool */
        bool dirty:1;                /* whether dirtied since last iterated_cache_get() */
        bool cached:1;               /* whether this hashmap is being cached */

#यदि ENABLE_DEBUG_HASHMAP
        संरचना hashmap_debug_info debug;
#यदि_अन्तः
};

/* Specific hash types
 * HashmapBase must be at the beginning of each hashmap struct. */

संरचना Hashmap {
        संरचना Hashmap_आधारः b;
};

संरचना OrderedHashmap {
        संरचना Hashmap_आधारः b;
        अचिह्नित iterate_list_शीर्ष, iterate_list_पुच्छ;
};

संरचना समुच्चयः_2 {
        संरचना Hashmap_आधारः b;
};

प्रकारनिर्देश संरचना CacheMem {
        ध्रुव शून्य **ptr;
        size_t n_populated;
        bool सक्रिय:1;
} CacheMem;

संरचना IteratedCache {
        Hashmap_आधारः *hashmap;
        CacheMem कुञ्चिकाः, values;
};

DEFINE_MEMPOOL(hashmap_pool,         Hashmap,        8);
DEFINE_MEMPOOL(ordered_hashmap_pool, OrderedHashmap, 8);
/* No need for a separate Set pool */
assert_cc(परिमाण(Hashmap) == परिमाण(समुच्चयः_2));

संरचना hashmap_type_info {
        size_t शीर्ष_आकार;
        size_t प्रविष्टिः_आकार;
        संरचना mempool *mempool;
        अचिह्नित n_प्रत्यक्षम्_buckets;
};

स्थिर प्रयुक्तम्_2 ध्रुव संरचना hashmap_type_info hashmap_type_info[_HASHMAP_TYPE_MAX] = {
        [HASHMAP_TYPE_PLAIN] = {
                .शीर्ष_आकार        = परिमाण(Hashmap),
                .प्रविष्टिः_आकार       = परिमाण(संरचना plain_hashmap_प्रविष्टिः),
                .mempool          = &hashmap_pool,
                .n_प्रत्यक्षम्_buckets = प्रत्यक्षम्_BUCKETS(संरचना plain_hashmap_प्रविष्टिः),
        },
        [HASHMAP_TYPE_ORDERED] = {
                .शीर्ष_आकार        = परिमाण(OrderedHashmap),
                .प्रविष्टिः_आकार       = परिमाण(संरचना ordered_hashmap_प्रविष्टिः),
                .mempool          = &ordered_hashmap_pool,
                .n_प्रत्यक्षम्_buckets = प्रत्यक्षम्_BUCKETS(संरचना ordered_hashmap_प्रविष्टिः),
        },
        [HASHMAP_TYPE_समुच्चयः] = {
                .शीर्ष_आकार        = परिमाण(समुच्चयः_2),
                .प्रविष्टिः_आकार       = परिमाण(संरचना समुच्चयः_प्रविष्टिः),
                .mempool          = &hashmap_pool,
                .n_प्रत्यक्षम्_buckets = प्रत्यक्षम्_BUCKETS(संरचना समुच्चयः_प्रविष्टिः),
        },
};

शून्य hashmap_trim_pools(शून्य) {
        पूर्णाङ्क r;

        /* The pool is only allocated by the main thread, but the memory can be passed to other
         * threads. Let's clean up if we are the main thread and no other threads are live. */

        /* We build our own is_main_thread() here, which doesn't use C11 TLS based caching of the
         * result. That's because valgrind apparently doesn't like TLS to be used from a GCC destructor. */
        यदि (getpid() != gettid())
                प्रत्यागम (शून्य) log_debug("Not cleaning up memory pools, not in main thread.");

        r = get_process_तन्तवः(0);
        यदि (r < 0)
                प्रत्यागम (शून्य) log_debug_errno(r, "Failed to determine number of threads, not cleaning up memory pools: %m");
        यदि (r != 1)
                प्रत्यागम (शून्य) log_debug("Not cleaning up memory pools, running in multi-threaded process.");

        mempool_trim(&hashmap_pool);
        mempool_trim(&ordered_hashmap_pool);
}

#यदि HAVE_VALGRIND_VALGRIND_H
_destructor_ स्थिर शून्य cleanup_pools(शून्य) {
        /* Be nice to valgrind */
        यदि (RUNNING_ON_VALGRIND)
                hashmap_trim_pools();
}
#यदि_अन्तः

स्थिर अन्तःस्थ अचिह्नित n_buckets(Hashmap_आधारः *h) {
        प्रतिज्ञा(h);
        प्रत्यागम h->has_परोक्षम् ? h->परोक्षम्.n_buckets
                               : hashmap_type_info[h->type].n_प्रत्यक्षम्_buckets;
}

स्थिर अचिह्नित n_entries(Hashmap_आधारः *h) {
        प्रतिज्ञा(h);
        प्रत्यागम h->has_परोक्षम् ? h->परोक्षम्.n_entries
                               : h->n_प्रत्यक्षम्_entries;
}

स्थिर शून्य n_entries_inc(Hashmap_आधारः *h) {
        प्रतिज्ञा(h);

        यदि (h->has_परोक्षम्)
                h->परोक्षम्.n_entries++;
        अन्यथा
                h->n_प्रत्यक्षम्_entries++;
}

स्थिर शून्य n_entries_dec(Hashmap_आधारः *h) {
        प्रतिज्ञा(h);

        यदि (h->has_परोक्षम्)
                h->परोक्षम्.n_entries--;
        अन्यथा
                h->n_प्रत्यक्षम्_entries--;
}

स्थिर अन्तःस्थ शून्य* storage_ptr(Hashmap_आधारः *h) {
        प्रतिज्ञा(h);
        प्रत्यागम h->has_परोक्षम् ? h->परोक्षम्.storage
                               : h->प्रत्यक्षम्.storage;
}

स्थिर अन्तःस्थ uint8_t* hash_कुञ्चिका(Hashmap_आधारः *h) {
        प्रतिज्ञा(h);
        प्रत्यागम h->has_परोक्षम् ? h->परोक्षम्.hash_कुञ्चिका
                               : shared_hash_कुञ्चिका;
}

स्थिर अचिह्नित आधारः_bucket_hash(Hashmap_आधारः *h, ध्रुव शून्य *p) {
        संरचना siphash state;
        uint64_t hash;

        प्रतिज्ञा(h);

        /* The keys of trivial_hash_ops and uint64_hash_ops fit in 64 bits. siphash24_uint64() hashes such
         * a key without the indirect call through hash_ops and keeps the SipHash state in registers. On
         * 32-bit and big-endian architectures, the result differs from the result of trivial_hash_func(),
         * which hashes the key in native byte order. Only this function maps keys to buckets, so the
         * difference does not matter. */
        यदि (h->hash_ops->hash == trivial_hash_func)
                hash = siphash24_uint64((uintptr_t) p, hash_कुञ्चिका(h));
        अन्यथा यदि (h->hash_ops->hash == (hash_func_t) uint64_hash_func)
                hash = siphash24_uint64(*(ध्रुव uint64_t*) p, hash_कुञ्चिका(h));
        अन्यथा {
                siphash24_init(&state, hash_कुञ्चिका(h));
                h->hash_ops->hash(p, &state);
                hash = siphash24_finalize(&state);
        }

        /* n_buckets is not a power of two, so a modulo needs a division. Multiplying the upper 32 bits of
         * the hash with n_buckets and keeping the upper 32 bits of the product maps the hash to
         * [0, n_buckets) almost as evenly, without a division. */
        प्रत्यागम (अचिह्नित) (((hash >> 32) * n_buckets(h)) >> 32);
}
#परिभाषय bucket_hash(h, p) आधारः_bucket_hash(HASHMAP_आधारः(h), p)

स्थिर शून्य आधारः_समुच्चयः_dirty(Hashmap_आधारः *h) {
        प्रतिज्ञा(h);

        h->dirty = true;
}
#परिभाषय hashmap_समुच्चयः_dirty(h) आधारः_समुच्चयः_dirty(HASHMAP_आधारः(h))

स्थिर शून्य get_hash_कुञ्चिका(uint8_t hash_कुञ्चिका[HASH_कुञ्चिका_आकार], bool reuse_is_ok) {
        स्थिर uint8_t current[HASH_कुञ्चिका_आकार];
        स्थिर bool current_initialized = false;

        /* Returns a hash function key to use. In order to keep things
         * fast we will not generate a new key each time we allocate a
         * new hash table. Instead, we'll just reuse the most recently
         * generated one, except if we never generated one or when we
         * are rehashing an entire hash table because we reached a
         * fill level */

        यदि (!current_initialized || !reuse_is_ok) {
                random_अष्टकानि(current, परिमाण(current));
                current_initialized = true;
        }

        स्मृतिं_प्रतिलिख(hash_कुञ्चिका, current, परिमाण(current));
}

स्थिर अन्तःस्थ संरचना hashmap_आधारः_प्रविष्टिः* bucket_at(Hashmap_आधारः *h, अचिह्नित idx) {
        प्रतिज्ञा(h);
        प्रत्यागम CAST_ALIGN_PTR(
                        संरचना hashmap_आधारः_प्रविष्टिः,
                        (uint8_t *) storage_ptr(h) + idx * hashmap_type_info[h->type].प्रविष्टिः_आकार);
}

स्थिर संरचना plain_hashmap_प्रविष्टिः* plain_bucket_at(Hashmap *h, अचिह्नित idx) {
        प्रत्यागम (संरचना plain_hashmap_प्रविष्टिः*) bucket_at(HASHMAP_आधारः(h), idx);
}

स्थिर संरचना ordered_hashmap_प्रविष्टिः* ordered_bucket_at(OrderedHashmap *h, अचिह्नित idx) {
        प्रत्यागम (संरचना ordered_hashmap_प्रविष्टिः*) bucket_at(HASHMAP_आधारः(h), idx);
}

स्थिर संरचना समुच्चयः_प्रविष्टिः *समुच्चयः_bucket_at(समुच्चयः_2 *h, अचिह्नित idx) {
        प्रत्यागम (संरचना समुच्चयः_प्रविष्टिः*) bucket_at(HASHMAP_आधारः(h), idx);
}

स्थिर संरचना ordered_hashmap_प्रविष्टिः* bucket_at_swap(संरचना swap_entries *swap, अचिह्नित idx) {
        प्रतिज्ञा(swap);
        प्रत्यागम &swap->e[idx - _IDX_SWAP_BEGIN];
}

/* Returns a pointer to the bucket at index idx.
 * Understands real indexes and swap indexes, hence "_virtual". */
स्थिर संरचना hashmap_आधारः_प्रविष्टिः* bucket_at_virtual(Hashmap_आधारः *h, संरचना swap_entries *swap,
                                                    अचिह्नित idx) {
        यदि (idx < _IDX_SWAP_BEGIN)
                प्रत्यागम bucket_at(h, idx);

        यदि (idx < IDX_SWAP_अन्तः)
                प्रत्यागम &bucket_at_swap(swap, idx)->p.b;

        assert_not_reached();
}

स्थिर अन्तःस्थ dib_raw_t* dib_raw_ptr(Hashmap_आधारः *h) {
        प्रतिज्ञा(h);
        प्रत्यागम (dib_raw_t*)
                ((uint8_t*) storage_ptr(h) + hashmap_type_info[h->type].प्रविष्टिः_आकार * n_buckets(h));
}

स्थिर अचिह्नित bucket_distance(Hashmap_आधारः *h, अचिह्नित idx, अचिह्नित from) {
        प्रत्यागम idx >= from ? idx - from
                           : n_buckets(h) + idx - from;
}

स्थिर अचिह्नित bucket_calculate_dib(Hashmap_आधारः *h, अचिह्नित idx, dib_raw_t raw_dib) {
        अचिह्नित initial_bucket;

        यदि (raw_dib == DIB_RAW_FREE)
                प्रत्यागम DIB_FREE;

        यदि (_likely_(raw_dib < DIB_RAW_अतिप्रवाहः))
                प्रत्यागम raw_dib;

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
        प्रत्यागम bucket_distance(h, idx, initial_bucket);
}

स्थिर शून्य bucket_समुच्चयः_dib(Hashmap_आधारः *h, अचिह्नित idx, अचिह्नित dib) {
        dib_raw_ptr(h)[idx] = dib != DIB_FREE ? MIN(dib, DIB_RAW_अतिप्रवाहः) : DIB_RAW_FREE;
}

स्थिर अचिह्नित skip_free_buckets(Hashmap_आधारः *h, अचिह्नित idx) {
        dib_raw_t *dibs;

        dibs = dib_raw_ptr(h);

        प्रति ( ; idx < n_buckets(h); idx++)
                यदि (dibs[idx] != DIB_RAW_FREE)
                        प्रत्यागम idx;

        प्रत्यागम IDX_NIL;
}

स्थिर शून्य bucket_mark_free(Hashmap_आधारः *h, अचिह्नित idx) {
        प्रतिज्ञा(h);

        memzero(bucket_at(h, idx), hashmap_type_info[h->type].प्रविष्टिः_आकार);
        bucket_समुच्चयः_dib(h, idx, DIB_FREE);
}

स्थिर शून्य bucket_move_प्रविष्टिः(Hashmap_आधारः *h, संरचना swap_entries *swap,
                              अचिह्नित from, अचिह्नित to) {
        संरचना hashmap_आधारः_प्रविष्टिः *e_from, *e_गन्तव्यम्;

        प्रतिज्ञा(h);
        प्रतिज्ञा(from != to);

        e_from = bucket_at_virtual(h, swap, from);
        e_गन्तव्यम्   = bucket_at_virtual(h, swap, to);

        स्मृतिं_प्रतिलिख(e_गन्तव्यम्, e_from, hashmap_type_info[h->type].प्रविष्टिः_आकार);

        यदि (h->type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*) h;
                संरचना ordered_hashmap_प्रविष्टिः *le, *le_गन्तव्यम्;

                le_गन्तव्यम् = (संरचना ordered_hashmap_प्रविष्टिः*) e_गन्तव्यम्;

                यदि (le_गन्तव्यम्->iterate_अग्र != IDX_NIL) {
                        le = (संरचना ordered_hashmap_प्रविष्टिः*)
                             bucket_at_virtual(h, swap, le_गन्तव्यम्->iterate_अग्र);
                        le->iterate_previous = to;
                }

                यदि (le_गन्तव्यम्->iterate_previous != IDX_NIL) {
                        le = (संरचना ordered_hashmap_प्रविष्टिः*)
                             bucket_at_virtual(h, swap, le_गन्तव्यम्->iterate_previous);
                        le->iterate_अग्र = to;
                }

                यदि (lh->iterate_list_शीर्ष == from)
                        lh->iterate_list_शीर्ष = to;
                यदि (lh->iterate_list_पुच्छ == from)
                        lh->iterate_list_पुच्छ = to;
        }
}

स्थिर अन्तःस्थ अचिह्नित अग्र_idx(Hashmap_आधारः *h, अचिह्नित idx) {
        प्रत्यागम idx + 1U < n_buckets(h) ? idx + 1U : 0U;
}

स्थिर अचिह्नित पूर्व_idx(Hashmap_आधारः *h, अचिह्नित idx) {
        प्रत्यागम idx > 0U ? idx - 1U : n_buckets(h) - 1U;
}

स्थिर अन्तःस्थ शून्य* प्रविष्टिः_मानम्(Hashmap_आधारः *h, संरचना hashmap_आधारः_प्रविष्टिः *e) {
        प्रतिज्ञा(h);
        प्रतिज्ञा(e);

        विकल्प (h->type) {

        पक्ष HASHMAP_TYPE_PLAIN:
        पक्ष HASHMAP_TYPE_ORDERED:
                प्रत्यागम ((संरचना plain_hashmap_प्रविष्टिः*)e)->मानम्;

        पक्ष HASHMAP_TYPE_समुच्चयः:
                प्रत्यागम (शून्य*) e->कुञ्चिका;

        स्वभाव:
                assert_not_reached();
        }
}

स्थिर शून्य आधारः_remove_प्रविष्टिः(Hashmap_आधारः *h, अचिह्नित idx) {
        अचिह्नित वामः, दक्षिणः, पूर्व, dib;
        dib_raw_t raw_dib, *dibs;

        dibs = dib_raw_ptr(h);
        प्रतिज्ञा(dibs[idx] != DIB_RAW_FREE);

#यदि ENABLE_DEBUG_HASHMAP
        प्रतिज्ञा(h);
        h->debug.rem_गणनम्++;
        h->debug.चरमः_rem_idx = idx;
#यदि_अन्तः

        वामः = idx;
        /* Find the stop bucket ("right"). It is either free or has DIB == 0. */
        प्रति (दक्षिणः = अग्र_idx(h, वामः); ; दक्षिणः = अग्र_idx(h, दक्षिणः)) {
                raw_dib = dibs[दक्षिणः];
                यदि (अन्तर्_समुच्चयः(raw_dib, 0, DIB_RAW_FREE))
                        भङ्ग;

                /* The buckets are not supposed to be all occupied and with DIB > 0.
                 * That would mean we could make everyone better off by shifting them
                 * backward. This scenario is impossible. */
                प्रतिज्ञा(वामः != दक्षिणः);
        }

        यदि (h->type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*) h;
                संरचना ordered_hashmap_प्रविष्टिः *le = ordered_bucket_at(lh, idx);

                यदि (le->iterate_अग्र != IDX_NIL)
                        ordered_bucket_at(lh, le->iterate_अग्र)->iterate_previous = le->iterate_previous;
                अन्यथा
                        lh->iterate_list_पुच्छ = le->iterate_previous;

                यदि (le->iterate_previous != IDX_NIL)
                        ordered_bucket_at(lh, le->iterate_previous)->iterate_अग्र = le->iterate_अग्र;
                अन्यथा
                        lh->iterate_list_शीर्ष = le->iterate_अग्र;
        }

        /* Now shift all buckets in the interval (left, right) one step backwards */
        प्रति (पूर्व = वामः, वामः = अग्र_idx(h, वामः); वामः != दक्षिणः;
             पूर्व = वामः, वामः = अग्र_idx(h, वामः)) {
                dib = bucket_calculate_dib(h, वामः, dibs[वामः]);
                प्रतिज्ञा(dib != 0);
                bucket_move_प्रविष्टिः(h, रिक्त, वामः, पूर्व);
                bucket_समुच्चयः_dib(h, पूर्व, dib - 1);
        }

        bucket_mark_free(h, पूर्व);
        n_entries_dec(h);
        आधारः_समुच्चयः_dirty(h);
}
#परिभाषय remove_प्रविष्टिः(h, idx) आधारः_remove_प्रविष्टिः(HASHMAP_आधारः(h), idx)

स्थिर अचिह्नित hashmap_iterate_अन्तर्_insertion_क्रमः(OrderedHashmap *h, Iterator *i) {
        संरचना ordered_hashmap_प्रविष्टिः *e;
        अचिह्नित idx;

        प्रतिज्ञा(h);
        प्रतिज्ञा(i);

        यदि (i->idx == IDX_NIL)
                गच्छ at_अन्तः;

        यदि (i->idx == IDX_FIRST && h->iterate_list_शीर्ष == IDX_NIL)
                गच्छ at_अन्तः;

        यदि (i->idx == IDX_FIRST) {
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
                यदि (e->p.b.कुञ्चिका != i->अग्र_कुञ्चिका) {
                        idx = पूर्व_idx(HASHMAP_आधारः(h), idx);
                        e = ordered_bucket_at(h, idx);
                }
                प्रतिज्ञा(e->p.b.कुञ्चिका == i->अग्र_कुञ्चिका);
        }

#यदि ENABLE_DEBUG_HASHMAP
        i->पूर्व_idx = idx;
#यदि_अन्तः

        यदि (e->iterate_अग्र != IDX_NIL) {
                संरचना ordered_hashmap_प्रविष्टिः *n;
                i->idx = e->iterate_अग्र;
                n = ordered_bucket_at(h, i->idx);
                i->अग्र_कुञ्चिका = n->p.b.कुञ्चिका;
        } अन्यथा
                i->idx = IDX_NIL;

        प्रत्यागम idx;

at_अन्तः:
        i->idx = IDX_NIL;
        प्रत्यागम IDX_NIL;
}

स्थिर अचिह्नित hashmap_iterate_अन्तर्_आन्तरिकम्_क्रमः(Hashmap_आधारः *h, Iterator *i) {
        अचिह्नित idx;

        प्रतिज्ञा(h);
        प्रतिज्ञा(i);

        यदि (i->idx == IDX_NIL)
                गच्छ at_अन्तः;

        यदि (i->idx == IDX_FIRST) {
                /* fast forward to the first occupied bucket */
                यदि (h->has_परोक्षम्) {
                        i->idx = skip_free_buckets(h, h->परोक्षम्.idx_निम्नतमम्_प्रविष्टिः);
                        h->परोक्षम्.idx_निम्नतमम्_प्रविष्टिः = i->idx;
                } अन्यथा
                        i->idx = skip_free_buckets(h, 0);

                यदि (i->idx == IDX_NIL)
                        गच्छ at_अन्तः;
        } अन्यथा {
                संरचना hashmap_आधारः_प्रविष्टिः *e;

                प्रतिज्ञा(i->idx > 0);

                e = bucket_at(h, i->idx);
                /*
                 * We allow removing the current entry while iterating, but removal may cause
                 * a backward shift. The next entry may thus move one bucket to the left.
                 * To detect when it happens, we remember the key pointer of the entry we were
                 * going to iterate next. If it does not match, there was a backward shift.
                 */
                यदि (e->कुञ्चिका != i->अग्र_कुञ्चिका)
                        e = bucket_at(h, --i->idx);

                प्रतिज्ञा(e->कुञ्चिका == i->अग्र_कुञ्चिका);
        }

        idx = i->idx;
#यदि ENABLE_DEBUG_HASHMAP
        i->पूर्व_idx = idx;
#यदि_अन्तः

        i->idx = skip_free_buckets(h, i->idx + 1);
        यदि (i->idx != IDX_NIL)
                i->अग्र_कुञ्चिका = bucket_at(h, i->idx)->कुञ्चिका;
        अन्यथा
                i->idx = IDX_NIL;

        प्रत्यागम idx;

at_अन्तः:
        i->idx = IDX_NIL;
        प्रत्यागम IDX_NIL;
}

स्थिर अचिह्नित hashmap_iterate_प्रविष्टिः(Hashmap_आधारः *h, Iterator *i) {
        यदि (!h) {
                i->idx = IDX_NIL;
                प्रत्यागम IDX_NIL;
        }

#यदि ENABLE_DEBUG_HASHMAP
        यदि (i->idx == IDX_FIRST) {
                i->put_गणनम् = h->debug.put_गणनम्;
                i->rem_गणनम् = h->debug.rem_गणनम्;
        } अन्यथा {
                /* While iterating, must not add any new entries */
                प्रतिज्ञा(i->put_गणनम् == h->debug.put_गणनम्);
                /* ... or remove entries other than the current one */
                प्रतिज्ञा(i->rem_गणनम् == h->debug.rem_गणनम् ||
                       (i->rem_गणनम् == h->debug.rem_गणनम् - 1 &&
                        i->पूर्व_idx == h->debug.चरमः_rem_idx));
                /* Reset our removals counter */
                i->rem_गणनम् = h->debug.rem_गणनम्;
        }
#यदि_अन्तः

        प्रत्यागम h->type == HASHMAP_TYPE_ORDERED ? hashmap_iterate_अन्तर्_insertion_क्रमः((OrderedHashmap*) h, i)
                                               : hashmap_iterate_अन्तर्_आन्तरिकम्_क्रमः(h, i);
}

bool _hashmap_iterate(Hashmap_आधारः *h, Iterator *i, शून्य **मानम्, ध्रुव शून्य **कुञ्चिका) {
        संरचना hashmap_आधारः_प्रविष्टिः *e;
        शून्य *दत्त;
        अचिह्नित idx;

        idx = hashmap_iterate_प्रविष्टिः(h, i);
        यदि (idx == IDX_NIL) {
                यदि (मानम्)
                        *मानम् = रिक्त;
                यदि (कुञ्चिका)
                        *कुञ्चिका = रिक्त;

                प्रत्यागम false;
        }

        e = bucket_at(h, idx);
        दत्त = प्रविष्टिः_मानम्(h, e);
        यदि (मानम्)
                *मानम् = दत्त;
        यदि (कुञ्चिका)
                *कुञ्चिका = e->कुञ्चिका;

        प्रत्यागम true;
}

#परिभाषय HASHMAP_FOREACH_IDX(idx, h, i) \
        प्रति ((i) = ITERATOR_FIRST, (idx) = hashmap_iterate_प्रविष्टिः((h), &(i)); \
             (idx != IDX_NIL); \
             (idx) = hashmap_iterate_प्रविष्टिः((h), &(i)))

IteratedCache* _hashmap_iterated_cache_new(Hashmap_आधारः *h) {
        IteratedCache *cache;

        प्रतिज्ञा(h);
        प्रतिज्ञा(!h->cached);

        यदि (h->cached)
                प्रत्यागम रिक्त;

        cache = new0(IteratedCache, 1);
        यदि (!cache)
                प्रत्यागम रिक्त;

        cache->hashmap = h;
        h->cached = true;

        प्रत्यागम cache;
}

स्थिर शून्य reset_प्रत्यक्षम्_storage(Hashmap_आधारः *h) {
        ध्रुव संरचना hashmap_type_info *hi = &hashmap_type_info[h->type];
        शून्य *p;

        प्रतिज्ञा(!h->has_परोक्षम्);

        p = mempset(h->प्रत्यक्षम्.storage, 0, hi->प्रविष्टिः_आकार * hi->n_प्रत्यक्षम्_buckets);
        स्मृतिं_पूरय(p, DIB_RAW_INIT, परिमाण(dib_raw_t) * hi->n_प्रत्यक्षम्_buckets);
}

स्थिर शून्य shared_hash_कुञ्चिका_initialize(शून्य) {
        random_अष्टकानि(shared_hash_कुञ्चिका, परिमाण(shared_hash_कुञ्चिका));
}

स्थिर संरचना Hashmap_आधारः* hashmap_आधारः_new(ध्रुव संरचना hash_ops *hash_ops, गणना HashmapType type) {
        Hashmap_आधारः *h;
        ध्रुव संरचना hashmap_type_info *hi = &hashmap_type_info[type];

        bool use_pool = mempool_enabled && mempool_enabled();  /* mempool_enabled is a weak symbol */

        h = use_pool ? mempool_alloc0_tile(hi->mempool) : malloc0(hi->शीर्ष_आकार);
        यदि (!h)
                प्रत्यागम रिक्त;

        h->type = type;
        h->from_pool = use_pool;
        h->hash_ops = hash_ops ?: &trivial_hash_ops;

        यदि (type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*)h;
                lh->iterate_list_शीर्ष = lh->iterate_list_पुच्छ = IDX_NIL;
        }

        reset_प्रत्यक्षम्_storage(h);

        स्थिर pthread_once_t once = PTHREAD_ONCE_INIT;
        assert_se(pthread_once(&once, shared_hash_कुञ्चिका_initialize) == 0);

#यदि ENABLE_DEBUG_HASHMAP
        assert_se(pthread_mutex_lock(&hashmap_debug_list_mutex) == 0);
        LIST_PREPEND(debug_list, hashmap_debug_list, &h->debug);
        assert_se(pthread_mutex_unlock(&hashmap_debug_list_mutex) == 0);
#यदि_अन्तः

        प्रत्यागम h;
}

Hashmap *hashmap_new(ध्रुव संरचना hash_ops *hash_ops) {
        प्रत्यागम (Hashmap*)        hashmap_आधारः_new(hash_ops, HASHMAP_TYPE_PLAIN);
}

OrderedHashmap *ordered_hashmap_new(ध्रुव संरचना hash_ops *hash_ops) {
        प्रत्यागम (OrderedHashmap*) hashmap_आधारः_new(hash_ops, HASHMAP_TYPE_ORDERED);
}

समुच्चयः_2 *समुच्चयः_new(ध्रुव संरचना hash_ops *hash_ops) {
        प्रत्यागम (समुच्चयः_2*)            hashmap_आधारः_new(hash_ops, HASHMAP_TYPE_समुच्चयः);
}

स्थिर पूर्णाङ्क hashmap_आधारः_ensure_allocated(Hashmap_आधारः **h, ध्रुव संरचना hash_ops *hash_ops,
                                         गणना HashmapType type) {
        Hashmap_आधारः *q;

        प्रतिज्ञा(h);

        यदि (*h) {
                प्रतिज्ञा((*h)->hash_ops == (hash_ops ?: &trivial_hash_ops));
                प्रत्यागम 0;
        }

        q = hashmap_आधारः_new(hash_ops, type);
        यदि (!q)
                प्रत्यागम -ENOMEM;

        *h = q;
        प्रत्यागम 1;
}

पूर्णाङ्क hashmap_ensure_allocated(Hashmap **h, ध्रुव संरचना hash_ops *hash_ops) {
        प्रत्यागम hashmap_आधारः_ensure_allocated((Hashmap_आधारः**)h, hash_ops, HASHMAP_TYPE_PLAIN);
}

पूर्णाङ्क ordered_hashmap_ensure_allocated(OrderedHashmap **h, ध्रुव संरचना hash_ops *hash_ops) {
        प्रत्यागम hashmap_आधारः_ensure_allocated((Hashmap_आधारः**)h, hash_ops, HASHMAP_TYPE_ORDERED);
}

पूर्णाङ्क समुच्चयः_ensure_allocated(समुच्चयः_2 **s, ध्रुव संरचना hash_ops *hash_ops) {
        प्रत्यागम hashmap_आधारः_ensure_allocated((Hashmap_आधारः**)s, hash_ops, HASHMAP_TYPE_समुच्चयः);
}

पूर्णाङ्क hashmap_ensure_put(Hashmap **h, ध्रुव संरचना hash_ops *hash_ops, ध्रुव शून्य *कुञ्चिका, शून्य *मानम्) {
        पूर्णाङ्क r;

        प्रतिज्ञा(h);

        r = hashmap_ensure_allocated(h, hash_ops);
        यदि (r < 0)
                प्रत्यागम r;

        प्रत्यागम hashmap_put(*h, कुञ्चिका, मानम्);
}

पूर्णाङ्क ordered_hashmap_ensure_put(OrderedHashmap **h, ध्रुव संरचना hash_ops *hash_ops, ध्रुव शून्य *कुञ्चिका, शून्य *मानम्) {
        पूर्णाङ्क r;

        प्रतिज्ञा(h);

        r = ordered_hashmap_ensure_allocated(h, hash_ops);
        यदि (r < 0)
                प्रत्यागम r;

        प्रत्यागम ordered_hashmap_put(*h, कुञ्चिका, मानम्);
}

पूर्णाङ्क ordered_hashmap_ensure_replace(OrderedHashmap **h, ध्रुव संरचना hash_ops *hash_ops, ध्रुव शून्य *कुञ्चिका, शून्य *मानम्) {
        पूर्णाङ्क r;

        प्रतिज्ञा(h);

        r = ordered_hashmap_ensure_allocated(h, hash_ops);
        यदि (r < 0)
                प्रत्यागम r;

        प्रत्यागम ordered_hashmap_replace(*h, कुञ्चिका, मानम्);
}

पूर्णाङ्क hashmap_ensure_replace(Hashmap **h, ध्रुव संरचना hash_ops *hash_ops, ध्रुव शून्य *कुञ्चिका, शून्य *मानम्) {
        पूर्णाङ्क r;

        प्रतिज्ञा(h);

        r = hashmap_ensure_allocated(h, hash_ops);
        यदि (r < 0)
                प्रत्यागम r;

        प्रत्यागम hashmap_replace(*h, कुञ्चिका, मानम्);
}

स्थिर शून्य hashmap_free_no_clear(Hashmap_आधारः *h) {
        प्रतिज्ञा(!h->has_परोक्षम्);
        प्रतिज्ञा(h->n_प्रत्यक्षम्_entries == 0);

#यदि ENABLE_DEBUG_HASHMAP
        assert_se(pthread_mutex_lock(&hashmap_debug_list_mutex) == 0);
        LIST_REMOVE(debug_list, hashmap_debug_list, &h->debug);
        assert_se(pthread_mutex_unlock(&hashmap_debug_list_mutex) == 0);
#यदि_अन्तः

        यदि (h->from_pool) {
                /* Ensure that the object didn't get migrated between threads. */
                assert_se(is_main_thread());
                mempool_free_tile(hashmap_type_info[h->type].mempool, h);
        } अन्यथा
                मुक्त(h);
}

Hashmap_आधारः* _hashmap_free(Hashmap_आधारः *h) {
        यदि (h) {
                _hashmap_clear(h);
                hashmap_free_no_clear(h);
        }

        प्रत्यागम रिक्त;
}

शून्य _hashmap_clear(Hashmap_आधारः *h) {
        यदि (!h)
                प्रत्यागम;

        यदि (h->hash_ops->free_कुञ्चिका || h->hash_ops->free_मानम्) {

                /* If destructor calls are defined, let's destroy things defensively: let's take the item out of the
                 * hash table, and only then call the destructor functions. If these destructors then try to unregister
                 * themselves from our hash table a second time, the entry is already gone. */

                यावत् (hashmap_आकार(h) > 0) {
                        शून्य *k = रिक्त;
                        शून्य *v;

                        v = hashmap_first_कुञ्चिका_and_मानम्(h, true, &k);

                        यदि (h->hash_ops->free_कुञ्चिका)
                                h->hash_ops->free_कुञ्चिका(k);

                        यदि (h->hash_ops->free_मानम्)
                                h->hash_ops->free_मानम्(v);
                }
        }

        यदि (h->has_परोक्षम्) {
                मुक्त(h->परोक्षम्.storage);
                h->has_परोक्षम् = false;
        }

        h->n_प्रत्यक्षम्_entries = 0;
        reset_प्रत्यक्षम्_storage(h);

        यदि (h->type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*) h;
                lh->iterate_list_शीर्ष = lh->iterate_list_पुच्छ = IDX_NIL;
        }

        आधारः_समुच्चयः_dirty(h);
}

स्थिर पूर्णाङ्क resize_buckets(Hashmap_आधारः *h, अचिह्नित entries_सङ्कलय);

/*
 * Finds an empty bucket to put an entry into, starting the scan at 'idx'.
 * Performs Robin Hood swaps as it goes. The entry to put must be placed
 * by the caller into swap slot IDX_PUT.
 * If used for in-place resizing, may leave a displaced entry in swap slot
 * IDX_PUT. Caller must rehash it next.
 * Returns: true if it left a displaced entry to rehash next in IDX_PUT,
 *          false otherwise.
 */
स्थिर bool hashmap_put_robin_hood(Hashmap_आधारः *h, अचिह्नित idx,
                                   संरचना swap_entries *swap) {
        dib_raw_t raw_dib, *dibs;
        अचिह्नित dib, distance;

#यदि ENABLE_DEBUG_HASHMAP
        प्रतिज्ञा(h);
        h->debug.put_गणनम्++;
#यदि_अन्तः

        dibs = dib_raw_ptr(h);

        प्रति (distance = 0; ; distance++) {
                raw_dib = dibs[idx];
                यदि (अन्तर्_समुच्चयः(raw_dib, DIB_RAW_FREE, DIB_RAW_REHASH)) {
                        यदि (raw_dib == DIB_RAW_REHASH)
                                bucket_move_प्रविष्टिः(h, swap, idx, IDX_TMP);

                        यदि (h->has_परोक्षम् && h->परोक्षम्.idx_निम्नतमम्_प्रविष्टिः > idx)
                                h->परोक्षम्.idx_निम्नतमम्_प्रविष्टिः = idx;

                        bucket_समुच्चयः_dib(h, idx, distance);
                        bucket_move_प्रविष्टिः(h, swap, IDX_PUT, idx);
                        यदि (raw_dib == DIB_RAW_REHASH) {
                                bucket_move_प्रविष्टिः(h, swap, IDX_TMP, IDX_PUT);
                                प्रत्यागम true;
                        }

                        प्रत्यागम false;
                }

                dib = bucket_calculate_dib(h, idx, raw_dib);

                यदि (dib < distance) {
                        /* Found a wealthier entry. Go Robin Hood! */
                        bucket_समुच्चयः_dib(h, idx, distance);

                        /* swap the entries */
                        bucket_move_प्रविष्टिः(h, swap, idx, IDX_TMP);
                        bucket_move_प्रविष्टिः(h, swap, IDX_PUT, idx);
                        bucket_move_प्रविष्टिः(h, swap, IDX_TMP, IDX_PUT);

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
स्थिर पूर्णाङ्क hashmap_आधारः_put_boldly(Hashmap_आधारः *h, अचिह्नित idx,
                                   संरचना swap_entries *swap, bool may_resize) {
        संरचना ordered_hashmap_प्रविष्टिः *new_प्रविष्टिः;
        पूर्णाङ्क r;

        प्रतिज्ञा(idx < n_buckets(h));

        new_प्रविष्टिः = bucket_at_swap(swap, IDX_PUT);

        यदि (may_resize) {
                r = resize_buckets(h, 1);
                यदि (r < 0)
                        प्रत्यागम r;
                यदि (r > 0)
                        idx = bucket_hash(h, new_प्रविष्टिः->p.b.कुञ्चिका);
        }
        प्रतिज्ञा(n_entries(h) < n_buckets(h));

        यदि (h->type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*) h;

                new_प्रविष्टिः->iterate_अग्र = IDX_NIL;
                new_प्रविष्टिः->iterate_previous = lh->iterate_list_पुच्छ;

                यदि (lh->iterate_list_पुच्छ != IDX_NIL) {
                        संरचना ordered_hashmap_प्रविष्टिः *old_पुच्छ;

                        old_पुच्छ = ordered_bucket_at(lh, lh->iterate_list_पुच्छ);
                        प्रतिज्ञा(old_पुच्छ->iterate_अग्र == IDX_NIL);
                        old_पुच्छ->iterate_अग्र = IDX_PUT;
                }

                lh->iterate_list_पुच्छ = IDX_PUT;
                यदि (lh->iterate_list_शीर्ष == IDX_NIL)
                        lh->iterate_list_शीर्ष = IDX_PUT;
        }

        assert_se(hashmap_put_robin_hood(h, idx, swap) == false);

        n_entries_inc(h);
#यदि ENABLE_DEBUG_HASHMAP
        h->debug.max_entries = MAX(h->debug.max_entries, n_entries(h));
#यदि_अन्तः

        आधारः_समुच्चयः_dirty(h);

        प्रत्यागम 1;
}
#परिभाषय hashmap_put_boldly(h, idx, swap, may_resize) \
        hashmap_आधारः_put_boldly(HASHMAP_आधारः(h), idx, swap, may_resize)

/*
 * Returns 0 if resize is not needed.
 *         1 if successfully resized.
 *         -ENOMEM on allocation failure.
 */
स्थिर पूर्णाङ्क resize_buckets(Hashmap_आधारः *h, अचिह्नित entries_सङ्कलय) {
        संरचना swap_entries swap;
        शून्य *new_storage;
        dib_raw_t *old_dibs, *new_dibs;
        ध्रुव संरचना hashmap_type_info *hi;
        अचिह्नित idx, optimal_idx;
        अचिह्नित old_n_buckets, new_n_buckets, n_rehashed, new_n_entries;
        uint8_t new_सरणम्;
        bool rehash_अग्र;

        प्रतिज्ञा(h);

        hi = &hashmap_type_info[h->type];
        new_n_entries = n_entries(h) + entries_सङ्कलय;

        /* overflow? */
        यदि (_unlikely_(new_n_entries < entries_सङ्कलय))
                प्रत्यागम -ENOMEM;

        /* For direct storage we allow 100% load, because it's tiny. */
        यदि (!h->has_परोक्षम् && new_n_entries <= hi->n_प्रत्यक्षम्_buckets)
                प्रत्यागम 0;

        /*
         * Load factor = n/m = 1 - (1/INV_KEEP_FREE).
         * From it follows: m = n + n/(INV_KEEP_FREE - 1)
         */
        new_n_buckets = new_n_entries + new_n_entries / (INV_KEEP_FREE - 1);
        /* overflow? */
        यदि (_unlikely_(new_n_buckets < new_n_entries))
                प्रत्यागम -ENOMEM;

        old_n_buckets = n_buckets(h);

        यदि (_likely_(new_n_buckets <= old_n_buckets))
                प्रत्यागम 0;

        यदि (_unlikely_(new_n_buckets > UINT_MAX / (hi->प्रविष्टिः_आकार + परिमाण(dib_raw_t))))
                प्रत्यागम -ENOMEM;

        new_सरणम् = log2u_आवर्तनम्_ऊर्ध्वम्(MAX(
                        new_n_buckets * (hi->प्रविष्टिः_आकार + परिमाण(dib_raw_t)),
                        2 * परिमाण(संरचना प्रत्यक्षम्_storage)));

        /* Realloc storage (buckets and DIB array). */
        new_storage = realloc(h->has_परोक्षम् ? h->परोक्षम्.storage : रिक्त,
                              1U << new_सरणम्);
        यदि (!new_storage)
                प्रत्यागम -ENOMEM;

        /* Must upgrade direct to indirect storage. */
        यदि (!h->has_परोक्षम्) {
                स्मृतिं_प्रतिलिख(new_storage, h->प्रत्यक्षम्.storage,
                       old_n_buckets * (hi->प्रविष्टिः_आकार + परिमाण(dib_raw_t)));
                h->परोक्षम्.n_entries = h->n_प्रत्यक्षम्_entries;
                h->परोक्षम्.idx_निम्नतमम्_प्रविष्टिः = 0;
                h->n_प्रत्यक्षम्_entries = 0;
        }

        /* Get a new hash key. If we've just upgraded to indirect storage,
         * allow reusing a previously generated key. It's still a different key
         * from the shared one that we used for direct storage. */
        get_hash_कुञ्चिका(h->परोक्षम्.hash_कुञ्चिका, !h->has_परोक्षम्);

        h->has_परोक्षम् = true;
        h->परोक्षम्.storage = new_storage;
        h->परोक्षम्.n_buckets = (1U << new_सरणम्) /
                                (hi->प्रविष्टिः_आकार + परिमाण(dib_raw_t));

        old_dibs = (dib_raw_t*)((uint8_t*) new_storage + hi->प्रविष्टिः_आकार * old_n_buckets);
        new_dibs = dib_raw_ptr(h);

        /*
         * Move the DIB array to the new place, replacing valid DIB values with
         * DIB_RAW_REHASH to indicate all of the used buckets need rehashing.
         * Note: Overlap is not possible, because we have at least doubled the
         * number of buckets and dib_raw_t is smaller than any entry type.
         */
        प्रति (idx = 0; idx < old_n_buckets; idx++) {
                प्रतिज्ञा(old_dibs[idx] != DIB_RAW_REHASH);
                new_dibs[idx] = old_dibs[idx] == DIB_RAW_FREE ? DIB_RAW_FREE
                                                              : DIB_RAW_REHASH;
        }

        /* Zero the area of newly added entries (including the old DIB area) */
        memzero(bucket_at(h, old_n_buckets),
               (n_buckets(h) - old_n_buckets) * hi->प्रविष्टिः_आकार);

        /* The upper half of the new DIB array needs initialization */
        स्मृतिं_पूरय(&new_dibs[old_n_buckets], DIB_RAW_INIT,
               (n_buckets(h) - old_n_buckets) * परिमाण(dib_raw_t));

        /* Rehash entries that need it */
        n_rehashed = 0;
        प्रति (idx = 0; idx < old_n_buckets; idx++) {
                यदि (new_dibs[idx] != DIB_RAW_REHASH)
                        अनुवर्त;

                optimal_idx = bucket_hash(h, bucket_at(h, idx)->कुञ्चिका);

                /*
                 * Not much to do if by luck the entry hashes to its current
                 * location. Just set its DIB.
                 */
                यदि (optimal_idx == idx) {
                        new_dibs[idx] = 0;
                        n_rehashed++;
                        अनुवर्त;
                }

                new_dibs[idx] = DIB_RAW_FREE;
                bucket_move_प्रविष्टिः(h, &swap, idx, IDX_PUT);
                /* bucket_move_entry does not clear the source */
                memzero(bucket_at(h, idx), hi->प्रविष्टिः_आकार);

                कुरु {
                        /*
                         * Find the new bucket for the current entry. This may make
                         * another entry homeless and load it into IDX_PUT.
                         */
                        rehash_अग्र = hashmap_put_robin_hood(h, optimal_idx, &swap);
                        n_rehashed++;

                        /* Did the current entry displace another one? */
                        यदि (rehash_अग्र)
                                optimal_idx = bucket_hash(h, bucket_at_swap(&swap, IDX_PUT)->p.b.कुञ्चिका);
                } यावत् (rehash_अग्र);
        }

        assert_se(n_rehashed == n_entries(h));

        प्रत्यागम 1;
}

/*
 * Finds an entry with a matching key
 * Returns: index of the found entry, or IDX_NIL if not found.
 */
स्थिर अचिह्नित आधारः_bucket_अवलोकनम्(Hashmap_आधारः *h, अचिह्नित idx, ध्रुव शून्य *कुञ्चिका) {
        compare_func_t compare = h->hash_ops->compare;
        संरचना hashmap_आधारः_प्रविष्टिः *e;
        अचिह्नित dib, distance;
        dib_raw_t *dibs = dib_raw_ptr(h);

        प्रतिज्ञा(idx < n_buckets(h));

        प्रति (distance = 0; ; distance++) {
                यदि (dibs[idx] == DIB_RAW_FREE)
                        प्रत्यागम IDX_NIL;

                dib = bucket_calculate_dib(h, idx, dibs[idx]);

                यदि (dib < distance)
                        प्रत्यागम IDX_NIL;
                यदि (dib == distance) {
                        e = bucket_at(h, idx);

                        /* Most maps compare keys by pointer. Do that here, without the indirect call. */
                        यदि (compare == trivial_compare_func ? e->कुञ्चिका == कुञ्चिका : compare(e->कुञ्चिका, कुञ्चिका) == 0)
                                प्रत्यागम idx;
                }

                idx = अग्र_idx(h, idx);
        }
}
#परिभाषय bucket_अवलोकनम्(h, idx, कुञ्चिका) आधारः_bucket_अवलोकनम्(HASHMAP_आधारः(h), idx, कुञ्चिका)

पूर्णाङ्क hashmap_put(Hashmap *h, ध्रुव शून्य *कुञ्चिका, शून्य *मानम्) {
        संरचना swap_entries swap;
        संरचना plain_hashmap_प्रविष्टिः *e;
        अचिह्नित hash, idx;

        प्रतिज्ञा(h);

        hash = bucket_hash(h, कुञ्चिका);
        idx = bucket_अवलोकनम्(h, hash, कुञ्चिका);
        यदि (idx != IDX_NIL) {
                e = plain_bucket_at(h, idx);
                यदि (e->मानम् == मानम्)
                        प्रत्यागम 0;
                प्रत्यागम -EEXIST;
        }

        e = &bucket_at_swap(&swap, IDX_PUT)->p;
        e->b.कुञ्चिका = कुञ्चिका;
        e->मानम् = मानम्;
        प्रत्यागम hashmap_put_boldly(h, hash, &swap, true);
}

पूर्णाङ्क समुच्चयः_put(समुच्चयः_2 *s, ध्रुव शून्य *कुञ्चिका) {
        संरचना swap_entries swap;
        संरचना hashmap_आधारः_प्रविष्टिः *e;
        अचिह्नित hash, idx;

        प्रतिज्ञा(s);

        hash = bucket_hash(s, कुञ्चिका);
        idx = bucket_अवलोकनम्(s, hash, कुञ्चिका);
        यदि (idx != IDX_NIL)
                प्रत्यागम 0;

        e = &bucket_at_swap(&swap, IDX_PUT)->p.b;
        e->कुञ्चिका = कुञ्चिका;
        प्रत्यागम hashmap_put_boldly(s, hash, &swap, true);
}

पूर्णाङ्क समुच्चयः_ensure_put(समुच्चयः_2 **s, ध्रुव संरचना hash_ops *hash_ops, ध्रुव शून्य *कुञ्चिका) {
        पूर्णाङ्क r;

        प्रतिज्ञा(s);

        r = समुच्चयः_ensure_allocated(s, hash_ops);
        यदि (r < 0)
                प्रत्यागम r;

        प्रत्यागम समुच्चयः_put(*s, कुञ्चिका);
}

पूर्णाङ्क समुच्चयः_ensure_consume(समुच्चयः_2 **s, ध्रुव संरचना hash_ops *hash_ops, शून्य *कुञ्चिका) {
        पूर्णाङ्क r;

        r = समुच्चयः_ensure_put(s, hash_ops, कुञ्चिका);
        यदि (r <= 0) {
                यदि (hash_ops && hash_ops->free_कुञ्चिका)
                        hash_ops->free_कुञ्चिका(कुञ्चिका);
                अन्यथा यदि (hash_ops && hash_ops->free_मानम्)
                        /* Sets store their element in the key slot but may carry a value destructor. */
                        hash_ops->free_मानम्(कुञ्चिका);
                अन्यथा
                        मुक्त(कुञ्चिका);
        }

        प्रत्यागम r;
}

पूर्णाङ्क hashmap_replace(Hashmap *h, ध्रुव शून्य *कुञ्चिका, शून्य *मानम्) {
        संरचना swap_entries swap;
        संरचना plain_hashmap_प्रविष्टिः *e;
        अचिह्नित hash, idx;

        प्रतिज्ञा(h);

        hash = bucket_hash(h, कुञ्चिका);
        idx = bucket_अवलोकनम्(h, hash, कुञ्चिका);
        यदि (idx != IDX_NIL) {
                e = plain_bucket_at(h, idx);
#यदि ENABLE_DEBUG_HASHMAP
                /* Although the key is equal, the key pointer may have changed,
                 * and this would break our assumption for iterating. So count
                 * this operation as incompatible with iteration. */
                यदि (e->b.कुञ्चिका != कुञ्चिका) {
                        h->b.debug.put_गणनम्++;
                        h->b.debug.rem_गणनम्++;
                        h->b.debug.चरमः_rem_idx = idx;
                }
#यदि_अन्तः
                e->b.कुञ्चिका = कुञ्चिका;
                e->मानम् = मानम्;
                hashmap_समुच्चयः_dirty(h);

                प्रत्यागम 0;
        }

        e = &bucket_at_swap(&swap, IDX_PUT)->p;
        e->b.कुञ्चिका = कुञ्चिका;
        e->मानम् = मानम्;
        प्रत्यागम hashmap_put_boldly(h, hash, &swap, true);
}

पूर्णाङ्क hashmap_update(Hashmap *h, ध्रुव शून्य *कुञ्चिका, शून्य *मानम्) {
        संरचना plain_hashmap_प्रविष्टिः *e;
        अचिह्नित hash, idx;

        प्रतिज्ञा(h);

        hash = bucket_hash(h, कुञ्चिका);
        idx = bucket_अवलोकनम्(h, hash, कुञ्चिका);
        यदि (idx == IDX_NIL)
                प्रत्यागम -ENOENT;

        e = plain_bucket_at(h, idx);
        e->मानम् = मानम्;
        hashmap_समुच्चयः_dirty(h);

        प्रत्यागम 0;
}

शून्य* _hashmap_get(Hashmap_आधारः *h, ध्रुव शून्य *कुञ्चिका) {
        संरचना hashmap_आधारः_प्रविष्टिः *e;
        अचिह्नित hash, idx;

        यदि (!h)
                प्रत्यागम रिक्त;

        hash = bucket_hash(h, कुञ्चिका);
        idx = bucket_अवलोकनम्(h, hash, कुञ्चिका);
        यदि (idx == IDX_NIL)
                प्रत्यागम रिक्त;

        e = bucket_at(h, idx);
        प्रत्यागम प्रविष्टिः_मानम्(h, e);
}

शून्य* hashmap_get2(Hashmap *h, ध्रुव शून्य *कुञ्चिका, शून्य **ret) {
        संरचना plain_hashmap_प्रविष्टिः *e;
        अचिह्नित hash, idx;

        यदि (!h)
                प्रत्यागम रिक्त;

        hash = bucket_hash(h, कुञ्चिका);
        idx = bucket_अवलोकनम्(h, hash, कुञ्चिका);
        यदि (idx == IDX_NIL)
                प्रत्यागम रिक्त;

        e = plain_bucket_at(h, idx);
        यदि (ret)
                *ret = (शून्य*) e->b.कुञ्चिका;

        प्रत्यागम e->मानम्;
}

bool hashmap_अन्तर्भवति_2(Hashmap_आधारः *h, ध्रुव शून्य *कुञ्चिका) {
        अचिह्नित hash;

        यदि (!h)
                प्रत्यागम false;

        hash = bucket_hash(h, कुञ्चिका);
        प्रत्यागम bucket_अवलोकनम्(h, hash, कुञ्चिका) != IDX_NIL;
}

शून्य* _hashmap_remove(Hashmap_आधारः *h, ध्रुव शून्य *कुञ्चिका) {
        संरचना hashmap_आधारः_प्रविष्टिः *e;
        अचिह्नित hash, idx;
        शून्य *दत्त;

        यदि (!h)
                प्रत्यागम रिक्त;

        hash = bucket_hash(h, कुञ्चिका);
        idx = bucket_अवलोकनम्(h, hash, कुञ्चिका);
        यदि (idx == IDX_NIL)
                प्रत्यागम रिक्त;

        e = bucket_at(h, idx);
        दत्त = प्रविष्टिः_मानम्(h, e);
        remove_प्रविष्टिः(h, idx);

        प्रत्यागम दत्त;
}

शून्य* hashmap_remove2(Hashmap *h, ध्रुव शून्य *कुञ्चिका, शून्य **ret) {
        संरचना plain_hashmap_प्रविष्टिः *e;
        अचिह्नित hash, idx;
        शून्य *दत्त;

        यदि (!h) {
                यदि (ret)
                        *ret = रिक्त;
                प्रत्यागम रिक्त;
        }

        hash = bucket_hash(h, कुञ्चिका);
        idx = bucket_अवलोकनम्(h, hash, कुञ्चिका);
        यदि (idx == IDX_NIL) {
                यदि (ret)
                        *ret = रिक्त;
                प्रत्यागम रिक्त;
        }

        e = plain_bucket_at(h, idx);
        दत्त = e->मानम्;
        यदि (ret)
                *ret = (शून्य*) e->b.कुञ्चिका;

        remove_प्रविष्टिः(h, idx);

        प्रत्यागम दत्त;
}

पूर्णाङ्क hashmap_remove_and_put(Hashmap *h, ध्रुव शून्य *old_कुञ्चिका, ध्रुव शून्य *new_कुञ्चिका, शून्य *मानम्) {
        संरचना swap_entries swap;
        संरचना plain_hashmap_प्रविष्टिः *e;
        अचिह्नित old_hash, new_hash, idx;

        यदि (!h)
                प्रत्यागम -ENOENT;

        old_hash = bucket_hash(h, old_कुञ्चिका);
        idx = bucket_अवलोकनम्(h, old_hash, old_कुञ्चिका);
        यदि (idx == IDX_NIL)
                प्रत्यागम -ENOENT;

        new_hash = bucket_hash(h, new_कुञ्चिका);
        यदि (bucket_अवलोकनम्(h, new_hash, new_कुञ्चिका) != IDX_NIL)
                प्रत्यागम -EEXIST;

        remove_प्रविष्टिः(h, idx);

        e = &bucket_at_swap(&swap, IDX_PUT)->p;
        e->b.कुञ्चिका = new_कुञ्चिका;
        e->मानम् = मानम्;
        assert_se(hashmap_put_boldly(h, new_hash, &swap, false) == 1);

        प्रत्यागम 0;
}

पूर्णाङ्क समुच्चयः_remove_and_put(समुच्चयः_2 *s, ध्रुव शून्य *old_कुञ्चिका, ध्रुव शून्य *new_कुञ्चिका) {
        संरचना swap_entries swap;
        संरचना hashmap_आधारः_प्रविष्टिः *e;
        अचिह्नित old_hash, new_hash, idx;

        यदि (!s)
                प्रत्यागम -ENOENT;

        old_hash = bucket_hash(s, old_कुञ्चिका);
        idx = bucket_अवलोकनम्(s, old_hash, old_कुञ्चिका);
        यदि (idx == IDX_NIL)
                प्रत्यागम -ENOENT;

        new_hash = bucket_hash(s, new_कुञ्चिका);
        यदि (bucket_अवलोकनम्(s, new_hash, new_कुञ्चिका) != IDX_NIL)
                प्रत्यागम -EEXIST;

        remove_प्रविष्टिः(s, idx);

        e = &bucket_at_swap(&swap, IDX_PUT)->p.b;
        e->कुञ्चिका = new_कुञ्चिका;
        assert_se(hashmap_put_boldly(s, new_hash, &swap, false) == 1);

        प्रत्यागम 0;
}

पूर्णाङ्क hashmap_remove_and_replace(Hashmap *h, ध्रुव शून्य *old_कुञ्चिका, ध्रुव शून्य *new_कुञ्चिका, शून्य *मानम्) {
        संरचना swap_entries swap;
        संरचना plain_hashmap_प्रविष्टिः *e;
        अचिह्नित old_hash, new_hash, idx_old, idx_new;

        यदि (!h)
                प्रत्यागम -ENOENT;

        old_hash = bucket_hash(h, old_कुञ्चिका);
        idx_old = bucket_अवलोकनम्(h, old_hash, old_कुञ्चिका);
        यदि (idx_old == IDX_NIL)
                प्रत्यागम -ENOENT;

        old_कुञ्चिका = bucket_at(HASHMAP_आधारः(h), idx_old)->कुञ्चिका;

        new_hash = bucket_hash(h, new_कुञ्चिका);
        idx_new = bucket_अवलोकनम्(h, new_hash, new_कुञ्चिका);
        यदि (idx_new != IDX_NIL)
                यदि (idx_old != idx_new) {
                        remove_प्रविष्टिः(h, idx_new);
                        /* Compensate for a possible backward shift. */
                        यदि (old_कुञ्चिका != bucket_at(HASHMAP_आधारः(h), idx_old)->कुञ्चिका)
                                idx_old = पूर्व_idx(HASHMAP_आधारः(h), idx_old);
                        प्रतिज्ञा(old_कुञ्चिका == bucket_at(HASHMAP_आधारः(h), idx_old)->कुञ्चिका);
                }

        remove_प्रविष्टिः(h, idx_old);

        e = &bucket_at_swap(&swap, IDX_PUT)->p;
        e->b.कुञ्चिका = new_कुञ्चिका;
        e->मानम् = मानम्;
        assert_se(hashmap_put_boldly(h, new_hash, &swap, false) == 1);

        प्रत्यागम 0;
}

शून्य* hashmap_remove_मानम्(Hashmap_आधारः *h, ध्रुव शून्य *कुञ्चिका, शून्य *मानम्) {
        संरचना hashmap_आधारः_प्रविष्टिः *e;
        अचिह्नित hash, idx;

        यदि (!h)
                प्रत्यागम रिक्त;

        hash = bucket_hash(h, कुञ्चिका);
        idx = bucket_अवलोकनम्(h, hash, कुञ्चिका);
        यदि (idx == IDX_NIL)
                प्रत्यागम रिक्त;

        e = bucket_at(h, idx);
        यदि (प्रविष्टिः_मानम्(h, e) != मानम्)
                प्रत्यागम रिक्त;

        remove_प्रविष्टिः(h, idx);

        प्रत्यागम मानम्;
}

स्थिर अचिह्नित find_first_प्रविष्टिः(Hashmap_आधारः *h) {
        Iterator i = ITERATOR_FIRST;

        यदि (!h || !n_entries(h))
                प्रत्यागम IDX_NIL;

        प्रत्यागम hashmap_iterate_प्रविष्टिः(h, &i);
}

शून्य* hashmap_first_कुञ्चिका_and_मानम्(Hashmap_आधारः *h, bool remove, शून्य **ret_कुञ्चिका) {
        संरचना hashmap_आधारः_प्रविष्टिः *e;
        शून्य *कुञ्चिका, *दत्त;
        अचिह्नित idx;

        idx = find_first_प्रविष्टिः(h);
        यदि (idx == IDX_NIL) {
                यदि (ret_कुञ्चिका)
                        *ret_कुञ्चिका = रिक्त;
                प्रत्यागम रिक्त;
        }

        e = bucket_at(h, idx);
        कुञ्चिका = (शून्य*) e->कुञ्चिका;
        दत्त = प्रविष्टिः_मानम्(h, e);

        यदि (remove)
                remove_प्रविष्टिः(h, idx);

        यदि (ret_कुञ्चिका)
                *ret_कुञ्चिका = कुञ्चिका;

        प्रत्यागम दत्त;
}

अचिह्नित hashmap_आकार(Hashmap_आधारः *h) {
        यदि (!h)
                प्रत्यागम 0;

        प्रत्यागम n_entries(h);
}

अचिह्नित _hashmap_buckets(Hashmap_आधारः *h) {
        यदि (!h)
                प्रत्यागम 0;

        प्रत्यागम n_buckets(h);
}

पूर्णाङ्क _hashmap_merge(Hashmap *h, Hashmap *other) {
        Iterator i;
        अचिह्नित idx;

        प्रतिज्ञा(h);

        HASHMAP_FOREACH_IDX(idx, HASHMAP_आधारः(other), i) {
                संरचना plain_hashmap_प्रविष्टिः *pe = plain_bucket_at(other, idx);
                पूर्णाङ्क r;

                r = hashmap_put(h, pe->b.कुञ्चिका, pe->मानम्);
                यदि (r < 0 && r != -EEXIST)
                        प्रत्यागम r;
        }

        प्रत्यागम 0;
}

पूर्णाङ्क समुच्चयः_merge(समुच्चयः_2 *s, समुच्चयः_2 *other) {
        Iterator i;
        अचिह्नित idx;

        प्रतिज्ञा(s);

        HASHMAP_FOREACH_IDX(idx, HASHMAP_आधारः(other), i) {
                संरचना समुच्चयः_प्रविष्टिः *se = समुच्चयः_bucket_at(other, idx);
                पूर्णाङ्क r;

                r = समुच्चयः_put(s, se->b.कुञ्चिका);
                यदि (r < 0)
                        प्रत्यागम r;
        }

        प्रत्यागम 0;
}

पूर्णाङ्क _hashmap_reserve(Hashmap_आधारः *h, अचिह्नित entries_सङ्कलय) {
        पूर्णाङ्क r;

        प्रतिज्ञा(h);

        r = resize_buckets(h, entries_सङ्कलय);
        यदि (r < 0)
                प्रत्यागम r;

        प्रत्यागम 0;
}

/*
 * The same as hashmap_merge(), but every new item from other is moved to h.
 * Keys already in h are skipped and stay in other.
 * Returns: 0 on success.
 *          -ENOMEM on alloc failure, in which case no move has been done.
 */
पूर्णाङ्क _hashmap_move(Hashmap_आधारः *h, Hashmap_आधारः *other) {
        संरचना swap_entries swap;
        संरचना hashmap_आधारः_प्रविष्टिः *e, *n;
        Iterator i;
        अचिह्नित idx;
        पूर्णाङ्क r;

        प्रतिज्ञा(h);

        यदि (!other)
                प्रत्यागम 0;

        प्रतिज्ञा(other->type == h->type);

        /*
         * This reserves buckets for the worst case, where none of other's
         * entries are yet present in h. This is preferable to risking
         * an allocation failure in the middle of the moving and having to
         * rollback or return a partial result.
         */
        r = resize_buckets(h, n_entries(other));
        यदि (r < 0)
                प्रत्यागम r;

        HASHMAP_FOREACH_IDX(idx, other, i) {
                अचिह्नित h_hash;

                e = bucket_at(other, idx);
                h_hash = bucket_hash(h, e->कुञ्चिका);
                यदि (bucket_अवलोकनम्(h, h_hash, e->कुञ्चिका) != IDX_NIL)
                        अनुवर्त;

                n = &bucket_at_swap(&swap, IDX_PUT)->p.b;
                n->कुञ्चिका = e->कुञ्चिका;
                यदि (h->type != HASHMAP_TYPE_समुच्चयः)
                        ((संरचना plain_hashmap_प्रविष्टिः*) n)->मानम् =
                                ((संरचना plain_hashmap_प्रविष्टिः*) e)->मानम्;
                assert_se(hashmap_put_boldly(h, h_hash, &swap, false) == 1);

                remove_प्रविष्टिः(other, idx);
        }

        प्रत्यागम 0;
}

पूर्णाङ्क _hashmap_move_one(Hashmap_आधारः *h, Hashmap_आधारः *other, ध्रुव शून्य *कुञ्चिका) {
        संरचना swap_entries swap;
        अचिह्नित h_hash, other_hash, idx;
        संरचना hashmap_आधारः_प्रविष्टिः *e, *n;
        पूर्णाङ्क r;

        प्रतिज्ञा(h);

        h_hash = bucket_hash(h, कुञ्चिका);
        यदि (bucket_अवलोकनम्(h, h_hash, कुञ्चिका) != IDX_NIL)
                प्रत्यागम -EEXIST;

        यदि (!other)
                प्रत्यागम -ENOENT;

        प्रतिज्ञा(other->type == h->type);

        other_hash = bucket_hash(other, कुञ्चिका);
        idx = bucket_अवलोकनम्(other, other_hash, कुञ्चिका);
        यदि (idx == IDX_NIL)
                प्रत्यागम -ENOENT;

        e = bucket_at(other, idx);

        n = &bucket_at_swap(&swap, IDX_PUT)->p.b;
        n->कुञ्चिका = e->कुञ्चिका;
        यदि (h->type != HASHMAP_TYPE_समुच्चयः)
                ((संरचना plain_hashmap_प्रविष्टिः*) n)->मानम् =
                        ((संरचना plain_hashmap_प्रविष्टिः*) e)->मानम्;
        r = hashmap_put_boldly(h, h_hash, &swap, true);
        यदि (r < 0)
                प्रत्यागम r;

        remove_प्रविष्टिः(other, idx);
        प्रत्यागम 0;
}

Hashmap_आधारः* _hashmap_copy(Hashmap_आधारः *h) {
        Hashmap_आधारः *copy;
        पूर्णाङ्क r;

        प्रतिज्ञा(h);

        copy = hashmap_आधारः_new(h->hash_ops, h->type);
        यदि (!copy)
                प्रत्यागम रिक्त;

        विकल्प (h->type) {
        पक्ष HASHMAP_TYPE_PLAIN:
        पक्ष HASHMAP_TYPE_ORDERED:
                r = hashmap_merge((Hashmap*)copy, (Hashmap*)h);
                भङ्ग;
        पक्ष HASHMAP_TYPE_समुच्चयः:
                r = समुच्चयः_merge((समुच्चयः_2*)copy, (समुच्चयः_2*)h);
                भङ्ग;
        स्वभाव:
                assert_not_reached();
        }

        यदि (r < 0)
                प्रत्यागम _hashmap_free(copy);

        प्रत्यागम copy;
}

वर्ण** _hashmap_get_strv(Hashmap_आधारः *h) {
        वर्ण **sv;
        Iterator i;
        अचिह्नित idx, n;

        यदि (!h)
                प्रत्यागम new0(वर्ण*, 1);

        sv = new(वर्ण*, n_entries(h)+1);
        यदि (!sv)
                प्रत्यागम रिक्त;

        n = 0;
        HASHMAP_FOREACH_IDX(idx, h, i)
                sv[n++] = प्रविष्टिः_मानम्(h, bucket_at(h, idx));
        sv[n] = रिक्त;

        प्रत्यागम sv;
}

वर्ण** समुच्चयः_गन्तव्यम्_strv(समुच्चयः_2 **s) {
        प्रतिज्ञा(s);

        /* This is similar to set_get_strv(), but invalidates the set on success. */

        वर्ण **v = new(वर्ण*, समुच्चयः_आकार(*s) + 1);
        यदि (!v)
                प्रत्यागम रिक्त;

        प्रति (वर्ण **p = v; (*p = समुच्चयः_steal_first(*s)); p++)
                ;

        प्रतिज्ञा(समुच्चयः_isempty(*s));
        *s = समुच्चयः_free(*s);
        प्रत्यागम v;
}

शून्य* ordered_hashmap_अग्र(OrderedHashmap *h, ध्रुव शून्य *कुञ्चिका) {
        संरचना ordered_hashmap_प्रविष्टिः *e;
        अचिह्नित hash, idx;

        यदि (!h)
                प्रत्यागम रिक्त;

        hash = bucket_hash(h, कुञ्चिका);
        idx = bucket_अवलोकनम्(h, hash, कुञ्चिका);
        यदि (idx == IDX_NIL)
                प्रत्यागम रिक्त;

        e = ordered_bucket_at(h, idx);
        यदि (e->iterate_अग्र == IDX_NIL)
                प्रत्यागम रिक्त;
        प्रत्यागम ordered_bucket_at(h, e->iterate_अग्र)->p.मानम्;
}

पूर्णाङ्क समुच्चयः_consume(समुच्चयः_2 *s, शून्य *मानम्) {
        पूर्णाङ्क r;

        प्रतिज्ञा(s);
        प्रतिज्ञा(मानम्);

        r = समुच्चयः_put(s, मानम्);
        यदि (r <= 0)
                मुक्त(मानम्);

        प्रत्यागम r;
}

पूर्णाङ्क hashmap_put_strdup_पूर्णम्(Hashmap **h, ध्रुव संरचना hash_ops *hash_ops, ध्रुव वर्ण *k, ध्रुव वर्ण *v) {
        पूर्णाङ्क r;

        प्रतिज्ञा(h);

        r = hashmap_ensure_allocated(h, hash_ops);
        यदि (r < 0)
                प्रत्यागम r;

        _cleanup_free_ वर्ण *kdup = रिक्त, *vdup = रिक्त;

        kdup = strdup(k);
        यदि (!kdup)
                प्रत्यागम -ENOMEM;

        यदि (v) {
                vdup = strdup(v);
                यदि (!vdup)
                        प्रत्यागम -ENOMEM;
        }

        r = hashmap_put(*h, kdup, vdup);
        यदि (r < 0) {
                यदि (r == -EEXIST && streq_ptr(v, hashmap_get(*h, kdup)))
                        प्रत्यागम 0;
                प्रत्यागम r;
        }

        /* 0 with non-null vdup would mean vdup is already in the hashmap, which cannot be */
        प्रतिज्ञा(vdup == रिक्त || r > 0);
        यदि (r > 0)
                kdup = vdup = रिक्त;

        प्रत्यागम r;
}

पूर्णाङ्क समुच्चयः_put_strndup_पूर्णम्(समुच्चयः_2 **s, ध्रुव संरचना hash_ops *hash_ops, ध्रुव वर्ण *p, size_t n) {
        वर्ण *c;
        पूर्णाङ्क r;

        प्रतिज्ञा(s);
        प्रतिज्ञा(p);

        r = समुच्चयः_ensure_allocated(s, hash_ops);
        यदि (r < 0)
                प्रत्यागम r;

        यदि (n == SIZE_MAX) {
                यदि (समुच्चयः_अन्तर्भवति(*s, (वर्ण*) p))
                        प्रत्यागम 0;

                c = strdup(p);
        } अन्यथा
                c = strndup(p, n);
        यदि (!c)
                प्रत्यागम -ENOMEM;

        प्रत्यागम समुच्चयः_consume(*s, c);
}

पूर्णाङ्क समुच्चयः_put_strdupv_पूर्णम्(समुच्चयः_2 **s, ध्रुव संरचना hash_ops *hash_ops, वर्ण **l) {
        पूर्णाङ्क n = 0, r;

        प्रतिज्ञा(s);

        STRV_FOREACH(i, l) {
                r = समुच्चयः_put_strndup_पूर्णम्(s, hash_ops, *i, SIZE_MAX);
                यदि (r < 0)
                        प्रत्यागम r;

                n += r;
        }

        प्रत्यागम n;
}

पूर्णाङ्क समुच्चयः_put_strsplit(समुच्चयः_2 *s, ध्रुव वर्ण *v, ध्रुव वर्ण *separators, ExtractFlags flags) {
        ध्रुव वर्ण *p = ASSERT_PTR(v);
        पूर्णाङ्क r;

        प्रतिज्ञा(s);

        प्रति (;;) {
                वर्ण *word;

                r = extract_first_word(&p, &word, separators, flags);
                यदि (r <= 0)
                        प्रत्यागम r;

                r = समुच्चयः_consume(s, word);
                यदि (r < 0)
                        प्रत्यागम r;
        }
}

/* expand the cachemem if needed, return true if newly (re)activated. */
स्थिर पूर्णाङ्क cachemem_maintain(CacheMem *mem, size_t आकार) {
        प्रतिज्ञा(mem);

        यदि (!GREEDY_REALLOC(mem->ptr, आकार)) {
                यदि (आकार > 0)
                        प्रत्यागम -ENOMEM;
        }

        यदि (!mem->सक्रिय) {
                mem->सक्रिय = true;
                प्रत्यागम true;
        }

        प्रत्यागम false;
}

पूर्णाङ्क iterated_cache_get(IteratedCache *cache, ध्रुव शून्य ***res_कुञ्चिकाः, ध्रुव शून्य ***res_मानानि, अचिह्नित *res_n_entries) {
        bool sync_कुञ्चिकाः = false, sync_मानानि = false;
        size_t आकार;
        पूर्णाङ्क r;

        प्रतिज्ञा(cache);
        प्रतिज्ञा(cache->hashmap);

        आकार = n_entries(cache->hashmap);

        यदि (res_कुञ्चिकाः) {
                r = cachemem_maintain(&cache->कुञ्चिकाः, आकार);
                यदि (r < 0)
                        प्रत्यागम r;

                sync_कुञ्चिकाः = r;
        } अन्यथा
                cache->कुञ्चिकाः.सक्रिय = false;

        यदि (res_मानानि) {
                r = cachemem_maintain(&cache->values, आकार);
                यदि (r < 0)
                        प्रत्यागम r;

                sync_मानानि = r;
        } अन्यथा
                cache->values.सक्रिय = false;

        यदि (cache->hashmap->dirty) {
                यदि (cache->कुञ्चिकाः.सक्रिय)
                        sync_कुञ्चिकाः = true;
                यदि (cache->values.सक्रिय)
                        sync_मानानि = true;

                cache->hashmap->dirty = false;
        }

        यदि (sync_कुञ्चिकाः || sync_मानानि) {
                अचिह्नित i, idx;
                Iterator iter;

                i = 0;
                HASHMAP_FOREACH_IDX(idx, cache->hashmap, iter) {
                        संरचना hashmap_आधारः_प्रविष्टिः *e;

                        e = bucket_at(cache->hashmap, idx);

                        यदि (sync_कुञ्चिकाः)
                                cache->कुञ्चिकाः.ptr[i] = e->कुञ्चिका;
                        यदि (sync_मानानि)
                                cache->values.ptr[i] = प्रविष्टिः_मानम्(cache->hashmap, e);
                        i++;
                }
        }

        यदि (res_कुञ्चिकाः)
                *res_कुञ्चिकाः = cache->कुञ्चिकाः.ptr;
        यदि (res_मानानि)
                *res_मानानि = cache->values.ptr;
        यदि (res_n_entries)
                *res_n_entries = आकार;

        प्रत्यागम 0;
}

IteratedCache* iterated_cache_free(IteratedCache *cache) {
        यदि (cache) {
                मुक्त(cache->कुञ्चिकाः.ptr);
                मुक्त(cache->values.ptr);
        }

        प्रत्यागम mfree(cache);
}

पूर्णाङ्क समुच्चयः_strjoin(समुच्चयः_2 *s, ध्रुव वर्ण *separator, bool wrap_with_separator, वर्ण **ret) {
        _cleanup_free_ वर्ण *str = रिक्त;
        size_t separator_len, len = 0;
        ध्रुव वर्ण *मानम्;
        bool first;

        प्रतिज्ञा(ret);

        यदि (समुच्चयः_isempty(s)) {
                *ret = रिक्त;
                प्रत्यागम 0;
        }

        separator_len = strlen_ptr(separator);

        यदि (separator_len == 0)
                wrap_with_separator = false;

        first = !wrap_with_separator;

        समुच्चयः_FOREACH(मानम्, s) {
                size_t l = strlen_ptr(मानम्);

                यदि (l == 0)
                        अनुवर्त;

                यदि (!GREEDY_REALLOC(str, len + l + (first ? 0 : separator_len) + (wrap_with_separator ? separator_len : 0) + 1))
                        प्रत्यागम -ENOMEM;

                यदि (separator_len > 0 && !first) {
                        स्मृतिं_प्रतिलिख(str + len, separator, separator_len);
                        len += separator_len;
                }

                स्मृतिं_प्रतिलिख(str + len, मानम्, l);
                len += l;
                first = false;
        }

        यदि (wrap_with_separator) {
                स्मृतिं_प्रतिलिख(str + len, separator, separator_len);
                len += separator_len;
        }

        str[len] = '\0';

        *ret = TAKE_PTR(str);
        प्रत्यागम 0;
}

bool समुच्चयः_equal(समुच्चयः_2 *a, समुच्चयः_2 *b) {
        शून्य *p;

        /* Checks whether each entry of 'a' is also in 'b' and vice versa, i.e. the two sets contain the same
         * entries */

        यदि (a == b)
                प्रत्यागम true;

        यदि (समुच्चयः_isempty(a) && समुच्चयः_isempty(b))
                प्रत्यागम true;

        यदि (समुच्चयः_आकार(a) != समुच्चयः_आकार(b)) /* Cheap check that hopefully catches a lot of inequality cases
                                         * already */
                प्रत्यागम false;

        समुच्चयः_FOREACH(p, a)
                यदि (!समुच्चयः_अन्तर्भवति(b, p))
                        प्रत्यागम false;

        /* If we have the same hashops, then we don't need to check things backwards given we compared the
         * size and that all of a is in b. */
        यदि (a->b.hash_ops == b->b.hash_ops)
                प्रत्यागम true;

        समुच्चयः_FOREACH(p, b)
                यदि (!समुच्चयः_अन्तर्भवति(a, p))
                        प्रत्यागम false;

        प्रत्यागम true;
}

स्थिर bool समुच्चयः_fnmatch_one(समुच्चयः_2 *patterns, ध्रुव वर्ण *needle) {
        ध्रुव वर्ण *p;

        प्रतिज्ञा(needle);

        /* Any failure of fnmatch() is treated as equivalent to FNM_NOMATCH, i.e. as non-matching pattern */

        समुच्चयः_FOREACH(p, patterns)
                यदि (fnmatch(p, needle, 0) == 0)
                        प्रत्यागम true;

        प्रत्यागम false;
}

bool समुच्चयः_fnmatch(समुच्चयः_2 *include_patterns, समुच्चयः_2 *exclude_patterns, ध्रुव वर्ण *needle) {
        प्रतिज्ञा(needle);

        यदि (समुच्चयः_fnmatch_one(exclude_patterns, needle))
                प्रत्यागम false;

        यदि (समुच्चयः_isempty(include_patterns))
                प्रत्यागम true;

        प्रत्यागम समुच्चयः_fnmatch_one(include_patterns, needle);
}

स्थिर पूर्णाङ्क hashmap_प्रविष्टिः_compare(
                संरचना hashmap_आधारः_प्रविष्टिः * ध्रुव *a,
                संरचना hashmap_आधारः_प्रविष्टिः * ध्रुव *b,
                compare_func_t compare) {

        प्रतिज्ञा(a && *a);
        प्रतिज्ञा(b && *b);
        प्रतिज्ञा(compare);

        प्रत्यागम compare((*a)->कुञ्चिका, (*b)->कुञ्चिका);
}

स्थिर पूर्णाङ्क _hashmap_dump_entries_sorted(
                Hashmap_आधारः *h,
                शून्य ***ret,
                size_t *ret_n) {
        _cleanup_free_ शून्य **entries = रिक्त;
        Iterator iter;
        अचिह्नित idx;
        size_t n = 0;

        प्रतिज्ञा(ret);
        प्रतिज्ञा(ret_n);

        यदि (hashmap_आकार(h) == 0) {
                *ret = रिक्त;
                *ret_n = 0;
                प्रत्यागम 0;
        }

        /* We append one more element than needed so that the resulting array can be used as a strv. We
         * don't count this entry in the returned size. */
        entries = new(शून्य*, hashmap_आकार(h) + 1);
        यदि (!entries)
                प्रत्यागम -ENOMEM;

        HASHMAP_FOREACH_IDX(idx, h, iter)
                entries[n++] = bucket_at(h, idx);

        प्रतिज्ञा(n == hashmap_आकार(h));
        entries[n] = रिक्त;

        typesafe_qsort_r((संरचना hashmap_आधारः_प्रविष्टिः**) entries, n,
                         hashmap_प्रविष्टिः_compare, h->hash_ops->compare);

        *ret = TAKE_PTR(entries);
        *ret_n = n;
        प्रत्यागम 0;
}

पूर्णाङ्क hashmap_dump_कुञ्चिकाः_sorted(Hashmap_आधारः *h, शून्य ***ret, size_t *ret_n) {
        _cleanup_free_ शून्य **entries = रिक्त;
        size_t n;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        r = _hashmap_dump_entries_sorted(h, &entries, &n);
        यदि (r < 0)
                प्रत्यागम r;

        /* Reuse the array. */
        FOREACH_ARRAY(e, entries, n)
                *e = (शून्य*) (*(संरचना hashmap_आधारः_प्रविष्टिः**) e)->कुञ्चिका;

        *ret = TAKE_PTR(entries);
        यदि (ret_n)
                *ret_n = n;
        प्रत्यागम 0;
}

पूर्णाङ्क _hashmap_dump_sorted(Hashmap_आधारः *h, शून्य ***ret, size_t *ret_n) {
        _cleanup_free_ शून्य **entries = रिक्त;
        size_t n;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        r = _hashmap_dump_entries_sorted(h, &entries, &n);
        यदि (r < 0)
                प्रत्यागम r;

        /* Reuse the array. */
        FOREACH_ARRAY(e, entries, n)
                *e = प्रविष्टिः_मानम्(h, *(संरचना hashmap_आधारः_प्रविष्टिः**) e);

        *ret = TAKE_PTR(entries);
        यदि (ret_n)
                *ret_n = n;
        प्रत्यागम 0;
}
