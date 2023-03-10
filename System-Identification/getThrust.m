angle = 31.42;
meu = 0.25;
weight = 0.594;
d1 = 0.2;
d2 = 0.045;
Dp = getm(0.5);
meu2 = 0.04;
lead = getm(1 / 10);
phi = 15;
syms p1x p1y p2 Fn;
equationOne = p1x + p2*cosd(angle) - Fn == 0;
equationTwo = p1y - p2*sind(angle) - Fn*meu == 0;
equationThree = Fn*d1 == p2*d2;
equationFour = 3*meu*Fn - weight == 0;
equations = [equationOne, equationTwo, equationThree, equationFour];
solutions = solve(equations, [p1x, p1y, p2, Fn]);
thrust = 3 * solutions.p2 * sind(angle);
thrust = vpa(thrust) %#ok<NOPTS> 
tan_lambda = lead / (pi * Dp);
torque = (thrust * Dp / 2) * (((cosd(phi) * tan_lambda) + meu2) / (cosd(phi) - (meu2 * tan_lambda)));
torque = vpa(torque) %#ok<NOPTS> 