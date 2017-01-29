#include "arcanecollate/Reader.hpp"

#include <cassert>
#include <fstream>

#include "arcanecore/base/str/StringOperations.hpp"
#include <arcanecore/base/Exceptions.hpp>

namespace arccol
{

//------------------------------------------------------------------------------
//                                  CONSTRUCTORS
//------------------------------------------------------------------------------

Reader::Reader(
        const Accessor* accessor,
        Encoding encoding,
        Newline newline)
    :
    arc::io::sys::FileReader(encoding, newline),
    m_accessor              (accessor),
    m_from_collated         (false),
    m_begin_page            (0),
    m_offset                (0),
    m_current_page          (0),
    m_current_offset        (0),
    m_current_size          (0),
    m_position              (0),
    m_eof                   (false)
{
}

Reader::Reader(
        const arc::io::sys::Path& resource,
        const Accessor* accessor,
        Encoding encoding,
        Newline newline)
    :
    arc::io::sys::FileReader(encoding, newline),
    m_accessor              (accessor),
    m_from_collated         (false),
    m_begin_page            (0),
    m_offset                (0),
    m_current_page          (0),
    m_current_offset        (0),
    m_current_size          (0),
    m_position              (0),
    m_eof                   (false)
{
    // set and open the file
    set_path(resource);
    open();
}

Reader::Reader(Reader&& other)
    :
    arc::io::sys::FileReader(std::move(other)),
    m_accessor              (other.m_accessor),
    m_from_collated         (other.m_from_collated),
    m_base_path             (std::move(other.m_base_path)),
    m_begin_page            (other.m_begin_page),
    m_offset                (other.m_offset),
    m_current_page          (other.m_current_page),
    m_current_offset        (other.m_current_offset),
    m_current_size          (other.m_current_size),
    m_position              (other.m_position),
    m_eof                   (other.m_eof)
{
    // reset other resources
    other.m_accessor = nullptr;
    other.m_from_collated = false;
    other.m_begin_page = 0;
    other.m_offset = 0;
    other.m_current_page = 0;
    other.m_current_offset = 0;
    other.m_current_size = 0;
    other.m_position = 0;
    other.m_eof = false;
}

//------------------------------------------------------------------------------
//                                   OPERATORS
//------------------------------------------------------------------------------

Reader& Reader::operator=(Reader&& other)
{
    // steal
    arc::io::sys::FileReader::operator=(std::move(other));
    m_accessor = other.m_accessor;
    m_from_collated = other.m_from_collated;
    m_base_path = std::move(other.m_base_path);
    m_begin_page = other.m_begin_page;
    m_offset = other.m_offset;
    m_current_page = other.m_current_page;
    m_current_offset = other.m_current_offset;
    m_current_size = other.m_current_size;
    m_position = other.m_position;
    other.m_eof = other.m_eof;

    // reset
    other.m_accessor = nullptr;
    other.m_from_collated = false;
    other.m_begin_page = 0;
    other.m_offset = 0;
    other.m_current_page = 0;
    other.m_current_offset = 0;
    other.m_current_size = 0;
    other.m_position = 0;
    other.m_eof = false;

    return *this;
}

//------------------------------------------------------------------------------
//                            PUBLIC STATIC FUNCTIONS
//------------------------------------------------------------------------------

arc::int64 Reader::get_bytes(
        const arc::io::sys::Path& resource,
        const Accessor* accessor,
        char** bytes,
        Encoding encoding,
        Newline newline)
{
    // crate the reader to access to file
    arccol::Reader reader(resource, accessor, encoding, newline);
    // read the data from the file
    const arc::int64 data_size = reader.get_size();
    *bytes = new char[static_cast<std::size_t>(data_size)];
    reader.read(*bytes, data_size);

    return data_size;
}

//------------------------------------------------------------------------------
//                            PUBLIC MEMBER FUNCTIONS
//------------------------------------------------------------------------------

bool Reader::from_collated() const
{
    // ensure the Reader is open
    if(!m_open)
    {
        throw arc::ex::StateError(
            "Whether the Reader is reading from a collated file cannot be "
            "queried while the Reader is closed."
        );
    }

    return m_from_collated;
}

void Reader::open()
{
    // ensure the file reader is not already open
    if(m_open)
    {
        throw arc::ex::StateError(
            "Reader cannot be opened since it is already open.");
    }

    // is the resource in the table of contents
    m_from_collated = m_accessor->has_resource(m_path);

    // default to standard behavior
    if(!m_from_collated)
    {
        FileReader::open();
        return;
    }

    // get the file information from the accessor
    m_accessor->get_resource(
        m_path,
        m_base_path,
        m_begin_page,
        m_offset,
        m_size
    );
    m_current_page = m_begin_page;

    // open the stream to the current path
    open_stream();

    // file reader is open
    m_open = true;
    m_newline_checker_valid = false;
    m_eof = false;

    // seek to the beginning of the resource
    m_stream->seekg(m_offset, std::ios_base::beg);

    // detect the encoding if needed
    if(m_encoding == ENCODING_DETECT)
    {
        m_encoding = detect_encoding();
    }
}

arc::int64 Reader::tell() const
{
    // ensure the Reader is open
    if(!m_open)
    {
        throw arc::ex::StateError(
            "File position indicator cannot be queried while the Reader is "
            "closed."
        );
    }

    // default to standard behavior
    if(!m_from_collated)
    {
        return FileReader::tell();
    }

    return m_position;
}

void Reader::seek(arc::int64 index)
{
    // ensure the Reader is open
    if(!m_open)
    {
        throw arc::ex::StateError(
            "File position indicator cannot be moved while the Reader is "
            "closed."
        );
    }

    // default to standard behavior
    if(!m_from_collated)
    {
        FileReader::seek(index);
        return;
    }

    // clamp to the resource range
    if(index >= m_size)
    {
        index = m_size;
    }
    // clear eof file flag before seeking within the file range
    else
    {
        m_eof = false;
    }

    // get the distance we need to seek
    arc::int64 seek_distance = index - m_position;

    // forwards seek
    if(seek_distance > 0)
    {
        // get the remaining bytes in this file
        arc::int64 remaing_size = m_current_size - m_stream->tellg();
        assert(remaing_size >= 0);

        // loop until we're not seeking into the next collated file
        while(seek_distance > remaing_size)
        {
            seek_distance -= remaing_size;
            // move to the next page
            ++m_current_page;
            open_stream();
            remaing_size = m_current_size;
        }

        // seek in this file
        m_stream->seekg(m_current_offset + seek_distance, std::ios_base::beg);
    }
    // backwards seek
    else if(seek_distance < 0)
    {
        // get the remaining bytes in this file
        arc::int64 remaing_size = m_stream->tellg();
        assert(remaing_size >= 0);

        // loop until we're not seeking into the next collated file
        while((-seek_distance) > remaing_size)
        {
            seek_distance += remaing_size;
            // move to the next page
            assert(m_current_page != 0);
            --m_current_page;
            open_stream();
            remaing_size = m_current_size;
        }

        // seek in this file
        m_stream->seekg(remaing_size + seek_distance, std::ios_base::beg);
    }

    // update position
    m_position = index;
}

bool Reader::eof() const
{
    // ensure the Reader is open
    if(!m_open)
    {
        throw arc::ex::StateError(
            "End of file cannot be queried while the Reader is closed."
        );
    }

    // default to standard behavior
    if(!m_from_collated)
    {
        return FileReader::eof();
    }

    return m_eof;
}

void Reader::read(char* data, arc::int64 length)
{
    check_can_read();

    // default to standard behavior
    if(!m_from_collated)
    {
        FileReader::read(data, length);
        return;
    }

    // if the length is -1 modify to the length of the file
    if(length < 0)
    {
        length = m_size;
    }

    // stores the remaining number of bytes to read
    arc::int64 remaining_read = length;

    while(remaining_read > 0)
    {
        // get the remaining bytes in this file
        arc::int64 remaing_size = m_current_size - m_stream->tellg();
        assert(remaing_size >= 0);

        // get the amount of data we will read from the current file
        arc::int64 current_read = remaining_read;
        if(current_read > remaing_size)
        {
            current_read = remaing_size;
        }


        // read the data
        m_stream->read(data + (length - remaining_read), current_read);

        // subtract the amount of data we've read
        remaining_read -= current_read;

        // should we move to the next page
        if(remaining_read > 0)
        {
            ++m_current_page;
            open_stream();
        }
    }

    // update position
    m_position += length;

    // if we have reached the end of the stream, set the end of file flag
    if(m_position >= m_size)
    {
        m_eof = true;
    }
}

void Reader::read(arc::str::UTF8String& data, arc::int64 length)
{
    // implemented to avoid function hiding
    FileReader::read(data, length);
}

//------------------------------------------------------------------------------
//                            PRIVATE MEMBER FUNCTIONS
//------------------------------------------------------------------------------

arc::io::sys::Path Reader::get_collated_path() const
{
    arc::io::sys::Path collated_path(m_base_path);
    arc::str::UTF8String filename(collated_path.get_back());
    collated_path.remove(collated_path.get_length() - 1);

    filename << "." << m_current_page;
    collated_path << filename;

    return collated_path;
}

void Reader::open_stream()
{
    // build the initial file name to read from
    arc::io::sys::Path read_path(get_collated_path());

    // ensure we clean up the existing stream
    if(m_stream)
    {
        delete m_stream;
    }

    // create a new stream
    #ifdef ARC_OS_WINDOWS

        // utf-16 path
        std::size_t length = 0;
        const char* p = arc::str::utf8_to_utf16(
            read_path.to_windows(),
            length,
            arc::data::ENDIAN_LITTLE
        );

        m_stream = new std::ifstream(
            (const wchar_t*) p,
            std::ios_base::in | std::ios_base::binary
        );

        delete[] p;

    #else

        m_stream = new std::ifstream(
            read_path.to_native().get_raw(),
            std::ios_base::in | std::ios_base::binary
        );

    #endif

    // did opening fail?
    if(!m_stream->good())
    {
        // clean up
        delete m_stream;

        // throw exception
        arc::str::UTF8String error_message;
        error_message << "Failed to open Reader to path: \'"
                      << read_path.to_native() << "\'.";
        throw arc::ex::IOError(error_message);
    }

    // set the current offset
    if(m_current_page != m_begin_page)
    {
        m_current_offset = 0;
    }
    else
    {
        m_current_offset = m_offset;
    }
    // set the current size
    m_stream->seekg(0, std::ios_base::end);
    m_current_size = m_stream->tellg();
    m_stream->seekg(0, std::ios_base::beg);
}

} // namespace arclog
