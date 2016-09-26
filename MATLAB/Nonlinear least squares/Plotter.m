clear all 
close all

ASL = @(P) (1 - (P/1013.25)^0.190263)*44330.8;         
                  
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

avg = 1;
escape = 1;
while(escape == 1)
    sSerialData = fscanf(s);            
    flushinput(s);
    t = strsplit(sSerialData,'\t');
    anc = strsplit(t{1},':');
    if anc{1} == 'N'
        N = str2double(anc(2));
        if size(t,2)-1 == (N*2+1)
            if i == 1
                Fs = 0.01*N;
                
                % Raw measurements plot
                figure('Name','RAW','units','normalized','outerposition',[0 0 1 1],'KeyPressFcn','escape = 0;')
                hold on
                c = hsv(N);
                for n = 1:N
                    h(n) = plot(0,'Color',c(n,:));
                    %mn(n) = plot(0,'r');
                end
                grid on
                ylabel('Distance (mm)')
                xlabel('Time (s)')
                
                figure('Name','PRES','units','normalized','outerposition',[0 0 1 1],'KeyPressFcn','escape = 0;')
                hold on
                c = hsv(N);
                for n = 1:N
                    p(n) = plot(0,'Color',c(n,:));
                    %mn(n) = plot(0,'r');
                end
                grid on
                ylabel('Pressure (hPa)')
                xlabel('Time (s)')
                
                
                figure('Name','DIFF','units','normalized','outerposition',[0 0 1 1],'KeyPressFcn','escape = 0;')
                d = plot(0,'Color','r');
                grid on
                ylabel('Altitude Diff (m)')
                xlabel('Time (s)')

                

            end
            
            tstart = tic;
            log.time = [log.time i*Fs];

            if(i>avg)
                for n = 1:N
                    log.raw(i,n) = str2double(t(n+1));
                    log.pres_raw(i,n) = str2double(t(N+n+1));
                    if(log.pres_raw(i,n) < 10)
                        log.pres_raw(i,n) = log.pres_raw(i-1,n);
                    end
                    log.H(i,n) = ASL(log.pres_raw(i,n));%str2double(t(N*2+n+2));
                    log.aver(i,n) = sum(log.raw(i-avg:i,n))/length(log.raw(i-avg:i,n));
                    log.H_aver(i,n) = sum(log.H(i-avg:i,n))/length(log.H(i-avg:i,n));
                    set(h(n),'XData',log.time(1:i),'YData',log.raw(:,n));
                    set(p(n),'XData',log.time(1:i),'YData',log.H_aver(:,n));
                    %set(p(n),'XData',log.time(1:i),'YData',log.pres_raw(:,n));
                    %set(mn(n),'XData',log.time(1:i),'YData',log.aver(:,n));
                end
                ax = get(h(1),'Parent');
                set(ax,'XLim', [(i-400)*Fs (i+100)*Fs]);
                %set(ax,'YLim', [1900 2400]);
                
                ax = get(p(1),'Parent');
                set(ax,'XLim', [(i-400)*Fs (i+100)*Fs]);
                set(ax,'YLim', [-40 -35]);
                
                set(d,'XData',log.time(1:i),'YData',(log.H_aver(:,1)+log.H_aver(:,2)+log.H_aver(:,3))/3-log.H_aver(:,4));
                ax = get(d,'Parent');
                set(ax,'XLim', [(i-400)*Fs (i+100)*Fs]);
            end
            drawnow
            i = i + 1;
            log.elapsed = [log.elapsed toc(tstart)];
        end
    end
end
%save('log.mat','log');
fclose(s);
delete(instrfindall);                      


