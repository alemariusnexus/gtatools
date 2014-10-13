GTATools
========

gtatools is a collection of cross-platform (currently officially Windows and Linux, but should run on other UNIXes without major changes) libraries and tools for working with the files of the GTA game series. Support is planned for the GTA III trilogy (III, Vice City, San Andreas).

gtatools currently consists of the following parts:

* **libgtaformats**: A library for low-level support of the various GTA file formats.
* **libgta**: A library that provides an engine which should work as a base for creating programs using the GTA files. This also includes the rendering system based on OpenGL
* **gtaimg**: A command line tool to work with IMG archives
* **gtatxd**: A command line tool to work with TXD archives
* **gtatools-gui**: A GUI tool based on Qt4 which currently serves as a debugging and testing tool for gtatools, but may also be useful to work with GTA files
* **gtatools-test**: A simple and ugly prototyping program used to test new features. Most of the time, it just renders the static map along with some testing stuff for features that are currently tested.



# Binaries and Prebuilt Dependencies

Binaries and prebuilt dependencies for Windows can be downloaded [here](http://alemariusnexus.com/gtatools).



# Building and Dependencies

GTATools uses CMake >= 3.0 as its build system (prior versions will **not** work). It is meant to be built from the $ROOT/src directory. Building from a component directory might work, but it is rarely tested, so don't do it.

To compile GTATools, you need a recent compiler with decent support for C++11. The mainly supported compiler is GCC. On Windows, you should use MinGW-w64 (still 32-bit). The original MinGW will probably not work because of some missing C++11 standard library features. The Visual Studio 2013 compiler is also somehow supported, though you have to prepare for a storm of compile-time warnings. Earlier versions of Visual Studio are not supported.

The components of GTATools depend on the following libraries. Note that this might change quite rapidly as GTATools is evolving:

* **libgtaformats**:
  * **libnxcommon**: My general-purpose library. Be sure to use the correct (i.e. current) version of libnxcommon. It is hosted [here](https://github.com/alemariusnexus/nxcommon). Also check out its own dependencies.
  * **libsquish** (optional): For DXT texture (de)compression. This is not strictly needed if you only want to render DXT compressed textures on a modern GPU that has GL_EXT_texture_compression_s3tc, but gtatxd and gtatools-gui need it to work with them. You can get it [here](https://code.google.com/p/libsquish/)
* **libgta**:
  * **libgtaformats**: Including all its dependencies
  * **OpenGL**: Version >= 3.0 is needed.
  * **GLEW**: OpenGL extension wrangler library
  * **Lua**
  * **Bullet**: The Bullet physics engine (version 2.x).
  * **CEGUI**
* **gtaimg**:
  * **libgtaformats**: Including all its dependencies
* **gtatxd**:
  * **libgtaformats**: Including all its dependencies
  * **libpng**
  * **zlib**: For libpng
* **gtatools-gui**:
  * **libgtaformats**: Including all its dependencies
  * **libgta**: Including all its dependencies
  * **Qt 4**: Qt 5 is not supported yet.

Prebuilt dependencies for Windows can be downloaded [here](http://alemariusnexus.com/gtatools).

Some hints for building gtatools from source:

* You can compile libnxcommon along with gtatools. See the file src/README_NXCOMMON for more information.
* libnxcommon must be compiled with support for Bullet (if you want to build libgta) and Qt4/Qt5 (if you want to build gtatools-gui). Look for the CMake variables NXCOMMON_BULLET_SUPPORT and NXCOMMON_QT_SUPPORT.
* When using the prebuilt dependencies, CMake will not find many libraries automatically. Look for them in the lib/ directory of the dependency package and specify them manually in CMake (you might have to enable advanced mode).
* If you get missing symbol errors for GLEW, especially in Visual Studio, try adding /DGLEW_STATIC to CMAKE_CXX_FLAGS.
* For running gtatools-test, you'll need to place a small config.xml in either the directory containing the executable, or the executable's working directory. You can find a sample config.xml [here](http://alemariusnexus.com/gtatools/files/config.xml).



# Licensing

gtatools is licensed under the [GNU General Public License 3.0 (GPLv3)](http://www.gnu.org/copyleft/gpl.html), or any later version of that license. This means that you are allowed to use, redistribute and modify gtatools as long as you comply to the terms of the GPLv3. In particular, this means if you link to gtatools' libraries, your program must be open-source, too. If you feel that you have some really good reasons for using gtatools in a closed-source project, please contact me via E-Mail.
