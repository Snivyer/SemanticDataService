#include "SDSlet/SDSlet.h"

using namespace SDSlet;
using namespace SDS;
using namespace std;
using arrow::Status;


int main() {

  std::shared_ptr<SDSlet::SDSlet> sdslet = SDSlet::SDSlet::createSDSlet(1000, "/tmp/store", "/tmp/meta");

  sdslet->init();
  sdslet->run();

  return 0;
    
    
}

