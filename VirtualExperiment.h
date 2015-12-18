#ifndef VIRTUALEXPERIMENT_H
#define VIRTUALEXPERIMENT_H



class VirtualExperiment
{
public:
    enum class ProcessOrder : int  { Rendering = 3  , Compression = -2 , Compositing = 1 };

    VirtualExperiment( const ProcessOrder processOrder );

    double processScale() const ;

    static double processScale( ProcessOrder processOrder ) ;
private:

    const ProcessOrder processOrder_ ;
};

#endif // VIRTUALEXPERIMENT_H
