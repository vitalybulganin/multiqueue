/*!==========================================================================
* \file
* - Program:       gtest-mmt-grpc
* - File:          main.cpp
* - Created:       11/23/2018
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
#include "units/gtest-servus-client.h"
//-------------------------------------------------------------------------//
int main(int argc, char ** argv)
{
    testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}
