
function F = myfunpres(x)

ALS = @(P) (-7*10^6)*log(P/101325);

global data index W



c = 0;
G = [0;0;1];
N = size(data.anchors,2);
for n = 1:N
    c = c + data.anchors(:,n);
end
c = c/N;

e = 0;
for n = 1:N
    e = e + (transpose(data.anchors(:,n)-x)*(data.anchors(:,n)-x)-data.meas(index,n)^2)^2;
end

F = e + (W*(ALS(data.pres(index)) - transpose(G)*x - ALS(data.Mp) + transpose(G)*c))^2;
