close all
clearvars
clc

t_spice = 427;

[ clean , fs ] = audioread( "clean.wav" );

N_samples = length( clean );

distData = zeros( 11 , fs * t_spice );
for ii = 0 : 10
    [ distData( ii + 1 , : ) , ~ ] = audioread( "distorted_" + num2str( 10 * ii ) + ".wav" );
end

peakClean = max( abs( clean ) , [ ] , "all" );
peakDistTot = max( abs( distData ) , [ ] , "all" );

distDataNorm = distData( : , 1 : N_samples ) * peakClean / peakDistTot;

audiowrite( "Distorted_Normalized_Amplitude\clean.wav" , clean , fs , "BitsPerSample" , 32 );
for ii = 0 : 10
    audiowrite( "Distorted_Normalized_Amplitude\distorted_" + num2str( 10 * ii ) + ".wav" , distDataNorm( ii + 1 , : ) , fs , "BitsPerSample" , 32 );
end