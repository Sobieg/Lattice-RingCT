#include <stdio.h>
#include "ring_test.h"


int main()
{
	LRCT_Byte_Test();
    LRCT_Setup_Test();
	LRCT_Com_Test();
	LRCT_Mul_Test();
	LRCT_Lift_Test();
	LRCT_MatrixMulVect_Test();
	LRCT_Spent_Test();
	MIMO_LRCT_Setup_Test();
	LRCT_Fun_Test();
	LRCT_Shift_Test();
	LRCT_ComHom_Test();
//	system("pause");
    return 0;
}