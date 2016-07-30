/*!
 * \file
 * \author David Saxon
 */
#ifndef ARCANECOLLATE_COLLATOR_HPP_
#define ARCANECOLLATE_COLLATOR_HPP_

#include <arcanecore/io/sys/Path.hpp>

//------------------------------------------------------------------------------
//                              FORWARD DECLARATIONS
//------------------------------------------------------------------------------

namespace arc
{
namespace io
{
namespace sys
{
class FileWriter;
} // namespace sys
} // namespace io
} // namespace arc

namespace arccol
{

//------------------------------------------------------------------------------
//                              FORWARD DECLARATIONS
//------------------------------------------------------------------------------

class TableOfContents;

/*!
 * \brief TODO
 */
class Collator
{
private:

    ARC_DISALLOW_COPY_AND_ASSIGN(Collator);

public:

    //--------------------------------------------------------------------------
    //                                CONSTRUCTOR
    //--------------------------------------------------------------------------

    /*!
     * \brief TODO
     */
    Collator(
        TableOfContents* table_of_contents,
        const arc::io::sys::Path& base_path,
        arc::int64 page_size = -1,
        std::size_t read_size = 268435456U);

    //--------------------------------------------------------------------------
    //                          PUBLIC MEMBER FUNCTIONS
    //--------------------------------------------------------------------------

    /*!
     * \brief Returns the pointer to the table of contents being used by this
     *        Collator.
     */
    TableOfContents* get_table_of_contents() const;

    /*!
     * \brief Returns the base path of this Collator.
     */
    const arc::io::sys::Path& get_base_path() const;

    /*!
     * \brief Returns the maximum size in bytes of a single collated file
     *        produced by this object.
     */
    arc::int64 get_page_size() const;

    /*!
     * \brief Returns the maximum number of bytes that will be read into memory
     *        from a resource at any one time.
     */
    std::size_t get_read_size() const;

    /*!
     * \brief Returns the resources that are going to be collated by this
     *        object.
     */
    const std::vector<arc::io::sys::Path>& get_resources() const;

    /*!
     * \brief Adds the path to a resource that will be collated by this object.
     *
     * This function does not check whether the resource exists, but if it does
     * not or is not accessible the execute() function will fail.
     *
     * \return Whether the resource path was added to this Collator or not,
     *         a resource will not be added if it has already been added.
     */
    bool add_resource(const arc::io::sys::Path& resource_path);

    /*!
     * \brief Performs writing of the collated document, and adds the resource
     *        mapping to the TableOfContents.
     *
     * \throws arc::ex::InvalidPathError If opening a resource file fails or
     *                                   writing to the collated file fails.
     */
    void execute();

    /*!
     * \brief Deletes any file system data that was created by this Collator.
     *
     * This function can be called if execution fails and you want to restore
     * the original state.
     */
    void revert();

private:

    //--------------------------------------------------------------------------
    //                             PRIVATE ATTRIBUTES
    //--------------------------------------------------------------------------

    /*!
     * \brief The table that will be used to define the locations of resources
     *        in the collated file.
     */
    TableOfContents* m_table_of_contents;
    /*!
     * \brief The base path used for writing collated resource.
     */
    arc::io::sys::Path m_base_path;
    /*!
     * \brief The maximum size of collated files produced by this object.
     */
    arc::int64 m_page_size;
    /*!
     * \brief Maximum number of bytes that will be read into memory from
     *        resources at one time.
     */
    std::size_t m_read_size;

    /*!
     * \brief The paths to the resources this object is collating.
     */
    std::vector<arc::io::sys::Path> m_resources;

    /*!
     * \brief The list of collated resources that have been created by this
     *        object.
     */
    std::vector<arc::io::sys::Path> m_created;

    //--------------------------------------------------------------------------
    //                          PRIVATE MEMBER FUNCTIONS
    //--------------------------------------------------------------------------

    /*!
     * \brief Returns a new FileWriter for the given page index.
     */
    arc::io::sys::FileWriter* new_page(std::size_t page_index);
};


} // namespace arccol

#endif
