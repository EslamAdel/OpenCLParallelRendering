#include "Timer.h"
#include <stdio.h>
#include <stdlib.h>
#include <Colors.hh>

template< class T >
Timer< T >::Timer( const std::string label )
{
    label_ = label;

}

template< class T >
void Timer< T >::start()
{
    gettimeofday( &this->timer_[ 0 ] , NULL );
}

template< class T >
void Timer< T >::stop()
{
    gettimeofday( &this->timer_[ 1 ], NULL);
    accumulate_();
}

template< class T >
void Timer< T >::print( const uint tabs  )
{

    T mean , std , var , min , max ;

    performStatistics_( mean , var , min , max );
    std = sqrt( var );

    for( uint i = 0 ; i < tabs ; i++ )
        printf("\t");

    printf(STD_WHITE  "%s>\n\t\t" STD_RESET
           STD_YELLOW "mean:%f|"STD_RESET
           STD_YELLOW "std:%f|"STD_RESET
           STD_GREEN  "min:%f|"STD_RESET
           STD_RED    "max:%f\n"STD_RESET ,
           label_.c_str() ,
           mean , std , min , max ) ;

}

template< class T >
void Timer< T >::performStatistics_( T &mean , T &var , T &min ,
                                     T &max )
{
    const uint64_t count = data_.size() ;
    T sum = 0 ;

    max = data_[ 0 ];
    min = data_[ 0 ];

    for( const auto datum : data_ )
    {
        sum += datum ;

        if( datum > max )
            max = datum;

        if( datum < min )
            min = datum;
    }

    mean = sum / count ;

    T meanSquareError = 0 ;

    for( const auto datum : data_ )
        meanSquareError += ( datum - mean) * ( datum - mean );

    var = meanSquareError / count ;
}

template< class T >
void Timer< T >::accumulate_( )
{
    const T duration = duration_( );

    data_.push_back( duration );

    //If the vector size is about to exceed the capacity, reserve/allocate
    //more space for a considered count of elements in advance,
    //to reduce allocation overhead.
    if( data_.size() + 2 > data_.capacity() )
        data_.reserve( data_.size() + 100 );
}

template< class T >
T Timer< T >::duration_() const
{
    int secs( this->timer_[ 1 ].tv_sec - this->timer_[ 0 ].tv_sec );
    int usecs( this->timer_[ 1 ].tv_usec - this->timer_[ 0 ].tv_usec );

    if( usecs < 0)
    {
        --secs;
        usecs += 1000000;
    }

    return static_cast<T>( secs * 1000 + usecs / 1000.0 );
}


#include "Timer.ipp"
