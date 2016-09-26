function [adjustmentTime, bias_mm] = compensate_bias(rxPower, plot)

SPEED_OF_LIGHT = 299702547;

rxPowerBase = -(rxPower + 61.0) .* 0.5;
rxPowerBaseLow = floor(rxPowerBase);
rxPowerBaseHigh = rxPowerBaseLow+1;

rxPowerBase(rxPowerBase<0) = 0;
rxPowerBaseHigh(rxPowerBaseLow<0) = 0;
rxPowerBaseLow(rxPowerBaseLow<0) = 0;


rxPowerBaseLow(rxPowerBaseHigh>17) = 17;
rxPowerBaseHigh(rxPowerBaseHigh>17) = 17;

% Compensate for matlab being one-indexed
rxPowerBaseLow = rxPowerBaseLow + 1;
rxPowerBaseHigh = rxPowerBaseHigh + 1;


% Bias tables taken from:
% APS011: Sources of error in DW1000 based two-way ranging (TWR) schemes
% range bias tables (500 MHz in [mm] and 900 MHz in [2mm] - to fit into bytes)
BIAS_500_16 = [-198, -187, -179, -163, -143, -127, -109, -84, -59, -31, 0,  36,  65,  84,  97,  106, 110, 112];
BIAS_500_64 = [-110, -105, -100, -93, -82, -69, -51,  -27, 0,  21, 35, 42,  49,  62,  71,  76,  81,  86];
BIAS_900_16 = [-137, -122, -105, -88, -69, -47, -25,  0,  21, 48, 79, 105, 127, 147, 160, 169, 178, 197];
BIAS_900_64 = [-147, -133, -117, -99, -75, -50, -29,  0,  24, 45, 63, 76,  87,  98,  116, 122, 132, 142];

BIAS_500_64 = BIAS_500_64;


% Add the correct range bias, calculated by rxPower as described in
% APS011: Sources of error in DW1000 based two-way ranging (TWR) schemes

dBm = -61:-2:-95;

if(plot)
    figure(1)
    hold off
    plot(dBm,BIAS_500_64);
    hold on
    grid on
    ylim([-200 150])
end
    for i=1:size(rxPower,1)
        for j=1:size(rxPower,2)
            if(j~=i || numel(rxPower) == 1)
                rangeBiasLow(i,j) = BIAS_500_64(rxPowerBaseLow(i,j));
                rangeBiasHigh(i,j) = BIAS_500_64(rxPowerBaseHigh(i,j));

                % Linear interpolation of bias values
                rangeBias(i,j) = rangeBiasHigh(i,j) + ((rxPowerBase(i,j) - rxPowerBaseLow(i,j)) * (rangeBiasHigh(i,j) - rangeBiasLow(i,j)));

                % range bias [mm] to [s]
                adjustmentTime(i,j) = (rangeBias(i,j) * 0.001) / SPEED_OF_LIGHT;
if(plot)
                plot(rxPower(i,j),rangeBias(i,j),'x')
end
            end
        end
    end

bias_mm = rangeBias;

end