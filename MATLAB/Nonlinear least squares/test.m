%% Generate measurement and pressure data based on known anchor positions and given tag positions
clear all
%AltToPres = @(Z) 101325 * (1 - 2.25577*10^-5 * (Z*10^-3))^5.25588;   % [Pa]
AltToPres = @(Z,SNR) awgn(101325 * exp(-(Z/(7*10^6))), SNR, 'measured');   % [Pa]

%ALS = @(P) ((((1015.7/P)^0.190263 - 1) * 298.15)/0.0065);
ALS = @(P) (-7*10^6)*log(P/101325);


global data

data.anchors     = [0     0    2960     % Anchor 1
                   2480  0    2960     % Anchor 2
                   2470  1630 2960     % Anchor 3
                   0     1630 2960]';  % Anchor 4       % [mm]
               
           

data.ALS0 = 2000;                                        % [mm]        



% Single point
%data.posTrue = [459; 2026; 2960]';                         % [mm]

% Linear motion path in all 3 axis
%N = 50;
%data.posTrue = [linspace(400, 1500, N)' linspace(500, 1900, N)' linspace(800, 1000, N)'];                         % [mm]

% Helix path
t = 0:pi/50:8*pi;
st = sin(t);
ct = cos(t);
data.posTrue = [st'*1000 ct'*1000 t'*100];

figure(5)
plot3(data.posTrue(:,1), data.posTrue(:,2), data.posTrue(:,3),'xg')
hold on
grid on
plot3(data.anchors(1,:)', data.anchors(2,:)', data.anchors(3,:)','xm','MarkerSize',10,'LineWidth',3)

% Generate clean data
data.Mz = sum(data.anchors(3,:)) / size(data.anchors,2);   % [mm]
data.Mp = AltToPres(data.Mz, 40);                             % [Pa]

data.measClean = zeros(size(data.posTrue,2),size(data.anchors,2));
data.presClean = zeros(1,size(data.posTrue,2));

for u = 1:size(data.posTrue,1)
    for n = 1:size(data.anchors,2)
        data.measClean(u,n) = sqrt(sum((transpose(data.posTrue(u,:))-data.anchors(:,n)).^2));
    end
    data.pres(u) = AltToPres(data.posTrue(u,3), 95);
end

%ColorNoise = 2;

%data.Mp = data.Mp + ColorNoise;     
%data.presCleanNoise = data.presClean + ColorNoise;

% Add noise
data.meas = awgn(data.measClean, 30, 'measured');


%%

figure()          
plot(ALS(data.pres),'b')         
hold on
plot(data.posTrue(:,3),'r')

%%

global index W

W = 20;
nl_struct = initiate_nonlinear(data.anchors);

x = zeros(size(data.meas,1),3);
xM = zeros(size(data.meas,1),3);
xMP = zeros(size(data.meas,1),3);

x0M = [1;1;1];
x0MP = [1;1;1];
sum1 = 0;
sum2 = 0;
error1 = 0;
error2 = 0;
options = optimoptions(@lsqnonlin,'Algorithm','levenberg-marquardt','MaxIter', 4, 'TolFun',1e-2,'Display','none');
for n = 1:size(data.meas,1)
    index = n;
    tic
    xM(n,:) = lsqnonlin(@myfun,x0M,[],[],options);
    sum1 = sum1 + toc;
    error1 = error1 + sqrt(sum(transpose((data.posTrue(n,:)'-xM(n,:)').^2)));
    tic
    xMP(n,:) = lsqnonlin(@myfunpres,x0MP,[],[],options);
    x(n,:) = calculate_nonlinear(nl_struct, data.anchors, data.meas(n,:))';
    sum2 = sum2 + toc;
    error2 = error2 + sqrt(sum(transpose((data.posTrue(n,:)'-xMP(n,:)').^2)));
    x0M = xM(n,:)';
    x0MP = xMP(n,:)';
end

disp(['MATLAB lsqnonlin without pressure: ' num2str(sum1/n) ' Seconds avg, with avg error: ' num2str(error1/n)])
disp(['MATLAB lsqnonlin with pressure: ' num2str(sum2/n) ' Seconds avg, with avg error: ' num2str(error2/n)])
disp(['MATLAB lsqnonlin without pressure ' num2str((sum1/n)/(sum2/n)) ' times faster'])

plot3(xM(:,1), xM(:,2), xM(:,3),'r')
plot3(x(:,1), x(:,2), x(:,3),'g')
plot3(xMP(:,1), xMP(:,2), xMP(:,3),'b')
%legend('True path','Anchors','MATLAB', 'Algebraic')
legend('True path','Anchors','Estimate no pressure', ['Estimate pressure, W=' num2str(W)])




