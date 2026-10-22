/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"

/* The head of the linked list. Use this in the structure that shall
 * contain the head of the linked list */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LIST_ਸਿਰ(t,name)                                               \
        t *name

/* The pointers in the linked list's items. Use this in the item structure */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LIST_FIELDS(t,name)                                             \
        t *name##_next, *name##_prev

/* Initialize the list's head */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LIST_ਸਿਰ_INIT(ਸਿਰ)                                            \
        ਕਰੋ {                                                            \
                (ਸਿਰ) = ਨਿਰਾ;                                          \
        } ਜਦੋਂ (false)

/* Initialize a list item */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LIST_INIT(name,ਵਸਤੂ_2)                                            \
        ਕਰੋ {                                                            \
                typeof(*(ਵਸਤੂ_2)) *ਵਸਤੂ_3 = (ਵਸਤੂ_2);                        \
                ਪ੍ਰਤਿਜ੍ਞਾ(ਵਸਤੂ_3);                                          \
                ਵਸਤੂ_3->name##_prev = ਵਸਤੂ_3->name##_next = ਨਿਰਾ;         \
        } ਜਦੋਂ (false)

/* Prepend an item to the list */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LIST_PREPEND(name,ਸਿਰ,ਵਸਤੂ_2)                                    \
        ({                                                              \
                typeof(*(ਸਿਰ)) **ਸਿਰ_2 = &(ਸਿਰ), *ਵਸਤੂ_3 = (ਵਸਤੂ_2);     \
                ਪ੍ਰਤਿਜ੍ਞਾ(ਵਸਤੂ_3);                                          \
                ਜੇ ((ਵਸਤੂ_3->name##_next = *ਸਿਰ_2))                      \
                        ਵਸਤੂ_3->name##_next->name##_prev = ਵਸਤੂ_3;        \
                ਵਸਤੂ_3->name##_prev = ਨਿਰਾ;                              \
                *ਸਿਰ_2 = ਵਸਤੂ_3;                                         \
                ਵਸਤੂ_3;                                                  \
        })

/* Append an item to the list */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LIST_APPEND(name,ਸਿਰ,ਵਸਤੂ_2)                                     \
        ({                                                              \
                typeof(*(ਸਿਰ)) **_hhead = &(ਸਿਰ), *ਪੂਛ;             \
                ਪੂਛ = LIST_ਲੱਭੋ_ਪੂਛ(name, *_hhead);                  \
                LIST_ਪਾਓ_AFTER(name, *_hhead, ਪੂਛ, ਵਸਤੂ_2);          \
        })

/* Remove an item from the list */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LIST_REMOVE(name,ਸਿਰ,ਵਸਤੂ_2)                                     \
        ({                                                              \
                typeof(*(ਸਿਰ)) **ਸਿਰ_2 = &(ਸਿਰ), *ਵਸਤੂ_3 = (ਵਸਤੂ_2);     \
                ਪ੍ਰਤਿਜ੍ਞਾ(ਵਸਤੂ_3);                                          \
                ਜੇ (ਵਸਤੂ_3->name##_next)                                 \
                        ਵਸਤੂ_3->name##_next->name##_prev = ਵਸਤੂ_3->name##_prev; \
                ਜੇ (ਵਸਤੂ_3->name##_prev)                                 \
                        ਵਸਤੂ_3->name##_prev->name##_next = ਵਸਤੂ_3->name##_next; \
                ਨਹੀਂਤਾਂ {                                                  \
                        ਪ੍ਰਤਿਜ੍ਞਾ(*ਸਿਰ_2 == ਵਸਤੂ_3);                        \
                        *ਸਿਰ_2 = ਵਸਤੂ_3->name##_next;                    \
                }                                                       \
                ਵਸਤੂ_3->name##_next = ਵਸਤੂ_3->name##_prev = ਨਿਰਾ;         \
                ਵਸਤੂ_3;                                                  \
        })

/* Find the head of the list */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LIST_ਲੱਭੋ_ਸਿਰ(name,ਵਸਤੂ_2)                                       \
        ({                                                              \
                typeof(*(ਵਸਤੂ_2)) *ਵਸਤੂ_3 = (ਵਸਤੂ_2);                        \
                ਜਦੋਂ (ਵਸਤੂ_3 && ਵਸਤੂ_3->name##_prev)                     \
                        ਵਸਤੂ_3 = ਵਸਤੂ_3->name##_prev;                     \
                ਵਸਤੂ_3;                                                  \
        })

/* Find the tail of the list */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LIST_ਲੱਭੋ_ਪੂਛ(name,ਵਸਤੂ_2)                                       \
        ({                                                              \
                typeof(*(ਵਸਤੂ_2)) *ਵਸਤੂ_3 = (ਵਸਤੂ_2);                        \
                ਜਦੋਂ (ਵਸਤੂ_3 && ਵਸਤੂ_3->name##_next)                     \
                        ਵਸਤੂ_3 = ਵਸਤੂ_3->name##_next;                     \
                ਵਸਤੂ_3;                                                  \
        })

/* Insert an item after another one (a = where, b = what) */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LIST_ਪਾਓ_AFTER(name,ਸਿਰ,a,b)                                \
        ({                                                              \
                typeof(*(ਸਿਰ)) **ਸਿਰ_2 = &(ਸਿਰ), *_a = (a), *_b = (b); \
                ਪ੍ਰਤਿਜ੍ਞਾ(_b);                                             \
                ਜੇ (!_a) {                                              \
                        ਜੇ ((_b->name##_next = *ਸਿਰ_2))                 \
                                _b->name##_next->name##_prev = _b;      \
                        _b->name##_prev = ਨਿਰਾ;                         \
                        *ਸਿਰ_2 = _b;                                    \
                } ਨਹੀਂਤਾਂ {                                                \
                        ਜੇ ((_b->name##_next = _a->name##_next))        \
                                _b->name##_next->name##_prev = _b;      \
                        _b->name##_prev = _a;                           \
                        _a->name##_next = _b;                           \
                }                                                       \
                _b;                                                     \
        })

/* Insert an item before another one (a = where, b = what) */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LIST_ਪਾਓ_BEFORE(name,ਸਿਰ,a,b)                               \
        ({                                                              \
                typeof(*(ਸਿਰ)) **ਸਿਰ_2 = &(ਸਿਰ), *_a = (a), *_b = (b); \
                ਪ੍ਰਤਿਜ੍ਞਾ(_b);                                             \
                ਜੇ (!_a) {                                              \
                        ਜੇ (!*ਸਿਰ_2) {                                  \
                                _b->name##_next = ਨਿਰਾ;                 \
                                _b->name##_prev = ਨਿਰਾ;                 \
                                *ਸਿਰ_2 = _b;                            \
                        } ਨਹੀਂਤਾਂ {                                        \
                                typeof(*(ਸਿਰ)) *ਪੂਛ = (ਸਿਰ);        \
                                ਜਦੋਂ (ਪੂਛ->name##_next)              \
                                        ਪੂਛ = ਪੂਛ->name##_next;     \
                                _b->name##_next = ਨਿਰਾ;                 \
                                _b->name##_prev = ਪੂਛ;                \
                                ਪੂਛ->name##_next = _b;                \
                        }                                               \
                } ਨਹੀਂਤਾਂ {                                                \
                        ਜੇ ((_b->name##_prev = _a->name##_prev))        \
                                _b->name##_prev->name##_next = _b;      \
                        ਨਹੀਂਤਾਂ                                            \
                                *ਸਿਰ_2 = _b;                            \
                        _b->name##_next = _a;                           \
                        _a->name##_prev = _b;                           \
                }                                                       \
                _b;                                                     \
        })

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LIST_JUST_US(name, ਵਸਤੂ_2)                                        \
        ({                                                              \
                typeof(*(ਵਸਤੂ_2)) *ਵਸਤੂ_3 = (ਵਸਤੂ_2);                        \
                !(ਵਸਤੂ_3)->name##_prev && !(ਵਸਤੂ_3)->name##_next;         \
        })

/* The type of the iterator 'i' is automatically determined by the type of 'head', and declared in the
 * loop. Hence, do not declare the same variable in the outer scope. Sometimes, we set 'head' through
 * hashmap_get(). In that case, you need to explicitly cast the result. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LIST_FOREACH_WITH_ਅਗਲਾ(name,i,n,ਸਿਰ)                           \
        ਲਈ (typeof(*(ਸਿਰ)) *n, *i = (ਸਿਰ); i && (n = i->name##_next, true); i = n)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LIST_FOREACH(name,i,ਸਿਰ)                                       \
        LIST_FOREACH_WITH_ਅਗਲਾ(name, i, UNIQ_T(n, UNIQ), ਸਿਰ)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _LIST_FOREACH_WITH_PREV(name,i,p,ਸ਼ੁਰੂ)                         \
        ਲਈ (typeof(*(ਸ਼ੁਰੂ)) *p, *i = (ਸ਼ੁਰੂ); i && (p = i->name##_prev, true); i = p)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LIST_FOREACH_BACKWARDS(name,i,ਸ਼ੁਰੂ)                            \
        _LIST_FOREACH_WITH_PREV(name, i, UNIQ_T(p, UNIQ), ਸ਼ੁਰੂ)

/* Iterate through all the members of the list p is included in, but skip over p */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LIST_FOREACH_OTHERS(name,i,p)                                   \
        ਲਈ (typeof(*(p)) *_p = (p), *i = ({                            \
                                typeof(*_p) *_j = _p;                   \
                                ਜਦੋਂ (_j && _j->name##_prev)           \
                                        _j = _j->name##_prev;           \
                                ਜੇ (_j == _p)                           \
                                        _j = _p->name##_next;           \
                                _j;                                     \
                        });                                             \
             i;                                                         \
             i = i->name##_next == _p ? _p->name##_next : i->name##_next)

/* Loop starting from p->next until p->prev. p can be adjusted meanwhile. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LIST_LOOP_BUT_ONE(name,i,ਸਿਰ,p)                                \
        ਲਈ (typeof(*(p)) *i = (p)->name##_next ? (p)->name##_next : (ਸਿਰ); \
             i != (p);                                                  \
             i = i->name##_next ? i->name##_next : (ਸਿਰ))

/* Join two lists tail to head: a->b, c->d to a->b->c->d and de-initialise second list */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LIST_JOIN(name,a,b)                                             \
        ({                                                              \
                ਪ੍ਰਤਿਜ੍ਞਾ(b);                                              \
                ਜੇ (!(a))                                               \
                        (a) = (b);                                      \
                ਨਹੀਂਤਾਂ {                                                  \
                        typeof(*(a)) *ਸਿਰ_2 = (b), *ਪੂਛ;              \
                        ਪੂਛ = LIST_ਲੱਭੋ_ਪੂਛ(name, (a));              \
                        ਪੂਛ->name##_next = ਸਿਰ_2;                     \
                        ਸਿਰ_2->name##_prev = ਪੂਛ;                     \
                }                                                       \
                (b) = ਨਿਰਾ;                                             \
                a;                                                      \
        })

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LIST_ਕੱਢੋ(name, a)                                               \
        ({                                                              \
                typeof(a)* _a = &(a);                                   \
                typeof(a) _p = *_a;                                     \
                ਜੇ (_p)                                                 \
                        LIST_REMOVE(name, *_a, _p);                     \
                _p;                                                     \
        })

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LIST_CLEAR(name, ਸਿਰ, free_func)       \
        _LIST_CLEAR(name, ਸਿਰ, free_func, UNIQ_T(elem, UNIQ))

/* Clear the list, destroying each element with free_func */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _LIST_CLEAR(name, ਸਿਰ, free_func, elem)        \
        ({                                              \
                typeof(ਸਿਰ) elem;                      \
                ਜਦੋਂ ((elem = LIST_ਕੱਢੋ(name, ਸਿਰ)))   \
                        free_func(elem);                \
                ਸਿਰ;                                   \
        })
