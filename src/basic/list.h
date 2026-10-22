/* SPDX-License-Identifier: LGPL-2.1-or-later */
#هدايت once

#ضمن "forward.h"

/* The head of the linked list. Use this in the structure that shall
 * contain the head of the linked list */
#وصف_ڪريو LIST_سر(t,name)                                               \
        t *name

/* The pointers in the linked list's items. Use this in the item structure */
#وصف_ڪريو LIST_FIELDS(t,name)                                             \
        t *name##_next, *name##_prev

/* Initialize the list's head */
#وصف_ڪريو LIST_سر_INIT(سر)                                            \
        افعل {                                                            \
                (سر) = عدم;                                          \
        } طالما (false)

/* Initialize a list item */
#وصف_ڪريو LIST_INIT(name,شي)                                            \
        افعل {                                                            \
                typeof(*(شي)) *شي_2 = (شي);                        \
                أكد(شي_2);                                          \
                شي_2->name##_prev = شي_2->name##_next = عدم;         \
        } طالما (false)

/* Prepend an item to the list */
#وصف_ڪريو LIST_PREPEND(name,سر,شي)                                    \
        ({                                                              \
                typeof(*(سر)) **سر_2 = &(سر), *شي_2 = (شي);     \
                أكد(شي_2);                                          \
                إذا ((شي_2->name##_next = *سر_2))                      \
                        شي_2->name##_next->name##_prev = شي_2;        \
                شي_2->name##_prev = عدم;                              \
                *سر_2 = شي_2;                                         \
                شي_2;                                                  \
        })

/* Append an item to the list */
#وصف_ڪريو LIST_APPEND(name,سر,شي)                                     \
        ({                                                              \
                typeof(*(سر)) **_hhead = &(سر), *پڇ;             \
                پڇ = LIST_ڳوليو_پڇ(name, *_hhead);                  \
                LIST_وجهو_AFTER(name, *_hhead, پڇ, شي);          \
        })

/* Remove an item from the list */
#وصف_ڪريو LIST_REMOVE(name,سر,شي)                                     \
        ({                                                              \
                typeof(*(سر)) **سر_2 = &(سر), *شي_2 = (شي);     \
                أكد(شي_2);                                          \
                إذا (شي_2->name##_next)                                 \
                        شي_2->name##_next->name##_prev = شي_2->name##_prev; \
                إذا (شي_2->name##_prev)                                 \
                        شي_2->name##_prev->name##_next = شي_2->name##_next; \
                وإلا {                                                  \
                        أكد(*سر_2 == شي_2);                        \
                        *سر_2 = شي_2->name##_next;                    \
                }                                                       \
                شي_2->name##_next = شي_2->name##_prev = عدم;         \
                شي_2;                                                  \
        })

/* Find the head of the list */
#وصف_ڪريو LIST_ڳوليو_سر(name,شي)                                       \
        ({                                                              \
                typeof(*(شي)) *شي_2 = (شي);                        \
                طالما (شي_2 && شي_2->name##_prev)                     \
                        شي_2 = شي_2->name##_prev;                     \
                شي_2;                                                  \
        })

/* Find the tail of the list */
#وصف_ڪريو LIST_ڳوليو_پڇ(name,شي)                                       \
        ({                                                              \
                typeof(*(شي)) *شي_2 = (شي);                        \
                طالما (شي_2 && شي_2->name##_next)                     \
                        شي_2 = شي_2->name##_next;                     \
                شي_2;                                                  \
        })

/* Insert an item after another one (a = where, b = what) */
#وصف_ڪريو LIST_وجهو_AFTER(name,سر,a,b)                                \
        ({                                                              \
                typeof(*(سر)) **سر_2 = &(سر), *_a = (a), *_b = (b); \
                أكد(_b);                                             \
                إذا (!_a) {                                              \
                        إذا ((_b->name##_next = *سر_2))                 \
                                _b->name##_next->name##_prev = _b;      \
                        _b->name##_prev = عدم;                         \
                        *سر_2 = _b;                                    \
                } وإلا {                                                \
                        إذا ((_b->name##_next = _a->name##_next))        \
                                _b->name##_next->name##_prev = _b;      \
                        _b->name##_prev = _a;                           \
                        _a->name##_next = _b;                           \
                }                                                       \
                _b;                                                     \
        })

/* Insert an item before another one (a = where, b = what) */
#وصف_ڪريو LIST_وجهو_BEFORE(name,سر,a,b)                               \
        ({                                                              \
                typeof(*(سر)) **سر_2 = &(سر), *_a = (a), *_b = (b); \
                أكد(_b);                                             \
                إذا (!_a) {                                              \
                        إذا (!*سر_2) {                                  \
                                _b->name##_next = عدم;                 \
                                _b->name##_prev = عدم;                 \
                                *سر_2 = _b;                            \
                        } وإلا {                                        \
                                typeof(*(سر)) *پڇ = (سر);        \
                                طالما (پڇ->name##_next)              \
                                        پڇ = پڇ->name##_next;     \
                                _b->name##_next = عدم;                 \
                                _b->name##_prev = پڇ;                \
                                پڇ->name##_next = _b;                \
                        }                                               \
                } وإلا {                                                \
                        إذا ((_b->name##_prev = _a->name##_prev))        \
                                _b->name##_prev->name##_next = _b;      \
                        وإلا                                            \
                                *سر_2 = _b;                            \
                        _b->name##_next = _a;                           \
                        _a->name##_prev = _b;                           \
                }                                                       \
                _b;                                                     \
        })

#وصف_ڪريو LIST_JUST_US(name, شي)                                        \
        ({                                                              \
                typeof(*(شي)) *شي_2 = (شي);                        \
                !(شي_2)->name##_prev && !(شي_2)->name##_next;         \
        })

/* The type of the iterator 'i' is automatically determined by the type of 'head', and declared in the
 * loop. Hence, do not declare the same variable in the outer scope. Sometimes, we set 'head' through
 * hashmap_get(). In that case, you need to explicitly cast the result. */
#وصف_ڪريو LIST_FOREACH_WITH_ايندڙ(name,i,n,سر)                           \
        لكل (typeof(*(سر)) *n, *i = (سر); i && (n = i->name##_next, true); i = n)

#وصف_ڪريو LIST_FOREACH(name,i,سر)                                       \
        LIST_FOREACH_WITH_ايندڙ(name, i, UNIQ_T(n, UNIQ), سر)

#وصف_ڪريو _LIST_FOREACH_WITH_PREV(name,i,p,شروعات)                         \
        لكل (typeof(*(شروعات)) *p, *i = (شروعات); i && (p = i->name##_prev, true); i = p)

#وصف_ڪريو LIST_FOREACH_BACKWARDS(name,i,شروعات)                            \
        _LIST_FOREACH_WITH_PREV(name, i, UNIQ_T(p, UNIQ), شروعات)

/* Iterate through all the members of the list p is included in, but skip over p */
#وصف_ڪريو LIST_FOREACH_OTHERS(name,i,p)                                   \
        لكل (typeof(*(p)) *_p = (p), *i = ({                            \
                                typeof(*_p) *_j = _p;                   \
                                طالما (_j && _j->name##_prev)           \
                                        _j = _j->name##_prev;           \
                                إذا (_j == _p)                           \
                                        _j = _p->name##_next;           \
                                _j;                                     \
                        });                                             \
             i;                                                         \
             i = i->name##_next == _p ? _p->name##_next : i->name##_next)

/* Loop starting from p->next until p->prev. p can be adjusted meanwhile. */
#وصف_ڪريو LIST_LOOP_BUT_ONE(name,i,سر,p)                                \
        لكل (typeof(*(p)) *i = (p)->name##_next ? (p)->name##_next : (سر); \
             i != (p);                                                  \
             i = i->name##_next ? i->name##_next : (سر))

/* Join two lists tail to head: a->b, c->d to a->b->c->d and de-initialise second list */
#وصف_ڪريو LIST_JOIN(name,a,b)                                             \
        ({                                                              \
                أكد(b);                                              \
                إذا (!(a))                                               \
                        (a) = (b);                                      \
                وإلا {                                                  \
                        typeof(*(a)) *سر_2 = (b), *پڇ;              \
                        پڇ = LIST_ڳوليو_پڇ(name, (a));              \
                        پڇ->name##_next = سر_2;                     \
                        سر_2->name##_prev = پڇ;                     \
                }                                                       \
                (b) = عدم;                                             \
                a;                                                      \
        })

#وصف_ڪريو LIST_ڪڍو(name, a)                                               \
        ({                                                              \
                typeof(a)* _a = &(a);                                   \
                typeof(a) _p = *_a;                                     \
                إذا (_p)                                                 \
                        LIST_REMOVE(name, *_a, _p);                     \
                _p;                                                     \
        })

#وصف_ڪريو LIST_CLEAR(name, سر, free_func)       \
        _LIST_CLEAR(name, سر, free_func, UNIQ_T(elem, UNIQ))

/* Clear the list, destroying each element with free_func */
#وصف_ڪريو _LIST_CLEAR(name, سر, free_func, elem)        \
        ({                                              \
                typeof(سر) elem;                      \
                طالما ((elem = LIST_ڪڍو(name, سر)))   \
                        free_func(elem);                \
                سر;                                   \
        })
