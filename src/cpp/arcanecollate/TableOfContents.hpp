/*!
 * \file
 * \author David Saxon
 */
#ifndef ARCANECOLLATE_TABLEOFCONTENTS_HPP_
#define ARCANECOLLATE_TABLEOFCONTENTS_HPP_

#include <memory>

#include <arcanecore/io/sys/Path.hpp>

namespace arccol
{

//------------------------------------------------------------------------------
//                              FORWARD DECLARATIONS
//------------------------------------------------------------------------------

class Collator;
struct ResourceEntry;

/*!
 * \brief TODO:
 */
class TableOfContents
{
private:

    ARC_DISALLOW_COPY_AND_ASSIGN(TableOfContents);

    //--------------------------------------------------------------------------
    //                                  FRIENDS
    //--------------------------------------------------------------------------

    friend class Collator;

public:

    //--------------------------------------------------------------------------
    //                                CONSTRUCTOR
    //--------------------------------------------------------------------------

    /*!
     * \brief TODO:
     */
    TableOfContents(const arc::io::sys::Path& path);

    //--------------------------------------------------------------------------
    //                                 DESTRUCTOR
    //--------------------------------------------------------------------------

    ~TableOfContents();

    //--------------------------------------------------------------------------
    //                          PUBLIC MEMBER FUNCTIONS
    //--------------------------------------------------------------------------

    /*!
     * \brief Writes this TableOfContents to the file system.
     *
     * \note This function should be called after all Collator objects using
     *       this TableOfContents have been executed.
     *
     * \throws arc::ex::InvalidPathError If the path for the TableOfContent
     *                                   cannot be written to.
     */
    void write();

protected:

    //--------------------------------------------------------------------------
    //                         PROTECTED MEMBER FUNCTIONS
    //--------------------------------------------------------------------------

    /*!
     * \brief Adds a resource to the table of contents.
     *
     * \param resource_path The path of the resource being added to the table of
     *                      contents.
     * \param collate_path The path to the collated file this resource will be
     *                     stored in.
     * \param offset The offset in bytes where the start of the resource begins
     *               in the collated file.
     * \param size The size in bytes of the resource.
     */
    void add_resource(
            const arc::io::sys::Path& resource_path,
            const arc::io::sys::Path& collate_path,
            arc::int64 offset,
            arc::int64 size);

private:

    //--------------------------------------------------------------------------
    //                             PRIVATE ATTRIBUTES
    //--------------------------------------------------------------------------

    /*!
     * \brief The path this TableOfContents will be written to.
     */
    arc::io::sys::Path m_path;

    /*!
     * \Information of the resources to write for this TableOfContents.
     */
    std::vector<std::unique_ptr<arccol::ResourceEntry>> m_entries;
};

} // namespace arccol

#endif
