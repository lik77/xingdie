#include<opencv2/opencv.hpp>
using namespace std;
using namespace cv;
Mat yvchuli(Mat img0)//预处理
{
    //二值化
    Mat img1;
    threshold(img0,img1,127,255,THRESH_BINARY);

    //高斯滤波
    Mat img2;
    GaussianBlur(img1,img2,Size(9,9),0,0);

    //形态学
    Mat img3;
    Mat element=getStructuringElement(MORPH_RECT,Size(3,3));
    morphologyEx(img2,img3,MORPH_CLOSE,element);

    return img3;
}
vector<vector<Point>> findlunkuo(Mat img_fram1,Mat img_fram)//查找轮廓
{
    vector<vector<Point>>ann;
    vector<Vec4i>hir;
    findContours(img_fram1,ann,hir,RETR_TREE,CHAIN_APPROX_NONE);

    drawContours(img_fram,ann,-1,Scalar(0,0,0),5);   //放个绘制轮廓便于观察

    return ann;
}
vector<Rect> shaixuan(vector<vector<Point>>ann1)//筛选轮廓
{
    //放筛选出的轮廓的矩形
    vector<Rect>ann1_num1;
    for(int i=0;i<ann1.size();i++)
    {
        Rect ann=boundingRect(ann1[i]);

        if(ann.height*ann.width<5){continue;}

        //存入矩形
        ann1_num1.push_back(ann);
    }

    return ann1_num1;
}
void huizhi(vector<Rect> ann1_cope,Mat img_fram)//框出灯
{

    for(int j=0;j<ann1_cope.size();j++)
    {
    rectangle(img_fram,Point(ann1_cope[j].x,ann1_cope[j].y),Point(ann1_cope[j].x+ann1_cope[j].width,ann1_cope[j].y+ann1_cope[j].height),Scalar(0,255,0),3);
    }

}
void sw_and_jd()
{
    Mat cameraMatrix = (Mat_<double>(3, 3) <<976.3268391629416, 0, 126.2400522817966,0, 745.3827832976327, 199.5296404260386,0, 0, 1);//内参矩阵
    Mat distCoeffs = (Mat_<double>(5, 1) << 0.5200687726809431, -0.3777490426637254, -0.045160952365915, -0.1042501407959941, -0.1492688106449521);//畸变系数  

    vector<Point2f> image=vector<Point2f>//像素二维坐标
    {
        Point2f(),
        Point2f(),
        Point2f(),
        Point2f()

    };
    vector<Point3f> obj=vector<Point3f>//灯外接矩形的三维坐标
    {
        Point3f(),
        Point3f(),
        Point3f(),
        Point3f()

    };

    Mat rvec,tvec;
    solvePnP(obj,image,cameraMatrix,distCoeffs,rvec,tvec);
    cout<<"三维"<<rvec<<endl;
   
}
int main()
{
    string number="R";//选择红蓝通道
    VideoCapture cap(2);
    for(;;)
    {
        Mat fram,fram_erzhi;cap>>fram;
        //resize(fram,fram,Size(),2,2);

        //通道相减
        vector<Mat>channels;
        split(fram,channels);

        if(number=="B")//识别蓝色
        {
            Mat img=channels[0]-channels[2];

            //预处理
            fram_erzhi=yvchuli(img);
        }
        else if(number=="R")//识别红色
        {
            Mat img=channels[2]-channels[0];

            //预处理
            fram_erzhi=yvchuli(img);
        }
        else{cout<<"没选择红蓝"<<endl;break;}

        //寻找轮廓
        vector<vector<Point>>ann0;
        ann0=findlunkuo(fram_erzhi,fram);

        //筛选轮廓
        vector<Rect> ann0_Rect=shaixuan(ann0);

        //绘制轮廓
        huizhi(ann0_Rect,fram);

        //图的显示
        imshow("二值图",fram_erzhi);
        imshow("原图",fram);
       
        waitKey(1);
    }    
    return 0;
}