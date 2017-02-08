%% Initialization

EDM = [];
PWR = [];

%% Measurements
% Run this part N times as per described in APS014: DW1000 Antenna Delay
% Calibration



log.AVG = 200;
log.BURN = 100;
         
             
delete(instrfindall);
s = serial('/dev/ttyS101');
set(s,'BaudRate', 115200);
set(s,'DataBits', 8);
set(s,'StopBits', 1);
fopen(s);
s.ReadAsyncMode = 'continuous'; 
readasync(s);
while(s.BytesAvailable <= 0)               
    %
end


log.time = [];
log.raw = [];
log.rxPower = [];

anchorId = 0;
i = 1;
j = 1;

trueDist = 5.0;
THRESH = 1.0;

Fs = 0.01;

anchors = [];

while(j <= log.AVG)
    sSerialData = fscanf(s);            
    flushinput(s);
    t = strsplit(sSerialData,', ');
    
    if(i > log.BURN)
        
        log.time = [log.time j*Fs];
        
        MyID = str2double(t(1));

        anchorId = str2double(t(2));
        
        if(numel(find(anchors==anchorId))==0)
            anchors = [anchors anchorId];
        end
        
        log.raw(j,anchorId) = str2double(t(3));
        %if(log.raw(j,anchorId) <= trueDist - THRESH || log.raw(j,anchorId) > trueDist + THRESH)
        %    if(j > 2)
        %        log.raw(j,anchorId) = log.raw(j-1,anchorId);
        %    end
        %end

        log.rxPower(j,anchorId) = str2double(t(4));
        if(log.rxPower(j,anchorId) <= -100.0 || log.rxPower(j,anchorId) > -40.0)
            if(j > 2)
                log.rxPower(j,anchorId) = log.rxPower(j-1,anchorId);
            end
        end


        j = j + 1;
    end
    i = i + 1;
end


d = log.raw;

dsum = zeros(1,size(d,2));
cnt = zeros(1,size(d,2));
for w=1:size(d,2)
    for v=1:size(d,1)
        if not(d(v,w)==0)
            dsum(w) = dsum(w) + d(v,w);
            cnt(w) = cnt(w) + 1;
        end
    end
end

DIST = dsum./cnt;

disp(DIST)


Pwr = log.rxPower;

Pwrsum = zeros(1,size(Pwr,2));
cnt = zeros(1,size(Pwr,2));
for w=1:size(Pwr,2)
    for v=1:size(Pwr,1)
        if not(Pwr(v,w)==0)
            Pwrsum(w) = Pwrsum(w) + Pwr(v,w);
            cnt(w) = cnt(w) + 1;
        end
    end
end

PW = Pwrsum./cnt;

disp(PW)


for r=1:length(anchors)
    EDM(MyID, anchors(r)) = DIST(anchors(r));
end

for r=1:length(anchors)
    PWR(MyID, anchors(r)) = PW(anchors(r));
end


disp(PWR)

% 0         0         0         0         0
% 0         0         0         0         0
% 0         0         0  -72.5444  -82.7067
% 0         0  -71.8756         0  -74.6309
% 0         0  -81.3597  -74.1895         0

disp(EDM)

% 0         0         0         0         0
% 0         0         0         0         0
% 0         0         0  159.3779  159.3432
% 0         0  159.3586         0  159.3641
% 0         0  159.3611  159.3833         0

fclose(s);
delete(instrfindall);                      


%% Clean Matrixes

% Remove zero rows
EDM( all(~EDM,2), : ) = [];
% Remove zero columns
EDM( :, all(~EDM,1) ) = [];

% EDM = 
%        0  159.3779  159.3432
% 159.3586         0  159.3641
% 159.3611  159.3833         0


% Remove zero rows
PWR( all(~PWR,2), : ) = [];
% Remove zero columns
PWR( :, all(~PWR,1) ) = [];

%         0  -72.5444  -82.7067
%  -71.8756         0  -74.6309
%  -81.3597  -74.1895         0



%% Perturbation algorithm
% Algorithm to perturbate the antenna delays as described in 
% APS014: DW1000 Antenna Delay Calibration

actual_dist = 5.0;

EDM_actual = [0 actual_dist actual_dist
              actual_dist 0 actual_dist
              actual_dist actual_dist 0];
        

SPEED_OF_LIGHT = 299702547;

tof_measured = EDM ./ SPEED_OF_LIGHT;
tof_actual = EDM_actual ./ SPEED_OF_LIGHT;



% base line dBm, which is -61, 2 dBm steps, total 18 data points (down to -95 dBm)
% Assumption is that no received signal strength is above -61 dBm!

% TOF in [s]
tof_measured = tof_measured + compensate_bias(PWR);



% Perturbation options
NO_candidates = 1000;
N = 3;

initial_delay = ones(NO_candidates,N)*-515*10^-9;
pertubation_limit = 0.2*10^-9;
initial_distribution = 6*10^-9;

candidates = [];

for n=1:100
    if(mod(n,20)==0)
        pertubation_limit = pertubation_limit/2;
    end
    if(n == 1)
        % Create initial delay with pertubation of +- 6 ns
        rng;
        candidates = initial_delay + (rand(NO_candidates,size(initial_delay,2)).*(initial_distribution*2) - initial_distribution);
    else
        % Add the best 25% of the candidates to a set and include 3x the
        % same best 25%, perturbed within the perturbation limit, to the set.
        best_candidates = candidates(candidate_set_sorted(1:round(NO_candidates*0.25),2),:);
        %disp(['Error: ' num2str(candidate_set_sorted(1,1))])
        candidates = best_candidates;
        for r=1:3
            rng;
            perturbation = rand(size(best_candidates,1),size(best_candidates,2)).*(pertubation_limit*2) - pertubation_limit;
            candidates = [candidates; best_candidates + perturbation];
        end
    end
    
    candidate_set = [];
    for r=1:NO_candidates
        tof_candidate = zeros(N,N);
        for i=1:N
            for j=1:N
                if(j~=i)
                    % Todo: Check up on this TWR equation
                    tof_candidate(i,j) = (3*candidates(r,i) + 3*candidates(r,j) + 6*tof_measured(i,j))/6;
                end
            end
        end
        error = norm(tof_actual - tof_candidate);
        candidate_set(r,:) = [error r];
        candidate_set_sorted = sortrows(candidate_set);
    end
end

DELAYS = abs(candidates(1,:)*10^9);

ANT_DLY = DELAYS*10^-9 / (15.65*10^-12);

disp(' ')
disp(' ')
disp('Antenna delays in ns:')
disp(num2str(DELAYS))
disp(' ')

disp('Antenna delays in DW units:')
disp(num2str(round(ANT_DLY)))
disp(' ')

disp('Resulting in an error of:')
disp(num2str(candidate_set_sorted(1,1)))

