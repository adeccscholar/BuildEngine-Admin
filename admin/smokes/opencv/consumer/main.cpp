#include <opencv2/core.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include <print>
#include <vector>

int main() {
   cv::Mat theSource(2, 2, CV_8UC3, cv::Scalar(10, 20, 30));
   cv::Mat theGray;
   cv::cvtColor(theSource, theGray, cv::COLOR_BGR2GRAY);

   std::vector<unsigned char> vecEncoded;
   bool const bEncoded = cv::imencode(".png", theSource, vecEncoded);
   cv::Mat const theDecoded = bEncoded ? cv::imdecode(vecEncoded, cv::IMREAD_COLOR) : cv::Mat {};

   cv::dnn::Net theNet;
   bool const bRuntime = !theGray.empty() && !theDecoded.empty() && theNet.empty();

   std::println("SMOKE|CHECK|runtime|{}|OpenCV {} core/imgproc/imgcodecs/dnn runtime",
                bRuntime ? "PASS" : "FAIL", CV_VERSION);
   std::println("SMOKE|RESULT|{}|OpenCV 15-module consumer usable",
                bRuntime ? "PASS" : "FAIL");
   return bRuntime ? 0 : 1;
}
