#include <arcanecore/test/ArcTest.hpp>

ARC_TEST_MODULE(Read)

#include <arcanecore/io/sys/FileSystemOperations.hpp>

#include <arcanecollate/Accessor.hpp>
#include <arcanecollate/Collator.hpp>
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

    //-------------------------PUBLIC MEMBER FUNCTIONS--------------------------

    virtual void setup()
    {
        toc_path << "tests" << "output" << "read_test.arccol_toc";
        base_path << "tests" << "output" << "read_test.arccol";

        {
            arc::io::sys::Path resource;
            resource << "tests" << "data" << "file_1.txt";
            resources.push_back(resource);
        }
        {
            arc::io::sys::Path resource;
            resource << "tests" << "data" << "file_2.txt";
            resources.push_back(resource);
        }
        {
            arc::io::sys::Path resource;
            resource << "tests" << "data" << "file_3.json";
            resources.push_back(resource);
        }
        {
            arc::io::sys::Path resource;
            resource << "tests" << "data" << "file_4.bin";
            resources.push_back(resource);
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
    }
}

} // namespace anonymous
