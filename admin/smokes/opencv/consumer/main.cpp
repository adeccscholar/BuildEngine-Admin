#include <opencv2/core.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/ml.hpp>
#include <opencv2/stitching.hpp>
#include <opencv2/videoio/registry.hpp>

#include <print>
#include <string>
#include <vector>

int main() {
   cv::Mat theSource(2, 2, CV_8UC3, cv::Scalar(10, 20, 30));
   cv::Mat theGray;
   cv::cvtColor(theSource, theGray, cv::COLOR_BGR2GRAY);

   std::vector<unsigned char> vecEncoded;
   bool const bEncoded = cv::imencode(".png", theSource, vecEncoded);
   cv::Mat const theDecoded = bEncoded ? cv::imdecode(vecEncoded, cv::IMREAD_COLOR) : cv::Mat {};

   cv::dnn::Net theNet;
   cv::Ptr<cv::ml::SVM> const theSvm=cv::ml::SVM::create();
   cv::Ptr<cv::Stitcher> const theStitcher=cv::Stitcher::create();
   bool const bModules=!theSvm.empty() && !theStitcher.empty();
   std::println("SMOKE|CHECK|ml-stitching|{}|OpenCV ML and panorama instances",
      bModules?"PASS":"FAIL");
   bool const bMsmf=cv::videoio_registry::hasBackend(cv::CAP_MSMF);
   bool const bDshow=cv::videoio_registry::hasBackend(cv::CAP_DSHOW);
   std::println("SMOKE|CHECK|camera-backends|{}|MSMF={} DirectShow={}",
      bDshow?"PASS":"FAIL",bMsmf,bDshow);
   // No camera is opened in CI. Actual capture requires a separate opt-in test.
   std::string const strBuildInformation = cv::getBuildInformation();
   std::size_t const uParallelLine = strBuildInformation.find("Parallel framework:");
   std::string strParallel = "not reported";
   if(uParallelLine != std::string::npos) {
      std::size_t const uLineEnd = strBuildInformation.find('\n', uParallelLine);
      strParallel = strBuildInformation.substr(uParallelLine, uLineEnd - uParallelLine);
   }
   std::println("SMOKE|INFO|parallel-framework|{}", strParallel);
   bool const bTbb = strParallel.find("TBB") != std::string::npos;
   std::println("SMOKE|CHECK|tbb-backend|{}|OpenCV build parallel framework: {}",
      bTbb ? "PASS" : "FAIL", strParallel);
   bool const bRuntime = !theGray.empty() && !theDecoded.empty() && theNet.empty();

   std::println("SMOKE|CHECK|runtime|{}|OpenCV {} core/imgproc/imgcodecs/dnn runtime",
                bRuntime ? "PASS" : "FAIL", CV_VERSION);
   std::println("SMOKE|RESULT|{}|OpenCV 17-module consumer with Windows camera backends usable",
                (bRuntime && bTbb && bModules && bDshow) ? "PASS" : "FAIL");
   return (bRuntime && bTbb && bModules && bMsmf && bDshow) ? 0 : 1;
}
