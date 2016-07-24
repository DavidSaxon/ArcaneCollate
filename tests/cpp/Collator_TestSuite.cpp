#include <arcanecore/test/ArcTest.hpp>

ARC_TEST_MODULE(Collator)

#include <arcanecollate/Collator.hpp>
#include <arcanecollate/TableOfContents.hpp>

ARC_TEST_UNIT(proto)
{
    arc::io::sys::Path toc_path;
    toc_path << "tests" << "output" << "proto_test.arccol_toc";
    arccol::TableOfContents toc(toc_path);

    arc::io::sys::Path base_path;
    base_path << "tests" << "output" << "proto_test";
    arccol::Collator collator(&toc, base_path);

    arc::io::sys::Path resource_1;
    resource_1 << "tests" << "data" << "file_1.txt";
    collator.add_resource(resource_1);

    arc::io::sys::Path resource_2;
    resource_2 << "tests" << "data" << "file_2.txt";
    collator.add_resource(resource_2);

    collator.execute();
}
