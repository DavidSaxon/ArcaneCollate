/*!
 * \file
 * \author David Saxon
 */
#ifndef ARCANECOLLATE_TABLEOFCONTENTS_HPP_
#define ARCANECOLLATE_TABLEOFCONTENTS_HPP_

#include <arcanecore/io/sys/Path.hpp>

namespace arccol
{

/*!
 * \brief TODO:
 */
class TableOfContents
{
public:

    //--------------------------------------------------------------------------
    //                                CONSTRUCTOR
    //--------------------------------------------------------------------------

    /*!
     * \brief TODO:
     */
    TableOfContents(const arc::io::sys::Path& path);

    /*!
     * \brief Copy constructor.
     *
     * \param other The other TableOfContents to make a copy of.
     */
    TableOfContents(const TableOfContents& other);

    //--------------------------------------------------------------------------
    //                                 OPERATORS
    //--------------------------------------------------------------------------
};

} // namespace arccol

#endif
