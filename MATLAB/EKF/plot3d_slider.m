
function plot3d_slider(STATE, TRUEPOS)

global anchor_pos

f = figure;
ax = axes('Parent',f,'position',[0.13 0.39  0.77 0.54]);
h1 = plot3(ax,anchor_pos(:,1),anchor_pos(:,2),anchor_pos(:,3), 'x', 'Linewidth', 2);
hold on
h = plot3(ax,STATE.data(1,7:3:end),STATE.data(1,8:3:end),STATE.data(1,9:3:end), 'o', 'Linewidth', 2);
h3 = plot3(ax,STATE.data(1,1),STATE.data(1,2),STATE.data(1,3), 'og', 'Linewidth', 2);

if(nargin > 1)
    h4 = plot3(ax,TRUEPOS.data(1,1),TRUEPOS.data(1,2),TRUEPOS.data(1,3), 'xg', 'Linewidth', 2);
end

axis equal
grid on

b = uicontrol('Parent',f,'Style','slider','Position',[81,54,419,23],...
              'value',1, 'min',1, 'max', length(STATE.time));
bgcolor = f.Color;

bl1 = uicontrol('Parent',f,'Style','text','Position',[50,54,23,23],...
                'String','1','BackgroundColor',bgcolor);
            
bl2 = uicontrol('Parent',f,'Style','text','Position',[500,54,50,23],...
                'String',num2str(length(STATE.time)),'BackgroundColor',bgcolor);
            
bl3 = uicontrol('Parent',f,'Style','text','Position',[240,25,100,23],...
                'String','Time','BackgroundColor',bgcolor);
            

if(nargin > 1)         
b.Callback = @(es,ed)(cellfun(@(x)feval(x,es,ed), ...
  {@(es,ed) set(h,'XData',STATE.data(round(es.Value),7:3:end),'YData',STATE.data(round(es.Value),8:3:end), 'ZData', STATE.data(round(es.Value),9:3:end)), ...
  @(es,ed) set(h4,'XData',TRUEPOS.data(round(es.Value),1),'YData',TRUEPOS.data(round(es.Value),2), 'ZData', TRUEPOS.data(round(es.Value),3)), ...
  @(es,ed) set(h3,'XData',STATE.data(round(es.Value),1),'YData',STATE.data(round(es.Value),2), 'ZData', STATE.data(round(es.Value),3))}));
else
    b.Callback = @(es,ed)(cellfun(@(x)feval(x,es,ed), ...
    {@(es,ed) set(h,'XData',STATE.data(round(es.Value),7:3:end),'YData',STATE.data(round(es.Value),8:3:end), 'ZData', STATE.data(round(es.Value),9:3:end)), ...
    @(es,ed) set(h3,'XData',STATE.data(round(es.Value),1),'YData',STATE.data(round(es.Value),2), 'ZData', STATE.data(round(es.Value),3))}));
end

end