#include "include/Util.h"
#include "include/Constants.h"

bool gt_almost_equalrad2(const gtfloat A, const gtfloat B)
{
  gtfloat myA, myB;
  
  myA = A;
  myB = B;
  normalizeZeroTo2Pi(myA);
  normalizeZeroTo2Pi(myB);
  
  if (myA >= myB)
    {
      return (GT_ALMOST_EQUAL2(myA, myB) || /* normal */
	      GT_ALMOST_EQUAL2(myA, myB+2*M_PI)); /* to catch 0 == 2*M_PI */
    }
  else 				/* (myA < myB) */
    {
      return (GT_ALMOST_EQUAL2(myA, myB) ||
	      GT_ALMOST_EQUAL2(myA + 2*M_PI, myB));
    }
}
