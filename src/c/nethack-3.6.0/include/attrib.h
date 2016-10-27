/* NetHack 3.6	attrib.h	$NHDT-Date: 1432512779 2015/05/25 00:12:59 $  $NHDT-Branch: master $:$NHDT-Revision: 1.8 $ */
/* Copyright 1988, Mike Stephenson				  */
/* NetHack may be freely redistributed.  See license for details. */

/*	attrib.h - Header file for character class processing. */

#ifndef ATTRIB_H
#define ATTRIB_H

#define A_STR 0
#define A_INT 1
#define A_WIS 2
#define A_DEX 3
#define A_CON 4
#define A_CHA 5

#define A_MAX 6 /* used in rn2() selection of attrib */

extern xchar yourCurrentAttr(int index);
extern void setYourCurrentAttr(int index, xchar value);
extern void increaseYourCurrentAttr(int index, xchar delta);
extern void decreaseYourCurrentAttr(int index, xchar delta);

extern xchar yourAttrMax(int index);
extern void setYourAttrMax(int index, xchar value);
extern void increaseYourAttrMax(int index, xchar delta);
extern void decreaseYourAttrMax(int index, xchar delta);

extern xchar yourAttrBonus(int index);
extern void setYourAttrBonus(int index, xchar value);
extern void increaseYourAttrBonus(int index, xchar delta);
extern void decreaseYourAttrBonus(int index, xchar delta);

extern xchar yourAttrChangeFromExercise(int index);
extern void setYourAttrChangeFromExercise(int index, xchar value);
extern void increaseYourAttrChangeFromExercise(int index, xchar delta);
extern void decreaseYourAttrChangeFromExercise(int index, xchar delta);

#define ACURR(x) (acurr(x))
#define ACURRSTR (acurrstr())
/* should be: */
/* #define ACURR(x) (ABON(x) + ATEMP(x) + (areYouPolymorphed()  ? MBASE(x) : ABASE(x)) */

extern xchar yourAttrAsMonster(int index);
extern void setYourAttrAsMonster(int index, xchar value);
extern void increaseYourAttrAsMonster(int index, xchar delta);
extern void decreaseYourAttrAsMonster(int index, xchar delta);

extern xchar yourAttrMaxAsMonster(int index);
extern void setYourAttrMaxAsMonster(int index, xchar value);
extern void increaseYourAttrMaxAsMonster(int index, xchar delta);
extern void decreaseYourAttrMaxAsMonster(int index, xchar delta);

extern xchar yourTemporaryAttrChange(int index);
extern void setYourTemporaryAttrChange(int index, xchar value);
extern void increaseYourTemporaryAttrChange(int index, xchar delta);
extern void decreaseYourTemporaryAttrChange(int index, xchar delta);

extern xchar yourAttrChangeTimeout(int index);
extern void setYourAttrChangeTimeout(int index, xchar value);
extern void increaseYourAttrChangeTimeout(int index, xchar delta);
extern void decreaseYourAttrChangeTimeout(int index, xchar delta);

/* KMH -- Conveniences when dealing with strength constants */
#define STR18(x) (18 + (x))  /* 18/xx */
#define STR19(x) (100 + (x)) /* For 19 and above */

struct attribs {
    schar a[A_MAX];
};

#define ATTRMAX(x)                                        \
    ((x == A_STR && areYouPolymorphed() && strongmonst(youmonst.data)) \
         ? STR18(100)                                     \
         : urace.attrmax[x])
#define ATTRMIN(x) (urace.attrmin[x])

#endif /* ATTRIB_H */
