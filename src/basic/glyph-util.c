/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ <stdlib.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "env-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "glyph-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "locale-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "strv.h"

bool emoji_enabled(ਖਾਲੀ) {
        ਸਥਿਰ ਪੂਰਨ cached_emoji_enabled = -1;

        ਜੇ (cached_emoji_enabled < 0) {
                ਪੂਰਨ val = getenv_bool("SYSTEMD_EMOJI");
                ਜੇ (val >= 0)
                        ਵਾਪਸ (cached_emoji_enabled = val);

                ਅਚਲ ਅੱਖਰ *ਪਦ = getenv("TERM");
                ਜੇ (!ਪਦ || STR_ਵਿੱਚ_ਸੈੱਟ(ਪਦ, "dumb", "linux"))
                        ਵਾਪਸ (cached_emoji_enabled = false);

                cached_emoji_enabled = is_locale_utf8();
        }

        ਵਾਪਸ cached_emoji_enabled;
}

ਅਚਲ ਅੱਖਰ* glyph_ਭਰਿਆ(Glyph ਕੋਡ, bool force_utf) {

        /* A list of a number of interesting unicode glyphs we can use to decorate our output. It's probably wise to be
         * conservative here, and primarily stick to the glyphs defined in the eurlatgr font, so that display still
         * works reasonably well on the Linux console. For details see:
         *
         * http://git.altlinux.org/people/legion/packages/kbd.git?p=kbd.git;a=blob;f=data/consolefonts/README.eurlatgr
         */

        ਸਥਿਰ ਅਚਲ ਅੱਖਰ* ਅਚਲ draw_ਸਾਰਣੀ[2][_GLYPH_MAX] = {
                /* ASCII fallback */
                [false] = {
                        [GLYPH_SPACE]                   = " ",
                        [GLYPH_ਰੁੱਖ_VERTICAL]           = "| ",
                        [GLYPH_ਰੁੱਖ_BRANCH]             = "|-",
                        [GLYPH_ਰੁੱਖ_ਸੱਜਾ]              = "`-",
                        [GLYPH_ਰੁੱਖ_SPACE]              = "  ",
                        [GLYPH_ਰੁੱਖ_ਸਿਖਰ]                = ",-",
                        [GLYPH_VERTICAL_DOTTED]         = ":",
                        [GLYPH_HORIZONTAL_DOTTED]       = "-",
                        [GLYPH_HORIZONTAL_FAT]          = "=",
                        [GLYPH_TRIANGULAR_BULLET]       = ">",
                        [GLYPH_BLACK_CIRCLE]            = "*",
                        [GLYPH_WHITE_CIRCLE]            = "*",
                        [GLYPH_MULTIPLICATION_SIGN]     = "x",
                        [GLYPH_CIRCLE_ARROW]            = "*",
                        [GLYPH_BULLET]                  = "*",
                        [GLYPH_MU]                      = "u",
                        [GLYPH_CHECK_MARK]              = "+",
                        [GLYPH_CROSS_MARK]              = "-",
                        [GLYPH_LIGHT_SHADE]             = "-",
                        [GLYPH_DARK_SHADE]              = "X",
                        [GLYPH_ਭਰਿਆ_ਖੰਡ]              = "#",
                        [GLYPH_SIGMA]                   = "S",
                        [GLYPH_ARROW_ਉੱਪਰ]                = "^",
                        [GLYPH_ARROW_ਥੱਲੇ]              = "v",
                        [GLYPH_ARROW_ਖੱਬਾ]              = "<-",
                        [GLYPH_ARROW_ਸੱਜਾ]             = "->",
                        [GLYPH_ELLIPSIS]                = "...",
                        [GLYPH_EXTERNAL_LINK]           = "[LNK]",
                        [GLYPH_ECSTATIC_SMILEY]         = ":-]",
                        [GLYPH_HAPPY_SMILEY]            = ":-}",
                        [GLYPH_SLIGHTLY_HAPPY_SMILEY]   = ":-)",
                        [GLYPH_NEUTRAL_SMILEY]          = ":-|",
                        [GLYPH_SLIGHTLY_UNHAPPY_SMILEY] = ":-(",
                        [GLYPH_UNHAPPY_SMILEY]          = ":-{",
                        [GLYPH_DEPRESSED_SMILEY]        = ":-[",
                        [GLYPH_ਤਾਲਾ_AND_ਕੁੰਜੀ]            = "o-,",
                        [GLYPH_TOUCH]                   = "O=",    /* Yeah, not very convincing, can you do it better? */
                        [GLYPH_RECYCLING]               = "~",
                        [GLYPH_DOWNLOAD]                = "\\",
                        [GLYPH_SPARKLES]                = "*",
                        [GLYPH_ਨੀਵਾਂ_BATTERY]             = "!",
                        [GLYPH_WARNING_SIGN]            = "!",
                        [GLYPH_COMPUTER_DISK]           = "o",
                        [GLYPH_WORLD]                   = "W",
                        [GLYPH_RED_CIRCLE]              = "o",
                        [GLYPH_ORANGE_CIRCLE]           = "o",
                        [GLYPH_YELLOW_CIRCLE]           = "o",
                        [GLYPH_BLUE_CIRCLE]             = "o",
                        [GLYPH_GREEN_CIRCLE]            = "o",
                        [GLYPH_SUPERHERO]               = "S",
                        [GLYPH_PUMPKIN]                 = "P",
                        [GLYPH_IDCARD]                  = "@",
                        [GLYPH_HOME]                    = "^",
                        [GLYPH_ROCKET]                  = "^",
                        [GLYPH_BROOM]                   = "/",
                        [GLYPH_KEYBOARD]                = "K",
                        [GLYPH_CLOCK]                   = "O",
                        [GLYPH_LABEL]                   = "L",
                        [GLYPH_SHELL]                   = "$",
                },

                /* UTF-8 */
                [true] = {
                        /* This exists to allow more consistent handling of optional whitespace */
                        [GLYPH_SPACE]                   = " ",

                        /* The following are multiple glyphs in both ASCII and in UNICODE */
                        [GLYPH_ਰੁੱਖ_VERTICAL]           = UTF8("│ "),
                        [GLYPH_ਰੁੱਖ_BRANCH]             = UTF8("├─"),
                        [GLYPH_ਰੁੱਖ_ਸੱਜਾ]              = UTF8("└─"),
                        [GLYPH_ਰੁੱਖ_SPACE]              = UTF8("  "),
                        [GLYPH_ਰੁੱਖ_ਸਿਖਰ]                = UTF8("┌─"),

                        /* Single glyphs in both cases */
                        [GLYPH_VERTICAL_DOTTED]         = UTF8("┆"),
                        [GLYPH_HORIZONTAL_DOTTED]       = UTF8("┄"),
                        [GLYPH_HORIZONTAL_FAT]          = UTF8("━"),
                        [GLYPH_TRIANGULAR_BULLET]       = UTF8("‣"),
                        [GLYPH_BLACK_CIRCLE]            = UTF8("●"),
                        [GLYPH_WHITE_CIRCLE]            = UTF8("○"),
                        [GLYPH_MULTIPLICATION_SIGN]     = UTF8("×"),
                        [GLYPH_CIRCLE_ARROW]            = UTF8("↻"),
                        [GLYPH_BULLET]                  = UTF8("•"),
                        [GLYPH_MU]                      = UTF8("μ"),       /* actually called: GREEK SMALL LETTER MU */
                        [GLYPH_CHECK_MARK]              = UTF8("✓"),
                        [GLYPH_CROSS_MARK]              = UTF8("✗"),        /* actually called: BALLOT X */
                        [GLYPH_LIGHT_SHADE]             = UTF8("░"),
                        [GLYPH_DARK_SHADE]              = UTF8("▒"),
                        [GLYPH_ਭਰਿਆ_ਖੰਡ]              = UTF8("█"),
                        [GLYPH_SIGMA]                   = UTF8("Σ"),
                        [GLYPH_ARROW_ਉੱਪਰ]                = UTF8("↑"),       /* actually called: UPWARDS ARROW */
                        [GLYPH_ARROW_ਥੱਲੇ]              = UTF8("↓"),       /* actually called: DOWNWARDS ARROW */

                        /* Single glyph in Unicode, two in ASCII */
                        [GLYPH_ARROW_ਖੱਬਾ]              = UTF8("←"),       /* actually called: LEFTWARDS ARROW */
                        [GLYPH_ARROW_ਸੱਜਾ]             = UTF8("→"),       /* actually called: RIGHTWARDS ARROW */

                        /* Single glyph in Unicode, three in ASCII */
                        [GLYPH_ELLIPSIS]                = UTF8("…"),       /* actually called: HORIZONTAL ELLIPSIS */

                        /* Three glyphs in Unicode, five in ASCII */
                        [GLYPH_EXTERNAL_LINK]           = UTF8("[🡕]"),      /* actually called: NORTH EAST SANS-SERIF ARROW, enclosed in [] */

                        /* These smileys are a single glyph in Unicode, and three in ASCII */
                        [GLYPH_ECSTATIC_SMILEY]         = UTF8("😇"),       /* actually called: SMILING FACE WITH HALO */
                        [GLYPH_HAPPY_SMILEY]            = UTF8("😀"),       /* actually called: GRINNING FACE */
                        [GLYPH_SLIGHTLY_HAPPY_SMILEY]   = UTF8("🙂"),       /* actually called: SLIGHTLY SMILING FACE */
                        [GLYPH_NEUTRAL_SMILEY]          = UTF8("😐"),       /* actually called: NEUTRAL FACE */
                        [GLYPH_SLIGHTLY_UNHAPPY_SMILEY] = UTF8("🙁"),       /* actually called: SLIGHTLY FROWNING FACE */
                        [GLYPH_UNHAPPY_SMILEY]          = UTF8("😨"),       /* actually called: FEARFUL FACE */
                        [GLYPH_DEPRESSED_SMILEY]        = UTF8("🤢"),       /* actually called: NAUSEATED FACE */

                        /* This emoji is a single character cell glyph in Unicode, and three in ASCII */
                        [GLYPH_ਤਾਲਾ_AND_ਕੁੰਜੀ]            = UTF8("🔐"),       /* actually called: CLOSED LOCK WITH KEY */

                        /* This emoji is a single character cell glyph in Unicode, and two in ASCII */
                        [GLYPH_TOUCH]                   = UTF8("👆"),       /* actually called: BACKHAND INDEX POINTING UP */

                        /* These four emojis are single character cell glyphs in Unicode and also in ASCII. */
                        [GLYPH_RECYCLING]               = UTF8("♻️"),        /* actually called: UNIVERSAL RECYCLNG SYMBOL */
                        [GLYPH_DOWNLOAD]                = UTF8("⤵️"),        /* actually called: RIGHT ARROW CURVING DOWN */
                        [GLYPH_SPARKLES]                = UTF8("✨"),
                        [GLYPH_ਨੀਵਾਂ_BATTERY]             = UTF8("🪫"),
                        [GLYPH_WARNING_SIGN]            = UTF8("⚠️"),
                        [GLYPH_COMPUTER_DISK]           = UTF8("💽"),
                        [GLYPH_WORLD]                   = UTF8("🌍"),
                        [GLYPH_RED_CIRCLE]              = UTF8("🔴"),
                        [GLYPH_ORANGE_CIRCLE]           = UTF8("🟠"),
                        [GLYPH_YELLOW_CIRCLE]           = UTF8("🟡"),
                        [GLYPH_BLUE_CIRCLE]             = UTF8("🔵"),
                        [GLYPH_GREEN_CIRCLE]            = UTF8("🟢"),
                        [GLYPH_SUPERHERO]               = UTF8("🦸"),
                        [GLYPH_PUMPKIN]                 = UTF8("🎃"),
                        [GLYPH_IDCARD]                  = UTF8("🪪"),
                        [GLYPH_HOME]                    = UTF8("🏠"),
                        [GLYPH_ROCKET]                  = UTF8("🚀"),
                        [GLYPH_BROOM]                   = UTF8("🧹"),
                        [GLYPH_KEYBOARD]                = UTF8("⌨️"),
                        [GLYPH_CLOCK]                   = UTF8("🕗"),
                        [GLYPH_LABEL]                   = UTF8("🏷️"),
                        [GLYPH_SHELL]                   = UTF8("🐚"),
                },
        };

        ਜੇ (ਕੋਡ < 0)
                ਵਾਪਸ ਨਿਰਾ;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਕੋਡ < _GLYPH_MAX);
        ਵਾਪਸ draw_ਸਾਰਣੀ[force_utf || (ਕੋਡ >= GLYPH_ਪਹਿਲਾ_EMOJI ? emoji_enabled() : is_locale_utf8())][ਕੋਡ];
}
