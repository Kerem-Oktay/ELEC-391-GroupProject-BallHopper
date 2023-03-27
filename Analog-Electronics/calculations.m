s = tf('s');    %back to laplace s 

Ts = 300;       %(ms) first time it gets %2 close to settle value after the rise
Kdc = 12;       %gain = y value of Tsettle
Z = 1;          %(pure) zeta = 1 when critically damped
beta = 0;       %(pure) beta = 0 when critically damped

% Wn = 4/(Z*Ts*10^(-3));               %(rad/s) when critically damped Tr and Tp can't be used so omega = 4/(zeta*Ts)
% G = Kdc*(Wn^2)/(s^2+2*Z*Wn*s+Wn^2);  %(V/V) xfer = Kdc*omega^2/(s^2+2*zeta*omega*s+omega^2)

%other way
syms s t w                                                                        %turns laplace s to letter s (to defie a value or do ilaplace)
step_response_s = Kdc/s*w^2/(s^2+2*w*s+w^2);
step_response_t = ilaplace(step_response_s,t);
Wn = double(vpasolve(subs(step_response_t,t,Ts) == 0.98*Kdc, w, 100))*1000;
s = tf('s');                                                                      %back to laplace s 
G = Kdc*(Wn)^2/(s^2+2*Z*Wn*s+(Wn)^2);                                             %(V/V) xfer = Kdc*omega^2/(s^2+2*zeta*omega*s+omega^2)

%plots to check
if 1
    [y t] = step(G);
    t = t*10^3;
    figure(1); clf;
    hold on
    plot(t,y);
    grid on
    hold off
end