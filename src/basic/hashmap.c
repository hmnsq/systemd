/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <fnmatch.h>
#ضمن <pthread.h>
#ضمن <unistd.h>
#إذا HAVE_VALGRIND_VALGRIND_H
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

#إذا ENABLE_DEBUG_HASHMAP
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
        ثابت فراغ *ئاچقۇچ;
};

/* Entry types for specific hashmap/set types
 * hashmap_base_entry must be at the beginning of each entry struct. */

بنية plain_hashmap_entry {
        بنية hashmap_base_entry b;
        فراغ *قىممەت;
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

#إذا ENABLE_DEBUG_HASHMAP
بنية hashmap_debug_info {
        LIST_FIELDS(بنية hashmap_debug_info, debug_list);
        unsigned max_entries;  /* high watermark of n_entries */

        /* fields to detect modification while iterating */
        unsigned put_سانى;    /* counts puts into the hashmap */
        unsigned rem_سانى;    /* counts removals from hashmap */
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
        uint8_t  hash_ئاچقۇچ[HASH_ئاچقۇچ_SIZE];  /* hash key; changes during resize */

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
ساكن uint8_t shared_hash_ئاچقۇچ[HASH_ئاچقۇچ_SIZE];

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

#إذا ENABLE_DEBUG_HASHMAP
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
        CacheMem ئاچقۇچلار, values;
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
        إذا (getpid() != gettid())
                أرجع (فراغ) log_debug("Not cleaning up memory pools, not in main thread.");

        r = get_process_threads(0);
        إذا (r < 0)
                أرجع (فراغ) log_debug_errno(r, "Failed to determine number of threads, not cleaning up memory pools: %m");
        إذا (r != 1)
                أرجع (فراغ) log_debug("Not cleaning up memory pools, running in multi-threaded process.");

        mempool_trim(&hashmap_pool);
        mempool_trim(&ordered_hashmap_pool);
}

#إذا HAVE_VALGRIND_VALGRIND_H
_destructor_ ساكن فراغ cleanup_pools(فراغ) {
        /* Be nice to valgrind */
        إذا (RUNNING_ON_VALGRIND)
                hashmap_trim_pools();
}
#endif

ساكن مضمن unsigned n_buckets(HashmapBase *h) {
        أكد(h);
        أرجع h->has_indirect ? h->indirect.n_buckets
                               : hashmap_type_info[h->type].n_direct_buckets;
}

ساكن unsigned n_entries(HashmapBase *h) {
        أكد(h);
        أرجع h->has_indirect ? h->indirect.n_entries
                               : h->n_direct_entries;
}

ساكن فراغ n_entries_inc(HashmapBase *h) {
        أكد(h);

        إذا (h->has_indirect)
                h->indirect.n_entries++;
        وإلا
                h->n_direct_entries++;
}

ساكن فراغ n_entries_dec(HashmapBase *h) {
        أكد(h);

        إذا (h->has_indirect)
                h->indirect.n_entries--;
        وإلا
                h->n_direct_entries--;
}

ساكن مضمن فراغ* storage_ptr(HashmapBase *h) {
        أكد(h);
        أرجع h->has_indirect ? h->indirect.storage
                               : h->direct.storage;
}

ساكن مضمن uint8_t* hash_ئاچقۇچ(HashmapBase *h) {
        أكد(h);
        أرجع h->has_indirect ? h->indirect.hash_ئاچقۇچ
                               : shared_hash_ئاچقۇچ;
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
        إذا (h->hash_ops->hash == trivial_hash_func)
                hash = siphash24_uint64((uintptr_t) p, hash_ئاچقۇچ(h));
        وإلا إذا (h->hash_ops->hash == (hash_func_t) uint64_hash_func)
                hash = siphash24_uint64(*(ثابت uint64_t*) p, hash_ئاچقۇچ(h));
        وإلا {
                siphash24_init(&state, hash_ئاچقۇچ(h));
                h->hash_ops->hash(p, &state);
                hash = siphash24_finalize(&state);
        }

        /* n_buckets is not a power of two, so a modulo needs a division. Multiplying the upper 32 bits of
         * the hash with n_buckets and keeping the upper 32 bits of the product maps the hash to
         * [0, n_buckets) almost as evenly, without a division. */
        أرجع (unsigned) (((hash >> 32) * n_buckets(h)) >> 32);
}
#define bucket_hash(h, p) base_bucket_hash(HASHMAP_BASE(h), p)

ساكن فراغ base_set_dirty(HashmapBase *h) {
        أكد(h);

        h->dirty = true;
}
#define hashmap_set_dirty(h) base_set_dirty(HASHMAP_BASE(h))

ساكن فراغ get_hash_ئاچقۇچ(uint8_t hash_ئاچقۇچ[HASH_ئاچقۇچ_SIZE], bool reuse_is_ok) {
        ساكن uint8_t current[HASH_ئاچقۇچ_SIZE];
        ساكن bool current_initialized = false;

        /* Returns a hash function key to use. In order to keep things
         * fast we will not generate a new key each time we allocate a
         * new hash table. Instead, we'll just reuse the most recently
         * generated one, except if we never generated one or when we
         * are rehashing an entire hash table because we reached a
         * fill level */

        إذا (!current_initialized || !reuse_is_ok) {
                random_bytes(current, حجم(current));
                current_initialized = true;
        }

        memcpy(hash_ئاچقۇچ, current, حجم(current));
}

ساكن مضمن بنية hashmap_base_entry* bucket_at(HashmapBase *h, unsigned idx) {
        أكد(h);
        أرجع CAST_ALIGN_PTR(
                        بنية hashmap_base_entry,
                        (uint8_t *) storage_ptr(h) + idx * hashmap_type_info[h->type].entry_size);
}

ساكن بنية plain_hashmap_entry* plain_bucket_at(Hashmap *h, unsigned idx) {
        أرجع (بنية plain_hashmap_entry*) bucket_at(HASHMAP_BASE(h), idx);
}

ساكن بنية ordered_hashmap_entry* ordered_bucket_at(OrderedHashmap *h, unsigned idx) {
        أرجع (بنية ordered_hashmap_entry*) bucket_at(HASHMAP_BASE(h), idx);
}

ساكن بنية set_entry *set_bucket_at(Set *h, unsigned idx) {
        أرجع (بنية set_entry*) bucket_at(HASHMAP_BASE(h), idx);
}

ساكن بنية ordered_hashmap_entry* bucket_at_swap(بنية swap_entries *swap, unsigned idx) {
        أكد(swap);
        أرجع &swap->e[idx - _IDX_SWAP_BEGIN];
}

/* Returns a pointer to the bucket at index idx.
 * Understands real indexes and swap indexes, hence "_virtual". */
ساكن بنية hashmap_base_entry* bucket_at_virtual(HashmapBase *h, بنية swap_entries *swap,
                                                    unsigned idx) {
        إذا (idx < _IDX_SWAP_BEGIN)
                أرجع bucket_at(h, idx);

        إذا (idx < _IDX_SWAP_END)
                أرجع &bucket_at_swap(swap, idx)->p.b;

        assert_not_reached();
}

ساكن مضمن dib_raw_t* dib_raw_ptr(HashmapBase *h) {
        أكد(h);
        أرجع (dib_raw_t*)
                ((uint8_t*) storage_ptr(h) + hashmap_type_info[h->type].entry_size * n_buckets(h));
}

ساكن unsigned bucket_distance(HashmapBase *h, unsigned idx, unsigned from) {
        أرجع idx >= from ? idx - from
                           : n_buckets(h) + idx - from;
}

ساكن unsigned bucket_calculate_dib(HashmapBase *h, unsigned idx, dib_raw_t raw_dib) {
        unsigned initial_bucket;

        إذا (raw_dib == DIB_RAW_FREE)
                أرجع DIB_FREE;

        إذا (_likely_(raw_dib < DIB_RAW_OVERFLOW))
                أرجع raw_dib;

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
        initial_bucket = bucket_hash(h, bucket_at(h, idx)->ئاچقۇچ);
        أرجع bucket_distance(h, idx, initial_bucket);
}

ساكن فراغ bucket_set_dib(HashmapBase *h, unsigned idx, unsigned dib) {
        dib_raw_ptr(h)[idx] = dib != DIB_FREE ? MIN(dib, DIB_RAW_OVERFLOW) : DIB_RAW_FREE;
}

ساكن unsigned skip_free_buckets(HashmapBase *h, unsigned idx) {
        dib_raw_t *dibs;

        dibs = dib_raw_ptr(h);

        لكل ( ; idx < n_buckets(h); idx++)
                إذا (dibs[idx] != DIB_RAW_FREE)
                        أرجع idx;

        أرجع IDX_NIL;
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

        إذا (h->type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*) h;
                بنية ordered_hashmap_entry *le, *le_to;

                le_to = (بنية ordered_hashmap_entry*) e_to;

                إذا (le_to->iterate_next != IDX_NIL) {
                        le = (بنية ordered_hashmap_entry*)
                             bucket_at_virtual(h, swap, le_to->iterate_next);
                        le->iterate_previous = to;
                }

                إذا (le_to->iterate_previous != IDX_NIL) {
                        le = (بنية ordered_hashmap_entry*)
                             bucket_at_virtual(h, swap, le_to->iterate_previous);
                        le->iterate_next = to;
                }

                إذا (lh->iterate_list_head == from)
                        lh->iterate_list_head = to;
                إذا (lh->iterate_list_tail == from)
                        lh->iterate_list_tail = to;
        }
}

ساكن مضمن unsigned next_idx(HashmapBase *h, unsigned idx) {
        أرجع idx + 1U < n_buckets(h) ? idx + 1U : 0U;
}

ساكن unsigned prev_idx(HashmapBase *h, unsigned idx) {
        أرجع idx > 0U ? idx - 1U : n_buckets(h) - 1U;
}

ساكن مضمن فراغ* entry_قىممەت(HashmapBase *h, بنية hashmap_base_entry *e) {
        أكد(h);
        أكد(e);

        بدل (h->type) {

        حالة HASHMAP_TYPE_PLAIN:
        حالة HASHMAP_TYPE_ORDERED:
                أرجع ((بنية plain_hashmap_entry*)e)->قىممەت;

        حالة HASHMAP_TYPE_SET:
                أرجع (فراغ*) e->ئاچقۇچ;

        افتراضي:
                assert_not_reached();
        }
}

ساكن فراغ base_remove_entry(HashmapBase *h, unsigned idx) {
        unsigned سول, ئوڭ, prev, dib;
        dib_raw_t raw_dib, *dibs;

        dibs = dib_raw_ptr(h);
        أكد(dibs[idx] != DIB_RAW_FREE);

#إذا ENABLE_DEBUG_HASHMAP
        أكد(h);
        h->debug.rem_سانى++;
        h->debug.last_rem_idx = idx;
#endif

        سول = idx;
        /* Find the stop bucket ("right"). It is either free or has DIB == 0. */
        لكل (ئوڭ = next_idx(h, سول); ; ئوڭ = next_idx(h, ئوڭ)) {
                raw_dib = dibs[ئوڭ];
                إذا (IN_SET(raw_dib, 0, DIB_RAW_FREE))
                        اكسر;

                /* The buckets are not supposed to be all occupied and with DIB > 0.
                 * That would mean we could make everyone better off by shifting them
                 * backward. This scenario is impossible. */
                أكد(سول != ئوڭ);
        }

        إذا (h->type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*) h;
                بنية ordered_hashmap_entry *le = ordered_bucket_at(lh, idx);

                إذا (le->iterate_next != IDX_NIL)
                        ordered_bucket_at(lh, le->iterate_next)->iterate_previous = le->iterate_previous;
                وإلا
                        lh->iterate_list_tail = le->iterate_previous;

                إذا (le->iterate_previous != IDX_NIL)
                        ordered_bucket_at(lh, le->iterate_previous)->iterate_next = le->iterate_next;
                وإلا
                        lh->iterate_list_head = le->iterate_next;
        }

        /* Now shift all buckets in the interval (left, right) one step backwards */
        لكل (prev = سول, سول = next_idx(h, سول); سول != ئوڭ;
             prev = سول, سول = next_idx(h, سول)) {
                dib = bucket_calculate_dib(h, سول, dibs[سول]);
                أكد(dib != 0);
                bucket_move_entry(h, عدم, سول, prev);
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

        إذا (i->idx == IDX_NIL)
                اذهب at_end;

        إذا (i->idx == IDX_FIRST && h->iterate_list_head == IDX_NIL)
                اذهب at_end;

        إذا (i->idx == IDX_FIRST) {
                idx = h->iterate_list_head;
                e = ordered_bucket_at(h, idx);
        } وإلا {
                idx = i->idx;
                e = ordered_bucket_at(h, idx);
                /*
                 * We allow removing the current entry while iterating, but removal may cause
                 * a backward shift. The next entry may thus move one bucket to the left.
                 * To detect when it happens, we remember the key pointer of the entry we were
                 * going to iterate next. If it does not match, there was a backward shift.
                 */
                إذا (e->p.b.ئاچقۇچ != i->next_ئاچقۇچ) {
                        idx = prev_idx(HASHMAP_BASE(h), idx);
                        e = ordered_bucket_at(h, idx);
                }
                أكد(e->p.b.ئاچقۇچ == i->next_ئاچقۇچ);
        }

#إذا ENABLE_DEBUG_HASHMAP
        i->prev_idx = idx;
#endif

        إذا (e->iterate_next != IDX_NIL) {
                بنية ordered_hashmap_entry *n;
                i->idx = e->iterate_next;
                n = ordered_bucket_at(h, i->idx);
                i->next_ئاچقۇچ = n->p.b.ئاچقۇچ;
        } وإلا
                i->idx = IDX_NIL;

        أرجع idx;

at_end:
        i->idx = IDX_NIL;
        أرجع IDX_NIL;
}

ساكن unsigned hashmap_iterate_in_internal_order(HashmapBase *h, Iterator *i) {
        unsigned idx;

        أكد(h);
        أكد(i);

        إذا (i->idx == IDX_NIL)
                اذهب at_end;

        إذا (i->idx == IDX_FIRST) {
                /* fast forward to the first occupied bucket */
                إذا (h->has_indirect) {
                        i->idx = skip_free_buckets(h, h->indirect.idx_lowest_entry);
                        h->indirect.idx_lowest_entry = i->idx;
                } وإلا
                        i->idx = skip_free_buckets(h, 0);

                إذا (i->idx == IDX_NIL)
                        اذهب at_end;
        } وإلا {
                بنية hashmap_base_entry *e;

                أكد(i->idx > 0);

                e = bucket_at(h, i->idx);
                /*
                 * We allow removing the current entry while iterating, but removal may cause
                 * a backward shift. The next entry may thus move one bucket to the left.
                 * To detect when it happens, we remember the key pointer of the entry we were
                 * going to iterate next. If it does not match, there was a backward shift.
                 */
                إذا (e->ئاچقۇچ != i->next_ئاچقۇچ)
                        e = bucket_at(h, --i->idx);

                أكد(e->ئاچقۇچ == i->next_ئاچقۇچ);
        }

        idx = i->idx;
#إذا ENABLE_DEBUG_HASHMAP
        i->prev_idx = idx;
#endif

        i->idx = skip_free_buckets(h, i->idx + 1);
        إذا (i->idx != IDX_NIL)
                i->next_ئاچقۇچ = bucket_at(h, i->idx)->ئاچقۇچ;
        وإلا
                i->idx = IDX_NIL;

        أرجع idx;

at_end:
        i->idx = IDX_NIL;
        أرجع IDX_NIL;
}

ساكن unsigned hashmap_iterate_entry(HashmapBase *h, Iterator *i) {
        إذا (!h) {
                i->idx = IDX_NIL;
                أرجع IDX_NIL;
        }

#إذا ENABLE_DEBUG_HASHMAP
        إذا (i->idx == IDX_FIRST) {
                i->put_سانى = h->debug.put_سانى;
                i->rem_سانى = h->debug.rem_سانى;
        } وإلا {
                /* While iterating, must not add any new entries */
                أكد(i->put_سانى == h->debug.put_سانى);
                /* ... or remove entries other than the current one */
                أكد(i->rem_سانى == h->debug.rem_سانى ||
                       (i->rem_سانى == h->debug.rem_سانى - 1 &&
                        i->prev_idx == h->debug.last_rem_idx));
                /* Reset our removals counter */
                i->rem_سانى = h->debug.rem_سانى;
        }
#endif

        أرجع h->type == HASHMAP_TYPE_ORDERED ? hashmap_iterate_in_insertion_order((OrderedHashmap*) h, i)
                                               : hashmap_iterate_in_internal_order(h, i);
}

bool _hashmap_iterate(HashmapBase *h, Iterator *i, فراغ **قىممەت, ثابت فراغ **ئاچقۇچ) {
        بنية hashmap_base_entry *e;
        فراغ *data;
        unsigned idx;

        idx = hashmap_iterate_entry(h, i);
        إذا (idx == IDX_NIL) {
                إذا (قىممەت)
                        *قىممەت = عدم;
                إذا (ئاچقۇچ)
                        *ئاچقۇچ = عدم;

                أرجع false;
        }

        e = bucket_at(h, idx);
        data = entry_قىممەت(h, e);
        إذا (قىممەت)
                *قىممەت = data;
        إذا (ئاچقۇچ)
                *ئاچقۇچ = e->ئاچقۇچ;

        أرجع true;
}

#define HASHMAP_FOREACH_IDX(idx, h, i) \
        لكل ((i) = ITERATOR_FIRST, (idx) = hashmap_iterate_entry((h), &(i)); \
             (idx != IDX_NIL); \
             (idx) = hashmap_iterate_entry((h), &(i)))

IteratedCache* _hashmap_iterated_cache_new(HashmapBase *h) {
        IteratedCache *cache;

        أكد(h);
        أكد(!h->cached);

        إذا (h->cached)
                أرجع عدم;

        cache = new0(IteratedCache, 1);
        إذا (!cache)
                أرجع عدم;

        cache->hashmap = h;
        h->cached = true;

        أرجع cache;
}

ساكن فراغ reset_direct_storage(HashmapBase *h) {
        ثابت بنية hashmap_type_info *hi = &hashmap_type_info[h->type];
        فراغ *p;

        أكد(!h->has_indirect);

        p = mempset(h->direct.storage, 0, hi->entry_size * hi->n_direct_buckets);
        memset(p, DIB_RAW_INIT, حجم(dib_raw_t) * hi->n_direct_buckets);
}

ساكن فراغ shared_hash_ئاچقۇچ_initialize(فراغ) {
        random_bytes(shared_hash_ئاچقۇچ, حجم(shared_hash_ئاچقۇچ));
}

ساكن بنية HashmapBase* hashmap_base_new(ثابت بنية hash_ops *hash_ops, تعداد HashmapType type) {
        HashmapBase *h;
        ثابت بنية hashmap_type_info *hi = &hashmap_type_info[type];

        bool use_pool = mempool_enabled && mempool_enabled();  /* mempool_enabled is a weak symbol */

        h = use_pool ? mempool_alloc0_tile(hi->mempool) : malloc0(hi->head_size);
        إذا (!h)
                أرجع عدم;

        h->type = type;
        h->from_pool = use_pool;
        h->hash_ops = hash_ops ?: &trivial_hash_ops;

        إذا (type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*)h;
                lh->iterate_list_head = lh->iterate_list_tail = IDX_NIL;
        }

        reset_direct_storage(h);

        ساكن pthread_once_t once = PTHREAD_ONCE_INIT;
        assert_se(pthread_once(&once, shared_hash_ئاچقۇچ_initialize) == 0);

#إذا ENABLE_DEBUG_HASHMAP
        assert_se(pthread_mutex_lock(&hashmap_debug_list_mutex) == 0);
        LIST_PREPEND(debug_list, hashmap_debug_list, &h->debug);
        assert_se(pthread_mutex_unlock(&hashmap_debug_list_mutex) == 0);
#endif

        أرجع h;
}

Hashmap *hashmap_new(ثابت بنية hash_ops *hash_ops) {
        أرجع (Hashmap*)        hashmap_base_new(hash_ops, HASHMAP_TYPE_PLAIN);
}

OrderedHashmap *ordered_hashmap_new(ثابت بنية hash_ops *hash_ops) {
        أرجع (OrderedHashmap*) hashmap_base_new(hash_ops, HASHMAP_TYPE_ORDERED);
}

Set *set_new(ثابت بنية hash_ops *hash_ops) {
        أرجع (Set*)            hashmap_base_new(hash_ops, HASHMAP_TYPE_SET);
}

ساكن صحيح hashmap_base_ensure_allocated(HashmapBase **h, ثابت بنية hash_ops *hash_ops,
                                         تعداد HashmapType type) {
        HashmapBase *q;

        أكد(h);

        إذا (*h) {
                أكد((*h)->hash_ops == (hash_ops ?: &trivial_hash_ops));
                أرجع 0;
        }

        q = hashmap_base_new(hash_ops, type);
        إذا (!q)
                أرجع -ENOMEM;

        *h = q;
        أرجع 1;
}

صحيح hashmap_ensure_allocated(Hashmap **h, ثابت بنية hash_ops *hash_ops) {
        أرجع hashmap_base_ensure_allocated((HashmapBase**)h, hash_ops, HASHMAP_TYPE_PLAIN);
}

صحيح ordered_hashmap_ensure_allocated(OrderedHashmap **h, ثابت بنية hash_ops *hash_ops) {
        أرجع hashmap_base_ensure_allocated((HashmapBase**)h, hash_ops, HASHMAP_TYPE_ORDERED);
}

صحيح set_ensure_allocated(Set **s, ثابت بنية hash_ops *hash_ops) {
        أرجع hashmap_base_ensure_allocated((HashmapBase**)s, hash_ops, HASHMAP_TYPE_SET);
}

صحيح hashmap_ensure_put(Hashmap **h, ثابت بنية hash_ops *hash_ops, ثابت فراغ *ئاچقۇچ, فراغ *قىممەت) {
        صحيح r;

        أكد(h);

        r = hashmap_ensure_allocated(h, hash_ops);
        إذا (r < 0)
                أرجع r;

        أرجع hashmap_put(*h, ئاچقۇچ, قىممەت);
}

صحيح ordered_hashmap_ensure_put(OrderedHashmap **h, ثابت بنية hash_ops *hash_ops, ثابت فراغ *ئاچقۇچ, فراغ *قىممەت) {
        صحيح r;

        أكد(h);

        r = ordered_hashmap_ensure_allocated(h, hash_ops);
        إذا (r < 0)
                أرجع r;

        أرجع ordered_hashmap_put(*h, ئاچقۇچ, قىممەت);
}

صحيح ordered_hashmap_ensure_replace(OrderedHashmap **h, ثابت بنية hash_ops *hash_ops, ثابت فراغ *ئاچقۇچ, فراغ *قىممەت) {
        صحيح r;

        أكد(h);

        r = ordered_hashmap_ensure_allocated(h, hash_ops);
        إذا (r < 0)
                أرجع r;

        أرجع ordered_hashmap_replace(*h, ئاچقۇچ, قىممەت);
}

صحيح hashmap_ensure_replace(Hashmap **h, ثابت بنية hash_ops *hash_ops, ثابت فراغ *ئاچقۇچ, فراغ *قىممەت) {
        صحيح r;

        أكد(h);

        r = hashmap_ensure_allocated(h, hash_ops);
        إذا (r < 0)
                أرجع r;

        أرجع hashmap_replace(*h, ئاچقۇچ, قىممەت);
}

ساكن فراغ hashmap_free_no_clear(HashmapBase *h) {
        أكد(!h->has_indirect);
        أكد(h->n_direct_entries == 0);

#إذا ENABLE_DEBUG_HASHMAP
        assert_se(pthread_mutex_lock(&hashmap_debug_list_mutex) == 0);
        LIST_REMOVE(debug_list, hashmap_debug_list, &h->debug);
        assert_se(pthread_mutex_unlock(&hashmap_debug_list_mutex) == 0);
#endif

        إذا (h->from_pool) {
                /* Ensure that the object didn't get migrated between threads. */
                assert_se(is_main_thread());
                mempool_free_tile(hashmap_type_info[h->type].mempool, h);
        } وإلا
                حرر(h);
}

HashmapBase* _hashmap_free(HashmapBase *h) {
        إذا (h) {
                _hashmap_clear(h);
                hashmap_free_no_clear(h);
        }

        أرجع عدم;
}

فراغ _hashmap_clear(HashmapBase *h) {
        إذا (!h)
                أرجع;

        إذا (h->hash_ops->free_ئاچقۇچ || h->hash_ops->free_قىممەت) {

                /* If destructor calls are defined, let's destroy things defensively: let's take the item out of the
                 * hash table, and only then call the destructor functions. If these destructors then try to unregister
                 * themselves from our hash table a second time, the entry is already gone. */

                طالما (_hashmap_size(h) > 0) {
                        فراغ *k = عدم;
                        فراغ *v;

                        v = hashmap_first_ئاچقۇچ_and_قىممەت(h, true, &k);

                        إذا (h->hash_ops->free_ئاچقۇچ)
                                h->hash_ops->free_ئاچقۇچ(k);

                        إذا (h->hash_ops->free_قىممەت)
                                h->hash_ops->free_قىممەت(v);
                }
        }

        إذا (h->has_indirect) {
                حرر(h->indirect.storage);
                h->has_indirect = false;
        }

        h->n_direct_entries = 0;
        reset_direct_storage(h);

        إذا (h->type == HASHMAP_TYPE_ORDERED) {
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

#إذا ENABLE_DEBUG_HASHMAP
        أكد(h);
        h->debug.put_سانى++;
#endif

        dibs = dib_raw_ptr(h);

        لكل (distance = 0; ; distance++) {
                raw_dib = dibs[idx];
                إذا (IN_SET(raw_dib, DIB_RAW_FREE, DIB_RAW_REHASH)) {
                        إذا (raw_dib == DIB_RAW_REHASH)
                                bucket_move_entry(h, swap, idx, IDX_TMP);

                        إذا (h->has_indirect && h->indirect.idx_lowest_entry > idx)
                                h->indirect.idx_lowest_entry = idx;

                        bucket_set_dib(h, idx, distance);
                        bucket_move_entry(h, swap, IDX_PUT, idx);
                        إذا (raw_dib == DIB_RAW_REHASH) {
                                bucket_move_entry(h, swap, IDX_TMP, IDX_PUT);
                                أرجع true;
                        }

                        أرجع false;
                }

                dib = bucket_calculate_dib(h, idx, raw_dib);

                إذا (dib < distance) {
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

        إذا (may_resize) {
                r = resize_buckets(h, 1);
                إذا (r < 0)
                        أرجع r;
                إذا (r > 0)
                        idx = bucket_hash(h, new_entry->p.b.ئاچقۇچ);
        }
        أكد(n_entries(h) < n_buckets(h));

        إذا (h->type == HASHMAP_TYPE_ORDERED) {
                OrderedHashmap *lh = (OrderedHashmap*) h;

                new_entry->iterate_next = IDX_NIL;
                new_entry->iterate_previous = lh->iterate_list_tail;

                إذا (lh->iterate_list_tail != IDX_NIL) {
                        بنية ordered_hashmap_entry *old_tail;

                        old_tail = ordered_bucket_at(lh, lh->iterate_list_tail);
                        أكد(old_tail->iterate_next == IDX_NIL);
                        old_tail->iterate_next = IDX_PUT;
                }

                lh->iterate_list_tail = IDX_PUT;
                إذا (lh->iterate_list_head == IDX_NIL)
                        lh->iterate_list_head = IDX_PUT;
        }

        assert_se(hashmap_put_robin_hood(h, idx, swap) == false);

        n_entries_inc(h);
#إذا ENABLE_DEBUG_HASHMAP
        h->debug.max_entries = MAX(h->debug.max_entries, n_entries(h));
#endif

        base_set_dirty(h);

        أرجع 1;
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
        إذا (_unlikely_(new_n_entries < entries_add))
                أرجع -ENOMEM;

        /* For direct storage we allow 100% load, because it's tiny. */
        إذا (!h->has_indirect && new_n_entries <= hi->n_direct_buckets)
                أرجع 0;

        /*
         * Load factor = n/m = 1 - (1/INV_KEEP_FREE).
         * From it follows: m = n + n/(INV_KEEP_FREE - 1)
         */
        new_n_buckets = new_n_entries + new_n_entries / (INV_KEEP_FREE - 1);
        /* overflow? */
        إذا (_unlikely_(new_n_buckets < new_n_entries))
                أرجع -ENOMEM;

        old_n_buckets = n_buckets(h);

        إذا (_likely_(new_n_buckets <= old_n_buckets))
                أرجع 0;

        إذا (_unlikely_(new_n_buckets > UINT_MAX / (hi->entry_size + حجم(dib_raw_t))))
                أرجع -ENOMEM;

        new_shift = log2u_round_up(MAX(
                        new_n_buckets * (hi->entry_size + حجم(dib_raw_t)),
                        2 * حجم(بنية direct_storage)));

        /* Realloc storage (buckets and DIB array). */
        new_storage = realloc(h->has_indirect ? h->indirect.storage : عدم,
                              1U << new_shift);
        إذا (!new_storage)
                أرجع -ENOMEM;

        /* Must upgrade direct to indirect storage. */
        إذا (!h->has_indirect) {
                memcpy(new_storage, h->direct.storage,
                       old_n_buckets * (hi->entry_size + حجم(dib_raw_t)));
                h->indirect.n_entries = h->n_direct_entries;
                h->indirect.idx_lowest_entry = 0;
                h->n_direct_entries = 0;
        }

        /* Get a new hash key. If we've just upgraded to indirect storage,
         * allow reusing a previously generated key. It's still a different key
         * from the shared one that we used for direct storage. */
        get_hash_ئاچقۇچ(h->indirect.hash_ئاچقۇچ, !h->has_indirect);

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
                إذا (new_dibs[idx] != DIB_RAW_REHASH)
                        استمر;

                optimal_idx = bucket_hash(h, bucket_at(h, idx)->ئاچقۇچ);

                /*
                 * Not much to do if by luck the entry hashes to its current
                 * location. Just set its DIB.
                 */
                إذا (optimal_idx == idx) {
                        new_dibs[idx] = 0;
                        n_rehashed++;
                        استمر;
                }

                new_dibs[idx] = DIB_RAW_FREE;
                bucket_move_entry(h, &swap, idx, IDX_PUT);
                /* bucket_move_entry does not clear the source */
                memzero(bucket_at(h, idx), hi->entry_size);

                افعل {
                        /*
                         * Find the new bucket for the current entry. This may make
                         * another entry homeless and load it into IDX_PUT.
                         */
                        rehash_next = hashmap_put_robin_hood(h, optimal_idx, &swap);
                        n_rehashed++;

                        /* Did the current entry displace another one? */
                        إذا (rehash_next)
                                optimal_idx = bucket_hash(h, bucket_at_swap(&swap, IDX_PUT)->p.b.ئاچقۇچ);
                } طالما (rehash_next);
        }

        assert_se(n_rehashed == n_entries(h));

        أرجع 1;
}

/*
 * Finds an entry with a matching key
 * Returns: index of the found entry, or IDX_NIL if not found.
 */
ساكن unsigned base_bucket_scan(HashmapBase *h, unsigned idx, ثابت فراغ *ئاچقۇچ) {
        compare_func_t compare = h->hash_ops->compare;
        بنية hashmap_base_entry *e;
        unsigned dib, distance;
        dib_raw_t *dibs = dib_raw_ptr(h);

        أكد(idx < n_buckets(h));

        لكل (distance = 0; ; distance++) {
                إذا (dibs[idx] == DIB_RAW_FREE)
                        أرجع IDX_NIL;

                dib = bucket_calculate_dib(h, idx, dibs[idx]);

                إذا (dib < distance)
                        أرجع IDX_NIL;
                إذا (dib == distance) {
                        e = bucket_at(h, idx);

                        /* Most maps compare keys by pointer. Do that here, without the indirect call. */
                        إذا (compare == trivial_compare_func ? e->ئاچقۇچ == ئاچقۇچ : compare(e->ئاچقۇچ, ئاچقۇچ) == 0)
                                أرجع idx;
                }

                idx = next_idx(h, idx);
        }
}
#define bucket_scan(h, idx, ئاچقۇچ) base_bucket_scan(HASHMAP_BASE(h), idx, ئاچقۇچ)

صحيح hashmap_put(Hashmap *h, ثابت فراغ *ئاچقۇچ, فراغ *قىممەت) {
        بنية swap_entries swap;
        بنية plain_hashmap_entry *e;
        unsigned hash, idx;

        أكد(h);

        hash = bucket_hash(h, ئاچقۇچ);
        idx = bucket_scan(h, hash, ئاچقۇچ);
        إذا (idx != IDX_NIL) {
                e = plain_bucket_at(h, idx);
                إذا (e->قىممەت == قىممەت)
                        أرجع 0;
                أرجع -EEXIST;
        }

        e = &bucket_at_swap(&swap, IDX_PUT)->p;
        e->b.ئاچقۇچ = ئاچقۇچ;
        e->قىممەت = قىممەت;
        أرجع hashmap_put_boldly(h, hash, &swap, true);
}

صحيح set_put(Set *s, ثابت فراغ *ئاچقۇچ) {
        بنية swap_entries swap;
        بنية hashmap_base_entry *e;
        unsigned hash, idx;

        أكد(s);

        hash = bucket_hash(s, ئاچقۇچ);
        idx = bucket_scan(s, hash, ئاچقۇچ);
        إذا (idx != IDX_NIL)
                أرجع 0;

        e = &bucket_at_swap(&swap, IDX_PUT)->p.b;
        e->ئاچقۇچ = ئاچقۇچ;
        أرجع hashmap_put_boldly(s, hash, &swap, true);
}

صحيح set_ensure_put(Set **s, ثابت بنية hash_ops *hash_ops, ثابت فراغ *ئاچقۇچ) {
        صحيح r;

        أكد(s);

        r = set_ensure_allocated(s, hash_ops);
        إذا (r < 0)
                أرجع r;

        أرجع set_put(*s, ئاچقۇچ);
}

صحيح set_ensure_consume(Set **s, ثابت بنية hash_ops *hash_ops, فراغ *ئاچقۇچ) {
        صحيح r;

        r = set_ensure_put(s, hash_ops, ئاچقۇچ);
        إذا (r <= 0) {
                إذا (hash_ops && hash_ops->free_ئاچقۇچ)
                        hash_ops->free_ئاچقۇچ(ئاچقۇچ);
                وإلا إذا (hash_ops && hash_ops->free_قىممەت)
                        /* Sets store their element in the key slot but may carry a value destructor. */
                        hash_ops->free_قىممەت(ئاچقۇچ);
                وإلا
                        حرر(ئاچقۇچ);
        }

        أرجع r;
}

صحيح hashmap_replace(Hashmap *h, ثابت فراغ *ئاچقۇچ, فراغ *قىممەت) {
        بنية swap_entries swap;
        بنية plain_hashmap_entry *e;
        unsigned hash, idx;

        أكد(h);

        hash = bucket_hash(h, ئاچقۇچ);
        idx = bucket_scan(h, hash, ئاچقۇچ);
        إذا (idx != IDX_NIL) {
                e = plain_bucket_at(h, idx);
#إذا ENABLE_DEBUG_HASHMAP
                /* Although the key is equal, the key pointer may have changed,
                 * and this would break our assumption for iterating. So count
                 * this operation as incompatible with iteration. */
                إذا (e->b.ئاچقۇچ != ئاچقۇچ) {
                        h->b.debug.put_سانى++;
                        h->b.debug.rem_سانى++;
                        h->b.debug.last_rem_idx = idx;
                }
#endif
                e->b.ئاچقۇچ = ئاچقۇچ;
                e->قىممەت = قىممەت;
                hashmap_set_dirty(h);

                أرجع 0;
        }

        e = &bucket_at_swap(&swap, IDX_PUT)->p;
        e->b.ئاچقۇچ = ئاچقۇچ;
        e->قىممەت = قىممەت;
        أرجع hashmap_put_boldly(h, hash, &swap, true);
}

صحيح hashmap_update(Hashmap *h, ثابت فراغ *ئاچقۇچ, فراغ *قىممەت) {
        بنية plain_hashmap_entry *e;
        unsigned hash, idx;

        أكد(h);

        hash = bucket_hash(h, ئاچقۇچ);
        idx = bucket_scan(h, hash, ئاچقۇچ);
        إذا (idx == IDX_NIL)
                أرجع -ENOENT;

        e = plain_bucket_at(h, idx);
        e->قىممەت = قىممەت;
        hashmap_set_dirty(h);

        أرجع 0;
}

فراغ* _hashmap_get(HashmapBase *h, ثابت فراغ *ئاچقۇچ) {
        بنية hashmap_base_entry *e;
        unsigned hash, idx;

        إذا (!h)
                أرجع عدم;

        hash = bucket_hash(h, ئاچقۇچ);
        idx = bucket_scan(h, hash, ئاچقۇچ);
        إذا (idx == IDX_NIL)
                أرجع عدم;

        e = bucket_at(h, idx);
        أرجع entry_قىممەت(h, e);
}

فراغ* hashmap_get2(Hashmap *h, ثابت فراغ *ئاچقۇچ, فراغ **ret) {
        بنية plain_hashmap_entry *e;
        unsigned hash, idx;

        إذا (!h)
                أرجع عدم;

        hash = bucket_hash(h, ئاچقۇچ);
        idx = bucket_scan(h, hash, ئاچقۇچ);
        إذا (idx == IDX_NIL)
                أرجع عدم;

        e = plain_bucket_at(h, idx);
        إذا (ret)
                *ret = (فراغ*) e->b.ئاچقۇچ;

        أرجع e->قىممەت;
}

bool _hashmap_contains(HashmapBase *h, ثابت فراغ *ئاچقۇچ) {
        unsigned hash;

        إذا (!h)
                أرجع false;

        hash = bucket_hash(h, ئاچقۇچ);
        أرجع bucket_scan(h, hash, ئاچقۇچ) != IDX_NIL;
}

فراغ* _hashmap_remove(HashmapBase *h, ثابت فراغ *ئاچقۇچ) {
        بنية hashmap_base_entry *e;
        unsigned hash, idx;
        فراغ *data;

        إذا (!h)
                أرجع عدم;

        hash = bucket_hash(h, ئاچقۇچ);
        idx = bucket_scan(h, hash, ئاچقۇچ);
        إذا (idx == IDX_NIL)
                أرجع عدم;

        e = bucket_at(h, idx);
        data = entry_قىممەت(h, e);
        remove_entry(h, idx);

        أرجع data;
}

فراغ* hashmap_remove2(Hashmap *h, ثابت فراغ *ئاچقۇچ, فراغ **ret) {
        بنية plain_hashmap_entry *e;
        unsigned hash, idx;
        فراغ *data;

        إذا (!h) {
                إذا (ret)
                        *ret = عدم;
                أرجع عدم;
        }

        hash = bucket_hash(h, ئاچقۇچ);
        idx = bucket_scan(h, hash, ئاچقۇچ);
        إذا (idx == IDX_NIL) {
                إذا (ret)
                        *ret = عدم;
                أرجع عدم;
        }

        e = plain_bucket_at(h, idx);
        data = e->قىممەت;
        إذا (ret)
                *ret = (فراغ*) e->b.ئاچقۇچ;

        remove_entry(h, idx);

        أرجع data;
}

صحيح hashmap_remove_and_put(Hashmap *h, ثابت فراغ *old_ئاچقۇچ, ثابت فراغ *new_ئاچقۇچ, فراغ *قىممەت) {
        بنية swap_entries swap;
        بنية plain_hashmap_entry *e;
        unsigned old_hash, new_hash, idx;

        إذا (!h)
                أرجع -ENOENT;

        old_hash = bucket_hash(h, old_ئاچقۇچ);
        idx = bucket_scan(h, old_hash, old_ئاچقۇچ);
        إذا (idx == IDX_NIL)
                أرجع -ENOENT;

        new_hash = bucket_hash(h, new_ئاچقۇچ);
        إذا (bucket_scan(h, new_hash, new_ئاچقۇچ) != IDX_NIL)
                أرجع -EEXIST;

        remove_entry(h, idx);

        e = &bucket_at_swap(&swap, IDX_PUT)->p;
        e->b.ئاچقۇچ = new_ئاچقۇچ;
        e->قىممەت = قىممەت;
        assert_se(hashmap_put_boldly(h, new_hash, &swap, false) == 1);

        أرجع 0;
}

صحيح set_remove_and_put(Set *s, ثابت فراغ *old_ئاچقۇچ, ثابت فراغ *new_ئاچقۇچ) {
        بنية swap_entries swap;
        بنية hashmap_base_entry *e;
        unsigned old_hash, new_hash, idx;

        إذا (!s)
                أرجع -ENOENT;

        old_hash = bucket_hash(s, old_ئاچقۇچ);
        idx = bucket_scan(s, old_hash, old_ئاچقۇچ);
        إذا (idx == IDX_NIL)
                أرجع -ENOENT;

        new_hash = bucket_hash(s, new_ئاچقۇچ);
        إذا (bucket_scan(s, new_hash, new_ئاچقۇچ) != IDX_NIL)
                أرجع -EEXIST;

        remove_entry(s, idx);

        e = &bucket_at_swap(&swap, IDX_PUT)->p.b;
        e->ئاچقۇچ = new_ئاچقۇچ;
        assert_se(hashmap_put_boldly(s, new_hash, &swap, false) == 1);

        أرجع 0;
}

صحيح hashmap_remove_and_replace(Hashmap *h, ثابت فراغ *old_ئاچقۇچ, ثابت فراغ *new_ئاچقۇچ, فراغ *قىممەت) {
        بنية swap_entries swap;
        بنية plain_hashmap_entry *e;
        unsigned old_hash, new_hash, idx_old, idx_new;

        إذا (!h)
                أرجع -ENOENT;

        old_hash = bucket_hash(h, old_ئاچقۇچ);
        idx_old = bucket_scan(h, old_hash, old_ئاچقۇچ);
        إذا (idx_old == IDX_NIL)
                أرجع -ENOENT;

        old_ئاچقۇچ = bucket_at(HASHMAP_BASE(h), idx_old)->ئاچقۇچ;

        new_hash = bucket_hash(h, new_ئاچقۇچ);
        idx_new = bucket_scan(h, new_hash, new_ئاچقۇچ);
        إذا (idx_new != IDX_NIL)
                إذا (idx_old != idx_new) {
                        remove_entry(h, idx_new);
                        /* Compensate for a possible backward shift. */
                        إذا (old_ئاچقۇچ != bucket_at(HASHMAP_BASE(h), idx_old)->ئاچقۇچ)
                                idx_old = prev_idx(HASHMAP_BASE(h), idx_old);
                        أكد(old_ئاچقۇچ == bucket_at(HASHMAP_BASE(h), idx_old)->ئاچقۇچ);
                }

        remove_entry(h, idx_old);

        e = &bucket_at_swap(&swap, IDX_PUT)->p;
        e->b.ئاچقۇچ = new_ئاچقۇچ;
        e->قىممەت = قىممەت;
        assert_se(hashmap_put_boldly(h, new_hash, &swap, false) == 1);

        أرجع 0;
}

فراغ* hashmap_remove_قىممەت(HashmapBase *h, ثابت فراغ *ئاچقۇچ, فراغ *قىممەت) {
        بنية hashmap_base_entry *e;
        unsigned hash, idx;

        إذا (!h)
                أرجع عدم;

        hash = bucket_hash(h, ئاچقۇچ);
        idx = bucket_scan(h, hash, ئاچقۇچ);
        إذا (idx == IDX_NIL)
                أرجع عدم;

        e = bucket_at(h, idx);
        إذا (entry_قىممەت(h, e) != قىممەت)
                أرجع عدم;

        remove_entry(h, idx);

        أرجع قىممەت;
}

ساكن unsigned find_first_entry(HashmapBase *h) {
        Iterator i = ITERATOR_FIRST;

        إذا (!h || !n_entries(h))
                أرجع IDX_NIL;

        أرجع hashmap_iterate_entry(h, &i);
}

فراغ* hashmap_first_ئاچقۇچ_and_قىممەت(HashmapBase *h, bool remove, فراغ **ret_ئاچقۇچ) {
        بنية hashmap_base_entry *e;
        فراغ *ئاچقۇچ, *data;
        unsigned idx;

        idx = find_first_entry(h);
        إذا (idx == IDX_NIL) {
                إذا (ret_ئاچقۇچ)
                        *ret_ئاچقۇچ = عدم;
                أرجع عدم;
        }

        e = bucket_at(h, idx);
        ئاچقۇچ = (فراغ*) e->ئاچقۇچ;
        data = entry_قىممەت(h, e);

        إذا (remove)
                remove_entry(h, idx);

        إذا (ret_ئاچقۇچ)
                *ret_ئاچقۇچ = ئاچقۇچ;

        أرجع data;
}

unsigned _hashmap_size(HashmapBase *h) {
        إذا (!h)
                أرجع 0;

        أرجع n_entries(h);
}

unsigned _hashmap_buckets(HashmapBase *h) {
        إذا (!h)
                أرجع 0;

        أرجع n_buckets(h);
}

صحيح _hashmap_merge(Hashmap *h, Hashmap *other) {
        Iterator i;
        unsigned idx;

        أكد(h);

        HASHMAP_FOREACH_IDX(idx, HASHMAP_BASE(other), i) {
                بنية plain_hashmap_entry *pe = plain_bucket_at(other, idx);
                صحيح r;

                r = hashmap_put(h, pe->b.ئاچقۇچ, pe->قىممەت);
                إذا (r < 0 && r != -EEXIST)
                        أرجع r;
        }

        أرجع 0;
}

صحيح set_merge(Set *s, Set *other) {
        Iterator i;
        unsigned idx;

        أكد(s);

        HASHMAP_FOREACH_IDX(idx, HASHMAP_BASE(other), i) {
                بنية set_entry *se = set_bucket_at(other, idx);
                صحيح r;

                r = set_put(s, se->b.ئاچقۇچ);
                إذا (r < 0)
                        أرجع r;
        }

        أرجع 0;
}

صحيح _hashmap_reserve(HashmapBase *h, unsigned entries_add) {
        صحيح r;

        أكد(h);

        r = resize_buckets(h, entries_add);
        إذا (r < 0)
                أرجع r;

        أرجع 0;
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

        إذا (!other)
                أرجع 0;

        أكد(other->type == h->type);

        /*
         * This reserves buckets for the worst case, where none of other's
         * entries are yet present in h. This is preferable to risking
         * an allocation failure in the middle of the moving and having to
         * rollback or return a partial result.
         */
        r = resize_buckets(h, n_entries(other));
        إذا (r < 0)
                أرجع r;

        HASHMAP_FOREACH_IDX(idx, other, i) {
                unsigned h_hash;

                e = bucket_at(other, idx);
                h_hash = bucket_hash(h, e->ئاچقۇچ);
                إذا (bucket_scan(h, h_hash, e->ئاچقۇچ) != IDX_NIL)
                        استمر;

                n = &bucket_at_swap(&swap, IDX_PUT)->p.b;
                n->ئاچقۇچ = e->ئاچقۇچ;
                إذا (h->type != HASHMAP_TYPE_SET)
                        ((بنية plain_hashmap_entry*) n)->قىممەت =
                                ((بنية plain_hashmap_entry*) e)->قىممەت;
                assert_se(hashmap_put_boldly(h, h_hash, &swap, false) == 1);

                remove_entry(other, idx);
        }

        أرجع 0;
}

صحيح _hashmap_move_one(HashmapBase *h, HashmapBase *other, ثابت فراغ *ئاچقۇچ) {
        بنية swap_entries swap;
        unsigned h_hash, other_hash, idx;
        بنية hashmap_base_entry *e, *n;
        صحيح r;

        أكد(h);

        h_hash = bucket_hash(h, ئاچقۇچ);
        إذا (bucket_scan(h, h_hash, ئاچقۇچ) != IDX_NIL)
                أرجع -EEXIST;

        إذا (!other)
                أرجع -ENOENT;

        أكد(other->type == h->type);

        other_hash = bucket_hash(other, ئاچقۇچ);
        idx = bucket_scan(other, other_hash, ئاچقۇچ);
        إذا (idx == IDX_NIL)
                أرجع -ENOENT;

        e = bucket_at(other, idx);

        n = &bucket_at_swap(&swap, IDX_PUT)->p.b;
        n->ئاچقۇچ = e->ئاچقۇچ;
        إذا (h->type != HASHMAP_TYPE_SET)
                ((بنية plain_hashmap_entry*) n)->قىممەت =
                        ((بنية plain_hashmap_entry*) e)->قىممەت;
        r = hashmap_put_boldly(h, h_hash, &swap, true);
        إذا (r < 0)
                أرجع r;

        remove_entry(other, idx);
        أرجع 0;
}

HashmapBase* _hashmap_copy(HashmapBase *h) {
        HashmapBase *copy;
        صحيح r;

        أكد(h);

        copy = hashmap_base_new(h->hash_ops, h->type);
        إذا (!copy)
                أرجع عدم;

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

        إذا (r < 0)
                أرجع _hashmap_free(copy);

        أرجع copy;
}

حرف** _hashmap_get_strv(HashmapBase *h) {
        حرف **sv;
        Iterator i;
        unsigned idx, n;

        إذا (!h)
                أرجع new0(حرف*, 1);

        sv = new(حرف*, n_entries(h)+1);
        إذا (!sv)
                أرجع عدم;

        n = 0;
        HASHMAP_FOREACH_IDX(idx, h, i)
                sv[n++] = entry_قىممەت(h, bucket_at(h, idx));
        sv[n] = عدم;

        أرجع sv;
}

حرف** set_to_strv(Set **s) {
        أكد(s);

        /* This is similar to set_get_strv(), but invalidates the set on success. */

        حرف **v = new(حرف*, set_size(*s) + 1);
        إذا (!v)
                أرجع عدم;

        لكل (حرف **p = v; (*p = set_steal_first(*s)); p++)
                ;

        أكد(set_isempty(*s));
        *s = set_free(*s);
        أرجع v;
}

فراغ* ordered_hashmap_next(OrderedHashmap *h, ثابت فراغ *ئاچقۇچ) {
        بنية ordered_hashmap_entry *e;
        unsigned hash, idx;

        إذا (!h)
                أرجع عدم;

        hash = bucket_hash(h, ئاچقۇچ);
        idx = bucket_scan(h, hash, ئاچقۇچ);
        إذا (idx == IDX_NIL)
                أرجع عدم;

        e = ordered_bucket_at(h, idx);
        إذا (e->iterate_next == IDX_NIL)
                أرجع عدم;
        أرجع ordered_bucket_at(h, e->iterate_next)->p.قىممەت;
}

صحيح set_consume(Set *s, فراغ *قىممەت) {
        صحيح r;

        أكد(s);
        أكد(قىممەت);

        r = set_put(s, قىممەت);
        إذا (r <= 0)
                حرر(قىممەت);

        أرجع r;
}

صحيح hashmap_put_strdup_full(Hashmap **h, ثابت بنية hash_ops *hash_ops, ثابت حرف *k, ثابت حرف *v) {
        صحيح r;

        أكد(h);

        r = hashmap_ensure_allocated(h, hash_ops);
        إذا (r < 0)
                أرجع r;

        _cleanup_free_ حرف *kdup = عدم, *vdup = عدم;

        kdup = strdup(k);
        إذا (!kdup)
                أرجع -ENOMEM;

        إذا (v) {
                vdup = strdup(v);
                إذا (!vdup)
                        أرجع -ENOMEM;
        }

        r = hashmap_put(*h, kdup, vdup);
        إذا (r < 0) {
                إذا (r == -EEXIST && streq_ptr(v, hashmap_get(*h, kdup)))
                        أرجع 0;
                أرجع r;
        }

        /* 0 with non-null vdup would mean vdup is already in the hashmap, which cannot be */
        أكد(vdup == عدم || r > 0);
        إذا (r > 0)
                kdup = vdup = عدم;

        أرجع r;
}

صحيح set_put_strndup_full(Set **s, ثابت بنية hash_ops *hash_ops, ثابت حرف *p, size_t n) {
        حرف *c;
        صحيح r;

        أكد(s);
        أكد(p);

        r = set_ensure_allocated(s, hash_ops);
        إذا (r < 0)
                أرجع r;

        إذا (n == SIZE_MAX) {
                إذا (set_contains(*s, (حرف*) p))
                        أرجع 0;

                c = strdup(p);
        } وإلا
                c = strndup(p, n);
        إذا (!c)
                أرجع -ENOMEM;

        أرجع set_consume(*s, c);
}

صحيح set_put_strdupv_full(Set **s, ثابت بنية hash_ops *hash_ops, حرف **l) {
        صحيح n = 0, r;

        أكد(s);

        STRV_FOREACH(i, l) {
                r = set_put_strndup_full(s, hash_ops, *i, SIZE_MAX);
                إذا (r < 0)
                        أرجع r;

                n += r;
        }

        أرجع n;
}

صحيح set_put_strsplit(Set *s, ثابت حرف *v, ثابت حرف *separators, ExtractFlags flags) {
        ثابت حرف *p = ASSERT_PTR(v);
        صحيح r;

        أكد(s);

        لكل (;;) {
                حرف *word;

                r = extract_first_word(&p, &word, separators, flags);
                إذا (r <= 0)
                        أرجع r;

                r = set_consume(s, word);
                إذا (r < 0)
                        أرجع r;
        }
}

/* expand the cachemem if needed, return true if newly (re)activated. */
ساكن صحيح cachemem_maintain(CacheMem *mem, size_t size) {
        أكد(mem);

        إذا (!GREEDY_REALLOC(mem->ptr, size)) {
                إذا (size > 0)
                        أرجع -ENOMEM;
        }

        إذا (!mem->active) {
                mem->active = true;
                أرجع true;
        }

        أرجع false;
}

صحيح iterated_cache_get(IteratedCache *cache, ثابت فراغ ***res_ئاچقۇچلار, ثابت فراغ ***res_قىممەتلەر, unsigned *res_n_entries) {
        bool sync_ئاچقۇچلار = false, sync_قىممەتلەر = false;
        size_t size;
        صحيح r;

        أكد(cache);
        أكد(cache->hashmap);

        size = n_entries(cache->hashmap);

        إذا (res_ئاچقۇچلار) {
                r = cachemem_maintain(&cache->ئاچقۇچلار, size);
                إذا (r < 0)
                        أرجع r;

                sync_ئاچقۇچلار = r;
        } وإلا
                cache->ئاچقۇچلار.active = false;

        إذا (res_قىممەتلەر) {
                r = cachemem_maintain(&cache->values, size);
                إذا (r < 0)
                        أرجع r;

                sync_قىممەتلەر = r;
        } وإلا
                cache->values.active = false;

        إذا (cache->hashmap->dirty) {
                إذا (cache->ئاچقۇچلار.active)
                        sync_ئاچقۇچلار = true;
                إذا (cache->values.active)
                        sync_قىممەتلەر = true;

                cache->hashmap->dirty = false;
        }

        إذا (sync_ئاچقۇچلار || sync_قىممەتلەر) {
                unsigned i, idx;
                Iterator iter;

                i = 0;
                HASHMAP_FOREACH_IDX(idx, cache->hashmap, iter) {
                        بنية hashmap_base_entry *e;

                        e = bucket_at(cache->hashmap, idx);

                        إذا (sync_ئاچقۇچلار)
                                cache->ئاچقۇچلار.ptr[i] = e->ئاچقۇچ;
                        إذا (sync_قىممەتلەر)
                                cache->values.ptr[i] = entry_قىممەت(cache->hashmap, e);
                        i++;
                }
        }

        إذا (res_ئاچقۇچلار)
                *res_ئاچقۇچلار = cache->ئاچقۇچلار.ptr;
        إذا (res_قىممەتلەر)
                *res_قىممەتلەر = cache->values.ptr;
        إذا (res_n_entries)
                *res_n_entries = size;

        أرجع 0;
}

IteratedCache* iterated_cache_free(IteratedCache *cache) {
        إذا (cache) {
                حرر(cache->ئاچقۇچلار.ptr);
                حرر(cache->values.ptr);
        }

        أرجع mfree(cache);
}

صحيح set_strjoin(Set *s, ثابت حرف *separator, bool wrap_with_separator, حرف **ret) {
        _cleanup_free_ حرف *str = عدم;
        size_t separator_len, len = 0;
        ثابت حرف *قىممەت;
        bool first;

        أكد(ret);

        إذا (set_isempty(s)) {
                *ret = عدم;
                أرجع 0;
        }

        separator_len = strlen_ptr(separator);

        إذا (separator_len == 0)
                wrap_with_separator = false;

        first = !wrap_with_separator;

        SET_FOREACH(قىممەت, s) {
                size_t l = strlen_ptr(قىممەت);

                إذا (l == 0)
                        استمر;

                إذا (!GREEDY_REALLOC(str, len + l + (first ? 0 : separator_len) + (wrap_with_separator ? separator_len : 0) + 1))
                        أرجع -ENOMEM;

                إذا (separator_len > 0 && !first) {
                        memcpy(str + len, separator, separator_len);
                        len += separator_len;
                }

                memcpy(str + len, قىممەت, l);
                len += l;
                first = false;
        }

        إذا (wrap_with_separator) {
                memcpy(str + len, separator, separator_len);
                len += separator_len;
        }

        str[len] = '\0';

        *ret = TAKE_PTR(str);
        أرجع 0;
}

bool set_equal(Set *a, Set *b) {
        فراغ *p;

        /* Checks whether each entry of 'a' is also in 'b' and vice versa, i.e. the two sets contain the same
         * entries */

        إذا (a == b)
                أرجع true;

        إذا (set_isempty(a) && set_isempty(b))
                أرجع true;

        إذا (set_size(a) != set_size(b)) /* Cheap check that hopefully catches a lot of inequality cases
                                         * already */
                أرجع false;

        SET_FOREACH(p, a)
                إذا (!set_contains(b, p))
                        أرجع false;

        /* If we have the same hashops, then we don't need to check things backwards given we compared the
         * size and that all of a is in b. */
        إذا (a->b.hash_ops == b->b.hash_ops)
                أرجع true;

        SET_FOREACH(p, b)
                إذا (!set_contains(a, p))
                        أرجع false;

        أرجع true;
}

ساكن bool set_fnmatch_one(Set *patterns, ثابت حرف *needle) {
        ثابت حرف *p;

        أكد(needle);

        /* Any failure of fnmatch() is treated as equivalent to FNM_NOMATCH, i.e. as non-matching pattern */

        SET_FOREACH(p, patterns)
                إذا (fnmatch(p, needle, 0) == 0)
                        أرجع true;

        أرجع false;
}

bool set_fnmatch(Set *include_patterns, Set *exclude_patterns, ثابت حرف *needle) {
        أكد(needle);

        إذا (set_fnmatch_one(exclude_patterns, needle))
                أرجع false;

        إذا (set_isempty(include_patterns))
                أرجع true;

        أرجع set_fnmatch_one(include_patterns, needle);
}

ساكن صحيح hashmap_entry_compare(
                بنية hashmap_base_entry * ثابت *a,
                بنية hashmap_base_entry * ثابت *b,
                compare_func_t compare) {

        أكد(a && *a);
        أكد(b && *b);
        أكد(compare);

        أرجع compare((*a)->ئاچقۇچ, (*b)->ئاچقۇچ);
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

        إذا (_hashmap_size(h) == 0) {
                *ret = عدم;
                *ret_n = 0;
                أرجع 0;
        }

        /* We append one more element than needed so that the resulting array can be used as a strv. We
         * don't count this entry in the returned size. */
        entries = new(فراغ*, _hashmap_size(h) + 1);
        إذا (!entries)
                أرجع -ENOMEM;

        HASHMAP_FOREACH_IDX(idx, h, iter)
                entries[n++] = bucket_at(h, idx);

        أكد(n == _hashmap_size(h));
        entries[n] = عدم;

        typesafe_qsort_r((بنية hashmap_base_entry**) entries, n,
                         hashmap_entry_compare, h->hash_ops->compare);

        *ret = TAKE_PTR(entries);
        *ret_n = n;
        أرجع 0;
}

صحيح hashmap_dump_ئاچقۇچلار_sorted(HashmapBase *h, فراغ ***ret, size_t *ret_n) {
        _cleanup_free_ فراغ **entries = عدم;
        size_t n;
        صحيح r;

        أكد(ret);

        r = _hashmap_dump_entries_sorted(h, &entries, &n);
        إذا (r < 0)
                أرجع r;

        /* Reuse the array. */
        FOREACH_ARRAY(e, entries, n)
                *e = (فراغ*) (*(بنية hashmap_base_entry**) e)->ئاچقۇچ;

        *ret = TAKE_PTR(entries);
        إذا (ret_n)
                *ret_n = n;
        أرجع 0;
}

صحيح _hashmap_dump_sorted(HashmapBase *h, فراغ ***ret, size_t *ret_n) {
        _cleanup_free_ فراغ **entries = عدم;
        size_t n;
        صحيح r;

        أكد(ret);

        r = _hashmap_dump_entries_sorted(h, &entries, &n);
        إذا (r < 0)
                أرجع r;

        /* Reuse the array. */
        FOREACH_ARRAY(e, entries, n)
                *e = entry_قىممەت(h, *(بنية hashmap_base_entry**) e);

        *ret = TAKE_PTR(entries);
        إذا (ret_n)
                *ret_n = n;
        أرجع 0;
}
