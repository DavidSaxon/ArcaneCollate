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

    // iterate over the resources and write their data
    for(const std::unique_ptr<ResourceEntry>& entry : m_entries)
    {
        writer.write_line(entry->resource_path.to_unix(), false);
        writer.write_line(entry->collate_path.to_unix(), false);

        arc::str::UTF8String offset_str;
        offset_str << entry->offset;
        writer.write_line(offset_str, false);

        arc::str::UTF8String size_str;
        size_str << entry->size;
        writer.write_line(size_str, false);
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
