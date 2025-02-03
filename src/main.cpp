#include <stdio.h>
#include <sstream>
#include <iostream>
#include <gtest/gtest.h>
#include "ThreadPool.hpp"

int main(int argc, char** argv){
    // Initialize Google Test framework
    ::testing::InitGoogleTest(&argc, argv);

    // Run all tests
    return RUN_ALL_TESTS();
}
