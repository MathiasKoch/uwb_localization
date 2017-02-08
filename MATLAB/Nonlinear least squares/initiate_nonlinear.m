function [nl_struct] = initiate_nonlinear(anchor_pos)
    N = size(anchor_pos,2);
    nl_struct.c = 0;
    nl_struct.H = 0;
    for n = 1:N
        nl_struct.c = nl_struct.c + anchor_pos(:,n);
        nl_struct.H = nl_struct.H + (anchor_pos(:,n)*transpose(anchor_pos(:,n)));
    end
    nl_struct.c = nl_struct.c/N;
    nl_struct.H = -2/N * nl_struct.H + 2*nl_struct.c*transpose(nl_struct.c);
    
    for n = 1:2
        nl_struct.H_m(:,n) = nl_struct.H(:,n)-nl_struct.H(:,3);
    end
    nl_struct.H_m = transpose(nl_struct.H_m);
    [nl_struct.Q, nl_struct.U] = qr(nl_struct.H_m);
end