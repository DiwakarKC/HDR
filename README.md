SDR to HDR conversion
THe Main purpose of this project is to enhance the SDR video/image to HDR quality for and SDR display

Users define the Video file that is needed to be enhanced.First the audio file (if there is ) is retrieved and stored in a “audio.acc” for later combination to the HDR Video.First, each frame is read from the video file and then processed to the HDR processing. Each algorithm is custom written lines of code without using the opencv functions for enhancement.

Gamma correction is done for increasing or decreasing brightness level required by use.
CLAHE is done for improving the local contrast of the image and revealing the hidden details in the image.
SigmoidTM is used as a compression technique that maps the wide HDR luminance values to SDR displayable range
Unsharp Mask is used to enhance the sharpness in the image.

Then the frames are combined together to get the HDR enhanced video file.

In the above figure, the GUI interface with a slider for each parameter is used for the user to set the parameter as required and the changes are shown dynamically with the image. The parameter is saved as .yml file,So when the program is run the parameters are set as default. Each time the parameters are changed, it is saved.

OPTIMIZATION
Since each frame is sequentially done. The time for completion for large video files is very long.To decrease the time taken, parallel processing is done. Instead of 1 frame , 3 frames are simultaneously processed to save time. For example,Before Optimization, it took 3 minutes to complete HDR conversion of a 30 sec video. After Optimization it took 1 minute to complete 30 sec. 

Sept 19th 2026
Added the deploy.bat file that will create a bin folder and add all the dependencies on the bin folder that is required by image_processing.exe file
Also added the launcher.cpp file that is created to add the bin folder on the environment varaible paths for the imageprocessing.exe file
Run the Launcher.exe file to run the imageprocessing.exe file
