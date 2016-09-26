
%% Calculate Jacobians for EKF
clear all
N = 4;


syms dt

Ax = sym('Ax',[1 N]);
Ay = sym('Ay',[1 N]);
Az = sym('Az',[1 N]);

r = sym('r',[1 N]);

x = sym('x',[1 6]);

F = [1 0 0 dt 0 0
     0 1 0 0 dt 0
     0 0 1 0 0 dt
     0 0 0 1 0 0
     0 0 0 0 1 0
     0 0 0 0 0 1];
 
yhat = [r(1); r(2); r(3); r(4)];
 
h = [sqrt( (x(1)-Ax(1))^2 + (x(2)-Ay(1))^2 + (x(3)-Az(1))^2 )
     sqrt( (x(1)-Ax(2))^2 + (x(2)-Ay(2))^2 + (x(3)-Az(2))^2 )
     sqrt( (x(1)-Ax(3))^2 + (x(2)-Ay(3))^2 + (x(3)-Az(3))^2 )
     sqrt( (x(1)-Ax(4))^2 + (x(2)-Ay(4))^2 + (x(3)-Az(4))^2 )];
    
H = subs(simplify(jacobian(h,x)), h, yhat);



%% Calculate Jacobians for Dynamic EKF
clear all

START_STATES = 6;

dt = 0.1;

N = 5;


x = sym('x',[1 START_STATES+(N*3)]);

F = eye(length(x));

F(1,4) = dt;
F(2,5) = dt;
F(3,6) = dt;

rT = sym('rT', [N 1]);
 
%h_tag = zeros(N,1);
for n = 1:N
    ind = START_STATES + (((n-1)*3));
    h_tag(n) = sqrt( (x(1)-x(ind+1))^2 + (x(2)-x(ind+2))^2 + (x(3)-x(ind+3))^2 );
end

h_tag = transpose(h_tag);

    
HTagUpdate = subs(simplify(jacobian(h_tag,x)), h_tag, rT);

rA = sym('rA', [(N-1)*N/2 1]);

Rcnt = 1;
%h_anchor = zeros((N-1)*N/2, 1);
cnt = N;
for n = 1:N
    ind1 = START_STATES + (((n-1)*3));
    for j = 1:cnt-1
        ind2 = ind1 + 3*j;
        h_anchor(Rcnt) = sqrt( (x(ind1+1)-x(ind2+1))^2 + (x(ind1+2)-x(ind2+2))^2 + (x(ind1+3)-x(ind2+3))^2 );
        Rcnt = Rcnt + 1;
    end
    cnt = cnt - 1;
end

h_anchor = transpose(h_anchor);

HAnchorUpdate = subs(simplify(jacobian(h_anchor,x)), h_anchor, rA);


%% Initialize variables for simulation


addpath('~/3d_wb_loc/MATLAB/EKF')

global anchor_pos

anchor_pos = [0 0 0
              2 0 0
              1 2 0
              2 2 1
              3 1.5 0.5];
          
          
dt = 0.1;
dtDeca = 0.3;

N = size(anchor_pos,1);

velocityCovariance = 0.0000001;


xhat = [0.8;0.8;0.8;0;0;0;reshape(transpose(anchor_pos), [], 1) + randn(N*3,1).*0.5];           % State vector
P = eye(length(xhat), length(xhat));                % Covariance Matrix
Q =  zeros(length(xhat),length(xhat));              % Process Noise
Q(4,4) = velocityCovariance;
Q(5,5) = velocityCovariance;
Q(6,6) = velocityCovariance;
Q(end-N*3+1:end,end-N*3+1:end) = eye(N*3)*1e-13;
RDeca =  0.0025;                                    % Measurement Noise



i = 1;

%% Predict 

% Calculate the Jacobians for the state and measurement equations
F = [1 0 0 dt 0 0
     0 1 0 0 dt 0
     0 0 1 0 0 dt
     0 0 0 1 0 0
     0 0 0 0 1 0
     0 0 0 0 0 1];
 
yhat = [sqrt( (xhat(1)-anchor_pos(1,1))^2 + (xhat(2)-anchor_pos(1,2))^2 + (xhat(3)-anchor_pos(1,3))^2 )];
   %  sqrt( (xhat(1)-anchor_pos(2,1))^2 + (xhat(2)-anchor_pos(2,2))^2 + (xhat(3)-anchor_pos(2,3))^2 )
   %  sqrt( (xhat(1)-anchor_pos(3,1))^2 + (xhat(2)-anchor_pos(3,2))^2 + (xhat(3)-anchor_pos(3,3))^2 )
   %  sqrt( (xhat(1)-anchor_pos(4,1))^2 + (xhat(2)-anchor_pos(4,2))^2 + (xhat(3)-anchor_pos(4,3))^2 )];
    
H = [ -(anchor_pos(1,1) - xhat(1))/yhat(1), -(anchor_pos(1,2) - xhat(2))/yhat(1), -(anchor_pos(1,3) - xhat(3))/yhat(1), 0, 0, 0];
    %  -(anchor_pos(2,1) - xhat(1))/yhat(2), -(anchor_pos(2,2) - xhat(2))/yhat(2), -(anchor_pos(2,3) - xhat(3))/yhat(2), 0, 0, 0
    %  -(anchor_pos(3,1) - xhat(1))/yhat(3), -(anchor_pos(3,2) - xhat(2))/yhat(3), -(anchor_pos(3,3) - xhat(3))/yhat(3), 0, 0, 0
    %  -(anchor_pos(4,1) - xhat(1))/yhat(4), -(anchor_pos(4,2) - xhat(2))/yhat(4), -(anchor_pos(4,3) - xhat(3))/yhat(4), 0, 0, 0];

% Propogate the state and covariance matrices
%xhat = F*xhat;
%P = F*P*F' + Q;


% Correct

% Calculate the Kalman gain
K = P*H'/(H*P*H' + RDeca);

% Update the state and covariance estimates
res = (meas(i,:)' - yhat)
inno = K*res
xhat = xhat + inno;
P = (eye(size(K,1))-K*H)*P;

i = i + 1;

%% Generate the C header file after simulation

meas = Ranges.signals.values(1:1024,:);


fd=fopen('values.h','wt');
fprintf(fd,'extern float states[%d];\n',numel(xhat));
fprintf(fd,'extern float P[%d];\n',numel(P));
fprintf(fd,'extern float Q[%d];\n',numel(Q));
fprintf(fd,'extern float R[%d];\n',numel(RDecae));
fprintf(fd,'extern float anchor_pos[%d];\n',numel(anchor_pos));
fprintf(fd,'extern const float VALS[%d][4];\n',size(meas,1));
fclose(fd)


fd=fopen('values.c','wt');

% Include
fprintf(fd, '#include "values.h"\n\n');

% States
fprintf(fd, '// States\n');
fprintf(fd,'float states[%d]={%.22f',numel(xhat),xhat(1));
for n=2:numel(xhat)
    fprintf(fd,',%.22f',xhat(n));
end

fprintf(fd, '};\n\n');

% Covariance
fprintf(fd, '// Covariance\n');
fprintf(fd,'float P[%d]={%.22f, %.22f, %.22f, %.22f, %.22f, %.22f',numel(P),P(1,1),P(1,2),P(1,3), P(1,4), P(1,5), P(1,6));
for n=2:size(P,1)
    fprintf(fd,',%.22f, %.22f, %.22f, %.22f, %.22f, %.22f',P(n,1),P(n,2),P(n,3), P(n,4), P(n,5), P(n,6));
end

fprintf(fd, '};\n\n');

% Process Noise
fprintf(fd, '// Process Noise\n');
fprintf(fd,'float Q[%d]={%.22f, %.22f, %.22f, %.22f, %.22f, %.22f',numel(Q),Q(1,1),Q(1,2),Q(1,3), Q(1,4), Q(1,5), Q(1,6));
for n=2:size(Q,1)
    fprintf(fd,',%.22f, %.22f, %.22f, %.22f, %.22f, %.22f',Q(n,1),Q(n,2),Q(n,3), Q(n,4), Q(n,5), Q(n,6));
end

fprintf(fd, '};\n\n');


% Measurement Noise
fprintf(fd, '// Measurement Noise\n');
fprintf(fd,'float R[%d]={%.22f, %.22f, %.22f, %.22f',numel(RDeca),RDeca(1,1),RDeca(1,2),RDeca(1,3), RDeca(1,4));
for n=2:size(RDeca,1)
    fprintf(fd,',%.22f, %.22f, %.22f, %.22f',RDeca(n,1),RDeca(n,2),RDeca(n,3), RDeca(n,4));
end

fprintf(fd, '};\n\n');


% Anchor Pos
fprintf(fd, '// Anchor Position\n');
fprintf(fd,'float anchor_pos[%d]={%.22f, %.22f, %.22f',numel(anchor_pos),anchor_pos(1,1),anchor_pos(1,2),anchor_pos(1,3));
for n=2:size(anchor_pos,1)
    fprintf(fd,',%.22f, %.22f, %.22f',anchor_pos(n,1),anchor_pos(n,2),anchor_pos(n,3));
end

fprintf(fd, '};\n\n');

% Measurements
fprintf(fd, '// Measurements\n');
fprintf(fd,'const float VALS[%d][4]={{%.22f, %.22f, %.22f, %.22f}',size(meas,1),meas(1,1),meas(1,2),meas(1,3),meas(1,4));
for n=2:size(meas,1)
    fprintf(fd,',\n {%.22f, %.22f, %.22f, %.22f}',meas(n,1),meas(n,2),meas(n,3),meas(n,4));
end
fprintf(fd,'};\n');


fclose(fd)

