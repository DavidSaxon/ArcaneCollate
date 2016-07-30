#include <iostream>

#include <arcanecore/base/Exceptions.hpp>
#include <arcanecore/io/sys/FileSystemOperations.hpp>

#include <arcanecollate/Collator.hpp>
#include <arcanecollate/TableOfContents.hpp>

#include <arcanelog/Shared.hpp>
#include <arcanelog/outputs/FileOutput.hpp>
#include <arcanelog/outputs/StdOutput.hpp>

//------------------------------------------------------------------------------
//                                   CONSTANTS
//------------------------------------------------------------------------------

static const arc::str::UTF8String APP_NAME("ArcaneCollate::CmdTool");
//----------------------------COMMAND LINE ARGUMENTS----------------------------
// defines the verbosity level this tool will report at
static const arc::str::UTF8String ARG_VERBOSITY("--verbosity");
// shows the help and exits
static const arc::str::UTF8String ARG_HELP("--help");
// defines the path to where the table of contents should be written
static const arc::str::UTF8String ARG_TABLEOFCONTENTS("--table_of_contents");
// defines the maximum size in bytes of collated files
static const arc::str::UTF8String ARG_PAGE_SIZE("--page_size");
// defines the maximum number of bytes that will be read into memory at any one
// point when generating collated files
static const arc::str::UTF8String ARG_READ_SIZE("--read_size");
// denotes the begin of a collation structure
static const arc::str::UTF8String ARG_COLLATE_BEGIN("--collate_begin");
// denotes the end of a collation structure
static const arc::str::UTF8String ARG_COLLATE_END("--collate_end");

//------------------------------------------------------------------------------
//                                    GLOBALS
//------------------------------------------------------------------------------

// logging
arclog::Input* g_logger = nullptr;
arclog::StdOutput* g_std_output = nullptr;
// table of contents
arccol::TableOfContents* g_toc = nullptr;
// page size
arc::int64 g_page_size = -1;
// read size
std::size_t g_read_size = 268435456U;
// collators
std::vector<arccol::Collator*> g_collators;

//------------------------------------------------------------------------------
//                                   PROTOTYPES
//------------------------------------------------------------------------------

/*!
 * \brief Parses the command line arguments.
 */
int parse_args(int argc, char* argv[]);

/*!
 * \brief Reports the state of this tool.
 */
void report_state();

/*!
 * \brief cleans up memory before exiting.
 */
void cleanup();

/*!
 * \brief Shows the help print out for this tool.
 */
void show_help();

//------------------------------------------------------------------------------
//                                 MAIN FUNCTION
//------------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    // set up logging
    g_logger = arclog::shared_handler.vend_input(arclog::Profile(APP_NAME));
    g_std_output = new arclog::StdOutput();
    arclog::shared_handler.add_output(g_std_output);

    // parse arguments
    int ret_code = parse_args(argc, argv);
    if(ret_code != 0)
    {
        cleanup();
        return ret_code;
    }

    // report state
    report_state();

    cleanup();
    return 0;
}

//------------------------------------------------------------------------------
//                                   FUNCTIONS
//------------------------------------------------------------------------------

int parse_args(int argc, char* argv[])
{
    std::size_t arg_count = static_cast<std::size_t>(argc);
    for(std::size_t i = 1; i < arg_count; ++i)
    {
        arc::str::UTF8String arg(argv[i]);

        // verbosity
        if(arg == ARG_VERBOSITY)
        {
            // check there is another argument
            if(i < arg_count - 1)
            {
                arc::str::UTF8String verbosity_str(argv[++i]);
                // check against known verbosities and set on the logger
                if(verbosity_str == "critical")
                {
                    g_std_output->set_verbosity_level(
                        arclog::VERBOSITY_CRITICAL);
                }
                else if(verbosity_str == "error")
                {
                    g_std_output->set_verbosity_level(arclog::VERBOSITY_ERROR);
                }
                else if(verbosity_str == "warning")
                {
                    g_std_output->set_verbosity_level(
                        arclog::VERBOSITY_WARNING);
                }
                else if(verbosity_str == "notice")
                {
                    g_std_output->set_verbosity_level(arclog::VERBOSITY_NOTICE);
                }
                else if(verbosity_str == "info")
                {
                    g_std_output->set_verbosity_level(arclog::VERBOSITY_INFO);
                }
                else if(verbosity_str == "debug")
                {
                    g_std_output->set_verbosity_level(arclog::VERBOSITY_DEBUG);
                }
                else
                {
                    g_logger->critical << "Unrecognised verbosity level "
                                       << "provided: \"" << verbosity_str
                                       << "\"" << std::endl;
                    return -1;
                }

                g_logger->info << "Running at verbosity level: "
                               << verbosity_str << std::endl;
            }
            else
            {
                g_logger->critical << "Incorrect usage of argument \"" << arg
                                   << "\". It must be followed by a verbosity "
                                   << "level to use." << std::endl;
                return -1;
            }
        }
        // table of contents
        else if(arg == ARG_TABLEOFCONTENTS)
        {
            // check there is another argument
            if(i < arg_count - 1)
            {
                // TODO: need a smarter way of splitting these paths
                g_toc = new arccol::TableOfContents(
                    arc::io::sys::Path(arc::str::UTF8String(argv[++i]))
                );
            }
            else
            {
                g_logger->critical << "Incorrect usage of argument \"" << arg
                                   << "\". It must be followed by the path to "
                                   << "write the table of contents to."
                                   << std::endl;
                return -1;
            }
        }
        // page size
        else if(arg == ARG_PAGE_SIZE)
        {
            // check there is another argument
            if(i < arg_count - 1)
            {
                arc::str::UTF8String page_size_s(argv[++i]);
                // ensure this is an int
                if(!page_size_s.is_int())
                {
                    g_logger->critical << "Incorrect usage of argument \""
                                       << arg << "\". The provided page size "
                                       << "must be an integral number, whereas "
                                       << "\"" << page_size_s << "\" was given."
                                       << std::endl;
                    return -1;
                }
                // store
                g_page_size = page_size_s.to_int64();
            }
            else
            {
                g_logger->critical << "Incorrect usage of argument \"" << arg
                                   << "\". It must be followed by the page "
                                   << "size to use." << std::endl;
                return -1;
            }
        }
        // read size
        else if(arg == ARG_READ_SIZE)
        {
            // check there is another argument
            if(i < arg_count - 1)
            {
                arc::str::UTF8String read_size_s(argv[++i]);
                // ensure this is an int
                if(!read_size_s.is_uint())
                {
                    g_logger->critical << "Incorrect usage of argument \""
                                       << arg << "\". The provided read size "
                                       << "must be an unsigned integral "
                                       << "number, whereas \"" << read_size_s
                                       << "\" was given." << std::endl;
                    return -1;
                }
                // store
                g_read_size = read_size_s.to_uint32();
            }
            else
            {
                g_logger->critical << "Incorrect usage of argument \"" << arg
                                   << "\". It must be followed by the read "
                                   << "size to use." << std::endl;
                return -1;
            }
        }
        // collate structure
        else if(arg == ARG_COLLATE_BEGIN)
        {
            // check there is another three arguments
            if(i < arg_count - 3)
            {
                // create a new Collator
                arccol::Collator* collator = new arccol::Collator(
                    g_toc,
                    arc::io::sys::Path(arc::str::UTF8String(argv[++i])),
                    g_page_size,
                    g_read_size
                );
                // read resources until we find the structure end
                arc::str::UTF8String sub_arg(argv[++i]);
                do
                {
                    // add the resource to the collator
                    collator->add_resource(arc::io::sys::Path(sub_arg));

                    // check for end of args
                    ++i;
                    if(i == arg_count)
                    {
                        g_logger->critical << arg << " was not followed by "
                                           << "the \"" << ARG_COLLATE_END
                                           << "\" argument." << std::endl;
                        return -1;
                    }

                    sub_arg = arc::str::UTF8String(argv[i]);
                }
                while(sub_arg != ARG_COLLATE_END);
                // store
                g_collators.push_back(collator);
            }
            else
            {
                g_logger->critical << "Incorrect usage of argument \"" << arg
                                   << "\". It must be followed by at least 3 "
                                   << "more arguments: the base path to write "
                                   << "the collated file to, one or more "
                                   << "resource files to write into the "
                                   << "collated file, and the \""
                                   << ARG_COLLATE_END << "\" argument to "
                                   << "signify the end of the definition."
                                   << std::endl;
                return -1;
            }
        }
        // help
        else if(arg == ARG_HELP)
        {
            show_help();
            return 0;
        }
        else
        {
            g_logger->warning << "Skipping unrecognised argument: \"" << arg
                              << "\"" << std::endl;
        }
    }

    return 0;
}

void report_state()
{
    // TODO:
}

void cleanup()
{
    if(g_toc != nullptr)
    {
        delete g_toc;
    }
    for(arccol::Collator* collator: g_collators)
    {
        delete collator;
    }
}

void show_help()
{
    arc::str::UTF8String divider("=");
    divider *= 80;

    std::cout << divider << std::endl;
    std::cout << APP_NAME << std::endl;
    std::cout << divider << std::endl;
    std::cout << "Arguments:" << std::endl;
    std::cout << "----------\n" << std::endl;
    std::cout << ARG_VERBOSITY << ": The verbosity this tool will report at, "
              << "possible values are:\n             critical, error, warning, "
              << "notice, info, and debug.\n" << std::endl;
    std::cout << ARG_HELP << ": Displays this help and exits.\n" << std::endl;
    std::cout << ARG_TABLEOFCONTENTS << ": Defines the file path where the "
              << "table of contents for the\n                     collated "
              << "resources will be written to.\n" << std::endl;
    std::cout << ARG_PAGE_SIZE << ": Defines the maximum size in bytes of "
              << "any single collated file, to\n             respect this "
              << "limit collated resources may be split across multiple\n      "
              << "       files. Defaults to -1 meaning there is not size "
              << "limit.\n" << std::endl;
    std::cout << ARG_READ_SIZE << ": Defines the maximum number of bytes that "
              << "will be read into memory at\n             anyone time. "
              << "Defaults to 268435456.\n" << std::endl;
    std::cout << ARG_COLLATE_BEGIN << ": Begins the definition of resources to "
              << "be collated. This\n                 argument should be "
              << "immediately followed by the base path to\n                 "
              << "write the resulting collated file(s) to, then followed by an"
              << "\n                 indefinite number of paths defining files "
              << "or directories to\n                 collate. If a directory "
              << "is supplied here, all child files\n                 will be "
              << "collated.\n" << std::endl;
    std::cout << ARG_COLLATE_END << ": Ends the definition of resources to be "
              << "collated." << std::endl;
}
