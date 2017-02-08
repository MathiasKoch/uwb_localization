function [xyz] = calculate_nonlinear_pres(nl_struct, anchor_pos, measurements, pressure, mp, W, ALS)


    a = 0;
    B = 0;
    G = [0;0;1];
    N = size(anchor_pos,2);
    for n = 1:N
        a = a + (anchor_pos(:,n)*transpose(anchor_pos(:,n))*anchor_pos(:,n) - measurements(n)^2*anchor_pos(:,n));
        B = B + (-2*anchor_pos(:,n)*transpose(anchor_pos(:,n)) - (transpose(anchor_pos(:,n))*anchor_pos(:,n))*eye(3) + measurements(n)^2*eye(3));
    end
    a = a/N;
    B = B/N;
    
    f = a + B*nl_struct.c + 2*nl_struct.c*transpose(nl_struct.c)*nl_struct.c + 2*W^2*G*(ALS(mp)-ALS(pressure));

    f_m = zeros(2,1);
    for n = 1:2
        f_m(n) = f(n)-f(3);
    end


    v = transpose(nl_struct.Q)*f_m;

    qtq = 0;
    for n = 1:N
        qtq = qtq + (measurements(n)^2 - transpose(anchor_pos(:,n))*anchor_pos(:,n));
    end
    qtq = qtq/N + transpose(nl_struct.c)*nl_struct.c - 2*W^2;


    g1 = (nl_struct.U(1,2)*v(2))/(nl_struct.U(1,1)*nl_struct.U(2,2)) - v(1)/nl_struct.U(1,1);

    g2 = (nl_struct.U(1,2)*nl_struct.U(2,3))/(nl_struct.U(1,1)*nl_struct.U(2,2)) - nl_struct.U(1,3)/nl_struct.U(1,1);
    
    g31 = v(2)/nl_struct.U(2,2);
    
    g32 = nl_struct.U(2,3)/nl_struct.U(2,2);
    
    q(3,1) = -(g1*g2 + g31*g32 + (- g1^2*g32^2 - g1^2 + 2*g1*g2*g31*g32 - g2^2*g31^2 + qtq*g2^2 - g31^2 + qtq*g32^2 + qtq)^(1/2))/(g2^2 + g32^2 + 1);
    q(3,2) = -q(3,1);

    q(1,1) = g1+g2*q(3,1);
    q(1,2) = g1+g2*q(3,2);

    q(2,1) = -g31-g32*q(3,1);
    q(2,2) = -g31-g32*q(3,2);

    qo = zeros(2,3);

    qo(1,:) = q(:,1) + nl_struct.c;
    qo(2,:) = q(:,2) + nl_struct.c;

    xyz = qo(1,:);
    
    clc
    B*q(:,1) + 2*nl_struct.c*transpose(nl_struct.c)*q(:,1) + q(:,1)*transpose(nl_struct.c)*nl_struct.c - q(:,1)*transpose(q(:,1))*q(:,1) + 2*W^2*G*transpose(G)*q(:,1) - nl_struct.H*q(:,1) 
    B*q(:,1) + 2*nl_struct.c*transpose(nl_struct.c)*q(:,1) + q(:,1)*transpose(nl_struct.c)*nl_struct.c - q(:,1)*transpose(q(:,1))*q(:,1) + 2*W^2*G*transpose(G)*q(:,1) + f
    nl_struct.H*q(:,1) + f
    
    %Check1P = f + nl_struct.H*q(:,1)
    %Check2P = (a + B*nl_struct.c + 2*nl_struct.c*transpose(nl_struct.c)*nl_struct.c + 2*W^2*G*(ALS(mp)-ALS(pressure))) + (B*q(:,1) + (2*nl_struct.c*transpose(nl_struct.c) + (transpose(nl_struct.c)*nl_struct.c)*eye(3))*q(:,1)) - q(:,1)*transpose(q(:,1))*q(:,1) - 2*W^2*G*transpose(G)*q(:,1)
    
end