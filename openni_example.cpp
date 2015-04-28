// STL Header
#pragma once
#include <iostream>
// 1. include OpenNI Header
#include "OpenNI.h"
#include < stdlib.h >
#include <time.h>     //計篹存檔時間

using namespace std;

int main( int argc, char** argv )
{
     // 2. initialize OpenNI    
     if( openni::OpenNI::initialize() != openni::STATUS_OK )
     {   //在使用openni::OpenNI裡任何的method之前, 必須先calling initialize
          cerr << "OpenNI Initial Error: "
          << openni::OpenNI::getExtendedError() << endl;
          //getExtendedError():擷取錯誤資訊。每次呼叫OpenNI method時,getExtendedError()都會清空, 所以使用此getExtendedError()時, 須在發生錯誤的OpenNI method後馬上呼叫
          return -1;
     }

     // 3. open a device
     openni::Device devAnyDevice;
     //宣告devAnyDevice為Device
     if( devAnyDevice.open( openni::ANY_DEVICE ) != openni::STATUS_OK )
     {
          cerr << "Can't Open Device: "
          << openni::OpenNI::getExtendedError() << endl;
          return -1;
     }

     // 4. create depth stream
     openni::VideoStream streamDepth;
     //宣告streamDepth為VideoStream
     streamDepth.create( devAnyDevice, openni::SENSOR_DEPTH );
     // set video mode
     openni::VideoMode vmMode;
     //宣告vmMode為VideoMode
     vmMode.setFps( 30 );
     vmMode.setResolution( 640, 480 );
     vmMode.setPixelFormat( openni::PIXEL_FORMAT_DEPTH_1_MM );
     if( streamDepth.setVideoMode( vmMode ) == openni::STATUS_OK )
     {
       // OK
     }
     // output basic video information
     /*openni::VideoMode*/ vmMode = streamDepth.getVideoMode();
     cout << "Video Mode : " << vmMode.getResolutionX();
     cout << " * " << vmMode.getResolutionY();
     cout << " @ " << vmMode.getFps() << "FPS";
     switch( vmMode.getPixelFormat() )
     {
     case openni::PIXEL_FORMAT_DEPTH_1_MM:
     cout << " , Unit is 1mm" << endl;
     break;

     case openni::PIXEL_FORMAT_DEPTH_100_UM:
     cout << " , Unit is 100um" << endl;
     break;
     }
     //影像校正
     if( devAnyDevice.isImageRegistrationModeSupported( openni::IMAGE_REGISTRATION_DEPTH_TO_COLOR ) )
     {
          devAnyDevice.setImageRegistrationMode( openni::IMAGE_REGISTRATION_DEPTH_TO_COLOR );
     }
     Sleep(1000);
     streamDepth.start();

     // 4a. create color stream
     //openni::VideoStream streamColor;
     //streamColor.create( devAnyDevice, openni::SENSOR_COLOR );
     //streamColor.start();

     // 5 main loop, continue read
     openni::VideoFrameRef frameDepth;
     //openni::VideoFrameRef frameColor;
    
     //檔案處理
     FILE *output;
     string myString = "C:\\output_@.asc";
     string DataNo;
    
     system("pause");
     //時間設定相關參數
     clock_t start, end;
    
     for( int i = 0; i < 30; ++ i ) //拍攝30張資料
     {
          // 5.1 get frame
          streamDepth.readFrame( &frameDepth );
          //streamColor.readFrame( &frameColor );

          // 5.2 get data array
          const openni::DepthPixel* pDepth = (const openni::DepthPixel*)frameDepth.getData();
          //const openni::RGB888Pixel* pColor = (const openni::RGB888Pixel*)frameColor.getData();

          // 5.3 output the depth value of center point
          int idx = frameDepth.getWidth() * ( frameDepth.getHeight() + 1 ) / 2;
          std::cout  << pDepth[idx] << std::endl;
          //檔案處理
          if(i==10)
          {
               //atoi()
               output = fopen("C:\\output.asc","wt");

               start = clock();     //1000 clock相當一秒
                    for ( int H=0; H < frameDepth.getHeight(); H++ )
                    {
                         for ( int W=0; W < frameDepth.getWidth(); W++ )
                         {
                              if( pDepth[frameDepth.getWidth()*H+W] != 0 && pDepth[frameDepth.getWidth()*H+W] <= 1500 )
                              {
                                   //fprintf( output, "%d\t%d\t%d\n", W,H,pDepth[frameDepth.getWidth()*H+W] );
                                   float fX, fY, fZ;
                                   openni::CoordinateConverter::convertDepthToWorld(     streamDepth,
                                                                                                    W, H, pDepth[frameDepth.getWidth()*H+W],
                                                                                                    &fX, &fY, &fZ );
                                   fprintf( output, "%f\t%f\t%f\n", fX, fY, fZ );
                              }
                         }
                    }
               fclose(output);
               end = clock();
          }
          //-----------------------------------

     }
     int CostTime = end - start;
     cout << "CostTime:" << CostTime << endl;
     system("pause");
  // 6. close
  streamDepth.destroy();
  //streamColor.destroy();
  devAnyDevice.close();

  // 7. shutdown
  openni::OpenNI::shutdown();

  return 0;
}
