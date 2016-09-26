
close all

load('ground_vs_no_ground.mat')

figure()
log.raw(:,1) = log.raw(:,1) + 110;
plot(log.raw(2500:end,2:-1:1))

title('Ground plane vs no ground plane')
xlabel('Sample')
ylabel('Measured distance (mm)')
legend('Without ground plane','With ground plane')
set(gcf,'paperunits','centimeters','PaperSize',[30 7],'PaperPosition',[0 0 30 7])
saveas(gcf,'../ground.eps','epsc');


%%


load('log6.mat')


ASL = @(P) (1-(P/1013.25).^0.190263)*44330.8;

for n = 1:size(log.pres,1)
    for i = 1:size(log.pres,2)
        if(log.pres(n,i) < 10)
            log.pres(n,i) = log.pres(n-1,i);
        end
    end
end

figure()
as = ASL(log.pres(:,:));
as_diff(:,1) = as(:,1)-as(:,2);
as_diff(:,2) = as(:,1)-as(:,4);
as_diff(:,3) = as(:,2)-as(:,3);

plot(as_diff(2000:end,:).*1000)

title('Difference of pressure')
xlabel('Sample')
ylabel('Altitude difference (mm)')
legend('A_1-A_2','A_1-A_3','A_1-A_4')

set(gcf,'paperunits','centimeters','PaperSize',[30 7],'PaperPosition',[0 0 30 7])
saveas(gcf,'../pressure.eps','epsc');



figure()
plot3(log.xyz_nl(1,:),log.xyz_nl(2,:),log.xyz_nl(3,:))
grid on
hold on
daspect([1 1 1])
view([0 90])


title('Robot driving square with rounded corners')
xlabel('X (mm)')
ylabel('Y (mm)')

set(gcf,'paperunits','centimeters','PaperSize',[25 18],'PaperPosition',[0 0 25 18])
saveas(gcf,'../square1.eps','epsc');

%plot3(log.ref(1,2:end-2),log.ref(2,2:end-2),log.ref(3,2:end-2),'o-','MarkerSize',10,'linewidth',3,'Color','g')

%%


figure()

subplot(2,2,1)
plot(log.raw(:,1),'b')
hold on
plot(log.mean(:,1),'r')
title('Anchor 1')
xlabel('Sample')
ylabel('Measured distance (mm)')

subplot(2,2,2)
plot(log.raw(:,2),'b')
hold on
plot(log.mean(:,2),'r')
title('Anchor 2')
xlabel('Sample')
ylabel('Measured distance (mm)')
legend('Raw measurements','FIR filtered','Location','SouthEast')

subplot(2,2,3)
plot(log.raw(:,3),'b')
hold on
plot(log.mean(:,3),'r')
title('Anchor 3')
xlabel('Sample')
ylabel('Measured distance (mm)')

subplot(2,2,4)
plot(log.raw(:,4),'b')
hold on
plot(log.mean(:,4),'r')
title('Anchor 4')
xlabel('Sample')
ylabel('Measured distance (mm)')
set(gcf,'paperunits','centimeters','PaperSize',[30 18],'PaperPosition',[0 0 30 18])
saveas(gcf,'../img/SMR_dist_good.pdf');



figure()
plot3(log.xyz_nl(1,:),log.xyz_nl(2,:),log.xyz_nl(3,:))
grid on
hold on
daspect([1 1 1])

plot3(log.ref(1,:),log.ref(2,:),log.ref(3,:),'o-','MarkerSize',10,'linewidth',3,'Color','g')

for n = 1:size(log.anchors,2)
    plot3(log.anchors(1,n),log.anchors(2,n),log.anchors(3,n),'x','MarkerSize',10,'linewidth',3,'Color','r')
end

legend('Estimated position','Measured route','Anchor positions', 'Location','NorthEast');
xlim([-500 3500])
view([-90 90])
xlabel('X position (mm)')
ylabel('Y position (mm)')
title('Estimated X & Y position vs measured route')

set(gcf,'paperunits','centimeters','PaperSize',[25 25],'PaperPosition',[0 0 25 25])
saveas(gcf,'../img/SMR_xy_good.pdf');