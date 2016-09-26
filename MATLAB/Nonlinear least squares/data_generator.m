%% Generate measurement and pressure data based on known anchor positions and given tag positions

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

figure(1)
plot3(data.posTrue(:,1), data.posTrue(:,2), data.posTrue(:,3),'x')
hold on
grid on
plot3(data.anchors(1,:)', data.anchors(2,:)', data.anchors(3,:)','xm','MarkerSize',10,'LineWidth',3)

% Generate clean data
data.Mz = sum(data.anchors(3,:)) / size(data.anchors,2);   % [mm]
data.Mp = AltToPres(data.Mz, 10000);                             % [Pa]

data.measClean = zeros(size(data.posTrue,2),size(data.anchors,2));
data.presClean = zeros(1,size(data.posTrue,2));

for u = 1:size(data.posTrue,1)
    for n = 1:size(data.anchors,2)
        data.measClean(u,n) = sqrt(sum((transpose(data.posTrue(u,:))-data.anchors(:,n)).^2));
    end
    data.presClean(u) = AltToPres(data.posTrue(u,3), 10000);
end

ColorNoise = 2;

data.Mp = data.Mp + ColorNoise;     
data.presCleanNoise = data.presClean + ColorNoise;

% Add noise
data.meas = awgn(data.measClean, 400, 'measured');


%% Estimate posTrue by weighted nonlinear least squares from measurements, anchor positions and pressure

W = 1;

%nl_struct = initiate_nonlinear(data.anchors);
%nl_structP = initiate_nonlinear_pres(data.anchors,W);

x = zeros(size(data.meas,1),3);
%xp = zeros(size(data.meas,1),3);
for n=1:size(data.meas,1)
    x(n,:) = round(calculate_nonlinear(nl_struct, data.anchors, data.meas(n,:))',4);
    %xp(n,:) = round(calculate_nonlinear_pres(nl_structP, data.anchors, data.meas(n,:), data.presClean(n), data.Mp, W, ALS)',4);
end

plot3(x(:,1), x(:,2), x(:,3),'r')
%hold on
%plot3(xp(:,1), xp(:,2), xp(:,3),'g')
legend('True path','Anchors','Estimate no pressure','Estimate w. pressure')
