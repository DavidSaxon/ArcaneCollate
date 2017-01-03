/*!
 * \file
 * \author David Saxon
 */
#ifndef ARCANECOLLATE_ACCESSOR_HPP_
#define ARCANECOLLATE_ACCESSOR_HPP_

#include <map>
#include <memory>

#include <arcanecore/io/sys/Path.hpp>

//------------------------------------------------------------------------------
//                              FORWARD DECLARATIONS
//------------------------------------------------------------------------------

namespace arclog
{
class Input;
class LogHandler;
} // namespace arclog

namespace arccol
{

/*!
 * \brief Object used to access the locations of resources in collated files
 *        from a table of contents file one disk.
 */
class Accessor
{
public:

    //--------------------------------------------------------------------------
    //                          PUBLIC STATIC ATTRIBUTES
    //--------------------------------------------------------------------------

    /*!
     * \brief If true this Accessor's will always report that resources are not
     *        located in collated files, and will not enforce that the table of
     *        contents must exist.
     *
     * Defaults to false.
     */
    static bool force_real_resources;

    //--------------------------------------------------------------------------
    //                                CONSTRUCTORS
    //--------------------------------------------------------------------------

    /*!
     * \brief Creates a new Accessor to retrieve resources from collated files.
     *
     * \param table_of_contents The path to the table of contents file which
     *                          defines the locations of resources in collated
     *                          files.
     *
     * \throws arc::ex::IOError If the table of contents file cannot be
     *                          accessed.
     */
    Accessor(const arc::io::sys::Path& table_of_contents);

    /*!
     * \brief Copy constructor.
     *
     * \param other The Accessor to copy from.
     */
    Accessor(const Accessor& other);

    //--------------------------------------------------------------------------
    //                                 DESTRUCTOR
    //--------------------------------------------------------------------------

    ~Accessor();

    //--------------------------------------------------------------------------
    //                                 OPERATORS
    //--------------------------------------------------------------------------

    /*!
     * \brief Copies values from the given Accessor.
     *
     * \param other Accessor to copy from.
     */
    Accessor& operator=(const Accessor& other);

    //--------------------------------------------------------------------------
    //                          PUBLIC STATIC FUNCTIONS
    //--------------------------------------------------------------------------

    /*!
     * \brief Sets the arclog::LogHandler that will be used to vend a new
     *        logging input for logging warnings in Accessor objects.
     *
     * If this function is never called ArcaneCollate will perform no logging
     * and run silently.
     *
     * \param handler The handler which will be used to vend an input for
     *                ArcaneCollate to log through.
     * \param profile_name The string used as the app_name of the logging
     *                     profile that ArcaneCollate will be logging through.
     *
     * \throws arc::ex::ValueError If ```handler``` is null.
     */
    static void set_logging(
            arclog::LogHandler* handler,
            const arc::str::UTF8String& profile_name = "AcraneCollate");

    //--------------------------------------------------------------------------
    //                          PUBLIC MEMBER FUNCTIONS
    //--------------------------------------------------------------------------

    /*!
     * \brief Reloads the resource location information from the table of
     *        contents this Accessor is using.
     *
     * \throws arc::ex::IOError If the table of contents file cannot be
     *                          accessed.
     */
    void reload();

    /*!
     * \brief Returns the path to the table of contents this Accessor is using.
     */
    const arc::io::sys::Path& get_table_of_contents_path() const;

    /*!
     * \brief Sets the path this Accessor will use to read the table of contents
     *        file.
     *
     * This operation will internal reload() in order to read resource location
     * information.
     *
     * \throws arc::ex::IOError If the table of contents file cannot be
     *                          accessed.
     */
    void set_table_of_contents_path(const arc::io::sys::Path& path);

    /*!
     * \brief Whether the given resource was found when loading from the table
     *        of contents.
     */
    bool has_resource(const arc::io::sys::Path& resource_path) const;

    /*!
     * \brief Returns information about where to find the given resource file
     *        in collated documents.
     *
     * \param resource_path The path of the resource to locate in collated
     *                      files.
     * \param base_path Returns the base path of the collated file this resource
     *                  is located in. The actual file the resource is located
     *                  in will follow this syntax: <base_path>.<page_index>
     * \param page_index Returns the index of the first collated file page this
     *                   resource is located within.
     * \param offset Returns the byte position of the start of the resource in
     *               the collated file.
     * \param size Returns the size of the resource in bytes. Note that if this
     *             is larger than the remaining bytes the in the initial
     *             collated file page then the rest of the resource will be
     *             located in the next page and so on.
     *
     * \throws arc::ex::KeyError If the resource is not in the table of
     *                           contents.
     */
    void get_resource(
            const arc::io::sys::Path& resource_path,
            arc::io::sys::Path& base_path,
            std::size_t& page_index,
            arc::int64& offset,
            arc::int64& size) const;

    /*!
     * \brief Lists the file system paths that are in the given path that are
     *        listed in the table of contents of this accessor.
     *
     * This operation performs a similar function to the `ls` command on Linux,
     * or the `dir` command on Windows. An empty vector will be returned if the
     * given path does not exist or is .not a directory.
     *
     * \note If Accessor::force_real_resources is ```true``` this function will
     *       return the result of arc::io::sys::list()
     *
     * \param path The file path to list sub-paths for.
     * \return The vector of sub-paths.
     */
    std::vector<arc::io::sys::Path> list(const arc::io::sys::Path& path);

    /*!
     * \brief Lists all descendant file system paths located under the given path.
     *
     * This operation returns a vector similar to that of list(), except any sub
     * paths that are also directories are traversed and so on, so that this
     * function returns all paths that are a descendant of the given path. This
     * function will not resolve symbolics in order to avoid infinite recursion.
     */

    /*!
     * \breif Lists all descendant file system paths located under the given
     *        path that are listed in the table of contents of this accessor.
     *
     * This operation returns a vector similar to that of list(), except any sub
     * paths that are also directories are traversed and so on, so that this
     * function returns all paths that are a descendant of the given path.
     *
     * \note If Accessor::force_real_resources is ```true``` this function will
     *       return the result of arc::io::sys::list_rec()
     *
     * \param path The file path to list the descendant paths for.
     * \return The vector of descendant paths.
     */
    std::vector< arc::io::sys::Path> list_rec(const arc::io::sys::Path& path);

private:

    //--------------------------------------------------------------------------
    //                                 STRUCTURES
    //--------------------------------------------------------------------------

    /*!
     * \brief Internal structure used to store the location of a resource in
     *        collated files.
     */
    struct ResourceLocation
    {
        arc::io::sys::Path base_path;
        std::size_t page_index;
        arc::int64 offset;
        arc::int64 size;
    };

    //--------------------------------------------------------------------------
    //                             PRIVATE ATTRIBUTES
    //--------------------------------------------------------------------------

    /*!
     * \brief The logging input to log through. If Null ArcaneCollate should run
     *         silently.
     */
    static std::unique_ptr<arclog::Input> logger;

    /*!
     * \brief The path to the table of contents file which is used to discover
     *        resource locations.
     */
    arc::io::sys::Path m_table_of_contents;

    /*!
     * \brief Mapping from resource paths to their location information.
     */
    std::map<arc::io::sys::Path, ResourceLocation> m_resources;
};

} // namespace arccol

#endif
