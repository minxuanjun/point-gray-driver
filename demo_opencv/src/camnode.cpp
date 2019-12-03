#include "FLIR.h"

using namespace FLIR;
unsigned int width = 640;
unsigned int height = 480;

// int main(int argc, char ** argv)
// {
//     if(argc==3){
//         width=atoi(argv[1]);
//         height=atoi(argv[2]);
//     }

//     mGigEGrab cam(width,height);

//      if(!cam.detectCam())return 0;

//     if(!cam.selectCam())return 0;

//     if(!cam.openCam())return 0;

//     cv::Mat img;
//     while(cv::waitKey(30)!=27)
//     {
//         cam.RetrieveBGR(img);
//         cv::imshow(cam.ipStr,img);
//     }

//     img.release();
//     cv::destroyAllWindows();

//     return 0;
// }

using namespace FlyCapture2;

int main(int argc, char *argv[])
{
    Error error;
    Camera camera;
    CameraInfo camInfo;

    BusManager busManager;
    PGRGuid Guid;
    error = busManager.GetCameraFromIndex(0, &Guid);
    //id: 787779698224061208220913002771374915717
    std::cout << "camera id: ";
    for(int i =0; i < 4; i++)
    std::cout <<Guid.value[i];
    std::cout << endl; 
    // Connect the camera
    error = camera.Connect(&Guid);
    if (error != PGRERROR_OK)
    {
        std::cout << "Failed to connect to camera" << std::endl;
        return false;
    }

    // Get the camera info and print it out
    error = camera.GetCameraInfo(&camInfo);
    if (error != PGRERROR_OK)
    {
        std::cout << "Failed to get camera info from camera" << std::endl;
        return false;
    }
    std::cout << camInfo.vendorName << " "
              << camInfo.modelName << " "
              << camInfo.serialNumber << std::endl;

    error = camera.StartCapture();
    if (error == PGRERROR_ISOCH_BANDWIDTH_EXCEEDED)
    {
        std::cout << "Bandwidth exceeded" << std::endl;
        return false;
    }
    else if (error != PGRERROR_OK)
    {
        std::cout << "Failed to start image capture" << std::endl;
        return false;
    }

    // capture loop
    char key = 0;
    while (key != 'q')
    {
        // Get the image
        Image rawImage;
        Error error = camera.RetrieveBuffer(&rawImage);
        if (error != PGRERROR_OK)
        {
            std::cout << "capture error" << std::endl;
            continue;
        }

        // convert to rgb
        Image rgbImage;
        rawImage.Convert(FlyCapture2::PIXEL_FORMAT_BGR, &rgbImage);

        // convert to OpenCV Mat
        unsigned int rowBytes = (double)rgbImage.GetReceivedDataSize() / (double)rgbImage.GetRows();
        cv::Mat image = cv::Mat(rgbImage.GetRows(), rgbImage.GetCols(), CV_8UC3, rgbImage.GetData(), rowBytes);

        cv::imshow("image", image);
        key = cv::waitKey(30);
    }

    error = camera.StopCapture();
    if (error != PGRERROR_OK)
    {
        // This may fail when the camera was removed, so don't show
        // an error message
    }

    camera.Disconnect();
    return 0;
}