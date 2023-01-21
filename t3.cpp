#include"gnet/gnet.hpp"
#include<cstdlib>

int main() {
  GNet *gnet = new GNet();

  free(gnet);
}