/*!==========================================================================
* \file
* - Program:       gtest-multiqueue
* - File:          main.cpp
* - Created:       09/14/2019
* - Author:        Vitaly Bulganin
* - Description:
* - Comments:
*
-----------------------------------------------------------------------------
*
* - History:
*
===========================================================================*/
#include <fstream>
#include <stdexcept>
//-------------------------------------------------------------------------//
#include <gtest/gtest.h>
//-------------------------------------------------------------------------//
#include "units/gtest-queue.h"
#include "units/gtest-map.h"
#include "units/gtest-processor.h"
//-------------------------------------------------------------------------//
int main(int argc, char ** argv)
{
    testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
