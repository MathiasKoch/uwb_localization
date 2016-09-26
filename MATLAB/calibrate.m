clear all 
close all

global log

          
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
log.elapsed = [];
i = 1;
escape = 1;
N = 1

while(escape == 1)
    sSerialData = fscanf(s);            
    flushinput(s);
    t = strsplit(sSerialData,', ');
  
    if i == 1
        Fs = 0.01;

        % Raw measurements plot
        figure('Name','RAW','units','normalized','outerposition',[0 0.5 0.5 0.5],'KeyPressFcn','escape = 0;')
        hold on
        for n = 1:N
            h(n) = plot(0);
            grid on
            mn(n) = plot(0,'r');
        end
        ylabel('Distance (mm)')
        xlabel('Time (s)')
        
        figure('Name','RX Power','units','normalized','outerposition',[0 0.5 0.5 0.5],'KeyPressFcn','escape = 0;')
        hold on
        for n = 1:N
            pw(n) = plot(0);
            grid on
        end
        ylabel('Distance (mm)')
        xlabel('Time (s)')
    end

    tstart = tic;
    log.time = [log.time i*Fs];

    for n = 1:N
        log.raw(i,n) = str2double(t(n));
        %log.pres(i,n) = str2double(t(N+n+1));
        if(log.raw(i,n) <= 0.0 || log.raw(i,n) > 3.9)
            if(i > 2)
                log.raw(i,n) = log.raw(i-1,n);
            end
        end
        set(h(n),'XData',log.time(1:i),'YData',log.raw(:,n));
        
        log.rxPower(i,n) = str2double(t(N+n));
        if(log.rxPower(i,n) <= -100.0 || log.rxPower(i,n) > -40.0)
            if(i > 2)
                log.rxPower(i,n) = log.rxPower(i-1,n);
            end
        end
       
        set(pw(n),'XData',log.time(1:i),'YData',log.rxPower(:,n));
    end
    ax = get(h(1),'Parent');
    set(ax,'XLim', [(i-400)*Fs (i+100)*Fs]);
    %set(ax,'YLim', [0.5 1.5]);
    ax = get(pw(1),'Parent');
    set(ax,'XLim', [(i-400)*Fs (i+100)*Fs]);


    if(i > log.AVG+1)                
        meas = sum(log.raw(i-(log.AVG-1):i,:))/log.AVG;
        log.mean = [log.mean; meas];
        for n = 1:N
            set(mn(n),'XData',log.time(1:i),'YData',log.mean(:,n)');
        end

        drawnow
    else
        log.mean = [log.mean; log.raw(i,:)];
    end



    if(mod(i,2000)==0)
        disp(['Range: ' num2str(log.mean(end))])
    end

    i = i + 1;
    log.elapsed = [log.elapsed toc(tstart)];
end
fclose(s);
delete(instrfindall);                      


