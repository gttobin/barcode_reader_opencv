# barcode_reader_opencv
This is an implementation of the algorithm described in the paper "Robust Recognition of 1-D Barcodes Using Camera Phones Steffen Wachenfeld, Sebastian Terlunen, Xiaoyi Jiang Computer Vision and Pattern Recognition Group, Department of Computer Science, University of Munster, Germany"

This uses gdi plus libraries for the initial processing of the input jpg files.

To compile, you will have to include opencv 450 c++ libraries

to run

./barcode.exe barcode1.jpg false

the first parameter is the file path of the jpeg image and the second parameter turns debug output on or off

Ten sample images have been uploaded, of which all except number nine are correctly read.

vector<long double> getScanline(string file_name,bool print)

This takes the a JPEG file path, converts the JPEG to a BMP file of size 600 x 400 pixels and extracts the RGB data matrix.
It returns the middle horizontal line of the image's intensity Y(x) according to the formula,
Y (x) = 0.299Red(x) + 0.587Green(x) + 0.114Blue(x)
If the second parameter is set to true, it will output various debug statements to the console.

vector<vector<int>> bars(vector<long double> scanline,long double iDiff,bool print)
This takes the scanline from the getScanline function and calculates the bars. The parameter iDiff is the difference between the intensity values of sequential points of the scanline. If the iDiff value of two sequential points  is less than iDiff, the next value of the scanline is examined until the difference is greater thsn iDiff and the points are reagrded as one bar.
If the print parameter is set to true, various debug statements are outputted to the console. 

vector<int> leftPadding(vector<vector<int>> bars)
This takes the bars calculated by the bars function and return the padding to the left of the barcode i.e. the space before the barcode begins.

vector<int> rightPadding(vector<vector<int>> bars)
This takes the bars calculated by the bars function and returns the padding to the right of the barcode i.e. the space after the barcode ends.

vector<vector<int>> leftGuard(vector<vector<int>> bars)
This takes the bars calculated by the bar function and returns the left guard bars of the bar code i.e. the first bars denoting the beginning of the barcode. 

vector<vector<int>> rightGuard(vector<vector<int>> bars)
This takes the bars calculated by the bar function and returns the right guard bars of the bar code i.e. the last bars of the barcode. 

vector<vector<int>> midGuard(vector<vector<int>> bars)
This takes the bars calculated by the bar function and returns the middle guard bars of the bar code. 

vector<vector<vector<int>>> lhsAndRhsBars(vector<vector<int>> bars)
This takes the bars calculated by the bar function and returns the left hand side and right hand side bars representing twelve numbers in the barcode

vector<vector<vector<int>>> lhsBars(vector<vector<int>> bars)
This takes the bars calculated by the bar function and returns the left hand side bars  representing the numbers in the left section of the barcode i.e. the first six numbers.

vector<vector<vector<int>>> rhsBars(vector<vector<int>> bars)
This takes the bars calculated by the bar function and returns the right hand side bars  representing the numbers in the right section of the barcode i.e. the last six numbers.

double averageBarLength1(vector<vector<int>> bars)
This takes the guard bars calculated in the bar function and calculated the average length of the black bars.

double averageBarLength0(vector<vector<int>> bars)
This takes the guard bars calculated in the bar function and calculated the average length of the white bars.

long double maxDist_lhs(int j,long double avbl1,long double avbl0,vector<vector<vector<int>>> bars_lhs)
This caculates the maximum distance between a number in the barcode encoded in black and white bars  and the ideal numbers encoded by bars in the left hand side of the barcode

long double maxDist_rhs(int j,long double avbl1,long double avbl0,vector<vector<vector<int>>> bars_rhs)  
This caculates the maximum distance between a number in the barcode encoded in black and white bars  and the ideal numbers encoded by bars in the right hand side of the barcode

long double pdash_lhs(int i,int j,long double avbl1,long double avbl0,vector<vector<vector<int>>> bars_lhs)
This calculates p dash for the left hand side bars according to the formula

p'(ck, s) = 1 −d(rk, rs)
	          ______________
            max(d(ri, rs))
where d(rk, rs) is the distance between a number (represented by  bars and spaces based on averageBarLength1 and averageBarLength1)   and a number in the left hand side of the barcode.

long double pdash_rhs(int i,int j,long double avbl1,long double avbl0,vector<vector<vector<int>>> bars_rhs)
This calculates p (similarity)dash for the right hand side bars according to the formula

p'(ck, s) = 1 −d(rk, rs)
            _______________
             max(d(ri, rs))
where d(rk, rs) is the distance between a number (represented by  bars and spaces based on averageBarLength1 and averageBarLength1)   and a number in the right hand side of the barcode.

long double p_lhs(int i,int j,double avbl1,double avbl0,vector<vector<vector<int>>> bars_lhs)
This calculates  p (similarity) for the left hand side

p(ck, s) = p'(ck, s)
          ______________
	      	20 
		      Sigma    p'(ci, s)	  
		      i=1

long double p_rhs(int i,int j,double avbl1,double avbl0,vector<vector<vector<int>>> bars_rhs)

This calculates  p(similarity)  for the right  hand side
p(ck, s) =  p'(ck, s)
            _____________
		        20 
		        Sigma    p'(ci, s)	  
		        i=1


bool checkSum(vector<int> bc)
This uses the calculated number in the barcode to check if the first twelve numbers compute to the thirtheenth check sum digit.

void barcode(long double st,vector<vector<int>> bars,long double avbl1,long double avbl0,long double d)
This calculates the probability for each combination of digits
p(c(1), .  , c(12)|s(1), .  , s(12)) =12
							Product p(c(i)|s(i))1/12
							i=1
The delta value gives a range of values to compate
avgBarLength1 - delta to avgBarLength1 + delta 
in incremental steps of d
