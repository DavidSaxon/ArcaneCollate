![](http://i.imgur.com/ztki1s3.gif)

ArcaneCollate provides functionality for writing and reading application
resource files into collated files that contain multiple or all of the
application's resources. An API is provided for both writing and reading from
from collated files and for convenience a command line tool is supplied to
write collated files.

## Technical Overview

ArcaneCollate works by collating resources (files on disk) into a single
collated file. This file may be broken into smaller subfiles referred to as
pages, however these pages still make a contiguous block of data, so these
multiple pages are still considered to be a single collated file. In order to
locate resources in collated files a separate file called the Table of
Contents exists that stores a mapping from resources to their location in a
collated file. A Table of Contents may contain resources that are from
multiple collated files.

## Using the Command Line Tool to Write Collated Files.

The command line tool can be used to write one or more collated files
from existing resources on disk, and generate a Table of Contents denoting
the location of resources in the collated files.

A path to the Table of Contents to generate must be specified using
the `--table_of_contents` argument, and at least one collated file must
be defined using the `--collate_begin` and `--collate_end` arguments.

Example usage:

```
arcanecollate --table_of_contents res/locations.toc --collate_begin
       res/images.arccol res/images/logo.png res/images/textures/ --collate_end
       --collate_begin res/fonts.arccol res/fonts/font_1.ttf
       res/fonts/font_2.ttf --collate_end
```

The above command will generate 3 new files: `res/locations.toc` the
Table of Contents which describes where resources are located in collated
files, `res/images.arccol.0` a collated file containing
images, and `res/fonts.arccol.0` a collated file containing fonts. The
other file paths in the command are the paths of resources to write to the
related collated files. If a file is specified here,
i.e. `res/images/logo.png` that exact file is written to the collated
file. If a directory is specified i.e. ` res/images/textures/` all files
within the directory are written to the collated file.

Note that collated files produced by this command `res/images.arccol.0`
and `res/fonts.arccol.0` have `.0` appended to the end of the path
that was specified for them: this is the page number. By default, collated
files have no size limit on them, but using the `--page_size` argument a
size limit on collated files can be enforced. The size limit is implemented
by splitting the collated file across sub files, each with their page number
appended to the end of the collated path, i.e. `.0`, `.1`, `.2`
and so on. In the example command no page limit is set, but by a matter of
convention the page number is still appended to the end of the collated file
path.

## Writing using the API

Writing collated files using the API requires at least two objects:
arccol::TableOfContents and one or more arccol::Collator.

The first step is to create the arccol::TableOfContents which will store the
resource locations, and eventually generate the Table of Contents file on
disk. First we will create the arccol::TableOfContents with the file path we
want the Table of Contents file to be written to:

```
arc::io::sys::Path toc_path;
toc_path << "res" << "locations.toc";
arccol::TableOfContents toc(toc_path);
```

Like the command line tool example, in this example we will write two
collated files, one for images and one for fonts. To do this we will need to
generate arccol::Collator objects for each. These objects take at least the
pointer to the arccol::TableOfContents to record the locations of resources,
and the path to the collated file to write (this should not include the page
index extension).

```
arc::io::sys::Path images_path;
images_path << "res" << "images.arccol";
arccol::Collator images_collator(&toc, images_path);

arc::io::sys::Path fonts_path;
fonts_path << "res" << "fonts.arccol";
arccol::Collator fonts_collator(&toc, fonts_path);
```

The next step is to add the resources to each arccol::Collator:

```
arc::io::sys::Path logo_path;
logo_path << "res" << "images" << "logo.png";
images_collator.add_resource(logo_path);

arc::io::sys::Path textures_path;
textures_path << "res" << "images" << "textures";
images_collator.add_resource(textures_path);

arc::io::sys::Path font_1_path;
font_1_path << "res" << "fonts" << "font_1.ttf";
fonts_collator.add_resource(font_1_path);

arc::io::sys::Path font_2_path;
font_2_path << "res" << "fonts" << "font_2.ttf";
fonts_collator.add_resource(font_2_path);
```

The final step is to actually performing the writing of the collated files
and the Table of Contents. Note that the arccol::Collator::execute()
functions should be called before the arccol::TableOfContents::write()
function.

```
images_collator.execute();
fonts_collator.execute();

toc.write();
```

## Reading using the API

In order to read from collated files we must use a arccol::Accessor to load
the Table of Contents file and access the locations of resources in collated
files:

```
arc::io::sys::Path toc_path;
toc_path << "res" << "locations.toc";
arccol::Accessor accessor(toc_path);
```

Here we could use the arccol::Accessor::get_resource() function to find
information about where to find the resource in a collated file and manually
read the subsection of the collated file to get the resource data.
However this proccess can be quite complicated since if a collated file is
broken into smaller pages a resource's data may exist in more than one file
on disk (however the resource's data is always contiguous within the pages).

ArcaneCollate provides the arccol::Reader object to handle reading resources
from collated files. arccol::Reader inherits from arc::io::sys::FileReader so
provides a familiar interface, and can be treated like any other reader. In
order to open an arccol::Reader to a resource we must provide it with an
arccol::Accessor object which represents the Table of Contents and the path
to the resource to read:

```
arc::io::sys::Path logo_path;
logo_path << "res" << "images" << "logo.png";
arccol::Reader image_reader(&accessor, logo_path);
```

If the resource does not exist in the accessor's Table of Contents the
resource will simply be read from its real file path. From here we can use
any of the common file reader functions to access the data in the resource.
i.e. tell, seek, eof, read() etc.
