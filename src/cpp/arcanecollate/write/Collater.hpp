/*!
 * \file
 * \author David Saxon
 */
#ifndef ARCANECOLLATE_WRITE_COLLATER_HPP_
#define ARCANECOLLATE_WRITE_COLLATER_HPP_

#include <arcanecore/io/sys/Path.hpp>

namespace arccol
{
/*!
 * \brief TODO:
 */
namespace write
{

/*!
 * \brief TODO:
 */
class Collater
{
private:

    ARC_DISALLOW_COPY_AND_ASSIGN(Collater);

public:

    //--------------------------------------------------------------------------
    //                                CONSTRUCTOR
    //--------------------------------------------------------------------------

    // TODO:
    Collater();

    //--------------------------------------------------------------------------
    //                          PUBLIC MEMBER FUNCTIONS
    //--------------------------------------------------------------------------

    /*!
     * \brief Sets the path this Collator will write its table of contents to.
     *
     * \param path The path that the table of contents will be written to.
     * \param append Whether this table of contents will be appended to the
     *               given file or override it.
     * \param hash Whether the table of contents will be hashed to make it
     *             non-human readable.
     */
    // void set_table_of_contents(
    //         const arc::io::sys::Path& path,
    //         bool append = false,
    //         bool hash = false);

    // void add_file();
};

} // namespace write
} // namespace arcol

#endif
