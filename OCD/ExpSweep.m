close all
clearvars
clc

f_0 = 10;
f_end = 24000;
f_s = 48000;
t_sil = 0.01;
t_tot = 5;

sweep = sweeptone( t_tot , t_sil , f_s , "SweepFrequencyRange" , [ f_0 , f_end ] );

audiowrite( 'inputSig.wav' , sweep , f_s );

