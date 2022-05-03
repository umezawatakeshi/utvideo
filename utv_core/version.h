#define UTVIDEO_COMMENT \
/* 文字コードはＳＪＩＳ 改行コードはＣＲＬＦ */ \
/* $Id$ */ \
/* Inno Setup で include すると普通にコメントを書けないので、こうやって書く。 */

#ifndef UTVIDEO_VERSION

#define UTVIDEO_VERSION_MAJOR  23
#define UTVIDEO_VERSION_MINOR   0
#define UTVIDEO_VERSION_BUGFIX  1

#ifndef ISPP_INVOKED

#define UTVIDEO_VERSION_MASK 0xffffff00
#define UTVIDEO_VERSION ((UTVIDEO_VERSION_MAJOR << 24) | \
                         (UTVIDEO_VERSION_MINOR << 16) | \
                         (UTVIDEO_VERSION_BUGFIX << 8))

#define UTVIDEO_GEN_VERSION0(a, b, c) (#a "." #b "." #c)
#define UTVIDEO_GEN_VERSION(a, b, c) UTVIDEO_GEN_VERSION0(a, b, c)
#define UTVIDEO_VERSION_STR UTVIDEO_GEN_VERSION(UTVIDEO_VERSION_MAJOR, UTVIDEO_VERSION_MINOR, UTVIDEO_VERSION_BUGFIX)

#endif

#endif

#undef UTVIDEO_COMMENT
