/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ "alloc-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ansi-seq.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string-util.h"

ਖਾਲੀ ansi_seq_parser_ਪੂਰਾ(AnsiSeqParser *p) {
        ਪ੍ਰਤਿਜ੍ਞਾ(p);

        p->string = mfree(p->string);
        p->string_len = 0;
}

ਸਥਿਰ ਖਾਲੀ ansi_seq_parser_begin_string(AnsiSeqParser *p, ਅੱਖਰ introducer) {
        ਪ੍ਰਤਿਜ੍ਞਾ(p);

        p->string_len = 0;
        ਜੇ (p->string)
                p->string[0] = 0;
        p->bad = false;
        p->introducer = introducer;
}

ਸਥਿਰ ਪੂਰਨ ansi_seq_parser_ਧੱਕੋ_string(AnsiSeqParser *p, ਅੱਖਰ c) {
        ਪ੍ਰਤਿਜ੍ਞਾ(p);
        ਪ੍ਰਤਿਜ੍ਞਾ(!p->bad); /* only ever set together with a reset to the ground state, hence never seen here */

        /* Safety check: don't capture unbounded string sequences */
        ਜੇ (p->string_len >= ANSI_SEQ_STRING_MAX)
                ਵਾਪਸ 0; /* too long */

        ਜੇ (p->capture) {
                ਜੇ (!GREEDY_REALLOC(p->string, p->string_len + 2))
                        ਵਾਪਸ -ENOMEM;

                p->string[p->string_len+0] = c;
                p->string[p->string_len+1] = 0;
        }

        p->string_len++;

        ਵਾਪਸ 1; /* added */
}

ਸਥਿਰ ਪੂਰਨ ansi_seq_parser_bad(AnsiSeqParser *p) {
        ਪ੍ਰਤਿਜ੍ਞਾ(p);

        /* A bad sequence has been received, reset state to ground, and remember this */

        p->state = ANSI_SEQ_STATE_GROUND;
        p->bad = true;
        ਵਾਪਸ ANSI_SEQ_EVENT_ABORT;
}

ਪੂਰਨ ansi_seq_parser_feed(AnsiSeqParser *p, ਅੱਖਰ c) {
        uint8_t u = (uint8_t) c;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(p);

        ਚੋਣ (p->state) {

        ਹਾਲਤ ANSI_SEQ_STATE_GROUND:
                ਜੇ (c == 0x1B) { /* ESC */
                        p->state = ANSI_SEQ_STATE_ESC;
                        ansi_seq_parser_begin_string(p, /* introducer= */ 0);
                        ਵਾਪਸ ANSI_SEQ_EVENT_SEQUENCE;
                }

                ਵਾਪਸ ANSI_SEQ_EVENT_ਪਾਠ;

        ਹਾਲਤ ANSI_SEQ_STATE_ESC:
                ਚੋਣ (c) {

                ਹਾਲਤ '[': /* CSI sequence */
                        p->state = ANSI_SEQ_STATE_CSI;
                        ansi_seq_parser_begin_string(p, c);
                        ਵਾਪਸ ANSI_SEQ_EVENT_SEQUENCE;

                ਹਾਲਤ ']': /* OSC */
                ਹਾਲਤ 'P': /* DCS */
                ਹਾਲਤ 'X': /* SOS */
                ਹਾਲਤ '^': /* PM */
                ਹਾਲਤ '_': /* APC */
                        p->state = ANSI_SEQ_STATE_STRING;
                        ansi_seq_parser_begin_string(p, c);
                        ਵਾਪਸ ANSI_SEQ_EVENT_SEQUENCE;

                ਹਾਲਤ 0x40 ... 0x4f: /* Fe sequence, i.e. the 7-bit form of the C1 controls (e.g. ESC M = RI),
                                     * except for those introducing CSI/string sequences, handled above */
                ਹਾਲਤ 0x51 ... 0x57:
                ਹਾਲਤ 0x59 ... 0x5a:
                ਹਾਲਤ 0x5c:
                ਹਾਲਤ 0x60 ... 0x7e: /* Fs sequence */
                ਹਾਲਤ 0x30 ... 0x3f: /* Fp sequence */
                        p->state = ANSI_SEQ_STATE_GROUND;
                        p->introducer = c;
                        ਵਾਪਸ ANSI_SEQ_EVENT_ਸਿਰਾ;

                ਹਾਲਤ 0x20 ... 0x2f: /* nF sequence */
                        p->state = ANSI_SEQ_STATE_NF;
                        ansi_seq_parser_begin_string(p, c);
                        ਵਾਪਸ ANSI_SEQ_EVENT_SEQUENCE;

                ਮੂਲ:
                        /* Anything else is not a valid escape sequence, give up on it */
                        ਵਾਪਸ ansi_seq_parser_bad(p);
                }

        ਹਾਲਤ ANSI_SEQ_STATE_CSI:
                ਜੇ (u >= 0x40 && u <= 0x7E) { /* final byte, the sequence is complete */
                        r = ansi_seq_parser_ਧੱਕੋ_string(p, c);
                        ਜੇ (r < 0)
                                ਵਾਪਸ r;
                        ਜੇ (r == 0)
                                ਵਾਪਸ ansi_seq_parser_bad(p);

                        p->state = ANSI_SEQ_STATE_GROUND;
                        ਵਾਪਸ ANSI_SEQ_EVENT_ਸਿਰਾ;
                }

                ਜੇ (u >= 0x20 && u <= 0x3F) {
                        /* parameter or intermediate byte, the sequence continues */
                        r = ansi_seq_parser_ਧੱਕੋ_string(p, c);
                        ਜੇ (r < 0)
                                ਵਾਪਸ r;
                        ਜੇ (r > 0)
                                ਵਾਪਸ ANSI_SEQ_EVENT_SEQUENCE;

                        /* fall through on too long */
                }

                /* Anything else is not a valid escape sequence, give up on it */
                ਵਾਪਸ ansi_seq_parser_bad(p);

        ਹਾਲਤ ANSI_SEQ_STATE_STRING:
                ਜੇ (c == 0x07) { /* BEL, the single character ST */
                        p->state = ANSI_SEQ_STATE_GROUND;
                        ਵਾਪਸ ANSI_SEQ_EVENT_ਸਿਰਾ;
                }

                ਜੇ (c == 0x1B) { /* Presumably the beginning of the two character ST. Note that we do not
                                  * support the C1 encoding of ST (0x9C), since that's a valid UTF-8
                                  * codepoint and would hence be ambiguous. */
                        p->state = ANSI_SEQ_STATE_STRING_ESC;
                        ਵਾਪਸ ANSI_SEQ_EVENT_SEQUENCE;
                }

                ਜੇ (u >= 0x20U) { /* regular payload */
                        r = ansi_seq_parser_ਧੱਕੋ_string(p, c);
                        ਜੇ (r < 0)
                                ਵਾਪਸ r;
                        ਜੇ (r > 0)
                                ਵਾਪਸ ANSI_SEQ_EVENT_SEQUENCE;

                        /* fall through on too long */
                }

                /* Any other control character: give up on this sequence */
                ਵਾਪਸ ansi_seq_parser_bad(p);

        ਹਾਲਤ ANSI_SEQ_STATE_STRING_ESC:
                ਜੇ (c == '\\') { /* ST is complete, and so is the string sequence */
                        p->state = ANSI_SEQ_STATE_GROUND;
                        ਵਾਪਸ ANSI_SEQ_EVENT_ਸਿਰਾ;
                }

                ਵਾਪਸ ansi_seq_parser_bad(p);

        ਹਾਲਤ ANSI_SEQ_STATE_NF:
                ਜੇ (u >= 0x30 && u <= 0x7E) { /* final byte, the sequence is complete */
                        r = ansi_seq_parser_ਧੱਕੋ_string(p, c);
                        ਜੇ (r < 0)
                                ਵਾਪਸ r;
                        ਜੇ (r == 0)
                                ਵਾਪਸ ansi_seq_parser_bad(p);

                        p->state = ANSI_SEQ_STATE_GROUND;
                        ਵਾਪਸ ANSI_SEQ_EVENT_ਸਿਰਾ;
                }

                ਜੇ (u >= 0x20 && u <= 0x2F) {
                        /* parameter or intermediate byte, the sequence continues */
                        r = ansi_seq_parser_ਧੱਕੋ_string(p, c);
                        ਜੇ (r < 0)
                                ਵਾਪਸ r;
                        ਜੇ (r > 0)
                                ਵਾਪਸ ANSI_SEQ_EVENT_SEQUENCE;

                        /* fall through on too long */
                }

                /* Anything else is not a valid escape sequence, give up on it */
                ਵਾਪਸ ansi_seq_parser_bad(p);

        ਮੂਲ:
                assert_not_reached();
        }
}

ਪੂਰਨ ansi_seq_parser_feed_harder(AnsiSeqParser *p, ਅੱਖਰ c) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(p);

        /* Just like ansi_seq_parser_feed(), but automatically eat up abort events, and resubmit the bad character. */

        ਕਰੋ {
                r = ansi_seq_parser_feed(p, c);
        } ਜਦੋਂ (r == ANSI_SEQ_EVENT_ABORT);

        ਵਾਪਸ r;
}

ਅਚਲ ਅੱਖਰ* ansi_seq_parser_string(ਅਚਲ AnsiSeqParser *p) {
        ਪ੍ਰਤਿਜ੍ਞਾ(p);

        ਜੇ (!p->capture || p->bad || p->introducer == 0)
                ਵਾਪਸ ਨਿਰਾ;

        ਵਾਪਸ strempty(p->string);
}
