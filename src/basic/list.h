/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ "forward.h"

/* The head of the linked list. Use this in the structure that shall
 * contain the head of the linked list */
#ವ್ಯಾಖ್ಯಾನಿಸು LIST_ತಲೆ(t,name)                                               \
        t *name

/* The pointers in the linked list's items. Use this in the item structure */
#ವ್ಯಾಖ್ಯಾನಿಸು LIST_FIELDS(t,name)                                             \
        t *name##_next, *name##_prev

/* Initialize the list's head */
#ವ್ಯಾಖ್ಯಾನಿಸು LIST_ತಲೆ_INIT(ತಲೆ)                                            \
        ಮಾಡು {                                                            \
                (ತಲೆ) = ಖಾಲಿ;                                          \
        } ಆಗ (false)

/* Initialize a list item */
#ವ್ಯಾಖ್ಯಾನಿಸು LIST_INIT(name,ಅಂಶ)                                            \
        ಮಾಡು {                                                            \
                typeof(*(ಅಂಶ)) *ಅಂಶ_2 = (ಅಂಶ);                        \
                ಪ್ರತಿಜ್ಞಾ(ಅಂಶ_2);                                          \
                ಅಂಶ_2->name##_prev = ಅಂಶ_2->name##_next = ಖಾಲಿ;         \
        } ಆಗ (false)

/* Prepend an item to the list */
#ವ್ಯಾಖ್ಯಾನಿಸು LIST_PREPEND(name,ತಲೆ,ಅಂಶ)                                    \
        ({                                                              \
                typeof(*(ತಲೆ)) **ತಲೆ_2 = &(ತಲೆ), *ಅಂಶ_2 = (ಅಂಶ);     \
                ಪ್ರತಿಜ್ಞಾ(ಅಂಶ_2);                                          \
                ಆದರೆ ((ಅಂಶ_2->name##_next = *ತಲೆ_2))                      \
                        ಅಂಶ_2->name##_next->name##_prev = ಅಂಶ_2;        \
                ಅಂಶ_2->name##_prev = ಖಾಲಿ;                              \
                *ತಲೆ_2 = ಅಂಶ_2;                                         \
                ಅಂಶ_2;                                                  \
        })

/* Append an item to the list */
#ವ್ಯಾಖ್ಯಾನಿಸು LIST_APPEND(name,ತಲೆ,ಅಂಶ)                                     \
        ({                                                              \
                typeof(*(ತಲೆ)) **_hhead = &(ತಲೆ), *ಬಾಲ;             \
                ಬಾಲ = LIST_ಕಂಡುಹಿಡಿ_ಬಾಲ(name, *_hhead);                  \
                LIST_ಸೇರಿಸು_AFTER(name, *_hhead, ಬಾಲ, ಅಂಶ);          \
        })

/* Remove an item from the list */
#ವ್ಯಾಖ್ಯಾನಿಸು LIST_REMOVE(name,ತಲೆ,ಅಂಶ)                                     \
        ({                                                              \
                typeof(*(ತಲೆ)) **ತಲೆ_2 = &(ತಲೆ), *ಅಂಶ_2 = (ಅಂಶ);     \
                ಪ್ರತಿಜ್ಞಾ(ಅಂಶ_2);                                          \
                ಆದರೆ (ಅಂಶ_2->name##_next)                                 \
                        ಅಂಶ_2->name##_next->name##_prev = ಅಂಶ_2->name##_prev; \
                ಆದರೆ (ಅಂಶ_2->name##_prev)                                 \
                        ಅಂಶ_2->name##_prev->name##_next = ಅಂಶ_2->name##_next; \
                ಇಲ್ಲದಿದ್ದರೆ {                                                  \
                        ಪ್ರತಿಜ್ಞಾ(*ತಲೆ_2 == ಅಂಶ_2);                        \
                        *ತಲೆ_2 = ಅಂಶ_2->name##_next;                    \
                }                                                       \
                ಅಂಶ_2->name##_next = ಅಂಶ_2->name##_prev = ಖಾಲಿ;         \
                ಅಂಶ_2;                                                  \
        })

/* Find the head of the list */
#ವ್ಯಾಖ್ಯಾನಿಸು LIST_ಕಂಡುಹಿಡಿ_ತಲೆ(name,ಅಂಶ)                                       \
        ({                                                              \
                typeof(*(ಅಂಶ)) *ಅಂಶ_2 = (ಅಂಶ);                        \
                ಆಗ (ಅಂಶ_2 && ಅಂಶ_2->name##_prev)                     \
                        ಅಂಶ_2 = ಅಂಶ_2->name##_prev;                     \
                ಅಂಶ_2;                                                  \
        })

/* Find the tail of the list */
#ವ್ಯಾಖ್ಯಾನಿಸು LIST_ಕಂಡುಹಿಡಿ_ಬಾಲ(name,ಅಂಶ)                                       \
        ({                                                              \
                typeof(*(ಅಂಶ)) *ಅಂಶ_2 = (ಅಂಶ);                        \
                ಆಗ (ಅಂಶ_2 && ಅಂಶ_2->name##_next)                     \
                        ಅಂಶ_2 = ಅಂಶ_2->name##_next;                     \
                ಅಂಶ_2;                                                  \
        })

/* Insert an item after another one (a = where, b = what) */
#ವ್ಯಾಖ್ಯಾನಿಸು LIST_ಸೇರಿಸು_AFTER(name,ತಲೆ,a,b)                                \
        ({                                                              \
                typeof(*(ತಲೆ)) **ತಲೆ_2 = &(ತಲೆ), *_a = (a), *_b = (b); \
                ಪ್ರತಿಜ್ಞಾ(_b);                                             \
                ಆದರೆ (!_a) {                                              \
                        ಆದರೆ ((_b->name##_next = *ತಲೆ_2))                 \
                                _b->name##_next->name##_prev = _b;      \
                        _b->name##_prev = ಖಾಲಿ;                         \
                        *ತಲೆ_2 = _b;                                    \
                } ಇಲ್ಲದಿದ್ದರೆ {                                                \
                        ಆದರೆ ((_b->name##_next = _a->name##_next))        \
                                _b->name##_next->name##_prev = _b;      \
                        _b->name##_prev = _a;                           \
                        _a->name##_next = _b;                           \
                }                                                       \
                _b;                                                     \
        })

/* Insert an item before another one (a = where, b = what) */
#ವ್ಯಾಖ್ಯಾನಿಸು LIST_ಸೇರಿಸು_BEFORE(name,ತಲೆ,a,b)                               \
        ({                                                              \
                typeof(*(ತಲೆ)) **ತಲೆ_2 = &(ತಲೆ), *_a = (a), *_b = (b); \
                ಪ್ರತಿಜ್ಞಾ(_b);                                             \
                ಆದರೆ (!_a) {                                              \
                        ಆದರೆ (!*ತಲೆ_2) {                                  \
                                _b->name##_next = ಖಾಲಿ;                 \
                                _b->name##_prev = ಖಾಲಿ;                 \
                                *ತಲೆ_2 = _b;                            \
                        } ಇಲ್ಲದಿದ್ದರೆ {                                        \
                                typeof(*(ತಲೆ)) *ಬಾಲ = (ತಲೆ);        \
                                ಆಗ (ಬಾಲ->name##_next)              \
                                        ಬಾಲ = ಬಾಲ->name##_next;     \
                                _b->name##_next = ಖಾಲಿ;                 \
                                _b->name##_prev = ಬಾಲ;                \
                                ಬಾಲ->name##_next = _b;                \
                        }                                               \
                } ಇಲ್ಲದಿದ್ದರೆ {                                                \
                        ಆದರೆ ((_b->name##_prev = _a->name##_prev))        \
                                _b->name##_prev->name##_next = _b;      \
                        ಇಲ್ಲದಿದ್ದರೆ                                            \
                                *ತಲೆ_2 = _b;                            \
                        _b->name##_next = _a;                           \
                        _a->name##_prev = _b;                           \
                }                                                       \
                _b;                                                     \
        })

#ವ್ಯಾಖ್ಯಾನಿಸು LIST_JUST_US(name, ಅಂಶ)                                        \
        ({                                                              \
                typeof(*(ಅಂಶ)) *ಅಂಶ_2 = (ಅಂಶ);                        \
                !(ಅಂಶ_2)->name##_prev && !(ಅಂಶ_2)->name##_next;         \
        })

/* The type of the iterator 'i' is automatically determined by the type of 'head', and declared in the
 * loop. Hence, do not declare the same variable in the outer scope. Sometimes, we set 'head' through
 * hashmap_get(). In that case, you need to explicitly cast the result. */
#ವ್ಯಾಖ್ಯಾನಿಸು LIST_FOREACH_WITH_ಮುಂದಿನ(name,i,n,ತಲೆ)                           \
        ಗಾಗಿ (typeof(*(ತಲೆ)) *n, *i = (ತಲೆ); i && (n = i->name##_next, true); i = n)

#ವ್ಯಾಖ್ಯಾನಿಸು LIST_FOREACH(name,i,ತಲೆ)                                       \
        LIST_FOREACH_WITH_ಮುಂದಿನ(name, i, UNIQ_T(n, UNIQ), ತಲೆ)

#ವ್ಯಾಖ್ಯಾನಿಸು _LIST_FOREACH_WITH_PREV(name,i,p,ಆರಂಭ)                         \
        ಗಾಗಿ (typeof(*(ಆರಂಭ)) *p, *i = (ಆರಂಭ); i && (p = i->name##_prev, true); i = p)

#ವ್ಯಾಖ್ಯಾನಿಸು LIST_FOREACH_BACKWARDS(name,i,ಆರಂಭ)                            \
        _LIST_FOREACH_WITH_PREV(name, i, UNIQ_T(p, UNIQ), ಆರಂಭ)

/* Iterate through all the members of the list p is included in, but skip over p */
#ವ್ಯಾಖ್ಯಾನಿಸು LIST_FOREACH_OTHERS(name,i,p)                                   \
        ಗಾಗಿ (typeof(*(p)) *_p = (p), *i = ({                            \
                                typeof(*_p) *_j = _p;                   \
                                ಆಗ (_j && _j->name##_prev)           \
                                        _j = _j->name##_prev;           \
                                ಆದರೆ (_j == _p)                           \
                                        _j = _p->name##_next;           \
                                _j;                                     \
                        });                                             \
             i;                                                         \
             i = i->name##_next == _p ? _p->name##_next : i->name##_next)

/* Loop starting from p->next until p->prev. p can be adjusted meanwhile. */
#ವ್ಯಾಖ್ಯಾನಿಸು LIST_LOOP_BUT_ONE(name,i,ತಲೆ,p)                                \
        ಗಾಗಿ (typeof(*(p)) *i = (p)->name##_next ? (p)->name##_next : (ತಲೆ); \
             i != (p);                                                  \
             i = i->name##_next ? i->name##_next : (ತಲೆ))

/* Join two lists tail to head: a->b, c->d to a->b->c->d and de-initialise second list */
#ವ್ಯಾಖ್ಯಾನಿಸು LIST_JOIN(name,a,b)                                             \
        ({                                                              \
                ಪ್ರತಿಜ್ಞಾ(b);                                              \
                ಆದರೆ (!(a))                                               \
                        (a) = (b);                                      \
                ಇಲ್ಲದಿದ್ದರೆ {                                                  \
                        typeof(*(a)) *ತಲೆ_2 = (b), *ಬಾಲ;              \
                        ಬಾಲ = LIST_ಕಂಡುಹಿಡಿ_ಬಾಲ(name, (a));              \
                        ಬಾಲ->name##_next = ತಲೆ_2;                     \
                        ತಲೆ_2->name##_prev = ಬಾಲ;                     \
                }                                                       \
                (b) = ಖಾಲಿ;                                             \
                a;                                                      \
        })

#ವ್ಯಾಖ್ಯಾನಿಸು LIST_ತೆಗೆ(name, a)                                               \
        ({                                                              \
                typeof(a)* _a = &(a);                                   \
                typeof(a) _p = *_a;                                     \
                ಆದರೆ (_p)                                                 \
                        LIST_REMOVE(name, *_a, _p);                     \
                _p;                                                     \
        })

#ವ್ಯಾಖ್ಯಾನಿಸು LIST_CLEAR(name, ತಲೆ, free_func)       \
        _LIST_CLEAR(name, ತಲೆ, free_func, UNIQ_T(elem, UNIQ))

/* Clear the list, destroying each element with free_func */
#ವ್ಯಾಖ್ಯಾನಿಸು _LIST_CLEAR(name, ತಲೆ, free_func, elem)        \
        ({                                              \
                typeof(ತಲೆ) elem;                      \
                ಆಗ ((elem = LIST_ತೆಗೆ(name, ತಲೆ)))   \
                        free_func(elem);                \
                ತಲೆ;                                   \
        })
