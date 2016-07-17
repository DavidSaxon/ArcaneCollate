#include <arcanecore/test/ArcTest.hpp>

ARC_TEST_MODULE(write.Collater)

#include <arcanecollate/write/Collater.hpp>

// TODO: REMOVE ME
#include <iostream>

ARC_TEST_UNIT(proto)
{
    arccol::write::Collater c;

    std::cout << "Hello tests!" << std::endl;
}
