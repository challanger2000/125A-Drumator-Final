#pragma once
// Shared native VSTGUI painter for both production view and pixel-regression fixture.
// All control geometry is taken from UiV101Layout; this class does not change DSP.
#include "core/UiV101Layout.h"
#include "vst3/UiV101Interaction.h"
#include "vstgui/lib/cdrawcontext.h"
#include "vstgui/lib/cgraphicspath.h"
#include "vstgui/lib/cgraphicstransform.h"
#include "vstgui/lib/cbitmap.h"
#include "vst3/BrandLogoGeometry.h"
#include <cstdlib>
#include <array>
#include <cmath>
#include <cstdio>
#include <string>
#include <algorithm>
#include <cstdint>

namespace drumator::v101 {
struct UiV101NativePreview final {
 using L=UiV101Layout;
 using Color=VSTGUI::CColor;
 using Box=VSTGUI::CRect;
 static constexpr double kChassisLeft=12.,kChassisRight=1260.;
 static constexpr double kRowMountRadius=23.;
 static constexpr double kHeaderMountRadius=22.;
 static_assert(2.*kRowMountRadius+2.<=L::rowHeight,
               "Rotary mounting socket must not overlap the next 48px row");
 static_assert(L::headerKnobY-kHeaderMountRadius>=92.&&
               L::headerKnobY+kHeaderMountRadius<=L::header-17.,
               "Header mounting socket must stay clear of the heading and ruler");
 // Material colors sampled from the supplied reference screenshot. The
 // original light steel is warm (R>G>B), and the sequencer is dark navy,
 // deliberately NOT Windows grey or near-black.
 struct Skin {
   static Color steel(){return {201,194,183,255};}
   static Color steelHighlight(){return {217,209,197,255};}
   static Color steelShadow(){return {171,164,155,255};}
   static Color headerWell(){return {197,190,179,255};}
   static Color headerSurface(){return {207,200,189,255};}
   static Color graphite(){return {29,32,36,255};}
   static Color panelEdge(){return {101,93,86,255};}
   static Color navyWell(){return {15,27,40,255};}
   static Color navyA(){return {27,41,56,255};}
   static Color navyB(){return {30,45,61,255};}
   static Color navyTrim(){return {63,78,92,255};}
   static Color stepOff(){return {36,51,70,255};}
   static Color amber(){return {250,145,30,255};}
   static Color amberLens(){return {255,195,74,255};}
 };
 struct RowData final {
   std::string name;
   bool hasSample=false,muted=false,solo=false;
   unsigned gain=100,randomRealtime=0,outputBus=0;
   int pan=0,pitch=0;
   std::array<std::uint8_t,L::steps> velocities{};
 };
 struct Scene final {
   std::size_t pattern=0,visibleRows=L::visibleRows;
   std::uint8_t patternLength=32;
   bool songEnabled=false,canRemoveRow=false;
   bool clipped=false;
   float meterLeft=0.f,meterRight=0.f;
   std::string status{};
   unsigned swing=0,humanize=0,master=100;
   std::array<RowData,L::instruments> rows{};
 };
 static void fill(VSTGUI::CDrawContext& c,Box r,Color color){
   c.setFillColor(color);c.drawRect(r,VSTGUI::kDrawFilled);
 }
 static void border(VSTGUI::CDrawContext& c,Box r,Color color){
   c.setFrameColor(color);c.setLineWidth(1.);c.drawRect(r,VSTGUI::kDrawStroked);
 }
 static Color mix(Color a,Color b,double t){
   t=std::clamp(t,0.,1.);
   return {static_cast<std::uint8_t>(std::lround(a.red+(b.red-a.red)*t)),
           static_cast<std::uint8_t>(std::lround(a.green+(b.green-a.green)*t)),
           static_cast<std::uint8_t>(std::lround(a.blue+(b.blue-a.blue)*t)),255};
 }
 // Deterministic native paint: two-slope satin-metal gradient without a
 // bitmap dependency. Evenly spaced strips give subtle directionality at
 // both 100% and 150% while keeping repaint costs bounded.
 static void satin(VSTGUI::CDrawContext& c,Box r,Color upper,Color middle,
                   Color lower,int strips=20){
   if(r.getWidth()<=0.||r.getHeight()<=0.||strips<1)return;
   for(int i=0;i<strips;++i){
     const double top=r.top+r.getHeight()*i/strips;
     const double bottom=r.top+r.getHeight()*(i+1)/strips;
     const double t=(i+.5)/strips;
     const auto ink=t<.5?mix(upper,middle,t*2.):
                            mix(middle,lower,(t-.5)*2.);
     fill(c,{r.left,top,r.right,bottom},ink);
   }
 }
 // Draw the supplied faceplate as ONE photograph. Never nine-slice it:
 // slicing a panel with continuous grooves, bevels and brushed grain
 // breaks the geometry at every slice boundary.
 // Keep the artwork's aspect ratio and crop the surplus at the center.
 // All views share the same 1280x686 reference framing, including SONG.
 static bool drawVintageFaceplate(VSTGUI::CDrawContext& c,
                                  VSTGUI::CBitmap* plate,double height,bool omitHeader=false){
   if(!plate||!plate->isLoaded()||plate->getWidth()<300.||
      plate->getHeight()<300.||height<L::header+L::footer)return false;
   const double w=plate->getWidth(),h=plate->getHeight();
   constexpr double referenceHeight=686.;
   const double scale=std::max(L::width/w,referenceHeight/h);
   const double x=(L::width-w*scale)*.5;
   const double y=(referenceHeight-h*scale)*.5;
   c.saveGlobalState();
   c.setClipRect({0.,omitHeader?L::header:0.,L::width,height});
   {
     VSTGUI::CDrawContext::Transform tx(c,
       VSTGUI::CGraphicsTransform().scale(scale,scale).translate(x,y));
     plate->draw(&c,{0.,0.,w,h});
   }
   c.restoreGlobalState();
   return true;
 }
 // Square buttons were the main Windows-98 cue. These tiny inset corner
 // cuts mimic a physical rounded metal key, without changing mouse hits.
 static void rounded(VSTGUI::CDrawContext& c,Box r,Color color,double corner=3.){
   if(r.getWidth()<=corner*2.||r.getHeight()<=corner*2.)return;
   fill(c,{r.left+corner,r.top,r.right-corner,r.bottom},color);
   fill(c,{r.left+1.,r.top+1.,r.right-1.,r.bottom-1.},color);
   fill(c,{r.left,r.top+corner,r.right,r.bottom-corner},color);
 }
 // Dark screw heads with a brushed rim, one highlight and cross recess.
 // These are paint-only and do not intrude into interactive rectangles.
 static void screw(VSTGUI::CDrawContext& c,double x,double y){
   c.setFillColor({69,63,58,255});
   c.drawEllipse({x-7.,y-7.,x+7.,y+7.},VSTGUI::kDrawFilled);
   c.setFrameColor({226,220,210,255});c.setLineWidth(1.);
   c.drawEllipse({x-6.,y-6.,x+6.,y+6.},VSTGUI::kDrawStroked);
   c.setFillColor({28,28,27,255});
   c.drawEllipse({x-4.5,y-4.5,x+4.5,y+4.5},VSTGUI::kDrawFilled);
   c.setFrameColor({122,117,111,255});
   c.drawLine({x-2.5,y-2.5},{x+2.5,y+2.5});
   c.drawLine({x+2.5,y-2.5},{x-2.5,y+2.5});
   c.setFrameColor({13,13,14,255});
   c.drawLine({x-2.5,y-1.5},{x+1.5,y+2.5});
 }
 static void label(VSTGUI::CDrawContext& c,const char* value,Box r,
                   int font=11,Color ink={191,209,223,255},bool center=true){
   c.setFont(VSTGUI::kNormalFont,font);c.setFontColor(ink);
   c.drawString(value,r,center?VSTGUI::kCenterText:VSTGUI::kLeftText);
 }
 // Complete official 125A Systems master (all 30 paths), sourced from
 // 125A-Branding/125A_Logo_Master_FINAL.svg. The master explicitly forbids
 // redrawing or omitting the SYSTEMS / AUDIO SOFTWARE lettering, and no
 // independently approved signet-only asset exists in the Branding repo.
 // Draw uniformly scaled, optically centered by the actual master silhouette.
 static void brandMasterLogo(VSTGUI::CDrawContext& c,L::Rect area){
   static_assert(kBrandLogoPaths.size()==30);
   // Actual bounds of the visible master paths; omit blank SVG viewBox margins.
   constexpr double sourceX=163.,sourceY=103.,sourceW=1446.,sourceH=703.;
   // Do not fit the logo into the entire headplate: the stepped horizontal
   // bevel crosses that rectangle. The caller passes only the verified
   // graphite interior above the bevel. Keep a further 3px inset.
   constexpr double margin=3.;
   const double sf=std::min(
       (area.x1-area.x0-2.*margin)/sourceW,
       (area.y1-area.y0-2.*margin)/sourceH);
   const double left=(area.x0+area.x1-sourceW*sf)*0.5-sourceX*sf;
   const double top=(area.y0+area.y1-sourceH*sf)*0.5-sourceY*sf;
   for(std::size_t n=0;n<kBrandLogoPaths.size();++n){
     const auto& shape=kBrandLogoPaths[n];
     auto* path=c.createGraphicsPath();
     if(!path)continue;
     const char* cursor=shape.path;
     while(*cursor){
       while(*cursor==' '||*cursor=='\n'||*cursor=='\t')++cursor;
       if(!*cursor)break;
       const char command=*cursor++;
       if(command=='Z'){path->closeSubpath();continue;}
       if(command!='M'&&command!='L')break;
       while(*cursor==' ')++cursor;
       char* end=nullptr;
       const double x=std::strtod(cursor,&end);
       if(end==cursor)break;
       cursor=end;
       while(*cursor==' ')++cursor;
       const double y=std::strtod(cursor,&end);
       if(end==cursor)break;
       cursor=end;
       if(command=='M')path->beginSubpath(left+x*sf,top+y*sf);
       else path->addLine(left+x*sf,top+y*sf);
     }
     c.setFillColor(shape.red?Color{215,25,32,255}:Color{217,217,217,255});
     c.drawGraphicsPath(path,VSTGUI::CDrawContext::kPathFilledEvenOdd);
     path->forget();
   }
 }
 // The user-supplied original PNG is the only V1.0.1 title source.
 // Measure alpha only ONCE when the view loads its image resource, never
 // when audio-driven editor repaints occur. Trim transparent canvas in the
 // layout calculation; leave the original user filename and pixels intact.
 struct TitleBounds final {
   double left=0.,top=0.,width=0.,height=0.;
   bool valid()const noexcept{return width>0.&&height>0.;}
 };
 static TitleBounds measureTitle(VSTGUI::CBitmap* image){
   if(!image||image->getWidth()<1.||image->getHeight()<1.)return {};
   auto* pixels=VSTGUI::CBitmapPixelAccess::create(image,false);
   if(!pixels)return {};
   const auto w=pixels->getBitmapWidth(),h=pixels->getBitmapHeight();
   std::uint32_t xMin=w,yMin=h,xMax=0,yMax=0;
   bool found=false;
   for(std::uint32_t y=0;y<h;++y)
     for(std::uint32_t x=0;x<w;++x){
       if(!pixels->setPosition(x,y))continue;
       VSTGUI::CColor rgba{};pixels->getColor(rgba);
       if(rgba.alpha<8)continue;
       found=true;
       xMin=std::min(xMin,x);xMax=std::max(xMax,x);
       yMin=std::min(yMin,y);yMax=std::max(yMax,y);
     }
   pixels->forget();
   if(!found)return {};
   return {static_cast<double>(xMin),static_cast<double>(yMin),
           static_cast<double>(xMax-xMin+1),
           static_cast<double>(yMax-yMin+1)};
 }
 static VSTGUI::CRect titleTarget(TitleBounds bounds){
   if(!bounds.valid())return {};
   // Stays between the 125A enamel badge (right edge x=157) and the
   // existing ZOOM selector on the far right; no flattening/distortion.
   constexpr double left=192.,right=1118.,top=7.,bottom=76.;
   const double fit=std::min((right-left)/bounds.width,
                             (bottom-top)/bounds.height);
   const double width=bounds.width*fit,height=bounds.height*fit;
   return {(left+right-width)/2.,(top+bottom-height)/2.,
           (left+right+width)/2.,(top+bottom+height)/2.};
 }
 static void hardwareTitle(VSTGUI::CDrawContext& c,VSTGUI::CBitmap* logo,
                           TitleBounds bounds){
   if(!logo||!bounds.valid())return;
   const auto dest=titleTarget(bounds);
   const double sf=dest.getWidth()/bounds.width;
   const double x=dest.left-bounds.left*sf;
   const double y=dest.top-bounds.top*sf;
   // A standalone metallic wordmark: draw at its intended 100% alpha.
   // The PNG has no backing plate. Alpha-bound measurement skips near-zero
   // stray pixels without altering or physically cropping the source asset.
   VSTGUI::CDrawContext::Transform tx(c,
      VSTGUI::CGraphicsTransform().scale(sf,sf).translate(x,y));
   logo->draw(&c,{0.,0.,logo->getWidth(),logo->getHeight()});
 }
 // Same 125x125, 101-frame JKnobMan image used by V1.0; the actual
 // opaque dial sits inside its transparent frame. Not a painted imitation.
 static void originalDial(VSTGUI::CDrawContext& c,VSTGUI::CMultiFrameBitmap* sprite,
                          double cx,double cy,double fraction,double frameSize){
   if(!sprite||sprite->getWidth()!=125.||sprite->getHeight()!=12625.)return;
   const double sf=frameSize/125.;
   const double left=cx-frameSize/2.,top=cy-frameSize/2.;
   const auto frame=sprite->normalizedValueToFrameIndex(
     static_cast<float>(std::clamp(fraction,0.,1.)));
   VSTGUI::CDrawContext::Transform tx(c,
     VSTGUI::CGraphicsTransform().scale(sf,sf).translate(left,top));
   sprite->drawFrame(&c,frame,{0.,0.});
 }
 // Owner-approved dark modern 83x32 high-resolution glossy button sprites
 // (332x128 @4x). For Pattern A-H the bezel uses undistorted edge strips;
 // for all other dimensions a nine-slice preserves bezel corner geometry.
 // Paint and hit rectangles are shared with UiV101Layout.
 struct ButtonSprites {
   VSTGUI::CBitmap* normal{};
   VSTGUI::CBitmap* active{};
   VSTGUI::CBitmap* pressed{};
   VSTGUI::CBitmap* disabled{};
 };
 static bool drawVintageButton(VSTGUI::CDrawContext& c,Box b,
                                VSTGUI::CBitmap* sprite){
   if(!sprite||!sprite->isLoaded()||sprite->getWidth()!=332.||
      sprite->getHeight()!=128.||b.getWidth()<=0.||b.getHeight()<=0.)
     return false;
   if(b.getWidth()==83.&&b.getHeight()==32.){
     VSTGUI::CDrawContext::Transform tx(c,
       VSTGUI::CGraphicsTransform().scale(.25,.25)
         .translate(b.left,b.top));
     sprite->draw(&c,{0.,0.,332.,128.});
     return true;
   }
   if(b.getWidth()==35.&&b.getHeight()==32.){
     // Preserve the approved 35x32 A-H image without distortion.
     VSTGUI::CDrawContext::Transform tx(c,
       VSTGUI::CGraphicsTransform().scale(.25,.25)
         .translate(b.left,b.top));
     sprite->draw(&c,{0.,0.,52.,128.},{0.,0.});
     sprite->draw(&c,{52.,0.,88.,128.},{148.,0.});
     sprite->draw(&c,{88.,0.,140.,128.},{280.,0.});
     return true;
   }
   // Nine-slice for remaining size families: preserve physical cap corners
   // and edge wear. Stretch only the broad unlettered center of the approved
   // high-resolution image. Do not alter paint/hit rectangles.
   // VEL and SET are genuine 37x20 function buttons; use the same dark
   // nine-slice (6px top/bottom bezels with 8px center), not silver fallback.
   if(b.getWidth()<22.||b.getWidth()>220.||
      b.getHeight()<18.||b.getHeight()>48.)return false;
   constexpr std::array<double,4> sx{{0.,40.,292.,332.}};
   constexpr std::array<double,4> sy{{0.,24.,104.,128.}};
   const std::array<double,4> dx{{b.left,b.left+10.,b.right-10.,b.right}};
   const std::array<double,4> dy{{b.top,b.top+6.,b.bottom-6.,b.bottom}};
   for(std::size_t row=0;row<3;++row)
     for(std::size_t col=0;col<3;++col){
       const double srcW=sx[col+1]-sx[col],srcH=sy[row+1]-sy[row];
       const double dstW=dx[col+1]-dx[col],dstH=dy[row+1]-dy[row];
       c.saveGlobalState();
       c.setClipRect({dx[col],dy[row],dx[col+1],dy[row+1]});
       {
         VSTGUI::CDrawContext::Transform tx(c,
           VSTGUI::CGraphicsTransform()
             .scale(dstW/srcW,dstH/srcH)
             .translate(dx[col]-sx[col]*dstW/srcW,
                        dy[row]-sy[row]*dstH/srcH));
         sprite->draw(&c,{0.,0.,332.,128.});
       }
       c.restoreGlobalState();
     }
   return true;
 }
 // Shared machined keycap family. Every action button uses the same
 // rim, satin-silver/amber lens, highlights, lower bevel, lettering and shadow.
 // All layers sit strictly inside the common paint/hit rectangle.
 static void cap(VSTGUI::CDrawContext& c,L::Rect r,double y,const char* text,
                 bool active=false,int font=10,
                 const ButtonSprites* vintage=nullptr,
                 bool disabled=false,bool pressed=false){
   const Box b{r.x0,y+r.y0,r.x1,y+r.y1};
   VSTGUI::CBitmap* sprite=!vintage?nullptr:
      disabled?vintage->disabled:pressed?vintage->pressed:
      active?vintage->active:vintage->normal;
   if(sprite&&drawVintageButton(c,b,sprite)){
     // Keep all labels native, crisp and dynamically localized/stateful.
     label(c,text,{b.left+2.,b.top+2.,b.right-2.,b.bottom-2.},
           font,disabled?Color{130,142,153,255}:
           active?Color{255,193,74,255}:Color{229,236,243,255});
     return;
   }
   const double l=b.left,t=b.top,rr=b.right,bt=b.bottom;
   const bool compact=b.getHeight()<24.;
   const double radius=compact?2.:3.;
   // Deep charcoal pocket + soft offset shadow under the raised cap.
   rounded(c,{l,t+2.,rr,bt+2.},{7,10,14,255},radius);
   rounded(c,{l,t,rr,bt-1.},active?
             Color{116,56,21,255}:Color{30,31,33,255},radius);
   rounded(c,{l+1.,t+1.,rr-1.,bt-3.},active?
             Color{255,169,47,255}:Color{106,101,94,255},radius);
   rounded(c,{l+2.,t+2.,rr-2.,bt-4.},active?
             Color{177,72,20,255}:Color{179,171,160,255},radius);
   // A mild warm facet gradient; do not let the controls look chrome-white.
   satin(c,{l+4.,t+4.,rr-4.,bt-6.},
         active?Color{255,224,122,255}:Color{226,219,208,255},
         active?Color{244,151,46,255}:Color{197,190,180,255},
         active?Color{217,94,24,255}:Color{158,151,143,255},compact?3:7);
   // Continuous edge lights make the pressed vs released geometry legible.
   fill(c,{l+5.,t+3.,rr-5.,t+4.},active?
        Color{255,235,158,255}:Color{248,240,226,255});
   fill(c,{l+5.,bt-6.,rr-5.,bt-5.},active?
        Color{151,53,13,255}:Color{110,104,98,255});
   fill(c,{l+2.,t+5.,l+3.,bt-6.},active?
        Color{255,189,89,255}:Color{237,228,214,255});
   fill(c,{rr-3.,t+5.,rr-2.,bt-6.},active?
        Color{132,49,17,255}:Color{85,80,77,255});
   label(c,text,{l+2.,t+2.,rr-2.,bt-2.},font,
         active?Color{65,32,16,255}:Color{24,27,30,255});
 }
 // Modal zoom is a topmost shared overlay in MAIN and SONG. Painting
 // it only inside the main sequencer would let the SONG heading hide the
 // fourth (150%) option while pointer capture was still active.
 static void zoomPopup(VSTGUI::CDrawContext& c,std::size_t selected,
                       const ButtonSprites* vintage=nullptr){
   constexpr const char* names[]={"100%","120%","130%","150%"};
   const double left=L::zoomOption(0).x0-4.;
   const double right=L::zoomOption(0).x1+4.;
   const double top=L::zoomOption(0).y0-4.;
   const double bottom=L::zoomOption(L::zoomPresets.size()-1).y1+4.;
   fill(c,{left,top,right,bottom},{12,20,29,255});
   border(c,{left,top,right,bottom},{104,115,121,255});
   for(std::size_t i=0;i<L::zoomPresets.size();++i)
     cap(c,L::zoomOption(i),0.,names[i],i==selected,11,vintage);
 }
 // Dark-blue physical sequencer keycap, with matching bezel and warm
 // backlit orange state. Keep the same 17px step width / 19px pitch.
 // Faithful V1.0 drawHardwareStep paint translated into compact 48px row.
 // Original established dark face, quarter accent and lit amber key lens.
 static void stepCap(VSTGUI::CDrawContext& c,double sx,double y,bool lit,bool downbeat){
   // This is a compact 17x27 physical switch INSIDE the established
   // 17x31 hit surface. Slightly rounded edges, black gasket, recess shadow.
   const double x=sx+1.;
   if(lit){
     // Restrained warm halo is visible against navy, not an orange rectangle.
     fill(c,{x,y+12.,x+17.,y+38.},{70,42,22,255});
   }
   rounded(c,{x,y+10.,x+17.,y+39.},{6,11,18,255},2.);
   rounded(c,{x+1.,y+11.,x+16.,y+37.},
           downbeat?Color{94,111,129,255}:Color{46,64,83,255},2.);
   rounded(c,{x+2.,y+12.,x+15.,y+36.},
           lit?Color{211,71,10,255}:Color{29,43,62,255},2.);
   satin(c,{x+3.,y+14.,x+14.,y+34.},
         lit?Color{255,206,91,255}:Color{86,105,124,255},
         lit?Color{252,150,35,255}:Color{47,67,90,255},
         lit?Color{229,86,14,255}:Color{23,38,55,255},9);
   fill(c,{x+4.,y+13.,x+13.,y+15.},
        lit?Color{255,235,145,255}:Color{125,140,156,255});
   fill(c,{x+4.,y+34.,x+13.,y+35.},
        lit?Color{166,54,9,255}:Color{13,26,41,255});
   if(lit)fill(c,{x+5.,y+28.,x+12.,y+31.},{255,198,79,255});
 }
 // Headphone pictogram replaces the ambiguous play triangle in PREVIEW.
 // All coordinates derive from the preview cap's shared hit/paint rectangle.
 static void headphone(VSTGUI::CDrawContext& c,L::Rect r,double y,
                       const ButtonSprites* vintage=nullptr){
   cap(c,r,y,"",false,10,vintage);
   const double cx=(r.x0+r.x1)/2.,cy=y+(r.y0+r.y1)/2.;
   c.setFrameColor({218,228,238,255});c.setLineWidth(2.5);
   // Rounded headband arc, mirrored for even balance at 100% zoom.
   constexpr std::array<double,9> arc{{-1.,-.96,-.80,-.52,0.,.52,.80,.96,1.}};
   for(std::size_t i=1;i<arc.size();++i){
     const auto px=[&](double t){return cx+10.*t;};
     const auto py=[&](double t){return cy-1.-10.*std::sqrt(1.-t*t);};
     c.drawLine({px(arc[i-1]),py(arc[i-1])},{px(arc[i]),py(arc[i])});
   }
   c.setFillColor({216,227,236,255});
   c.drawRect({cx-12.,cy-2.,cx-8.,cy+9.},VSTGUI::kDrawFilled);
   c.drawRect({cx+8.,cy-2.,cx+12.,cy+9.},VSTGUI::kDrawFilled);
   c.setFrameColor({155,181,200,255});c.setLineWidth(1.);
   c.drawLine({cx-9.,cy-1.},{cx-9.,cy+7.});
   c.drawLine({cx+9.,cy-1.},{cx+9.,cy+7.});
 }
 // Smaller hardware rotary with a steel mounting ring, scale and brushed face.
 // Used for the three actual global parameters, not painted as buttons.
 // Actual 125A knob sprite, reused for the two global musical controls
 // and master, with legible readouts and small engraved arc ticks.
 static void headerDial(VSTGUI::CDrawContext& c,VSTGUI::CMultiFrameBitmap* sprite,
                        double cx,double cy,double fraction,
                        const char* title,const char* readout){
   fraction=std::clamp(fraction,0.,1.);
   label(c,title,{cx-46.,78.,cx+46.,91.},11,{207,218,229,255});
   // Match VOL/PAN/PITCH's neutral graphite socket. Do not draw a warm
   // silver/tan rim on the top three controls.
   c.setFillColor({8,16,24,255});
   c.drawEllipse({cx-kHeaderMountRadius,cy-kHeaderMountRadius,
                  cx+kHeaderMountRadius,cy+kHeaderMountRadius},VSTGUI::kDrawFilled);
   c.setFrameColor({92,100,108,255});c.setLineWidth(1.);
   c.drawEllipse({cx-21.,cy-21.,cx+21.,cy+21.},VSTGUI::kDrawStroked);
   c.setFillColor({7,11,17,255});
   c.drawEllipse({cx-19.,cy-19.,cx+19.,cy+19.},VSTGUI::kDrawFilled);
   constexpr double pi=3.14159265358979323846;
   for(int tick=0;tick<9;++tick){
     const double angle=(-135.+static_cast<double>(tick)*270./8.)*pi/180.;
     const double dx=std::sin(angle),dy=-std::cos(angle);
     c.setFrameColor({92,105,118,255});
     c.drawLine({cx+dx*22.,cy+dy*22.},{cx+dx*24.,cy+dy*24.});
   }
   originalDial(c,sprite,cx,cy,fraction,44.);
   label(c,readout,{cx+24.,101.,cx+54.,126.},11,{221,231,239,255},false);
 }
 static void dial(VSTGUI::CDrawContext& c,VSTGUI::CMultiFrameBitmap* sprite,
                  double x,double y,int value,int index){
   const double fraction=index==0?value/200.:index==1?(value+100.)/200.:
                         (value+24.)/48.;
   // Radius < 24px: mounting rings never touch adjacent 48px rows.
   c.setFillColor({8,16,24,255});
   c.drawEllipse({x-kRowMountRadius,y-kRowMountRadius,x+kRowMountRadius,y+kRowMountRadius},VSTGUI::kDrawFilled);
   c.setFrameColor({92,100,108,255});c.setLineWidth(1.);
   c.drawEllipse({x-22.,y-22.,x+22.,y+22.},VSTGUI::kDrawStroked);
   c.setFillColor({7,11,17,255});
   c.drawEllipse({x-20.,y-20.,x+20.,y+20.},VSTGUI::kDrawFilled);
   originalDial(c,sprite,x,y,std::clamp(fraction,0.,1.),50.);
 }
 static void draw(VSTGUI::CDrawContext& c,VSTGUI::CMultiFrameBitmap* knobSprite,
                  VSTGUI::CBitmap* titlePng,TitleBounds titleBounds,
                  const Scene& scene,
                  bool zoomMenuOpen=false,std::size_t zoomPreset=0,
                  const UiV101Interaction::State& ui={},
                  VSTGUI::CBitmap* faceplatePng=nullptr,
                  const ButtonSprites* vintageButtons=nullptr,
                  VSTGUI::CBitmap* faceplateBadgeUnused=nullptr,
                  TitleBounds unusedBadgeBounds={}){
   // Preserve the old optional argument ABI for native preview callers.
   (void)faceplateBadgeUnused;
   (void)unusedBadgeBounds;
   const auto cream=Skin::steel();
   auto expanded=L::Expansions{};
   if(ui.expandedRow<L::instruments)expanded[ui.expandedRow]=ui.expansion;
   const auto limit=ui.expansion==L::Expansion::None?
                    L::visibleRows:L::visibleRows-1;
   const auto shown=ui.firstRow<scene.visibleRows?
                    std::min(limit,scene.visibleRows-ui.firstRow):std::size_t{0};
   const double total=L::editorHeight(ui.firstRow,shown,expanded);
   const bool hasVintageFaceplate=drawVintageFaceplate(c,faceplatePng,total);
   if(!hasVintageFaceplate){
     // Proven procedural fallback for missing/corrupt packaged image.
     fill(c,{0.,0.,L::width,total},{105,98,91,255});
     rounded(c,{2.,2.,L::width-2.,total-2.},{165,157,148,255},3.);
     satin(c,{5.,5.,L::width-5.,total-5.},
           {214,208,200,255},cream,{184,177,167,255},72);
     border(c,{5.,5.,L::width-5.,total-5.},Skin::panelEdge());
     // Subpixel-style horizontal brushing is deliberately faint (alpha ~22).
     // Highlights change with vertical position; no generated bitmap needed.
     c.setLineWidth(1.);
     for(int yy=14;yy<static_cast<int>(total)-13;yy+=5){
       c.setFrameColor({241,235,226,9});
       c.drawLine({12.,static_cast<double>(yy)},
                  {L::width-12.,static_cast<double>(yy)});
     }
     fill(c,{7.,7.,L::width-7.,9.},Skin::steelHighlight());
     fill(c,{7.,total-9.,L::width-7.,total-7.},{157,148,139,255});
   }
   // The source corner screws are now correctly sized and positioned by
   // the nine-slice. Do not overpaint them in the image-backed mode.
   if(!hasVintageFaceplate){
     screw(c,14.,15.);screw(c,L::width-14.,15.);
   }
   // Complete 125A-Branding master, including original vector captions.
   // No metal nameplate, frame, screws or substituted fonts are composited.
   brandMasterLogo(c,L::brandMasterSafeArea);
   // The standalone owner-approved DRUMATOR wordmark is alpha-composited
   // directly on the original graphite plate, with no additional plaque.
   hardwareTitle(c,titlePng,titleBounds);
   // The percentage and dropdown affordance identify the single zoom control;
   // no redundant "ZOOM" legend or decorative separating line.
   // The menu itself is painted as an
   // overlay after all rows, as a real hardware-style popover would be.
   const char* const zoomNames[]={"100%","120%","130%","150%"};
   const auto selected=zoomPreset<4?zoomPreset:std::size_t{0};
   char zoomCaption[20]{};
   std::snprintf(zoomCaption,sizeof(zoomCaption),"%s  v",zoomNames[selected]);
   cap(c,L::zoomTrigger,0.,zoomCaption,zoomMenuOpen,11,vintageButtons);
   // The uploaded graphite Faceplate PNG is the ONLY upper control background.
   // No synthetic bright strip, borders, dividers, brushed gradients or
   // inset pockets from y=78..137: buttons, rotaries, real labels and the
   // meter render directly on the original hardware photo. This applies to
   // both MAIN and SONG because they share this native painter.
   constexpr auto heading=L::patternHeading;
   label(c,"PATTERN",{heading.x0,heading.y0,heading.x1,heading.y1},
         11,{218,226,235,255},false);
   for(std::size_t i=0;i<8;++i){
     const char key[2]={static_cast<char>('A'+i),'\0'};
     cap(c,L::patternButton(i),0.,key,i==scene.pattern,12,vintageButtons);
   }
   // Spell out the action context; the obsolete "PTNR" abbreviation
   // was ambiguous beside the A-H PATTERN selector.
   label(c,"PATTERN EDIT",{364.,80.,447.,96.},10,{210,223,231,255});
   // One identical 32px machined keycap and vertical baseline for all
   // header actions: PATTERN, MENU, 1/2 BARS, SONG, SONG EDIT.
   cap(c,L::patternMenu,0.,"MENU",false,10,vintageButtons);
   cap(c,L::barsButton,0.,scene.patternLength==32?"2 BARS":"1 BAR",
       false,10,vintageButtons);
   cap(c,L::headerGroup(0),0.,"SONG",scene.songEnabled,11,vintageButtons);
   cap(c,L::headerGroup(1),0.,"SONG EDIT",false,10,vintageButtons);
   char swingText[18]{},humanText[18]{},masterText[18]{};
   std::snprintf(swingText,sizeof(swingText),"%u%%",scene.swing);
   std::snprintf(humanText,sizeof(humanText),"%u%%",scene.humanize);
   std::snprintf(masterText,sizeof(masterText),"%u%%",scene.master);
   headerDial(c,knobSprite,L::headerKnobCenters[0],L::headerKnobY,scene.swing/50.,
              "SWING",swingText);
   headerDial(c,knobSprite,L::headerKnobCenters[1],L::headerKnobY,scene.humanize/100.,
              "HUMANIZE",humanText);
   headerDial(c,knobSprite,L::headerKnobCenters[2],L::headerKnobY,scene.master/200.,
              "MASTER",masterText);
   // Actual 33 ms sampled post-master MAIN L/R levels from the live view,
   // never decorative LEDs. Clipping is the real Controller meter bitfield.
   fill(c,{1133.,100.,1246.,132.},Skin::navyWell());
   border(c,{1133.,100.,1246.,132.},Skin::navyTrim());
   const auto miniMeter=[&](float value,double top){
     const auto db=value>0.f?20.*std::log10(static_cast<double>(value)):-120.;
     for(int index=0;index<12;++index){
       const double threshold=-54.+5.*index;
       const double left=1138.+index*5.5;
       const auto lit=index<8?Color{44,187,125,255}:
                      index<10?Color{243,183,50,255}:Color{238,76,53,255};
       fill(c,{left,top,left+4.,top+7.},
            db>=threshold?lit:Color{34,55,63,255});
     }
   };
   miniMeter(scene.meterLeft,105.);
   miniMeter(scene.meterRight,118.);
   label(c,"L",{1207.,102.,1221.,115.},10,{218,227,233,255});
   label(c,"R",{1207.,117.,1221.,129.},10,{218,227,233,255});
   fill(c,{1230.,105.,1241.,116.},
        scene.clipped?Color{251,45,36,255}:Color{75,34,31,255});
   // Column headings remain outside the 48px-high channels.
   fill(c,{12.,142.,1261.,157.},Skin::navyWell());
   fill(c,{12.,142.,1261.,143.},Skin::navyTrim());
   label(c,"M",{20.,140.,47.,157.},10);
   label(c,"S",{52.,140.,79.,157.},10);
   label(c,"FILL",{84.,140.,124.,157.},10);
   label(c,"INSTRUMENT / SAMPLE",{132.,140.,304.,157.},10);
   label(c,"PLAY",{310.,140.,347.,157.},10);
   label(c,"EDIT",{354.,140.,406.,157.},10);
   for(std::size_t i=0;i<L::steps;i+=4){
     char n[8]{};std::snprintf(n,sizeof(n),"%u",
                              static_cast<unsigned>(i+1));
     const auto number=L::stepNumberRect(i);
     label(c,n,{number.x0,number.y0,number.x1,number.y1},11);
   }
   label(c,"VOL",{1037.,140.,1087.,157.},10);
   label(c,"PAN",{1097.,140.,1147.,157.},10);
   label(c,"PITCH",{1153.,140.,1214.,157.},10);
   // The photographed background contains a bright diagonal metal bevel.
   // At some zoom factors it leaks through the 2px gaps between adjacent
   // instrument chassis. Use one continuous dark enamel backing *only*
   // inside the instrument rack; keep faceplate, outer rails, header, logo,
   // column headings and bottom screw-bearing footer photo untouched.
   fill(c,{kChassisLeft,L::header,kChassisRight,total-L::footer},
        Skin::navyWell());
   for(std::size_t index=0;index<shown;++index){
     const std::size_t row=ui.firstRow+index;
     const double y=L::rowTop(ui.firstRow,index,expanded);
     const auto& data=scene.rows[row];
     const bool live=row<scene.visibleRows;
     // Recessed dark-navy instrument chassis: thick top gasket, soft
     // internal gradient, narrow upper reflection and lower occlusion.
     rounded(c,{kChassisLeft,y+1.,kChassisRight,y+L::rowHeight-1.},
           {7,15,25,255},3.);
     rounded(c,{14.,y+3.,1258.,y+L::rowHeight-3.},
             row%2?Skin::navyA():Skin::navyB(),2.);
     satin(c,{17.,y+5.,1255.,y+L::rowHeight-5.},
           {44,61,76,255},row%2?Skin::navyA():Skin::navyB(),
           {19,32,48,255},12);
     fill(c,{18.,y+4.,1254.,y+5.},{84,100,111,255});
     fill(c,{18.,y+L::rowHeight-5.,1254.,y+L::rowHeight-4.},
          {9,17,28,255});
     // Slim selection rail: visible without adding another chunky module.
     fill(c,{L::select.x0,y+L::select.y0,L::select.x1,y+L::select.y1},
          {5,12,19,255});
     fill(c,{L::select.x0+3.,y+14.,L::select.x1-3.,y+33.},
          live&&row==ui.selectedRow?Color{246,171,70,255}:Color{76,93,103,255});
     cap(c,L::mute,y,"M",live&&data.muted,10,vintageButtons);
     cap(c,L::solo,y,"S",live&&data.solo,10,vintageButtons);
     cap(c,L::fill,y,"FILL",false,9,vintageButtons);
     rounded(c,{L::sample.x0,y+8.,L::sample.x1,y+40.},{3,8,14,255},2.);
     satin(c,{L::sample.x0+2.,y+10.,L::sample.x1-2.,y+38.},
           {6,14,23,255},{11,22,35,255},{18,34,49,255},9);
     fill(c,{L::sample.x0+3.,y+10.,L::sample.x1-3.,y+11.},
          {71,88,101,255});
     border(c,{L::sample.x0,y+8.,L::sample.x1,y+40.},{62,82,98,255});
     cap(c,L::previous,y,"<",false,12,vintageButtons);
     cap(c,L::next,y,">",false,12,vintageButtons);
     const std::string emptyName="(EMPTY)";
     const auto& rowName=live?(data.name.empty()?emptyName:data.name):emptyName;
     const Box nameRect{L::load.x0+5.,y+9.,L::load.x1-2.,y+39.};
     // VSTGUI::drawString does not clip to its text rectangle. Measure with
     // the actual platform font and abbreviate on UTF-8 codepoint boundaries.
     // A final clip prevents long names from painting over sample-menu,
     // previous/next, preview or EDIT buttons.
     c.setFont(VSTGUI::kNormalFont,11);
     const char* displayedName=rowName.c_str();
     std::string shortenedName;
     if(c.getStringWidth(displayedName)>nameRect.getWidth()-2.){
       shortenedName=rowName;
       while(!shortenedName.empty()&&
             c.getStringWidth((shortenedName+"...").c_str())>
               nameRect.getWidth()-2.){
         std::size_t cut=shortenedName.size()-1;
         while(cut>0&&
               (static_cast<unsigned char>(shortenedName[cut])&0xc0u)==0x80u)
           --cut;
         shortenedName.erase(cut);
       }
       shortenedName+="...";
       displayedName=shortenedName.c_str();
     }
     c.saveGlobalState();
     c.setClipRect(nameRect);
     label(c,displayedName,nameRect,11,
           live?Color{203,222,235,255}:Color{103,116,127,255},false);
     c.restoreGlobalState();
     cap(c,L::sampleMenu,y,"...",false,10,vintageButtons);
     headphone(c,L::preview,y,vintageButtons);
     cap(c,L::edit,y,"EDIT",false,11,vintageButtons);
     rounded(c,{L::stepWell.x0,y+4.,L::stepWell.x1,y+44.},
             {9,17,29,255},2.);
     fill(c,{L::stepWell.x0+2.,y+6.,L::stepWell.x1-2.,y+8.},
          {8,15,26,255});
     fill(c,{L::stepWell.x0+2.,y+41.,L::stepWell.x1-2.,y+42.},
          {55,71,88,255});
     for(std::size_t step=0;step<L::steps;++step){
       const auto r=L::stepRect(step);
       const double sx=r.x0;
       const bool active=live&&step<scene.patternLength&&data.velocities[step]>0;
       stepCap(c,sx,y,active,step%4==0);
     }
     for(std::size_t k=0;k<L::knobCenters.size();++k)
       dial(c,knobSprite,L::knobCenters[k],y+24.,
            k==0?static_cast<int>(data.gain):k==1?data.pan:data.pitch,
            static_cast<int>(k));
     cap(c,L::velocity,y,"VEL",ui.expandedRow==row&&
           ui.expansion==L::Expansion::Velocity,11,vintageButtons);
     cap(c,L::settings,y,"SET",ui.expandedRow==row&&
           ui.expansion==L::Expansion::Settings,11,vintageButtons);
     if(ui.expandedRow==row&&ui.expansion!=L::Expansion::None){
       fill(c,{13.,y+49.,1260.,y+95.},Skin::navyWell());
       fill(c,{15.,y+50.,1258.,y+94.},Skin::navyA());
       if(ui.expansion==L::Expansion::Velocity){
         label(c,"VELOCITY",{132.,y+49.,400.,y+63.},9,{183,202,218,255},false);
         cap(c,UiV101Interaction::velocityRandom,y,"RANDOM VEL",false,10,vintageButtons);
         cap(c,UiV101Interaction::velocityReset,y,"RESET VEL",false,10,vintageButtons);
         fill(c,{417.,y+53.,1031.,y+92.},{4,13,25,255});
         for(std::size_t step=0;step<L::steps;++step){
           const auto velocity=step<scene.patternLength?
                               data.velocities[step]:std::uint8_t{0};
           const double xx=L::stepX+step*L::stepPitch;
           if(velocity==0){
             fill(c,{xx+2.,y+88.,xx+15.,y+90.},{37,56,75,255});
           }else{
             const double barHeight=3.+32.*static_cast<double>(velocity)/127.;
             fill(c,{xx+2.,y+89.-barHeight,xx+15.,y+89.},
                  {245,139,40,255});
             fill(c,{xx+3.,y+87.-barHeight,xx+14.,y+89.-barHeight},
                  {255,201,99,255});
           }
         }
         cap(c,UiV101Interaction::realtimeMinus,y,"-",false,13,vintageButtons);
         // Genuine numeric drag field rather than an oversized faux key.
         // Exact 112x32 bounds are shared with the interaction hit map.
         const auto r=UiV101Interaction::realtimeValue;
         const Box display{r.x0,y+r.y0,r.x1,y+r.y1};
         rounded(c,display,{7,12,17,255},3.);
         rounded(c,{display.left+2.,display.top+2.,
                    display.right-2.,display.bottom-2.},
                 {24,36,44,255},2.);
         border(c,display,{96,113,119,255});
         fill(c,{display.left+4.,display.top+3.,
                 display.right-4.,display.top+4.},{73,102,112,255});
         label(c,"LIVE VEL",
               {display.left+3.,display.top+3.,
                display.right-3.,display.top+15.},
               9,{159,183,191,255});
         char realtimeText[16]{};
         std::snprintf(realtimeText,sizeof(realtimeText),
                       "%u%%",data.randomRealtime);
         label(c,realtimeText,{display.left+3.,display.top+15.,
                               display.right-3.,display.bottom-2.},
               12,{247,193,99,255});
         cap(c,UiV101Interaction::realtimePlus,y,"+",false,13,vintageButtons);
       }else{
         char busLabel[20]{};
         std::snprintf(busLabel,sizeof(busLabel),"BUS %u",data.outputBus+1);
         const std::array<const char*,5> settingsLabels{{
           busLabel,"CHOKE","COPY ROW","PASTE ROW","CLEAR ROW"
         }};
         for(std::size_t button=0;button<settingsLabels.size();++button)
           cap(c,UiV101Interaction::settingsRect(button),y,
               settingsLabels[button],false,10,vintageButtons);
       }
     }
   }
   // Compact recessed fascia; its edges avoid both corner screw heads.
   const double footerTop=total-L::footer;
   const auto panel=L::footerPanel;
   rounded(c,{panel.x0,footerTop+panel.y0,panel.x1,footerTop+panel.y1},
           {36,49,63,255},3.);
   satin(c,{panel.x0+2.,footerTop+panel.y0+2.,
            panel.x1-2.,footerTop+panel.y1-2.},
         {44,56,68,255},{30,42,55,255},{18,30,44,255},8);
   border(c,{panel.x0,footerTop+panel.y0,panel.x1,footerTop+panel.y1},
          {79,101,122,255});
   char summary[100]{};
   std::snprintf(summary,sizeof(summary),
                 "ROWS %zu-%zu / %zu  |  PATTERN %c",
                 ui.firstRow+1,ui.firstRow+shown,scene.visibleRows,
                 static_cast<char>('A'+scene.pattern));
   cap(c,L::footerAdd,footerTop,"ADD ROW",false,10,vintageButtons);
   cap(c,L::footerRemove,footerTop,"REMOVE ROW",false,10,vintageButtons);
   label(c,summary,{260.,footerTop+9.,510.,footerTop+33.},11,
         {222,213,197,255},false);
   if(!scene.status.empty())
     label(c,scene.status.c_str(),{520.,footerTop+9.,900.,footerTop+33.},
           10,{220,203,180,255},false);
   // Both controls are identically sized 95x25 machined metal keys.
   cap(c,L::footerPrev,footerTop,"PREV PAGE",false,10,vintageButtons,ui.firstRow==0);
   cap(c,L::footerNext,footerTop,"NEXT PAGE",false,10,vintageButtons,ui.firstRow+shown>=scene.visibleRows);
   // Preserve clearly exposed corner screws in both the image-backed and
   // fallback skins. Draw AFTER the full-width inset, never underneath it.
   screw(c,L::footerScrewLeftX,total-15.);
   screw(c,L::footerScrewRightX,total-15.);
   if(zoomMenuOpen)zoomPopup(c,selected,vintageButtons);
 }
};
}
