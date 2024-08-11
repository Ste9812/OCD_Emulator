classdef OCD_ToneControl_PluginWDF < audioPlugin

    properties
        f_s = 48000;
        Switch = 'LP';
        Tone = 0;
        Volume = 0.5;
        Bypass = 'OFF';
    end

    properties ( Constant )
        PluginInterface = audioPluginInterface( ...
            audioPluginParameter( 'Switch' , ...
                'Style' , 'vtoggle' , ...
                'DisplayNameLocation' , 'none' , ...
                'Layout' , [ 2 , 1 ] , ... 
                'Mapping' , { 'enum' , 'LP' , 'HP' }) , ...
            audioPluginParameter( 'Tone' , ...
                'DisplayName' , 'Tone' , ... 
                'DisplayNameLocation' , 'above' , ...
                'Style' , 'rotaryknob' , ...
                'Layout' , [ 2 , 2 ] , ... 
                'Mapping' , { 'lin' , 0 , 1 }) , ...
            audioPluginParameter( 'Volume' , ...
                'DisplayName' , 'Volume' , ...
                'DisplayNameLocation' , 'above' , ...
                'Style' , 'rotaryknob' , ...
                'Layout' , [ 4 , 1 ] , ... 
                'Mapping' , { 'lin' , 0 , 1 }) , ...
            audioPluginParameter( 'Bypass' , ...
                'Style' , 'vrocker' , ...
                'DisplayNameLocation' , 'none' , ...
                'Layout' , [ 4 , 2 ] , ... 
                'Mapping' , { 'enum' , 'OFF' , 'ON' }) , ...
            audioPluginGridLayout( 'RowHeight' , [ 20 , 125 , 20 , 125 ] , ...
                'ColumnWidth' , [ 125 , 125 ] ) , ...
            'PluginName' , 'OCD_Emu' , ...
            'VendorName' , 'RS' , ...
            'VendorVersion' , '0.0.1' , ...
            'InputChannels' , 1 , ...
            'OutputChannels' , 1 );
    end

    properties ( Access = private )
        A = 0.0125;
        B = 81;
        C = - 0.0125;
        
        Rs = [ 33 , 13.2 ] * 10 ^ 3;
        Rt = 10 * 10 ^ 3;
        Rv = 500 * 10 ^ 3;
        Rtol = 1 * 10 ^ ( - 6 );
        
        Rg = 1 * 10 ^ ( - 9 );
        R9 = 150 * 10 ^ 3;
        R10 = 39 * 10 ^ 3;
        C5 = 220 * 10 ^ ( - 12 );
        C6 = 100 * 10 ^ ( - 9 );
        C7 = 1 * 10 ^ ( - 6 );
        C8 = 47 * 10 ^ ( - 9 );

        Rsw = 0;
        Rtc = 0;
        Rva = 0;
        Rvb = 0;

        B_V = [ - 1 , 0 ,   0 ,   0 , 0 ,   0 , 0 , 1 , 0 , 0 , 0 ;
                  0 , 0 ,   0 , - 1 , 1 ,   0 , 0 , 0 , 1 , 0 , 0 ;
                  1 , 1 , - 1 ,   1 , 0 , - 1 , 0 , 0 , 0 , 1 , 0 ;
                  0 , 0 , - 1 ,   0 , 0 , - 1 , 1 , 0 , 0 , 0 , 1 ];
        B_I = [ - 1 , 0 ,   0 , 0 , 0 ,   0 , 0 , 1 , 0 , 0 , 0 ;
                  1 , 0 ,   0 , 0 , 1 ,   0 , 0 , 0 , 1 , 0 , 0 ;
                  0 , 1 , - 1 , 0 , 0 , - 1 , 0 , 0 , 0 , 1 , 0 ;
                  0 , 0 , - 1 , 0 , 0 , - 1 , 1 , 0 , 0 , 0 , 1 ];

        Z = zeros( 11 );
        S = zeros( 11 );

        b = zeros( 11 , 1 );
        a = zeros( 11 , 1 );
    end

    methods

        function out = process( p , in )
            if strcmp( p.Bypass , 'OFF' )
                out = zeros( size( in , 1 ) , 1 );
                for ii = 1 : size( in , 1 )
                    processWDF( p , in( ii ) );
                    out( ii ) = ( p.a( 7 ) + p.b( 7 ) ) / 2;
                end   
            else
                out = in;
            end
        end

        function reset( p )
            p.f_s = getSampleRate( p );
            initializeZ( p );
            p.b = zeros( 11 , 1 );
            p.a = zeros( 11 , 1 );
        end

        function set.Switch( p , state )
            p.Switch = state;
            updateRsw( p );
        end

        function set.Tone( p , value )
            p.Tone = value;
            updateRtc( p );
        end

        function set.Volume( p , value )
            p.Volume = value;
            updateRvx( p );
        end

        function set.Bypass( p , state )
            p.Bypass = state;
        end

        function updateRsw( p )
            if strcmp( p.Switch , 'LP' )
                p.Rsw = p.Rs( 1 );
            else
                p.Rsw = p.Rs( 2 );
            end
            p.Z( 10 , 10 ) = p.Rsw;
            updateS( p );
        end

        function updateRtc( p )
            p.Rtc = p.Rt * p.Tone + p.Rtol;
            p.Z( 6 , 6 ) = p.Rtc;
            updateS( p );
        end

        function updateRvx( p )
            p.Rva = p.Rv * ( p.A * p.B ^ p.Volume + p.C ) + p.Rtol;
            p.Rvb = p.Rv * ( 1 - p.A * p.B ^ p.Volume - p.C ) + p.Rtol;
            p.Z( 7 , 7 ) = p.Rva;
            p.Z( 11 , 11 ) = p.Rvb;
            updateS( p );
        end

        function initializeZ( p )
            Z_C5 = 1 / ( 2 * p.f_s * p.C5 );
            Z_C6 = 1 / ( 2 * p.f_s * p.C6 );
            Z_C7 = 1 / ( 2 * p.f_s * p.C7 );
            Z_C8 = 1 / ( 2 * p.f_s * p.C8 );
            p.Rsw = p.Rs( 1 );
            p.Rtc = p.Rtol;
            p.Rva = 0.1 * p.Rv + p.Rtol;
            p.Rvb = 0.1 * p.Rv + p.Rtol;
            p.Z = diag( [ Z_C5 , Z_C7 , Z_C8 , p.Rg , Z_C6 , p.Rtc , p.Rva , p.R9 , p.R10 , p.Rsw , p.Rvb ] );
            updateS( p );
        end

        function updateS( p )
            p.S = eye( 11 ) - 2 * p.Z * p.B_I' * ( ( p.B_V * p.Z * p.B_I' ) \ p.B_V );
        end

        function processWDF( p , in )
            p.b( 1 ) = p.a( 1 );
            p.b( 2 ) = p.a( 2 );
            p.b( 3 ) = p.a( 3 );
            p.b( 4 ) = in;
            p.b( 5 ) = p.a( 5 );
            p.b( 6 ) = 0;
            p.b( 7 ) = 0;
            p.b( 8 ) = 0;
            p.b( 9 ) = 0;
            p.b( 10 ) = 0;
            p.b( 11 ) = 0;
            p.a = p.S * p.b;
        end

    end
end
