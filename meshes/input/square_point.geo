len = 0.1;

Point(1) = {0, 0, 0, len};
Point(2) = {0, 1, 0, len};
Point(3) = {1, 1, 0, len};
Point(4) = {1, 0, 0, len};
Point(5) = {0.5, 0.5, 0, 0.1*len};

Line(1) = {2, 3};
Line(2) = {1, 2};
Line(3) = {3, 4};
Line(4) = {4, 1};

Curve Loop(1) = {2, 1, 3, 4};

Surface(1) = {1};
