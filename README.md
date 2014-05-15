# ExtractDRS

A commandline utility for extracting .drs and .slp files from the
[Genie Game Engine](http://en.wikipedia.org/wiki/Genie_Engine),
which is used by Age of Empires (I & II) and Star Wars: Galactic Battlegrounds.

## Compiling

Easy.

`make`

`./extractdrs <directory/wheredrs/files/are/located/`

The program is written in C++11, so any reasonably up to date compiler should
work just fine. Compilation on Windows is unknown and untested, so may need
some minor modifications to get this to work. (Silly Visual Studio with its
not-quite POSIX support...) The only file I can think will have issues is
extractdrs.cpp and its include of dirent.h, which i know isn't included with
Windows.

## Resources

Much thanks to the people who are much cleverer than I, and were able to
document the file formats for these file types.

* [Age of Empires .drs format](http://artho.com/age/drs.html)
* http://alexander-jenkins.co.uk/blog/?p=9 (dead)
* [SLP - GenieWiki](http://www.digitization.org/wiki/index.php/SLP)
* http://www.ferazelhosting.net/~bryce/re.html (dead, but slp.rtf in repo)

## TODO

Inexhaustive list of other things to do:

* Fully implement the SLP extractor
  * Colour transform functions
  * Extended commands
* Apply proper palette to the SLP sprites
  * Based on info from .bin files
* Extract other files (saves, scenarios) ?
* Properly name extracted files (based on what the actual sprite represents) ?

## License

ExtractDRS is licensed under the GNU General Public License version 2.0. For
the complete license text, see the file 'COPYING'. This license applies to all
files in this distribution.
