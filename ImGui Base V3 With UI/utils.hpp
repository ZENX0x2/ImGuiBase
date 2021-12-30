#include "includes.hpp"

namespace utils {

    inline std::string random_string( int length = 6 ) {
        std::string result = { };
        std::string alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

        srand( (unsigned)time( 0 ) * 5 );

        for ( unsigned int i = 0; i < length; i++ ) 
            result += alphabet[rand( ) % ( alphabet.length( ) - 1 )];

        return result;
    }
    
    inline std::string get_current_process( ) {
        char buf[MAX_PATH];
        GetModuleFileNameA( 0 , buf , MAX_PATH );
        return buf;
    }

    inline void spoof_exe( bool should_spoof = false /* set to true, i turned it off for debugging :) */ ) {
        if ( should_spoof ) {
            auto title = random_string( );
            auto name  = random_string( );

            SetConsoleTitleA( title.c_str( ) );
            rename( get_current_process( ).c_str( ) , name.c_str( ) );

            return;
        }
    }

}