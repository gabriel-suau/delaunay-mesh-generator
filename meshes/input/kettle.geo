// A adapter en fonction du raffinement de maillage souhaité
h = 0.05;

// Rayon de la bouilloire
rk = 0.1;
// Centre du disque
xk = 0.0; yk = 0.0;
Point(1) = {xk, yk, 0.0, h};
Point(2) = {xk-rk, y1, 0.0, h/2};
Circle(1) = {2,1,2};

radius = 0.04;
cosangl1 = 0;
sinangl1 = 1;
cosangl2 = -0.866;
sinangl2 = -0.5;
cosangl3 = 0.866;
sinangl3 = -0.5;

// Résistances

// Rayon des résistances
rr = 0.015;
// Centre du disque resistance 1
xr1 = radius*cosangl1; yr1 = radius*sinangl1;
Point(3) = {xr1, yr1, 0.0, h/4};
Point(4) = {xr1-rr, yr1, 0.0, h/8};
Circle(2) = {4,3,4};

// Centre du disque resistance 2
xr2 = radius*cosangl2; yr2 = radius*sinangl2;
Point(5) = {xr2, yr2, 0.0, h/4};
Point(6) = {xr2-rr, yr2, 0.0, h/8};
Circle(3) = {6,5,6};


// Centre du disque resistance 3
xr3 = radius*cosangl3; yr3 = radius*sinangl3;
Point(7) = {xr3, yr3, 0.0, h/4};
Point(8) = {xr3-rr, yr3, 0.0, h/8};
Circle(4) = {8,7,8};

Line Loop(1) = {1}; // La bouilloire
Line Loop(2) = {2}; // Le 1° cercle
Line Loop(3) = {3}; // Le 2° cercle
Line Loop(4) = {4}; // Le 3° cercle

Plane Surface(1) = {1,2,3,4}; // La bouilloire et les cercles
