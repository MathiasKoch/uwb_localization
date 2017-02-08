clear all
close all


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

figure('Name','TEST','units','normalized','outerposition',[0 0 1 1],'KeyPressFcn','escape = 0;')
subplot(3,1,1)
presp = plot(0);
ylabel('Pa')
subplot(3,1,2)
aslp = plot(0);
ylabel('mm')
subplot(3,1,3)
tempp = plot(0);
ylabel('C')
xlabel('Time (s)')

pres = 0;
asl = 0;
temp = 0;

i = 1;
time = 0;
Fs = 0.01;
escape = 1;
while(escape == 1)
    sSerialData = fscanf(s);            
    %flushinput(s);
    t = strsplit(sSerialData,'\t');
    if(size(t,2) == 3)
        time = [time i*Fs];
        pres = [pres str2double(t(1))];
        asl = [asl str2double(t(2))];
        temp = [temp str2double(t(3))];
        set(presp,'XData',time,'YData',pres);
        set(aslp,'XData',time,'YData',asl);
        set(tempp,'XData',time,'YData',temp);
        ax = get(presp,'Parent');
        set(ax,'XLim', [(i-400)*Fs (i+100)*Fs]);
        ax = get(aslp,'Parent');
        set(ax,'XLim', [(i-400)*Fs (i+100)*Fs]);
        ax = get(tempp,'Parent');
        set(ax,'XLim', [(i-400)*Fs (i+100)*Fs]);
        drawnow;
        i = i + 1;
    end
end
fclose(s);
delete(instrfindall);