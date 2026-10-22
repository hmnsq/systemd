/* SPDX-License-Identifier: LGPL-2.1-or-later */
#निर्देशः once

#अन्तर्भाव "forward.h"

/* The head of the linked list. Use this in the structure that shall
 * contain the head of the linked list */
#परिभाषय LIST_शीर्ष(t,name)                                               \
        t *name

/* The pointers in the linked list's items. Use this in the item structure */
#परिभाषय LIST_FIELDS(t,name)                                             \
        t *name##_next, *name##_prev

/* Initialize the list's head */
#परिभाषय LIST_शीर्ष_INIT(शीर्ष)                                            \
        कुरु {                                                            \
                (शीर्ष) = रिक्त;                                          \
        } यावत् (false)

/* Initialize a list item */
#परिभाषय LIST_INIT(name,item)                                            \
        कुरु {                                                            \
                typeof(*(item)) *_item = (item);                        \
                प्रतिज्ञा(_item);                                          \
                _item->name##_prev = _item->name##_next = रिक्त;         \
        } यावत् (false)

/* Prepend an item to the list */
#परिभाषय LIST_PREPEND(name,शीर्ष,item)                                    \
        ({                                                              \
                typeof(*(शीर्ष)) **शीर्ष_2 = &(शीर्ष), *_item = (item);     \
                प्रतिज्ञा(_item);                                          \
                यदि ((_item->name##_next = *शीर्ष_2))                      \
                        _item->name##_next->name##_prev = _item;        \
                _item->name##_prev = रिक्त;                              \
                *शीर्ष_2 = _item;                                         \
                _item;                                                  \
        })

/* Append an item to the list */
#परिभाषय LIST_APPEND(name,शीर्ष,item)                                     \
        ({                                                              \
                typeof(*(शीर्ष)) **_hhead = &(शीर्ष), *पुच्छ;             \
                पुच्छ = LIST_FIND_पुच्छ(name, *_hhead);                  \
                LIST_निवेशय_AFTER(name, *_hhead, पुच्छ, item);          \
        })

/* Remove an item from the list */
#परिभाषय LIST_REMOVE(name,शीर्ष,item)                                     \
        ({                                                              \
                typeof(*(शीर्ष)) **शीर्ष_2 = &(शीर्ष), *_item = (item);     \
                प्रतिज्ञा(_item);                                          \
                यदि (_item->name##_next)                                 \
                        _item->name##_next->name##_prev = _item->name##_prev; \
                यदि (_item->name##_prev)                                 \
                        _item->name##_prev->name##_next = _item->name##_next; \
                अन्यथा {                                                  \
                        प्रतिज्ञा(*शीर्ष_2 == _item);                        \
                        *शीर्ष_2 = _item->name##_next;                    \
                }                                                       \
                _item->name##_next = _item->name##_prev = रिक्त;         \
                _item;                                                  \
        })

/* Find the head of the list */
#परिभाषय LIST_FIND_शीर्ष(name,item)                                       \
        ({                                                              \
                typeof(*(item)) *_item = (item);                        \
                यावत् (_item && _item->name##_prev)                     \
                        _item = _item->name##_prev;                     \
                _item;                                                  \
        })

/* Find the tail of the list */
#परिभाषय LIST_FIND_पुच्छ(name,item)                                       \
        ({                                                              \
                typeof(*(item)) *_item = (item);                        \
                यावत् (_item && _item->name##_next)                     \
                        _item = _item->name##_next;                     \
                _item;                                                  \
        })

/* Insert an item after another one (a = where, b = what) */
#परिभाषय LIST_निवेशय_AFTER(name,शीर्ष,a,b)                                \
        ({                                                              \
                typeof(*(शीर्ष)) **शीर्ष_2 = &(शीर्ष), *_a = (a), *_b = (b); \
                प्रतिज्ञा(_b);                                             \
                यदि (!_a) {                                              \
                        यदि ((_b->name##_next = *शीर्ष_2))                 \
                                _b->name##_next->name##_prev = _b;      \
                        _b->name##_prev = रिक्त;                         \
                        *शीर्ष_2 = _b;                                    \
                } अन्यथा {                                                \
                        यदि ((_b->name##_next = _a->name##_next))        \
                                _b->name##_next->name##_prev = _b;      \
                        _b->name##_prev = _a;                           \
                        _a->name##_next = _b;                           \
                }                                                       \
                _b;                                                     \
        })

/* Insert an item before another one (a = where, b = what) */
#परिभाषय LIST_निवेशय_BEFORE(name,शीर्ष,a,b)                               \
        ({                                                              \
                typeof(*(शीर्ष)) **शीर्ष_2 = &(शीर्ष), *_a = (a), *_b = (b); \
                प्रतिज्ञा(_b);                                             \
                यदि (!_a) {                                              \
                        यदि (!*शीर्ष_2) {                                  \
                                _b->name##_next = रिक्त;                 \
                                _b->name##_prev = रिक्त;                 \
                                *शीर्ष_2 = _b;                            \
                        } अन्यथा {                                        \
                                typeof(*(शीर्ष)) *पुच्छ = (शीर्ष);        \
                                यावत् (पुच्छ->name##_next)              \
                                        पुच्छ = पुच्छ->name##_next;     \
                                _b->name##_next = रिक्त;                 \
                                _b->name##_prev = पुच्छ;                \
                                पुच्छ->name##_next = _b;                \
                        }                                               \
                } अन्यथा {                                                \
                        यदि ((_b->name##_prev = _a->name##_prev))        \
                                _b->name##_prev->name##_next = _b;      \
                        अन्यथा                                            \
                                *शीर्ष_2 = _b;                            \
                        _b->name##_next = _a;                           \
                        _a->name##_prev = _b;                           \
                }                                                       \
                _b;                                                     \
        })

#परिभाषय LIST_JUST_US(name, item)                                        \
        ({                                                              \
                typeof(*(item)) *_item = (item);                        \
                !(_item)->name##_prev && !(_item)->name##_next;         \
        })

/* The type of the iterator 'i' is automatically determined by the type of 'head', and declared in the
 * loop. Hence, do not declare the same variable in the outer scope. Sometimes, we set 'head' through
 * hashmap_get(). In that case, you need to explicitly cast the result. */
#परिभाषय LIST_FOREACH_WITH_अग्र(name,i,n,शीर्ष)                           \
        प्रति (typeof(*(शीर्ष)) *n, *i = (शीर्ष); i && (n = i->name##_next, true); i = n)

#परिभाषय LIST_FOREACH(name,i,शीर्ष)                                       \
        LIST_FOREACH_WITH_अग्र(name, i, UNIQ_T(n, UNIQ), शीर्ष)

#परिभाषय LIST_FOREACH_WITH_पूर्व(name,i,p,start)                         \
        प्रति (typeof(*(start)) *p, *i = (start); i && (p = i->name##_prev, true); i = p)

#परिभाषय LIST_FOREACH_BACKWARDS(name,i,start)                            \
        LIST_FOREACH_WITH_पूर्व(name, i, UNIQ_T(p, UNIQ), start)

/* Iterate through all the members of the list p is included in, but skip over p */
#परिभाषय LIST_FOREACH_OTHERS(name,i,p)                                   \
        प्रति (typeof(*(p)) *_p = (p), *i = ({                            \
                                typeof(*_p) *_j = _p;                   \
                                यावत् (_j && _j->name##_prev)           \
                                        _j = _j->name##_prev;           \
                                यदि (_j == _p)                           \
                                        _j = _p->name##_next;           \
                                _j;                                     \
                        });                                             \
             i;                                                         \
             i = i->name##_next == _p ? _p->name##_next : i->name##_next)

/* Loop starting from p->next until p->prev. p can be adjusted meanwhile. */
#परिभाषय LIST_LOOP_BUT_ONE(name,i,शीर्ष,p)                                \
        प्रति (typeof(*(p)) *i = (p)->name##_next ? (p)->name##_next : (शीर्ष); \
             i != (p);                                                  \
             i = i->name##_next ? i->name##_next : (शीर्ष))

/* Join two lists tail to head: a->b, c->d to a->b->c->d and de-initialise second list */
#परिभाषय LIST_JOIN(name,a,b)                                             \
        ({                                                              \
                प्रतिज्ञा(b);                                              \
                यदि (!(a))                                               \
                        (a) = (b);                                      \
                अन्यथा {                                                  \
                        typeof(*(a)) *शीर्ष_2 = (b), *पुच्छ;              \
                        पुच्छ = LIST_FIND_पुच्छ(name, (a));              \
                        पुच्छ->name##_next = शीर्ष_2;                     \
                        शीर्ष_2->name##_prev = पुच्छ;                     \
                }                                                       \
                (b) = रिक्त;                                             \
                a;                                                      \
        })

#परिभाषय LIST_निष्कासय(name, a)                                               \
        ({                                                              \
                typeof(a)* _a = &(a);                                   \
                typeof(a) _p = *_a;                                     \
                यदि (_p)                                                 \
                        LIST_REMOVE(name, *_a, _p);                     \
                _p;                                                     \
        })

#परिभाषय LIST_CLEAR(name, शीर्ष, free_func)       \
        _LIST_CLEAR(name, शीर्ष, free_func, UNIQ_T(elem, UNIQ))

/* Clear the list, destroying each element with free_func */
#परिभाषय _LIST_CLEAR(name, शीर्ष, free_func, elem)        \
        ({                                              \
                typeof(शीर्ष) elem;                      \
                यावत् ((elem = LIST_निष्कासय(name, शीर्ष)))   \
                        free_func(elem);                \
                शीर्ष;                                   \
        })
