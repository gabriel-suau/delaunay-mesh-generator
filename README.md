# delaunay-mesh-generator

Implementation of a 2D simplicial mesh generator using the Delaunay kernel. DMG takes a 2D boundary mesh as input, and outputs a 2D simplicial volume mesh.

## Installation, compilation and execution
You can get the sources from here by typing in your terminal :

```shell
git clone https://github.com/gabriel-suau/delaunay-mesh-generator.git DMG
```

DMG uses CMake to generate a Makefile based on the user's configuration. To build and install the <code>dmg</code> executable, execute the following commands in your terminal (assuming you are in the project's top directory) :

```shell
mkdir build
cd build
cmake ..
make
make install
```

To execute the program, you can type :
```shell
/path/to/dmg input_boundary.mesh output_mesh.mesh
```

## Inputs and outputs
For the moment, DMG can only read and write meshes in the ASCII Medit format. More formats may (or may not) be added in the future.


## Documentation
A Doxygen documentation will be included in a future release.

## Credits
Coming soon...

## License
This project is distributed under the [GNU-GPLv3](https://www.gnu.org/licenses/gpl-3.0.html) license. A copy of the whole license is included in the repository.
