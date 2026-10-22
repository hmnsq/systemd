/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل "alloc-util.h"
#شامل "ansi-seq.h"
#شامل "string-util.h"

تهی ansi_seq_parser_done(AnsiSeqParser *p) {
        أكد(p);

        p->string = mfree(p->string);
        p->string_len = 0;
}

ایستا تهی ansi_seq_parser_begin_string(AnsiSeqParser *p, نویسه introducer) {
        أكد(p);

        p->string_len = 0;
        اگر (p->string)
                p->string[0] = 0;
        p->bad = false;
        p->introducer = introducer;
}

ایستا صحیح ansi_seq_parser_افزودن_string(AnsiSeqParser *p, نویسه c) {
        أكد(p);
        أكد(!p->bad); /* only ever set together with a reset to the ground state, hence never seen here */

        /* Safety check: don't capture unbounded string sequences */
        اگر (p->string_len >= ANSI_SEQ_STRING_MAX)
                بازگشت 0; /* too long */

        اگر (p->capture) {
                اگر (!GREEDY_REALLOC(p->string, p->string_len + 2))
                        بازگشت -ENOMEM;

                p->string[p->string_len+0] = c;
                p->string[p->string_len+1] = 0;
        }

        p->string_len++;

        بازگشت 1; /* added */
}

ایستا صحیح ansi_seq_parser_bad(AnsiSeqParser *p) {
        أكد(p);

        /* A bad sequence has been received, reset state to ground, and remember this */

        p->state = ANSI_SEQ_STATE_GROUND;
        p->bad = true;
        بازگشت ANSI_SEQ_EVENT_ABORT;
}

صحیح ansi_seq_parser_feed(AnsiSeqParser *p, نویسه c) {
        uint8_t u = (uint8_t) c;
        صحیح r;

        أكد(p);

        انتخاب (p->state) {

        حالت ANSI_SEQ_STATE_GROUND:
                اگر (c == 0x1B) { /* ESC */
                        p->state = ANSI_SEQ_STATE_ESC;
                        ansi_seq_parser_begin_string(p, /* introducer= */ 0);
                        بازگشت ANSI_SEQ_EVENT_SEQUENCE;
                }

                بازگشت ANSI_SEQ_EVENT_TEXT;

        حالت ANSI_SEQ_STATE_ESC:
                انتخاب (c) {

                حالت '[': /* CSI sequence */
                        p->state = ANSI_SEQ_STATE_CSI;
                        ansi_seq_parser_begin_string(p, c);
                        بازگشت ANSI_SEQ_EVENT_SEQUENCE;

                حالت ']': /* OSC */
                حالت 'P': /* DCS */
                حالت 'X': /* SOS */
                حالت '^': /* PM */
                حالت '_': /* APC */
                        p->state = ANSI_SEQ_STATE_STRING;
                        ansi_seq_parser_begin_string(p, c);
                        بازگشت ANSI_SEQ_EVENT_SEQUENCE;

                حالت 0x40 ... 0x4f: /* Fe sequence, i.e. the 7-bit form of the C1 controls (e.g. ESC M = RI),
                                     * except for those introducing CSI/string sequences, handled above */
                حالت 0x51 ... 0x57:
                حالت 0x59 ... 0x5a:
                حالت 0x5c:
                حالت 0x60 ... 0x7e: /* Fs sequence */
                حالت 0x30 ... 0x3f: /* Fp sequence */
                        p->state = ANSI_SEQ_STATE_GROUND;
                        p->introducer = c;
                        بازگشت ANSI_SEQ_EVENT_END;

                حالت 0x20 ... 0x2f: /* nF sequence */
                        p->state = ANSI_SEQ_STATE_NF;
                        ansi_seq_parser_begin_string(p, c);
                        بازگشت ANSI_SEQ_EVENT_SEQUENCE;

                پیشفرض:
                        /* Anything else is not a valid escape sequence, give up on it */
                        بازگشت ansi_seq_parser_bad(p);
                }

        حالت ANSI_SEQ_STATE_CSI:
                اگر (u >= 0x40 && u <= 0x7E) { /* final byte, the sequence is complete */
                        r = ansi_seq_parser_افزودن_string(p, c);
                        اگر (r < 0)
                                بازگشت r;
                        اگر (r == 0)
                                بازگشت ansi_seq_parser_bad(p);

                        p->state = ANSI_SEQ_STATE_GROUND;
                        بازگشت ANSI_SEQ_EVENT_END;
                }

                اگر (u >= 0x20 && u <= 0x3F) {
                        /* parameter or intermediate byte, the sequence continues */
                        r = ansi_seq_parser_افزودن_string(p, c);
                        اگر (r < 0)
                                بازگشت r;
                        اگر (r > 0)
                                بازگشت ANSI_SEQ_EVENT_SEQUENCE;

                        /* fall through on too long */
                }

                /* Anything else is not a valid escape sequence, give up on it */
                بازگشت ansi_seq_parser_bad(p);

        حالت ANSI_SEQ_STATE_STRING:
                اگر (c == 0x07) { /* BEL, the single character ST */
                        p->state = ANSI_SEQ_STATE_GROUND;
                        بازگشت ANSI_SEQ_EVENT_END;
                }

                اگر (c == 0x1B) { /* Presumably the beginning of the two character ST. Note that we do not
                                  * support the C1 encoding of ST (0x9C), since that's a valid UTF-8
                                  * codepoint and would hence be ambiguous. */
                        p->state = ANSI_SEQ_STATE_STRING_ESC;
                        بازگشت ANSI_SEQ_EVENT_SEQUENCE;
                }

                اگر (u >= 0x20U) { /* regular payload */
                        r = ansi_seq_parser_افزودن_string(p, c);
                        اگر (r < 0)
                                بازگشت r;
                        اگر (r > 0)
                                بازگشت ANSI_SEQ_EVENT_SEQUENCE;

                        /* fall through on too long */
                }

                /* Any other control character: give up on this sequence */
                بازگشت ansi_seq_parser_bad(p);

        حالت ANSI_SEQ_STATE_STRING_ESC:
                اگر (c == '\\') { /* ST is complete, and so is the string sequence */
                        p->state = ANSI_SEQ_STATE_GROUND;
                        بازگشت ANSI_SEQ_EVENT_END;
                }

                بازگشت ansi_seq_parser_bad(p);

        حالت ANSI_SEQ_STATE_NF:
                اگر (u >= 0x30 && u <= 0x7E) { /* final byte, the sequence is complete */
                        r = ansi_seq_parser_افزودن_string(p, c);
                        اگر (r < 0)
                                بازگشت r;
                        اگر (r == 0)
                                بازگشت ansi_seq_parser_bad(p);

                        p->state = ANSI_SEQ_STATE_GROUND;
                        بازگشت ANSI_SEQ_EVENT_END;
                }

                اگر (u >= 0x20 && u <= 0x2F) {
                        /* parameter or intermediate byte, the sequence continues */
                        r = ansi_seq_parser_افزودن_string(p, c);
                        اگر (r < 0)
                                بازگشت r;
                        اگر (r > 0)
                                بازگشت ANSI_SEQ_EVENT_SEQUENCE;

                        /* fall through on too long */
                }

                /* Anything else is not a valid escape sequence, give up on it */
                بازگشت ansi_seq_parser_bad(p);

        پیشفرض:
                assert_not_reached();
        }
}

صحیح ansi_seq_parser_feed_harder(AnsiSeqParser *p, نویسه c) {
        صحیح r;

        أكد(p);

        /* Just like ansi_seq_parser_feed(), but automatically eat up abort events, and resubmit the bad character. */

        انجامبده {
                r = ansi_seq_parser_feed(p, c);
        } وقتی (r == ANSI_SEQ_EVENT_ABORT);

        بازگشت r;
}

ثابت نویسه* ansi_seq_parser_string(ثابت AnsiSeqParser *p) {
        أكد(p);

        اگر (!p->capture || p->bad || p->introducer == 0)
                بازگشت پوچ;

        بازگشت strempty(p->string);
}
