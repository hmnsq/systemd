/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাৱ "alloc-util.h"
#অন্তর্ভাৱ "ansi-seq.h"
#অন্তর্ভাৱ "string-util.h"

শূন্য ansi_seq_parser_done(AnsiSeqParser *p) {
        প্রতিজ্ঞা(p);

        p->string = mfree(p->string);
        p->string_len = 0;
}

স্থির শূন্য ansi_seq_parser_begin_string(AnsiSeqParser *p, ৱর্ণ introducer) {
        প্রতিজ্ঞা(p);

        p->string_len = 0;
        যদি (p->string)
                p->string[0] = 0;
        p->bad = false;
        p->introducer = introducer;
}

স্থির পূর্ণাঙ্ক ansi_seq_parser_ঠেলো_string(AnsiSeqParser *p, ৱর্ণ c) {
        প্রতিজ্ঞা(p);
        প্রতিজ্ঞা(!p->bad); /* only ever set together with a reset to the ground state, hence never seen here */

        /* Safety check: don't capture unbounded string sequences */
        যদি (p->string_len >= ANSI_SEQ_STRING_MAX)
                ঘূৰাই 0; /* too long */

        যদি (p->capture) {
                যদি (!GREEDY_REALLOC(p->string, p->string_len + 2))
                        ঘূৰাই -ENOMEM;

                p->string[p->string_len+0] = c;
                p->string[p->string_len+1] = 0;
        }

        p->string_len++;

        ঘূৰাই 1; /* added */
}

স্থির পূর্ণাঙ্ক ansi_seq_parser_bad(AnsiSeqParser *p) {
        প্রতিজ্ঞা(p);

        /* A bad sequence has been received, reset state to ground, and remember this */

        p->state = ANSI_SEQ_STATE_GROUND;
        p->bad = true;
        ঘূৰাই ANSI_SEQ_EVENT_ABORT;
}

পূর্ণাঙ্ক ansi_seq_parser_feed(AnsiSeqParser *p, ৱর্ণ c) {
        uint8_t u = (uint8_t) c;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(p);

        ৱিকল্প (p->state) {

        পক্ষ ANSI_SEQ_STATE_GROUND:
                যদি (c == 0x1B) { /* ESC */
                        p->state = ANSI_SEQ_STATE_ESC;
                        ansi_seq_parser_begin_string(p, /* introducer= */ 0);
                        ঘূৰাই ANSI_SEQ_EVENT_SEQUENCE;
                }

                ঘূৰাই ANSI_SEQ_EVENT_TEXT;

        পক্ষ ANSI_SEQ_STATE_ESC:
                ৱিকল্প (c) {

                পক্ষ '[': /* CSI sequence */
                        p->state = ANSI_SEQ_STATE_CSI;
                        ansi_seq_parser_begin_string(p, c);
                        ঘূৰাই ANSI_SEQ_EVENT_SEQUENCE;

                পক্ষ ']': /* OSC */
                পক্ষ 'P': /* DCS */
                পক্ষ 'X': /* SOS */
                পক্ষ '^': /* PM */
                পক্ষ '_': /* APC */
                        p->state = ANSI_SEQ_STATE_STRING;
                        ansi_seq_parser_begin_string(p, c);
                        ঘূৰাই ANSI_SEQ_EVENT_SEQUENCE;

                পক্ষ 0x40 ... 0x4f: /* Fe sequence, i.e. the 7-bit form of the C1 controls (e.g. ESC M = RI),
                                     * except for those introducing CSI/string sequences, handled above */
                পক্ষ 0x51 ... 0x57:
                পক্ষ 0x59 ... 0x5a:
                পক্ষ 0x5c:
                পক্ষ 0x60 ... 0x7e: /* Fs sequence */
                পক্ষ 0x30 ... 0x3f: /* Fp sequence */
                        p->state = ANSI_SEQ_STATE_GROUND;
                        p->introducer = c;
                        ঘূৰাই ANSI_SEQ_EVENT_END;

                পক্ষ 0x20 ... 0x2f: /* nF sequence */
                        p->state = ANSI_SEQ_STATE_NF;
                        ansi_seq_parser_begin_string(p, c);
                        ঘূৰাই ANSI_SEQ_EVENT_SEQUENCE;

                স্ৱভাৱ:
                        /* Anything else is not a valid escape sequence, give up on it */
                        ঘূৰাই ansi_seq_parser_bad(p);
                }

        পক্ষ ANSI_SEQ_STATE_CSI:
                যদি (u >= 0x40 && u <= 0x7E) { /* final byte, the sequence is complete */
                        r = ansi_seq_parser_ঠেলো_string(p, c);
                        যদি (r < 0)
                                ঘূৰাই r;
                        যদি (r == 0)
                                ঘূৰাই ansi_seq_parser_bad(p);

                        p->state = ANSI_SEQ_STATE_GROUND;
                        ঘূৰাই ANSI_SEQ_EVENT_END;
                }

                যদি (u >= 0x20 && u <= 0x3F) {
                        /* parameter or intermediate byte, the sequence continues */
                        r = ansi_seq_parser_ঠেলো_string(p, c);
                        যদি (r < 0)
                                ঘূৰাই r;
                        যদি (r > 0)
                                ঘূৰাই ANSI_SEQ_EVENT_SEQUENCE;

                        /* fall through on too long */
                }

                /* Anything else is not a valid escape sequence, give up on it */
                ঘূৰাই ansi_seq_parser_bad(p);

        পক্ষ ANSI_SEQ_STATE_STRING:
                যদি (c == 0x07) { /* BEL, the single character ST */
                        p->state = ANSI_SEQ_STATE_GROUND;
                        ঘূৰাই ANSI_SEQ_EVENT_END;
                }

                যদি (c == 0x1B) { /* Presumably the beginning of the two character ST. Note that we do not
                                  * support the C1 encoding of ST (0x9C), since that's a valid UTF-8
                                  * codepoint and would hence be ambiguous. */
                        p->state = ANSI_SEQ_STATE_STRING_ESC;
                        ঘূৰাই ANSI_SEQ_EVENT_SEQUENCE;
                }

                যদি (u >= 0x20U) { /* regular payload */
                        r = ansi_seq_parser_ঠেলো_string(p, c);
                        যদি (r < 0)
                                ঘূৰাই r;
                        যদি (r > 0)
                                ঘূৰাই ANSI_SEQ_EVENT_SEQUENCE;

                        /* fall through on too long */
                }

                /* Any other control character: give up on this sequence */
                ঘূৰাই ansi_seq_parser_bad(p);

        পক্ষ ANSI_SEQ_STATE_STRING_ESC:
                যদি (c == '\\') { /* ST is complete, and so is the string sequence */
                        p->state = ANSI_SEQ_STATE_GROUND;
                        ঘূৰাই ANSI_SEQ_EVENT_END;
                }

                ঘূৰাই ansi_seq_parser_bad(p);

        পক্ষ ANSI_SEQ_STATE_NF:
                যদি (u >= 0x30 && u <= 0x7E) { /* final byte, the sequence is complete */
                        r = ansi_seq_parser_ঠেলো_string(p, c);
                        যদি (r < 0)
                                ঘূৰাই r;
                        যদি (r == 0)
                                ঘূৰাই ansi_seq_parser_bad(p);

                        p->state = ANSI_SEQ_STATE_GROUND;
                        ঘূৰাই ANSI_SEQ_EVENT_END;
                }

                যদি (u >= 0x20 && u <= 0x2F) {
                        /* parameter or intermediate byte, the sequence continues */
                        r = ansi_seq_parser_ঠেলো_string(p, c);
                        যদি (r < 0)
                                ঘূৰাই r;
                        যদি (r > 0)
                                ঘূৰাই ANSI_SEQ_EVENT_SEQUENCE;

                        /* fall through on too long */
                }

                /* Anything else is not a valid escape sequence, give up on it */
                ঘূৰাই ansi_seq_parser_bad(p);

        স্ৱভাৱ:
                assert_not_reached();
        }
}

পূর্ণাঙ্ক ansi_seq_parser_feed_harder(AnsiSeqParser *p, ৱর্ণ c) {
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(p);

        /* Just like ansi_seq_parser_feed(), but automatically eat up abort events, and resubmit the bad character. */

        কৰা {
                r = ansi_seq_parser_feed(p, c);
        } যেতিয়ালৈ (r == ANSI_SEQ_EVENT_ABORT);

        ঘূৰাই r;
}

ধ্রুৱ ৱর্ণ* ansi_seq_parser_string(ধ্রুৱ AnsiSeqParser *p) {
        প্রতিজ্ঞা(p);

        যদি (!p->capture || p->bad || p->introducer == 0)
                ঘূৰাই রিক্ত;

        ঘূৰাই strempty(p->string);
}
