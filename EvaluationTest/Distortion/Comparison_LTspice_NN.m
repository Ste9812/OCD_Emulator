close all
clearvars
clc

[ dist_LT , f_s ] = audioread( 'Output/dist_LT.wav' );
[ dist_NN , ~ ] = audioread( 'Output/dist_NN.wav' );

t = ( 0 : length( dist_LT ) - 1 )' / f_s;

figure
plot( t , dist_LT , '-r' , 'LineWidth' , 1.5 );
hold on;
plot( t , dist_NN , '--b' , 'LineWidth' , 1 );
xlim( [ 0 , 60 ] );
ylim( [ -0.55 , 0.55 ] );
grid on;
legend( 'LTspice' , 'NeuralNetwork' , 'Fontsize' , 14 , 'interpreter' , 'latex' );
xlabel( '$t$ [s]' , 'Fontsize' , 14 , 'interpreter' , 'latex' );
ylabel( '$y_{\mathrm{out}}(t)$ [V]' , 'Fontsize' , 14 , 'interpreter' , 'latex' );
title( 'Output Signal' , 'Fontsize' , 18 , 'interpreter' , 'latex' );

figure
plot( t , abs( dist_LT - dist_NN ) , 'b' );
xlim( [ 0 , 60 ] );
ylim( [ 0 , 1 ] );
grid on;
xlabel( '$t$ [s]' , 'Fontsize' , 14 , 'interpreter' , 'latex' );
ylabel( '$|e_{out}(t)|$ [V]' , 'Fontsize' , 14 , 'interpreter' , 'latex' );
title( 'Error Absolute Value' , 'Fontsize' , 18 , 'interpreter' , 'latex' );

err = abs( dist_LT - dist_NN );

maxErr = max( err );
meanErr = mean( err );
MSE = mean( err .^ 2 );

disp( maxErr );
disp( meanErr );
disp( MSE );