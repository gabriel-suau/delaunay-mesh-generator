len = 0.1;

Point(1) = {0, 0, 0, len};
Point(2) = {0.4, 0, 0, len};
Point(3) = {0.3, 0.25, 0, len};
Point(4) = {0.2, 0.5, 0, 10*len};
Point(5) = {0.7, 0.3, 0, 10*len};
Point(6) = {0.5, 0.2, 0, len};
Point(7) = {1, 0, 0, len};
Point(8) = {1, 1, 0, len};
Point(9) = {0, 1, 0, len};
Point(10) = {0.4, 0.5, 0, 0.3*len};

Line(1) = {1, 2};
Line(2) = {2, 6};
Line(3) = {6, 7};
Line(4) = {7, 8};
Line(5) = {8, 9};
Line(6) = {9, 1};
Line(7) = {3, 4};
Line(8) = {4, 5};
Line(9) = {5, 3};
Line(10) = {4, 10};
Line(11) = {10, 8};

Curve Loop(1) = {1, 2, 3, 4, 5, 6};
Curve Loop(2) = {7, 8, 9};

Plane Surface(1) = {1, 2};

Curve{10, 11} In Surface {1};
