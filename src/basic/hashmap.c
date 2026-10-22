/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ <fnmatch.h>
#అన్తర్భావ <pthread.h>
#అన్తర్భావ <unistd.h>
#ఒకవేళ HAVE_VALGRIND_VALGRIND_H
#  include <valgrind/valgrind.h>
#అయితే_ముగింపు

#అన్తర్భావ "alloc-util.h"
#అన్తర్భావ "extract-word.h"
#అన్తర్భావ "hashmap.h"
#అన్తర్భావ "log.h"
#అన్తర్భావ "logarithm.h"
#అన్తర్భావ "memory-util.h"
#అన్తర్భావ "mempool.h"
#అన్తర్భావ "process-util.h"
#అన్తర్భావ "random-util.h"
#అన్తర్భావ "set.h"
#అన్తర్భావ "siphash24.h"
#అన్తర్భావ "sort-util.h"
#అన్తర్భావ "string-util.h"
#అన్తర్భావ "strv.h"

#ఒకవేళ ENABLE_DEBUG_HASHMAP
#అన్తర్భావ "list.h"
#అయితే_ముగింపు

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
#నిర్వచించు INV_KEEP_FREE            5U

/* Fields common to entries of all hashmap/set types */
నిర్మాణం hashmap_పునాది_నమోదు {
        స్థిరరాశి శూన్య *కీలకం;
};

/* Entry types for specific hashmap/set types
 * hashmap_base_entry must be at the beginning of each entry struct. */

నిర్మాణం plain_hashmap_నమోదు {
        నిర్మాణం hashmap_పునాది_నమోదు b;
        శూన్య *విలువ;
};

నిర్మాణం ordered_hashmap_నమోదు {
        నిర్మాణం plain_hashmap_నమోదు p;
        గుర్తులేని iterate_తదుపరి, iterate_మునుపటి;
};

నిర్మాణం సమితి_నమోదు {
        నిర్మాణం hashmap_పునాది_నమోదు b;
};

/* In several functions it is advantageous to have the hash table extended
 * virtually by a couple of additional buckets. We reserve special index values
 * for these "swap" buckets. */
#నిర్వచించు IDX_మార్పిడి_BEGIN     (UINT_MAX - 3)
#నిర్వచించు IDX_PUT             (IDX_మార్పిడి_BEGIN + 0)
#నిర్వచించు IDX_TMP             (IDX_మార్పిడి_BEGIN + 1)
#నిర్వచించు IDX_మార్పిడి_చివర       (IDX_మార్పిడి_BEGIN + 2)

#నిర్వచించు IDX_మొదటి           (UINT_MAX - 1) /* special index for freshly initialized iterators */
#నిర్వచించు IDX_NIL             UINT_MAX       /* special index value meaning "none" or "end" */

assert_cc(IDX_మొదటి == IDX_మార్పిడి_చివర);
assert_cc(IDX_మొదటి == IDX_ITERATOR_మొదటి);

/* Storage space for the "swap" buckets.
 * All entry types can fit into an ordered_hashmap_entry. */
నిర్మాణం మార్పిడి_entries {
        నిర్మాణం ordered_hashmap_నమోదు e[IDX_మార్పిడి_చివర - IDX_మార్పిడి_BEGIN];
};

/* Distance from Initial Bucket */
రకనిర్వచనం uint8_t dib_raw_t;
#నిర్వచించు DIB_RAW_అధికప్రవాహం ((dib_raw_t)0xfdU)   /* indicates DIB value is greater than representable */
#నిర్వచించు DIB_RAW_REHASH   ((dib_raw_t)0xfeU)   /* entry yet to be rehashed during in-place resize */
#నిర్వచించు DIB_RAW_FREE     ((dib_raw_t)0xffU)   /* a free bucket */
#నిర్వచించు DIB_RAW_INIT     ((అక్షరం)DIB_RAW_FREE) /* a byte to memset a DIB store with when initializing */

#నిర్వచించు DIB_FREE UINT_MAX

#ఒకవేళ ENABLE_DEBUG_HASHMAP
నిర్మాణం hashmap_debug_info {
        LIST_FIELDS(నిర్మాణం hashmap_debug_info, debug_list);
        గుర్తులేని max_entries;  /* high watermark of n_entries */

        /* fields to detect modification while iterating */
        గుర్తులేని put_లెక్క;    /* counts puts into the hashmap */
        గుర్తులేని rem_లెక్క;    /* counts removals from hashmap */
        గుర్తులేని చివరి_rem_idx; /* remembers last removal index */
};

/* Tracks all existing hashmaps. Get at it from gdb. See sd_dump_hashmaps.py */
స్థిర LIST_తల(నిర్మాణం hashmap_debug_info, hashmap_debug_list);
స్థిర pthread_mutex_t hashmap_debug_list_mutex = PTHREAD_MUTEX_INITIALIZER;
#అయితే_ముగింపు

గణన HashmapType {
        HASHMAP_TYPE_PLAIN,
        HASHMAP_TYPE_ORDERED,
        HASHMAP_TYPE_సమితి,
        _HASHMAP_TYPE_MAX
};

నిర్మాణం _packed_ పరోక్ష_storage {
        శూన్య *storage;                     /* where buckets and DIBs are stored */
        uint8_t  hash_కీలకం[HASH_కీలకం_కొలత];  /* hash key; changes during resize */

        గుర్తులేని n_entries;                /* number of stored entries */
        గుర్తులేని n_buckets;                /* number of buckets */

        గుర్తులేని idx_అత్యల్ప_నమోదు;         /* Index below which all buckets are free.
                                              Makes "while (hashmap_steal_first())" loops
                                              O(n) instead of O(n^2) for unordered hashmaps. */
        uint8_t  _pad[3];                  /* padding for the whole HashmapBase */
        /* The bitfields in HashmapBase complete the alignment of the whole thing. */
};

నిర్మాణం ప్రత్యక్ష_storage {
        /* This gives us 39 bytes on 64-bit, or 35 bytes on 32-bit.
         * That's room for 4 set_entries + 4 DIB bytes + 3 unused bytes on 64-bit,
         *              or 7 set_entries + 7 DIB bytes + 0 unused bytes on 32-bit. */
        uint8_t storage[పరిమాణం(నిర్మాణం పరోక్ష_storage)];
};

#నిర్వచించు ప్రత్యక్ష_BUCKETS(నమోదు_t) \
        (పరిమాణం(నిర్మాణం ప్రత్యక్ష_storage) / (పరిమాణం(నమోదు_t) + పరిమాణం(dib_raw_t)))

/* We should be able to store at least one entry directly. */
assert_cc(ప్రత్యక్ష_BUCKETS(నిర్మాణం ordered_hashmap_నమోదు) >= 1);

/* We have 3 bits for n_direct_entries. */
assert_cc(ప్రత్యక్ష_BUCKETS(నిర్మాణం సమితి_నమోదు) < (1 << 3));

/* Hashmaps with directly stored entries all use this shared hash key.
 * It's no big deal if the key is guessed, because there can be only
 * a handful of directly stored entries in a hashmap. When a hashmap
 * outgrows direct storage, it gets its own key for indirect storage. */
స్థిర uint8_t shared_hash_కీలకం[HASH_కీలకం_కొలత];

/* Fields that all hashmap/set types must have */
నిర్మాణం Hashmap_పునాది {
        స్థిరరాశి నిర్మాణం hash_ops *hash_ops;  /* hash and compare ops to use */

        సంఘం _packed_ {
                నిర్మాణం పరోక్ష_storage పరోక్ష; /* if  has_indirect */
                నిర్మాణం ప్రత్యక్ష_storage ప్రత్యక్ష;     /* if !has_indirect */
        };

        గణన HashmapType type:2;     /* HASHMAP_TYPE_* */
        bool has_పరోక్ష:1;         /* whether indirect storage is used */
        గుర్తులేని n_ప్రత్యక్ష_entries:3; /* Number of entries in direct storage.
                                      * Only valid if !has_indirect. */
        bool from_pool:1;            /* whether was allocated from mempool */
        bool dirty:1;                /* whether dirtied since last iterated_cache_get() */
        bool cached:1;               /* whether this hashmap is being cached */

#ఒకవేళ ENABLE_DEBUG_HASHMAP
        నిర్మాణం hashmap_debug_info debug;
#అయితే_ముగింపు
};

/* Specific hash types
 * HashmapBase must be at the beginning of each hashmap struct. */

నిర్మాణం Hashmap {
        నిర్మాణం Hashmap_పునాది b;
};

నిర్మాణం OrderedHashmap {
        నిర్మాణం Hashmap_పునాది b;
        గుర్తులేని iterate_list_తల, iterate_list_తోక;
};

నిర్మాణం సమితి_2 {
        నిర్మాణం Hashmap_పునాది b;
};

రకనిర్వచనం నిర్మాణం CacheMem {
        స్థిరరాశి శూన్య **ptr;
        size_t n_populated;
        bool చురుకైన:1;
} CacheMem;

నిర్మాణం IteratedCache {
        Hashmap_పునాది *hashmap;
        CacheMem కీలకాలు, values;
};

DEFINE_MEMPOOL(hashmap_pool,         Hashmap,        8);
DEFINE_MEMPOOL(ordered_hashmap_pool, OrderedHashmap, 8);
/* No need for a separate Set pool */
assert_cc(పరిమాణం(Hashmap) == పరిమాణం(సమితి_2));

నిర్మాణం hashmap_type_info {
        size_t తల_కొలత;
        size_t నమోదు_కొలత;
        నిర్మాణం mempool *mempool;
        గుర్తులేని n_ప్రత్యక్ష_buckets;
};

స్థిర వాడిన_2 స్థిరరాశి నిర్మాణం hashmap_type_info hashmap_type_info[_HASHMAP_TYPE_MAX] = {
        [HASHMAP_TYPE_PLAIN] = {
                .తల_కొలత        = పరిమాణం(Hashmap),
                .నమోదు_కొలత       = పరిమాణం(నిర్మాణం plain_hashmap_నమోదు),
                .mempool          = &hashmap_pool,
                .n_ప్రత్యక్ష_buckets = ప్రత్యక్ష_BUCKETS(నిర్మాణం plain_hashmap_నమోదు),
        },
        [HASHMAP_TYPE_ORDERED] = {
                .తల_కొలత        = పరిమాణం(OrderedHashmap),
                .నమోదు_కొలత       = పరిమాణం(నిర్మాణం ordered_hashmap_నమోదు),
                .mempool          = &ordered_hashmap_pool,
                .n_ప్రత్యక్ష_buckets = ప్రత్యక్ష_BUCKETS(నిర్మాణం ordered_hashmap_నమోదు),
        },
        [HASHMAP_TYPE_సమితి] = {
                .తల_కొలత        = పరిమాణం(సమితి_2),
                .నమోదు_కొలత       = పరిమాణం(నిర్మాణం సమితి_నమోదు),
                .mempool          = &hashmap_pool,
                .n_ప్రత్యక్ష_buckets = ప్రత్యక్ష_BUCKETS(నిర్మాణం సమితి_నమోదు),
        },
};

శూన్య hashmap_trim_pools(శూన్య) {
        పూర్ణాంకం r;

        /* The pool is only allocated by the main thread, but the memory can be passed to other
         * threads. Let's clean up if we are the main thread and no other threads are live. */

        /* We build our own is_main_thread() here, which doesn't use C11 TLS based caching of the
         * result. That's because valgrind apparently doesn't like TLS to be used from a GCC destructor. */
        ఒకవేళ (getpid() != gettid())
                తిరిగి (శూన్య) log_debug("Not cleaning up memory pools, not in main thread.");

        r = get_process_దారాలు(0);
        ఒకవేళ (r < 0)
                తిరిగి (శూన్య) log_debug_errno(r, "Failed to determine number of threads, not cleaning up memory pools: %m");
        ఒకవేళ (r != 1)
                తిరిగి (శూన్య) log_debug("Not cleaning up memory pools, running in multi-threaded process.");

        mempool_trim(&hashmap_pool);
        mempool_trim(&ordered_hashmap_pool);
}

#ఒకవేళ HAVE_VALGRIND_VALGRIND_H
_destructor_ స్థిర శూన్య cleanup_pools(శూన్య) {
        /* Be nice to valgrind */
        ఒకవేళ (RUNNING_ON_VALGRIND)
                hashmap_trim_pools();
}
#అయితే_ముగింపు

స్థిర అంతఃవరుస గుర్తులేని n_buckets(Hashmap_పునాది *h) {
        ప్రతిజ్ఞా(h);
        తిరిగి h->has_పరోక్ష ? h->పరోక్ష.n_buckets
                               : hashmap_type_info[h->type].n_ప్రత్యక్ష_buckets;
}

స్థిర గుర్తులేని n_entries(Hashmap_పునాది *h) {
        ప్రతిజ్ఞా(h);
        తిరిగి h->has_పరోక్ష ? h->పరోక్ష.n_entries
                               : h->n_ప్రత్యక్ష_entries;
}

స్థిర శూన్య n_entries_inc(Hashmap_పునాది *h) {
        ప్రతిజ్ఞా(h);

        ఒకవేళ (h->has_పరోక్ష)
                h->పరోక్ష.n_entries++;
        లేకపోతే
                h->n_ప్రత్యక్ష_entries++;
}

స్థిర శూన్య n_entries_dec(Hashmap_పునాది *h) {
        ప్రతిజ్ఞా(h);

        ఒకవేళ (h->has_పరోక్ష)
                h->పరోక్ష.n_entries--;
        లేకపోతే
                h->n_ప్రత్యక్ష_entries--;
}

స్థిర అంతఃవరుస శూన్య* storage_ptr(Hashmap_పునాది *h) {
        ప్రతిజ్ఞా(h);
        తిరిగి h->has_పరోక్ష ? h->పరోక్ష.storage
                               : h->ప్రత్యక్ష.storage;
}

స్థిర అంతఃవరుస uint8_t* hash_కీలకం(Hashmap_పునాది *h) {
        ప్రతిజ్ఞా(h);
        తిరిగి h->has_పరోక్ష ? h->పరోక్ష.hash_కీలకం
                               : shared_hash_కీలకం;
}

స్థిర గుర్తులేని పునాది_పాత్ర_hash(Hashmap_పునాది *h, స్థిరరాశి శూన్య *p) {
        నిర్మాణం siphash state;
        uint64_t hash;

        ప్రతిజ్ఞా(h);

        /* The keys of trivial_hash_ops and uint64_hash_ops fit in 64 bits. siphash24_uint64() hashes such
         * a key without the indirect call through hash_ops and keeps the SipHash state in registers. On
         * 32-bit and big-endian architectures, the result differs from the result of trivial_hash_func(),
         * which hashes the key in native byte order. Only this function maps keys to buckets, so the
         * difference does not matter. */
        ఒకవేళ (h->hash_ops->hash == trivial_hash_func)
                hash = siphash24_uint64((uintptr_t) p, hash_కీలకం(h));
        లేకపోతే ఒకవేళ (h->hash_ops->hash == (hash_func_t) uint64_hash_func)
                hash = siphash24_uint64(*(స్థిరరాశి uint64_t*) p, hash_కీలకం(h));
        లేకపోతే {
                siphash24_init(&state, hash_కీలకం(h));
                h->hash_ops->hash(p, &state);
                hash = siphash24_finalize(&state);
        }

        /* n_buckets is not a power of two, so a modulo needs a division. Multiplying the upper 32 bits of
         * the hash with n_buckets and keeping the upper 32 bits of the product maps the hash to
         * [0, n_buckets) almost as evenly, without a division. */
        తిరిగి (గుర్తులేని) (((hash >> 32) * n_buckets(h)) >> 32);
}
#నిర్వచించు పాత్ర_hash(h, p) పునాది_పాత్ర_hash(HASHMAP_పునాది(h), p)

స్థిర శూన్య పునాది_సమితి_dirty(Hashmap_పునాది *h) {
        ప్రతిజ్ఞా(h);

        h->dirty = true;
}
#నిర్వచించు hashmap_సమితి_dirty(h) పునాది_సమితి_dirty(HASHMAP_పునాది(h))

స్థిర శూన్య get_hash_కీలకం(uint8_t hash_కీలకం[HASH_కీలకం_కొలత], bool reuse_is_ok) {
        స్థిర uint8_t current[HASH_కీలకం_కొలత];
        స్థిర bool current_initialized = false;

        /* Returns a hash function key to use. In order to keep things
         * fast we will not generate a new key each time we allocate a
         * new hash table. Instead, we'll just reuse the most recently
         * generated one, except if we never generated one or when we
         * are rehashing an entire hash table because we reached a
         * fill level */

        ఒకవేళ (!current_initialized || !reuse_is_ok) {
                random_బైట్లు(current, పరిమాణం(current));
                current_initialized = true;
        }

        జ్ఞాపకం_నకలు(hash_కీలకం, current, పరిమాణం(current));
}

స్థిర అంతఃవరుస నిర్మాణం hashmap_పునాది_నమోదు* పాత్ర_at(Hashmap_పునాది *h, గుర్తులేని idx) {
        ప్రతిజ్ఞా(h);
        తిరిగి CAST_ALIGN_PTR(
                        నిర్మాణం hashmap_పునాది_నమోదు,
                        (uint8_t *) storage_ptr(h) + idx * hashmap_type_info[h->type].నమోదు_కొలత);
}

స్థిర నిర్మాణం plain_hashmap_నమోదు* plain_పాత్ర_at(Hashmap *h, గుర్తులేని idx) {
        తిరిగి (నిర్మాణం plain_hashmap_నమోదు*) పాత్ర_at(HASHMAP_పునాది(h), idx);
}

స్థిర నిర్మాణం ordered_hashmap_నమోదు* ordered_పాత్ర_at(OrderedHashmap *h, గుర్తులేని idx) {
        తిరిగి (నిర్మాణం ordered_hashmap_నమోదు*) పాత్ర_at(HASHMAP_పునాది(h), idx);
}

స్థిర నిర్మాణం సమితి_నమోదు *సమితి_పాత్ర_at(సమితి_2 *h, గుర్తులేని idx) {
        తిరిగి (నిర్మాణం సమితి_నమోదు*) పాత్ర_at(HASHMAP_పునాది(h), idx);
}

స్థిర నిర్మాణం ordered_hashmap_నమోదు* పాత్ర_at_మార్పిడి(నిర్మాణం మార్పిడి_entries *మార్పిడి, గుర్తులేని idx) {
        ప్రతిజ్ఞా(మార్పిడి);
        తిరిగి &మార్పిడి->e[idx - IDX_మార్పిడి_BEGIN];
}

/* Returns a pointer to the bucket at index idx.
 * Understands real indexes and swap indexes, hence "_virtual". */
స్థిర నిర్మాణం hashmap_పునాది_నమోదు* పాత్ర_at_virtual(Hashmap_పునాది *h, నిర్మాణం మార్పిడి_entries *మార్పిడి,
                                                    గుర్తులేని idx) {
        ఒకవేళ (idx < IDX_మార్పిడి_BEGIN)
                తిరిగి పాత్ర_at(h, idx);

        ఒకవేళ (idx < IDX_మార్పిడి_చివర)
                తిరిగి &పాత్ర_at_మార్పిడి(మార్పిడి, idx)->p.b;

        assert_not_reached();
}

స్థిర అంతఃవరుస dib_raw_t* dib_raw_ptr(Hashmap_పునాది *h) {
        ప్రతిజ్ఞా(h);
        తిరిగి (dib_raw_t*)
                ((uint8_t*) storage_ptr(h) + hashmap_type_info[h->type].నమోదు_కొలత * n_buckets(h));
}

స్థిర గుర్తులేని పాత్ర_దూరం(Hashmap_పునాది *h, గుర్తులేని idx, గుర్తులేని from) {
        తిరిగి idx >= from ? idx - from
                           : n_buckets(h) + idx - from;
}

స్థిర గుర్తులేని పాత్ర_calculate_dib(Hashmap_పునాది *h, గుర్తులేని idx, dib_raw_t raw_dib) {
        గుర్తులేని initial_పాత్ర;

        ఒకవేళ (raw_dib == DIB_RAW_FREE)
                తిరిగి DIB_FREE;

        ఒకవేళ (_likely_(raw_dib < DIB_RAW_అధికప్రవాహం))
                తిరిగి raw_dib;

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
        initial_పాత్ర = పాత్ర_hash(h, పాత్ర_at(h, idx)->కీలకం);
        తిరిగి పాత్ర_దూరం(h, idx, initial_పాత్ర);
}

స్థిర శూన్య పాత్ర_సమితి_dib(Hashmap_పునాది *h, గుర్తులేని idx, గుర్తులేని dib) {
        dib_raw_ptr(h)[idx] = dib != DIB_FREE ? MIN(dib, DIB_RAW_అధికప్రవాహం) : DIB_RAW_FREE;
}

స్థిర గుర్తులేని skip_free_buckets(Hashmap_పునాది *h, గుర్తులేని idx) {
        dib_raw_t *dibs;

        dibs = dib_raw_ptr(h);

        కొరకు ( ; idx < n_buckets(h); idx++)
                ఒకవేళ (dibs[idx] != DIB_RAW_FREE)
                        తిరిగి idx;

        తిరిగి IDX_NIL;
}

స్థిర శూన్య పాత్ర_mark_free(Hashmap_పునాది *h, గుర్తులేని idx) {
        ప్రతిజ్ఞా(h);

        memzero(పాత్ర_at(h, idx), hashmap_type_info[h->type].నమోదు_కొలత);
        పాత్ర_సమితి_dib(h, idx, DIB_FREE);
}

స్థిర శూన్య పాత్ర_move_నమోదు(Hashmap_పునాది *h, నిర్మాణం మార్పిడి_entries *మార్పిడి,
                              గుర్తులేని from, గుర్తులేని to) {
        నిర్మాణం hashmap_పునాది_నమోదు *e_from, *e_గమ్యం;

        ప్రతిజ్ఞా(h);
        ప్రతిజ్ఞా(from != to);

        e_from = పాత్ర_at_virtual(h, మార్పిడి, from);
        e_గమ్యం   = పాత్ర_at_virtual(h, మార్పిడి, to);

        జ్ఞాపకం_నకలు(e_గమ్యం, e_from, hashmap_type_info[h->type].నమోదు_కొలత);

        ఒకవేళ (h->type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*) h;
                నిర్మాణం ordered_hashmap_నమోదు *le, *le_గమ్యం;

                le_గమ్యం = (నిర్మాణం ordered_hashmap_నమోదు*) e_గమ్యం;

                ఒకవేళ (le_గమ్యం->iterate_తదుపరి != IDX_NIL) {
                        le = (నిర్మాణం ordered_hashmap_నమోదు*)
                             పాత్ర_at_virtual(h, మార్పిడి, le_గమ్యం->iterate_తదుపరి);
                        le->iterate_మునుపటి = to;
                }

                ఒకవేళ (le_గమ్యం->iterate_మునుపటి != IDX_NIL) {
                        le = (నిర్మాణం ordered_hashmap_నమోదు*)
                             పాత్ర_at_virtual(h, మార్పిడి, le_గమ్యం->iterate_మునుపటి);
                        le->iterate_తదుపరి = to;
                }

                ఒకవేళ (lh->iterate_list_తల == from)
                        lh->iterate_list_తల = to;
                ఒకవేళ (lh->iterate_list_తోక == from)
                        lh->iterate_list_తోక = to;
        }
}

స్థిర అంతఃవరుస గుర్తులేని తదుపరి_idx(Hashmap_పునాది *h, గుర్తులేని idx) {
        తిరిగి idx + 1U < n_buckets(h) ? idx + 1U : 0U;
}

స్థిర గుర్తులేని prev_idx(Hashmap_పునాది *h, గుర్తులేని idx) {
        తిరిగి idx > 0U ? idx - 1U : n_buckets(h) - 1U;
}

స్థిర అంతఃవరుస శూన్య* నమోదు_విలువ(Hashmap_పునాది *h, నిర్మాణం hashmap_పునాది_నమోదు *e) {
        ప్రతిజ్ఞా(h);
        ప్రతిజ్ఞా(e);

        ఎంపిక (h->type) {

        స్థితి HASHMAP_TYPE_PLAIN:
        స్థితి HASHMAP_TYPE_ORDERED:
                తిరిగి ((నిర్మాణం plain_hashmap_నమోదు*)e)->విలువ;

        స్థితి HASHMAP_TYPE_సమితి:
                తిరిగి (శూన్య*) e->కీలకం;

        అప్రమేయ:
                assert_not_reached();
        }
}

స్థిర శూన్య పునాది_remove_నమోదు(Hashmap_పునాది *h, గుర్తులేని idx) {
        గుర్తులేని ఎడమ, కుడి, prev, dib;
        dib_raw_t raw_dib, *dibs;

        dibs = dib_raw_ptr(h);
        ప్రతిజ్ఞా(dibs[idx] != DIB_RAW_FREE);

#ఒకవేళ ENABLE_DEBUG_HASHMAP
        ప్రతిజ్ఞా(h);
        h->debug.rem_లెక్క++;
        h->debug.చివరి_rem_idx = idx;
#అయితే_ముగింపు

        ఎడమ = idx;
        /* Find the stop bucket ("right"). It is either free or has DIB == 0. */
        కొరకు (కుడి = తదుపరి_idx(h, ఎడమ); ; కుడి = తదుపరి_idx(h, కుడి)) {
                raw_dib = dibs[కుడి];
                ఒకవేళ (లోపల_సమితి(raw_dib, 0, DIB_RAW_FREE))
                        ఆపు;

                /* The buckets are not supposed to be all occupied and with DIB > 0.
                 * That would mean we could make everyone better off by shifting them
                 * backward. This scenario is impossible. */
                ప్రతిజ్ఞా(ఎడమ != కుడి);
        }

        ఒకవేళ (h->type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*) h;
                నిర్మాణం ordered_hashmap_నమోదు *le = ordered_పాత్ర_at(lh, idx);

                ఒకవేళ (le->iterate_తదుపరి != IDX_NIL)
                        ordered_పాత్ర_at(lh, le->iterate_తదుపరి)->iterate_మునుపటి = le->iterate_మునుపటి;
                లేకపోతే
                        lh->iterate_list_తోక = le->iterate_మునుపటి;

                ఒకవేళ (le->iterate_మునుపటి != IDX_NIL)
                        ordered_పాత్ర_at(lh, le->iterate_మునుపటి)->iterate_తదుపరి = le->iterate_తదుపరి;
                లేకపోతే
                        lh->iterate_list_తల = le->iterate_తదుపరి;
        }

        /* Now shift all buckets in the interval (left, right) one step backwards */
        కొరకు (prev = ఎడమ, ఎడమ = తదుపరి_idx(h, ఎడమ); ఎడమ != కుడి;
             prev = ఎడమ, ఎడమ = తదుపరి_idx(h, ఎడమ)) {
                dib = పాత్ర_calculate_dib(h, ఎడమ, dibs[ఎడమ]);
                ప్రతిజ్ఞా(dib != 0);
                పాత్ర_move_నమోదు(h, ఖాళీ, ఎడమ, prev);
                పాత్ర_సమితి_dib(h, prev, dib - 1);
        }

        పాత్ర_mark_free(h, prev);
        n_entries_dec(h);
        పునాది_సమితి_dirty(h);
}
#నిర్వచించు remove_నమోదు(h, idx) పునాది_remove_నమోదు(HASHMAP_పునాది(h), idx)

స్థిర గుర్తులేని hashmap_iterate_లోపల_insertion_క్రమం(OrderedHashmap *h, Iterator *i) {
        నిర్మాణం ordered_hashmap_నమోదు *e;
        గుర్తులేని idx;

        ప్రతిజ్ఞా(h);
        ప్రతిజ్ఞా(i);

        ఒకవేళ (i->idx == IDX_NIL)
                వెళ్ళు at_చివర;

        ఒకవేళ (i->idx == IDX_మొదటి && h->iterate_list_తల == IDX_NIL)
                వెళ్ళు at_చివర;

        ఒకవేళ (i->idx == IDX_మొదటి) {
                idx = h->iterate_list_తల;
                e = ordered_పాత్ర_at(h, idx);
        } లేకపోతే {
                idx = i->idx;
                e = ordered_పాత్ర_at(h, idx);
                /*
                 * We allow removing the current entry while iterating, but removal may cause
                 * a backward shift. The next entry may thus move one bucket to the left.
                 * To detect when it happens, we remember the key pointer of the entry we were
                 * going to iterate next. If it does not match, there was a backward shift.
                 */
                ఒకవేళ (e->p.b.కీలకం != i->తదుపరి_కీలకం) {
                        idx = prev_idx(HASHMAP_పునాది(h), idx);
                        e = ordered_పాత్ర_at(h, idx);
                }
                ప్రతిజ్ఞా(e->p.b.కీలకం == i->తదుపరి_కీలకం);
        }

#ఒకవేళ ENABLE_DEBUG_HASHMAP
        i->prev_idx = idx;
#అయితే_ముగింపు

        ఒకవేళ (e->iterate_తదుపరి != IDX_NIL) {
                నిర్మాణం ordered_hashmap_నమోదు *n;
                i->idx = e->iterate_తదుపరి;
                n = ordered_పాత్ర_at(h, i->idx);
                i->తదుపరి_కీలకం = n->p.b.కీలకం;
        } లేకపోతే
                i->idx = IDX_NIL;

        తిరిగి idx;

at_చివర:
        i->idx = IDX_NIL;
        తిరిగి IDX_NIL;
}

స్థిర గుర్తులేని hashmap_iterate_లోపల_అంతర్గత_క్రమం(Hashmap_పునాది *h, Iterator *i) {
        గుర్తులేని idx;

        ప్రతిజ్ఞా(h);
        ప్రతిజ్ఞా(i);

        ఒకవేళ (i->idx == IDX_NIL)
                వెళ్ళు at_చివర;

        ఒకవేళ (i->idx == IDX_మొదటి) {
                /* fast forward to the first occupied bucket */
                ఒకవేళ (h->has_పరోక్ష) {
                        i->idx = skip_free_buckets(h, h->పరోక్ష.idx_అత్యల్ప_నమోదు);
                        h->పరోక్ష.idx_అత్యల్ప_నమోదు = i->idx;
                } లేకపోతే
                        i->idx = skip_free_buckets(h, 0);

                ఒకవేళ (i->idx == IDX_NIL)
                        వెళ్ళు at_చివర;
        } లేకపోతే {
                నిర్మాణం hashmap_పునాది_నమోదు *e;

                ప్రతిజ్ఞా(i->idx > 0);

                e = పాత్ర_at(h, i->idx);
                /*
                 * We allow removing the current entry while iterating, but removal may cause
                 * a backward shift. The next entry may thus move one bucket to the left.
                 * To detect when it happens, we remember the key pointer of the entry we were
                 * going to iterate next. If it does not match, there was a backward shift.
                 */
                ఒకవేళ (e->కీలకం != i->తదుపరి_కీలకం)
                        e = పాత్ర_at(h, --i->idx);

                ప్రతిజ్ఞా(e->కీలకం == i->తదుపరి_కీలకం);
        }

        idx = i->idx;
#ఒకవేళ ENABLE_DEBUG_HASHMAP
        i->prev_idx = idx;
#అయితే_ముగింపు

        i->idx = skip_free_buckets(h, i->idx + 1);
        ఒకవేళ (i->idx != IDX_NIL)
                i->తదుపరి_కీలకం = పాత్ర_at(h, i->idx)->కీలకం;
        లేకపోతే
                i->idx = IDX_NIL;

        తిరిగి idx;

at_చివర:
        i->idx = IDX_NIL;
        తిరిగి IDX_NIL;
}

స్థిర గుర్తులేని hashmap_iterate_నమోదు(Hashmap_పునాది *h, Iterator *i) {
        ఒకవేళ (!h) {
                i->idx = IDX_NIL;
                తిరిగి IDX_NIL;
        }

#ఒకవేళ ENABLE_DEBUG_HASHMAP
        ఒకవేళ (i->idx == IDX_మొదటి) {
                i->put_లెక్క = h->debug.put_లెక్క;
                i->rem_లెక్క = h->debug.rem_లెక్క;
        } లేకపోతే {
                /* While iterating, must not add any new entries */
                ప్రతిజ్ఞా(i->put_లెక్క == h->debug.put_లెక్క);
                /* ... or remove entries other than the current one */
                ప్రతిజ్ఞా(i->rem_లెక్క == h->debug.rem_లెక్క ||
                       (i->rem_లెక్క == h->debug.rem_లెక్క - 1 &&
                        i->prev_idx == h->debug.చివరి_rem_idx));
                /* Reset our removals counter */
                i->rem_లెక్క = h->debug.rem_లెక్క;
        }
#అయితే_ముగింపు

        తిరిగి h->type == HASHMAP_TYPE_ORDERED ? hashmap_iterate_లోపల_insertion_క్రమం((OrderedHashmap*) h, i)
                                               : hashmap_iterate_లోపల_అంతర్గత_క్రమం(h, i);
}

bool _hashmap_iterate(Hashmap_పునాది *h, Iterator *i, శూన్య **విలువ, స్థిరరాశి శూన్య **కీలకం) {
        నిర్మాణం hashmap_పునాది_నమోదు *e;
        శూన్య *దత్తాంశం;
        గుర్తులేని idx;

        idx = hashmap_iterate_నమోదు(h, i);
        ఒకవేళ (idx == IDX_NIL) {
                ఒకవేళ (విలువ)
                        *విలువ = ఖాళీ;
                ఒకవేళ (కీలకం)
                        *కీలకం = ఖాళీ;

                తిరిగి false;
        }

        e = పాత్ర_at(h, idx);
        దత్తాంశం = నమోదు_విలువ(h, e);
        ఒకవేళ (విలువ)
                *విలువ = దత్తాంశం;
        ఒకవేళ (కీలకం)
                *కీలకం = e->కీలకం;

        తిరిగి true;
}

#నిర్వచించు HASHMAP_FOREACH_IDX(idx, h, i) \
        కొరకు ((i) = ITERATOR_మొదటి, (idx) = hashmap_iterate_నమోదు((h), &(i)); \
             (idx != IDX_NIL); \
             (idx) = hashmap_iterate_నమోదు((h), &(i)))

IteratedCache* _hashmap_iterated_cache_new(Hashmap_పునాది *h) {
        IteratedCache *cache;

        ప్రతిజ్ఞా(h);
        ప్రతిజ్ఞా(!h->cached);

        ఒకవేళ (h->cached)
                తిరిగి ఖాళీ;

        cache = new0(IteratedCache, 1);
        ఒకవేళ (!cache)
                తిరిగి ఖాళీ;

        cache->hashmap = h;
        h->cached = true;

        తిరిగి cache;
}

స్థిర శూన్య reset_ప్రత్యక్ష_storage(Hashmap_పునాది *h) {
        స్థిరరాశి నిర్మాణం hashmap_type_info *hi = &hashmap_type_info[h->type];
        శూన్య *p;

        ప్రతిజ్ఞా(!h->has_పరోక్ష);

        p = mempset(h->ప్రత్యక్ష.storage, 0, hi->నమోదు_కొలత * hi->n_ప్రత్యక్ష_buckets);
        memset(p, DIB_RAW_INIT, పరిమాణం(dib_raw_t) * hi->n_ప్రత్యక్ష_buckets);
}

స్థిర శూన్య shared_hash_కీలకం_initialize(శూన్య) {
        random_బైట్లు(shared_hash_కీలకం, పరిమాణం(shared_hash_కీలకం));
}

స్థిర నిర్మాణం Hashmap_పునాది* hashmap_పునాది_new(స్థిరరాశి నిర్మాణం hash_ops *hash_ops, గణన HashmapType type) {
        Hashmap_పునాది *h;
        స్థిరరాశి నిర్మాణం hashmap_type_info *hi = &hashmap_type_info[type];

        bool use_pool = mempool_enabled && mempool_enabled();  /* mempool_enabled is a weak symbol */

        h = use_pool ? mempool_alloc0_tile(hi->mempool) : malloc0(hi->తల_కొలత);
        ఒకవేళ (!h)
                తిరిగి ఖాళీ;

        h->type = type;
        h->from_pool = use_pool;
        h->hash_ops = hash_ops ?: &trivial_hash_ops;

        ఒకవేళ (type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*)h;
                lh->iterate_list_తల = lh->iterate_list_తోక = IDX_NIL;
        }

        reset_ప్రత్యక్ష_storage(h);

        స్థిర pthread_once_t once = PTHREAD_ONCE_INIT;
        assert_se(pthread_once(&once, shared_hash_కీలకం_initialize) == 0);

#ఒకవేళ ENABLE_DEBUG_HASHMAP
        assert_se(pthread_mutex_lock(&hashmap_debug_list_mutex) == 0);
        LIST_PREPEND(debug_list, hashmap_debug_list, &h->debug);
        assert_se(pthread_mutex_unlock(&hashmap_debug_list_mutex) == 0);
#అయితే_ముగింపు

        తిరిగి h;
}

Hashmap *hashmap_new(స్థిరరాశి నిర్మాణం hash_ops *hash_ops) {
        తిరిగి (Hashmap*)        hashmap_పునాది_new(hash_ops, HASHMAP_TYPE_PLAIN);
}

OrderedHashmap *ordered_hashmap_new(స్థిరరాశి నిర్మాణం hash_ops *hash_ops) {
        తిరిగి (OrderedHashmap*) hashmap_పునాది_new(hash_ops, HASHMAP_TYPE_ORDERED);
}

సమితి_2 *సమితి_new(స్థిరరాశి నిర్మాణం hash_ops *hash_ops) {
        తిరిగి (సమితి_2*)            hashmap_పునాది_new(hash_ops, HASHMAP_TYPE_సమితి);
}

స్థిర పూర్ణాంకం hashmap_పునాది_ensure_allocated(Hashmap_పునాది **h, స్థిరరాశి నిర్మాణం hash_ops *hash_ops,
                                         గణన HashmapType type) {
        Hashmap_పునాది *q;

        ప్రతిజ్ఞా(h);

        ఒకవేళ (*h) {
                ప్రతిజ్ఞా((*h)->hash_ops == (hash_ops ?: &trivial_hash_ops));
                తిరిగి 0;
        }

        q = hashmap_పునాది_new(hash_ops, type);
        ఒకవేళ (!q)
                తిరిగి -ENOMEM;

        *h = q;
        తిరిగి 1;
}

పూర్ణాంకం hashmap_ensure_allocated(Hashmap **h, స్థిరరాశి నిర్మాణం hash_ops *hash_ops) {
        తిరిగి hashmap_పునాది_ensure_allocated((Hashmap_పునాది**)h, hash_ops, HASHMAP_TYPE_PLAIN);
}

పూర్ణాంకం ordered_hashmap_ensure_allocated(OrderedHashmap **h, స్థిరరాశి నిర్మాణం hash_ops *hash_ops) {
        తిరిగి hashmap_పునాది_ensure_allocated((Hashmap_పునాది**)h, hash_ops, HASHMAP_TYPE_ORDERED);
}

పూర్ణాంకం సమితి_ensure_allocated(సమితి_2 **s, స్థిరరాశి నిర్మాణం hash_ops *hash_ops) {
        తిరిగి hashmap_పునాది_ensure_allocated((Hashmap_పునాది**)s, hash_ops, HASHMAP_TYPE_సమితి);
}

పూర్ణాంకం hashmap_ensure_put(Hashmap **h, స్థిరరాశి నిర్మాణం hash_ops *hash_ops, స్థిరరాశి శూన్య *కీలకం, శూన్య *విలువ) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(h);

        r = hashmap_ensure_allocated(h, hash_ops);
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి hashmap_put(*h, కీలకం, విలువ);
}

పూర్ణాంకం ordered_hashmap_ensure_put(OrderedHashmap **h, స్థిరరాశి నిర్మాణం hash_ops *hash_ops, స్థిరరాశి శూన్య *కీలకం, శూన్య *విలువ) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(h);

        r = ordered_hashmap_ensure_allocated(h, hash_ops);
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి ordered_hashmap_put(*h, కీలకం, విలువ);
}

పూర్ణాంకం ordered_hashmap_ensure_replace(OrderedHashmap **h, స్థిరరాశి నిర్మాణం hash_ops *hash_ops, స్థిరరాశి శూన్య *కీలకం, శూన్య *విలువ) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(h);

        r = ordered_hashmap_ensure_allocated(h, hash_ops);
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి ordered_hashmap_replace(*h, కీలకం, విలువ);
}

పూర్ణాంకం hashmap_ensure_replace(Hashmap **h, స్థిరరాశి నిర్మాణం hash_ops *hash_ops, స్థిరరాశి శూన్య *కీలకం, శూన్య *విలువ) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(h);

        r = hashmap_ensure_allocated(h, hash_ops);
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి hashmap_replace(*h, కీలకం, విలువ);
}

స్థిర శూన్య hashmap_free_no_clear(Hashmap_పునాది *h) {
        ప్రతిజ్ఞా(!h->has_పరోక్ష);
        ప్రతిజ్ఞా(h->n_ప్రత్యక్ష_entries == 0);

#ఒకవేళ ENABLE_DEBUG_HASHMAP
        assert_se(pthread_mutex_lock(&hashmap_debug_list_mutex) == 0);
        LIST_REMOVE(debug_list, hashmap_debug_list, &h->debug);
        assert_se(pthread_mutex_unlock(&hashmap_debug_list_mutex) == 0);
#అయితే_ముగింపు

        ఒకవేళ (h->from_pool) {
                /* Ensure that the object didn't get migrated between threads. */
                assert_se(is_main_thread());
                mempool_free_tile(hashmap_type_info[h->type].mempool, h);
        } లేకపోతే
                విడుదల(h);
}

Hashmap_పునాది* _hashmap_free(Hashmap_పునాది *h) {
        ఒకవేళ (h) {
                _hashmap_clear(h);
                hashmap_free_no_clear(h);
        }

        తిరిగి ఖాళీ;
}

శూన్య _hashmap_clear(Hashmap_పునాది *h) {
        ఒకవేళ (!h)
                తిరిగి;

        ఒకవేళ (h->hash_ops->free_కీలకం || h->hash_ops->free_విలువ) {

                /* If destructor calls are defined, let's destroy things defensively: let's take the item out of the
                 * hash table, and only then call the destructor functions. If these destructors then try to unregister
                 * themselves from our hash table a second time, the entry is already gone. */

                అయితే (hashmap_కొలత(h) > 0) {
                        శూన్య *k = ఖాళీ;
                        శూన్య *v;

                        v = hashmap_మొదటి_కీలకం_and_విలువ(h, true, &k);

                        ఒకవేళ (h->hash_ops->free_కీలకం)
                                h->hash_ops->free_కీలకం(k);

                        ఒకవేళ (h->hash_ops->free_విలువ)
                                h->hash_ops->free_విలువ(v);
                }
        }

        ఒకవేళ (h->has_పరోక్ష) {
                విడుదల(h->పరోక్ష.storage);
                h->has_పరోక్ష = false;
        }

        h->n_ప్రత్యక్ష_entries = 0;
        reset_ప్రత్యక్ష_storage(h);

        ఒకవేళ (h->type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*) h;
                lh->iterate_list_తల = lh->iterate_list_తోక = IDX_NIL;
        }

        పునాది_సమితి_dirty(h);
}

స్థిర పూర్ణాంకం resize_buckets(Hashmap_పునాది *h, గుర్తులేని entries_కూడు);

/*
 * Finds an empty bucket to put an entry into, starting the scan at 'idx'.
 * Performs Robin Hood swaps as it goes. The entry to put must be placed
 * by the caller into swap slot IDX_PUT.
 * If used for in-place resizing, may leave a displaced entry in swap slot
 * IDX_PUT. Caller must rehash it next.
 * Returns: true if it left a displaced entry to rehash next in IDX_PUT,
 *          false otherwise.
 */
స్థిర bool hashmap_put_robin_hood(Hashmap_పునాది *h, గుర్తులేని idx,
                                   నిర్మాణం మార్పిడి_entries *మార్పిడి) {
        dib_raw_t raw_dib, *dibs;
        గుర్తులేని dib, దూరం;

#ఒకవేళ ENABLE_DEBUG_HASHMAP
        ప్రతిజ్ఞా(h);
        h->debug.put_లెక్క++;
#అయితే_ముగింపు

        dibs = dib_raw_ptr(h);

        కొరకు (దూరం = 0; ; దూరం++) {
                raw_dib = dibs[idx];
                ఒకవేళ (లోపల_సమితి(raw_dib, DIB_RAW_FREE, DIB_RAW_REHASH)) {
                        ఒకవేళ (raw_dib == DIB_RAW_REHASH)
                                పాత్ర_move_నమోదు(h, మార్పిడి, idx, IDX_TMP);

                        ఒకవేళ (h->has_పరోక్ష && h->పరోక్ష.idx_అత్యల్ప_నమోదు > idx)
                                h->పరోక్ష.idx_అత్యల్ప_నమోదు = idx;

                        పాత్ర_సమితి_dib(h, idx, దూరం);
                        పాత్ర_move_నమోదు(h, మార్పిడి, IDX_PUT, idx);
                        ఒకవేళ (raw_dib == DIB_RAW_REHASH) {
                                పాత్ర_move_నమోదు(h, మార్పిడి, IDX_TMP, IDX_PUT);
                                తిరిగి true;
                        }

                        తిరిగి false;
                }

                dib = పాత్ర_calculate_dib(h, idx, raw_dib);

                ఒకవేళ (dib < దూరం) {
                        /* Found a wealthier entry. Go Robin Hood! */
                        పాత్ర_సమితి_dib(h, idx, దూరం);

                        /* swap the entries */
                        పాత్ర_move_నమోదు(h, మార్పిడి, idx, IDX_TMP);
                        పాత్ర_move_నమోదు(h, మార్పిడి, IDX_PUT, idx);
                        పాత్ర_move_నమోదు(h, మార్పిడి, IDX_TMP, IDX_PUT);

                        దూరం = dib;
                }

                idx = తదుపరి_idx(h, idx);
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
స్థిర పూర్ణాంకం hashmap_పునాది_put_boldly(Hashmap_పునాది *h, గుర్తులేని idx,
                                   నిర్మాణం మార్పిడి_entries *మార్పిడి, bool may_resize) {
        నిర్మాణం ordered_hashmap_నమోదు *new_నమోదు;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(idx < n_buckets(h));

        new_నమోదు = పాత్ర_at_మార్పిడి(మార్పిడి, IDX_PUT);

        ఒకవేళ (may_resize) {
                r = resize_buckets(h, 1);
                ఒకవేళ (r < 0)
                        తిరిగి r;
                ఒకవేళ (r > 0)
                        idx = పాత్ర_hash(h, new_నమోదు->p.b.కీలకం);
        }
        ప్రతిజ్ఞా(n_entries(h) < n_buckets(h));

        ఒకవేళ (h->type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*) h;

                new_నమోదు->iterate_తదుపరి = IDX_NIL;
                new_నమోదు->iterate_మునుపటి = lh->iterate_list_తోక;

                ఒకవేళ (lh->iterate_list_తోక != IDX_NIL) {
                        నిర్మాణం ordered_hashmap_నమోదు *old_తోక;

                        old_తోక = ordered_పాత్ర_at(lh, lh->iterate_list_తోక);
                        ప్రతిజ్ఞా(old_తోక->iterate_తదుపరి == IDX_NIL);
                        old_తోక->iterate_తదుపరి = IDX_PUT;
                }

                lh->iterate_list_తోక = IDX_PUT;
                ఒకవేళ (lh->iterate_list_తల == IDX_NIL)
                        lh->iterate_list_తల = IDX_PUT;
        }

        assert_se(hashmap_put_robin_hood(h, idx, మార్పిడి) == false);

        n_entries_inc(h);
#ఒకవేళ ENABLE_DEBUG_HASHMAP
        h->debug.max_entries = MAX(h->debug.max_entries, n_entries(h));
#అయితే_ముగింపు

        పునాది_సమితి_dirty(h);

        తిరిగి 1;
}
#నిర్వచించు hashmap_put_boldly(h, idx, మార్పిడి, may_resize) \
        hashmap_పునాది_put_boldly(HASHMAP_పునాది(h), idx, మార్పిడి, may_resize)

/*
 * Returns 0 if resize is not needed.
 *         1 if successfully resized.
 *         -ENOMEM on allocation failure.
 */
స్థిర పూర్ణాంకం resize_buckets(Hashmap_పునాది *h, గుర్తులేని entries_కూడు) {
        నిర్మాణం మార్పిడి_entries మార్పిడి;
        శూన్య *new_storage;
        dib_raw_t *old_dibs, *new_dibs;
        స్థిరరాశి నిర్మాణం hashmap_type_info *hi;
        గుర్తులేని idx, optimal_idx;
        గుర్తులేని old_n_buckets, new_n_buckets, n_rehashed, new_n_entries;
        uint8_t new_జరుపు;
        bool rehash_తదుపరి;

        ప్రతిజ్ఞా(h);

        hi = &hashmap_type_info[h->type];
        new_n_entries = n_entries(h) + entries_కూడు;

        /* overflow? */
        ఒకవేళ (_unlikely_(new_n_entries < entries_కూడు))
                తిరిగి -ENOMEM;

        /* For direct storage we allow 100% load, because it's tiny. */
        ఒకవేళ (!h->has_పరోక్ష && new_n_entries <= hi->n_ప్రత్యక్ష_buckets)
                తిరిగి 0;

        /*
         * Load factor = n/m = 1 - (1/INV_KEEP_FREE).
         * From it follows: m = n + n/(INV_KEEP_FREE - 1)
         */
        new_n_buckets = new_n_entries + new_n_entries / (INV_KEEP_FREE - 1);
        /* overflow? */
        ఒకవేళ (_unlikely_(new_n_buckets < new_n_entries))
                తిరిగి -ENOMEM;

        old_n_buckets = n_buckets(h);

        ఒకవేళ (_likely_(new_n_buckets <= old_n_buckets))
                తిరిగి 0;

        ఒకవేళ (_unlikely_(new_n_buckets > UINT_MAX / (hi->నమోదు_కొలత + పరిమాణం(dib_raw_t))))
                తిరిగి -ENOMEM;

        new_జరుపు = log2u_విడత_పైకి(MAX(
                        new_n_buckets * (hi->నమోదు_కొలత + పరిమాణం(dib_raw_t)),
                        2 * పరిమాణం(నిర్మాణం ప్రత్యక్ష_storage)));

        /* Realloc storage (buckets and DIB array). */
        new_storage = realloc(h->has_పరోక్ష ? h->పరోక్ష.storage : ఖాళీ,
                              1U << new_జరుపు);
        ఒకవేళ (!new_storage)
                తిరిగి -ENOMEM;

        /* Must upgrade direct to indirect storage. */
        ఒకవేళ (!h->has_పరోక్ష) {
                జ్ఞాపకం_నకలు(new_storage, h->ప్రత్యక్ష.storage,
                       old_n_buckets * (hi->నమోదు_కొలత + పరిమాణం(dib_raw_t)));
                h->పరోక్ష.n_entries = h->n_ప్రత్యక్ష_entries;
                h->పరోక్ష.idx_అత్యల్ప_నమోదు = 0;
                h->n_ప్రత్యక్ష_entries = 0;
        }

        /* Get a new hash key. If we've just upgraded to indirect storage,
         * allow reusing a previously generated key. It's still a different key
         * from the shared one that we used for direct storage. */
        get_hash_కీలకం(h->పరోక్ష.hash_కీలకం, !h->has_పరోక్ష);

        h->has_పరోక్ష = true;
        h->పరోక్ష.storage = new_storage;
        h->పరోక్ష.n_buckets = (1U << new_జరుపు) /
                                (hi->నమోదు_కొలత + పరిమాణం(dib_raw_t));

        old_dibs = (dib_raw_t*)((uint8_t*) new_storage + hi->నమోదు_కొలత * old_n_buckets);
        new_dibs = dib_raw_ptr(h);

        /*
         * Move the DIB array to the new place, replacing valid DIB values with
         * DIB_RAW_REHASH to indicate all of the used buckets need rehashing.
         * Note: Overlap is not possible, because we have at least doubled the
         * number of buckets and dib_raw_t is smaller than any entry type.
         */
        కొరకు (idx = 0; idx < old_n_buckets; idx++) {
                ప్రతిజ్ఞా(old_dibs[idx] != DIB_RAW_REHASH);
                new_dibs[idx] = old_dibs[idx] == DIB_RAW_FREE ? DIB_RAW_FREE
                                                              : DIB_RAW_REHASH;
        }

        /* Zero the area of newly added entries (including the old DIB area) */
        memzero(పాత్ర_at(h, old_n_buckets),
               (n_buckets(h) - old_n_buckets) * hi->నమోదు_కొలత);

        /* The upper half of the new DIB array needs initialization */
        memset(&new_dibs[old_n_buckets], DIB_RAW_INIT,
               (n_buckets(h) - old_n_buckets) * పరిమాణం(dib_raw_t));

        /* Rehash entries that need it */
        n_rehashed = 0;
        కొరకు (idx = 0; idx < old_n_buckets; idx++) {
                ఒకవేళ (new_dibs[idx] != DIB_RAW_REHASH)
                        కొనసాగు;

                optimal_idx = పాత్ర_hash(h, పాత్ర_at(h, idx)->కీలకం);

                /*
                 * Not much to do if by luck the entry hashes to its current
                 * location. Just set its DIB.
                 */
                ఒకవేళ (optimal_idx == idx) {
                        new_dibs[idx] = 0;
                        n_rehashed++;
                        కొనసాగు;
                }

                new_dibs[idx] = DIB_RAW_FREE;
                పాత్ర_move_నమోదు(h, &మార్పిడి, idx, IDX_PUT);
                /* bucket_move_entry does not clear the source */
                memzero(పాత్ర_at(h, idx), hi->నమోదు_కొలత);

                చేయి {
                        /*
                         * Find the new bucket for the current entry. This may make
                         * another entry homeless and load it into IDX_PUT.
                         */
                        rehash_తదుపరి = hashmap_put_robin_hood(h, optimal_idx, &మార్పిడి);
                        n_rehashed++;

                        /* Did the current entry displace another one? */
                        ఒకవేళ (rehash_తదుపరి)
                                optimal_idx = పాత్ర_hash(h, పాత్ర_at_మార్పిడి(&మార్పిడి, IDX_PUT)->p.b.కీలకం);
                } అయితే (rehash_తదుపరి);
        }

        assert_se(n_rehashed == n_entries(h));

        తిరిగి 1;
}

/*
 * Finds an entry with a matching key
 * Returns: index of the found entry, or IDX_NIL if not found.
 */
స్థిర గుర్తులేని పునాది_పాత్ర_పరిశీలన(Hashmap_పునాది *h, గుర్తులేని idx, స్థిరరాశి శూన్య *కీలకం) {
        compare_func_t compare = h->hash_ops->compare;
        నిర్మాణం hashmap_పునాది_నమోదు *e;
        గుర్తులేని dib, దూరం;
        dib_raw_t *dibs = dib_raw_ptr(h);

        ప్రతిజ్ఞా(idx < n_buckets(h));

        కొరకు (దూరం = 0; ; దూరం++) {
                ఒకవేళ (dibs[idx] == DIB_RAW_FREE)
                        తిరిగి IDX_NIL;

                dib = పాత్ర_calculate_dib(h, idx, dibs[idx]);

                ఒకవేళ (dib < దూరం)
                        తిరిగి IDX_NIL;
                ఒకవేళ (dib == దూరం) {
                        e = పాత్ర_at(h, idx);

                        /* Most maps compare keys by pointer. Do that here, without the indirect call. */
                        ఒకవేళ (compare == trivial_compare_func ? e->కీలకం == కీలకం : compare(e->కీలకం, కీలకం) == 0)
                                తిరిగి idx;
                }

                idx = తదుపరి_idx(h, idx);
        }
}
#నిర్వచించు పాత్ర_పరిశీలన(h, idx, కీలకం) పునాది_పాత్ర_పరిశీలన(HASHMAP_పునాది(h), idx, కీలకం)

పూర్ణాంకం hashmap_put(Hashmap *h, స్థిరరాశి శూన్య *కీలకం, శూన్య *విలువ) {
        నిర్మాణం మార్పిడి_entries మార్పిడి;
        నిర్మాణం plain_hashmap_నమోదు *e;
        గుర్తులేని hash, idx;

        ప్రతిజ్ఞా(h);

        hash = పాత్ర_hash(h, కీలకం);
        idx = పాత్ర_పరిశీలన(h, hash, కీలకం);
        ఒకవేళ (idx != IDX_NIL) {
                e = plain_పాత్ర_at(h, idx);
                ఒకవేళ (e->విలువ == విలువ)
                        తిరిగి 0;
                తిరిగి -EEXIST;
        }

        e = &పాత్ర_at_మార్పిడి(&మార్పిడి, IDX_PUT)->p;
        e->b.కీలకం = కీలకం;
        e->విలువ = విలువ;
        తిరిగి hashmap_put_boldly(h, hash, &మార్పిడి, true);
}

పూర్ణాంకం సమితి_put(సమితి_2 *s, స్థిరరాశి శూన్య *కీలకం) {
        నిర్మాణం మార్పిడి_entries మార్పిడి;
        నిర్మాణం hashmap_పునాది_నమోదు *e;
        గుర్తులేని hash, idx;

        ప్రతిజ్ఞా(s);

        hash = పాత్ర_hash(s, కీలకం);
        idx = పాత్ర_పరిశీలన(s, hash, కీలకం);
        ఒకవేళ (idx != IDX_NIL)
                తిరిగి 0;

        e = &పాత్ర_at_మార్పిడి(&మార్పిడి, IDX_PUT)->p.b;
        e->కీలకం = కీలకం;
        తిరిగి hashmap_put_boldly(s, hash, &మార్పిడి, true);
}

పూర్ణాంకం సమితి_ensure_put(సమితి_2 **s, స్థిరరాశి నిర్మాణం hash_ops *hash_ops, స్థిరరాశి శూన్య *కీలకం) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(s);

        r = సమితి_ensure_allocated(s, hash_ops);
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి సమితి_put(*s, కీలకం);
}

పూర్ణాంకం సమితి_ensure_consume(సమితి_2 **s, స్థిరరాశి నిర్మాణం hash_ops *hash_ops, శూన్య *కీలకం) {
        పూర్ణాంకం r;

        r = సమితి_ensure_put(s, hash_ops, కీలకం);
        ఒకవేళ (r <= 0) {
                ఒకవేళ (hash_ops && hash_ops->free_కీలకం)
                        hash_ops->free_కీలకం(కీలకం);
                లేకపోతే ఒకవేళ (hash_ops && hash_ops->free_విలువ)
                        /* Sets store their element in the key slot but may carry a value destructor. */
                        hash_ops->free_విలువ(కీలకం);
                లేకపోతే
                        విడుదల(కీలకం);
        }

        తిరిగి r;
}

పూర్ణాంకం hashmap_replace(Hashmap *h, స్థిరరాశి శూన్య *కీలకం, శూన్య *విలువ) {
        నిర్మాణం మార్పిడి_entries మార్పిడి;
        నిర్మాణం plain_hashmap_నమోదు *e;
        గుర్తులేని hash, idx;

        ప్రతిజ్ఞా(h);

        hash = పాత్ర_hash(h, కీలకం);
        idx = పాత్ర_పరిశీలన(h, hash, కీలకం);
        ఒకవేళ (idx != IDX_NIL) {
                e = plain_పాత్ర_at(h, idx);
#ఒకవేళ ENABLE_DEBUG_HASHMAP
                /* Although the key is equal, the key pointer may have changed,
                 * and this would break our assumption for iterating. So count
                 * this operation as incompatible with iteration. */
                ఒకవేళ (e->b.కీలకం != కీలకం) {
                        h->b.debug.put_లెక్క++;
                        h->b.debug.rem_లెక్క++;
                        h->b.debug.చివరి_rem_idx = idx;
                }
#అయితే_ముగింపు
                e->b.కీలకం = కీలకం;
                e->విలువ = విలువ;
                hashmap_సమితి_dirty(h);

                తిరిగి 0;
        }

        e = &పాత్ర_at_మార్పిడి(&మార్పిడి, IDX_PUT)->p;
        e->b.కీలకం = కీలకం;
        e->విలువ = విలువ;
        తిరిగి hashmap_put_boldly(h, hash, &మార్పిడి, true);
}

పూర్ణాంకం hashmap_update(Hashmap *h, స్థిరరాశి శూన్య *కీలకం, శూన్య *విలువ) {
        నిర్మాణం plain_hashmap_నమోదు *e;
        గుర్తులేని hash, idx;

        ప్రతిజ్ఞా(h);

        hash = పాత్ర_hash(h, కీలకం);
        idx = పాత్ర_పరిశీలన(h, hash, కీలకం);
        ఒకవేళ (idx == IDX_NIL)
                తిరిగి -ENOENT;

        e = plain_పాత్ర_at(h, idx);
        e->విలువ = విలువ;
        hashmap_సమితి_dirty(h);

        తిరిగి 0;
}

శూన్య* _hashmap_get(Hashmap_పునాది *h, స్థిరరాశి శూన్య *కీలకం) {
        నిర్మాణం hashmap_పునాది_నమోదు *e;
        గుర్తులేని hash, idx;

        ఒకవేళ (!h)
                తిరిగి ఖాళీ;

        hash = పాత్ర_hash(h, కీలకం);
        idx = పాత్ర_పరిశీలన(h, hash, కీలకం);
        ఒకవేళ (idx == IDX_NIL)
                తిరిగి ఖాళీ;

        e = పాత్ర_at(h, idx);
        తిరిగి నమోదు_విలువ(h, e);
}

శూన్య* hashmap_get2(Hashmap *h, స్థిరరాశి శూన్య *కీలకం, శూన్య **ret) {
        నిర్మాణం plain_hashmap_నమోదు *e;
        గుర్తులేని hash, idx;

        ఒకవేళ (!h)
                తిరిగి ఖాళీ;

        hash = పాత్ర_hash(h, కీలకం);
        idx = పాత్ర_పరిశీలన(h, hash, కీలకం);
        ఒకవేళ (idx == IDX_NIL)
                తిరిగి ఖాళీ;

        e = plain_పాత్ర_at(h, idx);
        ఒకవేళ (ret)
                *ret = (శూన్య*) e->b.కీలకం;

        తిరిగి e->విలువ;
}

bool hashmap_కలిగియుంది_2(Hashmap_పునాది *h, స్థిరరాశి శూన్య *కీలకం) {
        గుర్తులేని hash;

        ఒకవేళ (!h)
                తిరిగి false;

        hash = పాత్ర_hash(h, కీలకం);
        తిరిగి పాత్ర_పరిశీలన(h, hash, కీలకం) != IDX_NIL;
}

శూన్య* _hashmap_remove(Hashmap_పునాది *h, స్థిరరాశి శూన్య *కీలకం) {
        నిర్మాణం hashmap_పునాది_నమోదు *e;
        గుర్తులేని hash, idx;
        శూన్య *దత్తాంశం;

        ఒకవేళ (!h)
                తిరిగి ఖాళీ;

        hash = పాత్ర_hash(h, కీలకం);
        idx = పాత్ర_పరిశీలన(h, hash, కీలకం);
        ఒకవేళ (idx == IDX_NIL)
                తిరిగి ఖాళీ;

        e = పాత్ర_at(h, idx);
        దత్తాంశం = నమోదు_విలువ(h, e);
        remove_నమోదు(h, idx);

        తిరిగి దత్తాంశం;
}

శూన్య* hashmap_remove2(Hashmap *h, స్థిరరాశి శూన్య *కీలకం, శూన్య **ret) {
        నిర్మాణం plain_hashmap_నమోదు *e;
        గుర్తులేని hash, idx;
        శూన్య *దత్తాంశం;

        ఒకవేళ (!h) {
                ఒకవేళ (ret)
                        *ret = ఖాళీ;
                తిరిగి ఖాళీ;
        }

        hash = పాత్ర_hash(h, కీలకం);
        idx = పాత్ర_పరిశీలన(h, hash, కీలకం);
        ఒకవేళ (idx == IDX_NIL) {
                ఒకవేళ (ret)
                        *ret = ఖాళీ;
                తిరిగి ఖాళీ;
        }

        e = plain_పాత్ర_at(h, idx);
        దత్తాంశం = e->విలువ;
        ఒకవేళ (ret)
                *ret = (శూన్య*) e->b.కీలకం;

        remove_నమోదు(h, idx);

        తిరిగి దత్తాంశం;
}

పూర్ణాంకం hashmap_remove_and_put(Hashmap *h, స్థిరరాశి శూన్య *old_కీలకం, స్థిరరాశి శూన్య *new_కీలకం, శూన్య *విలువ) {
        నిర్మాణం మార్పిడి_entries మార్పిడి;
        నిర్మాణం plain_hashmap_నమోదు *e;
        గుర్తులేని old_hash, new_hash, idx;

        ఒకవేళ (!h)
                తిరిగి -ENOENT;

        old_hash = పాత్ర_hash(h, old_కీలకం);
        idx = పాత్ర_పరిశీలన(h, old_hash, old_కీలకం);
        ఒకవేళ (idx == IDX_NIL)
                తిరిగి -ENOENT;

        new_hash = పాత్ర_hash(h, new_కీలకం);
        ఒకవేళ (పాత్ర_పరిశీలన(h, new_hash, new_కీలకం) != IDX_NIL)
                తిరిగి -EEXIST;

        remove_నమోదు(h, idx);

        e = &పాత్ర_at_మార్పిడి(&మార్పిడి, IDX_PUT)->p;
        e->b.కీలకం = new_కీలకం;
        e->విలువ = విలువ;
        assert_se(hashmap_put_boldly(h, new_hash, &మార్పిడి, false) == 1);

        తిరిగి 0;
}

పూర్ణాంకం సమితి_remove_and_put(సమితి_2 *s, స్థిరరాశి శూన్య *old_కీలకం, స్థిరరాశి శూన్య *new_కీలకం) {
        నిర్మాణం మార్పిడి_entries మార్పిడి;
        నిర్మాణం hashmap_పునాది_నమోదు *e;
        గుర్తులేని old_hash, new_hash, idx;

        ఒకవేళ (!s)
                తిరిగి -ENOENT;

        old_hash = పాత్ర_hash(s, old_కీలకం);
        idx = పాత్ర_పరిశీలన(s, old_hash, old_కీలకం);
        ఒకవేళ (idx == IDX_NIL)
                తిరిగి -ENOENT;

        new_hash = పాత్ర_hash(s, new_కీలకం);
        ఒకవేళ (పాత్ర_పరిశీలన(s, new_hash, new_కీలకం) != IDX_NIL)
                తిరిగి -EEXIST;

        remove_నమోదు(s, idx);

        e = &పాత్ర_at_మార్పిడి(&మార్పిడి, IDX_PUT)->p.b;
        e->కీలకం = new_కీలకం;
        assert_se(hashmap_put_boldly(s, new_hash, &మార్పిడి, false) == 1);

        తిరిగి 0;
}

పూర్ణాంకం hashmap_remove_and_replace(Hashmap *h, స్థిరరాశి శూన్య *old_కీలకం, స్థిరరాశి శూన్య *new_కీలకం, శూన్య *విలువ) {
        నిర్మాణం మార్పిడి_entries మార్పిడి;
        నిర్మాణం plain_hashmap_నమోదు *e;
        గుర్తులేని old_hash, new_hash, idx_old, idx_new;

        ఒకవేళ (!h)
                తిరిగి -ENOENT;

        old_hash = పాత్ర_hash(h, old_కీలకం);
        idx_old = పాత్ర_పరిశీలన(h, old_hash, old_కీలకం);
        ఒకవేళ (idx_old == IDX_NIL)
                తిరిగి -ENOENT;

        old_కీలకం = పాత్ర_at(HASHMAP_పునాది(h), idx_old)->కీలకం;

        new_hash = పాత్ర_hash(h, new_కీలకం);
        idx_new = పాత్ర_పరిశీలన(h, new_hash, new_కీలకం);
        ఒకవేళ (idx_new != IDX_NIL)
                ఒకవేళ (idx_old != idx_new) {
                        remove_నమోదు(h, idx_new);
                        /* Compensate for a possible backward shift. */
                        ఒకవేళ (old_కీలకం != పాత్ర_at(HASHMAP_పునాది(h), idx_old)->కీలకం)
                                idx_old = prev_idx(HASHMAP_పునాది(h), idx_old);
                        ప్రతిజ్ఞా(old_కీలకం == పాత్ర_at(HASHMAP_పునాది(h), idx_old)->కీలకం);
                }

        remove_నమోదు(h, idx_old);

        e = &పాత్ర_at_మార్పిడి(&మార్పిడి, IDX_PUT)->p;
        e->b.కీలకం = new_కీలకం;
        e->విలువ = విలువ;
        assert_se(hashmap_put_boldly(h, new_hash, &మార్పిడి, false) == 1);

        తిరిగి 0;
}

శూన్య* hashmap_remove_విలువ(Hashmap_పునాది *h, స్థిరరాశి శూన్య *కీలకం, శూన్య *విలువ) {
        నిర్మాణం hashmap_పునాది_నమోదు *e;
        గుర్తులేని hash, idx;

        ఒకవేళ (!h)
                తిరిగి ఖాళీ;

        hash = పాత్ర_hash(h, కీలకం);
        idx = పాత్ర_పరిశీలన(h, hash, కీలకం);
        ఒకవేళ (idx == IDX_NIL)
                తిరిగి ఖాళీ;

        e = పాత్ర_at(h, idx);
        ఒకవేళ (నమోదు_విలువ(h, e) != విలువ)
                తిరిగి ఖాళీ;

        remove_నమోదు(h, idx);

        తిరిగి విలువ;
}

స్థిర గుర్తులేని కనుగొను_మొదటి_నమోదు(Hashmap_పునాది *h) {
        Iterator i = ITERATOR_మొదటి;

        ఒకవేళ (!h || !n_entries(h))
                తిరిగి IDX_NIL;

        తిరిగి hashmap_iterate_నమోదు(h, &i);
}

శూన్య* hashmap_మొదటి_కీలకం_and_విలువ(Hashmap_పునాది *h, bool remove, శూన్య **ret_కీలకం) {
        నిర్మాణం hashmap_పునాది_నమోదు *e;
        శూన్య *కీలకం, *దత్తాంశం;
        గుర్తులేని idx;

        idx = కనుగొను_మొదటి_నమోదు(h);
        ఒకవేళ (idx == IDX_NIL) {
                ఒకవేళ (ret_కీలకం)
                        *ret_కీలకం = ఖాళీ;
                తిరిగి ఖాళీ;
        }

        e = పాత్ర_at(h, idx);
        కీలకం = (శూన్య*) e->కీలకం;
        దత్తాంశం = నమోదు_విలువ(h, e);

        ఒకవేళ (remove)
                remove_నమోదు(h, idx);

        ఒకవేళ (ret_కీలకం)
                *ret_కీలకం = కీలకం;

        తిరిగి దత్తాంశం;
}

గుర్తులేని hashmap_కొలత(Hashmap_పునాది *h) {
        ఒకవేళ (!h)
                తిరిగి 0;

        తిరిగి n_entries(h);
}

గుర్తులేని _hashmap_buckets(Hashmap_పునాది *h) {
        ఒకవేళ (!h)
                తిరిగి 0;

        తిరిగి n_buckets(h);
}

పూర్ణాంకం hashmap_విలీనం(Hashmap *h, Hashmap *other) {
        Iterator i;
        గుర్తులేని idx;

        ప్రతిజ్ఞా(h);

        HASHMAP_FOREACH_IDX(idx, HASHMAP_పునాది(other), i) {
                నిర్మాణం plain_hashmap_నమోదు *pe = plain_పాత్ర_at(other, idx);
                పూర్ణాంకం r;

                r = hashmap_put(h, pe->b.కీలకం, pe->విలువ);
                ఒకవేళ (r < 0 && r != -EEXIST)
                        తిరిగి r;
        }

        తిరిగి 0;
}

పూర్ణాంకం సమితి_విలీనం(సమితి_2 *s, సమితి_2 *other) {
        Iterator i;
        గుర్తులేని idx;

        ప్రతిజ్ఞా(s);

        HASHMAP_FOREACH_IDX(idx, HASHMAP_పునాది(other), i) {
                నిర్మాణం సమితి_నమోదు *se = సమితి_పాత్ర_at(other, idx);
                పూర్ణాంకం r;

                r = సమితి_put(s, se->b.కీలకం);
                ఒకవేళ (r < 0)
                        తిరిగి r;
        }

        తిరిగి 0;
}

పూర్ణాంకం _hashmap_reserve(Hashmap_పునాది *h, గుర్తులేని entries_కూడు) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(h);

        r = resize_buckets(h, entries_కూడు);
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి 0;
}

/*
 * The same as hashmap_merge(), but every new item from other is moved to h.
 * Keys already in h are skipped and stay in other.
 * Returns: 0 on success.
 *          -ENOMEM on alloc failure, in which case no move has been done.
 */
పూర్ణాంకం _hashmap_move(Hashmap_పునాది *h, Hashmap_పునాది *other) {
        నిర్మాణం మార్పిడి_entries మార్పిడి;
        నిర్మాణం hashmap_పునాది_నమోదు *e, *n;
        Iterator i;
        గుర్తులేని idx;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(h);

        ఒకవేళ (!other)
                తిరిగి 0;

        ప్రతిజ్ఞా(other->type == h->type);

        /*
         * This reserves buckets for the worst case, where none of other's
         * entries are yet present in h. This is preferable to risking
         * an allocation failure in the middle of the moving and having to
         * rollback or return a partial result.
         */
        r = resize_buckets(h, n_entries(other));
        ఒకవేళ (r < 0)
                తిరిగి r;

        HASHMAP_FOREACH_IDX(idx, other, i) {
                గుర్తులేని h_hash;

                e = పాత్ర_at(other, idx);
                h_hash = పాత్ర_hash(h, e->కీలకం);
                ఒకవేళ (పాత్ర_పరిశీలన(h, h_hash, e->కీలకం) != IDX_NIL)
                        కొనసాగు;

                n = &పాత్ర_at_మార్పిడి(&మార్పిడి, IDX_PUT)->p.b;
                n->కీలకం = e->కీలకం;
                ఒకవేళ (h->type != HASHMAP_TYPE_సమితి)
                        ((నిర్మాణం plain_hashmap_నమోదు*) n)->విలువ =
                                ((నిర్మాణం plain_hashmap_నమోదు*) e)->విలువ;
                assert_se(hashmap_put_boldly(h, h_hash, &మార్పిడి, false) == 1);

                remove_నమోదు(other, idx);
        }

        తిరిగి 0;
}

పూర్ణాంకం _hashmap_move_one(Hashmap_పునాది *h, Hashmap_పునాది *other, స్థిరరాశి శూన్య *కీలకం) {
        నిర్మాణం మార్పిడి_entries మార్పిడి;
        గుర్తులేని h_hash, other_hash, idx;
        నిర్మాణం hashmap_పునాది_నమోదు *e, *n;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(h);

        h_hash = పాత్ర_hash(h, కీలకం);
        ఒకవేళ (పాత్ర_పరిశీలన(h, h_hash, కీలకం) != IDX_NIL)
                తిరిగి -EEXIST;

        ఒకవేళ (!other)
                తిరిగి -ENOENT;

        ప్రతిజ్ఞా(other->type == h->type);

        other_hash = పాత్ర_hash(other, కీలకం);
        idx = పాత్ర_పరిశీలన(other, other_hash, కీలకం);
        ఒకవేళ (idx == IDX_NIL)
                తిరిగి -ENOENT;

        e = పాత్ర_at(other, idx);

        n = &పాత్ర_at_మార్పిడి(&మార్పిడి, IDX_PUT)->p.b;
        n->కీలకం = e->కీలకం;
        ఒకవేళ (h->type != HASHMAP_TYPE_సమితి)
                ((నిర్మాణం plain_hashmap_నమోదు*) n)->విలువ =
                        ((నిర్మాణం plain_hashmap_నమోదు*) e)->విలువ;
        r = hashmap_put_boldly(h, h_hash, &మార్పిడి, true);
        ఒకవేళ (r < 0)
                తిరిగి r;

        remove_నమోదు(other, idx);
        తిరిగి 0;
}

Hashmap_పునాది* _hashmap_copy(Hashmap_పునాది *h) {
        Hashmap_పునాది *copy;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(h);

        copy = hashmap_పునాది_new(h->hash_ops, h->type);
        ఒకవేళ (!copy)
                తిరిగి ఖాళీ;

        ఎంపిక (h->type) {
        స్థితి HASHMAP_TYPE_PLAIN:
        స్థితి HASHMAP_TYPE_ORDERED:
                r = hashmap_విలీనం_2((Hashmap*)copy, (Hashmap*)h);
                ఆపు;
        స్థితి HASHMAP_TYPE_సమితి:
                r = సమితి_విలీనం((సమితి_2*)copy, (సమితి_2*)h);
                ఆపు;
        అప్రమేయ:
                assert_not_reached();
        }

        ఒకవేళ (r < 0)
                తిరిగి _hashmap_free(copy);

        తిరిగి copy;
}

అక్షరం** _hashmap_get_strv(Hashmap_పునాది *h) {
        అక్షరం **sv;
        Iterator i;
        గుర్తులేని idx, n;

        ఒకవేళ (!h)
                తిరిగి new0(అక్షరం*, 1);

        sv = new(అక్షరం*, n_entries(h)+1);
        ఒకవేళ (!sv)
                తిరిగి ఖాళీ;

        n = 0;
        HASHMAP_FOREACH_IDX(idx, h, i)
                sv[n++] = నమోదు_విలువ(h, పాత్ర_at(h, idx));
        sv[n] = ఖాళీ;

        తిరిగి sv;
}

అక్షరం** సమితి_గమ్యం_strv(సమితి_2 **s) {
        ప్రతిజ్ఞా(s);

        /* This is similar to set_get_strv(), but invalidates the set on success. */

        అక్షరం **v = new(అక్షరం*, సమితి_కొలత(*s) + 1);
        ఒకవేళ (!v)
                తిరిగి ఖాళీ;

        కొరకు (అక్షరం **p = v; (*p = సమితి_steal_మొదటి(*s)); p++)
                ;

        ప్రతిజ్ఞా(సమితి_isempty(*s));
        *s = సమితి_free(*s);
        తిరిగి v;
}

శూన్య* ordered_hashmap_తదుపరి(OrderedHashmap *h, స్థిరరాశి శూన్య *కీలకం) {
        నిర్మాణం ordered_hashmap_నమోదు *e;
        గుర్తులేని hash, idx;

        ఒకవేళ (!h)
                తిరిగి ఖాళీ;

        hash = పాత్ర_hash(h, కీలకం);
        idx = పాత్ర_పరిశీలన(h, hash, కీలకం);
        ఒకవేళ (idx == IDX_NIL)
                తిరిగి ఖాళీ;

        e = ordered_పాత్ర_at(h, idx);
        ఒకవేళ (e->iterate_తదుపరి == IDX_NIL)
                తిరిగి ఖాళీ;
        తిరిగి ordered_పాత్ర_at(h, e->iterate_తదుపరి)->p.విలువ;
}

పూర్ణాంకం సమితి_consume(సమితి_2 *s, శూన్య *విలువ) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(s);
        ప్రతిజ్ఞా(విలువ);

        r = సమితి_put(s, విలువ);
        ఒకవేళ (r <= 0)
                విడుదల(విలువ);

        తిరిగి r;
}

పూర్ణాంకం hashmap_put_strdup_నిండిన(Hashmap **h, స్థిరరాశి నిర్మాణం hash_ops *hash_ops, స్థిరరాశి అక్షరం *k, స్థిరరాశి అక్షరం *v) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(h);

        r = hashmap_ensure_allocated(h, hash_ops);
        ఒకవేళ (r < 0)
                తిరిగి r;

        _cleanup_free_ అక్షరం *kdup = ఖాళీ, *vdup = ఖాళీ;

        kdup = strdup(k);
        ఒకవేళ (!kdup)
                తిరిగి -ENOMEM;

        ఒకవేళ (v) {
                vdup = strdup(v);
                ఒకవేళ (!vdup)
                        తిరిగి -ENOMEM;
        }

        r = hashmap_put(*h, kdup, vdup);
        ఒకవేళ (r < 0) {
                ఒకవేళ (r == -EEXIST && streq_ptr(v, hashmap_get(*h, kdup)))
                        తిరిగి 0;
                తిరిగి r;
        }

        /* 0 with non-null vdup would mean vdup is already in the hashmap, which cannot be */
        ప్రతిజ్ఞా(vdup == ఖాళీ || r > 0);
        ఒకవేళ (r > 0)
                kdup = vdup = ఖాళీ;

        తిరిగి r;
}

పూర్ణాంకం సమితి_put_strndup_నిండిన(సమితి_2 **s, స్థిరరాశి నిర్మాణం hash_ops *hash_ops, స్థిరరాశి అక్షరం *p, size_t n) {
        అక్షరం *c;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(s);
        ప్రతిజ్ఞా(p);

        r = సమితి_ensure_allocated(s, hash_ops);
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (n == SIZE_MAX) {
                ఒకవేళ (సమితి_కలిగియుంది(*s, (అక్షరం*) p))
                        తిరిగి 0;

                c = strdup(p);
        } లేకపోతే
                c = strndup(p, n);
        ఒకవేళ (!c)
                తిరిగి -ENOMEM;

        తిరిగి సమితి_consume(*s, c);
}

పూర్ణాంకం సమితి_put_strdupv_నిండిన(సమితి_2 **s, స్థిరరాశి నిర్మాణం hash_ops *hash_ops, అక్షరం **l) {
        పూర్ణాంకం n = 0, r;

        ప్రతిజ్ఞా(s);

        STRV_FOREACH(i, l) {
                r = సమితి_put_strndup_నిండిన(s, hash_ops, *i, SIZE_MAX);
                ఒకవేళ (r < 0)
                        తిరిగి r;

                n += r;
        }

        తిరిగి n;
}

పూర్ణాంకం సమితి_put_strsplit(సమితి_2 *s, స్థిరరాశి అక్షరం *v, స్థిరరాశి అక్షరం *separators, వెలికితీయి_Flags flags) {
        స్థిరరాశి అక్షరం *p = ASSERT_PTR(v);
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(s);

        కొరకు (;;) {
                అక్షరం *మాట;

                r = వెలికితీయి_మొదటి_మాట(&p, &మాట, separators, flags);
                ఒకవేళ (r <= 0)
                        తిరిగి r;

                r = సమితి_consume(s, మాట);
                ఒకవేళ (r < 0)
                        తిరిగి r;
        }
}

/* expand the cachemem if needed, return true if newly (re)activated. */
స్థిర పూర్ణాంకం cachemem_maintain(CacheMem *mem, size_t కొలత) {
        ప్రతిజ్ఞా(mem);

        ఒకవేళ (!GREEDY_REALLOC(mem->ptr, కొలత)) {
                ఒకవేళ (కొలత > 0)
                        తిరిగి -ENOMEM;
        }

        ఒకవేళ (!mem->చురుకైన) {
                mem->చురుకైన = true;
                తిరిగి true;
        }

        తిరిగి false;
}

పూర్ణాంకం iterated_cache_get(IteratedCache *cache, స్థిరరాశి శూన్య ***res_కీలకాలు, స్థిరరాశి శూన్య ***res_విలువలు, గుర్తులేని *res_n_entries) {
        bool sync_కీలకాలు = false, sync_విలువలు = false;
        size_t కొలత;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(cache);
        ప్రతిజ్ఞా(cache->hashmap);

        కొలత = n_entries(cache->hashmap);

        ఒకవేళ (res_కీలకాలు) {
                r = cachemem_maintain(&cache->కీలకాలు, కొలత);
                ఒకవేళ (r < 0)
                        తిరిగి r;

                sync_కీలకాలు = r;
        } లేకపోతే
                cache->కీలకాలు.చురుకైన = false;

        ఒకవేళ (res_విలువలు) {
                r = cachemem_maintain(&cache->values, కొలత);
                ఒకవేళ (r < 0)
                        తిరిగి r;

                sync_విలువలు = r;
        } లేకపోతే
                cache->values.చురుకైన = false;

        ఒకవేళ (cache->hashmap->dirty) {
                ఒకవేళ (cache->కీలకాలు.చురుకైన)
                        sync_కీలకాలు = true;
                ఒకవేళ (cache->values.చురుకైన)
                        sync_విలువలు = true;

                cache->hashmap->dirty = false;
        }

        ఒకవేళ (sync_కీలకాలు || sync_విలువలు) {
                గుర్తులేని i, idx;
                Iterator iter;

                i = 0;
                HASHMAP_FOREACH_IDX(idx, cache->hashmap, iter) {
                        నిర్మాణం hashmap_పునాది_నమోదు *e;

                        e = పాత్ర_at(cache->hashmap, idx);

                        ఒకవేళ (sync_కీలకాలు)
                                cache->కీలకాలు.ptr[i] = e->కీలకం;
                        ఒకవేళ (sync_విలువలు)
                                cache->values.ptr[i] = నమోదు_విలువ(cache->hashmap, e);
                        i++;
                }
        }

        ఒకవేళ (res_కీలకాలు)
                *res_కీలకాలు = cache->కీలకాలు.ptr;
        ఒకవేళ (res_విలువలు)
                *res_విలువలు = cache->values.ptr;
        ఒకవేళ (res_n_entries)
                *res_n_entries = కొలత;

        తిరిగి 0;
}

IteratedCache* iterated_cache_free(IteratedCache *cache) {
        ఒకవేళ (cache) {
                విడుదల(cache->కీలకాలు.ptr);
                విడుదల(cache->values.ptr);
        }

        తిరిగి mfree(cache);
}

పూర్ణాంకం సమితి_strjoin(సమితి_2 *s, స్థిరరాశి అక్షరం *separator, bool wrap_with_separator, అక్షరం **ret) {
        _cleanup_free_ అక్షరం *str = ఖాళీ;
        size_t separator_len, len = 0;
        స్థిరరాశి అక్షరం *విలువ;
        bool మొదటి;

        ప్రతిజ్ఞా(ret);

        ఒకవేళ (సమితి_isempty(s)) {
                *ret = ఖాళీ;
                తిరిగి 0;
        }

        separator_len = strlen_ptr(separator);

        ఒకవేళ (separator_len == 0)
                wrap_with_separator = false;

        మొదటి = !wrap_with_separator;

        సమితి_FOREACH(విలువ, s) {
                size_t l = strlen_ptr(విలువ);

                ఒకవేళ (l == 0)
                        కొనసాగు;

                ఒకవేళ (!GREEDY_REALLOC(str, len + l + (మొదటి ? 0 : separator_len) + (wrap_with_separator ? separator_len : 0) + 1))
                        తిరిగి -ENOMEM;

                ఒకవేళ (separator_len > 0 && !మొదటి) {
                        జ్ఞాపకం_నకలు(str + len, separator, separator_len);
                        len += separator_len;
                }

                జ్ఞాపకం_నకలు(str + len, విలువ, l);
                len += l;
                మొదటి = false;
        }

        ఒకవేళ (wrap_with_separator) {
                జ్ఞాపకం_నకలు(str + len, separator, separator_len);
                len += separator_len;
        }

        str[len] = '\0';

        *ret = TAKE_PTR(str);
        తిరిగి 0;
}

bool సమితి_equal(సమితి_2 *a, సమితి_2 *b) {
        శూన్య *p;

        /* Checks whether each entry of 'a' is also in 'b' and vice versa, i.e. the two sets contain the same
         * entries */

        ఒకవేళ (a == b)
                తిరిగి true;

        ఒకవేళ (సమితి_isempty(a) && సమితి_isempty(b))
                తిరిగి true;

        ఒకవేళ (సమితి_కొలత(a) != సమితి_కొలత(b)) /* Cheap check that hopefully catches a lot of inequality cases
                                         * already */
                తిరిగి false;

        సమితి_FOREACH(p, a)
                ఒకవేళ (!సమితి_కలిగియుంది(b, p))
                        తిరిగి false;

        /* If we have the same hashops, then we don't need to check things backwards given we compared the
         * size and that all of a is in b. */
        ఒకవేళ (a->b.hash_ops == b->b.hash_ops)
                తిరిగి true;

        సమితి_FOREACH(p, b)
                ఒకవేళ (!సమితి_కలిగియుంది(a, p))
                        తిరిగి false;

        తిరిగి true;
}

స్థిర bool సమితి_fnmatch_one(సమితి_2 *patterns, స్థిరరాశి అక్షరం *needle) {
        స్థిరరాశి అక్షరం *p;

        ప్రతిజ్ఞా(needle);

        /* Any failure of fnmatch() is treated as equivalent to FNM_NOMATCH, i.e. as non-matching pattern */

        సమితి_FOREACH(p, patterns)
                ఒకవేళ (fnmatch(p, needle, 0) == 0)
                        తిరిగి true;

        తిరిగి false;
}

bool సమితి_fnmatch(సమితి_2 *include_patterns, సమితి_2 *exclude_patterns, స్థిరరాశి అక్షరం *needle) {
        ప్రతిజ్ఞా(needle);

        ఒకవేళ (సమితి_fnmatch_one(exclude_patterns, needle))
                తిరిగి false;

        ఒకవేళ (సమితి_isempty(include_patterns))
                తిరిగి true;

        తిరిగి సమితి_fnmatch_one(include_patterns, needle);
}

స్థిర పూర్ణాంకం hashmap_నమోదు_compare(
                నిర్మాణం hashmap_పునాది_నమోదు * స్థిరరాశి *a,
                నిర్మాణం hashmap_పునాది_నమోదు * స్థిరరాశి *b,
                compare_func_t compare) {

        ప్రతిజ్ఞా(a && *a);
        ప్రతిజ్ఞా(b && *b);
        ప్రతిజ్ఞా(compare);

        తిరిగి compare((*a)->కీలకం, (*b)->కీలకం);
}

స్థిర పూర్ణాంకం _hashmap_dump_entries_sorted(
                Hashmap_పునాది *h,
                శూన్య ***ret,
                size_t *ret_n) {
        _cleanup_free_ శూన్య **entries = ఖాళీ;
        Iterator iter;
        గుర్తులేని idx;
        size_t n = 0;

        ప్రతిజ్ఞా(ret);
        ప్రతిజ్ఞా(ret_n);

        ఒకవేళ (hashmap_కొలత(h) == 0) {
                *ret = ఖాళీ;
                *ret_n = 0;
                తిరిగి 0;
        }

        /* We append one more element than needed so that the resulting array can be used as a strv. We
         * don't count this entry in the returned size. */
        entries = new(శూన్య*, hashmap_కొలత(h) + 1);
        ఒకవేళ (!entries)
                తిరిగి -ENOMEM;

        HASHMAP_FOREACH_IDX(idx, h, iter)
                entries[n++] = పాత్ర_at(h, idx);

        ప్రతిజ్ఞా(n == hashmap_కొలత(h));
        entries[n] = ఖాళీ;

        typesafe_qsort_r((నిర్మాణం hashmap_పునాది_నమోదు**) entries, n,
                         hashmap_నమోదు_compare, h->hash_ops->compare);

        *ret = TAKE_PTR(entries);
        *ret_n = n;
        తిరిగి 0;
}

పూర్ణాంకం hashmap_dump_కీలకాలు_sorted(Hashmap_పునాది *h, శూన్య ***ret, size_t *ret_n) {
        _cleanup_free_ శూన్య **entries = ఖాళీ;
        size_t n;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(ret);

        r = _hashmap_dump_entries_sorted(h, &entries, &n);
        ఒకవేళ (r < 0)
                తిరిగి r;

        /* Reuse the array. */
        FOREACH_ARRAY(e, entries, n)
                *e = (శూన్య*) (*(నిర్మాణం hashmap_పునాది_నమోదు**) e)->కీలకం;

        *ret = TAKE_PTR(entries);
        ఒకవేళ (ret_n)
                *ret_n = n;
        తిరిగి 0;
}

పూర్ణాంకం _hashmap_dump_sorted(Hashmap_పునాది *h, శూన్య ***ret, size_t *ret_n) {
        _cleanup_free_ శూన్య **entries = ఖాళీ;
        size_t n;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(ret);

        r = _hashmap_dump_entries_sorted(h, &entries, &n);
        ఒకవేళ (r < 0)
                తిరిగి r;

        /* Reuse the array. */
        FOREACH_ARRAY(e, entries, n)
                *e = నమోదు_విలువ(h, *(నిర్మాణం hashmap_పునాది_నమోదు**) e);

        *ret = TAKE_PTR(entries);
        ఒకవేళ (ret_n)
                *ret_n = n;
        తిరిగి 0;
}
