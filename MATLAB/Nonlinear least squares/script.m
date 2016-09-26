clear all 
close all

global log data W Z

%log.anchors = [6930  1970  2770
%               0     2100  2580   
%               3510  640   2050
%               4020  2180  2900]'; 
         
           
log.anchors = [ 0           0        2900
        1510        -980        2040
        1590        2890        2900
           0        2890        2900]';

                  
log.ref =     [0     0   0      % Point 1
               200   0   0      % Point 2
               900   -350 0     % Point 3
               2850  -350 0
               2850  2750 0
               2400  2750 0
               1800  3150 0
               1150  3150 0
               550   2750 0 ]';

%log.ref = [];
          
log.AVG = 15;
         
             
delete(instrfindall);
s = serial('/dev/ttyUSB0');
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
log.mean = [];
log.xyz_nl = [];
log.xyz_l = [];
log.elapsed = [];
i = 1;
escape = 1;
W = 20000;

Z = [0 ;0; 200];

options = optimoptions(@lsqnonlin,'Algorithm','levenberg-marquardt','MaxIter', 4, 'TolFun',1e-2,'Display','none');
x0MP = [4631.5159;
2524.1049;
491.92748];
while(escape == 1)
    sSerialData = fscanf(s);            
    flushinput(s);
    t = strsplit(sSerialData,'\t');
    anc = strsplit(t{1},':');
    if anc{1} == 'N'
        N = 4;%str2double(anc(2));
        %if size(t,2)-1 == N
            if i == 1
                Fs = 0.01*N;
                
                % Raw measurements plot
                figure('Name','RAW','units','normalized','outerposition',[0 0.5 0.5 0.5],'KeyPressFcn','escape = 0;')
                hold on
                for n = 1:N
                    h(n) = plot(0);
                    mn(n) = plot(0,'r');
                end
                ylabel('Distance (mm)')
                xlabel('Time (s)')

                % XYZ subplots
                figure('Name','XYZ','units','normalized','outerposition',[0 0 0.5 0.5],'KeyPressFcn','escape = 0;')
                subplot(3,1,1)
                xp = plot(0);
                ylabel('X')
                subplot(3,1,2)
                yp = plot(0);
                ylabel('Y')
                subplot(3,1,3)
                zp = plot(0);
                ylabel('Z')
                xlabel('Time (s)')
                

                % XY plot
                figure('Name','XY','units','normalized','outerposition',[0.5 0 0.5 1],'KeyPressFcn','escape = 0;')
                hold on
                %xlim([-1500 4000])
                %ylim([-1500 4000])
                ylabel('Y (mm)')
                xlabel('X (mm)')
                
                %for n = 1:max([size(log.ref,1) N])
                %    if n<=N
                %        plot(log.anchors(1,n),log.anchors(2,n),'rx','LineWidth',2,'MarkerSize',10);
                %        
                %    end
                %    if n<=size(log.ref,1)
                %        plot(log.ref(1,n),log.ref(2,n),'gx','LineWidth',2,'MarkerSize',10);
                %    end
                %end
                xy = plot3(0, 0,0);
                daspect([1 1 1]);
                %legend('Anchors','References')
                
                %nl_struct = initiate_nonlinear(log.anchors);
                %l_struct = initiate_linear(log.anchors);
            end
            
            tstart = tic;
            log.time = [log.time i*Fs];

            for n = 1:N
                log.raw(i,n) = str2double(t(n+1));
                log.pres(i,n) = str2double(t(N+n+1));
                if(log.raw(i,n) == 0)
                    if(i > 2)
                        log.raw(i,n) = log.raw(i-1,n);
                    end
                end
                set(h(n),'XData',log.time(1:i),'YData',log.raw(:,n));
            end
            ax = get(h(1),'Parent');
            set(ax,'XLim', [(i-400)*Fs (i+100)*Fs]);


            if(i > log.AVG+1)                
                data.meas = sum(log.raw(i-(log.AVG-1):i,:))/log.AVG;
                log.mean = [log.mean; data.meas];
                for n = 1:N
                    set(mn(n),'XData',log.time(1:i),'YData',log.mean(:,n)');
                end
                
                %P0 = calculate_nonlinear(nl_struct, log.anchors, mean_val);
                %P0_l = calculate_linear(l_struct, log.anchors, mean_val);
                P0 = lsqnonlin(@myfunHFIX,x0MP,[],[],options);
                %P0 = lsqnonlin(@myfun,x0MP,[],[],options);
                x0MP = P0;
                log.xyz_nl = [log.xyz_nl P0];
                %log.xyz_l = [log.xyz_l P0_l];
                
                set(xy,'XData',log.xyz_nl(1,:),'YData',log.xyz_nl(2,:),'ZData',log.xyz_nl(3,:));

                set(xp,'XData',log.time(log.AVG+2:i),'YData',log.xyz_nl(1,:));
                set(yp,'XData',log.time(log.AVG+2:i),'YData',log.xyz_nl(2,:));
                set(zp,'XData',log.time(log.AVG+2:i),'YData',log.xyz_nl(3,:));

                drawnow
            else
                log.mean = [log.mean; log.raw(i,:)];
            end

            i = i + 1;
            log.elapsed = [log.elapsed toc(tstart)];
        %end
    end
end
save('log7.mat','log');
fclose(s);
delete(instrfindall);                      


