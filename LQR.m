m = 1;
g = 10;
u = 0.5;

A = [0 0 0;
     1 0 0;
     0 1 0];
B = [1/m -u*g;
     0   0;
     0   0];
C = [1 0 0];
D = 0;
lqr = ss(A,B,C,D,'StateName',{'a' 'v' 'x'},'InputName',{'Force' '1'},'OutputName','x');
bodeplot(lqr,'g--')
sys_tf  = tf(lqr)

% A = [0 1;
%      0 0];
% B = [0; 
%      1];
% C = [1 0];
% D = 0;
% sys_dc = ss(A,B,C,D,'StateName',{'x' 'v'},'InputName',{'a'},'OutputName','v');
% sys_tf = tf(sys_dc)