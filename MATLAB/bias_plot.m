

BIAS_500_64 = [-110, -105, -100, -93,  -82,  -69,  -51,  -27, 0,  21, 35, 42,  49,  62,  71,  76,  81,  86];
BIAS_500_64 = fliplr(BIAS_500_64);

BIAS_X = linspace(-95,-61, 18);



true_dist = [1821 3169 5383 5767 5793 6645 7818 13957 7873 9412 11487 10873 10203 8853 7653 6637 6245 5527 2649]';% 17248]';

measured_dist = [1.724738 3.157267 5.464477 5.828777 5.839377 6.671165 7.849828 14.139268 7.897975 9.419902 11.564169 11.048018 10.363039 9.054359 7.754895 6.697454 6.378698 5.679459 2.614874]';% 16.756816]';

measured_dist = measured_dist*1000;

rx_level = [-69.927834 -72.015144 -74.100487 -75.937866 -78.162666 -80.043449 -82.303917 -84.120300 -86.105576 -88.278275 -89.928108 -78.057831 -80.193130 -75.967018 -78.407791 -79.157234 -74.233826 -72.301033 -71.212440]';% -90.117073]';

rx_level_tag = [-70.202789 -72.491982 -74.917175 -76.561325 -78.841438 -80.773254 -83.281288 -84.924530 -87.099922 -88.824951 -90.445107 -78.699631 -80.989929 -76.664925 -79.255623 -79.925491 -74.777748 -72.882210 -71.624817]';% -90.742470

rx_level_anchor = [-69.692764 -71.534401 -73.257568 -75.305412 -77.485413 -79.309113 -81.370438 -83.297363 -85.065948 -87.746719 -89.440613 -77.458687 -79.327499 -75.249641 -77.541306 -78.410118 -73.665237 -71.698051 -70.747169]';% -89.513405

fpp_level = [-0.000000 -0.000000 -0.000000 -0.000000 -0.000000 -0.000000 -0.000000 -89.3684 -90.5675 -94.4026 -0.000000 -83.0048 -82.9788 -78.8191 -87.4546 -87.8011 -78.9715 -78.8392 -73.8489]';% -94.2905]

% Assume perfect line of sight for ranges without first power path
% measurement
%fpp_level(fpp_level==0) = rx_level(fpp_level==0);

[~, comp] = compensate_bias(rx_level_anchor, false)

measured_dist = measured_dist-110

error = true_dist-measured_dist;

error_after = true_dist-measured_dist-comp;


%qual = rx_level-fpp_level;

%LOS_Thresh = 9;

%isLOS = qual<LOS_Thresh

figure(1)
subplot(2,1,1)
hold on
plot(BIAS_X,BIAS_500_64,'g', 'LineWidth',2)
%plot(rx_level,error,'xb')
plot(rx_level_anchor,error,'xb', 'LineWidth',2)
%plot(rx_level_tag,error,'xr')
xlabel('Received power level [dBm]')
ylabel('Error in distance [mm]')








coeffs = polyfit(rx_level_anchor, error+50, 4);
% Get fitted values
fittedX = linspace(-95, -61, 18);
fittedY = polyval(coeffs, fittedX);
% Plot the fitted line
hold on;
%plot(fittedX, fittedY, 'm--');
grid on
legend('Decawave bias curve', 'Line of sight', 'fitted bias curve')

ylim([-200 200])
xlim([-95 -61])

subplot(2,1,2)
hold on
%plot(rx_level,error,'xb')
stem(rx_level_anchor,error,'xr', 'LineWidth',2)
stem(rx_level_anchor,error_after,'xb--', 'LineWidth',2)
%plot(rx_level_tag,error,'xr')
xlabel('Received power level [dBm]')
ylabel('Error in distance [mm]')
grid on

ylim([-200 200])
xlim([-95 -61])

legend('Error before compensation', 'Error after compensation')

%%
figure(2)
plot(measured_dist,error,'x')
xlabel('Measured distance [mm]')
ylabel('Error in distance [mm]')

coeffs = polyfit(measured_dist, error, 6);
% Get fitted values
fittedX = linspace(min(measured_dist), max(measured_dist), 200);
fittedY = polyval(coeffs, fittedX);
% Plot the fitted line
hold on;
plot(fittedX, fittedY, 'r--');