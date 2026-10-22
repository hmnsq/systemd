/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#അന്തര്ഭാവ "forward.h"
#അന്തര്ഭാവ "hashmap.h"

തരനിർവചനം ഘടന OrderedSet OrderedSet;

സ്ഥിതം വരിയിൽ OrderedSet* ordered_set_new(സ്ഥിരം ഘടന hash_ops *ops) {
        തിരികെ (OrderedSet*) ordered_hashmap_new(ops);
}

പൂർണ്ണസംഖ്യ ordered_set_ensure_allocated(OrderedSet **s, സ്ഥിരം ഘടന hash_ops *ops);

പൂർണ്ണസംഖ്യ ordered_set_ensure_put(OrderedSet **s, സ്ഥിരം ഘടന hash_ops *ops, ശൂന്യം *p);

സ്ഥിതം വരിയിൽ ശൂന്യം ordered_set_clear(OrderedSet *s) {
        തിരികെ ordered_hashmap_clear((OrderedHashmap*) s);
}

സ്ഥിതം വരിയിൽ OrderedSet* ordered_set_free(OrderedSet *s) {
        തിരികെ (OrderedSet*) ordered_hashmap_free((OrderedHashmap*) s);
}

സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ ordered_set_contains(OrderedSet *s, സ്ഥിരം ശൂന്യം *p) {
        തിരികെ ordered_hashmap_contains((OrderedHashmap*) s, p);
}

സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ ordered_set_put(OrderedSet *s, ശൂന്യം *p) {
        തിരികെ ordered_hashmap_put((OrderedHashmap*) s, p, p);
}

സ്ഥിതം വരിയിൽ ശൂന്യം *ordered_set_get(OrderedSet *s, സ്ഥിരം ശൂന്യം *p) {
        തിരികെ ordered_hashmap_get((OrderedHashmap*) s, p);
}

സ്ഥിതം വരിയിൽ unsigned ordered_set_size(OrderedSet *s) {
        തിരികെ ordered_hashmap_size((OrderedHashmap*) s);
}

സ്ഥിതം വരിയിൽ bool ordered_set_isempty(OrderedSet *s) {
        തിരികെ ordered_hashmap_isempty((OrderedHashmap*) s);
}

സ്ഥിതം വരിയിൽ bool ordered_set_iterate(OrderedSet *s, Iterator *i, ശൂന്യം **മൂല്യം) {
        തിരികെ ordered_hashmap_iterate((OrderedHashmap*) s, i, മൂല്യം, ശൂന്യ);
}

സ്ഥിതം വരിയിൽ ശൂന്യം* ordered_set_remove(OrderedSet *s, ശൂന്യം *p) {
        തിരികെ ordered_hashmap_remove((OrderedHashmap*) s, p);
}

സ്ഥിതം വരിയിൽ ശൂന്യം* ordered_set_first(OrderedSet *s) {
        തിരികെ ordered_hashmap_first((OrderedHashmap*) s);
}

സ്ഥിതം വരിയിൽ ശൂന്യം* ordered_set_steal_first(OrderedSet *s) {
        തിരികെ ordered_hashmap_steal_first((OrderedHashmap*) s);
}

സ്ഥിതം വരിയിൽ അക്ഷരം** ordered_set_get_strv(OrderedSet *s) {
        തിരികെ _hashmap_get_strv(HASHMAP_BASE((OrderedHashmap*) s));
}

സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ ordered_set_reserve(OrderedSet *s, unsigned entries_add) {
        തിരികെ ordered_hashmap_reserve((OrderedHashmap*) s, entries_add);
}

പൂർണ്ണസംഖ്യ ordered_set_consume(OrderedSet *s, ശൂന്യം *p);

പൂർണ്ണസംഖ്യ ordered_set_put_strdup_full(OrderedSet **s, സ്ഥിരം ഘടന hash_ops *hash_ops, സ്ഥിരം അക്ഷരം *p);
#define ordered_set_put_strdup(s, p) ordered_set_put_strdup_full(s, &string_hash_ops_free, p)
പൂർണ്ണസംഖ്യ ordered_set_put_strdupv_full(OrderedSet **s, സ്ഥിരം ഘടന hash_ops *hash_ops, അക്ഷരം **l);
#define ordered_set_put_strdupv(s, l) ordered_set_put_strdupv_full(s, &string_hash_ops_free, l)
പൂർണ്ണസംഖ്യ ordered_set_put_string_set_full(OrderedSet **s, സ്ഥിരം ഘടന hash_ops *hash_ops, OrderedSet *l);
#define ordered_set_put_string_set(s, l) ordered_set_put_string_set_full(s, &string_hash_ops_free, l)

ശൂന്യം ordered_set_print(FILE *f, സ്ഥിരം അക്ഷരം *field, OrderedSet *s);

#define _ORDERED_SET_FOREACH(e, s, i) \
        വേണ്ടി (Iterator i = ITERATOR_FIRST; ordered_set_iterate((s), &i, (ശൂന്യം**)&(e)); )
#define ORDERED_SET_FOREACH(e, s) \
        _ORDERED_SET_FOREACH(e, s, UNIQ_T(i, UNIQ))

DEFINE_TRIVIAL_CLEANUP_FUNC(OrderedSet*, ordered_set_free);

#define _cleanup_ordered_set_free_ _cleanup_(ordered_set_freep)
