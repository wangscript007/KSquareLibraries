/* Chart.cpp -- Used to build Charts from data-series.
 * Copyright (C) 1994-2011 Kurt Kramer
 * For conditions of distribution and use, see copyright notice in KKB.h
 */
#include "FirstIncludes.h"

#include <float.h>
#include <math.h>
#include <stdio.h>

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

#include "MemoryDebug.h"


using namespace std;


#include "Chart.h"
#include "ImageIO.h"


namespace  KKB
{
  class  Chart::PlotPoint
  {
  public:
    PlotPoint (double _xVal,
               double _yVal
              ):
      xVal (_xVal),
      yVal (_yVal)
    {}


    PlotPoint (kkint32 _xVal,
               kkint32 _yVal
              ):
      xVal (double (_xVal)),
      yVal (double (_yVal))
    {}


    double  XVal () const  {return xVal;}
    double  YVal () const  {return yVal;}


  private:
    double  xVal;
    double  yVal;
  };



  class  Chart::Series
  {
  public:
    Series (KKStr       _name,
            PixelValue  _color
           );


    void  AddAValue (float  _xVal,
                     float  _yVal
                    );

    kkint32 Size ()  {return  (kkint32)points.size ();}


  private:
    KKStr              name;
    PixelValue         color;
    vector<PlotPoint>  points;

    friend class  Chart;
  };


  typedef  Chart::SeriesPtr   SeriesPtr;

  typedef  vector<SeriesPtr>  SeriesList;


  class  Chart::XLabel
  {
  public:  
    XLabel (double  _xVal,
            KKStr  _name
           ):
       name (_name),
       xVal (_xVal)
     {}

   const  
   KKStr&  Name ()  {return name;}

   double   XVal ()  {return xVal;}

   bool  operator< (const XLabel&  right)  const
   {
     return  xVal < right.xVal;
   }

  private:
    KKStr  name;
    double  xVal;
  };
}  /* KKB */



using namespace KKB;



Chart::Series::Series (KKStr      _name,
                       PixelValue  _color
                      ):
   name  (_name),
   color (_color)
{
}



void  Chart::Series::AddAValue (float  _xVal,
                                float  _yVal
                               )
{
  points.push_back (PlotPoint (_xVal, _yVal));
}  /* AddAValue */






KKB::Chart::Chart (KKStr  _title):
  title      (_title),
  series     (),
  xLabels    (),
  minXValue  (FloatMax),
  maxXValue  (FloatMin),
  minYValue  (FloatMax),
  maxYValue  (FloatMin),

  maxPointsPlotedInASeries (0),
  numOfDefaultColorsUsed (0),

  chartWidth  (0),
  chartHeight (0),
  xOffset     (0),
  yOffset     (0),
  yRange      (0.0),
  xRange      (0.0),
  yScale      (0.0),
  xScale      (0.0),
  xMin        (0.0),
  xMax        (0.0),
  yMin        (0.0),
  yMax        (0.0),
  yIncrement  (0.0)

{
}


KKB::Chart::~Chart ()
{
  for  (kkuint32 x = 0;  x < series.size ();  x++)
  {
    delete  series[x];
    series[x] = NULL;
  }
}




void  KKB::Chart::AddAValue (kkuint32  _seriesIDX,
                             float     _xVal,
                             float     _yVal
                            )
{
  if  (_seriesIDX >= series.size ())
  {
    cerr << std::endl
         << "*** ERROR ***  Chart::AddAValue" << std::endl
         << "               Invalid SeriesIDX[" << _seriesIDX << "]"  << std::endl
         << "               Only [" << series.size () << "] series defined." << std::endl
         << std::endl;
  }

  series[_seriesIDX]->AddAValue (_xVal, _yVal);

  if  (series[_seriesIDX]->Size ()  >  maxPointsPlotedInASeries)
     maxPointsPlotedInASeries = series[_seriesIDX]->Size ();


  if  (_xVal <  minXValue)
    minXValue = _xVal;

  if  (_xVal >  maxXValue)
    maxXValue = _xVal;

  if  (_yVal <  minYValue)
    minYValue = _yVal;

  if  (_yVal >  maxYValue)
    maxYValue = _yVal;

}  /* AddAValue */




static
PixelValue  defaultColors[] = {PixelValue (255,   0,   0),  // Red 
                               PixelValue (  0, 255,   0),  // Green
                               PixelValue (  0,   0, 255),  // Blue
                               PixelValue (  0,   0,   0),  // Black
                               PixelValue (255, 255,   0),  // red-Green
                               PixelValue (255,   0, 255),  // red-Blue
                               PixelValue (  0, 255, 255),  // Green-Blue
                               PixelValue ( 70,  70,  70),  // Gray
                               PixelValue (120,   0,   0),  // Lite-Blue
                               PixelValue (  0, 120,   0),  // Lite-Green
                               PixelValue (  0,   0, 120),  // Lite-Blue
                               PixelValue (120, 120,   0),  // Lite-Red-Green
                               PixelValue (120,   0, 120),  // Lite-Red-Blue
                               PixelValue (  0, 120, 120)   // Lite-Green-Blue
                              };

static
const
kkint32  numOfDefinedDefaultColors = 14;



void  KKB::Chart::DefineASeries (KKStr  _name)
{
  series.push_back (new Series (_name, defaultColors[numOfDefaultColorsUsed % numOfDefinedDefaultColors]));
  numOfDefaultColorsUsed++;
}



void  KKB::Chart::DefineASeries (KKStr      _name,
                                 PixelValue  _color
                                 )
{
  series.push_back (new Series (_name, _color));
}




void  KKB::Chart::DefineAXLabel (float  _xVal,
                                 KKStr  _name
                                )
{
  xLabels.push_back (XLabel (_xVal, _name));
}






Point  Chart::RasterCoordinates (const PlotPoint&  plotPoint)
{
  kkint32  x = (kkint32)(((double)plotPoint.XVal () - xMin) * xScale + (double)xOffset + 0.5);
  kkint32  y = (kkint32)(((double)plotPoint.YVal () - yMin) * yScale + (double)yOffset + 0.5);
  return  Point (y, x);
}    /* ConvertToRasterCoordinates */                                    
      
                                   




RasterPtr  Chart::CreateRaster () 
{
  PixelValue  axisColor     (  0,   0,   0);
  PixelValue  scaleColor    (190, 190, 190);
  PixelValue  zeroAxisColor (240, 132, 240);

  chartWidth = maxPointsPlotedInASeries * 10;

  if  (chartWidth > 1000)
    chartWidth = 1000;

  chartHeight = chartWidth;

  if  (chartHeight > 500)
    chartHeight = 500;


  xOffset = 10;
  yOffset = 10;

  
  yRange = maxYValue - minYValue;
  xRange = maxXValue - minXValue;

  yScale = (double)chartHeight / yRange;
  xScale = (double)chartWidth  / xRange;

  yMin = minYValue;
  yMax = maxYValue;

  xMin = minXValue;
  xMax = maxXValue;
  

  {
    // Lets refine the scaling
    double  sigTensPlace = floor (log10 (yRange));

    sigTensPlace = pow (10.0, sigTensPlace);

    yIncrement = floor (yRange / sigTensPlace) * sigTensPlace;

    kkint32  numOfYIncremets = (kkint32)((yRange / yIncrement) + 0.5);

    if  (numOfYIncremets < 2)
    {
      yIncrement = yIncrement / 4.0;
    }
    else if  (numOfYIncremets < 5)
    {
      yIncrement = yIncrement / 2.0;
    }
    else if  (numOfYIncremets > 7)
    {
      yIncrement = yIncrement * 2.0;
    }
    else if  (numOfYIncremets > 13)
    {
      yIncrement = yIncrement * 4.0;
    }

    yMin = floor (minYValue / yIncrement) * yIncrement;
    yMax = ceil  (maxYValue / yIncrement) * yIncrement;

    yRange = yMax - yMin;
    yScale = (double)chartHeight / yRange;
  }


  kkint32  height = chartHeight + 2 * yOffset;
  kkint32  width  = chartWidth  + 2 * xOffset;

  RasterPtr  chart = new Raster (height, width, true);

  {
    // Draw Main Axis
    chart->DrawLine (yOffset, xOffset, yOffset + chartHeight, xOffset,              axisColor);
    chart->DrawLine (yOffset, xOffset, yOffset              , xOffset + chartWidth, axisColor);
  }


  {
    // Draw y scales

    double  y = yMin + yIncrement;

    while  (y < yMax)
    {
      chart->DrawLine (RasterCoordinates (PlotPoint (0.0,  (double)y)),
                       RasterCoordinates (PlotPoint (xMax, (double)y)),
                       scaleColor
                      );
      y += yIncrement;
    }
  }


  if  ((yMin < 0.0)  &&  (yMax > 0.0))
  {
    // Draw a line for y=0.0
    chart->DrawLine (RasterCoordinates (PlotPoint (double ((kkint32)(xMin + 0.5)),  0.0)),
                     RasterCoordinates (PlotPoint (double ((kkint32)(xMax + 0.5)),  0.0)),
                     zeroAxisColor
                    );

  }



  if  ((xMin < 0.0)  &&  (xMax > 0.0))
  {
    // Draw a line for x=0.0
    chart->DrawLine (RasterCoordinates (PlotPoint (0.0,  double ((kkint32)(yMin + 0.5)))),
                     RasterCoordinates (PlotPoint (0.0,  double ((kkint32)(yMax + 0.5)))),
                     zeroAxisColor
                    );

  }



  {
    // Lets plot the individual series

    for  (kkuint32 seriesIDX = 0;  seriesIDX < series.size ();  seriesIDX++)
    {
      SeriesPtr  s = series[seriesIDX];

      if  (s->points.size () == 0)
        continue;

      PlotPoint  lastPoint (s->points[0]);
      
      chart->DrawDot (RasterCoordinates (lastPoint), s->color, 3);

      for  (kkuint32 plotIDX = 1;  plotIDX < s->points.size ();  plotIDX++)
      {
        PlotPoint  point (s->points[plotIDX]);

        chart->DrawDot (RasterCoordinates (point), s->color, 3);

        chart->DrawLine (RasterCoordinates (lastPoint), 
                         RasterCoordinates (point),
                         s->color
                        );

        lastPoint = point;
      }
    }
  }


  return  chart;
}  /* CreateRaster */




void  Chart::SaveAsImage (KKStr  _fileName)
{
  RasterPtr  raster = CreateRaster ();
  SaveImage  (*raster, _fileName); 
} /* SaveAsImage */




kkint32  Chart::LookUpXLableIDX (double  xVal)
{
  for  (kkuint32 x = 0;  x < xLabels.size ();  x++)
  {
    if  (xLabels[x].XVal () == xVal)
      return  x;
  }

  return -1;
} /* LookUpXLableIDX */




Chart::PlotPointPtr  Chart::LookUpPoint (kkint32 seriesIDX,
                                         double  xVal
                                        )
{
  Series&  s = *(series[seriesIDX]);

  kkuint32  pIDX;
  for  (pIDX = 0;  pIDX <= s.points.size ();  pIDX++)
  {
    if  (s.points[pIDX].XVal () == xVal)
      return  &s.points[pIDX];
  }

  return  NULL;
}  /* LookUpPoint */




void  Chart::Save (KKStr  _fileName)
{
  ofstream  o (_fileName.Str ());
  if  (!o.is_open ())
  {
    cerr << std::endl
         << "*** ERROR ***     Chart::Save,    Error Opening file[" << _fileName << "]." << std::endl
         << std::endl;
    return;
  }


  {
    // Make sure we have a XLabel for all Points Plotted
    for  (kkuint32 seriesIDX = 0;  seriesIDX < series.size ();  seriesIDX++)
    {
      SeriesPtr s = series[seriesIDX];
      for  (kkuint32  plotIDX = 0;  plotIDX < s->points.size ();  plotIDX++)
      {
        PlotPoint& p = s->points[plotIDX];
        if  (LookUpXLableIDX (p.XVal ()) < 0)
        {
          char  buff[80];

#         ifdef  USE_SECURE_FUNCS
            sprintf_s (buff, sizeof (buff), "%g", p.XVal ());
#         else
            sprintf (buff, "%g", p.XVal ());
#         endif

          xLabels.push_back (XLabel (p.XVal (), buff));
        }
      }
    }
  }


  std::sort (xLabels.begin (), xLabels.end ());


  {
    // Output Header Info
    o << "Title" << "\t" << title << std::endl;
    o << "MinAndMaxXValues:" << "\t" << minXValue << "\t" << maxXValue << std::endl;
    o << "MinAndMaxYValues:" << "\t" << minYValue << "\t" << maxYValue << std::endl;

    o << "MaxPointsPlottedInASeries" << "\t" << maxPointsPlotedInASeries << std::endl;
    o << std::endl;
  }


  o << std::endl;
  o << std::endl;


  {
    // Column Headers 
    o << "XLabel" << "\t" << "XValue"; 
    for  (kkint32 seriesIDX = 0;  seriesIDX < NumOfSeries ();  seriesIDX++)
    {
      o << "\t" << series[seriesIDX]->name;
    }
    o << std::endl;
  }



  {
    kkuint32  x;

    for  (x = 0;  x < xLabels.size (); x++)
    {
      XLabel&  xLabel = xLabels[x];

      double  xVal = xLabel.XVal ();


      o << xLabel.Name () << "\t" << xVal;

      kkint32  seriesIDX = 0;

      for  (seriesIDX = 0;  seriesIDX < NumOfSeries ();  seriesIDX++)
      {
        o << "\t";

        PlotPointPtr  p = LookUpPoint (seriesIDX, xVal);
        if  (p)
        {
          o << p->YVal ();
        }
      }

      o << std::endl;
    }
  }
     

  o.close ();
}    /* Save */

