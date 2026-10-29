#pragma once
// Popups shared by V1.0.1 native drawing and pointer hit-testing.
// No DSP state or host calls. Every option is mouse-accessible.
#include "core/UiV101Layout.h"
#include <algorithm>
#include <cstddef>
#include <cstdint>

namespace drumator {
struct UiV101Popup final {
 using L=UiV101Layout;
 enum class Kind:std::uint8_t{None,Pattern,Fill,Output,Choke,Sample};
 struct Geometry {
   double x=0.,y=0.,width=0.,cellHeight=26.;
   int columns=1,rows=0;
 };
 static constexpr int count(Kind kind) noexcept {
   switch(kind){
     case Kind::Pattern:return 7;
     case Kind::Fill:return 8;
     case Kind::Output:return 16;
     case Kind::Choke:return 5;
     case Kind::Sample:return 3;
     case Kind::None:return 0;
   }
   return 0;
 }
 static Geometry geometry(Kind kind,double rowTop,double viewHeight) noexcept {
   const auto x=kind==Kind::Pattern?364.:kind==Kind::Fill?84.:
                kind==Kind::Output?336.:kind==Kind::Choke?458.:155.;
   const int cols=kind==Kind::Output?2:1;
   const int rows=(count(kind)+cols-1)/cols;
   const double w=cols==2?224.:kind==Kind::Pattern?190.:176.;
   const double top=kind==Kind::Pattern?136.:rowTop+L::rowHeight;
   const double maxY=std::max(L::header,viewHeight-rows*26.-5.);
   return {x,std::clamp(top,L::header,maxY),w,26.,cols,rows};
 }
 static L::Rect cell(Geometry g,int index) noexcept {
   const int row=index%g.rows,col=index/g.rows;
   const double step=g.width/g.columns;
   return {g.x+step*col,g.y+g.cellHeight*row,
           g.x+step*(col+1)-2.,g.y+g.cellHeight*(row+1)-2.};
 }
 static int hit(Kind kind,Geometry g,double x,double y) noexcept {
   for(int i=0;i<count(kind);++i)
     if(cell(g,i).hit(x,y))return i;
   return -1;
 }
 static const char* label(Kind kind,int option) noexcept {
   static constexpr const char* pattern[]={"COPY","PASTE","CLEAR","UNDO",
      "RANDOM VELOCITY","VELOCITY 100","DUPLICATE"};
   static constexpr const char* fills[]={"EVERY 2","EVERY 4","EVERY 8",
      "SNARE 2/4","ALL","OFFBEAT 8","GALLOP","SNARE GHOST"};
   static constexpr const char* choke[]={"OFF","1","2","3","4"};
   static constexpr const char* sample[]={"LOAD SAMPLE","RENAME ROW","REMOVE SAMPLE"};
   if(option<0||option>=count(kind))return "";
   switch(kind){
     case Kind::Pattern:return pattern[option];
     case Kind::Fill:return fills[option];
     case Kind::Choke:return choke[option];
     case Kind::Sample:return sample[option];
     default:return "";
   }
 }
};
} // namespace drumator
