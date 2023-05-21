//
// main.cpp
//
// This sample code demonstrates how to use the OrbitTools C++ classes
// to determine satellite position and look angles.
//
// Copyright (c) 2003-2014 Michael F. Henry
//
// 06/2014
//
#include "stdafx.h"

#include <stdio.h>

// "coreLib.h" includes basic types from the core library,
// such as cSite, cJulian, etc. The header file also contains a
// "using namespace" statement for Zeptomoby::OrbitTools.
#include "coreLib.h"

// "orbitLib.h" includes basic types from the orbit library,
// including cOrbit.
#include "orbitLib.h"

#include "GenSatPath.h"
#include <iostream>
using namespace std;
// Forward declaration of helper function; see below
//void PrintPosVel(const cSatellite& sat);

//////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
   string dataFolder = "../../data";
   cout << "count: " << argc << endl;
   cout << "arg: " << argv[0] << endl;
   if (argc > 1)
   {
      dataFolder = argv[1];
   }

   cout << "data folder: " << dataFolder << endl;

   GenSatPath gen(dataFolder + "/sat.db", dataFolder);
   gen.Do();
}