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


# Building and Dependencies

GTATools uses CMake >= 3.0 as its build system. It is meant to be built from the $ROOT/src directory. Building from a component directory might work, but it is rarely tested, so don't do it.

To compile GTATools, you need a recent compiler with decent support for C++11. The only officially supported compiler is GCC (on Windows: MinGW). Clang is sometimes tested and should work without too much hassle. **The Visual C++ compiler is currently not supported**, because I just don't have the nerves to fiddle around with it.

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

