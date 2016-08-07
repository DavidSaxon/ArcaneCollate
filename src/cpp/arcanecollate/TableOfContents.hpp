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
 * \brief Object that is used to write the table of contents that defines which
 *        collated files contain which resources and the size and the offsets of
 *        the resource in the collated files.
 *
 * This object should be passed to one or more Collator objects and will then be
 * informed by the Collator what resources should be added to the table of
 * contents.
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
     * \brief Creates a new TableOfContents.
     *
     * \param path The file path that the table of contents file will be written
     *             to.
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
     * \brief Returns the path this Table of contents is writing to.
     */
    const arc::io::sys::Path& get_path() const;

    /*!
     * \brief Writes this TableOfContents to the file system.
     *
     * \note This function should be called after all Collator objects using
     *       this TableOfContents have been executed.
     *
     * \throws arc::ex::IOError If the path for the TableOfContent
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
     * \param base_path The base path of the collated file this is resource is
     *                  located within.
     * \param page_index The page number of the collated file this resource is
     *                   within.
     * \param offset The offset in bytes where the start of the resource begins
     *               in the collated file.
     * \param size The size in bytes of the resource.
     */
    void add_resource(
            const arc::io::sys::Path& resource_path,
            const arc::io::sys::Path& base_path,
            std::size_t page_index,
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
