#include"gnet/gnet.hpp"
#include<cstdlib>
#include<iostream>

using namespace std;

int main() {
  GNet *gnet = new GNet();

  gnet->start_server(1234);
  cout<<"Is it?"<<endl;

  free(gnet);
}