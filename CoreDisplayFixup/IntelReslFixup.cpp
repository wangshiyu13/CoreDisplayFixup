//
//  IntelReslFixup.cpp
//  CoreDisplayFixup
//
//  Copyright © 2017 vit9696, Vanilla. All rights reserved.
//
//  This kext is made based on vit9696's Shiki, without his amazing repo it won't be here!
//

#include "IntelReslFixup.hpp"
#include "PrivateAPI.h"


static const char *binaryList[] {
  // 10.10.x and 10.11.x
  "/System/Library/Frameworks/IOKit.framework/Versions/A/IOKit",
  // as of 10.12.x
  "/System/Library/Frameworks/CoreDisplay.framework/Versions/A/CoreDisplay"
};

static const char *procList[] {
  // 10.10.x or 10.11.x
  "/System/Library/Frameworks/CoreGraphics.framework/Versions/A/Resources/WindowServer",
  // as of 10.12.x
  "/System/Library/PrivateFrameworks/SkyLight.framework/Versions/A/Resources/WindowServer"
};

// Patches
//
// Reference:
// https://github.com/Floris497/mac-pixel-clock-patch-V2/blob/master/CoreDisplay-patcher.command
//
static const uint8_t findBuf[] {
  0xB8, 0x01, 0x00, 0x00, 0x00,                   // mov eax, 0x1
  0xF6, 0xC1, 0x01,                               // test cl, 0x1
  0x0F, 0x85                                      // jne "somewhere" ; Don't care for the exact offset!
};

static const uint8_t replBuf[] {
  0x48, 0x33, 0xC0,                               // xor eax, eax    ; 0
  0x90, 0x90, 0x90, 0x90, 0x90, 0x90,             // 6x nop          ; placeholders
  0xE9                                            // jmp "somewhere" ; Don't care for the exact offset!
};

static const size_t bufSize = getArrayLength(findBuf);


static UserPatcher::BinaryModPatch genericPatch {
  CPU_TYPE_X86_64,
  findBuf,
  replBuf,
  bufSize,
  0,           // skip
  1,           // count
  UserPatcher::FileSegment::SegmentTextText,
  SectionYosEC // 10.10.x till 10.12.x
};

UserPatcher::BinaryModInfo ADDPR(binaryModYosEC)[] { // 10.10.x and 10.11.x
  binaryList[0], &genericPatch, 1
};

UserPatcher::BinaryModInfo ADDPR(binaryModSieHigh)[] { // as of 10.12.x
  binaryList[1], &genericPatch, 1
};

const size_t ADDPR(binaryModSize) = 1;


UserPatcher::ProcInfo ADDPR(procInfoYosEC)[] {
  procList[0], 83, SectionYosEC
};

UserPatcher::ProcInfo ADDPR(procInfoSieHigh)[] {
  procList[1], 86, SectionSieHS
};

const size_t ADDPR(procInfoSize) = 1;
