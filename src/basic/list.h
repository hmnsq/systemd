/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব "forward.h"

/* The head of the linked list. Use this in the structure that shall
 * contain the head of the linked list */
#define LIST_শীর্ষ(t,name)                                               \
        t *name

/* The pointers in the linked list's items. Use this in the item structure */
#define LIST_FIELDS(t,name)                                             \
        t *name##_next, *name##_prev

/* Initialize the list's head */
#define LIST_শীর্ষ_INIT(শীর্ষ)                                            \
        করো {                                                            \
                (শীর্ষ) = ফাঁকা;                                          \
        } যখন (false)

/* Initialize a list item */
#define LIST_INIT(name,item)                                            \
        করো {                                                            \
                typeof(*(item)) *_item = (item);                        \
                প্রতিজ্ঞা(_item);                                          \
                _item->name##_prev = _item->name##_next = ফাঁকা;         \
        } যখন (false)

/* Prepend an item to the list */
#define LIST_PREPEND(name,শীর্ষ,item)                                    \
        ({                                                              \
                typeof(*(শীর্ষ)) **শীর্ষ_2 = &(শীর্ষ), *_item = (item);     \
                প্রতিজ্ঞা(_item);                                          \
                যদি ((_item->name##_next = *শীর্ষ_2))                      \
                        _item->name##_next->name##_prev = _item;        \
                _item->name##_prev = ফাঁকা;                              \
                *শীর্ষ_2 = _item;                                         \
                _item;                                                  \
        })

/* Append an item to the list */
#define LIST_APPEND(name,শীর্ষ,item)                                     \
        ({                                                              \
                typeof(*(শীর্ষ)) **_hhead = &(শীর্ষ), *লেজ;             \
                লেজ = LIST_FIND_লেজ(name, *_hhead);                  \
                LIST_ঢোকাও_AFTER(name, *_hhead, লেজ, item);          \
        })

/* Remove an item from the list */
#define LIST_REMOVE(name,শীর্ষ,item)                                     \
        ({                                                              \
                typeof(*(শীর্ষ)) **শীর্ষ_2 = &(শীর্ষ), *_item = (item);     \
                প্রতিজ্ঞা(_item);                                          \
                যদি (_item->name##_next)                                 \
                        _item->name##_next->name##_prev = _item->name##_prev; \
                যদি (_item->name##_prev)                                 \
                        _item->name##_prev->name##_next = _item->name##_next; \
                অন্যথা {                                                  \
                        প্রতিজ্ঞা(*শীর্ষ_2 == _item);                        \
                        *শীর্ষ_2 = _item->name##_next;                    \
                }                                                       \
                _item->name##_next = _item->name##_prev = ফাঁকা;         \
                _item;                                                  \
        })

/* Find the head of the list */
#define LIST_FIND_শীর্ষ(name,item)                                       \
        ({                                                              \
                typeof(*(item)) *_item = (item);                        \
                যখন (_item && _item->name##_prev)                     \
                        _item = _item->name##_prev;                     \
                _item;                                                  \
        })

/* Find the tail of the list */
#define LIST_FIND_লেজ(name,item)                                       \
        ({                                                              \
                typeof(*(item)) *_item = (item);                        \
                যখন (_item && _item->name##_next)                     \
                        _item = _item->name##_next;                     \
                _item;                                                  \
        })

/* Insert an item after another one (a = where, b = what) */
#define LIST_ঢোকাও_AFTER(name,শীর্ষ,a,b)                                \
        ({                                                              \
                typeof(*(শীর্ষ)) **শীর্ষ_2 = &(শীর্ষ), *_a = (a), *_b = (b); \
                প্রতিজ্ঞা(_b);                                             \
                যদি (!_a) {                                              \
                        যদি ((_b->name##_next = *শীর্ষ_2))                 \
                                _b->name##_next->name##_prev = _b;      \
                        _b->name##_prev = ফাঁকা;                         \
                        *শীর্ষ_2 = _b;                                    \
                } অন্যথা {                                                \
                        যদি ((_b->name##_next = _a->name##_next))        \
                                _b->name##_next->name##_prev = _b;      \
                        _b->name##_prev = _a;                           \
                        _a->name##_next = _b;                           \
                }                                                       \
                _b;                                                     \
        })

/* Insert an item before another one (a = where, b = what) */
#define LIST_ঢোকাও_BEFORE(name,শীর্ষ,a,b)                               \
        ({                                                              \
                typeof(*(শীর্ষ)) **শীর্ষ_2 = &(শীর্ষ), *_a = (a), *_b = (b); \
                প্রতিজ্ঞা(_b);                                             \
                যদি (!_a) {                                              \
                        যদি (!*শীর্ষ_2) {                                  \
                                _b->name##_next = ফাঁকা;                 \
                                _b->name##_prev = ফাঁকা;                 \
                                *শীর্ষ_2 = _b;                            \
                        } অন্যথা {                                        \
                                typeof(*(শীর্ষ)) *লেজ = (শীর্ষ);        \
                                যখন (লেজ->name##_next)              \
                                        লেজ = লেজ->name##_next;     \
                                _b->name##_next = ফাঁকা;                 \
                                _b->name##_prev = লেজ;                \
                                লেজ->name##_next = _b;                \
                        }                                               \
                } অন্যথা {                                                \
                        যদি ((_b->name##_prev = _a->name##_prev))        \
                                _b->name##_prev->name##_next = _b;      \
                        অন্যথা                                            \
                                *শীর্ষ_2 = _b;                            \
                        _b->name##_next = _a;                           \
                        _a->name##_prev = _b;                           \
                }                                                       \
                _b;                                                     \
        })

#define LIST_JUST_US(name, item)                                        \
        ({                                                              \
                typeof(*(item)) *_item = (item);                        \
                !(_item)->name##_prev && !(_item)->name##_next;         \
        })

/* The type of the iterator 'i' is automatically determined by the type of 'head', and declared in the
 * loop. Hence, do not declare the same variable in the outer scope. Sometimes, we set 'head' through
 * hashmap_get(). In that case, you need to explicitly cast the result. */
#define LIST_FOREACH_WITH_পরবর্তী(name,i,n,শীর্ষ)                           \
        জন্য (typeof(*(শীর্ষ)) *n, *i = (শীর্ষ); i && (n = i->name##_next, true); i = n)

#define LIST_FOREACH(name,i,শীর্ষ)                                       \
        LIST_FOREACH_WITH_পরবর্তী(name, i, UNIQ_T(n, UNIQ), শীর্ষ)

#define LIST_FOREACH_WITH_পূর্ব(name,i,p,start)                         \
        জন্য (typeof(*(start)) *p, *i = (start); i && (p = i->name##_prev, true); i = p)

#define LIST_FOREACH_BACKWARDS(name,i,start)                            \
        LIST_FOREACH_WITH_পূর্ব(name, i, UNIQ_T(p, UNIQ), start)

/* Iterate through all the members of the list p is included in, but skip over p */
#define LIST_FOREACH_OTHERS(name,i,p)                                   \
        জন্য (typeof(*(p)) *_p = (p), *i = ({                            \
                                typeof(*_p) *_j = _p;                   \
                                যখন (_j && _j->name##_prev)           \
                                        _j = _j->name##_prev;           \
                                যদি (_j == _p)                           \
                                        _j = _p->name##_next;           \
                                _j;                                     \
                        });                                             \
             i;                                                         \
             i = i->name##_next == _p ? _p->name##_next : i->name##_next)

/* Loop starting from p->next until p->prev. p can be adjusted meanwhile. */
#define LIST_LOOP_BUT_ONE(name,i,শীর্ষ,p)                                \
        জন্য (typeof(*(p)) *i = (p)->name##_next ? (p)->name##_next : (শীর্ষ); \
             i != (p);                                                  \
             i = i->name##_next ? i->name##_next : (শীর্ষ))

/* Join two lists tail to head: a->b, c->d to a->b->c->d and de-initialise second list */
#define LIST_JOIN(name,a,b)                                             \
        ({                                                              \
                প্রতিজ্ঞা(b);                                              \
                যদি (!(a))                                               \
                        (a) = (b);                                      \
                অন্যথা {                                                  \
                        typeof(*(a)) *শীর্ষ_2 = (b), *লেজ;              \
                        লেজ = LIST_FIND_লেজ(name, (a));              \
                        লেজ->name##_next = শীর্ষ_2;                     \
                        শীর্ষ_2->name##_prev = লেজ;                     \
                }                                                       \
                (b) = ফাঁকা;                                             \
                a;                                                      \
        })

#define LIST_বের_করো(name, a)                                               \
        ({                                                              \
                typeof(a)* _a = &(a);                                   \
                typeof(a) _p = *_a;                                     \
                যদি (_p)                                                 \
                        LIST_REMOVE(name, *_a, _p);                     \
                _p;                                                     \
        })

#define LIST_CLEAR(name, শীর্ষ, free_func)       \
        _LIST_CLEAR(name, শীর্ষ, free_func, UNIQ_T(elem, UNIQ))

/* Clear the list, destroying each element with free_func */
#define _LIST_CLEAR(name, শীর্ষ, free_func, elem)        \
        ({                                              \
                typeof(শীর্ষ) elem;                      \
                যখন ((elem = LIST_বের_করো(name, শীর্ষ)))   \
                        free_func(elem);                \
                শীর্ষ;                                   \
        })
