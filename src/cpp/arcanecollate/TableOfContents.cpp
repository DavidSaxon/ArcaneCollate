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
    arc::io::sys::Path base_path;
    std::size_t page_index;
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

const arc::io::sys::Path& TableOfContents::get_path() const
{
    return m_path;
}

void TableOfContents::write()
{
    // open the path to write to
    arc::io::sys::FileWriter writer(
        m_path,
        arc::io::sys::FileWriter::OPEN_TRUNCATE,
        arc::io::sys::FileHandle::ENCODING_UTF8,
        arc::io::sys::FileHandle::NEWLINE_UNIX
    );

    // iterate over the resources and write their data
    for(const std::unique_ptr<ResourceEntry>& entry : m_entries)
    {
        // build the line and write
        arc::str::UTF8String line;
        line << entry->resource_path.to_unix() << ","
             << entry->base_path.to_unix() << "," << entry->page_index << ","
             << entry->offset << "," << entry->size;
        writer.write_line(line, false);
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
        const arc::io::sys::Path& base_path,
        std::size_t page_index,
        arc::int64 offset,
        arc::int64 size)
{
    std::unique_ptr<ResourceEntry> entry(new ResourceEntry());
    entry->resource_path = resource_path;
    entry->base_path = base_path;
    entry->page_index = page_index;
    entry->offset = offset;
    entry->size = size;
    m_entries.push_back(std::move(entry));
}

} // namespace arccol
