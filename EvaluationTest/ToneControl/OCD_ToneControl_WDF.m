close all
clearvars
clc

% OCD TONE CONTROL CIRCUIT - WDF IMPLEMENTATION %

%% Import input signal

[ x_in , f_s ] = audioread( 'Input/ExpSweep.wav' );

%% Import ground-truth signals

[ y_GT , ~ ] = audioread( 'Output/Output_LTspice.wav' );

%% Element parameters

x_aux = 0.5;
A = 0.0125;
B = 81;
C = - 0.0125;

R_s = 33 * 10 ^ 3;
R_t = 10 * 10 ^ 3;
R_v = 500 * 10 ^ 3;
R_tol = 1 * 10 ^ ( - 6 );

Rg = 1 * 10 ^ ( - 9 );
R9 = 150 * 10 ^ 3;
R10 = 39 * 10 ^ 3;
Rsw = R_s;
Rtc = R_t * x_aux + R_tol;
Rva = R_v * ( A * B ^ x_aux + C ) + R_tol;
Rvb = R_v * ( 1 - A * B ^ x_aux - C ) + R_tol;
C5 = 220 * 10 ^ ( - 12 );
C6 = 100 * 10 ^ ( - 9 );
C7 = 1 * 10 ^ ( - 6 );
C8 = 47 * 10 ^ ( - 9 );

%% Adaptation conditions 

Z_g = Rg;
Z_R9 = R9;
Z_R10 = R10;
Z_Rsw = Rsw;
Z_Rtc = Rtc;
Z_Rva = Rva;
Z_Rvb = Rvb;
Z_C5 = 1 / ( 2 * C5 * f_s );
Z_C6 = 1 / ( 2 * C6 * f_s );
Z_C7 = 1 / ( 2 * C7 * f_s );
Z_C8 = 1 / ( 2 * C8 * f_s );

%% Reference port resistances matrix

Z = diag( [ Z_C5 ; Z_C7 ; Z_C8 ; Z_g ; Z_C6 ; Z_Rtc ; Z_Rva ; Z_R9 ; Z_R10 ; Z_Rsw ; Z_Rvb ] );

%% Fundamental-loop matrices

B_V = [ - 1 , 0 ,   0 ,   0 , 0 ,   0 , 0 , 1 , 0 , 0 , 0 ;
          0 , 0 ,   0 , - 1 , 1 ,   0 , 0 , 0 , 1 , 0 , 0 ;
          1 , 1 , - 1 ,   1 , 0 , - 1 , 0 , 0 , 0 , 1 , 0 ;
          0 , 0 , - 1 ,   0 , 0 , - 1 , 1 , 0 , 0 , 0 , 1 ];

B_I = [ - 1 , 0 ,   0 , 0 , 0 ,   0 , 0 , 1 , 0 , 0 , 0 ;
          1 , 0 ,   0 , 0 , 1 ,   0 , 0 , 0 , 1 , 0 , 0 ;
          0 , 1 , - 1 , 0 , 0 , - 1 , 0 , 0 , 0 , 1 , 0 ;
          0 , 0 , - 1 , 0 , 0 , - 1 , 1 , 0 , 0 , 0 , 1 ];

%% Number of elements/samples

n = size( Z , 1 );
N = length( x_in );

%% Scattering matrix

S = eye( n ) - 2 * Z * B_I' * ( ( B_V * Z * B_I' ) \ B_V );

%% Vectors initialization

a = zeros( n , 1 );
b = zeros( n , 1 );

y_out = zeros( N , 1 );

%% WDF simulation

tic;

k = 0;

while ( k < N )

    k = k + 1;

    b( 1 ) = a( 1 );
    b( 2 ) = a( 2 );
    b( 3 ) = a( 3 );
    b( 4 ) = x_in( k );
    b( 5 ) = a( 5 );
    b( 6 ) = 0;
    b( 7 ) = 0;
    b( 8 ) = 0;
    b( 9 ) = 0;
    b( 10 ) = 0;
    b( 11 ) = 0;

    a = S * b;

    y_out( k ) = ( a( 7 ) + b( 7 ) ) / 2;

end

toc;

%% Write output files

audiowrite( 'Output_WDF.wav' , y_out , f_s );

%% Outputs/errors plot

plotGroundTruthWDF( y_out , y_GT , f_s );
plotAbsErrorSignal( y_out , y_GT , f_s );

%% Absolute error maximum

errMax = max( abs( y_out( 1 : min( length( y_GT ) , length( y_out ) ) ) - y_GT( 1 : min( length( y_GT ) , length( y_out ) ) ) ) );
disp( errMax );

%% Plot auxiliary functions

function plotGroundTruthWDF( y_WDF , y_GT , f_s )
    N_GT = length( y_GT );
    N_WDF = length( y_WDF );
    N = min( N_GT , N_WDF );
    f = figure;
    f.WindowState = 'maximized';
    set( gcf , 'Position' , [ get( 0 , 'ScreenSize' ) ] );
    plot( ( 1 : N ) / f_s , y_GT( 1 : N ) , 'r' , 'Linewidth' , 2 );
    hold on;
    plot( ( 1 : N ) / f_s , y_WDF( 1 : N ) , 'b--' , 'Linewidth' , 1 );
    grid on;
    xlim( [ 0 , N / f_s ] ); 
    delta = 0.1 * max( abs( y_GT ) , [ ] , 'all' );
    ylim( [ min( y_GT , [ ] , 'all' ) - delta , max( y_GT , [ ] , 'all' ) + delta ] );
    xlabel( '$t$ [s]' , 'Fontsize' , 14 , 'interpreter' , 'latex' );
    ylabel( '$y_{\mathrm{out}}(t)$ [V]' , 'Fontsize' , 14 , 'interpreter' , 'latex' );
    title( 'Output Signal' , 'Fontsize' , 18 , 'interpreter' , 'latex' );
    legend( 'Ground Truth' , 'WDF' , 'Fontsize' , 14 , 'interpreter' , 'latex' );
end

function plotAbsErrorSignal( y_WDF , y_GT , f_s )
    N_GT = length( y_GT );
    N_WDF = length( y_WDF );
    N = min( N_GT , N_WDF );
    absErr = abs( y_GT( 1 : N ) - y_WDF( 1 : N ) );
    [ absErrEnv , ~ ] = envelope( absErr , 20000 , 'peak' );
    f = figure;
    f.WindowState = 'maximized';
    set( gcf , 'Position' , [ get( 0 , 'ScreenSize' ) ] );
    plot( ( 1 : N ) / f_s , absErrEnv , 'b' , 'Linewidth' , 2 );
    grid on;
    xlim( [ 0 , N / f_s ] ); 
    ylim( [ 0 , 5 * max( absErr , [ ] , 'all' ) ] );
    xlabel( '$t$ [s]' , 'Fontsize' , 14 , 'interpreter' , 'latex' );
    ylabel( '$|e_{out}(t)|$ [V]' , 'Fontsize' , 14 , 'interpreter' , 'latex' );
    title( 'Error Signal Absolute Value' , 'Fontsize' , 18 , 'interpreter' , 'latex' );
end