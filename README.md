# delaunay-mesh-generator

Implementation of a 2D simplicial mesh generator using the constrained Delaunay algorithm. DMG takes a 2D boundary mesh as input, and outputs a 2D simplicial volume mesh.

## Installation, compilation and execution
You can get the sources from here by typing in your terminal :

```shell
git clone https://github.com/gabriel-suau/delaunay-mesh-generator.git DMG
```

DMG uses CMake to generate a Makefile based on the user's configuration. To build the <code>dmg</code> executable, execute the following commands in your terminal (assuming you are in the project's top directory) :

```shell
mkdir build
cd build
cmake ..
make
```

This will produce an executable called <code>dmg</code> inside a <code>bin</code> directory located in the project's top directory. To execute the program, you can type :
```shell
/path/to/dmg input_boundary.mesh output_mesh.mesh
```

## Inputs and outputs
For the moment, DMG can only read and write meshes in the ASCII [Medit](https://www.ljll.math.upmc.fr/frey/logiciels/Docmedit.dir/index.html) format. More formats may (or may not) be added in the future.

## Features
Here is a small checklist of the implemented features and the features that still need to be implemented :
- [x] Read a discretized boundary of a 2D domain from a Medit formatted file,
- [x] Create a rectangular bounding box divided in 2 triangles,
- [x] Insert the boundary points using the Bowyer-Watson algorithm,
- [x] Enforce the boundary edges,
- [x] Mark the subdomains and delete the bounding box,
- [x] Refine the mesh by inserting volume points in order to achieve a desired edge length.
- [ ] Optimizations (edge swaps, collapses and node relocation)

## Documentation
A Doxygen documentation can be built by enabling the `DMG_DOC` option when configuring the project.

## Credits
All credits go to Gabriel Suau and Lucas Trautmann.

## License
This project is distributed under the [GNU-GPLv3](https://www.gnu.org/licenses/gpl-3.0.html) license. A copy of the whole license is included in the repository.
