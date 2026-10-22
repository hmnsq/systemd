/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ "forward.h"

/* The head of the linked list. Use this in the structure that shall
 * contain the head of the linked list */
#నిర్వచించు LIST_తల(t,name)                                               \
        t *name

/* The pointers in the linked list's items. Use this in the item structure */
#నిర్వచించు LIST_FIELDS(t,name)                                             \
        t *name##_next, *name##_prev

/* Initialize the list's head */
#నిర్వచించు LIST_తల_INIT(తల)                                            \
        చేయి {                                                            \
                (తల) = ఖాళీ;                                          \
        } అయితే (false)

/* Initialize a list item */
#నిర్వచించు LIST_INIT(name,అంశం)                                            \
        చేయి {                                                            \
                typeof(*(అంశం)) *అంశం_2 = (అంశం);                        \
                ప్రతిజ్ఞా(అంశం_2);                                          \
                అంశం_2->name##_prev = అంశం_2->name##_next = ఖాళీ;         \
        } అయితే (false)

/* Prepend an item to the list */
#నిర్వచించు LIST_PREPEND(name,తల,అంశం)                                    \
        ({                                                              \
                typeof(*(తల)) **తల_2 = &(తల), *అంశం_2 = (అంశం);     \
                ప్రతిజ్ఞా(అంశం_2);                                          \
                ఒకవేళ ((అంశం_2->name##_next = *తల_2))                      \
                        అంశం_2->name##_next->name##_prev = అంశం_2;        \
                అంశం_2->name##_prev = ఖాళీ;                              \
                *తల_2 = అంశం_2;                                         \
                అంశం_2;                                                  \
        })

/* Append an item to the list */
#నిర్వచించు LIST_APPEND(name,తల,అంశం)                                     \
        ({                                                              \
                typeof(*(తల)) **_hhead = &(తల), *తోక;             \
                తోక = LIST_కనుగొను_తోక(name, *_hhead);                  \
                LIST_చొప్పించు_AFTER(name, *_hhead, తోక, అంశం);          \
        })

/* Remove an item from the list */
#నిర్వచించు LIST_REMOVE(name,తల,అంశం)                                     \
        ({                                                              \
                typeof(*(తల)) **తల_2 = &(తల), *అంశం_2 = (అంశం);     \
                ప్రతిజ్ఞా(అంశం_2);                                          \
                ఒకవేళ (అంశం_2->name##_next)                                 \
                        అంశం_2->name##_next->name##_prev = అంశం_2->name##_prev; \
                ఒకవేళ (అంశం_2->name##_prev)                                 \
                        అంశం_2->name##_prev->name##_next = అంశం_2->name##_next; \
                లేకపోతే {                                                  \
                        ప్రతిజ్ఞా(*తల_2 == అంశం_2);                        \
                        *తల_2 = అంశం_2->name##_next;                    \
                }                                                       \
                అంశం_2->name##_next = అంశం_2->name##_prev = ఖాళీ;         \
                అంశం_2;                                                  \
        })

/* Find the head of the list */
#నిర్వచించు LIST_కనుగొను_తల(name,అంశం)                                       \
        ({                                                              \
                typeof(*(అంశం)) *అంశం_2 = (అంశం);                        \
                అయితే (అంశం_2 && అంశం_2->name##_prev)                     \
                        అంశం_2 = అంశం_2->name##_prev;                     \
                అంశం_2;                                                  \
        })

/* Find the tail of the list */
#నిర్వచించు LIST_కనుగొను_తోక(name,అంశం)                                       \
        ({                                                              \
                typeof(*(అంశం)) *అంశం_2 = (అంశం);                        \
                అయితే (అంశం_2 && అంశం_2->name##_next)                     \
                        అంశం_2 = అంశం_2->name##_next;                     \
                అంశం_2;                                                  \
        })

/* Insert an item after another one (a = where, b = what) */
#నిర్వచించు LIST_చొప్పించు_AFTER(name,తల,a,b)                                \
        ({                                                              \
                typeof(*(తల)) **తల_2 = &(తల), *_a = (a), *_b = (b); \
                ప్రతిజ్ఞా(_b);                                             \
                ఒకవేళ (!_a) {                                              \
                        ఒకవేళ ((_b->name##_next = *తల_2))                 \
                                _b->name##_next->name##_prev = _b;      \
                        _b->name##_prev = ఖాళీ;                         \
                        *తల_2 = _b;                                    \
                } లేకపోతే {                                                \
                        ఒకవేళ ((_b->name##_next = _a->name##_next))        \
                                _b->name##_next->name##_prev = _b;      \
                        _b->name##_prev = _a;                           \
                        _a->name##_next = _b;                           \
                }                                                       \
                _b;                                                     \
        })

/* Insert an item before another one (a = where, b = what) */
#నిర్వచించు LIST_చొప్పించు_BEFORE(name,తల,a,b)                               \
        ({                                                              \
                typeof(*(తల)) **తల_2 = &(తల), *_a = (a), *_b = (b); \
                ప్రతిజ్ఞా(_b);                                             \
                ఒకవేళ (!_a) {                                              \
                        ఒకవేళ (!*తల_2) {                                  \
                                _b->name##_next = ఖాళీ;                 \
                                _b->name##_prev = ఖాళీ;                 \
                                *తల_2 = _b;                            \
                        } లేకపోతే {                                        \
                                typeof(*(తల)) *తోక = (తల);        \
                                అయితే (తోక->name##_next)              \
                                        తోక = తోక->name##_next;     \
                                _b->name##_next = ఖాళీ;                 \
                                _b->name##_prev = తోక;                \
                                తోక->name##_next = _b;                \
                        }                                               \
                } లేకపోతే {                                                \
                        ఒకవేళ ((_b->name##_prev = _a->name##_prev))        \
                                _b->name##_prev->name##_next = _b;      \
                        లేకపోతే                                            \
                                *తల_2 = _b;                            \
                        _b->name##_next = _a;                           \
                        _a->name##_prev = _b;                           \
                }                                                       \
                _b;                                                     \
        })

#నిర్వచించు LIST_JUST_US(name, అంశం)                                        \
        ({                                                              \
                typeof(*(అంశం)) *అంశం_2 = (అంశం);                        \
                !(అంశం_2)->name##_prev && !(అంశం_2)->name##_next;         \
        })

/* The type of the iterator 'i' is automatically determined by the type of 'head', and declared in the
 * loop. Hence, do not declare the same variable in the outer scope. Sometimes, we set 'head' through
 * hashmap_get(). In that case, you need to explicitly cast the result. */
#నిర్వచించు LIST_FOREACH_WITH_తదుపరి(name,i,n,తల)                           \
        కొరకు (typeof(*(తల)) *n, *i = (తల); i && (n = i->name##_next, true); i = n)

#నిర్వచించు LIST_FOREACH(name,i,తల)                                       \
        LIST_FOREACH_WITH_తదుపరి(name, i, UNIQ_T(n, UNIQ), తల)

#నిర్వచించు _LIST_FOREACH_WITH_PREV(name,i,p,ప్రారంభం)                         \
        కొరకు (typeof(*(ప్రారంభం)) *p, *i = (ప్రారంభం); i && (p = i->name##_prev, true); i = p)

#నిర్వచించు LIST_FOREACH_BACKWARDS(name,i,ప్రారంభం)                            \
        _LIST_FOREACH_WITH_PREV(name, i, UNIQ_T(p, UNIQ), ప్రారంభం)

/* Iterate through all the members of the list p is included in, but skip over p */
#నిర్వచించు LIST_FOREACH_OTHERS(name,i,p)                                   \
        కొరకు (typeof(*(p)) *_p = (p), *i = ({                            \
                                typeof(*_p) *_j = _p;                   \
                                అయితే (_j && _j->name##_prev)           \
                                        _j = _j->name##_prev;           \
                                ఒకవేళ (_j == _p)                           \
                                        _j = _p->name##_next;           \
                                _j;                                     \
                        });                                             \
             i;                                                         \
             i = i->name##_next == _p ? _p->name##_next : i->name##_next)

/* Loop starting from p->next until p->prev. p can be adjusted meanwhile. */
#నిర్వచించు LIST_LOOP_BUT_ONE(name,i,తల,p)                                \
        కొరకు (typeof(*(p)) *i = (p)->name##_next ? (p)->name##_next : (తల); \
             i != (p);                                                  \
             i = i->name##_next ? i->name##_next : (తల))

/* Join two lists tail to head: a->b, c->d to a->b->c->d and de-initialise second list */
#నిర్వచించు LIST_JOIN(name,a,b)                                             \
        ({                                                              \
                ప్రతిజ్ఞా(b);                                              \
                ఒకవేళ (!(a))                                               \
                        (a) = (b);                                      \
                లేకపోతే {                                                  \
                        typeof(*(a)) *తల_2 = (b), *తోక;              \
                        తోక = LIST_కనుగొను_తోక(name, (a));              \
                        తోక->name##_next = తల_2;                     \
                        తల_2->name##_prev = తోక;                     \
                }                                                       \
                (b) = ఖాళీ;                                             \
                a;                                                      \
        })

#నిర్వచించు LIST_తీయి(name, a)                                               \
        ({                                                              \
                typeof(a)* _a = &(a);                                   \
                typeof(a) _p = *_a;                                     \
                ఒకవేళ (_p)                                                 \
                        LIST_REMOVE(name, *_a, _p);                     \
                _p;                                                     \
        })

#నిర్వచించు LIST_CLEAR(name, తల, free_func)       \
        _LIST_CLEAR(name, తల, free_func, UNIQ_T(elem, UNIQ))

/* Clear the list, destroying each element with free_func */
#నిర్వచించు _LIST_CLEAR(name, తల, free_func, elem)        \
        ({                                              \
                typeof(తల) elem;                      \
                అయితే ((elem = LIST_తీయి(name, తల)))   \
                        free_func(elem);                \
                తల;                                   \
        })
