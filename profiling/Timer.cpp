#include "Timer.h"
#include "Logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <Colors.hh>

Timer::Timer( const std::string label )
{
    label_ = label;
}

void Timer::start()
{
    gettimeofday( &this->timer_[ 0 ] , NULL );
}

void Timer::stop()
{
    gettimeofday( &this->timer_[ 1 ], NULL);
    accumulate_();
}

void Timer::print( const uint tabs  )
{
    for( auto i = 0 ; i < tabs ; i++ )
        printf("\t");
    printf(STD_WHITE  "%s>\n\t\t" STD_RESET
           STD_YELLOW "mean:%f,"STD_RESET
           STD_YELLOW "var:%f,"STD_RESET
           STD_GREEN  "min:%f,"STD_RESET
           STD_RED    "max:%f\n"STD_RESET ,
           label_.c_str() ,
           mean( statistics_ ) ,
           variance( statistics_ ),
           min( statistics_ ),
           max( statistics_ ) );

}

void Timer::accumulate_( )
{
    statistics_( duration_( ));
}

double Timer::duration_() const
{
    int secs( this->timer_[ 1 ].tv_sec - this->timer_[ 0 ].tv_sec );
    int usecs( this->timer_[ 1 ].tv_usec - this->timer_[ 0 ].tv_usec );

    if( usecs < 0)
    {
        --secs;
        usecs += 1000000;
    }

    return static_cast<double>( secs * 1000 + usecs / 1000.0 );
}
