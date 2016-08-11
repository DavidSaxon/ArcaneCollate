#include <arcanecore/test/ArcTest.hpp>

ARC_TEST_MODULE(Read)

#include <arcanecore/io/sys/FileSystemOperations.hpp>

#include <arcanecollate/Accessor.hpp>
#include <arcanecollate/Collator.hpp>
#include <arcanecollate/Reader.hpp>
#include <arcanecollate/TableOfContents.hpp>

namespace
{

//------------------------------------------------------------------------------
//                                  BASE FIXTURE
//------------------------------------------------------------------------------

class ReadFixture : public arc::test::Fixture
{
public:

    //-------------------------PUBLIC MEMBER FUNCTIONS--------------------------

    arc::io::sys::Path toc_path;
    arc::io::sys::Path base_path;

    std::vector<arc::io::sys::Path> resources;

    std::vector<arc::str::UTF8String> resource_data;

    //-------------------------PUBLIC MEMBER FUNCTIONS--------------------------

    virtual void setup()
    {
        toc_path << "tests" << "output" << "read_test.arccol_toc";
        base_path << "tests" << "output" << "read_test.arccol";

        {
            arc::io::sys::Path resource;
            resource << "tests" << "data" << "file_1.txt";
            resources.push_back(resource);
            resource_data.push_back(
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
            );
        }
        {
            arc::io::sys::Path resource;
            resource << "tests" << "data" << "file_2.txt";
            resources.push_back(resource);
            resource_data.push_back(
                "Another file with some csv data\n"
                "1,2,4,5,7,8\n"
                "Hello,World,!\n"
                "\n"
                "1337\n"
            );
        }
        {
            arc::io::sys::Path resource;
            resource << "tests" << "data" << "file_3.json";
            resources.push_back(resource);
            resource_data.push_back(
                "{\n"
                "    \"value_1\": \"Hello world!\",\n"
                "    \"value_2\": 175,\n"
                "    \"value_3\": 3.14\n"
                "}\n"
            );
        }
        {
            arc::io::sys::Path resource;
            resource << "tests" << "data" << "file_4.bin";
            resources.push_back(resource);
            resource_data.push_back(
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
            );
        }
    }

    virtual void teardown()
    {
        // just delete everything in the base path directory...
        arc::io::sys::Path output_dir;
        output_dir << "tests" << "output";
        for(const arc::io::sys::Path& path : arc::io::sys::list(output_dir))
        {
            try
            {
                arc::io::sys::delete_path(path);
            }
            catch(...)
            {
                // do nothing
            }
        }
    }
};

//------------------------------------------------------------------------------
//                                   EMPTY TOC
//------------------------------------------------------------------------------

class EmptyTOCFixture : public ReadFixture
{
public:

    virtual void setup()
    {
        // super call
        ReadFixture::setup();

        arccol::TableOfContents toc(toc_path);
        toc.write();
    }
};

ARC_TEST_UNIT_FIXTURE(empty_toc, EmptyTOCFixture)
{
    // create the Acessor
    arccol::Accessor accessor(fixture->toc_path);

    // check table of contents path
    ARC_CHECK_EQUAL(accessor.get_table_of_contents_path(), fixture->toc_path);

    // check none of the resources are found by the accessor
    for(const arc::io::sys::Path& resource : fixture->resources)
    {
        ARC_CHECK_FALSE(accessor.has_resource(resource));

        arc::io::sys::Path base_path;
        std::size_t page_index = 0;
        arc::int64 offset = 0;
        arc::int64 size = 0;
        ARC_CHECK_THROW(
            accessor.get_resource(
                resource, base_path, page_index, offset, size),
            arc::ex::KeyError
        );
    }

}

//------------------------------------------------------------------------------
//                                  SINGLE PAGE
//------------------------------------------------------------------------------

class SinglePageFixture : public ReadFixture
{
public:

    //----------------------------PUBLIC ATTRIBUTES-----------------------------

    std::vector<arc::io::sys::Path> base_paths;
    std::vector<std::size_t> page_indices;
    std::vector<arc::int64> offsets;
    std::vector<arc::int64> sizes;

    //-------------------------PUBLIC MEMBER FUNCTIONS--------------------------

    virtual void setup()
    {
        // super call
        ReadFixture::setup();

        arccol::TableOfContents toc(toc_path);

        // build a collator for all files
        arccol::Collator collator(&toc, base_path);
        for(const arc::io::sys::Path& resource : resources)
        {
            collator.add_resource(resource);
        }

        collator.execute();
        toc.write();

        // set up the results
        {
            arc::io::sys::Path p;
            p << "tests" << "output" << "read_test.arccol";
            base_paths.push_back(p);
            page_indices.push_back(0);
            offsets.push_back(0);
            sizes.push_back(115);
        }
        {
            arc::io::sys::Path p;
            p << "tests" << "output" << "read_test.arccol";
            base_paths.push_back(p);
            page_indices.push_back(0);
            offsets.push_back(115);
            sizes.push_back(64);
        }
        {
            arc::io::sys::Path p;
            p << "tests" << "output" << "read_test.arccol";
            base_paths.push_back(p);
            page_indices.push_back(0);
            offsets.push_back(179);
            sizes.push_back(75);
        }
        {
            arc::io::sys::Path p;
            p << "tests" << "output" << "read_test.arccol";
            base_paths.push_back(p);
            page_indices.push_back(0);
            offsets.push_back(254);
            sizes.push_back(440);
        }
    }
};

ARC_TEST_UNIT_FIXTURE(single_page, SinglePageFixture)
{
    // create the Accessor
    arccol::Accessor accessor(fixture->toc_path);

    // check table of contents path
    ARC_CHECK_EQUAL(accessor.get_table_of_contents_path(), fixture->toc_path);

    // check that all resources are found by the accessor
    for(std::size_t i = 0; i < fixture->resources.size(); ++i )
    {
        ARC_TEST_MESSAGE(
            arc::str::UTF8String("Checking resource: ") +
            fixture->resources[i].to_native()
        );

        ARC_CHECK_TRUE(accessor.has_resource(fixture->resources[i]));

        arc::io::sys::Path base_path;
        std::size_t page_index = 0;
        arc::int64 offset = 0;
        arc::int64 size = 0;
        accessor.get_resource(
            fixture->resources[i],
            base_path,
            page_index,
            offset,
            size
        );

        ARC_CHECK_EQUAL(base_path, fixture->base_paths[i])
        ARC_CHECK_EQUAL(page_index, fixture->page_indices[i])
        ARC_CHECK_EQUAL(offset, fixture->offsets[i])
        ARC_CHECK_EQUAL(size, fixture->sizes[i])

        // open a reader to the file
        arccol::Reader reader(
            fixture->resources[i],
            &accessor,
            arc::io::sys::FileHandle::ENCODING_RAW,
            arc::io::sys::FileHandle::NEWLINE_UNIX
        );

        ARC_CHECK_TRUE(reader.from_collated());

        ARC_CHECK_EQUAL(reader.get_size(), size);

        // check reading the contents of the file
        arc::str::UTF8String file_data;
        reader.read(file_data);

        ARC_CHECK_EQUAL(file_data, fixture->resource_data[i]);

        ARC_CHECK_TRUE(reader.eof());

        reader.close();
    }
}

//------------------------------------------------------------------------------
//                                 HALF RESOURCES
//------------------------------------------------------------------------------

class HalfResourcesFixture : public ReadFixture
{
public:

    //----------------------------PUBLIC ATTRIBUTES-----------------------------

    std::vector<bool> in_toc;
    std::vector<arc::io::sys::Path> base_paths;
    std::vector<std::size_t> page_indices;
    std::vector<arc::int64> offsets;
    std::vector<arc::int64> sizes;

    //-------------------------PUBLIC MEMBER FUNCTIONS--------------------------

    virtual void setup()
    {
        // super call
        ReadFixture::setup();

        arccol::TableOfContents toc(toc_path);

        // build a collator for all files
        arccol::Collator collator(&toc, base_path);
        collator.add_resource(resources[0]);
        collator.add_resource(resources[1]);
        collator.execute();
        toc.write();

        // set up the results
        {
            in_toc.push_back(true);
            arc::io::sys::Path p;
            p << "tests" << "output" << "read_test.arccol";
            base_paths.push_back(p);
            page_indices.push_back(0);
            offsets.push_back(0);
            sizes.push_back(115);
        }
        {
            in_toc.push_back(true);
            arc::io::sys::Path p;
            p << "tests" << "output" << "read_test.arccol";
            base_paths.push_back(p);
            page_indices.push_back(0);
            offsets.push_back(115);
            sizes.push_back(64);
        }
        {
            in_toc.push_back(false);
        }
        {
            in_toc.push_back(false);
        }
    }
};

ARC_TEST_UNIT_FIXTURE(half_resources, HalfResourcesFixture)
{
    // create the Accessor
    arccol::Accessor accessor(fixture->toc_path);

    // check table of contents path
    ARC_CHECK_EQUAL(accessor.get_table_of_contents_path(), fixture->toc_path);

    // check that all resources are found by the accessor
    for(std::size_t i = 0; i < fixture->resources.size(); ++i )
    {
        ARC_TEST_MESSAGE(
            arc::str::UTF8String("Checking resource: ") +
            fixture->resources[i].to_native()
        );

        if(fixture->in_toc[i])
        {
            ARC_CHECK_TRUE(accessor.has_resource(fixture->resources[i]));

            arc::io::sys::Path base_path;
            std::size_t page_index = 0;
            arc::int64 offset = 0;
            arc::int64 size = 0;
            accessor.get_resource(
                fixture->resources[i],
                base_path,
                page_index,
                offset,
                size
            );

            ARC_CHECK_EQUAL(base_path, fixture->base_paths[i])
            ARC_CHECK_EQUAL(page_index, fixture->page_indices[i])
            ARC_CHECK_EQUAL(offset, fixture->offsets[i])
            ARC_CHECK_EQUAL(size, fixture->sizes[i])

            // open a reader to the file
            arccol::Reader reader(
                fixture->resources[i],
                &accessor,
                arc::io::sys::FileHandle::ENCODING_RAW,
                arc::io::sys::FileHandle::NEWLINE_UNIX
            );

            ARC_CHECK_TRUE(reader.from_collated());

            ARC_CHECK_EQUAL(reader.get_size(), size);

            // check reading the contents of the file
            arc::str::UTF8String file_data;
            reader.read(file_data);

            ARC_CHECK_EQUAL(file_data, fixture->resource_data[i]);

            ARC_CHECK_TRUE(reader.eof());

            // seek to the beginning and check eof is unset
            reader.seek(0);
            ARC_CHECK_FALSE(reader.eof());

            // seek to the last character and check eof is unset
            reader.seek(reader.get_size() - 1);
            ARC_CHECK_FALSE(reader.eof());

            // seek to the end of the file and check eof is set
            reader.seek(0);
            reader.seek(reader.get_size());
            ARC_CHECK_TRUE(reader.eof());

            // seek passed the end of the file and check eof is still set
            reader.seek(reader.get_size() * 2);
            ARC_CHECK_TRUE(reader.eof());

            reader.close();
        }
        else
        {
            ARC_CHECK_FALSE(accessor.has_resource(fixture->resources[i]));

            arc::io::sys::Path base_path;
            std::size_t page_index = 0;
            arc::int64 offset = 0;
            arc::int64 size = 0;
            ARC_CHECK_THROW(
                accessor.get_resource(
                    fixture->resources[i],
                    base_path,
                    page_index,
                    offset,
                    size
                ),
                arc::ex::KeyError
            );

            // open a reader to the file
            arccol::Reader reader(
                fixture->resources[i],
                &accessor,
                arc::io::sys::FileHandle::ENCODING_RAW,
                arc::io::sys::FileHandle::NEWLINE_UNIX
            );

            ARC_CHECK_FALSE(reader.from_collated());

            // check reading the contents of the file
            arc::str::UTF8String file_data;
            reader.read(file_data);

            ARC_CHECK_EQUAL(file_data, fixture->resource_data[i]);

            ARC_CHECK_TRUE(reader.eof());

            // seek to the beginning and check eof is unset
            reader.seek(0);
            ARC_CHECK_FALSE(reader.eof());

            // seek to the last character and check eof is unset
            reader.seek(reader.get_size() - 1);
            ARC_CHECK_FALSE(reader.eof());

            // TODO: renable once ArcaneCore is fixed
            // // seek to the end of the file and check eof is set
            // reader.seek(reader.get_size());
            // ARC_CHECK_TRUE(reader.eof());

            reader.close();
        }
    }
}

//------------------------------------------------------------------------------
//                                   MULTIPAGE
//------------------------------------------------------------------------------

class MultipageFixture : public ReadFixture
{
public:

    //----------------------------PUBLIC ATTRIBUTES-----------------------------

    std::vector<arc::io::sys::Path> base_paths;
    std::vector<std::size_t> page_indices;
    std::vector<arc::int64> offsets;
    std::vector<arc::int64> sizes;

    //-------------------------PUBLIC MEMBER FUNCTIONS--------------------------

    virtual void setup()
    {
        // super call
        ReadFixture::setup();

        arccol::TableOfContents toc(toc_path);

        // build a collator for all files
        arccol::Collator collator(&toc, base_path, 200);
        for(const arc::io::sys::Path& resource : resources)
        {
            collator.add_resource(resource);
        }

        collator.execute();
        toc.write();

        // set up the results
        {
            arc::io::sys::Path p;
            p << "tests" << "output" << "read_test.arccol";
            base_paths.push_back(p);
            page_indices.push_back(0);
            offsets.push_back(0);
            sizes.push_back(115);
        }
        {
            arc::io::sys::Path p;
            p << "tests" << "output" << "read_test.arccol";
            base_paths.push_back(p);
            page_indices.push_back(0);
            offsets.push_back(115);
            sizes.push_back(64);
        }
        {
            arc::io::sys::Path p;
            p << "tests" << "output" << "read_test.arccol";
            base_paths.push_back(p);
            page_indices.push_back(0);
            offsets.push_back(179);
            sizes.push_back(75);
        }
        {
            arc::io::sys::Path p;
            p << "tests" << "output" << "read_test.arccol";
            base_paths.push_back(p);
            page_indices.push_back(1);
            offsets.push_back(54);
            sizes.push_back(440);
        }
    }
};

ARC_TEST_UNIT_FIXTURE(multi_page, MultipageFixture)
{
    // create the Accessor
    arccol::Accessor accessor(fixture->toc_path);

    // check table of contents path
    ARC_CHECK_EQUAL(accessor.get_table_of_contents_path(), fixture->toc_path);

    // check that all resources are found by the accessor
    for(std::size_t i = 0; i < fixture->resources.size(); ++i )
    {
        ARC_TEST_MESSAGE(
            arc::str::UTF8String("Checking resource: ") +
            fixture->resources[i].to_native()
        );

        ARC_CHECK_TRUE(accessor.has_resource(fixture->resources[i]));

        arc::io::sys::Path base_path;
        std::size_t page_index = 0;
        arc::int64 offset = 0;
        arc::int64 size = 0;
        accessor.get_resource(
            fixture->resources[i],
            base_path,
            page_index,
            offset,
            size
        );

        ARC_CHECK_EQUAL(base_path, fixture->base_paths[i]);
        ARC_CHECK_EQUAL(page_index, fixture->page_indices[i]);
        ARC_CHECK_EQUAL(offset, fixture->offsets[i]);
        ARC_CHECK_EQUAL(size, fixture->sizes[i]);

        // open a reader to the file
        arccol::Reader reader(
            fixture->resources[i],
            &accessor,
            arc::io::sys::FileHandle::ENCODING_RAW,
            arc::io::sys::FileHandle::NEWLINE_UNIX
        );

        ARC_CHECK_TRUE(reader.from_collated());

        ARC_CHECK_EQUAL(reader.get_size(), size);

        // check reading the contents of the file
        arc::str::UTF8String file_data;
        reader.read(file_data);

        ARC_CHECK_EQUAL(file_data, fixture->resource_data[i]);

        ARC_CHECK_TRUE(reader.eof());

        // seek back to the beginning of the file
        reader.seek(0);

        ARC_CHECK_FALSE(reader.eof());
        ARC_CHECK_EQUAL(reader.tell(), 0);

        // read the file data again
        file_data.assign("");
        reader.read(file_data);

        ARC_CHECK_EQUAL(file_data, fixture->resource_data[i]);

        // seek from the beginning to half way through the file
        reader.seek(0);
        reader.seek(reader.get_size() / 2);

        // read the file data again
        file_data.assign("");
        reader.read(file_data);

        // get the half the file data
        arc::str::UTF8String half_resource(fixture->resource_data[i]);
        half_resource = half_resource.substring(
            static_cast<std::size_t>(reader.get_size() / 2),
            half_resource.get_length()
        );

        ARC_CHECK_EQUAL(file_data, half_resource);

        // repeat the same test but seek from the end of the file
        reader.seek(reader.get_size() / 2);
        file_data.assign("");
        reader.read(file_data);
        ARC_CHECK_EQUAL(file_data, half_resource);

        reader.close();
    }
}

//------------------------------------------------------------------------------
//                                   MULTI-BASE
//------------------------------------------------------------------------------

class MultiBaseFixture : public ReadFixture
{
public:

    //----------------------------PUBLIC ATTRIBUTES-----------------------------

    std::vector<arc::io::sys::Path> base_paths;
    std::vector<std::size_t> page_indices;
    std::vector<arc::int64> offsets;
    std::vector<arc::int64> sizes;

    //-------------------------PUBLIC MEMBER FUNCTIONS--------------------------

    virtual void setup()
    {
        // super call
        ReadFixture::setup();

        arccol::TableOfContents toc(toc_path);

        // build collators
        arccol::Collator collator_1(&toc, base_path);
        collator_1.add_resource(resources[0]);
        collator_1.add_resource(resources[1]);

        arc::io::sys::Path base_2;
        base_2 << "tests" << "output" << "read_test_2.arccol";
        arccol::Collator collator_2(&toc, base_2);
        collator_2.add_resource(resources[2]);
        collator_2.add_resource(resources[3]);

        collator_1.execute();
        collator_2.execute();
        toc.write();

        // set up the results
        {
            arc::io::sys::Path p;
            p << "tests" << "output" << "read_test.arccol";
            base_paths.push_back(p);
            page_indices.push_back(0);
            offsets.push_back(0);
            sizes.push_back(115);
        }
        {
            arc::io::sys::Path p;
            p << "tests" << "output" << "read_test.arccol";
            base_paths.push_back(p);
            page_indices.push_back(0);
            offsets.push_back(115);
            sizes.push_back(64);
        }
        {
            base_paths.push_back(base_2);
            page_indices.push_back(0);
            offsets.push_back(0);
            sizes.push_back(75);
        }
        {
            base_paths.push_back(base_2);
            page_indices.push_back(0);
            offsets.push_back(75);
            sizes.push_back(440);
        }
    }
};

ARC_TEST_UNIT_FIXTURE(multi_base, MultiBaseFixture)
{
    // create the Accessor
    arccol::Accessor accessor(fixture->toc_path);

    // check table of contents path
    ARC_CHECK_EQUAL(accessor.get_table_of_contents_path(), fixture->toc_path);

    // check that all resources are found by the accessor
    for(std::size_t i = 0; i < fixture->resources.size(); ++i )
    {
        ARC_TEST_MESSAGE(
            arc::str::UTF8String("Checking resource: ") +
            fixture->resources[i].to_native()
        );

        ARC_CHECK_TRUE(accessor.has_resource(fixture->resources[i]));

        arc::io::sys::Path base_path;
        std::size_t page_index = 0;
        arc::int64 offset = 0;
        arc::int64 size = 0;
        accessor.get_resource(
            fixture->resources[i],
            base_path,
            page_index,
            offset,
            size
        );

        ARC_CHECK_EQUAL(base_path, fixture->base_paths[i])
        ARC_CHECK_EQUAL(page_index, fixture->page_indices[i])
        ARC_CHECK_EQUAL(offset, fixture->offsets[i])
        ARC_CHECK_EQUAL(size, fixture->sizes[i])

        // open a reader to the file
        arccol::Reader reader(
            fixture->resources[i],
            &accessor,
            arc::io::sys::FileHandle::ENCODING_RAW,
            arc::io::sys::FileHandle::NEWLINE_UNIX
        );

        ARC_CHECK_TRUE(reader.from_collated());

        ARC_CHECK_EQUAL(reader.get_size(), size);

        // check reading the contents of the file
        arc::str::UTF8String file_data;
        reader.read(file_data);

        ARC_CHECK_EQUAL(file_data, fixture->resource_data[i]);

        ARC_CHECK_TRUE(reader.eof());

        reader.close();

    }
}

} // namespace anonymous
