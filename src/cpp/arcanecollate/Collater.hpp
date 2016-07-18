/*!
 * \file
 * \author David Saxon
 */
#ifndef ARCANECOLLATE_COLLATER_HPP_
#define ARCANECOLLATE_COLLATER_HPP_

#include "arcanecollate/TableOfContents.hpp"

namespace arccol
{

/*!
 * \brief TODO
 */
class Collater
{
private:

    ARC_DISALLOW_COPY_AND_ASSIGN(Collater);

public:

    //--------------------------------------------------------------------------
    //                                CONSTRUCTOR
    //--------------------------------------------------------------------------

    Collater(const TableOfContents& table_of_contents);

private:

    //--------------------------------------------------------------------------
    //                             PRIVATE ATTRIBUTES
    //--------------------------------------------------------------------------

    TableOfContents m_table_of_contents;
};


} // namespace arccol

#endif
