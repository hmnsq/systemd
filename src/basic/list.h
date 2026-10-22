/* SPDX-License-Identifier: LGPL-2.1-or-later */
#رهنمود once

#ضمن "forward.h"

/* The head of the linked list. Use this in the structure that shall
 * contain the head of the linked list */
#تعریف_کن LIST_سر(t,name)                                               \
        t *name

/* The pointers in the linked list's items. Use this in the item structure */
#تعریف_کن LIST_FIELDS(t,name)                                             \
        t *name##_next, *name##_prev

/* Initialize the list's head */
#تعریف_کن LIST_سر_INIT(سر)                                            \
        افعل {                                                            \
                (سر) = عدم;                                          \
        } طالما (false)

/* Initialize a list item */
#تعریف_کن LIST_INIT(name,قلم)                                            \
        افعل {                                                            \
                typeof(*(قلم)) *قلم_2 = (قلم);                        \
                أكد(قلم_2);                                          \
                قلم_2->name##_prev = قلم_2->name##_next = عدم;         \
        } طالما (false)

/* Prepend an item to the list */
#تعریف_کن LIST_PREPEND(name,سر,قلم)                                    \
        ({                                                              \
                typeof(*(سر)) **سر_2 = &(سر), *قلم_2 = (قلم);     \
                أكد(قلم_2);                                          \
                إذا ((قلم_2->name##_next = *سر_2))                      \
                        قلم_2->name##_next->name##_prev = قلم_2;        \
                قلم_2->name##_prev = عدم;                              \
                *سر_2 = قلم_2;                                         \
                قلم_2;                                                  \
        })

/* Append an item to the list */
#تعریف_کن LIST_APPEND(name,سر,قلم)                                     \
        ({                                                              \
                typeof(*(سر)) **_hhead = &(سر), *دم;             \
                دم = LIST_پیدا_کن_دم(name, *_hhead);                  \
                LIST_درج_AFTER(name, *_hhead, دم, قلم);          \
        })

/* Remove an item from the list */
#تعریف_کن LIST_REMOVE(name,سر,قلم)                                     \
        ({                                                              \
                typeof(*(سر)) **سر_2 = &(سر), *قلم_2 = (قلم);     \
                أكد(قلم_2);                                          \
                إذا (قلم_2->name##_next)                                 \
                        قلم_2->name##_next->name##_prev = قلم_2->name##_prev; \
                إذا (قلم_2->name##_prev)                                 \
                        قلم_2->name##_prev->name##_next = قلم_2->name##_next; \
                وإلا {                                                  \
                        أكد(*سر_2 == قلم_2);                        \
                        *سر_2 = قلم_2->name##_next;                    \
                }                                                       \
                قلم_2->name##_next = قلم_2->name##_prev = عدم;         \
                قلم_2;                                                  \
        })

/* Find the head of the list */
#تعریف_کن LIST_پیدا_کن_سر(name,قلم)                                       \
        ({                                                              \
                typeof(*(قلم)) *قلم_2 = (قلم);                        \
                طالما (قلم_2 && قلم_2->name##_prev)                     \
                        قلم_2 = قلم_2->name##_prev;                     \
                قلم_2;                                                  \
        })

/* Find the tail of the list */
#تعریف_کن LIST_پیدا_کن_دم(name,قلم)                                       \
        ({                                                              \
                typeof(*(قلم)) *قلم_2 = (قلم);                        \
                طالما (قلم_2 && قلم_2->name##_next)                     \
                        قلم_2 = قلم_2->name##_next;                     \
                قلم_2;                                                  \
        })

/* Insert an item after another one (a = where, b = what) */
#تعریف_کن LIST_درج_AFTER(name,سر,a,b)                                \
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
#تعریف_کن LIST_درج_BEFORE(name,سر,a,b)                               \
        ({                                                              \
                typeof(*(سر)) **سر_2 = &(سر), *_a = (a), *_b = (b); \
                أكد(_b);                                             \
                إذا (!_a) {                                              \
                        إذا (!*سر_2) {                                  \
                                _b->name##_next = عدم;                 \
                                _b->name##_prev = عدم;                 \
                                *سر_2 = _b;                            \
                        } وإلا {                                        \
                                typeof(*(سر)) *دم = (سر);        \
                                طالما (دم->name##_next)              \
                                        دم = دم->name##_next;     \
                                _b->name##_next = عدم;                 \
                                _b->name##_prev = دم;                \
                                دم->name##_next = _b;                \
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

#تعریف_کن LIST_JUST_US(name, قلم)                                        \
        ({                                                              \
                typeof(*(قلم)) *قلم_2 = (قلم);                        \
                !(قلم_2)->name##_prev && !(قلم_2)->name##_next;         \
        })

/* The type of the iterator 'i' is automatically determined by the type of 'head', and declared in the
 * loop. Hence, do not declare the same variable in the outer scope. Sometimes, we set 'head' through
 * hashmap_get(). In that case, you need to explicitly cast the result. */
#تعریف_کن LIST_FOREACH_WITH_بعدی(name,i,n,سر)                           \
        لكل (typeof(*(سر)) *n, *i = (سر); i && (n = i->name##_next, true); i = n)

#تعریف_کن LIST_FOREACH(name,i,سر)                                       \
        LIST_FOREACH_WITH_بعدی(name, i, UNIQ_T(n, UNIQ), سر)

#تعریف_کن _LIST_FOREACH_WITH_PREV(name,i,p,شروع)                         \
        لكل (typeof(*(شروع)) *p, *i = (شروع); i && (p = i->name##_prev, true); i = p)

#تعریف_کن LIST_FOREACH_BACKWARDS(name,i,شروع)                            \
        _LIST_FOREACH_WITH_PREV(name, i, UNIQ_T(p, UNIQ), شروع)

/* Iterate through all the members of the list p is included in, but skip over p */
#تعریف_کن LIST_FOREACH_OTHERS(name,i,p)                                   \
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
#تعریف_کن LIST_LOOP_BUT_ONE(name,i,سر,p)                                \
        لكل (typeof(*(p)) *i = (p)->name##_next ? (p)->name##_next : (سر); \
             i != (p);                                                  \
             i = i->name##_next ? i->name##_next : (سر))

/* Join two lists tail to head: a->b, c->d to a->b->c->d and de-initialise second list */
#تعریف_کن LIST_JOIN(name,a,b)                                             \
        ({                                                              \
                أكد(b);                                              \
                إذا (!(a))                                               \
                        (a) = (b);                                      \
                وإلا {                                                  \
                        typeof(*(a)) *سر_2 = (b), *دم;              \
                        دم = LIST_پیدا_کن_دم(name, (a));              \
                        دم->name##_next = سر_2;                     \
                        سر_2->name##_prev = دم;                     \
                }                                                       \
                (b) = عدم;                                             \
                a;                                                      \
        })

#تعریف_کن LIST_برداشت(name, a)                                               \
        ({                                                              \
                typeof(a)* _a = &(a);                                   \
                typeof(a) _p = *_a;                                     \
                إذا (_p)                                                 \
                        LIST_REMOVE(name, *_a, _p);                     \
                _p;                                                     \
        })

#تعریف_کن LIST_CLEAR(name, سر, free_func)       \
        _LIST_CLEAR(name, سر, free_func, UNIQ_T(elem, UNIQ))

/* Clear the list, destroying each element with free_func */
#تعریف_کن _LIST_CLEAR(name, سر, free_func, elem)        \
        ({                                              \
                typeof(سر) elem;                      \
                طالما ((elem = LIST_برداشت(name, سر)))   \
                        free_func(elem);                \
                سر;                                   \
        })
