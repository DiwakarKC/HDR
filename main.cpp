#include "header.h"

void onParameterchange(int,void* userdata)
{
    Parameters* data =(Parameters*)userdata;
    data->cliplimit = getTrackbarPos("Clip","Parameters");
    data->val = getTrackbarPos("Gamma","Parameters");
    data->st = getTrackbarPos("Strength","Parameters");
    data->c = getTrackbarPos("Center","Parameters");
    data->level = getTrackbarPos("Sharp","Parameters");
    int gam[256];
    for(int i=0; i<256; i++)
    {
        gam[i] = 255 * pow(i/255.0,(data->val)/10.0);
    }
    gamma_lut(gam,data->input,data->output);
    Clahe(data->cliplimit,data->output,data->Clahe_output);
    Sigmoid_TM(data->st,data->c,data->Clahe_output,data->Sig_output);
    unmask_sharp(data->level,data->Sig_output,data->final_output);
//    WBC(data->Sig_output,data->final_output);
    imshow("Output",data->final_output);

}
Mat frameprocess(const Mat& input,int gam[],int cliplimit,int st,int c,int level)
{
    Mat gammaout;
    Mat claheout;
    Mat sigout;
    Mat finalout;

    gamma_lut(gam,input,gammaout);
    Clahe(cliplimit,gammaout,claheout);
    Sigmoid_TM(st,c,claheout,sigout);
    unmask_sharp(level,sigout,finalout);

    return finalout;

}

// Using the OpenCV Trackbar to change the value of variables(cliplimit,gamma etc)
int main()
{
    Parameters data;
    int val=5,st=8,c=5,limit=50,level=50;
    int ID=0;
    int count_in=0;
    Mat frame;
    int count = 0;
    cout<< "Enter the video file: ";
//    string input_file = "C:\\Users\\Diwakar\\Documents\\New folder\\Image_Processing\\video\\Emotions-30Sec.mp4";
    string input_file;
    getline(cin,input_file);
    cout << "Enter the frame number: ";
    cin >> count_in;

    fs::path input_path(input_file);

    string output_file = input_path.stem().string() + "_HDR_Merged" + input_path.extension().string();
//    fs::path output_path = input_path.parent_path() / (output_file);
    fs::path output_path = "video/" + (output_file);

    // Reading the input video file to extract audio"
//    string command_in ="ffmpeg -y "
//                       "-i \"" + input_path.string() + "\" "
//                       "-vn "
//                       "-acodec copy "
//                       "audio.aac";

    string command_in ="ffmpeg -y "
                       "-i \"" + input_path.string() + "\" "
                       "-vn "
                       "-c:a aac "
                       "-b:a 192k "
                       "audio.aac";

    system(command_in.c_str());

    //Creating a folder name, if not there
    string folder="setting";
    if(!fs::exists(folder))
        fs::create_directory(folder);

    folder="video";
    if(!fs::exists(folder))
        fs::create_directory(folder);

    folder = "image";
    if(!fs::exists(folder))
        fs::create_directory(folder);
    //--------------------------------------------------

    // Reading the setting.yml file for default the parameters value
    // IF the program is run for first time there will be no setting.yml file ,but there will be no issue
    FileStorage fsRead("setting/setting.yml",FileStorage::READ);
    if(fsRead.isOpened())
    {
        fsRead["limit"]>>limit;
        fsRead["gamma"]>>val;
        fsRead["strength"]>>st;
        fsRead["center"]>>c;
        fsRead["sharp"]>>level;
        fsRead.release();
    }
    VideoCapture vid(input_file);
    if(!vid.isOpened())
    {
        cout<<"Video file not found"<<endl;
        return -1;
    }
    int frame_width = vid.get(CAP_PROP_FRAME_WIDTH);
    int frame_height = vid.get(CAP_PROP_FRAME_HEIGHT);
    double fps = vid.get(CAP_PROP_FPS);

    VideoWriter writer("video/output.mp4",VideoWriter::fourcc('a','v','c','1'),fps,Size(frame_width,frame_height));

    if(!writer.isOpened())
        return -1;

    while(vid.read(frame))
    {
        ID++;
        if(ID==count_in)
        {
            break;
        }
    }

    data.input = frame.clone();

    if(data.input.empty())
    {
        cout<<"Invalid Input Image";
        return -1;
    }
    //Creating a window for track bar
    namedWindow("Parameters",WINDOW_AUTOSIZE);
    // Creating a trackbar for each paramters in a single window
    createTrackbar("Clip","Parameters",&limit,250,onParameterchange,&data);
    createTrackbar("Gamma","Parameters",&val,20,onParameterchange,&data);
    createTrackbar("Strength","Parameters",&st,15,onParameterchange,&data);
    createTrackbar("Center","Parameters",&c,10,onParameterchange,&data);
    createTrackbar("Sharp","Parameters",&level,100,onParameterchange,&data);

    //Callback function that will run the code unless terminated
    onParameterchange(0,&data);
    waitKey(0);
    destroyAllWindows();

    imwrite("image/gamma_output.jpg",data.output);
    imwrite("image/clahe_output.jpg",data.Clahe_output);
    imwrite("image/Sig_output.jpg",data.Sig_output);
    imwrite("image/Final_output.jpg",data.final_output);

    // To restart the video frame reading from 0
    vid.set(CAP_PROP_POS_FRAMES,0);

    Mat frame1;
    Mat frame2;
    Mat frame3;

    int gam[256];
    for(int i=0; i<256; i++)
    {
        gam[i] = 255 * pow(i/255.0,data.val/10.0);
    }

    while(true)
    {
        if(!vid.read(frame1))
        {
            break;
        }
        bool hasframe2 = vid.read(frame2);
        bool hasframe3 = vid.read(frame3);
        future<Mat> future1 = async(launch::async,frameprocess,frame1.clone(),gam,data.cliplimit,data.st,data.c,data.level);
        future<Mat> future2;
        if(hasframe2)
        {
            future2 = async(launch::async,frameprocess,frame2.clone(),gam,data.cliplimit,data.st,data.c,data.level);
        }
        future<Mat> future3;
        if(hasframe3)
        {
            future3 = async(launch::async,frameprocess,frame3.clone(),gam,data.cliplimit,data.st,data.c,data.level);
        }
        Mat result1 = future1.get();
        writer.write(result1);
        if(hasframe2)
        {
            Mat result2 = future2.get();
            writer.write(result2);
        }
        if(hasframe3)
        {
            Mat result3 = future3.get();
            writer.write(result3);
        }
        count++;
        cout <<count<<endl;
        if(!hasframe2)
        {
            break;
        }
        if(!hasframe3)
        {
            break;
        }
    }
    // Releasing the input and output video to complete them
    vid.release();
    writer.release();
    cout << "Final" <<endl;

    // Command for reading the HDR output video and audio.acc and then merge them together
    string command =
        "ffmpeg -y "
        "-i video/output.mp4 "
        "-i audio.aac "
        "-c:v copy "
        "-c:a copy "
        "\"" + output_path.string() + "\"";
    system(command.c_str());

    //Write the new values in setting.yml file for next run default values
    FileStorage fsWrite("setting/setting.yml",FileStorage::WRITE);
    fsWrite << "limit" << data.cliplimit;
    fsWrite << "gamma" << data.val;
    fsWrite << "strength" << data.st;
    fsWrite << "center" << data.c;
    fsWrite << "sharp" << data.level;
    fsWrite.release();
    return 0;
}
