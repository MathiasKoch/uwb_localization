function F = myfun(x)

global data log


%a = 0;
%B = 0;
%c = 0;
%N = size(data.anchors,2);
%for n = 1:N
%    a = a + (data.anchors(:,n)*transpose(data.anchors(:,n))*data.anchors(:,n) - data.meas(index,n)^2*data.anchors(:,n));
%    B = B + (-2*data.anchors(:,n)*transpose(data.anchors(:,n)) - (transpose(data.anchors(:,n))*data.anchors(:,n))*eye(3) + data.meas(index,n)^2*eye(3));
%    c = c + data.anchors(:,n);
%end
%a = a/N;
%B = B/N;
%c = c/N;

%F = a + B*x + (2*x*transpose(x) + (transpose(x)*x)*eye(3))*c - x*transpose(x)*x;

c = 0;
G = [0;0;1];
N = size(log.anchors,2);
for n = 1:N
%    a = a + (data.anchors(:,n)*transpose(data.anchors(:,n))*data.anchors(:,n) - data.meas(index,n)^2*data.anchors(:,n));
%    B = B + (-2*data.anchors(:,n)*transpose(data.anchors(:,n)) - (transpose(data.anchors(:,n))*data.anchors(:,n))*eye(3) + data.meas(index,n)^2*eye(3));
    c = c + log.anchors(:,n);
end
%a = a/N;
%B = B/N;
%c = c/N;

%F = a + B*x + (2*x*transpose(x) + (transpose(x)*x)*eye(3))*c - x*transpose(x)*x + 2*G*(transpose(G)*x - transpose(G)*c + ALS(data.Mp) - ALS(data.presClean(index)));
e = 0;
for n = 1:N
    e = e + (transpose(log.anchors(:,n)-x)*(log.anchors(:,n)-x)-data.meas(n)^2)^2;
end

F = e ;%+ (W*(ALS(data.presClean(index)) - transpose(G)*x - ALS(data.Mp) + transpose(G)*c))^2;