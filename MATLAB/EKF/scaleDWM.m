function scale = scaleDWM(xhat)

global anchor_pos

B = transpose(reshape(xhat(7:21), 3,[]));

[~,~,tr] = procrustes(B,anchor_pos);

if isstruct(tr)
    scale = tr.b;
else
    scale = 0;
end