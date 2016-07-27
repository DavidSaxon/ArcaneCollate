#include <arcanecore/test/ArcTest.hpp>

ARC_TEST_MODULE(Write)

#include <arcanecore/base/Exceptions.hpp>
#include <arcanecore/io/sys/FileReader.hpp>
#include <arcanecore/io/sys/FileSystemOperations.hpp>

#include <arcanecollate/Collator.hpp>
#include <arcanecollate/TableOfContents.hpp>

namespace
{

//------------------------------------------------------------------------------
//                                  BASE FIXTURE
//------------------------------------------------------------------------------

class WriteFixture : public arc::test::Fixture
{
public:

    //----------------------------PUBLIC ATTRIBUTES-----------------------------

    arc::io::sys::Path toc_path;
    arc::io::sys::Path base_path;

    arc::str::UTF8String toc_result;
    std::vector<arc::str::UTF8String> collated_results;

    //-------------------------PUBLIC MEMBER FUNCTIONS--------------------------

    virtual void teardown()
    {
        if(arc::io::sys::exists(toc_path))
        {
            arc::io::sys::delete_path(toc_path);
        }
        for(std::size_t i = 0; i < collated_results.size(); ++i)
        {
            arc::io::sys::Path collated_path(get_collated_path(base_path, i));
            if(arc::io::sys::exists(collated_path))
            {
                arc::io::sys::delete_path(collated_path);
            }
        }
    }

    arc::io::sys::Path get_collated_path(
            const arc::io::sys::Path& base_path,
            std::size_t page_index)
    {
        // get and remove the final component of the base path
        arc::str::UTF8String filename(base_path.get_back());
        arc::io::sys::Path ret_path(base_path);
        ret_path.remove(ret_path.get_length() - 1);

        // add the page index
        filename << "." << page_index;
        ret_path << filename;

        return ret_path;
    }

    void check_toc()
    {
        // open the table of contents
        arc::io::sys::FileReader reader(
            toc_path,
            arc::io::sys::FileHandle::ENCODING_UTF8
        );
        arc::str::UTF8String toc_contents;
        reader.read(toc_contents);

        ARC_CHECK_EQUAL(toc_contents, toc_result);
    }

    virtual void check_collated()
    {
        for(std::size_t i = 0; i < collated_results.size(); ++i)
        {
            // get the path
            arc::io::sys::Path collated_path(get_collated_path(base_path, i));

            // read contents
            arc::io::sys::FileReader reader(
                collated_path,
                arc::io::sys::FileHandle::ENCODING_RAW
            );
            arc::str::UTF8String collate_contents;
            reader.read(collate_contents);

            ARC_CHECK_EQUAL(collate_contents, collated_results[i]);
        }
    }
};

//------------------------------------------------------------------------------
//                                     TEST 1
//------------------------------------------------------------------------------

class Test1Fixtue : public WriteFixture
{
public:

    //-------------------------PUBLIC MEMBER FUNCTIONS--------------------------

    virtual void setup()
    {
        toc_path << "tests" << "output" << "write_test.arccol_toc";
        base_path << "tests" << "output" << "write_test.arccol";

        toc_result =
            "tests/data/file_1.txt\n"
            "tests/output/write_test.arccol.0\n"
            "0\n"
            "115\n"
            "tests/data/file_2.txt\n"
            "tests/output/write_test.arccol.0\n"
            "115\n"
            "64\n";

        collated_results.push_back(arc::str::UTF8String(
            "This is the first file\n"
            "It has a couple of lines\n"
            "and then some numbers!\n"
            "0\n"
            "1\n"
            "2\n"
            "3\n"
            "4\n"
            "5\n"
            "6\n"
            "7\n"
            "8\n"
            "9\n"
            "It ends with a newline!\n"
            "Another file with some csv data\n"
            "1,2,4,5,7,8\n"
            "Hello,World,!\n"
            "\n"
            "1337\n"
        ));
    }
};

ARC_TEST_UNIT_FIXTURE(test_1, Test1Fixtue)
{
    // set up the table of contents and the collator
    arccol::TableOfContents toc(fixture->toc_path);
    arccol::Collator collator(&toc, fixture->base_path);

    // add resources
    arc::io::sys::Path resource_1;
    resource_1 << "tests" << "data" << "file_1.txt";
    collator.add_resource(resource_1);
    // --
    arc::io::sys::Path resource_2;
    resource_2 << "tests" << "data" << "file_2.txt";
    collator.add_resource(resource_2);

    // write out
    collator.execute();
    toc.write();

    // run checks
    fixture->check_toc();
    fixture->check_collated();
}

//------------------------------------------------------------------------------
//                                     TEST 2
//------------------------------------------------------------------------------

class Test2Fixtue : public WriteFixture
{
public:

    //-------------------------PUBLIC MEMBER FUNCTIONS--------------------------

    virtual void setup()
    {
        toc_path << "tests" << "output" << "write_test.arccol_toc";
        base_path << "tests" << "output" << "write_test.arccol";

        toc_result =
            "tests/data/file_1.txt\n"
            "tests/output/write_test.arccol.0\n"
            "0\n"
            "115\n"
            "tests/data/file_2.txt\n"
            "tests/output/write_test.arccol.0\n"
            "115\n"
            "64\n"
            "tests/data/file_3.json\n"
            "tests/output/write_test.arccol.0\n"
            "179\n"
            "75\n"
            "tests/data/file_4.bin\n"
            "tests/output/write_test.arccol.0\n"
            "254\n"
            "440\n";

        collated_results.push_back(arc::str::UTF8String(
            "This is the first file\n"
            "It has a couple of lines\n"
            "and then some numbers!\n"
            "0\n"
            "1\n"
            "2\n"
            "3\n"
            "4\n"
            "5\n"
            "6\n"
            "7\n"
            "8\n"
            "9\n"
            "It ends with a newline!\n"
            "Another file with some csv data\n"
            "1,2,4,5,7,8\n"
            "Hello,World,!\n"
            "\n"
            "1337\n"
            "{\n"
            "    \"value_1\": \"Hello world!\",\n"
            "    \"value_2\": 175,\n"
            "    \"value_3\": 3.14\n"
            "}\n"
            "54"
        ));

        collated_results.push_back(arc::str::UTF8String(
            "46 3853 7472 696e 6745 005f 5a4e 3361\n"
            "7263 3474 6573 7432 3372 6567 6973 7465\n"
            "725f 676c 6f62 616c 5f66 6978 7475 7265\n"
            "4550 4676 7645 5332 5f00 5f5f 6e69 7465\n"
            "725f 6261 7365 3c76 6f69 6420 282a 2a29\n"
            "2829 3e00 5f5a 5374 3334 5f5f 756e 696e\n"
            "6974 6961 6c69 7a6"
        ));

        collated_results.push_back(arc::str::UTF8String(
            "5 645f 6d6f 7665 5f69\n"
            "665f 6e6f 6578 6365 7074 5f61 4950 5046\n"
            "7676 4553 325f 5361 4953 315f 4545 5430\n"
            "5f54 5f53 355f 5334 5f52 5431 5f00 5f5a\n"
            "4e53 7436 7665 6374 6f72 4950 4676 7645\n"
        ));
    }
};

ARC_TEST_UNIT_FIXTURE(test_2, Test2Fixtue)
{
    // set up the table of contents and the collator
    arccol::TableOfContents toc(fixture->toc_path);
    arccol::Collator collator(&toc, fixture->base_path, 256);

    // add resources
    arc::io::sys::Path resource_1;
    resource_1 << "tests" << "data" << "file_1.txt";
    collator.add_resource(resource_1);
    // --
    arc::io::sys::Path resource_2;
    resource_2 << "tests" << "data" << "file_2.txt";
    collator.add_resource(resource_2);
    // --
    arc::io::sys::Path resource_3;
    resource_3 << "tests" << "data" << "file_3.json";
    collator.add_resource(resource_3);
    // --
    arc::io::sys::Path resource_4;
    resource_4 << "tests" << "data" << "file_4.bin";
    collator.add_resource(resource_4);

    // write out
    collator.execute();
    toc.write();

    // run checks
    fixture->check_toc();
    fixture->check_collated();
}

//------------------------------------------------------------------------------
//                                     TEST 3
//------------------------------------------------------------------------------

ARC_TEST_UNIT_FIXTURE(test_3, Test1Fixtue)
{
    // set up the table of contents and the collator with a smaller read size
    arccol::TableOfContents toc(fixture->toc_path);
    arccol::Collator collator(&toc, fixture->base_path, -1, 32);

    // add resources
    arc::io::sys::Path resource_1;
    resource_1 << "tests" << "data" << "file_1.txt";
    collator.add_resource(resource_1);
    // --
    arc::io::sys::Path resource_2;
    resource_2 << "tests" << "data" << "file_2.txt";
    collator.add_resource(resource_2);

    // write out
    collator.execute();
    toc.write();

    // run checks
    fixture->check_toc();
    fixture->check_collated();
}

//------------------------------------------------------------------------------
//                                     TEST 4
//------------------------------------------------------------------------------

class Test4Fixtue : public WriteFixture
{
public:

    //-------------------------PUBLIC MEMBER ATTRIBUTES-------------------------

    arc::io::sys::Path base_path_2;

    std::vector<arc::str::UTF8String> collated_results_2;

    //-------------------------PUBLIC MEMBER FUNCTIONS--------------------------

    virtual void setup()
    {
        toc_path << "tests" << "output" << "write_test.arccol_toc";
        base_path << "tests" << "output" << "write_test_1.arccol";
        base_path_2 << "tests" << "output" << "write_test_2.arccol";

        toc_result =
            "tests/data/file_3.json\n"
            "tests/output/write_test_1.arccol.0\n"
            "0\n"
            "75\n"
            "tests/data/file_4.bin\n"
            "tests/output/write_test_1.arccol.0\n"
            "75\n"
            "440\n"
            "tests/data/file_1.txt\n"
            "tests/output/write_test_2.arccol.0\n"
            "0\n"
            "115\n"
            "tests/data/file_2.txt\n"
            "tests/output/write_test_2.arccol.0\n"
            "115\n"
            "64\n";

        collated_results.push_back(arc::str::UTF8String(
            "{\n"
            "    \"value_1\": \"Hello world!\",\n"
            "    \"value_2\": 175,\n"
            "    \"value_3\": 3.14\n"
            "}\n"
            "5446 3853 7472 696e 6745 005f 5a4e 3361\n"
            "7263 3474 6573 7432 3372 6567 6973 7465\n"
            "725f 676c 6f62 616c 5f66 6978 7475 7265\n"
            "4550 4676 7645 5332 5f00 5f5f 6e69 7465\n"
            "725f 6261 7365 3c76 6f69 6420 282a 2a29\n"
            "2829 3e00 5f5a 5374 3334 5f5f 756e 696e\n"
            "6974 6961 6c69 7a65 645f 6d6f 7665 5f69\n"
            "665f 6e6f 6578 6365 7074 5f61 4950 5046\n"
            "7676 4553 325f 5361 4953 315f 4545 5430\n"
            "5f54 5f53 355f 5334 5f52 5431 5f00 5f5a\n"
            "4e53 7436 7665 6374 6f72 4950 4676 7645\n"
        ));

        collated_results_2.push_back(arc::str::UTF8String(
            "This is the first file\n"
            "It has a couple of lines\n"
            "and then some numbers!\n"
            "0\n"
            "1\n"
            "2\n"
            "3\n"
            "4\n"
            "5\n"
            "6\n"
            "7\n"
            "8\n"
            "9\n"
            "It ends with a newline!\n"
            "Another file with some csv data\n"
            "1,2,4,5,7,8\n"
            "Hello,World,!\n"
            "\n"
            "1337\n"
        ));
    }

    virtual void teardown()
    {
        // super call
        WriteFixture::teardown();

        for(std::size_t i = 0; i < collated_results_2.size(); ++i)
        {
            arc::io::sys::Path collated_path(get_collated_path(base_path_2, i));
            if(arc::io::sys::exists(collated_path))
            {
                arc::io::sys::delete_path(collated_path);
            }
        }
    }

    virtual void check_collated()
    {
        // super call
        WriteFixture::check_collated();

        for(std::size_t i = 0; i < collated_results_2.size(); ++i)
        {
            // get the path
            arc::io::sys::Path collated_path(get_collated_path(base_path_2, i));

            // read contents
            arc::io::sys::FileReader reader(
                collated_path,
                arc::io::sys::FileHandle::ENCODING_RAW
            );
            arc::str::UTF8String collate_contents;
            reader.read(collate_contents);

            ARC_CHECK_EQUAL(collate_contents, collated_results_2[i]);
        }
    }
};

ARC_TEST_UNIT_FIXTURE(test_4, Test4Fixtue)
{
    // set up the table of contents and the collator
    arccol::TableOfContents toc(fixture->toc_path);
    arccol::Collator collator_1(&toc, fixture->base_path);
    arccol::Collator collator_2(&toc, fixture->base_path_2);

    // add resources to the first collator
    arc::io::sys::Path resource_3;
    resource_3 << "tests" << "data" << "file_3.json";
    collator_1.add_resource(resource_3);
    // --
    arc::io::sys::Path resource_4;
    resource_4 << "tests" << "data" << "file_4.bin";
    collator_1.add_resource(resource_4);

    // add resources to the second collator
    arc::io::sys::Path resource_1;
    resource_1 << "tests" << "data" << "file_1.txt";
    collator_2.add_resource(resource_1);
    // --
    arc::io::sys::Path resource_2;
    resource_2 << "tests" << "data" << "file_2.txt";
    collator_2.add_resource(resource_2);

    // write out
    collator_1.execute();
    collator_2.execute();
    toc.write();

    // run checks
    fixture->check_toc();
    fixture->check_collated();
}

//------------------------------------------------------------------------------
//                                 INVALID CASES
//------------------------------------------------------------------------------

ARC_TEST_UNIT(invalid_cases)
{
    arccol::TableOfContents* toc = nullptr;
    arc::io::sys::Path valid_path;
    valid_path << "this" << "is" << "not" << "important";

    ARC_TEST_MESSAGE("Checking null TableOfContents");
    ARC_CHECK_THROW(
        arccol::Collator(toc, valid_path),
        arc::ex::ValueError
    );

    ARC_TEST_MESSAGE("Checking empty base path");
    toc = new arccol::TableOfContents(valid_path);
    arc::io::sys::Path empty_path;
    ARC_CHECK_THROW(
        arccol::Collator(toc, empty_path),
        arc::ex::ValueError
    );

    delete toc;
}

} // namespace anonymous

