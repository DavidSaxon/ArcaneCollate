#include "arcanecollate/TableOfContents.hpp"

#include <arcanecore/io/sys/FileWriter.hpp>

namespace arccol
{

//------------------------------------------------------------------------------
//                                 RESOURCE ENTRY
//------------------------------------------------------------------------------

/*!
 * \brief Records information about a resource's entry into the the table of
 *        contents.
 */
struct ResourceEntry
{
    arc::io::sys::Path resource_path;
    arc::io::sys::Path collate_path;
    arc::int64 offset;
    arc::int64 size;
};


//------------------------------------------------------------------------------
//                                  CONSTRUCTOR
//------------------------------------------------------------------------------

TableOfContents::TableOfContents(const arc::io::sys::Path& path)
    :
    m_path(path)
{
}

//------------------------------------------------------------------------------
//                                   DESTRUCTOR
//------------------------------------------------------------------------------

TableOfContents::~TableOfContents()
{
}

//------------------------------------------------------------------------------
//                            PUBLIC MEMBER FUNCTIONS
//------------------------------------------------------------------------------

void TableOfContents::write()
{
    // open the path to write to
    arc::io::sys::FileWriter writer(
        m_path,
        arc::io::sys::FileWriter::OPEN_TRUNCATE,
        arc::io::sys::FileHandle::ENCODING_UTF8
    );

    // iterate over the resources and write a line for
    for(const std::unique_ptr<ResourceEntry>& entry : m_entries)
    {
        arc::str::UTF8String resource_line;
        // TODO: need to hash here!
        resource_line << entry->resource_path.to_unix() << " "
                      << entry->collate_path.to_unix()  << " "
                      << entry->offset << " " << entry->size;
        writer.write_line(resource_line, false);
    }

    // done!
    writer.flush();
    writer.close();
}

//------------------------------------------------------------------------------
//                           PROTECTED MEMBER FUNCTIONS
//------------------------------------------------------------------------------

void TableOfContents::add_resource(
        const arc::io::sys::Path& resource_path,
        const arc::io::sys::Path& collate_path,
        arc::int64 offset,
        arc::int64 size)
{
    std::unique_ptr<ResourceEntry> entry(new ResourceEntry());
    entry->resource_path = resource_path;
    entry->collate_path = collate_path;
    entry->offset = offset;
    entry->size = size;
    m_entries.push_back(std::move(entry));
}

} // namespace arccol
