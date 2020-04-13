
#define ILU_INTERNAL_C

#include "ilu_internal.h"

const ILdouble IL_PI      = 3.1415926535897932384626;
const ILdouble IL_DEGCONV = 0.0174532925199432957692;
ILimage *iluCurImage = NULL;

ILfloat ilCos(ILfloat Angle) {
    return (ILfloat)(cos(Angle * IL_DEGCONV));
}

ILfloat ilSin(ILfloat Angle) {
    return (ILfloat)(sin(Angle * IL_DEGCONV));
}


ILint ilRound(ILfloat Num) {
    return (ILint)(Num + 0.5); // this is truncating in away-from-0, not rounding
}
