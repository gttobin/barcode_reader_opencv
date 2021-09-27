//============================================================================
// Name        : Barcode.cpp
// Author      : Gerard Tobin
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================
/**
to compile ,run
g++ -mwindows main.cpp -lgdiplus -o barcode
ro run
 ./barcode.exe C:\\Users\\Gerard\\Desktop\\business\\barcodes\\barcode10_mbdb.jpg false

*/

#include <windows.h>
#include <gdiplus.h>
#include <stdio.h>
#include <wchar.h>

#include <string>
#include <iostream>
#include <time.h>
#include <exception>
#include <stdio.h>
#include <math.h>
#include <vector>
#include <algorithm>
#include <list>
#pragma comment (lib,"Gdiplus.lib")
using namespace std;
using namespace Gdiplus;



int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
   UINT  num = 0;          // number of image encoders
   UINT  size = 0;         // size of the image encoder array in bytes

   ImageCodecInfo* pImageCodecInfo = NULL;

   GetImageEncodersSize(&num, &size);
   if(size == 0)
      return -1;  // Failure

   pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
   if(pImageCodecInfo == NULL)
      return -1;  // Failure

   GetImageEncoders(num, size, pImageCodecInfo);

   for(UINT j = 0; j < num; ++j)
   {
      if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
      {
         *pClsid = pImageCodecInfo[j].Clsid;
         free(pImageCodecInfo);
         return j;  // Success
      }
   }

   free(pImageCodecInfo);
   return -1;  // Failure
}




vector<long double> getScanline(string file_name,bool print)
{
	 vector<long double> scanline;

    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    CLSID   encoderClsid;
    Status  stat;
    wstring str1(file_name.begin(), file_name.end());
    Image*   image = new Image(str1.c_str());

    std::string delimiter = ".";
    std::string token = file_name.substr(0, file_name.find(delimiter));
    std::string new_file_name=token+".bmp";
    wstring str2(new_file_name.begin(), new_file_name.end());


    // Get the CLSID of the PNG encoder.
    GetEncoderClsid(L"image/bmp", &encoderClsid);

    stat = image->Save(str2.c_str(), &encoderClsid, NULL);

    Bitmap*    bmp = new Bitmap(str2.c_str() );




    int            w, h;
    w = bmp->GetWidth();
    h = bmp->GetHeight();

    float horizontalScalingFactor = 600.0 / (float)w;
    float verticalScalingFactor = 400.0 / (float)h;
    Bitmap* img = new Bitmap((int)600, (int)400);
    Graphics* g = new Graphics(img);

    g->ScaleTransform(horizontalScalingFactor, verticalScalingFactor);

    g->DrawImage(bmp, 0, 0);

    for( int x=0; x<600; x++ )
    {
            Color c;
            img->GetPixel( x, 400/2,&c );

             double red = c.GetRed();
             double green = c.GetGreen();
             double blue = c.GetBlue();

              //Y (x) = 0.299R(x) + 0.587G(x) + 0.114B(x)
             double intensity=(red*(0.299)+green*(0.587)+blue*(0.114))/255;

             scanline.push_back(intensity);

    }
    if(print)
    {
        if(stat == Ok)
        printf("bmp was saved successfully\n");
        else
        printf("Failure: stat = %d\n", stat);
    }

   delete image;
   GdiplusShutdown(gdiplusToken);

   double sum=0;
   int counter=0;;
   double average=0.0;
   for(unsigned int i=0;i<scanline.size();i++)
   {

   	  if(scanline[i]>0.5)
  	  {
   		  sum=sum+scanline[i];
  		  counter++;
  	  }
   }
   average=sum/counter;
   for(unsigned int i=0;i<scanline.size();i++)
   {
				    	  if(scanline[i]>0.5)
				    	  {
				    	  scanline[i]=scanline[i]+(1-average);
				    	  }
				    	  if(scanline[i]>1.0)
				    	  {
				    		  scanline[i]=1.0;
				    	  }
    }

    if(print)
    {
        printf("\nscanline length %llu",scanline.size());
	}
	return scanline;


}

vector<vector<int>> bars(vector<long double> scanline,long double iDiff,bool print)
{


		 long double  intensityDiff =iDiff;

		 long double diff=0;

		 struct  points_struct
		 {
			 int point;
			 long double intensity;
		 };

		 vector<points_struct> points;
		 if(print)
		 {
		 printf("\npoints");

		 }
		 for(unsigned  int i=0;i<=scanline.size()-2;i++)
		 {

			 diff=diff + scanline[i] -scanline[i+1];

			 if(abs(diff)>=intensityDiff )
			 {

				 points_struct temp;
				 temp.point=i+1;
				 temp.intensity=scanline[i+1];
				 if(print)
				 {
				 printf("( %i   %Lf)",temp.point,	temp.intensity);
				 }
				 points.push_back(temp);
				 diff=0;
			 }
		 }
		 if(print)
		 {
		 printf("\npoints length %llu",points.size());
		 }
		 struct binarizedPoints_struct
		 {
			 int point;
			 long double intensity;
			 bool blackOrWhite;

		 };
		 vector<binarizedPoints_struct> binarizedPoints;
		 if(print)
		 {
		 printf("\nbinarizedpoints 1\n");
		 }
		 for(unsigned  int i=0;i<points.size();i++)
		 {





			 	if(points[i].intensity>=0.5)
			 	{
			 		 binarizedPoints_struct temp1;
			 		temp1.point=points[i].point;
			 	    temp1.intensity=points[i].intensity;
			 		temp1.blackOrWhite=true;
			 		binarizedPoints.push_back(temp1);
			 		 if(print)
			 		 {
			 		  printf(" (%d  %Lf) ",temp1.point,temp1.intensity);
			 		 }
			 	}
			 	else
			 	{ binarizedPoints_struct temp1;
			 		temp1.point=points[i].point;
			 		temp1.intensity=points[i].intensity;
			 		temp1.blackOrWhite=false;
			 		binarizedPoints.push_back(temp1);
			 		 if(print)
			 		 {
			 		  printf(" (%d  %Lf) ",temp1.point,temp1.intensity);
			 		 }
			 	}
		 }
		 if(print)
		 {
		 printf("\nbinarizedPoints 1 length %llu",binarizedPoints.size());
		 printf("\nbinarizedPoints 2");
		 }
		 for(unsigned  int i=0;i<binarizedPoints.size()-1;i++)
		 {

			 if((binarizedPoints[i].blackOrWhite && binarizedPoints[i+1].blackOrWhite)
					 ||
		       (!binarizedPoints[i].blackOrWhite && !binarizedPoints[i+1].blackOrWhite))
			 {
				 binarizedPoints.erase(binarizedPoints.begin()+i);i--;
			 }
			 if(print)
			{
			 printf(" (%d %Lf)",binarizedPoints[i].point,binarizedPoints[i].intensity);
			}
		 }
		 if(print)
		 {
		 printf("\nbinarizedPoints length 2 %llu",binarizedPoints.size());
		 }
		 struct averagePoints_struct
		 {
			 int point;
			 long double intensity;
			 bool operator() (averagePoints_struct i,averagePoints_struct j) { return (i.point<j.point);}
		 }averagePoints_struct_object;

	    vector<averagePoints_struct> averagePoints;

	    for(unsigned  int i=0;i<=round(binarizedPoints.size()/2)-7;i++)
		{

			vector<long double> temp;
			temp.push_back(binarizedPoints[i].intensity);
			temp.push_back(binarizedPoints[i+1].intensity);
			temp.push_back(binarizedPoints[i+2].intensity);
			temp.push_back(binarizedPoints[i+3].intensity);
			temp.push_back(binarizedPoints[i+4].intensity);
			temp.push_back(binarizedPoints[i+5].intensity);
			temp.push_back(binarizedPoints[i+6].intensity);
			sort(temp.begin(),temp.end());

			long double avgy=(temp[1]+temp[5])/2;
			averagePoints_struct temp1;
			temp1.point=binarizedPoints[i+4].point;
			temp1.intensity=avgy;
			averagePoints.push_back(temp1);


		}

	    if(print)
	   	{
	    printf("\naveragePoints \n");
	   	}
	    for(unsigned  int i=binarizedPoints.size();i>=round(binarizedPoints.size()/2)-7;i--)
		{

			vector<long double> temp;
					temp.push_back(binarizedPoints[i].intensity);
					temp.push_back(binarizedPoints[i-1].intensity);
					temp.push_back(binarizedPoints[i-2].intensity);
					temp.push_back(binarizedPoints[i-3].intensity);
					temp.push_back(binarizedPoints[i-4].intensity);
					temp.push_back(binarizedPoints[i-5].intensity);
					temp.push_back(binarizedPoints[i-6].intensity);
					sort(temp.begin(),temp.end());
					long double avgy=(temp[1]+temp[5])/2;
					averagePoints_struct temp1;
					temp1.point=binarizedPoints[i-4].point;

					temp1.intensity=avgy;
					averagePoints.push_back(temp1);


		}


		sort(averagePoints.begin(),averagePoints.end(),averagePoints_struct_object);


		 if(print)
	     {
		for(unsigned  int i=0;i<averagePoints.size();i++)
		{
			printf(" (%d, %Lf )",averagePoints[i].point, averagePoints[i].intensity);
		}


		printf("\naveragePoints   length %llu\n ",averagePoints.size());
	     }
		vector<averagePoints_struct> averagePointsExpanded;

		if(print)
		{
		printf("\naverage points extended 1 \n");
		}
		for(unsigned  int i=0;i<(averagePoints.size());i++)
		{


			for(  int j=averagePoints[i].point;j<averagePoints[i+1].point;j++)
			{

				averagePoints_struct temp2;
				temp2.point=averagePoints[i].point;
				temp2.intensity=averagePoints[i].intensity;
				averagePointsExpanded.push_back(temp2);
				 if(print)
				 {
				 printf("(%d, %Lf) ",temp2.point,averagePoints[i].intensity);
				 }
			}

		}
		 if(print)
		 {
			 printf("\naverage points extended 1 length %llu\n",averagePointsExpanded.size());
			 printf("\naverage points extended 2");
		 }
		for(  int i=0;i<=averagePoints[0].point;i++)
			{
				averagePoints_struct temp;
							temp.point=i;
							temp.intensity=averagePoints[0].intensity;
				averagePointsExpanded.insert(averagePointsExpanded.begin(),temp);
				 if(print)
				{
					 printf(" (%d %Lf) ",temp.point,averagePoints[0].intensity);
				}
			}
		 if(print)
		 {
			 printf("\naverage points extended 2 length %llu\n",averagePointsExpanded.size());
		 }

		 if(print)
		 {
		printf("\naverage points extended 3");
		 }
			for(unsigned int i=averagePoints[averagePoints.size()-1].point+1;i<scanline.size();i++)
		    {
				averagePoints_struct temp;
				temp.point=i;
				temp.intensity=averagePoints[averagePoints.size()-1].intensity;
				averagePointsExpanded.push_back(temp);

		    }

			sort(averagePointsExpanded.begin(),averagePointsExpanded.end(),averagePoints_struct_object);
			 if(print)
			{
			for(unsigned  int i=0;i<averagePointsExpanded.size();i++)
			{
				printf(" (%d %Lf) ",averagePointsExpanded[i].point,averagePointsExpanded[i].intensity);

			}
			printf("\naveragePointsExpanded 3 length %llu",averagePointsExpanded.size());

		 printf("\n binarizedPoints2 ");
			}
	    vector<int> binarizedPoints2;
	    for(unsigned  int i=0;i<averagePointsExpanded.size();i++)
	    {
	    	if(scanline[i]>=averagePointsExpanded[i].intensity)
	    	{
	    		binarizedPoints2.push_back(0);
	    	}
	    	else
	    	{
	    		binarizedPoints2.push_back(1);
	    	}
	    }

	    if(print)
	   	{
	    for(unsigned  int i=0;i<binarizedPoints2.size();i++)
	    {
	    	printf("( %d %d )",i,binarizedPoints2[i]);
	    }
	    printf("\n binarizedPoints2 length %llu\n",binarizedPoints2.size());
	   	}
	    vector<vector<int>> bars;
	    int l=binarizedPoints2.size();

	    int i=0;
	    vector<int> templist0;
	    vector<int> templist1;
	    vector<int> templist;

	    while(i<=l)
	    {
	    	label0:
			if(binarizedPoints2[i]==0)
			{
				templist0.push_back(0);


				i++;
				goto label0;
			}
			else
			{
				bars.push_back(templist0);
				templist0.clear();
			}






			if(bars.size()>=60)
			{
				break;
			}
			label1:

			if(binarizedPoints2[i]==1)
			{
				templist1.push_back(1);

				i++;
				goto label1;
			}
			else
			{
				bars.push_back(templist1);
				templist1.clear();
			}




			if(bars.size()>=60)
			{
						break;
			}

	    }
	    if(print)
	   	{
	    printf("\n bars length 1 %llu",bars.size());
	   	}
	  for(unsigned  int i=l;i>0;i--)
	  {
		  if(binarizedPoints2[l]==binarizedPoints2[i])
		  {
			  templist.push_back(binarizedPoints2[l]);

		  }
		  else
		  {
			  break;
		  }
	  }
	  bars.push_back(templist);





	  return bars;
}

vector<int> leftPadding(vector<vector<int>> bars)
{

		return	bars.at(0);
}

vector<int> rightPadding(vector<vector<int>> bars)
{

		return	bars.at(60);
}

vector<vector<int>> leftGuard(vector<vector<int>> bars)
{

		vector<vector<int>> leftGuard;


		for(unsigned  int i=0;i<3;i++)
		{
		leftGuard.push_back(bars.at(i+1));
		}

		return	leftGuard;
}

vector<vector<int>> rightGuard(vector<vector<int>> bars)
{
			vector<vector<int>> rightGuard;


			for(unsigned int i=0;i<3;i++)
			{
			rightGuard.push_back(bars.at(i+57));
			}

			return	rightGuard;



}

vector<vector<int>> midGuard(vector<vector<int>> bars)
{
		vector<vector<int>> midGuard;


		for(unsigned int i=0;i<5;i++)
		{
			midGuard.push_back(bars.at(i+28));
		}

		return	midGuard;

}
vector<vector<vector<int>>> lhsAndRhsBars(vector<vector<int>> bars)
{
		vector<vector<vector<int>>> lhsAndRhsBars;
		vector<vector<int>> temp;

		for(unsigned int j=0;j<6;j++)
		{
			temp.clear();
		for(unsigned int i=0;i<4;i++)
		{
			temp.push_back(bars.at(i+j*4+4));
		}
			lhsAndRhsBars.push_back(temp);
		}

		for(unsigned int j=0;j<6;j++)
		{
			temp.clear();
		for(unsigned  int i=0;i<4;i++)
		{
			temp.push_back(bars.at(i+j*4+33));
		}
			lhsAndRhsBars.push_back(temp);
		}
		return	lhsAndRhsBars;

}

vector<vector<vector<int>>> lhsBars(vector<vector<int>> bars)
{
		vector<vector<vector<int>>> lhsBars;
		vector<vector<int>> temp;

		for(unsigned int j=0;j<6;j++)
		{
			temp.clear();
		for(unsigned int i=0;i<4;i++)
		{
			temp.push_back(bars.at(i+j*4+4));
		}
			lhsBars.push_back(temp);
		}


		return	lhsBars;

}

vector<vector<vector<int>>> rhsBars(vector<vector<int>> bars)
{
		vector<vector<vector<int>>> rhsBars;
		vector<vector<int>> temp;



		for(unsigned int j=0;j<6;j++)
		{
			temp.clear();
		for(unsigned  int i=0;i<4;i++)
		{
			temp.push_back(bars.at(i+j*4+33));
		}
			rhsBars.push_back(temp);
		}
		return	rhsBars;

}

double averageBarLength1(vector<vector<int>> bars)
{
		 vector<vector<int>> lGuard=leftGuard(bars);
		 vector<vector<int>> rGuard=rightGuard(bars);
		 vector<vector<int>> mGuard=midGuard(bars);
		 double avgBarLength1=0;

		avgBarLength1=(lGuard[0].size()+lGuard[2].size()+rGuard[0].size()+rGuard[2].size()+
				mGuard[1].size()+mGuard[3].size())/6;

		 return avgBarLength1;
}

double averageBarLength0(vector<vector<int>> bars)
{
	 	 	 vector<vector<int>> lGuard=leftGuard(bars);
			 vector<vector<int>> rGuard=rightGuard(bars);
			 vector<vector<int>> mGuard=midGuard(bars);
			 double avgBarLength0=0;

			avgBarLength0=(lGuard[1].size()+rGuard[1].size()+rGuard[2].size()+
					mGuard[0].size()+mGuard[2].size()+mGuard[4].size())/5;

			 return avgBarLength0;



}
long double maxDist_lhs(int j,long double avbl1,long double avbl0,vector<vector<vector<int>>> bars_lhs)
{
	long double avgBarLength0 = avbl0;
	long double  avgBarLength1 = avbl1;
	vector<vector<vector<int>>>  s = bars_lhs;

	vector<long double> r0L = {avgBarLength0*3, avgBarLength1*2, avgBarLength0,
	    avgBarLength1};
	vector<long double>  r0G = {avgBarLength0, avgBarLength1, avgBarLength0*2,
	    avgBarLength1*3};

	vector<long double>  r1L = {avgBarLength0*2, avgBarLength1*2, avgBarLength0*2,
	    avgBarLength1};
	vector<long double>  r1G = {avgBarLength0, avgBarLength1*2, avgBarLength0*2,
	    avgBarLength1*2};

	vector<long double>  r2L = {avgBarLength0*2, avgBarLength1, avgBarLength0*2,
	    avgBarLength1*2};
	vector<long double>  r2G = {avgBarLength0*2, avgBarLength1*2, avgBarLength0,
	    avgBarLength1*2};

	vector<long double>  r3L = {avgBarLength0, avgBarLength1*3, avgBarLength0, avgBarLength1};
	vector<long double>  r3G = {avgBarLength0, avgBarLength1, avgBarLength0*4, avgBarLength1};

	vector<long double>  r4L = {avgBarLength0, avgBarLength1, avgBarLength0*3,
	    avgBarLength1*2};
	vector<long double>  r4G = {avgBarLength0*2, avgBarLength1*3, avgBarLength0,
	    avgBarLength1};

	vector<long double>  r5L = {avgBarLength0, avgBarLength1*2, avgBarLength0*3,
	    avgBarLength1};
	vector<long double>  r5G = {avgBarLength0, avgBarLength1*3, avgBarLength0*2,
	    avgBarLength1};

	vector<long double>  r6L = {avgBarLength0, avgBarLength1, avgBarLength0, avgBarLength1*4};
	vector<long double>  r6G = {avgBarLength0*4, avgBarLength1, avgBarLength0, avgBarLength1};

	vector<long double>  r7L = {avgBarLength0, avgBarLength1*3, avgBarLength0,
	    avgBarLength1*2};
	vector<long double> r7G = {avgBarLength0*2, avgBarLength1, avgBarLength0*3,
	    avgBarLength1};

	vector<long double>  r8L = {avgBarLength0, avgBarLength1*2, avgBarLength0,
	    avgBarLength1*3};
	vector<long double>  r8G = {avgBarLength0*3, avgBarLength1, avgBarLength0*2,
	    avgBarLength1};

	vector<long double>  r9L = {avgBarLength0*3, avgBarLength1, avgBarLength0,
	    avgBarLength1*2};
	vector<long double>  r9G = {avgBarLength0*2, avgBarLength1, avgBarLength0,
	    avgBarLength1*3};



	  vector<vector<long double>>  r = {r0L, r0G, r1L,r1G, r2L, r2G, r3L, r3G,
	    r4L, r4G, r5L, r5G,  r6L, r6G, r7L, r7G, r8L,
	    r8G,  r9L, r9G};

	  long double total=0;

	  for(unsigned  int i=0;i<20;i++)
	  {
		  long double max=0;
		  max = pow(r.at(i).at(0)  - s.at(j).at(0).size(),2) +
				pow(r.at(i).at(1)  - s.at(j).at(1).size(),2) +
				pow(r.at(i).at(2)  - s.at(j).at(2).size(),2) +
				pow(r.at(i).at(3)  - s.at(j).at(3).size(),2);

		  if(max>=total)
		  {
			  total=max;
		  }
	  }
	return total;
}

long double maxDist_rhs(int j,long double avbl1,long double avbl0,vector<vector<vector<int>>> bars_rhs)
{
	long double avgBarLength0 = avbl0;
	long double  avgBarLength1 = avbl1;
	vector<vector<vector<int>>>  s = bars_rhs;


	vector<long double> r0R = {avgBarLength1*3, avgBarLength0*2, avgBarLength1,
	    avgBarLength0};

	vector<long double>  r1R = {avgBarLength1*2, avgBarLength0*2, avgBarLength1*2,
	    avgBarLength0};

	vector<long double>  r2R = {avgBarLength1*2, avgBarLength0, avgBarLength1*2,
	    avgBarLength0*2};

	vector<long double>  r3R = {avgBarLength1, avgBarLength0*3, avgBarLength1, avgBarLength0};

	vector<long double>  r4R = {avgBarLength1, avgBarLength0, avgBarLength1*3,
	    avgBarLength0*2};

	vector<long double>  r5R = {avgBarLength1, avgBarLength0*2, avgBarLength1*3,
	    avgBarLength0};

	vector<long double>  r6R = {avgBarLength1, avgBarLength0, avgBarLength1, avgBarLength0*4};

	vector<long double>  r7R = {avgBarLength1, avgBarLength0*3, avgBarLength1,
	    avgBarLength0*2};

	vector<long double>  r8R = {avgBarLength1, avgBarLength0*2, avgBarLength1,
	    avgBarLength0*3};

	vector<long double>  r9R = {avgBarLength1*3, avgBarLength0, avgBarLength1,
	    avgBarLength0*2};


	  vector<vector<long double>>  r = { r0R, r1R, r2R, r3R,
	     r4R,  r5R,  r6R, r7R, r8R,  r9R};

	  long double total=0;

	  for(unsigned  int i=0;i<10;i++)
	  {
		  long double max=0;
		  max = pow(r.at(i).at(0)  - s.at(j).at(0).size(),2) +
				pow(r.at(i).at(1)  - s.at(j).at(1).size(),2) +
				pow(r.at(i).at(2)  - s.at(j).at(2).size(),2) +
				pow(r.at(i).at(3)  - s.at(j).at(3).size(),2);

		  if(max>=total)
		  {
			  total=max;
		  }
	  }
	return total;
}

long double pdash_lhs(int i,int j,long double avbl1,long double avbl0,vector<vector<vector<int>>> bars_lhs)
{
	long double avgBarLength0 = avbl0;
	long double  avgBarLength1 = avbl1;
	vector<vector<vector<int>>>  s = bars_lhs;

	vector<long double> r0L = {avgBarLength0*3, avgBarLength1*2, avgBarLength0,
		    avgBarLength1};
		vector<long double>  r0G = {avgBarLength0, avgBarLength1, avgBarLength0*2,
		    avgBarLength1*3};

		vector<long double>  r1L = {avgBarLength0*2, avgBarLength1*2, avgBarLength0*2,
		    avgBarLength1};
		vector<long double>  r1G = {avgBarLength0, avgBarLength1*2, avgBarLength0*2,
		    avgBarLength1*2};

		vector<long double>  r2L = {avgBarLength0*2, avgBarLength1, avgBarLength0*2,
		    avgBarLength1*2};
		vector<long double>  r2G = {avgBarLength0*2, avgBarLength1*2, avgBarLength0,
		    avgBarLength1*2};

		vector<long double>  r3L = {avgBarLength0, avgBarLength1*3, avgBarLength0, avgBarLength1};
		vector<long double>  r3G = {avgBarLength0, avgBarLength1, avgBarLength0*4, avgBarLength1};

		vector<long double>  r4L = {avgBarLength0, avgBarLength1, avgBarLength0*3,
		    avgBarLength1*2};
		vector<long double>  r4G = {avgBarLength0*2, avgBarLength1*3, avgBarLength0,
		    avgBarLength1};

		vector<long double>  r5L = {avgBarLength0, avgBarLength1*2, avgBarLength0*3,
		    avgBarLength1};
		vector<long double>  r5G = {avgBarLength0, avgBarLength1*3, avgBarLength0*2,
		    avgBarLength1};

		vector<long double>  r6L = {avgBarLength0, avgBarLength1, avgBarLength0, avgBarLength1*4};
		vector<long double>  r6G = {avgBarLength0*4, avgBarLength1, avgBarLength0, avgBarLength1};

		vector<long double>  r7L = {avgBarLength0, avgBarLength1*3, avgBarLength0,
		    avgBarLength1*2};
		vector<long double> r7G = {avgBarLength0*2, avgBarLength1, avgBarLength0*3,
		    avgBarLength1};

		vector<long double>  r8L = {avgBarLength0, avgBarLength1*2, avgBarLength0,
		    avgBarLength1*3};
		vector<long double>  r8G = {avgBarLength0*3, avgBarLength1, avgBarLength0*2,
		    avgBarLength1};

		vector<long double>  r9L = {avgBarLength0*3, avgBarLength1, avgBarLength0,
		    avgBarLength1*2};
		vector<long double>  r9G = {avgBarLength0*2, avgBarLength1, avgBarLength0,
		    avgBarLength1*3};



		  vector<vector<long double>>  r = {r0L, r0G,r1L, r1G, r2L, r2G, r3L, r3G,
		    r4L, r4G, r5L, r5G,  r6L, r6G, r7L, r7G, r8L,
		    r8G,  r9L, r9G};


	  long double sum=0;
	  sum = pow(r.at(i).at(0)  - s.at(j).at(0).size(),2) +
	 				pow(r.at(i).at(1)  - s.at(j).at(1).size(),2) +
	 				pow(r.at(i).at(2)  - s.at(j).at(2).size(),2) +
	 				pow(r.at(i).at(3)  - s.at(j).at(3).size(),2);

	  long double total=0.0;
	  total = 1 - (sum/maxDist_lhs(j, avgBarLength1, avgBarLength0, s));
	  return total;
}

long double pdash_rhs(int i,int j,long double avbl1,long double avbl0,vector<vector<vector<int>>> bars_rhs)
{
	long double avgBarLength0 = avbl0;
	long double  avgBarLength1 = avbl1;
	vector<vector<vector<int>>>  s = bars_rhs;


	vector<long double> r0R = {avgBarLength1*3, avgBarLength0*2, avgBarLength1,
	    avgBarLength0};

	vector<long double>  r1R = {avgBarLength1*2, avgBarLength0*2, avgBarLength1*2,
	    avgBarLength0};

	vector<long double>  r2R = {avgBarLength1*2, avgBarLength0, avgBarLength1*2,
	    avgBarLength0*2};

	vector<long double>  r3R = {avgBarLength1, avgBarLength0*3, avgBarLength1, avgBarLength0};

	vector<long double>  r4R = {avgBarLength1, avgBarLength0, avgBarLength1*3,
	    avgBarLength0*2};

	vector<long double>  r5R = {avgBarLength1, avgBarLength0*2, avgBarLength1*3,
	    avgBarLength0};

	vector<long double>  r6R = {avgBarLength1, avgBarLength0, avgBarLength1, avgBarLength0*4};

	vector<long double>  r7R = {avgBarLength1, avgBarLength0*3, avgBarLength1,
	    avgBarLength0*2};

	vector<long double>  r8R = {avgBarLength1, avgBarLength0*2, avgBarLength1,
	    avgBarLength0*3};

	vector<long double>  r9R = {avgBarLength1*3, avgBarLength0, avgBarLength1,
	    avgBarLength0*2};


	  vector<vector<long double>>  r = { r0R, r1R, r2R, r3R,
	     r4R,  r5R,  r6R, r7R, r8R,  r9R};


	  long double sum=0;
	  sum = pow(r.at(i).at(0)  - s.at(j).at(0).size(),2) +
	 				pow(r.at(i).at(1)  - s.at(j).at(1).size(),2) +
	 				pow(r.at(i).at(2)  - s.at(j).at(2).size(),2) +
	 				pow(r.at(i).at(3)  - s.at(j).at(3).size(),2);

	  long double total=0.0;
	  total = 1 - (sum/maxDist_rhs(j, avgBarLength1, avgBarLength0, s));
	  return total;
}
long double p_lhs(int i,int j,double avbl1,double avbl0,vector<vector<vector<int>>> bars_lhs)
{
	long double avgBarLength0 = avbl0;
	long double  avgBarLength1 = avbl1;
	vector<vector<vector<int>>>  s = bars_lhs;

	vector<long double> r0L = {avgBarLength0*3, avgBarLength1*2, avgBarLength0,
			    avgBarLength1};
			vector<long double>  r0G = {avgBarLength0, avgBarLength1, avgBarLength0*2,
			    avgBarLength1*3};

			vector<long double>  r1L = {avgBarLength0*2, avgBarLength1*2, avgBarLength0*2,
			    avgBarLength1};
			vector<long double>  r1G = {avgBarLength0, avgBarLength1*2, avgBarLength0*2,
			    avgBarLength1*2};

			vector<long double>  r2L = {avgBarLength0*2, avgBarLength1, avgBarLength0*2,
			    avgBarLength1*2};
			vector<long double>  r2G = {avgBarLength0*2, avgBarLength1*2, avgBarLength0,
			    avgBarLength1*2};

			vector<long double>  r3L = {avgBarLength0, avgBarLength1*3, avgBarLength0, avgBarLength1};
			vector<long double>  r3G = {avgBarLength0, avgBarLength1, avgBarLength0*4, avgBarLength1};

			vector<long double>  r4L = {avgBarLength0, avgBarLength1, avgBarLength0*3,
			    avgBarLength1*2};
			vector<long double>  r4G = {avgBarLength0*2, avgBarLength1*3, avgBarLength0,
			    avgBarLength1};

			vector<long double>  r5L = {avgBarLength0, avgBarLength1*2, avgBarLength0*3,
			    avgBarLength1};
			vector<long double>  r5G = {avgBarLength0, avgBarLength1*3, avgBarLength0*2,
			    avgBarLength1};

			vector<long double>  r6L = {avgBarLength0, avgBarLength1, avgBarLength0, avgBarLength1*4};
			vector<long double>  r6G = {avgBarLength0*4, avgBarLength1, avgBarLength0, avgBarLength1};

			vector<long double>  r7L = {avgBarLength0, avgBarLength1*3, avgBarLength0,
			    avgBarLength1*2};
			vector<long double> r7G = {avgBarLength0*2, avgBarLength1, avgBarLength0*3,
			    avgBarLength1};

			vector<long double>  r8L = {avgBarLength0, avgBarLength1*2, avgBarLength0,
			    avgBarLength1*3};
			vector<long double>  r8G = {avgBarLength0*3, avgBarLength1, avgBarLength0*2,
			    avgBarLength1};

			vector<long double>  r9L = {avgBarLength0*3, avgBarLength1, avgBarLength0,
			    avgBarLength1*2};
			vector<long double>  r9G = {avgBarLength0*2, avgBarLength1, avgBarLength0,
			    avgBarLength1*3};



			  vector<vector<long double>>  r = {r0L, r0G,r1L, r1G, r2L, r2G, r3L, r3G,
			    r4L, r4G, r5L, r5G,  r6L, r6G, r7L, r7G, r8L,
			    r8G,  r9L, r9G};



	  long double total = 0;
	  for(unsigned int k = 0; k < 20; k++)
	  {
		  total = total + pdash_lhs(k, j, avgBarLength1, avgBarLength0, s);
	  }

	  long double  ret = pdash_lhs(i, j, avgBarLength1, avgBarLength0, s)/total;
	  return ret;



}


long double p_rhs(int i,int j,double avbl1,double avbl0,vector<vector<vector<int>>> bars_rhs)
{
	long double avgBarLength0 = avbl0;
	long double  avgBarLength1 = avbl1;
	vector<vector<vector<int>>>  s = bars_rhs;

	vector<long double> r0R = {avgBarLength1*3, avgBarLength0*2, avgBarLength1,
		    avgBarLength0};

		vector<long double>  r1R = {avgBarLength1*2, avgBarLength0*2, avgBarLength1*2,
		    avgBarLength0};

		vector<long double>  r2R = {avgBarLength1*2, avgBarLength0, avgBarLength1*2,
		    avgBarLength0*2};

		vector<long double>  r3R = {avgBarLength1, avgBarLength0*3, avgBarLength1, avgBarLength0};

		vector<long double>  r4R = {avgBarLength1, avgBarLength0, avgBarLength1*3,
		    avgBarLength0*2};

		vector<long double>  r5R = {avgBarLength1, avgBarLength0*2, avgBarLength1*3,
		    avgBarLength0};

		vector<long double>  r6R = {avgBarLength1, avgBarLength0, avgBarLength1, avgBarLength0*4};

		vector<long double>  r7R = {avgBarLength1, avgBarLength0*3, avgBarLength1,
		    avgBarLength0*2};

		vector<long double>  r8R = {avgBarLength1, avgBarLength0*2, avgBarLength1,
		    avgBarLength0*3};

		vector<long double>  r9R = {avgBarLength1*3, avgBarLength0, avgBarLength1,
		    avgBarLength0*2};


		  vector<vector<long double>>  r = { r0R, r1R, r2R, r3R,
		     r4R,  r5R,  r6R, r7R, r8R,  r9R};

	  long double total = 0;
	  for(unsigned int k = 0; k < 10; k++)
	  {
		  total = total + pdash_rhs(k, j, avgBarLength1, avgBarLength0, s);
	  }

	  long double  ret = pdash_rhs(i, j, avgBarLength1, avgBarLength0, s)/total;
	  return ret;



}
bool checkSum(vector<int> bc)
{
	vector<int> finalBarcode=bc;
	int evenSum=finalBarcode[10] + finalBarcode[8] + finalBarcode[6] +
		    finalBarcode[4] + finalBarcode[2] + finalBarcode[0];
	int oddSum=finalBarcode[11] + finalBarcode[9] + finalBarcode[7] +
		    finalBarcode[5] + finalBarcode[3] + finalBarcode[1];

	int	  checkSumDigit = (10 - (3*oddSum + evenSum)%10)%10;
	if(finalBarcode[12]==checkSumDigit)
	{ return true;}
	else
	{return false;}
}


void barcode(long double st,vector<vector<int>> bars,long double avbl1,long double avbl0,long double d)
{

	  long double delta = d;
	  long double step = st;

	  long double avgBarLength0 = avbl0;
	  long double  avgBarLength1 = avbl1;
	 vector<vector<int>>  s = bars;
	 vector<vector<vector<int>>>  s_lhs;
	 vector<vector<vector<int>>>  s_rhs;

	 s_lhs=lhsBars(s);

	 s_rhs=rhsBars(s);



	  vector<long double> maxProbabilities_lhs = {0, 0, 0, 0, 0, 0};
	  vector<int> maxProbabilitiesDigits_lhs = {0, 0, 0, 0, 0, 0};
	  vector<int> maxProbabilitiesIndices_lhs = {0, 0, 0, 0, 0, 0};
	  int maxProbIndex_lhs = 0;
	  long double maxProb_lhs=0;
	  vector<int> maxProbabilitiesDigits2_lhs = {0, 0, 0, 0, 0, 0};
	  vector<int> maxProbabilitiesIndices2_lhs = {0, 0, 0, 0, 0, 0};
	  vector<long double> maxProbabilities2_lhs = {0, 0, 0, 0, 0, 0};
	  vector<long double> finalProbList_lhs = {};
	  vector<double> parity_lhs = {};



	  vector<long double> maxProbabilities_rhs = {0, 0, 0, 0, 0, 0};
	  vector<int> maxProbabilitiesDigits_rhs = {0, 0, 0, 0, 0, 0};
	  vector<int> maxProbabilitiesIndices_rhs = {0, 0, 0, 0, 0, 0};
	  int maxProbIndex_rhs = 0;
	  long double maxProb_rhs=0;
	  vector<int> maxProbabilitiesDigits2_rhs = {0, 0, 0, 0, 0, 0};
	  vector<int> maxProbabilitiesIndices2_rhs = {0, 0, 0, 0, 0, 0};
	  vector<long double> maxProbabilities2_rhs = {0, 0, 0, 0, 0, 0,};
	  vector<long double> finalProbList_rhs = {};
	  vector<double> parity_rhs = {};
	  vector<int> finalbarCode={};


	  long double sigma = 1;
	  int digitNumber = -1;


	 for(long double c = (avgBarLength1 - delta); c <= (avgBarLength1 + delta); c = c + step)
	  {
	     for(long double e = (avgBarLength0 - delta); e <= (avgBarLength0 + delta); e = e + step)
		{


	      vector<long double> r0L ={c*3, e*2, c, e}; ;
	      vector<long double> r0G = {c, e, c*2, e*3};

	      vector<long double> r1L = {c*2, e*2, c*2, e};
	      vector<long double> r1G = {c, e*2, c*2, e*2};

	      vector<long double> r2L = {c*2, e, c*2, e*2};
	      vector<long double> r2G = {c*2, e*2, c, e*2};

	      vector<long double> r3L = {c, e*3, c, e};
	      vector<long double> r3G = {c, e, c*4, e};

	      vector<long double> r4L = {c, e, c*3, e*2};
	      vector<long double> r4G = {c*2, e*3, c, e};

	      vector<long double> r5L = {c, e*2, c*3, e};
	      vector<long double> r5G = {c, e*3, c*2, e};

	      vector<long double> r6L = {c, e, c, e*4};
	      vector<long double> r6G = {c*4, e, c, e};

	      vector<long double> r7L = {c, e*3, c, e*2};
	      vector<long double> r7G = {c*2, e, c*3, e};

	      vector<long double> r8L = {c, e*2, c, e*3};
	      vector<long double> r8G = {c*3, e, c*2, e};

	      vector<long double> r9L = {c*3, e, c, e*2};
	      vector<long double> r9G = {c*2, e, c, e*3};


	      vector<vector<long double>> r = {r0L, r0G, r1L, r1G,  r2L, r2G,  r3L, r3G,
	        r4L, r4G,  r5L, r5G,  r6L, r6G,  r7L, r7G, r8L,
	         r8G,  r9L, r9G};





	      for(int a = 0; a < 6; a++)
	      {

	       maxProb_lhs = 0;
	       maxProbIndex_lhs = 0;

	       for(int b = 0; b < 20; b++)
	       {

	        if(p_lhs(b, a, c, e, s_lhs) >= maxProb_lhs)
	        {
	        	maxProb_lhs = p_lhs(b, a, c, e, s_lhs);

	            maxProbIndex_lhs = b;
	        }

	       }


	       sigma = sigma*maxProb_lhs;





	       digitNumber = ((maxProbIndex_lhs - maxProbIndex_lhs%2)/2);




	       if(maxProb_lhs >= maxProbabilities_lhs[a])
	       {
	        maxProbabilities_lhs[a] = maxProb_lhs;
	        maxProbabilitiesDigits_lhs[a] = digitNumber;
	        maxProbabilitiesIndices_lhs[a] = maxProbIndex_lhs;
	       }



	       }



	      for(unsigned  int i = 0; i < 6; i++)
	      {
	       if(maxProbabilities_lhs[i] >= maxProbabilities2_lhs[i])
	       {
	        maxProbabilities2_lhs[i] = maxProbabilities_lhs[i];
	        maxProbabilitiesDigits2_lhs[i] = maxProbabilitiesDigits_lhs[i];
	        maxProbabilitiesIndices2_lhs[i] = maxProbabilitiesIndices_lhs[i];
	       }

	      }




		 }}



	 	 	 sigma=1;
	 for(long double c = (avgBarLength1 - delta); c <= (avgBarLength1 + delta); c = c + step)
		  {
		     for(long double e = (avgBarLength0 - delta); e <= (avgBarLength0 + delta); e = e + step)
			{



		      vector<long double> r0R = {e*3, c*2, e, c};

		      vector<long double> r1R = {e*2, c*2, e*2, c};

		      vector<long double> r2R = {e*2, c, e*2, c*2};

		      vector<long double> r3R = {e, c*3, e, c};

		      vector<long double> r4R = {e, c, e*3, c*2};

		      vector<long double> r5R = {e, c*2, e*3, c};

		      vector<long double> r6R = {e, c, e, c*4};

		      vector<long double> r7R = {e, c*3, e, c*2};

		      vector<long double> r8R = {e, c*2, e, c*3};

		      vector<long double> r9R = {e*3, c, e, c*2};

		      vector<vector<long double>> r = { r0R, r1R,  r2R,  r3R,
		         r4R,  r5R,  r6R,  r7R,  r8R,  r9R};





		      for(int a = 0; a < 6; a++)
		      {

		       maxProb_rhs = 0;
		       maxProbIndex_rhs = 0;

		       for(int b = 0; b < 10; b++)
		       {

		        if(p_rhs(b, a, c, e, s_rhs) >= maxProb_rhs)
		        {
		        	maxProb_rhs = p_rhs(b, a, c, e, s_rhs);

		            maxProbIndex_rhs = b;
		        }

		       }


		       sigma = sigma*maxProb_rhs;





		       digitNumber = maxProbIndex_rhs;



		       if(maxProb_rhs >= maxProbabilities_rhs[a])
		       {
		        maxProbabilities_rhs[a] = maxProb_rhs;
		        maxProbabilitiesDigits_rhs[a] = digitNumber;
		        maxProbabilitiesIndices_rhs[a] = maxProbIndex_rhs;
		       }



		       }



		      for(unsigned  int i = 0; i < 6; i++)
		      {
		       if(maxProbabilities_rhs[i] >= maxProbabilities2_rhs[i])
		       {
		        maxProbabilities2_rhs[i] = maxProbabilities_rhs[i];
		        maxProbabilitiesDigits2_rhs[i] = maxProbabilitiesDigits_rhs[i];
		        maxProbabilitiesIndices2_rhs[i] = maxProbabilitiesIndices_rhs[i];
		       }

		      }




	//		 }}


	     vector<int> finalBarCode = {};
	     for(unsigned  int i = 0; i < 6; i++)
	     {
	        finalBarCode.push_back(maxProbabilitiesDigits_lhs[i]);
	     }
	     for(unsigned  int i = 0; i < 6; i++)
	     {
	    	  finalBarCode.push_back(maxProbabilitiesDigits_rhs[i]);
	     }


	     for(unsigned int i = 0; i < 6; i++)
	     {
	        if((maxProbabilitiesIndices_lhs[i])%2 == 0)
	        {
	          parity_lhs.push_back(1);

	        }
	        if((maxProbabilitiesIndices_lhs[i])%2 == 1)
	        {
	        	parity_lhs.push_back(0);

	        }


	     }


	       vector<vector<int>> m = {{1, 1, 1, 1, 1, 1}, {1, 1, 0, 1, 0, 0}, {1, 1, 0, 0, 1, 0}, {1,
	           1, 0, 0, 0, 1}, {1, 0, 1, 1, 0, 0}, {1, 0, 0, 1, 1, 0}, {1, 0,
	          0, 0, 1, 1}, {1, 0, 1, 0, 1, 0}, {1, 0, 1, 0, 0, 1}, {1, 0, 0, 1,
	           0, 1}};

	       int mNumber = -1;

	       for(int i = 0; i < 10; i++)
	       {
	         if(m.at(i).at(0) == parity_lhs.at(0) &&
	            m.at(i).at(1) == parity_lhs.at(1) &&
				 m.at(i).at(2) == parity_lhs.at(2) &&
				 m.at(i).at(3) == parity_lhs.at(3) &&
				 m.at(i).at(4) == parity_lhs.at(4) &&
				 m.at(i).at(5) == parity_lhs.at(5) )
	         {
	        	 mNumber = i ; break;
	         }
	       }
	       finalBarCode.insert(finalBarCode.begin() , mNumber);


	       printf("\nfinal Bar Code   ");
	       for(unsigned int i=0;i<13;i++)
	        {
	    	   printf(" %d ",finalBarCode[i]);
	        }

	    	printf(checkSum(finalBarCode) ? "true" : "false");










}}
}




int main(int argc, char *argv[])
{





	string file_name=argv[1];

	bool debug=false;
    if(strcmp(argv[2],"true")==0)
    {
        debug=true;
    }
	 vector<long double> scanline=getScanline(file_name,debug);


	 clock_t start = clock();


	 vector<vector<int>> Bars=bars(scanline,0.1,debug);


	 vector<int> lPadding=leftPadding(Bars);
	 vector<int> rPadding=rightPadding(Bars);
	 vector<vector<int>> lGuard=leftGuard(Bars);
	 vector<vector<int>> rGuard=rightGuard(Bars);
	 vector<vector<int>> mGuard=midGuard(Bars);
	 vector<vector<vector<int>>> lAndRBars=lhsAndRhsBars(Bars);
	 vector<vector<vector<int>>> lBars=lhsBars(Bars);
	 vector<vector<vector<int>>> rBars=rhsBars(Bars);
	 double avbl1=averageBarLength1(Bars);
	 double avbl0=averageBarLength0(Bars);

	 if(debug)
     {


	 printf("\naverage bar length 0 %f\naverage bar length 1 %f",avbl0,avbl1);

	 printf("\nleft padding %llu right padding %llu",lPadding.size(),rPadding.size());

	 printf("\nleft guard");
	 for(unsigned int i=0;i<lGuard.size();i++)
	 {
		 printf("\n");
		 for(unsigned int j=0;j<lGuard.at(i).size();j++)
		 {
		 printf("%d",lGuard.at(i).at(j));
		 }
	 }
	 printf("\nright guard");
	 	 for(unsigned  int i=0;i<rGuard.size();i++)
	 	 {
	 		 printf("\n");
	 		 for(unsigned int j=0;j<rGuard.at(i).size();j++)
	 		 {
	 		 printf("%d",rGuard.at(i).at(j));
	 		 }
	 	 }
	 	 printf("\nmid guard");
	 		 	 for(unsigned int i=0;i<mGuard.size();i++)
	 		 	 {
	 		 		 printf("\n");
	 		 		 for(unsigned int j=0;j<mGuard.at(i).size();j++)
	 		 		 {
	 		 		 printf("%d",mGuard.at(i).at(j));
	 		 		 }
	 		 	 }




     }

	  barcode(0.1,Bars,avbl1,avbl0,0);

	  clock_t end = clock();
	  double time = (double) (end-start);
	  printf("\ntime %f",time/1000);





		      	    return 0;
}

