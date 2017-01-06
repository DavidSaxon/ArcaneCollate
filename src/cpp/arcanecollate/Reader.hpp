/*!
 * \file
 * \author David Saxon
 */
#ifndef ARCANECOLLATE_READER_HPP_
#define ARCANECOLLATE_READER_HPP_

#include <arcanecore/io/sys/FileReader.hpp>

#include "arcanecollate/Accessor.hpp"

namespace arccol
{

/*!
 * \brief Derived class of ArcaneCore's arc::io::sys::FileReader that can be
 *        used to read a collated resource.
 *
 * Once opened this reader will behave as a regular arc::io::sys::FileReader and
 * will internally manage collated file offsets and cross file boundaries. The
 * Reader must be passed an Accessor which it will use to find the location of
 * the resource within collated files. If the resource cannot be found in the
 * Accessor this Reader will read the file at the real path for the resource.
 */
class Reader : public arc::io::sys::FileReader
{
private:

    ARC_DISALLOW_COPY_AND_ASSIGN(Reader);

public:

    //--------------------------------------------------------------------------
    //                                CONSTRUCTORS
    //--------------------------------------------------------------------------

    /*!
     * \brief Unopened constructor.
     *
     * Creates a new unopened ArcaneCollate Reader with no resource yet defined.
     *
     * \param Accessor Object this Reader will use to locate resources.
     * \param encoding Defines the encoding of the contents of the resource to
     *                 read. If arc::io::sys::FileHandle::ENCODING_DETECT is
     *                 used the Reader will attempt to detect the encoding used
     *                 in the resource at the time of opening. If the encoding
     *                 cannot be detected arc::io::sys::FileHandle::ENCODING_RAW
     *                 will be used. The detected encoding can be queried using
     *                 get_encoding() once the resource has been opened.
     * \param newline The newline symbol used in the resource to read.
     *                See set_newline().
     */
    Reader(
            const Accessor* accessor,
            Encoding encoding = ENCODING_DETECT,
            Newline newline   = NEWLINE_UNIX);

    /*!
     * \brief Resource constructor.
     *
     * Creates a new unopened ArcaneCollate Reader opened to the given resource.
     * If the resource is found in this Reader's Accessor then it will be
     * sourced from the collated file, if it's not it will be sourced from the
     * real resource path.
     *
     * \param resource The path to the resource to be read.
     * \param Accessor Object this Reader will use to locate resources.
     * \param encoding Defines the encoding of the contents of the resource to
     *                 read. If arc::io::sys::FileHandle::ENCODING_DETECT is
     *                 used the Reader will attempt to detect the encoding used
     *                 in the resource at the time of opening. If the encoding
     *                 cannot be detected arc::io::sys::FileHandle::ENCODING_RAW
     *                 will be used. The detected encoding can be queried using
     *                 get_encoding() once the resource has been opened.
     * \param newline The newline symbol used in the resource to read.
     *                See set_newline().
     *
     * \throws arc::io::sys::IOError If the resource is being read from a
     *                               collated file but the collated cannot be
     *                               accessed. Or if the resource is being read
     *                               from its real path but it cannot be
     *                               accessed.
     */
    Reader(
            const arc::io::sys::Path& resource,
            const Accessor* accessor,
            Encoding encoding = ENCODING_DETECT,
            Newline newline   = NEWLINE_UNIX);

    /*!
     * \brief Move constructor.
     *
     * \param other The Reader to move resources from.
     */
    Reader(Reader&& other);

    //--------------------------------------------------------------------------
    //                                 OPERATORS
    //--------------------------------------------------------------------------

    /*!
     * \brief Move assignment operator.
     *
     * Moves resources from the given Reader to this Reader.
     *
     * \param other The Reader to move resources from.
     */
    Reader& operator=(Reader&& other);

    //--------------------------------------------------------------------------
    //                          PUBLIC STATIC FUNCTIONS
    //--------------------------------------------------------------------------

    /*!
     * \brief Reads and returns the given resource from the Accessor.
     *
     * \param resource The path to the resource to be read.
     * \param Accessor Object that will be used to locate resources.
     * \param bytes Returns newly allocated data which contains the raw bytes
     *              of the resource. This data will need to be deleted by the
     *              caller.
     * \param encoding Defines the encoding of the contents of the resource to
     *                 read. If arc::io::sys::FileHandle::ENCODING_DETECT is
     *                 used the Reader will attempt to detect the encoding used
     *                 in the resource at the time of opening. If the encoding
     *                 cannot be detected arc::io::sys::FileHandle::ENCODING_RAW
     *                 will be used. The detected encoding can be queried using
     *                 get_encoding() once the resource has been opened.
     * \param newline The newline symbol used in the resource to read.
     *                See set_newline().
     *
     * \return The number of bytes in the returned data.
     *
     * \throws arc::io::sys::IOError If the resource is being read from a
     *                               collated file but the collated cannot be
     *                               accessed. Or if the resource is being read
     *                               from its real path but it cannot be
     *                               accessed.
     */
    static arc::int64 get_bytes(
            const arc::io::sys::Path& resource,
            const Accessor* accessor,
            char** bytes,
            Encoding encoding = ENCODING_DETECT,
            Newline newline   = NEWLINE_UNIX);

    //--------------------------------------------------------------------------
    //                          PUBLIC MEMBER FUNCTIONS
    //--------------------------------------------------------------------------

    /*!
     * \brief Returns whether the resource is being read from a collated file or
     *        or not.
     *
     * \throws arc::ex::StateError If the Reader is not open.
     */
    bool from_collated() const;

    // override
    virtual void open();

    // override
    virtual arc::int64 tell() const;

    // override
    virtual void seek(arc::int64 index);

    // override
    virtual bool eof() const;

    // override
    virtual void read(char* data, arc::int64 length);

    // override
    virtual void read(arc::str::UTF8String& data, arc::int64 length = -1);

private:

    //--------------------------------------------------------------------------
    //                             PRIVATE ATTRIBUTES
    //--------------------------------------------------------------------------

    /*!
     * \brief Pointer to the Accessor object which will be used to find the
     *        location of resources.
     */
    const Accessor* m_accessor;

    /*!
     * \brief Whether the resource is being read from a collated file or not.
     */
    bool m_from_collated;

    /*!
     * \brief The base path of the collated file the resource is located in.
     */
    arc::io::sys::Path m_base_path;
    /*!
     * \brief The collated file page number the resource begins in.
     */
    std::size_t m_begin_page;
    /*!
     * \brief The offset in bytes of the resource in the initial collated file.
     */
    arc::int64 m_offset;

    /*!
     * \brief The current collated file page being read from.
     */
    std::size_t m_current_page;
    /*!
     * \brief The offset where the resource begins in the current page.
     */
    arc::int64 m_current_offset;
    /*!
     * \brief The size of the current collated file.
     */
    arc::int64 m_current_size;

    /*!
     * \brief The current position index in regards to the size of the resource.
     */
    arc::int64 m_position;

    /*!
     * \brief Stores whether the end of the file has been reached or not.
     */
    bool m_eof;

    //--------------------------------------------------------------------------
    //                          PRIVATE MEMBER FUNCTIONS
    //--------------------------------------------------------------------------

    /*!
     * \brief Convenience function to get the current collated file path to read
     *        from.
     */
    arc::io::sys::Path get_collated_path() const;

    /*!
     * \brief Deletes the current stream, and opens a new stream to the current
     *        collated path (determined from m_base_path and m_current_page).
     */
    void open_stream();
};

} // namespace arclog

#endif
